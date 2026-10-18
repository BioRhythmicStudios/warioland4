#include "port/x86_access.h"

unsigned int PortX86AccessSize(const uint8_t *code)
{
    const uint8_t *p = code;
    int operand16 = 0, repe = 0, repne = 0, rexW = 0;
    unsigned int full;
    uint8_t op;

    // Legacy prefixes: operand size, REP/REPNE (also SSE's mandatory prefixes), address size,
    // LOCK and segments.
    for (;; p++) {
        if (*p == 0x66)
            operand16 = 1;
        else if (*p == 0xF3)
            repe = 1;
        else if (*p == 0xF2)
            repne = 1;
        else if (*p != 0x67 && *p != 0xF0 && *p != 0x2E && *p != 0x36 && *p != 0x3E && *p != 0x26
                 && *p != 0x64 && *p != 0x65)
            break;
    }
    if ((*p & 0xF0) == 0x40)  // REX
        rexW = (*p++ >> 3) & 1;
    full = rexW ? 8 : operand16 ? 2 : 4;
    op = *p;

    if (op == 0x0F) {
        op = p[1];
        if ((op & 0xF0) == 0x90)  // SETcc
            return 1;
        switch (op) {
        case 0x11:  // MOVUPS/MOVUPD, MOVSS, MOVSD
            return repe ? 4 : repne ? 8 : 16;
        case 0x29:  // MOVAPS/MOVAPD
        case 0x2B:  // MOVNTPS
        case 0xE7:  // MOVNTDQ
            return 16;
        case 0x7F:  // MOVDQA/MOVDQU, or MMX MOVQ
            return operand16 || repe ? 16 : 8;
        case 0x7E:  // MOVD/MOVQ to memory
            return rexW ? 8 : 4;
        case 0xD6:  // MOVQ
        case 0x13:  // MOVLPS
        case 0x17:  // MOVHPS
            return 8;
        case 0xB0:  // CMPXCHG, XADD
        case 0xC0:
            return 1;
        case 0xB1:
        case 0xC1:
        case 0xAB:  // BTS, BTR, BTC
        case 0xB3:
        case 0xBB:
            return full;
        default:
            return 0;
        }
    }
    // ADD, OR, ADC, SBB, AND, SUB, XOR, CMP with a memory destination.
    if (op < 0x40 && (op & 7) <= 1)
        return op & 1 ? full : 1;
    switch (op) {
    case 0x80:  // ALU with an immediate
    case 0x86:  // XCHG
    case 0x88:  // MOV
    case 0xC0:  // shifts
    case 0xC6:  // MOV with an immediate
    case 0xD0:
    case 0xD2:
    case 0xF6:  // NOT, NEG
    case 0xFE:  // INC, DEC
    case 0xA4:  // MOVS, STOS
    case 0xAA:
        return 1;
    case 0x81:
    case 0x83:
    case 0x87:
    case 0x89:
    case 0xC1:
    case 0xC7:
    case 0xD1:
    case 0xD3:
    case 0xF7:
    case 0xFF:
    case 0xA5:
    case 0xAB:
        return full;
    default:
        return 0;
    }
}

int PortX86AddressRegister(const uint8_t *code, unsigned int *scale, int *reloads)
{
    const uint8_t *p = code;
    unsigned int rex = 0, modrm, mod, reg, rm, extension, address;
    int load = 0, usesReg = 0;
    uint8_t op;

    while (*p == 0x66 || *p == 0x67 || *p == 0xF2 || *p == 0xF3 || *p == 0xF0 || *p == 0x2E || *p == 0x36
           || *p == 0x3E || *p == 0x26 || *p == 0x64 || *p == 0x65)
        p++;
    if ((*p & 0xF0) == 0x40)
        rex = *p++ & 0xF;
    op = *p++;

    // The instructions this works for, and whether their ModRM reg field is a register.
    if (op == 0x0F) {
        op = *p++;
        if (op == 0xB6 || op == 0xB7 || op == 0xBE || op == 0xBF)  // MOVZX, MOVSX
            load = 1;
        else if (op == 0xAF)  // IMUL
            usesReg = 1;
        else
            return -1;
    } else if (op == 0x8A || op == 0x8B || op == 0x63) {  // MOV, MOVSXD
        load = 1;
    } else if ((op < 0x40 && (op & 7) <= 3) || op == 0x84 || op == 0x85 || op == 0x88 || op == 0x89) {
        usesReg = 1;  // ADD ... CMP, TEST, MOV to memory
    } else if (op != 0x80 && op != 0x81 && op != 0x83 && op != 0xC6 && op != 0xC7 && op != 0xF6 && op != 0xF7
               && op != 0xFE && op != 0xFF) {
        return -1;
    }

    modrm = *p++;
    mod = modrm >> 6;
    reg = ((modrm >> 3) & 7) | (rex & 4) << 1;
    extension = (modrm >> 3) & 7;
    rm = modrm & 7;
    // Of the groups, only TEST, INC and DEC with an immediate or none: the others (MUL, DIV,
    // CALL, PUSH) use registers or memory of their own.
    if ((op == 0xF6 || op == 0xF7) && extension != 0)
        return -1;
    if ((op == 0xFE || op == 0xFF) && extension > 1)
        return -1;
    if (mod == 3)
        return -1;
    if (rm == 4) {
        unsigned int sib = *p, index = ((sib >> 3) & 7) | (rex & 2) << 2, base = (sib & 7) | (rex & 1) << 3;
        int hasIndex = index != 4, hasBase = !((sib & 7) == 5 && mod == 0);

        if (hasBase && hasIndex && index == base)
            return -1;
        if (hasBase) {
            *scale = 1;
            address = base;
        } else if (hasIndex) {
            *scale = 1u << (sib >> 6);
            address = index;
        } else {
            return -1;  // an absolute address
        }
    } else if (rm == 5 && mod == 0) {
        return -1;  // RIP-relative
    } else {
        *scale = 1;
        address = rm | (rex & 1) << 3;
    }
    if (usesReg && reg == address)
        return -1;
    *reloads = load && reg == address;
    return (int)address;
}

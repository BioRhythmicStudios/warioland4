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

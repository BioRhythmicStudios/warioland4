#include "sram.h"

typedef void* (*SramOperationFunc)(u8*, u8*, u32);

#ifndef PORTABLE
const char sSramVersion[] = "SRAM_V112";
#else
extern const char sSramVersion[];
#endif

void SramWriteUncheckedInternal(u8* src, u8* dest, u32 size)
{
    while (size-- != 0) {
        *dest++ = *src++;
    }
}

void SramWriteUnchecked(u8* src, u8* dest, u32 size)
{
    u16 code[0x40];
    u16* code_ptr;
    u16* func_ptr;
    u16 csize;
    SramOperationFunc func;

    REG_WAITCNT = (REG_WAITCNT & ~WAITCNT_SRAM_MASK) | WAITCNT_SRAM_8;

#ifdef PORTABLE
    // The GBA runs a copy of the function from RAM, because code running from ROM cannot read
    // SRAM. Native code calls it directly.
    SramWriteUncheckedInternal(src, dest, size);
    return;
#endif
    func_ptr = (u16*)SramWriteUncheckedInternal;
    func_ptr = (u16*)((u32)func_ptr ^ 1);
    code_ptr = code;

    for (csize = (u16*)SramWriteUnchecked - (u16*)SramWriteUncheckedInternal; csize > 0; csize--) {
        *code_ptr++ = *func_ptr++;
    }

    func = (SramOperationFunc)code + 1;
    func(src, dest, size);
}

void SramWrite(u8* src, u8* dest, u32 size)
{
    REG_WAITCNT = (REG_WAITCNT & ~WAITCNT_SRAM_MASK) | WAITCNT_SRAM_8;

    while (size-- != 0) {
        *dest++ = *src++;
    }
}

u8* SramCheckInternal(u8* src, u8* dest, u32 size)
{
    while (size-- != 0) {
        if (*dest++ != *src++) {
            return dest - 1;
        }
    }

    return NULL;
}

u8* SramCheck(u8* src, u8* dest, u32 size)
{
    u16 code[0x60];
    u16* code_ptr;
    u16* func_ptr;
    u16 csize;
    SramOperationFunc func;

    REG_WAITCNT = (REG_WAITCNT & ~WAITCNT_SRAM_MASK) | WAITCNT_SRAM_8;

#ifdef PORTABLE
    return SramCheckInternal(src, dest, size);
#endif
    func_ptr = (u16*)SramCheckInternal;
    func_ptr = (u16*)((u32)func_ptr ^ 1);
    code_ptr = code;

    for (csize = (u16*)SramCheck - (u16*)SramCheckInternal; csize > 0; csize--) {
        *code_ptr++ = *func_ptr++;
    }

    func = (SramOperationFunc)code + 1;
    return func(src, dest, size);
}

u8* SramWriteChecked(u8* src, u8* dest, u32 size)
{
    u8* diff;
    u8 i;

    for (i = 0; i < 3; i++) {
        SramWrite(src, dest, size);
        diff = SramCheck(src, dest, size);
        if (!diff) {
            break;
        }
    }

    return diff;
}

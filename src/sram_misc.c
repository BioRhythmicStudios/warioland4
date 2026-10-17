#include "save_file.h"
#include "sram.h"

#ifndef PORTABLE
u8* const sUnk_80953EC[SAVE_COUNT] = {(u8*)0xE000100, (u8*)0xE000500};
u8* const sUnk_80953F4[SAVE_COUNT] = {(u8*)0xE000300, (u8*)0xE000700};
u8* const sUnk_80953FC[SAVE_COUNT] = {(u8*)0xE0002E0, (u8*)0xE0006E0};
u8* const sUnk_8095404[SAVE_COUNT] = {(u8*)0xE001000, (u8*)0xE004800};
#else
extern GBA_PTR(u8*) const sUnk_80953EC[SAVE_COUNT];
extern GBA_PTR(u8*) const sUnk_80953F4[SAVE_COUNT];
extern GBA_PTR(u8*) const sUnk_80953FC[SAVE_COUNT];
extern GBA_PTR(u8*) const sUnk_8095404[SAVE_COUNT];
#endif

void func_8000CA8(void)
{
    DmaFill16(3, 0, (u8*)0x02038100, 0x7F00);
    SramWriteChecked((u8*)0x02038100, (u8*)0x0E000100, 0x7F00U);
}

void func_8000CE0(void)
{
    DmaFill16(3, 0, (u8*)0x02038000, 0x8000);
    SramWriteChecked((u8*)0x02038000, (u8*)0x0E000000, 0x8000U);
}

void func_8000D18(void)
{
    DmaFill16(3, 0, 0x02039000, 0x10);
    SramWriteChecked((u8*)0x02039000, GP(sUnk_8095404[gSelectedSaveFile]), 0x10U);
    DmaFill16(3, 0, 0x02039A80, 0x10);
    SramWriteChecked((u8*)0x02039A80, GP(sUnk_8095404[gSelectedSaveFile]) + 0xA80, 0x10U);
}

void func_8000D98(u8 file)
{
    {
        u8* src = GP(sPrimarySaveHeaderBuffers[file]);
        u8* dest = GP(sUnk_80953EC[file]);
        SramWriteChecked(src, dest, 0x10U);
        SramWriteChecked(src + 0x160, dest + 0x160, 0x10U);
    }
    {
        u8* src = GP(sSecondarySaveHeaderBuffers[file]);
        u8* dest = GP(sUnk_80953F4[file]);
        SramWriteChecked(src, dest, 0x10U);
        SramWriteChecked(src + 0x160, dest + 0x160, 0x10U);
    }
    {
        u8* src = GP(sMainSaveDataBuffers[file]);
        u8* dest = GP(sUnk_8095404[file]);
        SramWriteChecked(src, dest, 0x10U);
        SramWriteChecked(src + 0xA80, dest + 0xA80, 0x10U);
    }
}

void func_8000E3C(void)
{
    SramWriteChecked((u8*)0x02038000, (u8*)0x0E000000, 0x8000U);
}

void func_8000E54(void)
{
    SramWriteChecked((u8*)0x02038000, (u8*)0x0E000000, 0x40U);
    DmaCopy16(3, 0x02038000, 0x02038900, 0x40);
    SramWriteChecked((u8*)0x02038900, (u8*)0x0E000900, 0x40U);
}

void func_8000E94(void)
{
    SramWriteChecked((u8*)0x02038040, (u8*)0x0E000040, 0x40U);
}

void func_8000EAC(void)
{
    SramWriteChecked((u8*)0x02038900, (u8*)0x0E000900, 0x40U);
}

void func_8000EC4(void)
{
    SramWriteChecked((u8*)0x02039000, GP(sUnk_8095404[gSelectedSaveFile]), 0x3800U);
}

void func_8000EEC(void)
{
    SramWriteChecked((u8*)0x02038100, GP(sUnk_80953EC[gSelectedSaveFile]), 0x200U);
}

void func_8000F14(void)
{
    SramWriteChecked((u8*)0x02038300, GP(sUnk_80953F4[gSelectedSaveFile]), 0x200U);
}

void func_8000F3C(void)
{
    SramWriteChecked((u8*)0x020382E0, GP(sUnk_80953FC[gSelectedSaveFile]), 0x20U);
}

void func_8000F64(void)
{
    SramWriteChecked(GP(sPrimarySaveHeaderBuffers[gSelectedSaveFile]), GP(sUnk_80953EC[gSelectedSaveFile]), 0x200U);
}

void func_8000F90(void)
{
    SramWriteChecked(GP(sSecondarySaveHeaderBuffers[gSelectedSaveFile]), GP(sUnk_80953F4[gSelectedSaveFile]), 0x200U);
}

void func_8000FBC(void)
{
    SramWriteChecked(GP(sMainSaveDataBuffers[gSelectedSaveFile]), GP(sUnk_8095404[gSelectedSaveFile]), 0x3800U);
}

void func_8000FE8(void)
{
    SramWriteChecked((u8*)0x02038080, (u8*)0x0E000080, 0x20U);
}

void func_8001000(void)
{
    SramWriteChecked((u8*)0x02038C00, (u8*)0x0E000C00, 0x400U);
}

void func_800101C(void)
{
    SramWriteUnchecked((u8*)0x0E000080, (u8*)0x02038080, 0x7F80U);
}

void func_8001038(void)
{
    SramWriteUnchecked((u8*)0x0E000000, (u8*)0x02038000, 0x80U);
    SramWriteUnchecked((u8*)0x0E000900, (u8*)0x02038900, 0x40U);
}

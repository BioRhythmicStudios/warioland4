#include "save_data_clear.h"
#include "background_registers.h"
#include "game_screen_helpers.h"
#include "gba/syscall.h"
#include "global_data.h"
#include "init_helpers.h"
#include "interrupt_callbacks.h"

extern struct SaveDataClearData gUnk_3004A70;
extern u8 gUnk_3004AB8;
extern u8 gUnk_3004AB9;
extern u8 sUnk_8746D70[];
extern u8 sUnk_8746D72[];
extern u8 sUnk_8746D74[];
extern u8 sUnk_8746D76[];
extern u8 sUnk_8746D78[];
extern s8 gStageEntryPauseMenuDisabled;
extern u8 sUnk_8745350[];
extern u8 sUnk_8745B58[];
extern u8 sUnk_8746480[];
extern u8 sUnk_8746680[];
extern u8 sUnk_8746880[];
extern u8 sUnk_8746926[];
extern GBA_PTR(const u8*) const sUnk_878F93C[];

extern void func_8092DA0(void);
extern void func_8092DEC(void);

s32 SaveResetSubroutine(void)
{
    s32 result;
    s32 saveResult;
    u8* saveResultPtr;

    result = 0;
    gUnk_300188E++;

    switch (gSubGameMode) {
        case 0:
            func_8092B30();
            gSubGameMode++;
            break;

        case 1:
            if (gBldy != 0) {
                gBldy--;
            } else {
                m4aSongNumStart(0x11E);
                gSubGameMode++;
                gUnk_300188E = 0;
            }
            break;

        case 2:
            saveResultPtr = &gStageEntrySoftResetDisabled;
            saveResult = func_80928E4();
            *saveResultPtr = saveResult;
            if ((s8)saveResult != 0) {
                gSubGameMode++;
                gUnk_300188E = 0;
                if ((s8)saveResult != 1) {
                    *(vu16*)0x04000050 = 0xFF;
                }
            }
            break;

        case 3:
            if (gUnk_300188E > 0x28) {
                gSubGameMode++;
            }
            break;

        case 4:
            if (gBldy <= 0xF) {
                gBldy++;
            } else {
                result = 1;
            }
            break;
    }

    gOamSlotsUsed = 0;
    func_8092DFC();
    ResetFreeOam();
    return result;
}

s32 func_80928E4(void)
{
    s32 var_r4;

    var_r4 = 0;
    if (0x80 & gUnk_3004AB9) {
        func_8092914();
    } else {
        var_r4 = func_80929C4();
    }
    func_8092D2C();
    return var_r4;
}

void func_8092914(void)
{
    u8 mode;
    u8 value;
    u8* table;

    if (gUnk_3004A70.unk_0A != 2) {
        value = gUnk_3004AB9 & 0x7F;
        gUnk_3004AB9 = value;
        gUnk_3004AB8 = value;
        mode = gUnk_3004AB9;

        switch (mode) {
            case 0:
                gUnk_3004A70.unk_22 = sUnk_8746D70[gLanguage];
                gUnk_3004A70.unk_16 = mode;
                break;

            case 1:
                gUnk_3004A70.unk_22 = sUnk_8746D72[gLanguage];
                gUnk_3004A70.unk_16 = 0;
                break;

            case 2:
                table = sUnk_8746D74;
                gUnk_3004A70.unk_16 = table[gLanguage];
                break;

            case 3:
                table = sUnk_8746D76;
                gUnk_3004A70.unk_16 = table[gLanguage];
                break;
        }

        gUnk_3004A70.unk_14 = 0;
        gUnk_3004A70.unk_15 = 0;
        gUnk_3004A70.unk_20 = 0;
        gUnk_3004A70.unk_21 = 0;
        gUnk_3004A70.unk_0A = 1;
        gUnk_3004A70.unk_08 = 0;
        gUnk_3004A70.unk_09 = 0;
    }
}

s32 func_80929C4(void)
{
    s32 result;
    s32 action;
    char new_var;
    u16 buttons;
    result = 0;
    action = 0;
    switch (gUnk_3004AB8) {
        case 0:
            if ((gButtonsPressed & 0x10) != 0) {
                gUnk_3004AB9 = 0x81;
                action = 2;
            } else if ((gButtonsPressed & 1) != 0) {
                result = 2;
                action = 3;
            }
            break;

        case 1:
            buttons = gButtonsPressed;
            if ((buttons & 0x20) != 0) {
                gUnk_3004AB9 = 0x80;
                action = 2;
            } else {
                new_var = buttons;
                if ((new_var & gUnk_3004AB8) != 0) {
                    gUnk_3004AB9 = 0x82;
                    action = 6;
                }
            }
            break;

        case 2:
            if ((gButtonsPressed & 0x10) != 0) {
                gUnk_3004AB9 = 0x83;
                action = 2;
            } else if ((gButtonsPressed & 3) != 0) {
                gUnk_3004AB9 = 0x80;
                action = 5;
            }
            break;

        case 3:
            if ((gButtonsPressed & 0x20) != 0) {
                gUnk_3004AB9 = 0x82;
                action = 2;
            } else if ((gButtonsPressed & 1) != 0) {
                result = 1;
                action = 4;
            } else if ((gButtonsPressed & 2) != 0) {
                gUnk_3004AB9 = 0x80;
                action = 5;
            }
            break;
    }

    if (action == 1) {
        gUnk_3004A70.unk_0A = 2;
        gUnk_3004A70.unk_08 = 0;
        gUnk_3004A70.unk_09 = 0;
        m4aSongNumStart(0x125);
    } else if (action == 2) {
        m4aSongNumStart(0x124);
    } else if (action == 3) {
        gUnk_3004A70.unk_0A = action;
        gUnk_3004A70.unk_08 = 0;
        gUnk_3004A70.unk_09 = 0;
        m4aSongNumStart(0x129);
    } else if (action == 4) {
        gUnk_3004A70.unk_0A = 2;
        gUnk_3004A70.unk_08 = 0;
        gUnk_3004A70.unk_09 = 0;
        m4aSongNumStart(0x126);
    } else if (action == 5) {
        m4aSongNumStart(0x129);
    } else if (action == 6) {
        m4aSongNumStart(0x125);
    }
    return result;
}

void func_8092B30(void)
{
    volatile u16* ime;
    char new_var7;
    const u8* new_var2;
    int new_var6;
    u8* new_var;
    int new_var3;
    int new_var4;
    int zero;
    volatile u16* reg;

    ime = (volatile u16*)0x04000208;
    *ime = 0;
    zero = 0x80000000;
    new_var7 = zero;
    reg = (volatile u16*)0x04000004;
    *reg &= 0xFFEF;
    new_var4 = 0x04000200;
    reg = (volatile u16*)new_var4;
    *reg &= 0xFFFD;
    *ime = 1;
    InterruptCallbackSetVBlank(&func_8092DEC);
    *((volatile u16*)0x04000050) = 0xBF;
    gBldy = 0x10;
    *((volatile u16*)0x04000054) = 0x10;
    new_var2 = (const u8*)0x05000200;
    InitializeVideoMemory();
    gOamSlotsUsed = zero;
    new_var6 = 0x0201F040;
    ResetFreeOam();
    {
        volatile u16* vcount;
        vcount = (volatile u16*)0x04000006;
        while (((u16)((*vcount) - 0x15)) <= 0x8B) {
        }
    }
    {
        volatile u32* dma;
        const u8* source;
        u32 control;
        dma = (volatile u32*)0x040000D4;
        source = sUnk_8746480;
        dma[0] = (u32)source;
        source = (const u8*)0x05000000;
        dma[1] = (u32)source;
        control = 0x80000100;
        dma[2] = control;
        dma[2];
        source = sUnk_8746680;
        dma[0] = (u32)source;
        source = new_var2;
        dma[1] = (u32)source;
        dma[2] = control;
        dma[2];
    }
    new_var3 = 0x0600D000;
    LZ77UnCompVram((const u32*)sUnk_8745350, (void*)0x06000000);
    LZ77UnCompVram((const u32*)sUnk_8745B58, (void*)0x06010000);
    {
        const u8* source;
        s32 size;
        if (gLanguage == 0) {
            source = sUnk_8746880;
        } else {
            new_var = sUnk_8746926;
            source = new_var;
        }
        size = 0x0201F040;
        source = (const u8*)DecompressRoomBackground(0, source, (u8*)size);
        {
            volatile u32* dma;
            s32 word;
            u32 enable;
            dma = (volatile u32*)0x040000D4;
            word = new_var6;
            dma[0] = word;
            word = new_var3;
            dma[1] = word;
            word = (s32)source + (((u32)source) >> 31);
            word >>= 1;
            enable = 0x80 << 0x18;
            word |= enable;
            dma[2] = word;
            dma[2];
        }
    }
    {
        register int clear PIN("r4");
        volatile u16* bgReg;
        bgReg = (volatile u16*)0x0400000E;
        clear = 0;
        *bgReg = clear;
        bgReg--;
        *bgReg = clear;
        *((volatile u16*)0x0400000A) = 0x3A01;
        *((volatile u16*)0x04000008) = clear;
        gBlendAlphaEvb = clear;
        *((volatile u16*)0x04000052) = clear;
        gStageEntryPauseMenuDisabled = clear;
        gBg1XPosition = 4;
        gBg1YPosition = clear;
        func_8092CC4();
        gOamSlotsUsed = clear;
    }
    func_8092DFC();
    *((volatile u16*)0x04000000) = 0x1200;
    InterruptCallbackSetVBlank(&func_8092DA0);
}

void func_8092CC4(void)
{
    gUnk_3004AB8 = 0;
    gUnk_3004AB9 = 0x80;
    gUnk_3004A70.unk_22 = (u8)sUnk_8746D70[gLanguage];
    gUnk_3004A70.unk_20 = 0;
    gUnk_3004A70.unk_20 = 0;
    gUnk_3004A70.unk_21 = 0;
    gUnk_3004A70.unk_18 = 0x66;
    gUnk_3004A70.unk_1A = 0x78;
    gUnk_3004A70.unk_0A = 1;
    gUnk_3004A70.unk_08 = 0;
    gUnk_3004A70.unk_09 = 0;
    gUnk_3004A70.unk_16 = 0;
    gUnk_3004A70.unk_14 = 0;
    gUnk_3004A70.unk_15 = 0;
    gUnk_3004A70.unk_0C = 0x5C;
    gUnk_3004A70.unk_0E = 0x90;
    func_8092D2C();
}

void func_8092D2C(void)
{
    register struct SaveDataClearData* data PIN("r2");
    u8* tbl;
    u8* ptr;
    s32 value;

    if (gUnk_3004AB8 <= 1) {
        data = &gUnk_3004A70;
        tbl = sUnk_8746D78;
        value = tbl[gUnk_3004AB8 * 2];
        value <<= 0x18;
        value >>= 0x18;
        value += data->unk_18;
        *(u16*)&data->pad_00[0] = value;

        ptr = (u8*)(gUnk_3004AB8 * 2);
        tbl++;
        ptr += (u32)tbl;
        value = *ptr;
        value <<= 0x18;
        value >>= 0x18;
        value += data->unk_1A;
    } else {
        data = &gUnk_3004A70;
        tbl = sUnk_8746D78;
        value = tbl[gUnk_3004AB8 * 2];
        value <<= 0x18;
        value >>= 0x18;
        value += data->unk_0C;
        *(u16*)&data->pad_00[0] = value;

        ptr = (u8*)(gUnk_3004AB8 * 2);
        tbl++;
        ptr += (u32)tbl;
        value = *ptr;
        value <<= 0x18;
        value >>= 0x18;
        value += data->unk_0E;
    }

    *(u16*)&data->pad_00[2] = value;
}

void func_8092DA0(void)
{
    vu32* dma;

    m4aSoundVSync();
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)&gOamBuffer;
    dma[1] = 0x07000000;
    dma[2] = 0x84000100;
    dma[2];
    *(vu16*)0x04000054 = gBldy;
    *(vu16*)0x04000014 = gBg1XPosition;
    *(vu16*)0x04000016 = gBg1YPosition;
}

void func_8092DEC(void)
{
    m4aSoundVSync();
    m4aSoundMain();
}

void func_8092DFC(void)
{
    u16* dest;
    u32 slots;
    u32 oldSlots;
    register u32 i PIN("r4");
    register u32 next PIN("r8");
    u32 yoff;
    register s32 xoff PIN("ip");
    register const u8* anim PIN("r5");
    const u16* src;
    u32 idx;
    u32 twice;
    register u32 off PIN("r1");
    register u32 cidx PIN("r0");
    register struct SaveClearAnimationState* firstData PIN("r3");
    GBA_TYPEOF(const u8* const*, &(sUnk_878F93C)[0]) tbl;
    u32 type;
    register u32 switchType PIN("r0");
    u32 xhalf;
    u32 count;
    u8* oam;
    u32 attr;
    u32 attr1;
    register u8* oamBase PIN("r0");

    dest = (u16*)gOamBuffer;
    idx = (u32)&gOamSlotsUsed;
    type = *(u8*)idx;
    dest = (u16*)((u8*)dest + (type << 3));
    slots = type;
    oldSlots = slots;
    i = 0;

loop:
    twice = i << 1;
    off = twice + i;
    off <<= 2;
    firstData = (struct SaveClearAnimationState*)((u8*)&gUnk_3004A70 + off);
    type = firstData->type;
    idx = twice;
    next = i + 1;
    if (type == 0) {
        goto end_iter;
    }
    switchType = type;
    switch (switchType) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12: {
            struct SaveClearAnimationState* data;
            tbl = sUnk_878F93C;
            cidx = idx;
            cidx += i;
            data = (struct SaveClearAnimationState*)((u8*)&gUnk_3004A70 + (cidx << 2));
            anim = GP(tbl[data->type]);
            if (data->timer >= anim[(data->frame << 3) + 4]) {
                data->timer = 0;
                data->frame++;
                if (anim[(data->frame << 3) + 4] == 0) {
                    data->frame = 0;
                }
            }
            break;
        }
        case 2: {
            struct SaveClearAnimationState* data;
            tbl = sUnk_878F93C;
            cidx = idx;
            cidx += i;
            data = (struct SaveClearAnimationState*)((u8*)&gUnk_3004A70 + (cidx << 2));
            anim = GP(tbl[data->type]);
            if (data->timer >= anim[(data->frame << 3) + 4]) {
                data->timer = 0;
                data->frame++;
                if (anim[(data->frame << 3) + 4] == 0) {
                    data->type += 2;
                    anim += 8;
                    data->timer = 0;
                    data->frame = 0;
                }
            }
            break;
        }
    }

    {
        struct SaveClearAnimationState* data2;
        data2 = (struct SaveClearAnimationState*)((u8*)&gUnk_3004A70 + ((idx + i) << 2));
        data2->timer++;
        yoff = (u16)(data2->y - gBg1YPosition);
        {
            u32 xpos;
            u32 bgx;
            bgx = (u32)&gBg1XPosition;
            xpos = data2->x;
            bgx = *(u16*)bgx;
            xpos -= bgx;
            xhalf = (u16)xpos;
        }
        anim += data2->frame << 3;
        src = GP(*(GBA_PTR(const u16*) const *)anim);
        slots += *src;
        src++;
    }
    i++;
    next = i;
    if ((s32)oldSlots >= (s32)slots) {
        goto end_iter;
    }
    oamBase = (u8*)gOamBuffer;
    xoff = (s16)xhalf;
    oam = (u8*)((oldSlots << 3) + (u32)oamBase);
    {
        register u32 tmpCount PIN("r1");
        tmpCount = slots;
        ASM_BARRIER(: "+r"(tmpCount));
        count = tmpCount - oldSlots;
    }
    do {
        attr = *src;
        src++;
        *dest = attr;
        dest++;
        oam[0] = attr + yoff;
        attr1 = *src;
        src++;
        *dest = attr1;
        dest++;
        attr1 += xoff;
        {
            register u32 mask9 PIN("r2");
            mask9 = 0x1FF;
            attr1 &= mask9;
        }
        {
            u32 old2;
            u32 keep;
            old2 = *(u16*)&oam[2];
            keep = 0xFFFFFE00;
            keep &= old2;
            keep |= attr1;
            *(u16*)&oam[2] = keep;
        }
        attr = *src;
        *dest = attr;
        src++;
        {
            s32 mask;
            u32 byte;
            byte = oam[5];
            mask = 13;
            mask = -mask;
            mask &= byte;
            byte = 4;
            mask |= byte;
            oam[5] = mask;
        }
        dest += 2;
        oam += 8;
        count--;
    } while (count != 0);
    oldSlots = slots;

end_iter:
    i = next;
    if ((s32)i > 2) {
        {
            register u32 finalSlots PIN("r0");
            finalSlots = slots;
            anim = (const u8*)&gOamSlotsUsed;
            *(u8*)anim = finalSlots;
        }
        return;
    }
    goto loop;
}

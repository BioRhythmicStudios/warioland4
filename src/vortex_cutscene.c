#include "vortex_cutscene.h"
#include "fixed_point.h"
#include "init_helpers.h"
#include "score.h"
#include "stage_results.h"
#include "stage_select.h"
#include "voice_set.h"

extern u8 gColorFading;
extern u8 gUnk_3003C34;
extern u8 gMainTimer;
extern u8 gUnk_3003D6A;
extern u16 gUnk_3003D68;
extern u16 gUnk_3003D6C;
extern u16 gUnk_3003D6E;
extern u16 gUnk_3003D70;
extern u16 gUnk_3003D72;
extern u16 gStageEntryMainSpriteState[];
extern u16 gStageEntryHorizontalScroll[];
extern u16 gUnk_3003D66;
extern u16 gUnk_3003D78[];
extern u16 gUnk_3003D8C;
extern u16 gUnk_3003D8E;
extern u16 gUnk_3003D90;
extern u16 gUnk_3003D92;
extern const u8 sUnk_867D8DC[];
extern const u8 sStageEjectionObjPalette[];
extern const u8 sStageEjectionObjTiles[];
extern const u8 sStageEjectionPassagePalettes[];
extern const u8 sStageEjectionPassageExtraPalettes[];
extern const u8 sUnk_867DC9C[];
extern const u8 sUnk_8681C9C[];
extern const u8 sUnk_868669C[];
extern const u8 sUnk_868681C[];
extern const u8 sUnk_868901C[];
extern const u8 sUnk_868981C[];
extern const u8 sUnk_868A01C[];
extern const u16 sStageEjectionWarioOam[];
extern const u16 sUnk_863D9EC[];
extern const u16 sUnk_863D9F4[];
extern GBA_PTR(const u16*) const sUnk_863A47C[];
extern const struct AnimationFrame sUnk_863DB7C[];
extern const struct AnimationFrame sUnk_863DBE4[];
extern const struct AnimationFrame sUnk_863DCB4[];
extern const struct AnimationFrame sUnk_863DCCC[];
extern const struct AnimationFrame sUnk_863DCDC[];
extern const struct AnimationFrame sUnk_863DCEC[];

u32 UpdateVortexCutscene(void)
{
    if (UpdateVortexCutsceneMainSprite() != 0) {
        if (gUnk_3003C34 == 0) {
            gColorFading = 4;
            *(vu16*)REG_ADDR_BLDCNT = 0x3FFF;
            return 1;
        }
        gStageEntrySequenceStep++;
        return 1;
    }
    UpdateVortexCutsceneAffine();
    UpdateVortexCutscenePaletteCycle();
    return 0;
}

void InitializeVortexCutsceneResources(void)
{
    volatile u32* dma;
    u32 status;
    u32 mask;
    u16 zero;

    InitializeVideoMemory();
    InitMinigameScoreDisplay();

    dma = (volatile u32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_867D8DC;
    dma[1] = 0x05000000;
    dma[2] = 0x80000020;
    status = dma[2];
    status = dma[2];
    mask = 0x80000000;
    if ((s32)status < 0) {
        do {
            status = dma[2] & mask;
        } while (status != 0);
    }

    dma = (volatile u32*)REG_ADDR_DMA3;
    dma[0] = (u32)sStageEjectionObjPalette;
    dma[1] = 0x05000200;
    dma[2] = 0x80000100;
    status = dma[2];
    status = dma[2];
    mask = 0x80000000;
    if ((s32)status < 0) {
        do {
            status = dma[2] & mask;
        } while (status != 0);
    }

    dma = (volatile u32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_867DC9C;
    dma[1] = 0x06000000;
    dma[2] = 0x80002000;
    status = dma[2];
    status = dma[2];
    mask = 0x80000000;
    if ((s32)status < 0) {
        do {
            status = dma[2] & mask;
        } while (status != 0);
    }

    dma = (volatile u32*)REG_ADDR_DMA3;
    dma[0] = (u32)sStageEjectionObjTiles;
    dma[1] = 0x06010000;
    dma[2] = 0x80002000;
    status = dma[2];
    status = dma[2];
    mask = 0x80000000;
    if ((s32)status < 0) {
        do {
            status = dma[2] & mask;
        } while (status != 0);
    }

    dma = (volatile u32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_8681C9C;
    dma[1] = 0x0600E000;
    dma[2] = 0x80000400;
    dma[2];

    REG_BG2CNT = 0x5C82;
    gUnk_3003D68 = 0;
    gUnk_3003D6A = 0;
    zero = 0;
    gStageEntryHorizontalScroll[0] = zero;
    gStageEntryHorizontalScroll[1] = zero;
    gUnk_3003D66 = zero;
    gStageEntrySequenceStep = zero;
    gStageResultsTimer = zero;
    REG_DISPCNT = 0x1401;
    REG_BG0VOFS = zero;
    REG_BG0HOFS = zero;
    REG_BG1VOFS = zero;
    REG_BG1HOFS = zero;
    REG_BG2VOFS = zero;
    REG_BG2HOFS = zero;
    UpdateVortexCutsceneAffine();
    m4aSongNumStart(0x1CB);
    VoiceSetPlay(6);
}

void InitializeVortexCutsceneFromPortal(void)
{
    s32 value;
    u16* sprite;
    const s16* sincos;
    u32 angle;
    u16 radius;
    u16 zero;

    InitializeVortexCutsceneResources();
    gUnk_3003D6C = 0xE00;
    gUnk_3003D70 = (angle = 0xE0);
    gUnk_3003D6E = 0x640;
    gUnk_3003D72 = (radius = 100);
    sprite = gStageEntryMainSpriteState;
    zero = 0;
    sprite[0] = zero;
    sprite[1] = zero;
    sincos = sSinCosTable;
    value = sincos[(angle + 0x40) & 0xFFFF];
    value = (value * radius) >> 8;
    sprite[2] = value + 124;
    value = sincos[angle];
    value = (value * radius) >> 8;
    sprite[3] = value + 78;
    sprite[6] = zero;
    value = 0x4000;
    sprite[7] = value;
    sprite[8] = value;
}

void InitializeVortexCutsceneFromPassage(void)
{
    struct AGB_ALIGN VortexSpriteStateCopy {
        u32 words[5];
    };
    u32 control;
    u16* sprite;
    volatile u32* transfer;
    InitializeVortexCutsceneResources();
    if ((gCurrentPassage == 0) || (gCurrentPassage == 5)) {
        gUnk_3003C4A = 1;
    } else {
        gUnk_3003C4A = 0;
    }
    if (gUnk_3003C4A != 0) {
        volatile u32* dma;
        u32 status;
        u32 mask;
        dma = (volatile u32*)(0x4000000 + 0xd4);
        dma[0] = (u32)sUnk_868A01C;
        dma[1] = 0x0600C000;
        dma[2] = 0x80000400;
        status = dma[2];
        status = dma[2];
        mask = 0x80000000;
        if (((s32)status) < 0) {
            do {
                status = dma[2] & mask;
            } while (status != 0);
        }
        transfer = (volatile u32*)(0x4000000 + 0xd4);
        transfer[0] = (u32)sUnk_868981C;
        transfer[1] = 0x0600D000;
        control = 0x80000400;
    } else {
        volatile u32* dma;
        u32 status;
        u32 mask;
        dma = (volatile u32*)(0x4000000 + 0xd4);
        dma[0] = (u32)sUnk_868901C;
        dma[1] = 0x0600C000;
        dma[2] = 0x80000400;
        status = dma[2];
        status = dma[2];
        mask = 0x80000000;
        if (((s32)status) < 0) {
            do {
                status = dma[2] & mask;
            } while (status != 0);
        }
        dma = (volatile u32*)(0x4000000 + 0xd4);
        dma[0] = (u32)sUnk_868981C;
        dma[1] = 0x0600D000;
        dma[2] = 0x80000400;
        status = dma[2];
        status = dma[2];
        mask = 0x80000000;
        if (((s32)status) < 0) {
            do {
                status = dma[2] & mask;
            } while (status != 0);
        }
        transfer = (volatile u32*)(0x4000000 + 0xd4);
        transfer[0] = (u32)(&sStageEjectionPassageExtraPalettes[(gCurrentPassage - 1) << 5]);
        transfer[1] = 0x050003C0;
        control = 0x80000010;
    }
    {
        volatile u32* waitDma;
        u32 status;
        u32 mask;
        transfer[2] = control;
        status = transfer[2];
        waitDma = (volatile u32*)(0x4000000 + 0xd4);
        status = waitDma[2];
        mask = 0x80000000;
        if (((s32)status) < 0) {
            do {
                do {
                    status = waitDma[2] & mask;
                } while (status != 0);
            } while (0);
        }
    }
    {
        volatile u32* dma;
        u32 status;
        u32 mask;
        dma = (volatile u32*)(0x4000000 + 0xd4);
        dma[0] = (u32)(&sStageEjectionPassagePalettes[gCurrentPassage << 5]);
        dma[1] = 0x050003A0;
        dma[2] = 0x80000010;
        status = dma[2];
        status = dma[2];
        mask = 0x80000000;
        if (((s32)status) < 0) {
            do {
                status = dma[2] & mask;
            } while (status != 0);
        }
        dma = (volatile u32*)(0x4000000 + 0xd4);
        dma[0] = (u32)sUnk_868669C;
        dma[1] = 0x05000080;
        dma[2] = 0x800000C0;
        status = dma[2];
        status = dma[2];
        mask = 0x80000000;
        if (((s32)status) < 0) {
            do {
                status = dma[2] & mask;
            } while (status != 0);
        }
        dma = (volatile u32*)(0x4000000 + 0xd4);
        dma[0] = (u32)sUnk_868681C;
        dma[1] = 0x06008000;
        dma[2] = 0x80001400;
        dma[2];
    }
    *((vu16*)(0x4000000 + 0x8)) = 0x1808;
    *((vu16*)(0x4000000 + 0xa)) = 0x1A09;
    {
        register u16 firstZero PIN("r1");
        u16 spriteZero;
        firstZero = 0;
        /* Keep the first zero live in r1 until the sprite-state zero is introduced in r2. */
        ASM_BARRIER(: : "r"(firstZero));
        gUnk_3003D6C = firstZero;
        gUnk_3003D70 = firstZero;
        gUnk_3003D6E = firstZero;
        gUnk_3003D72 = firstZero;
        spriteZero = 0;
        sprite = gStageEntryMainSpriteState;
        sprite[0] = spriteZero;
        sprite[1] = spriteZero;
        sprite[2] = 120;
        sprite[3] = 80;
        sprite[6] = spriteZero;
        sprite[7] = 128;
        sprite[8] = 128;
        gStageResultsNewHighScoreIcon.state = firstZero;
        *((struct VortexSpriteStateCopy*)gUnk_3003D78) = *((struct VortexSpriteStateCopy*)sprite);
        gUnk_3003D8C = spriteZero;
        gUnk_3003D90 = spriteZero;
        gUnk_3003D8E = spriteZero;
        gUnk_3003D92 = spriteZero;
    }
}

u32 UpdateVortexCutsceneMainSprite(void)
{
    u8* modePtr;
    u8 mode;
    u16* timerPtr;

    mode = gUnk_3003C34;
    modePtr = &gUnk_3003C34;
    if (mode == 0) {
        s32 value;
        s32 radius1;
        s32 radius2;
        u16 timer;
        u16* sprite;
        const s16* table;
        u32 angle;
        u32 zero;

        gUnk_3003D6C = (gUnk_3003D6C + 0xFBC) & 0xFFF;
        gUnk_3003D6E -= 17;
        timer = gUnk_3003D66;
        timerPtr = &gUnk_3003D66;
        sprite = gStageEntryMainSpriteState;
        if (timer != 0) {
            sprite[7] -= 159;
            sprite[8] -= 159;
        }
        sprite[6] = (sprite[6] + 2) & 0xFF;
        gUnk_3003D70 = (s16)gUnk_3003D6C >> 4;
        gUnk_3003D72 = (s16)gUnk_3003D6E >> 4;
        zero = 0;
        radius1 = (s16)gUnk_3003D72;
        table = sSinCosTable;
        angle = gUnk_3003D70;
        angle += 0x40;
        value = table[angle + zero];
        value = (value * radius1) >> 8;
        sprite[2] = value + 124;
        zero = 0;
        radius2 = (s16)gUnk_3003D72;
        value = table[gUnk_3003D70 + zero];
        value = (value * radius2) >> 8;
        sprite[3] = value + 78;
    } else {
        s32 value;
        s32 radius1;
        s32 radius2;
        u16 timer;
        u16* sprite;
        const s16* table;
        u32 angle;
        u32 zero;

        gUnk_3003D6C = (gUnk_3003D6C + 68) & 0xFFF;
        gUnk_3003D6E += 17;
        timer = gUnk_3003D66;
        timerPtr = &gUnk_3003D66;
        sprite = gStageEntryMainSpriteState;
        if (timer <= 89) {
            sprite[7] += 159;
            sprite[8] += 159;
        }
        sprite[6] = (sprite[6] + 254) & 0xFF;
        gUnk_3003D70 = (s16)gUnk_3003D6C >> 4;
        gUnk_3003D72 = (s16)gUnk_3003D6E >> 4;
        zero = 0;
        radius1 = (s16)gUnk_3003D72;
        table = sSinCosTable;
        angle = gUnk_3003D70;
        angle += 0x40;
        value = table[angle + zero];
        value = (value * radius1) >> 8;
        sprite[2] = value + 120;
        zero = 0;
        radius2 = (s16)gUnk_3003D72;
        value = table[gUnk_3003D70 + zero];
        value = (value * radius2) >> 8;
        sprite[3] = value + 80;
    }

    if (gCollectedKeyzer == 1 && *modePtr == 1) {
        if (*timerPtr > 29) {
            if (UpdateVortexCutsceneKeyzerSprite() != 0) {
                return 1;
            }
        }
    } else if (*timerPtr > 89) {
        return 1;
    }
    gUnk_3003D66++;
    return 0;
}

u32 UpdateVortexCutsceneKeyzerSprite(void)
{
    s32 value;
    s32 radius1;
    s32 radius2;
    u16 timer;
    u32 angle;
    u16* sprite;
    const s16* table;
    u32 zero;

    gUnk_3003D8C = (gUnk_3003D8C + 68) & 0xFFF;
    gUnk_3003D8E += 17;
    timer = gUnk_3003D66;
    sprite = gUnk_3003D78;
    if (timer <= 119) {
        sprite[7] += 159;
        sprite[8] += 159;
    }
    sprite[6] = (sprite[6] + 254) & 0xFF;
    gUnk_3003D90 = (s16)gUnk_3003D8C >> 4;
    gUnk_3003D92 = (s16)gUnk_3003D8E >> 4;
    zero = 0;
    radius1 = (s16)gUnk_3003D92;
    table = sSinCosTable;
    angle = gUnk_3003D90;
    angle += 0x40;
    value = table[angle + zero];
    value = (value * radius1) >> 8;
    sprite[2] = value + 120;
    zero = 0;
    radius2 = (s16)gUnk_3003D92;
    value = table[gUnk_3003D90 + zero];
    value = (value * radius2) >> 8;
    sprite[3] = value + 80;
    if (gUnk_3003D66 > 119) {
        return 1;
    }
    return 0;
}

void UpdateVortexCutsceneAffine(void)
{
    const s16* table;
    s32* tempAlias;
    s32 value;
    s16 inverse;
    s32* valueAlias;
    s32 temp;
    s16* affine;
    if (gUnk_3003C34 == 0) {
        gUnk_3003D68 = (gUnk_3003D68 + 5) & 0xFF;
    } else {
        gUnk_3003D68 = (gUnk_3003D68 + 251) & 0xFF;
    }
    table = sSinCosTable;
    {
        s32 index;
        index = gUnk_3003D68;
        index += 0x40;
        value = table[index];
    }
    inverse = FixedInverse(0x200);
    value = FixedMul(value, inverse);
    affine = (s16*)gStageEntryHorizontalScroll;
    affine[2] = value;
    value = table[gUnk_3003D68];
    valueAlias = &value;
    inverse = FixedInverse(0x200);
    affine[3] = FixedMul(value, inverse);
    value = (s16)(-table[gUnk_3003D68]);
    inverse = FixedInverse(0x200);
    affine[4] = FixedMul(value, inverse);
    {
        s32 index;
        index = gUnk_3003D68;
        index += 0x40;
        value = table[index];
    }
    inverse = FixedInverse(0x200);
    affine[5] = FixedMul(value, inverse);
    temp = (128 - affine[0]) << 8;
    value = affine[2];
    temp -= ((value << 4) - value) << 3;
    value = affine[3];
    temp -= ((value << 2) + value) << 4;
    /* Keep aliases alive so agbcc preserves the original value lifetimes. */
    tempAlias = &temp;
    *((s32*)(affine + 6)) = *tempAlias;
    temp = (128 - affine[1]) << 8;
    value = affine[4];
    temp -= ((value << 4) - value) << 3;
    value = affine[5];
    temp -= (((*valueAlias) << 2) + value) << 4;
    *((s32*)(affine + 8)) = *tempAlias;
}

void UpdateVortexCutscenePaletteCycle(void)
{
    vu32* dma;

    if (gMainTimer % 5 == 0) {
        gUnk_3003D6A++;
        if (gUnk_3003D6A > 14) {
            gUnk_3003D6A = 0;
        }
        dma = (vu32*)REG_ADDR_DMA3;
        dma[0] = (u32)(sUnk_867D8DC + (gUnk_3003D6A << 6));
        dma[1] = 0x05000000;
        dma[2] = 0x80000020;
        dma[2];
    }
}

void ApplyVortexCutsceneAffineRegisters(void)
{
    volatile u16* finalDestination;
    volatile u16* destination;
    u16* source;
    s32 value;
    u16* ySource;
    s32 upperMask;
    do {
        destination = (volatile u16*)0x04000020;
        /* agbcc otherwise reuses the later mask pseudo-register for the first halfword copy.
         * This zero-byte boundary only splits that lifetime and emits no instruction. */
        AGBCC_SPLIT_REGISTER_LIFETIME(upperMask);
        source = gStageEntryHorizontalScroll;
        *destination = source[2];
        destination++;
        *destination = source[3];
        destination++;
        *destination = source[4];
        destination++;
        *destination = source[5];
        destination++;
        value = *((s32*)(source + 6));
        *destination = value;
        destination++;
        upperMask = 0x0FFF0000;
        value &= upperMask;
    } while (0);
    value >>= 16;
    *destination = value;
    destination++;
    ySource = source;
    value = *((s32*)(ySource + 8));
    *destination = value;
    destination++;
    value &= upperMask;
    value >>= 16;
    finalDestination = destination;
    *finalDestination = value;
}

void RenderVortexCutsceneOam(void)
{
    /* The shared OamData splits attr1 bits 9..13 into a single 5-bit matrixNum.
       This routine only ever selects one of the first few affine matrices, so
       it writes the 3-bit index the hardware reads for that, leaving the two
       flip bits alone. */
    typedef union AGB_ALIGN {
        struct AGB_ALIGN {
            u32 y : 8;
            u32 affineMode : 2;
            u32 objMode : 2;
            u32 mosaic : 1;
            u32 bpp : 1;
            u32 shape : 2;

            u32 x : 9;
            u32 affineIndex : 3;
            u32 preserved : 2;
            u32 size : 2;

            u16 tileNum : 10;
            u16 priority : 2;
            u16 paletteNum : 4;

            u16 fractional : 8;
            u16 integer : 7;
            u16 sign : 1;
        } split;

        struct AGB_ALIGN {
            u16 attr0;
            u16 attr1;
            u16 attr2;
            u16 affineParam;
        } all;
    } VortexOamData;

    const u16* src;
    u16* dest;
    u16* spriteState;
    u16* pbPtr;
    u16* pcPtr;
    u16* pdPtr;
    u16 affine[4];
    u16 attr;
    u16 sine;
    u16 warioPd;
    s32 xOffset;
    s32 keyzerPd;
    s32 treasurePd;
    s32 currentSlot;
    s32 nextSlot;
    s32 i;

    currentSlot = 0;
    nextSlot = gOamSlotsUsed;
    dest = (u16*)&gOamBuffer[nextSlot];

    if ((u16)(gUnk_3003D66 - 1) <= 88) {
        src = sStageEjectionWarioOam;
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }

        spriteState = gStageEntryMainSpriteState;
        pbPtr = &affine[1];
        pcPtr = &affine[2];
        pdPtr = &affine[3];

        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + (u8)gStageEntryMainSpriteState[3] - 32;
            gOamBuffer[currentSlot].split.affineMode = 3;

            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + gStageEntryMainSpriteState[2] - 32) & 0x1FF;
            ((VortexOamData*)gOamBuffer)[currentSlot].split.affineIndex = 0;

            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }

        affine[0] = FixedMul(sSinCosTable[spriteState[6] + 64], (s16)FixedInverse(spriteState[7] >> 4));
        *pbPtr = FixedMul(sSinCosTable[spriteState[6]], (s16)FixedInverse(spriteState[7] >> 4));
        sine = sSinCosTable[spriteState[6]];
        *pcPtr = FixedMul((s16)-sine, (s16)FixedInverse(spriteState[8] >> 4));
        warioPd = FixedMul(sSinCosTable[spriteState[6] + 64], (s16)FixedInverse(spriteState[8] >> 4));
        *pdPtr = warioPd;
        gOamBuffer[0].all.affineParam = affine[0];
        gOamBuffer[1].all.affineParam = *pbPtr;
        gOamBuffer[2].all.affineParam = *pcPtr;
        gOamBuffer[3].all.affineParam = warioPd;
    }

    if (((u16)(gUnk_3003D66 - 30) <= 89) && (gCollectedKeyzer == 1) && (gUnk_3003C34 == 1)) {
        src = sUnk_863D9EC;
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }

        pbPtr = &affine[1];
        pcPtr = &affine[2];
        pdPtr = &affine[3];
        spriteState = gUnk_3003D78;

        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + (u8)gUnk_3003D78[3] - 32;
            gOamBuffer[currentSlot].split.affineMode = 3;

            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + gUnk_3003D78[2] - 32) & 0x1FF;
            ((VortexOamData*)gOamBuffer)[currentSlot].split.affineIndex = 1;

            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }

        affine[0] = FixedMul(sSinCosTable[spriteState[6] + 64], (s16)FixedInverse(spriteState[7] >> 4));
        *pbPtr = FixedMul(sSinCosTable[spriteState[6]], (s16)FixedInverse(spriteState[7] >> 4));
        sine = sSinCosTable[spriteState[6]];
        *pcPtr = FixedMul((s16)-sine, (s16)FixedInverse(spriteState[8] >> 4));
        keyzerPd = FixedMul(sSinCosTable[spriteState[6] + 64], (s16)FixedInverse(spriteState[8] >> 4));
        *pdPtr = keyzerPd;
        gOamBuffer[4].all.affineParam = affine[0];
        gOamBuffer[5].all.affineParam = *pbPtr;
        gOamBuffer[6].all.affineParam = *pcPtr;
        gOamBuffer[7].all.affineParam = keyzerPd;
    }

    if ((gSubGameMode == 18) && (gStageEntrySequenceStep > 1)) {
        if ((gStageEntrySequenceStep == 3) && (gStageResultsAnimationFinished == 0)) {
            src = GP(sUnk_863A47C[gStageResultsTreasure.state]);
            nextSlot += *(src++);
            pbPtr = &affine[1];
            pcPtr = &affine[2];
            pdPtr = &affine[3];

            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + (u8)gStageResultsTreasure.y - 16;
                if (gStageResultsTreasure.state == 4) {
                    gOamBuffer[currentSlot].split.y = attr + (u8)gStageResultsTreasure.y - 32;
                }
                gOamBuffer[currentSlot].split.affineMode = 3;

                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + (s16)gStageResultsTreasure.x - 16) & 0x1FF;
                if (gStageResultsTreasure.state == 4) {
                    gOamBuffer[currentSlot].split.x = ((attr + (s16)gStageResultsTreasure.x - 32) & 0xFF) & 0x1FF;
                }
                ((VortexOamData*)gOamBuffer)[currentSlot].split.affineIndex = 0;

                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }

            affine[0] = FixedMul(sSinCosTable[64], (s16)FixedInverse((s16)gStageResultsTreasure.scale));
            *pbPtr = FixedMul(sSinCosTable[0], (s16)FixedInverse((s16)gStageResultsTreasure.scale));
            sine = sSinCosTable[0];
            *pcPtr = FixedMul((s16)-sine, (s16)FixedInverse((s16)gStageResultsTreasure.scale));
            treasurePd = FixedMul(sSinCosTable[64], (s16)FixedInverse((s16)gStageResultsTreasure.scale));
            *pdPtr = treasurePd;
            gOamBuffer[0].all.affineParam = affine[0];
            gOamBuffer[1].all.affineParam = *pbPtr;
            gOamBuffer[2].all.affineParam = *pcPtr;
            gOamBuffer[3].all.affineParam = treasurePd;
        }

        if (gStageResultsAllJewelPieces == 1) {
            gStageResultsTreasureIcons[0].x += 1;
            if (sUnk_863DB7C[gStageResultsTreasureIcons[0].y].time < gStageResultsTreasureIcons[0].x) {
                gStageResultsTreasureIcons[0].x = 1;
                gStageResultsTreasureIcons[0].y += 1;
                if (sUnk_863DB7C[gStageResultsTreasureIcons[0].y].time == 0) {
                    gStageResultsTreasureIcons[0].y = 0;
                }
            }

            src = GP(sUnk_863DB7C[gStageResultsTreasureIcons[0].y].oam);
            nextSlot += *(src++);
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 112;

                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + sStageResultsTreasureXPositions[gUnk_3003C4A]) & 0x1FF;

                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }

        for (i = 0; i <= 3; i++) {
            if (gStageResultsTreasureIcons[i].state != 0) {
                src = GP(sUnk_863A47C[i]);
                nextSlot += *(src++);
                for (; currentSlot < nextSlot; currentSlot++) {
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.y = attr + 112;

                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.x = (attr + sStageResultsTreasureXPositions[gUnk_3003C4A]) & 0x1FF;

                    *(dest++) = *(src++);
                    gOamBuffer[currentSlot].split.priority = 0;
                    dest++;
                }
            }
        }

        if (gStageResultsTreasureIcons[4].state != 0) {
            gStageResultsTreasureIcons[4].x += 1;
            if (sUnk_863DBE4[gStageResultsTreasureIcons[4].y].time < gStageResultsTreasureIcons[4].x) {
                gStageResultsTreasureIcons[4].x = 1;
                gStageResultsTreasureIcons[4].y += 1;
                if (sUnk_863DBE4[gStageResultsTreasureIcons[4].y].time == 0) {
                    gStageResultsTreasureIcons[4].y = 0;
                }
            }

            src = GP(sUnk_863DBE4[gStageResultsTreasureIcons[4].y].oam);
            nextSlot += *(src++);
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 112;

                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + 144) & 0x1FF;

                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }

        if ((gStageEntrySequenceStep > 4) && (gStageResultsScoreRank != 0)) {
            if (gStageResultsScoreRank == 1) {
                src = GP(sUnk_863DCCC->oam);
            } else if (gStageResultsScoreRank == 2) {
                src = GP(sUnk_863DCDC->oam);
            } else {
                src = GP(sUnk_863DCEC->oam);
            }
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }

            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 64;

                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + 80) & 0x1FF;

                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }

        if (gStageResultsNewHighScoreIcon.state != 0) {
            gStageResultsNewHighScoreIcon.x += 1;
            if (sUnk_863DCB4[gStageResultsNewHighScoreIcon.y].time < gStageResultsNewHighScoreIcon.x) {
                gStageResultsNewHighScoreIcon.x = 1;
                gStageResultsNewHighScoreIcon.y += 1;
                if (sUnk_863DCB4[gStageResultsNewHighScoreIcon.y].time == 0) {
                    gStageResultsNewHighScoreIcon.y = 0;
                }
            }

            src = GP(sUnk_863DCB4[gStageResultsNewHighScoreIcon.y].oam);
            nextSlot += *(src++);
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 56;

                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + 120) & 0x1FF;

                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }

        for (i = 0; i < gStageResultsRemainingHearts; i++) {
            src = sUnk_863D9F4;
            nextSlot += *(src++);
            for (; currentSlot < nextSlot; currentSlot++) {
                xOffset = i * 8 + 88;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 88;

                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + xOffset) & 0x1FF;

                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }

    gOamSlotsUsed = nextSlot;
}

#include "passage_clear.h"
#include "boss_treasure_cutscene.h"
#include "compiler.h"
#include "fixed_point.h"
#include "init_helpers.h"
#include "stage_entry.h"
#include "stage_select.h"
#include "voice_set.h"

extern s8 gUnk_3003C3E;

extern const u8 sPassageClearBgPalette[];
extern const u8 sPassageClearBgTiles[];
extern const u8 sPassageClearObjPalette[];
extern const u8 sPassageClearObjTiles[];
extern const u8 sPassageClearBg0Tilemap[];
extern const u8 sPassageClearBg2Tilemap[];
extern const u8 sPassageClearBg1Tilemap0[];
extern const u16 sPassageClearBannerXOffsets[];
extern const u16 sPassageClearBannerYOffsets[];
extern GBA_PTR(const u16*) const sPassageClearBannerOamFrames[];
extern const u8 sPassageClearBannerFrameIndices[];
extern const s32 sPassageClearBonusItemInitialXOffsets[];
extern const s32 sPassageClearBonusItemInitialYOffsets[];
extern const s32 sPassageClearParticleSpawnX[];
extern const u8 sPassageClearBg1Tilemap1[];
extern const u8 sPassageClearBg1Tilemap2[];
extern const u8 sPassageClearPaletteCycleFrames[];
extern const struct AnimationFrame sPassageClearMainAnimation[];
extern const struct AnimationFrame sPassageClearSecondaryAnimation[];
extern const struct AnimationFrame sPassageClearSecondaryAltAnimation[];
extern const struct AnimationFrame sPassageClearBonusItemAnimation[];
extern const s32 sPassageClearBonusItemXVelocities[];
extern const s32 sPassageClearBonusItemYVelocities[];
extern const s32 sPassageClearParticleAngleSteps[];
extern GBA_PTR(const u16*) const sPassageClearSlowParticleOamFrames[];
extern GBA_PTR(const u16*) const sPassageClearMediumParticleOamFrames[];
extern GBA_PTR(const u16*) const sPassageClearFastParticleOamFrames[];

s32 MinigameRandom(void);

s32 UpdatePassageClearSequence(void)
{
    volatile u32* dma;
    s32 i;
    u16 timer;

    gStageEntrySequenceTimer++;
    if (gStageEntrySequenceTimer > 368) {
        return 1;
    }

    SpawnPassageClearParticles();

    if (gStageEntrySequenceTimer == 44) {
        REG_DISPCNT = 0x1700;
    }

    timer = gStageEntrySequenceTimer;
    if (timer == 56) {
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearBg1Tilemap1;
        dma[1] = 0x0600D000;
        dma[2] = 0x80000400;
        (void)dma[2];
    }

    if (timer == 68) {
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearBg1Tilemap2;
        dma[1] = 0x0600D000;
        dma[2] = 0x80000400;
        (void)dma[2];
    }

    if (gStageEntrySequenceTimer == 80) {
        gPassageClearBlendFadeActive = 1;
    }
    if (gPassageClearBlendFadeActive != 0) {
        UpdatePassageClearBlendFade();
    }

    if (gStageEntrySequenceTimer == 128) {
        gPassageClearPaletteCycleState++;
    }
    if (gPassageClearPaletteCycleState != 0) {
        UpdatePassageClearPaletteCycle();
    }

    if (gStageEntrySequenceTimer == 22) {
        gPassageClearSecondaryAnimationState.state = 1;
    }
    if (gStageEntrySequenceTimer == 45) {
        gPassageClearSecondaryAnimationState.state = 1;
    }
    if (gStageEntrySequenceTimer == 152) {
        gPassageClearMainAnimationState.state = 1;
    }
    if (gStageEntrySequenceTimer == 267) {
        gPassageClearMainAnimationState.state = 2;
    }

    if (gStageEntrySequenceTimer == 252) {
        gPassageClearBonusAnimationActive = 1;
        m4aSongNumStart(0x1B1);
        VoiceSetPlay(2);
        for (i = 0; i <= 5; i++) {
            gPassageClearBonusItemPositions[i].x =
                (gStageEntryMainSpriteState[2] + sPassageClearBonusItemInitialXOffsets[i]) << 4;
            gPassageClearBonusItemPositions[i].y =
                (gStageEntryMainSpriteState[3] + sPassageClearBonusItemInitialYOffsets[i]) << 4;
        }
    }

    if (gStageEntrySequenceTimer <= 316) {
        UpdatePassageClearBanner();
    }
    return 0;
}

void InitializePassageClear(void)
{
    u8* indexA;
    u8* indexB;
    u8* indexC;
    u8 zero8;
    u16 zero16;

    InitializeVideoMemory();

    {
        volatile u32* dma;
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearBgPalette;
        dma[1] = 0x05000000;
        dma[2] = 0x80000030;
        (void)dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
    {
        volatile u32* dma;
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearObjPalette;
        dma[1] = 0x05000200;
        dma[2] = 0x80000100;
        (void)dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
    {
        volatile u32* dma;
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearBgTiles;
        dma[1] = 0x06000000;
        dma[2] = 0x80002A00;
        (void)dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
    {
        volatile u32* dma;
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearObjTiles;
        dma[1] = 0x06010000;
        dma[2] = 0x80002800;
        (void)dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
    {
        volatile u32* dma;
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearBg0Tilemap;
        dma[1] = 0x0600C000;
        dma[2] = 0x80000400;
        (void)dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
    {
        volatile u32* dma;
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearBg1Tilemap0;
        dma[1] = 0x0600D000;
        dma[2] = 0x80000400;
        (void)dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
    {
        volatile u32* dma;
        dma = (volatile u32*)REG_ADDR_DMA3;
        dma[0] = (u32)sPassageClearBg2Tilemap;
        dma[1] = 0x0600E000;
        dma[2] = 0x80000400;
        (void)dma[2];
    }

    REG_BG0CNT = 0x1800;
    REG_BG1CNT = 0x1A01;
    REG_BG2CNT = 0x1C02;
    InitMinigameScoreDisplay();

    {
        volatile u16* bldy;

        bldy = (volatile u16*)REG_ADDR_BLDY;
        zero8 = 0;
        COMPILER_BARRIER(zero8);
        *bldy = zero8;
    }
    REG_DISPCNT = 0x1500;
    REG_BLDCNT = 0x1641;
    gStageTransitionBlendEvb = zero8;
    gStageTransitionBlendEva = 16;
    gUnk_3003C3E = 16;
    {
        volatile u16* blendAlpha;
        u16 alpha;

        blendAlpha = (volatile u16*)REG_ADDR_BLDALPHA;
        alpha = 0x1000;
        zero16 = 0;
        *blendAlpha = alpha;
    }
    gPassageClearBlendFadeActive = zero8;
    gStageEntrySequenceTimer = zero16;
    gStageEntrySequenceStep = zero8;
    {
        volatile u16* reg;
        reg = (volatile u16*)REG_ADDR_BG0VOFS;
        *reg = zero16;
        reg--;
        *reg = zero16;
        reg += 3;
        *reg = zero16;
        reg--;
        *reg = zero16;
        reg += 3;
        *reg = zero16;
        reg--;
        *reg = zero16;
    }
    gPassageClearPaletteCycleState = zero8;
    gStageEntryMainSpriteState[2] = 0x1E5;
    gStageEntryMainSpriteState[3] = 0x8C;
    UpdatePassageClearBanner();
    gPassageClearMainAnimationState.timer = zero16;
    gPassageClearMainAnimationState.frame = zero16;
    gPassageClearMainAnimationState.state = zero8;
    gPassageClearSecondaryAnimationState.timer = zero16;
    gPassageClearSecondaryAnimationState.frame = zero16;
    gPassageClearSecondaryAnimationState.state = zero8;
    gPassageClearBonusAnimationActive = zero8;
    gPassageClearBonusAnimationTimer = zero16;
    gPassageClearBonusAnimationFrame = zero16;

    {
        u8* start;
        u8* current;
        u8 zero;

        start = (u8*)gPassageClearSlowParticles;
        zero = 0;
        current = start;
        current += 32;
        do {
            current[4] = zero;
            current -= 8;
        } while ((s32)current >= (s32)start);
    }
    indexA = &gPassageClearSlowParticleIndex;
    indexB = &gPassageClearMediumParticleIndex;
    indexC = &gPassageClearFastParticleIndex;
    {
        u8* start;
        u8* current;
        u8 zero;

        start = (u8*)gPassageClearMediumParticles;
        zero = 0;
        current = start;
        current += 32;
        do {
            current[4] = zero;
            current -= 8;
        } while ((s32)current >= (s32)start);

        if ((s32)current < (s32)start) {
            u8* thirdStart;
            u8* thirdCurrent;
            u8 thirdZero;

            thirdStart = (u8*)gPassageClearFastParticles;
            thirdZero = 0;
            thirdCurrent = thirdStart;
            thirdCurrent += 32;
            do {
                thirdCurrent[4] = thirdZero;
                thirdCurrent -= 8;
            } while ((s32)thirdCurrent >= (s32)thirdStart);
        }
    }
    *indexA = 0;
    *indexB = 0;
    *indexC = 0;
    m4aSongNumStart(0x1B0);
}

void UpdatePassageClearBanner(void)
{
    u16* state;
    const u16* xOffsets;
    u16* timer;
    const u16* yOffsets;
    GBA_TYPEOF(const u16**, &gPassageClearBannerOam) animation;
    GBA_TYPEOF(const u16* const*, &(sPassageClearBannerOamFrames)[0]) animations;
    const u8* animationIndices;

    state = gStageEntryMainSpriteState;
    xOffsets = sPassageClearBannerXOffsets;
    timer = &gStageEntrySequenceTimer;
    state[2] += xOffsets[*timer];
    yOffsets = sPassageClearBannerYOffsets;
    state[3] += yOffsets[*timer];
    animation = &gPassageClearBannerOam;
    animations = sPassageClearBannerOamFrames;
    animationIndices = sPassageClearBannerFrameIndices;
    GP_SET(*animation, GP(animations[animationIndices[*timer]]));
}

void UpdatePassageClearBlendFade(void)
{
    u16 remainder;

    remainder = gStageEntrySequenceTimer % 3;
    if (remainder == 0) {
        gStageTransitionBlendEvb++;
        gStageTransitionBlendEva--;
        *(volatile u16*)0x04000052 = (gStageTransitionBlendEva << 8) | gStageTransitionBlendEvb;
        if (gStageTransitionBlendEvb == 16) {
            gPassageClearBlendFadeActive = remainder;
        }
    }
}

void UpdatePassageClearPaletteCycle(void)
{
    u8* source;
    volatile u8* state;
    u8 value;
    volatile u32* dma;
    u32 offset;

    source = &gPassageClearPaletteCycleState;
    value = *source;
    state = source;
    if (value <= 9) {
        dma = (volatile u32*)0x040000D4;
        offset = *state - 1;
    } else {
        dma = (volatile u32*)0x040000D4;
        value = *state;
        offset = 17 - value;
    }
    dma[0] = (u32)(sPassageClearPaletteCycleFrames + offset * 32);
    dma[1] = 0x05000020;
    dma[2] = 0x80000010;
    (void)dma[2];
    if (gStageEntrySequenceTimer % 3 == 0) {
        (*state)++;
    }
    if (*state == 18) {
        *state = 0;
    }
}

void SpawnPassageClearParticles(void)
{
    if (gStageEntrySequenceTimer % 18 == 0) {
        gPassageClearSlowParticles[gPassageClearSlowParticleIndex].x =
            sPassageClearParticleSpawnX[MinigameRandom() % 11];
        gPassageClearSlowParticles[gPassageClearSlowParticleIndex].y = -160;
        gPassageClearSlowParticles[gPassageClearSlowParticleIndex].type = MinigameRandom() % 6 + 1;
        gPassageClearSlowParticleIndex++;
        if (gPassageClearSlowParticleIndex > 4) {
            gPassageClearSlowParticleIndex = 0;
        }
    }
    if (gStageEntrySequenceTimer % 11 == 0) {
        gPassageClearMediumParticles[gPassageClearMediumParticleIndex].x =
            sPassageClearParticleSpawnX[MinigameRandom() % 11];
        gPassageClearMediumParticles[gPassageClearMediumParticleIndex].y = -160;
        gPassageClearMediumParticles[gPassageClearMediumParticleIndex].type = MinigameRandom() % 6 + 1;
        gPassageClearMediumParticleIndex++;
        if (gPassageClearMediumParticleIndex > 4) {
            gPassageClearMediumParticleIndex = 0;
        }
    }
    if (gStageEntrySequenceTimer % 9 == 0) {
        gPassageClearFastParticles[gPassageClearFastParticleIndex].x =
            sPassageClearParticleSpawnX[MinigameRandom() % 11];
        gPassageClearFastParticles[gPassageClearFastParticleIndex].y = -160;
        gPassageClearFastParticles[gPassageClearFastParticleIndex].type = MinigameRandom() % 6 + 1;
        gPassageClearFastParticleIndex++;
        if (gPassageClearFastParticleIndex > 4) {
            gPassageClearFastParticleIndex = 0;
        }
    }
}

void RenderPassageClearOam(void)
{
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
    } PassageClearOamData;

    struct PassageClearAnimationState* secondaryState;
    const u16* src;
    u16* dest;
    u16* pbPtr;
    u16* pcPtr;
    u16* pdPtr;
    u16 affine[4];
    u16 attr;
    u16 particlePd;
    s32 currentSlot;
    s32 nextSlot;
    s32 i;

    currentSlot = 0;
    nextSlot = gOamSlotsUsed;
    dest = (u16*)&gOamBuffer[nextSlot];

    if (gPassageClearMainAnimationState.state == 2) {
        gPassageClearMainAnimationState.timer++;
        if (sPassageClearMainAnimation[gPassageClearMainAnimationState.frame].time <
            gPassageClearMainAnimationState.timer) {
            gPassageClearMainAnimationState.timer = 1;
            gPassageClearMainAnimationState.frame++;
            if (sPassageClearMainAnimation[gPassageClearMainAnimationState.frame].time == 0) {
                gPassageClearMainAnimationState.timer = 0;
                gPassageClearMainAnimationState.frame = 0;
                gPassageClearMainAnimationState.state = 0;
            }
        }
        if (gPassageClearMainAnimationState.state == 2) {
            src = GP(sPassageClearMainAnimation[gPassageClearMainAnimationState.frame].oam);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = (u8)gStageEntryMainSpriteState[3] + attr - 4;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gStageEntryMainSpriteState[2] - 2) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }

    if (gPassageClearBonusAnimationActive != 0) {
        gPassageClearBonusAnimationTimer++;
        if (sPassageClearBonusItemAnimation[gPassageClearBonusAnimationTimer].time < gPassageClearBonusAnimationTimer) {
            gPassageClearBonusAnimationTimer = 1;
            gPassageClearBonusAnimationFrame++;
            if (sPassageClearBonusItemAnimation[gPassageClearBonusAnimationFrame].time == 0) {
                gPassageClearBonusAnimationTimer = 0;
                gPassageClearBonusAnimationFrame = 0;
                gPassageClearBonusAnimationActive = 0;
            }
        }
        if (gPassageClearBonusAnimationActive != 0) {
            for (i = 0; i < 6; i++) {
                src = GP(sPassageClearBonusItemAnimation[gPassageClearBonusAnimationFrame].oam);
                nextSlot += *(src++);
                if (nextSlot > 128) {
                    return;
                }
                for (; currentSlot < nextSlot; currentSlot++) {
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.y = (gPassageClearBonusItemPositions[i].y >> 4) + attr;
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.x = (attr + (gPassageClearBonusItemPositions[i].x >> 4)) & 0x1FF;
                    *(dest++) = *(src++);
                    gOamBuffer[currentSlot].split.priority = 0;
                    dest++;
                }
                gPassageClearBonusItemPositions[i].x += sPassageClearBonusItemXVelocities[i];
                gPassageClearBonusItemPositions[i].y += sPassageClearBonusItemYVelocities[i];
            }
        }
    }

    src = GP(gPassageClearBannerOam);
    nextSlot += *(src++);
    if (nextSlot > 128) {
        return;
    }
    secondaryState = &gPassageClearSecondaryAnimationState;
    for (; currentSlot < nextSlot; currentSlot++) {
        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.y = (u8)gStageEntryMainSpriteState[3] + attr;
        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.x = (attr + gStageEntryMainSpriteState[2]) & 0x1FF;
        *(dest++) = *(src++);
        gOamBuffer[currentSlot].split.priority = 0;
        dest++;
    }

    if (secondaryState->state != 0) {
        secondaryState->timer++;
        if (sPassageClearSecondaryAltAnimation[secondaryState->frame].time < secondaryState->timer) {
            secondaryState->timer = 1;
            secondaryState->frame++;
            if (sPassageClearSecondaryAltAnimation[secondaryState->frame].time == 0) {
                /* The original clears the main timer here rather than the secondary
                   one; the secondary timer is reset by the branch above instead. */
                gPassageClearMainAnimationState.timer = 0;
                secondaryState->frame = 0;
                secondaryState->state = 0;
            }
        }
        if (secondaryState->state != 0) {
            src = GP(sPassageClearSecondaryAltAnimation[secondaryState->frame].oam);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = (u8)gStageEntryMainSpriteState[3] + attr;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gStageEntryMainSpriteState[2] - 16) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }

    if (gPassageClearMainAnimationState.state == 1) {
        gPassageClearMainAnimationState.timer++;
        if (sPassageClearSecondaryAnimation[gPassageClearMainAnimationState.frame].time <
            gPassageClearMainAnimationState.timer) {
            gPassageClearMainAnimationState.timer = 1;
            gPassageClearMainAnimationState.frame++;
            if (sPassageClearSecondaryAnimation[gPassageClearMainAnimationState.frame].time == 0) {
                gPassageClearMainAnimationState.timer = 0;
                gPassageClearMainAnimationState.frame = 0;
                gPassageClearMainAnimationState.state = 0;
            }
        }
        if (gPassageClearMainAnimationState.state == 1) {
            src = GP(sPassageClearSecondaryAnimation[gPassageClearMainAnimationState.frame].oam);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = (u8)gStageEntryMainSpriteState[3] + attr - 4;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gStageEntryMainSpriteState[2]) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }

    for (i = 0; i < 5; i++) {
        if (gPassageClearFastParticles[i].type != 0) {
            src = GP(sPassageClearFastParticleOamFrames[gPassageClearFastParticles[i].type - 1]);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = (gPassageClearFastParticles[i].y >> 4) + attr;
                gOamBuffer[currentSlot].split.affineMode = 1;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gPassageClearFastParticles[i].x) & 0x1FF;
                ((PassageClearOamData*)gOamBuffer)[currentSlot].split.affineIndex =
                    (gPassageClearFastParticles[i].type - 1) / 3 + 4;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 2;
                dest++;
            }
            gPassageClearFastParticles[i].y += 60;
            if (gPassageClearFastParticles[i].y > 0xAA0) {
                gPassageClearFastParticles[i].type = 0;
            }
        }
    }

    for (i = 0; i < 5; i++) {
        if (gPassageClearMediumParticles[i].type != 0) {
            src = GP(sPassageClearMediumParticleOamFrames[gPassageClearMediumParticles[i].type - 1]);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = (gPassageClearMediumParticles[i].y >> 4) + attr;
                gOamBuffer[currentSlot].split.affineMode = 3;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gPassageClearMediumParticles[i].x) & 0x1FF;
                ((PassageClearOamData*)gOamBuffer)[currentSlot].split.affineIndex =
                    (gPassageClearMediumParticles[i].type - 1) / 3 + 2;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 2;
                dest++;
            }
            gPassageClearMediumParticles[i].y += 52;
            if (gPassageClearMediumParticles[i].y > 0xAA0) {
                gPassageClearMediumParticles[i].type = 0;
            }
        }
    }

    for (i = 0; i < 5; i++) {
        if (gPassageClearSlowParticles[i].type != 0) {
            src = GP(sPassageClearSlowParticleOamFrames[gPassageClearSlowParticles[i].type - 1]);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = (gPassageClearSlowParticles[i].y >> 4) + attr;
                gOamBuffer[currentSlot].split.affineMode = 1;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gPassageClearSlowParticles[i].x) & 0x1FF;
                ((PassageClearOamData*)gOamBuffer)[currentSlot].split.affineIndex =
                    (gPassageClearSlowParticles[i].type - 1) / 3;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 2;
                dest++;
            }
            gPassageClearSlowParticles[i].y += 32;
            if (gPassageClearSlowParticles[i].y > 0xAA0) {
                gPassageClearSlowParticles[i].type = 0;
            }
        }
    }

    for (i = 0; i < 6; i++) {
        pdPtr = &affine[3];
        gPassageClearParticleAffineAngles[i] += sPassageClearParticleAngleSteps[i];
        affine[0] = FixedMul(sSinCosTable[gPassageClearParticleAffineAngles[i] + 64], (s16)FixedInverse(0x100));
        pbPtr = &affine[1];
        *pbPtr = FixedMul(sSinCosTable[gPassageClearParticleAffineAngles[i]], (s16)FixedInverse(0x100));
        pcPtr = &affine[2];
        *pcPtr = FixedMul((s16) - (u16)sSinCosTable[gPassageClearParticleAffineAngles[i]], (s16)FixedInverse(0x100));
        particlePd = FixedMul(sSinCosTable[gPassageClearParticleAffineAngles[i] + 64], (s16)FixedInverse(0x100));
        *pdPtr = particlePd;
        gOamBuffer[i * 4].all.affineParam = affine[0];
        gOamBuffer[i * 4 + 1].all.affineParam = *pbPtr;
        gOamBuffer[i * 4 + 2].all.affineParam = *pcPtr;
        gOamBuffer[i * 4 + 3].all.affineParam = particlePd;
    }
    gOamSlotsUsed = nextSlot;
}

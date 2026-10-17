#include "boss_treasure_cutscene.h"

#include "background_registers.h"
#include "fixed_point.h"
#include "init_helpers.h"
#include "minigame.h"
#include "score.h"
#include "stage_select.h"

struct AGB_ALIGN BossTreasureItem {
    s16 x;
    s16 y;
    s16 scale;
    u8 active;
    u8 unused;
};

struct AGB_ALIGN BossTreasureUnlockIcon {
    u8 x;
    u8 y;
    u8 active;
    u8 unused;
};

struct AGB_ALIGN BossTreasureSparkleSlot {
    u8 x;
    u8 y;
    u8 active;
    u8 unused;
};

struct AGB_ALIGN BossTreasureParticle {
    s16 x;
    s16 y;
    u8 scaleIndex;
    s8 xVelocity;
    s8 yVelocity;
    u8 unused7;
    u16 animationTimer;
    u16 animationFrame;
    u8 active;
    u8 unusedD[3];
};

extern u16 gStageEntryHorizontalScroll[];
extern u16 gStageEntrySequenceTimer;
extern s16 gBossTreasureAffineScale;
extern struct BossTreasureItem gBossTreasureItems[];
extern u8 gBossTreasureLastItemIndex;
extern struct BossTreasureUnlockIcon gBossTreasureUnlockIcons[];
extern struct BossTreasureSparkleSlot gBossTreasureUnlockSparkles[];
extern struct BossTreasureParticle gBossTreasureParticles[][20];
extern u8 gBossTreasureParticleRingIndex;
extern u8 gBossTreasureActivePassage;
extern u16 gBossTreasurePaletteFlashTimer;
extern u8 gBossTreasurePaletteFlashActive;

extern const u16 sBossTreasureFirstPhaseXPositions[];
extern const u16 sBossTreasureFirstPhaseYPositions[];
extern const u16 sBossTreasureFirstPhaseTileValues[];
extern const u8 sBossTreasureAffineXOffsets[];
extern const u8 sBossTreasureAffineYOffsets[];
extern const u16 sBossTreasureAffineScales[];
extern const u16 sBossTreasureSecondPhaseXPositions[];
extern const u16 sBossTreasureSecondPhaseYPositions[];
extern const u16 sBossTreasureSecondPhaseTileValues[];
extern const u8 sBossTreasureBlendInTimes[];
extern const u8 sBossTreasureBlendOutTimes[];
extern const u8 sBossTreasureUnlockIconDurations[];
extern const u8 sBossTreasurePassageIconTiles[];
extern const u8 sBossTreasureLockedIconTiles[];
extern const u8 sBossTreasureUnlockedIconTiles[];
extern const u8 sBossTreasureIconUnlockThresholds[];
extern const u8 sBossTreasureSparkleStartTimes[];
extern const u8 sBossTreasureSparkleDurations[];
extern const u8 sBossTreasureSparkle0XPath[];
extern const u8 sBossTreasureSparkle0YPath[];
extern const u8 sBossTreasureSparkle1XPath[];
extern const u8 sBossTreasureSparkle1YPath[];
extern const u8 sBossTreasureSparkle2XPath[];
extern const u8 sBossTreasureSparkle2YPath[];
extern const u8 sBossTreasureSparkle3XPath[];
extern const u8 sBossTreasureSparkle3YPath[];
extern const u8 sBossTreasureSparkle4XPath[];
extern const u8 sBossTreasureSparkle4YPath[];
extern const u8 sBossTreasureSparkle5XPath[];
extern const u8 sBossTreasureSparkle5YPath[];
extern const u8 sBossTreasureSparkle6XPath[];
extern const u8 sBossTreasureSparkle6YPath[];
extern const u8 sBossTreasureSparkle7XPath[];
extern const u8 sBossTreasureSparkle7YPath[];
extern const u8 sBossTreasureSparkle8XPath[];
extern const u8 sBossTreasureSparkle8YPath[];
extern const u8 sBossTreasureSparkle9XPath[];
extern const u8 sBossTreasureSparkle9YPath[];
extern const u8 sBossTreasureSparkle10XPath[];
extern const u8 sBossTreasureSparkle10YPath[];
extern const s32 sBossTreasureParticleXVelocities[];
extern const s32 sBossTreasureParticleYVelocities[];
extern const s32 sBossTreasureParticleXOffsets[];
extern const s32 sBossTreasureParticleYOffsets[];
extern const u8 sBossTreasureBackgroundPalette[];
extern const u8 sBossTreasureBackgroundTilesLz[];
extern const u8 sBossTreasureSpritePalette[];
extern const u8 sBossTreasureSpriteTiles[];
extern const u16 sBossTreasureIdlePaletteFlashFrames[];
extern const u16 sBossTreasureActivePaletteFlashFrames[];
extern GBA_PTR(const u16*) const sBossTreasureItemOamFramesByPassage[];
extern GBA_PTR(const u16*) const sBossTreasureUnlockIconOamFrames[];
extern GBA_PTR(const u16*) const sBossTreasureSparkleOamFrames[];
extern const u16 sBossTreasureItemOamFrame[];
extern const struct AnimationFrame sBossTreasureParticleAnimation[];

int UpdateBossTreasureCutscene(void)
{
    s32 treasureIndex;
    s32 sparkleIndex;
    s32 iconIndex;
    s32 iconTrigger;
    s32 zero;
    vu16* phaseCounterPointer;
    s32 timer;
    u8* treasure;
    u8* icon;
    vu16* state3Counter;
    s8* blendBPointer;
    const u8* blendInTimes;
    const u8* sparkleStartTimes;
    vu16* blendAlphaRegister;
    UpdateBossTreasurePaletteFlash();
    UpdateBossTreasurePaletteSparkle();
    switch (gStageEntrySequenceStep) {
        case 0: {
            u8* maximumPointer;
            u8* maximumLoopPointer;
            s32 one;
            s32 iconTrigger;
            u8* caseTreasure;
            s32 treasureOffset;
            treasureIndex = 0;
            maximumPointer = &gBossTreasureLastItemIndex;
            if (treasureIndex <= (*maximumPointer)) {
                phaseCounterPointer = &gStageEntrySequenceTimer;
                maximumLoopPointer = maximumPointer;
                iconTrigger = 65;
                caseTreasure = (u8*)gBossTreasureItems;
                one = 1;
                do {
                    treasureOffset = treasureIndex << 3;
                    if ((*phaseCounterPointer) == treasureOffset) {
                        caseTreasure[6] = one;
                    }
                    if ((*phaseCounterPointer) == iconTrigger) {
                        iconIndex = 0;
                        caseTreasure[6] = iconIndex;
                        if (treasureIndex == (*maximumLoopPointer)) {
                            goto first_phase_complete;
                        }
                    }
                    iconTrigger += 8;
                    caseTreasure += 8;
                    treasureIndex++;
                } while (treasureIndex <= (*maximumLoopPointer));
            }
            UpdateBossTreasureFirstPhaseTreasureItems();
            UpdateBossTreasureParticles();
            if ((gStageEntrySequenceTimer % 3) == 0) {
                SpawnBossTreasureParticles();
            }
            gStageEntrySequenceTimer++;
            break;
        }

        case 1:
            UpdateBossTreasureParticles();
            gStageEntrySequenceTimer++;
            timer = gStageEntrySequenceTimer;
            if ((timer & 1) == 0) {
                gBldy++;
                *((vu16*)(0x4000000 + 0x54)) = gBldy;
                if (gBldy == 15) {
                    gStageEntrySequenceTimer = 0;
                    gStageEntrySequenceStep++;
                    *((vu16*)(0x4000000 + 0x0)) = 0x1404;
                    ClearBossTreasureParticles();
                    m4aSongNumStart(0x1AE);
                }
            }
            break;

        case 2: {
            u16 initialCounter;
            vu16* caseCounterPointer;
            s32 caseTrigger;
            initialCounter = gStageEntrySequenceTimer;
            if (((initialCounter & 3) == 0) && (gBldy != 0)) {
                gBldy--;
                *((vu16*)(0x4000000 + 0x54)) = gBldy;
            }
            treasureIndex = 0;
            if (treasureIndex <= gBossTreasureLastItemIndex) {
                caseCounterPointer = &gStageEntrySequenceTimer;
                ASM_BARRIER(: : "r"(caseCounterPointer));
                treasure = (u8*)gBossTreasureItems;
                caseTrigger = 14;
                do {
                    caseCounterPointer = &gStageEntrySequenceTimer;
                    timer = treasureIndex << 4;
                    if ((*caseCounterPointer) == caseTrigger) {
                        treasure[6] = 1;
                    }
                    if (gStageEntrySequenceTimer == (timer + 174)) {
                        zero = 0;
                        treasure[6] = zero;
                        gBossTreasurePaletteFlashActive = 1;
                        gBossTreasurePaletteFlashTimer = zero;
                        m4aSongNumStart(0x1AC);
                        if (treasureIndex == gBossTreasureLastItemIndex) {
                            goto second_phase_complete;
                        }
                    }
                    treasure += 8;
                    caseTrigger += 16;
                    treasureIndex++;
                } while (treasureIndex <= gBossTreasureLastItemIndex);
            }
            UpdateBossTreasureSecondPhaseTreasureItems();
            UpdateBossTreasureParticles();
            if ((gStageEntrySequenceTimer % 3) == 0) {
                SpawnBossTreasureParticles();
            }
            if (gStageEntrySequenceTimer <= 158) {
                gStageEntryHorizontalScroll[0] += (s8)sBossTreasureAffineXOffsets[gStageEntrySequenceTimer];
                gStageEntryHorizontalScroll[1] += (s8)sBossTreasureAffineYOffsets[gStageEntrySequenceTimer];
                gBossTreasureAffineScale = sBossTreasureAffineScales[gStageEntrySequenceTimer];
                UpdateBossTreasureAffineState();
            }
            gStageEntrySequenceTimer++;
            break;
        }

        case 3: {
            u16 stateTimer;
            sparkleIndex = 0;
            state3Counter = &gStageEntrySequenceTimer;
            blendInTimes = sBossTreasureBlendInTimes;
            sparkleStartTimes = sBossTreasureSparkleStartTimes;
            stateTimer = gStageEntrySequenceTimer;
            blendBPointer = &gStageTransitionBlendEvb;
            blendAlphaRegister = (vu16*)0x04000052;
            while (sparkleIndex <= 9) {
                if (stateTimer == (*((const u8*)(((u32)sparkleIndex) + ((u32)blendInTimes))))) {
                    gStageTransitionBlendEva--;
                    if (((s8)gStageTransitionBlendEva) <= 11) {
                        gStageTransitionBlendEva = 12;
                    }
                    (*blendBPointer)++;
                    *blendAlphaRegister = (((s8)gStageTransitionBlendEva) << 8) | (*blendBPointer);
                } else if (stateTimer == sBossTreasureBlendOutTimes[sparkleIndex]) {
                    gStageTransitionBlendEva++;
                    if (((s8)gStageTransitionBlendEva) > 16) {
                        gStageTransitionBlendEva = 16;
                    }
                    (*blendBPointer)--;
                    *blendAlphaRegister = (((s8)gStageTransitionBlendEva) << 8) | (*blendBPointer);
                }
                sparkleIndex++;
            }

            {
                vu16* freshCounter;
                sparkleIndex = 0;
                while (sparkleIndex <= 10) {
                    timer = *((const u8*)(((u32)sparkleIndex) + ((u32)sparkleStartTimes)));
                    ASM_BARRIER(: "=r"(freshCounter) : "0"(&gStageEntrySequenceTimer));
                    if ((*freshCounter) == timer) {
                        register s32 activeOne PIN("r0") = 1;
                        ((struct BossTreasureSparkleSlot*)gBossTreasureUnlockSparkles)[sparkleIndex].active = activeOne;
                    } else if ((*state3Counter) == (timer + sBossTreasureSparkleDurations[sparkleIndex])) {
                        ((struct BossTreasureSparkleSlot*)gBossTreasureUnlockSparkles)[sparkleIndex].active = 0;
                    }
                    sparkleIndex++;
                }
            }

            UpdateBossTreasureUnlockSparkles();
            {
                treasureIndex = 0;
                state3Counter = &gStageEntrySequenceTimer;
                icon = (u8*)gBossTreasureUnlockIcons;
                iconTrigger = 34;
                {
                    s32 holdR7;
                    ASM_BARRIER_VOLATILE(: "=r"(holdR7));
                    while (treasureIndex <= 4) {
                        ASM_BARRIER_VOLATILE(: "+r"(holdR7));
                        if ((*((volatile u16*)(&gStageEntrySequenceTimer))) == iconTrigger) {
                            icon[2] = 1;
                        }
                        if ((*((volatile u16*)(&gStageEntrySequenceTimer))) ==
                            (sBossTreasureUnlockIconDurations[treasureIndex] + iconTrigger)) {
                            {
                                s32 localZero;
                                localZero = 0;
                                icon[2] = localZero;
                            }
                            if (treasureIndex == 4) {
                                goto unlock_complete;
                            }
                        }
                        icon += 4;
                        iconTrigger -= 5;
                        treasureIndex++;
                        ASM_BARRIER_VOLATILE(: "+r"(holdR7));
                    }
                }
            }
            UpdateBossTreasureUnlockIcons();
            gStageEntrySequenceTimer++;
            break;
        }

        case 4:
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer > 10) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                return 1;
            }
            break;
    }

    return 0;
first_phase_complete:
    gStageEntrySequenceStep++;

    *phaseCounterPointer = iconIndex;
    return 0;
second_phase_complete:
    *((vu16*)(0x4000000 + 0x50)) = 0x410;

    gStageTransitionBlendEva = 16;
    gStageTransitionBlendEvb = 0;
    *((vu16*)(0x4000000 + 0x52)) = (((s8)gStageTransitionBlendEva) << 8) | ((s8)gStageTransitionBlendEvb);
    gBossTreasureActivePassage = 0;
    gStageEntrySequenceTimer = 0;
    gStageEntrySequenceStep++;
    m4aSongNumStart(0x1AF);
    return 0;
unlock_complete:
    InitMinigameScoreDisplay();

    gBldy = 0;
    *((vu16*)(0x4000000 + 0x54)) = 0;
    gStageEntrySequenceTimer = 0;
    gStageEntrySequenceStep++;
    return 0;
}

void InitializeBossTreasureCutscene(void)
{
    u8* collection;
    u8* passage;
    u8* collectionEntry;
    u16* scroll;
    u8 zeroByte;
    u8 loopZero;
    int new_var;
    u16 zeroHalf;
    s32 value;
    u16 finalZero;
    InitializeVideoMemory();
    {
        vu32* dma;
        u32 dmaStatus;
        u32 dmaMask;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)sBossTreasureSpritePalette;
        dma[1] = 0x05000200;
        dma[2] = 0x80000100;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }
    {
        vu32* dma;
        vu32* waitDma;
        u32 dmaStatus;
        u32 dmaMask;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)sBossTreasureBackgroundPalette;
        dma[1] = 0x05000000;
        dma[2] = 0x80000100;
        (void)dma[2];
        dma[0] = (u32)sBossTreasureSpriteTiles;
        dma[1] = 0x06014000;
        dma[2] = 0x80000C00;
        (void)dma[2];
        waitDma = dma;
        dmaStatus = waitDma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = waitDma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }
    LZ77UnCompVram((const u32*)sBossTreasureBackgroundTilesLz, (void*)0x06000000);
    InitMinigameScoreDisplay();
    scroll = gStageEntryHorizontalScroll;
    zeroByte = 0;
    new_var = 0;
    zeroHalf = new_var;
    scroll[0] = 32;
    scroll[1] = zeroHalf;
    gBossTreasureAffineScale = 0x100;
    gStageEntrySequenceStep = zeroByte;
    gStageEntrySequenceTimer = zeroHalf;
    UpdateBossTreasureAffineState();
    gBossTreasureLastItemIndex = zeroByte;
    collection = (u8*)gCurrentCollection;
    passage = &gCurrentPassageTemp;
    value = *passage;
    if (((s32)(((u32)collection[(value * 24) + 16]) << 29)) < 0) {
        gBossTreasureLastItemIndex = 1;
    }
    collectionEntry = collection + ((*passage) * 24);
    if (((s32)(((u32)collectionEntry[16]) << 30)) < 0) {
        gBossTreasureLastItemIndex++;
    }
    if ((((u32)collectionEntry[16]) << 31) != 0) {
        gBossTreasureLastItemIndex++;
    }
    {
        volatile u8* base;
        volatile u8* current;
        base = (u8*)gBossTreasureItems;
        loopZero = 0;
        current = base + 24;
        do {
            current[6] = loopZero;
            current -= 8;
        } while (((s32)current) >= ((s32)base));
        if (((s32)current) < ((s32)base)) {
            base = (u8*)gBossTreasureUnlockIcons;
        }
        loopZero = 0;
        current = base + 16;
        do {
            current[2] = loopZero;
            current -= 4;
        } while (((s32)current) >= ((s32)base));
        zeroHalf = ((s32)current) < ((s32)base);
        if (zeroHalf) {
            base = (u8*)gBossTreasureUnlockSparkles;
        }
        loopZero = 0;
        current = base + 40;
        do {
            current[2] = loopZero;
            current -= 4;
        } while (((s32)current) >= ((s32)base));
    }
    finalZero = 0;
    ClearBossTreasureParticles();
    gStageTransitionPaletteCycleTimer = finalZero;
    gBossTreasureActivePassage = gCurrentPassageTemp;
    gBossTreasurePaletteFlashTimer = finalZero;
    gBossTreasurePaletteFlashActive = 0;
    *((vu16*)0x04000000) = 0x1004;
    *((vu16*)0x0400001A) = gStageEntryHorizontalScroll[1];
    *((vu16*)0x04000018) = gStageEntryHorizontalScroll[0];
    m4aSongNumStart(0x1AD);
}

void UpdateBossTreasureFirstPhaseTreasureItems(void)
{
    s32 i;
    s16* treasure;
    const u16* xPositions;
    u16* timer;
    const u16* yPositions;
    const u16* tileValues;
    u32 baseIndex;
    u32 offset;

    i = 0;
    if (i <= gBossTreasureLastItemIndex) {
        xPositions = sBossTreasureFirstPhaseXPositions;
        timer = &gStageEntrySequenceTimer;
        yPositions = sBossTreasureFirstPhaseYPositions;
        tileValues = sBossTreasureFirstPhaseTileValues;
        treasure = (s16*)gBossTreasureItems;
        do {
            baseIndex = i << 3;
            if (*((u8*)treasure + 6) != 0) {
                offset = *timer - baseIndex;
                treasure[0] = xPositions[offset];
                offset = *timer - baseIndex;
                treasure[1] = yPositions[offset];
                offset = *timer - baseIndex;
                treasure[2] = tileValues[offset];
            }
            treasure = (s16*)((u8*)treasure + 8);
            i++;
        } while (i <= gBossTreasureLastItemIndex);
    }
}

void UpdateBossTreasureSecondPhaseTreasureItems(void)
{
    s32 i;
    s16* treasure;
    const u16* xPositions;
    u16* timer;
    const u16* yPositions;
    const u16* tileValues;
    s32 baseIndex;
    u32 offset;

    i = 0;
    if (i <= gBossTreasureLastItemIndex) {
        xPositions = sBossTreasureSecondPhaseXPositions;
        timer = &gStageEntrySequenceTimer;
        yPositions = sBossTreasureSecondPhaseYPositions;
        tileValues = sBossTreasureSecondPhaseTileValues;
        treasure = (s16*)gBossTreasureItems;
        baseIndex = 14;
        do {
            if (*((u8*)treasure + 6) != 0) {
                offset = *timer - baseIndex;
                treasure[0] = xPositions[offset];
                offset = *timer - baseIndex;
                treasure[1] = yPositions[offset];
                offset = *timer - baseIndex;
                treasure[2] = tileValues[offset];
            }
            treasure = (s16*)((u8*)treasure + 8);
            baseIndex += 16;
            i++;
        } while (i <= gBossTreasureLastItemIndex);
    }
}

void UpdateBossTreasureUnlockIcons(void)
{
    s32 iconIndex;
    s32 passage;
    s32 tableIndex;
    u8* icon;
    s32 threshold;
    u32 passageIcons;
    u32 unlockedTiles;
    u32 lockedTiles;

    iconIndex = 0;
    passageIcons = (u32)sBossTreasurePassageIconTiles;
    unlockedTiles = (u32)sBossTreasureUnlockedIconTiles;
    lockedTiles = (u32)sBossTreasureLockedIconTiles;
    icon = (u8*)gBossTreasureUnlockIcons;
    do {
        if (icon[2] != 0) {
            threshold = ((4 - iconIndex) * 5) + 14;
            passage = gCurrentPassageTemp;
            passage--;
            icon[0] = *(u8*)(passage + passageIcons);
            if (sBossTreasureIconUnlockThresholds[iconIndex] <= (gStageEntrySequenceTimer - threshold)) {
                passage = gCurrentPassageTemp;
                passage--;
                tableIndex = passage << 2;
                tableIndex += passage;
                tableIndex = -(-tableIndex - iconIndex);
                tableIndex = tableIndex + unlockedTiles;
            } else {
                passage = gCurrentPassageTemp;
                passage--;
                tableIndex = passage << 2;
                tableIndex += passage;
                tableIndex = -(-tableIndex - iconIndex);
                tableIndex = tableIndex + lockedTiles;
            }
            icon[1] = *(u8*)tableIndex;
        }
        icon += 4;
        iconIndex++;
    } while (iconIndex <= 4);
}

void UpdateBossTreasureUnlockSparkles(void)
{
    s32 sparkleIndex;
    s32 sparkleOffset;
    u8* sparkleBase;
    sparkleIndex = 0;
    sparkleBase = (u8*)gBossTreasureUnlockSparkles;
    do {
        sparkleOffset = sparkleIndex << 2;
        if (sparkleBase[sparkleOffset + 2] != 0) {
            switch (sparkleIndex) {
                case 0:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle0XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle0YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    break;

                case 1:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle1XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle1YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    break;

                case 2:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle2XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle2YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    break;

                case 3:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle3XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle3YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    break;

                case 4:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle4XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle4YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    break;

                case 5:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle5XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle5YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    break;

                case 6:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle6XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle6YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    break;

                case 7:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle7XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle7YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    break;

                case 8:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle8XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle8YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    break;

                case 9:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle9XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle9YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 36)];
                    break;

                case 10:
                    sparkleBase[sparkleIndex << 2] = sBossTreasureSparkle10XPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    sparkleBase[(sparkleIndex << 2) + 1] = sBossTreasureSparkle10YPath
                        [(gStageEntrySequenceTimer - sBossTreasureSparkleStartTimes[sparkleIndex]) +
                         ((gCurrentPassageTemp - 1) * 46)];
                    break;
            }
        }
        sparkleIndex++;
    } while (sparkleIndex <= 10);
}

void SpawnBossTreasureParticles(void)
{
    s32 treasureIndex;
    s32 groupOffset;
    u16* treasure;
    u16* writePointer;
    u8* ringIndex;
    const s32* verticalJitterTable;
    u8* treasureBase;
    u8(*particleArray)[];
    int zero;
    s32 scale;
    s32 particleGroupOffset;
    particleArray = (u8(*)[]) & gBossTreasureParticles;
    treasureIndex = 0;
    if (treasureIndex <= gBossTreasureLastItemIndex) {
        ringIndex = &gBossTreasureParticleRingIndex;
        groupOffset = 0;
        do {
            writePointer = (u16*)gBossTreasureItems;
            treasureBase = (u8*)writePointer;
            treasure = (u16*)(treasureBase + (treasureIndex << 3));
            if ((*(((u8*)treasure) + 6)) != 0) {
                scale = 0x200 / treasure[2];
                {
                    s32 random;
                    u32 particleOffset;
                    u8* particle;
                    s32 position;
                    const s32* table;
                    random = MinigameRandom();
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += groupOffset;
                    particleOffset += (u32)(*particleArray);
                    particle = (u8*)particleOffset;
                    zero = 0;
                    position = treasure[zero];
                    position <<= 4;
                    table = (const s32*)sBossTreasureParticleXOffsets;
                    random %= 3;
                    random = table[random];
                    random /= scale;
                    position += random;
                    *((u16*)particle) = position;
                }
                {
                    s32 random;
                    u32 particleOffset;
                    u8* particle;
                    s32 position;
                    const s32* table;
                    random = MinigameRandom();
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += groupOffset;
                    particleOffset += (u32)(*particleArray);
                    particle = (u8*)particleOffset;
                    position = treasure[1];
                    position <<= 4;
                    table = (verticalJitterTable = (const s32*)sBossTreasureParticleYOffsets);
                    random %= 3;
                    random = table[random];
                    random /= scale;
                    position += random;
                    *((u16*)(particle + 2)) = position;
                }
                {
                    s32 random;
                    u32 particleOffset;
                    u8* particle;
                    const s32* table;
                    random = MinigameRandom();
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += groupOffset;
                    particleOffset += (u32)(*particleArray);
                    particle = (u8*)particleOffset;
                    table = (const s32*)sBossTreasureParticleXVelocities;
                    random %= 3;
                    random = table[random];
                    random /= scale;
                    particle[5] = random;
                }
                {
                    s32 random;
                    u32 particleOffset;
                    u8* particle;
                    const s32* table;
                    random = MinigameRandom();
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += groupOffset;
                    particleOffset += (u32)(*particleArray);
                    particle = (u8*)particleOffset;
                    table = (const s32*)sBossTreasureParticleYVelocities;
                    random %= 3;
                    random = table[random];
                    random /= scale;
                    particle[6] = random;
                }
                {
                    u32 particleOffset;
                    u8* particle;
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += groupOffset;
                    particleOffset += (u32)(*particleArray);
                    particle = (u8*)particleOffset;
                    particle[4] = treasure[2] >> 7;
                }
                {
                    u32 particleOffset;
                    u8* particle;
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += groupOffset;
                    particleOffset += (u32)(*particleArray);
                    particle = (u8*)particleOffset;
                    particle[12] = 1;
                }
                particleGroupOffset = groupOffset;
                {
                    u32 particleOffset;
                    u8* particle;
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += particleGroupOffset;
                    particle = *particleArray;
                    particleOffset += (u32)particle;
                    particle = (u8*)particleOffset;
                    writePointer = (u16*)(particle + 8);
                    *writePointer = zero;
                }
                {
                    u32 particleOffset;
                    particleOffset = *ringIndex;
                    particleOffset <<= 4;
                    particleOffset += groupOffset;
                    *((u16*)((u8*)((u32)particleOffset + (u32)(*particleArray)) + 10)) = zero;
                }
            }
            groupOffset += 0x140;
            treasureIndex++;
        } while (treasureIndex <= gBossTreasureLastItemIndex);
    }
    gBossTreasureParticleRingIndex++;
    if (gBossTreasureParticleRingIndex == 20) {
        gBossTreasureParticleRingIndex = 0;
    }
}

void UpdateBossTreasureParticles(void)
{
    s32 group;
    s32 initialEndGroup;
    s32 endGroup;
    s32 nextGroup;
    s32 particleIndex;
    u8* particle;
    u8* particleBase;
    s32 velocity;
    u16 position;
    u32 offsetBase;
    u32 offset;

    group = 0;
    initialEndGroup = gBossTreasureLastItemIndex + 1;
    if (group < initialEndGroup) {
        particleBase = (u8*)gBossTreasureParticles;
        endGroup = initialEndGroup;
        do {
            offsetBase = group << 2;
            nextGroup = group + 1;
            offset = (offsetBase + group) << 6;
            particle = (u8*)((u32)offset + (u32)particleBase);
            particleIndex = 19;
            do {
                velocity = (s8)particle[5];
                position = *(u16*)&particle[0];
                *(u16*)&particle[0] = velocity + position;
                velocity = (s8)particle[6];
                position = *(u16*)&particle[2];
                *(u16*)&particle[2] = velocity + position;
                particle += 16;
                particleIndex--;
            } while (particleIndex >= 0);
            group = nextGroup;
        } while (group < endGroup);
    }
}

void ClearBossTreasureParticles(void)
{
    s32 group;
    s32 nextGroup;
    s32 particleIndex;
    u8* particle;
    u8* particleTail;
    u8* particleBase;
    u8* particleTailBase;
    u8 byteZero;
    u16 halfwordZero;
    u32 offset;
    u32 offsetBase;

    gBossTreasureParticleRingIndex = 0;
    group = 0;
    particleBase = (u8*)gBossTreasureParticles;
    byteZero = 0;
    halfwordZero = 0;
    particleTailBase = particleBase + 6;
    do {
        offsetBase = group << 2;
        nextGroup = group + 1;
        offset = (offsetBase + group) << 6;
        particleTail = (u8*)((u32)offset + (u32)particleTailBase);
        particle = (u8*)((u32)offset + (u32)particleBase);
        particleIndex = 19;
        do {
            *(u16*)&particleTail[2] = halfwordZero;
            *(u16*)&particleTail[4] = halfwordZero;
            particle[5] = byteZero;
            particleTail[0] = byteZero;
            particleTail[6] = byteZero;
            particleTail += 16;
            particle += 16;
            particleIndex--;
        } while (particleIndex >= 0);
        group = nextGroup;
    } while (group <= 3);
}

void UpdateBossTreasureAffineState(void)
{
    const s16* sinCosTable;
    s16* scale;
    u16* state;
    s32 cosValue;
    s32 sinValue;
    s32* new_var2;
    s16 inverse;
    s32 translation;
    s32 matrixValue;
    s32 product;
    s32 result;
    int new_var;
    sinCosTable = (const s16*)sSinCosTable;
    cosValue = sinCosTable[0x40];
    scale = &gBossTreasureAffineScale;
    inverse = FixedInverse(*scale);
    result = FixedMul(cosValue, inverse);
    state = gStageEntryHorizontalScroll;
    state[2] = result;
    sinValue = sinCosTable[0];
    inverse = FixedInverse(*scale);
    state[3] = FixedMul(sinValue, inverse);
    new_var = (s16)(-((u16)sinCosTable[0]));
    sinValue = new_var;
    inverse = FixedInverse(*scale);
    state[4] = FixedMul(sinValue, inverse);
    inverse = FixedInverse(*scale);
    state[5] = FixedMul(cosValue, inverse);
    translation = (120 - ((s16)state[0])) << 8;
    matrixValue = (s16)state[2];
    new_var2 = &translation;
    product = ((matrixValue << 4) - matrixValue) << 3;
    translation -= product;
    matrixValue = (s16)state[3];
    product = ((matrixValue << 2) + matrixValue) << 4;
    translation -= product;
    *((s32*)(&state[6])) = *new_var2;
    translation = (64 - ((s16)state[1])) << 8;
    matrixValue = (s16)state[4];
    product = ((matrixValue << 4) - matrixValue) << 3;
    translation -= product;
    matrixValue = (s16)state[5];
    product = ((matrixValue << 2) + matrixValue) << 4;
    translation -= product;
    *((s32*)(&gStageEntryHorizontalScroll[8])) = *new_var2;
}

void UpdateBossTreasurePaletteSparkle(void)
{
    vu32* dma;
    u16* timer;
    u8* collectionBase;
    u8* collection;
    u32 collectionFlag;
    s32 frame;

    timer = &gStageTransitionPaletteCycleTimer;
    (*timer)++;
    frame = (u16)(*timer / 10);
    if (frame > 11) {
        *timer = 0;
        frame = 0;
    }
    if (frame > 6) {
        frame = 12 - frame;
    }

    collectionBase = (u8*)gCurrentCollection;
    collectionFlag = collectionBase[0x70] & 0x20;
    collection = collectionBase;
    if ((collectionFlag != 0) && (gBossTreasureActivePassage != 4)) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)&sBossTreasureGoldenPaletteCycle[frame];
        dma[1] = 0x0500000C;
        dma[2] = 0x80000001;
        (void)dma[2];
    }
    if ((collection[0x40] & 0x20) && (gBossTreasureActivePassage != 2)) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)&sBossTreasureSapphirePaletteCycle[frame];
        dma[1] = 0x0500001A;
        dma[2] = 0x80000001;
        (void)dma[2];
    }
    if ((collection[0x28] & 0x20) && (gBossTreasureActivePassage != 1)) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)&sBossTreasureEmeraldPaletteCycle[frame];
        dma[1] = 0x0500000E;
        dma[2] = 0x80000001;
        (void)dma[2];
    }
    if ((collection[0x58] & 0x20) && (gBossTreasureActivePassage != 3)) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)&sBossTreasureRubyPaletteCycle[frame];
        dma[1] = 0x05000018;
        dma[2] = 0x80000001;
        (void)dma[2];
    }
}

void UpdateBossTreasurePaletteFlash(void)
{
    vu32* dma;
    vu16* timer;
    u8* activeFlag;
    s32 frame;
    s32 active;

    timer = &gBossTreasurePaletteFlashTimer;
    (*timer)++;
    activeFlag = &gBossTreasurePaletteFlashActive;
    active = *activeFlag;
    if (active != 0) {
        frame = *timer >> 1;
        if (frame > 5) {
            *timer = 0;
            *activeFlag = 0;
            frame = 0;
        }
        if (frame > 3) {
            frame = 6 - frame;
        }
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)(sBossTreasureActivePaletteFlashFrames + (frame * 16));
    } else {
        frame = *timer / 10;
        if (frame > 9) {
            *timer = active;
            frame = 0;
        }
        if (frame > 5) {
            frame = 10 - frame;
        }
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)(sBossTreasureIdlePaletteFlashFrames + (frame * 16));
    }
    dma[1] = 0x05000002;
    dma[2] = 0x80000010;
    dma[2];
}

void DrawBossTreasureCutsceneSprites(void)
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
    } BossTreasureOamData;

    const u16* src;
    u16* dest;
    u16 affine[4];
    u16 attr;
    u16 matrixPd;
    s16 scale;
    s32 currentSlot;
    s32 matrixSlot;
    s32 nextSlot;
    s32 particleIndex;
    s32 i;

    currentSlot = 0;
    nextSlot = gOamSlotsUsed;
    dest = (u16*)&gOamBuffer[nextSlot];

    for (i = 0; i <= gBossTreasureLastItemIndex; i++) {
        for (particleIndex = 0; particleIndex < 20; particleIndex++) {
            if (gBossTreasureParticles[i][particleIndex].active != 0) {
                gBossTreasureParticles[i][particleIndex].animationTimer++;
                if (sBossTreasureParticleAnimation[gBossTreasureParticles[i][particleIndex].animationFrame].time <
                    gBossTreasureParticles[i][particleIndex].animationTimer) {
                    gBossTreasureParticles[i][particleIndex].animationTimer = 0;
                    gBossTreasureParticles[i][particleIndex].animationFrame++;
                    if (sBossTreasureParticleAnimation[gBossTreasureParticles[i][particleIndex].animationFrame].time ==
                        0) {
                        gBossTreasureParticles[i][particleIndex].animationFrame--;
                        if (gBossTreasureItems[i].active == 0) {
                            gBossTreasureParticles[i][particleIndex].active = 0;
                        }
                    }
                }
                src = GP(sBossTreasureParticleAnimation[gBossTreasureParticles[i][particleIndex].animationFrame].oam);
                nextSlot += *(src++);
                if (nextSlot > 128) {
                    return;
                }
                for (; currentSlot < nextSlot; currentSlot++) {
                    attr = *(src++);
                    *(dest++) = attr;
                    if (gBossTreasureParticles[i][particleIndex].scaleIndex <= 1) {
                        gOamBuffer[currentSlot].split.y = (gBossTreasureParticles[i][particleIndex].y >> 4) + attr;
                        gOamBuffer[currentSlot].split.affineMode = 1;
                    } else {
                        gOamBuffer[currentSlot].split.y = (gBossTreasureParticles[i][particleIndex].y >> 4) + attr - 8;
                        gOamBuffer[currentSlot].split.affineMode = 3;
                    }
                    attr = *(src++);
                    *(dest++) = attr;
                    if (gBossTreasureParticles[i][particleIndex].scaleIndex <= 1) {
                        gOamBuffer[currentSlot].split.x =
                            (attr + (gBossTreasureParticles[i][particleIndex].x >> 4)) & 0x1FF;
                    } else {
                        gOamBuffer[currentSlot].split.x =
                            (attr + (gBossTreasureParticles[i][particleIndex].x >> 4) - 8) & 0x1FF;
                    }
                    ((BossTreasureOamData*)gOamBuffer)[currentSlot].split.affineIndex =
                        gBossTreasureParticles[i][particleIndex].scaleIndex + 4;
                    *(dest++) = *(src++);
                    gOamBuffer[currentSlot].split.priority = 0;
                    dest++;
                }
            }
        }
    }

    /* Matrices 4..7 are fixed unrotated scales used by the burst particles. */
    matrixSlot = 17;
    for (i = 0; i < 4; i++) {
        u16* pbPtr;
        u16* pcPtr;
        u16* pdPtr;

        pbPtr = &affine[1];
        pcPtr = &affine[2];
        pdPtr = &affine[3];
        scale = (i + 1) * 96;
        affine[0] = FixedMul(sSinCosTable[64], (s16)FixedInverse(scale));
        *pbPtr = FixedMul(sSinCosTable[0], (s16)FixedInverse(scale));
        *pcPtr = FixedMul((s16) - (u16)sSinCosTable[0], (s16)FixedInverse(scale));
        matrixPd = FixedMul(sSinCosTable[64], (s16)FixedInverse(scale));
        *pdPtr = matrixPd;
        gOamBuffer[i * 4 + 16].all.affineParam = affine[0];
        gOamBuffer[matrixSlot].all.affineParam = *pbPtr;
        gOamBuffer[matrixSlot + 1].all.affineParam = *pcPtr;
        gOamBuffer[matrixSlot + 2].all.affineParam = matrixPd;
        matrixSlot += 4;
    }

    if (gStageEntrySequenceStep == 0) {
        for (i = 0; i <= gBossTreasureLastItemIndex; i++) {
            u16* pdPtr;
            s32 itemPd;

            if (gBossTreasureItems[i].active != 0) {
                if (i == 0) {
                    src = GP(sBossTreasureItemOamFramesByPassage[gCurrentPassageTemp - 1]);
                } else {
                    src = sBossTreasureItemOamFrame;
                }
                nextSlot += *(src++);
                if (nextSlot > 128) {
                    return;
                }
                for (; currentSlot < nextSlot; currentSlot++) {
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.y = (u8)gBossTreasureItems[i].y + attr - 16;
                    gOamBuffer[currentSlot].split.affineMode = 3;
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.x = (attr + gBossTreasureItems[i].x - 16) & 0x1FF;
                    ((BossTreasureOamData*)gOamBuffer)[currentSlot].split.affineIndex = i;
                    *(dest++) = *(src++);
                    gOamBuffer[currentSlot].split.priority = 0;
                    dest++;
                }
                pdPtr = &affine[3];
                affine[0] = FixedMul(sSinCosTable[64], (s16)FixedInverse(gBossTreasureItems[i].scale));
                {
                    u16* pbPtr;
                    u16* pcPtr;

                    pbPtr = &affine[1];
                    *pbPtr = FixedMul(sSinCosTable[0], (s16)FixedInverse(gBossTreasureItems[i].scale));
                    pcPtr = &affine[2];
                    *pcPtr = FixedMul((s16) - (u16)sSinCosTable[0], (s16)FixedInverse(gBossTreasureItems[i].scale));
                    itemPd = FixedMul(sSinCosTable[64], (s16)FixedInverse(gBossTreasureItems[i].scale));
                    *pdPtr = itemPd;
                    gOamBuffer[i * 4].all.affineParam = affine[0];
                    gOamBuffer[i * 4 + 1].all.affineParam = *pbPtr;
                    gOamBuffer[i * 4 + 2].all.affineParam = *pcPtr;
                    gOamBuffer[i * 4 + 3].all.affineParam = itemPd;
                }
            }
        }
    } else {
        for (i = gBossTreasureLastItemIndex; i >= 0; i--) {
            u16* pdPtr;
            s32 itemPd;

            if (gBossTreasureItems[i].active != 0) {
                if (i == 0) {
                    src = GP(sBossTreasureItemOamFramesByPassage[gCurrentPassageTemp - 1]);
                } else {
                    src = sBossTreasureItemOamFrame;
                }
                nextSlot += *(src++);
                if (nextSlot > 128) {
                    return;
                }
                for (; currentSlot < nextSlot; currentSlot++) {
                    attr = *(src++);
                    *(dest++) = attr;
                    if ((u16)gBossTreasureItems[i].scale <= 255) {
                        gOamBuffer[currentSlot].split.y = (u8)gBossTreasureItems[i].y + attr;
                        gOamBuffer[currentSlot].split.affineMode = 1;
                    } else {
                        gOamBuffer[currentSlot].split.y = (u8)gBossTreasureItems[i].y + attr - 16;
                        gOamBuffer[currentSlot].split.affineMode = 3;
                    }
                    attr = *(src++);
                    *(dest++) = attr;
                    if ((u16)gBossTreasureItems[i].scale <= 255) {
                        gOamBuffer[currentSlot].split.x = (attr + gBossTreasureItems[i].x) & 0x1FF;
                    } else {
                        gOamBuffer[currentSlot].split.x = (attr + gBossTreasureItems[i].x - 16) & 0x1FF;
                    }
                    ((BossTreasureOamData*)gOamBuffer)[currentSlot].split.affineIndex = i;
                    *(dest++) = *(src++);
                    gOamBuffer[currentSlot].split.priority = 0;
                    dest++;
                }
                pdPtr = &affine[3];
                affine[0] = FixedMul(sSinCosTable[64], (s16)FixedInverse(gBossTreasureItems[i].scale));
                {
                    u16* pbPtr;
                    u16* pcPtr;

                    pbPtr = &affine[1];
                    *pbPtr = FixedMul(sSinCosTable[0], (s16)FixedInverse(gBossTreasureItems[i].scale));
                    pcPtr = &affine[2];
                    *pcPtr = FixedMul((s16) - (u16)sSinCosTable[0], (s16)FixedInverse(gBossTreasureItems[i].scale));
                    itemPd = FixedMul(sSinCosTable[64], (s16)FixedInverse(gBossTreasureItems[i].scale));
                    *pdPtr = itemPd;
                    gOamBuffer[i * 4].all.affineParam = affine[0];
                    gOamBuffer[i * 4 + 1].all.affineParam = *pbPtr;
                    gOamBuffer[i * 4 + 2].all.affineParam = *pcPtr;
                    gOamBuffer[i * 4 + 3].all.affineParam = itemPd;
                }
            }
        }
    }

    for (i = 0; i < 11; i++) {
        if (gBossTreasureUnlockSparkles[i].active != 0) {
            src = GP(sBossTreasureSparkleOamFrames[i]);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = gBossTreasureUnlockSparkles[i].y + attr;
                gOamBuffer[currentSlot].split.objMode = 0;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gBossTreasureUnlockSparkles[i].x) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }

    for (i = 4; i >= 0; i--) {
        if (gBossTreasureUnlockIcons[i].active != 0) {
            src = GP(sBossTreasureUnlockIconOamFrames[gCurrentPassageTemp - 1]);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = gBossTreasureUnlockIcons[i].y + attr;
                gOamBuffer[currentSlot].split.objMode = 1;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + gBossTreasureUnlockIcons[i].x) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }
    gOamSlotsUsed = nextSlot;
}

void ApplyBossTreasureAffineRegisters(void)
{
    u16* stateCopy;
    vu16* affineReg;
    u16* state;
    s32 stateIndex;
    s32 value;

    do {
        affineReg = (vu16*)0x04000020;
        state = gStageEntryHorizontalScroll;
        stateIndex = 2;
        *affineReg = state[stateIndex];
        affineReg++;
        *affineReg = state[3];
        affineReg++;
    } while (0);

    *affineReg = state[4];
    stateCopy = state;
    affineReg++;
    *affineReg = stateCopy[5];

    do {
        affineReg++;
        value = *(s32*)&state[6];
        *affineReg = value;
        affineReg++;
        *affineReg = (value & 0x0FFF0000) >> 16;
    } while (0);

    affineReg++;
    value = *(s32*)&state[8];
    *affineReg = value;
    affineReg++;
    *affineReg = (value & 0x0FFF0000) >> 16;
}

#include "stage_entry.h"
#include "color_effects.h"
#include "fixed_point.h"
#include "init_helpers.h"
#include "main.h"
#include "score.h"
#include "stage_select.h"
#include "voice_set.h"

#define PASSAGE_ENTRY 0
#define PASSAGE_GOLDEN 5

extern const struct AnimationFrame sUnk_863EFE0[];
extern const struct AnimationFrame sUnk_863EFB0[];
extern const struct AnimationFrame sUnk_863E1C4[];
extern const struct AnimationFrame sUnk_863E12C[];
extern const u32 sUnk_8639ADC[];
extern const u32 sUnk_8639AC4[];
extern const u16 sUnk_863ED00[];
extern const u16 sUnk_863ECF8[];
extern const u16 sUnk_863ECF0[];
extern const u16 sUnk_863E24C[];
extern const u16 sUnk_863E4AA[];
extern const struct AnimationFrame sUnk_863A2EC[];

s32 UpdateStageEntry(void)
{
    if (gStageEntryBlinkActive != 0) {
        UpdateStageEntryBlinkState();
    }
    UpdateStageEntryPaletteFadeCounters();
    CycleStageEntryPalette();
    return UpdateStageEntryStateMachine();
}

static inline s32 GetStageEntryWrappedScrollValue(const u16* horizontalScroll)
{
    s32 scroll;
    s32 adjusted;

    scroll = *((const s16*)horizontalScroll);
    adjusted = scroll;
    if (scroll < 0) {
        adjusted += 0x1FF;
    }
    return scroll - ((adjusted >> 9) << 9);
}

void InitializeStageEntrySceneGraphics(void)
{
    register vu32* dma PIN("r2");
    const u32* stageIndexTable;
    GBA_TYPEOF(const u8* const*, &(sStageEntryPassageBgGraphics)[0]) passageGraphics;
    struct ItemCollection* collectionEntry;
    u8* stageGraphicIndex;
    u8* stageGraphicMirror;
    u8* stageVariant;
    u8* sequenceFlag;
    u32 stageGraphic;
    s32 resetStart;
    s32 resetAddress;
    vu32* initialDma;
    s32 resetEnd;
    s32 searchIndex;
    s32 entryOffset;
    s16 scroll;
    u16* horizontalScroll;
    u32 graphicsDmaControl;
    u32 graphicsDestinationBase;
    register u32 graphicsStep PIN("r1");
    graphicsDestinationBase = 0x06005E80;
    InitializeVideoMemory();
    gStageEntryHorizontalScroll = sStageEntryNormalPassageScrollOffsets[gStageEntryStageVariant];
    if ((gCurrentPassage == PASSAGE_ENTRY) || (gCurrentPassage == PASSAGE_GOLDEN)) {
        gStageEntryHorizontalScroll = sStageEntryEntryPassageScrollOffsets[gStageEntryStageVariant];
    }
    gStageEntryMainSpriteState[0] = 0;
    gStageEntryMainSpriteState[1] = 0;
    if ((gCurrentPassage == PASSAGE_ENTRY) || (gCurrentPassage == PASSAGE_GOLDEN)) {
        stageGraphicIndex = &gStageEntryStageGraphicIndex;
        stageIndexTable = sStageEntryEntryStageGraphicIds;
    } else {
        stageGraphicIndex = &gStageEntryStageGraphicIndex;
        stageIndexTable = sStageEntryNormalStageGraphicIds;
    }
    {
        register u8* stageNumberPtr PIN("r3");
        stageNumberPtr = &gCurrentStageNumber;
        stageGraphic = stageIndexTable[*stageNumberPtr];
    }
    *stageGraphicIndex = stageGraphic;
    {
        register u8* stageGraphicStore PIN("r2");
        register u8* stageGraphicMirrorReg PIN("r6");
        stageGraphicStore = &gStageEntryStageGraphicMirror;
        *stageGraphicStore = stageGraphic;
        gStageEntryStageGraphicId = stageGraphic;
        ASM_BARRIER_VOLATILE(: "=r"(stageGraphicMirrorReg) : "0"(stageGraphicStore));
        stageGraphicMirror = stageGraphicMirrorReg;
    }
    gStageEntrySecondarySpriteState.timer = 0;
    gStageEntrySecondarySpriteState.frame = 0;
    ((u16*)(&gStageEntrySecondarySpriteState))[2] = (((*stageGraphicIndex) << 8) + 360) - gStageEntryHorizontalScroll;
    ((u16*)(&gStageEntrySecondarySpriteState))[3] = 136;
    gStageEntrySequenceStep = 0;
    gStageEntrySequenceTimer = 0;
    gStageEntryShakeTimer = 0;
    gStageEntryShakeMode = 0;
    gStageEntryKeyzerTargetState[2] = (((*stageGraphicMirror) << 8) + 396) - gStageEntryHorizontalScroll;
    gStageEntryKeyzerTargetState[3] = 68;
    gStageEntryKeyzerTargetState[4] = 64;
    gStageEntryPaletteFadeCounters[0] = 0;
    gStageEntryPaletteFadeCounters[1] = 0;
    gStageEntryPaletteFadeCounters[2] = 0;
    gStageEntryPaletteCycleOffset = 0;
    gStageEntryImpactEffectState[0] = 0;
    gStageEntryImpactEffectState[1] = 0;
    initialDma = (vu32*)0x040000D4;
    gStageEntryImpactEffectActive = 0;
    gStageEntryKeyzerFlightFinished = 0;
    horizontalScroll = (u16*)&gStageEntryHorizontalScroll;
    {
        register u8* selectedStage PIN("r4");
        register u8* sequenceStore PIN("r6");
        stageVariant = &gStageEntryStageVariant;
        selectedStage = &gStageEntrySelectedStage;
        {
            struct ItemCollection(*collection)[6];
            collection = gCurrentCollection;
            {
                u16* keyzerPosition;
                keyzerPosition = gStageEntryKeyzerPositionState;
                sequenceFlag = &gStageEntryKeyzerPaletteStep;
                passageGraphics = sStageEntryPassageBgGraphics;
                resetStart = (s32)gStageEntrySparkleFlags;
                {
                    u32 clearValue;
                    clearValue = 0;
                    resetEnd = resetStart + 20;
                    resetAddress = resetEnd;
                    do {
                        *((u32*)resetAddress) = clearValue;
                        resetAddress -= 4;
                    } while (resetAddress >= resetStart);
                }
                *selectedStage = 0;
                if ((gCurrentPassage == PASSAGE_ENTRY) || (gCurrentPassage == PASSAGE_GOLDEN)) {
                    if (((s32)(((u32)(*((u8*)(&collection[gCurrentPassage][0])))) << 26)) >= 0) {
                        *selectedStage = 1;
                    }
                } else {
                    for (searchIndex = 0; searchIndex <= 3; searchIndex++) {
                        entryOffset = (searchIndex * 4) + (gCurrentPassage * 24);
                        collectionEntry = (struct ItemCollection*)(entryOffset + ((s32)collection));
                        if (((s32)(((u32)(*((u8*)collectionEntry))) << 26)) >= 0) {
                            *selectedStage = searchIndex + 1;
                            break;
                        }
                    }
                }
                if (((s32)(((u32)(*((u8*)(&collection[gCurrentPassage][4])))) << 26)) < 0) {
                    *selectedStage = 5;
                }
                if ((gCurrentPassage == PASSAGE_ENTRY) || (gCurrentPassage == PASSAGE_GOLDEN)) {
                    if ((*selectedStage) == 5) {
                        keyzerPosition[2] = 732 - *horizontalScroll;
                    } else {
                        keyzerPosition[2] = (((*selectedStage) << 8) + 220) - *horizontalScroll;
                    }
                } else {
                    keyzerPosition[2] = (((*selectedStage) << 8) + 476) - *horizontalScroll;
                }
                {
                    u8 zero;
                    zero = 0;
                    keyzerPosition[3] = 142;
                    sequenceStore = sequenceFlag;
                    *sequenceStore = zero;
                }
            }
        }
        {
            vu32* dma1;
            dma1 = initialDma;
            dma1[0] = (u32)GP(passageGraphics[gCurrentPassage]);
            dma1[1] = 0x06000000;
            dma1[2] = 0x80002E00;
            dma1[2];
            while ((dma1[2] & 0x80000000) != 0) {
            }
        }
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)(sStageEntryPassageOverlayGraphics + (gCurrentPassage * 0x1400));
        ASM_BARRIER_VOLATILE(: : "r"(sequenceStore));
        dma[1] = 0x06005C00;
        dma[2] = 0x80000A00;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }

        {
            vu32* dma3;
            u32 dma3Scratch;
            dma3 = (vu32*)0x040000D4;
            dma3Scratch = (u32)sStageEntryCommonBgTiles;
            dma3[0] = dma3Scratch;
            dma3[1] = 0x06007000;
            dma3[2] = 0x80000400;
            dma3[2];
            dma3Scratch = 0x80000000;
            while ((dma3[2] & dma3Scratch) != 0) {
            }

            {
                vu32* dma4;
                dma4 = (vu32*)0x040000D4;
                dma4[0] = (u32)GP(sStageEntryPassagePalettes[gCurrentPassage]);
                ASM_BARRIER_VOLATILE(: : "r"(dma3Scratch));
                dma4[1] = 0x05000000;
                dma4[2] = 0x80000100;
                dma4[2];
                while ((dma4[2] & 0x80000000) != 0) {
                }
            }
        }

        if ((*selectedStage) != 5) {
            goto copyNormalStageGraphics;
        }
    }
    {
        s32 graphicsIndexBound;
        vu32* graphicsDma;
        const u8* graphicsSource;
        register u32 graphicsDestination PIN("r12");
        u32 graphicsControl;
        register u32 graphicsBusyMask PIN("r2");
        graphicsIndexBound = 0;
        graphicsDma = (vu32*)0x040000D4;
        {
            register u8* graphicsPassage PIN("r0");
            u32 graphicsPassageValue;
            graphicsPassage = &gCurrentPassage;
            graphicsPassageValue = *graphicsPassage;
            {
                u32 graphicsPassageOffset;
                register const u8* graphicsBase PIN("r1");
                graphicsPassageOffset = graphicsPassageValue * 5;
                graphicsPassageOffset <<= 10;
                graphicsBase = sStageEntryCompletedKeyzerGraphics;
                graphicsSource = (const u8*)((u32)graphicsPassageOffset + (u32)graphicsBase);
            }
        }
        {
            register u32 graphicsDestinationLoad PIN("r6");
            graphicsDestinationLoad = 0x06005E80;
            ASM_BARRIER_VOLATILE(: "=r"(graphicsDestination) : "0"(graphicsDestinationLoad));
        }
        graphicsControl = 0x80000050;
        do {
            graphicsStep = graphicsIndexBound << 10;
            graphicsDma[0] = (u32)(graphicsSource + graphicsStep);
            graphicsDma[1] = graphicsDestination + graphicsStep;
            graphicsDma[2] = graphicsControl;
            graphicsDma[2];
            graphicsBusyMask = 0x80000000;
            while ((graphicsDma[2] & graphicsBusyMask) != 0) {
            }
            graphicsStep = graphicsIndexBound + 1;
            graphicsIndexBound = graphicsStep;
        } while (graphicsIndexBound <= 4);
    }
    goto afterStageGraphics;
copyNormalStageGraphics: {
    u8* normalSelectedStage;
    ASM_BARRIER_VOLATILE(: "=r"(normalSelectedStage));
#ifdef PORTABLE
    normalSelectedStage = &gStageEntrySelectedStage;  // r4 holds selectedStage on the GBA
#endif
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)GP(sStageEntryStagePalettes[gCurrentPassage][(*normalSelectedStage) - 1]);
    {
        u32 normalOffset;
        normalOffset = (*normalSelectedStage) << 5;
        graphicsStep = 0x05000120;
        ASM_BARRIER_VOLATILE(: "+r"(graphicsStep));
        normalOffset += graphicsStep;
        dma[1] = normalOffset;
    }
    dma[2] = 0x80000010;
    dma[2];
    while ((dma[2] & 0x80000000) != 0) {
    }
}

afterStageGraphics:
    dma = (vu32*)&gCollectedKeyzer;
    if (*((u8*)dma) == 1) {
        dma = (vu32*)0x040000D4;
        {
            register u8* keyzerStagePtr PIN("r4");
            dma[0] = (u32)GP(sStageEntryStagePalettes[gCurrentPassage][*(keyzerStagePtr = &gCurrentStageNumber)]);
            dma[1] = 0x05000140 + ((*keyzerStagePtr) << 5);
            ASM_BARRIER_VOLATILE(: : "r"(keyzerStagePtr));
        }
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }

    graphicsDmaControl = 0x80000000;
    {
        vu32* dmaLarge;
        dmaLarge = (vu32*)0x040000D4;
        dmaLarge[0] = (u32)sStageEntryObjectTiles;
        dmaLarge[1] = 0x06010000;
        dmaLarge[2] = 0x80004000;
        dmaLarge[2];
        while ((dmaLarge[2] & graphicsDmaControl) != 0) {
        }
    }
    {
        vu32* dmaPalette;
        dmaPalette = (vu32*)0x040000D4;
        dmaPalette[0] = (u32)sStageEntryObjectPalette;
        dmaPalette[1] = 0x05000200;
        dmaPalette[2] = 0x80000100;
        dmaPalette[2];
        while ((dmaPalette[2] & 0x80000000) != 0) {
        }
    }
    scroll = GetStageEntryWrappedScrollValue(horizontalScroll);
    if (scroll <= 255) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(sStageEntryBg0TilemapChunkTable[(gCurrentPassage * 7) + (*stageVariant)]);
        dma[1] = 0x0600C000;
        dma[2] = 0x80000400;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }

        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(sStageEntryBg1TilemapChunkTable[(gCurrentPassage * 7) + (*stageVariant)]);
        dma[1] = 0x0600D000;
        dma[2] = 0x80000400;
        dma[2];
    } else {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(sStageEntryBg0TilemapChunkTable[(gCurrentPassage * 7) + (*stageVariant)]);
        dma[1] = 0x0600C800;
        dma[2] = 0x80000400;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }

        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(sStageEntryBg1TilemapChunkTable[(gCurrentPassage * 7) + (*stageVariant)]);
        dma[1] = 0x0600D800;
        dma[2] = 0x80000400;
        dma[2];
    }
    LoadStageEntryTilemapChunks();
    *((vu16*)0x04000008) = 0x5802;
    *((vu16*)0x0400000A) = 0x5A01;
    gStageEntryIdleAnimationVariant = 0;
    gStageEntryShowCollectionSparkle = 0;
    gStageEntryBlinkVisible = 0;
    gStageEntryBlinkActive = 0;
    gStageEntryBlinkCounter = 0;
    gStageEntryExitRequested = 0;
    gStageEntryMainSpriteFalling = 0;
    InitMinigameScoreDisplay();
    *((vu16*)0x04000012) = 8;
    *((vu16*)(resetStart = 0x04000010)) = GetStageEntryWrappedScrollValue((u16*)&gStageEntryHorizontalScroll);
    *((vu16*)0x04000016) = 8;
    *((vu16*)0x04000014) = GetStageEntryWrappedScrollValue((u16*)&gStageEntryHorizontalScroll);
    *((vu16*)0x04000054) = 0;
    *((vu16*)0x04000000) = 0x1300;
    *((vu16*)0x04000000) |= 0x2000;
    *((vu16*)0x04000048) = 0x3F;
    *((vu16*)0x0400004A) = 0;
    *((vu16*)0x04000050) = 0;
}

void InitializeStageEntryState(void)
{
    s32 sparkleIndex;

    gStageEntrySoftResetDisabled = 0;
    gStageEntryPauseMenuDisabled = 0;
    gPauseFlag = 0;
    gStageEntryStageVariant = gCurrentStageNumber;
    m4aSongNumStart(0x27F);
    m4aSongNumStart(462);
    VoiceSetPlay(6);
    InitializeStageEntrySceneGraphics();

    gStageEntryWindowLeft = 0;
    gStageEntryWindowRight = 240;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 160;
    gStageEntryMainSpriteState[7] = 16;
    gStageEntryMainSpriteState[6] = 0;
    gStageEntryUseAffineMainSprite = 1;
    gStageEntryMainSpriteState[2] = gStageEntryKeyzerTargetState[2];
    gStageEntryMainSpriteState[3] = gStageEntryKeyzerTargetState[3] + 16;
    gStageEntryVerticalVelocity = 0;
    gStageEntryKeyzerTargetState[4] = 256;

    if (gCollectedKeyzer == 1) {
        gStageEntryCompanionKeyzerState.timer = 0;
        gStageEntryCompanionKeyzerState.frame = 0;
        gStageEntryCompanionKeyzerState.x = 220;
        gStageEntryCompanionKeyzerState.y = 142;
        GP_SET(gStageEntryCompanionKeyzerAnimation, sStageEntryKeyzerCompanionAnimation);

        gStageEntryCompanionSpriteState.x = sStageEntryKeyzerXPositions[0];
        gStageEntryCompanionSpriteState.y = sStageEntryKeyzerYPositions[0];
        GP_SET(gStageEntryCompanionSpriteState.data, sStageEntryKeyzerSpriteData);
        gStageEntryCompanionSpriteState.attribute = 0;

        gStageEntryFlyingKeyzerState.x = gStageEntryKeyzerTargetState[2];
        gStageEntryFlyingKeyzerState.y = gStageEntryKeyzerTargetState[3];
        gStageEntryFlyingKeyzerState.frame = 0;
        gStageEntryFlyingKeyzerState.timer = 0;
        gStageEntryFlyingKeyzerState.attribute = 64;
        GP_SET(gStageEntryFlyingKeyzerAnimation, sStageEntryKeyzerFlightAnimation);

        gStageEntrySparkleGroupState.frame = 0;
        gStageEntrySparkleGroupState.timer = 0;
        for (sparkleIndex = 0; sparkleIndex < 6; sparkleIndex++) {
            gStageEntrySparkleAnimStates[sparkleIndex].frame = 0;
            gStageEntrySparkleAnimStates[sparkleIndex].timer = 0;
        }
        gStageEntrySequenceMode = 5;
    } else {
        gStageEntrySequenceMode = 4;
    }

    GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerMainAnimation);
    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryAnimation);
}

void InitializeStageEntryLandingState(void)
{
    gStageEntrySoftResetDisabled = 0;
    gStageEntryPauseMenuDisabled = 0;
    gPauseFlag = 0;
    gStageEntryStageVariant = gCurrentStageNumber;
    m4aSongNumStart(0x27F);
    InitializeStageEntrySceneGraphics();
    gStageEntryWindowLeft = 0;
    gStageEntryWindowRight = 240;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 160;
    gStageEntryMainSpriteFalling = 1;
    gStageEntryMainSpriteState[7] = 256;
    gStageEntryMainSpriteState[6] = 0;
    gStageEntryUseAffineMainSprite = 0;
    gStageEntryMainSpriteState[2] = 148;
    gStageEntryMainSpriteState[3] = 0;
    gStageEntryVerticalVelocity = 0;
    if (gStageExitType == 1) {
        m4aSongNumStart(0x1C7);
    }
    gStageEntrySequenceMode = 2;
    GP_SET(gStageEntryMainAnimation, sStageEntryLandingIntroAnimation);
    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryIdleAnimation);
}

void InitializeStageEntryKeyzerArrivalState(void)
{
    gStageEntrySoftResetDisabled = 0;
    gStageEntryPauseMenuDisabled = 0;
    gPauseFlag = 0;
    gStageEntryStageVariant = gCurrentStageNumber;
    InitializeStageEntrySceneGraphics();
    gStageEntryWindowLeft = 120;
    gStageEntryWindowRight = 120;
    gStageEntryWindowTop = 80;
    gStageEntryWindowBottom = 80;
    gStageEntryMainSpriteState[2] = 56;
    gStageEntryMainSpriteState[3] = 136;
    gStageEntryMainSpriteState[7] = 256;
    gStageEntryMainSpriteState[6] = 0;
    gStageEntryUseAffineMainSprite = 0;
    gStageEntrySequenceMode = 1;
    GP_SET(gStageEntryMainAnimation, GP(sStageEntryIdleAnimationsByVariant[gStageEntryIdleAnimationVariant]));
    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryIdleAnimation);
}

void InitializeStageEntryMovingLandingState(void)
{
    gStageEntrySoftResetDisabled = 0;
    gStageEntryPauseMenuDisabled = 0;
    gPauseFlag = 0;
    gStageEntryStageVariant = gCurrentStageNumber;
    m4aSongNumStart(0x27F);
    InitializeStageEntrySceneGraphics();
    gStageEntryWindowLeft = 0;
    gStageEntryWindowRight = 240;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 160;
    gStageEntryMainSpriteFalling = 1;
    gStageEntryMainSpriteState[7] = 256;
    gStageEntryMainSpriteState[6] = 0;
    gStageEntryUseAffineMainSprite = 0;
    gStageEntryMainSpriteState[2] = 240;
    gStageEntryMainSpriteState[3] = 132;
    gStageEntryHorizontalVelocity = -70;
    gStageEntryVerticalVelocity = -50;
    gStageEntrySequenceMode = 3;
    GP_SET(gStageEntryMainAnimation, sStageEntryMovingLandingIntroAnimation);
    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryIdleAnimation);
}

void LoadStageEntryTilemapChunks(void)
{
    GBA_TYPEOF(const u8* const*, &(sStageEntryBg1TilemapChunkTable)[0]) table;
    u8* stage;
    u8* passage;
    u32 control;

    if (gStageEntryHorizontalScroll % 512 <= 255) {
        vu32* dma;

        dma = (vu32*)0x040000D4;
        table = GBA_AS(table, sStageEntryBg0TilemapChunkTable);
        stage = &gCurrentStageNumber;
        passage = &gCurrentPassage;
        dma[0] = (u32)GP(table[*passage * 7 + *stage]);
        dma[1] = 0x0600C800;
        control = 0x80000400;
        dma[2] = control;
        dma[2];
        table = sStageEntryBg1TilemapChunkTable;
        dma[0] = (u32)GP(table[*passage * 7 + *stage]);
        dma[1] = 0x0600D800;
        dma[2] = control;
        dma[2];
    } else {
        vu32* dma;

        dma = (vu32*)0x040000D4;
        table = GBA_AS(table, sStageEntryBg0TilemapChunkTable);
        stage = &gCurrentStageNumber;
        passage = &gCurrentPassage;
        dma[0] = (u32)GP(table[*passage * 7 + *stage]);
        dma[1] = 0x0600C000;
        control = 0x80000400;
        dma[2] = control;
        dma[2];
        table = sStageEntryBg1TilemapChunkTable;
        dma[0] = (u32)GP(table[*passage * 7 + *stage]);
        dma[1] = 0x0600D000;
        dma[2] = control;
        dma[2];
    }
}

s32 UpdateStageEntryStateMachine(void)
{
    s32 result;

    result = 0;
    switch (gStageEntrySequenceMode) {
        case 1:
            result = UpdateStageEntryKeyzerSequence();
            break;
        case 2:
            result = UpdateStageEntryLandingSequence();
            break;
        case 3:
            result = UpdateStageEntryMovingLandingSequence();
            break;
        case 4:
            result = UpdateStageEntryNoKeyzerSequence();
            break;
        case 5:
            result = UpdateStageEntryWithKeyzerSequence();
            break;
    }

    if (result == 6) {
        return 1;
    }
    return 0;
}

s32 UpdateStageEntryKeyzerSequence(void)
{
    switch (gStageEntrySequenceStep) {
        case 0:
            if (ExpandStageEntryWindow() != 0) {
                gStageEntrySequenceStep++;
            }
            break;

        case 1:
            gStageEntryMainSpriteState[2] += sStageEntryKeyzerApproachXOffsets[gStageEntrySequenceTimer];
            gStageEntryMainSpriteState[3] += sStageEntryKeyzerApproachYOffsets[gStageEntrySequenceTimer];
            gStageEntrySequenceTimer++;
            GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerMovingAnimation);
            if (gStageEntrySequenceTimer == 23) {
                gStageEntrySequenceStep++;
                gStageEntrySequenceTimer = 0;
            }
            break;

        case 2:
            gStageEntryMainSpriteState[2] += sStageEntryKeyzerImpactXOffsets[gStageEntrySequenceTimer];
            gStageEntryMainSpriteState[3] += sStageEntryKeyzerImpactYOffsets[gStageEntrySequenceTimer];
            GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerImpactAnimation);
            if (gStageEntrySequenceTimer == 8) {
                VoiceSetPlay(3);
                m4aSongNumStart(218);
                gStageEntryImpactEffectState[1] = 0;
                gStageEntryImpactEffectState[0] = 0;
                gStageEntryImpactEffectActive = 1;
                gStageEntrySequenceStep++;
                gStageEntrySequenceTimer = 0;
            } else {
                gStageEntrySequenceTimer++;
            }
            break;

        case 3:
            GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerRiseAnimation);
            gStageEntryMainSpriteState[3] += sStageEntryKeyzerRiseYOffsets[gStageEntrySequenceTimer];
            gStageEntrySecondarySpriteState.timer++;
            GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryIntroAnimation);
            if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time <
                gStageEntrySecondarySpriteState.timer) {
                gStageEntrySecondarySpriteState.timer = 1;
                gStageEntrySecondarySpriteState.frame++;
                if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time == 0) {
                    gStageEntrySecondarySpriteState.frame = 0;
                    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryLoopAnimation);
                    gStageEntrySequenceStep++;
                    gStageEntrySequenceTimer = 0;
                    break;
                }
            }
            gStageEntrySequenceTimer++;
            break;

        case 4:
            gStageEntryMainSpriteState[3] += sStageEntryKeyzerFloatYOffsets[gStageEntrySequenceTimer];
            gStageEntryKeyzerTargetState[4] += 3;
            if (gStageEntryKeyzerTargetState[4] > 256) {
                gStageEntryKeyzerTargetState[4] = 256;
            }
            gStageEntrySecondarySpriteState.timer++;
            GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryLoopAnimation);
            if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time <
                gStageEntrySecondarySpriteState.timer) {
                gStageEntrySecondarySpriteState.timer = 1;
                gStageEntrySecondarySpriteState.frame++;
                if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time == 0) {
                    gStageEntrySecondarySpriteState.frame = 0;
                    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryAnimation);
                    gStageEntrySequenceStep++;
                    gStageEntrySequenceTimer = 0;
                    break;
                }
            }
            gStageEntrySequenceTimer++;
            break;

        case 5:
            gStageEntryMainSpriteState[2] += sStageEntryKeyzerExitXOffsets[gStageEntrySequenceTimer];
            gStageEntryMainSpriteState[3] += sStageEntryKeyzerExitYOffsets[gStageEntrySequenceTimer];
            GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerMovingAnimation);
            if (gStageEntrySequenceTimer == 11) {
                gStageEntrySequenceStep++;
                gStageEntrySequenceTimer = 0;
            } else {
                gStageEntrySequenceTimer++;
            }
            break;

        case 6:
            gStageEntryMainSpriteState[2] += sStageEntryKeyzerFinalXOffsets[gStageEntrySequenceTimer];
            gStageEntryMainSpriteState[3] += sStageEntryKeyzerFinalYOffsets[gStageEntrySequenceTimer];
            GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerImpactAnimation);
            if (gStageEntrySequenceTimer == 7) {
                gStageEntrySequenceStep++;
                gStageEntrySequenceTimer = 0;
            } else {
                gStageEntrySequenceTimer++;
            }
            break;

        case 7:
            gStageEntryUseAffineMainSprite = 1;
            GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerMainAnimation);
            gStageEntryMainSpriteState[2] = gStageEntryKeyzerTargetState[2];
            gStageEntryMainSpriteState[3] = gStageEntryKeyzerTargetState[3] + 16;
            gStageEntryMainSpriteState[6] = (gStageEntryMainSpriteState[6] + 248) & 0xFF;
            gStageEntryMainSpriteState[7] -= 4;
            if (gStageEntryMainSpriteState[7] <= 15) {
                return 6;
            }
            break;
    }

    return 0;
}

s32 UpdateStageEntryLandingSequence(void)
{
    vu16* blinkTimer;
    u32 value;
    u16 mask;

    switch (gStageEntrySequenceStep) {
        case 0:
            gStageEntryMainSpriteState[2] += -1;
            gStageEntryVerticalVelocity += 4;
            gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
            if (gStageEntryMainSpriteState[3] > 136) {
                m4aSongNumStart(446);
                gStageEntryImpactEffectState[1] = 0;
                gStageEntryImpactEffectState[0] = 0;
                gStageEntryImpactEffectActive = 1;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntryVerticalVelocity = -60;
                gStageEntryShakeMode = 1;
                gStageEntrySequenceStep++;
            }
            break;

        case 1:
            gStageEntryMainSpriteState[2] += -1;
            gStageEntryVerticalVelocity += 4;
            gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
            if (gStageEntryMainSpriteState[3] > 136) {
                gStageEntryImpactEffectState[1] = 0;
                gStageEntryImpactEffectState[0] = 0;
                gStageEntryImpactEffectActive = 1;
                gStageEntryVerticalVelocity = -30;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntryShakeMode = 1;
                gStageEntryShakeTimer = 0;
                gStageEntrySequenceStep++;
            }
            break;

        case 2:
            gStageEntryMainSpriteState[2] += -1;
            gStageEntryVerticalVelocity += 4;
            gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
            if (gStageEntryMainSpriteState[3] > 136) {
                gStageEntryImpactEffectState[1] = 0;
                gStageEntryImpactEffectState[0] = 0;
                gStageEntryImpactEffectActive = 1;
                gStageEntryMainSpriteState[2] = 56;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntryMainSpriteFalling = 0;
                GP_SET(gStageEntryMainAnimation, sStageEntryLandingAnimation);
                gStageEntrySequenceStep++;
                gStageEntryBlinkVisible = 1;
            }
            break;

        case 3:
            gStageEntryBlinkActive = 1;
            blinkTimer = &gStageEntrySequenceTimer;
            (*blinkTimer)++;
            value = *blinkTimer;
            mask = value & 3;
            if (mask <= 1) {
                gStageEntryBlinkVisible = 1;
            } else {
                gStageEntryBlinkVisible = 0;
            }
            if (*blinkTimer > 45) {
                gStageEntryBlinkVisible = 0;
                gStageEntrySequenceStep++;
                m4aSongNumStart(221);
            }
            break;

        case 4:
            if (CollapseStageEntryWindow() != 0) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                gStageEntryExitRequested = 1;
                return 6;
            }
            break;
    }

    return 0;
}

s32 UpdateStageEntryMovingLandingSequence(void)
{
    vu16* blinkTimer;
    u32 value;
    u16 mask;

    switch (gStageEntrySequenceStep) {
        case 0:
            gStageEntryHorizontalVelocity++;
            if (gStageEntryHorizontalVelocity >= -32) {
                gStageEntryHorizontalVelocity = -32;
            }
            gStageEntryMainSpriteState[2] += gStageEntryHorizontalVelocity >> 4;
            gStageEntryVerticalVelocity += 4;
            gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
            if (gStageEntryMainSpriteState[3] > 136) {
                m4aSongNumStart(446);
                gStageEntryImpactEffectState[1] = 0;
                gStageEntryImpactEffectState[0] = 0;
                gStageEntryImpactEffectActive = 1;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntryVerticalVelocity = -30;
                gStageEntryShakeMode = 1;
                gStageEntrySequenceStep++;
            }
            break;

        case 1:
            gStageEntryHorizontalVelocity++;
            if (gStageEntryHorizontalVelocity >= -32) {
                gStageEntryHorizontalVelocity = -32;
            }
            gStageEntryMainSpriteState[2] += gStageEntryHorizontalVelocity >> 4;
            gStageEntryVerticalVelocity += 4;
            gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
            if (gStageEntryMainSpriteState[3] > 136) {
                gStageEntryImpactEffectState[0] = (gStageEntryImpactEffectState[1] = 0);
                gStageEntryImpactEffectActive = 1;
                gStageEntryShakeTimer = (gStageEntryShakeMode = 1) - 1;
                gStageEntryVerticalVelocity = -20;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntrySequenceStep++;
            }
            break;

        case 2:
            gStageEntryHorizontalVelocity++;
            if (gStageEntryHorizontalVelocity >= -32) {
                gStageEntryHorizontalVelocity = -32;
            }
            gStageEntryMainSpriteState[2] += gStageEntryHorizontalVelocity >> 4;
            gStageEntryVerticalVelocity += 4;
            gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
            if (gStageEntryMainSpriteState[3] > 136) {
                gStageEntryImpactEffectState[1] = 0;
                gStageEntryImpactEffectState[0] = 0;
                gStageEntryImpactEffectActive = 1;
                gStageEntryMainSpriteState[2] = 56;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntryMainSpriteFalling = 0;
                GP_SET(gStageEntryMainAnimation, sStageEntryLandingAnimation);
                gStageEntrySequenceStep++;
                gStageEntryBlinkVisible = 1;
            }
            break;

        case 3:
            gStageEntryBlinkActive = 1;
            blinkTimer = &gStageEntrySequenceTimer;
            (*blinkTimer)++;
            value = *blinkTimer;
            mask = value & 3;
            if (mask <= 1) {
                gStageEntryBlinkVisible = 1;
            } else {
                gStageEntryBlinkVisible = 0;
            }
            if (*blinkTimer > 45) {
                gStageEntryBlinkVisible = 0;
                gStageEntrySequenceStep++;
                m4aSongNumStart(221);
            }
            break;

        case 4:
            if (CollapseStageEntryWindow() != 0) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                gStageEntryExitRequested = 1;
                return 6;
            }
            break;
    }

    return 0;
}

s32 UpdateStageEntryNoKeyzerSequence(void)
{
    switch (gStageEntrySequenceStep) {
        case 0:
            gStageEntryMainSpriteState[6] = (gStageEntryMainSpriteState[6] + 248) & 0xFF;
            gStageEntryMainSpriteState[7] += 4;
            if (gStageEntryMainSpriteState[7] > 256) {
                gStageEntrySequenceStep++;
                gStageEntryMainSpriteState[7] = 256;
                gStageEntryUseAffineMainSprite = 0;
                gStageEntryMainSpriteFalling = 1;
                GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerMovingAnimation);
                GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryFallAnimation);
                gStageEntryVerticalVelocity = -20;
            }
            break;

        case 1:
            gStageEntryVerticalVelocity += 4;
            if (gStageEntryVerticalVelocity > 0) {
                GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerImpactAnimation);
            }
            gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
            gStageEntryMainSpriteState[2] += -3;
            if (gStageEntryMainSpriteState[3] > 136) {
                gStageEntryImpactEffectState[1] = 0;
                gStageEntryImpactEffectState[0] = 0;
                gStageEntryImpactEffectActive = 1;
                gStageEntryVerticalVelocity = 0;
                gStageEntryMainSpriteFalling = 0;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntryMainSpriteState[2] = 56;
                GP_SET(gStageEntryMainAnimation, sStageEntryLandingAnimation);
                gStageEntrySequenceStep++;
            }
            gStageEntrySecondarySpriteState.timer++;
            if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time <
                gStageEntrySecondarySpriteState.timer) {
                gStageEntrySecondarySpriteState.timer = 1;
                gStageEntrySecondarySpriteState.frame++;
            }
            gStageEntryKeyzerTargetState[4] -= 2;
            if (gStageEntryKeyzerTargetState[4] <= 63) {
                gStageEntryKeyzerTargetState[4] = 64;
            }
            break;

        case 2:
            gStageEntrySecondarySpriteState.timer++;
            if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time <
                gStageEntrySecondarySpriteState.timer) {
                gStageEntrySecondarySpriteState.timer = 1;
                gStageEntrySecondarySpriteState.frame++;
                if (gStageEntrySecondarySpriteState.frame == 8) {
                    if (gCurrentCollection[gCurrentPassage][gCurrentStageNumber].keyzer) {
                        m4aSongNumStart(447);
                        GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerFoundAnimation);
                        gStageEntryShowCollectionSparkle = 1;
                        gStageEntrySequenceStep++;
                    } else {
                        gStageEntrySequenceStep += 2;
                    }
                }
            }
            gStageEntryKeyzerTargetState[4] -= 2;
            if (gStageEntryKeyzerTargetState[4] <= 63) {
                gStageEntryKeyzerTargetState[4] = 64;
            }
            break;

        case 3:
            gStageEntryMainSpriteState[0]++;
            if (GP(gStageEntryMainAnimation)[gStageEntryMainSpriteState[1]].time < gStageEntryMainSpriteState[0]) {
                gStageEntryMainSpriteState[1]++;
                if (GP(gStageEntryMainAnimation)[gStageEntryMainSpriteState[1]].time == 0) {
                    gStageEntryMainSpriteState[0] = 1;
                    gStageEntryMainSpriteState[1] = 0;
                    gStageEntryShowCollectionSparkle = 0;
                    gStageEntrySequenceStep = 5;
                    GP_SET(gStageEntryMainAnimation, GP(sStageEntryIdleAnimationsByVariant[gStageEntryIdleAnimationVariant]));
                }
            }
            gStageEntrySecondarySpriteState.timer++;
            if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time <
                gStageEntrySecondarySpriteState.timer) {
                gStageEntrySecondarySpriteState.timer = 1;
                gStageEntrySecondarySpriteState.frame++;
                if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time == 0) {
                    gStageEntrySecondarySpriteState.frame = 0;
                    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryIdleAnimation);
                }
            }
            gStageEntryKeyzerTargetState[4] -= 2;
            if (gStageEntryKeyzerTargetState[4] <= 63) {
                gStageEntryKeyzerTargetState[4] = 64;
            }
            break;

        case 4:
            gStageEntrySecondarySpriteState.timer++;
            if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time <
                gStageEntrySecondarySpriteState.timer) {
                gStageEntrySecondarySpriteState.timer = 1;
                gStageEntrySecondarySpriteState.frame++;
                if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time == 0) {
                    gStageEntrySecondarySpriteState.frame = 0;
                    GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryIdleAnimation);
                    gStageEntrySequenceStep++;
                    GP_SET(gStageEntryMainAnimation, GP(sStageEntryIdleAnimationsByVariant[gStageEntryIdleAnimationVariant]));
                }
            }
            gStageEntryKeyzerTargetState[4] -= 2;
            if (gStageEntryKeyzerTargetState[4] <= 63) {
                gStageEntryKeyzerTargetState[4] = 64;
            }
            break;

        case 5:
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer > 45) {
                m4aSongNumStart(221);
                gStageEntrySequenceStep++;
            }
            break;

        case 6:
            if (CollapseStageEntryWindow() != 0) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                gStageEntryExitRequested = 1;
                return 6;
            }
            break;
    }

    return 0;
}

s32 UpdateStageEntryWithKeyzerSequence(void)
{
    vu32* dma;
    s32 sparkleIndex;
    s32 triggerIndex;
    u32 paletteStepLimit;
    u16 keyzerRecoveryTimer;

    switch (gStageEntrySequenceStep) {
        case 0:
            gStageEntryMainSpriteState[6] = (gStageEntryMainSpriteState[6] + 248) & 0xFF;
            gStageEntryMainSpriteState[7] += 4;
            if (gStageEntryMainSpriteState[7] > 256) {
                gStageEntrySequenceStep++;
                gStageEntryMainSpriteState[7] = 256;
                gStageEntryUseAffineMainSprite = 0;
                gStageEntryMainSpriteFalling = 1;
                GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerMovingAnimation);
                GP_SET(gStageEntrySecondaryAnimation, sStageEntrySecondaryFallAnimation);
                gStageEntryVerticalVelocity = -20;
            }
            break;

        case 1:
            if (gStageEntryMainSpriteFalling != 0) {
                gStageEntryVerticalVelocity += 4;
                if (gStageEntryVerticalVelocity > 0) {
                    GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerImpactAnimation);
                }
                gStageEntryMainSpriteState[3] += gStageEntryVerticalVelocity >> 4;
                gStageEntryMainSpriteState[2] += -3;
            }
            if (gStageEntryMainSpriteState[3] > 136 && gStageEntryMainSpriteFalling != 0) {
                if (gStageEntryImpactEffectActive == 0) {
                    gStageEntryImpactEffectState[1] = 0;
                    gStageEntryImpactEffectState[0] = 0;
                    gStageEntryImpactEffectActive = 1;
                }
                gStageEntryMainSpriteFalling = 0;
                gStageEntryMainSpriteState[3] = 136;
                gStageEntryMainSpriteState[2] = 56;
                GP_SET(gStageEntryMainAnimation, sStageEntryLandingAnimation);
            }
            UpdateStageEntrySecondaryAnimationAndScale();
            UpdateStageEntryFlyingKeyzerAnimation();
            if (gStageEntrySequenceTimer > 39) {
                GP_SET(gStageEntryFlyingKeyzerAnimation, sStageEntryFlyingKeyzerEscapeAnimation);
                gStageEntryFlyingKeyzerState.attribute = 0;
                m4aSongNumStart(448);
                gStageEntrySequenceStep++;
                gStageEntrySequenceTimer = 0;
            } else {
                gStageEntryFlyingKeyzerState.attribute += 248;
                gStageEntryFlyingKeyzerState.x += sStageEntryFlyingKeyzerApproachXOffsets[gStageEntrySequenceTimer];
                gStageEntryFlyingKeyzerState.y += sStageEntryFlyingKeyzerApproachYOffsets[gStageEntrySequenceTimer];
                gStageEntrySequenceTimer++;
            }
            break;

        case 2:
            UpdateStageEntrySecondaryAnimationAndScale();
            if (gStageEntrySequenceTimer <= 143) {
                gStageEntryFlyingKeyzerState.x = sStageEntryFlyingKeyzerScriptXPositions[gStageEntrySequenceTimer];
                gStageEntryFlyingKeyzerState.y = sStageEntryFlyingKeyzerScriptYPositions[gStageEntrySequenceTimer];
            } else {
                gStageEntryFlyingKeyzerState.y = 200;
            }
            if (gStageEntrySequenceTimer == 43) {
                GP_SET(gStageEntryFlyingKeyzerAnimation, sStageEntryFlyingKeyzerSpinAnimation);
                gStageEntryFlyingKeyzerState.frame = 0;
                gStageEntryFlyingKeyzerState.timer = 0;
            }
            UpdateStageEntryFlyingKeyzerAnimation();
            if (gStageEntrySequenceTimer == 142) {
                gStageEntryKeyzerFlightFinished = 1;
            }
            if (gStageEntrySequenceTimer == 177) {
                gStageEntryShakeMode = 2;
            }
            if (gStageEntrySequenceTimer == 356) {
                gStageEntryShakeMode = 3;
            }
            if (gStageEntrySequenceTimer == 389) {
                gStageEntryShakeMode = 0;
            }
            if (gStageEntrySequenceTimer <= 277) {
                gStageEntryCompanionSpriteState.x = sStageEntryKeyzerXPositions[gStageEntrySequenceTimer];
                gStageEntryCompanionSpriteState.y = sStageEntryKeyzerYPositions[gStageEntrySequenceTimer];
                GP_SET(gStageEntryCompanionSpriteState.data, GP(sStageEntryCompanionSpriteDataByFrame
                    [sStageEntryCompanionSpriteFrameIndices[gStageEntrySequenceTimer]]));
            } else {
                gStageEntryCompanionSpriteState.y = 200;
            }
            if ((u16)(gStageEntrySequenceTimer - 199) <= 29) {
                gStageEntryCompanionSpriteState.attribute += 255;
            }
            sparkleIndex = 0;
            while (sparkleIndex <= 5) {
                triggerIndex = 0;
                while (triggerIndex <= 1) {
                    if (gStageEntrySequenceTimer == sStageEntrySparkleTriggerFrames[sparkleIndex][triggerIndex]) {
                        gStageEntrySparkleFlags[sparkleIndex] = 1;
                    }
                    triggerIndex++;
                }
                sparkleIndex++;
            }
            if (gStageEntrySequenceTimer == 259) {
                gStageEntryCompanionKeyzerState.frame = 0;
                gStageEntryCompanionKeyzerState.timer = 0;
                GP_SET(gStageEntryCompanionKeyzerAnimation, sStageEntryCompanionKeyzerRescueAnimation);
                m4aSongNumStart(457);
            }
            if (UpdateStageEntryCompanionKeyzerAnimation() != 0 && gStageEntrySequenceTimer > 259) {
                GP_SET(gStageEntryMainAnimation, sStageEntryKeyzerRewardAnimation);
                gStageEntryShowCollectionSparkle = 1;
                gStageEntryMainSpriteState[1] = 0;
                gStageEntryMainSpriteState[0] = 0;
                gStageEntryCompanionKeyzerState.x = 300;
                gStageEntrySequenceStep++;
                gStageEntryShakeMode = 0;
                gStageEntrySequenceTimer = 0;
            } else {
                gStageEntrySequenceTimer++;
            }
            break;

        case 3:
            keyzerRecoveryTimer = gStageEntrySequenceTimer;
            paletteStepLimit = 7;
            if ((keyzerRecoveryTimer & paletteStepLimit) == 0) {
                gStageEntryKeyzerPaletteStep++;
            }
            if (gStageEntryKeyzerPaletteStep > paletteStepLimit) {
                gStageEntryKeyzerPaletteStep = paletteStepLimit;
                BackupPalettesForColorFade();
            }
            dma = (vu32*)0x040000D4;
            dma[0] = (u32)(GP(sStageEntryStagePalettes[gCurrentPassage][gCurrentStageNumber]) +
                           (gStageEntryKeyzerPaletteStep << 5));
            dma[1] = 0x05000140 + (gCurrentStageNumber << 5);
            dma[2] = 0x80000010;
            dma[2];
            gStageEntrySequenceTimer++;
            gStageEntryMainSpriteState[0]++;
            if (GP(gStageEntryMainAnimation)[gStageEntryMainSpriteState[1]].time < gStageEntryMainSpriteState[0]) {
                gStageEntryMainSpriteState[0] = 1;
                gStageEntryMainSpriteState[1]++;
                if (GP(gStageEntryMainAnimation)[gStageEntryMainSpriteState[1]].time == 0) {
                    gStageEntryMainSpriteState[1] = 0;
                    gStageEntryShowCollectionSparkle = 0;
                    GP_SET(gStageEntryMainAnimation, GP(sStageEntryIdleAnimationsByVariant[gStageEntryIdleAnimationVariant]));
                    gStageEntrySequenceStep++;
                    m4aSongNumStart(221);
                }
            }
            break;

        case 4:
            if (CollapseStageEntryWindow() != 0) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                gStageEntryExitRequested = 1;
                return 6;
            }
            break;
    }

    return 0;
}

s32 UpdateStageEntryFlyingKeyzerAnimation(void)
{
    gStageEntryFlyingKeyzerState.timer++;
    if (GP(gStageEntryFlyingKeyzerAnimation)[gStageEntryFlyingKeyzerState.frame].time <
        gStageEntryFlyingKeyzerState.timer) {
        gStageEntryFlyingKeyzerState.timer = 1;
        gStageEntryFlyingKeyzerState.frame++;
        if (GP(gStageEntryFlyingKeyzerAnimation)[gStageEntryFlyingKeyzerState.frame].time == 0) {
            gStageEntryFlyingKeyzerState.frame = 0;
            return 1;
        }
    }
    return 0;
}

s32 UpdateStageEntryCompanionKeyzerAnimation(void)
{
    gStageEntryCompanionKeyzerState.timer++;
    if (GP(gStageEntryCompanionKeyzerAnimation)[gStageEntryCompanionKeyzerState.frame].time <
        gStageEntryCompanionKeyzerState.timer) {
        gStageEntryCompanionKeyzerState.timer = 1;
        gStageEntryCompanionKeyzerState.frame++;
        if (GP(gStageEntryCompanionKeyzerAnimation)[gStageEntryCompanionKeyzerState.frame].time == 0) {
            gStageEntryCompanionKeyzerState.frame = 0;
            return 1;
        }
    }
    return 0;
}

void UpdateStageEntrySecondaryAnimationAndScale(void)
{
    s32 value;

    gStageEntrySecondarySpriteState.timer++;
    if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time <
        gStageEntrySecondarySpriteState.timer) {
        gStageEntrySecondarySpriteState.timer = 1;
        gStageEntrySecondarySpriteState.frame++;
        if (GP(gStageEntrySecondaryAnimation)[gStageEntrySecondarySpriteState.frame].time == 0) {
            gStageEntrySecondarySpriteState.frame = 0;
            GP_SET(gStageEntrySecondaryAnimation, (const struct AnimationFrame*)sStageEntrySecondaryIdleAnimation);
        }
    }

    value = gStageEntryKeyzerTargetState[4] - 2;
    gStageEntryKeyzerTargetState[4] = value;
    if ((u16)value <= 63) {
        gStageEntryKeyzerTargetState[4] = 64;
    }
}

void UpdateStageEntryPaletteFadeCounters(void)
{
    gStageEntryPaletteFadeCounters[0]--;
    if ((gMainTimer & 1) != 0) {
        gStageEntryPaletteFadeCounters[1]--;
    }
    if ((gMainTimer & 3) == 0) {
        gStageEntryPaletteFadeCounters[2]--;
    }
}

void CycleStageEntryPalette(void)
{
    vu32* dma;

    if ((gMainTimer & 7) == 0) {
        gStageEntryPaletteCycleOffset = (gStageEntryPaletteCycleOffset + 1) & 31;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)(sStageEntryPaletteCycleTable + (gStageEntryPaletteCycleOffset << 5));
        dma[1] = 0x05000300;
        dma[2] = 0x80000010;
        dma[2];
    }
}

void UpdateStageEntryBlinkState(void)
{
    u8* counter;
    u32 value;

    value = gStageEntryBlinkCounter;
    value &= 3;
    counter = &gStageEntryBlinkCounter;
    if (value <= 1) {
        gStageEntryBlinkVisible = 1;
    } else {
        gStageEntryBlinkVisible = 0;
    }

    (*counter)++;
    if (*counter == 60) {
        *counter = 0;
        gStageEntryBlinkVisible = 0;
        gStageEntryBlinkActive = 0;
    }
}

void RenderStageEntrySprites(void)
{
    const struct AnimationFrame* animation;
    struct StageEntryPositionedAnimationState* positionedState;
    struct StageEntryAnimationState* animationState;
    struct StageEntryAnimationState* animationStatesBase;
    const u16* src;
    u16* dest;
    s32 currentSlot;
    s32 nextSlot;
    u16 attr;
    u32 shape;
    u32 size;
    s16 matrix[4];
    s16* finalMatrix1Ptr;
    s16* finalMatrix2Ptr;
    s16* finalMatrix3Ptr;
    s16* matrix1Ptr;
    s16* matrix2Ptr;
    s16* matrix3Ptr;
    s32 i;
    currentSlot = 0;
    nextSlot = gOamSlotsUsed;
    {
        u32 oamOffset;
        register OamData* oamBase PIN("r0");
        oamOffset = nextSlot << 3;
        oamBase = gOamBuffer;
        dest = (u16*)(oamOffset + ((u32)oamBase));
    }
    {
        u32* entryFlagAddress;
        u32 entryFlagValue;
        u32* entryFlagPtr;
        struct StageEntryPositionedAnimationState* entryState;
        entryFlagAddress = &gStageEntryImpactEffectActive;
        entryFlagValue = *entryFlagAddress;
        entryFlagPtr = entryFlagAddress;
        if (entryFlagValue != 0) {
            if (gStageEntryMainSpriteFalling == 1) {
                {
                    struct StageEntryPositionedAnimationState* entryStateAddress;
                    u32 entryStateWord;
                    const u16* entryPosition;
                    entryStateAddress = (struct StageEntryPositionedAnimationState*)gStageEntryImpactEffectState;
                    entryStateWord = *((u32*)entryStateAddress);
                    entryState = entryStateAddress;
                    if (entryStateWord == 0) {
                        if (entryFlagValue == 1) {
                            entryPosition = gStageEntryMainSpriteState;
                            entryState->x = entryPosition[2] + 3;
                        } else {
                            entryPosition = gStageEntryMainSpriteState;
                            entryState->x = entryPosition[2] + 6;
                        }
                        {
                            register const u16* entryPositionY PIN("r2");
                            entryPositionY = entryPosition;
                            ASM_BARRIER(: "+r"(entryPositionY) : "r"(entryPosition));
                            entryState->y = entryPositionY[3];
                        }
                    }
                    {
                        register u16 entryTimer PIN("r1");
                        entryTimer = entryState->timer;
                        entryTimer++;
                        entryState->timer = entryTimer;
                        animation = sUnk_863EFE0;
                        if (animation[entryState->frame].time < entryTimer) {
                            entryState->timer = 1;
                            entryState->frame++;
                            if (animation[entryState->frame].time == 0) {
                                if ((*entryFlagPtr) == 1) {
                                    *entryFlagPtr = 0;
                                    entryState->frame = 0;
                                    entryState->x = gStageEntryMainSpriteState[2] + 3;
                                    entryState->y = 200;
                                } else {
                                    entryState->frame = 0;
                                    entryState->x = gStageEntryMainSpriteState[2] + 6;
                                }
                            }
                        }
                    }
                }
            } else {
                {
                    struct StageEntryPositionedAnimationState* entryStateAddress;
                    u32 entryStateWord;
                    const u16* entryPosition;
                    entryStateAddress = (struct StageEntryPositionedAnimationState*)gStageEntryImpactEffectState;
                    entryStateWord = *((u32*)entryStateAddress);
                    entryState = entryStateAddress;
                    if (entryStateWord == 0) {
                        if (entryFlagValue == 1) {
                            entryPosition = gStageEntryMainSpriteState;
                            entryState->x = entryPosition[2] - 3;
                        } else {
                            entryPosition = gStageEntryMainSpriteState;
                            entryState->x = entryPosition[2] - 6;
                        }
                        {
                            register const u16* entryPositionY PIN("r2");
                            entryPositionY = entryPosition;
                            ASM_BARRIER(: "+r"(entryPositionY) : "r"(entryPosition));
                            entryState->y = entryPositionY[3];
                        }
                    }
                    {
                        register u16 entryTimer PIN("r1");
                        entryTimer = entryState->timer;
                        entryTimer++;
                        entryState->timer = entryTimer;
                        animation = sUnk_863EFB0;
                        if (animation[entryState->frame].time < entryTimer) {
                            entryState->timer = 0;
                            entryState->frame++;
                            if (animation[entryState->frame].time == 0) {
                                if ((*entryFlagPtr) == 1) {
                                    *entryFlagPtr = 0;
                                    entryState->frame = 0;
                                    entryState->x = gStageEntryMainSpriteState[2] - 3;
                                    entryState->y = 200;
                                } else {
                                    entryState->frame = 0;
                                    entryState->x = gStageEntryMainSpriteState[2] - 6;
                                }
                            }
                        }
                    }
                }
            }
            src = GP(animation[entryState->frame].oam);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y =
                    (attr + ((struct StageEntryPositionedAnimationState*)gStageEntryImpactEffectState)->y) - 8;
                {
                    u32 objectX;
                    attr = *(src++);
                    *(dest++) = attr;
                    objectX = attr + ((struct StageEntryPositionedAnimationState*)gStageEntryImpactEffectState)->x;
                    objectX &= 0x1FF;
                    gOamBuffer[currentSlot].split.x = objectX;
                }
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 2;
                dest++;
            }
        }
    }
#ifdef PORTABLE
    // Entering a level, StageSelect's sub-mode 36 switches to 13 and DrawStageSelectSprites()
    // draws in the same step, before case 13 has given the main sprite an animation. The GBA
    // then reads the animation's frames from address 0, in the BIOS, and gets open bus: a
    // sprite count over 128, so it returns below. The PC cannot read the bottom 64 KiB.
    if (GP(gStageEntryMainAnimation) == NULL) {
        return;
    }
#endif
    if (gStageEntryBlinkVisible == 0) {
        src = GP(GP(gStageEntryMainAnimation)[((struct StageEntryPositionedAnimationState*)gStageEntryMainSpriteState)->frame]
                  .oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            OamData* oamBase;
            oamBase = gOamBuffer;
            positionedState = (struct StageEntryPositionedAnimationState*)gStageEntryMainSpriteState;
            attr = *(src++);
            *(dest++) = attr;
            do {
                oamBase[currentSlot].split.y = (attr + positionedState->y) - 8;
                if (gStageEntryUseAffineMainSprite != 0) {
                    oamBase[currentSlot].split.affineMode = 1;
                }
                do {
                } while (0);
            } while (0);
            attr = *(src++);
            *(dest++) = attr;
            {
                u32* stageFlag;
                stageFlag = &gStageEntryShowCollectionSparkle;
                if ((*stageFlag) != 0) {
                    oamBase[currentSlot].split.x = attr + positionedState->x;
                } else {
                    u8* directionFlag;
                    directionFlag = &gStageEntryMainSpriteFalling;
                    if (((*directionFlag) == 0) && (gStageEntryUseAffineMainSprite != 1)) {
                        s32 mirroredX;
                        mirroredX = positionedState->x - attr;
                        size = ((u8*)(&oamBase[currentSlot]))[3] >> 6;
                        size <<= 2;
                        shape = ((u8*)(&oamBase[currentSlot]))[1] >> 6;
                        shape <<= 4;
                        size += shape;
                        mirroredX -= *((const u32*)(((const u8*)sUnk_863A2EC) + size));
                        oamBase[currentSlot].split.x = mirroredX;
                        ((u8*)(&oamBase[currentSlot]))[3] |= 0x10;
                    } else {
                        oamBase[currentSlot].split.x = attr + positionedState->x;
                        {
                            u8 priorityByte;
                            s32 priorityMask;
                            priorityByte = ((u8*)(&oamBase[currentSlot]))[3];
                            priorityMask = -17;
                            priorityMask &= priorityByte;
                            ((u8*)(&oamBase[currentSlot]))[3] = priorityMask;
                        }
                    }
                }
            }
            {
                u8 modeByte;
                s32 modeMask;
                modeByte = ((u8*)(&oamBase[currentSlot]))[3];
                modeMask = -15;
                modeMask &= modeByte;
                ((u8*)(&oamBase[currentSlot]))[3] = modeMask;
            }
            *(dest++) = *(src++);
            oamBase[currentSlot].split.priority = 2;
            dest++;
        }
    }
    if (gStageEntryKeyzerFlightFinished != 0) {
        gStageEntrySparkleGroupState.frame++;
        if (sUnk_863E1C4[gStageEntrySparkleGroupState.timer].time < gStageEntrySparkleGroupState.frame) {
            gStageEntrySparkleGroupState.frame = 1;
            gStageEntrySparkleGroupState.timer++;
            if (sUnk_863E1C4[gStageEntrySparkleGroupState.timer].time == 0) {
                gStageEntrySparkleGroupState.frame = 0;
                gStageEntrySparkleGroupState.timer = 0;
                gStageEntryKeyzerFlightFinished = 0;
            }
        }
        if (gStageEntryKeyzerFlightFinished != 0) {
            src = GP(sUnk_863E1C4[gStageEntrySparkleGroupState.timer].oam);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 120;
                {
                    u32 objectX;
                    attr = *(src++);
                    *(dest++) = attr;
                    objectX = attr + 209;
                    objectX &= 0x1FF;
                    gOamBuffer[currentSlot].split.x = objectX;
                }
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 2;
                dest++;
            }
        }
    }
    {
        register struct StageEntryAnimationState* statesLoad PIN("r2");
        statesLoad = (animationStatesBase = gStageEntrySparkleAnimStates);
        animationState = statesLoad;
    }
    i = 0;
    do {
        {
            u32* activeFlag;
            activeFlag = (u32*)(((u8*)gStageEntrySparkleFlags) + i);
            if ((*activeFlag) != 0) {
                {
                    register u16 stateFrame PIN("r0");
                    stateFrame = animationState->frame;
                    stateFrame++;
                    animationState->frame = stateFrame;
                    if (sUnk_863E12C[animationState->timer].time < stateFrame) {
                        animationState->frame = 1;
                        animationState->timer++;
                        if (sUnk_863E12C[animationState->timer].time == 0) {
                            animationState->frame = 0;
                            animationState->timer = 0;
                            *activeFlag = 0;
                        }
                    }
                }
                {
                    u32 stateFrameOffset;
                    register const struct AnimationFrame* secondTable PIN("r1");
                    stateFrameOffset = animationState->timer << 3;
                    secondTable = sUnk_863E12C;
                    src = GP(((const struct AnimationFrame*)(stateFrameOffset + ((u32)secondTable)))->oam);
                }
                nextSlot += *(src++);
                {
                    register s32 stateSlotLimit PIN("r2");
                    stateSlotLimit = nextSlot;
                    if (stateSlotLimit > 128) {
                        return;
                    }
                    if (currentSlot < (stateSlotLimit + nextSlot - stateSlotLimit)) {
                        register u32 objectY PIN("r12");
                        u32 objectX;
                        OamData* oam;
                        register OamData* oamBase PIN("r1");
                        {
                            const u32* stateYBase;
                            s32 stateOffsetY;
                            stateYBase = sUnk_8639ADC;
                            stateOffsetY = i;
                            stateYBase = (const u32*)(stateOffsetY + (u32)stateYBase);
                            objectY = *stateYBase;
                        }
                        {
                            s32 slotIndex;
                            u32 oamOffset;
                            slotIndex = currentSlot;
                            oamOffset = slotIndex << 3;
                            oamBase = gOamBuffer;
                            oam = (OamData*)(oamOffset + ((u32)oamBase));
                        }
                        {
                            const u32* stateXBase;
                            register s32 stateOffsetX PIN("r2");
                            stateXBase = sUnk_8639AC4;
                            stateOffsetX = i;
                            stateXBase = (const u32*)(stateOffsetX + (u32)stateXBase);
                            objectX = *stateXBase + stateOffsetX - stateOffsetX;
                        }
                        {
                            s32 slotEnd;
                            s32 slotStart;
                            slotEnd = nextSlot;
                            slotStart = currentSlot;
                            currentSlot = slotEnd - slotStart;
                        }
                        do {
                            attr = *(src++);
                            *(dest++) = attr;
                            oam->split.y = (attr + objectY) - 8;
                            attr = *(src++);
                            *(dest++) = attr;
                            {
                                u32 stateX;
                                register u32 stateMask PIN("r0");
                                u32 stateAttr1;
                                stateX = attr + objectX;
                                stateMask = 0x1FF;
                                stateX &= stateMask;
                                stateAttr1 = ((u16*)oam)[1];
                                stateMask = 0xFFFFFE00;
                                stateMask &= stateAttr1;
                                stateMask |= stateX;
                                ((u16*)oam)[1] = stateMask;
                            }
                            *(dest++) = *(src++);
                            oam->split.priority = 2;
                            dest++;
                            oam++;
                            currentSlot--;
                        } while (currentSlot != 0);
                        currentSlot = nextSlot;
                    }
                }
            }
        }
        animationState++;
        i += sizeof(u32);
    } while (((s32)animationState) <= ((s32)(animationStatesBase + 5)));
    if ((gStageEntrySequenceStep != 0) && (gStageEntrySequenceMode == 5)) {
        OamData* affineOam;
        register s32 modeMask PIN("r0");
        register s32 priorityBase PIN("r12");
        u32 modeByte;
        src = GP(GP(gStageEntryFlyingKeyzerAnimation)[gStageEntryFlyingKeyzerState.frame].oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        matrix1Ptr = &matrix[1];
        matrix2Ptr = &matrix[2];
        matrix3Ptr = &matrix[3];
        if (currentSlot < nextSlot) {
            struct StageEntryPositionedAnimationState* affineState;
            OamData* oamBase;
            affineState = (struct StageEntryPositionedAnimationState*)&gStageEntryFlyingKeyzerState;
            oamBase = gOamBuffer;
            priorityBase = -13;
            affineOam = &oamBase[currentSlot];
            do {
                attr = *(src++);
                *(dest++) = attr;
                affineOam->split.y = (attr + affineState->y) - 8;
                affineOam->split.affineMode = 1;
                attr = *(src++);
                *(dest++) = attr;
                {
                    s16 objectX;
                    objectX = attr + affineState->x;
                    objectX &= 0x1FF;
                    affineOam->split.x = objectX;
                }
                modeByte = ((u8*)affineOam)[3];
                modeMask = -15;
                modeMask &= modeByte;
                modeMask |= 10;
                ((u8*)affineOam)[3] = modeMask;
                *(dest++) = *(src++);
                modeByte = ((u8*)affineOam)[5];
                modeMask = priorityBase;
                modeMask &= modeByte;
                modeMask |= 8;
                ((u8*)affineOam)[5] = modeMask;
                dest++;
                affineOam++;
                {
                    register u8 one PIN("r0");
                    one = 1;
                    currentSlot += one;
                }
            } while (currentSlot < nextSlot);
        }
        matrix[0] = FixedMul(sSinCosTable[gStageEntryFlyingKeyzerState.attribute + 64], (s16)FixedInverse(256));
        *matrix1Ptr = FixedMul(sSinCosTable[gStageEntryFlyingKeyzerState.attribute], (s16)FixedInverse(256));
        *matrix2Ptr =
            FixedMul((s16)(-((u16)sSinCosTable[gStageEntryFlyingKeyzerState.attribute])), (s16)FixedInverse(256));
        *matrix3Ptr = FixedMul(sSinCosTable[gStageEntryFlyingKeyzerState.attribute + 64], (s16)FixedInverse(256));
        gOamBuffer[20].all.affineParam = matrix[0];
        gOamBuffer[21].all.affineParam = *matrix1Ptr;
        gOamBuffer[22].all.affineParam = *matrix2Ptr;
        gOamBuffer[23].all.affineParam = *matrix3Ptr;
    }
    if (gStageEntrySequenceMode == 5) {
        src = (const u16*)GP(gStageEntryCompanionSpriteState.data);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        matrix1Ptr = &matrix[1];
        matrix2Ptr = &matrix[2];
        matrix3Ptr = &matrix[3];
        if (currentSlot < nextSlot) {
            u8* ce8State;
            u32 eight;
            register s32 priorityBase PIN("r12");
            OamData* oam;
            {
                OamData* oamBase;
                ce8State = (u8*)&gStageEntryCompanionSpriteState;
                eight = 8;
                oamBase = gOamBuffer;
                priorityBase = -13;
                oam = oamBase + currentSlot;
            }
            do {
                attr = *(src++);
                *(dest++) = attr;
                {
                    u8* yState;
                    yState = ce8State;
                    oam->split.y = (attr + yState[2]) - 8;
                }
                oam->split.affineMode = 1;
                attr = *(src++);
                *(dest++) = attr;
                {
                    u32 objectX;
                    {
                        u8* xState;
                        xState = ce8State;
                        objectX = attr + *((u16*)xState);
                    }
                    objectX &= 0x1FF;
                    oam->split.x = objectX;
                }
                {
                    u8 modeByte;
                    register s32 signedMask PIN("r0");
                    modeByte = ((u8*)oam)[3];
                    signedMask = 15;
                    signedMask = -signedMask;
                    signedMask &= modeByte;
                    signedMask |= eight;
                    ((u8*)oam)[3] = signedMask;
                }
                *(dest++) = *(src++);
                {
                    register u8 priorityByte PIN("r1");
                    register s32 priorityMask PIN("r0");
                    priorityByte = ((u8*)oam)[5];
                    priorityMask = priorityBase;
                    priorityMask &= priorityByte;
                    priorityMask |= eight;
                    ((u8*)oam)[5] = priorityMask;
                }
                dest++;
                oam++;
                currentSlot++;
            } while (currentSlot < nextSlot);
        }
        matrix[0] = FixedMul(sSinCosTable[gStageEntryCompanionSpriteState.attribute + 64], (s16)FixedInverse(256));
        *matrix1Ptr = FixedMul(sSinCosTable[gStageEntryCompanionSpriteState.attribute], (s16)FixedInverse(256));
        *matrix2Ptr =
            FixedMul((s16)(-((u16)sSinCosTable[gStageEntryCompanionSpriteState.attribute])), (s16)FixedInverse(256));
        *matrix3Ptr = FixedMul(sSinCosTable[gStageEntryCompanionSpriteState.attribute + 64], (s16)FixedInverse(256));
        gOamBuffer[16].all.affineParam = matrix[0];
        gOamBuffer[17].all.affineParam = *matrix1Ptr;
        gOamBuffer[18].all.affineParam = *matrix2Ptr;
        gOamBuffer[19].all.affineParam = *matrix3Ptr;
    }
    if (((gStageEntryStageGraphicIndex == 0) &&
         ((gCurrentPassage == PASSAGE_ENTRY) || (gCurrentPassage == PASSAGE_GOLDEN))) ||
        ((((u8)(gStageEntryStageGraphicIndex - 1)) <= 3) && (((u8)(gCurrentPassage - 1)) <= 3))) {
        {
            struct StageEntryPositionedAnimationState* secondaryState;
            secondaryState = (struct StageEntryPositionedAnimationState*)&gStageEntrySecondarySpriteState;
            src = GP(GP(gStageEntrySecondaryAnimation)[secondaryState->frame].oam);
        }
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y =
                (attr + ((struct StageEntryPositionedAnimationState*)&gStageEntrySecondarySpriteState)->y) - 8;
            {
                u32 objectX;
                attr = *(src++);
                *(dest++) = attr;
                objectX = attr + ((struct StageEntryPositionedAnimationState*)&gStageEntrySecondarySpriteState)->x;
                objectX &= 0x1FF;
                gOamBuffer[currentSlot].split.x = objectX;
            }
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 2;
            dest++;
        }
    }
    {
        register struct StageEntryPositionedAnimationState* stateCheck PIN("r1");
        stateCheck = (struct StageEntryPositionedAnimationState*)gStageEntryKeyzerTargetState;
        if (stateCheck[1].timer > 64) {
            struct StageEntryPositionedAnimationState* lateState;
            register s32 lateModeMask PIN("r12");
            register s32 latePriorityMask PIN("r9");
            src = sUnk_863ED00;
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            if (currentSlot < nextSlot) {
                OamData* lateOam;
                OamData* oamBase;
                lateState = stateCheck;
                oamBase = gOamBuffer;
                lateModeMask = -15;
                latePriorityMask = -13;
                lateOam = &oamBase[currentSlot];
                do {
                    attr = *(src++);
                    *(dest++) = attr;
                    lateOam->split.y = (attr + lateState->y) - 8;
                    lateOam->split.affineMode = 1;
                    attr = *(src++);
                    *(dest++) = attr;
                    {
                        u32 objectX;
                        objectX = attr + lateState->x;
                        objectX &= 0x1FF;
                        lateOam->split.x = objectX;
                    }
                    {
                        register u8 modeByte PIN("r1");
                        register s32 modeMask PIN("r0");
                        modeByte = ((u8*)lateOam)[3];
                        modeMask = lateModeMask;
                        modeMask &= modeByte;
                        modeMask |= 2;
                        ((u8*)lateOam)[3] = modeMask;
                    }
                    *(dest++) = *(src++);
                    {
                        register u8 priorityByte PIN("r1");
                        register s32 priorityMask PIN("r0");
                        priorityByte = ((u8*)lateOam)[5];
                        priorityMask = latePriorityMask;
                        priorityMask &= priorityByte;
                        priorityMask |= 8;
                        ((u8*)lateOam)[5] = priorityMask;
                    }
                    dest++;
                    lateOam++;
                    currentSlot++;
                } while (currentSlot < nextSlot);
            }
            src = sUnk_863ECF8;
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            if (currentSlot < nextSlot) {
                OamData* lateOam;
                OamData* oamBase;
                lateState = (struct StageEntryPositionedAnimationState*)gStageEntryKeyzerTargetState;
                oamBase = gOamBuffer;
                lateModeMask = -15;
                latePriorityMask = -13;
                lateOam = &oamBase[currentSlot];
                do {
                    attr = *(src++);
                    *(dest++) = attr;
                    lateOam->split.y = (attr + lateState->y) - 8;
                    lateOam->split.affineMode = 1;
                    attr = *(src++);
                    *(dest++) = attr;
                    {
                        u32 objectX;
                        objectX = attr + lateState->x;
                        objectX &= 0x1FF;
                        lateOam->split.x = objectX;
                    }
                    {
                        register u8 modeByte PIN("r1");
                        register s32 modeMask PIN("r0");
                        modeByte = ((u8*)lateOam)[3];
                        modeMask = lateModeMask;
                        modeMask &= modeByte;
                        modeMask |= 4;
                        ((u8*)lateOam)[3] = modeMask;
                    }
                    *(dest++) = *(src++);
                    {
                        register u8 priorityByte PIN("r1");
                        register s32 priorityMask PIN("r0");
                        priorityByte = ((u8*)lateOam)[5];
                        priorityMask = latePriorityMask;
                        priorityMask &= priorityByte;
                        priorityMask |= 8;
                        ((u8*)lateOam)[5] = priorityMask;
                    }
                    dest++;
                    lateOam++;
                    currentSlot++;
                } while (currentSlot < nextSlot);
            }
            src = sUnk_863ECF0;
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            if (currentSlot < nextSlot) {
                OamData* lateOam;
                OamData* oamBase;
                lateState = (struct StageEntryPositionedAnimationState*)gStageEntryKeyzerTargetState;
                oamBase = gOamBuffer;
                lateModeMask = -15;
                latePriorityMask = -13;
                lateOam = &oamBase[currentSlot];
                do {
                    attr = *(src++);
                    *(dest++) = attr;
                    lateOam->split.y = (attr + lateState->y) - 8;
                    lateOam->split.affineMode = 1;
                    attr = *(src++);
                    *(dest++) = attr;
                    {
                        u32 objectX;
                        objectX = attr + lateState->x;
                        objectX &= 0x1FF;
                        lateOam->split.x = objectX;
                    }
                    {
                        register u8 modeByte PIN("r1");
                        register s32 modeMask PIN("r0");
                        modeByte = ((u8*)lateOam)[3];
                        modeMask = lateModeMask;
                        modeMask &= modeByte;
                        modeMask |= 6;
                        ((u8*)lateOam)[3] = modeMask;
                    }
                    *(dest++) = *(src++);
                    {
                        register u8 priorityByte PIN("r1");
                        register s32 priorityMask PIN("r0");
                        priorityByte = ((u8*)lateOam)[5];
                        priorityMask = latePriorityMask;
                        priorityMask &= priorityByte;
                        priorityMask |= 8;
                        ((u8*)lateOam)[5] = priorityMask;
                    }
                    dest++;
                    lateOam++;
                    currentSlot++;
                } while (currentSlot < nextSlot);
            }
        }
    }
    if (gStageEntrySequenceMode == 5) {
        src = GP(GP(gStageEntryCompanionKeyzerAnimation)[gStageEntryCompanionKeyzerState.frame].oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = (attr + gStageEntryCompanionKeyzerState.y) - 8;
            {
                u32 objectX;
                attr = *(src++);
                *(dest++) = attr;
                objectX = attr + gStageEntryCompanionKeyzerState.x;
                objectX &= 0x1FF;
                gOamBuffer[currentSlot].split.x = objectX;
            }
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 2;
            dest++;
        }
    }
    if ((gStageEntryKeyzerPositionState[2] <= 0x103) && (gStageEntrySelectedStage != 0)) {
        if (gStageEntrySelectedStage == 5) {
            src = sUnk_863E24C;
        } else {
            src = sUnk_863E4AA;
        }
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y =
                (attr + ((struct StageEntryPositionedAnimationState*)gStageEntryKeyzerPositionState)->y) - 8;
            attr = *(src++);
            *(dest++) = attr;
            {
                u32 objectX;
                objectX = attr + ((struct StageEntryPositionedAnimationState*)gStageEntryKeyzerPositionState)->x;
                objectX &= 0x1FF;
                gOamBuffer[currentSlot].split.x = objectX;
            }
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 2;
            dest++;
        }
    }
    matrix[0] = FixedMul(
        sSinCosTable[gStageEntryMainSpriteState[6] + 64], (s16)FixedInverse((s16)gStageEntryMainSpriteState[7])
    );
    *(finalMatrix1Ptr = &matrix[1]) =
        FixedMul(sSinCosTable[gStageEntryMainSpriteState[6]], (s16)FixedInverse((s16)gStageEntryMainSpriteState[7]));
    *(finalMatrix2Ptr = &matrix[2]) = FixedMul(
        (s16)(-((u16)sSinCosTable[gStageEntryMainSpriteState[6]])),
        (s16)FixedInverse((s16)gStageEntryMainSpriteState[7])
    );
    *(finalMatrix3Ptr = &matrix[3]) = FixedMul(
        sSinCosTable[gStageEntryMainSpriteState[6] + 64], (s16)FixedInverse((s16)gStageEntryMainSpriteState[7])
    );
    gOamBuffer[0].all.affineParam = matrix[0];
    gOamBuffer[1].all.affineParam = *finalMatrix1Ptr;
    gOamBuffer[2].all.affineParam = *finalMatrix2Ptr;
    gOamBuffer[3].all.affineParam = *finalMatrix3Ptr;
    matrix[0] = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[0] + 64], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix1Ptr = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[0]], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix2Ptr = FixedMul(
        (s16)(-((u16)sSinCosTable[gStageEntryPaletteFadeCounters[0]])),
        (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix3Ptr = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[0] + 64], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    gOamBuffer[4].all.affineParam = matrix[0];
    gOamBuffer[5].all.affineParam = *finalMatrix1Ptr;
    gOamBuffer[6].all.affineParam = *finalMatrix2Ptr;
    gOamBuffer[7].all.affineParam = *finalMatrix3Ptr;
    matrix[0] = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[1] - (-64)], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix1Ptr = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[1]], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix2Ptr = FixedMul(
        (s16)(-((u16)sSinCosTable[gStageEntryPaletteFadeCounters[1]])),
        (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix3Ptr = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[1] + 64], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    gOamBuffer[8].all.affineParam = matrix[0];
    gOamBuffer[9].all.affineParam = *finalMatrix1Ptr;
    gOamBuffer[10].all.affineParam = *finalMatrix2Ptr;
    gOamBuffer[11].all.affineParam = *finalMatrix3Ptr;
    matrix[0] = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[2] + 64], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix1Ptr = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[2]], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix2Ptr = FixedMul(
        (s16)(-((u16)sSinCosTable[gStageEntryPaletteFadeCounters[2]])),
        (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    *finalMatrix3Ptr = FixedMul(
        sSinCosTable[gStageEntryPaletteFadeCounters[2] + 64], (s16)FixedInverse((s16)gStageEntryKeyzerTargetState[4])
    );
    gOamBuffer[12].all.affineParam = matrix[0];
    gOamBuffer[13].all.affineParam = *finalMatrix1Ptr;
    gOamBuffer[14].all.affineParam = *finalMatrix2Ptr;
    gOamBuffer[15].all.affineParam = *finalMatrix3Ptr;
    gOamSlotsUsed = nextSlot;
}

s32 UpdateStageEntryHorizontalShake(void)
{
    s32 counter;
    u32 masked;
    s32 nextCounter;
    s32 offset;

    offset = 0;
    switch (gStageEntryShakeMode) {
        case 0:
            offset = 0;
            gStageEntryShakeTimer = 0;
            break;

        case 1:
            counter = gStageEntryShakeTimer;
            masked = counter & 3;
            offset = 1;
            if (masked <= 1) {
                offset -= 2;
            }
            nextCounter = counter + 1;
            gStageEntryShakeTimer = nextCounter;
            if ((u8)nextCounter > 30) {
                gStageEntryShakeTimer = 0;
                gStageEntryShakeMode = 0;
            }
            break;

        case 2:
            counter = gStageEntryShakeTimer;
            masked = counter & 3;
            offset = 1;
            if (masked <= 1) {
                offset -= 2;
            }
            nextCounter = counter + 1;
            gStageEntryShakeTimer = nextCounter;
            break;

        case 3:
            counter = gStageEntryShakeTimer;
            masked = counter & 3;
            offset = 1;
            if (masked <= 1) {
                offset = 0;
            }
            nextCounter = counter + 1;
            gStageEntryShakeTimer = nextCounter;
            break;

        case 4:
            counter = gStageEntryShakeTimer;
            masked = counter & 3;
            offset = 2;
            if (masked <= 1) {
                offset -= 4;
            }
            nextCounter = counter + 1;
            gStageEntryShakeTimer = nextCounter;
            break;
    }

    return offset;
}

s32 ExpandStageEntryWindow(void)
{
    s32 right;

    gStageEntryWindowLeft -= 10;
    right = gStageEntryWindowRight + 10;
    gStageEntryWindowRight = right;
    gStageEntryWindowTop -= 10;
    gStageEntryWindowBottom += 10;

    if ((s16)gStageEntryWindowBottom > 160) {
        gStageEntryWindowTop = 0;
        gStageEntryWindowBottom = 160;
    }

    if ((s16)right > 240) {
        gStageEntryWindowLeft = 0;
        gStageEntryWindowRight = 240;
        return 1;
    }

    return 0;
}

s32 CollapseStageEntryWindow(void)
{
    s32 top;

    top = gStageEntryWindowLeft + 10;
    gStageEntryWindowLeft = top;
    gStageEntryWindowRight -= 10;

    if ((s16)top > 40) {
        gStageEntryWindowTop += 10;
        gStageEntryWindowBottom -= 10;
    }

    if ((s16)gStageEntryWindowBottom <= 79) {
        gStageEntryWindowTop = 80;
        gStageEntryWindowBottom = 80;
    }

    if ((s16)gStageEntryWindowRight <= 119) {
        gStageEntryWindowLeft = 120;
        gStageEntryWindowRight = 120;
        return 1;
    }

    return 0;
}

static inline s32 GetStageEntryWrappedScroll(void)
{
    s32 scroll;
    s32 adjusted;

    scroll = gStageEntryHorizontalScroll;
    adjusted = scroll;
    if (scroll < 0) {
        adjusted += 0x1FF;
    }
    return scroll - ((adjusted >> 9) << 9);
}

void UpdateStageEntryDisplayRegisters(void)
{
    vu16* windowRegister;
    vu16* backgroundRegister;
    s32 shake;
    s32 verticalOffset;

    shake = UpdateStageEntryHorizontalShake();

    windowRegister = (vu16*)0x04000040;
    *windowRegister = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
    windowRegister += 2;
    *windowRegister = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;

    backgroundRegister = (vu16*)0x04000012;
    verticalOffset = shake;
    verticalOffset += 8;
    *backgroundRegister = verticalOffset;

    *(vu16*)0x04000010 = GetStageEntryWrappedScroll();
    *(vu16*)0x04000016 = verticalOffset;
    *(vu16*)0x04000014 = GetStageEntryWrappedScroll();
}

#ifndef GUARD_STAGE_ENTRY_H
#define GUARD_STAGE_ENTRY_H

#include "gba.h"
#include "oam.h"

struct AGB_ALIGN StageEntryAnimationState {
    u16 timer;
    u16 frame;
};

struct AGB_ALIGN StageEntryPositionedAnimationState {
    u16 timer;
    u16 frame;
    u16 x;
    u16 y;
};

struct AGB_ALIGN StageEntryAnimatedSpriteState {
    u16 timer;
    u16 frame;
    u16 x;
    u16 y;
    u8 attribute;
};

struct AGB_ALIGN StageEntrySpriteState {
    u16 x;
    u16 y;
    GBA_PTR(const u8*) data;
    u8 attribute;
};

extern struct StageEntryAnimationState gStageEntrySparkleGroupState;
extern struct StageEntryAnimationState gStageEntrySparkleAnimStates[];
extern const struct AnimationFrame sStageEntryKeyzerCompanionAnimation[];
extern const u8 sStageEntryKeyzerSpriteData[];
extern const struct AnimationFrame sStageEntryKeyzerFlightAnimation[];
extern struct StageEntryAnimatedSpriteState gStageEntryFlyingKeyzerState;
extern GBA_PTR(const struct AnimationFrame*) gStageEntryFlyingKeyzerAnimation;
extern struct StageEntryPositionedAnimationState gStageEntryCompanionKeyzerState;
extern GBA_PTR(const struct AnimationFrame*) gStageEntryCompanionKeyzerAnimation;
extern struct StageEntryAnimationState gStageEntrySecondarySpriteState;
extern GBA_PTR(const struct AnimationFrame*) gStageEntrySecondaryAnimation;
extern const struct AnimationFrame sStageEntrySecondaryIdleAnimation[];
extern u16 gStageEntryKeyzerTargetState[];
extern u8 gStageEntryPaletteFadeCounters[];
extern u8 gStageEntryPaletteCycleOffset;
extern const u8 sStageEntryPaletteCycleTable[];
extern struct StageEntrySpriteState gStageEntryCompanionSpriteState;
extern u32 gStageEntryShowCollectionSparkle;
extern u32 gStageEntryKeyzerFlightFinished;
extern u32 gStageEntrySparkleFlags[];
extern u8 gStageEntryKeyzerPaletteStep;
extern u16 gStageEntryKeyzerPositionState[];
extern const s16 sStageEntryKeyzerApproachXOffsets[];
extern const s16 sStageEntryKeyzerApproachYOffsets[];
extern const s16 sStageEntryKeyzerImpactXOffsets[];
extern const s16 sStageEntryKeyzerImpactYOffsets[];
extern const s16 sStageEntryKeyzerRiseYOffsets[];
extern const s16 sStageEntryKeyzerFloatYOffsets[];
extern const s16 sStageEntryKeyzerExitXOffsets[];
extern const s16 sStageEntryKeyzerExitYOffsets[];
extern const s16 sStageEntryKeyzerFinalXOffsets[];
extern const s16 sStageEntryKeyzerFinalYOffsets[];
extern const struct AnimationFrame sStageEntryKeyzerRiseAnimation[];
extern const struct AnimationFrame sStageEntrySecondaryIntroAnimation[];
extern const struct AnimationFrame sStageEntrySecondaryLoopAnimation[];
extern const struct AnimationFrame sStageEntrySecondaryAnimation[];
extern const struct AnimationFrame sStageEntryKeyzerMainAnimation[];
extern const struct AnimationFrame sStageEntryKeyzerMovingAnimation[];
extern const struct AnimationFrame sStageEntryKeyzerImpactAnimation[];
extern const struct AnimationFrame sStageEntrySecondaryFallAnimation[];
extern const struct AnimationFrame sStageEntryKeyzerFoundAnimation[];
extern const struct AnimationFrame sStageEntryFlyingKeyzerEscapeAnimation[];
extern const struct AnimationFrame sStageEntryFlyingKeyzerSpinAnimation[];
extern const struct AnimationFrame sStageEntryCompanionKeyzerRescueAnimation[];
extern const struct AnimationFrame sStageEntryKeyzerRewardAnimation[];
extern const u16 sStageEntryFlyingKeyzerApproachXOffsets[];
extern const u16 sStageEntryFlyingKeyzerApproachYOffsets[];
extern const u16 sStageEntryFlyingKeyzerScriptXPositions[];
extern const u16 sStageEntryFlyingKeyzerScriptYPositions[];
extern const u16 sStageEntryKeyzerXPositions[];
extern const u16 sStageEntryKeyzerYPositions[];
extern GBA_PTR(const u8*) const sStageEntryCompanionSpriteDataByFrame[];
extern const u8 sStageEntryCompanionSpriteFrameIndices[];
extern const u32 sStageEntrySparkleTriggerFrames[][2];
extern GBA_PTR(const u8*) const sStageEntryStagePalettes[][4];
extern const u8 sStageEntryPassagePal[];
extern const u8 sStageEmeraldPassagePal[];
extern const u8 sStageRubyPassagePal[];
extern const u8 sStageTopazPassagePal[];
extern const u8 sStageSapphirePassagePal[];
extern const u8 sStageGoldenPyramidPal[];

extern u8 gStageEntryBlinkCounter;
extern u32 gStageEntryBlinkVisible;
extern u32 gStageEntryBlinkActive;

extern u8 gStageEntrySelectedStage;
extern u8 gStageEntryStageGraphicIndex;
extern u8 gStageEntryStageGraphicMirror;
extern u8 gStageEntryStageGraphicId;
extern const u32 sStageEntryNormalPassageScrollOffsets[];
extern const u32 sStageEntryEntryPassageScrollOffsets[];
extern GBA_PTR(const u8*) const sStageEntryPassageBgGraphics[];
extern GBA_PTR(const u8*) const sStageEntryPassagePalettes[];
extern const u8 sStageEntryPassagePal[];
extern const u8 sStageEmeraldPassagePal[];
extern const u8 sStageRubyPassagePal[];
extern const u8 sStageTopazPassagePal[];
extern const u8 sStageSapphirePassagePal[];
extern const u8 sStageGoldenPyramidPal[];
extern const u32 sStageEntryNormalStageGraphicIds[];
extern const u32 sStageEntryEntryStageGraphicIds[];
extern const u8 sStageEntryPassageOverlayGraphics[];
extern const u8 sStageEntryCompletedKeyzerGraphics[];
extern const u8 sStageEntryCommonBgTiles[];
extern const u8 sStageEntryObjectPalette[];
extern const u8 sStageEntryObjectTiles[];
extern u8 gStageEntryStageVariant;
extern u8 gStageEntryMainSpriteFalling;
extern u16 gStageEntryMainSpriteState[];
extern u32 gStageEntryUseAffineMainSprite;
extern s16 gStageEntryVerticalVelocity;
extern s16 gStageEntryHorizontalVelocity;
extern u8 gStageEntrySequenceMode;
extern u8 gStageEntryIdleAnimationVariant;
extern GBA_PTR(const struct AnimationFrame*) gStageEntryMainAnimation;
extern const struct AnimationFrame sStageEntryLandingIntroAnimation[];
extern const struct AnimationFrame sStageEntryMovingLandingIntroAnimation[];
extern GBA_PTR(const struct AnimationFrame*) const sStageEntryIdleAnimationsByVariant[];

extern u8 gStageEntryExitRequested;
extern u8 gStageEntrySequenceStep;
extern u16 gStageEntrySequenceTimer;
extern u16 gStageEntryImpactEffectState[];
extern u32 gStageEntryImpactEffectActive;
extern const struct AnimationFrame sStageEntryLandingAnimation[];

extern u32 gStageEntryShakeMode;
extern u8 gStageEntryShakeTimer;
extern s16 gStageEntryHorizontalScroll;
extern u16 gStageEntryWindowLeft;
extern u16 gStageEntryWindowRight;
extern u16 gStageEntryWindowTop;
extern u16 gStageEntryWindowBottom;
extern GBA_PTR(const u8*) const sStageEntryBg0TilemapChunkTable[];
extern GBA_PTR(const u8*) const sStageEntryBg1TilemapChunkTable[];

s32 UpdateStageEntry(void);
void InitializeStageEntrySceneGraphics(void);
void InitializeStageEntryState(void);
void LoadStageEntryTilemapChunks(void);
void InitializeStageEntryLandingState(void);
void InitializeStageEntryKeyzerArrivalState(void);
void InitializeStageEntryMovingLandingState(void);
s32 UpdateStageEntryStateMachine(void);
s32 UpdateStageEntryKeyzerSequence(void);
s32 UpdateStageEntryLandingSequence(void);
s32 UpdateStageEntryMovingLandingSequence(void);
s32 UpdateStageEntryNoKeyzerSequence(void);
s32 UpdateStageEntryWithKeyzerSequence(void);
s32 UpdateStageEntryFlyingKeyzerAnimation(void);
s32 UpdateStageEntryCompanionKeyzerAnimation(void);
void UpdateStageEntrySecondaryAnimationAndScale(void);
void UpdateStageEntryPaletteFadeCounters(void);
void CycleStageEntryPalette(void);
void UpdateStageEntryBlinkState(void);
void RenderStageEntrySprites(void);
s32 UpdateStageEntryHorizontalShake(void);
s32 ExpandStageEntryWindow(void);
s32 CollapseStageEntryWindow(void);
void UpdateStageEntryDisplayRegisters(void);

#endif

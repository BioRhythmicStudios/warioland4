#ifndef BOSS_DOOR_OPENING_H
#define BOSS_DOOR_OPENING_H

#include "types.h"

struct AGB_ALIGN BossDoorOpeningBackground {
    u16 x;
    u16 y;
    u8 padding4[4];
    u16 width;
    u16 first;
    u8 paddingC[2];
    u16 second;
    u8 padding10[2];
};

struct AGB_ALIGN BossDoorOpeningScrollOffset {
    u16 horizontal;
    u16 vertical;
};

struct AGB_ALIGN BossDoorOpeningIcon {
    u16 animationTimer;
    u16 frame;
    u8 state;
    u8 padding[3];
};

extern struct BossDoorOpeningBackground gStageEntryMainSpriteState;
extern struct BossDoorOpeningScrollOffset gStageEntryHorizontalScroll;
extern u8 gStageEntryExitRequested;
extern u8 gStageEntrySequenceStep;
extern u16 gStageEntrySequenceTimer;
extern u8 gUnk_3003C4A;
extern u16 gStageEntryWindowLeft;
extern u16 gStageEntryWindowRight;
extern u16 gStageEntryWindowTop;
extern u16 gStageEntryWindowBottom;
extern u8 gUnk_3003C56;
extern u32 gStageEntryShakeMode;
extern u8 gUnk_3004730;
extern u8 gUnk_3004731;
extern struct BossDoorOpeningIcon gUnk_3004738[];
extern struct BossDoorOpeningIcon gUnk_3004758;
extern struct BossDoorOpeningIcon gUnk_3004760;
extern s16 gUnk_3004768;
extern s16 gUnk_300476A;
extern s16 gUnk_300476C;

void InitializeBossDoorOpeningScene(void);
void PrepareBossDoorOpeningResult(void);
void PrepareBossDoorOpeningJewelDisplay(void);
s32 UpdateBossDoorOpeningState(void);
void UpdateBossDoorOpeningDisplay(void);
s32 UpdateBossDoorOpeningSequence(void);
void UpdateBossDoorOpeningSprites(void);

#endif  // BOSS_DOOR_OPENING_H

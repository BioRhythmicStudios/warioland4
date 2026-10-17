#ifndef CREDITS_H
#define CREDITS_H

#include "types.h"

struct AGB_ALIGN CreditsSpriteState {
    u8 frame;
    u8 timer;
    u8 active;
    u8 reserved3;
    u16 x;
    u16 y;
};

struct AGB_ALIGN CreditsObjectState {
    u16 reserved0;
    u8 timer;
    u8 reserved3;
    u16 x;
    u16 y;
};

struct AGB_ALIGN CreditsSpawnState {
    u8 reserved0[2];
    u8 count;
    u8 reserved3;
    u16 x;
    u16 y;
};

struct AGB_ALIGN CreditsAnchor {
    u8 reserved0[4];
    u16 x;
    u16 y;
};

union AGB_ALIGN CreditsSpriteStorage {
    u8 bytes[8];
    struct CreditsSpriteState sprite;
    struct CreditsObjectState object;
    struct CreditsSpawnState spawn;
    struct CreditsAnchor anchor;
};

struct AGB_ALIGN CreditsTreasureState {
    u8 phase;
    u8 treasureIndex;
    u16 currentTreasureBit;
    u8 gfxFrame;
    u8 timer;
    u16 sequenceTimer;
};

struct AGB_ALIGN CreditsTreasureAnim {
    u8 type;
    u8 frame;
    u8 timer;
    u8 reserved3;
    u16 collectedMask;
    u16 scaleX;
    u16 scaleY;
    u16 xOffset;
    u16 dropY;
    u16 reserved14;
};

struct AGB_ALIGN CreditsTreasureDrop {
    u8 state;
    u8 timer;
    u16 scale;
    u16 y;
    u16 reserved6;
};

extern union CreditsSpriteStorage gCreditsBannerSpriteState;
extern union CreditsSpriteStorage gCreditsAnchorSpriteState;
extern union CreditsSpriteStorage gCreditsDifficultyLabelSpriteState;
extern union CreditsSpriteStorage gCreditsDifficultyEffectSpriteState;
extern union CreditsSpriteStorage gCreditsForegroundLayerSpriteState;
extern union CreditsSpriteStorage gCreditsMiddleLayerSpriteState;
extern union CreditsSpriteStorage gCreditsBackgroundLayerSpriteState;
extern union CreditsSpriteStorage gCreditsClosingSpriteState;
extern union CreditsSpriteStorage gCreditsTreasureBurstSpriteState;

extern char gCreditsMainState;
extern char gCreditsSequencePhase;
extern u16 gCreditsTreasureAffineAngle;
extern struct CreditsTreasureState gCreditsTreasureState;
extern struct CreditsTreasureAnim gCreditsTreasureAnim;
extern struct CreditsTreasureDrop gCreditsTreasureDrop;

u32 UpdateCreditsScreen(void);
void SelectCreditsVBlankCallback(void);
void CreditsVBlankMain(void);
void CreditsVBlankTreasureDma(void);
void CreditsVCountUpdateBgOffset(void);
void InitializeCredits(void);
void RenderCreditsOam(void);
u32 UpdateCreditsSequence(void);
u32 UpdateCreditsTreasureDrop(void);

#endif  // CREDITS_H

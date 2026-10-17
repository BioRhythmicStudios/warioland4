#ifndef MINIGAMES_WARIO_HOP_H
#define MINIGAMES_WARIO_HOP_H

#include "types.h"

struct AGB_ALIGN WarioHopObjState {
    u16 x;
    u16 y;
    u16 angle;
    u16 scale;
    u16 animTimer;
    u16 animFrame;
    u16 unkC;
    u16 yOffset;
};

struct AGB_ALIGN WarioHopSmallState {
    u16 x;
    u16 y;
    u8 type;
    u8 pad5;
    u16 animTimer;
    u16 animFrame;
    u8 active;
    u8 padB;
};

struct AGB_ALIGN WarioHopMidState {
    u16 x;
    u16 y;
    u16 angle;
    u16 scale;
    u16 xOffset;
    u16 yOffset;
    u8 active;
    u8 padD[3];
};

u32 WarioHopUpdateStateMachine(void);
void WarioHopApplyBgScroll(void);
void WarioHopInit(void);
void WarioHopRestartMusicForNextSpeed(void);
s32 WarioHopUpdateGameplay(void);
s32 WarioHopIsJumpPressed(void);
s32 WarioHopUpdateJumpArc(void);
void WarioHopUpdateWaitingForStart(void);
void WarioHopSpawnObstacleIfDue(void);
void WarioHopAdvancePlayerAnimation(void);
void WarioHopUpdateObstacles(void);
void WarioHopUpdateGameOverMusic(void);
void WarioHopScrollBackgrounds(void);
s32 WarioHopCheckPlayerCollision(void);
void WarioHopDrawScoreDigits(void);
s32 WarioHopAddScoreAndCheckMedal(void);
void WarioHopUpdateMedalReward(void);
void WarioHopDrawSprites(void);

#endif

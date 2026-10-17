#ifndef STAGE_RESULTS_H
#define STAGE_RESULTS_H

#include "types.h"

struct AGB_ALIGN StageResultsTreasure {
    u16 x;
    u16 y;
    u16 scale;
    u8 state;
    u8 padding;
};

struct AGB_ALIGN StageResultsIcon {
    u16 x;
    u16 y;
    u8 state;
    u8 padding[3];
};

extern struct StageResultsTreasure gStageResultsTreasure;
extern struct StageResultsIcon gStageResultsTreasureIcons[5];
extern struct StageResultsIcon gStageResultsNewHighScoreIcon;
extern u8 gStageResultsAllJewelPieces;
extern u16 gStageResultsTimer;
extern u16 gStageResultsScoreSoundTimer;
extern s32 gStageResultsTreasureSound;
extern u8 gStageResultsRemainingHearts;
extern s32 gStageResultsAnimationFinished;
extern u8 gStageResultsScoreRank;

extern const u32 sStageResultsTreasureXPositions[];
extern const u8 sStageResultsDigitGfx[];
extern const u8 sStageResultsLanguage0Tilemap[];
extern const u8 sStageResultsLanguage1Tilemap[];

u32 UpdateStageResults(void);
void InitStageResults(void);
void CheckStageResultsJewelSet(void);
void AdvanceStageResultsTreasure(void);
void ShrinkStageResultsTreasure(void);
u32 StageResultsTimerExpired(s32 duration);
void DrawStageResultsScores(void);
u32 TransferStageScoreToTotal(void);
void ConvertStageResultsHeartToScore(void);

#endif

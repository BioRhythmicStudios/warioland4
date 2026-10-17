#ifndef MINIGAME_HOMERUN_DERBY_H
#define MINIGAME_HOMERUN_DERBY_H

#include "types.h"

void InitHomerunDerby(void);
s32 UpdateHomerunDerby(void);
void DrawHomerunDerby(void);
void ResetHomerunDerbyRound(void);
void ResetHomerunDerbyPitch(void);
void SelectHomerunPitchPath(void);
s32 UpdateHomerunPitch(void);
void UpdateHomerunBatInput(void);
s32 CheckHomerunBatHit(void);
s32 UpdateHomerunBatSwing(void);
s32 UpdateHomerunHitBall(void);
s32 UpdateHomerunMissedBall(void);
void DrawHomerunDerbyScore(void);
s32 UpdateHomerunScoreSequence(void);
s32 UpdateHomerunBonusAnimation(void);
void StartHomerunBonusAnimation(void);

#endif  // MINIGAME_HOMERUN_DERBY_H

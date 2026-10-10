#ifndef SPRITE_AI_CHANCE_WHEEL_H
#define SPRITE_AI_CHANCE_WHEEL_H

#include "sprite.h"
#include "types.h"

extern const u8 sBigBoardSpaces[];
extern const struct AnimationFrame sChanceWheelIdleOam[];
extern const struct AnimationFrame sChanceWheelResult1Oam[];
extern const struct AnimationFrame sChanceWheelResult2Oam[];
extern const struct AnimationFrame sChanceWheelResult3Oam[];
extern const struct AnimationFrame sChanceWheelResult4Oam[];
extern const struct AnimationFrame sChanceWheelResult5Oam[];
extern const struct AnimationFrame sChanceWheelResult6Oam[];
extern const struct AnimationFrame sChanceWheelRewardEffectOam[];
extern const struct AnimationFrame sGoalBlockIdleOam[];
extern const struct AnimationFrame sGoalBlockResult1Oam[];
extern const struct AnimationFrame sGoalBlockResult2Oam[];
extern const struct AnimationFrame sGoalBlockResult3Oam[];
extern const struct AnimationFrame sGoalBlockResult4Oam[];
extern const struct AnimationFrame sGoalBlockResult5Oam[];
extern const struct AnimationFrame sGoalBlockResult6Oam[];
extern const s16 sChanceWheelDropYVelocity[];

void ClearChanceWheelRewardSprites(void);
void TransformWarioFromChanceWheel(void);
void PlayChanceWheelSpaceSound(void);
void ApplyChanceWheelReward(void);
int IsChanceWheelResultReady(void);
void SetChanceWheelSecondaryResult(u8 value);
void CheckWarioNearChanceWheel(void);
void InitChanceWheel(void);
void StartChanceWheelResult(void);
void ChanceWheelDropToResult(void);
void FinishChanceWheelResult(void);
void ChanceWheelShrink(void);
void ResetGoalBlock(void);
void GoalBlockShrink(void);
void InitChanceWheelRewardEffect(void);
void UpdateChanceWheelRewardEffect(void);
void SpriteChanceWheel(void);
void SpriteChanceWheelRewardEffect(void);

#endif  // SPRITE_AI_CHANCE_WHEEL_H

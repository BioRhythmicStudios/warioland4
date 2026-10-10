#ifndef SPRITE_AI_PINBALL_H
#define SPRITE_AI_PINBALL_H

#include "oam.h"

extern const struct AnimationFrame sPinballOam[];
extern const struct AnimationFrame sPinballLockUpResetOam[];
extern const struct AnimationFrame sPinballLockUpWaitingOam[];
extern const struct AnimationFrame sPinballLockUpMovingOam[];
extern const struct AnimationFrame sPinballLockUpOpenOam[];
extern const struct AnimationFrame sPinballLockUpDisappearOam[];
extern const struct AnimationFrame sPinballLockDownResetOam[];
extern const struct AnimationFrame sPinballLockDownWaitingOam[];
extern const struct AnimationFrame sPinballLockDownMovingOam[];
extern const struct AnimationFrame sPinballLockDownOpenOam[];
extern const struct AnimationFrame sPinballLockDownDisappearOam[];
extern const struct AnimationFrame sPinballLockLeftResetOam[];
extern const struct AnimationFrame sPinballLockLeftWaitingOam[];
extern const struct AnimationFrame sPinballLockLeftMovingOam[];
extern const struct AnimationFrame sPinballLockLeftOpenOam[];
extern const struct AnimationFrame sPinballLockRightResetOam[];
extern const struct AnimationFrame sPinballLockRightWaitingOam[];
extern const struct AnimationFrame sPinballLockRightMovingOam[];
extern const struct AnimationFrame sPinballLockLeftDisappearOam[];
extern const struct AnimationFrame sPinballLockRightOpenOam[];
extern const struct AnimationFrame sPinballCounterFourOam[];
extern const struct AnimationFrame sPinballCounterThreeOam[];
extern const struct AnimationFrame sPinballCounterTwoOam[];
extern const struct AnimationFrame sPinballCounterOneOam[];
extern const struct AnimationFrame sPinballCounterZeroOam[];
extern const struct AnimationFrame sPinballCompletionSparkleOam[];
extern const struct AnimationFrame sPinballLockRightDisappearOam[];
extern const struct AnimationFrame sPinballCounterDisappearOam[];

void SpritePinball(void);
void SpritePinballLockUp(void);
void SpritePinballLockDown(void);
void SpritePinballLockLeft(void);
void SpritePinballLockRight(void);
void SpritePinballCounter(void);
void SpritePinballCompletionSparkle(void);

#endif  // SPRITE_AI_PINBALL_H

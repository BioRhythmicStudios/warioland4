#ifndef SPRITE_AI_TOBAWANI_H
#define SPRITE_AI_TOBAWANI_H

#include "sprite.h"
#include "oam.h"
#include "types.h"

extern const struct AnimationFrame sTobawaniPatrolOam[];
extern const struct AnimationFrame sTobawaniTurnOam[];
extern const struct AnimationFrame sTobawaniChargeOam[];
extern const struct AnimationFrame sTobawaniRecoveryOam[];
extern const struct AnimationFrame sTobawaniKnockbackOam[];
extern const struct AnimationFrame sTobawaniFallingOam[];
extern const struct AnimationFrame sTobawaniDefeatedOam[];
extern const s16 sTobawaniKnockbackYVelocity[];
extern const s16 sTobawaniFallYVelocity[];
extern const s16 sTobawaniWallFallYVelocity[];

void SpriteTobawani(void);

#endif  // SPRITE_AI_TOBAWANI_H

#ifndef SPRITE_AI_PIG_HEAD_STATUE_H
#define SPRITE_AI_PIG_HEAD_STATUE_H

#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sPigHeadStatueIdleOam[];
extern const struct AnimationFrame sPigHeadStatueOpeningOam[];
extern const struct AnimationFrame sPigHeadStatueFiringOam[];
extern const struct AnimationFrame sPigHeadStatueChildFallingOam[];
extern const struct AnimationFrame sPigHeadStatueChildLandedOam[];
extern const struct AnimationFrame sPigHeadStatueChildDisappearingOam[];

void InitPigHeadStatue(void);
void PigHeadStatueWaitToOpen(void);
void PigHeadStatueWaitToFire(void);
void PigHeadStatueFireProjectile(void);
void PigHeadStatueStartCooldown(void);
void PigHeadStatueCooldown(void);
void InitPigHeadProjectile(void);
void PigHeadProjectileFall(void);
void PigHeadProjectileStartDespawn(void);
void PigHeadProjectileDespawn(void);
void SpritePigHeadStatue(void);
void SpritePigHeadProjectile(void);

#endif  // SPRITE_AI_PIG_HEAD_STATUE_H

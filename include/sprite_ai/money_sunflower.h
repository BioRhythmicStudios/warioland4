#ifndef SPRITE_AI_MONEY_SUNFLOWER_H
#define SPRITE_AI_MONEY_SUNFLOWER_H

#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sMoneySunflowerFullOam[];
extern const struct AnimationFrame sMoneySunflowerLargeOam[];
extern const struct AnimationFrame sMoneySunflowerMediumOam[];
extern const struct AnimationFrame sMoneySunflowerSmallOam[];
extern const struct AnimationFrame sMoneySunflowerDepletedOam[];

#ifdef PORTABLE
void SpawnScoreCoinDropSprites(u32 y, u32 x, s32 amount);
#else
void SpawnScoreCoinDropSprites(u16 yPosition, u16 xPosition, u8 amount);
#endif

void InitMoneySunflower(void);
void SetMoneySunflowerFull(void);
void SetMoneySunflowerLarge(void);
void SetMoneySunflowerMedium(void);
void SetMoneySunflowerSmall(void);
void SetMoneySunflowerDepleted(void);
void AdvanceMoneySunflowerStage(void);
void SpriteMoneySunflower(void);

#endif  // SPRITE_AI_MONEY_SUNFLOWER_H

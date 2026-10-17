#ifndef SPRITE_AI_CHANDELIER_H
#define SPRITE_AI_CHANDELIER_H

#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sChandelierIdleOam[];
extern const struct AnimationFrame sChandelierShakingOam[];
extern const struct AnimationFrame sChandelierFallingOam[];
extern const struct AnimationFrame sChandelierFallenOam[];
extern const s16 sChandelierFallVelocity[];

void InitChandelier(void);
void ChandelierStartShaking(void);
void ChandelierShake(void);
void ChandelierFall(void);
void InitFallenChandelier(void);
void FallenChandelierWait(void);
void FallenChandelierDisappear(void);
void SpriteChandelier(void);

#endif  // SPRITE_AI_CHANDELIER_H

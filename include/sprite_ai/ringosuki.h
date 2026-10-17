#ifndef SPRITE_AI_RINGOSUKI_H
#define SPRITE_AI_RINGOSUKI_H

#include "types.h"
#include "oam.h"

extern const struct AnimationFrame sRingosukiIdleOam[];
extern const struct AnimationFrame sRingosukiTurnStartOam[];
extern const struct AnimationFrame sRingosukiTurnEndOam[];
extern const struct AnimationFrame sRingosukiThrowAppleOam[];
extern const struct AnimationFrame sRingosukiStunnedOam[];
extern const struct AnimationFrame sRingosukiFallingOam[];
extern const struct AnimationFrame sRingosukiRecoverOam[];
extern const struct AnimationFrame sRingosukiDefeatedOam[];
extern const struct AnimationFrame sRingosukiDefeatedHardOam[];
extern const struct AnimationFrame sRingosukiPushedOam[];
extern const struct AnimationFrame sRingosukiStunnedStartOam[];
extern const struct AnimationFrame sRingosukiEatAppleOam[];
extern const struct AnimationFrame sRingosukiFallOffscreenOam[];
extern const struct AnimationFrame sRingosukiTurnHopOam[];
extern const struct AnimationFrame sRingosukiAppleOam[];
extern const s16 sRingosukiTurnHopYVelocity[];
extern const s16 sRingosukiRecoverYVelocity[];
extern const s16 sRingosukiAppleYVelocity[];

void SpriteRingosuki(void);
void SpriteRingosukiApple(void);

#endif  // SPRITE_AI_RINGOSUKI_H

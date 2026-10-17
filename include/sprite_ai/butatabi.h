#ifndef SPRITE_AI_BUTATABI_H
#define SPRITE_AI_BUTATABI_H

#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sButatabiMovingOam[];
extern const struct AnimationFrame sButatabiTurningOam[];
extern const struct AnimationFrame sButatabiTurnAroundOam[];
extern const struct AnimationFrame sButatabiFallingOam[];
extern const s16 sButatabiXVelocity[];
extern const s16 sButatabiYVelocity[];

void SetButatabiHitbox(void);
void InitButatabi(void);
void StartButatabiTackled(void);
void InitButatabiTackledRight(void);
void InitButatabiTackledLeft(void);
void StartButatabiBumped(void);
void InitButatabiBumpedRight(void);
void InitButatabiBumpedLeft(void);
void ButatabiHop(void);
void SpriteButatabi(void);

#endif  // SPRITE_AI_BUTATABI_H

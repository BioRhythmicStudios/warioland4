#ifndef SPRITE_AI_FLYING_CARPET_H
#define SPRITE_AI_FLYING_CARPET_H

#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sMagicCarpetRiddenOam[];
extern const struct AnimationFrame sMagicCarpetFlyingOam[];
extern const struct AnimationFrame sMagicCarpetIdleOam[];
extern const s16 sMagicCarpetVerticalVelocity[];

volatile long UpdateMagicCarpetCollisionTimer(void);
void InitMagicCarpet(void);
void SetMagicCarpetMovementState(void);
void MagicCarpetWaitForWario(void);
void MagicCarpetHandleStandingWario(void);
void MagicCarpetRiseAndMove(void);
void MagicCarpetMoveUnridden(void);
void MagicCarpetMoveWithWario(void);
void SpriteMagicCarpet(void);

#endif  // SPRITE_AI_FLYING_CARPET_H

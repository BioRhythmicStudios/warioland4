#ifndef WARIO_SNOWMAN_H
#define WARIO_SNOWMAN_H

#include "global_data.h"
#include "types.h"

extern GBA_PTR(const struct WarioAnimationFrame*) const sSnowmanWarioAnimationTable[][2];
extern const struct WarioPoseProperty sSnowmanWarioPoseProperties[];
extern const u16 sSnowmanWarioPalette[];

extern const struct WarioAnimationFrame sSnowmanWarioWalkingAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioBonkAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioJumpingAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioTransformingAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioLandingAfterMidairSnowContactAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioTurningAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioRollingSmallAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioRollingMediumAnimation[];
extern const struct WarioAnimationFrame sSnowmanWarioRollingLargeAnimation[];

u8 UpdateSnowmanWario(void);
u8 SnowmanWarioTransforming(void);
u8 SnowmanWarioFallingWithSnow(void);
u8 SnowmanWarioWalking(void);
u8 SnowmanWarioBonk(void);
u8 SnowmanWarioJumping(void);
u8 SnowmanWarioFalling(void);
u8 SnowmanWarioLanding(void);
u8 SnowmanWarioLandingAfterMidairSnowContact(void);
u8 SnowmanWarioUnknown8(void);
u8 SnowmanWarioTurning(void);
u8 SnowmanWarioRollingSmall(void);
u8 SnowmanWarioRollingSmallMidair(void);
u8 SnowmanWarioRollingMedium(void);
u8 SnowmanWarioRollingMediumMidair(void);
u8 SnowmanWarioRollingLarge(void);
u8 SnowmanWarioRollingLargeMidair(void);
void SetSnowmanWarioPose(u8 result);
void UpdateSnowmanWarioMovement(void);
void ProcessSnowmanWarioCollision(void);
void UpdateSnowmanWarioGraphics(u8 variant);
void ApplySnowmanWarioMusicEffect(void);
void UpdateSnowmanWarioHitbox(void);

#endif  // WARIO_SNOWMAN_H

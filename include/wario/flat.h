#ifndef WARIO_FLAT_H
#define WARIO_FLAT_H

#include "global_data.h"

extern const struct WarioAnimationFrame sFlatWarioTransformingAnimation[];
extern const struct WarioAnimationFrame sFlatWarioUnknown1Animation[];
extern const struct WarioAnimationFrame sFlatWarioFloatingAnimation[];
extern const struct WarioAnimationFrame sFlatWarioStandingAnimation[];
extern const struct WarioAnimationFrame sFlatWarioWalkingAnimation[];
extern const struct WarioAnimationFrame sFlatWarioDetransformingAnimation[];
extern const u16 sFlatWarioHorizontalMovement[];
extern const u16 sFlatWarioVerticalMovement[];
extern const struct WarioPoseProperty sFlatWarioPoseProperties[];
extern GBA_PTR(const struct WarioAnimationFrame*) sFlatWarioAnimationTable[][2];

u8 UpdateFlatWario(void);
u8 FlatWarioTransforming(void);
u8 FlatWarioUnknown1(void);
u8 FlatWarioStanding(void);
u8 FlatWarioWalking(void);
u8 FlatWarioFloating(void);
u8 FlatWarioJumping(void);
u8 FlatWarioDetransforming(void);
void SetFlatWarioPose(u8 pose);
void UpdateFlatWarioMovement(void);
void ProcessFlatWarioCollision(void);
void LoadFlatWarioGraphics(u8 animationSide);
void ApplyFlatWarioMusicEffects(void);
void UpdateFlatWarioHitbox(void);

#endif  // WARIO_FLAT_H

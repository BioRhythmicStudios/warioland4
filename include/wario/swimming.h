#ifndef WARIO_SWIMMING_H
#define WARIO_SWIMMING_H

#include "global_data.h"
#include "wario.h"

extern const struct WarioAnimationFrame sWarioSwimmingHorizontalAnimation[];
extern const struct WarioAnimationFrame sWarioSwimmingVerticalAnimation[];
extern const struct WarioAnimationFrame sWarioSwimmingFreelyAnimation[];
extern const struct WarioAnimationFrame sWarioFloatingAnimation[];
extern const struct WarioAnimationFrame sWarioJumpingOutOfWaterAnimation[];
extern const struct WarioAnimationFrame sBubbleWarioAnimation[];
extern GBA_PTR(const struct WarioAnimationFrame*) const sWarioWaterAnimationTable[];
extern GBA_PTR(const struct WarioAnimationFrame*) const sWarioWaterVerticalAnimationTable[];
extern const u8 sWarioWaterPoseProperties[];
extern const u16 sBubbleWarioPalette[];
extern const u16 sWarioSecondaryPalette[];

u8 UpdateWarioWaterPose(void);
u8 UpdateWarioWaterLanding(void);
u8 UpdateWarioSwimmingHorizontal(void);
u8 UpdateWarioSwimmingVertical(void);
u8 UpdateWarioSwimmingFreely(void);
u8 UpdateWarioFloating(void);
u8 UpdateWarioJumpingOutOfWater(void);
u8 UpdateWarioWaterDamage(void);
u8 UpdateWarioWaterSuperGroundPound(void);
u8 UpdateWarioWaterSuperGroundPoundLanding(void);
u8 UpdateWarioWaterHorizontalBonk(void);
u8 UpdateWarioWaterVerticalBonk(void);
u8 UpdateWarioWaterTransition(void);
u8 UpdateFatWarioFallingInWater(void);
u8 UpdateFatWarioLandingInWater(void);
u8 UpdateBubbleWarioInWater(void);
u8 UpdateWarioWaterRecovery(void);
void SetWarioWaterPose(u8 pose);
void UpdateWarioWaterPhysics(void);
void ProcessWarioWaterCollision(void);
u8 ResolveWarioWaterCollision(void);
u8 ResolveWarioWaterSurfaceCollision(void);
u8 ResolveWarioWaterCollisionContinuation(void);
void UpdateWarioWaterAnimation(u8 direction);
void UpdateBubbleWarioMusic(void);
void UpdateWarioWaterHitbox(void);

#endif  // WARIO_SWIMMING_H

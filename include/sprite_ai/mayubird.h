#ifndef SPRITE_AI_MAYUBIRD_H
#define SPRITE_AI_MAYUBIRD_H

#include "oam.h"
#include "types.h"

extern const struct AnimationFrame sMayubirdIdleOam[];
extern const struct AnimationFrame sMayubirdFlightEndOam[];
extern const struct AnimationFrame sMayubirdWaitOam[];
extern const struct AnimationFrame sMayubirdFlightOam[];
extern const struct AnimationFrame sMayubirdFallOam[];
extern const s16 sMayubirdFloatXVelocity[];
extern const s16 sMayubirdFloatYVelocity[];
extern const s16 sMayubirdFlyLeftXVelocity[];
extern const s16 sMayubirdFlyRightXVelocity[];

void SpriteMayubird(void);

#endif  // SPRITE_AI_MAYUBIRD_H

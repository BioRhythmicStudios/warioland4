#ifndef SPRITE_AI_ICICLE_H
#define SPRITE_AI_ICICLE_H

#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sIcicleIdleOam[];
extern const struct AnimationFrame sIcicleWarningOam[];
extern const struct AnimationFrame sIcicleFallingOam[];
extern const struct AnimationFrame sIcicleImpactOam[];

void SpriteIcicle(void);
void SpriteIcicleWarning(void);

#endif  // SPRITE_AI_ICICLE_H

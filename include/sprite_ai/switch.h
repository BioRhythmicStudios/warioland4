#ifndef SPRITE_AI_SWITCH_H
#define SPRITE_AI_SWITCH_H

#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sSwitchIdleOam[];
extern const struct AnimationFrame sEscapeSwitchPressedOam[];
extern const struct AnimationFrame sEscapeSwitchActiveOam[];
extern const struct AnimationFrame sSwitchReleasedOam[];
extern const struct AnimationFrame sSwitchPressedOam[];
extern const struct AnimationFrame sSwitchEffectIdleOam[];
extern const struct AnimationFrame sEscapeSwitchWaitingOam[];
extern const struct AnimationFrame sEscapeSwitchIdleOam[];
extern const struct AnimationFrame sSwitchEffectActiveOam[];

void SpriteSwitch(void);
void SpriteSwitchEffect(void);

#endif  // SPRITE_AI_SWITCH_H

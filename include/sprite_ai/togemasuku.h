#ifndef SPRITE_AI_TOGEMASUKU_H
#define SPRITE_AI_TOGEMASUKU_H

#include "types.h"
#include "oam.h"

struct AnimationFrame;

extern const struct AnimationFrame sTogemasukuIdleOam[];
extern const struct AnimationFrame sTogemasukuDefeatedOam[];
extern const struct AnimationFrame sTogemasukuRespawningOam[];

void InitTogemasuku(void);
void StartTogemasukuRespawn(void);
void TogemasukuRespawnDelay(void);
void RestoreTogemasukuIdle(void);
void DefeatTogemasuku(void);
void StartTogemasukuFallRight(void);
void StartTogemasukuFallLeft(void);
void KillTogemasuku(void);
void SpriteTogemasuku(void);

#endif  // SPRITE_AI_TOGEMASUKU_H

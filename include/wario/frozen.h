#ifndef WARIO_FROZEN_H
#define WARIO_FROZEN_H

#include "types.h"

struct AGB_ALIGN FrozenAnimationFrame {
    GBA_PTR(const u16*) oam;
    u32 unk4;
    u8 time;
};

struct AGB_ALIGN FrozenGraphicsFrame {
    GBA_PTR(const u8*) data;
    GBA_PTR(const u16*) oam;
    u32 unk8;
};

extern const struct FrozenAnimationFrame sFrozenTransformAnimation[];
extern const struct FrozenAnimationFrame sFrozenBonkAnimation[];
extern GBA_PTR(const struct FrozenGraphicsFrame*) sFrozenGraphicsTable[];
extern const u8 sFrozenPoseData[];
extern const u8 sFrozenPalette[];

u8 UpdateFrozenWario(void);
u8 FrozenWarioTransforming(void);
u8 FrozenWarioSlideLeft(void);
u8 FrozenWarioSlideRight(void);
u8 FrozenWarioBonk(void);
void SetFrozenWarioPose(int pose);
void UpdateFrozenWarioMovement(void);
void ProcessFrozenWarioCollision(void);
void LoadFrozenWarioGraphics(int index);
void ApplyFrozenWarioMusicEffects(void);
void UpdateFrozenWarioHitbox(void);

#endif  // WARIO_FROZEN_H

#ifndef WARIO_PUFFY_H
#define WARIO_PUFFY_H

#include "types.h"

struct AGB_ALIGN PuffyAnimationFrame {
    GBA_PTR(const u16*) oam;
    u32 unk4;
    u8 time;
};

struct AGB_ALIGN PuffyGraphicsFrame {
    GBA_PTR(const u8*) data;
    GBA_PTR(const u16*) oam;
    u32 unk8;
};

extern const struct PuffyAnimationFrame sPuffyTransformingAnimation[];
extern const struct PuffyAnimationFrame sPuffyFloatingAnimation[];
extern const struct PuffyAnimationFrame sPuffyHitCeilingAnimation[];
extern const struct PuffyAnimationFrame sPuffyInhalingAnimation[];
extern GBA_PTR(const struct PuffyGraphicsFrame*) sPuffyGraphicsTable[];
extern const u8 sPuffyPoseData[];
extern const u8 sPuffyPalette[];

u8 UpdatePuffyWario(void);
u8 PuffyWarioTransforming(void);
u8 PuffyWarioFloating(void);
u8 PuffyWarioInhaling(void);
u8 PuffyWarioHitCeiling(void);
void SetPuffyWarioPose(u8 pose);
void UpdatePuffyWarioPosition(void);
void UpdatePuffyWarioCollision(void);
void LoadPuffyWarioGraphics(int index);
void ApplyPuffyWarioMusicEffect(void);
void UpdatePuffyWarioHitbox(void);

#endif  // WARIO_PUFFY_H

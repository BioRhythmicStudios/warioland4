#ifndef WARIO_MASK_H
#define WARIO_MASK_H

#include "types.h"
#include "wario.h"

struct AGB_ALIGN MaskWarioGraphicsFrame {
    GBA_PTR(const u8*) gfx;
    GBA_PTR(const u16*) oam;
    u8 time;
    u8 pad_09[3];
};

extern GBA_PTR(const struct MaskWarioGraphicsFrame*) const sMaskWarioGraphicsTable[][2];
extern const u8 sMaskWarioPoseProperties[][8];
extern const u8 sMaskWarioPalette[];
extern const struct MaskWarioGraphicsFrame sMaskWarioTransformAnimation[];

u8 UpdateMaskWario(void);
u8 MaskWarioTransforming(void);
u8 MaskWarioAirborne(void);
void SetMaskWarioPose(u8 pose);
void UpdateMaskWarioMotion(void);
void ProcessMaskWarioCollision(void);
void LoadMaskWarioGraphics(u8 variant);
void UpdateMaskWarioAnimation(void);
void UpdateMaskWarioHitbox(void);

#endif  // WARIO_MASK_H

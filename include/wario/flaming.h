#ifndef WARIO_FLAMING_H
#define WARIO_FLAMING_H

#include "types.h"
#include "wario.h"

struct AGB_ALIGN FlamingWarioGraphicsFrame {
    GBA_PTR(const u8*) gfx;
    GBA_PTR(const u16*) oam;
    u8 time;
    u8 pad_09[3];
};

extern GBA_PTR(const struct FlamingWarioGraphicsFrame*) const sFlamingWarioGraphicsTable[][2];
extern const u8 sFlamingWarioPoseProperties[][8];
extern const u8 sFlamingWarioBluePalette[];
extern const u8 sFlamingWarioGreenPalette[];
extern const u8 sFlamingWarioRedPalette[];
extern const u8 sFlamingWarioEngulfedPalette1[];
extern const u8 sFlamingWarioEngulfedPalette2[];
extern const u8 sFlamingWarioEngulfedPalette3[];
extern const u8 sFlamingWarioRecoveringPalette[];
extern const struct FlamingWarioGraphicsFrame sFlamingWarioBlueFrames[];
extern const struct FlamingWarioGraphicsFrame sFlamingWarioGreenFrames[];
extern const struct FlamingWarioGraphicsFrame sFlamingWarioRedFrames[];
extern const struct FlamingWarioGraphicsFrame sFlamingWarioEngulfedFrames[];
extern const struct FlamingWarioGraphicsFrame sFlamingWarioRecoveringFrames[];

extern void UpdateWarioHorizontalCollisionOffset();
extern s32 GetAdjustedWarioXVelocity();
extern u8 func_80143D8();
extern u8 func_8014758();

u8 UpdateFlamingWario(void);
u8 FlamingWarioBlue(void);
u8 FlamingWarioBlueMidair(void);
u8 FlamingWarioGreen(void);
u8 FlamingWarioGreenMidair(void);
u8 FlamingWarioRed(void);
u8 FlamingWarioRedMidair(void);
u8 FlamingWarioEngulfed(void);
u8 FlamingWarioEngulfedMidair(void);
u8 FlamingWarioRecovering(void);
void SetFlamingWarioPose(u8 pose);
void UpdateFlamingWarioMotion(void);
void ProcessFlamingWarioCollision(void);
void LoadFlamingWarioGraphics(int variant);
void UpdateFlamingWarioMusic(void);
void UpdateFlamingWarioHitbox(void);

#endif  // WARIO_FLAMING_H

#ifndef WARIO_BAT_H
#define WARIO_BAT_H

#include "types.h"

struct AGB_ALIGN BatAnimationFrame {
    GBA_PTR(const u16*) oam;
    u32 unk4;
    u8 time;
};

struct AGB_ALIGN BatGraphicsFrame {
    GBA_PTR(const u8*) data;
    GBA_PTR(const u16*) oam;
    u32 unk8;
};

extern const struct BatAnimationFrame sBatTransformAnimation[];
extern const struct BatAnimationFrame sBatFlyAnimation[];
extern const struct BatAnimationFrame sBatLandAnimation[];
extern const struct BatAnimationFrame sBatTurnAnimation[];
extern const struct BatAnimationFrame sBatRecoverAnimation[];
extern GBA_PTR(const struct BatGraphicsFrame*) sBatGraphicsTable[];
extern const u8 sBatPoseData[];
extern const u8 sBatPalette[];

u8 UpdateBatWario(void);
u8 BatWarioTransforming(void);
u8 BatWarioFlying(void);
u8 BatWarioLanding(void);
u8 BatWarioTurning(void);
u8 BatWarioRecovering(void);
void SetBatWarioPose(int pose);
void UpdateBatWarioMovement(void);
void ProcessBatWarioCollision(void);
void LoadBatWarioGraphics(int index);
void ApplyBatWarioMusicEffects(void);
void UpdateBatWarioHitbox(void);

#endif  // WARIO_BAT_H

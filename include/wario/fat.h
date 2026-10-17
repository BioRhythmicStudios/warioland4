#ifndef WARIO_FAT_H
#define WARIO_FAT_H

#include "types.h"
#include "wario.h"

struct AGB_ALIGN FatWarioGraphicsFrame {
    GBA_PTR(const u8*) gfx;
    GBA_PTR(const u16*) oam;
    u8 time;
    u8 pad_09[3];
};

extern GBA_PTR(const struct FatWarioGraphicsFrame*) const sFatWarioGraphicsTable[][2];
extern const u8 sFatWarioPoseProperties[][8];
extern const struct FatWarioGraphicsFrame sFatWarioTransformingFrames[];
extern const struct FatWarioGraphicsFrame sFatWarioTurningFrames[];
extern const struct FatWarioGraphicsFrame sFatWarioWalkingFrames[];
extern const struct FatWarioGraphicsFrame sFatWarioJumpingFrames[];
extern const struct FatWarioGraphicsFrame sFatWarioKillingEnemyFrames[];
extern const struct FatWarioGraphicsFrame sFatWarioDetransformingFrames[];

extern int func_8014268(u8, u16, void*);
extern void UpdateWarioHorizontalCollisionOffset();
extern s32 GetAdjustedWarioXVelocity();
extern u8 func_8014758();
extern u8 func_80143D8();

u8 UpdateFatWario(void);
u8 FatWarioTransforming(void);
u8 FatWarioTransformingMidair(void);
u8 FatWarioTurning(void);
u8 FatWarioWalking(void);
u8 FatWarioJumping(void);
u8 FatWarioFalling(void);
u8 FatWarioLanding(void);
u8 FatWarioKillingEnemy(void);
u8 FatWarioDetransforming(void);
u8 FatWarioStartingWalkAfterLanding(void);
u8 FatWarioIdleInput(void);
void SetFatWarioPose(u8 pose);
void UpdateFatWarioMotion(void);
void ProcessFatWarioCollision(void);
void LoadFatWarioGraphics(int variant);
void UpdateFatWarioMusic(void);
void UpdateFatWarioHitbox(void);

#endif

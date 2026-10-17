#ifndef WARIO_ZOMBIE_H
#define WARIO_ZOMBIE_H

#include "types.h"
#include "wario.h"

struct AGB_ALIGN ZombieWarioGraphicsFrame {
    GBA_PTR(const u8*) gfx;
    GBA_PTR(const u16*) oam;
    u8 time;
    u8 pad_09[3];
};

extern GBA_PTR(const struct ZombieWarioGraphicsFrame*) const sZombieWarioGraphicsTable[][2];
extern const u8 sZombieWarioPoseProperties[][8];
extern const u8 sZombieWarioPalette[];
extern const u8 sZombieWarioPoseUnk05Values[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioWalkingFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioEmergingFromHatFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioJumpingFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioFallingThroughPlatformFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioLandingFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioHatLandingFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioFallingFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioHatFallingFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioTouchingLightFrames[];
extern const struct ZombieWarioGraphicsFrame sZombieWarioUnknown11Frames[];

extern int GetBackgroundCollisionAtPosition(u16, u16);

u8 UpdateZombieWario(void);
u8 ZombieWarioTransforming(void);
u8 ZombieWarioWalking(void);
u8 ZombieWarioEmergingFromHat(void);
u8 ZombieWarioJumping(void);
u8 ZombieWarioFalling(void);
u8 ZombieWarioUnknown5(void);
u8 ZombieWarioFallingThroughPlatform(void);
u8 ZombieWarioLanding(void);
u8 ZombieWarioHatLanding(void);
u8 ZombieWarioHatFalling(void);
u8 ZombieWarioTouchingLightSource(void);
u8 ZombieWarioUnknown11(void);
u8 ZombieWarioFallingAfterJump(void);
void SetZombieWarioPose(u8 pose);
void UpdateZombieWarioMotion(void);
void ProcessZombieWarioCollision(void);
int CheckZombieWarioFloor(void);
void LoadZombieWarioGraphics(int variant);
void UpdateZombieWarioMusic(void);
void UpdateZombieWarioHitbox(void);

#endif  // WARIO_ZOMBIE_H

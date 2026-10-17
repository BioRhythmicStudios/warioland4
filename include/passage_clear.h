#ifndef PASSAGE_CLEAR_H
#define PASSAGE_CLEAR_H

#include "types.h"

struct AGB_ALIGN PassageClearAnimationState {
    u16 timer;
    u16 frame;
    u8 state;
    u8 padding[3];
};

struct AGB_ALIGN PassageClearParticle {
    s16 x;
    s16 y;
    u8 type;
    u8 padding[3];
};

/* Both coordinates are 12.4 fixed point. */
struct AGB_ALIGN PassageClearBonusItem {
    s16 x;
    s16 y;
};

s32 UpdatePassageClearSequence(void);
void InitializePassageClear(void);
void UpdatePassageClearBanner(void);
void UpdatePassageClearBlendFade(void);
void UpdatePassageClearPaletteCycle(void);
void SpawnPassageClearParticles(void);
void RenderPassageClearOam(void);

extern u8 gPassageClearBlendFadeActive;
extern u8 gPassageClearPaletteCycleState;
extern struct PassageClearAnimationState gPassageClearMainAnimationState;
extern struct PassageClearAnimationState gPassageClearSecondaryAnimationState;
extern struct PassageClearBonusItem gPassageClearBonusItemPositions[];
extern u8 gPassageClearBonusAnimationActive;
extern u16 gPassageClearBonusAnimationTimer;
extern u16 gPassageClearBonusAnimationFrame;
extern struct PassageClearParticle gPassageClearSlowParticles[];
extern struct PassageClearParticle gPassageClearMediumParticles[];
extern struct PassageClearParticle gPassageClearFastParticles[];
extern u8 gPassageClearParticleAffineAngles[];
extern u8 gPassageClearSlowParticleIndex;
extern u8 gPassageClearMediumParticleIndex;
extern u8 gPassageClearFastParticleIndex;
extern GBA_PTR(const u16*) gPassageClearBannerOam;

#endif  // PASSAGE_CLEAR_H

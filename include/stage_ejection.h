#ifndef STAGE_EJECTION_H
#define STAGE_EJECTION_H

#include "global_data.h"

struct AGB_ALIGN StageEjectionParticle {
    s16 xVelocity;
    s16 yVelocity;
    u16 x;
    u16 y;
    u16 animationTimer;
    u16 frame;
    u8 active;
    u8 padding[3];
};

struct AGB_ALIGN StageEjectionTreasureParticle {
    u16 xVelocity;
    u16 yVelocity;
    u16 x;
    u16 y;
    u16 unused8;
    u16 unusedA;
    u8 active;
    u8 padding[3];
};

extern struct StageEjectionParticle gStageEjectionParticles[];
extern struct StageEjectionTreasureParticle gStageEjectionTreasureParticles[];
extern u8 gStageEjectionParticleIndex;
extern u8 gStageEjectionParticleSpawnCount;
extern u8 gStageEjectionParticleSpawnTimer;
extern u8 gStageEjectionTreasureIndex;

s32 UpdateStageEjectionExitState(void);
void InitializeStageEjection(void);
void UpdateStageEjectionParticles(void);
void UpdateStageEjectionTreasureParticles(void);
void AdvanceStageEjectionTreasureIndex(void);
void ClearStageEjectionCollectionFlags(void);
void UpdateStageEjectionArcMovement(void);
void DrawStageEjection(void);
void UpdateStageEjectionEffects(void);

#endif

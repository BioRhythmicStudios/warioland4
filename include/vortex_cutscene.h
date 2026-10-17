#ifndef VORTEX_CUTSCENE_H
#define VORTEX_CUTSCENE_H

#include "types.h"

#ifndef AGBCC_SPLIT_REGISTER_LIFETIME
#define AGBCC_SPLIT_REGISTER_LIFETIME(value) \
    ASM_BARRIER_VOLATILE(: "=r"(value))
#endif

u32 UpdateVortexCutscene(void);
void InitializeVortexCutsceneResources(void);
void InitializeVortexCutsceneFromPortal(void);
void InitializeVortexCutsceneFromPassage(void);
u32 UpdateVortexCutsceneMainSprite(void);
u32 UpdateVortexCutsceneKeyzerSprite(void);
void UpdateVortexCutsceneAffine(void);
void UpdateVortexCutscenePaletteCycle(void);
void ApplyVortexCutsceneAffineRegisters(void);
void RenderVortexCutsceneOam(void);

#endif

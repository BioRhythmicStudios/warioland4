#ifndef MINIGAME_TRANSITION_H
#define MINIGAME_TRANSITION_H

#include "gba.h"
#include "oam.h"
#include "types.h"

void SubGameInitAndDispatch(u32 subGameMode);
void SubGameClearGraphicsMemory(void);
void SubGameWaitAndAdvance(u16 frame);

void InitializeAffineCutsceneBackground(void);
void InitializeVariantCutsceneBackground(void);
void InitializeCutsceneBackgroundGraphics(void);
void InitializeLayeredScrollCutscene(void);
void InitializeWindowedCutsceneBackgrounds(void);
void InitializeScaledSpriteCutscene(void);
void InitializeThreeLayerCutsceneBackground(void);
void InitializeThreeLayerCutsceneBackgroundWithSprites(void);
void InitializeSubGameTransitionGraphics(void);
void func_8006680(void);
void InitializeLayeredWarioCutscene(void);
void InitializeAffineWarioCutscene(void);
void InitializeLayeredObjectCutscene(void);
void InitializeEndingTreasureCutscene(void);
void InitializeEndingBackgroundSetup(void);
void InitializeEndingCutscene(void);
void InitializeTitleScreenLogoCutscene(void);
void WaitForVBlankInterrupt(u32 subGameMode);
void ResetCutsceneBackgroundRegisters(void);

#endif  // MINIGAME_TRANSITION_H

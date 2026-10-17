#ifndef PAUSE_H
#define PAUSE_H

#include "global_data.h"

struct AGB_ALIGN PauseScreenState {
    u16 animationTimer;
    u16 animationFrame;
    u8 selection;
    u8 confirmedSelection;
};

typedef struct AGB_ALIGN PauseAnimationState {
    u16 animationTimer;
    u16 animationFrame;
} PauseAnimationState;

extern GBA_PTR(const struct AnimationFrame*) gPauseScreenAnimation;
extern struct PauseScreenState gPauseScreenState;
extern PauseAnimationState gPauseJewelAnimationStates[4];
extern PauseAnimationState gPauseCdAnimationState;
extern PauseAnimationState gPauseKeyzerAnimationState;
extern u8 gPauseJewelPieceStates[4];
extern u8 gPauseAllJewelPiecesCollected;

u32 PauseScreenSubroutine(void);
void SetPauseScreenVBlankCallback(void);
void PauseScreenVBlank(void);
void InitializePauseScreen(void);
u32 UpdatePauseScreenSelection(void);
u32 UpdatePauseScreenConfirmation(void);
u32 OpenPauseScreenWindow(void);
u32 ClosePauseScreenWindow(void);
void RenderPauseScreenOam(void);

#endif

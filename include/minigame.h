#ifndef MINIGAME_H
#define MINIGAME_H

#include "types.h"

u32 MinigameSubroutine(void);
s32 MinigameRandom(void);
s32 MinigameWaitForFrames(s32 frames);
void SetMinigameVBlankCallback(void);
void MinigameVBlankCallback(void);
void MinigameFadeVBlankCallback(void);
void DrawMinigameHighScore(u32 value, const u16* tiles, u32 destination);
void DrawActiveMinigame(void);
s32 ExpandMinigameWindow(void);
s32 ContractMinigameWindow(void);
u32 PrepareMinigameSave(void);
s32 UpdateMinigameMenu(void);
void InitMinigameMenu(void);
void InitMinigameMenuSelection(void);
void InitMinigameResultsMenu(void);
void ApplyMinigameMenuVBlank(void);
s32 HandleMinigameMenuInput(void);
s32 UpdateMinigameStartSequence(void);
s32 UpdateMinigameResultSequence(void);
void DrawMinigameMenuSprites(void);

#endif  // MINIGAME_H

#ifndef PASSAGE_SCREEN_H
#define PASSAGE_SCREEN_H

#include "gba.h"
#include "oam.h"

struct AGB_ALIGN PassageIconState {
    u16 timer;
    u16 frame;
    u8 active;
    u8 padding[3];
};

s32 PassageScreenSubroutine(void);
void InitializePassageScreen(void);
void BeginPassageStageCursorArrival(void);
void BeginPassageStageSwitch(void);
void InitializePassageStageEntry(void);
void BeginPassageScreenWindowClose(void);
void UpdatePassageStageCursorMovement(void);
void UpdatePassageStageCursorSound(void);
s32 HandlePassageScreenInput(void);
void BeginPassageScreenExit(void);
void BeginPassageBossStageEntry(void);
void LoadPassageStageDisplayGraphics(void);
void DrawPassageStageScore(void);
s32 UpdatePassageStageCursorArrival(void);
s32 UpdatePassageStageSwitch(void);
s32 UpdatePassageScreenExitMovement(void);
void DrawPassageScreenSprites(void);
s32 ClosePassageScreenWindow(void);
s32 OpenPassageScreenWindow(void);
void PassageScreenVBlank(void);

#endif /* PASSAGE_SCREEN_H */

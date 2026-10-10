#ifndef STAGE_SELECT_H
#define STAGE_SELECT_H

#include "types.h"

u32 StageSelectSubroutine(void);
s32 UpdateStageSelectFadeIn(void);
s32 UpdateStageSelectFadeOut(void);
void InitMinigameScoreDisplay(void);
void InitializeStageSelectPaletteFade(void);
void InitializeStageSelectBrightnessFade(void);
void InitializeStageSelectFullBrightnessFade(void);
void SetStageSelectVBlankCallback(void);
void StageSelectVBlank(void);
void StageSelectFadeVBlank(void);
s32 LoadSelectedStageData(void);
void UploadTallFiveDigitNumberTiles(s32 value, const void* tiles, u32 destination);
void UploadFiveDigitNumberTiles(s32 value, const void* tiles, u32 destination);
void DrawMinigameNumber(s32 value, const u16* tiles, u32 destination);
void EnableAllLayersInWindows(void);
void DrawStageSelectSprites(void);

#endif  // STAGE_SELECT_H

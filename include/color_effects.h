#ifndef COLOR_EFFECTS_H
#define COLOR_EFFECTS_H

#include "types.h"

struct AGB_ALIGN ColorFading {
    u8 type;
    u8 timer;
    u8 progress;
    u8 sourceFlags;
    u8 uploadFlags;
    u8 interpolationPending;
};

extern struct ColorFading gColorFading;

void BackupPalettesForColorFade(void);
void BackupPalettesToSecondaryBuffers(void);
void BackupPalettesToPrimaryBuffers(void);
void CopySecondaryPalettesToPrimaryBuffers(void);
void CopyAlternatePalettesToPrimaryBuffers(void);
void CopyAlternateBgAndSecondaryObjPalettesToPrimaryBuffers(void);
void BuildGrayscaleBgPaletteBuffer(void);
void UploadColorFadePalettes(void);
void ColorFadingProcess(void);
void UploadBossColorFadePalettes(void);
void ApplyPaletteFade(u32 mode, u32 amount);
u32 UpdatePaletteInterpolation(u32 paletteSet, u32 speed);
void ConvertObjPaletteBankToGrayscale(u8 paletteIndex);
void LoadBossSpriteGraphics(u32 type, u32 paletteIndex, u32 colorCount);
void StartGoldenDivaPaletteFade(u32 type, u32 paletteIndex, u32 colorCount);
void UpdateBossDefeatPaletteFade(void);
void UpdateGoldenDivaPaletteFade(void);
void ApplyTimedPaletteFade(s16* destination, u16* source, u8* state);
s32 BlendColorFromWhite(u8 red, u8 green, u8 blue, u8 amount);
s32 BlendColorToWhite(u32 red, u32 green, u32 blue, u32 amount);
s32 BlendColorFromBlack(u32 red, u32 green, u32 blue, u32 amount);
s32 BlendColorToBlack(u32 red, u32 green, u32 blue, u32 amount);
void UpdateEscapePaletteFadeSequence(void);
void BuildRoomTintPalette(u32 direction);
s32 UpdatePaletteFadeStep(u32 mode, u32 stepTableIndex);
void InitializeColorFadeEffect(void);
void PrepareColorFadeBuffers(void);
void FillColorFadePalettes(void);
s32 HandleSwitchColorFade(void);
s32 ExpandColorFadeWindow(void);
s32 CollapseColorFadeWindowAroundWario(void);
s32 CollapseColorFadeWindowVertically(void);

#endif  // COLOR_EFFECTS_H

#ifndef HBLANK_H
#define HBLANK_H

#include "types.h"

void InitRoomHBlank(void);
void SetHBlankMode(u32 mode);
void ClearHBlankTablesAndState(void);
void ResetHBlankStateAndCopyLineValues(void);
void HBlankCopyBg3HOffsetFromLineOffsets(void);
void HBlankCopyBg3HOffsetFromLineValues(void);
void HBlankCopyBg0HOffsetFromLineValues(void);
void HBlankApplyLineOffsetToAllBgX(void);
void HBlankApplyLineOffsetToAllBgXAlt(void);
void HBlankApplyDualBgHorizontalOffsets(void);
void HBlankApplyBg3XYLineValues(void);
void HBlankApplyLineOffsetToBg0AndBg3XY(void);
void HBlankApplyBg1ScrollFromBg3Tables(void);
void HBlankApplyPaletteLineValue(void);
void HBlankApplyBg3VerticalLineOffset(void);
void UpdateHBlankEffect(void);
void UpdateWaterRippleHBlank(void);
void UpdateWaterSplitHBlank(u8 mode);
void UpdateEscapeRippleHBlank(u8 mode);
void UpdateGrowingEscapeWaveHBlank(void);
void UpdateBg0RippleHBlank(void);
void UpdateDualRippleHBlank(void);
void UpdateSineWaveHBlank(void);
void UpdatePaletteFadeHBlankTable(void);
void UpdateGoldenBossTransitionHBlank(void);

#endif  // HBLANK_H

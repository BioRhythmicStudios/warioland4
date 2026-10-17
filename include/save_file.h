#ifndef SAVE_FILE_H
#define SAVE_FILE_H

#include "global_data.h"

extern GBA_PTR(u8*) const sPrimarySaveHeaderBuffers[SAVE_COUNT];
extern GBA_PTR(u8*) const sSecondarySaveHeaderBuffers[SAVE_COUNT];
extern GBA_PTR(u8*) const sMainSaveDataBuffers[SAVE_COUNT];
extern GBA_PTR(u8*) const sStageSelectionSaveBuffers[SAVE_COUNT];

void BuildTitleScreenSaveHeader(void);
void ValidateAndRepairSaveHeaderCopies(void);
u32 ValidateSaveHeaderBlock(u8 saveType);
void InitializeTemporarySaveBuffer(void);
void SerializeGameStateToTemporarySave(void);
void RefreshTemporarySaveStageId(void);
void RestoreGameStateFromTemporarySave(void);
void BuildMainSaveWorkingBuffer(void);
void SerializeGlobalSaveDataToWorkingBuffer(void);
void RestoreGlobalSaveDataFromWorkingBuffer(void);
void CopySaveFileBlock16(u8* destination, const u8* source);
void CopySaveFileBlock8(u8* destination, const u8* source);
void RunBackupSramSelfTest(void);
void DmaLoadSelectedSaveFileBlocks(void);
void InitializeSaveFileStatuses(void);
u32 ClassifyAndRepairSaveSlotCopy(u8 selectedFile);
u32 ValidateAndClearSaveSlotCopy(u8 copy, u8 selectedFile);
u32 ValidateMainSaveSlot(u8 selectedFile, u32 statusValue);
void FlushSaveFileAndMaybeRestoreTempSave(void);
void FlushSaveFileAndRefreshSaveData(void);
void BuildSelectedSaveValidationHeader(void);
u32 ValidateSelectedSaveHeader(void);
void BuildTemporaryStageSelectionSave(void);
u32 ValidateStageSelectionSaveSlot(u8 selectedFile);
void RestoreTempSaveStageSelection(void);

#endif  // SAVE_FILE_H

#include "init_helpers.h"
#include "input.h"
#include "interrupt_callbacks.h"
#include "main.h"
#include "save_file.h"

void SoftResetSubroutine(void)
{
    m4aSoundVSyncOff();
    m4aMPlayAllStop();

    REG_IME = FALSE;
    REG_DISPSTAT &= ~DISPSTAT_HBLANK_INTR;
    REG_IE &= ~INTR_FLAG_HBLANK;
    REG_IME = TRUE;
    REG_BLDCNT = 0xFF;
    REG_DISPCNT = 0;
    REG_BLDY = 0x10;
    REG_IME = FALSE;

    if (!gUnk_3000014) {
        DmaFill32(3, 0, EWRAM_START, EWRAM_SIZE);
    } else {
        DmaFill32(3, 0, EWRAM_START, EWRAM_SIZE - 0x8000);
    }
    DmaFill32(3, 0, IWRAM_START, IWRAM_SIZE - 0x200);
    InitializeInterruptHandler();
    InterruptCallbackSetVBlank(EmptyFunction);

    m4aSoundInit();
    m4aSoundMode(SOUND_MODE_DA_BIT_8);
    REG_IME = TRUE;
    m4aSoundVSyncOff();

    ValidateAndRepairSaveHeaderCopies();
    InitializeSaveFileStatuses();

    gMainGameMode = GM_CUTSCENE;
    gSubGameMode = 0;
    gButtonsHeld = 0;
    gButtonsHeldCopy = 0;
    gButtonsPressed = 0;

    m4aSoundVSyncOn();
}

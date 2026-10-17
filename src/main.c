#include "main.h"

#include "init_helpers.h"
#include "input.h"
#include "interrupt_callbacks.h"
#include "interrupts.h"
#include "save_file.h"
#include "sram_misc.h"

#include "credits.h"
#include "cutscenes.h"
#include "demo.h"
#include "file_select.h"
#include "game_screen.h"
#include "minigame.h"
#include "pause.h"
#include "quit.h"
#include "save_reset.h"
#include "shop.h"
#include "soft_reset.h"
#include "stage_select.h"

void irq_handler(void);  // test

#ifndef PORTABLE
IWRAM_DATA u8 gGoldenTreasureCount = 0;
IWRAM_DATA s8 gPauseFlag = 0;
IWRAM_DATA u8 gUnk_3000C36 = 0;
IWRAM_DATA s8 gUnk_3000C37 = 0;
IWRAM_DATA u8 gCurrentShopItem = 0;
IWRAM_DATA s16 gMainGameMode = 0;
IWRAM_DATA s16 gSubGameMode = 0;
IWRAM_DATA s8 gStageEntrySoftResetDisabled = 0;
IWRAM_DATA s8 gStageEntryPauseMenuDisabled = 0;
IWRAM_DATA s8 gUnk_3000C40 = 0;
IWRAM_DATA u8 gMainTimer = 0;
IWRAM_DATA vu16 gInterruptCheck = 0;
IWRAM_DATA u16 gInterruptHandlerBuffer[] = {};
#else
extern u8 gGoldenTreasureCount;
extern s8 gPauseFlag;
extern u8 gUnk_3000C36;
extern s8 gUnk_3000C37;
extern u8 gCurrentShopItem;
extern s16 gMainGameMode;
extern s16 gSubGameMode;
extern s8 gStageEntrySoftResetDisabled;
extern s8 gStageEntryPauseMenuDisabled;
extern s8 gUnk_3000C40;
extern u8 gMainTimer;
extern vu16 gInterruptCheck;
extern u16 gInterruptHandlerBuffer[];
#endif

void AgbMain(void)
{
    u32 cutsceneResult;

    InitializeGame();

    while (TRUE) {
        gUnk_3000C40 = 0;
        if (gMainGameMode != GM_CUTSCENE) {
            m4aSoundMain();
        }
        if (gResetSaveFile) {
            m4aSoundVSyncOff();
            if (gResetSaveFile == 2) {
                ClearSRAM();
            }
            break;
        }

        PollInput();
        CheckSoftReset();
        TIMER_COUNT_UP(gMainTimer);
        gRandomSeed += 1;

        switch (gMainGameMode) {
            case GM_CUTSCENE:
                cutsceneResult = UpdateCutsceneSequence();
                if (cutsceneResult == 1) {
                    gMainGameMode = GM_FILE_SELECT;
                    gSubGameMode = 0;
                } else if (cutsceneResult == 2) {
                    gMainGameMode = GM_STAGE_SELECT;
                    gSubGameMode = 5;
                } else if (cutsceneResult == 3) {
                    gMainGameMode = GM_CREDITS;
                    gSubGameMode = 0;
                } else if (cutsceneResult == 4) {
                    gMainGameMode = GM_CUTSCENE;
                    gSubGameMode = -1;
                    break;
                } else {
                    if (!(gUnk_3000020 & 0x80)) {
                        break;
                    }
                    gMainGameMode = GM_DEMO;
                    gSubGameMode = 0;
                }
                break;

            case GM_FILE_SELECT:
                if (FileSelectSubroutine()) {
                    if (gStageEntrySoftResetDisabled == 1) {
                        if (gHasTemporarySave) {
                            gSubGameMode = 0;
                            gMainGameMode = GM_GAME_SCREEN;
                        } else {
                            gMainGameMode = GM_STAGE_SELECT;
                            if (!gSaveFlag) {
                                gSubGameMode = -2;
                                gMainGameMode = GM_CUTSCENE;
                            } else if (gCurrentStageNumber == STAGE_MAX) {
                                gSubGameMode = 0;
                            } else {
                                gSubGameMode = 0x26;
                            }
                        }
                    } else {
                        gSubGameMode = 0;
                        gMainGameMode = GM_CUTSCENE;
                    }
                    gStageEntrySoftResetDisabled = 0;
                }
                gStageEntryPauseMenuDisabled = 0;
                break;

            case GM_STAGE_SELECT:
                if (StageSelectSubroutine()) {
                    gSubGameMode = 0;
                    gMainGameMode = GM_GAME_SCREEN;
                    if (gCurrentStageID == 6 || gCurrentStageID == 12 || gCurrentStageID == 18 ||
                        gCurrentStageID == 24) {
                        gMainGameMode = GM_MINIGAMES;
                    }
                }
                break;

            case GM_GAME_SCREEN:
                if (GameScreenSubroutine()) {
                    if (gPauseFlag) {
                        gMainGameMode = GM_PAUSE;
                    } else if (gUnk_3000C37) {
                        gMainGameMode = GM_ITEM_SHOP;
                    } else if (gUnk_3000022) {
                        gMainGameMode = GM_11;
                    } else {
                        gMainGameMode = GM_STAGE_SELECT;
                        switch (gStageExitType) {
                            case 0:
                            case 1:
                                gSubGameMode = 0x15;
                                break;
                            case 6:
                                gSubGameMode = 0x26;
                                break;
                            case 2:
                                gSubGameMode = 0x14;
                                break;
                            case 3:
                            case 4:
                                gSubGameMode = 0xE;
                                break;
                            case 5:
                                if (gCurrentPassageTemp == PASSAGE_ENTRY) {
                                    gSubGameMode = 0x1D;
                                } else if (gCurrentPassageTemp == PASSAGE_GOLDEN) {
                                    gMainGameMode = GM_CUTSCENE;
                                    gSubGameMode = -3;
                                } else {
                                    gSubGameMode = 0x19;
                                }
                                break;
                        }
                    }
                }
                break;

            case GM_PAUSE:
                if (PauseScreenSubroutine()) {
                    switch (gPauseFlag) {
                        case 1:
                            gMainGameMode = GM_GAME_SCREEN;
                            break;
                        case 2:
                            if (gUnk_3000022) {
                                gMainGameMode = GM_11;
                            } else {
                                gUnk_3000025 = 0;
                                gMainGameMode = GM_STAGE_SELECT;
                                gSubGameMode = 0x15;
                            }
                            break;
                        case 3:
                            gPauseFlag = 0;
                            gMainGameMode = GM_SOFT_RESET;
                            break;
                    }
                }
                break;

            case GM_QUIT:
                QuitSubroutine();
                gMainGameMode = GM_PAUSE;
                break;

            case GM_SOFT_RESET:
                SoftResetSubroutine();
                break;

            case GM_MINIGAMES:
                if (MinigameSubroutine()) {
                    if (gUnk_3000022) {
                        gMainGameMode = GM_11;
                        gSubGameMode = 0;
                    } else {
                        gMainGameMode = GM_STAGE_SELECT;
                        gSubGameMode = 0x27;
                    }
                }
                break;

            case GM_ITEM_SHOP:
                if (ItemShopSubroutine()) {
                    gMainGameMode = GM_GAME_SCREEN;
                }
                break;

            case GM_DEMO:
                if (GameScreenSubroutine() && gPauseFlag) {
                    gSubGameMode = AdvanceDemo() ? 0 : -1;
                    gMainGameMode = GM_CUTSCENE;
                }
                break;

            case GM_SAVE_RESET:
                if (SaveResetSubroutine()) {
                    gSubGameMode = 0;
                    if (gStageEntrySoftResetDisabled == 1) {
                        gResetSaveFile = 2;
                    } else {
                        gMainGameMode = GM_CUTSCENE;
                    }
                }
                break;

            case GM_CREDITS:
                if (UpdateCreditsScreen()) {
                    gMainGameMode = GM_CUTSCENE;
                    gSubGameMode = -4;
                }
                break;
        }
        if (gUnk_3000C40 && gMainGameMode != GM_CUTSCENE) {
            m4aSoundMain();
        }

        gInterruptCheck &= ~1;
        do {
            SYSCALL(SYSCALL_Halt);
        } while (!(gInterruptCheck & 1));
    }
}

void InitializeGame(void)
{
    REG_DISPCNT = DISPCNT_FORCED_BLANK;
    REG_DISPSTAT = 0;
    REG_IME = FALSE;

    DmaFill32(3, 0, EWRAM_START, EWRAM_SIZE);
    DmaFill32(3, 0, IWRAM_START, IWRAM_SIZE - 0x200);

    InitializeVideoMemory();
    InitializeInterruptHandler();
    InterruptCallbackSetVBlank(EmptyFunction);

    ValidateAndRepairSaveHeaderCopies();
    InitializeSaveFileStatuses();
    m4aSoundInit();
    m4aSoundMode(SOUND_MODE_DA_BIT_8);

    REG_IME = TRUE;
    REG_IE = INTR_FLAG_GAMEPAK | INTR_FLAG_VBLANK;
    REG_DISPSTAT = DISPSTAT_VBLANK_INTR;
    REG_WAITCNT = WAITCNT_SRAM_4 | WAITCNT_WS0_N_3 | WAITCNT_WS0_S_1 | WAITCNT_WS1_N_3 | WAITCNT_WS1_S_1 |
                  WAITCNT_WS2_N_3 | WAITCNT_WS2_S_1 | WAITCNT_PHI_OUT_NONE | WAITCNT_PREFETCH_ENABLE | WAITCNT_AGB;

    gButtonsHeld = 0;
    gButtonsHeldCopy = 0;
    gButtonsPressed = 0;
    gSubGameMode = 0;
    PollInput();
    if (gButtonsPressed == (L_BUTTON | R_BUTTON)) {
        gMainGameMode = GM_SAVE_RESET;
    } else {
        gMainGameMode = GM_CUTSCENE;
    }
    gButtonsHeld = 0;
    gButtonsHeldCopy = 0;
    gButtonsPressed = 0;

    gDisableSoftReset = 0;
};

void EmptyFunction() {}

void CheckSoftReset(void)
{
    if (gMainGameMode == GM_SOFT_RESET || gDisableSoftReset) {
        return;
    }

    if (CHECK_KEYS_ALL(gButtonsHeld, A_BUTTON | B_BUTTON | START_BUTTON | SELECT_BUTTON)) {
        gMainGameMode = GM_SOFT_RESET;
    }
}

void FadeOutAllSound(void)
{
    u8 i;

    for (i = 0; i < NUM_MUSIC_PLAYERS; i++) {
        m4aMPlayFadeOut(GP(gMPlayTable[i].info), 10);
    }
}

void ClearSRAM(void)
{
    func_8000CE0();
}

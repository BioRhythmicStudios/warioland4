#include "game_screen.h"
#include "game_screen_gfx.h"
#include "game_screen_helpers.h"

#include "background_registers.h"
#include "bg_clip.h"
#include "boss_pause.h"
#include "camera.h"
#include "color_effects.h"
#include "demo_input.h"
#include "door.h"
#include "hud.h"
#include "init_helpers.h"
#include "input.h"
#include "interrupt_callbacks.h"
#include "main.h"
#include "save_file.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "stage_end.h"
#include "wario.h"
#include "wario_effects.h"
#include "wario_palette.h"

#ifdef PORTABLE
#include <string.h>
#endif

u32 GameScreenSubroutine(void)
{
    u32 result;

    GameScreenSetVBlank();
    result = 0;
    switch (gSubGameMode) {
        case 0:
            if (!gStageEntryPauseMenuDisabled) {
                DemoInputInit();
            }
            if (gDemoState == DEMO_STATE_PLAYBACK) {
                DemoInputReadButtons();
            }
            GameScreenInitAndLoadGenerics();
            gSubGameMode++;
            break;

        case 1:
            func_801C1B4();
            if (GameScreenFadeIn()) {
                gSubGameMode++;
                break;
            }
            break;

        case 2:
            DemoInputSubroutine();
            func_801C1B4();

            if (CHECK_KEYS_ALL(gButtonsPressed, START_BUTTON) && (gWarioPauseTimer == 0) && (gWarioData.unk_02 == 0)) {
                if ((gUnk_300001B != 0) && (gDemoState == 0)) {
                    if ((gTimerState == 1) || ((gCurrentRoom | gSwitchStates[SWITCH_RED]) == 0)) {
                        gSubGameMode = 7;
                        gUnk_300001A = 1;
                    }
                } else {
                    gSubGameMode++;
                    gPauseFlag = TRUE;
                    FadeOutMusicForPause();
                }

                if (gSubGameMode != 2 && gDemoState != DEMO_STATE_PLAYBACK) {
                    m4aSongNumStart(SE_PAUSE);
                }
            }

            if (gSubGameMode == 2) {
                if (!CHECK_KEYS_ALL(gButtonsHeld, DPAD_UP)) {
                    gWarioCanEnterDoor = 1;
                }
                if (gWarioPauseTimer != 0) {
                    gWarioPauseTimer -= 1;
                } else {
                    WarioProcessControls();
                    WarioProcessCollision();
                }
            }

            ProcessRoomBackgrounds();
            break;

        case 3:
            func_801C1B4();
            if (GameScreenFadeOut()) {
                gSubGameMode = 0;
                if (gPauseFlag || gUnk_3000C37 || !gUnk_3000025) {
                    result = 1;
                }
            }
            break;

        case 4:
            WarioProcessControls();
            if (gSubGameMode == 2) {
                CheckRoomTransitionAtPosition(gWarioSubgameYPosition, gWarioSubgameXPosition);
            }
            break;

        case 6:
            result = UpdateStageExitSequence();
            if (result) {
                if (result == 1) {
                    gSubGameMode = 0;
                    if ((gStageExitType != 5) || (gUnk_300001B != 3)) {
                        m4aMPlayAllStop();
                    }
                    result = 1;
                } else if (result == 2) {
                    gSubGameMode = 0;
                    result = 0;
                }
            }

            if (gStageExitType == 5) {
                gButtonsHeld = 0;
                gButtonsPressed = 0;
                WarioProcessControls();
                ProcessWarioInteraction();
            }
            break;

        case 7:
            if (BossPause()) {
                gSubGameMode = 2;
            }
            break;

        case 8:
            GameScreenDebugSubroutine();
            ProcessRoomBackgrounds();
            break;
    }

    if (gSubGameMode != 0) {
        UpdateRoomAnimatedGraphics();
        UpdatePrimarySprites();
        if (!gDisableWario) {
            UpdateCamera();
            ProcessWarioInteraction();
        }
        UpdateWarioLifeHud();
        DrawGameplayHudOam();
        UpdateSecondarySprites();
        if (!gDisableWario) {
            GameScreenDrawWario();
        }
        DrawPrimarySprites();
        ResetFreeOam();
        DrawGameScreen();
    }

    FinalizeWarioUpdate();
    ProcessRoomMusic();

    return result;
}

void GameScreenSetVBlank(void)
{
    switch (gSubGameMode) {
        case 0:
        case 1:
        case 3:
            InterruptCallbackSetVBlank(func_801BC0C);
            break;

        case 6:
            InterruptCallbackSetVBlank(func_801C050);
            break;

        case 2:
        default:
            InterruptCallbackSetVBlank(func_801BEA8);
    }
}

void func_801BC0C(void)
{
    m4aSoundVSync();

    DmaCopy32(3, gOamBuffer, OAM, sizeof(gOamBuffer));
    if (!gDisableWario) {
        if (gWarioData.objData1Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData1), 0x6010000, gWarioData.objData1Size);
        }
        if (gWarioData.objData2Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData2), 0x6010400, gWarioData.objData2Size);
        }
    }
    DmaCopy16(3, GP(gCurrentHeartMeterGfx), 0x6011200, 0x100);
    DmaCopy16(3, GP(gCurrentHeartGaugeGfx), 0x6011600, 0x100);

    if (gBldCnt != 0) {
        REG_BLDCNT = gBldCnt;
        gBldCnt = 0;
    }
    if (gColorFading.type == 5) {
        REG_WIN1H = (gWindow.left << 8) | gWindow.right;
        REG_WIN1V = (gWindow.top << 8) | gWindow.bottom;
    }
    REG_BLDY = gBldy;
    REG_BG0HOFS = gBackgroundScroll.bg0Horizontal;
    REG_BG0VOFS = gBackgroundScroll.bg0Vertical;
    REG_BG1HOFS = gBackgroundScroll.bg1Horizontal;
    REG_BG1VOFS = gBackgroundScroll.bg1Vertical;
    REG_BG2HOFS = gBackgroundScroll.bg2Horizontal;
    REG_BG2VOFS = gBackgroundScroll.bg2Vertical;
    REG_BG3HOFS = gBackgroundScroll.bg3Horizontal;
    REG_BG3VOFS = gBackgroundScroll.bg3Vertical;
}

void func_801BD4C(void)
{
    DmaCopy32(3, gOamBuffer, OAM, sizeof(gOamBuffer));
    if (!gDisableWario) {
        if (gWarioData.objData1Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData1), 0x6010000, gWarioData.objData1Size);
        }
        if (gWarioData.objData2Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData2), 0x6010400, gWarioData.objData2Size);
        }
        DmaCopy16(3, gWarioPalette, OBJ_PLTT, gWarioPaletteSize);
    }
    DmaCopy16(3, GP(gCurrentHeartMeterGfx), 0x6011200, 0x100);
    DmaCopy16(3, GP(gCurrentHeartGaugeGfx), 0x6011600, 0x100);

    if (gBldCnt != 0) {
        REG_BLDCNT = gBldCnt;
        gBldCnt = 0;
    }
    if (gColorFading.type == 5) {
        REG_WIN1H = (gWindow.left << 8) | gWindow.right;
        REG_WIN1V = (gWindow.top << 8) | gWindow.bottom;
    }
    REG_BLDY = gBldy;
    REG_BG0HOFS = gBackgroundScroll.bg0Horizontal;
    REG_BG0VOFS = gBackgroundScroll.bg0Vertical;
    REG_BG1HOFS = gBackgroundScroll.bg1Horizontal;
    REG_BG1VOFS = gBackgroundScroll.bg1Vertical;
    REG_BG2HOFS = gBackgroundScroll.bg2Horizontal;
    REG_BG2VOFS = gBackgroundScroll.bg2Vertical;
    REG_BG3HOFS = gBackgroundScroll.bg3Horizontal;
    REG_BG3VOFS = gBackgroundScroll.bg3Vertical;
}

void func_801BEA8(void)
{
    s32 effect;

    m4aSoundVSync();

    DmaCopy32(3, gOamBuffer, OAM, sizeof(gOamBuffer));
    if (!gDisableWario) {
        if (gWarioData.objData1Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData1), 0x6010000, gWarioData.objData1Size);
        }
        if (gWarioData.objData2Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData2), 0x6010400, gWarioData.objData2Size);
        }
        if (gCurrentWarioEffect.type != 0) {
            effect = gCurrentWarioEffect.type - 1;
            DmaCopy16(3, GP(sWarioEffectsGfx[effect].pGfx1), 0x6010800, (s32)sWarioEffectsGfx[effect].gfx1Size);
            DmaCopy16(3, GP(sWarioEffectsGfx[effect].pGfx2), 0x6010C00, (s32)sWarioEffectsGfx[effect].gfx2Size);
        }
        DmaCopy16(3, gWarioPalette, OBJ_PLTT, gWarioPaletteSize);
    }
    DmaCopy16(3, GP(gCurrentHeartMeterGfx), 0x6011200, 0x100);
    DmaCopy16(3, GP(gCurrentHeartGaugeGfx), 0x6011600, 0x100);

    if (gBldAlpha != 0) {
        REG_BLDALPHA = gBldAlpha;
        gBldAlpha = 0;
    }
    REG_MOSAIC = (gMosaic << 4) | gMosaic;
    REG_BG0HOFS = gBackgroundScroll.bg0Horizontal;
    REG_BG0VOFS = gBackgroundScroll.bg0Vertical;
    REG_BG1HOFS = gBackgroundScroll.bg1Horizontal;
    REG_BG1VOFS = gBackgroundScroll.bg1Vertical;
    REG_BG2HOFS = gBackgroundScroll.bg2Horizontal;
    REG_BG2VOFS = gBackgroundScroll.bg2Vertical;
    REG_BG3HOFS = gBackgroundScroll.bg3Horizontal;
    REG_BG3VOFS = gBackgroundScroll.bg3Vertical;
}

void func_801C040(void)
{
    m4aSoundVSync();
    m4aSoundMain();
}

void func_801C050(void)
{
    m4aSoundVSync();

    DmaCopy32(3, gOamBuffer, OAM, sizeof(gOamBuffer));
    if (!gDisableWario) {
        if (gWarioData.objData1Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData1), 0x6010000, gWarioData.objData1Size);
        }
        if (gWarioData.objData2Size > 0) {
            DmaCopy16(3, GP(gWarioData.pObjData2), 0x6010400, gWarioData.objData2Size);
        }
    }

    if (gColorFading.type == 5) {
        REG_WIN1H = (gWindow.left << 8) | gWindow.right;
        REG_WIN1V = (gWindow.top << 8) | gWindow.bottom;
    }
    if (gStageExitType == 0x80) {
        REG_BG2PA = gBg2PA;
        REG_BG2PB = gBg2PB;
        REG_BG2PC = gBg2PC;
        REG_BG2PD = gBg2PD;
        REG_BG2X_L = gBg2X;
        REG_BG2X_H = (gBg2X & 0x0FFF0000) >> 16;
        REG_BG2Y_L = gBg2Y;
        REG_BG2Y_H = (gBg2Y & 0x0FFF0000) >> 16;
        return;
    }

    REG_BLDY = gBldy;
    REG_BG0HOFS = gBackgroundScroll.bg0Horizontal;
    REG_BG0VOFS = gBackgroundScroll.bg0Vertical;
    REG_BG1HOFS = gBackgroundScroll.bg1Horizontal;
    REG_BG1VOFS = gBackgroundScroll.bg1Vertical;
    REG_BG2HOFS = gBackgroundScroll.bg2Horizontal;
    REG_BG2VOFS = gBackgroundScroll.bg2Vertical;
    REG_BG3HOFS = gBackgroundScroll.bg3Horizontal;
    REG_BG3VOFS = gBackgroundScroll.bg3Vertical;
}

void func_801C1B4(void)
{
    ColorFadingProcess();
}

void GameScreenInitAndLoadGenerics(void)
{
    s16 bldcnt;
    u8 dispcnt;

    REG_IME = FALSE;
    REG_DISPSTAT &= ~DISPSTAT_HBLANK_INTR;
    REG_IE &= ~INTR_FLAG_HBLANK;
    REG_IME = TRUE;
    InterruptCallbackSetVBlank(func_801C040);

    REG_BLDCNT = 0xFF;
    REG_BLDY = 0x10;
    gBldy = 0x10;
    REG_DISPCNT = 0;

    gUnk_3000022 = 0;
    if (!gStageEntryPauseMenuDisabled) {
        InitializeVideoMemory();
        RefreshTemporarySaveStageId();
        DmaLoadSelectedSaveFileBlocks();
    }

    DmaCopy16(3, sUnk_82DF094, 0x06010B00, 0x100);
    LoadHudSpriteGraphics();
    DmaCopy16(3, sWarioDefaultObjPalette, OBJ_PLTT, 0x20);
    DmaCopy16(3, sWarioGroundPoundPalette, OBJ_PLTT + 0x40, 0x20);
    DmaCopy16(3, sCommonSpritesPal, OBJ_PLTT + 0x80, 0x80) GameScreenInitWario();
    do {
    } while ((u16)(REG_VCOUNT - 0x15) < 0x8C);

    LoadRoom();
    do {
    } while ((u16)(REG_VCOUNT - 0x15) < 0x8C);

    if ((gPauseFlag == 0) && (gStageEntryPauseMenuDisabled != 0)) {
        WarioProcessControls();
        WarioProcessCollision();
    }
    ProcessWarioInteraction();
    func_801BD4C();
    do {
    } while ((u16)(REG_VCOUNT - 0x15) < 0x8C);

    InitStageHudAndScore();
    InitializePrimarySprites();
    InitializeColorFadeEffect();
    InitializeRoomMusic();
    if (gStageEntryPauseMenuDisabled == 0) {
        UpdatePrimarySprites();
        UpdateWarioLifeHud();
        gStageEntryPauseMenuDisabled = 1;
        gWarioPauseTimer = 0;
        gDisableWario = 0;
        if (!gHasTemporarySave && !gUnk_3000025) {
            gTimerState = 0;
            if (!(gCurrentPassage == 0 && gCurrentStageNumber == 2) && gCurrentStageNumber != 4) {
                gWarioPauseTimer = CONVERT_SECONDS(16 + 2 / 3.0);
                gDisableWario = 1;
            }
        }
    }

    gBldy = 0xF;
    if (gCurrentRoomHeader.bgPriorityAlpha < 8) {
        bldcnt = 0x3F40;
    } else {
        bldcnt = 0x3E41;
    }
    do {
    } while ((u16)(REG_VCOUNT - 0x15) < 0x8C);

    gHasTemporarySave = 0;
    gPauseFlag = 0;

    if (gHBlankMode) {
        REG_IE |= INTR_FLAG_HBLANK;
        REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;
    } else {
        REG_IME = FALSE;
        REG_DISPSTAT &= ~DISPSTAT_HBLANK_INTR;
        REG_IE &= ~INTR_FLAG_HBLANK;
        REG_IME = TRUE;
    }
    REG_BLDCNT = bldcnt;
    dispcnt = (DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN1_ON) >> 8;
    REG_DISPCNT = dispcnt << 8;
    InterruptCallbackSetVBlank(func_801BC0C);
}

void GameScreenInitWario(void)
{
    if (!gPauseFlag) {
        gWarioAfterimageHistoryIndex = 0;
        gWarioAfterimageHistoryWrapped = 0;
        gWarioWaterCollisionActive = 0;
        if (!gHasTemporarySave) {
            gWarioDashAfterimage = sEmptyWarioAfterimage;
            gCurrentCarriedSprite = sEmptyCarriedSprite;
        }
    }
    if (gWarioData.reaction == 0 && gWarioData.pose == WPOSE_NORMAL_ENTERING_DOOR) {
        gWarioData.pose = WPOSE_NORMAL_EXITING_DOOR;
        gWarioData.unk_1F = 0;
        gWarioCanEnterDoor = 0;
    }
    if (gWarioData.unk_1A == 1) {
        gWarioData.unk_1A = 2;
    }
    if (gStageEntryPauseMenuDisabled != 0) {
        if (gWarioData.reaction != 0) {
            return;
        }
        if (gWarioData.pose == 23) {
            m4aSongNumStart(SOUND_29);
        } else if (gWarioData.pose == 28) {
            m4aSongNumStart(SOUND_23);
        } else if (gWarioData.pose == 35) {
            m4aSongNumStart(SOUND_27);
        }
        return;
    }

    if (!gHasTemporarySave) {
        memcpy(&gWarioData, &sStartingWarioData, sizeof(struct WarioData));
        gHeartMeter = sStartingHeartMeter;
        gHeartGauge = sStartingHeartGauge;
        gWarioMotionAfterimage = sEmptyWarioAfterimage;
        gCurrentWarioEffect = sStartingWarioEffect;
        gWarioDustEffect1 = sEmptyDustEffect;
        gWarioDustEffect2 = sEmptyDustEffect;
        gWarioCanEnterDoor = 0;
        if ((gCurrentPassage == PASSAGE_GOLDEN) || (gCurrentStageNumber == STAGE_BOSS)) {
            if (gDifficulty == DIFFICULTY_NORMAL) {
                gHeartMeter.current = 8;
            } else {
                gHeartMeter.current = 4;
            }
        } else if (gDifficulty == DIFFICULTY_SUPER_HARD) {
            gHeartMeter.current = 1;
        }
    }
    gWarioMusicState = 0;
}

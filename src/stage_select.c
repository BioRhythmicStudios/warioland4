#include "stage_select.h"
#include "background_registers.h"
#include "boss_treasure_cutscene.h"
#include "color_effects.h"
#include "game_screen_helpers.h"
#include "init_helpers.h"
#include "interrupt_callbacks.h"
#include "map_screen.h"
#include "minigame.h"
#include "passage_clear.h"
#include "passage_screen.h"
#include "score.h"
#include "sound_room.h"
#include "stage_ejection.h"
#include "stage_entry.h"
#include "stage_results.h"
#include "vortex_cutscene.h"

extern void UpdateBossDoorOpeningDisplay(void);
extern void UpdateBossDoorOpeningSprites(void);
extern void PrepareBossDoorOpeningResult(void);
extern void PrepareBossDoorOpeningJewelDisplay(void);
extern s32 UpdateBossDoorOpeningState(void);
extern u16 gUnk_3000000;
extern u8 gUnk_3003C34;
extern u16 gUnk_3003C58;
extern const u32 sUnk_86391C4[][6];

u32 StageSelectSubroutine(void)
{
    u32 finished;
    const struct MusicPlayer* musicPlayers;
    const struct Song* songs;

    finished = 0;
    SetStageSelectVBlankCallback();
    gOamSlotsUsed = 0;

    switch (gSubGameMode) {
        case 0:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            StartMapScreenFromStageSelect();
            gSubGameMode = 1;
            *(vu16*)0x04000200 |= 1;
            break;

        case 1:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                gSubGameMode = 2;
            }
            break;

        case 2:
            if (UpdateMapScreen() != 0) {
                gSubGameMode = 3;
                gColorFading.type = 2;
            }
            break;

        case 3:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeOut() != 0) {
                EnableAllLayersInWindows();
                if (gCurrentPassage == 6) {
                    gSubGameMode = 46;
                    m4aMPlayAllStop();
                } else {
                    gSubGameMode = 33;
                }
            }
            break;

        case 4:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            StartMapScreenAfterStageExit();
            gSubGameMode = 1;
            *(vu16*)0x04000200 |= 1;
            break;

        case 5:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            StartMapScreenIntroSequence();
            gSubGameMode = 1;
            *(vu16*)0x04000200 |= 1;
            break;

        case 6:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            StartMapScreenFinalPassageIntro();
            gSubGameMode = 1;
            *(vu16*)0x04000200 |= 1;
            break;

        case 7:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            StartMapScreenBossUnlockSequence();
            gSubGameMode = 1;
            *(vu16*)0x04000200 |= 1;
            break;

        case 8:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                musicPlayers = gMPlayTable;
                songs = gSongTable;
                m4aMPlayVolumeControl(GP(musicPlayers[*(const u16*)((const u8*)songs + 0x13FC)].info), 0xFFFF, 150);
                gSubGameMode = 9;
            }
            break;

        case 9:
            if (UpdateStageEntry() != 0 && LoadSelectedStageData() != 0) {
                gSubGameMode = 10;
            }
            break;

        case 10:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeOut() != 0) {
                EnableAllLayersInWindows();
                if (gUnk_3003C56 != 0) {
                    gSubGameMode = 45;
                } else {
                    switch (gStageEntryExitRequested) {
                        case 0:
                            gUnk_3000000 = 0;
                            m4aMPlayAllStop();
                            gSubGameMode = 15;
                            break;
                        case 1:
                            if (gCollectedKeyzer == 1) {
                                gCollectedKeyzer = 0;
                                gSubGameMode = 37;
                            } else {
                                gSubGameMode = 39;
                            }
                            break;
                    }
                }
            }
            break;

        case 11:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitializeStageEntryState();
            gSubGameMode = 8;
            *(vu16*)0x04000200 |= 1;
            break;

        case 12:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitializeStageEntryLandingState();
            gSubGameMode = 8;
            *(vu16*)0x04000200 |= 1;
            break;

        case 13:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitializeStageEntryKeyzerArrivalState();
            gSubGameMode = 8;
            *(vu16*)0x04000200 |= 1;
            break;

        case 14:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitializeStageEntryMovingLandingState();
            gSubGameMode = 8;
            *(vu16*)0x04000200 |= 1;
            break;

        case 15:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 4;
            gUnk_3003C58 = 0;
            InitializeVortexCutsceneFromPortal();
            gUnk_3003C34 = 0;
            gSubGameMode = 16;
            *(vu16*)0x04000200 |= 1;
            break;

        case 16:
            gUnk_3003C58++;
            UpdateVortexCutsceneAffine();
            UpdateVortexCutscenePaletteCycle();
            if (gUnk_3003C58 > 10) {
                UpdateVortexCutsceneMainSprite();
            }
            if (UpdateStageSelectFadeIn() != 0) {
                gSubGameMode = 17;
            }
            break;

        case 17:
            gUnk_3003C58++;
            if (UpdateVortexCutscene() != 0) {
                if (gUnk_3003C34 == 1) {
                    gSubGameMode = 18;
                } else {
                    gSubGameMode = 19;
                }
            }
            break;

        case 18:
            UpdateVortexCutsceneAffine();
            UpdateVortexCutscenePaletteCycle();
            if (UpdateStageResults() != 0) {
                gSubGameMode = 19;
            }
            break;

        case 19:
            gUnk_3003C58++;
            UpdateVortexCutsceneAffine();
            UpdateVortexCutscenePaletteCycle();
            if (UpdateStageSelectFadeOut() != 0) {
                if (gUnk_3003C34 == 0) {
                    finished = 1;
                    gSubGameMode = 0;
                } else {
                    gSubGameMode = 11;
                }
            }
            break;

        case 20:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 4;
            gUnk_3003C58 = 0;
            InitializeVortexCutsceneFromPassage();
            gUnk_3003C34 = 1;
            gSubGameMode = 16;
            *(vu16*)0x04000200 |= 1;
            break;

        case 21:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 4;
            gUnk_3003C58 = 0;
            InitializeStageEjection();
            gUnk_3003C34 = 1;
            gSubGameMode = 22;
            *(vu16*)0x04000200 |= 1;
            break;

        case 22:
            if (UpdateStageSelectFadeIn() != 0) {
                gSubGameMode = 23;
            }
            break;

        case 23:
            if (UpdateStageEjectionExitState() != 0) {
                gSubGameMode = 24;
            }
            break;

        case 24:
            if (UpdateStageSelectFadeOut() != 0) {
                gSubGameMode = 12;
            }
            break;

        case 25:
            gColorFading.type = 4;
            InitializeBossTreasureCutscene();
            gSubGameMode = 26;
            break;

        case 26:
            if (UpdateStageSelectFadeIn() != 0) {
                gSubGameMode = 27;
            }
            break;

        case 27:
            if (UpdateBossTreasureCutscene() != 0) {
                gSubGameMode = 28;
            }
            break;

        case 28:
            if (UpdateStageSelectFadeOut() != 0) {
                gSubGameMode = 29;
            }
            break;

        case 29:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitializePassageClear();
            gSubGameMode = 30;
            *(vu16*)0x04000200 |= 1;
            break;

        case 30:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                gSubGameMode = 31;
            }
            break;

        case 31:
            if (UpdatePassageClearSequence() != 0) {
                gSubGameMode = 32;
                gColorFading.type = 2;
            }
            break;

        case 32:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeOut() != 0) {
                if (gCurrentPassage == 0) {
                    gSubGameMode = 7;
                } else if (
                    gCurrentCollection[1][4].keyzer != 0 && gCurrentCollection[2][4].keyzer != 0 &&
                    gCurrentCollection[3][4].keyzer != 0 && gCurrentCollection[4][4].keyzer != 0
                ) {
                    gSubGameMode = 6;
                } else {
                    gSubGameMode = 0;
                }
            }
            break;

        case 33:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            BeginPassageStageCursorArrival();
            gSubGameMode = 34;
            *(vu16*)0x04000200 |= 1;
            break;

        case 34:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                musicPlayers = gMPlayTable;
                songs = gSongTable;
                m4aMPlayVolumeControl(GP(musicPlayers[*(const u16*)((const u8*)songs + 0x13FC)].info), 0xFFFF, 150);
                gSubGameMode = 35;
            }
            break;

        case 35:
            if (PassageScreenSubroutine() != 0 && LoadSelectedStageData() != 0) {
                gSubGameMode = 36;
            }
            break;

        case 36:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeOut() != 0) {
                EnableAllLayersInWindows();
                switch (gStageEntryExitRequested) {
                    case 0:
                        if (gStageRoomTableIndex <= 16 || gStageRoomTableIndex == 23) {
                            gSubGameMode = 13;
                        } else if (gStageRoomTableIndex <= 22) {
                            gSubGameMode = 41;
                        } else {
                            gSubGameMode = 40;
                        }
                        break;
                    case 1:
                        gSubGameMode = 4;
                        break;
                }
            }
            break;

        case 37:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            BeginPassageStageSwitch();
            gSubGameMode = 34;
            *(vu16*)0x04000200 |= 1;
            break;

        case 38:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitializePassageStageEntry();
            gSubGameMode = 34;
            *(vu16*)0x04000200 |= 1;
            break;

        case 39:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            BeginPassageScreenWindowClose();
            gSubGameMode = 34;
            *(vu16*)0x04000200 |= 1;
            break;

        case 40:
            if (MinigameWaitForFrames(10) != 0) {
                m4aMPlayAllStop();
                finished = 1;
                gSubGameMode = 0;
            }
            break;

        case 41:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            PrepareBossDoorOpeningResult();
            gSubGameMode = 42;
            *(vu16*)0x04000200 |= 1;
            break;

        case 42:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                musicPlayers = gMPlayTable;
                songs = gSongTable;
                m4aMPlayVolumeControl(GP(musicPlayers[*(const u16*)((const u8*)songs + 0x13FC)].info), 0xFFFF, 150);
                gSubGameMode = 43;
            }
            break;

        case 43:
            if (UpdateBossDoorOpeningState() != 0) {
                gSubGameMode = 44;
            }
            break;

        case 44:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeOut() != 0) {
                EnableAllLayersInWindows();
                if (gUnk_3003C56 != 0) {
                    gUnk_3003C56 = 0;
                    if (gCollectedKeyzer == 1) {
                        gCollectedKeyzer = 0;
                        gSubGameMode = 37;
                    } else {
                        gSubGameMode = 39;
                    }
                } else {
                    switch (gStageEntryExitRequested) {
                        case 0:
                            m4aMPlayAllStop();
                            finished = 1;
                            gSubGameMode = 0;
                            break;
                        case 1:
                            gSubGameMode = 39;
                            break;
                    }
                }
            }
            break;

        case 45:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            PrepareBossDoorOpeningJewelDisplay();
            gSubGameMode = 42;
            *(vu16*)0x04000200 |= 1;
            break;

        case 46:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitSoundRoomScreen();
            gSubGameMode = 47;
            *(vu16*)0x04000200 |= 1;
            break;

        case 47:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                gSubGameMode = 48;
            }
            break;

        case 48:
            if (UpdateSoundRoomTransition() != 0) {
                gSubGameMode = 49;
            }
            break;

        case 49:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeOut() != 0) {
                EnableAllLayersInWindows();
                gSubGameMode = 0;
                m4aMPlayAllStop();
            }
            break;
    }

    DrawStageSelectSprites();
    ResetFreeOam();
    return finished;
}

s32 UpdateStageSelectFadeIn(void)
{
    s32 result;
    s32 one;

    result = 0;
    one = 1;
    switch (gColorFading.type) {
        case 1:
        case 2:
            result = UpdatePaletteFadeStep(0, one);
            break;
        case 3:
        case 4:
            gUnk_300188E++;
            if ((gUnk_300188E & one) == 0) {
                if (gBldy != 0) {
                    gBldy--;
                } else {
                    result = one;
                }
            }
            break;
    }
    return result;
}

s32 UpdateStageSelectFadeOut(void)
{
    s32 result;
    s32 mask;

    result = 0;
    mask = 3;
    switch (gColorFading.type) {
        case 1:
        case 2:
            result = UpdatePaletteFadeStep(1, 1);
            break;
        case 3:
        case 4:
            gUnk_300188E++;
            if ((gUnk_300188E & mask) == 0) {
                if (gBldy <= 15) {
                    gBldy++;
                } else {
                    result = 1;
                }
            }
            break;
    }
    return result;
}

void InitMinigameScoreDisplay(void)
{
    s32 one;
    s32 fadeType;

    fadeType = gColorFading.type;
    if (fadeType == 3) {
        goto fade_three;
    }
    if (fadeType > 3) {
        goto greater_than_three;
    }
    one = 1;
    if (fadeType < one) {
        goto end;
    }
    InitializeStageSelectPaletteFade();
    goto end;

greater_than_three:
    if (fadeType == 4) {
        goto fade_four;
    }
    goto end;

fade_three:
    InitializeStageSelectBrightnessFade();
    goto end;

fade_four:
    InitializeStageSelectFullBrightnessFade();

end:
    return;
}

void InitializeStageSelectPaletteFade(void)
{
    gColorFading.uploadFlags = 3;
    FillColorFadePalettes();
}

void InitializeStageSelectBrightnessFade(void)
{
    gBldy = 15;
    *(vu16*)0x04000054 = 15;
    *(vu16*)0x04000050 = 0x3FBF;
}

void InitializeStageSelectFullBrightnessFade(void)
{
    gBldy = 15;
    *(vu16*)0x04000054 = 15;
    *(vu16*)0x04000050 = 0x3FFF;
}

void SetStageSelectVBlankCallback(void)
{
    switch (gSubGameMode) {
        case 16:
        case 19:
        case 22:
        case 24:
        case 26:
        case 28:
            InterruptCallbackSetVBlank(StageSelectFadeVBlank);
            break;
        default:
            InterruptCallbackSetVBlank(StageSelectVBlank);
            break;
    }
}

void StageSelectVBlank(void)
{
    vu32* dma;

    m4aSoundVSync();
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)gOamBuffer;
    dma[1] = 0x07000000;
    dma[2] = 0x84000100;
    dma[2];

    switch (gSubGameMode) {
        case 15:
        case 17:
        case 18:
        case 20:
            ApplyVortexCutsceneAffineRegisters();
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            UpdateStageEntryDisplayRegisters();
            break;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            UpdateMapScreenBackgroundsAndText();
            break;
        case 21:
        case 23:
            UpdateStageEjectionEffects();
            break;
        case 25:
        case 27:
            ApplyBossTreasureAffineRegisters();
            break;
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
            PassageScreenVBlank();
            break;
        case 41:
        case 42:
        case 43:
        case 44:
            UpdateBossDoorOpeningDisplay();
            break;
        case 46:
        case 47:
        case 48:
        case 49:
            SoundRoomVBlank();
            break;
    }
}

void StageSelectFadeVBlank(void)
{
    vu32* dma;

    m4aSoundVSync();
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)gOamBuffer;
    dma[1] = 0x07000000;
    dma[2] = 0x84000100;
    dma[2];
    *(vu16*)0x04000054 = gBldy;

    switch (gSubGameMode) {
        case 16:
        case 19:
            ApplyVortexCutsceneAffineRegisters();
            break;
        case 22:
        case 24:
            UpdateStageEjectionEffects();
            break;
        case 26:
        case 28:
            ApplyBossTreasureAffineRegisters();
            break;
    }
}

s32 LoadSelectedStageData(void)
{
    s32 stageId;
    s32 one;
    const u8* roomCounts;

    if (gStageEntryExitRequested == 1) {
        return 1;
    }
    gCurrentStageID = sUnk_86391C4[gCurrentPassage][gCurrentStageNumber];
    if (gCurrentStageID == 50) {
        goto no_stage;
    }
    roomCounts = sStageRoomCounts;
    stageId = gCurrentStageID;
    one = 1;
    if ((roomCounts + one)[stageId * 12] == 0) {
        goto no_stage;
    }
    gStageRoomTableIndex = sStageRoomCounts[stageId * 12];
    if (gDifficulty == 0) {
        gStageTimerDigits[0] = (sStageRoomCounts + 8)[gCurrentStageID * 12];
        gStageTimerDigits[1] = (sStageRoomCounts + 7)[gCurrentStageID * 12];
        gStageTimerDigits[2] = (sStageRoomCounts + 6)[gCurrentStageID * 12];
    } else if (gDifficulty == 1) {
        gStageTimerDigits[0] = (sStageRoomCounts + 5)[gCurrentStageID * 12];
        gStageTimerDigits[1] = (sStageRoomCounts + 4)[gCurrentStageID * 12];
        gStageTimerDigits[2] = (sStageRoomCounts + 3)[gCurrentStageID * 12];
    } else {
        gStageTimerDigits[0] = (sStageRoomCounts + 11)[gCurrentStageID * 12];
        gStageTimerDigits[1] = (sStageRoomCounts + 10)[gCurrentStageID * 12];
        gStageTimerDigits[2] = (sStageRoomCounts + 9)[gCurrentStageID * 12];
    }
    return 1;
no_stage:
    return 0;
}

void UploadTallFiveDigitNumberTiles(s32 value, const void* tiles, u32 destination)
{
    s32 tenThousands;
    s32 thousands;
    s32 hundreds;
    s32 tens;

    tenThousands = value / 10000 % 10;
    thousands = value / 1000 % 10;
    hundreds = value / 100 % 10;
    tens = value / 10 % 10;
    value = value % 10;

    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + tenThousands * 32;
        dma[1] = 0x06000000 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = tenThousands * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000400 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + thousands * 32;
        dma[1] = 0x06000020 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = thousands * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000420 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + hundreds * 32;
        dma[1] = 0x06000040 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = hundreds * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000440 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + tens * 32;
        dma[1] = 0x06000060 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = tens * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000460 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + value * 32;
        dma[1] = 0x06000080 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = value * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000480 + destination;
        dma[2] = 0x80000010;
        dma[2];
    }
}

void UploadFiveDigitNumberTiles(s32 value, const void* tiles, u32 destination)
{
    s32 tenThousands;
    s32 thousands;
    s32 hundreds;
    s32 tens;
    s32 ones;

    tenThousands = value / 10000 % 10;
    thousands = value / 1000 % 10;
    hundreds = value / 100 % 10;
    tens = value / 10 % 10;
    ones = value % 10;

    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + tenThousands * 32;
        dma[1] = 0x06000000 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + thousands * 32;
        dma[1] = 0x06000020 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + hundreds * 32;
        dma[1] = 0x06000040 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + tens * 32;
        dma[1] = 0x06000060 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + ones * 32;
        dma[1] = 0x06000080 + destination;
        dma[2] = 0x80000010;
        dma[2];
    }
}

void DrawMinigameNumber(s32 value, const u16* tiles, u32 destination)
{
    s32 hundreds;
    s32 tens;

    hundreds = value / 100 % 10;
    tens = value / 10 % 10;
    value = value % 10;

    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + hundreds * 32;
        dma[1] = 0x06000000 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = hundreds * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000400 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + tens * 32;
        dma[1] = 0x06000020 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = tens * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000420 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tiles + value * 32;
        dma[1] = 0x06000040 + destination;
        dma[2] = 0x80000010;
        dma[2];
        while ((dma[2] & 0x80000000) != 0) {
        }
    }
    {
        vu32* dma;
        u32 tileOffset;
        dma = (vu32*)0x040000D4;
        tileOffset = value * 32;
        tileOffset += 0x400;
        dma[0] = (u32)tiles + tileOffset;
        dma[1] = 0x06000440 + destination;
        dma[2] = 0x80000010;
        dma[2];
    }
}

void EnableAllLayersInWindows(void)
{
    *(vu16*)0x04000048 = 0x3F;
    *(vu16*)0x0400004A = 0x3F;
}

void DrawStageSelectSprites(void)
{
    switch (gSubGameMode) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            DrawMapScreenSprites();
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            RenderStageEntrySprites();
            break;
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            RenderVortexCutsceneOam();
            break;
        case 21:
        case 22:
        case 23:
        case 24:
            DrawStageEjection();
            break;
        case 25:
        case 26:
        case 27:
        case 28:
            DrawBossTreasureCutsceneSprites();
            break;
        case 29:
        case 30:
        case 31:
        case 32:
            RenderPassageClearOam();
            break;
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
            DrawPassageScreenSprites();
            break;
        case 41:
        case 42:
        case 43:
        case 44:
            UpdateBossDoorOpeningSprites();
            break;
        case 47:
        case 48:
        case 49:
            DrawSoundRoomSprites();
            break;
    }
}

#include "score.h"
#include "autosave.h"
#include "background_registers.h"
#include "gba/m4a.h"
#include "hud.h"
#include "main.h"
#include "screen_shake.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "stage_entry.h"
#include "wario.h"

void UpdateSecondarySprites(void)
{
    register struct SecondarySprite* spriteBase PIN("r2");
    GBA_TYPEOF(ProcedureFunc const *, &sSecondarySpriteAITable[0]) aiTable;
    u32 offset;
    s32 index;
    index = 0;
    spriteBase = gSecondarySpriteData;
    aiTable = sSecondarySpriteAITable;
    offset = index;
    do {
        struct SecondarySprite* sprite;
        register u32 byteOffset PIN("r1");
        byteOffset = offset;
        ASM_BARRIER(: : "r"(byteOffset));
        sprite = (struct SecondarySprite*)(byteOffset + ((u32)spriteBase));
        if ((sprite->status & 5) == 5) {
            gCurrentSecondarySprite = *sprite;
            GF(aiTable[gCurrentSecondarySprite.id])();
            if (gCurrentSecondarySprite.status & 1) {
                DrawCurrentSecondarySprite(index);
            }
            *sprite = gCurrentSecondarySprite;
            spriteBase = gSecondarySpriteData;
        }
        {
            u32 stride;
            stride = sizeof(struct SecondarySprite);
            offset += stride;
        }
        index++;
    } while (index <= 7);
    {
        register struct SecondarySprite* sprite PIN("r9");
        GBA_TYPEOF(ProcedureFunc const *, &sSecondarySpriteAITable[0]) secondAiTable;
        index = 0;
        {
            struct SecondarySprite* freshSpriteBase;
            register GBA_TYPEOF(ProcedureFunc const *, &sSecondarySpriteAITable[0]) freshAiTable PIN("r1");
            freshSpriteBase = gSecondarySpriteData;
            sprite = freshSpriteBase;
            ASM_BARRIER(: "=r"(freshAiTable) : "0"(sSecondarySpriteAITable) : "r8");
            secondAiTable = freshAiTable;
        }
        do {
            s32 active;
            register struct SecondarySprite* activeSprite PIN("r2");
            activeSprite = sprite;
            active = activeSprite->status & 5;
            if (active == 1) {
                struct SecondarySprite* current;
                current = &gCurrentSecondarySprite;
                *current = *sprite;
                GF(secondAiTable[current->id])();
                active &= current->status;
                if (active != 0) {
                    DrawCurrentSecondarySprite(index);
                }
                *sprite = *current;
                ASM_BARRIER(: : "r"(activeSprite), "r"(current));
            }
            {
                u32 stride;
                stride = sizeof(struct SecondarySprite);
                sprite = (struct SecondarySprite*)(((u8*)sprite) + stride);
            }
            index++;
        } while (index <= 7);
    }
    UpdateStageTimerAndScoreDrain();
    if (gStageScoreDigitsDirty != 0) {
        UploadStageScoreDigitTiles();
    }
    if (gStageTimerDigitsState != 0) {
        UploadStageTimerDigitTiles();
    }
    if (gStageTimerColonState != 0) {
        UploadStageTimerColonTiles();
    }
}

void UploadStageScoreDigitTiles(void)
{
    vu32* dma;

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)&sScoreDigitsGfx[gScoreDigits[4] * 32];
    dma[1] = 0x06011080;
    dma[2] = 0x80000010;
    dma[2];
    dma[0] = (u32)&sScoreDigitsGfx[gScoreDigits[3] * 32];
    dma[1] = 0x060110A0;
    dma[2] = 0x80000010;
    dma[2];
    dma[0] = (u32)&sScoreDigitsGfx[gScoreDigits[2] * 32];
    dma[1] = 0x060110C0;
    dma[2] = 0x80000010;
    dma[2];
    dma[0] = (u32)&sScoreDigitsGfx[gScoreDigits[1] * 32];
    dma[1] = 0x060110E0;
    dma[2] = 0x80000010;
    dma[2];
    dma[0] = (u32)&sScoreDigitsGfx[gScoreDigits[0] * 32];
    dma[1] = 0x06011100;
    dma[2] = 0x80000010;
    dma[2];
    gStageScoreDigitsDirty = 0;
}

void UploadStageTimerDigitTiles(void)
{
    vu32* dma;
    s8* digits;
    u32 dmaControl;
    u8 state;
    s32 seconds;

    state = gStageTimerDigitsState;
    if (gStageTimerDigits[2] == 0) {
        seconds = gStageTimerDigits[1] * 5;
        seconds = seconds * 2 + (u8)gStageTimerDigits[0];
        seconds = (s8)seconds;
        if (seconds == 10) {
            state = 35;
            m4aSongNumStart(0x1C2);
        } else if (seconds == 30) {
            state = 19;
        } else if (seconds <= 9) {
            state |= 32;
        } else if (seconds <= 29) {
            state |= 16;
        }
    }

    switch (state) {
        case 3:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsUpperGfx[digits[2] * 32];
            dma[1] = 0x06011000;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsLowerGfx[digits[2] * 32];
            dma[1] = 0x06011400;
            dma[2] = dmaControl;
            dma[2];
        case 2:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsUpperGfx[digits[1] * 32];
            dma[1] = 0x06011040;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsLowerGfx[digits[1] * 32];
            dma[1] = 0x06011440;
            dma[2] = dmaControl;
            dma[2];
        case 1:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsUpperGfx[digits[0] * 32];
            dma[1] = 0x06011060;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsLowerGfx[digits[0] * 32];
            dma[1] = 0x06011460;
            dma[2] = dmaControl;
            dma[2];
            break;
        case 19:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsYellowUpperGfx[digits[2] * 32];
            dma[1] = 0x06011000;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsYellowLowerGfx[digits[2] * 32];
            dma[1] = 0x06011400;
            dma[2] = dmaControl;
            dma[2];
        case 18:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsYellowUpperGfx[digits[1] * 32];
            dma[1] = 0x06011040;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsYellowLowerGfx[digits[1] * 32];
            dma[1] = 0x06011440;
            dma[2] = dmaControl;
            dma[2];
        case 17:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsYellowUpperGfx[digits[0] * 32];
            dma[1] = 0x06011060;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsYellowLowerGfx[digits[0] * 32];
            dma[1] = 0x06011460;
            dma[2] = dmaControl;
            dma[2];
            break;
        case 35:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsRedUpperGfx[digits[2] * 32];
            dma[1] = 0x06011000;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsRedLowerGfx[digits[2] * 32];
            dma[1] = 0x06011400;
            dma[2] = dmaControl;
            dma[2];
        case 34:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsRedUpperGfx[digits[1] * 32];
            dma[1] = 0x06011040;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsRedLowerGfx[digits[1] * 32];
            dma[1] = 0x06011440;
            dma[2] = dmaControl;
            dma[2];
        case 33:
            dma = (vu32*)0x040000D4;
            digits = gStageTimerDigits;
            dma[0] = (u32)&sTimeDigitsRedUpperGfx[digits[0] * 32];
            dma[1] = 0x06011060;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            dma[0] = (u32)&sTimeDigitsRedLowerGfx[digits[0] * 32];
            dma[1] = 0x06011460;
            dma[2] = dmaControl;
            dma[2];
            break;
    }
    gStageTimerDigitsState = 0;
}

void UploadStageTimerColonTiles(void)
{
    register vu32* dma PIN("r0");
    const u8* lowerGfx;
    u32 dmaControl;
    u8 state;
    s32 seconds;

    state = gStageTimerColonState;
    if (gStageTimerDigits[2] == 0) {
        seconds = gStageTimerDigits[1] * 5;
        seconds = seconds * 2 + (u8)gStageTimerDigits[0];
        seconds = (s8)seconds;
        if (seconds <= 10) {
            state |= 32;
        } else if (seconds <= 30) {
            state |= 16;
        }
    }

    switch (state) {
        case 2:
            dma = (vu32*)0x040000D4;
            dma[0] = (u32)sTimeColonUpperGfx;
            dma[1] = 0x06011020;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            lowerGfx = sTimeColonLowerGfx;
            break;
        case 1:
            dma = (vu32*)0x040000D4;
            dma[0] = (u32)sTimeColonEmptyUpperGfx;
            dma[1] = 0x06011020;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            lowerGfx = sTimeColonEmptyLowerGfx;
            break;
        case 18:
            dma = (vu32*)0x040000D4;
            dma[0] = (u32)sTimeColonYellowUpperGfx;
            dma[1] = 0x06011020;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            lowerGfx = sTimeColonYellowLowerGfx;
            break;
        case 17:
            dma = (vu32*)0x040000D4;
            dma[0] = (u32)sTimeColonEmptyYellowUpperGfx;
            dma[1] = 0x06011020;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            lowerGfx = sTimeColonEmptyYellowLowerGfx;
            break;
        case 34:
            dma = (vu32*)0x040000D4;
            dma[0] = (u32)sTimeColonRedUpperGfx;
            dma[1] = 0x06011020;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            lowerGfx = sTimeColonRedLowerGfx;
            break;
        case 33:
            dma = (vu32*)0x040000D4;
            dma[0] = (u32)sTimeColonEmptyRedUpperGfx;
            dma[1] = 0x06011020;
            dmaControl = 0x80000010;
            dma[2] = dmaControl;
            dma[2];
            lowerGfx = sTimeColonEmptyRedLowerGfx;
            break;
        default:
            gStageTimerColonState = 0;
            return;
    }

    dma[0] = (u32)lowerGfx;
    dma[1] = 0x06011420;
    dma[2] = dmaControl;
    dma[2];
    gStageTimerColonState = 0;
}

void StartTimeUpSequence(void)
{
    vu32* dma;
    u32 timeUpTextGfx;
    u32 timeUpWarioGfx;
    u32 dmaControlSmall;
    u32 dmaControlMedium;
    u32 dmaControlLarge;
    s32 gfxOffset;

    gWarioPauseTimer = 1000;
    gDisableWario = 1;
    gStageTimerColonState = 2;
    gUnk_3000046 = 1;

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)sTimeUpTextPal;
    dma[1] = 0x05000240;
    dmaControlSmall = 0x80000010;
    dma[2] = dmaControlSmall;
    dma[2];

    timeUpTextGfx = (u32)sTimeUpTextGfx;
    dma[0] = timeUpTextGfx;
    dma[1] = 0x06010380;
    dmaControlMedium = 0x80000080;
    dma[2] = dmaControlMedium;
    dma[2];

    timeUpWarioGfx = (u32)sTimeUpWarioGfx;
    dma[0] = timeUpWarioGfx;
    dma[1] = 0x06010780;
    dma[2] = dmaControlMedium;
    /* Preserve the DMA completion read before reusing this temporary as an offset. */
    gfxOffset = dma[2];
    gfxOffset = -0x300;
    timeUpTextGfx += gfxOffset;
    dma[0] = timeUpTextGfx;
    dma[1] = 0x06010800;
    dmaControlLarge = 0x80000200;
    dma[2] = dmaControlLarge;
    gfxOffset = dma[2];
    gfxOffset = -0x300;
    timeUpWarioGfx += gfxOffset;
    dma[0] = timeUpWarioGfx;
    dma[1] = 0x06010C00;
    dma[2] = dmaControlLarge;
    dma[2];

    dma[0] = (u32)sTimeUpWarioPal;
    dma[1] = 0x05000200;
    dma[2] = dmaControlSmall;
    dma[2];

    SpawnSecondarySprite(gWarioData.yPosition, gWarioData.xPosition, 77);
    if (gCurrentStageNumber == 4) {
        AutosaveDefeat();
    }
    func_801E4D4();
    m4aSongNumStart(22);
}

void PlayWarioDefeatJingle(void)
{
    func_801E4D4();
    m4aSongNumStart(0x1c5);
}

void UpdateStageTimerAndScoreDrain(void)
{
    s8* digits;
    u32 frame;
    s32 updatedDigit;
    u8 timerDigitsState;
    register u8 digit PIN("r2");

    if (gSubGameMode != 2) {
        return;
    }
    if (gTimerState > 3) {
        return;
    }
    if (gWarioPauseTimer != 0) {
        return;
    }
    if ((gHeartMeter.current == 0) && (gUnk_3000022 == 0)) {
        StartTimeUpSequence();
        return;
    }

    gStageFrameCounter++;
    frame = gStageFrameCounter;
    if (gTimerState > 1) {
        if ((frame & 7) == 0) {
            AddStageScore(-1);
            SpawnSecondarySprite(gWarioData.yPosition, gWarioData.xPosition, SSPRITE_2E);
            if (gStageScore == 0) {
                gTimerState = 4;
                gWarioPauseTimer = 1000;
                gUnk_3000046 = 1;
                PlayWarioDefeatJingle();
            }
        }
    } else if ((frame & 31) == 0) {
        if (frame & 32) {
            gStageTimerColonState = 1;
        } else {
            gStageTimerColonState = 2;
        }
    }

    if ((frame & 63) != 0) {
        return;
    }
    if (gSwitchPressed != 0) {
        ScreenShakeRequestX(64);
    }

    if (gTimerState == 1) {
        timerDigitsState = 1;
        digits = gStageTimerDigits;
        digit = digits[0];
        if (digits[0] > 0) {
            updatedDigit = digit - 1;
            digits[0] = updatedDigit;
            if (((updatedDigit << 24) == 0) && ((digits[1] | digits[2]) == 0)) {
                m4aSongNumStart(0x1C3);
                if (gStageScore != 0) {
                    gTimerState = 2;
                } else {
                    gTimerState = 4;
                    gWarioPauseTimer = 1000;
                    gUnk_3000046 = 1;
                    PlayWarioDefeatJingle();
                }
            }
        } else {
            timerDigitsState = 2;
            digits[0] = 9;
            digit = digits[1];
            if (digits[1] > 0) {
                updatedDigit = digit - 1;
                digits[1] = updatedDigit;
            } else {
                timerDigitsState = 3;
                digits[1] = 5;
                digit = digits[2];
                if (digits[2] > 0) {
                    updatedDigit = digit - 1;
                    digits[2] = updatedDigit;
                } else {
                    digits[2] = 9;
                }
            }
        }
        gStageTimerDigitsState = timerDigitsState;
    }

    if ((gCurrentStageNumber == 4) && (gCurrentPassage != 0)) {
        if (gCurrentPassage == 5) {
            UpdateTreasureCollectionHudEffects();
        } else {
            StartCollectedJewelPieceHudEffect();
        }
    }
}

u8 LoadStageCollectiblesFromSave(void)
{
    u32 reservedR5;
    s32 passage;
    u32 stage;
    u32 new_var2;
    s32 new_var;
    passage = gCurrentPassage;
    new_var = passage;
    stage = (new_var2 = gCurrentStageNumber);
    if (stage == 4) {
        switch (passage) {
            case PASSAGE_EMERALD:

            case PASSAGE_RUBY:

            case PASSAGE_TOPAZ:

            case PASSAGE_SAPPHIRE:
                goto boss_collectibles;

            case PASSAGE_GOLDEN:
                goto golden_collectibles;

            default:
                goto clear_collectibles;
        }

    boss_collectibles:
        gCollectedNEJewelPiece = 2;

        gCollectedSEJewelPiece = 2;
        gCollectedSWJewelPiece = 2;
        gCollectedNWJewelPiece = 0;
        gCollectedCD = 0;
        gCollectedKeyzer = 0;
        goto function_end;
    golden_collectibles: {
        if (gCurrentCollection[PASSAGE_EMERALD][4].jewelPieceNE) {
            gCollectedNEJewelPiece = 2;
            gGoldenTreasureCount++;
        } else {
            gCollectedNEJewelPiece = 0;
        }
        if (gCurrentCollection[PASSAGE_EMERALD][4].jewelPieceSE) {
            gCollectedSEJewelPiece = 2;
            gGoldenTreasureCount++;
        } else {
            gCollectedSEJewelPiece = 0;
        }
        if (gCurrentCollection[PASSAGE_EMERALD][4].jewelPieceSW) {
            gCollectedSWJewelPiece = 2;
            gGoldenTreasureCount++;
        } else {
            gCollectedSWJewelPiece = 0;
        }
        if (gCurrentCollection[PASSAGE_RUBY][4].jewelPieceNE) {
            gCollectedNWJewelPiece = 2;
            gGoldenTreasureCount++;
        } else {
            gCollectedNWJewelPiece = 0;
        }
        if (gCurrentCollection[PASSAGE_RUBY][4].jewelPieceSE) {
            gHasBossTreasure5 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure5 = 0;
        }
        if (gCurrentCollection[PASSAGE_RUBY][4].jewelPieceSW) {
            gHasBossTreasure6 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure6 = 0;
        }
        if (gCurrentCollection[PASSAGE_TOPAZ][4].jewelPieceNE) {
            gHasBossTreasure7 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure7 = 0;
        }
        if (gCurrentCollection[PASSAGE_TOPAZ][4].jewelPieceSE) {
            gHasBossTreasure8 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure8 = 0;
        }
        if (gCurrentCollection[PASSAGE_TOPAZ][4].jewelPieceSW) {
            gHasBossTreasure9 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure9 = 0;
        }
        if (gCurrentCollection[PASSAGE_SAPPHIRE][4].jewelPieceNE) {
            gHasBossTreasure10 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure10 = 0;
        }
        if (gCurrentCollection[PASSAGE_SAPPHIRE][4].jewelPieceSE) {
            gHasBossTreasure11 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure11 = 0;
        }
        if (gCurrentCollection[PASSAGE_SAPPHIRE][4].jewelPieceSW) {
            gHasBossTreasure12 = 2;
            gGoldenTreasureCount++;
        } else {
            gHasBossTreasure12 = 0;
        }
        gCollectedCD = 0;
        gCollectedKeyzer = 0;
        goto function_end;
    }

    clear_collectibles:
        gCollectedNEJewelPiece = 0;

        gCollectedSEJewelPiece = 0;
        gCollectedSWJewelPiece = 0;
        gCollectedNWJewelPiece = 0;
        gCollectedCD = 0;
        gCollectedKeyzer = 0;
        goto function_end;
    }
    reservedR5 = gCurrentCollection[new_var][new_var2].jewelPieceNE;
    if (reservedR5) {
        gCollectedNEJewelPiece = 3;
    } else if (1) {
        gCollectedNEJewelPiece = 0;
    }
    if (gCurrentCollection[new_var][new_var2].jewelPieceSE) {
        gCollectedSEJewelPiece = 3;
    } else {
        gCollectedSEJewelPiece = 0;
    }
    if (gCurrentCollection[new_var][new_var2].jewelPieceSW) {
        gCollectedSWJewelPiece = 3;
    } else {
        gCollectedSWJewelPiece = 0;
    }
    if (gCurrentCollection[new_var][new_var2].jewelPieceNW) {
        gCollectedNWJewelPiece = 3;
    } else {
        gCollectedNWJewelPiece = 0;
    }
    if (gCurrentCollection[new_var][new_var2].cd) {
        gCollectedCD = 3;
    } else {
        gCollectedCD = 0;
    }
    if (gCurrentCollection[new_var][new_var2].keyzer) {
        gCollectedKeyzer = 2;
    } else {
        gCollectedKeyzer = 0;
    }
function_end:
    RETURN_NO_VALUE(0);
}

void InitStageHudAndScore(void)
{
    s8* pauseDisabled;
    s8* pauseLoad;
    s32 pauseValue;

    gBossTreasureType = gCurrentPassage;
    gBossTreasureStage = gCurrentStageNumber;
    pauseLoad = &gStageEntryPauseMenuDisabled;
    pauseValue = *pauseLoad;
    pauseDisabled = pauseLoad;
    if ((pauseValue != 0) || (gHasTemporarySave != 0)) {
        struct SecondarySprite* sprite;
        s32 remaining;
        sprite = gSecondarySpriteData;
        remaining = 7;
        do {
            u8 id;
            id = sprite->id;
            if (id <= 73) {
                sprite->status = 0;
            } else if (id == 75) {
                register u32 status PIN("r1");
                u32 one;
                u32 test;
                status = sprite->status;
                one = 1;
                test = one;
                test &= status;
                if (test != 0) {
                    struct WarioData* wario;
                    wario = &gWarioData;
                    sprite->yPosition = wario->yPosition - 96;
                    if (wario->horizontalDirection & 32) {
                        sprite->xPosition = wario->xPosition + 64;
                        sprite->pose = one;
                    } else {
                        sprite->xPosition = wario->xPosition - 64;
                        sprite->pose = 3;
                    }
                }
            }
            sprite++;
            remaining--;
        } while (remaining >= 0);
    } else {
        s32 baseAddress;
        s32 spriteAddress;
        u32 clear;
        baseAddress = (s32)gSecondarySpriteData;
        clear = 0;
        spriteAddress = baseAddress + (7 * sizeof(struct SecondarySprite));
        do {
            *(u8*)spriteAddress = clear;
            spriteAddress -= sizeof(struct SecondarySprite);
        } while (spriteAddress >= baseAddress);
    }
    if (*pauseDisabled == 0) {
        u32 clear;
        clear = gHasTemporarySave;
        if (clear == 0) {
            gCurrentShopItem = clear;
            {
                u8* digits;
                digits = gScoreDigits;
                digits[0] = clear;
                digits[1] = clear;
                digits[2] = clear;
                digits[3] = clear;
                digits[4] = clear;
            }
            gStageScore = clear;
            gStageFrameCounter = clear;
            gEnemyScoreDropCounter = clear;
            gSwitchPressed = clear;
            gGoldenTreasureCount = clear;
            LoadStageCollectiblesFromSave();
            gBossState = clear;
            gCuckooCondorPendulumLength = clear;
            gCuckooCondorMoveRight = clear;
            gCuckooCondorHasCapturedWario = clear;
            gInitialHealth = clear;
            gBossSequenceState = clear;
            gBgAnimationFrame = clear;
            gBgAnimationTimer = clear;
            gSpriteAiDynamicGraphicsTimer = clear;
            gGoldenDivaShopItemHealthTarget = clear;
            gUnk_3000A62 = clear;
            gSpriteAiRadius = clear;
        }
    }
    gUnk_3000C04 = 0;
    gUnk_3000C0E = 0;
    gVortexYPosition = 0;
    gVortexXPosition = 0;
    gVortexGfxSlot = 0;
    gStageScoreDigitsDirty = 1;
    gStageTimerDigitsState = 3;
    gStageTimerColonState = 2;
    UploadStageScoreDigitTiles();
    UploadStageTimerDigitTiles();
    UploadStageTimerColonTiles();
    {
        vu32* dma;
        const u8* palette;
        switch (gCurrentPassage) {
            case 0:
                dma = (vu32*)0x040000D4;
                palette = sStageEntryPassagePal;
                break;
            case 1:
                dma = (vu32*)0x040000D4;
                palette = sStageEmeraldPassagePal;
                break;
            case 2:
                dma = (vu32*)0x040000D4;
                palette = sStageRubyPassagePal;
                break;
            case 3:
                dma = (vu32*)0x040000D4;
                palette = sStageTopazPassagePal;
                break;
            case 4:
                dma = (vu32*)0x040000D4;
                palette = sStageSapphirePassagePal;
                break;
            case 5:
                dma = (vu32*)0x040000D4;
                palette = sStageGoldenPyramidPal;
                break;
            default:
                goto skip_passage_palette;
        }
        dma[0] = (u32)palette;
        dma[1] = 0x05000280;
        dma[2] = 0x80000010;
        dma[2];
    }
skip_passage_palette:
    if (gCurrentStageNumber != 4) {
        u8 state;
        vu32* dma;
        const u8* graphics;
        state = gCollectedNEJewelPiece;
        if (state == 3) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconNEGfx;
        } else if (state != 0) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconPartialNEGfx;
        } else {
            goto skip_ne;
        }
        dma[0] = (u32)graphics;
        dma[1] = 0x06011C20;
        dma[2] = 0x80000010;
        dma[2];
    skip_ne:
        state = gCollectedSEJewelPiece;
        if (state == 3) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconSEGfx;
        } else if (state != 0) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconPartialSEGfx;
        } else {
            goto skip_se;
        }
        dma[0] = (u32)graphics;
        dma[1] = 0x06012020;
        dma[2] = 0x80000010;
        dma[2];
    skip_se:
        state = gCollectedSWJewelPiece;
        if (state == 3) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconSWGfx;
        } else if (state != 0) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconPartialSWGfx;
        } else {
            goto skip_sw;
        }
        dma[0] = (u32)graphics;
        dma[1] = 0x06012000;
        dma[2] = 0x80000010;
        dma[2];
    skip_sw:
        state = gCollectedNWJewelPiece;
        if (state == 3) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconNWGfx;
        } else if (state != 0) {
            dma = (vu32*)0x040000D4;
            graphics = sJewelPieceIconPartialNWGfx;
        } else {
            goto skip_nw;
        }
        dma[0] = (u32)graphics;
        dma[1] = 0x06011C00;
        dma[2] = 0x80000010;
        dma[2];
    skip_nw:
        state = gCollectedCD;
        if (state == 3) {
            dma = (vu32*)0x040000D4;
            graphics = sCDIconGfx;
        } else if (state != 0) {
            dma = (vu32*)0x040000D4;
            graphics = sCDIconPartialGfx;
        } else {
            return;
        }
        dma[0] = (u32)graphics;
        dma[1] = 0x060114C0;
        dma[2] = 0x80000010;
        dma[2];
    }
}

void DrawCurrentSecondarySprite(u32 index)
{
    struct SecondarySprite* spriteAddress;
    struct SecondarySprite* sprite;
    u32 spriteId;
    const u16* source;
    u16* destination;
    u32 count;
    u32 nextSlot;
    register u32 yOffset PIN("r9");
    u32 xOffset;
    u32 affine;
    register u32 matrixBits PIN("r10");
    register u32 attr PIN("r0");
    volatile u32 stackWords[3];
    stackWords[0] = index;
    spriteAddress = &gCurrentSecondarySprite;
    spriteId = spriteAddress->id;
    sprite = spriteAddress;
    if (spriteId > SSPRITE_WARIO_DYING) {
        u32 currentSlot;
        u8* currentSlotPointer;
        currentSlotPointer = &gOamSlotsUsed;
        currentSlot = *currentSlotPointer;
        source = (const u16*)GP(gCurrentSecondarySpriteOamData);
        count = *source++;
        nextSlot = count + currentSlot;
        if ((s32)nextSlot > 127) {
            return;
        }
        {
            u32 slotOffset;
            register OamData* oamBase PIN("r2");
            slotOffset = currentSlot << 3;
            oamBase = gOamBuffer;
            destination = (u16*)(slotOffset + (u32)oamBase);
        }
        yOffset = sprite->yPosition >> 2;
        xOffset = sprite->xPosition >> 2;
        affine = sprite->status & 2;
        if (count != 0) {
            OamData* oam;
            {
                u32 indexMask;
                u32 indexValue;
                indexMask = 7;
                indexValue = stackWords[0];
                indexValue &= indexMask;
                indexValue <<= 1;
                matrixBits = indexValue;
            }
            oam = (OamData*)destination;
            do {
                attr = *source++;
                *(volatile u16*)destination = attr;
                destination++;
                ((u8*)oam)[0] = attr + yOffset;
                attr = *source++;
                *(volatile u16*)destination = attr;
                destination++;
                {
                    u32 adjustedX;
                    u32 xWork;
                    register u32 mask PIN("r0");
                    xWork = xOffset;
                    adjustedX = attr + xWork;
                    mask = 0x1FF;
                    adjustedX &= mask;
                    xWork = oam->all.attr1;
                    mask = 0xFFFFFE00;
                    mask &= xWork;
                    mask |= adjustedX;
                    oam->all.attr1 = mask;
                }
                *destination++ = *source++;
                if (affine != 0) {
                    u8 value;
                    ((u8*)oam)[1] |= 3;
                    value = ((u8*)oam)[3];
                    value |= 0x20;
                    value |= 0x10;
                    {
                        s32 mask;
                        mask = 15;
                        mask = -mask;
                        value &= mask;
                    }
                    value |= matrixBits;
                    ((u8*)oam)[3] = value;
                }
                destination++;
                oam++;
                count--;
            } while (count != 0);
        }
    } else if (spriteId == SSPRITE_KEYZER) {
        u32 currentSlot;
        /* Reserve the later loop counter across this branch so agbcc does not reuse r6 for the zero value. */
        if (gUnk_3000034 != 0) {
            matrixBits = 0;
        } else {
            u32 overBackground;
            overBackground = gDrawWarioOverBackground;
            matrixBits = 2;
            if (overBackground != 0) {
                matrixBits = 1;
            }
        }
        ASM_BARRIER(: : "r"(count));
        yOffset = (u16)((sprite->yPosition >> 2) - (gBg1YPosition >> 2));
        {
            u8* currentSlotPointer;
            currentSlotPointer = &gOamSlotsUsed;
            currentSlot = *currentSlotPointer;
        }
        source = (const u16*)GP(gCurrentSecondarySpriteOamData);
        count = *source++;
        nextSlot = count + currentSlot;
        if ((s32)nextSlot > 127) {
            return;
        }
        {
            u32 slotOffset;
            register OamData* oamBase PIN("r2");
            slotOffset = currentSlot << 3;
            oamBase = gOamBuffer;
            destination = (u16*)(slotOffset + (u32)oamBase);
        }
        xOffset = (u16)((sprite->xPosition >> 2) - (gBg1XPosition >> 2));
        affine = sprite->status & 2;
        if (count != 0) {
            OamData* oam;
            {
                register u32 modeValue PIN("r2");
                register volatile u8* stackPointer PIN("r1");
                u32 priorityValue;
                modeValue = 3;
                stackPointer = (volatile u8*)stackWords;
                stackPointer[4] = modeValue;
                priorityValue = matrixBits;
                ASM_BARRIER(: "+r"(priorityValue));
                priorityValue <<= 2;
                stackWords[2] = priorityValue;
            }
            oam = (OamData*)destination;
            {
                u32 indexMask;
                u32 indexValue;
                indexMask = 7;
                indexValue = stackWords[0];
                indexValue &= indexMask;
                indexValue <<= 1;
                matrixBits = indexValue;
            }
            do {
                u8 value;
                attr = *source++;
                *(volatile u16*)destination = attr;
                destination++;
                ((u8*)oam)[0] = attr + yOffset;
                attr = *source++;
                *(volatile u16*)destination = attr;
                destination++;
                {
                    u32 adjustedX;
                    u32 xWork;
                    register u32 mask PIN("r0");
                    xWork = xOffset;
                    adjustedX = attr + xWork;
                    mask = 0x1FF;
                    adjustedX &= mask;
                    xWork = oam->all.attr1;
                    mask = 0xFFFFFE00;
                    mask &= xWork;
                    mask |= adjustedX;
                    oam->all.attr1 = mask;
                }
                *destination++ = *source++;
                {
                    u32 priorityByte;
                    u32 priorityMask;
                    u32 priorityValue;
                    priorityByte = ((u8*)oam)[5];
                    priorityMask = 13;
                    priorityMask = -priorityMask;
                    priorityMask &= priorityByte;
                    priorityValue = stackWords[2];
                    priorityMask |= priorityValue;
                    ((u8*)oam)[5] = priorityMask;
                }
                if (affine != 0) {
                    u32 modeByte;
                    register u32 modeBits PIN("r2");
                    modeByte = ((u8*)oam)[1];
                    modeBits = (u32)stackWords;
                    modeBits = *(volatile u8*)(modeBits + 4);
                    ASM_BARRIER(: "+r"(modeBits));
                    modeByte |= modeBits;
                    ((u8*)oam)[1] = modeByte;
                    value = ((u8*)oam)[3];
                    value |= 0x20;
                    value |= 0x10;
                    {
                        s32 mask;
                        mask = 15;
                        mask = -mask;
                        value &= mask;
                    }
                    {
                        u32 matrixValue;
                        matrixValue = matrixBits;
                        ASM_BARRIER(: "+r"(matrixValue));
                        value |= matrixValue;
                    }
                    ((u8*)oam)[3] = value;
                }
                destination++;
                oam++;
                count--;
            } while (count != 0);
        }
        {
            u32 slotValue;
            u8* slotPointer;
            slotValue = nextSlot;
            slotPointer = &gOamSlotsUsed;
            *slotPointer = slotValue;
        }
        if (affine == 0) {
            return;
        }
        {
            register u32 inputIndex PIN("r0");
            u32 indexOffset;
            register u32 address PIN("r0");
            register OamData* paBase PIN("r1");
            register OamData* basePointer PIN("r6");
            inputIndex = stackWords[0];
            indexOffset = inputIndex << 2;
            address = indexOffset;
            address += 96;
            address <<= 3;
            paBase = gOamBuffer;
            address += (u32)paBase;
            ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePA;
            address = indexOffset;
            address += 97;
            address <<= 3;
            basePointer = gOamBuffer;
            address += (u32)basePointer;
            ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePB;
            address = indexOffset;
            address += 98;
            address <<= 3;
            address += (u32)basePointer;
            ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePC;
            address = indexOffset;
            address += 99;
            address <<= 3;
            address += (u32)basePointer;
            ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePD;
        }
        return;
    } else {
        u32 currentSlot;
        register u16* backgroundPointer PIN("r5");
        backgroundPointer = &gBg1YPosition;
        if (spriteId <= SSPRITE_49) {
            u32 background;
            register u32 lowerBound PIN("r1");
            u32 spriteY;
            u32 offset;
            u32 adjustedY;
            background = *backgroundPointer;
            lowerBound = background;
            lowerBound += 0xC0;
            spriteY = sprite->yPosition;
            offset = 0x80;
            offset <<= 1;
            adjustedY = spriteY + offset;
            if ((s32)lowerBound > (s32)adjustedY) {
                sprite->status = 0;
                return;
            }
            lowerBound = 0xF0;
            lowerBound <<= 2;
            spriteY = background + lowerBound;
            if ((s32)spriteY < (s32)adjustedY) {
                sprite->status = 0;
                return;
            }
        }
        yOffset = (u16)((sprite->yPosition >> 2) - (*backgroundPointer >> 2));
        {
            u8* currentSlotPointer;
            currentSlotPointer = &gOamSlotsUsed;
            currentSlot = *currentSlotPointer;
        }
        source = (const u16*)GP(gCurrentSecondarySpriteOamData);
        count = *source++;
        nextSlot = count + currentSlot;
        if ((s32)nextSlot > 127) {
            return;
        }
        {
            u32 slotOffset;
            register OamData* oamBase PIN("r2");
            slotOffset = currentSlot << 3;
            oamBase = gOamBuffer;
            destination = (u16*)(slotOffset + (u32)oamBase);
        }
        xOffset = (u16)((sprite->xPosition >> 2) - (gBg1XPosition >> 2));
        affine = sprite->status & 2;
        if (count != 0) {
            register OamData* oam PIN("r4");
            {
                u32 indexMask;
                u32 indexValue;
                indexMask = 7;
                indexValue = stackWords[0];
                indexValue &= indexMask;
                indexValue <<= 1;
                matrixBits = indexValue;
            }
            oam = (OamData*)destination;
            do {
                attr = *source++;
                *(volatile u16*)destination = attr;
                destination++;
                ((u8*)oam)[0] = attr + yOffset;
                attr = *source++;
                *(volatile u16*)destination = attr;
                destination++;
                {
                    u32 adjustedX;
                    u32 xWork;
                    register u32 mask PIN("r0");
                    xWork = xOffset;
                    adjustedX = attr + xWork;
                    mask = 0x1FF;
                    adjustedX &= mask;
                    xWork = oam->all.attr1;
                    mask = 0xFFFFFE00;
                    mask &= xWork;
                    mask |= adjustedX;
                    oam->all.attr1 = mask;
                }
                *destination++ = *source++;
                if (affine != 0) {
                    u8 value;
                    ((u8*)oam)[1] |= 3;
                    value = ((u8*)oam)[3];
                    value |= 0x20;
                    value |= 0x10;
                    {
                        s32 mask;
                        mask = 15;
                        mask = -mask;
                        value &= mask;
                    }
                    value |= matrixBits;
                    ((u8*)oam)[3] = value;
                }
                destination++;
                oam++;
                count--;
            } while (count != 0);
        }
    }
    {
        u32 slotValue;
        u8* slotPointer;
        slotValue = nextSlot;
        slotPointer = &gOamSlotsUsed;
        *slotPointer = slotValue;
    }
    if (affine == 0) {
        return;
    }
    {
        register u32 inputIndex PIN("r1");
        register u32 indexOffset PIN("r2");
        register u32 address PIN("r0");
        register OamData* basePointer PIN("r6");
        inputIndex = stackWords[0];
        indexOffset = inputIndex << 2;
        address = indexOffset;
        address += 96;
        address <<= 3;
        basePointer = gOamBuffer;
        address += (u32)basePointer;
        ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePA;
        address = indexOffset;
        address += 97;
        address <<= 3;
        address += (u32)basePointer;
        ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePB;
        address = indexOffset;
        address += 98;
        address <<= 3;
        address += (u32)basePointer;
        ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePC;
        address = indexOffset;
        address += 99;
        address <<= 3;
        address += (u32)basePointer;
        ((OamData*)address)->all.affineParam = gSecondarySpriteAffinePD;
    }
}

void AddStageScore(s32 score)
{
    gStageScoreDigitsDirty = 1;
    gStageScore += score;
    if (gStageScore < 0) {
        gStageScore = 0;
    }

    gScoreDigits[4] = gStageScore / 10000;
    if (gScoreDigits[4] > 9) {
        gScoreDigits[4] = 9;
        gScoreDigits[3] = 9;
        gScoreDigits[2] = 9;
        gScoreDigits[1] = 9;
        gScoreDigits[0] = 9;
    } else {
        gScoreDigits[3] = (gStageScore / 1000) % 10;
        gScoreDigits[2] = (gStageScore / 100) % 10;
        gScoreDigits[1] = (gStageScore / 10) % 10;
        gScoreDigits[0] = gStageScore % 10;
    }
}

void SpawnSecondarySprite(u32 y, u32 x, u32 id)
{
    struct SecondarySprite* base;
    volatile u8* statusBytes;
    struct SecondarySprite* sprite;
    u16 yPosition;
    u16 xPosition;
    u8 spriteId;
    u8 replacementIndex;
    u8 longestTimer;
    u8 currentId;
    s32 index;
    u8 timer;
    s32 offset;
    yPosition = y;
    xPosition = x;
    spriteId = id;
    longestTimer = 0;
    index = 0;
    if (gSecondarySpriteData[0].status != 0) {
        do {
            statusBytes = (volatile u8*)gSecondarySpriteData;
            index++;
            if (index > 7) {
                break;
            }

            offset = index;
            offset += index;
            offset += index;
            offset <<= 2;
            if (statusBytes[offset] == 0) {
                longestTimer = 1;
                break;
            }
        } while (1);
        if (longestTimer == 0) {
            replacementIndex = 0xFF;
            index = 0;
            do {
                currentId = gSecondarySpriteData[index].id;
                timer = 0;
                if (currentId <= 73) {
                    timer = gSecondarySpriteData[index].timer;
                }
                if (longestTimer < timer) {
                    longestTimer = timer;
                    replacementIndex = index;
                }
                index++;
            } while (index <= 7);

            if (replacementIndex == 0xFF) {
                return;
            }
            index = replacementIndex;
        }
    }
    base = gSecondarySpriteData;
    sprite = &base[index];
    sprite->status = 1;
    sprite->yPosition = yPosition;
    sprite->xPosition = xPosition;
    sprite->animationTimer = 0;
    sprite->work0 = 0;
    sprite->id = spriteId;
    sprite->pose = 0;
    sprite->timer = 0;
    sprite->unk5 = 0;
}

void ConvertKeyzerToTimeUpEffect(void)
{
    struct SecondarySprite* sprite;

    sprite = &gCurrentSecondarySprite;
    sprite->status = 1;
    sprite->animationTimer = 0;
    sprite->work0 = 0;
    sprite->id = SSPRITE_2F;
    sprite->pose = 0;
    sprite->timer = 0;
    sprite->unk5 = 0;
    AdvanceSecondarySpriteAnimation(sUnk_840FB94);
}

void ResetTimeUpWarioSecondarySprite(void)
{
    struct SecondarySprite* sprite;

    sprite = &gCurrentSecondarySprite;
    sprite->status = 1;
    sprite->animationTimer = 0;
    sprite->work0 = 0;
    sprite->id = SSPRITE_50;
    sprite->pose = 0;
    sprite->timer = 0;
    sprite->unk5 = 0;
    AdvanceSecondarySpriteAnimation(sUnk_840FB94);
}

void SpawnScoreCoinDropSprites(u32 y, u32 x, s32 amount)
{
    u16 yPosition;
    u16 xPosition;
    s32 offset;

    yPosition = y;
    xPosition = x;
    offset = 0;
    while (amount > 99) {
        SpriteSpawnAsChild(133, 0, 0, yPosition - offset, xPosition - offset);
        amount -= 100;
        offset += 32;
    }
    while (amount > 49) {
        SpriteSpawnAsChild(132, 0, 0, yPosition - offset, xPosition - offset);
        amount -= 50;
        offset += 32;
    }
    while (amount > 9) {
        SpriteSpawnAsChild(131, 0, 0, yPosition - offset, xPosition - offset);
        amount -= 10;
        offset += 32;
    }
    while (amount > 4) {
        SpriteSpawnAsChild(130, 0, 0, yPosition - offset, xPosition - offset);
        amount -= 5;
        offset += 32;
    }
    while (amount > 0) {
        SpriteSpawnAsChild(129, 0, 0, yPosition - offset, xPosition - offset);
        amount--;
        offset += 32;
    }
}

void SpawnEnemyScoreDrop(u32 id, u32 y, u32 x, u32 amount)
{
    u16 yPosition;
    u16 xPosition;

    yPosition = y;
    xPosition = x;
    gEnemyScoreDropCounter++;
    if (amount != 0) {
        SpawnScoreCoinDropSprites(yPosition, xPosition, amount);
        SpriteSpawnAsChild(13, 0, 0, yPosition, xPosition);
    }
}

void ChangeStageScoreAndDropCoins(s32 scoreChange)
{
    s32 currentScore;
    s32 lostScore;

    currentScore = gStageScore;
    gUnk_3000C04 = 0;
    if (scoreChange < 0) {
        lostScore = -scoreChange;
        if (currentScore >= lostScore) {
            SpawnScoreCoinDropSprites(gWarioData.yPosition, gWarioData.xPosition, lostScore);
            AddStageScore(scoreChange);
        } else {
            SpawnScoreCoinDropSprites(gWarioData.yPosition, gWarioData.xPosition, currentScore);
            AddStageScore(-currentScore);
        }
    }
}

void SpawnOnePointTileEffect(u32 y, u32 x)
{
    u16 yPosition;
    u16 xPosition;

    yPosition = y;
    xPosition = x;
    AddStageScore(1);
    SpawnSecondarySprite(yPosition, xPosition, 0);
    if (gWarioData.reaction == 1) {
        m4aSongNumStart(0x150);
    } else {
        m4aSongNumStart(0x13e);
    }
}

void SpawnTenPointTileEffect(u32 y, u32 x)
{
    u16 yPosition;
    u16 xPosition;

    yPosition = y;
    xPosition = x;
    AddStageScore(10);
    SpawnSecondarySprite(yPosition, xPosition, 2);
    if (gWarioData.reaction == 1) {
        m4aSongNumStart(0x151);
    } else {
        m4aSongNumStart(0x13f);
    }
}

void SpawnTileDebrisParticle(u32 y, u32 x, u32 debrisOffset, u32 direction, u32 parity)
{
    u16 yPosition;
    u16 xPosition;
    u8 offset;
    u8 spriteDirection;
    u8 variant;
    u8 id;

    yPosition = y;
    xPosition = x;
    offset = debrisOffset;
    spriteDirection = direction;
    variant = parity;
    switch (offset) {
        case 0:
            id = spriteDirection + 7;
            if (variant != 0) {
                id += 6;
            }
            break;
        case 1:
            id = spriteDirection + 19;
            if (variant != 0) {
                id += 6;
            }
            break;
        case 2:
            id = spriteDirection + 31;
            if (variant != 0) {
                id += 6;
            }
            break;
        case 3:
            id = 44;
            if (variant != 0) {
                id = 45;
            }
            break;
        default:
            id = 6;
            break;
    }
    SpawnSecondarySprite((u16)(yPosition + 32), (u16)(xPosition + 32), id);
}

void AdvanceSecondarySpriteAnimation(const struct AnimationFrame* animation)
{
    gCurrentSecondarySprite.work0++;
    if (animation[gCurrentSecondarySprite.animationTimer].time <= (u8)gCurrentSecondarySprite.work0) {
        gCurrentSecondarySprite.work0 = 0;
        gCurrentSecondarySprite.animationTimer++;
        if (animation[gCurrentSecondarySprite.animationTimer].time == 0) {
            gCurrentSecondarySprite.animationTimer = 0;
        }
    }
    GP_SET(gCurrentSecondarySpriteOamData, GP(animation[gCurrentSecondarySprite.animationTimer].oam));
}

#include "stage_results.h"
#include "color_effects.h"
#include "score.h"
#include "sound.h"
#include "stage_select.h"
#include "wario.h"

u32 UpdateStageResults(void)
{
    u32 pressed;
    volatile u32* dma;

    pressed = 0;
    switch (gStageEntrySequenceStep) {
        case 1:
            InitStageResults();
            gStageEntrySequenceStep++;
            break;
        case 2:
            REG_DISPCNT = 0x1701;
            if (!StageResultsTimerExpired(30)) {
                break;
            }
            if (gStageResultsTreasure.state > 4) {
                gStageEntrySequenceStep = 4;
                break;
            }
            if (gStageResultsTreasureSound == 1) {
                m4aSongNumStart(SE_RESULTS_FOUND_JEWEL_PIECE);
                gStageResultsTreasureSound = 0;
            } else if (gStageResultsTreasureSound == 2) {
                m4aSongNumStart(SE_RESULTS_FOUND_CD);
                gStageResultsTreasureSound = 0;
            }
            gStageEntrySequenceStep++;
            break;
        case 3:
            if (gStageResultsAnimationFinished == 0) {
                ShrinkStageResultsTreasure();
                break;
            }
            if (!StageResultsTimerExpired(15)) {
                break;
            }
            gStageResultsAnimationFinished = 0;
            if (gStageResultsTreasureSound == 1) {
                m4aSongNumStart(SE_RESULTS_FOUND_JEWEL_PIECE);
                gStageResultsTreasureSound = 0;
            } else if (gStageResultsTreasureSound == 2) {
                m4aSongNumStart(SE_RESULTS_FOUND_CD);
                gStageResultsTreasureSound = 0;
            }
            if (gStageResultsTreasure.state > 4) {
                gStageEntrySequenceStep++;
            }
            break;
        case 4:
            if (gStageResultsAnimationFinished == 0) {
                ConvertStageResultsHeartToScore();
                break;
            }
            if (!StageResultsTimerExpired(15)) {
                break;
            }
            gStageResultsAnimationFinished = 0;
            if (gStageResultsRemainingHearts != 0) {
                break;
            }
            if (gStageScore > 999) {
                gStageResultsScoreRank = 1;
            } else if (gStageScore > 799) {
                gStageResultsScoreRank = 2;
            } else if (gStageScore > 599) {
                gStageResultsScoreRank = 3;
            } else {
                gStageResultsScoreRank = 0;
            }
            gStageEntrySequenceStep++;
            if (gNewHighScore != 0) {
                gStageResultsNewHighScoreIcon.state = 1;
                m4aSongNumStart(SE_HIGH_SCORE);
            }
            break;
        case 5:
            if (StageResultsTimerExpired(75)) {
                gStageEntrySequenceStep++;
            }
            break;
        case 6:
            if (TransferStageScoreToTotal()) {
                gStageEntrySequenceStep++;
            }
            break;
        case 7:
            if (gButtonsPressed & A_BUTTON) {
                pressed = 1;
            }
            if (!pressed) {
                if (!StageResultsTimerExpired(120)) {
                    break;
                }
            } else {
                m4aSongNumStart(SE_STAGE_RESULTS_SKIP);
            }
            if (gCurrentPassage != 0 || gCollectedKeyzer != 1) {
                goto exitResults;
            }
            REG_DISPCNT = 0x701;
            gStageEntrySequenceStep++;
            if (gLanguage == 0) {
                dma = (volatile u32*)0x040000D4;
                dma[0] = (u32)sStageResultsLanguage0Tilemap;
                dma[1] = 0x0600C000;
                dma[2] = 0x80000400;
                (void)dma[2];
            }
            if (gLanguage == 1) {
                dma = (volatile u32*)0x040000D4;
                dma[0] = (u32)sStageResultsLanguage1Tilemap;
                dma[1] = 0x0600C000;
                dma[2] = 0x80000400;
                (void)dma[2];
            }
            break;
        case 8:
            if (StageResultsTimerExpired(120)) {
                gStageEntrySequenceStep++;
            }
            break;
        case 9:
            if (!(gButtonsPressed & A_BUTTON)) {
                break;
            }
            m4aSongNumStart(SE_CONFIRM);
        exitResults:
            REG_DISPCNT = 0x401;
            gColorFading.type = 4;
            REG_BLDCNT = 0x3FFF;
            m4aMPlayFadeOut(GP(gMPlayTable[2].info), 8);
            return 1;
    }
    return 0;
}

void InitStageResults(void)
{
    struct StageResultsIcon* icons;
    s32 i;

    gStageResultsTreasure.x = sStageResultsTreasureXPositions[gUnk_3003C4A];
    gStageResultsTreasure.y = 112;
    gStageResultsTreasure.scale = 0x400;
    gStageResultsTreasure.state = 0;

    icons = gStageResultsTreasureIcons;
    i = 0;
    do {
        icons[i].x = 0;
        icons[i].y = 0;
        icons[i].state = 0;
        i++;
    } while (i <= 4);

    gStageResultsNewHighScoreIcon.x = 0;
    gStageResultsNewHighScoreIcon.y = 0;
    gStageResultsNewHighScoreIcon.state = 0;

    if (gCollectedNEJewelPiece == 3) {
        gStageResultsTreasureIcons[0].state = 1;
    }
    if (gCollectedSEJewelPiece == 3) {
        gStageResultsTreasureIcons[1].state = 1;
    }
    if (gCollectedSWJewelPiece == 3) {
        gStageResultsTreasureIcons[2].state = 1;
    }
    if (gCollectedNWJewelPiece == 3) {
        gStageResultsTreasureIcons[3].state = 1;
    }
    if (gCollectedCD == 3) {
        gStageResultsTreasureIcons[4].state = 1;
    }

    gStageResultsTreasureSound = 0;
    CheckStageResultsJewelSet();
    AdvanceStageResultsTreasure();
    gStageResultsScoreSoundTimer = 0;
    DrawStageResultsScores();
    gStageResultsAnimationFinished = 0;
    gStageResultsRemainingHearts = gHeartMeter.current + gHeartMeter.filling;
    if (gStageResultsRemainingHearts > 8) {
        gStageResultsRemainingHearts = 8;
    }
    m4aSongNumStart(SE_RESULTS_START);
    gUnk_3003C56 = 0;
    REG_BLDCNT = 0x442;
    REG_BLDALPHA = 0x907;
}

void CheckStageResultsJewelSet(void)
{
    if (gStageResultsTreasureIcons[0].state != 0 && gStageResultsTreasureIcons[1].state != 0 &&
        gStageResultsTreasureIcons[2].state != 0 && gStageResultsTreasureIcons[3].state != 0) {
        gStageResultsTreasureIcons[0].state = 0;
        gStageResultsTreasureIcons[1].state = 0;
        gStageResultsTreasureIcons[2].state = 0;
        gStageResultsTreasureIcons[3].state = 0;
        gStageResultsAllJewelPieces = 1;
    } else {
        gStageResultsAllJewelPieces = 0;
    }
}

void AdvanceStageResultsTreasure(void)
{
    switch (gStageResultsTreasure.state) {
        case 0:
            if ((u8)(gCollectedNEJewelPiece - 1) <= 1) {
                gStageResultsTreasureSound = 1;
                break;
            }
            gStageResultsTreasure.state++;
        case 1:
            if ((u8)(gCollectedSEJewelPiece - 1) <= 1) {
                gStageResultsTreasureSound = 1;
                break;
            }
            gStageResultsTreasure.state++;
        case 2:
            if ((u8)(gCollectedSWJewelPiece - 1) <= 1) {
                gStageResultsTreasureSound = 1;
                break;
            }
            gStageResultsTreasure.state++;
        case 3:
            if ((u8)(gCollectedNWJewelPiece - 1) <= 1) {
                gStageResultsTreasureSound = 1;
                break;
            }
            gStageResultsTreasure.state++;
        case 4:
            if (gStageResultsAllJewelPieces == 0) {
                CheckStageResultsJewelSet();
                if (gStageResultsAllJewelPieces == 1) {
                    m4aSongNumStart(SE_GET_ALL_JEWEL_PIECES);
                    gUnk_3003C56 = 1;
                }
            }
            if ((u8)(gCollectedCD - 1) <= 1) {
                gStageResultsTreasureSound = 2;
                gStageResultsTreasure.y = 112;
                gStageResultsTreasure.x = 144;
            } else {
                gStageResultsTreasure.state++;
                gStageResultsAnimationFinished = 1;
            }
            break;
    }
}

void ShrinkStageResultsTreasure(void)
{
    gStageResultsTreasure.scale -= 30;
    if (gStageResultsTreasure.scale <= 0xFF) {
        gStageResultsTreasure.scale = 0x400;
        gStageResultsTreasureIcons[gStageResultsTreasure.state].state = 2;
        if (gStageResultsTreasure.state == 4) {
            m4aSongNumStart(SE_RESULTS_CD_APPEAR);
        } else {
            m4aSongNumStart(SE_RESULTS_JEWEL_APPEAR);
        }
        gStageResultsTreasure.state++;
        AdvanceStageResultsTreasure();
        gStageResultsAnimationFinished = 1;
    }
}

u32 StageResultsTimerExpired(s32 duration)
{
    gStageResultsTimer++;
    if (gStageResultsTimer > duration) {
        gStageResultsTimer = 0;
        return 1;
    }
    return 0;
}

void DrawStageResultsScores(void)
{
    UploadFiveDigitNumberTiles(gStageScore, sStageResultsDigitGfx, 0x8740);
    UploadTallFiveDigitNumberTiles(gTotalScoreTemp, sStageResultsDigitGfx - 0x2C0, 0x8980);
}

u32 TransferStageScoreToTotal(void)
{
    s32 i;
    s32* totalScore;
    s32* stageScore;
    s32* stageLoad;
    s32* totalLoop;
    if (gButtonsPressed & A_BUTTON) {
        m4aSongNumStart(SE_STAGE_RESULTS_SKIP);
        gTotalScoreTemp += gStageScore;
        gStageScore = 0;
    }

    totalScore = &gTotalScoreTemp;
    stageScore = &gStageScore;
    stageLoad = (s32*)(volatile s32*)stageScore;
    totalLoop = (s32*)(volatile s32*)totalScore;
    i = 3;
    while (i >= 0) {
        if (*stageLoad > 0) {
            *stageScore = *stageLoad - 1;
            *totalLoop = *totalLoop + 1;
        }
        i--;
    }
    if (*totalScore > 99999) {
        *totalScore = 99999;
    }
    DrawStageResultsScores();
    if (gStageScore == 0) {
        return 1;
    }

    if (gStageResultsScoreSoundTimer % 5 == 0) {
        m4aSongNumStart(SE_RESULTS_SCORE_COUNT);
    }
    gStageResultsScoreSoundTimer++;
    return 0;
}

void ConvertStageResultsHeartToScore(void)
{
    gStageResultsRemainingHearts--;
    m4aSongNumStart(SE_RESULTS_HEART_TO_SCORE);
    gStageScore += 5;
    if (gStageScore > 99999) {
        gStageScore = 99999;
    }
    DrawStageResultsScores();
    gStageResultsAnimationFinished = 1;
}

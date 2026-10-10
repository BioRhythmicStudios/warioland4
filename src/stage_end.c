#include "stage_end.h"

#include "background_registers.h"
#include "color_effects.h"
#include "fixed_point.h"
#include "hblank.h"
#include "interrupt_callbacks.h"
#include "main.h"
#include "score.h"

s32 RunCurrentPassageStageEnd(void);

void UpdateStageEndAffineTransform(void);
void UpdateStageEndScale(void);
void InitStageEndAffine(void);
void InitStageEndVBlank(void);
void UpdateStageEndBlend(void);

extern s8 gUnk_3003BF0;
extern s8 gUnk_3003BF1;
extern s8 gStageEndHorizontalDirection;
extern s8 gUnk_3003BF3;
extern s8 gStageEndScaleStep;

s32 UpdateStageExitSequence(void)
{
    s32 resultCode;
    s32 transitionComplete;
    u8 nextState;
    u8 exitType;

    resultCode = 0;
    UploadColorFadePalettes();

    if (((s8)gStageExitType) < 0) {
        resultCode = RunCurrentPassageStageEnd();
        return resultCode;
    }

    nextState = 0;
    switch (gSpriteAiDropTimer) {
        case 0:
            gUnk_3000046 = 1;
            exitType = gStageExitType;
            nextState = 18;
            if (exitType == 5) {
                goto set_state;
            }
            nextState = 14;
            if (exitType == 3) {
                goto set_state;
            }
            nextState = 1;
            if (exitType == 2) {
                goto set_state;
            }
            nextState = 7;
            if (exitType == 1) {
                goto set_state;
            }
            nextState = 22;
            if (exitType == 0) {
                goto set_state;
            }
            if (exitType == 6) {
                nextState = 29;
                goto set_state;
            }
            break;

        case 1:
            gUnk_300188E++;
            if (gUnk_300188E > 20) {
                nextState = 128;
                goto set_state;
            }
            break;

        case 2:
            SetHBlankMode(1);
            nextState = 128;
            /* fall through */
        case 3:
            gUnk_300188E++;
            if (gUnk_300188E > 120) {
                nextState = 128;
                goto set_state;
            }
            break;

        case 6:
            gUnk_300188E++;
            if (gUnk_300188E <= 45) {
                break;
            }
            /* fall through */
        case 29:
        case 30:
        set_31:
            nextState = 31;
            goto set_state;

        case 13:
            gUnk_300188E++;
            if (gUnk_300188E > 10) {
                goto set_31_bridge;
            }
            break;
        set_31_bridge:
            goto set_31;

        case 18:
            gUnk_300188E++;
            if (gUnk_300188E > 60) {
                nextState = 128;
                gUnk_3003BF0 = 0;
            }
            break;

        case 7:
        case 10:
        case 14:
        case 22:
        case 25:
            gUnk_300188E++;
            if (gUnk_300188E > 60) {
                nextState = 128;
                goto set_state;
            }
            break;

        case 17:
        case 21:
        case 28:
            gUnk_300188E++;
            if (gUnk_300188E <= 60) {
                break;
            }
            goto set_31;

        case 8:
        case 15:
        case 23:
            gColorFading.type = 5;
            gColorFading.progress = 0;
            nextState = 128;
            goto set_state;

        case 9:
        case 16:
        case 24:
            transitionComplete = CollapseColorFadeWindowVertically();
            if (transitionComplete != 0) {
                nextState = 128;
                goto set_state;
            }
            break;

        case 4:
        case 19:
            gColorFading.type = 2;
            gColorFading.progress = 0;
            BackupPalettesForColorFade();
            nextState = 128;
            goto set_state;

        case 5:
        case 20:
            if (gMainTimer & 1) {
                goto finish;
            }
            transitionComplete = UpdatePaletteFadeStep(1, 2);
            if (transitionComplete != 0) {
                nextState = 128;
                goto set_state;
            }
            break;

        case 11:
        case 26:
            gBldy = 0;
            gBldCnt = 0xFF;
            nextState = 128;
            goto set_state;

        case 12:
        case 27:
            if (gBldy > 15) {
                nextState = 128;
                goto set_state;
            }
            gBldy++;
            break;

        case 31:
            *(u16*)0x04000004 &= 0xFFEF;
            *(u16*)0x04000200 &= 0xFFFD;
            gSwitchPressed = 0;
            gStageEntryPauseMenuDisabled = 0;
            gUnk_3000025 = 0;
            resultCode = 1;
            break;
    }

    if (nextState == 0) {
        goto finish;
    }

set_state:
    if (nextState == 128) {
        gSpriteAiDropTimer++;
    } else {
        gSpriteAiDropTimer = nextState;
    }
    gUnk_300188E = 0;

finish:
    return resultCode;
}

s32 RunCurrentPassageStageEnd(void)
{
    s32 result;
    StageEndUpdateFunc updateFunction;

    UploadBossColorFadePalettes();
    updateFunction = sStageEndUpdateFunctions[gCurrentPassage];
    result = updateFunction();
    UpdateBossDefeatPaletteFade();
    UpdateGoldenDivaPaletteFade();
    return result;
}

s32 UpdateStageEndSequence(void)
{
    s32 resultCode;
    u32 fadeTimer;
    u32 delayTimer;
    u32 scaleTimer;
    u32 mosaicTimer;
    u32 mosaicLevel;
    u32 finishTimer;
    u8 alphaStep;

    resultCode = 0;
    switch ((u32)gSpriteAiDropTimer) {
        case 0:
            InitStageEndAffine();
            gUnk_3003BF6 = 0xA8;
            gUnk_3003BF5 = 0x88;
            gUnk_300188E = 0;
            gSpriteAiDropTimer += 1;
            break;

        case 1:
            fadeTimer = gUnk_300188E + 1;
            gUnk_300188E = fadeTimer;
            if (fadeTimer & 7) {
                break;
            } else if (gUnk_30000D8.defaultAlpha == 0) {
                alphaStep = 1;
                fadeTimer += alphaStep;
                gUnk_300188E = (u16)alphaStep;
                gUnk_300188E = 0;
                gSpriteAiDropTimer += alphaStep;
                m4aSongNumStart(0x1E3);
            } else {
                gUnk_30000D8.defaultAlpha = (u8)(gUnk_30000D8.defaultAlpha - 1);
                *((u16*)0x04000052) = gUnk_30000D8.defaultAlpha | (0xFF00 & (*((u16*)0x04000052)));
            }
            break;

        case 2:
            delayTimer = gUnk_300188E + 1;
            gUnk_300188E = delayTimer;
            if (((u16)delayTimer) > 0xAU) {
                gUnk_300188E = 0;
                gSpriteAiDropTimer += 1;
            }
            break;

        case 3:
            InitStageEndVBlank();
            gStageEndScaleStep = 2;
            gUnk_300188E = 0;
            gSpriteAiDropTimer++;
            break;

        case 4:
            scaleTimer = gUnk_300188E + 1;
            gUnk_300188E = scaleTimer;
            gStageEndScaleStep = 2;
            if (((u16)scaleTimer) > 0x13U) {
                gUnk_300188E = 0;
                gSpriteAiDropTimer += 1;
                gUnk_3000038 = 1;
            }
            break;

        case 5:
            mosaicTimer = gUnk_300188E + 1;
            gUnk_300188E = mosaicTimer;
            if (((u16)mosaicTimer) > 0xEU) {
                gUnk_300188E = 0;
                gSpriteAiDropTimer += 1;
            }
            if (!(7 & gUnk_300188E)) {
                mosaicLevel = gMosaic + 1;
                gMosaic = mosaicLevel;
                *((s16*)0x0400004C) = mosaicLevel | ((2 * mosaicLevel) * 8);
            }
            break;

        case 6:
            gUnk_300188E += 1;
            gColorFading.type = 2;
            gColorFading.progress = 0;
            BackupPalettesForColorFade();
            gUnk_300188E = 0;
            gSpriteAiDropTimer += 1;
            break;

        case 7:
            if (UpdatePaletteFadeStep(1, 2) != 0) {
                gUnk_300188E = 0;
                gSpriteAiDropTimer += 1;
            }
            break;

        case 8:
            finishTimer = gUnk_300188E + 1;
            gUnk_300188E = finishTimer;
            if (((u16)finishTimer) > 0x28U) {
                gUnk_300188E = 0;
                gSpriteAiDropTimer += 1;
                resultCode = 2;
            }
            break;
    }

    UpdateStageEndBlend();
    UpdateStageEndScale();
    UpdateStageEndAffineTransform();
    return resultCode;
}

void UpdateStageEndAffineTransform(void)
{
    int scaledSine;
    s32 matrixValue;
    s16 trigValue;

    trigValue = sSinCosTable[gStageEndAngle + 0x40];
    gBg2PA = FixedMul(trigValue, (s16)FixedInverse((s16)gStageEndScaleX));

    trigValue = sSinCosTable[gStageEndAngle];
    trigValue = FixedMul(trigValue, (s16)FixedInverse((s16)gStageEndScaleX));
    gBg2PB = trigValue;

    scaledSine = (-gStageEndHorizontalDirection) * ((s16)sSinCosTable[gStageEndAngle]);
    trigValue = scaledSine;
    matrixValue = FixedMul(trigValue, (s16)FixedInverse((s16)gStageEndScaleY));
    gBg2PC = matrixValue;

    trigValue = gStageEndHorizontalDirection * sSinCosTable[gStageEndAngle + 0x40];
    matrixValue = FixedMul(trigValue, (s16)FixedInverse((s16)gStageEndScaleY));
    gBg2PD = matrixValue;

    gBg2X = (((gUnk_3003BF6 - (s16)gStageEndAffineOriginX) << 8) - (gBg2PA * gUnk_3003BF6)) - (gBg2PB * gUnk_3003BF5);

    gBg2Y = (((gUnk_3003BF5 - (s16)gStageEndAffineOriginY) << 8) - (gBg2PC * gUnk_3003BF6)) - (gBg2PD * gUnk_3003BF5);
}

void UpdateStageEndScale(void)
{
    s32 nextScale;

    if (gStageEndScaleStep != 0) {
        nextScale = gStageEndScaleX + gStageEndScaleStep;
        if (nextScale > 0x400) {
            gStageEndScaleX = 0x400;
        } else {
            gStageEndScaleX = (u16)nextScale;
            gStageEndScaleY += gStageEndScaleStep;
        }
        if (gStageEndScaleStep != 0x7F) {
            gStageEndScaleStep = (u8)gStageEndScaleStep + 1;
        }
    }
}

void InitStageEndAffine(void)
{
    gBg2PA = 0x100;
    gBg2PB = 0;
    gBg2PC = 0;
    gBg2PD = 0x100;
    gStageEndScaleX = 0x100;
    gStageEndScaleY = 0x100;
    gStageEndAngle = 0;
    gBg2X = 0;
    gBg2Y = 0;
    gStageEndAffineOriginX = 0;
    gStageEndAffineOriginY = 0;
    gUnk_3003BF1 = 0;
    gUnk_3003BF3 = 0;
    gStageEndScaleStep = 0;
    gUnk_3003BF5 = 0x50;
    gUnk_3003BF6 = 0x78;
    gMosaic = 0;
    gStageEndHorizontalDirection = 1;
}

void StageEndVBlank(void)
{
    vu16* ioRegister;
    s32 affineReference;

    m4aSoundVSync();

    ioRegister = (vu16*)0x040000D4;
    ((vu32*)ioRegister)[0] = (u32)gOamBuffer;
    ((vu32*)ioRegister)[1] = 0x07000000;
    ((vu32*)ioRegister)[2] = 0x84000100;
    ((vu32*)ioRegister)[2];

    *(vu16*)0x04000014 = 0;
    *(vu16*)0x04000016 = 0;

    ioRegister -= 90;
    *ioRegister++ = gBg2PA;
    *ioRegister++ = gBg2PB;
    *ioRegister++ = gBg2PC;
    *ioRegister++ = gBg2PD;

    affineReference = gBg2X;
    *ioRegister++ = (s16)affineReference;
    *ioRegister++ = (s16)((affineReference & 0x0FFF0000) >> 16);

    do {
        /* Keep agbcc's I/O pointer in r1 without register binding. */
        affineReference = gBg2Y;
        *ioRegister++ = (s16)affineReference;
        *ioRegister = (s16)((affineReference & 0x0FFF0000) >> 16);

        ioRegister -= 23;
        *ioRegister = 0x1601;
    } while (0);

    ioRegister += 6;
    *ioRegister = 0x4709;
    ioRegister--;
    *ioRegister = 0x064A;

    SetHBlankMode(0xE);
    *(vu16*)0x05000000 = 0;
}

void InitStageEndVBlank(void)
{
    gDrawWarioOverBackground = 1;
    gUnk_3000046 = 1;
    InterruptCallbackSetVBlank(&StageEndVBlank);
}

void UpdateStageEndBlend(void)
{
    int nextAlpha;
    int blendState;
    int currentAlpha;

    blendState = gUnk_3000038;
    nextAlpha = 2;
    if (blendState == 1) {
        goto initialize_blend;
    }
    if (blendState <= 1) {
        goto finish;
    }
    if (blendState == nextAlpha) {
        goto update_blend;
    }
    goto finish;

initialize_blend:
    gBlendAlphaEvb = 0;

    *((volatile u16*)0x04000052) = 0x10;
    *((volatile u16*)0x04000050) = 0x2F50;
    gUnk_3000038++;
    goto finish;

update_blend:
    currentAlpha = gBlendAlphaEvb;

    if (currentAlpha == 0x10) {
        gUnk_3000038 = 0;
        goto finish;
    }
    if (gMainTimer & 3) {
        goto finish;
    }
    currentAlpha++;
    nextAlpha = currentAlpha;
    gBlendAlphaEvb = nextAlpha;
    *((volatile u16*)0x04000052) = (nextAlpha << 8) | (0x10 - nextAlpha);

finish:
    return;
}

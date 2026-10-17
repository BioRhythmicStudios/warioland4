#include "cutscenes.h"
#include "global_data.h"
#include "interrupt_callbacks.h"
#include "minigame_transition.h"

#define DMA_COPY_32(count) (((DMA_ENABLE | DMA_32BIT) << 16) | (count))

void UpdateAffineCutsceneBackground(u16 frame);
void UpdateCutsceneSequence150(u16 frame);
#ifdef PORTABLE
// Callers cast the frame to u16 themselves.
void UpdateCutsceneBackgroundVariantTransition(s32 frame);
void UpdateLayeredBackgroundTransition(s32 frame);
void UpdateLayeredSpriteCutsceneSequence(s32 frame);
#else
void UpdateCutsceneBackgroundVariantTransition(u16 frame);
void UpdateLayeredBackgroundTransition(u16 frame);
void UpdateLayeredSpriteCutsceneSequence(u16 frame);
#endif
void func_8005500(u16 frame);
void func_80058AC(u16 frame);
#ifdef PORTABLE
void UpdateLayeredSubGameCutscene(s32 frame);
void UpdateLayeredEndingCutsceneSequence(s32 frame);
void UpdateLayeredWarioCutsceneSequence(s32 frame);
void UpdateEndingScrollCutscene(s32 frame);
#else
void UpdateLayeredSubGameCutscene(u16 frame);
void UpdateLayeredEndingCutsceneSequence(u16 frame);
void UpdateLayeredWarioCutsceneSequence(u16 frame);
void UpdateEndingScrollCutscene(u16 frame);
#endif
void BuildCutsceneBackgroundAffineMatrix(
    u16* affineRegisters,
    u16 rotation,
    u16 scaleX,
    u16 scaleY,
    s32 centerX,
    s32 centerY
);

void ResetPrimaryCutsceneAffineState(void)
{
    gUnk_3002C28 = 0;
    gUnk_3002C2A = 0;
    gUnk_3002C2C = 0;
    gUnk_3002C2E = 0x100;
    gUnk_3002C30 = 0x100;
    gUnk_3002C42 = 0;
    gUnk_3002C44 = 0;
    gCutsceneBackgroundHorizontalOffset = 0;
    gCutsceneBackgroundVerticalOffset = 0;
    gUnk_3002C5A = 8;
    gUnk_3002C5C = 8;
    gUnk_3002C5E = 0;
}

void ResetSecondaryCutsceneAffineState(void)
{
    gCutscenePrimaryObjectX = 0;
    gCutscenePrimaryObjectY = 0;
    gUnk_3002C4E = 0;
    gUnk_3002C50 = 0x100;
    gUnk_3002C52 = 0x100;
    gUnk_3002C54 = 0;
    gUnk_3002C56 = 0xC;
    gUnk_3002C58 = 0;
}

u32 UpdateCutsceneSequence(void)
{
    s32 restartFromBeginning;
    s32 transitionResult;
    s16 initialMode;
    s16 modeAfterInitialization;

    transitionResult = 0;
    restartFromBeginning = 0;
    initialMode = gSubGameMode;

    if (initialMode == -1) {
        restartFromBeginning = 1;
        gSubGameMode = 0;
    } else if (initialMode == -2) {
        gSubGameMode = 19;
    } else if (initialMode == -3) {
        gSubGameMode = 26;
    } else if (initialMode == -4) {
        gSubGameMode = 35;
    }

    switch ((u32)gSubGameMode) {
        case 0:
            REG_IE &= INTR_FLAG_GAMEPAK;
            InterruptCallbackSetVBlank(CutsceneVBlankCallback);
            REG_IE |= INTR_FLAG_VBLANK;
            CountEndingTreasuresAndSelectTier();
            SubGameInitAndDispatch(0);
            gUnk_3002C74 = 0;
            gUnk_3002C78 = gLanguage;
            gUnk_3002C7C = 0;
            /* This read keeps agbcc's mode pointer live across the branch. */
            modeAfterInitialization = gSubGameMode;
            if (restartFromBeginning != 0) {
                gSubGameMode = 0x2E;
            } else {
                gSubGameMode = 2;
            }
            break;

        case 1:
            SubGameWaitAndAdvance((u16)(gUnk_300188E++));
            break;

        case 2:
            SubGameInitAndDispatch(2);
            break;

        case 3:
            UpdateAffineCutsceneBackground((u16)(gUnk_300188E++));
            break;

        case 6:
            SubGameInitAndDispatch(6);
            break;

        case 7:
            UpdateCutsceneDisplayTimeline((u16)(gUnk_300188E++));
            break;

        case 8:
            SubGameInitAndDispatch(8);
            break;

        case 9:
            UpdateCutsceneBackgroundVariantTransition((u16)(gUnk_300188E++));
            break;

        case 14:
            SubGameInitAndDispatch(0xE);
            break;

        case 15:
            func_8005500((u16)(gUnk_300188E++));
            break;

        case 16:
            SubGameInitAndDispatch(0x10);
            break;

        case 17:
            UpdateLayeredSubGameCutscene((u16)(gUnk_300188E++));
            break;

        case 18:
            SubGameClearGraphicsMemory();
            transitionResult = 1;
            break;

        case 19:
            REG_IE &= INTR_FLAG_GAMEPAK;
            InterruptCallbackSetVBlank(CutsceneVBlankCallback);
            REG_IE |= INTR_FLAG_VBLANK;
            SubGameClearGraphicsMemory();
            SubGameInitAndDispatch(0x13);
            break;

        case 20:
            UpdateLayeredEndingCutsceneSequence((u16)(gUnk_300188E++));
            break;

        case 21:
            SubGameInitAndDispatch(0x15);
            break;

        case 22:
            UpdateLayeredWarioCutsceneSequence((u16)(gUnk_300188E++));
            break;

        case 23:
            SubGameInitAndDispatch(0x17);
            break;

        case 24:
            UpdateAffineWarioCutscene((u16)(gUnk_300188E++));
            break;

        case 25:
            SubGameClearGraphicsMemory();
            transitionResult = 2;
            break;

        case 26:
            REG_IE &= INTR_FLAG_GAMEPAK;
            InterruptCallbackSetVBlank(CutsceneVBlankCallback);
            REG_IE |= INTR_FLAG_VBLANK;
            gUnk_3002C78 = gLanguage;
            CountEndingTreasuresAndSelectTier();
            SubGameClearGraphicsMemory();
            SubGameInitAndDispatch(0x1A);
            break;

        case 27:
            UpdateLayeredEndingTreasureCutsceneSequence((u16)(gUnk_300188E++));
            break;

        case 28:
            SubGameInitAndDispatch(0x1C);
            break;

        case 29:
            func_8008B20((u16)(gUnk_300188E++));
            break;

        case 30:
            SubGameInitAndDispatch(0x1E);
            break;

        case 31:
            UpdateEndingScrollCutscene((u16)(gUnk_300188E++));
            break;

        case 32:
            SubGameInitAndDispatch(0x20);
            break;

        case 33:
            func_800A43C((u16)(gUnk_300188E++));
            break;

        case 34:
            SubGameClearGraphicsMemory();
            transitionResult = 3;
            break;

        case 35:
            REG_IE &= INTR_FLAG_GAMEPAK;
            InterruptCallbackSetVBlank(CutsceneVBlankCallback);
            REG_IE |= INTR_FLAG_VBLANK;
            gUnk_3002C78 = gLanguage;
            gUnk_3002C7C = 1;
            CountEndingTreasuresAndSelectTier();
            SubGameClearGraphicsMemory();
            /* fallthrough */
        case 4:
            SubGameInitAndDispatch(4);
            break;

        case 5:
        case 36:
            UpdateCutsceneSequence150((u16)(gUnk_300188E++));
            break;

        case 10:
        case 37:
            SubGameInitAndDispatch(0xA);
            break;

        case 11:
        case 38:
            UpdateLayeredBackgroundTransition((u16)(gUnk_300188E++));
            break;

        case 12:
        case 39:
            SubGameInitAndDispatch(0xC);
            break;

        case 13:
        case 40:
            UpdateLayeredSpriteCutsceneSequence((u16)(gUnk_300188E++));
            break;

        case 41:
            SubGameInitAndDispatch(0x29);
            break;

        case 42:
            func_80058AC((u16)(gUnk_300188E++));
            break;

        case 43:
            SubGameInitAndDispatch(0x2B);
            break;

        case 44:
            UpdateTitleScreenLogoCutscene((u16)(gUnk_300188E++));
            break;

        case 45:
            gDisableSoftReset = 0;
            SubGameClearGraphicsMemory();
            transitionResult = 4;
            break;

        case 46:
            SubGameInitAndDispatch(0x2E);
            break;
    }

    BuildCutsceneBackgroundAffineMatrix(
        gUnk_3002C32, gUnk_3002C2C, gUnk_3002C2E, gUnk_3002C30, 0x78 - gUnk_3002C28, 0x50 - gUnk_3002C2A
    );

    if (((u16)(gSubGameMode - 3) <= 0xD) && ((gButtonsPressed & (A_BUTTON | START_BUTTON)) != 0)) {
        m4aSongNumStartOrChange(0x124);
        gSubGameMode = 0x2E;
    }

    return transitionResult;
}

void CutsceneVBlankCallback(void)
{
    vu16* affineRegisters;
    vu16* ioRegister;
    u32 registerValue;
    u32 oamByteSize;

    m4aSoundVSync();

    oamByteSize = gUnk_3002C6C;
    if (oamByteSize != 0) {
        ioRegister = (vu16*)REG_ADDR_DMA3SAD;
        ((vu32*)ioRegister)[0] = (u32)gOamBuffer;
        ((vu32*)ioRegister)[1] = OAM;
        ((vu32*)ioRegister)[2] = DMA_COPY_32(oamByteSize >> 2);
        ((vu32*)ioRegister)[2];
    }

    affineRegisters = gUnk_3002C32;
    ioRegister = (vu16*)REG_ADDR_BG2PA;

    registerValue = *affineRegisters;
    *ioRegister = registerValue;
    affineRegisters++;
    ioRegister++;
    registerValue = *affineRegisters;
    *ioRegister = registerValue;
    affineRegisters++;
    ioRegister++;
    registerValue = *affineRegisters;
    *ioRegister = registerValue;
    affineRegisters++;
    ioRegister++;
    registerValue = *affineRegisters;
    *ioRegister = registerValue;
    affineRegisters++;
    ioRegister++;
    registerValue = *affineRegisters;
    *ioRegister = registerValue;
    affineRegisters++;
    ioRegister++;
    registerValue = *affineRegisters;
    *ioRegister = registerValue;
    affineRegisters++;
    ioRegister++;
    registerValue = *affineRegisters;
    *ioRegister = registerValue;
    REG_BG2Y_H = affineRegisters[1];

    if (gSubGameMode == 0x16) {
        CutsceneWarioLoadObjTiles();
    }

    m4aSoundMain();
}

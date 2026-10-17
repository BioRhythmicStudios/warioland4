#include "minigame_transition.h"
#include "cutscenes.h"
#include "global_data.h"

#define SUBGAME_MODE_DELAYED_RETURN 46
#define SUBGAME_MODE_RETURN_TARGET 17
#define SUBGAME_INIT_DELAY_FRAMES 0x15
#define DMA_FILL_32(count) (((DMA_ENABLE | DMA_32BIT | DMA_SRC_FIXED) << 16) | (count))

void SubGameInitAndDispatch(u32 subGameMode)
{
    u32 fillValue;

    WaitForVBlankInterrupt(subGameMode);
    REG_DISPCNT = 0;
    ResetCutsceneBackgroundRegisters();

    gUnk_3002C6C = 0;
    GP_SET(gUnk_3002C70, gOamBuffer);

    fillValue = 0xA0;
    {
        vu32* dma3;

        dma3 = (vu32*)REG_ADDR_DMA3SAD;
        dma3[0] = (u32)&fillValue;
        dma3[1] = OAM;
        dma3[2] = DMA_FILL_32(0x100);
        dma3[2];
    }

    ResetPrimaryCutsceneAffineState();
    ResetSecondaryCutsceneAffineState();

    gUnk_300188E = 0;
    gUnk_3002C60 = 0;
    gUnk_3002C64 = 0;

    switch (subGameMode) {
        case 0:
            SubGameClearGraphicsMemory();
            break;
        case 2:
            InitializeAffineCutsceneBackground();
            break;
        case 4:
            InitializeVariantCutsceneBackground();
            break;
        case 6:
            InitializeCutsceneBackgroundGraphics();
            break;
        case 8:
            InitializeLayeredScrollCutscene();
            break;
        case 10:
            InitializeWindowedCutsceneBackgrounds();
            break;
        case 12:
            InitializeScaledSpriteCutscene();
            break;
        case 14:
            InitializeThreeLayerCutsceneBackground();
            break;
        case 16:
            InitializeSubGameTransitionGraphics();
            break;
        case 19:
            func_8006680();
            break;
        case 21:
            InitializeLayeredWarioCutscene();
            break;
        case 23:
            InitializeAffineWarioCutscene();
            break;
        case 26:
            InitializeLayeredObjectCutscene();
            break;
        case 28:
            InitializeEndingTreasureCutscene();
            break;
        case 30:
            InitializeEndingBackgroundSetup();
            break;
        case 32:
            InitializeEndingCutscene();
            break;
        case 43:
            InitializeTitleScreenLogoCutscene();
            break;
        case 41:
            InitializeThreeLayerCutsceneBackgroundWithSprites();
            break;
        case SUBGAME_MODE_DELAYED_RETURN:
            gUnk_3002C74 = 1;
            InitializeSubGameTransitionGraphics();
            break;
    }

    if (subGameMode == SUBGAME_MODE_DELAYED_RETURN) {
        gSubGameMode = SUBGAME_MODE_RETURN_TARGET;
    } else {
        gSubGameMode++;
    }
}

void SubGameClearGraphicsMemory(void)
{
    u32 fillValue;

    fillValue = 0;
    {
        vu32* dma3;

        dma3 = (vu32*)REG_ADDR_DMA3SAD;
        dma3[0] = (u32)&fillValue;
        dma3[1] = BG_PLTT;
        dma3[2] = DMA_FILL_32(0x100);
        dma3[2];

        fillValue = -1;
        dma3[0] = (u32)&fillValue;
        dma3[1] = VRAM + 0x7F80;
        dma3[2] = DMA_FILL_32(0x10);
        dma3[2];

        fillValue = 0;
        dma3[0] = (u32)&fillValue;
        dma3[1] = VRAM + 0x7FC0;
        dma3[2] = DMA_FILL_32(0x10);
        dma3[2];

        fillValue = 0x03FF03FF;
        dma3[0] = (u32)&fillValue;
        dma3[1] = VRAM + 0x8000;
        dma3[2] = DMA_FILL_32(0x1000);
        dma3[2];
    }

    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_DISPCNT = 0;
}

void SubGameWaitAndAdvance(u16 frame UNUSED)
{
    s32 frameCounter;

    frameCounter = gUnk_3002C64 + 1;
    gUnk_3002C64 = frameCounter;
    if (frameCounter == SUBGAME_INIT_DELAY_FRAMES) {
        gSubGameMode = (u16)gSubGameMode + 1;
    }
}

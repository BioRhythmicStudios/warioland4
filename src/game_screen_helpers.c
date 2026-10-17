#include "game_screen_helpers.h"

#include "animated_graphics.h"
#include "background_registers.h"
#include "color_effects.h"
#include "demo.h"
#include "global_data.h"
#include "hblank.h"
#include "main.h"
#include "score.h"
#include "sound.h"
#include "transparency.h"
#include "wario.h"

#include "gba/m4a.h"

/* Neighboring modules still use these original ABI symbols on the main base. */
void TransparencyProcessTiles();
void UpdateCamera();
void RefreshCollectedTileEffects();
void InitAnimatedGraphics();
void ApplyRoomTransitionTileOffset();
void SetRoomTransitionTileValues();
void func_8000D18();
void StopDemo();
void LoadCurrentRoomCameraControlData();
void TransparencyProcessWater();
void UpdateTileEffect();
void UpdateAnimatedGraphics();
void UpdateWarioTileInteractions();
void m4aSongNumStartOrContinue();
void m4aMPlayFadeOutTemporarily();

#define SE_HURRY_UP_SWITCH 0xD9

struct AGB_ALIGN DmaRegisters {
    GBA_PTR(const void*) source;
    GBA_PTR(void*) destination;
    vu32 control;
};

void ConfigureRoomDisplay(void)
{
    u16 backgroundControl[4];
    struct Window* windowState;
    u16* backgroundControlPointer;
    int blendControlTen;
    u8 blendControl;
    u8 blendLevel;
    s32 position;
    int defaultWindowLeft;
    int backgroundY;
    struct Window* windowPointer;
    {
        u8* drawFlag;
        u16 zero;
        drawFlag = &gDrawWarioOverBackground;
        zero = 0;
        backgroundControlPointer = &backgroundControl[3];
        do {
            *backgroundControlPointer = zero;
            backgroundControlPointer--;
        } while ((s32)backgroundControlPointer >= (s32)backgroundControl);
        *drawFlag = 0;
    }
    if (gCurrentRoomHeader.bg0Param & 0x20) {
        backgroundControl[0] = GetBackgroundScreenSize(*GP(gCurrentRoomHeader.pBg0Data)) | 8;
    } else {
        backgroundControl[0] = 0x4004;
    }
    backgroundControl[1] = 0x4204;
    defaultWindowLeft = 0;
    backgroundControl[2] = 0x4404;
    backgroundControl[3] = 0x0603;
    if (gCurrentRoomHeader.bg3Param == 0x20) {
        backgroundControl[3] |= 8;
    } else {
        backgroundControl[3] |= 4;
    }
    backgroundControl[3] |= GetBackgroundScreenSize(*GP(gCurrentRoomHeader.pBg3Data));
    switch (gCurrentRoomHeader.bgPriorityAlpha) {
        case 0:

        case 1:

        case 2:

        case 3:

        case 4:

        default:
            backgroundControl[0] = backgroundControl[0];
            backgroundControl[1] |= 1;
            backgroundControl[2] |= 2;
            break;

        case 5:

        case 9:

        case 13:

        case 17:

        case 21:

        case 25:

        case 29:

        case 33:

        case 37:

        case 41:

        case 45:

        case 49:
            backgroundControl[0] |= 1;
            backgroundControl[1] = backgroundControl[1];
            backgroundControl[2] |= 2;
            break;

        case 6:

        case 10:

        case 14:

        case 18:

        case 22:

        case 26:

        case 30:

        case 34:

        case 38:

        case 42:

        case 46:

        case 50:
            backgroundControl[0] |= 1;
            backgroundControl[1] = backgroundControl[1];
            backgroundControl[2] |= 2;
            gDrawWarioOverBackground = 1;
            break;

        case 7:

        case 11:

        case 15:

        case 19:

        case 23:

        case 27:

        case 31:

        case 35:

        case 39:

        case 43:

        case 47:

        case 51:
            backgroundControl[0] |= 2;
            backgroundControl[1] = backgroundControl[1];
            backgroundControl[2] |= 1;
            gDrawWarioOverBackground = 1;
            break;
    }

    blendControl = 0;
    blendLevel = 0;
    blendControlTen = 10;
    switch (gCurrentRoomHeader.bgPriorityAlpha) {
        case 24:
        case 25:
        case 26:
        case 27:
            blendLevel = 0;
            break;

        case 28:
        case 29:
        case 30:
        case 31:
            blendLevel = 3;
            break;

        case 32:
        case 33:
        case 34:
        case 35:
            blendLevel = 6;
            break;

        case 36:
        case 37:
        case 38:
        case 39:
            blendLevel = 9;
            break;

        case 40:
        case 41:
        case 42:
        case 43:
            blendLevel = 11;
            break;

        case 44:
        case 45:
        case 46:
        case 47:
            blendLevel = 13;
            break;

        case 48:
        case 49:
        case 50:
        case 51:
            blendLevel = 16;
            break;

        case 8:
        case 9:
        case 10:
        case 11:
            blendControl = 7;
            break;

        case 12:
        case 13:
        case 14:
        case 15:
            blendControl = blendControlTen;
            break;

        case 16:
        case 17:
        case 18:
        case 19:
            blendControl = 13;
            break;

        case 20:
        case 21:
        case 22:
        case 23:
            blendControl = 16;
            break;
    }

    if (blendControl == 0) {
        if ((gPauseFlag == 0) && (gHasTemporarySave == 0)) {
            gUnk_30000D8.blendTimer = 0;
            gUnk_30000D8.targetAlpha = blendLevel;
            gUnk_30000D8.currentAlpha = blendLevel;
            gUnk_30000D8.defaultAlpha = blendLevel;
            if (gStageEntryPauseMenuDisabled != 0) {
                TransparencyProcessTiles(0);
                gUnk_30000D8.currentAlpha = gUnk_30000D8.targetAlpha;
            }
        }
        *((vu16*)0x04000052) = (gUnk_30000D8.currentAlpha << 8) | (16 - gUnk_30000D8.currentAlpha);
        gBldAlpha = 0;
    } else {
        gUnk_30000D8.defaultAlpha = blendControl;
        *((vu16*)0x04000052) = 0x1000 | blendControl;
        gBldAlpha = 0;
    }
    if (gColorFading.type == 5) {
        gWindow.left = (position = (gWarioData.xPosition - gBg1XPosition) >> 2);
        if (((u8)position) > 240) {
            if (gWarioData.xPosition < gBg1XPosition) {
                gWindow.left = 0;
            } else {
                gWindow.left = 240;
            }
        }
        {
            register s32 yPosition PIN("r1");
            yPosition = gWarioData.yPosition;
            backgroundY = gBg1YPosition + 63;
            yPosition = (yPosition - backgroundY) >> 2;
            gWindow.top = yPosition;
            if (((u8)yPosition) > 160) {
                if (((s32)(gWarioData.yPosition - 63)) < gBg1YPosition) {
                    gWindow.top = 0;
                } else {
                    gWindow.top = 160;
                }
            }
        }
        gWindow.right = gWindow.left;
        gWindow.bottom = gWindow.top;
        gWindow.content = 0;
    } else {
        struct Window* window;
        int zero;
        window = &gWindow;
        zero = 0;
        window->left = zero;
        window->right = 240;
        window->top = zero;
        window->bottom = 160;
        window->content = zero;
    }
    *((vu8*)0x0400004A) = 32;
    *((vu8*)0x04000049) = 63;
    windowPointer = &gWindow;
    windowState = &(*windowPointer);
    *((vu16*)0x04000042) = ((*windowState).left << 8) | (*windowState).right;
    *((vu16*)0x04000046) = ((*windowState).top << 8) | gWindow.bottom;
    *((vu8*)0x04000048) = 0;
    *((vu16*)0x04000040) = 0;
    *((vu16*)0x04000044) = 0;
    *((vu16*)0x04000008) = backgroundControl[0];
    *((vu16*)0x0400000A) = backgroundControl[1];
    *((vu16*)0x0400000C) = backgroundControl[2];
    *((vu16*)0x0400000E) = backgroundControl[3];
    gMosaic = 0;
    {
        vu16* mosaicRegister;
        int hwZero;
        mosaicRegister = (vu16*)0x0400004C;
        hwZero = 0;
        *mosaicRegister = hwZero;
    }
}

u32 GetBackgroundScreenSize(u8 arg0)
{
    u32 result;

    result = 0;
    switch (arg0) {
        case 0:
            break;
        case 1:
            result = 0x4000;
            break;
        default:
            result = 0x4000;
            break;
        case 2:
            result = 0x8000;
            break;
    }
    return result;
}

void LoadRoom(void)
{
    struct RoomHeader* roomHeader;
    volatile struct DmaRegisters* dma;
    const u8* blankTiles;
    u32 dmaFillControl;
    u32 dmaEnable;
    struct TilesetLoadData tileset;
    const struct RoomHeader* roomTable;
    GBA_TYPEOF(const struct RoomHeader* const*, &(sRoomHeaderTables)[0]) roomTableBase;
    GBA_PTR(const struct RoomHeader*) const * roomTablePointer;
    const void* selectedData;
    u32 roomIndex;
    register u16 tileAttribute PIN("r3");
    register const u16* tileAttributes PIN("r1");
    register const u16* tilePointer PIN("r0");

    InitializeRoomState();
    roomHeader = &gCurrentRoomHeader;
    roomTableBase = sRoomHeaderTables;
    roomTablePointer = (GBA_TYPEOF(const struct RoomHeader* const*, roomTablePointer))(gStageRoomTableIndex * 4);
    roomTablePointer = (GBA_TYPEOF(const struct RoomHeader* const*, roomTablePointer))((u32)roomTablePointer + (u32)roomTableBase);
    roomIndex = gCurrentRoom;
    roomTable = GP(*roomTablePointer);
    *roomHeader = roomTable[roomIndex];
    tileset = sTilesetLoadData[roomHeader->tileset];
    UpdateBossRoomState();
    if (gDifficulty == DIFFICULTY_NORMAL) {
        GP_SET(roomHeader->pHardSpriteData, GP(roomHeader->pNormalSpriteData));
    } else if (gDifficulty == DIFFICULTY_SUPER_HARD) {
        GP_SET(roomHeader->pHardSpriteData, GP(roomHeader->pSHardSpriteData));
    }
    if (gUnk_300001C != 0) {
        struct RoomHeader* musicRoomHeader;

        musicRoomHeader = &gCurrentRoomHeader;
        musicRoomHeader->musicVolume = 0;
    }
    GP_SET(gBackgroundTileTables.top, GP(tileset.metatileTop));
    GP_SET(gBackgroundTileTables.bottom, GP(tileset.metatileBottom));
    GP_SET(gBackgroundTileTables.attributes, GP(tileset.metatileAttributes));
    InitializeRoomEffects();
    if (gStageEntryPauseMenuDisabled == 0 || gHasTemporarySave != 0) {
        PreloadStageRoomBackgrounds();
    }

    dma = (struct DmaRegisters*)0x040000D4;
    GP_SET(dma->source, GP(tileset.backgroundGraphics));
    GP_SET(dma->destination, (void*)0x06004820);
    dma->control = (tileset.backgroundGraphicsSize >> 1) | (dmaEnable = 0x80000000);
    WaitForDma3Transfer(dma->control);
    blankTiles = sBlankBackgroundTiles;
    GP_SET(dma->source, blankTiles);
    GP_SET(dma->destination, (void*)0x06004800);
    dmaFillControl = 0x80000010;
    dma->control = dmaFillControl;
    dma->control;
    GP_SET(dma->source, GP(tileset.palette));
    GP_SET(dma->destination, (void*)0x05000000);
    dma->control = 0x80000100;
    WaitForDma3Transfer(dma->control);
    if (gCurrentStageNumber != STAGE_BOSS) {
        GP_SET(dma->source, GP(tileset.extraPalette));
        GP_SET(dma->destination, (void*)0x05000260);
        dma->control = dmaFillControl;
        dma->control;
    }
    gHBlankPaletteFadeColors[0] = *(const u16*)GP(tileset.palette);
    *(vu16*)0x05000000 = 0;
    gHBlankPaletteFadeColors[1] = 0;
    GP_SET(dma->source, GP(tileset.animatedGraphics));
    GP_SET(dma->destination, (void*)(0x0600FFE0 - tileset.animatedGraphicsSize));
    dma->control = dmaEnable | (tileset.animatedGraphicsSize >> 1);
    WaitForDma3Transfer(dma->control);
    GP_SET(dma->source, blankTiles);
    GP_SET(dma->destination, (void*)0x0600FFE0);
    dma->control = dmaFillControl;
    dma->control;

    LoadRoomBackgrounds();
    RestoreTemporarySaveRoomTiles();
    gPreviousXPosition = gWarioData.xPosition;
    gPreviousYPosition = gWarioData.yPosition;
    if (gPauseFlag == 0 && gHasTemporarySave == 0) {
        UpdateCamera();
        gBg1YPosition = gCameraPositionState.y;
        gBg1XPosition = gCameraPositionState.x;
        UpdateCamera();
    }
    ConfigureRoomDisplay();
    UpdateBackgroundScrollRegisters();
    DrawRoomBackgroundLayer(0);
    DrawRoomBackgroundLayer(1);
    DrawRoomBackgroundLayer(2);
    if (gUnk_300001B == 1) {
        if (gCurrentPassage == PASSAGE_GOLDEN) {
            LZ77UnCompVram(sGoldenBossRoomOverlayTiles, (void*)0x06008000);
            selectedData = sGoldenBossRoomOverlayTilemap;
        } else {
            LZ77UnCompVram(sBossRoomOverlayTiles, (void*)0x06008000);
            selectedData = sBossRoomOverlayTilemap;
        }
        {
            register s32 transferSize PIN("r3");
            volatile struct DmaRegisters* overlayDma;
            u8* overlayBuffer;
            register s32 halfSize PIN("r0");
            register u32 overlayDmaEnable PIN("r1");

            overlayBuffer = (u8*)0x0201F840;
            ASM_BARRIER_VOLATILE(: "+r"(overlayBuffer));
            transferSize = DecompressRoomBackground(0, selectedData, overlayBuffer);
            ASM_BARRIER(: "+r"(transferSize));
            overlayDma = (struct DmaRegisters*)0x040000D4;
            GP_SET(overlayDma->source, (void*)0x0201F840);
            GP_SET(overlayDma->destination, (void*)0x06003800);
            halfSize = transferSize / 2;
            overlayDmaEnable = 0x80000000;
            overlayDma->control = halfSize | overlayDmaEnable;
            overlayDma->control;
        }
    }
    if (gUnk_300001B != 0) {
        DecompressRoomBackground(0, sBossRoomBackgroundData, (u8*)0x0201F040);
    }
    RefreshCollectedTileEffects();
    InitAnimatedGraphics();
    InitRoomHBlank();
    UpdateHBlankEffect();
    if (gWarioData.unk_02 != 0) {
        if (gWarioData.pose == 17) {
            gWarioData.yPosition += 0x80;
        } else if (gWarioData.pose == 18) {
            gWarioData.yPosition -= 0x80;
        }
        gPreviousYPosition = gWarioData.yPosition;
    }
    tileAttribute = *(const u16*)GP(gBackgroundInfo.pBg1Data);
    tileAttributes = GP(gBackgroundTileTables.attributes);
    tilePointer = (const u16*)((tileAttribute * 2) + (u32)tileAttributes);
    tileAttribute = *tilePointer;
    if (tileAttribute == 0xA0) {
        gUnk_3000034 = 1;
    } else {
        gUnk_3000034 = 0;
    }
}

void LoadRoomBackgrounds(void)
{
    struct DmaRegisters* dma;
    struct BackgroundInfo* backgroundInfo;
    u32 dmaEnable;
    u8* buffer;
    u32 size;
    s32 halfSize;
    u32 roomDataBase;
    const u8* data;
    const u8* source;

    source = (const u8*)GP(gCurrentRoomHeader.pBg3Data);
    buffer = (u8*)0x0201F040;
    size = DecompressRoomBackground(0, source, buffer);
    dma = (struct DmaRegisters*)0x040000D4;
    GP_SET(dma->source, buffer);
    GP_SET(dma->destination, (void*)0x06003000);
    halfSize = (s32)size / 2;
    dmaEnable = 0x80000000;
    dma->control = dmaEnable | halfSize;
    dma->control;

    roomDataBase = 0x02000000;
    data = GP(*(GBA_PTR(const u8*) *)(roomDataBase + gCurrentRoom * 4));
    backgroundInfo = &gBackgroundInfo;
    backgroundInfo->bg1Width = *(const u16*)data;
    data += 2;
    backgroundInfo->bg1Height = *(const u16*)data;
    data += 2;
    GP_SET(backgroundInfo->pBg1Data, (void*)data);

    if (gCurrentRoomHeader.bg2Param & 0x10) {
        u8* destination;

        source = (const u8*)GP(gCurrentRoomHeader.pBg2Data);
        destination = (u8*)0x0201A040;
        GP_SET(backgroundInfo->pBg2Data, (void*)destination);
        size = DecompressRoomBackground(1, source, destination);
        backgroundInfo->bg2Width = (s32)size >> 16;
        backgroundInfo->bg2Height = size & 0xFF;
    }

    if (gCurrentRoomHeader.bg0Param & 0x10) {
        u8* destination;

        source = (const u8*)GP(gCurrentRoomHeader.pBg0Data);
        destination = (u8*)0x0201C840;
        GP_SET(backgroundInfo->pBg0Data, (void*)destination);
        size = DecompressRoomBackground(1, source, destination);
        backgroundInfo->bg0Width = (s32)size >> 16;
        backgroundInfo->bg0Height = size & 0xFF;
    } else if (gCurrentRoomHeader.bg0Param & 0x20) {
        source = (const u8*)GP(gCurrentRoomHeader.pBg0Data);
        buffer = (u8*)0x0201C840;
        size = DecompressRoomBackground(0, source, buffer);
        GP_SET(dma->source, buffer);
        GP_SET(dma->destination, (void*)0x06000000);
        dma->control = dmaEnable | ((s32)size / 2);
        dma->control;
    }
}

void RestoreTemporarySaveRoomTiles(void)
{
    u32 base;
    u16* tileIndex;
    u32 zero;
    u16* count;
    s32 index;

    if (gHasTemporarySave != 0) {
        count = &gClearedRoomTileCount;
        if (*count != 0) {
            index = 0;
            if (index != *count) {
                base = 0x02000000;
                zero = 0;
                tileIndex = (u16*)0x0203A000;
                do {
                    *(vu16*)(base + (*tileIndex * 2)) = zero;
                    tileIndex++;
                    index++;
                } while (index != *count);
            }
        }
        ApplyRoomTransitionTileOffset(1);
        SetRoomTransitionTileValues(1);
    }
}

void InitializeRoomState(void)
{
    register struct ColorFading* colorBase PIN("r2");
    register struct ColorFading* colorFading PIN("r5");
    s8* pauseFlag;
    s32 initialize;
    const struct RoomStartData* roomData;
    s32 switchState;
    s32 switchStateStart;
    u32 roomIndex;

    gSpriteAiDropTimer = 0;
    gStageExitType = 0;
    gUnk_300001A = 0;
    colorBase = &gColorFading;
    colorBase->timer = 0;
    colorBase->progress = 0;
    colorBase->sourceFlags = 0;
    colorBase->uploadFlags = 0;
    colorBase->interpolationPending = 0;
    gShopItemState = 0;
    gBldCnt = 0;
    gScrollingUpdateFrameCounter = 0;
    initialize = gStageEntryPauseMenuDisabled;
    colorFading = colorBase;
    pauseFlag = &gPauseFlag;

    if (initialize == 0) {
        colorFading->type = 2;
        gGameMusicState.state = 0;
        gGameMusicState.memoryAccessState = 0;
        gGameMusicState.previousSongId = 0;
        gGameMusicState.playerId = 0;
        gEscapeHBlankEffectState = 0;
        gBossDefeatTimer[0] = 0;
        gBossDefeatTimer[1] = 0;
        gBossDefeatTimer[2] = 0;
        gBossDefeatTimer[3] = 0;
        gBossDefeatTimer[4] = 0;
        gBossDefeatTimer[5] = 0;
        gGoldenDivaRoomTimer[0] = 0;
        gGoldenDivaRoomTimer[1] = 0;
        gGoldenDivaRoomTimer[2] = 0;
        gGoldenDivaRoomTimer[3] = 0;
        gGoldenDivaRoomTimer[4] = 0;
        gGoldenDivaRoomTimer[5] = 0;
        gSpriteTileInteractionMode = 0;
        if (gHasTemporarySave == 0) {
            if ((gUnk_3000020 & 0x80) == 0) {
                gUnk_3000025 = 0;
            }
            gClearedRoomTileCount = 0;
            gUnk_300003A = 0;
            gGoldenBossTransitionBg0Y = 0;
            {
                register u32 switchZero PIN("r3");

                switchStateStart = (s32)gSwitchStates;
                switchZero = 0;
                switchState = switchStateStart + 4;
                do {
                    *(u8*)switchState = switchZero;
                    switchState--;
                } while (switchState >= switchStateStart);
            }
        }
    }

    if (*pauseFlag != 0) {
        return;
    }

    gUnk_3000046 = 0;
    roomData = GP(sRoomStartDataTables[gStageRoomTableIndex]);
    roomIndex = gUnk_3000025;
    roomData += roomIndex;
    gCurrentRoom = roomData->room;
    gUnk_3000027 = roomData->flags;
    gUnk_3000026 = roomData->type;
    if (roomData->type == 5 && gHasTemporarySave == 0) {
        colorFading->type = 5;
    }
    if (gHasTemporarySave != 0) {
        func_8000D18();
        return;
    }

    gCameraPositionState.x = 0;
    gCameraPositionState.y = 0;
    gCameraPositionState.xOffset = 0;
    gCameraPositionState.yOffset = 0;

    {
        u16* tile;
        register u32 index PIN("r1");
        u8* stageFlags;
        register struct WarioData* wario PIN("r4");
        u16 zero;
        register u16 nextIndex PIN("r0");
        u16 music;
        u32 xPosition;
        u32 yOffset;
        u32 yPosition;
        u32 pose;

        tile = (u16*)0x02039F00;
        index = 0;
        stageFlags = &gUnk_3000020;
        wario = &gWarioData;
        zero = 0;
        do {
            *tile++ = zero;
            nextIndex = index + 1;
            index = nextIndex;
        } while (index <= 63);

        music = roomData->music;
        if (music != 0) {
            gGameMusicState.songId = music;
        }

        if (roomData->type == 3) {
            xPosition = roomData->x + 1;
            yOffset = roomData->y + 1;
            wario->xPosition = xPosition << 6;
            yPosition = yOffset << 6;
            wario->yPosition = yPosition - 1;
            pose = wario->pose;
            if (pose == 17) {
                wario->yPosition = yPosition - 65;
            } else if (pose == 18) {
                wario->yPosition = yPosition + 127;
            }
        } else {
            xPosition = roomData->x;
            yOffset = roomData->y + 1;
            xPosition <<= 6;
            xPosition += (roomData->xOffset + 8) << 2;
            wario->xPosition = xPosition;
            yPosition = yOffset << 6;
            yPosition += roomData->yOffset << 2;
            yPosition--;
            wario->yPosition = yPosition;
        }

        if (*stageFlags & 0x80) {
            StopDemo();
        }
    }

    if (gDemoState != 0) {
        gMainTimer = 0;
    }
    gScreenShakeY.duration = 0;
    gScreenShakeY.frameTimer = 0;
    gScreenShakeY.amplitude = 0;
    gScreenShakeX.duration = 0;
    gScreenShakeX.frameTimer = 0;
    gScreenShakeX.amplitude = 0;
    gBg1YPosition = 0;
    gBg1XPosition = 0;
    gBg0XPosition = 0;
    gBg0YPosition = 0;
    gRoomTransitionTileDataCount = 0;
    gUnk_3000038 = 0;
}

void InitializeRoomEffects(void)
{
    struct RoomHeader* roomHeader;
    register u32 zeroType PIN("r4");

    roomHeader = &gCurrentRoomHeader;
    if (roomHeader->cameraControl == 3) {
        LoadCurrentRoomCameraControlData();
    }

    {
        u16* effectValues;
        u32 zeroValue;
        register u32 defaultValue PIN("r2");

        effectValues = gRoomCameraBounds;
        zeroType = 0;
        zeroValue = 0;
        defaultValue = 128;
        effectValues[0] = defaultValue;
        effectValues[1] = defaultValue;
        effectValues[3] = defaultValue;
        effectValues[2] = defaultValue;
        if (gStageRoomTableIndex == 24) {
            effectValues[0] = zeroValue;
            effectValues[1] = zeroValue;
            effectValues[3] = zeroValue;
        }

        gLayer3ScrollEffectState.value = zeroValue;
        gLayer3ScrollEffectState.timer = zeroType;
        if (roomHeader->layer3Scrolling == 7) {
            gLayer3ScrollEffectState.type = 1;
        } else {
            gLayer3ScrollEffectState.type = zeroType;
        }
    }

    {
        struct RoomEffectState* effect;
        register u32 zeroType2 PIN("r2");
        u32 zeroValue2;

        effect = &gBg0ScrollEffect;
        zeroType2 = 0;
        zeroValue2 = 0;
        effect->value = zeroValue2;
        effect->timer = zeroType2;
        if (gCurrentRoomHeader.bg0Param == 34) {
            effect->type = 2;
        } else {
            effect->type = zeroType2;
        }
    }
}

void DrawRoomBackgroundLayer(u8 layer)
{
    /*
     * Zero-instruction asm constraints below only preserve agbcc register lifetimes.
     * The renderer logic and memory operations remain expressed entirely in C.
     */
    volatile u32 layerValue;
    volatile u32 backgroundParameter;
    u16* volatile source;
    volatile u32 startX;
    u32 columns;
    s32 rows;
    struct BackgroundInfo* savedBackground;
    u32 nextStartY;
    register s32 nextRow PIN("sl");
    s32 row;
    u32 column;
    u16 sourceIndex;
    vu16* destination;
    u32 screenBlockOffset;
    u16 metatile;
    u16 tileIndex;
    register u16 screenColumn PIN("r1");
    layerValue = layer;
    {
        u32 yPosition;

        {
            register struct BackgroundInfo* background PIN("r6");
            register u32 width PIN("r4");
            register u32 calculation PIN("r0");
            u32 startRegister;

            {
                u32 xPosition;

                if (layer == 0) {
                    backgroundParameter = gCurrentRoomHeader.bg0Param;
                    yPosition = gBg0YPosition;
                    xPosition = gBg0XPosition;
                } else if (layerValue == 1) {
                    backgroundParameter = gCurrentRoomHeader.bg1Param;
                    yPosition = gBg1YPosition;
                    xPosition = gBg1XPosition;
                } else {
                    backgroundParameter = gCurrentRoomHeader.bg2Param;
                    yPosition = gBg2YPosition;
                    xPosition = gBg2XPosition;
                }

                {
                    u32 mask;
                    register u32 parameter PIN("r3");

                    mask = 0x10;
                    parameter = backgroundParameter;
                    if ((mask & parameter) == 0) {
                        goto RenderComplete;
                    }
                }

                calculation = xPosition >> 6;
                width = 21;
                ASM_BARRIER(: "+r"(width));
                columns = width;
                calculation -= 3;
                calculation <<= 16;
                startRegister = calculation >> 16;
                if ((s32)calculation < 0) {
                    startRegister = 0;
                }
                {
                    register u8* base PIN("r1");
                    register u32 layerIndex PIN("r5");

                    base = (u8*)&gBackgroundInfo;
                    layerIndex = layerValue;
                    calculation = layerIndex << 3;
                    background = (struct BackgroundInfo*)(calculation + (u32)base);
                }
                width = background->bg0Width;
                {
                    u32 shiftedStart;
                    s32 columnCount;

                    shiftedStart = startRegister << 16;
                    calculation = (s32)shiftedStart >> 16;
                    calculation = width - calculation;
                    columnCount = columns;
                    if (columnCount > (s32)calculation) {
                        calculation = width - startRegister;
                        calculation <<= 16;
                        calculation >>= 16;
                        columns = calculation;
                    }
                    shiftedStart >>= 16;
                    startX = shiftedStart;
                }
            }

            calculation = yPosition >> 6;
            {
                u32 rowCount;
                rowCount = 16;
                rows = rowCount;
            }
            calculation -= 3;
            calculation <<= 16;
            startRegister = calculation >> 16;
            if ((s32)calculation < 0) {
                startRegister = 0;
            }
            {
                u32 height;
                s32 rowCount;

                height = background->bg0Height;
                yPosition = startRegister << 16;
                calculation = (s32)yPosition >> 16;
                calculation = height - calculation;
                rowCount = rows;
                if (rowCount > (s32)calculation) {
                    calculation = height - startRegister;
                    calculation <<= 16;
                    calculation >>= 16;
                    rows = calculation;
                }
            }
            yPosition >>= 16;

            calculation = width;
            calculation *= startRegister;
            calculation = (u16)calculation;
            {
                u32 pointer;
                pointer = startX;
                calculation = pointer + calculation;
                calculation <<= 1;
                pointer = (u32)GP(background->pBg0Data);
                pointer += calculation;
                source = (u16*)pointer;
            }

            row = 0;
            {
                register s32 rowLimit PIN("r3");
                rowLimit = rows;
                ASM_BARRIER(: "+r"(rowLimit));
                if (row >= rowLimit) {
                    goto RenderComplete;
                }
            }
            savedBackground = background;
        }

        do {
            register s32 remainingColumns PIN("r5");
            register struct BackgroundInfo* rowBackground PIN("r4");
            register u32 nextY PIN("r5");
            register u32 columnCount PIN("r0");

            rowBackground = savedBackground;
            ASM_BARRIER(: "+r"(rowBackground));
            sourceIndex = rowBackground->bg0Width * row;
            {
                u32 columnRegister;

                columnRegister = startX;
                column = columnRegister;
            }
            nextRow = row + 1;
            nextY = yPosition + 1;
            ASM_BARRIER(: "+r"(nextY));
            nextStartY = nextY;
            columnCount = columns;
            if (columnCount != 0) {
                u32 screenBlockRegister;
                u32 layerForOffset;
                u32 rowOffset;

                layerForOffset = layerValue;
                screenBlockRegister = layerForOffset << 12;
                screenBlockOffset = screenBlockRegister;
                {
                    register u32 rowMask PIN("r3");
                    rowMask = 0xF;
                    yPosition &= rowMask;
                }
                yPosition <<= 6;
                rowOffset = yPosition;
                remainingColumns = columnCount;
                do {
                    register u32 destinationBase PIN("r4");
                    register u32 vramBase PIN("r0");
                    register u32 tileOffset PIN("r0");
                    register u32 indexValue PIN("r3");
                    register u32 sourceOffset PIN("r0");
                    register u16* sourcePointer PIN("r4");

                    vramBase = 0x06000000;
                    ASM_BARRIER(: "+r"(vramBase));
                    destinationBase = screenBlockOffset;
                    ASM_BARRIER(: "+r"(destinationBase));
                    destination = (vu16*)(destinationBase + vramBase);
                    screenColumn = column & 0x1F;
                    if ((screenColumn & 0x10) != 0) {
                        destination = (vu16*)(0x06000800 + screenBlockOffset);
                    }
                    {
                        u32 mask;
                        register u32 columnOffset PIN("r1");
                        register u32 rowBase PIN("r2");
                        mask = screenColumn & 0xF;
                        columnOffset = mask << 1;
                        rowBase = rowOffset;
                        ASM_BARRIER(: "+r"(rowBase));
                        tileOffset = rowBase + columnOffset;
                        ASM_BARRIER(: "+r"(tileOffset));
                    }
                    destination += tileOffset;
                    indexValue = sourceIndex;
                    ASM_BARRIER(: "+r"(indexValue));
                    sourceOffset = indexValue << 1;
                    sourcePointer = source;
                    sourceOffset = sourceOffset + (u32)sourcePointer;
                    metatile = *(u16*)sourceOffset;
                    tileIndex = metatile * 4;
                    destination[0] = GP(gBackgroundTileTables.top)[tileIndex++];
                    destination[1] = GP(gBackgroundTileTables.top)[tileIndex++];
                    destination[32] = GP(gBackgroundTileTables.top)[tileIndex++];
                    destination[33] = GP(gBackgroundTileTables.top)[tileIndex++];
                    sourceIndex++;
                    remainingColumns--;
                    column = (u16)(column + 1);
                } while (remainingColumns != 0);
            }
            row = nextRow;
            {
                register u32 nextYRaw PIN("r2");
                register u32 normalizedY PIN("r0");
                register s32 rowLimit PIN("r3");
                nextYRaw = nextStartY;
                ASM_BARRIER(: "+r"(nextYRaw));
                normalizedY = nextYRaw << 16;
                yPosition = normalizedY >> 16;
                rowLimit = rows;
                ASM_BARRIER(: "+r"(rowLimit));
                if (row >= rowLimit) {
                    break;
                }
            }
        } while (1);
    }

RenderComplete: {
    register u32 parameter PIN("r4");
    parameter = backgroundParameter;
    if (parameter == 0) {
        u8* bufferRegister;
        struct DmaRegisters* dmaRegister;
        u32 layerOffset;
        u32 controlRegister;
        const u8* compressedData;
        register u32 layerTemporary PIN("r0");
        u32 controlValue;
        register u32 finalControl PIN("r2");
        u32 vramBase;
        u32 secondVramBase;

        compressedData = sEmptyRoomBackgroundTilemap;
        bufferRegister = (u8*)0x0201F040;
        DecompressRoomBackground(0, compressedData, bufferRegister);
        dmaRegister = (struct DmaRegisters*)0x040000D4;
        GP_SET(dmaRegister->source, bufferRegister);
        layerTemporary = layerValue;
        layerOffset = layerTemporary << 12;
        vramBase = 0x06000000;
        ASM_BARRIER(: "+r"(vramBase));
        GP_SET(dmaRegister->destination, (void*)(layerOffset + vramBase));
        controlValue = 0x80000400;
        controlRegister = controlValue;
        dmaRegister->control = controlValue;
        WaitForDma3Transfer(dmaRegister->control);
        GP_SET(dmaRegister->source, bufferRegister);
        secondVramBase = 0x06000800;
        layerOffset += secondVramBase;
        GP_SET(dmaRegister->destination, (void*)layerOffset);
        finalControl = controlRegister;
        dmaRegister->control = finalControl;
        dmaRegister->control;
    }
}
}

void PreloadStageRoomBackgrounds(void)
{
    u8* destination;
    const struct RoomHeader* entry;
    s32 index;
    const u8* stageCountBase;
    const u8* stageCountTable;
    GBA_PTR(u8*) * destinationTable;
    const u8* graphics;
    u32 size;
    s32 stageOffset;

    destination = (u8*)0x02000040;
    entry = GP(sRoomHeaderTables[gStageRoomTableIndex]);
    index = 0;
    stageCountBase = sStageRoomCounts;
    stageOffset = gCurrentStageID * 12;
    stageCountBase++;
    if (index < stageCountBase[stageOffset]) {
        stageCountTable = stageCountBase;
        destinationTable = (GBA_TYPEOF(u8**, destinationTable))0x02000000;
        do {
            graphics = (const u8*)GP(entry->pBg1Data);
            size = (graphics[0] * graphics[1] + 2) * 2;
            DecompressRoomBackground(2, graphics, destination);
            GP_SET(*destinationTable++, destination);
            destination += size;
            entry++;
            index++;
        } while (index < stageCountTable[gCurrentStageID * 12]);
    }
}

u32 DecompressRoomBackground(u8 type, const u8* sourceArg, u8* destinationArg)
{
    const u8* source;
    u8* destination;
    register u32 result PIN("r5");
    u8* base;
    s32 pass;
    u32 count;
    u8 marker;
    u8 value;

    source = sourceArg;
    destination = destinationArg;
    result = 0;
    if (type == 0) {
        result = *source++;
        if (result == 0) {
            result = 0x800;
        } else if (result == 1) {
            result = 0x1000;
        } else if (result == 2) {
            result = 0x1000;
        } else {
            result = 0x2000;
        }
    } else if (type == 1) {
        result = *source++;
        value = *source;
        result <<= 16;
        result |= value;
        source++;
    } else {
        *destination++ = *source++;
        *destination++ = result;
        *destination++ = *source++;
        *destination++ = result;
    }

    base = destination;
    pass = 0;
    do {
        marker = *source++;
        if (marker == 1) {
            count = *source++;
            pass++;
            if (count != 0) {
                do {
                    if (count & 0x80) {
                        count &= 0x7F;
                        if (count != 0) {
                            do {
                                *destination = *source;
                                destination += 2;
                                count--;
                            } while (count != 0);
                        }
                        source++;
                    } else {
                        while (count != 0) {
                            *destination = *source++;
                            destination += 2;
                            count--;
                        }
                    }
                    count = *source++;
                } while (count != 0);
            }
        } else {
            count = *source++;
            count <<= 8;
            count |= *source++;
            pass++;
            if (count != 0) {
                do {
                    if (count & 0x8000) {
                        count &= 0x7FFF;
                        if (count != 0) {
                            do {
                                *destination = *source;
                                destination += 2;
                                count--;
                            } while (count != 0);
                        }
                        source++;
                    } else {
                        while (count != 0) {
                            *destination = *source++;
                            destination += 2;
                            count--;
                        }
                    }
                    count = *source++;
                    count <<= 8;
                    count |= *source++;
                } while (count != 0);
            }
        }
        destination = base;
        destination++;
    } while (pass <= 1);

    return result;
}

u32 LoadBossPauseBackgroundTiles(u8 useAlternateTiles)
{
    struct DmaRegisters* dma;

    if (useAlternateTiles != 0) {
        dma = (struct DmaRegisters*)0x040000D4;
        GP_SET(dma->source, sBossPauseAlternateBackgroundTiles);
    } else {
        dma = (struct DmaRegisters*)0x040000D4;
        GP_SET(dma->source, sBossPauseBackgroundTiles);
    }
    GP_SET(dma->destination, (void*)0x0600FE40);
    dma->control = 0x800000D0;
    return dma->control;
}

void UpdateBossRoomState(void)
{
    gUnk_300001B = 0;
    if (gCurrentStageNumber == STAGE_BOSS) {
        gUnk_300001B = 1;
        if (gCurrentRoom != 0) {
            gUnk_300001B = 2;
            if (gCurrentPassage == PASSAGE_GOLDEN) {
                gUnk_300001B = 3;
                gSpriteAiDropTimer = 0;
                gShopItemTimer = 0;
            }
        }
    }
}

void InitializeRoomMusic(void)
{
    struct MusicPlayerInfo* musicPlayer;
    const struct MusicPlayer* musicPlayers;
    const struct Song* songs;
    u16 playerId;

    if (gUnk_300001C != 0) {
        return;
    }
    gGameMusicState.specialState = 0;
    gGameMusicState.specialPlayerId = 0;
    if (gSwitchPressed != 0) {
        gCurrentRoomHeader.musicVolume = 0x100;
    }

    if (gStageEntryPauseMenuDisabled == 0) {
        gGameMusicState.specialSongId = 0;
        if (gHasTemporarySave != 0 && gSwitchPressed != 0) {
            SetHurryUpMusic(0);
        }
        m4aSongNumStartOrChange(gGameMusicState.songId);
        musicPlayers = gMPlayTable;
        songs = gSongTable;
        playerId = songs[gGameMusicState.songId].ms;
        musicPlayer = GP(musicPlayers[playerId].info);
        m4aMPlayVolumeControl(musicPlayer, 0xFFFF, gCurrentRoomHeader.musicVolume);
        gGameMusicState.previousSongId = gGameMusicState.songId;
        if (gHasTemporarySave != 0) {
            if (gSwitchPressed != 0) {
                m4aSongNumStart(SE_HURRY_UP_SWITCH);
            } else {
                m4aSongNumStart(SOUND_1CC);
            }
        }
    } else if (gPauseFlag != 0 || gUnk_3000026 == 5) {
        if (gGameMusicState.specialSongId != 0) {
            m4aSongNumStartOrContinue(gGameMusicState.specialSongId);
            gGameMusicState.specialPlayerId = gSongTable[gGameMusicState.specialSongId].ms;
            gGameMusicState.specialState = 1;
        } else {
            if (gGameMusicState.songId == gGameMusicState.previousSongId) {
                musicPlayers = gMPlayTable;
                songs = gSongTable;
                playerId = songs[gGameMusicState.songId].ms;
                m4aMPlayFadeIn(GP(musicPlayers[playerId].info), 1);
            } else {
                m4aSongNumStart(gGameMusicState.songId);
            }
        }
        gGameMusicState.previousSongId = gGameMusicState.songId;
    } else {
        gGameMusicState.specialSongId = 0;
        if (gSwitchPressed != 0) {
            SetHurryUpMusic(0);
        }
        gGameMusicState.state = 1;
    }
    gGameMusicState.playerId = gSongTable[gGameMusicState.previousSongId].ms;
}

void SetHurryUpMusic(u8 startMusic)
{
    if (gUnk_300001C == 0) {
        gGameMusicState.songId = BGM_HURRY_UP;
        if (startMusic != 0) {
            gGameMusicState.state = 0;
            gGameMusicState.memoryAccessState = 0;
            gGameMusicState.previousSongId = BGM_HURRY_UP;
            m4aSongNumStartOrChange(BGM_HURRY_UP);
            gGameMusicState.playerId = gSongTable[BGM_HURRY_UP].ms;
        }
    }
}

void ProcessRoomMusic(void)
{
    u16 playerId;

    if (gUnk_300001C != 0) {
        return;
    }
    switch (gGameMusicState.state) {
        case 0:
            ProcessWarioTransformationMusic();
            break;
        case 1: {
            struct MusicPlayerInfo* musicPlayer;
            const struct MusicPlayer* musicPlayers;
            const struct Song* songs;

            if (gGameMusicState.previousSongId == gGameMusicState.songId) {
                musicPlayers = gMPlayTable;
                songs = gSongTable;
                playerId = songs[gGameMusicState.songId].ms;
                musicPlayer = GP(musicPlayers[playerId].info);
                m4aMPlayVolumeControl(musicPlayer, 0xFFFF, gCurrentRoomHeader.musicVolume);
                gGameMusicState.state = 0;
            } else {
                gGameMusicState.state = 2;
                gGameMusicState.memoryAccessState = gMPlayMemAccArea[0];
            }
            break;
        }
        case 2: {
            struct MusicPlayerInfo* musicPlayer;
            const struct MusicPlayer* musicPlayers;
            const struct Song* songs;

            if (gGameMusicState.memoryAccessState != gMPlayMemAccArea[0]) {
                musicPlayers = gMPlayTable;
                playerId = gGameMusicState.playerId;
                m4aMPlayFadeOut(GP(musicPlayers[playerId].info), 12);
                m4aSongNumStart(gGameMusicState.songId);
                songs = gSongTable;
                playerId = songs[gGameMusicState.songId].ms;
                musicPlayer = GP(musicPlayers[playerId].info);
                m4aMPlayVolumeControl(musicPlayer, 0xFFFF, gCurrentRoomHeader.musicVolume);
                gGameMusicState.previousSongId = gGameMusicState.songId;
                gGameMusicState.playerId = gSongTable[gGameMusicState.previousSongId].ms;
                gGameMusicState.state = 0;
            }
            break;
        }
    }
}

void ProcessWarioTransformationMusic(void)
{
    struct MusicPlayerInfo* musicPlayer;
    const struct MusicPlayer* musicPlayers;
    u16 playerId;

    if (gSwitchPressed != 0) {
        return;
    }
    switch (gGameMusicState.specialState) {
        case 0:
            if (gWarioData.reaction == 0 && (gWarioData.pose == 54 || gWarioData.pose == 59)) {
                SelectWarioTransformationMusic();
                m4aSongNumStart(gGameMusicState.specialSongId);
                musicPlayers = gMPlayTable;
                playerId = gGameMusicState.playerId;
                musicPlayer = GP(musicPlayers[playerId].info);
                m4aMPlayFadeOutTemporarily(musicPlayer, 38);
                gGameMusicState.specialState++;
            }
            break;
        case 1:
            if (gWarioData.reaction == 0 && gWarioData.pose != 54 && gWarioData.pose != 59) {
                gGameMusicState.specialState = 2;
            }
            break;
        case 2:
            musicPlayers = gMPlayTable;
            playerId = gGameMusicState.playerId;
            m4aMPlayFadeIn(GP(musicPlayers[playerId].info), 1);
            m4aSongNumStop(gGameMusicState.specialSongId);
            gGameMusicState.specialState = 0;
            gGameMusicState.specialSongId = 0;
            break;
    }
}

void SelectWarioTransformationMusic(void)
{
    register const u8* musicIndexPointer PIN("r1");
    u32 currentMusicIndex;
    u32 index;
    struct GameMusicState* musicState;
    const u32* specialMusicTable;
    u32 tableOffset;

    musicIndexPointer = &gUnk_3000C36;
    currentMusicIndex = *(volatile const u8*)musicIndexPointer;
    index = 0;
    if (currentMusicIndex <= 16) {
        index = currentMusicIndex;
    }
    musicState = &gGameMusicState;
    specialMusicTable = sWarioTransformationMusic;
    tableOffset = index << 2;
    tableOffset += (u32)specialMusicTable;
    musicState->specialSongId = *(const u32*)tableOffset;
    musicState->specialPlayerId = gSongTable[musicState->specialSongId].ms;
}

void FadeOutMusicForPause(void)
{
    m4aMPlayFadeOutTemporarily(GP(gMPlayTable[0].info), 2);
    m4aMPlayFadeOutTemporarily(GP(gMPlayTable[1].info), 2);
    m4aSongNumStop(gGameMusicState.specialSongId);
}

void ProcessGoldenPassageBossTransition(void)
{
    u8* transitionFlag;
    u32 advanceState;
    u8* transitionState;

    advanceState = 0;
    {
        u8* temporaryPointer;
        u32 temporaryValue;

        temporaryPointer = &gShopItemTimer;
        temporaryValue = *temporaryPointer;
        transitionFlag = temporaryPointer;
        if (temporaryValue != 0) {
            gUnk_300188E++;
        }
    }
    {
        u8* temporaryPointer;
        u32 temporaryValue;

        temporaryPointer = &gSpriteAiDropTimer;
        temporaryValue = *temporaryPointer;
        transitionState = temporaryPointer;
        switch (temporaryValue) {
            case 0:
                gGoldenBossTransitionBg0Y = 0;
                gUnk_3003BF5 = 0;
                gUnk_3003BF6 = 0;
                if (*transitionFlag != 1) {
                    break;
                }
                goto increment_state;
            case 1: {
                u16* firstAlpha;
                u16* blendAlpha;
                u32 value;

                *(vu16*)0x04000050 = 0x3748;
                firstAlpha = &gBlendAlphaEvb;
                value = 16;
                *firstAlpha = value;
                gBlendAlphaEva = 0;
                blendAlpha = &gBldAlpha;
                value <<= 8;
                *blendAlpha = value;
                goto increment_state;
            }
            case 2:
                *(vu16*)0x04000008 = (*(vu16*)0x04000008 & 0xFFFC) | 1;
                *(vu16*)0x0400000A = (*(vu16*)0x0400000A & 0xFFFC) | 2;
                *(vu16*)0x0400000C = (*(vu16*)0x0400000C & 0xFFFC) | 3;
                *(vu16*)0x0400000E = *(vu16*)0x0400000E & 0xFFFC;
                goto increment_state;
            case 3:
                if ((gUnk_300188E & 3) == 0) {
                    gBlendAlphaEvb--;
                    if (gBlendAlphaEvb == 8) {
                        advanceState = 1;
                    }
                    gBldAlpha = (gBlendAlphaEvb << 8) | gBlendAlphaEva;
                }
                break;
            case 4:
                if (gUnk_300188E > 40) {
                    SetHBlankMode(15);
                    goto increment_state;
                }
                break;
            case 5:
                if ((gUnk_300188E & 7) == 0) {
                    gBlendAlphaEva++;
                    if (gBlendAlphaEva == 8) {
                        advanceState = 1;
                    }
                    gBldAlpha = gBlendAlphaEva | (gBlendAlphaEvb << 8);
                }
                break;
            case 6:
                if (gUnk_300188E > 45) {
                    gUnk_3003BF5 = 1;
                    goto increment_state;
                }
                break;
            case 7:
                if (gUnk_300188E <= 45) {
                    break;
                }
                goto increment_state;
            case 8:
                *transitionFlag = 2;
                goto increment_state;
            case 9:
                if (*transitionFlag != 3) {
                    break;
                }
                goto increment_state;
            case 10:
                if ((gUnk_300188E & 3) == 0) {
                    gBlendAlphaEva--;
                    gBlendAlphaEvb++;
                    if (gBlendAlphaEva == 0) {
                        gBlendAlphaEvb = 16;
                        advanceState = 1;
                    }
                    gBldAlpha = gBlendAlphaEva | (gBlendAlphaEvb << 8);
                }
                break;
            case 11:
                *(vu16*)0x04000008 = (*(vu16*)0x04000008 & 0xFFFC) | 1;
                *(vu16*)0x0400000A = *(vu16*)0x0400000A & 0xFFFC;
                *(vu16*)0x0400000C = (*(vu16*)0x0400000C & 0xFFFC) | 2;
                *(vu16*)0x0400000E = (*(vu16*)0x0400000E & 0xFFFC) | 3;
                *(vu16*)0x04000050 = 0x3F40;
                *transitionFlag = 0;
                *transitionState = 0;
                gUnk_300188E = 0;
                break;
        }
    }
    if (advanceState != 0) {
    increment_state:
        gSpriteAiDropTimer++;
        gUnk_300188E = 0;
    }
    UpdateGoldenPassageBossTransitionTimer();
}

void UpdateGoldenPassageBossTransitionTimer(void)
{
    if (gUnk_3003BF5 == 0) {
        gUnk_3003BF6 = 0;
    } else {
        gUnk_3003BF6++;
        if (gGoldenBossTransitionBg0Y <= 149) {
            gGoldenBossTransitionBg0Y++;
        } else {
            gUnk_3003BF5 = 0;
        }
    }
}

void ProcessRoomBackgrounds(void)
{
    if (gCurrentRoomHeader.bg0Param <= 31) {
        if (gCurrentRoomHeader.bg0Param == 17) {
            TransparencyProcessWater();
        } else if (gCurrentRoomHeader.bgPriorityAlpha > 23) {
            TransparencyProcessTiles(1);
        }
    }
    if (gTimerState != 0) {
        UpdateEscapePaletteFadeSequence();
    }
    UpdateBossDefeatPaletteFade();
    UpdateGoldenDivaPaletteFade();
    if (gShopItemState != 0) {
        ProcessRoomWindowEffect();
    }
    if (gUnk_300001B > 1) {
        UpdateTileEffect();
        if (gUnk_300001B == 3) {
            ProcessGoldenPassageBossTransition();
        }
    }
    if (gPauseFlag != 0) {
        BackupPalettesForColorFade();
        gGameMusicState.state = 0;
        gColorFading.progress = 0;
        gColorFading.type = 2;
    }
}

void ProcessRoomWindowEffect(void)
{
    u8* statePointer;
    register u32 state PIN("r4");

    statePointer = &gShopItemState;
    state = *statePointer;
    {
        u32 value;

        value = 0xC0;
        value &= state;
        if (value == 0) {
            value = 0xC0;
            value |= state;
            *statePointer = value;
            gUnk_30037BF = 1;
            return;
        }
    }

    if ((state & 0x80) != 0) {
        u8* timerPointer;
        u32 timer;

        timerPointer = &gUnk_30037BF;
        timer = *timerPointer;
        timer--;
        *timerPointer = timer;
        if ((timer << 24) == 0) {
            struct Window* window;
            u32 windowValue;
            vu16* windowRegister;
            u32 windowBottom;

            window = &gWindow;
            window->top = 0xA0;
            *(vu16*)0x05000000 = 0x7FFF;
            windowRegister = (vu16*)0x04000046;
            windowValue = 0xA000;
            windowBottom = window->bottom;
            windowBottom |= windowValue;
            *windowRegister = windowBottom;
            *timerPointer = 2;
            *statePointer = state & 0x7F;
        }
    } else {
        u8* timerPointer;
        u32 timerValue;
        u8 timer;

        timerPointer = &gUnk_30037BF;
        timerValue = *timerPointer;
        timerValue--;
        *timerPointer = timerValue;
        timer = timerValue;
        if (timer == 0) {
            struct Window* window;
            register u32 nextState PIN("r1");

            window = &gWindow;
            window->top = 0;
            *(vu16*)0x04000046 = window->bottom;
            *(vu16*)0x05000000 = gHBlankPaletteFadeColors[1];
            nextState = state - 1;
            *statePointer = nextState;
            if ((u8)nextState == 0x40) {
                *statePointer = 0;
            } else {
                *timerPointer = 2;
                {
                    register u32 highBit PIN("r0");
                    highBit = 0x80;
                    nextState |= highBit;
                }
                *statePointer = nextState;
            }
        }
    }
}

void UpdateRoomAnimatedGraphics(void)
{
    if (gSubGameMode == 2 || gSubGameMode == 6 || (gSubGameMode == 4 && gUnk_300001B == 1)) {
        if (gUnk_3000046 == 0) {
            UpdateAnimatedGraphics();
        }
    }
}

void DrawGameScreen(void)
{
    UpdateBackgroundScrollRegisters();
    gScrollingUpdateFrameCounter++;
    if ((gScrollingUpdateFrameCounter & 1) != 0 || gCameraPositionState.xOffset < -28 ||
        gCameraPositionState.xOffset > 28) {
        PrepareHorizontalScrollingTileUploads(16);
        ApplyHorizontalScrollingTileUploads();
        PrepareHorizontalScrollingTileUploads(-2);
        ApplyHorizontalScrollingTileUploads();
    }
    if ((gScrollingUpdateFrameCounter & 1) == 0 || gCameraPositionState.yOffset < -28 ||
        gCameraPositionState.yOffset > 28) {
        PrepareVerticalScrollingTileUploads(11);
        ApplyVerticalScrollingTileUploads();
        PrepareVerticalScrollingTileUploads(-2);
        ApplyVerticalScrollingTileUploads();
    }
    if (gSubGameMode == 2) {
        UpdateWarioTileInteractions();
    }
    UpdateHBlankEffect();
}

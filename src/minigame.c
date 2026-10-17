#include "minigame.h"
#include "background_registers.h"
#include "color_effects.h"
#include "init_helpers.h"
#include "interrupt_callbacks.h"
#include "minigames/homerun_derby.h"
#include "minigames/roulette.h"
#include "minigames/wario_hop.h"
#include "save_file.h"
#include "score.h"
#include "stage_entry.h"
#include "stage_select.h"

extern u16 gMinigameWaitTimer;
extern u16 gUnk_3000000;
extern u8 gSelectedMinigame;
extern u8 gMinigamePlayedFlag;
extern u8 gMinigameMessageId;
extern u8 gMinigameCostUnits;
extern u8 gMinigameMenuItems[];
extern u16 gSoundRoomMessageTimer;
extern const u32 sMinigameItemXPositions[3];
extern const u8 sMinigameEnglishMessageTable[];
extern const u8 sMinigameJapaneseMessageTable[];
extern const u32 sMinigameResultSongIds[];
extern const u32 sMinigameStartSongIds[];
extern const u8 sMinigameTotalScoreDigitTiles[];
extern const u32 sMinigameCursorXPositions[];
extern const u32 sMinigameCursorYPositions[];
extern const u32 sMinigameNavigationTable[];
extern const struct AnimationFrame sMinigameCursorConfirmAnimation[];
extern u8 gMinigameCursorAnimationState[];
extern GBA_PTR(const struct AnimationFrame*) gMinigameCursorAnimation;
extern u8 gMinigameSparkleAnimationState[];
extern GBA_PTR(const struct AnimationFrame*) const sMinigameItemAnimationTable[3][6];
extern const u32 sMinigameSparkleXOffsets[];
extern const u32 sMinigameSparkleYOffsets[];
extern const struct AnimationFrame sMinigameSparkleAnimation[];
extern const struct AnimationFrame sMinigameCursorIdleAnimation[];
extern const struct AnimationFrame sMinigameLockedIconAnimation[];
extern const u8 sMinigameBgPalette[];
extern const u8 sMinigameBgTiles[];
extern const u8 sMinigameHighScoreDigitTiles[];
extern const u8 sMinigameObjPalette[];
extern const u8 sMinigameObjTiles[];
extern const u8 sMinigameBg2Tilemap[];
extern const u8 sMinigameBg1Tilemap[];
extern const u8 sMinigameBg0Tilemap[];

void Shop_DrawText(const u16* text, u32 destination, u32 length);

u32 MinigameSubroutine(void)
{
    u32 result;

    result = 0;
    SetMinigameVBlankCallback();
    gOamSlotsUsed = 0;
    switch (gSubGameMode) {
        case 0:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitMinigameMenuSelection();
            gSubGameMode++;
            *(vu16*)0x04000200 |= 1;
            break;
        case 1:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
        case 2:
            if (UpdateMinigameMenu() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
        case 3:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeOut() != 0) {
                EnableAllLayersInWindows();
                gUnk_3000000 = 0;
                switch (gSelectedMinigame) {
                    case 3:
                        gSubGameMode = 0;
                        result = 1;
                        break;
                    case 0:
                        gSubGameMode = 5;
                        break;
                    case 1:
                        gSubGameMode = 9;
                        break;
                    case 2:
                        gSubGameMode = 13;
                        break;
                }
            }
            break;
        case 4:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitMinigameResultsMenu();
            gSubGameMode = 1;
            *(vu16*)0x04000200 |= 1;
            break;
        case 5:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitHomerunDerby();
            gSubGameMode++;
            *(vu16*)0x04000200 |= 1;
            break;
        case 6:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
        case 7:
            if (UpdateHomerunDerby() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
        case 8:
        case 12:
        case 16:
            goto outro;
        case 9:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            WarioHopInit();
            gSubGameMode++;
            *(vu16*)0x04000200 |= 1;
            break;
        case 10:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
        case 11:
            if (WarioHopUpdateStateMachine() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
        case 13:
            *(vu16*)0x04000200 ^= 1;
            gColorFading.type = 2;
            InitRouletteGame();
            gSubGameMode++;
            *(vu16*)0x04000200 |= 1;
            break;
        case 14:
            UploadColorFadePalettes();
            if (UpdateStageSelectFadeIn() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
        case 15:
            if (UpdateRouletteGame() != 0) {
                s16* modePointer;
                u16 modeValue;
                modePointer = &gSubGameMode;
                modeValue = *modePointer;
                modeValue++;
                *modePointer = modeValue;
            }
            break;
    }
    goto end;

outro:
    UploadColorFadePalettes();
    if (UpdateStageSelectFadeOut() != 0) {
        gSubGameMode = 4;
    }
end:
    DrawActiveMinigame();
    ResetFreeOam();
    return result;
}

s32 MinigameRandom(void)
{
    gRandomSeed = gRandomSeed * 109 + 0x3FD;
    return gRandomSeed;
}

s32 MinigameWaitForFrames(s32 frames)
{
    gMinigameWaitTimer++;
    if (gMinigameWaitTimer > frames) {
        gMinigameWaitTimer = 0;
        return 1;
    }
    return 0;
}

void SetMinigameVBlankCallback(void)
{
    InterruptCallbackSetVBlank(MinigameVBlankCallback);
}

void MinigameVBlankCallback(void)
{
    vu32* dma;

    m4aSoundVSync();
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)gOamBuffer;
    dma[1] = 0x07000000;
    dma[2] = 0x84000100;
    dma[2];
    switch (gSubGameMode) {
        case 1:
        case 2:
        case 3:
        case 4:
            ApplyMinigameMenuVBlank();
            break;
        case 13:
        case 14:
        case 15:
        case 16:
            ApplyRouletteBlend();
            break;
        case 9:
        case 10:
        case 11:
        case 12:
            WarioHopApplyBgScroll();
            break;
    }
}

void MinigameFadeVBlankCallback(void)
{
    vu32* dma;

    m4aSoundVSync();
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)gOamBuffer;
    dma[1] = 0x07000000;
    dma[2] = 0x84000100;
    dma[2];
    *(vu16*)0x04000054 = gBldy;
}

void DrawMinigameHighScore(u32 value, const u16* tiles, u32 destination)
{
    u32 currentValue;
    const u16* tileBase;
    u32 destinationBase;
    register s32 hundreds PIN("r4");
    s32 tens;
    s32 ones;
    vu32* dma;
    u32 control;
    u32 busy;
    register u32 source PIN("r0");

    currentValue = value;
    tileBase = tiles;
    destinationBase = destination;
    hundreds = __modsi3(__divsi3(currentValue, 100), 10);
    tens = __modsi3(__divsi3(currentValue, 10), 10);
    ones = __modsi3(currentValue, 10);

    dma = (vu32*)0x040000D4;
    hundreds <<= 5;
    hundreds = (s32)tileBase + hundreds;
    dma[0] = hundreds;
    dma[1] = 0x06000000 + destinationBase;
    dma[2] = 0x80000010;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)(tileBase + tens * 16);
    dma[1] = 0x06000020 + destinationBase;
    dma[2] = 0x80000010;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    hundreds = ones << 5;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    source = (u32)tileBase + hundreds;
    dma[0] = source;
    dma[1] = 0x06000040 + destinationBase;
    dma[2] = 0x80000010;
    dma[2];
}

void DrawActiveMinigame(void)
{
    switch (gSubGameMode) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            DrawMinigameMenuSprites();
            break;
        case 14:
        case 15:
        case 16:
            DrawRoulette();
            break;
        case 10:
        case 11:
        case 12:
            WarioHopDrawSprites();
            break;
        case 6:
        case 7:
        case 8:
            DrawHomerunDerby();
            break;
    }
}

s32 ExpandMinigameWindow(void)
{
    gStageEntryWindowLeft -= 5;
    gStageEntryWindowRight += 5;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 160;
    if ((s16)gStageEntryWindowLeft < 0) {
        gStageEntryWindowLeft = 0;
        gStageEntryWindowRight = 240;
        return 1;
    }
    return 0;
}

s32 ContractMinigameWindow(void)
{
    gStageEntryWindowLeft += 5;
    gStageEntryWindowRight -= 5;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 160;
    if ((s16)gStageEntryWindowLeft > 120) {
        gStageEntryWindowLeft = 120;
        gStageEntryWindowRight = 120;
        return 1;
    }
    return 0;
}

u32 PrepareMinigameSave(void)
{
    u8* flag;
    u8 value;
    u32 result;

#ifdef PORTABLE
    // The GBA reads whatever BuildMainSaveWorkingBuffer leaves in r0. The only caller ignores it.
    BuildMainSaveWorkingBuffer();
    result = 0;
#else
    result = ((u32 (*)(void))BuildMainSaveWorkingBuffer)();
#endif
    flag = &gSaveFlag;
    value = 1;
    *flag = value;
    return result;
}

s32 UpdateMinigameMenu(void)
{
    s32 result;

    result = 0;
    switch (gMinigameState) {
        case 0:
            if (gSoundRoomMessageTimer <= 26) {
                gSoundRoomMessageTimer++;
            }
            if (HandleMinigameMenuInput() != 0) {
                u8 selected;

                selected = gSelectedMinigame;
                if (selected == 3) {
                    const struct MusicPlayer* players;
                    const u8* songAddress;
                    u32 songOffset;
                    u16 playerIndex;

                    players = gMPlayTable;
                    songAddress = (const u8*)gSongTable;
                    songOffset = 0x15EC;
                    songAddress += songOffset;
                    playerIndex = *(const u16*)songAddress;
                    m4aMPlayFadeOut(GP(players[playerIndex].info), 8);
                    gMinigameState = selected;
                } else {
                    const struct MusicPlayer* players;
                    const u8* songAddress;
                    u32 songOffset;
                    u16 playerIndex;

                    gMinigamePlayedFlag = 1;
                    players = gMPlayTable;
                    songAddress = (const u8*)gSongTable;
                    songOffset = 0x15EC;
                    songAddress += songOffset;
                    playerIndex = *(const u16*)songAddress;
                    m4aMPlayFadeOut(GP(players[playerIndex].info), 12);
                    gMinigameState++;
                }
            }
            break;

        case 1:
            if (gSoundRoomMessageTimer <= 26) {
                gSoundRoomMessageTimer++;
            }
            if (UpdateMinigameStartSequence() != 0) {
                gMinigameState = 3;
            }
            break;

        case 2:
            if (UpdateMinigameResultSequence() != 0) {
                gMinigameState = 0;
            }
            break;

        case 3: {
            u8* base;
            u8* selected;
            u8 selectedValue;
            u32 index;
            u8* item;

            if (gSoundRoomMessageTimer <= 26) {
                gSoundRoomMessageTimer++;
            }
            base = gMinigameMenuItems;
            selected = &gSelectedMinigame;
            selectedValue = *selected;
            index = selectedValue * 12;
            item = (u8*)(index + (u32)base);
            if (item[8] == 4) {
                *(u16*)(item + 6) -= 2;
            }
            if (ContractMinigameWindow() != 0) {
                result = 1;
            }
            break;
        }

        case 4: {
            u8* base;
            u8* selected;
            u8 selectedValue;
            u32 index;
            u8* item;

            base = gMinigameMenuItems;
            selected = &gSelectedMinigame;
            selectedValue = *selected;
            index = selectedValue * 12;
            item = (u8*)(index + (u32)base);
            if (item[8] == 4) {
                *(u16*)(item + 6) += 2;
            }
            if (ExpandMinigameWindow() != 0) {
                gMinigameState = gMinigameSubstate;
            }
            break;
        }
    }
    return result;
}

void InitMinigameMenu(void)
{
    const u8* graphicsBase;
    const u8* tilesA;
    register const u8* tilesB PIN("r5");
    register const u8* tilesC PIN("r6");
    register const u8* totalScoreTiles PIN("r8");
    register const u8* scoreLabelTiles PIN("r9");
    const u8* medalTiles;
    register const u8* medalCopy PIN("r3");
    u8* messageDuration;
    register vu32* dma PIN("r1");
    u32 control;
    register u32 busy PIN("r2");
    register vu16* bgControl PIN("r2");
    u16* scores;
    s32 i;
    u32 destination;

    InitializeVideoMemory();

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)sMinigameBgPalette;
    dma[1] = 0x05000000;
    dma[2] = 0x80000100;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)sMinigameObjPalette;
    dma[1] = 0x05000200;
    dma[2] = 0x80000100;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;

    graphicsBase = sMinigameBgTiles;
    tilesA = sMinigameObjTiles;
    tilesB = sMinigameBg0Tilemap;
    tilesC = sMinigameBg1Tilemap;
    scoreLabelTiles = graphicsBase + 0x5F00;
    medalTiles = graphicsBase + 0x5E60;
    messageDuration = &gMinigameCostUnits;
    totalScoreTiles = graphicsBase + 0x5800;

    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)graphicsBase;
    dma[1] = 0x06000000;
    dma[2] = 0x80003000;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    *(vu16*)0x05000000 = 0;

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)tilesA;
    dma[1] = 0x06010000;
    dma[2] = 0x80003C00;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)tilesB;
    dma[1] = 0x0600C000;
    dma[2] = 0x80000400;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)tilesC;
    dma[1] = 0x0600D000;
    dma[2] = 0x80000400;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)sMinigameBg2Tilemap;
    dma[1] = 0x0600E000;
    dma[2] = 0x80000400;
    control = dma[2];

    bgControl = (vu16*)0x04000008;
    *bgControl = 0x5800;
    bgControl++;
    *bgControl = 0x5A01;
    bgControl++;
    *bgControl = 0x5C02;

    dma[0] = (u32)scoreLabelTiles;
    dma[1] = 0x06005B00;
    dma[2] = 0x80000010;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    dma = (vu32*)0x040000D4;
    medalCopy = medalTiles;
    dma[0] = (u32)medalCopy;
    dma[1] = 0x06005B20;
    dma[2] = 0x80000010;
    control = dma[2];
    control = dma[2];
    busy = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & busy;
        } while (control != 0);
    }

    {
        const u8* collection;
        u32 collectionFlags;
        collection = (const u8*)gCurrentCollection;
        collectionFlags = collection[16];
        if ((collectionFlags & 0x20) != 0) {
            goto setDuration50;
        }
        *messageDuration = 20;
        ASM_BARRIER_VOLATILE(
                     :
                     : "r"(collectionFlags),
                       "r"(busy),
                       "r"(medalCopy),
                       "r"(collection),
                       "r"(tilesB),
                       "r"(tilesC),
                       "r"(totalScoreTiles),
                       "r"(scoreLabelTiles),
                       "r"(medalTiles));
    }
    goto durationDone;
setDuration50: {
    u8 value;
    u8* duration;
    value = 50;
    duration = messageDuration;
    *duration = value;
}
durationDone:;

    {
        u32 keepR1;
        u32 keepR2;
        u32 totalScore;
        ASM_BARRIER_VOLATILE(: "=r"(keepR1), "=r"(keepR2));
        totalScore = gTotalScore;
        UploadTallFiveDigitNumberTiles(totalScore, totalScoreTiles, 0x59A0);
    }
    DrawMinigameNumber(gMedalCount, (const u16*)totalScoreTiles, 0x50E0);

    destination = 0x52A0;
    scores = gMinigameHighScores;
    i = 2;
    do {
        DrawMinigameNumber(*scores, (const u16*)sMinigameHighScoreDigitTiles, destination);
        destination += 0x80;
        scores++;
        i--;
    } while (i >= 0);

    {
        vu16* scroll;
        register u16 zero PIN("r2");

        scroll = (vu16*)0x04000012;
        zero = 0;
        *scroll = zero;
        scroll--;
        *scroll = zero;
        scroll += 3;
        *scroll = zero;
        scroll--;
        *scroll = zero;
        scroll += 3;
        *scroll = zero;
        scroll--;
        *scroll = zero;

        gMinigameSequenceState = zero;
        gMinigameSequenceTimer = zero;
        gStageEntryWindowLeft = 120;
        gStageEntryWindowRight = 120;
        gStageEntryWindowTop = zero;
        gStageEntryWindowBottom = 160;
        *(vu16*)0x04000000 |= 0x2000;
        *(vu16*)0x04000048 = 63;
        *(vu16*)0x0400004A = zero;
    }

    {
        u16* smallData;
        u16 entryZero;
        u16* selectionData;
        register const u32* table PIN("r2");
        u8* selected;
        u32 index;

        smallData = (u16*)gMinigameSparkleAnimationState;
        entryZero = 0;
        smallData[0] = entryZero;
        smallData[1] = entryZero;
        selectionData = (u16*)gMinigameCursorAnimationState;
        table = sMinigameCursorXPositions;
        selected = &gSelectedMinigame;
        index = *selected;
        selectionData[2] = table[index];
        table = sMinigameCursorYPositions;
        index = *selected;
        selectionData[3] = table[index];
        selectionData[0] = entryZero;
        selectionData[1] = entryZero;
    }

    GP_SET(gMinigameCursorAnimation, sMinigameCursorIdleAnimation);
    {
        u16* timer;
        u16 loopZero;
        u8* entry;
        s32 count;

        timer = &gSoundRoomMessageTimer;
        loopZero = 0;
        entry = gMinigameMenuItems;
        count = 2;
        do {
            *(u16*)(entry + 0) = loopZero;
            *(u16*)(entry + 2) = loopZero;
            entry += 12;
            count--;
        } while (count >= 0);

        count = 0;
        *timer = count;
        InitMinigameScoreDisplay();
        *(vu16*)0x04000054 = count;
    }
    m4aSongNumStartOrChange(0x2BD);
}

void InitMinigameMenuSelection(void)
{
    u8* selected;
    u8* substate;
    u8* state;
    u8* sequenceFlag;
    u8* message;
    u8* destination;
    u8* base;
    u8* entry;
    const u32* positions;
    u32 zero;
    u32 y;
    s32 i;

    {
        u8* address;
        u32 value;
        address = &gSelectedMinigame;
        value = 3;
        *address = value;
        selected = address;
    }
    substate = &gMinigameSubstate;
    state = &gMinigameState;
    sequenceFlag = &gMinigamePlayedFlag;
    {
        u8* messageAddress;
        messageAddress = &gMinigameMessageId;
        message = messageAddress;
    }
    base = gMinigameMenuItems;
    zero = 0;
    y = 72;
    entry = base + 4;
    positions = sMinigameItemXPositions;
    i = 2;
    do {
        entry[4] = zero;
        *(u16*)entry = *positions++;
        *(u16*)(entry + 2) = y;
        entry += 12;
        i--;
    } while (i >= 0);
    *substate = 0;
    {
        u8 stateValue;
        stateValue = 4;
        *state = stateValue;
        destination = sequenceFlag;
        *destination = 0;
    }
    {
        u8* source;
        u8 value;
        source = selected;
        value = *source;
        destination = message;
        *destination = value;
    }
    *(vu16*)0x04000000 = 0x1700;
    InitMinigameMenu();
}

void InitMinigameResultsMenu(void)
{
    s32 i;
    u8* base;
    u8* entry;
    const u32* positions;
    u32 selectedY;
    u8* selected;

    PrepareMinigameSave();
    i = 0;
    base = gMinigameMenuItems;
    selectedY = 0xFFEC;
    entry = base + 4;
    positions = sMinigameItemXPositions;
    selected = &gSelectedMinigame;
    do {
        if (i == *selected) {
            u32 value;
            value = 4;
            entry[4] = value;
            value = *positions;
            *(u16*)entry = value;
            *(u16*)(entry + 2) = selectedY;
        } else {
            register u32 value PIN("r0");
            value = 0;
            entry[4] = value;
            value = *positions;
            *(u16*)entry = value;
            value = 72;
            *(u16*)(entry + 2) = value;
        }
        entry += 12;
        positions++;
        i++;
    } while (i <= 2);
    gMinigameSubstate = 2;
    gMinigameState = 4;
    *(vu16*)0x04000000 = 0x1400;
    InitMinigameMenu();
    if (gMinigameNewHighScore != 0) {
        gMinigameMessageId = 10;
    } else {
        gMinigameMessageId = 9;
    }
    {
        const u8* songs;
        u8* selectedPtr;
        u32 index;
        songs = (const u8*)sMinigameResultSongIds;
        selectedPtr = &gSelectedMinigame;
        index = *selectedPtr;
        index <<= 2;
        index += (u32)songs;
        m4aSongNumStart(*(const u16*)index);
    }
}

void ApplyMinigameMenuVBlank(void)
{
    vu16* window;
    GBA_PTR(const u16*) const * table;
    u32 index;

    window = (vu16*)0x04000040;
    window[0] = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
    window += 2;
    window[0] = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;

    switch (gLanguage) {
        case 0:
            table = (GBA_TYPEOF(const u16* const*, table))sMinigameEnglishMessageTable;
            index = gMinigameMessageId;
            Shop_DrawText(GP(table[index]), 0x7800, 26);
            break;
        case 1:
            table = (GBA_TYPEOF(const u16* const*, table))sMinigameJapaneseMessageTable;
            index = gMinigameMessageId;
            Shop_DrawText(GP(table[index]), 0x7800, 26);
            break;
    }
}

s32 HandleMinigameMenuInput(void)
{
    register u32 direction PIN("r4");
    u16* buttonsPointer;
    register u16 buttons PIN("r2");
    u16* savedButtonsPointer;

    direction = 4;
    buttonsPointer = &gButtonsPressed;
    buttons = *buttonsPointer;
    {
        u32 pressedA;

        pressedA = 1;
        pressedA &= buttons;
        savedButtonsPointer = buttonsPointer;
        if (pressedA != 0) {
            u8* selected;

            selected = &gSelectedMinigame;
            if (*selected == 3) {
                m4aSongNumStart(0x129);
                goto return_one;
            }
            {
                s32* score;
                u8* requiredScoreSource;
                u8 requiredScoreByte;
                u32 requiredScore;
                s32 scoreValue;

                score = &gTotalScore;
                requiredScoreSource = &gMinigameCostUnits;
                requiredScoreByte = *requiredScoreSource;
                requiredScore = requiredScoreByte * 5;
                requiredScore *= 2;
                scoreValue = *score;
                if (scoreValue >= (s32)requiredScore) {
                    GBA_PTR(const u8*)* animation;
                    const u8* animationTable;
                    u8* coords;
                    u32 zero;
                    u8* entries;
                    u8 selectedValue;
                    u32 index;
                    u8* entry;

                    m4aSongNumStart(0x126);
                    animation = (GBA_TYPEOF(const u8**, animation))&gMinigameCursorAnimation;
                    animationTable = (const u8*)sMinigameCursorConfirmAnimation;
                    GP_SET(*animation, animationTable);
                    coords = gMinigameCursorAnimationState;
                    direction = 0;
                    zero = 0;
                    *(u16*)(coords + 2) = zero;
                    *(u16*)coords = zero;
                    entries = gMinigameMenuItems;
                    selectedValue = *selected;
                    index = selectedValue * 12;
                    entry = (u8*)(index + (u32)entries);
                    entry[8] = direction;
                    selectedValue = *selected;
                    index = selectedValue * 12;
                    entry = (u8*)(index + (u32)entries);
                    *(u16*)entry = zero;
                    selectedValue = *selected;
                    index = selectedValue * 12;
                    entry = (u8*)(index + (u32)entries);
                    *(u16*)(entry + 2) = zero;
                    gMinigameMessageId = 8;
                    gSoundRoomMessageTimer = zero;
                    goto return_one;
                }
            }
            m4aSongNumStart(0x225);
            gMinigameMessageId = direction;
            gSoundRoomMessageTimer = 0;
            return 0;
        }
    }

    if (buttons & 0x20) {
        direction = 0;
    } else if (buttons & 0x10) {
        direction = 1;
    } else if (buttons & 0x40) {
        direction = 2;
    } else if (buttons & 0x80) {
        direction = 3;
    }

    if (direction != 4) {
        const u32* navigation;
        u32 navigationIndex;
        register u8* selected PIN("r4");
        u8 selectedValue;
        u32 nextSelection;

        navigation = sMinigameNavigationTable;
        navigationIndex = direction * 4;
        selected = &gSelectedMinigame;
        selectedValue = *selected;
        navigationIndex += selectedValue * 16;
        nextSelection = navigation[navigationIndex / 4];
        if (nextSelection != 4) {
            u8* coords;
            register const u32* positions PIN("r1");
            u32 index;
            u32 position;
            s32 i;
            u8* loopSelected;
            s32* score;
            u8* message;
            u8* entry;
            u32 zero;

            *selected = nextSelection;
            coords = gMinigameCursorAnimationState;
            positions = sMinigameCursorXPositions;
            selectedValue = *selected;
            index = selectedValue * 4;
            position = *(const u32*)(index + (u32)positions);
            *(u16*)(coords + 4) = position;
            positions = sMinigameCursorYPositions;
            selectedValue = *selected;
            index = selectedValue * 4;
            position = *(const u32*)(index + (u32)positions);
            *(u16*)(coords + 6) = position;
            m4aSongNumStart(0x124);
            i = 0;
            loopSelected = selected;
            zero = 0;
            score = &gTotalScore;
            message = &gMinigameMessageId;
            entry = gMinigameMenuItems;
            do {
                if (*loopSelected == 3) {
                    u8* requiredScoreSource;
                    u8 requiredScoreByte;
                    u32 requiredScore;
                    s32 scoreValue;

                    entry[8] = zero;
                    requiredScoreSource = &gMinigameCostUnits;
                    requiredScoreByte = *requiredScoreSource;
                    requiredScore = requiredScoreByte * 5;
                    requiredScore *= 2;
                    scoreValue = *score;
                    if (scoreValue < (s32)requiredScore) {
                        *message = 7;
                    } else if (gMinigamePlayedFlag != 0) {
                        *message = 6;
                    } else {
                        *message = 11;
                    }
                } else {
                    s32 currentSelectionValue;

                    currentSelectionValue = *loopSelected;
                    if (currentSelectionValue == i) {
                        *message = i;
                        entry[8] = 1;
                    } else {
                        *message = currentSelectionValue;
                        entry[8] = zero;
                    }
                }
                *(u16*)entry = zero;
                *(u16*)(entry + 2) = zero;
                gSoundRoomMessageTimer = zero;
                entry += 12;
                i++;
            } while (i <= 2);
            return 0;
        }
    }

    if (*savedButtonsPointer & 2) {
        m4aSongNumStart(0x129);
        gSelectedMinigame = 3;
        goto return_one;
    }
    return 0;

return_one:
    return 1;
}

s32 UpdateMinigameStartSequence(void)
{
    u8* state;

    state = &gMinigameSequenceState;
    switch (*state) {
        case 0:
            if (MinigameWaitForFrames(20) != 0) {
                (*state)++;
            }
            break;

        case 1: {
            s32* score;
            u16* timer;

            score = &gTotalScore;
            *score -= 10;
            timer = &gMinigameSequenceTimer;
            (*timer)++;
            if ((*timer % 5) == 0) {
                m4aSongNumStart(0x222);
            }
            {
                s32 scoreValue;
                const void* tiles;
                u32 destination;

                scoreValue = *score;
                tiles = sMinigameTotalScoreDigitTiles;
                destination = 0x59A0;
                UploadTallFiveDigitNumberTiles(scoreValue, tiles, destination);
            }
            if (*timer == gMinigameCostUnits) {
                *timer = 0;
                (*state)++;
            }
            break;
        }

        case 2:
            if (MinigameWaitForFrames(60) != 0) {
                const u8* songTable;
                u8* selected;
                u8* base;
                u32 index;
                u32 zero;
                register u8* item PIN("r0");

                songTable = (const u8*)sMinigameStartSongIds;
                selected = &gSelectedMinigame;
                m4aSongNumStart(*(u16*)(songTable + (*selected * 4)));
                (*state)++;
                *(vu16*)0x04000000 = 0x3400;
                base = gMinigameMenuItems;
                index = *selected * 12;
                item = (u8*)(index + (u32)base);
                zero = 0;
                item[8] = 3;
                index = *selected * 12;
                item = (u8*)(index + (u32)base);
                *(u16*)(item + 0) = zero;
                index = *selected * 12;
                item = (u8*)(index + (u32)base);
                *(u16*)(item + 2) = zero;
            }
            break;

        case 3: {
            u8* base;
            u8* selected;
            u8 selectedIndex;
            register u32 index PIN("r1");
            u8* item;
            s32 offset;

            base = gMinigameMenuItems;
            selected = &gSelectedMinigame;
            selectedIndex = *selected;
            index = selectedIndex * 12;
            item = (u8*)(index + (u32)base);
            if (item[8] == 4) {
                *(u16*)(item + 6) -= 2;
            }
            selectedIndex = *selected;
            index = selectedIndex * 12;
            item = (u8*)(index + (u32)base);
            offset = 6;
            if (*(s16*)(item + offset) <= 19) {
                return 1;
            }
            break;
        }
    }
    return 0;
}

s32 UpdateMinigameResultSequence(void)
{
    u8* state;
    state = &gMinigameSequenceState;
    switch (*state) {
        case 0: {
            u8* base;
            u8* selected;
            u8* address0;
            u32 index0;
            u32 value1;
            u8* address1;
            s32 offset;

            base = gMinigameMenuItems;
            selected = &gSelectedMinigame;
            index0 = *selected * 12;
            address0 = (u8*)(index0 + (u32)base);
            value1 = *(u16*)(address0 + 6);
            value1 += 2;
            *(u16*)(address0 + 6) = value1;
            index0 = *selected * 12;
            address1 = (u8*)(index0 + (u32)base);
            offset = 6;
            if (*(s16*)(address1 + offset) > 40) {
                *(u16*)(address1 + 6) = 72;
                index0 = *selected * 12;
                address0 = (u8*)(index0 + (u32)base);
                value1 = 5;
                address0[8] = value1;
                index0 = *selected * 12;
                address0 = (u8*)(index0 + (u32)base);
                *(u16*)(address0 + 0) = 0;
                index0 = *selected * 12;
                address0 = (u8*)(index0 + (u32)base);
                *(u16*)(address0 + 2) = 0;
                (*state)++;
            }
            break;
        }
        case 1: {
            u8* base;
            u8* selected;
            u8 selectedIndex;
            u32 index;
            u8* address;

            base = gMinigameMenuItems;
            selected = &gSelectedMinigame;
            selectedIndex = *selected;
            index = selectedIndex * 12;
            address = (u8*)(index + (u32)base);
            if (address[8] == 1) {
                if (gMinigameNewHighScore != 0) {
                    m4aSongNumStart(0x251);
                }
                gSoundRoomMessageTimer = 0;
                *(vu16*)0x04000000 = 0x3700;
                *state = 0;
                return 1;
            }
            break;
        }
    }
    return 0;
}

void DrawMinigameMenuSprites(void)
{
    s32 processed;
    s32 slots;
    u16* append;
    const u16* source;
    u8* adjust;
    u16 attr;
    u32 value;

    processed = 0;
    {
        register u8* slotsPointer PIN("r3");
        u32 offset;
        u8* oamBase;
        u8* entries;
        u8* selectedPointer;
        u8 selected;
        slotsPointer = &gOamSlotsUsed;
        slots = *slotsPointer;
        offset = slots << 3;
        oamBase = (u8*)gOamBuffer;
        append = (u16*)(offset + (u32)oamBase);
        entries = gMinigameMenuItems;
        selectedPointer = &gSelectedMinigame;
        selected = *selectedPointer;
        offset = selected * 3;
        offset <<= 2;
        selectedPointer = (u8*)(offset + (u32)entries);
        if ((u8)(selectedPointer[8] - 3) > 2) {
            const u8* collection;
            collection = (const u8*)gCurrentCollection;
            if ((collection[16] & 0x20) == 0) {
                source = GP(*(GBA_PTR(const u16*) const *)sMinigameLockedIconAnimation);
                slots += *source++;
                if (slots > 128) {
                    return;
                }

                if (processed < slots) {
                    u8* firstAdjust;
                    u32 xMask;
                    register u32 attr1Mask PIN("r10");
                    s32 clearMask;
                    firstAdjust = (u8*)gOamBuffer;
                    xMask = 0x1FF;
                    attr1Mask = 0xFFFFFE00;
                    clearMask = -13;
                    adjust = firstAdjust;
                    processed = slots;
                    do {
                        attr = *source++;
                        *append++ = attr;
                        adjust[0] = attr + 38;

                        attr = *source++;
                        *append++ = attr;
                        value = attr;
                        value += 168;
                        value &= xMask;
                        {
                            register u32 oldAttr PIN("r2");
                            register u32 maskedAttr PIN("r0");
                            oldAttr = ((u16*)adjust)[1];
                            maskedAttr = attr1Mask;
                            maskedAttr &= oldAttr;
                            maskedAttr |= value;
                            ((u16*)adjust)[1] = maskedAttr;
                        }

                        *append = *source++;
                        {
                            register u8 oldFlags PIN("r1");
                            u32 newFlags;
                            oldFlags = adjust[5];
                            newFlags = clearMask;
                            newFlags &= oldFlags;
                            adjust[5] = newFlags;
                        }
                        append += 2;
                        adjust += 8;
                        {
                            s32 decrement;
                            s32 test;
                            decrement = -1;
                            processed += decrement;
                            test = processed;
                            if (test == 0) {
                                break;
                            }
                        }
                    } while (1);
                    processed = slots;
                }
            }

            {
                u8* coords;
                GBA_PTR(const u8*)* animationPointer;
                register const u8* currentAnimation PIN("r4");
                register u32 timer PIN("r1");
                register u16 frame PIN("r0");

                coords = gMinigameCursorAnimationState;
                timer = *(u16*)(coords + 0);
                timer++;
                *(u16*)(coords + 0) = timer;
                frame = *(u16*)(coords + 2);
                animationPointer = (GBA_TYPEOF(const u8**, animationPointer))&gMinigameCursorAnimation;
                currentAnimation = GP(*animationPointer);
                {
                    u32 duration;
                    duration = ((const struct AnimationFrame*)currentAnimation)[frame].time;
                    timer = (u16)timer;
                    if (duration < timer) {
                        timer = 1;
                        *(u16*)(coords + 0) = timer;
                        frame = *(u16*)(coords + 2);
                        frame++;
                        *(u16*)(coords + 2) = frame;
                        frame = *(volatile u16*)(coords + 2);
                        if (((const struct AnimationFrame*)currentAnimation)[frame].time == 0) {
                            *(u16*)(coords + 2) = 0;
                            if (currentAnimation == (const u8*)sMinigameCursorConfirmAnimation) {
                                *(u16*)(coords + 2) = timer;
                            }
                        }
                    }
                }

                frame = *(u16*)(coords + 2);
                {
                    const u8* reloadedAnimation;
                    reloadedAnimation = GP(*animationPointer);
                    source = GP(((const struct AnimationFrame*)reloadedAnimation)[frame].oam);
                }
                slots += *source++;
                if (slots > 128) {
                    return;
                }

                if (processed < slots) {
                    u8* copyCoords;
                    u8* copyAdjust;
                    register u8* oamBase PIN("r1");
                    register u32 maskTemp PIN("r4");
                    register u32 attrMaskTemp PIN("r0");
                    s32 clearTemp;
                    register u32 xMask PIN("r10");
                    u32 attr1Mask;
                    s32 clearMask;
                    oamBase = (u8*)gOamBuffer;
                    copyCoords = coords;
                    maskTemp = 0x1FF;
                    ASM_BARRIER_VOLATILE(: "+r"(maskTemp));
                    xMask = maskTemp;
                    attrMaskTemp = 0xFFFFFE00;
                    attr1Mask = attrMaskTemp;
                    clearTemp = 13;
                    clearTemp = -clearTemp;
                    ASM_BARRIER_VOLATILE(: "+r"(clearTemp));
                    clearMask = clearTemp;
                    maskTemp = processed;
                    ASM_BARRIER_VOLATILE(: "+r"(maskTemp));
                    attrMaskTemp = maskTemp << 3;
                    copyAdjust = (u8*)((u32)attrMaskTemp + (u32)oamBase);
                    adjust = copyAdjust;
                    {
                        s32 previousProcessed;
                        previousProcessed = processed;
                        processed = slots - previousProcessed;
                    }
                    do {
                        attr = *source++;
                        *append++ = attr;
                        adjust[0] = attr + copyCoords[6];

                        attr = *source++;
                        *append++ = attr;
                        {
                            register s32 xOffset PIN("r1");
                            xOffset = *(s16*)(copyCoords + 4);
                            xOffset = attr + xOffset;
                            value = xOffset & xMask;
                        }
                        {
                            register u32 oldAttr PIN("r2");
                            u32 maskedAttr;
                            oldAttr = ((u16*)adjust)[1];
                            maskedAttr = attr1Mask;
                            maskedAttr &= oldAttr;
                            maskedAttr |= value;
                            ((u16*)adjust)[1] = maskedAttr;
                        }

                        *append = *source++;
                        {
                            u32 oldFlags;
                            u32 maskedFlags;
                            oldFlags = adjust[5];
                            maskedFlags = clearMask;
                            ASM_BARRIER_VOLATILE(: "+r"(maskedFlags));
                            maskedFlags &= oldFlags;
                            adjust[5] = maskedFlags;
                        }
                        append += 2;
                        adjust += 8;
                        {
                            s32 decrement;
                            s32 test;
                            decrement = -1;
                            processed += decrement;
                            test = processed;
                            if (test == 0) {
                                break;
                            }
                        }
                    } while (1);
                    processed = slots;
                }
            }
        }
    }

    {
        u32 index;
        register u32 r0v PIN("r0");
        register u32 r1v PIN("r1");
        register u32 r2v PIN("r2");
        u32 r3v;
        register u32 r4v PIN("r4");
        register u32 r6v PIN("r6");
        register u32 nextIndex PIN("r9");
        register u32 clearMask PIN("r10");

        r2v = 0;
        index = r2v;
    mainItemLoop:
        r4v = index;
        r2v = r4v << 1;
        r1v = r2v + r4v;
        r0v = r1v << 2;
        r4v = (u32)gMinigameMenuItems;
        r3v = r0v + r4v;
        r0v = *(u8*)(r3v + 8);
        r0v <<= 2;
        r6v = r1v << 3;
        r0v += r6v;
        r1v = (u32)sMinigameItemAnimationTable;
        r0v += r1v;
        r4v = *(u32*)r0v;

        r1v = *(u16*)(r3v + 0);
        r1v++;
        *(u16*)(r3v + 0) = r1v;
        r0v = *(u16*)(r3v + 2);
        r0v <<= 3;
        r0v += r4v;
        r0v = *(u8*)(r0v + 4);
        r1v = (u16)r1v;
        if (r0v < r1v) {
            r1v = 1;
            r0v = 1;
            *(u16*)(r3v + 0) = r0v;
            r0v = *(u16*)(r3v + 2);
            r0v++;
            *(u16*)(r3v + 2) = r0v;
            r0v = *(u16*)(r3v + 2);
            r0v <<= 3;
            r0v += r4v;
            r0v = *(u8*)(r0v + 4);
            if (r0v == 0) {
                *(u16*)(r3v + 2) = r0v;
                r0v = *(u8*)(r3v + 8);
                if (r0v == 3) {
                    r0v = 4;
                    *(u8*)(r3v + 8) = r0v;
                    r0v = r6v;
                    r0v += 16;
                    r4v = (u32)sMinigameItemAnimationTable;
                    r0v += r4v;
                    goto loadItemAnimation;
                }
                if (r0v == 5) {
                    *(u8*)(r3v + 8) = r1v;
                    r0v = r6v + 4;
                    r1v = (u32)sMinigameItemAnimationTable;
                    r0v += r1v;
                loadItemAnimation:
                    r4v = *(u32*)r0v;
                }
            }
        }

        r1v = index;
        r0v = r2v + r1v;
        r2v = r0v << 2;
        r1v = (u32)gMinigameMenuItems;
        r0v = r2v + r1v;
        r0v = *(u16*)(r0v + 2);
        r0v <<= 3;
        r0v += r4v;
        r3v = *(u32*)r0v;
        r0v = *(u16*)r3v;
        slots += r0v;
        r3v += 2;
        if (slots > 128) {
            return;
        }

        r4v = 1;
        ASM_BARRIER_VOLATILE(: "+r"(r4v));
        r4v += index;
        nextIndex = r4v;
        if (processed < slots) {
            r0v = r1v;
            ASM_BARRIER_VOLATILE(: "+r"(r0v));
            r6v = r2v + r0v;
            r1v = processed;
            ASM_BARRIER_VOLATILE(: "+r"(r1v));
            r0v = r1v << 3;
            r2v = (u32)gOamBuffer;
            r4v = r0v + r2v;
            r0v = 0x1FF;
            ASM_BARRIER_VOLATILE(: "+r"(r0v));
            index = r0v;
            r1v = -13;
            ASM_BARRIER_VOLATILE(: "+r"(r1v));
            clearMask = r1v;
            r2v = processed;
            r2v = slots - r2v;
            processed = r2v;
        itemCopyLoop:
            r2v = *(u16*)r3v;
            r3v += 2;
            *append++ = r2v;
            r0v = *(u8*)(r6v + 6);
            r0v += r2v;
            *(u8*)r4v = r0v;
            r2v = *(u16*)r3v;
            r3v += 2;
            *append++ = r2v;
            r0v = 4;
            r1v = *(s16*)(r6v + r0v);
            r1v = r2v + r1v;
            r2v = index;
            r1v &= r2v;
            r2v = *(u16*)(r4v + 2);
            r0v = 0xFFFFFE00;
            r0v &= r2v;
            r0v |= r1v;
            *(u16*)(r4v + 2) = r0v;
            r0v = *(u16*)r3v;
            *append = r0v;
            r3v += 2;
            r1v = *(u8*)(r4v + 5);
            r0v = clearMask;
            r0v &= r1v;
            r1v = 8;
            r0v |= r1v;
            *(u8*)(r4v + 5) = r0v;
            append += 2;
            r4v += 8;
            r0v = -1;
            processed += r0v;
            r1v = processed;
            if (r1v != 0) {
                goto itemCopyLoop;
            }
            processed = slots;
        }
        index = nextIndex;
        r2v = index;
        if ((s32)r2v <= 2) {
            goto mainItemLoop;
        }
    }

    {
        register u32 index PIN("r12");
        u32 r0v;
        register u32 r1v PIN("r1");
        register u32 r2v PIN("r2");
        u32 r3v;
        register u32 r4v PIN("r4");
        u32 r6v;
        u32 nextIndex;
        u32 statePointer;

        r2v = (u32)gMinigameSparkleAnimationState;
        r1v = *(u16*)(r2v + 0);
        r1v++;
        *(u16*)(r2v + 0) = r1v;
        r3v = (u32)sMinigameSparkleAnimation;
        r0v = *(u16*)(r2v + 2);
        r0v <<= 3;
        r0v += r3v;
        r0v = *(u8*)(r0v + 4);
        r1v = (u16)r1v;
        statePointer = r2v;
        if (r0v < r1v) {
            r0v = 1;
            *(u16*)(r2v + 0) = r0v;
            r0v = *(u16*)(r2v + 2);
            r0v++;
            *(u16*)(r2v + 2) = r0v;
            r0v = *(u16*)(r2v + 2);
            r0v <<= 3;
            r0v += r3v;
            r0v = *(u8*)(r0v + 4);
            if (r0v == 0) {
                *(u16*)(r2v + 2) = r0v;
            }
        }

        r4v = 0;
        ASM_BARRIER_VOLATILE(: "+r"(r4v));
        index = r4v;
    finalAnimationLoop:
        r1v = statePointer;
        r0v = *(u16*)(r1v + 2);
        r0v <<= 3;
        r2v = (u32)sMinigameSparkleAnimation;
        r0v += r2v;
        r3v = *(u32*)r0v;
        r0v = *(u16*)r3v;
        slots += r0v;
        r3v += 2;
        if (slots > 128) {
            return;
        }

        r4v = 1;
        ASM_BARRIER_VOLATILE(: "+r"(r4v));
        r4v += index;
        nextIndex = r4v;
        if (processed < slots) {
            r0v = (u32)sMinigameSparkleYOffsets;
            r2v = index;
            r1v = r2v << 2;
            r0v = r1v + r0v;
            r0v = *(u32*)r0v;
            index = r0v;
            r4v = processed;
            ASM_BARRIER_VOLATILE(: "+r"(r4v));
            r0v = r4v << 3;
            r2v = (u32)gOamBuffer;
            r4v = r0v + r2v;
            r0v = (u32)sMinigameSparkleXOffsets;
            r1v += r0v;
            r6v = *(u32*)r1v;
            r0v = processed;
            r0v = slots - r0v;
            processed = r0v;
        finalCopyLoop:
            r2v = *(u16*)r3v;
            r3v += 2;
            *append++ = r2v;
            r1v = index;
            r0v = r1v + r2v;
            *(u8*)r4v = r0v;
            r2v = *(u16*)r3v;
            r3v += 2;
            *append++ = r2v;
            r1v = r2v + r6v;
            r0v = 0x1FF;
            r1v &= r0v;
            r2v = *(u16*)(r4v + 2);
            r0v = 0xFFFFFE00;
            r0v &= r2v;
            r0v |= r1v;
            *(u16*)(r4v + 2) = r0v;
            r0v = *(u16*)r3v;
            *append = r0v;
            r3v += 2;
            r1v = *(u8*)(r4v + 5);
            r0v = 13;
            r0v = -r0v;
            r0v &= r1v;
            r1v = 8;
            r0v |= r1v;
            *(u8*)(r4v + 5) = r0v;
            append += 2;
            r4v += 8;
            r2v = 1;
            r2v = -r2v;
            processed += r2v;
            r0v = processed;
            if (r0v != 0) {
                goto finalCopyLoop;
            }
            processed = slots;
        }
        index = nextIndex;
        r1v = index;
        if ((s32)r1v <= 2) {
            goto finalAnimationLoop;
        }
    }

    {
        register u8* slotsPointer PIN("r2");
        slotsPointer = &gOamSlotsUsed;
        *slotsPointer = slots;
    }
}

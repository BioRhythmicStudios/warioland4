#include "sound_room.h"

#include "fixed_point.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "init_helpers.h"
#include "main.h"
#include "minigame.h"
#include "oam.h"
#include "score.h"
#include "stage_select.h"

extern s16 gSoundRoomTileWaveOffset;
extern s8 gSoundRoomMenuState;
extern s8 gSoundRoomSelectedStage;
extern s8 gSoundRoomSelectedPassage;
extern u16 gSoundRoomTrackAnimState[];
extern u16 gSoundRoomPreviewAnimState[];
extern s8 gSoundRoomPreviewRequest;
extern s8 gSoundRoomBonusTrackUnlocked;
extern s8 gSoundRoomMusicEnabled;
extern s8 gSoundRoomPitchOffset;
extern s8 gSoundRoomTempoOffset;
extern s16 gSoundRoomMessageTimer;
extern u16 gStageEntryMainSpriteState[];
extern u16 gStageEntrySequenceTimer;
extern u16 gStageEntryWindowLeft;
extern u16 gStageEntryWindowRight;
extern u16 gStageEntryWindowTop;
extern u16 gStageEntryWindowBottom;

extern const u8 sUnk_86B2888[];
extern const u8 sUnk_86B3088[];
extern const u8 sUnk_86B3888[];
extern const u8 sUnk_86B4088[];
extern const u8 sUnk_86B4288[];
extern const u8 sUnk_86B8A88[];
extern const u8 sUnk_86BF288[];
extern const u8 sUnk_86BF488[];
extern const u8 sUnk_86CB7C8[];
extern const u8 sUnk_86CB908[];
extern const u8 sUnk_86D3108[];
extern const u8 sUnk_86D3226[];
extern const u32 sUnk_863D920[];
extern const u32 sUnk_863D948[];
extern const u32 sUnk_863D8A0[];
extern const u32 sUnk_863D8E0[];
extern const u32 sUnk_863C694[][4];
extern const u32 sUnk_863C6D4[][4];
extern const u32 sUnk_863C714[];
extern const u32 sUnk_863C754[];
extern const u32 sUnk_863C794[];
extern GBA_PTR(const u16*) const sUnk_863C7D4[][4];
extern GBA_PTR(const u16*) const sUnk_863C814[][4];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C854[];
extern const u32 sUnk_863C85C[];
extern const u16 sUnk_863D85C[];
extern const u16 sUnk_863D97C[];
extern const s16 sUnk_863D984[];
extern const u16 sUnk_863D9A0[];
extern const struct AnimationFrame sUnk_8642860[];
extern const u16 sUnk_864261C[];
extern const u16 sUnk_864262A[];
extern const u16 sUnk_8642638[];
extern const u16 sUnk_86427A0[];
extern const u16 sUnk_86427BA[];
extern const struct AnimationFrame sUnk_8642820[];
extern const struct AnimationFrame sUnk_8642898[];

void Shop_DrawText(const u8* text, u32 destination, s32 length);

s32 UpdateSoundRoomTransition(void)
{
    s32 result;
    u8* step;

    step = &gStageEntrySequenceStep;
    switch (*step) {
        case 0:
            result = ExpandMinigameWindow();
            if (result != 0) {
                (*step)++;
            }
            break;
        case 1:
            result = UpdateSoundRoomMenu();
            if (result != 0) {
                (*step)++;
            }
            break;
        case 2:
            result = ContractMinigameWindow();
            if (result != 0) {
                return 1;
            }
            break;
    }
    return 0;
}

void InitSoundRoomScreen(void)
{
    vu32* dma;
    long long new_var;
    register u32 busy PIN("r2");
    s32 passedScoreCount;
    register s32 passage PIN("r1");
    s32 nextPassage;
    s32 minigame;
    s32* score;
    s16* bgReg;
    s8* clearCounter0;
    s8* clearCounter1;
    s8* clearCounter2;
    u16* clearPair;
    s32(*highScores)[4];
    s32 scoreThreshold;
    u32 scoreOffset;
    u32 dmaStatus;
    InitializeVideoMemory();
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)sUnk_86B4088;
    dma[1] = 0x05000000;
    dma[2] = 0x80000100;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0) {
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)sUnk_86B4088;
    dma[1] = 0x05000200;
    dma[2] = 0x80000100;
    dma[2];
    dmaStatus = dma[2];
    busy = 0x80000000;
    {
        u8* tileset0;
        u8* tileset1;
        u8* tilemap0;
        u8* tilemap1;
        u8* tilemap2;
        tileset0 = sUnk_86B8A88;
        tileset1 = sUnk_86B4288;
        tilemap0 = sUnk_86B2888;
        tilemap1 = sUnk_86B3088;
        tilemap2 = sUnk_86B3888;
        if (((s32)dmaStatus) < 0) {
            while ((dma[2] & busy) != 0) {
            }
        }
        *((vu16*)0x05000000) = 0;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tileset0;
        dma[1] = 0x06000000;
        dma[2] = 0x80003400;
        dma[2];
        busy = 0x80000000;
        while ((dma[2] & busy) != 0) {
        }

        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tileset1;
        dma[1] = 0x06011800;
        dma[2] = 0x80002400;
        dma[2];
        busy = 0x80000000;
        while ((dma[2] & busy) != 0) {
        }

        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tilemap0;
        dma[1] = 0x0600C000;
        dma[2] = 0x80000400;
        dma[2];
        busy = 0x80000000;
        while ((dma[2] & busy) != 0) {
        }

        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tilemap1;
        dma[1] = 0x0600D000;
        dma[2] = 0x80000400;
        dma[2];
        busy = 0x80000000;
        while ((dma[2] & busy) != 0) {
        }

        dma = (vu32*)0x040000D4;
        dma[0] = (u32)tilemap2;
        dma[1] = 0x0600E000;
        dma[2] = 0x80000400;
        dma[2];
    }
    bgReg = (s16*)0x04000008;
    bgReg[0] = 0x1800;
    bgReg++;
    bgReg[0] = 0x1A01;
    {
        register s16* nextBgReg PIN("r1");
        nextBgReg = bgReg;
        nextBgReg++;
        *(volatile s16*)nextBgReg = 0x1C02;
    }
    InitMinigameScoreDisplay();
    *((vu16*)0x04000054) = 0;
    *((vu16*)0x04000000) = 0x1700;
    gStageEntryWindowLeft = 0x78;
    gStageEntryWindowRight = 0x78;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 0xA0;
    *((vu16*)0x04000000) |= 0x2000;
    *((vu16*)0x04000048) = 0x3F;
    *((vu16*)0x0400004A) = 0;
    gStageEntrySequenceTimer = 0;
    gStageEntrySequenceStep = 0;
    gSoundRoomMessageTimer = 0;
    *((vu16*)0x04000012) = 0;
    *((vu16*)0x04000010) = 0;
    *((vu16*)0x04000016) = 0;
    *((vu16*)0x04000014) = 0;
    *((vu16*)0x0400001A) = 0;
    *((vu16*)0x04000018) = 0;
    gSoundRoomTileWaveOffset = 0;
    gSoundRoomMenuState = 0;
    gSoundRoomSelectedStage = 0;
    gSoundRoomSelectedPassage = 0;
    gStageEntryMainSpriteState[4] = 0x14;
    gStageEntryMainSpriteState[5] = 0x10;
    gStageEntryMainSpriteState[0] = 0;
    gStageEntryMainSpriteState[1] = 0;
    ((u16*)gSoundRoomTrackAnimState)[0] = 0;
    ((u16*)gSoundRoomTrackAnimState)[1] = 0;
    gSoundRoomPreviewRequest = 0;
    gSoundRoomBonusTrackUnlocked = 0;
    passedScoreCount = 0;
    passage = 0;
    clearCounter0 = &gSoundRoomMusicEnabled;
    clearCounter1 = &gSoundRoomPitchOffset;
    clearCounter2 = &gSoundRoomTempoOffset;
    clearPair = (u16*)gSoundRoomPreviewAnimState;
    highScores = gHighScoreTable;
    scoreThreshold = 0x3E7;
    do {
        scoreOffset = passage << 4;
        nextPassage = passage + 1;
        score = (s32*)(scoreOffset + (u32)highScores);
        minigame = 3;
        do {
            if ((*score) > scoreThreshold) {
                passedScoreCount++;
            }
            score++;
            minigame--;
        } while (minigame >= 0);
        passage = (new_var = nextPassage);
    } while (passage <= 5);
    if (passedScoreCount > 0x11) {
        gSoundRoomBonusTrackUnlocked = 1;
    }
    *clearCounter0 = 0;
    *clearCounter1 = 0;
    *clearCounter2 = 0;
    clearPair[0] = 0;
    clearPair[1] = 0;
    m4aSongNumStart(0x26A);
}

#ifndef PORTABLE
asm(".set $t, InitSoundRoomScreen + 0x240\n"
    ".set $d, InitSoundRoomScreen + 0x246\n");
#endif

void SoundRoomVBlank(void)
{
    vu16* window;
    register u32 soundValue PIN("r2");
    u16* row0;
    int new_var;
    u16* row1;
    s32 i;
    s32 random;
    s32 adjusted;
    u32 tileOffset;
    const u8* text;
    const u32* trackTable;
    new_var = 4;
    window = (vu16*)0x04000040;
    window[0] = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
    window += 2;
    window[0] = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;
    switch ((u8)gSoundRoomMenuState) {
        case 1:
            switch (gLanguage) {
                case 0:
                    text = sUnk_86D3226;
                    break;

                case 1:

                    text = sUnk_86D3108;
                    break;

                default:
                    return;
            }

            goto drawText;

        case 2: {
            register s32 li PIN("r5");
            u32 tileMask;
            u32 paletteBase;
            register u32 paletteLoad PIN("r0");
            soundValue = gMPlayMemAccArea[0];
            li = 1;
            tileMask = 0x0FFF;
            paletteLoad = 0x2000;
            ASM_BARRIER_VOLATILE(: "=r"(paletteBase) : "0"(paletteLoad));
            row0 = (u16*)0x0600C444;
            row1 = (u16*)0x0600C484;
            do {
                *row0 &= tileMask;
                *row0 += paletteBase;
                *row1 &= tileMask;
                *row1 += paletteBase;
                row0++;
                row1++;
                li++;
            } while (li <= 26);
            if (soundValue == 0) {
                gSoundRoomMessageTimer = soundValue;
                text = sUnk_86D3108;
                goto drawTrackText;
            }
            adjusted = soundValue;
            if (adjusted > 100) {
                gSoundRoomMessageTimer = 1000;
                switch (gLanguage) {
                    case 0:
                        trackTable = sUnk_863D948;
                        goto drawTrackName;
                    case 1:
                        trackTable = sUnk_863D920;
                        break;
                    default:
                        goto updateRandomTrack;
                }
            drawTrackName: {
                const u32* trackTableReg;
                u32 trackIndex;
                trackTableReg = trackTable;
                trackIndex = adjusted;
                trackIndex -= 101;
                text = (const u8*)trackTableReg[trackIndex];
            }
            drawTrackText:
                Shop_DrawText(text, 0x7800, 26);
                goto updateRandomTrack;
            }
            if (adjusted <= 0) {
                goto updateRandomTrack;
            }
            {
                u32 tileOffsetReg;
                u16* row0Reg;
                u16* row1Reg;

                tileOffsetReg = adjusted << 1;
                row0Reg = (u16*)(0x0600C442 + tileOffsetReg);
                *row0Reg = ((*row0Reg) & 0x0FFF) + 0xF000;
                row1Reg = (u16*)(0x0600C482 + tileOffsetReg);
                *row1Reg = ((*row1Reg) & 0x0FFF) + 0xF000;
            }
        updateRandomTrack:
            if ((gStageEntrySequenceTimer % 300) == 0) {
                {
                    s32 randomOriginal;
                    random = MinigameRandom();
                    randomOriginal = random;
                    if (randomOriginal < 0) {
                        random += 15;
                    }
                    LoadSoundRoomTrackGraphics(randomOriginal - ((random >> 4) << (new_var = 4)));
                }
            }

            return;
        }

        case 4:
            i = gStageEntrySequenceTimer % 600;
            if ((i == 0) || (i == 300)) {
                LoadRandomSoundRoomPreview();
            }
            break;

        default:
            break;
    }

    switch (gLanguage) {
        case 0: {
            const u32* languageTable;
            u32 languageIndex;
            u32 languageTemp;

            languageTable = sUnk_863D8E0;
            languageIndex = (u8)gSoundRoomSelectedStage;
            languageIndex <<= 2;
            languageTemp = (u8)gSoundRoomSelectedPassage;
            languageTemp <<= 4;
            languageIndex += languageTemp;
            languageIndex += (u32)languageTable;
            languageTemp = *(const u32*)languageIndex;
            text = (const u8*)languageTemp;
        }

        drawText:
            Shop_DrawText(text, 0x7800, 26);
            break;

        case 1: {
            const u32* languageTable;
            u32 languageIndex;
            register u32 languageTemp PIN("r0");

            languageTable = sUnk_863D8A0;
            languageIndex = (u8)gSoundRoomSelectedStage;
            languageIndex <<= 2;
            languageTemp = (u8)gSoundRoomSelectedPassage;
            languageTemp <<= 4;
            languageIndex += languageTemp;
            languageIndex += (u32)languageTable;
            languageTemp = *(const u32*)languageIndex;
            Shop_DrawText((const u8*)languageTemp, 0x7800, 26);
            break;
        }

        default:
            return;
    }
}

#ifndef PORTABLE
asm(".set $t, UpdateSoundRoomMenu + 0x258\n");
#endif

s32 UpdateSoundRoomMenu(void)
{
    u8* new_var5;
    u16 buttons;
    int new_var7;
    s32 selected;
    u32 passageOffset;
    int new_var2;
    s32 value;
    int i;
    s32 adjusted;
    int new_var12;
    unsigned long new_var10;
    short new_var8;
    u16* row0;
    int new_var9;
    u32 new_var4;
    u16* row1;
    switch ((u8)gSoundRoomMenuState) {
        case 0: {
            register u16* clearTarget PIN("r3");
            register u32 clearValue PIN("r2");
            clearTarget = &gSoundRoomMessageTimer;
            clearValue = 0;
            *clearTarget = clearValue;
        }
            if ((*((u8*)(&gSoundRoomBonusTrackUnlocked))) != 0) {
                buttons = gButtonsPressed;
                if ((buttons & 0x30) != 0) {
                    m4aSongNumStart(0x124);
                    gSoundRoomMenuState = 1;
                    gStageEntryMainSpriteState[4] = 60;
                    gStageEntryMainSpriteState[5] = 16;
                    goto checkSecret;
                }
                value = 0x80;
            } else {
                buttons = gButtonsPressed;
                value = 0x90;
            }
            if (((value & buttons) != 0) && (SelectFirstUnlockedSoundRoomTrack() != 0)) {
                m4aSongNumStart(0x124);
                gSoundRoomMenuState = 3;
                {
                    u16* spriteState;
                    register const u8* graphicsTable PIN("r2");
                    u8* stagePtr;
                    u8* passagePtr;
                    register u32 offset PIN("r1");
                    register u32 tableValue PIN("r0");
                    spriteState = gStageEntryMainSpriteState;
                    graphicsTable = (const u8*)sUnk_863C694;
                    stagePtr = (u8*)(&gSoundRoomSelectedStage);
                    offset = *stagePtr;
                    offset <<= 2;
                    passagePtr = (u8*)(&gSoundRoomSelectedPassage);
                    tableValue = *passagePtr;
                    tableValue <<= 4;
                    offset += tableValue;
                    offset += (u32)graphicsTable;
                    tableValue = *((const u32*)offset);
                    spriteState[4] = tableValue;
                    graphicsTable = (const u8*)sUnk_863C6D4;
                    tableValue = *stagePtr;
                    tableValue <<= 2;
                    offset = *passagePtr;
                    offset <<= 4;
                    tableValue += offset;
                    tableValue += (u32)graphicsTable;
                    tableValue = *((const u32*)tableValue);
                    spriteState[5] = tableValue;
                }
            }
        checkSecret:
            if ((gButtonsHeld & 0x34C) == 0x34C) {
                gSoundRoomBonusTrackUnlocked = 1;
            }

            buttons = gButtonsPressed;
            value = 3;
            goto checkExitButtons;

        case 3:
            gSoundRoomMessageTimer = 1000;
            MoveSoundRoomSelection();
            buttons = gButtonsPressed;
            if ((buttons & 1) != 0) {
                const u8* trackTable;
                u8* stagePtr;
                u8* passagePtr;
                s32 selectedTrack;
                register u32 offset PIN("r1");
                u32 temp;
                gStageEntrySequenceTimer = 0;
                trackTable = (const u8*)sUnk_863C714;
                stagePtr = (u8*)(&gSoundRoomSelectedStage);
                offset = *stagePtr;
                offset <<= 2;
                passagePtr = (u8*)(&gSoundRoomSelectedPassage);
                temp = *passagePtr;
                temp <<= 4;
                offset += temp;
                offset += (u32)trackTable;
                selectedTrack = *((const s32*)offset);
                m4aSongNumStart(0x120);
                temp = (u32)sUnk_863D85C;
                selectedTrack++;
                selectedTrack <<= 2;
                selectedTrack += temp;
                temp = *((const u16*)selectedTrack);
                m4aSongNumStart(temp);
                gSoundRoomMenuState = 4;
                gSoundRoomTrackAnimState[0] = 0;
                gSoundRoomTrackAnimState[1] = 0;
                temp = *stagePtr;
                temp <<= 2;
                offset = *passagePtr;
                offset <<= 4;
                temp += offset;
                temp += (u32)trackTable;
                temp = *((const s32*)temp);
                LoadSoundRoomTrackGraphics(temp);
                break;
            }
            value = 2;
            goto checkExitButtons;

        case 4: {
            u16* cycleOffset;
            gStageEntrySequenceTimer++;
            gSoundRoomMessageTimer = 1000;
            cycleOffset = &gSoundRoomTileWaveOffset;
            *cycleOffset = (*cycleOffset + 4) & 0xFF;
            if (((gButtonsPressed & 3) != 0) || (gMPlayMemAccArea[0] == 0x7F)) {
                u32 zero;
                zero = 0;
                gMPlayMemAccArea[0] = zero;
                m4aSongNumStart(0x26A);
                m4aSongNumStart(0x121);
                gSoundRoomMenuState = 3;
                *cycleOffset = zero;
            }
            break;
        }

        case 1:
            gSoundRoomMessageTimer = 1000;
            ;
            if ((gButtonsPressed & 0x30) != 0) {
                gSoundRoomMessageTimer = 0;
                m4aSongNumStart(0x124);
                gSoundRoomMenuState = 0;
                {
                    u16* spriteStateBase;
                    spriteStateBase = (u16*)0x03003C0C;
                    spriteStateBase[4] = 20;
                    spriteStateBase[5] = 16;
                }
            } else {
                new_var5 = (u8*)sUnk_863C6D4;
                if (((gButtonsPressed & 0x80) != 0) && (SelectFirstUnlockedSoundRoomTrack() != 0)) {
                    m4aSongNumStart(0x124);
                    gSoundRoomMenuState = 3;
                    {
                        u16* spriteState;
                        register const u8* graphicsTable PIN("r2");
                        u8* stagePtr;
                        u8* passagePtr;
                        register u32 offset PIN("r1");
                        register u32 tableValue PIN("r0");
                        spriteState = gStageEntryMainSpriteState;
                        graphicsTable = (const u8*)sUnk_863C694;
                        stagePtr = (u8*)(&gSoundRoomSelectedStage);
                        offset = *stagePtr;
                        offset <<= 2;
                        passagePtr = (u8*)(&gSoundRoomSelectedPassage);
                        tableValue = *passagePtr;
                        tableValue <<= 4;
                        offset += tableValue;
                        offset += (u32)graphicsTable;
                        tableValue = *((const u32*)offset);
                        spriteState[4] = tableValue;
                        graphicsTable = (const u8*)sUnk_863C6D4;
                        tableValue = *stagePtr;
                        tableValue <<= 2;
                        offset = *passagePtr;
                        offset <<= 4;
                        tableValue += offset;
                        tableValue += (u32)graphicsTable;
                        tableValue = *((const u32*)tableValue);
                        spriteState[5] = tableValue;
                    }
                }
            }
            if (((new_var12 = gButtonsPressed) & 1) != 0) {
                new_var7 = MinigameRandom();
                value = new_var7;
                adjusted = value;
                if (value < 0) {
                    adjusted += 15;
                }
                LoadSoundRoomTrackGraphics(value - ((adjusted >> 4) << 4));
                gStageEntrySequenceTimer = 0;
                gSoundRoomMessageTimer = 0;
                m4aSongNumStart(0x120);
                m4aSongNumStart(0x2D9);
                {
                    const u8* languageSongTable;
                    u32 languageSong;
                    languageSongTable = (const u8*)sUnk_863D97C;
                    languageSong = gLanguage;
                    languageSong <<= 2;
                    languageSong += (u32)languageSongTable;
                    languageSong = *((const u16*)languageSong);
                    m4aSongNumStart(languageSong);
                }
                gSoundRoomMenuState = 2;
                break;
            }
            buttons = gButtonsPressed;
            value = 2;

        checkExitButtons:
            value &= buttons;
            if (value != 0) {
                m4aSongNumStart(0x129);
                return 1;
            }
            break;

        case 2:
            gStageEntrySequenceTimer++;
            if ((gButtonsPressed & 0x10) != 0) {
                gSoundRoomTempoOffset++;
            }
            if ((gButtonsPressed & 0x20) != 0) {
                gSoundRoomTempoOffset--;
            }
            if ((gButtonsPressed & 0x40) != 0) {
                gSoundRoomPitchOffset++;
            }
            if ((gButtonsPressed & 0x80) != 0) {
                gSoundRoomPitchOffset--;
            }
            if ((gButtonsPressed & 0x300) != 0) {
                gSoundRoomMusicEnabled = gSoundRoomMusicEnabled ^ 1;
                m4aSongNumStart(0x11D);
            }
            if (gSoundRoomTempoOffset > 3) {
                gSoundRoomTempoOffset = 3;
            }
            if (gSoundRoomTempoOffset < (-3)) {
                gSoundRoomTempoOffset = -3;
            }
            if (gSoundRoomPitchOffset > 3) {
                gSoundRoomPitchOffset = 3;
            }
            if (gSoundRoomPitchOffset < (-3)) {
                gSoundRoomPitchOffset = -(new_var7 = 3);
            }
            gSoundRoomPreviewAnimState[0]++;
            if (sUnk_8642860[gSoundRoomPreviewAnimState[1]].time < gSoundRoomPreviewAnimState[0]) {
                gSoundRoomPreviewAnimState[0] = 0;
                gSoundRoomPreviewAnimState[1]++;
                if (sUnk_8642860[gSoundRoomPreviewAnimState[1]].time == 0) {
                    gSoundRoomPreviewAnimState[1] = 0;
                }
            }
            if (((gButtonsPressed & 3) != 0) || (gMPlayMemAccArea[0] == 0x7F)) {
                m4aSongNumStop(0x2D9);
                gSoundRoomMessageTimer = 0;
                gMPlayMemAccArea[0] = 0;
                m4aSongNumStart(0x26A);
                m4aSongNumStart(0x121);
                gSoundRoomMenuState = 1;
                gSoundRoomTempoOffset = 0;
                gSoundRoomPitchOffset = 0;
                gSoundRoomMusicEnabled = 0;
                {
                    int tileIndex;
                    register unsigned long paletteBits PIN("r5");
                    register int paletteTemp PIN("r2");
                    tileIndex = 1;
                    new_var4 = 0x0FFF;
                    paletteTemp = 0x2000;
                    ASM_BARRIER_VOLATILE(: : "r"(paletteTemp));
                    paletteBits = paletteTemp;
                    ASM_BARRIER_VOLATILE(: "+r"(tileIndex), "+r"(new_var4), "+r"(paletteBits));
                    row0 = (u16*)0x0600C444;
                    row1 = (u16*)0x0600C484;
                    do {
                        {
                            u32 maskedTile;
                            register u16 tileValue PIN("r1");
                            tileValue = *row0;
                            maskedTile = new_var4;
                            maskedTile &= tileValue;
                            maskedTile += paletteBits;
                            *row0 = maskedTile;
                            tileValue = *row1;
                            maskedTile = new_var4;
                            maskedTile &= tileValue;
                            maskedTile += paletteBits;
                            *row1 = maskedTile;
                        }
                        row0++;
                        row1++;
                        tileIndex++;
                    } while (tileIndex <= 26);
                }

                gSoundRoomPreviewAnimState[0] = 0;
                gSoundRoomPreviewAnimState[1] = 0;
            }
            UpdateSoundRoomAudioEffects();
            break;
    }

    return 0;
}

s32 SelectFirstUnlockedSoundRoomTrack(void)
{
    u8* selectedPassage;
    u8* selectedStage;
    s32 passage;
    s32 nextPassage;
    s32 stage;
    u8* collection;
    selectedPassage = &gSoundRoomSelectedPassage;
    if ((*selectedPassage) != 0) {
        goto returnTrue;
    }
    {
        u8* stagePtr;
        stagePtr = &gSoundRoomSelectedStage;
        if ((*stagePtr) == 0) {
            selectedStage = stagePtr;
            goto search;
        }
        selectedStage = stagePtr;
    }
    goto returnTrue;
found:
    *(&gSoundRoomSelectedPassage) = passage;

    *selectedStage = stage;
returnTrue:
    return 1;

search:
    passage = 0;

    collection = (u8*)gCurrentCollection;
    do {
        stage = 0;
        nextPassage = passage + 1;
        collection = ((u8*)gCurrentCollection) + ((nextPassage * 3) << 3);
        do {
            if (((s32)((*collection) << 27)) < 0) {
                goto found;
            }
            collection += 4;
            stage++;
        } while (stage <= 3);
        passage = nextPassage;
    } while (passage <= 3);
    return 0;
}

s32 MoveSoundRoomSelection(void)
{
    s32 currentItem;
    register u16 buttons PIN("r2");
    const u32* new_var;
    u8* new_var3;
    const u32* selectionTable;
    s32 direction;
    register const u32* neighbors PIN("r1");
    const u32* list;
    register const u32* listCursor PIN("r5");
    const u32* passageTable;
    s32 candidate;
    s32 passage;
    short new_var2;
    s32 stage;
    const u8* collection;
    s8* new_var4;
    selectionTable = sUnk_863C714;
    currentItem =
        *((s32*)((((*((u8*)(&gSoundRoomSelectedStage))) << 2) + ((*((u8*)(&gSoundRoomSelectedPassage))) << 4)) +
                 ((s32)selectionTable)));
    buttons = gButtonsPressed;
    direction = ((buttons & 0x40) != 0) ? (4) : (0);
    if ((buttons & 0x10) != 0) {
        direction = 1;
    }
    if ((buttons & 0x80) != 0) {
        direction = 3;
    }
    if ((buttons & 0x20) != 0) {
        direction = 2;
    }
    if (direction != 0) {
        {
            register const u8* neighborsBase PIN("r2");
            s32 directionOffset;
            neighborsBase = sUnk_863C85C;
            currentItem <<= 6;
            directionOffset = (direction - 1) << 10;
            currentItem += directionOffset;
            currentItem += (s32)neighborsBase;
            neighbors = (const u32*)currentItem;
        }
        if ((*neighbors) != 99) {
            passageTable = sUnk_863C754;
            collection = (const u8*)gCurrentCollection;
            list = neighbors;
            listCursor = list;
            do {
                candidate = *listCursor;
                stage = passageTable[candidate];
                {
                    register const u32* stageTable PIN("r0");
                    stageTable = sUnk_863C794;
                    passage = stageTable[candidate];
                }
                {
                    s32 entryOffset;
                    entryOffset = stage << 2;
                    entryOffset += ((passage + 1) * 3) << 3;
                    entryOffset += (s32)collection;
                    new_var2 = ((s32)(((u32)(*((u8*)entryOffset))) << 27)) < 0;
                }
                if (new_var2) {
                    goto found;
                }
                list++;
                listCursor++;
            } while ((*list) != 99);
        }
        if (direction == 4) {
            gSoundRoomMenuState = 0;
            m4aSongNumStart(0x124);
            gStageEntryMainSpriteState[4] = 20;
            gStageEntryMainSpriteState[5] = 16;
        }
    }
    return 0;
found:
    m4aSongNumStart(0x124);

    {
        u8* passagePtr;
        u8* stagePtr;
        u16* spriteState;
        register const u8* table PIN("r4");
        register u32 stageOffset PIN("r0");
        register u32 passageOffset PIN("r1");
        u32 zero;

        passagePtr = &gSoundRoomSelectedPassage;
        *passagePtr = passage;
        stagePtr = &gSoundRoomSelectedStage;
        *stagePtr = stage;
        spriteState = gStageEntryMainSpriteState;
        table = (const u8*)sUnk_863C694;
        stageOffset = *stagePtr;
        stageOffset <<= 2;
        passageOffset = *passagePtr;
        passageOffset <<= 4;
        stageOffset += passageOffset;
        stageOffset += (u32)table;
        stageOffset = *((const u32*)stageOffset);
        zero = 0;
        spriteState[4] = stageOffset;
        table = (const u8*)sUnk_863C6D4;
        stageOffset = *stagePtr;
        stageOffset <<= 2;
        passageOffset = *passagePtr;
        passageOffset <<= 4;
        stageOffset += passageOffset;
        stageOffset += (u32)table;
        stageOffset = *((const u32*)stageOffset);
        spriteState[5] = stageOffset;
        gSoundRoomMessageTimer = zero;
    }
    return 0;
}

void UpdateSoundRoomAudioEffects(void)
{
    u16 songId;
    u16 pairedSongId;
    struct MusicPlayerInfo* firstPlayer;
    struct MusicPlayerInfo* secondPlayer;
    u8* new_var;
    u16* new_var2;
    u16 tempo;
    s16 pitch;
    const u16* new_var3;
    const u32* songIndexTable;
    u32 pitchMask;
    const s16* pitchTable;
    s8* pitchAdjustPtr;
    new_var = (u8*)gSongTable;
    songId = *((u16*)(new_var + 0x16CC));
    songIndexTable = (const u32*)sUnk_863D97C;
    pairedSongId = ((const struct Song*)gSongTable)[songIndexTable[gLanguage]].ms;
    if (*(u8*)&gSoundRoomMusicEnabled != 0) {
        m4aMPlayVolumeControl(GP(gMPlayTable[songId].info), 0xFFFF, 0x100);
    } else {
        m4aMPlayVolumeControl(GP(gMPlayTable[songId].info), 0xFFFF, 0);
    }
    firstPlayer = GP(gMPlayTable[songId].info);
    new_var3 = sUnk_863D9A0;
    new_var2 = &(*((u16*)(((u8*)new_var3) + ((((s8)gSoundRoomTempoOffset) + 3) << 2))));
    tempo = *new_var2;
    m4aMPlayTempoControl(firstPlayer, tempo);
    songId = 2;
    secondPlayer = GP(gMPlayTable[pairedSongId].info);
    tempo = *((u16*)(((u8*)sUnk_863D9A0) + ((((s8)gSoundRoomTempoOffset) + 3) << songId)));
    m4aMPlayTempoControl(secondPlayer, tempo);
    pitchMask = 0xFFFF;
    pitchTable = sUnk_863D984;
    pitchAdjustPtr = &gSoundRoomPitchOffset;
    pitch = *((s16*)(((u8*)pitchTable) + (((*pitchAdjustPtr) + 3) << 2)));
    m4aMPlayPitchControl(firstPlayer, pitchMask, pitch);
    pitch = *((s16*)(((u8*)pitchTable) + (((*pitchAdjustPtr) + 3) << 2)));
    m4aMPlayPitchControl(secondPlayer, pitchMask, pitch);
}

void LoadSoundRoomTrackGraphics(s32 index)
{
    int new_var;
    vu32* dma;
    s32 row;
    s32 column;
    s32 baseOffset;
    s32 tileOffset;
    s32 i;
    u32 busy;

    row = (index + ((s32)(((u32)index) >> 31))) >> 1;
    baseOffset = (row * 3) << 11;
    column = index - (row << 1);
    tileOffset = (column * 3) << 7;

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)(sUnk_86BF288 + (index << 5));
    dma[1] = 0x050003E0;
    dma[2] = 0x80000010;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0) {
    }

    {
        vu32* dma3;
        i = 0;
        dma3 = (vu32*)0x040000D4;
        do {
            dma3[0] = (u32)(((sUnk_86BF488 + baseOffset) + tileOffset) + (i << 10));
            dma3[1] = 0x06016000 + (i << 10);
            dma3[2] = 0x800000C0;
            dma3[2];
            ;
            while ((dma3[2] & 0x80000000) != (new_var = 0)) {
            }
            i++;
        } while (i <= 5);
    }
}

void DrawSoundRoomSprites(void)
{
    /* The shared OamData merges attr1 bits 9..13 into one 5-bit matrixNum. The
       selected track icon only clears the 3-bit affine index the hardware reads
       in affine mode, leaving the two flip bits alone (as in RenderVortexCutsceneOam). */
    typedef union AGB_ALIGN {
        struct AGB_ALIGN {
            u32 y : 8;
            u32 affineMode : 2;
            u32 objMode : 2;
            u32 mosaic : 1;
            u32 bpp : 1;
            u32 shape : 2;

            u32 x : 9;
            u32 affineIndex : 3;
            u32 preserved : 2;
            u32 size : 2;

            u16 tileNum : 10;
            u16 priority : 2;
            u16 paletteNum : 4;

            u16 fractional : 8;
            u16 integer : 7;
            u16 sign : 1;
        } split;

        struct AGB_ALIGN {
            u16 attr0;
            u16 attr1;
            u16 attr2;
            u16 affineParam;
        } all;
    } SoundRoomOamData;

    const struct AnimationFrame* animation;
    const u16* src;
    u16* dest;
    s32 currentSlot;
    s32 nextSlot;
    u16 attr;
    u16 timer;
    s32 cycle;
    u16 matrixD;
    u16 matrix[4];
    u16* matrix1Ptr;
    u16* matrix2Ptr;
    u16* matrix3Ptr;
    s32 passage;
    s32 stage;

    currentSlot = 0;
    nextSlot = gOamSlotsUsed;
    dest = (u16*)&gOamBuffer[nextSlot];

    if ((u8)gSoundRoomMenuState != 4 && (u8)gSoundRoomMenuState != 2) {
        gStageEntryMainSpriteState[0]++;
        if (sUnk_8642898[gStageEntryMainSpriteState[1]].time < gStageEntryMainSpriteState[0]) {
            gStageEntryMainSpriteState[0] = 0;
            gStageEntryMainSpriteState[1]++;
            if (sUnk_8642898[gStageEntryMainSpriteState[1]].time == 0) {
                gStageEntryMainSpriteState[1] = 0;
            }
        }
        src = GP(sUnk_8642898[gStageEntryMainSpriteState[1]].oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + gStageEntryMainSpriteState[5];
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + (s16)gStageEntryMainSpriteState[4]) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    if (*(u8*)&gSoundRoomBonusTrackUnlocked != 0) {
        src = GP(sUnk_8642860[gSoundRoomPreviewAnimState[1]].oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 8;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 56) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    for (passage = 0; passage < 4; passage++) {
        for (stage = 0; stage < 4; stage++) {
            if (passage == (u8)gSoundRoomSelectedPassage && stage == (u8)gSoundRoomSelectedStage &&
                (u8)gSoundRoomMenuState > 2) {
                src = GP(sUnk_863C7D4[passage][stage]);
                nextSlot += *(src++);
                for (; currentSlot < nextSlot; currentSlot++) {
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.y = sUnk_863C6D4[passage][stage] + attr - 8;
                    gOamBuffer[currentSlot].split.affineMode = 3;
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.x = (attr + sUnk_863C694[passage][stage] - 8) & 0x1FF;
                    ((SoundRoomOamData*)gOamBuffer)[currentSlot].split.affineIndex = 0;
                    *(dest++) = *(src++);
                    gOamBuffer[currentSlot].split.priority = 0;
                    dest++;
                }
            } else if (gCurrentCollection[passage + 1][stage].cd) {
                src = GP(sUnk_863C7D4[passage][stage]);
                nextSlot += *(src++);
                for (; currentSlot < nextSlot; currentSlot++) {
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.y = sUnk_863C6D4[passage][stage] + attr;
                    attr = *(src++);
                    *(dest++) = attr;
                    gOamBuffer[currentSlot].split.x = (attr + sUnk_863C694[passage][stage]) & 0x1FF;
                    *(dest++) = *(src++);
                    gOamBuffer[currentSlot].split.priority = 0;
                    dest++;
                }
            }
        }
    }

    if ((u8)gSoundRoomMenuState > 2) {
        src = GP(sUnk_863C814[(u8)gSoundRoomSelectedPassage][(u8)gSoundRoomSelectedStage]);
    } else if (*(u8*)&gSoundRoomMusicEnabled != 0) {
        if (gMPlayMemAccArea[0] % 2 == 1 && gMPlayMemAccArea[0] <= 99) {
            src = sUnk_864262A;
        } else {
            src = sUnk_8642638;
        }
    } else {
        src = sUnk_864261C;
    }
    if (*(u8*)&gSoundRoomMenuState != 0) {
        nextSlot += *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 88;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 160) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    if ((u8)gSoundRoomMenuState == 2) {
        timer = gStageEntrySequenceTimer % 7;
        if (timer == 0) {
            if (MinigameRandom() % 10 < 5) {
                gSoundRoomPreviewRequest = 1;
            } else {
                gSoundRoomPreviewRequest = 0;
            }
        }
        if (*(u8*)&gSoundRoomPreviewRequest == 0) {
            src = sUnk_86427A0;
        }
        if (*(u8*)&gSoundRoomPreviewRequest == 1) {
            src = sUnk_86427BA;
        }
        nextSlot += *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 16;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 160) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    } else if ((u8)gSoundRoomMenuState == 4) {
        if (gStageEntrySequenceTimer % 1200 < 300) {
            timer = gStageEntrySequenceTimer % 7;
            if (timer == 0) {
                if (MinigameRandom() % 10 < 5) {
                    gSoundRoomPreviewRequest = 1;
                } else {
                    gSoundRoomPreviewRequest = 0;
                }
            }
            if (*(u8*)&gSoundRoomPreviewRequest == 0) {
                src = sUnk_86427A0;
            }
            if (*(u8*)&gSoundRoomPreviewRequest == 1) {
                src = sUnk_86427BA;
            }
            nextSlot += *(src++);
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 16;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + 160) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        } else {
            cycle = gStageEntrySequenceTimer % 600;
            if (cycle == 0 || cycle == 300) {
                gSoundRoomTrackAnimState[0] = 1;
                gSoundRoomTrackAnimState[1] = 0;
            }
            if (cycle < 300) {
                animation = GP(sUnk_863C854[0]);
            } else {
                animation = GP(sUnk_863C854[1]);
            }
            gSoundRoomTrackAnimState[0]++;
            if (animation[gSoundRoomTrackAnimState[1]].time < gSoundRoomTrackAnimState[0]) {
                gSoundRoomTrackAnimState[0] = 0;
                gSoundRoomTrackAnimState[1]++;
                if (animation[gSoundRoomTrackAnimState[1]].time == 0) {
                    gSoundRoomTrackAnimState[1] = 0;
                }
            }
            src = GP(animation[gSoundRoomTrackAnimState[1]].oam);
            nextSlot += *(src++);
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 16;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + 160) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    } else {
        animation = sUnk_8642820;
        gSoundRoomTrackAnimState[0]++;
        if (animation[gSoundRoomTrackAnimState[1]].time < gSoundRoomTrackAnimState[0]) {
            gSoundRoomTrackAnimState[0] = 1;
            gSoundRoomTrackAnimState[1]++;
            if (animation[gSoundRoomTrackAnimState[1]].time == 0) {
                gSoundRoomTrackAnimState[1] = 0;
            }
        }
        src = GP(animation[gSoundRoomTrackAnimState[1]].oam);
        nextSlot += *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 16;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 160) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    matrix[0] = FixedMul(sSinCosTable[(u16)gSoundRoomTileWaveOffset + 64], (s16)FixedInverse(384));
    *(matrix1Ptr = &matrix[1]) = FixedMul(sSinCosTable[(u16)gSoundRoomTileWaveOffset], (s16)FixedInverse(384));
    *(matrix2Ptr = &matrix[2]) =
        FixedMul((s16)(-((u16)sSinCosTable[(u16)gSoundRoomTileWaveOffset])), (s16)FixedInverse(384));
    matrixD = FixedMul(sSinCosTable[(u16)gSoundRoomTileWaveOffset + 64], (s16)FixedInverse(384));
    *(matrix3Ptr = &matrix[3]) = matrixD;
    gOamBuffer[0].all.affineParam = matrix[0];
    gOamBuffer[1].all.affineParam = *matrix1Ptr;
    gOamBuffer[2].all.affineParam = *matrix2Ptr;
    gOamBuffer[3].all.affineParam = matrixD;
    gOamSlotsUsed = nextSlot;
}

void LoadRandomSoundRoomPreview(void)
{
    vu32* dma;
    register s32 index PIN("r3");
    u32 busy;

    index = MinigameRandom() % 5;
    dma = (vu32*)0x040000D4;
    dma[0] = (u32)(sUnk_86CB7C8 + (index << 6));
    dma[1] = 0x050003A0;
    dma[2] = 0x80000020;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0) {
    }

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)(sUnk_86CB908 + ((index * 3) << 11));
    dma[1] = 0x06010000;
    dma[2] = 0x80000C00;
    dma[2];
}

#include "color_effects.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "init_helpers.h"
#include "input.h"
#include "interrupt_callbacks.h"
#include "main.h"
#include "pause.h"
#include "score.h"
#include "stage_entry.h"
#include "stage_select.h"

struct AGB_ALIGN DmaRegisters {
    vu32 src;
    vu32 dst;
    vu32 cnt;
};

extern u8 gLanguage;
extern u8 gCurrentStageNumber;
extern u8 gCurrentPassage;
extern GBA_PTR(const u8*) const sFileSelectPassageNameTableLanguage0[][6];
extern GBA_PTR(const u8*) const sFileSelectPassageNameTableLanguage1[][6];
extern void Shop_DrawText(const u8* str, u32 tileBase, u32 param);
extern void m4aMPlayContinue(struct MusicPlayerInfo* mplayInfo);
extern u16 gUnk_3000000;
extern const struct AnimationFrame sUnk_86D3F58[];
extern const u8 sUnk_86DF1F8[];
extern const u8 sUnk_86D9638[];
extern u16 gSoundRoomMessageTimer;
extern const u8 sUnk_86D4038[];
extern const u8 sUnk_86D6838[];
extern const u8 sUnk_86D6E78[];
extern const u8 sUnk_86D7038[];
extern const u8 sUnk_86D7238[];
extern const u8 sUnk_86DCE38[];
extern const u8 sUnk_86DD638[];
extern const u8 sUnk_86DE638[];
extern const u8 sUnk_86DEE38[];
extern const u8 sUnk_86DEEF8[];
extern const u8 sUnk_86DF0F8[];
extern const u8 sUnk_86E29F8[];
extern const struct AnimationFrame sUnk_86D3F40[];
extern const s32 sUnk_86D36A4[];
extern const s32 sUnk_86D36B8[];
extern const s32 sUnk_86D36CC[];
extern const s32 sUnk_86D36D4[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_86D36DC[][4];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_86D36FC[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_86D370C[2][2];
extern const struct AnimationFrame sUnk_86D3CF8[];
extern const struct AnimationFrame sUnk_86D3D08[];
extern const struct AnimationFrame sUnk_86D3DB0[];
extern const struct AnimationFrame sUnk_86D3F80[];
extern const struct AnimationFrame sUnk_86D3FD0[];
extern const struct AnimationFrame sUnk_86D4008[];
extern const struct AnimationFrame sUnk_86D4018[];
extern const struct AnimationFrame sUnk_86D4028[];

u32 PauseScreenSubroutine(void)
{
    u32 done;
    u32 result;

    SetPauseScreenVBlankCallback();
    gOamSlotsUsed = 0;
    done = 0;

    switch ((s16)gUnk_3000000) {
        case 0:
            InitializePauseScreen();
            gUnk_3000000++;
            break;
        case 1:
            UploadColorFadePalettes();
            if (UpdatePaletteFadeStep(0, 1)) {
                gUnk_3000000++;
            }
            break;
        case 2:
            if (OpenPauseScreenWindow()) {
                gUnk_3000000++;
            }
            break;
        case 3:
            if (UpdatePauseScreenSelection()) {
                gUnk_3000000++;
            }
            break;
        case 4:
            result = gStageEntrySequenceTimer + 1;
            gStageEntrySequenceTimer = result;
            if ((u16)result > 30) {
                gStageEntrySequenceTimer = result + 1;
                gUnk_3000000++;
            }
            break;
        case 5:
            if (ClosePauseScreenWindow()) {
                gUnk_3000000++;
            }
            break;
        case 6:
            UploadColorFadePalettes();
            if (UpdatePaletteFadeStep(1, 1)) {
                *(vu16*)0x0400004A = 63;
                gUnk_3000000 = 0;
                if (gPauseScreenState.confirmedSelection == 0) {
                    m4aMPlayContinue(GP(gMPlayTable[3].info));
                    m4aMPlayContinue(GP(gMPlayTable[4].info));
                    m4aMPlayContinue(GP(gMPlayTable[5].info));
                    m4aMPlayContinue(GP(gMPlayTable[6].info));
                }
                done = 1;
            }
            break;
        case 7: {
            u32 result_r0;
            result_r0 = UpdatePauseScreenConfirmation();
            if (result_r0 == 1) {
                gUnk_3000000 = 4;
                GP_SET(gPauseScreenAnimation, sUnk_86D3F58);
                gPauseScreenState.animationFrame = 0;
                gPauseScreenState.animationTimer = 0;
                if (gPauseScreenState.confirmedSelection == 1) {
                    gPauseFlag = 3;
                    gMainGameMode++;
                }
                if (gPauseScreenState.confirmedSelection == 2) {
                    gPauseFlag = 2;
                }
            } else if (result_r0 == 2) {
                gPauseScreenState.selection = gPauseScreenState.confirmedSelection;
                gUnk_3000000 = 3;
            }
        } break;
    }

    RenderPauseScreenOam();
    ResetFreeOam();
    return done;
}

void SetPauseScreenVBlankCallback(void)
{
    InterruptCallbackSetVBlank(PauseScreenVBlank);
}

void PauseScreenVBlank(void)
{
    struct DmaRegisters* dma;

    m4aSoundVSync();
    dma = (struct DmaRegisters*)0x040000D4;
    dma->src = (u32)gOamBuffer;
    dma->dst = 0x07000000;
    dma->cnt = 0x84000100;
    dma->cnt;

    *(vu16*)0x04000044 = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;
    *(vu16*)0x04000040 = 0xF0;

    switch (gLanguage) {
        case 0:
            Shop_DrawText(GP(sFileSelectPassageNameTableLanguage0[gCurrentPassage][gCurrentStageNumber]), 0x7800, 26);
            break;
        case 1:
            Shop_DrawText(GP(sFileSelectPassageNameTableLanguage1[gCurrentPassage][gCurrentStageNumber]), 0x7800, 26);
            break;
    }
}

#define PAUSE_DMA_TRANSFER(src_, dst_, cnt_) do { \
    ((struct DmaRegisters *)0x040000D4)->src = (u32)(src_); \
    ((struct DmaRegisters *)0x040000D4)->dst = (u32)(dst_); \
    ((struct DmaRegisters *)0x040000D4)->cnt = (cnt_); \
    dmaStatus = dma->cnt; \
    dmaStatus = dma->cnt; \
    dmaMask = 0x80000000; \
    if ((s32)dmaStatus < 0) { \
        do { \
            dmaStatus = dma->cnt & dmaMask; \
        } while (dmaStatus != 0); \
    } \
} while (0)

#define PAUSE_DMA_TRANSFER_DMA(src_, dst_, cnt_) do { \
    dma->src = (u32)(src_); \
    dma->dst = (u32)(dst_); \
    dma->cnt = (cnt_); \
    dmaStatus = dma->cnt; \
    dmaStatus = dma->cnt; \
    dmaMask = 0x80000000; \
    if ((s32)dmaStatus < 0) { \
        do { \
            dmaStatus = dma->cnt & dmaMask; \
        } while (dmaStatus != 0); \
    } \
} while (0)

#define PAUSE_DMA_TRANSFER_R1(src_, dst_, cnt_) do { \
    register struct DmaRegisters *localDma; \
    localDma = (struct DmaRegisters *)0x040000D4; \
    localDma->src = (u32)(src_); \
    localDma->dst = (u32)(dst_); \
    localDma->cnt = (cnt_); \
    dmaStatus = localDma->cnt; \
    dmaStatus = localDma->cnt; \
    dmaMask = 0x80000000; \
    if ((s32)dmaStatus < 0) { \
        do { \
            dmaStatus = localDma->cnt & dmaMask; \
        } while (dmaStatus != 0); \
    } \
} while (0)

void InitializePauseScreen(void)
{
    struct DmaRegisters* dma;
    s32 i;
    u32 dmaStatus;

    register u32 dmaMask PIN("r2");
    u8* sideAddress;
    register const u8* pausePaletteBase PIN("r6");
    register u8* pauseSidePtr PIN("r8");

    register const u8* tilesBase PIN("sl");
    const u8* early7038;
    const u8* early7038Plus;
    const u8* early4038;
    const u8* sideTiles;
    struct DmaRegisters* sideDma;
    const u8* languageTiles;
    const u8* languagePtr;
    struct DmaRegisters* languageDma;

    *(vu16*)0x04000200 ^= 1;
    *(vu16*)0x04000208 = 0;
    *(vu16*)0x04000004 &= 0xFFEF;
    *(vu16*)0x04000200 &= 0xFFFD;
    *(vu16*)0x04000208 = 1;

    if (gCurrentPassage == 0 || gCurrentPassage == 5) {
        sideAddress = &gUnk_3003C4A;
        *sideAddress = 1;
    } else {
        sideAddress = &gUnk_3003C4A;
        *sideAddress = 0;
    }
    pauseSidePtr = sideAddress;

    dma = (struct DmaRegisters*)0x040000D4;
    do {
        dma->src = (u32)((const void*)0x02020600);
        dma->dst = (u32)((void*)0x05000200);
        dma->cnt = 0x80000100;
        dmaStatus = dma->cnt;
        dmaStatus = dma->cnt;
        dmaMask = 0x80000000;
        early7038 = sUnk_86D7038;
        early7038Plus = early7038 + 0x100;
        early4038 = sUnk_86D4038;
        languagePtr = &gLanguage;
        tilesBase = sUnk_86DD638;
        if ((s32)dmaStatus < 0) {
            do {
                dmaStatus = dma->cnt & dmaMask;
            } while (dmaStatus != 0);
        }
    } while (0);
    PAUSE_DMA_TRANSFER_R1((const void*)0x02020600, (void*)0x05000000, 0x80000100);
    PAUSE_DMA_TRANSFER_R1(early7038, (void*)0x05000000, 0x80000100);
    PAUSE_DMA_TRANSFER_R1(early7038Plus, (void*)0x05000300, 0x80000080);
    PAUSE_DMA_TRANSFER_R1(early4038, (void*)0x06000000, 0x80001800);
    {
        register struct DmaRegisters* d7238Dma PIN("r1");
        d7238Dma = (struct DmaRegisters*)0x040000D4;
        dmaStatus = (u32)sUnk_86D7238;
        d7238Dma->src = dmaStatus;
        d7238Dma->dst = (u32)((void*)0x06012400);
        d7238Dma->cnt = 0x80002E00;
        dmaStatus = d7238Dma->cnt;
        dmaStatus = d7238Dma->cnt;
        dmaMask = 0x80000000;
        if ((s32)dmaStatus < 0) {
            do {
                dmaStatus = d7238Dma->cnt & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    if (*languagePtr == 0) {
        languageDma = (struct DmaRegisters*)0x040000D4;
        languageTiles = sUnk_86DCE38;
    } else {
        languageDma = (struct DmaRegisters*)0x040000D4;
        languageTiles = sUnk_86E29F8;
    }
    languageDma->src = (u32)languageTiles;
    languageDma->dst = 0x0600C000;
    languageDma->cnt = 0x80000400;
    dmaStatus = languageDma->cnt;
    {
        struct DmaRegisters* languageWaitDma;
        register u32 languageWaitMask PIN("r1");
        languageWaitDma = (struct DmaRegisters*)0x040000D4;
        dmaStatus = languageWaitDma->cnt;
        languageWaitMask = 0x80000000;
        if ((s32)dmaStatus < 0) {
            do {
                dmaStatus = languageWaitDma->cnt & languageWaitMask;
            } while (dmaStatus != 0);
        }
    }

    {
        struct DmaRegisters* tilesDma;
        tilesDma = (struct DmaRegisters*)0x040000D4;
        dmaMask = (u32)tilesBase;
        tilesDma->src = dmaMask;
        tilesDma->dst = (u32)((void*)0x0600D000);
        tilesDma->cnt = 0x80000400;
        dmaStatus = tilesDma->cnt;
        dmaStatus = tilesDma->cnt;
        dmaMask = 0x80000000;
        if ((s32)dmaStatus < 0) {
            do {
                dmaStatus = tilesDma->cnt & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        register u8* sideCheck1 PIN("r4");
        sideCheck1 = pauseSidePtr;
        if ((*sideCheck1) != 0) {
            sideDma = (struct DmaRegisters*)0x040000D4;
            sideTiles = sUnk_86DD638 + 0x800;
        } else {
            sideDma = (struct DmaRegisters*)0x040000D4;
            sideTiles = sUnk_86DE638;
        }
    }
    sideDma->src = (u32)sideTiles;
    sideDma->dst = 0x0600E000;
    sideDma->cnt = 0x80000400;
    dmaStatus = sideDma->cnt;
    {
        struct DmaRegisters* sideWaitDma;
        register u32 sideWaitMask PIN("r1");
        sideWaitDma = (struct DmaRegisters*)0x040000D4;
        dmaStatus = sideWaitDma->cnt;
        sideWaitMask = 0x80000000;
        if ((s32)dmaStatus < 0) {
            do {
                dmaStatus = sideWaitDma->cnt & sideWaitMask;
            } while (dmaStatus != 0);
        }
    }

    {
        register u8* sideCheck2 PIN("r5");
        sideCheck2 = pauseSidePtr;
        if ((*sideCheck2) == 0) {
            struct DmaRegisters* optionalDma;
            optionalDma = (struct DmaRegisters*)0x040000D4;
            optionalDma->src = (u32)(sUnk_86DF0F8 + ((gCurrentPassage - 1) << 6));
            optionalDma->dst = 0x05000200;
            optionalDma->cnt = 0x80000020;
            optionalDma->cnt;
        }
    }

    PAUSE_DMA_TRANSFER_R1(
        (dmaStatus = (gCurrentPassage << 6), pausePaletteBase = sUnk_86DEEF8, dmaStatus + (u32)pausePaletteBase),
        (void*)0x05000240,
        0x80000020
    );
    ASM_BARRIER_VOLATILE(: "+r"(pausePaletteBase));

    ((struct DmaRegisters*)0x040000D4)->src = (u32)(sUnk_86DEE38 + (gCurrentPassage << 5));
    ((struct DmaRegisters*)0x040000D4)->dst = 0x05000160;
    ((struct DmaRegisters*)0x040000D4)->cnt = 0x80000010;
    ((struct DmaRegisters*)0x040000D4)->cnt;

    *(vu16*)0x05000000 = 0;
    *(vu16*)0x04000008 = 0x1800;
    *(vu16*)0x0400000A = 0x1A01;
    *(vu16*)0x0400000C = 0x1C02;
    {
        int soundTmp;
        register u16 soundValue PIN("r0");
        u16* soundPtr;
        soundTmp = 250;
        soundTmp <<= 2;
        soundValue = soundTmp;
        soundPtr = &gSoundRoomMessageTimer;
        *soundPtr = soundValue;
    }

    gColorFading.type = 2;
    gColorFading.uploadFlags = 3;
    FillColorFadePalettes();

    GP_SET(gPauseScreenAnimation, sUnk_86D3F40);
    gPauseScreenState.animationTimer = 0;
    gPauseScreenState.animationFrame = 0;
    gPauseScreenState.selection = 0;

    if (gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceNE &&
        gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceSE &&
        gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceSW &&
        gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceNW) {
        gPauseAllJewelPiecesCollected = 1;
    } else {
        gPauseAllJewelPiecesCollected = 0;
    }

    {
        u8* passagePtr;
        u8* stagePtr;
        u8* jewelStates;
        u8* nePtr;
        u8* sePtr;
        register u8* swPtr PIN("r12");
        register u8* nwPtr PIN("sl");
        PauseAnimationState* cdPtr;
        PauseAnimationState* keyPtr;

        passagePtr = &gCurrentPassage;
        stagePtr = &gCurrentStageNumber;
        jewelStates = gPauseJewelPieceStates;
        nePtr = &gCollectedNEJewelPiece;
        sePtr = &gCollectedSEJewelPiece;
        swPtr = &gCollectedSWJewelPiece;
        nwPtr = &gCollectedNWJewelPiece;
        cdPtr = &gPauseCdAnimationState;
        keyPtr = &gPauseKeyzerAnimationState;

        {
            s32 jewelBase;
            s32 jewelIt;
            s32 jewelZero;
            jewelBase = (s32)jewelStates;
            jewelZero = 0;
            ASM_BARRIER(: : "r"(jewelZero));
            jewelIt = jewelBase + 3;
            do {
                *(u8*)jewelIt = 0;
                jewelIt--;
            } while (jewelIt >= jewelBase);
        }

        if ((u8)(*nePtr - 1) <= 1) {
            jewelStates[0] = 1;
        } else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceNE) {
            jewelStates[0] = 2;
        }

        if ((u8)(*sePtr - 1) <= 1) {
            jewelStates[1] = 1;
        } else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceSE) {
            jewelStates[1] = 2;
        }

        {
            register u8* swRead PIN("r2");
            swRead = swPtr;
            if ((u8)(*swRead - 1) <= 1) {
                jewelStates[2] = 1;
            } else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceSW) {
                jewelStates[2] = 2;
            }
        }

        {
            register u8* nwRead PIN("r6");
            nwRead = nwPtr;
            if ((u8)(*nwRead - 1) <= 1) {
                jewelStates[3] = 1;
            } else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceNW) {
                jewelStates[3] = 2;
            }
        }

        {
            register u16 zero PIN("r0");
            register u16* sequenceTimer PIN("r1");
            register u8* seqStepPtr PIN("r2");

            zero = 0;
            sequenceTimer = &gStageEntrySequenceTimer;
            *sequenceTimer = zero;
            seqStepPtr = &gStageEntrySequenceStep;
            *seqStepPtr = zero;
        }

        {
            u16 jewelReset;

            jewelReset = 0;
            for (i = 0; i < 4; i++) {
                gPauseJewelAnimationStates[i].animationTimer = jewelReset;
                gPauseJewelAnimationStates[i].animationFrame = jewelReset;
            }
            cdPtr->animationTimer = 0;
            cdPtr->animationFrame = 0;
            keyPtr->animationTimer = 0;
            keyPtr->animationFrame = 0;
        }

        UploadFiveDigitNumberTiles(gHighScoreTable[*passagePtr][*stagePtr], sUnk_86D6E78, 0x2A80);
        {
            struct DmaRegisters* waitDma;
            register u32 waitMask PIN("r1");
            u32 waitStatus;
            waitDma = (struct DmaRegisters*)0x040000D4;
            waitStatus = waitDma->cnt;
            waitMask = 0x80000000;
            if ((s32)waitStatus < 0) {
                while (waitDma->cnt & waitMask) {
                }
            }
        }
    }
    UploadTallFiveDigitNumberTiles(gTotalScore, sUnk_86D6838, 0x2980);

    gStageEntryWindowTop = 160;
    gStageEntryWindowBottom = 160;

    *(vu16*)0x04000012 = 0;
    *(vu16*)0x04000010 = 0;
    *(vu16*)0x04000016 = 0;
    *(vu16*)0x04000014 = 0;
    *(vu16*)0x0400001A = 0;
    *(vu16*)0x04000018 = 0;
    *(vu16*)0x04000000 = 0x1700;
    *(vu16*)0x04000000 |= 0x2000;
    *(vu16*)0x04000048 = 0x3F;
    *(vu16*)0x0400004A = 0;
    *(vu16*)0x04000050 = 0;

    MPlayStop(GP(gMPlayTable[3].info));
    MPlayStop(GP(gMPlayTable[4].info));
    MPlayStop(GP(gMPlayTable[5].info));
    MPlayStop(GP(gMPlayTable[6].info));

    *(vu16*)0x04000200 |= 1;
}

#undef PAUSE_DMA_TRANSFER_DMA
#undef PAUSE_DMA_TRANSFER

u32 UpdatePauseScreenSelection(void)
{
    u16 buttons;
    struct DmaRegisters* dma;

    switch (gPauseScreenState.selection) {
        case 0:
            buttons = gButtonsPressed;
            if (buttons & 1) {
                GP_SET(gPauseScreenAnimation, sUnk_86D3F58);
                gPauseScreenState.animationFrame = 0;
                gPauseScreenState.animationTimer = 0;
                gPauseFlag = 1;
                m4aSongNumStart(0x123);
                return 1;
            }
            if (buttons & 0x10) {
                gPauseScreenState.selection = 1;
                m4aSongNumStart(0x124);
            } else {
                u32 mask;
                mask = 0x20;
                mask &= buttons;
                if (mask) {
                    gPauseScreenState.selection = 2;
                    m4aSongNumStart(0x124);
                }
            }
            break;
        case 1:
            buttons = gButtonsPressed;
            if (buttons & 1) {
                dma = (struct DmaRegisters*)0x040000D4;
                dma->src = (u32)sUnk_86DF1F8;
                dma->dst = 0x06014800;
                dma->cnt = 0x80001C00;
                dma->cnt;
                gUnk_3000000 = 7;
                gPauseScreenState.confirmedSelection = gPauseScreenState.selection;
                gPauseScreenState.selection = 3;
                m4aSongNumStart(0x125);
                return 0;
            }
            if (buttons & 0x10) {
                gPauseScreenState.selection = 2;
                m4aSongNumStart(0x124);
            } else {
                u32 mask;
                mask = 0x20;
                mask &= buttons;
                if (mask) {
                    gPauseScreenState.selection = 0;
                    m4aSongNumStart(0x124);
                }
            }
            break;
        case 2:
            buttons = gButtonsPressed;
            if (buttons & 1) {
                dma = (struct DmaRegisters*)0x040000D4;
                dma->src = (u32)sUnk_86D9638;
                dma->dst = 0x06014800;
                dma->cnt = 0x80001C00;
                dma->cnt;
                gUnk_3000000 = 7;
                gPauseScreenState.confirmedSelection = gPauseScreenState.selection;
                gPauseScreenState.selection = 3;
                m4aSongNumStart(0x125);
                return 0;
            }
            if (buttons & 0x10) {
                gPauseScreenState.selection = 0;
                m4aSongNumStart(0x124);
            } else {
                u32 mask;
                mask = 0x20;
                mask &= buttons;
                if (mask) {
                    gPauseScreenState.selection = 1;
                    m4aSongNumStart(0x124);
                }
            }
            break;
    }

    if (gButtonsPressed & 2) {
        gPauseFlag = 1;
        m4aSongNumStart(0x123);
        return 1;
    }
    return 0;
}

u32 UpdatePauseScreenConfirmation(void)
{
    switch (gPauseScreenState.selection) {
        case 3:
            if (gButtonsPressed & 1) {
                m4aSongNumStart(0x129);
                return 2;
            }
            if (gButtonsPressed & 0x10) {
                gPauseScreenState.selection = 4;
                m4aSongNumStart(0x124);
            }
            break;
        case 4:
            if (gButtonsPressed & 1) {
                m4aSongNumStart(0x126);
                return 1;
            }
            if (gButtonsPressed & 0x20) {
                gPauseScreenState.selection = 3;
                m4aSongNumStart(0x124);
            }
            break;
    }

    if (gButtonsPressed & 2) {
        m4aSongNumStart(0x129);
        return 2;
    }
    return 0;
}

u32 OpenPauseScreenWindow(void)
{
    gStageEntryWindowTop -= 17;
    if ((s16)gStageEntryWindowTop < 0) {
        gStageEntryWindowTop = 0;
        return 1;
    }
    return 0;
}

u32 ClosePauseScreenWindow(void)
{
    gStageEntryWindowTop += 17;
    if ((s16)gStageEntryWindowTop > 160) {
        gStageEntryWindowTop = 160;
        return 1;
    }
    return 0;
}

void RenderPauseScreenOam(void)
{
    const struct AnimationFrame* animation;
    const u16* src;
    u16* dest;
    u16 attr;
    s32 currentSlot;
    s32 nextSlot;
    s32 i;
    u32 keyzerState;
    u32 cdState;
    s32 score;

    currentSlot = 0;
    nextSlot = gOamSlotsUsed;
    dest = (u16*)&gOamBuffer[nextSlot];

    gPauseScreenState.animationTimer++;
    if (GP(gPauseScreenAnimation)[gPauseScreenState.animationFrame].time < gPauseScreenState.animationTimer) {
        gPauseScreenState.animationTimer = 1;
        gPauseScreenState.animationFrame++;
        if (GP(gPauseScreenAnimation)[gPauseScreenState.animationFrame].time == 0) {
            gPauseScreenState.animationFrame = 0;
            if (GP(gPauseScreenAnimation) == sUnk_86D3F58) {
                gPauseScreenState.animationFrame = 1;
            }
        }
    }
    src = GP(GP(gPauseScreenAnimation)[gPauseScreenState.animationFrame].oam);
    nextSlot += *(src++);
    if (nextSlot > 128) {
        return;
    }
    for (; currentSlot < nextSlot; currentSlot++) {
        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.y = attr + sUnk_86D36B8[gPauseScreenState.selection];
        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36A4[gPauseScreenState.selection]) & 0x1FF;
        *(dest++) = *(src++);
        gOamBuffer[currentSlot].split.priority = 0;
        dest++;
    }

    if ((u8)(gPauseScreenState.selection - 3) <= 1) {
        if (gLanguage == 0) {
            animation = GP(sUnk_86D370C[0][gPauseScreenState.selection - 3]);
        } else {
            animation = GP(sUnk_86D370C[1][gPauseScreenState.selection - 3]);
        }
        src = GP(animation->oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 80;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 120) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 176;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 203;
    gOamBuffer[currentSlot].split.paletteNum = 6;
    currentSlot++;
    dest++;

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 184;
    gOamBuffer[currentSlot].split.size = 0;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 132;
    gOamBuffer[currentSlot].split.paletteNum = 4;
    currentSlot++;
    dest++;

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 200;
    gOamBuffer[currentSlot].split.size = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 134;
    gOamBuffer[currentSlot].split.paletteNum = 4;
    currentSlot++;
    dest++;

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 8;
    gOamBuffer[currentSlot].split.size = 2;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 144;
    gOamBuffer[currentSlot].split.paletteNum = 7;
    currentSlot++;
    dest++;

    nextSlot += 5;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 40;
    gOamBuffer[currentSlot].split.size = 2;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 148;
    gOamBuffer[currentSlot].split.paletteNum = 7;
    currentSlot++;
    dest++;

    for (i = 0; i < 4; i++) {
        if (gPauseJewelPieceStates[i] == 2) {
            animation = GP(sUnk_86D36FC[i]);
            gPauseJewelAnimationStates[i].animationTimer++;
            if (animation[gPauseJewelAnimationStates[i].animationFrame].time <
                gPauseJewelAnimationStates[i].animationTimer) {
                gPauseJewelAnimationStates[i].animationTimer = 1;
                gPauseJewelAnimationStates[i].animationFrame++;
                if (animation[gPauseJewelAnimationStates[i].animationFrame].time == 0) {
                    gPauseJewelAnimationStates[i].animationFrame = 0;
                }
            }
            src = GP(animation[gPauseJewelAnimationStates[i].animationFrame].oam);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 88;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36CC[gUnk_3003C4A]) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
        if (gPauseAllJewelPiecesCollected == 0 && gPauseJewelPieceStates[i] != 0) {
            animation = GP(sUnk_86D36DC[gPauseJewelPieceStates[i] - 1][i]);
            src = GP(animation->oam);
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 88;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36CC[gUnk_3003C4A]) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }

    if (gPauseAllJewelPiecesCollected != 0) {
        src = GP(sUnk_86D3DB0->oam);
        nextSlot += *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 88;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36CC[gUnk_3003C4A]) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    keyzerState = gCollectedKeyzer;
    if (keyzerState == 1 || gCurrentCollection[gCurrentPassage][gCurrentStageNumber].keyzer) {
        animation = sUnk_86D3D08;
        if (keyzerState == 1) {
            animation = sUnk_86D3CF8;
        }
        gPauseKeyzerAnimationState.animationTimer++;
        if (animation[gPauseKeyzerAnimationState.animationFrame].time < gPauseKeyzerAnimationState.animationTimer) {
            gPauseKeyzerAnimationState.animationTimer = 1;
            gPauseKeyzerAnimationState.animationFrame++;
            if (animation[gPauseKeyzerAnimationState.animationFrame].time == 0) {
                gPauseKeyzerAnimationState.animationFrame = 0;
            }
        }
        src = GP(animation[gPauseKeyzerAnimationState.animationFrame].oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 96;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36D4[gUnk_3003C4A]) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    cdState = (u8)(gCollectedCD - 1);
    if ((cdState <= 1 || gCurrentCollection[gCurrentPassage][gCurrentStageNumber].cd) && gUnk_3003C4A == 0) {
        animation = sUnk_86D3FD0;
        if (cdState <= 1) {
            animation = sUnk_86D3F80;
        }
        gPauseCdAnimationState.animationTimer++;
        if (animation[gPauseCdAnimationState.animationFrame].time < gPauseCdAnimationState.animationTimer) {
            gPauseCdAnimationState.animationTimer = 1;
            gPauseCdAnimationState.animationFrame++;
            if (animation[gPauseCdAnimationState.animationFrame].time == 0) {
                gPauseCdAnimationState.animationFrame = 0;
            }
        }
        src = GP(animation[gPauseCdAnimationState.animationFrame].oam);
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 88;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 200) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    score = gHighScoreTable[gCurrentPassage][gCurrentStageNumber];
    if (score > 599) {
        if (score > 999) {
            src = GP(sUnk_86D4008->oam);
        } else if (score > 799) {
            src = GP(sUnk_86D4018->oam);
        } else {
            src = GP(sUnk_86D4028->oam);
        }
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 48;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 168) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }
    gOamSlotsUsed = nextSlot;
}

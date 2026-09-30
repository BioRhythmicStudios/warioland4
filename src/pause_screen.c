#include "global_data.h"
#include "pause.h"
#include "gba/m4a.h"
#include "input.h"
#include "interrupt_callbacks.h"
#include "stage_entry.h"
#include "main.h"
#include "color_effects.h"
#include "init_helpers.h"
#include "score.h"
#include "stage_select.h"

struct DmaRegisters {
    vu32 src;
    vu32 dst;
    vu32 cnt;
};

extern u8 gLanguage;
extern u8 gCurrentStageNumber;
extern u8 gCurrentPassage;
extern const u8 *const sFileSelectPassageNameTableLanguage0[][6];
extern const u8 *const sFileSelectPassageNameTableLanguage1[][6];
extern void Shop_DrawText(const u8 *str, u32 tileBase, u32 param);
extern void m4aMPlayContinue(struct MusicPlayerInfo *mplayInfo);
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
extern const struct AnimationFrame *const sUnk_86D36DC[];
extern const struct AnimationFrame *const sUnk_86D36FC[];
extern const struct AnimationFrame *const sUnk_86D370C[2][2];
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
        if (UpdatePaletteFadeStep(0, 1))
            gUnk_3000000++;
        break;
    case 2:
        if (OpenPauseScreenWindow())
            gUnk_3000000++;
        break;
    case 3:
        if (UpdatePauseScreenSelection())
            gUnk_3000000++;
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
        if (ClosePauseScreenWindow())
            gUnk_3000000++;
        break;
    case 6:
        UploadColorFadePalettes();
        if (UpdatePaletteFadeStep(1, 1)) {
            *(vu16 *)0x0400004A = 63;
            gUnk_3000000 = 0;
            if (gPauseScreenState.confirmedSelection == 0) {
                m4aMPlayContinue(gMPlayTable[3].info);
                m4aMPlayContinue(gMPlayTable[4].info);
                m4aMPlayContinue(gMPlayTable[5].info);
                m4aMPlayContinue(gMPlayTable[6].info);
            }
            done = 1;
        }
        break;
    case 7:
        {
            u32 result_r0;
            result_r0 = UpdatePauseScreenConfirmation();
            if (result_r0 == 1) {
                gUnk_3000000 = 4;
                gPauseScreenAnimation = sUnk_86D3F58;
                gPauseScreenState.animationFrame = 0;
                gPauseScreenState.animationTimer = 0;
                if (gPauseScreenState.confirmedSelection == 1) {
                    gPauseFlag = 3;
                    gMainGameMode++;
                }
                if (gPauseScreenState.confirmedSelection == 2)
                    gPauseFlag = 2;
            } else if (result_r0 == 2) {
                gPauseScreenState.selection = gPauseScreenState.confirmedSelection;
                gUnk_3000000 = 3;
            }
        }
        break;
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
    struct DmaRegisters *dma;

    m4aSoundVSync();
    dma = (struct DmaRegisters *)0x040000D4;
    dma->src = (u32)gOamBuffer;
    dma->dst = 0x07000000;
    dma->cnt = 0x84000100;
    dma->cnt;

    *(vu16 *)0x04000044 = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;
    *(vu16 *)0x04000040 = 0xF0;

    switch (gLanguage) {
    case 0:
        Shop_DrawText(sFileSelectPassageNameTableLanguage0[gCurrentPassage][gCurrentStageNumber], 0x7800, 26);
        break;
    case 1:
        Shop_DrawText(sFileSelectPassageNameTableLanguage1[gCurrentPassage][gCurrentStageNumber], 0x7800, 26);
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
    register struct DmaRegisters *localDma asm("r1"); \
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
    struct DmaRegisters *dma;
    s32 i;
    u32 dmaStatus;

    register u32 dmaMask asm("r2");
    u8 *sideAddress;
    register const u8 *pausePaletteBase asm("r6");
    register u8 *pauseSidePtr asm("r8");

    register const u8 *tilesBase asm("sl");
    const u8 *early7038;
    const u8 *early7038Plus;
    const u8 *early4038;
    const u8 *sideTiles;
    struct DmaRegisters *sideDma;
    const u8 *languageTiles;
    const u8 *languagePtr;
    struct DmaRegisters *languageDma;

    *(vu16 *)0x04000200 ^= 1;
    *(vu16 *)0x04000208 = 0;
    *(vu16 *)0x04000004 &= 0xFFEF;
    *(vu16 *)0x04000200 &= 0xFFFD;
    *(vu16 *)0x04000208 = 1;

    if (gCurrentPassage == 0 || gCurrentPassage == 5) {
        sideAddress = &gUnk_3003C4A;
        *sideAddress = 1;
    } else {
        sideAddress = &gUnk_3003C4A;
        *sideAddress = 0;
    }
    pauseSidePtr = sideAddress;

    dma = (struct DmaRegisters *)0x040000D4;
    do {
        dma->src = (u32)((const void *)0x02020600);
        dma->dst = (u32)((void *)0x05000200);
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
    PAUSE_DMA_TRANSFER_R1((const void *)0x02020600, (void *)0x05000000, 0x80000100);
    PAUSE_DMA_TRANSFER_R1(early7038, (void *)0x05000000, 0x80000100);
    PAUSE_DMA_TRANSFER_R1(early7038Plus, (void *)0x05000300, 0x80000080);
    PAUSE_DMA_TRANSFER_R1(early4038, (void *)0x06000000, 0x80001800);
    {
        register struct DmaRegisters *d7238Dma asm("r1");
        d7238Dma = (struct DmaRegisters *)0x040000D4;
        dmaStatus = (u32)sUnk_86D7238;
        d7238Dma->src = dmaStatus;
        d7238Dma->dst = (u32)((void *)0x06012400);
        d7238Dma->cnt = 0x80002E00;
        dmaStatus = d7238Dma->cnt;
        dmaStatus = d7238Dma->cnt;
        dmaMask = 0x80000000;
        if ((s32)dmaStatus < 0) { do { dmaStatus = d7238Dma->cnt & dmaMask; } while (dmaStatus != 0); }
    }

    if (*languagePtr == 0) {
        languageDma = (struct DmaRegisters *)0x040000D4;
        languageTiles = sUnk_86DCE38;
    } else {
        languageDma = (struct DmaRegisters *)0x040000D4;
        languageTiles = sUnk_86E29F8;
    }
    languageDma->src = (u32)languageTiles;
    languageDma->dst = 0x0600C000;
    languageDma->cnt = 0x80000400;
    dmaStatus = languageDma->cnt;
    {
        struct DmaRegisters *languageWaitDma;
        register u32 languageWaitMask asm("r1");
        languageWaitDma = (struct DmaRegisters *)0x040000D4;
        dmaStatus = languageWaitDma->cnt;
        languageWaitMask = 0x80000000;
        if ((s32)dmaStatus < 0) {
            do {
                dmaStatus = languageWaitDma->cnt & languageWaitMask;
            } while (dmaStatus != 0);
        }
    }

    {
        struct DmaRegisters *tilesDma;
        tilesDma = (struct DmaRegisters *)0x040000D4;
        dmaMask = (u32)tilesBase;
        tilesDma->src = dmaMask;
        tilesDma->dst = (u32)((void *)0x0600D000);
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
        register u8 *sideCheck1 asm("r4");
        sideCheck1 = pauseSidePtr;
        if ((*sideCheck1) != 0) {
        sideDma = (struct DmaRegisters *)0x040000D4;
        sideTiles = sUnk_86DD638 + 0x800;
    } else {
        sideDma = (struct DmaRegisters *)0x040000D4;
        sideTiles = sUnk_86DE638;
        }
    }
    sideDma->src = (u32)sideTiles;
    sideDma->dst = 0x0600E000;
    sideDma->cnt = 0x80000400;
    dmaStatus = sideDma->cnt;
    {
        struct DmaRegisters *sideWaitDma;
        register u32 sideWaitMask asm("r1");
        sideWaitDma = (struct DmaRegisters *)0x040000D4;
        dmaStatus = sideWaitDma->cnt;
        sideWaitMask = 0x80000000;
        if ((s32)dmaStatus < 0) {
            do {
                dmaStatus = sideWaitDma->cnt & sideWaitMask;
            } while (dmaStatus != 0);
        }
    }

    {
        register u8 *sideCheck2 asm("r5");
        sideCheck2 = pauseSidePtr;
        if ((*sideCheck2) == 0) {
            struct DmaRegisters *optionalDma;
            optionalDma = (struct DmaRegisters *)0x040000D4;
            optionalDma->src = (u32)(sUnk_86DF0F8 + ((gCurrentPassage - 1) << 6));
            optionalDma->dst = 0x05000200;
            optionalDma->cnt = 0x80000020;
            optionalDma->cnt;
        }
    }

    PAUSE_DMA_TRANSFER_R1((dmaStatus = (gCurrentPassage << 6), pausePaletteBase = sUnk_86DEEF8, dmaStatus + (u32)pausePaletteBase), (void *)0x05000240, 0x80000020);
    __asm__ volatile ("" : "+r" (pausePaletteBase));

    ((struct DmaRegisters *)0x040000D4)->src = (u32)(sUnk_86DEE38 + (gCurrentPassage << 5));
    ((struct DmaRegisters *)0x040000D4)->dst = 0x05000160;
    ((struct DmaRegisters *)0x040000D4)->cnt = 0x80000010;
    ((struct DmaRegisters *)0x040000D4)->cnt;

    *(vu16 *)0x05000000 = 0;
    *(vu16 *)0x04000008 = 0x1800;
    *(vu16 *)0x0400000A = 0x1A01;
    *(vu16 *)0x0400000C = 0x1C02;
    { int soundTmp; register u16 soundValue asm("r0"); u16 *soundPtr; soundTmp = 250; soundTmp <<= 2; soundValue = soundTmp; soundPtr = &gSoundRoomMessageTimer; *soundPtr = soundValue; }

    gColorFading.type = 2;
    gColorFading.uploadFlags = 3;
    FillColorFadePalettes();

    gPauseScreenAnimation = sUnk_86D3F40;
    gPauseScreenState.animationTimer = 0;
    gPauseScreenState.animationFrame = 0;
    gPauseScreenState.selection = 0;

    if (gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceNE
     && gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceSE
     && gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceSW
     && gCurrentCollection[gCurrentPassage][gCurrentStageNumber].jewelPieceNW)
        gPauseAllJewelPiecesCollected = 1;
    else
        gPauseAllJewelPiecesCollected = 0;

    {
        u8 *passagePtr;
        u8 *stagePtr;
        u8 *jewelStates;
        u8 *nePtr;
        u8 *sePtr;
        register u8 *swPtr asm("r12");
        register u8 *nwPtr asm("sl");
        PauseAnimationState *cdPtr;
        PauseAnimationState *keyPtr;

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
            asm("" : : "r"(jewelZero));
            jewelIt = jewelBase + 3;
            do {
                *(u8 *)jewelIt = 0;
                jewelIt--;
            } while (jewelIt >= jewelBase);
        }

        if ((u8)(*nePtr - 1) <= 1)
            jewelStates[0] = 1;
        else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceNE)
            jewelStates[0] = 2;

        if ((u8)(*sePtr - 1) <= 1)
            jewelStates[1] = 1;
        else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceSE)
            jewelStates[1] = 2;

        { register u8 *swRead asm("r2"); swRead = swPtr;
        if ((u8)(*swRead - 1) <= 1)
            jewelStates[2] = 1;
        else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceSW)
            jewelStates[2] = 2; }

        { register u8 *nwRead asm("r6"); nwRead = nwPtr;
        if ((u8)(*nwRead - 1) <= 1)
            jewelStates[3] = 1;
        else if (gCurrentCollection[*passagePtr][*stagePtr].jewelPieceNW)
            jewelStates[3] = 2; }

        {
            u16 zero;
            register u16 *sequenceTimer asm("r1");

            zero = 0;
            sequenceTimer = &gStageEntrySequenceTimer;
            asm("strh r0, [r1]" : : "r"(zero), "r"(sequenceTimer) : "memory");
        }
        { register u8 *seqStepPtr asm("r2"); seqStepPtr = &gStageEntrySequenceStep; *seqStepPtr = 0; }

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

        UploadFiveDigitNumberTiles(gHighScoreTable[*passagePtr][*stagePtr],
                                   sUnk_86D6E78, 0x2A80);
        {
            struct DmaRegisters *waitDma;
            register u32 waitMask asm("r1");
            u32 waitStatus;
            waitDma = (struct DmaRegisters *)0x040000D4;
            waitStatus = waitDma->cnt;
            waitMask = 0x80000000;
            if ((s32)waitStatus < 0) {
                while (waitDma->cnt & waitMask) {}
            }
        }
    }
    UploadTallFiveDigitNumberTiles(gTotalScore, sUnk_86D6838, 0x2980);

    gStageEntryWindowTop = 160;
    gStageEntryWindowBottom = 160;

    *(vu16 *)0x04000012 = 0;
    *(vu16 *)0x04000010 = 0;
    *(vu16 *)0x04000016 = 0;
    *(vu16 *)0x04000014 = 0;
    *(vu16 *)0x0400001A = 0;
    *(vu16 *)0x04000018 = 0;
    *(vu16 *)0x04000000 = 0x1700;
    *(vu16 *)0x04000000 |= 0x2000;
    *(vu16 *)0x04000048 = 0x3F;
    *(vu16 *)0x0400004A = 0;
    *(vu16 *)0x04000050 = 0;

    MPlayStop(gMPlayTable[3].info);
    MPlayStop(gMPlayTable[4].info);
    MPlayStop(gMPlayTable[5].info);
    MPlayStop(gMPlayTable[6].info);

    *(vu16 *)0x04000200 |= 1;
}

#undef PAUSE_DMA_TRANSFER_DMA
#undef PAUSE_DMA_TRANSFER

u32 UpdatePauseScreenSelection(void)
{
    u16 buttons;
    struct DmaRegisters *dma;

    switch (gPauseScreenState.selection) {
    case 0:
        buttons = gButtonsPressed;
        if (buttons & 1) {
            gPauseScreenAnimation = sUnk_86D3F58;
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
            dma = (struct DmaRegisters *)0x040000D4;
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
            dma = (struct DmaRegisters *)0x040000D4;
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
  u8 *oamSlotsPtr;
  u8 *new_var2;
  const struct AnimationFrame *new_var5;
  const struct AnimationFrame *animation;
  const struct AnimationFrame *initialAnimation;
  PauseAnimationState *animationState;
  const u16 *src;
  u16 *dest;
  u16 attr;
  const struct AnimationFrame * const *new_var9;
  s32 currentSlot;
  s32 nextSlot;
  s32 i;
  u8 itemState;
  char new_var3;
  int new_var;
  u8 cdState;
  s32 score;
  register u32 frameLoad asm("r0");
  const struct AnimationFrame *animationLoad;
  s32 slotCheck;
  s32 cdSlotCheck;
  int yMask;
  const s32 *new_var10;
  const struct AnimationFrame **new_var4;
  currentSlot = 0;
  oamSlotsPtr = &gOamSlotsUsed;
  new_var = *oamSlotsPtr;
  nextSlot = new_var;
  dest = (u16 *) (&gOamBuffer[nextSlot]);
  gPauseScreenState.animationTimer++;
  frameLoad = gPauseScreenState.animationFrame;
  asm("" : "+r"(frameLoad));
  score = frameLoad;
  animationLoad = gPauseScreenAnimation;
  initialAnimation = animationLoad;
  if (initialAnimation[score].time < gPauseScreenState.animationTimer)
  {
    gPauseScreenState.animationTimer = 1;
    gPauseScreenState.animationFrame++;
    if (initialAnimation[gPauseScreenState.animationFrame].time == 0)
    {
      gPauseScreenState.animationFrame = 0;
      if (initialAnimation == sUnk_86D3F58)
      {
        gPauseScreenState.animationFrame = 1;
      }
    }
  }
  src = gPauseScreenAnimation[gPauseScreenState.animationFrame].oam;
  nextSlot += *(src++);
  slotCheck = nextSlot;
  if (slotCheck > 128)
  {
    return;
  }
  if (currentSlot < nextSlot)
  {
    OamData *oam;
    register OamData *initialBase asm("r1");
    struct PauseScreenState *pauseStateReg;
    s32 initialXMask;
    register s32 initialPriorityMask asm("r10");
    s32 initialXLoad;
    s32 initialPriorityLoad;
    s32 initialCount;
    initialBase = gOamBuffer;
    pauseStateReg = &gPauseScreenState;
    oam = (OamData *) ((currentSlot << 3) + (u32) initialBase);
    initialXLoad = 0x1FF;
    asm("" : "+r"(initialXLoad));
    initialXMask = initialXLoad;
    initialPriorityLoad = -13;
    initialPriorityMask = initialPriorityLoad;
    initialCount = nextSlot;
    currentSlot = initialCount - currentSlot;
    do
    {
      attr = *(src++);
      *(dest++) = attr;
      {
        struct PauseScreenState *stateY = pauseStateReg;
        oam->split.y = (attr + sUnk_86D36B8[stateY->selection]) & 0xFF;
      }
      attr = *(src++);
      *(dest++) = attr;
      {
        struct PauseScreenState *stateX = pauseStateReg;
        {
          s32 initialNewX;
          s32 initialXMaskRead;
          u16 initialOldAttr1;
          s32 initialAttr1;
          initialNewX = attr + sUnk_86D36A4[stateX->selection];
          initialXMaskRead = initialXMask;
          initialNewX &= initialXMaskRead;
          initialOldAttr1 = oam->all.attr1;
          initialAttr1 = -0x200;
          initialAttr1 &= initialOldAttr1;
          initialAttr1 |= initialNewX;
          oam->all.attr1 = initialAttr1;
        }
      }
      *(dest++) = *(src++);
      {
        register u8 initialPriorityByte asm("r1") = ((u8 *) oam)[5];
        register u16 initialPriorityOut asm("r0") = initialPriorityMask;
        initialPriorityOut &= initialPriorityByte;
        ((u8 *) oam)[5] = initialPriorityOut;
      }
      dest++;
      oam++;
      currentSlot--;
    }
    while (currentSlot != 0);
    currentSlot = nextSlot;
  }
  if (((u8) (gPauseScreenState.selection - 3)) <= 1)
  {
    if (gLanguage == 0)
    {
      animation = sUnk_86D370C[0][gPauseScreenState.selection - 3];
    }
    else
    {
      animation = sUnk_86D370C[1][gPauseScreenState.selection - 3];
    }
    src = animation->oam;
    nextSlot += *(src++);
    if (nextSlot > 128)
    {
      return;
    }
    if (currentSlot < nextSlot)
    {
      OamData *oam;
      register OamData *langBase asm("r1");
      s32 langXMask;
      register s32 langPreserveMask asm("r9");
      s32 langPriorityMask;
      langBase = gOamBuffer;
      langXMask = 0x1FF;
      langPreserveMask = -0x200;
      langPriorityMask = -13;
      oam = (OamData *) ((currentSlot << 3) + (u32) langBase);
      currentSlot = nextSlot - currentSlot;
      do
      {
        attr = *(src++);
        *(dest++) = attr;
        oam->split.y = (attr + 80) & 0xFF;
        attr = *(src++);
        *(dest++) = attr;
        {
          s32 langNewX;
          register u16 langOldAttr1 asm("r2");
          register s32 langAttr1 asm("r0");
          langNewX = (attr + 120) & langXMask;
          langOldAttr1 = oam->all.attr1;
          langAttr1 = langPreserveMask;
          langAttr1 &= langOldAttr1;
          langAttr1 |= langNewX;
          oam->all.attr1 = langAttr1;
        }
        *(dest++) = *(src++);
        {
          register u8 langPriorityByte asm("r1") = ((u8 *) oam)[5];
          register unsigned short langPriorityOut asm("r0") = langPriorityMask;
          langPriorityOut &= langPriorityByte;
          ((u8 *) oam)[5] = langPriorityOut;
        }
        dest++;
        oam++;
        currentSlot--;
      }
      while (currentSlot != 0);
      currentSlot = nextSlot;
    }
  }
  *(dest++) = 0;
  gOamBuffer[currentSlot].split.y = 32;
  *(dest++) = 0;
  gOamBuffer[currentSlot].split.x = 176;
  *(dest++) = 0;
  cdState = 7;
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
  new_var3 = 32;
  currentSlot++;
  dest++;
  *(dest++) = 0;
  gOamBuffer[currentSlot].split.y = new_var3;
  gOamBuffer[currentSlot].split.shape = 1;
  *(dest++) = 0;
  gOamBuffer[currentSlot].split.x = 8;
  gOamBuffer[currentSlot].split.size = 2;
  *(dest++) = 0;
  new_var3 = 2;
  gOamBuffer[currentSlot].split.tileNum = 144;
  gOamBuffer[currentSlot].split.paletteNum = 7;
  currentSlot++;
  dest++;
  nextSlot += 5;
  *(dest++) = 0;
  gOamBuffer[currentSlot].split.y = 32;
  gOamBuffer[currentSlot].split.shape = 1;
  *(dest++) = 0;
  new_var9 = sUnk_86D36FC;
  new_var = 128;
  gOamBuffer[currentSlot].split.x = 40;
  gOamBuffer[currentSlot].split.size = new_var3;
  *(dest++) = 0;
  gOamBuffer[currentSlot].split.tileNum = 148;
  gOamBuffer[currentSlot].split.paletteNum = cdState;
  currentSlot++;
  dest++;
  {
    s32 tableOffset;
    for (i = 0, tableOffset = 0;;)
    {
    itemState = gPauseJewelPieceStates[i];
    if (itemState == 2)
    {
      u8 *jewelStateBase;
      {
        register const u8 *jewelAnimationBase asm("r0");
        const struct AnimationFrame *jewelAnimation;
        register s32 jewelAnimOffset asm("r5");
        jewelAnimationBase = (const u8 *) new_var9;

        // Couldn't figure out how to get this to match without using inline assembly.
        // The compiler was generating a lot of extra instructions to load the values into registers, so I just used inline assembly to do it in one instruction.
        asm("ldr %1, [sp, #8]\n\t"
            "add %0, %1, %0\n\t"
            "ldr %2, [%0]"
            : "+r"(jewelAnimationBase), "=r"(jewelAnimOffset), "=r"(jewelAnimation));
        animation = jewelAnimation;
        jewelStateBase = (u8 *) gPauseJewelAnimationStates;
        animationState = (PauseAnimationState *) (jewelAnimOffset + (u32) jewelStateBase);
      }
      {
      register u16 jewelTimer asm("r1") = animationState->animationTimer;
      jewelTimer++;
      animationState->animationTimer = jewelTimer;
      if (animation[animationState->animationFrame].time < jewelTimer)
      {
        animationState->animationTimer = 1;
        animationState->animationFrame++;
        ;
        if ((*(&animation[animationState->animationFrame])).time == 0)
        {
          animationState->animationFrame = 0;
        }
      }
      }
      {
        register s32 finalOffset asm("r1") = tableOffset;
        register PauseAnimationState *finalState asm("r0");
        asm("" : : "r"(finalOffset));
        finalState = (PauseAnimationState *) (finalOffset + (u32) jewelStateBase);
        src = animation[finalState->animationFrame].oam;
      }
      nextSlot += *(src++);
      // Couldn't figure out how to get this to match without using inline assembly.
      // The compiler was generating a lot of extra instructions to load the values into registers, so I just used inline assembly to do it in one instruction.
      {
        register s32 jewelSlotCheck asm("r2");
        asm("mov %0, ip" : "=r"(jewelSlotCheck));
        if (jewelSlotCheck > new_var)
        {
          return;
        }
      }
      if (currentSlot < nextSlot)
      {
        const s32 *jewelXTable = sUnk_86D36CC;
        s32 jewelPreserveMask = -0x200;
        register s32 jewelPriorityMask asm("r8") = -13;
        u8 *jewelState = &gUnk_3003C4A;
        register OamData *oam asm("r5");
        register u32 jewelOffset asm("r0");
        jewelOffset = currentSlot << 3;
        oam = (OamData *) (((u8 *) gOamBuffer) + jewelOffset);
        currentSlot = nextSlot - currentSlot;
        do
        {
          attr = *(src++);
          *(dest++) = attr;
          do
          {
            oam->split.y = (attr + 88) & 0xFF;
            attr = *(src++);
            *(dest++) = attr;
            {
              s32 jewelNewX;
              register u16 jewelOldAttr1 asm("r2");
              s32 jewelAttr1;
              jewelNewX = attr + jewelXTable[*jewelState];
              {
                register s32 jewelXMaskRead asm("r2") = 0x1FF;
                jewelNewX &= jewelXMaskRead;
              }
              jewelOldAttr1 = oam->all.attr1;
              jewelAttr1 = jewelPreserveMask;
              jewelAttr1 &= jewelOldAttr1;
              jewelAttr1 |= jewelNewX;
              oam->all.attr1 = jewelAttr1;
            }
            *(dest++) = *(src++);
          }
          while (0);
          {
            register u8 jewelPriorityByte asm("r1") = ((u8 *) oam)[5];
            register unsigned short jewelPriorityOut asm("r0") = jewelPriorityMask;
            jewelPriorityOut &= jewelPriorityByte;
            ((u8 *) oam)[5] = jewelPriorityOut;
          }
          dest++;
          oam++;
          currentSlot--;
        }
        while (currentSlot != 0);
        currentSlot = nextSlot;
      }
    }
    {
      register u8 *jewelPieceStates asm("r0") = &gPauseAllJewelPiecesCollected;
      register u8 allCollectedCheck asm("r1") = *jewelPieceStates;
      if (allCollectedCheck == 0)
      {
        register s32 jewelPieceIndex asm("r5") = i;
        register u8 *jewelPieceState asm("r1");
        jewelPieceStates = gPauseJewelPieceStates;
        asm("add %0, %1, %2"
            : "=r"(jewelPieceState)
            : "r"(jewelPieceIndex), "r"(jewelPieceStates));
        if (*jewelPieceState != 0)
        {
        new_var4 = sUnk_86D36DC;
        new_var2 = ((u8 *) new_var4) + (tableOffset + ((*jewelPieceState - 1) * 16));
        animation = *((const struct AnimationFrame **) new_var2);
        src = animation->oam;
        nextSlot += *(src++);
        // This too, couldn't figure out how to get this to match without using inline assembly.
        {
          register s32 jewel2SlotCheck asm("r5");
          asm("mov %0, ip" : "=r"(jewel2SlotCheck));
          if (jewel2SlotCheck > new_var)
          {
            return;
          }
        }
        if (currentSlot < nextSlot)
        {
          const s32 *jewel2XTable;
          register s32 jewel2PreserveMask asm("r10");
          s32 jewel2PriorityMask;
          register s32 jewel2LowR2 asm("r2");
          register u8 *jewel2State asm("r4");
          OamData *oam;
          jewelPieceStates = (u8 *) sUnk_86D36CC;
          asm("mov %0, %1"
              : "=r"(jewel2XTable)
              : "r"(jewelPieceStates));
          jewelPieceState = (u8 *) -0x200;
          asm("mov %0, %1"
              : "=r"(jewel2PreserveMask)
              : "r"(jewelPieceState));
          jewel2LowR2 = -13;
          asm("mov %0, %1"
              : "=r"(jewel2PriorityMask)
              : "r"(jewel2LowR2));
          jewel2State = &gUnk_3003C4A;
          jewelPieceStates = (u8 *) (currentSlot << 3);
          jewelPieceState = (u8 *) gOamBuffer;
          oam = (OamData *) ((u32) jewelPieceStates + (u32) jewelPieceState);
          asm("mov %0, ip" : "=r"(jewel2LowR2));
          currentSlot = jewel2LowR2 - currentSlot;
          do
          {
            attr = *(src++);
            *(dest++) = attr;
            oam->split.y = (attr + 88) & 0xFF;
            attr = *(src++);
            *(dest++) = attr;
            {
              s32 jewelNewX;
              register u16 jewelOldAttr1 asm("r2");
              register s32 jewelAttr1 asm("r0");
              jewelNewX = (attr + jewel2XTable[*jewel2State]) & 0x1FF;
              jewelOldAttr1 = oam->all.attr1;
              jewelAttr1 = jewel2PreserveMask;
              jewelAttr1 &= jewelOldAttr1;
              jewelAttr1 |= jewelNewX;
              oam->all.attr1 = jewelAttr1;
            }
            *(dest++) = *(src++);
            {
              register u8 jewel2PriorityByte asm("r1") = ((u8 *) oam)[5];
              register unsigned short jewel2PriorityOut asm("r0") = jewel2PriorityMask;
              jewel2PriorityOut &= jewel2PriorityByte;
              ((u8 *) oam)[5] = jewel2PriorityOut;
            }
            dest++;
            oam++;
            currentSlot--;
          }
          while (currentSlot != 0);
          currentSlot = nextSlot;
        }
        }
      }
    }
      {
        register s32 footerOffset asm("r1");
        asm("add %0, #4" : "=r"(footerOffset) : "0"(tableOffset));
        tableOffset = footerOffset;
      }
      {
        register s32 footerI asm("r2") = i;
        footerI++;
        i = footerI;
        if (footerI > 3)
          break;
      }
    }
  }

  {
    register u8 *allCollectedPtr asm("r5") = &gPauseAllJewelPiecesCollected;
    if (*allCollectedPtr != 0)
    {
      src = sUnk_86D3DB0[0].oam;
      nextSlot += *(src++);
      if (currentSlot < nextSlot)
      {
        OamData *oam;
        register OamData *cdOamBase asm("r1") = gOamBuffer;
        const s32 *allXTable;
        register const s32 *allXTableLoad asm("r0") = sUnk_86D36CC;
        s32 allPriorityMask;
        register s32 allPriorityMaskLoad asm("r2");
        u8 *allState;
        u8 *allStateLoad;
        register s32 allXMask asm("r5");
        // This too, couldn't figure out how to get this to match without using inline assembly.
        asm(
            "mov %0, %2\n\t"
            "mov %1, #13\n\t"
            "neg %1, %1"
            : "=r"(allXTable), "=r"(allPriorityMaskLoad)
            : "r"(allXTableLoad));
        allPriorityMask = allPriorityMaskLoad;
        allStateLoad = &gUnk_3003C4A;
        asm("" : "+r"(allStateLoad));
        allState = allStateLoad;
        oam = (OamData *) ((currentSlot << 3) + (u32) cdOamBase);
        allXMask = 0x1FF;
        currentSlot = nextSlot - currentSlot;
        do
        {
          attr = *(src++);
          *(dest++) = attr;
          oam->split.y = (attr + 88) & 0xFF;
          attr = *(src++);
          *(dest++) = attr;
          {
            u8 *allStateRead = allState;
            s32 allNewX;
            u16 allOldAttr1;
            s32 allAttr1;
            allNewX = (attr + allXTable[*allStateRead]) & allXMask;
            allOldAttr1 = oam->all.attr1;
            allAttr1 = -0x200;
            allAttr1 &= allOldAttr1;
            allAttr1 |= allNewX;
            oam->all.attr1 = allAttr1;
          }
          *(dest++) = *(src++);
          {
            register u8 allPriorityByte asm("r1") = ((u8 *) oam)[5];
            register unsigned short allPriorityOut asm("r0") = allPriorityMask;
            allPriorityOut &= allPriorityByte;
            ((u8 *) oam)[5] = allPriorityOut;
          }
          dest++;
          oam++;
          currentSlot--;
        }
        while (currentSlot != 0);
        currentSlot = nextSlot;
      }
    }
  }
  {
  u8 *collectedKeyzer = &gCollectedKeyzer;
  s32 collectedKeyzerValue;
  collectedKeyzerValue = *collectedKeyzer;
  if ((collectedKeyzerValue == 1) || gCurrentCollection[gCurrentPassage][gCurrentStageNumber].keyzer)
  {
    if (collectedKeyzerValue == 1)
    {
      animation = sUnk_86D3CF8;
    }
    else
    {
      animation = sUnk_86D3D08;
    }
    gPauseKeyzerAnimationState.animationTimer++;
    if (animation[gPauseKeyzerAnimationState.animationFrame].time < gPauseKeyzerAnimationState.animationTimer)
    {
      gPauseKeyzerAnimationState.animationTimer = 1;
      gPauseKeyzerAnimationState.animationFrame++;
      if (animation[gPauseKeyzerAnimationState.animationFrame].time == 0)
      {
        gPauseKeyzerAnimationState.animationFrame = 0;
      }
    }
    src = animation[gPauseKeyzerAnimationState.animationFrame].oam;
    nextSlot += *(src++);
    {
      s32 keySlotCheck = nextSlot;
      if (keySlotCheck > new_var)
      {
        return;
      }
    }
    if (currentSlot < nextSlot)
    {
      register OamData *keyOamBase asm("r1") = gOamBuffer;
      const s32 *keyXTable;
      const s32 *keyXTableLoad = sUnk_86D36D4;
      s32 keyPriorityMask;
      register s32 keyPriorityMaskLoad asm("r2");
      OamData *oam;
      u8 *keyState;
      register u8 *keyStateLoad asm("r5");
      register s32 keyXMask asm("r5");
      u32 keyOffset;
      // This too, couldn't figure out how to get this to match without using inline assembly.
      asm(
          "mov %0, %2\n\t"
          "mov %1, #13\n\t"
          "neg %1, %1"
          : "=r"(keyXTable), "=r"(keyPriorityMaskLoad)
          : "r"(keyXTableLoad));
      keyPriorityMask = keyPriorityMaskLoad;
      keyStateLoad = &gUnk_3003C4A;
      asm("" : "+r"(keyStateLoad));
      keyState = keyStateLoad;
      keyOffset = currentSlot << 3;
      oam = (OamData *) (keyOffset + (u32) keyOamBase);
      keyXMask = 0x1FF;
      asm("" : : "r"(keyXMask));
      {
        s32 keyCount = nextSlot;
        asm("" : "+r"(keyCount));
        keyCount -= currentSlot;
        currentSlot = keyCount;
      }
      do
      {
        attr = *(src++);
        *(dest++) = attr;
        yMask = 0xFF;
        oam->split.y = (attr + 96) & 0xFF;
        attr = *(src++);
        *(dest++) = attr;
        {
          u8 *keyStateRead = keyState;
          oam->split.x = (attr + keyXTable[*keyStateRead]) & 0x1FF;
        }
        *(dest++) = *(src++);
        {
          register u8 keyPriorityByte asm("r1") = ((u8 *) oam)[5];
          register unsigned short keyPriorityOut asm("r0") = keyPriorityMask;
          keyPriorityOut &= keyPriorityByte;
          ((u8 *) oam)[5] = keyPriorityOut;
        }
        dest++;
        oam++;
        currentSlot--;
      }
      while (currentSlot != 0);
      currentSlot = nextSlot;
    }
  }
  }
  cdState = (u8) (gCollectedCD - 1);
  if (((cdState <= 1) || gCurrentCollection[gCurrentPassage][gCurrentStageNumber].cd) && (gUnk_3003C4A == 0))
  {
    if (cdState <= 1)
    {
      animation = sUnk_86D3F80;
    }
    else
    {
      animation = sUnk_86D3FD0;
    }
    gPauseCdAnimationState.animationTimer++;
    if (animation[gPauseCdAnimationState.animationFrame].time < gPauseCdAnimationState.animationTimer)
    {
      gPauseCdAnimationState.animationTimer = 1;
      gPauseCdAnimationState.animationFrame++;
      if (animation[gPauseCdAnimationState.animationFrame].time == 0)
      {
        gPauseCdAnimationState.animationFrame = 0;
      }
    }
    src = animation[gPauseCdAnimationState.animationFrame].oam;
    nextSlot += *(src++);
    cdSlotCheck = nextSlot;
    if (cdSlotCheck > new_var)
    {
      return;
    }
    if (currentSlot < nextSlot)
    {
      OamData *oam;
      register OamData *cdBase asm("r1") = gOamBuffer;
      s32 cdXMask = 0x1FF;
      register s32 cdPreserveMask asm("r9") = -0x200;
      s32 cdPriorityMask = -13;
      oam = (OamData *) ((currentSlot << 3) + (u32) cdBase);
      currentSlot = cdSlotCheck - currentSlot;
      do
      {
        attr = *(src++);
        *(dest++) = attr;
        oam->split.y = (attr + 88) & 0xFF;
        attr = *(src++);
        *(dest++) = attr;
        {
          s32 cdNewX;
          register u16 cdOldAttr1 asm("r2");
          register s32 cdAttr1 asm("r0");
          cdNewX = (attr + 200) & cdXMask;
          cdOldAttr1 = oam->all.attr1;
          cdAttr1 = cdPreserveMask;
          cdAttr1 &= cdOldAttr1;
          cdAttr1 |= cdNewX;
          oam->all.attr1 = cdAttr1;
        }
        *(dest++) = *(src++);
        {
          register u8 cdPriorityByte asm("r1") = ((u8 *) oam)[5];
          register unsigned short cdPriorityOut asm("r0") = cdPriorityMask;
          cdPriorityOut &= cdPriorityByte;
          ((u8 *) oam)[5] = cdPriorityOut;
        }
        dest++;
        oam++;
        currentSlot--;
      }
      while (currentSlot != 0);
      currentSlot = nextSlot;
    }
  }
  score = gHighScoreTable[gCurrentPassage][gCurrentStageNumber];
  if (score > 599)
  {
    const struct AnimationFrame *scoreAnimation;
    if (score > 999)
    {
      scoreAnimation = sUnk_86D4008;
    }
    else
      if (score > 799)
    {
      scoreAnimation = sUnk_86D4018;
    }
    else
    {
      new_var5 = sUnk_86D4028;
      scoreAnimation = new_var5;
    }
    src = scoreAnimation->oam;
    nextSlot += *(src++);
    if (nextSlot > new_var)
    {
      return;
    }
    if (currentSlot < nextSlot)
    {
      register OamData *scoreOamBase asm("r1") = gOamBuffer;
      s32 scoreXMask = 0x1FF;
      register s32 scorePreserveMask asm("r9") = -0x200;
      s32 scorePriorityMask = -13;
      OamData *oam;
      {
        u32 scoreOffset = currentSlot << 3;
        oam = (OamData *) (scoreOffset + (u32) scoreOamBase);
      }
      currentSlot = nextSlot - currentSlot;
      do
      {
        attr = *(src++);
        *(dest++) = attr;
        oam->split.y = (attr + 48) & 0xFF;
        attr = *(src++);
        *(dest++) = attr;
        {
          s32 scoreNewX;
          register u16 scoreOldAttr1 asm("r2");
          register s32 scoreAttr1 asm("r0");
          scoreNewX = (attr + 168) & scoreXMask;
          scoreOldAttr1 = oam->all.attr1;
          scoreAttr1 = scorePreserveMask;
          scoreAttr1 &= scoreOldAttr1;
          scoreAttr1 |= scoreNewX;
          oam->all.attr1 = scoreAttr1;
        }
        *(dest++) = *(src++);
        {
          register u8 scorePriorityByte asm("r1");
          register u8 scorePriorityOut asm("r0");
          scorePriorityByte = ((u8 *) oam)[5];
          asm("" : "+r"(scorePriorityByte));
          scorePriorityOut = scorePriorityMask;
          scorePriorityOut &= scorePriorityByte;
          ((u8 *) oam)[5] = scorePriorityOut;
        }
        dest++;
        oam++;
        currentSlot--;
      }
      while (currentSlot != 0);
      currentSlot = nextSlot;
    }
  }
  {
    s32 finalSlot = nextSlot;
    gOamSlotsUsed = finalSlot;
  }
}

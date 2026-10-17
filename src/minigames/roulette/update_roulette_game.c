#include "minigames/roulette.h"

#include "minigame.h"

int UpdateRouletteGame(void)
{
    register u8 *statePtr;
    register u8 *stateLoad;
    u32 sound;
    s8 *fadePtr;
    u8 fadeRaw;
    s32 fade;
    int parity;
    u32 state;

    stateLoad = &gMinigameState;
    state = *stateLoad;
    statePtr = stateLoad;
    switch (state) {
    case 0:
        if (MinigameWaitForFrames(40) == 0)
            return 0;
        {
            register int barrier PIN("r2");
            ASM_BARRIER_VOLATILE(: "=r"(barrier));
        }
        goto advanceStateReload;

    case 1:
        gMinigameCameraState.scale -= 20;
        if (gMinigameCameraState.scale > 31)
            return 0;
        gMinigameCameraState.scale = 32;
        gMinigameCameraState.active = 0;
        goto advanceStateCached;

    case 2:
        if (MinigameWaitForFrames(60) == 0)
            return 0;
        { 
            register int barrier PIN("r2");
            ASM_BARRIER_VOLATILE(: "=r"(barrier));
        }
        goto advanceStateReload;

    case 3:
        gUnk_300188E++;
        if ((gUnk_300188E & 1) != 0)
            return 0;
        fadePtr = &gRouletteBlendEvb;
        fadeRaw = *(u8 *)fadePtr;
        fade = *fadePtr;
        if (fade > 0)
            goto fadeForward;
        gRouletteTopResult.frame = gRouletteWinningValues[0];
        gRouletteMiddleResult.frame = gRouletteWinningValues[1];
        gRouletteBottomResult.frame = gRouletteWinningValues[2];
        RefreshRouletteResultTiles();
        goto advanceStateReload;

    case 4:
        gUnk_300188E++;
        if ((gUnk_300188E & 1) != 0)
            return 0;
        if (gRouletteBlendEvb > 15)
            goto advanceStateCached;
        gRouletteBlendEva--;
        gRouletteBlendEvb++;
        return 0;

    case 5:
        {
            u16 *timerPtr;
            register int timer PIN("r2");
            s16 period;

            timerPtr = &gMinigameSequenceTimer;
            (*timerPtr)++;
            timer = *timerPtr;
            period = gRouletteTickPeriod;
            if (timer == period * 4) {
                *timerPtr = 0;
                m4aSongNumStart(592);
                sound = 590;
                goto playSoundAndAdvance;
            }
            if (__modsi3(timer, period) == 0)
                m4aSongNumStart(591);
        }
        return 0;

    case 6:
        {
            parity = (u32)&gUnk_300188E;
            fade = *(u16 *)parity;
            fade += 1;
            *(u16 *)parity = fade;
            parity = 1;
            parity &= fade;
            if (parity != 0)
                return 0;
            fadePtr = &gRouletteBlendEvb;
            fadeRaw = *(u8 *)fadePtr;
            fade = *fadePtr;
            if (fade <= 0)
                goto fadeFinished;
        }

fadeForward:
        fade = (u32)&gRouletteBlendEva;
        parity = *(u8 *)fade;
        parity += 1;
        *(u8 *)fade = parity;
        fade = fadeRaw - 1;
        *fadePtr = fade;
        return 0;

fadeFinished:
        gRouletteBlendEva = parity;
        *fadePtr = 16;
        gRouletteStopRequested = parity;

advanceStateCached:
        (*statePtr)++;
        return 0;

    case 7:
        UpdateRouletteWheel();
        if (IsRouletteConfirmPressed() == 0)
            return 0;
        SortRouletteItemsByScale();
        {
            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base;
            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) ref;
            register int hiddenX PIN("r2");
            int count;
            struct RouletteItem *hidden;

            base = gRouletteItemDrawOrder;
            hiddenX = 250;
            hiddenX <<= 2;
            ref = base + 1;
            count = 1;
            do {
                hidden = GP(*ref++);
                hidden->x = hiddenX;
                count--;
            } while (count >= 0);
        }
        m4aSongNumStart(587);
        {
            struct RouletteSmallState *result;
            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base;
            struct RouletteItem *item;

            result = &gRouletteTopResult;
            base = gRouletteItemDrawOrder;
            item = GP(base[0]);
            result->frame = item->value;
            gRouletteMoveStartX = item->x;
            gRouletteMoveStartY = item->y;
            gRouletteMoveStartTile = item->scale;
            gRouletteAngularAcceleration = 0;
        }
        UpdateRouletteResultTiles();
        goto advanceStateReload;

    case 8:
        {
            int done;
            done = MoveRouletteResultIntoSlot();
            if (done != 1)
                return 0;
            {
                register u8 *caseState PIN("r1");
                caseState = &gMinigameState;
                (*caseState)++;
            }
            ShuffleRouletteValuePool();
            gRouletteStopRequested = done;
        {
            u16 *itemHalfwords;
            int zero;
            register int angle PIN("r2");
            int count;

            itemHalfwords = (u16 *)gRouletteItems;
            zero = 0;
            angle = 224;
            angle <<= 5;
            itemHalfwords += 2;
            count = 2;
            do {
                itemHalfwords[2] = angle;
                itemHalfwords[0] = zero;
                itemHalfwords += 6;
                count--;
            } while (count >= 0);
        }
            return 0;
        }

    case 9:
        UpdateRouletteWheel();
        if (IsRouletteConfirmPressed() == 0)
            return 0;
        SortRouletteItemsByScale();
        {
            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base;
            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) ref;
            register int hiddenX PIN("r2");
            int count;
            struct RouletteItem *hidden;

            base = gRouletteItemDrawOrder;
            hiddenX = 250;
            hiddenX <<= 2;
            ref = base + 1;
            count = 1;
            do {
                hidden = GP(*ref++);
                hidden->x = hiddenX;
                count--;
            } while (count >= 0);
        }
        m4aSongNumStart(588);
        {
            struct RouletteSmallState *result;
            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base;
            struct RouletteItem *item;

            result = &gRouletteMiddleResult;
            base = gRouletteItemDrawOrder;
            item = GP(base[0]);
            result->frame = item->value;
            gRouletteMoveStartX = item->x;
            gRouletteMoveStartY = item->y;
            gRouletteMoveStartTile = item->scale;
            gRouletteAngularAcceleration = 0;
        }
        UpdateRouletteResultTiles();
        {
            register int barrier PIN("r2");
            ASM_BARRIER_VOLATILE(: "=r"(barrier));
        }
        goto advanceStateReload;

    case 10:
        if (MoveRouletteResultIntoSlot() == 0)
            return 0;
        gMinigameState++;
        ShuffleRouletteValuePool();
        gRouletteStopRequested = 2;
        {
            u16 *itemHalfwords;
            int zero;
            register int angle PIN("r2");
            int count;

            itemHalfwords = (u16 *)gRouletteItems;
            zero = 0;
            angle = 224;
            angle <<= 5;
            itemHalfwords += 2;
            count = 2;
            do {
                itemHalfwords[2] = angle;
                itemHalfwords[0] = zero;
                itemHalfwords += 6;
                count--;
            } while (count >= 0);
        }
        return 0;

    case 11:
        UpdateRouletteWheel();
        if (IsRouletteConfirmPressed() == 0)
            return 0;
        SortRouletteItemsByScale();
        {
            register GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base PIN("r0");
            register GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) ref PIN("r3");
            int hiddenX;
            int count;
            struct RouletteItem *hidden;

            base = gRouletteItemDrawOrder;
            hiddenX = 250;
            hiddenX <<= 2;
            ref = base + 1;
            count = 1;
            do {
                hidden = GP(*ref++);
                hidden->x = hiddenX;
                count--;
            } while (count >= 0);
        }
        m4aSongNumStart(589);
        {
            struct RouletteSmallState *result;
            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base;
            struct RouletteItem *item;

            result = &gRouletteBottomResult;
            base = gRouletteItemDrawOrder;
            item = GP(base[0]);
            result->frame = item->value;
            gRouletteMoveStartX = item->x;
            gRouletteMoveStartY = item->y;
            gRouletteMoveStartTile = item->scale;
            gRouletteAngularAcceleration = 0;
        }
        UpdateRouletteResultTiles();
        {
            register int barrier PIN("r2");
            ASM_BARRIER_VOLATILE(: "=r"(barrier));
        }
        goto advanceStateReload;

    case 12:
        if (MoveRouletteResultIntoSlot() == 0)
            return 0;
        if (gRouletteTopResult.frame == gRouletteWinningValues[0] &&
            gRouletteMiddleResult.frame == gRouletteWinningValues[1] &&
            gRouletteBottomResult.frame == gRouletteWinningValues[2]) {
            sound = 582;
            goto playSoundAndAdvance;
        }
        goto rouletteMismatch;

playSoundAndAdvance:
        ((void (*)())m4aSongNumStart)(sound);
advanceStateReload:
        gMinigameState++;
        return 0;

rouletteMismatch:
        m4aSongNumStart(583);
        /* Separate alias prevents agbcc from reusing the cached r4 state pointer. */
        gMinigameState = 14;
        {
            const struct MusicPlayer *mplay;
            const u8 *songBase;
            int songOffset;
            u16 songIndex;

            mplay = gMPlayTable;
            songBase = (const u8 *)gSongTable;
            songOffset = 0x1624;
            songBase += songOffset;
            songIndex = *(u16 *)songBase;
            m4aMPlayFadeOut(GP(mplay[songIndex].info), 8);
        }
        return 0;

    case 13:
        if (UpdateRouletteResults() != 0)
            gMinigameState = 5;
        return 0;

    case 14:
        if (UpdateRouletteIntro() != 0)
            return 1;
        return 0;
    }
    return 0;

}

#include "minigames/roulette.h"

int MoveRouletteResultIntoSlot(void)
{
    register u8 *progressPtr;
    s16 *tileStepPtr;
    register u16 *tileBasePtr PIN("r5");
    register s16 *xStepPtr PIN("r6");
    register u16 *xBasePtr PIN("r8");
    s16 *yStepPtr;
    u16 *yBasePtr;
    u8 progress;

    {
        u8 *progressAddress;
        register u8 initialProgress PIN("r1");
        progressAddress = &gRouletteMoveProgress;
        initialProgress = *progressAddress;
        progressPtr = progressAddress;
        progress = initialProgress;
    }
    tileStepPtr = &gRouletteMoveStepTile;
    tileBasePtr = &gRouletteMoveStartTile;
    xStepPtr = &gRouletteMoveStepX;
    xBasePtr = &gRouletteMoveStartX;
    yStepPtr = &gRouletteMoveStepY;
    yBasePtr = &gRouletteMoveStartY;

    if (progress == 0) {
        *tileStepPtr = __divsi3(((s16)*tileBasePtr - 256) << 5, 30);
        *xStepPtr = __divsi3((122 - (s16)*xBasePtr) << 5, 30);

        switch (gMinigameState) {
        case 8:
            *yStepPtr = __divsi3((88 - (s16)*yBasePtr) << 5, 30);
            break;
        case 10:
            {
                s16 *base;
                s32 baseValue;
                base = (s16 *)yBasePtr;
                baseValue = *base;
                *yStepPtr = __divsi3((106 - baseValue) << 5, 30);
            }
            break;
        case 12:
            {
                s16 *base;
                s32 baseValue;
                base = (s16 *)yBasePtr;
                baseValue = *base;
                *yStepPtr = __divsi3((116 - baseValue) << 5, 30);
            }
            break;
        }
    }

    progress = *progressPtr + 1;
    *progressPtr = progress;

    {
        GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) refs;
        struct RouletteItem *item;
        s32 step;
        register s32 value PIN("r0");

        refs = gRouletteItemDrawOrder;
        item = GP(refs[0]);
        step = *tileStepPtr;
        value = *progressPtr;
        step *= value;
        step >>= 5;
        value = *tileBasePtr;
        value -= step;
        item->scale = value;

        item = GP(refs[0]);
        {
            register s32 zero PIN("r4");

            ASM_BARRIER_VOLATILE(: "=r"(zero));
            zero = 0;
            step = *(s16 *)((u8 *)xStepPtr + zero);
        }
        value = *progressPtr;
        value *= step;
        value >>= 5;
        /* Prevent agbcc from moving the r8 pointer copy before the shift. */
        ASM_BARRIER_VOLATILE(: "+r"(value));
        step = *xBasePtr;
        value += step;
        item->x = value;

        item = GP(refs[0]);
        {
            register s16 *stepBase PIN("r4");
            u16 zero;

            stepBase = yStepPtr;
            zero = 0;
            ASM_BARRIER_VOLATILE(: : "r"(stepBase));
            step = *(s16 *)((u8 *)stepBase + zero);
        }
        value = *progressPtr;
        value *= step;
        value >>= 5;
        /* Prevent agbcc from moving the sl pointer copy before the shift. */
        step = *yBasePtr;
        value += step;
        item->y = value;

        if (*progressPtr == 30) {
            u32 state;
            state = gMinigameState;
            if ((state == 8 && GP(refs[0])->value == gRouletteWinningValues[0]) ||
                (state == 10 && GP(refs[0])->value == gRouletteWinningValues[1]) ||
                (state == 12 && GP(refs[0])->value == gRouletteWinningValues[2])) {
                m4aSongNumStart(580);
            } else {
                m4aSongNumStart(581);
            }

            {
                u8 *resetProgress;
                register GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) resetRefs PIN("r12");
                register u8 *selectedPtr PIN("r8");
                register u8 *sequencePtr PIN("r9");
                register u16 *yPtr PIN("r0");
                u8 *statePtr;
                int zero;
                int yForState8;
                int yForState10;
                register int count PIN("r3");
                GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) refsLoad;
                u8 *selectedLoad;
                u8 *sequenceLoad;
                register struct RouletteItem *itemBase PIN("r0");

                resetProgress = &gRouletteMoveProgress;
                refsLoad = gRouletteItemDrawOrder;
                resetRefs = refsLoad;
                selectedLoad = &gRouletteSelectedItemIndex;
                selectedPtr = selectedLoad;
                sequenceLoad = &gRouletteValueSequenceIndex;
                sequencePtr = sequenceLoad;
                itemBase = gRouletteItems;
                zero = 0;
                yPtr = (u16 *)itemBase;
                yPtr++;
                statePtr = &gMinigameState;
                yForState8 = 106;
                yForState10 = 116;
                count = 2;
                do {
                    ((u8 *)yPtr)[5] = zero;
                    switch (*statePtr) {
                    case 8:
                        *yPtr = yForState8;
                        break;
                    case 10:
                        *yPtr = yForState10;
                        break;
                    }
                    yPtr += 6;
                    count--;
                } while (count >= 0);

                {
                    register GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) refsLow PIN("r1");
                    struct RouletteItem *first;
                    int clear;
                    register u8 *selectedLow PIN("r2");
                    register u8 *sequenceLow PIN("r4");

                    refsLow = resetRefs;
                    first = GP(refsLow[0]);
                    clear = 0;
                    first->x = 1000;
                    *resetProgress = clear;
                    selectedLow = selectedPtr;
                    *selectedLow = clear;
                    sequenceLow = sequencePtr;
                    *sequenceLow = clear;
                }
            }
            return 1;
        }
    }
    return 0;
}

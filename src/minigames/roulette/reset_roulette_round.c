#include "minigames/roulette.h"
#include "minigame.h"

void ResetRouletteRound(void)
{
    volatile u32 *dma;
    u16 remainder;

    dma = (volatile u32 *)0x040000D4;
    dma[0] = (u32)sRouletteTopResultTiles;
    dma[1] = 0x06013000;
    dma[2] = 0x80002000;
    dma[2];

    {
        s32 value;
        u16 *angularSpeedAddress;
        angularSpeedAddress = &gRouletteAngularSpeed;
        value = (s16)*(volatile u16 *)angularSpeedAddress;
        if (value > 100)
            *angularSpeedAddress = 100;
    }

    remainder = __umodsi3(gMinigameScore, 10);
    if (remainder == 0)
    {
        gRouletteValueCount++;
        if (gRouletteValueCount > 15)
            gRouletteValueCount = 15;
    }

    gRouletteTickPeriod -= 5;
    if ((s16)gRouletteTickPeriod <= 2)
        gRouletteTickPeriod = 48;

    gRoulettePromptAnim.timer = 0;
    gRoulettePromptAnim.frame = 0;
    gRouletteMoveProgress = 0;
    gRouletteAngularAcceleration = 0;
    gRouletteValueSequenceIndex = 0;
    gRouletteSelectedItemIndex = 0;
    gRouletteStopRequested = 0;
    gRouletteFrameCounter = 0;
    gRouletteTopResult.y = 88;
    gRouletteMiddleResult.y = 106;
    gRouletteBottomResult.y = 116;
    ShuffleRouletteOamTable();
    ShuffleRouletteValuePool();

    {
        register s32 tileValue PIN("r0");
        s32 tileOffset;
        struct RouletteItem *item;
        s32 y;
        s32 x;
        register s32 remaining PIN("r5");
        s32 angle;
        register s32 inactive;

        inactive = 0;
        angle = 7168;
        tileOffset = 0;
        tileValue = sRouletteInitialTileSin;
        item = gRouletteItems;
        x = 1000;
        y = 88;
        tileOffset = 256;
        tileValue += tileOffset;
        tileValue = ((s32)(tileValue + ((u32)tileValue >> 31)) >> 1) << 1;
        remaining = 2;
        do
        {
            item->x = x;
            item->y = y;
            item->angle = angle;
            item->scale = tileValue;
            item->active = inactive;
            item++;
            remaining--;
        } while (remaining >= 0);
    }

    {
        GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base;
        struct RouletteItem *item;
        register GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) dst PIN("r1");
        base = gRouletteItemDrawOrder;
        item = gRouletteItems;
        item += 2;
        dst = base;
        dst += 2;
        do
        {
            GP_SET(*dst, item);
            item--;
            dst--;
        } while ((s32)dst >= (s32)base);
    }

    {
        s32 index;
        register u8 *values PIN("r6");
        u8 *dst;
        register s32 value PIN("r0");

        index = 0;
        do
        {
            value = MinigameRandom();
            values = gRouletteWinningValues;
            dst = (u8 *)(index + (u32)values);
            value = __modsi3(value, gRouletteValueCount);
            *dst = value;
            index++;
        } while (index <= 2);

        {
            struct RouletteSmallState *label;
            u8 value;
            label = &gRouletteTopResult;
            value = values[0];
            label->frame = value;
            label = &gRouletteMiddleResult;
            value = values[1];
            label->frame = value;
            label = &gRouletteBottomResult;
            value = values[2];
            label->frame = value;
        }
    }
    RefreshRouletteResultTiles();
}

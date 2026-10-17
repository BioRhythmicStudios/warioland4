#include "minigames/roulette.h"
#include "minigame.h"

void ShuffleRouletteOamTable(void)
{
    int i;
    int index;
    u8 swappedOrder;

    {
        GBA_TYPEOF(const u16 **, &(gRouletteItemOamTable)[0]) dst;
        register GBA_TYPEOF(const u16 **, &(sRouletteItemOamFrames)[0]) frames PIN("r0");
        dst = gRouletteItemOamTable;
        frames = sRouletteItemOamFrames;
        GP_SET(*dst, GP(*frames));
    }

    {
        u8 *order;
        order = gRouletteOamOrder;
        /* Prevent agbcc from folding +13 into the relocation addend. */
        ASM_BARRIER_VOLATILE(: "+r"(order));
        i = 13;
        order += 13;
        do {
            *order-- = i;
            i--;
        } while (i >= 0);
    }

    {
        register u8 *order PIN("r5");
        u8 *current;
        u8 *random;
        i = 13;
        order = gRouletteOamOrder;
        do {
            index = __modsi3(MinigameRandom(), i);
            current = (u8 *)((unsigned int)i + (unsigned int)order);
            swappedOrder = *current;
            random = (u8 *)index;
            random = (u8 *)((u32)random + (u32)order);
            *current = *random;
            *random = swappedOrder;
            i--;
        } while (i > 0);
    }

    {
        GBA_TYPEOF(const u16 **, &(gRouletteItemOamTable)[0]) dstBase;
        GBA_TYPEOF(const u16 **, &(sRouletteItemOamFrames)[0]) frames;
        u8 *order;
        GBA_TYPEOF(const u16 **, &(gRouletteItemOamTable)[0]) dst;
        dstBase = gRouletteItemOamTable;
        frames = sRouletteItemOamFrames;
        order = gRouletteOamOrder;
        dst = dstBase + 1;
        i = 13;
        do {
            int frameIndex;
            frameIndex = *order;
            frameIndex++;
            GP_SET(*dst++, GP(frames[frameIndex]));
            order++;
            i--;
        } while (i >= 0);
    }
}

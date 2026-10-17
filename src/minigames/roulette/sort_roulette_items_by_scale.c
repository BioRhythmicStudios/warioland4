#include "minigames/roulette.h"

void SortRouletteItemsByScale(void)
{
    int i;
    int j;
    int selected;
    s16 selectedScale;
    struct RouletteItem *swappedItem;

    {
        GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) base;
        struct RouletteItem *itemPtr;
        GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) refPtr;
        base = gRouletteItemDrawOrder;
        itemPtr = gRouletteItems;
        /* Keep the data relocation unmodified; the target adds 24 at runtime. */
        ASM_BARRIER_VOLATILE(: "+r"(itemPtr));
        itemPtr += 2;
        refPtr = base;
        refPtr += 2;
        do {
            GP_SET(*refPtr, itemPtr);
            itemPtr--;
            refPtr--;
        } while ((int)refPtr >= (int)base);
    }

    for (i = 0; i <= 1; i++) {
        selectedScale = GP(gRouletteItemDrawOrder[i])->scale;
        selected = i;
        for (j = i + 1; j <= 2; j++) {
            if (GP(gRouletteItemDrawOrder[j])->scale > selectedScale) {
                selectedScale = GP(gRouletteItemDrawOrder[j])->scale;
                selected = j;
            }
        }
        swappedItem = GP(gRouletteItemDrawOrder[i]);
        GP_SET(gRouletteItemDrawOrder[i], GP(gRouletteItemDrawOrder[selected]));
        GP_SET(gRouletteItemDrawOrder[selected], swappedItem);
    }
}

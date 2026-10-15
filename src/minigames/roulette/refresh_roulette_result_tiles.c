#include "minigames/roulette.h"

void RefreshRouletteResultTiles(void)
{
    s32 idx;
    s32 i;
    u8 *src;
    u8 *dst;
    volatile u32 *dma;

    idx = GetRouletteFrameIndex(gRouletteTopResult.frame);
    src = sRouletteTopResultTiles + (idx % 4) * 0x100 + (idx / 4) * 0x1000;
    dst = (u8 *)0x06017000;
    for (i = 3; i >= 0; i--) {
        dma = (volatile u32 *)0x040000D4;
        dma[0] = (u32)src;
        dma[1] = (u32)dst;
        dma[2] = 0x80000080;
        dma[2];
        while (dma[2] & 0x80000000)
            ;
        dst += 0x400;
        src += 0x400;
    }

    idx = GetRouletteFrameIndex(gRouletteMiddleResult.frame);
    src = sRouletteMiddleResultTiles + (idx % 4) * 0x100 + (idx / 4) * 0x1000;
    dst = (u8 *)0x06017100;
    for (i = 3; i >= 0; i--) {
        dma = (volatile u32 *)0x040000D4;
        dma[0] = (u32)src;
        dma[1] = (u32)dst;
        dma[2] = 0x80000080;
        dma[2];
        while (dma[2] & 0x80000000)
            ;
        dst += 0x400;
        src += 0x400;
    }

    idx = GetRouletteFrameIndex(gRouletteBottomResult.frame);
    src = sRouletteBottomResultTiles + (idx % 4) * 0x100 + (idx / 4) * 0x1000;
    for (i = 0; i <= 3; i++) {
        dma = (volatile u32 *)0x040000D4;
        dma[0] = (u32)(src + (i << 10));
        dma[1] = (u32)((u8 *)0x06017200 + (i << 10));
        dma[2] = 0x80000080;
        dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
}

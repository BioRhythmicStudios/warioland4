#include "minigames/roulette.h"

void RefreshRouletteResultTiles(void)
{
    s32 idx, quotient, i;
    u32 remainder;
    u32 src, dst;
    volatile u32 *dma;

    idx = GetRouletteFrameIndex(gRouletteTopResult.frame);
    quotient = idx / 4;
    remainder = idx - quotient * 4;
    quotient <<= 12;
    src = (u32)sRouletteTopResultTiles + (remainder << 8);
    src += quotient;
    dst = 0x06017000;
    for (i = 0x1000; i != 0; i -= 0x400) {
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
    quotient = idx / 4;
    remainder = idx - quotient * 4;
    quotient <<= 12;
    src = (u32)sRouletteMiddleResultTiles + (remainder << 8);
    src += quotient;
    dst = 0x06017100;
    for (i = 0x1000; i != 0; i -= 0x400) {
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
    quotient = idx / 4;
    remainder = idx - quotient * 4;
    quotient <<= 12;
    src = (u32)sRouletteBottomResultTiles + (remainder << 8);
    src += quotient;
    for (i = 0; i <= 3; i++) {
        dma = (volatile u32 *)0x040000D4;
        dma[0] = (u32)(src + (i << 10));
        dma[1] = (u32)(0x06017200 + (i << 10));
        dma[2] = 0x80000080;
        dma[2];
        while (dma[2] & 0x80000000)
            ;
    }
}

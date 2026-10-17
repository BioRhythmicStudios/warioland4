void RefreshRouletteResultTiles(void)
{
    register struct RouletteSmallState *next asm("r8");
    {
        register s32 idx asm("r3");
        register s32 quotient asm("r2");
        register s32 offset asm("r0");
        offset = GetRouletteFrameIndex(gRouletteTopResult.frame);
        idx = offset;
        if (idx < 0) offset = idx + 3;
        offset >>= 2;
        quotient = offset;
        offset <<= 2;
        offset = idx - offset;
        quotient <<= 12;
        {
        register u32 src asm("r1");
        register u32 dst asm("r6");
        register volatile u32 *dma asm("r4");
        register u32 control asm("r12");
        register s32 i asm("r5");
        src = offset << 8;
        offset = (u32)sRouletteTopResultTiles;
        src += offset;
        next = &gRouletteMiddleResult;
        dma = (volatile u32 *)0x040000D4;
        control = 0x80000080;
        dst = 0x06017000;
        src = quotient - (-src);
        quotient = 0x400;
        i = 3;
        do {
            dma[0] = src;
            dma[1] = dst;
            dma[2] = control;
            dma[2];
            while (dma[2] & 0x80000000) ;
            dst += quotient;
            src += quotient;
            i--;
        } while (i >= 0);
        }
    }
    {
        register s32 idx asm("r3");
        register s32 quotient asm("r2");
        register s32 offset asm("r0");
        offset = GetRouletteFrameIndex(next->frame);
        idx = offset;
        if (idx < 0) offset = idx + 3;
        offset >>= 2;
        quotient = offset;
        offset <<= 2;
        offset = idx - offset;
        quotient <<= 12;
        {
        register u32 src asm("r1");
        register u32 dst asm("r6");
        register volatile u32 *dma asm("r4");
        register u32 control asm("r12");
        register s32 i asm("r5");
        src = offset << 8;
        offset = (u32)sRouletteMiddleResultTiles;
        src += offset;
        next = &gRouletteBottomResult;
        dma = (volatile u32 *)0x040000D4;
        control = 0x80000080;
        dst = 0x06017100;
        src = quotient - (-src);
        quotient = 0x400;
        i = 3;
        do {
            dma[0] = src;
            dma[1] = dst;
            dma[2] = control;
            dma[2];
            while (dma[2] & 0x80000000) ;
            dst += quotient;
            src += quotient;
            i--;
        } while (i >= 0);
        }
    }
    {
        register s32 idx asm("r3");
        register s32 quotient asm("r2");
        register s32 offset asm("r0");
        offset = GetRouletteFrameIndex(next->frame);
        idx = offset;
        if (idx < 0) offset = idx + 3;
        offset >>= 2;
        quotient = offset;
        offset <<= 2;
        offset = idx - offset;
        quotient <<= 12;
        {
        register u32 src asm("r4");
        register u32 dst asm("r12");
        register vu32 *dma asm("r2");
        register s32 i asm("r5");
        offset <<= 8;
        offset += (u32)sRouletteBottomResultTiles;
        src = quotient + offset;
        dst = 0x06017200;
        for (i = 0; i <= 3; i++) {
            dma = (vu32 *)REG_ADDR_DMA3;
            dma[0] = src + (i << 10);
            dma[1] = dst + (i << 10);
            dma[2] = 0x80000080;
            dma[2];
            while (dma[2] & 0x80000000)
                ;
        }
        }
    }
}

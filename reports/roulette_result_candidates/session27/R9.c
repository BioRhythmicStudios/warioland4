void RefreshRouletteResultTiles(void)
{
    register struct RouletteSmallState *next asm("r8");
    s32 idx;
    s32 quotient;
    s32 remainder, i;
    register u32 src asm("r1");
    register u32 dst asm("r6");
    register u32 control asm("r12");
    register vu32 *dma asm("r4");

    idx = GetRouletteFrameIndex(gRouletteTopResult.frame);
    quotient = idx / 4;
    remainder = idx % 4;
    quotient <<= 12;
    src = remainder << 8;
    src += (u32)sRouletteTopResultTiles;
    next = &gRouletteMiddleResult;
    dma = (vu32 *)REG_ADDR_DMA3;
    control = 0x80000080;
    dst = 0x06017000;
    src = quotient + src;
    for (i = 0; i < 4; i++) {
        dma[0] = src;
        dma[1] = dst;
        dma[2] = control;
        dma[2];
        while (dma[2] & 0x80000000)
            ;
        dst += 0x400;
        src += 0x400;
    }

    idx = GetRouletteFrameIndex(next->frame);
    quotient = idx / 4;
    remainder = idx % 4;
    quotient <<= 12;
    src = remainder << 8;
    src += (u32)sRouletteMiddleResultTiles;
    next = &gRouletteBottomResult;
    dma = (vu32 *)REG_ADDR_DMA3;
    control = 0x80000080;
    dst = 0x06017100;
    src = quotient + src;
    for (i = 0; i < 4; i++) {
        dma[0] = src;
        dma[1] = dst;
        dma[2] = control;
        dma[2];
        while (dma[2] & 0x80000000)
            ;
        dst += 0x400;
        src += 0x400;
    }

    {
        register u32 src3 asm("r4");
        register u32 dst3 asm("r12");
        register vu32 *dma3 asm("r2");
        u32 offset;

        idx = GetRouletteFrameIndex(next->frame);
        quotient = idx / 4;
        remainder = idx % 4;
        quotient <<= 12;
        offset = remainder << 8;
        offset += (u32)sRouletteBottomResultTiles;
        src3 = quotient + offset;
        dst3 = 0x06017200;
        for (i = 0; i <= 3; i++) {
            dma3 = (vu32 *)REG_ADDR_DMA3;
            dma3[0] = src3 + (i << 10);
            dma3[1] = dst3 + (i << 10);
            dma3[2] = 0x80000080;
            dma3[2];
            while (dma3[2] & 0x80000000)
                ;
        }
    }
}

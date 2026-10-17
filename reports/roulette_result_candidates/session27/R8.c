void RefreshRouletteResultTiles(void)
{
    register struct RouletteSmallState *next asm("r8");
    s32 idx, quotient, remainder, i;

    {
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
    }
    {
        register u32 src asm("r1");
        register u32 dst asm("r6");
        register u32 control asm("r12");
        register vu32 *dma asm("r4");

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
    }
    {
        register u32 src asm("r4");
        register u32 dst asm("r12");
        register vu32 *dma asm("r2");
        u32 offset;

        idx = GetRouletteFrameIndex(next->frame);
        quotient = idx / 4;
        remainder = idx % 4;
        quotient <<= 12;
        offset = remainder << 8;
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

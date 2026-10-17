void RefreshRouletteResultTiles(void)
{
    struct RouletteSmallState *next;
    s32 idx, quotient, i;
    u32 src, dst;

    idx = GetRouletteFrameIndex(gRouletteTopResult.frame);
    quotient = idx / 4;
    src = ((idx - quotient * 4) << 8) + (u32)sRouletteTopResultTiles;
    next = &gRouletteMiddleResult;
    dst = 0x06017000;
    src += quotient << 12;
    for (i = 0; i < 4; i++) {
        DmaSet(3, src, dst, 0x80000080);
        DmaWait(3);
        dst += 0x400;
        src += 0x400;
    }

    idx = GetRouletteFrameIndex(next->frame);
    quotient = idx / 4;
    src = ((idx - quotient * 4) << 8) + (u32)sRouletteMiddleResultTiles;
    next = &gRouletteBottomResult;
    dst = 0x06017100;
    src += quotient << 12;
    for (i = 0; i < 4; i++) {
        DmaSet(3, src, dst, 0x80000080);
        DmaWait(3);
        dst += 0x400;
        src += 0x400;
    }

    idx = GetRouletteFrameIndex(next->frame);
    quotient = idx / 4;
    src = ((idx - quotient * 4) << 8) + (u32)sRouletteBottomResultTiles;
    src += quotient << 12;
    for (i = 0; i <= 3; i++) {
        DmaSet(3, src + (i << 10), 0x06017200 + (i << 10), 0x80000080);
        DmaWait(3);
    }
}

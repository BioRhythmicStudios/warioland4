#include "minigames/roulette.h"

void AnimateRouletteResultTiles(void)
{
    register u32 phase PIN("r1");
    volatile u32 *dma;

    {
        volatile u16 *frameCounter;
        frameCounter = &gRouletteFrameCounter;
        phase = *frameCounter;
        phase++;
        *frameCounter = phase;
        phase = *frameCounter;
    }
    phase &= 15;
    if (phase == 0) {
        dma = (volatile u32 *)0x040000D4;
        dma[0] = (u32)sRouletteResultFlashTiles;
        dma[1] = 0x06017000;
        dma[2] = 0x80000800;
        dma[2];
    } else if (phase == 8) {
        int i;
        u8 *source0;
        u8 *source1;
        u8 *source2;
        volatile u32 *dmaRegs;
        register u32 control;
        u32 busy;

        i = 0;
        source0 = sRouletteTopResultTiles;
        source1 = sRouletteMiddleResultTiles;
        source2 = sRouletteBottomResultTiles;
        dmaRegs = (volatile u32 *)0x040000D4;
        control = 0x80000080;
        busy = 0x80000000;
        do {
            u32 offset;
            u32 value;
            int next;
            u8 *address;

            offset = i << 10;
            {
                u8 *base;
                base = source0;
                address = (u8 *)(offset + (u32)base);
            }
            dmaRegs[0] = (u32)address;
            {
                u8 *base;
                base = (u8 *)0x06017000;
                address = base + offset;
            }
            dmaRegs[1] = (u32)address;
            dmaRegs[2] = control;
            value = dmaRegs[2];
            value = dmaRegs[2];
            value &= busy;
            next = i + 1;
            if (value != 0) {
                u32 pollMask;
                pollMask = 0x80000000;
                do {
                    value = dmaRegs[2];
                    value &= pollMask;
                } while (value != 0);
            }

            {
                u8 *base;
                base = source1;
                address = (u8 *)(offset + (u32)base);
            }
            dmaRegs[0] = (u32)address;
            {
                u8 *base;
                base = (u8 *)0x06017100;
                address = offset + base;
            }
            dmaRegs[1] = (u32)address;
            dmaRegs[2] = control;
            value = dmaRegs[2];
            {
                volatile u32 *waitDma;
                waitDma = dmaRegs;
                value = dmaRegs[2];
                value &= busy;
                if (value != 0) {
                    register u32 pollMask PIN("r3");
                    pollMask = 0x80000000;
                    do {
                        value = waitDma[2];
                        value &= pollMask;
                    } while (value != 0);
                }
            }

            {
                register u8 *base PIN("r3");
                base = source2;
                address = (u8 *)(offset + (u32)base);
            }
            dmaRegs[0] = (u32)address;
            {
                u8 *base;
                base = (u8 *)0x06017200;
                address = offset + base;
            }
            dmaRegs[1] = (u32)address;
            dmaRegs[2] = control;
            value = dmaRegs[2];
            {
                register volatile u32 *waitDma PIN("r1");
                waitDma = dmaRegs;
                value = waitDma[2];
                value &= busy;
                if (value != 0) {
                    u32 pollMask;
                    pollMask = 0x80000000;
                    do {
                        value = waitDma[2];
                        value &= pollMask;
                    } while (value != 0);
                }
            }
            i = next;
        } while (i <= 3);
    }
}

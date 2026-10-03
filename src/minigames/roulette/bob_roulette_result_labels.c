#include "minigames/roulette.h"

void BobRouletteResultLabels(void)
{
    u16 value;
    u16 mask;
    u16 *frameCounter = (u16 *)&gRouletteFrameCounter;
    (*frameCounter)++;
    value = *frameCounter;
    mask = 0xF;
    value &= mask;
    if (value <= 3) {
        gRouletteTopResult.y++;
        gRouletteMiddleResult.y--;
        gRouletteBottomResult.y++;
    } else {
        u16 wrappedFrame = value;
        if ((u16)(wrappedFrame - 8) <= 3) {
            gRouletteTopResult.y--;
            gRouletteMiddleResult.y++;
            gRouletteBottomResult.y--;
        }
    }
}

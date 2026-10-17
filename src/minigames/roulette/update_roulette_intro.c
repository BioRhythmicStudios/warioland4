#include "minigames/roulette.h"

#include "minigame.h"

int UpdateRouletteIntro(void)
{
    switch (gMinigameSequenceState) {
    case 0:
        gMinigameSequenceTimer++;
        if (gMinigameSequenceTimer > 120) {
            gMinigameSequenceState++;
            gMinigameSequenceTimer = 0;
            gMinigameCameraState.active = 2;
        }
        break;
    case 1:
        gMinigameCameraState.scale += 20;
        if (gMinigameCameraState.scale > 0x200) {
            m4aSongNumStart(0x236);
            gMinigameCameraState.scale = 0x200;
            gMinigameSequenceState++;
        }
        break;
    case 2:
        if (MinigameWaitForFrames(60)) {
            gMinigameSequenceState = 0;
            return 1;
        }
        break;
    }
    return 0;
}
/* Keep the original two zero bytes as C data between functions; agbcc otherwise emits a Thumb NOP. */
__attribute__((section(".text"))) static const u16 sRouletteIntroAlignment = 0;

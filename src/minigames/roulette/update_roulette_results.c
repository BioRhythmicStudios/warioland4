#include "minigames/roulette.h"

int UpdateRouletteResults(void)
{
    u8 *statePtr;
    u8 *stateLoad;
    u8 *fadeStorePtr;
    u32 fadeStoreValue;
    u32 state;

    stateLoad = &gMinigameSequenceState;
    state = *stateLoad;
    statePtr = stateLoad;

    switch (state) {
    case 0:
        {
            u16 *timer;
            timer = &gMinigameSequenceTimer;
            (*timer)++;
            BobRouletteResultLabels();
            if (*timer > 60) {
                u8 *reload;
                u32 value;
                reload = &gMinigameSequenceState;
                value = *reload;
                value++;
                *reload = value;
                *timer = 0;
            }
        }
        goto end;

    case 1:
        gMinigameScore++;
        if (gMinigameScore > 999) {
            gMinigameScore = 999;
        } else {
            m4aSongNumStart(548);
        }
        if (gMinigameScore > gMinigameHighScores[2]) {
            gMinigameNewHighScore = 1;
            gMinigameHighScores[2] = gMinigameScore;
        }
        DrawRouletteScoreText();
        BobRouletteResultLabels();
        goto advanceState;

    case 2:
        {
            u16 *timer;
            timer = &gMinigameSequenceTimer;
            (*timer)++;
            BobRouletteResultLabels();
            if (*timer <= 20)
                goto end;
            {
                u16 zero;
                zero = 0;
                *timer = zero;
            }
        }
        goto advanceState;

    case 3:
        if ((u16)__umodsi3(gMinigameScore, 3) == 0) {
            gMedalCount++;
            if (gMedalCount > 999) {
                gMedalCount = 999;
            } else {
                m4aSongNumStart(547);
                gMinigameMedalHighlight = 1;
            }
        }
        DrawRouletteScoreText();
        {
            u8 *reload;
            u32 value;
            reload = &gMinigameSequenceState;
            value = *reload;
            value++;
            *reload = value;
        }
        BobRouletteResultLabels();
        goto end;

    case 4:
        {
            u16 *timer;
            timer = &gMinigameSequenceTimer;
            (*timer)++;
            BobRouletteResultLabels();
            if (*timer <= 20)
                goto end;
            *timer = 0;
        }
        goto advanceState;

    case 5:
        {
            u16 *timer;
            u32 one;
            u32 timerValue;
            u8 *fade;
            u32 fadeRaw;

            timer = &gMinigameSequenceTimer;
            timerValue = *timer;
            timerValue++;
            *timer = timerValue;
            one = 1;
            one &= timerValue;
            if (one != 0)
                goto end;

            fade = (u8 *)&gRouletteBlendEvb;
            fadeRaw = *fade;
            if ((s8)fadeRaw > 0) {
                {
                    register u8 *blendPtr PIN("r0");
                    u32 blendValue;
                    blendPtr = &gRouletteBlendEva;
                    blendValue = *blendPtr;
                    blendValue++;
                    *blendPtr = blendValue;
                }
                fadeStorePtr = fade;
                fadeStoreValue = fadeRaw - 1;
                goto storeFade;
            }

            ResetRouletteRound();
            *timer = one;
        }

advanceState:
        {
            u8 *reload;
            u32 value;
            reload = &gMinigameSequenceState;
            value = *reload;
            value++;
            *reload = value;
        }
        goto end;

    case 6:
        {
            u16 *timer;
            u32 one;
            u32 timerValue;
            register u8 *fade PIN("r2");
            u32 fadeRaw;

            timer = &gMinigameSequenceTimer;
            timerValue = *timer;
            timerValue++;
            *timer = timerValue;
            one = 1;
            one &= timerValue;
            if (one != 0)
                goto end;

            fade = (u8 *)&gRouletteBlendEvb;
            fadeRaw = *fade;
            if ((s8)fadeRaw > 15) {
                u32 clear;
                clear = 0;
                *timer = one;
                *statePtr = clear;
                return 1;
            }

            {
                register u8 *blendPtr PIN("r0");
                u32 blendValue;
                blendPtr = &gRouletteBlendEva;
                blendValue = *blendPtr;
                blendValue--;
                *blendPtr = blendValue;
            }
            fadeStorePtr = fade;
            fadeStoreValue = fadeRaw + 1;
        }

storeFade:
        *fadeStorePtr = fadeStoreValue;
        break;
    }

end:
    return 0;
}

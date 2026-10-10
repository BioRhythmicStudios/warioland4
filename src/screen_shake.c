#include "global_data.h"

u8 ScreenShakeRequestY(u8 duration, u8 amplitude)
{
    if (duration != 0 && duration > gScreenShakeY.duration) {
        gScreenShakeY.duration = duration;
        gScreenShakeY.frameTimer = 0;
        gScreenShakeY.amplitude = amplitude;
        gScreenShakeY.direction = 0;
    }
    return gScreenShakeY.duration;
}

u8 ScreenShakeRequestX(u8 duration)
{
    if (duration != 0 && duration > gScreenShakeX.duration) {
        gScreenShakeX.duration = duration;
        gScreenShakeX.frameTimer = 0;
        gScreenShakeX.amplitude = 0;
        gScreenShakeX.direction = 0;
    }
    return gScreenShakeX.duration;
}

u8 ScreenShakeRequestXWithAmplitude(u8 duration, u8 amplitude)
{
    if (duration != 0 && duration > gScreenShakeX.duration) {
        gScreenShakeX.duration = duration;
        gScreenShakeX.frameTimer = 0;
        gScreenShakeX.amplitude = amplitude;
        gScreenShakeX.direction = 0;
    }
    return gScreenShakeX.duration;
}

s32 ScreenShakeUpdateY(void)
{
    s32 currentDuration;
    s32 amplitude;
    s32 nextDuration;
    s32 shakeOffset;
    s32 duration;
    struct ScreenShakeParameters* params;
    struct ScreenShakeParameters* paramsAlt;

    shakeOffset = 0;
    params = &gScreenShakeY;
    currentDuration = params->duration;
    paramsAlt = params;
    duration = currentDuration;
    if (duration == 0) {
        return 0;
    }

    nextDuration = duration - DELTA_TIME;
    paramsAlt->duration = nextDuration;
    if (gScreenShakeY.frameTimer <= DELTA_TIME) {
        params->frameTimer = gScreenShakeY.frameTimer + DELTA_TIME;
        return 0;
    }

    gScreenShakeY.frameTimer = shakeOffset;
    amplitude = paramsAlt->amplitude & S8_MAX;
    shakeOffset = -2;
    if (paramsAlt->direction != 0) {
        shakeOffset = 2;
        shakeOffset &= ((-amplitude) | amplitude) >> 31;
    }

    gScreenShakeY.direction = paramsAlt->direction ^ 1;
    if ((u8)nextDuration > CONVERT_SECONDS(0.25) + DELTA_TIME) {
        return shakeOffset;
    }

    return shakeOffset / 2;
}

s32 ScreenShakeUpdateX(void)
{
    s32 currentDuration;
    s32 amplitude;
    s32 nextDuration;
    s32 shakeOffset;
    s32 duration;
    struct ScreenShakeParameters* params;
    struct ScreenShakeParameters* paramsAlt;

    shakeOffset = 0;
    params = &gScreenShakeX;
    currentDuration = params->duration;
    paramsAlt = params;
    duration = currentDuration;
    if (duration == 0) {
        return 0;
    }

    nextDuration = duration - DELTA_TIME;
    paramsAlt->duration = nextDuration;
    if (gScreenShakeX.frameTimer <= DELTA_TIME) {
        params->frameTimer = gScreenShakeX.frameTimer + DELTA_TIME;
        return 0;
    }

    gScreenShakeX.frameTimer = shakeOffset;
    amplitude = paramsAlt->amplitude & S8_MAX;
    shakeOffset = -2;
    if (paramsAlt->direction != 0) {
        shakeOffset = 2;
        shakeOffset &= ((-amplitude) | amplitude) >> 31;
    }

    gScreenShakeX.direction = paramsAlt->direction ^ 1;
    if ((u8)nextDuration > CONVERT_SECONDS(0.25) + DELTA_TIME) {
        return shakeOffset;
    }

    return shakeOffset / 2;
}

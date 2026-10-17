#include "global_data.h"

void SetBgTileColor(s32 x, s32 y, s32 color)
{
    s32 offset;
    u16* dst;
    s32 mode;

    color = (u16)color;
    mode = gVideoMode;

    if (mode == 4) {
        goto mode4;
    }
    if (mode > 4) {
        goto modeDefault;
    }
    if (mode != 3) {
        goto modeDefault;
    }

    offset = (y * 0x1E0);
    y = x * 2;
    offset += y;
    dst = (u16*)(offset + 0x06000000);
    *dst = color;
    return;

mode4:
    if (gVideoPage == 0) {
        offset = (y * 0xF0) + x;
        dst = (u16*)(offset + 0x06000000);
    } else {
        offset = (y * 0xF0) + x;
        dst = (u16*)(offset + 0x0600A000);
    }
    *dst = (*dst & 0xFF00) | color;
    return;

modeDefault:
    if (gVideoPage == 0) {
        offset = y * 0x140;
        y = x * 2;
        offset += y;
        dst = (u16*)(offset + 0x06000000);
    } else {
        offset = (y * 0x140) + x;
        dst = (u16*)(offset + 0x0600A000);
    }
    *dst = color;
}

void DrawBgLine(s32 x0, s32 y0, s32 x1, s32 y1, s32 color)
{
    // Used specific register binding to match
    volatile s32 lineColor;
    volatile s32 endMajorCoord;
    s32 r0;
    s32 r1;
    register s32 startMajorCoord PIN("r4");
    s32 deltaMajor;
    s32 currentStep;
    s32 startMinorCoord;
    register s32 maxStep PIN("r8");
    register s32 endMinorCoord PIN("r9");
    register s32 temp;
    register s32 startCoord PIN("sl");

    startMajorCoord = x0;
    currentStep = y0;
    r1 = y1;
    r0 = color;
    r0 = (u16)r0;
    lineColor = r0;

    deltaMajor = x1 - startMajorCoord;
    if (deltaMajor < 0) {
        deltaMajor = -deltaMajor;
    }

    r0 = r1 - currentStep;
    if (r0 < 0) {
        r0 = -r0;
    }

    if (deltaMajor >= r0) {
        if (startMajorCoord <= x1) {
            startCoord = startMajorCoord;
            endMajorCoord = x1;
            startMinorCoord = currentStep;
            endMinorCoord = r1;
        } else {
            startCoord = x1;
            endMajorCoord = startMajorCoord;
            startMinorCoord = r1;
            endMinorCoord = currentStep;
        }

        currentStep = 0;
        r0 = endMajorCoord;
        r1 = startCoord;
        startMajorCoord = r0 - r1;
        if (currentStep <= startMajorCoord) {
            do {
                temp = endMinorCoord;
                r0 = temp - startMinorCoord;
                r0 = r0 * currentStep;
                maxStep = startMajorCoord;
                r1 = startMajorCoord;
                deltaMajor = r0 / r1;
                r1 = startCoord;
                r0 = currentStep + r1;
                r1 = deltaMajor + startMinorCoord;
                temp = lineColor;
                SetBgTileColor(r0, r1, temp);
                currentStep += 1;
            } while (currentStep <= maxStep);
        }
    } else {
        if (currentStep <= r1) {
            startCoord = startMajorCoord;
            endMajorCoord = x1;
            startMinorCoord = currentStep;
            endMinorCoord = r1;
        } else {
            startCoord = x1;
            endMajorCoord = startMajorCoord;
            startMinorCoord = r1;
            endMinorCoord = currentStep;
        }

        deltaMajor = 0;
        temp = endMinorCoord;
        temp = temp - startMinorCoord;
        maxStep = temp;
        if (deltaMajor <= maxStep) {
            do {
                r1 = endMajorCoord;
                temp = startCoord;
                r0 = r1 - temp;
                r0 = r0 * deltaMajor;
                endMinorCoord = maxStep;
                r1 = maxStep;
                currentStep = r0 / r1;
                r1 = startCoord;
                r0 = currentStep + r1;
                r1 = deltaMajor + startMinorCoord;
                temp = lineColor;
                SetBgTileColor(r0, r1, temp);
                deltaMajor += 1;
            } while (deltaMajor <= endMinorCoord);
        }
    }
}

#include "transparency.h"

#ifdef PORTABLE
#include <string.h>
#endif

void TransparencyProcessTiles(s32 isMoving)
{
    u8 alphaTable[7];
    s32 alpha;
    s32 holdUpdate;
    s32 xTile;
    s32 temp;

    alpha = isMoving;

    memcpy(alphaTable, sUnk_83F7820, 7);
    holdUpdate = 0;

    if (alpha != 0) {
        if (gWarioData.xPosition > gPreviousXPosition) {
            alpha = gWarioData.xPosition - gPreviousXPosition;
        } else {
            alpha = gPreviousXPosition - gWarioData.xPosition;
        }
        ASM_BARRIER_VOLATILE(: : "r"(alpha));

        if (gUnk_30000D8.blendTimer < (temp = 1)) {
            gUnk_30000D8.blendTimer++;
            holdUpdate = 1;
        } else {
            gUnk_30000D8.blendTimer = 0;
        }
    }

    xTile = BLOCKS_FROM_SUBPIXELS(gWarioData.xPosition);
    temp = ((gWarioData.hitboxOffsetTop >> 1) + gWarioData.yPosition) >> 6;
    alpha = (temp * gBackgroundInfo.bg1Width) + xTile;
    {
        GBA_PTR(u8*)* bgTileLookupPointer = (GBA_TYPEOF(u8**, bgTileLookupPointer))&gBackgroundTileTables;
        u32 tile = GP(gBackgroundInfo.pBg0Data)[alpha];
        alpha = GP(bgTileLookupPointer[1])[tile];
        alpha -= 0x80;
    }

    if (alpha >= 0 && alpha <= 6) {
        alpha = alphaTable[alpha];
    } else if (alpha == 7) {
        alpha = gUnk_30000D8.defaultAlpha;
    } else {
        alpha = gUnk_30000D8.targetAlpha;
    }

    if (alpha != gUnk_30000D8.targetAlpha) {
        gUnk_30000D8.blendTimer = 0;
    } else if (holdUpdate != 0) {
        return;
    }

    gUnk_30000D8.targetAlpha = alpha;
    alpha = gUnk_30000D8.targetAlpha - gUnk_30000D8.currentAlpha;

    if (alpha != 0) {
        if (alpha > 0) {
            gUnk_30000D8.currentAlpha++;
        } else {
            gUnk_30000D8.currentAlpha--;
        }
        gBldAlpha = (gUnk_30000D8.currentAlpha << 8) | (16 - gUnk_30000D8.currentAlpha);
    }
}

void TransparencyProcessWater(void)
{
    u16 songId;
    s32 alphaDelta;
    s32 nextAlpha;
    register s32 newTarget PIN("r2");
    register struct TransparencyState* p PIN("r2") = &gUnk_30000D8;

    if (p->blendTimer == 0) {
        if (gUnk_3000038 != 0) {
            newTarget = 0;
            songId = SOUND_E2;
        } else {
            newTarget = p->defaultAlpha;
            songId = SOUND_E3;
        }
        if (gUnk_30000D8.targetAlpha != newTarget) {
            gUnk_30000D8.targetAlpha = newTarget;
            m4aSongNumStart(songId);
        }
    }
    gUnk_3000038 = 0;
    alphaDelta = gUnk_30000D8.targetAlpha - gUnk_30000D8.currentAlpha;
    gUnk_30000D8.blendTimer = alphaDelta;
    if (alphaDelta != 0) {
        if (alphaDelta > 0) {
            nextAlpha = gUnk_30000D8.currentAlpha + 1;
        } else {
            nextAlpha = gUnk_30000D8.currentAlpha - 1;
        }
        gUnk_30000D8.currentAlpha = nextAlpha;
        gBldAlpha = (gUnk_30000D8.currentAlpha << 8) | (0x10 - gUnk_30000D8.currentAlpha);
    }
}

#include "global_data.h"
#include "wario.h"
#include "port/game_thread.h"

#include <stdio.h>

// Replays the environmental BG0 lookup with the wrapped index from the
// Toy Block Tower fault. This is a focused memory-state test, not a full route.
static int sTileWrapCompleted;
extern void UpdateWarioEnvironmentalTiles(void);

static void TileWrapEntry(void)
{
    int xTile, yTile;
    u32 index;

    *(volatile u16 *)0x0201C7F0 = 0;
    *(volatile u16 *)0x02020000 = 0;
    GP_SET(gBackgroundInfo.pBg0Data, (u16 *)0x0201C840);
    GP_SET(gBackgroundInfo.pBg1Data, (u16 *)0x0201C840);
    GP_SET(gBackgroundInfo.pBg2Data, (u16 *)0x0201C840);
    gBackgroundInfo.bg0Width = 40;
    gBackgroundInfo.bg1Width = 40;
    gBackgroundInfo.bg2Width = 40;
    GP_SET(gBackgroundTileTables.attributes, (const u16 *)0x02020000);
    gCurrentRoomHeader.bg0Param = 16;
    gWarioData.xPosition = 0;
    gWarioData.yPosition = 0;
    gWarioData.hitboxOffsetLeft = 0;
    gWarioData.hitboxOffsetRight = 0;
    gWarioData.hitboxOffsetTop = -2;
    gWarioData.reaction = 0;
    gWarioData.pose = 0;
    xTile = (gWarioData.xPosition + (gWarioData.hitboxOffsetRight >> 1)) >> 6;
    yTile = (gWarioData.yPosition + (gWarioData.hitboxOffsetTop >> 1)) >> 6;
    index = yTile * gBackgroundInfo.bg0Width + xTile;
    printf("tile-wrap fixture: roomParam=%u bg0=%08X width=%u x=%d y=%d index=%08X expected GBA read=0x0201C7F0\n",
           gCurrentRoomHeader.bg0Param, gBackgroundInfo.pBg0Data.a, gBackgroundInfo.bg0Width,
           xTile, yTile, index);
    fflush(stdout);
    if (gCurrentRoomHeader.bg0Param != 16 || gBackgroundInfo.pBg0Data.a != 0x0201C840
        || gBackgroundInfo.bg0Width != 40 || xTile != 0 || yTile != -1 || index != 0xFFFFFFD8)
        return;
    UpdateWarioEnvironmentalTiles();
    sTileWrapCompleted = 1;
}

int PortTileWrapTest(void)
{
    if (PortGameThreadCreate(TileWrapEntry))
        return 1;
    while (PortGameThreadResume())
        ;
    printf("tile-wrap environmental lookup: %s\n", sTileWrapCompleted ? "PASS" : "FAIL");
    return !sTileWrapCompleted;
}

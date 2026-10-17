#include "bg_clip.h"
#include "game_screen_helpers.h"
#include "main.h"
#include "wario.h"

#define BLOCK_SOLID_FLAG 0x01000000
#define BLOCK_TILE_SIZE 64
#define BLOCK_TILE_MASK 63

struct AGB_ALIGN BlockCollisionContext {
    s32 tileType;
    s32 localX;
    s32 localY;
    s32 switchIndex;
    s32 switchCollisionType;
    s32 isSprite;
};

typedef s32 (*BlockCollisionHandler)(struct BlockCollisionContext* context);

extern GBA_FUNC_PTR(BlockCollisionHandler) const sBlockCollisionHandlers[];
#ifdef PORTABLE
s32 ProcessTileInteractionAtPosition(u32 y, u32 x, u32 tile);
#define ProcessTileInteractionAtPosition(y, x, tileType) \
    ProcessTileInteractionAtPosition((u16)(y), (u16)(x), (u16)(tileType))
#else
extern s32 ProcessTileInteractionAtPosition(u16 y, u16 x, u16 tileType);
#endif

s32 GetSwitchBlockCollision(struct BlockCollisionContext* context);

void UpdateRoomStartDebugInfo(void)
{
    const struct RoomStartData* roomStart;
    s32 entryIndex;
    u16 warioXTile;
    u16 warioYTile;
    u16 tileTypeIndex;
    u16 attribute;
    u8 leftX;
    u8 rightX;
    u8 topY;
    u8 bottomY;

    if ((gMainTimer & 7) != 0) {
        return;
    }

    gGameScreenDebugInfo.roomEntryTensDigit = U8_MAX;
    gGameScreenDebugInfo.roomEntryOnesDigitAndPalette = U8_MAX;
    warioXTile = gWarioData.xPosition >> 6;
    warioYTile = gWarioData.yPosition >> 6;
    tileTypeIndex = warioYTile * gBackgroundInfo.bg1Width + warioXTile;
    attribute = GP(gBackgroundInfo.pBg1Data)[tileTypeIndex];
    attribute = GP(gBackgroundTileTables.attributes)[attribute];
    gGameScreenDebugInfo.tileAttribute = attribute;
    roomStart = GP(sRoomStartDataTables[gStageRoomTableIndex]);
    entryIndex = 0;

    while (roomStart->type != 0) {
        leftX = roomStart->x;
        rightX = roomStart->unk03;
        topY = roomStart->unk04;
        bottomY = roomStart->y;
        if (gCurrentRoom == roomStart->room && leftX <= warioXTile && warioXTile <= rightX && topY <= warioYTile &&
            warioYTile <= bottomY) {
            if ((u16)(attribute - 2) <= 5) {
                gGameScreenDebugInfo.roomEntryOnesDigitAndPalette = 0x80;
            } else {
                gGameScreenDebugInfo.roomEntryOnesDigitAndPalette = 0;
            }
            gGameScreenDebugInfo.roomEntryTensDigit = entryIndex / 10;
            gGameScreenDebugInfo.roomEntryOnesDigitAndPalette |= entryIndex % 10;
        }
        roomStart++;
        entryIndex++;
    }
}

s32 GetWarioBlockCollisionAtPosition(u32 yPosition, u32 xPosition)
{
    s32 layerCollision[3];
    struct BlockCollisionContext context;
    s32 result;
    u32 tileTypeIndex;
    u16 tileType;
    u16 y;
    u16 x;
    const struct BackgroundTileTables* tables;

    y = yPosition;
    x = xPosition;
    if (x >= gBackgroundInfo.bg1Width * BLOCK_TILE_SIZE) {
        return BLOCK_SOLID_FLAG | 1;
    }
    if (y >= gBackgroundInfo.bg1Height * BLOCK_TILE_SIZE) {
        return 0;
    }

    context.localY = y >> 6;
    context.localX = x >> 6;
    tileTypeIndex = context.localY * gBackgroundInfo.bg1Width + context.localX;
    tables = &gBackgroundTileTables;
    tileType = GP(gBackgroundInfo.pBg1Data)[tileTypeIndex];
    layerCollision[1] = GP(tables->bottom)[tileType];
    tileType = GP(gBackgroundInfo.pBg2Data)[tileTypeIndex];
    layerCollision[2] = GP(tables->bottom)[tileType];
    context.tileType = layerCollision[1];
    context.localY = y & BLOCK_TILE_MASK;
    context.localX = x & BLOCK_TILE_MASK;
    context.isSprite = 0;
    result = 0;

    if (context.tileType <= 12) {
        result = GF(sBlockCollisionHandlers[context.tileType])(&context);
    } else if ((u32)(context.tileType - 20) <= 43) {
        result = GetSwitchBlockCollision(&context);
    } else {
        context.tileType = result;
    }

    if (context.tileType == 0) {
        context.tileType = layerCollision[2];
        if (context.tileType == 12 && gCurrentRoomHeader.bg2Param == 16) {
            result = GF(sBlockCollisionHandlers[12])(&context);
        } else if ((u32)(context.tileType - 38) <= 7 && gCurrentRoomHeader.bg2Param == 16) {
            result = GetSwitchBlockCollision(&context);
        }
    }
    return result;
}

s32 GetSpriteBlockCollisionAtPosition(u16 y, u16 x)
{
    u16 layerCollision[3];
    struct BlockCollisionContext context;
    s32 layer;
    u8* backgroundInfo;
    u8* layerInfo;

    if (x >= gBackgroundInfo.bg1Width * BLOCK_TILE_SIZE || y >= gBackgroundInfo.bg1Height * BLOCK_TILE_SIZE) {
        gBackgroundCollisionData.primaryType = 0;
        gBackgroundCollisionData.waterType = 0;
        return 0;
    }

    GetBackgroundCollisionAtPosition(y, x);
    context.localY = y >> 6;
    context.localX = x >> 6;

    for (layer = 1; layer <= 2; layer++) {
        backgroundInfo = (u8*)&gBackgroundInfo;
        layerInfo = backgroundInfo + layer * 8;
        layerCollision[layer] = context.localY * *(u16*)(layerInfo + 4) + context.localX;
        layerCollision[layer] = (GP(*(GBA_PTR(u16*) *)layerInfo))[layerCollision[layer]];
        if (*(u8*)&gSpriteTileInteractionMode != 0 && layer == 1) {
            if (ProcessTileInteractionAtPosition(context.localY, context.localX, layerCollision[1]) != 0) {
                if (*(u8*)&gSpriteTileInteractionMode == 3) {
                    layerCollision[1] = layer;
                } else {
                    layerCollision[1] = 0;
                }
            }
            gSpriteTileInteractionMode = 0;
        }
        layerCollision[layer] = GP(gBackgroundTileTables.bottom)[layerCollision[layer]];
    }

    context.tileType = layerCollision[1];
    context.localY = y & BLOCK_TILE_MASK;
    context.localX = x & BLOCK_TILE_MASK;
    context.isSprite = 1;
    layer = 0;

    if (context.tileType <= 12) {
        layer = GF(sBlockCollisionHandlers[context.tileType])(&context);
    } else if ((u32)(context.tileType - 20) <= 43) {
        layer = GetSwitchBlockCollision(&context);
    } else {
        context.tileType = layer;
    }

    if (context.tileType == 0) {
        context.tileType = layerCollision[2];
        if (context.tileType == 12 && gCurrentRoomHeader.bg2Param == 16) {
            layer = GF(sBlockCollisionHandlers[12])(&context);
        } else if ((u32)(context.tileType - 38) <= 7 && gCurrentRoomHeader.bg2Param == 16) {
            layer = GetSwitchBlockCollision(&context);
        }
    }
    return layer;
}

s32 GetNonSolidBlockCollision(struct BlockCollisionContext* context)
{
    return context->tileType;
}

s32 GetSolidBlockCollision(struct BlockCollisionContext* context)
{
    return context->tileType | BLOCK_SOLID_FLAG;
}

s32 GetDownRightSlopeCollision(struct BlockCollisionContext* context)
{
    s32 result;

    if (context->localY >= context->localX) {
        result = context->tileType | BLOCK_SOLID_FLAG;
    } else {
        result = context->tileType;
    }
    return result;
}

s32 GetDownLeftSlopeCollision(struct BlockCollisionContext* context)
{
    s32 result;

    if (context->localY >= BLOCK_TILE_MASK - context->localX) {
        result = context->tileType | BLOCK_SOLID_FLAG;
    } else {
        result = context->tileType;
    }
    return result;
}

s32 GetUpperShallowDownRightSlopeCollision(struct BlockCollisionContext* context)
{
    s32 result;

    if (context->localY >= (context->localX >> 1)) {
        result = context->tileType | BLOCK_SOLID_FLAG;
    } else {
        result = context->tileType;
    }
    return result;
}

s32 GetLowerShallowDownRightSlopeCollision(struct BlockCollisionContext* context)
{
    s32 result;

    if (context->localY >= ((context->localX + BLOCK_TILE_MASK) >> 1)) {
        result = context->tileType | BLOCK_SOLID_FLAG;
    } else {
        result = context->tileType;
    }
    return result;
}

s32 GetLowerShallowDownLeftSlopeCollision(struct BlockCollisionContext* context)
{
    s32 result;

    if (context->localY >= BLOCK_TILE_MASK - (context->localX >> 1)) {
        result = context->tileType | BLOCK_SOLID_FLAG;
    } else {
        result = context->tileType;
    }
    return result;
}

s32 GetUpperShallowDownLeftSlopeCollision(struct BlockCollisionContext* context)
{
    s32 result;

    if (context->localY >= ((BLOCK_TILE_MASK - context->localX) >> 1)) {
        result = context->tileType | BLOCK_SOLID_FLAG;
    } else {
        result = context->tileType;
    }
    return result;
}

s32 GetDamagedWarioOnlyBlockCollision(struct BlockCollisionContext* context)
{
    s32 result;

    result = 0;
    if (context->isSprite == 0 && gWarioData.damageTimer != 0) {
        context->tileType = 1;
        result = GetSolidBlockCollision(context);
    } else {
        context->tileType = result;
        result = GetNonSolidBlockCollision(context);
    }
    return result;
}

s32 GetSpriteOnlySolidBlockCollision(struct BlockCollisionContext* context)
{
    s32 result;

    result = 0;
    if (context->isSprite == 0) {
        context->tileType = result;
        result = GetNonSolidBlockCollision(context);
    } else {
        context->tileType = 1;
        result = GetSolidBlockCollision(context);
    }
    return result;
}

s32 GetDamagedWarioOrSpriteBlockCollision(struct BlockCollisionContext* context)
{
    s32 result;

    result = 0;
    if (context->isSprite == 0 && gWarioData.damageTimer == 0) {
        context->tileType = result;
        result = GetNonSolidBlockCollision(context);
    } else {
        context->tileType = 1;
        result = GetSolidBlockCollision(context);
    }
    return result;
}

s32 GetEmptyBlockCollision(struct BlockCollisionContext* context)
{
    context->tileType = 0;
    return GetNonSolidBlockCollision(context);
}

s32 GetSwitchBlockCollision(struct BlockCollisionContext* context)
{
    s32 inverse;
    s32 result;

    inverse = 0;
    switch (context->tileType) {
        case 20 ... 23:
            context->switchIndex = context->tileType - 19;
            context->switchCollisionType = 1;
            break;
        case 24 ... 27:
            context->switchIndex = context->tileType - 23;
            context->switchCollisionType = 1;
            inverse = 1;
            break;
        case 29 ... 32:
            context->switchIndex = context->tileType - 28;
            context->switchCollisionType = 2;
            break;
        case 33 ... 36:
            context->switchIndex = context->tileType - 32;
            context->switchCollisionType = 2;
            inverse = 1;
            break;
        case 38 ... 41:
            context->switchIndex = context->tileType - 37;
            context->switchCollisionType = 3;
            break;
        case 42 ... 45:
            context->switchIndex = context->tileType - 41;
            context->switchCollisionType = 3;
            inverse = 1;
            break;
        case 47 ... 50:
            context->switchIndex = context->tileType - 46;
            context->switchCollisionType = 4;
            break;
        case 51 ... 54:
            context->switchIndex = context->tileType - 50;
            context->switchCollisionType = 4;
            inverse = 1;
            break;
        case 56 ... 59:
            context->switchIndex = context->tileType - 55;
            context->switchCollisionType = 5;
            break;
        case 60 ... 63:
            context->switchIndex = context->tileType - 59;
            context->switchCollisionType = 5;
            inverse = 1;
            break;
        default:
            context->switchIndex = 0;
            context->switchCollisionType = 0;
            break;
    }

    result = 0;
    switch (context->switchCollisionType) {
        case 0:
            goto emptyBlock;
        case 1:
            if ((gSwitchStates[context->switchIndex] & 1) != inverse) {
                goto caseOneSolidBlock;
            }
            goto emptyBlock;
        case 2:
            if ((gSwitchStates[context->switchIndex] & 1) == inverse) {
                goto emptyBlock;
            }
            result = GetDamagedWarioOnlyBlockCollision(context);
            goto done;
        case 3:
            if ((gSwitchStates[context->switchIndex] & 1) == inverse) {
                goto emptyBlock;
            }
            context->tileType = 12;
            goto returnTile;

        emptyBlock:
            context->tileType = 0;
        returnTile:
            result = GetNonSolidBlockCollision(context);
            goto done;

        case 4:
            if ((gSwitchStates[context->switchIndex] & 1) == inverse) {
                context->tileType = 1;
                goto callSolidBlock;
            }
            context->tileType = 2;
            result = GetDownRightSlopeCollision(context);
            goto done;

        caseOneSolidBlock:
            context->tileType = 1;
        callSolidBlock:
            result = GetSolidBlockCollision(context);
            goto done;

        case 5:
            if ((gSwitchStates[context->switchIndex] & 1) == inverse) {
                context->tileType = 1;
                goto callCaseFiveSolidBlock;
            }
            context->tileType = 3;
            result = GetDownLeftSlopeCollision(context);
            goto done;

        callCaseFiveSolidBlock:
            result = GetSolidBlockCollision(context);
            break;
    }

done:
    return result;
}

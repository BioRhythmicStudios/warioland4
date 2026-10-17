#ifndef TILE_INTERACTION_H
#define TILE_INTERACTION_H

#include "tile_collision.h"
#include "tile_effect.h"
#include "types.h"

struct AGB_ALIGN TileInteractionContext {
    u16 xTile;
    u16 yTile;
    u16 attribute;
    u8 direction;
    u8 debrisType;
    u8 collisionMode;
    u8 debrisOffset;
    u8 soundIndex;
    u8 padding;
};

typedef void (*TileInteractionHandler)(struct TileInteractionContext* context);
typedef void (*TileCoordinateHandler)(u32 y, u32 x);

extern TileInteractionHandler gTileInteractionHandlers[];
extern TileCoordinateHandler gTileCoordinateHandlers[];

s32 ProcessTileInteractionAtPosition(u32 y, u32 x, u32 tile);
void UpdateTileEffect(void);
void RefreshCollectedTileEffects(void);
void UpdateWarioTileInteractions(void);

void BreakSingleTile(struct TileInteractionContext* context);
void BreakAdjacentTiles(struct TileInteractionContext* context);
void BreakTrackedAdjacentTiles(struct TileInteractionContext* context);
void ProcessTileAtPosition(u32 y, u32 x);
void ProcessAlternateTileAtPosition(u32 y, u32 x);

#endif

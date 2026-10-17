#include "bg_clip.h"

#include "color_effects.h"
#include "game_screen_helpers.h"
#include "main.h"

struct AGB_ALIGN BgClipStack {
    u32 bg0Attribute;
    u32 attributes[2];
    s32 yPosition;
    s32 xPosition;
};

s32 GetBackgroundCollisionAtPosition(u16 y, u16 x)
{
    register struct BackgroundCollisionData* collision;
    register struct BackgroundCollisionData* collisionInit PIN("r1");
    struct BackgroundInfo* background;
    struct BackgroundInfo* backgroundInit;
    register struct RoomHeader* roomHeader PIN("sl");
    register struct RoomHeader* roomHeaderInit PIN("r3");
    u32 tileY;
    register u32 tileX PIN("ip");
    u32 attribute;
    u32 shiftedY;
    register u32 shiftedX PIN("r4");
    register s32 xTruncated PIN("r1");
    register s32 collisionType PIN("r2");
    s32 overrideType;
    s32 one;
    register s32 limit PIN("r0");
    u32 parameter;
    struct BgClipStack stack;
    const u16* attributeTable;
    struct BackgroundInfo* layer;
    u32* destination;
    s32 layerIndex;
    u32 value;
    s32 waterCollision;

    shiftedY = y << 16;
    stack.yPosition = shiftedY >> 16;
    shiftedX = x << 16;
    xTruncated = (u32)shiftedX >> 16;
    stack.xPosition = xTruncated;
    collisionInit = &gBackgroundCollisionData;
    collisionInit->primaryType = 0;
    collisionInit->waterType = 0;
    backgroundInit = &gBackgroundInfo;
    limit = backgroundInit->bg1Height;
    limit <<= 6;
    collision = collisionInit;
    background = backgroundInit;

    if (stack.yPosition >= limit || stack.xPosition >= (backgroundInit->bg1Width << 6)) {
        return 0;
    }

    tileY = shiftedY >> 22;
    tileX = shiftedX >> 22;
    roomHeaderInit = &gCurrentRoomHeader;
    ASM_BARRIER(: "=r"(roomHeader) : "0"(roomHeaderInit));
    attributeTable = GP(gBackgroundTileTables.attributes);
    destination = stack.attributes;
    layer = (struct BackgroundInfo*)&background->pBg1Data;
    layerIndex = 1;
    do {
        register u32 temporary PIN("r0");
        u32 product;
        register u32 tileIndex PIN("r2");
        const u16* tileMap;

        temporary = layer->bg0Width;
        product = tileY;
        product *= temporary;
        temporary = product;
        product = tileX;
        tileIndex = temporary + product;
        tileMap = GP(layer->pBg0Data);
        temporary = tileIndex << 1;
        temporary += (u32)tileMap;
        tileIndex = *(u16*)temporary;
        temporary = tileIndex << 1;
        temporary += (u32)attributeTable;
        temporary = *(u16*)temporary;
        *destination++ = temporary;
        layer = (struct BackgroundInfo*)((u8*)layer + 8);
        layerIndex--;
    } while (layerIndex >= 0);

    collisionType = 0;
    attribute = stack.attributes[0];
    if (attribute == 106) {
        collisionType = 5;
    } else if (attribute == 107) {
        collisionType = 6;
    } else if (attribute == 104) {
        collisionType = 11;
    } else if (attribute == 4) {
        collisionType = 7;
    } else if (attribute == 5) {
        collisionType = 8;
    } else if (attribute == 6) {
        collisionType = 9;
    } else if (attribute == 7) {
        collisionType = 10;
    } else if (attribute == 121) {
        collisionType = 2;
    } else if (attribute == 123) {
        collisionType = 4;
    } else if (attribute == 2) {
        collisionType = 1;
    } else if (attribute == 122) {
        collisionType = 3;
    } else if ((u32)(attribute - 89) <= 4) {
        collisionType = 11;
    }

    if (collisionType == 0) {
        register struct RoomHeader* headerTemporary PIN("r3");

        headerTemporary = roomHeader;
        if (headerTemporary->bg2Param != 16) {
            goto primary_collision_done;
        }
        value = stack.attributes[1];
        if (value == 2) {
            collisionType = 1;
        } else if (value == 121) {
            collisionType = 2;
        } else if (value == 123) {
            collisionType = 4;
        } else if (value == 104) {
            collisionType = 11;
        } else if (value == 122) {
            collisionType = 3;
        } else if ((u32)(value - 56) <= 7) {
            overrideType = value - 55;
            if (overrideType <= 3) {
                u32 switchState;
                u8* switchPointer;

                switchPointer = (u8*)overrideType;
                switchPointer += (u32)gSwitchStates;
                switchState = *switchPointer;
                if (switchState == SWITCH_STATE_OFF || switchState == SWITCH_STATE_SWITCHING_OFF) {
                    collisionType = 1;
                }
            } else if ((u8)(gSwitchStates[value - 59] - 1) <= 1) {
                collisionType = 1;
            }
        } else {
            u32 rangeValue;

            rangeValue = value;
            rangeValue -= 152;
            if (rangeValue <= 3) {
                register u32 switchState PIN("r1");
                u32 maskedState;

                overrideType = rangeValue;
                switchState = gSwitchStates[1];
                one = 1;
                maskedState = one;
                maskedState &= switchState;
                if (maskedState == 0) {
                    if (overrideType <= 1) {
                        collisionType = value - 150;
                    }
                } else if (overrideType > 1) {
                    overrideType &= one;
                    collisionType = overrideType + 2;
                }
            }
        }
    }

primary_collision_done:
    collision->primaryType = collisionType;
    waterCollision = 0;
    overrideType = 0;
    {
        struct RoomHeader* headerTemporary;

        headerTemporary = roomHeader;
        parameter = headerTemporary->bgPriorityAlpha;
        if (parameter <= 7) {
            goto check_primary_water_attribute;
        }
        parameter = headerTemporary->bg0Param;
        if (parameter <= 16) {
            goto process_bg0_collision;
        }
    }

check_primary_water_attribute: {
    u32 rangeValue;

    rangeValue = attribute;
    rangeValue -= 76;
    if (rangeValue > 12) {
        goto secondary_collision_done;
    }
    waterCollision = 1;
    goto secondary_collision_done;
}

process_bg0_collision:
    if (parameter == 0) {
        goto secondary_collision_done;
    }
    {
        register struct BackgroundInfo* backgroundTemporary PIN("r1");
        s32 bound;
        register s32 coordinate PIN("r3");
        u32 width;

        backgroundTemporary = background;
        bound = backgroundTemporary->bg0Height;
        bound <<= 6;
        coordinate = stack.yPosition;
        if (coordinate >= bound) {
            goto bg0_out_of_bounds;
        }
        width = backgroundTemporary->bg0Width;
        bound = width;
        bound <<= 6;
        coordinate = stack.xPosition;
        if (coordinate < bound) {
            goto bg0_in_bounds;
        }

    bg0_out_of_bounds:
        return collision->primaryType << 16;

    bg0_in_bounds: {
        register u32 tileIndex PIN("r0");
        struct BackgroundInfo* backgroundReload;
        const u16* tileMap;
        struct BackgroundTileTables* tables;
        const u16* attributesReload;
        u32 bgAttribute;

        tileIndex = tileY;
        tileIndex *= width;
        tileIndex += tileX;
        backgroundReload = background;
        tileMap = GP(backgroundReload->pBg0Data);
        tileIndex <<= 1;
        tileIndex += (u32)tileMap;
        tileIndex = *(u16*)tileIndex;
        tables = &gBackgroundTileTables;
        attributesReload = GP(tables->attributes);
        tileIndex <<= 1;
        tileIndex += (u32)attributesReload;
        bgAttribute = *(u16*)tileIndex;
        stack.bg0Attribute = bgAttribute;
        parameter = bgAttribute;
        parameter -= 76;
        if (parameter > 12) {
            goto secondary_collision_done;
        }
        waterCollision = 1;
        if (bgAttribute == 81) {
            overrideType = 12;
        } else if (bgAttribute == 82) {
            overrideType = 13;
        } else if (bgAttribute == 83) {
            overrideType = 14;
        } else if (bgAttribute == 84) {
            overrideType = 15;
        } else if (bgAttribute == 85) {
            overrideType = 16;
        } else if (bgAttribute == 86) {
            overrideType = 17;
        } else if (bgAttribute == 87) {
            overrideType = 18;
        } else if (bgAttribute == 88) {
            overrideType = 19;
        }
    }

        {
            u32 switchState;
            u32 maskedState;

            switchState = gSwitchStates[4];
            maskedState = 1;
            maskedState &= switchState;
            if (maskedState == 0) {
                parameter = stack.bg0Attribute;
                if (parameter == 76) {
                    overrideType = 12;
                } else if (parameter == 77) {
                    overrideType = 17;
                } else if (parameter == 78) {
                    overrideType = 18;
                } else if (parameter == 79) {
                    overrideType = 19;
                }
            }
        }
    }

secondary_collision_done:

    if (waterCollision == 1) {
        if (overrideType != 0 || collision->primaryType != 1) {
            collision->primaryType = overrideType;
        }
    }
    collision->waterType = waterCollision;
    return (collision->primaryType << 16) | collision->waterType;
}

s32 GetBg2CollisionTypeAtTile(s32 y, s32 x)
{
    register u32 yCoord PIN("r5");
    u32 xCoord;
    u16 xSaved;
    struct BackgroundCollisionData* collision;
    struct BackgroundCollisionData* collisionTemp;
    register struct BackgroundInfo* background PIN("r4");
    s32 result;
    s32 zero;
    u32 indexTemporary;
    register const u16* tileMap PIN("r1");
    u16 tile;

    y = (u16)y;
    ASM_BARRIER(: "+r"(y));
    yCoord = y;
    xCoord = (u16)x;
    xSaved = xCoord;
    collisionTemp = &gBackgroundCollisionData;
    zero = 0;
    collisionTemp->bg2Type = zero;
    result = 0;
    background = &gBackgroundInfo;
    collision = collisionTemp;

    if (yCoord >= background->bg1Height || xCoord >= background->bg1Width) {
        return 0;
    }

    indexTemporary = background->bg2Width;
    indexTemporary *= yCoord;
    xCoord = indexTemporary + xSaved;
    tileMap = GP(background->pBg2Data);
    indexTemporary = xCoord << 1;
    indexTemporary += (u32)tileMap;
    tile = *(u16*)indexTemporary;
    tile = GP(gBackgroundTileTables.attributes)[tile];

    if ((u32)(tile - 96) <= 7) {
        result = 1;
        if (tile != 96) {
            result = 2;
            if (tile != 97) {
                result = 3;
                if (tile != 98) {
                    result = 4;
                    if (tile != 99) {
                        result = 5;
                        if (tile != 100) {
                            result = 6;
                            if (tile != 101) {
                                result = 8;
                                if (tile == 102) {
                                    result = 7;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if ((u32)(tile - 132) <= 10) {
        result = tile - 123;
    }

    collision->bg2Type = result;
    return result;
}

s32 TryTriggerRoomTransitionAtTile(s32 y, s32 x)
{
    s32 temporary;
    u32 yCoord;
    u32 xCoord;
    s32 result;
    const u8* entry;
    s16* modeState;

    temporary = y;
    ASM_BARRIER(: "+r"(temporary));
    temporary <<= 16;
    yCoord = (u32)temporary >> 16;
    xCoord = (u16)x;
    result = 0;
    {
        register s16* modePointer PIN("r1");
        u16 offset;

        modePointer = &gSubGameMode;
        offset = 0;
        temporary = *(s16*)((u8*)modePointer + offset);
        modeState = modePointer;
    }

    if (temporary != 2) {
        return y;
    }

    entry = (const u8*)GP(sRoomStartDataTables[gStageRoomTableIndex]);
    while (entry[0] != 0) {
        if (entry[0] == 2 && entry[1] == gCurrentRoom && entry[2] <= xCoord && xCoord <= entry[3] &&
            entry[4] <= yCoord && yCoord <= entry[5]) {
            gUnk_3000025 = entry[6];
            *modeState = 3;
            SetRoomTransitionTileValues(0);
            ApplyRoomTransitionTileOffset(0);
            StartRoomTransitionFade(2);
            if (gUnk_3000025 == 0) {
                gStageExitType = 6;
            }
            result = 1;
            break;
        }
        entry += 12;
    }

    return result;
}

void CheckRoomTransitionAtPosition(u16 y, u16 x)
{
    const u8* entry;
    u16 tileY;
    u16 tileX;

    if (gSubGameMode != 2) {
        return;
    }

    tileY = y >> 6;
    tileX = x >> 6;
    entry = (const u8*)GP(sRoomStartDataTables[gStageRoomTableIndex]);
    while (entry[0] != 0) {
        s32 fadeMusic;

        fadeMusic = 0;
        if (entry[0] != 1 && entry[0] != 3 && entry[0] != 4) {
            if (entry[0] != 5) {
                entry += 12;
                continue;
            }
            fadeMusic = 1;
        }

        if (entry[1] == gCurrentRoom && entry[2] <= tileX && tileX <= entry[3] && entry[4] <= tileY &&
            tileY <= entry[5]) {
            gUnk_3000025 = entry[6];
            gSubGameMode = 3;
            SetRoomTransitionTileValues(0);
            ApplyRoomTransitionTileOffset(0);
            StartRoomTransitionFade(entry[0]);
            if (fadeMusic != 0) {
                FadeOutMusicForPause();
                gUnk_3000C37 = 1;
            }
            return;
        }

        entry += 12;
    }
}

void ApplyRoomTransitionTileOffset(u8 value)
{
    struct BackgroundInfo* background;
    struct BackgroundInfo* widthBackground;
    u16* loadedMap;
    s32 firstIndex;
    register s32 secondIndex PIN("r4");
    register s32 temporary PIN("r0");
    u16* data;
    u16* map;
    u16 tile;
    s32 count;

    if (gRoomTransitionTileDataCount == 0) {
        return;
    }

    count = gRoomTransitionTileDataCount;
    if (count != 0) {
        background = &gBackgroundInfo;
        loadedMap = GP(background->pBg1Data);
        map = loadedMap;
        data = (u16*)0x02039F80 + count;
        do {
            s32 i;

            data--;
            tile = *data;
            data--;
            count -= 2;
            firstIndex = *data;
            widthBackground = background;
            temporary = widthBackground->bg1Width;
            temporary = firstIndex + temporary;
            secondIndex = (u16)temporary;
            tile += value;
            i = 1;
            do {
                map[firstIndex] = tile;
                map[secondIndex] = tile + 4;
                temporary = tile + 2;
                tile = (s16)temporary;
                temporary = firstIndex + 1;
                firstIndex = (u16)temporary;
                temporary = secondIndex + 1;
                secondIndex = (u16)temporary;
                i--;
            } while (i >= 0);
        } while (count != 0);
    }

    if (value == 0) {
        gRoomTransitionTileDataCount = 0;
    }
}

void SetRoomTransitionTileValues(s32 value)
{
    u16 tileValue;
    const u16* data;
    struct BackgroundInfo* background;
    s32 count;
    u32 mask;
    u32 descriptor;
    u32 packed;
    u32 index;
    u32 width;
    u16* map;

    tileValue = (u16)value;
    data = (const u16*)0x02039F00;
    background = &gBackgroundInfo;
    count = 63;
    mask = 0xFF;
    do {
        descriptor = *data;
        packed = descriptor << 16;
        if (packed != 0) {
            width = background->bg1Width;
            index = mask;
            index &= descriptor;
            index *= width;
            width = packed >> 24;
            index += width;
            map = GP(background->pBg1Data);
            map[index] = tileValue;
        }
        data++;
        count--;
    } while (count >= 0);
}

void StartRoomTransitionFade(u8 type)
{
    if (type == 1 || type == 5) {
        gWindow.content = 0;
        gColorFading.type = 5;
    } else if (type == 4) {
        gColorFading.progress = 0;
        gColorFading.type = 2;
        BackupPalettesForColorFade();
        gSubGameMode = 6;
        gSpriteAiDropTimer = 0;
        gStageExitType = 0x80;
    } else {
        gColorFading.progress = 0;
        gColorFading.type = 2;
        BackupPalettesForColorFade();
    }
}

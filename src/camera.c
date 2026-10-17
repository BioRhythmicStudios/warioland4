#include "camera.h"
#include "background_registers.h"
#include "global_data.h"
#include "wario.h"

struct AGB_ALIGN CameraTargetPosition {
    u16 x;
    u16 y;
};

extern GBA_PTR(GBA_PTR(const u8*) const*) const sRoomCameraControlDataTable[];

void MoveCameraTowardTarget(s32 x, s32 y);
s32 GetCameraXForControlArea(const struct CameraControlArea* area, const struct CameraTargetPosition* position);
s32 GetCameraYForControlArea(const struct CameraControlArea* area, const struct CameraTargetPosition* position);
void BuildCurrentRoomCameraControlAreas(void);
void BuildBottomRoomCameraControlAreas(void);
void UpdateStandardCamera(const struct CameraTargetPosition* position);
void UpdateBg0CameraPosition(const struct CameraTargetPosition* unusedPosition);
void UpdateBg0ScrollEffect(void);
void SetCameraPositionClampedToRoom(const struct CameraTargetPosition* position);

void UpdateCameraFromControlAreas(const struct CameraTargetPosition* position)
{
    struct CameraControlArea* area;
    s32 x;
    s32 y;

    BuildCurrentRoomCameraControlAreas();
    x = gCameraPositionState.x;
    y = gCameraPositionState.y;
    area = gCameraControlAreas;

    if (area->mode != 0) {
        x = GetCameraXForControlArea(area, position);
        y = GetCameraYForControlArea(area, position);
    }

    area++;
    if (area->mode != 0) {
        x = (x + GetCameraXForControlArea(area, position)) >> 1;
        y = (y + GetCameraYForControlArea(area, position)) >> 1;
    }

    MoveCameraTowardTarget((u16)x, (u16)y);
}

void MoveCameraTowardTarget(s32 x, s32 y)
{
    u32 targetX;
    u16* positionY;
    u16* positionX;
    register u16* pointer PIN("r0");
    s32 targetY;
    s32 delta;
    s32 movement;

    targetX = (u16)x;
    targetY = (u16)y;
    gCameraPositionState.x = targetX;
    gCameraPositionState.y = targetY;

    if (gSubGameMode != 0) {
        pointer = &gBg1YPosition;
        /* Preserve the original pointer register lifetime across this smoothing path. */
        ASM_BARRIER_VOLATILE(: "+r"(pointer));
        positionY = pointer;
        if (targetY != *positionY) {
            movement = gWarioData.yPosition - gPreviousYPosition;
            delta = targetY - *(volatile u16*)positionY;
            if (delta > 0) {
                if (delta > 48) {
                    delta = 48;
                }
                if (movement >= 0) {
                    movement += 4;
                    if (delta < movement) {
                        movement = delta;
                    }
                } else {
                    movement = 0;
                }
            } else {
                if (delta < -48) {
                    delta = -48;
                }
                if (movement <= 0) {
                    movement -= 4;
                    if (delta > movement) {
                        movement = delta;
                    }
                } else {
                    movement = 0;
                }
            }
            gCameraPositionState.yOffset = movement;
            *positionY += movement;
        }

        pointer = &gBg1XPosition;
        /* Preserve the original pointer register lifetime across this smoothing path. */
        ASM_BARRIER_VOLATILE(: "+r"(pointer));
        positionX = pointer;
        if (targetX != *positionX) {
            movement = gWarioData.xPosition - gPreviousXPosition;
            delta = targetX - *(volatile u16*)positionX;
            if (delta > 0) {
                if (delta > 48) {
                    delta = 48;
                }
                if (movement >= 0) {
                    movement += 4;
                    if (delta < movement) {
                        movement = delta;
                    }
                } else {
                    movement = 0;
                }
            } else {
                if (delta < -48) {
                    delta = -48;
                }
                if (movement <= 0) {
                    movement -= 4;
                    if (delta > movement) {
                        movement = delta;
                    }
                } else {
                    movement = 0;
                }
            }
            gCameraPositionState.xOffset = movement;
            *positionX += movement;
        }
    }
}

s32 GetCameraXForControlArea(const struct CameraControlArea* area, const struct CameraTargetPosition* position)
{
    s32 x;
    s32 result;

    x = position->x;
    if (x < area->minX + 480) {
        result = area->minX;
    } else if (x > area->maxX - 480) {
        result = area->maxX - 960;
    } else {
        result = x - 480;
    }
    return result;
}

s32 GetCameraYForControlArea(const struct CameraControlArea* area, const struct CameraTargetPosition* position)
{
    s32 y;
    s32 result;

    if (area->mode == 2) {
        y = position->y;
        if (y < area->minY + 448) {
            return area->minY;
        }
        if (y > area->maxY - 192) {
            result = area->maxY - 640;
            if (result >= area->minY) {
                return result;
            }
            return area->minY;
        }
        return y - 448;
    }
    return area->maxY - 640;
}

void LoadCurrentRoomCameraControlData(void)
{
    GBA_TYPEOF(const u8* const*, GP(sRoomCameraControlDataTable[0])) list;
    const u8* entry;
    GBA_TYPEOF(const u8**, &gCurrentRoomCameraControlData) output;
    register u8* roomPtr PIN("r4");
    u8 room;
    s32 roomId;

    list = GP(sRoomCameraControlDataTable[gStageRoomTableIndex]);
    roomPtr = &gCurrentRoom;
    output = &gCurrentRoomCameraControlData;
    while ((roomId = (entry = GP(*list))[0]) != (room = *roomPtr) && roomId != 0xFF) {
        list++;
    }
    GP_SET(*output, entry);
}

void BuildCurrentRoomCameraControlAreas(void)
{
    register u32 value PIN("r0");
    register u32 countOrBoundary PIN("r1");
    register u32 baseAddress PIN("r2");
    s32 offsetOrLimit;
    u32 recordAddress;
    u32 areaAddress;
    register u32 backgroundInfoAddress PIN("r6");
    struct CameraControlArea* areas;
    u32 xTile;
    u32 remaining;
    u32 nextRecord;
    u32 yTile;
    u32 fieldOffsets[4];
    u32* fieldOffsetPtr;

    baseAddress = (u32)gCameraControlAreas;
    value = 0;
    *(u8*)(baseAddress + 0) = value;
    *(u8*)(baseAddress + 12) = value;
    countOrBoundary = (u32)&gWarioData;
    value = *(u16*)(countOrBoundary + 18);
    value >>= 6;
    xTile = value;
    value = *(u16*)(countOrBoundary + 20);
    value >>= 6;
    yTile = value;
    value = (u32)&gCurrentRoomCameraControlData;
    value = *(u32*)(value + 0);
    value += 1;
    countOrBoundary = *(u8*)(value + 0);
    recordAddress = value + 1;
    areas = (struct CameraControlArea*)baseAddress;
    if (countOrBoundary == 0) {
        goto recordsDone;
    }
    backgroundInfoAddress = (u32)&gBackgroundInfo;
    areaAddress = (u32)areas;
recordLoop:
    value = (u32)gCameraControlAreas;
    value += 24;
    if (areaAddress == value) {
        goto functionDone;
    }
    offsetOrLimit = 1;
    value = 9;
    value = recordAddress + value;
    nextRecord = value;
    countOrBoundary -= 1;
    remaining = countOrBoundary;
    fieldOffsetPtr = fieldOffsets;
offsetLoop:
    *fieldOffsetPtr++ = offsetOrLimit;
    offsetOrLimit += 1;
    if (offsetOrLimit <= 4) {
        goto offsetLoop;
    }
    value = *(volatile u8*)(recordAddress + 5);
    if (value == 0xFF) {
        goto testBounds;
    }
    value = *(u8*)(recordAddress + 8);
    if (value == 0xFF) {
        goto testBounds;
    }
    countOrBoundary = *(u8*)(recordAddress + 6);
    value = *(u16*)(backgroundInfoAddress + 12);
    value *= countOrBoundary;
    countOrBoundary = *(u8*)(recordAddress + 5);
    offsetOrLimit = value + countOrBoundary;
    countOrBoundary = *(u32*)(backgroundInfoAddress + 8);
    value = offsetOrLimit << 1;
    value += countOrBoundary;
    value = *(u16*)(value + 0);
    if (value != 0) {
        goto testBounds;
    }
    value = *(u8*)(recordAddress + 7);
    if (value == 0xFF) {
        goto testBounds;
    }
    value <<= 2;
    baseAddress = (u32)fieldOffsets;
    countOrBoundary = baseAddress + value;
    value = 8;
    *(u32*)(countOrBoundary + 0) = value;
testBounds:
    value = fieldOffsets[0];
    value = recordAddress + value;
    countOrBoundary = *(u8*)(value + 0);
    if (countOrBoundary > xTile) {
        goto next;
    }
    value = fieldOffsets[1];
    value = recordAddress + value;
    value = *(u8*)(value + 0);
    if (xTile > value) {
        goto next;
    }
    value = fieldOffsets[2];
    value = recordAddress + value;
    value = *(u8*)(value + 0);
    if (value > yTile) {
        goto next;
    }
    value = fieldOffsets[3];
    value = recordAddress + value;
    value = *(u8*)(value + 0);
    if (yTile > value) {
        goto next;
    }
    value = *(u8*)(areaAddress + 0);
    if (value != 0) {
        goto next;
    }
    countOrBoundary <<= 6;
    baseAddress = (u32)gRoomCameraBounds;
    value = *(u16*)(baseAddress + 2);
    offsetOrLimit = baseAddress;
    if ((s32)value < (s32)countOrBoundary) {
        value = countOrBoundary;
    }
    *(u16*)(areaAddress + 4) = value;
    value = *(u16*)(backgroundInfoAddress + 12);
    value <<= 6;
    countOrBoundary = *(u16*)(offsetOrLimit + 0);
    offsetOrLimit = value - countOrBoundary;
    value = fieldOffsets[1];
    value = recordAddress + value;
    value = *(u8*)(value + 0);
    value += 1;
    value <<= 6;
    countOrBoundary = offsetOrLimit;
    if ((s32)value < (s32)offsetOrLimit) {
        countOrBoundary = value;
    }
    *(u16*)(areaAddress + 2) = countOrBoundary;
    value = fieldOffsets[2];
    value = recordAddress + value;
    value = *(u8*)(value + 0);
    value <<= 6;
    countOrBoundary = *(u16*)(baseAddress + 4);
    if ((s32)countOrBoundary < (s32)value) {
        countOrBoundary = value;
    }
    *(u16*)(areaAddress + 6) = countOrBoundary;
    value = *(u16*)(backgroundInfoAddress + 14);
    value <<= 6;
    countOrBoundary = *(u16*)(baseAddress + 6);
    offsetOrLimit = value - countOrBoundary;
    value = fieldOffsets[3];
    value = recordAddress + value;
    value = *(u8*)(value + 0);
    value += 1;
    value <<= 6;
    countOrBoundary = offsetOrLimit;
    if ((s32)value < (s32)offsetOrLimit) {
        countOrBoundary = value;
    }
    *(u16*)(areaAddress + 8) = countOrBoundary;
    value = *(u8*)(recordAddress + 0);
    *(u8*)(areaAddress + 0) = value;
    areaAddress += 12;
next:
    recordAddress = nextRecord;
    countOrBoundary = remaining;
    if (countOrBoundary != 0) {
        goto recordLoop;
    }
recordsDone:
    value = areas[0].mode;
    if (value != 0) {
        goto functionDone;
    }
    value = areas[1].mode;
    if (value != 0) {
        goto functionDone;
    }
    areas[0].mode = value;
    areas[0].maxX = value;
    areas[0].minX = value;
    areas[0].minY = value;
    areas[0].maxY = value;
functionDone:
    return;
}

void UpdateBottomRoomCamera(const struct CameraTargetPosition* position)
{
    struct CameraControlArea* area;
    s32 x;
    s32 y;

    BuildBottomRoomCameraControlAreas();
    area = gCameraControlAreas;
    x = GetCameraXForControlArea(area, position);
    y = GetCameraYForControlArea(area, position);
    area++;
    if (area->mode != 0) {
        x = (x + GetCameraXForControlArea(area, position)) >> 1;
        y = (y + GetCameraYForControlArea(area, position)) >> 1;
    }
    MoveCameraTowardTarget((u16)x, (u16)y);
}

void BuildBottomRoomCameraControlAreas(void)
{
    struct CameraControlArea* areas;
    s32 warioY;
    u32 temp;
    s32 index;
    s32 boundary;

    areas = gCameraControlAreas;
    areas[0].mode = 0;
    areas[1].mode = 0;
    index = 0;
    temp = gWarioData.yPosition >> 6;
    warioY = temp;

    boundary = gBackgroundInfo.bg1Height - 12;
    if (warioY < boundary) {
        areas[0].mode = 2;
        areas[0].minX = 128;
        areas[0].maxX = (gBackgroundInfo.bg1Width - 2) << 6;
        areas[0].minY = 128;
        areas[0].maxY = boundary << 6;
        index = 1;
    }
    boundary = gBackgroundInfo.bg1Height - 16;
    if (boundary <= warioY) {
        areas[index].mode = 1;
        areas[index].minX = 128;
        areas[index].maxX = (gBackgroundInfo.bg1Width - 2) << 6;
        areas[index].minY = boundary << 6;
        areas[index].maxY = (gBackgroundInfo.bg1Height - 2) << 6;
    }
}

void UpdateCamera(void)
{
    struct CameraTargetPosition position;
    u8 cameraControl;

    if (gWarioData.unk_02 != 0 && gSubGameMode != 0) {
        return;
    }
    if (gSubGameMode == 3) {
        return;
    }

    position.x = gWarioData.xPosition;
    position.y = gWarioData.yPosition;
    if (position.y & 0x8000) {
        position.y = 0;
    }

    if (gGameScreenDebugInfo.freeCameraEnabled == 0) {
        goto loadCameraControl;
    }
    if (gSubGameMode != 8) {
        goto loadCameraControl;
    }
    goto dispatchClamp;

updateOtherCameraModes:
    if (cameraControl == 4) {
        goto updateBottomCamera;
    }
    UpdateStandardCamera(&position);
    goto cameraUpdated;

dispatchClamp:
    /* Taking this label as a zero-output operand preserves agbcc's original block split. */
    ASM_BARRIER_VOLATILE(: : "g"(&&dispatchClamp));
    goto clampCamera;

clampCamera:
    SetCameraPositionClampedToRoom(&position);
    goto cameraUpdated;

loadCameraControl:
    cameraControl = gCurrentRoomHeader.cameraControl;
    if (cameraControl != 3) {
        goto updateOtherCameraModes;
    }
    UpdateCameraFromControlAreas(&position);

cameraUpdated:
    if ((gCurrentRoomHeader.bg0Param & 0xF) == 0) {
        goto copyPrimaryPosition;
    }
    UpdateBg0CameraPosition(&position);
    goto backgroundPositionUpdated;

updateBottomCamera:
    UpdateBottomRoomCamera(&position);
    goto cameraUpdated;

copyPrimaryPosition:
    gBg0XPosition = gBg1XPosition;
    gBg0YPosition = gBg1YPosition;

backgroundPositionUpdated:
    if (gBg0ScrollEffect.type != 0) {
        UpdateBg0ScrollEffect();
    }

    gBg2XPosition = gBg1XPosition;
    gBg2YPosition = gBg1YPosition;
    if ((gCurrentRoomHeader.bg2Param & 0xF) != 0) {
        gBg2XPosition = (u16)-32 + gBg1XPosition;
    }
}

void UpdateStandardCamera(const struct CameraTargetPosition* position)
{
    s32 value;
    s32 limit;
    s32 positionValue;
    register s32 cameraTarget PIN("r3");
    register s32 positionAddress PIN("r4");
    s32 boundsAddress;
    s32 cameraStateAddress;
    s16* subGameModePtr;

    positionAddress = (u32)position;
    positionValue = *(u16*)(positionAddress + 0);
    value = (u32)gRoomCameraBounds;
    cameraTarget = *(u16*)(value + 2);
    boundsAddress = 480;
    limit = cameraTarget + boundsAddress;
    boundsAddress = value;
    if (positionValue < limit) {
        goto horizontalReady;
    }
    value = (u32)&gBackgroundInfo;
    value = *(u16*)(value + 12);
    value <<= 6;
    limit = -480;
    value += limit;
    limit = *(u16*)(boundsAddress + 0);
    value -= limit;
    if (positionValue <= value) {
        goto horizontalInside;
    }
    positionValue = -480;
    cameraTarget = value + positionValue;
    goto horizontalReady;
horizontalInside:
    value = -480;
    cameraTarget = positionValue + value;
horizontalReady:
    positionValue = (u32)&gCameraPositionState;
    *(u16*)(positionValue + 0) = cameraTarget;
    limit = (u32)&gBg1XPosition;
    value = *(u16*)(limit + 0);
    cameraTarget -= value;
    cameraStateAddress = positionValue;
    if (cameraTarget <= 0) {
        goto horizontalNegative;
    }
    if (cameraTarget <= 48) {
        goto horizontalClamped;
    }
    cameraTarget = 48;
    goto horizontalClamped;
horizontalNegative:
    value = -48;
    if (cameraTarget >= value) {
        goto horizontalClamped;
    }
    cameraTarget = value;
horizontalClamped:
    *(u8*)(cameraStateAddress + 4) = cameraTarget;
    value = *(u16*)(limit + 0);
    value += cameraTarget;
    *(u16*)(limit + 0) = value;

    value = (u32)&gCurrentRoomHeader;
    value = *(u8*)(value + 24);
    if (value == 1) {
        goto specialCamera;
    }

    positionValue = *(u16*)(positionAddress + 2);
    limit = *(u16*)(boundsAddress + 4);
    cameraTarget = 448;
    value = limit + cameraTarget;
    if (positionValue >= value) {
        goto verticalPastStart;
    }
    cameraTarget = limit;
    goto verticalReady;
verticalPastStart:
    value = (u32)&gBackgroundInfo;
    value = *(u16*)(value + 14);
    value <<= 6;
    value -= 192;
    limit = *(u16*)(boundsAddress + 6);
    value -= limit;
    boundsAddress = -448;
    cameraTarget = positionValue + boundsAddress;
    if (positionValue <= value) {
        goto verticalReady;
    }
    cameraTarget = value + boundsAddress;
verticalReady:
    *(u16*)(cameraStateAddress + 2) = cameraTarget;
    limit = (u32)&gBg1YPosition;
    value = *(u16*)(limit + 0);
    cameraTarget -= value;
    if (cameraTarget <= 0) {
        goto verticalNegative;
    }
    if (cameraTarget <= 48) {
        goto verticalClamped;
    }
    cameraTarget = 48;
    goto verticalClamped;
verticalNegative:
    value = -48;
    if (cameraTarget >= value) {
        goto verticalClamped;
    }
    cameraTarget = value;
verticalClamped:
    *(u8*)(cameraStateAddress + 5) = cameraTarget;
    value = *(u16*)(limit + 0);
    value += cameraTarget;
    *(u16*)(limit + 0) = value;
    goto done;

specialCamera:
    value = (u32)&gSubGameMode;
    positionValue = 0;
    limit = *(s16*)(value + positionValue);
    subGameModePtr = (s16*)value;
    if (limit == 8) {
        goto done;
    }
    value = *(u16*)(boundsAddress + 4);
    cameraTarget = value;
    cameraTarget -= 32;
    boundsAddress = 608;
    value += boundsAddress;
    limit = *(u16*)(positionAddress + 2);
    boundsAddress = (u32)&gBackgroundInfo;
    if (value >= limit) {
        goto specialTargetReady;
    }
    positionAddress = 576;
    positionValue = 640;
specialTargetLoop:
    cameraTarget += positionAddress;
    value = cameraTarget + positionValue;
    if (value < limit) {
        goto specialTargetLoop;
    }
specialTargetReady:
    *(u16*)(cameraStateAddress + 2) = cameraTarget;
    value = 0;
    positionAddress = *(s16*)((u32)subGameModePtr + value);
    if (positionAddress != 0) {
        goto activeSpecial;
    }
    positionValue = 640;
    limit = cameraTarget + positionValue;
    value = *(u16*)(boundsAddress + 14);
    value <<= 6;
    if (limit <= value) {
        goto done;
    }
    boundsAddress = -576;
    value = cameraTarget + boundsAddress;
    *(u16*)(cameraStateAddress + 2) = value;
    goto done;
activeSpecial:
    value = 672;
    limit = cameraTarget + value;
    value = *(u16*)(boundsAddress + 14);
    value <<= 6;
    if (limit > value) {
        goto done;
    }
    value = (u32)&gBg1YPosition;
    positionValue = *(u16*)(value + 0);
    limit = value;
    if (positionValue == cameraTarget) {
        goto done;
    }
    if (positionAddress != 5) {
        goto beginSpecialTransition;
    }
    if (positionValue <= cameraTarget) {
        goto moveSpecialDown;
    }
    value = positionValue - cameraTarget;
    if (value <= 31) {
        goto snapSpecial;
    }
    value = positionValue;
    value -= 32;
    goto storeSpecial;
moveSpecialDown:
    value = cameraTarget - positionValue;
    if (value > 31) {
        goto stepSpecialDown;
    }
snapSpecial:
    *(u16*)(limit + 0) = cameraTarget;
    goto testSpecialDone;
stepSpecialDown:
    value = positionValue;
    value += 32;
storeSpecial:
    *(u16*)(limit + 0) = value;
testSpecialDone:
    value = *(u16*)(limit + 0);
    if (value != cameraTarget) {
        goto done;
    }
    value = 2;
    goto storeMode;
beginSpecialTransition:
    positionAddress = 0;
    if (positionValue <= cameraTarget) {
        goto allowTransition;
    }
    limit = (u32)&gWarioData;
    value = *(u8*)(limit + 26);
    if (value == 0) {
        goto setModeFive;
    }
    value = *(u8*)(limit + 0);
    if (value != 0) {
        goto checkTransition;
    }
    value = *(u8*)(limit + 1);
    if (value == 40) {
        goto setModeFive;
    }
    if (value == 43) {
        goto setModeFive;
    }
    if (value == 44) {
        goto setModeFive;
    }
    if (value == 41) {
        goto setModeFive;
    }
    if (value != 45) {
        goto checkTransition;
    }
    goto setModeFive;
allowTransition:
    positionAddress = 1;
checkTransition:
    if (positionAddress == 0) {
        goto done;
    }
setModeFive:
    value = 5;
storeMode:
    *(u16*)subGameModePtr = value;
done:
    return;
}

void UpdateBg0CameraPosition(const struct CameraTargetPosition* unusedPosition)
{
    u32 mode;

    /* The caller passes this pointer in r0 in the original code. */
    (void)unusedPosition;

    mode = gCurrentRoomHeader.bg0Param;

    if (mode == 18) {
        gBg0XPosition = gBg1XPosition >> 1;
        gBg0YPosition = gBg1YPosition;
        return;
    }
    if (mode == 34) {
        gBg0XPosition = gBg1XPosition >> 1;
        gBg0YPosition = gBg1YPosition;
        return;
    }
    if (mode == 17) {
        u16* output;
        u16* scroll;
        u16* bounds;

        output = &gBg0XPosition;
        scroll = &gUnk_300003A;
        bounds = gRoomCameraBounds;
        *output = *scroll + bounds[1] - 32;
        gBg0YPosition = (u16)-32 + bounds[2];
        return;
    }
    if (mode == 33) {
        gBg0XPosition = gUnk_300003A;
        gBg0YPosition = gGoldenBossTransitionBg0Y;
    }
}

void UpdateBg0ScrollEffect(void)
{
    if (gBg0ScrollEffect.type == 1) {
        if ((gBg0ScrollEffect.timer & 7) == 0) {
            gBg0ScrollEffect.value++;
        }
    } else if (gBg0ScrollEffect.type == 2) {
        if ((gBg0ScrollEffect.timer & 3) == 0) {
            gBg0ScrollEffect.value++;
        }
    }
    gBg0ScrollEffect.timer++;
}

void SetCameraPositionClampedToRoom(const struct CameraTargetPosition* position)
{
    s32 roomSize;

    if (position->x <= 479) {
        gBg1XPosition = 0;
    } else {
        u16 x;

        x = position->x;
        roomSize = gBackgroundInfo.bg1Width << 6;
        if (x > roomSize - 480) {
            gBg1XPosition = roomSize - 960;
        } else {
            gBg1XPosition = x - 480;
        }
    }

    if (position->y <= 447) {
        gBg1YPosition = 0;
    } else {
        u16 y;

        y = position->y;
        roomSize = gBackgroundInfo.bg1Height << 6;
        if (y > roomSize - 192) {
            gBg1YPosition = roomSize - 640;
        } else {
            gBg1YPosition = y - 448;
        }
    }
}

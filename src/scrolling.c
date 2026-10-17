#include "scrolling.h"
#include "background_registers.h"
#include "global_data.h"
#include "screen_shake.h"

void UpdateBackgroundScrollRegisters()
{
    s32 temp_r0;
    s32 temp_r6;
    s16 valueX1;
    s16 valueY1;
    s16 valueX2;
    s16 valueY2;
    s16 valueX3;
    s16 valueY3;

    temp_r6 = ScreenShakeUpdateY();
    temp_r0 = ScreenShakeUpdateX();

    valueX1 = (((u16)gBg1XPosition >> 2) & 0x1FF);
    valueY1 = (((u16)gBg1YPosition >> 2) & 0x1FF);

    gBackgroundScroll.bg1Horizontal = valueX1;
    gBackgroundScroll.bg1Horizontal += temp_r0;
    gBackgroundScroll.bg1Vertical = valueY1;
    gBackgroundScroll.bg1Vertical += temp_r6;

    valueX2 = (((u16)gBg2XPosition >> 2) & 0x1FF);
    gBackgroundScroll.bg2Horizontal = valueX2;
    gBackgroundScroll.bg2Horizontal += temp_r0;

    valueY2 = (((u16)gBg2YPosition >> 2) & 0x1FF);
    gBackgroundScroll.bg2Vertical = valueY2;
    gBackgroundScroll.bg2Vertical += temp_r6;

    gBackgroundScroll.bg0Horizontal = ((((u16)gBg0XPosition >> 2) + gBg0ScrollEffect.value) & 0x1FF) + temp_r0;
    gBackgroundScroll.bg0Vertical = (((u16)gBg0YPosition >> 2) & 0x1FF) + temp_r6;
    UpdateLayer3ScrollPosition();
    UpdateLayer3ScrollEffect();

    valueX3 = (((u16)gBg3XPosition >> 2) + gLayer3ScrollEffectState.value) & 0x1FF;
    valueY3 = (((u16)gBg3YPosition >> 2) & 0x1FF);

    gBackgroundScroll.bg3Horizontal = valueX3;
    gBackgroundScroll.bg3Vertical = valueY3;
    gBackgroundScroll.bg3Vertical += (temp_r6 >> 1);
}

void UpdateLayer3ScrollPosition()
{
    s32 var_r2;
    s32 var_r3;

    var_r2 = 0;
    var_r3 = 0;
    switch (gCurrentRoomHeader.layer3Scrolling) {
        case 0:
            break;
        case 1:
            var_r2 = 2;
            var_r3 = 0;
            break;
        case 2:
            var_r2 = 0;
            var_r3 = 2;
            break;
        case 3:
            var_r2 = 2;
            var_r3 = 2;
            break;
        case 4:
            var_r2 = 1;
            var_r3 = 2;
            break;
        case 5:
            var_r2 = 2;
            var_r3 = 1;
            break;
        case 6:
            var_r2 = 1;
            var_r3 = 1;
            break;
    }

    if (var_r2 == 0) {
        gBg3XPosition = gRoomCameraBounds[0];
    } else if (var_r2 == 1) {
        gBg3XPosition = gBg1XPosition;
    } else {
        gBg3XPosition = (u16)gBg1XPosition >> 1;
    }

    if (var_r3 == 0) {
        gBg3YPosition = gRoomCameraBounds[2];
        if (gCurrentRoomHeader.cameraControl == 1) {
            gBg3YPosition = gRoomCameraBounds[2] - 0x20;
        }
    } else if (var_r3 == 1) {
        gBg3YPosition = gBg1YPosition;
    } else {
        gBg3YPosition = (u16)gBg1YPosition >> 1;
    }
}

void UpdateLayer3ScrollEffect()
{
    if (gLayer3ScrollEffectState.type != 0) {
        if ((gLayer3ScrollEffectState.type == 1)) {
            if (!(7 & gLayer3ScrollEffectState.timer)) {
                gLayer3ScrollEffectState.value += 1;
            }
        }
        gLayer3ScrollEffectState.timer += 1;
    }
}

typedef struct AGB_ALIGN VerticalScrollingTileUpload {
    u8 tileCount;
    u8 unused;
    u16 destinationBlock;
    u16 destinationOffset;
    u16 tileData[77];
} VerticalScrollingTileUpload;

extern VerticalScrollingTileUpload gVerticalScrollingTileUploads[3];

void PrepareVerticalScrollingTileUploads(s32 scrollDelta)
{
    u8 layerParams[3] = {
        gCurrentRoomHeader.bg0Param,
        gCurrentRoomHeader.bg1Param,
        gCurrentRoomHeader.bg2Param,
    };
    u32 scratch0;
    register u32 scratch1 PIN("r1");
    register u32 scratch2 PIN("r2");
    s32 signedRow;
    u32 xPositionOrTileIndex;
    u32 layerOrBackground;
    s32 copiedTiles;
    u32 background;
    s32 tileCount;
    VerticalScrollingTileUpload* nextUpload;
    s32 nextLayer;
    register VerticalScrollingTileUpload* upload PIN("ip");
    u16* destination;

    upload = gVerticalScrollingTileUploads;
    layerOrBackground = 0;

layer_loop:
    scratch0 = 0;
    scratch1 = (u32)upload;
    *(u8*)scratch1 = scratch0;

    scratch2 = (u32)layerParams;
    ASM_BARRIER(: "+r"(scratch2));
    scratch0 = scratch2 + layerOrBackground;
    scratch1 = *(u8*)scratch0;
    scratch0 = 0x10;
    scratch0 &= scratch1;

    scratch1 = layerOrBackground + 1;
    nextLayer = scratch1;
    scratch2 = sizeof(*upload);
    scratch2 += (u32)upload;
    nextUpload = (VerticalScrollingTileUpload*)scratch2;

    if (scratch0 == 0) {
        goto next_layer;
    }

    if (layerOrBackground == 0) {
        scratch0 = (u32)&gBg0YPosition;
        scratch1 = *(u16*)scratch0;
        scratch0 = (u32)&gBg0XPosition;
    } else if (layerOrBackground == 1) {
        scratch0 = (u32)&gBg1YPosition;
        scratch1 = *(u16*)scratch0;
        scratch0 = (u32)&gBg1XPosition;
    } else {
        scratch0 = (u32)&gBg2YPosition;
        scratch1 = *(u16*)scratch0;
        scratch0 = (u32)&gBg2XPosition;
    }

    xPositionOrTileIndex = *(u16*)scratch0;
    scratch2 = scrollDelta;
    scratch0 = scratch2 << 16;
    scratch2 = scratch0 >> 16;
    scratch0 = scratch1 >> 6;
    scratch0 = (scratch2) - (-(scratch0));
    scratch0 <<= 16;
    scratch2 = scratch0 >> 16;
    signedRow = (s32)scratch0 >> 16;
    ASM_BARRIER_VOLATILE(: "+r"(layerOrBackground), "+r"(upload), "+r"(signedRow));

    scratch0 = layerOrBackground + 1;
    nextLayer = scratch0;
    scratch1 = sizeof(*upload);
    scratch1 += (u32)upload;
    nextUpload = (VerticalScrollingTileUpload*)scratch1;

    if (signedRow < 0) {
        goto next_layer;
    }

    scratch1 = (u32)&gBackgroundInfo;
    scratch0 = layerOrBackground << 3;
    layerOrBackground = scratch0 + scratch1;
    scratch0 = *(u16*)(layerOrBackground + 6);
    if (signedRow > (s32)scratch0) {
        goto next_layer;
    }

    scratch1 = (u32)upload;
    *(u16*)(scratch1 + 4) = scratch2;

    scratch0 = xPositionOrTileIndex >> 6;
    scratch0 -= 2;
    scratch0 <<= 16;
    scratch2 = scratch0 >> 16;
    if ((s32)scratch0 < 0) {
        scratch2 = 0;
    }

    scratch0 = (u32)upload;
    *(u16*)(scratch0 + 2) = scratch2;

    copiedTiles = 19;
    scratch1 = 19;
    tileCount = scratch1;
    scratch0 = *(u16*)(layerOrBackground + 4);
    if ((s32)scratch0 < copiedTiles) {
        tileCount = scratch0;
    }

    scratch2 = tileCount;
    ASM_BARRIER(: "+r"(scratch2));
    scratch2 <<= 2;
    tileCount = scratch2;
    scratch0 = (u32)upload;
    *(u8*)scratch0 = scratch2;

    scratch0 = *(u16*)(layerOrBackground + 4);
    scratch2 = (u32)upload;
    scratch1 = *(u16*)(scratch2 + 4);
    scratch0 *= scratch1;
    scratch1 = *(u16*)(scratch2 + 2);
    xPositionOrTileIndex = scratch0 + scratch1;

    copiedTiles = 0;
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }

    background = layerOrBackground;
    layerOrBackground = (u32)&gBackgroundTileTables;
    destination = upload->tileData;

copy_loop:
#define COPY_METATILE() \
    do { \
        scratch1 = *(u32 *)background; \
        scratch0 = xPositionOrTileIndex << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        scratch2 = scratch0 << 2; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 <<= 3; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        scratch2++; \
        destination++; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 = scratch2 << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        scratch2++; \
        destination++; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 = scratch2 << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        scratch2++; \
        destination++; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 = scratch2 << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        destination++; \
        copiedTiles += 4; \
        xPositionOrTileIndex++; \
    } while (0)

    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles < tileCount) {
        goto copy_loop;
    }

#undef COPY_METATILE
next_layer:
    layerOrBackground = nextLayer;
    upload = nextUpload;
    if ((s32)layerOrBackground <= 2) {
        goto layer_loop;
    }
}

void ApplyVerticalScrollingTileUploads(void)
{
    VerticalScrollingTileUpload* upload;
    VerticalScrollingTileUpload* descriptor;
    VerticalScrollingTileUpload* nextUpload;
    register VerticalScrollingTileUpload* fieldDescriptor PIN("r2");
    register u16* source PIN("r4");
    u32 sourceOffset;
    u32 firstTileOffset;
    u32 firstMask;
    register volatile u16* destination PIN("r1");
    u32 vramBase;
    u32 destinationBlock;
    u8 tileCount;
    s32 copiedTiles;
    u32 blockMask;
    u32 rowBit;
    register u32 rowOffset PIN("ip");
    u32 tileOffset;
    s32 uploadIndex;
    s32 nextUploadIndex;

    upload = gVerticalScrollingTileUploads;
    uploadIndex = 0;

outer_loop_vertical:
    descriptor = upload;
    tileCount = descriptor->tileCount;
    nextUploadIndex = uploadIndex + 1;
    nextUpload = descriptor + 1;
    if (tileCount != 0) {
        vramBase = 0x06000000 + (uploadIndex << 12);
        fieldDescriptor = upload;
        vramBase += (fieldDescriptor->destinationOffset & 0xF) << 7;
        destinationBlock = fieldDescriptor->destinationBlock;
        copiedTiles = 0;

        if (copiedTiles < tileCount) {
            {
                register u32 constant PIN("r0");
                constant = 0xF;
                ASM_BARRIER(: "+r"(constant));
                blockMask = constant;
            }
            {
                register u32 constant PIN("r1");
                constant = 0x10;
                ASM_BARRIER(: "+r"(constant));
                rowBit = constant;
            }
            {
                u32 constant;
                constant = 0x80;
                constant <<= 3;
                rowOffset = constant;
            }
        copy_loop_vertical:
            firstTileOffset = destinationBlock;
            firstMask = blockMask;
            firstTileOffset &= firstMask;
            tileOffset = firstTileOffset << 1;
            if ((destinationBlock & rowBit) != 0) {
                tileOffset += rowOffset;
            }
            destination = (volatile u16*)((tileOffset << 1) + vramBase);
            sourceOffset = copiedTiles << 1;
            source = (u16*)upload;
            source += 3;
            destination[0] = *(u16*)((u32)source + sourceOffset);
            copiedTiles++;
            destination[1] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x20] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x21] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done_vertical;
            }

            tileOffset = (destinationBlock & blockMask) << 1;
            if ((destinationBlock & rowBit) != 0) {
                tileOffset += rowOffset;
            }
            destination = (volatile u16*)((tileOffset << 1) + vramBase);
            destination[0] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[1] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x20] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x21] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done_vertical;
            }

            tileOffset = (destinationBlock & blockMask) << 1;
            if ((destinationBlock & rowBit) != 0) {
                tileOffset += rowOffset;
            }
            destination = (volatile u16*)((tileOffset << 1) + vramBase);
            destination[0] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[1] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x20] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x21] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done_vertical;
            }

            tileOffset = (destinationBlock & blockMask) << 1;
            if ((destinationBlock & rowBit) != 0) {
                tileOffset += rowOffset;
            }
            destination = (volatile u16*)((tileOffset << 1) + vramBase);
            destination[0] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[1] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x20] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x21] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done_vertical;
            }

            tileOffset = (destinationBlock & blockMask) << 1;
            if ((destinationBlock & rowBit) != 0) {
                tileOffset += rowOffset;
            }
            destination = (volatile u16*)((tileOffset << 1) + vramBase);
            destination[0] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[1] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x20] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x21] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done_vertical;
            }

            tileOffset = (destinationBlock & blockMask) << 1;
            if ((destinationBlock & rowBit) != 0) {
                tileOffset += rowOffset;
            }
            destination = (volatile u16*)((tileOffset << 1) + vramBase);
            destination[0] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[1] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x20] = *(u16*)((u32)source + (copiedTiles++ << 1));
            destination[0x21] = *(u16*)((u32)source + (copiedTiles++ << 1));
            ASM_BARRIER(: : "r"(source));
            destinationBlock++;
            if (copiedTiles < tileCount) {
                goto copy_loop_vertical;
            }
        }
    }

descriptor_done_vertical:
    uploadIndex = nextUploadIndex;
    upload = nextUpload;
    if (uploadIndex <= 2) {
        goto outer_loop_vertical;
    }
}

typedef struct AGB_ALIGN HorizontalScrollingTileUpload {
    u8 tileCount;
    u8 unused;
    u16 destinationFlags;
    u16 destinationBlock;
    u16 tileData[57];
} HorizontalScrollingTileUpload;

extern HorizontalScrollingTileUpload gHorizontalScrollingTileUploads[3];

void PrepareHorizontalScrollingTileUploads(s32 scrollDelta)
{
    u8 layerParams[3] = {
        gCurrentRoomHeader.bg0Param,
        gCurrentRoomHeader.bg1Param,
        gCurrentRoomHeader.bg2Param,
    };
    u32 scratch0;
    register u32 scratch1 PIN("r1");
    register u32 scratch2 PIN("r2");
    s32 signedColumn;
    u32 yPositionOrTileIndex;
    u32 layerOrBackground;
    s32 copiedTiles;
    u32 background;
    s32 tileCount;
    HorizontalScrollingTileUpload* nextUpload;
    s32 nextLayer;
    HorizontalScrollingTileUpload* upload;
    u16* destination;

    upload = gHorizontalScrollingTileUploads;
    layerOrBackground = 0;

layer_loop:
    scratch0 = 0;
    scratch1 = (u32)upload;
    *(u8*)scratch1 = scratch0;

    scratch2 = (u32)layerParams;
    ASM_BARRIER(: "+r"(scratch2));
    scratch0 = scratch2 + layerOrBackground;
    scratch1 = *(u8*)scratch0;
    scratch0 = 0x10;
    scratch0 &= scratch1;

    scratch1 = layerOrBackground + 1;
    nextLayer = scratch1;
    scratch2 = sizeof(*upload);
    scratch2 += (u32)upload;
    nextUpload = (HorizontalScrollingTileUpload*)scratch2;

    if (scratch0 == 0) {
        goto next_layer;
    }

    if (layerOrBackground == 0) {
        scratch0 = (u32)&gBg0YPosition;
        yPositionOrTileIndex = *(u16*)scratch0;
        scratch0 = (u32)&gBg0XPosition;
    } else if (layerOrBackground == 1) {
        scratch0 = (u32)&gBg1YPosition;
        yPositionOrTileIndex = *(u16*)scratch0;
        scratch0 = (u32)&gBg1XPosition;
    } else {
        scratch0 = (u32)&gBg2YPosition;
        yPositionOrTileIndex = *(u16*)scratch0;
        scratch0 = (u32)&gBg2XPosition;
    }

    scratch1 = *(u16*)scratch0;
    scratch2 = scrollDelta;
    scratch0 = scratch2 << 16;
    scratch2 = scratch0 >> 16;
    scratch0 = scratch1 >> 6;
    scratch0 = (scratch2) - (-(scratch0));
    scratch0 <<= 16;
    scratch2 = scratch0 >> 16;
    signedColumn = (s32)scratch0 >> 16;
    ASM_BARRIER_VOLATILE(: "+r"(layerOrBackground), "+r"(upload), "+r"(signedColumn));

    scratch0 = layerOrBackground + 1;
    nextLayer = scratch0;
    scratch1 = sizeof(*upload);
    scratch1 += (u32)upload;
    nextUpload = (HorizontalScrollingTileUpload*)scratch1;

    if (signedColumn < 0) {
        goto next_layer;
    }

    scratch1 = (u32)&gBackgroundInfo;
    scratch0 = layerOrBackground << 3;
    layerOrBackground = scratch0 + scratch1;
    scratch0 = *(u16*)(layerOrBackground + 4);
    if (signedColumn > (s32)scratch0) {
        goto next_layer;
    }

    scratch1 = (u32)upload;
    *(u16*)(scratch1 + 2) = scratch2;

    scratch0 = yPositionOrTileIndex >> 6;
    scratch0 -= 2;
    scratch0 <<= 16;
    scratch2 = scratch0 >> 16;
    if ((s32)scratch0 < 0) {
        scratch2 = 0;
    }

    scratch0 = (u32)upload;
    *(u16*)(scratch0 + 4) = scratch2;

    copiedTiles = 14;
    scratch1 = 14;
    tileCount = scratch1;
    scratch0 = *(u16*)(layerOrBackground + 6);
    if ((s32)scratch0 < copiedTiles) {
        tileCount = scratch0;
    }

    scratch2 = tileCount;
    ASM_BARRIER(: "+r"(scratch2));
    scratch2 <<= 2;
    tileCount = scratch2;
    scratch0 = (u32)upload;
    *(u8*)scratch0 = scratch2;

    scratch0 = *(u16*)(layerOrBackground + 4);
    scratch2 = (u32)upload;
    scratch1 = *(u16*)(scratch2 + 4);
    scratch0 *= scratch1;
    scratch1 = *(u16*)(scratch2 + 2);
    yPositionOrTileIndex = scratch0 + scratch1;

    copiedTiles = 0;
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }

    background = layerOrBackground;
    layerOrBackground = (u32)&gBackgroundTileTables;
    destination = upload->tileData;

copy_loop:
#define COPY_METATILE() \
    do { \
        scratch1 = *(u32 *)background; \
        scratch0 = yPositionOrTileIndex << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        scratch2 = scratch0 << 2; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 <<= 3; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        scratch2++; \
        destination++; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 = scratch2 << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        scratch2++; \
        destination++; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 = scratch2 << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        scratch2++; \
        destination++; \
        scratch1 = *(volatile u32 *)layerOrBackground; \
        scratch0 = scratch2 << 1; \
        scratch0 += scratch1; \
        scratch0 = *(u16 *)scratch0; \
        *destination = scratch0; \
        destination++; \
        copiedTiles += 4; \
        scratch0 = *(u16 *)(background + 4); \
        yPositionOrTileIndex += scratch0; \
    } while (0)

    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles >= tileCount) {
        goto next_layer;
    }
    COPY_METATILE();
    if (copiedTiles < tileCount) {
        goto copy_loop;
    }

#undef COPY_METATILE
next_layer:
    layerOrBackground = nextLayer;
    upload = nextUpload;
    if ((s32)layerOrBackground <= 2) {
        goto layer_loop;
    }
}

void ApplyHorizontalScrollingTileUploads(void)
{
    HorizontalScrollingTileUpload* upload;
    HorizontalScrollingTileUpload* nextUpload;
    u16* source;
    register volatile u16* destination PIN("r0");
    u32 vramBase;
    u16 destinationFlags;
    u32 destinationBlock;
    u8 tileCount;
    s32 copiedTiles;
    u32 mask;
    s32 uploadIndex;
    register s32 nextUploadIndex PIN("r8");

    upload = gHorizontalScrollingTileUploads;
    uploadIndex = 0;

outer_loop:
    tileCount = upload->tileCount;
    nextUploadIndex = uploadIndex + 1;
    nextUpload = upload + 1;
    if (tileCount != 0) {
        vramBase = 0x06000000 + (uploadIndex << 12);
        destinationFlags = upload->destinationFlags;
        ASM_BARRIER(: "+r"(destinationFlags));

        if ((destinationFlags & 0x10) != 0) {
            vramBase = 0x06000800 + (uploadIndex << 12);
        }

        vramBase += (destinationFlags & 0xF) << 2;
        destinationBlock = upload->destinationBlock;
        copiedTiles = 0;

        if (copiedTiles < tileCount) {
            mask = 0xF;
            source = upload->tileData;

        copy_loop:
            destination = (volatile u16*)(((destinationBlock & mask) << 7) + vramBase);
            destination[0] = *source++;
            destination[1] = *source++;
            destination[0x20] = *source++;
            destination[0x21] = *source++;
            copiedTiles += 4;
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done;
            }

            destination = (volatile u16*)(((destinationBlock & mask) << 7) + vramBase);
            destination[0] = *source++;
            destination[1] = *source++;
            destination[0x20] = *source++;
            destination[0x21] = *source++;
            copiedTiles += 4;
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done;
            }

            destination = (volatile u16*)(((destinationBlock & mask) << 7) + vramBase);
            destination[0] = *source++;
            destination[1] = *source++;
            destination[0x20] = *source++;
            destination[0x21] = *source++;
            copiedTiles += 4;
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done;
            }

            destination = (volatile u16*)(((destinationBlock & mask) << 7) + vramBase);
            destination[0] = *source++;
            destination[1] = *source++;
            destination[0x20] = *source++;
            destination[0x21] = *source++;
            copiedTiles += 4;
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done;
            }

            destination = (volatile u16*)(((destinationBlock & mask) << 7) + vramBase);
            destination[0] = *source++;
            destination[1] = *source++;
            destination[0x20] = *source++;
            destination[0x21] = *source++;
            copiedTiles += 4;
            destinationBlock++;
            if (copiedTiles >= tileCount) {
                goto descriptor_done;
            }

            destination = (volatile u16*)(((destinationBlock & mask) << 7) + vramBase);
            destination[0] = *source++;
            destination[1] = *source++;
            destination[0x20] = *source++;
            destination[0x21] = *source++;
            copiedTiles += 4;
            destinationBlock++;
            if (copiedTiles < tileCount) {
                goto copy_loop;
            }
        }
    }

descriptor_done:
    uploadIndex = nextUploadIndex;
    upload = nextUpload;
    if (uploadIndex <= 2) {
        goto outer_loop;
    }
}
void WaitForDma3Transfer(void)
{
    volatile u32* dma;
    register u32 enableMask PIN("r1");
    s32 status;

    dma = (volatile u32*)0x040000D4;
    status = (s32)dma[2];
    enableMask = 0x80;
    enableMask <<= 24;
    if (status < 0) {
        do {
            status = dma[2] & enableMask;
        } while (status != 0);
    }
}

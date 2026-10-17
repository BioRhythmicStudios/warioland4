#include "sprite_ai/togenobi.h"

#include "fixed_point.h"
#include "main.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"



void InitTogenobi(void)
{
    gCurrentSprite.status |= 0x8480;
    gCurrentSprite.warioCollision = 82;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 32;
    gCurrentSprite.hitboxExtentDown = 28;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sTogenobiOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 0;

    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_OUTER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition, gCurrentSprite.xPosition + 192, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_INNER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition, gCurrentSprite.xPosition + 192, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_OUTER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition - 192, gCurrentSprite.xPosition, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_INNER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition - 192, gCurrentSprite.xPosition, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_OUTER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition, gCurrentSprite.xPosition - 192, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_INNER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition, gCurrentSprite.xPosition - 192, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_OUTER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition + 192, gCurrentSprite.xPosition, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_INNER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition + 192, gCurrentSprite.xPosition, 0);
    SpawnPrimarySpriteWithStatus(PSPRITE_TOGENOBI_CENTER_PART, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition, gCurrentSprite.xPosition, 0);
}

void InitTogenobiOuterPart(void)
{
    struct PrimarySpriteData* current;
    register int zero PIN("r5");
    int zeroTimer;
    register u16 status PIN("r0");
    u16 oldStatus;
    struct PrimarySpriteData* sprites;
    register u32 temp PIN("r0");
    u32 index;
    u32 parentY;
    u32 parentX;
    u32 currentY;
    u8* ptr0;
    u8* ptr1;

    current = &gCurrentSprite;
    oldStatus = current->status;
    status = 0x8488;
    zero = 0;
    zeroTimer = 0;
    status |= oldStatus;
    status &= 0xFFFB;
    current->status = status;
    *(volatile u8*)((u8*)current + 30) = zero;
    ptr1 = (u8*)&current->drawDistanceDown;
    ptr0 = (u8*)32;
    *ptr1 = (u32)ptr0;
    ptr1++;
    *ptr1 = (u32)ptr0;
    ptr1++;
    *ptr1 = (u32)ptr0;
    ptr0 = (u8*)&current->hitboxExtentUp;
    ptr1 = (u8*)4;
    *ptr0 = (u32)ptr1;
    ptr0++;
    *ptr0 = (u32)ptr1;
    ptr0++;
    *ptr0 = (u32)ptr1;
    ptr0++;
    *ptr0 = (u32)ptr1;
    *(volatile u8*)((u8*)current + 22) = zero;
    current->animationTimer = zeroTimer;
    current->pose = 16;

    SpriteUtilFindParentSlotWork3(PSPRITE_TOGENOBI);
    sprites = gSpriteData;
    temp = (u32)current;
    temp += 42;
    index = *(u8*)temp;
    temp = sizeof(struct PrimarySpriteData);
    temp *= index;
    temp += (u32)sprites;
    parentY = ((struct PrimarySpriteData *)temp)->yPosition;
    parentX = ((struct PrimarySpriteData *)temp)->xPosition;
    currentY = current->yPosition;

    if (currentY == parentY) {
        GP_SET(current->pOamData, sTogenobiOuterPartHorizontalOam);
        if (current->xPosition > parentX) {
            ptr0 = (u8*)&current->work1;
            *ptr0 = zero;
            ptr0--;
            *ptr0 = zero;
        } else {
            ptr1 = (u8*)&current->work1;
            *ptr1 = 2;
            ptr1--;
            *ptr1 = 128;
        }
    } else {
        GP_SET(current->pOamData, sTogenobiOuterPartVerticalOam);
        if (currentY > parentY) {
            ptr1 = (u8*)&current->work1;
            *ptr1 = 3;
            ptr1--;
            *ptr1 = 64;
        } else {
            ptr1 = (u8*)&current->work1;
            *ptr1 = 1;
            ptr1--;
            *ptr1 = 192;
        }
    }
}

void UpdateTogenobiOuterPartPosition(void)
{
    s32 radius;
    s32 value;
    u32 offset;
    u32 xOffset;
    u32 angle;
    const s16* table;
    const s16* table2;

    gCurrentSprite.work0 = gSpriteData[gCurrentSprite.work3].work0 + (gCurrentSprite.work1 << 6);

    radius = 192;
    table = sSinCosTable;
    value = table[gCurrentSprite.work0];
    if (value < 0) {
        value = -value * radius;
        if (value < 0) {
            value += 255;
        }
        offset = (u32)(value << 8) >> 16;
        gCurrentSprite.yPosition = gSpriteData[gCurrentSprite.work3].yPosition - offset;
    } else {
        value *= radius;
        if (value < 0) {
            value += 255;
        }
        offset = (u32)(value << 8) >> 16;
        gCurrentSprite.yPosition = offset + gSpriteData[gCurrentSprite.work3].yPosition;
    }

    angle = gCurrentSprite.work0;
    angle += ANGLE_HALF_PI;
    value = sSinCosTable[angle];
    if (value < 0) {
        value = -value * radius;
        if (value < 0) {
            value += 255;
        }
        xOffset = (u32)(value << 8) >> 16;
        gCurrentSprite.xPosition = gSpriteData[gCurrentSprite.work3].xPosition - xOffset;
    } else {
        value *= radius;
        if (value < 0) {
            value += 255;
        }
        xOffset = (u32)(value << 8) >> 16;
        gCurrentSprite.xPosition = xOffset + gSpriteData[gCurrentSprite.work3].xPosition;
    }
}

void FindTogenobiPartSlot(u8 id)
{
    u8 spriteId;
    u8 roomSlot;
    u16 yPosition;
    u16 xPosition;
    int i;
    struct PrimarySpriteData* current;
    struct PrimarySpriteData* sprites;
    struct PrimarySpriteData* spritesTemp;
    struct PrimarySpriteData* currentTemp;
    int offset;
    u8* base;
    struct PrimarySpriteData* sprite;

    spriteId = id;
    currentTemp = &gCurrentSprite;
    roomSlot = currentTemp->roomSlot;
    yPosition = currentTemp->yPosition;
    xPosition = currentTemp->xPosition;
    i = 0;
    current = currentTemp;
    spritesTemp = gSpriteData;
    sprites = spritesTemp;

    while (1) {
        offset = sizeof(struct PrimarySpriteData);
        offset *= i;
        base = (u8*)sprites;
        sprite = (struct PrimarySpriteData*)(offset + (u32)base);
        if ((sprite->status & SPRITE_STATUS_EXISTS)
            && sprite->yPosition == yPosition
            && sprite->xPosition == xPosition
            && sprite->globalID == spriteId
            && sprite->roomSlot == roomSlot) {
            break;
        }
        i++;
        if (i > 23) {
            break;
        }
    }

    current->work2 = i;
}

void InitTogenobiInnerPart(void)
{
    struct PrimarySpriteData* current;
    register int zero PIN("r6");
    u32 zeroTimer;
    register u16 status PIN("r0");
    u16 oldStatus;
    u8* work0;
    struct PrimarySpriteData* sprites;
    register u32 temp PIN("r0");
    u32 index;
    u32 parentY;
    u32 parentX;
    u32 currentY;
    u8* work1;
    u32 workValue;
    u8 distance;
    u8 extentUp;
    u8* property;

    current = &gCurrentSprite;
    oldStatus = current->status;
    status = 0x8480;
    zero = 0;
    zeroTimer = 0;
    status |= oldStatus;
    status &= 0xFFFB;
    current->status = status;
    current->warioCollision = 81;
    property = (u8*)current;
    property += 32;
    ASM_BARRIER(: "+r"(property));
    distance = 16;
    *property = distance;
    property++;
    *property = distance;
    property++;
    *property = distance;
    property++;
    extentUp = 32;
    *property = extentUp;
    property++;
    *property = 28;
    property++;
    *property = extentUp;
    property++;
    *(volatile u8 *)property = 28;
    GP_SET(current->pOamData, sTogenobiInnerPartOam);
    *(volatile u8*)((u8*)current + 22) = zero;
    current->animationTimer = zeroTimer;
    current->pose = distance;
    work0 = (u8*)&current->work0;
    *work0 = zero;

    FindTogenobiPartSlot(PSPRITE_TOGENOBI_OUTER_PART);
    SpriteUtilFindParentSlotWork3(PSPRITE_TOGENOBI);
    sprites = gSpriteData;
    temp = (u32)current;
    temp += 42;
    index = *(u8*)temp;
    temp = sizeof(struct PrimarySpriteData);
    temp *= index;
    temp += (u32)sprites;
    parentY = ((struct PrimarySpriteData *)temp)->yPosition;
    parentX = ((struct PrimarySpriteData *)temp)->xPosition;
    currentY = current->yPosition;

    if (currentY == parentY) {
        if (current->xPosition > parentX) {
            work1 = (u8*)&current->work1;
            *work1 = zero;
            *work0 = zero;
        } else {
            work1 = (u8*)&current->work1;
            workValue = 2;
            *work1 = workValue;
            temp = 128;
            *work0 = temp;
        }
    } else if (currentY > parentY) {
        work1 = (u8*)&current->work1;
        workValue = 3;
        *work1 = workValue;
        temp = 192;
        *work0 = temp;
    } else {
        work1 = (u8*)&current->work1;
        workValue = 1;
        *work1 = workValue;
        temp = 64;
        *work0 = temp;
    }
}

void UpdateTogenobiInnerPartPosition(void)
{
    register struct PrimarySpriteData* current PIN("r12");
    register s32 value PIN("r0");
    s32 reg1;
    s32 reg2;
    s32 reg3;
    s32 preservedRadius;
    u32 dataBase;
    const s16* tableBase;

    current = &gCurrentSprite;
    reg2 = (u32)gSpriteData;
    value = (u32)current;
    value += 42;
    value = *(u8*)value;
    ASM_BARRIER(: "+r"(value));
    reg3 = sizeof(struct PrimarySpriteData);
    reg1 = value;
    reg1 *= reg3;
    reg1 += reg2;
    reg1 += 39;
    value = (u32)current;
    value += 40;
    value = *(u8*)value;
    value <<= 6;
    reg1 = *(u8*)reg1;
    value += reg1;
    reg1 = (u32)current;
    reg1 += 39;
    *(u8*)reg1 = value;

    value = (u32)current;
    value += 41;
    value = *(u8*)value;
    value *= reg3;
    value += reg2;
    value = ((struct PrimarySpriteData *)value)->animationTimer;
    value--;
    dataBase = reg2;

    switch (value) {
        case 0:
        case 9:
            reg2 = 224;
            break;
        case 1:
        case 8:
            reg2 = 128;
            reg2 <<= 1;
            break;
        case 2:
        case 7:
            reg2 = 144;
            reg2 <<= 1;
            break;
        case 3:
        case 6:
            reg2 = 160;
            reg2 <<= 1;
            break;
        case 4:
        case 5:
            reg2 = 176;
            reg2 <<= 1;
            break;
        default:
            reg2 = 192;
            break;
    }

    reg1 = (u32)sSinCosTable;
    value = (u32)current;
    value += 39;
    value = *(u8*)value;
    value <<= 1;
    value += reg1;
    reg3 = 0;
    value = *(s16 *)(value + reg3);
    tableBase = (const s16*)reg1;

    if (value < 0) {
        value = -value;
        value *= reg2;
        preservedRadius = reg2 << 16;
        if (value < 0) {
            value += 255;
        }
        value <<= 8;
        reg2 = (u32)value >> 16;
        value = (u32)current;
        value += 42;
        reg1 = *(u8*)value;
        value = sizeof(struct PrimarySpriteData);
        value *= reg1;
        value += dataBase;
        value = ((struct PrimarySpriteData *)value)->yPosition;
        value -= reg2;
        reg1 = (u32)current;
        ((struct PrimarySpriteData *)reg1)->yPosition = value;
    } else {
        value *= reg2;
        preservedRadius = reg2 << 16;
        if (value < 0) {
            value += 255;
        }
        reg1 = value << 8;
        value = (u32)current;
        value += 42;
        reg2 = *(u8*)value;
        value = sizeof(struct PrimarySpriteData);
        value *= reg2;
        value += dataBase;
        reg1 = (u32)reg1 >> 16;
        value = ((struct PrimarySpriteData *)value)->yPosition;
        reg1 += value;
        reg2 = (u32)current;
        ((struct PrimarySpriteData *)reg2)->yPosition = reg1;
    }

    reg3 = (u32)current;
    value = reg3;
    value += 39;
    value = *(u8*)value;
    value += 64;
    value <<= 1;
    value += (u32)tableBase;
    reg2 = 0;
    reg1 = *(s16 *)(value + reg2);

    if (reg1 < 0) {
        reg1 = -reg1;
        value = preservedRadius >> 16;
        value *= reg1;
        if (value < 0) {
            value += 255;
        }
        value <<= 8;
        reg2 = (u32)value >> 16;
        value = reg3;
        value += 42;
        reg1 = *(u8*)value;
        value = sizeof(struct PrimarySpriteData);
        value *= reg1;
        value += dataBase;
        value = ((struct PrimarySpriteData *)value)->xPosition;
        value -= reg2;
        ((struct PrimarySpriteData *)reg3)->xPosition = value;
    } else {
        value = preservedRadius >> 16;
        value *= reg1;
        if (value < 0) {
            value += 255;
        }
        reg1 = value << 8;
        value = (u32)current;
        value += 42;
        reg2 = *(u8*)value;
        value = sizeof(struct PrimarySpriteData);
        value *= reg2;
        value += dataBase;
        reg1 = (u32)reg1 >> 16;
        value = ((struct PrimarySpriteData *)value)->xPosition;
        reg1 += value;
        reg3 = (u32)current;
        ((struct PrimarySpriteData *)reg3)->xPosition = reg1;
    }
}

void InitTogenobiCenterPart(void)
{
    register struct PrimarySpriteData* sprite PIN("r12");
    register u16 status PIN("r0");
    u16 oldStatus;
    int zero;
    register int zeroTimer PIN("r3");
    u8* ptr;
    u8* hitbox;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = 0x8408;
    zero = 0;
    zeroTimer = 0;
    status |= oldStatus;
    status &= 0xFFFB;
    ptr = (u8*)sprite;
    *(u16*)ptr = status;
    ((struct PrimarySpriteData *)ptr)->warioCollision = zero;
    ptr += 32;
    *ptr = 40;
    ptr++;
    *ptr = 40;
    ptr++;
    *(volatile u8 *)ptr = 40;
    hitbox = (u8*)&sprite->hitboxExtentUp;
    *hitbox = 4;
    hitbox++;
    *hitbox = 4;
    hitbox++;
    *hitbox = 4;
    hitbox++;
    *(volatile u8 *)hitbox = 4;
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zeroTimer;
    {
        const struct AnimationFrame* oam;
        struct PrimarySpriteData* destination;
        oam = sTogenobiCenterPartOam;
        destination = sprite;
        GP_SET(destination->pOamData, oam);
        destination->pose = 16;
    }
}

void SpriteTogenobi(void)
{
    register s32 value PIN("r4");
    s32 scale;

    if (gCurrentSprite.pose == 0) {
        InitTogenobi();
    } else {
        if ((gMainTimer & 3) == 0) {
            gCurrentSprite.work0--;
        }
        if ((gMainTimer & 63) == 0) {
            m4aSongNumStart(SE_TOGENOBI_SPIN);
        }
    }

    value = sSinCosTable[gCurrentSprite.work0 + 64];
    scale = 256;
    gCurrentSprite.affinePA = FixedMul(value, FixedInverse(scale));
    value = sSinCosTable[gCurrentSprite.work0];
    gCurrentSprite.affinePB = FixedMul(value, FixedInverse(scale));
    value = -sSinCosTable[gCurrentSprite.work0];
    gCurrentSprite.affinePC = FixedMul(value, FixedInverse(scale));
    value = sSinCosTable[gCurrentSprite.work0 + 64];
    gCurrentSprite.affinePD = FixedMul(value, FixedInverse(scale));
}

void SpriteTogenobiOuterPart(void)
{
    register s32 value PIN("r4");
    s32 scale;

    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0) {
        InitTogenobiOuterPart();
    } else {
        UpdateTogenobiOuterPartPosition();
    }

    value = sSinCosTable[gCurrentSprite.work0 + 64];
    scale = 256;
    gCurrentSprite.affinePA = FixedMul(value, FixedInverse(scale));
    value = sSinCosTable[gCurrentSprite.work0];
    gCurrentSprite.affinePB = FixedMul(value, FixedInverse(scale));
    value = -sSinCosTable[gCurrentSprite.work0];
    gCurrentSprite.affinePC = FixedMul(value, FixedInverse(scale));
    value = sSinCosTable[gCurrentSprite.work0 + 64];
    gCurrentSprite.affinePD = FixedMul(value, FixedInverse(scale));
}

void SpriteTogenobiInnerPart(void)
{
    register s32 value PIN("r4");
    s32 scale;

    if (gCurrentSprite.pose == 0) {
        InitTogenobiInnerPart();
    } else {
        UpdateTogenobiInnerPartPosition();
    }

    value = sSinCosTable[gCurrentSprite.work0 + 64];
    scale = 256;
    gCurrentSprite.affinePA = FixedMul(value, FixedInverse(scale));
    value = sSinCosTable[gCurrentSprite.work0];
    gCurrentSprite.affinePB = FixedMul(value, FixedInverse(scale));
    value = -sSinCosTable[gCurrentSprite.work0];
    gCurrentSprite.affinePC = FixedMul(value, FixedInverse(scale));
    value = sSinCosTable[gCurrentSprite.work0 + 64];
    gCurrentSprite.affinePD = FixedMul(value, FixedInverse(scale));
}

void SpriteTogenobiCenterPart(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0) {
        InitTogenobiCenterPart();
    }
}

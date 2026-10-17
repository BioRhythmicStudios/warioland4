#include "sprite_ai/icicle.h"

#include "global_data.h"
#include "sound.h"
#include "sprite_util.h"
#include "wario.h"

void InitIcicle(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprite;
    u16 oldStatus;
    register u16 status PIN("r0");
    register int zeroByte PIN("r2");
    int zeroHalf;
    u8 *bounds;
    u8 *rightExtent;
    int extentValue;

    current = &gCurrentSprite;
    oldStatus = current->status;
    status = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    sprite = current;
    sprite->status = status;
    bounds = (u8 *)current + 32;
    *bounds = 24;
    bounds++;
    *bounds = 24;
    bounds++;
    *bounds = 24;
    bounds++;
    *bounds = 48;
    bounds++;
    *bounds = 140;
    bounds++;
    *bounds = 20;
    rightExtent = (u8 *)current + 38;
    extentValue = 16;
    *rightExtent = extentValue;
    GP_SET(sprite->pOamData, sIcicleIdleOam);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    current->work0 = 62;
    sprite->pose = extentValue;
    sprite->warioCollision = 8;
    sprite->drawPriority = 1;
    m4aSongNumStart(SE_ICICLE_WARNING);
}

void IcicleWaitToFall(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    register int oldTimer PIN("r0");
    register int nextTimer PIN("r1");
    s32 timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    oldTimer = *timerPointer;
    nextTimer = oldTimer - 1;
    *timerPointer = nextTimer;
    oldTimer = nextTimer << 24;
    timer = (u32)oldTimer >> 24;
    if (timer != 0) {
        u16 compareTimer;
        compareTimer = timer;
        if (compareTimer > 32) {
            sprite->disableWarioCollisionTimer = 1;
            if (compareTimer == 52) {
                SpriteSpawnAsChild(PSPRITE_ICICLE_WARNING, sprite->roomSlot, sprite->gfxSlot,
                                   sprite->yPosition, sprite->xPosition);
            }
        }
    } else {
        int zero;
        u8 *work3Pointer;
        GP_SET(sprite->pOamData, sIcicleFallingOam);
        sprite->currentAnimationFrame = timer;
        zero = 0;
        sprite->animationTimer = timer;
        sprite->pose = 28;
        work3Pointer = &sprite->work3;
        *work3Pointer = zero;
        m4aSongNumStart(SE_FALLING);
    }
}

void IcicleFall(void)
{
    struct PrimarySpriteData *sprite;
    u32 yPosition;

    sprite = &gCurrentSprite;
    yPosition = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        sprite->status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult != 0) {
        sprite->yPosition = yPosition;
        sprite->pose = 23;
    } else {
        u8 *indexPointer;
        u8 index;
        const s16 *velocities;
        const s16 *entry;
        s32 velocityRaw;
        s32 velocity;
        u32 tempR6;

        tempR6 = 42;
        tempR6 = (u32)(tempR6 + (u8 *)sprite);
        indexPointer = (u8 *)tempR6;
        index = *(u8 *)tempR6;
        velocities = sSpriteGravityVelocityTable;
        entry = (const s16 *)((index << 1) + (u32)velocities);
        velocityRaw = *(const u16 *)entry;
        tempR6 = 0;
        velocity = *(s16 *)((u8 *)entry + tempR6);
        if (velocity == 0x7FFF) {
            u32 previousOffset;
            u16 currentY;
            s32 previousVelocity;

            previousOffset = index - 1;
            previousOffset <<= 1;
            previousOffset += (u32)velocities;
            currentY = *(volatile u16 *)&sprite->yPosition;
            previousVelocity = *(const u16 *)previousOffset;
            currentY += previousVelocity;
            sprite->yPosition = currentY;
        } else {
            u8 nextIndex;
            u8 *pointer;
            nextIndex = index + 1;
            pointer = indexPointer;
            *pointer = nextIndex;
            sprite->yPosition += velocityRaw;
        }
    }
}

void IcicleLandInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sIcicleImpactOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 10;
    gCurrentSprite.pose = 24;
}

void IcicleLandWait(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 110;
    }
}

void IcicleResetAboveRoom(void)
{
    u8 (*roomData)[3];
    struct PrimarySpriteData *sprite;

    roomData = gRoomSpriteData;
    sprite = &gCurrentSprite;
    sprite->yPosition = roomData[sprite->roomSlot][0] * 64 + 64;
    sprite->pose = 111;
    GP_SET(sprite->pOamData, sIcicleIdleOam);
    sprite->work0 = 100;
    sprite->status |= SPRITE_STATUS_HIDDEN;
}

void IcicleWaitHidden(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    if (gCurrentSprite.work0 != 0) {
        gCurrentSprite.work0--;
    } else if (gWarioData.reaction == 0) {
        gCurrentSprite.work0 = 62;
        gCurrentSprite.pose = 16;
        gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
        m4aSongNumStart(SE_ICICLE_WARNING);
    }
}

void InitIcicleWarning(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprite;
    u16 oldStatus;
    register u16 status PIN("r0");
    register int zeroByte PIN("r2");
    int zeroHalf;
    u8 *bounds;
    u8 *hitbox;
    int one;

    current = &gCurrentSprite;
    oldStatus = current->status;
    status = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    sprite = current;
    sprite->status = status;
    bounds = (u8 *)current + 32;
    *bounds = 24;
    bounds++;
    *bounds = 24;
    bounds++;
    *bounds = 24;
    hitbox = (u8 *)current + 35;
    one = 1;
    *hitbox = one;
    hitbox++;
    *hitbox = one;
    hitbox++;
    *hitbox = one;
    hitbox++;
    *hitbox = one;
    GP_SET(sprite->pOamData, sIcicleWarningOam);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    current->work0 = 32;
    sprite->pose = 16;
    sprite->warioCollision = zeroByte;
    sprite->drawPriority = one;
}

void IcicleWarningDisappear(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = 0;
    }
}

void SpriteIcicle(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = 107;
    }
    switch (gCurrentSprite.pose) {
        case 0:
            InitIcicle();
            break;
        case 16:
            IcicleWaitToFall();
            break;
        case 28:
            IcicleFall();
            break;
        case 23:
            IcicleLandInit();
        case 24:
            IcicleLandWait();
            break;
        case 110:
            IcicleResetAboveRoom();
        case 111:
            IcicleWaitHidden();
            break;
        default:
            gCurrentSprite.disableWarioCollisionTimer = 1;
            IcicleLandInit();
            break;
    }
}

void SpriteIcicleWarning(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitIcicleWarning();
            break;
        case 16:
            IcicleWarningDisappear();
            break;
    }
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

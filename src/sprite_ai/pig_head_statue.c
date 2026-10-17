#include "sprite_ai/pig_head_statue.h"

#include "global_data.h"
#include "sound.h"
#include "sprite_util.h"


void InitPigHeadStatue(void)
{
    struct PrimarySpriteData* sprite;
    u8 zero;
    u16 zero16;

    sprite = &gCurrentSprite;
    sprite->status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    zero = 0;
    zero16 = 0;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = zero;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 1;
    sprite->hitboxExtentDown = 1;
    sprite->hitboxExtentLeft = 1;
    sprite->hitboxExtentRight = 1;
    GP_SET(sprite->pOamData, sPigHeadStatueIdleOam);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero16;
    sprite->warioCollision = zero;
    sprite->work0 = 30;
    sprite->pose = SPOSE_IDLE;
}

void PigHeadStatueWaitToOpen(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    u8 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    zero = --*timer;
    if (zero == 0) {
        GP_SET(sprite->pOamData, sPigHeadStatueOpeningOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
        *timer = 63;
        sprite->pose = SPOSE_12;
    }
}

void PigHeadStatueWaitToFire(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    u8 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    zero = --*timer;
    if (zero == 0) {
        GP_SET(sprite->pOamData, sPigHeadStatueFiringOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
        *timer = 8;
        sprite->pose = SPOSE_14;
    }
}

void PigHeadStatueFireProjectile(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    if (--*timer == 0) {
        SpriteSpawnAsChild(PSPRITE_PIG_HEAD_PROJECTILE, sprite->roomSlot, sprite->gfxSlot, sprite->yPosition, sprite->xPosition);
        if (sprite->status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PIG_HEAD_STATUE_FIRE);
        }
        sprite->pose = SPOSE_16;
        *timer = 60;
    }
}

void PigHeadStatueStartCooldown(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    u8 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    zero = --*timer;
    if (zero == 0) {
        GP_SET(sprite->pOamData, sPigHeadStatueIdleOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
        *timer = 160;
        sprite->pose = SPOSE_18;
    }
}

void PigHeadStatueCooldown(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    if (--*timer == 0) {
        *timer = 30;
        sprite->pose = SPOSE_IDLE;
    }
}

void InitPigHeadProjectile(void)
{
    struct PrimarySpriteData* sprite;
    u8 zero;
    u16 zero16;
    register u16 status PIN("r0");
    u16 oldStatus;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    {
        register u8 zeroTemp PIN("r2");
        zeroTemp = 0;
        zero = zeroTemp;
        zero16 = 0;
    }
    status |= oldStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    sprite->status = status;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = zero;
    sprite->drawDistanceLeftRight = 24;
    sprite->hitboxExtentUp = 16;
    sprite->hitboxExtentDown = 16;
    sprite->hitboxExtentLeft = 48;
    sprite->hitboxExtentRight = 44;
    GP_SET(sprite->pOamData, sPigHeadStatueChildFallingOam);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero16;
    sprite->warioCollision = 14;
    sprite->pose = SPOSE_12;
    sprite->work3 = zero;
    sprite->drawPriority = 1;
}

void PigHeadProjectileFall(void)
{
    struct PrimarySpriteData* sprite;
    u32 yPosition;
    u32 work0;
    u8* timerPointer;

    sprite = &gCurrentSprite;
    yPosition = func_8023A60(sprite->yPosition + 32, sprite->xPosition);
    work0 = gBackgroundCollisionData.waterType;
    if (work0 == 1) {
        timerPointer = &sprite->work0;
        goto landed;
    }

    if (gSpriteCollisionResult) {
        register u32 zero PIN("r1");
        u32 zero16;

        zero = 0;
        zero16 = 0;
        sprite->yPosition = yPosition;
        sprite->hitboxExtentUp = 96;
        sprite->hitboxExtentDown = zero;
        GP_SET(sprite->pOamData, sPigHeadStatueChildLandedOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero16;
        timerPointer = &sprite->work0;
        work0 = 126;
landed:
        *timerPointer = work0;
        sprite->pose = SPOSE_14;
    } else {
        u8* work3Pointer;
        u32 timer;
        s16 yVelocity;

        work3Pointer = &sprite->work3;
        timer = *work3Pointer;
        yVelocity = sSpriteGravityVelocityTable[timer];
        if (yVelocity == S16_MAX) {
            u32 previousTimer;
            const s16* previousEntry;
            u16 oldY;
            s16 previousVelocity;
            previousTimer = timer - 1;
            previousEntry = &sSpriteGravityVelocityTable[previousTimer];
            oldY = *(volatile u16 *)&sprite->yPosition;
            previousVelocity = *previousEntry;
            oldY += previousVelocity;
            sprite->yPosition = oldY;
        } else {
            u32 nextTimer;
            u8* storePointer;

            nextTimer = timer + 1;
            storePointer = work3Pointer;
            *storePointer = nextTimer;
            sprite->yPosition += yVelocity;
        }
    }
}

void PigHeadProjectileStartDespawn(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    u8 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    zero = --*timer;
    if (zero == 0) {
        GP_SET(sprite->pOamData, sPigHeadStatueChildDisappearingOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
        *timer = 33;
        sprite->pose = SPOSE_16;
    }
}

void PigHeadProjectileDespawn(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    register int value PIN("r1");
    register u8 oldValue PIN("r0");

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    oldValue = *timer;
    value = oldValue - 1;
    *timer = value;
    if ((u8)value != 0) {
        register u32 check PIN("r0");

        check = (u8)value;
        if (check <= 14) {
            sprite->disableWarioCollisionTimer = 1;
        }
    } else {
        sprite->status = 0;
    }
}

void SpritePigHeadStatue(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitPigHeadStatue();
            break;
        case SPOSE_IDLE:
            PigHeadStatueWaitToOpen();
            break;
        case SPOSE_12:
            PigHeadStatueWaitToFire();
            break;
        case SPOSE_14:
            PigHeadStatueFireProjectile();
            break;
        case SPOSE_16:
            PigHeadStatueStartCooldown();
            break;
        case SPOSE_18:
            PigHeadStatueCooldown();
            break;
    }
}

void SpritePigHeadProjectile(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitPigHeadProjectile();
            break;
        case SPOSE_12:
            PigHeadProjectileFall();
            break;
        case SPOSE_14:
            PigHeadProjectileStartDespawn();
            break;
        case SPOSE_16:
            PigHeadProjectileDespawn();
            break;
    }
}

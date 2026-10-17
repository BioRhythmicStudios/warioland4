#include "sprite_ai/electric_lift.h"

#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"


void InitElectricLiftStationary(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    sprite->yPosition -= 32;
    sprite->warioCollision = 29;
    sprite->drawDistanceDown = 16;
    sprite->drawDistanceUp = 16;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 36;
    sprite->hitboxExtentDown = 32;
    sprite->hitboxExtentLeft = 40;
    sprite->hitboxExtentRight = 36;
    GP_SET(sprite->pOamData, sElectricLiftStationaryIdleOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
    sprite->work1 = 0;
}

void ElectricLiftStationaryIdle(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_WARIO_STANDING_ON) {
        GP_SET(sprite->pOamData, sElectricLiftStationaryActiveOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->work0 = 81;
        sprite->pose = 18;
    }
}

void ElectricLiftStationaryActivated(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    --sprite->work0;
    if (sprite->work0 == 37)
        m4aSongNumStart(SE_ELECTRIC_LIFT_ACTIVATE);
    timer = sprite->work0;
    if (timer <= 9) {
        sprite->disableWarioCollisionTimer = 1;
        if (timer == 0) {
            GP_SET(sprite->pOamData, sElectricLiftStationaryTransitionOam);
            sprite->currentAnimationFrame = 0;
            sprite->animationTimer = 0;
            sprite->work0 = 60;
            sprite->pose = 20;
        }
    }
}

void ElectricLiftStationaryTransition(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = --sprite->work0;
    if (timer == 0) {
        GP_SET(sprite->pOamData, sElectricLiftStationaryResetOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->work0 = 70;
        sprite->pose = 22;
    }
}

void ElectricLiftStationaryReset(void)
{
    struct PrimarySpriteData* sprite;
    u8* work0;
    u32 value;
    u32 timer;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0 - 1;
    *work0 = value;
    timer = (u8)value;
    if (timer > 5) {
        sprite->disableWarioCollisionTimer = 1;
    } else if (timer == 0) {
        GP_SET(sprite->pOamData, sElectricLiftStationaryIdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 16;
    }
}

void SpriteElectricLiftStationary(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitElectricLiftStationary();
            break;
        case 16:
            ElectricLiftStationaryIdle();
            break;
        case 18:
            ElectricLiftStationaryActivated();
            break;
        case 20:
            ElectricLiftStationaryTransition();
            break;
        case 22:
            ElectricLiftStationaryReset();
            break;
        default:
            gCurrentSprite.pose = 0;
            break;
    }
    func_8026838();
}

void InitElectricLiftMoving(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    sprite->yPosition -= 32;
    sprite->warioCollision = 29;
    sprite->drawDistanceDown = 16;
    sprite->drawDistanceUp = 16;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 32;
    sprite->hitboxExtentDown = 32;
    sprite->hitboxExtentLeft = 36;
    sprite->hitboxExtentRight = 32;
    GP_SET(sprite->pOamData, sElectricLiftMovingOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
    sprite->work1 = 0;
}

void ElectricLiftMoveDown(void)
{
    struct PrimarySpriteData* sprite;
    u8* work0;
    u32 value;
    u32 timer;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0 - 1;
    *work0 = value;
    timer = (u8)value;
    if (timer > 15) {
        sprite->yPosition += 2;
        if (sprite->status & SPRITE_STATUS_WARIO_STANDING_ON)
            gWarioData.yPosition += 2;
    } else if (timer == 0) {
        GP_SET(sprite->pOamData, sElectricLiftMovingTurnOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        *work0 = 144;
        sprite->pose = 18;
    }
}

void ElectricLiftMoveUp(void)
{
    struct PrimarySpriteData* sprite;
    u8* work0;
    u32 value;
    u32 timer;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0 - 1;
    *work0 = value;
    timer = (u8)value;
    if (timer > 15) {
        sprite->yPosition -= 2;
        if (sprite->status & SPRITE_STATUS_WARIO_STANDING_ON)
            gWarioData.yPosition -= 2;
    } else if (timer == 0) {
        GP_SET(sprite->pOamData, sElectricLiftMovingOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        *work0 = 144;
        sprite->pose = 16;
    }
}

void ElectricLiftMoveRight(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer > 15) {
        sprite->xPosition += 2;
        if (sprite->status & SPRITE_STATUS_WARIO_STANDING_ON)
            gWarioData.xPosition += 2;
    } else if (timer == 0) {
        sprite->work0 = 176;
        sprite->pose = 18;
    }
}

void ElectricLiftMoveLeft(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer > 15) {
        sprite->xPosition -= 2;
        if (sprite->status & SPRITE_STATUS_WARIO_STANDING_ON)
            gWarioData.xPosition -= 2;
    } else if (timer == 0) {
        sprite->work0 = 176;
        sprite->pose = 16;
    }
}

void SpriteElectricLiftVertical(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitElectricLiftMoving();
            gCurrentSprite.work0 = 80;
            break;
        case 16:
            ElectricLiftMoveDown();
            break;
        case 18:
            ElectricLiftMoveUp();
            break;
    }
    func_8026838();
}

void SpriteElectricLiftHorizontal(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitElectricLiftMoving();
            gCurrentSprite.work0 = 80;
            break;
        case 16:
            ElectricLiftMoveRight();
            break;
        case 18:
            ElectricLiftMoveLeft();
            break;
    }
    func_8026838();
}

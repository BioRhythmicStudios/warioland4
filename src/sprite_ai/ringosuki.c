#include "sprite_ai/ringosuki.h"

#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

void InitRingosuki(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_HEAVY;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.drawDistanceDown = 48;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.hitboxExtentUp = 80;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 44;
    gCurrentSprite.hitboxExtentRight = 40;
    GP_SET(gCurrentSprite.pOamData, sRingosukiIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work1 = 42;
    SpriteUtilTurnTowardWario();
}

void StartRingosukiIdle(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work1 = 42;
}

void RingosukiIdle(void)
{
    u32 nearby;
    register u8 collision PIN("r2");
    u8 extent;
    register u8 *collisionPointer PIN("r5");
    register u8 *extentPointer;
    u16 xPosition;
    u16 maskedX;
    u16 mask;
    u8 collisionResult;

    nearby = SpriteUtilWaitCheckWarioNearbyLeftRight(256, 256);
    func_80238A4();
    func_8023B88();
    collision = gSpriteCollisionResult;
    if (collision == 0)
    {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
        {
            func_8023BFC(gCurrentSprite.yPosition,
                         gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        }
        else
        {
            func_8023BFC(gCurrentSprite.yPosition,
                         gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (gSpriteCollisionTileType != 0)
            goto move;
        gCurrentSprite.pose = 27;
        return;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
    {
        if (nearby == NS_RIGHT)
        {
            if (gWarioData.reaction == 3)
                gCurrentSprite.pose = 114;
            else
                gCurrentSprite.pose = 110;
            return;
        }
        if (nearby == NS_LEFT)
        {
            gCurrentSprite.pose = 17;
            return;
        }
        if ((collision & 0xF0) == 0)
            goto move;
        xPosition = gCurrentSprite.xPosition;
        maskedX = xPosition & 0x3F;
        extentPointer = &gCurrentSprite.hitboxExtentRight;
        extent = *extentPointer;
        if (maskedX + extent <= 0x3F)
            goto move;
        func_8023BFC(gCurrentSprite.yPosition, xPosition + extent);
        collisionPointer = &gSpriteCollisionTileType;
        if (*collisionPointer == 0)
        {
            gCurrentSprite.pose = 17;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition - 32,
                     gCurrentSprite.xPosition + *extentPointer);
        collisionResult = *collisionPointer;
        if (collisionResult == 17)
        {
            gCurrentSprite.pose = collisionResult;
            return;
        }
    }
    else
    {
        if (nearby == NS_LEFT)
        {
            if (gWarioData.reaction == 3)
                gCurrentSprite.pose = 114;
            else
                gCurrentSprite.pose = 110;
            return;
        }
        if (nearby == NS_RIGHT)
        {
            gCurrentSprite.pose = 17;
            return;
        }
        if ((collision & 0xF0) == 0)
            goto move;
        xPosition = gCurrentSprite.xPosition;
        mask = 0x3F;
        extentPointer = &gCurrentSprite.hitboxExtentLeft;
        extent = *extentPointer;
        maskedX = mask & xPosition;
        if (maskedX >= extent)
            goto move;
        func_8023BFC(gCurrentSprite.yPosition, xPosition - extent);
        collisionPointer = &gSpriteCollisionTileType;
        if (*collisionPointer == 0)
        {
            gCurrentSprite.pose = 17;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition - 32,
                     gCurrentSprite.xPosition - *extentPointer);
        collisionResult = *collisionPointer;
        if (collisionResult == 17)
        {
            gCurrentSprite.pose = collisionResult;
            return;
        }
    }

move:
    func_80263AC();
}

void StartRingosukiTurn(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiTurnStartOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 9;
}

void RingosukiTurn(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0)
    {
        gCurrentSprite.pose = 27;
    }
    else if (--gCurrentSprite.work0 == 0)
    {
        if (gCurrentSprite.pose == 18)
        {
            gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
            GP_SET(gCurrentSprite.pOamData, sRingosukiTurnEndOam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            gCurrentSprite.pose = 20;
            gCurrentSprite.work0 = 7;
        }
        else
        {
            gCurrentSprite.pose = 112;
        }
    }
}

void SetupRingosukiFallingKnockback(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5)
        gCurrentSprite.warioCollision = 5;
}

void StartRingosukiFallingRight(void)
{
    gCurrentSprite.pose = 52;
    SetupRingosukiFallingKnockback();
}

void StartRingosukiFallingLeft(void)
{
    gCurrentSprite.pose = 54;
    SetupRingosukiFallingKnockback();
}

void StartRingosukiThrowApple(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiThrowAppleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work0 = 61;
}

void RingosukiThrowApple(void)
{
    struct PrimarySpriteData *ringosuki;
    register u32 oldTimer PIN("r0");
    register u32 newTimer PIN("r1");
    u32 shifted;
    u8 remainingTimer;
    register u8 *throwTimer;
    u16 facingRight;

    ringosuki = &gCurrentSprite;
    throwTimer = &ringosuki->work0;
    oldTimer = *throwTimer;
    newTimer = oldTimer - 1;
    *throwTimer = newTimer;
    shifted = newTimer << 24;
    if (shifted != 0)
    {
        remainingTimer = shifted >> 24;
        if (remainingTimer == 48)
        {
            facingRight = ringosuki->status & SPRITE_STATUS_FACING_RIGHT;
            if (facingRight != 0)
            {
                SpawnPrimarySpriteWithStatus(PSPRITE_RINGOSUKI_APPLE, ringosuki->roomSlot, ringosuki->gfxSlot,
                             ringosuki->yPosition - 110, ringosuki->xPosition + 92,
                             SPRITE_STATUS_FACING_RIGHT);
            }
            else
            {
                SpawnPrimarySpriteWithStatus(PSPRITE_RINGOSUKI_APPLE, ringosuki->roomSlot, ringosuki->gfxSlot,
                             ringosuki->yPosition - 110, ringosuki->xPosition - 92, facingRight);
            }
            m4aSongNumStart(SE_RINGOSUKI_THROW_APPLE);
        }

        func_80238A4();
        func_8023B88();
        if (gSpriteCollisionResult == 0)
            gCurrentSprite.pose = 27;
    }
    else
    {
        ringosuki->pose = 17;
    }
}

void StartRingosukiTurnHop(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiTurnHopOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 113;
    gCurrentSprite.work0 = 16;
    gCurrentSprite.work3 = 0;
}

void RingosukiTurnHop(void)
{
    u8 velocityIndex;
    s16 yVelocity;
    u16 workValue;

    workValue = gCurrentSprite.work3;
    velocityIndex = workValue;
    yVelocity = sRingosukiTurnHopYVelocity[velocityIndex];
    if (yVelocity == S16_MAX)
    {
        workValue = sRingosukiTurnHopYVelocity[velocityIndex - 1];
        gCurrentSprite.yPosition = gCurrentSprite.yPosition + workValue;
    }
    else
    {
        gCurrentSprite.work3 = velocityIndex + 1;
        gCurrentSprite.yPosition += yVelocity;
    }
    workValue = --gCurrentSprite.work0;
    if (workValue == 0)
    {
        workValue = 15;
        gCurrentSprite.pose = workValue;
    }
}

void StartRingosukiEatApple(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiEatAppleOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 115;
    gCurrentSprite.work0 = 58;
}

void RingosukiEatApple(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0)
        gCurrentSprite.pose = 27;
    if (--gCurrentSprite.work0 == 0)
        gCurrentSprite.pose = 17;
}

void StartRingosukiStunned(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiStunnedStartOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 14;
}

void StartRingosukiRecover(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiRecoverOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 30;
    gCurrentSprite.work3 = 0;
}

void RingosukiRecover(void)
{
    u8 velocityIndex;
    s16 yVelocity;
    u16 workValue;

    workValue = gCurrentSprite.work3;
    velocityIndex = workValue;
    yVelocity = sRingosukiRecoverYVelocity[velocityIndex];
    if (yVelocity == S16_MAX)
    {
        workValue = sRingosukiRecoverYVelocity[velocityIndex - 1];
        gCurrentSprite.yPosition = gCurrentSprite.yPosition + workValue;
    }
    else
    {
        workValue = velocityIndex + 1;
        gCurrentSprite.work3 = workValue;
        gCurrentSprite.yPosition += yVelocity;
    }

    if (gCurrentSprite.work0 <= 17)
    {
        func_80238A4();
        func_8023B88();
        if (gSpriteCollisionResult == 0)
            gCurrentSprite.pose = 27;
        gCurrentSprite.warioCollision = 1;
    }

    workValue = --gCurrentSprite.work0;
    if (workValue == 0)
    {
        if (gCurrentSprite.xPosition > gWarioData.xPosition)
        {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = 17;
            else
                gCurrentSprite.pose = 23;
        }
        else
        {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = 23;
            else
                gCurrentSprite.pose = 17;
        }
    }
}

void StartRingosukiFall(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
}

void StartRingosukiLand(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void SetupRingosukiFallOffscreen(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiFallOffscreenOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 4;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
}

void StartRingosukiFallOffscreenRight(void)
{
    gCurrentSprite.pose = 32;
    SetupRingosukiFallOffscreen();
}

void StartRingosukiFallOffscreenLeft(void)
{
    gCurrentSprite.pose = 34;
    SetupRingosukiFallOffscreen();
}

void SetupRingosukiKnockbackFall(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiFallOffscreenOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 3;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void StartRingosukiKnockbackFallRight(void)
{
    gCurrentSprite.pose = 32;
    SetupRingosukiKnockbackFall();
}

void StartRingosukiKnockbackFallLeft(void)
{
    gCurrentSprite.pose = 34;
    SetupRingosukiKnockbackFall();
}

void SetupRingosukiPushed(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
}

void StartRingosukiPushedRight(void)
{
    gCurrentSprite.pose = 36;
    SetupRingosukiPushed();
}

void StartRingosukiPushedLeft(void)
{
    gCurrentSprite.pose = 38;
    SetupRingosukiPushed();
}

void SetupRingosukiTumble(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void StartRingosukiTumbleRight(void)
{
    gCurrentSprite.pose = 44;
    SetupRingosukiTumble();
}

void StartRingosukiTumbleLeft(void)
{
    gCurrentSprite.pose = 46;
    SetupRingosukiTumble();
}

void StartRingosukiDefeat(void)
{
    register u8 *workPointer;
    register struct PrimarySpriteData *sprite PIN("r2");
    u8 collision;

    workPointer = (u8 *)&gCurrentSprite;
    collision = ((struct PrimarySpriteData *)workPointer)->warioCollision;
    sprite = (struct PrimarySpriteData *)workPointer;
    if (collision == 5)
    {
        GP_SET(sprite->pOamData, sRingosukiDefeatedHardOam);
    }
    else
    {
        GP_SET(sprite->pOamData, sRingosukiDefeatedOam);
        workPointer = (u8 *)sprite;
    }
    workPointer += 39;
    *workPointer = 9;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 50;
    sprite->status = (sprite->status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                     SPRITE_STATUS_MAYBE_DEAD;
}

void DefeatRingosuki(void)
{
    SpriteUtilDie();
}

void SetupRingosukiBounce(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void StartRingosukiBounceRight(void)
{
    gCurrentSprite.pose = 72;
    SetupRingosukiBounce();
}

void StartRingosukiBounceLeft(void)
{
    gCurrentSprite.pose = 74;
    SetupRingosukiBounce();
}

void StartRingosukiThrown(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiPushedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 15;
    gCurrentSprite.work3 = 0;
}

void RingosukiThrown(void)
{
    register struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    register u8 *indexPointer;
    register u32 index;
    register const s16 *table;
    const s16 *entry;
    s32 rawVelocity;
    s32 velocity;
    register const s16 *previousEntry;
    u16 currentY;
    s32 previousVelocity;
    register u32 newIndex;
    register u8 *storePointer;
    u16 nextPose;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    if (--*timerPointer == 0)
        goto pose_27;

    func_8023BFC(sprite->yPosition - sprite->hitboxExtentUp, sprite->xPosition);
    if (gSpriteCollisionTileType & 0xF)
    {
        nextPose = 29;
        goto set_pose;
    }

    indexPointer = &sprite->work3;
    index = *indexPointer;
    table = sUnk_8352B18;
    entry = (const s16 *)((index << 1) + (u32)table);
    rawVelocity = *(const u16 *)entry;
    velocity = *entry;
    if (velocity == S16_MAX)
    {
        previousEntry = (const s16 *)(((index - 1) << 1) + (u32)table);
        currentY = *(volatile u16 *)&sprite->yPosition;
        previousVelocity = *(const u16 *)previousEntry;
        currentY += previousVelocity;
        sprite->yPosition = currentY;
    }
    else
    {
        newIndex = index + 1;
        storePointer = indexPointer;
        *storePointer = newIndex;
        sprite->yPosition += rawVelocity;
    }
    goto end;

pose_27:
    nextPose = 27;
set_pose:
    sprite->pose = nextPose;
end:
    ;
}

void SetupRingosukiLifted(void)
{
    GP_SET(gCurrentSprite.pOamData, sRingosukiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void StartRingosukiLiftedRight(void)
{
    gCurrentSprite.pose = 82;
    SetupRingosukiLifted();
}

void StartRingosukiLiftedLeft(void)
{
    gCurrentSprite.pose = 84;
    SetupRingosukiLifted();
}

void SetupRingosukiCarried(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sRingosukiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void StartRingosukiCarriedRight(void)
{
    gCurrentSprite.pose = 88;
    SetupRingosukiCarried();
}

void StartRingosukiCarriedLeft(void)
{
    gCurrentSprite.pose = 86;
    SetupRingosukiCarried();
}

void InitRingosukiApple(void)
{
    int collisionType;

    gCurrentSprite.status = gCurrentSprite.status | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status = gCurrentSprite.status & ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 8;
    gCurrentSprite.hitboxExtentUp = 48;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 24;
    gCurrentSprite.hitboxExtentRight = 20;
    GP_SET(gCurrentSprite.pOamData, sRingosukiAppleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    collisionType = 15;
    gCurrentSprite.warioCollision = collisionType;
    gCurrentSprite.drawPriority = 1;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work3 = 0;
}

void RingosukiAppleFly(void)
{
    u8 velocityIndex;
    s16 yVelocity;
    u16 workValue;

    func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1 || (gSpriteCollisionResult & 0xF) != 0)
    {
        gCurrentSprite.pose = 107;
        return;
    }

    if (gCurrentSprite.work3 <= 7)
    {
        func_8023BFC(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition);
        if (gSpriteCollisionTileType & 0xF)
        {
            gCurrentSprite.pose = 107;
            return;
        }
    }

    workValue = gCurrentSprite.work3;
    velocityIndex = workValue;
    yVelocity = sRingosukiAppleYVelocity[velocityIndex];
    if (yVelocity == S16_MAX)
    {
        workValue = sRingosukiAppleYVelocity[velocityIndex - 1];
        gCurrentSprite.yPosition = gCurrentSprite.yPosition + workValue;
    }
    else
    {
        gCurrentSprite.work3 = velocityIndex + 1;
        gCurrentSprite.yPosition += yVelocity;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
    {
        func_8023BFC(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition + 32);
        if (gSpriteCollisionTileType & 0xF)
            gCurrentSprite.pose = 107;
        else
            gCurrentSprite.xPosition += 8;
    }
    else
    {
        func_8023BFC(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition - 32);
        if (gSpriteCollisionTileType & 0xF)
            gCurrentSprite.pose = 107;
        else
            gCurrentSprite.xPosition -= 8;
    }
}

void StartRingosukiAppleFallRight(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.work3 = 0;
}

void RingosukiAppleFallRight(void)
{
    u8 velocityIndex;
    s16 yVelocity;
    u16 workValue;

    workValue = gCurrentSprite.work3;
    velocityIndex = workValue;
    yVelocity = sSpriteFallingOffscreenYVelocity[velocityIndex];
    if (yVelocity == S16_MAX)
    {
        workValue = sSpriteFallingOffscreenYVelocity[velocityIndex - 1];
        gCurrentSprite.yPosition = gCurrentSprite.yPosition + workValue;
    }
    else
    {
        gCurrentSprite.work3 = velocityIndex + 1;
        gCurrentSprite.yPosition += yVelocity;
    }
    gCurrentSprite.xPosition += 8;
}

void StartRingosukiAppleFallLeft(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.work3 = 0;
}

void RingosukiAppleFallLeft(void)
{
    u8 velocityIndex;
    s16 yVelocity;
    u16 workValue;

    workValue = gCurrentSprite.work3;
    velocityIndex = workValue;
    yVelocity = sSpriteFallingOffscreenYVelocity[velocityIndex];
    if (yVelocity == S16_MAX)
    {
        workValue = sSpriteFallingOffscreenYVelocity[velocityIndex - 1];
        gCurrentSprite.yPosition = gCurrentSprite.yPosition + workValue;
    }
    else
    {
        gCurrentSprite.work3 = velocityIndex + 1;
        gCurrentSprite.yPosition += yVelocity;
    }
    gCurrentSprite.xPosition -= 8;
}

void RemoveRingosukiApple(void)
{
    gCurrentSprite.status = 0;
}

void SpriteRingosuki(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER)
        gCurrentSprite.pose = 107;

    switch (gCurrentSprite.pose)
    {
        case 0:
            InitRingosuki();
            break;

        case 15:
            StartRingosukiIdle();
            /* Fall through. */
        case 16:
            RingosukiIdle();
            break;

        case 17:
            StartRingosukiTurn();
            /* Fall through. */
        case 18:
        case 20:
            RingosukiTurn();
            break;

        case 110:
            StartRingosukiThrowApple();
            /* Fall through. */
        case 111:
            RingosukiThrowApple();
            break;

        case 112:
            StartRingosukiTurnHop();
            /* Fall through. */
        case 113:
            RingosukiTurnHop();
            break;

        case 114:
            StartRingosukiEatApple();
            /* Fall through. */
        case 115:
            RingosukiEatApple();
            break;

        case 23:
            StartRingosukiStunned();
            /* Fall through. */
        case 24:
            func_8023C94();
            break;

        case 25:
            StartRingosukiRecover();
            /* Fall through. */
        case 26:
            RingosukiRecover();
            break;

        case 27:
            StartRingosukiFall();
            /* Fall through. */
        case 28:
            func_8023D48();
            break;

        case 29:
            StartRingosukiLand();
            /* Fall through. */
        case 30:
            func_8023EE0();
            break;

        case 31:
        case 79:
            StartRingosukiFallOffscreenRight();
            /* Fall through. */
        case 32:
            SpriteUtilFallOffscreenRight();
            break;

        case 33:
        case 80:
            StartRingosukiFallOffscreenLeft();
            /* Fall through. */
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;

        case 35:
            StartRingosukiPushedRight();
            /* Fall through. */
        case 36:
            SpriteUtilPushedRight();
            break;

        case 37:
            StartRingosukiPushedLeft();
            /* Fall through. */
        case 38:
            SpriteUtilPushedLeft();
            break;

        case 43:
            StartRingosukiTumbleRight();
            /* Fall through. */
        case 44:
            func_8024478();
            break;

        case 45:
            StartRingosukiTumbleLeft();
            /* Fall through. */
        case 46:
            func_802449C();
            break;

        case 47:
            func_80244C0();
            /* Fall through. */
        case 48:
            func_80244E0();
            break;

        case 49:
            StartRingosukiDefeat();
            /* Fall through. */
        case 50:
            SpriteUtilDieAfterDelay();
            break;

        case 51:
            StartRingosukiFallingRight();
            goto state_53;

        case 53:
            StartRingosukiFallingLeft();
            goto state_55;

        case 55:
            func_8024688();
            /* Fall through. */
        case 56:
            func_80246B8();
            break;

        case 57:
            func_802473C();
            goto state_55;

        case 54:
        case 58:
state_55:
            func_802476C();
            break;

        case 59:
            func_80247F0();
            /* Fall through. */
        case 60:
            func_8024820();
            break;

        case 61:
            func_80248A4();
            /* Fall through. */
        case 62:
            func_80248D4();
            break;

        case 63:
            func_8024958();
            goto state_53;

        case 52:
        case 64:
state_53:
            func_8024988();
            break;

        case 65:
            func_8024A0C();
            /* Fall through. */
        case 66:
            func_8024A3C();
            break;

        case 67:
            func_8024AC0();
            goto state_69;

        case 69:
            func_8024BEC();
            goto state_71;

        case 71:
            StartRingosukiBounceRight();
            /* Fall through. */
        case 68:
        case 72:
state_69:
            func_8024AD4();
            break;

        case 73:
            StartRingosukiBounceLeft();
            /* Fall through. */
        case 70:
        case 74:
state_71:
            func_8024C00();
            break;

        case 75:
            StartRingosukiThrown();
            /* Fall through. */
        case 76:
            RingosukiThrown();
            break;

        case 81:
            StartRingosukiLiftedRight();
            break;

        case 82:
            SpriteUtilLiftingSpriteRight();
            break;

        case 83:
            StartRingosukiLiftedLeft();
            break;

        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;

        case 87:
            StartRingosukiCarriedRight();
            break;

        case 88:
            SpriteUtilCarryingSpriteRight();
            break;

        case 85:
            StartRingosukiCarriedLeft();
            break;

        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;

        case 89:
            gCurrentSprite.pose = 90;
            /* Fall through. */
        case 90:
            SpriteUtilThrownLeftSoft();
            break;

        case 93:
            gCurrentSprite.pose = 94;
            /* Fall through. */
        case 94:
            SpriteUtilThrownLeftHard();
            break;

        case 97:
            gCurrentSprite.pose = 98;
            /* Fall through. */
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;

        case 101:
            gCurrentSprite.pose = 102;
            /* Fall through. */
        case 102:
            SpriteUtilThrownUpLeftHard();
            break;

        case 91:
            gCurrentSprite.pose = 92;
            /* Fall through. */
        case 92:
            SpriteUtilThrownRightSoft();
            break;

        case 95:
            gCurrentSprite.pose = 96;
            /* Fall through. */
        case 96:
            SpriteUtilThrownRightHard();
            break;

        case 99:
            gCurrentSprite.pose = 100;
            /* Fall through. */
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;

        case 103:
            gCurrentSprite.pose = 104;
            /* Fall through. */
        case 104:
            SpriteUtilThrownUpRightHard();
            break;

        case 105:
            StartRingosukiKnockbackFallLeft();
            break;

        case 106:
            StartRingosukiKnockbackFallRight();
            break;

        default:
            DefeatRingosuki();
            break;
    }
}

void SpriteRingosukiApple(void)
{
    switch (gCurrentSprite.pose)
    {
        case 0:
            InitRingosukiApple();
            break;

        case 16:
            RingosukiAppleFly();
            break;

        case 31:
        case 35:
        case 51:
            StartRingosukiAppleFallRight();
            /* Fall through. */
        case 52:
            gCurrentSprite.disableWarioCollisionTimer = 1;
            RingosukiAppleFallRight();
            break;

        case 33:
        case 37:
        case 53:
            StartRingosukiAppleFallLeft();
            /* Fall through. */
        case 54:
            gCurrentSprite.disableWarioCollisionTimer = 1;
            RingosukiAppleFallLeft();
            break;

        case 49:
            RemoveRingosukiApple();
            break;

        default:
            SpriteUtilDie();
            break;
    }
}

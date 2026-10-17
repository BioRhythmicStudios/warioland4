#include "sprite_ai/denden.h"

#include "fixed_point.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"

void DendenUpdateHitboxExtents(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
    {
        gCurrentSprite.hitboxExtentLeft = 40;
        gCurrentSprite.hitboxExtentRight = 64;
    }
    else
    {
        gCurrentSprite.hitboxExtentLeft = 68;
        gCurrentSprite.hitboxExtentRight = 36;
    }
}

void DendenInit(void)
{
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.warioCollision = 38;
    gCurrentSprite.drawDistanceDown = 48;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    DendenUpdateHitboxExtents();
    GP_SET(gCurrentSprite.pOamData, sDendenIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 110;
}

void DendenIdleInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 38;
    DendenUpdateHitboxExtents();
    gCurrentSprite.drawPriority = 2;
}

void DendenIdle(void)
{
    u32 nearby;
    register u8 collision PIN("r2");
    u8 extent;
    register u8 *collisionPointer PIN("r5");
    register u8 *extentPointer;
    u16 xPosition;
    u16 maskedX;
    u16 mask;

    nearby = SpriteUtilCheckWarioNearbyLeftRight(80, 320);
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
        if (gSpriteCollisionTileType == 0)
        {
            gCurrentSprite.pose = 27;
            return;
        }
        goto move;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
    {
        if (nearby == NS_RIGHT)
        {
            gCurrentSprite.pose = 112;
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

        func_8023BFC(gCurrentSprite.yPosition, xPosition + 44);
        collisionPointer = &gSpriteCollisionTileType;
        if ((*collisionPointer & 0xF0) == 0)
        {
            gCurrentSprite.pose = 17;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition - 32,
                     gCurrentSprite.xPosition + *extentPointer);
        if ((*collisionPointer & 0xF) != 0)
        {
            gCurrentSprite.pose = 17;
            return;
        }
    }
    else
    {
        if (nearby == NS_LEFT)
        {
            gCurrentSprite.pose = 112;
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

        func_8023BFC(gCurrentSprite.yPosition, xPosition - 48);
        collisionPointer = &gSpriteCollisionTileType;
        if ((*collisionPointer & 0xF0) == 0)
        {
            gCurrentSprite.pose = 17;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition - 32,
                     gCurrentSprite.xPosition - *extentPointer);
        if ((*collisionPointer & 0xF) != 0)
        {
            gCurrentSprite.pose = 17;
            return;
        }
    }

move:
    func_80263AC();
}

void DendenTurnAroundInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenTurnStartOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 11;
}

void DendenTurnAround(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0)
    {
        gCurrentSprite.pose = 27;
    }
    else if (--gCurrentSprite.work0 == 0)
    {
        switch (gCurrentSprite.pose)
        {
            case 18:
                GP_SET(gCurrentSprite.pOamData, sDendenTurnMiddleOam);
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                gCurrentSprite.pose = 20;
                gCurrentSprite.work0 = 9;
                gCurrentSprite.warioCollision = 33;
                break;

            case 20:
                GP_SET(gCurrentSprite.pOamData, sDendenTurnEndOam);
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                gCurrentSprite.pose = 22;
                gCurrentSprite.work0 = 17;
                gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
                DendenUpdateHitboxExtents();
                break;

            default:
                gCurrentSprite.pose = 15;
                break;
        }
    }
}

void DendenRetreatInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenRetreatOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 23;
    gCurrentSprite.warioCollision = 38;
    DendenUpdateHitboxExtents();
}

void DendenRetreat(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenHideOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 36;
}

void DendenFallInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenAirborneOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
}

void DendenUnderwaterFallInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenAirborneOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 1;
}

void DendenFall(void)
{
    u32 floorY;

    floorY = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1)
    {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
        m4aSongNumStart(SE_ENTER_WATER);
    }

    if (gSpriteCollisionResult != 0)
    {
        gCurrentSprite.yPosition = floorY;
        gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
        gCurrentSprite.drawPriority = 2;
        if (gSpriteCollisionResult & 0x10)
            gCurrentSprite.pose = 23;
        else if (gSpriteCollisionResult & 1)
            gCurrentSprite.pose = 53;
        else
            gCurrentSprite.pose = 51;
    }
    else
    {
        SpriteUtilLookupGravity(sDendenFallingYVelocity);
    }
}

void DendenLandingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenLandingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 36;
}

void ConfigureDendenFallOffscreen(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenKnockedAwayOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void BeginDendenFallOffscreenRight(void)
{
    gCurrentSprite.pose = 32;
    ConfigureDendenFallOffscreen();
}

void BeginDendenFallOffscreenLeft(void)
{
    gCurrentSprite.pose = 34;
    ConfigureDendenFallOffscreen();
}

void ConfigureDendenFallOffscreenWithoutStatusChange(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenKnockedAwayOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    gCurrentSprite.work2 = 7;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void BeginDendenFallOffscreenRightWithoutStatusChange(void)
{
    gCurrentSprite.pose = 32;
    ConfigureDendenFallOffscreenWithoutStatusChange();
}

void BeginDendenFallOffscreenLeftWithoutStatusChange(void)
{
    gCurrentSprite.pose = 34;
    ConfigureDendenFallOffscreenWithoutStatusChange();
}

void DendenPushedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void DendenPushedRightInit(void)
{
    gCurrentSprite.pose = 36;
    DendenPushedCommon();
}

void DendenPushedLeftInit(void)
{
    gCurrentSprite.pose = 38;
    DendenPushedCommon();
}

void DendenBumpedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void DendenBumpedRightInit(void)
{
    gCurrentSprite.pose = 40;
    DendenBumpedCommon();
}

void DendenBumpedLeftInit(void)
{
    gCurrentSprite.pose = 42;
    DendenBumpedCommon();
}

void DendenImpactCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenLandingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 36;
    gCurrentSprite.hitboxExtentLeft = 40;
    gCurrentSprite.hitboxExtentRight = 36;
}

void DendenImpactRightInit(void)
{
    gCurrentSprite.pose = 44;
    DendenImpactCommon();
}

void DendenImpactLeftInit(void)
{
    gCurrentSprite.pose = 46;
    DendenImpactCommon();
}

void DendenDeathDelayInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenDeathOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 50;
    gCurrentSprite.work0 = 11;
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
}

void DendenThrownCommon(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.hitboxExtentLeft = 40;
    gCurrentSprite.hitboxExtentRight = 36;
    if (gCurrentSprite.warioCollision != 36)
    {
        gCurrentSprite.warioCollision = 36;
        GP_SET(gCurrentSprite.pOamData, sDendenThrownOam);
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.work2 = 0;
        gCurrentSprite.drawPriority = 2;
    }
    else
    {
        gCurrentSprite.work2 = 8;
    }
}

void DendenThrownRightInit(void)
{
    gCurrentSprite.pose = 52;
    DendenThrownCommon();
}

void DendenThrownLeftInit(void)
{
    gCurrentSprite.pose = 54;
    DendenThrownCommon();
}

void DendenThrownUpCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenLandingOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 36;
    gCurrentSprite.hitboxExtentLeft = 40;
    gCurrentSprite.hitboxExtentRight = 36;
}

void DendenThrownUpRightInit(void)
{
    gCurrentSprite.pose = 72;
    DendenThrownUpCommon();
}

void DendenThrownUpLeftInit(void)
{
    gCurrentSprite.pose = 74;
    DendenThrownUpCommon();
}

void DendenLiftedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
}

void DendenLifted(void)
{
    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 > 0)
    {
        func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp,
                     gCurrentSprite.xPosition);
        if (gSpriteCollisionTileType & 0xF)
        {
            gCurrentSprite.pose = 29;
            return;
        }
        SpriteUtilLookupGravity(sUnk_8352B18);
        return;
    }
    gCurrentSprite.pose = 27;
}

void DendenSpawnShellInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDendenSpawnShellOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 113;
    gCurrentSprite.work0 = 50;
}

void DendenSpawnShell(void)
{
    if (--gCurrentSprite.work0 == 0)
    {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
        {
            SpawnPrimarySpriteWithStatus(PSPRITE_D1, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                         gCurrentSprite.yPosition, gCurrentSprite.xPosition,
                         SPRITE_STATUS_FACING_RIGHT);
        }
        else
        {
            SpawnPrimarySpriteWithStatus(PSPRITE_D1, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                         gCurrentSprite.yPosition, gCurrentSprite.xPosition, 0);
        }
        gCurrentSprite.status = 0;
    }
}

void DendenShellInit(void)
{
    register u16 currentStatus PIN("r0");
    u16 status;
    u8 zeroByte;
    u16 zeroHalfword;
    register u16 facingRight PIN("r0");

    currentStatus = gCurrentSprite.status;
    status = SPRITE_STATUS_HEAVY | SPRITE_STATUS_AFFINE;
    zeroByte = currentStatus ^ currentStatus;
    zeroHalfword = 0;
    status |= currentStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.status = status;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 2;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 80;
    gCurrentSprite.hitboxExtentDown = 140;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sDendenShellOam);
    gCurrentSprite.currentAnimationFrame = zeroByte;
    gCurrentSprite.animationTimer = zeroHalfword;
    gCurrentSprite.warioCollision = 9;
    gCurrentSprite.work1 = zeroByte;
    facingRight = status & SPRITE_STATUS_FACING_RIGHT;
    if (facingRight != 0)
        gCurrentSprite.pose = 51;
    else
        gCurrentSprite.pose = 53;
}

void DendenShellBumpedCommon(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void DendenShellBumpedRightInit(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    DendenShellBumpedCommon();
}

void DendenShellBumpedLeftInit(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    DendenShellBumpedCommon();
}

void DendenShellDie(void)
{
    SpriteUtilDie();
}

void DendenShellDisappear(void)
{
    if (--gCurrentSprite.work0 != 0)
    {
        if (gCurrentSprite.work0 <= 7)
            gCurrentSprite.disableWarioCollisionTimer = 1;
    }
    else
    {
        gCurrentSprite.status = 0;
    }
}

void DendenShellFallCommon(void)
{
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 4;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
}

void DendenShellFallRightInit(void)
{
    gCurrentSprite.pose = 32;
    DendenShellFallCommon();
}

void DendenShellFallLeftInit(void)
{
    gCurrentSprite.pose = 34;
    DendenShellFallCommon();
}

void SpriteDenden(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER)
        gCurrentSprite.pose = 107;

    switch (gCurrentSprite.pose)
    {
        case 0:
            DendenInit();
            break;

        case 15:
            DendenIdleInit();
            /* Fall through. */
        case 16:
            DendenIdle();
            break;

        case 112:
            DendenSpawnShellInit();
            /* Fall through. */
        case 113:
            DendenSpawnShell();
            break;

        case 17:
            DendenTurnAroundInit();
            /* Fall through. */
        case 18:
        case 20:
        case 22:
            DendenTurnAround();
            break;

        case 23:
            DendenRetreatInit();
            /* Fall through. */
        case 24:
            func_8023C94();
            break;

        case 25:
            DendenRetreat();
            /* Fall through. */
        case 26:
            func_8023CD8();
            break;

        case 110:
            DendenUnderwaterFallInit();
            /* Fall through. */
        case 111:
            DendenFall();
            break;

        case 27:
            DendenFallInit();
            /* Fall through. */
        case 28:
            func_8023D48();
            break;

        case 29:
            DendenLandingInit();
            /* Fall through. */
        case 30:
            func_8023EE0();
            break;

        case 31:
        case 79:
            BeginDendenFallOffscreenRight();
            /* Fall through. */
        case 32:
            SpriteUtilFallOffscreenRight();
            break;

        case 33:
        case 80:
            BeginDendenFallOffscreenLeft();
            /* Fall through. */
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;

        case 35:
            if (gCurrentSprite.warioCollision == 36)
            {
                DendenThrownRightInit();
                func_8024988();
                break;
            }
            DendenPushedRightInit();
            /* Fall through. */
        case 36:
            SpriteUtilPushedRight();
            break;

        case 37:
            if (gCurrentSprite.warioCollision == 36)
            {
                DendenThrownLeftInit();
                func_802476C();
                break;
            }
            DendenPushedLeftInit();
            /* Fall through. */
        case 38:
            SpriteUtilPushedLeft();
            break;

        case 39:
            if (gCurrentSprite.warioCollision == 36)
            {
                DendenThrownRightInit();
                func_8024988();
                break;
            }
            DendenBumpedRightInit();
            /* Fall through. */
        case 40:
            SpriteUtilPushedRight();
            break;

        case 41:
            if (gCurrentSprite.warioCollision == 36)
            {
                DendenThrownLeftInit();
                func_802476C();
                break;
            }
            DendenBumpedLeftInit();
            /* Fall through. */
        case 42:
            SpriteUtilPushedLeft();
            break;

        case 43:
            DendenImpactRightInit();
            /* Fall through. */
        case 44:
            func_8024478();
            break;

        case 45:
            DendenImpactLeftInit();
            /* Fall through. */
        case 46:
            func_802449C();
            break;

        case 47:
            GP_SET(gCurrentSprite.pOamData, sDendenLandingOam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            func_80244C0();
            /* Fall through. */
        case 48:
            func_80244E0();
            break;

        case 49:
            DendenDeathDelayInit();
            /* Fall through. */
        case 50:
            SpriteUtilDieAfterDelay();
            break;

        case 51:
            DendenThrownRightInit();
            /* Fall through. */
        case 52:
            func_8024988();
            break;

        case 53:
            DendenThrownLeftInit();
            /* Fall through. */
        case 54:
            func_802476C();
            break;

        case 55:
            func_8024688();
            /* Fall through. */
        case 56:
            func_80246B8();
            break;

        case 57:
            func_802473C();
            /* Fall through. */
        case 58:
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
            /* Fall through. */
        case 64:
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
            /* Fall through. */
        case 68:
            func_8024AD4();
            break;

        case 69:
            func_8024BEC();
            /* Fall through. */
        case 70:
            func_8024C00();
            break;

        case 71:
            DendenThrownUpRightInit();
            /* Fall through. */
        case 72:
            func_8024AD4();
            break;

        case 73:
            DendenThrownUpLeftInit();
            /* Fall through. */
        case 74:
            func_8024C00();
            break;

        case 75:
            if (gStageFrameCounter & 1)
            {
                DendenThrownRightInit();
                func_8024988();
            }
            else
            {
                DendenThrownLeftInit();
                func_802476C();
            }
            break;

        case 105:
            BeginDendenFallOffscreenLeftWithoutStatusChange();
            break;

        case 106:
            BeginDendenFallOffscreenRightWithoutStatusChange();
            break;

        default:
            SpriteUtilDie();
            break;
    }
}

void SpriteDendenShell(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER)
    {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose)
    {
        case SPOSE_INIT:
            DendenShellInit();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            goto fallRightInit;

        case SPOSE_TACKLED_RIGHT:
            goto fallRight;

        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            goto fallLeftInit;

        case SPOSE_TACKLED_LEFT:
            goto fallLeft;

        case SPOSE_2B:
        case SPOSE_2C:
            gCurrentSprite.work2 = 5;
            func_80238E8();
            func_8025E98();
            break;

        case SPOSE_2D:
        case SPOSE_2E:
            gCurrentSprite.work2 = 5;
            func_80238E8();
            func_8025DE8();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED:
            goto disappear;

        case SPOSE_33:
            DendenShellBumpedRightInit();
            /* Fall through. */
        case SPOSE_34:
            func_8024988();
            break;

        case SPOSE_35:
            DendenShellBumpedLeftInit();
            /* Fall through. */
        case SPOSE_36:
            func_802476C();
            break;

        case SPOSE_37:
            func_8024688();
            /* Fall through. */
        case SPOSE_38:
            func_80246B8();
            break;

        case SPOSE_39:
            func_802473C();
            /* Fall through. */
        case SPOSE_3A:
            func_802476C();
            break;

        case SPOSE_3B:
            func_80247F0();
            /* Fall through. */
        case SPOSE_3C:
            func_8024820();
            break;

        case SPOSE_3D:
            func_80248A4();
            /* Fall through. */
        case SPOSE_3E:
            func_80248D4();
            break;

        case SPOSE_3F:
            func_8024958();
            /* Fall through. */
        case SPOSE_40:
            func_8024988();
            break;

        case SPOSE_41:
            func_8024A0C();
            /* Fall through. */
        case SPOSE_42:
            func_8024A3C();
            break;

        case SPOSE_43:
        case SPOSE_47:
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
            else
                func_8024AC0();
            /* Fall through. */
        case SPOSE_44:
            func_8024AD4();
            break;

        case SPOSE_45:
        case SPOSE_49:
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                func_8024BEC();
            else
                gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
            /* Fall through. */
        case SPOSE_46:
            func_8024C00();
            break;

        case SPOSE_6B:
            goto die;

        default:
            goto shellDie;
    }
    goto updateAffine;

fallRightInit:
    DendenShellFallRightInit();
fallRight:
    SpriteUtilFallOffscreenRight();
    goto updateAffine;

fallLeftInit:
    DendenShellFallLeftInit();
fallLeft:
    SpriteUtilFallOffscreenLeft();
    goto updateAffine;

die:
    SpriteUtilDie();
    goto updateAffine;

disappear:
    DendenShellDisappear();
    goto updateAffine;

shellDie:
    DendenShellDie();

updateAffine:
    if (gCurrentSprite.status & SPRITE_STATUS_AFFINE)
    {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
        {
            gCurrentSprite.work1 += 5;
        }
        else
        {
            gCurrentSprite.work1 -= 5;
        }
        gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(256));
        gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(256));
        gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(256));
        gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(256));
    }
}

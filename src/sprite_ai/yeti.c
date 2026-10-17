#include "sprite_ai/yeti.h"

#include "score.h"
#include "sprite.h"
#include "sprite_util.h"

void YetiInit(void)
{
    struct PrimarySpriteData *sprite;
    u32 status;
    register u32 heavy PIN("r3");
    u8 zero8;
    u16 zero16;
    u32 stateOrPriority;

    sprite = &gCurrentSprite;
    status = sprite->status;
    heavy = SPRITE_STATUS_HEAVY;
    zero8 = 0;
    zero16 = 0;
    ASM_BARRIER(: "+r"(sprite), "+r"(status), "+r"(heavy), "+r"(zero8), "+r"(zero16));
    status |= heavy;
    sprite->status = status;
    sprite->warioCollision = 1;
    sprite->drawDistanceDown = 48;
    sprite->drawDistanceUp = zero8;
    sprite->drawDistanceLeftRight = heavy;
    sprite->hitboxExtentUp = 104;
    sprite->hitboxExtentDown = zero8;
    sprite->hitboxExtentLeft = 44;
    sprite->hitboxExtentRight = 40;
    {
        const struct AnimationFrame *oam;

        oam = sYetiIdleOam;
        stateOrPriority = (u32)sprite;
        GP_SET(((struct PrimarySpriteData *)stateOrPriority)->pOamData, oam);
        ((struct PrimarySpriteData *)stateOrPriority)->currentAnimationFrame = zero8;
        ((struct PrimarySpriteData *)stateOrPriority)->animationTimer = zero16;
        ((struct PrimarySpriteData *)stateOrPriority)->pose = SPOSE_IDLE;
        stateOrPriority = ((struct PrimarySpriteData *)stateOrPriority)->drawPriority;
    }
    if (stateOrPriority & 0x80) {
        u32 maskedPriority;

        maskedPriority = 0x7F;
        maskedPriority &= stateOrPriority;
        stateOrPriority = (u32)sprite;
        ((struct PrimarySpriteData *)stateOrPriority)->drawPriority = maskedPriority;
    } else {
        SpriteUtilTurnTowardWario();
    }
}

void YetiIdleInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.work0 = 128;
}

void YetiIdle(void)
{
    u32 nearby;

    nearby = SpriteUtilCheckWarioNearbyLeftRight(80, 960);
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_11;
        return;
    }

    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (!gSpriteCollisionTileType) {
            gCurrentSprite.pose = SPOSE_1B;
            return;
        }
    } else if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        if (nearby == 8) {
            gCurrentSprite.pose = SPOSE_6E;
            return;
        }
        if (nearby == 4) {
            gCurrentSprite.pose = SPOSE_11;
            return;
        }
        if ((gSpriteCollisionResult & 0xF0) &&
            ((gCurrentSprite.xPosition & 0x3F) + gCurrentSprite.hitboxExtentRight > 0x3F)) {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
            if (!(gSpriteCollisionTileType & 0xF0)) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
            func_8023BFC(gCurrentSprite.yPosition - 32,
                gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
            if (gSpriteCollisionTileType & 0xF) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
        }
    } else {
        if (nearby == 4) {
            gCurrentSprite.pose = SPOSE_6E;
            return;
        }
        if (nearby == 8) {
            gCurrentSprite.pose = SPOSE_11;
            return;
        }
        if ((gSpriteCollisionResult & 0xF0) &&
            ((gCurrentSprite.xPosition & 0x3F) < gCurrentSprite.hitboxExtentLeft)) {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (!(gSpriteCollisionTileType & 0xF0)) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
            func_8023BFC(gCurrentSprite.yPosition - 32,
                gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (gSpriteCollisionTileType & 0xF) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
        }
    }
    func_80263AC();
}

void YetiTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 7;
}

void YetiTurn(void)
{
    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        gCurrentSprite.pose = SPOSE_1B;
        return;
    }
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        return;
    }
    if (gCurrentSprite.pose == SPOSE_12) {
        gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
        GP_SET(gCurrentSprite.pOamData, sYetiTurningAroundOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_14;
        gCurrentSprite.work0 = 5;
    } else {
        gCurrentSprite.pose = SPOSE_70;
    }
}

void YetiStunnedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
    }
}

void YetiPose33Init(void)
{
    gCurrentSprite.pose = SPOSE_34;
    YetiStunnedInit();
}

void YetiPose35Init(void)
{
    gCurrentSprite.pose = SPOSE_36;
    YetiStunnedInit();
}

void YetiAttackInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiAttackOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_6F;
    gCurrentSprite.work0 = 64;
}

void YetiAttack(void)
{
    register int shifted PIN("r0");
    register int timer PIN("r1");
    u8 *work0;
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    shifted = *work0;
    timer = shifted - 1;
    *work0 = timer;
    shifted = timer << 24;
    if (shifted != 0) {
        shifted = (u32)shifted >> 24;
        if (shifted == 50) {
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
                SpawnPrimarySpriteWithStatus(PSPRITE_BB, sprite->roomSlot, sprite->gfxSlot,
                    sprite->yPosition - 16, sprite->xPosition + 24,
                    SPRITE_STATUS_FACING_RIGHT);
            } else {
                SpawnPrimarySpriteWithStatus(PSPRITE_BB, sprite->roomSlot, sprite->gfxSlot,
                    sprite->yPosition - 16, sprite->xPosition - 24, 0);
            }
        }
        func_80238A4();
        func_8023B88();
        if (!gSpriteCollisionResult) {
            gCurrentSprite.pose = SPOSE_1B;
        }
    } else {
        sprite->pose = SPOSE_11;
    }
}

void YetiAttackRecoverInit(void)
{
    u8 zero8;
    u16 zero16;

    GP_SET(gCurrentSprite.pOamData, sYetiAttackRecoverOam);
    zero8 = 0;
    zero16 = 0;
    ASM_BARRIER(: "+r"(zero8), "+r"(zero16));
    gCurrentSprite.animationTimer = zero16;
    gCurrentSprite.currentAnimationFrame = zero8;
    gCurrentSprite.pose = SPOSE_71;
    gCurrentSprite.work0 = 40;
}

void YetiAttackRecover(void)
{
    struct PrimarySpriteData *sprite;
    u32 nearby;

    nearby = SpriteUtilCheckWarioNearbyLeftRight(80, 960);
    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        if (nearby == 8) {
            sprite->pose = SPOSE_6E;
            return;
        }
    } else if (nearby == 4) {
        sprite->pose = SPOSE_6E;
        return;
    }

    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        gCurrentSprite.pose = SPOSE_1B;
    }
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_IDLE_INIT;
    }
}

void YetiRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiRecoveringOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 12;
}

void YetiFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
}

void YetiStunnedFallingInit(void)
{
    u8 zero8;

    GP_SET(gCurrentSprite.pOamData, sYetiStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    zero8 = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = zero8;
    gCurrentSprite.warioCollision = 5;
}

void YetiTackledInit(void)
{
    struct PrimarySpriteData *sprite;
    register u8 zero PIN("r4");

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sYetiTackledOrDefeatedOam);
    sprite->currentAnimationFrame = 0;
    zero = 0;
    sprite->animationTimer = 0;
    ASM_BARRIER(: "+r"(zero));
    sprite->health = zero;
    sprite->work2 = 4;
    {
        u8 *work3;

        work3 = &sprite->work3;
        *work3 = zero;
    }
    sprite->drawPriority = zero;
    SpawnEnemyScoreDrop(sprite->globalID, sprite->yPosition, sprite->xPosition, 5);
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    {
        register u16 oldStatus PIN("r1");
        u16 newStatus;
        u16 deadBit;

        oldStatus = sprite->status;
        newStatus = oldStatus & (u16)~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
        deadBit = SPRITE_STATUS_MAYBE_DEAD;
        oldStatus = deadBit;
        newStatus |= oldStatus;
        sprite->status = newStatus;
    }
    gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
}

void YetiTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    YetiTackledInit();
}

void YetiTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    YetiTackledInit();
}

void YetiDefeatedInit(void)
{
    struct PrimarySpriteData *sprite;
    u8 zero;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sYetiTackledOrDefeatedOam);
    sprite->currentAnimationFrame = 0;
    zero = 0;
    sprite->animationTimer = 0;
    sprite->health = zero;
    {
        u8 *work2;
        u8 *work3;

        work2 = &sprite->work2;
        *work2 = 3;
        work3 = &sprite->work3;
        *work3 = zero;
    }
    sprite->drawPriority = zero;
    SpawnEnemyScoreDrop(sprite->globalID, sprite->yPosition, sprite->xPosition, 5);
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
}

void YetiDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    YetiDefeatedInit();
}

void YetiDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    YetiDefeatedInit();
}

void YetiPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 30;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
}

void YetiPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    YetiPushedInit();
}

void YetiPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    YetiPushedInit();
}

void YetiPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void YetiPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    YetiPose2BOr2DInit();
}

void YetiPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    YetiPose2BOr2DInit();
}

void YetiCrushed(void)
{
    struct PrimarySpriteData *sprite;
    register u8 *current PIN("r1");

    {
        u32 collision;

        current = (u8 *)&gCurrentSprite;
        collision = ((struct PrimarySpriteData *)current)->warioCollision;
        sprite = (struct PrimarySpriteData *)current;
        if (collision == 5) {
            GP_SET(sprite->pOamData, sYetiCrushedWhileStunnedOam);
        } else {
            GP_SET(sprite->pOamData, sYetiCrushedOam);
            current = (u8 *)sprite;
        }
    }
    {
        u32 zero;

        current += 39;
        *current = 9;
        zero = 0;
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
    }
    sprite->pose = SPOSE_CRUSHED_OR_COLLECTED;
    {
        register u16 oldStatus PIN("r1");
        u16 newStatus;
        u16 deadBit;

        oldStatus = sprite->status;
        newStatus = oldStatus & (u16)~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
        deadBit = SPRITE_STATUS_MAYBE_DEAD;
        oldStatus = deadBit;
        newStatus |= oldStatus;
        sprite->status = newStatus;
    }
    SpawnEnemyScoreDrop(sprite->globalID, sprite->yPosition, sprite->xPosition, 5);
    gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
}

void YetiInstantlyKill(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    SpawnEnemyScoreDrop(sprite->globalID, sprite->yPosition, sprite->xPosition, 5);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
}

void YetiStunnedWithoutTimerInit(void)
{
    struct PrimarySpriteData *sprite;
    u8 zero8;
    u16 zero16;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sYetiStunnedOam);
    zero8 = 0;
    zero16 = 0;
    ASM_BARRIER(: "+r"(zero8), "+r"(zero16));
    sprite->animationTimer = zero16;
    sprite->currentAnimationFrame = zero8;
    sprite->work2 = 6;
    sprite->work3 = zero8;
    sprite->warioCollision = 5;
}

void YetiPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    YetiStunnedWithoutTimerInit();
}

void YetiPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    YetiStunnedWithoutTimerInit();
}

void YetiAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 15;
    gCurrentSprite.work3 = 0;
}

void YetiAirborne(void)
{
    u8 *indexPointer;
    u32 index;
    s16 velocity;

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp,
            gCurrentSprite.xPosition);
        if (gSpriteCollisionTileType & 0xF) {
            gCurrentSprite.pose = SPOSE_1D;
        } else {
            indexPointer = &gCurrentSprite.work3;
            index = *indexPointer;
            velocity = sUnk_8352B2C[index];
            if (velocity == 0x7FFF) {
                u32 previousVelocityAddress;
                u16 yPosition;

                previousVelocityAddress = index - 1;
                previousVelocityAddress <<= 1;
                previousVelocityAddress += (u32)sUnk_8352B2C;
                yPosition = *(volatile u16 *)&gCurrentSprite.yPosition;
                gCurrentSprite.yPosition = yPosition + *(const s16 *)previousVelocityAddress;
            } else {
                u32 nextIndex;
                u8 *storePointer;

                nextIndex = index + 1;
                storePointer = indexPointer;
                /* Keep the incremented index in r0 and the destination pointer in r1. */
                *storePointer = nextIndex;
                gCurrentSprite.yPosition += velocity;
            }
        }
    } else {
        gCurrentSprite.pose = SPOSE_1B;
        gCurrentSprite.warioCollision = 1;
    }
}

void YetiHeldInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sYetiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void YetiLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
    YetiHeldInit();
}

void YetiLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
    YetiHeldInit();
}

void YetiCarriedInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sYetiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void YetiCarriedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    YetiCarriedInit();
}

void YetiCarriedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    YetiCarriedInit();
}

void YetiSnowballInit(void)
{
    struct PrimarySpriteData *sprite;
    u8 *fieldPointer;
    u8 zero8;
    u16 zero16;

    sprite = &gCurrentSprite;
    {
        u16 originalStatus;
        register u16 status PIN("r0");

        originalStatus = sprite->status;
        status = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
        zero8 = 0;
        zero16 = 0;
        ASM_BARRIER(: "+r"(sprite), "+r"(originalStatus), "+r"(status), "+r"(zero8), "+r"(zero16));
        status |= originalStatus;
        status &= ~SPRITE_STATUS_HIDDEN;
        fieldPointer = (u8 *)sprite;
        *(u16 *)fieldPointer = status;
    }
    {
        u8 *extentPointer;
        u8 extent;

        sprite->drawDistanceDown = 32;
        extentPointer = &sprite->drawDistanceUp;
        extent = 16;
        *extentPointer = extent;
        extentPointer++;
        *extentPointer = extent;
        fieldPointer += 35;
        *fieldPointer = 64;
        extentPointer = &sprite->hitboxExtentDown;
        *extentPointer = extent;
        fieldPointer += 2;
        *fieldPointer = 40;
        fieldPointer++;
        *(volatile u8 *)fieldPointer = 36;
        {
            const struct AnimationFrame *oam;

            oam = sYetiSnowballOam;
            fieldPointer = (u8 *)sprite;
            GP_SET(((struct PrimarySpriteData *)fieldPointer)->pOamData, oam);
        }
        ((struct PrimarySpriteData *)fieldPointer)->currentAnimationFrame = zero8;
        ((struct PrimarySpriteData *)fieldPointer)->animationTimer = zero16;
        ((struct PrimarySpriteData *)fieldPointer)->warioCollision = 20;
        ((struct PrimarySpriteData *)fieldPointer)->pose = extent;
    }
    sprite->work0 = 18;
    ((struct PrimarySpriteData *)fieldPointer)->drawPriority = 1;
}

void YetiSnowballMove(void)
{
    struct PrimarySpriteData *loaded;
    u8 *timerPointer;
    u32 timer;
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *current PIN("r4");

    loaded = &gCurrentSprite;
    timerPointer = &loaded->work0;
    timer = *timerPointer;
    sprite = loaded;
    if (timer != 0) {
        u32 nextTimer;

        nextTimer = timer - 1;
        *timerPointer = nextTimer;
        timer = (u8)nextTimer;
        if (timer == 0) {
            GP_SET(sprite->pOamData, sYetiSnowballMovingOam);
            sprite->currentAnimationFrame = timer;
            sprite->animationTimer = timer;
        }
    }
    current = sprite;
    func_8023BFC(current->yPosition, current->xPosition);
    if (gSpriteCollisionTileType & 0xF) {
        current->pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    }
    if (current->status & SPRITE_STATUS_FACING_RIGHT) {
        current->xPosition += 8;
    } else {
        sprite->xPosition -= 8;
    }
}

void YetiSnowballDisappearInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sYetiSnowballDisappearOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 8;
}

void YetiSnowballDisappear(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = 0;
    }
}

void SpriteYeti(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT: YetiInit(); break;
        case SPOSE_IDLE_INIT: YetiIdleInit();
        case SPOSE_IDLE: YetiIdle(); break;
        case SPOSE_11: YetiTurnInit();
        case SPOSE_12:
        case SPOSE_14: YetiTurn(); break;
        case SPOSE_6E: YetiAttackInit();
        case SPOSE_6F: YetiAttack(); break;
        case SPOSE_70: YetiAttackRecoverInit();
        case SPOSE_71: YetiAttackRecover(); break;
        case SPOSE_17: YetiRecoverInit();
        case SPOSE_18: func_8023C94(); break;
        case SPOSE_1B: YetiFallingInit();
        case SPOSE_1C: func_8023D48(); break;
        case SPOSE_1D: YetiStunnedFallingInit();
        case SPOSE_1E: func_8023EE0(); break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F: YetiTackledRightInit();
        case SPOSE_TACKLED_RIGHT: SpriteUtilFallOffscreenRight(); break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50: YetiTackledLeftInit();
        case SPOSE_TACKLED_LEFT: SpriteUtilFallOffscreenLeft(); break;
        case SPOSE_PUSHED_RIGHT_INIT: YetiPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            func_80238A4();
            if (gCurrentSprite.work2 > 2) func_80242C8();
            else if (--gCurrentSprite.work0 == 0) gCurrentSprite.pose = SPOSE_11;
            break;
        case SPOSE_PUSHED_LEFT_INIT: YetiPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            func_80238A4();
            if (gCurrentSprite.work2 > 2) func_8024180();
            else if (--gCurrentSprite.work0 == 0) gCurrentSprite.pose = SPOSE_11;
            break;
        case SPOSE_2B: YetiPose2BInit();
        case SPOSE_2C: func_8024478(); break;
        case SPOSE_2D: YetiPose2DInit();
        case SPOSE_2E: func_802449C(); break;
        case SPOSE_2F: func_80244C0();
        case SPOSE_30: func_80244E0(); break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT: YetiCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED: SpriteUtilDieAfterDelay(); break;
        case SPOSE_33: YetiPose33Init();
        case SPOSE_34: func_8024988(); break;
        case SPOSE_35: YetiPose35Init();
        case SPOSE_36: func_802476C(); break;
        case SPOSE_37: func_8024688();
        case SPOSE_38: func_80246B8(); break;
        case SPOSE_39: func_802473C();
        case SPOSE_3A: func_802476C(); break;
        case SPOSE_3B: func_80247F0();
        case SPOSE_3C: func_8024820(); break;
        case SPOSE_3D: func_80248A4();
        case SPOSE_3E: func_80248D4(); break;
        case SPOSE_3F: func_8024958();
        case SPOSE_40: func_8024988(); break;
        case SPOSE_41: func_8024A0C();
        case SPOSE_42: func_8024A3C(); break;
        case SPOSE_43: func_8024AC0();
        case SPOSE_44: func_8024AD4(); break;
        case SPOSE_45: func_8024BEC();
        case SPOSE_46: func_8024C00(); break;
        case SPOSE_47: YetiPose47Init();
        case SPOSE_48: func_8024AD4(); break;
        case SPOSE_49: YetiPose49Init();
        case SPOSE_4A: func_8024C00(); break;
        case SPOSE_19:
        case SPOSE_4B: YetiAirborneInit();
        case SPOSE_4C: YetiAirborne(); break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT: YetiLiftedRightInit(); break;
        case SPOSE_BEING_LIFTED_RIGHT: SpriteUtilLiftingSpriteRight(); break;
        case SPOSE_BEING_LIFTED_LEFT_INIT: YetiLiftedLeftInit(); break;
        case SPOSE_BEING_LIFTED_LEFT: SpriteUtilLiftingSpriteLeft(); break;
        case SPOSE_CARRIED_RIGHT_INIT: YetiCarriedRightInit(); break;
        case SPOSE_CARRIED_RIGHT: SpriteUtilCarryingSpriteRight(); break;
        case SPOSE_CARRIED_LEFT_INIT: YetiCarriedLeftInit(); break;
        case SPOSE_CARRIED_LEFT: SpriteUtilCarryingSpriteLeft(); break;
        case SPOSE_THROWN_LEFT_SOFT_INIT: gCurrentSprite.pose = SPOSE_THROWN_LEFT_SOFT;
        case SPOSE_THROWN_LEFT_SOFT: SpriteUtilThrownLeftSoft(); break;
        case SPOSE_THROWN_LEFT_HARD_INIT: gCurrentSprite.pose = SPOSE_THROWN_LEFT_HARD;
        case SPOSE_THROWN_LEFT_HARD: SpriteUtilThrownLeftHard(); break;
        case SPOSE_THROWN_UP_LEFT_SOFT_INIT: gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_SOFT;
        case SPOSE_THROWN_UP_LEFT_SOFT: SpriteUtilThrownUpLeftSoft(); break;
        case SPOSE_THROWN_UP_LEFT_HARD_INIT: gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_HARD;
        case SPOSE_THROWN_UP_LEFT_HARD: SpriteUtilThrownUpLeftHard(); break;
        case SPOSE_THROWN_RIGHT_SOFT_INIT: gCurrentSprite.pose = SPOSE_THROWN_RIGHT_SOFT;
        case SPOSE_THROWN_RIGHT_SOFT: SpriteUtilThrownRightSoft(); break;
        case SPOSE_THROWN_RIGHT_HARD_INIT: gCurrentSprite.pose = SPOSE_THROWN_RIGHT_HARD;
        case SPOSE_THROWN_RIGHT_HARD: SpriteUtilThrownRightHard(); break;
        case SPOSE_THROWN_UP_RIGHT_SOFT_INIT: gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_SOFT;
        case SPOSE_THROWN_UP_RIGHT_SOFT: SpriteUtilThrownUpRightSoft(); break;
        case SPOSE_THROWN_UP_RIGHT_HARD_INIT: gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_HARD;
        case SPOSE_THROWN_UP_RIGHT_HARD: SpriteUtilThrownUpRightHard(); break;
        case SPOSE_69: YetiDefeatedLeftInit(); break;
        case SPOSE_6A: YetiDefeatedRightInit(); break;
        default: YetiInstantlyKill(); break;
    }
}

void SpriteYetiSnowball(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            YetiSnowballInit();
            break;

        case SPOSE_IDLE:
            YetiSnowballMove();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED:
            YetiSnowballDisappear();
            break;

        default:
            YetiSnowballDisappearInit();
            break;
    }
}

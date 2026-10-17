#include "sprite_ai/bow_balloon.h"

#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

void BowBalloonInit(void)
{
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 80;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 40;
    gCurrentSprite.hitboxExtentRight = 36;
    GP_SET(gCurrentSprite.pOamData, sBowBalloonIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    SpriteUtilTurnTowardWario();
}

void BowBalloonIdleInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.work1 = 42;
}

void BowBalloonIdle(void)
{
    u32 nearby;

    nearby = SpriteUtilWaitCheckWarioNearbyLeftRight(256, 256);
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

    func_8026374();
}

void BowBalloonTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 21;
}

void BowBalloonTurn(void)
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
        GP_SET(gCurrentSprite.pOamData, sBowBalloonTurningAroundOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_14;
        gCurrentSprite.work0 = 24;
    } else {
        GP_SET(gCurrentSprite.pOamData, sBowBalloonIdleOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_IDLE;
    }
}

void BowBalloonStunnedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
    }
}

void BowBalloonPose33Init(void)
{
    gCurrentSprite.pose = SPOSE_34;
    BowBalloonStunnedInit();
}

void BowBalloonPose35Init(void)
{
    gCurrentSprite.pose = SPOSE_36;
    BowBalloonStunnedInit();
}

void BowBalloonAttackInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonAttackingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_6F;
    gCurrentSprite.work0 = 79;
}

void BowBalloonAttack(void)
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
        if (shifted == 22) {
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
                SpawnPrimarySpriteWithStatus(PSPRITE_A1, sprite->roomSlot, sprite->gfxSlot,
                    sprite->yPosition - 64, sprite->xPosition + 96,
                    SPRITE_STATUS_FACING_RIGHT);
            } else {
                SpawnPrimarySpriteWithStatus(PSPRITE_A1, sprite->roomSlot, sprite->gfxSlot,
                    sprite->yPosition - 64, sprite->xPosition - 96, 0);
            }
            m4aSongNumStart(SE_BOW_BALLOON_SHOOT);
        }
        func_80238A4();
        func_8023B88();
        if (!gSpriteCollisionResult) {
            gCurrentSprite.pose = SPOSE_1B;
        }
    } else {
        sprite->pose = SPOSE_IDLE_INIT;
    }
}

void BowBalloonRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonRecoveringOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 84;
}

void BowBalloonGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 52;
    gCurrentSprite.work3 = 0;
}

void BowBalloonGetUp(void)
{
    {
    struct PrimarySpriteData *sprite;
    u8 *indexPointer;
    u32 index;
    const s16 *table;
    s16 velocity;

    sprite = &gCurrentSprite;
    indexPointer = &sprite->work3;
    index = *indexPointer;
    table = sBowBalloonGettingUpYVelocity;
    velocity = table[index];
    if (velocity == 0x7FFF) {
        u32 previousVelocityAddress;
        u16 yPosition;

        previousVelocityAddress = index - 1;
        previousVelocityAddress <<= 1;
        previousVelocityAddress += (u32)table;
        yPosition = *(volatile u16 *)&sprite->yPosition;
        sprite->yPosition = yPosition + *(const s16 *)previousVelocityAddress;
    } else {
        u32 nextIndex;
        u8 *storePointer;

        nextIndex = index + 1;
        storePointer = indexPointer;
        *storePointer = nextIndex;
        sprite->yPosition += velocity;
    }
    }

    {
        struct PrimarySpriteData *current;
        u8 work0;

        current = &gCurrentSprite;
        work0 = current->work0;
        if (work0 > 42) {
            func_80238E8();
            func_8023B88();
            if (!gSpriteCollisionResult) {
                current->pose = SPOSE_1D;
            }
        } else if (work0 <= 23) {
            func_80238A4();
            func_8023B88();
            if (!gSpriteCollisionResult) {
                current->pose = SPOSE_1B;
            }
            current->warioCollision = 1;
        }

    }

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        if (gCurrentSprite.xPosition > gWarioData.xPosition) {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
                gCurrentSprite.pose = SPOSE_11;
            } else {
                gCurrentSprite.pose = SPOSE_17;
            }
        } else {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
                gCurrentSprite.pose = SPOSE_17;
            } else {
                gCurrentSprite.pose = SPOSE_11;
            }
        }
    }
}

void BowBalloonFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
}

void BowBalloonStunnedFallingInit(void)
{
    u8 zero8;

    GP_SET(gCurrentSprite.pOamData, sBowBalloonStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    zero8 = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = zero8;
    gCurrentSprite.warioCollision = 5;
}

void BowBalloonTackledInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void BowBalloonTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    BowBalloonTackledInit();
}

void BowBalloonTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    BowBalloonTackledInit();
}

void BowBalloonDefeatedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void BowBalloonDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    BowBalloonDefeatedInit();
}

void BowBalloonDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    BowBalloonDefeatedInit();
}

void BowBalloonPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void BowBalloonPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    BowBalloonPushedInit();
}

void BowBalloonPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    BowBalloonPushedInit();
}

void BowBalloonPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void BowBalloonPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    BowBalloonPose2BOr2DInit();
}

void BowBalloonPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    BowBalloonPose2BOr2DInit();
}

void BowBalloonCrushed(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sBowBalloonCrushedWhileStunnedOam);
        gCurrentSprite.work0 = 8;
    } else {
        GP_SET(gCurrentSprite.pOamData, sBowBalloonCrushedOam);
        gCurrentSprite.work0 = 10;
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void BowBalloonInstantlyKill(void)
{
    SpriteUtilDie();
}

void BowBalloonStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void BowBalloonPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    BowBalloonStunnedWithoutTimerInit();
}

void BowBalloonPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    BowBalloonStunnedWithoutTimerInit();
}

void BowBalloonAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 22;
    gCurrentSprite.work3 = 0;
}

void BowBalloonAirborne(void)
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
            velocity = sUnk_8352B18[index];
            if (velocity == 0x7FFF) {
                u32 previousVelocityAddress;
                u16 yPosition;

                previousVelocityAddress = index - 1;
                previousVelocityAddress <<= 1;
                previousVelocityAddress += (u32)sUnk_8352B18;
                yPosition = *(volatile u16 *)&gCurrentSprite.yPosition;
                gCurrentSprite.yPosition = yPosition + *(const s16 *)previousVelocityAddress;
            } else {
                u32 nextIndex;
                u8 *storePointer;

                nextIndex = index + 1;
                storePointer = indexPointer;
                *storePointer = nextIndex;
                gCurrentSprite.yPosition += velocity;
            }
        }
    } else {
        gCurrentSprite.pose = SPOSE_1B;
    }
}

void BowBalloonHeldInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sBowBalloonStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void BowBalloonLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
    BowBalloonHeldInit();
}

void BowBalloonLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
    BowBalloonHeldInit();
}

void BowBalloonCarriedInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sBowBalloonStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void BowBalloonCarriedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    BowBalloonCarriedInit();
}

void BowBalloonCarriedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    BowBalloonCarriedInit();
}

void BowBalloonProjectileInit(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 8;
    gCurrentSprite.drawDistanceUp = 8;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 16;
    gCurrentSprite.hitboxExtentDown = 12;
    GP_SET(gCurrentSprite.pOamData, sBowBalloonProjectileOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 39;
    gCurrentSprite.drawPriority = 1;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 0;
        gCurrentSprite.hitboxExtentRight = 12;
    } else {
        gCurrentSprite.hitboxExtentLeft = 16;
        gCurrentSprite.hitboxExtentRight = 0;
    }
}

void BowBalloonProjectileMove(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += 8;
    } else {
        gCurrentSprite.xPosition -= 8;
    }
}

void BowBalloonProjectileDisappearInit(void)
{
    gCurrentSprite.work0 = 20;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
}

void BowBalloonProjectileDisappear(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_6B;
    }
}

void SpriteBowBalloon(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BowBalloonInit();
            break;
        case SPOSE_IDLE_INIT:
            BowBalloonIdleInit();
        case SPOSE_IDLE:
            BowBalloonIdle();
            break;
        case SPOSE_11:
            BowBalloonTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            BowBalloonTurn();
            break;
        case SPOSE_6E:
            BowBalloonAttackInit();
        case SPOSE_6F:
            BowBalloonAttack();
            break;
        case SPOSE_17:
            BowBalloonRecoverInit();
        case SPOSE_18:
            func_8023C94();
            break;
        case SPOSE_19:
            BowBalloonGetUpInit();
        case SPOSE_1A:
            BowBalloonGetUp();
            break;
        case SPOSE_1B:
            BowBalloonFallingInit();
        case SPOSE_1C:
            func_8023D48();
            break;
        case SPOSE_1D:
            BowBalloonStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            BowBalloonTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            BowBalloonTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            BowBalloonPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;
        case SPOSE_PUSHED_LEFT_INIT:
            BowBalloonPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            BowBalloonPose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            BowBalloonPose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            BowBalloonCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            BowBalloonPose33Init();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            BowBalloonPose35Init();
        case SPOSE_36:
            func_802476C();
            break;
        case SPOSE_37:
            func_8024688();
        case SPOSE_38:
            func_80246B8();
            break;
        case SPOSE_39:
            func_802473C();
        case SPOSE_3A:
            func_802476C();
            break;
        case SPOSE_3B:
            func_80247F0();
        case SPOSE_3C:
            func_8024820();
            break;
        case SPOSE_3D:
            func_80248A4();
        case SPOSE_3E:
            func_80248D4();
            break;
        case SPOSE_3F:
            func_8024958();
        case SPOSE_40:
            func_8024988();
            break;
        case SPOSE_41:
            func_8024A0C();
        case SPOSE_42:
            func_8024A3C();
            break;
        case SPOSE_43:
            func_8024AC0();
        case SPOSE_44:
            func_8024AD4();
            break;
        case SPOSE_45:
            func_8024BEC();
        case SPOSE_46:
            func_8024C00();
            break;
        case SPOSE_47:
            BowBalloonPose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            BowBalloonPose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            BowBalloonAirborneInit();
        case SPOSE_4C:
            BowBalloonAirborne();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            BowBalloonLiftedRightInit();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            BowBalloonLiftedLeftInit();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            BowBalloonCarriedRightInit();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            BowBalloonCarriedLeftInit();
            break;
        case SPOSE_CARRIED_LEFT:
            SpriteUtilCarryingSpriteLeft();
            break;
        case SPOSE_THROWN_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_SOFT;
        case SPOSE_THROWN_LEFT_SOFT:
            SpriteUtilThrownLeftSoft();
            break;
        case SPOSE_THROWN_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_HARD;
        case SPOSE_THROWN_LEFT_HARD:
            SpriteUtilThrownLeftHard();
            break;
        case SPOSE_THROWN_UP_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_SOFT;
        case SPOSE_THROWN_UP_LEFT_SOFT:
            SpriteUtilThrownUpLeftSoft();
            break;
        case SPOSE_THROWN_UP_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_HARD;
        case SPOSE_THROWN_UP_LEFT_HARD:
            SpriteUtilThrownUpLeftHard();
            break;
        case SPOSE_THROWN_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_SOFT;
        case SPOSE_THROWN_RIGHT_SOFT:
            SpriteUtilThrownRightSoft();
            break;
        case SPOSE_THROWN_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_HARD;
        case SPOSE_THROWN_RIGHT_HARD:
            SpriteUtilThrownRightHard();
            break;
        case SPOSE_THROWN_UP_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_SOFT;
        case SPOSE_THROWN_UP_RIGHT_SOFT:
            SpriteUtilThrownUpRightSoft();
            break;
        case SPOSE_THROWN_UP_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_HARD;
        case SPOSE_THROWN_UP_RIGHT_HARD:
            SpriteUtilThrownUpRightHard();
            break;
        case SPOSE_69:
            BowBalloonDefeatedLeftInit();
            break;
        case SPOSE_6A:
            BowBalloonDefeatedRightInit();
            break;
        default:
            BowBalloonInstantlyKill();
            break;
    }
}

void SpriteBowBalloonProjectile(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BowBalloonProjectileInit();
            break;
        case SPOSE_IDLE:
            BowBalloonProjectileMove();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            BowBalloonProjectileDisappearInit();
        case SPOSE_CRUSHED_OR_COLLECTED:
            BowBalloonProjectileDisappear();
            gCurrentSprite.disableWarioCollisionTimer = 1;
            break;
        default:
            SpriteUtilDie();
            break;
    }
}

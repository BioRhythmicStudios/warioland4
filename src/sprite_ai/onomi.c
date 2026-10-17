#include "sprite_ai/onomi.h"

#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"

void OnomiSetWalkingHitbox(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 32;
        gCurrentSprite.hitboxExtentRight = 52;
    } else {
        gCurrentSprite.hitboxExtentLeft = 56;
        gCurrentSprite.hitboxExtentRight = 28;
    }
}

void OnomiInit(void)
{
    gCurrentSprite.warioCollision = 3;
    gCurrentSprite.drawDistanceDown = 40;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    GP_SET(gCurrentSprite.pOamData, sOnomiWalkingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    SpriteUtilTurnTowardWario();
}

void OnomiInitWalk(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiWalkingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 20;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = 3;
    OnomiSetWalkingHitbox();
}

void OnomiWalk(void)
{
    u32 nearby;

    if (gCurrentSprite.pose == SPOSE_IDLE) {
        nearby = SpriteUtilWaitCheckWarioNearbyLeftRight(127, 400);
    } else {
        nearby = 0;
    }

    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
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
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
            if (!gSpriteCollisionTileType) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
            func_8023BFC(
                gCurrentSprite.yPosition - 32,
                gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
            );
            if (gSpriteCollisionTileType == 17) {
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
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (!gSpriteCollisionTileType) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
            func_8023BFC(
                gCurrentSprite.yPosition - 32,
                gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
            );
            if (gSpriteCollisionTileType == 17) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
        }
    }

    if (gCurrentSprite.pose == SPOSE_IDLE) {
        func_8026374();
    } else {
        func_8026418();
    }
}

void OnomiPrepareAttack(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiPreparingAttackOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_6F;
    gCurrentSprite.work0 = 40;
    m4aSongNumStart(SE_MENONO_ONOMI_PREPARE_ATTACK);
}

void OnomiWaitToAttack(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        OnomiWalk();
    } else {
        gCurrentSprite.pose = SPOSE_18;
        GP_SET(gCurrentSprite.pOamData, sOnomiAttackingOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.work0 = 13;
        m4aSongNumStart(SE_MENONO_ONOMI_ATTACK);
    }
}

void OnomiTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.work0 = 9;
    OnomiSetWalkingHitbox();
}

void OnomiTurn(void)
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
        GP_SET(gCurrentSprite.pOamData, sOnomiTurningAroundOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_14;
        gCurrentSprite.work0 = 14;
        OnomiSetWalkingHitbox();
    } else {
        GP_SET(gCurrentSprite.pOamData, sOnomiWalkingOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_IDLE;
        gCurrentSprite.warioCollision = 3;
    }
}

void OnomiStunnedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
        gCurrentSprite.hitboxExtentLeft = 32;
        gCurrentSprite.hitboxExtentRight = 28;
    }
}

void OnomiPose33(void)
{
    gCurrentSprite.pose = SPOSE_34;
    OnomiStunnedInit();
}

void OnomiPose35(void)
{
    gCurrentSprite.pose = SPOSE_36;
    OnomiStunnedInit();
}

void OnomiRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 20;
}

void OnomiGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 16;
    gCurrentSprite.warioCollision = 3;
    gCurrentSprite.work3 = 0;
    OnomiSetWalkingHitbox();
}

void OnomiFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 3;
}

void OnomiStunnedFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
}

void OnomiTackledInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void OnomiTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    OnomiTackledInit();
}

void OnomiTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    OnomiTackledInit();
}

void OnomiDefeatedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void OnomiDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    OnomiDefeatedInit();
}

void OnomiDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    OnomiDefeatedInit();
}

void OnomiPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void OnomiPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    OnomiPushedInit();
}

void OnomiPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    OnomiPushedInit();
}

void OnomiPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
}

void OnomiPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    OnomiPose2BOr2DInit();
}

void OnomiPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    OnomiPose2BOr2DInit();
}

void OnomiCrushed(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sOnomiCrushedWhileStunnedOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sOnomiCrushedOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 10;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void OnomiInstantlyKill(void)
{
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void OnomiStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
}

void OnomiPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    OnomiStunnedWithoutTimerInit();
}

void OnomiPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    OnomiStunnedWithoutTimerInit();
}

void OnomiAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sOnomiPushedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 3;
}

void OnomiAirborne(void)
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
                previousVelocityAddress += (u32) sUnk_8352B18;
                yPosition = *(volatile u16 *)&gCurrentSprite.yPosition;
                gCurrentSprite.yPosition = yPosition + *(const s16 *) previousVelocityAddress;
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
    }
}

void OnomiLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
}

void OnomiLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
}

void OnomiCarriedRight(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void OnomiCarriedLeft(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void SpriteOnomi(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            OnomiInit();
        case SPOSE_IDLE_INIT:
            OnomiInitWalk();
        case SPOSE_IDLE:
            OnomiWalk();
            break;
        case SPOSE_6E:
            OnomiPrepareAttack();
        case SPOSE_6F:
            OnomiWaitToAttack();
            break;
        case SPOSE_11:
            OnomiTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            OnomiTurn();
            break;
        case SPOSE_17:
            OnomiRecoverInit();
        case SPOSE_18:
            func_8023C94();
            break;
        case SPOSE_19:
            OnomiGetUpInit();
            break;
        case SPOSE_1A:
            OnomiAirborne();
            break;
        case SPOSE_1B:
            OnomiFallingInit();
        case SPOSE_1C:
            func_8023E00();
            break;
        case SPOSE_1D:
            OnomiStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            OnomiTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            OnomiTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
        case SPOSE_27:
            OnomiPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;
        case SPOSE_PUSHED_LEFT_INIT:
        case SPOSE_29:
            OnomiPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            OnomiPose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            OnomiPose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            OnomiCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            OnomiPose33();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            OnomiPose35();
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
            OnomiPose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            OnomiPose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            OnomiAirborneInit();
        case SPOSE_4C:
            OnomiAirborne();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            OnomiLiftedRightInit();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            OnomiLiftedLeftInit();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            OnomiCarriedRight();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            OnomiCarriedLeft();
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
            OnomiDefeatedLeftInit();
            break;
        case SPOSE_6A:
            OnomiDefeatedRightInit();
            break;
        default:
            OnomiInstantlyKill();
            break;
    }
}

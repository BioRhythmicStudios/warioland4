#include "sprite_ai/toy_car.h"

#include "score.h"
#include "sprite.h"
#include "sprite_util.h"

void UpdateToyCarHorizontalHitbox(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 40;
        gCurrentSprite.hitboxExtentRight = 60;
    } else {
        gCurrentSprite.hitboxExtentLeft = 64;
        gCurrentSprite.hitboxExtentRight = 36;
    }
}

void ToyCarInit(void)
{
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    SpriteUtilTurnTowardWario();
}

void ToyCarIdleInit(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    GP_SET(gCurrentSprite.pOamData, sToyCarIdleOam);
    gCurrentSprite.warioCollision = 3;
    UpdateToyCarHorizontalHitbox();
}

void ToyCarIdle(void)
{
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
        if ((gSpriteCollisionResult & 0xF0) &&
            ((gCurrentSprite.xPosition & 0x3F) + gCurrentSprite.hitboxExtentRight > 0x3F)) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 44);
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
    } else if ((gSpriteCollisionResult & 0xF0) &&
        ((gCurrentSprite.xPosition & 0x3F) < gCurrentSprite.hitboxExtentLeft)) {
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 48);
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

    SpriteUtilMoveForward1HalfPixel();
}

void ToyCarTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 19;
    gCurrentSprite.warioCollision = 3;
    UpdateToyCarHorizontalHitbox();
}

void ToyCarTurn(void)
{
    struct PrimarySpriteData *sprite;
    u32 remaining;
    u32 zero;
    u8 *work0;

    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        gCurrentSprite.pose = SPOSE_1B;
        return;
    }

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    {
        u32 value;

        value = *work0;
        value--;
        *work0 = value;
        remaining = (u8)value;
    }
    if (remaining != 0) {
        switch (sprite->pose) {
            case SPOSE_12:
                if (sprite->animationTimer > 2)
                    sprite->warioCollision = 1;
                break;

            case SPOSE_14:
                if (sprite->animationTimer > 1)
                    sprite->warioCollision = 3;
                break;
        }
    } else {
        switch (sprite->pose) {
            case SPOSE_12:
            {
                u16 status;

                status = sprite->status;
                status ^= SPRITE_STATUS_FACING_RIGHT;
                zero = 0;
                sprite->status = status;
                UpdateToyCarHorizontalHitbox();
                GP_SET(sprite->pOamData, sToyCarTurningAroundOam);
                sprite->currentAnimationFrame = zero;
                sprite->animationTimer = remaining;
                sprite->pose = SPOSE_14;
                *work0 = 14;
                break;
            }

            case SPOSE_14:
                sprite->pose = SPOSE_IDLE_INIT;
                break;
        }
    }
}

void ToyCarRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarRecoveringOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 16;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void ToyCarGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 25;
}

void ToyCarFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
}

void ToyCarStunnedFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void ToyCarTackledInit(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sToyCarTackledOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sToyCarAlternateTackledOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(PSPRITE_TOY_CAR, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void ToyCarTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    ToyCarTackledInit();
}

void ToyCarTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    ToyCarTackledInit();
}

void ToyCarDefeatedInit(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sToyCarTackledOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sToyCarAlternateTackledOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(PSPRITE_TOY_CAR, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void ToyCarDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    ToyCarDefeatedInit();
}

void ToyCarDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    ToyCarDefeatedInit();
}

void ToyCarPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void ToyCarPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    ToyCarPushedInit();
}

void ToyCarPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    ToyCarPushedInit();
}

void ToyCarAlternatePushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarAlternatePushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void ToyCarAlternatePushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_28;
    ToyCarAlternatePushedInit();
}

void ToyCarAlternatePushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_2A;
    ToyCarAlternatePushedInit();
}

void ToyCarPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void ToyCarPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    ToyCarPose2BOr2DInit();
}

void ToyCarPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    ToyCarPose2BOr2DInit();
}

void ToyCarCrushed(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sToyCarCrushedOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sToyCarAlternateCrushedOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 11;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    SpawnEnemyScoreDrop(PSPRITE_TOY_CAR, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void ToyCarInstantlyKill(void)
{
    SpawnEnemyScoreDrop(PSPRITE_TOY_CAR, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void ToyCarStunnedInit(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sToyCarStunnedOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sToyCarAlternateStunnedOam);
    }
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
        gCurrentSprite.hitboxExtentLeft = 36;
        gCurrentSprite.hitboxExtentRight = 32;
    }
}

void ToyCarPose33Init(void)
{
    gCurrentSprite.pose = SPOSE_34;
    ToyCarStunnedInit();
}

void ToyCarPose35Init(void)
{
    gCurrentSprite.pose = SPOSE_36;
    ToyCarStunnedInit();
}

void ToyCarStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void ToyCarPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    ToyCarStunnedWithoutTimerInit();
}

void ToyCarPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    ToyCarStunnedWithoutTimerInit();
}

void ToyCarAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarFallingOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 3;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void ToyCarAirborne(void)
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

void ToyCarHeldInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sToyCarHeldOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void ToyCarLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
    ToyCarHeldInit();
}

void ToyCarLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
    ToyCarHeldInit();
}

void ToyCarCarriedInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sToyCarHeldOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void ToyCarCarriedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    ToyCarCarriedInit();
}

void ToyCarCarriedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    ToyCarCarriedInit();
}

void SpriteToyCar(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            ToyCarInit();
        case SPOSE_IDLE_INIT:
            ToyCarIdleInit();
        case SPOSE_IDLE:
            ToyCarIdle();
            break;
        case SPOSE_11:
            ToyCarTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            ToyCarTurn();
            break;
        case SPOSE_17:
            ToyCarRecoverInit();
        case SPOSE_18:
            func_8023C94();
            break;
        case SPOSE_19:
            ToyCarGetUpInit();
        case SPOSE_1A:
            func_8023CD8();
            break;
        case SPOSE_1B:
            ToyCarFallingInit();
        case SPOSE_1C:
            func_8023D48();
            break;
        case SPOSE_1D:
            ToyCarStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            ToyCarTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            ToyCarTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            ToyCarPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;
        case SPOSE_PUSHED_LEFT_INIT:
            ToyCarPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_27:
            ToyCarAlternatePushedRightInit();
        case SPOSE_28:
            SpriteUtilPushedRight();
            break;
        case SPOSE_29:
            ToyCarAlternatePushedLeftInit();
        case SPOSE_2A:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            ToyCarPose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            ToyCarPose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            ToyCarCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            ToyCarPose33Init();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            ToyCarPose35Init();
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
            ToyCarPose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            ToyCarPose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            ToyCarAirborneInit();
        case SPOSE_4C:
            ToyCarAirborne();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            ToyCarLiftedRightInit();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            ToyCarLiftedLeftInit();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            ToyCarCarriedRightInit();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            ToyCarCarriedLeftInit();
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
            ToyCarDefeatedLeftInit();
            break;
        case SPOSE_6A:
            ToyCarDefeatedRightInit();
            break;
        default:
            ToyCarInstantlyKill();
            break;
    }
}

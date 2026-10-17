#include "sprite_ai/dice.h"

#include "score.h"
#include "sprite.h"
#include "sprite_util.h"

void UpdateDiceHorizontalHitbox(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 32;
        gCurrentSprite.hitboxExtentRight = 60;
    } else {
        gCurrentSprite.hitboxExtentLeft = 64;
        gCurrentSprite.hitboxExtentRight = 28;
    }
}

void DiceInit(void)
{
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    SpriteUtilTurnTowardWario();
}

void DiceIdleInit(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    GP_SET(gCurrentSprite.pOamData, sDiceIdleOam);
    gCurrentSprite.warioCollision = 3;
    UpdateDiceHorizontalHitbox();
}

void DiceIdle(void)
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
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 48);
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
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 52);
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

void DiceTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 13;
    gCurrentSprite.warioCollision = 3;
    UpdateDiceHorizontalHitbox();
}

void DiceTurn(void)
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
                if (sprite->animationTimer == 0)
                    sprite->warioCollision = 1;
                break;

            case SPOSE_14:
                if (sprite->animationTimer == 0)
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
                UpdateDiceHorizontalHitbox();
                GP_SET(sprite->pOamData, sDiceTurningAroundOam);
                sprite->currentAnimationFrame = zero;
                sprite->animationTimer = remaining;
                sprite->pose = SPOSE_14;
                *work0 = 7;
                break;
            }

            case SPOSE_14:
                sprite->pose = SPOSE_IDLE_INIT;
                break;
        }
    }
}

void DiceRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceRecoveringOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 28;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void DiceGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 17;
}

void DiceFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void DiceStunnedFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void DiceTackledInit(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sDiceTackledOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sDiceAlternateTackledOam);
    }
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

void DiceTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    DiceTackledInit();
}

void DiceTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    DiceTackledInit();
}

void DiceDefeatedInit(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sDiceTackledOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sDiceAlternateTackledOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void DiceDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    DiceDefeatedInit();
}

void DiceDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    DiceDefeatedInit();
}

void DicePushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDicePushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void DicePushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    DicePushedInit();
}

void DicePushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    DicePushedInit();
}

void DiceAlternatePushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceAlternatePushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void DiceAlternatePushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_28;
    DiceAlternatePushedInit();
}

void DiceAlternatePushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_2A;
    DiceAlternatePushedInit();
}

void DicePose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void DicePose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    DicePose2BOr2DInit();
}

void DicePose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    DicePose2BOr2DInit();
}

void DiceCrushed(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sDiceCrushedOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sDiceAlternateCrushedOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 9;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void DiceInstantlyKill(void)
{
    SpriteUtilDie();
}

void DiceStunnedInit(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sDiceStunnedOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sDiceAlternateStunnedOam);
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

void DicePose33Init(void)
{
    gCurrentSprite.pose = SPOSE_34;
    DiceStunnedInit();
}

void DicePose35Init(void)
{
    gCurrentSprite.pose = SPOSE_36;
    DiceStunnedInit();
}

void DiceStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void DicePose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    DiceStunnedWithoutTimerInit();
}

void DicePose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    DiceStunnedWithoutTimerInit();
}

void DiceAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceFallingOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 3;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
}

void DiceAirborne(void)
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
                {
                    s16 previousVelocity = *(const s16 *)previousVelocityAddress;
                    yPosition += previousVelocity;
                }
                gCurrentSprite.yPosition = yPosition;
            } else {
                u32 nextIndex;

                nextIndex = index + 1;
                *indexPointer = nextIndex;
                gCurrentSprite.yPosition += velocity;
            }
        }
    } else {
        gCurrentSprite.pose = SPOSE_1B;
    }
}

void DiceHeldInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void DiceLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
    DiceHeldInit();
}

void DiceLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
    DiceHeldInit();
}

void DiceCarriedInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sDiceStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void DiceCarriedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    DiceCarriedInit();
}

void DiceCarriedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    DiceCarriedInit();
}

void SpriteDice(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            DiceInit();
        case SPOSE_IDLE_INIT:
            DiceIdleInit();
        case SPOSE_IDLE:
            DiceIdle();
            break;
        case SPOSE_11:
            DiceTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            DiceTurn();
            break;
        case SPOSE_17:
            DiceRecoverInit();
        case SPOSE_18:
            func_8023C94();
            break;
        case SPOSE_19:
            DiceGetUpInit();
        case SPOSE_1A:
            func_8023CD8();
            break;
        case SPOSE_1B:
            DiceFallingInit();
        case SPOSE_1C:
            func_8023D48();
            break;
        case SPOSE_1D:
            DiceStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            DiceTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            DiceTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            DicePushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;
        case SPOSE_PUSHED_LEFT_INIT:
            DicePushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_27:
            DiceAlternatePushedRightInit();
        case SPOSE_28:
            SpriteUtilPushedRight();
            break;
        case SPOSE_29:
            DiceAlternatePushedLeftInit();
        case SPOSE_2A:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            DicePose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            DicePose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            DiceCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            DicePose33Init();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            DicePose35Init();
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
            DicePose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            DicePose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            DiceAirborneInit();
        case SPOSE_4C:
            DiceAirborne();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            DiceLiftedRightInit();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            DiceLiftedLeftInit();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            DiceCarriedRightInit();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            DiceCarriedLeftInit();
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
            DiceDefeatedLeftInit();
            break;
        case SPOSE_6A:
            DiceDefeatedRightInit();
            break;
        default:
            DiceInstantlyKill();
            break;
    }
}

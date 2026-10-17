#include "sprite_ai/menhammer.h"

#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"

void MenhammerSetWalkingHitbox(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 36;
        gCurrentSprite.hitboxExtentRight = 52;
    } else {
        gCurrentSprite.hitboxExtentLeft = 56;
        gCurrentSprite.hitboxExtentRight = 32;
    }
}

void MenhammerInit(void)
{
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.drawDistanceDown = 40;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 80;
    gCurrentSprite.hitboxExtentDown = 0;
    SpriteUtilTurnTowardWario();
}

void MenhammerInitWalk(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerWalkingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 1;
    MenhammerSetWalkingHitbox();
}

void MenhammerWalk(void)
{
    u32 nearby;

    nearby = SpriteUtilWaitCheckWarioNearbyLeftRight(136, 322);
    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 40);
        } else {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 36);
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
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 36);
            if (!gSpriteCollisionTileType) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
            func_8023BFC(gCurrentSprite.yPosition - 32,
                gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
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
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 40);
            if (!gSpriteCollisionTileType) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
            func_8023BFC(gCurrentSprite.yPosition - 32,
                gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (gSpriteCollisionTileType == 17) {
                gCurrentSprite.pose = SPOSE_11;
                return;
            }
        }
    }

    func_80263AC();
}





void MenhammerTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 9;
    MenhammerSetWalkingHitbox();
}

void MenhammerTurn(void)
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
        GP_SET(gCurrentSprite.pOamData, sMenhammerTurningAroundOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_14;
        gCurrentSprite.work0 = 8;
        MenhammerSetWalkingHitbox();
    } else {
        GP_SET(gCurrentSprite.pOamData, sMenhammerWalkingOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_IDLE;
    }
}

void MenhammerPrepareAttack(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerJumpingAttackOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_6F;
    gCurrentSprite.work0 = 5;
    gCurrentSprite.work3 = 0;
}

void MenhammerAttack(void)
{
    {
        struct PrimarySpriteData *sprite;
        register u8 *work0Pointer PIN("r5");
        register u32 initialWork0 PIN("r6");
        u32 work0;

        sprite = &gCurrentSprite;
        work0Pointer = &sprite->work0;
        work0 = *work0Pointer;
        initialWork0 = work0;
        if (initialWork0 != 0) {
            work0--;
            *work0Pointer = work0;
            if ((work0 << 24) == 0) {
                sprite->warioCollision = 52;
                m4aSongNumStart(SE_MENHAMMER_ATTACK);
            }
            return;
        }

        if (sprite->work3 <= 9) {
            func_8023BFC(sprite->yPosition - sprite->hitboxExtentUp, sprite->xPosition);
            if (gSpriteCollisionTileType & 0xF) {
                sprite->pose = SPOSE_1D;
                return;
            }
        } else {
            u32 yPosition;

            yPosition = func_8023A60(sprite->yPosition, sprite->xPosition);
            if (gBackgroundCollisionData.waterType == 1) {
                sprite->status |= SPRITE_STATUS_UNDERWATER;
            }
            if (gSpriteCollisionResult) {
                sprite->yPosition = yPosition;
                sprite->pose = SPOSE_18;
                GP_SET(sprite->pOamData, sMenhammerLandingAttackOam);
                sprite->currentAnimationFrame = 0;
                sprite->animationTimer = initialWork0;
                *work0Pointer = 25;
                sprite->warioCollision = 1;
                return;
            }
        }
    }

    {
        struct PrimarySpriteData *spriteBase;
        register u8 *indexPointer PIN("ip");
        u32 index;
        const s16 *table;
        register u32 r4Value PIN("r4");
        u32 r5Value;
        u32 velocityAddress;
        s32 signedVelocity;
        u32 sentinel;
        s32 yPosition;

        spriteBase = &gCurrentSprite;
        r4Value = 42;
        /* Keep the field offset in r4 before adding the sprite base. */
        ASM_BARRIER(: "+r" (r4Value));
        r4Value += (u32) spriteBase;
        indexPointer = (u8 *) r4Value;
        index = *(const u8 *) r4Value;
        table = sMenhammerYVelocity;
        velocityAddress = index << 1;
        velocityAddress += (u32) table;
        r5Value = *(const u16 *) velocityAddress;
        r4Value = 0;
        signedVelocity = *(const s16 *) (velocityAddress + r4Value);
        sentinel = 0x7FFF;
        r4Value = (u32) spriteBase;
        if (signedVelocity == sentinel) {
            register u32 previousVelocityAddress PIN("r1");

            previousVelocityAddress = index - 1;
            previousVelocityAddress <<= 1;
            previousVelocityAddress += (u32) table;
            yPosition = ((struct PrimarySpriteData *) r4Value)->yPosition;
            previousVelocityAddress = *(const u16 *) previousVelocityAddress;
            yPosition = yPosition + previousVelocityAddress;
        } else {
            u32 nextIndex;
            register u8 *storePointer PIN("r1");

            nextIndex = index + 1;
            storePointer = indexPointer;
            *storePointer = nextIndex;
            yPosition = ((struct PrimarySpriteData *) r4Value)->yPosition;
            yPosition += r5Value;
        }
        ((struct PrimarySpriteData *) r4Value)->yPosition = yPosition;
        r5Value = r4Value;

        if (((struct PrimarySpriteData *) r5Value)->status & SPRITE_STATUS_FACING_RIGHT) {
            register u32 xPosition PIN("r1");

            xPosition = ((struct PrimarySpriteData *) r5Value)->xPosition;
            xPosition += 8;
            ((struct PrimarySpriteData *) r5Value)->xPosition = xPosition;
            func_8023BFC(((struct PrimarySpriteData *) r5Value)->yPosition - 32,
                xPosition + ((struct PrimarySpriteData *) r5Value)->hitboxExtentRight);
            if (gSpriteCollisionTileType == 17) {
                ((struct PrimarySpriteData *) r5Value)->pose = SPOSE_47;
                m4aSongNumStart(SE_WALL_BOUNCE);
            }
        } else {
            u32 xPosition;

            xPosition = ((struct PrimarySpriteData *) r4Value)->xPosition;
            xPosition -= 8;
            ((struct PrimarySpriteData *) r4Value)->xPosition = xPosition;
            func_8023BFC(((struct PrimarySpriteData *) r4Value)->yPosition - 32,
                xPosition - ((struct PrimarySpriteData *) r4Value)->hitboxExtentLeft);
            if (gSpriteCollisionTileType == 17) {
                ((struct PrimarySpriteData *) r4Value)->pose = SPOSE_45;
                m4aSongNumStart(SE_WALL_BOUNCE);
            }
        }
    }
}

void MenhammerStunnedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
        gCurrentSprite.hitboxExtentLeft = 40;
        gCurrentSprite.hitboxExtentRight = 36;
    }
}

void MenhammerPose33(void)
{
    gCurrentSprite.pose = SPOSE_34;
    MenhammerStunnedInit();
}

void MenhammerPose35(void)
{
    gCurrentSprite.pose = SPOSE_36;
    MenhammerStunnedInit();
}

void MenhammerRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 20;
}

void MenhammerGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 16;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.work3 = 0;
    MenhammerSetWalkingHitbox();
}

void MenhammerFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
}

void MenhammerStunnedFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 40;
    gCurrentSprite.hitboxExtentRight = 36;
}

void MenhammerTackledInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerTackledOam);
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

void MenhammerTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    MenhammerTackledInit();
}

void MenhammerTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    MenhammerTackledInit();
}

void MenhammerDefeatedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void MenhammerDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    MenhammerDefeatedInit();
}

void MenhammerDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    MenhammerDefeatedInit();
}

void MenhammerPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void MenhammerPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    MenhammerPushedInit();
}

void MenhammerPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    MenhammerPushedInit();
}

void MenhammerPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 40;
    gCurrentSprite.hitboxExtentRight = 36;
}

void MenhammerPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    MenhammerPose2BOr2DInit();
}

void MenhammerPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    MenhammerPose2BOr2DInit();
}

void MenhammerCrushed(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sMenhammerCrushedWhileStunnedOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sMenhammerCrushedOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 10;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void MenhammerInstantlyKill(void)
{
    SpriteUtilDie();
}

void MenhammerStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 40;
    gCurrentSprite.hitboxExtentRight = 36;
}

void MenhammerPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    MenhammerStunnedWithoutTimerInit();
}

void MenhammerPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    MenhammerStunnedWithoutTimerInit();
}

void MenhammerAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMenhammerPushedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
}

void MenhammerAirborne(void)
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

void MenhammerLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
}

void MenhammerLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
}

void MenhammerCarriedRight(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void MenhammerCarriedLeft(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void SpriteMenhammer(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            MenhammerInit();
        case SPOSE_IDLE_INIT:
            MenhammerInitWalk();
        case SPOSE_IDLE:
            MenhammerWalk();
            break;
        case SPOSE_11:
            MenhammerTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            MenhammerTurn();
            break;
        case SPOSE_6E:
            MenhammerPrepareAttack();
        case SPOSE_6F:
            MenhammerAttack();
            break;
        case SPOSE_17:
            MenhammerRecoverInit();
        case SPOSE_18:
            func_8023C94();
            break;
        case SPOSE_19:
            MenhammerGetUpInit();
            break;
        case SPOSE_1A:
            MenhammerAirborne();
            break;
        case SPOSE_1B:
            MenhammerFallingInit();
        case SPOSE_1C:
            func_8023E00();
            break;
        case SPOSE_1D:
            MenhammerStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            MenhammerTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            MenhammerTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            MenhammerPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;
        case SPOSE_PUSHED_LEFT_INIT:
            MenhammerPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            MenhammerPose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            MenhammerPose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            MenhammerCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            MenhammerPose33();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            MenhammerPose35();
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
            MenhammerPose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            MenhammerPose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            MenhammerAirborneInit();
        case SPOSE_4C:
            MenhammerAirborne();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            MenhammerLiftedRightInit();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            MenhammerLiftedLeftInit();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            MenhammerCarriedRight();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            MenhammerCarriedLeft();
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
            MenhammerDefeatedLeftInit();
            break;
        case SPOSE_6A:
            MenhammerDefeatedRightInit();
            break;
        default:
            MenhammerInstantlyKill();
            break;
    }
}

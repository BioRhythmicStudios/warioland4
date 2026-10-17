#include "sprite_ai/caterpillar.h"

#include "score.h"
#include "sprite.h"
#include "sprite_util.h"

void CaterpillarInit(void)
{
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 32;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sCaterpillarCrawlingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    SpriteUtilTurnTowardWario();
}

void CaterpillarInitCrawl(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarCrawlingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.hitboxExtentUp = 32;
}

void CaterpillarCrawl(void)
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
    } else if ((gSpriteCollisionResult & 0xF0) &&
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

    SpriteUtilMoveForward1Subpixel();
}

void CaterpillarTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 22;
}

void CaterpillarTurn(void)
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
        GP_SET(gCurrentSprite.pOamData, sCaterpillarTurningAroundOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_14;
        gCurrentSprite.work0 = 21;
    } else {
        GP_SET(gCurrentSprite.pOamData, sCaterpillarCrawlingOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_IDLE;
    }
}

void CaterpillarStunnedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
    }
}

void CaterpillarPose33(void)
{
    gCurrentSprite.pose = SPOSE_34;
    CaterpillarStunnedInit();
}

void CaterpillarPose35(void)
{
    gCurrentSprite.pose = SPOSE_36;
    CaterpillarStunnedInit();
}

void CaterpillarRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarCrawlingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 60;
}

void CaterpillarGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 32;
}

void CaterpillarGetUp(void)
{
    func_80238E8();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        gCurrentSprite.pose = SPOSE_1D;
    }

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_17;
        gCurrentSprite.warioCollision = 1;
        gCurrentSprite.hitboxExtentUp = 32;
    }
}

void CaterpillarFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
}

void CaterpillarStunnedFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void CaterpillarTackledInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void CaterpillarTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    CaterpillarTackledInit();
}

void CaterpillarTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    CaterpillarTackledInit();
}

void CaterpillarDefeatedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void CaterpillarDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    CaterpillarDefeatedInit();
}

void CaterpillarDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    CaterpillarDefeatedInit();
}

void CaterpillarPushedInit(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void CaterpillarPushedRightInit(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sCaterpillarPushedForwardOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sCaterpillarPushedBackwardOam);
    }
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    CaterpillarPushedInit();
}

void CaterpillarPushedLeftInit(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sCaterpillarPushedBackwardOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sCaterpillarPushedForwardOam);
    }
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    CaterpillarPushedInit();
}

void CaterpillarPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void CaterpillarPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    CaterpillarPose2BOr2DInit();
}

void CaterpillarPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    CaterpillarPose2BOr2DInit();
}

void CaterpillarCrushed(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarCrushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 11;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void CaterpillarInstantlyKill(void)
{
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void CaterpillarStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void CaterpillarPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    CaterpillarStunnedWithoutTimerInit();
}

void CaterpillarPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    CaterpillarStunnedWithoutTimerInit();
}

void CaterpillarAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarFallingOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
}

void CaterpillarAirborne(void)
{
    u8* indexPointer;
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
                yPosition = *(volatile u16*)&gCurrentSprite.yPosition;
                gCurrentSprite.yPosition = yPosition + *(const s16*)previousVelocityAddress;
            } else {
                u32 nextIndex;
                u8* storePointer;

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

void CaterpillarLiftedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sCaterpillarCarriedOam);
    gCurrentSprite.animationTimer = (u16)0;
    gCurrentSprite.currentAnimationFrame = (u8)0;
    gCurrentSprite.hitboxExtentUp = 64;
}

void CaterpillarLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
    CaterpillarLiftedInit();
}

void CaterpillarLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
    CaterpillarLiftedInit();
}

void CaterpillarCarriedInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sCaterpillarCarriedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.hitboxExtentUp = 64;
}

void CaterpillarCarriedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    CaterpillarCarriedInit();
}

void CaterpillarCarriedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    CaterpillarCarriedInit();
}

void SpriteCaterpillar(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            CaterpillarInit();
            break;
        case SPOSE_IDLE_INIT:
            CaterpillarInitCrawl();
        case SPOSE_IDLE:
            CaterpillarCrawl();
            break;
        case SPOSE_11:
            CaterpillarTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            CaterpillarTurn();
            break;
        case SPOSE_17:
            CaterpillarRecoverInit();
        case SPOSE_18:
            func_8023C94();
            break;
        case SPOSE_19:
            CaterpillarGetUpInit();
        case SPOSE_1A:
            CaterpillarGetUp();
            break;
        case SPOSE_1B:
            CaterpillarFallingInit();
        case SPOSE_1C:
            func_8023D48();
            break;
        case SPOSE_1D:
            CaterpillarStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            CaterpillarTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            CaterpillarTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            CaterpillarPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;
        case SPOSE_PUSHED_LEFT_INIT:
            CaterpillarPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            CaterpillarPose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            CaterpillarPose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            CaterpillarCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            CaterpillarPose33();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            CaterpillarPose35();
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
            CaterpillarPose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            CaterpillarPose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            CaterpillarAirborneInit();
        case SPOSE_4C:
            CaterpillarAirborne();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            CaterpillarLiftedRightInit();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            CaterpillarLiftedLeftInit();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            CaterpillarCarriedRightInit();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            CaterpillarCarriedLeftInit();
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
            CaterpillarDefeatedLeftInit();
            break;
        case SPOSE_6A:
            CaterpillarDefeatedRightInit();
            break;
        default:
            CaterpillarInstantlyKill();
            break;
    }
}

#include "sprite_ai/spiky.h"

#include "score.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"


void SpikyInit(void)
{
    struct PrimarySpriteData *sprite;
    int zero;

    sprite = &gCurrentSprite;
    sprite->warioCollision = 35;
    sprite->drawDistanceDown = 36;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 20;
    sprite->hitboxExtentUp = 64;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 48;
    sprite->hitboxExtentRight = 44;
    GP_SET(sprite->pOamData, sSpikyIdleOam);
    sprite->currentAnimationFrame = 0;
    zero = 0;
    sprite->animationTimer = 0;
    sprite->pose = SPOSE_IDLE;
    SpriteUtilTurnTowardWario();
    if (sprite->globalID == 29) {
        sprite->palette = 1;
        sprite->status |= SPRITE_STATUS_HEAVY;
    } else {
        sprite->palette = zero;
    }
}

void SpikyCheckScreenShake(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        if (gScreenShakeY.duration > 57) {
            gCurrentSprite.pose = SPOSE_4B;
        } else if (gScreenShakeY.duration > 41) {
            gCurrentSprite.pose = SPOSE_17;
        }
    } else if (gScreenShakeY.duration > 41) {
        gCurrentSprite.pose = SPOSE_4B;
    }
}

void SpikyIdleInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = 35;
}

void SpikyIdle(void)
{
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
                gCurrentSprite.xPosition + 28);
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
            gCurrentSprite.xPosition - 32);
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

    func_80263AC();
}

void SpikyTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 15;
    gCurrentSprite.warioCollision = 35;
}

void SpikyTurn(void)
{
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
        GP_SET(gCurrentSprite.pOamData, sSpikyTurningAroundOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_14;
        gCurrentSprite.work0 = 20;
        gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
    } else {
        gCurrentSprite.pose = SPOSE_IDLE_INIT;
    }
}

void SpikyRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyRecoveringOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 24;
    gCurrentSprite.warioCollision = 35;
}

void SpikyRecover(void)
{
    SpikyCheckScreenShake();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        gCurrentSprite.pose = SPOSE_1B;
        return;
    }

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        return;
    }

    if (gCurrentSprite.xPosition > gWarioData.xPosition) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            gCurrentSprite.pose = SPOSE_11;
        } else {
            gCurrentSprite.pose = SPOSE_IDLE_INIT;
        }
    } else if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.pose = SPOSE_IDLE_INIT;
    } else {
        gCurrentSprite.pose = SPOSE_11;
    }
}

void SpikyGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 42;
}

void SpikyGetUp(void)
{
    u8 timer;
    u8 timerCopy;

    timer = gCurrentSprite.work0;
    timerCopy = timer;
    if (timer > 34) {
        func_8023B88();
        if (!gSpriteCollisionResult) {
            gCurrentSprite.pose = SPOSE_1D;
        }
        func_80238E8();
        gCurrentSprite.work0--;
    } else if (timer <= 13) {
        func_8023B88();
        if (!gSpriteCollisionResult) {
            gCurrentSprite.pose = SPOSE_1B;
        } else {
            gCurrentSprite.work0--;
            if (gCurrentSprite.work0 != 0) {
                return;
            }
            gCurrentSprite.pose = SPOSE_17;
        }
        SpikyCheckScreenShake();
    } else {
        if (timer == 34) {
            gCurrentSprite.yPosition -= 24;
        } else if (timer == 30) {
            gCurrentSprite.yPosition -= 24;
        } else if (timer == 22) {
            gCurrentSprite.yPosition += 24;
            gCurrentSprite.warioCollision = 35;
        } else if (timerCopy == 18) {
            gCurrentSprite.yPosition += 24;
        }
        gCurrentSprite.work0--;
    }
}

void SpikyFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
}

void SpikyStunnedFallingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 36;
}

void SpikyTackledInit(void)
{
    u8 scoreType;

    GP_SET(gCurrentSprite.pOamData, sSpikyDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    if (gCurrentSprite.globalID == 29) {
        scoreType = 30;
        gCurrentSprite.work2 = 4;
    } else {
        scoreType = 10;
        gCurrentSprite.work2 = 8;
    }
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, scoreType);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void SpikyTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    SpikyTackledInit();
}

void SpikyTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    SpikyTackledInit();
}

void SpikyDefeatedInit(void)
{
    u8 scoreType;

    GP_SET(gCurrentSprite.pOamData, sSpikyDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    if (gCurrentSprite.globalID == 29) {
        scoreType = 30;
        gCurrentSprite.work2 = 3;
    } else {
        scoreType = 10;
        gCurrentSprite.work2 = 7;
    }
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, scoreType);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void SpikyDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    SpikyDefeatedInit();
}

void SpikyDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    SpikyDefeatedInit();
}

void SpikyPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work2 = 6;
    } else {
        gCurrentSprite.work2 = 8;
    }
}

void SpikyPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    SpikyPushedInit();
}

void SpikyPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    SpikyPushedInit();
}

void SpikyPose27Or29Init(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work2 = 6;
    } else {
        gCurrentSprite.work2 = 8;
    }
}

void SpikyPose27Init(void)
{
    gCurrentSprite.pose = SPOSE_28;
    SpikyPose27Or29Init();
}

void SpikyPose29Init(void)
{
    gCurrentSprite.pose = SPOSE_2A;
    SpikyPose27Or29Init();
}

void SpikyPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 36;
}

void SpikyPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    SpikyPose2BOr2DInit();
}

void SpikyPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    SpikyPose2BOr2DInit();
}

void SpikyCrushed(void)
{
    int globalID;
    int scoreType;

    GP_SET(gCurrentSprite.pOamData, sSpikyDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 7;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    globalID = gCurrentSprite.globalID;
    scoreType = 10;
    if (globalID == 29) {
        scoreType = 30;
    }
    SpawnEnemyScoreDrop(globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, scoreType);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void SpikyInstantlyKill(void)
{
    int globalID;
    int scoreType;

    globalID = gCurrentSprite.globalID;
    scoreType = 10;
    if (globalID == 29) {
        scoreType = 30;
    }
    SpawnEnemyScoreDrop(globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, scoreType);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void SpikyStunnedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 36) {
        gCurrentSprite.warioCollision = 36;
    }
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work2 = 6;
    } else {
        gCurrentSprite.work2 = 8;
    }
}

void SpikyPose33Init(void)
{
    gCurrentSprite.pose = SPOSE_34;
    SpikyStunnedInit();
}

void SpikyPose35Init(void)
{
    gCurrentSprite.pose = SPOSE_36;
    SpikyStunnedInit();
}

void SpikyStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 36;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work2 = 6;
    } else {
        gCurrentSprite.work2 = 8;
    }
}

void SpikyPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    SpikyStunnedWithoutTimerInit();
}

void SpikyPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    SpikyStunnedWithoutTimerInit();
}

void SpikyAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpikyAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 49;
}

void SpikyAirborne(void)
{
    register struct PrimarySpriteData *sprite PIN("r1");
    register u8 *timerPointer PIN("r3");
    int timerCopy;
    u8 timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    timerCopy = --(*timerPointer);
    if (timerCopy != 0) {
        timer = *timerPointer;
        ASM_BARRIER(: "+r"(timer));
        timerCopy = timer;
        ASM_BARRIER(: "+r"(timerCopy));
        if (timer == 38 || timer == 33) {
            sprite->yPosition -= 16;
        } else if (timer == 29) {
            sprite->yPosition -= 16;
            sprite->warioCollision = 36;
        } else if (timer == 24 || timer == 20 || timerCopy == 15) {
            sprite->yPosition += 16;
        }
    } else {
        GP_SET(sprite->pOamData, sSpikyStunnedOam);
        sprite->animationTimer = timerCopy;
        sprite->currentAnimationFrame = 0;
        sprite->pose = SPOSE_2F;
    }
}

void SpriteSpiky(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            SpikyInit();
            break;
        case SPOSE_IDLE_INIT:
            SpikyIdleInit();
        case SPOSE_IDLE:
            SpikyIdle();
            SpikyCheckScreenShake();
            break;
        case SPOSE_11:
            SpikyTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            SpikyTurn();
            SpikyCheckScreenShake();
            break;
        case SPOSE_17:
            SpikyRecoverInit();
        case SPOSE_18:
            SpikyRecover();
            break;
        case SPOSE_19:
            SpikyGetUpInit();
        case SPOSE_1A:
            SpikyGetUp();
            break;
        case SPOSE_1B:
            SpikyFallingInit();
        case SPOSE_1C:
            func_8023D48();
            break;
        case SPOSE_1D:
            SpikyStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            SpikyTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            SpikyTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            if (gCurrentSprite.warioCollision == 36) {
                SpikyPose33Init();
                func_8024988();
                break;
            }
            SpikyPushedRightInit();
            goto pushedRight;
        case SPOSE_PUSHED_LEFT_INIT:
            if (gCurrentSprite.warioCollision == 36) {
                SpikyPose35Init();
                func_802476C();
                break;
            }
            SpikyPushedLeftInit();
            goto pushedLeft;
        case SPOSE_27:
            if (gCurrentSprite.warioCollision == 36) {
                SpikyPose33Init();
                func_8024988();
                break;
            }
            SpikyPose27Init();
        case SPOSE_PUSHED_RIGHT:
        case SPOSE_28:
pushedRight:
            SpriteUtilPushedRight();
            SpikyCheckScreenShake();
            break;
        case SPOSE_29:
            if (gCurrentSprite.warioCollision == 36) {
                SpikyPose35Init();
                func_802476C();
                break;
            }
            SpikyPose29Init();
        case SPOSE_PUSHED_LEFT:
        case SPOSE_2A:
pushedLeft:
            SpriteUtilPushedLeft();
            SpikyCheckScreenShake();
            break;
        case SPOSE_2B:
            SpikyPose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            SpikyPose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            SpikyCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            SpikyPose33Init();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            SpikyPose35Init();
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
            SpikyPose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            SpikyPose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            SpikyAirborneInit();
        case SPOSE_4C:
            SpikyAirborne();
            break;
        case SPOSE_69:
            SpikyDefeatedLeftInit();
            break;
        case SPOSE_6A:
            SpikyDefeatedRightInit();
            break;
        default:
            SpikyInstantlyKill();
            break;
    }
}

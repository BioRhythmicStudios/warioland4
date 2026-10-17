#include "sprite_ai/marumen.h"

#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

void MarumenInit(void)
{
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
    SpriteUtilTurnTowardWario();
    if (gCurrentSprite.globalID == PSPRITE_MARUMEN_RED) {
        gCurrentSprite.palette++;
    }
}

void MarumenIdleInit(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sMarumenIdleOam);
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = 1;
}

void MarumenIdle(void)
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

    func_80263AC();
}

void MarumenTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenTurningOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 17;
}

void MarumenTurn(void)
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
        GP_SET(gCurrentSprite.pOamData, sMarumenTurningAroundOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_14;
        gCurrentSprite.work0 = 32;
        gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
    } else {
        gCurrentSprite.pose = SPOSE_IDLE_INIT;
    }
}

void MarumenRecoverInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenRecoveringOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.warioCollision = 1;
}

void MarumenGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = 75;
    gCurrentSprite.work3 = 0;
}

void MarumenGetUp(void)
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
    table = sMarumenGettingUpYVelocity;
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
        if (work0 > 63) {
            func_80238E8();
            func_8023B88();
            if (!gSpriteCollisionResult) {
                current->pose = SPOSE_1D;
            }
        } else if (work0 <= 33) {
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

void MarumenFallingInit(void)
{
    u8 zero8;

    GP_SET(gCurrentSprite.pOamData, sMarumenFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    zero8 = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = zero8;
    gCurrentSprite.warioCollision = 1;
}

void MarumenStunnedFallingInit(void)
{
    u8 zero8;

    GP_SET(gCurrentSprite.pOamData, sMarumenStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    zero8 = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = zero8;
    gCurrentSprite.warioCollision = 5;
}

void MarumenTackledInit(void)
{
    u32 globalId;
    u32 effect;

    GP_SET(gCurrentSprite.pOamData, sMarumenDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    globalId = gCurrentSprite.globalID;
    effect = 2;
    if (globalId == PSPRITE_MARUMEN_RED) {
        effect = 5;
    }
    SpawnEnemyScoreDrop(globalId, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, effect);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void MarumenTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    MarumenTackledInit();
}

void MarumenTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    MarumenTackledInit();
}

void MarumenDefeatedInit(void)
{
    u32 globalId;
    u32 effect;

    GP_SET(gCurrentSprite.pOamData, sMarumenDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    globalId = gCurrentSprite.globalID;
    effect = 2;
    if (globalId == PSPRITE_MARUMEN_RED) {
        effect = 5;
    }
    SpawnEnemyScoreDrop(globalId, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, effect);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void MarumenDefeatedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    MarumenDefeatedInit();
}

void MarumenDefeatedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    MarumenDefeatedInit();
}

void MarumenPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
}

void MarumenPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    MarumenPushedInit();
}

void MarumenPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    MarumenPushedInit();
}

void MarumenPose2BOr2DInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void MarumenPose2BInit(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    MarumenPose2BOr2DInit();
}

void MarumenPose2DInit(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    MarumenPose2BOr2DInit();
}

void MarumenCrushed(void)
{
    struct PrimarySpriteData *sprite;

    {
        struct PrimarySpriteData *current;
        u32 collision;

        current = &gCurrentSprite;
        collision = current->warioCollision;
        sprite = current;
        if (collision == 5) {
            GP_SET(sprite->pOamData, sMarumenCrushedOam);
        } else {
            GP_SET(sprite->pOamData, sMarumenAlternateCrushedOam);
        }
    }
    {
        u8 *work0;

        work0 = &sprite->work0;
        *work0 = 10;
    }
    {
        register struct PrimarySpriteData *state PIN("r2");

        state = sprite;
        {
            u32 zero;

            zero = 0;
            state->currentAnimationFrame = zero;
            state->animationTimer = zero;
            state->pose = SPOSE_CRUSHED_OR_COLLECTED;
        }
        {
            register u16 oldStatus PIN("r1");
            u16 newStatus;
            u16 deadBit;

            oldStatus = state->status;
            newStatus = oldStatus & (u16)~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            deadBit = SPRITE_STATUS_MAYBE_DEAD;
            oldStatus = deadBit;
            newStatus |= oldStatus;
            state->status = newStatus;
        }
        {
            u32 globalId;
            u32 effect;

            globalId = state->globalID;
            effect = 2;
            if (globalId == PSPRITE_MARUMEN_RED) {
                effect = 5;
            }
            {
                u16 yPosition;
                u16 xPosition;

                yPosition = *(volatile u16 *)&sprite->yPosition;
                xPosition = sprite->xPosition;
                SpawnEnemyScoreDrop(globalId, yPosition, xPosition, effect);
            }
        }
    }
    gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void MarumenInstantlyKill(void)
{
    u32 globalId;
    u32 effect;

    globalId = gCurrentSprite.globalID;
    effect = 2;
    if (globalId == PSPRITE_MARUMEN_RED) {
        effect = 5;
    }
    SpawnEnemyScoreDrop(globalId, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, effect);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void MarumenStunnedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
    }
}

void MarumenPose33Init(void)
{
    gCurrentSprite.pose = SPOSE_34;
    MarumenStunnedInit();
}

void MarumenPose35Init(void)
{
    gCurrentSprite.pose = SPOSE_36;
    MarumenStunnedInit();
}

void MarumenStunnedWithoutTimerInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void MarumenPose47Init(void)
{
    gCurrentSprite.pose = SPOSE_48;
    MarumenStunnedWithoutTimerInit();
}

void MarumenPose49Init(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    MarumenStunnedWithoutTimerInit();
}

void MarumenAirborneInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = 71;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
    m4aSongNumStart(SOUND_62);
    if (gCurrentSprite.globalID == PSPRITE_MARUMEN_PURPLE) {
        gCurrentSprite.globalID = PSPRITE_MARUMEN_RED;
        gCurrentSprite.palette++;
    } else {
        gCurrentSprite.globalID = PSPRITE_MARUMEN_PURPLE;
        gCurrentSprite.palette = 0;
    }
}

void MarumenAirborne(void)
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
    table = sMarumenAirborneYVelocity;
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
        if (work0 > 67) {
            func_8023BFC(current->yPosition - current->hitboxExtentUp,
                current->xPosition);
            if (gSpriteCollisionTileType & 0xF) {
                current->pose = SPOSE_1D;
            }
        } else if (work0 <= 27) {
            func_8023B88();
            if (!gSpriteCollisionResult) {
                current->pose = SPOSE_1B;
            }
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

void MarumenHeldInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sMarumenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void MarumenLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
    MarumenHeldInit();
}

void MarumenLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
    MarumenHeldInit();
}

void MarumenCarriedInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sMarumenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void MarumenCarriedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    MarumenCarriedInit();
}

void MarumenCarriedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    MarumenCarriedInit();
}

void SpriteMarumen(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            MarumenInit();
        case SPOSE_IDLE_INIT:
            MarumenIdleInit();
        case SPOSE_IDLE:
            MarumenIdle();
            break;
        case SPOSE_11:
            MarumenTurnInit();
        case SPOSE_12:
        case SPOSE_14:
            MarumenTurn();
            break;
        case SPOSE_17:
            MarumenRecoverInit();
        case SPOSE_18:
            func_8023C94();
            break;
        case SPOSE_19:
            MarumenGetUpInit();
        case SPOSE_1A:
            MarumenGetUp();
            break;
        case SPOSE_1B:
            MarumenFallingInit();
        case SPOSE_1C:
            func_8023D48();
            break;
        case SPOSE_1D:
            MarumenStunnedFallingInit();
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            MarumenTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            MarumenTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            MarumenPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;
        case SPOSE_PUSHED_LEFT_INIT:
            MarumenPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            MarumenPose2BInit();
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            MarumenPose2DInit();
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            MarumenCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            MarumenPose33Init();
        case SPOSE_34:
            func_8024988();
            break;
        case SPOSE_35:
            MarumenPose35Init();
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
            MarumenPose47Init();
        case SPOSE_48:
            func_8024AD4();
            break;
        case SPOSE_49:
            MarumenPose49Init();
        case SPOSE_4A:
            func_8024C00();
            break;
        case SPOSE_4B:
            MarumenAirborneInit();
        case SPOSE_4C:
            MarumenAirborne();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            MarumenLiftedRightInit();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            MarumenLiftedLeftInit();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            MarumenCarriedRightInit();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            MarumenCarriedLeftInit();
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
            MarumenDefeatedLeftInit();
            break;
        case SPOSE_6A:
            MarumenDefeatedRightInit();
            break;
        default:
            MarumenInstantlyKill();
            break;
    }
}

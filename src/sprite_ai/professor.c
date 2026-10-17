
#include "main.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

extern const struct AnimationFrame sProfessorWalkOam[];
extern const struct AnimationFrame sProfessorTurnOam[];
extern const struct AnimationFrame sProfessorAirborneOam[];
extern const struct AnimationFrame sProfessorThrownRightOam[];
extern const struct AnimationFrame sProfessorThrownLeftOam[];
extern const struct AnimationFrame sProfessorWaitOam[];
extern const struct AnimationFrame sProfessorFallOam[];
extern const struct AnimationFrame sProfessorSplashOam[];
extern const struct AnimationFrame sProfessorScoreDigitTwoOrSevenOam[];
extern const struct AnimationFrame sProfessorUnderwaterPatrolOam[];
extern const struct AnimationFrame sProfessorScoreDigitFourOrNineOam[];
extern const s16 sProfessorPatrolYOffsets[];
extern const struct AnimationFrame sProfessorProjectileOam[];

void ProfessorInit(void)
{
    s8 scoreDigit;

    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 52;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 28;
    gCurrentSprite.hitboxExtentRight = 24;

    if (gUnk_300001B != 0) {
        gCurrentSprite.disableWarioCollisionTimer = 0xFF;
        GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = 30;
        gCurrentSprite.work3 = 0;
        gCurrentSprite.warioCollision = 0;
        gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
        gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
        gCurrentSprite.palette = 13;
        return;
    }

    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    scoreDigit = gScoreDigits[1];
    switch (scoreDigit) {
        case 2:
        case 7:
            GP_SET(gCurrentSprite.pOamData, sProfessorScoreDigitTwoOrSevenOam);
            break;

        case 4:
        case 9:
            GP_SET(gCurrentSprite.pOamData, sProfessorScoreDigitFourOrNineOam);
            break;

        default:
            GP_SET(gCurrentSprite.pOamData, sProfessorWalkOam);
            SpriteUtilTurnTowardWario();
            break;
    }
}

void ProfessorPatrolAndSpawnProjectiles(void)
{
    struct PrimarySpriteData* sprite;
    register u32 facingRightMask PIN("r5");
    u32 facingRightTest;
    register u32 spriteStatus PIN("r1");

    sprite = &gCurrentSprite;
    spriteStatus = sprite->status;
    facingRightMask = SPRITE_STATUS_FACING_RIGHT;
    facingRightTest = facingRightMask;
    if (facingRightTest & spriteStatus) {
        func_8023BFC(sprite->yPosition - 32, sprite->xPosition + 48);
        if (gSpriteCollisionTileType & 0xF) {
            sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
            return;
        }
        if ((gMainTimer & 0x3F) == 0) {
            SpawnPrimarySpriteWithStatus(PSPRITE_PROFESSOR_PROJECTILE, 0, 0, sprite->yPosition - 60,
                sprite->xPosition + 16, 0);
        }
    } else {
        func_8023BFC(sprite->yPosition - 32, sprite->xPosition - 48);
        if (gSpriteCollisionTileType & 0xF) {
            u32 updatedStatus;
            u32 facingRightFlag;

            updatedStatus = sprite->status;
            facingRightFlag = SPRITE_STATUS_FACING_RIGHT;
            updatedStatus |= facingRightFlag;
            sprite->status = updatedStatus;
            return;
        }
        if ((gMainTimer & 0x3F) == 0) {
            SpawnPrimarySpriteWithStatus(PSPRITE_PROFESSOR_PROJECTILE, 0, 0, sprite->yPosition - 60,
                sprite->xPosition - 16, facingRightMask);
        }
    }

    {
        struct PrimarySpriteData* movingSprite;
        const u16* verticalOffsetTable;
        u16 verticalOffset;
        u32 verticalOffsetIndex;
        u8* verticalOffsetIndexPointer;
        const u16* verticalOffsetEntry;
        s16 signedVerticalOffset;

        movingSprite = &gCurrentSprite;
        if ((*(u32*)&movingSprite->animationTimer & 0xFFFFFF) == 0x10000)
            movingSprite->work3 = 0;

        verticalOffsetIndexPointer = &movingSprite->work3;
        verticalOffsetIndex = *verticalOffsetIndexPointer;
        verticalOffsetTable = (const u16*)sProfessorPatrolYOffsets;
        verticalOffsetEntry = (const u16*)((verticalOffsetIndex << 1) + (u32)verticalOffsetTable);
        verticalOffset = *verticalOffsetEntry;
        signedVerticalOffset = *(const s16*)verticalOffsetEntry;
        if (signedVerticalOffset == 0x7FFF) {
            u32 updatedYPosition;
            register const u16* previousOffsetEntry PIN("r1");

            previousOffsetEntry = (const u16*)(((verticalOffsetIndex - 1) << 1) + (u32)verticalOffsetTable);
            updatedYPosition = movingSprite->yPosition;
            updatedYPosition += *previousOffsetEntry;
            movingSprite->yPosition = updatedYPosition;
        } else {
            u32 nextOffsetIndex;
            u8* offsetIndexStore;

            nextOffsetIndex = verticalOffsetIndex + 1;
            offsetIndexStore = verticalOffsetIndexPointer;
            *offsetIndexStore = nextOffsetIndex;
            {
                u16 yPosition;
                yPosition = movingSprite->yPosition;
                yPosition += verticalOffset;
                movingSprite->yPosition = yPosition;
            }
        }
    }
    SpriteUtilMoveForward1Subpixel();
}

void ProfessorWalkInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorWalkOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 1;
}

void ProfessorWalk(void)
{
    register u32 floorCollision PIN("r2");
    struct PrimarySpriteData* sprite;

    func_80238E8();
    func_8023B88();
    floorCollision = gSpriteCollisionResult;
    if (floorCollision == 0) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (gSpriteCollisionTileType == 0) {
            gCurrentSprite.pose = 27;
            goto end;
        }
        goto moveForward;
    }

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        register u32 collisionMask PIN("r6");
        register u32 maskedCollision PIN("r0");
        register u32 probeCollision PIN("r1");
        u8* probeCollisionPointer;

        collisionMask = 0xF0;
        maskedCollision = collisionMask;
        if ((maskedCollision & floorCollision) == 0)
            goto moveForward;
        if (((sprite->xPosition & 0x3F) + sprite->hitboxExtentRight) <= 0x3F)
            goto moveForward;

        func_8023BFC(sprite->yPosition, sprite->xPosition + 70);
        probeCollisionPointer = &gSpriteCollisionTileType;
        probeCollision = *probeCollisionPointer;
        maskedCollision = collisionMask;
        maskedCollision &= probeCollision;
        if (maskedCollision == 0)
            goto setPose17;

        func_8023BFC(sprite->yPosition - 32, sprite->xPosition + 70);
        if ((*probeCollisionPointer & 0xF) == 0)
            goto moveForward;
        goto setPose17;
    } else {
        register u32 collisionMask PIN("r6");
        register u32 maskedCollision PIN("r0");
        register u32 probeCollision PIN("r1");
        u8* probeCollisionPointer;

        collisionMask = 0xF0;
        maskedCollision = collisionMask;
        if ((maskedCollision & floorCollision) == 0)
            goto moveForward;
        if ((sprite->xPosition & 0x3F) >= sprite->hitboxExtentLeft)
            goto moveForward;

        func_8023BFC(sprite->yPosition, sprite->xPosition - 70);
        probeCollisionPointer = &gSpriteCollisionTileType;
        probeCollision = *probeCollisionPointer;
        maskedCollision = collisionMask;
        maskedCollision &= probeCollision;
        if (maskedCollision == 0)
            goto setPose17;

        func_8023BFC(sprite->yPosition - 32, sprite->xPosition - 70);
        if ((*probeCollisionPointer & 0xF) == 0)
            goto moveForward;
    }

setPose17:
    sprite->pose = 17;
    goto end;

moveForward:
    if (GP(gCurrentSprite.pOamData) == sProfessorWalkOam)
        SpriteUtilMoveForward1Subpixel();

end:
    return;
}

void ProfessorTurnAroundInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorTurnOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 22;
}

void ProfessorTurnAround(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
        return;
    }

    if (--gCurrentSprite.work0 != 0)
        return;

    if (gCurrentSprite.pose == 18) {
        gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
        GP_SET(gCurrentSprite.pOamData, sProfessorTurnOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = 20;
        gCurrentSprite.work0 = 21;
    } else {
        GP_SET(gCurrentSprite.pOamData, sProfessorWalkOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = 16;
    }
}

void ProfessorKnockbackCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 6;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5)
        gCurrentSprite.warioCollision = 5;
}

void ProfessorKnockedRightInit(void)
{
    gCurrentSprite.work2 = 8;
    gCurrentSprite.pose = 52;
    ProfessorKnockbackCommon();
}

void ProfessorKnockedLeftInit(void)
{
    gCurrentSprite.work2 = 8;
    gCurrentSprite.pose = 54;
    ProfessorKnockbackCommon();
}

void ProfessorTackledRightInit(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition - 32, 6);
    m4aSongNumStart(SE_PROFESSOR_HIT);
    if (sprite->pose == 106)
        sprite->work2 = 8;
    else
        sprite->work2 = 12;
    gCurrentSprite.pose = 52;
    ProfessorKnockbackCommon();
}

void ProfessorTackledLeftInit(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition + 32, 6);
    m4aSongNumStart(SE_PROFESSOR_HIT);
    if (sprite->pose == 105)
        sprite->work2 = 8;
    else
        sprite->work2 = 12;
    gCurrentSprite.pose = 54;
    ProfessorKnockbackCommon();
}

void ProfessorWaitInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorWaitOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 40;
}

void ProfessorWait(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 15;
    }
}

void ProfessorHopInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.work0 = 8;
    gCurrentSprite.warioCollision = 1;
}

void ProfessorFallInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorFallOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
}

void ProfessorLandingInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void ProfessorPushedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorTurnOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void ProfessorPushedRightInit(void)
{
    gCurrentSprite.pose = 36;
    ProfessorPushedCommon();
}

void ProfessorPushedLeftInit(void)
{
    gCurrentSprite.pose = 38;
    ProfessorPushedCommon();
}

void ProfessorImpactCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void ProfessorImpactRightInit(void)
{
    gCurrentSprite.pose = 44;
    ProfessorImpactCommon();
}

void ProfessorImpactLeftInit(void)
{
    gCurrentSprite.pose = 46;
    ProfessorImpactCommon();
}

void ProfessorKnockAwayFromWario(void)
{
    m4aSongNumStart(SE_PROFESSOR_HIT);
    if (gCurrentSprite.xPosition > gWarioData.xPosition)
        ProfessorKnockedRightInit();
    else
        ProfessorKnockedLeftInit();
}

void ProfessorThrownUpCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void ProfessorThrownUpRightInit(void)
{
    gCurrentSprite.pose = 72;
    ProfessorThrownUpCommon();
}

void ProfessorThrownUpLeftInit(void)
{
    gCurrentSprite.pose = 74;
    ProfessorThrownUpCommon();
}

void ProfessorLiftedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorWaitOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
}

void ProfessorLifted(void)
{
    struct PrimarySpriteData* sprite;
    const u16* gravityTable;
    u16 verticalOffset;
    u32 gravityIndex;
    u8* gravityIndexPointer;
    const u16* gravityEntry;
    s16 signedVerticalOffset;
    u8* liftTimerPointer;

    sprite = &gCurrentSprite;
    liftTimerPointer = &sprite->work0;
    if (--*liftTimerPointer != 0) {
        func_8023BFC(sprite->yPosition - sprite->hitboxExtentUp,
            sprite->xPosition);
        if (gSpriteCollisionTileType & 0xF) {
            sprite->pose = 29;
        } else {
            gravityIndexPointer = &sprite->work3;
            gravityIndex = *gravityIndexPointer;
            gravityTable = (const u16*)sUnk_8352B18;
            gravityEntry = (const u16*)((gravityIndex << 1) + (u32)gravityTable);
            verticalOffset = *gravityEntry;
            signedVerticalOffset = *(const s16*)gravityEntry;
            if (signedVerticalOffset == 0x7FFF) {
                u32 updatedYPosition;
                register const u16* previousGravityEntry PIN("r1");

                previousGravityEntry = (const u16*)(((gravityIndex - 1) << 1) + (u32)gravityTable);
                updatedYPosition = sprite->yPosition;
                updatedYPosition += *previousGravityEntry;
                sprite->yPosition = updatedYPosition;
            } else {
                u32 nextGravityIndex;
                u8* gravityIndexStore;

                nextGravityIndex = gravityIndex + 1;
                gravityIndexStore = gravityIndexPointer;
                *gravityIndexStore = nextGravityIndex;
                {
                    u16 yPosition;
                    yPosition = sprite->yPosition;
                    yPosition += verticalOffset;
                    sprite->yPosition = yPosition;
                }
            }
        }
    } else {
        sprite->pose = 27;
    }
}

void ProfessorLiftedByWarioCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void ProfessorLiftedRightInit(void)
{
    gCurrentSprite.pose = 82;
    ProfessorLiftedByWarioCommon();
}

void ProfessorLiftedLeftInit(void)
{
    gCurrentSprite.pose = 84;
    ProfessorLiftedByWarioCommon();
}

void ProfessorCarriedCommon(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sProfessorAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    if (gCurrentSprite.status | SPRITE_STATUS_MAYBE_DEAD)
        gCurrentSprite.status &= ~SPRITE_STATUS_MAYBE_DEAD;
}

void ProfessorCarriedRightInit(void)
{
    gCurrentSprite.pose = 88;
    ProfessorCarriedCommon();
}

void ProfessorCarriedLeftInit(void)
{
    gCurrentSprite.pose = 86;
    ProfessorCarriedCommon();
}
void SpriteProfessor(void)
{
    struct PrimarySpriteData* sprite;
    struct PrimarySpriteData* currentSprite;
    struct PrimarySpriteData* damagedSprite;
    u8 collisionState;

    currentSprite = &gCurrentSprite;
    collisionState = currentSprite->health & 0xF;
    sprite = currentSprite;
    if (collisionState == 2 && sprite->pose == 48) {
        ProfessorPatrolAndSpawnProjectiles();
        return;
    }

    if ((sprite->status & SPRITE_STATUS_UNDERWATER) && (gStageFrameCounter & 3))
        return;

    switch (sprite->pose) {
        case 0:
            ProfessorInit();
            goto afterPose;

        case 15:
            ProfessorWalkInit();
            /* fallthrough */
        case 16:
            ProfessorWalk();
            goto afterPose;

        case 17:
            ProfessorTurnAroundInit();
            /* fallthrough */
        case 18:
        case 20:
            ProfessorTurnAround();
            goto afterPose;

        case 23:
            ProfessorWaitInit();
            /* fallthrough */
        case 24:
            ProfessorWait();
            goto afterPose;

        case 25:
            ProfessorHopInit();
            goto afterPose;

        case 27:
            ProfessorFallInit();
            /* fallthrough */
        case 28:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8023D48();
            goto afterPose;

        case 29:
            ProfessorLandingInit();
            /* fallthrough */
        case 30:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8023EE0();
            goto afterPose;

        case 35:
            ProfessorPushedRightInit();
            /* fallthrough */
        case 36:
            SpriteUtilPushedRight();
            goto afterPose;

        case 37:
            ProfessorPushedLeftInit();
            /* fallthrough */
        case 38:
            SpriteUtilPushedLeft();
            goto afterPose;

        case 43:
            ProfessorImpactRightInit();
            /* fallthrough */
        case 44:
            func_8024478();
            goto afterPose;

        case 45:
            ProfessorImpactLeftInit();
            /* fallthrough */
        case 46:
            func_802449C();
            goto afterPose;

        case 47:
            func_80244C0();
            if ((gCurrentSprite.health & 0xF) == 2) {
                GP_SET(gCurrentSprite.pOamData, sProfessorUnderwaterPatrolOam);
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                gCurrentSprite.work3 = 0;
            }
            /* fallthrough */
        case 48:
            if ((gCurrentSprite.health & 0xF) == 2)
                ProfessorPatrolAndSpawnProjectiles();
            else
                func_80244E0();
            goto afterPose;

        case 31:
        case 79:
        case 106:
            ProfessorTackledRightInit();
            goto callA3C;

        case 52:
        case 66:
callA3C:
            func_8024A3C();
            goto afterPose;

        case 33:
        case 80:
        case 105:
            ProfessorTackledLeftInit();
            goto call4820;

        case 54:
        case 60:
call4820:
            func_8024820();
            goto afterPose;

        case 51:
            ProfessorKnockedRightInit();
            goto callA3C;

        case 53:
            ProfessorKnockedLeftInit();
            goto call4820;

        case 55:
            func_8024688();
            /* fallthrough */
        case 56:
            func_80246B8();
            goto afterPose;

        case 57:
            func_802473C();
            /* fallthrough */
        case 58:
            func_802476C();
            goto afterPose;

        case 59:
            func_80247F0();
            goto call4820;

        case 61:
            func_80248A4();
            /* fallthrough */
        case 62:
            func_80248D4();
            goto afterPose;

        case 63:
            func_8024958();
            /* fallthrough */
        case 64:
            func_8024988();
            goto afterPose;

        case 65:
            func_8024A0C();
            goto callA3C;

        case 67:
            func_8024AC0();
            goto clearUnderwaterAndCall4AD4;

        case 69:
            func_8024BEC();
            goto clearUnderwaterAndCall4C00;

        case 71:
            ProfessorThrownUpRightInit();
            /* fallthrough */
        case 68:
        case 72:
clearUnderwaterAndCall4AD4:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8024AD4();
            goto afterPose;

        case 73:
            ProfessorThrownUpLeftInit();
            /* fallthrough */
        case 70:
        case 74:
clearUnderwaterAndCall4C00:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8024C00();
            goto afterPose;

        case 75:
            ProfessorLiftedInit();
            /* fallthrough */
        case 26:
        case 76:
            ProfessorLifted();
            goto afterPose;

        case 81:
            ProfessorLiftedRightInit();
            goto afterPose;

        case 82:
            SpriteUtilLiftingSpriteRight();
            goto afterPose;

        case 83:
            ProfessorLiftedLeftInit();
            goto afterPose;

        case 84:
            SpriteUtilLiftingSpriteLeft();
            goto afterPose;

        case 87:
            ProfessorCarriedRightInit();
            goto afterPose;

        case 88:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilCarryingSpriteRight();
            goto afterPose;

        case 85:
            ProfessorCarriedLeftInit();
            goto afterPose;

        case 86:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilCarryingSpriteLeft();
            goto afterPose;

        case 89:
            sprite->pose = 90;
            /* fallthrough */
        case 90:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownLeftSoft();
            goto afterPose;

        case 93:
            sprite->pose = 94;
            sprite->health = (sprite->health & 0xF) | 0x10;
            GP_SET(sprite->pOamData, sProfessorThrownLeftOam);
            sprite->animationTimer = 0;
            sprite->currentAnimationFrame = 0;
            /* fallthrough */
        case 94:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownLeftHard();
            goto afterPose;

        case 97:
            sprite->pose = 98;
            /* fallthrough */
        case 98:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpLeftSoft();
            goto afterPose;

        case 101:
            sprite->pose = 102;
            sprite->health = (sprite->health & 0xF) | 0x20;
            GP_SET(sprite->pOamData, sProfessorThrownLeftOam);
            sprite->animationTimer = 0;
            sprite->currentAnimationFrame = 0;
            /* fallthrough */
        case 102:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpLeftHard();
            goto afterPose;

        case 91:
            sprite->pose = 92;
            /* fallthrough */
        case 92:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownRightSoft();
            goto afterPose;

        case 95:
            sprite->pose = 96;
            sprite->health = (sprite->health & 0xF) | 0x40;
            GP_SET(sprite->pOamData, sProfessorThrownRightOam);
            sprite->animationTimer = 0;
            sprite->currentAnimationFrame = 0;
            /* fallthrough */
        case 96:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownRightHard();
            goto afterPose;

        case 99:
            sprite->pose = 100;
            /* fallthrough */
        case 100:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpRightSoft();
            goto afterPose;

        case 103:
            sprite->pose = 104;
            sprite->health = (sprite->health & 0xF) | 0x80;
            GP_SET(sprite->pOamData, sProfessorThrownRightOam);
            sprite->animationTimer = 0;
            sprite->currentAnimationFrame = 0;
            /* fallthrough */
        case 104:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpRightHard();
            goto afterPose;

        case 107: {
            damagedSprite = &gCurrentSprite;
            if (damagedSprite->health & 0x10) {
                m4aSongNumStart(SE_PROFESSOR_HIT);
                damagedSprite->health = damagedSprite->health & 0xF;
                {
                    u8* work2;
                    work2 = &damagedSprite->work2;
                    *work2 = 8;
                }
                func_8024BEC();
            } else if (damagedSprite->health & 0x20) {
                m4aSongNumStart(SE_PROFESSOR_HIT);
                damagedSprite->health = damagedSprite->health & 0xF;
                {
                    u8* work2;
                    work2 = &damagedSprite->work2;
                    *work2 = 4;
                }
                func_8024AC0();
            } else if (damagedSprite->health & 0x40) {
                m4aSongNumStart(SE_PROFESSOR_HIT);
                damagedSprite->health = damagedSprite->health & 0xF;
                {
                    u8* work2;
                    work2 = &damagedSprite->work2;
                    *work2 = 8;
                }
                func_8024AC0();
            } else if (damagedSprite->health & 0x80) {
                m4aSongNumStart(SE_PROFESSOR_HIT);
                damagedSprite->health = damagedSprite->health & 0xF;
                {
                    u8* work2;
                    work2 = &damagedSprite->work2;
                    *work2 = 4;
                }
                func_8024BEC();
            } else {
                ProfessorKnockAwayFromWario();
            }
            goto afterPose;
        }

        default:
            ProfessorKnockAwayFromWario();
            break;
    }

afterPose:
    {
        struct PrimarySpriteData* spriteAfterState;
        u32 healthFlags;
        register u32 healthState PIN("r0");

        spriteAfterState = &gCurrentSprite;
        healthFlags = spriteAfterState->health;
        healthState = 0xF;
        healthState &= healthFlags;
        if (healthState == 1) {
            u32 zeroValue;
            u16 status;

            status = spriteAfterState->status;
            healthState = SPRITE_STATUS_UNDERWATER;
            healthState &= status;
            if (healthState == 0)
                return;

            healthState = healthFlags + 1;
            zeroValue = 0;
            spriteAfterState->health = healthState;
            SpawnSecondarySprite(spriteAfterState->yPosition, spriteAfterState->xPosition, SSPRITE_07);
            m4aSongNumStart(SE_ENTER_WATER);
            GP_SET(spriteAfterState->pOamData, sProfessorSplashOam);
            *(u8*)((u8*)spriteAfterState + 22) = zeroValue;
            *(u16*)((u8*)spriteAfterState + 20) = zeroValue;
            healthState = spriteAfterState->status;
            status = SPRITE_STATUS_BACKGROUND;
            healthState |= status;
            spriteAfterState->status = healthState;
            if (spriteAfterState->pose == 28) {
                u8* work3;

                spriteAfterState->pose = 30;
                work3 = &spriteAfterState->work3;
                healthState = 0;
                *work3 = healthState;
                spriteAfterState->warioCollision = 5;
            }
        } else if (healthState == 2) {
            u16 status;
            u16 underwater;

            func_8023BFC(spriteAfterState->yPosition, spriteAfterState->xPosition);
            if (gBackgroundCollisionData.waterType == 1)
                spriteAfterState->status |= SPRITE_STATUS_UNDERWATER;

            status = spriteAfterState->status;
            underwater = status & SPRITE_STATUS_UNDERWATER;
            if (underwater == 0) {
                u16 updatedStatus;

                spriteAfterState->health--;
                GP_SET(spriteAfterState->pOamData, sProfessorAirborneOam);
                spriteAfterState->currentAnimationFrame = underwater;
                spriteAfterState->animationTimer = underwater;
                updatedStatus = status & ~SPRITE_STATUS_BACKGROUND;
                spriteAfterState->status = updatedStatus;
                if (updatedStatus | SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) {
                    healthState = (u16)~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                    healthState &= updatedStatus;
                    spriteAfterState->status = healthState;
                }
            }
        }
    }

}

void SpriteProfessorProjectile(void)
{
    register struct PrimarySpriteData* projectile PIN("r12");
    u32 collisionDisableDuration;
    struct PrimarySpriteData* projectileState;
    u8 initialPose;

    projectile = &gCurrentSprite;
    collisionDisableDuration = 100;
    projectileState = projectile;
    projectileState->disableWarioCollisionTimer = collisionDisableDuration;
    initialPose = projectileState->pose;
    if (initialPose == 0) {
        u32 spriteStatus;
        register u32 ignoreCollisionFlag PIN("r2");
        register u32 statusWithIgnoredCollision PIN("r0");
        register u32 zeroValue PIN("r3");
        const struct AnimationFrame* projectileOam;
        u32 largeExtent;
        u32 smallExtent;
        u8* extentField;

        spriteStatus = projectileState->status;
        ignoreCollisionFlag = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
        /* Prevent agbcc from folding the shifted spriteStatus mask into the later copy. */
        ASM_BARRIER(: "+r"(ignoreCollisionFlag));
        statusWithIgnoredCollision = ignoreCollisionFlag;
        /* Keep the copied mask in r0 before agbcc materializes the shared zeroValue in r3. */
        ASM_BARRIER(: "+r"(statusWithIgnoredCollision));
        zeroValue = 0;
        statusWithIgnoredCollision |= spriteStatus;
        spriteStatus = 0xFFFB;
        statusWithIgnoredCollision &= spriteStatus;
        projectileState = projectile;
        projectileState->status = statusWithIgnoredCollision;
        projectileState->warioCollision = zeroValue;

        extentField = &projectile->drawDistanceDown;
        largeExtent = 32;
        *extentField++ = largeExtent;
        smallExtent = 16;
        *extentField++ = smallExtent;
        *extentField++ = largeExtent;
        largeExtent = 4;
        *extentField++ = largeExtent;
        *extentField++ = largeExtent;
        *extentField++ = largeExtent;
        /* Preserve the final extent pointer increment instead of folding it into [extentField, #1]. */
        *(volatile u8 *)extentField = largeExtent;

        projectileOam = sProfessorProjectileOam;
        projectileState = projectile;
        GP_SET(projectileState->pOamData, projectileOam);
        projectileState->currentAnimationFrame = zeroValue;
        projectileState->animationTimer = initialPose;
        *(u8*)((u8*)projectileState + 39) = 30;
        projectile->pose = smallExtent;
    }

    {
        u32 lifetime;
        register u8* lifetimePointer PIN("r1");

        lifetimePointer = &projectile->work0;
        lifetime = *lifetimePointer;
        lifetime--;
        *lifetimePointer = lifetime;
        lifetime = (lifetime << 24) >> 24;
        if (lifetime == 0) {
            projectileState = projectile;
            projectileState->status = lifetime;
        }
    }
}


#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

extern const struct AnimationFrame sKaentsuboIdleOam[];
extern const struct AnimationFrame sKaentsuboPushedFacingRightOam[];
extern const struct AnimationFrame sKaentsuboPushedFacingLeftOam[];
extern const struct AnimationFrame sKaentsuboSpitWindupOam[];
extern const struct AnimationFrame sKaentsuboStunnedOam[];
extern const struct AnimationFrame sKaentsuboRecoverOam[];
extern const struct AnimationFrame sKaentsuboHitReactOam[];
extern const struct AnimationFrame sKaentsuboTurnAroundOam[];
extern const struct AnimationFrame sKaentsuboAirborneOam[];
extern const struct AnimationFrame sKaentsuboDefeatedOam[];
extern const struct AnimationFrame sKaentsuboFlameIdleOam[];
extern const struct AnimationFrame sKaentsuboFlamePushedRightOam[];
extern const struct AnimationFrame sKaentsuboFlamePushedLeftOam[];
extern const struct AnimationFrame sKaentsuboFlameAlertOam[];
extern const struct AnimationFrame sKaentsuboFireballLaunchOam[];
extern const struct AnimationFrame sKaentsuboFireballTravelOam[];
extern const struct AnimationFrame sKaentsuboDefeatSmokeOam[];
extern const struct AnimationFrame sKaentsuboFireballSpawnOam[];


void InitKaentsubo(void)
{
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sKaentsuboIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    SpriteUtilTurnTowardWario();
    SpriteSpawnAsChild(PSPRITE_92, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                       gCurrentSprite.yPosition, gCurrentSprite.xPosition);
}

void InitKaentsuboIdle(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.work1 = 50;
}

void UpdateKaentsuboIdle(void)
{
    u32 warioSide;
    register u8 terrainCollision PIN("r2");
    struct PrimarySpriteData *currentSprite;

    warioSide = SpriteUtilWaitCheckWarioNearbyLeftRight(0x7F, 0xC0);
    func_80238A4();
    func_8023B88();
    terrainCollision = gSpriteCollisionResult;

    if (terrainCollision == 0) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition,
                         gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition,
                         gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (gSpriteCollisionTileType == 0) {
            gCurrentSprite.pose = 27;
            goto done;
        }
        goto fallback;
    }

    currentSprite = &gCurrentSprite;
    if (currentSprite->status & SPRITE_STATUS_FACING_RIGHT) {
        if (warioSide == NS_RIGHT) {
            currentSprite->pose = 19;
            goto done;
        }
        if (warioSide == NS_LEFT) {
            currentSprite->pose = 17;
            currentSprite->work1 = 7;
            goto done;
        }
        if ((terrainCollision & 0xF0) &&
            ((currentSprite->xPosition & 0x3F) + currentSprite->hitboxExtentRight > 0x3F)) {
            func_8023BFC(currentSprite->yPosition,
                         currentSprite->xPosition + currentSprite->hitboxExtentRight);
            if ((gSpriteCollisionTileType & 0xF0) == 0) {
                currentSprite->pose = 17;
                goto done;
            }
            func_8023BFC(currentSprite->yPosition - 32,
                         currentSprite->xPosition + currentSprite->hitboxExtentRight);
            if (gSpriteCollisionTileType & 0xF) {
                currentSprite->pose = 17;
                goto done;
            }
        }
    } else {
        if (warioSide == NS_LEFT) {
            currentSprite->pose = 19;
            goto done;
        }
        if (warioSide == NS_RIGHT) {
            currentSprite->pose = 17;
            currentSprite->work1 = 7;
            goto done;
        }
        if ((terrainCollision & 0xF0) &&
            ((currentSprite->xPosition & 0x3F) < currentSprite->hitboxExtentLeft)) {
            func_8023BFC(currentSprite->yPosition,
                         currentSprite->xPosition - currentSprite->hitboxExtentLeft);
            if ((gSpriteCollisionTileType & 0xF0) == 0) {
                currentSprite->pose = 17;
                goto done;
            }
            func_8023BFC(currentSprite->yPosition - 32,
                         currentSprite->xPosition - currentSprite->hitboxExtentLeft);
            if (gSpriteCollisionTileType & 0xF) {
                currentSprite->pose = 17;
                goto done;
            }
        }
    }

fallback:
    func_8026374();
done:
    return;
}

void InitKaentsuboSpitWindup(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboSpitWindupOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 20;
    gCurrentSprite.work0 = 52;
}

void UpdateKaentsuboSpitWindup(void)
{
    u16 isFacingRight;

    if (gCurrentSprite.work0 == 30) {
        gCurrentSprite.warioCollision = 5;
        isFacingRight = gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT;
        if (isFacingRight) {
            SpawnPrimarySpriteWithStatus(PSPRITE_93, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                         gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition + 64, 64);
        } else {
            SpawnPrimarySpriteWithStatus(PSPRITE_93, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                         gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition - 63, 0);
        }
    }

    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 29;
    } else if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 48;
        gCurrentSprite.work0 = 80;
        GP_SET(gCurrentSprite.pOamData, sKaentsuboStunnedOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
    }
}

void InitKaentsuboTurnAround(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboTurnAroundOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 22;
    gCurrentSprite.warioCollision = 1;
}

void UpdateKaentsuboTurnAround(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.warioCollision = 1;
        GP_SET(gCurrentSprite.pOamData, sKaentsuboIdleOam);
        gCurrentSprite.pose = 16;
        gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
    }
}

void InitKaentsuboHitReact(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboHitReactOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 15;
}

void InitKaentsuboRecover(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboRecoverOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 62;
}

void UpdateKaentsuboRecover(void)
{
    if (gCurrentSprite.work0 == 15) {
        gCurrentSprite.warioCollision = 1;
        SpriteSpawnAsChild(PSPRITE_92, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                           gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    }
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 29;
    } else if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 17;
        gCurrentSprite.work1 = 100;
    }
}

void InitKaentsuboAirborne(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboAirborneOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
}

void InitKaentsuboStunned(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void InitKaentsuboDefeatedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
}

void InitKaentsuboDefeatedRight(void)
{
    gCurrentSprite.pose = 32;
    InitKaentsuboDefeatedCommon();
}

void InitKaentsuboDefeatedLeft(void)
{
    gCurrentSprite.pose = 34;
    InitKaentsuboDefeatedCommon();
}

void InitKaentsuboThrownDefeatedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void InitKaentsuboThrownDefeatedRight(void)
{
    gCurrentSprite.pose = 32;
    InitKaentsuboThrownDefeatedCommon();
}

void InitKaentsuboThrownDefeatedLeft(void)
{
    gCurrentSprite.pose = 34;
    InitKaentsuboThrownDefeatedCommon();
}

void InitKaentsuboPushedCommon(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
}

void InitKaentsuboPushedRight(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboPushedFacingRightOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboPushedFacingLeftOam);
    }
    gCurrentSprite.pose = 36;
    InitKaentsuboPushedCommon();
}

void InitKaentsuboPushedLeft(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboPushedFacingLeftOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboPushedFacingRightOam);
    }
    gCurrentSprite.pose = 38;
    InitKaentsuboPushedCommon();
}

void InitKaentsuboBumpedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void InitKaentsuboBumpedRight(void)
{
    gCurrentSprite.pose = 44;
    InitKaentsuboBumpedCommon();
}

void InitKaentsuboBumpedLeft(void)
{
    gCurrentSprite.pose = 46;
    InitKaentsuboBumpedCommon();
}

void InitKaentsuboDefeatSmoke(void)
{
    if (gCurrentSprite.xPosition < gWarioData.xPosition) {
        SpawnPrimarySpriteWithStatus(PSPRITE_93, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                     gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 64);
    } else {
        SpawnPrimarySpriteWithStatus(PSPRITE_93, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                     gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 0);
    }
    GP_SET(gCurrentSprite.pOamData, sKaentsuboDefeatSmokeOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 21;
    gCurrentSprite.drawPriority = 0;
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
    if (gCurrentSprite.pose == 49) {
        gCurrentSprite.pose = 50;
    } else {
        gCurrentSprite.pose = 109;
    }
}

void KillKaentsubo(void)
{
    SpriteUtilDie();
}

void InitKaentsuboGroundBounceCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
    }
}

void InitKaentsuboGroundBounceRight(void)
{
    gCurrentSprite.pose = 52;
    InitKaentsuboGroundBounceCommon();
}

void InitKaentsuboGroundBounceLeft(void)
{
    gCurrentSprite.pose = 54;
    InitKaentsuboGroundBounceCommon();
}

void InitKaentsuboAirBounceCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void InitKaentsuboAirBounceRight(void)
{
    gCurrentSprite.pose = 72;
    InitKaentsuboAirBounceCommon();
}

void InitKaentsuboAirBounceLeft(void)
{
    gCurrentSprite.pose = 74;
    InitKaentsuboAirBounceCommon();
}

void InitKaentsuboDropped(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboIdleOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
}

void UpdateKaentsuboDropped(void)
{
    u8 dropTimer;

    dropTimer = --gCurrentSprite.work0;
    if (dropTimer != 0) {
        func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp,
                      gCurrentSprite.xPosition);
        if (gSpriteCollisionTileType & 0xF) {
            gCurrentSprite.pose = 29;
        } else {
            SpriteUtilLookupGravity(sUnk_8352B18);
        }
    } else {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboAirborneOam);
        gCurrentSprite.currentAnimationFrame = dropTimer;
        gCurrentSprite.animationTimer = dropTimer;
        gCurrentSprite.pose = 77;
        gCurrentSprite.work3 = 0;
    }
}

void UpdateKaentsuboLandAfterDrop(void)
{
    u32 groundY;

    groundY = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult != 0) {
        gCurrentSprite.yPosition = groundY;
        gCurrentSprite.pose = 17;
        gCurrentSprite.work1 = 7;
    } else {
        SpriteUtilLookupGravity(sUnk_8352A28);
    }
}

void InitKaentsuboLiftedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitKaentsuboLiftedRight(void)
{
    gCurrentSprite.pose = 82;
    InitKaentsuboLiftedCommon();
}

void InitKaentsuboLiftedLeft(void)
{
    gCurrentSprite.pose = 84;
    InitKaentsuboLiftedCommon();
}

void InitKaentsuboCarriedCommon(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sKaentsuboStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitKaentsuboCarriedRight(void)
{
    gCurrentSprite.pose = 88;
    InitKaentsuboCarriedCommon();
}

void InitKaentsuboCarriedLeft(void)
{
    gCurrentSprite.pose = 86;
    InitKaentsuboCarriedCommon();
}

void InitKaentsuboThrownCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboDefeatedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitKaentsuboFlame(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    sprite->status &= 0xFFFB;
    sprite->drawDistanceDown = 48;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 120;
    sprite->hitboxExtentDown = 208;
    sprite->hitboxExtentLeft = 24;
    sprite->hitboxExtentRight = 20;
    sprite->warioCollision = 14;
    GP_SET(sprite->pOamData, sKaentsuboFlameIdleOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitKaentsuboFlameIdle(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboFlameIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
}

void UpdateKaentsuboFlameFollowParent(u32 slot)
{
    switch (gSpriteData[slot].pose) {
        case 35:
        case 36:
            goto pose35;
        case 37:
        case 38:
            goto pose37;
        case 19:
        case 20:
            goto pose20;
        default:
            return;
    }

pose35:
    gCurrentSprite.pose = 35;
    return;
pose37:
    gCurrentSprite.pose = 37;
    return;
pose20:
    gCurrentSprite.pose = 20;
    GP_SET(gCurrentSprite.pOamData, sKaentsuboFlameAlertOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
}

void SyncKaentsuboFlamePushState(u32 slot)
{
    switch (gSpriteData[slot].pose) {
        case 35:
        case 36:
            gCurrentSprite.pose = 35;
            break;
        case 37:
        case 38:
            gCurrentSprite.pose = 37;
            break;
    }
}

void InitKaentsuboFlamePushedRight(u32 slot)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 36;
    if (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboFlamePushedRightOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboFlamePushedLeftOam);
    }
}

void UpdateKaentsuboFlamePushedRight(u32 slot)
{
    switch (gSpriteData[slot].pose) {
        case 17:
        case 18:
            gCurrentSprite.pose = 15;
            break;
        case 37:
        case 38:
            gCurrentSprite.pose = 37;
            break;
    }
}

void InitKaentsuboFlamePushedLeft(u32 slot)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 38;
    if (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboFlamePushedLeftOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sKaentsuboFlamePushedRightOam);
    }
}

void UpdateKaentsuboFlamePushedLeft(u32 slot)
{
    switch (gSpriteData[slot].pose) {
        case 17:
        case 18:
            gCurrentSprite.pose = 15;
            break;
        case 35:
        case 36:
            gCurrentSprite.pose = 35;
            break;
    }
}

void InitKaentsuboFireball(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    sprite->status &= 0xFFFB;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 40;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 24;
    sprite->hitboxExtentRight = 20;
    sprite->warioCollision = 14;
    GP_SET(sprite->pOamData, sKaentsuboFireballSpawnOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 28;
    sprite->work3 = 20;
    sprite->drawPriority = 0;
    m4aSongNumStart(SOUND_54);
}

void InitKaentsuboFireballLaunch(void)
{
    GP_SET(gCurrentSprite.pOamData, sKaentsuboFireballLaunchOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 84;
    gCurrentSprite.work1 = 100;
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentRight = 28;
    } else {
        gCurrentSprite.hitboxExtentLeft = 32;
    }
}

void UpdateKaentsuboFireballLaunch(void)
{
    if (gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            if (gCurrentSprite.animationTimer > 4) {
                gCurrentSprite.hitboxExtentRight = 56;
                gCurrentSprite.hitboxExtentUp = 56;
            } else if (gCurrentSprite.animationTimer > 7) {
                gCurrentSprite.hitboxExtentRight = 88;
                gCurrentSprite.hitboxExtentUp = 56;
            }
        } else {
            if (gCurrentSprite.animationTimer > 4) {
                gCurrentSprite.hitboxExtentLeft = 60;
                gCurrentSprite.hitboxExtentUp = 56;
            } else if (gCurrentSprite.animationTimer > 7) {
                gCurrentSprite.hitboxExtentLeft = 92;
                gCurrentSprite.hitboxExtentUp = 56;
            }
        }

        if (--gCurrentSprite.work0 == 0) {
            GP_SET(gCurrentSprite.pOamData, sKaentsuboFireballTravelOam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            gCurrentSprite.hitboxExtentLeft = 60;
            gCurrentSprite.hitboxExtentRight = 56;
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
                gCurrentSprite.xPosition += 44;
            } else {
                gCurrentSprite.xPosition -= 44;
            }
        }
    } else if (gCurrentSprite.work1 != 0) {
        gCurrentSprite.work1--;
    } else {
        gCurrentSprite.status = 0;
    }
}

void SpriteKaentsubo(void)
{
    if (gCurrentSprite.status & 0x800) {
        gCurrentSprite.pose = 107;
    }

    switch (gCurrentSprite.pose) {
        case 0:
            InitKaentsubo();
            break;
        case 15:
            InitKaentsuboIdle();
            /* fallthrough */
        case 16:
            UpdateKaentsuboIdle();
            break;
        case 17:
            InitKaentsuboTurnAround();
            /* fallthrough */
        case 18:
            UpdateKaentsuboTurnAround();
            break;
        case 19:
            InitKaentsuboSpitWindup();
            /* fallthrough */
        case 20:
            UpdateKaentsuboSpitWindup();
            break;
        case 23:
            InitKaentsuboHitReact();
            /* fallthrough */
        case 24:
            func_8023C94();
            break;
        case 25:
            InitKaentsuboRecover();
            /* fallthrough */
        case 26:
            UpdateKaentsuboRecover();
            break;
        case 27:
            InitKaentsuboAirborne();
            /* fallthrough */
        case 28:
            func_8023D48();
            break;
        case 29:
            InitKaentsuboStunned();
            /* fallthrough */
        case 30:
            func_8023FA8();
            break;
        case 31:
        case 79:
            InitKaentsuboDefeatedRight();
            /* fallthrough */
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            InitKaentsuboDefeatedLeft();
            /* fallthrough */
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
            InitKaentsuboPushedRight();
            /* fallthrough */
        case 36:
            SpriteUtilPushedRight();
            break;
        case 37:
            InitKaentsuboPushedLeft();
            /* fallthrough */
        case 38:
            SpriteUtilPushedLeft();
            break;
        case 43:
            InitKaentsuboBumpedRight();
            /* fallthrough */
        case 44:
            func_8024478();
            break;
        case 45:
            InitKaentsuboBumpedLeft();
            /* fallthrough */
        case 46:
            func_802449C();
            break;
        case 47:
            func_80244C0();
            /* fallthrough */
        case 48:
            func_80244E0();
            break;
        case 50:
        case 109:
            SpriteUtilDieAfterDelay();
            break;
        case 51:
            InitKaentsuboGroundBounceRight();
            /* fallthrough */
        case 52:
            func_8024988();
            break;
        case 53:
            InitKaentsuboGroundBounceLeft();
            /* fallthrough */
        case 54:
            func_802476C();
            break;
        case 67:
            func_8024AC0();
            goto call_8024AD4;
        case 69:
            func_8024BEC();
            goto call_8024C00;
        case 71:
            InitKaentsuboAirBounceRight();
            /* fallthrough */
        case 68:
        case 72:
call_8024AD4:
            func_8024AD4();
            break;
        case 73:
            InitKaentsuboAirBounceLeft();
            /* fallthrough */
        case 70:
        case 74:
call_8024C00:
            func_8024C00();
            break;
        case 75:
            InitKaentsuboDropped();
            /* fallthrough */
        case 76:
            UpdateKaentsuboDropped();
            break;
        case 77:
            UpdateKaentsuboLandAfterDrop();
            break;
        case 81:
            InitKaentsuboLiftedRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            InitKaentsuboLiftedLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            InitKaentsuboCarriedRight();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            InitKaentsuboCarriedLeft();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;
        case 89:
            InitKaentsuboThrownCommon();
            gCurrentSprite.pose = 90;
            /* fallthrough */
        case 90:
            SpriteUtilThrownLeftSoft();
            break;
        case 93:
            gCurrentSprite.pose = 94;
            InitKaentsuboThrownCommon();
            /* fallthrough */
        case 94:
            SpriteUtilThrownLeftHard();
            break;
        case 97:
            gCurrentSprite.pose = 98;
            InitKaentsuboThrownCommon();
            /* fallthrough */
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;
        case 101:
            gCurrentSprite.pose = 102;
            InitKaentsuboThrownCommon();
            /* fallthrough */
        case 102:
            SpriteUtilThrownUpLeftHard();
            break;
        case 91:
            gCurrentSprite.pose = 92;
            InitKaentsuboThrownCommon();
            /* fallthrough */
        case 92:
            SpriteUtilThrownRightSoft();
            break;
        case 95:
            gCurrentSprite.pose = 96;
            InitKaentsuboThrownCommon();
            /* fallthrough */
        case 96:
            SpriteUtilThrownRightHard();
            break;
        case 99:
            gCurrentSprite.pose = 100;
            InitKaentsuboThrownCommon();
            /* fallthrough */
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;
        case 103:
            gCurrentSprite.pose = 104;
            InitKaentsuboThrownCommon();
            /* fallthrough */
        case 104:
            SpriteUtilThrownUpRightHard();
            break;
        case 105:
            InitKaentsuboThrownDefeatedLeft();
            break;
        case 106:
            InitKaentsuboThrownDefeatedRight();
            break;
        case 107:
            KillKaentsubo();
            break;
        default:
            InitKaentsuboDefeatSmoke();
            SpriteUtilDieAfterDelay();
            break;
    }
}

void SpriteKaentsuboFlame(void)
{
    u32 parentSlot;
    u32 parentOffset;
    register struct PrimarySpriteData *spriteData PIN("r1");
    struct PrimarySpriteData *parentSprite;
    u8 parentHealth;

    parentSlot = SpriteUtilFindParentSlot(PSPRITE_KAENTSUBO);
    if (parentSlot == 24) {
        goto clear;
    }

    spriteData = gSpriteData;
    parentOffset = sizeof(struct PrimarySpriteData);
    parentOffset *= parentSlot;
    parentSprite = (struct PrimarySpriteData *)(parentOffset + (u32)spriteData);
    if (parentSprite->warioCollision != 5) {
        goto check_health;
    }

clear:
    gCurrentSprite.status = 0;
    goto done;

check_health:
    parentHealth = parentSprite->health;
    if (parentHealth == 0) {
        gCurrentSprite.status = parentHealth;
        goto done;
    }

    gCurrentSprite.yPosition = parentSprite->yPosition;
    gCurrentSprite.xPosition = parentSprite->xPosition;
    switch (gCurrentSprite.pose) {
        case 0:
            InitKaentsuboFlame();
            break;
        case 15:
            InitKaentsuboFlameIdle();
            /* fallthrough */
        case 16:
            UpdateKaentsuboFlameFollowParent(parentSlot);
            break;
        case 20:
            SyncKaentsuboFlamePushState(parentSlot);
            break;
        case 35:
            InitKaentsuboFlamePushedRight(parentSlot);
            /* fallthrough */
        case 36:
            UpdateKaentsuboFlamePushedRight(parentSlot);
            break;
        case 37:
            InitKaentsuboFlamePushedLeft(parentSlot);
            /* fallthrough */
        case 38:
            UpdateKaentsuboFlamePushedLeft(parentSlot);
            break;
    }

done:
    return;
}

void SpriteKaentsuboFireball(void)
{
    if (gCurrentSprite.status & 0x800) {
        gCurrentSprite.status = 0;
    }

    switch (gCurrentSprite.pose) {
        case 0:
            InitKaentsuboFireball();
            break;
        case 28:
            func_8023D48();
            break;
        case 23:
            InitKaentsuboFireballLaunch();
            break;
        case 16:
            UpdateKaentsuboFireballLaunch();
            break;
    }
}

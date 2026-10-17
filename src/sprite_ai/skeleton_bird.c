#include "main.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"


extern const struct AnimationFrame sSkeletonBirdGlideOam[];
extern const struct AnimationFrame sSkeletonBirdTurnOam[];
extern const struct AnimationFrame sSkeletonBirdStunnedOam[];
extern const struct AnimationFrame sSkeletonBirdAttackOam[];
extern const struct AnimationFrame sSkeletonBirdFireOam[];
extern const struct AnimationFrame sSkeletonBirdFallOam[];
extern const struct AnimationFrame sSkeletonBirdRecoverOam[];
extern const struct AnimationFrame sSkeletonBirdProjectileOam[];
extern const struct AnimationFrame sSkeletonBirdProjectileBreakOam[];
extern const struct AnimationFrame sSkeletonBirdDebrisOam[];
extern const s16 sSkeletonBirdBobYSpeed[];

void UpdateSkeletonBirdFacingAndProjectile(void)
{
    struct PrimarySpriteData *currentSprite = &gCurrentSprite;
    u8 *actionTimer = &currentSprite->work1;
    register u8 timer PIN("r0") = *actionTimer;
    u16 initialTimer = timer;
    struct PrimarySpriteData *turnSprite;
    turnSprite = currentSprite;

    if (initialTimer == 0) {
        if (currentSprite->health == 1
            && ((*(u32 *)&currentSprite->animationTimer) & 0xFFFFFF) == 0x70007) {
            *actionTimer = 77;
            GP_SET(currentSprite->pOamData, sSkeletonBirdFireOam);
            currentSprite->currentAnimationFrame = initialTimer;
            currentSprite->animationTimer = initialTimer;
            currentSprite->health = 107;
            return;
        }

        if (turnSprite->xPosition > gWarioData.xPosition) {
            if (!(turnSprite->status & SPRITE_STATUS_FACING_RIGHT)) {
                return;
            }
        } else if (turnSprite->status & SPRITE_STATUS_FACING_RIGHT) {
            return;
        }

        {
            u8 *turnTimer = &turnSprite->work1;
            u16 zero;
            zero = 0;
            {
                u8 turnDelay = 28;
                *turnTimer = turnDelay;
            }
            GP_SET(turnSprite->pOamData, sSkeletonBirdTurnOam);
            turnSprite->currentAnimationFrame = zero;
            turnSprite->animationTimer = zero;
        }
        return;
    }

    timer--;
    *actionTimer = timer;
    {
        u8 remaining = timer;
        if (remaining != 0) {
            if (gWarioData.reaction != REACTION_ZOMBIE && currentSprite->health > 29
                && ((*(u32 *)&currentSprite->animationTimer) & 0xFFFFFF) == 0x10008) {
                if (currentSprite->status & SPRITE_STATUS_FACING_RIGHT) {
                    SpawnPrimarySpriteWithStatus(PSPRITE_SKELETON_BIRD_PROJECTILE, currentSprite->roomSlot,
                                 currentSprite->gfxSlot, currentSprite->yPosition,
                                 currentSprite->xPosition + 64, SPRITE_STATUS_FACING_RIGHT);
                } else {
                    SpawnPrimarySpriteWithStatus(PSPRITE_SKELETON_BIRD_PROJECTILE, currentSprite->roomSlot,
                                 currentSprite->gfxSlot, currentSprite->yPosition,
                                 currentSprite->xPosition - 64, 0);
                }
            }
            return;
        }

        GP_SET(currentSprite->pOamData, sSkeletonBirdGlideOam);
        currentSprite->currentAnimationFrame = remaining;
        currentSprite->animationTimer = remaining;
    }
    if (gWarioData.reaction == REACTION_ZOMBIE || currentSprite->health <= 29) {
        currentSprite->status ^= SPRITE_STATUS_FACING_RIGHT;
    }
}



void InitSkeletonBird(void)
{
    struct PrimarySpriteData *currentSprite = &gCurrentSprite;

    currentSprite->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    currentSprite->drawDistanceDown = 24;
    currentSprite->drawDistanceUp = 24;
    currentSprite->drawDistanceLeftRight = 24;
    currentSprite->hitboxExtentUp = 56;
    currentSprite->hitboxExtentDown = 52;
    currentSprite->hitboxExtentLeft = 32;
    currentSprite->hitboxExtentRight = 28;
    GP_SET(currentSprite->pOamData, sSkeletonBirdGlideOam);
    currentSprite->currentAnimationFrame = 0;
    currentSprite->animationTimer = 0;
    currentSprite->work0 = 0;
    currentSprite->work1 = 0;
    currentSprite->work2 = 80;
    currentSprite->work3 = 0;
    currentSprite->warioCollision = 32;
    currentSprite->drawPriority = 1;
    currentSprite->health = 30;

    if (currentSprite->xPosition > gWarioData.xPosition) {
        currentSprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
        currentSprite->pose = 111;
    } else {
        currentSprite->status |= SPRITE_STATUS_FACING_RIGHT;
        currentSprite->pose = 113;
    }
}

void UpdateSkeletonBirdMoveLeft(void)
{
    struct PrimarySpriteData *bobSprite;
    u8 *delayPtr;
    u32 delay;
    u8 *indexPtr;
    u8 index;
    s16 delta;

    UpdateSkeletonBirdFacingAndProjectile();
    bobSprite = &gCurrentSprite;
    delayPtr = &bobSprite->work0;
    delay = *delayPtr;
    if (delay != 0) {
        delay--;
        goto storeDelay;
    }

    indexPtr = &bobSprite->work3;
    index = *indexPtr;
    delta = sSkeletonBirdBobYSpeed[index];
    if (delta == S16_MAX) {
        u32 previousIndex = index - 1;
        u16 yPosition;
        s16 previousSpeed;
        previousIndex <<= 1;
        previousIndex += (u32)sSkeletonBirdBobYSpeed;
        yPosition = *(volatile u16 *)&bobSprite->yPosition;
        previousSpeed = *(const s16 *)previousIndex;
        yPosition += previousSpeed;
        bobSprite->yPosition = yPosition;
    } else {
        u8 nextIndex = index + 1;
        u8 *destination = indexPtr;
        *destination = nextIndex;
        bobSprite->yPosition += delta;
    }

    if (!(gMainTimer & 1)) {
        struct PrimarySpriteData *currentSprite = &gCurrentSprite;
        u8 *distancePtr = &currentSprite->work2;
        register u8 distance PIN("r1") = *distancePtr;
        u32 zeroOrDistance = distance;

        if (zeroOrDistance != 0) {
            distance--;
            *distancePtr = distance;
            currentSprite->xPosition -= 4;
            if (distance != 80) {
                return;
            }
            currentSprite->work3 = 0;
            if (!(currentSprite->status & SPRITE_STATUS_FACING_RIGHT)) {
                return;
            }
            currentSprite->pose = 113;
        } else {
            currentSprite->pose = 113;
            currentSprite->work3 = zeroOrDistance;
        }
        delayPtr = &currentSprite->work0;
        delay = 60;
storeDelay:
        *delayPtr = delay;
    }
}

void UpdateSkeletonBirdMoveRight(void)
{
    struct PrimarySpriteData *bobSprite;
    u8 *delayPtr;
    u32 delay;
    u8 *indexPtr;
    u8 index;
    s16 delta;

    UpdateSkeletonBirdFacingAndProjectile();
    bobSprite = &gCurrentSprite;
    delayPtr = &bobSprite->work0;
    delay = *delayPtr;
    if (delay != 0) {
        delay--;
        goto storeDelay;
    }

    indexPtr = &bobSprite->work3;
    index = *indexPtr;
    delta = sSkeletonBirdBobYSpeed[index];
    if (delta == S16_MAX) {
        u32 previousIndex = index - 1;
        u16 yPosition;
        s16 previousSpeed;
        previousIndex <<= 1;
        previousIndex += (u32)sSkeletonBirdBobYSpeed;
        yPosition = *(volatile u16 *)&bobSprite->yPosition;
        previousSpeed = *(const s16 *)previousIndex;
        yPosition += previousSpeed;
        bobSprite->yPosition = yPosition;
    } else {
        u8 nextIndex = index + 1;
        u8 *destination = indexPtr;
        *destination = nextIndex;
        bobSprite->yPosition += delta;
    }

    {
        u32 timerValue = gMainTimer;
        u32 timerBit;
        timerBit = 1;
        timerBit &= timerValue;
        if (timerBit != 0) {
            return;
        }

        {
            struct PrimarySpriteData *currentSprite = &gCurrentSprite;
            u8 *distancePtr = &currentSprite->work2;
            register u8 distance PIN("r1") = *distancePtr;

            if (distance <= 159) {
                distance = ++*distancePtr;
                currentSprite->xPosition += 4;
                if (distance != 80) {
                    return;
                }
                currentSprite->work3 = 0;
                if (currentSprite->status & SPRITE_STATUS_FACING_RIGHT) {
                    return;
                }
                currentSprite->pose = 111;
            } else {
                currentSprite->pose = 111;
                currentSprite->work3 = timerBit;
            }
            delayPtr = &currentSprite->work0;
            delay = 60;
storeDelay:
            *delayPtr = delay;
        }
    }
}



void InitSkeletonBirdAttackWindup(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdAttackOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 10;
    gCurrentSprite.pose = 115;
    m4aSongNumStart(SOUND_56);
}

void UpdateSkeletonBirdAttackWindup(void)
{
    struct PrimarySpriteData *currentSprite = &gCurrentSprite;
    u8 *countdown;
    u32 timer;
    register struct PrimarySpriteData *savedSprite PIN("r12");
    u8 remaining;

    currentSprite->disableWarioCollisionTimer = 1;
    countdown = &currentSprite->work0;
    timer = *countdown;
    timer--;
    *countdown = timer;
    remaining = timer;
    savedSprite = currentSprite;
    if (remaining == 0) {
        u8 recoveryTimer = 15;
        u16 zero;

        currentSprite->disableWarioCollisionTimer = recoveryTimer;
        GP_SET(currentSprite->pOamData, sSkeletonBirdStunnedOam);
        currentSprite->currentAnimationFrame = remaining;
        zero = 0;
        ASM_BARRIER(: "+r"(zero) : "r"(savedSprite));
        savedSprite->animationTimer = remaining;
        {
            register u16 status PIN("r1") = savedSprite->status;
            u16 maskedStatus =
                status & ~SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
            u8 *extentPtr = (u8 *)savedSprite;

            *(u16 *)extentPtr = maskedStatus;
            extentPtr += 35;
            *extentPtr = 56;
            {
                register u8 *downPtr PIN("r0") = (u8 *)savedSprite + 36;
                *downPtr = zero;
            }
            extentPtr += 2;
            *extentPtr = 32;
            extentPtr++;
            *(volatile u8 *)extentPtr = 28;
        }
        *countdown = zero;

        {
            u8 collisionType = 5;
            currentSprite = savedSprite;
            currentSprite->warioCollision = collisionType;
        }
        currentSprite->drawPriority = 2;
        currentSprite->status &= ~SPRITE_STATUS_HIDDEN;
        currentSprite->health = recoveryTimer;

        if (currentSprite->xPosition > gWarioData.xPosition) {
            currentSprite->pose = 73;
        } else {
            u8 leftPose = 71;
            register struct PrimarySpriteData *poseSprite PIN("r4");
            poseSprite = savedSprite;
            poseSprite->pose = leftPose;
        }

        {
            register struct PrimarySpriteData *spawnSprite PIN("r5") = savedSprite;
            register u16 spriteStatus PIN("r1") = spawnSprite->status;
            u32 spawnStatus = SPRITE_STATUS_FACING_RIGHT;
            u32 statusMask = spawnStatus;
            u32 facingFlag;

            statusMask &= spriteStatus;
            facingFlag = (u16)statusMask;
            if (facingFlag != 0) {
                SpawnPrimarySpriteWithStatus(PSPRITE_SKELETON_BIRD_DEBRIS, savedSprite->roomSlot,
                             savedSprite->gfxSlot, savedSprite->yPosition,
                             savedSprite->xPosition, spawnStatus);
            } else {
                register struct PrimarySpriteData *idSprite PIN("r2") = savedSprite;
                u8 roomSlot = idSprite->roomSlot;
                u8 gfxSlot = idSprite->gfxSlot;
                register struct PrimarySpriteData *positionSprite PIN("r5") = savedSprite;
                SpawnPrimarySpriteWithStatus(PSPRITE_SKELETON_BIRD_DEBRIS, roomSlot, gfxSlot,
                             positionSprite->yPosition, positionSprite->xPosition,
                             facingFlag);
            }
        }
    }
}


void InitSkeletonBirdTurnTowardLeft(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdFallOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void InitSkeletonBirdTurnCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdFallOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void InitSkeletonBirdPushedRight(void)
{
    gCurrentSprite.pose = 44;
    InitSkeletonBirdTurnCommon();
}

void InitSkeletonBirdPushedLeft(void)
{
    gCurrentSprite.pose = 46;
    InitSkeletonBirdTurnCommon();
}

void InitSkeletonBirdTumbleCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 1;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) | SPRITE_STATUS_MAYBE_DEAD;
}

void InitSkeletonBirdTumbleRight(void)
{
    gCurrentSprite.pose = 32;
    InitSkeletonBirdTumbleCommon();
}

void InitSkeletonBirdTumbleLeft(void)
{
    gCurrentSprite.pose = 34;
    InitSkeletonBirdTumbleCommon();
}

void InitSkeletonBirdShortTumbleCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 1;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}

void InitSkeletonBirdShortTumbleRight(void)
{
    gCurrentSprite.pose = 32;
    InitSkeletonBirdShortTumbleCommon();
}

void InitSkeletonBirdShortTumbleLeft(void)
{
    gCurrentSprite.pose = 34;
    InitSkeletonBirdShortTumbleCommon();
}

void InitSkeletonBirdStunCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdFallOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
    }
}

void InitSkeletonBirdAirStunRight(void)
{
    gCurrentSprite.pose = 52;
    InitSkeletonBirdStunCommon();
}

void InitSkeletonBirdAirStunLeft(void)
{
    gCurrentSprite.pose = 54;
    InitSkeletonBirdStunCommon();
}

void InitSkeletonBirdAirBounceCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdFallOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void InitSkeletonBirdAirBounceRight(void)
{
    gCurrentSprite.pose = 72;
    InitSkeletonBirdAirBounceCommon();
}

void InitSkeletonBirdAirBounceLeft(void)
{
    gCurrentSprite.pose = 74;
    InitSkeletonBirdAirBounceCommon();
}

void InitSkeletonBirdAirStunFromTimer(void)
{
    if (gMainTimer & 1) {
        InitSkeletonBirdAirStunRight();
    } else {
        InitSkeletonBirdAirStunLeft();
    }
}

void InitSkeletonBirdLiftedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdFallOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.health = 1;
}

void InitSkeletonBirdLiftedRight(void)
{
    gCurrentSprite.pose = 82;
    InitSkeletonBirdLiftedCommon();
}

void InitSkeletonBirdLiftedLeft(void)
{
    gCurrentSprite.pose = 84;
    InitSkeletonBirdLiftedCommon();
}

void InitSkeletonBirdCarriedCommon(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdFallOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.health = 1;
}

void InitSkeletonBirdCarriedRight(void)
{
    gCurrentSprite.pose = 88;
    InitSkeletonBirdCarriedCommon();
}

void InitSkeletonBirdCarriedLeft(void)
{
    gCurrentSprite.pose = 86;
    InitSkeletonBirdCarriedCommon();
}

void InitSkeletonBirdThrownCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdFallOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitSkeletonBirdCrashRecover(void)
{
    gCurrentSprite.pose = 48;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
}

void UpdateSkeletonBirdCrashRecover(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 29;
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            gCurrentSprite.pose = 25;
        }
    }
}

void InitSkeletonBirdPerch(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdRecoverOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 65;
    gCurrentSprite.work1 = 80;
    gCurrentSprite.warioCollision = 32;
    gCurrentSprite.drawPriority = 1;
}

void UpdateSkeletonBirdPerch(void)
{
    if (gWarioData.reaction == REACTION_ZOMBIE && !(gMainTimer & 3)) {
        gCurrentSprite.status ^= SPRITE_STATUS_HIDDEN;
    }
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.work0 != 0) {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            gCurrentSprite.yPosition -= 68;
            GP_SET(gCurrentSprite.pOamData, sSkeletonBirdGlideOam);
            gCurrentSprite.animationTimer = 0;
            gCurrentSprite.currentAnimationFrame = 0;
        }
    } else if (gCurrentSprite.work1 != 0) {
        gCurrentSprite.work1--;
        gCurrentSprite.yPosition -= 4;
    } else {
        InitSkeletonBird();
        gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    }
}

void InitSkeletonBirdProjectile(void)
{
    struct PrimarySpriteData *currentSprite = &gCurrentSprite;
    u8 zeroByte;
    u16 zeroHalf;

    currentSprite->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    zeroByte = 0;
    zeroHalf = 0;
    currentSprite->status &= ~SPRITE_STATUS_HIDDEN;
    currentSprite->drawDistanceDown = 8;
    currentSprite->drawDistanceUp = 8;
    currentSprite->drawDistanceLeftRight = 8;
    currentSprite->hitboxExtentUp = 16;
    currentSprite->hitboxExtentDown = 12;
    currentSprite->hitboxExtentLeft = 16;
    currentSprite->hitboxExtentRight = 12;
    GP_SET(currentSprite->pOamData, sSkeletonBirdProjectileOam);
    currentSprite->currentAnimationFrame = zeroByte;
    currentSprite->animationTimer = zeroHalf;
    currentSprite->warioCollision = 16;
    currentSprite->drawPriority = 1;
    currentSprite->pose = 16;
    m4aSongNumStart(SOUND_55);
}

void UpdateSkeletonBirdProjectile(void)
{
    func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1 || (gSpriteCollisionResult & 0xF) != 0) {
        gCurrentSprite.pose = 49;
    } else {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            gCurrentSprite.xPosition += 4;
        } else {
            gCurrentSprite.xPosition -= 4;
        }
        gCurrentSprite.yPosition += 4;
    }
}

void InitSkeletonBirdProjectileBreak(void)
{
    GP_SET(gCurrentSprite.pOamData, sSkeletonBirdProjectileBreakOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 7;
    gCurrentSprite.pose = 50;
}

void UpdateSkeletonBirdProjectileBreak(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = 0;
    }
}

void InitSkeletonBirdDebris(void)
{
    struct PrimarySpriteData *currentSprite = &gCurrentSprite;
    u8 zeroByte;
    u16 zeroHalf;

    currentSprite->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    zeroByte = 0;
    zeroHalf = 0;
    currentSprite->status &= ~SPRITE_STATUS_HIDDEN;
    currentSprite->drawDistanceDown = 32;
    currentSprite->drawDistanceUp = 56;
    currentSprite->drawDistanceLeftRight = 40;
    currentSprite->hitboxExtentUp = 32;
    currentSprite->hitboxExtentDown = 28;
    currentSprite->hitboxExtentLeft = 32;
    currentSprite->hitboxExtentRight = 28;
    GP_SET(currentSprite->pOamData, sSkeletonBirdDebrisOam);
    currentSprite->currentAnimationFrame = zeroByte;
    currentSprite->animationTimer = zeroHalf;
    currentSprite->warioCollision = 6;
    currentSprite->drawPriority = 1;
    currentSprite->pose = 16;
    currentSprite->work0 = 27;
}

void UpdateSkeletonBirdDebris(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = 0;
    }
}

void SpriteSkeletonBird(void)
{
    struct PrimarySpriteData *currentSprite;

    if (gWarioData.reaction != REACTION_ZOMBIE) {
        struct PrimarySpriteData *sprite = &gCurrentSprite;
        u8 collisionType = sprite->warioCollision;

        currentSprite = sprite;
        if (collisionType != 5) {
            currentSprite->status ^= SPRITE_STATUS_HIDDEN;
            if (currentSprite->health > 1) {
                currentSprite->health--;
            }
        }
    } else {
        struct PrimarySpriteData *sprite = &gCurrentSprite;
        u8 pose = sprite->pose;

        currentSprite = sprite;
        if (pose != 26) {
            currentSprite->status &= ~SPRITE_STATUS_HIDDEN;
        }
    }

    currentSprite = &gCurrentSprite;
    if (currentSprite->status & SPRITE_STATUS_UNDERWATER) {
        currentSprite->pose = 107;
    }

    switch (currentSprite->pose) {
        case 0:
            InitSkeletonBird();
            break;
        case 111:
            UpdateSkeletonBirdMoveLeft();
            break;
        case 113:
            UpdateSkeletonBirdMoveRight();
            break;
        case 114:
            InitSkeletonBirdAttackWindup();
            /* fallthrough */
        case 115:
            UpdateSkeletonBirdAttackWindup();
            break;
        case 29:
            InitSkeletonBirdTurnTowardLeft();
            /* fallthrough */
        case 30:
            func_8023FA8();
            break;
        case 31:
        case 79:
            InitSkeletonBirdTumbleRight();
            /* fallthrough */
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            InitSkeletonBirdTumbleLeft();
            /* fallthrough */
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
        case 51:
            InitSkeletonBirdAirStunRight();
            goto update_air_stun_right;
        case 37:
        case 53:
            InitSkeletonBirdAirStunLeft();
            goto update_air_stun_left;
        case 43:
            InitSkeletonBirdPushedRight();
            /* fallthrough */
        case 44:
            func_8024478();
            break;
        case 45:
            InitSkeletonBirdPushedLeft();
            /* fallthrough */
        case 46:
            func_802449C();
            break;
        case 25:
            InitSkeletonBirdPerch();
            /* fallthrough */
        case 26:
            UpdateSkeletonBirdPerch();
            break;
        case 47:
            gCurrentSprite.health = 1;
            InitSkeletonBirdCrashRecover();
            /* fallthrough */
        case 48:
            UpdateSkeletonBirdCrashRecover();
            break;
        case 55:
            if (gCurrentSprite.health == 1) {
                gCurrentSprite.pose = 107;
                break;
            }
            func_8024688();
            /* fallthrough */
        case 56:
            func_80246B8();
            break;
        case 57:
            if (gCurrentSprite.health == 1) {
                gCurrentSprite.pose = 107;
                break;
            }
            func_802473C();
            /* fallthrough */
        case 54:
        case 58:
update_air_stun_left:
            func_802476C();
            break;
        case 59:
            if (gCurrentSprite.health == 1) {
                gCurrentSprite.pose = 107;
                break;
            }
            func_80247F0();
            /* fallthrough */
        case 60:
            func_8024820();
            break;
        case 61:
            if (gCurrentSprite.health == 1) {
                gCurrentSprite.pose = 107;
                break;
            }
            func_80248A4();
            /* fallthrough */
        case 62:
            func_80248D4();
            break;
        case 63:
            if (gCurrentSprite.health == 1) {
                gCurrentSprite.pose = 107;
                break;
            }
            func_8024958();
            /* fallthrough */
        case 52:
        case 64:
update_air_stun_right:
            func_8024988();
            break;
        case 65:
            if (gCurrentSprite.health == 1) {
                gCurrentSprite.pose = 107;
                break;
            }
            func_8024A0C();
            /* fallthrough */
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
            /* fallthrough */
        case 68:
        case 72:
            func_8024AD4();
            break;
        case 69:
            func_8024BEC();
            /* fallthrough */
        case 70:
        case 74:
            func_8024C00();
            break;
        case 71:
            InitSkeletonBirdAirBounceRight();
            func_8024AD4();
            break;
        case 73:
            InitSkeletonBirdAirBounceLeft();
            func_8024C00();
            break;
        case 75:
            InitSkeletonBirdAirStunFromTimer();
            break;
        case 81:
            InitSkeletonBirdLiftedRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            InitSkeletonBirdLiftedLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            InitSkeletonBirdCarriedRight();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            InitSkeletonBirdCarriedLeft();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;
        case 89:
            InitSkeletonBirdThrownCommon();
            gCurrentSprite.pose = 90;
            /* fallthrough */
        case 90:
            SpriteUtilThrownLeftSoft();
            break;
        case 93:
            gCurrentSprite.pose = 94;
            InitSkeletonBirdThrownCommon();
            /* fallthrough */
        case 94:
            SpriteUtilThrownLeftHard();
            break;
        case 97:
            gCurrentSprite.pose = 98;
            InitSkeletonBirdThrownCommon();
            /* fallthrough */
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;
        case 101:
            gCurrentSprite.pose = 102;
            InitSkeletonBirdThrownCommon();
            /* fallthrough */
        case 102:
            SpriteUtilThrownUpLeftHard();
            break;
        case 91:
            gCurrentSprite.pose = 92;
            InitSkeletonBirdThrownCommon();
            /* fallthrough */
        case 92:
            SpriteUtilThrownRightSoft();
            break;
        case 95:
            gCurrentSprite.pose = 96;
            InitSkeletonBirdThrownCommon();
            /* fallthrough */
        case 96:
            SpriteUtilThrownRightHard();
            break;
        case 99:
            gCurrentSprite.pose = 100;
            InitSkeletonBirdThrownCommon();
            /* fallthrough */
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;
        case 103:
            gCurrentSprite.pose = 104;
            InitSkeletonBirdThrownCommon();
            /* fallthrough */
        case 104:
            SpriteUtilThrownUpRightHard();
            break;
        case 105:
            InitSkeletonBirdShortTumbleLeft();
            break;
        case 106:
            InitSkeletonBirdShortTumbleRight();
            break;
        default:
            SpriteUtilDie();
            break;
    }
}

void SpriteSkeletonBirdProjectile(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitSkeletonBirdProjectile();
            break;
        case 16:
            UpdateSkeletonBirdProjectile();
            break;
        case 50:
            UpdateSkeletonBirdProjectileBreak();
            break;
        default:
            InitSkeletonBirdProjectileBreak();
            break;
    }
}

void SpriteSkeletonBirdDebris(void)
{
    if (!(gCurrentSprite.status & SPRITE_STATUS_ONSCREEN)) {
        gCurrentSprite.status = 0;
    } else {
        switch (gCurrentSprite.pose) {
            case 0:
                InitSkeletonBirdDebris();
                break;
            case 16:
                UpdateSkeletonBirdDebris();
                break;
        }
        gCurrentSprite.disableWarioCollisionTimer = 1;
    }
}

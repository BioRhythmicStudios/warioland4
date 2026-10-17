#include "fixed_point.h"
#include "global_data.h"
#include "gba/m4a.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/moguramen.h"
#include "sprite_util.h"
#include "wario.h"


extern const struct AnimationFrame sMoguramenBurrowedOam[];
extern const struct AnimationFrame sMoguramenIdleOam[];
extern const struct AnimationFrame sMoguramenAttackWindupOam[];
extern const struct AnimationFrame sMoguramenFallingOam[];
extern const struct AnimationFrame sMoguramenCrashRecoverOam[];
extern const struct AnimationFrame sMoguramenTurnStartOam[];
extern const struct AnimationFrame sMoguramenTurnMiddleOam[];
extern const struct AnimationFrame sMoguramenTurnFinishOam[];
extern const struct AnimationFrame sMoguramenStunnedOam[];
extern const struct AnimationFrame sMoguramenBurrowEmergeOam[];
extern const struct AnimationFrame sMoguramenPushedWindupOam[];
extern const struct AnimationFrame sMoguramenProjectileOam[];
extern const struct AnimationFrame sMoguramenTumbleOam[];
extern const struct AnimationFrame sMoguramenDefeatedOam[];
extern const struct AnimationFrame sMoguramenPushedSlideOam[];
extern const s16 sUnk_8352B18[];
extern const s16 sMoguramenProjectileYSpeed[];
extern const s16 sMoguramenProjectileXSpeed[];
void SpawnTileDebrisParticle(u32 yPosition, u32 xPosition, u32 xVelocity, u32 yVelocity, u32 angleStep);

void UpdateMoguramenHitboxForFacing(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 32;
        gCurrentSprite.hitboxExtentRight = 52;
    } else {
        gCurrentSprite.hitboxExtentLeft = 56;
        gCurrentSprite.hitboxExtentRight = 28;
    }
}

void InitMoguramenAttackWindup(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenAttackWindupOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.warioCollision = 3;
    gCurrentSprite.work1 = 96;
    UpdateMoguramenHitboxForFacing();
}

void InitMoguramenIdle(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 3;
    UpdateMoguramenHitboxForFacing();
}

void InitMoguramen(void)
{
    u8 *roomData;
    u8 roomSlot;
    int roomDataOffset;

    gCurrentSprite.status |= SPRITE_STATUS_HEAVY;
    gCurrentSprite.drawDistanceDown = 36;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 20;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    SpriteUtilTurnTowardWario();

    roomData = gRoomSpriteData;
    roomSlot = gCurrentSprite.roomSlot;
    roomDataOffset = roomSlot * 3;
    roomData += 2;
    if (roomData[roomDataOffset] == 17) {
        gCurrentSprite.health = 2;
        gCurrentSprite.drawPriority = 1;
        gCurrentSprite.pose = 23;
    } else {
        gCurrentSprite.health = 1;
        InitMoguramenIdle();
    }
}

void UpdateMoguramenAttackWindup(void)
{
#ifndef PORTABLE
    // Called without a prototype, as in the original. The callee narrows its own arguments.
    extern void func_8023BFC();
#endif
    struct PrimarySpriteData *currentSprite;
    u32 animationState;
    register u32 frontY PIN("r0");
    u16 frontX;
    u8 nextPose;
    u8 *attackTimer;

    currentSprite = &gCurrentSprite;
    animationState = *((u32 *)&currentSprite->animationTimer) & 0xFFFFFF;
    if (animationState == 0x10002) {
        m4aSongNumStart(SOUND_71);
    } else if (animationState == 0x10004) {
        if (currentSprite->status & SPRITE_STATUS_FACING_RIGHT) {
            SpawnPrimarySpriteWithStatus(PSPRITE_9A, currentSprite->roomSlot, currentSprite->gfxSlot,
                         currentSprite->yPosition - 32, currentSprite->xPosition + 32, 0);
        } else {
            SpawnPrimarySpriteWithStatus(PSPRITE_9A, currentSprite->roomSlot, currentSprite->gfxSlot,
                         currentSprite->yPosition - 32, currentSprite->xPosition - 32,
                         SPRITE_STATUS_FACING_RIGHT);
        }
        m4aSongNumStart(SOUND_72);
    }

    func_80238A4();
    currentSprite = &gCurrentSprite;
    if (currentSprite->status & SPRITE_STATUS_FACING_RIGHT) {
        frontY = currentSprite->yPosition - 32;
        frontY <<= 16;
        frontY >>= 16;
        frontX = currentSprite->xPosition + currentSprite->hitboxExtentRight;
    } else {
        frontY = currentSprite->yPosition - 32;
        frontY <<= 16;
        frontY >>= 16;
        frontX = currentSprite->xPosition - currentSprite->hitboxExtentLeft;
    }
    func_8023BFC(frontY, frontX);
    if (gSpriteCollisionTileType & 0xF) {
        nextPose = 17;
        currentSprite->pose = nextPose;
    } else {
        struct PrimarySpriteData *currentSprite2;
        func_8026374();
        currentSprite2 = &gCurrentSprite;
        attackTimer = &currentSprite2->work1;
        (*attackTimer)--;
        if (*attackTimer == 0) {
            currentSprite2->pose = 23;
        }
    }
}

void UpdateMoguramenIdle(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition,
                         gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition,
                         gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (gSpriteCollisionTileType == 0) {
            gCurrentSprite.pose = 27;
            return;
        }
    } else {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            if (gSpriteCollisionResult & 0xF0) {
                u16 xPosition;
                xPosition = gCurrentSprite.xPosition;
                if (((xPosition & 0x3F) + gCurrentSprite.hitboxExtentRight) > 63) {
                    func_8023BFC(gCurrentSprite.yPosition, xPosition + 28);
                    if (gSpriteCollisionTileType & 0xF0) {
                        func_8023BFC(gCurrentSprite.yPosition - 32,
                                     gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
                        if ((gSpriteCollisionTileType & 0xF) != 0) {
                            gCurrentSprite.pose = 17;
                            return;
                        }
                    } else {
                        gCurrentSprite.pose = 17;
                        return;
                    }
                }
            }
        } else {
            if (gSpriteCollisionResult & 0xF0) {
                u16 xPosition;
                xPosition = gCurrentSprite.xPosition;
                if ((xPosition & 0x3F) < gCurrentSprite.hitboxExtentLeft) {
                    func_8023BFC(gCurrentSprite.yPosition, xPosition - 28);
                    if (gSpriteCollisionTileType & 0xF0) {
                        func_8023BFC(gCurrentSprite.yPosition - 32,
                                     gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
                        if ((gSpriteCollisionTileType & 0xF) != 0) {
                            gCurrentSprite.pose = 17;
                            return;
                        }
                    } else {
                        gCurrentSprite.pose = 17;
                        return;
                    }
                }
            }
        }
    }
    func_8026374();
}

void InitMoguramenTurnAround(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenTurnStartOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 15;
    gCurrentSprite.warioCollision = 3;
    UpdateMoguramenHitboxForFacing();
}

void UpdateMoguramenTurnAround(void)
{
    struct PrimarySpriteData *currentSprite;
    u8 nextPose;

    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        currentSprite = &gCurrentSprite;
        currentSprite->work0--;
        if (currentSprite->work0 == 0) {
            nextPose = currentSprite->pose;
            switch (nextPose) {
                case 18:
                    GP_SET(currentSprite->pOamData, sMoguramenTurnMiddleOam);
                    currentSprite->currentAnimationFrame = 0;
                    currentSprite->animationTimer = 0;
                    currentSprite->pose = 20;
                    currentSprite->work0 = 14;
                    currentSprite->warioCollision = 1;
                    currentSprite->hitboxExtentLeft = 32;
                    currentSprite->hitboxExtentRight = 28;
                    break;
                case 20:
                    GP_SET(currentSprite->pOamData, sMoguramenTurnFinishOam);
                    currentSprite->currentAnimationFrame = 0;
                    currentSprite->animationTimer = 0;
                    currentSprite->pose = 22;
                    currentSprite->work0 = 7;
                    currentSprite->warioCollision = 3;
                    currentSprite->status ^= SPRITE_STATUS_FACING_RIGHT;
                    UpdateMoguramenHitboxForFacing();
                    break;
                default:
                    if (currentSprite->health == 2) {
                        currentSprite->pose = 110;
                    } else {
                        currentSprite->pose = 15;
                    }
                    break;
            }
        }
    }
}

void InitMoguramenStunnedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
        gCurrentSprite.hitboxExtentLeft = 32;
        gCurrentSprite.hitboxExtentRight = 28;
    }
}

void InitMoguramenKnockbackRight(void)
{
    gCurrentSprite.pose = 52;
    InitMoguramenStunnedCommon();
}

void InitMoguramenKnockbackLeft(void)
{
    gCurrentSprite.pose = 54;
    InitMoguramenStunnedCommon();
}

void InitMoguramenBurrowedWait(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenBurrowedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 72;
    gCurrentSprite.warioCollision = 3;
    UpdateMoguramenHitboxForFacing();
}

void UpdateMoguramenBurrowedWait(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            if (gCurrentSprite.health == 2) {
                gCurrentSprite.pose = 110;
            } else {
                gCurrentSprite.pose = 15;
            }
        }
    }
}

void InitMoguramenBurrowEmerge(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenBurrowEmergeOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 35;
}

void UpdateMoguramenBurrowEmerge(void)
{
    u8 timer;
    u8 timerCopy;

    timer = gCurrentSprite.work0;
    timerCopy = timer;
    if ((u8)(timer - 12) > 16) {
        func_8023CD8();
    } else {
        if (timer == 28) {
            gCurrentSprite.yPosition -= 32;
        } else if (timer == 24) {
            gCurrentSprite.yPosition -= 32;
        } else if (timer == 20) {
            gCurrentSprite.yPosition += 32;
        } else if (timerCopy == 16) {
            gCurrentSprite.yPosition += 32;
        }
        gCurrentSprite.work0--;
    }
}

void InitMoguramenFalling(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 3;
    UpdateMoguramenHitboxForFacing();
}

void InitMoguramenStunned(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
}

void InitMoguramenTumbleCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenTumbleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 4;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void InitMoguramenTumbleRight(void)
{
    gCurrentSprite.pose = 32;
    InitMoguramenTumbleCommon();
}

void InitMoguramenTumbleLeft(void)
{
    gCurrentSprite.pose = 34;
    InitMoguramenTumbleCommon();
}

void InitMoguramenShortTumbleCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenTumbleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 3;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void InitMoguramenShortTumbleRight(void)
{
    gCurrentSprite.pose = 32;
    InitMoguramenShortTumbleCommon();
}

void InitMoguramenShortTumbleLeft(void)
{
    gCurrentSprite.pose = 34;
    InitMoguramenShortTumbleCommon();
}

void InitMoguramenPushedWindupCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenPushedWindupOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.warioCollision = 1;
}

void InitMoguramenPushedRightWindup(void)
{
    gCurrentSprite.pose = 36;
    InitMoguramenPushedWindupCommon();
}

void InitMoguramenPushedLeftWindup(void)
{
    gCurrentSprite.pose = 38;
    InitMoguramenPushedWindupCommon();
}

void InitMoguramenPushedSlideCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenPushedSlideOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
}

void InitMoguramenPushedRightSlide(void)
{
    gCurrentSprite.pose = 40;
    InitMoguramenPushedSlideCommon();
}

void InitMoguramenPushedLeftSlide(void)
{
    gCurrentSprite.pose = 42;
    InitMoguramenPushedSlideCommon();
}

void InitMoguramenStunHoldCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
}

void InitMoguramenBumpedRight(void)
{
    gCurrentSprite.pose = 44;
    InitMoguramenStunHoldCommon();
}

void InitMoguramenBumpedLeft(void)
{
    gCurrentSprite.pose = 46;
    InitMoguramenStunHoldCommon();
}

void InitMoguramenDefeated(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 7;
    gCurrentSprite.pose = 50;
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void KillMoguramen(void)
{
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void InitMoguramenAirBounceCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
}

void InitMoguramenAirBounceRight(void)
{
    gCurrentSprite.pose = 72;
    InitMoguramenAirBounceCommon();
}

void InitMoguramenAirBounceLeft(void)
{
    gCurrentSprite.pose = 74;
    InitMoguramenAirBounceCommon();
}

void InitMoguramenThrownCrash(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenCrashRecoverOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
    if (gCurrentSprite.health == 2) {
        gCurrentSprite.pose = 113;
        gCurrentSprite.work0 = 30;
        gCurrentSprite.health = 1;
    } else {
        gCurrentSprite.pose = 76;
        gCurrentSprite.work0 = 20;
    }
}

void UpdateMoguramenThrownCrash(void)
{
    int index;
    u8 *indexPtr;
    const s16 *table;
    s16 delta;
    int prevIndex;

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp,
                     gCurrentSprite.xPosition);
        if ((gSpriteCollisionTileType & 0xF) != 0) {
            gCurrentSprite.pose = 29;
        } else {
            indexPtr = &gCurrentSprite.work3;
            index = *indexPtr;
            table = sUnk_8352B18;
            delta = table[index];
            if (delta == 0x7FFF) {
                prevIndex = index - 1;
                delta = table[prevIndex];
                gCurrentSprite.yPosition += delta;
            } else {
                *indexPtr = index + 1;
                gCurrentSprite.yPosition += delta;
            }
        }
    } else {
        gCurrentSprite.pose = 27;
    }
}

void UpdateMoguramenRecoverAfterCrash(void)
{
    int index;
    u8 *indexPtr;
    const s16 *table;
    s16 delta;
    int prevIndex;

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        gCurrentSprite.disableWarioCollisionTimer = 1;
        indexPtr = &gCurrentSprite.work3;
        index = *indexPtr;
        table = sMoguramenProjectileYSpeed;
        delta = table[index];
        if (delta == 0x7FFF) {
            prevIndex = index - 1;
            delta = table[prevIndex];
            gCurrentSprite.yPosition += delta;
        } else {
            *indexPtr = index + 1;
            gCurrentSprite.yPosition += delta;
        }
        if (gCurrentSprite.work3 == 14) {
            gCurrentSprite.drawPriority = 2;
        }
    } else {
        gCurrentSprite.pose = 27;
    }
}

void InitMoguramenLiftedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoguramenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitMoguramenLiftedRight(void)
{
    gCurrentSprite.pose = 82;
    InitMoguramenLiftedCommon();
}

void InitMoguramenLiftedLeft(void)
{
    gCurrentSprite.pose = 84;
    InitMoguramenLiftedCommon();
}

void InitMoguramenCarriedCommon(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sMoguramenStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitMoguramenCarriedRight(void)
{
    gCurrentSprite.pose = 88;
    InitMoguramenCarriedCommon();
}

void InitMoguramenCarriedLeft(void)
{
    gCurrentSprite.pose = 86;
    InitMoguramenCarriedCommon();
}

void InitMoguramenProjectile(void)
{
    struct PrimarySpriteData *currentSprite;
    u16 statusValue;
    register u16 finalStatus PIN("r0");
    u8 zeroByte;
    u16 zeroHalf;

    currentSprite = &gCurrentSprite;
    {
        register u16 status PIN("r1");
        u16 flags;
        register u16 flagsCopy PIN("r0");
        status = currentSprite->status;
        flags = SPRITE_STATUS_AFFINE | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
        flagsCopy = flags;
        /* agbcc otherwise schedules these zeros before the status flag copy. */
        zeroByte = 0;
        ASM_BARRIER(: "+r"(zeroByte));
        zeroHalf = 0;
        statusValue = flagsCopy | status;
    }
    finalStatus = statusValue & ~SPRITE_STATUS_HIDDEN;
    currentSprite->status = finalStatus;
    currentSprite->drawDistanceDown = 16;
    currentSprite->drawDistanceUp = 4;
    currentSprite->drawDistanceLeftRight = 16;
    currentSprite->hitboxExtentUp = 20;
    currentSprite->hitboxExtentDown = 16;
    currentSprite->hitboxExtentLeft = 20;
    currentSprite->hitboxExtentRight = 16;
    GP_SET(currentSprite->pOamData, sMoguramenProjectileOam);
    currentSprite->currentAnimationFrame = zeroByte;
    currentSprite->animationTimer = zeroHalf;
    currentSprite->warioCollision = 9;
    currentSprite->drawPriority = 1;
    currentSprite->pose = 16;
    currentSprite->work3 = zeroByte;
    currentSprite->work0 = zeroByte;
}

void UpdateMoguramenProjectileIdle(void)
{
    u8 index;
    const s16 *table;
    s16 delta;
    int prevIndex;

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += 4;
        gCurrentSprite.work0 += 4;
    } else {
        gCurrentSprite.xPosition -= 4;
        gCurrentSprite.work0 -= 4;
    }

    index = gCurrentSprite.work3;
    table = sMoguramenProjectileXSpeed;
    delta = table[index];
    if (delta == 0x7FFF) {
        prevIndex = index - 1;
        delta = table[prevIndex];
        gCurrentSprite.yPosition += delta;
    } else {
        gCurrentSprite.work3 = index + 1;
        gCurrentSprite.yPosition += delta;
    }

    if (gCurrentSprite.work3 == 25) {
        gCurrentSprite.drawPriority = 2;
    } else if (gCurrentSprite.work3 > 41) {
        gCurrentSprite.status = 0;
    }
}

void UpdateMoguramenProjectileThrown(void)
{
    gCurrentSprite.status = 0;
    if (gCurrentSprite.xPosition > gWarioData.xPosition) {
        SpawnTileDebrisParticle(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition, 0, 1, 0);
    } else {
        SpawnTileDebrisParticle(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition, 0, 2, 0);
    }
}

void SpriteMoguramen(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = 107;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitMoguramen();
            break;
        case SPOSE_IDLE_INIT:
            InitMoguramenIdle();
            /* fallthrough */
        case SPOSE_IDLE:
            UpdateMoguramenIdle();
            break;
        case SPOSE_6E:
            InitMoguramenAttackWindup();
            /* fallthrough */
        case SPOSE_6F:
            UpdateMoguramenAttackWindup();
            break;
        case SPOSE_11:
            InitMoguramenTurnAround();
            /* fallthrough */
        case SPOSE_12:
        case SPOSE_14:
        case SPOSE_16:
            UpdateMoguramenTurnAround();
            break;
        case SPOSE_17:
            InitMoguramenBurrowedWait();
            /* fallthrough */
        case SPOSE_18:
            UpdateMoguramenBurrowedWait();
            break;
        case SPOSE_19:
            InitMoguramenBurrowEmerge();
            /* fallthrough */
        case SPOSE_1A:
            UpdateMoguramenBurrowEmerge();
            break;
        case SPOSE_1B:
            InitMoguramenFalling();
            /* fallthrough */
        case SPOSE_1C:
            func_8023D48();
            break;
        case SPOSE_1D:
            InitMoguramenStunned();
            /* fallthrough */
        case SPOSE_1E:
            func_8023EE0();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            InitMoguramenTumbleRight();
            /* fallthrough */
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            InitMoguramenTumbleLeft();
            /* fallthrough */
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_PUSHED_RIGHT_INIT:
            InitMoguramenPushedRightWindup();
            goto call_pushed_right;
        case SPOSE_PUSHED_LEFT_INIT:
            InitMoguramenPushedLeftWindup();
            goto call_pushed_left;
        case SPOSE_27:
            InitMoguramenPushedRightSlide();
            goto call_pushed_right;
        case SPOSE_PUSHED_RIGHT:
        case SPOSE_28:
call_pushed_right:
            SpriteUtilPushedRight();
            break;
        case SPOSE_29:
            InitMoguramenPushedLeftSlide();
            goto call_pushed_left;
        case SPOSE_PUSHED_LEFT:
        case SPOSE_2A:
call_pushed_left:
            SpriteUtilPushedLeft();
            break;
        case SPOSE_2B:
            InitMoguramenBumpedRight();
            /* fallthrough */
        case SPOSE_2C:
            func_8024478();
            break;
        case SPOSE_2D:
            InitMoguramenBumpedLeft();
            /* fallthrough */
        case SPOSE_2E:
            func_802449C();
            break;
        case SPOSE_2F:
            func_80244C0();
            /* fallthrough */
        case SPOSE_30:
            func_80244E0();
            break;
        case 49:
            InitMoguramenDefeated();
            /* fallthrough */
        case 50:
            SpriteUtilDieAfterDelay();
            break;
        case SPOSE_33:
            InitMoguramenKnockbackRight();
            goto call_8024988;
        case SPOSE_34:
            goto call_8024988;
        case SPOSE_35:
            InitMoguramenKnockbackLeft();
            goto call_802476C;
        case SPOSE_36:
            goto call_802476C;
        case SPOSE_37:
            func_8024688();
            /* fallthrough */
        case SPOSE_38:
            func_80246B8();
            break;
        case SPOSE_39:
            func_802473C();
            /* fallthrough */
        case SPOSE_3A:
call_802476C:
            func_802476C();
            break;
        case SPOSE_3B:
            func_80247F0();
            /* fallthrough */
        case SPOSE_3C:
            func_8024820();
            break;
        case SPOSE_3D:
            func_80248A4();
            /* fallthrough */
        case SPOSE_3E:
            func_80248D4();
            break;
        case SPOSE_3F:
            func_8024958();
            /* fallthrough */
        case SPOSE_40:
call_8024988:
            func_8024988();
            break;
        case SPOSE_41:
            func_8024A0C();
            /* fallthrough */
        case SPOSE_42:
            func_8024A3C();
            break;
        case SPOSE_43:
            func_8024AC0();
            goto call_8024AD4;
        case SPOSE_44:
            goto call_8024AD4;
        case SPOSE_45:
            func_8024BEC();
            goto call_8024C00;
        case SPOSE_46:
            goto call_8024C00;
        case SPOSE_47:
            InitMoguramenAirBounceRight();
            /* fallthrough */
call_8024AD4:
            func_8024AD4();
            break;
        case SPOSE_48:
            goto call_8024AD4;
        case SPOSE_49:
            InitMoguramenAirBounceLeft();
            /* fallthrough */
call_8024C00:
            func_8024C00();
            break;
        case SPOSE_4A:
            goto call_8024C00;
        case SPOSE_4B:
            InitMoguramenThrownCrash();
            break;
        case SPOSE_4C:
            UpdateMoguramenThrownCrash();
            break;
        case SPOSE_71:
            UpdateMoguramenRecoverAfterCrash();
            break;
        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            InitMoguramenLiftedRight();
            break;
        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;
        case SPOSE_BEING_LIFTED_LEFT_INIT:
            InitMoguramenLiftedLeft();
            break;
        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;
        case SPOSE_CARRIED_RIGHT_INIT:
            InitMoguramenCarriedRight();
            break;
        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;
        case SPOSE_CARRIED_LEFT_INIT:
            InitMoguramenCarriedLeft();
            break;
        case SPOSE_CARRIED_LEFT:
            SpriteUtilCarryingSpriteLeft();
            break;
        case SPOSE_THROWN_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_SOFT;
            /* fallthrough */
        case SPOSE_THROWN_LEFT_SOFT:
            SpriteUtilThrownLeftSoft();
            break;
        case SPOSE_THROWN_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_HARD;
            /* fallthrough */
        case SPOSE_THROWN_LEFT_HARD:
            SpriteUtilThrownLeftHard();
            break;
        case SPOSE_THROWN_UP_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_SOFT;
            /* fallthrough */
        case SPOSE_THROWN_UP_LEFT_SOFT:
            SpriteUtilThrownUpLeftSoft();
            break;
        case SPOSE_THROWN_UP_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_HARD;
            /* fallthrough */
        case SPOSE_THROWN_UP_LEFT_HARD:
            SpriteUtilThrownUpLeftHard();
            break;
        case SPOSE_THROWN_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_SOFT;
            /* fallthrough */
        case SPOSE_THROWN_RIGHT_SOFT:
            SpriteUtilThrownRightSoft();
            break;
        case SPOSE_THROWN_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_HARD;
            /* fallthrough */
        case SPOSE_THROWN_RIGHT_HARD:
            SpriteUtilThrownRightHard();
            break;
        case SPOSE_THROWN_UP_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_SOFT;
            /* fallthrough */
        case SPOSE_THROWN_UP_RIGHT_SOFT:
            SpriteUtilThrownUpRightSoft();
            break;
        case SPOSE_THROWN_UP_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_HARD;
            /* fallthrough */
        case SPOSE_THROWN_UP_RIGHT_HARD:
            SpriteUtilThrownUpRightHard();
            break;
        case SPOSE_69:
            InitMoguramenShortTumbleLeft();
            break;
        case SPOSE_6A:
            InitMoguramenShortTumbleRight();
            break;
        default:
            KillMoguramen();
            break;
    }
}

void SpriteMoguramenProjectile(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitMoguramenProjectile();
            break;
        case SPOSE_IDLE:
            UpdateMoguramenProjectileIdle();
            break;
        case SPOSE_6B:
            SpriteUtilDie();
            break;
        default:
            UpdateMoguramenProjectileThrown();
            break;
    }

    gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work0), FixedInverse(0x100));
    gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work0), FixedInverse(0x100));
    gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work0), FixedInverse(0x100));
    gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work0), FixedInverse(0x100));
}

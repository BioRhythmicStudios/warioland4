#include "sprite_ai/utsuboanko.h"

#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"

enum UtsuboankoCollision {
    UTSUBOANKO_COLLISION_IDLE = 81,
    UTSUBOANKO_COLLISION_EMERGING = 82,
};

void InitUtsuboanko(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.drawDistanceDown = 48;
    gCurrentSprite.drawDistanceUp = 48;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 48;
    gCurrentSprite.hitboxExtentDown = 44;
    gCurrentSprite.hitboxExtentLeft = 64;
    gCurrentSprite.hitboxExtentRight = 60;
    gCurrentSprite.work0 = 0;
}

void UtsuboankoIdleInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sUtsuboankoIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = UTSUBOANKO_COLLISION_IDLE;
}

void UtsuboankoIdle(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.work0 != 0) {
        gCurrentSprite.work0--;
    } else if (SpriteUtilCheckWarioNearbyLeftRight(140, 256)) {
        GP_SET(gCurrentSprite.pOamData, sUtsuboankoEmergingOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_12;
        gCurrentSprite.work0 = 75;
    }
}

void UtsuboankoEmerge(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    register s32 nextTimer PIN("r2");
    u8 timer;

    sprite = &gCurrentSprite;
    {
        u32 address;

        address = 39;
        address += (u32) sprite;
        timerPointer = (u8 *) address;
        {
            register int oldTimer PIN("r0");
            oldTimer = *(u8 *) address;
            nextTimer = oldTimer - 1;
            *timerPointer = nextTimer;
        }
    }
    {
        register u32 shiftedTimer PIN("r0");

        shiftedTimer = nextTimer << 24;
        /* Keep the byte truncation in r0 before agbcc moves the timer to r1. */
        ASM_BARRIER(: "+r" (shiftedTimer));
        timer = shiftedTimer >> 24;
    }
    if (timer != 0) {
        if (timer > 51) {
            sprite->disableWarioCollisionTimer = 1;
            if (timer == 52) {
                m4aSongNumStart(SOUND_64);
            }
        } else if (timer == 10) {
            sprite->warioCollision = UTSUBOANKO_COLLISION_EMERGING;
        }
    } else {
        GP_SET(sprite->pOamData, sUtsuboankoRetreatingOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = SPOSE_14;
        {
            int resetTimer;
            resetTimer = 117;
            *timerPointer = resetTimer;
        }
    }
}

void UtsuboankoRetreat(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    register int oldTimer PIN("r0");
    register int nextTimer PIN("r1");
    register u32 shiftedTimer PIN("r0");

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    oldTimer = *timerPointer;
    nextTimer = oldTimer - 1;
    *timerPointer = nextTimer;
    shiftedTimer = nextTimer << 24;
    if (shiftedTimer != 0) {
        shiftedTimer >>= 24;
        if (shiftedTimer > 68) {
            return;
        }
    } else {
        sprite->pose = SPOSE_IDLE_INIT;
        *timerPointer = 70;
    }
    sprite->disableWarioCollisionTimer = 1;
}

void UtsuboankoHideInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sUtsuboankoHidingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_6D;
    gCurrentSprite.work0 = 70;
}

void UtsuboankoHide(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_IDLE_INIT;
        gCurrentSprite.work0 = 70;
    }
}

void SpriteUtsuboanko(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitUtsuboanko();
        case SPOSE_IDLE_INIT:
            UtsuboankoIdleInit();
        case SPOSE_IDLE:
            UtsuboankoIdle();
            break;
        case SPOSE_12:
            UtsuboankoEmerge();
            break;
        case SPOSE_14:
            UtsuboankoRetreat();
            break;
        case SPOSE_6C:
            UtsuboankoHideInit();
        case SPOSE_6D:
            UtsuboankoHide();
            break;
        default:
            gCurrentSprite.pose = SPOSE_6C;
            gCurrentSprite.disableWarioCollisionTimer = 1;
            break;
    }
}

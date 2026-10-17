#include "sprite_ai/boss_corridor_cat.h"

#include "global_data.h"
#include "sound.h"

void InitBossCorridorCat(void)
{
    u8 *persistentData;
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *caseSprite;
    int zeroHalf;
    register int zeroByte PIN("r2");
    register int offset PIN("r0");
    int slot;
    u16 oldStatus;
    register u16 status PIN("r0");
    int random;

    persistentData = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    offset = gCurrentRoom;
    offset <<= 6;
    slot = sprite->roomSlot;
    offset += slot;
    offset += (int)persistentData;
    zeroHalf = 0;
    *(u8 *)offset = 17;
    oldStatus = sprite->status;
    status = SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND;
    zeroByte = 0;
    status |= oldStatus;
    sprite->status = status;
    sprite->warioCollision = zeroByte;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = zeroByte;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 4;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->work3 = zeroByte;

    random = (gRandomSeed & 7) - 2;
    caseSprite = sprite;
    switch (random) {
        case 0:
            caseSprite->pose = 110;
            GP_SET(caseSprite->pOamData, sBossCorridorCatWaitToTurnOam);
            caseSprite->work0 = 30;
            caseSprite->xPosition += 888;
            break;

        case 1:
            caseSprite->pose = 22;
            GP_SET(caseSprite->pOamData, sBossCorridorCatWalkLeftOam);
            caseSprite->work0 = 32;
            caseSprite->xPosition += 960;
            caseSprite->status |= SPRITE_STATUS_FACING_RIGHT;
            m4aSongNumStart(SE_BOSS_CORRIDOR_CAT_DASH);
            break;

        case 2:
            caseSprite->pose = 20;
            GP_SET(caseSprite->pOamData, sBossCorridorCatWaitRightOam);
            caseSprite->work0 = 24;
            caseSprite->xPosition += 448;
            break;

        case 3:
            caseSprite->pose = 18;
            GP_SET(caseSprite->pOamData, sBossCorridorCatWaitLeftOam);
            caseSprite->work0 = 20;
            caseSprite->xPosition += 640;
            caseSprite->status |= SPRITE_STATUS_FACING_RIGHT;
            break;

        case 4:
            caseSprite->pose = 16;
            GP_SET(caseSprite->pOamData, sBossCorridorCatRunOam);
            caseSprite->work0 = 56;
            caseSprite->xPosition += 256;
            m4aSongNumStart(SE_BOSS_CORRIDOR_CAT_DASH);
            break;

        default:
            sprite->status = zeroHalf;
            break;
    }
}

void BossCorridorCatWaitToTurn(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    int pointerValue;
    int timerValue;
    u8 timer;
    u16 oldStatus;
    s32 status;
    register int zero PIN("r2");

    sprite = &gCurrentSprite;
    pointerValue = 39;
    pointerValue += (int)sprite;
    timerPointer = (u8 *)pointerValue;
    timerValue = *timerPointer;
    timerValue--;
    *timerPointer = timerValue;
    timer = (u8)timerValue;
    if (timer == 0) {
        oldStatus = sprite->status;
        status = SPRITE_STATUS_FACING_RIGHT;
        zero = 0;
        status |= oldStatus;
        sprite->status = status;
        sprite->pose = 111;
        timerValue = 16;
        *timerPointer = timerValue;
        GP_SET(sprite->pOamData, sBossCorridorCatTurnOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = timer;
    }
}

void BossCorridorCatStartFastLeftDash(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    int timerValue;
    u8 timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    timerValue = *timerPointer;
    timerValue--;
    *timerPointer = timerValue;
    timer = (u8)timerValue;
    if (timer == 0) {
        sprite->pose = 112;
        *timerPointer = 10;
        GP_SET(sprite->pOamData, sBossCorridorCatRunOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        m4aSongNumStart(SE_BOSS_CORRIDOR_CAT_DASH);
    }
}

void BossCorridorCatFastLeftDash(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    u32 oldTimer;
    u32 nextTimer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    oldTimer = *timerPointer;
    nextTimer = oldTimer - 1;
    *timerPointer = nextTimer;
    oldTimer = nextTimer << 24;
    oldTimer = (u32)oldTimer >> 24;
    if (oldTimer == 0) {
        sprite->status = oldTimer;
    } else {
        sprite->xPosition -= 10;
        oldTimer = nextTimer << 24;
        oldTimer = (u32)oldTimer >> 24;
        if (oldTimer <= 6) {
            sprite->status ^= SPRITE_STATUS_HIDDEN;
        }
    }
}

void BossCorridorCatSlowLeftDash(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    u32 oldTimer;
    u32 nextTimer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    oldTimer = *timerPointer;
    nextTimer = oldTimer - 1;
    *timerPointer = nextTimer;
    oldTimer = nextTimer << 24;
    oldTimer = (u32)oldTimer >> 24;
    if (oldTimer == 0) {
        sprite->status = oldTimer;
    } else {
        sprite->xPosition -= 5;
        oldTimer = nextTimer << 24;
        oldTimer = (u32)oldTimer >> 24;
        if (oldTimer <= 6) {
            sprite->status ^= SPRITE_STATUS_HIDDEN;
        }
    }
}

void BossCorridorCatWaitToDashRight(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    int timerValue;
    u8 timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    timerValue = *timerPointer;
    timerValue--;
    *timerPointer = timerValue;
    timer = (u8)timerValue;
    if (timer == 0) {
        sprite->pose = 16;
        *timerPointer = 36;
        GP_SET(sprite->pOamData, sBossCorridorCatRunOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        m4aSongNumStart(SE_BOSS_CORRIDOR_CAT_DASH);
    }
}

void BossCorridorCatWaitToDashRightShort(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    int timerValue;
    u8 timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    timerValue = *timerPointer;
    timerValue--;
    *timerPointer = timerValue;
    timer = (u8)timerValue;
    if (timer == 0) {
        sprite->pose = 16;
        *timerPointer = 16;
        GP_SET(sprite->pOamData, sBossCorridorCatRunOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        m4aSongNumStart(SE_BOSS_CORRIDOR_CAT_DASH);
        sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
    }
}

void BossCorridorCatDashRight(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    u32 oldTimer;
    u32 nextTimer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    oldTimer = *timerPointer;
    nextTimer = oldTimer - 1;
    *timerPointer = nextTimer;
    oldTimer = nextTimer << 24;
    oldTimer = (u32)oldTimer >> 24;
    if (oldTimer == 0) {
        sprite->status = oldTimer;
    } else {
        sprite->xPosition += 10;
        oldTimer = nextTimer << 24;
        oldTimer = (u32)oldTimer >> 24;
        if (oldTimer <= 6) {
            sprite->status ^= SPRITE_STATUS_HIDDEN;
        }
    }
}

void SpriteBossCorridorCat(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitBossCorridorCat();
            break;
        case 1:
            gCurrentSprite.status = 0;
            break;
        case 110:
            BossCorridorCatWaitToTurn();
            break;
        case 111:
            BossCorridorCatStartFastLeftDash();
            break;
        case 112:
            BossCorridorCatFastLeftDash();
            break;
        case 22:
            BossCorridorCatSlowLeftDash();
            break;
        case 20:
            BossCorridorCatWaitToDashRight();
            break;
        case 18:
            BossCorridorCatWaitToDashRightShort();
            break;
        case 16:
            BossCorridorCatDashRight();
            break;
    }
}

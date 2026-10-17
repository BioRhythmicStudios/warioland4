#include "sprite_ai/tobawani.h"

#include "score.h"
#include "sound.h"
#include "sprite_util.h"
#include "wario.h"


void TobawaniEnterWater(void)
{
    gCurrentSprite.yPosition = gRoomSpriteData[gCurrentSprite.roomSlot][0] * 64 + 64;
    gCurrentSprite.pose = 39;
    gCurrentSprite.warioCollision = 80;
    m4aSongNumStart(SE_ENTER_WATER);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition, SSPRITE_07);
    gCurrentSprite.status &= ~0x200;
    gCurrentSprite.status |= 0x400;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 144;
    gCurrentSprite.hitboxExtentLeft = 52;
    gCurrentSprite.hitboxExtentRight = 48;
}

void InitTobawani(void)
{
    gCurrentSprite.status |= 0x400;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 144;
    gCurrentSprite.hitboxExtentLeft = 52;
    gCurrentSprite.hitboxExtentRight = 48;
    gCurrentSprite.work0 = 60;
}

void TobawaniSetPatrol(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniPatrolOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 80;
}

void TobawaniPatrol(void)
{
    u32 horizontal;
    u32 vertical;
    struct PrimarySpriteData* sprite;

    horizontal = 0;
    vertical = 0;
    if (gWarioData.reaction == 1) {
        horizontal = SpriteUtilCheckWarioNearbyLeftRight(70, 520);
    } else {
        vertical = SpriteUtilCheckWarioNearbyAboveBelow(520, 70);
    }

    sprite = &gCurrentSprite;
    {
        u8* timerPointer;
        u32 timer;
        u32 newTimer;

        timerPointer = &sprite->work0;
        timer = *timerPointer;
        newTimer = timer - 1;
        *timerPointer = newTimer;
        if (timer == 0) {
            goto idle;
        }
    }
    if (vertical == 1) {
        sprite->pose = 112;
        return;
    }

    {
        u32 xPosition;

        if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
            if (horizontal == 8) {
                goto attack;
            }
            func_8023BFC((u16)(sprite->yPosition - 16),
                         (u16)(sprite->xPosition + 110));
            if ((gSpriteCollisionTileType & 15) != 0) {
                goto idle;
            }
            xPosition = sprite->xPosition + 3;
            goto updatePosition;
        }
        if (horizontal == 4) {
attack:
            sprite->pose = 110;
            return;
        }
        func_8023BFC((u16)(sprite->yPosition - 16),
                     (u16)(sprite->xPosition - 110));
        if ((gSpriteCollisionTileType & 15) != 0) {
idle:
            sprite->pose = 17;
            return;
        }
        xPosition = sprite->xPosition - 3;
updatePosition:
        sprite->xPosition = xPosition;
    }
}

void TobawaniSetTurnAround(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniTurnOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 5;
    gCurrentSprite.pose = 18;
    gCurrentSprite.warioCollision = 80;
}

void TobawaniTurnAround(void)
{
    struct PrimarySpriteData* sprite;
    register u8* timer PIN("r12");
    register u32 temp PIN("r0");
    s16 remaining;
    u32 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    temp = *timer;
    temp--;
    *timer = temp;
    remaining = (u8)temp;
    if (remaining == 0) {
        if (sprite->pose == 18) {
            temp = sprite->status;
            temp ^= SPRITE_STATUS_FACING_RIGHT;
            zero = 0;
            sprite->status = temp;
            GP_SET(sprite->pOamData, sTobawaniTurnOam);
            sprite->currentAnimationFrame = zero;
            sprite->animationTimer = remaining;
            temp = 20;
            sprite->pose = temp;
            temp = 3;
            *timer = temp;
        } else {
            temp = 120;
            *timer = temp;
            temp = 15;
            sprite->pose = temp;
        }
    }
}

void TobawaniSetCharge(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniChargeOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 54;
    gCurrentSprite.pose = 111;
    gCurrentSprite.warioCollision = 79;
}

void TobawaniCharge(void)
{
    struct PrimarySpriteData* sprite;
    u32 xPosition;

    sprite = &gCurrentSprite;
    sprite->work0--;
    if (sprite->work0 == 0) {
        sprite->pose = 15;
        sprite->work0 = 80;
        return;
    }

    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC((u16)(sprite->yPosition - 16), (u16)(sprite->xPosition + 110));
        if ((gSpriteCollisionTileType & 15) != 0) {
            goto blocked;
        }
        xPosition = sprite->xPosition + 6;
        goto updatePosition;
    }

    func_8023BFC((u16)(sprite->yPosition - 16), (u16)(sprite->xPosition - 110));
    if ((gSpriteCollisionTileType & 15) != 0) {
blocked:
        sprite->pose = 17;
        return;
    }
    xPosition = sprite->xPosition - 6;
updatePosition:
    sprite->xPosition = xPosition;
}

void TobawaniSetKnockback(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniKnockbackOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 113;
    gCurrentSprite.warioCollision = 55;
    gCurrentSprite.status &= ~0x400;
    gCurrentSprite.hitboxExtentUp = 96;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 24;
    gCurrentSprite.hitboxExtentRight = 20;
    m4aSongNumStart(SE_TOBAWANI_KNOCKBACK);
}

void TobawaniKnockback(void)
{
    struct PrimarySpriteData* sprite;
    u8* indexPointer;
    s16 index;
    u16 velocity;
    int zero;
    u8 nextIndex;
    u8* storePointer;
    const s16* table;
    const s16* entry;
    u16 y;

    sprite = &gCurrentSprite;
    indexPointer = &sprite->work3;
    index = *indexPointer;
    table = sTobawaniKnockbackYVelocity;
    entry = table + index;
    velocity = *entry;
    zero = 0;
    if (*(s16*)entry == S16_MAX) {
        sprite->pose = 114;
    } else {
        nextIndex = index + 1;
        storePointer = indexPointer;
        *storePointer = nextIndex;
        y = sprite->yPosition;
        sprite->yPosition = y + velocity;
    }
}

void TobawaniSetFall(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 115;
}

void TobawaniFall(void)
{
    struct PrimarySpriteData* sprite;
    const s16* table;
    int zero;
    u32 temp;
    u32 auxiliary;
    u32 index;
    u32 velocity;
    u8* indexPointer;

    sprite = &gCurrentSprite;
    func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        TobawaniEnterWater();
    } else {
        indexPointer = &sprite->work3;
        index = *indexPointer;
        table = sTobawaniFallYVelocity;
        temp = index;
        temp <<= 1;
        temp += (u32)table;
        velocity = *(u16*)temp;
        zero = 0;
        auxiliary = *(s16*)temp;
        temp = S16_MAX;
        if (auxiliary == temp) {
            auxiliary = index - 1;
            auxiliary <<= 1;
            auxiliary += (u32)table;
            temp = sprite->yPosition;
            auxiliary = *(u16*)auxiliary;
            temp += auxiliary;
        } else {
            temp = index + 1;
            auxiliary = (u32)indexPointer;
            *(u8*)auxiliary = temp;
            temp = sprite->yPosition;
            temp += velocity;
        }
        sprite->yPosition = temp;
    }
}

void TobawaniSetRecovery(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniRecoveryOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 80;
    gCurrentSprite.pose = 111;
}

void TobawaniPrepareFallOffscreen(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, SSPRITE_06);
    gCurrentSprite.status &= ~0x100;
    gCurrentSprite.status |= 0x200;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
}

void TobawaniStartFallOffscreenRight(void)
{
    gCurrentSprite.pose = 32;
    TobawaniPrepareFallOffscreen();
}

void TobawaniStartFallOffscreenLeft(void)
{
    gCurrentSprite.pose = 34;
    TobawaniPrepareFallOffscreen();
}

void TobawaniSetWallFall(void)
{
    GP_SET(gCurrentSprite.pOamData, sTobawaniFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
}

void TobawaniStartWallFallRight(void)
{
    gCurrentSprite.pose = 52;
    TobawaniSetWallFall();
}

void TobawaniStartWallFallLeft(void)
{
    gCurrentSprite.pose = 54;
    TobawaniSetWallFall();
}

void TobawaniWallFall(void)
{
    struct PrimarySpriteData* sprite;
    const s16* table;
    int zero;
    u32 temp;
    u32 auxiliary;
    u32 index;
    u32 velocity;
    u8* indexPointer;

    sprite = &gCurrentSprite;
    func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        TobawaniEnterWater();
    } else if ((gSpriteCollisionResult & 15) != 0) {
        if (sprite->pose == 52) {
            sprite->pose = 31;
        } else {
            sprite->pose = 33;
        }
    } else {
        indexPointer = &sprite->work3;
        index = *indexPointer;
        table = sTobawaniWallFallYVelocity;
        temp = index;
        temp <<= 1;
        temp += (u32)table;
        velocity = *(u16*)temp;
        zero = 0;
        auxiliary = *(s16*)temp;
        temp = S16_MAX;
        if (auxiliary == temp) {
            auxiliary = index - 1;
            auxiliary <<= 1;
            auxiliary += (u32)table;
            temp = sprite->yPosition;
            auxiliary = *(u16*)auxiliary;
            temp += auxiliary;
        } else {
            temp = index + 1;
            auxiliary = (u32)indexPointer;
            *(u8*)auxiliary = temp;
            temp = sprite->yPosition;
            temp += velocity;
        }
        sprite->yPosition = temp;
    }
}

void TobawaniWallFallRight(void)
{
    gCurrentSprite.xPosition += 2;
    func_8023BFC(gCurrentSprite.yPosition - 32,
                 gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
    if (gSpriteCollisionTileType == 17) {
        gCurrentSprite.pose = 54;
        m4aSongNumStart(SE_WALL_BOUNCE);
    }
    TobawaniWallFall();
}

void TobawaniWallFallLeft(void)
{
    gCurrentSprite.xPosition -= 2;
    func_8023BFC(gCurrentSprite.yPosition - 32,
                 gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
    if (gSpriteCollisionTileType == 17) {
        gCurrentSprite.pose = 52;
        m4aSongNumStart(SE_WALL_BOUNCE);
    }
    TobawaniWallFall();
}

void TobawaniDie(void)
{
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
}

void SpriteTobawani(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitTobawani();
        case 15:
            TobawaniSetPatrol();
        case 16:
            TobawaniPatrol();
            break;
        case 17:
            TobawaniSetTurnAround();
        case 18:
        case 20:
            TobawaniTurnAround();
            break;
        case 110:
            TobawaniSetCharge();
        case 111:
            TobawaniCharge();
            break;
        case 112:
            TobawaniSetKnockback();
        case 113:
            TobawaniKnockback();
            break;
        case 114:
            TobawaniSetFall();
        case 115:
            TobawaniFall();
            break;
        case 39:
        case 41:
            TobawaniSetRecovery();
            break;
        case 31:
        case 79:
        case 106:
            TobawaniStartFallOffscreenRight();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
        case 105:
            TobawaniStartFallOffscreenLeft();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
        case 51:
            TobawaniStartWallFallRight();
            break;
        case 52:
            TobawaniWallFallRight();
            break;
        case 37:
        case 53:
            TobawaniStartWallFallLeft();
            break;
        case 54:
            TobawaniWallFallLeft();
            break;
        default:
            TobawaniDie();
            break;
    }
}

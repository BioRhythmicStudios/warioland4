#include "sprite_ai/goggley_blade.h"

#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"


void InitGoggleyBlade(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.warioCollision = 75;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 16;
}

void GoggleyBladeSetHorizontalMovement(void)
{
    GP_SET(gCurrentSprite.pOamData, sGoggleyBladeHorizontalOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.work2 = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 75;
    gCurrentSprite.hitboxExtentUp = 40;
    gCurrentSprite.hitboxExtentDown = 24;
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 24;
        gCurrentSprite.hitboxExtentRight = 40;
    } else {
        gCurrentSprite.hitboxExtentLeft = 40;
        gCurrentSprite.hitboxExtentRight = 24;
    }
}

void GoggleyBladeHorizontalMovement(void)
{
    u32 nearby;
    u32 loops;
    u8 globalID;
    u8 index;
    s16 velocity;

    nearby = SpriteUtilCheckWarioNearbyLeftRight(70, 450);
    globalID = gCurrentSprite.globalID;
    loops = 3;
    if (globalID == 75) {
        loops = 1;
    }

    index = gCurrentSprite.work2;
    velocity = sGoggleyBladeMovementVelocity[index];
    if (velocity == S16_MAX) {
        velocity = sGoggleyBladeMovementVelocity[0];
        index = 0;
        gCurrentSprite.work0++;
    }
    gCurrentSprite.work2 = index + 1;

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        if (nearby == 8) {
            gCurrentSprite.pose = 112;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 76);
        if ((gSpriteCollisionTileType & 15) != 0) {
            gCurrentSprite.pose = 19;
            return;
        }
        gCurrentSprite.xPosition += velocity;
        if (gCurrentSprite.work0 == loops) {
            gCurrentSprite.pose = 19;
        }
    } else {
        if (nearby == 4) {
            gCurrentSprite.pose = 112;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 76);
        if ((gSpriteCollisionTileType & 15) != 0) {
            gCurrentSprite.pose = 17;
            return;
        }
        gCurrentSprite.xPosition -= velocity;
        if (gCurrentSprite.work0 == loops) {
            gCurrentSprite.pose = 17;
        }
    }
}

void GoggleyBladeSetTurnDown(void)
{
    GP_SET(gCurrentSprite.pOamData, sGoggleyBladeTurnDownOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 18;
}

void GoggleyBladeTurnDown(void)
{
    u8* indexPointer;
    struct PrimarySpriteData* sprite;
    u32 index;
    s32 velocity;

    {
        u32 base;
        base = (u32)&gCurrentSprite;
        indexPointer = (u8*)(base + 42);
        index = *indexPointer;
        sprite = (struct PrimarySpriteData*)base;
    }
    if (index == 8) {
        sprite->status ^= SPRITE_STATUS_FACING_RIGHT;
        sprite->warioCollision = 76;
    }
    {
        const s16* table;
        int zero;
        register u32 temp PIN("r0");
        s32 auxiliary;

        table = sGoggleyBladeTurnVelocity;
        temp = index;
        temp <<= 1;
        temp += (u32)table;
        velocity = *(u16*)temp;
        zero = 0;
        auxiliary = *(s16*)(temp + zero);
        temp = S16_MAX;
        if (auxiliary == temp) {
            temp = index - 1;
            temp <<= 1;
            temp += (u32)table;
            velocity = *(u16*)temp;
            sprite->pose = 110;
        } else {
            temp = index + 1;
            *(u8*)indexPointer = temp;
        }
    }

    {
        register struct PrimarySpriteData* moveSprite PIN("r4");
        moveSprite = sprite;
        func_8023BFC(moveSprite->yPosition + 56, moveSprite->xPosition);
        if ((gSpriteCollisionTileType & 15) == 0) {
            moveSprite->yPosition += velocity;
        }
        func_8023BFC(sprite->yPosition, sprite->xPosition - 56);
        if ((gSpriteCollisionTileType & 15) == 0) {
            sprite->xPosition--;
        }
    }

}

void GoggleyBladeSetTurnUp(void)
{
    GP_SET(gCurrentSprite.pOamData, sGoggleyBladeTurnUpOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 20;
}

void GoggleyBladeTurnUp(void)
{
    struct PrimarySpriteData* sprite;
    u8* indexPointer;
    u32 index;
    const s16* table;
    u32 velocity;
    int zero;
    u32 temp;
    s32 auxiliary;

    sprite = &gCurrentSprite;
    indexPointer = &sprite->work3;
    index = *indexPointer;
    if (index == 8) {
        sprite->status ^= SPRITE_STATUS_FACING_RIGHT;
        sprite->warioCollision = 77;
    }
    table = sGoggleyBladeTurnVelocity;
    temp = index;
    temp <<= 1;
    temp += (u32)table;
    velocity = *(u16*)temp;
    zero = 0;
    auxiliary = *(s16*)temp;
    temp = S16_MAX;
    if (auxiliary == temp) {
        temp = index - 1;
        temp <<= 1;
        temp += (u32)table;
        velocity = *(u16*)temp;
        sprite->pose = 110;
    } else {
        temp = index + 1;
        auxiliary = (u32)indexPointer;
        *(u8*)auxiliary = temp;
    }

    func_8023BFC(gCurrentSprite.yPosition - 56, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1 && (gSpriteCollisionTileType & 15) == 0) {
        gCurrentSprite.yPosition -= velocity;
    }
    func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 56);
    if ((gSpriteCollisionTileType & 15) == 0) {
        gCurrentSprite.xPosition++;
    }
}

void GoggleyBladeSetVerticalMovement(void)
{
    gCurrentSprite.hitboxExtentLeft = 24;
    gCurrentSprite.hitboxExtentRight = 40;
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeVerticalDownOam);
        gCurrentSprite.warioCollision = 76;
        gCurrentSprite.hitboxExtentUp = 24;
        gCurrentSprite.hitboxExtentDown = 40;
    } else {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeVerticalUpOam);
        gCurrentSprite.warioCollision = 77;
        gCurrentSprite.hitboxExtentUp = 40;
        gCurrentSprite.hitboxExtentDown = 24;
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.work2 = 0;
    gCurrentSprite.pose = 111;
}

void GoggleyBladeVerticalMovement(void)
{
    u32 nearby;
    u32 loops;
    u8 globalID;
    u8 index;
    s16 velocity;

    nearby = SpriteUtilCheckWarioNearbyAboveBelow(450, 70);
    globalID = gCurrentSprite.globalID;
    loops = 1;
    if (globalID == 75) {
        loops = 3;
    }

    index = gCurrentSprite.work2;
    velocity = sGoggleyBladeMovementVelocity[index];
    if (velocity == S16_MAX) {
        velocity = sGoggleyBladeMovementVelocity[0];
        index = 0;
        gCurrentSprite.work0++;
    }
    gCurrentSprite.work2 = index + 1;

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        if (nearby == 2) {
            gCurrentSprite.pose = 112;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition + 64, gCurrentSprite.xPosition);
        if ((gSpriteCollisionTileType & 15) != 0) {
            gCurrentSprite.pose = 21;
            return;
        }
        gCurrentSprite.yPosition += velocity;
    } else {
        if (nearby == 1) {
            gCurrentSprite.pose = 112;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition - 76, gCurrentSprite.xPosition);
        if (gBackgroundCollisionData.waterType != 1 || (gSpriteCollisionTileType & 15) != 0) {
            gCurrentSprite.pose = 21;
            return;
        }
        gCurrentSprite.yPosition -= velocity;
    }
    if (gCurrentSprite.work0 == loops) {
        gCurrentSprite.pose = 21;
    }
}

void GoggleyBladeSetTurnHorizontal(void)
{
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 22;
}

void GoggleyBladeTurnHorizontal(void)
{
    u8 index;
    s16 velocity;

    index = gCurrentSprite.work3;
    velocity = sGoggleyBladeTurnVelocity[index];
    if (velocity == S16_MAX) {
        velocity = sGoggleyBladeTurnVelocity[index - 1];
        gCurrentSprite.pose = 15;
    } else {
        gCurrentSprite.work3 = index + 1;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC(gCurrentSprite.yPosition + 56, gCurrentSprite.xPosition);
        if ((gSpriteCollisionTileType & 15) == 0) {
            gCurrentSprite.yPosition++;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 56);
        if ((gSpriteCollisionTileType & 15) == 0) {
            gCurrentSprite.xPosition += velocity;
        }
    } else {
        func_8023BFC(gCurrentSprite.yPosition - 56, gCurrentSprite.xPosition);
        if (gBackgroundCollisionData.waterType == 1 && (gSpriteCollisionTileType & 15) == 0) {
            gCurrentSprite.yPosition--;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 56);
        if ((gSpriteCollisionTileType & 15) == 0) {
            gCurrentSprite.xPosition -= velocity;
        }
    }
}

void GoggleyBladeSetKnockback(void)
{
    if (gCurrentSprite.warioCollision == 76) {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeKnockbackVerticalDownOam);
    } else if (gCurrentSprite.warioCollision == 77) {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeKnockbackVerticalUpOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeKnockbackHorizontalOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.pose = 36;
}

void GoggleyBladeKnockback(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.warioCollision == 76) {
            func_8023BFC(gCurrentSprite.yPosition + 76, gCurrentSprite.xPosition);
            if ((gSpriteCollisionTileType & 15) != 0) {
                gCurrentSprite.pose = 21;
            } else {
                gCurrentSprite.yPosition += 8;
            }
        } else if (gCurrentSprite.warioCollision == 77) {
            func_8023BFC(gCurrentSprite.yPosition - 76, gCurrentSprite.xPosition);
            if (gBackgroundCollisionData.waterType != 1 || (gSpriteCollisionTileType & 15) != 0) {
                gCurrentSprite.pose = 21;
            } else {
                gCurrentSprite.yPosition -= 8;
            }
        } else if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + 76);
            if ((gSpriteCollisionTileType & 15) != 0) {
                gCurrentSprite.pose = 19;
            } else {
                gCurrentSprite.xPosition += 8;
            }
        } else {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 76);
            if ((gSpriteCollisionTileType & 15) != 0) {
                gCurrentSprite.pose = 17;
            } else {
                gCurrentSprite.xPosition -= 8;
            }
        }
    } else if (gCurrentSprite.warioCollision == 76) {
        gCurrentSprite.pose = 21;
    } else if (gCurrentSprite.warioCollision == 77) {
        gCurrentSprite.pose = 110;
    } else {
        gCurrentSprite.pose = 15;
    }
}

void GoggleyBladeSetStunned(void)
{
    if (gCurrentSprite.warioCollision == 76) {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeKnockbackVerticalDownOam);
    } else if (gCurrentSprite.warioCollision == 77) {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeKnockbackVerticalUpOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeKnockbackHorizontalOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 30;
    gCurrentSprite.pose = 113;
}

void GoggleyBladeStunned(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        if (gCurrentSprite.warioCollision == 76) {
            GP_SET(gCurrentSprite.pOamData, sGoggleyBladeHurtVerticalDownOam);
        } else if (gCurrentSprite.warioCollision == 77) {
            GP_SET(gCurrentSprite.pOamData, sGoggleyBladeHurtVerticalUpOam);
        } else {
            GP_SET(gCurrentSprite.pOamData, sGoggleyBladeHurtHorizontalOam);
        }
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.work0 = 100;
        gCurrentSprite.pose = 36;
        m4aSongNumStart(SE_GOGGLEY_BLADE_HIT);
    }
}

void DefeatGoggleyBlade(void)
{
    if (gCurrentSprite.warioCollision == 76) {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeDefeatedVerticalDownOam);
    } else if (gCurrentSprite.warioCollision == 77) {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeDefeatedVerticalUpOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sGoggleyBladeDefeatedHorizontalOam);
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 1);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, SSPRITE_06);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void GoggleyBladeFallRight(void)
{
    gCurrentSprite.pose = 32;
    DefeatGoggleyBlade();
}

void GoggleyBladeFallLeft(void)
{
    gCurrentSprite.pose = 34;
    DefeatGoggleyBlade();
}

void SpriteGoggleyBlade(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitGoggleyBlade();
        case 15:
            GoggleyBladeSetHorizontalMovement();
        case 16:
            GoggleyBladeHorizontalMovement();
            break;
        case 17:
            GoggleyBladeSetTurnDown();
        case 18:
            GoggleyBladeTurnDown();
            break;
        case 19:
            GoggleyBladeSetTurnUp();
        case 20:
            GoggleyBladeTurnUp();
            break;
        case 110:
            GoggleyBladeSetVerticalMovement();
        case 111:
            GoggleyBladeVerticalMovement();
            break;
        case 112:
            GoggleyBladeSetStunned();
        case 113:
            GoggleyBladeStunned();
            break;
        case 21:
            GoggleyBladeSetTurnHorizontal();
        case 22:
            GoggleyBladeTurnHorizontal();
            break;
        case 35:
        case 37:
        case 39:
        case 41:
        case 51:
        case 53:
            GoggleyBladeSetKnockback();
        case 36:
            GoggleyBladeKnockback();
            break;
        case 31:
            GoggleyBladeFallRight();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
            GoggleyBladeFallLeft();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        default:
            SpriteUtilDie();
            break;
    }
}

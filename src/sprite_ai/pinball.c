#include "sprite_ai/pinball.h"

#include "fixed_point.h"
#include "global_data.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "types.h"
#include "wario.h"
#include "voice_set.h"


void PinballReactToScreenShake(void)
{
    if ((gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) && gScreenShakeY.duration > 41) {
        if (gWarioData.horizontalDirection & 32) {
            gCurrentSprite.pose = 53;
        } else {
            gCurrentSprite.pose = 51;
        }
    }
}

void PinballInit(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 56;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 28;
    gCurrentSprite.hitboxExtentRight = 24;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sPinballOam);
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.pose = 15;
}

void PinballIdleInit(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_AFFINE;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.status &= ~(SPRITE_STATUS_CAN_HIT_OTHER_SPRITES | SPRITE_STATUS_MAYBE_DEAD);
}

void PinballIdle(void)
{
    PinballReactToScreenShake();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (!gSpriteCollisionTileType) {
            gCurrentSprite.pose = 27;
        }
    }
}

void PinballLandingInit(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void PinballImpactRightInit(void)
{
    gCurrentSprite.pose = 44;
    gCurrentSprite.work1 = 0;
}

void PinballImpactLeftInit(void)
{
    gCurrentSprite.pose = 46;
    gCurrentSprite.work1 = 0;
}

void PinballThrownUpRightInit(void)
{
    gCurrentSprite.pose = 72;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void PinballThrownUpLeftInit(void)
{
    gCurrentSprite.pose = 74;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void PinballKnockedRightInit(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void PinballKnockedLeftInit(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void PinballTackledRightInit(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void PinballTackledLeftInit(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void PinballKnockAwayFromWario(void)
{
    if (gCurrentSprite.xPosition > gWarioData.xPosition) {
        PinballKnockedRightInit();
    } else {
        PinballKnockedLeftInit();
    }
}

void PinballLiftedRightInit(void)
{
    gCurrentSprite.pose = 82;
}

void PinballLiftedLeftInit(void)
{
    gCurrentSprite.pose = 84;
}

void PinballCarriedRightInit(void)
{
    gCurrentSprite.pose = 88;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void PinballCarriedLeftInit(void)
{
    gCurrentSprite.pose = 86;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}


void PinballFindTouchingSprite(void)
{
    s32 i;
    u8 gfxSlot;
    u16 currentY;
    u16 currentX;
    u16 currentTop;
    u16 currentBottom;
    u16 currentLeft;
    u16 currentRight;
    struct PrimarySpriteData *sprite;
    u16 spriteY;
    u16 spriteX;
    u16 spriteTop;
    u16 spriteBottom;
    u16 spriteLeft;
    u16 spriteRight;

    if (!(gCurrentSprite.status & SPRITE_STATUS_ONSCREEN)) {
        return;
    }

    gfxSlot = gCurrentSprite.globalID;
    currentY = gCurrentSprite.yPosition + 0x1000;
    currentX = gCurrentSprite.xPosition + 0x1000;
    currentTop = currentY - gCurrentSprite.hitboxExtentUp;
    currentBottom = currentY + gCurrentSprite.hitboxExtentDown;
    currentLeft = currentX - gCurrentSprite.hitboxExtentLeft;
    currentRight = currentX + gCurrentSprite.hitboxExtentRight;

    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        {
            struct PrimarySpriteData *sprites;
            u32 offset;

            sprites = gSpriteData;
            offset = i * sizeof(struct PrimarySpriteData);
            sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
        }
        if ((sprite->status & 0x613) != (SPRITE_STATUS_EXISTS | SPRITE_STATUS_ONSCREEN)) {
            continue;
        }
        if (sprite->globalID == gfxSlot) {
            continue;
        }

        spriteY = sprite->yPosition + 0x1000;
        spriteX = sprite->xPosition + 0x1000;
        spriteTop = spriteY - sprite->hitboxExtentUp;
        spriteBottom = spriteY + sprite->hitboxExtentDown;
        spriteLeft = spriteX - sprite->hitboxExtentLeft;
        spriteRight = spriteX + sprite->hitboxExtentRight;

        if (SpriteCollisionCheckObjectsTouching(
                currentTop, currentBottom, currentLeft, currentRight,
                spriteTop, spriteBottom, spriteLeft, spriteRight)) {
            gCurrentSprite.work3 = i;
            sprite->disableWarioCollisionTimer = 15;
            sprite->status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            if (sprite->pose == 86 || sprite->pose == 88) {
                func_801E4B0();
                gCurrentCarriedSprite.state = 0;
            }
            return;
        }
    }
}


void PinballLockInitCommon(void)
{
    gCurrentSprite.drawPriority = 1;
    gCurrentSprite.status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 24;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 15;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.xPosition += 32;
    gCurrentSprite.yPosition -= 64;
    gCurrentSprite.work0 = 100;
    gCurrentSprite.work3 = 255;
    func_80269EC();
}

void PinballLockUpReset(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 124;
        gCurrentSprite.pose = 16;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockUpResetOam);
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PINBALL_LOCK_RESET);
        }
    }
}

void PinballLockUpWaitForSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    PinballFindTouchingSprite();
    slot = gCurrentSprite.work3;
    if (slot != 255) {
        gCurrentSprite.work0 = 48;
        gCurrentSprite.pose = 20;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockUpWaitingOam);
        {
            struct PrimarySpriteData *sprites;
            u32 offset;

            sprites = gSpriteData;
            offset = slot * sizeof(struct PrimarySpriteData);
            sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
        }
        sprite->xPosition = gCurrentSprite.xPosition;
        sprite->pose = 87;
    } else {

        if (--gCurrentSprite.work0 != 0) {
            if (gCurrentSprite.work0 == 120) {
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                GP_SET(gCurrentSprite.pOamData, sPinballLockUpWaitingOam);
            }
        } else {
            gCurrentSprite.work0 = 44;
            gCurrentSprite.pose = 18;
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockUpMovingOam);
            if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
        }

    }
}



void PinballLockUpOpen(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 40) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockUpOpenOam);
        }
    } else {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.pose = 15;
    }
}

void PinballLockUpPullSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition - gCurrentSprite.work0;
    sprite->xPosition = gCurrentSprite.xPosition;
    gCurrentSprite.work0 -= 2;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 4;
        gCurrentSprite.pose = 22;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockUpMovingOam);
        m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
    }
}

void PinballLockUpFinishCapture(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 32;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockUpOpenOam);
        if (sprite->globalID == 87) {
            gCurrentSprite.work0 = 40;
            gCurrentSprite.pose = 50;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
            sprite->status = 0;
            gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
            func_8026A54();
            m4aSongNumStart(SE_PINBALL_LOCK_COMPLETE);
            VoiceSetPlay(VS_WARIO_REACTION_END);
        } else {
            gCurrentSprite.work0 = 20;
            gCurrentSprite.pose = 109;
        }
    }
}


void PinballLockUpDisappear(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 20) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockUpDisappearOam);
        }
    } else {
        gCurrentSprite.status = 0;
        SpriteSpawnAsChild(
            PSPRITE_B5,
            gCurrentSprite.roomSlot,
            gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition,
            gCurrentSprite.xPosition
        );
        SpawnScoreCoinDropSprites(gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    }
}

void PinballLockUpReleaseSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 32;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.work3 |= 0xFF;
        gCurrentSprite.pose = 15;
        sprite->pose = 107;
    }
}

void PinballLockDownReset(void)
{
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 124;
        gCurrentSprite.pose = 16;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockDownResetOam);
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PINBALL_LOCK_RESET);
        }
    }
}

void PinballLockDownWaitForSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;
    u8 *spriteBytes;

    PinballFindTouchingSprite();
    slot = gCurrentSprite.work3;
    if (slot != 255) {
        gCurrentSprite.work0 = 48;
        gCurrentSprite.pose = 20;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockDownWaitingOam);
        {
            struct PrimarySpriteData *sprites;
            u32 offset;

            sprites = gSpriteData;
            offset = gCurrentSprite.work3 * sizeof(struct PrimarySpriteData);
            spriteBytes = (u8 *)sprites + offset;
            sprite = (struct PrimarySpriteData *)spriteBytes;
        }
        sprite->xPosition = gCurrentSprite.xPosition;
        {
            struct PrimarySpriteData *sprites;
            u32 offset;

            sprites = gSpriteData;
            offset = slot * sizeof(struct PrimarySpriteData);
            sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
        }
        sprite->pose = 87;
    } else if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 120) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockDownWaitingOam);
        }
    } else {
        gCurrentSprite.work0 = 44;
        gCurrentSprite.pose = 18;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockDownMovingOam);
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
        }
    }
}


void PinballLockDownOpen(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 40) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockDownOpenOam);
        }
    } else {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.pose = 15;
    }
}

void PinballLockDownPullSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    {
        u8 *workPtr;
        u16 positionOffset;
        u16 y;
        s32 newTimer;

        workPtr = &gCurrentSprite.work0;
        positionOffset = *workPtr;
        positionOffset += 64;
        y = gCurrentSprite.yPosition;
        sprite->yPosition = positionOffset + y;
        sprite->xPosition = gCurrentSprite.xPosition;
        newTimer = *workPtr - 2;
        *workPtr = newTimer;
        if ((u8)newTimer == 0) {
            gCurrentSprite.work0 = 4;
            gCurrentSprite.pose = 22;
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockDownMovingOam);
            m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
        }
    }
}

void PinballLockDownFinishCapture(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 64;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockDownOpenOam);
        if (sprite->globalID == 87) {
            gCurrentSprite.work0 = 40;
            gCurrentSprite.pose = 50;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
            sprite->status = 0;
            gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
            func_8026A54();
            m4aSongNumStart(SE_PINBALL_LOCK_COMPLETE);
            VoiceSetPlay(VS_WARIO_REACTION_END);
        } else {
            gCurrentSprite.work0 = 20;
            gCurrentSprite.pose = 109;
        }
    }
}

void PinballLockDownDisappear(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 20) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockDownDisappearOam);
        }
    } else {
        gCurrentSprite.status = 0;
        SpriteSpawnAsChild(
            PSPRITE_B5,
            gCurrentSprite.roomSlot,
            gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition,
            gCurrentSprite.xPosition
        );
        SpawnScoreCoinDropSprites(gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    }
}

void PinballLockDownReleaseSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 64;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.work3 |= 0xFF;
        gCurrentSprite.pose = 15;
        sprite->pose = 107;
    }
}

void PinballLockLeftReset(void)
{
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 124;
        gCurrentSprite.pose = 16;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockLeftResetOam);
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PINBALL_LOCK_RESET);
        }
    }
}

void PinballLockLeftWaitForSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    PinballFindTouchingSprite();
    slot = gCurrentSprite.work3;
    if (slot != 255) {
        gCurrentSprite.work0 = 48;
        gCurrentSprite.pose = 20;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockLeftWaitingOam);
        {
            struct PrimarySpriteData *sprites;
            u32 offset;

            sprites = gSpriteData;
            offset = slot * sizeof(struct PrimarySpriteData);
            sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
        }
        sprite->yPosition = gCurrentSprite.yPosition + 40;
        sprite->pose = 87;
    } else if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 120) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockLeftWaitingOam);
        }
    } else {
        gCurrentSprite.work0 = 44;
        gCurrentSprite.pose = 18;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockLeftMovingOam);
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
        }
    }
}


void PinballLockLeftOpen(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 40) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockLeftOpenOam);
        }
    } else {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.pose = 15;
    }
}

void PinballLockLeftPullSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 40;
    {
        u8 *workPtr;
        u16 positionOffset;
        u16 x;
        s32 newTimer;

        workPtr = &gCurrentSprite.work0;
        positionOffset = *workPtr;
        positionOffset += 16;
        x = gCurrentSprite.xPosition;
        sprite->xPosition = x - positionOffset;
        newTimer = *workPtr - 2;
        *workPtr = newTimer;
        if ((u8)newTimer == 0) {
            gCurrentSprite.work0 = 4;
            gCurrentSprite.pose = 22;
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockLeftMovingOam);
            m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
        }
    }
}

void PinballLockLeftFinishCapture(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 40;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockLeftOpenOam);
        if (sprite->globalID == 87) {
            gCurrentSprite.work0 = 40;
            gCurrentSprite.pose = 50;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
            sprite->status = 0;
            gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
            func_8026A54();
            m4aSongNumStart(SE_PINBALL_LOCK_COMPLETE);
            VoiceSetPlay(VS_WARIO_REACTION_END);
        } else {
            gCurrentSprite.work0 = 20;
            gCurrentSprite.pose = 109;
        }
    }
}

void PinballLockLeftDisappear(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 20) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockLeftDisappearOam);
        }
    } else {
        gCurrentSprite.status = 0;
        SpriteSpawnAsChild(
            PSPRITE_B5,
            gCurrentSprite.roomSlot,
            gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition,
            gCurrentSprite.xPosition
        );
        SpawnScoreCoinDropSprites(gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    }
}

void PinballLockLeftReleaseSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 40;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.work3 |= 0xFF;
        gCurrentSprite.pose = 15;
        sprite->pose = 107;
    }
}

void PinballLockRightReset(void)
{
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 124;
        gCurrentSprite.pose = 16;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockRightResetOam);
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PINBALL_LOCK_RESET);
        }
    }
}

void PinballLockRightWaitForSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    PinballFindTouchingSprite();
    slot = gCurrentSprite.work3;
    if (slot != 255) {
        gCurrentSprite.work0 = 48;
        gCurrentSprite.pose = 20;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockRightWaitingOam);
        {
            struct PrimarySpriteData *sprites;
            u32 offset;

            sprites = gSpriteData;
            offset = slot * sizeof(struct PrimarySpriteData);
            sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
        }
        sprite->yPosition = gCurrentSprite.yPosition + 40;
        sprite->pose = 87;
    } else if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 120) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockRightWaitingOam);
        }
    } else {
        gCurrentSprite.work0 = 44;
        gCurrentSprite.pose = 18;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockRightMovingOam);
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
        }
    }
}


void PinballLockRightOpen(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 40) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockRightOpenOam);
        }
    } else {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.pose = 15;
    }
}

void PinballLockRightPullSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 40;
    {
        u8 *workPtr;
        u16 positionOffset;
        u16 x;
        s32 newTimer;

        workPtr = &gCurrentSprite.work0;
        positionOffset = *workPtr;
        positionOffset += 12;
        x = gCurrentSprite.xPosition;
        sprite->xPosition = positionOffset + x;
        newTimer = *workPtr - 2;
        *workPtr = newTimer;
        if ((u8)newTimer == 0) {
            gCurrentSprite.work0 = 4;
            gCurrentSprite.pose = 22;
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockRightMovingOam);
            m4aSongNumStart(SE_PINBALL_LOCK_MOVE);
        }
    }
}

void PinballLockRightFinishCapture(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 40;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        GP_SET(gCurrentSprite.pOamData, sPinballLockRightOpenOam);
        if (sprite->globalID == 87) {
            gCurrentSprite.work0 = 40;
            gCurrentSprite.pose = 50;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
            sprite->status = 0;
            gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
            func_8026A54();
            m4aSongNumStart(SE_PINBALL_LOCK_COMPLETE);
            VoiceSetPlay(VS_WARIO_REACTION_END);
        } else {
            gCurrentSprite.work0 = 20;
            gCurrentSprite.pose = 109;
        }
    }
}

void PinballLockRightDisappear(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 20) {
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockRightDisappearOam);
        }
    } else {
        gCurrentSprite.status = 0;
        SpriteSpawnAsChild(
            PSPRITE_B5,
            gCurrentSprite.roomSlot,
            gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition,
            gCurrentSprite.xPosition
        );
        SpawnScoreCoinDropSprites(gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    }
}

void PinballLockRightReleaseSprite(void)
{
    u8 slot;
    struct PrimarySpriteData *sprite;

    slot = gCurrentSprite.work3;
    {
        struct PrimarySpriteData *sprites;
        u32 offset;

        sprites = gSpriteData;
        offset = slot * sizeof(struct PrimarySpriteData);
        sprite = (struct PrimarySpriteData *)((u8 *)sprites + offset);
    }
    sprite->pose = 87;
    sprite->yPosition = gCurrentSprite.yPosition + 40;
    sprite->xPosition = gCurrentSprite.xPosition;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 1;
        gCurrentSprite.work3 |= 0xFF;
        gCurrentSprite.pose = 15;
        sprite->pose = 107;
    }
}

void PinballCounterInit(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 4;
    gCurrentSprite.hitboxExtentDown = 4;
    gCurrentSprite.hitboxExtentLeft = 4;
    gCurrentSprite.hitboxExtentRight = 4;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.work0 = 120;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.xPosition += 32;
    gCurrentSprite.yPosition -= 64;
    func_80269EC();
}

void PinballCounterUpdateDisplay(void)
{
    s32 count = 0;
    s32 i = 0;
    for (; i < 64 && gRoomSpriteData[i][0] != 255; i++) {
        switch (gRoomSpriteData[i][2]) {
            case 17:
            case 18:
            case 19:
            case 20:
                if (gPersistentSpriteData[gCurrentRoom][i] == 2)
                    count++;
                break;
        }
    }
    switch (count) {
        case 0: GP_SET(gCurrentSprite.pOamData, sPinballCounterZeroOam); break;
        case 1: GP_SET(gCurrentSprite.pOamData, sPinballCounterOneOam); break;
        case 2: GP_SET(gCurrentSprite.pOamData, sPinballCounterTwoOam); break;
        case 3: GP_SET(gCurrentSprite.pOamData, sPinballCounterThreeOam); break;
        case 4: GP_SET(gCurrentSprite.pOamData, sPinballCounterFourOam); gCurrentSprite.pose = 49; break;
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
}

void PinballCounterReady(void)
{
    if ((gCurrentSprite.status & (SPRITE_STATUS_EXISTS | SPRITE_STATUS_ONSCREEN)) ==
        (SPRITE_STATUS_EXISTS | SPRITE_STATUS_ONSCREEN)) {
        gCurrentSprite.pose = 50;
        m4aSongNumStart(SE_PINBALL_COUNTER_READY);
    }
}

void PinballCounterComplete(void)
{
    if (--gCurrentSprite.work0 != 0) {
        if (gCurrentSprite.work0 == 60) {
            GP_SET(gCurrentSprite.pOamData, sPinballCounterDisappearOam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
        }
    } else {
        gCurrentSprite.status = 0;
        gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
        func_8026A54();
        SpriteSpawnAsChild(
            PSPRITE_B5,
            gCurrentSprite.roomSlot,
            gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition,
            gCurrentSprite.xPosition
        );
        SpawnScoreCoinDropSprites(gCurrentSprite.yPosition, gCurrentSprite.xPosition, 50);
        VoiceSetPlay(VS_WARIO_TREASURE);
    }
}

void PinballCompletionSparkleInit(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 32;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.hitboxExtentUp = 4;
    gCurrentSprite.hitboxExtentDown = 4;
    gCurrentSprite.hitboxExtentLeft = 4;
    gCurrentSprite.hitboxExtentRight = 4;
    GP_SET(gCurrentSprite.pOamData, sPinballCompletionSparkleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.work0 = 32;
    gCurrentSprite.pose = 16;
    gCurrentSprite.drawPriority = 1;
    m4aSongNumStart(SE_PINBALL_COUNTER_APPEAR);
}

void PinballCompletionSparkle(void)
{
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = 0;
    }
}


void SpritePinball(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = 107;
    }

    switch (gCurrentSprite.pose) {
        case 0:
            PinballInit();
            break;

        case 15:
        case 23:
        case 47:
            PinballIdleInit();
        case 16:
        case 48:
            PinballIdle();
            break;

        case 27:
        case 29:
            PinballLandingInit();
        case 30:
            func_8023EE0();
            break;

        case 43:
            PinballImpactRightInit();
        case 44:
            func_8024478();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 45:
            PinballImpactLeftInit();
        case 46:
            func_802449C();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 49:
            PinballKnockAwayFromWario();
            break;

        case 31:
        case 79:
            PinballTackledRightInit();
            func_8024988();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 33:
        case 80:
            PinballTackledLeftInit();
            func_802476C();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 35:
        case 51:
        case 106:
            PinballKnockedRightInit();
        case 52:
            func_8024988();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 37:
        case 53:
        case 105:
            PinballKnockedLeftInit();
        case 54:
            func_802476C();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 55:
            func_8024688();
        case 56:
            func_80246B8();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 57:
            func_802473C();
        case 58:
            func_802476C();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 59:
            func_80247F0();
        case 60:
            func_8024820();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 61:
            func_80248A4();
        case 62:
            func_80248D4();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 63:
            func_8024958();
        case 64:
            func_8024988();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 65:
            func_8024A0C();
        case 66:
            func_8024A3C();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 67:
            func_8024AC0();
            func_8024AD4();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 69:
            func_8024BEC();
            func_8024C00();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 71:
            PinballThrownUpRightInit();
        case 68:
        case 72:
            func_8024AD4();
            gCurrentSprite.currentAnimationFrame -= 4;
            break;

        case 73:
            PinballThrownUpLeftInit();
        case 70:
        case 74:
            func_8024C00();
            gCurrentSprite.currentAnimationFrame += 4;
            break;

        case 81:
            PinballLiftedRightInit();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            PinballLiftedLeftInit();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            PinballCarriedRightInit();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            PinballCarriedLeftInit();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;

        case 89:
            gCurrentSprite.pose = 90;
        case 90:
            SpriteUtilThrownLeftSoft();
            gCurrentSprite.currentAnimationFrame += 8;
            break;

        case 93:
            gCurrentSprite.pose = 94;
            gCurrentSprite.health = 1;
        case 94:
            SpriteUtilThrownLeftHard();
            gCurrentSprite.currentAnimationFrame += 16;
            break;

        case 97:
            gCurrentSprite.pose = 98;
        case 98:
            SpriteUtilThrownUpLeftSoft();
            gCurrentSprite.currentAnimationFrame += 8;
            break;

        case 101:
            gCurrentSprite.pose = 102;
            gCurrentSprite.health = 18;
        case 102:
            SpriteUtilThrownUpLeftHard();
            gCurrentSprite.currentAnimationFrame += 16;
            break;

        case 91:
            gCurrentSprite.pose = 92;
        case 92:
            SpriteUtilThrownRightSoft();
            gCurrentSprite.currentAnimationFrame -= 8;
            break;

        case 95:
            gCurrentSprite.pose = 96;
            gCurrentSprite.health = 2;
        case 96:
            SpriteUtilThrownRightHard();
            gCurrentSprite.currentAnimationFrame -= 16;
            break;

        case 99:
            gCurrentSprite.pose = 100;
        case 100:
            SpriteUtilThrownUpRightSoft();
            gCurrentSprite.currentAnimationFrame -= 8;
            break;

        case 103:
            gCurrentSprite.pose = 104;
            gCurrentSprite.health = 17;
        case 104:
            SpriteUtilThrownUpRightHard();
            gCurrentSprite.currentAnimationFrame -= 16;
            break;

        case 107:
            if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
                goto die;
            }
            if (gCurrentSprite.health & 0xF0) {
                gCurrentSprite.work2 = 4;
            } else {
                gCurrentSprite.work2 = 8;
            }
            if ((gCurrentSprite.health & 0xF) == 2) {
                func_8024AC0();
            } else {
                func_8024BEC();
            }
            break;

        default:
die:
            SpriteUtilDie();
            break;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_AFFINE) {
        gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.currentAnimationFrame), FixedInverse(0x100));
        gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.currentAnimationFrame), FixedInverse(0x100));
        gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.currentAnimationFrame), FixedInverse(0x100));
        gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.currentAnimationFrame), FixedInverse(0x100));
        gCurrentSprite.currentAnimationFrame--;
    }
}

void SpritePinballLockUp(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            PinballLockInitCommon();
            gCurrentSprite.hitboxExtentUp = 72;
            gCurrentSprite.hitboxExtentDown = 0;
            gCurrentSprite.hitboxExtentLeft = 4;
            gCurrentSprite.hitboxExtentRight = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockUpOpenOam);
            break;
        case 15:
            PinballLockUpReset();
            break;
        case 16:
            PinballLockUpWaitForSprite();
            break;
        case 18:
            PinballLockUpOpen();
            break;
        case 20:
            PinballLockUpPullSprite();
            break;
        case 22:
            PinballLockUpFinishCapture();
            break;
        case 50:
            PinballLockUpDisappear();
            break;
        case 109:
            PinballLockUpReleaseSprite();
            break;
    }
}

void SpritePinballLockDown(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            PinballLockInitCommon();
            gCurrentSprite.hitboxExtentUp = 0;
            gCurrentSprite.hitboxExtentDown = 72;
            gCurrentSprite.hitboxExtentLeft = 4;
            gCurrentSprite.hitboxExtentRight = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockDownOpenOam);
            break;
        case 15:
            PinballLockDownReset();
            break;
        case 16:
            PinballLockDownWaitForSprite();
            break;
        case 18:
            PinballLockDownOpen();
            break;
        case 20:
            PinballLockDownPullSprite();
            break;
        case 22:
            PinballLockDownFinishCapture();
            break;
        case 50:
            PinballLockDownDisappear();
            break;
        case 109:
            PinballLockDownReleaseSprite();
            break;
    }
}

void SpritePinballLockLeft(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            PinballLockInitCommon();
            gCurrentSprite.hitboxExtentUp = 4;
            gCurrentSprite.hitboxExtentDown = 8;
            gCurrentSprite.hitboxExtentLeft = 72;
            gCurrentSprite.hitboxExtentRight = 0;
            GP_SET(gCurrentSprite.pOamData, sPinballLockLeftOpenOam);
            break;
        case 15:
            PinballLockLeftReset();
            break;
        case 16:
            PinballLockLeftWaitForSprite();
            break;
        case 18:
            PinballLockLeftOpen();
            break;
        case 20:
            PinballLockLeftPullSprite();
            break;
        case 22:
            PinballLockLeftFinishCapture();
            break;
        case 50:
            PinballLockLeftDisappear();
            break;
        case 109:
            PinballLockLeftReleaseSprite();
            break;
    }
}

void SpritePinballLockRight(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            PinballLockInitCommon();
            gCurrentSprite.hitboxExtentUp = 4;
            gCurrentSprite.hitboxExtentDown = 8;
            gCurrentSprite.hitboxExtentLeft = 0;
            gCurrentSprite.hitboxExtentRight = 68;
            GP_SET(gCurrentSprite.pOamData, sPinballLockRightOpenOam);
            break;
        case 15:
            PinballLockRightReset();
            break;
        case 16:
            PinballLockRightWaitForSprite();
            break;
        case 18:
            PinballLockRightOpen();
            break;
        case 20:
            PinballLockRightPullSprite();
            break;
        case 22:
            PinballLockRightFinishCapture();
            break;
        case 50:
            PinballLockRightDisappear();
            break;
        case 109:
            PinballLockRightReleaseSprite();
            break;
    }
}

void SpritePinballCounter(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            PinballCounterInit();
        case 16:
            PinballCounterUpdateDisplay();
            break;
        case 49:
            PinballCounterReady();
            break;
        case 50:
            PinballCounterComplete();
            break;
    }
}

void SpritePinballCompletionSparkle(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0) {
        PinballCompletionSparkleInit();
    } else {
        PinballCompletionSparkle();
    }
}

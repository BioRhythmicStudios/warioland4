#include "sprite_ai/chandelier.h"

#include "global_data.h"
#include "sound.h"
#include "sprite_util.h"

void InitChandelier(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.warioCollision = 14;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 96;
    gCurrentSprite.hitboxExtentDown = 192;
    gCurrentSprite.hitboxExtentLeft = 48;
    gCurrentSprite.hitboxExtentRight = 44;
    GP_SET(gCurrentSprite.pOamData, sChandelierIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 1;
    gCurrentSprite.xPosition += 32;
}

void ChandelierStartShaking(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        GP_SET(gCurrentSprite.pOamData, sChandelierShakingOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.work0 = 96;
        gCurrentSprite.pose = 18;
    }
}

void ChandelierShake(void)
{
    struct PrimarySpriteData *sprite;
    u32 animation;

    sprite = &gCurrentSprite;
    animation = *(u32 *)&sprite->animationTimer;
    animation &= 0xFFFFFF;
    if (animation == 0x60001) {
        if (sprite->status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_CHANDELIER_SHAKE);
        }
    } else if (animation == 0x60009) {
        if (sprite->status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_CHANDELIER_SHAKE);
        }
    }

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        GP_SET(gCurrentSprite.pOamData, sChandelierFallingOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.work3 = 0;
        gCurrentSprite.pose = 20;
        gCurrentSprite.drawPriority = 1;
        if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_CHANDELIER_FALL);
        }
    }
}

void ChandelierFall(void)
{
    struct PrimarySpriteData *sprite;
    u32 yPosition;

    sprite = &gCurrentSprite;
    yPosition = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gSpriteCollisionResult != 0) {
        sprite->yPosition = yPosition;
        sprite->pose = 49;
        if (sprite->status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_CHANDELIER_LAND);
        }
    } else {
        u8 *indexPointer;
        u8 index;
        const s16 *velocities;
        const s16 *entry;
        s32 velocityRaw;
        s32 velocity;
        u16 zero;
        indexPointer = &sprite->work3;
        index = *indexPointer;
        velocities = sChandelierFallVelocity;
        entry = (const s16 *)((index << 1) + (u32)velocities);
        velocityRaw = *(const u16 *)entry;
        zero = 0;
        velocity = *(s16 *)((u8 *)entry + zero);
        if (velocity == 0x7FFF) {
            u32 previousOffset;
            u16 currentY;
            s32 previousVelocity;
            previousOffset = index - 1;
            previousOffset <<= 1;
            previousOffset += (u32)velocities;
            currentY = *(volatile u16*)&sprite->yPosition;
            previousVelocity = *(const u16 *)previousOffset;
            currentY += previousVelocity;
            sprite->yPosition = currentY;
        } else {
            u8 nextIndex;
            u8 *pointer;
            nextIndex = index + 1;
            pointer = indexPointer;
            *pointer = nextIndex;
            sprite->yPosition += velocityRaw;
        }
    }
}

void InitFallenChandelier(void)
{
    gCurrentSprite.drawDistanceLeftRight = 56;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 48;
    gCurrentSprite.hitboxExtentRight = 44;
    GP_SET(gCurrentSprite.pOamData, sChandelierFallenOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 255;
    gCurrentSprite.pose = 50;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
}

void FallenChandelierWait(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.work0 = 25;
        gCurrentSprite.pose = 109;
    }
}

void FallenChandelierDisappear(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = 0;
    }
}

void SpriteChandelier(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitChandelier();
            break;
        case 16:
            ChandelierStartShaking();
            break;
        case 18:
            ChandelierShake();
            break;
        case 20:
            ChandelierFall();
            break;
        case 50:
            FallenChandelierWait();
            break;
        case 109:
            FallenChandelierDisappear();
            break;
        default:
            InitFallenChandelier();
            break;
    }
}

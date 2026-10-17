#include "sprite_ai/magma.h"
#include "background_registers.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"

void InitMagma(void)
{
    if (gSwitchPressed) {
        gCurrentSprite.status = 0;
        return;
    }

    gCurrentSprite.status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.warioCollision = 56;
    gCurrentSprite.drawDistanceDown = 64;
    gCurrentSprite.drawDistanceUp = 64;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.hitboxExtentUp = 200;
    gCurrentSprite.hitboxExtentLeft = 104;
    gCurrentSprite.hitboxExtentRight = 100;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE_INIT;
    gCurrentSprite.xPosition += 32;
    gCurrentSprite.yPosition += 448;
    gCurrentSprite.work0 = 24;
}

void MagmaWaitBeforeRising(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_IDLE;
        gCurrentSprite.work3 = 0;
        m4aSongNumStart(SE_MAGMA_RISE_SOUND);
    }
}

void MagmaRise(void)
{
    u8 index;
    s16 velocity;

    index = gCurrentSprite.work3;
    velocity = sMagmaYVelocity[index];
    if (velocity == 0x7FFF) {
        gCurrentSprite.work0 = 247;
        gCurrentSprite.pose = SPOSE_12;
        m4aSongNumStart(SE_MAGMA_SINK_SOUND);
    } else {
        gCurrentSprite.work3 = index + 1;
        gCurrentSprite.yPosition += velocity;
    }
}

void MagmaSink(void)
{
    if (gCurrentSprite.work0 != 0) {
        gCurrentSprite.work0--;
        gCurrentSprite.yPosition += 2;
    } else {
        gCurrentSprite.pose = SPOSE_IDLE_INIT;
        gCurrentSprite.work0 = 24;
    }
}

void SpriteMagma(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitMagma();
            break;
        case SPOSE_IDLE_INIT:
            MagmaWaitBeforeRising();
            break;
        case SPOSE_IDLE:
            MagmaRise();
            break;
        case SPOSE_12:
            MagmaSink();
            break;
    }

    if ((u16)((gCurrentSprite.yPosition >> 2) - (gBg1YPosition >> 2)) <= 160) {
        GP_SET(gCurrentSprite.pOamData, sMagmaOam_83D2570);
        gCurrentSprite.hitboxExtentDown = 127;
    } else {
        GP_SET(gCurrentSprite.pOamData, sMagmaOam_83D2598);
        gCurrentSprite.hitboxExtentDown = 0;
    }
}

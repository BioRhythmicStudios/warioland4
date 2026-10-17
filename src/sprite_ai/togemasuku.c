#include "sprite_ai/togemasuku.h"

#include "score.h"
#include "sprite.h"
#include "sprite_util.h"

void InitTogemasuku(void)
{
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 32;
    gCurrentSprite.hitboxExtentDown = 32;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 32;
    GP_SET(gCurrentSprite.pOamData, sTogemasukuIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = 11;
    gCurrentSprite.yPosition -= 32;
}

void StartTogemasukuRespawn(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_MAYBE_DEAD) {
        gCurrentSprite.status &= ~SPRITE_STATUS_MAYBE_DEAD;
    }
    GP_SET(gCurrentSprite.pOamData, sTogemasukuDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_6D;
    gCurrentSprite.work0 = 60;
}

void TogemasukuRespawnDelay(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        GP_SET(gCurrentSprite.pOamData, sTogemasukuRespawningOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_6F;
        gCurrentSprite.work0 = 128;
    }
}

void RestoreTogemasukuIdle(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        GP_SET(gCurrentSprite.pOamData, sTogemasukuIdleOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_IDLE;
    }
}

void DefeatTogemasuku(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogemasukuDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, SSPRITE_06);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void StartTogemasukuFallRight(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    DefeatTogemasuku();
}

void StartTogemasukuFallLeft(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    DefeatTogemasuku();
}

void KillTogemasuku(void)
{
    SpriteUtilDie();
}

void SpriteTogemasuku(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            goto init;
        case SPOSE_IDLE:
            goto end;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            goto fallRightInitShared;
        case SPOSE_TACKLED_RIGHT:
            goto fallRight;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            goto fallLeftInitShared;
        case SPOSE_TACKLED_LEFT:
            goto fallLeft;
        case SPOSE_69:
            goto fallLeftInit;
        case SPOSE_6A:
            goto fallRightInit;
        case SPOSE_6B:
            goto die;
        case SPOSE_6C:
            goto beginRespawn;
        case SPOSE_6D:
            goto respawnDelay;
        case SPOSE_6F:
            goto restoreIdle;
        default:
            goto invalidPose;
    }

init:
    InitTogemasuku();
    goto end;

beginRespawn:
    StartTogemasukuRespawn();
respawnDelay:
    TogemasukuRespawnDelay();
    goto end;

restoreIdle:
    RestoreTogemasukuIdle();
    goto end;

fallRightInit:
    StartTogemasukuFallRight();
fallRight:
    SpriteUtilFallOffscreenRight();
    goto end;

fallRightInitShared:
    StartTogemasukuFallRight();
    goto fallRight;

fallLeftInit:
    StartTogemasukuFallLeft();
fallLeft:
    SpriteUtilFallOffscreenLeft();
    goto end;

fallLeftInitShared:
    StartTogemasukuFallLeft();
    goto fallLeft;

die:
    KillTogemasuku();
    goto end;

invalidPose:
    gCurrentSprite.pose = SPOSE_6C;

end:
    return;
}

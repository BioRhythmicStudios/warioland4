#include "global_data.h"
#include "sound.h"
#include "score.h"
#include "sprite.h"
#include "sprite_ai/togetoge.h"
#include "sprite_util.h"
#include "gba/m4a.h"

void InitTogetoge(void)
{
    struct PrimarySpriteData* sprite;
    sprite = &gCurrentSprite;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 108;
    sprite->hitboxExtentDown = 148;
    sprite->hitboxExtentLeft = 32;
    sprite->hitboxExtentRight = 28;
    GP_SET(sprite->pOamData, sTogetogeIdleOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->warioCollision = 12;
    sprite->work0 = 1;
    sprite->pose = 16;
    SpriteUtilTurnTowardWario();
}

void TogetogeWaitBeforeFalling(void)
{
    u8 timer;
    timer = --gCurrentSprite.work0;
    if (timer == 0)
    {
        gCurrentSprite.work3 = 0;
        gCurrentSprite.pose = 18;
        m4aSongNumStart(SE_TOGETOGE_START_FALLING);
    }
}

void TogetogeSetFallTimers(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void TogetogeSetRightFallPose(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    TogetogeSetFallTimers();
}

void TogetogeSetLeftFallPose(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    TogetogeSetFallTimers();
}

void TogetogeFall(void)
{
    struct PrimarySpriteData* sprite;
    u32 y;
    int previousIndex;
    u16 previousMovement;
    s16 movement;
    u8 index;
    sprite = &gCurrentSprite;
    y = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1)
        sprite->status |= SPRITE_STATUS_UNDERWATER;
    if (gSpriteCollisionResult != 0)
    {
        sprite->yPosition = y;
        GP_SET(sprite->pOamData, sTogetogeFallingOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        SpriteUtilTurnTowardWario();
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
            TogetogeSetRightFallPose();
        else
            TogetogeSetLeftFallPose();
        return;
    }
    index = sprite->work3;
    movement = sSpriteGravityVelocityTable[index];
    if (movement == 0x7FFF)
    {
        previousIndex = index - 1;
        previousMovement = ((const u16*)sSpriteGravityVelocityTable)[previousIndex];
        sprite->yPosition += previousMovement;
    }
    else
    {
        sprite->work3 = index + 1;
        sprite->yPosition += movement;
    }
}

void TogetogeStartFallingOffscreen(void)
{
    gCurrentSprite.work2 = 4;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & 0xFEFF) | 0x200;
}

void TogetogeStartFallingOffscreenRight(void)
{
    gCurrentSprite.pose = 32;
    TogetogeStartFallingOffscreen();
}

void TogetogeStartFallingOffscreenLeft(void)
{
    gCurrentSprite.pose = 34;
    TogetogeStartFallingOffscreen();
}

void SpriteTogetoge(void)
{
    gCurrentSprite.status &= 0xFFDF;
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER)
        gCurrentSprite.pose = 107;
    switch (gCurrentSprite.pose)
    {
        case 0: InitTogetoge(); break;
        case 16: TogetogeWaitBeforeFalling(); break;
        case 18: TogetogeFall(); break;
        case 43:
        case 44:
            gCurrentSprite.work2 = 4;
            func_8025E98();
            break;
        case 45:
        case 46:
            gCurrentSprite.work2 = 4;
            func_8025DE8();
            break;
        case 35:
        case 51:
            TogetogeSetRightFallPose();
            goto pose_52;
        case 37:
        case 53:
            TogetogeSetLeftFallPose();
            goto pose_54;
        case 55:
            func_8024688();
            m4aSongNumStart(SE_BOUNCE);
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
            m4aSongNumStart(SE_BOUNCE);
pose_54:
        case 54:
        case 58:
            func_802476C();
            break;
        case 59:
            func_80247F0();
            m4aSongNumStart(SE_BOUNCE);
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
            m4aSongNumStart(SE_BOUNCE);
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
            m4aSongNumStart(SE_BOUNCE);
pose_52:
        case 52:
        case 64:
            func_8024988();
            break;
        case 65:
            func_8024A0C();
            m4aSongNumStart(SE_BOUNCE);
        case 66:
            func_8024A3C();
            break;
        case 67:
        case 71:
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = 49;
            else
                func_8024AC0();
pose_68:
        case 68:
            func_8024AD4();
            break;
        case 69:
        case 73:
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                func_8024BEC();
            else
                gCurrentSprite.pose = 49;
pose_70:
        case 70:
            func_8024C00();
            break;
        case 31:
        case 106:
            TogetogeStartFallingOffscreenRight();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 105:
            TogetogeStartFallingOffscreenLeft();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        default:
            gCurrentSprite.status = 0;
            SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 5);
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot]--;
            break;
    }
    gCurrentSprite.status |= 32;
}

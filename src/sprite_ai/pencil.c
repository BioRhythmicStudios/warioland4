#include "sprite_ai/pencil.h"

#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"

enum PencilPose {
    PENCIL_POSE_IDLE = SPOSE_IDLE,
    PENCIL_POSE_MOVE_RIGHT_INIT = SPOSE_TACKLED_RIGHT_INIT,
    PENCIL_POSE_MOVE_RIGHT = SPOSE_TACKLED_RIGHT,
    PENCIL_POSE_MOVE_LEFT_INIT = SPOSE_TACKLED_LEFT_INIT,
    PENCIL_POSE_MOVE_LEFT = SPOSE_TACKLED_LEFT,
};

enum PencilCollision {
    PENCIL_COLLISION_MOVING = 25,
    PENCIL_COLLISION_IDLE = 26,
};

void InitPencil(void)
{
    const struct AnimationFrame *oamData;

    gCurrentSprite.status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE | SPRITE_STATUS_IGNORE_SPRITE_COLLISION |
                             SPRITE_STATUS_BACKGROUND;
    gCurrentSprite.warioCollision = PENCIL_COLLISION_IDLE;
    gCurrentSprite.drawDistanceDown = 20;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 40;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 96;
    gCurrentSprite.hitboxExtentRight = 160;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work2 = 0;
    gCurrentSprite.pose = PENCIL_POSE_IDLE;

    if (gCurrentSprite.globalID == PSPRITE_PENCIL_BLUE) {
        goto bluePencil;
    }
    if (gCurrentSprite.globalID != PSPRITE_PENCIl_RED) {
        goto purplePencil;
    }
    oamData = sPencilRedOam;
    goto setOam;

bluePencil:
    oamData = sPencilBlueOam;
    goto setOam;

purplePencil:
    oamData = sPencilPurpleOam;

setOam:
    GP_SET(gCurrentSprite.pOamData, oamData);
    gCurrentSprite.work1 = 0;
}

void PencilStartMovingRight(void)
{
    struct PrimarySpriteData *initial;
    struct PrimarySpriteData *sprite;
    u8 *direction;
    u8 currentDirection;

    initial = &gCurrentSprite;
    direction = &initial->work2;
    currentDirection = *direction;
    sprite = initial;
    {
        u8 nextDirection;

        if (currentDirection != 0) {
            if (sprite->globalID != PSPRITE_PENCIL_BLUE) {
                goto cancel;
            }
            nextDirection = 0;
            goto startMoving;
        }
        if (sprite->globalID != PSPRITE_PENCIL_BLUE) {
            goto setDirection;
        }

cancel:
        sprite->pose = PENCIL_POSE_IDLE;
        return;

setDirection:
        nextDirection = 1;

startMoving:
        *direction = nextDirection;
    }

    sprite->pose = PENCIL_POSE_MOVE_RIGHT;
    sprite->work0 = 16;
    sprite->warioCollision = PENCIL_COLLISION_MOVING;
    m4aSongNumStart(SOUND_DB);
}

void PencilMoveRight(void)
{
    struct PrimarySpriteData *sprite;
    sprite = &gCurrentSprite;
    sprite->xPosition += 8;
    if (--sprite->work0 == 0) {
        sprite->pose = PENCIL_POSE_IDLE;
        sprite->warioCollision = PENCIL_COLLISION_IDLE;
    }
}

void PencilStartMovingLeft(void)
{
    struct PrimarySpriteData *initial;
    struct PrimarySpriteData *sprite;
    u8 *direction;
    u8 currentDirection;

    initial = &gCurrentSprite;
    direction = &initial->work2;
    currentDirection = *direction;
    sprite = initial;
    {
        u8 nextDirection;

        if (currentDirection != 0) {
            if (sprite->globalID == PSPRITE_PENCIL_BLUE) {
                goto cancel;
            }
            nextDirection = 0;
            goto startMoving;
        }
        if (sprite->globalID == PSPRITE_PENCIL_BLUE) {
            goto setDirection;
        }

cancel:
        sprite->pose = PENCIL_POSE_IDLE;
        return;

setDirection:
        nextDirection = 1;

startMoving:
        *direction = nextDirection;
    }

    sprite->pose = PENCIL_POSE_MOVE_LEFT;
    sprite->work0 = 16;
    sprite->warioCollision = PENCIL_COLLISION_MOVING;
    m4aSongNumStart(SOUND_DB);
}

void PencilMoveLeft(void)
{
    struct PrimarySpriteData *sprite;
    sprite = &gCurrentSprite;
    sprite->xPosition -= 8;
    if (--sprite->work0 == 0) {
        sprite->pose = PENCIL_POSE_IDLE;
        sprite->warioCollision = PENCIL_COLLISION_IDLE;
    }
}

void SpritePencil(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitPencil();
            break;
        case PENCIL_POSE_MOVE_RIGHT_INIT:
            PencilStartMovingRight();
            break;
        case PENCIL_POSE_MOVE_RIGHT:
            PencilMoveRight();
            break;
        case PENCIL_POSE_MOVE_LEFT_INIT:
            PencilStartMovingLeft();
            break;
        case PENCIL_POSE_MOVE_LEFT:
            PencilMoveLeft();
            break;
    }

    func_8026838();
}

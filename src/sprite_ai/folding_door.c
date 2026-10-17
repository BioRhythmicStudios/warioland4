#include "block.h"
#include "score.h"
#include "sprite.h"
#include "wario.h"
#include "sprite_ai/folding_door.h"

void SetFoldingDoorCollision(void)
{
    register int yPosition PIN("r5");
    register int xPosition PIN("r6");
    register int lowerY PIN("r4");
    register int leftX PIN("r8");
    s8 *blockAction;
    register s8 action PIN("r9");

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    blockAction = &gSpriteTileInteractionMode;
    action = 3;
    *blockAction = action;
    leftX = 64;
    leftX = -leftX;
    leftX = (u16)(leftX + xPosition);
    GetSpriteBlockCollisionAtPosition(yPosition, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    xPosition = (u16)(xPosition + 64);
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
    lowerY = (u16)(yPosition + 64);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, xPosition);
    lowerY = (u16)(yPosition + 128);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, xPosition);
    yPosition = (u16)(yPosition + 192);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(yPosition, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
}

void ClearFoldingDoorCollision(void)
{
    register int yPosition PIN("r5");
    register int xPosition PIN("r6");
    register int lowerY PIN("r4");
    register int leftX PIN("r8");
    s8 *blockAction;
    register s8 action PIN("r9");

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    blockAction = &gSpriteTileInteractionMode;
    action = 2;
    *blockAction = action;
    leftX = 64;
    leftX = -leftX;
    leftX = (u16)(leftX + xPosition);
    GetSpriteBlockCollisionAtPosition(yPosition, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    xPosition = (u16)(xPosition + 64);
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
    lowerY = (u16)(yPosition + 64);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, xPosition);
    lowerY = (u16)(yPosition + 128);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(lowerY, xPosition);
    yPosition = (u16)(yPosition + 192);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(yPosition, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
}

void InitFoldingDoor(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.drawDistanceDown = 0;
    gCurrentSprite.drawDistanceUp = 64;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 4;
    gCurrentSprite.hitboxExtentDown = 4;
    gCurrentSprite.hitboxExtentLeft = 4;
    gCurrentSprite.hitboxExtentRight = 4;
    GP_SET(gCurrentSprite.pOamData, sFoldingDoorClosedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.drawPriority = 1;
    SetFoldingDoorCollision();
}

void FoldingDoorWaitForSwitch(void)
{
    int flag;
    int maxDistance;
    u32 warioY;
    u32 warioX;
    s32 distance;
    u32 spriteX;
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *spriteTemp;
    int yMax;
    int zero;

    if (gSwitchPressed == 0) {
        return;
    }

    flag = 0;
    yMax = 128;
    maxDistance = 256;
    warioY = gWarioData.yPosition;
    warioX = gWarioData.xPosition;
    spriteTemp = &gCurrentSprite;
    distance = (u16)(spriteTemp->yPosition + 192);
    spriteX = spriteTemp->xPosition;
    sprite = spriteTemp;

    if ((u32)distance > warioY) {
        distance -= warioY;
    } else {
        distance = warioY - distance;
    }
    if (distance >= yMax) {
        return;
    }

    if (spriteX > warioX) {
        distance = spriteX - warioX;
        if ((s32)distance >= maxDistance) {
            goto checkFlag;
        }
        goto activate;
    } else {
        distance = warioX - spriteX;
        if ((s32)distance >= maxDistance) {
            goto checkFlag;
        }
        flag = 8;
    }

checkFlag:
    if (flag == 0) {
        return;
    }

activate:
    {
        u8 *work0;

        zero = 0;
        sprite->pose = 18;
        work0 = &sprite->work0;
        *work0 = 24;
        GP_SET(sprite->pOamData, sFoldingDoorOpeningOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
    }
    ClearFoldingDoorCollision();
}

void FoldingDoorOpening(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 24;
        GP_SET(gCurrentSprite.pOamData, sFoldingDoorOpenOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
    }
}

void SpriteFoldingDoor(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitFoldingDoor();
            break;
        case 16:
            FoldingDoorWaitForSwitch();
            break;
        case 18:
            FoldingDoorOpening();
            break;
    }
}

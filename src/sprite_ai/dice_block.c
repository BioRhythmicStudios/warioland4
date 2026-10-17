#include "sprite_ai/dice_block.h"

#include "block.h"
#include "global_data.h"
#include "sprite.h"
#include "sprite_util.h"
#include "sound.h"

void SetDiceBlockCollision(void)
{
    int yPosition;
    u16 xPosition;
    u16 upperY;
    u16 leftX;
    s8 *blockAction;
    register s8 action PIN("r9");

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    blockAction = &gSpriteTileInteractionMode;
    action = 3;
    *blockAction = action;
    upperY = (u16) (yPosition - 4);
    leftX = (u16) (xPosition - 4);
    GetSpriteBlockCollisionAtPosition(upperY, leftX);
    {
        s8 value;
        s8 *pointer;

        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    xPosition = (u16) (xPosition + 4);
    GetSpriteBlockCollisionAtPosition(upperY, xPosition);
    {
        s8 value;
        s8 *pointer;

        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    yPosition = (u16) (yPosition - 68);
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
    {
        s8 value;
        s8 *pointer;

        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(yPosition, leftX);
}

void ClearDiceBlockCollision(void)
{
    int yPosition;
    u16 xPosition;
    u16 upperY;
    u16 leftX;
    s8 *blockAction;
    register s8 action PIN("r9");

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    blockAction = &gSpriteTileInteractionMode;
    action = 2;
    *blockAction = action;
    upperY = (u16) (yPosition - 4);
    leftX = (u16) (xPosition - 4);
    GetSpriteBlockCollisionAtPosition(upperY, leftX);
    {
        s8 value;
        s8 *pointer;

        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    xPosition = (u16) (xPosition + 4);
    GetSpriteBlockCollisionAtPosition(upperY, xPosition);
    {
        s8 value;
        s8 *pointer;

        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    yPosition = (u16) (yPosition - 68);
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
    {
        s8 value;
        s8 *pointer;

        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    GetSpriteBlockCollisionAtPosition(yPosition, leftX);
}

void InitDiceBlock(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.warioCollision = 25;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 124;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 60;
    gCurrentSprite.hitboxExtentRight = 56;
    GP_SET(gCurrentSprite.pOamData, sDiceBlockIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.xPosition += 32;
    gCurrentSprite.work1 = 0;
    SetDiceBlockCollision();
}

void DiceBlockCrushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceBlockCrushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = 60;
    ClearDiceBlockCollision();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void DiceBlockCrushed(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 0;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_6C;
    }
}

void DiceBlockBreakInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sDiceBlockBreakingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_6D;
    gCurrentSprite.work0 = 43;
    m4aSongNumStart(SE_SOUND_DICE_BLOCK_BREAK);
}

void DiceBlockBreak(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = SPRITE_STATUS_NONE;
    }
}

void SpriteDiceBlock(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitDiceBlock();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            DiceBlockCrushedInit();
        case SPOSE_CRUSHED_OR_COLLECTED:
            DiceBlockCrushed();
            func_8026838();
            break;
        case SPOSE_6C:
            DiceBlockBreakInit();
        case SPOSE_6D:
            DiceBlockBreak();
            func_8026838();
            break;
    }
}

#include "block.h"
#include "global_data.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/crumbling_block.h"

void SetCrumblingBlockCollision(void)
{
    int yPosition;
    u16 xPosition;
    u16 lowerY;
    u16 leftX;
    s8 *blockAction;
    register s8 action PIN("r9");

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    blockAction = &gSpriteTileInteractionMode;
    action = 3;
    *blockAction = action;
    lowerY = (u16)(yPosition + 4);
    leftX = (u16)(xPosition - 4);
    GetSpriteBlockCollisionAtPosition(lowerY, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    xPosition = (u16)(xPosition + 4);
    GetSpriteBlockCollisionAtPosition(lowerY, xPosition);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    yPosition = (u16)(yPosition - 4);
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

void ClearCrumblingBlockCollision(void)
{
    int yPosition;
    u16 xPosition;
    u16 lowerY;
    u16 leftX;
    s8 *blockAction;
    register s8 action PIN("r9");

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    blockAction = &gSpriteTileInteractionMode;
    action = 2;
    *blockAction = action;
    lowerY = (u16)(yPosition + 4);
    leftX = (u16)(xPosition - 4);
    GetSpriteBlockCollisionAtPosition(lowerY, leftX);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    xPosition = (u16)(xPosition + 4);
    GetSpriteBlockCollisionAtPosition(lowerY, xPosition);
    {
        s8 value;
        s8 *pointer;
        value = action;
        pointer = blockAction;
        *pointer = value;
    }
    yPosition = (u16)(yPosition - 4);
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

void InitCrumblingBlock(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.warioCollision = 6;
    gCurrentSprite.drawDistanceDown = 8;
    gCurrentSprite.drawDistanceUp = 8;
    gCurrentSprite.drawDistanceLeftRight = 8;
    gCurrentSprite.hitboxExtentUp = 40;
    gCurrentSprite.hitboxExtentDown = 28;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sCrumblingBlockIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.yPosition -= 32;
    SetCrumblingBlockCollision();
}

void StartCrumblingBlockCollapse(void)
{
    GP_SET(gCurrentSprite.pOamData, sCrumblingBlockCrackingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 50;
    gCurrentSprite.work0 = 88;
    if (gCurrentSprite.status & SPRITE_STATUS_ONSCREEN) {
        m4aSongNumStart(SE_CRUMBLING_BLOCK_CRACK);
    }
}

void CrumblingBlockCollapse(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    register s32 nextTimer PIN("r2");
    u8 timer;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    {
        u32 address;

        address = 39;
        address += (u32)sprite;
        timerPointer = (u8 *)address;
        {
            register int oldTimer PIN("r0");
            oldTimer = *(u8 *)address;
            nextTimer = oldTimer - 1;
            *timerPointer = nextTimer;
        }
    }
    {
        register u32 shiftedTimer PIN("r0");

        shiftedTimer = nextTimer << 24;
        ASM_BARRIER(: "+r"(shiftedTimer));
        timer = shiftedTimer >> 24;
    }
    if (timer != 0) {
        u16 comparison;

        comparison = timer;
        ASM_BARRIER(: "+r"(comparison));
        if (comparison == 38) {
            ClearCrumblingBlockCollision();
        }
    } else {
        GP_SET(sprite->pOamData, sCrumblingBlockCollapsedOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = SPOSE_12;
        {
            int resetTimer;
            resetTimer = 120;
            *timerPointer = resetTimer;
        }
    }
}

void CrumblingBlockRespawnDelay(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u8 value;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = &sprite->work0;
    value = --*timer;
    if (value == 0) {
        GP_SET(sprite->pOamData, sCrumblingBlockReformingOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->pose = SPOSE_14;
        *timer = 39;
        if (sprite->status & SPRITE_STATUS_ONSCREEN) {
            m4aSongNumStart(SE_CRUMBLING_BLOCK_REFORM);
        }
    }
}

void CrumblingBlockReform(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    register s32 nextTimer PIN("r2");
    u8 timer;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timerPointer = &sprite->work0;
    {
        register int oldTimer PIN("r0");

        oldTimer = *timerPointer;
        nextTimer = oldTimer - 1;
        *timerPointer = nextTimer;
    }
    {
        register u32 shiftedTimer PIN("r0");

        shiftedTimer = nextTimer << 24;
        ASM_BARRIER(: "+r"(shiftedTimer));
        timer = shiftedTimer >> 24;
    }
    if (timer != 0) {
        u16 comparison;

        comparison = timer;
        ASM_BARRIER(: "+r"(comparison));
        if (comparison == 21) {
            SetCrumblingBlockCollision();
        }
    } else {
        GP_SET(sprite->pOamData, sCrumblingBlockIdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = SPOSE_IDLE;
    }
}

void SpriteCrumblingBlock(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitCrumblingBlock();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            StartCrumblingBlockCollapse();
        case SPOSE_CRUSHED_OR_COLLECTED:
            CrumblingBlockCollapse();
            break;
        case SPOSE_12:
            CrumblingBlockRespawnDelay();
            break;
        case SPOSE_14:
            CrumblingBlockReform();
            break;
    }
}

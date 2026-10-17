#include "global_data.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/money_flower.h"
#include "gba/m4a.h"

void SetupMoneyFlower(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->warioCollision = 7;
    sprite->drawDistanceDown = 64;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 48;
    sprite->hitboxExtentUp = 112;
    sprite->hitboxExtentDown = 192;
    sprite->hitboxExtentLeft = 28;
    sprite->hitboxExtentRight = 24;
}

void InitMoneyFlowerSeed(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoneyFlowerSeedIdleOam);
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.pose = 23;
    SetupMoneyFlower();
}

void InitMoneyFlowerStage1(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoneyFlowerStage1IdleOam);
    gCurrentSprite.work1 = 1;
    gCurrentSprite.pose = 16;
    SetupMoneyFlower();
}

void InitMoneyFlowerStage2(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoneyFlowerStage2IdleOam);
    gCurrentSprite.work1 = 2;
    gCurrentSprite.pose = 19;
    gCurrentSprite.work0 = 100;
    SetupMoneyFlower();
}

void InitMoneyFlowerStage3(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoneyFlowerStage3IdleOam);
    gCurrentSprite.work1 = 3;
    gCurrentSprite.pose = 21;
    gCurrentSprite.work0 = 100;
    SetupMoneyFlower();
}

void InitMoneyFlowerStage4(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoneyFlowerStage4IdleOam);
    gCurrentSprite.work1 = 4;
    gCurrentSprite.pose = 47;
    gCurrentSprite.work0 = 100;
    SetupMoneyFlower();
}

void InitMoneyFlowerStage5(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoneyFlowerStage5IdleOam);
    gCurrentSprite.work1 = 5;
    gCurrentSprite.pose = 111;
    SetupMoneyFlower();
}

void InitMoneyFlowerCollected(void)
{
    GP_SET(gCurrentSprite.pOamData, sMoneyFlowerCollectedOam);
    gCurrentSprite.work1 = 6;
    gCurrentSprite.pose = 16;
    SetupMoneyFlower();
}

void MoneyFlowerSeedOpening(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerSeedOpeningOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 24;
        sprite->work0 = MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
        m4aSongNumStart(SE_MONEY_FLOWER_OPEN);
    }
}

void MoneyFlowerFinishStage1(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage1IdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 16;
        sprite->work1 = 1;
        gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = MAKE_PERSISTENT_DATA(SPOSE_INIT_1, PERSISTENT_STATUS_LOADED);
    }
}

void StartMoneyFlowerStage2Growth(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sMoneyFlowerStage2GrowingOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 18;
    sprite->work0 = 98;
    gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
    sprite->work1 = 2;
    m4aSongNumStart(SE_MONEY_FLOWER_GROW);
}

void MoneyFlowerStage2Idle(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage2IdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 19;
        sprite->work0 = 100;
    }
}

void StartMoneyFlowerStage3Growth(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage3GrowingOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 20;
        sprite->work0 = 4;
        gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = MAKE_PERSISTENT_DATA(SPOSE_INIT_3, PERSISTENT_STATUS_LOADED);
        sprite->work1 = 3;
        m4aSongNumStart(SE_MONEY_FLOWER_GROW);
    }
}

void MoneyFlowerStage3Idle(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage3IdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 21;
        sprite->work0 = 100;
    }
}

void StartMoneyFlowerStage4Growth(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage4GrowingOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 22;
        sprite->work0 = 4;
        gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = MAKE_PERSISTENT_DATA(SPOSE_INIT_4, PERSISTENT_STATUS_LOADED);
        sprite->work1 = 4;
        m4aSongNumStart(SE_MONEY_FLOWER_GROW);
    }
}

void MoneyFlowerStage4Idle(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage4IdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 47;
        sprite->work0 = 100;
    }
}

void StartMoneyFlowerStage5Growth(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage5GrowingOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 48;
        sprite->work0 = 4;
        gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = MAKE_PERSISTENT_DATA(SPOSE_INIT_5, PERSISTENT_STATUS_LOADED);
        sprite->work1 = 5;
        m4aSongNumStart(SE_MONEY_FLOWER_FULLY_GROWN);
    }
}

void MoneyFlowerStage5Idle(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerStage5IdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 111;
    }
}

void MoneyFlowerCollectedIdle(void)
{
}

void CollectMoneyFlower(void)
{
    struct PrimarySpriteData* initialSprite;
    struct PrimarySpriteData* sprite;
    s32 work1;
    u8 amount;

    initialSprite = &gCurrentSprite;
    work1 = initialSprite->work1;
    sprite = initialSprite;
    switch (work1)
    {
        case 2:
            GP_SET(sprite->pOamData, sMoneyFlowerStage2CollectionOam);
            sprite->work0 = 41;
            amount = 2;
            break;
        case 3:
            GP_SET(sprite->pOamData, sMoneyFlowerStage3CollectionOam);
            sprite->work0 = 41;
            amount = 5;
            break;
        case 4:
            GP_SET(sprite->pOamData, sMoneyFlowerStage4CollectionOam);
            sprite->work0 = 42;
            amount = 20;
            break;
        case 5:
            GP_SET(sprite->pOamData, sMoneyFlowerStage5CollectionOam);
            sprite->work0 = 30;
            amount = 1;
            break;
        default:
            break;
    }

    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 50;
    gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = MAKE_PERSISTENT_DATA(SPOSE_INIT_6, PERSISTENT_STATUS_LOADED);
    sprite->work1 = 6;
    SpawnScoreCoinDropSprites(sprite->yPosition - 120, sprite->xPosition, amount);
}

void FinishMoneyFlowerCollection(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sMoneyFlowerCollectedOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 16;
    }
}

void SpriteMoneyFlower(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose)
    {
        case 0:
            InitMoneyFlowerSeed();
            break;
        case 1:
            InitMoneyFlowerStage1();
            break;
        case 2:
            InitMoneyFlowerStage2();
            break;
        case 3:
            InitMoneyFlowerStage3();
            break;
        case 4:
            InitMoneyFlowerStage4();
            break;
        case 5:
            InitMoneyFlowerStage5();
            break;
        case 6:
            InitMoneyFlowerCollected();
            break;
        case 23:
            MoneyFlowerSeedOpening();
            break;
        case 24:
            MoneyFlowerFinishStage1();
            break;
        case 17:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            StartMoneyFlowerStage2Growth();
        case 18:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            MoneyFlowerStage2Idle();
            break;
        case 19:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            StartMoneyFlowerStage3Growth();
            break;
        case 20:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            MoneyFlowerStage3Idle();
            break;
        case 21:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            StartMoneyFlowerStage4Growth();
            break;
        case 22:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            MoneyFlowerStage4Idle();
            break;
        case 47:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            StartMoneyFlowerStage5Growth();
            break;
        case 48:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            MoneyFlowerStage5Idle();
            break;
        case 111:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            MoneyFlowerCollectedIdle();
            break;
        case 49:
            CollectMoneyFlower();
        case 50:
            FinishMoneyFlowerCollection();
            break;
        default:
            break;
    }
}

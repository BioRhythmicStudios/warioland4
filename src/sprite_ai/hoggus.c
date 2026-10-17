#include "background_registers.h"
#include "sprite.h"
#include "sprite_util.h"
#include "transparency.h"
#include "sprite_ai/hoggus.h"

void SpriteSpawnAsChildWide(u32 id, u32 roomSlot, u32 gfxSlot, u32 yPosition, u32 xPosition) asm("SpriteSpawnAsChild");

void UpdateHoggusPosition(void)
{
    register struct PrimarySpriteData* sprite PIN("r2");
    u16 spriteCoordinate;
    u16 targetCoordinate;
    u16 distance;
    u16 velocity;

    sprite = &gCurrentSprite;
    spriteCoordinate = sprite->xPosition + 0x200;
    targetCoordinate = gWarioData.xPosition + 0x200;

    if (gWarioData.horizontalDirection & 0x20)
    {
        targetCoordinate -= 0x180;
        if (spriteCoordinate > targetCoordinate)
            distance = spriteCoordinate - targetCoordinate;
        else
            distance = targetCoordinate - spriteCoordinate;

        if (distance <= 8)
        {
            velocity = 0;
        }
        else
        {
            velocity = (distance - 8) >> 2;
            if (velocity > 16)
                velocity = 16;
        }

        if (spriteCoordinate > targetCoordinate)
            sprite->xPosition -= velocity;
        else if (spriteCoordinate < targetCoordinate)
            sprite->xPosition += velocity;
    }
    else
    {
        targetCoordinate += 0x140;
        if (spriteCoordinate > targetCoordinate)
            distance = spriteCoordinate - targetCoordinate;
        else
            distance = targetCoordinate - spriteCoordinate;

        velocity = (distance - 8) >> 2;
        if (velocity > 16)
            velocity = 16;

        if (spriteCoordinate < targetCoordinate)
            sprite->xPosition += velocity;
        else if (spriteCoordinate > targetCoordinate)
            sprite->xPosition -= velocity;
    }

    spriteCoordinate = sprite->yPosition + 0x200;
    targetCoordinate = gBg1YPosition + 0x2AA;
    if (spriteCoordinate > targetCoordinate)
        distance = spriteCoordinate - targetCoordinate;
    else
        distance = targetCoordinate - spriteCoordinate;

    if (distance <= 8)
    {
        velocity = 0;
    }
    else
    {
        velocity = (distance - 8) >> 5;
        if (velocity <= 1)
            velocity = 2;
    }

    if (spriteCoordinate > targetCoordinate)
        (*(volatile u16 *)&sprite->yPosition) -= velocity;
    else if (spriteCoordinate < targetCoordinate)
        (*(volatile u16 *)&sprite->yPosition) += velocity;
}

void InitHoggus(void)
{
    u32 existingSprite;
    struct PrimarySpriteData* sprite;
    u8* work2;

    existingSprite = SpriteUtilDespawnIfSpriteExists(PSPRITE_HOGGUS);
    if (existingSprite == 0)
    {
        sprite = &gCurrentSprite;
        sprite->status |= 0x408;
        sprite->warioCollision = 0;
        sprite->drawDistanceDown = 32;
        sprite->drawDistanceUp = 32;
        sprite->drawDistanceLeftRight = 64;
        sprite->hitboxExtentUp = 4;
        sprite->hitboxExtentDown = 4;
        sprite->hitboxExtentLeft = 4;
        sprite->hitboxExtentRight = 4;
        GP_SET(sprite->pOamData, sHoggusIdleOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = existingSprite;
        sprite->pose = 15;
        sprite->work1 = 1;
        work2 = &sprite->work2;
        *work2 = 0;
    }
}

void StartHoggusIdle(void)
{
    struct PrimarySpriteData* sprite;
    u8* direction;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sHoggusIdleOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
    sprite->work0 = 60;
    if (sprite->xPosition > gWarioData.xPosition)
    {
        direction = &sprite->work2;
        *direction = 0;
    }
    else
    {
        direction = &sprite->work2;
        *direction = 1;
    }
}

void HoggusIdle(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    int count;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    {
        u8 value;

        value = *timer;
        if (value != 0)
        {
            value--;
            *timer = value;
        }
        else
        {
            func_8023BFC((u16)(sprite->yPosition - 32), (u16)(sprite->xPosition + 96));
        if ((gSpriteCollisionTileType & 0xF) == 0)
        {
            count = SpriteUtilCountSpriteType(PSPRITE_DENDEN);
            count += SpriteUtilCountSpriteType(PSPRITE_BUTATABI);
            if (count <= 1)
            {
                sprite->pose = 17;
                *timer = 120;
                {
                    u8* direction;

                    direction = &sprite->work1;
                    *direction ^= 1;
                }
            }
            }
        }
    }
    UpdateHoggusPosition();
}

void StartHoggusCharge(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sHoggusChargeOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 18;
    sprite->work0 = 160;
    m4aSongNumStart(SE_HOGGUS_CHARGE);
}

void HoggusCharge(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    int value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    if ((value << 24) == 0)
        sprite->pose = 19;
    UpdateHoggusPosition();
}

void StartHoggusAttack(void)
{
    struct PrimarySpriteData* initialSprite;
    struct PrimarySpriteData* sprite;

    initialSprite = &gCurrentSprite;
    {
        u8 work1;

        work1 = initialSprite->work1;
        sprite = initialSprite;
        if (work1 != 0)
            GP_SET(sprite->pOamData, sHoggusAttackVariant1Oam);
        else
            GP_SET(sprite->pOamData, sHoggusAttackVariant0Oam);
    }
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 20;
    {
        u8* timer;

        timer = &sprite->work0;
        *timer = 40;
    }
    m4aSongNumStart(SE_HOGGUS_ATTACK);
}

void HoggusAttack(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    int value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    if ((value << 24) == 0)
        sprite->pose = 21;
    UpdateHoggusPosition();
}

void StartHoggusSpawn(void)
{
    struct PrimarySpriteData* initialSprite;
    struct PrimarySpriteData* sprite;

    initialSprite = &gCurrentSprite;
    {
        u8 work1;

        work1 = initialSprite->work1;
        sprite = initialSprite;
        if (work1 != 0)
            GP_SET(sprite->pOamData, sHoggusSpawnVariant1Oam);
        else
            GP_SET(sprite->pOamData, sHoggusSpawnVariant0Oam);
    }
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 22;
    {
        u8* timer;

        timer = &sprite->work0;
        *timer = 40;
    }
}

void HoggusSpawn(void)
{
    struct PrimarySpriteData* sprite;
    u8* variant;
    u8* timerPointer;
    register int timer PIN("r1");
    int pose;
    register int collisionState PIN("r1");
    int maskedState;
    int shiftedTimer;

    sprite = &gCurrentSprite;
    func_8023BFC((u16)(sprite->yPosition - 32), (u16)(sprite->xPosition + 96));
    collisionState = gSpriteCollisionTileType;
    pose = 15;
    maskedState = pose;
    maskedState &= collisionState;
    if (maskedState != 0)
        goto set_idle_pose;

    timerPointer = &sprite->work0;
    shiftedTimer = *timerPointer;
    timer = shiftedTimer - 1;
    *timerPointer = timer;
    shiftedTimer = timer << 24;
    if (shiftedTimer == 0)
        goto set_idle_pose;

    if ((u32)shiftedTimer >> 24 == 10)
    {
        variant = &sprite->work1;
        SpriteSpawnAsChild(PSPRITE_D0, *variant, sprite->gfxSlot,
            sprite->yPosition - 32, sprite->xPosition + 96);
        if (*variant != 0)
            m4aSongNumStart(SE_HOGGUS_SPAWN_BUTATABI);
        else
            m4aSongNumStart(SE_HOGGUS_SPAWN_DENDEN);
    }
    goto update_movement;

set_idle_pose:
    sprite->pose = pose;

update_movement:
    UpdateHoggusPosition();
}

void StartHoggusEscape(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sHoggusIdleOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 111;
    sprite->status &= 0x7FFF;
}

void HoggusEscape(void)
{
    gCurrentSprite.yPosition -= 8;
}

void InitHoggusEgg(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->status &= 0xFFFB;
    sprite->status |= 0x400;
    sprite->drawPriority = 0;
    sprite->warioCollision = 0;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = 32;
    sprite->drawDistanceLeftRight = 32;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 4;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    GP_SET(sprite->pOamData, sHoggusEggOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 15;
    sprite->work0 = 36;
}

void HoggusEggUpdate(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPointer;
    register int timer PIN("r1");
    register int value PIN("r0");

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    value = *timerPointer;
    timer = value - 1;
    *timerPointer = timer;
    value = (u8)timer;
    if (value == 0)
        goto despawn;

    value = (u8)timer;
    if (value != 16)
        goto finished;

    value = sprite->roomSlot;
    if (value == 0)
    {
        int roomSlot;
        int gfxSlot;

        roomSlot = value;
        gfxSlot = sprite->gfxSlot + 3;
        SpriteSpawnAsChildWide(PSPRITE_DENDEN, roomSlot, gfxSlot,
            sprite->yPosition + 16, sprite->xPosition);
    }
    else
    {
        int roomSlot;
        int gfxSlot;

        roomSlot = *(volatile u8 *)((u8 *)(sprite) + (24));
        gfxSlot = sprite->gfxSlot + 5;
        SpriteSpawnAsChildWide(PSPRITE_BUTATABI, roomSlot, gfxSlot,
            sprite->yPosition + 16, sprite->xPosition);
    }
    goto finished;

despawn:
    sprite->status = value;

finished:
    return;
}

void SpriteHoggus(void)
{
    struct PrimarySpriteData* initialSprite;
    struct PrimarySpriteData* sprite;
    u8 pose;

    initialSprite = &gCurrentSprite;
    pose = initialSprite->pose;
    sprite = initialSprite;
    if (pose != 110 && gUnk_30000D8.targetAlpha == 16)
        sprite->pose = 110;

    switch (sprite->pose)
    {
        case 0:
            InitHoggus();
            break;
        case 15:
            StartHoggusIdle();
        case 16:
            HoggusIdle();
            break;
        case 17:
            StartHoggusCharge();
        case 18:
            HoggusCharge();
            break;
        case 19:
            StartHoggusAttack();
        case 20:
            HoggusAttack();
            break;
        case 21:
            StartHoggusSpawn();
        case 22:
            HoggusSpawn();
            break;
        case 110:
            StartHoggusEscape();
        case 111:
            HoggusEscape();
            break;
    }
}

void SpriteHoggusEgg(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0)
        InitHoggusEgg();
    HoggusEggUpdate();
}

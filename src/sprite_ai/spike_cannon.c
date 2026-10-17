#include "global_data.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/spike_cannon.h"
#include "sprite_util.h"

void InitSpikeCannon(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprite;
    u16 oldStatus;
    register u16 status PIN("r0");
    u32 flags;
    register int zeroByte PIN("r2");
    int zeroHalf;

    current = &gCurrentSprite;
    oldStatus = current->status;
    flags = SPRITE_STATUS_HIDDEN | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    status = flags;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    sprite = current;
    sprite->status = status;
    current->drawDistanceDown = 1;
    current->drawDistanceUp = 1;
    current->drawDistanceLeftRight = 1;
    current->hitboxExtentUp = 1;
    current->hitboxExtentDown = 1;
    current->hitboxExtentLeft = 1;
    current->hitboxExtentRight = 1;
    GP_SET(sprite->pOamData, sSpikeCannonOam);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->pose = 15;
    sprite->warioCollision = zeroByte;
    current->work0 = 80;
    sprite->drawPriority = 1;
}

void SpikeCannonWait(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    int next;
    u8 value;

    if (SpriteUtilCheckWarioNearbyAboveBelow(70, 96) != 1) {
        sprite = &gCurrentSprite;
        timer = &sprite->work0;
        next = *timer;
        next--;
        {
            u8 *storePointer;
            storePointer = timer;
            *storePointer = next;
        }
        value = (u8)next;
        if (value == 0) {
            sprite->status &= ~SPRITE_STATUS_HIDDEN;
            sprite->currentAnimationFrame = 0;
            sprite->animationTimer = value;
            sprite->pose = 16;
            {
                int resetValue;
                u8 *resetPointer;
                resetValue = 72;
                resetPointer = timer;
                *resetPointer = resetValue;
            }
        }
    }
}

void SpikeCannonFire(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    int next;
    u8 value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    next = *timer;
    next--;
    *timer = next;
    value = (u8)next;
    if (value == 0) {
        sprite->status |= SPRITE_STATUS_HIDDEN;
        SpawnPrimarySpriteWithStatus(184, sprite->roomSlot, sprite->gfxSlot, sprite->yPosition,
                     sprite->xPosition + 32, value);
        SpawnPrimarySpriteWithStatus(184, sprite->roomSlot, sprite->gfxSlot, sprite->yPosition,
                     sprite->xPosition + 32, SPRITE_STATUS_FACING_RIGHT);
        *timer = 220;
        sprite->pose = 15;
        m4aSongNumStart(SE_SPIKE_CANNON_FIRE);
    }
}

void InitSpikeCannonProjectile(void)
{
    register struct PrimarySpriteData *sprite PIN("ip");
    u16 oldStatus;
    register u16 status PIN("r3");
    register int zeroByte PIN("r5");
    int zeroHalf;
    int distance;
    u8 *bounds;
    int horizontalExtent;
    int verticalExtent;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_HEAVY;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    {
        register struct PrimarySpriteData *storePointer PIN("r1");
        storePointer = sprite;
        storePointer->status = status;
    }
    bounds = (u8 *)sprite + 32;
    distance = 16;
    *bounds = distance;
    bounds++;
    *bounds = distance;
    bounds++;
    *bounds = distance;
    bounds++;
    horizontalExtent = 32;
    *bounds = horizontalExtent;
    bounds++;
    verticalExtent = 28;
    *bounds = verticalExtent;
    bounds++;
    *bounds = horizontalExtent;
    bounds++;
    *(volatile u8 *)bounds = verticalExtent;
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    {
        int collision;
        register u8 *workPointer PIN("r1");
        collision = 9;
        workPointer = (u8 *)sprite;
        workPointer[30] = collision;
        workPointer += 39;
        *workPointer = 36;
        workPointer += 2;
        *(volatile u8 *)workPointer = 4;
    }
    sprite->work3 = zeroByte;
    sprite->pose = distance;
    {
        u32 facingTest;
        facingTest = 64;
        facingTest &= status;
        ASM_BARRIER_VOLATILE(: "+r"(facingTest));
    }
    {
        const struct AnimationFrame *oam;
        register struct PrimarySpriteData *storePointer PIN("r1");
        oam = sSpikeCannonProjectileFlyingOam;
        storePointer = sprite;
        GP_SET(storePointer->pOamData, oam);
    }
}

void SpikeCannonProjectileMove(void)
{
    struct PrimarySpriteData *sprite;
    const s16 *movement;
    int index;
    u8 *indexPointer;
    u8 *timer;
    int timerValue;
    s16 delta;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    timerValue = *timer;
    if (timerValue != 0) {
        timerValue--;
        *timer = timerValue;
        sprite->currentAnimationFrame--;
        sprite->yPosition -= 2;
        return;
    }

    func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1 || (gSpriteCollisionResult & 0xF) != 0) {
        sprite->pose = 49;
        return;
    }

    indexPointer = &sprite->work3;
    index = *indexPointer;
    movement = sSpikeCannonProjectileYVelocity;
    delta = movement[index];
    if (delta == 0x7FFF) {
        int previous;
        u16 yPosition;
        register const s16 *previousPointer PIN("r1");
        u16 previousDelta;

        previous = index - 1;
        previous <<= 1;
        previous += (u32)movement;
        previousPointer = (const s16 *)previous;
        yPosition = sprite->yPosition;
        previousDelta = *previousPointer;
        yPosition += previousDelta;
        sprite->yPosition = yPosition;
    } else {
        int nextIndex;
        u8 *storePointer;
        nextIndex = index + 1;
        storePointer = indexPointer;
        *storePointer = nextIndex;
        sprite->yPosition += delta;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += gCurrentSprite.work2;
    } else {
        gCurrentSprite.xPosition -= gCurrentSprite.work2;
    }
}

void StartSpikeCannonProjectileDespawn(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    GP_SET(sprite->pOamData, sSpikeCannonProjectileDespawnOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 50;
    sprite->work0 = 27;
    sprite->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
}

void SpikeCannonProjectileDespawn(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u8 value;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = &sprite->work0;
    value = --*timer;
    if (value == 0) {
        sprite->status = 0;
    }
}

void SpriteSpikeCannon(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitSpikeCannon();
            break;
        case 15:
            SpikeCannonWait();
            break;
        case 16:
            SpikeCannonFire();
            break;
    }
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void SpriteSpikeCannonProjectile(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitSpikeCannonProjectile();
            break;
        case 16:
            SpikeCannonProjectileMove();
            break;
        case 50:
            SpikeCannonProjectileDespawn();
            break;
        default:
            StartSpikeCannonProjectileDespawn();
            break;
    }
}

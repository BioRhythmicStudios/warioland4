#include "sprite_ai/bubble.h"

#include "global_data.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"


/* Untyped function-pointer view prevents agbcc from re-normalizing an already-clean r5 argument. */
#define BubbleCollisionCheck ((void (*)())func_8023BFC)


void InitBubbleSpawner(void)
{
    register struct PrimarySpriteData *sprite asm("ip");
    u16 oldStatus;
    register u16 status asm("r0");
    register int zeroByte asm("r2");
    int zeroHalf;
    u8 *bounds;
    int one;
    const struct AnimationFrame *oam;
    register struct PrimarySpriteData *postSprite asm("r1");

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_HIDDEN;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    sprite->status = status;
    bounds = (u8 *)sprite + 32;
    one = 1;
    *bounds = one;
    bounds++;
    *bounds = one;
    bounds++;
    *bounds = one;
    bounds++;
    *bounds = one;
    bounds++;
    *bounds = one;
    bounds++;
    *bounds = one;
    bounds++;
    *(volatile u8 *)bounds = one;
    oam = sBubbleSpawnerOam;
    postSprite = sprite;
    postSprite->pOamData = oam;
    postSprite->currentAnimationFrame = zeroByte;
    postSprite->animationTimer = zeroHalf;
    postSprite->pose = 16;
    postSprite->warioCollision = 6;
    postSprite->work0 = 60;
}

void BubbleSpawnerIdle(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    int timerValue;
    u8 timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    timerValue = *timerPointer;
    timerValue--;
    *timerPointer = timerValue;
    timer = (u8)timerValue;
    if (timer == 0) {
        SpawnPrimarySpriteWithStatus(PSPRITE_9B, sprite->roomSlot, sprite->gfxSlot,
                     sprite->yPosition, sprite->xPosition, timer);
        *timerPointer = 250;
    }
}

void StartBubblePop(void)
{
    struct PrimarySpriteData *sprite;
    int temp;

    sprite = &gCurrentSprite;
    temp = 0;
    sprite->pose = 50;
    sprite->pOamData = sBubblePopOam;
    sprite->currentAnimationFrame = temp;
    sprite->animationTimer = temp;
    temp = (int)sprite;
    temp += 39;
    *(u8 *)temp = 12;
    sprite->disableWarioCollisionTimer = 1;
}

void MoveBubble(void)
{
    const u16 *velocityTable;
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *movementSprite asm("r4");
    u8 *phasePointer;
    u8 *directionPointer;
    int direction;
    u16 velocity;
    u8 *phaseIncrementPointer;
    int phaseValue;
    int zero;
    register u32 r0Work asm("r0");
    register u32 collisionWork asm("r6");

    velocityTable = sHorizontalOscillationVelocity;
    current = &gCurrentSprite;
    phasePointer = &current->work3;
    velocity = velocityTable[*phasePointer >> 3];
    directionPointer = &current->work1;
    direction = *directionPointer;
    sprite = current;
    if (direction != 0) {
        u16 xPosition = sprite->xPosition;
        xPosition += velocity;
        sprite->xPosition = xPosition;
    } else {
        sprite->xPosition -= velocity;
    }

    if (*phasePointer > 94) {
        *phasePointer = 0;
        *directionPointer ^= 1;
    }

    movementSprite = sprite;
    phaseIncrementPointer = &movementSprite->work3;
    phaseValue = *phaseIncrementPointer;
    phaseValue++;
    {
        u8 *phaseStorePointer;
        phaseStorePointer = phaseIncrementPointer;
        *phaseStorePointer = phaseValue;
    }

    {
        u16 oldY;
        int yValue;
        register int zeroSource asm("r2");
        register u8 *wallCooldownPointer;
        register u8 *wallCollisionResult asm("r9");
        register int wallCollisionValue asm("r1");
        int maskedCollision;

        oldY = movementSprite->yPosition;
        yValue = oldY - 2;
        zeroSource = 0;
        zero = zeroSource;
        movementSprite->yPosition = yValue;
        wallCooldownPointer = &movementSprite->work2;
        maskedCollision = *wallCooldownPointer;
        if (maskedCollision == 0) {
            func_8023BFC(oldY - 66,
                         movementSprite->xPosition - movementSprite->hitboxExtentLeft);
            wallCollisionResult = &gSpriteCollisionTileType;
            wallCollisionValue = *wallCollisionResult;
            collisionWork = 15;
            maskedCollision = collisionWork;
            maskedCollision &= wallCollisionValue;
            if (maskedCollision != 0) {
                r0Work = (u32)movementSprite;
                goto wallCollision;
            }

            func_8023BFC(sprite->yPosition - 64,
                         sprite->xPosition + sprite->hitboxExtentRight);
            wallCollisionValue = *wallCollisionResult;
            maskedCollision = collisionWork;
            maskedCollision &= wallCollisionValue;
            if (maskedCollision != 0) {
                r0Work = (u32)sprite;
wallCollision:
                {
                    u8 *hitDirectionPointer;
                    int hitDirection;
                    int one;
                    hitDirectionPointer = &((struct PrimarySpriteData *)r0Work)->work1;
                    hitDirection = *hitDirectionPointer;
                    one = 1;
                    hitDirection ^= one;
                    *hitDirectionPointer = hitDirection;
                }
                *wallCooldownPointer = collisionWork;
                zeroSource = zero;
                {
                    u8 *phaseStorePointer;
                    phaseStorePointer = phaseIncrementPointer;
                    *phaseStorePointer = zeroSource;
                }
                goto end;
            }
        } else {
            maskedCollision--;
            *wallCooldownPointer = maskedCollision;
        }
    }

    {
        u8 *extentPointer;
        int extent;
        register int topY asm("r5");
        int xValue;
        int groundCollisionValue;
        register int groundCollisionMask;

        movementSprite = &gCurrentSprite;
        extentPointer = &movementSprite->hitboxExtentUp;
        extent = *extentPointer;
        r0Work = movementSprite->yPosition;
        r0Work -= extent;
        topY = (u16)r0Work;
        xValue = movementSprite->xPosition;
        xValue -= 28;
        xValue <<= 16;
        xValue = (u32)xValue >> 16;
        BubbleCollisionCheck(topY, xValue);
        collisionWork = (u32)&gSpriteCollisionTileType;
        groundCollisionValue = *(u8 *)collisionWork;
        groundCollisionMask = 15;
        asm("" : "+r"(groundCollisionMask));
        r0Work = groundCollisionMask;
        r0Work &= groundCollisionValue;
        if (r0Work != 0) {
            StartBubblePop();
            goto end;
        }

        xValue = movementSprite->xPosition;
        xValue += 28;
        xValue <<= 16;
        xValue = (u32)xValue >> 16;
        BubbleCollisionCheck(topY, xValue);
        groundCollisionValue = *(u8 *)collisionWork;
        asm("" : "+r"(groundCollisionMask));
        r0Work = groundCollisionMask;
        r0Work &= groundCollisionValue;
        if (r0Work != 0) {
            StartBubblePop();
            goto end;
        }

        func_8023BFC(movementSprite->yPosition - 40, movementSprite->xPosition);
        if (gBackgroundCollisionData.waterType != 1) {
            StartBubblePop();
        }
    }

end:
    return;
}

void UpdateBubblePop(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPointer;
    int timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    timer = *timerPointer;
    timer--;
    *timerPointer = timer;
    timer = (u8)timer;
    if (timer != 0) {
        sprite->disableWarioCollisionTimer = 1;
    } else {
        sprite->status = timer;
    }
}

void InitBubble(void)
{
    struct PrimarySpriteData *sprite;
    register int zero asm("r4");
    register u16 oldStatus asm("r0");
    register u16 status asm("r1");
    int one;
    register u8 *bounds asm("r2");
    u8 *bytePointer;
    register int condition asm("r1");

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = oldStatus & ~SPRITE_STATUS_HIDDEN;
    zero = 0;
    status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    /* agbcc otherwise removes the required zero OR used by the original register flow. */
    asm volatile("" : "+r"(zero));
    status |= zero;
    sprite->status = status;
    bounds = (u8 *)sprite + 32;
    *bounds = 32;
    bytePointer = (u8 *)sprite + 33;
    *bytePointer = zero;
    bounds += 2;
    *bounds = 16;
    bounds++;
    *bounds = 112;
    bounds++;
    *bounds = 144;
    bounds++;
    *bounds = 48;
    bounds++;
    *(volatile u8 *)bounds = 44;
    sprite->warioCollision = 74;
    one = 1;
    sprite->drawPriority = one;

    status &= SPRITE_STATUS_FACING_RIGHT;
    condition = status;
    if (condition != 0) {
        if (gUnk_3000027 == 33) {
            sprite->gfxSlot = 5;
        }
        sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
        StartBubblePop();
    } else {
        sprite->pOamData = sBubbleIdleOam;
        /* This byte-lvalue form lets agbcc store the pinned zero directly from r4. */
        *(u8 *)&sprite->currentAnimationFrame = zero;
        sprite->animationTimer = status;
        sprite->pose = 111;
        sprite->work0 = 26;
        bytePointer = (u8 *)sprite + 41;
        *bytePointer = zero;
        bytePointer++;
        *bytePointer = zero;
        if (sprite->xPosition > gWarioData.xPosition) {
            bytePointer = (u8 *)sprite + 40;
            *bytePointer = zero;
        } else {
            bytePointer = (u8 *)sprite + 40;
            *bytePointer = one;
        }
    }
}

void BubbleWaitBeforeMoving(void)
{
    struct PrimarySpriteData *sprite;
    int timerValue;
    int timer;
    u8 *timerPointer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    timerValue = *timerPointer;
    timerValue--;
    *timerPointer = timerValue;
    timer = (u8)timerValue;
    if (timer != 0) {
        sprite->disableWarioCollisionTimer = 1;
    } else {
        sprite->pOamData = sBubbleMovingOam;
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 16;
    }
}

void SpriteBubble(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitBubbleSpawner();
            break;
        case 16:
            BubbleSpawnerIdle();
            break;
    }
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void SpriteBubbleInstance(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprite;
    s32 pose;

    current = &gCurrentSprite;
    pose = current->pose;
    sprite = current;
    switch (pose) {
        case 0:
            InitBubble();
            break;
        case 111:
            BubbleWaitBeforeMoving();
            /* fallthrough */
        case 16:
            MoveBubble();
            break;
        case 50:
            UpdateBubblePop();
            break;
        default:
            sprite->status = 0;
            break;
    }
}

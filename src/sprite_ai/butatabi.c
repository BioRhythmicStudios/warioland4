#include "sprite_ai/butatabi.h"

#include "score.h"
#include "sound.h"
#include "sprite_util.h"

void SetButatabiHitbox(void)
{
    struct PrimarySpriteData* sprite;
    u8* extent;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        extent = &sprite->hitboxExtentLeft;
        *extent = 40;
        extent++;
        *extent = 80;
    } else {
        extent = &sprite->hitboxExtentLeft;
        *extent = 84;
        extent++;
        *extent = 36;
    }
}

void InitButatabi(void)
{
    struct PrimarySpriteData* sprite;
    register u8 zero PIN("r6");
    s32 sixteen;
    u8 thirtyTwo;
    u16 zero16;
    register u16 status PIN("r1");
    u16 maskedStatus;
    u8* distance;

    sprite = &gCurrentSprite;
    status = sprite->status;
    maskedStatus = status & ~SPRITE_STATUS_HIDDEN;
    zero = 0;
    status = 0;
    zero16 = status;
    sprite->status = maskedStatus;
    sprite->warioCollision = 3;
    sprite->drawDistanceDown = 24;
    distance = &sprite->drawDistanceUp;
    sixteen = 16;
    *distance = sixteen;
    distance++;
    thirtyTwo = 32;
    *distance = thirtyTwo;
    SetButatabiHitbox();
    sprite->hitboxExtentUp = sixteen;
    sprite->hitboxExtentDown = thirtyTwo;
    GP_SET(sprite->pOamData, sButatabiMovingOam);
    ASM_BARRIER(: "+r"(zero));
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero16;
    sprite->pose = sixteen;
    sprite->work2 = zero;
    sprite->work3 = zero;
    sprite->drawPriority = zero;
}

void StartButatabiTackled(void)
{
    struct PrimarySpriteData* sprite;
    u16 frameZero;
    u8 zero;
    u8* work3Pointer;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sButatabiFallingOam);
    frameZero = 0;
    sprite->currentAnimationFrame = frameZero;
    zero = 0;
    sprite->animationTimer = frameZero;
    sprite->health = zero;
    work3Pointer = &sprite->work3;
    *work3Pointer = zero;
    sprite->drawPriority = zero;
    work3Pointer--;
    *work3Pointer = 8;
    sprite->status = (sprite->status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) | SPRITE_STATUS_MAYBE_DEAD;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, SSPRITE_06);
}

void InitButatabiTackledRight(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    StartButatabiTackled();
}

void InitButatabiTackledLeft(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    StartButatabiTackled();
}

void StartButatabiBumped(void)
{
    struct PrimarySpriteData* sprite;
    u16 frameZero;
    u8 zero;
    u8* work3Pointer;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sButatabiFallingOam);
    frameZero = 0;
    sprite->currentAnimationFrame = frameZero;
    zero = 0;
    sprite->animationTimer = frameZero;
    sprite->health = zero;
    work3Pointer = &sprite->work3;
    *work3Pointer = zero;
    sprite->drawPriority = zero;
    work3Pointer--;
    *work3Pointer = 7;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, SSPRITE_06);
}

void InitButatabiBumpedRight(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    StartButatabiBumped();
}

void InitButatabiBumpedLeft(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    StartButatabiBumped();
}

void ButatabiHop(void)
{
    struct PrimarySpriteData* sprite;
    u8* gravityIndex;
    u32 gravityTimer;
    const s16* gravityTable;
    u32 gravityAddress;
    const s16* gravityEntry;
    u32 rawVelocity;
    u32 zero;
    s32 velocity;
    register u32 position PIN("r0");

    sprite = &gCurrentSprite;
    gravityIndex = &sprite->work3;
    gravityTimer = *gravityIndex;
    if (gravityTimer == 1) {
        m4aSongNumStart(SE_BUTATABI_HOP);
    }

    gravityTable = sButatabiYVelocity;
    gravityAddress = gravityTimer << 1;
    gravityAddress += (u32)gravityTable;
    gravityEntry = (const s16*)gravityAddress;
    rawVelocity = *(const u16*)gravityEntry;
    zero = 0;
    velocity = *(s16 *)((u8 *)gravityEntry + zero);
    if (velocity == S16_MAX) {
        u32 previousAddress;
        const s16* previousEntry;

        previousAddress = gravityTimer - 1;
        previousAddress <<= 1;
        previousAddress += (u32)gravityTable;
        previousEntry = (const s16*)previousAddress;
        position = sprite->yPosition;
        position += *(const u16*)previousEntry;
    } else {
        u32 nextTimer;

        nextTimer = gravityTimer + 1;
        *gravityIndex = nextTimer;
        position = sprite->yPosition;
        position += rawVelocity;
    }
    sprite->yPosition = position;

    {
        struct PrimarySpriteData* current;
        u8* timerPointer;

        current = &gCurrentSprite;
        timerPointer = &current->work2;
        gravityTimer = *timerPointer;
        if (gravityTimer == 80) {
            register u32 animationZero PIN("r0");

            GP_SET(current->pOamData, sButatabiTurningOam);
            animationZero = 0;
            *(u8 *)((u8 *)current + 22) = animationZero;
            current->animationTimer = animationZero;
        } else if (gravityTimer == 88) {
            register u32 animationZero PIN("r0");

            GP_SET(current->pOamData, sButatabiTurnAroundOam);
            animationZero = 0;
            *(u8 *)((u8 *)current + 22) = animationZero;
            current->animationTimer = animationZero;
            current->status ^= SPRITE_STATUS_FACING_RIGHT;
            SetButatabiHitbox();
        }
    }

    {
        const u8* xTable;
        u32 xAddress;
        const u8* xEntry;
        u16 zeroOffset;
        register struct PrimarySpriteData* current PIN("r0");
        u8* timerPointer;
        u32 oneOrNext;

        xTable = (const u8*)sButatabiXVelocity;
        xAddress = (gravityTimer >> 1) << 1;
        xAddress += (u32)xTable;
        xEntry = (const u8*)xAddress;
        rawVelocity = *(const u16*)xEntry;
        zeroOffset = 0;
        velocity = *(s16 *)(xEntry + zeroOffset);
        if (velocity == S16_MAX) {
            current = &gCurrentSprite;
            timerPointer = &current->work2;
            oneOrNext = 1;
            *timerPointer = oneOrNext;
            timerPointer++;
            *(volatile u8 *)timerPointer = oneOrNext;
            gravityTimer = 0;
            rawVelocity = *(const u16*)xTable;
            GP_SET(current->pOamData, sButatabiMovingOam);
            current->currentAnimationFrame = gravityTimer;
            current->animationTimer = gravityTimer;
        } else {
            oneOrNext = gravityTimer;
            oneOrNext += 1;
            current = &gCurrentSprite;
            timerPointer = &current->work2;
            *timerPointer = oneOrNext;
        }

        {
            struct PrimarySpriteData* positionSprite;
            s32 status;
            u32 facing;
            u32 xPosition;

            positionSprite = current;
            status = positionSprite->status;
            facing = SPRITE_STATUS_FACING_RIGHT;
            facing &= status;
            if (facing) {
                xPosition = positionSprite->xPosition;
                xPosition += rawVelocity;
            } else {
                xPosition = positionSprite->xPosition;
                xPosition -= rawVelocity;
            }
            positionSprite->xPosition = xPosition;
        }
    }
}

void SpriteButatabi(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitButatabi();
            break;
        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            SpriteUtilDie();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_33:
        case SPOSE_4F:
            InitButatabiTackledRight();
            break;
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_35:
        case SPOSE_50:
            InitButatabiTackledLeft();
            break;
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_69:
            InitButatabiBumpedLeft();
            break;
        case SPOSE_6A:
            InitButatabiBumpedRight();
            break;
        default:
            gCurrentSprite.pose = SPOSE_IDLE;
            ButatabiHop();
            break;
    }
}

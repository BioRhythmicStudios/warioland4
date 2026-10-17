#include "global_data.h"
#include "screen_shake.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

extern const struct AnimationFrame sPistonChildOam[];
extern const struct AnimationFrame sPistonIdleOam[];
extern const struct AnimationFrame sPistonPrepareOam[];
extern const struct AnimationFrame sPistonPostImpactOam[];
extern const struct AnimationFrame sPistonExtendOam[];
extern const struct AnimationFrame sPistonImpactOam[];
extern const s16 sPistonYMovement[];

void InitPiston(void)
{
    struct PrimarySpriteData* sprite;
    sprite = &gCurrentSprite;
    sprite->status |= 0x400;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = 32;
    sprite->drawDistanceLeftRight = 24;
    sprite->hitboxExtentUp = 128;
    sprite->hitboxExtentDown = 124;
    sprite->hitboxExtentLeft = 80;
    sprite->hitboxExtentRight = 80;
    sprite->warioCollision = 30;
    sprite->work1 = 0;
    SpriteSpawnAsChild(PSPRITE_B9, sprite->roomSlot, sprite->gfxSlot, sprite->yPosition, sprite->xPosition);
}

void PistonSetIdle(void)
{
    struct PrimarySpriteData* sprite;
    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sPistonIdleOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
    if (sprite->globalID == PSPRITE_PISTON)
        sprite->work0 = 75;
    else
        sprite->work0 = 30;
}

void PistonWaitBeforeExtending(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;
    if (gWarioData.reaction == 10)
        return;
    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        sprite->pose = 27;
        sprite->work0 = 30;
        GP_SET(sprite->pOamData, sPistonPrepareOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
    }
}

void PistonStartExtending(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;
    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0)
    {
        GP_SET(sprite->pOamData, sPistonExtendOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->pose = 28;
        sprite->work1 = 0;
        sprite->work3 = 0;
        sprite->warioCollision = 31;
        m4aSongNumStart(SE_PISTON_EXTEND);
    }
}

void PistonExtend(void)
{
    struct PrimarySpriteData* sprite;
    u32 y;
    s16 movement;
    u8 index;
    sprite = &gCurrentSprite;
    y = func_8023A60((u16)(sprite->yPosition + 128), sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1)
        sprite->status |= SPRITE_STATUS_UNDERWATER;
    if (gSpriteCollisionResult != 0)
    {
        sprite->yPosition = y - 128;
        sprite->pose = 23;
        m4aSongNumStart(SE_PISTON_IMPACT);
        if (sprite->work1 != 0)
            gWarioData.yPosition = sprite->yPosition + 116;
        return;
    }
    index = sprite->work3;
    movement = sPistonYMovement[index];
    if (movement == 0x7FFF)
        movement = sPistonYMovement[index - 1];
    else
        sprite->work3 = index + 1;
    sprite->yPosition += movement;
    if (gCurrentSprite.work1 != 0)
        gWarioData.yPosition += movement;
}

void PistonImpact(void)
{
    GP_SET(gCurrentSprite.pOamData, sPistonImpactOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 40;
    gCurrentSprite.warioCollision = 30;
    ScreenShakeRequestY(48, 129);
}

void PistonWaitAfterImpact(void)
{
    u8 timer;
    timer = --gCurrentSprite.work0;
    if (timer == 0)
    {
        gCurrentSprite.pose = 18;
        GP_SET(gCurrentSprite.pOamData, sPistonPostImpactOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
    }
}

void PistonRetract(void)
{
    u8 value;
    value = gRoomSpriteData[gCurrentSprite.roomSlot][0];
    if (gCurrentSprite.yPosition <= (value << 6) + 64)
        gCurrentSprite.pose = 15;
    else
        gCurrentSprite.yPosition -= 8;
}

void InitPistonChild(void)
{
    register struct PrimarySpriteData* sprite PIN("ip");
    register u8 zero PIN("r2");
    u16 zero16;
    u8 pose;
    u8 extent;
    register u8* ptr PIN("r0");
    u8* ptr32;

    sprite = &gCurrentSprite;
    {
        u16 oldStatus;
        register u32 statusBits PIN("r2");
        register u16 status PIN("r0");

        oldStatus = sprite->status;
        statusBits = 0x8400;
        status = statusBits;
        zero = 0;
        zero16 = 0;
        status |= oldStatus;
        status &= 0xFFFB;
        sprite->status = status;
    }

    ptr32 = (u8*)sprite + 32;
    *ptr32 = 48;
    ptr = (u8*)sprite + 33;
    *ptr++ = zero;
    pose = 16;
    *ptr++ = pose;
    extent = 4;
    *ptr++ = extent;
    *ptr++ = extent;
    *ptr++ = extent;
    /* Volatile keeps this as a direct store instead of folding it into indexed addressing. */
    *(volatile u8 *)ptr = extent;

    ptr = (u8*)sprite;
    ptr[30] = zero;
    ptr = (u8*)sPistonChildOam;
    ptr32 = (u8*)sprite;
    GP_SET(((struct PrimarySpriteData*)ptr32)->pOamData, (const struct AnimationFrame*)ptr);
    ((struct PrimarySpriteData*)ptr32)->currentAnimationFrame = zero;
    ((struct PrimarySpriteData*)ptr32)->animationTimer = zero16;
    ((struct PrimarySpriteData*)ptr32)->pose = pose;
}

void SpritePiston(void)
{
    if (gWarioData.reaction == 10)
        gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose)
    {
        case 0: InitPiston();
        case 15: PistonSetIdle();
        case 16: PistonWaitBeforeExtending(); break;
        case 27: PistonStartExtending(); break;
        case 28: PistonExtend(); break;
        case 23: PistonImpact();
        case 24: PistonWaitAfterImpact(); break;
        case 18: PistonRetract(); break;
        default: SpriteUtilDie(); break;
    }
    if (gWarioData.reaction != 10 && (gWarioData.reaction != 0 || gWarioData.damageTimer != 0))
        func_8026838();
}

void SpritePistonChild(void)
{
    u16 zero;
    struct PrimarySpriteData* current;
    struct PrimarySpriteData* currentLoad;
    u8 roomSlot;
    u8* sprite;
    register u8* positionSprite PIN("r3");
    u8* end;

    zero = 0;
    currentLoad = &gCurrentSprite;
    roomSlot = currentLoad->roomSlot;
    current = currentLoad;
    sprite = (u8*)gSpriteData;
    positionSprite = sprite;
    end = sprite + 23 * sizeof(struct PrimarySpriteData);

    do
    {
        if (((struct PrimarySpriteData*)sprite)->roomSlot == roomSlot &&
            (u8)(((struct PrimarySpriteData*)sprite)->globalID - PSPRITE_PISTON) <= 1)
        {
            u16 status;
            u32 active;

            status = ((struct PrimarySpriteData*)sprite)->status;
            active = 1;
            active &= status;
            if (active != 0)
                goto found;
        }

        sprite += sizeof(struct PrimarySpriteData);
        positionSprite += sizeof(struct PrimarySpriteData);
    } while ((s32)sprite <= (s32)end);

    if (zero != 0)
        goto afterCopy;

    current->status = zero;
    return;

found:
    current->yPosition = ((struct PrimarySpriteData*)positionSprite)->yPosition;

afterCopy:
    if (current->pose == 0)
        InitPistonChild();

    current->disableWarioCollisionTimer = 1;
}

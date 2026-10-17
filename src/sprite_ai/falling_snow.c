#include "sprite_ai/falling_snow.h"

#include "global_data.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

extern const struct AnimationFrame sFallingSnowIdleOam[];
extern const struct AnimationFrame sFallingSnowFallingOam[];
extern const struct AnimationFrame sFallingSnowMeltingOam[];
extern const struct AnimationFrame sFallingSnowChildOam[];
extern const struct AnimationFrame sFallingSnowDropOam[];
extern const struct AnimationFrame sFallingSnowResetOam[];
extern const struct AnimationFrame sFallingSnowFragmentRightOam[];
extern const struct AnimationFrame sFallingSnowFragmentLeftOam[];
extern u8 gRoomSpriteData[][3];
extern void SpawnPrimarySpriteWithStatus(u8, u8, u8, u32, u32, u32);

void InitFallingSnow(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    s->status |= 0x400;
    s->drawDistanceDown = 24;
    s->drawDistanceUp = 24;
    s->drawDistanceLeftRight = 24;
    s->hitboxExtentUp = 32;
    s->hitboxExtentDown = 148;
    s->hitboxExtentLeft = 40;
    s->hitboxExtentRight = 36;
    GP_SET(s->pOamData, sFallingSnowIdleOam);
    s->currentAnimationFrame = 0;
    s->animationTimer = 0;
    s->work0 = 30;
    s->pose = 16;
    s->warioCollision = 18;
    s->drawPriority = 1;
}

void FallingSnowWaitBeforeDropping(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    s->disableWarioCollisionTimer = 1;
    if (--s->work0 == 0) {
        GP_SET(s->pOamData, sFallingSnowDropOam);
        s->currentAnimationFrame = 0;
        s->animationTimer = 0;
        s->work0 = 68;
        s->pose = 18;
        SpriteSpawnAsChild(0x9d, s->roomSlot, s->gfxSlot, s->yPosition, s->xPosition);
    }
}

void FallingSnowStartFalling(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    s->disableWarioCollisionTimer = 1;
    if (--s->work0 == 0) {
        GP_SET(s->pOamData, sFallingSnowFallingOam);
        s->currentAnimationFrame = 0;
        s->animationTimer = 0;
        s->pose = 28;
        s->work3 = 0;
        s->yPosition += 12;
    }
}

void FallingSnowFall(void)
{
    int new_var;
    struct PrimarySpriteData* sprite;
    u32 newY;
    u8 index;
    u16 new_var2;
    s16 movement;

    sprite = &gCurrentSprite;
    newY = func_8023A60(sprite->yPosition, (*sprite).xPosition);
    if ((double)(gBackgroundCollisionData.waterType == 1))
        sprite->status |= 0x800;
    if (gSpriteCollisionResult != 0)
    {
        sprite->yPosition = newY;
        sprite->pose = 23;
        return;
    }
    index = sprite->work3;
    movement = sSpriteGravityVelocityTable[index];
    if (movement == 0x7FFF)
    {
        new_var = index - 1;
        new_var2 = ((const u16*)sSpriteGravityVelocityTable)[new_var];
        sprite->yPosition += new_var2;
    }
    else
    {
        sprite->work3 = index + 1;
        sprite->yPosition += movement;
    }
}

void FallingSnowStartMelting(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    GP_SET(s->pOamData, sFallingSnowMeltingOam);
    s->currentAnimationFrame = 0;
    s->animationTimer = 0;
    s->work0 = 17;
    s->pose = 24;
}

void FallingSnowWaitToRespawn(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    s->disableWarioCollisionTimer = 1;
    if (--s->work0 == 0)
        s->pose = 110;
}

void FallingSnowRespawn(void)
{
    u8 (*map)[3];
    struct PrimarySpriteData* sprite;
    u8 roomSlot;
    u16 y;

    map = gRoomSpriteData;
    sprite = &gCurrentSprite;
    roomSlot = sprite->roomSlot;
    y = map[roomSlot][0];
    sprite->yPosition = (y << 6) + 64;
    sprite->pose = 111;
    GP_SET(sprite->pOamData, sFallingSnowIdleOam);
    sprite->work0 = 100;
    sprite->status |= 4;
}

void FallingSnowWaitBeforeDroppingAgain(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    s->disableWarioCollisionTimer = 1;
    s->currentAnimationFrame = 0;
    s->animationTimer = 0;
    if (--s->work0 == 0) {
        s->work0 = 30;
        s->pose = 16;
        s->status &= ~4;
    }
}

void FallingSnowSpawnFragment(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    if (s->xPosition > gWarioData.xPosition)
        SpawnPrimarySpriteWithStatus(0x9e, s->roomSlot, s->gfxSlot, s->yPosition, s->xPosition, 64);
    else
        SpawnPrimarySpriteWithStatus(0x9e, s->roomSlot, s->gfxSlot, s->yPosition, s->xPosition, 0);
}

void FallingSnowResetAfterImpact(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    GP_SET(s->pOamData, sFallingSnowResetOam);
    s->currentAnimationFrame = 0;
    s->animationTimer = 0;
    s->work0 = 12;
    s->pose = 24;
}

void InitFallingSnowChild(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x400;
    sprite->status &= 0xFFFB;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 24;
    sprite->drawDistanceLeftRight = 24;
    sprite->hitboxExtentUp = 1;
    sprite->hitboxExtentDown = 1;
    sprite->hitboxExtentLeft = 1;
    sprite->hitboxExtentRight = 1;
    GP_SET(sprite->pOamData, sFallingSnowChildOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->work0 = 68;
    sprite->pose = 16;
    sprite->warioCollision = 18;
    sprite->drawPriority = 1;
}

void FallingSnowChildExpire(void)
{
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    if (--s->work0 == 0)
        s->status = 0;
}

void InitFallingSnowFragment(void)
{
    register struct PrimarySpriteData* sprite PIN("ip");
    u8 zero;
    u16 zero16;
    register u16 status PIN("r4");
    u8 one;
    u8* ptr;
    register u8* ptr32 PIN("r1");

    sprite = &gCurrentSprite;
    {
        s32 oldStatus;
        u32 statusBits;
        register u16 base PIN("r0");
        u32 mask;

        oldStatus = sprite->status;
        statusBits = 0x400;
        base = statusBits;
        zero = 0;
        ASM_BARRIER_VOLATILE(: "+r"(zero));
        zero16 = 0;
        status = base;
        status |= oldStatus;
        mask = 0xFFFB;
        status &= mask;
        sprite->status = status;
    }

    ptr32 = (u8*)sprite + 32;
    *ptr32 = 24;
    ptr32++;
    *ptr32 = 24;
    ptr32++;
    *(volatile u8 *)ptr32 = 24;

    ptr = (u8*)sprite + 35;
    one = 1;
    *ptr = one;
    ptr++;
    *ptr = one;
    ptr++;
    *ptr = one;
    ptr++;
    *(volatile u8 *)ptr = one;

    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero16;
    {
        u8 collision;
        struct PrimarySpriteData* store;
        collision = 18;
        store = sprite;
        store->warioCollision = collision;
        store->drawPriority = one;
        collision = 28;
        store->pose = collision;
        ptr = (u8*)sprite + 42;
        *ptr = zero;

        if (status & 0x40)
        {
            GP_SET(store->pOamData, sFallingSnowFragmentLeftOam);
            store->status = status & 0xFFBF;
        }
        else
        {
            const struct AnimationFrame* oam;
            register struct PrimarySpriteData* dst PIN("r1");
            oam = sFallingSnowFragmentRightOam;
            dst = sprite;
            GP_SET(dst->pOamData, oam);
        }
    }
}

void SpriteFallingSnow(void)
{
    if (gCurrentSprite.status & 0x800)
        gCurrentSprite.pose = 107;
    switch (gCurrentSprite.pose) {
    case 0: InitFallingSnow(); break;
    case 16: FallingSnowWaitBeforeDropping(); break;
    case 18: FallingSnowStartFalling(); break;
    case 28: FallingSnowFall(); break;
    case 23: FallingSnowStartMelting();
    case 24: FallingSnowWaitToRespawn(); break;
    case 110: FallingSnowRespawn();
    case 111: FallingSnowWaitBeforeDroppingAgain(); break;
    case 49: FallingSnowSpawnFragment();
    default: FallingSnowResetAfterImpact(); break;
    }
}

void SpriteFallingSnowChild(void)
{
    switch (gCurrentSprite.pose) {
    case 0: InitFallingSnowChild(); break;
    case 16: FallingSnowChildExpire(); break;
    }
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void SpriteFallingSnowFragment(void)
{
    switch (gCurrentSprite.pose) {
    case 0: InitFallingSnowFragment(); break;
    case 28: FallingSnowFall(); break;
    default: gCurrentSprite.status = 0; break;
    }
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

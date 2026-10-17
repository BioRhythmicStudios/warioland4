#include "global_data.h"
#include "main.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/ball_lightning.h"
#include "sprite_util.h"

void InitBallLightningSpawner(void)
{
    register struct PrimarySpriteData* sprite PIN("ip");
    u16 oldStatus;
    register u16 status PIN("r0");
    u8 zero;
    u16 zero16;
    u8* ptr;
    u8 one;
    const struct AnimationFrame* oam;
    struct PrimarySpriteData* postSprite;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = 0x40C;
    /* Preserve the target's early zero values in r2 and r3. */
    zero = 0;
    ASM_BARRIER_VOLATILE(: "+r"(zero));
    zero16 = 0;
    status |= oldStatus;
    postSprite = sprite;
    postSprite->status = status;
    ptr = (u8*)sprite + 32;
    one = 1;
    *ptr = one;
    ptr++;
    *ptr = one;
    ptr++;
    *ptr = one;
    ptr++;
    *ptr = one;
    ptr++;
    *ptr = one;
    ptr++;
    *ptr = one;
    /* Volatile keeps the final byte store separate from the pointer increment. */
    ptr++;
    *(volatile u8 *)ptr = one;
    oam = sBallLightningSpawnerOam;
    postSprite = sprite;
    GP_SET(postSprite->pOamData, oam);
    postSprite->currentAnimationFrame = zero;
    postSprite->animationTimer = zero16;
    postSprite->pose = 16;
    postSprite->warioCollision = 6;
    postSprite->work0 = 200;
}

void BallLightningSpawnerIdle(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u8 timer;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    timer = --*timerPtr;
    if (timer == 0)
    {
        SpawnPrimarySpriteWithStatus(PSPRITE_BALL_LIGHTNING_BOLT, sprite->roomSlot, sprite->gfxSlot,
            sprite->yPosition, sprite->xPosition, timer);
        *timerPtr = 200;
        if (sprite->status & 2)
            m4aSongNumStart(SE_BALL_LIGHTNING_SPAWN);
    }
}

void InitBallLightningBolt(void)
{
    struct PrimarySpriteData* sprite;
    u16 oldStatus;
    register u16 status PIN("r0");
    u8 zero;
    u16 zero16;
    u8* ptr;
    u8 value16;
    u8 value20;
    u8* leftPtr;
    const struct AnimationFrame* oam;
    struct PrimarySpriteData* postSprite;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = 0x420;
    /* Preserve the target's early zero values in r3 and r4. */
    zero = 0;
    ASM_BARRIER_VOLATILE(: "+r"(zero));
    zero16 = 0;
    status |= oldStatus;
    status &= 0xFFFB;
    postSprite = sprite;
    postSprite->status = status;
    ptr = (u8*)sprite + 32;
    value16 = 16;
    *ptr = value16;
    ptr++;
    *ptr = value16;
    ptr++;
    *ptr = value16;
    ptr += 2;
    value20 = 20;
    *ptr = value20;
    leftPtr = (u8*)sprite + 37;
    *leftPtr = 24;
    ptr = (u8*)sprite + 38;
    *ptr = value20;
    oam = sBallLightningBoltIdleOam;
    postSprite = sprite;
    GP_SET(postSprite->pOamData, oam);
    postSprite->currentAnimationFrame = zero;
    postSprite->animationTimer = zero16;
    postSprite->warioCollision = 9;
    {
        u8* timerPtr;
        timerPtr = (u8*)sprite + 39;
        *timerPtr = 30;
    }
    ptr = (u8*)sprite;
    ptr[28] = value16;
    SpriteUtilTurnTowardWario();
}

void BallLightningBoltWaitToFall(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = --sprite->work0;
    if (timer == 0)
    {
        sprite->work3 = 0;
        sprite->drawPriority = 1;
        sprite->pose = 18;
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
            GP_SET(sprite->pOamData, sBallLightningBoltFallingRightOam);
        else
            GP_SET(sprite->pOamData, sBallLightningBoltFallingLeftOam);
    }
}

void SetBallLightningBoltRollTimer(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void StartBallLightningBoltRollRight(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    SetBallLightningBoltRollTimer();
}

void StartBallLightningBoltRollLeft(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    SetBallLightningBoltRollTimer();
}

void BallLightningBoltFall(void)
{
    struct PrimarySpriteData* sprite;
    u32 newY;

    sprite = &gCurrentSprite;
    newY = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1)
    {
        u16 oldStatus;
        register u16 status PIN("r0");
        u16 underwater;

        oldStatus = sprite->status;
        underwater = SPRITE_STATUS_UNDERWATER;
        status = underwater;
        status |= oldStatus;
        sprite->status = status;
    }

    if (gSpriteCollisionResult != 0)
    {
        sprite->yPosition = newY;
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
            StartBallLightningBoltRollRight();
        else
            StartBallLightningBoltRollLeft();

        if (gCurrentSprite.status & 2)
            m4aSongNumStart(SE_BALL_LIGHTNING_BOUNCE);
    }
    else
    {
        u8* workPtr;
        u8* savedWorkPtr;
        u32 index;
        const s16* table;
        u32 movement;
        s32 signedMovement;
        register u32 value PIN("r0");

        /* Preserve the target's immediate-first construction of the work pointer. */
        workPtr = (u8 *)42;
        workPtr = (u8 *)((u32)workPtr + (u32)sprite);
        savedWorkPtr = workPtr;
        index = *workPtr;
        table = sSpriteGravityVelocityTable;
        value = index << 1;
        value += (u32)table;
        movement = *(const u16*)value;
        signedMovement = *(const s16*)value;
        if (signedMovement == 0x7FFF)
        {
            s32 previousIndex;
            u32 previousMovement;

            previousIndex = index - 1;
            previousIndex <<= 1;
            previousIndex += (u32)table;
            value = sprite->yPosition;
            previousMovement = *(const u16*)previousIndex;
            value += previousMovement;
        }
        else
        {
            value = index + 1;
            *savedWorkPtr = value;
            value = sprite->yPosition;
            value += movement;
        }
        sprite->yPosition = value;
    }
}

void StartBallLightningBoltDisappearing(void)
{
    GP_SET(gCurrentSprite.pOamData, sBallLightningBoltDisappearingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 50;
    gCurrentSprite.work0 = 19;
    gCurrentSprite.warioCollision = 8;
    gCurrentSprite.status |= 0x400;
}

void BallLightningBoltDisappearing(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u32 oldValue;
    u32 value;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    oldValue = *timerPtr;
    value = oldValue - 1;
    *timerPtr = value;
    oldValue = value << 24;
    oldValue >>= 24;
    if (oldValue != 0)
    {
        oldValue = (u8)value;
        if (oldValue <= 7)
            sprite->disableWarioCollisionTimer = 1;
        return;
    }
    sprite->status = oldValue;
}

void InitBallLightningTrail(void)
{
    register struct PrimarySpriteData* sprite PIN("ip");
    u16 oldStatus;
    register u16 status PIN("r0");
    u8 zero;
    u16 zero16;
    u8* drawPtr;
    register u8 value10 PIN("r0");
    u8* hitboxPtr;
    u8 value4;
    const struct AnimationFrame* oam;
    struct PrimarySpriteData* postSprite;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = 0x400;
    /* Preserve the target's early zero values in r2 and r3. */
    zero = 0;
    ASM_BARRIER_VOLATILE(: "+r"(zero));
    zero16 = 0;
    status |= oldStatus;
    status &= 0xFFFB;
    postSprite = sprite;
    postSprite->status = status;
    drawPtr = (u8*)sprite + 32;
    value10 = 10;
    *drawPtr = value10;
    drawPtr += 1;
    *drawPtr = value10;
    drawPtr++;
    *(volatile u8 *)drawPtr = value10;
    hitboxPtr = (u8*)sprite + 35;
    value4 = 4;
    *hitboxPtr = value4;
    hitboxPtr += 1;
    *hitboxPtr = value4;
    hitboxPtr += 1;
    *hitboxPtr = value4;
    hitboxPtr++;
    *(volatile u8 *)hitboxPtr = value4;
    oam = sBallLightningTrailOam;
    postSprite = sprite;
    GP_SET(postSprite->pOamData, oam);
    postSprite->currentAnimationFrame = zero;
    postSprite->animationTimer = zero16;
    postSprite->warioCollision = 9;
    postSprite = sprite;
    postSprite->work0 = 13;
    {
        u8* work3Ptr;
        work3Ptr = (u8*)sprite + 42;
        *work3Ptr = zero;
    }
    {
        u8 pose;
        register struct PrimarySpriteData* finalSprite PIN("r2");
        pose = 16;
        finalSprite = sprite;
        finalSprite->pose = pose;
        pose = 1;
        finalSprite->drawPriority = pose;
    }
}

void BallLightningTrailMove(void)
{
    struct PrimarySpriteData* initialSprite;
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    register u32 value PIN("r0");

    initialSprite = &gCurrentSprite;
    timerPtr = &initialSprite->work0;
    value = *timerPtr;
    value--;
    *timerPtr = value;
    value = (u8)value;
    sprite = initialSprite;
    if (value != 0)
    {
        u8* workPtr;
        u8* savedWorkPtr;
        u32 index;
        const s16* table;
        register u32 movement PIN("r5");
        s32 signedMovement;

        /* Preserve the target's immediate-first construction of the work pointer. */
        workPtr = (u8 *)42;
        workPtr = (u8 *)((u32)workPtr + (u32)initialSprite);
        savedWorkPtr = workPtr;
        index = *workPtr;
        table = sBallLightningTrailYVelocity;
        value = index << 1;
        value += (u32)table;
        movement = *(const u16*)value;
        signedMovement = *(const s16*)value;
        if (signedMovement == 0x7FFF)
        {
            s32 previousIndex;
            u32 previousMovement;

            previousIndex = index - 1;
            previousIndex <<= 1;
            previousIndex += (u32)table;
            value = initialSprite->yPosition;
            previousMovement = *(const u16*)previousIndex;
            value += previousMovement;
        }
        else
        {
            value = index + 1;
            *savedWorkPtr = value;
            value = initialSprite->yPosition;
            value += movement;
        }
        initialSprite->yPosition = value;

        if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
        {
            value = sprite->xPosition;
            value += 6;
            sprite->xPosition = value;
        }
        else
        {
            value = sprite->xPosition;
            value -= 6;
            sprite->xPosition = value;
        }
    }
    else
    {
        initialSprite->status = value;
    }
}

void SpriteBallLightning(void)
{
    switch (gCurrentSprite.pose)
    {
        case 0:
            InitBallLightningSpawner();
            break;
        case 16:
            BallLightningSpawnerIdle();
            break;
        default:
            break;
    }
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void SpriteBallLightningBolt(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->hitboxExtentUp = 64;
    if (sprite->status & SPRITE_STATUS_UNDERWATER)
        sprite->pose = 107;

    if ((gMainTimer & 31) == 0 && sprite->pose > 18)
    {
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
            SpawnPrimarySpriteWithStatus(PSPRITE_BALL_LIGHTNING_TRAIL, sprite->roomSlot, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition - 16, 0);
        else
            SpawnPrimarySpriteWithStatus(PSPRITE_BALL_LIGHTNING_TRAIL, sprite->roomSlot, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition + 16, 64);
    }

    switch (gCurrentSprite.pose)
    {
        case 0:
            InitBallLightningBolt();
            break;
        case 16:
            BallLightningBoltWaitToFall();
            break;
        case 18:
            BallLightningBoltFall();
            break;
        case 43:
        case 44:
            gCurrentSprite.work2 = 5;
            func_8025D34();
            break;
        case 45:
        case 46:
            gCurrentSprite.work2 = 5;
            func_8025C80();
            break;
        case 51:
            StartBallLightningBoltRollRight();
        case 52:
            func_8024988();
            break;
        case 53:
            StartBallLightningBoltRollLeft();
        case 54:
        case 58:
            func_802476C();
            break;
        case 55:
            func_8024688();
            m4aSongNumStart(SE_BALL_LIGHTNING_BOUNCE);
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
            m4aSongNumStart(SE_BALL_LIGHTNING_BOUNCE);
            goto pose54;
        case 59:
            func_80247F0();
            m4aSongNumStart(SE_BALL_LIGHTNING_BOUNCE);
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
            m4aSongNumStart(SE_BALL_LIGHTNING_BOUNCE);
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
            m4aSongNumStart(SE_BALL_LIGHTNING_BOUNCE);
            goto pose52;
        case 64:
pose52:
            func_8024988();
            break;
        case 65:
            func_8024A0C();
            m4aSongNumStart(SE_BALL_LIGHTNING_BOUNCE);
        case 66:
            func_8024A3C();
            break;
        case 67:
        case 71:
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = 49;
            else
                func_8024AC0();
        case 68:
            func_8024AD4();
            break;
        case 69:
        case 73:
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                func_8024BEC();
            else
                gCurrentSprite.pose = 49;
        case 70:
            func_8024C00();
            break;
        case 107:
            SpriteUtilDie();
            break;
        case 50:
            BallLightningBoltDisappearing();
            break;
        default:
            StartBallLightningBoltDisappearing();
            break;
    }

    gCurrentSprite.hitboxExtentUp = 24;
    return;

pose54:
    func_802476C();
    gCurrentSprite.hitboxExtentUp = 24;
}

void SpriteBallLightningTrail(void)
{
    if (gCurrentSprite.pose == 0)
        InitBallLightningTrail();
    else
        BallLightningTrailMove();
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

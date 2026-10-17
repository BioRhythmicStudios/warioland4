#include "sprite.h"
#include "score.h"
#include "sprite_ai/mayubird.h"
#include "sprite_util.h"

void InitMayubird(void)
{
    struct PrimarySpriteData *sprite;
    u8 *field;
    int zeroFrame;
    u8 horizontalDistance;
    u8 zero;

    sprite = &gCurrentSprite;
    field = &sprite->drawDistanceDown;
    zeroFrame = 0;
    horizontalDistance = 16;
    *field = horizontalDistance;
    {
        u8 *nextField;

        nextField = &sprite->drawDistanceUp;
        *nextField = 8;
        field = &sprite->drawDistanceLeftRight;
        *field = horizontalDistance;
        nextField += 2;
        *nextField = 48;
        nextField++;
        *nextField = 144;
        nextField++;
        *nextField = 36;
        nextField++;
        *nextField = 32;
    }
    {
        const struct AnimationFrame *oam;
        struct PrimarySpriteData *store;

        oam = sMayubirdIdleOam;
        store = sprite;
        GP_SET(store->pOamData, oam);
        store->currentAnimationFrame = zeroFrame;
    }
    zero = 0;
    field = (u8 *)sprite;
    sprite->animationTimer = zeroFrame;
    sprite->pose = horizontalDistance;
    {
        int collision;
        struct PrimarySpriteData *store;

        collision = 46;
        store = sprite;
        store->warioCollision = collision;
    }
    field = &sprite->work2;
    *field = zero;
    field++;
    *field = zero;
}

void MayubirdFloat(void)
{
    struct PrimarySpriteData *base;
    struct PrimarySpriteData *sprite;

    base = &gCurrentSprite;
    {
        register u8 *counterPointer PIN("ip");
        u32 counter;
        const s16 *table;
        const s16 *entry;
        register u16 movement PIN("r3");
        int signedMovement;
        register int zero PIN("r5");

        counterPointer = &base->work2;
        counter = *counterPointer;
        entry = (const s16 *)(counter >> 3);
        table = sMayubirdFloatXVelocity;
        entry = (const s16 *)(((u32)entry << 1) + (u32)table);
        movement = *(const u16 *)entry;
        zero = 0;
        signedMovement = entry[zero];
        {
            int sentinel;

            sentinel = 0x7FFF;
            sprite = base;
            if (signedMovement == sentinel) {
            int next;
            register u8 *store PIN("r6");

            movement = *(const u16 *)table;
            next = 0;
            store = counterPointer;
            *store = next;
        } else {
            int next;
            register u8 *store PIN("r1");

            next = counter + 1;
            store = counterPointer;
            *store = next;
            }
        }
        sprite->xPosition += movement;
    }
    {
        register u8 *counterPointer PIN("ip");
        register u8 *pointerTemp PIN("r6");
        register u32 counter PIN("r2");
        register const s16 *table PIN("r4");
        const s16 *entry;
        u16 movement;
        int signedMovement;
        int zero;

        pointerTemp = (u8 *)42;
        ASM_BARRIER(: "+r"(pointerTemp));
        pointerTemp = (u8 *)((u32)pointerTemp + (u32)sprite);
        counterPointer = pointerTemp;
        counter = *pointerTemp;
        entry = (const s16 *)(counter >> 1);
        table = sMayubirdFloatYVelocity;
        entry = (const s16 *)(((u32)entry << 1) + (u32)table);
        movement = *(const u16 *)entry;
        zero = 0;
        signedMovement = entry[zero];
        if (signedMovement == 0x7FFF) {
            register int next PIN("r0");
            u8 *store;

            movement = *(const u16 *)table;
            next = 0;
            store = counterPointer;
            *store = next;
        } else {
            register int next PIN("r0");
            register u8 *store PIN("r6");

            next = counter + 1;
            store = counterPointer;
            *store = next;
        }
        {
            u16 yPosition;

            yPosition = sprite->yPosition;
            yPosition += movement;
            sprite->yPosition = yPosition;
        }
    }
}

void MayubirdStartWait(void)
{
    struct PrimarySpriteData *sprite;
    int zero;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sMayubirdWaitOam);
    zero = 0;
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->pose = 40;
    sprite->work0 = 90;
}

void MayubirdWait(void)
{
    struct PrimarySpriteData *sprite;
    int timer;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    {
        u8 *timerPointer;

        timerPointer = &sprite->work0;
        timer = *timerPointer;
        timer--;
        *timerPointer = timer;
    }
    {
        int narrowedTimer;

        narrowedTimer = (u8)timer;
        if (narrowedTimer == 0) {
            const struct AnimationFrame *oam;

            oam = sMayubirdIdleOam;
            GP_SET(sprite->pOamData, oam);
            sprite->currentAnimationFrame = narrowedTimer;
            sprite->animationTimer = narrowedTimer;
            sprite->pose = 16;
        }
    }
}

void MayubirdPrepareLongFall(void)
{
    struct PrimarySpriteData *sprite;
    u8 zero;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sMayubirdFallOam);
    sprite->currentAnimationFrame = 0;
    zero = 0;
    sprite->animationTimer = 0;
    sprite->health = zero;
    sprite->work2 = 8;
    {
        u8 *work3Pointer;

        work3Pointer = &sprite->work3;
        *work3Pointer = zero;
    }
    sprite->drawPriority = zero;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    sprite->status = (sprite->status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) | SPRITE_STATUS_MAYBE_DEAD;
}

void MayubirdStartLongFallRight(void)
{
    gCurrentSprite.pose = 32;
    MayubirdPrepareLongFall();
}

void MayubirdStartLongFallLeft(void)
{
    gCurrentSprite.pose = 34;
    MayubirdPrepareLongFall();
}

void MayubirdPrepareShortFall(void)
{
    struct PrimarySpriteData *sprite;
    u32 zero;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sMayubirdFallOam);
    sprite->currentAnimationFrame = 0;
    zero = 0;
    sprite->animationTimer = 0;
    sprite->health = zero;
    sprite->work2 = 2;
    {
        u8 *work3Pointer;

        work3Pointer = &sprite->work3;
        *work3Pointer = zero;
    }
    sprite->drawPriority = zero;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    sprite->status = ((sprite->status | SPRITE_STATUS_MAYBE_DEAD) | zero) & (0xFEFF | (zero ^ zero));
}

void MayubirdStartShortFallRight(void)
{
    gCurrentSprite.pose = 32;
    MayubirdPrepareShortFall();
}

void MayubirdStartShortFallLeft(void)
{
    gCurrentSprite.pose = 34;
    MayubirdPrepareShortFall();
}

void MayubirdPrepareFlight(void)
{
    struct PrimarySpriteData *sprite;
    int zeroFrame;
    int zeroCounter;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sMayubirdFlightOam);
    zeroFrame = 0;
    sprite->currentAnimationFrame = zeroFrame;
    zeroCounter = 0;
    sprite->animationTimer = zeroFrame;
    sprite->work0 = 30;
    sprite->work2 = zeroCounter;
}

void MayubirdStartFlightRight(void)
{
    gCurrentSprite.pose = 36;
    MayubirdPrepareFlight();
}

void MayubirdStartFlightLeft(void)
{
    gCurrentSprite.pose = 38;
    MayubirdPrepareFlight();
}

void MayubirdFlyRight(void)
{
    struct PrimarySpriteData *base;
    struct PrimarySpriteData *sprite;
    u8 *counterPointer;
    const s16 *movementTable;
    u32 counter;
    const s16 *entry;
    u32 movement;
    int signedMovement;
    int zero;

    base = &gCurrentSprite;
    base->disableWarioCollisionTimer = 1;
    counterPointer = &base->work2;
    counter = *counterPointer;
    movementTable = sMayubirdFlyRightXVelocity;
    entry = (const s16 *)(counter << 1);
    entry = (const s16 *)((u32)entry + (u32)movementTable);
    movement = *(const u16 *)entry;
    zero = 0;
    signedMovement = entry[zero];
    {
        int sentinel;

        sentinel = 0x7FFF;
        sprite = base;
        if (signedMovement == sentinel) {
            register u32 previous PIN("r1");
            int xPosition;

            previous = counter - 1;
            previous <<= 1;
            previous += (u32)movementTable;
            xPosition = sprite->xPosition;
            previous = *(const u16 *)previous;
            xPosition += previous;
            sprite->xPosition = xPosition;
        } else {
            int next;
            u8 *store;
            int xPosition;

            next = counter + 1;
            store = counterPointer;
            *store = next;
            xPosition = sprite->xPosition;
            xPosition += movement;
            sprite->xPosition = xPosition;
        }
    }
    {
        int rawTimer;
        u8 *timerPointer;

        timerPointer = &sprite->work0;
        rawTimer = *timerPointer;
        if (rawTimer != 0) {
            rawTimer--;
            *timerPointer = rawTimer;
            {
                int timer;

                timer = (u8)rawTimer;
                if (timer != 0) {
                    sprite->yPosition -= 2;
                } else {
                    const struct AnimationFrame *oam;

                    oam = sMayubirdFlightEndOam;
                    GP_SET(sprite->pOamData, oam);
                    sprite->currentAnimationFrame = timer;
                    sprite->animationTimer = timer;
                }
            }
        } else {
            sprite->yPosition -= 12;
        }
    }
}

void MayubirdFlyLeft(void)
{
    struct PrimarySpriteData *base;
    struct PrimarySpriteData *sprite;
    u8 *counterPointer;
    const s16 *movementTable;
    u32 counter;
    const s16 *entry;
    u32 movement;
    int signedMovement;
    int zero;

    base = &gCurrentSprite;
    base->disableWarioCollisionTimer = 1;
    counterPointer = &base->work2;
    counter = *counterPointer;
    movementTable = sMayubirdFlyLeftXVelocity;
    entry = (const s16 *)(counter << 1);
    entry = (const s16 *)((u32)entry + (u32)movementTable);
    movement = *(const u16 *)entry;
    zero = 0;
    signedMovement = entry[zero];
    {
        int sentinel;

        sentinel = 0x7FFF;
        sprite = base;
        if (signedMovement == sentinel) {
            register u32 previous PIN("r1");
            int xPosition;

            previous = counter - 1;
            previous <<= 1;
            previous += (u32)movementTable;
            xPosition = sprite->xPosition;
            previous = *(const u16 *)previous;
            xPosition += previous;
            sprite->xPosition = xPosition;
        } else {
            int next;
            u8 *store;
            int xPosition;

            next = counter + 1;
            store = counterPointer;
            *store = next;
            xPosition = sprite->xPosition;
            xPosition += movement;
            sprite->xPosition = xPosition;
        }
    }
    {
        int rawTimer;
        u8 *timerPointer;

        timerPointer = &sprite->work0;
        rawTimer = *timerPointer;
        if (rawTimer != 0) {
            rawTimer--;
            *timerPointer = rawTimer;
            {
                int timer;

                timer = (u8)rawTimer;
                if (timer != 0) {
                    sprite->yPosition -= 2;
                } else {
                    const struct AnimationFrame *oam;

                    oam = sMayubirdFlightEndOam;
                    GP_SET(sprite->pOamData, oam);
                    sprite->currentAnimationFrame = timer;
                    sprite->animationTimer = timer;
                }
            }
        } else {
            sprite->yPosition -= 12;
        }
    }
}

void MayubirdStartDeathDelay(void)
{
    struct PrimarySpriteData *sprite;
    int zero;

    sprite = &gCurrentSprite;
    zero = 0;
    sprite->pose = 50;
    GP_SET(sprite->pOamData, sMayubirdFallOam);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->work0 = 10;
    sprite->status = (sprite->status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) | SPRITE_STATUS_MAYBE_DEAD;
}

void MayubirdDie(void)
{
    SpriteUtilDie();
}

void SpriteMayubird(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitMayubird();
        case 16:
            MayubirdFloat();
            break;
        case 31:
        case 79:
        case 106:
            MayubirdStartLongFallRight();
            goto fallRight;
        case 33:
        case 80:
        case 105:
            MayubirdStartLongFallLeft();
            goto fallLeft;
        case 51:
            MayubirdStartShortFallRight();
        case 32:
        case 52:
        fallRight:
            SpriteUtilFallOffscreenRight();
            break;
        case 53:
            MayubirdStartShortFallLeft();
        case 34:
        case 54:
        fallLeft:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
            MayubirdStartFlightRight();
        case 36:
            MayubirdFlyRight();
            break;
        case 37:
            MayubirdStartFlightLeft();
        case 38:
            MayubirdFlyLeft();
            break;
        case 39:
        case 41:
            MayubirdStartWait();
        case 40:
            MayubirdWait();
            break;
        case 49:
            MayubirdStartDeathDelay();
        case 50:
            SpriteUtilDieAfterDelay();
            break;
        default:
            MayubirdDie();
            break;
    }
}

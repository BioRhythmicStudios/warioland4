#include "global_data.h"
#include "gba/m4a.h"
#include "oam.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/dominoes.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

extern const struct AnimationFrame sDominoTile0Animation0Oam[];
extern const struct AnimationFrame sDominoTile0Animation1Oam[];
extern const struct AnimationFrame sDominoTile0Animation2Oam[];
extern const struct AnimationFrame sDominoTile0Animation3Oam[];
extern const struct AnimationFrame sDominoTile0Animation4Oam[];
extern const struct AnimationFrame sDominoTile0Animation5Oam[];
extern const struct AnimationFrame sDominoTile0Animation6Oam[];
extern const struct AnimationFrame sDominoTile0Animation7Oam[];
extern const struct AnimationFrame sDominoTile1Animation0Oam[];
extern const struct AnimationFrame sDominoTile1Animation1Oam[];
extern const struct AnimationFrame sDominoTile1Animation2Oam[];
extern const struct AnimationFrame sDominoTile1Animation3Oam[];
extern const struct AnimationFrame sDominoTile1Animation4Oam[];
extern const struct AnimationFrame sDominoTile1Animation5Oam[];
extern const struct AnimationFrame sDominoTile1Animation6Oam[];
extern const struct AnimationFrame sDominoTile1Animation7Oam[];
extern const struct AnimationFrame sDominoTile2Animation0Oam[];
extern const struct AnimationFrame sDominoTile2Animation1Oam[];
extern const struct AnimationFrame sDominoTile2Animation2Oam[];
extern const struct AnimationFrame sDominoTile2Animation3Oam[];
extern const struct AnimationFrame sDominoTile2Animation4Oam[];
extern const struct AnimationFrame sDominoTile2Animation5Oam[];
extern const struct AnimationFrame sDominoTile2Animation6Oam[];
extern const struct AnimationFrame sDominoTile2Animation7Oam[];
extern const struct AnimationFrame sDominoTile2Animation8Oam[];
extern const struct AnimationFrame sDominoTile2Animation9Oam[];
extern const struct AnimationFrame sDominoTile3Animation0Oam[];
extern const struct AnimationFrame sDominoTile3Animation1Oam[];
extern const struct AnimationFrame sDominoTile3Animation2Oam[];
extern const struct AnimationFrame sDominoTile3Animation3Oam[];
extern const struct AnimationFrame sDominoTile3Animation4Oam[];
extern const struct AnimationFrame sDominoTile3Animation5Oam[];
extern const struct AnimationFrame sDominoTile3Animation6Oam[];
extern const struct AnimationFrame sDominoTile3Animation7Oam[];
extern const struct AnimationFrame sDominoTile3Animation8Oam[];
extern const struct AnimationFrame sDominoTile3Animation9Oam[];
extern const struct AnimationFrame sDominoTile4Animation0Oam[];
extern const struct AnimationFrame sDominoTile4Animation1Oam[];
extern const struct AnimationFrame sDominoTile4Animation2Oam[];
extern const struct AnimationFrame sDominoTile4Animation3Oam[];
extern const struct AnimationFrame sDominoTile4Animation4Oam[];
extern const struct AnimationFrame sDominoTile4Animation5Oam[];
extern const struct AnimationFrame sDominoTile4Animation6Oam[];
extern const struct AnimationFrame sDominoTile4Animation7Oam[];
extern const struct AnimationFrame sDominoTile4Animation8Oam[];
extern const struct AnimationFrame sDominoTile5Animation0Oam[];
extern const struct AnimationFrame sDominoTile5Animation1Oam[];
extern const struct AnimationFrame sDominoTile5Animation2Oam[];
extern const struct AnimationFrame sDominoTile5Animation3Oam[];
extern const struct AnimationFrame sDominoTile5Animation4Oam[];
extern const struct AnimationFrame sDominoTile5Animation5Oam[];
extern const struct AnimationFrame sDominoTile5Animation6Oam[];
extern const struct AnimationFrame sDominoTile5Animation7Oam[];
extern const struct AnimationFrame sDominoTile5Animation8Oam[];
extern const struct AnimationFrame sDominoTile6Animation0Oam[];
extern const struct AnimationFrame sDominoTile6Animation1Oam[];
extern const struct AnimationFrame sDominoTile6Animation2Oam[];
extern const struct AnimationFrame sDominoTile6Animation3Oam[];
extern const struct AnimationFrame sDominoTile6Animation4Oam[];
extern const struct AnimationFrame sDominoTile6Animation5Oam[];
extern const struct AnimationFrame sDominoTile6Animation6Oam[];
extern const struct AnimationFrame sDominoTile6Animation7Oam[];
extern const struct AnimationFrame sDominoTile6Animation8Oam[];
extern const struct AnimationFrame sDominoTile6Animation9Oam[];
extern const struct AnimationFrame sDominoTile7Animation0Oam[];
extern const struct AnimationFrame sDominoTile7Animation1Oam[];
extern const struct AnimationFrame sDominoTile7Animation2Oam[];
extern const struct AnimationFrame sDominoTile7Animation3Oam[];
extern const struct AnimationFrame sDominoTile7Animation4Oam[];
extern const struct AnimationFrame sDominoTile7Animation5Oam[];
extern const struct AnimationFrame sDominoTile7Animation6Oam[];
extern const struct AnimationFrame sDominoTile7Animation7Oam[];
extern const struct AnimationFrame sDominoTile7Animation8Oam[];
extern const struct AnimationFrame sDominoTile7Animation9Oam[];
extern const struct AnimationFrame sDominoTile8Animation0Oam[];
extern const struct AnimationFrame sDominoTile8Animation1Oam[];
extern const struct AnimationFrame sDominoTile8Animation2Oam[];
extern const struct AnimationFrame sDominoTile8Animation3Oam[];
extern const struct AnimationFrame sDominoTile8Animation4Oam[];
extern const struct AnimationFrame sDominoTile8Animation5Oam[];
extern const struct AnimationFrame sDominoTile8Animation6Oam[];
extern const struct AnimationFrame sDominoTile8Animation7Oam[];
extern const struct AnimationFrame sDominoTile8Animation8Oam[];
extern const struct AnimationFrame sDominoTile8Animation9Oam[];
extern const struct AnimationFrame sDominoTile9Animation0Oam[];
extern const struct AnimationFrame sDominoTile9Animation1Oam[];
extern const struct AnimationFrame sDominoTile9Animation2Oam[];
extern const struct AnimationFrame sDominoTile9Animation3Oam[];
extern const struct AnimationFrame sDominoTile9Animation4Oam[];
extern const struct AnimationFrame sDominoTile9Animation5Oam[];
extern const struct AnimationFrame sDominoTile9Animation6Oam[];
extern const struct AnimationFrame sDominoTile9Animation7Oam[];
extern const struct AnimationFrame sDominoTile9Animation8Oam[];
extern const struct AnimationFrame sDominoTile9Animation9Oam[];
extern const struct AnimationFrame sDominoGoalOpeningAnimationOam[];
extern const struct AnimationFrame sDominoGoalOpenLeftAnimationOam[];
extern const struct AnimationFrame sDominoGoalOpenRightAnimationOam[];
extern const struct AnimationFrame sDominoGoalClosedAnimationOam[];
extern const struct AnimationFrame sDominoLongPieceAAnimationOam[];
extern const struct AnimationFrame sDominoLongPieceBAnimationOam[];
extern const struct AnimationFrame sDominoActivatorAnimationOam[];

extern int GetBg2CollisionTypeAtTile(int y, int x);
extern void SpawnScoreCoinDropSprites(int y, int x, int type);

void UpdateDominoPalette(void)
{
    if (gSwitchStates[1] != 0)
        gCurrentSprite.palette = 0;
    else
        gCurrentSprite.palette = 1;
}

void StopDominoActivator(void)
{
    int i;

    i = 0;
    while (i <= 23) {
        struct PrimarySpriteData *sprite;

        sprite = &gSpriteData[i];
        if (sprite->globalID == PSPRITE_DOMINOES_2D && (sprite->status & 1) != 0) {
            sprite->status = 0;
            gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = 2;
            m4aMPlayTempoControl(GP(gMPlayTable[0].info), 0x100);
            m4aMPlayTempoControl(GP(gMPlayTable[1].info), 0x100);
            break;
        }
        i++;
    }
}

void WakeDominoGoals(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *end;
    int one;
    int pose;

    sprite = gSpriteData;
    end = (struct PrimarySpriteData *)((u32)sprite + (253 << 2));
    one = 1;
    pose = 49;
    do {
        if (sprite->globalID == 59) {
            register u16 status PIN("r1");
            register int exists PIN("r0");

            status = sprite->status;
            exists = one;
            exists &= status;
            if (exists != 0)
                sprite->pose = pose;
        }
        sprite++;
    } while ((s32)sprite <= (s32)end);
}

void AdvanceDominoAtTile(int yArg, int xArg)
{
    int y;
    int x;
    int found;
    int i;
    register u8 *data PIN("r1");
    register u8 *address PIN("r0");
    u8 *persistentBase;
    int value;
    int offset;

    y = yArg;
    x = xArg;
    found = 0;
    i = 0;
    address = gRoomSpriteData;
    ASM_BARRIER(: "+r"(address));
    data = address;
    if (y != data[0] || x != data[1]) {
        while (1) {
            i++;
            if (i > 63)
                break;
            offset = (i << 1) + i;
            address = (u8 *)offset;
            address += (u32)data;
            if (y != *address)
                continue;
            address = data + 1;
            address = (u8 *)offset + (u32)address;
            if (x != *address)
                continue;
            found++;
            break;
        }
        if (found == 0)
            return;
    }

    persistentBase = &gPersistentSpriteData[0][0];
    address = (u8 *)(gCurrentRoom << 6);
    address = (u8 *)i + (u32)address;
    address += (u32)persistentBase;
    value = *address;
    value += 16;
    *address = value;
}

void InitDominoActivator(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x8404;
    sprite->drawDistanceDown = 8;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 8;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    GP_SET(sprite->pOamData, sDominoActivatorAnimationOam);
    sprite->drawPriority = 1;
    sprite->pose = 16;
}

void WaitForDominoActivator(void)
{
    int warioY;
    struct PrimarySpriteData *sprite;
    int warioX;
    int spriteY;
    int spriteX;

    warioY = gWarioData.yPosition;
    warioX = gWarioData.xPosition;
    sprite = &gCurrentSprite;
    spriteY = sprite->yPosition;
    spriteX = sprite->xPosition;
    if (spriteX - 255 < warioX &&
        spriteX + 255 > warioX &&
        spriteY - 255 < warioY &&
        spriteY + 255 > warioY) {
        gSwitchStates[1] = 2;
        sprite->yPosition += 32;
        sprite->pose = 18;
        sprite->work0 = 0;
        sprite->work1 = 255;
        sprite->work2 = 0;
        sprite->work3 = 0;
        m4aSongNumStart(SE_DOMINO_ACTIVATOR_START);
        VoiceSetPlay(3);
        AdvanceDominoAtTile(sprite->yPosition >> 6, sprite->xPosition >> 6);
    }
}

void UpdateDominoActivatorTrail(void)
{
    int y;
    int x;
    u8 *counter;
    u32 value;
    int collision;

    if (gCurrentSprite.work1 == 255)
        return;

    y = gCurrentSprite.work2;
    x = gCurrentSprite.work3;
    collision = GetBg2CollisionTypeAtTile(y, x);
    switch (collision) {
    case 9:
    case 10:
        value = (u32)&gCurrentSprite;
        counter = (u8 *)value;
        counter += 40;
        value = *counter;
        if (value > 59)
            break;
        goto increment10;
    case 11:
    case 12:
        value = (u32)&gCurrentSprite;
        counter = (u8 *)value;
        counter += 40;
        value = *counter;
        if (value > 79)
            break;
        goto increment10;
    case 13:
    case 14:
        value = (u32)&gCurrentSprite;
        counter = (u8 *)value;
        counter += 40;
        value = *counter;
        if (value > 69)
            break;
increment10:
        value++;
        *counter = value;
        value = *counter;
        if ((u8)(value % 10) == 0)
            AdvanceDominoAtTile(y, x);
        break;
    case 15:
    case 16:
    case 17:
    case 18:
        value = (u32)&gCurrentSprite;
        counter = (u8 *)value;
        counter += 40;
        value = *counter;
        if (value > 64)
            break;
        value++;
        *counter = value;
        value = *counter;
        if ((u8)(value % 5) == 0)
            AdvanceDominoAtTile(y, x);
        break;
    }
}


void UpdateDominoChain(void)
{
    u32 yTile;
    u32 xTile;
    register u32 coordinate PIN("r0");
    struct PrimarySpriteData *current;

    m4aMPlayTempoControl(GP(gMPlayTable[0].info), 400);
    m4aMPlayTempoControl(GP(gMPlayTable[1].info), 400);
    current = &gCurrentSprite;
    coordinate = current->yPosition;
    yTile = coordinate >> 6;
    coordinate = current->xPosition;
    xTile = coordinate >> 6;

    switch (GetBg2CollisionTypeAtTile(yTile, xTile)) {
    case 9: {
        struct PrimarySpriteData *sprite;
        u8 *timerPtr;
        register u32 timer PIN("r4");
        u32 remainder;
        u32 value;
        u8 *storePtr;
        int collision;

        sprite = &gCurrentSprite;
        timerPtr = &sprite->work0;
        value = *timerPtr;
        if (value > 70)
            break;
        value++;
        storePtr = timerPtr;
        *storePtr = value;
        timer = *storePtr;
        remainder = (u8)(timer % 10);
        if (remainder != 0)
            break;
        m4aSongNumStart(SE_DOMINO_TUMBLE);
        if ((u8)(timer / 10) != 4) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register u32 previous PIN("r0");
            u8 *work1Ptr;
            u32 storedRemainder;
            u8 *timerStore;

            previous = timer - 1;
            work1Ptr = &sprite->work1;
            *work1Ptr = previous;
            storedRemainder = remainder;
            timerStore = timerPtr;
            *timerStore = storedRemainder;
        }
        sprite->work2 = yTile;
        sprite->work3 = xTile;
        collision = GetBg2CollisionTypeAtTile(yTile, xTile + 4);
        if (collision == 19)
            sprite->xPosition += 192;
        else
            sprite->xPosition += 256;
        {
            struct PrimarySpriteData *callSprite;
            u32 callY;
            u32 callX;

            callSprite = &gCurrentSprite;
            callY = callSprite->yPosition >> 6;
            callX = callSprite->xPosition;
            AdvanceDominoAtTile(callY, callX >> 6);
        }
        break;
    }
    case 10: {
        struct PrimarySpriteData *sprite;
        u8 *timerPtr;
        register u32 timer PIN("r4");
        u32 remainder;
        u32 value;
        u8 *storePtr;
        int collision;

        sprite = &gCurrentSprite;
        timerPtr = &sprite->work0;
        value = *timerPtr;
        if (value > 70)
            break;
        value++;
        storePtr = timerPtr;
        *storePtr = value;
        timer = *storePtr;
        remainder = (u8)(timer % 10);
        if (remainder != 0)
            break;
        m4aSongNumStart(SE_DOMINO_TUMBLE);
        if ((u8)(timer / 10) != 4) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register u32 previous PIN("r1");
            u8 *work1Ptr;
            u32 storedRemainder;
            u8 *timerStore;

            previous = timer - 1;
            work1Ptr = &sprite->work1;
            *work1Ptr = previous;
            storedRemainder = remainder;
            timerStore = timerPtr;
            *timerStore = storedRemainder;
        }
        sprite->work2 = yTile;
        sprite->work3 = xTile;
        collision = GetBg2CollisionTypeAtTile(yTile, xTile - 4);
        if (collision == 19)
            sprite->xPosition -= 192;
        else
            sprite->xPosition -= 256;
        {
            struct PrimarySpriteData *callSprite;
            u32 callY;
            u32 callX;

            callSprite = &gCurrentSprite;
            callY = callSprite->yPosition >> 6;
            callX = callSprite->xPosition;
            AdvanceDominoAtTile(callY, callX >> 6);
        }
        break;
    }
    case 11: {
        struct PrimarySpriteData *sprite;
        u8 *timerPtr;
        register u32 timer PIN("r4");
        u32 remainder;
        u32 value;
        u8 *storePtr;

        sprite = &gCurrentSprite;
        timerPtr = &sprite->work0;
        value = *timerPtr;
        if (value > 90)
            break;
        value++;
        storePtr = timerPtr;
        *storePtr = value;
        timer = *storePtr;
        remainder = (u8)(timer % 10);
        if (remainder != 0)
            break;
        m4aSongNumStart(SE_DOMINO_TUMBLE);
        if ((u8)(timer / 10) != 6) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register u32 previous PIN("r1");
            u8 *work1Ptr;
            u32 storedRemainder;
            u8 *timerStore;

            previous = timer - 1;
            work1Ptr = &sprite->work1;
            *work1Ptr = previous;
            storedRemainder = remainder;
            timerStore = timerPtr;
            *timerStore = storedRemainder;
        }
        sprite->work2 = yTile;
        sprite->work3 = xTile;
        sprite->xPosition += 320;
        sprite->yPosition += 64;
        {
            u32 callY;
            u32 callX;

            ASM_BARRIER_VOLATILE(: "+r"(sprite));
            callY = sprite->yPosition >> 6;
            callX = sprite->xPosition;
            AdvanceDominoAtTile(callY, callX >> 6);
        }
        break;
    }
    case 12: {
        struct PrimarySpriteData *sprite;
        u8 *timerPtr;
        register u32 timer PIN("r4");
        u32 remainder;
        u32 value;
        u8 *storePtr;

        sprite = &gCurrentSprite;
        timerPtr = &sprite->work0;
        value = *timerPtr;
        if (value > 90)
            break;
        value++;
        storePtr = timerPtr;
        *storePtr = value;
        timer = *storePtr;
        remainder = (u8)(timer % 10);
        if (remainder != 0)
            break;
        m4aSongNumStart(SE_DOMINO_TUMBLE);
        if ((u8)(timer / 10) != 6) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register u32 previous PIN("r1");
            u8 *work1Ptr;
            u32 storedRemainder;
            u8 *timerStore;

            previous = timer - 1;
            work1Ptr = &sprite->work1;
            *work1Ptr = previous;
            storedRemainder = remainder;
            timerStore = timerPtr;
            *timerStore = storedRemainder;
        }
        sprite->work2 = yTile;
        sprite->work3 = xTile;
        sprite->xPosition -= 320;
        sprite->yPosition += 64;
        {
            u32 callY;
            u32 callX;

            ASM_BARRIER_VOLATILE(: "+r"(sprite));
            callY = sprite->yPosition >> 6;
            callX = sprite->xPosition;
            AdvanceDominoAtTile(callY, callX >> 6);
        }
        break;
    }
    case 13: {
        struct PrimarySpriteData *sprite;
        u8 *timerPtr;
        register u32 timer PIN("r4");
        u32 remainder;
        u32 value;
        u8 *storePtr;

        sprite = &gCurrentSprite;
        timerPtr = &sprite->work0;
        value = *timerPtr;
        if (value > 80)
            break;
        value++;
        storePtr = timerPtr;
        *storePtr = value;
        timer = *storePtr;
        remainder = (u8)(timer % 10);
        if (remainder != 0)
            break;
        m4aSongNumStart(SE_DOMINO_TUMBLE);
        if ((u8)(timer / 10) != 5) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register u32 previous PIN("r1");
            u8 *work1Ptr;
            u32 storedRemainder;
            u8 *timerStore;

            previous = timer - 1;
            work1Ptr = &sprite->work1;
            *work1Ptr = previous;
            storedRemainder = remainder;
            timerStore = timerPtr;
            *timerStore = storedRemainder;
        }
        sprite->work2 = yTile;
        sprite->work3 = xTile;
        sprite->xPosition += 320;
        sprite->yPosition -= 64;
        {
            u32 callY;
            u32 callX;

            ASM_BARRIER_VOLATILE(: "+r"(sprite));
            callY = sprite->yPosition >> 6;
            callX = sprite->xPosition;
            AdvanceDominoAtTile(callY, callX >> 6);
        }
        break;
    }
    case 14: {
        struct PrimarySpriteData *sprite;
        u8 *timerPtr;
        register u32 timer PIN("r4");
        u32 remainder;
        u32 value;
        u8 *storePtr;

        sprite = &gCurrentSprite;
        timerPtr = &sprite->work0;
        value = *timerPtr;
        if (value > 80)
            break;
        value++;
        storePtr = timerPtr;
        *storePtr = value;
        timer = *storePtr;
        remainder = (u8)(timer % 10);
        if (remainder != 0)
            break;
        m4aSongNumStart(SE_DOMINO_TUMBLE);
        if ((u8)(timer / 10) != 5) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register u32 previous PIN("r1");
            u8 *work1Ptr;
            u32 storedRemainder;
            u8 *timerStore;

            previous = timer - 1;
            work1Ptr = &sprite->work1;
            *work1Ptr = previous;
            storedRemainder = remainder;
            timerStore = timerPtr;
            *timerStore = storedRemainder;
        }
        sprite->work2 = yTile;
        sprite->work3 = xTile;
        sprite->xPosition -= 320;
        sprite->yPosition -= 64;
        {
            u32 callY;
            u32 callX;

            ASM_BARRIER_VOLATILE(: "+r"(sprite));
            callY = sprite->yPosition >> 6;
            callX = sprite->xPosition;
            AdvanceDominoAtTile(callY, callX >> 6);
        }
        break;
    }
    case 15: {
        u32 timer;
        u32 value;
        register struct PrimarySpriteData *base PIN("r0");
        register u8 *timerPtr PIN("r1");

        base = &gCurrentSprite;
        ASM_BARRIER_VOLATILE(: "+r"(base));
        timerPtr = (u8 *)base;
        timerPtr += 39;
        value = *timerPtr;
        if (value > 70)
            break;
        value++;
        *timerPtr = value;
        timer = *timerPtr;
        if ((u8)(timer % 5) != 0)
            break;
        if ((u8)(timer % 10) == 0)
            m4aSongNumStart(SE_DOMINO_TUMBLE);
        else if ((u8)(timer / 5) == 11)
            m4aSongNumStart(SE_DOMINO_TUMBLE_ALT);
        if ((u8)(timer / 5) != 13) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register struct PrimarySpriteData *sprite PIN("r3");
            u32 previous;
            u8 *workPtr;
            u32 zero;
            u32 position;

            sprite = &gCurrentSprite;
            previous = timer - 1;
            workPtr = (u8 *)sprite;
            workPtr += 40;
            zero = 0;
            *workPtr = previous;
            workPtr--;
            *workPtr = zero;
            workPtr += 2;
            *workPtr = yTile;
            workPtr++;
            *(volatile u8 *)workPtr = xTile;
            position = sprite->xPosition;
            position -= 64;
            sprite->xPosition = position;
            position = sprite->yPosition;
            position += 192;
            *(volatile u16 *)&sprite->yPosition = position;
            AdvanceDominoAtTile(sprite->yPosition >> 6, sprite->xPosition >> 6);
        }
        break;
    }
    case 16: {
        u32 timer;
        u32 value;
        register struct PrimarySpriteData *base PIN("r0");
        register u8 *timerPtr PIN("r1");

        base = &gCurrentSprite;
        ASM_BARRIER_VOLATILE(: "+r"(base));
        timerPtr = (u8 *)base;
        timerPtr += 39;
        value = *timerPtr;
        if (value > 70)
            break;
        value++;
        *timerPtr = value;
        timer = *timerPtr;
        if ((u8)(timer % 5) != 0)
            break;
        if ((u8)(timer % 10) == 0)
            m4aSongNumStart(SE_DOMINO_TUMBLE);
        else if ((u8)(timer / 5) == 11)
            m4aSongNumStart(SE_DOMINO_TUMBLE_ALT);
        if ((u8)(timer / 5) != 13) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register struct PrimarySpriteData *sprite PIN("r3");
            u32 previous;
            u8 *workPtr;
            u32 zero;
            u32 position;

            sprite = &gCurrentSprite;
            previous = timer - 1;
            workPtr = (u8 *)sprite;
            workPtr += 40;
            zero = 0;
            *workPtr = previous;
            workPtr--;
            *workPtr = zero;
            workPtr += 2;
            *workPtr = yTile;
            workPtr++;
            *(volatile u8 *)workPtr = xTile;
            position = sprite->xPosition;
            position += 64;
            sprite->xPosition = position;
            position = sprite->yPosition;
            position += 192;
            *(volatile u16 *)&sprite->yPosition = position;
            AdvanceDominoAtTile(sprite->yPosition >> 6, sprite->xPosition >> 6);
        }
        break;
    }
    case 17: {
        u32 timer;
        u32 value;
        register struct PrimarySpriteData *base PIN("r0");
        register u8 *timerPtr PIN("r1");

        base = &gCurrentSprite;
        ASM_BARRIER_VOLATILE(: "+r"(base));
        timerPtr = (u8 *)base;
        timerPtr += 39;
        value = *timerPtr;
        if (value > 70)
            break;
        value++;
        *timerPtr = value;
        timer = *timerPtr;
        if ((u8)(timer % 5) != 0)
            break;
        if ((u8)(timer % 10) == 0)
            m4aSongNumStart(SE_DOMINO_TUMBLE);
        else if ((u8)(timer / 5) == 11)
            m4aSongNumStart(SE_DOMINO_TUMBLE_ALT);
        if ((u8)(timer / 5) != 13) {
            AdvanceDominoAtTile(yTile, xTile);
            break;
        }
        {
            register struct PrimarySpriteData *sprite PIN("r3");
            u32 previous;
            u8 *workPtr;
            u32 zero;
            u32 position;

            sprite = &gCurrentSprite;
            previous = timer - 1;
            workPtr = (u8 *)sprite;
            workPtr += 40;
            zero = 0;
            *workPtr = previous;
            workPtr--;
            *workPtr = zero;
            workPtr += 2;
            *workPtr = yTile;
            workPtr++;
            *(volatile u8 *)workPtr = xTile;
            position = sprite->xPosition;
            position -= 64;
            sprite->xPosition = position;
            position = sprite->yPosition;
            position -= 192;
            *(volatile u16 *)&sprite->yPosition = position;
            AdvanceDominoAtTile(sprite->yPosition >> 6, sprite->xPosition >> 6);
        }
        break;
    }
    case 18: {
        u32 timer;
        u32 value;
        register struct PrimarySpriteData *base PIN("r0");
        register u8 *timerPtr PIN("r1");

        base = &gCurrentSprite;
        ASM_BARRIER_VOLATILE(: "+r"(base));
        timerPtr = (u8 *)base;
        timerPtr += 39;
        value = *timerPtr;
        if (value > 70)
            break;
        value++;
        *timerPtr = value;
        timer = *timerPtr;
        if ((u8)(timer % 5) != 0)
            break;
        if ((u8)(timer % 10) == 0)
            m4aSongNumStart(SE_DOMINO_TUMBLE);
        else if ((u8)(timer / 5) == 11)
            m4aSongNumStart(SE_DOMINO_TUMBLE_ALT);
        if ((u8)(timer / 5) != 13)
            goto callOriginalPosition;
        {
            register struct PrimarySpriteData *sprite PIN("r3");
            u32 previous;
            u8 *workPtr;
            u32 zero;
            u32 position;

            sprite = &gCurrentSprite;
            previous = timer - 1;
            workPtr = (u8 *)sprite;
            workPtr += 40;
            zero = 0;
            *workPtr = previous;
            workPtr--;
            *workPtr = zero;
            workPtr += 2;
            *workPtr = yTile;
            workPtr++;
            *(volatile u8 *)workPtr = xTile;
            position = sprite->xPosition;
            position += 64;
            sprite->xPosition = position;
            position = sprite->yPosition;
            position -= 192;
            *(volatile u16 *)&sprite->yPosition = position;
            AdvanceDominoAtTile(sprite->yPosition >> 6, sprite->xPosition >> 6);
        }
        break;
    }
callOriginalPosition:
    AdvanceDominoAtTile(yTile, xTile);
    break;
    default:
        current->pose = 109;
        break;
    }

    UpdateDominoActivatorTrail();
}



void FinishDominoChain(void)
{
    struct PrimarySpriteData *current;
    register int y PIN("r6");
    register int x PIN("r4");
    int found;
    int index;
    register int coordinate PIN("r0");

    current = &gCurrentSprite;
    if (current->work1 > 59) {
        current->status = 0;
        gPersistentSpriteData[gCurrentRoom][current->roomSlot] = 2;
        m4aMPlayTempoControl(GP(gMPlayTable[0].info), 0x100);
        m4aMPlayTempoControl(GP(gMPlayTable[1].info), 0x100);

        coordinate = current->yPosition;
        y = coordinate >> 6;
        coordinate = current->xPosition;
        x = coordinate >> 6;
        found = 0;
        index = 0;
        {
            register u8 *data PIN("r0");
            register u8 *base PIN("r3");
            int shift;
            int offset;

            data = gRoomSpriteData;
            if (y != data[0] || x != data[1]) {
searchRoomEntry:
                index++;
                if (index > 63)
                    goto roomSearchDone;
                base = gRoomSpriteData;
                shift = index << 1;
                offset = shift + index;
                data = (u8 *)(offset + (u32)base);
                if (y != data[0])
                    goto searchRoomEntry;
                data = base + 1;
                data = (u8 *)(offset + (u32)data);
                if (x != data[0])
                    goto searchRoomEntry;
                found++;
roomSearchDone:
                if (found == 0)
                    goto end;
            }
        }

        {
            u8 *persistent;
            u8 *persistentBase;
            u32 persistentOffset;
            int value;
            int mask;

            persistentBase = &gPersistentSpriteData[0][0];
            persistentOffset = index;
            persistentOffset += gCurrentRoom << 6;
            persistent = persistentBase + persistentOffset;
            value = *persistent;
            mask = 0xF0;
            mask &= value;
            if (mask != 0x10)
                goto end;
        }

        found = 0xFF;
        index = 0;
        {
            u8 *baseLoad;
            struct PrimarySpriteData *sprites;
            struct PrimarySpriteData *sprite;
            int id;
            int offset;
            int one;

            baseLoad = (u8 *)gSpriteData;
            id = baseLoad[23];
            sprites = (struct PrimarySpriteData *)baseLoad;
            if (id == 58) {
                id = sprites[0].status;
                one = 1;
                one &= id;
                if (one != 0) {
                    found = 0;
                    goto spriteSearchDone;
                }
            }
searchSprites:
            index++;
            if (index > 23)
                goto spriteSearchDone;
            offset = 44;
            offset *= index;
            sprite = (struct PrimarySpriteData *)(offset + (u32)sprites);
            if (sprite->globalID != 58)
                goto searchSprites;
            id = sprite->status;
            one = 1;
            one &= id;
            if (one == 0)
                goto searchSprites;
            found = index;

spriteSearchDone:
            if (found != 0xFF) {
                offset = 44;
                offset *= found;
                sprite = (struct PrimarySpriteData *)(offset + (u32)sprites);
                if (sprite->pose == 17)
                    goto end;
                sprite->warioCollision = 29;
            }
        }

        gSwitchStates[1] = 3;
        m4aSongNumStart(SE_DOMINO_GOAL_OPEN);
    } else {
        UpdateDominoActivatorTrail();
    }
end:
    return;
}



void InitDominoLongPieceA(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x400;
    sprite->drawDistanceDown = 40;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 40;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->drawPriority = 3;
    GP_SET(sprite->pOamData, sDominoLongPieceAAnimationOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoLongPieceB(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x400;
    sprite->drawDistanceDown = 40;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 40;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->drawPriority = 3;
    GP_SET(sprite->pOamData, sDominoLongPieceBAnimationOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoGoalClosed(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 40;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 32;
    sprite->hitboxExtentUp = 52;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 40;
    sprite->hitboxExtentRight = 0;
    sprite->warioCollision = 48;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    GP_SET(sprite->pOamData, sDominoGoalClosedAnimationOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
    sprite->work0 = 30;
    sprite->work1 = 0;
}

void InitDominoGoalOpenLeft(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 40;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 32;
    sprite->hitboxExtentUp = 40;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 40;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 29;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    GP_SET(sprite->pOamData, sDominoGoalOpenLeftAnimationOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 24;
    sprite->work1 = 0;
}

void InitDominoGoalOpenRight(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 40;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 32;
    sprite->hitboxExtentUp = 40;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 40;
    sprite->hitboxExtentRight = 0;
    sprite->warioCollision = 29;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    GP_SET(sprite->pOamData, sDominoGoalOpenRightAnimationOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 24;
    sprite->work1 = 0;
}

void UpdateDominoGoalClosed(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    u8 *persistent;
    register int room PIN("r0");
    u32 temp;
    int value;
    int zero;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    room = gCurrentRoom;
    room <<= 6;
    temp = sprite->roomSlot;
    room += temp;
    persistent = (u8 *)room;
    persistent += (u32)persistentBase;
    if (*persistent == 17) {
        temp = (u32)sprite;
        temp += 39;
        value = *(u8 *)temp;
        value--;
        *(u8 *)temp = value;
        zero = (u8)value;
        if (zero == 0) {
            GP_SET(sprite->pOamData, sDominoGoalOpeningAnimationOam);
            sprite->currentAnimationFrame = zero;
            sprite->animationTimer = zero;
            *(u8 *)temp = 15;
            sprite->pose = 20;
            sprite->warioCollision = 29;
            zero = (u32)sprite;
            zero += 35;
            *(u8 *)zero = 40;
            m4aSongNumStart(SE_DOMINO_GOAL_OPEN);
            VoiceSetPlay(4);
        }
    }
}

void UpdateDominoGoalOpenLeft(void)
{
    struct PrimarySpriteData *sprite;
    u8 value;

    sprite = &gCurrentSprite;
    value = --sprite->work0;
    if (value == 0) {
        GP_SET(sprite->pOamData, sDominoGoalOpenLeftAnimationOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->pose = 24;
    }
}

void StartDominoGoalOpening(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    u8 *persistent;
    register int room PIN("r0");
    u32 temp;
    int value;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    room = gCurrentRoom;
    room <<= 6;
    temp = sprite->roomSlot;
    room += temp;
    persistent = (u8 *)room;
    persistent += (u32)persistentBase;
    temp = 0;
    value = 33;
    *persistent = value;
    gSwitchStates[1] = 3;
    GP_SET(sprite->pOamData, sDominoGoalOpeningAnimationOam);
    sprite->currentAnimationFrame = temp;
    sprite->animationTimer = temp;
    sprite->work0 = 15;
    sprite->pose = 18;
    sprite->warioCollision = 29;
    sprite->hitboxExtentUp = 40;
    StopDominoActivator();
    WakeDominoGoals();
    m4aSongNumStart(SE_DOMINO_GOAL_TRIGGER);
    VoiceSetPlay(1);
}

void UpdateDominoGoalOpenRight(void)
{
    struct PrimarySpriteData *sprite;
    u8 value;

    sprite = &gCurrentSprite;
    value = --sprite->work0;
    if (value == 0) {
        GP_SET(sprite->pOamData, sDominoGoalOpenRightAnimationOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->pose = 24;
        SpawnScoreCoinDropSprites(sprite->yPosition - 64, sprite->xPosition + 96, 10);
    }
}

void InitDominoTile0(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 48;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile2(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 8;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile4(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile6(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 40;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile8(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 64;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition -= 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile1(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 48;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition += 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile3(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 8;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition += 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile5(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition += 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile7(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 40;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition += 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}

void InitDominoTile9(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 64;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 64;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    sprite->warioCollision = 6;
    sprite->xPosition += 32;
    sprite->drawPriority = 3;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 16;
}


void SelectDominoTile0Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;
    const struct AnimationFrame *oam;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x40)
        goto case40;
    if (pose > 0x40)
        goto greater40;
    if (pose == 0x20)
        goto case20;
    if (pose > 0x20)
        goto greater20;
    if (pose == 0x10)
        goto case10;
    goto useDefault;

greater20:
    if (pose == 0x30)
        goto case30;
    goto useDefault;

greater40:
    if (pose == 0x60)
        goto case60;
    if (pose > 0x60)
        goto greater60;
    if (pose == 0x50)
        goto case50;
    goto useDefault;

greater60:
    if (value == 0x70)
        goto case70;
    goto useDefault;

case10:
    oam = sDominoTile0Animation1Oam;
    goto store;
case20:
    oam = sDominoTile0Animation2Oam;
    goto store;
case30:
    oam = sDominoTile0Animation3Oam;
    goto store;
case40:
    oam = sDominoTile0Animation4Oam;
    goto store;
case50:
    oam = sDominoTile0Animation5Oam;
    goto store;
case60:
    oam = sDominoTile0Animation6Oam;
    goto store;
case70:
    oam = sDominoTile0Animation7Oam;
    goto store;
useDefault:
    oam = sDominoTile0Animation0Oam;
store:
    GP_SET(sprite->pOamData, oam);
}

void SelectDominoTile1Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;
    const struct AnimationFrame *oam;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x40)
        goto case40;
    if (pose > 0x40)
        goto greater40;
    if (pose == 0x20)
        goto case20;
    if (pose > 0x20)
        goto greater20;
    if (pose == 0x10)
        goto case10;
    goto useDefault;

greater20:
    if (pose == 0x30)
        goto case30;
    goto useDefault;

greater40:
    if (pose == 0x60)
        goto case60;
    if (pose > 0x60)
        goto greater60;
    if (pose == 0x50)
        goto case50;
    goto useDefault;

greater60:
    if (value == 0x70)
        goto case70;
    goto useDefault;

case10:
    oam = sDominoTile1Animation1Oam;
    goto store;
case20:
    oam = sDominoTile1Animation2Oam;
    goto store;
case30:
    oam = sDominoTile1Animation3Oam;
    goto store;
case40:
    oam = sDominoTile1Animation4Oam;
    goto store;
case50:
    oam = sDominoTile1Animation5Oam;
    goto store;
case60:
    oam = sDominoTile1Animation6Oam;
    goto store;
case70:
    oam = sDominoTile1Animation7Oam;
    goto store;
useDefault:
    oam = sDominoTile1Animation0Oam;
store:
    GP_SET(sprite->pOamData, oam);
}

void SelectDominoTile2Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;
    const struct AnimationFrame *oam;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x50)
        goto case50;
    if (pose > 0x50)
        goto greater50;
    if (pose == 0x20)
        goto case20;
    if (pose > 0x20)
        goto greater20;
    if (pose == 0x10)
        goto case10;
    goto useDefault;

greater20:
    if (pose == 0x30)
        goto case30;
    if (pose == 0x40)
        goto case40;
    goto useDefault;

greater50:
    if (pose == 0x70)
        goto case70;
    if (pose > 0x70)
        goto greater70;
    if (pose == 0x60)
        goto case60;
    goto useDefault;

greater70:
    if (value == 0x80)
        goto case80;
    if (value == 0x90)
        goto case90;
    goto useDefault;

case10:
    oam = sDominoTile2Animation1Oam;
    goto store;
case20:
    oam = sDominoTile2Animation2Oam;
    goto store;
case30:
    oam = sDominoTile2Animation3Oam;
    goto store;
case40:
    oam = sDominoTile2Animation4Oam;
    goto store;
case50:
    oam = sDominoTile2Animation5Oam;
    goto store;
case60:
    oam = sDominoTile2Animation6Oam;
    goto store;
case70:
    oam = sDominoTile2Animation7Oam;
    goto store;
case80:
    oam = sDominoTile2Animation8Oam;
    goto store;
case90:
    oam = sDominoTile2Animation9Oam;
    goto store;
useDefault:
    oam = sDominoTile2Animation0Oam;
store:
    GP_SET(sprite->pOamData, oam);
}

void SelectDominoTile3Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;
    const struct AnimationFrame *oam;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x50)
        goto case50;
    if (pose > 0x50)
        goto greater50;
    if (pose == 0x20)
        goto case20;
    if (pose > 0x20)
        goto greater20;
    if (pose == 0x10)
        goto case10;
    goto useDefault;

greater20:
    if (pose == 0x30)
        goto case30;
    if (pose == 0x40)
        goto case40;
    goto useDefault;

greater50:
    if (pose == 0x70)
        goto case70;
    if (pose > 0x70)
        goto greater70;
    if (pose == 0x60)
        goto case60;
    goto useDefault;

greater70:
    if (value == 0x80)
        goto case80;
    if (value == 0x90)
        goto case90;
    goto useDefault;

case10:
    oam = sDominoTile3Animation1Oam;
    goto store;
case20:
    oam = sDominoTile3Animation2Oam;
    goto store;
case30:
    oam = sDominoTile3Animation3Oam;
    goto store;
case40:
    oam = sDominoTile3Animation4Oam;
    goto store;
case50:
    oam = sDominoTile3Animation5Oam;
    goto store;
case60:
    oam = sDominoTile3Animation6Oam;
    goto store;
case70:
    oam = sDominoTile3Animation7Oam;
    goto store;
case80:
    oam = sDominoTile3Animation8Oam;
    goto store;
case90:
    oam = sDominoTile3Animation9Oam;
    goto store;
useDefault:
    oam = sDominoTile3Animation0Oam;
store:
    GP_SET(sprite->pOamData, oam);
}

void SelectDominoTile4Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;
    const struct AnimationFrame *oam;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x40)
        goto case40;
    if (pose > 0x40)
        goto greater40;
    if (pose == 0x20)
        goto case20;
    if (pose > 0x20)
        goto greater20;
    if (pose == 0x10)
        goto case10;
    goto useDefault;

greater20:
    if (pose == 0x30)
        goto case30;
    goto useDefault;

greater40:
    if (pose == 0x60)
        goto case60;
    if (pose > 0x60)
        goto greater60;
    if (pose == 0x50)
        goto case50;
    goto useDefault;

greater60:
    if (value == 0x70)
        goto case70;
    if (value == 0x80)
        goto case80;
    goto useDefault;

case10:
    oam = sDominoTile4Animation1Oam;
    goto store;
case20:
    oam = sDominoTile4Animation2Oam;
    goto store;
case30:
    oam = sDominoTile4Animation3Oam;
    goto store;
case40:
    oam = sDominoTile4Animation4Oam;
    goto store;
case50:
    oam = sDominoTile4Animation5Oam;
    goto store;
case60:
    oam = sDominoTile4Animation6Oam;
    goto store;
case70:
    oam = sDominoTile4Animation7Oam;
    goto store;
case80:
    oam = sDominoTile4Animation8Oam;
    goto store;
useDefault:
    oam = sDominoTile4Animation0Oam;
store:
    GP_SET(sprite->pOamData, oam);
}

void SelectDominoTile5Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;
    const struct AnimationFrame *oam;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x40)
        goto case40;
    if (pose > 0x40)
        goto greater40;
    if (pose == 0x20)
        goto case20;
    if (pose > 0x20)
        goto greater20;
    if (pose == 0x10)
        goto case10;
    goto useDefault;

greater20:
    if (pose == 0x30)
        goto case30;
    goto useDefault;

greater40:
    if (pose == 0x60)
        goto case60;
    if (pose > 0x60)
        goto greater60;
    if (pose == 0x50)
        goto case50;
    goto useDefault;

greater60:
    if (value == 0x70)
        goto case70;
    if (value == 0x80)
        goto case80;
    goto useDefault;

case10:
    oam = sDominoTile5Animation1Oam;
    goto store;
case20:
    oam = sDominoTile5Animation2Oam;
    goto store;
case30:
    oam = sDominoTile5Animation3Oam;
    goto store;
case40:
    oam = sDominoTile5Animation4Oam;
    goto store;
case50:
    oam = sDominoTile5Animation5Oam;
    goto store;
case60:
    oam = sDominoTile5Animation6Oam;
    goto store;
case70:
    oam = sDominoTile5Animation7Oam;
    goto store;
case80:
    oam = sDominoTile5Animation8Oam;
    goto store;
useDefault:
    oam = sDominoTile5Animation0Oam;
store:
    GP_SET(sprite->pOamData, oam);
}

void SelectDominoTile6Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x70)
        goto case70;
    if (pose > 0x70)
        goto greater70;
    if (pose == 0x30)
        goto case30;
    if (pose > 0x30)
        goto greater30;
    if (pose == 0x10)
        goto useDefault;
    if (pose == 0x20)
        goto case20;
    goto useDefault;

greater30:
    if (pose == 0x50)
        goto case50;
    if (pose > 0x50)
        goto greater50;
    if (pose == 0x40)
        goto case40;
    goto useDefault;

greater50:
    if (pose == 0x60)
        goto case60;
    goto useDefault;

greater70:
    if (pose == 0xB0)
        goto caseB0;
    if (pose > 0xB0)
        goto greaterB0;
    if (pose == 0x90)
        goto case90;
    if (pose > 0x90)
        goto greater90;
    if (pose == 0x80)
        goto case80;
    goto useDefault;

greater90:
    if (pose == 0xA0)
        goto caseA0;
    goto useDefault;

greaterB0:
    if (pose == 0xD0)
        goto caseD0;
    if (pose > 0xD0)
        goto greaterD0;
    if (pose == 0xC0)
        goto caseC0;
    goto useDefault;

greaterD0:
    if (value == 0xE0)
        goto caseE0;
    goto useDefault;

case20:
    GP_SET(sprite->pOamData, sDominoTile6Animation1Oam);
    return;
case30:
    GP_SET(sprite->pOamData, sDominoTile6Animation1Oam);
    return;
case40:
    GP_SET(sprite->pOamData, sDominoTile6Animation2Oam);
    return;
case50:
    GP_SET(sprite->pOamData, sDominoTile6Animation2Oam);
    return;
case60:
    GP_SET(sprite->pOamData, sDominoTile6Animation3Oam);
    return;
case70:
    GP_SET(sprite->pOamData, sDominoTile6Animation3Oam);
    return;
case80:
    GP_SET(sprite->pOamData, sDominoTile6Animation4Oam);
    return;
case90:
    GP_SET(sprite->pOamData, sDominoTile6Animation4Oam);
    return;
caseA0:
    GP_SET(sprite->pOamData, sDominoTile6Animation5Oam);
    return;
caseB0:
    GP_SET(sprite->pOamData, sDominoTile6Animation6Oam);
    return;
caseC0:
    GP_SET(sprite->pOamData, sDominoTile6Animation7Oam);
    return;
caseD0:
    GP_SET(sprite->pOamData, sDominoTile6Animation8Oam);
    return;
caseE0:
    GP_SET(sprite->pOamData, sDominoTile6Animation9Oam);
    return;
useDefault:
    GP_SET(sprite->pOamData, sDominoTile6Animation0Oam);
}

void SelectDominoTile7Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x70)
        goto case70;
    if (pose > 0x70)
        goto greater70;
    if (pose == 0x30)
        goto case30;
    if (pose > 0x30)
        goto greater30;
    if (pose == 0x10)
        goto useDefault;
    if (pose == 0x20)
        goto case20;
    goto useDefault;

greater30:
    if (pose == 0x50)
        goto case50;
    if (pose > 0x50)
        goto greater50;
    if (pose == 0x40)
        goto case40;
    goto useDefault;

greater50:
    if (pose == 0x60)
        goto case60;
    goto useDefault;

greater70:
    if (pose == 0xB0)
        goto caseB0;
    if (pose > 0xB0)
        goto greaterB0;
    if (pose == 0x90)
        goto case90;
    if (pose > 0x90)
        goto greater90;
    if (pose == 0x80)
        goto case80;
    goto useDefault;

greater90:
    if (pose == 0xA0)
        goto caseA0;
    goto useDefault;

greaterB0:
    if (pose == 0xD0)
        goto caseD0;
    if (pose > 0xD0)
        goto greaterD0;
    if (pose == 0xC0)
        goto caseC0;
    goto useDefault;

greaterD0:
    if (value == 0xE0)
        goto caseE0;
    goto useDefault;

case20:
    GP_SET(sprite->pOamData, sDominoTile7Animation1Oam);
    return;
case30:
    GP_SET(sprite->pOamData, sDominoTile7Animation1Oam);
    return;
case40:
    GP_SET(sprite->pOamData, sDominoTile7Animation2Oam);
    return;
case50:
    GP_SET(sprite->pOamData, sDominoTile7Animation2Oam);
    return;
case60:
    GP_SET(sprite->pOamData, sDominoTile7Animation3Oam);
    return;
case70:
    GP_SET(sprite->pOamData, sDominoTile7Animation3Oam);
    return;
case80:
    GP_SET(sprite->pOamData, sDominoTile7Animation4Oam);
    return;
case90:
    GP_SET(sprite->pOamData, sDominoTile7Animation4Oam);
    return;
caseA0:
    GP_SET(sprite->pOamData, sDominoTile7Animation5Oam);
    return;
caseB0:
    GP_SET(sprite->pOamData, sDominoTile7Animation6Oam);
    return;
caseC0:
    GP_SET(sprite->pOamData, sDominoTile7Animation7Oam);
    return;
caseD0:
    GP_SET(sprite->pOamData, sDominoTile7Animation8Oam);
    return;
caseE0:
    GP_SET(sprite->pOamData, sDominoTile7Animation9Oam);
    return;
useDefault:
    GP_SET(sprite->pOamData, sDominoTile7Animation0Oam);
}

void SelectDominoTile8Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x70)
        goto case70;
    if (pose > 0x70)
        goto greater70;
    if (pose == 0x30)
        goto case30;
    if (pose > 0x30)
        goto greater30;
    if (pose == 0x10)
        goto useDefault;
    if (pose == 0x20)
        goto case20;
    goto useDefault;

greater30:
    if (pose == 0x50)
        goto case50;
    if (pose > 0x50)
        goto greater50;
    if (pose == 0x40)
        goto case40;
    goto useDefault;

greater50:
    if (pose == 0x60)
        goto case60;
    goto useDefault;

greater70:
    if (pose == 0xB0)
        goto caseB0;
    if (pose > 0xB0)
        goto greaterB0;
    if (pose == 0x90)
        goto case90;
    if (pose > 0x90)
        goto greater90;
    if (pose == 0x80)
        goto case80;
    goto useDefault;

greater90:
    if (pose == 0xA0)
        goto caseA0;
    goto useDefault;

greaterB0:
    if (pose == 0xD0)
        goto caseD0;
    if (pose > 0xD0)
        goto greaterD0;
    if (pose == 0xC0)
        goto caseC0;
    goto useDefault;

greaterD0:
    if (value == 0xE0)
        goto caseE0;
    goto useDefault;

case20:
    GP_SET(sprite->pOamData, sDominoTile8Animation1Oam);
    return;
case30:
    GP_SET(sprite->pOamData, sDominoTile8Animation1Oam);
    return;
case40:
    GP_SET(sprite->pOamData, sDominoTile8Animation2Oam);
    return;
case50:
    GP_SET(sprite->pOamData, sDominoTile8Animation2Oam);
    return;
case60:
    GP_SET(sprite->pOamData, sDominoTile8Animation3Oam);
    return;
case70:
    GP_SET(sprite->pOamData, sDominoTile8Animation3Oam);
    return;
case80:
    GP_SET(sprite->pOamData, sDominoTile8Animation4Oam);
    return;
case90:
    GP_SET(sprite->pOamData, sDominoTile8Animation4Oam);
    return;
caseA0:
    GP_SET(sprite->pOamData, sDominoTile8Animation5Oam);
    return;
caseB0:
    GP_SET(sprite->pOamData, sDominoTile8Animation6Oam);
    return;
caseC0:
    GP_SET(sprite->pOamData, sDominoTile8Animation7Oam);
    return;
caseD0:
    GP_SET(sprite->pOamData, sDominoTile8Animation8Oam);
    return;
caseE0:
    GP_SET(sprite->pOamData, sDominoTile8Animation9Oam);
    return;
useDefault:
    GP_SET(sprite->pOamData, sDominoTile8Animation0Oam);
}

void SelectDominoTile9Animation(void)
{
    u8 *persistentBase;
    struct PrimarySpriteData *sprite;
    register int value PIN("r0");
    int pose;

    persistentBase = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    value = gCurrentRoom;
    value <<= 6;
    value += sprite->roomSlot;
    value += (u32)persistentBase;
    value = *(u8 *)value;
    pose = 0xF0;
    pose &= value;
    value = pose;

    if (pose == 0x70)
        goto case70;
    if (pose > 0x70)
        goto greater70;
    if (pose == 0x30)
        goto case30;
    if (pose > 0x30)
        goto greater30;
    if (pose == 0x10)
        goto useDefault;
    if (pose == 0x20)
        goto case20;
    goto useDefault;

greater30:
    if (pose == 0x50)
        goto case50;
    if (pose > 0x50)
        goto greater50;
    if (pose == 0x40)
        goto case40;
    goto useDefault;

greater50:
    if (pose == 0x60)
        goto case60;
    goto useDefault;

greater70:
    if (pose == 0xB0)
        goto caseB0;
    if (pose > 0xB0)
        goto greaterB0;
    if (pose == 0x90)
        goto case90;
    if (pose > 0x90)
        goto greater90;
    if (pose == 0x80)
        goto case80;
    goto useDefault;

greater90:
    if (pose == 0xA0)
        goto caseA0;
    goto useDefault;

greaterB0:
    if (pose == 0xD0)
        goto caseD0;
    if (pose > 0xD0)
        goto greaterD0;
    if (pose == 0xC0)
        goto caseC0;
    goto useDefault;

greaterD0:
    if (value == 0xE0)
        goto caseE0;
    goto useDefault;

case20:
    GP_SET(sprite->pOamData, sDominoTile9Animation1Oam);
    return;
case30:
    GP_SET(sprite->pOamData, sDominoTile9Animation1Oam);
    return;
case40:
    GP_SET(sprite->pOamData, sDominoTile9Animation2Oam);
    return;
case50:
    GP_SET(sprite->pOamData, sDominoTile9Animation2Oam);
    return;
case60:
    GP_SET(sprite->pOamData, sDominoTile9Animation3Oam);
    return;
case70:
    GP_SET(sprite->pOamData, sDominoTile9Animation3Oam);
    return;
case80:
    GP_SET(sprite->pOamData, sDominoTile9Animation4Oam);
    return;
case90:
    GP_SET(sprite->pOamData, sDominoTile9Animation4Oam);
    return;
caseA0:
    GP_SET(sprite->pOamData, sDominoTile9Animation5Oam);
    return;
caseB0:
    GP_SET(sprite->pOamData, sDominoTile9Animation6Oam);
    return;
caseC0:
    GP_SET(sprite->pOamData, sDominoTile9Animation7Oam);
    return;
caseD0:
    GP_SET(sprite->pOamData, sDominoTile9Animation8Oam);
    return;
caseE0:
    GP_SET(sprite->pOamData, sDominoTile9Animation9Oam);
    return;
useDefault:
    GP_SET(sprite->pOamData, sDominoTile9Animation0Oam);
}

void SpriteDominoActivator(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
    case 0: InitDominoActivator(); break;
    case 16: WaitForDominoActivator(); break;
    case 18: UpdateDominoChain(); break;
    case 109: FinishDominoChain(); break;
    }
}

void SpriteDominoLongPieceA(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0)
        InitDominoLongPieceA();
}

void SpriteDominoLongPieceB(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0)
        InitDominoLongPieceB();
}

void SpriteDominoGoal(void)
{
    switch (gCurrentSprite.pose) {
    case 0: InitDominoGoalClosed(); break;
    case 1: InitDominoGoalOpenLeft(); break;
    case 16: UpdateDominoGoalClosed(); break;
    case 20: UpdateDominoGoalOpenLeft(); break;
    case 17: StartDominoGoalOpening(); break;
    case 18: UpdateDominoGoalOpenRight(); break;
    case 24: break;
    default: InitDominoGoalOpenRight(); break;
    }
    func_8026838();
}

void SpriteDominoTile0(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile0Animation1Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile0Animation2Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile0Animation3Oam;
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile0Animation4Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile0Animation5Oam;
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile0Animation6Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile0Animation7Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile0();
    goto end;

selectAnimation:
    SelectDominoTile0Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile0Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile0();

end:
    return;
}

void SpriteDominoTile1(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile1Animation1Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile1Animation2Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile1Animation3Oam;
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile1Animation4Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile1Animation5Oam;
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile1Animation6Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile1Animation7Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile1();
    goto end;

selectAnimation:
    SelectDominoTile1Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile1Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile1();

end:
    return;
}

void SpriteDominoTile2(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation1Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation2Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation3Oam;
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation4Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation5Oam;
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation6Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation7Oam;
        goto commonUpdate;
    case 8:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation8Oam;
        goto commonUpdate;
    case 9:
        target = &gCurrentSprite;
        oam = sDominoTile2Animation9Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile2();
    goto end;

selectAnimation:
    SelectDominoTile2Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile2Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile2();

end:
    return;
}

void SpriteDominoTile3(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation1Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation2Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation3Oam;
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation4Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation5Oam;
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation6Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation7Oam;
        goto commonUpdate;
    case 8:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation8Oam;
        goto commonUpdate;
    case 9:
        target = &gCurrentSprite;
        oam = sDominoTile3Animation9Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile3();
    goto end;

selectAnimation:
    SelectDominoTile3Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile3Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile3();

end:
    return;
}

void SpriteDominoTile4(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation1Oam);
        InitDominoTile4();
        return;
    case 2:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation2Oam);
        InitDominoTile4();
        return;
    case 3:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation3Oam);
        InitDominoTile4();
        return;
    case 4:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation4Oam);
        InitDominoTile4();
        return;
    case 5:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation5Oam);
        InitDominoTile4();
        return;
    case 6:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation6Oam);
        InitDominoTile4();
        return;
    case 7:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation7Oam);
        InitDominoTile4();
        return;
    case 8:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation8Oam);
        InitDominoTile4();
        return;
    case 16:
        SelectDominoTile4Animation();
        return;
    default:
        GP_SET(gCurrentSprite.pOamData, sDominoTile4Animation0Oam);
        InitDominoTile4();
        return;
    }
}

void SpriteDominoTile5(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation1Oam);
        InitDominoTile5();
        return;
    case 2:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation2Oam);
        InitDominoTile5();
        return;
    case 3:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation3Oam);
        InitDominoTile5();
        return;
    case 4:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation4Oam);
        InitDominoTile5();
        return;
    case 5:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation5Oam);
        InitDominoTile5();
        return;
    case 6:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation6Oam);
        InitDominoTile5();
        return;
    case 7:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation7Oam);
        InitDominoTile5();
        return;
    case 8:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation8Oam);
        InitDominoTile5();
        return;
    case 16:
        SelectDominoTile5Animation();
        return;
    default:
        GP_SET(gCurrentSprite.pOamData, sDominoTile5Animation0Oam);
        InitDominoTile5();
        return;
    }
}

void SpriteDominoTile6(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation0Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation1Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation1Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(3));
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation2Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation2Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(5));
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation3Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation3Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(7));
        goto commonUpdate;
    case 8:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation4Oam;
        goto commonUpdate;
    case 9:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation4Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(9));
        goto commonUpdate;
    case 10:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation5Oam;
        goto commonUpdate;
    case 11:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation6Oam;
        goto commonUpdate;
    case 12:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation7Oam;
        goto commonUpdate;
    case 13:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation8Oam;
        goto commonUpdate;
    case 14:
        target = &gCurrentSprite;
        oam = sDominoTile6Animation9Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile6();
    goto end;

selectAnimation:
    SelectDominoTile6Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile6Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile6();

end:
    return;
}

void SpriteDominoTile7(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation0Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation1Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation1Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(3));
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation2Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation2Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(5));
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation3Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation3Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(7));
        goto commonUpdate;
    case 8:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation4Oam;
        goto commonUpdate;
    case 9:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation4Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(9));
        goto commonUpdate;
    case 10:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation5Oam;
        goto commonUpdate;
    case 11:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation6Oam;
        goto commonUpdate;
    case 12:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation7Oam;
        goto commonUpdate;
    case 13:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation8Oam;
        goto commonUpdate;
    case 14:
        target = &gCurrentSprite;
        oam = sDominoTile7Animation9Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile7();
    goto end;

selectAnimation:
    SelectDominoTile7Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile7Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile7();

end:
    return;
}

void SpriteDominoTile8(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation0Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation1Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation1Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(3));
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation2Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation2Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(5));
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation3Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation3Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(7));
        goto commonUpdate;
    case 8:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation4Oam;
        goto commonUpdate;
    case 9:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation4Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(9));
        goto commonUpdate;
    case 10:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation5Oam;
        goto commonUpdate;
    case 11:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation6Oam;
        goto commonUpdate;
    case 12:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation7Oam;
        goto commonUpdate;
    case 13:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation8Oam;
        goto commonUpdate;
    case 14:
        target = &gCurrentSprite;
        oam = sDominoTile8Animation9Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile8();
    goto end;

selectAnimation:
    SelectDominoTile8Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile8Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile8();

end:
    return;
}

void SpriteDominoTile9(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *target;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateDominoPalette();
    switch (sprite->pose) {
    case 1:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation0Oam;
        goto commonUpdate;
    case 2:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation1Oam;
        goto commonUpdate;
    case 3:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation1Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(3));
        goto commonUpdate;
    case 4:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation2Oam;
        goto commonUpdate;
    case 5:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation2Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(5));
        goto commonUpdate;
    case 6:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation3Oam;
        goto commonUpdate;
    case 7:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation3Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(7));
        goto commonUpdate;
    case 8:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation4Oam;
        goto commonUpdate;
    case 9:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation4Oam;
        ASM_BARRIER_VOLATILE(: "+r"(target), "+r"(oam) : "i"(9));
        goto commonUpdate;
    case 10:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation5Oam;
        goto commonUpdate;
    case 11:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation6Oam;
        goto commonUpdate;
    case 12:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation7Oam;
        goto commonUpdate;
    case 13:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation8Oam;
        goto commonUpdate;
    case 14:
        target = &gCurrentSprite;
        oam = sDominoTile9Animation9Oam;
        goto commonUpdate;
    case 16:
        goto selectAnimation;
    default:
        goto useDefault;
    }

commonUpdate:
    GP_SET(target->pOamData, oam);
    InitDominoTile9();
    goto end;

selectAnimation:
    SelectDominoTile9Animation();
    goto end;

useDefault:
    target = &gCurrentSprite;
    oam = sDominoTile9Animation0Oam;
    GP_SET(target->pOamData, oam);
    InitDominoTile9();

end:
    return;
}

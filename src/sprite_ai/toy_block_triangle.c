#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/toy_block_triangle.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

s32 ToyBlockTriangleUnlockTouchedLock(void)
{
    u32 currentBottom;
    u32 currentLeft;
    u32 currentRight;
    s32 i;
    struct PrimarySpriteData* sprite;
    u8 pose;
    volatile u32 currentTop;

    {
        struct PrimarySpriteData* current;
        u32 value;
        register u32 currentY PIN("r1");
        register u32 currentX PIN("r2");

        current = &gCurrentSprite;
        {
            register u32 bias PIN("r0");
            bias = 0x1000;
            ASM_BARRIER(: "=r"(currentX) : "0"(bias));
        }
        value = current->yPosition;
        currentY = currentX + value;
        currentY = (u16)currentY;
        currentX += current->xPosition;
        currentX = (u16)currentX;

        value = current->hitboxExtentUp;
        value = currentY - value;
        value = (u16)value;
        currentTop = value;

        value = current->hitboxExtentDown;
        currentY += value;
        currentY = (u16)currentY;
        currentBottom = currentY;

        value = current->hitboxExtentLeft;
        value = currentX - value;
        value = (u16)value;
        currentLeft = value;

        {
            u8* extent;

            extent = (u8*)current;
            extent += 38;
            value = *extent;
        }
        currentX += value;
        currentX = (u16)currentX;
        currentRight = currentX;
    }

    i = 0;
    pose = 17;
    do
    {
        struct PrimarySpriteData* sprites;
        register u32 value PIN("r0");

        sprites = gSpriteData;
        value = sizeof(struct PrimarySpriteData);
        value *= i;
        sprite = (struct PrimarySpriteData*)(value + (u32)sprites);
        if ((sprite->status & SPRITE_STATUS_EXISTS) &&
            sprite->globalID == PSPRITE_TOY_BLOCK_TRIANGLE_LOCK && sprite->pose == 16)
        {
            register u32 spriteY PIN("r3");
            register u32 spriteX PIN("r2");
            register u32 top PIN("r4");
            u32 left;
            register u8* extent PIN("r0");

            value = 0x1000;
            /* Keep the position bias in r0 before copying it to r2. */
            ASM_BARRIER(: "=r"(spriteX) : "0"(value));
            value = sprite->yPosition;
            spriteY = spriteX + value;
            spriteY = (u16)spriteY;
            spriteX += sprite->xPosition;
            spriteX = (u16)spriteX;

            extent = (u8*)sprite + 35;
            top = spriteY - *extent;
            top = (u16)top;

            /* Volatile keeps the pointer increment separate from the byte load. */
            extent++;
            spriteY -= *(volatile u8 *)extent;
            spriteY = (u16)spriteY;

            extent = (u8*)sprite + 37;
            left = spriteX - *extent;
            left = (u16)left;

            extent++;
            { register u8 loaded PIN("r0");
              loaded = *(volatile u8 *)extent;
              spriteX += loaded;
            }
            spriteX = (u16)spriteX;

            /* Use word-width arguments so agbcc preserves the already-truncated register values. */
            if (((s32 (*)(u32, u32, u32, u32, u32, u32, u32, u32))
                    SpriteCollisionCheckObjectsTouching)(currentTop, currentBottom,
                    currentLeft, currentRight, top, spriteY, left, spriteX))
            {
                sprite->pose = pose;
                sprite->status &= 0xFFFB;
                gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = pose;
                m4aSongNumStart(SE_TOY_BLOCK_TRIANGLE_UNLOCK);
                VoiceSetPlay(0);
                return TRUE;
            }
        }
        i++;
    } while (i <= 23);

    return FALSE;
}

void InitToyBlockTriangle(void)
{
    struct PrimarySpriteData* sprite;
    struct PrimarySpriteData* temp;
    u8* drawPtr;
    u16 zero16;
    register u8 zero PIN("r2");
    u8 value;
    u16 status;
    u16 x;

    sprite = &gCurrentSprite;
    x = sprite->xPosition;
    x += 32;
    zero = 0;
    zero16 = 0;
    temp = sprite;
    temp->xPosition = x;
    status = temp->status;
    value = 32;
    status |= value;

    {
        u8* hitboxPtr;
        hitboxPtr = (u8*)sprite;
        *(u16*)hitboxPtr = status;
        drawPtr = (u8*)sprite + 32;
        *drawPtr++ = value;
        *drawPtr = zero;
        hitboxPtr += 34;
        *hitboxPtr = 16;
        hitboxPtr++;
        *hitboxPtr = 96;
        *((u8*)sprite + 36) = zero;
        hitboxPtr += 2;
        *hitboxPtr = 36;
        *((u8*)sprite + 38) = value;
    }

    {
        const struct AnimationFrame* oam;
        oam = sToyBlockTriangleOam;
        temp = sprite;
        GP_SET(temp->pOamData, oam);
    }
    temp->currentAnimationFrame = zero;
    temp->animationTimer = zero16;
    temp->warioCollision = 5;
}

void ResetToyBlockTriangle(void)
{
    gCurrentSprite.pose = 16;
    gCurrentSprite.status &= 0xFCFF;
}

void ToyBlockTriangleIdle(void)
{
    func_8023B88();
    if (gSpriteCollisionResult == 0)
    {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
            func_8023BFC(gCurrentSprite.yPosition,
                (u16)(gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft));
        else
            func_8023BFC(gCurrentSprite.yPosition,
                (u16)(gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight));

        if (gSpriteCollisionTileType == 0)
            gCurrentSprite.pose = 27;
    }
}

void StartToyBlockTriangleFalling(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockTriangleCounterclockwisePush(void)
{
    gCurrentSprite.pose = 44;
    gCurrentSprite.work1 = 0;
}

void StartToyBlockTriangleClockwisePush(void)
{
    gCurrentSprite.pose = 46;
    gCurrentSprite.work1 = 0;
}

void StartToyBlockTriangleCounterclockwiseSpin(void)
{
    gCurrentSprite.pose = 72;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockTriangleClockwiseSpin(void)
{
    gCurrentSprite.pose = 74;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockTrianglePushedRight(void)
{
    gCurrentSprite.pose = 36;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
}

void StartToyBlockTrianglePushedLeft(void)
{
    gCurrentSprite.pose = 38;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
}

void StartToyBlockTriangleRollRight(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockTriangleRollLeft(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockTrianglePose66(void)
{
    gCurrentSprite.pose = 66;
    gCurrentSprite.work0 = 6;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockTrianglePose60(void)
{
    gCurrentSprite.pose = 60;
    gCurrentSprite.work0 = 6;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockTriangleRollAwayFromWario(void)
{
    if (gCurrentSprite.xPosition > gWarioData.xPosition)
        StartToyBlockTriangleRollRight();
    else
        StartToyBlockTriangleRollLeft();
}

void StartLiftingToyBlockTriangleRight(void)
{
    gCurrentSprite.pose = 82;
}

void StartLiftingToyBlockTriangleLeft(void)
{
    gCurrentSprite.pose = 84;
}

void StartCarryingToyBlockTriangleRight(void)
{
    gCurrentSprite.pose = 88;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void StartCarryingToyBlockTriangleLeft(void)
{
    gCurrentSprite.pose = 86;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void InitToyBlockTriangleLock(void)
{
    struct PrimarySpriteData* sprite;
    u8* ptr;
    u8* temp;
    u16 oldStatus;
    register u16 status PIN("r0");
    register u8 zero PIN("r2");
    u16 zero16;
    u16 x;

    sprite = &gCurrentSprite;
    x = sprite->xPosition;
    x += 32;
    zero16 = 0;
    ptr = (u8*)sprite;
    ((struct PrimarySpriteData*)ptr)->xPosition = x;
    oldStatus = ((struct PrimarySpriteData*)ptr)->status;
    status = 0x408;
    zero = 0;
    status |= oldStatus;
    ptr = (u8*)sprite;
    *(u16*)ptr = status;
    ptr += 32;
    *ptr = 32;
    temp = (u8*)sprite + 33;
    *temp = zero;
    ptr += 2;
    *ptr = 16;
    ptr++;
    *ptr = 64;
    ptr++;
    *ptr = 60;
    ptr++;
    *ptr = 4;
    ptr += 1;
    *ptr = 4;
    {
        const struct AnimationFrame* oam;
        oam = sToyBlockTriangleOam;
        ptr = (u8*)sprite;
        GP_SET(((struct PrimarySpriteData*)ptr)->pOamData, oam);
    }
    ptr[22] = zero;
    *(u16*)(ptr + 20) = zero16;
    ((struct PrimarySpriteData*)ptr)->warioCollision = 6;
}

void InitClosedToyBlockTriangleLock(void)
{
    gCurrentSprite.pose = 16;
    gCurrentSprite.status |= 4;
    InitToyBlockTriangleLock();
}

void InitOpenToyBlockTriangleLock(void)
{
    gCurrentSprite.pose = 24;
    InitToyBlockTriangleLock();
}

void UnlockMatchingToyBlockTriangleLock(void)
{
    u32 x;
    u32 y;
    s32 found;
    s32 i;
    register u8* data PIN("r3");
    s32 value;
    register u32 temp PIN("r1");

    {
        struct PrimarySpriteData* sprite;

        sprite = &gCurrentSprite;
        sprite->pose = 24;
        {
            register u32 yValue PIN("r0");
            yValue = sprite->yPosition;
            ASM_BARRIER(: "=r"(temp) : "0"(yValue));
        }
        temp += 252;
        y = temp >> 6;
        temp = sprite->xPosition;
        /* Preserve the target's copy from r1 to r0 before subtracting. */
        ASM_BARRIER(: "=r"(value) : "0"(temp));
        value -= 32;
    }

    if (value < 0)
        value += 63;
    x = (u32)(value << 10) >> 16;

    found = FALSE;
    i = 0;
    {
        register u8* rawData PIN("r0");

        rawData = &gRoomSpriteData[0][0];
        ASM_BARRIER(: "=r"(data) : "0"(rawData));
    }

    value = data[0];
    if (y != value || x != data[1])
    {
        do
        {
            i++;
            if (i > 63)
                break;
            value = i << 1;
            temp = value + i;
            {
                u8* entry;

                entry = (u8*)((u32)temp + (u32)data);
                value = *entry;
            }
            if (y != value)
                continue;
            {
                register u8* entry PIN("r0");

                entry = data + 1;
                entry = (u8*)((u32)temp + (u32)entry);
                value = *entry;
            }
            if (x != value)
                continue;
            found++;
            break;
        } while (TRUE);

        if (!found)
            return;
    }

    {
        gPersistentSpriteData[gCurrentRoom][i] = 17;

        {
            s32 roomSlot;
            s32 spriteIndex;
            struct PrimarySpriteData* sprites;
            struct PrimarySpriteData* first;

            roomSlot = i;
            spriteIndex = 0;
            first = gSpriteData;
            temp = first->roomSlot;
            sprites = first;
            if (temp == roomSlot)
            {
                sprites->pose = 17;
                return;
            }

            do
            {
                spriteIndex++;
                if (spriteIndex > 23)
                    break;
                value = sizeof(struct PrimarySpriteData);
                value *= spriteIndex;
                {
                    struct PrimarySpriteData* current;

                    current = (struct PrimarySpriteData*)((u32)value + (u32)sprites);
                    if (current->roomSlot != roomSlot)
                        continue;
                    /* Keep the short-lived pose materialization in r0 without instruction ASM. */
                    {
                        register u8 poseValue PIN("r0");
                        poseValue = 17;
                        current->pose = poseValue;
                    }
                }
                break;
            } while (TRUE);
        }
    }
}

void InitToyBlockTriangleDoor(void)
{
    struct PrimarySpriteData* sprite;
    u8* ptr;
    u8* temp;
    u16 oldStatus;
    register u16 status PIN("r0");
    register u8 zero PIN("r2");
    u16 zero16;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = 0x408;
    zero = 0;
    zero16 = 0;
    status |= oldStatus;
    ptr = (u8*)sprite;
    *(u16*)ptr = status;
    ((struct PrimarySpriteData*)ptr)->yPosition -= 32;
    ((struct PrimarySpriteData*)ptr)->xPosition += 32;
    ptr += 32;
    *ptr = 48;
    ptr++;
    *ptr = 16;
    ptr++;
    *ptr = 16;
    ptr++;
    *ptr = 36;
    ptr++;
    *ptr = 32;
    ptr++;
    *ptr = 64;
    ptr += 1;
    *ptr = 60;
    temp = (u8*)sprite;
    temp[22] = zero;
    *(u16*)(temp + 20) = zero16;
    {
        struct PrimarySpriteData* work;
        u8 collision;
        collision = 29;
        work = sprite;
        work->warioCollision = collision;
    }
    temp = (u8*)sprite;
    temp += 40;
    *temp = zero;
}

void InitClosedToyBlockTriangleDoor(void)
{
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 30;
    GP_SET(gCurrentSprite.pOamData, sToyBlockTriangleDoorClosedOam);
    InitToyBlockTriangleDoor();
}

void InitOpenToyBlockTriangleDoor(void)
{
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 180;
    GP_SET(gCurrentSprite.pOamData, sToyBlockTriangleDoorOpenOam);
    InitToyBlockTriangleDoor();
}

void ToyBlockTriangleDoorWaitToOpen(void)
{
    u8 value;

    value = --gCurrentSprite.work0;
    if (value == 0)
    {
        gCurrentSprite.pose = 18;
        GP_SET(gCurrentSprite.pOamData, sToyBlockTriangleDoorOpeningOam);
        gCurrentSprite.currentAnimationFrame = value;
        gCurrentSprite.animationTimer = value;
        gCurrentSprite.work0 = 29;
    }
}

void ToyBlockTriangleDoorOpen(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u32 value;
    u8 truncated;

    sprite = &gCurrentSprite;
    timerPtr = (u8*)sprite + 39;
    value = *timerPtr;
    value--;
    *timerPtr = value;
    truncated = (u8)value;
    if (truncated == 0)
    {
        GP_SET(sprite->pOamData, sToyBlockTriangleDoorOpenOam);
        sprite->currentAnimationFrame = truncated;
        sprite->animationTimer = truncated;
        *timerPtr = 180;
        SpawnSecondarySprite(sprite->yPosition + 32, sprite->xPosition - 64, SSPRITE_31);
        SpawnSecondarySprite(sprite->yPosition + 32, sprite->xPosition - 32, SSPRITE_32);
        SpawnSecondarySprite(sprite->yPosition + 32, sprite->xPosition, SSPRITE_31);
        SpawnSecondarySprite(sprite->yPosition + 32, sprite->xPosition + 32, SSPRITE_32);
        SpawnSecondarySprite(sprite->yPosition + 32, sprite->xPosition + 64, SSPRITE_31);
        sprite->pose = 24;
    }
}

void ToyBlockTriangleDoorStartOpenIdleAnimation(void)
{
    u8 value;

    value = --gCurrentSprite.work0;
    if (value == 0)
    {
        gCurrentSprite.pose = 20;
        GP_SET(gCurrentSprite.pOamData, sToyBlockTriangleDoorOpenIdleAnimationOam);
        gCurrentSprite.currentAnimationFrame = value;
        gCurrentSprite.animationTimer = value;
        gCurrentSprite.work0 = 14;
    }
}

void ToyBlockTriangleDoorFinishOpenIdleAnimation(void)
{
    u8 value;

    value = --gCurrentSprite.work0;
    if (value == 0)
    {
        GP_SET(gCurrentSprite.pOamData, sToyBlockTriangleDoorOpenOam);
        gCurrentSprite.currentAnimationFrame = value;
        gCurrentSprite.animationTimer = value;
        gCurrentSprite.work0 = 180;
        gCurrentSprite.pose = 24;
    }
}

void SpriteToyBlockTriangle(void)
{
    struct PrimarySpriteData* sprite;

    if ((gCurrentCarriedSprite.state == 0 || gCurrentCarriedSprite.state == 12) && ToyBlockTriangleUnlockTouchedLock())
    {
        gCurrentSprite.status = 0;
        gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
        return;
    }

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_UNDERWATER)
        sprite->pose = 107;

    switch (sprite->pose)
    {
        case 0:
            InitToyBlockTriangle();
        case 15:
        case 23:
        case 47:
            ResetToyBlockTriangle();
        case 16:
            ToyBlockTriangleIdle();
            break;
        case 27:
        case 29:
            StartToyBlockTriangleFalling();
        case 30:
            func_8023EE0();
            break;
        case 35:
            StartToyBlockTrianglePushedRight();
        case 36:
            SpriteUtilPushedRight();
            break;
        case 37:
            StartToyBlockTrianglePushedLeft();
        case 38:
            SpriteUtilPushedLeft();
            break;
        case 43:
            StartToyBlockTriangleCounterclockwisePush();
        case 44:
            func_8024478();
            break;
        case 45:
            StartToyBlockTriangleClockwisePush();
        case 46:
            func_802449C();
            break;
        case 49:
            StartToyBlockTriangleRollAwayFromWario();
            break;
        case 31:
        case 51:
        case 79:
            StartToyBlockTriangleRollRight();
            goto pose64;
        case 33:
        case 53:
        case 80:
            StartToyBlockTriangleRollLeft();
            goto pose58;
        case 106:
            StartToyBlockTrianglePose66();
            break;
        case 105:
            StartToyBlockTrianglePose60();
            break;
        case 55:
            func_8024688();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
pose58:
        case 54:
        case 58:
            func_802476C();
            break;
        case 59:
            func_80247F0();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
pose64:
        case 52:
        case 64:
            func_8024988();
            break;
        case 65:
            func_8024A0C();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
            goto pose68;
        case 69:
            func_8024BEC();
            goto pose70;
        case 71:
            StartToyBlockTriangleCounterclockwiseSpin();
pose68:
        case 68:
        case 72:
            func_8024AD4();
            break;
        case 73:
            StartToyBlockTriangleClockwiseSpin();
pose70:
        case 70:
        case 74:
            func_8024C00();
            break;
        case 81:
            StartLiftingToyBlockTriangleRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            StartLiftingToyBlockTriangleLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            StartCarryingToyBlockTriangleRight();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            StartCarryingToyBlockTriangleLeft();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;
        case 89:
            sprite->pose = 90;
        case 90:
            SpriteUtilThrownLeftSoft();
            break;
        case 93:
            sprite->pose = 94;
            sprite->health = 1;
        case 94:
            SpriteUtilThrownLeftHard();
            break;
        case 97:
            sprite->pose = 98;
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;
        case 101:
            sprite->pose = 102;
            sprite->health = 18;
        case 102:
            SpriteUtilThrownUpLeftHard();
            break;
        case 91:
            sprite->pose = 92;
        case 92:
            SpriteUtilThrownRightSoft();
            break;
        case 95:
            sprite->pose = 96;
            sprite->health = 2;
        case 96:
            SpriteUtilThrownRightHard();
            break;
        case 99:
            sprite->pose = 100;
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;
        case 103:
            sprite->pose = 104;
            sprite->health = 17;
        case 104:
            SpriteUtilThrownUpRightHard();
            break;
        case 107:
            if (!(sprite->status & SPRITE_STATUS_UNDERWATER))
            {
                if (sprite->health & 0xF0)
                    sprite->work2 = 4;
                else
                    sprite->work2 = 8;

                if ((sprite->health & 0xF) == 2)
                    func_8024AC0();
                else
                    func_8024BEC();
                break;
            }
        default:
            gPersistentSpriteData[gCurrentRoom][sprite->roomSlot]--;
            SpriteUtilDie();
            break;
    }
    return;

}

void SpriteToyBlockTriangleLock(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose)
    {
        case 0:
            InitClosedToyBlockTriangleLock();
            break;
        case 1:
            InitOpenToyBlockTriangleLock();
            break;
        case 17:
            UnlockMatchingToyBlockTriangleLock();
            break;
        case 16:
        default:
            break;
    }
}

void SpriteToyBlockTriangleDoor(void)
{
    switch (gCurrentSprite.pose)
    {
        case 0:
            InitClosedToyBlockTriangleDoor();
        case 16:
            gCurrentSprite.disableWarioCollisionTimer = 1;
            break;
        case 1:
            InitOpenToyBlockTriangleDoor();
            func_8026838();
            break;
        case 17:
            gCurrentSprite.disableWarioCollisionTimer = 1;
            ToyBlockTriangleDoorWaitToOpen();
            break;
        case 18:
            gCurrentSprite.disableWarioCollisionTimer = 1;
            ToyBlockTriangleDoorOpen();
            break;
        case 24:
            ToyBlockTriangleDoorStartOpenIdleAnimation();
            func_8026838();
            break;
        case 20:
            ToyBlockTriangleDoorFinishOpenIdleAnimation();
            func_8026838();
            break;
        default:
            break;
    }
}

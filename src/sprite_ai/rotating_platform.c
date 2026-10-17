#include "sprite_ai/rotating_platform.h"

#include "bg_clip.h"
#include "fixed_point.h"
#include "sprite.h"
#include "sprite_util.h"
#include "sound.h"
#include "wario.h"

void func_8028148(void)
{
    switch (gCurrentSprite.work2) {
        case 1:
            gCurrentSprite.xPosition += 8;
            break;

        case 2:
            gCurrentSprite.xPosition -= 8;
            break;

        case 17:
            gCurrentSprite.yPosition += 4;
            gCurrentSprite.xPosition += 4;
            break;

        case 33:
            gCurrentSprite.yPosition -= 4;
            gCurrentSprite.xPosition += 4;
            break;

        case 18:
            gCurrentSprite.yPosition += 4;
            gCurrentSprite.xPosition -= 4;
            break;

        case 34:
            gCurrentSprite.yPosition -= 4;
            gCurrentSprite.xPosition -= 4;
            break;
    }
}

void func_802822C(void)
{
    u32 temp;
    register u16 status PIN("r1");
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *current;
    s32 i;
    u8 roomSlot;
    u32 exists;
    struct PrimarySpriteData *sprites;

    temp = (u32)&gCurrentSprite;
    roomSlot = ((struct PrimarySpriteData *)temp)->roomSlot;
    i = 0;
    current = (struct PrimarySpriteData *)temp;
    sprites = gSpriteData;
    exists = SPRITE_STATUS_EXISTS;
    sprite = sprites;
    do {
        if (sprite->globalID == PSPRITE_ROTATING_PLATFORM &&
            sprite->roomSlot == roomSlot) {
            status = sprite->status;
            temp = exists;
            temp &= status;
            if (temp != FALSE) {
                goto foundSprite;
            }
        }
        sprite++;
        i++;
    } while (i < MAX_SPRITE_COUNT);

    temp = FALSE;
    /* Keep the zero value live in r0 through the loop exit. */
    ASM_BARRIER(: "+r"(temp));
    if (temp != FALSE) {
        goto foundSprite;
    }
    return;

foundSprite:
    switch (sprites[i].work2) {
        case 1:
            current->xPosition += 8;
            if (current->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                gWarioData.xPosition += 8;
            }
            break;

        case 2:
            current->xPosition -= 8;
            if (current->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                gWarioData.xPosition -= 8;
            }
            break;

        case 17:
            current->yPosition += 4;
            current->xPosition += 4;
            if (current->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                gWarioData.yPosition += 4;
                gWarioData.xPosition += 4;
            }
            break;

        case 33:
            current->yPosition -= 4;
            current->xPosition += 4;
            if (current->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                gWarioData.yPosition -= 4;
                gWarioData.xPosition += 4;
            }
            break;

        case 18:
            current->yPosition += 4;
            current->xPosition -= 4;
            if (current->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                gWarioData.yPosition += 4;
                gWarioData.xPosition -= 4;
            }
            break;

        case 34:
            current->yPosition -= 4;
            current->xPosition -= 4;
            if (current->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                gWarioData.yPosition -= 4;
                gWarioData.xPosition -= 4;
            }
            break;
    }
}

void func_8028400(void)
{
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 32;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
    GP_SET(gCurrentSprite.pOamData, sRotatingPlatformOam_83B4534);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 0;
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND |
                             SPRITE_STATUS_AFFINE |
                             SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.yPosition -= 32;

    SpriteSpawnAsChild(
        PSPRITE_94,
        gCurrentSprite.roomSlot,
        gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition,
        gCurrentSprite.xPosition + 128
    );
    SpriteSpawnAsChild(
        PSPRITE_96,
        gCurrentSprite.roomSlot,
        gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition,
        gCurrentSprite.xPosition - 128
    );
    SpriteSpawnAsChild(
        PSPRITE_97,
        gCurrentSprite.roomSlot,
        gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition - 128,
        gCurrentSprite.xPosition
    );
    SpriteSpawnAsChild(
        PSPRITE_95,
        gCurrentSprite.roomSlot,
        gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition + 128,
        gCurrentSprite.xPosition
    );
}

void func_80284BC(void)
{
    s32 block;
    struct PrimarySpriteData *caseSprite;
    u8 *caseWork;
    register u32 caseDirection PIN("r1");

    block = GetBg2CollisionTypeAtTile(
        gCurrentSprite.yPosition >> 6,
        gCurrentSprite.xPosition >> 6
    );

    switch (block) {
        case 1:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 0;
            goto setDirection;
        case 2:
        case 5:
        case 6:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 1;
            goto setDirection;
        case 3:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 33;
            goto setDirection;
        case 4:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 17;
            goto setDirection;
        default:
            goto updateExisting;
    }

setDirection:
    *caseWork = caseDirection;
    caseWork++;
    /* Prevent agbcc from folding the second byte store into an offset store. */
    *(volatile u8 *)caseWork = caseDirection;
    caseSprite->pose = 18;
    goto done;

updateExisting:
    {
        register struct PrimarySpriteData *current PIN("r4");
        u8 *work;
        register u32 direction PIN("r1");

        current = &gCurrentSprite;
        current->pose = 18;
        work = &current->work1;
        direction = *work;
        if (direction == 2) {
            direction = 1;
            *work = direction;
            work++;
            goto storeWork2;
        }
        if (direction == 34) {
            direction = 17;
            *work = direction;
        } else if (direction == 18) {
            direction = 33;
            *work = direction;
        }
        work = (u8 *)current;
        work += 41;
storeWork2:;
        *work = direction;
    }

done:
    return;
}

void func_8028578(void)
{
    if (gCurrentSprite.work3 <= 31) {
        gCurrentSprite.work3++;
        gCurrentSprite.work0 += 2;
    } else {
        gCurrentSprite.pose = 16;
        gCurrentSprite.work2 = 0;
    }
}

void func_80285A4(void)
{
    s32 block;
    struct PrimarySpriteData *caseSprite;
    u8 *caseWork;
    register u32 caseDirection PIN("r1");

    block = GetBg2CollisionTypeAtTile(
        gCurrentSprite.yPosition >> 6,
        gCurrentSprite.xPosition >> 6
    );

    switch (block) {
        case 2:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 0;
            goto setDirection;
        case 1:
        case 3:
        case 4:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 2;
            goto setDirection;
        case 5:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 34;
            goto setDirection;
        case 6:
            caseSprite = &gCurrentSprite;
            caseWork = &caseSprite->work1;
            caseDirection = 18;
            goto setDirection;
        default:
            goto updateExisting;
    }

setDirection:
    *caseWork = caseDirection;
    caseWork++;
    /* Prevent agbcc from folding the second byte store into an offset store. */
    *(volatile u8 *)caseWork = caseDirection;
    caseSprite->pose = 20;
    goto done;

updateExisting:
    {
        register struct PrimarySpriteData *current PIN("r4");
        u8 *work;
        register u32 direction PIN("r1");

        current = &gCurrentSprite;
        current->pose = 20;
        work = &current->work1;
        direction = *work;
        if (direction == 1) {
            direction = 2;
            *work = direction;
            work++;
            goto storeWork2;
        }
        if (direction == 33) {
            direction = 18;
            *work = direction;
        } else if (direction == 17) {
            direction = 34;
            *work = direction;
        }
        work = (u8 *)current;
        work += 41;
storeWork2:;
        *work = direction;
    }

done:
    return;
}

void func_8028660(void)
{
    struct PrimarySpriteData *current;
    register u8 *work PIN("r1");
    register u32 value PIN("r0");

    current = &gCurrentSprite;
    work = (u8 *)current;
    work += 42;
    value = *work;
    if (value <= 31) {
        value++;
        *work = value;
        work -= 3;
        value = *work;
        value -= 2;
    } else {
        value = 0;
        current->pose = 16;
        work = (u8 *)current;
        work += 41;
    }
    *work = value;
}

void func_802868C(void)
{
    s32 block;

    block = GetBg2CollisionTypeAtTile(
        gCurrentSprite.yPosition >> 6,
        ((s32)gCurrentSprite.xPosition - 128) / 64
    );
    if (block == 1) {
        gCurrentSprite.warioCollision = 29;
    } else {
        gCurrentSprite.warioCollision = 28;
    }
}

void func_80286C0(void)
{
    s32 block;

    block = GetBg2CollisionTypeAtTile(
        gCurrentSprite.yPosition >> 6,
        ((s32)gCurrentSprite.xPosition + 128) / 64
    );
    if (block == 2) {
        gCurrentSprite.warioCollision = 29;
    } else {
        gCurrentSprite.warioCollision = 28;
    }
}

u32 func_80286EC(const s16 *movement)
{
    u8 index;
    s16 yMovement;
    s16 xMovement;

    index = gCurrentSprite.work3;
    yMovement = movement[index];
    if (yMovement == 0x7FFF) {
        return FALSE;
    }

    index++;
    xMovement = movement[index];
    gCurrentSprite.work3 = index + 1;
    gCurrentSprite.yPosition += yMovement;
    gCurrentSprite.xPosition += xMovement;

    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON) {
        gWarioData.yPosition += yMovement;
        gWarioData.xPosition += xMovement;
    }

    return TRUE;
}

void func_8028758(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE |
                             SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 8;
    gCurrentSprite.drawDistanceUp = 7;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 28;
    gCurrentSprite.hitboxExtentDown = 28;
    gCurrentSprite.hitboxExtentLeft = 56;
    gCurrentSprite.hitboxExtentRight = 52;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sRotatingPlatformOam_83B4544);
    gCurrentSprite.pose = 16;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.work1 = 0;
}

void func_80287CC(void)
{
    gCurrentSprite.warioCollision = 29;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = 50;
}

void func_80287E4(void)
{
    if (!func_80286EC(sUnk_83B32EC)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_95;
    }
}

void func_8028808(void)
{
    if (!func_80286EC(sUnk_83B34F4)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_95;
    }
}

void func_802882C(void)
{
    if (!func_80286EC(sUnk_83B33F0)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_97;
    }
}

void func_8028850(void)
{
    if (!func_80286EC(sUnk_83B31E8)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_97;
    }
}

void func_8028874(void)
{
    if (!func_80286EC(sUnk_83B326A)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_94;
        func_802868C();
    }
}

void func_802889C(void)
{
    if (!func_80286EC(sUnk_83B3472)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_96;
        func_80286C0();
    }
}

void func_80288C4(void)
{
    if (!func_80286EC(sUnk_83B336E)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_96;
        func_80286C0();
    }
}

void func_80288EC(void)
{
    if (!func_80286EC(sUnk_83B3576)) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.globalID = PSPRITE_94;
        func_802868C();
    }
}

void func_8028914(void)
{
    u32 temp;
    struct PrimarySpriteData *sprite;
    s32 i;
    struct PrimarySpriteData *current;
    u8 roomSlot;
    struct PrimarySpriteData *sprites;
    register u32 zero PIN("r6");

    temp = (u32)&gCurrentSprite;
    roomSlot = ((struct PrimarySpriteData *)temp)->roomSlot;
    i = 0;
    sprites = gSpriteData;
    current = (struct PrimarySpriteData *)temp;
    zero = 0;
loop:
    temp = 44;
    temp *= i;
    sprite = (struct PrimarySpriteData *)(temp + (u32)sprites);
    if (sprite->globalID != PSPRITE_96) {
        goto next;
    }
    temp = (u8)(sprite->pose - 49);
    if (temp > 1) {
        goto next;
    }
    temp = sprite->roomSlot;
    if (temp != roomSlot) {
        goto next;
    }
    {
        register u16 status PIN("r1");
        status = sprite->status;
        temp = SPRITE_STATUS_EXISTS;
        temp &= status;
    }
    if (temp == 0) {
        goto next;
    }
    current->warioCollision = 29;
    temp = (u32)current;
    temp += 42;
    *(u8 *)temp = zero;
    current->pose = 18;
    goto done;
next:
    i++;
    if (i <= 23) {
        goto loop;
    }
done:
    return;
}

void func_8028970(void)
{
    u32 temp;
    struct PrimarySpriteData *sprite;
    s32 i;
    struct PrimarySpriteData *current;
    u8 roomSlot;
    struct PrimarySpriteData *sprites;
    register u32 zero PIN("r6");

    temp = (u32)&gCurrentSprite;
    roomSlot = ((struct PrimarySpriteData *)temp)->roomSlot;
    i = 0;
    sprites = gSpriteData;
    current = (struct PrimarySpriteData *)temp;
    zero = 0;
loop:
    temp = 44;
    temp *= i;
    sprite = (struct PrimarySpriteData *)(temp + (u32)sprites);
    if (sprite->globalID != PSPRITE_94) {
        goto next;
    }
    temp = (u8)(sprite->pose - 49);
    if (temp > 1) {
        goto next;
    }
    temp = sprite->roomSlot;
    if (temp != roomSlot) {
        goto next;
    }
    {
        register u16 status PIN("r1");
        status = sprite->status;
        temp = SPRITE_STATUS_EXISTS;
        temp &= status;
    }
    if (temp == 0) {
        goto next;
    }
    current->warioCollision = 29;
    temp = (u32)current;
    temp += 42;
    *(u8 *)temp = zero;
    current->pose = 18;
    goto done;
next:
    i++;
    if (i <= 23) {
        goto loop;
    }
done:
    return;
}

void func_80289CC(void)
{
    u32 temp;
    register u16 status PIN("r1");
    struct PrimarySpriteData *sprite;
    s32 i;
    struct PrimarySpriteData *current;
    u32 zero;
    register u8 roomSlot PIN("r6");
    register u8 *work3 PIN("r12");
    struct PrimarySpriteData *sprites;

    temp = (u32)&gCurrentSprite;
    roomSlot = ((struct PrimarySpriteData *)temp)->roomSlot;
    i = 0;
    sprites = gSpriteData;
    current = (struct PrimarySpriteData *)temp;
    temp = 42;
    temp += (u32)current;
    work3 = (u8 *)temp;
    zero = 0;
loop:
    temp = 44;
    temp *= i;
    sprite = (struct PrimarySpriteData *)(temp + (u32)sprites);
    temp = (u8)(sprite->pose - 49);
    if (temp > 1) {
        goto next;
    }
    temp = sprite->roomSlot;
    if (temp != roomSlot) {
        goto next;
    }
    status = sprite->status;
    temp = SPRITE_STATUS_EXISTS;
    temp &= status;
    if (temp == 0) {
        goto next;
    }
    temp = sprite->globalID;
    if (temp == PSPRITE_94) {
        temp = (u32)work3;
        *(u8 *)temp = zero;
        temp = 18;
        current->pose = temp;
        goto done;
    }
    if (temp == PSPRITE_96) {
        temp = (u32)work3;
        *(u8 *)temp = zero;
        temp = 20;
        current->pose = temp;
    }
    goto done;
next:
    i++;
    if (i <= 23) {
        goto loop;
    }
done:
    return;
}

void func_8028A38(void)
{
    u32 temp;
    register u16 status PIN("r1");
    struct PrimarySpriteData *sprite;
    s32 i;
    struct PrimarySpriteData *current;
    register u32 zero PIN("r5");
    register u8 roomSlot PIN("r6");
    register u8 *work3 PIN("r12");
    struct PrimarySpriteData *sprites;

    temp = (u32)&gCurrentSprite;
    roomSlot = ((struct PrimarySpriteData *)temp)->roomSlot;
    i = 0;
    sprites = gSpriteData;
    current = (struct PrimarySpriteData *)temp;
    temp = 42;
    temp += (u32)current;
    work3 = (u8 *)temp;
    zero = 0;
loop:
    temp = 44;
    temp *= i;
    sprite = (struct PrimarySpriteData *)(temp + (u32)sprites);
    temp = (u8)(sprite->pose - 49);
    if (temp > 1) {
        goto next;
    }
    temp = sprite->roomSlot;
    if (temp != roomSlot) {
        goto next;
    }
    status = sprite->status;
    temp = SPRITE_STATUS_EXISTS;
    temp &= status;
    if (temp == 0) {
        goto next;
    }
    temp = sprite->globalID;
    if (temp == PSPRITE_94) {
        temp = (u32)work3;
        *(u8 *)temp = zero;
        temp = 17;
        goto setPoseAndSound;
    }
    if (temp == PSPRITE_96) {
        temp = (u32)work3;
        *(u8 *)temp = zero;
        temp = 19;
setPoseAndSound:
        current->pose = temp;
        m4aSongNumStart(SE_ROTATING_PLATFORM_TRIGGER);
    }
    goto done;
next:
    i++;
    if (i <= 23) {
        goto loop;
    }
done:
    return;
}

void SpriteRotatingPlatform(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    func_8028148();

    switch (gCurrentSprite.pose) {
        case 0:
            func_8028400();
            break;
        case 17:
            func_80284BC();
        case 18:
            func_8028578();
            break;
        case 19:
            func_80285A4();
        case 20:
            func_8028660();
            break;
        default:
            func_8028A38();
            break;
    }

    {
        const s16 *table;
        struct PrimarySpriteData *current;
        s32 trig0;
        s32 trig1;
        s32 trig2;
        s32 trig3;
        s32 scale;
        u32 index;
        s32 inverse;
        u8 *angle;

        table = sSinCosTable;
        current = &gCurrentSprite;
        angle = (u8 *)current;
        angle += 39;
        index = *angle;
        index += ANGLE_HALF_PI;
        index <<= 1;
        index += (u32)table;
        trig0 = *(s16 *)index;
        scale = 256;
        inverse = FixedInverse(scale);
        current->affinePA = FixedMul(trig0, inverse);
        index = *angle;
        index <<= 1;
        index += (u32)table;
        trig1 = *(s16 *)index;
        inverse = FixedInverse(scale);
        current->affinePB = FixedMul(trig1, inverse);
        index = *angle;
        index <<= 1;
        index += (u32)table;
        trig2 = -*(s16 *)index;
        inverse = FixedInverse(scale);
        current->affinePC = FixedMul(trig2, inverse);
        index = *angle;
        index += ANGLE_HALF_PI;
        index <<= 1;
        index += (u32)table;
        trig3 = *(s16 *)index;
        inverse = FixedInverse(scale);
        current->affinePD = FixedMul(trig3, inverse);
    }
}

void SpriteUnknown94(void)
{
    if (!SpriteUtilDespawnChildWithParent(PSPRITE_ROTATING_PLATFORM)) {
        func_802822C();
        switch (gCurrentSprite.pose) {
            case 0:
                func_8028758();
                func_802868C();
                break;
            case 49:
                func_80287CC();
                break;
            case 50:
                func_80287E4();
                break;
            case 18:
                func_802882C();
                break;
            default:
                func_8028914();
                break;
        }
        func_8026838();
    }
}

void SpriteUnknown96(void)
{
    if (!SpriteUtilDespawnChildWithParent(PSPRITE_ROTATING_PLATFORM)) {
        func_802822C();
        switch (gCurrentSprite.pose) {
            case 0:
                func_8028758();
                func_80286C0();
                break;
            case 49:
                func_80287CC();
                break;
            case 50:
                func_8028808();
                break;
            case 18:
                func_8028850();
                break;
            default:
                func_8028970();
                break;
        }
        func_8026838();
    }
}

void SpriteUnknown97(void)
{
    if (!SpriteUtilDespawnChildWithParent(PSPRITE_ROTATING_PLATFORM)) {
        func_802822C();
        switch (gCurrentSprite.pose) {
            case 0:
                func_8028758();
                gCurrentSprite.warioCollision = 29;
                break;
            case 18:
                func_8028874();
                break;
            case 20:
                func_802889C();
                break;
            default:
                func_80289CC();
                break;
        }
        func_8026838();
    }
}

void SpriteUnknown95(void)
{
    if (!SpriteUtilDespawnChildWithParent(PSPRITE_ROTATING_PLATFORM)) {
        func_802822C();
        switch (gCurrentSprite.pose) {
            case 0:
                func_8028758();
                gCurrentSprite.warioCollision = 29;
                break;
            case 18:
                func_80288C4();
                break;
            case 20:
                func_80288EC();
                break;
            default:
                func_80289CC();
                break;
        }
        func_8026838();
    }
}

void func_8028D2C(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND |
                             SPRITE_STATUS_AFFINE |
                             SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 32;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.hitboxExtentUp = 1;
    gCurrentSprite.hitboxExtentDown = 1;
    gCurrentSprite.hitboxExtentLeft = 1;
    gCurrentSprite.hitboxExtentRight = 1;
    GP_SET(gCurrentSprite.pOamData, sRotatingPlatformOam_83B4534);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.yPosition -= 32;
}

void SpriteRotatingPlatform2(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0) {
        func_8028D2C();
        SpriteSpawnAsChild(PSPRITE_AE, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                           gCurrentSprite.yPosition, gCurrentSprite.xPosition + 128);
        SpriteSpawnAsChild(PSPRITE_AF, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                           gCurrentSprite.yPosition, gCurrentSprite.xPosition - 128);
    } else {
        if (gCurrentSprite.work0 == 0) {
            m4aSongNumStart(SE_ROTATING_PLATFORM_ROTATE);
        }
        gCurrentSprite.work0 += 2;
    }
    {
        struct PrimarySpriteData *current;
        register s32 angleOrSine PIN("r4");
        const s16 *table;
        s32 cosine;
        s32 scale;
        u32 index;
        s32 inverse;

        current = &gCurrentSprite;
        angleOrSine = current->work0;
        table = sSinCosTable;
        index = angleOrSine;
        index += ANGLE_HALF_PI;
        index <<= 1;
        index += (u32)table;
        cosine = *(s16 *)index;
        scale = 256;
        inverse = FixedInverse(scale);
        current->affinePA = FixedMul(cosine, inverse);
        angleOrSine = table[angleOrSine];
        inverse = FixedInverse(scale);
        current->affinePB = FixedMul(angleOrSine, inverse);
        angleOrSine = -angleOrSine;
        inverse = FixedInverse(scale);
        current->affinePC = FixedMul(angleOrSine, inverse);
        inverse = FixedInverse(scale);
        current->affinePD = FixedMul(cosine, inverse);
    }
}

void SpriteRotatingPlatform3(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0) {
        func_8028D2C();
        SpriteSpawnAsChild(PSPRITE_B0, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                           gCurrentSprite.yPosition, gCurrentSprite.xPosition + 128);
        SpriteSpawnAsChild(PSPRITE_B1, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                           gCurrentSprite.yPosition, gCurrentSprite.xPosition - 128);
    } else {
        if (gCurrentSprite.work0 == 0) {
            m4aSongNumStart(SE_ROTATING_PLATFORM_ROTATE);
        }
        gCurrentSprite.work0 -= 2;
    }
    {
        struct PrimarySpriteData *current;
        register s32 angleOrSine PIN("r4");
        const s16 *table;
        s32 cosine;
        s32 scale;
        u32 index;
        s32 inverse;

        current = &gCurrentSprite;
        angleOrSine = current->work0;
        table = sSinCosTable;
        index = angleOrSine;
        index += ANGLE_HALF_PI;
        index <<= 1;
        index += (u32)table;
        cosine = *(s16 *)index;
        scale = 256;
        inverse = FixedInverse(scale);
        current->affinePA = FixedMul(cosine, inverse);
        angleOrSine = table[angleOrSine];
        inverse = FixedInverse(scale);
        current->affinePB = FixedMul(angleOrSine, inverse);
        angleOrSine = -angleOrSine;
        inverse = FixedInverse(scale);
        current->affinePC = FixedMul(angleOrSine, inverse);
        inverse = FixedInverse(scale);
        current->affinePD = FixedMul(cosine, inverse);
    }
}

void SpriteUnknownAE(void)
{
    if (!SpriteUtilDespawnChildWithParent(85)) {
        if (gCurrentSprite.pose == 0) {
            func_8028758();
            gCurrentSprite.warioCollision = 29;
            gCurrentSprite.work3 = 64;
        }
        func_802655C();
        func_8026838();
    }
}

void SpriteUnknownAF(void)
{
    if (!SpriteUtilDespawnChildWithParent(85)) {
        u8 pose;
        pose = gCurrentSprite.pose;
        if (pose == 0) {
            func_8028758();
            gCurrentSprite.warioCollision = 29;
            gCurrentSprite.work3 = pose;
        }
        func_802655C();
        func_8026838();
    }
}

void SpriteUnknownB0(void)
{
    if (!SpriteUtilDespawnChildWithParent(86)) {
        u8 pose;
        pose = gCurrentSprite.pose;
        if (pose == 0) {
            func_8028758();
            gCurrentSprite.warioCollision = 29;
            gCurrentSprite.work3 = pose;
        }
        func_80264A0();
        func_8026838();
    }
}

void SpriteUnknownB1(void)
{
    if (!SpriteUtilDespawnChildWithParent(86)) {
        if (gCurrentSprite.pose == 0) {
            func_8028758();
            gCurrentSprite.warioCollision = 29;
            gCurrentSprite.work3 = 64;
        }
        func_80264A0();
        func_8026838();
    }
}

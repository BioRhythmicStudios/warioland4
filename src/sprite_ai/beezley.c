#include "sprite_ai/beezley.h"

#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "wario.h"

s32 func_804012C(void)
{
    u16 currentTop;
    u16 currentBottom;
    u16 currentLeft;
    u16 currentRight;
    s32 i;

    {
        struct PrimarySpriteData *sprite;
        u16 yPosition;
        u16 xPosition;

        sprite = &gCurrentSprite;
        yPosition = (u16)(sprite->yPosition + 0x1000);
        xPosition = (u16)(sprite->xPosition + 0x1000);
        currentTop = (u16)(yPosition - sprite->hitboxExtentUp);
        currentBottom = (u16)(yPosition + sprite->hitboxExtentDown);
        currentLeft = (u16)(xPosition - sprite->hitboxExtentLeft);
        currentRight = (u16)(xPosition + sprite->hitboxExtentRight);
    }

    i = 0;
    do {
        struct PrimarySpriteData *sprite;

        {
            struct PrimarySpriteData *sprites;
            register u32 spriteOffset PIN("r0");

            sprites = gSpriteData;
            spriteOffset = sizeof(*sprite);
            spriteOffset *= i;
            spriteOffset += (u32)sprites;
            sprite = (struct PrimarySpriteData *)spriteOffset;
        }
        if ((sprite->status & SPRITE_STATUS_EXISTS) &&
            sprite->globalID == 0x41 && sprite->work1 == 1) {
            u16 top;
            u16 bottom;
            u16 left;
            u16 right;
            u16 yPosition;
            u16 xPosition;

            yPosition = (u16)(sprite->yPosition + 0x1000);
            xPosition = (u16)(sprite->xPosition + 0x1000);
            top = (u16)(yPosition - sprite->hitboxExtentUp);
            bottom = (u16)(yPosition + sprite->hitboxExtentDown);
            left = (u16)(xPosition - sprite->hitboxExtentLeft);
            right = (u16)(xPosition + sprite->hitboxExtentRight);

            if (SpriteCollisionCheckObjectsTouching(
                    currentTop,
                    currentBottom,
                    currentLeft,
                    currentRight,
                    top,
                    bottom,
                    left,
                    right)) {
                m4aSongNumStart(SOUND_51);
                gCurrentSprite.yPosition += 20;
                return 1;
            }
        }
        i++;
    } while (i < MAX_SPRITE_COUNT);

    return 0;
}

void func_8040238(void)
{
    u16 currentTop;
    u16 currentBottom;
    u16 currentLeft;
    u16 currentRight;
    s32 i;

    {
        struct PrimarySpriteData *sprite;
        u16 yPosition;
        u16 xPosition;

        sprite = &gCurrentSprite;
        yPosition = (u16)(sprite->yPosition + 0x1000);
        xPosition = (u16)(sprite->xPosition + 0x1000);
        currentTop = (u16)(yPosition - sprite->hitboxExtentUp);
        currentBottom = (u16)(yPosition + sprite->hitboxExtentDown);
        currentLeft = (u16)(xPosition - sprite->hitboxExtentLeft);
        currentRight = (u16)(xPosition + sprite->hitboxExtentRight);
    }

    i = 0;
    do {
        struct PrimarySpriteData *sprite;

        {
            struct PrimarySpriteData *sprites;
            register u32 spriteOffset PIN("r0");

            sprites = gSpriteData;
            spriteOffset = sizeof(*sprite);
            spriteOffset *= i;
            spriteOffset += (u32)sprites;
            sprite = (struct PrimarySpriteData *)spriteOffset;
        }
        if ((sprite->status & SPRITE_STATUS_EXISTS) &&
            sprite->globalID == 0x41 && sprite->work1 == 1) {
            u16 top;
            u16 bottom;
            u16 left;
            u16 right;
            u16 yPosition;
            u16 xPosition;

            yPosition = (u16)(sprite->yPosition + 0x1000);
            xPosition = (u16)(sprite->xPosition + 0x1000);
            top = (u16)(yPosition - sprite->hitboxExtentUp);
            bottom = (u16)(yPosition + sprite->hitboxExtentDown);
            left = (u16)(xPosition - sprite->hitboxExtentLeft);
            right = (u16)(xPosition + sprite->hitboxExtentRight);

            if (SpriteCollisionCheckObjectsTouching(
                    currentTop,
                    currentBottom,
                    currentLeft,
                    currentRight,
                    top,
                    bottom,
                    left,
                    right)) {
                sprite->pose = 17;
                return;
            }
        }
        i++;
    } while (i < MAX_SPRITE_COUNT);
}


void func_8040338(void)
{
    gCurrentSprite.warioCollision = 49;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 8;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 8;
    gCurrentSprite.hitboxExtentLeft = 24;
    gCurrentSprite.hitboxExtentRight = 20;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.work1 = 70;
    gCurrentSprite.yPosition -= 32;
    SpriteUtilTurnTowardWario();
}


void func_8040398(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C62FC);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.status &= ~SPRITE_STATUS_MAYBE_DEAD;
}


void func_80403C0(void)
{
    u32 work2;
    u16 warioY;
    u16 warioX;
    u16 spriteY;
    u16 spriteX;

    work2 = 8;
    warioY = gWarioData.yPosition;
    warioX = gWarioData.xPosition;
    spriteY = gCurrentSprite.yPosition;
    spriteX = gCurrentSprite.xPosition;

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition = spriteX + 2;
        func_8023BFC(
            (u16)(gCurrentSprite.yPosition - 32),
            (u16)(gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight)
        );
        if (gSpriteCollisionTileType & 0xF) {
            gCurrentSprite.pose = 17;
            return;
        }

        gCurrentSprite.work1--;
        if (gCurrentSprite.work1 == 0) {
            gCurrentSprite.pose = 17;
        }

        if (warioY > spriteY && warioY - spriteY <= 447 && warioY - spriteY > 128 &&
            warioX > spriteX && warioX - spriteX <= 383) {
            if (warioX - spriteX <= 127) {
                work2 = 2;
            } else if (warioX - spriteX <= 255) {
                work2 = 6;
            }
            gCurrentSprite.work2 = work2;
            gCurrentSprite.pose = 110;
        }
    } else {
        gCurrentSprite.xPosition = spriteX - 2;
        func_8023BFC(
            (u16)(gCurrentSprite.yPosition - 32),
            (u16)(gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft)
        );
        if (gSpriteCollisionTileType & 0xF) {
            gCurrentSprite.pose = 17;
            return;
        }

        gCurrentSprite.work1--;
        if (gCurrentSprite.work1 == 0) {
            gCurrentSprite.pose = 17;
        }

        if (warioY > spriteY && warioY - spriteY <= 447 && warioY - spriteY > 128 &&
            spriteX > warioX && spriteX - warioX <= 383) {
            if (spriteX - warioX <= 127) {
                work2 = 2;
            } else if (spriteX - warioX <= 255) {
                work2 = 6;
            }
            gCurrentSprite.work2 = work2;
            gCurrentSprite.pose = 110;
        }
    }

    {
        u8 index;
        s16 movement;

        index = gCurrentSprite.work3;
        movement = sUnk_83C6594[index];
        if (movement == 0x7FFF) {
            movement = sUnk_83C6594[0];
            index = 0;
        }
        gCurrentSprite.work3 = index + 1;
        gCurrentSprite.yPosition += movement;
    }
}


void func_8040538(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C6384);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 24;
    m4aSongNumStart(SOUND_4F);
}

void func_8040564(void)
{
    u8 index;
    s16 movement;

    index = gCurrentSprite.work3;
    movement = sUnk_83C6594[index];
    if (movement == 0x7FFF) {
        movement = sUnk_83C6594[0];
        index = 0;
    }
    gCurrentSprite.work3 = index + 1;
    gCurrentSprite.yPosition += movement;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 19;
    }
}

void func_80405B8(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C63EC);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 20;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
}

void func_80405E4(void)
{
    u8 index;
    s16 movement;

    index = gCurrentSprite.work3;
    movement = sUnk_83C6594[index];
    if (movement == 0x7FFF) {
        movement = sUnk_83C6594[0];
        index = 0;
    }
    gCurrentSprite.work3 = index + 1;
    gCurrentSprite.yPosition += movement;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 15;
        gCurrentSprite.work1 = 140;
    }
}

void func_8040640(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C651C);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 180;
    gCurrentSprite.warioCollision = 50;
}

void func_8040668(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        if (gCurrentSprite.xPosition > gWarioData.xPosition) {
            gCurrentSprite.pose = 35;
        } else {
            gCurrentSprite.pose = 37;
        }
        func_8040238();
    }
}

void func_80406A0(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C6424);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work0 = 16;
    m4aSongNumStart(SOUND_50);
}

void func_80406CC(void)
{
    u8 index;
    s16 movement;

    index = gCurrentSprite.work3;
    movement = sUnk_83C6594[index];
    if (movement == 0x7FFF) {
        movement = sUnk_83C6594[0];
        index = 0;
    }
    gCurrentSprite.work3 = index + 1;
    gCurrentSprite.yPosition += movement;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 113;
    }
}

void func_8040720(void)
{
    struct PrimarySpriteData *sprite;
    register u32 xPosition PIN("r0");

    if (func_804012C()) {
        gCurrentSprite.pose = 23;
        return;
    }

    sprite = &gCurrentSprite;
    if (sprite->yPosition > gWarioData.yPosition - 110 || sprite->work0 > 239) {
        goto setPose;
    }

    {
        u32 yPosition;

        yPosition = func_8023A60(sprite->yPosition, sprite->xPosition);
        if (gBackgroundCollisionData.waterType == 1) {
            goto setPose;
        }
        if (gSpriteCollisionResult & 0xF) {
            sprite->yPosition = yPosition;
            goto setPose;
        }
    }

    {
        u16 oldY;

        sprite->work0++;
        oldY = sprite->yPosition;
        sprite->yPosition = oldY + 8;
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(
                (u16)(oldY - 24),
                (u16)(sprite->xPosition + sprite->hitboxExtentRight)
            );
            if (gSpriteCollisionTileType & 0xF) {
                goto setPose;
            }
            {
                u8 *workPtr;

                workPtr = &sprite->work2;
                xPosition = sprite->xPosition;
                xPosition += *workPtr;
            }
            goto updateX;
        }

        func_8023BFC(
            (u16)(oldY - 24),
            (u16)(sprite->xPosition - sprite->hitboxExtentLeft)
        );
        if (gSpriteCollisionTileType & 0xF) {
            goto setPose;
        }
        goto computeLeftX;
    }

setPose:
    sprite->pose = 115;
    return;

computeLeftX:
    {
        u8 *workPtr;
        u32 amount;

        workPtr = &sprite->work2;
        amount = *workPtr;
        xPosition = sprite->xPosition;
        xPosition -= amount;
    }

updateX:
    sprite->xPosition = xPosition;
}

void func_8040818(void)
{
    func_8023BFC(
        (u16)(gCurrentSprite.yPosition - 96),
        gCurrentSprite.xPosition
    );
    if (gCurrentSprite.work0 == 0 || (gSpriteCollisionTileType & 0xF)) {
        gCurrentSprite.pose = 117;
        gCurrentSprite.work0 = 16;
        return;
    }

    gCurrentSprite.work0--;
    {
        u16 oldY;

        oldY = gCurrentSprite.yPosition;
        gCurrentSprite.yPosition = oldY - 8;
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(
                (u16)(oldY - 40),
                (u16)(gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight)
            );
            if (!(gSpriteCollisionTileType & 0xF)) {
                gCurrentSprite.xPosition += gCurrentSprite.work2;
            }
        } else {
            func_8023BFC(
                (u16)(oldY - 40),
                (u16)(gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft)
            );
            if (!(gSpriteCollisionTileType & 0xF)) {
                gCurrentSprite.xPosition -= gCurrentSprite.work2;
            }
        }
    }
}

void func_80408D0(void)
{
    u8 index;
    s16 movement;

    index = gCurrentSprite.work3;
    movement = sUnk_83C6594[index];
    if (movement == 0x7FFF) {
        movement = sUnk_83C6594[0];
        index = 0;
    }
    gCurrentSprite.work3 = index + 1;
    gCurrentSprite.yPosition += movement;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            if (gCurrentSprite.xPosition > gWarioData.xPosition) {
                gCurrentSprite.work1 = 16;
            }
        } else {
            if (gCurrentSprite.xPosition < gWarioData.xPosition) {
                gCurrentSprite.work1 = 16;
            }
        }
        gCurrentSprite.pose = 15;
    }
}

void func_8040954(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C646C);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 8;
}

void func_8040970(void)
{
    gCurrentSprite.pose = 40;
    func_8040954();
}

void func_8040984(void)
{
    gCurrentSprite.pose = 42;
    func_8040954();
}

void func_8040998(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 35;
    }
}

void func_80409B4(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 37;
    }
}

void func_80409D0(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C6494);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 24;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 49;
}

void func_8040A00(void)
{
    gCurrentSprite.pose = 36;
    func_80409D0();
}

void func_8040A14(void)
{
    gCurrentSprite.pose = 38;
    func_80409D0();
}

void func_8040A28(void)
{
    u8 index;
    s16 movement;

    func_8023BFC((u16)(gCurrentSprite.yPosition - 96), gCurrentSprite.xPosition);
    if (gSpriteCollisionTileType & 0xF) {
        gCurrentSprite.pose = 15;
        return;
    }

    index = gCurrentSprite.work3;
    movement = sUnk_83C6616[index];
    if (movement == 0x7FFF) {
        register u32 tableOffset PIN("r1");
        u32 yPosition;

        tableOffset = index - 1;
        tableOffset *= 2;
        tableOffset += (u32)sUnk_83C6616;
        yPosition = gCurrentSprite.yPosition;
        yPosition += *(u16 *)tableOffset;
        gCurrentSprite.yPosition = yPosition;
    } else {
        gCurrentSprite.work3 = index + 1;
        gCurrentSprite.yPosition += movement;
    }

    func_8023BFC(
        (u16)(gCurrentSprite.yPosition - 32),
        (u16)(gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight)
    );
    if (gSpriteCollisionTileType & 0xF) {
        gCurrentSprite.pose = 15;
        return;
    }

    gCurrentSprite.xPosition += 6;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 15;
    }
}

void func_8040ADC(void)
{
    u8 index;
    s16 movement;

    func_8023BFC((u16)(gCurrentSprite.yPosition - 96), gCurrentSprite.xPosition);
    if (gSpriteCollisionTileType & 0xF) {
        gCurrentSprite.pose = 15;
        return;
    }

    index = gCurrentSprite.work3;
    movement = sUnk_83C6616[index];
    if (movement == 0x7FFF) {
        register u32 tableOffset PIN("r1");
        u32 yPosition;

        tableOffset = index - 1;
        tableOffset *= 2;
        tableOffset += (u32)sUnk_83C6616;
        yPosition = gCurrentSprite.yPosition;
        yPosition += *(u16 *)tableOffset;
        gCurrentSprite.yPosition = yPosition;
    } else {
        gCurrentSprite.work3 = index + 1;
        gCurrentSprite.yPosition += movement;
    }

    func_8023BFC(
        (u16)(gCurrentSprite.yPosition - 32),
        (u16)(gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft)
    );
    if (gSpriteCollisionTileType & 0xF) {
        gCurrentSprite.pose = 15;
        return;
    }

    gCurrentSprite.xPosition -= 6;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 15;
    }
}

void func_8040B90(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C64EC);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition, gCurrentSprite.xPosition, SSPRITE_06);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void func_8040BE0(void)
{
    gCurrentSprite.pose = 32;
    func_8040B90();
}

void func_8040BF4(void)
{
    gCurrentSprite.pose = 34;
    func_8040B90();
}

void func_8040C08(void)
{
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C64EC);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition, gCurrentSprite.xPosition, SSPRITE_06);
}

void func_8040C40(void)
{
    gCurrentSprite.pose = 32;
    func_8040C08();
}

void func_8040C54(void)
{
    gCurrentSprite.pose = 34;
    func_8040C08();
}

void func_8040C68(void)
{
    gCurrentSprite.pose = 50;
    GP_SET(gCurrentSprite.pOamData, sBeezleyOam_83C64BC);
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 10;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void func_8040CA0(void)
{
    SpriteUtilDie();
}

void SpriteBeezley(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            func_8040338();
        case 15:
            func_8040398();
        case 16:
            func_80403C0();
            break;

        case 17:
            func_8040538();
        case 18:
            func_8040564();
            break;

        case 19:
            func_80405B8();
        case 20:
            func_80405E4();
            break;

        case 23:
            func_8040640();
        case 24:
            func_8040668();
            break;

        case 110:
            func_80406A0();
        case 111:
            func_80406CC();
            break;

        case 113:
            func_8040720();
            break;

        case 115:
            func_8040818();
            break;

        case 117:
            func_80408D0();
            break;

        case 39:
            func_8040970();
        case 40:
            func_8040998();
            break;

        case 41:
            func_8040984();
        case 42:
            func_80409B4();
            break;

        case 31:
        case 79:
            func_8040BE0();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;

        case 33:
        case 80:
            func_8040BF4();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;

        case 35:
        case 51:
            func_8040A00();
        case 36:
            func_8040A28();
            break;

        case 37:
        case 53:
            func_8040A14();
        case 38:
            func_8040ADC();
            break;

        case 49:
            func_8040C68();
        case 50:
            SpriteUtilDieAfterDelay();
            break;

        case 105:
            func_8040C54();
            break;

        case 106:
            func_8040C40();
            break;

        default:
            func_8040CA0();
            break;
    }
}

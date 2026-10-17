#include "sprite_ai/sprite_a9.h"

#include "score.h"
#include "sound.h"
#include "sprite_util.h"

void func_805FB24(void)
{
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.warioCollision = 33;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E1F30);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work1 = 42;
    SpriteUtilTurnTowardWario();
}

void func_805FB94(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E1F30);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 33;
    gCurrentSprite.work1 = 42;
}

void func_805FBB8(void)
{
    u32 nearby;

    nearby = SpriteUtilWaitCheckWarioNearbyLeftRight(56, 320);
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition,
                gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (gSpriteCollisionTileType == 0) {
            gCurrentSprite.pose = 27;
            return;
        }
    } else if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        if (nearby == NS_RIGHT) {
            gCurrentSprite.pose = 110;
            return;
        }
        if (nearby == NS_LEFT) {
            gCurrentSprite.pose = 17;
            return;
        }
        if (gSpriteCollisionResult & 0xF0) {
            if (((gCurrentSprite.xPosition & 0x3F) + gCurrentSprite.hitboxExtentRight) > 0x3F) {
                func_8023BFC(gCurrentSprite.yPosition,
                    gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
                if (gSpriteCollisionTileType & 0xF0) {
                    func_8023BFC(gCurrentSprite.yPosition - 32,
                        gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
                    if (gSpriteCollisionTileType & 0xF) {
                        gCurrentSprite.pose = 17;
                        return;
                    }
                } else {
                    gCurrentSprite.pose = 17;
                    return;
                }
            }
        }
    } else {
        if (nearby == NS_LEFT) {
            gCurrentSprite.pose = 110;
            return;
        }
        if (nearby == NS_RIGHT) {
            gCurrentSprite.pose = 17;
            return;
        }
        if (gSpriteCollisionResult & 0xF0) {
            if ((gCurrentSprite.xPosition & 0x3F) < gCurrentSprite.hitboxExtentLeft) {
                func_8023BFC(gCurrentSprite.yPosition,
                    gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
                if (gSpriteCollisionTileType & 0xF0) {
                    func_8023BFC(gCurrentSprite.yPosition - 32,
                        gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
                    if (gSpriteCollisionTileType & 0xF) {
                        gCurrentSprite.pose = 17;
                        return;
                    }
                } else {
                    gCurrentSprite.pose = 17;
                    return;
                }
            }
        }
    }
    func_8026374();
}

void func_805FD04(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2130);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 13;
    gCurrentSprite.warioCollision = 33;
}

void func_805FD2C(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            if (gCurrentSprite.pose == 18) {
                gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
                GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2150);
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                gCurrentSprite.pose = 20;
                gCurrentSprite.work0 = 16;
            } else {
                GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E1F30);
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                gCurrentSprite.pose = 16;
                gCurrentSprite.warioCollision = 33;
            }
        }
    }
}

void func_805FDAC(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5)
        gCurrentSprite.warioCollision = 5;
}

void func_805FDE4(void)
{
    gCurrentSprite.pose = 52;
    func_805FDAC();
}

void func_805FDF8(void)
{
    gCurrentSprite.pose = 54;
    func_805FDAC();
}

void func_805FE0C(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E1FA0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work0 = 57;
    gCurrentSprite.warioCollision = 33;
}

void func_805FE34(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    register u32 old PIN("r0");
    register u32 next PIN("r2");
    u32 timer;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    old = *work0;
    next = old - 1;
    *work0 = next;
    old = next << 24;
    timer = old >> 24;
    if (timer != 0) {
        register u32 check PIN("r0");
        check = timer;
        /* Keep the countdown comparison in r0 instead of coalescing it with r1. */
        ASM_BARRIER(: "+r"(check));
        if (check == 42)
            sprite->warioCollision = 3;
        func_80238A4();
        func_8023B88();
        if (gSpriteCollisionResult == 0)
            sprite->pose = 27;
    } else {
        GP_SET(sprite->pOamData, sSpriteA9Oam_83E1FE8);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 113;
        m4aSongNumStart(SE_TOTSUMEN_CHARGE);
    }
}

void func_805FE90(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC(gCurrentSprite.yPosition,
            gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        if (!(gSpriteCollisionTileType & 0xF0)) {
            func_8023B88();
            if (gSpriteCollisionResult == 0) {
                gCurrentSprite.pose = 70;
                gCurrentSprite.work2 = 12;
                gCurrentSprite.work3 = 0;
                gCurrentSprite.xPosition += 8;
                return;
            }
        }
        func_8023BFC(gCurrentSprite.yPosition - 32,
            gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        if (gSpriteCollisionTileType & 0xF) {
            if (gSpriteCollisionTileType == 17) {
                gCurrentSprite.pose = 114;
                m4aSongNumStart(SE_TOTSUMEN_HIT_WALL);
            } else {
                gCurrentSprite.pose = 51;
            }
        } else {
            gCurrentSprite.xPosition += 12;
        }
    } else {
        func_8023BFC(gCurrentSprite.yPosition,
            gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        if (!(gSpriteCollisionTileType & 0xF0)) {
            func_8023B88();
            if (gSpriteCollisionResult == 0) {
                gCurrentSprite.pose = 68;
                gCurrentSprite.work2 = 12;
                gCurrentSprite.work3 = 0;
                gCurrentSprite.xPosition -= 8;
                return;
            }
        }
        func_8023BFC(gCurrentSprite.yPosition - 32,
            gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        if (gSpriteCollisionTileType & 0xF) {
            if (gSpriteCollisionTileType == 17) {
                gCurrentSprite.pose = 114;
                m4aSongNumStart(SE_TOTSUMEN_HIT_WALL);
            } else {
                gCurrentSprite.pose = 53;
            }
        } else {
            gCurrentSprite.xPosition -= 12;
        }
    }
}

void func_805FFD4(void)
{
    if (gCurrentSprite.warioCollision == 3) {
        GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.warioCollision = 5;
    }
    m4aSongNumStart(SE_TOTSUMEN_IMPACT);
}

void func_8060000(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2020);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 115;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.work3 = 0;
}

void func_806002C(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u8 *work3;
    u32 index;
    const s16 *velocities;
    u32 velocityAddress;
    u32 rawVelocity;
    u32 zero;
    s16 velocity;
    u32 collisionY;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    *work0 += 1;
    work3 = &sprite->work3;
    index = *work3;
    velocities = sUnk_83E22C8;
    velocityAddress = index << 1;
    velocityAddress += (u32)velocities;
    rawVelocity = *(const u16 *)velocityAddress;
    zero = 0;
    velocity = *(const s16 *)(velocityAddress + zero);
    if (velocity == 0x7FFF) {
        u32 previous;
        register u32 yPosition PIN("r0");
        previous = index - 1;
        previous <<= 1;
        previous += (u32)velocities;
        yPosition = sprite->yPosition;
        yPosition += *(const u16 *)previous;
        sprite->yPosition = yPosition;
    } else {
        u32 nextIndex;
        u8 *storeWork3;
        nextIndex = index + 1;
        storeWork3 = work3;
        *storeWork3 = nextIndex;
        nextIndex = sprite->yPosition;
        nextIndex += rawVelocity;
        sprite->yPosition = nextIndex;
    }

    if (gCurrentSprite.work0 > 16) {
        collisionY = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
        if (gBackgroundCollisionData.waterType == 1)
            gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
        if (gSpriteCollisionResult != 0) {
            gCurrentSprite.yPosition = collisionY;
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = 54;
            else
                gCurrentSprite.pose = 52;
            GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
            gCurrentSprite.animationTimer = 0;
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.work0 = 4;
            gCurrentSprite.work2 = 8;
            gCurrentSprite.work3 = 0;
            gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
            m4aSongNumStart(SE_TOTSUMEN_IMPACT);
            return;
        }
    } else {
        func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp,
            gCurrentSprite.xPosition);
        if (gSpriteCollisionTileType & 0xF) {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = 67;
            else
                gCurrentSprite.pose = 69;
        }
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
        gCurrentSprite.xPosition -= 8;
    else
        gCurrentSprite.xPosition += 8;
}

void func_8060168(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E21C0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.warioCollision = 33;
}

void func_8060190(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E20C0);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 84;
}

void func_80601B4(void)
{
    u32 timer;
    register u32 original PIN("r3");

    timer = gCurrentSprite.work0;
    original = timer;
    if ((u8)(timer - 28) > 28) {
        func_8023CD8();
        return;
    }
    if (timer == 56 || timer == 49) {
        gCurrentSprite.yPosition -= 32;
    } else if (timer == 42) {
        gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
        GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E20F8);
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.yPosition += 32;
    } else if (timer == 35) {
        gCurrentSprite.yPosition += 32;
    } else if (original == 28) {
        gCurrentSprite.warioCollision = 33;
    }
    gCurrentSprite.work0--;
}

void func_806022C(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E1F68);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
}

void func_806024C(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void func_8060274(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2238);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, SSPRITE_06);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void func_80602C4(void)
{
    gCurrentSprite.pose = 32;
    func_8060274();
}

void func_80602D8(void)
{
    gCurrentSprite.pose = 34;
    func_8060274();
}

void func_80602EC(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2238);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, SSPRITE_06);
}

void func_8060328(void)
{
    gCurrentSprite.pose = 32;
    func_80602EC();
}

void func_806033C(void)
{
    gCurrentSprite.pose = 34;
    func_80602EC();
}

void func_8060350(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E21A8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
}

void func_8060384(void)
{
    gCurrentSprite.pose = 36;
    func_8060350();
}

void func_8060398(void)
{
    gCurrentSprite.pose = 38;
    func_8060350();
}

void func_80603AC(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2208);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void func_80603DC(void)
{
    gCurrentSprite.pose = 40;
    func_80603AC();
}

void func_80603F0(void)
{
    gCurrentSprite.pose = 42;
    func_80603AC();
}

void func_8060404(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void func_8060428(void)
{
    gCurrentSprite.pose = 44;
    func_8060404();
}

void func_806043C(void)
{
    gCurrentSprite.pose = 46;
    func_8060404();
}

void func_8060450(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E21F0);
        gCurrentSprite.work0 = 7;
    } else {
        GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E21D0);
        gCurrentSprite.work0 = 8;
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 50;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void func_806049C(void)
{
    SpriteUtilDie();
}

void func_80604A8(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void func_80604D4(void)
{
    gCurrentSprite.pose = 72;
    func_80604A8();
}

void func_80604E8(void)
{
    gCurrentSprite.pose = 74;
    func_80604A8();
}

void func_80604FC(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2068);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 19;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 33;
}

void func_806052C(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 timer;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    timer = *work0;
    timer -= 1;
    *work0 = timer;
    timer <<= 24;
    if (timer != 0) {
        func_8023BFC(sprite->yPosition - sprite->hitboxExtentUp,
            sprite->xPosition);
        if (gSpriteCollisionTileType & 0xF) {
            sprite->pose = 29;
        } else {
            u8 *work3;
            u32 index;
            const s16 *velocities;
            u32 velocityAddress;
            u32 rawVelocity;
            u32 zero;
            s16 velocity;

            work3 = &sprite->work3;
            index = *work3;
            velocities = sUnk_8352B18;
            velocityAddress = index << 1;
            velocityAddress += (u32)velocities;
            rawVelocity = *(const u16 *)velocityAddress;
            zero = 0;
            velocity = *(const s16 *)(velocityAddress + zero);
            if (velocity == 0x7FFF) {
                u32 previous;
                register u32 yPosition PIN("r0");
                previous = index - 1;
                previous <<= 1;
                previous += (u32)velocities;
                yPosition = sprite->yPosition;
                yPosition += *(const u16 *)previous;
                sprite->yPosition = yPosition;
            } else {
                u32 nextIndex;
                u8 *storeWork3;
                nextIndex = index + 1;
                storeWork3 = work3;
                *storeWork3 = nextIndex;
                nextIndex = sprite->yPosition;
                nextIndex += rawVelocity;
                sprite->yPosition = nextIndex;
            }
        }
    } else {
        sprite->pose = 27;
    }
}

void func_80605B8(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void func_80605D0(void)
{
    gCurrentSprite.pose = 82;
    func_80605B8();
}

void func_80605E4(void)
{
    gCurrentSprite.pose = 84;
    func_80605B8();
}

void func_80605F8(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sSpriteA9Oam_83E2088);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void func_8060614(void)
{
    gCurrentSprite.pose = 88;
    func_80605F8();
}

void func_8060628(void)
{
    gCurrentSprite.pose = 86;
    func_80605F8();
}

void SpriteUnknownA9(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER)
        gCurrentSprite.pose = 107;

    switch (gCurrentSprite.pose) {
        case 0:
            func_805FB24();
            break;
        case 15:
            func_805FB94();
        case 16:
            func_805FBB8();
            break;
        case 17:
            func_805FD04();
        case 18:
        case 20:
            func_805FD2C();
            break;
        case 110:
            func_805FE0C();
        case 111:
            func_805FE34();
            break;
        case 113:
            func_805FE90();
            break;
        case 114:
            func_8060000();
        case 115:
            func_806002C();
            break;
        case 23:
            func_8060168();
        case 24:
            func_8023C94();
            break;
        case 25:
            func_8060190();
        case 26:
            func_80601B4();
            break;
        case 27:
            func_806022C();
        case 28:
            func_8023D48();
            break;
        case 29:
            func_806024C();
        case 30:
            func_8023EE0();
            break;
        case 31:
        case 79:
            func_80602C4();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            func_80602D8();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
            func_8060384();
        case 36:
            SpriteUtilPushedRight();
            break;
        case 37:
            func_8060398();
        case 38:
            SpriteUtilPushedLeft();
            break;
        case 39:
            func_80603DC();
        case 40:
            SpriteUtilPushedRight();
            func_80238E8();
            break;
        case 41:
            func_80603F0();
        case 42:
            SpriteUtilPushedLeft();
            func_80238E8();
            break;
        case 43:
            func_8060428();
        case 44:
            func_8024478();
            break;
        case 45:
            func_806043C();
        case 46:
            func_802449C();
            break;
        case 47:
            func_80244C0();
        case 48:
            func_80244E0();
            break;
        case 49:
            func_8060450();
        case 50:
            SpriteUtilDieAfterDelay();
            break;
        case 51:
            func_805FDE4();
            goto call_8024988;
        case 53:
            func_805FDF8();
            goto call_802476C;
        case 55:
            func_805FFD4();
            func_8024688();
        case 56:
            func_80246B8();
            break;
        case 57:
            func_805FFD4();
            func_802473C();
        case 54:
        case 58:
call_802476C:
            func_802476C();
            break;
        case 59:
            func_805FFD4();
            func_80247F0();
        case 60:
            func_8024820();
            break;
        case 61:
            func_805FFD4();
            func_80248A4();
        case 62:
            func_80248D4();
            break;
        case 63:
            func_805FFD4();
            func_8024958();
        case 52:
        case 64:
call_8024988:
            func_8024988();
            break;
        case 65:
            func_805FFD4();
            func_8024A0C();
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
            goto call_8024AD4;
        case 69:
            func_8024BEC();
            goto call_8024C00;
        case 71:
            func_80604D4();
        case 68:
        case 72:
call_8024AD4:
            func_8024AD4();
            break;
        case 73:
            func_80604E8();
        case 70:
        case 74:
call_8024C00:
            func_8024C00();
            break;
        case 75:
            func_80604FC();
        case 76:
            func_806052C();
            break;
        case 81:
            func_80605D0();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            func_80605E4();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            func_8060614();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            func_8060628();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;
        case 89:
            gCurrentSprite.pose = 90;
        case 90:
            SpriteUtilThrownLeftSoft();
            break;
        case 93:
            gCurrentSprite.pose = 94;
        case 94:
            SpriteUtilThrownLeftHard();
            break;
        case 97:
            gCurrentSprite.pose = 98;
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;
        case 101:
            gCurrentSprite.pose = 102;
        case 102:
            SpriteUtilThrownUpLeftHard();
            break;
        case 91:
            gCurrentSprite.pose = 92;
        case 92:
            SpriteUtilThrownRightSoft();
            break;
        case 95:
            gCurrentSprite.pose = 96;
        case 96:
            SpriteUtilThrownRightHard();
            break;
        case 99:
            gCurrentSprite.pose = 100;
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;
        case 103:
            gCurrentSprite.pose = 104;
        case 104:
            SpriteUtilThrownUpRightHard();
            break;
        case 105:
            func_806033C();
            break;
        case 106:
            func_8060328();
            break;
        default:
            func_806049C();
            break;
    }
}

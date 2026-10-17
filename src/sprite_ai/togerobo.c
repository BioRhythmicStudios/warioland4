#include "sprite_ai/togerobo.h"

#include "fixed_point.h"
#include "score.h"
#include "sound.h"
#include "sprite_util.h"
#include "wario.h"

void func_8035668(void)
{
    u32 facing;

    facing = gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT;
    if (facing) {
        SpawnPrimarySpriteWithStatus(PSPRITE_9C, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition - 60, gCurrentSprite.xPosition + 32,
            SPRITE_STATUS_FACING_RIGHT);
    } else {
        SpawnPrimarySpriteWithStatus(PSPRITE_9C, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition - 60, gCurrentSprite.xPosition - 32, facing);
    }
}

void func_80356BC(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = 32;
        gCurrentSprite.hitboxExtentRight = 48;
    } else {
        gCurrentSprite.hitboxExtentLeft = 52;
        gCurrentSprite.hitboxExtentRight = 28;
    }
}

void func_80356EC(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD04C);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    if (gCurrentSprite.health == 2) {
        gCurrentSprite.health = 1;
        gCurrentSprite.hitboxExtentUp = 80;
        func_8035668();
    }
}

void func_803572C(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_HEAVY;
    gCurrentSprite.drawDistanceDown = 36;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 20;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentUp = 112;
    gCurrentSprite.health = 2;
    gCurrentSprite.warioCollision = 37;
    if (gCurrentSprite.xPosition > gWarioData.xPosition)
        gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    else
        gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    func_80356BC();
}

void func_8035790(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BCF4C);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 62;
    gCurrentSprite.work1 = 50;
}

void func_80357B8(void)
{
    u32 nearby;

    nearby = SpriteUtilWaitCheckWarioNearbyLeftRight(127, 400);
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
            gCurrentSprite.pose = 112;
            return;
        }
        if (gSpriteCollisionResult & 0xF0) {
            if (((gCurrentSprite.xPosition & 0x3F) +
                    gCurrentSprite.hitboxExtentRight) > 0x3F) {
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
            gCurrentSprite.pose = 112;
            return;
        }
        if (gSpriteCollisionResult & 0xF0) {
            if ((gCurrentSprite.xPosition & 0x3F) <
                gCurrentSprite.hitboxExtentLeft) {
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

    {
        struct PrimarySpriteData *sprite;
        u8 *work0;
        u32 next;
        u32 timer;

        sprite = &gCurrentSprite;
        work0 = &sprite->work0;
        next = *work0;
        if (next != 0) {
            next -= 1;
            *work0 = next;
            timer = (u8)next;
            if (timer == 0) {
                GP_SET(sprite->pOamData, sTogeroboOam_83BCEFC);
                sprite->currentAnimationFrame = timer;
                sprite->animationTimer = timer;
            } else {
                next = *work0;
                if (next == 55) {
                    if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                        sprite->xPosition -= 4;
                    else
                        sprite->xPosition += 4;
                } else if (next == 48) {
                    if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                        sprite->xPosition -= 4;
                    else
                        sprite->xPosition += 4;
                } else if (next == 34) {
                    if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                        sprite->xPosition += 4;
                    else
                        sprite->xPosition -= 4;
                } else if (next == 20) {
                    if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                        sprite->xPosition += 4;
                    else
                        sprite->xPosition -= 4;
                }
            }
        } else {
            func_80263AC();
        }
    }
}

void func_8035978(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BCF94);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 11;
}

void func_8035998(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 next;
    u32 timer;

    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        sprite = &gCurrentSprite;
        work0 = &sprite->work0;
        next = *work0;
        next -= 1;
        *work0 = next;
        timer = (u8)next;
        if (timer == 0) {
            switch (sprite->pose) {
                case 18:
                    GP_SET(sprite->pOamData, sTogeroboOam_83BCFAC);
                    sprite->currentAnimationFrame = timer;
                    sprite->animationTimer = timer;
                    sprite->pose = 20;
                    *work0 = 5;
                    sprite->warioCollision = 33;
                    {
                        u8 *hitbox;
                        hitbox = &sprite->hitboxExtentLeft;
                        *hitbox++ = 28;
                        *hitbox = 24;
                    }
                    break;
                case 20:
                    GP_SET(sprite->pOamData, sTogeroboOam_83BCFBC);
                    sprite->currentAnimationFrame = timer;
                    sprite->animationTimer = timer;
                    sprite->pose = 22;
                    *work0 = 9;
                    sprite->warioCollision = 37;
                    sprite->status ^= SPRITE_STATUS_FACING_RIGHT;
                    func_80356BC();
                    break;
                default:
                    sprite->pose = 15;
                    break;
            }
        }
    }
}

void func_8035A34(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 24;
    if (gCurrentSprite.health == 2)
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD19C);
    else
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD18C);
}

void func_8035A64(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            if (gCurrentSprite.health == 2) {
                if (gCurrentSprite.xPosition > gWarioData.xPosition) {
                    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                        gCurrentSprite.pose = 17;
                    else
                        gCurrentSprite.pose = 15;
                } else {
                    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                        gCurrentSprite.pose = 15;
                    else
                        gCurrentSprite.pose = 17;
                }
            } else {
                gCurrentSprite.pose = 110;
            }
        }
    }
}

void func_8035AE0(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BCEB4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work0 = 43;
}

void func_8035B00(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    register u32 old PIN("r0");
    register u32 next PIN("r1");
    register u32 check PIN("r0");

    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        sprite = &gCurrentSprite;
        work0 = &sprite->work0;
        old = *work0;
        next = old - 1;
        *work0 = next;
        check = next << 24;
        if (check != 0) {
            check >>= 24;
            if (check == 25) {
                sprite->warioCollision = 37;
                sprite->hitboxExtentUp = 112;
                sprite->health = 2;
            }
        } else {
            sprite->pose = 23;
        }
    }
}

void func_8035B5C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD01C);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 113;
    gCurrentSprite.work0 = 100;
    m4aSongNumStart(SE_TOGEROBO_ATTACK_START);
}

void func_8035B88(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    register u32 old PIN("r0");
    register u32 next PIN("r1");
    register u32 check PIN("r0");

    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        sprite = &gCurrentSprite;
        work0 = &sprite->work0;
        old = *work0;
        next = old - 1;
        *work0 = next;
        check = next << 24;
        if (check != 0) {
            check >>= 24;
            if (check == 86) {
                sprite->health = 1;
                sprite->hitboxExtentUp = 80;
                sprite->warioCollision = 3;
                func_8035668();
            }
        } else {
            sprite->pose = 23;
        }
    }
}

void func_8035BE8(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD074);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 40;
}

void func_8035C0C(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u8 timer;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    timer = *work0;
    if (timer > 25) {
        func_8023B88();
        if (gSpriteCollisionResult == 0)
            sprite->pose = 29;
        func_80238E8();
        (*work0)--;
    } else if (timer <= 4) {
        func_8023B88();
        if (gSpriteCollisionResult == 0) {
            sprite->pose = 27;
        } else {
            u32 next;
            next = *work0;
            next -= 1;
            *work0 = next;
            next <<= 24;
            if (next == 0)
                sprite->pose = 23;
        }
    } else {
        if (timer == 25) {
            sprite->yPosition -= 24;
        } else if (timer == 20) {
            GP_SET(sprite->pOamData, sTogeroboOam_83BD09C);
            sprite->animationTimer = 0;
            sprite->currentAnimationFrame = 0;
            sprite->yPosition -= 24;
            sprite->status ^= SPRITE_STATUS_FACING_RIGHT;
        } else if (timer == 10) {
            sprite->yPosition += 24;
            sprite->warioCollision = 3;
            func_80356BC();
        } else if (timer == 5) {
            sprite->yPosition += 24;
        }
        gCurrentSprite.work0--;
    }
}

void func_8035CD0(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.health == 2) {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BCF24);
        gCurrentSprite.warioCollision = 37;
    } else {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD1AC);
        gCurrentSprite.warioCollision = 3;
    }
    func_80356BC();
}

void func_8035D10(void)
{
    func_80356EC();
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void func_8035D2C(void)
{
    if (gCurrentSprite.health == 2)
        func_8035668();
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD164);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 4;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, 20);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32,
        gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
}

void func_8035DAC(void)
{
    gCurrentSprite.pose = 32;
    func_8035D2C();
}

void func_8035DC0(void)
{
    gCurrentSprite.pose = 34;
    func_8035D2C();
}

void func_8035DD4(void)
{
    if (gCurrentSprite.health == 2)
        func_8035668();
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD164);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 3;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, 20);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32,
        gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
}

void func_8035E40(void)
{
    gCurrentSprite.pose = 32;
    func_8035DD4();
}

void func_8035E54(void)
{
    gCurrentSprite.pose = 34;
    func_8035DD4();
}

void func_8035E68(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
}

void func_8035E90(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD10C);
        if (gCurrentSprite.health == 2) {
            gCurrentSprite.health = 1;
            gCurrentSprite.hitboxExtentUp = 80;
            gCurrentSprite.warioCollision = 3;
            func_8035668();
        }
    } else if (gCurrentSprite.health == 2) {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD14C);
    } else {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD134);
    }
    gCurrentSprite.pose = 36;
    func_8035E68();
}

void func_8035EF4(void)
{
    func_80238A4();
    if (gCurrentSprite.work2 > 3) {
        func_80242C8();
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0)
            gCurrentSprite.pose = 23;
    }
}

void func_8035F28(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        if (gCurrentSprite.health == 2)
            GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD14C);
        else
            GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD134);
    } else {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD10C);
        if (gCurrentSprite.health == 2) {
            gCurrentSprite.health = 1;
            gCurrentSprite.hitboxExtentUp = 80;
            gCurrentSprite.warioCollision = 3;
            func_8035668();
        }
    }
    gCurrentSprite.pose = 38;
    func_8035E68();
}

void func_8035F8C(void)
{
    func_80238A4();
    if (gCurrentSprite.work2 > 3) {
        func_8024180();
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0)
            gCurrentSprite.pose = 23;
    }
}

void func_8035FC0(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 6;
    if (gCurrentSprite.health == 2)
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD14C);
    else
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD134);
}

void func_8036000(void)
{
    gCurrentSprite.pose = 40;
    func_8035FC0();
}

void func_8036014(void)
{
    func_80238A4();
    if (gCurrentSprite.work2 > 3) {
        func_80242C8();
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            if (gCurrentSprite.health == 2)
                gCurrentSprite.pose = 17;
            else
                gCurrentSprite.pose = 23;
        }
    }
}

void func_8036054(void)
{
    gCurrentSprite.pose = 42;
    func_8035FC0();
}

void func_8036068(void)
{
    func_80238A4();
    if (gCurrentSprite.work2 > 3) {
        func_8024180();
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            if (gCurrentSprite.health == 2)
                gCurrentSprite.pose = 17;
            else
                gCurrentSprite.pose = 23;
        }
    }
}

void func_80360A8(void)
{
    func_80356EC();
    gCurrentSprite.work1 = 0;
}

void func_80360C0(void)
{
    gCurrentSprite.pose = 44;
    func_80360A8();
}

void func_80360D4(void)
{
    gCurrentSprite.pose = 46;
    func_80360A8();
}

void func_80360E8(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 50;
    gCurrentSprite.work0 = 9;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, 20);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
    if (gCurrentSprite.warioCollision != 5)
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD0CC);
    else
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD0EC);
}

void func_803615C(void)
{
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition,
        gCurrentSprite.xPosition, 20);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = 2;
}

void func_8036194(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
    func_80356EC();
}

void func_80361BC(void)
{
    gCurrentSprite.pose = 52;
    func_8036194();
}

void func_80361D0(void)
{
    gCurrentSprite.pose = 54;
    func_8036194();
}

void func_80361E4(void)
{
    gCurrentSprite.work3 = 0;
    gCurrentSprite.work2 = 6;
    func_80356EC();
}

void func_8036204(void)
{
    gCurrentSprite.pose = 72;
    func_80361E4();
}

void func_8036218(void)
{
    gCurrentSprite.pose = 74;
    func_80361E4();
}

void func_803622C(void)
{
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.health == 2) {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD19C);
        gCurrentSprite.warioCollision = 37;
    } else {
        GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD18C);
        gCurrentSprite.warioCollision = 3;
    }
    func_80356BC();
}

void func_8036274(void)
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
            velocities = sUnk_8352B2C;
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

void func_8036300(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD04C);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void func_8036318(void)
{
    gCurrentSprite.pose = 82;
    func_8036300();
}

void func_803632C(void)
{
    gCurrentSprite.pose = 84;
    func_8036300();
}

void func_8036340(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BD04C);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void func_803635C(void)
{
    gCurrentSprite.pose = 88;
    func_8036340();
}

void func_8036370(void)
{
    gCurrentSprite.pose = 86;
    func_8036340();
}

void func_8036384(void)
{
    struct PrimarySpriteData *sprite;
    register u16 status PIN("r2");
    register u32 zeroByte PIN("r3");
    u32 zeroHalf;
    u16 oldStatus;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = 0xA0;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    {
        u8 *fields;
        fields = (u8 *)sprite;
        *(u16 *)fields = status;
        fields += 32;
        *fields++ = 24;
        *fields++ = 2;
        *fields++ = 16;
        *fields++ = 80;
        *fields++ = 140;
        *fields++ = 40;
        /* Preserve the post-increment form required by agbcc for the final extent. */
        *fields = 36;
    }
    {
        const struct AnimationFrame *oam;
        register u8 *fields PIN("r1");
        oam = sTogeroboOam_83BD00C;
        fields = (u8 *)sprite;
        GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(fields + 4), oam);
        fields[22] = zeroByte;
        *(u16 *)(fields + 20) = zeroHalf;
        fields[30] = 9;
        sprite->work1 = zeroByte;
    }
    if (status & SPRITE_STATUS_FACING_RIGHT) {
        sprite->pose = 51;
    } else {
        u32 pose;
        pose = 53;
        sprite->pose = pose;
    }
}

void func_8036400(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void func_8036420(void)
{
    gCurrentSprite.pose = 52;
    gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    func_8036400();
}

void func_803643C(void)
{
    gCurrentSprite.pose = 54;
    gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    func_8036400();
}

void func_803645C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTogeroboOam_83BCFD4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 50;
    gCurrentSprite.work0 = 16;
    gCurrentSprite.status &= ~SPRITE_STATUS_AFFINE;
    gCurrentSprite.drawPriority = 1;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    m4aSongNumStart(SE_TOGEROBO_CHILD_DEFEATED);
}

void func_80364A4(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    register u32 old PIN("r0");
    register u32 next PIN("r1");
    register u32 check PIN("r0");

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    old = *work0;
    next = old - 1;
    *work0 = next;
    check = (u8)next;
    if (check != 0) {
        check = (u8)next;
        if (check <= 7)
            sprite->disableWarioCollisionTimer = 1;
    } else {
        sprite->status = check;
    }
}

void func_80364D0(void)
{
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 4;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32,
        gCurrentSprite.xPosition, 6);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
}

void func_8036514(void)
{
    gCurrentSprite.pose = 32;
    func_80364D0();
}

void func_8036528(void)
{
    gCurrentSprite.pose = 34;
    func_80364D0();
}

void SpriteTogerobo(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_UNDERWATER)
        sprite->pose = 107;

    switch (sprite->pose) {
        case 0:
            func_803572C();
            func_8035790();
            break;
        case 15:
            func_8035790();
        case 16:
            func_80357B8();
            break;
        case 17:
            func_8035978();
        case 18:
        case 20:
        case 22:
            func_8035998();
            break;
        case 23:
            func_8035A34();
        case 24:
            func_8035A64();
            break;
        case 25:
            func_8035BE8();
        case 26:
            func_8035C0C();
            break;
        case 27:
            func_8035CD0();
        case 28:
            func_8023D48();
            break;
        case 29:
            func_8035D10();
        case 30:
            func_8023EE0();
            break;
        case 31:
        case 79:
            func_8035DAC();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            func_8035DC0();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
            func_8035E90();
        case 36:
            func_8035EF4();
            break;
        case 37:
            func_8035F28();
        case 38:
            func_8035F8C();
            break;
        case 39:
            func_8036000();
        case 40:
            func_8036014();
            break;
        case 41:
            func_8036054();
        case 42:
            func_8036068();
            break;
        case 43:
            func_80360C0();
        case 44:
            func_8024478();
            break;
        case 45:
            func_80360D4();
        case 46:
            func_802449C();
            break;
        case 47:
            func_80244C0();
        case 48:
            func_80244E0();
            break;
        case 49:
            func_80360E8();
        case 50:
            SpriteUtilDieAfterDelay();
            break;
        case 51:
            func_80361BC();
            goto call_8024988;
        case 53:
            func_80361D0();
            goto call_802476C;
        case 55:
            func_8024688();
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
        case 54:
        case 58:
call_802476C:
            func_802476C();
            break;
        case 59:
            func_80247F0();
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
        case 52:
        case 64:
call_8024988:
            func_8024988();
            break;
        case 65:
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
            func_8036204();
        case 68:
        case 72:
call_8024AD4:
            func_8024AD4();
            break;
        case 73:
            func_8036218();
        case 70:
        case 74:
call_8024C00:
            func_8024C00();
            break;
        case 75:
            func_803622C();
        case 76:
            func_8036274();
            break;
        case 105:
            func_8035E54();
            break;
        case 106:
            func_8035E40();
            break;
        case 110:
            func_8035AE0();
        case 111:
            func_8035B00();
            break;
        case 112:
            func_8035B5C();
        case 113:
            func_8035B88();
            break;
        case 81:
            func_8036318();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            func_803632C();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            func_803635C();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            func_8036370();
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
        case 104:
            SpriteUtilThrownUpRightHard();
            break;
        default:
            func_803615C();
            break;
    }
}

void SpriteUnknown9C(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_UNDERWATER)
        sprite->pose = 107;

    switch (sprite->pose) {
        case 0:
            func_8036384();
            break;
        case 43:
        case 44:
            gCurrentSprite.work2 = 5;
            func_80238E8();
            func_8025D34();
            break;
        case 45:
        case 46:
            gCurrentSprite.work2 = 5;
            func_80238E8();
            func_8025C80();
            break;
        case 51:
            func_8036420();
            goto call_8024988;
        case 53:
            func_803643C();
            goto call_802476C;
        case 55:
            func_8024688();
            m4aSongNumStart(SE_TOGEROBO_CHILD_IMPACT);
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
            m4aSongNumStart(SE_TOGEROBO_CHILD_IMPACT);
        case 54:
        case 58:
call_802476C:
            func_802476C();
            break;
        case 59:
            func_80247F0();
            m4aSongNumStart(SE_TOGEROBO_CHILD_IMPACT);
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
            m4aSongNumStart(SE_TOGEROBO_CHILD_IMPACT);
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
            m4aSongNumStart(SE_TOGEROBO_CHILD_IMPACT);
        case 52:
        case 64:
call_8024988:
            func_8024988();
            break;
        case 65:
            func_8024A0C();
            m4aSongNumStart(SE_TOGEROBO_CHILD_IMPACT);
        case 66:
            func_8024A3C();
            break;
        case 67:
        case 71:
            if (!(gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)) {
                func_8024AC0();
                func_8024AD4();
            } else {
                gCurrentSprite.pose = 49;
            }
            break;
        case 68:
            func_8024AD4();
            break;
        case 69:
        case 73:
            if (!(gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)) {
                gCurrentSprite.pose = 49;
            } else {
                func_8024BEC();
                func_8024C00();
            }
            break;
        case 70:
            func_8024C00();
            break;
        case 31:
        case 79:
            func_8036514();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            func_8036528();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 107:
            SpriteUtilDie();
            break;
        case 50:
            func_80364A4();
            break;
        default:
            func_803645C();
            break;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_AFFINE) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
            gCurrentSprite.work1 += 5;
        else
            gCurrentSprite.work1 -= 5;

        gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(256));
        gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(256));
        gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(256));
        gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(256));
    }
}

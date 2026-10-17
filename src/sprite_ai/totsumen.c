#include "sprite_ai/totsumen.h"

#include "score.h"
#include "sound.h"
#include "sprite_util.h"

void func_8032320(void)
{
    gCurrentSprite.warioCollision = 33;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB0E8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work1 = 42;
    SpriteUtilTurnTowardWario();
}

void func_803238C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB0E8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 33;
    gCurrentSprite.work1 = 42;
}

void func_80323B0(void)
{
    u32 near;

    near = SpriteUtilWaitCheckWarioNearbyLeftRight(56, 320);
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
        if (near == NS_RIGHT) {
            gCurrentSprite.pose = 110;
            return;
        }
        if (near == NS_LEFT) {
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
    } else if (gSpriteCollisionResult != 0) {
        if (near == NS_LEFT) {
            gCurrentSprite.pose = 110;
            return;
        }
        if (near == NS_RIGHT) {
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

void func_80324FC(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB2E8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 13;
    gCurrentSprite.warioCollision = 33;
}

void func_8032524(void)
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
                GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB308);
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                gCurrentSprite.pose = 20;
                gCurrentSprite.work0 = 16;
            } else {
                GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB0E8);
                gCurrentSprite.currentAnimationFrame = 0;
                gCurrentSprite.animationTimer = 0;
                gCurrentSprite.pose = 16;
                gCurrentSprite.warioCollision = 33;
            }
        }
    }
}

void func_80325A4(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5)
        gCurrentSprite.warioCollision = 5;
}

void func_80325DC(void)
{
    gCurrentSprite.pose = 52;
    func_80325A4();
}

void func_80325F0(void)
{
    gCurrentSprite.pose = 54;
    func_80325A4();
}

void func_8032604(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB158);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work0 = 57;
    gCurrentSprite.warioCollision = 33;
}

void func_803262C(void)
{
    struct PrimarySpriteData *sprite;
    register u32 next PIN("r2");
    u8 timer;
    u32 check;

    sprite = &gCurrentSprite;
    {
        u8 *work0;
        register u32 old PIN("r0");
        work0 = &sprite->work0;
        old = *work0;
        next = old - 1;
        *work0 = next;
    }
    timer = next;
    /* Keep next live through the byte conversion so agbcc assigns timer to r1. This emits no instructions. */
    ASM_BARRIER(: : "r"(next));
    if (timer != 0) {
        /* Force the comparison copy through r0; the empty constraint itself emits no instructions. */
        ASM_BARRIER(: "=r"(check) : "0"(timer));
        if (check == 42)
            sprite->warioCollision = 3;
        func_80238A4();
        func_8023B88();
        if (gSpriteCollisionResult == 0)
            sprite->pose = 27;
    } else {
        GP_SET(sprite->pOamData, sTotsumenOam_83BB1A0);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 113;
        m4aSongNumStart(SE_TOTSUMEN_CHARGE);
    }
}

void func_8032688(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC(gCurrentSprite.yPosition,
            gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        if ((gSpriteCollisionTileType & 0xF0) == 0) {
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
        if ((gSpriteCollisionTileType & 0xF0) == 0) {
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

void func_80327CC(void)
{
    if (gCurrentSprite.warioCollision == 3) {
        GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.warioCollision = 5;
    }
    m4aSongNumStart(SE_TOTSUMEN_IMPACT);
}

void func_80327F8(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB1D8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 115;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.work3 = 0;
}

void func_8032824(void)
{
    u32 yPosition;

    gCurrentSprite.work0++;
    SpriteUtilLookupGravity(sUnk_83BB480);

    if (gCurrentSprite.work0 > 16) {
        yPosition = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
        if (gBackgroundCollisionData.waterType == 1)
            gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
        if (gSpriteCollisionResult != 0) {
            gCurrentSprite.yPosition = yPosition;
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
                gCurrentSprite.pose = 54;
            else
                gCurrentSprite.pose = 52;

            GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
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

void func_8032960(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB378);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.warioCollision = 33;
}

void func_8032988(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB278);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 84;
}

void func_80329AC(void)
{
    register u32 timer PIN("r2");
    u32 original;

    timer = gCurrentSprite.work0;
    original = timer;
    if ((u8)(timer - 28) > 28) {
        func_8023CD8();
    } else {
        if (timer == 56 || timer == 49) {
            gCurrentSprite.yPosition -= 32;
        } else if (timer == 42) {
            gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
            GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB2B0);
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
}

void func_8032A24(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB120);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
}

void func_8032A44(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void func_8032A6C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB3F0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) | SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void func_8032AE4(void)
{
    gCurrentSprite.pose = 32;
    func_8032A6C();
}

void func_8032AF8(void)
{
    gCurrentSprite.pose = 34;
    func_8032A6C();
}

void func_8032B0C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB3F0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void func_8032B70(void)
{
    gCurrentSprite.pose = 32;
    func_8032B0C();
}

void func_8032B84(void)
{
    gCurrentSprite.pose = 34;
    func_8032B0C();
}

void func_8032B98(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB360);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
}

void func_8032BCC(void)
{
    gCurrentSprite.pose = 36;
    func_8032B98();
}

void func_8032BE0(void)
{
    gCurrentSprite.pose = 38;
    func_8032B98();
}

void func_8032BF4(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB3C0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void func_8032C24(void)
{
    gCurrentSprite.pose = 40;
    func_8032BF4();
}

void func_8032C38(void)
{
    gCurrentSprite.pose = 42;
    func_8032BF4();
}

void func_8032C4C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
}

void func_8032C70(void)
{
    gCurrentSprite.pose = 44;
    func_8032C4C();
}

void func_8032C84(void)
{
    gCurrentSprite.pose = 46;
    func_8032C4C();
}

void func_8032C98(void)
{
    if (gCurrentSprite.warioCollision == 5) {
        GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB3A8);
        gCurrentSprite.work0 = 7;
    } else {
        GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB388);
        gCurrentSprite.work0 = 8;
    }
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 50;
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) | SPRITE_STATUS_MAYBE_DEAD;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}
void func_8032D14(void)
{
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void func_8032D4C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
}

void func_8032D78(void)
{
    gCurrentSprite.pose = 72;
    func_8032D4C();
}

void func_8032D8C(void)
{
    gCurrentSprite.pose = 74;
    func_8032D4C();
}

void func_8032DA0(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB220);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 19;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 33;
}

void func_8032DD0(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
        if (gSpriteCollisionTileType & 0xF) {
            gCurrentSprite.pose = 29;
        } else {
            SpriteUtilLookupGravity(sUnk_8352B18);
        }
    } else {
        gCurrentSprite.pose = 27;
    }
}

void func_8032E5C(void)
{
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void func_8032E74(void)
{
    gCurrentSprite.pose = 82;
    func_8032E5C();
}

void func_8032E88(void)
{
    gCurrentSprite.pose = 84;
    func_8032E5C();
}

void func_8032E9C(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sTotsumenOam_83BB240);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void func_8032EB8(void)
{
    gCurrentSprite.pose = 88;
    func_8032E9C();
}

void func_8032ECC(void)
{
    gCurrentSprite.pose = 86;
    func_8032E9C();
}

void SpriteTotsumen(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_UNDERWATER)
        sprite->pose = 107;

    switch (sprite->pose) {
        case 0:
            func_8032320();
            break;
        case 15:
            func_803238C();
        case 16:
            func_80323B0();
            break;
        case 17:
            func_80324FC();
        case 18:
        case 20:
            func_8032524();
            break;
        case 110:
            func_8032604();
        case 111:
            func_803262C();
            break;
        case 113:
            func_8032688();
            break;
        case 114:
            func_80327F8();
        case 115:
            func_8032824();
            break;
        case 23:
            func_8032960();
        case 24:
            func_8023C94();
            break;
        case 25:
            func_8032988();
        case 26:
            func_80329AC();
            break;
        case 27:
            func_8032A24();
        case 28:
            func_8023D48();
            break;
        case 29:
            func_8032A44();
        case 30:
            func_8023EE0();
            break;
        case 31:
        case 79:
            func_8032AE4();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            func_8032AF8();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
            func_8032BCC();
        case 36:
            SpriteUtilPushedRight();
            break;
        case 37:
            func_8032BE0();
        case 38:
            SpriteUtilPushedLeft();
            break;
        case 39:
            func_8032C24();
        case 40:
            SpriteUtilPushedRight();
            func_80238E8();
            break;
        case 41:
            func_8032C38();
        case 42:
            SpriteUtilPushedLeft();
            func_80238E8();
            break;
        case 43:
            func_8032C70();
        case 44:
            func_8024478();
            break;
        case 45:
            func_8032C84();
        case 46:
            func_802449C();
            break;
        case 47:
            func_80244C0();
        case 48:
            func_80244E0();
            break;
        case 49:
            func_8032C98();
        case 50:
            SpriteUtilDieAfterDelay();
            break;
        case 51:
            func_80325DC();
            goto call_8024988;
        case 53:
            func_80325F0();
            goto call_802476C;
        case 55:
            func_80327CC();
            func_8024688();
        case 56:
            func_80246B8();
            break;
        case 57:
            func_80327CC();
            func_802473C();
        case 54:
        case 58:
call_802476C:
            func_802476C();
            break;
        case 59:
            func_80327CC();
            func_80247F0();
        case 60:
            func_8024820();
            break;
        case 61:
            func_80327CC();
            func_80248A4();
        case 62:
            func_80248D4();
            break;
        case 63:
            func_80327CC();
            func_8024958();
        case 52:
        case 64:
call_8024988:
            func_8024988();
            break;
        case 65:
            func_80327CC();
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
            func_8032D78();
        case 68:
        case 72:
call_8024AD4:
            func_8024AD4();
            break;
        case 73:
            func_8032D8C();
        case 70:
        case 74:
call_8024C00:
            func_8024C00();
            break;
        case 75:
            func_8032DA0();
        case 76:
            func_8032DD0();
            break;
        case 81:
            func_8032E74();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            func_8032E88();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            func_8032EB8();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            func_8032ECC();
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
        case 105:
            func_8032B84();
            break;
        case 106:
            func_8032B70();
            break;
        default:
            func_8032D14();
            break;
    }
}


#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

#ifndef PORTABLE
const u16 sRockOam_Frame1[] = {
    2,
    OAM_ENTRY(-10, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(6, -16, SPRITE_SIZE_8x16, 0, 514, 8, 0),
};

const u16 sRockOam_UnusedFrame1[] = {
    1,
    OAM_ENTRY(-4, -4, SPRITE_SIZE_8x8, 0, 515, 8, 0),
};
const u16 sRockOam_UnusedFrame2[] = {
    1,
    OAM_ENTRY(-4, -4, SPRITE_SIZE_8x8, 0, 547, 8, 0),
};
const u16 sRockOam_UnusedFrame3[] = {
    1,
    OAM_ENTRY(-4, -4, SPRITE_SIZE_8x8, 0, 516, 8, 0),
};
const u16 sRockOam_UnusedFrame4[] = {
    1,
    OAM_ENTRY(-4, -4, SPRITE_SIZE_8x8, 0, 548, 8, 0),
};

const struct AnimationFrame sRockOamData[] = {{sRockOam_Frame1, 0xC8}, ANIMATION_TERMINATOR};
#else
extern const u16 sRockOam_Frame1[];
extern const u16 sRockOam_UnusedFrame1[];
extern const u16 sRockOam_UnusedFrame2[];
extern const u16 sRockOam_UnusedFrame3[];
extern const u16 sRockOam_UnusedFrame4[];
extern const struct AnimationFrame sRockOamData[];
#endif

void RockInit(void)
{
    gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;

    gCurrentSprite.hitboxExtentUp = SUBPIXELS_FROM_PIXELS(13);
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = SUBPIXELS_FROM_PIXELS(7);
    gCurrentSprite.hitboxExtentRight = SUBPIXELS_FROM_PIXELS(6);
}

void RockIdleInit(void)
{
    gCurrentSprite.animationTimer = gCurrentSprite.currentAnimationFrame = 0;
    GP_SET(gCurrentSprite.pOamData, sRockOamData);
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.status &= ~(SPRITE_STATUS_CAN_HIT_OTHER_SPRITES | SPRITE_STATUS_MAYBE_DEAD);
}

void RockIdle(void)
{
    func_80238E8();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (!gSpriteCollisionTileType) {
            gCurrentSprite.pose = SPOSE_1B;
        }
    }
}

void RockPose1B(void)
{
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
}

void RockPose2B(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    gCurrentSprite.work1 = 0;
}

void RockPose2D(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    gCurrentSprite.work1 = 0;
}

void RockPose47(void)
{
    gCurrentSprite.pose = SPOSE_48;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void RockPose49(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void RockBumpedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_34;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void RockBumpedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_36;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void RockGroundPounded(void)
{
    if (gCurrentSprite.xPosition > gWarioData.xPosition) {
        RockBumpedRightInit();
    } else {
        RockBumpedLeftInit();
    }
}

void RockLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
}

void RockLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
}

void RockCarriedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    if (gCurrentSprite.status | SPRITE_STATUS_MAYBE_DEAD) {
        gCurrentSprite.status &= ~SPRITE_STATUS_MAYBE_DEAD;
    }
}

void RockCarriedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    if (gCurrentSprite.status | SPRITE_STATUS_MAYBE_DEAD) {
        gCurrentSprite.status &= ~SPRITE_STATUS_MAYBE_DEAD;
    }
}

void SpriteRock(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER && (gStageFrameCounter & 3)) {
        return;
    }
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            RockInit();

        case SPOSE_IDLE_INIT:
        case SPOSE_17:
        case SPOSE_2F:
            RockIdleInit();
        case SPOSE_IDLE:
            RockIdle();
            break;

        case SPOSE_1B:
        case SPOSE_1D:
            RockPose1B();
        case SPOSE_1E:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8023EE0();
            break;

        case SPOSE_2B:
            RockPose2B();
        case SPOSE_2C:
            func_8024478();
            break;

        case SPOSE_2D:
            RockPose2D();
        case SPOSE_2E:
            func_802449C();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            RockGroundPounded();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_PUSHED_RIGHT_INIT:
        case SPOSE_33:
        case SPOSE_4F:
        case SPOSE_6A:
            RockBumpedRightInit();
        case SPOSE_34:
            func_8024988();
            break;

        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_PUSHED_LEFT_INIT:
        case SPOSE_35:
        case SPOSE_50:
        case SPOSE_69:
            RockBumpedLeftInit();
        case SPOSE_36:
            func_802476C();
            break;

        case SPOSE_37:
            func_8024688();
        case SPOSE_38:
            func_80246B8();
            break;

        case SPOSE_39:
            func_802473C();
        case SPOSE_3A:
            func_802476C();
            break;

        case SPOSE_3B:
            func_80247F0();

        case SPOSE_3C:
            func_8024820();
            break;

        case SPOSE_3D:
            func_80248A4();
        case SPOSE_3E:
            func_80248D4();
            break;

        case SPOSE_3F:
            func_8024958();
        case SPOSE_40:
            func_8024988();
            break;

        case SPOSE_41:
            func_8024A0C();
        case SPOSE_42:
            func_8024A3C();
            break;

        case SPOSE_43:
            func_8024AC0();
        case SPOSE_44:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8024AD4();
            break;

        case SPOSE_45:
            func_8024BEC();
        case SPOSE_46:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8024C00();
            break;

        case SPOSE_47:
            RockPose47();
        case SPOSE_48:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8024AD4();
            break;

        case SPOSE_49:
            RockPose49();
        case SPOSE_4A:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8024C00();
            break;

        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            RockLiftedRightInit();
            break;

        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;

        case SPOSE_BEING_LIFTED_LEFT_INIT:
            RockLiftedLeftInit();
            break;

        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;

        case SPOSE_CARRIED_RIGHT_INIT:
            RockCarriedRightInit();
            break;

        case SPOSE_CARRIED_RIGHT:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilCarryingSpriteRight();
            break;

        case SPOSE_CARRIED_LEFT_INIT:
            RockCarriedLeftInit();
            break;

        case SPOSE_CARRIED_LEFT:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilCarryingSpriteLeft();
            break;

        case SPOSE_THROWN_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_SOFT;
        case SPOSE_THROWN_LEFT_SOFT:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownLeftSoft();
            break;

        case SPOSE_THROWN_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_HARD;
            gCurrentSprite.health = (gCurrentSprite.health & 0xF) | 0x10;
        case SPOSE_THROWN_LEFT_HARD:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownLeftHard();
            break;

        case SPOSE_THROWN_UP_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_SOFT;
        case SPOSE_THROWN_UP_LEFT_SOFT:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpLeftSoft();
            break;

        case SPOSE_THROWN_UP_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_HARD;
            gCurrentSprite.health = (gCurrentSprite.health & 0xF) | 0x20;
        case SPOSE_THROWN_UP_LEFT_HARD:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpLeftHard();
            break;

        case SPOSE_THROWN_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_SOFT;
        case SPOSE_THROWN_RIGHT_SOFT:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownRightSoft();
            break;

        case SPOSE_THROWN_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_HARD;
            gCurrentSprite.health = (gCurrentSprite.health & 0xF) | 0x40;
        case SPOSE_THROWN_RIGHT_HARD:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownRightHard();
            break;

        case SPOSE_THROWN_UP_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_SOFT;
        case SPOSE_THROWN_UP_RIGHT_SOFT:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpRightSoft();
            break;

        case SPOSE_THROWN_UP_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_HARD;
            gCurrentSprite.health = (gCurrentSprite.health & 0xF) | 0x80;
        case SPOSE_THROWN_UP_RIGHT_HARD:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            SpriteUtilThrownUpRightHard();
            break;

        case SPOSE_6B:
            if (gCurrentSprite.health & 0x10) {
                gCurrentSprite.health &= 0xF;
                gCurrentSprite.work2 = 8;
                func_8024BEC();
            } else if (gCurrentSprite.health & 0x20) {
                gCurrentSprite.health &= 0xF;
                gCurrentSprite.work2 = 4;
                func_8024AC0();
            } else if (gCurrentSprite.health & 0x40) {
                gCurrentSprite.health &= 0xF;
                gCurrentSprite.work2 = 8;
                func_8024AC0();
            } else if (gCurrentSprite.health & 0x80) {
                gCurrentSprite.health &= 0xF;
                gCurrentSprite.work2 = 4;
                func_8024BEC();
            } else {
                SpriteUtilDie();
                break;
            }
            break;

        default:
            SpriteUtilDie();
            break;
    }

    if ((gCurrentSprite.health & 0xF) == 1) {
        if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
            gCurrentSprite.health += 1;
            SpawnSecondarySprite(gCurrentSprite.yPosition, gCurrentSprite.xPosition, SSPRITE_07);
            m4aSongNumStart(SE_ENTER_WATER);
        }
    } else if ((gCurrentSprite.health & 0xF) == 2) {
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
        if (gBackgroundCollisionData.waterType == 1) {
            gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
        }
        if (!(gCurrentSprite.status & SPRITE_STATUS_UNDERWATER)) {
            gCurrentSprite.health -= 1;
        }
    }
}

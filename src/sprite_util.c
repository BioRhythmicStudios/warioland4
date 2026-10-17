#include "sprite_util.h"

#include "background_registers.h"
#include "block.h"
#include "color_effects.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "wario.h"

#ifndef PORTABLE
const s16 sSpriteFallingOffscreenYVelocity[] = {
    -28, -24, -20, -16, -12, -12, -8, -8, -4, -4, 0,  0,  0,  0,  0,  0,
    0,   4,   0,   4,   8,   8,   12, 8,  12, 12, 16, 16, 20, 24, 28, S16_MAX,
};

const s16 sSpriteFallingOffscreenYVelocityHeavy[] = {
    -16, -12, -12, -12, -8, -8, -4, -4, -4, 0,  0,  4,  0,  4,  0,  4,
    8,   8,   8,   8,   8,  12, 12, 12, 16, 20, 24, 28, 32, 36, 40, S16_MAX,
};

const s16 sUnk_8352A28[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, S16_MAX,
};

const s16 sUnk_8352A68[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 8, 12, 12, 16, 16, 20, S16_MAX,
};

const s16 sUnk_8352AA2[] = {
    -8,
    -4,
    -4,
    0,
    S16_MAX,
};

const s16 sUnk_8352AAC[] = {
    -4,
    0,
    S16_MAX,
};

const u16 sUnk_8352AB2[] = {
    -12,
    -8,
    -8,
    -4,
    0,
    S16_MAX,
};

const u16 sUnk_8352ABE[] = {
    -8,
    -4,
    0,
    S16_MAX,
};

const u16 sUnk_8352AC6[] = {
    -16,
    -12,
    -12,
    -8,
    -4,
    0,
    S16_MAX,
};

const u16 sUnk_8352AD4[] = {
    -12,
    -8,
    0,
    S16_MAX,
};

const s16 sSpriteGravityVelocityTable[] = {
    4,
    4,
    4,
    4,
    4,
    4,
    8,
    8,
    8,
    12,
    12,
    12,
    12,
    12,
    16,
    S16_MAX,
};

const s16 sUnk_8352AFC[] = {
    2,
    4,
    6,
    8,
    8,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    18,
    S16_MAX,
};

const s16 sUnk_8352B18[] = {
    0,
    -20,
    -16,
    -12,
    -10,
    -8,
    -4,
    -2,
    0,
    S16_MAX,
};

const s16 sUnk_8352B2C[] = {
    0,
    0,
    0,
    -12,
    -10,
    -8,
    -4,
    -2,
    0,
    S16_MAX,
};

const s16 sUnk_8352B40[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    2,
    3,
    4,
    S16_MAX,
};

const s16 sSpriteThrownForwardSoftYVelocity[] = {
    -4,
    0,
    4,
    4,
    4,
    8,
    8,
    8,
    8,
    12,
    12,
    12,
    12,
    12,
    16,
    S16_MAX,
};

const s16 sSpriteThrownForwardSoftYVelocityHeavy[] = {
    -4,
    2,
    3,
    4,
    6,
    8,
    10,
    12,
    14,
    16,
    18,
    S16_MAX,
};

const u16 sSpriteThrownForwardHardYVelocity[] = {
    -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 4, 4, 4, 8, 8, 8, 12, 12, 16, S16_MAX,
};

const u16 sSpriteThrownForwardHardYVelocityHeavy[] = {
    -4, -4, 0, 0, 0, 0, 0, 0, 2, 4, 6, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, S16_MAX,
};

const u16 sSpriteThrownUpSoftYVelocity[] = {
    -20, -20, -16, -16, -16, -16, -12, -12, -12, -12, -12, -12, -8, -8, -4, -4,
    0,   0,   0,   4,   4,   4,   8,   8,   8,   12,  12,  12,  16, 16, 20, S16_MAX,
};

const u16 sSpriteThrownUpSoftYVelocityHeavy[] = {
    -16, -16, -12, -12, -10, -10, -8, -8, -4, -4, 0, 0, 0, 4, 4, 4, 8, 8, 12, 12, 12, 16, 16, 16, 20, S16_MAX,
};

const u16 sSpriteThrownUpHardYVelocity[] = {
    -36, -36, -36, -32, -32, -28, -28, -24, -24, -20, -20, -20, -16, -16, -16, -12,
    -12, -12, -8,  -8,  -4,  0,   0,   0,   0,   4,   4,   8,   12,  16,  20,  S16_MAX,
};

const u16 sSpriteThrownUpHardYVelocityHeavy[] = {
    -28, -24, -20, -20, -20, -16, -16, -16, -12, -12, -12, -12, -8, -8, -4,      -4,
    0,   0,   0,   4,   4,   8,   8,   12,  12,  16,  16,  20,  20, 24, S16_MAX,
};
#else
extern const s16 sSpriteFallingOffscreenYVelocity[];
extern const s16 sSpriteFallingOffscreenYVelocityHeavy[];
extern const s16 sUnk_8352A28[];
extern const s16 sUnk_8352A68[];
extern const s16 sUnk_8352AA2[];
extern const s16 sUnk_8352AAC[];
extern const u16 sUnk_8352AB2[];
extern const u16 sUnk_8352ABE[];
extern const u16 sUnk_8352AC6[];
extern const u16 sUnk_8352AD4[];
extern const s16 sSpriteGravityVelocityTable[];
extern const s16 sUnk_8352AFC[];
extern const s16 sUnk_8352B18[];
extern const s16 sUnk_8352B2C[];
extern const s16 sUnk_8352B40[];
extern const s16 sSpriteThrownForwardSoftYVelocity[];
extern const s16 sSpriteThrownForwardSoftYVelocityHeavy[];
extern const u16 sSpriteThrownForwardHardYVelocity[];
extern const u16 sSpriteThrownForwardHardYVelocityHeavy[];
extern const u16 sSpriteThrownUpSoftYVelocity[];
extern const u16 sSpriteThrownUpSoftYVelocityHeavy[];
extern const u16 sSpriteThrownUpHardYVelocity[];
extern const u16 sSpriteThrownUpHardYVelocityHeavy[];
#endif

void func_80238A4(void)
{
    if (gScreenShakeY.duration >= CONVERT_SECONDS(0.97)) {
        if (gStageFrameCounter & 1) {
            gCurrentSprite.pose = SPOSE_33;
        } else {
            gCurrentSprite.pose = SPOSE_35;
        }
    } else if (gScreenShakeY.duration >= CONVERT_SECONDS(0.7)) {
        gCurrentSprite.pose = SPOSE_4B;
    }
}

void func_80238E8(void)
{
    if (gScreenShakeY.duration >= CONVERT_SECONDS(0.7)) {
        if (gStageFrameCounter & 1) {
            gCurrentSprite.pose = SPOSE_33;
        } else {
            gCurrentSprite.pose = SPOSE_35;
        }
    }
}

void func_802391C(void)
{
    if (gScreenShakeY.duration >= CONVERT_SECONDS(0.7)) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            gCurrentSprite.pose = SPOSE_35;
        } else {
            gCurrentSprite.pose = SPOSE_33;
        }
    }
}

u32 func_8023944(u16 y, u16 x)
{
    s32 temp;
    s32 tempX;
    s32 tempY;
    u16 result;

    temp = GetSpriteBlockCollisionAtPosition(y, x);
    if (temp & 0x01000000) {
        gSpriteCollisionResult = 0x11;
    } else {
        gSpriteCollisionResult = 0;
    }

    temp &= 0xFF;
    switch (temp - 2) {
        case 1:
            tempY = y & 0xFFC0;
            tempX = (x & 0x3F) - 0x3F;
            result = tempY - tempX;
            gSpriteCollisionResult = 5;
            break;

        case 4:
            tempY = y & 0xFFC0;
            tempX = ((x & 0x3F) >> 1) - 0x3F;
            result = tempY - tempX;
            gSpriteCollisionResult = 3;
            break;

        case 5:
            tempY = 0xFFC0 & y;
            tempX = ((x & 0x3F) >> 1) - 0x1F;
            result = tempY - tempX;
            gSpriteCollisionResult = 3;
            break;

        case 0:
            result = (y & 0xFFC0) | (0x3F & x);
            gSpriteCollisionResult = 4;
            break;

        case 3:
            tempX = y & 0xFFC0;
            tempY = ((x & 0x3F) >> 1) + 0x1F;
            result = tempX | tempY;
            gSpriteCollisionResult = 2;
            break;

        case 2:
            tempX = y & 0xFFC0;
            tempY = (x & 0x3F) >> 1;
            result = tempX | tempY;
            gSpriteCollisionResult = 2;
            break;

        case 10:
            gSpriteCollisionResult = 0x10;
        default:
            result = y & 0xFFC0;
            break;
    }
    return result;
}

u32 func_8023A60(u16 y, u16 x)
{
    s32 temp1;
    s32 tempX;
    s32 tempY;
    u8 temp2;
    u16 result;

    temp1 = GetSpriteBlockCollisionAtPosition(y, x);
    if (temp1 & 0x01000000) {
        temp2 = 0x11;
    } else {
        temp2 = 0;
    }

    temp1 &= 0xFF;
    switch (temp1 - 2) {
        case 1:
            tempY = y & 0xFFC0;
            tempX = ((x & 0x3F) - 0x3F);
            result = tempY - tempX;
            temp2 = 5;
            break;

        case 4:
            tempY = y & 0xFFC0;
            tempX = ((x & 0x3F) >> 1) - 0x3F;
            result = tempY - tempX;
            temp2 = 3;
            break;

        case 5:
            tempY = y & 0xFFC0;
            tempX = ((x & 0x3F) >> 1) - 0x1F;
            result = tempY - tempX;
            temp2 = 3;
            break;

        case 0:
            tempY = y & 0xFFC0;
            tempX = x & 0x3F;
            result = tempY | tempX;
            temp2 = 4;
            break;

        case 3:
            tempY = y & 0xFFC0;
            tempX = ((x & 0x3F) >> 1) + 0x1F;
            result = tempY | tempX;
            temp2 = 2;
            break;

        case 2:
            tempY = y & 0xFFC0;
            tempX = (x & 0x3F) >> 1;
            result = tempY | tempX;
            temp2 = 2;
            break;

        case 10:
            temp2 = 0x10;
        default:
            result = y & 0xFFC0;
            break;
    }

    if (temp1 == 12) {
        if (y - result <= 25u) {
            gSpriteCollisionResult = temp2;
        } else {
            gSpriteCollisionResult = 0;
        }
    } else {
        if (y >= result) {
            gSpriteCollisionResult = temp2;
        } else {
            gSpriteCollisionResult = 0;
        }
    }
    return result;
}

void func_8023B88(void)
{
    u16 xPosition;
    u16 yPosition;
    u32 temp;

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;

    temp = func_8023944(yPosition - 4, xPosition);
    if ((gSpriteCollisionResult & 0xF) > 1) {
        gCurrentSprite.yPosition = temp;
        return;
    }

    temp = func_8023944(yPosition, xPosition);
    if ((gSpriteCollisionResult & 0xF) > 1) {
        gCurrentSprite.yPosition = temp;
        return;
    }

    temp = func_8023944(yPosition + 4, xPosition);
    if (gSpriteCollisionResult != 0) {
        gCurrentSprite.yPosition = temp;
        return;
    }
}

void func_8023BFC(u16 y, u16 x)
{
    s32 temp1;
    s32 temp2;

    temp1 = GetSpriteBlockCollisionAtPosition(y, x);
    temp2 = 0x01000000 & temp1;
    if (temp2 != 0) {
        gSpriteCollisionTileType = 0x11;
    } else {
        gSpriteCollisionTileType = temp2;
    }

    switch ((temp1 & 0xFF) - 2) {
        case 1:
            gSpriteCollisionTileType = 5;
            break;

        case 4:
            gSpriteCollisionTileType = 3;
            break;

        case 5:
            gSpriteCollisionTileType = 3;
            break;

        case 0:
            gSpriteCollisionTileType = 4;
            break;

        case 3:
            gSpriteCollisionTileType = 2;
            break;

        case 2:
            gSpriteCollisionTileType = 2;
            break;

        case 10:
            gSpriteCollisionTileType = 0x10;
            break;
    }
}

void func_8023C94(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = SPOSE_1B;
        return;
    }

    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_IDLE_INIT;
    }
}

void func_8023CD8(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = SPOSE_1D;
        return;
    }

    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 == 0) {
        if (gCurrentSprite.xPosition > gWarioData.xPosition) {
            if (!(gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)) {
                gCurrentSprite.pose = SPOSE_17;
            } else {
                gCurrentSprite.pose = SPOSE_11;
            }
        } else {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
                gCurrentSprite.pose = SPOSE_17;
            } else {
                gCurrentSprite.pose = SPOSE_11;
            }
        }
    }
}

void func_8023D48(void)
{
    u32 temp;

    temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult) {
        gCurrentSprite.yPosition = temp;
        gCurrentSprite.pose = SPOSE_17;
        return;
    }

    SpriteUtilLookupGravityByWeight(sUnk_8352A68, sUnk_8352A28);
}

void func_8023E00(void)
{
    u32 temp;

    temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult) {
        gCurrentSprite.yPosition = temp;
        if (gCurrentSprite.xPosition > gWarioData.xPosition) {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
                gCurrentSprite.pose = SPOSE_11;
            } else {
                gCurrentSprite.pose = SPOSE_17;
            }
        } else {
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
                gCurrentSprite.pose = SPOSE_17;
            } else {
                gCurrentSprite.pose = SPOSE_11;
            }
        }
        return;
    }

    SpriteUtilLookupGravityByWeight(sUnk_8352A68, sUnk_8352A28);
}

void func_8023EE0(void)
{
    u32 temp;

    temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult) {
        gCurrentSprite.yPosition = temp;
        if (gCurrentSprite.xPosition > gWarioData.xPosition) {
            gCurrentSprite.pose = SPOSE_33;
        } else {
            gCurrentSprite.pose = SPOSE_35;
        }
        return;
    }

    SpriteUtilLookupGravityByWeight(sUnk_8352AFC, sSpriteGravityVelocityTable);
}

void func_8023FA8(void)
{
    u32 temp;

    temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult) {
        gCurrentSprite.yPosition = temp;
        gCurrentSprite.pose = SPOSE_6C;
        return;
    }

    SpriteUtilLookupGravityByWeight(sUnk_8352AFC, sSpriteGravityVelocityTable);
}

void SpriteUtilFallOffscreenRight(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    gCurrentSprite.xPosition += gCurrentSprite.work2;
    SpriteUtilLookupGravityByWeight(sSpriteFallingOffscreenYVelocityHeavy, sSpriteFallingOffscreenYVelocity);
}

void SpriteUtilFallOffscreenLeft(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    gCurrentSprite.xPosition -= gCurrentSprite.work2;
    SpriteUtilLookupGravityByWeight(sSpriteFallingOffscreenYVelocityHeavy, sSpriteFallingOffscreenYVelocity);
}

void func_8024180(void)
{
    TIMER_COUNT_UP(gCurrentSprite.work1);
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
    );
    if (gSpriteCollisionTileType == 0x11) {
        if (gCurrentSprite.work2 < 4) {
            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.work2--;
            }
        } else {
            gCurrentSprite.work2--;
        }
        gCurrentSprite.pose = SPOSE_2D;
        m4aSongNumStart(SE_WALL_BOUNCE);
        return;
    }

    gCurrentSprite.xPosition -= gCurrentSprite.work2;
    func_8023B88();
    if (!(gSpriteCollisionResult & 0xE)) {
        if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2C)) {
            gCurrentSprite.pose = SPOSE_43;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        if (gSpriteCollisionTileType == 0) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
            if (gSpriteCollisionTileType == 0) {
                gCurrentSprite.pose = SPOSE_47;
                return;
            }
        }
        if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
            if (!(gCurrentSprite.work1 & 1)) {
                gCurrentSprite.work2--;
            }
        } else {
            if (!(gCurrentSprite.work1 & 3)) {
                gCurrentSprite.work2--;
            }
        }
    } else if ((gSpriteCollisionResult & 0xF1) == 1) {
        if (gCurrentSprite.pose != SPOSE_2C) {
            gCurrentSprite.pose = SPOSE_47;
        }
    } else {
        if (gCurrentSprite.work2 > 4) {
            gCurrentSprite.work2--;
            return;
        }
        gCurrentSprite.pose = SPOSE_45;
    }
}

void func_80242C8(void)
{
    gCurrentSprite.work1 += 1;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
    );
    if (gSpriteCollisionTileType == 0x11) {
        if (gCurrentSprite.work2 < 4) {
            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.work2--;
            }
        } else {
            gCurrentSprite.work2--;
        }
        gCurrentSprite.pose = SPOSE_2B;
        m4aSongNumStart(SE_WALL_BOUNCE);
        return;
    }

    gCurrentSprite.xPosition += gCurrentSprite.work2;
    func_8023B88();
    if (!(gSpriteCollisionResult & 0xE)) {
        if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2E)) {
            gCurrentSprite.pose = SPOSE_45;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        if (gSpriteCollisionTileType == 0) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (gSpriteCollisionTileType == 0) {
                gCurrentSprite.pose = SPOSE_49;
                return;
            }
        }
        if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
            if (!(gCurrentSprite.work1 & 1)) {
                gCurrentSprite.work2--;
            }
        } else {
            if (!(gCurrentSprite.work1 & 3)) {
                gCurrentSprite.work2--;
            }
        }
    } else if (!(0xF1 & gSpriteCollisionResult)) {
        if (gCurrentSprite.pose != SPOSE_2E) {
            gCurrentSprite.pose = SPOSE_49;
        }
    } else {
        if (gCurrentSprite.work2 > 4) {
            gCurrentSprite.work2--;
            return;
        }
        gCurrentSprite.pose = SPOSE_43;
    }
}

void SpriteUtilPushedLeft(void)
{
    func_80238A4();
    if (gCurrentSprite.work2 > 2) {
        func_8024180();
        return;
    }

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_11;
    }
}

void SpriteUtilPushedRight(void)
{
    func_80238A4();
    if (gCurrentSprite.work2 > 2) {
        func_80242C8();
        return;
    }

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_11;
    }
}

void func_8024478(void)
{
    func_80238E8();
    if (gCurrentSprite.work2 > 2) {
        func_8024180();
        return;
    }

    gCurrentSprite.pose = SPOSE_2F;
}

void func_802449C(void)
{
    func_80238E8();
    if (gCurrentSprite.work2 > 2) {
        func_80242C8();
        return;
    }

    gCurrentSprite.pose = SPOSE_2F;
}

void func_80244C0(void)
{
    gCurrentSprite.pose = SPOSE_30;
    gCurrentSprite.work0 = 0x50;
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
}

void func_80244E0(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = SPOSE_1D;
        return;
    }

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_19;
    }
}

void SpriteUtilDieAfterDelay(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = SPRITE_STATUS_NONE;
        SpawnSecondarySprite(
            gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition, SSPRITE_ENEMY_KILLED_EFFECT
        );
    }
}

void SpriteUtilDie(void)
{
    gCurrentSprite.status = SPRITE_STATUS_NONE;
    SpawnSecondarySprite(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition, SSPRITE_ENEMY_KILLED_EFFECT
    );
}

void func_8024570(void)
{
    u8 pose;

    gCurrentSprite.xPosition -= gCurrentSprite.work2;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
    );
    if ((gSpriteCollisionTileType == 0x11) ||
        (func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition),
         ((0xF & gSpriteCollisionTileType) != 0))) {
        pose = SPOSE_45;
        m4aSongNumStart(SE_WALL_BOUNCE);
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0) {
            return;
        }
        pose = SPOSE_43;
    }
    gCurrentSprite.pose = pose;
    gCurrentSprite.status &= ~SPRITE_STATUS_MAYBE_DEAD;
}

void func_80245FC(void)
{
    u8 pose;

    gCurrentSprite.xPosition += gCurrentSprite.work2;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
    );
    if ((gSpriteCollisionTileType == 0x11) ||
        (func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition),
         ((0xF & gSpriteCollisionTileType) != 0))) {
        pose = SPOSE_43;
        m4aSongNumStart(SE_WALL_BOUNCE);
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0) {
            return;
        }
        pose = SPOSE_45;
    }
    gCurrentSprite.pose = pose;
    gCurrentSprite.status &= ~SPRITE_STATUS_MAYBE_DEAD;
}

void func_8024688(void)
{
    gCurrentSprite.pose = SPOSE_38;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work0 = 1;
    } else {
        gCurrentSprite.work0 = 4;
    }
}

void func_80246B8(void)
{
    SpriteUtilLookupGravityByWeight(sUnk_8352AAC, sUnk_8352AA2);
    func_8024570();
}

void func_802473C(void)
{
    gCurrentSprite.pose = SPOSE_3A;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 5;
    }
}

void func_802476C(void)
{
    SpriteUtilLookupGravityByWeight(sUnk_8352ABE, sUnk_8352AB2);
    func_8024570();
}

void func_80247F0(void)
{
    gCurrentSprite.pose = SPOSE_3C;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 6;
    }
}

void func_8024820(void)
{
    SpriteUtilLookupGravityByWeight(sUnk_8352AD4, sUnk_8352AC6);
    func_8024570();
}

void func_80248A4(void)
{
    gCurrentSprite.pose = SPOSE_3E;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work0 = 1;
    } else {
        gCurrentSprite.work0 = 4;
    }
}

void func_80248D4(void)
{
    SpriteUtilLookupGravityByWeight(sUnk_8352AAC, sUnk_8352AA2);
    func_80245FC();
}

void func_8024958(void)
{
    gCurrentSprite.pose = SPOSE_40;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 5;
    }
}

void func_8024988(void)
{
    SpriteUtilLookupGravityByWeight(sUnk_8352ABE, sUnk_8352AB2);
    func_80245FC();
}

void func_8024A0C(void)
{
    gCurrentSprite.pose = SPOSE_42;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 6;
    }
}

void func_8024A3C(void)
{
    SpriteUtilLookupGravityByWeight(sUnk_8352AD4, sUnk_8352AC6);
    func_80245FC();
}

void func_8024AC0(void)
{
    gCurrentSprite.pose = SPOSE_44;
    gCurrentSprite.work3 = 0;
}

void func_8024AD4(void)
{
    s16 yVelocity;
    u32 timer;
    u32 temp;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sUnk_8352AFC, sSpriteGravityVelocityTable);
    gCurrentSprite.xPosition -= gCurrentSprite.work2;
    temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }

    if (gSpriteCollisionResult != 0) {
        gCurrentSprite.yPosition = temp;
        if (gCurrentSprite.work2 > 7) {
            gCurrentSprite.work2 -= 4;
        }

        if (yVelocity < 8) {
            gCurrentSprite.pose = SPOSE_2B;
        } else if (yVelocity < 12) {
            gCurrentSprite.pose = SPOSE_37;
        } else if (yVelocity < 16) {
            gCurrentSprite.pose = SPOSE_39;
        } else {
            gCurrentSprite.pose = SPOSE_3B;
        }
        m4aSongNumStart(SOUND_36);
        return;
    }

    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
    );
    if (gSpriteCollisionTileType == 0x11) {
        gCurrentSprite.pose = SPOSE_45;
        m4aSongNumStart(SE_WALL_BOUNCE);
    }
}

void func_8024BEC(void)
{
    gCurrentSprite.pose = SPOSE_46;
    gCurrentSprite.work3 = 0;
}

void func_8024C00(void)
{
    s16 yVelocity;
    u32 timer;
    u32 temp;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sUnk_8352AFC, sSpriteGravityVelocityTable);
    gCurrentSprite.xPosition += gCurrentSprite.work2;
    temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }

    if (gSpriteCollisionResult != 0) {
        gCurrentSprite.yPosition = temp;
        if (gCurrentSprite.work2 > 7) {
            gCurrentSprite.work2 -= 4;
        }

        if (yVelocity < 8) {
            gCurrentSprite.pose = SPOSE_2D;
        } else if (yVelocity < 12) {
            gCurrentSprite.pose = SPOSE_3D;
        } else if (yVelocity < 16) {
            gCurrentSprite.pose = SPOSE_3F;
        } else {
            gCurrentSprite.pose = SPOSE_41;
        }
        m4aSongNumStart(SOUND_36);
        return;
    }

    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
    );
    if (gSpriteCollisionTileType == 0x11) {
        gCurrentSprite.pose = SPOSE_43;
        m4aSongNumStart(SE_WALL_BOUNCE);
    }
}

void SpriteUtilLiftingSpriteRight(void)
{
    s32 clipCheck;
    u16 yPositionBackup;
    u16 xPositionBackup;

    clipCheck = 0;
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentCarriedSprite.state) {
        case 4:
            if (gWarioData.pose == 4) {
                gCurrentSprite.pose = SPOSE_33;
                gCurrentCarriedSprite.state = 0;
            } else {
                gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
                gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
                gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;
            }
            return;

        case 1:
        case 2:
        case 3:
            yPositionBackup = gCurrentSprite.yPosition;
            xPositionBackup = gCurrentSprite.xPosition;
            gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
            gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;

            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
            );
            if (gSpriteCollisionTileType == 0x11) {
                clipCheck = 1;
            }
            func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
            if (gSpriteCollisionTileType == 0x11) {
                clipCheck = 2;
            }
            if (clipCheck == 0) {
                return;
            }

            gCurrentSprite.yPosition = yPositionBackup;
            gCurrentSprite.xPosition = xPositionBackup;
            if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
                gCurrentSprite.work2 = 6;
                gCurrentSprite.pose = SPOSE_47;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = 0;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            if (clipCheck == 2) {
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_47;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = 0;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            gCurrentSprite.pose = SPOSE_CARRIED_RIGHT_INIT;
            gCurrentCarriedSprite.unk1 = 1;
            gCurrentCarriedSprite.state = 4;
            WarioRequestPose(WPOSE_NORMAL_STANDING);
            return;

        default:
            gCurrentSprite.pose = SPOSE_1D;
            return;
    }
}

void SpriteUtilLiftingSpriteLeft(void)
{
    s32 clipCheck;
    u16 yPositionBackup;
    u16 xPositionBackup;

    clipCheck = 0;
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentCarriedSprite.state) {
        case 4:
            if (gWarioData.pose == 4) {
                gCurrentSprite.pose = SPOSE_35;
                gCurrentCarriedSprite.state = 0;
            } else {
                gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
                gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
                gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;
            }
            return;

        case 1:
        case 2:
        case 3:
            yPositionBackup = gCurrentSprite.yPosition;
            xPositionBackup = gCurrentSprite.xPosition;
            gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
            gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;

            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
            );
            if (gSpriteCollisionTileType == 0x11) {
                clipCheck = 1;
            }
            func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
            if (gSpriteCollisionTileType == 0x11) {
                clipCheck = 2;
            }
            if (clipCheck == 0) {
                return;
            }

            gCurrentSprite.yPosition = yPositionBackup;
            gCurrentSprite.xPosition = xPositionBackup;
            if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
                gCurrentSprite.work2 = 6;
                gCurrentSprite.pose = SPOSE_49;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = 0;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            if (clipCheck == 2) {
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_49;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = 0;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            gCurrentSprite.pose = SPOSE_CARRIED_LEFT_INIT;
            gCurrentCarriedSprite.unk1 = 1;
            gCurrentCarriedSprite.state = 4;
            WarioRequestPose(WPOSE_NORMAL_STANDING);
            return;

        default:
            gCurrentSprite.pose = SPOSE_1D;
            return;
    }
}

void SpriteUtilCarryingSpriteRight(void)
{
    u16 yPositionBackup;
    u16 xPositionBackup;
    u32 temp;

    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentCarriedSprite.state - 4) {
        case 0:
        case 4:
            gCurrentSprite.status |= SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
            gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;
            if (gWarioData.horizontalDirection & DPAD_LEFT) {
                gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
                gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
                return;
            }

            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_47;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = SPRITE_STATUS_NONE;
                gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            if (gWarioData.yVelocity >= 0) {
                func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
                if (!(0xF & gSpriteCollisionTileType)) {
                    return;
                }
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_49;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = SPRITE_STATUS_NONE;
                gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
            if (gBackgroundCollisionData.waterType == 1) {
                gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
            }
            if (!(0xF & gSpriteCollisionResult)) {
                return;
            }

            gCurrentSprite.yPosition = temp;
            gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
            gCurrentSprite.pose = SPOSE_37;
            gCurrentSprite.work2 = 4;
            gCurrentCarriedSprite.state = 0;
            gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            m4aSongNumStart(SOUND_36);
            return;

        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
            if (gWarioData.horizontalDirection & DPAD_LEFT) {
                gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
                gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
            }
            yPositionBackup = gCurrentSprite.yPosition;
            xPositionBackup = gCurrentSprite.xPosition;
            gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
            gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;

            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.yPosition = yPositionBackup;
                gCurrentSprite.xPosition = xPositionBackup;
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_49;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                func_801E4B0();
                gCurrentCarriedSprite.state = SPRITE_STATUS_NONE;
                gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                m4aSongNumStart(SE_WALL_BOUNCE);
            }
            return;

        case 8:
            gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
            gCurrentSprite.work0 = 0;
            gCurrentSprite.work3 = 0;
            if (gCurrentCarriedSprite.unk1 & 0x80) {
                gCurrentSprite.work2 = 4;
                if (gWarioData.pose == 0x25) {
                    gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_HARD_INIT;
                } else {
                    gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_SOFT_INIT;
                }
                return;
            }

            if (gWarioData.pose == 0x25) {
                gCurrentSprite.pose = SPOSE_THROWN_RIGHT_HARD_INIT;
                if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
                    gCurrentSprite.work2 = 0x10;
                } else {
                    gCurrentSprite.work2 = 0x1B;
                }
                return;
            }

            gCurrentSprite.work2 = 0xC;
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_SOFT_INIT;
            return;

        default:
            if (gWarioData.pose == 5) {
                gCurrentSprite.pose = SPOSE_49;
            } else {
                gCurrentSprite.pose = SPOSE_1D;
            }
            gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
            gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            return;
    }
}

void SpriteUtilCarryingSpriteLeft(void)
{
    u16 yPositionBackup;
    u16 xPositionBackup;
    u32 temp;

    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentCarriedSprite.state - 4) {
        case 0:
        case 4:
            gCurrentSprite.status |= SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
            gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;
            if (gWarioData.horizontalDirection & DPAD_RIGHT) {
                gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
                gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
                return;
            }

            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_49;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = SPRITE_STATUS_NONE;
                gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            if (gWarioData.yVelocity >= 0) {
                func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
                if (!(0xF & gSpriteCollisionTileType)) {
                    return;
                }
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_47;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                gCurrentCarriedSprite.state = SPRITE_STATUS_NONE;
                gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                m4aSongNumStart(SE_WALL_BOUNCE);
                return;
            }

            temp = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (gBackgroundCollisionData.waterType == 1) {
                gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
            }
            if (!(0xF & gSpriteCollisionResult)) {
                return;
            }

            gCurrentSprite.yPosition = temp;
            gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
            gCurrentSprite.pose = SPOSE_3D;
            gCurrentSprite.work2 = 4;
            gCurrentCarriedSprite.state = 0;
            gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            m4aSongNumStart(SOUND_36);
            return;

        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
            if (gWarioData.horizontalDirection & DPAD_RIGHT) {
                gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
                gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
            }
            yPositionBackup = gCurrentSprite.yPosition;
            xPositionBackup = gCurrentSprite.xPosition;
            gCurrentSprite.yPosition = gWarioData.yPosition + gCurrentCarriedSprite.yOffset;
            gCurrentSprite.xPosition = gWarioData.xPosition + gCurrentCarriedSprite.xOffset;

            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.yPosition = yPositionBackup;
                gCurrentSprite.xPosition = xPositionBackup;
                gCurrentSprite.work2 = 8;
                gCurrentSprite.pose = SPOSE_47;
                gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
                func_801E4B0();
                gCurrentCarriedSprite.state = SPRITE_STATUS_NONE;
                gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                m4aSongNumStart(SE_WALL_BOUNCE);
            }
            return;

        case 8:
            gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.5) + DELTA_TIME;
            gCurrentSprite.work0 = 0;
            gCurrentSprite.work3 = 0;
            if (gCurrentCarriedSprite.unk1 & 0x80) {
                gCurrentSprite.work2 = 4;
                if (gWarioData.pose == 0x25) {
                    gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_HARD_INIT;
                } else {
                    gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_SOFT_INIT;
                }
                return;
            }

            if (gWarioData.pose == 0x25) {
                gCurrentSprite.pose = SPOSE_THROWN_LEFT_HARD_INIT;
                if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
                    gCurrentSprite.work2 = 0x10;
                } else {
                    gCurrentSprite.work2 = 0x1B;
                }
                return;
            }

            gCurrentSprite.work2 = 0xC;
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_SOFT_INIT;
            return;

        default:
            if (gWarioData.pose == 5) {
                gCurrentSprite.pose = SPOSE_47;
            } else {
                gCurrentSprite.pose = SPOSE_1D;
            }
            gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
            gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
            return;
    }
}

void func_80254E8(void)
{
    u32 temp;

    if (gCurrentSprite.work0 != 0) {
        gSpriteTileInteractionMode = 1;
        temp = func_8023A60(gCurrentSprite.yPosition - PIXEL_SIZE, gCurrentSprite.xPosition);
        if (gBackgroundCollisionData.waterType == 1) {
            gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
        }
        if (gSpriteCollisionResult != 0) {
            func_8023BFC(gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition);
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.xPosition += gCurrentSprite.hitboxExtentLeft;
            } else {
                gCurrentSprite.yPosition = temp;
                if (gCurrentSprite.work2 > 0xF) {
                    gCurrentSprite.work2 = gCurrentSprite.work2 >> 1;
                } else if (gCurrentSprite.work2 > 7) {
                    gCurrentSprite.work2 = gCurrentSprite.work2 - 4;
                }
                gCurrentSprite.pose = SPOSE_39;
                m4aSongNumStart(SOUND_36);
                return;
            }
        }
    }

    gSpriteTileInteractionMode = 1;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
    );
    if (gSpriteCollisionTileType == 0x11) {
        if ((gCurrentSprite.pose == SPOSE_THROWN_LEFT_SOFT) || (gCurrentSprite.pose == SPOSE_THROWN_UP_LEFT_SOFT) ||
            (gCurrentSprite.work0 == 2)) {
            gCurrentSprite.pose = SPOSE_49;
            m4aSongNumStart(SE_WALL_BOUNCE);
        } else {
            gCurrentSprite.pose = SPOSE_6B;
        }
        return;
    }

    gSpriteTileInteractionMode = 1;
    func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
    if (0xF & gSpriteCollisionTileType) {
        if ((gCurrentSprite.pose == SPOSE_THROWN_LEFT_HARD) || (gCurrentSprite.pose == SPOSE_THROWN_UP_LEFT_HARD)) {
            gCurrentSprite.pose = SPOSE_6B;
        } else {
            gCurrentSprite.pose = SPOSE_47;
            m4aSongNumStart(SE_WALL_BOUNCE);
        }
        return;
    }

    gCurrentSprite.xPosition -= gCurrentSprite.work2;
}

void SpriteUtilThrownLeftSoft(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownForwardSoftYVelocityHeavy, sSpriteThrownForwardSoftYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80254E8();
}

void SpriteUtilThrownLeftHard(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownForwardHardYVelocityHeavy, sSpriteThrownForwardHardYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80254E8();
}

void SpriteUtilThrownUpLeftSoft(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownUpSoftYVelocityHeavy, sSpriteThrownUpSoftYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80254E8();
}

void SpriteUtilThrownUpLeftHard(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownUpHardYVelocityHeavy, sSpriteThrownUpHardYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80254E8();
}

void func_80258B4(void)
{
    u32 temp;

    if (gCurrentSprite.work0 != 0) {
        gSpriteTileInteractionMode = 1;
        temp = func_8023A60(gCurrentSprite.yPosition - PIXEL_SIZE, gCurrentSprite.xPosition);
        if (gBackgroundCollisionData.waterType == 1) {
            gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
        }
        if (gSpriteCollisionResult != 0) {
            func_8023BFC(gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition);
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.xPosition -= gCurrentSprite.hitboxExtentRight;
            } else {
                gCurrentSprite.yPosition = temp;
                if (gCurrentSprite.work2 > 0xF) {
                    gCurrentSprite.work2 = gCurrentSprite.work2 >> 1;
                } else if (gCurrentSprite.work2 > 7) {
                    gCurrentSprite.work2 = gCurrentSprite.work2 - 4;
                }
                gCurrentSprite.pose = SPOSE_3F;
                m4aSongNumStart(SOUND_36);
                return;
            }
        }
    }

    gSpriteTileInteractionMode = 1;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
    );
    if (gSpriteCollisionTileType == 0x11) {
        if ((gCurrentSprite.pose == SPOSE_THROWN_RIGHT_SOFT) || (gCurrentSprite.pose == SPOSE_THROWN_UP_RIGHT_SOFT) ||
            (gCurrentSprite.work0 == 2)) {
            gCurrentSprite.pose = SPOSE_47;
            m4aSongNumStart(SE_WALL_BOUNCE);
        } else {
            gCurrentSprite.pose = SPOSE_6B;
        }
        return;
    }

    gSpriteTileInteractionMode = 1;
    func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
    if (0xF & gSpriteCollisionTileType) {
        if ((gCurrentSprite.pose == SPOSE_THROWN_RIGHT_HARD) || (gCurrentSprite.pose == SPOSE_THROWN_UP_RIGHT_HARD)) {
            gCurrentSprite.pose = SPOSE_6B;
        } else {
            gCurrentSprite.pose = SPOSE_49;
            m4aSongNumStart(SE_WALL_BOUNCE);
        }
        return;
    }

    gCurrentSprite.xPosition += gCurrentSprite.work2;
}

void SpriteUtilThrownRightSoft(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownForwardSoftYVelocityHeavy, sSpriteThrownForwardSoftYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80258B4();
}

void SpriteUtilThrownRightHard(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownForwardHardYVelocityHeavy, sSpriteThrownForwardHardYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80258B4();
}

void SpriteUtilThrownUpRightSoft(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownUpSoftYVelocityHeavy, sSpriteThrownUpSoftYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80258B4();
}

void SpriteUtilThrownUpRightHard(void)
{
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sSpriteThrownUpHardYVelocityHeavy, sSpriteThrownUpHardYVelocity);
    if (yVelocity == 0) {
        gCurrentSprite.work0 = 1;
    } else if (yVelocity > 0) {
        gCurrentSprite.work0 = 2;
    } else {
        gCurrentSprite.work0 = 0;
    }
    func_80258B4();
}

void func_8025C80(void)
{
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
    );
    if (gSpriteCollisionTileType != 0x11) {
        gCurrentSprite.xPosition += gCurrentSprite.work2;
        func_8023B88();
        if (!(0xE & gSpriteCollisionResult)) {
            if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2E)) {
                gCurrentSprite.pose = SPOSE_45;
                return;
            }
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
            if (gSpriteCollisionTileType == 0) {
                func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
                if (gSpriteCollisionTileType == 0) {
                    gCurrentSprite.pose = SPOSE_49;
                }
            }
        } else if (0xF1 & gSpriteCollisionResult) {
            gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
        }
    } else {
        gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    }
}

void func_8025D34(void)
{
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
    );
    if (gSpriteCollisionTileType != 0x11) {
        gCurrentSprite.xPosition -= gCurrentSprite.work2;
        func_8023B88();
        if (!(0xE & gSpriteCollisionResult)) {
            if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2C)) {
                gCurrentSprite.pose = SPOSE_43;
                return;
            }
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (gSpriteCollisionTileType == 0) {
                func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
                if (gSpriteCollisionTileType == 0) {
                    gCurrentSprite.pose = SPOSE_47;
                }
            }
        } else if ((0xF1 & gSpriteCollisionResult) != 1) {
            gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
        }
    } else {
        gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
    }
}

void func_8025DE8(void)
{
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
    );
    if (gSpriteCollisionTileType == 0x11) {
        gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
        return;
    }

    gCurrentSprite.xPosition += gCurrentSprite.work2;
    func_8023B88();
    if (!(0xE & gSpriteCollisionResult)) {
        if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2E)) {
            gCurrentSprite.pose = SPOSE_45;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        if (gSpriteCollisionTileType) {
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        if (gSpriteCollisionTileType) {
            return;
        }
        gCurrentSprite.pose = SPOSE_49;
    }
}

void func_8025E98(void)
{
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
    );
    if (gSpriteCollisionTileType == 0x11) {
        gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED_INIT;
        return;
    }

    gCurrentSprite.xPosition -= gCurrentSprite.work2;
    func_8023B88();
    if (!(0xE & gSpriteCollisionResult)) {
        if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2C)) {
            gCurrentSprite.pose = SPOSE_43;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        if (gSpriteCollisionTileType) {
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        if (gSpriteCollisionTileType) {
            return;
        }
        gCurrentSprite.pose = SPOSE_47;
    }
}

void func_8025F48(void)
{
    gCurrentSprite.work0 += 1;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - (BLOCK_SIZE - EIGHTH_BLOCK_SIZE)
    );
    if (gSpriteCollisionTileType == 0x11) {
        if (gCurrentSprite.work2 > 3) {
            gCurrentSprite.work2 -= 1;
        }
        func_8023BFC(
            gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + (BLOCK_SIZE - EIGHTH_BLOCK_SIZE)
        );
        if (gSpriteCollisionTileType == 0x11) {
            gCurrentSprite.work2 = 0;
            gCurrentSprite.pose = SPOSE_2F;
        } else {
            gCurrentSprite.pose = SPOSE_2D;
        }
        m4aSongNumStart(SE_WALL_BOUNCE);
        return;
    }

    gCurrentSprite.xPosition -= gCurrentSprite.work2;
    func_8023B88();
    if (!(0xE & gSpriteCollisionResult)) {
        if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2C)) {
            gCurrentSprite.pose = SPOSE_43;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        if (gSpriteCollisionTileType == 0) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
            if (gSpriteCollisionTileType == 0) {
                gCurrentSprite.pose = SPOSE_47;
                return;
            }
        }
        if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
            if (!(gCurrentSprite.work0 & 1)) {
                gCurrentSprite.work2 -= 1;
            }
        } else {
            if (!(gCurrentSprite.work0 & 3)) {
                gCurrentSprite.work2 -= 1;
            }
        }
    } else if ((0xF1 & gSpriteCollisionResult) == 1) {
        if (gCurrentSprite.pose != SPOSE_2C) {
            gCurrentSprite.pose = SPOSE_47;
        }
    } else {
        if (gCurrentSprite.work2 > 4U) {
            gCurrentSprite.work2 -= 1;
            return;
        }
        gCurrentSprite.pose = SPOSE_45;
    }
}

void func_802608C(void)
{
    gCurrentSprite.work0 += 1;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + (BLOCK_SIZE - EIGHTH_BLOCK_SIZE)
    );
    if (gSpriteCollisionTileType == 0x11) {
        if (gCurrentSprite.work2 > 3) {
            gCurrentSprite.work2 -= 1;
        }
        func_8023BFC(
            gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - (BLOCK_SIZE - EIGHTH_BLOCK_SIZE)
        );
        if (gSpriteCollisionTileType == 0x11) {
            gCurrentSprite.work2 = 0;
            gCurrentSprite.pose = SPOSE_2F;
        } else {
            gCurrentSprite.pose = SPOSE_2B;
        }
        m4aSongNumStart(SE_WALL_BOUNCE);
        return;
    }

    gCurrentSprite.xPosition += gCurrentSprite.work2;
    func_8023B88();
    if (!(0xE & gSpriteCollisionResult)) {
        if ((gSpriteCollisionResult == 0) && (gCurrentSprite.pose == SPOSE_2E)) {
            gCurrentSprite.pose = SPOSE_45;
            return;
        }
        func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        if (gSpriteCollisionTileType == 0) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
            if (gSpriteCollisionTileType == 0) {
                gCurrentSprite.pose = SPOSE_49;
                return;
            }
        }
        if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
            if (!(gCurrentSprite.work0 & 1)) {
                gCurrentSprite.work2 -= 1;
            }
        } else {
            if (!(gCurrentSprite.work0 & 3)) {
                gCurrentSprite.work2 -= 1;
            }
        }
    } else if ((0xF1 & gSpriteCollisionResult) == 0) {
        if (gCurrentSprite.pose != SPOSE_2E) {
            gCurrentSprite.pose = SPOSE_49;
        }
    } else {
        if (gCurrentSprite.work2 > 4U) {
            gCurrentSprite.work2 -= 1;
            return;
        }
        gCurrentSprite.pose = SPOSE_43;
    }
}

u32 SpriteUtilFindParentSlot(u8 parentId)
{
    s32 i;
    u8 roomSlot;

    roomSlot = gCurrentSprite.roomSlot;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].roomSlot == roomSlot && gSpriteData[i].globalID == parentId &&
            (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            return i;
        }
    }
    return MAX_SPRITE_COUNT;
}

u32 SpriteUtilWaitCheckWarioNearbyLeftRight(s32 maxDY, s32 maxDX)
{
    u16 spriteX;
    u16 spriteY;
    u16 warioX;
    u16 warioY;
    u32 result;

    result = NS_OUT_OF_RANGE;
    if (gCurrentSprite.work1 != 0) {
        gCurrentSprite.work1 -= 1;
        return NS_OUT_OF_RANGE;
    }

    warioY = gWarioData.yPosition;
    warioX = gWarioData.xPosition;
    spriteY = gCurrentSprite.yPosition;
    spriteX = gCurrentSprite.xPosition;

    if (spriteY > warioY) {
        if (spriteY - warioY >= maxDY) {
            return NS_OUT_OF_RANGE;
        }
    } else if (warioY - spriteY >= maxDY) {
        return NS_OUT_OF_RANGE;
    }

    if (spriteX > warioX) {
        if (spriteX - warioX < maxDX) {
            result = NS_LEFT;
        }
    } else if (warioX - spriteX < maxDX) {
        result = NS_RIGHT;
    }
    return result;
}

u32 SpriteUtilCheckWarioNearbyLeftRight(s32 maxDY, s32 maxDX)
{
    u16 spriteX;
    u16 spriteY;
    u16 warioX;
    u16 warioY;
    u32 result;

    result = NS_OUT_OF_RANGE;
    warioY = gWarioData.yPosition;
    warioX = gWarioData.xPosition;
    spriteY = gCurrentSprite.yPosition;
    spriteX = gCurrentSprite.xPosition;

    if (spriteY > warioY) {
        if (spriteY - warioY >= maxDY) {
            return NS_OUT_OF_RANGE;
        }
    } else if (warioY - spriteY >= maxDY) {
        return NS_OUT_OF_RANGE;
    }

    if (spriteX > warioX) {
        if (spriteX - warioX < maxDX) {
            result = NS_LEFT;
        }
    } else if (warioX - spriteX < maxDX) {
        result = NS_RIGHT;
    }
    return result;
}

u32 SpriteUtilCheckWarioNearbyAboveBelow(s32 maxDY, s32 maxDX)
{
    u16 spriteX;
    u16 spriteY;
    u16 warioX;
    u16 warioY;
    u32 result;

    result = NS_OUT_OF_RANGE;
    warioY = gWarioData.yPosition;
    warioX = gWarioData.xPosition;
    spriteY = gCurrentSprite.yPosition;
    spriteX = gCurrentSprite.xPosition;

    if (spriteX > warioX) {
        if (spriteX - warioX >= maxDX) {
            return NS_OUT_OF_RANGE;
        }
    } else if (warioX - spriteX >= maxDX) {
        return NS_OUT_OF_RANGE;
    }

    if (spriteY > warioY) {
        if (spriteY - warioY < maxDY) {
            result = NS_ABOVE;
        }
    } else if (warioY - spriteY < maxDY) {
        result = NS_BELOW;
    }
    return result;
}

void SpriteUtilTurnTowardWario(void)
{
    if (gCurrentSprite.xPosition > gWarioData.xPosition) {
        gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    } else {
        gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    }
}

void SpriteUtilMoveForward1Subpixel(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += 1;
    } else {
        gCurrentSprite.xPosition -= 1;
    }
}

void SpriteUtilMoveForward1HalfPixel(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += PIXEL_SIZE / 2;
    } else {
        gCurrentSprite.xPosition -= PIXEL_SIZE / 2;
    }
}

void func_8026374(void)
{
    u16 xVelocity;

    switch (gSpriteCollisionResult) {
        case 2:
        case 3:
            xVelocity = 2;
            break;
        case 4:
        case 5:
            xVelocity = 1;
            break;
        default:
            xVelocity = 3;
            break;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += xVelocity;
    } else {
        gCurrentSprite.xPosition -= xVelocity;
    }
}

void func_80263AC(void)
{
    u16 xVelocity;

    switch (gSpriteCollisionResult) {
        case 4:
        case 5:
            xVelocity = 1;
            break;
        default:
            xVelocity = 2;
            break;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += xVelocity;
    } else {
        gCurrentSprite.xPosition -= xVelocity;
    }
}

void func_80263E0(void)
{
    u16 xVelocity;

    switch (gSpriteCollisionResult) {
        case 2:
        case 3:
            xVelocity = 3;
            break;
        case 4:
        case 5:
            xVelocity = 2;
            break;
        default:
            xVelocity = 4;
            break;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += xVelocity;
    } else {
        gCurrentSprite.xPosition -= xVelocity;
    }
}

void func_8026418(void)
{
    u16 xVelocity;

    switch (gSpriteCollisionResult) {
        case 2:
        case 3:
            xVelocity = 7;
            break;
        case 4:
        case 5:
            xVelocity = 6;
            break;
        default:
            xVelocity = 8;
            break;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.xPosition += xVelocity;
    } else {
        gCurrentSprite.xPosition -= xVelocity;
    }
}

void func_8026450(void)
{
    s16 xVelocity;
    s16 yVelocity;
    u32 timer;

    timer = gCurrentSprite.work3;
    yVelocity = sUnk_83B35F8[timer][0];
    if (yVelocity == S16_MAX) {
        timer = 0;
        yVelocity = sUnk_83B35F8[0][0];
    }
    xVelocity = sUnk_83B35F8[timer][1];

    gCurrentSprite.work3 = timer + 1;
    gCurrentSprite.yPosition += yVelocity;
    gCurrentSprite.xPosition += xVelocity;
}

void func_80264A0(void)
{
    s16 xVelocity;
    s16 yVelocity;
    u32 timer;

    timer = gCurrentSprite.work3;
    yVelocity = sUnk_83B35F8[timer][0];
    if (yVelocity == S16_MAX) {
        timer = 0;
        yVelocity = sUnk_83B35F8[0][0];
    }
    xVelocity = sUnk_83B35F8[timer][1];

    gCurrentSprite.work3 = timer + 1;
    gCurrentSprite.yPosition += yVelocity;
    gCurrentSprite.xPosition += xVelocity;

    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON) {
        gWarioData.yPosition += yVelocity;
        gWarioData.xPosition += xVelocity;
    }
}

void func_802650C(void)
{
    s16 xVelocity;
    s16 yVelocity;
    u32 timer;

    timer = gCurrentSprite.work3;
    yVelocity = sSpriteAffineScaleTable[timer][0];
    if (yVelocity == S16_MAX) {
        timer = 0;
        yVelocity = sSpriteAffineScaleTable[0][0];
    }
    xVelocity = sSpriteAffineScaleTable[timer][1];
    gCurrentSprite.work3 = timer + 1;
    gCurrentSprite.yPosition += yVelocity;
    gCurrentSprite.xPosition += xVelocity;
}

void func_802655C(void)
{
    s16 xVelocity;
    s16 yVelocity;
    u32 timer;

    timer = gCurrentSprite.work3;
    yVelocity = sSpriteAffineScaleTable[timer][0];
    if (yVelocity == S16_MAX) {
        timer = 0;
        yVelocity = sSpriteAffineScaleTable[0][0];
    }
    xVelocity = sSpriteAffineScaleTable[timer][1];

    gCurrentSprite.work3 = timer + 1;
    gCurrentSprite.yPosition += yVelocity;
    gCurrentSprite.xPosition += xVelocity;

    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON) {
        gWarioData.yPosition += yVelocity;
        gWarioData.xPosition += xVelocity;
    }
}

void func_80265C8(s32 slot)
{
    s32 i;

    u16 yPosition;
    u16 xPosition;
    u16 thisTop;
    u16 thisBottom;
    u16 thisLeft;
    u16 thisRight;

    u16 otherYPosition;
    u16 otherXPosition;
    u16 otherTop;
    u16 otherBottom;
    u16 otherLeft;
    u16 otherRight;

    u16 statusCheck;

    yPosition = gSpriteData[slot].yPosition;
    xPosition = gSpriteData[slot].xPosition;
    thisTop = yPosition - gSpriteData[slot].hitboxExtentUp;
    thisBottom = yPosition + gSpriteData[slot].hitboxExtentDown;
    thisLeft = xPosition - gSpriteData[slot].hitboxExtentLeft;
    thisRight = xPosition + gSpriteData[slot].hitboxExtentRight;

    statusCheck = SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_MAYBE_DEAD | SPRITE_STATUS_HIDDEN |
                  SPRITE_STATUS_ONSCREEN | SPRITE_STATUS_EXISTS;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (i == slot) {
            continue;
        }

        if ((gSpriteData[i].status & statusCheck) != (SPRITE_STATUS_ONSCREEN | SPRITE_STATUS_EXISTS)) {
            continue;
        }

        otherYPosition = gSpriteData[i].yPosition;
        otherXPosition = gSpriteData[i].xPosition;
        otherTop = otherYPosition - gSpriteData[i].hitboxExtentUp;
        otherBottom = otherYPosition + gSpriteData[i].hitboxExtentDown;
        otherLeft = otherXPosition - gSpriteData[i].hitboxExtentLeft;
        otherRight = otherXPosition + gSpriteData[i].hitboxExtentRight;

        if (!SpriteCollisionCheckObjectsTouching(
                thisTop, thisBottom, thisLeft, thisRight, otherTop, otherBottom, otherLeft, otherRight
            )) {
            continue;
        }

        if ((gSpriteData[slot].pose == SPOSE_CARRIED_LEFT) || (gSpriteData[slot].pose == SPOSE_CARRIED_RIGHT)) {
            func_801E4B0();
            gCurrentCarriedSprite.state = 0;
            if (xPosition > otherXPosition) {
                gSpriteData[slot].pose = SPOSE_33;
                gSpriteData[i].pose = SPOSE_35;
            } else {
                gSpriteData[slot].pose = SPOSE_35;
                gSpriteData[i].pose = SPOSE_33;
            }
        } else {
            if (gSpriteData[i].pose >= SPOSE_BEING_LIFTED_RIGHT_INIT && gSpriteData[i].pose <= SPOSE_CARRIED_RIGHT) {
                func_801E4B0();
                gCurrentCarriedSprite.state = 0;
            }
            if (gSpriteData[slot].status & SPRITE_STATUS_HEAVY) {
                if (gSpriteData[i].status & SPRITE_STATUS_HEAVY) {
                    if (xPosition > otherXPosition) {
                        gSpriteData[slot].pose = SPOSE_6A;
                        gSpriteData[i].pose = SPOSE_69;
                    } else {
                        gSpriteData[slot].pose = SPOSE_69;
                        gSpriteData[i].pose = SPOSE_6A;
                    }
                } else {
                    if (xPosition > otherXPosition) {
                        gSpriteData[slot].pose = SPOSE_33;
                        gSpriteData[i].pose = SPOSE_69;
                    } else {
                        gSpriteData[slot].pose = SPOSE_35;
                        gSpriteData[i].pose = SPOSE_6A;
                    }
                }
            } else {
                if (gSpriteData[i].status & SPRITE_STATUS_HEAVY) {
                    if (xPosition > otherXPosition) {
                        gSpriteData[slot].pose = SPOSE_6A;
                        gSpriteData[i].pose = SPOSE_35;
                    } else {
                        gSpriteData[slot].pose = SPOSE_69;
                        gSpriteData[i].pose = SPOSE_33;
                    }
                } else {
                    if (xPosition > otherXPosition) {
                        gSpriteData[slot].pose = SPOSE_6A;
                        gSpriteData[i].pose = SPOSE_69;
                    } else {
                        gSpriteData[slot].pose = SPOSE_69;
                        gSpriteData[i].pose = SPOSE_6A;
                    }
                }
            }
        }

        gSpriteData[i].status |= SPRITE_STATUS_MAYBE_DEAD;
        gSpriteData[slot].status |= SPRITE_STATUS_MAYBE_DEAD;
        gSpriteData[i].status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
        gSpriteData[slot].status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
        gSpriteData[i].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
        gSpriteData[slot].disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
        m4aSongNumStart(SOUND_3B);
        return;
    }
}

void func_80267C0(void)
{
    s32 i;
    s32 j;
    u32 found;
    u16 statusCheck;

    found = FALSE;
    statusCheck = SPRITE_STATUS_CAN_HIT_OTHER_SPRITES | SPRITE_STATUS_EXISTS;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if ((gSpriteData[i].status & statusCheck) == statusCheck) {
            found++;
            break;
        }
    }
    if (!found) {
        return;
    }
    func_80265C8(i);

    found = FALSE;
    for (j = 0; j < MAX_SPRITE_COUNT; j++) {
        if ((gSpriteData[j].status & statusCheck) == statusCheck && j != i) {
            found++;
            break;
        }
    }
    if (!found) {
        return;
    }
    func_80265C8(j);
}

void func_8026838(void)
{
    if (!(gCurrentSprite.status & SPRITE_STATUS_EXISTS)) {
        return;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON) {
        if (gWarioData.unk_1A != 1) {
            gWarioData.unk_1A = 1;
            WarioRequestPose(-3);
        }
    } else {
        if (gCurrentSprite.work1 != 1) {
            if (gCurrentSprite.work1 == 2) {
                gCurrentSprite.work1 = 1;
                if (gPreviousYPosition <= gWarioData.yPosition) {
                    gWarioData.yPosition = (gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp) + 1;
                }
            }
        } else {
            gCurrentSprite.work1 = 0;
            if (gWarioData.unk_1A == 1) {
                gWarioData.unk_1A = 0;
            }
        }
    }
    gCurrentSprite.status &= ~SPRITE_STATUS_WARIO_STANDING_ON;
}

u32 SpriteUtilDespawnChildWithParent(u8 parentId)
{
    s32 i;
    u8 roomSlot;

    roomSlot = gCurrentSprite.roomSlot;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].roomSlot == roomSlot && gSpriteData[i].globalID == parentId &&
            (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            return FALSE;
        }
    }
    gCurrentSprite.status = SPRITE_STATUS_NONE;
    return TRUE;
}

u32 SpriteUtilFindParentSlotOrU8MaxUnused(u8 parentId)
{
    s32 i;
    u8 roomSlot;

    roomSlot = gCurrentSprite.roomSlot;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].roomSlot == roomSlot && gSpriteData[i].globalID == parentId &&
            (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            return i;
        }
    }
    return U8_MAX;
}

u32 SpriteUtilDespawnIfSpriteExists(u8 id)
{
    s32 i;
    u8 roomSlot;

    roomSlot = gCurrentSprite.roomSlot;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == id && gSpriteData[i].roomSlot != roomSlot &&
            (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            return TRUE;
        }
    }
    return FALSE;
}

u32 SpriteUtilCountSpriteType(u8 id)
{
    s32 i;
    u32 count;

    count = 0;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == id && (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            count++;
        }
    }
    return count;
}

void func_80269EC(void)
{
    u16 xPosition;
    u16 yPosition;

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    gSpriteTileInteractionMode = 3;
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
    gSpriteTileInteractionMode = 3;
    GetSpriteBlockCollisionAtPosition(yPosition - PIXEL_SIZE, xPosition);
    gSpriteTileInteractionMode = 3;
    GetSpriteBlockCollisionAtPosition(yPosition - PIXEL_SIZE, xPosition - PIXEL_SIZE);
    gSpriteTileInteractionMode = 3;
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition - PIXEL_SIZE);
}

void func_8026A54(void)
{
    u16 xPosition;
    u16 yPosition;

    yPosition = gCurrentSprite.yPosition;
    xPosition = gCurrentSprite.xPosition;
    gSpriteTileInteractionMode = 2;
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition);
    gSpriteTileInteractionMode = 2;
    GetSpriteBlockCollisionAtPosition(yPosition - PIXEL_SIZE, xPosition);
    gSpriteTileInteractionMode = 2;
    GetSpriteBlockCollisionAtPosition(yPosition - PIXEL_SIZE, xPosition - PIXEL_SIZE);
    gSpriteTileInteractionMode = 2;
    GetSpriteBlockCollisionAtPosition(yPosition, xPosition - PIXEL_SIZE);
}

void SpriteUtilFindSpriteSlotWork3(u8 id)
{
    s32 i;

    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == id && (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            break;
        }
    }
    gCurrentSprite.work3 = i;
}

u32 SpriteUtilFindSpriteSlot(u8 id)
{
    s32 i;

    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == id && (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            break;
        }
    }
    return i;
}

u32 SpriteUtilFindSpriteSlotOrU8Max(u8 id)
{
    u8 i;
    u8 found;

    found = 0;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == id && (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            found++;
            break;
        }
    }
    if (!found) {
        i = U8_MAX;
    }
    return i;
}

void SpriteUtilFindOwnSlot(u8 roomSlot)
{
    s32 i;

    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == gCurrentSprite.globalID && (gSpriteData[i].status & SPRITE_STATUS_EXISTS) &&
            gSpriteData[i].roomSlot == roomSlot) {
            break;
        }
    }
    gCurrentSprite.work3 = i;
}

void SpriteUtilFindSpriteWork3(u8 id, u8 roomSlot)
{
    s32 i;

    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == id && (gSpriteData[i].status & SPRITE_STATUS_EXISTS) &&
            gSpriteData[i].roomSlot == roomSlot) {
            break;
        }
    }
    gCurrentSprite.work3 = i;
}

s32 SpriteUtilFindSprite(u8 id, u8 roomSlot)
{
    s32 i;

    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == id && (gSpriteData[i].status & SPRITE_STATUS_EXISTS) &&
            gSpriteData[i].roomSlot == roomSlot) {
            break;
        }
    }
    return i;
}

void SpriteUtilFindParentSlotWork3(u8 parentId)
{
    s32 i;
    u8 roomSlot;

    roomSlot = gCurrentSprite.roomSlot;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == parentId && gSpriteData[i].roomSlot == roomSlot &&
            (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            break;
        }
    }
    gCurrentSprite.work3 = i;
}

s32 SpriteUtilFindParentSlotOrU8Max(u8 parentId)
{
    s32 i;
    u8 roomSlot;
    u8 found;

    roomSlot = gCurrentSprite.roomSlot;
    found = 0;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == parentId && gSpriteData[i].roomSlot == roomSlot &&
            (gSpriteData[i].status & SPRITE_STATUS_EXISTS)) {
            return i;
        }
    }
    return U8_MAX;
}

s32 SpriteUtilFindBossTreasureChest(u8 roomSlot)
{
    s32 i;
    u32 slot;

    slot = U8_MAX;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].globalID == PSPRITE_BOSS_TREASURE_CHEST && (gSpriteData[i].status & SPRITE_STATUS_EXISTS) &&
            gSpriteData[i].roomSlot == roomSlot) {
            slot = i;
            break;
        }
    }
    return slot;
}

void SpriteUtilClearAllSpritesBackground(void)
{
    s32 i;

    gCurrentSprite.status &= ~SPRITE_STATUS_BACKGROUND;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].status & SPRITE_STATUS_EXISTS) {
            gSpriteData[i].status &= ~SPRITE_STATUS_BACKGROUND;
        }
    }
}

void SpriteUtilSetAllSpritesHighPriority(void)
{
    s32 i;

    gCurrentSprite.drawPriority |= 0x80;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].status & SPRITE_STATUS_EXISTS) {
            gSpriteData[i].drawPriority |= 0x80;
        }
    }
}

void SpriteUtilUnsetAllSpritesHighPriority(void)
{
    s32 i;

    gCurrentSprite.drawPriority &= 0x7F;
    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if (gSpriteData[i].status & SPRITE_STATUS_EXISTS) {
            gSpriteData[i].drawPriority &= 0x7F;
        }
    }
}

void SpriteUtilStartBossTimer(void)
{
    u32 yPosition;
    u32 xPosition;
    u32 xOffset;

    WarioRequestPose(WPOSE_NORMAL_STANDING);
    gWarioPauseTimer = CONVERT_SECONDS(3 + 1 / 3.0);
    gTimerState = 1;
    gStageFrameCounter = 0;
    gStageTimerDigitsState = 3;
    gStageTimerColonState = 2;
    SpawnSecondarySprite(3 * BLOCK_SIZE + EIGHTH_BLOCK_SIZE, 6 * BLOCK_SIZE, SSPRITE_4E);
    yPosition = gWarioData.yPosition - gBg1YPosition;
    xPosition = gWarioData.xPosition;
    xOffset = (gBg1XPosition + BLOCK_SIZE);
    xPosition -= xOffset;
    SpawnSecondarySprite(yPosition, xPosition, SSPRITE_4F);
    m4aSongNumStart(SOUND_1BC);
    if (gCurrentPassage == PASSAGE_GOLDEN) {
        m4aSongNumStart(BGM_GOLDEN_DIVA);
    } else {
        m4aSongNumStart(BGM_BOSS);
    }
}

void func_8026E78(void)
{
    switch (gCurrentPassage) {
        case PASSAGE_ENTRY:
            SpriteUtilFindSpriteSlotWork3(PSPRITE_SPOILED_ROTTEN);
            gSpriteData[gCurrentSprite.work3].pose = SPOSE_74;
            break;

        case PASSAGE_EMERALD:
            SpriteUtilFindSpriteSlotWork3(PSPRITE_CRACTUS_BOSS);
            gSpriteData[gCurrentSprite.work3].pose = SPOSE_7A;
            break;

        case PASSAGE_RUBY:
            SpriteUtilFindSpriteSlotWork3(PSPRITE_CUCKOO_CONDOR);
            gSpriteData[gCurrentSprite.work3].pose = SPOSE_7A;
            break;

        case PASSAGE_TOPAZ:
            SpriteUtilFindSpriteSlotWork3(PSPRITE_AERODENT_COMPANION);
            gSpriteData[gCurrentSprite.work3].pose = SPOSE_7A;
            break;

        case PASSAGE_SAPPHIRE:
            SpriteUtilFindSpriteSlotWork3(PSPRITE_CATBAT);
            gSpriteData[gCurrentSprite.work3].pose = SPOSE_7A;
            break;

        case PASSAGE_GOLDEN:
            SpriteUtilFindSpriteSlotWork3(PSPRITE_EC);
            gSpriteData[gCurrentSprite.work3].pose = SPOSE_74;
            break;
    }
}

void SpriteUtilFadeBackgroundToBlack(void)
{
    StartGoldenDivaPaletteFade(8, 0, 16);
}

void SpriteUtilSetWarioBossVictoryPose(void)
{
    if ((gWarioData.reaction == REACTION_NORMAL) && (gWarioData.unk_1A != 3)) {
        if (gWarioData.unk_1A == 0) {
            WarioRequestPose(WPOSE_NORMAL_BOSS_DEFEATED);
        } else {
            WarioRequestPose(WPOSE_NORMAL_BOSS_DEFEATED_MIDAIR);
        }
    }
}


#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

#ifndef PORTABLE
const u16 sKeyzerOamFrame_83B4554[] = {
    2,
    OAM_ENTRY(-16, -2, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x16, 0, 288, 7, 0),
};
const u16 sKeyzerOamFrame_83B4562[] = {
    2,
    OAM_ENTRY(-16, -2, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x16, 0, 290, 7, 0),
};
const u16 sKeyzerOamFrame_83B4570[] = {
    2,
    OAM_ENTRY(-16, -3, SPRITE_SIZE_16x8, 0, 326, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x16, 0, 370, 7, 0),
};
const u16 sKeyzerOamFrame_83B457E[] = {
    2,
    OAM_ENTRY(-16, -3, SPRITE_SIZE_16x8, 0, 326, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x16, 0, 372, 7, 0),
};
const u16 sKeyzerIdleOam_Frame1[] = {
    4,
    OAM_ENTRY(-13, -6, SPRITE_SIZE_8x8, 0, 507, 5, 0),
    OAM_ENTRY(-16, -2, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 320, 7, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 292, 7, 0),
};
const u16 sKeyzerIdleOam_Frame2[] = {
    4,
    OAM_ENTRY(-13, -6, SPRITE_SIZE_8x8, 0, 508, 5, 0),
    OAM_ENTRY(-16, -2, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 292, 7, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 322, 7, 0),
};
const u16 sKeyzerIdleOam_Frame3[] = {
    4,
    OAM_ENTRY(-13, -7, SPRITE_SIZE_8x8, 0, 508, 5, 0),
    OAM_ENTRY(-16, -3, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 402, 7, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 324, 7, 0),
};
const u16 sKeyzerIdleOam_Frame4[] = {
    4,
    OAM_ENTRY(-13, -6, SPRITE_SIZE_8x8, 0, 507, 5, 0),
    OAM_ENTRY(-16, -3, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 404, 7, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 324, 7, 0),
};
const u16 sKeyzerOamFrame_83B45F4[] = {
    1,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 364, 7, 0),
};
const u16 sKeyzerOamFrame_83B45FC[] = {
    1,
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x16, 0, 366, 7, 0),
};
const u16 sKeyzerOamFrame_83B4604[] = {
    1,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 364, 7, 0),
};
const u16 sKeyzerOamFrame_83B460C[] = {
    1,
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 366, 7, 0),
};
const u16 sKeyzerOamFrame_83B4614[] = {
    2,
    OAM_ENTRY(-19, -2, SPRITE_SIZE_16x8, 0, 328, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x16, 0, 368, 7, 0),
};
const u16 sKeyzerOamFrame_83B4622[] = {
    2,
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x16, 0, 368, 7, 0),
    OAM_ENTRY(-18, -2, SPRITE_SIZE_16x8, 0, 328, 6, 0),
};
const u16 sKeyzerOamFrame_83B4630[] = {
    2,
    OAM_ENTRY(-4, -3, SPRITE_SIZE_8x8, 0, 283, 6, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 374, 7, 0),
};
const u16 sKeyzerOamFrame_83B463E[] = {
    2,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 364, 7, 0),
    OAM_ENTRY(-11, -2, SPRITE_SIZE_8x8, 0, 328, 6, 0),
};
const u16 sKeyzerOamFrame_83B464C[] = {
    2,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 364, 7, 0),
    OAM_ENTRY(3, -2, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 328, 6, 0),
};
const u16 sKeyzerIdleOam_Frame5[] = {
    3,
    OAM_ENTRY(-16, -2, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 320, 7, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 292, 7, 0),
};
const u16 sKeyzerIdleOam_Frame6[] = {
    3,
    OAM_ENTRY(-16, -2, SPRITE_SIZE_16x8, 0, 294, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 292, 7, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 322, 7, 0),
};
const u16 sKeyzerIdleOam_Frame7[] = {
    3,
    OAM_ENTRY(-16, -3, SPRITE_SIZE_16x8, 0, 326, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 324, 7, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 402, 7, 0),
};
const u16 sKeyzerIdleOam_Frame8[] = {
    3,
    OAM_ENTRY(-16, -3, SPRITE_SIZE_16x8, 0, 326, 6, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 324, 7, 0),
    OAM_ENTRY(-7, 0, SPRITE_SIZE_16x8, 0, 404, 7, 0),
};
const u16 sKeyzerOamFrame_83B46AA[] = {
    2,
    OAM_ENTRY(0, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 288, 7, 0),
};
const u16 sKeyzerOamFrame_83B46B8[] = {
    2,
    OAM_ENTRY(0, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 290, 7, 0),
};
const u16 sKeyzerOamFrame_83B46C6[] = {
    2,
    OAM_ENTRY(0, -3, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 326, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 370, 7, 0),
};
const u16 sKeyzerOamFrame_83B46D4[] = {
    2,
    OAM_ENTRY(0, -3, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 326, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 372, 7, 0),
};
const u16 sKeyzerOamFrame_83B46E2[] = {
    4,
    OAM_ENTRY(5, -6, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 507, 5, 0),
    OAM_ENTRY(0, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 320, 7, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 292, 7, 0),
};
const u16 sKeyzerOamFrame_83B46FC[] = {
    4,
    OAM_ENTRY(5, -6, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 508, 5, 0),
    OAM_ENTRY(0, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 292, 7, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 322, 7, 0),
};
const u16 sKeyzerOamFrame_83B4716[] = {
    4,
    OAM_ENTRY(5, -7, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 508, 5, 0),
    OAM_ENTRY(0, -3, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 324, 7, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 402, 7, 0),
};
const u16 sKeyzerOamFrame_83B4730[] = {
    4,
    OAM_ENTRY(5, -7, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 507, 5, 0),
    OAM_ENTRY(0, -3, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 324, 7, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 404, 7, 0),
};
const u16 sKeyzerOamFrame_83B474A[] = {
    2,
    OAM_ENTRY(3, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 328, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 368, 7, 0),
};
const u16 sKeyzerOamFrame_83B4758[] = {
    2,
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 368, 7, 0),
    OAM_ENTRY(2, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 328, 6, 0),
};
const u16 sKeyzerOamFrame_83B4766[] = {
    2,
    OAM_ENTRY(-4, -3, SPRITE_SIZE_8x8, 0, 283, 6, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 374, 7, 0),
};
const u16 sKeyzerOamFrame_83B4774[] = {
    3,
    OAM_ENTRY(0, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 320, 7, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 292, 7, 0),
};
const u16 sKeyzerOamFrame_83B4788[] = {
    3,
    OAM_ENTRY(0, -2, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 292, 7, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 322, 7, 0),
};
const u16 sKeyzerOamFrame_83B479C[] = {
    3,
    OAM_ENTRY(0, -3, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 402, 7, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 324, 7, 0),
};
const u16 sKeyzerOamFrame_83B47B0[] = {
    3,
    OAM_ENTRY(0, -3, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 294, 6, 0),
    OAM_ENTRY(-9, 0, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 404, 7, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 324, 7, 0),
};

const struct AnimationFrame sKeyzerOam_83B47C4[] = {
    {sKeyzerOamFrame_83B4554, 10},
    {sKeyzerOamFrame_83B4562, 10},
    {sKeyzerOamFrame_83B4570, 10},
    {sKeyzerOamFrame_83B457E, 10},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B47EC[] = {
    {sKeyzerOamFrame_83B46AA, 10},
    {sKeyzerOamFrame_83B46B8, 10},
    {sKeyzerOamFrame_83B46C6, 10},
    {sKeyzerOamFrame_83B46D4, 10},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B4814[] = {
    {sKeyzerOamFrame_83B4554, 1},
    {sKeyzerOamFrame_83B4614, 6},
    {sKeyzerOamFrame_83B4622, 1},
    {sKeyzerOamFrame_83B463E, 2},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B483C[] = {
    {sKeyzerOamFrame_83B46AA, 1},
    {sKeyzerOamFrame_83B474A, 6},
    {sKeyzerOamFrame_83B4758, 1},
    {sKeyzerOamFrame_83B464C, 2},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B4864[] = {
    {sKeyzerOamFrame_83B463E, 10},
    {sKeyzerOamFrame_83B45FC, 10},
    {sKeyzerOamFrame_83B464C, 10},
    {sKeyzerOamFrame_83B460C, 10},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B488C[] = {
    {sKeyzerOamFrame_83B463E, 3},
    {sKeyzerOamFrame_83B4622, 1},
    {sKeyzerOamFrame_83B4614, 6},
    {sKeyzerOamFrame_83B457E, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B48B4[] = {
    {sKeyzerOamFrame_83B464C, 3},
    {sKeyzerOamFrame_83B4758, 1},
    {sKeyzerOamFrame_83B474A, 6},
    {sKeyzerOamFrame_83B46D4, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B48DC[] = {
    {sKeyzerOamFrame_83B4554, 1},
    {sKeyzerIdleOam_Frame6, 10},
    {sKeyzerOamFrame_83B4630, 6},
    {sKeyzerOamFrame_83B46D4, 8},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B4904[] = {
    {sKeyzerOamFrame_83B46AA, 1},
    {sKeyzerOamFrame_83B4788, 10},
    {sKeyzerOamFrame_83B4766, 6},
    {sKeyzerOamFrame_83B457E, 8},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerIdleOam[] = {
    {sKeyzerIdleOam_Frame1, 13},
    {sKeyzerIdleOam_Frame2, 13},
    {sKeyzerIdleOam_Frame3, 13},
    {sKeyzerIdleOam_Frame4, 13},
    {sKeyzerIdleOam_Frame5, 13},
    {sKeyzerIdleOam_Frame6, 13},
    {sKeyzerIdleOam_Frame7, 13},
    {sKeyzerIdleOam_Frame8, 13},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B4974[] = {
    {sKeyzerOamFrame_83B46E2, 13},
    {sKeyzerOamFrame_83B46FC, 13},
    {sKeyzerOamFrame_83B4716, 13},
    {sKeyzerOamFrame_83B4730, 13},
    {sKeyzerOamFrame_83B4774, 13},
    {sKeyzerOamFrame_83B4788, 13},
    {sKeyzerOamFrame_83B479C, 13},
    {sKeyzerOamFrame_83B47B0, 13},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B49BC[] = {
    {sKeyzerIdleOam_Frame5, 6},
    {sKeyzerOamFrame_83B4562, 6},
    {sKeyzerIdleOam_Frame7, 6},
    {sKeyzerOamFrame_83B457E, 6},
    {sKeyzerIdleOam_Frame6, 6},
    {sKeyzerOamFrame_83B4554, 6},
    {sKeyzerIdleOam_Frame8, 6},
    {sKeyzerOamFrame_83B4570, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sKeyzerOam_83B4A04[] = {
    {sKeyzerOamFrame_83B4774, 6},
    {sKeyzerOamFrame_83B46B8, 6},
    {sKeyzerOamFrame_83B479C, 6},
    {sKeyzerOamFrame_83B46D4, 6},
    {sKeyzerOamFrame_83B4788, 6},
    {sKeyzerOamFrame_83B46AA, 6},
    {sKeyzerOamFrame_83B47B0, 6},
    {sKeyzerOamFrame_83B46C6, 6},
    ANIMATION_TERMINATOR
};

const s16 sKeyzerFloatYVelocity[] = {0, 0, 0, 0, 0, 0, -1, -1, -1, -2, -2, -3, -2, -2, -1, -1, -1, 0, 0, 0,      0,
                                     0, 0, 0, 0, 0, 1, 1,  1,  2,  2,  3,  2,  2,  1,  1,  1,  0,  0, 0, S16_MAX};
#else
extern const u16 sKeyzerOamFrame_83B4554[];
extern const u16 sKeyzerOamFrame_83B4562[];
extern const u16 sKeyzerOamFrame_83B4570[];
extern const u16 sKeyzerOamFrame_83B457E[];
extern const u16 sKeyzerIdleOam_Frame1[];
extern const u16 sKeyzerIdleOam_Frame2[];
extern const u16 sKeyzerIdleOam_Frame3[];
extern const u16 sKeyzerIdleOam_Frame4[];
extern const u16 sKeyzerOamFrame_83B45F4[];
extern const u16 sKeyzerOamFrame_83B45FC[];
extern const u16 sKeyzerOamFrame_83B4604[];
extern const u16 sKeyzerOamFrame_83B460C[];
extern const u16 sKeyzerOamFrame_83B4614[];
extern const u16 sKeyzerOamFrame_83B4622[];
extern const u16 sKeyzerOamFrame_83B4630[];
extern const u16 sKeyzerOamFrame_83B463E[];
extern const u16 sKeyzerOamFrame_83B464C[];
extern const u16 sKeyzerIdleOam_Frame5[];
extern const u16 sKeyzerIdleOam_Frame6[];
extern const u16 sKeyzerIdleOam_Frame7[];
extern const u16 sKeyzerIdleOam_Frame8[];
extern const u16 sKeyzerOamFrame_83B46AA[];
extern const u16 sKeyzerOamFrame_83B46B8[];
extern const u16 sKeyzerOamFrame_83B46C6[];
extern const u16 sKeyzerOamFrame_83B46D4[];
extern const u16 sKeyzerOamFrame_83B46E2[];
extern const u16 sKeyzerOamFrame_83B46FC[];
extern const u16 sKeyzerOamFrame_83B4716[];
extern const u16 sKeyzerOamFrame_83B4730[];
extern const u16 sKeyzerOamFrame_83B474A[];
extern const u16 sKeyzerOamFrame_83B4758[];
extern const u16 sKeyzerOamFrame_83B4766[];
extern const u16 sKeyzerOamFrame_83B4774[];
extern const u16 sKeyzerOamFrame_83B4788[];
extern const u16 sKeyzerOamFrame_83B479C[];
extern const u16 sKeyzerOamFrame_83B47B0[];
extern const struct AnimationFrame sKeyzerOam_83B47C4[];
extern const struct AnimationFrame sKeyzerOam_83B47EC[];
extern const struct AnimationFrame sKeyzerOam_83B4814[];
extern const struct AnimationFrame sKeyzerOam_83B483C[];
extern const struct AnimationFrame sKeyzerOam_83B4864[];
extern const struct AnimationFrame sKeyzerOam_83B488C[];
extern const struct AnimationFrame sKeyzerOam_83B48B4[];
extern const struct AnimationFrame sKeyzerOam_83B48DC[];
extern const struct AnimationFrame sKeyzerOam_83B4904[];
extern const struct AnimationFrame sKeyzerIdleOam[];
extern const struct AnimationFrame sKeyzerOam_83B4974[];
extern const struct AnimationFrame sKeyzerOam_83B49BC[];
extern const struct AnimationFrame sKeyzerOam_83B4A04[];
extern const s16 sKeyzerFloatYVelocity[];
#endif

void KeyzerInit()
{
    if (gCollectedKeyzer) {
        gCurrentSprite.status = SPRITE_STATUS_NONE;
        gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
    } else {
        gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND;
        gCurrentSprite.warioCollision = 6;
        gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
        gCurrentSprite.drawDistanceUp = BLOCK_SIZE_PIXELS;
        gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS * 3 / 2;
        gCurrentSprite.hitboxExtentUp = HALF_BLOCK_SIZE;
        gCurrentSprite.hitboxExtentDown = QUARTER_BLOCK_SIZE + PIXEL_SIZE;
        gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE;
        gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE - PIXEL_SIZE;
        GP_SET(gCurrentSprite.pOamData, sKeyzerIdleOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = SPOSE_IDLE;
        gCurrentSprite.work3 = 0;
        gCurrentSprite.yPosition -= BLOCK_SIZE;
        gCurrentSprite.xPosition -= HALF_BLOCK_SIZE;
    }
}

void KeyzerCollect()
{
    gCurrentSprite.status = SPRITE_STATUS_NONE;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
    gCollectedKeyzer = 1;
    if (gWarioData.reaction == REACTION_WATER) {
        m4aSongNumStart(SE_GET_KEYZER_UNDERWATER);
    } else {
        m4aSongNumStart(SE_GET_KEYZER);
    }
    SpawnSecondarySprite(gCurrentSprite.yPosition, gCurrentSprite.xPosition, SSPRITE_KEYZER);
    VoiceSetPlay(VS_WARIO_KEYZER_GET);
}

void SpriteKeyzer(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            KeyzerInit();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            KeyzerCollect();
        default:
            SpriteUtilLookupFloatingAnimation(sKeyzerFloatYVelocity);
            break;
    }

    if (gCurrentSprite.status == SPRITE_STATUS_NONE) {
        return;
    }

    func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gSpriteCollisionTileType & 15) {
        gCurrentSprite.status |= SPRITE_STATUS_HIDDEN;
    } else {
        gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    }
}


#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

#ifndef PORTABLE
const u16 sCoin500PointsOam_Frame1[] = {
    1,
    OAM_ENTRY(-8, -15, SPRITE_SIZE_16x16, 0, 430, 6, 0),
};
const u16 sCoin500PointsOam_Frame2[] = {
    1,
    OAM_ENTRY(-8, -15, SPRITE_SIZE_16x16, 0, 434, 6, 0),
};
const u16 sCoin500PointsOam_Frame3[] = {
    1,
    OAM_ENTRY(-4, -15, SPRITE_SIZE_8x16, 0, 436, 6, 0),
};
const u16 sCoin500PointsOam_Frame4[] = {
    1,
    OAM_ENTRY(-4, -15, SPRITE_SIZE_8x16, 0, 437, 6, 0),
};
const u16 sCoin500PointsOam_Frame5[] = {
    1,
    OAM_ENTRY(-4, -15, SPRITE_SIZE_8x16, ST_OAM_HFLIP, 436, 6, 0),
};
const u16 sCoin500PointsOam_Frame6[] = {
    1,
    OAM_ENTRY(-8, -15, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 434, 6, 0),
};
const u16 sCoin500PointsOam_Frame7[] = {
    1,
    OAM_ENTRY(-8, -15, SPRITE_SIZE_16x16, 0, 432, 6, 0),
};
const u16 sCoin100PointsOam_Frame1[] = {
    1,
    OAM_ENTRY(-8, -14, SPRITE_SIZE_16x16, 0, 423, 4, 0),
};
const u16 sCoin100PointsOam_Frame2[] = {
    1,
    OAM_ENTRY(-8, -14, SPRITE_SIZE_16x16, 0, 427, 4, 0),
};
const u16 sCoin100PointsOam_Frame3[] = {
    1,
    OAM_ENTRY(-4, -14, SPRITE_SIZE_8x16, 0, 429, 4, 0),
};
const u16 sCoin100PointsOam_Frame4[] = {
    2,
    OAM_ENTRY(-4, -13, SPRITE_SIZE_8x8, 0, 437, 4, 0),
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 437, 4, 0),
};
const u16 sCoin100PointsOam_Frame5[] = {
    1,
    OAM_ENTRY(-4, -14, SPRITE_SIZE_8x16, ST_OAM_HFLIP, 429, 4, 0),
};
const u16 sCoin100PointsOam_Frame6[] = {
    1,
    OAM_ENTRY(-8, -14, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 427, 4, 0),
};
const u16 sCoin100PointsOam_Frame7[] = {
    1,
    OAM_ENTRY(-8, -14, SPRITE_SIZE_16x16, 0, 425, 4, 0),
};
const u16 sCoin50PointsOam_Frame1[] = {
    1,
    OAM_ENTRY(-8, -13, SPRITE_SIZE_16x16, 0, 416, 7, 0),
};
const u16 sCoin50PointsOam_Frame2[] = {
    1,
    OAM_ENTRY(-8, -13, SPRITE_SIZE_16x16, 0, 420, 7, 0),
};
const u16 sCoin50PointsOam_Frame3[] = {
    1,
    OAM_ENTRY(-4, -13, SPRITE_SIZE_8x16, 0, 422, 7, 0),
};
const u16 sCoin50PointsOam_Frame4[] = {
    2,
    OAM_ENTRY(-4, -11, SPRITE_SIZE_8x8, 0, 437, 7, 0),
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 437, 7, 0),
};
const u16 sCoin50PointsOam_Frame5[] = {
    1,
    OAM_ENTRY(-4, -13, SPRITE_SIZE_8x16, ST_OAM_HFLIP, 422, 7, 0),
};
const u16 sCoin50PointsOam_Frame6[] = {
    1,
    OAM_ENTRY(-8, -13, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 420, 7, 0),
};
const u16 sCoin50PointsOam_Frame7[] = {
    1,
    OAM_ENTRY(-8, -13, SPRITE_SIZE_16x16, 0, 418, 7, 0),
};
const u16 sCoin10PointsOam_Frame1[] = {
    1,
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, 0, 499, 5, 0),
};
const u16 sCoin10PointsOam_Frame2[] = {
    1,
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, 0, 501, 5, 0),
};
const u16 sCoin10PointsOam_Frame3[] = {
    1,
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, 0, 502, 5, 0),
};
const u16 sCoin10PointsOam_Frame4[] = {
    1,
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 501, 5, 0),
};
const u16 sCoin10PointsOam_Frame5[] = {
    1,
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, 0, 500, 5, 0),
};

const u16 sCoinsUnusedOamFrame[] = {
    1,
    OAM_ENTRY(-4, -7, SPRITE_SIZE_8x8, 0, 203, 6, 0),
};

const struct AnimationFrame sCoin500PointsOamData[] = {
    {sCoin500PointsOam_Frame1, 3},
    {sCoin500PointsOam_Frame2, 3},
    {sCoin500PointsOam_Frame3, 3},
    {sCoin500PointsOam_Frame4, 3},
    {sCoin500PointsOam_Frame5, 3},
    {sCoin500PointsOam_Frame6, 3},
    {sCoin500PointsOam_Frame7, 3},
    {sCoin500PointsOam_Frame2, 3},
    {sCoin500PointsOam_Frame3, 3},
    {sCoin500PointsOam_Frame4, 3},
    {sCoin500PointsOam_Frame5, 3},
    {sCoin500PointsOam_Frame6, 3},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sCoin100PointsOamData[] = {
    {sCoin100PointsOam_Frame1, 3},
    {sCoin100PointsOam_Frame2, 3},
    {sCoin100PointsOam_Frame3, 3},
    {sCoin100PointsOam_Frame4, 3},
    {sCoin100PointsOam_Frame5, 3},
    {sCoin100PointsOam_Frame6, 3},
    {sCoin100PointsOam_Frame7, 3},
    {sCoin100PointsOam_Frame2, 3},
    {sCoin100PointsOam_Frame3, 3},
    {sCoin100PointsOam_Frame4, 3},
    {sCoin100PointsOam_Frame5, 3},
    {sCoin100PointsOam_Frame6, 3},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sCoin50PointsOamData[] = {
    {sCoin50PointsOam_Frame1, 3},
    {sCoin50PointsOam_Frame2, 3},
    {sCoin50PointsOam_Frame3, 3},
    {sCoin50PointsOam_Frame4, 3},
    {sCoin50PointsOam_Frame5, 3},
    {sCoin50PointsOam_Frame6, 3},
    {sCoin50PointsOam_Frame7, 3},
    {sCoin50PointsOam_Frame2, 3},
    {sCoin50PointsOam_Frame3, 3},
    {sCoin50PointsOam_Frame4, 3},
    {sCoin50PointsOam_Frame5, 3},
    {sCoin50PointsOam_Frame6, 3},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sCoin10PointsOamData[] = {
    {sCoin10PointsOam_Frame1, 3},
    {sCoin10PointsOam_Frame2, 3},
    {sCoin10PointsOam_Frame3, 3},
    {sCoin10PointsOam_Frame4, 3},
    {sCoin10PointsOam_Frame5, 3},
    {sCoin10PointsOam_Frame2, 3},
    {sCoin10PointsOam_Frame3, 3},
    {sCoin10PointsOam_Frame4, 3},
    ANIMATION_TERMINATOR
};

const u16 sCoinsUnusedOam_Frame1[] = {
    4,
    OAM_ENTRY(-16, -28, SPRITE_SIZE_16x16, 0, 352, 6, 0),
    OAM_ENTRY(0, -28, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 352, 6, 0),
    OAM_ENTRY(-16, -12, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 352, 6, 0),
    OAM_ENTRY(0, -12, SPRITE_SIZE_16x16, ST_OAM_HFLIP | ST_OAM_VFLIP, 352, 6, 0),

};
const u16 sCoinsUnusedOam_Frame2[] = {
    2,
    OAM_ENTRY(-16, -28, SPRITE_SIZE_32x16, 0, 354, 6, 0),
    OAM_ENTRY(-16, -12, SPRITE_SIZE_32x16, ST_OAM_VFLIP, 354, 6, 0),

};
const u16 sCoinsUnusedOam_Frame3[] = {
    3,
    OAM_ENTRY(-8, -28, SPRITE_SIZE_16x16, 0, 359, 6, 0),
    OAM_ENTRY(-8, -12, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 359, 6, 0),
    OAM_ENTRY(-16, -20, SPRITE_SIZE_8x16, 0, 358, 6, 0),

};
const u16 sCoinsUnusedOam_Frame4[] = {
    2,
    OAM_ENTRY(-8, -28, SPRITE_SIZE_16x16, 0, 361, 6, 0),
    OAM_ENTRY(-8, -12, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 361, 6, 0),

};
const u16 sCoinsUnusedOam_Frame5[] = {
    2,
    OAM_ENTRY(-4, -28, SPRITE_SIZE_8x16, 0, 363, 6, 0),
    OAM_ENTRY(-4, -12, SPRITE_SIZE_8x16, ST_OAM_VFLIP, 363, 6, 0),

};
const u16 sCoinsUnusedOam_Frame6[] = {
    2,
    OAM_ENTRY(-8, -28, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 361, 6, 0),
    OAM_ENTRY(-8, -12, SPRITE_SIZE_16x16, ST_OAM_HFLIP | ST_OAM_VFLIP, 361, 6, 0),

};
const u16 sCoinsUnusedOam_Frame7[] = {
    3,
    OAM_ENTRY(-8, -28, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 359, 6, 0),
    OAM_ENTRY(-8, -12, SPRITE_SIZE_16x16, ST_OAM_HFLIP | ST_OAM_VFLIP, 359, 6, 0),
    OAM_ENTRY(8, -20, SPRITE_SIZE_8x16, ST_OAM_HFLIP, 358, 6, 0),

};
const u16 sCoinsUnusedOam_Frame8[] = {
    2,
    OAM_ENTRY(-16, -28, SPRITE_SIZE_32x16, ST_OAM_HFLIP, 354, 6, 0),
    OAM_ENTRY(-16, -12, SPRITE_SIZE_32x16, ST_OAM_HFLIP | ST_OAM_VFLIP, 354, 6, 0),

};

const struct AnimationFrame sCoinsUnusedOamData[] = {
    {sCoinsUnusedOam_Frame1, 6},
    {sCoinsUnusedOam_Frame2, 6},
    {sCoinsUnusedOam_Frame3, 6},
    {sCoinsUnusedOam_Frame4, 6},
    {sCoinsUnusedOam_Frame5, 6},
    {sCoinsUnusedOam_Frame6, 6},
    {sCoinsUnusedOam_Frame7, 6},
    {sCoinsUnusedOam_Frame8, 6},
    ANIMATION_TERMINATOR
};

const u16 sDiamondOam_Frame1[] = {
    3,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame2[] = {
    4,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_8x16, 0, 359, 5, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame3[] = {
    6,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(4, -12, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_16x16, 0, 360, 5, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_8x8, 0, 509, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame4[] = {
    6,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(4, -12, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-1, -24, SPRITE_SIZE_8x16, ST_OAM_HFLIP, 362, 5, 0),
    OAM_ENTRY(-8, -24, SPRITE_SIZE_8x16, 0, 362, 5, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame5[] = {
    3,
    OAM_ENTRY(4, -12, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame6[] = {
    3,
    OAM_ENTRY(4, -12, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame7[] = {
    4,
    OAM_ENTRY(4, -12, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame8[] = {
    3,
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame9[] = {
    4,
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame10[] = {
    4,
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame11[] = {
    4,
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame12[] = {
    4,
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame13[] = {
    5,
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 220, 4, 0),
};
const u16 sDiamondOam_Frame14[] = {
    4,
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame15[] = {
    4,
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame16[] = {
    4,
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(7, -22, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame17[] = {
    3,
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame18[] = {
    3,
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame19[] = {
    3,
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame20[] = {
    4,
    OAM_ENTRY(8, -14, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(-20, -16, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame21[] = {
    3,
    OAM_ENTRY(-20, -16, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame22[] = {
    3,
    OAM_ENTRY(-21, -16, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame23[] = {
    3,
    OAM_ENTRY(-20, -16, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame24[] = {
    4,
    OAM_ENTRY(-20, -16, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame25[] = {
    4,
    OAM_ENTRY(-20, -16, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame26[] = {
    4,
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
    OAM_ENTRY(-20, -16, SPRITE_SIZE_8x8, 0, 220, 4, 0),
};
const u16 sDiamondOam_Frame27[] = {
    4,
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(-20, -16, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame28[] = {
    4,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame29[] = {
    4,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame30[] = {
    4,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 219, 4, 0),
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 220, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame31[] = {
    4,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 218, 4, 0),
    OAM_ENTRY(-13, -26, SPRITE_SIZE_8x8, 0, 221, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};
const u16 sDiamondOam_Frame32[] = {
    3,
    OAM_ENTRY(11, -18, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    OAM_ENTRY(-16, -24, SPRITE_SIZE_32x16, 0, 298, 5, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 296, 5, 0),
};

const struct AnimationFrame sDiamondOamData[] = {
    {sDiamondOam_Frame1, 4},  {sDiamondOam_Frame2, 4},  {sDiamondOam_Frame3, 4},  {sDiamondOam_Frame4, 4},
    {sDiamondOam_Frame5, 4},  {sDiamondOam_Frame6, 4},  {sDiamondOam_Frame7, 4},  {sDiamondOam_Frame8, 4},
    {sDiamondOam_Frame9, 4},  {sDiamondOam_Frame10, 4}, {sDiamondOam_Frame11, 4}, {sDiamondOam_Frame12, 4},
    {sDiamondOam_Frame13, 4}, {sDiamondOam_Frame14, 4}, {sDiamondOam_Frame15, 4}, {sDiamondOam_Frame16, 4},
    {sDiamondOam_Frame17, 4}, {sDiamondOam_Frame18, 4}, {sDiamondOam_Frame19, 4}, {sDiamondOam_Frame20, 4},
    {sDiamondOam_Frame21, 4}, {sDiamondOam_Frame22, 4}, {sDiamondOam_Frame23, 4}, {sDiamondOam_Frame24, 4},
    {sDiamondOam_Frame25, 4}, {sDiamondOam_Frame26, 4}, {sDiamondOam_Frame27, 4}, {sDiamondOam_Frame28, 4},
    {sDiamondOam_Frame29, 4}, {sDiamondOam_Frame30, 4}, {sDiamondOam_Frame31, 4}, {sDiamondOam_Frame32, 4},
    ANIMATION_TERMINATOR
};

const u16 sLittleHeartOam_Frame1[] = {
    1,
    OAM_ENTRY(-4, -8, SPRITE_SIZE_8x8, 0, 496, 6, 0),
};
const u16 sLittleHeartOam_Frame2[] = {
    1,
    OAM_ENTRY(-4, -8, SPRITE_SIZE_8x8, 0, 497, 6, 0),
};
const u16 sLittleHeartOam_Frame3[] = {
    1,
    OAM_ENTRY(-4, -8, SPRITE_SIZE_8x8, 0, 498, 6, 0),
};
const u16 sLittleHeartOam_Frame4[] = {
    1,
    OAM_ENTRY(-3, -7, SPRITE_SIZE_8x8, 0, 497, 6, 0),
};

const struct AnimationFrame sLittleHeartOamData[] = {
    {sLittleHeartOam_Frame1, 8},
    {sLittleHeartOam_Frame2, 8},
    {sLittleHeartOam_Frame3, 8},
    {sLittleHeartOam_Frame4, 8},
    ANIMATION_TERMINATOR
};

const s16 sDiamondFloatYVelocity[] = {0, 0, 0, 0, 0,           0, 0, 0, 0, -PIXEL_SIZE, 0,      0, 0, 0, 0,
                                      0, 0, 0, 0, -PIXEL_SIZE, 0, 0, 0, 0, 0,           0,      0, 0, 0, PIXEL_SIZE,
                                      0, 0, 0, 0, 0,           0, 0, 0, 0, PIXEL_SIZE,  S16_MAX};
#else
extern const u16 sCoin500PointsOam_Frame1[];
extern const u16 sCoin500PointsOam_Frame2[];
extern const u16 sCoin500PointsOam_Frame3[];
extern const u16 sCoin500PointsOam_Frame4[];
extern const u16 sCoin500PointsOam_Frame5[];
extern const u16 sCoin500PointsOam_Frame6[];
extern const u16 sCoin500PointsOam_Frame7[];
extern const u16 sCoin100PointsOam_Frame1[];
extern const u16 sCoin100PointsOam_Frame2[];
extern const u16 sCoin100PointsOam_Frame3[];
extern const u16 sCoin100PointsOam_Frame4[];
extern const u16 sCoin100PointsOam_Frame5[];
extern const u16 sCoin100PointsOam_Frame6[];
extern const u16 sCoin100PointsOam_Frame7[];
extern const u16 sCoin50PointsOam_Frame1[];
extern const u16 sCoin50PointsOam_Frame2[];
extern const u16 sCoin50PointsOam_Frame3[];
extern const u16 sCoin50PointsOam_Frame4[];
extern const u16 sCoin50PointsOam_Frame5[];
extern const u16 sCoin50PointsOam_Frame6[];
extern const u16 sCoin50PointsOam_Frame7[];
extern const u16 sCoin10PointsOam_Frame1[];
extern const u16 sCoin10PointsOam_Frame2[];
extern const u16 sCoin10PointsOam_Frame3[];
extern const u16 sCoin10PointsOam_Frame4[];
extern const u16 sCoin10PointsOam_Frame5[];
extern const u16 sCoinsUnusedOamFrame[];
extern const struct AnimationFrame sCoin500PointsOamData[];
extern const struct AnimationFrame sCoin100PointsOamData[];
extern const struct AnimationFrame sCoin50PointsOamData[];
extern const struct AnimationFrame sCoin10PointsOamData[];
extern const u16 sCoinsUnusedOam_Frame1[];
extern const u16 sCoinsUnusedOam_Frame2[];
extern const u16 sCoinsUnusedOam_Frame3[];
extern const u16 sCoinsUnusedOam_Frame4[];
extern const u16 sCoinsUnusedOam_Frame5[];
extern const u16 sCoinsUnusedOam_Frame6[];
extern const u16 sCoinsUnusedOam_Frame7[];
extern const u16 sCoinsUnusedOam_Frame8[];
extern const struct AnimationFrame sCoinsUnusedOamData[];
extern const u16 sDiamondOam_Frame1[];
extern const u16 sDiamondOam_Frame2[];
extern const u16 sDiamondOam_Frame3[];
extern const u16 sDiamondOam_Frame4[];
extern const u16 sDiamondOam_Frame5[];
extern const u16 sDiamondOam_Frame6[];
extern const u16 sDiamondOam_Frame7[];
extern const u16 sDiamondOam_Frame8[];
extern const u16 sDiamondOam_Frame9[];
extern const u16 sDiamondOam_Frame10[];
extern const u16 sDiamondOam_Frame11[];
extern const u16 sDiamondOam_Frame12[];
extern const u16 sDiamondOam_Frame13[];
extern const u16 sDiamondOam_Frame14[];
extern const u16 sDiamondOam_Frame15[];
extern const u16 sDiamondOam_Frame16[];
extern const u16 sDiamondOam_Frame17[];
extern const u16 sDiamondOam_Frame18[];
extern const u16 sDiamondOam_Frame19[];
extern const u16 sDiamondOam_Frame20[];
extern const u16 sDiamondOam_Frame21[];
extern const u16 sDiamondOam_Frame22[];
extern const u16 sDiamondOam_Frame23[];
extern const u16 sDiamondOam_Frame24[];
extern const u16 sDiamondOam_Frame25[];
extern const u16 sDiamondOam_Frame26[];
extern const u16 sDiamondOam_Frame27[];
extern const u16 sDiamondOam_Frame28[];
extern const u16 sDiamondOam_Frame29[];
extern const u16 sDiamondOam_Frame30[];
extern const u16 sDiamondOam_Frame31[];
extern const u16 sDiamondOam_Frame32[];
extern const struct AnimationFrame sDiamondOamData[];
extern const u16 sLittleHeartOam_Frame1[];
extern const u16 sLittleHeartOam_Frame2[];
extern const u16 sLittleHeartOam_Frame3[];
extern const u16 sLittleHeartOam_Frame4[];
extern const struct AnimationFrame sLittleHeartOamData[];
extern const s16 sDiamondFloatYVelocity[];
#endif

void func_802BADC(void)
{
    gCurrentSprite.work1 += 1;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
    );
    if (gSpriteCollisionTileType == 0x11) {
        if (gCurrentSprite.work2 >= 2) {
            gCurrentSprite.work2 -= 1;
        } else {
            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.work2 = 0;
            }
        }
        gCurrentSprite.pose = SPOSE_2D;
    } else {
        gCurrentSprite.xPosition -= gCurrentSprite.work2;
        func_8023B88();
        if ((gSpriteCollisionResult & 14) == 0) {
            if (gSpriteCollisionResult == 0 && gCurrentSprite.pose == SPOSE_2C) {
                gCurrentSprite.pose = SPOSE_43;
            } else {
                if ((func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft),
                     gSpriteCollisionTileType == 0) &&
                    (func_8023BFC(
                         gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
                     ),
                     gSpriteCollisionTileType == 0)) {
                    gCurrentSprite.pose = SPOSE_47;
                } else {
                    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
                        if ((gCurrentSprite.work1 & 1) == 0) {
                            gCurrentSprite.work2 -= 1;
                        }
                    } else {
                        if ((gCurrentSprite.work1 & 3) == 0) {
                            gCurrentSprite.work2 -= 1;
                        }
                    }
                }
            }
        } else if ((gSpriteCollisionResult & 0xF1) == 1) {
            if (gCurrentSprite.pose != SPOSE_2C) {
                gCurrentSprite.pose = SPOSE_47;
            }
        } else if (gCurrentSprite.work2 >= 5) {
            gCurrentSprite.work2 -= 1;
        } else {
            gCurrentSprite.pose = SPOSE_45;
        }
    }
}

void func_802BC24(void)
{
    gCurrentSprite.work1 += 1;
    func_8023BFC(
        gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
    );
    if (gSpriteCollisionTileType == 0x11) {
        if (gCurrentSprite.work2 >= 2) {
            gCurrentSprite.work2 -= 1;
        } else {
            func_8023BFC(
                gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
            );
            if (gSpriteCollisionTileType == 0x11) {
                gCurrentSprite.work2 = 0;
            }
        }
        gCurrentSprite.pose = SPOSE_2B;
    } else {
        gCurrentSprite.xPosition += gCurrentSprite.work2;
        func_8023B88();
        if ((gSpriteCollisionResult & 14) == 0) {
            if (gSpriteCollisionResult == 0 && gCurrentSprite.pose == SPOSE_2E) {
                gCurrentSprite.pose = SPOSE_45;
            } else {
                if ((func_8023BFC(
                         gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
                     ),
                     gSpriteCollisionTileType == 0) &&
                    (func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft),
                     gSpriteCollisionTileType == 0)) {
                    gCurrentSprite.pose = SPOSE_49;
                } else {
                    if (gCurrentSprite.status & SPRITE_STATUS_HEAVY) {
                        if ((gCurrentSprite.work1 & 1) == 0) {
                            gCurrentSprite.work2 -= 1;
                        }
                    } else {
                        if ((gCurrentSprite.work1 & 3) == 0) {
                            gCurrentSprite.work2 -= 1;
                        }
                    }
                }
            }
        } else if ((gSpriteCollisionResult & 0xF1) == 0) {
            if (gCurrentSprite.pose != SPOSE_2E) {
                gCurrentSprite.pose = SPOSE_49;
            }
        } else if (gCurrentSprite.work2 >= 5) {
            gCurrentSprite.work2 -= 1;
        } else {
            gCurrentSprite.pose = SPOSE_43;
        }
    }
}

void CoinPose44(void)
{
    u32 unk_r2;
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sUnk_8352AFC, sSpriteGravityVelocityTable);
    gCurrentSprite.xPosition -= gCurrentSprite.work2;

    unk_r2 = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }

    if (gSpriteCollisionResult) {
        gCurrentSprite.yPosition = unk_r2;
        if (gCurrentSprite.work2 > 3) {
            gCurrentSprite.work2 -= 2;
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
    } else {
        func_8023BFC(
            gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
        );
        if (gSpriteCollisionTileType == 0x11) {
            gCurrentSprite.pose = SPOSE_45;
        }
    }
}

void CoinPose46(void)
{
    u32 unk_r2;
    s16 yVelocity;
    u32 timer;

    SPRITE_UTIL_LOOKUP_GRAVITY_BY_WEIGHT(sUnk_8352AFC, sSpriteGravityVelocityTable);
    gCurrentSprite.xPosition += gCurrentSprite.work2;

    unk_r2 = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }

    if (gSpriteCollisionResult) {
        gCurrentSprite.yPosition = unk_r2;
        if (gCurrentSprite.work2 > 3) {
            gCurrentSprite.work2 -= 2;
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
    } else {
        func_8023BFC(
            gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
        );
        if (gSpriteCollisionTileType == 0x11) {
            gCurrentSprite.pose = SPOSE_43;
        }
    }
}

void CoinInit(void)
{
    switch (gCurrentSprite.globalID) {
        case PSPRITE_COIN_50POINTS:
            GP_SET(gCurrentSprite.pOamData, sCoin50PointsOamData);
            gCurrentSprite.work2 = 7;
            gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
            gCurrentSprite.drawDistanceUp = 0;
            gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.hitboxExtentUp = 3 * QUARTER_BLOCK_SIZE + PIXEL_SIZE;
            gCurrentSprite.hitboxExtentDown = 0;
            gCurrentSprite.hitboxExtentLeft = QUARTER_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentRight = 3 * PIXEL_SIZE;
            break;

        case PSPRITE_COIN_100POINTS:
            GP_SET(gCurrentSprite.pOamData, sCoin100PointsOamData);
            gCurrentSprite.work2 = 6;
            gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
            gCurrentSprite.drawDistanceUp = 0;
            gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.hitboxExtentUp = BLOCK_SIZE - EIGHTH_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentDown = 0;
            gCurrentSprite.hitboxExtentLeft = QUARTER_BLOCK_SIZE + PIXEL_SIZE;
            gCurrentSprite.hitboxExtentRight = QUARTER_BLOCK_SIZE;
            break;

        case PSPRITE_COIN_500POINTS:
            GP_SET(gCurrentSprite.pOamData, sCoin500PointsOamData);
            gCurrentSprite.work2 = 5;
            gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
            gCurrentSprite.drawDistanceUp = 0;
            gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.hitboxExtentUp = BLOCK_SIZE - PIXEL_SIZE;
            gCurrentSprite.hitboxExtentDown = 0;
            gCurrentSprite.hitboxExtentLeft = QUARTER_BLOCK_SIZE + 2 * PIXEL_SIZE;
            gCurrentSprite.hitboxExtentRight = QUARTER_BLOCK_SIZE + PIXEL_SIZE;
            break;

        case PSPRITE_COIN_1000POINTS:
            GP_SET(gCurrentSprite.pOamData, sDiamondOamData);
            gCurrentSprite.work2 = 4;
            gCurrentSprite.drawDistanceDown = 24;
            gCurrentSprite.drawDistanceUp = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
            gCurrentSprite.hitboxExtentUp = BLOCK_SIZE + QUARTER_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentDown = 2 * BLOCK_SIZE + EIGHTH_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + EIGHTH_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE + EIGHTH_BLOCK_SIZE - PIXEL_SIZE;
            break;

        case PSPRITE_LITTLE_HEART:
            gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.drawDistanceUp = 0;
            gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.hitboxExtentUp = HALF_BLOCK_SIZE - PIXEL_SIZE;
            gCurrentSprite.hitboxExtentDown = 0;
            gCurrentSprite.hitboxExtentLeft = QUARTER_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentRight = QUARTER_BLOCK_SIZE - PIXEL_SIZE;
            gCurrentSprite.work2 = 4;
            if (gHeartGauge.current + gHeartGauge.filling >= HEART_GAUGE_MAX) {
                gCurrentSprite.globalID = PSPRITE_COIN_100POINTS;
                GP_SET(gCurrentSprite.pOamData, sCoin100PointsOamData);
            } else {
                GP_SET(gCurrentSprite.pOamData, sLittleHeartOamData);
            }
            break;

        default:
            GP_SET(gCurrentSprite.pOamData, sCoin10PointsOamData);
            gCurrentSprite.work2 = 8;
            gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.drawDistanceUp = 0;
            gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS / 2;
            gCurrentSprite.hitboxExtentUp = HALF_BLOCK_SIZE - PIXEL_SIZE;
            gCurrentSprite.hitboxExtentDown = 0;
            gCurrentSprite.hitboxExtentLeft = QUARTER_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentRight = 3 * PIXEL_SIZE;
            break;
    }
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;

    gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
    gCurrentSprite.warioCollision = 6;
    gCurrentSprite.yPosition -= 3 * EIGHTH_BLOCK_SIZE;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;

    if (gWarioData.damageTimer) {
        if (gWarioData.horizontalDirection & DPAD_RIGHT) {
            gCurrentSprite.pose = SPOSE_3F;
            gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
        } else {
            gCurrentSprite.pose = SPOSE_39;
            gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
        }
    } else {
        SpriteUtilTurnTowardWario();
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            gCurrentSprite.pose = SPOSE_39;
        } else {
            gCurrentSprite.pose = SPOSE_3F;
        }
    }
}

void CoinPose2F(void)
{
    gCurrentSprite.pose = SPOSE_30;
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.work0 = CONVERT_SECONDS(8.0 / 15);
    } else {
        gCurrentSprite.work0 = CONVERT_SECONDS(2);
    }
}

void CoinPose30(void)
{
    if (gCurrentSprite.work0 != 0) {
        TIMER_COUNT_DOWN(gCurrentSprite.work0);
        if (gCurrentSprite.work0 == 0) {
            gCurrentSprite.health = CONVERT_SECONDS(2);
        }
    }
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = SPOSE_1D;
    }
}

void CoinCollect(void)
{
    gCurrentSprite.status = SPRITE_STATUS_NONE;
    switch (gCurrentSprite.globalID) {
        case PSPRITE_COIN_50POINTS:
            AddStageScore(CONVERT_SCORE(50));
            SpawnSecondarySprite(gCurrentSprite.yPosition - 90, gCurrentSprite.xPosition, SSPRITE_SCORE_50);
            if (gWarioData.reaction == REACTION_WATER) {
                m4aSongNumStart(SE_GET_COIN_50POINTS_UNDERWATER);
            } else {
                m4aSongNumStart(SE_GET_COIN_50POINTS);
            }
            break;

        case PSPRITE_COIN_100POINTS:
            AddStageScore(CONVERT_SCORE(100));
            SpawnSecondarySprite(gCurrentSprite.yPosition - 120, gCurrentSprite.xPosition, SSPRITE_SCORE_100);
            if (gWarioData.reaction == REACTION_WATER) {
                m4aSongNumStart(SE_GET_COIN_100POINTS_UNDERWATER);
            } else {
                m4aSongNumStart(SE_GET_COIN_100POINTS);
            }
            break;

        case PSPRITE_COIN_500POINTS:
            AddStageScore(CONVERT_SCORE(500));
            SpawnSecondarySprite(gCurrentSprite.yPosition - 150, gCurrentSprite.xPosition, SSPRITE_SCORE_500);
            if (gWarioData.reaction == REACTION_WATER) {
                m4aSongNumStart(SE_GET_COIN_500POINTS_UNDERWATER);
            } else {
                m4aSongNumStart(SE_GET_COIN_500POINTS);
            }
            break;

        case PSPRITE_COIN_1000POINTS:
            AddStageScore(CONVERT_SCORE(1000));
            SpawnSecondarySprite(
                gCurrentSprite.yPosition - 180, gCurrentSprite.xPosition - BLOCK_SIZE, SSPRITE_SCORE_1000
            );
            if (gWarioData.reaction == REACTION_WATER) {
                m4aSongNumStart(SE_GET_DIAMOND_UNDERWATER);
            } else {
                m4aSongNumStart(SE_GET_DIAMOND);
            }
            break;

        case PSPRITE_LITTLE_HEART:
            if (gHeartGauge.current + gHeartGauge.filling < HEART_GAUGE_MAX) {
                gHeartGauge.filling += 1;
            }
            if (gWarioData.reaction == REACTION_WATER) {
                m4aSongNumStart(SE_GET_LITTLE_HEART_UNDERWATER);
            } else {
                m4aSongNumStart(SE_GET_LITTLE_HEART);
            }
            break;

        default:
            AddStageScore(CONVERT_SCORE(10));
            SpawnSecondarySprite(gCurrentSprite.yPosition - 60, gCurrentSprite.xPosition, SSPRITE_SCORE_10);
            if (gWarioData.reaction == REACTION_WATER) {
                m4aSongNumStart(SE_GET_COIN_10POINTS_UNDERWATER);
            } else {
                m4aSongNumStart(SE_GET_COIN_10POINTS);
            }
            break;
    }
}

void CoinPose1D(void)
{
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
}

void CoinPose2B(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    gCurrentSprite.work1 = 0;
}

void CoinPose2C(void)
{
    func_80238E8();
    if (gCurrentSprite.work2 >= 2) {
        func_802BADC();
    } else {
        gCurrentSprite.pose = SPOSE_2F;
    }
}

void CoinPose2D(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    gCurrentSprite.work1 = 0;
}

void CoinPose2E(void)
{
    func_80238E8();
    if (gCurrentSprite.work2 >= 2) {
        func_802BC24();
    } else {
        gCurrentSprite.pose = SPOSE_2F;
    }
}

void DiamondInit(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND;
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 8;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
    gCurrentSprite.hitboxExtentUp = BLOCK_SIZE + QUARTER_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentDown = 2 * BLOCK_SIZE + EIGHTH_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + EIGHTH_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE + EIGHTH_BLOCK_SIZE - PIXEL_SIZE;
    GP_SET(gCurrentSprite.pOamData, sDiamondOamData);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = 6;
    gCurrentSprite.yPosition -= PIXEL_SIZE;
    gCurrentSprite.xPosition += HALF_BLOCK_SIZE;
}

void DiamondFloat(void)
{
    SpriteUtilLookupFloatingAnimation(sDiamondFloatYVelocity);
}

void DiamondCollect(void)
{
    gCurrentSprite.status = SPRITE_STATUS_NONE;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
    AddStageScore(CONVERT_SCORE(1000));
    SpawnSecondarySprite(
        gCurrentSprite.yPosition - SUBPIXELS_FROM_PIXELS(25), gCurrentSprite.xPosition - BLOCK_SIZE, SSPRITE_SCORE_1000
    );
    if (gWarioData.reaction == REACTION_WATER) {
        m4aSongNumStart(SE_GET_DIAMOND_UNDERWATER);
    } else {
        m4aSongNumStart(SE_GET_DIAMOND);
    }
    VoiceSetPlay(VS_WARIO_TREASURE);
}

void ChanceWheelDiamondInit(void)
{
    gCurrentSprite.globalID = PSPRITE_COIN_1000POINTS;
    GP_SET(gCurrentSprite.pOamData, sDiamondOamData);
    gCurrentSprite.drawDistanceDown = 24;
    gCurrentSprite.drawDistanceUp = 8;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
    gCurrentSprite.hitboxExtentUp = BLOCK_SIZE + QUARTER_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentDown = 2 * BLOCK_SIZE + EIGHTH_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + EIGHTH_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE + EIGHTH_BLOCK_SIZE - PIXEL_SIZE;
    gCurrentSprite.work2 = 4;
    SpriteUtilTurnTowardWario();
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.pose = SPOSE_3B;
    } else {
        gCurrentSprite.pose = SPOSE_41;
    }
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.disableWarioCollisionTimer = CONVERT_SECONDS(0.25);
    gCurrentSprite.warioCollision = 6;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
}

void SpriteCoin(void)
{
    u8 despawnTimer;

    despawnTimer = gCurrentSprite.health;
    if (despawnTimer) {
        if ((despawnTimer & 1) == 0) {
            gCurrentSprite.status = gCurrentSprite.status ^ SPRITE_STATUS_HIDDEN;
        }
        TIMER_COUNT_DOWN(gCurrentSprite.health);
        if (gCurrentSprite.health == 0) {
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            return;
        }
    }
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        if (gStageFrameCounter & 1) {
            TIMER_COUNT_DOWN(gCurrentSprite.currentAnimationFrame);
        }
        if (gStageFrameCounter & 3 && gCurrentSprite.pose != SPOSE_CRUSHED_OR_COLLECTED_INIT) {
            return;
        }
    }
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            CoinInit();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            CoinCollect();
            break;

        case SPOSE_1D:
            CoinPose1D();
        case SPOSE_1E:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            func_8023EE0();
            break;

        case SPOSE_2B:
            CoinPose2B();
        case SPOSE_2C:
            CoinPose2C();
            break;

        case SPOSE_2D:
            CoinPose2D();
        case SPOSE_2E:
            CoinPose2E();
            break;

        case SPOSE_2F:
            CoinPose2F();
        case SPOSE_30:
            CoinPose30();
            break;

        case SPOSE_35:
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

        case SPOSE_33:
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
        case SPOSE_47:
            func_8024AC0();
        case SPOSE_44:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            CoinPose44();
            break;

        case SPOSE_45:
        case SPOSE_49:
            func_8024BEC();
        case SPOSE_46:
            gCurrentSprite.status &= ~SPRITE_STATUS_UNDERWATER;
            CoinPose46();
            break;

        default:
            gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
            break;
    }
}

void SpriteDiamond(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            DiamondInit();
            break;

        case SPOSE_IDLE:
            DiamondFloat();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            DiamondCollect();
            break;
    }
}

void SpriteChanceWheelDiamond(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            ChanceWheelDiamondInit();
    }
}

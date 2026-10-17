
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

#ifndef PORTABLE
const s16 sTreasureFloatYVelocity[] = {
    0, 0, 0, 0, 0, 0, -1, -1, -2, -2, -3, -4, -3, -2, -2, -1, -1, 0, 0, 0,       0,
    0, 0, 0, 0, 0, 1, 1,  2,  2,  3,  4,  3,  2,  2,  1,  1,  0,  0, 0, S16_MAX,
};

const u16 sHeartRefillOam_Frame1[] = {
    2,
    OAM_ENTRY(-8, -16, SPRITE_SIZE_16x8, 0, 164, 6, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 376, 7, 0),
};
const u16 sHeartRefillOam_Frame2[] = {
    2,
    OAM_ENTRY(-8, -16, SPRITE_SIZE_16x8, 0, 164, 6, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 376, 6, 0),
};
const u16 sJewelPieceNWOamFrame[] = {
    1,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 226, 4, 0),
};
const u16 sJewelPieceSWOamFrame[] = {
    1,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 228, 4, 0),
};
const u16 sJewelPieceNEOamFrame[] = {
    1,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 230, 4, 0),
};
const u16 sJewelPieceSEOamFrame[] = {
    1,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 232, 4, 0),
};
const u16 sTreasureUnusedOamFrame1[] = {
    2,
    OAM_ENTRY(-8, -16, SPRITE_SIZE_16x8, 0, 164, 6, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 376, 5, 0),
};
const u16 sTreasureGlowOam_Frame2[] = {
    1,
    OAM_ENTRY(-4, -4, SPRITE_SIZE_8x8, 0, 193, 4, 0),
};
const u16 sTreasureGlowOam_Frame1[] = {
    2,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_32x16, 0, 444, 4, 0),
    OAM_ENTRY(-16, 0, SPRITE_SIZE_32x16, ST_OAM_VFLIP, 444, 4, 0),
};
const u16 sTreasureGlowOam_Frame3[] = {
    2,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_32x16, 0, 440, 4, 0),
    OAM_ENTRY(-16, 0, SPRITE_SIZE_32x16, ST_OAM_VFLIP, 440, 4, 0),
};
const u16 sTreasureGlowDisappearingOam_Frame1[] = {
    4,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 378, 4, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 378, 4, 0),
    OAM_ENTRY(-16, 0, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 378, 4, 0),
    OAM_ENTRY(0, 0, SPRITE_SIZE_16x16, ST_OAM_HFLIP | ST_OAM_VFLIP, 378, 4, 0),
};
const u16 sTreasureGlowDisappearingOam_Frame2[] = {
    4,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 380, 4, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 380, 4, 0),
    OAM_ENTRY(-16, 0, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 380, 4, 0),
    OAM_ENTRY(0, 0, SPRITE_SIZE_16x16, ST_OAM_HFLIP | ST_OAM_VFLIP, 380, 4, 0),
};
const u16 sTreasureUnusedOamFrame2[] = {
    1,
    OAM_ENTRY(0, 0, SPRITE_SIZE_8x8, 0, 166, 4, 0),
};
const u16 sTreasureUnusedOamFrame3[] = {
    1,
    OAM_ENTRY(0, 0, SPRITE_SIZE_16x16, 0, 224, 4, 0),
};
const u16 sHeartRefillOam_Frame3[] = {
    2,
    OAM_ENTRY(-8, -16, SPRITE_SIZE_16x8, 0, 164, 6, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 376, 4, 0),
};
const u16 sTreasureUnusedOamFrame4[] = {
    1,
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x16, 0, 234, 7, 0),
};
const u16 sCDOamFrame[] = {
    2,
    OAM_ENTRY(-12, -12, SPRITE_SIZE_32x16, 0, 352, 4, 0),
    OAM_ENTRY(-20, -4, SPRITE_SIZE_32x16, ST_OAM_HFLIP | ST_OAM_VFLIP, 352, 4, 0),
};

const struct AnimationFrame sCDOam[] = {{sCDOamFrame, U8_MAX}, ANIMATION_TERMINATOR};
const struct AnimationFrame sHeartRefillOam[] = {
    {sHeartRefillOam_Frame1, 14},
    {sHeartRefillOam_Frame2, 12},
    {sHeartRefillOam_Frame3, 12},
    {sHeartRefillOam_Frame2, 12},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sJewelPieceNWOam[] = {{sJewelPieceNWOamFrame, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sJewelPieceSWOam[] = {{sJewelPieceSWOamFrame, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sJewelPieceNEOam[] = {{sJewelPieceNEOamFrame, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sJewelPieceSEOam[] = {{sJewelPieceSEOamFrame, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sTreasureGlowOam[] = {
    {sTreasureGlowOam_Frame1, 2},
    {sTreasureGlowOam_Frame2, 1},
    {sTreasureGlowOam_Frame1, 2},
    {sTreasureGlowOam_Frame2, 1},
    {sTreasureGlowOam_Frame3, 2},
    {sTreasureGlowOam_Frame2, 1},
    {sTreasureGlowOam_Frame3, 2},
    {sTreasureGlowOam_Frame2, 1},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sTreasureGlowDisappearingOam[] = {
    {sTreasureGlowOam_Frame1, 2},
    {sTreasureGlowOam_Frame2, 1},
    {sTreasureGlowOam_Frame1, 2},
    {sTreasureGlowOam_Frame2, 1},
    {sTreasureGlowDisappearingOam_Frame1, 1},
    {sTreasureGlowDisappearingOam_Frame2, 2},
    ANIMATION_TERMINATOR
};

const u16 sBoxCDClosedOamFrame[] = {
    4,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 514, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 514, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
};
const u16 sBoxCDOpeningOam_Frame1[] = {
    4,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 516, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 516, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
};
const u16 sBoxCDOpeningOam_Frame2[] = {
    4,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 518, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 518, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
};
const u16 sBoxCDOpenedOamFrame[] = {
    4,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 520, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 520, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
};
const u16 sBoxCDOpeningOam_Frame3[] = {
    4,
    OAM_ENTRY(-16, -31, SPRITE_SIZE_16x16, 0, 520, 8, 0),
    OAM_ENTRY(0, -31, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 520, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
};
const u16 sBoxUnusedOamFrame1[] = {
    2,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 520, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 520, 8, 0),
};
const u16 sBoxUnusedOamFrame2[] = {
    2,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
};
const u16 sBoxJewelPieceNWClosedOamFrame[] = {
    5,
    OAM_ENTRY(-5, -29, SPRITE_SIZE_8x8, 0, 532, 13, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 524, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 524, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxJewelPieceOpeningOam_Frame1[] = {
    4,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 526, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 526, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxJewelPieceOpeningOam_Frame2[] = {
    4,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 528, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 528, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxJewelPieceOpeningOam_Frame3[] = {
    4,
    OAM_ENTRY(-16, -31, SPRITE_SIZE_16x16, 0, 530, 8, 0),
    OAM_ENTRY(0, -31, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 530, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxJewelPieceOpenedOamFrame[] = {
    4,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 530, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 530, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxUnusedOamFrame3[] = {
    2,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 530, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 530, 8, 0),
};
const u16 sBoxUnusedOamFrame4[] = {
    2,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxJewelPieceNEClosedOamFrame[] = {
    5,
    OAM_ENTRY(-3, -29, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 532, 13, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 524, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 524, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxJewelPieceSWClosedOamFrame[] = {
    5,
    OAM_ENTRY(-5, -27, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 13, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 524, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 524, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxJewelPieceSEClosedOamFrame[] = {
    5,
    OAM_ENTRY(-3, -27, SPRITE_SIZE_8x8, 0, 564, 13, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 524, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 524, 8, 0),
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 522, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 522, 8, 0),
};
const u16 sBoxHeartRefillClosedOamFrame[] = {
    4,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 533, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 533, 8, 0),
};
const u16 sBoxHeartRefillOpeningOam_Frame1[] = {
    4,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 535, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 535, 8, 0),
};
const u16 sBoxHeartRefillOpeningOam_Frame2[] = {
    4,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 537, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 537, 8, 0),
};
const u16 sBoxHeartRefillOpenedOamFrame[] = {
    4,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 539, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 539, 8, 0),
};
const u16 sBoxHeartRefillOpeningOam_Frame3[] = {
    4,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_16x16, 0, 512, 8, 0),
    OAM_ENTRY(0, -16, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 512, 8, 0),
    OAM_ENTRY(-16, -31, SPRITE_SIZE_16x16, 0, 539, 8, 0),
    OAM_ENTRY(0, -31, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 539, 8, 0),
};
const u16 sBoxUnusedOamFrame5[] = {
    2,
    OAM_ENTRY(-16, -32, SPRITE_SIZE_16x16, 0, 539, 8, 0),
    OAM_ENTRY(0, -32, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 539, 8, 0),
};

const struct AnimationFrame sBoxCDOpeningOam[] = {
    {sBoxCDOpeningOam_Frame1, 3},
    {sBoxCDOpeningOam_Frame2, 3},
    {sBoxCDOpenedOamFrame, 3},
    {sBoxCDOpeningOam_Frame3, 2},
    {sBoxCDOpenedOamFrame, 1},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxCDClosedOam[] = {{sBoxCDClosedOamFrame, U8_MAX}, ANIMATION_TERMINATOR};
const struct AnimationFrame sBoxCDOpenedOam[] = {{sBoxCDOpenedOamFrame, U8_MAX}, ANIMATION_TERMINATOR};
const struct AnimationFrame sBoxJewelPieceOpeningOam[] = {
    {sBoxJewelPieceOpeningOam_Frame1, 3},
    {sBoxJewelPieceOpeningOam_Frame2, 3},
    {sBoxJewelPieceOpenedOamFrame, 3},
    {sBoxJewelPieceOpeningOam_Frame3, 2},
    {sBoxJewelPieceOpenedOamFrame, 1},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxJewelPieceNWClosedOam[] = {
    {sBoxJewelPieceNWClosedOamFrame, U8_MAX},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxJewelPieceSWClosedOam[] = {
    {sBoxJewelPieceSWClosedOamFrame, U8_MAX},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxJewelPieceNEClosedOam[] = {
    {sBoxJewelPieceNEClosedOamFrame, U8_MAX},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxJewelPieceSEClosedOam[] = {
    {sBoxJewelPieceSEClosedOamFrame, U8_MAX},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxJewelPieceOpenedOam[] = {{sBoxJewelPieceOpenedOamFrame, U8_MAX}, ANIMATION_TERMINATOR};
const struct AnimationFrame sBoxHeartRefillOpeningOam[] = {
    {sBoxHeartRefillOpeningOam_Frame1, 3},
    {sBoxHeartRefillOpeningOam_Frame2, 3},
    {sBoxHeartRefillOpenedOamFrame, 3},
    {sBoxHeartRefillOpeningOam_Frame3, 2},
    {sBoxHeartRefillOpenedOamFrame, 1},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxHeartRefillClosedOam[] = {
    {sBoxHeartRefillClosedOamFrame, U8_MAX},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sBoxHeartRefillOpenedOam[] = {
    {sBoxHeartRefillOpenedOamFrame, U8_MAX},
    ANIMATION_TERMINATOR
};
#else
extern const s16 sTreasureFloatYVelocity[];
extern const u16 sHeartRefillOam_Frame1[];
extern const u16 sHeartRefillOam_Frame2[];
extern const u16 sJewelPieceNWOamFrame[];
extern const u16 sJewelPieceSWOamFrame[];
extern const u16 sJewelPieceNEOamFrame[];
extern const u16 sJewelPieceSEOamFrame[];
extern const u16 sTreasureUnusedOamFrame1[];
extern const u16 sTreasureGlowOam_Frame2[];
extern const u16 sTreasureGlowOam_Frame1[];
extern const u16 sTreasureGlowOam_Frame3[];
extern const u16 sTreasureGlowDisappearingOam_Frame1[];
extern const u16 sTreasureGlowDisappearingOam_Frame2[];
extern const u16 sTreasureUnusedOamFrame2[];
extern const u16 sTreasureUnusedOamFrame3[];
extern const u16 sHeartRefillOam_Frame3[];
extern const u16 sTreasureUnusedOamFrame4[];
extern const u16 sCDOamFrame[];
extern const struct AnimationFrame sCDOam[];
extern const struct AnimationFrame sHeartRefillOam[];
extern const struct AnimationFrame sJewelPieceNWOam[];
extern const struct AnimationFrame sJewelPieceSWOam[];
extern const struct AnimationFrame sJewelPieceNEOam[];
extern const struct AnimationFrame sJewelPieceSEOam[];
extern const struct AnimationFrame sTreasureGlowOam[];
extern const struct AnimationFrame sTreasureGlowDisappearingOam[];
extern const u16 sBoxCDClosedOamFrame[];
extern const u16 sBoxCDOpeningOam_Frame1[];
extern const u16 sBoxCDOpeningOam_Frame2[];
extern const u16 sBoxCDOpenedOamFrame[];
extern const u16 sBoxCDOpeningOam_Frame3[];
extern const u16 sBoxUnusedOamFrame1[];
extern const u16 sBoxUnusedOamFrame2[];
extern const u16 sBoxJewelPieceNWClosedOamFrame[];
extern const u16 sBoxJewelPieceOpeningOam_Frame1[];
extern const u16 sBoxJewelPieceOpeningOam_Frame2[];
extern const u16 sBoxJewelPieceOpeningOam_Frame3[];
extern const u16 sBoxJewelPieceOpenedOamFrame[];
extern const u16 sBoxUnusedOamFrame3[];
extern const u16 sBoxUnusedOamFrame4[];
extern const u16 sBoxJewelPieceNEClosedOamFrame[];
extern const u16 sBoxJewelPieceSWClosedOamFrame[];
extern const u16 sBoxJewelPieceSEClosedOamFrame[];
extern const u16 sBoxHeartRefillClosedOamFrame[];
extern const u16 sBoxHeartRefillOpeningOam_Frame1[];
extern const u16 sBoxHeartRefillOpeningOam_Frame2[];
extern const u16 sBoxHeartRefillOpenedOamFrame[];
extern const u16 sBoxHeartRefillOpeningOam_Frame3[];
extern const u16 sBoxUnusedOamFrame5[];
extern const struct AnimationFrame sBoxCDOpeningOam[];
extern const struct AnimationFrame sBoxCDClosedOam[];
extern const struct AnimationFrame sBoxCDOpenedOam[];
extern const struct AnimationFrame sBoxJewelPieceOpeningOam[];
extern const struct AnimationFrame sBoxJewelPieceNWClosedOam[];
extern const struct AnimationFrame sBoxJewelPieceSWClosedOam[];
extern const struct AnimationFrame sBoxJewelPieceNEClosedOam[];
extern const struct AnimationFrame sBoxJewelPieceSEClosedOam[];
extern const struct AnimationFrame sBoxJewelPieceOpenedOam[];
extern const struct AnimationFrame sBoxHeartRefillOpeningOam[];
extern const struct AnimationFrame sBoxHeartRefillClosedOam[];
extern const struct AnimationFrame sBoxHeartRefillOpenedOam[];
#endif

void BoxSetCommonProperties(void)
{
    gCurrentSprite.gfxSlot = 7;
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND;
    gCurrentSprite.drawDistanceDown = 2 * BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = BLOCK_SIZE - PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = BLOCK_SIZE - EIGHTH_BLOCK_SIZE;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
}

void BoxInit(void)
{
    gCurrentSprite.hitboxExtentUp = 2 * BLOCK_SIZE - EIGHTH_BLOCK_SIZE;
    gCurrentSprite.warioCollision = 0x1A;
    gCurrentSprite.pose = SPOSE_IDLE;
    BoxSetCommonProperties();
}

void BoxInit1(void)
{
    gCurrentSprite.hitboxExtentUp = BLOCK_SIZE;
    gCurrentSprite.warioCollision = 0x19;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceOpenedOam);
    gCurrentSprite.work0 = CONVERT_SECONDS(5.0 / 6.0);
    BoxSetCommonProperties();
}

void BoxInit2(void)
{
    gCurrentSprite.status = SPRITE_STATUS_NONE;
}

void BoxStartOpening(void)
{
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceOpeningOam);
    gCurrentSprite.work0 = CONVERT_SECONDS(0.2) + DELTA_TIME;
    gCurrentSprite.hitboxExtentUp = BLOCK_SIZE;
    gCurrentSprite.warioCollision = 0x19;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
        MAKE_PERSISTENT_DATA(SPOSE_INIT_1, PERSISTENT_STATUS_LOADED);
    m4aSongNumStart(SE_BOX_OPENING);
    VoiceSetPlay(VS_WARIO_TREASURE);
}

void BoxFinishOpening(void)
{
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceOpenedOam);
    gCurrentSprite.work0 = CONVERT_SECONDS(5.0 / 6.0);
    SpriteSpawnAsChild(PSPRITE_COIN_500POINTS, 0, 0, gCurrentSprite.yPosition - BLOCK_SIZE, gCurrentSprite.xPosition);
}

void BoxDisappearing(void)
{
    u8 temp, temp2;

    temp = TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (temp == 0) {
        gCurrentSprite.status = 0;
        return;
    }

    temp2 = gCurrentSprite.work0;
    if (temp2 < CONVERT_SECONDS(0.8)) {
        gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
        if ((temp2 & 1) == 0) {
            gCurrentSprite.status ^= SPRITE_STATUS_HIDDEN;
        }
    }
}

void Treasure_Init(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 2 * BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
    gCurrentSprite.hitboxExtentUp = BLOCK_SIZE;
    gCurrentSprite.hitboxExtentDown = QUARTER_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + EIGHTH_BLOCK_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE + PIXEL_SIZE;
    gCurrentSprite.warioCollision = 6;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
}

void TreasureGlow_Init(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
    gCurrentSprite.hitboxExtentUp = PIXEL_SIZE;
    gCurrentSprite.hitboxExtentDown = PIXEL_SIZE;
    gCurrentSprite.hitboxExtentLeft = PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = PIXEL_SIZE;
    gCurrentSprite.warioCollision = 6;
    GP_SET(gCurrentSprite.pOamData, sTreasureGlowOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
}

void TreasureGlow_Collected(void)
{
    GP_SET(gCurrentSprite.pOamData, sTreasureGlowDisappearingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = 9;
}

void SpriteBoxJewelPieceNE(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BoxInit();
            GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceNEClosedOam);
            break;

        case SPOSE_INIT_1:
            BoxInit1();
            SpriteSpawnAsChild(
                PSPRITE_JEWEL_PIECE_NE,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_GLOW_JEWEL_PIECE_NE,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            break;

        case SPOSE_INIT_2:
            BoxInit2();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_33:
        case SPOSE_35:
            BoxStartOpening();
        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 > 0) {
                break;
            }
            BoxFinishOpening();
            if (gCollectedNEJewelPiece) {
                gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                    MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            } else {
                SpriteSpawnAsChild(
                    PSPRITE_JEWEL_PIECE_NE,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
                SpriteSpawnAsChild(
                    PSPRITE_GLOW_JEWEL_PIECE_NE,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
            }
        case SPOSE_CRUSHED_OR_COLLECTED:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                BoxDisappearing();
            }
            break;
    }

    func_8026838();
}

void SpriteBoxJewelPieceSE(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BoxInit();
            GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceSEClosedOam);
            break;

        case SPOSE_INIT_1:
            BoxInit1();
            SpriteSpawnAsChild(
                PSPRITE_JEWEL_PIECE_SE,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_GLOW_JEWEL_PIECE_SE,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            break;

        case SPOSE_INIT_2:
            BoxInit2();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_33:
        case SPOSE_35:
            BoxStartOpening();
        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 > 0) {
                break;
            }
            BoxFinishOpening();
            if (gCollectedSEJewelPiece) {
                gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                    MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            } else {
                SpriteSpawnAsChild(
                    PSPRITE_JEWEL_PIECE_SE,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
                SpriteSpawnAsChild(
                    PSPRITE_GLOW_JEWEL_PIECE_SE,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
            }
        case SPOSE_CRUSHED_OR_COLLECTED:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                BoxDisappearing();
            }
            break;
    }

    func_8026838();
}

void SpriteBoxJewelPieceSW(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BoxInit();
            GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceSWClosedOam);
            break;

        case SPOSE_INIT_1:
            BoxInit1();
            SpriteSpawnAsChild(
                PSPRITE_JEWEL_PIECE_SW,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_GLOW_JEWEL_PIECE_SW,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            break;

        case SPOSE_INIT_2:
            BoxInit2();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_33:
        case SPOSE_35:
            BoxStartOpening();
        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 > 0) {
                break;
            }
            BoxFinishOpening();
            if (gCollectedSWJewelPiece) {
                gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                    MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            } else {
                SpriteSpawnAsChild(
                    PSPRITE_JEWEL_PIECE_SW,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
                SpriteSpawnAsChild(
                    PSPRITE_GLOW_JEWEL_PIECE_SW,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
            }
        case SPOSE_CRUSHED_OR_COLLECTED:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                BoxDisappearing();
            }
            break;
    }

    func_8026838();
}

void SpriteBoxJewelPieceNW(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BoxInit();
            GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceNWClosedOam);
            break;

        case SPOSE_INIT_1:
            BoxInit1();
            SpriteSpawnAsChild(
                PSPRITE_JEWEL_PIECE_NW,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_GLOW_JEWEL_PIECE_NW,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            break;

        case SPOSE_INIT_2:
            BoxInit2();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_33:
        case SPOSE_35:
            BoxStartOpening();
        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 > 0) {
                break;
            }
            BoxFinishOpening();
            if (gCollectedNWJewelPiece) {
                gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                    MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            } else {
                SpriteSpawnAsChild(
                    PSPRITE_JEWEL_PIECE_NW,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
                SpriteSpawnAsChild(
                    PSPRITE_GLOW_JEWEL_PIECE_NW,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
            }
        case SPOSE_CRUSHED_OR_COLLECTED:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                BoxDisappearing();
            }
            break;
    }

    func_8026838();
}

void SpriteBoxCD(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BoxInit();
            GP_SET(gCurrentSprite.pOamData, sBoxCDClosedOam);
            break;

        case SPOSE_INIT_1:
            BoxInit1();
            GP_SET(gCurrentSprite.pOamData, sBoxCDOpenedOam);
            SpriteSpawnAsChild(
                PSPRITE_CD,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_GLOW_CD,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            break;

        case SPOSE_INIT_2:
            BoxInit2();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_33:
        case SPOSE_35:
            BoxStartOpening();
            GP_SET(gCurrentSprite.pOamData, sBoxCDOpeningOam);
        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 > 0) {
                break;
            }
            BoxFinishOpening();
            GP_SET(gCurrentSprite.pOamData, sBoxCDOpenedOam);
            if (gCollectedCD) {
                gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                    MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            } else {
                SpriteSpawnAsChild(
                    PSPRITE_CD,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
                SpriteSpawnAsChild(
                    PSPRITE_GLOW_CD,
                    gCurrentSprite.roomSlot,
                    0,
                    gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                    gCurrentSprite.xPosition
                );
            }
        case SPOSE_CRUSHED_OR_COLLECTED:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                BoxDisappearing();
            }
            break;
    }

    func_8026838();
}

void SpriteBoxHeartRefill(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BoxInit();
            GP_SET(gCurrentSprite.pOamData, sBoxHeartRefillClosedOam);
            break;

        case SPOSE_INIT_1:
            BoxInit1();
            GP_SET(gCurrentSprite.pOamData, sBoxHeartRefillOpenedOam);
            SpriteSpawnAsChild(
                PSPRITE_HEART_REFILL,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_GLOW_HEART_REFILL,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            break;

        case SPOSE_INIT_2:
            BoxInit2();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_33:
        case SPOSE_35:
            BoxStartOpening();
            GP_SET(gCurrentSprite.pOamData, sBoxHeartRefillOpeningOam);
        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 > 0) {
                break;
            }
            BoxFinishOpening();
            GP_SET(gCurrentSprite.pOamData, sBoxHeartRefillOpenedOam);
            SpriteSpawnAsChild(
                PSPRITE_HEART_REFILL,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_GLOW_HEART_REFILL,
                gCurrentSprite.roomSlot,
                0,
                gCurrentSprite.yPosition - 2 * BLOCK_SIZE,
                gCurrentSprite.xPosition
            );
        case SPOSE_CRUSHED_OR_COLLECTED:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                BoxDisappearing();
            }
            break;
    }

    func_8026838();
}

void SpriteJewelPieceNE(void)
{
    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_NE)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            GP_SET(gCurrentSprite.pOamData, sJewelPieceNEOam);
            Treasure_Init();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            m4aSongNumStart(SE_GET_JEWEL_PIECE);
            gCollectedNEJewelPiece = 1;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            if (gWarioData.horizontalDirection & DPAD_LEFT) {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition - SUBPIXELS_FROM_PIXELS(50),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            } else {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition + SUBPIXELS_FROM_PIXELS(18),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            }
            break;

        default:
            SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            break;
    }
}

void SpriteJewelPieceSE(void)
{
    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_SE)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            GP_SET(gCurrentSprite.pOamData, sJewelPieceSEOam);
            Treasure_Init();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            m4aSongNumStart(SE_GET_JEWEL_PIECE);
            gCollectedSEJewelPiece = 1;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            if (gWarioData.horizontalDirection & DPAD_LEFT) {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition - SUBPIXELS_FROM_PIXELS(50),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            } else {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition + SUBPIXELS_FROM_PIXELS(18),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            }
            break;

        default:
            SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            break;
    }
}

void SpriteJewelPieceSW(void)
{
    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_SW)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            GP_SET(gCurrentSprite.pOamData, sJewelPieceSWOam);
            Treasure_Init();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            m4aSongNumStart(SE_GET_JEWEL_PIECE);
            gCollectedSWJewelPiece = 1;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            if (gWarioData.horizontalDirection & DPAD_LEFT) {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition - SUBPIXELS_FROM_PIXELS(50),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            } else {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition + SUBPIXELS_FROM_PIXELS(18),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            }
            break;

        default:
            SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            break;
    }
}

void SpriteJewelPieceNW(void)
{
    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_NW)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            GP_SET(gCurrentSprite.pOamData, sJewelPieceNWOam);
            Treasure_Init();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            m4aSongNumStart(SE_GET_JEWEL_PIECE);
            gCollectedNWJewelPiece = 1;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            if (gWarioData.horizontalDirection & DPAD_LEFT) {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition - SUBPIXELS_FROM_PIXELS(50),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            } else {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition + SUBPIXELS_FROM_PIXELS(18),
                    SSPRITE_JEWEL_PIECE_ICONS
                );
            }
            break;

        default:
            SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            break;
    }
}

void SpriteCD(void)
{
    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_CD)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            GP_SET(gCurrentSprite.pOamData, sCDOam);
            Treasure_Init();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            m4aSongNumStart(SE_GET_CD);
            gCollectedCD = 1;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            if (gWarioData.horizontalDirection & DPAD_LEFT) {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition - SUBPIXELS_FROM_PIXELS(34),
                    SSPRITE_CD_ICON
                );
            } else {
                SpawnSecondarySprite(
                    gWarioData.yPosition - SUBPIXELS_FROM_PIXELS(40),
                    gWarioData.xPosition + SUBPIXELS_FROM_PIXELS(18),
                    SSPRITE_CD_ICON
                );
            }
            break;

        default:
            SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            break;
    }
}

void SpriteHeartRefill(void)
{
    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_HEART_REFILL)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            GP_SET(gCurrentSprite.pOamData, sHeartRefillOam);
            Treasure_Init();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED);
            gHeartMeter.filling += MAX_HEARTS;
            if (gHeartMeter.current + gHeartMeter.filling > MAX_HEARTS) {
                gHeartMeter.filling = MAX_HEARTS - gHeartMeter.current;
            }
            gHeartMeter.unk_2 = gHeartMeter.filling << 3;
            gHeartMeter.unk_3 = 0;
            m4aSongNumStart(SE_GET_HEART_REFILL);
            break;

        default:
            SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            break;
    }
}

void SpriteTreasureGlowJewelPieceNE(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;

    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_NE)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            TreasureGlow_Init();
            break;

        case SPOSE_IDLE:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                TreasureGlow_Collected();
            } else {
                SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            }
            break;

        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;
    }
}

void SpriteTreasureGlowJewelPieceSE(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;

    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_SE)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            TreasureGlow_Init();
            break;

        case SPOSE_IDLE:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                TreasureGlow_Collected();
            } else {
                SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            }
            break;

        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;
    }
}

void SpriteTreasureGlowJewelPieceSW(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;

    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_SW)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            TreasureGlow_Init();
            break;

        case SPOSE_IDLE:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                TreasureGlow_Collected();
            } else {
                SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            }
            break;

        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;
    }
}

void SpriteTreasureGlowJewelPieceNW(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;

    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_JEWEL_PIECE_NW)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            TreasureGlow_Init();
            break;

        case SPOSE_IDLE:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                TreasureGlow_Collected();
            } else {
                SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            }
            break;

        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;
    }
}

void SpriteTreasureGlowCD(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;

    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_CD)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            TreasureGlow_Init();
            break;

        case SPOSE_IDLE:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                TreasureGlow_Collected();
            } else {
                SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            }
            break;

        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;
    }
}

void SpriteTreasureGlowHeartRefill(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;

    if (SpriteUtilDespawnChildWithParent(PSPRITE_BOX_HEART_REFILL)) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            TreasureGlow_Init();
            break;

        case SPOSE_IDLE:
            if (gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] ==
                MAKE_PERSISTENT_DATA(SPOSE_INIT_2, PERSISTENT_STATUS_LOADED)) {
                TreasureGlow_Collected();
            } else {
                SpriteUtilLookupFloatingAnimation(sTreasureFloatYVelocity);
            }
            break;

        case SPOSE_12:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;
    }
}

void SpriteTreasureUnused(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            gCurrentSprite.gfxSlot = 7;
            gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND;
            gCurrentSprite.drawDistanceDown = 2 * BLOCK_SIZE_PIXELS;
            gCurrentSprite.drawDistanceUp = 0;
            gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
            gCurrentSprite.hitboxExtentUp = 2 * BLOCK_SIZE - EIGHTH_BLOCK_SIZE;
            gCurrentSprite.hitboxExtentDown = 0;
            gCurrentSprite.hitboxExtentLeft = BLOCK_SIZE - PIXEL_SIZE;
            gCurrentSprite.hitboxExtentRight = BLOCK_SIZE - EIGHTH_BLOCK_SIZE;
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceOpenedOam);
            gCurrentSprite.pose = SPOSE_IDLE;
            gCurrentSprite.warioCollision = 0x19;
            gCurrentSprite.work1 = 0;
            break;
    }

    func_8026838();
}

void SpriteUnknown10(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            BoxInit();
            GP_SET(gCurrentSprite.pOamData, sBoxJewelPieceOpenedOam);
            break;
    }
}

#include "sprite_ai/spear_mask.h"

#include "score.h"
#include "sound.h"
#include "sprite_util.h"

#ifndef PORTABLE
const u16 sSpearMaskWalkingOam_Frame1[] = {
    3,
    OAM_ENTRY(-17, -13, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -8, SPRITE_SIZE_16x8, 0, 512, 8, 0),
    OAM_ENTRY(-10, -20, SPRITE_SIZE_16x16, 0, 576, 8, 0),
};
const u16 sSpearMaskWalkingOam_Frame2[] = {
    3,
    OAM_ENTRY(-16, -15, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, 0, 544, 8, 0),
    OAM_ENTRY(-10, -22, SPRITE_SIZE_16x16, 0, 576, 8, 0),
};
const u16 sSpearMaskWalkingOam_Frame3[] = {
    3,
    OAM_ENTRY(-15, -13, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 514, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
};
const u16 sSpearMaskWalkingOam_Frame4[] = {
    3,
    OAM_ENTRY(-14, -12, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 546, 8, 0),
    OAM_ENTRY(-10, -20, SPRITE_SIZE_16x16, 0, 576, 8, 0),
};
const u16 sSpearMaskWalkingOam_Frame5[] = {
    3,
    OAM_ENTRY(-15, -14, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 516, 8, 0),
    OAM_ENTRY(-10, -22, SPRITE_SIZE_16x16, 0, 576, 8, 0),
};
const u16 sSpearMaskWalkingOam_Frame6[] = {
    3,
    OAM_ENTRY(-16, -14, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 548, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
};
const u16 sOamFrame_83B3A78[] = {
    5,
    OAM_ENTRY(-4, -18, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-8, -21, SPRITE_SIZE_16x16, 0, 578, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
    OAM_ENTRY(6, -14, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-14, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sOamFrame_83B3A98[] = {
    5,
    OAM_ENTRY(-4, -17, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-8, -20, SPRITE_SIZE_16x16, 0, 578, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 518, 8, 0),
    OAM_ENTRY(6, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-14, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskCrushedOam_Frame1[] = {
    2,
    OAM_ENTRY(-17, -8, SPRITE_SIZE_32x8, 0, 552, 8, 0),
    OAM_ENTRY(-9, -16, SPRITE_SIZE_16x8, 0, 532, 8, 0),
};
const u16 sSpearMaskCrushedOam_Frame2[] = {
    1,
    OAM_ENTRY(-16, -8, SPRITE_SIZE_32x8, 0, 520, 8, 0),
};
const u16 sSpearMaskBashedOam_Frame1[] = {
    3,
    OAM_ENTRY(-6, -32, SPRITE_SIZE_16x32, 0, 535, 8, 0),
    OAM_ENTRY(-19, -33, SPRITE_SIZE_16x16, 0, 590, 8, 0),
    OAM_ENTRY(-8, -41, SPRITE_SIZE_8x8, 0, 565, 8, 0),
};
const u16 sSpearMaskBashedOam_Frame2[] = {
    1,
    OAM_ENTRY(-6, -32, SPRITE_SIZE_16x32, 0, 535, 8, 0),
};
const u16 sSpearMaskTurning1Oam_Frame1[] = {
    4,
    OAM_ENTRY(-5, -16, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-16, -12, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskTurning1Oam_Frame2[] = {
    4,
    OAM_ENTRY(-14, -13, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-5, -14, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-10, -19, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 518, 8, 0),
};
const u16 sSpearMaskTurning2Oam_Frame1[] = {
    4,
    OAM_ENTRY(-13, -12, SPRITE_SIZE_16x8, 0, 593, 8, 0),
    OAM_ENTRY(-5, -16, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 548, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
};
const u16 sSpearMaskTurning2Oam_Frame2[] = {
    4,
    OAM_ENTRY(-3, -17, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-8, -11, SPRITE_SIZE_16x8, 0, 593, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 526, 8, 0),
    OAM_ENTRY(-8, -22, SPRITE_SIZE_16x16, 0, 578, 8, 0),
};
const u16 sSpearMaskTurning2Oam_Frame3[] = {
    4,
    OAM_ENTRY(0, -17, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-5, -11, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 593, 8, 0),
    OAM_ENTRY(-6, -22, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 588, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 516, 8, 0),
};
const u16 sSpearMaskTurning3Oam_Frame1[] = {
    4,
    OAM_ENTRY(-8, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-14, -11, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -19, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 518, 8, 0),
};
const u16 sSpearMaskTurning3Oam_Frame2[] = {
    3,
    OAM_ENTRY(-14, -13, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3B9A[] = {
    4,
    OAM_ENTRY(-8, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 514, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
};
const u16 sSpearMaskHitWarioOam_Frame1[] = {
    3,
    OAM_ENTRY(-22, -13, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-9, -16, SPRITE_SIZE_16x16, 0, 580, 8, 0),
    OAM_ENTRY(-12, -22, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 582, 8, 0),
};
const u16 sSpearMaskStunnedOam_Frame1[] = {
    5,
    OAM_ENTRY(5, -12, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-8, -9, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-7, -20, SPRITE_SIZE_16x8, ST_OAM_VFLIP, 548, 8, 0),
    OAM_ENTRY(-10, -15, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 588, 8, 0),
    OAM_ENTRY(-12, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskStunnedOam_Frame2[] = {
    5,
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-8, -10, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-6, -22, SPRITE_SIZE_16x8, ST_OAM_VFLIP, 516, 8, 0),
    OAM_ENTRY(-10, -16, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 588, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskStunnedOam_Frame3[] = {
    5,
    OAM_ENTRY(5, -12, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-7, -19, SPRITE_SIZE_16x8, ST_OAM_VFLIP, 546, 8, 0),
    OAM_ENTRY(-8, -9, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-10, -15, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 588, 8, 0),
    OAM_ENTRY(-12, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskStunnedOam_Frame4[] = {
    5,
    OAM_ENTRY(-8, -20, SPRITE_SIZE_16x8, ST_OAM_VFLIP, 514, 8, 0),
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-5, -9, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-10, -15, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 588, 8, 0),
    OAM_ENTRY(-13, -12, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskStunnedOam_Frame5[] = {
    5,
    OAM_ENTRY(-10, -22, SPRITE_SIZE_16x8, ST_OAM_VFLIP, 544, 8, 0),
    OAM_ENTRY(4, -13, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-6, -10, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-11, -16, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 588, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskStunnedOam_Frame6[] = {
    5,
    OAM_ENTRY(-10, -19, SPRITE_SIZE_16x8, ST_OAM_VFLIP, 512, 8, 0),
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-5, -9, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-10, -15, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 588, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame1[] = {
    4,
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x8, 0, 528, 8, 0),
    OAM_ENTRY(4, -13, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-11, -15, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 576, 8, 0),
    OAM_ENTRY(-14, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame2[] = {
    4,
    OAM_ENTRY(-10, -23, SPRITE_SIZE_16x8, 0, 562, 8, 0),
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-10, -16, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 576, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame3[] = {
    4,
    OAM_ENTRY(-6, -22, SPRITE_SIZE_16x8, 0, 530, 8, 0),
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-10, -15, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 576, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame4[] = {
    2,
    OAM_ENTRY(-9, -21, SPRITE_SIZE_16x8, 0, 528, 8, 0),
    OAM_ENTRY(-10, -15, SPRITE_SIZE_16x16, ST_OAM_VFLIP, 576, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame5[] = {
    1,
    OAM_ENTRY(-8, -16, SPRITE_SIZE_16x16, 0, 528, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame6[] = {
    4,
    OAM_ENTRY(4, -12, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-6, -8, SPRITE_SIZE_16x8, 0, 558, 8, 0),
    OAM_ENTRY(-6, -22, SPRITE_SIZE_16x16, 0, 582, 8, 0),
    OAM_ENTRY(-12, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame7[] = {
    4,
    OAM_ENTRY(4, -13, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-6, -8, SPRITE_SIZE_16x8, 0, 556, 8, 0),
    OAM_ENTRY(-6, -24, SPRITE_SIZE_16x16, 0, 582, 8, 0),
    OAM_ENTRY(-12, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame8[] = {
    3,
    OAM_ENTRY(4, -12, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-6, -8, SPRITE_SIZE_16x8, 0, 524, 8, 0),
    OAM_ENTRY(-6, -24, SPRITE_SIZE_16x16, 0, 582, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame9[] = {
    5,
    OAM_ENTRY(-3, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 548, 8, 0),
    OAM_ENTRY(-6, -21, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 588, 8, 0),
    OAM_ENTRY(4, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame10[] = {
    5,
    OAM_ENTRY(-4, -16, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 526, 8, 0),
    OAM_ENTRY(-8, -21, SPRITE_SIZE_16x16, 0, 578, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame11[] = {
    5,
    OAM_ENTRY(-5, -17, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, 0, 544, 8, 0),
    OAM_ENTRY(-10, -22, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame12[] = {
    4,
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-10, -20, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 518, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskGettingUpOam_Frame13[] = {
    4,
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskTurningOam_Frame3[] = {
    3,
    OAM_ENTRY(-13, -13, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskSpearTwirlingOam_Frame1[] = {
    3,
    OAM_ENTRY(-12, -32, SPRITE_SIZE_32x32, ST_OAM_HFLIP | ST_OAM_VFLIP, 540, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskSpearTwirlingOam_Frame2[] = {
    3,
    OAM_ENTRY(-20, -32, SPRITE_SIZE_32x32, ST_OAM_HFLIP | ST_OAM_VFLIP, 537, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskSpearTwirlingOam_Frame3[] = {
    3,
    OAM_ENTRY(-19, -24, SPRITE_SIZE_32x32, 0, 540, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskSpearTwirlingOam_Frame4[] = {
    3,
    OAM_ENTRY(-12, -24, SPRITE_SIZE_32x32, 0, 537, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskTurningOam_Frame1[] = {
    5,
    OAM_ENTRY(-13, -11, SPRITE_SIZE_8x8, 0, 595, 8, 0),
    OAM_ENTRY(-5, -12, SPRITE_SIZE_8x8, 0, 596, 8, 0),
    OAM_ENTRY(3, -13, SPRITE_SIZE_8x8, 0, 597, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskTurningOam_Frame2[] = {
    5,
    OAM_ENTRY(-13, -15, SPRITE_SIZE_8x8, 0, 595, 8, 0),
    OAM_ENTRY(-5, -14, SPRITE_SIZE_8x8, 0, 596, 8, 0),
    OAM_ENTRY(3, -13, SPRITE_SIZE_8x8, 0, 597, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3E6C[] = {
    4,
    OAM_ENTRY(-8, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-16, -11, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3E86[] = {
    5,
    OAM_ENTRY(-15, -10, SPRITE_SIZE_16x8, 0, 595, 8, 0),
    OAM_ENTRY(1, -11, SPRITE_SIZE_8x8, 0, 597, 8, 0),
    OAM_ENTRY(-7, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3EA6[] = {
    5,
    OAM_ENTRY(-3, -11, SPRITE_SIZE_8x8, 0, 594, 8, 0),
    OAM_ENTRY(-11, -10, SPRITE_SIZE_8x8, 0, 593, 8, 0),
    OAM_ENTRY(-6, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3EC6[] = {
    5,
    OAM_ENTRY(-9, -10, SPRITE_SIZE_8x8, 0, 593, 8, 0),
    OAM_ENTRY(-2, -11, SPRITE_SIZE_8x8, 0, 594, 8, 0),
    OAM_ENTRY(-4, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-8, -21, SPRITE_SIZE_16x16, 0, 578, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3EE6[] = {
    5,
    OAM_ENTRY(1, -10, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 593, 8, 0),
    OAM_ENTRY(-6, -11, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 594, 8, 0),
    OAM_ENTRY(-4, -16, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-8, -21, SPRITE_SIZE_16x16, 0, 578, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3F06[] = {
    5,
    OAM_ENTRY(-5, -11, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 594, 8, 0),
    OAM_ENTRY(3, -10, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 593, 8, 0),
    OAM_ENTRY(-2, -16, SPRITE_SIZE_8x8, 0, 566, 8, 0),
    OAM_ENTRY(-6, -21, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, ST_OAM_HFLIP, 550, 8, 0),
};
const u16 sOamFrame_83B3F26[] = {
    5,
    OAM_ENTRY(0, -11, SPRITE_SIZE_8x8, 0, 594, 8, 0),
    OAM_ENTRY(-15, -10, SPRITE_SIZE_16x8, 0, 595, 8, 0),
    OAM_ENTRY(-7, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B3F46[] = {
    4,
    OAM_ENTRY(-16, -11, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-8, -16, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 566, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 588, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskStartingCaseOam_Frame1[] = {
    3,
    OAM_ENTRY(-15, -19, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -19, SPRITE_SIZE_16x16, 0, 584, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 518, 8, 0),
};
const u16 sSpearMaskStartingCaseOam_Frame2[] = {
    3,
    OAM_ENTRY(-15, -21, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 584, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskStartingCaseOam_Frame3[] = {
    3,
    OAM_ENTRY(-15, -25, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 584, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sSpearMaskChasingOam_Frame1[] = {
    3,
    OAM_ENTRY(-14, -22, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -8, SPRITE_SIZE_16x8, 0, 512, 8, 0),
    OAM_ENTRY(-10, -20, SPRITE_SIZE_16x16, 0, 584, 8, 0),
};
const u16 sSpearMaskChasingOam_Frame2[] = {
    3,
    OAM_ENTRY(-17, -26, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-9, -8, SPRITE_SIZE_16x8, 0, 544, 8, 0),
    OAM_ENTRY(-10, -22, SPRITE_SIZE_16x16, 0, 584, 8, 0),
};
const u16 sSpearMaskChasingOam_Frame3[] = {
    3,
    OAM_ENTRY(-16, -23, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 514, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 584, 8, 0),
};
const u16 sSpearMaskChasingOam_Frame4[] = {
    3,
    OAM_ENTRY(-16, -22, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 546, 8, 0),
    OAM_ENTRY(-10, -20, SPRITE_SIZE_16x16, 0, 584, 8, 0),
};
const u16 sSpearMaskChasingOam_Frame5[] = {
    3,
    OAM_ENTRY(-15, -26, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-6, -8, SPRITE_SIZE_16x8, 0, 516, 8, 0),
    OAM_ENTRY(-10, -22, SPRITE_SIZE_16x16, 0, 584, 8, 0),
};
const u16 sSpearMaskChasingOam_Frame6[] = {
    3,
    OAM_ENTRY(-15, -24, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 584, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 548, 8, 0),
};
const u16 sSpearMaskTransforming1Oam_Frame1[] = {
    4,
    OAM_ENTRY(5, -12, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 586, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
    OAM_ENTRY(-13, -12, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskTransforming2Oam_Frame1[] = {
    4,
    OAM_ENTRY(5, -12, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 586, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
    OAM_ENTRY(-13, -12, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sOamFrame_83B4048[] = {
    4,
    OAM_ENTRY(5, -11, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-10, -19, SPRITE_SIZE_16x16, 0, 586, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 518, 8, 0),
    OAM_ENTRY(-13, -11, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sSpearMaskRecoveringOam_Frame1[] = {
    4,
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sSpearMaskRecoveringOam_Frame2[] = {
    3,
    OAM_ENTRY(-16, -10, SPRITE_SIZE_32x8, 0, 595, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B4090[] = {
    5,
    OAM_ENTRY(-19, -15, SPRITE_SIZE_8x8, 0, 595, 8, 0),
    OAM_ENTRY(-11, -15, SPRITE_SIZE_8x8, 0, 594, 8, 0),
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B40B0[] = {
    5,
    OAM_ENTRY(-14, -10, SPRITE_SIZE_8x8, 0, 595, 8, 0),
    OAM_ENTRY(-6, -10, SPRITE_SIZE_8x8, 0, 594, 8, 0),
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 550, 8, 0),
};
const u16 sOamFrame_83B40D0[] = {
    4,
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-10, -9, SPRITE_SIZE_16x8, 0, 512, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-13, -15, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sOamFrame_83B40EA[] = {
    4,
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-9, -7, SPRITE_SIZE_16x8, 0, 544, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-13, -15, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sOamFrame_83B4104[] = {
    4,
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-8, -8, SPRITE_SIZE_16x8, 0, 514, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sOamFrame_83B411E[] = {
    4,
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-7, -9, SPRITE_SIZE_16x8, 0, 546, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};
const u16 sOamFrame_83B4138[] = {
    4,
    OAM_ENTRY(5, -13, SPRITE_SIZE_8x8, 0, 564, 8, 0),
    OAM_ENTRY(-7, -7, SPRITE_SIZE_16x8, 0, 516, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-13, -13, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 564, 8, 0),
};
const u16 sOamFrame_83B4152[] = {
    4,
    OAM_ENTRY(5, -14, SPRITE_SIZE_8x8, ST_OAM_VFLIP, 564, 8, 0),
    OAM_ENTRY(-7, -8, SPRITE_SIZE_16x8, 0, 548, 8, 0),
    OAM_ENTRY(-10, -21, SPRITE_SIZE_16x16, 0, 576, 8, 0),
    OAM_ENTRY(-13, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP | ST_OAM_VFLIP, 564, 8, 0),
};

const struct AnimationFrame sSpearMaskWalkingOam[] = {
    {sSpearMaskWalkingOam_Frame1, 7},
    {sSpearMaskWalkingOam_Frame2, 7},
    {sSpearMaskWalkingOam_Frame3, 7},
    {sSpearMaskWalkingOam_Frame4, 7},
    {sSpearMaskWalkingOam_Frame5, 7},
    {sSpearMaskWalkingOam_Frame6, 7},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskCrushedOam[] = {
    {sSpearMaskCrushedOam_Frame1, 2},
    {sSpearMaskCrushedOam_Frame2, 5},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskTackledOam[] = {
    {sSpearMaskBashedOam_Frame1, 18},
    {sSpearMaskBashedOam_Frame2, 40},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskTurning1Oam[] = {
    {sSpearMaskTurning1Oam_Frame1, 5},
    {sSpearMaskTurning1Oam_Frame2, 7},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskTurning2Oam[] = {
    {sSpearMaskTurning2Oam_Frame1, 6},
    {sSpearMaskTurning2Oam_Frame2, 6},
    {sSpearMaskTurning2Oam_Frame3, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskTurning3Oam[] = {
    {sSpearMaskTurning3Oam_Frame1, 8},
    {sSpearMaskTurning3Oam_Frame2, 8},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskOam_83B4224[] = {
    {sOamFrame_83B3B9A, 7},
    {sSpearMaskHitWarioOam_Frame1, 10},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskStunnedOam[] = {
    {sSpearMaskStunnedOam_Frame1, 6},
    {sSpearMaskStunnedOam_Frame2, 6},
    {sSpearMaskStunnedOam_Frame3, 6},
    {sSpearMaskStunnedOam_Frame4, 6},
    {sSpearMaskStunnedOam_Frame5, 6},
    {sSpearMaskStunnedOam_Frame6, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskGettingUpOam[] = {
    {sSpearMaskGettingUpOam_Frame1, 5},
    {sSpearMaskGettingUpOam_Frame2, 5},
    {sSpearMaskGettingUpOam_Frame3, 5},
    {sSpearMaskGettingUpOam_Frame2, 5},
    {sSpearMaskGettingUpOam_Frame1, 5},
    {sSpearMaskGettingUpOam_Frame2, 5},
    {sSpearMaskGettingUpOam_Frame3, 5},
    {sSpearMaskGettingUpOam_Frame2, 5},
    {sSpearMaskGettingUpOam_Frame4, 5},
    {sSpearMaskGettingUpOam_Frame5, 5},
    {sSpearMaskGettingUpOam_Frame6, 10},
    {sSpearMaskGettingUpOam_Frame7, 5},
    {sSpearMaskGettingUpOam_Frame8, 5},
    {sSpearMaskGettingUpOam_Frame9, 5},
    {sSpearMaskGettingUpOam_Frame10, 5},
    {sSpearMaskGettingUpOam_Frame11, 5},
    {sSpearMaskGettingUpOam_Frame12, 5},
    {sSpearMaskGettingUpOam_Frame13, 5},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskTransforming1Oam[] = {
    {sSpearMaskTransforming1Oam_Frame1, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskTransforming2Oam[] = {
    {sSpearMaskTransforming2Oam_Frame1, 5},
    {sSpearMaskTransforming1Oam_Frame1, 5},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskOam_83B4334[] = {{sOamFrame_83B4048, 8}, ANIMATION_TERMINATOR};
const struct AnimationFrame sSpearMaskOam_83B4344[] = {{sSpearMaskBashedOam_Frame2, 4}, ANIMATION_TERMINATOR};
const struct AnimationFrame sSpearMaskHitWarioOam[] = {{sSpearMaskHitWarioOam_Frame1, 4}, ANIMATION_TERMINATOR};
const struct AnimationFrame sSpearMaskPushedOam[] = {{sSpearMaskGettingUpOam_Frame13, 6}, ANIMATION_TERMINATOR};
const struct AnimationFrame sSpearMaskOam_83B4374[] = {{sSpearMaskRecoveringOam_Frame2, 4}, ANIMATION_TERMINATOR};
const struct AnimationFrame sSpearMaskRecoveringOam[] = {
    {sSpearMaskRecoveringOam_Frame1, 5},
    {sSpearMaskGettingUpOam_Frame13, 5},
    {sSpearMaskRecoveringOam_Frame2, 10},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskOam_83B43A4[] = {
    {sOamFrame_83B40D0, 4},
    {sOamFrame_83B40EA, 4},
    {sOamFrame_83B4104, 4},
    {sOamFrame_83B411E, 4},
    {sOamFrame_83B4138, 4},
    {sOamFrame_83B4152, 4},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskSpearTwirlingOam[] = {
    {sSpearMaskSpearTwirlingOam_Frame1, 7},
    {sSpearMaskSpearTwirlingOam_Frame2, 7},
    {sSpearMaskSpearTwirlingOam_Frame3, 7},
    {sSpearMaskSpearTwirlingOam_Frame4, 7},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskTurningOam[] = {
    {sSpearMaskTurningOam_Frame1, 6},
    {sSpearMaskTurningOam_Frame2, 6},
    {sSpearMaskTurningOam_Frame3, 8},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskOam_83B4424[] = {
    {sOamFrame_83B3E6C, 6},
    {sOamFrame_83B3E86, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskOam_83B443C[] = {
    {sOamFrame_83B3EA6, 6},
    {sOamFrame_83B3EC6, 6},
    {sOamFrame_83B3EE6, 6},
    {sOamFrame_83B3F06, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskOam_83B4464[] =
    {{sOamFrame_83B3F26, 6}, {sOamFrame_83B3F46, 6}, {sOamFrame_83B3F26, 6}, ANIMATION_TERMINATOR};
const struct AnimationFrame sSpearMaskOam_83B4484[] = {
    {sOamFrame_83B3F06, 6},
    {sOamFrame_83B3EE6, 6},
    {sOamFrame_83B3EC6, 6},
    {sOamFrame_83B3EA6, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskOam_83B44AC[] = {{sOamFrame_83B3E86, 6}, ANIMATION_TERMINATOR};
const struct AnimationFrame sSpearMaskStartingChaseOam[] = {
    {sSpearMaskStartingCaseOam_Frame1, 6},
    {sSpearMaskStartingCaseOam_Frame2, 6},
    {sSpearMaskStartingCaseOam_Frame3, 6},
    {sSpearMaskStartingCaseOam_Frame2, 6},
    ANIMATION_TERMINATOR
};
const struct AnimationFrame sSpearMaskChasingOam[] = {
    {sSpearMaskChasingOam_Frame1, 4},
    {sSpearMaskChasingOam_Frame2, 4},
    {sSpearMaskChasingOam_Frame3, 4},
    {sSpearMaskChasingOam_Frame4, 4},
    {sSpearMaskChasingOam_Frame5, 4},
    {sSpearMaskChasingOam_Frame6, 4},
    ANIMATION_TERMINATOR
};
#else
extern const u16 sSpearMaskWalkingOam_Frame1[];
extern const u16 sSpearMaskWalkingOam_Frame2[];
extern const u16 sSpearMaskWalkingOam_Frame3[];
extern const u16 sSpearMaskWalkingOam_Frame4[];
extern const u16 sSpearMaskWalkingOam_Frame5[];
extern const u16 sSpearMaskWalkingOam_Frame6[];
extern const u16 sOamFrame_83B3A78[];
extern const u16 sOamFrame_83B3A98[];
extern const u16 sSpearMaskCrushedOam_Frame1[];
extern const u16 sSpearMaskCrushedOam_Frame2[];
extern const u16 sSpearMaskBashedOam_Frame1[];
extern const u16 sSpearMaskBashedOam_Frame2[];
extern const u16 sSpearMaskTurning1Oam_Frame1[];
extern const u16 sSpearMaskTurning1Oam_Frame2[];
extern const u16 sSpearMaskTurning2Oam_Frame1[];
extern const u16 sSpearMaskTurning2Oam_Frame2[];
extern const u16 sSpearMaskTurning2Oam_Frame3[];
extern const u16 sSpearMaskTurning3Oam_Frame1[];
extern const u16 sSpearMaskTurning3Oam_Frame2[];
extern const u16 sOamFrame_83B3B9A[];
extern const u16 sSpearMaskHitWarioOam_Frame1[];
extern const u16 sSpearMaskStunnedOam_Frame1[];
extern const u16 sSpearMaskStunnedOam_Frame2[];
extern const u16 sSpearMaskStunnedOam_Frame3[];
extern const u16 sSpearMaskStunnedOam_Frame4[];
extern const u16 sSpearMaskStunnedOam_Frame5[];
extern const u16 sSpearMaskStunnedOam_Frame6[];
extern const u16 sSpearMaskGettingUpOam_Frame1[];
extern const u16 sSpearMaskGettingUpOam_Frame2[];
extern const u16 sSpearMaskGettingUpOam_Frame3[];
extern const u16 sSpearMaskGettingUpOam_Frame4[];
extern const u16 sSpearMaskGettingUpOam_Frame5[];
extern const u16 sSpearMaskGettingUpOam_Frame6[];
extern const u16 sSpearMaskGettingUpOam_Frame7[];
extern const u16 sSpearMaskGettingUpOam_Frame8[];
extern const u16 sSpearMaskGettingUpOam_Frame9[];
extern const u16 sSpearMaskGettingUpOam_Frame10[];
extern const u16 sSpearMaskGettingUpOam_Frame11[];
extern const u16 sSpearMaskGettingUpOam_Frame12[];
extern const u16 sSpearMaskGettingUpOam_Frame13[];
extern const u16 sSpearMaskTurningOam_Frame3[];
extern const u16 sSpearMaskSpearTwirlingOam_Frame1[];
extern const u16 sSpearMaskSpearTwirlingOam_Frame2[];
extern const u16 sSpearMaskSpearTwirlingOam_Frame3[];
extern const u16 sSpearMaskSpearTwirlingOam_Frame4[];
extern const u16 sSpearMaskTurningOam_Frame1[];
extern const u16 sSpearMaskTurningOam_Frame2[];
extern const u16 sOamFrame_83B3E6C[];
extern const u16 sOamFrame_83B3E86[];
extern const u16 sOamFrame_83B3EA6[];
extern const u16 sOamFrame_83B3EC6[];
extern const u16 sOamFrame_83B3EE6[];
extern const u16 sOamFrame_83B3F06[];
extern const u16 sOamFrame_83B3F26[];
extern const u16 sOamFrame_83B3F46[];
extern const u16 sSpearMaskStartingCaseOam_Frame1[];
extern const u16 sSpearMaskStartingCaseOam_Frame2[];
extern const u16 sSpearMaskStartingCaseOam_Frame3[];
extern const u16 sSpearMaskChasingOam_Frame1[];
extern const u16 sSpearMaskChasingOam_Frame2[];
extern const u16 sSpearMaskChasingOam_Frame3[];
extern const u16 sSpearMaskChasingOam_Frame4[];
extern const u16 sSpearMaskChasingOam_Frame5[];
extern const u16 sSpearMaskChasingOam_Frame6[];
extern const u16 sSpearMaskTransforming1Oam_Frame1[];
extern const u16 sSpearMaskTransforming2Oam_Frame1[];
extern const u16 sOamFrame_83B4048[];
extern const u16 sSpearMaskRecoveringOam_Frame1[];
extern const u16 sSpearMaskRecoveringOam_Frame2[];
extern const u16 sOamFrame_83B4090[];
extern const u16 sOamFrame_83B40B0[];
extern const u16 sOamFrame_83B40D0[];
extern const u16 sOamFrame_83B40EA[];
extern const u16 sOamFrame_83B4104[];
extern const u16 sOamFrame_83B411E[];
extern const u16 sOamFrame_83B4138[];
extern const u16 sOamFrame_83B4152[];
extern const struct AnimationFrame sSpearMaskWalkingOam[];
extern const struct AnimationFrame sSpearMaskCrushedOam[];
extern const struct AnimationFrame sSpearMaskTackledOam[];
extern const struct AnimationFrame sSpearMaskTurning1Oam[];
extern const struct AnimationFrame sSpearMaskTurning2Oam[];
extern const struct AnimationFrame sSpearMaskTurning3Oam[];
extern const struct AnimationFrame sSpearMaskOam_83B4224[];
extern const struct AnimationFrame sSpearMaskStunnedOam[];
extern const struct AnimationFrame sSpearMaskGettingUpOam[];
extern const struct AnimationFrame sSpearMaskTransforming1Oam[];
extern const struct AnimationFrame sSpearMaskTransforming2Oam[];
extern const struct AnimationFrame sSpearMaskOam_83B4334[];
extern const struct AnimationFrame sSpearMaskOam_83B4344[];
extern const struct AnimationFrame sSpearMaskHitWarioOam[];
extern const struct AnimationFrame sSpearMaskPushedOam[];
extern const struct AnimationFrame sSpearMaskOam_83B4374[];
extern const struct AnimationFrame sSpearMaskRecoveringOam[];
extern const struct AnimationFrame sSpearMaskOam_83B43A4[];
extern const struct AnimationFrame sSpearMaskSpearTwirlingOam[];
extern const struct AnimationFrame sSpearMaskTurningOam[];
extern const struct AnimationFrame sSpearMaskOam_83B4424[];
extern const struct AnimationFrame sSpearMaskOam_83B443C[];
extern const struct AnimationFrame sSpearMaskOam_83B4464[];
extern const struct AnimationFrame sSpearMaskOam_83B4484[];
extern const struct AnimationFrame sSpearMaskOam_83B44AC[];
extern const struct AnimationFrame sSpearMaskStartingChaseOam[];
extern const struct AnimationFrame sSpearMaskChasingOam[];
#endif

void SpearMaskSetWalkingHitbox(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
        gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE;
        gCurrentSprite.hitboxExtentRight = 3 * QUARTER_BLOCK_SIZE;
    } else {
        gCurrentSprite.hitboxExtentLeft = 3 * QUARTER_BLOCK_SIZE + PIXEL_SIZE;
        gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE - PIXEL_SIZE;
    }
}

void SpearMaskInit(void)
{
    gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS * 3 / 2;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS * 3 / 2;
    gCurrentSprite.hitboxExtentUp = BLOCK_SIZE;
    gCurrentSprite.hitboxExtentDown = 0;
    SpriteUtilTurnTowardWario();
    switch (gCurrentSprite.globalID) {
        case PSPRITE_SPEAR_MASK_RED:
            gCurrentSprite.palette = SPEAR_MASK_PALETTE_RED;
            break;
        case PSPRITE_SPEAR_MASK_BLUE:
            gCurrentSprite.palette = SPEAR_MASK_PALETTE_BLUE;
            break;
        default:
            gCurrentSprite.palette = SPEAR_MASK_PALETTE_YELLOW;
            break;
    }
}

void SpearMaskInitWalk(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    if (gCurrentSprite.globalID == PSPRITE_SPEAR_MASK_YELLOW) {
        GP_SET(gCurrentSprite.pOamData, sSpearMaskWalkingOam);
        gCurrentSprite.warioCollision = 3;
        SpearMaskSetWalkingHitbox();
    } else {
        GP_SET(gCurrentSprite.pOamData, sSpearMaskSpearTwirlingOam);
        gCurrentSprite.work0 = CONVERT_SECONDS(1.25) + DELTA_TIME;
        gCurrentSprite.warioCollision = 10;
        gCurrentSprite.work1 = 0;
        gCurrentSprite.hitboxExtentLeft = BLOCK_SIZE - PIXEL_SIZE;
        gCurrentSprite.hitboxExtentRight = BLOCK_SIZE - EIGHTH_BLOCK_SIZE;
    }
}

void SpearMaskWalkYellow(void)
{
    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (!gSpriteCollisionTileType) {
            gCurrentSprite.pose = SPOSE_1B;
            return;
        }
    } else {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            if ((gSpriteCollisionResult & 0xF0) &&
                (gCurrentSprite.xPosition % BLOCK_SIZE + gCurrentSprite.hitboxExtentRight >= BLOCK_SIZE)) {
                func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
                if (!(gSpriteCollisionTileType & 0xF0)) {
                    gCurrentSprite.pose = SPOSE_11;
                    return;
                }
                func_8023BFC(
                    gCurrentSprite.yPosition - HALF_BLOCK_SIZE,
                    gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
                );
                if (gSpriteCollisionTileType & 0x0F) {
                    gCurrentSprite.pose = SPOSE_11;
                    return;
                }
            }
        } else {
            if ((gSpriteCollisionResult & 0xF0) && (gCurrentSprite.xPosition % BLOCK_SIZE < gCurrentSprite.hitboxExtentLeft)) {
                func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
                if (!(gSpriteCollisionTileType & 0xF0)) {
                    gCurrentSprite.pose = SPOSE_11;
                    return;
                }
                func_8023BFC(
                    gCurrentSprite.yPosition - HALF_BLOCK_SIZE,
                    gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
                );
                if (gSpriteCollisionTileType & 0x0F) {
                    gCurrentSprite.pose = SPOSE_11;
                    return;
                }
            }
        }
    }
    func_8026374();
}

void SpearMaskTurnInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskTurning1Oam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_12;
    gCurrentSprite.work0 = CONVERT_SECONDS(0.2) + DELTA_TIME;
    gCurrentSprite.warioCollision = 3;
    SpearMaskSetWalkingHitbox();
}

void SpearMaskTurn(void)
{
    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        gCurrentSprite.pose = SPOSE_1B;
        return;
    }

    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 != 0) {
        return;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_12:
            gCurrentSprite.warioCollision = 1;
            GP_SET(gCurrentSprite.pOamData, sSpearMaskTurning2Oam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            gCurrentSprite.pose = SPOSE_14;
            gCurrentSprite.work0 = CONVERT_SECONDS(0.3);
            gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
            gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
            break;

        case SPOSE_14:
            gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
            gCurrentSprite.warioCollision = 3;
            SpearMaskSetWalkingHitbox();
            GP_SET(gCurrentSprite.pOamData, sSpearMaskTurning3Oam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            gCurrentSprite.pose = SPOSE_16;
            gCurrentSprite.work0 = CONVERT_SECONDS(0.25);
            break;

        default:
            gCurrentSprite.pose = SPOSE_IDLE_INIT;
            break;
    }
}

void SpearMaskPose17(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskRecoveringOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_18;
    gCurrentSprite.work0 = CONVERT_SECONDS(1.0 / 3.0);
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
}

void SpearMaskGetUpInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskGettingUpOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_1A;
    gCurrentSprite.work0 = CONVERT_SECONDS(1.0 + 7.0 / 12.0);
}

void SpearMaskGetUp(void)
{
    u8 timer;

    timer = gCurrentSprite.work0;
    if (timer < CONVERT_SECONDS(0.75) || timer > CONVERT_SECONDS(11.0 / 12.0)) {
        func_8023CD8();
        return;
    }

    if (gCurrentSprite.work0 == CONVERT_SECONDS(11.0 / 12.0)) {
        gCurrentSprite.yPosition -= HALF_BLOCK_SIZE;
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            gCurrentSprite.xPosition += 4;
        } else {
            gCurrentSprite.xPosition -= 4;
        }
    } else if (timer == CONVERT_SECONDS(5.0 / 6.0)) {
        gCurrentSprite.yPosition -= HALF_BLOCK_SIZE;
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            gCurrentSprite.xPosition += 4;
        } else {
            gCurrentSprite.xPosition -= 4;
        }
    } else if (timer == CONVERT_SECONDS(0.75)) {
        gCurrentSprite.yPosition += BLOCK_SIZE;
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            gCurrentSprite.xPosition -= 8;
        } else {
            gCurrentSprite.xPosition += 8;
        }
    }
    TIMER_COUNT_DOWN(gCurrentSprite.work0);
}

void SpearMaskPose1B(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskOam_83B43A4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1C;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
}

void SpearMaskPose1D(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_1E;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
}

void SpearMaskTackledInit(void)
{
    u32 value;

    GP_SET(gCurrentSprite.pOamData, sSpearMaskTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    switch (gCurrentSprite.globalID) {
        case PSPRITE_SPEAR_MASK_RED:
            value = CONVERT_SCORE(100);
            break;
        case PSPRITE_SPEAR_MASK_BLUE:
            value = CONVERT_SCORE(50);
            break;
        default:
            value = CONVERT_SCORE(10);
            break;
    }
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, value);
    SpawnSecondarySprite(gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition, SSPRITE_06);
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void SpearMaskTackledRightInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    SpearMaskTackledInit();
}

void SpearMaskTackledLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    SpearMaskTackledInit();
}

// Defeated a different way
void func_8027474(void)
{
    u32 value;

    GP_SET(gCurrentSprite.pOamData, sSpearMaskTackledOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    switch (gCurrentSprite.globalID) {
        case PSPRITE_SPEAR_MASK_RED:
            value = CONVERT_SCORE(100);
            break;
        case PSPRITE_SPEAR_MASK_BLUE:
            value = CONVERT_SCORE(50);
            break;
        default:
            value = CONVERT_SCORE(10);
            break;
    }
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, value);
    SpawnSecondarySprite(gCurrentSprite.yPosition - HALF_BLOCK_SIZE, gCurrentSprite.xPosition, SSPRITE_06);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void SpearMaskPose6A(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    func_8027474();
}

void SpearMaskPose69(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    func_8027474();
}

void SpearMaskPushedInit(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = CONVERT_SECONDS(0.2);
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
}

void SpearMaskPushedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_RIGHT;
    SpearMaskPushedInit();
}

void SpearMaskPushedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_PUSHED_LEFT;
    SpearMaskPushedInit();
}

void SpearMaskHitWario(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskHitWarioOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = CONVERT_SECONDS(0.2);
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void SpearMaskHitWarioLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_28;
    SpearMaskHitWario();
}

void SpearMaskHitWarioRightInit(void)
{
    gCurrentSprite.pose = SPOSE_2A;
    SpearMaskHitWario();
}

void func_80275D4(void)
{
    gCurrentSprite.work0 = CONVERT_SECONDS(1.0 / 3.0);
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
}

void SpearMaskPose6E(void)
{
    gCurrentSprite.pose = SPEAR_MASK_POSE_6F;
    func_80275D4();
}

void SpearMaskPose6F(void)
{
    func_80238A4();

    if (gCurrentSprite.work2 >= 4) {
        func_80242C8();
        return;
    }

    if (gCurrentSprite.work0 == CONVERT_SECONDS(1.0 / 3.0)) {
        GP_SET(gCurrentSprite.pOamData, sSpearMaskTurningOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.warioCollision = 3;
        SpearMaskSetWalkingHitbox();
    }

    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_11;
    }
}

void SpearMaskPose70(void)
{
    gCurrentSprite.pose = SPEAR_MASK_POSE_71;
    func_80275D4();
}

void SpearMaskPose71(void)
{
    func_80238A4();

    if (gCurrentSprite.work2 >= 4) {
        func_8024180();
        return;
    }

    if (gCurrentSprite.work0 == CONVERT_SECONDS(1.0 / 3.0)) {
        GP_SET(gCurrentSprite.pOamData, sSpearMaskTurningOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.warioCollision = 3;
        SpearMaskSetWalkingHitbox();
    }

    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = SPOSE_11;
    }
}

void func_80276C4(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
}

void SpearMaskPose2B(void)
{
    gCurrentSprite.pose = SPOSE_2C;
    func_80276C4();
}

void SpearMaskPose2D(void)
{
    gCurrentSprite.pose = SPOSE_2E;
    func_80276C4();
}

void SpearMaskCrushed(void)
{
    u32 value;

    GP_SET(gCurrentSprite.pOamData, sSpearMaskCrushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gCurrentSprite.work0 = CONVERT_SECONDS(2.0 / 15.0);  // 8?
    gCurrentSprite.status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    gCurrentSprite.status |= SPRITE_STATUS_MAYBE_DEAD;
    switch (gCurrentSprite.globalID) {
        case PSPRITE_SPEAR_MASK_RED:
            value = CONVERT_SCORE(100);
            break;
        case PSPRITE_SPEAR_MASK_BLUE:
            value = CONVERT_SCORE(50);
            break;
        default:
            value = CONVERT_SCORE(10);
            break;
    }
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, value);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void SpearMaskInstantlyKill(void)
{
    u32 value;

    switch (gCurrentSprite.globalID) {
        case PSPRITE_SPEAR_MASK_RED:
            value = CONVERT_SCORE(100);
            break;
        case PSPRITE_SPEAR_MASK_BLUE:
            value = CONVERT_SCORE(50);
            break;
        default:
            value = CONVERT_SCORE(10);
            break;
    }
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, value);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void func_80277E0(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = CONVERT_SECONDS(1.0 / 15.0);
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
        gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
        gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
    }
}

void SpearMaskPose33(void)
{
    gCurrentSprite.pose = SPOSE_34;
    func_80277E0();
}

void SpearMaskPose35(void)
{
    gCurrentSprite.pose = SPOSE_36;
    func_80277E0();
}

void func_802784C(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
}

void SpearMaskPose47(void)
{
    gCurrentSprite.pose = SPOSE_48;
    func_802784C();
}

void SpearMaskPose49(void)
{
    gCurrentSprite.pose = SPOSE_4A;
    func_802784C();
}

void SpearMaskInitTransform(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskTransforming1Oam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4C;
    gCurrentSprite.work0 = CONVERT_SECONDS(1.0 / 3.0);
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentLeft = HALF_BLOCK_SIZE + PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = HALF_BLOCK_SIZE;
    m4aSongNumStart(SOUND_62);
    switch (gCurrentSprite.globalID) {
        case PSPRITE_SPEAR_MASK_YELLOW:
            gCurrentSprite.globalID = PSPRITE_SPEAR_MASK_BLUE;
            gCurrentSprite.palette += 1;
            break;
        case PSPRITE_SPEAR_MASK_BLUE:
            gCurrentSprite.globalID = PSPRITE_SPEAR_MASK_RED;
            gCurrentSprite.palette += 1;
            break;
        default:
            gCurrentSprite.globalID = PSPRITE_SPEAR_MASK_YELLOW;
            gCurrentSprite.palette = SPEAR_MASK_PALETTE_YELLOW;
            break;
    }
}

void SpearMaskTransform(void)
{
    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 > 0) {
        func_8023BFC(gCurrentSprite.yPosition - gCurrentSprite.hitboxExtentUp, gCurrentSprite.xPosition);
        if (gSpriteCollisionTileType & 0xF) {
            gCurrentSprite.pose = SPOSE_1D;
            return;
        }

        SpriteUtilLookupGravity(sUnk_8352B18);
        return;
    }

    GP_SET(gCurrentSprite.pOamData, sSpearMaskTransforming2Oam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = SPOSE_4D;
    gCurrentSprite.work3 = 0;
}

void SpearMaskPose4D(void)
{
    u32 yPosition;

    yPosition = func_8023A60(gCurrentSprite.yPosition, gCurrentSprite.xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        gCurrentSprite.status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult) {
        gCurrentSprite.yPosition = yPosition;
        GP_SET(gCurrentSprite.pOamData, sSpearMaskOam_83B4334);
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.work0 = CONVERT_SECONDS(1.0 / 6.0);
        gCurrentSprite.pose = SPOSE_4E;
        return;
    }

    SpriteUtilLookupGravity(sUnk_8352B40);
}

void SpearMaskPose4E(void)
{
    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0) {
        func_8023B88();
        if (!gSpriteCollisionResult) {
            gCurrentSprite.pose = SPOSE_1B;
        }
    } else {
        gCurrentSprite.pose = SPOSE_17;
    }
}

void func_8027A8C(void)
{
    GP_SET(gCurrentSprite.pOamData, sSpearMaskStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void SpearMaskLiftedRightInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_RIGHT;
    func_8027A8C();
}

void SpearMaskLiftedLeftInit(void)
{
    gCurrentSprite.pose = SPOSE_BEING_LIFTED_LEFT;
    func_8027A8C();
}

void func_8027ACC(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    GP_SET(gCurrentSprite.pOamData, sSpearMaskStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void SpearMaskCarriedRight(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_RIGHT;
    func_8027ACC();
}

void SpearMaskCarriedLeft(void)
{
    gCurrentSprite.pose = SPOSE_CARRIED_LEFT;
    func_8027ACC();
}

void SpearMaskStartChasing(void)
{
    gCurrentSprite.pose = SPEAR_MASK_STARTING_CHASE;
    GP_SET(gCurrentSprite.pOamData, sSpearMaskStartingChaseOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 3;
    SpearMaskSetWalkingHitbox();
    gCurrentSprite.work0 = CONVERT_SECONDS(0.4);
}

void SpearMaskStartChase(void)
{
    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        gCurrentSprite.pose = SPOSE_1B;
        return;
    }

    TIMER_COUNT_DOWN(gCurrentSprite.work0);
    if (gCurrentSprite.work0 != 0) {
        return;
    }
    gCurrentSprite.pose = SPEAR_MASK_CHASING;
    GP_SET(gCurrentSprite.pOamData, sSpearMaskChasingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
}

void SpearMaskWalkBlueOrRed(void)
{
    u32 temp;

    func_80238A4();
    func_8023B88();
    if (!gSpriteCollisionResult) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
        } else {
            func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
        }
        if (!gSpriteCollisionTileType) {
            gCurrentSprite.pose = SPOSE_1B;
            return;
        }
    } else {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            if ((gSpriteCollisionResult & 0xF0) &&
                (gCurrentSprite.xPosition % BLOCK_SIZE + gCurrentSprite.hitboxExtentRight >= BLOCK_SIZE)) {
                func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight);
                if (!gSpriteCollisionTileType) {
                    gCurrentSprite.pose = SPOSE_11;
                    GP_SET(gCurrentSprite.pOamData, sSpearMaskTurning1Oam);
                    gCurrentSprite.currentAnimationFrame = 0;
                    gCurrentSprite.animationTimer = 0;
                    return;
                }
                func_8023BFC(
                    gCurrentSprite.yPosition - HALF_BLOCK_SIZE,
                    gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight
                );
                if (gSpriteCollisionTileType == 0x11) {
                    gCurrentSprite.pose = SPOSE_11;
                    GP_SET(gCurrentSprite.pOamData, sSpearMaskTurning1Oam);
                    gCurrentSprite.currentAnimationFrame = 0;
                    gCurrentSprite.animationTimer = 0;
                    return;
                }
            }
        } else {
            if ((gSpriteCollisionResult & 0xF0) && (gCurrentSprite.xPosition % BLOCK_SIZE < gCurrentSprite.hitboxExtentLeft)) {
                func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft);
                if (!gSpriteCollisionTileType) {
                    gCurrentSprite.pose = SPOSE_11;
                    GP_SET(gCurrentSprite.pOamData, sSpearMaskTurning1Oam);
                    gCurrentSprite.currentAnimationFrame = 0;
                    gCurrentSprite.animationTimer = 0;
                    return;
                }
                func_8023BFC(
                    gCurrentSprite.yPosition - HALF_BLOCK_SIZE,
                    gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft
                );
                if (gSpriteCollisionTileType == 0x11) {
                    gCurrentSprite.pose = SPOSE_11;
                    GP_SET(gCurrentSprite.pOamData, sSpearMaskTurning1Oam);
                    gCurrentSprite.currentAnimationFrame = 0;
                    gCurrentSprite.animationTimer = 0;
                    return;
                }
            }
        }
    }

    if (gCurrentSprite.work0 != 0) {
        TIMER_COUNT_DOWN(gCurrentSprite.work0);
        if (gCurrentSprite.work0 == 0) {
            GP_SET(gCurrentSprite.pOamData, sSpearMaskWalkingOam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
        } else {
            // What
            switch (gCurrentSprite.work0) {
                case CONVERT_SECONDS(1.0 / 3.0):
                    GP_SET(gCurrentSprite.pOamData, sSpearMaskTurningOam);
                    gCurrentSprite.currentAnimationFrame = 0;
                    gCurrentSprite.animationTimer = 0;
                    gCurrentSprite.warioCollision = 3;
                    SpearMaskSetWalkingHitbox();
            }
        }
        return;
    }

    if (gCurrentSprite.globalID == PSPRITE_SPEAR_MASK_RED) {
        if (gCurrentSprite.pose == SPOSE_IDLE) {
            temp = SpriteUtilWaitCheckWarioNearbyLeftRight(2 * BLOCK_SIZE - 1, SUBPIXELS_FROM_PIXELS(100));
            if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
                if (temp == NS_RIGHT) {
                    SpearMaskStartChasing();
                } else {
                    func_8026374();
                }
            } else {
                if (temp == NS_LEFT) {
                    SpearMaskStartChasing();
                } else {
                    func_8026374();
                }
            }
        } else {
            func_80263E0();
        }
    } else {
        func_8026374();
    }
}

void SpriteSpearMask(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = SPOSE_6B;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            SpearMaskInit();
        case SPOSE_IDLE_INIT:
            SpearMaskInitWalk();
        case SPOSE_IDLE:
            if (gCurrentSprite.globalID == PSPRITE_SPEAR_MASK_YELLOW) {
                SpearMaskWalkYellow();
            } else {
                SpearMaskWalkBlueOrRed();
            }
            break;

        case SPOSE_11:
            SpearMaskTurnInit();
        case SPOSE_12:
        case SPOSE_14:
        case SPOSE_16:
            SpearMaskTurn();
            break;

        case SPOSE_17:
            SpearMaskPose17();
        case SPOSE_18:
            func_8023C94();
            break;

        case SPOSE_19:
            SpearMaskGetUpInit();
        case SPOSE_1A:
            SpearMaskGetUp();
            break;

        case SPOSE_1B:
            SpearMaskPose1B();
        case SPOSE_1C:
            func_8023D48();
            break;

        case SPOSE_1D:
            SpearMaskPose1D();
        case SPOSE_1E:
            func_8023EE0();
            break;

        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            SpearMaskTackledRightInit();
        case SPOSE_TACKLED_RIGHT:
            SpriteUtilFallOffscreenRight();
            break;

        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            SpearMaskTackledLeftInit();
        case SPOSE_TACKLED_LEFT:
            SpriteUtilFallOffscreenLeft();
            break;

        case SPOSE_PUSHED_RIGHT_INIT:
            SpearMaskPushedRightInit();
        case SPOSE_PUSHED_RIGHT:
            SpriteUtilPushedRight();
            break;

        case SPOSE_PUSHED_LEFT_INIT:
            SpearMaskPushedLeftInit();
        case SPOSE_PUSHED_LEFT:
            SpriteUtilPushedLeft();
            break;

        case SPOSE_27:
            SpearMaskHitWarioLeftInit();
        case SPOSE_28:
            SpriteUtilPushedRight();
            break;

        case SPOSE_29:
            SpearMaskHitWarioRightInit();
        case SPOSE_2A:
            SpriteUtilPushedLeft();
            break;

        case SPOSE_2B:
            SpearMaskPose2B();
        case SPOSE_2C:
            func_8024478();
            break;

        case SPOSE_2D:
            SpearMaskPose2D();
        case SPOSE_2E:
            func_802449C();
            break;

        case SPOSE_2F:
            func_80244C0();
        case SPOSE_30:
            func_80244E0();
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            SpearMaskCrushed();
        case SPOSE_CRUSHED_OR_COLLECTED:
            SpriteUtilDieAfterDelay();
            break;

        case SPOSE_33:
            SpearMaskPose33();
        case SPOSE_34:
            func_8024988();
            break;

        case SPOSE_35:
            SpearMaskPose35();
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
            func_8024AD4();
            break;

        case SPOSE_45:
            func_8024BEC();
        case SPOSE_46:
            func_8024C00();
            break;

        case SPOSE_47:
            SpearMaskPose47();
        case SPOSE_48:
            func_8024AD4();
            break;

        case SPOSE_49:
            SpearMaskPose49();
        case SPOSE_4A:
            func_8024C00();
            break;

        case SPOSE_4B:
            SpearMaskInitTransform();
        case SPOSE_4C:
            SpearMaskTransform();
            break;

        case SPOSE_4D:
            SpearMaskPose4D();
            break;

        case SPOSE_4E:
            SpearMaskPose4E();
            break;

        case SPOSE_BEING_LIFTED_RIGHT_INIT:
            SpearMaskLiftedRightInit();
            break;

        case SPOSE_BEING_LIFTED_RIGHT:
            SpriteUtilLiftingSpriteRight();
            break;

        case SPOSE_BEING_LIFTED_LEFT_INIT:
            SpearMaskLiftedLeftInit();
            break;

        case SPOSE_BEING_LIFTED_LEFT:
            SpriteUtilLiftingSpriteLeft();
            break;

        case SPOSE_CARRIED_RIGHT_INIT:
            SpearMaskCarriedRight();
            break;

        case SPOSE_CARRIED_RIGHT:
            SpriteUtilCarryingSpriteRight();
            break;

        case SPOSE_CARRIED_LEFT_INIT:
            SpearMaskCarriedLeft();
            break;

        case SPOSE_CARRIED_LEFT:
            SpriteUtilCarryingSpriteLeft();
            break;

        case SPOSE_THROWN_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_SOFT;
        case SPOSE_THROWN_LEFT_SOFT:
            SpriteUtilThrownLeftSoft();
            break;

        case SPOSE_THROWN_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_LEFT_HARD;
        case SPOSE_THROWN_LEFT_HARD:
            SpriteUtilThrownLeftHard();
            break;

        case SPOSE_THROWN_UP_LEFT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_SOFT;
        case SPOSE_THROWN_UP_LEFT_SOFT:
            SpriteUtilThrownUpLeftSoft();
            break;

        case SPOSE_THROWN_UP_LEFT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_LEFT_HARD;
        case SPOSE_THROWN_UP_LEFT_HARD:
            SpriteUtilThrownUpLeftHard();
            break;

        case SPOSE_THROWN_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_SOFT;
        case SPOSE_THROWN_RIGHT_SOFT:
            SpriteUtilThrownRightSoft();
            break;

        case SPOSE_THROWN_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_RIGHT_HARD;
        case SPOSE_THROWN_RIGHT_HARD:
            SpriteUtilThrownRightHard();
            break;

        case SPOSE_THROWN_UP_RIGHT_SOFT_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_SOFT;
        case SPOSE_THROWN_UP_RIGHT_SOFT:
            SpriteUtilThrownUpRightSoft();
            break;

        case SPOSE_THROWN_UP_RIGHT_HARD_INIT:
            gCurrentSprite.pose = SPOSE_THROWN_UP_RIGHT_HARD;
        case SPOSE_THROWN_UP_RIGHT_HARD:
            SpriteUtilThrownUpRightHard();
            break;

        case SPOSE_69:
            SpearMaskPose69();
            break;

        case SPOSE_6A:
            SpearMaskPose6A();
            break;

        case SPEAR_MASK_POSE_6E:
            SpearMaskPose6E();
        case SPEAR_MASK_POSE_6F:
            SpearMaskPose6F();
            break;

        case SPEAR_MASK_POSE_70:
            SpearMaskPose70();
        case SPEAR_MASK_POSE_71:
            SpearMaskPose71();
            break;

        case SPEAR_MASK_STARTING_CHASE:
            SpearMaskStartChase();
            break;

        case SPEAR_MASK_CHASING:
            SpearMaskWalkBlueOrRed();
            break;

        default:
            SpearMaskInstantlyKill();
            break;
    }
}

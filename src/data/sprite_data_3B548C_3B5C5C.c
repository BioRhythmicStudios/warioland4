#include "oam.h"

/*
 * Shared sprite data region: 0x083B548C - 0x083B5C5C
 *
 * METHOD 3 MAINTENANCE RULES
 * --------------------------
 * This file owns one contiguous ROM interval shared by multiple sprite modules.
 * Keep every definition in ascending ROM-address order.
 *
 * When merging another sprite_ai branch:
 *   1. Locate each new OAM frame/table's original ROM address.
 *   2. Find the raw sSpriteDataRaw_* or sUnk_* array covering that address.
 *   3. Split that raw array and replace only the verified byte interval with
 *      const u16 OAM frames and const struct AnimationFrame tables.
 *   4. Preserve all bytes before and after the decoded interval.
 *   5. Update tools/sprite_data_regions.json.
 *   6. Rebuild immediately and verify MD5 5fe47355a33e3fabec2a1607af88a404.
 *
 * Never append a definition merely to group it with its sprite. Never add
 * OAM_DATA_SECTION, per-address linker sections, or asm/generated_oam files.
 */

/* 0x083B548C: decoded OAM frame. */
const u16 sSwitchIdleOam_Frame1[] = {
    4,
    /* 40D1 81F4 5139 */
    OAM_ENTRY(-12, -47, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B54A6: decoded OAM frame. */
const u16 sEscapeSwitchPressedOam_Frame2[] = {
    4,
    /* 40D3 81F4 5139 */
    OAM_ENTRY(-12, -45, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B54C0: decoded OAM frame. */
const u16 sEscapeSwitchPressedOam_Frame3[] = {
    4,
    /* 40D8 81F4 5139 */
    OAM_ENTRY(-12, -40, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B54DA: decoded OAM frame. */
const u16 sEscapeSwitchPressedOam_Frame4[] = {
    4,
    /* 40D7 81F4 5139 */
    OAM_ENTRY(-12, -41, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B54F4: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame10[] = {
    5,
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 00EB 41F9 51B6 */
    OAM_ENTRY(-7, -21, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B5514: decoded OAM frame. */
const u16 sEscapeSwitchIdleOam_Frame1[] = {
    4,
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
};
/* 0x083B552E: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame2[] = {
    4,
    /* 40D6 81F4 5139 */
    OAM_ENTRY(-12, -42, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DE 81EC 511C */
    OAM_ENTRY(-20, -34, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EE 41EC 515C */
    OAM_ENTRY(-20, -18, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
};
/* 0x083B5548: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame3[] = {
    4,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D5 81F4 5139 */
    OAM_ENTRY(-12, -43, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DD 81EC 511C */
    OAM_ENTRY(-20, -35, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40ED 41EC 515C */
    OAM_ENTRY(-20, -19, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B5562: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame6[] = {
    4,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D4 81F4 5139 */
    OAM_ENTRY(-12, -44, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DC 81EC 511C */
    OAM_ENTRY(-20, -36, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EC 41EC 515C */
    OAM_ENTRY(-20, -20, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B557C: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame5[] = {
    4,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D3 81F4 5139 */
    OAM_ENTRY(-12, -45, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DB 81EC 511C */
    OAM_ENTRY(-20, -37, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EB 41EC 515C */
    OAM_ENTRY(-20, -21, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B5596: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame4[] = {
    4,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D2 81F4 5139 */
    OAM_ENTRY(-12, -46, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DA 81EC 511C */
    OAM_ENTRY(-20, -38, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EA 41EC 515C */
    OAM_ENTRY(-20, -22, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B55B0: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame3[] = {
    4,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D1 81F4 5139 */
    OAM_ENTRY(-12, -47, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D9 81EC 511C */
    OAM_ENTRY(-20, -39, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E9 41EC 515C */
    OAM_ENTRY(-20, -23, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B55CA: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame2[] = {
    4,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D0 81F4 5139 */
    OAM_ENTRY(-12, -48, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D8 81EC 511C */
    OAM_ENTRY(-20, -40, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E8 41EC 515C */
    OAM_ENTRY(-20, -24, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B55E4: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame1[] = {
    4,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B55FE: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame4[] = {
    5,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D4 81F4 5139 */
    OAM_ENTRY(-12, -44, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DC 81EC 511C */
    OAM_ENTRY(-20, -36, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EC 41EC 515C */
    OAM_ENTRY(-20, -20, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 00F0 41F9 51B6 */
    OAM_ENTRY(-7, -16, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B561E: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame5[] = {
    5,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D3 81F4 5139 */
    OAM_ENTRY(-12, -45, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DB 81EC 511C */
    OAM_ENTRY(-20, -37, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EB 41EC 515C */
    OAM_ENTRY(-20, -21, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 00F0 41F9 51B6 */
    OAM_ENTRY(-7, -16, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B563E: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame6[] = {
    5,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D2 81F4 5139 */
    OAM_ENTRY(-12, -46, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DA 81EC 511C */
    OAM_ENTRY(-20, -38, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EA 41EC 515C */
    OAM_ENTRY(-20, -22, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 00EF 41F9 51B6 */
    OAM_ENTRY(-7, -17, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B565E: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame7[] = {
    5,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D1 81F4 5139 */
    OAM_ENTRY(-12, -47, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D9 81EC 511C */
    OAM_ENTRY(-20, -39, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E9 41EC 515C */
    OAM_ENTRY(-20, -23, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 00EE 41F9 51B6 */
    OAM_ENTRY(-7, -18, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B567E: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame8[] = {
    5,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40D0 81F4 5139 */
    OAM_ENTRY(-12, -48, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D8 81EC 511C */
    OAM_ENTRY(-20, -40, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E8 41EC 515C */
    OAM_ENTRY(-20, -24, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 00ED 41F9 51B6 */
    OAM_ENTRY(-7, -19, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B569E: decoded OAM frame. */
const u16 sSwitchReleasedOam_Frame9[] = {
    5,
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 00ED 41F9 51B6 */
    OAM_ENTRY(-7, -19, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B56BE: decoded OAM frame. */
const u16 sSwitchPressedOam_Frame1[] = {
    6,
    /* 40D5 01FA 51F7 */
    OAM_ENTRY(-6, -43, SPRITE_SIZE_16x8, 0, 503, 5, 0),
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 00EB 41FA 51B6 */
    OAM_ENTRY(-6, -21, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B56E4: decoded OAM frame. */
const u16 sSwitchPressedOam_Frame2[] = {
    6,
    /* 40D5 01FA 51F9 */
    OAM_ENTRY(-6, -43, SPRITE_SIZE_16x8, 0, 505, 5, 0),
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 00EA 41F9 51B6 */
    OAM_ENTRY(-7, -22, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B570A: decoded OAM frame. */
const u16 sSwitchPressedOam_Frame3[] = {
    6,
    /* 00EA 0006 40A8 */
    OAM_ENTRY(6, -22, SPRITE_SIZE_8x8, 0, 168, 4, 0),
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 00EB 41F8 51B6 */
    OAM_ENTRY(-8, -21, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B5730: decoded OAM frame. */
const u16 sSwitchPressedOam_Frame4[] = {
    6,
    /* 00EA 0006 40A7 */
    OAM_ENTRY(6, -22, SPRITE_SIZE_8x8, 0, 167, 4, 0),
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 00E9 41F9 51B6 */
    OAM_ENTRY(-7, -23, SPRITE_SIZE_16x16, 0, 438, 5, 0),
};
/* 0x083B5756: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame1[] = {
    1,
    /* 00EC 0006 40A7 */
    OAM_ENTRY(6, -20, SPRITE_SIZE_8x8, 0, 167, 4, 0),
};
/* 0x083B575E: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame2[] = {
    1,
    /* 00EB 0006 40A8 */
    OAM_ENTRY(6, -21, SPRITE_SIZE_8x8, 0, 168, 4, 0),
};
/* 0x083B5766: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame3[] = {
    4,
    /* 00E6 0002 40A7 */
    OAM_ENTRY(2, -26, SPRITE_SIZE_8x8, 0, 167, 4, 0),
    /* 00EE 0001 40A7 */
    OAM_ENTRY(1, -18, SPRITE_SIZE_8x8, 0, 167, 4, 0),
    /* 00E9 0008 40A7 */
    OAM_ENTRY(8, -23, SPRITE_SIZE_8x8, 0, 167, 4, 0),
    /* 00F1 0008 40A7 */
    OAM_ENTRY(8, -15, SPRITE_SIZE_8x8, 0, 167, 4, 0),
};
/* 0x083B5780: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame4[] = {
    1,
    /* 00ED 000C 40A7 */
    OAM_ENTRY(12, -19, SPRITE_SIZE_8x8, 0, 167, 4, 0),
};
/* 0x083B5788: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame5[] = {
    4,
    /* 00E9 0008 40A7 */
    OAM_ENTRY(8, -23, SPRITE_SIZE_8x8, 0, 167, 4, 0),
    /* 00F3 0006 40A7 */
    OAM_ENTRY(6, -13, SPRITE_SIZE_8x8, 0, 167, 4, 0),
    /* 00EB 0011 40A7 */
    OAM_ENTRY(17, -21, SPRITE_SIZE_8x8, 0, 167, 4, 0),
    /* 00F4 000E 40A7 */
    OAM_ENTRY(14, -12, SPRITE_SIZE_8x8, 0, 167, 4, 0),
};
/* 0x083B57A2: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame6[] = {
    1,
    /* 00F1 0008 40A7 */
    OAM_ENTRY(8, -15, SPRITE_SIZE_8x8, 0, 167, 4, 0),
};
/* 0x083B57AA: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame7[] = {
    1,
    /* 00F1 0009 40A8 */
    OAM_ENTRY(9, -15, SPRITE_SIZE_8x8, 0, 168, 4, 0),
};
/* 0x083B57B2: decoded OAM frame. */
const u16 sSwitchEffectIdleOam_Frame8[] = {
    4,
    /* 00E8 1009 40A7 */
    OAM_ENTRY(9, -24, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 167, 4, 0),
    /* 00F2 1011 40A7 */
    OAM_ENTRY(17, -14, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 167, 4, 0),
    /* 00EE 1001 40A7 */
    OAM_ENTRY(1, -18, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 167, 4, 0),
    /* 00F8 1009 40A7 */
    OAM_ENTRY(9, -8, SPRITE_SIZE_8x8, ST_OAM_HFLIP, 167, 4, 0),
};
/* 0x083B57CC - 0x083B57F0: not yet typed. */
const u8 sSpriteDataRaw_083B57CC[] = {
    0x02, 0x00, 0xF8, 0x00, 0xF3, 0x41, 0x7C, 0x51, 0xF8, 0x00, 0xFC, 0x51, 0x7C, 0x51, 0x02, 0x00, 0xF8, 0x00,
    0xF0, 0x41, 0x7C, 0x51, 0xF8, 0x00, 0x00, 0x50, 0x7C, 0x51, 0x01, 0x00, 0xFD, 0x00, 0xFC, 0x01, 0x83, 0x51,
};
/* 0x083B57F0: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame10[] = {
    4,
    /* 40D6 81F4 5139 */
    OAM_ENTRY(-12, -42, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B580A: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame11[] = {
    4,
    /* 40D5 81F4 5139 */
    OAM_ENTRY(-12, -43, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B5824: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame12[] = {
    4,
    /* 40D2 81F4 5139 */
    OAM_ENTRY(-12, -46, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B583E: decoded OAM frame. */
const u16 sEscapeSwitchWaitingOam_Frame13[] = {
    4,
    /* 40D0 81F4 5139 */
    OAM_ENTRY(-12, -48, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B5858 - 0x083B5872: not yet typed. */
const u8 sSpriteDataRaw_083B5858[] = {
    0x04, 0x00, 0xF0, 0x40, 0xEC, 0x81, 0x63, 0x51, 0xC9, 0x40, 0xF4, 0x81, 0x39,
    0x51, 0xD7, 0x40, 0xEC, 0x81, 0x1C, 0x51, 0xE7, 0x40, 0xEC, 0x41, 0x5C, 0x51,
};
/* 0x083B5872: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame2[] = {
    7,
    /* 40E0 81EF 60CC */
    OAM_ENTRY(-17, -32, SPRITE_SIZE_32x16, 0, 204, 6, 0),
    /* 40F0 81EF 60D0 */
    OAM_ENTRY(-17, -16, SPRITE_SIZE_32x16, 0, 208, 6, 0),
    /* 40D3 01F8 51FE */
    OAM_ENTRY(-8, -45, SPRITE_SIZE_16x8, 0, 510, 5, 0),
    /* 40CF 81F4 5139 */
    OAM_ENTRY(-12, -49, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D7 81EC 511C */
    OAM_ENTRY(-20, -41, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E7 41EC 515C */
    OAM_ENTRY(-20, -25, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F0 81EC 5163 */
    OAM_ENTRY(-20, -16, SPRITE_SIZE_32x16, 0, 355, 5, 0),
};
/* 0x083B589E: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame3[] = {
    8,
    /* 00E3 41F4 60D4 */
    OAM_ENTRY(-12, -29, SPRITE_SIZE_16x16, 0, 212, 6, 0),
    /* 80E3 0004 60D6 */
    OAM_ENTRY(4, -29, SPRITE_SIZE_8x16, 0, 214, 6, 0),
    /* 40F3 41EC 609A */
    OAM_ENTRY(-20, -13, SPRITE_SIZE_32x8, 0, 154, 6, 0),
    /* 40D5 01F8 51FE */
    OAM_ENTRY(-8, -43, SPRITE_SIZE_16x8, 0, 510, 5, 0),
    /* 40D1 81F4 5139 */
    OAM_ENTRY(-12, -47, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40D9 81EC 511C */
    OAM_ENTRY(-20, -39, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40E9 41EC 515C */
    OAM_ENTRY(-20, -23, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F2 81EC 5163 */
    OAM_ENTRY(-20, -14, SPRITE_SIZE_32x16, 0, 355, 5, 0),
};
/* 0x083B58D0: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame4[] = {
    6,
    /* 00E8 41F6 60D7 */
    OAM_ENTRY(-10, -24, SPRITE_SIZE_16x16, 0, 215, 6, 0),
    /* 40D7 01F8 51FE */
    OAM_ENTRY(-8, -41, SPRITE_SIZE_16x8, 0, 510, 5, 0),
    /* 40D3 81F4 5139 */
    OAM_ENTRY(-12, -45, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DB 81EC 511C */
    OAM_ENTRY(-20, -37, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EB 41EC 515C */
    OAM_ENTRY(-20, -21, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 40F2 81EC 5163 */
    OAM_ENTRY(-20, -14, SPRITE_SIZE_32x16, 0, 355, 5, 0),
};
/* 0x083B58F6: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame5[] = {
    6,
    /* 40E3 81EF 60CC */
    OAM_ENTRY(-17, -29, SPRITE_SIZE_32x16, 0, 204, 6, 0),
    /* 40F3 81EF 60D0 */
    OAM_ENTRY(-17, -13, SPRITE_SIZE_32x16, 0, 208, 6, 0),
    /* 40D9 01F8 51FE */
    OAM_ENTRY(-8, -39, SPRITE_SIZE_16x8, 0, 510, 5, 0),
    /* 40D5 81F4 5139 */
    OAM_ENTRY(-12, -43, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DD 81EC 511C */
    OAM_ENTRY(-20, -35, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40ED 41EC 515C */
    OAM_ENTRY(-20, -19, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B591C: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame6[] = {
    7,
    /* 00E8 41F4 60D4 */
    OAM_ENTRY(-12, -24, SPRITE_SIZE_16x16, 0, 212, 6, 0),
    /* 80E8 0004 60D6 */
    OAM_ENTRY(4, -24, SPRITE_SIZE_8x16, 0, 214, 6, 0),
    /* 40F8 41EC 609A */
    OAM_ENTRY(-20, -8, SPRITE_SIZE_32x8, 0, 154, 6, 0),
    /* 40DB 01F8 51FE */
    OAM_ENTRY(-8, -37, SPRITE_SIZE_16x8, 0, 510, 5, 0),
    /* 40D7 81F4 5139 */
    OAM_ENTRY(-12, -41, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40DF 81EC 511C */
    OAM_ENTRY(-20, -33, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40EF 41EC 515C */
    OAM_ENTRY(-20, -17, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B5948: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame7[] = {
    6,
    /* 00E6 41F3 60D7 */
    OAM_ENTRY(-13, -26, SPRITE_SIZE_16x16, 0, 215, 6, 0),
    /* 00ED 41FA 60D7 */
    OAM_ENTRY(-6, -19, SPRITE_SIZE_16x16, 0, 215, 6, 0),
    /* 40DE 01F8 51FE */
    OAM_ENTRY(-8, -34, SPRITE_SIZE_16x8, 0, 510, 5, 0),
    /* 40DA 81F4 5139 */
    OAM_ENTRY(-12, -38, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40E2 81EC 511C */
    OAM_ENTRY(-20, -30, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40F2 41EC 515C */
    OAM_ENTRY(-20, -14, SPRITE_SIZE_32x8, 0, 348, 5, 0),
};
/* 0x083B596E: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame8[] = {
    7,
    /* 40DE 81EF 60CC */
    OAM_ENTRY(-17, -34, SPRITE_SIZE_32x16, 0, 204, 6, 0),
    /* 40EE 81EF 60D0 */
    OAM_ENTRY(-17, -18, SPRITE_SIZE_32x16, 0, 208, 6, 0),
    /* 40E1 01F8 51FE */
    OAM_ENTRY(-8, -31, SPRITE_SIZE_16x8, 0, 510, 5, 0),
    /* 40DD 81F4 5139 */
    OAM_ENTRY(-12, -35, SPRITE_SIZE_32x16, 0, 313, 5, 0),
    /* 40E5 81EC 511C */
    OAM_ENTRY(-20, -27, SPRITE_SIZE_32x16, 0, 284, 5, 0),
    /* 40F5 41EC 515C */
    OAM_ENTRY(-20, -11, SPRITE_SIZE_32x8, 0, 348, 5, 0),
    /* 00D2 51F6 4098 */
    OAM_ENTRY(-10, -46, SPRITE_SIZE_16x16, ST_OAM_HFLIP, 152, 4, 0),
};
/* 0x083B599A: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame9[] = {
    4,
    /* 00E8 41F4 60D4 */
    OAM_ENTRY(-12, -24, SPRITE_SIZE_16x16, 0, 212, 6, 0),
    /* 80E8 0004 60D6 */
    OAM_ENTRY(4, -24, SPRITE_SIZE_8x16, 0, 214, 6, 0),
    /* 40F8 41EC 609A */
    OAM_ENTRY(-20, -8, SPRITE_SIZE_32x8, 0, 154, 6, 0),
    /* 00D4 41FB 4098 */
    OAM_ENTRY(-5, -44, SPRITE_SIZE_16x16, 0, 152, 4, 0),
};
/* 0x083B59B4: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame10[] = {
    3,
    /* 00EC 41F7 60D7 */
    OAM_ENTRY(-9, -20, SPRITE_SIZE_16x16, 0, 215, 6, 0),
    /* 00DB 01F8 40BA */
    OAM_ENTRY(-8, -37, SPRITE_SIZE_8x8, 0, 186, 4, 0),
    /* 00D7 0003 40BA */
    OAM_ENTRY(3, -41, SPRITE_SIZE_8x8, 0, 186, 4, 0),
};
/* 0x083B59C8: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame11[] = {
    3,
    /* 00F2 01FC 40D9 */
    OAM_ENTRY(-4, -14, SPRITE_SIZE_8x8, 0, 217, 4, 0),
    /* 80D2 2007 4099 */
    OAM_ENTRY(7, -46, SPRITE_SIZE_8x16, ST_OAM_VFLIP, 153, 4, 0),
    /* 80D4 11F8 4099 */
    OAM_ENTRY(-8, -44, SPRITE_SIZE_8x16, ST_OAM_HFLIP, 153, 4, 0),
};
/* 0x083B59DC: decoded OAM frame. */
const u16 sSwitchEffectActiveOam_Frame12[] = {
    2,
    /* 00D8 01FA 4099 */
    OAM_ENTRY(-6, -40, SPRITE_SIZE_8x8, 0, 153, 4, 0),
    /* 00D3 0007 4099 */
    OAM_ENTRY(7, -45, SPRITE_SIZE_8x8, 0, 153, 4, 0),
};
/* 0x083B59EA - 0x083B59EC: not yet typed. */
const u8 sSpriteDataRaw_083B59EA[] = {
    0x00,
    0x00,
};
/* 0x083B59EC: Match_disasm_switch. */
const struct AnimationFrame sSwitchIdleOam[] = {{sSwitchIdleOam_Frame1, 200}, ANIMATION_TERMINATOR};
/* 0x083B59FC: Match_disasm_switch. */
const struct AnimationFrame sEscapeSwitchPressedOam[] = {
    {sSwitchIdleOam_Frame1, 1},
    {sEscapeSwitchPressedOam_Frame2, 4},
    {sEscapeSwitchPressedOam_Frame3, 4},
    {sEscapeSwitchPressedOam_Frame4, 4},
    ANIMATION_TERMINATOR
};
/* 0x083B5A24: Match_disasm_switch. */
const struct AnimationFrame sEscapeSwitchActiveOam[] = {{sEscapeSwitchPressedOam_Frame4, 200}, ANIMATION_TERMINATOR};
/* 0x083B5A34: Match_disasm_switch. */
const struct AnimationFrame sSwitchReleasedOam[] = {
    {sEscapeSwitchPressedOam_Frame4, 1},
    {sSwitchReleasedOam_Frame2, 4},
    {sSwitchReleasedOam_Frame3, 4},
    {sSwitchReleasedOam_Frame4, 4},
    {sSwitchReleasedOam_Frame5, 4},
    {sSwitchReleasedOam_Frame6, 4},
    {sSwitchReleasedOam_Frame7, 4},
    {sSwitchReleasedOam_Frame8, 4},
    {sSwitchReleasedOam_Frame9, 4},
    {sSwitchReleasedOam_Frame10, 1},
    ANIMATION_TERMINATOR
};
/* 0x083B5A8C: Match_disasm_switch. */
const struct AnimationFrame sSwitchPressedOam[] = {
    {sSwitchPressedOam_Frame1, 7},
    {sSwitchPressedOam_Frame2, 7},
    {sSwitchPressedOam_Frame3, 7},
    {sSwitchPressedOam_Frame4, 7},
    ANIMATION_TERMINATOR
};
/* 0x083B5AB4: Match_disasm_switch. */
const struct AnimationFrame sSwitchEffectIdleOam[] = {
    {sSwitchEffectIdleOam_Frame1, 5},
    {sSwitchEffectIdleOam_Frame2, 5},
    {sSwitchEffectIdleOam_Frame3, 5},
    {sSwitchEffectIdleOam_Frame4, 5},
    {sSwitchEffectIdleOam_Frame5, 5},
    {sSwitchEffectIdleOam_Frame6, 5},
    {sSwitchEffectIdleOam_Frame7, 5},
    {sSwitchEffectIdleOam_Frame8, 5},
    ANIMATION_TERMINATOR
};
/* 0x083B5AFC - 0x083B5B1C: not yet typed. */
const u8 sSpriteDataRaw_083B5AFC[] = {
    0xCC, 0x57, 0x3B, 0x08, 0x05, 0x00, 0x00, 0x00, 0xDA, 0x57, 0x3B, 0x08, 0x05, 0x00, 0x00, 0x00,
    0xE8, 0x57, 0x3B, 0x08, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/* 0x083B5B1C: Match_disasm_switch. */
const struct AnimationFrame sEscapeSwitchWaitingOam[] = {
    {sEscapeSwitchWaitingOam_Frame1, 19},
    {sEscapeSwitchWaitingOam_Frame2, 12},
    {sEscapeSwitchWaitingOam_Frame3, 12},
    {sEscapeSwitchWaitingOam_Frame4, 12},
    {sEscapeSwitchWaitingOam_Frame5, 12},
    {sEscapeSwitchWaitingOam_Frame6, 12},
    {sSwitchReleasedOam_Frame3, 12},
    {sSwitchReleasedOam_Frame2, 12},
    {sEscapeSwitchPressedOam_Frame4, 15},
    {sEscapeSwitchWaitingOam_Frame10, 2},
    {sEscapeSwitchWaitingOam_Frame11, 3},
    {sEscapeSwitchWaitingOam_Frame12, 3},
    {sEscapeSwitchWaitingOam_Frame13, 3},
    {sSwitchIdleOam_Frame1, 1},
    ANIMATION_TERMINATOR
};
/* 0x083B5B94 - 0x083B5BE4: not yet typed. */
const u8 sSpriteDataRaw_083B5B94[] = {
    0xDA, 0x54, 0x3B, 0x08, 0x01, 0x00, 0x00, 0x00, 0x2E, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00,
    0x48, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00, 0x62, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00,
    0x7C, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00, 0x96, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00,
    0xB0, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00, 0xCA, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00,
    0xE4, 0x55, 0x3B, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/* 0x083B5BE4: Match_disasm_switch. */
const struct AnimationFrame sEscapeSwitchIdleOam[] = {{sEscapeSwitchIdleOam_Frame1, 200}, ANIMATION_TERMINATOR};
/* 0x083B5BF4: Match_disasm_switch. */
const struct AnimationFrame sSwitchEffectActiveOam[] = {
    {sEscapeSwitchIdleOam_Frame1, 1},
    {sSwitchEffectActiveOam_Frame2, 3},
    {sSwitchEffectActiveOam_Frame3, 3},
    {sSwitchEffectActiveOam_Frame4, 3},
    {sSwitchEffectActiveOam_Frame5, 3},
    {sSwitchEffectActiveOam_Frame6, 3},
    {sSwitchEffectActiveOam_Frame7, 3},
    {sSwitchEffectActiveOam_Frame8, 3},
    {sSwitchEffectActiveOam_Frame9, 3},
    {sSwitchEffectActiveOam_Frame10, 3},
    {sSwitchEffectActiveOam_Frame11, 3},
    {sSwitchEffectActiveOam_Frame12, 3},
    ANIMATION_TERMINATOR
};

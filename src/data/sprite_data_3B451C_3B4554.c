#include "oam.h"

/*
 * Shared sprite data region: 0x083B451C - 0x083B4554
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

/* 0x083B451C: decoded OAM frame. */
const u16 sRotatingPlatformOam_83B4534_Frame1[] = {
    1,
    /* 00E0 C1E0 8200 */
    OAM_ENTRY(-32, -32, SPRITE_SIZE_64x64, 0, 512, 8, 0),
};
/* 0x083B4524: decoded OAM frame. */
const u16 sRotatingPlatformOam_83B4544_Frame1[] = {
    2,
    /* 40FA 41F0 8249 */
    OAM_ENTRY(-16, -6, SPRITE_SIZE_32x8, 0, 585, 8, 0),
    /* 4002 01F8 826A */
    OAM_ENTRY(-8, 2, SPRITE_SIZE_16x8, 0, 618, 8, 0),
};
/* 0x083B4532 - 0x083B4534: not yet typed. */
const u8 sSpriteDataRaw_083B4532[] = {
    0x00,
    0x00,
};
/* 0x083B4534: Match_disasm_rotating_platform. */
const struct AnimationFrame sRotatingPlatformOam_83B4534[] = {
    {sRotatingPlatformOam_83B4534_Frame1, 200},
    ANIMATION_TERMINATOR
};
/* 0x083B4544: Match_disasm_rotating_platform. */
const struct AnimationFrame sRotatingPlatformOam_83B4544[] = {
    {sRotatingPlatformOam_83B4544_Frame1, 200},
    ANIMATION_TERMINATOR
};

#ifndef OAM_H
#define OAM_H

#include "types.h"

#define MAX_OAM_SLOTS 128

struct AGB_ALIGN AnimationFrame {
    GBA_PTR(const u16*) oam;
    u8 time;
};

// ????
struct AGB_ALIGN AnimationFrameU16 {
    GBA_PTR(const u16*) oam;
    u16 time;
};

#define OAM_ENTRY(x, y, dimensions, flip, tile, palette, priority)                              \
    SPRITE_SHAPE(dimensions) << 14 | ST_OAM_4BPP << 12 | ST_OAM_AFFINE_OFF << 8 | ((y) & 0xFF), \
    SPRITE_SIZE(dimensions) << 14 | (flip) << 9 | ((x) & 0x1FF),                                \
    ((palette) << 12) | ((priority) << 10) | (tile)

#define ANIMATION_TERMINATOR { NULL, 0 }

extern OamData gOamBuffer[MAX_OAM_SLOTS];
extern u8 gOamSlotsUsed;

#endif  // OAM_H

#ifndef GUARD_PORT_PPU_H
#define GUARD_PORT_PPU_H

#include <stdint.h>

// The GBA's display: draws each visible line from the registers, VRAM, palette RAM and OAM
// at the start of its HBlank, before the HBlank DMA transfers, as the hardware does. That
// keeps the game's per-line effects (HBlank DMA, HBlank and VCount interrupts) working.
//
// It covers modes 0-5, text and affine backgrounds, regular and affine sprites, the
// windows (including the OBJ window), blending, brightness and mosaic. Where the hardware is
// unclear, it does what mGBA 0.10.5's software renderer does, which the port is checked
// against (port/tools/diff_screens.py).

#define PORT_SCREEN_WIDTH 240
#define PORT_SCREEN_HEIGHT 160

// How blended colours are computed. The GBA blends 5-bit channels; mGBA blends 8-bit ones
// and rounds differently, so its blended pixels can be one step off. PORT_PPU_COLORS_MGBA
// reproduces mGBA exactly, for comparing screenshots.
enum PortPpuColors {
    PORT_PPU_COLORS_GBA,
    PORT_PPU_COLORS_MGBA,
};

// Puts the display in its power-on state, with the given colour maths.
void PortPpuReset(enum PortPpuColors colors);

// Called at the start of each line's HBlank (0-227): draws lines 0-159, and finishes the
// frame at line 160.
void PortPpuHBlank(unsigned int line);

// The picture, row by row: 32-bit pixels with red in the low byte, then green and blue
// (SDL_PIXELFORMAT_XBGR8888). The GBA's 5-bit channels c become (c << 3) | (c >> 2).
// Complete at the start of VBlank.
const uint32_t *PortPpuFrame(void);

#endif  // GUARD_PORT_PPU_H

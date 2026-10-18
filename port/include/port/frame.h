#ifndef GUARD_PORT_FRAME_H
#define GUARD_PORT_FRAME_H

#include <stdio.h>

// The frame loop. The game runs on its coroutine until it halts (BIOS Halt), as it does once
// a frame while it waits for its VBlank handler. While the game is halted, the host moves the
// display on, one HBlank or scanline at a time, until an interrupt that IE enables is
// requested. Then the game resumes: Halt runs the interrupt handler and returns.
//
// The game's code takes no time: it runs where it halted, and an interrupt raised while it
// runs (by polling VCOUNT, say) is handled at its next Halt.

// Sets the game's entry point (AgbMain) and has port/src/ppu.c draw each line. Returns
// nonzero on failure.
int PortFrameStart(void (*entry)(void));

// Runs the game up to the start of the next VBlank, before the VBlank interrupt is handled.
// Returns 0 once the game has stopped.
int PortFrameRun(void);

// How many frames PortFrameRun() has completed.
unsigned int PortFrameCount(void);

// How many steps of the game's main loop have finished, as seen at the start of each VBlank:
// gMainTimer has changed and the loop waits for VBlank (bit 0 of gInterruptCheck is clear).
// On the GBA a step can take several frames; counted this way, the port and mGBA agree.
// port/tools/diff_frames.py and port/tools/mgba_dump.lua count them the same way.
unsigned int PortFrameSteps(void);

// BIOS Halt, on the game coroutine.
void PortFrameHalt(void);

// Interrupt callbacks observe the current display event; their register reads are not polls.
int PortFrameInInterrupt(void);

// Appends GBA memory to a frame dump: a struct PortFrameDumpHeader, then EWRAM, IWRAM, the
// I/O registers, palette RAM, VRAM, OAM and SRAM. port/tools/mgba_dump.lua writes the same
// format from mGBA, for port/tools/diff_frames.py. Returns nonzero on a write error.
#define PORT_FRAME_DUMP_MAGIC 0x46344C57u  // "WL4F"
struct PortFrameDumpHeader {
    unsigned int magic;
    unsigned int frame;
};
int PortFrameDump(FILE *file);

// Appends the picture to a screenshot file: a header as above, with its own magic, then
// 240x160 pixels, each red, green and blue bytes, row by row, for port/tools/diff_screens.py.
#define PORT_FRAME_SHOT_MAGIC 0x53344C57u  // "WL4S"
int PortFrameShot(FILE *file);

#endif  // GUARD_PORT_FRAME_H

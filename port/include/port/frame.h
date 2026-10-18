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

// Sets the game's entry point (AgbMain). Returns nonzero on failure.
int PortFrameStart(void (*entry)(void));

// Runs the game up to the start of the next VBlank, before the VBlank interrupt is handled.
// Returns 0 once the game has stopped.
int PortFrameRun(void);

// How many frames PortFrameRun() has completed.
unsigned int PortFrameCount(void);

// BIOS Halt, on the game coroutine.
void PortFrameHalt(void);

// Appends GBA memory to a frame dump: a struct PortFrameDumpHeader, then EWRAM, IWRAM, the
// I/O registers, palette RAM, VRAM, OAM and SRAM. port/tools/mgba_dump.lua writes the same
// format from mGBA, for port/tools/diff_frames.py. Returns nonzero on a write error.
#define PORT_FRAME_DUMP_MAGIC 0x46344C57u  // "WL4F"
struct PortFrameDumpHeader {
    unsigned int magic;
    unsigned int frame;
};
int PortFrameDump(FILE *file);

#endif  // GUARD_PORT_FRAME_H

#ifndef GUARD_PORT_INPUT_SCRIPT_H
#define GUARD_PORT_INPUT_SCRIPT_H

#include <stdint.h>

// Scripted input for headless runs, keyed by steps of the game's main loop (PortFrameSteps())
// rather than frames, so that the port and mGBA (port/tools/mgba_dump.lua reads the same
// files) press the same keys on the same steps although their frames differ.
//
// A text file of lines "STEP KEYS": from main-loop step STEP on, the game reads KEYS, until
// the next line. KEYS are button names joined by '+' (A B SELECT START RIGHT LEFT UP DOWN R
// L), or '-' for none. Steps count from 1 and must increase. '#' starts a comment.
//
//     # press Start for one step at the title, then A on the file select screen
//     1800 START
//     1801 -
//     1900 A
//     1901 -

// Reads a script. Returns nonzero, having said why on stderr, if it cannot.
int PortInputScriptLoad(const char *path);

// The buttons the script holds on main-loop step `step`, as KEYINPUT bits set for pressed.
uint16_t PortInputScriptKeys(unsigned int step);

#endif  // GUARD_PORT_INPUT_SCRIPT_H

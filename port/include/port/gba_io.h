#ifndef GUARD_PORT_GBA_IO_H
#define GUARD_PORT_GBA_IO_H

#include <stdint.h>

// The GBA's I/O registers. Game code reaches them through every kind of pointer (the REG_
// macros, structs, addresses computed at run time), so instead of changing it, the port traps
// each access to the register page (port/src/os_win32.c) and gives it the hardware's effects:
// - DMA: a write that enables a channel latches its registers. An immediate transfer runs at
//   once and clears the enable bit; VBlank and HBlank transfers run from PortIoVBlank() and
//   PortIoHBlank(), and repeat if asked to. Sound FIFO (special) transfers are left alone.
// - IF: writing 1 to a bit clears it.
// - VCOUNT and DISPSTAT: each read by the game moves the display on by a scanline, so that
//   loops waiting for a scanline end. The host sets the line with PortIoSetScanline().
// - Writes to read-only registers (VCOUNT, DISPSTAT's status bits, KEYINPUT) are undone.
// The host uses PortIoHost(), which never traps.

#define PORT_IO_SIZE 0x400u

// Maps the register page at GBA_IO_BASE with trapped accesses. Returns 0 on success.
int PortIoMap(void);

// The host's view of the registers: PORT_IO_SIZE bytes, read and written without traps.
uint8_t *PortIoHost(void);

static inline uint16_t PortIoRead16(unsigned int offset)
{
    const uint8_t *io = PortIoHost();
    return (uint16_t)(io[offset] | io[offset + 1] << 8);
}

static inline void PortIoWrite16(unsigned int offset, uint16_t value)
{
    uint8_t *io = PortIoHost();
    io[offset] = (uint8_t)value;
    io[offset + 1] = (uint8_t)(value >> 8);
}

// Puts the display at the start of a scanline (0-227) and updates VCOUNT and DISPSTAT.
void PortIoSetScanline(unsigned int line);

// Sets interrupt request bits in IF.
void PortIoRaiseInterrupt(uint16_t flags);

// Run the enabled DMA transfers timed to the start of VBlank, or to an HBlank.
void PortIoVBlank(void);
void PortIoHBlank(void);

// How many game accesses have been trapped, for tests and profiling.
uint64_t PortIoTrapCount(void);

// Runs the I/O unit tests (--test-io). Returns nonzero on failure.
int PortIoTest(void);

#endif  // GUARD_PORT_GBA_IO_H

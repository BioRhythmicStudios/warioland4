#ifndef GUARD_PORT_GBA_MEMORY_H
#define GUARD_PORT_GBA_MEMORY_H

#include <stddef.h>
#include <stdint.h>

// The GBA address space, mapped at the real addresses so that game pointers work unchanged.
// Each region is reserved in whole 64 KiB blocks (the Windows allocation granularity);
// only the part the hardware has is accessible, so stray accesses fault.
#define GBA_EWRAM_BASE 0x02000000u
#define GBA_EWRAM_SIZE 0x40000u
#define GBA_IWRAM_BASE 0x03000000u
#define GBA_IWRAM_SIZE 0x8000u
#define GBA_IO_BASE 0x04000000u
#define GBA_IO_SIZE 0x400u
#define GBA_PLTT_BASE 0x05000000u
#define GBA_PLTT_SIZE 0x400u
#define GBA_VRAM_BASE 0x06000000u
#define GBA_VRAM_SIZE 0x18000u
#define GBA_OAM_BASE 0x07000000u
#define GBA_OAM_SIZE 0x400u
#define GBA_ROM_BASE 0x08000000u
#define GBA_ROM_SIZE 0x800000u      // Wario Land 4 is 8 MiB
#define GBA_ROM_MAX_SIZE 0x2000000u // the cartridge address space
#define GBA_SRAM_BASE 0x0E000000u
#define GBA_SRAM_SIZE 0x10000u

// The game's call stack: below EWRAM, so that game code may keep stack addresses in
// 32-bit variables and compare them as signed numbers. The coroutine running the game
// uses [PORT_GAME_STACK_LIMIT, PORT_GAME_STACK_TOP). Below the limit, a guard page and
// 60 KiB of headroom let the fault handler report an overflow, and an inaccessible block
// catches anything beyond.
#define PORT_GAME_STACK_RESERVE_BASE 0x01000000u
#define PORT_GAME_STACK_HEADROOM_BASE 0x01010000u
#define PORT_GAME_STACK_LIMIT 0x01020000u
#define PORT_GAME_STACK_TOP 0x01810000u

// Reserves and maps all of the above. On failure, prints which range is unavailable.
int PortReserveGbaMemory(void);

// Makes the ROM read-only, once it is loaded.
int PortProtectRom(void);

// Names the GBA region holding an address, for fault reports ("ROM", "IWRAM", ...), or NULL.
const char *PortGbaRegionName(uintptr_t address);

// Installs the handler that reports faults on GBA addresses and the game stack.
void PortInstallFaultHandler(void);

#endif  // GUARD_PORT_GBA_MEMORY_H

#ifndef GUARD_PORT_X86_ACCESS_H
#define GUARD_PORT_X86_ACCESS_H

#include <stdint.h>

// The size in bytes of the memory operand of the x86-64 instruction at code: 1, 2, 4, 8 or
// 16, for the stores and read-modify-write instructions compilers emit. 0 for one it does
// not recognise. The trapped I/O registers use it to tell which bytes a store wrote, since a
// write-only register bit may be written with the value it already reads as.
unsigned int PortX86AccessSize(const uint8_t *code);

// For an instruction that accessed memory it cannot: the register to move its address by, so
// that it can run again on other memory, and how far the address moves when that register
// moves by 1 (*scale). *reloads is set if the instruction loads that register itself, which
// must then not be moved back. Returns the register (0 = RAX ... 15 = R15), or -1 for an
// instruction this does not work for: only loads, stores, arithmetic and compares whose
// address comes from a register they use for nothing else.
int PortX86AddressRegister(const uint8_t *code, unsigned int *scale, int *reloads);

#endif  // GUARD_PORT_X86_ACCESS_H

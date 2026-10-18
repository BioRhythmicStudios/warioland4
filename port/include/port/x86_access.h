#ifndef GUARD_PORT_X86_ACCESS_H
#define GUARD_PORT_X86_ACCESS_H

#include <stdint.h>

// The size in bytes of the memory operand of the x86-64 instruction at code: 1, 2, 4, 8 or
// 16, for the stores and read-modify-write instructions compilers emit. 0 for one it does
// not recognise. The trapped I/O registers use it to tell which bytes a store wrote, since a
// write-only register bit may be written with the value it already reads as.
unsigned int PortX86AccessSize(const uint8_t *code);

#endif  // GUARD_PORT_X86_ACCESS_H

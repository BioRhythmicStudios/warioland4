#ifndef GUARD_PORT_OS_H
#define GUARD_PORT_OS_H

#include <stddef.h>
#include <stdint.h>

// The operating system services the port runtime needs. port/src/os_win32.c implements them.

enum PortProtection {
    PORT_NO_ACCESS,
    PORT_READ_ONLY,
    PORT_READ_WRITE,
    PORT_READ_WRITE_GUARD,  // read/write, faults once on first touch (the stack guard page)
};

// Reserves [address, address + size) at exactly that address, inaccessible.
// Returns 0 on success.
int PortOsReserveFixed(uintptr_t address, size_t size);

// Changes the protection of reserved pages, committing them if needed. Returns 0 on success.
int PortOsProtect(uintptr_t address, size_t size, enum PortProtection protection);

// Describes what occupies an address, for error messages.
void PortOsDescribeAddress(uintptr_t address, char *buffer, size_t size);

enum PortFaultKind {
    PORT_FAULT_READ,
    PORT_FAULT_WRITE,
    PORT_FAULT_EXECUTE,
    PORT_FAULT_STACK_OVERFLOW,
};

// Called on the faulting thread for every access violation and stack overflow, before
// the process dies. It must use little stack: a stack overflow leaves 60 KiB.
typedef void (*PortFaultCallback)(enum PortFaultKind kind, uintptr_t address, uintptr_t pc, uintptr_t sp);
void PortOsInstallFaultHandler(PortFaultCallback callback);

// Writes a message to stderr without the C library, for use inside the fault handler.
void PortOsWriteError(const char *message);

#endif  // GUARD_PORT_OS_H

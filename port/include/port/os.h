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

// A region whose game view traps every access, for the GBA's I/O registers. The game view is
// mapped at `address` (`reserve` bytes, of which the first `size` are accessible through
// traps). Each access by any thread first calls before(), then runs the instruction with the
// page open, then calls after(). The host reads and writes the same memory freely through
// *hostView. Returns 0 on success.
// pc is the address of the trapped instruction.
typedef void (*PortTrapCallback)(uintptr_t address, int isWrite, uintptr_t pc);
int PortOsMapTrappedRegion(uintptr_t address, size_t reserve, size_t size, uint8_t **hostView,
                           PortTrapCallback before, PortTrapCallback after);

// Accesses to 32-bit addresses that are not mapped, for the GBA addresses the port does not
// map (the BIOS, which Windows cannot map, unused addresses and mirrors): redirect(address)
// returns the host address the access goes to instead, or 0 to let it fault. The faulting
// instruction runs again with the register its address comes from moved by the difference
// (port/include/port/x86_access.h); one whose address cannot be moved so faults as before.
// redirect runs in the exception handler: it must not be instrumented, nor call what is.
typedef uintptr_t (*PortRedirectCallback)(uintptr_t address);
void PortOsRedirectFaults(PortRedirectCallback redirect);

// Writes a message to stderr without the C library, for use inside the fault handler. After
// PortOsShowErrors(1) it also shows the message in a dialog and waits for it to be closed, for
// windowed runs, which may have no console.
void PortOsWriteError(const char *message);
void PortOsShowErrors(int show);

// The offset of a code address in the executable, for llvm-symbolizer; 0 if it is elsewhere.
uintptr_t PortOsExecutableOffset(uintptr_t pc);

#endif  // GUARD_PORT_OS_H

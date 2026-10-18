#include "port/os.h"

#include <stdio.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int PortOsReserveFixed(uintptr_t address, size_t size)
{
    void *p = VirtualAlloc((void *)address, size, MEM_RESERVE, PAGE_NOACCESS);

    if (p == (void *)address)
        return 0;
    if (p != NULL)
        VirtualFree(p, 0, MEM_RELEASE);
    return -1;
}

int PortOsProtect(uintptr_t address, size_t size, enum PortProtection protection)
{
    static const DWORD kFlags[] = {
        [PORT_NO_ACCESS] = PAGE_NOACCESS,
        [PORT_READ_ONLY] = PAGE_READONLY,
        [PORT_READ_WRITE] = PAGE_READWRITE,
        [PORT_READ_WRITE_GUARD] = PAGE_READWRITE | PAGE_GUARD,
    };
    DWORD old;

    if (protection == PORT_NO_ACCESS)
        return VirtualFree((void *)address, size, MEM_DECOMMIT) ? 0 : -1;
    if (VirtualAlloc((void *)address, size, MEM_COMMIT, kFlags[protection]) == NULL)
        return -1;
    return VirtualProtect((void *)address, size, kFlags[protection], &old) ? 0 : -1;
}

void PortOsDescribeAddress(uintptr_t address, char *buffer, size_t size)
{
    MEMORY_BASIC_INFORMATION info;
    char module[MAX_PATH] = "";

    if (VirtualQuery((void *)address, &info, sizeof(info)) == 0) {
        snprintf(buffer, size, "VirtualQuery failed (error %lu)", GetLastError());
        return;
    }
    if (info.State == MEM_FREE) {
        snprintf(buffer, size, "the range is free, but VirtualAlloc failed (error %lu)", GetLastError());
        return;
    }
    if (info.Type == MEM_IMAGE)
        GetModuleFileNameA((HMODULE)info.AllocationBase, module, sizeof(module));
    snprintf(buffer, size, "already in use from %p (%s%s%s)", info.AllocationBase,
             info.Type == MEM_IMAGE ? "module " : info.Type == MEM_MAPPED ? "mapped file" : "private memory",
             module, info.State == MEM_RESERVE ? ", reserved" : "");
}

void PortOsWriteError(const char *message)
{
    DWORD written;

    WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)strlen(message), &written, NULL);
}

// --- Trapped region ---------------------------------------------------------------------------
// The game view stays PAGE_NOACCESS. An access faults; the handler calls before(), opens the
// page, sets the trap flag and lets the instruction run. The single-step exception that
// follows calls after() and closes the page again.

#define TRAP_FLAG 0x100

static uintptr_t sTrapBase;
static size_t sTrapSize;
static PortTrapCallback sTrapBefore, sTrapAfter;
static _Thread_local int sTrapPending;
static _Thread_local uintptr_t sTrapAddress;
static _Thread_local int sTrapWrite;

// Not instrumented: AddressSanitizer commits its shadow memory from access violations too, and
// this handler sees them first. An instrumented access here would fault again, endlessly.
__attribute__((no_sanitize("address"))) static LONG CALLBACK TrapHandler(EXCEPTION_POINTERS *info)
{
    const EXCEPTION_RECORD *record = info->ExceptionRecord;
    CONTEXT *context = info->ContextRecord;
    DWORD old;

    if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
        uintptr_t address = record->ExceptionInformation[1];
        if (address - sTrapBase >= sTrapSize || sTrapPending)
            return EXCEPTION_CONTINUE_SEARCH;
        sTrapAddress = address;
        sTrapWrite = record->ExceptionInformation[0] == 1;
        sTrapBefore(address, sTrapWrite);
        if (!VirtualProtect((void *)sTrapBase, sTrapSize, PAGE_READWRITE, &old))
            return EXCEPTION_CONTINUE_SEARCH;
        sTrapPending = 1;
        context->EFlags |= TRAP_FLAG;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    if (record->ExceptionCode == EXCEPTION_SINGLE_STEP && sTrapPending) {
        sTrapPending = 0;
        context->EFlags &= ~TRAP_FLAG;
        VirtualProtect((void *)sTrapBase, sTrapSize, PAGE_NOACCESS, &old);
        sTrapAfter(sTrapAddress, sTrapWrite);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int PortOsMapTrappedRegion(uintptr_t address, size_t reserve, size_t size, uint8_t **hostView,
                           PortTrapCallback before, PortTrapCallback after)
{
    HANDLE section = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)reserve, NULL);
    void *game, *host;
    DWORD old;

    if (section == NULL)
        return -1;
    game = MapViewOfFileEx(section, FILE_MAP_ALL_ACCESS, 0, 0, reserve, (void *)address);
    host = MapViewOfFile(section, FILE_MAP_ALL_ACCESS, 0, 0, reserve);
    CloseHandle(section);  // the views keep the section alive
    if (game != (void *)address || host == NULL || !VirtualProtect(game, reserve, PAGE_NOACCESS, &old))
        return -1;
    sTrapBase = address;
    sTrapSize = size;
    sTrapBefore = before;
    sTrapAfter = after;
    *hostView = host;
    // First in line: these faults are expected, and are resolved before anyone reports them.
    AddVectoredExceptionHandler(1, TrapHandler);
    return 0;
}

static PortFaultCallback sFaultCallback;

static LONG CALLBACK VectoredHandler(EXCEPTION_POINTERS *info)
{
    const EXCEPTION_RECORD *record = info->ExceptionRecord;
    const CONTEXT *context = info->ContextRecord;

    if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
        ULONG_PTR access = record->ExceptionInformation[0];
        enum PortFaultKind kind = access == 1 ? PORT_FAULT_WRITE : access == 8 ? PORT_FAULT_EXECUTE : PORT_FAULT_READ;
        sFaultCallback(kind, record->ExceptionInformation[1], context->Rip, context->Rsp);
    } else if (record->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
        sFaultCallback(PORT_FAULT_STACK_OVERFLOW, 0, context->Rip, context->Rsp);
    }
    // Report only: the debugger, AddressSanitizer or the default handler takes it from here.
    return EXCEPTION_CONTINUE_SEARCH;
}

void PortOsInstallFaultHandler(PortFaultCallback callback)
{
    sFaultCallback = callback;
    // Called last, after handlers that resolve faults themselves (AddressSanitizer's).
    AddVectoredExceptionHandler(0, VectoredHandler);
}

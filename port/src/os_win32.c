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

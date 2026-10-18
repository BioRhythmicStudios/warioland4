#include "port/os.h"

#include "port/x86_access.h"

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

static int sShowErrors;

void PortOsShowErrors(int show)
{
    sShowErrors = show;
}

static DWORD WINAPI ErrorDialogThread(LPVOID message)
{
    MessageBoxA(NULL, (const char *)message, "Wario Land 4", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    return 0;
}

void PortOsWriteError(const char *message)
{
    DWORD written;

    WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)strlen(message), &written, NULL);
    if (sShowErrors) {
        // From a thread of its own: a dialog on this thread would run the game window's
        // message handling while the game is stopped in a fault.
        HANDLE thread = CreateThread(NULL, 0, ErrorDialogThread, (LPVOID)message, 0, NULL);
        if (thread != NULL) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
        }
    }
}

uintptr_t PortOsExecutableOffset(uintptr_t pc)
{
    HMODULE module;

    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCSTR)pc, &module)
        || module != GetModuleHandleA(NULL))
        return 0;
    return pc - (uintptr_t)module;
}

// --- Files ------------------------------------------------------------------------------------

// A path as UTF-16: from UTF-8 if it is valid UTF-8, else from the ANSI code page.
static int WidePath(const char *path, const char *suffix, wchar_t *wide, int capacity)
{
    char joined[MAX_PATH * 3];
    int n;

    if (snprintf(joined, sizeof(joined), "%s%s", path, suffix) >= (int)sizeof(joined))
        return 0;
    n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, joined, -1, wide, capacity);
    if (n == 0)
        n = MultiByteToWideChar(CP_ACP, 0, joined, -1, wide, capacity);
    return n != 0;
}

long PortOsReadFile(const char *path, void *buffer, size_t size, int *missing)
{
    wchar_t wide[MAX_PATH * 2];
    HANDLE file;
    DWORD read = 0;
    BOOL ok;

    *missing = 0;
    if (!WidePath(path, "", wide, MAX_PATH * 2))
        return -1;
    file = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        *missing = error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
        return -1;
    }
    ok = ReadFile(file, buffer, (DWORD)size, &read, NULL);
    CloseHandle(file);
    return ok ? (long)read : -1;
}

int PortOsWriteFileAtomic(const char *path, const void *data, size_t size, char *error, size_t errorSize)
{
    static unsigned int sequence;
    wchar_t wide[MAX_PATH * 2], temporary[MAX_PATH * 2];
    char suffix[64];
    HANDLE file = INVALID_HANDLE_VALUE;
    DWORD written = 0, failure = ERROR_FILENAME_EXCED_RANGE;
    unsigned int attempt;
    BOOL ok;
    int created = 0;

    if (!WidePath(path, "", wide, MAX_PATH * 2))
        goto failed;
    // Never truncate an existing .tmp or another process's in-progress save.
    for (attempt = 0; attempt < 16; attempt++) {
        snprintf(suffix, sizeof(suffix), ".tmp.%lu.%u", (unsigned long)GetCurrentProcessId(), ++sequence);
        if (!WidePath(path, suffix, temporary, MAX_PATH * 2))
            goto failed;
        file = CreateFileW(temporary, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE)
            break;
        failure = GetLastError();
        if (failure != ERROR_FILE_EXISTS && failure != ERROR_ALREADY_EXISTS)
            goto failed;
    }
    if (file == INVALID_HANDLE_VALUE)
        goto failed;
    created = 1;
    ok = WriteFile(file, data, (DWORD)size, &written, NULL);
    failure = ok ? ERROR_WRITE_FAULT : GetLastError();
    if (ok && written == size) {
        ok = FlushFileBuffers(file);
        if (!ok)
            failure = GetLastError();
    } else {
        ok = FALSE;
    }
    if (!CloseHandle(file)) {
        if (ok)
            failure = GetLastError();
        ok = FALSE;
    }
    if (!ok)
        goto failed;
    if (!MoveFileExW(temporary, wide, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        failure = GetLastError();
        goto failed;
    }
    return 0;

failed:
    if (created)
        DeleteFileW(temporary);  // only the temporary file this call exclusively created
    if (errorSize) {
        wchar_t detail[256];
        char utf8[768];
        if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                           failure, 0, detail, 256, NULL)
            && WideCharToMultiByte(CP_UTF8, 0, detail, -1, utf8, sizeof(utf8), NULL, NULL))
            snprintf(error, errorSize, "Windows error %lu: %s", (unsigned long)failure, utf8);
        else
            snprintf(error, errorSize, "Windows error %lu.", (unsigned long)failure);
    }
    return -1;
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
static _Thread_local uintptr_t sTrapPc;

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
        sTrapPc = context->Rip;
        sTrapBefore(address, sTrapWrite, sTrapPc);
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
        sTrapAfter(sTrapAddress, sTrapWrite, sTrapPc);
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

// --- Redirected faults ------------------------------------------------------------------------
// The handler moves the instruction's address register to where redirect() says and sets the
// trap flag; the single-step exception that follows moves the register back.

static PortRedirectCallback sRedirect;
static _Thread_local int sRedirectPending;
static _Thread_local int sRedirectRegister;
static _Thread_local int sRedirectReloads;
static _Thread_local DWORD64 sRedirectDelta;

__attribute__((no_sanitize("address"))) static LONG CALLBACK RedirectHandler(EXCEPTION_POINTERS *info)
{
    const EXCEPTION_RECORD *record = info->ExceptionRecord;
    CONTEXT *context = info->ContextRecord;
    DWORD64 *registers = &context->Rax;  // RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, R8-R15

    if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
        uintptr_t address = record->ExceptionInformation[1], host;
        unsigned int scale;
        int reg;

        if (address > UINT32_MAX || sRedirectPending || sRedirect == NULL)
            return EXCEPTION_CONTINUE_SEARCH;
        reg = PortX86AddressRegister((const uint8_t *)context->Rip, &scale, &sRedirectReloads);
        if (reg < 0 || (host = sRedirect(address)) == 0 || (host - address) % scale != 0)
            return EXCEPTION_CONTINUE_SEARCH;
        sRedirectRegister = reg;
        sRedirectDelta = (DWORD64)(int64_t)((intptr_t)(host - address) / (intptr_t)scale);
        registers[reg] += sRedirectDelta;
        sRedirectPending = 1;
        context->EFlags |= TRAP_FLAG;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    if (record->ExceptionCode == EXCEPTION_SINGLE_STEP && sRedirectPending) {
        sRedirectPending = 0;
        context->EFlags &= ~TRAP_FLAG;
        if (!sRedirectReloads)
            registers[sRedirectRegister] -= sRedirectDelta;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void PortOsRedirectFaults(PortRedirectCallback redirect)
{
    sRedirect = redirect;
    // First in line, as the trapped region's handler: these faults are resolved here.
    AddVectoredExceptionHandler(1, RedirectHandler);
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

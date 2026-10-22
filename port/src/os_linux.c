// Linux x86-64 backend. All GBA execution and trapped accesses stay on one thread.
#define _GNU_SOURCE
#include "port/os.h"
#include "port/x86_access.h"
#include "port/gba_memory.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ucontext.h>
#include <unistd.h>

#if !defined(__linux__) || !defined(__x86_64__)
#error This backend requires Linux x86-64
#endif

int PortOsReserveFixed(uintptr_t address, size_t size)
{
    void *p = mmap((void *)address, size, PROT_NONE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (p == (void *)address) return 0;
    if (p != MAP_FAILED) munmap(p, size);
    return -1;
}

int PortOsProtect(uintptr_t address, size_t size, enum PortProtection protection)
{
    // A persistent inaccessible guard stops overflow before the 60 KiB headroom.
    static const int flags[] = {PROT_NONE, PROT_READ, PROT_READ | PROT_WRITE, PROT_NONE};
    return mprotect((void *)address, size, flags[protection]);
}

void PortOsDescribeAddress(uintptr_t address, char *buffer, size_t size)
{
    int reason = errno;
    (void)address;
    snprintf(buffer, size, "fixed mmap failed: %s", strerror(reason));
}
void PortOsShowErrors(int show) { (void)show; } // Terminal diagnostics on Linux.
void PortOsWriteError(const char *message)
{
    size_t remaining = strlen(message);
    while (remaining) {
        ssize_t n = write(STDERR_FILENO, message, remaining);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) break;
        message += n; remaining -= (size_t)n;
    }
}
uintptr_t PortOsExecutableOffset(uintptr_t pc)
{
    // ET_EXEC: symbolizers take the linked virtual address, without image-base subtraction.
    extern char __executable_start, _end;
    return pc >= (uintptr_t)&__executable_start && pc < (uintptr_t)&_end ? pc : 0;
}

#define TRAP_FLAG 0x100
static uintptr_t sTrapBase;
static size_t sTrapSize;
static PortTrapCallback sBefore, sAfter;
static PortRedirectCallback sRedirect;
static PortFaultCallback sFault;
static struct sigaction sPreviousSegv, sPreviousTrap, sPreviousFpe;
static int sInstalled;
static _Thread_local int sTrapPending, sTrapWrite, sRedirectPending, sRedirectRegister, sReloads;
static _Thread_local uintptr_t sTrapAddress, sTrapPc, sDelta;
// ModRM register numbers differ from Linux's gregset_t order.
static const int sRegisters[] = {REG_RAX, REG_RCX, REG_RDX, REG_RBX, REG_RSP, REG_RBP, REG_RSI, REG_RDI,
                               REG_R8, REG_R9, REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_R15};

__attribute__((no_sanitize("address", "undefined")))
static void SignalHandler(int sig, siginfo_t *info, void *context)
{
    int savedErrno = errno;
    ucontext_t *uc = context;
    greg_t *r = uc->uc_mcontext.gregs;
    uintptr_t address = (uintptr_t)info->si_addr, pc = r[REG_RIP];
    int writeAccess = !!(r[REG_ERR] & 2);
    if (sig == SIGTRAP && info->si_code == TRAP_TRACE && (sTrapPending || sRedirectPending)) {
        r[REG_EFL] &= ~TRAP_FLAG;
        if (sTrapPending) {
            sTrapPending = 0;
            if (mprotect((void *)sTrapBase, 4096, PROT_NONE)) _exit(127);
            sAfter(sTrapAddress, sTrapWrite, sTrapPc);
        }
        if (sRedirectPending) {
            sRedirectPending = 0;
            if (!sReloads) r[sRegisters[sRedirectRegister]] -= sDelta;
        }
        errno = savedErrno;
        return;
    }
    if (sig == SIGSEGV && info->si_code > 0 && r[REG_TRAPNO] == 14 && !(r[REG_ERR] & 16)) {
        if (sTrapSize && address - sTrapBase < sTrapSize && !sTrapPending) {
            sTrapAddress = address; sTrapWrite = writeAccess; sTrapPc = pc;
            sBefore(address, writeAccess, pc);
            if (mprotect((void *)sTrapBase, 4096, PROT_READ | PROT_WRITE)) _exit(127);
            sTrapPending = 1; r[REG_EFL] |= TRAP_FLAG;
            errno = savedErrno;
            return;
        }
        if (address <= UINT32_MAX && sRedirect && !sRedirectPending) {
            unsigned scale;
            int reg = PortX86AddressRegister((const uint8_t *)pc, &scale, &sReloads);
            uintptr_t host;
            if (reg >= 0 && (host = sRedirect(address)) && (host - address) % scale == 0) {
                sRedirectRegister = reg;
                sDelta = (uintptr_t)((intptr_t)(host - address) / (intptr_t)scale);
                r[sRegisters[reg]] += sDelta;
                sRedirectPending = 1; r[REG_EFL] |= TRAP_FLAG;
                errno = savedErrno;
                return;
            }
        }
    }
    if (sig == SIGSEGV && sFault) {
        enum PortFaultKind kind = r[REG_ERR] & 16 ? PORT_FAULT_EXECUTE : writeAccess ? PORT_FAULT_WRITE : PORT_FAULT_READ;
        if (address >= PORT_GAME_STACK_RESERVE_BASE && address < PORT_GAME_STACK_LIMIT + 4096)
            kind = PORT_FAULT_STACK_OVERFLOW;
        sFault(kind, address, pc, r[REG_RSP]);
    }
    // Preserve sanitizer/debugger handlers for faults we do not own.
    struct sigaction *previous = sig == SIGSEGV ? &sPreviousSegv : sig == SIGFPE ? &sPreviousFpe : &sPreviousTrap;
    if (previous->sa_handler != SIG_DFL && previous->sa_handler != SIG_IGN) {
        if (previous->sa_flags & SA_SIGINFO) previous->sa_sigaction(sig, info, context);
        else previous->sa_handler(sig);
    } else {
        // A real fault is about to terminate the process. Leave its PC in the
        // diagnostic log even when the host has no core-dump debugger.
        char report[192];
        snprintf(report, sizeof(report),
                 "Fatal Linux signal %d (code %d): address=%p pc=%p sp=%p\n",
                 sig, info->si_code, info->si_addr, (void *)pc, (void *)r[REG_RSP]);
        PortOsWriteError(report);
        struct sigaction action = {.sa_handler = SIG_DFL};
        sigemptyset(&action.sa_mask);
        sigaction(sig, &action, NULL);
        raise(sig);
    }
    errno = savedErrno;
}

static void InstallHandlers(void)
{
    if (sInstalled) return;
    stack_t stack;
    if (sigaltstack(NULL, &stack)) { perror("sigaltstack"); exit(1); }
    if (stack.ss_flags & SS_DISABLE) {
        stack.ss_size = 128 * 1024;
        stack.ss_sp = mmap(NULL, stack.ss_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        stack.ss_flags = 0;
        if (stack.ss_sp == MAP_FAILED || sigaltstack(&stack, NULL)) { perror("signal stack"); exit(1); }
    }
    struct sigaction action = {.sa_sigaction = SignalHandler, .sa_flags = SA_SIGINFO | SA_ONSTACK};
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGSEGV, &action, &sPreviousSegv) || sigaction(SIGTRAP, &action, &sPreviousTrap) ||
        sigaction(SIGFPE, &action, &sPreviousFpe)) {
        perror("sigaction"); exit(1);
    }
    sInstalled = 1;
}
void PortOsInstallFaultHandler(PortFaultCallback callback) { sFault = callback; InstallHandlers(); }
void PortOsRedirectFaults(PortRedirectCallback redirect) { sRedirect = redirect; InstallHandlers(); }

int PortOsMapTrappedRegion(uintptr_t address, size_t reserve, size_t size, uint8_t **hostView,
                           PortTrapCallback before, PortTrapCallback after)
{
    int fd = memfd_create("wl4-io", MFD_CLOEXEC);
    void *game, *host;
    if (fd < 0) return -1;
    if (ftruncate(fd, (off_t)reserve)) { close(fd); return -1; }
    game = mmap((void *)address, reserve, PROT_NONE, MAP_SHARED | MAP_FIXED_NOREPLACE, fd, 0);
    host = mmap(NULL, reserve, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (game != (void *)address || host == MAP_FAILED) {
        if (game != MAP_FAILED) munmap(game, reserve);
        if (host != MAP_FAILED) munmap(host, reserve);
        return -1;
    }
    sTrapBase = address; sTrapSize = size; sBefore = before; sAfter = after;
    *hostView = host;
    InstallHandlers();
    return 0;
}

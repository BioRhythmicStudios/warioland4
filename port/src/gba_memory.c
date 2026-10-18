#include "port/gba_memory.h"

#include <stdio.h>
#include <string.h>

#include "port/gba_functions.h"
#include "port/gba_io.h"
#include "port/os.h"

#define BLOCK 0x10000u  // Windows allocation granularity
#define PAGE 0x1000u
#define ROUND_UP(x, n) (((x) + (n) - 1) & ~((n) - 1))

struct Region {
    const char *name;
    uintptr_t base;
    size_t size;  // the accessible part; the reservation is rounded up to whole blocks
    size_t reserve;
};

static const struct Region sRegions[] = {
    { "EWRAM", GBA_EWRAM_BASE, GBA_EWRAM_SIZE, GBA_EWRAM_SIZE },
    { "IWRAM", GBA_IWRAM_BASE, GBA_IWRAM_SIZE, BLOCK },
    { "I/O registers", GBA_IO_BASE, GBA_IO_SIZE, BLOCK },
    { "palette RAM", GBA_PLTT_BASE, GBA_PLTT_SIZE, BLOCK },
    { "VRAM", GBA_VRAM_BASE, GBA_VRAM_SIZE, 0x20000 },
    { "OAM", GBA_OAM_BASE, GBA_OAM_SIZE, BLOCK },
    { "ROM", GBA_ROM_BASE, GBA_ROM_SIZE, GBA_ROM_MAX_SIZE },
    { "SRAM", GBA_SRAM_BASE, GBA_SRAM_SIZE, GBA_SRAM_SIZE },
};

// The stack reservation runs from the inaccessible block at the bottom to a block above the top.
#define STACK_RESERVE_SIZE (PORT_GAME_STACK_TOP + BLOCK - PORT_GAME_STACK_RESERVE_BASE)

static int Reserve(const char *name, uintptr_t base, size_t size)
{
    char what[160];

    if (PortOsReserveFixed(base, size) == 0)
        return 0;
    PortOsDescribeAddress(base, what, sizeof(what));
    fprintf(stderr, "Cannot reserve the %s at 0x%08llX-0x%08llX: %s\n", name, (unsigned long long)base,
            (unsigned long long)(base + size - 1), what);
    return -1;
}

static int Protect(const char *name, uintptr_t base, size_t size, enum PortProtection protection)
{
    if (PortOsProtect(base, size, protection) == 0)
        return 0;
    fprintf(stderr, "Cannot map the %s at 0x%08llX\n", name, (unsigned long long)base);
    return -1;
}

int PortReserveGbaMemory(void)
{
    unsigned int i;

    if (Reserve("game stack", PORT_GAME_STACK_RESERVE_BASE, STACK_RESERVE_SIZE)
        || Protect("game stack", PORT_GAME_STACK_HEADROOM_BASE, PORT_GAME_STACK_TOP - PORT_GAME_STACK_HEADROOM_BASE,
                   PORT_READ_WRITE)
        || Protect("game stack guard page", PORT_GAME_STACK_LIMIT, PAGE, PORT_READ_WRITE_GUARD))
        return -1;
    for (i = 0; i < sizeof(sRegions) / sizeof(sRegions[0]); i++) {
        const struct Region *r = &sRegions[i];
        if (r->base == GBA_IO_BASE) {
            // Every access to the registers is trapped, to give them their hardware effects.
            if (PortIoMap()) {
                fprintf(stderr, "Cannot map the I/O registers at 0x%08X\n", GBA_IO_BASE);
                return -1;
            }
            continue;
        }
        if (Reserve(r->name, r->base, r->reserve) || Protect(r->name, r->base, ROUND_UP(r->size, PAGE), PORT_READ_WRITE))
            return -1;
    }
    // A cartridge with no save yet: erased SRAM reads 0xFF, as in mGBA.
    memset((void *)(uintptr_t)GBA_SRAM_BASE, 0xFF, GBA_SRAM_SIZE);
    return 0;
}

int PortProtectRom(void)
{
    return Protect("ROM", GBA_ROM_BASE, GBA_ROM_SIZE, PORT_READ_ONLY);
}

const char *PortGbaRegionName(uintptr_t address)
{
    unsigned int i;

    if (address < 0x4000)
        return "BIOS";
    if (address >= PORT_GAME_STACK_RESERVE_BASE && address < PORT_GAME_STACK_HEADROOM_BASE)
        return "game stack (below its overflow headroom)";
    if (address >= PORT_GAME_STACK_HEADROOM_BASE && address < PORT_GAME_STACK_LIMIT)
        return "game stack (overflow headroom)";
    if (address >= PORT_GAME_STACK_LIMIT && address < PORT_GAME_STACK_TOP)
        return "game stack";
    if (address >= PORT_GAME_STACK_TOP && address < PORT_GAME_STACK_RESERVE_BASE + STACK_RESERVE_SIZE)
        return "game stack (above its top)";
    for (i = 0; i < sizeof(sRegions) / sizeof(sRegions[0]); i++) {
        const struct Region *r = &sRegions[i];
        if (address >= r->base && address < r->base + r->reserve)
            return address < r->base + r->size ? r->name : "past the end of a GBA region";
    }
    if (address < 0x10000000)
        return "unmapped GBA address";
    return NULL;
}

// Appends " (executable+0x<offset>), in or after <game function>+0x<offset> (GBA 0x<address>)"
// for a native pc. The game function is the nearest one below the pc in the dispatch table;
// llvm-symbolizer on the executable offset gives the exact function.
static void AppendGameFunction(char *message, size_t size, uintptr_t pc)
{
    const struct GbaFunction *f = PortGbaFunctionBelow(pc);
    uintptr_t offset = PortOsExecutableOffset(pc);
    size_t len = strlen(message);

    if (len == 0 || len >= size)
        return;
    len--;  // overwrite the newline
    if (offset != 0 && len < size)
        len += snprintf(message + len, size - len, " (executable+0x%llX)", (unsigned long long)offset);
    if (f != NULL && len < size)
        len += snprintf(message + len, size - len, ", in or after %s+0x%llX (GBA 0x%08X)", f->name,
                        (unsigned long long)(pc - (uintptr_t)f->func), f->gbaAddress);
    if (len < size)
        snprintf(message + len, size - len, "\n");
}

static void ReportFault(enum PortFaultKind kind, uintptr_t address, uintptr_t pc, uintptr_t sp)
{
    // Static, so that a report from an overflowed stack fits in the headroom.
    static char message[384];
    static const char *const kAccess[] = { "read from", "write to", "execute at" };
    const char *region = PortGbaRegionName(address);

    if (kind == PORT_FAULT_STACK_OVERFLOW) {
        if (sp >= PORT_GAME_STACK_RESERVE_BASE && sp < PORT_GAME_STACK_TOP)
            snprintf(message, sizeof(message), "Game stack overflow (more than %u KiB) at pc %p, sp 0x%08llX\n",
                     (PORT_GAME_STACK_TOP - PORT_GAME_STACK_LIMIT) / 1024, (void *)pc, (unsigned long long)sp);
        else
            return;
    } else if (region != NULL) {
        snprintf(message, sizeof(message), "GBA memory fault: %s 0x%08llX (%s%s) at pc %p\n", kAccess[kind],
                 (unsigned long long)address, region,
                 kind == PORT_FAULT_WRITE && address >= GBA_ROM_BASE && address < GBA_ROM_BASE + GBA_ROM_SIZE
                     ? ", read-only"
                     : "",
                 (void *)pc);
    } else if (PortGbaFunctionBelow(pc) != NULL) {
        snprintf(message, sizeof(message), "Memory fault: %s %p at pc %p\n", kAccess[kind], (void *)address,
                 (void *)pc);
    } else {
        return;
    }
    AppendGameFunction(message, sizeof(message), pc);
    PortOsWriteError(message);
}

void PortInstallFaultHandler(void)
{
    PortOsInstallFaultHandler(ReportFault);
}

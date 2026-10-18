#include "port/gba_io.h"

#include <stdio.h>
#include <string.h>

#include "port/gba_memory.h"
#include "port/os.h"

#define TRAP_RESERVE 0x10000u  // a whole Windows allocation block
#define TRAP_PAGE 0x1000u      // the page that holds the registers

#define REG_DISPSTAT 0x004
#define REG_VCOUNT 0x006
#define REG_SOUND1CNT_X 0x064
#define REG_SOUND2CNT_H 0x06C
#define REG_SOUND3CNT_X 0x074
#define REG_SOUND4CNT_H 0x07C
#define REG_SOUNDCNT_H 0x082
#define REG_DMA0SAD 0x0B0
#define DMA_STRIDE 12
#define REG_KEYINPUT 0x130
#define REG_IE 0x200
#define REG_IF 0x202

#define DISPSTAT_VBLANK 0x0001
#define DISPSTAT_HBLANK 0x0002
#define DISPSTAT_VCOUNT 0x0004
#define DISPSTAT_STATUS (DISPSTAT_VBLANK | DISPSTAT_HBLANK | DISPSTAT_VCOUNT)
#define DISPSTAT_VBLANK_IRQ 0x0008
#define DISPSTAT_HBLANK_IRQ 0x0010
#define DISPSTAT_VCOUNT_IRQ 0x0020

#define INTR_VBLANK 0x0001
#define INTR_HBLANK 0x0002
#define INTR_VCOUNT 0x0004

#define DMA_DEST_MODE(c) (((c) >> 5) & 3)
#define DMA_SRC_MODE(c) (((c) >> 7) & 3)
#define DMA_REPEAT 0x0200
#define DMA_32BIT 0x0400
#define DMA_TIMING(c) (((c) >> 12) & 3)
#define DMA_ENABLE 0x8000
#define DMA_IMMEDIATE 0
#define DMA_AT_VBLANK 1
#define DMA_AT_HBLANK 2

#define LINES 228
#define VISIBLE_LINES 160
#define VBLANK_FIRST_LINE 160
#define VBLANK_LAST_LINE 226  // DISPSTAT's VBlank flag is clear on the last line

static uint8_t *sIo;
static uint8_t sBefore[PORT_IO_SIZE];  // the registers before the trapped instruction ran
static unsigned int sLine;
static int sInHBlank;
static uint64_t sTraps;
static uintptr_t sTrapPc;  // the game instruction being trapped

struct DmaChannel {
    int active;
    uint32_t src, dst, count;
    uint16_t control;
};
static struct DmaChannel sDma[4];

uint8_t *PortIoHost(void)
{
    return sIo;
}

uint64_t PortIoTrapCount(void)
{
    return sTraps;
}

static uint32_t Read32(unsigned int offset)
{
    return PortIoRead16(offset) | (uint32_t)PortIoRead16(offset + 2) << 16;
}

static uint16_t Before16(unsigned int offset)
{
    return (uint16_t)(sBefore[offset] | sBefore[offset + 1] << 8);
}

static void UpdateDisplayStatus(void)
{
    uint16_t status = PortIoRead16(REG_DISPSTAT) & ~DISPSTAT_STATUS;

    if (sLine >= VBLANK_FIRST_LINE && sLine <= VBLANK_LAST_LINE)
        status |= DISPSTAT_VBLANK;
    if (sInHBlank)
        status |= DISPSTAT_HBLANK;
    if (sLine == (unsigned int)(status >> 8))
        status |= DISPSTAT_VCOUNT;
    PortIoWrite16(REG_DISPSTAT, status);
    PortIoWrite16(REG_VCOUNT, (uint16_t)sLine);
}

void PortIoSetScanline(unsigned int line)
{
    sLine = line % LINES;
    sInHBlank = 0;
    UpdateDisplayStatus();
}

void PortIoRaiseInterrupt(uint16_t flags)
{
    PortIoWrite16(REG_IF, PortIoRead16(REG_IF) | flags);
}

static void DmaRunTimed(int timing);

unsigned int PortIoAdvance(void)
{
    uint16_t status = PortIoRead16(REG_DISPSTAT);
    unsigned int events = 0;

    if (!sInHBlank) {
        sInHBlank = 1;
        if (sLine < VISIBLE_LINES)
            DmaRunTimed(DMA_AT_HBLANK);
        if (status & DISPSTAT_HBLANK_IRQ)
            PortIoRaiseInterrupt(INTR_HBLANK);
        events |= PORT_IO_HBLANK_STARTED;
    } else {
        sInHBlank = 0;
        sLine = (sLine + 1) % LINES;
        if (sLine == VBLANK_FIRST_LINE) {
            DmaRunTimed(DMA_AT_VBLANK);
            if (status & DISPSTAT_VBLANK_IRQ)
                PortIoRaiseInterrupt(INTR_VBLANK);
            events |= PORT_IO_VBLANK_STARTED;
        }
        if (sLine == (unsigned int)(status >> 8) && (status & DISPSTAT_VCOUNT_IRQ))
            PortIoRaiseInterrupt(INTR_VCOUNT);
    }
    UpdateDisplayStatus();
    return events;
}

unsigned int PortIoScanline(void)
{
    return sLine;
}

// --- DMA ------------------------------------------------------------------------------------

// DMA reaches the registers through the host view; everything else is at its GBA address.
static uintptr_t DmaAddress(uint32_t address)
{
    if (address - GBA_IO_BASE < PORT_IO_SIZE)
        return (uintptr_t)sIo + (address - GBA_IO_BASE);
    return address;
}

// Whether DMA can reach an address: RAM, the registers and, for reading, the ROM. The port's
// game stack stands in for the part of IWRAM that the GBA's stack uses.
static int DmaReaches(uint32_t address, unsigned int unit, int isWrite)
{
    static const struct {
        uint32_t base, size;
    } kRam[] = {
        { GBA_EWRAM_BASE, GBA_EWRAM_SIZE }, { GBA_IWRAM_BASE, GBA_IWRAM_SIZE }, { GBA_IO_BASE, PORT_IO_SIZE },
        { GBA_PLTT_BASE, GBA_PLTT_SIZE },   { GBA_VRAM_BASE, GBA_VRAM_SIZE },   { GBA_OAM_BASE, GBA_OAM_SIZE },
        { PORT_GAME_STACK_LIMIT, PORT_GAME_STACK_TOP - PORT_GAME_STACK_LIMIT },
    };
    unsigned int i;

    for (i = 0; i < sizeof(kRam) / sizeof(kRam[0]); i++)
        if (address - kRam[i].base <= kRam[i].size - unit)
            return 1;
    return !isWrite && address - GBA_ROM_BASE <= GBA_ROM_SIZE - unit;
}

// The hardware ignores writes to the BIOS and to unmapped addresses, and reads from them give
// whatever was last on the bus. The port skips such units and reports the first one.
static void DmaReportOutOfReach(const struct DmaChannel *d, uint32_t address, int isWrite)
{
    static int reported;

    if (reported)
        return;
    reported = 1;
    fprintf(stderr,
            "DMA %s 0x%08X, which it cannot reach, is skipped (source 0x%08X, destination 0x%08X, control "
            "0x%04X; started by the game at executable+0x%llX). This message is shown once.\n",
            isWrite ? "write to" : "read from", address, d->src, d->dst, d->control,
            (unsigned long long)PortOsExecutableOffset(sTrapPc));
}

static void DmaTransfer(struct DmaChannel *d)
{
    unsigned int unit = d->control & DMA_32BIT ? 4 : 2;
    int32_t srcStep = DMA_SRC_MODE(d->control) == 1 ? -(int32_t)unit : DMA_SRC_MODE(d->control) == 2 ? 0 : (int32_t)unit;
    int32_t dstStep = DMA_DEST_MODE(d->control) == 1 ? -(int32_t)unit : DMA_DEST_MODE(d->control) == 2 ? 0 : (int32_t)unit;
    uint32_t src = d->src & ~(unit - 1), dst = d->dst & ~(unit - 1);
    uint32_t i;

    for (i = 0; i < d->count; i++) {
        if (!DmaReaches(src, unit, 0))
            DmaReportOutOfReach(d, src, 0);
        else if (!DmaReaches(dst, unit, 1))
            DmaReportOutOfReach(d, dst, 1);
        else if (unit == 4)
            *(volatile uint32_t *)DmaAddress(dst) = *(const volatile uint32_t *)DmaAddress(src);
        else
            *(volatile uint16_t *)DmaAddress(dst) = *(const volatile uint16_t *)DmaAddress(src);
        src += srcStep;
        dst += dstStep;
    }
    // The internal registers carry on from where the transfer stopped.
    d->src = src;
    d->dst = dst;
}

static uint32_t DmaCount(int n)
{
    uint32_t count = PortIoRead16(REG_DMA0SAD + n * DMA_STRIDE + 8) & (n == 3 ? 0xFFFF : 0x3FFF);
    return count ? count : n == 3 ? 0x10000 : 0x4000;
}

static void DmaDisable(int n)
{
    unsigned int cnt = REG_DMA0SAD + n * DMA_STRIDE + 10;
    sDma[n].active = 0;
    PortIoWrite16(cnt, PortIoRead16(cnt) & ~DMA_ENABLE);
}

// A write that sets a channel's enable bit latches its source, destination and count.
static void DmaEnable(int n)
{
    unsigned int base = REG_DMA0SAD + n * DMA_STRIDE;
    struct DmaChannel *d = &sDma[n];

    d->control = PortIoRead16(base + 10);
    d->src = Read32(base) & (n == 0 ? 0x07FFFFFF : 0x0FFFFFFF);
    d->dst = Read32(base + 4) & (n == 3 ? 0x0FFFFFFF : 0x07FFFFFF);
    d->count = DmaCount(n);
    d->active = 1;
    if (DMA_TIMING(d->control) == DMA_IMMEDIATE) {
        DmaTransfer(d);
        DmaDisable(n);
    }
}

static void DmaRunTimed(int timing)
{
    int n;

    for (n = 0; n < 4; n++) {
        struct DmaChannel *d = &sDma[n];
        if (!d->active || DMA_TIMING(d->control) != timing)
            continue;
        DmaTransfer(d);
        if (d->control & DMA_REPEAT) {
            d->count = DmaCount(n);
            if (DMA_DEST_MODE(d->control) == 3)
                d->dst = Read32(REG_DMA0SAD + n * DMA_STRIDE + 4) & (n == 3 ? 0x0FFFFFFF : 0x07FFFFFF);
        } else {
            DmaDisable(n);
        }
    }
}

void PortIoVBlank(void)
{
    DmaRunTimed(DMA_AT_VBLANK);
}

void PortIoHBlank(void)
{
    DmaRunTimed(DMA_AT_HBLANK);
}

// --- Trapped accesses -----------------------------------------------------------------------

static int Touches(unsigned int offset, unsigned int reg, unsigned int size)
{
    // An access starting up to 3 bytes before the register may cover it.
    return offset + 3 >= reg && offset < reg + size;
}

static void BeforeAccess(uintptr_t address, int isWrite, uintptr_t pc)
{
    unsigned int offset = (unsigned int)(address - GBA_IO_BASE);

    sTraps++;
    sTrapPc = pc;
    if (offset >= PORT_IO_SIZE)
        return;
    if (!isWrite && Touches(offset, REG_DISPSTAT, 4)) {
        // The game is polling the display: move it on by half a scanline per read. An
        // interrupt this raises waits for the game's next Halt.
        PortIoAdvance();
    }
    memcpy(sBefore, sIo, PORT_IO_SIZE);
}

static void AfterAccess(uintptr_t address, int isWrite, uintptr_t pc)
{
    unsigned int offset = (unsigned int)(address - GBA_IO_BASE);
    uint16_t value, written;
    int n;

    (void)pc;

    if (offset >= PORT_IO_SIZE)
        return;

    // Read-only registers and bits, and write-only bits, which read as 0: the sound channels'
    // restart bits and the sound FIFO resets.
    PortIoWrite16(REG_SOUND1CNT_X, PortIoRead16(REG_SOUND1CNT_X) & 0x7FFF);
    PortIoWrite16(REG_SOUND2CNT_H, PortIoRead16(REG_SOUND2CNT_H) & 0x7FFF);
    PortIoWrite16(REG_SOUND3CNT_X, PortIoRead16(REG_SOUND3CNT_X) & 0x7FFF);
    PortIoWrite16(REG_SOUND4CNT_H, PortIoRead16(REG_SOUND4CNT_H) & 0x7FFF);
    PortIoWrite16(REG_SOUNDCNT_H, PortIoRead16(REG_SOUNDCNT_H) & ~0x8800);
    PortIoWrite16(REG_VCOUNT, Before16(REG_VCOUNT));
    PortIoWrite16(REG_KEYINPUT, Before16(REG_KEYINPUT));
    value = PortIoRead16(REG_DISPSTAT);
    PortIoWrite16(REG_DISPSTAT, (value & ~DISPSTAT_STATUS) | (Before16(REG_DISPSTAT) & DISPSTAT_STATUS));

    // IF: the bits written as 1 are cleared. A write that starts at IF certainly wrote it,
    // even with the value it already had (REG_IF |= flag writes back every pending bit).
    // Otherwise IF was written only if it changed: a 32-bit store of IE and IF that leaves
    // IF as it was is taken for a store of IE alone.
    value = PortIoRead16(REG_IF);
    written = 0;
    if (isWrite && offset == REG_IF)
        written = value;
    else if (isWrite && offset == REG_IF + 1)
        written = value & 0xFF00;
    else if (value != Before16(REG_IF))
        written = value;
    PortIoWrite16(REG_IF, Before16(REG_IF) & ~written);

    // DMA: act on a channel's enable bit changing.
    for (n = 0; n < 4; n++) {
        unsigned int cnt = REG_DMA0SAD + n * DMA_STRIDE + 10;
        uint16_t before = Before16(cnt), now = PortIoRead16(cnt);
        if (!(before & DMA_ENABLE) && (now & DMA_ENABLE))
            DmaEnable(n);
        else if ((before & DMA_ENABLE) && !(now & DMA_ENABLE))
            sDma[n].active = 0;
        else if (now & DMA_ENABLE)
            sDma[n].control = now;  // a running channel takes new control bits
    }
}

int PortIoMap(void)
{
    if (PortOsMapTrappedRegion(GBA_IO_BASE, TRAP_RESERVE, TRAP_PAGE, &sIo, BeforeAccess, AfterAccess))
        return -1;
    // The state the BIOS leaves when it starts the game.
    PortIoWrite16(0x000, 0x0080);  // DISPCNT: forced blank
    PortIoWrite16(0x020, 0x0100);  // BG2PA and BG2PD: the identity transform
    PortIoWrite16(0x026, 0x0100);
    PortIoWrite16(0x030, 0x0100);  // BG3PA and BG3PD
    PortIoWrite16(0x036, 0x0100);
    PortIoWrite16(0x088, 0x0200);  // SOUNDBIAS
    PortIoWrite16(0x134, 0x8000);  // RCNT
    PortIoWrite16(REG_KEYINPUT, 0x03FF);  // no buttons pressed
    PortIoWrite16(0x300, 0x0001);  // POSTFLG: past the boot
    PortIoSetScanline(0);
    return 0;
}

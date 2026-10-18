// Unit tests for the trapped I/O registers (port/src/gba_io.c), run with wl4 --test-io.
#include "port/gba_io.h"

#include <stdio.h>
#include <string.h>

#include "port/game_thread.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define IO16(offset) (*(volatile uint16_t *)(uintptr_t)(0x04000000u + (offset)))
#define IO32(offset) (*(volatile uint32_t *)(uintptr_t)(0x04000000u + (offset)))
#define EWRAM16(offset) ((volatile uint16_t *)(uintptr_t)(0x02000000u + (offset)))
#define EWRAM32(offset) ((volatile uint32_t *)(uintptr_t)(0x02000000u + (offset)))

static int sFailures;

static void Check(int ok, const char *what)
{
    if (!ok) {
        printf("FAIL: %s\n", what);
        sFailures++;
    }
}

static void Fill16(volatile uint16_t *p, unsigned int count, uint16_t first)
{
    unsigned int i;
    for (i = 0; i < count; i++)
        p[i] = (uint16_t)(first + i);
}

static void TestPlainRegisters(void)
{
    IO16(0x10) = 0x1234;  // BG0HOFS
    Check(IO16(0x10) == 0x1234 && PortIoRead16(0x10) == 0x1234, "a register keeps what the game writes");
    PortIoWrite16(0x12, 0x5678);
    Check(IO16(0x12) == 0x5678, "the game reads what the host writes");
    IO16(0x00) = 0x0100;
    IO16(0x00) |= 0x0080;  // read-modify-write
    Check(IO16(0x00) == 0x0180, "read-modify-write of DISPCNT");
}

static void TestImmediateDma(void)
{
    volatile uint16_t *src = EWRAM16(0x100), *dst = EWRAM16(0x200);
    volatile uint32_t *dst32 = EWRAM32(0x300);
    uint32_t fill = 0xCAFEF00D;
    uint16_t fill16 = 0x7777;

    Fill16(src, 8, 1);
    memset((void *)dst, 0, 32);
    IO32(0xD4) = 0x02000100;  // DMA3SAD
    IO32(0xD8) = 0x02000200;  // DMA3DAD
    IO32(0xDC) = 0x80000000 | 6;  // enable, immediate, 16-bit, 6 units
    Check(dst[0] == 1 && dst[5] == 6 && dst[6] == 0, "immediate 16-bit DMA copies 6 halfwords");
    Check((IO16(0xDE) & 0x8000) == 0, "an immediate DMA clears its enable bit");

    // A fill from the game stack, as DmaFill32 does.
    IO32(0xD4) = (uint32_t)(uintptr_t)&fill;
    IO32(0xD8) = 0x02000300;
    IO32(0xDC) = (0x8000 | 0x0400 | 0x0100) << 16 | 3;  // enable, 32-bit, source fixed
    Check(dst32[0] == fill && dst32[2] == fill && dst32[3] != fill, "immediate 32-bit fill from the stack");

    // Separate 16-bit writes through a computed address, decrementing destination.
    {
        uintptr_t dma = 0x040000D4;
        memset((void *)dst, 0, 32);
        ((volatile uint32_t *)dma)[0] = (uint32_t)(uintptr_t)&fill16;
        ((volatile uint32_t *)dma)[1] = 0x02000206;
        ((volatile uint16_t *)dma)[4] = 3;
        ((volatile uint16_t *)dma)[5] = 0x8000 | 0x0100 | 0x0020;  // fixed source, decrementing destination
        Check(dst[3] == 0x7777 && dst[1] == 0x7777 && dst[0] == 0 && dst[4] == 0,
              "DMA through a computed address, decrementing");
    }

    // A source DMA cannot read (the BIOS) gives the last value any DMA moved, 16-bit
    // values in both halves.
    memset((void *)dst32, 0, 16);
    IO32(0xD4) = 0x00000100;
    IO32(0xD8) = 0x02000300;
    IO32(0xDC) = (0x8000 | 0x0400) << 16 | 2;  // enable, 32-bit
    Check(dst32[0] == 0x77777777 && dst32[1] == 0x77777777 && dst32[2] == 0,
          "DMA from the BIOS writes the last value moved");
}

static void TestTimedDma(void)
{
    volatile uint16_t *src = EWRAM16(0x400), *dst = EWRAM16(0x500);

    Fill16(src, 16, 100);
    memset((void *)dst, 0, 32);
    IO32(0xC8) = 0x02000400;  // DMA2SAD
    IO32(0xCC) = 0x02000500;  // DMA2DAD
    IO32(0xD0) = (0x8000 | 0x1000) << 16 | 4;  // enable at VBlank, 4 halfwords
    Check(dst[0] == 0 && (IO16(0xD2) & 0x8000), "a VBlank DMA waits for VBlank");
    PortIoVBlank();
    Check(dst[0] == 100 && dst[3] == 103 && dst[4] == 0, "a VBlank DMA runs at VBlank");
    Check((IO16(0xD2) & 0x8000) == 0, "a VBlank DMA without repeat disables itself");

    // HBlank, repeating, destination reloaded: the scroll-per-line pattern.
    memset((void *)dst, 0, 32);
    IO32(0xBC) = 0x02000400;  // DMA1SAD
    IO32(0xC0) = 0x02000500;  // DMA1DAD
    IO32(0xC4) = (0x8000 | 0x2000 | 0x0200 | 0x0060) << 16 | 2;  // HBlank, repeat, reload destination
    PortIoHBlank();
    PortIoHBlank();
    Check(dst[0] == 102 && dst[1] == 103 && dst[2] == 0, "a repeating HBlank DMA reloads its destination");
    Check((IO16(0xC6) & 0x8000) != 0, "a repeating DMA stays enabled");
    IO16(0xC6) = 0;
    PortIoHBlank();
    Check(dst[0] == 102, "a stopped DMA does not run");
}

static void TestInterruptFlags(void)
{
    PortIoWrite16(0x202, 0x0005);
    IO16(0x202) = 0x0001;
    Check(PortIoRead16(0x202) == 0x0004, "writing 1 to an IF bit clears it");
    PortIoWrite16(0x202, 0x0007);
    IO16(0x202) |= 0x0001;  // the game's way: writes back every pending bit
    Check(PortIoRead16(0x202) == 0x0000, "REG_IF |= flag clears every pending bit");
    PortIoWrite16(0x202, 0x0003);
    IO32(0x200) = 0x00020000 | 0x2001;  // IE and IF together
    Check(PortIoRead16(0x200) == 0x2001 && PortIoRead16(0x202) == 0x0001, "a 32-bit store to IE and IF");
    PortIoWrite16(0x202, 0x0001);
    IO16(0x200) = 0x0003;
    Check(PortIoRead16(0x202) == 0x0001, "a store to IE alone leaves IF");
}

static void TestDisplayStatus(void)
{
    unsigned int reads = 0;

    PortIoSetScanline(0);
    while (IO16(0x06) <= 159 && reads < 1000)  // the game's wait for VBlank
        reads++;
    Check(IO16(0x06) == 160 && reads < 1000, "a loop waiting for VCOUNT ends");
    Check((IO16(0x04) & 1) != 0, "DISPSTAT shows VBlank");
    IO16(0x06) = 5;
    Check(PortIoRead16(0x06) != 5, "VCOUNT ignores writes");
    IO16(0x04) = 0x4508;  // VCount line 0x45, VBlank interrupt enabled, status bits 0
    Check((PortIoRead16(0x04) & 0xFF38) == 0x4508 && (PortIoRead16(0x04) & 1), "DISPSTAT keeps its status bits");
    PortIoSetScanline(0x45);
    Check((PortIoRead16(0x04) & 4) != 0, "DISPSTAT flags the VCount match");
    IO16(0x130) = 0;
    Check(PortIoRead16(0x130) == 0x03FF, "KEYINPUT ignores writes");
}

static void TestSpeed(void)
{
    LARGE_INTEGER frequency, start, end;
    uint64_t traps = PortIoTrapCount();
    int i;
    double us;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    for (i = 0; i < 20000; i++)
        IO16(0x10) = (uint16_t)i;
    QueryPerformanceCounter(&end);
    us = (double)(end.QuadPart - start.QuadPart) * 1e6 / (double)frequency.QuadPart;
    printf("I/O: %llu trapped accesses, %.2f us each\n", (unsigned long long)(PortIoTrapCount() - traps), us / 20000);
}

// On the game coroutine, so that stack addresses fit the 32-bit DMA registers.
static void IoTestEntry(void)
{
    TestPlainRegisters();
    TestImmediateDma();
    TestTimedDma();
    TestInterruptFlags();
    TestDisplayStatus();
    TestSpeed();
}

int PortIoTest(void)
{
    sFailures = 0;
    if (PortGameThreadCreate(IoTestEntry))
        return 1;
    while (PortGameThreadResume())
        ;
    printf(sFailures ? "I/O tests: %d failed\n" : "I/O tests passed\n", sFailures);
    return sFailures != 0;
}

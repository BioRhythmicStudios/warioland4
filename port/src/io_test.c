// Unit tests for the trapped I/O registers (port/src/gba_io.c), run with wl4 --test-io.
#include "port/gba_io.h"

#include <stdio.h>
#include <string.h>

#include "port/audio.h"
#include "port/frame.h"
#include "port/gba_functions.h"
#include "port/game_thread.h"
#include "port/x86_access.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif

#define IO8(offset) (*(volatile uint8_t *)(uintptr_t)(0x04000000u + (offset)))
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

static void TestAccessSize(void)
{
    static const struct {
        uint8_t code[8];
        unsigned int size;
    } kCases[] = {
        { { 0x88, 0x07 }, 1 },                    // mov [rdi], al
        { { 0x66, 0x89, 0x07 }, 2 },              // mov [rdi], ax
        { { 0x89, 0x07 }, 4 },                    // mov [rdi], eax
        { { 0x48, 0x89, 0x07 }, 8 },              // mov [rdi], rax
        { { 0x66, 0xC7, 0x07, 0x34, 0x12 }, 2 },  // mov word [rdi], 0x1234
        { { 0xC6, 0x07, 0x80 }, 1 },              // mov byte [rdi], 0x80
        { { 0x80, 0x0F, 0x80 }, 1 },              // or byte [rdi], 0x80
        { { 0x0F, 0x11, 0x07 }, 16 },             // movups [rdi], xmm0
        { { 0xF3, 0x0F, 0x11, 0x07 }, 4 },        // movss [rdi], xmm0
        { { 0xC5, 0xF8, 0x11, 0x07 }, 0 },        // vmovups: not recognised
    };
    unsigned int i;

    for (i = 0; i < sizeof(kCases) / sizeof(kCases[0]); i++)
        Check(PortX86AccessSize(kCases[i].code) == kCases[i].size, "x86 store sizes");
}

static void DrainAudio(void)
{
    int16_t frames[1024 * 2];

    while (PortAudioTake(frames, 1024) != 0)
        ;
}

// Runs the sound hardware for a number of samples and counts the left channel's rising edges.
static unsigned int RisingEdges(unsigned int samples)
{
    int16_t frames[1024 * 2];
    unsigned int edges = 0, n, i;
    int last = 0;

    DrainAudio();
    while (samples != 0) {
        n = samples < 1024 ? samples : 1024;
        PortAudioRun(n * PORT_AUDIO_CYCLES_PER_SAMPLE);
        n = (unsigned int)PortAudioTake(frames, n);
        for (i = 0; i < n; i++) {
            if (frames[i * 2] > 0 && last <= 0)
                edges++;
            last = frames[i * 2];
        }
        samples -= n;
    }
    return edges;
}

static void TestSound(void)
{
    IO16(0x84) = 0x0080;  // sound on
    IO16(0x80) = 0xFF77;  // PSG full volume, every channel both sides
    IO16(0x82) = 0x0002;  // PSG 100%, Direct Sound off

    // Write-only bits read as 0, as on the GBA; the PSG still gets them.
    IO16(0x60) = 0x00FF;
    Check(IO16(0x60) == 0x007F, "SOUND1CNT_L keeps its 7 bits");
    IO16(0x60) = 0;
    IO16(0x62) = 0xF080;  // duty 50%, length 64, envelope 15, constant
    IO16(0x64) = 0x0400;  // frequency 1024 (128 Hz): a 16-bit store, high byte 4
    Check(IO16(0x64) == 0, "SOUND1CNT_X's frequency reads as 0");
    IO8(0x65) = 0x84;  // restart, frequency high bits written again
    Check((IO16(0x84) & 1) != 0, "SOUNDCNT_X shows square 1 playing");
    // 128 Hz for a quarter of a second.
    {
        unsigned int edges = RisingEdges(PORT_AUDIO_RATE / 4);
        Check(edges >= 31 && edges <= 33, "square 1 plays at its frequency");
    }

    // Length: 1/256 s with length 63, then square 1 stops.
    IO16(0x62) = 0xF03F;
    IO16(0x64) = 0xC400;  // restart, length on
    Check((IO16(0x84) & 1) != 0, "square 1 restarts");
    RisingEdges(PORT_AUDIO_RATE / 100);
    Check((IO16(0x84) & 1) == 0, "square 1 stops when its length runs out");

    // Wave RAM: the CPU reaches the bank that is not playing.
    IO16(0x70) = 0x0040;  // bank 1 plays
    IO16(0x90) = 0x1234;  // into bank 0
    IO16(0x70) = 0x0000;  // bank 0 plays: the CPU sees bank 1
    Check(IO16(0x90) != 0x1234, "wave RAM switches banks");
    IO16(0x70) = 0x0040;
    Check(IO16(0x90) == 0x1234, "wave RAM keeps each bank");

    // Direct Sound: DMA 1 feeds FIFO A from EWRAM on timer 0.
    {
        volatile uint8_t *pcm = (volatile uint8_t *)(uintptr_t)0x02000800;
        unsigned int i;

        for (i = 0; i < 64; i++)
            pcm[i] = 0x60;
        IO16(0x80) = 0;
        IO16(0x82) = 0x0B0E;  // A full volume, both sides, timer 0, FIFO reset
        IO16(0x100) = (uint16_t)(0x10000 - 1254);  // 13379 Hz
        IO16(0x102) = 0x0080;
        IO32(0xBC) = 0x02000800;
        IO32(0xC0) = 0x040000A0;
        IO16(0xC6) = 0xB600;  // enable, special timing, 32-bit, repeat
        DrainAudio();
        PortAudioRun(PORT_AUDIO_CYCLES_PER_SAMPLE * 64);
        {
            int16_t frames[64 * 2];
            size_t n = PortAudioTake(frames, 64);
            Check(n == 64 && frames[63 * 2] > 0 && frames[63 * 2 + 1] > 0, "FIFO A plays what DMA 1 feeds it");
        }
        IO16(0xC6) = 0;
        IO16(0x102) = 0;
    }
    IO16(0x84) = 0;  // sound off: the PSG registers clear
    Check(IO16(0x62) == 0 && IO16(0x80) == 0, "turning the sound off clears the PSG registers");
}

static void TestSpeed(void)
{
#ifdef _WIN32
    LARGE_INTEGER frequency, start, end;
#else
    struct timespec start, end;
#endif
    uint64_t traps = PortIoTrapCount();
    int i;
    double us;

#ifdef _WIN32
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
#else
    clock_gettime(CLOCK_MONOTONIC, &start);
#endif
    for (i = 0; i < 20000; i++)
        IO16(0x10) = (uint16_t)i;
#ifdef _WIN32
    QueryPerformanceCounter(&end);
    us = (double)(end.QuadPart - start.QuadPart) * 1e6 / (double)frequency.QuadPart;
#else
    clock_gettime(CLOCK_MONOTONIC, &end);
    us = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
#endif
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
    TestAccessSize();
    TestSound();
    TestSpeed();
}

// Frog Switch regression: an IRQ reads VCOUNT as a snapshot, not as a scanline wait.
extern uint32_t gHBlankCallback, gVBlankCallback;
extern uint16_t gHBlankCallbackTrampoline[];
extern volatile uint16_t gInterruptCheck;
static unsigned int sHBlankCalls;

static void ReadDisplayInHBlank(void)
{
    unsigned int line = PortIoScanline();
    uint16_t count = IO16(0x06);
    uint16_t status = IO16(0x04);
    Check(count == line && PortIoScanline() == line && (status & 2),
          "HBlank VCOUNT/DISPSTAT reads preserve the current display event");
    // Keep the regression finite even when display reads erroneously generate more IRQs.
    if (++sHBlankCalls > 1000) {
        PortIoWrite16(0x200, 1);
        PortIoWrite16(0x04, 8);
    }
}

static void DisplayInterruptEntry(void)
{
    *(uint32_t *)(uintptr_t)0x03007FFC = 0x080000FC;
    PortSetRamFunction((uint32_t)(uintptr_t)gHBlankCallbackTrampoline, ReadDisplayInHBlank);
    gHBlankCallback = (uint32_t)(uintptr_t)gHBlankCallbackTrampoline | 1;
    gVBlankCallback = 0;
    PortIoWrite16(0x202, 0);
    PortIoWrite16(0x200, 3);
    PortIoWrite16(0x208, 1);
    PortIoWrite16(0x04, 0x18);
    for (;;) {
        PortFrameHalt();
    }
}

static void TestDisplayInterrupts(void)
{
    unsigned int i;
    sHBlankCalls = 0;
    PortIoSetScanline(0);
    if (PortFrameStart(DisplayInterruptEntry)) {
        Check(0, "create display IRQ regression coroutine");
        return;
    }
    for (i = 0; i < 3; i++) {
        Check(PortFrameRun(), "frame loop continues with HBlank display reads");
        Check(PortIoScanline() == 160, "host observes VBlank despite HBlank display reads");
        if (i) {
            Check(gInterruptCheck == 1, "VBlank wakes the game after HBlank display reads");
        }
    }
    Check(sHBlankCalls == 616, "one HBlank callback per line through three VBlanks");
    PortIoWrite16(0x200, 0);
    PortIoWrite16(0x208, 0);
    PortIoWrite16(0x04, 0);
    gHBlankCallback = 0;
}

int PortIoTest(void)
{
    sFailures = 0;
    if (PortGameThreadCreate(IoTestEntry))
        return 1;
    while (PortGameThreadResume())
        ;
    TestDisplayInterrupts();
    printf(sFailures ? "I/O tests: %d failed\n" : "I/O tests passed\n", sFailures);
    return sFailures != 0;
}

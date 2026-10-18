#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "port/bios.h"
#include "port/frame.h"
#include "port/game_thread.h"
#include "port/gba_io.h"
#include "port/gba_functions.h"
#include "port/gba_memory.h"
#include "port/ppu.h"
#include "port/rom.h"
#include "port/sdl_frontend.h"

static void Usage(void)
{
    fprintf(stderr,
            "usage: wl4 [ROM] [--scale N] [--frames N [--dump FILE] [--shots FILE]] [--mgba-colors]\n"
            "           [--selftest] [--test-bios] [--test-io]\n"
            "           [--fault rom-write|unmapped|stack-overflow] [--asan-probe]\n"
            "  ROM defaults to baserom_us.gba in the current directory.\n"
            "  With no --frames or tests, the game runs in a window, --scale times 240x160 (default 3).\n"
            "  --frames N runs the game headless for N frames; --dump writes GBA memory after each,\n"
            "  --shots the picture. --mgba-colors blends colours as mGBA does, to compare with it.\n");
}

// --- Self-test of the game coroutine -------------------------------------------------------

static int sYields;
static int sStackOk;
static unsigned int sRecurseResult;

static int OnGameStack(const void *p)
{
    uintptr_t a = (uintptr_t)p;
    return a >= PORT_GAME_STACK_LIMIT && a < PORT_GAME_STACK_TOP;
}

// Uses about depth * 4 KiB of stack, yielding from the deepest frame.
static __attribute__((noinline)) unsigned int Recurse(unsigned int depth)
{
    volatile unsigned char frame[4000];
    unsigned int sum;

    frame[0] = (unsigned char)depth;
    frame[sizeof(frame) - 1] = (unsigned char)(depth >> 8);
    if (depth == 0) {
        PortGameThreadYield();
        return 0;
    }
    sum = Recurse(depth - 1);
    return sum + frame[0] + frame[sizeof(frame) - 1];
}

static void SelfTestEntry(void)
{
    volatile int local[64];
    int i;

    local[0] = 1;
    // The frame, not a local: AddressSanitizer may keep locals on its own fake stacks.
    sStackOk = OnGameStack(__builtin_frame_address(0));
    for (i = 0; i < 3; i++) {
        local[i + 1] = i;
        PortGameThreadYield();
        sYields++;
    }
    // 2 MiB deep, to touch many stack pages; yields once at the bottom.
    sRecurseResult = Recurse(512);
}

static int SelfTest(void)
{
    unsigned int expected = 0, depth;
    int yields = 0;

    for (depth = 1; depth <= 512; depth++)
        expected += (depth & 0xFF) + (depth >> 8);
    if (PortGameThreadCreate(SelfTestEntry))
        return 1;
    while (PortGameThreadResume())
        yields++;
    printf("coroutine: %d yields, locals on the game stack: %s, 2 MiB recursion: %s\n", yields,
           sStackOk ? "yes" : "NO", sRecurseResult == expected ? "ok" : "WRONG");
    return !(sStackOk && sYields == 3 && yields == 4 && sRecurseResult == expected);
}

// --- Deliberate faults, to show the reports -------------------------------------------------

static __attribute__((noinline)) unsigned int Overflow(unsigned int n)
{
    volatile unsigned char frame[16384];

    frame[0] = (unsigned char)n;
    return Overflow(n + 1) + frame[0];
}

static const char *sFault;

static void FaultEntry(void)
{
    if (strcmp(sFault, "rom-write") == 0)
        *(volatile uint8_t *)(uintptr_t)(GBA_ROM_BASE + 0x100) = 0;
    else if (strcmp(sFault, "unmapped") == 0)
        (void)*(volatile uint32_t *)(uintptr_t)0x09000000;
    else if (strcmp(sFault, "stack-overflow") == 0)
        Overflow(0);
}

static void AsanProbeEntry(void)
{
    volatile char buffer[16];
    volatile int index = 16;

    PortGameThreadYield();
    buffer[index] = 1;  // one past the end: AddressSanitizer must report this on the game stack
}

// --- Running the game -----------------------------------------------------------------------

void AgbMain(void);
void irq_handler(void);

// What crt0's _start does after setting up the stacks: it points the interrupt vector at
// irq_handler in the ROM, calls AgbMain, and starts again if AgbMain returns.
static void GameEntry(void)
{
    for (;;) {
        *(volatile uint32_t *)(uintptr_t)0x03007FFC = PortNativeFunctionToGba(irq_handler) & ~1u;
        AgbMain();
    }
}

static int RunGame(unsigned int frames, const char *dumpPath, const char *shotsPath)
{
    FILE *dump = NULL, *shots = NULL;
    clock_t start = clock();
    double seconds;

    if (dumpPath != NULL && (dump = fopen(dumpPath, "wb")) == NULL) {
        perror(dumpPath);
        return 1;
    }
    if (shotsPath != NULL && (shots = fopen(shotsPath, "wb")) == NULL) {
        perror(shotsPath);
        return 1;
    }
    if (PortFrameStart(GameEntry))
        return 1;
    while (PortFrameCount() < frames) {
        if (!PortFrameRun()) {
            printf("The game stopped after %u frames\n", PortFrameCount());
            break;
        }
        if (dump != NULL && PortFrameDump(dump)) {
            perror(dumpPath);
            return 1;
        }
        if (shots != NULL && PortFrameShot(shots)) {
            perror(shotsPath);
            return 1;
        }
    }
    if (dump != NULL && fclose(dump) != 0) {
        perror(dumpPath);
        return 1;
    }
    if (shots != NULL && fclose(shots) != 0) {
        perror(shotsPath);
        return 1;
    }
    seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("%u frames in %.2f s (%.0f fps); %.0f trapped I/O accesses per frame\n", PortFrameCount(), seconds,
           seconds > 0 ? PortFrameCount() / seconds : 0.0,
           PortFrameCount() ? (double)PortIoTrapCount() / PortFrameCount() : 0.0);
    return 0;
}

int main(int argc, char **argv)
{
    const char *romPath = "baserom_us.gba";
    const char *dumpPath = NULL, *shotsPath = NULL;
    enum PortPpuColors colors = PORT_PPU_COLORS_GBA;
    unsigned int frames = 0, scale = 3;
    int selfTest = 0, biosTest = 0, ioTest = 0, asanProbe = 0;
    int i;

    // Reserve the GBA address space before anything else can take it.
    if (PortReserveGbaMemory())
        return 1;
    PortInstallFaultHandler();

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--selftest") == 0)
            selfTest = 1;
        else if (strcmp(argv[i], "--test-bios") == 0)
            biosTest = 1;
        else if (strcmp(argv[i], "--test-io") == 0)
            ioTest = 1;
        else if (strcmp(argv[i], "--asan-probe") == 0)
            asanProbe = 1;
        else if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc)
            frames = (unsigned int)strtoul(argv[++i], NULL, 10);
        else if (strcmp(argv[i], "--dump") == 0 && i + 1 < argc)
            dumpPath = argv[++i];
        else if (strcmp(argv[i], "--shots") == 0 && i + 1 < argc)
            shotsPath = argv[++i];
        else if (strcmp(argv[i], "--mgba-colors") == 0)
            colors = PORT_PPU_COLORS_MGBA;
        else if (strcmp(argv[i], "--scale") == 0 && i + 1 < argc)
            scale = (unsigned int)strtoul(argv[++i], NULL, 10);
        else if (strcmp(argv[i], "--fault") == 0 && i + 1 < argc)
            sFault = argv[++i];
        else if (argv[i][0] == '-') {
            Usage();
            return 2;
        } else
            romPath = argv[i];
    }

    if (PortLoadRom(romPath))
        return 1;
    printf("ROM loaded at 0x%08X (%u KiB, MD5 %s, read-only); title %.12s\n", GBA_ROM_BASE, GBA_ROM_SIZE / 1024,
           PORT_ROM_MD5, (const char *)(uintptr_t)(GBA_ROM_BASE + 0xA0));
    printf("GBA memory reserved; game stack 0x%08X-0x%08X; %u game functions linked\n", PORT_GAME_STACK_LIMIT,
           PORT_GAME_STACK_TOP, gGbaFunctionCount);

    if (selfTest && SelfTest())
        return 1;
    if (biosTest && PortBiosTest())
        return 1;
    if (ioTest && PortIoTest())
        return 1;
    if (sFault || asanProbe) {
        if (PortGameThreadCreate(sFault ? FaultEntry : AsanProbeEntry))
            return 1;
        while (PortGameThreadResume())
            ;
        printf("no fault happened\n");
        return 1;
    }
    if (frames) {
        PortPpuReset(colors);
        return RunGame(frames, dumpPath, shotsPath);
    }
    if (selfTest || biosTest || ioTest)
        return 0;
#ifdef WL4_HAVE_SDL
    PortPpuReset(colors);
    return PortSdlRun(GameEntry, scale ? scale : 1);
#else
    fprintf(stderr, "This build has no window (it was built without SDL3); use --frames to run headless.\n");
    return 2;
#endif
}

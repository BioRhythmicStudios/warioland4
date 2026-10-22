#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "port/audio.h"
#include "port/bios.h"
#include "port/frame.h"
#include "port/game_thread.h"
#include "port/gba_io.h"
#include "port/gba_functions.h"
#include "port/gba_memory.h"
#include "port/input_script.h"
#include "port/os.h"
#include "port/ppu.h"
#include "port/rom.h"
#include "port/save.h"
#include "port/sdl_frontend.h"

extern int PortTileWrapTest(void);

static void Usage(void)
{
    fprintf(stderr,
            "usage: wl4 [ROM] [--scale N] [--frames N [--dump FILE] [--shots FILE] [--wav FILE] [--from-step S]]\n"
            "           [--input FILE] [--save FILE] [--mgba-colors] [--selftest] [--test-bios] [--test-io]\n"
            "           [--fault rom-write|unmapped|stack-overflow] [--asan-probe] [--test-save DIRECTORY]\n"
            "  ROM defaults to baserom_us.gba in the current directory.\n"
            "  With no --frames or tests, the game runs in a window, --scale times 240x160 (saved scale, or default 3).\n"
            "  --frames N runs the game headless for N frames; --dump writes GBA memory after each,\n"
            "  --shots the picture and --wav the sound, from the frame on which main-loop step S\n"
            "  finishes with --from-step.\n"
            "  --input presses keys from a script (port/include/port/input_script.h),\n"
            "  in the window as well. --mgba-colors blends colours as mGBA does, to compare with it.\n"
            "  --save keeps SRAM in FILE (an mGBA .sav). The window defaults to wl4.sav in the\n"
            "  user's application data (%%APPDATA%%\\wl4); headless runs save only with --save.\n");
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
extern int16_t gMainGameMode;
extern char gCreditsMainState;

// What crt0's _start does after setting up the stacks: it points the interrupt vector at
// irq_handler in the ROM, calls AgbMain, and starts again if AgbMain returns.
static void GameEntry(void)
{
    for (;;) {
        *(volatile uint32_t *)(uintptr_t)0x03007FFC = PortNativeFunctionToGba(irq_handler) & ~1u;
        AgbMain();
    }
}

#define REG_KEYINPUT 0x130
#define KEYS_ALL 0x3FF

// --- WAV output ---------------------------------------------------------------------------

static void Put16(uint8_t *p, unsigned int v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static void Put32(uint8_t *p, uint32_t v)
{
    Put16(p, v & 0xFFFF);
    Put16(p + 2, v >> 16);
}

// A 16-bit stereo WAV header for a number of frames.
static int WriteWavHeader(FILE *file, uint32_t frames)
{
    uint8_t h[44];

    memcpy(h, "RIFF", 4);
    Put32(h + 4, 36 + frames * 4);
    memcpy(h + 8, "WAVEfmt ", 8);
    Put32(h + 16, 16);
    Put16(h + 20, 1);  // PCM
    Put16(h + 22, 2);
    Put32(h + 24, PORT_AUDIO_RATE);
    Put32(h + 28, PORT_AUDIO_RATE * 4);
    Put16(h + 32, 4);
    Put16(h + 34, 16);
    memcpy(h + 36, "data", 4);
    Put32(h + 40, frames * 4);
    return fseek(file, 0, SEEK_SET) != 0 || fwrite(h, sizeof(h), 1, file) != 1 ? -1 : 0;
}

// Appends the sound produced since the last call, or drops it when file is NULL; *frames
// counts what the file holds.
static int WriteWavFrames(FILE *file, uint32_t *frames)
{
    int16_t buffer[4096 * 2];
    uint8_t bytes[sizeof(buffer)];
    size_t n, i;

    while ((n = PortAudioTake(buffer, 4096)) != 0) {
        if (file == NULL)
            continue;
        for (i = 0; i < n * 2; i++)
            Put16(bytes + i * 2, (uint16_t)buffer[i]);
        if (fseek(file, 0, SEEK_END) != 0 || fwrite(bytes, 4, n, file) != n)
            return -1;
        *frames += (uint32_t)n;
    }
    return 0;
}

static int RunGame(unsigned int frames, const char *dumpPath, const char *shotsPath, const char *wavPath,
                   unsigned int fromStep, const char *inputPath, const char *savePath, int creditsTest)
{
    FILE *dump = NULL, *shots = NULL, *wav = NULL;
    uint32_t wavFrames = 0;
    int creditsDone = 0;
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
    if (wavPath != NULL && ((wav = fopen(wavPath, "wb")) == NULL || WriteWavHeader(wav, 0))) {
        perror(wavPath);
        return 1;
    }
    if (inputPath != NULL && PortInputScriptLoad(inputPath))
        return 1;
    if (savePath != NULL && PortSaveOpen(savePath))
        return 1;
    if (PortFrameStart(GameEntry))
        return 1;
    while (PortFrameCount() < frames) {
        // The keys for the step the main loop runs next, which reads them when it starts.
        if (inputPath != NULL)
            PortIoWrite16(REG_KEYINPUT, (uint16_t)(~PortInputScriptKeys(PortFrameSteps() + 1) & KEYS_ALL));
        if (!PortFrameRun()) {
            printf("The game stopped after %u frames\n", PortFrameCount());
            break;
        }
        if (creditsTest && PortFrameCount() == 1150) {
            gCreditsMainState = 0;
            gMainGameMode = 12;
            printf("Credits regression: entering credits from disposable save at frame %u\n", PortFrameCount());
        } else if (creditsTest && PortFrameCount() > 1151 && gMainGameMode != 12) {
            printf("Credits regression: completed at frame %u, next mode %d\n", PortFrameCount(), gMainGameMode);
            creditsDone = 1;
            break;
        }
        PortSaveFrame();
        if (WriteWavFrames(PortFrameSteps() < fromStep ? NULL : wav, &wavFrames)) {
            perror(wavPath);
            return 1;
        }
        if (PortFrameSteps() < fromStep)
            continue;
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
    if (wav != NULL && (WriteWavHeader(wav, wavFrames) || fclose(wav) != 0)) {
        perror(wavPath);
        return 1;
    }
    if (PortSaveClose()) {
        fprintf(stderr, "Final save failed; exiting with unsaved progress.\n");
        return 1;
    }
    seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
    printf("%u frames in %.2f s (%.0f fps); %.0f trapped I/O accesses per frame\n", PortFrameCount(), seconds,
           seconds > 0 ? PortFrameCount() / seconds : 0.0,
           PortFrameCount() ? (double)PortIoTrapCount() / PortFrameCount() : 0.0);
    if (creditsTest && !creditsDone) {
        fprintf(stderr, "Credits regression: did not finish within %u frames\n", frames);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *romPath = "baserom_us.gba";
    const char *dumpPath = NULL, *shotsPath = NULL, *wavPath = NULL, *inputPath = NULL;
    const char *savePath = NULL;
    enum PortPpuColors colors = PORT_PPU_COLORS_GBA;
    unsigned int frames = 0, scale = 0, fromStep = 0;
    const char *saveTest = NULL;
    int selfTest = 0, biosTest = 0, ioTest = 0, asanProbe = 0, tileWrapTest = 0, creditsTest = 0;
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
        else if (strcmp(argv[i], "--test-save") == 0 && i + 1 < argc)
            saveTest = argv[++i];
        else if (strcmp(argv[i], "--asan-probe") == 0)
            asanProbe = 1;
        else if (strcmp(argv[i], "--test-tile-wrap") == 0)
            tileWrapTest = 1;
        else if (strcmp(argv[i], "--test-credits") == 0)
            creditsTest = 1;
        else if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc)
            frames = (unsigned int)strtoul(argv[++i], NULL, 10);
        else if (strcmp(argv[i], "--dump") == 0 && i + 1 < argc)
            dumpPath = argv[++i];
        else if (strcmp(argv[i], "--shots") == 0 && i + 1 < argc)
            shotsPath = argv[++i];
        else if (strcmp(argv[i], "--wav") == 0 && i + 1 < argc)
            wavPath = argv[++i];
        else if (strcmp(argv[i], "--from-step") == 0 && i + 1 < argc)
            fromStep = (unsigned int)strtoul(argv[++i], NULL, 10);
        else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc)
            inputPath = argv[++i];
        else if (strcmp(argv[i], "--save") == 0 && i + 1 < argc)
            savePath = argv[++i];
        else if (strcmp(argv[i], "--mgba-colors") == 0)
            colors = PORT_PPU_COLORS_MGBA;
        else if (strcmp(argv[i], "--scale") == 0 && i + 1 < argc) {
            scale = (unsigned int)strtoul(argv[++i], NULL, 10);
            if (scale < 1) scale = 1;
        }
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
    if (saveTest && PortSaveTest(saveTest))
        return 1;
    if (tileWrapTest)
        return PortTileWrapTest();
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
        return RunGame(frames, dumpPath, shotsPath, wavPath, fromStep, inputPath, savePath, creditsTest);
    }
    if (selfTest || biosTest || ioTest || saveTest)
        return 0;
#ifdef WL4_HAVE_SDL
    if (inputPath != NULL && PortInputScriptLoad(inputPath))
        return 1;
    PortPpuReset(colors);
    PortOsShowErrors(1);
    return PortSdlRun(GameEntry, scale, inputPath != NULL, savePath);
#else
    fprintf(stderr, "This build has no window (it was built without SDL3); use --frames to run headless.\n");
    return 2;
#endif
}

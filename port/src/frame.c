#include "port/frame.h"

#include <stdint.h>
#include <stdlib.h>

#include "port/game_thread.h"
#include "port/gba_io.h"
#include "port/gba_memory.h"
#include "port/ppu.h"

#define REG_IE 0x200
#define REG_IF 0x202
#define REG_IME 0x208

#define INTR_MASK 0x3FFF
#define INTR_KEYPAD 0x1000
#define INTR_GAMEPAK 0x2000

#define INTR_VECTOR 0x03007FFCu
#define IRQ_HANDLER_IN_ROM 0x080000FCu  // where crt0 points the vector before AgbMain copies it

// The game's interrupt handler, irq_handler in asm/crt0.s, is ARM code. The port runs this
// native version instead, with the same table and order.
void InterruptCallbackCallVBlank(void);
void InterruptCallbackCallHBlank(void);
void InterruptCallbackCallVCount(void);
void InterruptCallbackEmpty(void);
extern uint16_t gInterruptHandlerBuffer[];  // where InitializeInterruptHandler copies irq_handler

static void (*const kInterruptTable[])(void) = {
    InterruptCallbackCallVBlank,  // VBlank
    InterruptCallbackCallHBlank,  // HBlank
    InterruptCallbackCallVCount,  // VCount
    InterruptCallbackEmpty,       // timer 0
    InterruptCallbackEmpty,       // timer 1
    InterruptCallbackEmpty,       // timer 2
    InterruptCallbackEmpty,       // timer 3
    InterruptCallbackEmpty,       // serial
    InterruptCallbackEmpty,       // DMA 0
    InterruptCallbackEmpty,       // DMA 1
    InterruptCallbackEmpty,       // DMA 2
    InterruptCallbackEmpty,       // DMA 3
    InterruptCallbackEmpty,       // keypad
};

static int sInInterrupt;
static int sStarted;
static int sWake;  // an enabled interrupt is requested: the halted game should resume
static unsigned int sFrames;

// The main loop counts its steps in gMainTimer, and at the end of each one clears bit 0 of
// gInterruptCheck and halts until the VBlank handler sets it again.
extern volatile uint8_t gMainTimer;
extern volatile uint16_t gInterruptCheck;
static unsigned int sSteps;
static uint8_t sLastStepTimer;

static uint16_t Requested(void)
{
    return PortIoRead16(REG_IE) & PortIoRead16(REG_IF) & INTR_MASK;
}

// The BIOS calls the handler at INTR_VECTOR while IME is set and an enabled interrupt is
// requested. irq_handler acknowledges the first one in its table and calls its callback.
static void HandleInterrupts(void)
{
    uint16_t requested;

    while ((PortIoRead16(REG_IME) & 1) && (requested = Requested()) != 0) {
        uint32_t vector = *(volatile uint32_t *)(uintptr_t)INTR_VECTOR;
        unsigned int n;
        uint16_t flag;

        if (vector != (uint32_t)(uintptr_t)gInterruptHandlerBuffer && vector != IRQ_HANDLER_IN_ROM) {
            fprintf(stderr, "Interrupt 0x%04X requested with the interrupt vector at 0x%08X, not irq_handler\n",
                    requested, vector);
            abort();
        }
        if (requested & INTR_GAMEPAK) {
            // irq_handler loops forever when the cartridge is pulled.
            fprintf(stderr, "Game Pak interrupt: the GBA would freeze here\n");
            abort();
        }
        for (n = 0; n < 12 && !(requested & (1u << n)); n++)
            ;
        flag = n < 12 ? (uint16_t)(1u << n) : requested & INTR_KEYPAD;
        PortIoWrite16(REG_IF, PortIoRead16(REG_IF) & ~flag);  // strh to IF: writing 1 clears
        sInInterrupt = 1;
        kInterruptTable[n]();
        sInInterrupt = 0;
    }
}

int PortFrameInInterrupt(void)
{
    return sInInterrupt;
}

void PortFrameHalt(void)
{
    PortGameThreadYield();
    HandleInterrupts();
}

int PortFrameStart(void (*entry)(void))
{
    if (PortGameThreadCreate(entry))
        return -1;
    sStarted = 0;
    sFrames = 0;
    sSteps = 0;
    sLastStepTimer = 0;
    PortIoSetHBlankHandler(PortPpuHBlank);
    return 0;
}

unsigned int PortFrameSteps(void)
{
    return sSteps;
}

int PortFrameRun(void)
{
    for (;;) {
        unsigned int events;

        if (!sStarted || sWake) {
            sStarted = 1;
            sWake = 0;
            if (!PortGameThreadResume())
                return 0;
        }
        // The game is halted. If it halted with an interrupt still requested (IME clear), the
        // display moves on before the next resume, as time would pass on the GBA.
        events = PortIoAdvance();
        sWake = Requested() != 0;
        if (events & PORT_IO_VBLANK_STARTED) {
            sFrames++;
            if (!(gInterruptCheck & 1) && gMainTimer != sLastStepTimer) {
                sSteps++;
                sLastStepTimer = gMainTimer;
            }
            return 1;
        }
    }
}

unsigned int PortFrameCount(void)
{
    return sFrames;
}

int PortFrameDump(FILE *file)
{
    static const struct {
        uintptr_t base;
        size_t size;
    } kRegions[] = {
        { GBA_EWRAM_BASE, GBA_EWRAM_SIZE },
        { GBA_IWRAM_BASE, GBA_IWRAM_SIZE },
        { 0, PORT_IO_SIZE },  // the host view
        { GBA_PLTT_BASE, GBA_PLTT_SIZE },
        { GBA_VRAM_BASE, GBA_VRAM_SIZE },
        { GBA_OAM_BASE, GBA_OAM_SIZE },
        { GBA_SRAM_BASE, 0x8000 },  // the 32 KiB the cartridge has
    };
    struct PortFrameDumpHeader header = { PORT_FRAME_DUMP_MAGIC, sFrames };
    unsigned int i;

    if (fwrite(&header, sizeof(header), 1, file) != 1)
        return -1;
    for (i = 0; i < sizeof(kRegions) / sizeof(kRegions[0]); i++) {
        const void *p = kRegions[i].base ? (const void *)kRegions[i].base : PortIoHost();
        if (fwrite(p, 1, kRegions[i].size, file) != kRegions[i].size)
            return -1;
    }
    return 0;
}

int PortFrameShot(FILE *file)
{
    struct PortFrameDumpHeader header = { PORT_FRAME_SHOT_MAGIC, sFrames };
    const uint32_t *frame = PortPpuFrame();
    uint8_t rgb[PORT_SCREEN_WIDTH * 3];
    unsigned int x, y;

    if (fwrite(&header, sizeof(header), 1, file) != 1)
        return -1;
    for (y = 0; y < PORT_SCREEN_HEIGHT; y++) {
        for (x = 0; x < PORT_SCREEN_WIDTH; x++) {
            uint32_t pixel = frame[y * PORT_SCREEN_WIDTH + x];
            rgb[x * 3] = (uint8_t)pixel;
            rgb[x * 3 + 1] = (uint8_t)(pixel >> 8);
            rgb[x * 3 + 2] = (uint8_t)(pixel >> 16);
        }
        if (fwrite(rgb, 1, sizeof(rgb), file) != sizeof(rgb))
            return -1;
    }
    return 0;
}

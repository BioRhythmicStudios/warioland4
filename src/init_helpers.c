#include "interrupts.h"
#include "oam.h"

#ifndef PORTABLE
IWRAM_DATA OamData gOamBuffer[MAX_OAM_SLOTS] = {};
IWRAM_DATA u16 gButtonsHeld = 0;
IWRAM_DATA u16 gButtonsHeldCopy = 0;
IWRAM_DATA u16 gButtonsPressed = 0;
IWRAM_DATA u8 gOamSlotsUsed = 0;
#else
extern OamData gOamBuffer[MAX_OAM_SLOTS];
extern u16 gButtonsHeld;
extern u16 gButtonsHeldCopy;
extern u16 gButtonsPressed;
extern u8 gOamSlotsUsed;
#endif

void LZ77UnCompVramWrapper(const u32* src, void* dest)
{
    LZ77UnCompVram(src, dest);
}

void PollInput(void)
{
    u16 keys;

    keys = ~REG_KEYINPUT & KEYS_MASK;
    gButtonsPressed = keys & ~gButtonsHeldCopy;
    gButtonsHeldCopy = gButtonsHeld = keys;
}

void InitializeVideoMemory(void)
{
    DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, U8_MAX, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
}

void InitializeInterruptHandler(void)
{
#ifdef PORTABLE
    // irq_handler is ARM code: copy its bytes from the ROM image so that IWRAM matches the
    // GBA's. The platform layer calls the interrupt callbacks itself.
    DmaCopy16(3, (const void *)(PortNativeFunctionToGba((void (*)(void))irq_handler) & ~1u),
              gInterruptHandlerBuffer, sizeof(gInterruptHandlerBuffer));
#else
    DmaCopy16(3, irq_handler, gInterruptHandlerBuffer, sizeof(gInterruptHandlerBuffer));
#endif
    GP_SET(INTR_VECTOR, &gInterruptHandlerBuffer);
}

void ResetFreeOam(void)
{
    s32 i;
    u16* pOamSlot;

    pOamSlot = (u16*)gOamBuffer;
    pOamSlot += gOamSlotsUsed * sizeof(OamData) / sizeof(u16);
    for (i = gOamSlotsUsed; i < MAX_OAM_SLOTS; pOamSlot++, i++) {
        *pOamSlot++ = U8_MAX;
        *pOamSlot++ = U8_MAX;
        *pOamSlot++ = 0;
    }
}

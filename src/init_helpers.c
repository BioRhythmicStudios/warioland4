#include "interrupts.h"
#include "oam.h"

IWRAM_DATA OamData gOamBuffer[MAX_OAM_SLOTS] = {};
IWRAM_DATA u16 gButtonsHeld = 0;
IWRAM_DATA u16 gButtonsHeldCopy = 0;
IWRAM_DATA u16 gButtonsPressed = 0;
IWRAM_DATA u8 gOamSlotsUsed = 0;

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
    DmaCopy16(3, irq_handler, gInterruptHandlerBuffer, sizeof(gInterruptHandlerBuffer));
    INTR_VECTOR = &gInterruptHandlerBuffer;
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

#include "interrupt_callbacks.h"

#include "interrupts.h"

void InterruptCallbackCallVBlank(void)
{
    if (GF(gVBlankCallback) != NULL) {
        GF(gVBlankCallback)();
    }

    REG_IF |= INTR_FLAG_VBLANK;
    gInterruptCheck = 1;
}

void InterruptCallbackSetVBlank(ProcedureFunc cb)
{
    GF_SET(gVBlankCallback, cb);
    if (cb == NULL) {
        GF_SET(gVBlankCallback, InterruptCallbackEmpty);
    }
}

void InterruptCallbackCallHBlank(void)
{
    if (GF(gHBlankCallback) != NULL) {
        GF(gHBlankCallback)();
    }

    REG_IF |= INTR_FLAG_HBLANK;
}

void InterruptCallbackSetHBlank(ProcedureFunc cb)
{
    GF_SET(gHBlankCallback, cb);
    if (cb == NULL) {
        GF_SET(gHBlankCallback, InterruptCallbackEmpty);
    }
}

void InterruptCallbackCallVCount(void)
{
    if (GF(gVCountCallback) != NULL) {
        GF(gVCountCallback)();
    }

    REG_IF |= INTR_FLAG_VCOUNT;
}

void InterruptCallbackSetVCount(ProcedureFunc cb)
{
    GF_SET(gVCountCallback, cb);
    if (cb == NULL) {
        GF_SET(gVCountCallback, InterruptCallbackEmpty);
    }
}

void InterruptCallbackEmpty(void) {}

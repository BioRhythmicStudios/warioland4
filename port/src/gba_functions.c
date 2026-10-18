#include "port/gba_functions.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PortFunc PortLookupGbaFunction(unsigned int gbaAddress)
{
    unsigned int lo = 0, hi = gGbaFunctionCount;

    gbaAddress &= ~1u;
    while (lo < hi) {
        unsigned int mid = lo + (hi - lo) / 2;
        if (gGbaFunctions[mid].gbaAddress < gbaAddress)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (lo < gGbaFunctionCount && gGbaFunctions[lo].gbaAddress == gbaAddress)
        return gGbaFunctions[lo].func;
    return NULL;
}

const struct GbaFunction *PortGbaFunctionBelow(uintptr_t pc)
{
    const struct GbaFunction *best = NULL;
    unsigned int i;

    for (i = 0; i < gGbaFunctionCount; i++) {
        uintptr_t start = (uintptr_t)gGbaFunctions[i].func;
        if (start <= pc && (best == NULL || start > (uintptr_t)best->func))
            best = &gGbaFunctions[i];
    }
    return best;
}

void PortMissingFunction(const char *name, unsigned int gbaAddress)
{
    fprintf(stderr, "GBA function %s (0x%08X) has no native version yet\n", name, gbaAddress);
    fflush(stderr);
    abort();
}

// --- Function pointers held in GBA memory (include/gba/gba_ptr.h) ---------------------------

static struct GbaFunction *sByNative;

static int CompareNative(const void *a, const void *b)
{
    uintptr_t x = (uintptr_t)((const struct GbaFunction *)a)->func;
    uintptr_t y = (uintptr_t)((const struct GbaFunction *)b)->func;
    return x < y ? -1 : x > y;
}

PortFunc PortGbaFunctionToNative(unsigned int gbaAddress)
{
    PortFunc func;

    if (gbaAddress == 0)
        return NULL;
    func = PortLookupGbaFunction(gbaAddress);
    if (func == NULL) {
        fprintf(stderr, "No native function for the GBA function pointer 0x%08X\n", gbaAddress);
        fflush(stderr);
        abort();
    }
    return func;
}

unsigned int PortNativeFunctionToGba(PortFunc func)
{
    unsigned int lo = 0, hi = gGbaFunctionCount;

    if (func == NULL)
        return 0;
    if (sByNative == NULL) {
        sByNative = malloc(gGbaFunctionCount * sizeof(*sByNative));
        if (sByNative == NULL)
            abort();
        memcpy(sByNative, gGbaFunctions, gGbaFunctionCount * sizeof(*sByNative));
        qsort(sByNative, gGbaFunctionCount, sizeof(*sByNative), CompareNative);
    }
    while (lo < hi) {
        unsigned int mid = lo + (hi - lo) / 2;
        if ((uintptr_t)sByNative[mid].func < (uintptr_t)func)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (lo < gGbaFunctionCount && sByNative[lo].func == func)
        return sByNative[lo].gbaAddress | 1;  // Thumb
    fprintf(stderr, "The native function %p has no GBA address\n", (void *)func);
    fflush(stderr);
    abort();
}

void PortBadGbaPointer(unsigned long long address)
{
    fprintf(stderr, "Native address 0x%llX stored in GBA memory does not fit 32 bits\n", address);
    fflush(stderr);
    abort();
}

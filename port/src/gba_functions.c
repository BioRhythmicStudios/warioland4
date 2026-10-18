#include "port/gba_functions.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port/os.h"

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

#define MAX_RAM_FUNCTIONS 8

static struct {
    unsigned int gbaAddress;  // without the Thumb bit
    PortFunc func;
} sRamFunctions[MAX_RAM_FUNCTIONS];
static unsigned int sRamFunctionCount;

static int IsGbaRam(uintptr_t address)
{
    return address - 0x02000000u < 0x02000000u;  // EWRAM or IWRAM
}

void PortSetRamFunction(unsigned int gbaAddress, PortFunc func)
{
    unsigned int i;

    gbaAddress &= ~1u;
    for (i = 0; i < sRamFunctionCount && sRamFunctions[i].gbaAddress != gbaAddress; i++)
        ;
    if (i == MAX_RAM_FUNCTIONS) {
        fprintf(stderr, "More than %d functions copied to RAM\n", MAX_RAM_FUNCTIONS);
        fflush(stderr);
        abort();
    }
    if (i == sRamFunctionCount)
        sRamFunctionCount++;
    sRamFunctions[i].gbaAddress = gbaAddress;
    sRamFunctions[i].func = func;
}

// The entry for code copied to a RAM address, or -1. Its function is NULL when what was copied
// is not a function (open bus, say).
static int RamFunction(unsigned int gbaAddress)
{
    unsigned int i;

    gbaAddress &= ~1u;
    for (i = 0; i < sRamFunctionCount; i++)
        if (sRamFunctions[i].gbaAddress == gbaAddress)
            return (int)i;
    return -1;
}

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
    if (IsGbaRam(gbaAddress)) {
        int i = RamFunction(gbaAddress);
        func = i >= 0 ? sRamFunctions[i].func : NULL;
    } else {
        func = PortLookupGbaFunction(gbaAddress);
    }
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
    if (IsGbaRam((uintptr_t)func) && RamFunction((unsigned int)(uintptr_t)func) >= 0)
        return (unsigned int)(uintptr_t)func;  // already a GBA address: code copied to RAM
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
    uintptr_t pc = (uintptr_t)__builtin_return_address(0);
    const struct GbaFunction *target = PortGbaFunctionBelow((uintptr_t)address);
    const struct GbaFunction *caller = PortGbaFunctionBelow(pc);
    char message[384];

    // A game function stored as a data pointer (GP_SET) instead of with GF_SET, say.
    if (target != NULL && (uintptr_t)target->func != (uintptr_t)address)
        target = NULL;
    snprintf(message, sizeof(message),
             "Native address 0x%llX%s%s stored in GBA memory does not fit 32 bits, by the code at "
             "executable+0x%llX, in or after %s (GBA 0x%08X)\n",
             address, target != NULL ? ", the game function " : "", target != NULL ? target->name : "",
             (unsigned long long)PortOsExecutableOffset(pc), caller != NULL ? caller->name : "?",
             caller != NULL ? caller->gbaAddress : 0);
    PortOsWriteError(message);
    abort();
}

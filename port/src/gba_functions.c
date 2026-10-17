#include "port/gba_functions.h"

#include <stdio.h>
#include <stdlib.h>

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

void PortMissingFunction(const char *name, unsigned int gbaAddress)
{
    fprintf(stderr, "GBA function %s (0x%08X) has no native version yet\n", name, gbaAddress);
    fflush(stderr);
    abort();
}

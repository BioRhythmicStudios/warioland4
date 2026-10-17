#include <stdio.h>

#include "port/gba_functions.h"

int main(int argc, char **argv)
{
    unsigned int i;

    (void)argc;
    (void)argv;
    for (i = 1; i < gGbaFunctionCount; i++) {
        if (gGbaFunctions[i - 1].gbaAddress >= gGbaFunctions[i].gbaAddress) {
            fprintf(stderr, "dispatch table out of order at 0x%08X\n", gGbaFunctions[i].gbaAddress);
            return 1;
        }
    }
    printf("%u game functions linked\n", gGbaFunctionCount);
    return 0;
}

// Stub platform layer: just enough for the game code to link. The real Windows layer
// (window, renderer, audio, input, saves) replaces it.
#include <stdio.h>
#include <stdlib.h>

void PlatformSyscall(int num)
{
    fprintf(stderr, "PlatformSyscall(%d) is not implemented\n", num);
    abort();
}

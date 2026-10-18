// Stub platform layer: just enough for the game code to link. The real Windows layer
// (window, renderer, audio, input, saves) replaces it. BIOS calls are not platform code:
// they are in port/src/bios.c.

// ISO C wants at least one declaration in a translation unit.
typedef int PlatformStubUnused;

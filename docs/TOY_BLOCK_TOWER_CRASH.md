# Toy Block Tower environmental-tile fault (2026-10-22)

Windows Beta 3 crashed after the frog switch, Keyzer pickup and large red pipe
entry. The user's fault dialog reported `UpdateWarioEnvironmentalTiles+0x120`,
GBA function `0x0806FD74`, executable RVA `0x1ACD0`, and invalid read
`0x000000020201C7F0`.

The matching Beta 3 executable disassembles RVA `0x1ACD0` to the BG0 tile-map
halfword load at `src/tile_interaction.c:2201`. The BG0 pointer is
`0x0201C840`; the fault address corresponds to tile index `0xFFFFFFD8`
(-40). The original GBA computes the byte address with 32-bit wraparound and
reads `0x0201C7F0` in EWRAM. Native x86-64 pointer indexing instead added the
unsigned index in 64 bits and read unmapped `0x000000020201C7F0`.

The portable branch now calculates this lookup's byte address as `u32` before
dereferencing it. The original GBA expression remains unchanged. An opt-in
`--test-tile-wrap` fixture calls the actual game function with the same BG0
pointer, width and negative index. Before the fix it faulted at the exact
address from the user's dialog; afterward it passes on normal and ASan Windows
builds. The fixture is a matched memory state, not a reproduction of the full
Toy Block Tower route. The room ID and transition flags at the user's original
fault were not captured, so the user still needs to retest the complete route.

Normal and ASan builds pass the focused test, runtime/BIOS/I/O/save tests, CRT,
input/settings and rumble tests. A disposable 8500-frame Hall route produced
byte-identical 38,615,760-byte late-state dumps before and after the change
(SHA-256 `31875f07f706095771d05b2f9507fac3b8775c13b53b49331b559a6828765761`).
Fresh GBA build `build/v7-80` has ROM MD5
`940b597bef3be3f4041021b47a2f4096`, identical ROM bytes and all 202
object files identical to `build/us/obj`.

The private test build in Downloads has an isolated-save launcher. Its first
run copies the existing save and settings into `test-data`, and subsequent runs
use only those copies. It contains no ROM or Nintendo assets. The manual test
should enter and exit the pipe during the post-Keyzer escape, then repeat.

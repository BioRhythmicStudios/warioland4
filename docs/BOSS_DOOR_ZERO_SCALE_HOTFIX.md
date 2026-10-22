# Boss-door transition divide-by-zero (2026-10-22)

CM30's issue save exposes a native crash when entering a passage boss door. A
disposable copy of the 32 KiB save and a supported US ROM reproduced a Linux
`SIGFPE` at `FixedInverse`, called from `UpdateBossDoorOpeningSprites` line
1549. The argument was zero. The same input sequence then completed on Windows
and Linux after the fix. Original saves and ROMs were not edited.

The stage-select state machine switches from passage-screen mode 36 to
boss-door mode 41 at the end of a frame. It draws sprites using the new mode
before mode 41 initializes `gStageEntryMainSpriteState.second` on the next
frame. Its zero value reaches `FixedInverse(background->second >> 4)`. Native
x86 integer division raises `SIGFPE`. The GBA `__divsi3` at `0x08094DC8`
branches to `__div0` for a zero divisor and then returns zero at `0x08094E56`.

`FixedInverse` now returns zero for a zero divisor under `PORTABLE` only,
matching that GBA helper. All nonzero divisions, scene timing, save format,
and original GBA code remain unchanged. No broad bounds check or exception
suppression was added.

The focused sequence starts from a disposable copy of CM30's save, enters the
passage, holds Right to approach the boss door, and advances the opening scene.
Before the fix it exited with signal 8 between frames 2000 and 2050; afterward
it completed 2700 frames on both Windows and WSL2 Linux. The same path passed
Linux AddressSanitizer. The extracted private AppImage passed the sequence,
BIOS, I/O, and Toy Block Tower tile-wrap checks. Windows also passed those
checks plus the full credits regression, finishing at frame 15263.

Fresh Windows GBA build `build/v7-83` produced ROM MD5
`940b597bef3be3f4041021b47a2f4096`; its ROM and all 202 objects are
byte-identical to `build/us`. The updated AppImage is private and requires a
real Steam Deck boss-door retest. No Linux release has been pushed or published.

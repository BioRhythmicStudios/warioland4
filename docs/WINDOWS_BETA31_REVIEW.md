# Windows v0.3.1-beta private review (2026-10-22)

Source commit `a0328564535d7ff35c122491636d079a267c70e6` includes the
Toy Block Tower fix (`4b61e24`) and the user's successful Keyzer/large red pipe
escape playtest record. No tag, push or publication has been made.

## Focused address-arithmetic audit

The confirmed fault was unsigned tile index `0xFFFFFFD8` added to a BG0 map
pointer in 64-bit native arithmetic. A focused review of nearby BG map and
collision reads found no second confirmed instance of that failure:

- `src/transparency.c` indexes BG0 with signed `s32 alpha`, so a negative
  index subtracts from the low mapped pointer on x86-64 as it does on GBA.
- `src/block.c` checks x/y coordinates against room width/height before its
  BG1/BG2 collision map reads. `src/bg_clip.c` also checks map bounds and
  calculates the halfword address through `u32` arithmetic.
- `src/tile_interaction.c` has a separate BG1 lookup using `u16` coordinates.
  Its inputs were not shown to hit the same signed transition-edge case.

This is a scoped audit, not proof that every GBA address expression is safe.
No additional gameplay code was changed without a reproducer.

## Build and verification

The package was built from a Git archive of the committed source into a fresh
`build/release-v0.3.1-beta-20261022/native` directory. Windows x64/UCRT,
llvm-mingw Clang 23.1.3, SDL3 3.4.18, CMake Release `-O3 -DNDEBUG`, ASan Off.
Both game and launcher are fresh binaries. DLL imports close over the bundled
SDL3.dll plus Windows system/UCRT libraries. No ROM, save, settings, extracted
Nintendo assets, developer test executables or removed artwork are packaged.

Clean Release checks passed: tile-wrap reproducer, coroutine/BIOS/I/O/save
tests with disposable data, settings/keyboard/virtual-controller tests, rumble
event tests and CRT software rendering. The 8,500-frame Hall route with the
retained input script produced the same 38,615,760-byte late-state dump as
Beta 3 (SHA-256
`31875f07f706095771d05b2f9507fac3b8775c13b53b49331b559a6828765761`).
Focused ASan tile-wrap and runtime/save checks also pass. The ASan binaries
need the existing LLVM runtime directory on PATH; an initial attempt without
that directory failed to load before any test ran.

Fresh original GBA build `build/v7-81` has MD5
`940b597bef3be3f4041021b47a2f4096`; its ROM and all 202 object files are
byte-identical to `build/us`. The ZIP contains 20 allowlisted files; CRC and
extracted hashes match. Its copied executable passed tile-wrap and 120-frame
smoke checks from the Downloads review folder with disposable data.

Private review ZIP:
`build/release-v0.3.1-beta-20261022/WarioLand4-Native-PC-Port-v0.3.1-beta-Windows-x64.zip`
(2,894,573 bytes, SHA-256
`edcc81af9151a1b1f143f22a052e836c8a3147265313dc04bf51890941f239b8`).
A copy, checksum, manifest and extracted folder are in
`C:\Users\Benny\Downloads\WarioLand4-v0.3.1-beta-review` for user review.

The full Toy Block Tower route passed in the earlier private fix build, which
has the same gameplay source. The final ZIP's GUI launcher and real gameplay
have not yet been manually retested; avoid claiming that from headless checks.
The next step is user review and package playtest. Do not push, tag or publish
without explicit approval.

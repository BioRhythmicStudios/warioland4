# Wario Land 4 PC port plan

Status: decided 2026-10-17. M0 steps 1–6 are done on the `port` branch (see [Where we are](#where-we-are-2026-10-17)). Targets: Windows first, then Linux, then Android.

## Summary

The port is a 64-bit build of the decompiled C with a replacement platform layer.

- **Memory.** GBA memory lives in one reserved block. Every translation is written as `base + addr`, and the port runs with `base = 0`: the block sits at the real GBA addresses.
- **Pointers.** Pointers stored in game memory stay 32-bit GBA addresses (`GbaPtr`), so RAM is byte-identical to the GBA and can be diffed against mGBA every frame. Address arithmetic the code does through `u32` keeps working unchanged, as long as `base = 0` holds.
- **Assets.** The user's own ROM is loaded into the block at runtime, and the executable contains no ROM bytes.
- **Parallel work.** The renderer, mixer, input and saves are built against state dumped from mGBA, without waiting for the game code to boot.

## Prior art

| Project | Platform | Graphics | Sound | Assets | Pointers |
|---|---|---|---|---|---|
| [SAT-R/sa2](https://github.com/SAT-R/sa2) (Sonic Advance 2) | SDL2, `PORTABLE` macro in the same tree | Software renderer adapted from gpSP (GPL), 426×240 option | m4a mixer and PSG rewritten in C | Compiled in (user builds it) | Native 64-bit, registers become arrays |
| [MatheoVignaud/tmc](https://github.com/MatheoVignaud/tmc) (Minish Cap) | SDL3 | Own scanline software renderer | agbplay (high-level m4a player) | Extractor run on the user's ROM, which is also read at runtime | Native 64-bit, GBA addresses translated by helpers |
| [Kurausukun/pokeemerald pc_port](https://github.com/Kurausukun/pokeemerald/tree/pc_port) | SDL, Windows/Linux | Software | C mixer | Built from source | Origin of sa2's SDL layer |
| [EmeraldRecomp](https://raw.githubusercontent.com/mstan/EmeraldRecomp/HEAD/README.md) | gbarecomp runtime | Modelled PPU | Modelled APU and m4a | Never ships the ROM | Static recompilation, not a decomp port |

Lesson: the hard part of a 64-bit port is ROM data that stores 4-byte pointers. tmc's port spends most of its workarounds there.

## What the WL4 decomp looks like from a porting angle

- **BIOS:** 3 calls: `CPUSet` and `LZ77UnCompVram` (`asm/syscalls.s`), and `Halt` (`src/main.c`, plus a raw `asm("svc 2")` in `src/cutscenes.c`).
- **Register pins and barriers:** 3,299 register pins (now `PIN()`, see M0), and about 680 empty `asm("" : "+r"(x))` barriers. The barriers are valid GCC/Clang code.
- **Data:** 2,862 `baserom_blob` includes cover 6.76 of 8 MB. Inside them are about 21.5k ROM-pointer-like words and 381 Thumb function pointers.
- **RAM globals:** 543 IWRAM globals placed by `linker.ld`. The highest non-absolute IWRAM symbol is at `0x03006240`, leaving about 7 KB for the GBA's two stacks below `0x03007E60`.
- **Pointer fields:** 50 aggregates with 121 pointer fields (119 live in game memory), plus 92 extern pointer tables.
- **Sound:** `m4a.c` is C, but the mixer and sequencer core is ARM asm (`asm/m4a_asm.s`).
- **Video:** about 25 per-scanline HBlank effects. The interrupt handler is copied into IWRAM.
- **Save:** 32 KB SRAM.

## Memory model (decided)

- **Block.** The GBA regions are reserved at their real addresses: EWRAM `0x02000000`, IWRAM `0x03000000`, I/O, palette, VRAM, OAM, ROM `0x08000000` and SRAM `0x0E000000`. That's about 8.5 MB committed, with unmapped gaps that fault so stray addresses are caught. The ROM pages are read-only. If a reservation fails, the port stops with a clear error.
- **Translation.** It is always written as `base + addr`, through `GBA_PTR()`/`GBA_ADDR()` accessors and the hardware-address macros, with `base = 0`.
- **Pointers in game memory.** These use `typedef struct { u32 a; } GbaPtr;`. A distinct type means every missed use is a compile error. Function pointers in game memory keep their original Thumb address and are called through a table generated from the ELF symbols (GBA address → native function).
- **Every data object lives at its GBA address.** RAM globals, linker-placed globals, blob labels, and all C-defined ROM data (including the tables in `src/data/` and the game sources) resolve to their GBA address in the block or ROM image, through absolute symbols generated from the ELF (`port/generated/gba_symbols.s`). The executable defines no game data. The only exception is native function-pointer tables, which are code, not assets. This also keeps pointer equality consistent between data read from the ROM image and symbols referenced directly.
- **Casts are left alone.** The ~6,200 pointer↔integer casts and ~1,100 constant hardware-address casts are lossless while `base = 0` and every native address the game touches stays below `0x80000000` (some loops compare addresses as `s32`). If a platform ever forces a different base, those sites need fixing. That's the known cost of this route, not a surprise.
- **Rejected: a 32-bit scaffold.** Android has no x86-32 build, the migration would be the same edits later, and about 1–2 weeks of 32-bit-only work would be thrown away.

### Native-memory cast audit (2026-10-17)

The audit was a libclang pass over every game source. Each local variable's origin was traced by a fixed-point pass over its assignments, and each data symbol was classified by its real address in the ELF. Tools and output are in `reports/port_gbaptr_dryrun/` (`cast_provenance.py`, `extern_split.py`, `provenance/`).

| Origin of the cast's address | Lines | Verdict with `base = 0` |
|---|---|---|
| Data symbols at ROM addresses | 2,918 | Fine (ROM image) |
| Data symbols at RAM addresses | 2,676 | Fine (block) |
| Absolute linker symbols (`gNumMusicPlayers`, `gMaxLines`, `SoundMainRAM`) | 5 | Fine or replaced |
| Values loaded from memory, call results, constants | 433 | Fine |
| **Native stack** (locals, `&local`, local arrays) | **204 in 16 files** (about 100 source casts plus follow-on uses) | **Truncates unless the stack is low** |
| **Native function addresses** | **3** (`m4a.c:487`, `sram.c:25,67`) | Replaced by platform code |

The stack sites fall into four patterns:
- DMA fill values in locals (`dma[0] = (u32)&fillValue;`, and the `DmaFill16/32` macros), about 60 lines.
- Address arithmetic on local arrays through `u32` (`scrolling.c`, `camera.c`, `file_select.c`, `tile_interaction.c`, `credits.c`, and the `locals` struct in `normal.c`).
- Loop ends tested by comparing addresses as `s32`.
- Pointers to locals stored in GBA-style DMA register blocks.

### Game stack (decided)

The game code runs on a coroutine with its own native stack. That stack is allocated in low memory: 8 MB, outside every GBA region (for example `0x01000000`–`0x01800000`), with guard pages, below `0x80000000`.

- With this, all 204 stack lines work unchanged, and no matched code has to change.
- The coroutine also turns `AgbMain`'s endless loop into one frame per resume, which Android and a host-owned main loop need, and which makes headless frame stepping and soft reset straightforward.
- Implemented in M0 step 6 with a patched minicoro (`port/third_party/minicoro/`). The stack is `0x01020000`–`0x01810000`, with a guard page, 60 KiB of overflow headroom and inaccessible blocks around it. minicoro keeps the Windows TIB stack bounds, and the patches add correct ASan fiber annotations.

The stack does **not** go in IWRAM itself:
- Only about 7 KB is free there, while native x86-64 and arm64 frames, ASan redzones and OS/SDL calls need far more. An overflow would silently corrupt IWRAM globals.
- Native frames never match agbcc's, so the diff harness masks the stack area whatever its location.

### GbaPtr dry run (2026-10-17)

The check was a host syntax check of every source file with 64-bit GCC 15. Error sites count unique file:line locations in game code, excluding `src/data/`.

| Run | New error sites (game code) |
|---|---|
| Baseline (no conversion) | 178, all agbcc-isms (170 are one incomplete `struct AnimationFrame` array type) |
| 119 struct pointer fields → `GbaPtr` | 1,827 in 116 files (275 are static ROM-data initializers that move out of the build) |
| Plus 92 pointer tables → `GbaPtr` | 1,917 in 120 files |

## Key decisions

| Decision | Choice | Trade-off |
|---|---|---|
| Pointer width | 64-bit, `GbaPtr` u32 fields in game memory | About 1.6k sites up front |
| Block placement | Accessors written as `base + addr`, run with `base = 0` | About 7.3k cast sites need fixing if a platform forces another base |
| Data placement | Every data symbol at its GBA address. No game data in the exe | A generator maps ELF symbols to absolute symbols, and data defined in `src/` sits behind `#ifndef PORTABLE` |
| Game stack | Coroutine with an 8 MB low native stack outside the GBA regions | Needs a portable context switch |
| Assets | Runtime load of the user's ROM, MD5-verified | A mod/HD override layer comes later |
| Graphics | Own scanline software renderer, with mGBA as the oracle (or adapt mGBA's MPL-2.0 renderer) | gpSP via sa2 is port-ready but GPL |
| Sound | The game's own m4a engine, asm ported to C (starting from sa2's mixer), on `GbaPtr` structs | agbplay sounds better but behaves differently |
| Platform library | SDL3 | sa2's code is SDL2 |
| Code base | `port` branch, `PORTABLE` macro. The GBA build stays byte-identical | Some `#if` noise. Re-run codemods after merging `decomp-wtf` |
| Compiler | Clang via llvm-mingw on Windows (x86-64), Clang/GCC on Linux, NDK Clang on Android, CMake + Ninja. Not MSVC | — |
| Timing | Vsync-locked on 60 Hz displays, fixed-timestep accumulator otherwise | Exact 59.73 Hz stutters on 60 Hz panels |

## Parallel workstreams against mGBA dumps

An mGBA Lua script dumps VRAM, OAM, palette, I/O (per scanline where HBlank effects run) and the m4a RAM state from the real ROM. Each subsystem is tested against those dumps without needing the game code to boot:

- **Renderer.** Feed the dumped VRAM, OAM, palette and per-line I/O in, and compare the output pixel for pixel with mGBA's frame. It covers text and affine BGs, 4bpp and 8bpp tiles, 128 sprites (affine, double-size, semi-transparent), WIN0, WIN1 and OBJ windows, alpha and brightness blending, mosaic, and the affine reference-point latch.
- **Mixer.** Feed the dumped `SoundInfo` and track state into the C mixer and compare the next frame's state. PCM goes to an SDL3 `SDL_AudioStream`. It must match WL4's m4a variant (8 players, its sample rate).
- **Input.** SDL3 keyboard and gamepad map to the active-low `KEYINPUT` bits. Bindings are configurable, opposite D-pad directions are filtered, and input is recorded per frame.
- **Saves.** A 32 KB SRAM region persisted to `wl4.sav` in `SDL_GetPrefPath()` with an atomic write. The format is compatible with emulator saves.

## Assets from the user's ROM

- The ROM is checked by MD5 (US/EU `5fe47355…` first, JP `99c8ad…` later with its own symbol map) and copied into the ROM region at `0x08000000`.
- CI fails if any 32-byte ROM window appears in the executable.
- ROMs edited with WL4Editor (data-only changes) would likely work behind an opt-in setting.

## M0: headless boot on Windows x64

The invariant for every change to shared game code: a clean rebuild gives a ROM and object files byte-identical to the build before the change. (`make` still fails its MD5 check while 11 functions remain unmatched. Once matching completes, `make` passing becomes the invariant.)

1. **PIN() macro.** Done, commit `9569554`. 3,299 pins in 113 files, verified byte-identical. `tools/port_pin_codemod.py --check` finds leftovers after merges.
2. **Remaining non-portable asm.** Done (`bc5083b`; `ba49d50` on `decomp-wtf`). `asm("svc 2")` became `SYSCALL(SYSCALL_Halt)`, and 676 inline barriers go through `ASM_BARRIER()`/`ASM_BARRIER_VOLATILE()` (`tools/port_asm_barrier_codemod.py`).
3. **The 178 host-compiler errors.** Done (`94c796f`; `d602a7b` on `decomp-wtf`).
4. **Build skeleton.** Done (`d11e8a1`, `9dfd1c6`). CMake + Ninja with llvm-mingw Clang 23 for x86-64. The source changes, all byte-identical for the GBA, are:
   - 504 data definitions moved behind `#ifndef PORTABLE` (`tools/port_data_codemod.py`).
   - `AGB_ALIGN` on all 196 struct/union definitions, because agbcc aligns every record to 4 bytes (`tools/port_struct_align_codemod.py`).
   - `SYSCALL()` → `PlatformSyscall()`.
   - Missing includes and prototypes.
   - `RETURN_NO_VALUE()` for 7 non-void functions that return nothing.

   The layout asserts check 73 pointer-free records against agbcc now. 42 records with pointers wait for GbaPtr (`PORT_CHECK_POINTER_LAYOUTS`). The PC build needs no extracted assets.
5. **Symbol generator.** Done (`50b45d8`). `port/tools/gen_symbols.py` turns the playtest ELF into three things:
   - 8,649 absolute data symbols, including the linker constants.
   - A dispatch table of 4,022 functions.
   - Weak stubs for the 13 functions still in asm.

   Clang reaches extern data through 64-bit `.refptr` slots, so the exe keeps the default image base and ASLR. `wl4.exe` links with nothing unresolved.
6. **Memory block and ROM loader.** Done (`6dfa2fb`, `3661c9c`, `2bf1cb3`). This step did the following:
   - Reserved the regions at their GBA addresses and loaded the ROM with an MD5 check, read-only.
   - Added a vectored fault handler that reports the GBA address and region, ROM writes and game stack overflows.
   - Put the game coroutine on the low stack.

   The coroutine passes its self-test with and without ASan (including fake stacks).
7. **GbaPtr conversion.** 119 fields and 92 tables (about 1.9k sites), plus the `m4a.c` and `sram.c` function-address paths replaced under `PORTABLE`.
8. **BIOS.** `CpuSet` and `LZ77UnCompVram` (unit-tested on every compressed blob). `Halt` becomes `Platform_WaitVBlank()`, which yields from the game coroutine.
9. **DMA and I/O.** DMA macros become immediate copies, and DMA register writes made through structs trigger the transfer. HBlank/VBlank-timed transfers are queued. VCOUNT and DISPSTAT are maintained.
10. **Frame loop and diff runner.** Headless frame loop on the game coroutine, per-frame memory dump, an mGBA Lua dumper, and a diff runner aligned on the game's frame counter that masks the stack. Exit criterion: identical memory through boot and the idle title screen.

Steps 1–3 change shared code under the byte-identical invariant and can merge back to `decomp-wtf`. Steps 4–10 are port-only. The renderer, mixer, input and save workstreams run alongside. Step 4's source changes are byte-identical too, but so far they are only on `port`.

### Where we are (2026-10-17)

- **Done:** M0 steps 1–6, pushed to `myfork/port`. Steps 1–3 are also on `myfork/decomp-wtf`.
- **Half-finished:** nothing in progress. Known gaps, all for step 7 or later:
  - 179 `static` GBA functions have no native symbol, so a ROM function pointer to one cannot be dispatched yet.
  - Structs defined inside `.c` files (23 files) have no layout asserts.
  - The fault handler prints a native pc, not a game function name.
- **Next:** step 7, the GbaPtr conversion. It must also cover what steps 4–5 turned up:
  - `sColorBlendFunctionTable` in `color_fading.c` is indexed with a 4-byte stride through `(u32)table`, across two separately defined arrays.
  - Some declarations differ from their definitions, and on x86-64 the difference matters:
    - `wario/matched_helpers.h` declares the u8 `ResolveWario*Collision` functions as returning `s32`.
    - `snowman.c` declares `CheckWarioPointCollision(u8, u16, void *)`.
    - Four header pairs conflict with each other: `save_reset.h`/`save_data_clear.h`, `money_sunflower.h`/`score.h`, `stage_entry.h`/`boss_door_opening.h`, and `matched_helpers.h`/`normal.h`.
  - `irq_handler` is copied as data by `InitHelpers`, and `SoundMainRAM` is copied to RAM. Both paths need platform replacements.
  - 302 compiler warnings remain (pointer-type mismatches and int↔pointer conversions). The 7k cast warnings are hidden unless `WL4_CAST_WARNINGS=ON`.

## Later milestones

- **M1 (first visible milestone), pixel-exact title screen on Windows.** SDL3 window, renderer, input and pacing. Exit criteria: the framebuffer matches mGBA, Start reaches file select, and the sanitizers are clean.
- **M2.** Audio, saves, and the first passage with HBlank effects and affine.
- **M3.** Full playthrough under the diff harness, using TAS input movies (expect some lag-frame desyncs).
- **M4.** Linux x64.
- **M5.** Android arm64: verify the low-address reservations (below ART's default heap at about `0x12C00000`), add touch controls, use the NDK build.
- **M6.** Configuration, filters, the JP version, and widescreen experiments.

## Licence

- `TsilaAllaoui/warioland4` (upstream `origin`) states no licence. It has no LICENSE file, GitHub reports none, and the README is silent.
- It is a GitHub fork of [lilDavid/warioland4](https://github.com/lilDavid/warioland4), which is **MIT**, "Copyright (c) 2025 lilDavid" (initial commit `27b90b2`, 2025-05-27).
- TsilaAllaoui deleted the LICENSE in commit `c1cc627` ("-Cleaning", 2026-09-30).
- lilDavid's portions remain MIT. Their LICENSE is restored byte for byte in this fork (commit `c4f568f`, on `decomp-wtf` and `port`).
- TsilaAllaoui's own additions have no stated licence, which by default means all rights reserved. An issue asking them to state one, ideally MIT, is drafted but not yet posted.
- Our own and decomp.wtf commits can be licensed as we choose.
- Borrowed port code: mGBA (MPL-2.0) fits under MIT. gpSP (GPL) would make the port GPL. agbplay's licence needs checking. minicoro (vendored, patched) is Public Domain or MIT-0. Its asm context switch is from LuaCoco (MIT, Mike Pall), whose notice must ship with binaries. The agbcc submodule is a GPL build tool and doesn't affect the port.
- Not legal advice. Releasing binaries still carries Nintendo IP risk even with no assets inside.

## Risks

- Hidden undefined behaviour in matched fakematch code. Mitigation: the diff harness and sanitizers.
- Fixed reservations failing on some machines (detected, with a clear error). Android is unverified.
- The coroutine context switch interacting with Windows stack checks or ASan. Checked on Windows x64: stack overflow is reported, and ASan sees the coroutine stack. Linux and Android are still to verify.
- WL4's m4a variant differing from sa2's mixer.
- The licence gap above.

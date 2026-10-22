# Wario Land 4 PC port plan

Status: decided 2026-10-17. M0, M1 and M2 are done on the `port` branch (see [Where we are](#where-we-are-2026-10-21)). Targets: Windows first, then Linux, then Android.

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
- **Pointers in game memory.** These are `GBA_PTR(T *)` and `GBA_FUNC_PTR(F)` (`include/gba/gba_ptr.h`). On the PC each is a packed 4-byte union `{ u32 a; T t_[0]; }` that keeps the pointee type, read with `GP()`/`GF()` and written with `GP_SET()`/`GF_SET()`. On the GBA they expand to the original types and expressions. A distinct type means every missed use is a compile error. Function pointers in game memory keep their original Thumb address and are called through a table generated from the ELF symbols (GBA address → native function).
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

   The layout asserts check 73 pointer-free records against agbcc now. 42 records with pointers waited for GbaPtr. Since step 7, all of them are checked except the unused `FlashSetupInfo`. The PC build needs no extracted assets.
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
7. **GbaPtr conversion.** Done (`2d5f8f2` to `06431ec`). Every commit was verified byte-identical for the GBA.
   - `GBA_PTR`/`GBA_FUNC_PTR` with the `GP`/`GP_SET`/`GF`/`GF_SET` accessors, plus `GBA_TYPEOF`/`GBA_AS` to name the union types on the PC (`include/gba/gba_ptr.h`).
   - Struct pointer fields, globals and pointer tables in game memory (`port/tools/gbaptr_rewrite.py`, now 0 targets left). Locals and casts that walk pointer tables (`port/tools/gbaptr_derived.py`, now 0 edits left).
   - Hand fixes:
     - `SOUND_INFO_PTR` and `INTR_VECTOR`.
     - `irq_handler`'s IWRAM copy, taken from the ROM image.
     - `gMPlayJumpTable`, now in IWRAM.
     - `SoundInfo.plynote`.
     - `sram.c`'s code-on-stack calls, which the PC build makes directly.
     - The colour-blend table, which spans two arrays.
     - `sSecondarySpriteAITable`.
   - Calls through mismatched signatures. `port/tools/check_signatures.py` compares every declaration and direct call, including calls through cast function types, with the definition in LLVM IR, and ignores bits that the callee or caller masks off. It found 19 functions. They are fixed under `PORTABLE` (`GBA_CALL_AS` for the casts), and none are left. `snowman.c`'s `CheckWarioPointCollision(u8, u16, void *)` is harmless, because the definition masks the arguments.
   - Layout asserts cover 147 records, including 33 structs defined in game sources (`port/generated/layout_checks/`, compiled but never linked).
   - The dispatch table now includes the asm stubs and every function's name. Fault reports name the nearest game function.
8. **BIOS.** Done (`0a30e0e`). The BIOS calls run in `port/src/bios.c`:
   - `CPUSet` follows the hardware.
   - `LZ77UnCompVram` writes halfwords and reads earlier output back from VRAM, so the distance-1 quirk behaves as on the GBA.
   - `Halt` yields from the game coroutine, one resume per frame.
   - m4a's `umul3232H32` is in `gba_runtime.c`.
   - `wl4 --test-bios` checks all of these. It also decodes all 50 LZ77 streams the game passes to `LZ77UnCompVram` (630 KB) and compares them with a plain byte decoder. It passes with and without ASan. None of the game's streams hits the distance-1 quirk.
9. **DMA and I/O.** Done (`3933d98`). The game writes the registers through the `REG_` macros, through structs and through about 400 addresses it computes at run time. So instead of changing that code, the port traps every access to the register page:
   - The page is a section with two views. The game's view at `0x04000000` has no access rights; each access faults, and the handler single-steps the instruction with the page open. The host's view never traps.
   - `port/src/gba_io.c` gives the registers their effects:
     - DMA: immediate transfers run at once; VBlank/HBlank transfers are queued for `PortIoVBlank()`/`PortIoHBlank()`, with repeat and destination reload.
     - IF write-1-to-clear, including `REG_IF |= flag`.
     - VCOUNT/DISPSTAT advance by half a line per game read, so scanline waits end.
     - Read-only registers are restored after writes.
   - `wl4 --test-io` covers all of this. It passes with and without ASan.
   - A trapped access costs about 4.3 µs, so 1,000 accesses a frame would cost about 4 ms.
10. **Frame loop and diff runner.** Done (`bf33bab`, with the barrier fix `532af6f`). `wl4 --frames N --dump FILE` runs the game headless; `port/tools/mgba_dump.lua` makes the same dumps in mGBA; `port/tools/diff_frames.py` compares them on main-loop steps. Exit criterion met: memory matches mGBA through boot, the intro and the idle title screen (1,756 steps, port frames 1–1764).

Steps 1–3 change shared code under the byte-identical invariant and can merge back to `decomp-wtf`. Steps 4–10 are port-only. The renderer, mixer, input and save workstreams run alongside. Step 4's source changes are on `decomp-wtf` too (`4d7db63`). Step 7's are byte-identical as well, but they only serve the PC build, so they stay on `port`.

### Where we are (2026-10-21)

Boss-door transition native hotfix (2026-10-22):
Local fix commits: Windows port 97be0bc; Linux port bd88af0.
A disposable copy of CM30's post-ending save reproduced SIGFPE in
FixedInverse on the first boss-door frame. The native renderer drew mode
41 before its scale was initialized; the GBA division helper returns 0
for this zero divisor. A PORTABLE-only zero case now preserves that
behavior. Windows, WSL2 Linux and extracted private AppImage boss-door
runs completed 2700 frames; Linux ASan passed. Windows credits, BIOS,
I/O and Toy Block Tower tests passed. Fresh Windows build/v7-83 matched
the original ROM MD5 and all 202 objects. See
docs/BOSS_DOOR_ZERO_SCALE_HOTFIX.md. The updated private AppImage awaits
real Deck retesting; next, confirm boss entry and continue the ending.

Golden Diva credits native-port hotfix (2026-10-22):
Local fix commits: Windows port 8c07329; Linux port 509c45c.
The VCount callback HBlank wait deadlocked in native IRQ emulation.
Windows and Linux now advance that one native-only wait. Full credits
from the verified issue-save copy completed at frame 15263 on both;
Linux ASan and the extracted private AppImage candidate passed.
Windows fresh build/v7-82 matched the ROM and all 202 objects; Linux
build/v7-86 could not run because the protected agbcc symlink target is
unavailable. Do not change tools/agbcc.
Fix and tests: docs/CREDITS_ENDING_HOTFIX.md. AppImage candidate is
private, unpublished and still needs real Deck ending playtest.
Next: CM30/user retests actual Golden Diva-to-credits transition;
then resume AppImage release readiness and final artifact review.

Linux shortcut correction (2026-10-22): the Steam Deck Dolphin error was
caused by the Desktop Entry parser treating the literal percent in
`${0%/*}` as a field-code marker. Both play and diagnostic shortcuts
now escape it as `%%`, pass `%k` separately, and invoke their sibling
scripts through an explicit shell. GIO launched both shortcuts from a
folder with spaces; play_gui.sh and native gameplay code were unchanged.
Fresh build/v7-85 has ROM MD5 940b597bef3be3f4041021b47a2f4096
and all 202 objects byte-identical to build/us/obj.
Next: real Deck double-click acceptance, then save/load, CRT, fullscreen
and rumble checks. No Linux push or release.

Linux graphical launcher preview (2026-10-22): the Deck standalone B-button
closure was confirmed as Desktop Layout mapping B to Escape; Gamepad Mode fixed
it without changing game input code. A native SDL3 Linux front window now
reuses the Windows purple/gold pixel style and the existing shared Settings
editor. It validates/recalls the user's US ROM, edits bindings, scale,
fullscreen startup, VSync, CRT and rumble, and starts the packaged game with
package-local userdata. The Linux Settings window scales to fit Deck-height
displays. The diagnostic launcher remains available with disposable saves.
Prior local commits: a3c2a59 diagnostics, 7a3cabe Toy Block Tower fix,
c239a3a Gamepad Mode documentation/package, and a45b30b native Linux
launcher. Windows v0.3.1-beta is untouched.
Normal and ASan headless BIOS/I/O/save/Toy Block Tower tests pass; input,
rumble and software/OpenGL CRT tests pass. Extracted WSLg UI automation
opened the launcher and ROM, saved Settings, launched gameplay, sent keyboard
input, and returned to the launcher after Escape. Hardware Deck testing of
the launcher and save/load/CRT/fullscreen/rumble remains pending; Steam
Machine testing is deferred by the user. Fresh build/v7-84 matched the
original ROM MD5 and all 202 objects byte for byte. The committed
launcher-preview archive was extracted outside the worktree and passed native
ELF/library and WSLg launcher/settings/game checks; the verified archive and
SHA-256 are in the user's Windows Downloads folder. Next: obtain real Deck
save/load, CRT, fullscreen and physical rumble feedback, then prepare a clean
v0.1.0-beta candidate. No Linux release is published or pushed.

Linux first playable build (2026-10-21), local and unpublished:
linux-port starts at verified Windows v0.3.0-beta commit
970cdd91580a4cbb7d0a80517df251b2aa0af5b0. Worktree:
/home/benny/Decomp/warioland4-linux (locked against accidental pruning).
Implementation commit: 7906092 (first native Linux x86-64 port).
No tag, push or release. Windows port remains at Beta 3; its
runtime sources, release binaries and existing local notes are preserved.
See docs/LINUX_FIRST_BUILD.md in the Linux worktree for commands and evidence.
Ubuntu 26.04.1 / WSL2 x86-64 now has Clang 21.1.8, CMake 4.2.3,
Ninja 1.13.2 and SDL3 3.4.2. The approved apt installation used
--no-install-recommends; no additional sanitizer package was installed.
Native ELF: build/linux/wl4. Linux fixed mmap/mprotect, shared I/O aliases,
SIGSEGV/SIGTRAP redirection, alternate signal stack and POSIX file operations
are implemented. The ET_EXEC image starts at 0x40000000 to keep the randomized
brk heap clear of GBA memory; 24 repeated startups pass. Shared game sources
and generated symbols are unchanged. A shared coroutine-owner leak was fixed.
Normal and Linux ASan BIOS/I/O/coroutine/save/input/event tests pass. The
8500-frame Hall route matches retained Windows Beta 3 SRAM, late pictures and
audio exactly; 40 captured late memory steps match with existing masks.
ASan uses installed GCC libasan with Clang instrumentation. High fake stacks
are disabled to preserve 32-bit DMA addresses; intentional open-bus unit
probes are uninstrumented. Default shadow protection remains enabled, and
the deliberate stack-buffer overflow is detected. No whole-game claim.
Visible WSLg X11/OpenGL gameplay reaches Hall room 11: 8528 frames in
142.782 seconds, 8507 main-loop steps, zero clock resets, zero audio drops,
two audio refills total. PulseAudio monitor contains nonzero game audio.
Keyboard A press/release and F11 fullscreen toggles pass. Software/OpenGL
CRT checks pass. Earlier transient WSLg audio stalls remain documented;
physical controller/rumble and real Linux desktop/Steam Deck tests are pending.
Separate Windows normal/ASan builds (including launcher) and runtime/input/
event tests pass. All automated game tests use explicit disposable save and
settings paths. The supplied ROM is read-only and retains its supported MD5.
GBA checks v7-72 through final v7-75: ROM-SAME, all 202 objects OBJ-SAME,
MD5 940b597bef3be3f4041021b47a2f4096. These builds use the Windows worktree's
unchanged GBA inputs (equal after newline normalization). Final checkpoint v7-76 outputs live in the Linux worktree: ROM-SAME and all
202 objects OBJ-SAME; next fresh: v7-77. The legacy make-check checksum target
uses hardcoded build/us; explicit baseline comparisons passed.
Normal and ASan checks also pass new partial-write, failed-rename and UTF-8
save tests. See build/evidence/checkpoint. Implementation commit 7906092 and
handover commit 094f2c1 remain local; no push or publication.

Private Steam Deck Desktop Mode test package (2026-10-21): packaging commit
21d2c5f adds a reproducible SDL3 3.4.2 compatibility build, a portable Linux
ELF bundle, launcher, settings helper, licenses, and exact Deck instructions.
The package includes SDL3 but uses host glibc and the host ELF loader. Game
and SDL3 require GLIBC 2.34 and 2.38 respectively. WSLg extracted-package
checks passed 400+50 headless frames, save reload, all six CRT/VSync variants
at about 59.7 Hz, and measured nonzero PulseAudio samples. Synthetic X11
keyboard input reached GBA A after focus retries; WSLg focus makes this probe
intermittent. A real Steam Deck has not yet been tested. Final fresh GBA check
v7-77: ROM-SAME, all 202 objects OBJ-SAME, MD5
940b597bef3be3f4041021b47a2f4096. No ROM, save, or extracted artwork
is in the package. Next: user tests the private archive on a physical Deck,
especially controls, audio, VSync/CRT, saves, and rumble; collect its SteamOS
version and loader check. No Linux graphical launcher yet.

The real Deck log used the earlier provisional archive that bundled Ubuntu
glibc/loader, and showed no default audio device. The user's Deck also has no
audio device in system settings. Start exit status and backtrace were absent.
The revised private deckfix archive uses host glibc/loader, includes SDL3
with dynamic PipeWire/PulseAudio/ALSA backends, and adds a graphical diagnostic
launcher with disposable save/settings and actual game exit-code logging.
WSL2 700-frame two-Start comparisons match across old and host-glibc builds;
WSLg extracted gameplay survived 825 visible OpenGL frames with reported
VSync and zero audio drops. Normal/ASan and input/rumble-event tests pass.
Fresh GBA v7-78 is ROM-SAME, all 202 objects OBJ-SAME, MD5
940b597bef3be3f4041021b47a2f4096. Next: retest this exact new package on
a physical Deck, return its one diagnostic log, then investigate any
Deck-specific Start fault. Deck diagnostic implementation commit ae62f34 is local; the latest
handover documentation will be committed separately. Nothing is pushed or
published.


Windows v0.3.0-beta release preparation (2026-10-21), The Rumble Update:
user manually confirms completed native rumble gameplay and authorizes verified
integration/publication. Rumble commit 20fc6e4 follows cleaned Beta 2 d777f81.
Fetched myfork/port has no remote-only commits; integrate by fast-forward without
losing local notes. Original SDL3 effects, remapping, CRT and VSync are preserved;
colour restoration mod is excluded. No new hardware coverage is inferred.
Normal/ASan event/settings/runtime and CRT checks pass; bounded 8,500-frame Hall
normal/ASan/Beta2 save and late picture/audio outputs match. GBA v7-68 (also
v7-67) must remain ROM-SAME and all 202 objects OBJ-SAME, MD5
940b597bef3be3f4041021b47a2f4096; next fresh v7-69.
Package from a clean checkout of this final commit and gate public push/tag/
prerelease on external portable-package acceptance. Preserve current saves,
settings, previous releases, all worktrees and unrelated local notes. See
docs/WINDOWS_BETA3_RELEASE.md and build/release-v0.3.0-beta-20261021 evidence.
Next after successful Windows publication: report release hashes/status and
propose Linux work on a separate branch/worktree. Do not create it or implement
Linux code yet.


Native rumble verified milestone (2026-10-21), user-authorized local commit:
this commit adds optional SDL3 gamepad rumble on feature/native-rumble, based on
published Beta 2 source d777f81 (after 831151f and 909d998). Rumble defaults Off;
Controller settings include strength and Test Rumble. Gameplay effects cover
Wario damage, impacts, accepted enemy attacks, rocks/switches, Frog Switch,
boss/shop-item damage and transformation activation/recovery, with finite Fire
pulse trains and a bounded snowball roll. Host-time sequences cancel on inactive
play/disconnect and do not replay; settings v1-v3 load without rewriting.
Final normal/ASan builds and native-event, virtual-controller/settings and
coroutine/BIOS/I/O/disposable-save regressions pass. Fresh GBA build/v7-66
(also v7-65): ROM-SAME and all 202 objects OBJ-SAME, MD5
940b597bef3be3f4041021b47a2f4096; next fresh build/v7-67.
Current personal SRAM/settings, prior test saves/configs, original ROM and local
Beta 2 artifacts match before/after hashes (85 protected files). Published Beta 2
tag still targets d777f81; all three public assets match the retained release.
GitHub currently reports prerelease=false, unlike the historical handover;
read-only verification changed no public metadata or assets. port and release
tags remain unchanged. No push, merge into port, tag or release is authorized.
Unrelated handover/provenance and previous local plan notes remain unstaged.
See docs/WINDOWS_RUMBLE.md for detailed effects, evidence and manual-coverage
limits. Next: further user gameplay feedback/acceptance; keep this milestone
local until the user explicitly authorizes a named-branch push.


Windows v0.2.0-beta release preparation (2026-10-20), The CRT Update.
The user confirms CRT Lite and Enhanced (including glow/phosphor effects) on
both the Windows development PC and Intel integrated-graphics/60 Hz OLED laptop,
and authorizes publishing the prerelease. See docs/WINDOWS_CRT.md and
docs/WINDOWS_BETA2_RELEASE.md. Original remains Off by default; v1/v2 settings
migrate without load-time writes; v3 CRT controls apply after a game restart.
Fresh normal/ASan, display and GBA verification are recorded under
build/release-v0.2.0-beta-20261020; latest GBA v7-51 is ROM-SAME and all 202
objects OBJ-SAME, MD5 940b597bef3be3f4041021b47a2f4096; next v7-52.
CRT work is based on cleaned commit 831151f following 909d998. Beta 1 tag
v0.1.0-beta remains at 909d998 and its published ZIP is preserved. Unrelated
local cleanup/provenance notes remain outside this release commit.
Next: clean committed-source Windows x64 build, external ZIP extraction and
launcher/game acceptance, then normal myfork/port push and annotated
v0.2.0-beta tag/GitHub prerelease if all checks pass.

- **Beta 1 candidate and rights hold (2026-10-19):** user manually confirms the portable launcher/game work on the Intel integrated-graphics/60 Hz OLED laptop; VSync Off tears visibly and On does not. Recorded without inferring unreported audio/driver/VRR/mode coverage. Runtime commit **7dd76cb** adds YoshiKong to Credits atop **5cef1d7** VSync. See `docs/WINDOWS_BETA1_RC.md` and `docs/LAUNCHER_ASSET_RIGHTS.md`.
- **Latest local candidate:** `build/beta1-rc2-20261019/WarioLand4-Native-PC-Port-v0.1.0-beta.1-Windows-x64.zip`, SHA-256 `3cc3d702109e401e23d5b8c1144c4563eaa2ea72c8de5cc300700401796de65a`. Fresh optimized Release, exact 20-file allowlist/manifest/import audit, Windows-only-PATH extraction outside repo, visible game/VSync/F11 and launcher info/Settings smoke pass. Release/normal/ASan relevant regressions and Credits checks pass; SRAM/config hashes unchanged; own processes closed. Older archives/evidence preserved.
- **Public release stopped:** MFGG's sheet verifies the MisteryGX/RamenNoodleSprites + Nintendo credit condition; do not call that absent, or a blanket MIT/Nintendo/public-redistribution grant. YoshiKong is the icon sheet uploader, distinct from MisteryGX; source cataloguing suggests game level-icon graphics, while direct image access is blocked. Its creator/underlying rights are not cleared by credit alone. Font public-domain glyph provenance and SDL notice verified. Separate upstream imported-code licence coverage remains unresolved; existing MIT grants are not revoked merely by missing current notices. No public push, tag or release performed; proposed v0.1.0-beta.1 is not created locally. Next: scoped asset/code permission evidence or permitted replacements, then fresh remote checks before any push.
- **Latest GBA identity:** fresh **v7-46**, ROM-SAME, OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`; earlier candidate v7-45 also passes. Next fresh **v7-47**. No tools/agbcc, main-tree or real-save changes. Older entries below are historical.


- **Optional VSync (2026-10-19):** local implementation commit 5cef1d7; user confirms the private beta runs on the second laptop, with Intel/60 Hz OLED tearing. Added Display On/Off using checked SDL renderer VSync calls, reported-interval diagnostics, non-fatal fallback and reapply across fullscreen/display changes. New v2 configs default On; v1 retains all values and previous Off behaviour without rewriting on load. Direct/launcher game paths share it. See `docs/WINDOWS_VSYNC.md`.
- **Clock/presentation verification:** original ~59.73 Hz simulation clock runs up to four due frames independently of present, skips upload/present when none is due, and waits only for the remaining absolute deadline; short stalls retain frames/audio and long stalls retain the prior backlog-reset policy. Normal/ASan builds, config migration/persistence, synthetic 30/60/120/144 Hz, API-failure fixtures, launcher On/Off/reopen/Cancel and runtime/save/input regressions pass. Explicitly visible windowed/fullscreen/F11 On/Off and ASan windows run at 59.724-59.730 simulation Hz, with one present per simulated frame in these runs, zero clock resets and no post-startup audio refills/overflows. Tab bypass/restoration passes. Personal SRAM/settings hashes unchanged; owned processes closed.
- **Presentation investigation:** initial loop submitted unchanged framebuffers repeatedly; removed that traffic. Native visibility checks also proved the original harness left SDL windows hidden, permitting simulated VSync. Earlier ~240/sec/mixed-GPU scanout implications are withdrawn; SDL GetVSync reports configuration, not scanout. Final tests explicitly show/check owned windows. No tear-free/Intel/OLED or audible-glitch claim is made. Intel laptop visual/listening checks are next. Evidence `build/vsync-20261019/`; private beta ZIP/hash unchanged; no repackage, push or publication.
- **Latest GBA identity:** final **build/v7-44** (earlier v7-42/v7-43), ROM-SAME, OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`; next fresh **v7-45**. Follows source `67609ea`; prior private-beta handover/plan work remains preserved. Older entries below are historical.


- **Private USB beta (2026-10-19):** packaged source 67609ea into a fresh Windows x64 Release build (-O2, ASan off). ZIP contains only two EXEs, SDL3.dll and instructions/notices; no ROM, extracted assets, save or personal config. See docs/PRIVATE_WINDOWS_BETA.md for artifact SHA-256, exact payload and test limits. Artwork redistribution grants remain undocumented; confirm before wider sharing. No push/public release.
- **Package verification:** archive/hash allowlist audit, extracted launcher information/Settings UI and separately started game window pass outside the repo in a path with spaces and Windows-only PATH. Release/ASan runtime, disposable-save and input/config/virtual-controller tests pass. Real SRAM hash unchanged; owned processes closed. Existing RivaTuner overlay observed and left untouched. Full Browse/Launch/save/restart on the separate laptop is next; no pristine-machine claim.
- **Latest GBA check:** fresh **build/v7-41**, ROM-SAME, OBJ-SAME, MD5 940b597bef3be3f4041021b47a2f4096; next fresh **v7-42**. Game/launcher source unchanged; prior local font/icon c3d4eef, published base 0c2a4a5. Older statuses below are historical.


- **Selected ROM filename (2026-10-19):** user-approved launcher-only presentation follow-up to `c3d4eef` (published base `0c2a4a5`). A gold filename line updates on remembered-path restoration and path edits, including Browse selection. The complete path stays in the existing editable field; long names use display-only ellipsis and ampersands remain literal. Validation, persistence and Launch routines are byte-for-byte unchanged in source.
- **Filename verification:** normal/ASan builds and controlled UI checks pass for restored names, live edits, empty paths, trailing separators, spaces, forward slashes, UNC paths, a synthetic 32,657-character path and a long filename. Full editable text is unchanged in each case. These are UI-text tests, not extended filesystem-path acceptance claims. No ROM/game/SRAM opened; explicit disposable config overrides used and all owned test processes closed. Evidence `build/launcher-rom-name-20261019/` includes results, logs and previews.
- **Latest GBA check:** fresh **build/v7-40**, ROM-SAME, OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`; next fresh directory **v7-41**. No push/release for this follow-up; further milestones require approval. Older statuses below are historical.


- **Launcher font/icon (2026-10-19):** user approved matching Settings' pixel font across the main launcher and owned information/error windows, plus the supplied launchericon PNG as executable/window/Settings/shortcut icon. Private embedded TTF preserves SDL's exact 8x8 glyphs; multi-size ICO preserves source aspect ratio with nearest-neighbour scaling. No system font install, settings/config or save-code changes. This follows published `0c2a4a5` (settings `38e8d4f`, theme `2b6ea67`).
- **Font/icon verification:** normal/ASan builds and sequential main/info/error/Settings UI checks pass. All 191 glyphs match SDL at 2x (48,896 pixels, zero differences). All seven embedded icon sizes load. Original icon PNG is byte-identical; sprite/MisteryGX attribution retained. Tests used explicit disposable config paths and did not launch a game or open SRAM. Evidence `build/launcher-font-20261019/`; details `docs/LAUNCHER_FONT_ICON.md`.
- **Latest GBA check:** final **build/v7-39**, ROM-SAME, OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`; next fresh directory **v7-40**. Earlier font/icon checks were v7-37/v7-38. All owned UI test processes closed. No push or release for this follow-up; further milestones require approval. Older entries below are historical.


- **Verified branch publication (2026-10-19):** settings implementation `38e8d4f` and theme `2b6ea67` are committed. The user approved a normal push to `myfork/port`; the pre-push fetch found no remote-only commits. Reviewed outgoing paths contain only the intended launcher/settings code, tests, documentation and credited sprite; no SRAM saves, ROMs or generated data. Existing untracked `data`, `expected` and `include/data` entries remain untouched. This status-only follow-up uses fresh **build/v7-36** for the final GBA identity check; next fresh directory **v7-37**. No release is authorized; further milestone work still needs approval.


- **Settings and controls (2026-10-19):** user-approved milestone implemented following launcher theme `2b6ea67` (earlier launcher `bf9734f`, status `fc143c3`). Shared versioned application-folder settings, physical keyboard capture/conflict checks, SDL gamepad button/trigger bindings, hotplug, optional analog movement/dead zone, startup fullscreen and scale; separate purple/gold Keyboard/Controller/Display window. Legacy ROM-path persistence and MisteryGX artwork retained. No core game, GBA or save implementation edits.
- **Settings verification:** normal/ASan builds, config/input/virtual-controller tests, launcher capture/save/reopen/cancel checks, runtime regressions and independent game config/display startup pass. User confirms C/D keyboard remaps in gameplay, real controller operation (SDL name Xbox 360 Controller), controller reconnect and keyboard fallback. Physical controller tests are manual; virtual tests are distinguished. See `docs/WINDOWS_SETTINGS.md`; evidence `build/settings-20261019/`.
- **Latest GBA check:** final **build/v7-35**, ROM-SAME and OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`; next fresh directory **v7-36**. Initial settings check was v7-34. Older entries below are historical.
- **Save evidence correction:** older launcher/theme GUI tests using only APPDATA redirection did access the normal SDL save path; those old never-accessed claims are withdrawn. This milestone's automated game tests use explicit disposable --save paths, config tests use WL4_SETTINGS_FILE. Real save hashes changed during user manual testing; the agent did not restore/overwrite the save. No save-path or save-format changes. All owned automated test processes are closed; user-started applications remain alone.
- **Next:** further milestone work requires approval. Clean-PC/high-DPI/accessibility review and additional controller profiles remain outside this focused implementation. No push or release created; M3 route extension stays paused.


- **Launcher theme (2026-10-19):** user-approved purple/gold styling, rounded buttons/window, embedded transparent Vampire Wario sprite at exact 4x nearest-neighbour scale, separate About/CREDITS/README buttons. Credit: MisteryGX. Builds on launcher implementation `bf9734f` and verification/status commit `fc143c3`. No game or save implementation changes.
- **Theme verification:** normal/ASan builds and sequential GUI browse/persistence/game-launch/information-button checks pass; runtime coroutine/BIOS/I/O/save tests pass. Original PNG is byte-identical; screenshot pixel checks confirm exact 4x opaque pixel blocks. Previous launcher settings restored, disposable AppData used and all test processes closed. Evidence: `build/launcher-theme-20261019/`; details in `docs/WINDOWS_LAUNCHER_V1.md`.
- **Latest GBA check:** fresh **build/v7-33**, ROM-SAME, OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`; next fresh directory **v7-34**. Older entries below are historical. Next: user review of themed launcher; clean-PC/high-DPI and older-Windows corner fallback acceptance remain open. No push or release, and further milestones still require approval.


- **Windows launcher v1 (2026-10-19):** approved by the user; separate `wl4-launcher.exe` with native file dialog, shared ROM validation, remembered path/scale, About credits, adjacent-game startup and visible dependency/exit errors. No game sources, generated data or save logic changed. Implementation commit `bf9734f` follows `929652a` / `771dcc2`; see `docs/WINDOWS_LAUNCHER_V1.md` and `port/README.md`.
- **Launcher verification:** normal/ASan runtime tests and controlled GUI checks pass: genuine ROM browse, missing/size/hash errors, spaced path, reopen persistence and game startup/close. Missing executable and missing/incompatible SDL3 fixture tests pass. Initial harness cleanup errors were corrected; ASan found a nested-callback stack overflow, fixed by moving long path buffers off the stack. All owned test processes closed. Tests use disposable AppData and read the genuine ROM through a junction; real ROM/save bytes remain untouched. Evidence: `build/launcher-v1-20261019/`.
- **Latest GBA verification:** fresh **build/v7-32**, ROM-SAME, OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`. Next fresh build: v7-33. Earlier statuses below are historical.
- **Next:** user acceptance of launcher v1, then approval for any further milestone; clean-PC/high-DPI acceptance remains unverified. No push, package or public release authorized or created. M3 route extension stays paused.


- **Latest approved save-recovery task:** implementation `771dcc2` follows `7ba5638`; full details in `docs/M3_SAVE_RECOVERY.md`. Nonmodal warning/F8 details, retained SRAM and bounded-frequency 300-frame retries, explicit failed-quit choices, headless failure status and exclusive atomic temporary files are implemented. Reject incorrectly sized/unreadable save inputs without altering SRAM or opening a writable session. No game source, format or generated file changed.
- **Verification:** normal/ASan coroutine/BIOS/I/O/save tests pass, as do disposable-window warning/recovery/quit-choice checks and 8500-frame Hall save/restart tests. Save SHA256 matches the prior Hall reference. The real AppData save was not accessed. All new evidence remains under ignored `build/m3-save-recovery-20261018/`. Final fresh **build/v7-29**: ROM-SAME, OBJ-SAME, MD5 `940b597bef3be3f4041021b47a2f4096`; next is v7-30. Older build/status entries below are historical.
- **Next:** propose first-launch ROM/dependency UX for approval. Other alpha-readiness gaps remain; no public/private package was created. Manual coverage is unchanged and M3 is not fully verified. Independent concurrent save writers and power-loss retention of pending RAM remain outside this task.


- **Latest M3 manual coverage / alpha review:** see `docs/M3_ALPHA_READINESS.md`. The user reports manual Windows completion of Hall (switch/timer/escape), Spoiled Rotten victory and Golden Pyramid return, two Wario Hop rounds through Game Over, zombie platform fall-through, bat flight and save restoration after restart. These are manual observations; M3/full-game automated equivalence remains incomplete. They satisfy the earlier manual Hall follow-up without extending the automated recording.
- **Current base and preservation:** reviewed HEAD `f3d22a5` includes the later README/screenshot work; runtime fix `c25cc4d` and fixture notes `38c49ef` remain intact. This task updates four documentation files only. All previous test evidence, routes, generated files and game sources are preserved. Latest GBA verification: **build/v7-26**, MD5 `940b597bef3be3f4041021b47a2f4096`, ROM-SAME and OBJ-SAME; next is v7-27. Older status/build numbers below are historical.
- **Alpha assessment:** guided private testing is reasonable, but no unattended package/public release yet. Highest priorities: (1) visible save-write failures and pending-data retry, (2) first-launch ROM/dependency UX, (3) durable crash/hang evidence, (4) allowlisted bundle plus clean-PC/controller checks, (5) broader progression/failure coverage. ROM-rejection smoke tests and PE imports were checked locally; no new gameplay, physical controller or clean-machine validation is claimed.
- **Next small task, proposed only:** make save-write failure visible, rate-limit warnings and retry pending SRAM safely, with disposable failure/recovery tests. Await milestone approval before runtime changes. No archive/release was created and no ROM data distributed.


- **Priority Frog Switch fix (2026-10-18):** implementation `c25cc4d`, following `ff28d49`. LLDB captured VBlank starvation after activation: the HBlank callback's VCOUNT read advanced the display, consumed the host's VBlank event and allowed IF acknowledgement to clear the wakeup. IRQ display reads now stay on the current event; ordinary polling still advances. No game source or generated symbols changed.
  - Normal and ASan coroutine, BIOS and I/O tests pass, including a regression covering HBlank display reads and three VBlanks. Both replay the unchanged room-6 route for 25,200 frames and complete positioned switch/countdown/movement/vortex-exit fixtures, returning to stage select. RAM positions the test character above the switch and later at the vortex: an uninterrupted walk through the barrier is **not** claimed.
  - Final fresh GBA verification is **build/v7-25**, ROM MD5 `940b597bef3be3f4041021b47a2f4096`, ROM-SAME and every object OBJ-SAME; next is v7-26. Earlier v7-18/v7-23 statements below are historical.
  - All prior recordings and unfinished M3 artifacts remain intact. New debugger transcripts, original executable, probes and captures are in ignored `build/frog-freeze-20261018/`. See `docs/M3_FROG_SWITCH.md` for evidence, reproduction commands and precise limitations. Next: manual switch-to-exit traversal with the rebuilt executable; general route extension remains paused outside this bug request.

- **Done:** M0 (steps 1–10), M1 and M2, pushed to `myfork/port` (last commit `abd1e87`). The game-source changes of steps 1–4 and the barrier fix are also on `myfork/decomp-wtf` (`ba49d50`, `d602a7b`, `4d7db63`; barrier fix cherry-picked as `0c2f07a`, byte-identical). GBA byte identity was last verified as `build/v7-18` (MD5 `940b597b…`, objects identical).
- **M1 results:**
  - **Renderer** (`port/src/ppu.c`): draws each line at HBlank start, before HBlank DMA. Modes 0–5, text/affine/bitmap BGs, regular and affine sprites, windows, blending, brightness, mosaic. Where hardware behaviour is unclear it copies mGBA 0.10.5's software renderer.
  - **Colours:** blended on 5-bit channels as on the GBA by default. `--mgba-colors` copies mGBA's 8-bit maths for exact comparison.
  - **Pixel-exact with mGBA** (`--mgba-colors`) from boot through the intro, the title and file select (1,394 steps with `file_select.keys`).
  - **SDL3 window** (`port/platform/sdl/sdl_frontend.c`): integer scaling, mGBA's default keys, the first gamepad, pacing at 59.73 Hz, Tab fast-forward, F11 fullscreen.
- **M2 results:**
  - **The crash entering Hall of Hieroglyphs** was two port bugs, both byte-identical on the GBA:
    - `SetHBlankMode` copies the HBlank callback's code into IWRAM by DMA. The port now copies the ROM bytes and registers the native callback for the copy's address (`cd074f2`).
    - `UpdateWarioTileInteractions` indexed the native tile handler table through a truncated `u32` (`4ec4c1b`).
  - **First level diffed** (`50ea471`): `port/tools/inputs/hall_of_hieroglyphs.keys` starts a new game and plays into the level. Memory matches mGBA on all 2,190 steps from the passage screen on. Pictures match on all but one frame, where the vortex setup writes display registers while mGBA draws the top lines.
  - **Sound engine** (`68abf56`): `port/src/m4a_native.c` translates `asm/m4a_asm.s` (`SoundMain` and its mixer, `MPlayMain`, `ply_note` and the other `ply_` commands) step by step. The engine's state matches mGBA until the first lag frame where the game runs `SoundMain` from its VBlank callback. mGBA's lag frames are extra music ticks the port does not have, so from there the music runs ahead on mGBA (file select: steps 1–158 match exactly). `diff_frames.py --mask-sound` compares the rest.
  - **Sound hardware and audio** (`1c23282`): `port/src/audio.c` covers the PSG channels, FIFOs fed by DMA on timer overflows, and SOUNDCNT/SOUNDBIAS mixing at 65536 Hz. The sound registers read as mGBA keeps them. The window plays through an SDL3 stream whose speed follows the queue. `--wav` writes headless audio.
  - **Open bus and mirrors** (`4e36ff8`): with a save, leaving a stage for the passage map crashed. On the map's first frame, `DrawMapScreenSprites` reads through two pointers `InitMapScreen` has not set yet. It reads first from address 0, the BIOS, then from the open-bus value it found there. The GBA tolerates this, and mGBA does the same.
    - The port now redirects faults on unmapped 32-bit addresses (`gba_memory.c`, `os_win32.c`, `x86_access.c`). BIOS and unused addresses read `0xE3A02004` (the opcode the BIOS leaves after an SWI), and writes there are dropped. The RAM, palette, VRAM and OAM mirrors go to their memory.
    - Unused addresses on the GBA return the CPU's prefetch, which the port cannot know.
    - With the save, the port matches mGBA for all 1,671 steps into the level.
  - **SRAM saves** (`abd1e87`): `port/src/save.c` keeps 32 KiB in mGBA's `.sav` format. The file is written atomically half a second after the game stops changing SRAM, and on quitting. The window uses `%APPDATA%\wl4\wl4.sav` or `--save FILE`. A reloaded save shows "Hall of Hieroglyphs" in slot A. `run_mgba_dump.ps1 -Save` boots mGBA with the same save.
  - **Sanitizers:** the ASan build passes the unit tests and plays the level script, with and without a save, with no reports. Its WAV output equals the normal build's.
- **Diff tooling** (`port/tools/`, documented in `port/README.md`):
  - `run_mgba_dump.ps1` drives mGBA unattended. Options: `-Shots`, `-InputScript`, `-FromStep`, `-Save`.
  - `diff_frames.py` (memory) and `diff_screens.py` (pictures) pair one frame per finished main-loop step.
  - Input scripts (`STEP KEYS`) work in wl4, headless or in the window, and in mGBA alike.
- **Diff masks** (`diff_frames.py`, each with its reason): the GBA stacks; VCOUNT/DISPSTAT status; timer counters; write-only DMA and FIFO registers; mGBA's FIFO DMA quirk; the music engine's memacc area; the HBlank callback copy while it holds open bus; the SOUNDCNT_X status. The sound engine's state and registers are masked only with `--mask-sound`.
- **Known gaps:**
  - **Game code takes no time.** Lag frames do not exist in the port, which is why the music drifts from mGBA, and raster effects that poll VCOUNT mid-step are approximate. Interrupts raised while the game runs are handled at its next Halt.
  - **Sound paths not yet compared with mGBA**, because the lag drift starts first: the fixed-frequency mixer, echo, reverb and the CGB channels driven by the engine. A lag-frame replay (running `SoundMain` an extra time where mGBA lagged) would cover them.
  - Open bus at unused addresses is approximate (see above). BIOS 0x2A (`GetJumpList`) is unimplemented. UBSan has not been tried.
  - 295 compiler warnings remain; two header pairs still conflict (`save_reset.h`/`save_data_clear.h`, `stage_entry.h`/`boss_door_opening.h`). `StartLargeLipsKiss`'s fifth argument still needs confirming against mGBA.
- **Housekeeping:** the main tree's verify-step4 and barrier leftovers were deleted (2026-10-18); `barrier-base.log`, `barrier-after.log` and `barrier-after.objcheck` remain there. Work is handed over to Codex: see `CODEX_HANDOVER.md` and `AGENTS.md`.
- **Codex handover verification (2026-10-18):** local `port` and live `myfork/port` both started at `500e20b`, with all M2 and handover commits present. The scoped status contained only the existing `include/data` symlink entry. No runtime or game source changed in this investigation.
  - Normal and ASan builds are current; coroutine, BIOS and I/O tests pass in both. Fresh GBA checks `build/v7-19` and `build/v7-20` match MD5 `940b597bef3be3f4041021b47a2f4096`, the reference ROM and every object in `build/us/obj`.
  - File select: all 1,394 native steps match memory with `--mask-sound` and pictures with `--mgba-colors`. mGBA needed 1,500 frames to cover them; 1,400 frames completed only 1,348 steps here. Without the sound mask, the first difference remains step 159.
  - Existing Hall baseline: all 2,132 shared steps match memory with sound masked (native 6,800 frames from step 4,600 contains 2,190 steps; mGBA contains 2,132). The only picture difference remains the vortex at local comparison step 384, native frame 4,994 versus mGBA frame 5,046: 11,400 pixels on lines 0-68. The unpaired tail is not claimed as verified by this run.
  - **First M3 route extension:** `port/tools/inputs/hall_room11.keys` preserves the old route and adds jumps up the east platforms into room 11 at finished step 8,094 (native frame 8,105). Run native for 8,500 frames and mGBA for 8,600, both from step 7,800. All 690 native steps match memory with sound masked and pictures exactly. ASan completes the extended route without reports; its WAV and 32 KiB save equal the normal build's, and the save equals mGBA's final SRAM byte for byte. No new gameplay bug was established.
  - Both diff tools zip streams and stop at the shorter one: always report coverage counts and allow extra mGBA frames. All captures, exploratory scripts, saves and WAVs remain under ignored `build/m3-handover-20261018/`; none are committed. This route and these notes are the first M3 testing change after `500e20b`.
- **M3 paused at the user's request (2026-10-18):** implementation commit `6a0a186` follows `feb6eb4`. It fixes two 64-bit unsigned-negation pointer subtractions in `ResolveWarioWaterSurfaceCollision`, only under `PORTABLE`, and adds `hall_room6_entry.keys` without changing either earlier passing script. Normal/ASan tests pass; the repaired water window matches mGBA on 890 memory and picture steps. Fresh GBA `build/v7-23` is byte-identical (MD5 `940b597bef3be3f4041021b47a2f4096`, every object identical); next is `v7-24`. `src/block.c` has no diff: only our diagnostic include/printf were removed, without restoring a backup.
  - Verified windows: 2,190 memory steps through the pipes; 4,890 memory/picture steps through the first jewel and attack wall; 2,890 memory/picture steps through the successful room-5 crossing; 3,290 memory steps through room 5; and a final 590 memory steps through the rock throw, ending at absolute step 27,289. These are separate windows with gaps, not a continuous full-playthrough result. The earlier vortex remains separate; five new score-digit timing pictures are documented with previous-frame pixel evidence in `CODEX_HANDOVER.md`. Masks are unchanged.
  - Committed route reaches room 6 entry. Preserved native-only checkpoint `build/m3-hall-exit/hall_room6_middle.keys` reaches the middle platform at 31,000 frames: room 6, `(312, 319.75)`, standing, no carried rock; second rock `(408, 320)`. Farthest probe `hall_room6_upper_probe.keys` reaches `(343.5, 191.75)`, crouched against the upper brown blocks at 33,400 frames. The middle ladder climb awaits mGBA comparison. Two jewel pieces collected; Frog Switch and exit not reached.
  - No further collision defect was established: room-4 block/ceiling behavior matches mGBA, and the apparent map mismatch used different route states. Next route hypothesis is an upward-left throw of the middle-platform rock toward the upper brown gate; it is unverified. All artifacts and diagnostic reports remain under ignored `build/m3-hall-exit/`, including unsuccessful probes. Normal/ASan WAVs and saves match byte for byte; the save matches mGBA SRAM.
- **Next:** wait for explicit resume. Follow the precise checkpoint and comparison commands in `CODEX_HANDOVER.md`, verify the middle climb, solve the upper passage, then continue collectibles, Keyzer, Frog Switch and exit. No playtests or test runners remain active.

## Later milestones

- **M1 (first visible milestone), pixel-exact title screen on Windows.** SDL3 window, renderer, input and pacing. Exit criteria: the framebuffer matches mGBA, Start reaches file select, and the sanitizers are clean. **Done 2026-10-18** (see "Where we are").
- **M2.** Audio, saves, and the first passage with HBlank effects and affine. **Done 2026-10-18** (see "Where we are"; sound is compared with mGBA up to the first lag frame).
- **M3.** Full playthrough under the diff harness, using TAS input movies (expect some lag-frame desyncs).
- **M4.** Linux x64.
- **M5.** Android arm64: verify the low-address reservations (below ART's default heap at about `0x12C00000`), add touch controls, use the NDK build.
- **M6.** Configuration, filters, the JP version, and widescreen experiments.

## Licence

- `TsilaAllaoui/warioland4` (upstream `origin`) states no licence. It has no LICENSE file, GitHub reports none, and the README is silent.
- It is a GitHub fork of [lilDavid/warioland4](https://github.com/lilDavid/warioland4), which is **MIT**, "Copyright (c) 2025 lilDavid" (initial commit `27b90b2`, 2025-05-27).
- TsilaAllaoui deleted the LICENSE in commit `c1cc627` ("-Cleaning", 2026-09-30).
- lilDavid's portions remain MIT. Their LICENSE is restored byte for byte in this fork (commit `c4f568f`, on `decomp-wtf` and `port`).
- Licence coverage for later upstream additions in the imported revision is not established. Current upstream metadata states no licence; a missing/deleted notice does not revoke earlier valid MIT grants or prove every imported file unlicensed. Resolve the revision-level provenance/permission gap rather than assuming the restored lilDavid notice covers all later additions. See `docs/LAUNCHER_ASSET_RIGHTS.md`. An issue asking for clarification was drafted but has not been posted.
- Our own and decomp.wtf commits can be licensed as we choose.
- Borrowed port code: mGBA (MPL-2.0) fits under MIT. gpSP (GPL) would make the port GPL. agbplay's licence needs checking. minicoro (vendored, patched) is Public Domain or MIT-0. Its asm context switch is from LuaCoco (MIT, Mike Pall), whose notice must ship with binaries. The agbcc submodule is a GPL build tool and doesn't affect the port.
- Not legal advice. Releasing binaries still carries Nintendo IP risk even with no assets inside.

## Risks

- Hidden undefined behaviour in matched fakematch code. Mitigation: the diff harness and sanitizers.
- Fixed reservations failing on some machines (detected, with a clear error). Android is unverified.
- The coroutine context switch interacting with Windows stack checks or ASan. Checked on Windows x64: stack overflow is reported, and ASan sees the coroutine stack. Linux and Android are still to verify.
- WL4's m4a variant differing from sa2's mixer.
- Portable builds keep the asm barriers. One (`score.c`) clobbers `"r8"`, valid on x86-64 but not arm64. `-mno-sse` is x86-only, and arm64 tolerates unaligned NEON loads.
- The I/O traps depend on the OS. Windows and Linux x86-64 can single-step in-process (the trap flag). Android arm64 cannot, so it would need the faulting store emulated, or native fast paths for every register access.
- The licence gap above.

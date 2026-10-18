# PC port

The plan and the decisions behind it are in [docs/PC_PORT_PLAN.md](../docs/PC_PORT_PLAN.md).
The GBA ROM is still built with `make`. The PC build compiles the same `src/` with
`PORTABLE` defined, and must not change the ROM: every change to shared code is checked
by rebuilding the ROM and comparing it and all objects with the previous build.

## Building (Windows)

You need [llvm-mingw](https://github.com/mstorsjo/llvm-mingw) (UCRT, x86_64), CMake 3.20+ and Ninja.
For the window you also need SDL3: unpack the `SDL3-devel-<version>-mingw` release from
[libsdl-org/SDL](https://github.com/libsdl-org/SDL/releases) (3.4.18 is tested).

    cmake -S . -B build_pc -G Ninja --toolchain port/cmake/llvm-mingw-x86_64.cmake ^
          -DLLVM_MINGW=C:/path/to/llvm-mingw -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
          -DSDL3_DIR=C:/path/to/SDL3-3.4.18/x86_64-w64-mingw32/lib/cmake/SDL3
    cmake --build build_pc

Without SDL3, `wl4` only runs headless. With it, the build copies `SDL3.dll` next to `wl4.exe`.

The PC build does not need the extracted assets (`make extract`): game data is read from
your own ROM at run time.

## How game code builds for PC

- `PIN()` and the `COMPILER_BARRIER*()` macros only steer agbcc, and are empty. `ASM_BARRIER()`
  stays an empty asm statement, because some barriers carry a value: `ASM_BARRIER(: "=r"(x) : "0"(y))`
  sets x to y. `ASM_BARRIER_ARM()`, for the ones that name ARM registers, is empty.
- `SYSCALL()` calls `PlatformSyscall()` instead of the BIOS (`port/src/bios.c`).
- Data defined in `src/` sits in `#ifndef PORTABLE` blocks with `extern` declarations in
  the `#else` branch. The port places each symbol at its GBA address. Only tables of
  function pointers are defined natively. The ROM data in `src/data/` is not compiled,
  except the three function pointer tables there.
- agbcc aligns every struct and union to 4 bytes. Every definition carries `AGB_ALIGN`,
  which gives the PC build the same alignment.
- `port/generated/layout_asserts_*.c` checks agbcc's size and field offsets for every
  struct and union in `include/`. Structs that hold pointers are checked only with
  `PORT_CHECK_POINTER_LAYOUTS`, until the GbaPtr conversion.

The rewrites are done by scripts. Run them again after merging new matches, and use
`--check` to list anything left:

| Script | Rewrites |
| --- | --- |
| `tools/port_pin_codemod.py` | `register ... asm("rN")` → `PIN("rN")` |
| `tools/port_asm_barrier_codemod.py` | empty-template `asm` → `ASM_BARRIER()` |
| `tools/port_data_codemod.py` | data definitions in `src/` → `#ifndef PORTABLE` blocks |
| `tools/port_struct_align_codemod.py` | `struct Foo {` → `struct AGB_ALIGN Foo {` |

## Links to the GBA symbol table

`port/tools/gen_symbols.py` reads the GBA symbol table and the PC build of the game code,
and writes three files to `port/generated/`:

- `gba_symbols.s` defines every data symbol as an absolute symbol at its GBA address,
  together with the linker constants (`gNumMusicPlayers`, `gMaxLines`). Clang reaches
  extern data through 64-bit `.refptr` slots, so the executable can load at any address.
- `gba_functions.c` is the dispatch table from each function's GBA address to the native
  function (`PortLookupGbaFunction()`), for function pointers read from GBA memory.
- `gba_stubs.c` holds weak stubs for GBA functions that are still asm. Each one reports
  its name and address and stops. A platform definition with the same name replaces it.

The symbol table must keep every symbol at its base-ROM address, which `build/us` does not
(the unmatched functions move what follows them). Build the playtest ELF under WSL, then
generate from Windows after `cmake --build`:

    make -k; python3 tools/build_playtest_rom.py        (WSL; needs expected/us/obj)
    python port/tools/gen_symbols.py --elf build/playtest/warioland4_us_playtest.elf ^
        --lib build_pc/libwl4_game.a --nm C:/path/to/llvm-mingw/bin/llvm-nm.exe
    python port/tools/gen_lz77_sources.py               (the LZ77 streams for --test-bios)

`port/tools/gen_layout_asserts.py` regenerates the layout checks. It runs under WSL,
because it needs agbcc and libclang (`pip install libclang`).

## Runtime

`wl4.exe [ROM] [--scale N] [--frames N [--dump FILE] [--shots FILE]] [--input FILE] [--mgba-colors] [--selftest] [--test-bios] [--test-io] [--fault rom-write|unmapped|stack-overflow] [--asan-probe]`

With no `--frames` or tests, the game runs in a window (`port/platform/sdl/sdl_frontend.c`), at
`--scale` times 240x160 (default 3). The window scales in whole steps. The keys are mGBA's
defaults: arrows, X = A, Z = B, A = L, S = R, Enter = Start, Backspace = Select. Tab
fast-forwards while held, F11 toggles fullscreen and Escape quits. The first gamepad works by
button position (bottom = A, right = B). Frames are paced at the GBA's rate, 2^24 Hz / 280896
cycles (about 59.73 a second). There is no sound and no saving yet.

At startup, `port/src/gba_memory.c` reserves the GBA regions at their real addresses
(`port/include/port/gba_memory.h`). It also reserves the game stack at `0x01020000`–`0x01810000`,
with a guard page, 60 KiB of headroom and inaccessible blocks around it. A vectored
exception handler reports faults on GBA addresses (for example a write to the read-only
ROM) and overflows of the game stack. `port/src/rom.c` loads the ROM (default
`baserom_us.gba`), checks its size and MD5, and makes it read-only. The game runs as a
minicoro coroutine on the low stack (`port/src/game_thread.c`).

`port/src/bios.c` runs the BIOS calls the game makes:
- `CPUSet` and `LZ77UnCompVram` follow the hardware, including LZ77's halfword writes,
  which read stale data for a back-reference at distance 1.
- `Halt` yields from the game coroutine until the frame loop requests an interrupt.
- Any other call is reported and stops the game.

`port/src/gba_runtime.c` has the libgcc division helpers and m4a's `umul3232H32`.
`port/src/m4a_native.c` has the small m4a asm routines the game calls directly. `SoundMain` is a
silent placeholder, so music players never advance.

The game reaches the I/O registers through the `REG_` macros, structs, and addresses it
computes at run time. So instead of changing the game code, the port traps every access
to the register page:
- The page is a section with two views. The game's view at `0x04000000` has no access rights.
  The host reads and writes the other view (`PortIoHost()`) freely.
- A fault there calls `port/src/gba_io.c` before the access. The page is then opened, and the
  one instruction runs with the trap flag set. The single-step exception that follows applies
  the register's effects and closes the page.
- The effects:
  - **DMA.** A write that sets a channel's enable bit latches its registers. Immediate
    transfers run at once and clear the bit. VBlank and HBlank transfers run from
    `PortIoVBlank()`/`PortIoHBlank()`, with repeat and destination reload.
  - **IF.** Writing 1 to a bit clears it.
  - **VCOUNT and DISPSTAT.** These advance by half a line per read, so loops that wait for
    a scanline end. The host sets the line with `PortIoSetScanline()`.
  - **Read-only registers.** Writes to them are undone. Write-only bits read as 0.
- A trapped access costs about 4 µs.

`--frames N` runs the game headless for N frames (`port/src/frame.c`):
- The game runs until it halts, as it does once a frame while it waits for VBlank.
- While it is halted, the host moves the display on one HBlank or line at a time: it runs
  timed DMA and raises the interrupts that DISPSTAT enables.
- When IE enables a requested interrupt, the game resumes. Halt then runs a native copy of
  `irq_handler`, which calls the game's callbacks.
- Game code takes no time. A scene setup that takes the GBA several frames finishes in one.

`port/src/ppu.c` draws each visible line at the start of its HBlank, before the HBlank DMA
transfers, as the hardware does. It reads the registers, VRAM, palette RAM and OAM where
the game left them. It covers modes 0-5, text, affine and bitmap backgrounds, regular and
affine sprites, the windows (including the OBJ window), blending, brightness and mosaic.
Where the hardware is unclear, it does what mGBA 0.10.5's software renderer does: the delay
before a background turned on mid-frame appears, the sprite cycle budget, the OBJ priority
quirk, and how semi-transparent sprites and brightness combine. The I/O traps report
writes to the affine reference points (`PortIoTakeAffineWrites()`), even with the same
value, because each write restarts the display's internal reference point.

Colours are blended on 5-bit channels, as on the GBA. mGBA blends 8-bit channels and rounds
differently. `--mgba-colors` blends as mGBA does, so its screenshots can be compared
exactly.

`--dump FILE` writes GBA memory (EWRAM, IWRAM, I/O, palette, VRAM, OAM, SRAM) at the start of
every VBlank. `--shots FILE` writes the picture then, as 240x160 RGB bytes after a header.
`--input FILE` presses keys from a script, by steps of the game's main loop rather than by
frames, so that mGBA can press the same keys on the same steps. In the window, they add to the
keys you press. Each line is `STEP KEYS`: from
that step on, the game reads those keys (`START`, `A+RIGHT`, or `-` for none). The format is
in `port/include/port/input_script.h`. This one goes from the title to file select
(`port/tools/inputs/file_select.keys` also moves the cursor there):

    500 START   # skip the intro
    501 -
    700 START   # at the title
    701 -

To compare a run with mGBA (0.10 or later, the Qt build):
1. Run `wl4 --frames N --dump PORT.bin`.
2. Make the same dump in mGBA. `port/tools/run_mgba_dump.ps1` does it unattended:

       powershell -ExecutionPolicy Bypass -File port/tools/run_mgba_dump.ps1 ^
           -Mgba C:\path\to\mGBA.exe -Frames N -Out MGBA.bin [-Shots] [-InputScript FILE]

   It copies the ROM into a new run directory under `%TEMP%` and starts mGBA. It loads
   `port/tools/mgba_dump.lua` through the Scripting window and closes mGBA once the dump is
   written. `-Shots` also saves a screenshot of every frame in the run directory
   (`wl4-N.png` is frame N+1). `-InputScript` presses keys as `wl4 --input` does.
   `Get-Help port/tools/run_mgba_dump.ps1 -Full` has the details.

   To do it by hand, load the ROM and choose Tools > Scripting... > File > Load script...,
   then pick `port/tools/mgba_dump.lua`. The script erases SRAM, resets the game and writes
   the dump to `%WL4_DUMP%` (default `%TEMP%\wl4_mgba.bin`). `%WL4_FRAMES%` sets the frame
   count, `WL4_SHOTS=1` turns the screenshots on and `%WL4_INPUT%` names an input script.
3. Run `python port/tools/diff_frames.py PORT.bin MGBA.bin`.
4. To compare the pictures, run wl4 with `--shots PORT.shots --mgba-colors` and mGBA with
   `-Shots`, then `python port/tools/diff_screens.py PORT.bin PORT.shots MGBA.bin RUN_DIR`.
   `RUN_DIR` is the run directory that the mGBA script prints. `--save DIR` writes the
   differing frames side by side as PNGs.

The diff runners compare one frame for each step of the game's main loop: the first on which
the step has finished (`gMainTimer` changed, and the loop waits for VBlank with bit 0 of
`gInterruptCheck` clear). Boot time does not matter, nor do the frames on which mGBA is still
running a step that takes the port no time. The input scripts count steps the same way. The
memory diff masks state the port does not model, each mask with its reason: stacks,
VCOUNT/DISPSTAT status, timers, write-only registers, and the sound engine's state and registers.

`--selftest` runs the coroutine through yields and 2 MiB of recursion. `--test-bios` runs
the BIOS unit tests (`port/src/bios_test.c`). These include decoding every LZ77 stream the
game passes to `LZ77UnCompVram`, and comparing the result with a plain byte decoder.
`--test-io` tests the trapped registers on the game coroutine, and times the traps. `--fault` and
`--asan-probe` trigger the fault reports and an AddressSanitizer report on the game stack.
To build with AddressSanitizer, configure with `-DWL4_ASAN=ON`. Then put
`llvm-mingw/x86_64-w64-mingw32/bin` on `PATH` for `libclang_rt.asan_dynamic-x86_64.dll`.

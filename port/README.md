# PC port

The plan and the decisions behind it are in [docs/PC_PORT_PLAN.md](../docs/PC_PORT_PLAN.md).
The GBA ROM is still built with `make`. The PC build compiles the same `src/` with
`PORTABLE` defined, and must not change the ROM: every change to shared code is checked
by rebuilding the ROM and comparing it and all objects with the previous build.

## Building (Windows)

You need [llvm-mingw](https://github.com/mstorsjo/llvm-mingw) (UCRT, x86_64), CMake 3.20+ and Ninja.

    cmake -S . -B build_pc -G Ninja --toolchain port/cmake/llvm-mingw-x86_64.cmake ^
          -DLLVM_MINGW=C:/path/to/llvm-mingw -DCMAKE_BUILD_TYPE=RelWithDebInfo
    cmake --build build_pc

The PC build does not need the extracted assets (`make extract`): game data is read from
your own ROM at run time.

## How game code builds for PC

- `PIN()`, `ASM_BARRIER()` and the `COMPILER_BARRIER*()` macros only steer agbcc, and are empty.
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

`wl4.exe [ROM] [--selftest] [--test-bios] [--test-io] [--fault rom-write|unmapped|stack-overflow] [--asan-probe]`

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
- `Halt` yields from the game coroutine. The game halts in a loop until its VBlank handler
  has run, so the host resumes it once per frame.
- Any other call is reported and stops the game.

`port/src/gba_runtime.c` has the libgcc division helpers and m4a's `umul3232H32`.

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
  - **Read-only registers.** Writes to them are undone.
- A trapped access costs about 4 �s.

`--selftest` runs the coroutine through yields and 2 MiB of recursion. `--test-bios` runs
the BIOS unit tests (`port/src/bios_test.c`). These include decoding every LZ77 stream the
game passes to `LZ77UnCompVram`, and comparing the result with a plain byte decoder.
`--test-io` tests the trapped registers on the game coroutine, and times the traps. `--fault` and
`--asan-probe` trigger the fault reports and an AddressSanitizer report on the game stack.
To build with AddressSanitizer, configure with `-DWL4_ASAN=ON`. Then put
`llvm-mingw/x86_64-w64-mingw32/bin` on `PATH` for `libclang_rt.asan_dynamic-x86_64.dll`.

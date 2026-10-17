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
- `SYSCALL()` calls `PlatformSyscall()` instead of the BIOS.
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

`port/tools/gen_layout_asserts.py` regenerates the layout checks. It runs under WSL,
because it needs agbcc and libclang (`pip install libclang`).

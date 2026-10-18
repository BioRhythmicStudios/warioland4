# Handover: Wario Land 4 PC port (2026-10-18)

The working rules are in `AGENTS.md`, so read that first. The full plan and its decisions are
in `docs/PC_PORT_PLAN.md`; its "Where we are" section is the most detailed status. Each
runtime piece is documented in `port/README.md`.

## Current state

- **Milestones.** M0 (headless boot), M1 (renderer and SDL3 window, pixel-exact with mGBA to
  file select) and M2 (first level, sound, saves) are done on branch `port`, pushed to
  `myfork/port`.
- **GBA build.** It is byte-identical, last verified as `build/v7-18` (MD5
  `940b597bef3be3f4041021b47a2f4096`, all objects identical).
- **Latest commits:**

  | Commit    | What it did                                                                                  |
  |-----------|----------------------------------------------------------------------------------------------|
  | `c492073` | Updated "Where we are" after M2                                                              |
  | `abd1e87` | SRAM saves (`port/src/save.c`, `--save`, `%APPDATA%\wl4\wl4.sav` in the window)              |
  | `4e36ff8` | Faults on unmapped GBA addresses redirected to open bus (`0xE3A02004`) or to the RAM mirrors |
  | `1c23282` | Sound hardware (`port/src/audio.c`), SDL3 audio, `--wav`                                     |
  | `68abf56` | m4a sound engine translated to C (`port/src/m4a_native.c`)                                   |
  | `50ea471` | Hall of Hieroglyphs diffed against mGBA (`port/tools/inputs/hall_of_hieroglyphs.keys`)       |
  | `4ec4c1b` | Crash fix: index of the native tile handler table                                            |
  | `cd074f2` | Crash fix: HBlank callbacks copied into IWRAM                                                |

- **How it works.**
  - The game's C (`src/`) is compiled for x86-64 with `PORTABLE`.
  - GBA memory is reserved at its real addresses, and pointers in game memory stay 32-bit
    (`GBA_PTR`).
  - The I/O page is trapped and single-stepped (`port/src/gba_io.c`).
  - The game runs as a coroutine on a low stack. `port/src/frame.c` runs one GBA frame at a
    time: VBlank, HBlank effects and DMA.
- **What works.** Title, file select, passage select, the first level, music and sound
  effects, and saves.

## Building

Toolchains are in `C:\Users\Benny\Decomp\toolchains`:
- llvm-mingw: `llvm-mingw-20261006-ucrt-x86_64`
- CMake and Ninja: `buildtools-venv\Scripts`
- SDL3: `SDL3-3.4.18\SDL3-3.4.18\x86_64-w64-mingw32\lib\cmake\SDL3`

### PC build

`build_pc` and `build_pc_asan` are already configured. To rebuild (Git Bash):

    export PATH="/c/Users/Benny/Decomp/toolchains/buildtools-venv/Scripts:/c/Users/Benny/Decomp/toolchains/llvm-mingw-20261006-ucrt-x86_64/bin:$PATH"
    cmake --build build_pc

To configure from scratch:

    cmake -S . -B build_pc -G Ninja --toolchain port/cmake/llvm-mingw-x86_64.cmake \
          -DLLVM_MINGW=C:/Users/Benny/Decomp/toolchains/llvm-mingw-20261006-ucrt-x86_64 \
          -DCMAKE_BUILD_TYPE=RelWithDebInfo \
          -DSDL3_DIR=C:/Users/Benny/Decomp/toolchains/SDL3-3.4.18/SDL3-3.4.18/x86_64-w64-mingw32/lib/cmake/SDL3

### ASan build

Configure the same way into `build_pc_asan` with `-DWL4_ASAN=ON`, then
`cmake --build build_pc_asan`. To run it, also put the ASan DLL's folder,
`.../llvm-mingw-20261006-ucrt-x86_64/x86_64-w64-mingw32/bin`, on `PATH`.

### GBA byte-identity check

Run this from PowerShell, not Git Bash, which rewrites `/mnt` paths. Use a new `NN` each time:

    wsl --cd /mnt/c/Users/Benny/Decomp/warioland4-port --exec sh -c 'make -j16 BUILD=build/v7-19 check > build/v7-19.log 2>&1; md5sum build/v7-19/*.gba; cmp build/us/*.gba build/v7-19/*.gba && echo ROM-SAME; diff -rq build/us/obj build/v7-19/obj && echo OBJ-SAME'

Both `ROM-SAME` and `OBJ-SAME` must print. `make check` itself reports an MD5 failure,
because 11 functions are still unmatched; that is expected.

### Regenerating `port/generated/`

Do this after changing game sources. See `port/README.md`, "How game code builds for PC":
1. Build `build/playtest/warioland4_us_playtest.elf` in WSL.
2. Build the `wl4_game` target.
3. Run `python port/tools/gen_symbols.py --elf build/playtest/warioland4_us_playtest.elf --lib build_pc/libwl4_game.a`.

## Running

The ROM must be passed by path. A copy is in the main tree, `C:\Users\Benny\Decomp\warioland4-2\baserom_us.gba`;
the worktree's `baserom_us.gba` is a WSL symlink, which Windows programs cannot read.

    build_pc\wl4.exe C:\path\to\baserom_us.gba                  # window, scale 3
    build_pc\wl4.exe ROM --save C:\path\to\test.sav             # window with a chosen save file
    build_pc\wl4.exe ROM --frames 1400 --input port\tools\inputs\file_select.keys --dump port.bin
    build_pc\wl4.exe ROM --frames 6800 --input port\tools\inputs\hall_of_hieroglyphs.keys --dump port.bin --from-step 4600
    build_pc\wl4.exe ROM --selftest --test-bios --test-io       # unit tests (all must pass)

**Keys** (mGBA's defaults):

| Key           | Does         |
|---------------|--------------|
| Arrows        | D-pad        |
| X             | A            |
| Z             | B            |
| A             | L            |
| S             | R            |
| Enter         | Start        |
| Backspace     | Select       |
| Tab (held)    | Fast forward |
| F11           | Fullscreen   |
| Escape        | Quit         |

**Headless options:**
- `--dump FILE` writes GBA memory each VBlank.
- `--shots FILE` writes the pictures.
- `--wav FILE` writes the sound.
- `--mgba-colors` blends colours as mGBA does, for exact picture diffs.
- `--save FILE` keeps SRAM in that file. Headless runs save only with it, and they overwrite
  it, so give them a copy.

A fault prints the GBA function it happened in, for example
`in or after DrawMapScreenSprites+0x467 (GBA 0x0807C24C)`. Map the
`executable+0x...` offset with `llvm-symbolizer` or `llvm-objdump -d` (image base `0x140000000`).

## mGBA dumps and diffs

mGBA 0.10.5 is at `C:/Users/Benny/Downloads/mGBA-0.10.5-win32/mGBA-0.10.5-win32/mGBA.exe`.

- **`port/tools/run_mgba_dump.ps1`** makes an mGBA dump with nobody at the keyboard.
  1. It copies the ROM into a new run directory.
  2. It starts mGBA and loads `mgba_dump.lua` through the Scripting window, using UI
     Automation.
  3. It waits for the dump, then closes mGBA.

  Options: `-Frames N -Out FILE`, `-InputScript FILE`, `-FromStep S`, `-Shots`
  (a screenshot per frame in the run directory), `-Save FILE` (boot with that save instead of
  erased SRAM) and `-RunRoot DIR`.

      powershell -ExecutionPolicy Bypass -File port/tools/run_mgba_dump.ps1 -Mgba <mGBA.exe> `
          -Frames 1400 -Out mgba.bin -InputScript port/tools/inputs/file_select.keys

- **`port/tools/mgba_dump.lua`** is the Lua driver. It erases SRAM (or loads `WL4_SAVE`),
  resets the game, and at each VBlank start writes the same dump format as `wl4 --dump`
  (EWRAM, IWRAM, I/O, palette, VRAM, OAM, SRAM). It presses input-script keys and counts
  main-loop steps exactly as wl4 does. It is configured by environment variables, which the
  `.ps1` sets: `WL4_FRAMES`, `WL4_DUMP`, `WL4_INPUT`, `WL4_FROM_STEP`, `WL4_SHOTS`, `WL4_SAVE`.
- **Input scripts** (`port/tools/inputs/*.keys`, format in
  `port/include/port/input_script.h`) are `STEP KEYS` lines, for example `120 A` or
  `300 RIGHT+B`. A step is a *finished* main-loop step: `gMainTimer` changed, and the loop
  waits for VBlank with bit 0 of `gInterruptCheck` clear. Counting by steps rather than frames
  keeps both sides pressing the same keys despite mGBA's lag frames. `wl4 --input` also works
  in the window.
- **`python port/tools/diff_frames.py PORT.bin MGBA.bin`** compares memory, one frame per
  finished step on each side. Options:
  - `--mask-sound` leaves out the sound engine's state and registers.
  - `--list` lists all differences.
  - `--show N` shows one step.

  The masks for state the port does not model are listed in the script, each with its reason.
- **`python port/tools/diff_screens.py PORT.bin PORT.shots MGBA.bin RUN_DIR`** compares
  pictures. Run wl4 with `--shots ... --mgba-colors` and mGBA with `-Shots`. `--save DIR`
  writes the differing frames side by side.
- **Reference results:**
  - `file_select.keys`, 1400 frames: 1,394 steps identical, pictures included.
  - `hall_of_hieroglyphs.keys`, 6800 frames from step 4600: 2,190 steps identical with
    `--mask-sound`.
  - With a save, through the level: 1,671 steps identical.

## Known gaps

- **Music drifts after mGBA's lag frames.** In the intro, title, file select and level
  transitions, the game runs `SoundMain` from its VBlank callback. On mGBA a lag frame (a
  step longer than one frame) adds a music tick and moves `pcmDmaCounter` on. The port has no
  lag frames, because game code takes no time, so the sound state matches only up to the first
  lag frame (file select: steps 1–158). After that, use `--mask-sound`. It sounds right; only
  the diff loses track.
- **Sound paths not yet compared with mGBA:** the fixed-frequency mixer, echo, reverb, and the
  CGB (PSG) channels driven by the engine. A lag-frame replay would cover them: running
  `SoundMain` an extra time where mGBA lagged.
- **Game code takes no time.** Raster effects that poll VCOUNT mid-step are approximate. One
  picture frame in the level differs (the vortex setup).
- **Open bus at unused addresses** returns `0xE3A02004`. The real GBA returns the CPU's
  prefetch, which the port cannot know (see `port/src/gba_memory.c`).
- **Smaller gaps:**
  - BIOS `GetJumpList` (0x2A) is unimplemented.
  - UBSan has not been tried.
  - 295 compiler warnings remain.
  - Two header pairs conflict: `save_reset.h`/`save_data_clear.h` and
    `stage_entry.h`/`boss_door_opening.h`.
  - `StartLargeLipsKiss`'s fifth argument still needs confirming against mGBA.

## M3 plan: a full playthrough under the diff harness

Get the user's approval before starting.
1. **More input scripts**, passage by passage from Hall of Hieroglyphs onward (Entry Passage,
   then Emerald, Ruby, Topaz, Sapphire, Golden Pyramid). Use `hall_of_hieroglyphs.keys` as the
   template, and work out the keys in the window or by bisecting with dumps. Save a `.sav` at
   each passage boundary, so later scripts can start from it in both the port (`--save`, on a
   copy) and mGBA (`-Save`).
2. **Diff each script against mGBA** with `diff_frames.py --mask-sound` and `diff_screens.py`.
   Fix every divergence at its first differing step: dump both sides and compare the globals
   the code involved touches. Bosses, minigames, the shop and the ending are new code paths.
3. **Crashes.** Use the fault report's GBA function, then `llvm-objdump` at the executable
   offset. Check whether mGBA reads the same state, as with the open-bus case.
4. **Lag-frame replay (optional).** Record which steps lagged in mGBA, then have the port run
   the VBlank-callback `SoundMain` an extra time there. The sound state then stays comparable,
   which also tests the untested sound paths.
5. **Before stopping each session:** verify byte identity, commit by name, push to
   `myfork/port`, and update "Where we are" in `docs/PC_PORT_PLAN.md`.

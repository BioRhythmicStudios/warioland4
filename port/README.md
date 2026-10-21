# PC port

The plan and the decisions behind it are in [docs/PC_PORT_PLAN.md](../docs/PC_PORT_PLAN.md).
The GBA ROM is still built with `make`. The PC build compiles the same `src/` with
`PORTABLE` defined, and must not change the ROM: every change to shared code is checked
by rebuilding the ROM and comparing it and all objects with the previous build.

## Building (Linux x86-64)

The first Linux build is tested on Ubuntu 26.04.1 under WSL2/WSLg with Clang
21.1.8 and SDL3 3.4.2. It is a native ELF executable. See
[Linux implementation and verification](../docs/LINUX_FIRST_BUILD.md) for
sanitizer instructions, test limits and the initial WSLg audio observations.

Install `clang cmake ninja-build pkg-config libsdl3-dev` using your package
manager. Clang is currently required for the existing DMA volatile ordering.
The executable loads your separately supplied, unmodified supported ROM.
No extraction step, ROM data, Windows launcher or Wine is required.

```sh
cmake -S . -B build/linux -G Ninja -DCMAKE_C_COMPILER=clang \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/linux -j4
mkdir -p build/play
SDL_VIDEODRIVER=x11 WL4_SETTINGS_FILE="$PWD/build/play/settings.cfg" \
    ./build/linux/wl4 /path/to/your/baserom_us.gba \
    --save "$PWD/build/play/wl4.sav"
```

The explicit X11 selection is the backend tested through WSLg; direct Wayland
and physical Linux desktops have not yet been accepted. This launch example
uses separate Linux save/settings files under `build/play`. Keep those files
if you play there; do not clean that directory. Ordinary launches without
these overrides use SDL's Linux preference directory, separate from Windows.
Default keys: arrows, X/Z for A/B, A/S for L/R, Enter for Start and Backspace
for Select. F11 toggles fullscreen, Tab fast-forwards and Escape closes/saves.
The existing binding, VSync, CRT and rumble settings format is retained, but
there is no Linux launcher or graphical settings editor in this milestone.

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

## Native controller rumble (v0.3.0-beta)

Settings -> Controller provides Rumble On/Off, strength (0-100%) and Test Rumble.
Rumble defaults Off for new and existing configurations. Save and restart the game
to apply settings. Unsupported controllers and keyboard-only play remain available.
Native SDL3 effects cover damage, impacts, enemy attacks, breakable objects,
switches, boss damage and transformations; no ROM hack or modified ROM is needed.
Effects stop on disconnect/shutdown and scheduled pulses never replay on reconnect.

Version-4 settings read versions 1-3 without rewriting, preserving bindings,
CRT and VSync preferences. Explicit Save upgrades the file; older releases cannot
read v4. SRAM format is unchanged. See [rumble effects and tests](../docs/WINDOWS_RUMBLE.md)
and [Beta 3 release verification](../docs/WINDOWS_BETA3_RELEASE.md).

## Optional CRT display filters (v0.2.0-beta)

Settings -> Display offers Original (Off), CRT Lite and CRT Enhanced. Click the
filter selector to cycle presets. Original is the default, including when loading
older settings. Lite adds adjustable scanlines (0-50%). Enhanced adds mild glow
(0-30%) and an optional phosphor mask. Save and restart the game to apply changes.

These are lightweight SDL rendering filters, without a programmable-shader backend
or additional runtime DLLs. They affect only the final displayed image; the software
PPU, headless screenshots, simulation, audio and VSync scheduling are unchanged.
Scanlines follow integer-scaled GBA rows and are suppressed at 1x; the RGB mask
needs at least 3x. Glow can be disabled independently. Unsupported filter resources
fall back to Original for the session with a diagnostic. Save warnings stay clear
above the filter. See [implementation and tests](../docs/WINDOWS_CRT.md).

## Windows launcher

Display settings include VSync On/Off. Save, then start a new game to apply it.
New configurations default to On where supported; older profiles retain Off
until changed. Gameplay stays at the GBA's original rate independently of display
refresh. See [VSync implementation and test limits](../docs/WINDOWS_VSYNC.md).

After building with SDL3, double-click `wl4-launcher.exe` in `build_pc`.
Keep it beside `wl4.exe` and the matching x64 `SDL3.dll` (the build copies SDL3).
Choose **Browse...**, select your own legally obtained ROM, then **Launch Game**.
The gold **Selected ROM** line shows the filename clearly beneath the full,
editable path. It updates when browsing, editing or restoring the remembered
selection. Long filenames may be visually ellipsized; the full path remains in
the horizontally scrolling edit field, with its existing length limit. Paths with
spaces are kept intact. This display does not change validation or game launch.

The purple-and-gold launcher includes rounded gold buttons and rounded window
corners (native Windows 11 rounding, with a region fallback on older Windows).
**About** describes the project, **CREDITS** lists contributors and font provenance,
and **README** opens a built-in quick-start guide; **Settings** customizes controls and display.

The launcher has a text-only layout. Its neutral purple/gold square icon is
original geometry generated by `port/tools/make_neutral_icon.py`; it contains no
game artwork. The earlier third-party sprite and icon files were removed from the cleaned
Git history and are not included in the launcher or release.
No external artwork or Windows Imaging Component decoder is used.

The launcher uses the same crisp pixel font as Settings, embedded privately for
its labels, buttons, ROM-path field and About/CREDITS/README/error windows. No
system font installation is needed. Windows title bars and the standard Browse
dialog retain their operating-system font. The window is slightly wider to fit
the square glyphs.

The generated `neutral-placeholder.ico` is embedded in 16/32/48/256-pixel
sizes for the executable, taskbar, Settings and information windows. Desktop
shortcuts inherit that neutral icon; none are created or changed by the launcher.
Regenerate it with `python port/tools/make_neutral_icon.py` (build-time only).
See [font provenance](assets/fonts/README.md) for font regeneration.

The launcher accepts only the unmodified 8 MiB US release (game code AWAE), with
MD5 `5fe47355a33e3fabec2a1607af88a404`, using the same validator as the game.
No ROM is included, copied, downloaded or installed by the launcher.

Window scale (1x-6x) uses the game's existing `--scale` option; press F11 in the
game to toggle fullscreen. About credits BioRhythmicStudios and the original
decompilation contributors. The ROM path and scale are remembered per Windows
user (ROM path in `HKCU\Software\BioRhythmicStudios\WL4Launcher`, scale in the shared settings file). A successfully browsed
ROM is remembered immediately; scale changes are remembered when launching.
The ROM is checked again on every launch. Errors explain invalid/missing ROMs,
missing game/runtime files and unsuccessful child-process exits.

The launcher starts the adjacent `wl4.exe`, quotes the absolute ROM path and uses
the executable's directory as the working directory. It passes no save override:
the game's existing `%APPDATA%\wl4\wl4.sav` location and save recovery remain in
charge. Closing the launcher leaves an already running game open. The launch
button is disabled while its child game runs; this does not prevent separately
started instances. Direct command-line usage remains available:

    build_pc\wl4.exe "C:\path with spaces\baserom_us.gba" --scale 3

The game's current narrow-character file API limits supported path characters to
those representable by the Windows active code page; the launcher reports a
clear error on lossy paths. UTF-8 Windows code pages are supported. See
[launcher verification](../docs/WINDOWS_LAUNCHER_V1.md) for results and limits.

## Settings and customizable controls

Click **Settings** in the launcher to open Keyboard, Controller and Display tabs.
Changes are drafts until **Save**; **Cancel** discards them. **Defaults** restores
only the current section. Saved changes take effect when the game next starts.

- **Keyboard:** click an action's binding and press the next physical key.
  Escape cancels capture. Duplicate bindings are rejected without losing the
  previous mapping. Escape (quit), Tab (fast-forward), F8 (save details) and F11
  (fullscreen) remain reserved. All ten GBA actions can be rebound.
- **Controller:** SDL3 supports Xbox/XInput-compatible gamepads. Connected names
  update while devices are connected or disconnected; additional names cycle on
  the second line. The game uses the first available supported controller and
  reopens an available controller after disconnect. Click a binding to choose a
  face button, shoulder, D-pad, stick click, supported extra button or LT/RT.
  The chooser also offers **Press input** to capture a controller button/trigger.
  Bindings can be edited without any controller present. Duplicate controller
  bindings are rejected. Keyboard and gamepad work together.
- **Movement:** D-pad bindings work independently of the optional left analog
  stick. Stick movement can be disabled; its dead zone is adjustable from 10% to
  80% (default 50%). Triggers activate past half travel.
- **Display:** choose a window scale from 1x to 6x and startup fullscreen. F11
  still toggles fullscreen in-game. The main launcher's scale control stays in
  sync. Direct `wl4.exe` launches read the same configuration; an explicit
  `--scale N` overrides the saved scale for that run.

Configuration lives in the Windows user's application configuration directory:
`%APPDATA%\wl4\settings.cfg` on a standard installation, resolved by SDL's
preference-folder API. It is separate from the 32 KB `wl4.sav`; save paths and
save handling are unchanged. The launcher keeps its existing registry ROM-path
persistence and imports its old scale when no settings file exists. Settings are
written by **Save** or **Launch Game**, using an exclusive temporary sibling and
atomic replacement. Missing files use defaults; invalid/truncated/outdated files
show a warning and load defaults without rewriting the file during load.

The launcher now also uses the adjacent SDL3.dll for the Settings window and
controller support. Its Vampire Wario artwork and MisteryGX attribution remain.
See [settings implementation and verification](../docs/WINDOWS_SETTINGS.md).

Developer tests may explicitly set `WL4_SETTINGS_FILE` to a disposable config path.
For example, run `wl4-input-test.exe --isolated` with that variable naming a **new**
file under `build/`. Never rely on changing APPDATA to isolate SDL saves: Windows
SDL resolves the known preference folder independently. Every automated game test
must pass an explicit disposable `--save` path.

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
| `tools/port_pin_codemod.py` | `register ... asm("rN")` Ã¢â€ â€™ `PIN("rN")` |
| `tools/port_asm_barrier_codemod.py` | empty-template `asm` Ã¢â€ â€™ `ASM_BARRIER()` |
| `tools/port_data_codemod.py` | data definitions in `src/` Ã¢â€ â€™ `#ifndef PORTABLE` blocks |
| `tools/port_struct_align_codemod.py` | `struct Foo {` Ã¢â€ â€™ `struct AGB_ALIGN Foo {` |

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

`wl4.exe [ROM] [--scale N] [--frames N [--dump FILE] [--shots FILE] [--wav FILE] [--from-step S]] [--input FILE] [--save FILE] [--mgba-colors] [--selftest] [--test-bios] [--test-io] [--fault rom-write|unmapped|stack-overflow] [--asan-probe]`

With no `--frames` or tests, the game runs in a window (`port/platform/sdl/sdl_frontend.c`), at
`--scale` times 240x160 (default 3). The window scales in whole steps. The keys are mGBA's
defaults: arrows, X = A, Z = B, A = L, S = R, Enter = Start, Backspace = Select. Tab
fast-forwards while held, F11 toggles fullscreen and Escape quits. The first gamepad works by
button position (bottom = A, right = B). Frames are paced at the GBA's rate, 2^24 Hz / 280896
cycles (about 59.73 a second). Sound plays on the default device. The system clock that paces
the frames drifts from the device's, so the sound stream plays up to 0.5% faster or slower to
keep about 4 frames queued. Fast forward drops the sound. If the game faults, the report goes
to stderr and, in the window, to a dialog as well.

`port/src/save.c` keeps the cartridge's 32 KiB of SRAM in mGBA-compatible `.sav` format.
The window uses `%APPDATA%\wl4\wl4.sav`, or `--save FILE`; headless runs save only with
`--save`. Give tests disposable copies. After 30 stable frames the port writes an exclusive
temporary sibling, flushes/closes it and atomically replaces the destination. Failed writes
keep the last good file and latest SRAM in memory, then retry after at least 300 game frames
once SRAM is stable. A nonmodal warning remains visible until recovery; F8 shows the full
save destination and Windows error. Failed shutdown saves offer Retry saving, Return to game
or explicit Quit without saving. Headless final-save failure exits 1. Unreadable saves and
files not exactly 32768 bytes are rejected without opening a writable session. Independent
concurrent writers are not coordinated. See [the save-recovery report](../docs/M3_SAVE_RECOVERY.md)
for verification and limits.

`--test-save DIRECTORY` runs disposable filesystem failure/recovery tests in a new child of
the specified existing directory. Use a directory under `build/`; test files remain as evidence.
The test never uses the default AppData save.

At startup, `port/src/gba_memory.c` reserves the GBA regions at their real addresses
(`port/include/port/gba_memory.h`). It also reserves the game stack at `0x01020000`Ã¢â‚¬â€œ`0x01810000`,
with a guard page, 60 KiB of headroom and inaccessible blocks around it. A vectored
exception handler reports faults on GBA addresses (for example a write to the read-only
ROM) and overflows of the game stack. `port/src/rom.c` loads the ROM (default
`baserom_us.gba`), checks its size and MD5, and makes it read-only. The game runs as a
minicoro coroutine on the low stack (`port/src/game_thread.c`).

The game sometimes reads through a pointer it has not set yet, as the GBA lets it: on the
first frame of the passage map after leaving a stage, `DrawMapScreenSprites` reads animation
tables from address 0, then a sprite count through the pointer it found there. The GBA reads
its BIOS's open bus there, the last opcode the BIOS fetched (`0xE3A02004` after the SWI the
game waits for VBlank with), then the open bus of an unused address. So does the port. Before
the reporting handler, a second one redirects faults on 32-bit addresses as
`port/src/gba_memory.c` says. Reads from the BIOS and unused addresses see `0xE3A02004`, and
writes there are dropped. The mirrors of EWRAM, IWRAM, palette RAM, VRAM and OAM go to that
memory. `port/src/x86_access.c` finds the register the faulting instruction takes its address
from. The handler moves it by the difference, runs the instruction with the trap flag set,
and moves it back. Only loads, stores, arithmetic and compares are redirected, and only when
that register serves just as the address. Anything else, and faults on the cartridge (such as
a write to the ROM), are reported as before. On the GBA an unused address returns the CPU's
prefetch, an opcode near the reading instruction, which the port cannot know. In the one case
seen, the count it gives is too large and the game draws nothing, as with mGBA's value.

`port/src/bios.c` runs the BIOS calls the game makes:
- `CPUSet` and `LZ77UnCompVram` follow the hardware, including LZ77's halfword writes,
  which read stale data for a back-reference at distance 1.
- `Halt` yields from the game coroutine until the frame loop requests an interrupt.
- Any other call is reported and stops the game.

`port/src/gba_runtime.c` has the libgcc division helpers and m4a's `umul3232H32`.
`port/src/m4a_native.c` is the m4a sound engine's asm (`asm/m4a_asm.s`) in C: `SoundMain` and its
mixer, `MPlayMain`, `ply_note` and the other `ply_` commands. Each routine follows the asm step
by step, quirks included, so the channels, tracks and PCM buffer in GBA RAM come out as on the
GBA. The engine reads song data through a guard that makes BIOS addresses read as 0; the port
checks first, since it cannot read there at all. `MPlayMain` and `ply_note` take the arguments
the asm passes them, so `include/gba/m4a.h` declares them that way for the PC.

`port/src/audio.c` is the sound hardware, written from GBATEK's description:
- **PSG.** The two square channels (the first with its sweep), the wave channel with its two
  banks of wave RAM, and the noise channel. Each has its length counter and envelope, clocked
  by a 512 Hz frame sequencer.
- **Direct Sound.** FIFOs A and B play a byte on each overflow of their timer, and ask DMA 1
  or 2 for 4 more words when 4 or fewer are left. The DMA reads the PCM buffer that
  `SoundMain` mixes.
- **Mixing.** As SOUNDCNT and SOUNDBIAS set it, into the 10-bit DAC around the bias level.
  The PSG's balance against Direct Sound is mGBA's. The output is 65536 Hz, SOUNDBIAS's
  8-bit rate, which the game sets, and a filter blocks DC as the GBA's output capacitor does.

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
  - **Sound.** The bytes a store wrote to the sound registers, wave RAM or FIFOs go to
    `port/src/audio.c`, in order. `port/src/x86_access.c` reads the store's size from the
    instruction, because a write-only bit may be written with the 0 it reads as. The
    registers then read as mGBA keeps them: write-only bits as 0, SOUNDCNT_X with the
    channels that are playing, and wave RAM with the bank that is not playing. As the
    display moves on, so does the sound hardware, a scanline at a time.
- A trapped access costs about 4 Ã‚Âµs.

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
`--wav FILE` writes the sound, 16-bit stereo at 65536 Hz.
`--from-step S` starts all three at the frame on which main-loop step S finishes, to look at a
late scene without gigabytes of dump (each frame is 419 KiB).
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
           -Mgba C:\path\to\mGBA.exe -Frames N -Out MGBA.bin [-Shots] [-InputScript FILE] [-FromStep S] [-Save FILE]

   It copies the ROM into a new run directory under `%TEMP%` and starts mGBA. It loads
   `port/tools/mgba_dump.lua` through the Scripting window and closes mGBA once the dump is
   written. `-Shots` also saves a screenshot of every frame in the run directory
   (`wl4-N.png` goes with the N-th frame dumped, from 0). `-InputScript` presses keys as
   `wl4 --input` does, and `-FromStep` starts the dump as `wl4 --from-step` does. `-Save`
   boots with a save file, for comparing with `wl4 --save` (give wl4 a copy: it writes it).
   `Get-Help port/tools/run_mgba_dump.ps1 -Full` has the details.

   To do it by hand, load the ROM and choose Tools > Scripting... > File > Load script...,
   then pick `port/tools/mgba_dump.lua`. The script erases SRAM (or loads `%WL4_SAVE%` into
   it), resets the game and writes the dump to `%WL4_DUMP%` (default `%TEMP%\wl4_mgba.bin`).
   `%WL4_FRAMES%` sets the frame count, `WL4_SHOTS=1` turns the screenshots on, `%WL4_INPUT%`
   names an input script and `%WL4_FROM_STEP%` sets the first step to dump. It writes `MGBA.bin.done` when finished.
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
VCOUNT/DISPSTAT status, timers, write-only DMA and FIFO registers, and the open bus that rooms
without a raster effect copy as their HBlank callback (the last value any DMA moved, on mGBA
usually sound data).

The sound engine's state is compared too, and matches until the first frame mGBA lags: a step
that takes the GBA more than a frame. Where the game runs `SoundMain` from its VBlank callback
(the intro, the title, file select, the level transitions), a lag frame is an extra music tick
and moves `pcmDmaCounter` on; the port, whose steps take no time, has neither. From there the
music runs ahead on mGBA. `--mask-sound` leaves the sound engine's state and the sound registers
out, to compare the rest.

`--selftest` runs the coroutine through yields and 2 MiB of recursion. `--test-bios` runs
the BIOS unit tests (`port/src/bios_test.c`). These include decoding every LZ77 stream the
game passes to `LZ77UnCompVram`, and comparing the result with a plain byte decoder, and
reads and writes of the BIOS, unused addresses and mirrors by several kinds of instruction.
`--test-io` tests the trapped registers on the game coroutine, and times the traps. It covers
the sound registers too: their masks, a square wave's pitch, length counters, the wave RAM
banks, and a FIFO fed by DMA. `--fault` and
`--asan-probe` trigger the fault reports and an AddressSanitizer report on the game stack.
To build with AddressSanitizer, configure with `-DWL4_ASAN=ON`. Then put
`llvm-mingw/x86_64-w64-mingw32/bin` on `PATH` for `libclang_rt.asan_dynamic-x86_64.dll`.

# Handover: Wario Land 4 PC port (2026-10-18)

The working rules are in `AGENTS.md`, so read that first. The full plan and its decisions are
in `docs/PC_PORT_PLAN.md`; its "Where we are" section is the most detailed status. Each
runtime piece is documented in `port/README.md`.

## Latest: launcher settings and customizable controls (2026-10-19)

Settings milestone `617c6d8` follows launcher/theme commits `bf9734f`, `fc143c3`, `1fd013f`.
Read `docs/WINDOWS_SETTINGS.md` and the newest Where we are entries first.
Keyboard/Controller/Display settings are shared by launcher and game in SDL's
user preference folder, separate from SRAM. Physical key capture, collision checks,
button/trigger remapping, hotplug, analog/dead zone and startup display preferences
are implemented. Normal/ASan automated checks pass. User manually confirms keyboard
remaps, Xbox-compatible controller operation, reconnect and keyboard fallback.
Latest GBA identity: **v7-36**, ROM-SAME, OBJ-SAME, MD5
`940b597bef3be3f4041021b47a2f4096`; next fresh directory **v7-37**.
User approved a normal push to `myfork/port`; the pre-push fetch found no remote-only commits.
No release is authorized. Further milestone work needs approval; M3 route extension is paused.

Important test correction: changing APPDATA does NOT isolate SDL_GetPrefPath on
this Windows build. Older launcher/theme GUI logs show the normal save was opened;
the old never-accessed claims are withdrawn. Always use explicit disposable --save
paths for game tests, and WL4_SETTINGS_FILE for disposable config tests. This
milestone did so. Before/after player-save hashes differ during user manual testing;
do not restore an older save over their current progress. Preserve all evidence in
`build/settings-20261019/` and earlier directories. Details and limits are documented.

## Latest: reliable save errors and recovery (2026-10-18)

User-approved implementation `771dcc2` follows `7ba5638`. See
`docs/M3_SAVE_RECOVERY.md` for the failure inventory, behavior, tests and limits.
Native-only changes: nonmodal warning/F8 details, 300-frame retry after failure,
retained latest SRAM and a failed-close Retry/Return/explicit-Discard prompt;
headless final failures now exit 1. Exclusive temporary siblings preserve the old
file on failure. Invalid-size/unreadable saves are rejected without a writable
session. Ordinary 32 KiB save format and 30-stable-frame debounce remain unchanged.

Normal/ASan runtime and disposable-file tests pass. Window tests cover F8, continued
play, automatic recovery and all three close choices. Both builds save the Hall
8500-frame route and reload recognized file A in a fresh process; save hashes match
the prior Hall reference. All tests used explicit new build paths: the real AppData
save was not accessed. Evidence: `build/m3-save-recovery-20261018/`; no prior evidence
was deleted and no test processes remain active. No release/package was created.
Final GBA verification **build/v7-29** prints ROM-SAME and OBJ-SAME with MD5
`940b597bef3be3f4041021b47a2f4096`; next fresh directory **v7-30**.
Next proposed task: first-launch ROM/dependency UX, awaiting milestone approval.
Multi-instance save conflicts, power-loss recovery of pending RAM and rotating
backups remain outside this focused change. Earlier statuses below are historical.


## Latest: manual M3 results and private-alpha review (2026-10-18)

Read `docs/M3_ALPHA_READINESS.md` first for current coverage and readiness. User
manual results now confirm Hall completion (Frog Switch and escape), Spoiled Rotten
victory/return to the Golden Pyramid, two Wario Hop rounds through Game Over,
zombie transformation/platform fall-through, bat transformation/flight and save
progress restored after closing/reopening. These are manual observations, not new
sanitizer runs or full automated equivalence. Exact build/device details were not
supplied. The older request for manual Hall completion is now satisfied; the old
fixture and automated-route limits remain unchanged.

Reviewed HEAD `f3d22a5` preserves the README/screenshot work merged since `38c49ef`;
Frog Switch implementation remains `c25cc4d`. Documentation review only: no runtime
changes or package/release. Guided private testing is reasonable; an unattended
alpha package still needs save-error visibility, startup/dependency UX, durable
failure evidence and clean-PC/controller acceptance. First proposed implementation:
visible save-write failure with rate-limited pending-data retry. Await the user's
approval before that milestone task. Do not extend the full playthrough implicitly.

Fresh GBA verification for this documentation update: `build/v7-26`, ROM MD5
`940b597bef3be3f4041021b47a2f4096`, ROM-SAME and OBJ-SAME. Next fresh build: v7-27.
Older build numbers below are historical. All prior recordings and artifacts remain
intact; this review's small negative-ROM/import-check artifacts are under ignored
`build/m3-alpha-review-20261018/`.


## Priority Frog Switch bug follow-up (2026-10-18)

The user authorized this focused bug task; general playthrough extension remains paused.
Implementation `c25cc4d` follows `ff28d49`. See `docs/M3_FROG_SWITCH.md` for the full
LLDB evidence and verification. HBlank VCOUNT reads were advancing display time,
consuming VBlank inside the callback and starving the main-loop wakeup. IRQ display
reads now observe the current event; ordinary polling still advances. Game sources,
generated symbols and existing route scripts are unchanged.

Normal and ASan unit tests (including the new HBlank regression), the unchanged
25,200-frame room-6 route and positioned activation/countdown/movement/exit fixtures
pass. Both exit fixtures return to stage select. They position Wario above the
switch and later in the vortex using process RAM, so an uninterrupted input-only
switch-to-exit traversal is not claimed. All old work is preserved; new artifacts
are in `build/frog-freeze-20261018/`. All diagnostic processes finished or were closed.
Final byte-identical GBA check: **build/v7-25**, MD5
`940b597bef3be3f4041021b47a2f4096`, ROM-SAME and OBJ-SAME. Next: **v7-26**.
Next useful check is manual switch-to-exit traversal with rebuilt normal/ASan binaries.

## M3 paused by the user (2026-10-18; read this before the older M2 notes)

- **Do not resume playtesting until the user asks.** All test processes were finished when paused. No new long run or rebuild was started for the pause. Latest implementation commit: `6a0a186`, following `feb6eb4`. The pause documentation is a separate commit.
- **Completed fix:** `src/wario/swimming.c`, two `PORTABLE` pointer additions in `ResolveWarioWaterSurfaceCollision`. The old unsigned-negation pointer subtraction produced `FFFFFFFF082DEB70` instead of `082DEB70` on x86-64. Both original GBA expressions remain in `#else` branches. Normal and ASan reproduced the fault before the fix and pass afterward. Generated symbols were regenerated and have no content changes.
- **GBA identity:** fresh `build/v7-21`, `v7-22` and `v7-23` all printed `ROM-SAME` and `OBJ-SAME`, MD5 `940b597bef3be3f4041021b47a2f4096`. `v7-23` is the latest, after diagnostic removal. The next fresh directory is `build/v7-24`. As before, `make check` itself fails the base-ROM checksum because unmatched functions remain.
- **`src/block.c`:** only our temporary stdio include and conditional `printf` were removed, by exact edits. The file now has **no Git diff**. No backup was restored over it. `block-probe.log` and `block-before-probe.c` remain under the ignored artifact directory; do not copy the backup over future work.

### Routes and exact pause positions

- `port/tools/inputs/hall_room11.keys` and `hall_of_hieroglyphs.keys` are unchanged.
- **Committed continuation:** `port/tools/inputs/hall_room6_entry.keys`, exactly the tested `build/m3-hall-exit/route-room5.keys` inputs with comments added. It preserves every room-11 input and reaches room 6, `(184, 463.75)`, standing, after 25,200 native frames. Two jewel pieces (NE and SE) have been collected. Frog Switch and level exit have **not** been reached.
- **Preferred native checkpoint, not committed:** `build/m3-hall-exit/hall_room6_middle.keys`. This freezes the successful route through the rock throw, ladder and middle platform. At 31,000 native frames: room 6, `(312, 319.75)`, normal reaction, standing (pose 2), carrying nothing. Its capture is `climb6.bin` / `climb6.shots`, screenshot `climb6-31000.png`. It drops the rock at step 30,400, jumps with A+Up at 30,420 and climbs to the platform; the ladder ends there. A second rock is at `(408, 320)`; the lower rock is at `(281.25, 432)`. This climb is native-verified; its mGBA comparison remains pending.
- **Farthest native probe, not committed:** `build/m3-hall-exit/hall_room6_upper_probe.keys` (frozen copy of `route-climb.keys`). At 33,400 frames: room 6, `(343.5, 191.75)`, crouching (pose 5), carrying nothing, stopped against brown blocks in the low upper passage. Capture: `climb10.bin` / `.shots`, screenshot `climb10-33400.png`. The right platforms can be climbed after dropping the heavy rock. No new collision code bug has been established here.
- **Next movement to investigate:** use the preferred middle checkpoint, inspect/pick up the second rock, and investigate an upward-left throw toward the brown upper-passage blocks near `(304..320, 176)`. This is a route hypothesis, **not verified**. Carrying a rock prevents ladder grabs and reduces jump height; charged throws can bounce off solid ceiling. Earlier probes repeatedly confused those normal behaviors with input failures. Continue toward the other jewel pieces, CD, Keyzer, Frog Switch and exit only after the gate is solved.
- `route-carry-stairs.keys` is an unsuccessful alternative: the heavy rock prevented a higher jump and that probe returned to room 5. Do not use it as the preferred checkpoint.

### Verification and limitations

All comparisons below use finished main-loop steps and existing masks; memory comparisons add `--mask-sound`, pictures use `--mgba-colors`. No masks or renderer behavior changed.

| Existing capture | Native / mGBA frames; first step | Memory result | Pictures |
|---|---|---|---|
| `a3.bin`, `a3-mgba.bin` | 10700 / 10900; 8500 | 2190 steps identical | Three score-digit timing frames, 6-8 pixels each |
| `c6-full-native.bin`, `c6-mgba.bin` | 15600 / 15800; 10700 | 4890 identical | All identical |
| `e2-fixed.bin`, `e2-mgba.bin` | 19000 / 19200; 18100 | 890 identical, through former crash | All identical |
| `normal-full.bin`, `normal-mgba.bin` | 21100 / 21300; 19000 | 2090 identical, unsuccessful passage probe | All identical |
| `next-full.bin`, `next-mgba.bin` | 21900 / 22100; 19000 | 2890 identical, successful room-5 crossing | All identical |
| `room5-full.bin`, `room5-mgba.bin` | 25200 / 25400; 21900 | 3290 identical, through room-6 entry | Two score-digit timing frames |
| `throw7.bin`, tail of `throw-mgba.bin` | 27300 / 27500; native 26700, mGBA 25200 | Last 590 native steps identical (skip first 1500 reference steps), absolute 26700-27289 | Not compared yet |

- **Last matching absolute gameplay step:** 27,289 in the rock-throw window, using existing dumps only during pause. Native-only exploration extends to step 33,389. These are separate windows, **not a continuous full-playthrough verification**. Gaps between capture windows have not been claimed as matching. Both diff tools zip streams and stop at the shorter side; always report counts and allow extra mGBA frames.
- The known vortex remains separate: native frame 4994 / mGBA 5046, top lines 0-68. It was documented before this continuation.
- New score-digit picture timing: `a3` local steps 526, 532, 552 (native frames 9036, 9042, 9062); `room5` local steps 519 and 1919 (native 22429, 23829), affecting 11 and 41 pixels there. Every differing mGBA pixel matches the previous native frame's pixel. `UploadStageScoreDigitTiles` performs immediate DMA during a game step. This supports a mid-render upload timing explanation; these differences were not hidden with new masks. Diagnostic scripts: `hud_check.py`, `hud_room5_check.py`.
- **Collision conclusion:** the later room-4 block/ceiling behavior matches mGBA, not a second pointer defect. The apparent map mismatch compared two different route states: a dash had already destroyed the blue block in one. Standing jump/dash, low ceiling, carrying and input-facing rules account for the failed probes. The genuine pointer defect was the earlier water-surface crash and is fixed.
- Normal and ASan coroutine, BIOS and I/O tests pass. Both run 21,100 frames of `route-normal.keys` without faults. `normal-test.wav` and `asan-test.wav` are byte-identical (SHA256 `c2c66e77c6b2c9a1d818daf19e15fb963d752c9b379da96bb63f49e1ce70a2de`). Their 32 KiB saves match each other and final mGBA SRAM (SHA256 `2476ee2e3f5e7fcb2f1d20ead2cdb18a6868d27cf791029da1e5ac0ae9bf50ee`). A saved-boot rerun and sound-state lag replay were not added this session.

### Preserved artifacts and resume commands (PowerShell)

Everything is under ignored `build/m3-hall-exit/`; nothing was deleted. `ARTIFACTS.txt` inventories files. Keep the original scripts, all PNGs, `.bin` memory dumps, `.shots`, WAVs, saves, before-fix crash dumps, `block-probe.log`, `playtest.log`, inspection/sweep helpers and frozen checkpoints. `c6-diffs/` and `bc-diffs/` came from incorrectly paired exploratory diagnostics; their apparent differences are **invalid**, superseded by the continuous aligned captures above. The scripts and assets in this directory are local artifacts, not Git content. Prior-session artifacts remain in `build/m3-handover-20261018/`.

After the user explicitly resumes, reproduce the preferred checkpoint into **new** output files:

```powershell
$rom = 'C:/Users/Benny/Decomp/warioland4-2/baserom_us.gba'
$mgba = 'C:/Users/Benny/Downloads/mGBA-0.10.5-win32/mGBA-0.10.5-win32/mGBA.exe'
& build_pc/wl4.exe $rom --frames 31000 --input build/m3-hall-exit/hall_room6_middle.keys --from-step 30400 --dump build/m3-hall-exit/resume-middle-port.bin --shots build/m3-hall-exit/resume-middle-port.shots --mgba-colors
powershell -ExecutionPolicy Bypass -File port/tools/run_mgba_dump.ps1 -Mgba $mgba -Rom $rom -Frames 31300 -FromStep 30400 -InputScript build/m3-hall-exit/hall_room6_middle.keys -Out build/m3-hall-exit/resume-middle-mgba.bin -Shots -RunRoot C:/Users/Benny/Decomp/warioland4-port/build/m3-hall-exit
python port/tools/diff_frames.py build/m3-hall-exit/resume-middle-port.bin build/m3-hall-exit/resume-middle-mgba.bin --mask-sound
# Use the actual run directory printed by the mGBA command:
python port/tools/diff_screens.py build/m3-hall-exit/resume-middle-port.bin build/m3-hall-exit/resume-middle-port.shots build/m3-hall-exit/resume-middle-mgba.bin PRINTED_RUN_DIRECTORY --list
```

Expected native checkpoint coverage is 590 steps, absolute 30400-30989. After comparing it, copy the checkpoint to a **new** input script and append movement after step 31000. Replaying it from SRAM alone will not restore a mid-level position: these scripts boot and replay the whole route. If testing `--save`, always use fresh copies; the native run writes the save. Existing build/toolchain commands below remain valid. The next fresh GBA verification, when authorized to resume, is `BUILD=build/v7-24` using the documented identity command.

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

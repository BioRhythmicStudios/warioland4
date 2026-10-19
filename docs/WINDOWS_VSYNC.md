# Optional game VSync (2026-10-19)

The user confirms Private Beta 1 works on their separate Windows laptop, with
tearing on Intel integrated graphics and a 60 Hz OLED. This focused follow-up
adds Settings > Display > VSync On/Off. Save and start a new game process to apply
it. No beta archive was changed, rebuilt or published by this milestone.

## Presentation and timing

Inspection found `SDL_CreateWindowAndRenderer`, a streaming XBGR8888 texture,
nearest-neighbour integer scaling, `SDL_RenderTexture` and `SDL_RenderPresent`.
The software GBA PPU supplies that texture; the application does not use window
surfaces or a custom OpenGL swap path. SDL selects the renderer backend; local
tests report `direct3d11`.

The main thread requests `SDL_SetRenderVSync(renderer, 1 or 0)`, checks its result,
queries `SDL_GetRenderVSync` and records requested/reported configured state. An unsupported
On request attempts Off and continues with clock pacing, without modal errors
or changing the saved preference. Failed queries, fallback and fullscreen calls
also produce diagnostics. Fullscreen enter/leave and display-change events
reapply the preference. Tab temporarily requests Off; release/focus loss restores
the saved choice. SDL can also emulate VSync with a timer; its successful Set/Get
calls are not proof of native synchronization or tear-free scanout. The Settings
editor's own existing VSync remains independent.
See [SDL's API contract](https://wiki.libsdl.org/SDL3/SDL_SetRenderVSync).

Simply adding a blocking present to the old one-simulation-frame-per-loop path
would risk locking gameplay to display refresh. `PortFramePacerDue` instead uses
the unchanged integer period `280896 * 1e9 / 16777216` nanoseconds (~59.7275 Hz).
Each loop runs up to four due simulation frames, supplies input/save handling
and queues generated audio for every one, then presents the newest framebuffer
once. If no simulation frame is due, it waits only for the unelapsed absolute
deadline and skips texture upload/render/present completely.
Brief present delays catch up without skipping simulation/audio frames. Long
stalls beyond four periods retain the prior reset policy rather than building
unbounded backlog. Both On/Off wait only for time remaining before the absolute
simulation deadline. If present already passed it, no extra wait is added; the
next iteration catches up. No fixed 60 Hz interval or extra full-frame sleep is
introduced. Drivers accepting presents immediately cannot cause empty-frame
presentation traffic or accelerate simulation.

At 60 Hz, ~59.7275 GBA frames cannot map one-to-one to refreshes: an occasional
repeated refresh about every 3.7 seconds is expected. At ~240 Hz the same GBA
frame appears for approximately four refreshes. Lower refresh rates may omit
intermediate displayed images while still simulating/queuing their audio. There
is no frame interpolation, change of emulated cycles, or change of sample rate.
The existing audio queue target, drift correction and stall handling are intact.

## Investigation of the initial ~240 presents/sec observation

There is one final-frame `SDL_RenderPresent` call site in the game frontend.
The Settings editor has its own call in a separate window; it is not part of the
running game's display loop. No second game present, window-surface update or
OpenGL swap occurs in the upload/draw path.

The initial implementation still reached the final-frame present when the pacer
returned zero due simulation frames. One 12.519-second moved-window run recorded
748 simulation frames but 3,000 present calls, including 2,252 submissions without
new simulation. This was unnecessary repeated submission of the same completed
framebuffer, not four simulation frames or four distinct passes per GBA frame.
The smallest correction is the early skip/wait described above; bounded
simulation/audio catch-up remains unchanged.

The exact SDL 3.4.18 source was inspected:

- [SDL renderer implementation](https://github.com/libsdl-org/SDL/blob/release-3.4.18/src/render/SDL_render.c):
  `SDL_RenderPresent` flushes and forwards one backend present, and can use
  `SDL_SimulateRenderVSync` when hidden/backend presentation is unavailable.
  SetVSync can likewise select timer simulation. GetRenderVSync reads SDL's
  stored interval property, not a physical refresh or scanout measurement.
- [Direct3D11 backend](https://github.com/libsdl-org/SDL/blob/release-3.4.18/src/render/direct3d11/SDL_render_d3d11.c):
  VSync 1 selects DXGI sync interval 1 and clears the nonblocking/allow-tearing
  present flags. A backend present calls `IDXGISwapChain1_Present1` once. This
  proves the request path, not which submitted buffers reached the panel.

An important test correction followed: native `IsWindowVisible` returned False
after the harness's `Start-Process -WindowStyle Hidden`. Explicit ShowWindow on
that owned window changed it to True. The initial ~240/sec and mixed-monitor
runs therefore are scheduler/hidden-window tests, not scanout evidence. SDL's
hidden-window simulated pacing is consistent with the initial rate. The earlier
mixed-GPU explanation was only a hypothesis and is not established. There is no
evidence here proving that the compositor displayed 240 distinct buffers/sec on
the 59.95 Hz panel, nor that native VSync was broken or tear-free.

Final display tests explicitly show each owned window and assert native
visibility before testing. SDL logs now say `reported=1`, avoiding an implication
of measured physical VSync. The user will validate visible tearing on Intel.

Developer-only `WL4_TIMING_STATS=1` prints exit statistics to stdout: elapsed
time, simulation frames, present calls, presents without a new simulated frame,
long-stall clock resets, longest present call, audio low-queue refills and
overflow clears. One startup audio refill is expected; Tab intentionally drops
game audio and can cause further silence refills. These counters do not prove
tear-free scanout or that every audible sample is glitch-free.

## Configuration compatibility

`WL4_SETTINGS 2` stores five display/stick integers: scale, fullscreen, analog,
dead zone, VSync (0/1), then the unchanged keyboard/gamepad arrays. Defaults are
VSync On. Both launcher and direct `wl4.exe` load this shared file from the
existing per-user application directory. ROM-path registry persistence, CLI
scale override, SRAM path and SRAM format are unchanged.

Valid version-1 files keep every existing value and migrate VSync as Off to
preserve prior behaviour. Loading never rewrites them; explicit Settings Save
or an existing launcher save-on-launch upgrades the file. Invalid/unknown files
retain the previous whole-file recovery policy with defaults; no automatic
repair overwrites the original file. VSync values outside 0/1 are rejected.
Older executable builds do not understand v2; retain matching launcher/game EXEs.

## Verification

Evidence: `build/vsync-20261019/`; corrected build/regressions in `final/`,
explicitly visible display tests in `visible/`. Earlier evidence is preserved.
All tests used explicit disposable config
overrides. All game windows used explicit disposable `--save`; headless tests
used their isolated test directories. No launcher Browse/Launch button was used.

- Normal and ASan builds pass without new compiler warnings.
- Both input-test variants pass v2 On/Off persistence, v1 preservation without
  rewriting, explicit upgrade, invalid-version/truncation/value recovery,
  defaults, existing key/controller/collision/virtual-hotplug regressions.
- Synthetic 100-second 30/60/120/144 Hz present schedules produce exactly the
  expected GBA-clock frame count. Early presents run no simulation; short stalls
  preserve due frames; long stalls reset bounded backlog; fast-forward release
  resumes without a burst. These are synthetic, not physical monitor tests.
- Invalid-renderer fixtures exercise Set/Get/fallback API failure handling
  without GUI or fatal exit. No unsupported physical driver was available.
- Normal/ASan launcher UI tests persist On and Off across closing/reopening.
  Display Defaults followed by Cancel leaves the saved file byte-identical.
- Final visible normal game tests: windowed, startup fullscreen and F11 both directions,
  each with On/Off. ASan windowed On/Off also passes. SDL reports the requested
  interval, reapplied across mode transitions. Simulation stays near 59.73 Hz,
  with zero clock resets, one startup audio refill, zero subsequent refills and
  zero audio overflow clears. Presents no longer occur without new simulation.
- Main test display: Odyssey G60SD at 239.96 Hz. Corrected application presentation
  is near the GBA rate, not monitor refresh; no redundant four-presents-per-frame
  path remains.
  Local GPUs include NVIDIA RTX 5080 and AMD integrated graphics, not Intel.
- Additional visible owned-window move/fullscreen tests on AW2725DF (59.95 Hz)
  check simulation and submission counts. These are still not proof of panel
  scanout or Intel behaviour. No monitor settings or unrelated apps changed.
- Tab test logs On -> Off -> On and exceeds normal simulation speed as intended.
  Its deliberate audio suppression/refill counters are excluded from the normal
  playback stability claim.
- Normal/ASan coroutine, BIOS/LZ77, trapped I/O and disposable-save recovery
  regressions pass. Personal SRAM and settings SHA-256 hashes remain unchanged.
  Owned GUI processes are closed; all prior evidence/files remain intact.
- Fresh GBA v7-42/v7-43 and final v7-44: ROM-SAME, OBJ-SAME, MD5
  `940b597bef3be3f4041021b47a2f4096`. No game/GBA/generated source or tools/agbcc
  changes. Private Beta 1 ZIP and its SHA-256 remain unchanged.

Representative final visible results (both On and Off were tested):

| Case | Seconds | GBA frames | Present calls | Empty-frame presents |
| --- | ---: | ---: | ---: | ---: |
| Windowed, VSync On | 9.995 | 597 | 597 | 0 |
| Startup fullscreen, On | 7.986 | 477 | 477 | 0 |
| F11 fullscreen and back, On | 8.003 | 478 | 478 | 0 |
| ASan windowed, On | 8.037 | 480 | 480 | 0 |
| Visible 59.95 Hz window, On | 12.591 | 752 | 752 | 0 |
| Visible 59.95 Hz fullscreen, On | 12.540 | 749 | 749 | 0 |

All normal-speed visible runs stayed within 59.724-59.730 simulated frames/sec.
The longest measured present in the primary visible suite was 18.889 ms (ASan).
Zero clock resets, one startup audio refill and zero overflow clears were
recorded in each normal-speed run. No half-rate simulation, catch-up burst or
extra same-frame submission occurred in those samples. Visible Tab fast-forward
intentionally exceeded normal speed and restored reported VSync 1 on release.
It generated expected silence refills during audio suppression, not included in
the normal-speed queue-stability result. No claim about audible quality or
physical scanout is inferred solely from these counters.

## Intel laptop acceptance

Use the updated matching normal EXEs when a new test copy is explicitly prepared;
the existing beta ZIP still contains the pre-VSync build. In Display, enable
VSync and Save, then launch a new game. Existing v1 profiles start Off deliberately.

On the laptop's normal 60 Hz setting, compare Off/On during steady horizontal
scrolling in the same area. Check windowed, startup fullscreen, and F11 in both
directions. Listen for clicks, gaps, pitch changes and recurring hitching; verify
gameplay has not dropped to half speed. A rare repeated refresh from 59.73/60
mismatch is expected. Check Tab fast-forward/release and reopen the game to
confirm persistence. Report Intel adapter/driver, Windows version, fullscreen
state, and whether VRR is enabled if tearing remains. No Intel/OLED visual or
listening validation is claimed from the desktop tests.

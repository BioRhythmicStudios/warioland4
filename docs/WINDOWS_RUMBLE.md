# Native rumble prototype (2026-10-21)

Local branch: `feature/native-rumble`, based on release commit `d777f81`.
This milestone is committed locally; no push, tag, package or publication is made.
Earlier development entries below describe their status at the time.
`port` and annotated `v0.2.0-beta` remain unchanged at the release commit.

## Verified local milestone (2026-10-21)

The user authorized committing the complete rumble work locally, with no push or
public release. This commit is on `feature/native-rumble`, parent `d777f81`.
Normal/ASan builds and native-event, virtual-controller/settings/pattern and
coroutine/BIOS/I/O/disposable-save regressions all pass. Fresh GBA v7-66 (also
v7-65) is byte-identical: ROM-SAME and all 202 objects OBJ-SAME, MD5
`940b597bef3be3f4041021b47a2f4096`; next fresh directory v7-67.

Before/after SHA256 checks cover 85 protected files: current personal save and
settings, original ROM, prior rumble test saves/configs, retained Beta 2 packages,
release binaries and unrelated local handover/provenance notes. All are unchanged.
No historical personal snapshot was restored. New tests used explicit new config
paths and disposable save-test directories under `build/rumble-milestone-20261021/`.

Published Beta 2 was checked read-only: its tag and target commit remain
`d777f81e07e701507084075e00d966ef66accdcf`; all three public downloads hash-match
the retained release. ZIP SHA256 remains
`852cd7cde038148ef057e6db1ac48664608e6049289397e4910ea94e911abfd2`.
GitHub currently reports `prerelease=false` (updated 2026-10-20), unlike the older
handover's prerelease label. No release metadata, public asset or remote ref was
written. Local `port`, Beta 1 and Beta 2 tags are unchanged; no history rewriting.

Evidence: `build/rumble-milestone-20261021/`, including `test-results.json`, test
logs, `protected-before.json`, `protected-after.json`, `protection-result.json`,
`public-release.json` and `public-download-check.json`. These private audit files
are not commit/release payloads. The effects remain subject to the per-form manual
coverage limits below; automated fixtures do not establish a complete playthrough.

## Transformation extension (2026-10-21)

The user reports missing Wildflower Fields bee/inflation feedback and requests
transformation activation/recovery, snowball rolling and Fire Wario effects.
This extension is PC-only: it changes native observation, scheduling and tests;
no additional shared game sources, GBA state, physics, save or timing logic.
It remains uncommitted on `feature/native-rumble`; no push/tag/publication.

`SampleTransformation` reads the real `gWarioData.reaction` and `pose` enums after
each simulated frame. It compares active samples in the same room, suppresses
load/room/pause baselines, and ignores ordinary normal/swimming transitions.
It observes every named transformation reaction in include/wario.h and Bubble
Wario's separate water pose. Entering a new form queues activation once; returning
to normal/swimming queues recovery once. Direct form-to-form changes produce
only the new activation. Existing rumble settings and v4 configuration are unchanged.

| Form | Covered events | Manual gameplay status |
| --- | --- | --- |
| Puffy / inflated | Bee-induced activation; ceiling deflation starts a light recovery pulse; final return to normal does not repeat it | Pending Wildflower Fields bee and ceiling test |
| Bat | Activation; completed light-source recovery or water recovery | Pending both recovery routes |
| Zombie | Activation; completed light-source recovery or water recovery | Pending both recovery routes |
| Snowman | Activation/recovery; one sustained effect entering the small/medium/large rolling family; growth and airborne variants do not restart it; bonk impact | Pending downhill growth, jumps, wall and water |
| Fire | Activation and progression from blue to green/red/engulfed each give a finite pulse train; ground/air alternation does not repeat it; distinct completed ash recovery; ordinary light recovery on water extinguishing | Pending colour stages, ashes and water |
| Fat | Activation/recovery including water; landing impact | Pending food, landing and recovery |
| Frozen | Activation/recovery; bonk impact | Pending ice activation and wall break-out |
| Bouncy | Activation/recovery including water; ceiling impact | Pending spring, ceiling and final bounce |
| Flat | Activation and final recovery, including water | Pending flattening, pump/recovery and water |
| Mask | Activation and reaction exit | Pending mask course and exit |
| Bubble (water pose) | Entering/leaving bubble pose | Pending underwater bubble test |

Original subtle envelopes, before user strength scaling:

- Activation: 18000/14000 for 65 ms, 55 ms gap, 24000/10000 for 80 ms.
- Recovery/deflation: one 9000/14000 pulse for 65 ms.
- Transformation physical impact: one 20000/12000 pulse for 85 ms.
- Fire: three 60 ms pulses at 14000/18000, 16000/20000 and 18000/22000,
  separated by 60 ms gaps (300 ms overall).
- Ash recovery: 12000/24000 for 110 ms, 70 ms gap, 20000/6000 for 60 ms.
- Snowball roll: 18000/8000 for up to 3000 ms, cancelled immediately on the
  first sampled exit from rolling. This is one bounded effect per rolling episode,
  not endless vibration for a long hill. The duration needs the user's feel test.

`rumble_pattern.h` schedules segments against SDL host ticks without sleeping,
changing gameplay clocks or submitting the same segment each frame. SDL pulse
expiry handles each submitted segment; a late update skips expired segments and
clips remaining duration to the original envelope. Disabling, zero strength,
inactive gameplay/focus, shutdown and disconnect cancel outstanding segments.
The sequence records the selected SDL joystick ID so a reconnect cannot play an
old sequence tail. No sequence resumes after interruption. The existing five-second
Frog Switch effect retains priority; transformation cues during it are consumed.

Transformation activation/recovery replaces simultaneous damage, shake and
contact output, bypassing the ordinary impact cooldown. Health samples are still
consumed, so the damage cue is not deferred to another frame. A genuine transformed
bonk/landing can interrupt activation with the quieter physical-impact cue.
Incidental generic shakes do not interrupt scheduled envelopes.

Normal/ASan builds and native transition fixtures pass for all reaction IDs,
Puffy deflation deduplication, Bat/Zombie light and water exits, snow growth/air
variants, Fire colour/air variants and ash recovery, room/pause baselines and
selected transformed impacts. Normal/ASan virtual-controller tests pass for
simultaneous damage suppression, sequence segments/gaps, stall skipping, snow
cancellation, settings disable, disconnect/no replay and the previous features.
Normal/ASan coroutine/BIOS/I/O/disposable-save regressions pass. These fixtures
exercise sampled states and SDL output; they do not prove input-only gameplay
coverage. All new physical feel/level-specific checks in the table remain pending.

Evidence: `build/rumble-transformations-20261021/` (normal/ASan event, input and
runtime logs and disposable configurations). No physical controller pulses or
personal SRAM/settings writes were used for these automated checks. An initially
locked normal executable was left alone; it subsequently rebuilt successfully.
GBA v7-63 and final v7-64: ROM-SAME, all 202 objects OBJ-SAME, MD5
`940b597bef3be3f4041021b47a2f4096`; next unused directory v7-65.

## Gameplay feedback follow-up (2026-10-21)

User reports the initial prototype works and requested enemy attack contacts,
headbutting rocks/switch blocks, and continuous Frog Switch feedback. These are
implemented locally on the same branch; no commit/push/release. Reopen the built
launcher/game for this revision, using the same isolated configuration as before.

- Sprite collision processing now brackets the real collision handler. Native
  observation requires a normal shoulder/dash attack facing the contact, or a
  ground pound from above, and an accepted tackled/crushed response that changes
  the target's pose. Unchanged overlaps and attacks facing away do not queue a cue.
- Rising head contact with rock tile attributes queues a headbutt cue even when
  the rock resists breaking. Successful debris generation also catches upward
  breaks. Upward contact with a carryable rock sprite requires a resolved velocity
  or pose change. Ordinary ceiling tiles are not treated as rock attributes.
- Switch tiles queue a cue only when the switch actually changes state, including
  headbutts from below; repeated overlap while the switch is transitioning does
  not requeue it.
- Frog Switch activation queues one continuous 5000 ms SDL effect at 28000/12000
  before strength scaling. It bypasses the short-impact cooldown. Incidental
  impacts during those five host seconds are consumed without replacing it.
  It stops early on inactive gameplay/focus loss, disable/zero strength, disconnect
  or shutdown. It does not resume after interruption or replay on reconnect.
- New contact/switch cues are 100 ms at 44000/26000; headbutts use 30000/26000.
  All retain the existing strength control and Off default.

Normal/ASan native event fixtures and virtual controller tests pass, including
accepted shoulder/ground-pound contacts, rejected unchanged/backward contacts,
headbutts, event consumption, disabled/zero strength, and actual SDL five-second
expiry without blocking the caller. Runtime/BIOS/I/O/disposable-save tests pass.
No new physical pulses or desktop mouse automation were used for this follow-up.
New effects still need the user's gameplay feel check, including different enemy
families and both breakable and resistant rock tiles. Event fixtures verify the
native bridge; they are not a claim of input-only gameplay coverage.
Evidence: `build/rumble-followup-20261021/`. Fresh v7-59/v7-60 and final v7-62
all preserve the ROM and 202 objects; next fresh directory v7-63.

## Architecture and effects

`PortControls` retains ownership of the first available SDL3 gamepad. Native
rumble uses `SDL_RumbleGamepad`, capability properties and its checked return
value. Missing/unsupported devices do not affect keyboard or controller input.
Closing/removing a controller sends zero intensity before closing its handle;
newly opened controllers never inherit a pending pulse. Shutdown, loss of active
gameplay/focus and fast forward stop rumble. Settings disable/zero strength stop
an active preview immediately. SDL events continue pumping for pulse expiry.

`rumble_snapshot.c` compiles with the game's layout flags, reads gameplay state
and consumes native event flags. The frontend samples it after each simulation
frame. No game clock, coroutine, audio pacing or GBA memory layout changes.
The small shared-source hooks are entirely inside `PORTABLE` guards.
Generated symbols were regenerated and have no Git content changes.

Original effects, specified as low/high motor magnitudes before percentage
scaling (0-65535) and host milliseconds:

| Event | Signal | Low / high | Duration |
| --- | --- | --- | --- |
| Wario hit | Health decreases between active samples | 26000 / 48000 | 110 ms |
| Ground pound / heavy impact / boss-induced shake | New or increased screen-shake duration | 44000 / 16000 | 160 ms |
| Shoulder bash / dash breaks rock tiles | Successful tile replacement spawns debris while in normal attacking pose | 38000 / 24000 | 85 ms |
| Boss damaged, including shop-item damage ticks | Shared boss-health HUD update observes a decrease for the same boss/room | 42000 / 34000 | 140 ms |
| Enemy shoulder/dash or ground-pound contact | Accepted tackled/crushed sprite response | 44000 / 26000 | 100 ms |
| Headbutting rock | Rising rock-tile contact or resolved rock-sprite contact | 30000 / 26000 | 100 ms |
| Regular switch block | Actual switch-state change | 44000 / 26000 | 100 ms |
| Frog Switch | Activation once | 28000 / 12000 | 5000 ms |
| Settings Test Rumble | Explicit button, only when enabled and nonzero strength | 36000 / 18000 | 180 ms |

Only one effect plays at a time. Frog Switch holds priority for its duration.
For short pulses, damage takes priority, then boss damage, then headbutts, then
rock breaking, then general impact. A host-time 180 ms cooldown suppresses
clusters. State edges and event flags are consumed even when output is disabled
or suppressed, so there is no deferred replay. A continuous shake countdown or
unchanged damage/animation state does not trigger every frame. Shop items that
remove health in separate ticks produce distinct pulses at those ticks, rather
than continuous vibration throughout flashing. The shared health hook also
covers ordinary boss damage. Boss identity includes passage, room and sprite ID;
initial health is a baseline, not damage.

These effects were designed here; no ROM-hack code or detailed patterns were
consulted. This is a minimal prototype: generic shakes can include non-combat
impacts, an impact that does not increase an already longer shake can be missed,
and a second event inside the cooldown is deliberately dropped. Shoulder cues now also cover accepted enemy attack responses; headbutts
include upward contact with carryable rock sprites. Boss-specific attack windups are not separately hooked.
Gameplay acceptance and feel tuning remain pending.

## Settings and safe gameplay testing

Settings -> Controller adds Rumble On/Off, strength 0-100% in 10% steps and
Test Rumble. Defaults are Off, 50% strength. Test obeys Off and zero strength.
Controller defaults restore these values. Changes apply on the next game start.

Version 4 persists the two new values atomically. Versions 1-3 load without
rewriting and default rumble to Off, preserving bindings, VSync and CRT values.
The published v0.2.0-beta cannot read version 4: use an isolated prototype
configuration to keep your release configuration intact. SRAM format is unchanged.

For a separate PowerShell testing session (close the launcher/game afterward):

```powershell
$env:WL4_SETTINGS_FILE = 'C:/Users/Benny/Decomp/warioland4-port/build/rumble-20261021/gameplay.cfg'
& ./build_pc/wl4-launcher.exe
```

Open Settings -> Controller, enable rumble, adjust strength, press Test Rumble
and Save, then launch. The override is inherited by the game. This uses a separate
configuration, but an ordinary launcher game still uses the player's normal save
and remembers the selected ROM. Normal gameplay saving is expected. For fully
isolated save testing, launch `build_pc/wl4.exe` directly with the ROM path and an
explicit disposable `--save` path. No personal configuration or SRAM was written
by the automated checks below. Never restore old personal data over new progress.

## Verification and evidence

- Normal and ASan builds pass, including regenerated-symbol rebuilds.
- Normal/ASan settings/input tests pass with disposable configs: enabled/disabled,
  strength persistence, v1/v2/v3 migration without rewriting, invalid values,
  virtual supported/unsupported pads, reconnect and keyboard fallback, pulse
  magnitudes, persistent-state deduplication, shutdown/disable zero intensity.
- Normal/ASan coroutine, BIOS, I/O and disposable-save checks pass.
- Isolated Settings event harness passed Controller tab, On, strength increase,
  Test Rumble, Off and Save (`ui-sdl.cfg`: Off/60%). No launcher registry access.
- A direct SDL pulse on the physical Xbox 360 Controller returned success,
  explicit stop returned success, and the user confirmed feeling it. The first
  Python diagnostic printed a false capability result because it used the wrong
  property string; production uses SDL's correct header constant and supported
  virtual tests pass. This diagnostic output is not a hardware limitation.
- Initial Windows posted-click UI harness attempts timed out; their own preview
  processes closed. SDL-event UI testing then passed. Desktop screenshot evidence
  is private and must never be packaged. No claim of successful posted-click tests.
- Physical unsupported-controller testing and physical disconnect/reconnect,
  actual gameplay effect timing and all shop items/bosses remain for manual testing.
- Fresh GBA v7-55, v7-56 and v7-57 pass ROM-SAME and OBJ-SAME, MD5
  `940b597bef3be3f4041021b47a2f4096` (202 objects). See the current plan for the
  final documentation verification and next unused directory.

Evidence: `build/rumble-20261021/`, including `normal-final.log`, `asan-final.log`,
`normal-runtime-final.log`, `asan-runtime-final.log`, disposable configurations and
preview harnesses. Existing local handover/provenance notes, symlinks, saves,
ROMs, release files and cleaned Git history were retained. No blanket cleanup.

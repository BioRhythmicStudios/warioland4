# M3 manual coverage and private Windows alpha review

Review recorded 2026-10-18. Reviewed HEAD: `f3d22a5`; runtime fix: `c25cc4d`;
fixture documentation: `38c49ef`. Later README/screenshot commits and the merge
are preserved. This review changes documentation only. No alpha archive or release
was created, no ROM data was distributed, and no runtime implementation was started.

## Evidence classes

| Evidence | What it establishes | What it does not establish |
| --- | --- | --- |
| User-reported manual gameplay | The particular actions below worked in the Windows port during the user's playtest | Every room, difficulty, transformation variant, controller, or PC; ASan coverage or exact mGBA equivalence |
| Earlier automated checks | Normal/ASan runtime tests, preserved Hall route replays, and the specific mGBA comparison windows recorded in the handover | Continuous full-game equivalence; unpaired capture tails or gaps between windows |
| Earlier positioned fixtures | Switch, countdown, movement and vortex exit handling work separately in normal/ASan | An uninterrupted input-only switch-to-exit traversal |
| This review | Source inspection, PE import inspection, and three ROM-rejection smoke tests | A clean-machine install, missing-DLL runtime test, physical controller test or new gameplay/sanitizer run |

## Manual results supplied by the user

Reported in this session; exact executable hash, Windows version, input device,
repeat count (except Wario Hop), and sanitizer configuration were not supplied.
Do not assign these observations to a particular executable hash or to ASan.

| System/path | Reported result | Remaining scope |
| --- | --- | --- |
| Hall of Hieroglyphs | Frog Switch no longer freezes; escape timer works; level completed in manual gameplay | A continuous automated input recording and full mGBA comparison remain absent |
| Spoiled Rotten | Boss defeated; transition back to the Golden Pyramid worked | Other boss phases/paths, failures, rematches and later bosses remain unverified |
| Wario Hop | Two rounds played through to Game Over | Broader score/reward/retry paths and other minigames remain unverified |
| Zombie Wario | Transformation and falling through the required platform worked | Reversion, damage/water/light interactions and other zombie rooms remain unverified |
| Bat Wario | Transformation and flight worked | Reversion, flight collision and all bat-specific obstacles remain unverified |
| Save persistence | Closed and reopened the game; progression restored correctly | Failure recovery, interrupted writes, multiple instances/slots and malformed saves remain unverified |

The Hall result closes the earlier **manual completion** follow-up. The historical
fixture limitation remains accurate for automated evidence, but must no longer be
read as saying that nobody has completed Hall in real gameplay. M3 is still incomplete.
No route recording, screenshot or new save was supplied with these observations.
The existing README independently records earlier manual progress; it is not an
additional automated test and is not counted twice.

## Major coverage gaps

- Progression: remaining Emerald/Ruby/Topaz/Sapphire stages and all route branches,
  boss gates, Keyzer and jewel/CD collection accounting, passage completion, later
  bosses, Golden Pyramid/final boss, ending/credits, alternate endings and unlocks.
  A successful return to the Golden Pyramid is not final-passage completion.
- Difficulty/save paths: Hard and Super Hard, fresh versus continued games, multiple
  file slots, copy/erase/reset, temporary saves, interrupted sessions and old saves.
- Failure/recovery: timer expiry, score drain and defeat, damage/death, Game Over,
  retry/quit/pause transitions, boss timeouts and incomplete collectible exits.
- Mechanics: flaming, fat, frozen, snowman, bouncy, puffy, flat and mask reactions;
  transformation reversions/combinations; rolling, charged attacks, carried-object
  collisions and throws, water currents and room-specific mechanisms. Zombie and
  bat observations cover only the actions explicitly reported above.
- Side systems: item shop and purchased boss items, remaining minigames, reward and
  medal accounting, sound room and persistent collectible/unlock state.
- Runtime diversity: later raster/affine/palette effects, long-session stability,
  memory reservations on other PCs, alt-tab/focus, fullscreen/DPI/window resizing,
  different refresh rates/audio devices and reconnects. Known lag-frame audio drift,
  score-digit upload timing differences, open-bus approximation and unimplemented
  BIOS 0x2A remain documented. Unreached weak-stub paths need a reachability audit.

## Private-alpha assessment

**Conditional go for guided testing with a few technical testers; hold an unattended
private alpha package until startup, save-failure visibility and clean-machine checks
are addressed.** This is an exploratory development alpha, not a full-game quality
claim. No public release is authorized. These are readiness findings, not new promises.

| Area | Observed implementation/evidence | Gap before independent testers |
| --- | --- | --- |
| ROM loading | `rom.c` validates 8 MiB and exact MD5 `5fe47355a33e3fabec2a1607af88a404` (AWAE). All three negative smoke tests exit 1 with useful stderr | Default path is relative to working directory; no picker, recent-path setting or persistent startup log. ROM validation happens before window error dialogs are enabled. Verify Explorer launch, spaces/non-ASCII paths, missing/unreadable and unsupported ROMs |
| Missing DLLs | PE import inspection: normal exe imports SDL3 plus Windows/UCRT libraries; ASan additionally imports `libclang_rt.asan_dynamic-x86_64.dll`. SDL3 imports Windows system libraries | SDL3 is a load-time import: failure occurs before main, so the current app cannot explain it itself. No custom dependency preflight. Inventory exact binaries; validate on a PC without the toolchain/PATH. Do not ship developer ASan build as the default |
| Controllers | SDL positional face buttons, D-pad, shoulders, Start/Back, left stick threshold; first controller and add/remove handling implemented | No physical-controller results supplied; no remapping/device chooser. Test wired/Bluetooth where available, disconnect/reconnect, multiple devices, no-controller keyboard fallback and focus changes |
| Saving | 32 KiB SRAM, per-user preference directory or explicit path; temporary file + flush + rename; save on settle/quit. Manual reload passed | Failed writes print only to stderr; Write returns void, and after the 30-frame attempt there is no automatic retry until another SRAM change or close. No failure dialog/backups or multi-instance protection. Short saves accepted with erased remainder; oversized input is not fully size-validated. Fault-injection/recovery tests needed |
| Crash/hang reporting | Recognized memory faults include PC/nearest game function and show a dialog in window mode. Developer fault/ASan probes exist | No durable crash log, build identifier, automatic stack/minidump or hang capture. Handler is selective, not universal. Hangs do not raise it. Require steps, exact build, save copy and a documented dump collection procedure; preserve matching debug symbols privately |
| Packaging | CMake copies SDL3 beside the executable; generated LZ77 test source contains addresses/names, not compressed asset bytes | No install/CPack/staging manifest here. Need an explicit allowlist, dependency/license inventory, hashes, tested OS baseline, launcher/help and artifact audit. Do not zip the worktree or build directory |

Source references: `port/src/{rom,main,save,gba_memory,os_win32}.c`,
`port/platform/sdl/sdl_frontend.c`, `port/include/port/rom.h`, `CMakeLists.txt`.
Import inspection is not proof that every optional/dynamically loaded driver path
works, and is not a clean-machine result. Missing DLLs were not deliberately removed.
No private-alpha package has been assembled or ROM-byte audit newly completed.

## Five priorities, ordered by risk and user impact

1. **Make failed saves visible and recoverable.** Protect tester progress first:
   retain pending SRAM, report the destination/error visibly, and retry with a bounded
   delay. Verify old-file preservation and later recovery using disposable saves.
   Keep malformed-save policy and concurrent writers as explicit follow-ups.
2. **Make first launch predictable.** Give missing/unsupported ROM errors a durable
   and visible explanation; provide an executable-relative launcher or ROM chooser,
   accurate accepted-image details and dependency guidance. Preserve command-line
   use. Test Explorer/other working directories and paths with spaces/non-ASCII text.
3. **Make failures diagnosable.** Save startup/fault logs with build ID and useful
   game/frame context; retain matching symbols and document live-hang dump capture.
   Test one deliberate fault and one controlled hang without swallowing real errors.
4. **Produce a reproducible private test bundle and clean-PC acceptance record.**
   Allowlist normal exe, exact SDL3 DLL, applicable notices, launch/help and test form;
   record hashes and supported Windows baseline. Validate with no toolchain/PATH,
   keyboard plus a real controller/hotplug, and private save copies. Keep ASan/symbols
   separate. Gate sharing on auditing out ROMs, extracted assets, dumps, screenshots
   and personal saves, and reviewing the existing dependency/provenance notes.
5. **Broaden progression/failure coverage under a recorded test matrix.** First cover
   a complete additional passage/boss, timer-expiry/death/retry, an untested reaction
   and a shop/minigame reward path; add targeted normal/ASan recordings and mGBA
   comparisons where practical. Preserve all older passing scripts and evidence.

## Proposed next small implementation task (not started)

**A visible, rate-limited save-write failure with pending-data retry.** Limit the
change to the native save/runtime error path and a focused disposable-file test.
Reuse the existing atomic replacement; do not alter game SRAM layout or game code.

Acceptance criteria:

- A failed automatic write leaves the prior save byte-for-byte intact and the newest
  SRAM pending. A visible message names the destination and says progress is unsaved.
- Repeated failures do not produce per-frame dialogs or a busy loop. Once the
  destination becomes writable, a bounded retry persists pending data without a new
  gameplay SRAM change. A failed final close is also visibly reported.
- Successful ordinary saves keep existing behavior. Verify failure/recovery using an
  injected write failure or disposable locked destination, never the user's live save.
- Normal/ASan save-path checks and existing runtime tests pass; a fresh GBA build
  prints ROM-SAME and OBJ-SAME with MD5 `940b597bef3be3f4041021b47a2f4096`.

Implementation awaits the user's milestone approval per AGENTS.md. Documentation
and the review itself are authorized and completed; no implementation approval is
inferred from the user's manual results.

## This review's checks and preservation

- ROM-negative tests: nonexistent path, one-byte file, and an 8 MiB all-zero dummy;
  each rejected with exit 1 and expected stderr. The dummy is not game data.
- PE imports inspected using a local standard-library Python helper. Artifacts are
  isolated in ignored `build/m3-alpha-review-20261018/`; no older evidence removed.
- No new gameplay, controller, missing-DLL runtime, fault-injection or clean-machine
  test is claimed. Existing automated results retain their original coverage limits.
- Only this report, handover, plan and Frog Switch notes changed. GBA identity is
  verified after these documentation edits in fresh `build/v7-26` (see handover for
  the recorded result). No change to tools/agbcc, game sources or generated files.

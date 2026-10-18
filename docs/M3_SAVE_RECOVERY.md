# Native Windows save-failure recovery (2026-10-18)

Implementation: `771dcc2`, based on `7ba5638`. This is the user-approved first
private-alpha readiness task. Only PC runtime/UI/tests and CMake wiring changed;
no game source, SRAM layout, generated code or ROM data changed. No package/release.

## Failure inventory and resulting behavior

| Path | Previous behavior | Implemented behavior |
| --- | --- | --- |
| Quiet-frame automatic write | stderr only; no retry until another SRAM change or close | Persistent nonmodal red banner/title, F8 destination/Windows-error details, pending SRAM retained, periodic retry |
| Shutdown write | void return ignored; exited despite unsaved progress | Close returns failure and retains the session; Windows offers Retry saving, Return to game (default), or explicit Quit without saving |
| Headless shutdown | Could return success after failed save | Prints final-save failure and exits 1; has no interactive recovery after process exit |
| Temporary create/write/short write/flush/close/rename failures | Generic reason; fixed `.tmp` could truncate a pre-existing temporary file | Preserve Windows error code/reason; exclusive uniquely named sibling; complete write + flush + close required before replacement |
| Unreadable existing input | Window dialog already available, partial read targeted live SRAM | Visible error and no writable session; read into separate buffer so rejected input cannot alter current SRAM |
| Short/oversized input | Short accepted with warning on stderr; excess bytes ignored | Require exactly 32768 bytes, report visibly and leave original file unchanged |
| Oversized explicit path/default-folder lookup or path overflow | stderr only or silent failure | Visible startup save error, no writable session |

Normal play shows no modal save dialogs automatically. The banner says progress is
still in memory and saving is being retried. The destination is in the window title;
F8 opens full details on request. The same banner render path works in fullscreen
(the automated UI tests below used windowed mode). A successful write clears the
banner/title and logs recovery. Only one stderr warning is emitted per failure
episode; explicit F8/quit/retry actions can show details again deliberately.

The original 30-stable-frame debounce remains. Failure starts a 300-game-frame
cooldown (about five seconds at normal speed). SRAM changes during cooldown update
the pending image without bypassing it. Once cooldown expires, a stable latest
image is retried, even if the game made no further SRAM changes. New attempts are
bounded in frequency/work, not limited to a fixed total retry count. Fast forward
also advances this frame-based interval faster. An explicit shutdown retry is immediate.

The last successfully persisted snapshot advances only on success. Temporary files
use CREATE_NEW and a process/sequence suffix (up to 16 name-collision attempts), so
an existing `.tmp` or another process's temporary save is not truncated. Only a
complete flushed/closed temporary file can replace the destination. Failure cleanup
only targets this call's own temporary file. No destructive restore, destination
truncation, game-format change or automatic repair of malformed saves was added.

## Verification

All test saves were new, explicit locations under ignored
`build/m3-save-recovery-20261018/`. **The real AppData save was not accessed.**

- Normal and ASan builds pass coroutine, BIOS (50 game LZ77 streams), I/O (including
  Frog Switch HBlank regression) and new `--test-save DIRECTORY` tests.
- Filesystem tests create their own new child directory. They verify successful
  settled writes; exact old-file preservation when rename is denied by a read lock;
  newest SRAM retention during repeated failures; failed-close status/session
  retention; exactly 299 frames with no early retry followed by successful retry;
  recovery without a new SRAM change; close/reopen; untouched legacy `.tmp`; missing
  directory recovery; close-only retry; rejected short/oversized/unreadable saves and
  oversized paths without modifying either pending SRAM or original files.
- Each final unit run recorded exactly three failure episodes and three recoveries,
  including repeated failed attempts within an episode, without repeated-warning spam.
- Actual window checks: normal build continued gameplay under the warning, opened F8
  details, returned to gameplay after failed quit and automatically recovered when
  the disposable destination lock was released. ASan verified explicit Retry while
  still locked, then successful Retry/exit after release. A further normal check
  verified explicit Quit without saving: exit 1, original destination unchanged.
  Advancing gMainTimer samples established the warning did not stop the game.
- Normal and ASan each played `hall_room11.keys` for 8500 frames with new save paths,
  then loaded a copy in a fresh process. Both produced the same 32768-byte save,
  SHA256 `2476ee2e3f5e7fcb2f1d20ead2cdb18a6868d27cf791029da1e5ac0ae9bf50ee`
  (also the prior Hall reference). Fresh boot recognized file A as a temporary-save
  slot (status 3). Persistent bytes matched except for the game's expected backup
  header refresh at 0x40-0x7F and SRAM self-test at 0x980-0x98F. An initial naive
  whole-SRAM equality assertion exposed these 46 expected boot writes; no game code
  or existing comparison masks were changed. The save unit reload checks compare all
  32768 bytes directly, before running game boot code.
- Both headless builds with a locked disposable save report one failure episode,
  preserve all original bytes and exit 1 on final failure instead of claiming success.
- Fresh GBA `build/v7-27`, `build/v7-28` and final `build/v7-29` print ROM-SAME and OBJ-SAME,
  MD5 `940b597bef3be3f4041021b47a2f4096`. Expected unmatched-function base-ROM check
  failure remains separate. Next fresh verification directory: v7-30.

## Reproduction and artifacts

```
build_pc/wl4.exe YOUR_ROM --selftest --test-bios --test-io --test-save build/m3-save-recovery-20261018
build_pc_asan/wl4.exe YOUR_ROM --selftest --test-bios --test-io --test-save build/m3-save-recovery-20261018
```

The ASan runtime DLL directory must be on PATH. The parent test directory must exist;
the test creates a new unique child and never uses the default save path. Test files
are retained as evidence. New developer test source: `port/src/save_test.c`.

Preserved artifacts: final normal/ASan unit logs and summary, `restart2-report.txt`,
normal2/asan2 game/restart/final-failure logs and disposable saves, normal/ASan/discard
UI reports, screenshots and scripts, plus all earlier exploratory check evidence.
Early GDI screenshots could not capture the composited render surface correctly;
`discard-warning.png` uses screen capture and visibly confirms the full banner.
No test/game processes remain running after verification.

## Limits and next step

Pending progress remains in this process's RAM; forced termination/power loss cannot
preserve unwritten progress. Explicit Quit without saving discards it knowingly.
Disk-full/physical hardware faults, abrupt power loss and every filesystem type were
not fault-injected; the write/flush/close failure branches were reviewed, while real
create/read/rename failures and recovery were exercised. Independent instances or
external editors writing the same destination are not coordinated; unique temporary
names prevent temporary-file collisions, not stale-session last-writer conflicts.
No rotating backup policy or cloud synchronization was added.

Private alpha still needs the other readiness tasks. The next proposed small task
is predictable first-launch ROM/dependency error handling, subject to the user's
approval. General M3 automated equivalence remains incomplete; earlier manual
coverage and positioned-fixture limitations are unchanged.

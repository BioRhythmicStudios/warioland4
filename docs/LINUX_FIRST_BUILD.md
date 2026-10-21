# First native Linux x86-64 build

Date: 2026-10-21. Status: playable locally under WSL2/WSLg; unpublished.
Branch `linux-port`, based on `970cdd91580a4cbb7d0a80517df251b2aa0af5b0`
(the verified Windows `v0.3.0-beta` commit). Implementation commit: `7906092`, committed locally on `linux-port`; no publication is authorized.

## Workspace and environment

The isolated checkout is `/home/benny/Decomp/warioland4-linux`. Build products
and test saves stay under its ignored `build/` directory. The registration is
locked so it cannot be pruned while WSL is unavailable. Its relative `.git`
path works from both WSL and Windows Git. Other worktrees and published refs
are retained; do not run worktree pruning from WSL against Windows-only paths.

Ubuntu 26.04.1 LTS, kernel `6.18.54.1-microsoft-standard-WSL2`, x86-64.
Clang 21.1.8, CMake 4.2.3, Ninja 1.13.2, pkg-config 2.5.1, SDL3 3.4.2.
GCC 15.2, Python, make and ARM binutils were already available. The approved
apt plan installed the requested development tools with `--no-install-recommends`:
104 new packages, 22 dependency/security patch updates, no removals. No
additional package was installed for sanitizer testing. Initial `sudo -n`
authentication failed; the authorized installation used WSL's root account
without changing sudo policy or passwords.

## Minimal platform changes

- `os_linux.c`: non-clobbering fixed mappings (`MAP_FIXED_NOREPLACE`), page
  protection, `memfd_create` shared host/game I/O views, SIGSEGV/SIGTRAP
  handlers using x86-64 `ucontext_t`, register restoration after single-step,
  and an alternate signal stack. Existing hardware callbacks and decoder
  implement I/O effects, mirrors and BIOS/open-bus behavior. Unknown faults
  are reported and passed to prior handlers, including ASan.
- The existing low game coroutine stack and GBA addresses remain unchanged.
  The Linux ET_EXEC image is linked at `0x40000000`, above all GBA mappings.
  The first low-image prototype occasionally collided with Linux's randomized
  brk heap; safe reservation rejected it. The final placement passed 24 fresh
  randomized startups. ASLR remains enabled for the system and shared libraries.
- `os_file_posix.c`: bounded reads and exclusive mode-0600 temporary siblings,
  complete writes, file fsync, close and atomic rename. Failed temporary files
  remain for diagnosis. This preserves the old destination on pre-rename
  failure; directory metadata is not fsynced, so power-loss durability of a
  just-renamed directory entry is not guaranteed. Concurrent saves remain
  uncoordinated, as in the Windows implementation.
- Settings parsing and migration remain shared. Only file access changes by
  platform. Save tests use POSIX permission failures instead of Windows sharing
  locks. The I/O benchmark uses `CLOCK_MONOTONIC` on Linux.
- CMake selects the OS and save-test backend and keeps the GUI launcher
  Windows-only. Clang retains the DMA-sensitive `-fms-volatile` semantics,
  game ABI/alignment flags and absolute GBA symbols. GCC game compilation is
  deliberately rejected until an equivalent ordering implementation is tested.
- Combined tests exposed a preexisting leaked coroutine allocation when
  `PortGameThreadCreate` replaced its owner. It now destroys the previous
  coroutine before replacement; normal and ASan Windows/Linux tests pass.
- Shared gameplay sources in `src/`, GBA headers and generated symbols are
  unchanged. SDL rendering, audio, timing, keyboard/controller mapping, VSync,
  CRT and rumble code are reused. No Linux graphical launcher was added.

## Build and run

See `port/README.md` for the normal build and isolated interactive command.
The tested binary is `build/linux/wl4`: Linux ELF64 x86-64, interpreter
`/lib64/ld-linux-x86-64.so.2`, dynamically linked to native SDL3 and libc.
It is not a distributable compatibility package for older Linux distributions.
It requires a separately supplied 8 MiB US ROM with MD5
`5fe47355a33e3fabec2a1607af88a404`. The file is read, never patched or copied
into source control. GBA memory becomes read-only for the ROM after validation.

Headless validation (use a fresh test directory):

```sh
mkdir -p build/my-tests
WL4_SETTINGS_FILE="$PWD/build/my-tests/settings.cfg" ./build/linux/wl4 "$ROM" \
    --selftest --test-bios --test-io --test-save "$PWD/build/my-tests"
WL4_SETTINGS_FILE="$PWD/build/my-tests/input.cfg" \
    ./build/linux/wl4-input-test --isolated
./build/linux/wl4-rumble-event-test
```

`--frames` runs headless; with no `--frames` it opens SDL gameplay. All automated
runs used explicit disposable `--save` and `WL4_SETTINGS_FILE` paths. No tests
used the normal preference-directory save or changed personal settings.

## Sanitizers

The package installation omitted Clang's optional compiler-rt package, so
`build/linux-asan` records an unsuccessful link due to missing runtime archives.
The successful tested configuration uses Clang instrumentation with the already
installed GCC 15 libasan runtime:

```sh
cmake -S . -B build/linux-asan-gcc-runtime -G Ninja \
    -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWL4_ASAN=ON \
    -DCMAKE_EXE_LINKER_FLAGS=-fno-sanitize-link-runtime \
    -DCMAKE_C_STANDARD_LIBRARIES=-lasan
cmake --build build/linux-asan-gcc-runtime -j4
LD_PRELOAD="$(gcc -print-file-name=libasan.so)" \
    WL4_SETTINGS_FILE="$PWD/build/my-tests/asan-settings.cfg" \
    ./build/linux-asan-gcc-runtime/wl4 "$ROM" \
    --selftest --test-bios --test-io --test-save "$PWD/build/my-tests"
```

Apply LD_PRELOAD to the game/test executable, not to unrelated commands.
This combination was tested here; it is not a blanket guarantee for other
Clang/libasan versions. Installing matching compiler-rt later permits the
ordinary `-DWL4_ASAN=ON` build without those fallback link flags.

Linux ASan's high fake stack would truncate address-taken game locals when
written to 32-bit DMA registers. Linux sanitizer builds therefore disable
use-after-return fake stacks; ordinary stack/heap bounds instrumentation and
leak detection remain active. The intentional `TestOpenBus` hardware probe is
uninstrumented so deliberately invalid addresses reach the signal handler.
Default ASan shadow-gap protection stays enabled. Disabling it was tested and
rejected because it changes open-bus reads. General game code stays instrumented;
untested intentional high-address accesses can still conflict with ASan's
reserved address space. The deliberate `--asan-probe` correctly reports a
stack-buffer-overflow on the low coroutine stack. This is bounded validation,
not proof of whole-game sanitizer compatibility.

## Results and evidence

Evidence lives under `build/evidence/` in the Linux checkout. Test helper
scripts from this session are retained under the Windows worktree's ignored
`build/` directory. No ROMs, captures or extracted artwork are tracked.

| Check | Result / evidence |
| --- | --- |
| Native ELF and dependencies | `file`, ELF headers and ldd identify native Linux x86-64 / SDL3 |
| Normal coroutine/BIOS/I/O/save/input/events | Pass, `normal-final/` and initial `tests.json` |
| 24 randomized starts | Pass, `startup-final/result.json` |
| Linux ASan runtime/input/events/Hall | Pass, `asan-final/results.json` |
| Deliberate ROM write / unmapped read / stack overflow | Expected SIGSEGV with specific reports, `faults-final/results.json` |
| Deliberate ASan stack overflow | Expected stack-buffer-overflow report in `faults-final/asan-probe.log` |
| 8500-frame headless Hall | `linux-hall.*`; final SRAM, late pictures and audio byte-identical to retained Windows Beta 3 capture |
| Late game-memory comparison | All 40 captured steps match using existing masks, `windows-memory-diff.log`; no new masks |
| Normal versus ASan Hall | SRAM/pictures/audio byte-identical, `asan-final/results.json` |
| WSLg X11/OpenGL keyboard/fullscreen | A press/release reaches KEYINPUT; F11 toggles and clean exit, `wslg-result.json` |
| Visible WSLg Hall gameplay | `wslg-gameplay/result.json`: mode 2, room 11, 8507 finished main-loop steps |
| Timing | 8528 frames / 142.782 s, approximately 59.73 Hz; zero clock resets, zero audio drops, two total refills |
| Actual audio output | `wslg-gameplay/monitor.log`: 5-second PulseAudio sink-monitor recording, nonzero amplitude (peak 12538) |
| CRT | Software and OpenGL checks pass, `crt-software.log` / `crt-opengl.log` |
| Windows compatibility | Separate normal/ASan builds including launcher, runtime/input/event tests pass; Windows `build/linux-windows*-tests/results.json` |
| GBA identity | Fresh Windows-worktree `build/v7-72` through final `v7-75`: ROM-SAME and 202 objects OBJ-SAME against `build/us` |

The GBA build MD5 is `940b597bef3be3f4041021b47a2f4096`, distinct from the
supported input ROM MD5. GBA input files are unchanged and verified equal after CRLF/LF normalization
between the worktrees; the existing Windows-worktree build setup is used to
avoid modifying protected agbcc or extracted-data symlinks. Final checkpoint: Linux-worktree `build/v7-76` also matches the baseline ROM
and all 202 objects; next fresh directory is `build/v7-77`. The legacy `make check`
recipe checks the hardcoded `build/us` path against its historical checksum and
reported failure; explicit MD5 and byte/object comparisons above passed. The
Makefile and checksum file were left unchanged.

Early WSLg runs showed repeated audio queue stalls. Their logs are retained;
the cause was not established. A later isolated audio run had one initial
refill and no drops, and the complete 142-second visible Hall run had two
refills total and no drops. PulseAudio monitor capture confirms routed sound;
speaker listening quality and long-session stability still need user testing.

## Final local checkpoint

`build/evidence/checkpoint/results.json` records fresh normal and ASan
runtime, BIOS, I/O, save, virtual-controller and rumble-event passes. Linux save
tests now additionally force a partial write using a temporary file-size limit,
force rename failure against a nonempty directory, and round-trip a UTF-8 path.
Both failure paths preserve the previous destination; the test restores its
process limits and signal handler. Only newly created disposable files are used.
The supplied ROM hash remains unchanged.

To finish within the requested usage budget, the six-setting live CRT/VSync
matrix, longer route comparison and cross-platform save-reload replay were
deferred. Existing CRT tests, virtual reconnect/rumble tests and the bounded
8500-frame Windows comparison remain the evidence; no broader coverage is claimed.

## Scope and next step

This validates Ubuntu under WSL2/WSLg, specifically X11/OpenGL/PulseAudio.
It does not validate a physical Linux desktop, Steam Deck, direct Wayland,
physical controllers/rumble, every level, or all display/audio configurations.
Virtual-controller and rumble logic tests pass; actual hardware acceptance is
still needed. Reported SDL VSync state is not a physical scanout measurement.

The next useful step is a user playtest using the isolated launch command,
then real Linux desktop and controller/audio testing. The Windows Beta 3
branch, release tags, binaries, ROMs, personal saves/settings and protected
`tools/agbcc` were not replaced. No push, tag, publication or history rewrite
was performed. Linux changes remain available for review in `linux-port`.

## Private Steam Deck package candidate (2026-10-21)

port/tools/build_steamdeck_sdl.sh downloads and checksum-verifies upstream
SDL3 3.4.2 and Debian 13 libc6 2.41, then builds SDL3 with the older libm
only as a link-time baseline. The package bundles SDL3 and uses the host
glibc/ELF loader; it does not bundle glibc. The native game needs GLIBC 2.34,
and the rebuilt SDL3 needs GLIBC 2.38. The package builder is
port/tools/package_linux.py, with launcher and Deck instructions in
port/steamdeck/README.md.

The compatibility archive was extracted under /tmp outside the development
tree. Its 400+50 headless frames and 32 KiB save passed. WSLg OpenGL ran all
three CRT presets with VSync on/off at about 59.7 fps. An X11 synthetic key
reached GBA A after focus retries; WSLg focus makes that probe intermittent.
PulseAudio monitor capture contained nonzero game samples. Under monitor
capture both the bundled SDL and the original host SDL recorded 9 audio queue
drops in about 6.8 seconds; ordinary six-second runs recorded zero. A physical
Steam Deck has not yet been tested for graphics, keyboard, gamepad, or rumble.

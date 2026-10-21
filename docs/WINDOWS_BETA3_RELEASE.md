# Windows v0.3.0-beta - The Rumble Update

The user authorizes integration and publication after final checks, and manually
confirms the native rumble system works well in gameplay. Do not infer additional
hardware/driver/level-specific coverage. The existing Windows launcher, remapping,
CRT filters, VSync and original GBA simulation rate remain in scope. The colour
restoration mod is excluded while awaiting permission from its creator.

## Source and release procedure

Rumble implementation milestone: `20fc6e4`, directly based on cleaned Beta 2
`d777f81`. It uses SDL3 gamepad rumble and original finite host-time envelopes;
no third-party ROM-hack code or detailed patterns were consulted. PORTABLE-only
hooks and native state observation leave original GBA output unchanged.

Only rumble/release documentation is committed. Integrate into `port` by safe
fast-forward after fetching `myfork/port`. Do not rewrite history or overwrite
tags. Verify reachable history contains none of the removed artwork paths.
Preserve all other worktrees, uncommitted handover/provenance notes, ROMs and
personal data. Existing public releases are tagged v0.1.0-beta and v0.2.0-beta;
there is no v0.1.0-beta.1 tag on myfork at review time. Preserve the actual tags.

Fresh Windows x64 Release binaries must come from a clean detached sparse
checkout of this exact release commit, excluding shared agbcc/data symlinks.
Use llvm-mingw/UCRT and vendor SDL3 3.4.18, -O3 -DNDEBUG and ASan Off for the
package. Also build/test ASan. Record commit/tree, flags, binary hashes, import
closure and resource provenance. Include only both executables, SDL3.dll,
instructions, known issues, project licence and applicable third-party notices.
No ROM, extracted game assets, personal path/save/config, removed artwork,
colour restoration mod or developer-test executable belongs in the ZIP.

Extract outside the repository in the same Windows user context that will run
GUI tests. Use Windows-only PATH. Check packaged runtime, launcher-to-game
startup, settings migration/persistence, rumble disable and keyboard fallback,
CRT presets, VSync and fullscreen/F11. Automated game tests use explicit disposable
--save paths. The brief launcher startup test, which lacks a save override, must
hold the current personal SRAM read-only and verify its hash. Snapshot and restore
launcher registry values in the same process/user context; never use old snapshots.

## Verification and limits

Normal/ASan event, settings/virtual-controller, runtime/BIOS/I/O/disposable-save
and CRT readback tests are required. Test UI controls and host pacing separately.
A bounded 8,500-frame Hall route compares normal, ASan and retained Beta 2: final
SRAM and captured late pictures/audio match. This is not full-game validation.
Fresh GBA v7-68 (also v7-67) must print ROM-SAME and OBJ-SAME for all 202 objects,
MD5 `940b597bef3be3f4041021b47a2f4096`. The known make-check base-ROM checksum
failure is separate from baseline byte equality. Next fresh directory: v7-69.

Version-4 settings read v1-v3 without rewriting, default rumble to Off, and retain
bindings, CRT and VSync values. Explicit Save writes v4; older Betas cannot read
v4. SRAM remains the ordinary 32 KiB format. Controller support depends on SDL,
device and driver. Rumble settings apply after restarting the game. Frog Switch
rumble is five host seconds; snowball rumble is capped at three seconds per roll.
No full-game, new physical hardware, Linux or colour-mod compatibility is claimed.

Evidence and final package/publication reports belong under ignored
`build/release-v0.3.0-beta-20261021/`. A first CRT harness attempt had missing
screenshot directories; after creating them, normal/ASan readback checks pass.
Retain the failed setup logs; this is not a renderer defect or ignored assertion.
Protected-file hashes are taken from the user's latest files, never restored
from historical saves/settings. Publication is gated on package acceptance.

## Publication and Linux handoff

After all gates pass, push only `port` normally to `myfork`, then create/push the
new annotated `v0.3.0-beta` tag at the verified commit, without replacing a tag.
Create a GitHub prerelease named Wario Land 4 v0.3.0-beta - The Rumble Update,
attach the verified ZIP/checksum and supplied release notes, and read back/download
assets to verify hashes. Keep previous releases and their assets unchanged.

After successful publication, recommend a separate `linux-port` branch/worktree
from the released port commit. First audit os_win32.c memory/exception handling,
x86_access.c, coroutine stack/context, Windows save/settings/ROM file operations,
CMake platform selection and SDL3 dependencies. Design Linux fixed-address mmap,
mprotect and fault/signal handling before enabling the native GBA memory model.
Keep the Windows launcher target guarded. Do not create the Linux worktree or
implement Linux code during the Windows release task.

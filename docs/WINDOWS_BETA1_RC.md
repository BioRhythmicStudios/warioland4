# Beta 1 local release candidate: verification and hold

Proposed public version: `v0.1.0-beta.1`. **Not published.** Functional checks pass;
the rights gate remains open. No fetch/push/tag/release step was attempted after
the distribution audit identified unresolved questions. No public upload,
including a branch push containing the affected art, is cleared by this report.

## Latest local artifact (RC2)

`build/beta1-rc2-20261019/WarioLand4-Native-PC-Port-v0.1.0-beta.1-Windows-x64.zip`

2,830,067 bytes. SHA-256:
`3cc3d702109e401e23d5b8c1144c4563eaa2ea72c8de5cc300700401796de65a`

Adjacent `.zip.sha256` and internal `SHA256SUMS.txt` provide archive/payload hashes.
This is a held local candidate, not an approved public release asset. The earlier
RC1 and original private beta archives/evidence remain untouched.

Native source: **7dd76cbc08ccd524d67a9167a464cc0c0803673a**, branch `port`, adding
the requested YoshiKong credit and asset audit atop VSync **5cef1d7**. All runtime
code was committed before the fresh RC2 build. Later changes are audit/testing
documentation only. The source remote target remains `myfork`, never `origin`.
No tag was created or overwritten and there is no release URL.

## Build and identity

Fresh CMake/Ninja Release, Windows x64/UCRT, Clang 23.1.3/LLVM-MinGW 20261006,
`-O2 -DNDEBUG`, ASan OFF, SDL 3.4.18. EXE debug-only sections were stripped before
packaging/testing. SDL3.dll is the byte-identical vendor runtime used by the
normal build, not a rewritten or newly downloaded DLL. Original tools, existing
builds and source artwork were not replaced.

The game's `.text` SHA-256 is unchanged from the user-tested VSync normal build:
`012d230026105d947449a0f35e496a77bf9920408516a648ba985fe0041acb10`.
The launcher's `.text` and embedded resources match the freshly verified normal
Credits build. This does not claim the entire recompiled/stripped EXE files are
byte-identical to the previously copied Intel-laptop files. The exact shipping
files were independently exercised after ZIP extraction, and then matched back
to the ZIP and fresh build by complete-file SHA-256/content checks.

## Payload and dependencies

The strict allowlist contains exactly 20 files:

- `wl4-launcher.exe`, `wl4.exe`, `SDL3.dll`.
- `README.txt`, `KNOWN_ISSUES.txt`, `CREDITS.txt`, `HARDWARE_TESTS.txt`,
  `BUILD_INFO.txt`, `LICENSE.txt`, `SHA256SUMS.txt`.
- Ten `licenses/` notices: SDL, LLVM runtime, MinGW-w64 runtime/project/AUTHORS/
  disclaimers, minicoro, LuaCoco and notice provenance.

PE32+/AMD64 and each DLL's import closure were inspected. SDL3.dll is the sole
non-Windows imported DLL. Windows 10/11 x64 supplies the Windows/UCRT components;
no compiler, Python, Visual C++ installer or ASan runtime is packaged. Runtime
module inspection resolves SDL3 from the extracted folder, and system modules
from Windows. The already-installed RTSSHooks64 overlay is recorded separately,
left untouched and excluded from the archive.

Archive CRCs, exact filename allowlist, complete manifest coverage and every
payload hash pass. No ROM, SRAM, personal config, credentials, source tree, build
utility, logs or loose extracted-art files are packaged. No local development-user
absolute path was found in the binaries or package notices. **Embedded sprite,
icon and font remain in the launcher.** The no-extracted-Nintendo-art/permission
gate cannot be called passed merely because no PNG appears separately in the ZIP.

## Tests

- Fresh Release build and existing normal/ASan builds pass. Normal/ASan visible
  Credits checks confirm **YoshiKong**, **MisteryGX** and **Nintendo**, with clean
  sequential closure. Credits naming changes do not modify game/input/save logic.
- RC2 Release and ASan settings/input/virtual-gamepad/synthetic-pacing tests pass,
  including VSync defaults, v1/v2 migration/persistence, collision handling,
  corrupt-file recovery, API failures and hotplug fixtures. Virtual tests are not
  claims of fresh physical controller testing.
- RC2 Release (from the extracted ZIP) and ASan coroutine, BIOS/LZ77, trapped I/O
  and disposable-save failure/recovery regressions pass. Extracted Release
  headless startup runs 600 frames; earlier RC1 ASan startup also ran 600 frames.
- The ZIP was extracted into a unique path with spaces outside the repository.
  Its child PATH contains only Windows and System32. Disposable WL4_SETTINGS_FILE
  and explicit disposable --save paths protect normal preferences/SRAM.
- The actual extracted game window is explicitly shown and native visibility
  verified; VSync reports 1, F11 enters/exits fullscreen, and the game closes
  normally. The extracted launcher opens About/Credits/README/Settings in sequence;
  Credits includes YoshiKong/MisteryGX/Nintendo, and Settings is cancelled.
- The launcher Launch button was not clicked because it intentionally uses the
  normal save location. The separate-laptop user has verified normal launcher/
  game operation; this smoke separately verifies the extracted game with an
  isolated save. No repeated Browse or invalid-ROM popups were generated.
- Personal SRAM and configuration SHA-256 values remain identical to the start
  of this preparation. Test-owned processes are closed; no unrelated app stopped.
- Fresh GBA v7-45 and final **v7-46**: ROM-SAME, OBJ-SAME, MD5
  `940b597bef3be3f4041021b47a2f4096`. Next fresh check: v7-47. No tools/agbcc or
  main-tree source changes, and no destructive cleanup.

User-performed Intel/60 Hz OLED evidence is recorded in PRIVATE_WINDOWS_BETA.md
and WINDOWS_VSYNC.md: portable launcher/game work, Off tears visibly, On does
not. Driver/VRR/audio/mode details and full-game completion were not specified.

Evidence: `build/beta1-rc2-20261019/` (builds, credits, imports, binary/ZIP audits,
regressions, module lists, smoke results and checksum). First-candidate evidence
remains under `build/beta1-rc1-20261019/`; prior VSync evidence is preserved.

## Publication gate and required decisions

See [per-asset and upstream audit](LAUNCHER_ASSET_RIGHTS.md).

1. Font provenance is documented; retain its SDL/altered-font notices.
2. MisteryGX's attributed-use condition is verified on the sheet. Broader public
   build/source redistribution and Nintendo-derived base-pixel clearance are not
   established beyond that condition. Record an applicable basis or choose
   permitted original replacement artwork before representing full clearance.
3. The icon source/credit is YoshiKong's Level Icons collection. It appears to be
   game graphics rather than original fan drawing; sheet access was blocked, so
   the user-reported credit footer and exact pixel provenance remain qualified.
   Decide whether to establish the applicable rights or replace/remove this icon.
4. Establish licence provenance/grants for the imported upstream code revision.
   Restoring lilDavid's MIT notice does not on its own establish coverage for
   later contributions; absence of a current notice also does not revoke earlier
   valid grants. This needs a scoped provenance/grant resolution, not attribution.

Public push, annotated tag and prerelease remain stopped. No force push or history
rewrite is planned. Fetch/remote-divergence checks must be freshly performed once
the gate is resolved, before any future authorized push.

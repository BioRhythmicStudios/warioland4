# Private Windows x64 beta 1 (2026-10-19)

## Manual laptop results and release-candidate follow-up

The user reports the original portable beta works on their second Windows
laptop without development tools. They subsequently tested the updated launcher
and game containing VSync commit `e260529` on that separate laptop, with Intel
integrated graphics and a 60 Hz OLED panel, and explicitly confirmed:

- VSync Off: visible screen tearing.
- VSync On: no visible screen tearing.
- Both updated executables work on the laptop.

This is user-performed physical hardware validation, not a mock or an inference
from SDL's VSync status. Intel model/driver, Windows version, test duration,
VRR state, individual display modes, audible quality and full-game completion
were not specified. Those details are not inferred from the report.

The original pre-VSync archive below is historical and remains unchanged.
See [Beta 1 release-candidate verification](WINDOWS_BETA1_RC.md) for the new candidate.
Earlier automated hidden-window tests are not scanout validation; the VSync
report records the corrected explicitly visible tests and that limitation.

User-authorized USB/laptop package, not a public release. No push, publication,
save migration or game/launcher source changes were performed.

## Artifact

`build/private-beta1-20261019/WarioLand4-PC-PrivateBeta1-Windows-x64.zip`

2,866,473 bytes. SHA-256:
`3a196ed2da3b5a38c6e912387ffe924c5d466375c8262d24f30904fcf8eb487d`

Adjacent `.zip.sha256` checks the archive. Its internal `SHA256SUMS.txt` checks
every other payload file. Archive CRCs, complete manifest coverage and every
payload hash were verified. The allowlist packager and all test evidence remain
under `build/private-beta1-20261019/`; existing evidence was not cleaned.

Extract All, copy the whole extracted folder to USB/laptop, open
`wl4-launcher.exe`, Browse for the tester's own supported US ROM, then Launch.
Keep both EXEs and SDL3.dll together. README.txt provides default controls,
troubleshooting, data locations and a laptop checklist. No development tools or
installation are needed by the payload. Windows 10/11 x64 is the beta target;
acceptance on the separate laptop is still pending.

## Build and payload

Source revision `452b3e3`, following local font/icon `6d241cf` and published base
`b1474c4`. A fresh `native-release` CMake/Ninja directory uses LLVM-MinGW
20261006, Clang 23.1.3, Release `-O2 -DNDEBUG`, ASan OFF, and SDL 3.4.18. It does
not change the existing normal/ASan builds. The dedicated input-test executable
is built only for verification and is excluded from the ZIP.

Only `wl4-launcher.exe`, `wl4.exe`, `SDL3.dll`, README, known issues, build info,
credits, integrity manifest and licences are included. Art, icon and private font
are embedded in the launcher. Import inspection confirms AMD64 binaries; SDL3
is the only non-Windows imported DLL, including SDL3's own dependency closure.
No compiler, ASan DLL, Python, source tree, ROM, extracted assets, SRAM, personal
configuration, credentials or loose artwork is packaged. Text and executable
scans found no development-user absolute path (the normal generic
`baserom_us.gba` command-line default is intentionally present in the game).

Notices include project MIT, SDL zlib, minicoro's dual licence, LuaCoco MIT,
LLVM runtime and official MinGW-w64 runtime/project notices and disclaimers.
MisteryGX remains credited both in the launcher and package. The supplied icon's
artist and sprite/icon redistribution grants are not documented. Confirm those
permissions before wider sharing; software licensing does not grant character
or artwork rights. This archive is for the requested private personal evaluation.

## Verification and limits

- Archive extracted to a unique Windows temporary directory outside the
  repository, with spaces in its path. Child PATH restricted to Windows and
  System32; working directory was the extracted application folder.
- Release coroutine, BIOS/LZ77, I/O, disposable save failure/recovery tests and
  600-frame headless game startup passed. Deliberate save-error diagnostics are
  negative tests against disposable files; no GUI error/file dialogs were opened.
- Extracted game window started with an explicit disposable `--save`, loaded
  SDL3.dll from the extracted folder, and closed normally.
- Extracted launcher opened About, Credits, README and Settings sequentially;
  MisteryGX attribution was checked. Each owned dialog closed before the next.
  Settings cancelled without saving. No Browse or Launch button was clicked.
- Existing RivaTuner injected RTSSHooks64.dll into GUI processes. It was recorded
  separately as an external overlay, left running, and not packaged. Other loaded
  DLLs came from the package or Windows. This is not a pristine-machine proof.
- Release and existing ASan input/config/virtual-gamepad tests passed. ASan
  runtime/coroutine/BIOS/I/O/disposable-save regressions passed. ASan tests use
  the compiler's runtime on PATH; the release needs no ASan runtime. Zero real
  controllers were connected during these tests; earlier manual hardware results
  remain documented in WINDOWS_SETTINGS.md.
- Explicit WL4_SETTINGS_FILE overrides protected real settings. Every game test
  had a disposable save path, or was a headless/test mode which never uses the
  default save. Real SRAM SHA-256 was identical before/after the final smoke run.
  No ROM copies were made; the supported ROM was read from its existing location.
- Initial harness failures were exit-code retention, empty window-title matching,
  SDL hidden-helper-window selection and overlay detection. These were corrected
  in the test harness only. Each run cleaned up its own processes. Final run passes;
  all owned test processes closed. Temporary extractions remain as evidence.
- Launcher-to-game Launch was previously verified, but was not repeated here:
  its child intentionally uses the normal save location. The laptop checklist
  covers the complete Browse/Launch/save/restart workflow with that laptop's user
  profile. Do not substitute APPDATA redirection for explicit save isolation.
- Fresh GBA `build/v7-41`: MD5 `940b597bef3be3f4041021b47a2f4096`, ROM-SAME,
  OBJ-SAME. No tools/agbcc or main-tree changes. Next fresh check: `v7-42`.

The application folder is portable; preferences and SRAM remain in the Windows
user profile, not on USB. ROM-path persistence is per-user registry state.
Full-game completion, clean-laptop operation, high DPI and older Windows remain
acceptance items. Keep one game instance running at a time.

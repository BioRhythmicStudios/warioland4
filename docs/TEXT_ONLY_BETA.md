# Text-only local Beta 1 — 2026-10-19

Fresh build from HEAD `ecf5771` plus the uncommitted launcher-only changes in
this worktree. No tag, push or publication. Prior beta archives remain intact.
Existing upstream audit documents/work are preserved.

## Package

`build/text-beta-20261019/WarioLand4-Native-PC-Port-Beta1-text-only-Windows-x64.zip`

SHA-256: `61b9d2ea1a5dcc92c3730c63f46ba12a826f741e2fd29b4c0e14d25322a06b1e`

22 allowlisted files: fresh Release x64 launcher/game, vendor SDL3.dll, seven
instruction/credit/build/integrity documents and twelve licence/provenance notices.
No ROM, extracted game asset bundle, personal save/config, logs or tools.
BUILD_INFO records the base commit, local changes and runtime-source hashes.
SHA256SUMS covers every payload file other than itself; the adjacent ZIP checksum
covers the archive. Rebuilt game EXE is not claimed byte-identical to earlier
native builds; game sources and behaviour were not intentionally changed.

## Changes

- Removed resource 101 (Vampire Wario PNG), WIC decoder, sprite painting and
  artwork startup dependency. Removed unused WIC/COM/image-blending libraries.
- Replaced old ICO with an original square-within-square icon generated entirely
  by `port/tools/make_neutral_icon.py`, with no external art input. Committed
  source assets are preserved historically, but not included in this package.
- Retained text-only purple/gold layout, controls, ROM validation/launch logic,
  settings, About, Credits and built-in README. Credits describe shipped assets.
- Retained private pixel font: SDL release-3.4.18 debug-font header explicitly
  identifies Marcel Sondaar / IBM public-domain VGA fonts. SDL wrapper is zlib;
  TTF conversion is documented as altered packaging. licenses/PIXEL-FONT.txt
  includes the provenance and full SDL notice; licenses/NEUTRAL-ICON.txt covers
  the new project icon. No system font is copied or installed.

Font source: https://github.com/libsdl-org/SDL/blob/release-3.4.18/src/render/SDL_render_debug_font.h
The exact header cached during the earlier audit was reinspected; this turn's
web fetch failed, so no claim of a new successful download is made.

## Verification

- Fresh Release and separate ASan builds pass. ASan needs the LLVM runtime on
  test PATH; initial missing-runtime exits were corrected before running tests.
  Shipping package is Release with ASan OFF.
- Normal and ASan isolated settings/keyboard/virtual-gamepad and runtime
  selftest/BIOS/IO/disposable-save tests exit 0. No new hardware controller or
  Intel-laptop validation is claimed for this artwork-only build.
- Normal and ASan launcher startup/Credits/close tests pass sequentially. A
  captured normal window was inspected: text-only, geometric icon, usable layout.
  Every owned test GUI process was closed; no Browse dialogs were opened.
- PE resources: only four geometric RT_ICON images, their RT_GROUP_ICON, and
  the retained font RCDATA 102. No sprite resource 101. Icon payloads match the
  generated ICO exactly; font resource matches the retained TTF exactly.
- Binary search rejects the old sprite PNG, old icon PNG/ICO, every old ICO
  image payload and PNG IDAT chunks: 12 payloads absent. Positive control on
  the held old launcher detects nine of those payloads. Removed creator/image
  strings are absent in UTF-8/ASCII and UTF-16. Evidence: resource-audit.json.
- Only non-Windows imported launcher DLL is SDL3.dll; unused imaging imports
  are gone. Vendor SDL3.dll matches the previous local build byte-for-byte.
- ZIP CRC, exact 22-file allowlist, extraction/payload byte comparisons pass.
  Extracted game selftests pass with Windows-only PATH and disposable save/config;
  extracted launcher startup/Credits/close passes. Package files match build files.
- Fresh original GBA `build/v7-47`: MD5 940b597bef3be3f4041021b47a2f4096,
  ROM-SAME, OBJ-SAME. No game-source, main-tree or tools/agbcc edits.

Logs/scripts/resource report are preserved under build/text-beta-20261019,
build/text-beta-asan-20261019 and build/text-beta-* files. This report does not
resolve the separate upstream contributor/game-code licensing research.

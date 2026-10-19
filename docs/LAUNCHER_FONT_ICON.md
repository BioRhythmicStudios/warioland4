# Launcher pixel font and icon (2026-10-19)

User-approved follow-up to published HEAD `0c2a4a5`. The main Win32 launcher now
uses a private TrueType conversion of the exact SDL debug font seen in Settings.
Labels, native edit/combo controls and gold buttons use 2x 8x8 glyphs; the title
uses 4x. About, CREDITS, README and ordinary error messages use owned themed
windows with readable/copyable multiline text, including CRLF conversion and
OK/Escape dismissal. Fatal startup fallbacks and OS-owned title/file-dialog text
retain Windows fonts. The client area grows to 900x400 to fit square glyph widths.
The existing Vampire Wario sprite and MisteryGX credit are preserved.

The user's Downloads/launchericon.png is retained byte-for-byte in port/assets.
The 56x57 original is aspect-preserved in seven square ICO sizes using nearest
neighbour and transparent padding. Resource 1 supplies the executable's Explorer
and default-shortcut icon. Main, Settings and information windows set small/big
icons. Existing desktop shortcuts are neither created nor modified; those using
the executable's default icon inherit the new resource.

Font registration is private to the launcher process. No global installation,
new runtime library, or normal-build Python dependency is introduced. The optional
regeneration utilities and font provenance are documented under port/assets/fonts.
The original icon PNG SHA256 is
`4f0a969e021d64af060ab1f07efae153c00de04b048468b7683229d7102961dc`.

Verification:

- Normal and ASan builds pass without launcher warnings.
- Native GDI reports WL4 Pixel with 16x16 cells. All 191 glyphs / 48,896 display
  pixels match the installed SDL3 bitmap exactly at 2x, with zero mismatches.
- Both executable icon groups contain seven variants. Windows loads all requested
  sizes (16, 24, 32, 48, 64, 128, 256). Main and Settings small/big icons are set.
- Sequential normal/ASan UI checks pass for main layout, About/Credits/README,
  missing-ROM error, information-window dismissal, Settings icon and clean exit.
  Main and Credits previews were visually inspected; all test processes cleaned
  up in finally blocks. ASan launcher stderr is empty.
- UI tests use explicit disposable WL4_SETTINGS_FILE paths, do not save settings
  and never launch a game. No SRAM file or ROM is opened or altered by these tests.
- Fresh GBA v7-37/v7-38 and final **v7-39**: ROM-SAME, OBJ-SAME and MD5
  `940b597bef3be3f4041021b47a2f4096`. Next fresh directory: v7-40.

Evidence: ignored build/launcher-font-20261019, including font atlas/GDI comparison,
font-results.txt, icon-results.txt, ui-results.txt, normal/ASan launcher logs and
owned-window previews. No prior evidence removed. Settings persistence, bindings,
ROM validation and game/save code were not changed. Windows icon cache refresh,
small-screen/high-DPI acceptance and glyph fallback outside Latin-1 are not claimed.
No push or release was created for this follow-up.

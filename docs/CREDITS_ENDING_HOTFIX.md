# Golden Diva ending / credits native-port fix (2026-10-22)

CM30's GitHub Issue #2 `wl4.zip` contains one 32 KiB `wl4.sav` (ZIP integrity
check passed). The contained save has SHA-256
`daecc6ff2014ea29e006ef2de104a5a0c213cd550b519833c618e48706fbb344`,
identical to the separately supplied Downloads save. Testing used copies only
under ignored `build/`; no personal save, configuration or ROM was edited.

The game's `CreditsVCountUpdateBgOffset` callback waits for HBlank on scanline
126. On GBA hardware, time continues while the interrupt handler spins. In the
native port, trapped `DISPSTAT` reads inside interrupts intentionally preserve
the current event, to prevent HBlank handlers consuming VBlank. Consequently
this particular wait could never observe HBlank and the game would hang at the
start of the staff roll. A focused regression registered the real credits
callback as the VCount handler: the pre-fix binary timed out, while the fixed
binary completed three VBlank frames. The `PORTABLE`-only fix advances the
native display clock *inside this wait*; GBA behavior is unchanged.

The `--test-credits` headless regression boots with an isolated copy of the
issue save, enters credits at frame 1150 after save selection, and requires the
entire sequence to leave `GM_CREDITS`. On both Windows and WSL2 Linux normal
builds, it reached the next cutscene at frame 15263. Linux ASan also completed
at frame 15263. This jump exercises the complete credits but does not replay
the full Golden Diva fight or prove the preceding cutscene transition on real
hardware. That remains the focused manual retest before a public update.

Normal Windows and Linux BIOS, I/O, and Toy Block Tower tile-wrap tests pass.
Windows CRT, settings/keyboard/virtual-gamepad and rumble-event tests pass
using disposable settings. The Linux credits run also passed under ASan.
Fresh GBA build `build/v7-82` has MD5
`940b597bef3be3f4041021b47a2f4096`: ROM and all 202 objects are
byte-identical to `build/us`. The Windows ASan binary built but exited before
printing a test result on this host; Linux ASan is the completed memory check.
An independent Linux-worktree GBA build in `build/v7-86` could not compile
because its protected `tools/agbcc/agbcc` symlink target is unavailable; it
was not changed. The Windows-worktree fresh identity build is complete.

The private extracted AppImage candidate in Linux
`build/appimage-credits-fix-2` contains the same tested ELF (SHA-256
`c85a1065b4aa679d81f29855ff87428f31876bfc9d73e89bd6e5dfcc55fbf0c8`).
The AppImage itself is 4,762,104 bytes with SHA-256
`4d9e0f4cb131b440fc3ae4e2a0beedff2bbc6d439f4b9b65e7275bf5f9a65ff9`.
Its bundled SDL3 resolves from the AppImage and glibc/ELF loader from WSL2.
The extracted image passed BIOS, I/O, Toy Block Tower tile-wrap and the full
credits regression. This is WSL2 validation, not a real Steam Deck playtest.
No ROM or save is included in the AppImage.

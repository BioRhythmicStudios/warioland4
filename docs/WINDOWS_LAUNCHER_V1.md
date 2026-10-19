# Windows launcher v1 verification (2026-10-19)

A separate Win32 C executable uses standard controls, GetOpenFileNameW,
CreateProcessW, and HKCU settings. CMake builds it alongside the SDL3 game with
LLVM-MinGW; it has no SDL or large GUI dependency itself. The game's ROM loader
and launcher share PortValidateRom, the ROM-size constant and supported hash.
No GBA sources, generated files or save implementation changed.

## Results

- Normal and ASan builds succeed without launcher warnings.
- Both launchers: empty path, missing file, short invalid ROM, and an 8 MiB
  zero-filled wrong-hash fixture produce the expected visible errors.
- Both launchers: native Browse selects the genuine supported ROM through a
  directory path containing spaces. Closing and reopening restores that path.
- Both launchers start their adjacent native game with the quoted absolute path
  and `--scale 3`; captured child command lines confirm this. The game window
  opens and closes normally, and the launcher enables Launch Game again.
- Both About dialogs contain the requested credits.
- A separate executable fixture reports missing wl4.exe, missing SDL3.dll and an
  incompatible dummy SDL3.dll without starting a game.
- Normal and ASan coroutine, BIOS, trapped I/O and disposable-file save tests pass.
  Direct command-line game runs also pass for 60 frames through the spaced path.
  Independent normal/ASan game windows start and close with exit code 0.
- Final fresh GBA build v7-32: MD5 940b597bef3be3f4041021b47a2f4096,
  ROM-SAME and OBJ-SAME. Earlier implementation checks were v7-30 and v7-31.

All new local evidence and controlled scripts are under ignored
`build/launcher-v1-20261019/`: `gui-results.txt`, `negative-results.txt`,
`dependency-results.txt`, build-specific runtime logs and child command lines.
The latest controlled scripts are `verify.ps1`, `negative-controlled.ps1`,
`dependencies-controlled.ps1` and `browse-controlled.ps1`. They require an
interactive Windows desktop and test only one launcher at a time. The GUI
scripts use a finally block to close their owned test processes and check cleanup.
No automated script is needed by launcher users.

## Test corrections and limits

Initial GUI automation incorrectly used GetWindowText on another process's edit
control. It falsely reported an empty path after Browse and left nine launchers
with modal dialogs on assertion failures. After the user reported the stacked
dialogs, testing paused and those nine identified processes were closed. The
harness now reads edit text with WM_GETTEXT and guarantees cleanup. Earlier
failed automation is not counted as passed verification.

ASan then found a real stack overflow: long path buffers in window callbacks
consumed excessive stack when Windows pumped nested dialog messages. Those
buffers now use static storage. Both positive and negative GUI checks pass after
that fix; the final ASan launcher stderr is empty. Owned launcher/game processes
were closed at the end of the controlled checks.

Tests use an explicitly disposable APPDATA beneath the evidence directory, and
headless save tests use fresh disposable children there. The genuine ROM is read
through a directory junction; its bytes are never copied or modified. No real
player save is opened by these tests. Existing test evidence is preserved.
Launcher tests remember their selected path in the launcher-only registry key;
Browse can replace it on the next ordinary launch.

Clean-PC acceptance, arbitrary Unicode paths beyond the game's active code page,
high-DPI accessibility review, and multi-instance save conflicts are not claimed.
The launcher does not offer a startup fullscreen flag because the game currently
supports only the F11 toggle. No level/room selector, package, push or release was
created. Further milestone work requires user approval.

# Launcher settings and customizable controls (2026-10-19)

## Implementation

A shared `wl4_settings` library contains defaults, validation, versioned config I/O
and SDL input polling. `settings_window.c` is a separate SDL window with Keyboard,
Controller and Display tabs, matching the launcher's purple/gold palette. The main
Win32 launcher, sprite, Credits and README remain intact. No core game source,
GBA input routine, ROM data, generated symbols or save implementation changed.

Keyboard bindings store SDL physical scancodes. Ten actions use the original GBA
bit order: A, B, Select, Start, Right, Left, Up, Down, R, L. The frontend combines
keyboard and controller state, filters opposite directions and still writes
`(~pressed & 0x3FF)` to KEYINPUT before each frame. Physical input is neutral when
the game window lacks focus; script inputs retain their existing path. F11 ignores
repeats; loss of focus also releases fast-forward.

SDL3's gamepad API supplies Xbox/XInput-compatible support; there is no separate
XInput implementation. One shared binding profile applies to supported gamepads;
the first available device is opened and replaced on removal. The Settings window
shows connected names (cycling the additional-name line if necessary). All SDL
buttons plus LT/RT can be chosen without hardware, or captured when present.
Unsupported physical extras naturally depend on the device. Controller connection
and removal use the same helper in the game, settings window and virtual tests.
Left-stick movement is optional, with a 10-80% per-axis dead zone; triggers activate
above 16384. Keyboard stays available with or without a controller.

Binding collisions within each device are rejected without mutating the prior
binding. To swap two occupied bindings, use a spare input temporarily or restore
defaults. Escape cancels capture; Escape, Tab, F8 and F11 cannot be gameplay keys
because they retain their existing frontend functions. Defaults restore only the
selected section. Cancel leaves the existing file untouched. Changes apply to new
game processes, not a game already running.

## Configuration and compatibility

Ordinary config: SDL_GetPrefPath(NULL, "wl4") + `settings.cfg`, normally
`%APPDATA%\wl4\settings.cfg`. SRAM remains the separate 32 KB `wl4.sav`, at its
existing location or explicit --save override. The legacy ROM-path registry key
remains, and its scale is imported when the config is missing. Saved config scale
subsequently takes precedence; launcher scale changes are persisted on launch.
Direct game launch loads the same bindings/display preferences. Explicit --scale
still overrides the saved scale; no new fullscreen CLI option is required.

The strict text format starts with `WL4_SETTINGS 1`, followed by four integers
(scale, fullscreen, analog enabled, deadzone percent), ten scancode integers and
ten gamepad integers in GBA bit order. Button codes are SDL_GamepadButton; 26 and
27 represent LT and RT for the tested SDL 3.4.18 build. Missing files use defaults.
Unreadable, incomplete, oversized, wrong-version, malformed, duplicate and invalid
settings fall back as a whole with a warning. Loading never overwrites a bad file.
An explicit Settings Save or Launch Game writes the selected valid configuration.

Writes create an exclusive temporary sibling, flush/close it and atomically
replace the destination. Failure reports an error; a failed temporary is retained
for diagnosis. Config writes never call SRAM save code. Concurrent launchers are
last-successful-writer-wins; profile merging and live reload are outside this step.
Developer/test override `WL4_SETTINGS_FILE` names an explicit alternative config
file and is inherited by a game started from that launcher. Normal use needs no
environment variables or terminal commands.

## Verification

- Normal and ASan builds pass without new warnings.
- `wl4-input-test --isolated`: missing/default config, physical-key remap, reserved
  keys, per-device collisions, all-field persistence, corrupt/truncated/old-version
  fallback, invalid-value rejection and preservation of a valid existing config.
- SDL virtual gamepad tests pass in both builds: face button, D-pad, remapped RT,
  neutral trigger, adjustable stick dead zone, disabled stick, disconnect, keyboard
  fallback and reconnect. These tests are software fixtures, not hardware claims.
- Controlled normal/ASan launcher UI tests pass: physical scancode capture (using
  messages to the owned test window), conflict status, Escape cancellation,
  controller trigger chooser without hardware, dead-zone edit, Display edit,
  Save/reopen and draft Defaults followed by Cancel without file changes.
- Normal/ASan coroutine, BIOS, I/O and disposable-save regression tests pass.
  Independently started game windows load the saved `A=C` binding and fullscreen,
  accept an explicit --scale 1 override and close with exit code 0.
- SDL initially reported no hardware, then detected **Xbox 360 Controller** after
  the user enabled it. The user manually confirmed rebinding A to C and Right to D
  works in gameplay, confirmed the controller works, and explicitly confirmed
  controller disconnect/reconnect plus keyboard fallback. Exact controller model,
  build variant and individual controller bindings were not specified; no physical
  ASan hardware coverage is asserted from these manual reports.
- Fresh GBA builds **v7-34** and final **v7-35** print ROM-SAME and OBJ-SAME;
  ROM MD5 `940b597bef3be3f4041021b47a2f4096`. Next fresh build: v7-36.

Evidence: ignored `build/settings-20261019/`, including unit-normal-final.log,
unit-asan-final.log, ui-results.txt, runtime-results.txt, config fixtures, UI PNGs,
explicit disposable saves, and controlled PowerShell scripts. The compiled
`wl4-input-test` refuses to run without an explicit new override destination.
Tests close only their own processes in finally blocks and restore prior registry
launcher values. No prior evidence was deleted. No push or release was created.

## Test limits and corrected older evidence

Automated gameplay key-message attempts reached Hall but did not demonstrate the
remapped movement: the background harness could not obtain real keyboard focus.
The guarded global-input attempt refused injection. These attempts were cleaned
up and are not counted as successful gameplay verification; the subsequent user
manual gameplay reports supply that verification instead.

Inspection found that older launcher/theme tests' APPDATA environment override did
not redirect this SDL build's known-folder preference path. An old independent
window log shows the real save path was opened. The old claims that these specific
launcher GUI runs never accessed the normal save are withdrawn; whether they
changed it cannot be proved retrospectively. Older save-recovery tests that passed
explicit --save paths are a separate set of evidence.

All automated game runs in this settings milestone use explicit disposable --save
paths. All automated config tests use explicit WL4_SETTINGS_FILE paths. A before
and after SHA256 snapshot of the real player save is preserved; it changed during
the user-controlled manual playtesting period. It was not restored or overwritten
by the agent. ROMs are read-only inputs. No default save-location change was made.

This small SDL editor uses mouse-operated controls and a pixel font. Screen-reader
accessibility, high-DPI/small-screen acceptance, multiple controller profiles,
right-stick movement and live in-game rebinding are not claimed for this version.

# Wario Land 4 — private Steam Deck Linux test

This folder is a native Linux x86-64 test build, for Desktop Mode. It does not
contain a ROM, game artwork, saved progress, or a Windows/Wine executable. Supply
your own unmodified US ROM (8 MiB; MD5 5fe47355a33e3fabec2a1607af88a404).
The executable reads the ROM; it does not modify it.

## Graphical Steam Deck test

In Desktop Mode, open Downloads in Dolphin and extract the archive to a new
folder. Open the extracted wl4-steamdeck-private-20261022-inputtrace3-toyfix folder.
Double-click Play-Wario-Land-4.desktop for ordinary play, or
Diagnose-Wario-Land-4.desktop to investigate the input-triggered Desktop Mode
window closure. If Dolphin asks whether to execute a launcher, choose Execute.
Both launchers find
~/Downloads/baserom_us.gba automatically; otherwise it opens a file picker.
With the diagnostic launcher, press one Deck button that previously closed the
window. Please note which physical button you pressed. If it stays open, try
the on-screen keyboard and then Start. Dolphin selects one new
wl4-deck-diagnostic-*.log file in the diagnostics folder. Copy that single
log back to the developer. You do not need Konsole.

Every diagnostic run uses a new disposable save and settings folder inside
diagnostics/run-*. It never reads or writes the normal userdata save/settings.
The log records the game's own exit status, dependency paths, audio backends
and devices, SDL keyboard/gamepad/quit events, frame timing, and available
core details. A full backtrace depends on SteamOS
providing coredumpctl and gdb; the game also reports a fault PC for unhandled
Linux memory faults, which can be symbolized from this exact ELF.

The direct Desktop Mode game is known to open before a button press closes its
window. Steam-launched gameplay has worked. This package records the input
event immediately before that closure and the game's real exit status; it does
not assume a startup, audio or glibc failure. SteamOS audio was restored after
a Deck restart. The exact closing event remains to be identified from a real
Deck diagnostic log.

## Optional terminal launch

Copy the .tar.gz package and its .sha256 file to the Deck's Downloads folder,
then open Konsole. Use your actual ROM location in the final command:

    cd ~/Downloads
    sha256sum -c wl4-steamdeck-private-20261022-inputtrace3-toyfix.tar.gz.sha256
    mkdir -p ~/Games/wl4
    tar -xzf wl4-steamdeck-private-20261022-inputtrace3-toyfix.tar.gz -C ~/Games/wl4
    cd ~/Games/wl4/wl4-steamdeck-private-20261022-inputtrace3-toyfix
    ./play.sh --check
    ./play.sh "/absolute/path/to/your/baserom_us.gba"

Keep the extracted folder writable. The game creates userdata/wl4.sav and
userdata/settings.cfg there. These are your test save and settings; keep the
folder if you want to keep progress. The package never uses your Windows save or
settings. Replacing the extracted folder with another build can replace those
files, so copy userdata/ somewhere safe before updating.

Default keyboard: arrows to move; X/Z for A/B; A/S for L/R; Enter to start;
Backspace to select; Escape to exit and save; F11 fullscreen; Tab fast-forward.
The Deck controls should appear as an SDL gamepad in Desktop Mode. If they do
not, check the Desktop Mode Steam Input layout or use a keyboard. Physical Deck
controls and rumble still need acceptance testing.

## Display, sound and controls

The game reads userdata/settings.cfg at startup. Close the game before using:

    ./configure.sh vsync on
    ./configure.sh vsync off
    ./configure.sh crt original
    ./configure.sh crt lite
    ./configure.sh crt enhanced
    ./configure.sh rumble on

Rumble defaults Off. configure.sh creates a default v4 settings file on first
use, preserving all bindings and other fields on later changes. It does not
touch the save. For other changes, edit the documented numeric settings format
in settings.template or use the Windows settings editor on a separate copy.

Test VSync by moving through a scrolling scene with it on and off. Test all
three CRT presets at fullscreen or at least 3x size; scanlines are suppressed
at 1x and the mask needs 3x. Check sound, pause/resume, a save followed by a
reload, and Deck buttons/rumble. Please report your SteamOS version, whether
Desktop Mode uses Wayland or X11, and any diagnostic log from the graphical
launcher. If the Deck has no audio device in system settings, note that too;
the game cannot produce sound until the system exposes a playback device.

For diagnosis, run ./play.sh --check; this prints library
resolution. This package includes SDL3 3.4.2 built with a glibc 2.38 symbol
floor and dynamic PipeWire, PulseAudio and ALSA audio backends, with notices in licenses/ and hashes in DEPENDENCIES.json. It uses the
SteamOS glibc, ELF loader, GPU drivers, window server, and audio server.
This revised package was tested after extraction on WSL2/WSLg; its physical
Deck retest is pending.
Some host driver combinations may need a build against Valve's Steam Linux
Runtime. This is a private test package, not a general Linux release.

The game needs glibc 2.38 or newer. Check ldd --version on the Deck if the
loader reports a missing GLIBC version. Library availability and input on a
physical Deck have not yet been tested. WSLg synthetic X11 keys sometimes
need the window raised and focused more than once; this is a WSLg test issue.

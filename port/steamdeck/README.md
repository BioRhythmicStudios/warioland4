# Wario Land 4 — private Linux x86-64 test

This folder is a native Linux x86-64 test build, for Desktop Mode. It does not
contain a ROM, game artwork, saved progress, or a Windows/Wine executable. Supply
your own unmodified US ROM (8 MiB; MD5 5fe47355a33e3fabec2a1607af88a404).
The executable reads the ROM; it does not modify it.

## Graphical Steam Deck and Steam Machine play

In Desktop Mode, extract the archive with Dolphin and open the extracted
folder. Double-click Play-Wario-Land-4.desktop. If Dolphin asks whether to
execute the launcher, choose Execute. It finds
~/Downloads/baserom_us.gba automatically or opens a ROM file picker. This
ordinary launcher keeps test saves and settings in the extracted folder's
userdata/ directory. Keep that folder if you want to keep your progress.

**Steam Deck Desktop Mode controls:** set the Deck's Desktop Mode controller
layout to **Gamepad Mode** before playing directly outside Steam. The default
**Desktop Layout** maps the physical B button to the keyboard Escape key.
Escape intentionally quits the game, so that layout made the window close
when B was pressed. With Gamepad Mode selected, the user confirmed direct
standalone play stays open and both A and B work. The game does not need an
input-code workaround. Steam-launched Desktop and Gaming Mode were also
reported working. For a Steam Machine, use a gamepad layout that sends
gamepad buttons rather than desktop keyboard shortcuts.

If an unexpected closure returns, double-click
Diagnose-Wario-Land-4.desktop. It opens the game with a fresh disposable
save and settings in diagnostics/run-*, captures the game's actual exit
status and recent SDL events, then selects one
diagnostics/wl4-deck-diagnostic-*.log file in Dolphin. Copy that one log to
the developer. This diagnostic launcher does not touch userdata/ progress
and needs no Konsole commands. Please note the physical control you used.

## Optional terminal launch

Copy the .tar.gz package and its .sha256 file to the Deck's Downloads folder,
then open Konsole. Use your actual ROM location in the final command:

    cd ~/Downloads
    sha256sum -c wl4-linux-private-20261022-gamepad-toyfix.tar.gz.sha256
    mkdir -p ~/Games/wl4
    tar -xzf wl4-linux-private-20261022-gamepad-toyfix.tar.gz -C ~/Games/wl4
    cd ~/Games/wl4/wl4-linux-private-20261022-gamepad-toyfix
    ./play.sh --check
    ./play.sh "/absolute/path/to/your/baserom_us.gba"

Keep the extracted folder writable. The game creates userdata/wl4.sav and
userdata/settings.cfg there. These are your test save and settings; keep the
folder if you want to keep progress. The package never uses your Windows save or
settings. Replacing the extracted folder with another build can replace those
files, so copy userdata/ somewhere safe before updating.

Default keyboard: arrows to move; X/Z for A/B; A/S for L/R; Enter to start;
Backspace to select; Escape to exit and save; F11 fullscreen; Tab fast-forward.
The Deck controls work in Desktop Mode when its layout is set to Gamepad
Mode. Escape remains an intentional keyboard quit command. Controller rumble
and broader physical hardware coverage still need acceptance testing.

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
floor and dynamic PipeWire, PulseAudio and ALSA audio backends, with
notices in licenses/ and hashes in DEPENDENCIES.json. It uses the
SteamOS glibc, ELF loader, GPU drivers, window server, and audio server.
This revised package was tested after extraction on WSL2/WSLg. The user
confirmed standalone Deck A/B input with Gamepad Mode; this newly packaged
archive and Steam Machine hardware still need separate acceptance testing.
Some host driver combinations may need a build against Valve's Steam Linux
Runtime. This is a private test package, not a general Linux release.

The game needs glibc 2.38 or newer. Check ldd --version on the Deck if the
loader reports a missing GLIBC version. The SteamOS library check for this
newly packaged archive remains to be confirmed on Deck hardware. WSLg
synthetic X11 keys sometimes need the window raised and focused more than
once; this is a WSLg test issue.

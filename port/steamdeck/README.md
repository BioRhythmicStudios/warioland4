# Wario Land 4 — private Steam Deck Linux test

This folder is a native Linux x86-64 test build, for Desktop Mode. It does not
contain a ROM, game artwork, saved progress, or a Windows/Wine executable. Supply
your own unmodified US ROM (8 MiB; MD5 5fe47355a33e3fabec2a1607af88a404).
The executable reads the ROM; it does not modify it.

## Install and start in Steam Deck Desktop Mode

Copy the .tar.gz package and its .sha256 file to the Deck's Downloads folder,
then open Konsole. Use your actual ROM location in the final command:

    cd ~/Downloads
    sha256sum -c wl4-steamdeck-private-20261021-compat.tar.gz.sha256
    mkdir -p ~/Games/wl4
    tar -xzf wl4-steamdeck-private-20261021-compat.tar.gz -C ~/Games/wl4
    cd ~/Games/wl4/wl4-steamdeck-private-20261021-compat
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
Desktop Mode uses Wayland or X11, and any error text shown in Konsole.

For diagnosis, run ./play.sh --check; this prints library
resolution. This package includes SDL3 3.4.2 built with a glibc 2.38 symbol
floor, with notices in licenses/ and hashes in DEPENDENCIES.json. It uses the
SteamOS glibc, ELF loader, GPU drivers, window server, and audio server. It was tested after extraction on WSL2/WSLg, not yet on a Deck.
Some host driver combinations may need a build against Valve's Steam Linux
Runtime. This is a private test package, not a general Linux release.

The game needs glibc 2.38 or newer. Check ldd --version on the Deck if the
loader reports a missing GLIBC version. Library availability and input on a
physical Deck have not yet been tested. WSLg synthetic X11 keys sometimes
need the window raised and focused more than once; this is a WSLg test issue.

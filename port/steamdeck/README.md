# Wario Land 4 — private Linux x86-64 test

This folder is a native Linux x86-64 test build, for Desktop Mode. It does not
contain a ROM, game artwork, saved progress, or a Windows/Wine executable. Supply
your own unmodified US ROM (8 MiB; MD5 5fe47355a33e3fabec2a1607af88a404).
The executable reads the ROM; it does not modify it.

## Graphical Steam Deck and Steam Machine play

In Desktop Mode, extract the archive with Dolphin into a new folder. Open it
and double-click **Play-Wario-Land-4.desktop**. Choose Execute if Dolphin
asks. This shortcut works from any writable extraction folder, including
folders with spaces. If Dolphin cannot run the shortcut, double-click
**play_gui.sh** in the same folder and choose Execute. The native Linux
launcher uses the Windows port's purple, gold and
pixel-font style. It automatically validates ~/Downloads/baserom_us.gba, or
use Browse to select your own unmodified US ROM. The selected path is kept in
userdata/launcher_rom.txt inside this test folder. Click Settings to edit
keyboard/controller bindings, window scale, fullscreen startup, VSync, CRT
filters and rumble. Save changes, then click Launch Game. Changes apply on
the next game launch. The game saves to userdata/wl4.sav and reads
userdata/settings.cfg; keep the folder if you want to keep progress.

**Steam Deck Desktop Mode controls:** set the Deck's Desktop Mode controller
layout to **Gamepad Mode** before playing directly outside Steam. The default
**Desktop Layout** maps physical B to keyboard Escape. Escape intentionally
quits the game. The user confirmed direct A and B input works in Gamepad Mode.
The launcher accepts trackpad clicks; Tab/Up/Down and Enter also navigate it.
For a Steam Machine, use a gamepad layout that sends gamepad buttons rather
than desktop keyboard shortcuts. Steam Machine hardware testing is pending.

If an unexpected problem returns, double-click
**Diagnose-Wario-Land-4.desktop**. It opens the game with a fresh disposable
save and settings in diagnostics/run-*, records the exit status and recent SDL
events, then selects one diagnostics/wl4-deck-diagnostic-*.log file in Dolphin.
Send that one log to the developer and note the physical control used. The
diagnostic launcher never touches userdata/ progress and needs no Konsole.

## Focused Deck acceptance checks for this preview

Use this newly extracted folder so previous test saves and settings stay
untouched. With Gamepad Mode selected, perform these checks:

1. Start a new file, reach a save point, quit with Escape, reopen the launcher
   and game, then verify the same file and progress load. Repeat after a normal
   game exit. Keep userdata/ if you want to retain this test progress.
2. In Settings > Display, try Original, Lite and Enhanced CRT presets at 3x
   scale or larger. Check a moving scene. Enhanced's phosphor mask needs 3x;
   scanlines are suppressed at 1x. Toggle VSync and restart the game to check
   presentation with both settings.
3. Press F11 during gameplay to enter and leave fullscreen. Also enable
   Start fullscreen in Settings, save, launch, and verify the game starts
   fullscreen. The settings window should fit the Deck display and expose
   its Save button without moving the window off-screen.
4. In Settings > Controller, enable rumble and choose a comfortable strength.
   Try Test Rumble, then check a gameplay event that normally rumbles. Report
   if one works and the other does not. Rumble defaults off.
5. Check A/B, Start, directional controls, audio, and a return to the launcher
   after quitting the game. No Steam Machine result is required yet.

## Optional terminal launch

Copy the .tar.gz package and its .sha256 file to the Deck's Downloads folder,
then open Konsole. Use your actual ROM location in the final command:

    cd ~/Downloads
    sha256sum -c wl4-linux-private-20261022-launcher-shortcut-fix.tar.gz.sha256
    mkdir -p ~/Games/wl4
    tar -xzf wl4-linux-private-20261022-launcher-shortcut-fix.tar.gz -C ~/Games/wl4
    cd ~/Games/wl4/wl4-linux-private-20261022-launcher-shortcut-fix
    ./play.sh --check
    ./play.sh "/absolute/path/to/your/baserom_us.gba"

Keep the extracted folder writable. The game creates userdata/wl4.sav and
userdata/settings.cfg there. These are your test save and settings; keep the
folder if you want to keep progress. The package never uses your Windows save or
settings. Replacing the extracted folder with another build can replace those
files, so copy userdata/ somewhere safe before updating.

Default keyboard: arrows to move; X/Z for A/B; A/S for L/R; Enter to start;
Backspace to select; Escape to exit and save; F11 fullscreen; Tab fast-forward.
The Deck controls work in Desktop Mode with Gamepad Mode selected.
Escape remains an intentional keyboard quit command.

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
The native launcher and game were tested after extraction under WSL2/WSLg.
The user confirmed standalone Deck A/B input with Gamepad Mode in the earlier
package. This corrected shortcut needs physical Deck acceptance, especially
save/load, CRT, fullscreen and rumble; Steam Machine testing is later.
Some host driver combinations may need a build against Valve's Steam Linux
Runtime. This is a private test package, not a general Linux release.

The game needs glibc 2.38 or newer. Check ldd --version on the Deck if the
loader reports a missing GLIBC version. The SteamOS library check for this
newly packaged archive remains to be confirmed on Deck hardware. WSLg
synthetic X11 keys sometimes need the window raised and focused more than
once; this is a WSLg test issue.

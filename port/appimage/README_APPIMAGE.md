# Wario Land 4 PC Port — Linux v0.1.0-beta

This is a native Linux x86-64 AppImage with the SDL3 game and graphical launcher.
It needs a user-supplied, unmodified US Wario Land 4 ROM (8 MiB, game code AWAE,
MD5 `5fe47355a33e3fabec2a1607af88a404`). No ROM is included. It does not
use Wine or Proton.

Requires a 64-bit Linux desktop with glibc **2.38 or newer**, working graphics
drivers, and a window system supported by SDL3. SteamOS glibc 2.41 meets the
symbol requirement. SDL3 is bundled; the OS provides glibc, the ELF loader,
graphics, controller and audio services. The v0.1.0-beta AppImage was accepted on real Steam Deck hardware: the
tester entered the boss doors, defeated all bosses and Golden Diva,
and watched the full ending and credits complete without crashes.

## Start in Desktop Mode

Download the AppImage to a writable folder. In Dolphin, right-click it, open
Properties > Permissions, select **Is executable**, and double-click it. The
graphical launcher opens. Click Browse, choose your own supported US ROM, then
Launch Game. The launcher remembers the ROM path and opens its existing Settings
window for bindings, scale, fullscreen, VSync, CRT presets and rumble.

For Steam Deck direct Desktop Mode play, set the Deck's Desktop controller layout
to **Gamepad Mode**. The default Desktop Layout maps physical B to Escape;
Escape intentionally quits the game. For Gaming Mode, add the AppImage in Steam
as a **Non-Steam Game**. In Steam's Properties, leave the compatibility tool
unchecked; launch the native AppImage directly.

The AppImage runtime normally mounts through FUSE. If a host cannot use FUSE,
use its built-in `--appimage-extract-and-run` option. This fallback needs no
SteamOS configuration change but may take longer to start. Another non-destructive
fallback is `--appimage-extract`, then run the extracted `squashfs-root/AppRun`.
Both paths use the same persistent saves and settings below.

## Persistent data

SRAM saves and the remembered ROM path are in
`~/.local/share/biorhythmicstudios/warioland4/` (`wl4.sav` is the unchanged
32 KiB save format). Settings are in
`~/.config/biorhythmicstudios/warioland4/settings.cfg`.
`XDG_DATA_HOME` and `XDG_CONFIG_HOME` override the respective base folders.
Moving, renaming or updating the AppImage leaves these files in place. The
AppImage does not write to its read-only mount or extraction directory.

Existing extracted private packages keep their own `userdata/`; this AppImage
does **not** move or overwrite it. To import deliberately, first close the game,
back up both folders, then copy `userdata/wl4.sav` and
`userdata/launcher_rom.txt` into the AppImage data folder and copy
`userdata/settings.cfg` into the AppImage settings folder. In Dolphin, press
Ctrl+L to enter either destination path. Confirm any overwrite prompt only if
you intend to replace the AppImage version's progress or preferences.

If the launcher does not open, verify executable permission and try the
extract-and-run fallback. If it cannot find a ROM, select it again with Browse.
If audio is missing, check that SteamOS itself has a default output device.
The application uses the host's GPU and audio stack; WSLg testing does not
substitute for Deck or Steam Machine hardware testing.

Known limits: only the unmodified US ROM is supported; Steam Machine hardware
testing is pending. The game targets approximately 59.73 Hz and retains its
existing SDL3 graphics, controller, CRT, VSync and rumble behavior.

Third-party notices and dependency metadata are inside the AppImage at
`usr/share/doc/wl4/`. The bundled SDL3 is version 3.4.2. The AppImage runtime
is the official type-2 runtime release `20251108`.

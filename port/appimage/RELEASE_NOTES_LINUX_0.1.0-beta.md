# Wario Land 4 PC Port — Linux v0.1.0-beta

The first native Linux x86-64 beta runs Wario Land 4 gameplay through SDL3,
with a graphical launcher for selecting a separately supplied US ROM and
editing settings. It supports keyboard and controllers, audio, fullscreen,
VSync, Original/CRT Lite/CRT Enhanced display modes, and optional controller
rumble. Gameplay pacing remains approximately 59.73 Hz.

This build includes the Toy Block Tower post-Keyzer red-pipe fix, the Golden
Diva ending/credits hang fix, and the passage boss-door transition fix.
On a real Steam Deck, the tester entered the boss doors, defeated every
boss and Golden Diva, and watched the complete ending and credits without
crashes. Steam Machine hardware testing is still pending.

Saves and settings persist in XDG user directories across AppImage updates.
Existing extracted-package saves are not migrated automatically. A supported,
unmodified US Wario Land 4 ROM is required and is not included. The AppImage
bundles SDL3 but uses the host's glibc and graphics/audio services; glibc 2.38
or newer is required. No Wine or Proton is needed.

#ifndef GUARD_PORT_SDL_FRONTEND_H
#define GUARD_PORT_SDL_FRONTEND_H

// Runs the game in a window (port/platform/sdl/sdl_frontend.c) until it is closed. The window
// starts at scale (or the saved scale when zero) times the GBA's 240x160. Returns nonzero if SDL could not start.
//
// Keys (mGBA's defaults): arrows, X = A, Z = B, A = L, S = R, Enter = Start, Backspace =
// Select. Bindings and startup fullscreen are loaded from the shared settings file.
// Tab fast-forwards while held, F11 toggles fullscreen, Escape quits. The first
// gamepad works too, by button position (bottom = A, right = B). With scripted set, the game
// also gets the keys of the input script that has been loaded (port/include/port/input_script.h).
// SRAM is kept in savePath, or with NULL in wl4.sav in SDL's preference directory for the
// user (on Windows %APPDATA%\wl4).
int PortSdlRun(void (*entry)(void), unsigned int scale, int scripted, const char *savePath);

#endif  // GUARD_PORT_SDL_FRONTEND_H

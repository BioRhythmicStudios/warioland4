#ifndef GUARD_PORT_SDL_FRONTEND_H
#define GUARD_PORT_SDL_FRONTEND_H

// Runs the game in a window (port/platform/sdl/sdl_frontend.c) until it is closed. The window
// starts at scale times the GBA's 240x160. Returns nonzero if SDL could not start.
//
// Keys (mGBA's defaults): arrows, X = A, Z = B, A = L, S = R, Enter = Start, Backspace =
// Select. Tab fast-forwards while held, F11 toggles fullscreen, Escape quits. The first
// gamepad works too, by button position (bottom = A, right = B).
int PortSdlRun(void (*entry)(void), unsigned int scale);

#endif  // GUARD_PORT_SDL_FRONTEND_H

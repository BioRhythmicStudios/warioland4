#ifndef GUARD_PORT_GAME_THREAD_H
#define GUARD_PORT_GAME_THREAD_H

// The game runs as a coroutine on its own stack at PORT_GAME_STACK_LIMIT..PORT_GAME_STACK_TOP.
// The host resumes it once per frame; the game yields where the GBA would wait for VBlank.

// Creates the coroutine; entry runs on the first resume. Returns nonzero on failure.
int PortGameThreadCreate(void (*entry)(void));

// Runs the game until it yields. Returns 0 once the entry function has returned.
int PortGameThreadResume(void);

// Called by game code: returns to the host until the next resume.
void PortGameThreadYield(void);

// Nonzero when called from game code.
int PortOnGameThread(void);

#endif  // GUARD_PORT_GAME_THREAD_H

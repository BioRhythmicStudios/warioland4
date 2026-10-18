#ifndef GUARD_PORT_SAVE_H
#define GUARD_PORT_SAVE_H

// The cartridge's battery-backed SRAM, kept in a file: its 32 KiB as they are, the format of
// mGBA's .sav files too.

// Loads the file into SRAM before the game starts; with no file yet, SRAM stays erased. The
// file is then written whenever the game has saved. Returns 0 on success, or -1 if the file
// exists but cannot be read, so that a save is never overwritten by mistake.
int PortSaveOpen(const char *path);

// Once a frame: writes the file when SRAM has changed and then stayed the same for half a
// second, so a save the game writes over several frames goes to the file whole.
void PortSaveFrame(void);

// Writes the file if SRAM has changed since, and stops.
void PortSaveClose(void);

#endif  // GUARD_PORT_SAVE_H

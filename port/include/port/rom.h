#ifndef GUARD_PORT_ROM_H
#define GUARD_PORT_ROM_H

// The MD5 of the one ROM the port supports: Wario Land 4, game code AWAE (the US release).
#define PORT_ROM_MD5 "5fe47355a33e3fabec2a1607af88a404"

// Reads the user's ROM, checks it, copies it to GBA_ROM_BASE and makes that read-only.
// GBA memory must be reserved first. Prints the reason and returns nonzero on failure.
int PortLoadRom(const char *path);

#endif  // GUARD_PORT_ROM_H

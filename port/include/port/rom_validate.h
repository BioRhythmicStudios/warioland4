#ifndef PORT_ROM_VALIDATE_H
#define PORT_ROM_VALIDATE_H
#include <stddef.h>
// Checks the same unmodified US release for both the game and launcher.
int PortValidateRom(const void *data, size_t size, char *error, size_t capacity);
#endif

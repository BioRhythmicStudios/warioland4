#include "port/rom_validate.h"
#include "port/rom.h"
#include "port/gba_memory.h"
#include "port/md5.h"
#include <stdio.h>
#include <string.h>

int PortValidateRom(const void *data, size_t size, char *error, size_t capacity)
{
    unsigned char digest[16];
    char hex[33];
    int i;
    if (size != GBA_ROM_SIZE) {
        snprintf(error, capacity, "The ROM must be exactly 8 MiB (8388608 bytes).");
        return -1;
    }
    PortMd5(data, size, digest);
    for (i = 0; i < 16; ++i)
        sprintf(hex + i * 2, "%02x", digest[i]);
    if (strcmp(hex, PORT_ROM_MD5)) {
        snprintf(error, capacity,
                 "Unsupported ROM. Select an unmodified Wario Land 4 US ROM (AWAE).\n"
                 "MD5: %s\nExpected: %s", hex, PORT_ROM_MD5);
        return -1;
    }
    return 0;
}

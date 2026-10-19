#include "port/rom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port/gba_memory.h"
#include "port/rom_validate.h"

int PortLoadRom(const char *path)
{
    FILE *f = fopen(path, "rb");
    unsigned char *rom;
    long size;
    char error[256];

    if (f == NULL) {
        fprintf(stderr, "Cannot open the ROM %s\n", path);
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size != GBA_ROM_SIZE) {
        fprintf(stderr, "%s is %ld bytes; the Wario Land 4 ROM is %u bytes\n", path, size, GBA_ROM_SIZE);
        fclose(f);
        return -1;
    }
    // Read straight into the GBA ROM region, which is writable until PortProtectRom().
    rom = (unsigned char *)(uintptr_t)GBA_ROM_BASE;
    if (fread(rom, 1, GBA_ROM_SIZE, f) != GBA_ROM_SIZE) {
        fprintf(stderr, "Cannot read the ROM %s\n", path);
        fclose(f);
        return -1;
    }
    fclose(f);

    if (PortValidateRom(rom, GBA_ROM_SIZE, error, sizeof(error))) {
        fprintf(stderr, "%s: %s\n", path, error);
        memset(rom, 0, GBA_ROM_SIZE);
        return -1;
    }
    return PortProtectRom();
}

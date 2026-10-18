#include "port/save.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "port/gba_memory.h"
#include "port/os.h"

#define SAVE_SIZE 0x8000u  // Wario Land 4's SRAM
#define QUIET_FRAMES 30

#define SRAM ((uint8_t *)(uintptr_t)GBA_SRAM_BASE)

static char sPath[1024];
static int sOpen;
static uint8_t sWritten[SAVE_SIZE];  // what the file holds
static uint8_t sSeen[SAVE_SIZE];     // SRAM as it was last frame
static unsigned int sQuietFrames;

static void Write(void)
{
    if (memcmp(sSeen, sWritten, SAVE_SIZE) == 0)
        return;
    if (PortOsWriteFileAtomic(sPath, sSeen, SAVE_SIZE)) {
        fprintf(stderr, "Cannot write the save file %s\n", sPath);
        return;
    }
    memcpy(sWritten, sSeen, SAVE_SIZE);
}

int PortSaveOpen(const char *path)
{
    int missing;
    long n;

    if (strlen(path) >= sizeof(sPath)) {
        fprintf(stderr, "The save file's path is too long: %s\n", path);
        return -1;
    }
    n = PortOsReadFile(path, SRAM, SAVE_SIZE, &missing);
    if (n < 0 && !missing) {
        char message[1200];
        snprintf(message, sizeof(message), "Cannot read the save file %s\n", path);
        PortOsWriteError(message);
        return -1;
    }
    if (n >= 0 && n != (long)SAVE_SIZE)
        fprintf(stderr, "The save file %s has %ld bytes, not %u: the rest of SRAM stays erased\n", path, n,
                SAVE_SIZE);
    strcpy(sPath, path);
    memcpy(sWritten, SRAM, SAVE_SIZE);
    memcpy(sSeen, SRAM, SAVE_SIZE);
    sQuietFrames = QUIET_FRAMES;
    sOpen = 1;
    printf("Save file: %s%s\n", path, missing ? " (none yet)" : "");
    return 0;
}

void PortSaveFrame(void)
{
    if (!sOpen)
        return;
    if (memcmp(SRAM, sSeen, SAVE_SIZE) != 0) {
        memcpy(sSeen, SRAM, SAVE_SIZE);
        sQuietFrames = 0;
    } else if (sQuietFrames < QUIET_FRAMES && ++sQuietFrames == QUIET_FRAMES) {
        Write();
    }
}

void PortSaveClose(void)
{
    if (!sOpen)
        return;
    memcpy(sSeen, SRAM, SAVE_SIZE);
    Write();
    sOpen = 0;
}

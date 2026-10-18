#include "port/save.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "port/gba_memory.h"
#include "port/os.h"

#define SAVE_SIZE 0x8000u
#define QUIET_FRAMES 30
#define RETRY_FRAMES 300  // at most one automatic attempt per five seconds of game frames
#define SRAM ((uint8_t *)(uintptr_t)GBA_SRAM_BASE)

static char sPath[1024];
static char sError[1792];
static int sOpen;
static uint8_t sWritten[SAVE_SIZE];  // last successfully persisted SRAM
static uint8_t sSeen[SAVE_SIZE];     // latest SRAM, including changes made during a failure
static unsigned int sQuietFrames, sRetryFrames;

const char *PortSaveError(void)
{
    return sError[0] ? sError : NULL;
}

const char *PortSavePath(void)
{
    return sPath;
}

static void Recovered(void)
{
    if (sError[0])
        fprintf(stderr, "Save recovered: %s\n", sPath);
    sError[0] = 0;
    sRetryFrames = 0;
}

static int Write(void)
{
    char reason[384];
    int firstFailure = !sError[0];

    if (memcmp(sSeen, sWritten, SAVE_SIZE) == 0) {
        Recovered();
        return 0;
    }
    if (PortOsWriteFileAtomic(sPath, sSeen, SAVE_SIZE, reason, sizeof(reason))) {
        snprintf(sError, sizeof(sError),
                 "Cannot save progress to:\n%s\n\n%s\n\n"
                 "Progress is still in memory, but has not been saved to disk.\n"
                 "Check the location and permissions. Automatic retries continue while playing.",
                 sPath, reason);
        if (firstFailure)
            fprintf(stderr, "%s\n", sError);
        sRetryFrames = RETRY_FRAMES;
        return -1;
    }
    memcpy(sWritten, sSeen, SAVE_SIZE);
    Recovered();
    return 0;
}

int PortSaveOpen(const char *path)
{
    // Read into a separate buffer so a bad/partial read cannot replace pending SRAM.
    uint8_t loaded[SAVE_SIZE + 1];
    char message[1280];
    int missing;
    long n;

    if (sOpen) {
        PortOsWriteError("A save is already open. Save pending progress before opening another file.\n");
        return -1;
    }
    if (strlen(path) >= sizeof(sPath)) {
        PortOsWriteError("Cannot open the save: its path is too long. Choose a shorter save path.\n");
        return -1;
    }
    n = PortOsReadFile(path, loaded, sizeof(loaded), &missing);
    if (n < 0 && !missing) {
        snprintf(message, sizeof(message), "Cannot read the save file:\n%s\nThe file will not be overwritten.\n", path);
        PortOsWriteError(message);
        return -1;
    }
    if (n >= 0 && n != (long)SAVE_SIZE) {
        snprintf(message, sizeof(message),
                 "Cannot load the save file:\n%s\nExpected exactly 32768 bytes; read %ld%s.\n"
                 "The file will not be overwritten. Back it up before repairing it.\n",
                 path, n, n > SAVE_SIZE ? " or more" : "");
        PortOsWriteError(message);
        return -1;
    }
    if (missing)
        memset(SRAM, 0xFF, SAVE_SIZE);
    else
        memcpy(SRAM, loaded, SAVE_SIZE);
    strcpy(sPath, path);
    memcpy(sWritten, SRAM, SAVE_SIZE);
    memcpy(sSeen, SRAM, SAVE_SIZE);
    sQuietFrames = QUIET_FRAMES;
    sRetryFrames = 0;
    sError[0] = 0;
    sOpen = 1;
    printf("Save file: %s%s\n", path, missing ? " (none yet)" : "");
    return 0;
}

void PortSaveFrame(void)
{
    if (!sOpen)
        return;
    if (sRetryFrames)
        sRetryFrames--;
    if (memcmp(SRAM, sSeen, SAVE_SIZE) != 0) {
        memcpy(sSeen, SRAM, SAVE_SIZE);
        sQuietFrames = 0;
    } else if (sQuietFrames < QUIET_FRAMES) {
        sQuietFrames++;
    }
    if (memcmp(sSeen, sWritten, SAVE_SIZE) == 0) {
        if (sError[0])
            Recovered();
    } else if (sQuietFrames >= QUIET_FRAMES && !sRetryFrames) {
        Write();
    }
}

int PortSaveClose(void)
{
    if (!sOpen)
        return 0;
    memcpy(sSeen, SRAM, SAVE_SIZE);
    if (Write())
        return -1;  // keep the session and latest SRAM for retry or return to gameplay
    sOpen = 0;
    return 0;
}

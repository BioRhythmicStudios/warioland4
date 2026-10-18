// Real filesystem failures, always in a newly created disposable child of --test-save DIR.
#include "port/save.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "port/gba_memory.h"
#include "port/os.h"

#define SAVE_SIZE 0x8000u
#define SRAM ((uint8_t *)(uintptr_t)GBA_SRAM_BASE)
static int sFailures;
static uint8_t sExpected[SAVE_SIZE + 1];

static void Check(int ok, const char *what)
{
    if (!ok) {
        fprintf(stderr, "FAIL: %s\n", what);
        sFailures++;
    }
}

static const wchar_t *Wide(const char *path)
{
    static wchar_t buffer[2048];
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, buffer, 2048))
        return L"";
    return buffer;
}

static void Pattern(unsigned int seed)
{
    unsigned int i;
    for (i = 0; i < SAVE_SIZE; i++)
        sExpected[i] = (uint8_t)((seed + i * 17) ^ (i >> 8));
}

static void Change(unsigned int seed)
{
    Pattern(seed);
    memcpy(SRAM, sExpected, SAVE_SIZE);
}

static int Matches(const char *path, unsigned int seed)
{
    static uint8_t loaded[SAVE_SIZE + 1];
    int missing;
    long n = PortOsReadFile(path, loaded, sizeof(loaded), &missing);
    Pattern(seed);
    return n == SAVE_SIZE && memcmp(loaded, sExpected, SAVE_SIZE) == 0;
}

static void Seed(const char *path, unsigned int seed, unsigned int size)
{
    char reason[384];
    Pattern(seed);
    Check(PortOsWriteFileAtomic(path, sExpected, size, reason, sizeof(reason)) == 0,
          "create disposable seed file");
}

static void Frames(unsigned int count)
{
    while (count--)
        PortSaveFrame();
}

int PortSaveTest(const char *directory)
{
    char root[768], path[1024], pending[1024], closeOnly[1024], shortSave[1024], longSave[1024], legacy[1100];
    HANDLE lock;
    int missing;
    char marker[4];
    sFailures = 0;
    if (snprintf(root, sizeof(root), "%s/wl4-save-test-%lu-%llu", directory,
                 (unsigned long)GetCurrentProcessId(), (unsigned long long)GetTickCount64()) >= (int)sizeof(root)
        || !CreateDirectoryW(Wide(root), NULL)) {
        fprintf(stderr, "Cannot create a NEW disposable save test directory under %s\n", directory);
        return 1;
    }
    printf("Disposable save test directory: %s\n", root);
    snprintf(path, sizeof(path), "%s/progress.sav", root);
    snprintf(legacy, sizeof(legacy), "%s.tmp", path);
    snprintf(pending, sizeof(pending), "%s/later/progress.sav", root);
    snprintf(closeOnly, sizeof(closeOnly), "%s/close-only/progress.sav", root);
    snprintf(shortSave, sizeof(shortSave), "%s/short.sav", root);
    snprintf(longSave, sizeof(longSave), "%s/oversized.sav", root);

    Seed(path, 1, SAVE_SIZE);
    Seed(legacy, 99, 4);  // an existing .tmp must not be truncated, renamed or deleted
    Check(PortSaveOpen(path) == 0, "load known-good 32 KiB save");
    Pattern(1);
    Check(memcmp(SRAM, sExpected, SAVE_SIZE) == 0, "loaded SRAM equals disk");
    Change(2);
    Frames(30);
    Check(Matches(path, 1), "do not persist before 30 stable frames");
    Frames(1);
    Check(Matches(path, 2) && !PortSaveError(), "ordinary settled save succeeds");
    Check(PortSaveClose() == 0 && PortSaveOpen(path) == 0, "close and reload successful save");
    Pattern(2);
    Check(memcmp(SRAM, sExpected, SAVE_SIZE) == 0, "reload reads saved bytes");

    // Permit reads but deny replacement. The write and flush succeed; rename must fail.
    lock = CreateFileW(Wide(path), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    Check(lock != INVALID_HANDLE_VALUE, "lock only the disposable destination");
    Change(3);
    Frames(31);
    Check(PortSaveError() && strstr(PortSaveError(), path) && strstr(PortSaveError(), "Windows error"),
          "failure status includes destination and Windows reason");
    Check(Matches(path, 2), "failed atomic replacement preserves every old byte");
    Change(4);
    Frames(31);
    Check(Matches(path, 2), "new SRAM during failure does not overwrite the old file");
    Frames(600);
    Check(Matches(path, 2) && PortSaveError(), "repeated automatic failures retain old file and warning");
    Check(PortSaveClose() != 0, "failed close returns failure, not success");
    Check(PortSaveOpen(pending) != 0, "cannot replace an open unsaved session with another save");
    Pattern(4);
    Check(memcmp(SRAM, sExpected, SAVE_SIZE) == 0, "failed close/open preserve newest SRAM");
    if (lock != INVALID_HANDLE_VALUE)
        CloseHandle(lock);
    Frames(299);
    Check(Matches(path, 2) && PortSaveError(), "retry cooldown survives unchanged SRAM and failed close");
    Frames(1);
    Check(Matches(path, 4) && !PortSaveError(), "bounded retry persists latest SRAM and clears warning");
    Check(PortSaveClose() == 0 && PortSaveOpen(path) == 0, "recovery closes and reloads normally");
    Pattern(4);
    Check(memcmp(SRAM, sExpected, SAVE_SIZE) == 0, "reloaded recovered save is newest complete image");
    Check(PortSaveClose() == 0, "close recovered save");
    Pattern(99);
    Check(PortOsReadFile(legacy, marker, sizeof(marker), &missing) == 4
          && memcmp(marker, sExpected, 4) == 0, "legacy temporary file remains untouched");

    Check(PortSaveOpen(pending) == 0, "missing destination starts a new save session");
    Change(5);
    Frames(31);
    Check(PortSaveError() != NULL, "temporary creation failure is reported");
    {
        char parent[1024];
        snprintf(parent, sizeof(parent), "%s/later", root);
        Check(CreateDirectoryW(Wide(parent), NULL) != 0, "restore missing disposable destination directory");
    }
    Frames(299);
    Check(!Matches(pending, 5), "no retry before cooldown expires");
    Frames(1);
    Check(Matches(pending, 5) && !PortSaveError(), "retry recovers without another SRAM change");
    Check(PortSaveClose() == 0, "close new recovered save");

    Check(PortSaveOpen(closeOnly) == 0, "open disposable close-failure session");
    Change(6);
    Check(PortSaveClose() != 0 && PortSaveError(), "close-only failure has a visible-status message");
    {
        char parent[1024];
        snprintf(parent, sizeof(parent), "%s/close-only", root);
        Check(CreateDirectoryW(Wide(parent), NULL) != 0, "restore close-only destination");
    }
    Check(PortSaveClose() == 0 && Matches(closeOnly, 6), "explicit close retry saves retained SRAM");
    Check(PortSaveOpen(closeOnly) == 0, "reload after explicit retry");
    Pattern(6);
    Check(memcmp(SRAM, sExpected, SAVE_SIZE) == 0, "close retry survives reload");
    Check(PortSaveClose() == 0, "close reloaded file");

    Seed(shortSave, 7, 1);
    Seed(longSave, 8, SAVE_SIZE + 1);
    Check(PortSaveOpen(shortSave) != 0 && PortSaveOpen(longSave) != 0,
          "reject truncated and oversized saves without opening writable sessions");
    Pattern(6);
    Check(memcmp(SRAM, sExpected, SAVE_SIZE) == 0, "rejected saves do not modify SRAM");
    Check(PortSaveClose() == 0, "no writable session after rejected load");
    {
        static uint8_t data[SAVE_SIZE + 2];
        Check(PortOsReadFile(shortSave, data, sizeof(data), &missing) == 1
              && PortOsReadFile(longSave, data, sizeof(data), &missing) == SAVE_SIZE + 1,
              "rejected files retain their original lengths");
    }
    lock = CreateFileW(Wide(path), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    Check(lock != INVALID_HANDLE_VALUE, "deny read access to disposable input only");
    Check(PortSaveOpen(path) != 0, "unreadable existing save cannot open a writable session");
    if (lock != INVALID_HANDLE_VALUE)
        CloseHandle(lock);
    {
        char tooLong[1100];
        memset(tooLong, 'x', sizeof(tooLong) - 1);
        tooLong[sizeof(tooLong) - 1] = 0;
        Check(PortSaveOpen(tooLong) != 0, "oversized save path fails without filesystem changes");
    }
    Pattern(6);
    Check(memcmp(SRAM, sExpected, SAVE_SIZE) == 0 && Matches(path, 4),
          "input failures preserve in-memory SRAM and known-good disk save");
    printf(sFailures ? "Save tests: %d failed\n" : "Save tests passed\n", sFailures);
    return sFailures != 0;
}

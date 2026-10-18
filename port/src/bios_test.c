// Unit tests for port/src/bios.c, run with wl4 --test-bios.
#include "port/bios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port/game_thread.h"

static int sFailures;

static void Check(int ok, const char *what)
{
    if (!ok) {
        printf("FAIL: %s\n", what);
        sFailures++;
    }
}

static void TestCpuSet(void)
{
    uint16_t src16[6] = { 1, 2, 3, 4, 5, 6 };
    uint16_t dst16[6] = { 0 };
    uint32_t src32[4] = { 0x11111111, 0x22222222, 0x33333333, 0x44444444 };
    uint32_t dst32[5] = { 0 };
    uint32_t fill = 0xA5A5A5A5;

    CPUSet(src16, dst16, 5);
    Check(memcmp(dst16, src16, 10) == 0 && dst16[5] == 0, "CPUSet copies 5 halfwords");

    CPUSet(&fill, dst32, 0x05000000 | 3);
    Check(dst32[0] == fill && dst32[2] == fill && dst32[3] == 0, "CPUSet fills 3 words");

    memset(dst32, 0, sizeof(dst32));
    CPUSet(src32, (uint8_t *)&dst32[1] + 2, 0x04000000 | 2);
    Check(dst32[1] == src32[0] && dst32[2] == src32[1] && dst32[3] == 0, "CPUSet aligns word addresses down");

    memset(dst16, 0, sizeof(dst16));
    CPUSet(src16, (uint8_t *)dst16 + 1, 1);
    Check(dst16[0] == 1 && dst16[1] == 0, "CPUSet aligns halfword addresses down");

    memset(dst16, 0, sizeof(dst16));
    CPUSet(src16, dst16, 0);
    Check(dst16[0] == 0, "CPUSet with a count of 0 does nothing");

    CPUSet((const void *)(uintptr_t)0x100, dst16, 1);
    Check(dst16[0] == 0, "CPUSet refuses to read the BIOS ROM");
}

unsigned int umul3232H32(unsigned int multiplier, unsigned int multiplicand);

static void TestRuntimeHelpers(void)
{
    Check(umul3232H32(0x80000000u, 4) == 2 && umul3232H32(0xFFFFFFFFu, 0xFFFFFFFFu) == 0xFFFFFFFEu,
          "umul3232H32 returns the high word");
}

// A plain byte-at-a-time LZ77 decoder, independent of the BIOS version. Returns the size,
// or 0 if a back-reference points before the start or the stream ends early.
static uint32_t DecodeLz77Bytes(const uint8_t *s, const uint8_t *end, uint8_t *out)
{
    uint32_t size = s[1] | (uint32_t)s[2] << 8 | (uint32_t)s[3] << 16;
    uint32_t pos = 0;

    s += 4;
    while (pos < size) {
        unsigned int flags, bit;
        if (s >= end)
            return 0;
        flags = *s++;
        for (bit = 0x80; bit != 0 && pos < size; bit >>= 1) {
            if (flags & bit) {
                uint32_t distance, length;
                if (s + 2 > end)
                    return 0;
                distance = (((uint32_t)s[0] & 0xF) << 8 | s[1]) + 1;
                length = (s[0] >> 4) + 3;
                s += 2;
                if (distance > pos)
                    return 0;
                while (length-- && pos < size) {
                    out[pos] = out[pos - distance];
                    pos++;
                }
            } else {
                if (s >= end)
                    return 0;
                out[pos++] = *s++;
            }
        }
    }
    return size;
}

static void TestLz77Streams(void)
{
    // "ABAB" + "ABABAB": four literals, then a 6-byte back-reference at distance 4.
    static const uint32_t stream1[] = { 0x00000A10, 0x41424108, 0x00033042 };
    // Literal 'A', then a back-reference of 3 at distance 1: the VRAM quirk.
    static const uint32_t stream2[] = { 0x00000410, 0x00004140, 0x00000000 };
    uint16_t out[8];
    const uint8_t *bytes = (const uint8_t *)out;

    memset(out, 0xEE, sizeof(out));
    LZ77UnCompVram(stream1, out);
    Check(memcmp(out, "ABABABABAB", 10) == 0 && bytes[10] == 0xEE, "LZ77UnCompVram decodes literals and a copy");

    // On the hardware the 'A' is still waiting in a register when the copy reads it back.
    memset(out, 0xEE, sizeof(out));
    LZ77UnCompVram(stream2, out);
    Check(bytes[0] == 'A' && bytes[1] == 0xEE && bytes[2] == 0xEE && bytes[3] == 0xEE,
          "LZ77UnCompVram reads stale data at distance 1");

    memset(out, 0xEE, sizeof(out));
    LZ77UnCompVram((const uint32_t *)(uintptr_t)0x100, out);
    Check(bytes[0] == 0xEE, "LZ77UnCompVram refuses to read the BIOS ROM");
}

// Every stream the game decompresses: the BIOS version must agree with the byte decoder.
static void TestGameLz77Streams(void)
{
    const uint8_t *romEnd = (const uint8_t *)(uintptr_t)0x08800000;
    unsigned int i, checked = 0;
    uint64_t total = 0;

    for (i = 0; i < gLz77SourceCount; i++) {
        const uint8_t *s = (const uint8_t *)(uintptr_t)gLz77Sources[i].gbaAddress;
        uint32_t size = s[1] | (uint32_t)s[2] << 8 | (uint32_t)s[3] << 16;
        uint8_t *vram = calloc(size + 64, 1);
        uint8_t *bytes = calloc(size + 64, 1);
        char what[128];

        if (vram == NULL || bytes == NULL)
            abort();
        snprintf(what, sizeof(what), "%s (0x%08X) is an LZ77 stream", gLz77Sources[i].name,
                 gLz77Sources[i].gbaAddress);
        Check(s[0] == 0x10 && size > 0 && DecodeLz77Bytes(s, romEnd, bytes) == size, what);
        LZ77UnCompVram((const uint32_t *)s, vram);
        snprintf(what, sizeof(what), "LZ77UnCompVram matches the byte decoder on %s", gLz77Sources[i].name);
        // The halfword writer leaves a final odd byte unwritten, as on the hardware.
        Check(memcmp(vram, bytes, size & ~1u) == 0, what);
        free(vram);
        free(bytes);
        checked++;
        total += size;
    }
    printf("LZ77: %u game streams, %llu bytes decompressed\n", checked, (unsigned long long)total);
}

static int sHaltsDone;

static void HaltThreeTimes(void)
{
    int i;

    for (i = 0; i < 3; i++) {
        PlatformSyscall(2);  // SYSCALL_Halt
        sHaltsDone++;
    }
}

// Halt returns to the host, and the next resume continues after it.
static void TestHalt(void)
{
    int resumes = 0;

    sHaltsDone = 0;
    if (PortGameThreadCreate(HaltThreeTimes)) {
        Check(0, "the game coroutine starts");
        return;
    }
    while (PortGameThreadResume()) {
        Check(sHaltsDone == resumes, "Halt yields once per call");
        resumes++;
    }
    Check(resumes == 3 && sHaltsDone == 3, "three halts take three resumes");
}

int PortBiosTest(void)
{
    sFailures = 0;
    TestHalt();
    TestCpuSet();
    TestRuntimeHelpers();
    TestLz77Streams();
    TestGameLz77Streams();
    printf(sFailures ? "BIOS tests: %d failed\n" : "BIOS tests passed\n", sFailures);
    return sFailures != 0;
}

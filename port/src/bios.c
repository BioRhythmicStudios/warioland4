#include "port/bios.h"

#include <stdio.h>
#include <stdlib.h>

#include "port/frame.h"
#include "port/game_thread.h"

#define SYSCALL_HALT 2

// The BIOS refuses to read its own ROM through these calls: it requires one of the source
// address bits 25-27 to be set. Only the BIOS ROM is refused here, because the full check
// would also refuse the port's game stack at 0x01020000, which stands in for IWRAM's stack.
#define BIOS_ROM_END 0x4000u

static int ReadsBios(const void *src)
{
    return (uintptr_t)src < BIOS_ROM_END;
}

void CPUSet(const void *src, void *dest, uint32_t control)
{
    uint32_t count = control & 0x1FFFFF;
    int fixed = (control & 0x01000000) != 0;

    if (ReadsBios(src))
        return;
    if (control & 0x04000000) {
        const volatile uint32_t *s = (const volatile uint32_t *)((uintptr_t)src & ~(uintptr_t)3);
        volatile uint32_t *d = (volatile uint32_t *)((uintptr_t)dest & ~(uintptr_t)3);
        if (fixed) {
            uint32_t value = *s;
            while (count--)
                *d++ = value;
        } else {
            while (count--)
                *d++ = *s++;
        }
    } else {
        const volatile uint16_t *s = (const volatile uint16_t *)((uintptr_t)src & ~(uintptr_t)1);
        volatile uint16_t *d = (volatile uint16_t *)((uintptr_t)dest & ~(uintptr_t)1);
        if (fixed) {
            uint16_t value = *s;
            while (count--)
                *d++ = value;
        } else {
            while (count--)
                *d++ = *s++;
        }
    }
}

// Collects output bytes into halfwords and stores each one once its high byte arrives.
struct HalfwordWriter {
    uintptr_t next;
    uint16_t pending;
};

static void PutByte(struct HalfwordWriter *w, unsigned int byte)
{
    if (w->next & 1) {
        w->pending |= (uint16_t)(byte << 8);
        *(volatile uint16_t *)(w->next ^ 1) = w->pending;
    } else {
        w->pending = (uint16_t)byte;
    }
    w->next++;
}

void LZ77UnCompVram(const uint32_t *src, void *dest)
{
    const uint8_t *s = (const uint8_t *)src;
    struct HalfwordWriter w = { (uintptr_t)dest, 0 };
    uint32_t remaining;

    if (ReadsBios(src))
        return;
    // The header's low byte is the compression type (0x10); the BIOS does not check it.
    remaining = s[1] | (uint32_t)s[2] << 8 | (uint32_t)s[3] << 16;
    s += 4;
    while (remaining > 0) {
        unsigned int flags = *s++;
        int i;
        for (i = 0; i < 8 && remaining > 0; i++, flags <<= 1) {
            if (flags & 0x80) {
                unsigned int block = (unsigned int)s[0] << 8 | s[1];
                uintptr_t from = w.next - (block & 0xFFF) - 1;
                unsigned int length = (block >> 12) + 3;
                s += 2;
                // The whole back-reference is copied even past the end, as on the hardware.
                while (length--) {
                    uint16_t halfword = *(const volatile uint16_t *)(from & ~(uintptr_t)1);
                    PutByte(&w, (halfword >> ((from & 1) * 8)) & 0xFF);
                    from++;
                    if (remaining)
                        remaining--;
                }
            } else {
                PutByte(&w, *s++);
                remaining--;
            }
        }
    }
}

void PlatformSyscall(int num)
{
    if (num == SYSCALL_HALT && PortOnGameThread()) {
        // Halt sleeps until an enabled interrupt is requested, and the handler runs.
        PortFrameHalt();
        return;
    }
    fprintf(stderr, "BIOS call 0x%02X is not implemented%s\n", num,
            num == SYSCALL_HALT ? " outside the game coroutine" : "");
    fflush(stderr);
    abort();
}

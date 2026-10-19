#ifndef PORT_FRAME_PACER_H
#define PORT_FRAME_PACER_H
#include <stdint.h>

// 228 lines * 1232 cycles at 2^24 Hz. Display refresh never changes this clock.
#define PORT_FRAME_NS (280896ull * 1000000000ull / 16777216ull)
typedef struct PortFramePacer {
    uint64_t next;
    unsigned int resets;
} PortFramePacer;

// Catch up short presentation delays without dropping simulation/audio frames.
// Preserve the old long-stall policy: discard backlog after four frame periods.
static inline unsigned int PortFramePacerDue(PortFramePacer *p, uint64_t now, int fastForward)
{
    if (fastForward) {
        p->next = now + PORT_FRAME_NS;
        return 1;
    }
    if (now > p->next && now - p->next > 4 * PORT_FRAME_NS) {
        p->next = now;
        ++p->resets;
    }
    unsigned int frames = 0;
    while (now >= p->next && frames < 4) {
        ++frames;
        p->next += PORT_FRAME_NS;
    }
    return frames;
}
#endif

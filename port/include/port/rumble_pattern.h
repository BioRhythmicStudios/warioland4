#ifndef PORT_RUMBLE_PATTERN_H
#define PORT_RUMBLE_PATTERN_H
#include "port/controls.h"
enum { PORT_PATTERN_ACTIVATE = 1, PORT_PATTERN_FIRE, PORT_PATTERN_ASH, PORT_PATTERN_SNOW };
typedef struct PortRumbleSequence {
    int pattern, segment;
    SDL_JoystickID pad;
    Uint64 started;
} PortRumbleSequence;
typedef struct PortRumbleSegment { unsigned low, high, start, end; } PortRumbleSegment;
static inline void PortRumbleSequenceCancel(PortRumbleSequence *s, PortControls *c)
{
    if (s->pattern && c->gamepad && s->pad == SDL_GetGamepadID(c->gamepad)) PortControlsStopRumble(c);
    s->pattern = 0;
}
// Fixed host-time envelopes. Missed segments after a stall are skipped, never replayed.
static inline int PortRumbleSequenceUpdate(PortRumbleSequence *s, PortControls *c, Uint64 now)
{
    static const PortRumbleSegment activation[] = {{18000,14000,0,65},{0,0,65,120},{24000,10000,120,200}};
    static const PortRumbleSegment fire[] = {{14000,18000,0,60},{0,0,60,120},{16000,20000,120,180},
        {0,0,180,240},{18000,22000,240,300}};
    static const PortRumbleSegment ash[] = {{12000,24000,0,110},{0,0,110,180},{20000,6000,180,240}};
    static const PortRumbleSegment snow[] = {{18000,8000,0,3000}};
    const PortRumbleSegment *segments;
    unsigned count, i;
    Uint64 elapsed;
    if (!s->pattern) return 0;
    if (!c->gamepad || s->pad != SDL_GetGamepadID(c->gamepad) || !c->settings.rumble || !c->settings.rumbleStrength) {
        PortRumbleSequenceCancel(s, c); return 0;
    }
    switch (s->pattern) {
    case PORT_PATTERN_FIRE: segments = fire; count = SDL_arraysize(fire); break;
    case PORT_PATTERN_ASH: segments = ash; count = SDL_arraysize(ash); break;
    case PORT_PATTERN_SNOW: segments = snow; count = SDL_arraysize(snow); break;
    default: segments = activation; count = SDL_arraysize(activation); break;
    }
    elapsed = now - s->started;
    for (i = 0; i < count && elapsed >= segments[i].end; ++i) {}
    if (i == count) { PortRumbleSequenceCancel(s, c); return 0; }
    if (s->segment != (int)i) {
        s->segment = (int)i;
        if (!PortControlsRumble(c, segments[i].low, segments[i].high, (Uint32)(segments[i].end - elapsed))) {
            s->pattern = 0; return 0;
        }
    }
    return 1;
}
static inline void PortRumbleSequenceStart(PortRumbleSequence *s, PortControls *c, int pattern, Uint64 now)
{
    s->pattern = pattern; s->segment = -1; s->started = now;
    s->pad = c->gamepad ? SDL_GetGamepadID(c->gamepad) : 0;
    PortRumbleSequenceUpdate(s, c, now);
}
#endif

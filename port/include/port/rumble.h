#ifndef PORT_RUMBLE_H
#define PORT_RUMBLE_H
#include "port/controls.h"
#include "port/rumble_snapshot.h"
#include "port/rumble_pattern.h"
typedef struct PortRumbleObserver { int valid, health, shakeX, shakeY; Uint64 next, frogUntil; PortRumbleSequence sequence; } PortRumbleObserver;
// Sample once after each simulated frame. Edges are consumed even when output is suppressed.
static inline void PortRumbleObserve(PortRumbleObserver *o, PortControls *c,
    int active, int health, int shakeX, int shakeY, int rock, int boss, unsigned events, Uint64 now)
{
    if (!active) {
        PortRumbleSequenceCancel(&o->sequence, c);
        if (o->valid) PortControlsStopRumble(c);
        o->valid = 0; o->frogUntil = 0; return;
    }
    int damage = o->valid && health < o->health;
    int impact = o->valid && (shakeX > o->shakeX || shakeY > o->shakeY);
    o->health = health; o->shakeX = shakeX; o->shakeY = shakeY; o->valid = 1;
    if (!c->settings.rumble || !c->settings.rumbleStrength) {
        PortRumbleSequenceCancel(&o->sequence, c);
        if (o->frogUntil) PortControlsStopRumble(c);
        o->frogUntil = 0; return;
    }
    if (events & PORT_RUMBLE_FROG) {
        PortRumbleSequenceCancel(&o->sequence, c);
        if (PortControlsRumble(c, 28000, 12000, 5000)) o->frogUntil = now + 5000;
        return;
    }
    // Preserve the continuous activation effect: incidental shakes cannot shorten it.
    if (now < o->frogUntil) return;
    if ((events & PORT_RUMBLE_SNOW_END) && o->sequence.pattern == PORT_PATTERN_SNOW) {
        PortRumbleSequenceCancel(&o->sequence, c); o->next = now;
    }
    // Transition cues replace damage/shake feedback for the same event and bypass its cooldown.
    if (events & (PORT_RUMBLE_TRANSFORM | PORT_RUMBLE_RECOVER | PORT_RUMBLE_FIRE | PORT_RUMBLE_ASH | PORT_RUMBLE_SNOW)) {
        if (events & PORT_RUMBLE_RECOVER) {
            PortRumbleSequenceCancel(&o->sequence, c);
            PortControlsRumble(c, 9000, 14000, 65);
            o->next = now + 180;
        } else {
            int pattern = events & PORT_RUMBLE_ASH ? PORT_PATTERN_ASH : events & PORT_RUMBLE_FIRE ? PORT_PATTERN_FIRE :
                events & PORT_RUMBLE_SNOW ? PORT_PATTERN_SNOW : PORT_PATTERN_ACTIVATE;
            PortRumbleSequenceStart(&o->sequence, c, pattern, now);
            o->next = now + (pattern == PORT_PATTERN_SNOW ? 3000 : pattern == PORT_PATTERN_FIRE ? 300 :
                pattern == PORT_PATTERN_ASH ? 240 : 200) + 180;
        }
        return;
    }
    if (events & PORT_RUMBLE_FORM_IMPACT) {
        PortRumbleSequenceCancel(&o->sequence, c);
        PortControlsRumble(c, 20000, 12000, 85);
        o->next = now + 180; return;
    }
    if (PortRumbleSequenceUpdate(&o->sequence, c, now)) return;
    events &= ~PORT_RUMBLE_SNOW_END;
    if (now < o->next) return;
    if (damage || impact || rock || boss || events) {
        // Original short two-motor pulses: sharp damage, heavier low-frequency impact.
        PortControlsRumble(c, damage ? 26000 : boss ? 42000 : (events & PORT_RUMBLE_FORM_IMPACT) ? 20000 : (events & PORT_RUMBLE_HEAD) ? 30000 : rock ? 38000 : 44000,
            damage ? 48000 : boss ? 34000 : (events & PORT_RUMBLE_FORM_IMPACT) ? 12000 : events ? 26000 : rock ? 24000 : 16000,
            damage ? 110 : boss ? 140 : events ? 100 : rock ? 85 : 160);
        o->next = now + 180;
    }
}
#endif

#ifndef ANIMATED_GRAPHICS_H
#define ANIMATED_GRAPHICS_H

#include "types.h"

struct AGB_ALIGN AnimatedGraphicsState {
    u8 mode;
    u8 frameDelay;
    u8 frameTimer;
    u8 frameCount;
    s8 frameIndex;
    u8 pad5;
    u8 pad6;
    u8 pad7;
    GBA_PTR(const void*) tiles;
};

struct AGB_ALIGN AnimatedGraphicsDefinition {
    u8 mode;
    u8 frameDelay;
    u8 frameCount;
    u8 pad3;
    GBA_PTR(const void*) tiles;
};

extern struct AnimatedGraphicsState gAnimatedGraphicsStates[];
extern const struct AnimatedGraphicsDefinition sAnimatedGraphicsDefinitions[];
extern const u8 sAnimatedGraphicsSwitchIds[];
extern const u16 sAnimatedGraphicsInactiveIds[];
extern const u16 sAnimatedGraphicsActiveIds[];

void UpdateAnimatedGraphics(void);
void InitAnimatedGraphics(void);
void UpdateAnimatedGraphicsSwitches(void);
void ReloadAnimatedGraphicsForSwitch(u8 switchId);

#endif /* ANIMATED_GRAPHICS_H */

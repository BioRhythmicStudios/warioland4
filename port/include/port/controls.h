#ifndef PORT_CONTROLS_H
#define PORT_CONTROLS_H
#include "port/settings.h"
typedef struct PortControls { PortSettings settings; SDL_Gamepad *gamepad; } PortControls;
void PortControlsOpen(PortControls *controls, const PortSettings *settings);
// Returns false for disabled, missing or unsupported devices; input remains available.
bool PortControlsRumble(PortControls *controls, unsigned low, unsigned high, Uint32 milliseconds);
void PortControlsStopRumble(PortControls *controls);
void PortControlsClose(PortControls *controls);
void PortControlsEvent(PortControls *controls, const SDL_Event *event);
Uint16 PortControlsPressed(PortControls *controls, const bool *keyboard);
#endif

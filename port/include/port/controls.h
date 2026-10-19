#ifndef PORT_CONTROLS_H
#define PORT_CONTROLS_H
#include "port/settings.h"
typedef struct PortControls { PortSettings settings; SDL_Gamepad *gamepad; } PortControls;
void PortControlsOpen(PortControls *controls, const PortSettings *settings);
void PortControlsClose(PortControls *controls);
void PortControlsEvent(PortControls *controls, const SDL_Event *event);
Uint16 PortControlsPressed(PortControls *controls, const bool *keyboard);
#endif

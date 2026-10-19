#include "port/controls.h"

static void OpenFirst(PortControls *c)
{
    if (c->gamepad) return;
    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);
    for (int i = 0; i < count && !c->gamepad; ++i) c->gamepad = SDL_OpenGamepad(ids[i]);
    SDL_free(ids);
}
void PortControlsOpen(PortControls *c, const PortSettings *settings)
{
    c->settings = *settings; c->gamepad = NULL; OpenFirst(c);
}
void PortControlsClose(PortControls *c)
{
    if (c->gamepad) SDL_CloseGamepad(c->gamepad);
    c->gamepad = NULL;
}
void PortControlsEvent(PortControls *c, const SDL_Event *event)
{
    if (event->type == SDL_EVENT_GAMEPAD_REMOVED && c->gamepad
        && event->gdevice.which == SDL_GetGamepadID(c->gamepad)) PortControlsClose(c);
    if (event->type == SDL_EVENT_GAMEPAD_REMOVED || event->type == SDL_EVENT_GAMEPAD_ADDED) OpenFirst(c);
}
Uint16 PortControlsPressed(PortControls *c, const bool *keyboard)
{
    Uint16 keys = 0;
    for (int i = 0; i < PORT_ACTIONS; ++i) {
        int binding = c->settings.buttons[i];
        bool pad = false;
        if (c->gamepad) {
            if (binding < SDL_GAMEPAD_BUTTON_COUNT)
                pad = SDL_GetGamepadButton(c->gamepad, (SDL_GamepadButton)binding);
            else pad = SDL_GetGamepadAxis(c->gamepad, binding == PORT_PAD_LEFT_TRIGGER
                        ? SDL_GAMEPAD_AXIS_LEFT_TRIGGER : SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 16384;
        }
        if ((keyboard && keyboard[c->settings.keys[i]]) || pad) keys |= PortActionBits[i];
    }
    if (c->gamepad && c->settings.analog) {
        int threshold = c->settings.deadzone * 32767 / 100;
        int x = SDL_GetGamepadAxis(c->gamepad, SDL_GAMEPAD_AXIS_LEFTX);
        int y = SDL_GetGamepadAxis(c->gamepad, SDL_GAMEPAD_AXIS_LEFTY);
        if (x > threshold) keys |= 0x10;
        if (x < -threshold) keys |= 0x20;
        if (y > threshold) keys |= 0x80;
        if (y < -threshold) keys |= 0x40;
    }
    if ((keys & 0x30) == 0x30) keys &= ~0x30;
    if ((keys & 0xC0) == 0xC0) keys &= ~0xC0;
    return keys;
}

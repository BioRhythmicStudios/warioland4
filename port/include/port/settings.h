#ifndef PORT_SETTINGS_H
#define PORT_SETTINGS_H
#include <SDL3/SDL.h>
#define PORT_ACTIONS 10
#define PORT_PAD_LEFT_TRIGGER SDL_GAMEPAD_BUTTON_COUNT
#define PORT_PAD_RIGHT_TRIGGER (SDL_GAMEPAD_BUTTON_COUNT + 1)
#define PORT_PAD_CHOICES (SDL_GAMEPAD_BUTTON_COUNT + 2)
typedef struct PortSettings {
    int keys[PORT_ACTIONS], buttons[PORT_ACTIONS];
    int scale, fullscreen, analog, deadzone, vsync;
    int rumble, rumbleStrength;
    int crtPreset, crtScanlines, crtGlow, crtMask;
} PortSettings;
extern const char *const PortActionNames[PORT_ACTIONS];
extern const Uint16 PortActionBits[PORT_ACTIONS];
void PortSettingsDefaults(PortSettings *settings);
int PortSettingsValid(const PortSettings *settings, char *error, size_t size);
// 0 loaded; 1 missing/defaults; -1 invalid/unreadable/defaults, with explanation.
int PortSettingsLoad(PortSettings *settings, char *error, size_t size);
int PortSettingsSave(const PortSettings *settings, char *error, size_t size);
int PortSettingsPath(char *path, size_t size);
int PortSettingsBind(PortSettings *settings, int controller, int action, int code, char *error, size_t size);
const char *PortPadName(int code);
// Separate SDL window; edits are a draft until Save. Returns 1 if saved.
int PortSettingsShow(PortSettings *settings);
#endif

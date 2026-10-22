// A small SDL settings editor. SDL events capture physical keys and gamepad inputs.
#include "port/settings.h"
#include "port/crt.h"
#include "port/controls.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static void Text(SDL_Renderer *r, float x, float y, const char *text)
{
    SDL_SetRenderDrawColor(r, 249, 235, 255, 255);
    SDL_RenderDebugText(r, x, y, text);
}
static bool Hit(float x, float y, float left, float top, float width, float height)
{ return x >= left && y >= top && x < left + width && y < top + height; }
static void Button(SDL_Renderer *r, float x, float y, float w, float h, const char *text)
{
    SDL_FRect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(r, 255, 204, 64, 255); SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawColor(r, 40, 17, 66, 255);
    SDL_RenderDebugText(r, x + 6, y + (h - 8) / 2, text);
}
static void Status(char *target, const char *text)
{ snprintf(target, 180, "%s", text); }

int PortSettingsShow(PortSettings *settings)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    PortSettings draft = *settings;
    PortControls controls;
    int tab = 0, capture = -1, chooser = -1, running = 1, saved = 0;
    char status[180] = "Changes apply after restarting the game.";
    float uiScale = 2.0f;
    int width = 900, height = 880;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) return -1;
#ifndef _WIN32
    SDL_Rect desktop;
    if (SDL_GetDisplayUsableBounds(SDL_GetPrimaryDisplay(), &desktop) && desktop.h < 920) {
        height = SDL_clamp(desktop.h - 40, 520, 880);
        uiScale = height / 440.0f;
        width = (int)(450 * uiScale);
    }
#endif
    if (!SDL_CreateWindowAndRenderer("Wario Land 4 - Settings", width, height, 0, &window, &renderer)) {
        SDL_Quit(); return -1;
    }
#ifdef _WIN32
    HWND hwnd = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1), IMAGE_ICON, 32, 32, LR_SHARED));
    SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1), IMAGE_ICON, 16, 16, LR_SHARED));
#endif
    SDL_SetRenderScale(renderer, uiScale, uiScale);
    if (!SDL_SetRenderVSync(renderer, 1))
        fprintf(stderr, "Settings window VSync unavailable: %s\n", SDL_GetError());
    PortControlsOpen(&controls, &draft);
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            PortControlsEvent(&controls, &event);
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {running = 0; break;}
            if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST && capture >= 0) {
                capture = -1; Status(status, "Remapping cancelled because Settings lost focus.");
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                if (capture >= 0 || chooser >= 0) {
                    capture = chooser = -1; Status(status, "Remapping cancelled. Previous binding kept.");
                } else running = 0;
                continue;
            }
            if (capture >= 0) {
                int code = -1;
                if (tab == 0 && event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) code = event.key.scancode;
                if (tab == 1 && event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) code = event.gbutton.button;
                if (tab == 1 && event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION && event.gaxis.value > 16384) {
                    if (event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER) code = PORT_PAD_LEFT_TRIGGER;
                    if (event.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) code = PORT_PAD_RIGHT_TRIGGER;
                }
                if (code >= 0 && !PortSettingsBind(&draft, tab == 1, capture, code, status, sizeof(status))) {
                    capture = -1; Status(status, "Binding updated in draft. Save to keep changes.");
                }
                continue;
            }
            if (event.type != SDL_EVENT_MOUSE_BUTTON_DOWN || event.button.button != SDL_BUTTON_LEFT) continue;
            float x = event.button.x / uiScale, y = event.button.y / uiScale;
            if (chooser >= 0) {
                if (Hit(x, y, 12, 60, 426, 24)) {
                    capture = chooser; chooser = -1;
                    Status(status, "Press a controller button/trigger. Escape cancels.");
                    continue;
                }
                for (int code = 0; code < PORT_PAD_CHOICES; ++code)
                    if (Hit(x, y, 12 + (code / 15) * 216, 94 + (code % 15) * 18, 210, 16)) {
                        if (!PortSettingsBind(&draft, 1, chooser, code, status, sizeof(status))) {
                            chooser = -1; Status(status, "Binding updated in draft. Save to keep changes.");
                        }
                        break;
                    }
                continue;
            }
            if (y >= 34 && y < 56) {
                for (int i = 0; i < 3; ++i) if (Hit(x, y, 12 + i * 144, 34, 138, 22)) tab = i;
                Status(status, "Changes apply after restarting the game.");
            }
            if (Hit(x, y, 12, 407, 105, 24)) {
                PortSettings defaults; PortSettingsDefaults(&defaults);
                if (tab == 0) memcpy(draft.keys, defaults.keys, sizeof(draft.keys));
                else if (tab == 1) {
                    memcpy(draft.buttons, defaults.buttons, sizeof(draft.buttons));
                    draft.rumble = defaults.rumble; draft.rumbleStrength = defaults.rumbleStrength;
                    PortControlsStopRumble(&controls);
                    draft.analog = defaults.analog; draft.deadzone = defaults.deadzone;
                } else { draft.scale = defaults.scale; draft.fullscreen = defaults.fullscreen; draft.vsync = defaults.vsync;
                    draft.crtPreset = defaults.crtPreset; draft.crtScanlines = defaults.crtScanlines;
                    draft.crtGlow = defaults.crtGlow; draft.crtMask = defaults.crtMask; }
                Status(status, "Section defaults restored in draft. Save to keep.");
            }
            if (Hit(x, y, 228, 407, 90, 24)) running = 0;
            if (Hit(x, y, 332, 407, 106, 24) && !PortSettingsSave(&draft, status, sizeof(status))) {
                *settings = draft; saved = 1; running = 0;
            }
            if (tab < 2 && x >= 130 && x < 438 && y >= 98 && y < 278) {
                int action = (int)(y - 98) / 18;
                if (tab == 0) {
                    capture = action; Status(status, "Press a physical key. Escape cancels.");
                } else chooser = action;
            }
            if (tab == 1) {
                if (Hit(x, y, 12, 336, 140, 20)) {
                    draft.rumble = !draft.rumble;
                    if (!draft.rumble) PortControlsStopRumble(&controls);
                }
                if (Hit(x, y, 264, 336, 38, 20)) draft.rumbleStrength = SDL_max(0, draft.rumbleStrength - 10);
                if (Hit(x, y, 310, 336, 38, 20)) draft.rumbleStrength = SDL_min(100, draft.rumbleStrength + 10);
                if (!draft.rumbleStrength) PortControlsStopRumble(&controls);
                if (Hit(x, y, 12, 360, 140, 16)) {
                    controls.settings = draft;
                    Status(status, PortControlsRumble(&controls, 36000, 18000, 180)
                        ? "Test pulse sent. Save to keep settings." : "Rumble Off, zero strength, or unavailable.");
                }
                if (Hit(x, y, 12, 286, 282, 20)) draft.analog = !draft.analog;
                if (Hit(x, y, 264, 310, 38, 20) && draft.deadzone > 10) draft.deadzone = SDL_max(10, draft.deadzone - 5);
                if (Hit(x, y, 310, 310, 38, 20) && draft.deadzone < 80) draft.deadzone = SDL_min(80, draft.deadzone + 5);
            }
            if (tab == 2) {
                if (Hit(x, y, 180, 76, 38, 24) && draft.scale > 1) --draft.scale;
                if (Hit(x, y, 226, 76, 38, 24) && draft.scale < 6) ++draft.scale;
                if (Hit(x, y, 12, 108, 426, 24)) draft.fullscreen = !draft.fullscreen;
                if (Hit(x, y, 12, 140, 426, 24)) draft.vsync = !draft.vsync;
                if (Hit(x, y, 12, 172, 426, 24)) draft.crtPreset = (draft.crtPreset + 1) % 3;
                if (draft.crtPreset) {
                    if (Hit(x, y, 302, 204, 38, 24)) draft.crtScanlines = SDL_max(0, draft.crtScanlines - 5);
                    if (Hit(x, y, 346, 204, 38, 24)) draft.crtScanlines = SDL_min(50, draft.crtScanlines + 5);
                }
                if (draft.crtPreset == 2) {
                    if (Hit(x, y, 302, 236, 38, 24)) draft.crtGlow = SDL_max(0, draft.crtGlow - 5);
                    if (Hit(x, y, 346, 236, 38, 24)) draft.crtGlow = SDL_min(30, draft.crtGlow + 5);
                    if (Hit(x, y, 12, 268, 426, 24)) draft.crtMask = !draft.crtMask;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 40, 17, 66, 255); SDL_RenderClear(renderer);
        Text(renderer, 12, 12, "WARIO LAND 4 / SETTINGS");
        Button(renderer, 12, 34, 138, 22, tab == 0 ? "[Keyboard]" : "Keyboard");
        Button(renderer, 156, 34, 138, 22, tab == 1 ? "[Controller]" : "Controller");
        Button(renderer, 300, 34, 138, 22, tab == 2 ? "[Display]" : "Display");
        char label[160];
        if (chooser >= 0) {
            snprintf(label, sizeof(label), "Press input for %s", PortActionNames[chooser]);
            Button(renderer, 12, 60, 426, 24, label);
            for (int code = 0; code < PORT_PAD_CHOICES; ++code) {
                snprintf(label, sizeof(label), "%.24s", PortPadName(code));
                Button(renderer, 12 + (code / 15) * 216, 94 + (code % 15) * 18, 210, 16, label);
            }
        } else if (tab < 2) {
            if (tab == 0) {
                Text(renderer, 12, 66, "Click a binding, then press a physical key.");
                Text(renderer, 12, 80, "Escape cancels. Tab/F8/F11 are reserved.");
            } else {
                int count = 0; SDL_JoystickID *ids = SDL_GetGamepads(&count);
                snprintf(label, sizeof(label), "%d connected / %.33s", count,
                         controls.gamepad ? SDL_GetGamepadName(controls.gamepad) : "Keyboard works without a controller");
                Text(renderer, 12, 64, label);
                // Cycle the visible name so all connected devices can be inspected.
                if (count > 0) {
                    int index = (int)(SDL_GetTicks() / 3000 % count);
                    snprintf(label, sizeof(label), "[%d/%d] %.43s", index + 1, count, SDL_GetGamepadNameForID(ids[index]));
                    Text(renderer, 12, 79, label);
                } else Text(renderer, 12, 79, "Click binding: choose input without hardware.");
                SDL_free(ids);
            }
            for (int i = 0; i < PORT_ACTIONS; ++i) {
                Text(renderer, 20, 103 + i * 18, PortActionNames[i]);
                snprintf(label, sizeof(label), "%.35s", capture == i ? "PRESS INPUT... (Escape cancels)" : tab == 0
                         ? SDL_GetScancodeName((SDL_Scancode)draft.keys[i]) : PortPadName(draft.buttons[i]));
                Button(renderer, 130, 98 + i * 18, 308, 16, label);
            }
            if (tab == 1) {
                snprintf(label, sizeof(label), "Left stick movement: %s", draft.analog ? "ON" : "OFF");
                Button(renderer, 12, 286, 282, 20, label);
                snprintf(label, sizeof(label), "Dead zone: %d%%", draft.deadzone); Text(renderer, 12, 316, label);
                Button(renderer, 264, 310, 38, 20, "-"); Button(renderer, 310, 310, 38, 20, "+");
                snprintf(label, sizeof(label), "Rumble: %s", draft.rumble ? "ON" : "OFF");
                Button(renderer, 12, 336, 140, 20, label);
                snprintf(label, sizeof(label), "%d%%", draft.rumbleStrength); Text(renderer, 180, 342, label);
                Button(renderer, 264, 336, 38, 20, "-"); Button(renderer, 310, 336, 38, 20, "+");
                Button(renderer, 12, 360, 140, 16, "Test Rumble");
            } else Text(renderer, 12, 295, "Physical key positions stay stable across layouts.");
        } else {
            snprintf(label, sizeof(label), "Window scale: %dx", draft.scale); Text(renderer, 12, 84, label);
            Button(renderer, 180, 76, 38, 24, "-"); Button(renderer, 226, 76, 38, 24, "+");
            snprintf(label, sizeof(label), "Start fullscreen: %s", draft.fullscreen ? "ON" : "OFF");
            Button(renderer, 12, 108, 426, 24, label);
            snprintf(label, sizeof(label), "VSync: %s", draft.vsync ? "ON" : "OFF");
            Button(renderer, 12, 140, 426, 24, label);
            snprintf(label, sizeof(label), "Filter: %s (click to cycle)", PortCrtName(draft.crtPreset));
            Button(renderer, 12, 172, 426, 24, label);
            if (draft.crtPreset) {
                snprintf(label, sizeof(label), "Scanline strength: %d%%", draft.crtScanlines); Text(renderer, 12, 212, label);
                Button(renderer, 302, 204, 38, 24, "-"); Button(renderer, 346, 204, 38, 24, "+");
            }
            if (draft.crtPreset == 2) {
                snprintf(label, sizeof(label), "Glow: %d%%", draft.crtGlow); Text(renderer, 12, 244, label);
                Button(renderer, 302, 236, 38, 24, "-"); Button(renderer, 346, 236, 38, 24, "+");
                snprintf(label, sizeof(label), "Phosphor mask: %s", draft.crtMask ? "ON" : "OFF");
                Button(renderer, 12, 268, 426, 24, label);
            }
            Text(renderer, 12, 304, "Scanlines need 2x; mask needs 3x or larger.");
            Text(renderer, 12, 320, "Save, then restart the game to apply changes.");
        }
        if (chooser < 0) {
            char line[54]; snprintf(line, sizeof(line), "%.53s", status); Text(renderer, 12, 378, line);
            if (strlen(status) > 53) {snprintf(line, sizeof(line), "%.53s", status + 53); Text(renderer, 12, 390, line);}
            Button(renderer, 12, 407, 105, 24, "Defaults");
            Button(renderer, 228, 407, 90, 24, "Cancel"); Button(renderer, 332, 407, 106, 24, "Save");
        }
        SDL_RenderPresent(renderer); SDL_Delay(10);
    }
    PortControlsClose(&controls);
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
    return saved;
}

// A small SDL settings editor. SDL events capture physical keys and gamepad inputs.
#include "port/settings.h"
#include "port/controls.h"
#include <stdio.h>
#include <string.h>

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
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) return -1;
    if (!SDL_CreateWindowAndRenderer("Wario Land 4 - Settings", 900, 800, 0, &window, &renderer)) {
        SDL_Quit(); return -1;
    }
    SDL_SetRenderScale(renderer, 2, 2);
    SDL_SetRenderVSync(renderer, 1);
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
            float x = event.button.x / 2, y = event.button.y / 2;
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
            if (Hit(x, y, 12, 367, 105, 24)) {
                PortSettings defaults; PortSettingsDefaults(&defaults);
                if (tab == 0) memcpy(draft.keys, defaults.keys, sizeof(draft.keys));
                else if (tab == 1) {
                    memcpy(draft.buttons, defaults.buttons, sizeof(draft.buttons));
                    draft.analog = defaults.analog; draft.deadzone = defaults.deadzone;
                } else { draft.scale = defaults.scale; draft.fullscreen = defaults.fullscreen; }
                Status(status, "Section defaults restored in draft. Save to keep.");
            }
            if (Hit(x, y, 228, 367, 90, 24)) running = 0;
            if (Hit(x, y, 332, 367, 106, 24) && !PortSettingsSave(&draft, status, sizeof(status))) {
                *settings = draft; saved = 1; running = 0;
            }
            if (tab < 2 && x >= 130 && x < 438 && y >= 98 && y < 278) {
                int action = (int)(y - 98) / 18;
                if (tab == 0) {
                    capture = action; Status(status, "Press a physical key. Escape cancels.");
                } else chooser = action;
            }
            if (tab == 1) {
                if (Hit(x, y, 12, 286, 282, 20)) draft.analog = !draft.analog;
                if (Hit(x, y, 264, 310, 38, 20) && draft.deadzone > 10) draft.deadzone = SDL_max(10, draft.deadzone - 5);
                if (Hit(x, y, 310, 310, 38, 20) && draft.deadzone < 80) draft.deadzone = SDL_min(80, draft.deadzone + 5);
            }
            if (tab == 2) {
                if (Hit(x, y, 180, 110, 38, 24) && draft.scale > 1) --draft.scale;
                if (Hit(x, y, 226, 110, 38, 24) && draft.scale < 6) ++draft.scale;
                if (Hit(x, y, 12, 160, 330, 24)) draft.fullscreen = !draft.fullscreen;
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
            } else Text(renderer, 12, 295, "Physical key positions stay stable across layouts.");
        } else {
            snprintf(label, sizeof(label), "Window scale: %dx", draft.scale); Text(renderer, 12, 119, label);
            Button(renderer, 180, 110, 38, 24, "-"); Button(renderer, 226, 110, 38, 24, "+");
            snprintf(label, sizeof(label), "Start fullscreen: %s", draft.fullscreen ? "ON" : "OFF");
            Button(renderer, 12, 160, 330, 24, label);
            Text(renderer, 12, 210, "F11 still toggles fullscreen while playing.");
            Text(renderer, 12, 231, "Explicit --scale overrides the saved window scale.");
            Text(renderer, 12, 270, "Settings are separate from your SRAM save.");
        }
        if (chooser < 0) {
            char line[54]; snprintf(line, sizeof(line), "%.53s", status); Text(renderer, 12, 338, line);
            if (strlen(status) > 53) {snprintf(line, sizeof(line), "%.53s", status + 53); Text(renderer, 12, 350, line);}
            Button(renderer, 12, 367, 105, 24, "Defaults");
            Button(renderer, 228, 367, 90, 24, "Cancel"); Button(renderer, 332, 367, 106, 24, "Save");
        }
        SDL_RenderPresent(renderer); SDL_Delay(10);
    }
    PortControlsClose(&controls);
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
    return saved;
}

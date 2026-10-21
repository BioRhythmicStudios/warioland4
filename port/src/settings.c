#include "port/settings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include "port/os.h"
#endif

const char *const PortActionNames[PORT_ACTIONS] = {"A", "B", "Select", "Start", "Right", "Left", "Up", "Down", "R", "L"};
const Uint16 PortActionBits[PORT_ACTIONS] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};

void PortSettingsDefaults(PortSettings *s)
{
    static const int keys[PORT_ACTIONS] = {SDL_SCANCODE_X, SDL_SCANCODE_Z, SDL_SCANCODE_BACKSPACE,
        SDL_SCANCODE_RETURN, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT, SDL_SCANCODE_UP,
        SDL_SCANCODE_DOWN, SDL_SCANCODE_S, SDL_SCANCODE_A};
    static const int buttons[PORT_ACTIONS] = {SDL_GAMEPAD_BUTTON_SOUTH, SDL_GAMEPAD_BUTTON_EAST,
        SDL_GAMEPAD_BUTTON_BACK, SDL_GAMEPAD_BUTTON_START, SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
        SDL_GAMEPAD_BUTTON_DPAD_LEFT, SDL_GAMEPAD_BUTTON_DPAD_UP, SDL_GAMEPAD_BUTTON_DPAD_DOWN,
        SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER};
    memset(s, 0, sizeof(*s));
    memcpy(s->keys, keys, sizeof(keys)); memcpy(s->buttons, buttons, sizeof(buttons));
    s->rumbleStrength = 50;
    s->scale = 3; s->analog = 1; s->deadzone = 50; s->vsync = 1;
    s->crtScanlines = 20; s->crtGlow = 10; s->crtMask = 1;
}

static int AllowedKey(int key)
{
    return key > SDL_SCANCODE_UNKNOWN && key < SDL_SCANCODE_COUNT
        && SDL_GetScancodeName((SDL_Scancode)key)[0]
        && key != SDL_SCANCODE_ESCAPE && key != SDL_SCANCODE_TAB
        && key != SDL_SCANCODE_F8 && key != SDL_SCANCODE_F11;
}

int PortSettingsBind(PortSettings *s, int controller, int action, int code, char *error, size_t size)
{
    int *bindings = controller ? s->buttons : s->keys;
    if (action < 0 || action >= PORT_ACTIONS || (controller ? code < 0 || code >= PORT_PAD_CHOICES : !AllowedKey(code))) {
        snprintf(error, size, "Unsupported binding. Escape, Tab, F8 and F11 are reserved.");
        return -1;
    }
    for (int i = 0; i < PORT_ACTIONS; ++i)
        if (i != action && bindings[i] == code) {
            snprintf(error, size, "Already assigned to %s. Choose another input first.", PortActionNames[i]);
            return -1;
        }
    bindings[action] = code;
    return 0;
}

int PortSettingsValid(const PortSettings *s, char *error, size_t size)
{
    PortSettings draft = *s;
    if (s->scale < 1 || s->scale > 6 || s->fullscreen < 0 || s->fullscreen > 1
        || s->analog < 0 || s->analog > 1 || s->deadzone < 10 || s->deadzone > 80
        || s->rumble < 0 || s->rumble > 1 || s->rumbleStrength < 0 || s->rumbleStrength > 100
        || s->vsync < 0 || s->vsync > 1
        || s->crtPreset < 0 || s->crtPreset > 2 || s->crtScanlines < 0 || s->crtScanlines > 50
        || s->crtGlow < 0 || s->crtGlow > 30 || s->crtMask < 0 || s->crtMask > 1) {
        snprintf(error, size, "Display or stick settings are outside their supported range.");
        return -1;
    }
    for (int i = 0; i < PORT_ACTIONS; ++i)
        if (PortSettingsBind(&draft, 0, i, s->keys[i], error, size)
            || PortSettingsBind(&draft, 1, i, s->buttons[i], error, size)) return -1;
    return 0;
}

const char *PortPadName(int code)
{
    if (code == PORT_PAD_LEFT_TRIGGER) return "Left trigger (LT)";
    if (code == PORT_PAD_RIGHT_TRIGGER) return "Right trigger (RT)";
    // Positional SDL names with Xbox labels where applicable.
    static const char *names[] = {"South (Xbox A)", "East (Xbox B)", "West (Xbox X)", "North (Xbox Y)"};
    if (code >= 0 && code < 4) return names[code];
    const char *name = SDL_GetGamepadStringForButton((SDL_GamepadButton)code);
    return name ? name : "Unsupported";
}

int PortSettingsPath(char *path, size_t size)
{
    const char *override = SDL_getenv("WL4_SETTINGS_FILE");
    if (override && override[0]) {
        // Explicit developer/test override; ordinary users use the application folder.
        int n = snprintf(path, size, "%s", override);
        return n < 0 || (size_t)n >= size ? -1 : 0;
    }
    char *directory = SDL_GetPrefPath(NULL, "wl4");
    if (!directory) return -1;
    int n = snprintf(path, size, "%ssettings.cfg", directory);
    SDL_free(directory);
    return n < 0 || (size_t)n >= size ? -1 : 0;
}

#ifdef _WIN32
static int Wide(const char *path, wchar_t *wide, int capacity)
{
    return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, capacity) != 0;
}

#endif

int PortSettingsLoad(PortSettings *s, char *error, size_t size)
{
    char path[4096], data[1024];
#ifdef _WIN32
    wchar_t wide[4096];
    DWORD count;
#else
    long count;
    int missing;
#endif
    PortSettingsDefaults(s); error[0] = 0;
    if (PortSettingsPath(path, sizeof(path))) goto invalid;
#ifdef _WIN32
    if (!Wide(path, wide, 4096)) goto invalid;
    HANDLE file = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        DWORD reason = GetLastError();
        if (reason == ERROR_FILE_NOT_FOUND || reason == ERROR_PATH_NOT_FOUND) return 1;
        goto invalid;
    }
    LARGE_INTEGER length;
    int ok = GetFileSizeEx(file, &length) && length.QuadPart > 0 && length.QuadPart < (LONGLONG)sizeof(data)
        && ReadFile(file, data, sizeof(data) - 1, &count, NULL) && count == length.QuadPart;
    CloseHandle(file);
    if (!ok) goto invalid;
#else
    count = PortOsReadFile(path, data, sizeof(data), &missing);
    if (count < 0 && missing) return 1;
    if (count <= 0 || count >= (long)sizeof(data)) goto invalid;
#endif
    data[count] = 0;
    // Strict bounded integer tokens; reject truncation, extra data, unknown versions,
    // duplicate bindings and reserved keys as a whole. Never rewrite a bad file on load.
    int values[1 + 11 + PORT_ACTIONS * 2];
    size_t tokens = SDL_arraysize(values);
    char *cursor = data, *end;
    if (strncmp(cursor, "WL4_SETTINGS ", 13)) goto invalid;
    cursor += 13;
    for (size_t i = 0; i < tokens; ++i) {
        while (*cursor == ' ' || *cursor == '\r' || *cursor == '\n') ++cursor;
        if (*cursor < '0' || *cursor > '9') goto invalid;
        unsigned long value = strtoul(cursor, &end, 10);
        if (end == cursor || value > INT_MAX || (*end && *end != ' ' && *end != '\r' && *end != '\n')) goto invalid;
        values[i] = (int)value; cursor = end;
        if (i == 0) {
            if (values[0] < 1 || values[0] > 4) goto invalid;
            if (values[0] < 4) tokens -= values[0] == 1 ? 7 : values[0] == 2 ? 6 : 2;
        }
    }
    while (*cursor == ' ' || *cursor == '\r' || *cursor == '\n') ++cursor;
    if (*cursor || cursor != data + count) goto invalid;
    PortSettings draft;
    PortSettingsDefaults(&draft);
    draft.scale = values[1]; draft.fullscreen = values[2]; draft.analog = values[3]; draft.deadzone = values[4];
    // v1 had no VSync control: preserve its Off behaviour and all other values.
    // Loading never rewrites an old file; the next explicit Save upgrades it.
    draft.vsync = values[0] >= 2 ? values[5] : 0;
    // v1/v2 preserve all existing values and default to Original (Off).
    if (values[0] >= 3) {
        draft.crtPreset = values[6]; draft.crtScanlines = values[7];
        draft.crtGlow = values[8]; draft.crtMask = values[9];
    }
    if (values[0] == 4) { draft.rumble = values[10]; draft.rumbleStrength = values[11]; }
    int bindings = values[0] == 4 ? 12 : values[0] == 3 ? 10 : values[0] == 2 ? 6 : 5;
    memcpy(draft.keys, values + bindings, sizeof(draft.keys));
    memcpy(draft.buttons, values + bindings + PORT_ACTIONS, sizeof(draft.buttons));
    if (PortSettingsValid(&draft, error, size)) goto invalid;
    *s = draft;
    return 0;
invalid:
    snprintf(error, size, "Settings could not be read or are unsupported. Defaults are active; Save in Settings to replace the file.");
    return -1;
}

int PortSettingsSave(const PortSettings *s, char *error, size_t size)
{
    char path[4096], data[1024];
#ifdef _WIN32
    wchar_t wide[4096], temporary[4200];
#endif
    if (PortSettingsValid(s, error, size)) return -1;
    if (PortSettingsPath(path, sizeof(path))) goto failed;
#ifdef _WIN32
    if (!Wide(path, wide, 4096)) goto failed;
#endif
    int used = snprintf(data, sizeof(data), "WL4_SETTINGS 4\n%d %d %d %d %d %d %d %d %d %d %d\n",
                        s->scale, s->fullscreen, s->analog, s->deadzone, s->vsync,
                        s->crtPreset, s->crtScanlines, s->crtGlow, s->crtMask, s->rumble, s->rumbleStrength);
    for (int device = 0; device < 2; ++device)
        for (int i = 0; i < PORT_ACTIONS; ++i)
            used += snprintf(data + used, sizeof(data) - used, "%d%c", device ? s->buttons[i] : s->keys[i], i == PORT_ACTIONS - 1 ? '\n' : ' ');
#ifdef _WIN32
    swprintf(temporary, 4200, L"%ls.%lu.%llu.tmp", wide, GetCurrentProcessId(), (unsigned long long)GetTickCount64());
    HANDLE file = CreateFileW(temporary, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto failed;
    DWORD written;
    int ok = WriteFile(file, data, used, &written, NULL) && written == (DWORD)used && FlushFileBuffers(file);
    if (!CloseHandle(file)) ok = 0;
    if (ok) ok = MoveFileExW(temporary, wide, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    if (!ok) {
        // Retain the exclusively created temporary file for diagnosis; do not touch saves.
        goto failed;
    }
#else
    if (PortOsWriteFileAtomic(path, data, (size_t)used, error, size)) return -1;
#endif
    error[0] = 0;
    return 0;
failed:
#ifdef _WIN32
    snprintf(error, size, "Cannot save settings in your application configuration folder (Windows error %lu).", GetLastError());
#else
    snprintf(error, size, "Cannot resolve the application configuration path.");
#endif
    return -1;
}

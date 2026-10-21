// Disposable settings tests plus SDL virtual-gamepad tests, never SRAM.
#include "port/settings.h"
#include "port/controls.h"
#include "port/rumble.h"
#include "port/frame_pacer.h"
#include "port/render_vsync.h"
#include <stdio.h>
#include <string.h>

static int rumbleCalls;
static Uint16 rumbleLow, rumbleHigh;
static bool SDLCALL Rumble(void *userdata, Uint16 low, Uint16 high)
{
    (void)userdata; ++rumbleCalls; rumbleLow = low; rumbleHigh = high; return true;
}
static int failures;
#define CHECK(test, label) do { if (!(test)) { fprintf(stderr, "FAIL: %s (%s)\n", label, SDL_GetError()); ++failures; } } while (0)
static void Pump(PortControls *controls)
{
    SDL_UpdateJoysticks();
    SDL_Event event;
    while (SDL_PollEvent(&event)) PortControlsEvent(controls, &event);
}

static void TestPacing(void)
{
    // Fake present completion times: monitor refresh must not set game speed.
    const unsigned int rates[] = {30, 60, 120, 144};
    for (size_t i = 0; i < SDL_arraysize(rates); ++i) {
        PortFramePacer p = {0};
        unsigned int frames = 0, most = 0;
        uint64_t now = 0;
        for (unsigned int refresh = 0; refresh <= rates[i] * 100; ++refresh) {
            now = (uint64_t)refresh * 1000000000ull / rates[i];
            unsigned int n = PortFramePacerDue(&p, now, 0);
            frames += n; if (n > most) most = n;
        }
        CHECK(frames == now / PORT_FRAME_NS + 1 && !p.resets, "GBA clock independent of synthetic monitor/present rate");
        CHECK(most <= 2, "no unbounded catch-up or doubled speed");
    }
    PortFramePacer p = {0};
    CHECK(PortFramePacerDue(&p, 0, 0) == 1, "first simulation frame");
    CHECK(PortFramePacerDue(&p, PORT_FRAME_NS - 1, 0) == 0, "early present does not advance simulation");
    CHECK(PortFramePacerDue(&p, 3 * PORT_FRAME_NS, 0) == 3, "short blocking present preserves due simulation frames");
    CHECK(PortFramePacerDue(&p, 100 * PORT_FRAME_NS, 0) == 1 && p.resets == 1, "long stall resets bounded backlog");
    CHECK(PortFramePacerDue(&p, 100 * PORT_FRAME_NS, 1) == 1, "fast forward bypasses clock");
    CHECK(PortFramePacerDue(&p, 100 * PORT_FRAME_NS, 0) == 0, "fast-forward release resumes clock without burst");
}

int main(int argc, char **argv)
{
    const char *override = SDL_getenv("WL4_SETTINGS_FILE");
    if (argc != 2 || strcmp(argv[1], "--isolated") || !override || !override[0]) {
        fprintf(stderr, "Set WL4_SETTINGS_FILE to a NEW disposable .cfg path and pass --isolated.\n"); return 2;
    }
    SDL_PathInfo info;
    if (SDL_GetPathInfo(override, &info)) {fprintf(stderr, "Refusing an existing test destination.\n"); return 2;}
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (!SDL_Init(SDL_INIT_GAMEPAD)) return 1;
    TestPacing();
    // Invalid-renderer fixture exercises API failure handling without a GUI.
    // This is not a claim that an unsupported physical display driver was tested.
    CHECK(!PortApplyRenderVSync(NULL, 1) && !PortApplyRenderVSync(NULL, 0), "VSync API failures are non-fatal");
    int count = 0; SDL_JoystickID *real = SDL_GetGamepads(&count);
    printf("Connected hardware before virtual tests: %d\n", count);
    for (int i = 0; i < count; ++i) printf("  %s\n", SDL_GetGamepadNameForID(real[i]));
    SDL_free(real);
    PortSettings s, loaded; char error[256];
    CHECK(PortSettingsLoad(&s, error, sizeof(error)) == 1, "missing config uses defaults");
    CHECK(s.keys[0] == SDL_SCANCODE_X && s.scale == 3 && s.analog && s.deadzone == 50 && s.vsync == 1, "defaults include VSync On");
    CHECK(s.crtPreset == 0 && s.crtScanlines == 20 && s.crtGlow == 10 && s.crtMask == 1, "CRT defaults Off with mild strengths");
    CHECK(PortSettingsBind(&s, 0, 0, SDL_SCANCODE_Z, error, sizeof(error)) < 0 && s.keys[0] == SDL_SCANCODE_X,
          "keyboard collision rejected without mutation");
    CHECK(PortSettingsBind(&s, 0, 0, SDL_SCANCODE_ESCAPE, error, sizeof(error)) < 0, "Escape is reserved");
    CHECK(PortSettingsBind(&s, 0, 0, SDL_SCANCODE_TAB, error, sizeof(error)) < 0, "Tab is reserved");
    CHECK(!PortSettingsBind(&s, 0, 0, SDL_SCANCODE_C, error, sizeof(error)), "physical key remap");
    CHECK(!PortSettingsBind(&s, 1, 0, PORT_PAD_RIGHT_TRIGGER, error, sizeof(error)), "trigger remap");
    CHECK(PortSettingsBind(&s, 1, 1, PORT_PAD_RIGHT_TRIGGER, error, sizeof(error)) < 0, "controller collision rejected");
    CHECK(!s.rumble && s.rumbleStrength == 50, "rumble defaults Off");
    s.rumble = 1; s.rumbleStrength = 70;
    CHECK(!PortSettingsSave(&s, error, sizeof(error)) && !PortSettingsLoad(&loaded, error, sizeof(error))
          && loaded.rumble == 1 && loaded.rumbleStrength == 70, "rumble persistence");
    PortSettingsDefaults(&loaded); s.rumble = loaded.rumble; s.rumbleStrength = loaded.rumbleStrength;
    s.scale = 2; s.fullscreen = 1; s.deadzone = 30;
    CHECK(!PortSettingsSave(&s, error, sizeof(error)), "atomic settings save");
    CHECK(!PortSettingsLoad(&loaded, error, sizeof(error)) && !memcmp(&s, &loaded, sizeof(s)), "restart/roundtrip all settings");
    s.vsync = 0;
    CHECK(!PortSettingsSave(&s, error, sizeof(error)) && !PortSettingsLoad(&loaded, error, sizeof(error))
          && !memcmp(&s, &loaded, sizeof(s)), "explicit VSync Off survives restart");
    char version3[1024];
    int v3used = snprintf(version3, sizeof(version3), "WL4_SETTINGS 3\n%d %d %d %d %d 2 35 15 1\n",
        s.scale, s.fullscreen, s.analog, s.deadzone, s.vsync);
    for (int device = 0; device < 2; ++device)
        for (int i = 0; i < PORT_ACTIONS; ++i)
            v3used += snprintf(version3 + v3used, sizeof(version3) - v3used, "%d%c",
                device ? s.buttons[i] : s.keys[i], i == PORT_ACTIONS - 1 ? '\n' : ' ');
    CHECK(SDL_SaveFile(override, version3, v3used), "write published v3 fixture");
    CHECK(!PortSettingsLoad(&loaded, error, sizeof(error)) && !loaded.rumble && loaded.rumbleStrength == 50
        && loaded.crtPreset == 2 && loaded.crtScanlines == 35 && loaded.crtGlow == 15 && loaded.crtMask == 1
        && !memcmp(s.keys, loaded.keys, sizeof(s.keys)) && !memcmp(s.buttons, loaded.buttons, sizeof(s.buttons)),
        "published v3 preserves CRT and bindings with rumble Off");
    size_t v3size; char *v3bytes = SDL_LoadFile(override, &v3size);
    CHECK(v3bytes && v3size == (size_t)v3used && !memcmp(v3bytes, version3, v3used), "v3 load never rewrites file");
    SDL_free(v3bytes);
    PortSettings invalidRumble = s; invalidRumble.rumbleStrength = 101;
    CHECK(PortSettingsValid(&invalidRumble, error, sizeof(error)) < 0, "excess rumble strength rejected");
    invalidRumble = s; invalidRumble.rumble = 2;
    CHECK(PortSettingsValid(&invalidRumble, error, sizeof(error)) < 0, "invalid rumble switch rejected");
    char version2[1024];
    int v2used = snprintf(version2, sizeof(version2), "WL4_SETTINGS 2\n%d %d %d %d 1\n", s.scale, s.fullscreen, s.analog, s.deadzone);
    for (int device = 0; device < 2; ++device)
        for (int i = 0; i < PORT_ACTIONS; ++i)
            v2used += snprintf(version2 + v2used, sizeof(version2) - v2used, "%d%c", device ? s.buttons[i] : s.keys[i], i == PORT_ACTIONS - 1 ? '\n' : ' ');
    CHECK(SDL_SaveFile(override, version2, v2used), "write v2 fixture with VSync On");
    s.vsync = 1;
    CHECK(!PortSettingsLoad(&loaded, error, sizeof(error)) && !memcmp(&s, &loaded, sizeof(s)), "v2 preserves VSync On and bindings; CRT stays Off");
    size_t v2size = 0; char *v2bytes = SDL_LoadFile(override, &v2size);
    CHECK(v2bytes && v2size == (size_t)v2used && !memcmp(v2bytes, version2, v2used), "v2 migration does not rewrite config");
    SDL_free(v2bytes); s.vsync = 0;
    char legacy[1024];
    int used = snprintf(legacy, sizeof(legacy), "WL4_SETTINGS 1\n%d %d %d %d\n", s.scale, s.fullscreen, s.analog, s.deadzone);
    for (int device = 0; device < 2; ++device)
        for (int i = 0; i < PORT_ACTIONS; ++i)
            used += snprintf(legacy + used, sizeof(legacy) - used, "%d%c", device ? s.buttons[i] : s.keys[i], i == PORT_ACTIONS - 1 ? '\n' : ' ');
    CHECK(SDL_SaveFile(override, legacy, used), "write v1 fixture");
    CHECK(!PortSettingsLoad(&loaded, error, sizeof(error)) && !memcmp(&s, &loaded, sizeof(s)), "v1 preserves all settings and former VSync Off behaviour");
    size_t unchangedSize = 0;
    char *unchanged = SDL_LoadFile(override, &unchangedSize);
    CHECK(unchanged && unchangedSize == (size_t)used && !memcmp(unchanged, legacy, used), "legacy load never rewrites file");
    SDL_free(unchanged);
    CHECK(!PortSettingsSave(&loaded, error, sizeof(error)), "explicit save upgrades v1");
    const char *bad[] = {"WL4_SETTINGS 99\n", "garbage", "WL4_SETTINGS 1\n2 1 1 30\n6", "WL4_SETTINGS 0\n",
                         "WL4_SETTINGS 2\n2 1 1 30\n", "WL4_SETTINGS 2\n2 1 1 30 2\n"};
    for (size_t i = 0; i < SDL_arraysize(bad); ++i) {
        CHECK(SDL_SaveFile(override, bad[i], strlen(bad[i])), "write deliberate corrupt fixture");
        CHECK(PortSettingsLoad(&loaded, error, sizeof(error)) < 0 && loaded.keys[0] == SDL_SCANCODE_X,
              "corrupt/outdated config falls back as a whole");
    }
    CHECK(!PortSettingsSave(&s, error, sizeof(error)), "explicit save recovers invalid file");
    // Struct validation also protects every path that saves an edited configuration.
    PortSettings invalid = s; invalid.keys[0] = invalid.keys[1];
    CHECK(PortSettingsValid(&invalid, error, sizeof(error)) < 0, "duplicate keys rejected by file validation");
    invalid = s; invalid.buttons[0] = invalid.buttons[1];
    CHECK(PortSettingsValid(&invalid, error, sizeof(error)) < 0, "duplicate pad mappings rejected by file validation");
    invalid = s; invalid.deadzone = 99;
    CHECK(PortSettingsSave(&invalid, error, sizeof(error)) < 0, "out-of-range config cannot overwrite valid settings");
    CHECK(!PortSettingsLoad(&loaded, error, sizeof(error)) && !memcmp(&loaded, &s, sizeof(s)), "failed validation preserves previous config");
    invalid = s; invalid.vsync = 2;
    CHECK(PortSettingsSave(&invalid, error, sizeof(error)) < 0, "invalid VSync rejected");

    for (int preset = 0; preset < 3; ++preset) {
        s.crtPreset = preset; s.crtScanlines = preset * 25; s.crtGlow = preset * 15; s.crtMask = preset % 2;
        CHECK(!PortSettingsSave(&s, error, sizeof(error)) && !PortSettingsLoad(&loaded, error, sizeof(error))
              && !memcmp(&s, &loaded, sizeof(s)), "all CRT presets and strength endpoints survive restart");
    }
    invalid = s; invalid.crtPreset = 3;
    CHECK(PortSettingsSave(&invalid, error, sizeof(error)) < 0, "invalid CRT preset rejected");
    invalid = s; invalid.crtScanlines = 51;
    CHECK(PortSettingsSave(&invalid, error, sizeof(error)) < 0, "excessive scanline darkness rejected");
    invalid = s; invalid.crtGlow = -1;
    CHECK(PortSettingsSave(&invalid, error, sizeof(error)) < 0, "negative glow rejected");
    invalid = s; invalid.crtMask = 2;
    CHECK(PortSettingsSave(&invalid, error, sizeof(error)) < 0, "invalid mask rejected");
    CHECK(!PortSettingsLoad(&loaded, error, sizeof(error)) && !memcmp(&s, &loaded, sizeof(s)), "invalid CRT edits preserve prior file");
    PortControls controls; PortControlsOpen(&controls, &s);
    PortControlsClose(&controls); // Keyboard-only assertions must not sample a user-held physical pad.
    bool keyboard[SDL_SCANCODE_COUNT] = {0};
    keyboard[SDL_SCANCODE_C] = true;
    CHECK(PortControlsPressed(&controls, keyboard) & 1, "new keyboard binding reaches GBA A bit");
    keyboard[SDL_SCANCODE_C] = false; keyboard[SDL_SCANCODE_X] = true;
    CHECK(!(PortControlsPressed(&controls, keyboard) & 1), "old binding released");
    keyboard[SDL_SCANCODE_LEFT] = keyboard[SDL_SCANCODE_RIGHT] = true;
    CHECK(!(PortControlsPressed(&controls, keyboard) & 0x30), "opposite directions filtered");
    memset(keyboard, 0, sizeof(keyboard));
    SDL_VirtualJoystickDesc description;
    SDL_INIT_INTERFACE(&description);
    description.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    description.naxes = SDL_GAMEPAD_AXIS_COUNT;
    description.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
    description.button_mask = (1u << SDL_GAMEPAD_BUTTON_COUNT) - 1;
    description.axis_mask = (1u << SDL_GAMEPAD_AXIS_COUNT) - 1;
    description.name = "WL4 virtual Xbox-style test (NOT hardware)";
    SDL_JoystickID id = SDL_AttachVirtualJoystick(&description);
    CHECK(id != 0, "attach virtual controller");
    SDL_Joystick *joystick = SDL_OpenJoystick(id);
    CHECK(joystick != NULL, "open virtual joystick");
    Pump(&controls);
    // Do not depend on a physical device being first, if one is connected.
    PortControlsClose(&controls); controls.gamepad = SDL_OpenGamepad(id);
    CHECK(controls.gamepad != NULL, "virtual device recognized through SDL gamepad API");
    if (controls.gamepad && joystick) {
        CHECK(SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFT_TRIGGER, -32768), "release virtual LT");
        CHECK(SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, -32768), "release virtual RT");
        Pump(&controls);
        CHECK(!(PortControlsPressed(&controls, keyboard) & 1), "released trigger is neutral");
        SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, 32767); Pump(&controls);
        CHECK(PortControlsPressed(&controls, keyboard) & 1, "remapped trigger reaches GBA A");
        SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, -32768);
        SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_EAST, true); Pump(&controls);
        CHECK(PortControlsPressed(&controls, keyboard) & 2, "face button reaches GBA B");
        SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_EAST, false);
        SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_DPAD_UP, true); Pump(&controls);
        CHECK(PortControlsPressed(&controls, keyboard) & 0x40, "D-pad movement");
        SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_DPAD_UP, false);
        SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFTX, 5000); Pump(&controls);
        CHECK(!(PortControlsPressed(&controls, keyboard) & 0x30), "stick below configurable deadzone");
        SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFTX, 25000); Pump(&controls);
        CHECK(PortControlsPressed(&controls, keyboard) & 0x10, "stick above deadzone");
        controls.settings.analog = 0;
        CHECK(!(PortControlsPressed(&controls, keyboard) & 0x30), "optional stick disabled");
    }
    CHECK(!PortControlsRumble(&controls, 40000, 20000, 100), "disabled rumble produces no effect");
    controls.settings.rumble = 1;
    CHECK(!PortControlsRumble(&controls, 40000, 20000, 100), "unsupported rumble is nonfatal");
    SDL_CloseJoystick(joystick);
    CHECK(SDL_DetachVirtualJoystick(id), "disconnect virtual controller"); Pump(&controls);
    CHECK(!controls.gamepad || SDL_GetGamepadID(controls.gamepad) != id, "removed device released without stuck state");
    keyboard[SDL_SCANCODE_C] = true;
    CHECK(PortControlsPressed(&controls, keyboard) & 1, "keyboard survives disconnect");
    description.Rumble = Rumble;
    id = SDL_AttachVirtualJoystick(&description); Pump(&controls);
    CHECK(controls.gamepad != NULL, "reconnect opens an available gamepad");
    if (count == 0) CHECK(SDL_GetGamepadID(controls.gamepad) == id, "reconnected virtual controller selected");
    PortControlsClose(&controls); controls.gamepad = SDL_OpenGamepad(id);
    controls.settings.rumble = 1; controls.settings.rumbleStrength = 50;
    PortRumbleObserver observer = {0};
    PortRumbleObserve(&observer, &controls, 1, 8, 0, 0, 0, 0, 0, 1000);
    rumbleCalls = 0;
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 1010);
    CHECK(rumbleCalls == 1 && rumbleLow == 13000 && rumbleHigh == 24000, "damage edge scales original pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 1300);
    CHECK(rumbleCalls == 1, "persistent damage state does not retrigger");
    PortRumbleObserve(&observer, &controls, 1, 7, 48, 0, 0, 0, 0, 1400);
    CHECK(rumbleLow == 22000 && rumbleHigh == 8000, "impact pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 47, 0, 0, 0, 0, 1700);
    CHECK(rumbleCalls == 2, "shake countdown does not retrigger");
    PortRumbleObserve(&observer, &controls, 0, 7, 47, 0, 0, 0, 0, 1800);
    CHECK(!rumbleLow && !rumbleHigh, "leaving gameplay stops vibration");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 1900);
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 1, 0, 0, 2000);
    CHECK(rumbleLow == 19000 && rumbleHigh == 12000, "successful shoulder-bash debris pulse");
    PortControlsRumble(&controls, 40000, 20000, 100);
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 1, 0, 2400);
    CHECK(rumbleLow == 21000 && rumbleHigh == 17000, "boss health loss pulse including shop damage");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_CONTACT, 2700);
    CHECK(rumbleLow == 22000 && rumbleHigh == 13000, "accepted enemy attack contact");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_HEAD, 3000);
    CHECK(rumbleLow == 15000 && rumbleHigh == 13000, "headbutt pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_SWITCH, 3300);
    CHECK(rumbleLow == 22000 && rumbleHigh == 13000, "switch activation pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_FROG, 3310);
    CHECK(observer.frogUntil == 8310 && rumbleLow == 14000 && rumbleHigh == 6000, "frog effect lasts five host seconds despite cooldown");
    int frogCalls = rumbleCalls;
    PortRumbleObserve(&observer, &controls, 1, 7, 80, 0, 1, 1, PORT_RUMBLE_CONTACT, 5000);
    CHECK(rumbleCalls == frogCalls && observer.frogUntil == 8310, "incidental impacts do not restart or shorten frog rumble");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_HEAD, 8310);
    CHECK(rumbleLow == 15000 && rumbleHigh == 13000, "normal feedback resumes at frog deadline");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_FROG, 8500);
    controls.settings.rumble = 0;
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 8510);
    CHECK(!observer.frogUntil && !rumbleLow && !rumbleHigh, "disable interrupts frog effect");
    CHECK(!PortControlsRumble(&controls, 40000, 20000, 100) && !rumbleLow && !rumbleHigh, "disable stops vibration");
    controls.settings.rumble = 1;
    PortControlsRumble(&controls, 40000, 20000, 100);
    // Real SDL host-time expiry on the selected virtual device, never a physical pad.
    Uint64 sent = SDL_GetTicks();
    CHECK(PortControlsRumble(&controls, 28000, 12000, 5000), "SDL accepts five-second virtual effect");
    CHECK(SDL_GetTicks() - sent < 100, "five-second effect submission does not wait for its duration");
    while (SDL_GetTicks() - sent < 5300 && (rumbleLow || rumbleHigh)) {
        SDL_UpdateJoysticks(); SDL_Delay(10);
    }
    CHECK(!rumbleLow && !rumbleHigh && SDL_GetTicks() - sent >= 4900,
        "SDL expires continuous effect near five seconds while events keep pumping");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_FROG, 10000);
    PortRumbleObserve(&observer, &controls, 0, 7, 0, 0, 0, 0, 0, 10001);
    CHECK(!observer.frogUntil && !rumbleLow && !rumbleHigh, "inactive gameplay interrupts long effect");
    controls.settings.rumbleStrength = 0;
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_FROG, 11000);
    CHECK(!observer.frogUntil && !rumbleLow && !rumbleHigh, "zero strength never starts a long effect");
    controls.settings.rumbleStrength = 50;
    PortControlsStopRumble(&controls);
    memset(&observer, 0, sizeof(observer));
    PortRumbleObserve(&observer, &controls, 1, 8, 0, 0, 0, 0, 0, 20000);
    rumbleCalls = 0;
    PortRumbleObserve(&observer, &controls, 1, 7, 48, 0, 0, 0, PORT_RUMBLE_TRANSFORM, 20010);
    CHECK(rumbleCalls == 1 && rumbleLow == 9000 && rumbleHigh == 7000,
        "activation replaces simultaneous damage and shake with one subtle cue");
    PortRumbleObserve(&observer, &controls, 1, 7, 47, 0, 0, 0, 0, 20030);
    CHECK(rumbleCalls == 1, "same activation segment never submits every frame");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 20075);
    CHECK(!rumbleLow && !rumbleHigh, "activation has a quiet gap");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 20130);
    CHECK(rumbleLow == 12000 && rumbleHigh == 5000, "activation second pulse follows host clock");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_RECOVER, 20140);
    CHECK(!observer.sequence.pattern && rumbleLow == 4500 && rumbleHigh == 7000,
        "recovery preempts activation with a single light pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_FIRE, 21000);
    CHECK(observer.sequence.pattern == PORT_PATTERN_FIRE && rumbleLow == 7000 && rumbleHigh == 9000, "fire starts brief pulse train");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 21120);
    CHECK(rumbleLow == 8000 && rumbleHigh == 10000, "fire second pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 21240);
    CHECK(rumbleLow == 9000 && rumbleHigh == 11000, "fire third pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 22000);
    CHECK(!observer.sequence.pattern && !rumbleLow && !rumbleHigh, "stall skips expired segments instead of replaying them");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_ASH, 23000);
    CHECK(rumbleLow == 6000 && rumbleHigh == 12000, "ash recovery distinct first pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 23180);
    CHECK(rumbleLow == 10000 && rumbleHigh == 3000, "ash recovery shifts to low motor");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_SNOW, 24000);
    CHECK(observer.sequence.pattern == PORT_PATTERN_SNOW && rumbleLow == 9000 && rumbleHigh == 4000, "snowball starts one sustained effect");
    int snowCalls = rumbleCalls;
    PortRumbleObserve(&observer, &controls, 1, 7, 48, 0, 0, 0, 0, 25000);
    CHECK(rumbleCalls == snowCalls, "snowball effect not restarted by frames or shakes");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_SNOW_END, 25010);
    CHECK(!observer.sequence.pattern && !rumbleLow && !rumbleHigh, "snowball effect stops when rolling ends");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_TRANSFORM, 26000);
    controls.settings.rumble = 0;
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 26020);
    CHECK(!observer.sequence.pattern && !rumbleLow && !rumbleHigh, "disabled setting cancels remaining sequence segments");
    controls.settings.rumble = 1;
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 26130);
    CHECK(!observer.sequence.pattern && !rumbleLow && !rumbleHigh, "enabling never replays a consumed transformation");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_TRANSFORM, 26500);
    PortRumbleObserve(&observer, &controls, 1, 7, 48, 0, 0, 0, PORT_RUMBLE_FORM_IMPACT, 26520);
    CHECK(!observer.sequence.pattern && rumbleLow == 10000 && rumbleHigh == 6000,
        "real transformation impact preempts activation with a distinct subtle pulse");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_FIRE, 27000);
    PortControlsStopRumble(&controls);
    { PortControls missing = controls; missing.gamepad = NULL;
      PortRumbleObserve(&observer, &missing, 1, 7, 0, 0, 0, 0, 0, 27020);
      CHECK(!observer.sequence.pattern, "disconnect discards the scheduled tail"); }
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, 0, 27120);
    CHECK(!observer.sequence.pattern && !rumbleLow && !rumbleHigh, "reconnect cannot replay scheduled fire pulses");
    PortRumbleObserve(&observer, &controls, 1, 7, 0, 0, 0, 0, PORT_RUMBLE_SNOW, 28000);
    PortRumbleObserve(&observer, &controls, 0, 7, 0, 0, 0, 0, 0, 28020);
    CHECK(!observer.sequence.pattern && !rumbleLow && !rumbleHigh, "inactive gameplay cancels sustained snowball feedback");
    PortControlsRumble(&controls, 28000, 12000, 5000);
    PortControlsClose(&controls);
    CHECK(!rumbleLow && !rumbleHigh, "close stops vibration");
    SDL_DetachVirtualJoystick(id);
    SDL_Quit();
    printf("Settings/keyboard/virtual-gamepad checks: %s\n", failures ? "FAILED" : "PASS");
    return failures ? 1 : 0;
}

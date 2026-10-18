// The SDL3 frontend: a window showing the display, keyboard and gamepad input, and frame
// pacing at the GBA's rate. Sound and saves come later (M2).

#include "port/sdl_frontend.h"

#include <stdint.h>
#include <stdio.h>

#include <SDL3/SDL.h>

#include "port/frame.h"
#include "port/gba_io.h"
#include "port/ppu.h"

#define REG_KEYINPUT 0x130

// KEYINPUT bits; a pressed button reads as 0.
#define KEY_A 0x001
#define KEY_B 0x002
#define KEY_SELECT 0x004
#define KEY_START 0x008
#define KEY_RIGHT 0x010
#define KEY_LEFT 0x020
#define KEY_UP 0x040
#define KEY_DOWN 0x080
#define KEY_R 0x100
#define KEY_L 0x200
#define KEYS_ALL 0x3FF

// A GBA frame: 228 lines of 1232 cycles at 2^24 Hz, about 59.73 frames a second.
#define FRAME_NS (280896ull * 1000000000ull / 16777216ull)
#define STICK_THRESHOLD 16384

// mGBA's default keyboard layout.
static const struct {
    SDL_Scancode scancode;
    uint16_t key;
} kKeyboard[] = {
    { SDL_SCANCODE_X, KEY_A },          { SDL_SCANCODE_Z, KEY_B },
    { SDL_SCANCODE_BACKSPACE, KEY_SELECT }, { SDL_SCANCODE_RETURN, KEY_START },
    { SDL_SCANCODE_RIGHT, KEY_RIGHT },  { SDL_SCANCODE_LEFT, KEY_LEFT },
    { SDL_SCANCODE_UP, KEY_UP },        { SDL_SCANCODE_DOWN, KEY_DOWN },
    { SDL_SCANCODE_S, KEY_R },          { SDL_SCANCODE_A, KEY_L },
};

// Gamepad buttons by position: the bottom face button is A, as on mGBA with an Xbox layout.
static const struct {
    SDL_GamepadButton button;
    uint16_t key;
} kGamepad[] = {
    { SDL_GAMEPAD_BUTTON_SOUTH, KEY_A },          { SDL_GAMEPAD_BUTTON_EAST, KEY_B },
    { SDL_GAMEPAD_BUTTON_BACK, KEY_SELECT },      { SDL_GAMEPAD_BUTTON_START, KEY_START },
    { SDL_GAMEPAD_BUTTON_DPAD_RIGHT, KEY_RIGHT }, { SDL_GAMEPAD_BUTTON_DPAD_LEFT, KEY_LEFT },
    { SDL_GAMEPAD_BUTTON_DPAD_UP, KEY_UP },       { SDL_GAMEPAD_BUTTON_DPAD_DOWN, KEY_DOWN },
    { SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, KEY_R }, { SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, KEY_L },
};

static SDL_Gamepad *sGamepad;

static uint16_t PressedKeys(void)
{
    const bool *keyboard = SDL_GetKeyboardState(NULL);
    uint16_t keys = 0;
    unsigned int i;

    for (i = 0; i < SDL_arraysize(kKeyboard); i++)
        if (keyboard[kKeyboard[i].scancode])
            keys |= kKeyboard[i].key;
    if (sGamepad != NULL) {
        Sint16 x = SDL_GetGamepadAxis(sGamepad, SDL_GAMEPAD_AXIS_LEFTX);
        Sint16 y = SDL_GetGamepadAxis(sGamepad, SDL_GAMEPAD_AXIS_LEFTY);

        for (i = 0; i < SDL_arraysize(kGamepad); i++)
            if (SDL_GetGamepadButton(sGamepad, kGamepad[i].button))
                keys |= kGamepad[i].key;
        if (x > STICK_THRESHOLD)
            keys |= KEY_RIGHT;
        else if (x < -STICK_THRESHOLD)
            keys |= KEY_LEFT;
        if (y > STICK_THRESHOLD)
            keys |= KEY_DOWN;
        else if (y < -STICK_THRESHOLD)
            keys |= KEY_UP;
    }
    // The D-pad cannot press opposite directions at once.
    if ((keys & (KEY_LEFT | KEY_RIGHT)) == (KEY_LEFT | KEY_RIGHT))
        keys &= ~(KEY_LEFT | KEY_RIGHT);
    if ((keys & (KEY_UP | KEY_DOWN)) == (KEY_UP | KEY_DOWN))
        keys &= ~(KEY_UP | KEY_DOWN);
    return keys;
}

static void OpenFirstGamepad(void)
{
    SDL_JoystickID *ids;
    int count;

    if (sGamepad != NULL)
        return;
    ids = SDL_GetGamepads(&count);
    if (ids != NULL && count > 0)
        sGamepad = SDL_OpenGamepad(ids[0]);
    SDL_free(ids);
}

int PortSdlRun(void (*entry)(void), unsigned int scale)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    Uint64 next;
    int result = 1, running = 1, fastForward = 0;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (!SDL_CreateWindowAndRenderer("Wario Land 4", (int)(PORT_SCREEN_WIDTH * scale), (int)(PORT_SCREEN_HEIGHT * scale),
                                     SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        fprintf(stderr, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
        goto done;
    }
    // Whole-number scaling with black bars keeps the pixels square and sharp.
    SDL_SetRenderLogicalPresentation(renderer, PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XBGR8888, SDL_TEXTUREACCESS_STREAMING, PORT_SCREEN_WIDTH,
                                PORT_SCREEN_HEIGHT);
    if (texture == NULL) {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        goto done;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    OpenFirstGamepad();

    if (PortFrameStart(entry))
        goto done;
    next = SDL_GetTicksNS();
    while (running) {
        SDL_Event event;
        Uint64 now;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.scancode == SDL_SCANCODE_ESCAPE)
                    running = 0;
                else if (event.key.scancode == SDL_SCANCODE_F11)
                    SDL_SetWindowFullscreen(window, !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
                else if (event.key.scancode == SDL_SCANCODE_TAB)
                    fastForward = 1;
                break;
            case SDL_EVENT_KEY_UP:
                if (event.key.scancode == SDL_SCANCODE_TAB)
                    fastForward = 0;
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
                OpenFirstGamepad();
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                if (sGamepad != NULL && event.gdevice.which == SDL_GetGamepadID(sGamepad)) {
                    SDL_CloseGamepad(sGamepad);
                    sGamepad = NULL;
                    OpenFirstGamepad();
                }
                break;
            default:
                break;
            }
        }

        // The game reads the buttons after VBlank, when the frame below resumes it.
        PortIoWrite16(REG_KEYINPUT, (uint16_t)(~PressedKeys() & KEYS_ALL));
        if (!PortFrameRun()) {
            fprintf(stderr, "The game stopped after %u frames\n", PortFrameCount());
            break;
        }

        SDL_UpdateTexture(texture, NULL, PortPpuFrame(), PORT_SCREEN_WIDTH * 4);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Pace to the GBA's frame rate. After a stall (a dragged window, a breakpoint) start
        // again from now rather than rushing to catch up.
        next += FRAME_NS;
        now = SDL_GetTicksNS();
        if (fastForward || now > next + 4 * FRAME_NS)
            next = now;
        else if (next > now)
            SDL_DelayPrecise(next - now);
    }
    result = 0;

done:
    if (sGamepad != NULL)
        SDL_CloseGamepad(sGamepad);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}

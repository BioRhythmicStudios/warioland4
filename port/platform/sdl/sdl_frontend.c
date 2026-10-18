// The SDL3 frontend: a window showing the display, keyboard and gamepad input, sound, saves,
// and frame pacing at the GBA's rate.

#include "port/sdl_frontend.h"

#include <stdint.h>
#include <stdio.h>

#include <SDL3/SDL.h>

#include "port/audio.h"
#include "port/frame.h"
#include "port/gba_io.h"
#include "port/input_script.h"
#include "port/os.h"
#include "port/ppu.h"
#include "port/save.h"

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

// The sound kept queued for the device: about 4 frames.
#define AUDIO_TARGET_FRAMES (PORT_AUDIO_RATE / 15)
// How far the stream's speed may move from 1 to keep that much queued.
#define AUDIO_MAX_ADJUST 0.005f

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
static SDL_AudioStream *sAudio;

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

static void OpenAudio(void)
{
    const SDL_AudioSpec spec = { SDL_AUDIO_S16, 2, PORT_AUDIO_RATE };

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)
        || (sAudio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL)) == NULL) {
        fprintf(stderr, "No sound: %s\n", SDL_GetError());
        return;
    }
    SDL_ResumeAudioStreamDevice(sAudio);
}

// Queues the sound of the frame just run. The frames are paced by the system clock, which
// drifts from the sound device's, so the stream plays a little faster or slower to keep about
// AUDIO_TARGET_FRAMES queued. After a stall it starts again from silence; fast forward drops
// the sound.
static void QueueAudio(int fastForward)
{
    int16_t frames[2048 * 2];
    size_t n;
    int queued;
    float error;

    while ((n = PortAudioTake(frames, 2048)) != 0)
        if (sAudio != NULL && !fastForward)
            SDL_PutAudioStreamData(sAudio, frames, (int)(n * sizeof(int16_t) * 2));
    if (sAudio == NULL)
        return;
    queued = SDL_GetAudioStreamQueued(sAudio) / (int)(sizeof(int16_t) * 2);
    if (queued > 4 * AUDIO_TARGET_FRAMES) {
        SDL_ClearAudioStream(sAudio);
        queued = 0;
    }
    if (queued < AUDIO_TARGET_FRAMES / 2) {
        static const int16_t kSilence[AUDIO_TARGET_FRAMES * 2];
        SDL_PutAudioStreamData(sAudio, kSilence, (AUDIO_TARGET_FRAMES - queued) * (int)(sizeof(int16_t) * 2));
        queued = AUDIO_TARGET_FRAMES;
    }
    error = (float)(queued - AUDIO_TARGET_FRAMES) / AUDIO_TARGET_FRAMES;
    error = error > 1 ? 1 : error < -1 ? -1 : error;
    SDL_SetAudioStreamFrequencyRatio(sAudio, 1 + AUDIO_MAX_ADJUST * error);
}

// Loads the save from savePath, or from wl4.sav in the user's preference directory.
static int OpenSave(const char *savePath)
{
    char path[1024];
    char *directory;
    int result;

    if (savePath != NULL)
        return PortSaveOpen(savePath);
    directory = SDL_GetPrefPath(NULL, "wl4");
    if (directory == NULL) {
        char message[512];
        snprintf(message, sizeof(message), "Cannot locate the save folder: %s\n", SDL_GetError());
        PortOsWriteError(message);
        return -1;
    }
    if (snprintf(path, sizeof(path), "%swl4.sav", directory) >= (int)sizeof(path)) {
        PortOsWriteError("Cannot open the save: the preference-directory path is too long.\n");
        result = -1;
    } else {
        result = PortSaveOpen(path);
    }
    SDL_free(directory);
    return result;
}

// Normal save failures must not open dialogs every frame or stop the game. The banner
// remains visible in fullscreen too; F8 and the quit prompt provide the full path/reason.
static void DrawSaveWarning(SDL_Window *window, SDL_Renderer *renderer, int *wasFailed)
{
    int failed = PortSaveError() != NULL;
    if (failed != *wasFailed) {
        char title[1152];
        if (failed)
            snprintf(title, sizeof(title), "Wario Land 4 - UNSAVED: %s", PortSavePath());
        else
            snprintf(title, sizeof(title), "Wario Land 4");
        SDL_SetWindowTitle(window, title);
        *wasFailed = failed;
    }
    if (failed) {
        const SDL_FRect banner = { 0, PORT_SCREEN_HEIGHT - 24, PORT_SCREEN_WIDTH, 24 };
        SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
        SDL_RenderFillRect(renderer, &banner);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer, 0, banner.y, "SAVE FAILED - retrying");
        SDL_RenderDebugText(renderer, 0, banner.y + 8, "Progress remains in memory");
        SDL_RenderDebugText(renderer, 0, banner.y + 16, "F8: details");
    }
}

// 1: saved, 0: return to game (also the safe default if the dialog fails), -1: explicitly
// quit without saving. A failed close keeps the save session/SRAM open for another attempt.
static int FinishSave(SDL_Window *window)
{
    static const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Return to game" },
        { 0, 1, "Retry saving" },
        { 0, 2, "Quit without saving" },
    };
    while (PortSaveClose()) {
        char message[2048];
        int choice = 0;
        SDL_MessageBoxData box = { SDL_MESSAGEBOX_WARNING, window, "Progress has not been saved",
                                   message, 3, buttons, NULL };
        snprintf(message, sizeof(message), "%s\n\nRetry saving, return to the game, or quit and lose unsaved progress.",
                 PortSaveError());
        if (!SDL_ShowMessageBox(&box, &choice)) {
            PortOsWriteError(message);
            return 0;
        }
        if (choice == 2)
            return -1;
        if (choice != 1)
            return 0;
    }
    return 1;
}

int PortSdlRun(void (*entry)(void), unsigned int scale, int scripted, const char *savePath)
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    Uint64 next;
    int result = 1, running = 1, fastForward = 0;
    int wasSaveFailed = 0, saveHandled = 0;

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
    OpenAudio();

    if (OpenSave(savePath) || PortFrameStart(entry))
        goto done;
    next = SDL_GetTicksNS();
    while (running) {
        SDL_Event event;
        Uint64 now;
        uint16_t keys;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.scancode == SDL_SCANCODE_ESCAPE)
                    running = 0;
                else if (event.key.scancode == SDL_SCANCODE_F8 && !event.key.repeat && PortSaveError())
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Progress has not been saved", PortSaveError(), window);
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

        if (!running) {
            int saved = FinishSave(window);
            if (saved) {
                saveHandled = 1;
                result = saved > 0 ? 0 : 1;
                goto done;
            }
            running = 1;
            next = SDL_GetTicksNS();
        }

        // The game reads the buttons after VBlank, when the frame below resumes it.
        keys = PressedKeys();
        if (scripted)
            keys |= PortInputScriptKeys(PortFrameSteps() + 1);
        PortIoWrite16(REG_KEYINPUT, (uint16_t)(~keys & KEYS_ALL));
        if (!PortFrameRun()) {
            fprintf(stderr, "The game stopped after %u frames\n", PortFrameCount());
            goto done;
        }
        PortSaveFrame();

        QueueAudio(fastForward);
        SDL_UpdateTexture(texture, NULL, PortPpuFrame(), PORT_SCREEN_WIDTH * 4);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        DrawSaveWarning(window, renderer, &wasSaveFailed);
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
    if (!saveHandled && PortSaveClose()) {
        PortOsWriteError(PortSaveError());
        result = 1;
    }
    SDL_DestroyAudioStream(sAudio);
    sAudio = NULL;
    if (sGamepad != NULL)
        SDL_CloseGamepad(sGamepad);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}

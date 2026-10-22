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
#include "port/controls.h"
#include "port/frame_pacer.h"
#include "port/render_vsync.h"
#include "port/crt.h"
#include "port/rumble.h"
#include "port/rumble_snapshot.h"

#define REG_KEYINPUT 0x130

#define KEYS_ALL 0x3FF

// The sound kept queued for the device: about 4 frames.
#define AUDIO_TARGET_FRAMES (PORT_AUDIO_RATE / 15)
// How far the stream's speed may move from 1 to keep that much queued.
#define AUDIO_MAX_ADJUST 0.005f

static SDL_AudioStream *sAudio;
static unsigned int sAudioRefills, sAudioDrops;

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
        ++sAudioDrops;
        SDL_ClearAudioStream(sAudio);
        queued = 0;
    }
    if (queued < AUDIO_TARGET_FRAMES / 2) {
        ++sAudioRefills;
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
    PortFramePacer pacer = {0};
    PortCrt crt = {0};
    PortRumbleObserver rumble = {0};
    Uint64 started = 0, presents = 0, repeats = 0, maxPresent = 0;
    int timingStats = SDL_getenv("WL4_TIMING_STATS") != NULL;
    int traceInput = SDL_getenv("WL4_DIAG_EVENTS") != NULL;
    uint16_t lastKeys = 0;
    unsigned eventSequence = 0;
    unsigned traceFrames = 0;
    int requestedVSync = -1, reapplyVSync = 1;
    int result = 1, running = 1, fastForward = 0;
    int wasSaveFailed = 0, saveHandled = 0;
    PortSettings settings;
    PortControls controls = {0};
    char settingsError[256];
    int loaded = PortSettingsLoad(&settings, settingsError, sizeof(settingsError));
    if (scale) settings.scale = (int)scale;
    char settingsPath[4096];
    if (!PortSettingsPath(settingsPath, sizeof(settingsPath)))
        printf("Settings: %s (A=%s, scale=%d, fullscreen=%d, vsync=%d)\n", settingsPath,
               SDL_GetScancodeName((SDL_Scancode)settings.keys[0]), settings.scale, settings.fullscreen, settings.vsync);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (traceInput)
        fprintf(stderr, "SDL video backend: %s; Steam environment: %s\n",
                SDL_GetCurrentVideoDriver(), SDL_getenv("SteamAppId") ? "present" : "absent");
    printf("Display filter: %s; scanlines=%d%% glow=%d%% mask=%d\n", PortCrtName(settings.crtPreset),
           settings.crtScanlines, settings.crtGlow, settings.crtMask);
    if (loaded < 0) {
        fprintf(stderr, "%s\n", settingsError);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Settings reset to defaults", settingsError, NULL);
    }
    if (!SDL_CreateWindowAndRenderer("Wario Land 4", (int)(PORT_SCREEN_WIDTH * settings.scale), (int)(PORT_SCREEN_HEIGHT * settings.scale),
                                     SDL_WINDOW_RESIZABLE | (settings.fullscreen ? SDL_WINDOW_FULLSCREEN : 0), &window, &renderer)) {
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
    sAudioRefills = sAudioDrops = 0;
    PortControlsOpen(&controls, &settings);
    if (traceInput)
        fprintf(stderr, "SDL gamepad at startup: %s\n", controls.gamepad ? "present" : "none");
    OpenAudio();

    if (OpenSave(savePath) || PortFrameStart(entry))
        goto done;
    pacer.next = started = SDL_GetTicksNS();
    while (running) {
        SDL_Event event;
        Uint64 now;
        uint16_t keys;

        while (SDL_PollEvent(&event)) {
            if (traceInput) {
                ++eventSequence;
                switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                    fprintf(stderr, "SDL event #%u frame=%u keyboard type=0x%x scancode=%d (%s) repeat=%d\n",
                            eventSequence, PortFrameCount(), (unsigned)event.type,
                            event.key.scancode, SDL_GetScancodeName(event.key.scancode), event.key.repeat);
                    traceFrames = 120;
                    break;
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                    fprintf(stderr, "SDL event #%u frame=%u gamepad type=0x%x button=%u\n",
                            eventSequence, PortFrameCount(), (unsigned)event.type, event.gbutton.button);
                    traceFrames = 120;
                    break;
                case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
                case SDL_EVENT_JOYSTICK_BUTTON_UP:
                    fprintf(stderr, "SDL event #%u frame=%u joystick type=0x%x button=%u\n",
                            eventSequence, PortFrameCount(), (unsigned)event.type, event.jbutton.button);
                    traceFrames = 120;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    fprintf(stderr, "SDL event #%u frame=%u mouse type=0x%x button=%u\n",
                            eventSequence, PortFrameCount(), (unsigned)event.type, event.button.button);
                    traceFrames = 120;
                    break;
                case SDL_EVENT_FINGER_DOWN:
                case SDL_EVENT_FINGER_UP:
                    fprintf(stderr, "SDL event #%u frame=%u touch type=0x%x\n",
                            eventSequence, PortFrameCount(), (unsigned)event.type);
                    traceFrames = 120;
                    break;
                case SDL_EVENT_GAMEPAD_ADDED:
                case SDL_EVENT_GAMEPAD_REMOVED:
                case SDL_EVENT_GAMEPAD_REMAPPED:
                    fprintf(stderr, "SDL event #%u frame=%u gamepad-device type=0x%x\n",
                            eventSequence, PortFrameCount(), (unsigned)event.type);
                    break;
                case SDL_EVENT_QUIT:
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                case SDL_EVENT_TERMINATING:
                    fprintf(stderr, "SDL event #%u frame=%u lifecycle type=0x%x\n",
                            eventSequence, PortFrameCount(), (unsigned)event.type);
                    break;
                default:
                    break;
                }
                fflush(stderr);
            }
            PortControlsEvent(&controls, &event);
            switch (event.type) {
            case SDL_EVENT_QUIT:
                if (traceInput)
                    fprintf(stderr, "SDL exit reason: SDL_EVENT_QUIT at frame %u\n",
                            PortFrameCount());
                running = 0;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    if (traceInput)
                        fprintf(stderr, "SDL exit reason: Escape keydown at frame %u\n",
                                PortFrameCount());
                    running = 0;
                }
                else if (event.key.scancode == SDL_SCANCODE_F8 && !event.key.repeat && PortSaveError())
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Progress has not been saved", PortSaveError(), window);
                else if (event.key.scancode == SDL_SCANCODE_F11 && !event.key.repeat) {
                    if (!SDL_SetWindowFullscreen(window, !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN)))
                        fprintf(stderr, "Cannot toggle fullscreen: %s\n", SDL_GetError());
                }
                else if (event.key.scancode == SDL_SCANCODE_TAB)
                    fastForward = 1;
                break;
            case SDL_EVENT_KEY_UP:
                if (event.key.scancode == SDL_SCANCODE_TAB)
                    fastForward = 0;
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                fastForward = 0;
                break;
            case SDL_EVENT_RENDER_DEVICE_RESET:
            case SDL_EVENT_RENDER_TARGETS_RESET:
                PortCrtDestroy(&crt);
                break;
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
                reapplyVSync = 1;
                printf("Display: fullscreen=%d\n", !!(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
                break;
            default:
                break;
            }
        }

        if (!running) {
            PortControlsStopRumble(&controls);
            int saved = FinishSave(window);
            if (saved) {
                saveHandled = 1;
                result = saved > 0 ? 0 : 1;
                goto done;
            }
            running = 1;
            pacer.next = SDL_GetTicksNS();
        }

        int wantedVSync = settings.vsync && !fastForward;
        if (reapplyVSync || wantedVSync != requestedVSync) {
            PortApplyRenderVSync(renderer, wantedVSync);
            requestedVSync = wantedVSync;
            reapplyVSync = 0;
        }
        unsigned int frames = PortFramePacerDue(&pacer, SDL_GetTicksNS(), fastForward);
        if (!frames) {
            now = SDL_GetTicksNS();
            if (pacer.next > now) SDL_DelayPrecise(pacer.next - now);
            continue;
        }
        for (unsigned int frame = 0; frame < frames; ++frame) {
            // The game reads the buttons after VBlank, when the frame below resumes it.
            if (traceInput && traceFrames)
                fprintf(stderr, "SDL controls poll begin frame=%u gamepad=%s\n",
                        PortFrameCount(), controls.gamepad ? "present" : "none");
            keys = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS)
                       ? PortControlsPressed(&controls, SDL_GetKeyboardState(NULL)) : 0;
            if (scripted)
                keys |= PortInputScriptKeys(PortFrameSteps() + 1);
            if (traceInput && keys != lastKeys) {
                fprintf(stderr, "SDL input frame=%u GBA keys=0x%03x\n",
                        PortFrameCount(), keys);
                lastKeys = keys;
            }
            if (traceInput && traceFrames)
                fprintf(stderr, "SDL controls poll end frame=%u keys=0x%03x; game frame begin\n",
                        PortFrameCount(), keys);
            PortIoWrite16(REG_KEYINPUT, (uint16_t)(~keys & KEYS_ALL));
            if (!PortFrameRun()) {
                fprintf(stderr, "The game stopped after %u frames\n", PortFrameCount());
                goto done;
            }
            if (traceInput && traceFrames) {
                fprintf(stderr, "SDL game frame end frame=%u\n", PortFrameCount());
                --traceFrames;
            }
            PortSaveFrame();
            PortRumbleSnapshot snapshot;
            PortRumbleRead(&snapshot);
            PortRumbleObserve(&rumble, &controls, snapshot.active
                    && !fastForward && (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS),
                snapshot.health, snapshot.shakeX, snapshot.shakeY, snapshot.rock, snapshot.boss, snapshot.events, SDL_GetTicks());

            QueueAudio(fastForward);
        }
        // Present only a newly simulated framebuffer, once after any catch-up.
        // The display/compositor retains it between GBA frames; no redundant
        // upload/present loop is needed at higher refresh rates.
        SDL_UpdateTexture(texture, NULL, PortPpuFrame(), PORT_SCREEN_WIDTH * 4);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        if (settings.crtPreset && !PortCrtDraw(&crt, renderer, PortPpuFrame(), &settings)) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
        }
        DrawSaveWarning(window, renderer, &wasSaveFailed);
        Uint64 beforePresent = SDL_GetTicksNS();
        if (!SDL_RenderPresent(renderer)) {
            fprintf(stderr, "SDL_RenderPresent: %s\n", SDL_GetError());
            goto done;
        }
        now = SDL_GetTicksNS();
        if (timingStats) {
            ++presents;
            if (!frames) ++repeats;
            if (now - beforePresent > maxPresent) maxPresent = now - beforePresent;
        }
        // Wait only for the unelapsed absolute GBA deadline, never a full period
        // added to present time. If present blocked past it, the next loop catches
        // up simulation/audio on the same clock, independently of monitor Hz.
        if (!fastForward && pacer.next > now) SDL_DelayPrecise(pacer.next - now);
    }
    result = 0;

done:
    if (traceInput)
        fprintf(stderr, "SDL shutdown: result=%d frame=%u\n", result, PortFrameCount());
    PortControlsStopRumble(&controls);
    if (timingStats && started) {
        printf("Timing: seconds=%.3f frames=%u presents=%llu repeats=%llu max_present_ms=%.3f clock_resets=%u audio_refills=%u audio_drops=%u\n",
               (SDL_GetTicksNS() - started) / 1e9, PortFrameCount(), (unsigned long long)presents,
               (unsigned long long)repeats, maxPresent / 1e6, pacer.resets, sAudioRefills, sAudioDrops);
    }
    if (!saveHandled && PortSaveClose()) {
        PortOsWriteError(PortSaveError());
        result = 1;
    }
    SDL_DestroyAudioStream(sAudio);
    sAudio = NULL;
    PortControlsClose(&controls);
    PortCrtDestroy(&crt);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}

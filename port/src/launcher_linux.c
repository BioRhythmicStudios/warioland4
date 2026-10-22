// SDL3 front window for the Linux package. The game owns its save and runtime.
#include <SDL3/SDL.h>
#include <SDL3/SDL_dialog.h>
#include "port/settings.h"
#include "port/rom_validate.h"
#include "port/gba_memory.h"
#include <errno.h>
#include <limits.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

enum { UI_W = 450, UI_H = 240, BROWSE = 0, SCALE_DOWN, SCALE_UP, SETTINGS, LAUNCH, QUIT, BUTTONS };
static const SDL_FRect boxes[BUTTONS] = {
    {345, 97, 92, 22}, {139, 145, 32, 22}, {181, 145, 32, 22},
    {18, 197, 98, 27}, {283, 190, 154, 34}, {128, 197, 77, 27}
};
static const char *const labels[BUTTONS] = {"BROWSE", "-", "+", "SETTINGS", "LAUNCH GAME", "QUIT"};
static char root[PATH_MAX], rom[PATH_MAX], message[180] = "Choose your own supported US ROM.";
static PortSettings settings;
static SDL_Window *window;
static SDL_Renderer *renderer;
static int focus = LAUNCH, running = 1;

static void status(const char *value) { SDL_strlcpy(message, value, sizeof(message)); }
static void path_for(char *out, size_t size, const char *suffix)
{ snprintf(out, size, "%s/%s", root, suffix); }

static void data_path_for(char *out, size_t size, const char *name)
{
    const char *dir = getenv("WL4_USERDATA_DIR");
    if (dir && *dir) snprintf(out, size, "%s/%s", dir, name);
    else snprintf(out, size, "%s/userdata/%s", root, name);
}

static int validate_rom(const char *path)
{
    unsigned char *bytes;
    char error[240];
    FILE *file = fopen(path, "rb");
    size_t count;
    int ok;
    if (!file) { status("ROM is missing or unreadable."); return 0; }
    bytes = malloc(GBA_ROM_SIZE);
    if (!bytes) { fclose(file); status("Not enough memory to validate the ROM."); return 0; }
    count = fread(bytes, 1, GBA_ROM_SIZE, file);
    ok = count == GBA_ROM_SIZE && fgetc(file) == EOF && !ferror(file)
         && PortValidateRom(bytes, count, error, sizeof(error)) == 0;
    if (!ok) status(count != GBA_ROM_SIZE ? "ROM must be exactly 8 MiB." :
                    ferror(file) ? "Cannot read the complete ROM." :
                    "Unsupported ROM. Choose the unmodified US release.");
    free(bytes); fclose(file);
    return ok;
}

static void remember_rom(void)
{
    char path[PATH_MAX];
    data_path_for(path, sizeof(path), "launcher_rom.txt");
    FILE *file = fopen(path, "w");
    if (file) { fprintf(file, "%s\n", rom); fclose(file); }
}

static void choose_rom(const char *path)
{
    if (!path || !*path || strlen(path) >= sizeof(rom)) return;
    if (!validate_rom(path)) return;
    SDL_strlcpy(rom, path, sizeof(rom));
    remember_rom();
    status("Supported US ROM selected. Ready to launch.");
}

static void dialog_done(void *userdata, const char *const *files, int filter)
{
    SDL_Event event = {0};
    (void)userdata; (void)filter;
    event.type = SDL_EVENT_USER;
    event.user.code = files == NULL ? -1 : 0;
    if (files && files[0]) event.user.data1 = SDL_strdup(files[0]);
    if (!SDL_PushEvent(&event)) SDL_free(event.user.data1);
}

static int create_window(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 0;
    }
    if (!SDL_CreateWindowAndRenderer("Wario Land 4 - Native PC Port", UI_W * 2, UI_H * 2,
                                     0, &window, &renderer)) {
        fprintf(stderr, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
        SDL_Quit(); return 0;
    }
    SDL_SetRenderScale(renderer, 2, 2);
    SDL_SetRenderVSync(renderer, 1);
    return 1;
}

static void close_window(void)
{
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window);
    renderer = NULL; window = NULL; SDL_Quit();
}

static void text(float x, float y, const char *value, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDebugText(renderer, x, y, value);
}

static void draw(void)
{
    char line[220], file[90];
    SDL_SetRenderDrawColor(renderer, 40, 17, 66, 255);
    SDL_RenderClear(renderer);
    SDL_FRect panel = {9, 9, 432, 224};
    SDL_SetRenderDrawColor(renderer, 61, 28, 91, 255);
    SDL_RenderFillRect(renderer, &panel);
    text(19, 20, "WARIO LAND 4", 255, 204, 64);
    text(19, 36, "NATIVE PC PORT", 249, 235, 255);
    text(19, 52, "BioRhythmicStudios", 249, 235, 255);
    text(19, 78, "YOUR ROM  /  UNMODIFIED US RELEASE (AWAE)", 249, 235, 255);
    if (rom[0]) {
        const char *name = strrchr(rom, '/'); name = name ? name + 1 : rom;
        snprintf(file, sizeof(file), "%.74s", name);
    } else SDL_strlcpy(file, "No ROM selected", sizeof(file));
    SDL_FRect field = {18, 97, 318, 22};
    SDL_SetRenderDrawColor(renderer, 28, 10, 47, 255);
    SDL_RenderFillRect(renderer, &field);
    text(24, 104, file, 249, 235, 255);
    text(19, 134, "WINDOW SCALE", 249, 235, 255);
    snprintf(line, sizeof(line), "%dx", settings.scale);
    text(223, 152, line, 255, 204, 64);
    text(263, 151, "F11: FULLSCREEN IN GAME", 249, 235, 255);
    text(19, 174, message, 249, 235, 255);
    for (int i = 0; i < BUTTONS; ++i) {
        SDL_SetRenderDrawColor(renderer, i == focus ? 255 : 227, i == focus ? 204 : 170,
                               i == focus ? 64 : 57, 255);
        SDL_RenderFillRect(renderer, &boxes[i]);
        text(boxes[i].x + 6, boxes[i].y + (boxes[i].h - 8) / 2, labels[i], 40, 17, 66);
    }
    SDL_RenderPresent(renderer);
}

static void launch_game(void)
{
    char script[PATH_MAX];
    pid_t child;
    int result, wait_status = 0;
    pid_t waited;
    if (!rom[0] || !validate_rom(rom)) return;
    settings.scale = SDL_clamp(settings.scale, 1, 6);
    char error[180];
    if (PortSettingsSave(&settings, error, sizeof(error)) < 0) {
        status("Cannot save settings; game not launched."); return;
    }
    path_for(script, sizeof(script), "play.sh");
    char *args[] = {script, rom, NULL};
    SDL_HideWindow(window);
    result = posix_spawn(&child, script, NULL, NULL, args, environ);
    if (!result) {
        do waited = waitpid(child, &wait_status, 0); while (waited < 0 && errno == EINTR);
        if (waited < 0) status("Could not wait for game process.");
        else if (WIFEXITED(wait_status) && WEXITSTATUS(wait_status) == 0)
            status("Game closed. Ready to launch again.");
        else status("Game exited with an error. Try Diagnostics.");
    } else status("Could not start game. Check package files.");
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
}

static void activate(int id)
{
    char error[180];
    switch (id) {
    case BROWSE: {
        static const SDL_DialogFileFilter filter[] = {{"GBA ROM", "gba"}};
        SDL_ShowOpenFileDialog(dialog_done, NULL, window, filter, 1, NULL, false);
        break;
    }
    case SCALE_DOWN: if (settings.scale > 1) --settings.scale; break;
    case SCALE_UP: if (settings.scale < 6) ++settings.scale; break;
    case SETTINGS:
        close_window();
        if (PortSettingsShow(&settings) == 1) status("Settings saved. Changes apply next game launch.");
        if (!create_window()) running = 0;
        break;
    case LAUNCH: launch_game(); break;
    case QUIT: running = 0; break;
    }
    (void)error;
}

int main(int argc, char **argv)
{
    char path[PATH_MAX], warning[180];
    const char *home;
    FILE *file;
    if (argc < 2 || strlen(argv[1]) >= sizeof(root)) return 2;
    SDL_strlcpy(root, argv[1], sizeof(root));
    if (PortSettingsLoad(&settings, warning, sizeof(warning)) < 0)
        fprintf(stderr, "Settings reset: %s\n", warning);
    data_path_for(path, sizeof(path), "launcher_rom.txt");
    file = fopen(path, "r");
    if (file) {
        if (fgets(rom, sizeof(rom), file)) rom[strcspn(rom, "\r\n")] = 0;
        fclose(file);
        if (rom[0] && !validate_rom(rom)) rom[0] = 0;
    }
    if (argc > 2) choose_rom(argv[2]);
    if (!rom[0] && (home = getenv("HOME"))) {
        snprintf(path, sizeof(path), "%s/Downloads/baserom_us.gba", home);
        if (access(path, R_OK) == 0) choose_rom(path);
    }
    if (!create_window()) return 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
                running = 0;
            else if (event.type == SDL_EVENT_USER) {
                if (event.user.data1) {
                    choose_rom((const char *)event.user.data1);
                    SDL_free(event.user.data1);
                } else if (event.user.code < 0) status("ROM picker unavailable; place ROM in Downloads.");
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                float x = event.button.x / 2, y = event.button.y / 2;
                for (int i = 0; i < BUTTONS; ++i)
                    if (x >= boxes[i].x && y >= boxes[i].y &&
                        x < boxes[i].x + boxes[i].w && y < boxes[i].y + boxes[i].h) {
                        focus = i; activate(i); break;
                    }
            } else if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) running = 0;
                else if (event.key.scancode == SDL_SCANCODE_TAB || event.key.scancode == SDL_SCANCODE_DOWN)
                    focus = (focus + 1) % BUTTONS;
                else if (event.key.scancode == SDL_SCANCODE_UP)
                    focus = (focus + BUTTONS - 1) % BUTTONS;
                else if (event.key.scancode == SDL_SCANCODE_RETURN || event.key.scancode == SDL_SCANCODE_SPACE)
                    activate(focus);
            } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
                if (event.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH) activate(focus);
                else if (event.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_DOWN)
                    focus = (focus + 1) % BUTTONS;
                else if (event.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_UP)
                    focus = (focus + BUTTONS - 1) % BUTTONS;
            }
        }
        if (running) { draw(); SDL_Delay(16); }
    }
    if (window) close_window();
    return 0;
}

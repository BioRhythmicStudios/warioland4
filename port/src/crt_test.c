// Deterministic readback, visual comparisons and bounded presentation benchmark.
// No game, ROM, save or personal settings are opened by this executable.
#include "port/crt.h"
#include "port/ppu.h"
#include <stdio.h>
#include <string.h>

static int failures;
static uint32_t frame[PORT_SCREEN_WIDTH * PORT_SCREEN_HEIGHT];
#define CHECK(test, label) do { if (!(test)) { fprintf(stderr, "FAIL: %s (%s)\n", label, SDL_GetError()); ++failures; } } while (0)

static SDL_Surface *Capture(SDL_Renderer *renderer, SDL_Texture *source, PortCrt *crt, PortSettings *settings, int bypass)
{
    CHECK(SDL_UpdateTexture(source, NULL, frame, PORT_SCREEN_WIDTH * 4), "frame upload");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    CHECK(SDL_RenderClear(renderer) && SDL_RenderTexture(renderer, source, NULL, NULL), "original draw");
    if (!bypass) CHECK(PortCrtDraw(crt, renderer, frame, settings), "CRT draw succeeds");
    // ReadPixels returns the image viewport. Keep the game's presentation state
    // unchanged; window screenshots separately cover the letterbox area.
    SDL_Surface *surface = SDL_RenderReadPixels(renderer, NULL);
    CHECK(SDL_RenderPresent(renderer), "present captured frame");
    CHECK(surface != NULL, "render readback");
    SDL_Surface *converted = surface ? SDL_ConvertSurface(surface, SDL_PIXELFORMAT_XBGR8888) : NULL;
    SDL_DestroySurface(surface);
    CHECK(converted != NULL, "readback conversion");
    return converted;
}

static uint32_t Pixel(SDL_Surface *s, int x, int y)
{
    return *(uint32_t *)((Uint8 *)s->pixels + y * s->pitch + x * 4) & 0xffffff;
}

static int Different(SDL_Surface *a, SDL_Surface *b)
{
    int n = 0;
    for (int y = 0; y < a->h; ++y)
        for (int x = 0; x < a->w; ++x)
            if (Pixel(a, x, y) != Pixel(b, x, y)) {
                if (!n) printf("Readback first difference (%d,%d): %06x / %06x\n", x, y, Pixel(a,x,y), Pixel(b,x,y));
                ++n;
            }
    if (n) printf("Readback differing pixels: %d\n", n);
    return n;
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: wl4-crt-test OUTPUT_DIRECTORY [FRAME.xbgr|-] [software|RENDERER]\n");
        return 2;
    }
    const char *driver = argc > 3 ? argv[3] : "software";
    int software = !strcmp(driver, "software");
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;
    for (int y = 0; y < PORT_SCREEN_HEIGHT; ++y)
        for (int x = 0; x < PORT_SCREEN_WIDTH; ++x)
            frame[y * PORT_SCREEN_WIDTH + x] = x < 80 ? 0xffffff : x < 90 ? 0
                : (((x / 8 + y / 8) & 1) ? 0x40b8f8 : 0xf08040);
    uint32_t original[PORT_SCREEN_WIDTH * PORT_SCREEN_HEIGHT];
    memcpy(original, frame, sizeof(frame));
    PortSettings settings = {0};
    settings.crtScanlines = 20; settings.crtGlow = 10; settings.crtMask = 1;
    PortCrt invalid = {0}; settings.crtPreset = 1;
    CHECK(!PortCrtDraw(&invalid, NULL, frame, &settings) && invalid.disabled, "unsupported API fails once and disables filter");
    CHECK(PortCrtDraw(&invalid, NULL, frame, &settings), "fallback avoids repeated resource retries");
    PortCrtDestroy(&invalid);
    const int sizes[][2] = {{240,160},{480,320},{720,480},{1440,960},{853,571}};
    for (size_t size = 0; size < SDL_arraysize(sizes); ++size) {
        int w = sizes[size][0], h = sizes[size][1];
        SDL_Surface *target = software ? SDL_CreateSurface(w, h, SDL_PIXELFORMAT_XBGR8888) : NULL;
        SDL_Window *window = software ? NULL : SDL_CreateWindow("WL4 CRT renderer test", w, h, 0);
        SDL_Renderer *renderer = software ? SDL_CreateSoftwareRenderer(target) : SDL_CreateRenderer(window, driver);
        CHECK(renderer != NULL, "create test renderer");
        if (!renderer) return 1;
        SDL_SetRenderVSync(renderer, 0);
        SDL_SetRenderLogicalPresentation(renderer, PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
        SDL_Texture *source = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XBGR8888, SDL_TEXTUREACCESS_STREAMING, PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT);
        CHECK(source && SDL_SetTextureScaleMode(source, SDL_SCALEMODE_NEAREST), "nearest source texture");
        PortCrt crt = {0}; settings.crtPreset = 0;
        SDL_Surface *baseline = Capture(renderer, source, &crt, &settings, 1);
        SDL_Surface *off = Capture(renderer, source, &crt, &settings, 0);
        CHECK(baseline && off && !Different(baseline, off), "Original is pixel-identical to bypass");
        CHECK(!crt.overlay && !crt.glow && !crt.glowPixels, "Off allocates no filter resources");
        SDL_DestroySurface(off);
        SDL_FRect rect; SDL_GetRenderLogicalPresentationRect(renderer, &rect);
        int scale = SDL_max(1, (int)(rect.w / PORT_SCREEN_WIDTH + 0.5f));
        int ox = 0, oy = 0;
        for (int mode = 1; mode <= 2; ++mode) {
            settings.crtPreset = mode;
            SDL_Surface *filtered = Capture(renderer, source, &crt, &settings, 0);
            if (filtered && baseline) {
                if (scale >= 2) {
                    CHECK(Pixel(filtered, ox + 10 * scale, oy + scale - 1) != Pixel(baseline, ox + 10 * scale, oy + scale - 1), "physical scanline visible");
                    CHECK((Pixel(filtered, ox + 10 * scale, oy + scale - 1) & 255) >= 180, "mild scanlines keep bright text readable");
                } else if (mode == 1) CHECK(!Different(filtered, baseline), "1x Lite suppresses undersampled scanlines");
                if (mode == 2) CHECK(Pixel(filtered, ox + 80 * scale, oy + 20 * scale) != 0, "glow reaches adjacent dark pixels");
                CHECK(filtered->w == (int)rect.w && filtered->h == (int)rect.h, "integer-scaled viewport preserved");
                SDL_Surface *repeat = Capture(renderer, source, &crt, &settings, 0);
                CHECK(repeat && !Different(filtered, repeat), "static input has no temporal flicker");
                SDL_DestroySurface(repeat);
            }
            SDL_DestroySurface(filtered);
        }
        settings.crtPreset = 2; settings.crtGlow = 0; settings.crtScanlines = 0; settings.crtMask = 0;
        SDL_Surface *neutral = Capture(renderer, source, &crt, &settings, 0);
        CHECK(neutral && baseline && !Different(neutral, baseline), "all strengths zero and mask off is neutral");
        SDL_DestroySurface(neutral); SDL_DestroySurface(baseline);
        CHECK(!memcmp(frame, original, sizeof(frame)), "software framebuffer remains untouched");
        // Optional real-game framebuffer is an explicit local test input.
        if (argc > 2 && strcmp(argv[2], "-")) {
            size_t bytes; void *data = SDL_LoadFile(argv[2], &bytes);
            CHECK(data && bytes == sizeof(frame), "explicit framebuffer fixture size");
            if (data && bytes == sizeof(frame)) memcpy(frame, data, sizeof(frame));
            SDL_free(data);
        }
        settings.crtGlow = 10; settings.crtScanlines = 20; settings.crtMask = 1;
        for (int mode = 0; mode <= 2; ++mode) {
            settings.crtPreset = mode;
            SDL_Surface *shot = Capture(renderer, source, &crt, &settings, 0);
            char path[4096]; snprintf(path, sizeof(path), "%s/%s-%dx%d-%d.png", argv[1], driver, w, h, mode);
            CHECK(shot && SDL_SavePNG(shot, path), "save comparison screenshot");
            SDL_DestroySurface(shot);
            const int runs = 120;
            Uint64 start = SDL_GetTicksNS();
            for (int run = 0; run < runs; ++run) {
                SDL_UpdateTexture(source, NULL, frame, PORT_SCREEN_WIDTH * 4);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);
                SDL_RenderTexture(renderer, source, NULL, NULL);
                if (mode) CHECK(PortCrtDraw(&crt, renderer, frame, &settings), "benchmark draw");
                CHECK(SDL_RenderPresent(renderer), "benchmark present");
                SDL_PumpEvents();
            }
            printf("CRT benchmark renderer=%s output=%dx%d mode=%d mean_ms=%.4f\n", SDL_GetRendererName(renderer), w, h, mode,
                   (SDL_GetTicksNS() - start) / (1e6 * runs));
        }
        memcpy(frame, original, sizeof(frame));
        PortCrtDestroy(&crt); SDL_DestroyTexture(source); SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window); SDL_DestroySurface(target);
    }
    SDL_Quit();
    printf("CRT render checks: %s\n", failures ? "FAIL" : "PASS");
    return failures ? 1 : 0;
}

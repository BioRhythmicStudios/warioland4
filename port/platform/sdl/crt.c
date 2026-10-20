#include "port/crt.h"
#include "port/ppu.h"
#include <stdio.h>
#include <string.h>

const char *PortCrtName(int preset)
{
    static const char *const names[] = {"Original (Off)", "CRT Lite", "CRT Enhanced"};
    return preset >= 0 && preset < 3 ? names[preset] : names[0];
}

void PortCrtDestroy(PortCrt *crt)
{
    SDL_DestroyTexture(crt->overlay);
    SDL_DestroyTexture(crt->glow);
    SDL_free(crt->glowPixels);
    memset(crt, 0, sizeof(*crt));
}

// One cached multiplicative texture at the actual integer-scaled image size.
// The pattern is anchored to source rows and physical columns, never to time.
// Below 2x there is no room for scanlines; below 3x suppress RGB mask aliasing.
static bool MakeOverlay(PortCrt *crt, SDL_Renderer *renderer, int scale, int strength, int mask)
{
    int w = PORT_SCREEN_WIDTH * scale, h = PORT_SCREEN_HEIGHT * scale;
    Sint64 limit = SDL_GetNumberProperty(SDL_GetRendererProperties(renderer), SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
    if (scale > 16 || (limit > 0 && (w > limit || h > limit)))
        return SDL_SetError("CRT overlay exceeds the supported texture size");
    SDL_DestroyTexture(crt->overlay);
    crt->overlay = NULL;
    uint32_t *pixels = SDL_malloc((size_t)w * h * sizeof(*pixels));
    if (!pixels) return false;
    for (int y = 0; y < h; ++y) {
        int gain = (scale >= 2 && y % scale >= (scale + 1) / 2) ? 255 * (100 - strength) / 100 : 255;
        for (int x = 0; x < w; ++x) {
            unsigned int color = 0xff000000u;
            for (int channel = 0; channel < 3; ++channel) {
                // Only 7.5% attenuation in the two non-selected phosphors.
                int value = mask && scale >= 3 && x % 3 != channel ? gain * 236 / 255 : gain;
                color |= (unsigned int)value << (8 * channel);
            }
            pixels[y * w + x] = color;
        }
    }
    crt->overlay = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, w, h);
    bool ok = crt->overlay
        && SDL_SetTextureScaleMode(crt->overlay, SDL_SCALEMODE_NEAREST)
        && SDL_SetTextureBlendMode(crt->overlay, SDL_BLENDMODE_MOD)
        && SDL_UpdateTexture(crt->overlay, NULL, pixels, w * 4);
    SDL_free(pixels);
    if (ok) { crt->scale = scale; crt->scanlines = strength; crt->mask = mask; }
    return ok;
}

static int Bright(uint32_t pixel, int shift)
{
    int value = (pixel >> shift) & 255;
    return value > 96 ? value - 96 : 0;
}

static bool DrawGlow(PortCrt *crt, SDL_Renderer *renderer, const uint32_t *frame, int strength)
{
    const int w = PORT_SCREEN_WIDTH, h = PORT_SCREEN_HEIGHT;
    if (!crt->glow) {
        crt->glowPixels = SDL_malloc((size_t)w * h * sizeof(*crt->glowPixels));
        if (!crt->glowPixels) return false;
        crt->glow = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        if (!crt->glow || !SDL_SetTextureScaleMode(crt->glow, SDL_SCALEMODE_LINEAR)
            || !SDL_SetTextureBlendMode(crt->glow, SDL_BLENDMODE_ADD)) return false;
    }
    // A five-tap bright-pass glow at native resolution. No full-window CPU pass,
    // temporal history, feedback or change to the original software PPU pixels.
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = y * w + x;
            unsigned int color = 0xff000000u;
            for (int shift = 0; shift < 24; shift += 8) {
                int sum = Bright(frame[i], shift) * 4
                    + Bright(frame[y * w + (x ? x - 1 : x)], shift)
                    + Bright(frame[y * w + (x + 1 < w ? x + 1 : x)], shift)
                    + Bright(frame[(y ? y - 1 : y) * w + x], shift)
                    + Bright(frame[(y + 1 < h ? y + 1 : y) * w + x], shift);
                color |= (unsigned int)(sum * strength / 800) << shift;
            }
            crt->glowPixels[i] = color;
        }
    }
    return SDL_UpdateTexture(crt->glow, NULL, crt->glowPixels, w * 4)
        && SDL_RenderTexture(renderer, crt->glow, NULL, NULL);
}

bool PortCrtDraw(PortCrt *crt, SDL_Renderer *renderer, const uint32_t *frame, const PortSettings *settings)
{
    if (!settings->crtPreset || crt->disabled) return true;
    SDL_FRect rect;
    if (!SDL_GetRenderLogicalPresentationRect(renderer, &rect)) goto failed;
    int scale = (int)(rect.w / PORT_SCREEN_WIDTH + 0.5f);
    if (scale < 1) scale = 1;
    int mask = settings->crtPreset == 2 && settings->crtMask && scale >= 3;
    int scanlines = scale >= 2 ? settings->crtScanlines : 0;
    if (settings->crtPreset == 2 && settings->crtGlow
        && !DrawGlow(crt, renderer, frame, settings->crtGlow)) goto failed;
    if (scanlines || mask) {
        if ((!crt->overlay || crt->scale != scale || crt->scanlines != scanlines || crt->mask != mask)
            && !MakeOverlay(crt, renderer, scale, scanlines, mask)) goto failed;
        if (!SDL_RenderTexture(renderer, crt->overlay, NULL, NULL)) goto failed;
    }
    return true;
failed:
    fprintf(stderr, "CRT unavailable; using Original for this session: %s\n", SDL_GetError());
    PortCrtDestroy(crt);
    crt->disabled = 1;
    return false;
}

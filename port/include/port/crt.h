#ifndef PORT_CRT_H
#define PORT_CRT_H
#include "port/settings.h"
#include <stdint.h>

// SDL-only presentation resources. Never owns or modifies the GBA framebuffer.
typedef struct PortCrt {
    SDL_Texture *overlay, *glow;
    uint32_t *glowPixels;
    int scale, scanlines, mask, disabled;
} PortCrt;

const char *PortCrtName(int preset);
void PortCrtDestroy(PortCrt *crt);
// Call after drawing the original image, before host warning/UI overlays.
// On failure, returns false once: the caller redraws Original for that frame.
// Later frames stay in Original until renderer reset/recreation, without retries.
bool PortCrtDraw(PortCrt *crt, SDL_Renderer *renderer, const uint32_t *frame, const PortSettings *settings);
#endif

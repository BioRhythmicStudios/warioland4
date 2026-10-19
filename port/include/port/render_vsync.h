#ifndef PORT_RENDER_VSYNC_H
#define PORT_RENDER_VSYNC_H
#include <SDL3/SDL.h>
#include <stdio.h>

// Main thread only. Unsupported drivers are non-fatal; keep the user's preference.
static inline int PortApplyRenderVSync(SDL_Renderer *renderer, int wanted)
{
    int active = 0;
    bool applied = SDL_SetRenderVSync(renderer, wanted);
    if (!applied) {
        fprintf(stderr, "VSync %s unavailable: %s\n", wanted ? "On" : "Off", SDL_GetError());
        if (wanted && !SDL_SetRenderVSync(renderer, 0))
            fprintf(stderr, "VSync fallback unavailable: %s\n", SDL_GetError());
    }
    if (!SDL_GetRenderVSync(renderer, &active)) {
        fprintf(stderr, "Cannot query VSync: %s\n", SDL_GetError());
        active = applied ? wanted : 0;
    }
    const char *name = SDL_GetRendererName(renderer);
    // SDL reports its configured interval, not proof of synchronized scanout.
    printf("Renderer: %s; VSync requested=%d reported=%d\n", name ? name : "unavailable", wanted, active);
    return active != 0;
}
#endif

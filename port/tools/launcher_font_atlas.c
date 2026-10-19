#include <SDL3/SDL.h>
#include <stdio.h>
int main(int argc, char **argv) {
    if (argc != 2) return 2;
    SDL_Surface *surface = SDL_CreateSurface(128, 112, SDL_PIXELFORMAT_RGBA32);
    SDL_Renderer *renderer = surface ? SDL_CreateSoftwareRenderer(surface) : NULL;
    if (!renderer) {fprintf(stderr, "%s\n", SDL_GetError());return 1;}
    SDL_SetRenderDrawColor(renderer,0,0,0,255);SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    for (int code=32;code<=255;code++) {
        if (code>126 && code<160) continue;
        char text[3]={0};
        if(code<128) text[0]=(char)code;
        else {text[0]=(char)(0xC0|(code>>6));text[1]=(char)(0x80|(code&63));}
        SDL_RenderDebugText(renderer,((code-32)%16)*8,((code-32)/16)*8,text);
    }
    SDL_RenderPresent(renderer);
    int ok=SDL_SaveBMP(surface,argv[1]);
    SDL_DestroyRenderer(renderer);SDL_DestroySurface(surface);
    return ok?0:1;
}

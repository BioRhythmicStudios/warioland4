#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
int main(void) {
    int count = SDL_GetNumAudioDrivers();
    printf("SDL audio drivers (%d):", count);
    for (int i = 0; i < count; ++i) printf(" %s", SDL_GetAudioDriver(i));
    putchar('\n');
    const char *forced = getenv("SDL_AUDIODRIVER");
    printf("SDL_AUDIODRIVER=%s\n", forced ? forced : "(automatic)");
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        printf("SDL audio init failed: %s\n", SDL_GetError());
        return 1;
    }
    printf("Selected SDL audio driver: %s\n", SDL_GetCurrentAudioDriver());
    int devices = 0;
    SDL_AudioDeviceID *ids = SDL_GetAudioPlaybackDevices(&devices);
    printf("Playback devices (%d):\n", devices);
    for (int i = 0; i < devices; ++i)
        printf("  %u: %s\n", (unsigned)ids[i], SDL_GetAudioDeviceName(ids[i]));
    SDL_free(ids);
    SDL_AudioSpec spec = { SDL_AUDIO_S16, 2, 32768 };
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!stream) {
        printf("Open default playback failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }
    printf("Open default playback: OK\n");
    SDL_DestroyAudioStream(stream);
    SDL_Quit();
    return 0;
}

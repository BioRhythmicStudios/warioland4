#include "port/game_thread.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "port/gba_memory.h"

#define MINICORO_IMPL
#define MCO_NO_DEBUG
#include "minicoro.h"

static mco_coro *sGame;
static void (*sEntry)(void);

static void GameMain(mco_coro *co)
{
    (void)co;
    sEntry();
}

int PortGameThreadCreate(void (*entry)(void))
{
    // Tests and soft restarts can create another coroutine in the same process.
    // Release its host allocation before replacing the global owner.
    if (sGame != NULL) {
        mco_result old = mco_destroy(sGame);
        if (old != MCO_SUCCESS) {
            fprintf(stderr, "Cannot destroy the previous game coroutine: %s\n", mco_result_description(old));
            return -1;
        }
        sGame = NULL;
    }
    mco_desc desc = mco_desc_init_with_stack(GameMain, (void *)(uintptr_t)PORT_GAME_STACK_LIMIT,
                                             PORT_GAME_STACK_TOP - PORT_GAME_STACK_LIMIT);
    mco_result res = mco_create(&sGame, &desc);

    if (res != MCO_SUCCESS) {
        fprintf(stderr, "Cannot create the game coroutine: %s\n", mco_result_description(res));
        return -1;
    }
    sEntry = entry;
    return 0;
}

int PortGameThreadResume(void)
{
    mco_result res = mco_resume(sGame);

    if (res != MCO_SUCCESS) {
        fprintf(stderr, "Cannot resume the game: %s\n", mco_result_description(res));
        abort();
    }
    return mco_status(sGame) != MCO_DEAD;
}

void PortGameThreadYield(void)
{
    mco_result res = mco_yield(sGame);

    if (res != MCO_SUCCESS) {
        fprintf(stderr, "Cannot yield from the game: %s\n", mco_result_description(res));
        abort();
    }
}

int PortOnGameThread(void)
{
    return sGame != NULL && mco_running() == sGame;
}

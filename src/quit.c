#include "interrupt_callbacks.h"
#include "main.h"
#include "save_file.h"
#include "score.h"

void QuitVBlankCallback(void)
{
    m4aSoundVSync();
    m4aSoundMain();
}

void QuitSubroutine(void)
{
    InterruptCallbackSetVBlank(QuitVBlankCallback);
    InitializeTemporarySaveBuffer();
    gTimerState = 0;
    gSwitchPressed = 0;
    gStageEntryPauseMenuDisabled = 0;
}

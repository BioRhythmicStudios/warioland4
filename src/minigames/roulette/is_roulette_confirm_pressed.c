#include "input.h"

int IsRouletteConfirmPressed(void)
{
    if (gButtonsPressed & 1)
        return 1;
    return 0;
}

#ifndef BACKGROUNDS_H
#define BACKGROUNDS_H

#include "types.h"

struct AGB_ALIGN BgTileLookupPointers {
    GBA_PTR(u8*) bg0;
    GBA_PTR(u8*) bg1;
    GBA_PTR(u8*) bg2;
};

void BackgroundProcessMain();
void func_806C75C();

#endif  // BACKGROUNDS_H

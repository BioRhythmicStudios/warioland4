#ifndef TRANSPARENCY_H
#define TRANSPARENCY_H

#include "backgrounds.h"
#include "global_data.h"
#include "sound.h"
#include "wario.h"

extern struct TransparencyState gUnk_30000D8;
extern const u8 sUnk_83F7820[];

void TransparencyProcessTiles(s32 isMoving);
void TransparencyProcessWater(void);

#endif  // TRANSPARENCY_H

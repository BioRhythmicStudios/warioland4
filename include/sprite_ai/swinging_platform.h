#ifndef SPRITE_AI_SWINGING_PLATFORM_H
#define SPRITE_AI_SWINGING_PLATFORM_H

#include "types.h"
#include "oam.h"

struct AnimationFrame;

extern const struct AnimationFrame sSwingingPlatformOam_83CFB48[];
extern const struct AnimationFrame sSwingingPlatformOam_83CFB58[];
extern const struct AnimationFrame sSwingingPlatformOam_83CFB68[];

extern u8 gUnk_3000A4B;
extern u8 gUnk_3000A4D;

extern const s16 sUnk_83CFB78[];
extern const s16 sUnk_83CFBD8[];
extern const s16 sUnk_83CFC38[];
extern const u8 sUnk_83CFC98[];
extern const u8 sUnk_83CFCC8[];
extern const s16 sUnk_83CFCF8[];
extern const s16 sUnk_83CFD70[];
extern const s16 sUnk_83CFDE8[];
extern const u8 sUnk_83CFE60[];
extern const u8 sUnk_83CFE9C[];
extern const s16 sUnk_83CFED8[];
extern const s16 sUnk_83CFF38[];
extern const s16 sUnk_83CFF98[];
extern const s16 sUnk_83CFFF8[];
extern const s16 sUnk_83D0070[];
extern const s16 sUnk_83D00E8[];
extern const s16 sUnk_83D0160[];
extern const s16 sUnk_83D01C0[];
extern const s16 sUnk_83D0220[];
extern const s16 sUnk_83D0280[];
extern const s16 sUnk_83D02F8[];
extern const s16 sUnk_83D0370[];

void SpriteSwingingPlatform(void);
void SpriteSwingingPlatform2(void);
void SpriteUnknownAC(void);
void SpriteUnknownAD(void);

#endif  // SPRITE_AI_SWINGING_PLATFORM_H

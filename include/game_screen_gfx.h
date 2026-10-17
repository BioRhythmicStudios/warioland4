#ifndef GAME_SCREEN_GFX_H
#define GAME_SCREEN_GFX_H

#include "oam.h"

struct AGB_ALIGN Unk_82DDCF0 {
    GBA_PTR(struct AnimationFrameU16*) unk0;
    GBA_PTR(struct AnimationFrameU16*) unk4;
    GBA_PTR(struct AnimationFrameU16*) unk8;
    GBA_PTR(struct AnimationFrameU16*) unkC;
    GBA_PTR(struct AnimationFrameU16*) unk10;
    GBA_PTR(struct AnimationFrameU16*) unk14;
    GBA_PTR(struct AnimationFrameU16*) unk18;
    GBA_PTR(struct AnimationFrameU16*) unk1C;
    GBA_PTR(struct AnimationFrameU16*) unk20;
    GBA_PTR(struct AnimationFrameU16*) unk24;
    GBA_PTR(struct AnimationFrameU16*) unk28;
    GBA_PTR(struct AnimationFrameU16*) unk2C;
};

extern struct Unk_82DDCF0 sUnk_82DDCF0;
extern struct AnimationFrameU16 sUnk_82E06BC[];
extern struct AnimationFrameU16 sUnk_82E0BA8[];
extern struct AnimationFrameU16 sUnk_82E0CD8[];
extern struct AnimationFrameU16 sUnk_82E09A4[];
extern struct AnimationFrameU16 sUnk_82E11E4[];
extern struct AnimationFrameU16 sUnk_82E126C[];
extern struct AnimationFrameU16 sUnk_82E1294[];
extern struct AnimationFrameU16 sUnk_82E12BC[];
extern struct AnimationFrameU16 sUnk_82E1304[];
extern struct AnimationFrameU16 sUnk_82E1334[];
extern struct AnimationFrameU16 sUnk_82E13DC[];
extern struct AnimationFrameU16 sUnk_82E140C[];
extern struct AnimationFrameU16 sUnk_82E143C[];
extern struct AnimationFrameU16 sUnk_82E149C[];
extern struct AnimationFrameU16 sUnk_82E14FC[];
extern struct AnimationFrameU16 sUnk_82E1598[];
extern struct AnimationFrameU16 sUnk_82E1658[];
extern struct AnimationFrameU16 sUnk_82E1690[];

void GameScreenDrawWario(void);
void GameScreenDrawWarioEffects(void);
s32 GameScreenFadeIn(void);
s32 GameScreenFadeOut(void);
void GameScreenDebugSubroutine(void);

#endif  // GAME_SCREEN_GFX_H

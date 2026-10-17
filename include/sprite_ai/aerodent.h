#ifndef SPRITE_AI_AERODENT_H
#define SPRITE_AI_AERODENT_H

#include "types.h"
#include "oam.h"

extern const struct AnimationFrame sAerodentOam_83DAB28[];
extern const struct AnimationFrame sAerodentOam_83DABB0[];
extern const struct AnimationFrame sAerodentOam_83DABC8[];
extern const struct AnimationFrame sAerodentOam_83DACC0[];
extern const struct AnimationFrame sAerodentOam_83DAD28[];
extern const struct AnimationFrame sAerodentOam_83DAD48[];
extern const struct AnimationFrame sAerodentOam_83DAD68[];
extern const struct AnimationFrame sAerodentOam_83DADA8[];
extern const struct AnimationFrame sAerodentOam_83DADF0[];
extern const struct AnimationFrame sAerodentOam_83DAE38[];
extern const struct AnimationFrame sAerodentOam_83DAE50[];
extern const struct AnimationFrame sAerodentOam_83DAE78[];
extern const struct AnimationFrame sAerodentOam_83DAF00[];
extern const struct AnimationFrame sAerodentOam_83DB0B0[];
extern const struct AnimationFrame sAerodentOam_83DB0E8[];
extern const struct AnimationFrame sAerodentOam_83DB110[];
extern const struct AnimationFrame sAerodentOam_83DB3A4[];
extern const struct AnimationFrame sAerodentOam_83DB3B4[];
extern const struct AnimationFrame sAerodentOam_83DB3EC[];
extern const struct AnimationFrame sAerodentOam_83DB3FC[];
extern const struct AnimationFrame sAerodentOam_83DB43C[];
extern const struct AnimationFrame sAerodentOam_83DB48C[];
extern const struct AnimationFrame sAerodentOam_83DB49C[];
extern const struct AnimationFrame sAerodentOam_83DB4AC[];
extern const struct AnimationFrame sAerodentOam_83DB4BC[];
extern const struct AnimationFrame sAerodentOam_83DB4D4[];
extern const struct AnimationFrame sAerodentOam_83DB4E4[];
extern const struct AnimationFrame sAerodentOam_83DB588[];
extern const struct AnimationFrame sAerodentOam_83DB5A8[];
extern const struct AnimationFrame sAerodentOam_83DB5C8[];
extern const struct AnimationFrame sAerodentOam_83DB5E8[];
extern const struct AnimationFrame sAerodentOam_83DBA60[];
extern u16 sUnk_83DB610[];
extern u16 sUnk_83DB810[];
extern s16 sUnk_83DBA10[];
extern u16 sUnk_83DBA34[];
extern s16 sUnk_83DBA70[];
extern s16 sUnk_83DBAE2[][2];
extern s16 sUnk_83DBB62[][2];
extern s16 sUnk_83DBBE2[];
extern s16 sUnk_83DBC08[];
extern s16 sUnk_83DBC32[];
extern s16 sUnk_83DBC52[];
extern s16 sUnk_83DBC68[];

void SpriteAerodentWeakPoint(void);
void SpriteAerodent(void);
void SpriteAerodentCompanion(void);
void SpriteAerodentThrowable(void);
void SpriteAerodentEmitter(void);

#endif  // SPRITE_AI_AERODENT_H

#ifndef SPRITE_AI_CRACTUS_H
#define SPRITE_AI_CRACTUS_H

#include "oam.h"
#include "types.h"

void SpriteCractusSpawner(void);
void SpriteCractusBoss(void);
void SpriteCractusStemSegment(void);
void SpriteCractusStemTip(void);
void SpriteCractusArmSegment(void);
void SpriteCractusDefeatPiece(void);
void SpriteCractusDefeatAttachment(void);
void SpriteCractusProjectile(void);
void SpriteCractusImpactEffect(void);

extern const struct AnimationFrame sCractusProjectileFallingOam[];
extern const struct AnimationFrame sCractusProjectileActiveOam[];
extern const struct AnimationFrame sCractusProjectileLandedOam[];
extern const struct AnimationFrame sCractusProjectileDespawnOam[];
extern const struct AnimationFrame sCractusDefeatPieceOam[];
extern const struct AnimationFrame sCractusSegmentOam[];
extern const struct AnimationFrame sCractusImpactEffectOam[];
extern const s16 sCractusProjectileYVelocity[];
extern const s16 sCractusDefeatPieceYVelocity[];
extern const struct AnimationFrame sCractusSpawnerDefeatOam[];
extern const struct AnimationFrame sCractusBossReadyOam[];
extern const struct AnimationFrame sCractusBossIntroOam[];
extern const struct AnimationFrame sBossTreasureOam_83DF9E8[];
extern const struct AnimationFrame sCractusBossInitialOam[];
extern const struct AnimationFrame sCractusArmSegmentB1Oam[];
extern const struct AnimationFrame sCractusArmSegmentB2Oam[];
extern const struct AnimationFrame sCractusArmSegmentB3Oam[];
extern const struct AnimationFrame sCractusArmSegmentBRootOam[];
extern const struct AnimationFrame sCractusArmSegmentA1Oam[];
extern const struct AnimationFrame sCractusArmSegmentA2Oam[];
extern const struct AnimationFrame sCractusArmSegmentA3Oam[];
extern const struct AnimationFrame sCractusArmSegmentARootOam[];
extern const u8 sCractusBossPose113GraphicsAnimation[];
extern const u8 sCractusBossIntroGraphicsAnimation[];
extern const u8 sCractusBossPose111GraphicsAnimation[];
extern const u8 sCractusBossDamageGraphicsAnimation[];
extern const u8 sCractusBossAttackGraphicsAnimation[];
extern const u8 sCractusBossHurtGraphicsAnimation[];
extern const struct AnimationFrame sCractusBossDamagedOam[];
extern const struct AnimationFrame sCractusSpawnerIdleOam[];
extern const struct AnimationFrame sCractusBossArmExtendedOam[];
extern const struct AnimationFrame sCractusBossImpactOam[];
extern const u8 sCractusBossPose112GraphicsAnimation[];
extern const struct AnimationFrame sCractusStemSegment0Oam[];
extern const struct AnimationFrame sCractusStemSegment1Oam[];
extern const struct AnimationFrame sCractusStemSegment2Oam[];
extern const struct AnimationFrame sCractusStemSegment3Oam[];
extern const struct AnimationFrame sCractusStemSegment4Oam[];
extern const s16 sCractusBossPose113YVelocity[];
extern const s16 sCractusBossPose18YVelocity[];
extern const s16 sCractusBossPose19YVelocity[];
extern const s16 sCractusBossPose50LeftYVelocity[];
extern const s16 sCractusBossPose50RightYVelocity[];
extern const s16 sCractusBossPose15YVelocity[];
extern const s16 sCractusBossIntroYVelocity[];
extern const s16 sCractusArmPose24XVelocityA[];
extern const s16 sCractusArmPose24XVelocityB[];
extern const s16 sCractusArmPose15LeftXVelocity[];
extern const s16 sCractusArmPose15LeftYVelocity[];
extern const s16 sCractusArmPose15RightXVelocity[];
extern const s16 sCractusArmPose15RightYVelocity[];
extern const u8 sCractusBossDefaultGraphicsAnimation[];

#endif  // SPRITE_AI_CRACTUS_H

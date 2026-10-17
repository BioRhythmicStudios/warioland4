
#include "autosave.h"
#include "fixed_point.h"
#include "main.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "voice_set.h"
#include "wario.h"

#ifndef PORTABLE
const u16 sVortexLargePartOam_Frame1[] = {
    1,
    OAM_ENTRY(-32, -32, SPRITE_SIZE_64x64, 0, 512, 8, 0),
};
const u16 sVortexMediumPartOam_Frame1[] = {
    1,
    OAM_ENTRY(-32, -32, SPRITE_SIZE_64x64, 0, 520, 8, 0),
};
const u16 sVortexSmallPartOam_Frame1[] = {
    1,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_32x32, 0, 528, 8, 0),
};
const u16 sVortexWarioOam_Frame1[] = {
    1,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_32x32, 0, 532, 9, 0),
};
const u16 sVortexKeyzerOam_Frame1[] = {
    1,
    OAM_ENTRY(-16, -16, SPRITE_SIZE_32x32, 0, 536, 10, 0),
};

const struct AnimationFrame sVortexLargePartOam[] = {{sVortexLargePartOam_Frame1, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sVortexMediumPartOam[] = {{sVortexMediumPartOam_Frame1, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sVortexSmallPartOam[] = {{sVortexSmallPartOam_Frame1, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sVortexWarioOam[] = {{sVortexWarioOam_Frame1, 200}, ANIMATION_TERMINATOR};
const struct AnimationFrame sVortexKeyzerOam[] = {{sVortexKeyzerOam_Frame1, 200}, ANIMATION_TERMINATOR};

const u16 sVortexPal[] = {
#include "data/sprites/vortex.pal.inc"
};
#else
extern const u16 sVortexLargePartOam_Frame1[];
extern const u16 sVortexMediumPartOam_Frame1[];
extern const u16 sVortexSmallPartOam_Frame1[];
extern const u16 sVortexWarioOam_Frame1[];
extern const u16 sVortexKeyzerOam_Frame1[];
extern const struct AnimationFrame sVortexLargePartOam[];
extern const struct AnimationFrame sVortexMediumPartOam[];
extern const struct AnimationFrame sVortexSmallPartOam[];
extern const struct AnimationFrame sVortexWarioOam[];
extern const struct AnimationFrame sVortexKeyzerOam[];
extern const u16 sVortexPal[];
#endif

void VortexSetCommonProperties(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_AFFINE | SPRITE_STATUS_BACKGROUND;
    gCurrentSprite.drawPriority |= 0x80;
    gCurrentSprite.hitboxExtentUp = PIXEL_SIZE;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = PIXEL_SIZE;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 6;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work3 = 0;
}

void VortexInitSmallPart(void)
{
    gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
    GP_SET(gCurrentSprite.pOamData, sVortexSmallPartOam);
    VortexSetCommonProperties();
}

void VortexInitMediumPart(void)
{
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 2 * BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = 2 * BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceLeftRight = 2 * BLOCK_SIZE_PIXELS;
    GP_SET(gCurrentSprite.pOamData, sVortexMediumPartOam);
    VortexSetCommonProperties();
}

void VortexInitLargePart(void)
{
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 2 * BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = 2 * BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceLeftRight = 2 * BLOCK_SIZE_PIXELS;
    GP_SET(gCurrentSprite.pOamData, sVortexLargePartOam);
    VortexSetCommonProperties();
}

void VortexFinishStage(void)
{
    gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
    gDisableWario = 1;
    gWarioPauseTimer = CONVERT_SECONDS(16.0 + 2.0 / 3.0);
    gStageTimerColonState = 2;
    AutosaveStageClear();
    SpawnHighPriorityPrimarySprite(
        PSPRITE_WARIO_ENTERING_VORTEX,
        gCurrentSprite.roomSlot,
        gCurrentSprite.gfxSlot,
        gCurrentSprite.yPosition,
        gCurrentSprite.xPosition
    );
    gUnk_3000C0E = 1;
    gVortexYPosition = gCurrentSprite.yPosition;
    gVortexXPosition = gCurrentSprite.xPosition;
    gVortexGfxSlot = gCurrentSprite.gfxSlot;
    func_801E4D4();
    VoiceSetPlay(VS_WARIO_KEYZER_GET);
}

void VortexAnimatePalette(void)
{
    s32 destAddress;

    if (gMainTimer & 7 || gSubGameMode != 2) {
        return;
    }

    TIMER_COUNT_UP(gCurrentSprite.work3);
    if (gCurrentSprite.work3 >= 32) {
        gCurrentSprite.work3 = gMainTimer & 7;
    }

    destAddress = PLTT_ROW_SIZE * (gCurrentSprite.gfxSlot + 8);
    DmaCopy16(3, sVortexPal + 16 * gCurrentSprite.work3, OBJ_PLTT + destAddress, PLTT_ROW_SIZE);
}

void VortexInitWarioOrKeyzer(void)
{
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_AFFINE;
    gCurrentSprite.drawPriority |= 0x80;
    gCurrentSprite.drawDistanceDown = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceUp = BLOCK_SIZE_PIXELS;
    gCurrentSprite.drawDistanceLeftRight = BLOCK_SIZE_PIXELS;
    gCurrentSprite.hitboxExtentUp = PIXEL_SIZE;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = PIXEL_SIZE;
    gCurrentSprite.hitboxExtentRight = PIXEL_SIZE;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
}

void SpriteVortex(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] =
                MAKE_PERSISTENT_DATA(SPOSE_INIT_1, PERSISTENT_STATUS_LOADED);
            gCurrentSprite.pose = SPOSE_11;
            gCurrentSprite.work0 = CONVERT_SECONDS(1.5);
            gCurrentSprite.work2 = U8_MAX;
            VortexInitSmallPart();
            SpriteSpawnAsChild(
                PSPRITE_VORTEX_PART_MEDIUM,
                gCurrentSprite.roomSlot,
                gCurrentSprite.gfxSlot,
                gCurrentSprite.yPosition,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_VORTEX_PART_LARGE,
                gCurrentSprite.roomSlot,
                gCurrentSprite.gfxSlot,
                gCurrentSprite.yPosition,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_WARIO_EXITING_VORTEX,
                gCurrentSprite.roomSlot,
                gCurrentSprite.gfxSlot,
                gCurrentSprite.yPosition,
                gCurrentSprite.xPosition
            );
            if (gCurrentSprite.globalID == PSPRITE_VORTEX_2) {
                SpawnSecondarySprite(SUBPIXELS_FROM_BLOCKS(8.25), BLOCK_SIZE, SSPRITE_51);
            }
            break;

        case SPOSE_11:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.pose = SPOSE_12;
            }
            break;

        case SPOSE_12:
            if (gCurrentSprite.work2 <= 0x40) {
                gCurrentSprite.pose = SPOSE_18;
                gCurrentSprite.work2 = 0x40;
            } else if (gCurrentSprite.work2 <= 0x80) {
                gCurrentSprite.work2 -= 4;
            } else {
                gCurrentSprite.work2 -= 2;
            }
            break;

        case SPOSE_INIT_1:
            if (gSwitchPressed) {
                gCurrentSprite.work2 = U8_MAX;
                gCurrentSprite.pose = SPOSE_IDLE;
            } else {
                gCurrentSprite.work2 = 64;
                gCurrentSprite.pose = SPOSE_18;
            }
            VortexInitSmallPart();
            SpriteSpawnAsChild(
                PSPRITE_VORTEX_PART_MEDIUM_2,
                gCurrentSprite.roomSlot,
                gCurrentSprite.gfxSlot,
                gCurrentSprite.yPosition,
                gCurrentSprite.xPosition
            );
            SpriteSpawnAsChild(
                PSPRITE_VORTEX_PART_LARGE_2,
                gCurrentSprite.roomSlot,
                gCurrentSprite.gfxSlot,
                gCurrentSprite.yPosition,
                gCurrentSprite.xPosition
            );
            break;

        case SPOSE_18:
            if (gSwitchPressed) {
                gCurrentSprite.pose = SPOSE_14;
            }
            break;

        case SPOSE_14:
            if (gCurrentSprite.work2 >= U8_MAX) {
                gCurrentSprite.pose = SPOSE_IDLE;
                gCurrentSprite.work2 = U8_MAX;
            } else {
                gCurrentSprite.work2 += 1;
            }
            break;

        case SPOSE_IDLE:
            gCurrentSprite.disableWarioCollisionTimer = 0;
            break;

        case SPOSE_CRUSHED_OR_COLLECTED_INIT:
            VortexFinishStage();
            break;
    }

    TIMER_COUNT_DOWN(gCurrentSprite.work1);
    gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    VortexAnimatePalette();
}

void SpriteVortexPartMedium(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            VortexInitMediumPart();
            if (gCurrentSprite.globalID == PSPRITE_VORTEX_PART_MEDIUM) {
                gCurrentSprite.pose = SPOSE_11;
                gCurrentSprite.work0 = CONVERT_SECONDS(1.5);
                gCurrentSprite.work2 = U8_MAX;
            } else if (gCurrentSprite.globalID == PSPRITE_VORTEX_PART_MEDIUM_2) {
                if (gSwitchPressed) {
                    gCurrentSprite.work2 = U8_MAX;
                    gCurrentSprite.pose = SPOSE_IDLE;
                } else {
                    gCurrentSprite.work2 = 64;
                    gCurrentSprite.pose = SPOSE_18;
                }
            } else {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;

        case SPOSE_11:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.pose = SPOSE_12;
            }
            break;

        case SPOSE_12:
            if (gCurrentSprite.work2 <= 0x40) {
                gCurrentSprite.pose = SPOSE_18;
                gCurrentSprite.work2 = 0x40;
            } else if (gCurrentSprite.work2 <= 0x80) {
                gCurrentSprite.work2 -= 4;
            } else {
                gCurrentSprite.work2 -= 2;
            }
            break;

        case SPOSE_18:
            if (gSwitchPressed) {
                gCurrentSprite.pose = SPOSE_14;
            }
            break;

        case SPOSE_14:
            if (gCurrentSprite.work2 >= U8_MAX) {
                gCurrentSprite.pose = SPOSE_IDLE;
                gCurrentSprite.work2 = U8_MAX;
            } else {
                gCurrentSprite.work2 += 1;
            }
            break;
    }

    if ((gMainTimer & 1) != 0) {
        TIMER_COUNT_DOWN(gCurrentSprite.work1);
    }
    gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    VortexAnimatePalette();
}

void SpriteVortexPartLarge(void)
{
    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            VortexInitLargePart();
            if (gCurrentSprite.globalID == PSPRITE_VORTEX_PART_LARGE) {
                gCurrentSprite.pose = SPOSE_11;
                gCurrentSprite.work0 = CONVERT_SECONDS(1.5);
                gCurrentSprite.work2 = U8_MAX;
            } else if (gCurrentSprite.globalID == PSPRITE_VORTEX_PART_LARGE_2) {
                if (gSwitchPressed) {
                    gCurrentSprite.work2 = U8_MAX;
                    gCurrentSprite.pose = SPOSE_IDLE;
                } else {
                    gCurrentSprite.work2 = 64;
                    gCurrentSprite.pose = SPOSE_18;
                }
            } else {
                gCurrentSprite.status = SPRITE_STATUS_NONE;
            }
            break;

        case SPOSE_11:
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.pose = SPOSE_12;
            }
            break;

        case SPOSE_12:
            if (gCurrentSprite.work2 <= 0x40) {
                gCurrentSprite.pose = SPOSE_18;
                gCurrentSprite.work2 = 0x40;
            } else if (gCurrentSprite.work2 <= 0x80) {
                gCurrentSprite.work2 -= 4;
            } else {
                gCurrentSprite.work2 -= 2;
            }
            break;

        case SPOSE_18:
            if (gSwitchPressed) {
                gCurrentSprite.pose = SPOSE_14;
            }
            break;

        case SPOSE_14:
            if (gCurrentSprite.work2 >= U8_MAX) {
                gCurrentSprite.pose = SPOSE_IDLE;
                gCurrentSprite.work2 = U8_MAX;
            } else {
                gCurrentSprite.work2 += 1;
            }
            break;
    }

    if ((gMainTimer & 3) == 0) {
        TIMER_COUNT_DOWN(gCurrentSprite.work1);
    }
    gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(gCurrentSprite.work2));
    VortexAnimatePalette();
}

void SpriteWarioExitingVortex(void)
{
    s32 scale;

    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            VortexInitWarioOrKeyzer();
            GP_SET(gCurrentSprite.pOamData, sVortexWarioOam);
            gCurrentSprite.work0 = CONVERT_SECONDS(1.0);
            gCurrentSprite.work1 = -ANGLE_PI / 8;
            gCurrentSprite.work2 = 0x100 / 16;
            m4aSongNumStart(SOUND_1CC);
            break;

        case SPOSE_IDLE:
            if (gCurrentSprite.work2) {
                gCurrentSprite.work2 += 4;
            }
            gCurrentSprite.work1 -= ANGLE_PI / 16;
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
            }
            break;

        default:
            gDisableWario = FALSE;
            gWarioPauseTimer = 0;
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            break;
    }

    if (gCurrentSprite.work2 == 0) {
        scale = 0x100;
    } else {
        scale = gCurrentSprite.work2;
    }
    gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(scale));
}

void SpriteWarioEnteringVortex(void)
{
    s32 scale;

    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            VortexInitWarioOrKeyzer();
            GP_SET(gCurrentSprite.pOamData, sVortexWarioOam);
            gCurrentSprite.work0 = CONVERT_SECONDS(1.0);
            gCurrentSprite.work1 = -ANGLE_PI / 8;
            gCurrentSprite.work2 = 0;
            break;

        case SPOSE_IDLE:
            if ((u8)(gCurrentSprite.work2 - 1) >= (0x100 / 16)) {
                gCurrentSprite.work2 -= 4;
            }
            gCurrentSprite.work1 -= ANGLE_PI / 16;
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
            }
            break;

        default:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            if (gCollectedKeyzer != 1) {
                gSubGameMode = 6;
                gSpriteAiDropTimer = 0;
                gStageExitType = 2;
            }
            break;
    }

    if (gCurrentSprite.work2 == 0) {
        scale = 0x100;
    } else {
        scale = gCurrentSprite.work2;
    }
    gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(scale));
}

void SpriteKeyzerEnteringVortex(void)
{
    s32 scale;

    gCurrentSprite.disableWarioCollisionTimer = DELTA_TIME;
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            VortexInitWarioOrKeyzer();
            GP_SET(gCurrentSprite.pOamData, sVortexKeyzerOam);
            gCurrentSprite.work0 = CONVERT_SECONDS(1.0);
            gCurrentSprite.work1 = -ANGLE_PI / 8;
            gCurrentSprite.work2 = 0;
            break;

        case SPOSE_IDLE:
            if ((u8)(gCurrentSprite.work2 - 1) >= (0x100 / 16)) {
                gCurrentSprite.work2 -= 4;
            }
            gCurrentSprite.work1 -= ANGLE_PI / 16;
            TIMER_COUNT_DOWN(gCurrentSprite.work0);
            if (gCurrentSprite.work0 == 0) {
                gCurrentSprite.pose = SPOSE_CRUSHED_OR_COLLECTED;
            }
            break;

        default:
            gCurrentSprite.status = SPRITE_STATUS_NONE;
            if (gCollectedKeyzer == 1) {
                gSubGameMode = 6;
                gSpriteAiDropTimer = 0;
                gStageExitType = 2;
            }
            break;
    }

    if (gCurrentSprite.work2 == 0) {
        scale = 0x100;
    } else {
        scale = gCurrentSprite.work2;
    }
    gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.work1), FixedInverse(scale));
    gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.work1), FixedInverse(scale));
}

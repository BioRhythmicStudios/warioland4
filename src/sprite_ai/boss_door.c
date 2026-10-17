#include "sprite_ai/boss_door.h"

#include "screen_shake.h"
#include "sprite.h"
#include "wario.h"
#include "sound.h"

void SetBossDoorOpeningAnimation(void)
{
    switch (gCurrentSprite.globalID) {
        case PSPRITE_BOSS_DOOR_EMERALD_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorEmeraldOpeningOam);
            break;
        case PSPRITE_BOSS_DOOR_RUBY_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorRubyOpeningOam);
            break;
        case PSPRITE_BOSS_DOOR_TOPAZ_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorTopazOpeningOam);
            break;
        case PSPRITE_BOSS_DOOR_SAPPHIRE_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorSapphireOpeningOam);
            break;
        case PSPRITE_BOSS_DOOR_GOLDEN_PYRAMID:
            GP_SET(gCurrentSprite.pOamData, sBossDoorGoldenPyramidOpeningOam);
            break;
        default:
            GP_SET(gCurrentSprite.pOamData, sBossDoorEntryOpeningOam);
    }
}

void SetBossDoorClosedAnimation(void)
{
    switch (gCurrentSprite.globalID) {
        case PSPRITE_BOSS_DOOR_EMERALD_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorEmeraldClosedOam);
            break;
        case PSPRITE_BOSS_DOOR_RUBY_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorRubyClosedOam);
            break;
        case PSPRITE_BOSS_DOOR_TOPAZ_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorTopazClosedOam);
            break;
        case PSPRITE_BOSS_DOOR_SAPPHIRE_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorSapphireClosedOam);
            break;
        case PSPRITE_BOSS_DOOR_GOLDEN_PYRAMID:
            GP_SET(gCurrentSprite.pOamData, sBossDoorGoldenPyramidClosedOam);
            break;
        default:
            GP_SET(gCurrentSprite.pOamData, sBossDoorEntryClosedOam);
    }
}

void SetBossDoorOpenAnimation(void)
{
    switch (gCurrentSprite.globalID) {
        case PSPRITE_BOSS_DOOR_EMERALD_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorEmeraldOpenOam);
            break;
        case PSPRITE_BOSS_DOOR_RUBY_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorRubyOpenOam);
            break;
        case PSPRITE_BOSS_DOOR_TOPAZ_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorTopazOpenOam);
            break;
        case PSPRITE_BOSS_DOOR_SAPPHIRE_PASSAGE:
            GP_SET(gCurrentSprite.pOamData, sBossDoorSapphireOpenOam);
            break;
        case PSPRITE_BOSS_DOOR_GOLDEN_PYRAMID:
            GP_SET(gCurrentSprite.pOamData, sBossDoorGoldenPyramidOpenOam);
            break;
        default:
            GP_SET(gCurrentSprite.pOamData, sBossDoorEntryOpenOam);
    }
}

void InitBossDoor(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.drawDistanceDown = 64;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 48;
    gCurrentSprite.hitboxExtentUp = 4;
    gCurrentSprite.hitboxExtentDown = 4;
    gCurrentSprite.hitboxExtentLeft = 4;
    gCurrentSprite.hitboxExtentRight = 4;
    SetBossDoorClosedAnimation();
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 1;
    gCurrentSprite.drawPriority = 3;
}

void BossDoorWaitForWario(void)
{
    if (gWarioData.xPosition > 0x600 && gWarioData.pose == 49) {
        gCurrentSprite.pose = 2;
        gCurrentSprite.work0 = 255;
        m4aSongNumStart(SOUND_BOSS_DOOR_WAITING_WARIO);
        ScreenShakeRequestY(32, 129);
    }
}

void BossDoorRise(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        if ((gCurrentSprite.work0 & 31) == 0) {
            ScreenShakeRequestY(32, 129);
        }
        gCurrentSprite.yPosition--;
    } else {
        gCurrentSprite.status |= SPRITE_STATUS_HIDDEN;
        gCurrentSprite.pose = 24;
        ScreenShakeRequestY(32, 129);
    }
}

void SpriteBossDoor(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitBossDoor();
            break;
        case 1:
            BossDoorWaitForWario();
            break;
        case 2:
            BossDoorRise();
            break;
    }
}

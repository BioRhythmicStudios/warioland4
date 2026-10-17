#include "background_registers.h"
#include "score.h"
#include "sprite.h"
#include "sprite_ai/boss_treasure.h"
#include "sprite_ai/treasure.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "tile_effect.h"

void BossTreasureInit(void)
{

    gCurrentSprite.drawPriority |= 0x80;
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.work0 = 20;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 32;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.hitboxExtentUp = 4;
    gCurrentSprite.hitboxExtentDown = 4;
    gCurrentSprite.hitboxExtentLeft = 4;
    gCurrentSprite.hitboxExtentRight = 4;
    gCurrentSprite.warioCollision = 0;

    switch (gBossTreasureType) {
        case 1:
            GP_SET(gCurrentSprite.pOamData, sBossTreasureOam_83DF9E8);
            break;
        case 2:
            GP_SET(gCurrentSprite.pOamData, sBossTreasureOam_83BA7BC);
            break;
        case 3:
            GP_SET(gCurrentSprite.pOamData, sBossTreasureOam_83DAB40);
            break;
        case 4:
            GP_SET(gCurrentSprite.pOamData, sBossTreasureOam_83CF72C);
            break;
        default:
            GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DABA0);
            break;
    }
}

void SpriteBossTreasure(void)
{
    struct PrimarySpriteData* sprite;
    u16 originalY;
    u32 relativeY;
    u8 one;
    s32 completed;
    register u32 coordinate PIN("r1");
    struct PrimarySpriteData* current;
    s32 pose;
    u32 backgroundPosition;
    u8* indexPtr;
    u8* indexBase;
    register const s16* velocities PIN("r8");

    completed = 0;
    sprite = &gCurrentSprite;
    originalY = sprite->yPosition;
    coordinate = originalY >> 2;
    backgroundPosition = gBg1YPosition;
    backgroundPosition >>= 2;
    coordinate -= backgroundPosition;
    relativeY = (u32)(coordinate << 16) >> 16;
    coordinate = sprite->xPosition >> 2;
    backgroundPosition = gBg1XPosition;
    backgroundPosition >>= 2;
    coordinate -= backgroundPosition;
    coordinate = (u32)(coordinate << 16) >> 16;
    one = 1;
    sprite->disableWarioCollisionTimer = one;
    pose = sprite->pose;
    current = sprite;

    switch (pose) {
        case SPOSE_INIT:
            BossTreasureInit();
            break;

        case SPOSE_IDLE: {
            u16 adjustedY;

            if (relativeY > 32) {
                adjustedY = originalY - 2;
                sprite->yPosition = adjustedY;
            } else if (relativeY <= 29) {
                adjustedY = originalY + 2;
                sprite->yPosition = adjustedY;
            } else {
                completed = 1;
            }

            if (coordinate > 122) {
                current->xPosition -= 2;
            } else if (coordinate <= 119) {
                current->xPosition += 2;
            } else {
                completed++;
            }

            if (completed == 2) {
                current->pose = SPOSE_12;
            }
            break;
        }

        case SPOSE_12: {
            u16 movement;
            u16 newY;
            u8 nextIndex;
            const s16* velocity;
            u8 index;
            u8* timer;

            indexPtr = &sprite->work3;
            index = *indexPtr;
            coordinate = (u32)sTreasureFloatYVelocity;
            /* Keep the table load in r1 before agbcc moves it to r8. */
            velocities = (const s16*)coordinate;
            velocity = &velocities[index];
            movement = *(const u16*)velocity;
            if (*velocity == 0x7FFF) {
                movement = *(const u16*)velocities;
                index = 0;
            }
            nextIndex = index + 1;
            coordinate = (u32)indexPtr;
            *(u8*)coordinate = nextIndex;
            newY = originalY + movement;
            sprite->yPosition = newY;

            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            sprite->pose = SPOSE_14;
            if (gCollectedNEJewelPiece == 2) {
                StartTileEffect(0);
                gCollectedNEJewelPiece = one;
                *timer = 80;
            } else {
                *timer = one;
            }
            break;
        }

        case SPOSE_14: {
            u16 movement;
            u16 newY;
            u8 nextIndex;
            const s16* velocity;
            u8 index;
            u8* timer;

            indexBase = (u8*)42;
            /* Prevent agbcc from folding the required constant-first pointer addition. */
            indexBase = (u8*)((u32)indexBase + (u32)sprite);
            indexPtr = indexBase;
            index = *indexBase;
            velocities = sTreasureFloatYVelocity;
            velocity = &velocities[index];
            movement = *(const u16*)velocity;
            if (*velocity == 0x7FFF) {
                movement = *(const u16*)velocities;
                index = 0;
            }
            nextIndex = index + 1;
            coordinate = (u32)indexPtr;
            *(u8*)coordinate = nextIndex;
            newY = originalY + movement;
            sprite->yPosition = newY;

            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            sprite->pose = SPOSE_16;
            if (gCollectedSEJewelPiece == 2) {
                StartTileEffect(1);
                gCollectedSEJewelPiece = one;
                *timer = 80;
            } else {
                *timer = one;
            }
            break;
        }

        case SPOSE_16: {
            const s16* velocities;
            u16 movement;
            u16 newY;
            u8 nextIndex;
            const s16* velocity;
            u8 index;
            u8* timer;

            indexBase = (u8*)42;
            /* Prevent agbcc from folding the required constant-first pointer addition. */
            indexBase = (u8*)((u32)indexBase + (u32)sprite);
            indexPtr = indexBase;
            index = *indexBase;
            velocities = sTreasureFloatYVelocity;
            velocity = &velocities[index];
            movement = *(const u16*)velocity;
            if (*velocity == 0x7FFF) {
                movement = *(const u16*)velocities;
                index = 0;
            }
            nextIndex = index + 1;
            coordinate = (u32)indexPtr;
            *(u8*)coordinate = nextIndex;
            newY = originalY + movement;
            sprite->yPosition = newY;

            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            sprite->pose = SPOSE_17;
            if (gCollectedSWJewelPiece == 2) {
                StartTileEffect(2);
                gCollectedSWJewelPiece = one;
                *timer = 110;
            } else {
                *timer = 31;
            }
            break;
        }

        case SPOSE_17: {
            const s16* velocities;
            u16 movement;
            u16 newY;
            u8 nextIndex;
            const s16* velocity;
            u8 index;
            u8* timer;
            s32 zero;

            indexBase = (u8*)42;
            /* Prevent agbcc from folding the required constant-first pointer addition. */
            indexBase = (u8*)((u32)indexBase + (u32)sprite);
            indexPtr = indexBase;
            index = *indexBase;
            velocities = sTreasureFloatYVelocity;
            velocity = (const s16*)((index << 1) + (u32)velocities);
            movement = *(const u16*)velocity;
            zero = 0;
            coordinate = (s32)*(s16 *)((u8 *)velocity + zero);
            if ((s32)coordinate == 0x7FFF) {
                movement = *(const u16*)velocities;
                index = 0;
            }
            nextIndex = index + 1;
            coordinate = (u32)indexPtr;
            *(u8*)coordinate = nextIndex;
            newY = originalY + movement;
            sprite->yPosition = newY;

            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            SpriteUtilFadeBackgroundToBlack();
            sprite->pose = SPOSE_18;
            *timer = 60;
            break;
        }

        case SPOSE_18: {
            const s16* velocities;
            u16 movement;
            u16 newY;
            u8 nextIndex;
            const s16* velocity;
            u8 index;
            u8* timer;
            s32 zero;
            u8 clear;

            indexBase = (u8*)42;
            /* Prevent agbcc from folding the required constant-first pointer addition. */
            indexBase = (u8*)((u32)indexBase + (u32)sprite);
            indexPtr = indexBase;
            index = *indexBase;
            velocities = sTreasureFloatYVelocity;
            velocity = (const s16*)((index << 1) + (u32)velocities);
            movement = *(const u16*)velocity;
            zero = 0;
            coordinate = (s32)*(s16 *)((u8 *)velocity + zero);
            if ((s32)coordinate == 0x7FFF) {
                movement = *(const u16*)velocities;
                index = 0;
            }
            nextIndex = index + 1;
            coordinate = (u32)indexPtr;
            *(u8*)coordinate = nextIndex;
            newY = originalY + movement;
            sprite->yPosition = newY;

            if (gGoldenDivaRoomTimer[0] != 0) {
                break;
            }
            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            sprite->pose = SPOSE_CRUSHED_OR_COLLECTED;
            clear = 0;
            *timer = clear;
            VoiceSetPlay(VS_20);
            break;
        }

        case SPOSE_CRUSHED_OR_COLLECTED:
            if (sprite->status & SPRITE_STATUS_ONSCREEN) {
                u8* timer;
                u16 adjustedY;

                adjustedY = originalY - 8;
                sprite->yPosition = adjustedY;
                timer = &sprite->work0;
                if ((*timer & 7) == 0) {
                    SpawnSecondarySprite(sprite->yPosition + 32, sprite->xPosition, SSPRITE_47);
                }
                (*timer)++;
            } else {
                sprite->pose = SPOSE_71;
                completed = SpriteUtilFindBossTreasureChest(0);
                if (completed == 0xFF) {
                    u8* timer;

                    timer = &sprite->work0;
                    *timer = one;
                } else {
                    u8* timer;

                    timer = &sprite->work0;
                    *timer = 60;
                    gSpriteData[completed].pose = SPOSE_14;
                    VoiceSetPlay(VS_20);
                }
            }
            break;

        case SPOSE_71: {
            u8* timer;
            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            sprite->pose = SPOSE_73;
            completed = SpriteUtilFindBossTreasureChest(1);
            if (completed == 0xFF) {
                *timer = one;
            } else {
                *timer = 60;
                gSpriteData[completed].pose = SPOSE_14;
                VoiceSetPlay(VS_20);
            }
            break;
        }

        case SPOSE_73: {
            register u8* timer PIN("r5");
            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            sprite->pose = SPOSE_6D;
            completed = SpriteUtilFindBossTreasureChest(2);
            if (completed == 0xFF) {
                *timer = one;
            } else {
                *timer = 60;
                gSpriteData[completed].pose = SPOSE_14;
                VoiceSetPlay(VS_20);
            }
            break;
        }

        case SPOSE_6D: {
            u8* timer;

            timer = &sprite->work0;
            if (--*timer != 0) {
                break;
            }
            sprite->status = SPRITE_STATUS_NONE;
            gSubGameMode = 6;
            gSpriteAiDropTimer = 0;
            gStageExitType = 5;
            break;
        }
    }
}

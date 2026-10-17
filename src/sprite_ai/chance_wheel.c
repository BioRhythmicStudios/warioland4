#include "fixed_point.h"
#include "score.h"
#include "sound.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "wario.h"
#include "sprite_ai/chance_wheel.h"

void ClearChanceWheelRewardSprites(void)
{
    struct PrimarySpriteData* sprite;
    register struct PrimarySpriteData* end PIN("r4");
    u16 exists;
    s16 check;
    int id;
    u16 zero;

    sprite = gSpriteData;
    end = (struct PrimarySpriteData*)((u8*)sprite + 253 * 4);
    exists = SPRITE_STATUS_EXISTS;
    do {
        check = exists;
        check &= sprite->status;
        if (check != 0) {
            id = sprite->globalID;
            switch (id) {
                case PSPRITE_COIN_10POINTS:
                case PSPRITE_COIN_50POINTS:
                case PSPRITE_COIN_100POINTS:
                case PSPRITE_COIN_500POINTS:
                case PSPRITE_COIN_1000POINTS:
                case PSPRITE_DICE:
                case PSPRITE_CHANCE_WHEEL_DIAMOND:
                    goto clear;
                default:
                    goto next;
            }
clear:
            zero = 0;
            /* Keep zero in r0 for the target halfword store. */
            ASM_BARRIER(: "+r"(zero));
            sprite->status = zero;
        }
next:
        sprite++;
    } while ((s32)sprite <= (s32)end);
}

void TransformWarioFromChanceWheel(void)
{
    switch (gCurrentRoom) {
        case 1:
        case 7:
        case 8:
            SpriteCollisionTransformWarioFlaming();
            break;
        case 2:
        case 3:
            SpriteCollisionTransformWarioFat();
            break;
        case 4:
            SpriteCollisionTransformWarioFlat();
            break;
        case 5:
            SpriteCollisionTransformWarioBouncy();
            break;
    }
}

void PlayChanceWheelSpaceSound(void)
{
    struct SecondarySprite* secondary;
    register struct SecondarySprite* saved PIN("r4");
    struct SecondarySprite* previous;
    int space;
    int i;

    space = 0;
    i = 0;
    secondary = gSecondarySpriteData;
    saved = secondary;
    do {
        previous = saved;
        if (secondary->status != 0 && secondary->id == 81) {
            space = sBigBoardSpaces[saved->animationTimer];
            break;
        }
        secondary++;
        saved = previous;
        saved++;
        i++;
    } while (i <= 7);

    switch (space) {
        case 4:
            m4aSongNumStart(SE_THUNDER);
            break;
        case 1:
            m4aSongNumStart(SE_CHANCE_WHEEL_SWITCH_REWARD_ALT);
            break;
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
            break;
    }
}

void ApplyChanceWheelReward(void)
{
    struct SecondarySprite* secondary;
    register struct SecondarySprite* saved PIN("r4");
    struct SecondarySprite* previous;
    int y;
    register int x PIN("r5");
    int space;
    int i;
    u32 spawnY;

    y = gCurrentSprite.yPosition;
    x = gCurrentSprite.xPosition;
    space = 0;
    i = 0;
    secondary = gSecondarySpriteData;
    saved = secondary;
    do {
        previous = saved;
        if (secondary->status != 0 && secondary->id == 81) {
            space = sBigBoardSpaces[saved->animationTimer];
            break;
        }
        secondary++;
        saved = previous;
        saved++;
        i++;
    } while (i <= 7);

    switch (space) {
        case 2:
            m4aSongNumStart(SE_CHANCE_WHEEL_DIAMOND_REWARD);
            if (gCurrentSprite.globalID == PSPRITE_GOAL_BLOCK) {
                spawnY = y;
                spawnY -= 64;
                SpriteSpawnAsChild(PSPRITE_CHANCE_WHEEL_DIAMOND, 0, 0, spawnY, x);
            } else {
                SpriteSpawnAsChild(PSPRITE_CHANCE_WHEEL_DIAMOND, 0, 0, y, x);
            }
            break;
        case 6:
            gHeartMeter.filling++;
            if (gHeartMeter.current + gHeartMeter.filling > 8) {
                gHeartMeter.filling = 8 - gHeartMeter.current;
            }
            gHeartMeter.unk_2 = gHeartMeter.filling * 8;
            gHeartMeter.unk_3 = 0;
            m4aSongNumStart(SE_GET_HEART);
            break;
        case 3:
            m4aSongNumStart(SE_CHANCE_WHEEL_DICE_REWARD);
            if (gCurrentSprite.globalID == PSPRITE_GOAL_BLOCK) {
                spawnY = y;
                spawnY += 72;
            } else {
                spawnY = y;
                spawnY += 64;
            }
            SpriteSpawnAsChild(PSPRITE_DICE, 0, 6, spawnY, x);
            SpriteSpawnAsChild(PSPRITE_DICE, 0, 6, y, x + 80);
            {
                register int spawnX PIN("r0");

                spawnX = x;
                spawnX -= 80;
                SpriteSpawnAsChild(PSPRITE_DICE, 0, 6, y, spawnX);
            }
            break;
        case 4:
            gShopItemState = 5;
            WarioRequestPose(19);
            ChangeStageScoreAndDropCoins(-40);
            break;
        case 5:
            m4aSongNumStart(SE_CHANCE_WHEEL_SWITCH_REWARD);
            gSwitchStates[1] |= 2;
            break;
        case 7:
            m4aSongNumStart(SE_CHANCE_WHEEL_TRANSFORMATION_REWARD);
            TransformWarioFromChanceWheel();
            break;
        case 1:
            gSwitchStates[2] |= 2;
            break;
    }
}

int IsChanceWheelResultReady(void)
{
    struct SecondarySprite* secondary;
    int i;
    int value;

    i = 0;
    secondary = gSecondarySpriteData;
loop:
    value = secondary->status;
    if (value == 0) {
        goto next;
    }
    value = secondary->id;
    if (value != 81) {
        goto next;
    }
    value = secondary->pose;
    if (value == 5) {
        return TRUE;
    }
next:
    secondary++;
    i++;
    if (i <= 7) {
        goto loop;
    }
    return FALSE;
}

void SetChanceWheelSecondaryResult(u8 value)
{
    struct SecondarySprite* secondary;
    int i;
    u8 work;
    register u8 timer PIN("r4");
    u16 check;

    work = value;
    i = 0;
    timer = 40;
    secondary = gSecondarySpriteData;
loop:
    check = secondary->status;
    if (check == 0) {
        goto next;
    }
    check = secondary->id;
    if (check != 81) {
        goto next;
    }
    secondary->work0 = work;
    secondary->timer = timer;
    goto end;
next:
    secondary++;
    i++;
    if (i <= 7) {
        goto loop;
    }
end:
    return;
}

void CheckWarioNearChanceWheel(void)
{
    int warioY;
    int warioX;
    int spriteY;
    int spriteX;
    int bound;

    if (gWarioData.reaction == 0) {
        warioY = gWarioData.yPosition;
        warioX = gWarioData.xPosition;
        spriteY = gCurrentSprite.yPosition;
        spriteX = gCurrentSprite.xPosition;

        bound = spriteX;
        bound -= 130;
        if (bound >= warioX) {
            return;
        }

        bound = spriteX;
        bound += 130;
        if (bound <= warioX) {
            return;
        }

        bound = spriteY + 330;
        if (bound <= warioY) {
            return;
        }

        bound = spriteY;
        bound -= 64;
        if (bound >= warioY) {
            return;
        }

        gUnk_3000038 = 1;
    }
}

void InitChanceWheel(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND | SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.drawPriority |= 0x80;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 4;
    gCurrentSprite.hitboxExtentDown = 64;
    gCurrentSprite.hitboxExtentLeft = 28;
    gCurrentSprite.hitboxExtentRight = 24;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
        GP_SET(gCurrentSprite.pOamData, sChanceWheelIdleOam);
    } else {
        GP_SET(gCurrentSprite.pOamData, sGoalBlockIdleOam);
    }
    gCurrentSprite.pose = SPOSE_IDLE;
    gCurrentSprite.warioCollision = 54;
    gCurrentSprite.xPosition += 32;
    gCurrentSprite.yPosition -= 64;
    func_80269EC();
}

void StartChanceWheelResult(void)
{
    int value;
    int result;

    gCurrentSprite.pose = 109;
    gCurrentSprite.warioCollision = 0;
    gCurrentSprite.work0 = 9;
    gCurrentSprite.work3 = 0;
    value = gCurrentSprite.animationTimer / 6;
    gCurrentSprite.work2 = value;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    value = gCurrentSprite.work2;

    switch (value) {
        case 0:
            if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
                GP_SET(gCurrentSprite.pOamData, sChanceWheelResult2Oam);
            } else {
                GP_SET(gCurrentSprite.pOamData, sGoalBlockResult2Oam);
            }
            result = 2;
            break;
        case 1:
            if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
                GP_SET(gCurrentSprite.pOamData, sChanceWheelResult3Oam);
            } else {
                GP_SET(gCurrentSprite.pOamData, sGoalBlockResult3Oam);
            }
            result = 3;
            break;
        case 2:
            if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
                GP_SET(gCurrentSprite.pOamData, sChanceWheelResult4Oam);
            } else {
                GP_SET(gCurrentSprite.pOamData, sGoalBlockResult4Oam);
            }
            result = 4;
            break;
        case 3:
            if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
                GP_SET(gCurrentSprite.pOamData, sChanceWheelResult5Oam);
            } else {
                GP_SET(gCurrentSprite.pOamData, sGoalBlockResult5Oam);
            }
            result = 5;
            break;
        case 4:
            if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
                GP_SET(gCurrentSprite.pOamData, sChanceWheelResult6Oam);
            } else {
                GP_SET(gCurrentSprite.pOamData, sGoalBlockResult6Oam);
            }
            result = 6;
            break;
        default:
            if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
                GP_SET(gCurrentSprite.pOamData, sChanceWheelResult1Oam);
            } else {
                GP_SET(gCurrentSprite.pOamData, sGoalBlockResult1Oam);
            }
            result = 1;
            break;
    }

    SetChanceWheelSecondaryResult(result);
    m4aSongNumStart(SE_CHANCE_WHEEL_STOP);
    if (gCurrentSprite.globalID == PSPRITE_CHANCE_WHEEL) {
        gCurrentSprite.health = 0;
        gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
        func_8026A54();
    } else {
        ClearChanceWheelRewardSprites();
    }
}

void ChanceWheelDropToResult(void)
{
    u8 index;
    s16 movement;

    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 != 0) {
        index = gCurrentSprite.work3;
        movement = sChanceWheelDropYVelocity[index];
        if (movement == 0x7FFF) {
            gCurrentSprite.yPosition = sChanceWheelDropYVelocity[index - 1] + gCurrentSprite.yPosition;
        } else {
            gCurrentSprite.work3 = index + 1;
            gCurrentSprite.yPosition += movement;
        }
    } else {
        gCurrentSprite.pose = 111;
        gWarioPauseTimer = 1000;
    }
}

void FinishChanceWheelResult(void)
{
    u8* ptr;
    struct PrimarySpriteData* sprite;
    int value;
    int i;

    ptr = (u8*)&gCurrentSprite;
    ((struct PrimarySpriteData*)ptr)->disableWarioCollisionTimer = 1;
    ((struct PrimarySpriteData*)ptr)->currentAnimationFrame--;
    value = 1;
    sprite = (struct PrimarySpriteData*)ptr;
    ptr = (u8*)gSecondarySpriteData;
    i = 7;
    do {
        if (ptr[0] != 0 && ptr[2] == 81) {
            value = ptr[1];
        }
        ptr += sizeof(struct SecondarySprite);
        i--;
    } while (i >= 0);

    if (value != 0) {
        return;
    }

    sprite->work0 = 90;
    sprite->work3 = value;
    if (sprite->globalID == PSPRITE_GOAL_BLOCK) {
        if (IsChanceWheelResultReady()) {
            sprite->pose = 117;
            sprite->health = value;
            gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] = PERSISTENT_STATUS_DESPAWNED;
            func_8026A54();
        } else {
            sprite->pose = 115;
        }
    } else {
        sprite->pose = 113;
    }
}

void ChanceWheelShrink(void)
{
    u8 timer;

    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.currentAnimationFrame--;
    gCurrentSprite.work0--;
    timer = gCurrentSprite.work0;
    if (timer > 60) {
        if (timer == 62) {
            PlayChanceWheelSpaceSound();
        }
    } else {
        if (timer == 0) {
            gCurrentSprite.status = 0;
        } else {
            if (timer == 19) {
                gWarioPauseTimer = 0;
                SpriteSpawnAsChild(PSPRITE_A2, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                    gCurrentSprite.yPosition, gCurrentSprite.xPosition);
            } else if (timer == 16) {
                ApplyChanceWheelReward();
            }
            {
                struct PrimarySpriteData* sprite;
                s32 scale;
                s32 sine;
                s32 cosine;
                u8* work3;

                sprite = &gCurrentSprite;
                work3 = &sprite->work3;
                *work3 -= 4;
                scale = *work3;
                sprite->status |= SPRITE_STATUS_AFFINE;
                cosine = COS(0);
                sprite->affinePA = FixedMul(cosine, FixedInverse(scale));
                sine = SIN(0);
                sprite->affinePB = FixedMul(sine, FixedInverse(scale));
                sine = -sine;
                sprite->affinePC = FixedMul(sine, FixedInverse(scale));
                sprite->affinePD = FixedMul(cosine, FixedInverse(scale));
            }
        }
    }
}

void ResetGoalBlock(void)
{
    u8 timer;

    gCurrentSprite.currentAnimationFrame--;
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0--;
    timer = gCurrentSprite.work0;
    if (timer > 60) {
        if (timer == 62) {
            PlayChanceWheelSpaceSound();
        }
    } else if (timer == 0) {
        switch (gCurrentSprite.work2) {
            case 0: gCurrentSprite.animationTimer = 5; break;
            case 1: gCurrentSprite.animationTimer = 11; break;
            case 2: gCurrentSprite.animationTimer = 17; break;
            case 3: gCurrentSprite.animationTimer = 23; break;
            case 4: gCurrentSprite.animationTimer = 29; break;
            default: gCurrentSprite.animationTimer = 35; break;
        }
        gCurrentSprite.currentAnimationFrame = 0;
        GP_SET(gCurrentSprite.pOamData, sGoalBlockIdleOam);
        gCurrentSprite.pose = SPOSE_IDLE;
        gCurrentSprite.warioCollision = 54;
    } else if (timer == 19) {
        gWarioPauseTimer = 0;
    } else if (timer == 16) {
        ApplyChanceWheelReward();
    }
}

void GoalBlockShrink(void)
{
    u8 timer;
    s32 scale;
    s32 sine;
    s32 cosine;
    u8* work3;

    gCurrentSprite.currentAnimationFrame--;
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0--;
    timer = gCurrentSprite.work0;
    if (timer > 60) {
        if (timer == 62) {
            PlayChanceWheelSpaceSound();
        }
    } else {
        if (timer == 0) {
            gCurrentSprite.status = 0;
        } else {
            if (timer == 20) {
                gWarioPauseTimer = 0;
                SpriteSpawnAsChild(PSPRITE_A2, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
                    gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                ApplyChanceWheelReward();
            }
            work3 = &gCurrentSprite.work3;
            *work3 -= 4;
            scale = *work3;
            gCurrentSprite.status |= SPRITE_STATUS_AFFINE;
            cosine = COS(0);
            gCurrentSprite.affinePA = FixedMul(cosine, FixedInverse(scale));
            sine = SIN(0);
            gCurrentSprite.affinePB = FixedMul(sine, FixedInverse(scale));
            sine = -sine;
            gCurrentSprite.affinePC = FixedMul(sine, FixedInverse(scale));
            gCurrentSprite.affinePD = FixedMul(cosine, FixedInverse(scale));
        }
    }
}

void InitChanceWheelRewardEffect(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 32;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.hitboxExtentUp = 4;
    gCurrentSprite.hitboxExtentDown = 4;
    gCurrentSprite.hitboxExtentLeft = 4;
    gCurrentSprite.hitboxExtentRight = 4;
    GP_SET(gCurrentSprite.pOamData, sChanceWheelRewardEffectOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.warioCollision = 6;
    gCurrentSprite.work0 = 30;
    gCurrentSprite.pose = SPOSE_IDLE;
}

void UpdateChanceWheelRewardEffect(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.status = 0;
    }
}

void SpriteChanceWheel(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            if (IsChanceWheelResultReady()) {
                gCurrentSprite.status = 0;
                gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
            } else {
                InitChanceWheel();
            }
            break;
        case 108:
            gUnk_3000038 = 1;
            StartChanceWheelResult();
            break;
        case 109:
            gUnk_3000038 = 1;
            ChanceWheelDropToResult();
            break;
        case 111:
            gUnk_3000038 = 1;
            FinishChanceWheelResult();
            break;
        case 113:
            gUnk_3000038 = 1;
            ChanceWheelShrink();
            break;
        case 115:
            gUnk_3000038 = 1;
            ResetGoalBlock();
            break;
        case 117:
            gUnk_3000038 = 1;
            GoalBlockShrink();
            break;
        default:
            CheckWarioNearChanceWheel();
            if (IsChanceWheelResultReady()) {
                func_8026A54();
                gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
                SpriteUtilDie();
            }
            break;
    }
}

void SpriteChanceWheelRewardEffect(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == SPOSE_INIT) {
        InitChanceWheelRewardEffect();
    } else {
        UpdateChanceWheelRewardEffect();
    }
}

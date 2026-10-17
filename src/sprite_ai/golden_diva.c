#include "fixed_point.h"
#include "color_effects.h"
#include "background_registers.h"
#include "block.h"
#include "global_data.h"
#include "hud.h"
#include "gba/m4a.h"
#include "main.h"
#include "oam.h"
#include "score.h"
#include "screen_shake.h"
#include "sound.h"
#include "tile_effect.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "sprite_ai/golden_diva.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

struct AGB_ALIGN GoldenDivaDmaRegs {
    vu32 src;
    vu32 dst;
    vu32 control;
};

/*
 * Any instruction-emitting inline asm in this file is an isolated agbcc
 * compiler boundary. Each site documents the exact register-allocation or
 * optimizer issue that pure C could not reproduce; no gameplay logic is
 * implemented as an asm block.
 */

extern u8 gSpriteAiDropTimer;
extern u8 gShopItemState;
extern u8 gShopItemTimer;
extern u8 gSpriteCollisionResult;
extern s8 gCuckooCondorPendulumLength;
extern u8 gCuckooCondorMoveRight;
extern u8 gCuckooCondorHasCapturedWario;
extern u8 gInitialHealth;
extern u8 gBgAnimationFrame;
extern u8 gBgAnimationTimer;
extern u8 gSpriteAiDynamicGraphicsTimer;
extern u8 gGoldenDivaShopItemHealthTarget;
extern u16 gSpriteAiRadius;

extern const u8 sGoldenDivaFanDamageTiles0[];
extern const u8 sGoldenDivaFanDamageTiles1[];
extern const u8 sGoldenDivaFanDamageTiles2[];
extern const u8 sGoldenDivaFanDamageTiles3[];
extern const struct AnimationFrame sGoldenDivaBodyMoveOam[];
extern const struct AnimationFrame sGoldenDivaBodyIdleOam[];
extern const struct AnimationFrame sGoldenDivaMaskFaceOam[];
extern const struct AnimationFrame sGoldenDivaMaskAttackOam[];
extern const struct AnimationFrame sGoldenDivaMaskSpinOam[];
extern const struct AnimationFrame sGoldenDivaBodyFaceStartOam[];
extern const struct AnimationFrame sGoldenDivaBodyPhaseStartOam[];
extern const struct AnimationFrame sGoldenDivaLowerBodyOam[];
extern const struct AnimationFrame sGoldenDivaFanOam[];
extern const struct AnimationFrame sGoldenDivaBodyFaceLoopOam[];
extern const struct AnimationFrame sGoldenDivaBodyAttackOam[];
extern const struct AnimationFrame sGoldenDivaRobeOam[];
extern const struct AnimationFrame sGoldenDivaExplosiveBallFallOam[];
extern const struct AnimationFrame sGoldenDivaExplosiveBallIdleOam[];
extern const struct AnimationFrame sGoldenDivaExplosiveBallBounceOam[];
extern const struct AnimationFrame sGoldenDivaBlackEggIdleOam[];
extern const struct AnimationFrame sGoldenDivaGreenEnemyTransformOam[];
extern const struct AnimationFrame sGoldenDivaBlackEggLandingOam[];
extern const struct AnimationFrame sGoldenDivaBlackEggSlopeOam[];
extern const struct AnimationFrame sGoldenDivaBlackEggCarryOam[];
extern const struct AnimationFrame sGoldenDivaGreenEnemyIdleOam[];
extern const struct AnimationFrame sGoldenDivaGreenEnemyActiveOam[];
extern const struct AnimationFrame sGoldenDivaBlackChickIdleOam[];
extern const struct AnimationFrame sGoldenDivaBlackChickAttackOam[];
extern const struct AnimationFrame sGoldenDivaBlackChickPose110Oam[];
extern const struct AnimationFrame sGoldenDivaBlackChickPose111Oam[];
extern const struct AnimationFrame sGoldenDivaBlackChickPose111AltOam[];
extern const struct AnimationFrame sGoldenDivaHammerIdleOam[];
extern const struct AnimationFrame sGoldenDivaHammerActiveOam[];
extern const struct AnimationFrame sGoldenDivaFaceIdleOam[];
extern const struct AnimationFrame sGoldenDivaDebrisOam[];
extern const struct AnimationFrame sGoldenDivaAttackEffectOam[];
extern const struct AnimationFrame sGoldenDivaBlackChickFinalOam[];
extern const struct AnimationFrame sGoldenDivaBlackEggFallOam[];
extern const struct AnimationFrame sGoldenDivaBodyDamagedOam[];
extern const struct AnimationFrame sGoldenDivaBodyDamagedAltOam[];
extern const struct AnimationFrame sGoldenDivaRedMaskOam[];
extern const struct AnimationFrame sGoldenDivaGreenMaskOam[];
extern const struct AnimationFrame sGoldenDivaBlueMaskOam[];
extern const struct AnimationFrame sGoldenDivaWhiteMaskOam[];
extern const u8 sGoldenDivaFaceStartGraphicsSequence[];
extern const u8 sGoldenDivaRecoveryGraphicsSequence[];
extern const u8 sGoldenDivaHitGraphicsSequence[];
extern const u8 sGoldenDivaMovementGraphicsSequence[];
extern const u8 sGoldenDivaAttackGraphicsSequence[];
extern const u8 sGoldenDivaDefaultGraphicsSequence[];
extern const u8 sGoldenDivaIntroGraphicsSequence[];
extern const u8 sGoldenDivaLowHealthGraphicsSequence[];
extern const s16 sGoldenDivaHitMotion[];
extern const s16 sGoldenDivaHammerOrbitMotion[];
extern const s16 sGoldenDivaBlackChickFallMotion[];
extern const s16 sGoldenDivaBlackEggBounceMotion[];
extern const s16 sGoldenDivaBlackEggBounceAngles[];
extern const s16 sGoldenDivaMaskAttackXMotion[];
extern const s16 sGoldenDivaMaskAttackYMotion[];
extern const s16 sGoldenDivaMaskAttackPoseMotion[];
extern const s16 sGoldenDivaIntroMotion[];
extern const s16 sGoldenDivaAttackMotion[];
extern const s16 sGoldenDivaMaskBounceMotion[];
extern const s16 sGoldenDivaPhaseTransitionXMotion[];
extern const s16 sGoldenDivaPhaseTransitionYMotion[];
extern const s16 sGoldenDivaExplosiveBallOrbitMotion[];
extern const u32 sGoldenDivaMaskGraphicsPointers[];
extern const u32 sGoldenDivaFanDamageGraphicsPointers[];
extern const u32 sGoldenDivaDynamicGraphicsPointers[];
extern const u8 sGoldenDivaDefeatSpawnSchedule[];
extern const u8 sGoldenDivaTreasureReleaseOrder[];
extern const u8 sGoldenDivaDefeatPalette[];
extern void func_8023FA8(void);


void UpdateGoldenDivaGreenEnemyTransformation(void);
void UpdateGoldenDivaDebrisShrink(void);

void DespawnGoldenDivaAttack(void)
{
    struct PrimarySpriteData *sprite;
    s32 pose;

    if (gCuckooCondorMoveRight != 0)
        return;

    sprite = &gCurrentSprite;
    sprite->status = 0;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 5);
    pose = sprite->pose;
    switch (pose) {
    case 81 ... 88:
        gCurrentCarriedSprite.state = 0;
        break;
    }
}

s32 UpdateGoldenDivaDynamicGraphics(const u8 *animation)
{
    const u8 *table;
    s32 result;
    register u8 *timer PIN("r6");
    u32 temp;
    u8 remaining;
    u8 frame;
    u8 gfx;
    const u8 *src;
    volatile struct GoldenDivaDmaRegs *dma;

    table = animation;
    {
        u8 *counter;

        counter = &gInitialHealth;
        temp = *counter;
        timer = &gSpriteAiDynamicGraphicsTimer;
        if (temp != 0) {
            temp--;
            *counter = temp;
            remaining = temp;
            if (remaining != 0) {
                if (gCurrentSprite.pose != 32 && gCurrentSprite.pose != 50) {
                    table = sGoldenDivaDefaultGraphicsSequence;
                    if (gBgAnimationTimer > 9)
                        gBgAnimationTimer = 10;
                    if (*timer > 8)
                        *timer = 0;
                }
            } else {
                gBgAnimationTimer = remaining;
                *timer = remaining;
            }
        }
    }

    result = 0;
    {
        u8 *timerPointer;

        timerPointer = timer;
        temp = *timerPointer;
        if (temp != 0) {
            temp--;
            *timerPointer = temp;
            return 0;
        }
    }

    frame = gBgAnimationTimer++;
    gfx = table[frame * 4];
    if (gfx == 0xFF) {
        frame = 0;
        gBgAnimationTimer = 1;
        gfx = table[0];
        result = 1;
    }
    *timer = table[frame * 4 + 1];

    dma = (volatile struct GoldenDivaDmaRegs *)0x040000D4;
    src = (const u8 *)sGoldenDivaDynamicGraphicsPointers[gfx];
    dma->src = (u32)src;
    dma->dst = 0x06014000;
    dma->control = 0x80000080;
    (void)dma->control;
    dma->src = (u32)src + 0x400;
    dma->dst = 0x06014400;
    dma->control = 0x80000080;
    (void)dma->control;
    dma->src = (u32)src + 0x800;
    dma->dst = 0x06014800;
    dma->control = 0x80000080;
    (void)dma->control;
    dma->src = (u32)src + 0xC00;
    dma->dst = 0x06014C00;
    dma->control = 0x80000080;
    (void)dma->control;

    return result;
}

void LoadGoldenDivaMaskGraphics(u8 index)
{
    struct GoldenDivaDmaRegs *dma;
    const u8 *src;

    dma = (struct GoldenDivaDmaRegs *)0x040000D4;
    src = (const u8 *)sGoldenDivaMaskGraphicsPointers[index];
    dma->src = (u32)src;
    dma->dst = 0x06017380;
    dma->control = 0x80000040;
    (void)dma->control;
    dma->src = (u32)src + 0x400;
    dma->dst = 0x06017780;
    dma->control = 0x80000040;
    (void)dma->control;
    dma->src = (u32)src + 0x800;
    dma->dst = 0x06017B80;
    dma->control = 0x80000040;
    (void)dma->control;
    dma->src = (u32)src + 0xC00;
    dma->dst = 0x06017F80;
    dma->control = 0x80000040;
    (void)dma->control;
}

void AdvanceGoldenDivaMaskGraphics(void)
{
    if (gCuckooCondorMoveRight == 0)
        return;

    gCuckooCondorHasCapturedWario--;
    switch (--gCuckooCondorMoveRight) {
    case 11:
        LoadGoldenDivaMaskGraphics(0);
        break;
    case 10:
        LoadGoldenDivaMaskGraphics(1);
        break;
    case 9:
        LoadGoldenDivaMaskGraphics(2);
        break;
    case 8:
        LoadGoldenDivaMaskGraphics(3);
        break;
    case 7:
        LoadGoldenDivaMaskGraphics(4);
        break;
    case 6:
        LoadGoldenDivaMaskGraphics(5);
        break;
    case 5:
        LoadGoldenDivaMaskGraphics(6);
        break;
    case 4:
        LoadGoldenDivaMaskGraphics(7);
        break;
    case 3:
        LoadGoldenDivaMaskGraphics(8);
        break;
    case 2:
        LoadGoldenDivaMaskGraphics(9);
        break;
    case 1:
        LoadGoldenDivaMaskGraphics(10);
        break;
    case 0:
        LoadGoldenDivaMaskGraphics(11);
        break;
    }
}

void LoadGoldenDivaFanDamageGraphics(u8 index)
{
    struct GoldenDivaDmaRegs *dma;
    const u8 *src;

    dma = (struct GoldenDivaDmaRegs *)0x040000D4;
    src = (const u8 *)sGoldenDivaFanDamageGraphicsPointers[index];
    dma->src = (u32)src;
    dma->dst = 0x06014240;
    dma->control = 0x800000E0;
    (void)dma->control;
    dma->src = (u32)src + 0x400;
    dma->dst = 0x06014640;
    dma->control = 0x800000E0;
    (void)dma->control;
    dma->src = (u32)src + 0x800;
    dma->dst = 0x06014A40;
    dma->control = 0x800000E0;
    (void)dma->control;
}


void HandleGoldenDivaMaskFanCollision(void)
{
    s32 i;
    u16 yPosition;
    u16 xPosition;
    u16 thisTop;
    u16 thisBottom;
    u16 thisLeft;
    u16 thisRight;
    u16 otherYPosition;
    u16 otherXPosition;
    u16 otherTop;
    u16 otherBottom;
    u16 otherLeft;
    u16 otherRight;

    yPosition = gCurrentSprite.yPosition + 0x1000;
    xPosition = gCurrentSprite.xPosition + 0x1000;
    thisTop = yPosition - gCurrentSprite.hitboxExtentUp;
    thisBottom = yPosition + gCurrentSprite.hitboxExtentDown;
    thisLeft = xPosition - gCurrentSprite.hitboxExtentLeft;
    thisRight = xPosition + gCurrentSprite.hitboxExtentRight;

    for (i = 0; i < MAX_SPRITE_COUNT; i++) {
        if ((gSpriteData[i].status & 1) != 0 && gSpriteData[i].globalID == PSPRITE_F1) {
            otherYPosition = gSpriteData[i].yPosition + 0x1000;
            otherXPosition = gSpriteData[i].xPosition + 0x1000;
            otherTop = otherYPosition - gSpriteData[i].hitboxExtentUp;
            otherBottom = otherYPosition + gSpriteData[i].hitboxExtentDown;
            otherLeft = otherXPosition - gSpriteData[i].hitboxExtentLeft;
            otherRight = otherXPosition + gSpriteData[i].hitboxExtentRight;

            if (SpriteCollisionCheckObjectsTouching(
                    thisTop, thisBottom, thisLeft, thisRight,
                    otherTop, otherBottom, otherLeft, otherRight)) {
                if (gCurrentSprite.health != 0) {
                    gCuckooCondorHasCapturedWario--;
                    switch ((u8)--gCuckooCondorPendulumLength) {
                    case 3:
                        LoadGoldenDivaFanDamageGraphics(2);
                        VoiceSetPlay(VS_17);
                        break;
                    case 2:
                        LoadGoldenDivaFanDamageGraphics(3);
                        VoiceSetPlay(VS_17);
                        break;
                    case 1:
                        LoadGoldenDivaFanDamageGraphics(4);
                        VoiceSetPlay(VS_17);
                        break;
                    case 0:
                        LoadGoldenDivaFanDamageGraphics(5);
                        gSpriteData[i].pose = 17;
                        gSpriteData[i].work0 = 40;
                        break;
                    }
                    gCurrentSprite.status = 0;
                    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 5);
                    SpawnSecondarySprite(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition, 6);
                    SpriteSpawnAsChild(10, 0, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                    VoiceSetPlay(VS_WARIO_TREASURE);
                } else {
                    if (gCurrentCarriedSprite.state != 0)
                        gCurrentCarriedSprite.state = 0;
                    gCurrentSprite.work2 = 8;
                    gCurrentSprite.work3 = 0;
                    gCurrentSprite.disableWarioCollisionTimer = 15;
                    if (gCurrentSprite.xPosition > gSpriteData[i].xPosition)
                        gCurrentSprite.pose = 70;
                    else
                        gCurrentSprite.pose = 68;
                    m4aSongNumStart(SE_WALL_BOUNCE);
                }
                break;
            }
        }
    }
}

void OrbitGoldenDivaChild(void)
{
    u32 yPosition;
    u32 yPositionCopy;
    u32 xPosition;
    register s32 angle PIN("r5");
    u32 amplitude;
    register u32 angleShift PIN("r1");
    u32 r2Value;
    u32 r1Value;
    register s32 temp PIN("r0");
    const u8 *table;
    u32 slot;

    angle = gCurrentSprite.work0;
    if (angle > 0x80)
        gCurrentSprite.status |= 8;
    else
        gCurrentSprite.status &= ~8;

    slot = SpriteUtilFindSpriteSlot(PSPRITE_EC);
    amplitude = gSpriteAiRadius;
    yPosition = (u16)(gSpriteData[slot].yPosition + 56);
    yPositionCopy = yPosition;
    xPosition = gSpriteData[slot].xPosition;

    table = (const u8 *)sSinCosTable;
    angleShift = angle << 16;
    r2Value = (s32)*(const s16 *)(table + (angleShift >> 15));
    angle = angleShift;
    if ((s32)r2Value < 0) {
        r2Value = -(s32)r2Value;
        angleShift = amplitude << 16;
        temp = (s16)(angleShift >> 16) * (s32)r2Value;
        amplitude = angleShift;
        if (temp < 0)
            temp += 0x1FF;
        temp <<= 7;
        {
            s32 signedY;

            r2Value = (u32)&gCurrentSprite;
            signedY = (s16)yPosition;
            ((struct PrimarySpriteData *)r2Value)->yPosition = signedY - (temp >> 16);
        }
    } else {
        angleShift = amplitude << 16;
        temp = (s16)(angleShift >> 16) * (s32)r2Value;
        amplitude = angleShift;
        if (temp < 0)
            temp += 0x1FF;
        temp <<= 7;
        {
            s32 signedY;

            r2Value = (u32)&gCurrentSprite;
            signedY = (s16)yPositionCopy;
            ((struct PrimarySpriteData *)r2Value)->yPosition = signedY + (temp >> 16);
        }
    }

    r1Value = (u32)sSinCosTable;
    temp = (u32)angle >> 15;
    temp += 128;
    temp += r1Value;
    r1Value = (s32)*(const s16 *)temp;
    if ((s32)r1Value < 0) {
        r1Value = -(s32)r1Value;
        temp = (s16)(amplitude >> 16) * (s32)r1Value;
        if (temp < 0)
            temp += 0xFF;
        r1Value = temp << 8;
        temp = (s16)xPosition;
        ((struct PrimarySpriteData *)r2Value)->xPosition = temp - ((s32)r1Value >> 16);
    } else {
        temp = (s16)(amplitude >> 16) * (s32)r1Value;
        if (temp < 0)
            temp += 0xFF;
        r1Value = temp << 8;
        temp = (s16)xPosition;
        ((struct PrimarySpriteData *)r2Value)->xPosition = temp + ((s32)r1Value >> 16);
    }
}


s32 ChooseGoldenDivaPhaseTwoAttack(void)
{
    s32 result;
    s32 count1;
    s32 count2;
    s32 count3;
    s32 count4;
    s32 total;
    struct PrimarySpriteData *sprite;

    result = 1;
    switch ((u8)gBossState) {
    case 8:
        result = SpriteUtilCountSpriteType(PSPRITE_F2);
        if (result != 0)
            goto check_animation;

        count1 = SpriteUtilCountSpriteType(PSPRITE_F3);
        count2 = SpriteUtilCountSpriteType(PSPRITE_F4);
        count3 = SpriteUtilCountSpriteType(PSPRITE_F5);
        count4 = SpriteUtilCountSpriteType(PSPRITE_F6);
        total = count1 + count2;
        total += count3;
        total += count4;
        if (total > 1)
            goto return_one;

        sprite = &gCurrentSprite;
        SpriteSpawnAsChild(PSPRITE_F8, 0, 0, sprite->yPosition + 192, sprite->xPosition - 16);
        if (sprite->xPosition < gWarioData.xPosition)
            SpawnPrimarySpriteWithStatus(PSPRITE_F2, 0, 0, sprite->yPosition + 224, sprite->xPosition - 16, 64);
        else
            SpawnPrimarySpriteWithStatus(PSPRITE_F2, 0, 0, sprite->yPosition + 224, sprite->xPosition - 16, result);
        m4aSongNumStart(SOUND_80);
        break;

    case 9:
        result = SpriteUtilCountSpriteType(PSPRITE_F3);
        if (result == 0)
            goto case_9_empty;
check_animation:
        if (gCuckooCondorMoveRight <= 4)
            result = 0;
        break;

case_9_empty:
        count1 = SpriteUtilCountSpriteType(PSPRITE_F2);
        count2 = SpriteUtilCountSpriteType(PSPRITE_F4);
        count3 = SpriteUtilCountSpriteType(PSPRITE_F5);
        count4 = SpriteUtilCountSpriteType(PSPRITE_F6);
        total = count1 + count2;
        total += count3;
        total += count4;
        if (total > 1)
            goto return_one;

        sprite = &gCurrentSprite;
        SpriteSpawnAsChild(PSPRITE_F8, 0, 0, sprite->yPosition - 64, sprite->xPosition);
        if (sprite->xPosition < gWarioData.xPosition)
            SpawnPrimarySpriteWithStatus(PSPRITE_F3, 0, 0, sprite->yPosition - 32, sprite->xPosition, 64);
        else
            SpawnPrimarySpriteWithStatus(PSPRITE_F3, 0, 0, sprite->yPosition - 32, sprite->xPosition, result);
        m4aSongNumStart(SOUND_7D);
        break;

    case 10:
        result = SpriteUtilCountSpriteType(PSPRITE_F4);
        if (result != 0)
            break;
        result = SpriteUtilCountSpriteType(PSPRITE_F5);
        if (result != 0)
            break;

        count1 = SpriteUtilCountSpriteType(PSPRITE_F3);
        count2 = SpriteUtilCountSpriteType(PSPRITE_F2);
        count3 = SpriteUtilCountSpriteType(PSPRITE_F6);
        total = count1 + count2;
        total += count3;
        if (total > 1)
            goto return_one;

        sprite = &gCurrentSprite;
        if (sprite->xPosition < gWarioData.xPosition) {
            SpriteSpawnAsChild(PSPRITE_F8, 0, 0, sprite->yPosition + 92, sprite->xPosition + 64);
            SpawnPrimarySpriteWithStatus(PSPRITE_F4, 0, 0, sprite->yPosition + 64, sprite->xPosition + 64, 64);
        } else {
            SpriteSpawnAsChild(PSPRITE_F8, 0, 0, sprite->yPosition + 92, sprite->xPosition - 64);
            SpawnPrimarySpriteWithStatus(PSPRITE_F4, 0, 0, sprite->yPosition + 64, sprite->xPosition - 64, result);
        }
        m4aSongNumStart(SOUND_7F);
        break;

    case 11:
        result = SpriteUtilCountSpriteType(PSPRITE_F6);
        if (result != 0)
            break;

        count1 = SpriteUtilCountSpriteType(PSPRITE_F3);
        count2 = SpriteUtilCountSpriteType(PSPRITE_F4);
        count3 = SpriteUtilCountSpriteType(PSPRITE_F5);
        count4 = SpriteUtilCountSpriteType(PSPRITE_F2);
        total = count1 + count2;
        total += count3;
        total += count4;
        if (total <= 1)
            goto case_11_spawn;

return_one:
        result = 1;
        break;

case_11_spawn:
        sprite = &gCurrentSprite;
        SpriteSpawnAsChild(PSPRITE_F8, 0, 0, sprite->yPosition + 160, sprite->xPosition);
        if (sprite->xPosition < gWarioData.xPosition)
            SpawnPrimarySpriteWithStatus(PSPRITE_F6, 0, 0, sprite->yPosition + 192, sprite->xPosition, 64);
        else
            SpawnPrimarySpriteWithStatus(PSPRITE_F6, 0, 0, sprite->yPosition + 192, sprite->xPosition, result);
        m4aSongNumStart(SOUND_7E);
        break;
    }

    return result;
}


void AttachGoldenDivaBodyToChild(u32 xDelta, u32 collisionOffset)
{
    u16 delta;
    u16 offset;
    u32 slot;
    struct PrimarySpriteData *sprite;

    delta = (u16)xDelta;
    offset = (u16)collisionOffset;
    sprite = &gCurrentSprite;
    slot = sprite->work1;
    if (gSpriteData[slot].status & SPRITE_STATUS_FACING_RIGHT) {
        u32 newX;

        newX = delta;
        newX += sprite->xPosition;
        sprite->xPosition = newX;
        func_8023BFC(sprite->yPosition, (u16)(offset + newX));
    } else {
        sprite->xPosition -= delta;
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - offset));
    }

    if (gSpriteCollisionTileType & 0xF) {
        u32 value;
        register u32 pointer PIN("r1");
        u32 data;

        data = (u32)gSpriteData;
        value = sizeof(struct PrimarySpriteData);
        pointer = slot;
        pointer *= value;
        pointer += data;
        value = ((struct PrimarySpriteData *)pointer)->status;
        data = SPRITE_STATUS_FACING_RIGHT;
        value ^= data;
        ((struct PrimarySpriteData *)pointer)->status = value;
    }
}


void InitGoldenDivaSpawner(void)
{
    struct PrimarySpriteData *sprite;
    u32 health;
    u32 zero;
    u32 byteZero;
    u16 *counter;
    register s32 yOffset PIN("r1");
    s32 yValue;

    LoadGoldenDivaFanDamageGraphics(0);
    sprite = &gCurrentSprite;
    zero = 0;
    health = 20;
    sprite->health = health;
    UpdateBossHealthGauge();

    counter = &gSpriteAiRadius;
    byteZero = 0;
    *counter = zero;
    gBossState = byteZero;
    gCuckooCondorPendulumLength = 4;
    gCuckooCondorMoveRight = 12;
    gCuckooCondorHasCapturedWario = health;
    gInitialHealth = byteZero;
    gBossSequenceState = byteZero;
    gBgAnimationFrame = byteZero;
    gGoldenDivaShopItemHealthTarget = byteZero;
    gBgAnimationTimer = byteZero;
    gSpriteAiDynamicGraphicsTimer = byteZero;

    yOffset = -0x3A0;
    /* Keep the signed offset in r1 before copying it to the addition result. */
    ASM_BARRIER(: "=r"(yValue) : "0"(yOffset));
    yOffset = sprite->yPosition;
    yValue += yOffset;
    sprite->yPosition = yValue;
    SpawnHighPriorityPrimarySprite(PSPRITE_EE, sprite->roomSlot, 0, sprite->yPosition + 192, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_F1, sprite->roomSlot, 0, sprite->yPosition + 56, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_EF, sprite->roomSlot, 0, sprite->yPosition + 56, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_EC, sprite->roomSlot, 0, sprite->yPosition, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_ED, sprite->roomSlot, 0, sprite->yPosition + 192, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_F0, 0, 0, sprite->yPosition + 56, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_F0, 1, 0, sprite->yPosition + 56, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_F0, 2, 0, sprite->yPosition + 56, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(PSPRITE_F0, 3, 0, sprite->yPosition + 56, sprite->xPosition);
    sprite->status = zero;
}


void InitGoldenDivaBody(void)
{
    struct PrimarySpriteData *sprite;
    u32 stride;
    u32 zeroHalf;

    sprite = &gCurrentSprite;
    {
        u32 status;
        register u32 temp PIN("r1");
        u32 zeroByte;

        temp = sprite->status;
        {
            register u32 statusBits PIN("r2");

            statusBits = SPRITE_STATUS_DISABLE_DRAW_DISTANCE | SPRITE_STATUS_IGNORE_SPRITE_COLLISION |
                SPRITE_STATUS_BACKGROUND;
            /* Materialize the status bits in r2 before copying them to r0. */
            ASM_BARRIER(: "+r"(statusBits));
            status = statusBits;
            ASM_BARRIER(: "+r"(status));
        }
        zeroByte = 0;
        ASM_BARRIER(: "+r"(zeroByte));
        zeroHalf = 0;
        status |= temp;
        temp = 0xFFFB;
        status &= temp;
        sprite->status = status;
        sprite->warioCollision = zeroByte;
        sprite->drawDistanceDown = 40;
        sprite->drawDistanceUp = 40;
        sprite->drawDistanceLeftRight = 24;
        sprite->hitboxExtentUp = 32;
        sprite->hitboxExtentDown = 112;
        sprite->hitboxExtentLeft = 48;
        status = (u32)&sprite->hitboxExtentRight;
        stride = sizeof(struct PrimarySpriteData);
        *(u8 *)status = stride;
        GP_SET(sprite->pOamData, sGoldenDivaBodyIdleOam);
        sprite->currentAnimationFrame = zeroByte;
        sprite->animationTimer = zeroHalf;
        sprite->health = gCuckooCondorHasCapturedWario;
        sprite->pose = 110;
        status = (u32)&sprite->work3;
        *(u8 *)status = zeroByte;
        status -= 3;
        *(u8 *)status = zeroByte;
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaIntroGraphicsSequence);
    }
    gShopItemTimer = 1;

    {
        u32 index;
        u32 child;
        register u32 data PIN("r2");
        u32 slotPointer;

        index = SpriteUtilFindSpriteSlot(PSPRITE_EF);
        slotPointer = (u32)&sprite->work2;
        *(u8 *)slotPointer = index;
        data = (u32)gSpriteData;
        /* Load the child array base before reading the stored slot. */
        index = *(u8 *)slotPointer;
        /* Keep the stored slot in r0 before copying it to the multiply register. */
        child = index;
        child *= stride;
        child += data;
        index = sprite->yPosition;
        index += 56;
        ((struct PrimarySpriteData *)child)->yPosition = index;
        index = *(u8 *)slotPointer;
        index *= stride;
        index += data;
        child = sprite->xPosition;
        ((struct PrimarySpriteData *)index)->xPosition = child;
    }
}


void UpdateGoldenDivaShopItemCharge(void)
{
    struct PrimarySpriteData *sprite;
    u8 *health;
    u32 animation;

    switch (gCurrentShopItem) {
    case 3:
        animation = 18;
        break;
    case 4:
        animation = 17;
        break;
    case 5:
        animation = 12;
        break;
    case 6:
        animation = 15;
        break;
    case 7:
        animation = 14;
        break;
    case 8:
        animation = 13;
        break;
    case 2:
    default:
        animation = 19;
        break;
    }

    gGoldenDivaShopItemHealthTarget = animation;
    gBgAnimationFrame = 32;
    sprite = &gCurrentSprite;
    sprite->pose = 117;
    health = &gCuckooCondorHasCapturedWario;
    if (*health != 0) {
        AdvanceGoldenDivaMaskGraphics();
        sprite->health = *health;
        UpdateBossHealthGauge();
        m4aSongNumStart(SE_DAMAGE);
    }
    VoiceSetPlay(17);
}

void UpdateGoldenDivaShopItemHit(void)
{
    u32 timerOrPhase;
    u32 initialTimer;
    register u32 value PIN("r0");
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *spriteCopy;
    u32 field;
    u32 remaining;

    timerOrPhase = (u32)&gBgAnimationFrame;
    value = *(u8 *)timerOrPhase;
    initialTimer = value;
    if (initialTimer != 0) {
        value--;
        *(u8 *)timerOrPhase = value;
        remaining = (u8)value;
        value = 7;
        value &= remaining;
        if (value != 0)
            return;

        value = 8;
        value &= remaining;
        {
            u32 phase;

            /* Compiler boundary: pure C removes this required byte-truncation sequence. */
            phase = (u8)(value);
            if (phase != 0) {
                gSpriteData[gCurrentSprite.work2].palette = 4;
            } else {
                gSpriteData[gCurrentSprite.work2].palette = phase;
            }
        }
        return;
    }

    sprite = &gCurrentSprite;
    field = (u32)&gGoldenDivaShopItemHealthTarget;
    value = sprite->health;
    spriteCopy = sprite;
    field = *(u8 *)field;
    if (value == field) {
        sprite->pose = 118;
        sprite->work0 = initialTimer;
        switch (gCurrentShopItem) {
        case 1:
        case 2:
            field = (u32)spriteCopy;
            ((struct PrimarySpriteData *)field)->work1 = 110;
            break;
        case 4:
            field = (u32)spriteCopy;
            ((struct PrimarySpriteData *)field)->work1 = 240;
            break;
        case 5:
            field = (u32)spriteCopy;
            goto setWork1To120;
        case 6:
            field = (u32)spriteCopy;
            ((struct PrimarySpriteData *)field)->work1 = 255;
            break;
        case 3:
        case 8:
            field = (u32)spriteCopy;
            ((struct PrimarySpriteData *)field)->work1 = 180;
            break;
        case 7:
            field = (u32)spriteCopy;
            ASM_BARRIER(: "+r"(field));
            goto setWork1To120;
        default:
            field = (u32)sprite;
setWork1To120:
            ((struct PrimarySpriteData *)field)->work1 = 120;
            break;
        }
    } else {
        *(u8 *)timerOrPhase = 32;
        {
            u8 *health;

            health = &gCuckooCondorHasCapturedWario;
            if (*health != 0) {
                AdvanceGoldenDivaMaskGraphics();
                sprite->health = *health;
                UpdateBossHealthGauge();
                m4aSongNumStart(SE_DAMAGE);
            }
        }
    }
}


void TriggerGoldenDivaShopItem(void)
{
    register u32 slot PIN("r2");
    u32 address;
    u32 base;
    u32 spriteData;
    u32 stride;
    u32 zero;

    switch (gCurrentShopItem) {
    case 1:
        slot = SpriteUtilFindSpriteSlot(PSPRITE_SHOPKEEPER);
        ASM_BARRIER(: "+r"(slot));
        base = (u32)gSpriteData;
        address = sizeof(struct PrimarySpriteData);
        address *= slot;
        address += base;
        base = 63;
        ((struct PrimarySpriteData *)address)->pose = base;
        break;
    case 2:
        slot = SpriteUtilFindSpriteSlot(PSPRITE_SHOPKEEPER);
        ASM_BARRIER(: "+r"(slot));
        spriteData = (u32)gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot;
        address *= stride;
        address += spriteData;
        zero = 0;
        base = 63;
        ((struct PrimarySpriteData *)address)->pose = base;
        address = PSPRITE_ITEM_BLAST_CANNON;
        goto findItemSlot;
    case 3:
        slot = SpriteUtilFindSpriteSlot(PSPRITE_SHOPKEEPER);
        ASM_BARRIER(: "+r"(slot));
        spriteData = (u32)gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot;
        address *= stride;
        address += spriteData;
        zero = 0;
        base = 63;
        ((struct PrimarySpriteData *)address)->pose = base;
        address = PSPRITE_ITEM_VIZORMAN;
        goto findItemSlot;
    case 4:
        slot = SpriteUtilFindSpriteSlot(PSPRITE_SHOPKEEPER);
        ASM_BARRIER(: "+r"(slot));
        spriteData = (u32)gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot;
        address *= stride;
        address += spriteData;
        zero = 0;
        base = 63;
        ((struct PrimarySpriteData *)address)->pose = base;
        address = PSPRITE_ITEM_BUGLE;
findItemSlot:
        /* Compiler boundary: the u8 prototype would truncate the live u32 ID in r0 before the direct call. */
        address = ((u32 (*)(u32))SpriteUtilFindSpriteSlot)(address);
        slot = address;
        ASM_BARRIER(: "+r"(slot));
        address = slot;
        address *= stride;
        address += spriteData;
        ((struct PrimarySpriteData *)address)->status = zero;
        {
            u32 y;
            u32 x;

            y = ((struct PrimarySpriteData *)address)->yPosition;
            x = ((struct PrimarySpriteData *)address)->xPosition;
            SpriteSpawnAsChild(PSPRITE_0B, 0, 0, y, x);
        }
        break;
    case 5:
        address = SpriteUtilFindSpriteSlot(PSPRITE_C1);
        slot = address;
        base = (u32)gSpriteData;
        address = sizeof(struct PrimarySpriteData);
        address *= slot;
        address += base;
        base = PSPRITE_SHOPKEEPER;
        ((struct PrimarySpriteData *)address)->globalID = base;
        base = 63;
        ((struct PrimarySpriteData *)address)->pose = base;
        break;
    case 6:
        address = SpriteUtilFindSpriteSlot(PSPRITE_C2);
        slot = address;
        base = (u32)gSpriteData;
        address = sizeof(struct PrimarySpriteData);
        address *= slot;
        address += base;
        base = PSPRITE_SHOPKEEPER;
        ((struct PrimarySpriteData *)address)->globalID = base;
        base = 63;
        ((struct PrimarySpriteData *)address)->pose = base;
        break;
    case 7:
        address = SpriteUtilFindSpriteSlot(PSPRITE_C3);
        slot = address;
        base = (u32)gSpriteData;
        address = sizeof(struct PrimarySpriteData);
        address *= slot;
        address += base;
        base = PSPRITE_SHOPKEEPER;
        ((struct PrimarySpriteData *)address)->globalID = base;
        base = 63;
        ((struct PrimarySpriteData *)address)->pose = base;
        break;
    case 8:
        address = SpriteUtilFindSpriteSlot(PSPRITE_C4);
        slot = address;
        base = (u32)gSpriteData;
        address = sizeof(struct PrimarySpriteData);
        address *= slot;
        address += base;
        base = PSPRITE_SHOPKEEPER;
        ((struct PrimarySpriteData *)address)->globalID = base;
        base = 65;
        ((struct PrimarySpriteData *)address)->pose = base;
        break;
    }
}


void UpdateGoldenDivaBodyPose118(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    s32 value;

    sprite = &gCurrentSprite;
    timer = &sprite->work1;
    value = *timer;
    if (value != 0) {
        value--;
        *timer = value;
        value <<= 24;
        if (value == 0)
            StartGoldenDivaPaletteFade(4, 0, 16);
        return;
    }

    if (gGoldenDivaRoomTimer[0] != 0)
        return;

    timer = &sprite->work0;
    value = *timer;
    value++;
    *timer = value;
    value = *timer;
    switch (value) {
    case 60:
        SpawnSecondarySprite(sprite->yPosition + 48, sprite->xPosition + 20, 67);
        SpawnSecondarySprite(sprite->yPosition + 48, sprite->xPosition - 16, 67);
        m4aSongNumStart(SOUND_C2);
        break;
    case 70:
        gShopItemState = 3;
        m4aSongNumStart(SOUND_C3);
        break;
    case 80:
        TriggerGoldenDivaShopItem();
        m4aSongNumStart(SOUND_C4);
        break;
    case 100:
        StartGoldenDivaPaletteFade(3, 0, 16);
        sprite->pose = 119;
        m4aSongNumStart(SOUND_7C);
        break;
    }
}

void UpdateGoldenDivaBodyPose119(void)
{
    struct PrimarySpriteData *sprite;
    u32 pose;
    u32 zero;

    zero = gGoldenDivaRoomTimer[0];
    if (zero != 0)
        return;

    sprite = &gCurrentSprite;
    pose = 112;
    sprite->pose = pose;
    sprite->work0 = zero;
    sprite->work1 = zero;
}

void BeginGoldenDivaIntro(void)
{
    u8 *timer;
    register struct PrimarySpriteData *spriteBase PIN("r0");
    register u32 value PIN("r0");
    u32 count;
    struct PrimarySpriteData *sprite;
    u32 y;
    u32 temp;

    spriteBase = &gCurrentSprite;
    ASM_BARRIER(: "+r"(spriteBase));
    timer = (u8 *)spriteBase;
    timer += 39;
    value = *timer;
    if (value == 0) {
        value = gShopItemTimer;
        if (value != 2)
            return;
        m4aSongNumStart(SOUND_C1);
        value = *timer;
        value++;
        *timer = value;
    }

    count = *timer;
    if (count <= 254) {
        temp = gMainTimer;
        value = 1;
        value &= temp;
        if (value != 0) {
            value = count + 1;
            *timer = value;
        }
        value = *timer;
        if (value == 180)
            m4aSongNumStart(SOUND_78);
    }

    sprite = &gCurrentSprite;
    y = sprite->yPosition;
    value = y >> 2;
    temp = gBg1YPosition;
    temp >>= 2;
    value -= temp;
    value = (u16)value;
    if (value == 56)
        goto reachedTarget;

    value = y + 1;
    sprite->yPosition = value;
    {
        register u32 data PIN("r2");
        u32 pointer;

        data = (u32)gSpriteData;
        pointer = (u32)sprite;
        pointer += 41;
        temp = *(u8 *)pointer;
        pointer = sizeof(struct PrimarySpriteData);
        pointer *= temp;
        pointer += data;
        temp = y;
        temp += 57;
        ((struct PrimarySpriteData *)pointer)->yPosition = temp;
    }
    return;

reachedTarget:
    temp = (u32)sprite;
    temp += 39;
    value = 60;
    *(u8 *)temp = value;
    value = 111;
    sprite->pose = value;
}


void UpdateGoldenDivaIntro(void)
{
    struct PrimarySpriteData *sprite3;
    register u8 *timer PIN("r12");
    u32 value;
    u32 temp;
    u32 zero;
    register u32 scratch6 PIN("r6");
    register struct PrimarySpriteData *sprite4 PIN("r4");
    register const s16 *table PIN("r5");
    u32 velocity;

    sprite3 = &gCurrentSprite;
    value = 39;
    ASM_BARRIER(: "+r"(value));
    value += (u32)sprite3;
    timer = (u8 *)value;
    value = *(u8 *)value;
    if (value != 0) {
        value--;
        temp = (u32)timer;
        *(u8 *)temp = value;
        value <<= 24;
        if (value == 0) {
            temp = (u32)&gShopItemTimer;
            value = 3;
            *(u8 *)temp = value;
        }
    } else {
        zero = gShopItemTimer;
        if (zero == 0) {
            temp = (u32)&gBossState;
            value = 1;
            *(u8 *)temp = value;
            value = 112;
            sprite3->pose = value;
            scratch6 = (u32)timer;
            *(u8 *)scratch6 = zero;
            value = (u32)sprite3;
            value += 40;
            *(u8 *)value = zero;

            {
                u32 shopItem;

                shopItem = gCurrentShopItem;
                switch (shopItem) {
                case 1:
                    m4aSongNumStart(SOUND_2AC);
                    break;
                default:
                    m4aSongNumStart(SOUND_2AB);
                    break;
                case 0:
                    m4aSongNumStart(SOUND_7C);
                    break;
                }
            }
        }
    }

    sprite4 = &gCurrentSprite;
    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += (u32)sprite4;
    timer = (u8 *)value;
    zero = *(u8 *)value;
    table = sGoldenDivaIntroMotion;
    value = zero << 1;
    value += (u32)table;
    velocity = *(const u16 *)value;
    scratch6 = 0;
    temp = *(const s16 *)(value + scratch6);
    value = 0x7FFF;
    if ((s32)temp == (s32)value) {
        velocity = *(const u16 *)table;
        zero = 0;
    }
    value = zero + 1;
    temp = (u32)timer;
    *(u8 *)temp = value;
    value = sprite4->yPosition;
    value += velocity;
    sprite4->yPosition = value;
}

void FinishGoldenDivaIntro(void)
{
    u8 *state;
    u32 value;
    register struct PrimarySpriteData *spriteIp PIN("r12");
    u8 *work1;
    u32 oldTimer;
    u32 offset;
    u32 y;
    register struct PrimarySpriteData *sprite4 PIN("r4");
    register u32 reg6 PIN("r6");
    register u8 *work3 PIN("r12");
    register const s16 *table PIN("r5");
    u32 velocity;

    state = (u8 *)&gBossState;
    switch (*state) {
    case 3:
        value = (u32)&gCurrentSprite;
        spriteIp = (struct PrimarySpriteData *)value;
        work1 = &spriteIp->work1;
        if (*work1 > 11)
            break;
        {
            register u8 *work0 PIN("r2");

            work0 = &spriteIp->work0;
            value = *work0;
            offset = value + 1;
            *work0 = offset;
            value <<= 24;
            oldTimer = value >> 24;
        }
        value = 7;
        value &= oldTimer;
        if (value != 0)
            break;

        offset = oldTimer << 1;
        offset += oldTimer;
        {
            register struct PrimarySpriteData *sprite2 PIN("r2");
            u32 x;

            sprite2 = spriteIp;
            y = sprite2->yPosition;
            y += offset;
            y <<= 16;
            y >>= 16;
            x = sprite2->xPosition;
            x -= offset;
            x = (u16)x;
            offset = *work1;
            ASM_BARRIER(: : "r"(offset));
            y += 56;
            SpriteSpawnAsChild(PSPRITE_AB, offset, 0, y, x);
        }
        value = *work1;
        value++;
        *work1 = value;
        value = (u8)value;
        if (value == 12)
            *state = 4;
        break;
    case 5:
        {
            struct PrimarySpriteData *sprite;
            u32 temp;

            sprite = &gCurrentSprite;
            temp = 113;
            sprite->pose = temp;
            sprite = (struct PrimarySpriteData *)((u32)sprite + 39);
            temp = 120;
            *(u8 *)sprite = temp;
        }
        break;
    case 2:
    case 4:
        break;
    default:
        sprite4 = &gCurrentSprite;
        reg6 = 42;
        ASM_BARRIER(: "+r"(reg6));
        reg6 += (u32)sprite4;
        work3 = (u8 *)reg6;
        oldTimer = *(u8 *)reg6;
        table = sGoldenDivaIntroMotion;
        value = oldTimer << 1;
        value += (u32)table;
        velocity = *(const u16 *)value;
        reg6 = 0;
        offset = *(const s16 *)(value + reg6);
        value = 0x7FFF;
        if ((s32)offset == (s32)value) {
            velocity = *(const u16 *)table;
            oldTimer = 0;
        }
        value = oldTimer + 1;
        offset = (u32)work3;
        *(u8 *)offset = value;
        value = sprite4->yPosition;
        value += velocity;
        sprite4->yPosition = value;
        break;
    }
}


void BeginGoldenDivaFirstPhase(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;
    u32 zero;
    u32 data;
    u32 stride;
    register u32 offset PIN("r2");

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    value--;
    *timer = value;
    zero = (u8)value;
    if (zero != 0)
        return;

    value = 114;
    sprite->pose = value;
    value = 30;
    *timer = value;

    value = SpriteUtilFindSpriteSlot(PSPRITE_EE);
    data = (u32)gSpriteData;
    stride = sizeof(struct PrimarySpriteData);
    offset = value;
    offset *= stride;
    value = data + 4;
    value = offset + value;
    timer = (u8 *)sGoldenDivaBodyPhaseStartOam;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)value, (const struct AnimationFrame *)timer);
    offset += data;
    ((struct PrimarySpriteData *)offset)->currentAnimationFrame = zero;
    ((struct PrimarySpriteData *)offset)->animationTimer = zero;

    value = SpriteUtilFindSpriteSlot(PSPRITE_F1);
    timer = (u8 *)value;
    timer = (u8 *)((u32)timer * stride);
    timer += data;
    offset = *(u16 *)timer;
    value = 0xFFFB;
    value &= offset;
    *(u16 *)timer = value;
    m4aSongNumStart(SOUND_82);
}

void UpdateGoldenDivaFirstPhase(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    register u32 value PIN("r0");
    u32 statusMask;
    register u32 index PIN("r1");
    u32 data;
    u32 stride;
    struct PrimarySpriteData *sprite4;
    u8 *work3;
    u32 tableIndex;
    const s16 *table;
    u32 velocity;
    u32 zero;
    s32 signedVelocity;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    if (value != 0) {
        value--;
        *timer = value;
        value <<= 24;
        if (value == 0) {
            gBossState = 6;
            value = sprite->status;
            index = 0x80;
            index <<= 6;
            ASM_BARRIER(: "+r"(index));
            statusMask = index;
            value |= statusMask;
            sprite->status = value;

            value = SpriteUtilFindSpriteSlot(PSPRITE_ED);
            index = value;
            ASM_BARRIER(: "+r"(index));
            data = (u32)gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            index *= stride;
            index += data;
            value = *(u16 *)index;
            value |= statusMask;
            *(u16 *)index = value;

            value = SpriteUtilFindSpriteSlot(PSPRITE_EE);
            index = value;
            index *= stride;
            index += data;
            value = *(u16 *)index;
            value |= statusMask;
            *(u16 *)index = value;

            value = SpriteUtilFindSpriteSlot(PSPRITE_EF);
            index = value;
            ASM_BARRIER(: "+r"(index));
            value = index;
            value *= stride;
            value += data;
            index = *(u16 *)value;
            statusMask |= index;
            *(u16 *)value = statusMask;
            m4aSongNumStart(SOUND_B7);
        }
    } else {
        u16 *counter;

        counter = &gSpriteAiRadius;
        value = *counter;
        value++;
        *counter = value;
        value = (s16)value;
        if ((s32)value > 191) {
            sprite->pose = 120;
            *timer = 60;
            LoadGoldenDivaFanDamageGraphics(1);
            value = SpriteUtilFindSpriteSlot(PSPRITE_F1);
            index = value;
            {
                u32 spriteData;
                register u32 spriteStride PIN("r0");

                spriteData = (u32)gSpriteData;
                spriteStride = sizeof(struct PrimarySpriteData);
                index *= spriteStride;
                index += spriteData;
                spriteData = *(u16 *)index;
                spriteStride = 8;
                spriteStride |= spriteData;
                *(u16 *)index = spriteStride;
            }
        }
    }

    sprite4 = &gCurrentSprite;
    work3 = &sprite4->work3;
    tableIndex = *work3;
    table = sGoldenDivaIntroMotion;
    value = tableIndex << 1;
    value += (u32)table;
    velocity = *(const u16 *)value;
    zero = 0;
    signedVelocity = *(s16 *)(value + zero);
    value = 0x7FFF;
    if (signedVelocity == (s32)value) {
        velocity = *(const u16 *)table;
        tableIndex = 0;
    }
    value = tableIndex + 1;
    *work3 = value;
    value = sprite4->yPosition;
    value += velocity;
    sprite4->yPosition = value;
}

void BeginGoldenDivaMaskRecall(void)
{
    struct PrimarySpriteData *sprite2;
    u8 *timer;
    u32 value;
    struct PrimarySpriteData *sprite4;
    const s16 *table;
    u32 velocity;
    u32 index;
    u32 zero;
    register s32 signedVelocity PIN("r1");
    register u8 *work3 PIN("r12");

    sprite2 = &gCurrentSprite;
    timer = &sprite2->work0;
    value = *timer;
    if (value != 0) {
        value--;
        *timer = value;
    } else {
        sprite2->pose = 115;
        SpriteUtilStartBossTimer();
    }

    sprite4 = &gCurrentSprite;
    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += (u32)sprite4;
    work3 = (u8 *)value;
    index = *(u8 *)value;
    table = sGoldenDivaIntroMotion;
    value = index << 1;
    value += (u32)table;
    velocity = *(const u16 *)value;
    zero = 0;
    signedVelocity = *(const s16 *)(value + zero);
    value = 0x7FFF;
    if (signedVelocity == (s32)value) {
        velocity = *(const u16 *)table;
        index = 0;
    }
    value = index + 1;
    timer = work3;
    *timer = value;
    value = sprite4->yPosition;
    value += velocity;
    sprite4->yPosition = value;
}

void UpdateGoldenDivaMaskRecall(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;
    register u8 *work3 PIN("r12");
    u32 index;
    register const s16 *table PIN("r5");
    u32 velocity;
    u32 zero;
    register s32 signedVelocity PIN("r1");

    sprite = &gCurrentSprite;
    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += (u32)sprite;
    work3 = (u8 *)value;
    index = *(u8 *)value;
    table = sGoldenDivaIntroMotion;
    value = index << 1;
    value += (u32)table;
    velocity = *(const u16 *)value;
    zero = 0;
    signedVelocity = *(const s16 *)(value + zero);
    value = 0x7FFF;
    if (signedVelocity == (s32)value) {
        velocity = *(const u16 *)table;
        index = 0;
    }
    value = index + 1;
    {
        register u8 *destination PIN("r1");

        destination = work3;
        *destination = value;
    }
    value = sprite->yPosition;
    value += velocity;
    sprite->yPosition = value;

    value = gWarioPauseTimer;
    if (value == 0) {
        value = gTimerState;
        if (value == 1) {
            SpriteUtilUnsetAllSpritesHighPriority();
            sprite->pose = 15;
        }
    }
}

void BeginGoldenDivaAttackSelection(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    u32 y;
    u32 bgY;
    u8 *work1;

    sprite = &gCurrentSprite;
    y = sprite->yPosition;
    value = y >> 2;
    bgY = gBg1YPosition;
    bgY >>= 2;
    value -= bgY;
    value = (u16)value;
    if (value > 32) {
        value = y - 4;
        sprite->yPosition = value;
    } else {
        sprite->pose = 16;
        value = SpriteUtilFindSpriteSlot(PSPRITE_ED);
        work1 = &sprite->work1;
        *work1 = value;
        gBossState = 7;
    }
}

void UpdateGoldenDivaMaskDamage(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;
    u32 health;
    register u8 *work3 PIN("r12");
    u32 index;
    const s16 *table;
    u32 velocity;
    u32 zero;
    register s32 signedVelocity PIN("r1");

    sprite = &gCurrentSprite;
    health = gCuckooCondorHasCapturedWario;
    value = sprite->health;
    if (value != health) {
        sprite->health = health;
        UpdateBossHealthGauge();
        value = (u8)gCuckooCondorPendulumLength;
        if (value == 0)
            sprite->pose = 17;
    }

    AttachGoldenDivaBodyToChild(2, 0xC0);

    sprite = &gCurrentSprite;
    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += (u32)sprite;
    work3 = (u8 *)value;
    index = *(u8 *)value;
    table = sGoldenDivaIntroMotion;
    value = index << 1;
    value += (u32)table;
    velocity = *(const u16 *)value;
    zero = 0;
    signedVelocity = *(const s16 *)(value + zero);
    value = 0x7FFF;
    if (signedVelocity == (s32)value) {
        velocity = *(const u16 *)table;
        index = 0;
    }
    value = index + 1;
    {
        register u8 *destination PIN("r1");

        destination = work3;
        *destination = value;
    }
    value = sprite->yPosition;
    value += velocity;
    sprite->yPosition = value;
}

void BeginGoldenDivaFaceAnimation(void)
{
    u32 value;
    u32 stride;
    register u32 offset PIN("r2");
    register u32 data PIN("r3");

    value = SpriteUtilFindSpriteSlot(PSPRITE_EE);
    data = (u32)gSpriteData;
    stride = sizeof(struct PrimarySpriteData);
    offset = value;
    offset *= stride;
    value = data + 4;
    value = (offset) - (-(value));
    stride = (u32)sGoldenDivaBodyFaceStartOam;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)value, (const struct AnimationFrame *)stride);
    offset += data;
    value = 0;
    *(u8 *)(offset + 22) = value;
    *(u16 *)(offset + 20) = value;

    value = (u32)&gCurrentSprite;
    stride = 18;
    ((struct PrimarySpriteData *)value)->pose = stride;
    value += 39;
    stride = 60;
    *(u8 *)value = stride;
}

void UpdateGoldenDivaFaceAnimation(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    register u32 value PIN("r0");
    u32 temp;
    u32 y;
    u32 zero;
    u32 data;
    register u32 stride PIN("r8");
    u32 index;
    u32 zero6;
    u32 statusMask;

    value = (u32)&gCurrentSprite;
    ASM_BARRIER(: "+r"(value));
    temp = value;
    temp += 39;
    value = *(u8 *)temp;
    if (value != 0) {
        value--;
        *(u8 *)temp = value;
        value = (u8)value;
        if (value == 30)
            VoiceSetPlay(17);
    }

    sprite = &gCurrentSprite;
    y = sprite->yPosition;
    value = y >> 2;
    temp = gBg1YPosition;
    temp >>= 2;
    value -= temp;
    value = (u16)value;
    if (value > 32) {
        value = y - 4;
        sprite->yPosition = value;
    } else {
        value = 39;
        ASM_BARRIER(: "+r"(value));
        value += (u32)sprite;
        work0 = (u8 *)value;
        zero = *(u8 *)value;
        if (zero == 0) {
            value = SpriteUtilFindSpriteSlot(PSPRITE_EE);
            data = (u32)gSpriteData;
            temp = sizeof(struct PrimarySpriteData);
            stride = temp;
            index = stride;
            index *= value;
            value = index + data;
            ((struct PrimarySpriteData *)value)->currentAnimationFrame = zero;
            zero6 = 0;
            ((struct PrimarySpriteData *)value)->animationTimer = zero;
            value = data + 4;
            index += value;
            value = (u32)sGoldenDivaBodyFaceLoopOam;
            GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)index, (const struct AnimationFrame *)value);

            value = gCuckooCondorMoveRight;
            if (value == 0) {
                gBgAnimationTimer = zero6;
                gSpriteAiDynamicGraphicsTimer = zero6;
                UpdateGoldenDivaDynamicGraphics(sGoldenDivaFaceStartGraphicsSequence);
                sprite->warioCollision = 73;
                value = 90;
                temp = (u32)work0;
                *(u8 *)temp = value;
                value = (u32)sprite;
                value += 42;
                *(u8 *)value = zero6;
                sprite->pose = 21;
                temp = (u32)&gBossState;
                value = 12;
                *(u8 *)temp = value;

                temp = sprite->status;
                statusMask = 0xDFFF;
                value = statusMask;
                value &= temp;
                sprite->status = value;

                value = SpriteUtilFindSpriteSlot(PSPRITE_ED);
                index = stride;
                index *= value;
                index += data;
                y = *(u16 *)index;
                value = statusMask;
                value &= y;
                *(u16 *)index = value;

                value = SpriteUtilFindSpriteSlot(PSPRITE_EE);
                index = stride;
                index *= value;
                value = index;
                value += data;
                temp = *(u16 *)value;
                statusMask &= temp;
                *(u16 *)value = statusMask;
                m4aSongNumStart(SOUND_30);
            } else {
                sprite->pose = 19;
                temp = (u32)sprite;
                temp += 42;
                value = 48;
                *(u8 *)temp = value;

                temp = (u32)&gBossState;
                value = *(u8 *)temp;
                if (value == 7) {
                    value = 9;
                } else if (value == 9) {
                    value = 11;
                } else if (value == 11) {
                    value = 10;
                } else if (value == 10) {
                    value = 8;
                } else if (value == 8) {
                    value = 9;
                } else {
                    return;
                }
                *(u8 *)temp = value;
            }
        }
    }
}

void UpdateGoldenDivaPhaseTransition(void)
{
    u32 finished;
    struct PrimarySpriteData *sprite;
    u8 *work3;
    u32 value;
    u32 temp;
    u32 index;
    const s16 *table;
    u32 velocityA;
    register u32 velocityB PIN("r4");
    register s32 signedVelocity PIN("r1");
    u32 zero;

    finished = 0;
    sprite = &gCurrentSprite;
    temp = gCuckooCondorHasCapturedWario;
    value = sprite->health;
    if (value != temp) {
        sprite->health = temp;
        UpdateBossHealthGauge();
        sprite->pose = 17;
    } else {
        work3 = &sprite->work3;
        index = *work3;
        table = sGoldenDivaPhaseTransitionXMotion;
        value = index >> 1;
        value <<= 1;
        value += (u32)table;
        velocityA = *(const u16 *)value;
        zero = 0;
        signedVelocity = *(const s16 *)(value + zero);
        value = 0x7FFF;
        if (signedVelocity == (s32)value) {
            finished = 1;
            index = 0;
            velocityA = *(const u16 *)table;
        }

        temp = (u32)sGoldenDivaPhaseTransitionYMotion;
        value = index >> 1;
        value <<= 1;
        value += temp;
        velocityB = *(const u16 *)value;
        temp = index + 1;
        value = temp << 24;
        index = value >> 24;
        *work3 = temp;
        if (index == 48)
            m4aSongNumStart(SOUND_7B);

        value = sprite->yPosition;
        value += velocityB;
        sprite->yPosition = value;
        AttachGoldenDivaBodyToChild(velocityA, 0x80);

        velocityB = finished;
        if (velocityB != 0) {
            value = ChooseGoldenDivaPhaseTwoAttack();
            if (value != 0) {
                *work3 = 48;
            } else if (gCuckooCondorMoveRight <= 4) {
                temp = (u32)&gBossState;
                value = *(u8 *)temp;
                if (value == 9) {
                    value = 11;
                } else if (value == 11) {
                    value = 10;
                } else if (value == 10) {
                    value = 8;
                } else if (value == 8) {
                    value = 9;
                } else {
                    return;
                }
                *(u8 *)temp = value;
            }
        }
    }
}

void BeginGoldenDivaAttack(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    register u32 value PIN("r0");
    u32 temp;
    u32 y;
    u32 timerValue;
    u32 zero;

    UpdateGoldenDivaDynamicGraphics(sGoldenDivaFaceStartGraphicsSequence);
    sprite = &gCurrentSprite;
    y = sprite->yPosition;
    temp = y >> 2;
    value = gBg1YPosition;
    value >>= 2;
    temp -= value;
    temp <<= 16;
    temp >>= 16;
    if (temp > 10) {
        value = y - 2;
        sprite->yPosition = value;
    } else {
        timer = &sprite->work0;
        value = *timer;
        value--;
        *timer = value;
        timerValue = (u8)value;
        if (timerValue == 0) {
            value = SpriteUtilFindSpriteSlot(PSPRITE_EE);
            {
                u32 data;
                u32 stride;
                u32 offset;

                data = (u32)gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                stride *= value;
                offset = stride + data;
                ((struct PrimarySpriteData *)offset)->currentAnimationFrame = timerValue;
                zero = 0;
                ((struct PrimarySpriteData *)offset)->animationTimer = timerValue;
                data += 4;
                stride += data;
                offset = (u32)sGoldenDivaBodyAttackOam;
                GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)stride, (const struct AnimationFrame *)offset);
            }
            gBgAnimationTimer = zero;
            gSpriteAiDynamicGraphicsTimer = zero;
            UpdateGoldenDivaDynamicGraphics(sGoldenDivaLowHealthGraphicsSequence);
            sprite->pose = 22;
            *timer = zero;
            sprite->work2 = 1;
            value = (u32)sprite;
            value += 42;
            *(u8 *)value = zero;
        }
    }
}

void UpdateGoldenDivaAttack(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;
    register u32 temp PIN("r1");
    u32 index;
    const s16 *table;
    register u32 velocity PIN("r4");
    u32 zero;
    register s32 signedVelocity PIN("r1");

    if (gCurrentSprite.health == 2)
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
    else
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaLowHealthGraphicsSequence);

    sprite = &gCurrentSprite;
    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += (u32)sprite;
    {
        register u8 *work3 PIN("r12");

        work3 = (u8 *)value;
        index = *(u8 *)value;
        table = sGoldenDivaAttackMotion;
        value = index << 1;
        value += (u32)table;
        velocity = *(const u16 *)value;
        zero = 0;
        signedVelocity = *(const s16 *)(value + zero);
        value = 0x7FFF;
        if (signedVelocity == (s32)value) {
            velocity = *(const u16 *)table;
            index = 0;
        }
        value = index + 1;
        temp = (u32)work3;
        *(u8 *)temp = value;
    }
    value = sprite->yPosition;
    value += velocity;
    sprite->yPosition = value;

    temp = (u32)&sprite->work2;
    value = *(u8 *)temp;
    if (value <= 31) {
        value++;
        *(u8 *)temp = value;
    }
    value = *(u8 *)temp;
    velocity = value >> 2;

    index = (u32)gSpriteData;
    value = (u32)&sprite->work1;
    temp = *(u8 *)value;
    value = sizeof(struct PrimarySpriteData);
    value *= temp;
    value += index;
    temp = *(u16 *)value;
    value = 0x40;
    value &= temp;
    if (value != 0) {
        temp = sprite->xPosition;
        temp = velocity + temp;
        sprite->xPosition = temp;
        value = sprite->yPosition;
        temp += 28;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
    } else {
        temp = sprite->xPosition;
        temp -= velocity;
        sprite->xPosition = temp;
        value = sprite->yPosition;
        temp -= 28;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
    }

    temp = gSpriteCollisionTileType;
    value = 15;
    value &= temp;
    sprite = &gCurrentSprite;
    if (value != 0) {
        index = (u32)gSpriteData;
        value = (u32)&sprite->work1;
        temp = *(u8 *)value;
        value = sizeof(struct PrimarySpriteData);
        temp *= value;
        temp += index;
        value = *(u16 *)temp;
        index = 0x40;
        value ^= index;
        *(u16 *)temp = value;
        sprite->work2 = 8;
    }

    temp = (u32)&sprite->work0;
    value = *(u8 *)temp;
    velocity = value;
    if (velocity != 0) {
        value--;
        *(u8 *)temp = value;
    } else {
        temp = gWarioData.xPosition;
        ASM_BARRIER(: "+r"(temp));
        index = sprite->xPosition;
        value = temp;
        value += 32;
        if ((s32)value > (s32)index) {
            value -= 64;
            if ((s32)value < (s32)index) {
                sprite->pose = 28;
                value = (u32)sprite;
                value += 42;
                *(u8 *)value = velocity;
                {
                    volatile u8 *gatePtr;
                    u32 gateReg;
                    u32 gate;

                    gatePtr = &gInitialHealth;
                    /* Compiler boundary: pure C inserts an extra register copy after this byte load. */
                    gateReg = *(u8 *)(u8 *)(gatePtr);
                    gate = gateReg;
                    if (gate == 0) {
                        gBgAnimationTimer = gate;
                        gSpriteAiDynamicGraphicsTimer = gate;
                        value = sprite->health;
                        if (value == 2)
                            UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
                        else
                            UpdateGoldenDivaDynamicGraphics(sGoldenDivaHitGraphicsSequence);
                        m4aSongNumStart(SOUND_BD);
                    }
                }
            }
        }
    }
}



void BeginGoldenDivaHitReaction(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    register u32 temp PIN("r1");
    u32 temp2;
    u32 temp3;
    register u32 result PIN("r5");
    register u32 zero PIN("r6");

    if (gCurrentSprite.health == 2)
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
    else
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaHitGraphicsSequence);

    sprite = &gCurrentSprite;
    temp = 0x90;
    temp <<= 1;
    ASM_BARRIER(: "+r"(temp));
    value = temp;
    ASM_BARRIER(: "+r"(value));
    temp2 = sprite->yPosition;
    value += temp2;
    value = (u16)value;
    temp = sprite->xPosition;
    result = func_8023A60(value, temp);
    value = gSpriteCollisionResult;
    if (value != 0) {
        value = result << 16;
        temp = sprite->xPosition;
        temp3 = (u32)&gSpriteTileInteractionMode;
        temp2 = 4;
        *(u8 *)temp3 = temp2;
        zero = 0x80;
        ASM_BARRIER(: "+r"(zero));
        zero <<= 11;
        value += zero;
        value >>= 16;
        GetSpriteBlockCollisionAtPosition(value, temp);
        temp = -0x120;
        value = result + temp;
        sprite->yPosition = value;
        value = 24;
        sprite->pose = value;
        value = sprite->health;
        if (value == 2) {
            temp = (u32)sprite;
            temp += 39;
            value = 10;
        } else if (value == 3) {
            temp = (u32)sprite;
            temp += 39;
            value = 20;
        } else {
            temp = (u32)sprite;
            temp += 39;
            value = 30;
        }
        *(u8 *)temp = value;
        value = 48;
        temp = 129;
        ScreenShakeRequestY(value, temp);
        value = 62;
        m4aSongNumStart(value);
    } else {
        register u8 *work3 PIN("r12");

        temp2 = 42;
        ASM_BARRIER(: "+r"(temp2));
        temp2 += (u32)sprite;
        work3 = (u8 *)temp2;
        temp2 = *(u8 *)temp2;
        result = (u32)sGoldenDivaHitMotion;
        value = temp2 << 1;
        value += result;
        temp3 = *(u16 *)value;
        zero = 0;
        temp = *(s16 *)(value + zero);
        value = 0x7FFF;
        if (temp == value) {
            temp = temp2 - 1;
            temp <<= 1;
            temp += result;
            value = sprite->yPosition;
            temp = *(u16 *)temp;
            value += temp;
        } else {
            value = temp2 + 1;
            temp = (u32)work3;
            *(u8 *)temp = value;
            value = sprite->yPosition;
            value += temp3;
        }
        sprite->yPosition = value;
    }
}



void UpdateGoldenDivaHitReaction(void)
{
    if (gCurrentSprite.health == 2)
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
    else
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaHitGraphicsSequence);

    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 26;
        if (gInitialHealth == 0) {
            gBgAnimationTimer = 0;
            gSpriteAiDynamicGraphicsTimer = 0;
            if (gCurrentSprite.health == 2)
                UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
            else
                UpdateGoldenDivaDynamicGraphics(sGoldenDivaRecoveryGraphicsSequence);
        }
    }
}

void BeginGoldenDivaRecovery(void)
{
    gCurrentSprite.pose = 22;
    gCurrentSprite.work0 = 30;
    gCurrentSprite.work2 = 1;
    gCurrentSprite.work3 = 0;
    if (gInitialHealth == 0) {
        gBgAnimationTimer = 0;
        gSpriteAiDynamicGraphicsTimer = 0;
        if (gCurrentSprite.health == 2)
            UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
        else
            UpdateGoldenDivaDynamicGraphics(sGoldenDivaLowHealthGraphicsSequence);
    }
}

void UpdateGoldenDivaRecovery(void)
{
    if (gCurrentSprite.health == 2)
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
    else
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaRecoveryGraphicsSequence);

    if ((u16)((gCurrentSprite.yPosition >> 2) - (gBg1YPosition >> 2)) > 10)
        gCurrentSprite.yPosition -= 4;
    else
        BeginGoldenDivaRecovery();
}


void ApplyGoldenDivaDamage(void)
{
    u32 slot;

    gCurrentSprite.work0 = 32;
    gBgAnimationTimer = 0;
    gSpriteAiDynamicGraphicsTimer = 0;
    gCuckooCondorHasCapturedWario--;
    if (gCurrentSprite.health != gCuckooCondorHasCapturedWario) {
        gCurrentSprite.health = gCuckooCondorHasCapturedWario;
        UpdateBossHealthGauge();
    }

    slot = gCurrentSprite.work1;
    GP_SET(gSpriteData[slot].pOamData, sGoldenDivaBodyDamagedOam);
    gSpriteData[slot].currentAnimationFrame = 0;
    gSpriteData[slot].animationTimer = 0;

    slot = SpriteUtilFindSpriteSlot(PSPRITE_EE);
    gSpriteData[slot].status |= SPRITE_STATUS_HIDDEN;
    VoiceSetPlay(1);
}

void MoveGoldenDivaBodyRight(void)
{
    gCurrentSprite.pose = 32;
    gSpriteData[gCurrentSprite.work1].status |= SPRITE_STATUS_FACING_RIGHT;
    ApplyGoldenDivaDamage();
}

void MoveGoldenDivaBodyLeft(void)
{
    gCurrentSprite.pose = 34;
    gSpriteData[gCurrentSprite.work1].status &= ~SPRITE_STATUS_FACING_RIGHT;
    ApplyGoldenDivaDamage();
}





void UpdateGoldenDivaBodyMovement(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    u32 temp;
    u32 temp2;
    u32 timer;
    u32 sprites;
    u32 work0;
    u32 slot;
    register u32 stride PIN("r8");

    sprite = &gCurrentSprite;
    value = (u32)sprite;
    value += 40;
    slot = *(u8 *)value;
    value = 1;
    sprite->disableWarioCollisionTimer = value;
    UpdateGoldenDivaDynamicGraphics(sGoldenDivaMovementGraphicsSequence);

    temp2 = sprite->yPosition;
    temp = temp2 >> 2;
    value = gBg1YPosition;
    value >>= 2;
    temp -= value;
    temp <<= 16;
    temp >>= 16;
    if (temp > 32) {
        value = temp2 - 4;
        sprite->yPosition = value;
    }

    work0 = (u32)sprite;
    work0 += 39;
    value = *(u8 *)work0;
    value >>= 1;
    temp = 64;
    AttachGoldenDivaBodyToChild(value, temp);
    value = *(u8 *)work0;
    temp = value - 1;
    *(u8 *)work0 = temp;
    value = temp << 24;
    timer = value >> 24;
    if (timer != 0) {
        value = timer;
        ASM_BARRIER(: "+r"(value));
        if (value == 26) {
            value = sprite->health;
            if (value == 1) {
                value = 121;
                m4aSongNumStart(value);
            } else {
                value = 17;
                VoiceSetPlay(value);
            }
        }

        sprite = &gCurrentSprite;
        value = (u32)sprite;
        value += 39;
        temp = *(u8 *)value;
        value = 7;
        value &= temp;
        if (value != 0)
            return;

        value = 8;
        value &= temp;
        timer = (u8)value;
        if (timer != 0) {
            value = 4;
            sprite->palette = value;
            temp = (u32)gSpriteData;
            value = sizeof(struct PrimarySpriteData);
            temp2 = slot;
            temp2 *= value;
            timer = temp2 + temp;
            value = 3;
            ((struct PrimarySpriteData *)timer)->palette = value;
            value = (u32)sGoldenDivaBodyMoveOam;
            GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
            temp += 4;
            temp2 += temp;
            value = (u32)sGoldenDivaBodyDamagedAltOam;
            *(u32 *)temp2 = value;
        } else {
            sprite->palette = timer;
            temp = (u32)gSpriteData;
            value = sizeof(struct PrimarySpriteData);
            temp2 = slot;
            temp2 *= value;
            value = temp2 + temp;
            ((struct PrimarySpriteData *)value)->palette = timer;
            value = (u32)sGoldenDivaBodyIdleOam;
            GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
            temp += 4;
            temp2 += temp;
            value = (u32)sGoldenDivaBodyDamagedOam;
            *(u32 *)temp2 = value;
        }
        return;
    }

    sprite->palette = timer;
    sprites = (u32)gSpriteData;
    value = sizeof(struct PrimarySpriteData);
    stride = value;
    temp = stride;
    temp *= slot;
    temp2 = temp + sprites;
    ((struct PrimarySpriteData *)temp2)->palette = timer;
    value = (u32)sGoldenDivaBodyIdleOam;
    GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
    value = sprites + 4;
    temp += value;
    value = (u32)sGoldenDivaBodyDamagedOam;
    *(u32 *)temp = value;
    value = sprite->health;
    if (value == 1) {
        value = 50;
        sprite->pose = value;
        value = 60;
        *(u8 *)work0 = value;
        temp = (u32)&gBossState;
        value = 13;
        *(u8 *)temp = value;
        temp = (u32)gSwitchStates;
        value = 2;
        *(u8 *)(temp + 2) = value;
        value = SOUND_2B1;
        m4aSongNumStart(value);
    } else {
        value = (u32)sGoldenDivaRobeOam;
        *(u32 *)temp = value;
        *(u8 *)(temp2 + 22) = timer;
        work0 = 0;
        *(u16 *)(temp2 + 20) = timer;
        value = PSPRITE_EE;
        /* Compiler boundary: the u8 prototype would truncate the live u32 ID in r0 before the direct call. */
        value = ((u32 (*)(u32))SpriteUtilFindSpriteSlot)(value);
        temp = stride;
        temp *= value;
        temp += sprites;
        temp2 = *(u16 *)temp;
        value = 0xFFFB;
        value &= temp2;
        *(u16 *)temp = value;
        value = 26;
        sprite->pose = value;
        value = gInitialHealth;
        if (value == 0) {
            gBgAnimationTimer = work0;
            gSpriteAiDynamicGraphicsTimer = work0;
            value = sprite->health;
            if (value == 2)
                UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
            else
                UpdateGoldenDivaDynamicGraphics(sGoldenDivaRecoveryGraphicsSequence);
        }
    }
}

void BeginGoldenDivaTurnRight(void)
{
    gCurrentSprite.pose = 36;
    gCurrentSprite.work0 = 60;
    gBgAnimationTimer = 0;
    gSpriteAiDynamicGraphicsTimer = 0;
    gSpriteData[gCurrentSprite.work1].status |= SPRITE_STATUS_FACING_RIGHT;
    m4aSongNumStart(SOUND_BD);
}

void BeginGoldenDivaTurnLeft(void)
{
    gCurrentSprite.pose = 36;
    gCurrentSprite.work0 = 60;
    gBgAnimationTimer = 0;
    gSpriteAiDynamicGraphicsTimer = 0;
    gSpriteData[gCurrentSprite.work1].status &= ~SPRITE_STATUS_FACING_RIGHT;
    m4aSongNumStart(SOUND_BD);
}

void UpdateGoldenDivaTurn(void)
{
    if (gCurrentSprite.health == 2)
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaAttackGraphicsSequence);
    else
        UpdateGoldenDivaDynamicGraphics(sGoldenDivaHitGraphicsSequence);

    AttachGoldenDivaBodyToChild(6, 64);
    if (gCurrentSprite.work0 != 0)
        gCurrentSprite.work0--;

    if ((u16)((gCurrentSprite.yPosition >> 2) - (gBg1YPosition >> 2)) > 10)
        gCurrentSprite.yPosition -= 4;
    else if (gCurrentSprite.work0 == 0)
        BeginGoldenDivaRecovery();
}


void UpdateGoldenDivaBodyAttackPose(void)
{
    u8 *work0;
    register u32 stride PIN("r5");
    register u32 sprites PIN("r6");
    register u32 zero PIN("r8");
    u32 address;
    u32 value;

    work0 = (u8 *)&gCurrentSprite;
    work0[31] = 1;
    UpdateGoldenDivaDynamicGraphics(sGoldenDivaMovementGraphicsSequence);
    work0 += 39;
    (*work0)--;

    switch (*work0) {
    case 57:
        SpriteSpawnAsChild(PSPRITE_0B, 0, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
        m4aSongNumStart(SOUND_B5);
        break;
    case 53:
        SpriteSpawnAsChild(PSPRITE_0B, 0, 0, gCurrentSprite.yPosition + 128, gCurrentSprite.xPosition - 64);
        SpriteSpawnAsChild(PSPRITE_0B, 0, 0, gCurrentSprite.yPosition + 128, gCurrentSprite.xPosition + 64);
        break;
    case 50:
        SpriteSpawnAsChild(PSPRITE_0B, 0, 0, gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition);
        break;
    case 40:
        SpriteSpawnAsChild(PSPRITE_0A, 0, 0, gCurrentSprite.yPosition + 128, gCurrentSprite.xPosition);
        break;
    case 30:
        SpriteSpawnAsChild(PSPRITE_0A, 0, 0, gCurrentSprite.yPosition + 192, gCurrentSprite.xPosition);
        break;
    case 24:
        SpriteSpawnAsChild(PSPRITE_0A, 0, 0, gCurrentSprite.yPosition + 256, gCurrentSprite.xPosition + 64);
        break;
    case 20:
        SpriteSpawnAsChild(PSPRITE_0A, 0, 0, gCurrentSprite.yPosition + 256, gCurrentSprite.xPosition - 92);
        break;
    case 0:
        work0 = (u8 *)&gCurrentSprite;
        zero = 0;
        work0[28] = 109;
        value = *(u16 *)work0;
        address = SPRITE_STATUS_HIDDEN;
        address |= value;
        *(u16 *)work0 = address;
        sprites = (u32)gSpriteData;
        address = (u32)work0;
        address += 40;
        address = *(u8 *)address;
        stride = sizeof(struct PrimarySpriteData);
        address *= stride;
        address += sprites;
        value = zero;
        *(u16 *)address = value;
        address = SpriteUtilFindSpriteSlot(PSPRITE_EE);
        address *= stride;
        address += sprites;
        value = zero;
        *(u16 *)address = value;
        SpriteSpawnAsChild(PSPRITE_F0, 0, 0, ((struct PrimarySpriteData *)work0)->yPosition + 56, ((struct PrimarySpriteData *)work0)->xPosition);
        SpriteSpawnAsChild(PSPRITE_F0, 1, 0, ((struct PrimarySpriteData *)work0)->yPosition + 56, ((struct PrimarySpriteData *)work0)->xPosition);
        SpriteSpawnAsChild(PSPRITE_F0, 2, 0, ((struct PrimarySpriteData *)work0)->yPosition + 56, ((struct PrimarySpriteData *)work0)->xPosition);
        SpriteSpawnAsChild(PSPRITE_F0, 3, 0, ((struct PrimarySpriteData *)work0)->yPosition + 56, ((struct PrimarySpriteData *)work0)->xPosition);
        SpriteSpawnAsChild(PSPRITE_F7, 0, 0, ((struct PrimarySpriteData *)work0)->yPosition + 56, ((struct PrimarySpriteData *)work0)->xPosition);
        break;
    }
}

void UpdateGoldenDivaOrbitPhase(void)
{
    struct PrimarySpriteData *sprite;
    u32 complete;
    u16 timer;
    u16 yPosition;
    u16 yDelta;
    u16 xDelta;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    if (sprite->health != gCuckooCondorHasCapturedWario) {
        sprite->health = gCuckooCondorHasCapturedWario;
        UpdateBossHealthGauge();
        gTimerState = 11;
        SpriteUtilSetWarioBossVictoryPose();
    }

    timer = gSpriteAiRadius;
    if ((s16)gSpriteAiRadius <= 159) {
        gSpriteAiRadius = timer + 1;
        goto checkHealth;
    }

    complete = 0;
    yPosition = sprite->yPosition;
    yDelta = (u16)((yPosition >> 2) - (gBg1YPosition >> 2));
    xDelta = (u16)((sprite->xPosition >> 2) - (gBg1XPosition >> 2));
    if (yDelta > 32)
        sprite->yPosition = yPosition - 2;
    else if (yDelta <= 29)
        sprite->yPosition = yPosition + 2;
    else
        complete = 1;

    if (xDelta > 122)
        gCurrentSprite.xPosition -= 2;
    else if (xDelta <= 119)
        gCurrentSprite.xPosition += 2;
    else
        complete++;

    if (complete != 2)
        return;

checkHealth:
    if (gCurrentSprite.health == 0) {
        gCurrentSprite.pose = 122;
        gCurrentSprite.work0 = 80;
    }
}

void UpdateGoldenDivaDefeatSequence(void)
{
    register u32 value PIN("r0");
    u32 temp;
    struct PrimarySpriteData *sprite;
    u32 state;
    u8 *statePointer;
    register u8 *work0Pointer PIN("r5");

    {
        struct PrimarySpriteData *initialSprite;
        u8 *initialState;

        initialSprite = &gCurrentSprite;
        value = 1;
        initialSprite->disableWarioCollisionTimer = value;
        initialState = &gBossSequenceState;
        state = *initialState;
        sprite = initialSprite;
        statePointer = initialState;
    }
    switch (state) {
        case 0:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gCollectedNEJewelPiece;
            if (*statePointer != 2)
                goto waitOne;
            value = 0;
            goto releaseTreasure;
        case 1:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gCollectedNWJewelPiece;
            if (*statePointer != 2)
                goto waitOne;
            value = 3;
            goto releaseTreasure;
        case 2:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure7;
            if (*statePointer != 2)
                goto waitOne;
            value = 6;
            goto releaseTreasure;
        case 3:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure10;
            if (*statePointer != 2)
                goto waitOne;
            value = 9;
            goto releaseTreasure;
        case 4:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gCollectedSEJewelPiece;
            if (*statePointer != 2)
                goto waitOne;
            value = 1;
            goto releaseTreasure;
        case 5:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure5;
            if (*statePointer != 2)
                goto waitOne;
            value = 4;
            goto releaseTreasure;
        case 6:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure8;
            if (*statePointer != 2)
                goto waitOne;
            value = 7;
            goto releaseTreasure;
        case 7:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure11;
            if (*statePointer != 2)
                goto waitOne;
            value = 10;
            goto releaseTreasure;
        case 8:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gCollectedSWJewelPiece;
            if (*statePointer != 2)
                goto waitOne;
            value = 2;
            goto releaseTreasure;
        case 9:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure6;
            if (*statePointer != 2)
                goto waitOne;
            value = 5;
            goto releaseTreasure;
        case 10:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure9;
            if (*statePointer != 2)
                goto waitOne;
            value = 8;
            goto releaseTreasure;
        case 11:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value--;
            *work0Pointer = value;
            value <<= 24;
            if (value != 0)
                break;
            (*statePointer)++;
            statePointer = &gHasBossTreasure12;
            if (*statePointer != 2)
                goto waitOne;
            value = 11;
releaseTreasure:
            StartTileEffect(value);
            value = 1;
            *statePointer = value;
            value = 80;
            *work0Pointer = value;
            break;
waitOne:
            value = 1;
            *work0Pointer = value;
            break;
        case 12: {
            struct WarioData *wario;
            temp = (u32)&sprite->work0;
            value = *(u8 *)temp;
            if (value != 0) {
                value--;
                *(u8 *)temp = value;
                break;
            }
            wario = &gWarioData;
            value = wario->pose;
            if (value != 65 || wario->unk_1F <= 21)
                break;
            (*statePointer)++;
            value = 90;
            *(u8 *)temp = value;
            GF(sWarioPoseRequestFuncTable[wario->reaction])(66);
            break;
        }
        case 13:
            state = (u32)&sprite->work0;
            value = *(u8 *)state;
            value--;
            *(u8 *)state = value;
            temp = (u8)value;
            if (temp != 0)
                break;
            (*statePointer)++;
            value = 1;
            *(u8 *)state = value;
            value = (u32)&sprite->work2;
            *(u8 *)value = temp;
            break;
        case 14: {
            u32 slot;
            u8 *work;
            u32 pointer;

            work = &sprite->work0;
            value = *work;
            value--;
            *work = value;
            value <<= 24;
            if (value != 0)
                break;
            pointer = (u32)sGoldenDivaDefeatSpawnSchedule;
            value = (u32)sprite;
            value += 41;
            value = *(u8 *)value;
            value += pointer;
            value = *(u8 *)value;
            value = SpriteUtilFindBossTreasureChest(value);
            slot = value;
            if (slot == 0xFF) {
                value = 1;
                *work = value;
            } else {
                value = 60;
                *work = value;
                pointer = (u32)gSpriteData;
                value = 44;
                value *= slot;
                value += pointer;
                pointer = 110;
                *(u8 *)(value + 28) = pointer;
            }
            value = (u32)&gCurrentSprite;
            ASM_BARRIER(: "+r"(value));
            pointer = value;
            pointer += 41;
            value = *(u8 *)pointer;
            if (value > 10) {
                pointer = (u32)&gBossSequenceState;
                value = *(u8 *)pointer;
                value++;
                goto storeCase14;
            }
            value++;
            ASM_BARRIER(: "+r"(value), "+r"(pointer));
storeCase14:
            *(u8 *)pointer = value;
            break;
        }
        case 15: {
            u32 y;
            u8 *work;

            work = &sprite->work0;
            value = *work;
            if (value != 0) {
                value--;
                *work = value;
                break;
            }
            y = sprite->yPosition;
            value = y >> 2;
            temp = gBg1YPosition >> 2;
            value -= temp;
            value = (u16)value;
            if (value <= 99) {
                value = y + 2;
                sprite->yPosition = value;
                break;
            }
            WarioRequestPose(67);
            value = 160;
            *work = value;
            temp = (u32)&gBossSequenceState;
            value = *(u8 *)temp;
            value++;
            *(u8 *)temp = value;
            break;
        }
        case 16: {
            volatile struct GoldenDivaDmaRegs *dma;
            u8 *work;
            register u32 mask PIN("r5");

            work = &sprite->work0;
            value = *work;
            temp = value - 1;
            *work = temp;
            mask = 0xFF;
            value = temp << 24;
            if (value != 0) {
                value >>= 24;
                if (value == 32)
                    ScreenShakeRequestY(32, 129);
                break;
            }
            dma = (volatile struct GoldenDivaDmaRegs *)0x040000D4;
            dma->src = (u32)sGoldenDivaTreasureReleaseOrder;
            dma->dst = 0x06017800;
            dma->control = 0x80000400;
            (void)dma->control;
            dma->src = (u32)sGoldenDivaDefeatPalette;
            dma->dst = 0x05000380;
            dma->control = 0x80000010;
            (void)dma->control;
            WarioRequestPose(68);
            value = *work;
            value |= mask;
            *work = value;
            temp = (u32)&gBossSequenceState;
            value = *(u8 *)temp;
            value++;
            *(u8 *)temp = value;
            ScreenShakeRequestY(32, 129);
            m4aSongNumStart(800);
            m4aSongNumStart(423);
            break;
        }
        case 17:
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            temp = value - 1;
            *work0Pointer = temp;
            value = temp << 24;
            if (value == 0)
                goto finishPhase17;
            value = 31;
            temp &= value;
            if (temp == 0)
                ScreenShakeRequestY(32, 129);
            value = *work0Pointer;
            value -= 160;
            switch (value) {
                case 75:
                    SpawnSecondarySprite(880, 360, 87);
                    break;
                case 50:
                    SpawnSecondarySprite(820, 720, 89);
                    break;
                case 47:
                    SpawnSecondarySprite(840, 240, 83);
                    break;
                case 40:
                    SpawnSecondarySprite(896, 620, 85);
                    break;
                case 35:
                    SpawnSecondarySprite(860, 540, 88);
                    break;
                case 22:
                    SpawnSecondarySprite(0, 120, 82);
                    break;
                case 20:
                    SpawnSecondarySprite(0, 440, 84);
                    break;
                case 12:
                    SpawnSecondarySprite(0, 840, 86);
                    break;
                case 0:
                    SpriteSpawnAsChild(128, 0, 7, gCurrentSprite.yPosition - 450,
                        gCurrentSprite.xPosition - 360);
                    break;
            }
            break;
finishPhase17:
            (*statePointer)++;
            LoadBossSpriteGraphics(8, 2, 14);
            StartGoldenDivaPaletteFade(8, 0, 16);
            break;
        case 18: {
            register u8 *work PIN("r4");
            work = &sprite->work0;
            temp = *work;
            value = 31;
            value &= temp;
            if (value == 0)
                ScreenShakeRequestY(32, 129);
            value = *work;
            value--;
            *work = value;
            if (gGoldenDivaRoomTimer[0] != 0 || *gBossDefeatTimer != 0)
                break;
            temp = (u32)&gBossSequenceState;
            value = *(u8 *)temp;
            value++;
            *(u8 *)temp = value;
            gSubGameMode = 6;
            gSpriteAiDropTimer = 0;
            gStageExitType = 5;
            break;
        }
    }
}

void InitGoldenDivaRobe(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 constant PIN("r2");
    register u32 zero8 PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    temp = *(u16 *)value;
    constant = 0x8408;
    ASM_BARRIER(: "+r"(constant));
    value = constant;
    ASM_BARRIER(: "+r"(value));
    zero8 = 0;
    zero16 = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = sprite;
    *(u16 *)temp = value;
    *(u8 *)(temp + 30) = zero8;
    temp += 32;
    value = 64;
    *(u8 *)temp = value;
    temp++;
    *(u8 *)temp = value;
    temp++;
    *(u8 *)temp = value;
    value = sprite;
    value += 35;
    constant = 96;
    *(u8 *)value = constant;
    temp += 2;
    value = 104;
    *(u8 *)temp = value;
    value = sprite;
    value += 37;
    *(u8 *)value = constant;
    temp += 2;
    ASM_BARRIER(: "+r"(temp));
    value = 92;
    *(u8 *)temp = value;
    value = (u32)sGoldenDivaRobeOam;
    constant = sprite;
    *(u32 *)(constant + 4) = value;
    ((struct PrimarySpriteData *)constant)->currentAnimationFrame = zero8;
    ((struct PrimarySpriteData *)constant)->animationTimer = zero16;
    value = 16;
    ((struct PrimarySpriteData *)constant)->pose = value;
    SpriteUtilFindSpriteSlotWork3(PSPRITE_EC);
}

void InitGoldenDivaLowerBody(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 constant PIN("r2");
    register u32 zero16 PIN("r3");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    temp = *(u16 *)value;
    constant = 0x8408;
    ASM_BARRIER(: "+r"(constant));
    value = constant;
    constant = 0;
    zero16 = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = sprite;
    *(u16 *)temp = value;
    *(u8 *)(temp + 30) = constant;
    temp += 32;
    value = 80;
    *(u8 *)temp = value;
    temp++;
    value = 64;
    *(u8 *)temp = value;
    temp++;
    value = 96;
    *(volatile u8 *)temp = value;
    value = sprite;
    value += 35;
    temp = 4;
    *(u8 *)value = temp;
    value++;
    *(u8 *)value = temp;
    value++;
    *(u8 *)value = temp;
    value++;
    *(volatile u8 *)value = temp;
    value = (u32)sGoldenDivaLowerBodyOam;
    temp = sprite;
    *(u32 *)(temp + 4) = value;
    ((struct PrimarySpriteData *)temp)->currentAnimationFrame = constant;
    ((struct PrimarySpriteData *)temp)->animationTimer = zero16;
    value = 1;
    ((struct PrimarySpriteData *)temp)->pose = value;
    SpriteUtilFindSpriteSlotWork3(PSPRITE_EC);
}

void UpdateGoldenDivaLowerBody(void)
{
    SpriteUtilFindSpriteSlotWork3(PSPRITE_EC);
    gCurrentSprite.pose = 16;
}

void InitGoldenDivaFaceHitbox(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 constant PIN("r2");
    register u32 zero8 PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    temp = *(u16 *)value;
    constant = 0x8428;
    ASM_BARRIER(: "+r"(constant));
    value = constant;
    ASM_BARRIER(: "+r"(value));
    zero8 = 0;
    zero16 = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = sprite;
    *(u16 *)temp = value;
    *(u8 *)(temp + 30) = zero8;
    value = sprite;
    value += 32;
    constant = 16;
    *(u8 *)value = constant;
    value++;
    *(u8 *)value = constant;
    value++;
    *(volatile u8 *)value = constant;
    temp += 35;
    value = 48;
    *(u8 *)temp = value;
    temp++;
    value = 56;
    *(u8 *)temp = value;
    temp++;
    value = 40;
    *(u8 *)temp = value;
    temp++;
    value = 36;
    *(volatile u8 *)temp = value;
    value = (u32)sGoldenDivaMaskFaceOam;
    temp = sprite;
    *(u32 *)(temp + 4) = value;
    ((struct PrimarySpriteData *)temp)->currentAnimationFrame = zero8;
    ((struct PrimarySpriteData *)temp)->animationTimer = zero16;
    ((struct PrimarySpriteData *)temp)->pose = constant;
}

void UpdateGoldenDivaFaceHitbox(void)
{
    if (gCurrentSprite.status & 0x400) {
        if (*(u8 *)&gCuckooCondorPendulumLength == 0) {
            gCurrentSprite.status &= 0xDBFF;
            gCurrentSprite.warioCollision = 72;
        }
    } else {
        if (gCurrentSprite.status & 0x200) {
            gCurrentSprite.work0--;
            if (gCurrentSprite.work0 == 0)
                gCurrentSprite.status &= 0xFDFF;
        }

        if (gCuckooCondorMoveRight == 0) {
            gCurrentSprite.status |= 0x400;
            gCurrentSprite.warioCollision = 0;
            gCurrentSprite.pose = 18;
            gCurrentSprite.work0 = 40;
        }
    }
}

void HideGoldenDivaFaceHitbox(void)
{
    gCurrentSprite.work0 = 16;
    gCurrentSprite.pose = 16;
}

void FollowGoldenDivaFaceHitbox(void)
{
    struct PrimarySpriteData *sprite;
    register u32 timer PIN("r5");
    u32 value;
    u32 temp;
    u8 *work0;

    sprite = &gCurrentSprite;
    value = 1;
    sprite->disableWarioCollisionTimer = value;
    work0 = (u8 *)sprite;
    work0 += 39;
    value = *work0;
    temp = value - 1;
    *work0 = temp;
    value = temp << 24;
    timer = value >> 24;
    if (timer != 0) {
        value = timer;
        if (value == 15) {
            value = sprite->yPosition;
            value -= 32;
            temp = sprite->xPosition;
            SpawnSecondarySprite(value, temp, 6);
            m4aSongNumStart(SOUND_31);
        } else if (value == 10) {
            value = sprite->yPosition;
            value += 32;
            temp = sprite->xPosition;
            temp -= 32;
            SpawnSecondarySprite(value, temp, 6);
        } else if (value == 5) {
            value = sprite->yPosition;
            value += 32;
            temp = sprite->xPosition;
            temp += 32;
            SpawnSecondarySprite(value, temp, 6);
        }
    } else {
        SpriteSpawnAsChild(PSPRITE_0A, 0, 0, sprite->yPosition + 72, sprite->xPosition);
        sprite->status = timer;
    }
}

void InitGoldenDivaFan(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 constant PIN("r2");
    register u32 zero8 PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    temp = *(u16 *)value;
    constant = 0x84;
    constant <<= 8;
    ASM_BARRIER(: "+r"(constant));
    value = constant;
    ASM_BARRIER(: "+r"(value));
    zero8 = 0;
    zero16 = 0;
    value |= temp;
    temp = sprite;
    *(u16 *)temp = value;
    *(u8 *)(temp + 30) = zero8;
    value = sprite;
    value += 32;
    temp = 24;
    *(u8 *)value = temp;
    value++;
    *(u8 *)value = temp;
    value++;
    constant = 32;
    *(volatile u8 *)value = constant;
    temp = sprite;
    temp += 35;
    value = 48;
    *(u8 *)temp = value;
    temp++;
    value = 8;
    *(u8 *)temp = value;
    temp++;
    value = 64;
    *(volatile u8 *)temp = value;
    value = sprite;
    value += 38;
    *(u8 *)value = constant;
    value = (u32)sGoldenDivaFanOam;
    constant = sprite;
    *(u32 *)(constant + 4) = value;
    ((struct PrimarySpriteData *)constant)->currentAnimationFrame = zero8;
    ((struct PrimarySpriteData *)constant)->animationTimer = zero16;
    value = 1;
    ((struct PrimarySpriteData *)constant)->pose = value;
}

void FollowGoldenDivaFan(void)
{
    SpriteUtilFindSpriteSlotWork3(PSPRITE_EC);
    gCurrentSprite.pose = 16;
}

void BreakGoldenDivaFan(void)
{
    struct PrimarySpriteData *sprite;
    u32 timer;
    register u32 value PIN("r0");
    u32 temp;
    u8 *work0;
    volatile struct GoldenDivaDmaRegs *dma;

    sprite = &gCurrentSprite;
    work0 = (u8 *)sprite;
    work0 += 39;
    value = *work0;
    temp = value - 1;
    *work0 = temp;
    value = temp << 24;
    timer = value >> 24;
    if (timer != 0) {
        value = timer;
        if (value == 15) {
            dma = (volatile struct GoldenDivaDmaRegs *)0x040000D4;
            dma->src = (u32)sGoldenDivaFanDamageTiles0;
            dma->dst = 0x06013000;
            dma->control = 0x80000200;
            (void)dma->control;
            value = sprite->yPosition;
            value -= 32;
            temp = sprite->xPosition;
            temp -= 80;
            SpawnSecondarySprite(value, temp, 6);
        } else if (value == 10) {
            dma = (volatile struct GoldenDivaDmaRegs *)0x040000D4;
            dma->src = (u32)sGoldenDivaFanDamageTiles1;
            dma->dst = 0x06013400;
            dma->control = 0x80000200;
            (void)dma->control;
            value = sprite->yPosition;
            value -= 64;
            temp = sprite->xPosition;
            temp -= 16;
            SpawnSecondarySprite(value, temp, 6);
        } else if (value == 5) {
            dma = (volatile struct GoldenDivaDmaRegs *)0x040000D4;
            dma->src = (u32)sGoldenDivaFanDamageTiles2;
            dma->dst = 0x06013800;
            dma->control = 0x80000200;
            (void)dma->control;
            value = sprite->yPosition;
            value -= 32;
            temp = sprite->xPosition;
            temp += 48;
            SpawnSecondarySprite(value, temp, 6);
        }
    } else {
        dma = (volatile struct GoldenDivaDmaRegs *)0x040000D4;
        dma->src = (u32)sGoldenDivaFanDamageTiles3;
        dma->dst = 0x06013C00;
        dma->control = 0x80000200;
        (void)dma->control;
        SpriteSpawnAsChild(PSPRITE_0A, 0, 0, sprite->yPosition + 96, sprite->xPosition - 16);
        SpriteSpawnAsChild(PSPRITE_0B, 0, 0, sprite->yPosition, sprite->xPosition - 80);
        SpriteSpawnAsChild(PSPRITE_0B, 0, 0, sprite->yPosition, sprite->xPosition + 48);
        sprite->status = timer;
    }
}

void InitGoldenDivaMask(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 constant PIN("r2");
    register u32 zero8 PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    temp = *(u16 *)value;
    constant = 0xA4;
    constant <<= 8;
    ASM_BARRIER(: "+r"(constant));
    value = constant;
    ASM_BARRIER(: "+r"(value));
    zero8 = 0;
    zero16 = 0;
    value |= temp;
    temp = sprite;
    *(u16 *)temp = value;
    *(u8 *)(temp + 30) = zero8;
    temp += 32;
    value = 16;
    *(u8 *)temp = value;
    temp++;
    *(u8 *)temp = value;
    temp++;
    *(volatile u8 *)temp = value;
    value = sprite;
    value += 35;
    constant = 32;
    *(u8 *)value = constant;
    value++;
    temp = 28;
    *(u8 *)value = temp;
    value++;
    *(u8 *)value = constant;
    value++;
    *(volatile u8 *)value = temp;
    value = (u32)sGoldenDivaMaskFaceOam;
    constant = sprite;
    *(u32 *)(constant + 4) = value;
    ((struct PrimarySpriteData *)constant)->currentAnimationFrame = zero8;
    ((struct PrimarySpriteData *)constant)->animationTimer = zero16;
    value = 110;
    ((struct PrimarySpriteData *)constant)->pose = value;

    switch (((struct PrimarySpriteData *)constant)->roomSlot) {
        case 1:
            ((struct PrimarySpriteData *)constant)->work0 = 96;
            break;
        case 2:
            ((struct PrimarySpriteData *)constant)->work0 = 160;
            break;
        case 3:
            ((struct PrimarySpriteData *)constant)->work0 = 224;
            break;
        default:
            ((struct PrimarySpriteData *)constant)->work0 = 32;
            break;
    }

    if (*(u8 *)&gBossState == 13) {
        *(u16 *)constant &= 0xDFFB;
        switch (((struct PrimarySpriteData *)constant)->roomSlot) {
            case 1:
                GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(constant + 4), sGoldenDivaRedMaskOam);
                break;
            case 2:
                GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(constant + 4), sGoldenDivaGreenMaskOam);
                break;
            case 3:
                GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(constant + 4), sGoldenDivaBlueMaskOam);
                break;
            default:
                GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(constant + 4), sGoldenDivaWhiteMaskOam);
                break;
        }
    }
}

void BeginGoldenDivaMaskOrbit(void)
{
    if (*(u8 *)&gBossState == 6)
        gCurrentSprite.status &= 0xFFFB;

    if (*(u8 *)&gBossState == 13) {
        if ((s16)gSpriteAiRadius > 160)
            gCurrentSprite.work0--;
    } else {
        if ((s16)gSpriteAiRadius > 191)
            gCurrentSprite.work0++;
    }

    OrbitGoldenDivaChild();
    if (*(u8 *)&gBossState == 7)
        gCurrentSprite.pose = 111;
}

void UpdateGoldenDivaMaskOrbit(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u8 *state;
    u32 value;
    u32 temp;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value += 5;
    state = (u8 *)&gCuckooCondorPendulumLength;
    temp = *state;
    value -= temp;
    *work0 = value;
    OrbitGoldenDivaChild();

    if ((*work0 & 0xF8) == 0x40) {
        switch (sprite->roomSlot) {
            case 0:
                if (*state == 4)
                    sprite->pose = 116;
                break;
            case 1:
                if (*state == 3)
                    sprite->pose = 116;
                break;
            case 2:
                if (*state == 2)
                    sprite->pose = 116;
                break;
            case 3:
                if (*state == 1)
                    sprite->pose = 116;
                break;
        }
    }
}

void BeginGoldenDivaMaskAttack(void)
{
    gCurrentSprite.status &= 0xDFF7;
    gCurrentSprite.pose = 117;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 21;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaMaskAttackOam);
    gCurrentSprite.hitboxExtentUp = 96;
    gCurrentSprite.hitboxExtentDown = 4;
    gCurrentSprite.hitboxExtentLeft = 36;
    gCurrentSprite.hitboxExtentRight = 32;
    gCurrentSprite.yPosition += 56;
    m4aSongNumStart(SOUND_B6);
}

void UpdateGoldenDivaMaskAttack(void)
{
    u32 value;
    register u32 state PIN("r1");
    u32 frame;
    u32 velocity;
    u32 spriteOrCounter;
    register struct PrimarySpriteData *sprite PIN("r5");
    u8 *movementType;

    value = (u32)&gCuckooCondorPendulumLength;
    state = *(u8 *)value;
    movementType = (u8 *)value;
    if (state == 1) {
        velocity = (u32)sGoldenDivaMaskAttackPoseMotion;
        state = (u32)&gCurrentSprite;
        spriteOrCounter = state + 42;
        value = *(u8 *)spriteOrCounter;
        value <<= 24;
        frame = value >> 24;
        value >>= 27;
        value <<= 1;
        value += velocity;
        velocity = *(u16 *)value;
        sprite = (struct PrimarySpriteData *)state;
        if (frame > 110)
            *(u8 *)spriteOrCounter = 0;
    } else if (state == 2) {
        velocity = (u32)sGoldenDivaMaskAttackYMotion;
        state = (u32)&gCurrentSprite;
        spriteOrCounter = state + 42;
        value = *(u8 *)spriteOrCounter;
        value <<= 24;
        frame = value >> 24;
        value >>= 27;
        value <<= 1;
        value += velocity;
        velocity = *(u16 *)value;
        sprite = (struct PrimarySpriteData *)state;
        if (frame > 254)
            *(u8 *)spriteOrCounter = 0;
    } else {
        velocity = (u32)sGoldenDivaMaskAttackXMotion;
        state = (u32)&gCurrentSprite;
        spriteOrCounter = state + 42;
        value = *(u8 *)spriteOrCounter;
        value <<= 24;
        frame = value >> 24;
        value >>= 27;
        value <<= 1;
        value += velocity;
        velocity = *(u16 *)value;
        sprite = (struct PrimarySpriteData *)state;
        if (frame > 190)
            *(u8 *)spriteOrCounter = 0;
    }

    spriteOrCounter = (u32)sprite;
    value = ((struct PrimarySpriteData *)spriteOrCounter)->yPosition;
    frame = value + velocity;
    ((struct PrimarySpriteData *)spriteOrCounter)->yPosition = frame;
    state = spriteOrCounter + 42;
    value = *(u8 *)state;
    value++;
    *(u8 *)state = value;
    state = ((struct PrimarySpriteData *)spriteOrCounter)->status;
    value = SPRITE_STATUS_FACING_RIGHT;
    value &= state;
    if (value != 0) {
        value = *movementType;
        state = 4;
        state -= value;
        state <<= 1;
        state += 6;
        value = ((struct PrimarySpriteData *)spriteOrCounter)->xPosition;
        state += value;
        ((struct PrimarySpriteData *)spriteOrCounter)->xPosition = state;
        value = frame - 32;
        value <<= 16;
        value >>= 16;
        state += 32;
        state = (u16)state;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, state);
        value = gSpriteCollisionTileType;
        if (value == 17) {
            state = ((struct PrimarySpriteData *)spriteOrCounter)->status;
            value = 0xFFBF;
            value &= state;
            ((struct PrimarySpriteData *)spriteOrCounter)->status = value;
        }
    } else {
        state = *movementType;
        value = 4;
        value -= state;
        value <<= 1;
        value += 6;
        state = sprite->xPosition;
        state -= value;
        sprite->xPosition = state;
        value = sprite->yPosition;
        value -= 32;
        value = (u16)value;
        state -= 32;
        state = (u16)state;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, state);
        value = gSpriteCollisionTileType;
        if (value == 17) {
            value = sprite->status;
            state = SPRITE_STATUS_FACING_RIGHT;
            value |= state;
            sprite->status = value;
        }
    }
}

void BeginGoldenDivaMaskFall(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaMaskSpinOam);
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.status &= 0xFCFF;
    gCurrentSprite.work0 = 4;
}

void UpdateGoldenDivaMaskFall(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value--;
    *work0 = value;
    value <<= 24;
    if (value == 0) {
        sprite->pose = 17;
    } else {
        func_80238E8();
        func_8023B88();
        if (gSpriteCollisionResult == 0)
            sprite->pose = 27;
    }
}

void BeginGoldenDivaMaskBounce(void)
{
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 40;
    gCurrentSprite.work3 = 0;
}

void UpdateGoldenDivaMaskBounce(void)
{
    struct PrimarySpriteData *s;
    register u32 timer PIN("r6");
    u32 frame;
    u32 velocity;
    const s16 *table;
    register u32 next PIN("r0");
    register u32 value PIN("r1");

    s = &gCurrentSprite;
    timer = s->work0;
    velocity = timer;
    if (velocity != 0) {
        frame = s->work3;
        table = sGoldenDivaMaskBounceMotion;
        velocity = (u16)table[frame];
        if (table[frame] == 0x7FFF) {
            velocity = (u16)table[0];
            frame = 0;
        }
        s->work3 = frame + 1;
        s->yPosition += velocity;
        next = timer - 1;
        s->work0 = next;
        value = (u8)next;
        if (value == 0) {
            s->warioCollision = 21;
            s->currentAnimationFrame = 0;
            s->animationTimer = value;
            GP_SET(s->pOamData, sGoldenDivaMaskAttackOam);
        }
    } else {
        if ((u16)((s->yPosition >> 2) - (gBg1YPosition >> 2)) > 80) {
            s->yPosition -= 4;
        } else {
            s->pose = 117;
            s->work3 = velocity;
        }
    }
}

void BeginGoldenDivaMaskGrounded(void)
{
    gCurrentSprite.status |= 4;
    gCurrentSprite.warioCollision = 21;
    gCurrentSprite.pose = 20;
    gCurrentSprite.work0 = 90;
    gCurrentSprite.work3 = 0;
}

void UpdateGoldenDivaMaskGrounded(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.yPosition = gWarioData.yPosition;
    gCurrentSprite.xPosition = gWarioData.xPosition;
    if (gWarioData.pose != 0 || gWarioData.reaction == 0) {
        gCurrentSprite.disableWarioCollisionTimer = 15;
        gCurrentSprite.status &= 0xFFFB;
        gCurrentSprite.pose = 22;
    }
}

void BeginGoldenDivaMaskThrow(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    u32 temp;
    u32 yPosition;

    sprite = &gCurrentSprite;
    yPosition = sprite->yPosition;
    value = yPosition >> 2;
    temp = gBg1YPosition;
    temp >>= 2;
    value -= temp;
    value <<= 16;
    temp = 0xFFAF0000;
    value += temp;
    value >>= 16;
    if (value > 79) {
        value = yPosition + 4;
        sprite->yPosition = value;
    } else {
        value = 0;
        sprite->pose = 117;
        temp = (u32)sprite + 42;
        *(u8 *)temp = value;
    }
}

void PrepareGoldenDivaMaskMovement(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaMaskGroundRight(void)
{
    gCurrentSprite.pose = 44;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaMaskGroundLeft(void)
{
    gCurrentSprite.pose = 46;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaMaskSlopeRight(void)
{
    gCurrentSprite.pose = 72;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaMaskSlopeLeft(void)
{
    gCurrentSprite.pose = 74;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void ResetGoldenDivaMaskAnimation(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaMaskSpinOam);
}

void MoveGoldenDivaMaskRight(void)
{
    gCurrentSprite.pose = 52;
    ResetGoldenDivaMaskAnimation();
}

void MoveGoldenDivaMaskLeft(void)
{
    gCurrentSprite.pose = 54;
    ResetGoldenDivaMaskAnimation();
}

void UpdateGoldenDivaMaskLanding(void)
{
    if (gCurrentSprite.xPosition > gWarioData.xPosition)
        MoveGoldenDivaMaskRight();
    else
        MoveGoldenDivaMaskLeft();
}

void BeginGoldenDivaMaskLiftRight(void)
{
    gCurrentSprite.pose = 82;
}

void BeginGoldenDivaMaskLiftLeft(void)
{
    gCurrentSprite.pose = 84;
}

void BeginGoldenDivaMaskCarryRight(void)
{
    gCurrentSprite.pose = 88;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void BeginGoldenDivaMaskCarryLeft(void)
{
    gCurrentSprite.pose = 86;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void PrepareGoldenDivaMaskHardThrow(void)
{
    gCurrentSprite.work1 = 0;
}

void UpdateGoldenDivaMaskHardThrow(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;
    u32 inverse;
    u32 zero;
    s32 cosine;
    s32 scale;
    const s16 *table;
    s32 sine;

    sprite = &gCurrentSprite;
    sprite->status |= 0x80;
    {
        register u8 *anglePointer PIN("r1");

        anglePointer = &sprite->work1;
        value = *anglePointer;
        value -= 32;
        *anglePointer = value;
        cosine = *anglePointer;
    }
    table = sSinCosTable;
    value = cosine;
    value += 64;
    value <<= 1;
    value += (u32)table;
    zero = 0;
    inverse = *(s16 *)value;
    sine = inverse;
    scale = 256;

    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePA = value;

    cosine <<= 1;
    cosine += (u32)table;
    value = zero;
    cosine = *(s16 *)cosine;
    value = FixedInverse(scale);
    inverse = value;
    value = cosine;
    value = FixedMul(value, inverse);
    sprite->affinePB = value;

    cosine = -cosine;
    value = FixedInverse(scale);
    inverse = value;
    value = cosine;
    value = FixedMul(value, inverse);
    sprite->affinePC = value;

    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePD = value;
}

void InitGoldenDivaExplosiveBall(void)
{
    u32 value;
    u32 status;
    register u32 zero PIN("r2");
    register u32 temp PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 pointer PIN("r5");
    u32 sprite;

    value = (u32)&gCurrentSprite;
    sprite = value;
    value = *(u16 *)value;
    zero = 128;
    zero <<= 8;
    ASM_BARRIER(: "+r"(zero));
    status = zero;
    zero = 0;
    zero16 = 0;
    status |= value;
    value = 0xFFFB;
    status &= value;
    temp = sprite;
    *(u16 *)temp = status;
    value = 69;
    *(u8 *)(temp + 30) = value;
    value = sprite;
    value += 32;
    temp = 24;
    *(u8 *)value = temp;
    pointer = sprite;
    pointer += 33;
    value = 8;
    *(u8 *)pointer = value;
    pointer++;
    value = 16;
    *(u8 *)pointer = value;
    pointer++;
    value = 48;
    *(volatile u8 *)pointer = value;
    value = sprite;
    value += 36;
    *(u8 *)value = zero;
    value = (u32)sGoldenDivaExplosiveBallIdleOam;
    pointer = sprite;
    *(u32 *)(pointer + 4) = value;
    *(u8 *)(pointer + 22) = zero;
    ((struct PrimarySpriteData *)pointer)->animationTimer = zero16;
    value = sprite;
    value += 39;
    *(u8 *)value = temp;
    temp = sprite;
    temp += 41;
    value = 4;
    *(u8 *)temp = value;
    value = sprite;
    value += 42;
    *(u8 *)value = zero;
    if ((status & 64) != 0) {
        value = 113;
        ((struct PrimarySpriteData *)pointer)->pose = value;
    } else {
        value = 111;
        status = sprite;
        ((struct PrimarySpriteData *)status)->pose = value;
    }
}

void BeginGoldenDivaExplosiveBallOrbit(void)
{
    struct PrimarySpriteData *sprite;
    register u8 *work3 PIN("ip");
    u32 value;
    u32 temp;
    u32 frame;
    register u32 velocity PIN("r4");
    const s16 *table;
    u32 zero;

    sprite = &gCurrentSprite;
    value = 42;
    value += (u32)sprite;
    work3 = (u8 *)value;
    frame = *work3;
    table = sGoldenDivaExplosiveBallOrbitMotion;
    value = frame << 1;
    value += (u32)table;
    velocity = *(u16 *)value;
    zero = 0;
    temp = *(s16 *)value;
    value = 0x7FFF;
    if (temp == value) {
        temp = frame - 1;
        temp <<= 1;
        temp += (u32)table;
        value = sprite->yPosition;
        temp = *(u16 *)temp;
        value += temp;
    } else {
        value = frame + 1;
        temp = (u32)work3;
        *(u8 *)temp = value;
        value = sprite->yPosition;
        value += velocity;
    }
    sprite->yPosition = value;
    func_8024570();
}

void UpdateGoldenDivaExplosiveBallOrbit(void)
{
    struct PrimarySpriteData *sprite;
    register u8 *work3 PIN("ip");
    u32 value;
    u32 temp;
    u32 frame;
    register u32 velocity PIN("r4");
    const s16 *table;
    u32 zero;

    sprite = &gCurrentSprite;
    value = 42;
    value += (u32)sprite;
    work3 = (u8 *)value;
    frame = *work3;
    table = sGoldenDivaExplosiveBallOrbitMotion;
    value = frame << 1;
    value += (u32)table;
    velocity = *(u16 *)value;
    zero = 0;
    temp = *(s16 *)value;
    value = 0x7FFF;
    if (temp == value) {
        temp = frame - 1;
        temp <<= 1;
        temp += (u32)table;
        value = sprite->yPosition;
        temp = *(u16 *)temp;
        value += temp;
    } else {
        value = frame + 1;
        temp = (u32)work3;
        *(u8 *)temp = value;
        value = sprite->yPosition;
        value += velocity;
    }
    sprite->yPosition = value;
    func_80245FC();
}

void BeginGoldenDivaExplosiveBallBounce(void)
{
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 44;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaExplosiveBallBounceOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
}

void UpdateGoldenDivaExplosiveBallBounce(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value--;
    *work0 = value;
    value <<= 24;
    if (value == 0)
        sprite->pose = 15;
}

void BeginGoldenDivaExplosiveBallFall(void)
{
    struct PrimarySpriteData *sprite;
    u32 field;

    sprite = &gCurrentSprite;
    if (GP(sprite->pOamData) == sGoldenDivaExplosiveBallIdleOam) {
        sprite->pose = 17;
    } else {
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        GP_SET(sprite->pOamData, sGoldenDivaExplosiveBallFallOam);
        sprite->pose = 16;
        sprite->warioCollision = 5;
        sprite->status &= 0xFCFF;
        field = (u32)sprite;
        field += 39;
        *(u8 *)field = 150;
    }
}

void UpdateGoldenDivaExplosiveBallFall(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;
    u32 truncated;
    u32 remaining;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value--;
    *work0 = value;
    truncated = (u8)value;
    remaining = truncated;
    if (truncated <= 49) {
        value = 1;
        value &= truncated;
        if (value == 0)
            sprite->status ^= 4;
        if (remaining == 0) {
            sprite->status = remaining;
            return;
        }
    }

    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0)
        gCurrentSprite.pose = 27;
}

void PrepareGoldenDivaExplosiveBallMovement(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaExplosiveBallGroundRight(void)
{
    gCurrentSprite.pose = 44;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaExplosiveBallGroundLeft(void)
{
    gCurrentSprite.pose = 46;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaExplosiveBallSlopeRight(void)
{
    gCurrentSprite.pose = 72;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaExplosiveBallSlopeLeft(void)
{
    gCurrentSprite.pose = 74;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void ResetGoldenDivaExplosiveBallThrow(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.status &= 0xFFFB;
}

void MoveGoldenDivaExplosiveBallRight(void)
{
    gCurrentSprite.pose = 52;
    ResetGoldenDivaExplosiveBallThrow();
}

void MoveGoldenDivaExplosiveBallLeft(void)
{
    gCurrentSprite.pose = 54;
    ResetGoldenDivaExplosiveBallThrow();
}

void UpdateGoldenDivaExplosiveBallLanding(void)
{
    if (gCurrentSprite.xPosition > gWarioData.xPosition)
        MoveGoldenDivaExplosiveBallRight();
    else
        MoveGoldenDivaExplosiveBallLeft();
}

void BeginGoldenDivaExplosiveBallLiftRight(void)
{
    gCurrentSprite.pose = 82;
    gCurrentSprite.status &= 0xFFFB;
}

void BeginGoldenDivaExplosiveBallLiftLeft(void)
{
    gCurrentSprite.pose = 84;
    gCurrentSprite.status &= 0xFFFB;
}

void BeginGoldenDivaExplosiveBallCarryRight(void)
{
    gCurrentSprite.pose = 88;
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.status &= 0xFFFB;
}

void BeginGoldenDivaExplosiveBallCarryLeft(void)
{
    gCurrentSprite.pose = 86;
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.status &= 0xFFFB;
}

void PrepareGoldenDivaExplosiveBallHardThrow(void)
{
    gCurrentSprite.work1 = 0;
}

void UpdateGoldenDivaExplosiveBallHardThrow(void)
{
    u32 value;
    register u32 inverse PIN("r1");
    u32 zero;
    s32 angle;
    s32 scale;
    const s16 *table;
    struct PrimarySpriteData *sprite;
    s32 sine;
    struct PrimarySpriteData *initial;

    initial = &gCurrentSprite;
    value = initial->work0;
    sprite = initial;
    if (value != 2)
        return;

    sprite->warioCollision = 51;
    inverse = sprite->status;
    value = 0x80;
    value |= inverse;
    sprite->status = value;
    inverse = 0x40;
    value &= inverse;
    if (value != 0) {
        inverse = (u32)sprite;
        inverse += 40;
        value = *(u8 *)inverse;
        value += 16;
    } else {
        inverse = (u32)sprite;
        inverse += 40;
        value = *(u8 *)inverse;
        value -= 16;
    }
    *(u8 *)inverse = value;
    value = (u32)sprite;
    value += 40;
    angle = *(u8 *)value;
    table = sSinCosTable;
    value = angle;
    value += 64;
    value <<= 1;
    value += (u32)table;
    zero = 0;
    sine = *(s16 *)(value + zero);
    scale = 256;
    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePA = value;

    angle <<= 1;
    angle += (u32)table;
    value = 0;
    angle = *(s16 *)(angle + value);
    value = FixedInverse(scale);
    inverse = value;
    value = angle;
    value = FixedMul(value, inverse);
    sprite->affinePB = value;

    angle = -angle;
    value = FixedInverse(scale);
    inverse = value;
    value = angle;
    value = FixedMul(value, inverse);
    sprite->affinePC = value;

    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePD = value;
}

void InitGoldenDivaBlackEgg(void)
{
    gCurrentSprite.status &= 0xFFFB;
    gCurrentSprite.warioCollision = 47;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 0;
    gCurrentSprite.drawDistanceLeftRight = 24;
    gCurrentSprite.hitboxExtentUp = 48;
    gCurrentSprite.hitboxExtentDown = 0;
    gCurrentSprite.hitboxExtentLeft = 28;
    gCurrentSprite.hitboxExtentRight = 24;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggIdleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 17;
}

void BeginGoldenDivaBlackEggBounce(void)
{
    gCurrentSprite.pose = 18;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.work2 = 0;
}

void UpdateGoldenDivaBlackEggBounce(void)
{
    u32 value;
    u32 temp;
    u32 current;
    u32 pointer;
    u32 index;
    s32 movement;
    register u32 sprite PIN("r6");
    u32 table;

    sprite = (u32)&gCurrentSprite;
    table = sprite;
    table += 42;
    index = *(u8 *)table;
    if (index == 1) {
        value = 109;
        m4aSongNumStart(value);
    }

    current = (u32)sGoldenDivaBlackEggBounceAngles;
    value = index << 1;
    value += current;
    movement = *(u16 *)value;
    pointer = 0;
    temp = *(s16 *)(value + pointer);
    value = 0x7FFF;
    if (temp == value) {
        temp = index - 1;
        temp <<= 1;
        temp += current;
        value = ((struct PrimarySpriteData *)sprite)->yPosition;
        temp = *(u16 *)temp;
        value += temp;
    } else {
        value = index + 1;
        *(u8 *)table = value;
        value = ((struct PrimarySpriteData *)sprite)->yPosition;
        value += movement;
    }
    ((struct PrimarySpriteData *)sprite)->yPosition = value;

    current = (u32)&gCurrentSprite;
    pointer = current;
    pointer += 41;
    index = *(u8 *)pointer;
    table = (u32)sGoldenDivaBlackEggBounceMotion;
    value = index >> 1;
    value <<= 1;
    value += table;
    movement = *(u16 *)value;
    sprite = 0;
    temp = *(s16 *)(value + sprite);
    value = 0x7FFF;
    sprite = current;
    if (temp == value) {
        value = *(u16 *)sprite;
        temp = 0x40;
        value ^= temp;
        *(u16 *)sprite = value;
        value = 1;
        *(u8 *)pointer = value;
        temp = sprite;
        temp += 42;
        *(u8 *)temp = value;
        movement = *(u16 *)table;
    } else {
        value = index + 1;
        *(u8 *)pointer = value;
    }

    index = sprite;
    value = ((struct PrimarySpriteData *)index)->yPosition;
    temp = ((struct PrimarySpriteData *)index)->xPosition;
    /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
    ((void (*)(u32, u16))func_8023BFC)(value, temp);
    value = gSpriteCollisionTileType;
    temp = value;
    value = 15;
    value &= temp;
    if (value != 0) {
        temp = *(u16 *)index;
        value = 2;
        value &= temp;
        value = (u16)value;
        if (value == 0) {
            *(u16 *)index = value;
            return;
        }
        value = 1;
        *(u8 *)(index + 26) = value;
        *(u8 *)(index + 31) = value;
        value = 4;
        value ^= temp;
        *(u16 *)index = value;
        temp = 0x40;
        value &= temp;
        if (value != 0) {
            value = *(u16 *)(index + 10);
            value += movement;
            *(u16 *)(index + 10) = value;
        } else {
            value = *(u16 *)(index + 10);
            value -= movement;
            *(u16 *)(index + 10) = value;
        }
        return;
    }

    value = *(u16 *)index;
    temp = 0xFFFB;
    temp &= value;
    *(u16 *)index = temp;
    value = 0x40;
    value &= temp;
    if (value != 0) {
        temp = *(u16 *)(index + 10);
        temp += movement;
        *(u16 *)(index + 10) = temp;
        value = *(u16 *)(index + 8);
        value -= 32;
        value = (u16)value;
        current = index;
        current += 38;
        current = *(u8 *)current;
        temp += current;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
    } else {
        temp = *(u16 *)(sprite + 10);
        temp -= movement;
        *(u16 *)(sprite + 10) = temp;
        value = *(u16 *)(sprite + 8);
        value -= 32;
        value = (u16)value;
        current = sprite;
        current += 37;
        current = *(u8 *)current;
        temp -= current;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
    }

    value = gSpriteCollisionTileType;
    temp = value;
    value = 15;
    value &= temp;
    if (value != 0) {
        current = (u32)&gCurrentSprite;
        value = *(u16 *)current;
        temp = 0x40;
        value ^= temp;
        *(u16 *)current = value;
        temp = current;
        temp += 41;
        value = 1;
        *(u8 *)temp = value;
        current += 42;
        *(u8 *)current = value;
    }
}

void BeginGoldenDivaBlackEggFall(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggFallOam);
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.status &= 0xFCFF;
    gCurrentSprite.work0 = 120;
}

void UpdateGoldenDivaBlackEggFall(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;
    u32 truncated;
    u32 remaining;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value--;
    *work0 = value;
    truncated = (u8)value;
    remaining = truncated;
    if (truncated <= 59) {
        value = 1;
        value &= truncated;
        if (value == 0)
            sprite->status ^= 4;
        if (remaining == 0) {
            sprite->status = remaining;
            return;
        }
    }

    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0)
        gCurrentSprite.pose = 27;
}

void PrepareGoldenDivaBlackEggMovement(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaBlackEggGroundRight(void)
{
    gCurrentSprite.pose = 44;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaBlackEggGroundLeft(void)
{
    gCurrentSprite.pose = 46;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaBlackEggSlopeRight(void)
{
    gCurrentSprite.pose = 72;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaBlackEggSlopeLeft(void)
{
    gCurrentSprite.pose = 74;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void ResetGoldenDivaBlackEggAnimation(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.status &= 0xFFFB;
    if (gCurrentSprite.warioCollision == 47) {
        GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggSlopeOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.warioCollision = 5;
    }
}

void MoveGoldenDivaBlackEggRight(void)
{
    gCurrentSprite.pose = 52;
    ResetGoldenDivaBlackEggAnimation();
}

void MoveGoldenDivaBlackEggLeft(void)
{
    gCurrentSprite.pose = 54;
    ResetGoldenDivaBlackEggAnimation();
}


void UpdateGoldenDivaBlackEggLanding(void)
{
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggLandingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    gCurrentSprite.status = (gCurrentSprite.status & 0xFEFB) | 0x200;
}

void MoveGoldenDivaBlackEggOffscreenRight(void)
{
    if (gCurrentSprite.pose == 106 && gCurrentSprite.health != 0)
        SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition - 32, 6);
    else if (gCurrentSprite.pose != 106)
        SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);

    gCurrentSprite.pose = 32;
    UpdateGoldenDivaBlackEggLanding();
}

void MoveGoldenDivaBlackEggOffscreenLeft(void)
{
    if (gCurrentSprite.pose == 105 && gCurrentSprite.health != 0)
        SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition + 32, 6);
    else if (gCurrentSprite.pose != 105)
        SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);

    gCurrentSprite.pose = 34;
    UpdateGoldenDivaBlackEggLanding();
}

void BeginGoldenDivaBlackEggLiftRight(void)
{
    gCurrentSprite.pose = 82;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggCarryOam);
    gCurrentSprite.status &= 0xFFFB;
    gCurrentSprite.health = 0;
}

void BeginGoldenDivaBlackEggLiftLeft(void)
{
    gCurrentSprite.pose = 84;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggCarryOam);
    gCurrentSprite.status &= 0xFFFB;
    gCurrentSprite.health = 0;
}

void BeginGoldenDivaBlackEggCarryRight(void)
{
    gCurrentSprite.pose = 88;
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggCarryOam);
    gCurrentSprite.status &= 0xFFFB;
    gCurrentSprite.health = 0;
}

void BeginGoldenDivaBlackEggCarryLeft(void)
{
    gCurrentSprite.pose = 86;
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackEggCarryOam);
    gCurrentSprite.status &= 0xFFFB;
    gCurrentSprite.health = 0;
}

void PrepareGoldenDivaBlackEggHardThrow(void)
{
    gCurrentSprite.work1 = 0;
}

void UpdateGoldenDivaBlackEggHardThrow(void)
{
    u32 value;
    register u32 inverse PIN("r1");
    u32 zero;
    s32 angle;
    s32 scale;
    const s16 *table;
    struct PrimarySpriteData *sprite;
    s32 sine;
    struct PrimarySpriteData *initial;

    initial = &gCurrentSprite;
    inverse = initial->status;
    value = 0xA0;
    value |= inverse;
    initial->status = value;
    value = initial->health;
    sprite = initial;
    if (value == 2 || value == 17) {
        inverse = (u32)sprite;
        inverse += 40;
        value = *(u8 *)inverse;
        value -= 32;
    } else {
        inverse = (u32)sprite;
        inverse += 40;
        value = *(u8 *)inverse;
        value += 32;
    }
    *(u8 *)inverse = value;
    value = (u32)sprite;
    value += 40;
    angle = *(u8 *)value;
    table = sSinCosTable;
    value = angle;
    value += 64;
    value <<= 1;
    value += (u32)table;
    zero = 0;
    sine = *(s16 *)(value + zero);
    scale = 256;
    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePA = value;

    angle <<= 1;
    angle += (u32)table;
    value = 0;
    angle = *(s16 *)(angle + value);
    value = FixedInverse(scale);
    inverse = value;
    value = angle;
    value = FixedMul(value, inverse);
    sprite->affinePB = value;

    angle = -angle;
    value = FixedInverse(scale);
    inverse = value;
    value = angle;
    value = FixedMul(value, inverse);
    sprite->affinePC = value;

    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePD = value;
}

void InitGoldenDivaGreenEnemy(void)
{
    u32 value;
    register u32 temp PIN("r1");
    u32 status;
    register u32 zero PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 sprite PIN("ip");

    value = (u32)&gCurrentSprite;
    sprite = value;
    value = *(u16 *)value;
    status = 0xFFFB;
    status &= value;
    zero = 0;
    zero16 = 0;
    temp = sprite;
    *(u16 *)temp = status;
    value = 5;
    *(u8 *)(temp + 30) = value;
    temp += 32;
    value = 16;
    *(u8 *)temp = value;
    value = sprite;
    value += 33;
    *(u8 *)value = zero;
    temp += 2;
    value = 8;
    *(u8 *)temp = value;
    temp++;
    value = 44;
    *(u8 *)temp = value;
    value = sprite;
    value += 36;
    *(u8 *)value = zero;
    temp += 2;
    value = 24;
    *(u8 *)temp = value;
    temp++;
    *(volatile u8 *)temp = value;
    value = (u32)sGoldenDivaGreenEnemyIdleOam;
    temp = sprite;
    *(u32 *)(temp + 4) = value;
    ((struct PrimarySpriteData *)temp)->currentAnimationFrame = zero;
    ((struct PrimarySpriteData *)temp)->animationTimer = zero16;
    value = 64;
    value &= status;
    status = sprite;
    if (value != 0) {
        value = 52;
    } else {
        value = 54;
    }
    ((struct PrimarySpriteData *)status)->pose = value;
    zero = status;
    zero += 39;
    temp = 0;
    value = 10;
    *(u8 *)zero = value;
    zero += 2;
    ASM_BARRIER(: "+r"(zero));
    value = 5;
    *(u8 *)zero = value;
    value = status;
    value += 42;
    *(u8 *)value = temp;
}

void UpdateGoldenDivaGreenEnemy(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 sprite PIN("r2");
    register u32 zero PIN("r3");
    register u32 zero16 PIN("r4");

    sprite = (u32)&gCurrentSprite;
    zero16 = 0;
    value = 1;
    ((struct PrimarySpriteData *)sprite)->disableWarioCollisionTimer = value;
    temp = *(u16 *)sprite;
    zero = 0x400;
    ASM_BARRIER(: "+r"(zero));
    value = zero;
    zero = 0;
    value |= temp;
    temp = 0xFEFF;
    value &= temp;
    *(u16 *)sprite = value;
    value = (u32)sGoldenDivaGreenEnemyActiveOam;
    *(u32 *)(sprite + 4) = value;
    *(u8 *)(sprite + 22) = zero;
    *(u16 *)(sprite + 20) = zero16;
    value = 123;
    *(u8 *)(sprite + 28) = value;
    sprite += 39;
    value = 36;
    *(u8 *)sprite = value;
}

void BeginGoldenDivaGreenEnemyAttack(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;
    u32 decremented;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    work0 = &sprite->work0;
    value = *work0;
    decremented = value - 1;
    *work0 = decremented;
    value = decremented << 24;
    value >>= 24;
    if (value == 0)
        goto zero;
    value = decremented << 24;
    value >>= 24;
    if (value == 28) {
        SpriteSpawnAsChild(245, 0, 0, sprite->yPosition, sprite->xPosition);
        goto end;
    }
    goto end;
zero:
    sprite->status = value;
end:
    return;
}

void PrepareGoldenDivaGreenEnemyMovement(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaGreenEnemyGroundRight(void)
{
    gCurrentSprite.pose = 52;
    PrepareGoldenDivaGreenEnemyMovement();
}

void BeginGoldenDivaGreenEnemyGroundLeft(void)
{
    gCurrentSprite.pose = 54;
    PrepareGoldenDivaGreenEnemyMovement();
}

void BeginGoldenDivaGreenEnemyFall(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void UpdateGoldenDivaGreenEnemyLanding(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work2;
    u32 zero;
    register u32 value PIN("r0");

    sprite = &gCurrentSprite;
    work2 = &sprite->work2;
    zero = 0;
    value = 8;
    *work2 = value;
    value = (u32)sprite;
    value += 42;
    *(u8 *)value = zero;
    sprite->drawPriority = zero;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    sprite->status = (sprite->status & 0xFEFF) | 0x200;
}

void MoveGoldenDivaGreenEnemyRight(void)
{
    gCurrentSprite.pose = 32;
    UpdateGoldenDivaGreenEnemyLanding();
}

void MoveGoldenDivaGreenEnemyLeft(void)
{
    gCurrentSprite.pose = 34;
    UpdateGoldenDivaGreenEnemyLanding();
}

void ResetGoldenDivaGreenEnemyMotion(void)
{
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaGreenEnemySlopeRight(void)
{
    gCurrentSprite.pose = 72;
    ResetGoldenDivaGreenEnemyMotion();
}

void BeginGoldenDivaGreenEnemySlopeLeft(void)
{
    gCurrentSprite.pose = 74;
    ResetGoldenDivaGreenEnemyMotion();
}

void BeginGoldenDivaGreenEnemyLiftRight(void)
{
    gCurrentSprite.pose = 82;
}

void BeginGoldenDivaGreenEnemyLiftLeft(void)
{
    gCurrentSprite.pose = 84;
}

void UpdateGoldenDivaGreenEnemyTransformation(void)
{
    u32 value;
    register u32 temp PIN("r1");
    u32 remaining;
    register u32 sprite PIN("r3");
    register u32 zero PIN("r4");
    register u32 work0 PIN("r12");

    sprite = (u32)&gCurrentSprite;
    value = *(u32 *)(sprite + 4);
    temp = (u32)sGoldenDivaGreenEnemyTransformOam;
    if (value != temp) {
        *(u32 *)(sprite + 4) = temp;
        value = 0;
        ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = value;
        ((struct PrimarySpriteData *)sprite)->animationTimer = value;
        temp = sprite;
        temp += 39;
        value = 70;
        *(u8 *)temp = value;
    }

    value = 39;
    ASM_BARRIER(: "+r"(value));
    value += sprite;
    work0 = value;
    value = *(u8 *)value;
    value--;
    temp = work0;
    *(u8 *)temp = value;
    remaining = (u8)value;
    if (remaining != 0)
        return;

    temp = *(u16 *)sprite;
    zero = 128;
    zero <<= 3;
    ASM_BARRIER(: "+r"(zero));
    value = zero;
    zero = 0;
    value |= temp;
    temp = 0xFEFF;
    value &= temp;
    *(u16 *)sprite = value;
    value = (u32)sGoldenDivaGreenEnemyActiveOam;
    *(u32 *)(sprite + 4) = value;
    *(u8 *)(sprite + 22) = zero;
    *(u16 *)(sprite + 20) = remaining;
    value = 69;
    *(u8 *)(sprite + 30) = value;
    value = 121;
    *(u8 *)(sprite + 28) = value;
    remaining = 36;
    value = work0;
    *(u8 *)value = remaining;
    temp = sprite;
    temp += 35;
    value = 48;
    *(u8 *)temp = value;
    temp++;
    value = 32;
    *(u8 *)temp = value;
    temp++;
    value = 40;
    *(volatile u8 *)temp = value;
    value = sprite;
    value += 38;
    *(u8 *)value = remaining;
    value = *(u16 *)(sprite + 8);
    value -= 16;
    temp = *(u16 *)(sprite + 10);
    remaining = 64;
    SpawnSecondarySprite(value, temp, remaining);
    value = (u32)&gCurrentCarriedSprite;
    *(u8 *)value = zero;
}

void BeginGoldenDivaGreenEnemyTransformation(void)
{
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaGreenEnemyTransformOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.work0 = 70;
}

void BeginGoldenDivaGreenEnemyCarryRight(void)
{
    gCurrentSprite.pose = 88;
    BeginGoldenDivaGreenEnemyTransformation();
}

void BeginGoldenDivaGreenEnemyCarryLeft(void)
{
    SpriteUtilCarryingSpriteRight();
    UpdateGoldenDivaGreenEnemyTransformation();
}

void BeginGoldenDivaGreenEnemyThrownRight(void)
{
    gCurrentSprite.pose = 86;
    BeginGoldenDivaGreenEnemyTransformation();
}

void BeginGoldenDivaGreenEnemyThrownLeft(void)
{
    SpriteUtilCarryingSpriteLeft();
    UpdateGoldenDivaGreenEnemyTransformation();
}

void BeginGoldenDivaGreenEnemyDefeat(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;
    u32 decremented;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    decremented = value - 1;
    *work0 = decremented;
    value = decremented << 24;
    value >>= 24;
    if (value == 0)
        goto zero;
    value = decremented << 24;
    value >>= 24;
    if (value == 28) {
        value = 0;
        sprite->warioCollision = value;
    }
    goto end;
zero:
    sprite->status = value;
end:
    return;
}

void UpdateGoldenDivaGreenEnemyDefeat(void)
{
    struct PrimarySpriteData *sprite;
    u32 zero;

    sprite = &gCurrentSprite;
    SpawnSecondarySprite(sprite->yPosition - 16, sprite->xPosition, 64);
    zero = 0;
    sprite->status = zero;
}

void InitGoldenDivaBlackChick(void)
{
    u32 value;
    register u32 ptr PIN("r1");
    register u32 zero16 PIN("r2");
    u32 sprite;
    register u32 zero PIN("r4");
    u32 status;

    sprite = (u32)&gCurrentSprite;
    value = *(u16 *)sprite;
    status = 0xFFFB;
    status &= value;
    zero = 0;
    zero16 = 0;
    *(u16 *)sprite = status;
    value = 2;
    *(u8 *)(sprite + 30) = value;
    ptr = sprite;
    ptr += 32;
    value = 16;
    *(u8 *)ptr = value;
    value = sprite;
    value += 33;
    *(u8 *)value = zero;
    ptr += 2;
    value = 8;
    *(u8 *)ptr = value;
    ptr++;
    value = 40;
    *(u8 *)ptr = value;
    value = sprite;
    value += 36;
    *(u8 *)value = zero;
    ptr += 2;
    value = 24;
    *(u8 *)ptr = value;
    ptr++;
    value = 20;
    *(u8 *)ptr = value;
    value = (u32)sGoldenDivaBlackChickIdleOam;
    *(u32 *)(sprite + 4) = value;
    *(u8 *)(sprite + 22) = zero;
    *(u16 *)(sprite + 20) = zero16;
    value = 28;
    *(u8 *)(sprite + 28) = value;
    ptr++;
    value = 10;
    *(u8 *)ptr = value;
    ptr++;
    value = 170;
    *(volatile u8 *)ptr = value;
    value = sprite;
    value += 42;
    *(u8 *)value = zero;
    ptr = (u32)&gWarioData;
    value = *(u16 *)(sprite + 10);
    ptr = *(u16 *)(ptr + 18);
    if (value < ptr) {
        value = 0xFFBF;
        value &= status;
    } else {
        ptr = 64;
        value = status;
        value |= ptr;
    }
    *(u16 *)sprite = value;
    value = 210;
    m4aSongNumStart(value);
}

void BeginGoldenDivaBlackChickFall(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;
    u32 remaining;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value--;
    *work0 = value;
    remaining = (u8)value;
    if (remaining == 0) {
        value = 16;
        sprite->pose = value;
        value = (u32)sprite;
        value += 42;
        *(u8 *)value = remaining;
        value = 30;
        *work0 = value;
    }
}

void UpdateGoldenDivaBlackChickFall(void)
{
    u32 value;
    u32 temp;
    u32 index;
    s32 movement;
    u32 sprite;
    u32 table;
    register u32 indexPointer PIN("r6");
    u32 scratch;
    u32 work0;

    sprite = (u32)&gCurrentSprite;
    table = sprite;
    table += 42;
    value = *(u8 *)table;
    if (value == 1) {
        value = ((struct PrimarySpriteData *)sprite)->yPosition;
        value += 8;
        value = (u16)value;
        temp = ((struct PrimarySpriteData *)sprite)->xPosition;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        value = gSpriteCollisionTileType;
        if (value != 17) {
            temp = 0;
            value = 28;
            ((struct PrimarySpriteData *)sprite)->pose = value;
            *(u8 *)table = temp;
            return;
        }
    }

    value = (u32)&gCurrentSprite;
    temp = 39;
    ASM_BARRIER(: "+r"(temp));
    temp += value;
    work0 = temp;
    index = *(u8 *)temp;
    sprite = value;
    if (index != 0) {
        value = index - 1;
        *(u8 *)temp = value;
    }

    indexPointer = sprite;
    indexPointer += 42;
    index = *(u8 *)indexPointer;
    table = (u32)sGoldenDivaBlackChickFallMotion;
    value = index << 1;
    value += table;
    movement = *(u16 *)value;
    scratch = 0;
    /* agbcc otherwise spills the r0 address while forming this signed halfword load. */
    temp = *(s16 *)(value + scratch);
    value = 0x7FFF;
    if (temp == value) {
        movement = *(u16 *)table;
        index = 0;
    }
    value = index + 1;
    *(u8 *)indexPointer = value;
    value = *(u16 *)(sprite + 8);
    value += movement;
    *(u16 *)(sprite + 8) = value;

    temp = *(u16 *)sprite;
    value = 0x40;
    value &= temp;
    if (value != 0) {
        temp = work0;
        value = *(u8 *)temp;
        if (value != 0) {
            value = *(u16 *)(sprite + 10);
            value += 4;
        } else {
            value = *(u16 *)(sprite + 10);
            value += 2;
        }
        *(u16 *)(sprite + 10) = value;
        value = *(u16 *)(sprite + 8);
        value -= 32;
        value = (u16)value;
        temp = *(u16 *)(sprite + 10);
        temp += 32;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        value = gSpriteCollisionTileType;
        if (value == 17) {
            temp = *(u16 *)sprite;
            value = 0xFFBF;
            value &= temp;
            *(u16 *)sprite = value;
        }
    } else {
        scratch = work0;
        value = *(u8 *)scratch;
        if (value != 0) {
            value = *(u16 *)(sprite + 10);
            value -= 4;
        } else {
            value = *(u16 *)(sprite + 10);
            value -= 2;
        }
        *(u16 *)(sprite + 10) = value;
        value = *(u16 *)(sprite + 8);
        value -= 32;
        value = (u16)value;
        temp = *(u16 *)(sprite + 10);
        temp -= 32;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        value = gSpriteCollisionTileType;
        if (value == 17) {
            temp = *(u16 *)sprite;
            value = 0x40;
            value |= temp;
            *(u16 *)sprite = value;
        }
    }
}

void ResetGoldenDivaBlackChickAnimation(void)
{
    u32 value;
    register u32 zero PIN("r1");
    register u32 sprite PIN("r2");
    register u32 constant PIN("r3");

    sprite = (u32)&gCurrentSprite;
    value = (u32)sGoldenDivaBlackChickIdleOam;
    *(u32 *)(sprite + 4) = value;
    value = 0;
    ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = value;
    zero = 0;
    ((struct PrimarySpriteData *)sprite)->animationTimer = value;
    *(u8 *)(sprite + 29) = zero;
    value = sprite;
    value += 42;
    *(u8 *)value = zero;
    value -= 2;
    *(u8 *)value = zero;
    value--;
    *(u8 *)value = zero;
    ((struct PrimarySpriteData *)sprite)->drawPriority = zero;
    zero = *(u16 *)sprite;
    value = 0xFEFF;
    value &= zero;
    constant = 0x280;
    ASM_BARRIER(: "+r"(constant));
    zero = constant;
    ASM_BARRIER(: "+r"(zero));
    value |= zero;
    *(u16 *)sprite = value;
}

void MoveGoldenDivaBlackChickRight(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 32;
    sprite->work2 = 8;
    ResetGoldenDivaBlackChickAnimation();
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
}

void MoveGoldenDivaBlackChickLeft(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 34;
    sprite->work2 = 8;
    ResetGoldenDivaBlackChickAnimation();
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
}

void BeginGoldenDivaBlackChickAttackRight(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.pose = 32;
    gCurrentSprite.work2 = 4;
    ResetGoldenDivaBlackChickAnimation();
}

void BeginGoldenDivaBlackChickAttackLeft(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.pose = 34;
    gCurrentSprite.work2 = 4;
    ResetGoldenDivaBlackChickAnimation();
}

void BeginGoldenDivaBlackChickCharge(void)
{
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackChickAttackOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 32;
    gCurrentSprite.work1 = 60;
}

void BeginGoldenDivaBlackChickGroundRight(void)
{
    gCurrentSprite.status |= 0x40;
    gCurrentSprite.pose = 36;
    BeginGoldenDivaBlackChickCharge();
}

void UpdateGoldenDivaBlackChickGroundRight(void)
{
    u32 value;
    register u32 temp PIN("r1");
    u32 work0;
    u32 sprite;

    temp = (u32)&gCurrentSprite;
    value = ((struct PrimarySpriteData *)temp)->xPosition;
    value += 4;
    ((struct PrimarySpriteData *)temp)->xPosition = value;
    work0 = temp;
    work0 += 39;
    value = *(u8 *)work0;
    sprite = temp;
    if (value != 0) {
        value--;
        *(u8 *)work0 = value;
        value <<= 24;
        temp = value >> 24;
        if (temp == 0) {
            value = (u32)sGoldenDivaBlackChickIdleOam;
            *(u32 *)(sprite + 4) = value;
            value = 0;
            *(u8 *)(sprite + 22) = value;
            *(u16 *)(sprite + 20) = temp;
        }
    } else {
        value = *(u8 *)(sprite + 22);
        value++;
        *(u8 *)(sprite + 22) = value;
    }

    value = *(u16 *)(sprite + 8);
    value -= 32;
    value = (u16)value;
    temp = *(u16 *)(sprite + 10);
    temp += 32;
    temp = (u16)temp;
    /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
    ((void (*)(u32, u16))func_8023BFC)(value, temp);
    value = gSpriteCollisionTileType;
    if (value == 17) {
        temp = *(u16 *)sprite;
        value = 0xFFBF;
        value &= temp;
        temp = 0;
        work0 = 0;
        *(u16 *)sprite = value;
        value = 16;
        *(u8 *)(sprite + 28) = value;
        value = (u32)sGoldenDivaBlackChickIdleOam;
        *(u32 *)(sprite + 4) = value;
        *(u8 *)(sprite + 22) = temp;
        *(u16 *)(sprite + 20) = work0;
    }
}

void BeginGoldenDivaBlackChickGroundLeft(void)
{
    gCurrentSprite.status &= 0xFFBF;
    gCurrentSprite.pose = 38;
    BeginGoldenDivaBlackChickCharge();
}

void UpdateGoldenDivaBlackChickGroundLeft(void)
{
    u32 value;
    register u32 temp PIN("r1");
    u32 work0;
    register u32 zero PIN("r3");
    u32 sprite;

    temp = (u32)&gCurrentSprite;
    value = ((struct PrimarySpriteData *)temp)->xPosition;
    value -= 4;
    ((struct PrimarySpriteData *)temp)->xPosition = value;
    work0 = temp;
    work0 += 39;
    value = *(u8 *)work0;
    sprite = temp;
    if (value != 0) {
        value--;
        *(u8 *)work0 = value;
        value <<= 24;
        temp = value >> 24;
        if (temp == 0) {
            value = (u32)sGoldenDivaBlackChickIdleOam;
            *(u32 *)(sprite + 4) = value;
            value = 0;
            *(u8 *)(sprite + 22) = value;
            *(u16 *)(sprite + 20) = temp;
        }
    } else {
        value = *(u8 *)(sprite + 22);
        value++;
        *(u8 *)(sprite + 22) = value;
    }

    value = *(u16 *)(sprite + 8);
    value -= 32;
    value = (u16)value;
    temp = *(u16 *)(sprite + 10);
    temp -= 32;
    temp = (u16)temp;
    /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
    ((void (*)(u32, u16))func_8023BFC)(value, temp);
    value = gSpriteCollisionTileType;
    if (value == 17) {
        temp = *(u16 *)sprite;
        value = 64;
        work0 = 0;
        zero = 0;
        value |= temp;
        *(u16 *)sprite = value;
        value = 16;
        *(u8 *)(sprite + 28) = value;
        value = (u32)sGoldenDivaBlackChickIdleOam;
        *(u32 *)(sprite + 4) = value;
        *(u8 *)(sprite + 22) = work0;
        *(u16 *)(sprite + 20) = zero;
    }
}

void BeginGoldenDivaBlackChickPose110(void)
{
    GP_SET(gCurrentSprite.pOamData, sGoldenDivaBlackChickPose110Oam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 1;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work0 = 140;
    SpriteUtilTurnTowardWario();
}

void UpdateGoldenDivaBlackChickPose111(void)
{
    u32 value;
    u32 temp;
    u32 sprite;
    register u32 work0 PIN("r3");
    u32 zero;

    zero = (u32)&gCurrentSprite;
    temp = *(u16 *)zero;
    value = 64;
    value &= temp;
    if (value != 0) {
        temp = ((struct PrimarySpriteData *)zero)->xPosition;
        value = temp + 4;
        ((struct PrimarySpriteData *)zero)->xPosition = value;
        value = ((struct PrimarySpriteData *)zero)->yPosition;
        value -= 32;
        value = (u16)value;
        temp += 36;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        value = gSpriteCollisionTileType;
        if (value == 17) {
            temp = *(u16 *)zero;
            value = 0xFFBF;
            value &= temp;
            *(u16 *)zero = value;
        }
    } else {
        temp = *(u16 *)(zero + 10);
        value = temp - 4;
        *(u16 *)(zero + 10) = value;
        value = *(u16 *)(zero + 8);
        value -= 32;
        value = (u16)value;
        temp -= 36;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        value = gSpriteCollisionTileType;
        if (value == 17) {
            value = *(u16 *)zero;
            temp = 64;
            value |= temp;
            *(u16 *)zero = value;
        }
    }

    sprite = (u32)&gCurrentSprite;
    work0 = sprite;
    work0 += 39;
    value = *(u8 *)work0;
    temp = value - 1;
    zero = 0;
    *(u8 *)work0 = temp;
    value = temp << 24;
    if (value == 0)
        goto timerExpired;
    value >>= 24;
    if (value == 130) {
        value = 70;
        ((struct PrimarySpriteData *)sprite)->warioCollision = value;
        goto end;
    }
    if (value == 80) {
        value = (u32)sGoldenDivaBlackChickPose111Oam;
        goto setOam;
    }
    if (value != 40)
        goto end;
    value = (u32)sGoldenDivaBlackChickPose111AltOam;
setOam:
    *(u32 *)(sprite + 4) = value;
    ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = zero;
    value = 1;
    ((struct PrimarySpriteData *)sprite)->animationTimer = value;
    goto end;
timerExpired:
    value = 112;
    ((struct PrimarySpriteData *)sprite)->pose = value;
end:
    return;
}

void BeginGoldenDivaBlackChickPose112(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 zero PIN("r2");
    register u32 zero16 PIN("r3");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    temp = *(u16 *)value;
    zero = 128;
    zero <<= 3;
    ASM_BARRIER(: "+r"(zero));
    value = zero;
    zero = 0;
    zero16 = 0;
    value |= temp;
    temp = sprite;
    *(u16 *)temp = value;
    value = (u32)sGoldenDivaBlackChickFinalOam;
    *(u32 *)(temp + 4) = value;
    *(u8 *)(temp + 22) = zero;
    *(u16 *)(temp + 20) = zero16;
    temp += 39;
    value = 22;
    *(u8 *)temp = value;
    value = 69;
    zero = sprite;
    ((struct PrimarySpriteData *)zero)->warioCollision = value;
    value = sprite;
    value += 35;
    temp = 32;
    *(u8 *)value = temp;
    value++;
    *(volatile u8 *)value = temp;
    temp = sprite;
    temp += 37;
    value = 48;
    *(u8 *)temp = value;
    temp++;
    *(volatile u8 *)temp = value;
    value = 113;
    ((struct PrimarySpriteData *)zero)->pose = value;
    value = 1;
    ((struct PrimarySpriteData *)zero)->drawPriority = value;
    value = ((struct PrimarySpriteData *)zero)->yPosition;
    value -= 28;
    ((struct PrimarySpriteData *)zero)->yPosition = value;
    value = 211;
    m4aSongNumStart(value);
}

void UpdateGoldenDivaBlackChickPose113(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value--;
    *work0 = value;
    value = (u8)value;
    if (value == 0)
        sprite->status = value;
}

void InitGoldenDivaHammer(void)
{
    u32 value;
    u32 temp;
    register u32 zero PIN("r2");
    register u32 constant PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    value = *(u16 *)value;
    constant = 32;
    zero = 0;
    zero16 = 0;
    value |= constant;
    temp = 0xFFFB;
    value &= temp;
    temp = sprite;
    *(u16 *)temp = value;
    value = 2;
    *(u8 *)(temp + 30) = value;
    value = sprite;
    value += 32;
    *(u8 *)value = constant;
    value++;
    *(u8 *)value = zero;
    temp += 34;
    value = 24;
    *(u8 *)temp = value;
    temp++;
    value = 80;
    *(u8 *)temp = value;
    value = sprite;
    value += 36;
    *(u8 *)value = zero;
    temp += 2;
    value = 40;
    *(u8 *)temp = value;
    temp++;
    value = 36;
    *(volatile u8 *)temp = value;
    value = (u32)sGoldenDivaHammerIdleOam;
    temp = sprite;
    *(u32 *)(temp + 4) = value;
    *(u8 *)(temp + 22) = zero;
    ((struct PrimarySpriteData *)temp)->animationTimer = zero16;
    value = 111;
    ((struct PrimarySpriteData *)temp)->pose = value;
    value = sprite;
    value += 42;
    *(u8 *)value = zero;
    value = 240;
    ((struct PrimarySpriteData *)temp)->health = value;
}


void UpdateGoldenDivaHammerOrbit(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work3;
    u32 index;
    s32 scale;
    s32 cosValue;
    s32 sinValue;
    s32 inverse;
    s32 motion;
    struct PrimarySpriteData *current;

    sprite = &gCurrentSprite;
    work3 = &sprite->work3;
    index = *work3;

    if (index <= 15) {
        scale = index;
        do {
            scale++;
            scale <<= 4;
            sprite->status |= 128;

            cosValue = sSinCosTable[64];
            inverse = FixedInverse(scale);
            sprite->affinePA = FixedMul(cosValue, inverse);

            sinValue = sSinCosTable[0];
            inverse = FixedInverse(scale);
            sprite->affinePB = FixedMul(sinValue, inverse);

            sinValue = -sinValue;
            inverse = FixedInverse(scale);
            sprite->affinePC = FixedMul(sinValue, inverse);

            inverse = FixedInverse(scale);
            sprite->affinePD = FixedMul(cosValue, inverse);
        } while (0);
    }

    {
        u32 lookupTable;
        u32 lookupIndex;
        u32 entryAddress;
        u32 lookupZero;
        s32 entryValue;

        lookupTable = (u32)sGoldenDivaHammerOrbitMotion;
        lookupIndex = index;
        entryAddress = lookupIndex << 1;
        entryAddress += lookupTable;
        motion = *(u16 *)entryAddress;
        lookupZero = 0;
        entryValue = *(s16 *)(entryAddress + lookupZero);

        if (entryValue == 0x7FFF) {
            entryAddress = index;
            entryAddress--;
            entryAddress <<= 1;
            entryAddress += lookupTable;
            motion = *(u16 *)entryAddress;
        } else {
            u32 nextIndex;
            u8 *work3Pointer;

            nextIndex = index;
            nextIndex++;
            work3Pointer = work3;
            *work3Pointer = nextIndex;
        }
    }

    sprite->yPosition += motion;
    current = &gCurrentSprite;

    {
        u32 checkedY;

        checkedY = func_8023A60(current->yPosition, current->xPosition);
        if (gSpriteCollisionResult != 0) {
            current->yPosition = checkedY;
            ScreenShakeRequestY(48, 129);
            m4aSongNumStart(62);

            if (current->status & 64) {
                current->pose = 51;
            } else {
                current->pose = 53;
            }
        } else {
            motion = (u16)((s32)(motion << 16) >> 17);

            if (current->status & 64) {
                u32 y;
                u32 x;

                x = current->xPosition;
                x += motion;
                current->xPosition = x;
                y = (u16)(current->yPosition - 32);
                x += current->hitboxExtentRight;
                x = (u16)x;
                ((void (*)(u32, u16))func_8023BFC)(y, x);
            } else {
                u32 y;
                u32 x;

                x = current->xPosition;
                x -= motion;
                current->xPosition = x;
                y = (u16)(current->yPosition - 32);
                x -= current->hitboxExtentLeft;
                x = (u16)x;
                ((void (*)(u32, u16))func_8023BFC)(y, x);
            }

            if (gSpriteCollisionTileType & 15) {
                gCurrentSprite.status ^= 64;
            }
        }
    }
}

void BeginGoldenDivaHammerFall(void)
{
    gCurrentSprite.pose = 16;
    gCurrentSprite.status &= 0xFCFF;
}

void UpdateGoldenDivaHammerFall(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0)
        gCurrentSprite.pose = 27;
}

void BeginGoldenDivaHammerSwing(void)
{
    u32 value;
    u32 temp;
    register u32 sprite PIN("r2");
    u32 y;

    sprite = (u32)&gCurrentSprite;
    temp = 0;
    ((struct PrimarySpriteData *)sprite)->disableWarioCollisionTimer = temp;
    value = 115;
    ((struct PrimarySpriteData *)sprite)->pose = value;
    value = (u32)sGoldenDivaHammerActiveOam;
    *(u32 *)(sprite + 4) = value;
    ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = temp;
    ((struct PrimarySpriteData *)sprite)->animationTimer = temp;
    temp = sprite;
    temp += 39;
    value = 10;
    *(u8 *)temp = value;
    temp -= 4;
    value = 128;
    *(u8 *)temp = value;
    temp++;
    value = 16;
    *(u8 *)temp = value;
    temp++;
    value = 68;
    *(u8 *)temp = value;
    temp++;
    *(volatile u8 *)temp = value;
    value = 69;
    ((struct PrimarySpriteData *)sprite)->warioCollision = value;
    y = ((struct PrimarySpriteData *)sprite)->yPosition;
    y -= 48;
    value = ((struct PrimarySpriteData *)sprite)->xPosition;
    SpriteSpawnAsChild(11, 0, 0, y, value);
    m4aSongNumStart(129);
}

void UpdateGoldenDivaHammerSwing(void)
{
    u32 value;
    u32 temp;
    register u32 sprite PIN("r2");

    temp = (u32)&gCurrentSprite;
    sprite = temp;
    sprite += 39;
    value = *(u8 *)sprite;
    value--;
    *(u8 *)sprite = value;
    value = *(u8 *)sprite;
    sprite = temp;
    switch (value) {
        case 9:
            value = *(u16 *)(sprite + 8);
            value -= 72;
            temp = *(u16 *)(sprite + 10);
            temp += 32;
            SpawnSecondarySprite(value, temp, 6);
            break;
        case 8:
            value = *(u16 *)(sprite + 8);
            value -= 16;
            temp = *(u16 *)(sprite + 10);
            temp -= 32;
            SpawnSecondarySprite(value, temp, 6);
            break;
        case 7:
            value = *(u16 *)(sprite + 8);
            value -= 16;
            temp = *(u16 *)(sprite + 10);
            temp += 32;
            SpawnSecondarySprite(value, temp, 6);
            break;
        case 6:
            value = *(u16 *)(sprite + 8);
            value -= 72;
            temp = *(u16 *)(sprite + 10);
            temp -= 32;
            SpawnSecondarySprite(value, temp, 6);
            break;
        case 0:
            value = 0;
            *(u16 *)sprite = value;
            break;
    }
}

void PrepareGoldenDivaHammerMovement(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaHammerGroundRight(void)
{
    gCurrentSprite.pose = 44;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaHammerGroundLeft(void)
{
    gCurrentSprite.pose = 46;
    gCurrentSprite.work1 = 0;
}

void BeginGoldenDivaHammerSlopeRight(void)
{
    gCurrentSprite.pose = 72;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void BeginGoldenDivaHammerSlopeLeft(void)
{
    gCurrentSprite.pose = 74;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void ResetGoldenDivaHammerMotion(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.health <= 48)
        gCurrentSprite.pose = 114;
}

void MoveGoldenDivaHammerRight(void)
{
    gCurrentSprite.pose = 52;
    ResetGoldenDivaHammerMotion();
}

void MoveGoldenDivaHammerLeft(void)
{
    gCurrentSprite.pose = 54;
    ResetGoldenDivaHammerMotion();
}

void UpdateGoldenDivaHammerLanding(void)
{
    if (gCurrentSprite.xPosition > gWarioData.xPosition)
        MoveGoldenDivaHammerRight();
    else
        MoveGoldenDivaHammerLeft();
}

void BeginGoldenDivaHammerSpin(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 constant PIN("r2");
    u32 sprite;
    u32 zero16;

    sprite = (u32)&gCurrentSprite;
    temp = sprite;
    temp += 42;
    value = 0;
    *(u8 *)temp = value;
    value = ((struct PrimarySpriteData *)sprite)->health;
    if (value <= 48)
        goto setPose;
    temp = *(u16 *)sprite;
    constant = 128;
    constant <<= 1;
    value = constant;
    value &= temp;
    zero16 = (u16)value;
    if (zero16 == 0)
        goto initialize;

setPose:
    value = 114;
    ((struct PrimarySpriteData *)sprite)->pose = value;
    goto end;

initialize:
    value = constant;
    value |= temp;
    constant = 0;
    temp = 0xFDFF;
    value &= temp;
    *(u16 *)sprite = value;
    value = (u32)sGoldenDivaHammerActiveOam;
    *(u32 *)(sprite + 4) = value;
    *(u8 *)(sprite + 22) = constant;
    *(u16 *)(sprite + 20) = zero16;
    value = sprite;
    value += 39;
    *(u8 *)value = constant;
    temp = sprite;
    temp += 41;
    value = 40;
    *(u8 *)temp = value;
    m4aSongNumStart(199);
end:
    return;
}

void MoveGoldenDivaHammerOffscreenRight(void)
{
    gCurrentSprite.status &= 0xFFBF;
    gCurrentSprite.pose = 32;
    BeginGoldenDivaHammerSpin();
}

void MoveGoldenDivaHammerOffscreenLeft(void)
{
    gCurrentSprite.status |= 0x40;
    gCurrentSprite.pose = 34;
    BeginGoldenDivaHammerSpin();
}

void UpdateGoldenDivaHammerAffine(void)
{
    u32 value;
    u32 temp;
    register u32 offset PIN("r2");
    s32 trig;
    register s32 scale PIN("r5");
    register u32 table PIN("r6");
    u32 sprite;
    register u32 work0 PIN("r8");

    sprite = (u32)&gCurrentSprite;
    temp = *(u16 *)sprite;
    value = 0x80;
    value |= temp;
    *(u16 *)sprite = value;
    table = (u32)sSinCosTable;
    value = 39;
    ASM_BARRIER(: "+r"(value));
    value += sprite;
    work0 = value;
    value = *(u8 *)value;
    value += 64;
    value <<= 1;
    value += table;
    temp = 0;
    trig = *(s16 *)(value + temp);
    scale = 0x100;
    value = FixedInverse(scale);
    temp = value;
    value = trig;
    value = FixedMul(value, temp);
    *(u16 *)(sprite + 12) = value;

    temp = work0;
    value = *(u8 *)temp;
    value <<= 1;
    value += table;
    temp = 0;
    trig = *(s16 *)(value + temp);
    value = FixedInverse(scale);
    temp = value;
    value = trig;
    value = FixedMul(value, temp);
    *(u16 *)(sprite + 14) = value;

    temp = work0;
    value = *(u8 *)temp;
    value <<= 1;
    value += table;
    temp = 0;
    trig = *(s16 *)(value + temp);
    trig = -trig;
    value = FixedInverse(scale);
    temp = value;
    value = trig;
    value = FixedMul(value, temp);
    *(u16 *)(sprite + 16) = value;

    temp = work0;
    value = *(u8 *)temp;
    value += 64;
    value <<= 1;
    value += table;
    temp = 0;
    trig = *(s16 *)(value + temp);
    value = FixedInverse(scale);
    temp = value;
    value = trig;
    value = FixedMul(value, temp);
    *(u16 *)(sprite + 18) = value;

    temp = sprite;
    temp += 41;
    value = *(u8 *)temp;
    if (value > 24) {
        value--;
        *(u8 *)temp = value;
    }
    value = *(u8 *)temp;
    scale = value >> 1;
    trig = sprite;
    trig += 42;
    value = *(u8 *)trig;
    if (value == 0) {
        value = *(u16 *)(sprite + 8);
        value -= scale;
        *(u16 *)(sprite + 8) = value;
        temp = sprite;
        temp += 35;
        temp = *(u8 *)temp;
        value -= temp;
        value = (u16)value;
        temp = *(u16 *)(sprite + 10);
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        value = gSpriteCollisionTileType;
        temp = value;
        value = 15;
        value &= temp;
        if (value != 0) {
            value = 1;
            *(u8 *)trig = value;
            return;
        }
    } else {
        value = *(u16 *)(sprite + 8);
        value += scale;
        *(u16 *)(sprite + 8) = value;
        value = *(u16 *)(sprite + 8);
        temp = *(u16 *)(sprite + 10);
        value = func_8023A60(value, temp);
        temp = value;
        value = gSpriteCollisionResult;
        if (value != 0) {
            *(u16 *)(sprite + 8) = temp;
            value = 107;
            *(u8 *)(sprite + 28) = value;
            return;
        }
    }

    trig = (u32)&gCurrentSprite;
    temp = *(u16 *)trig;
    value = 0x40;
    value &= temp;
    if (value != 0) {
        temp = ((struct PrimarySpriteData *)trig)->xPosition;
        temp -= scale;
        ((struct PrimarySpriteData *)trig)->xPosition = temp;
        value = ((struct PrimarySpriteData *)trig)->yPosition;
        value -= 32;
        value = (u16)value;
        offset = trig;
        offset += 37;
        offset = *(u8 *)offset;
        temp -= offset;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        temp = trig;
        temp += 39;
        value = *(u8 *)temp;
        value -= 8;
    } else {
        temp = ((struct PrimarySpriteData *)trig)->xPosition;
        temp += scale;
        ((struct PrimarySpriteData *)trig)->xPosition = temp;
        value = ((struct PrimarySpriteData *)trig)->yPosition;
        value -= 32;
        value = (u16)value;
        offset = trig;
        offset += 38;
        offset = *(u8 *)offset;
        temp += offset;
        temp = (u16)temp;
        /* Compiler boundary: the u16 prototype would truncate the live u32 Y value before the direct call. */
        ((void (*)(u32, u16))func_8023BFC)(value, temp);
        temp = trig;
        temp += 39;
        value = *(u8 *)temp;
        value += 8;
    }
    *(u8 *)temp = value;
    value = gSpriteCollisionTileType;
    temp = value;
    value = 15;
    value &= temp;
    if (value != 0) {
        value = (u32)&gCurrentSprite;
        temp = *(u16 *)value;
        offset = 0x40;
        temp ^= offset;
        *(u16 *)value = temp;
    }
}

void InitGoldenDivaFace(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 zero16 PIN("r2");
    u32 sprite;
    register u32 zero8 PIN("r4");
    u32 status;

    sprite = (u32)&gCurrentSprite;
    value = *(u16 *)sprite;
    status = 0xFFFB;
    status &= value;
    zero8 = 0;
    zero16 = 0;
    *(u16 *)sprite = status;
    *(u8 *)(sprite + 30) = zero8;
    temp = sprite;
    temp += 32;
    value = 16;
    *(u8 *)temp = value;
    value = sprite;
    value += 33;
    *(u8 *)value = zero8;
    temp += 2;
    value = 8;
    *(u8 *)temp = value;
    temp++;
    value = 48;
    *(u8 *)temp = value;
    value = sprite;
    value += 36;
    *(u8 *)value = zero8;
    temp += 2;
    value = 36;
    *(u8 *)temp = value;
    temp++;
    value = 32;
    *(u8 *)temp = value;
    value = (u32)sGoldenDivaFaceIdleOam;
    *(u32 *)(sprite + 4) = value;
    *(u8 *)(sprite + 22) = zero8;
    *(u16 *)(sprite + 20) = zero16;
    value = 28;
    *(u8 *)(sprite + 28) = value;
    temp++;
    value = 10;
    *(volatile u8 *)temp = value;
    temp = (u32)&gWarioData;
    value = *(u16 *)(sprite + 10);
    temp = *(u16 *)(temp + 18);
    if (value < temp) {
        value = 0xFFBF;
        value &= status;
    } else {
        temp = 64;
        value = status;
        value |= temp;
    }
    *(u16 *)sprite = value;
}

void BeginGoldenDivaFaceFall(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work0;
    u32 value;

    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    value = *work0;
    value--;
    *work0 = value;
    value <<= 24;
    if (value == 0) {
        sprite->pose = 16;
        *work0 = 30;
    }
}

void UpdateGoldenDivaFaceFall(void)
{
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *sprite2 PIN("r5");
    u8 *work0;
    u32 value;
    u32 temp;

    value = (u32)&gCurrentSprite;
    work0 = (u8 *)value;
    work0 += 39;
    temp = *work0;
    sprite = (struct PrimarySpriteData *)value;
    if (temp != 0) {
        value = temp - 1;
        *work0 = value;
        temp = (u8)value;
        if (temp == 0) {
            sprite->warioCollision = 1;
            sprite->palette = temp;
        }
    }

    sprite2 = sprite;
    if (sprite2->status & 0x40) {
        temp = sprite2->xPosition;
        value = temp + 4;
        sprite2->xPosition = value;
        func_8023BFC(sprite2->yPosition - 32, temp + 36);
        if (gSpriteCollisionTileType == 17)
            sprite2->status &= 0xFFBF;
    } else {
        temp = sprite->xPosition;
        value = temp - 4;
        sprite->xPosition = value;
        func_8023BFC(sprite->yPosition - 32, temp - 36);
        if (gSpriteCollisionTileType == 17)
            sprite->status |= 0x40;
    }

    if ((gMainTimer & 7) == 0)
        m4aSongNumStart(202);
}

void ResetGoldenDivaFaceMotion(void)
{
    gCuckooCondorHasCapturedWario--;
    SpriteSpawnAsChild(15, 0, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
}

void UpdateGoldenDivaFaceLanding(void)
{
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    ResetGoldenDivaFaceMotion();
}

void MoveGoldenDivaFaceRight(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 32;
    UpdateGoldenDivaFaceLanding();
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition - 32, 6);
}

void MoveGoldenDivaFaceLeft(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 34;
    UpdateGoldenDivaFaceLanding();
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition + 32, 6);
}

void BeginGoldenDivaFaceGroundRight(void)
{
    gCurrentSprite.status |= 0x40;
    gCurrentSprite.pose = 36;
}

void UpdateGoldenDivaFaceGroundRight(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->xPosition += 8;
    if ((gMainTimer & 3) == 0)
        m4aSongNumStart(202);
    func_8023BFC(sprite->yPosition - 32, sprite->xPosition + 32);
    if (gSpriteCollisionTileType == 17) {
        sprite->status &= 0xFFBF;
        sprite->pose = 16;
    }
}

void BeginGoldenDivaFaceGroundLeft(void)
{
    gCurrentSprite.status &= 0xFFBF;
    gCurrentSprite.pose = 38;
}

void UpdateGoldenDivaFaceGroundLeft(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->xPosition -= 8;
    if ((gMainTimer & 3) == 0)
        m4aSongNumStart(202);
    func_8023BFC(sprite->yPosition - 32, sprite->xPosition - 32);
    if (gSpriteCollisionTileType == 17) {
        sprite->status |= 0x40;
        sprite->pose = 16;
    }
}

void InitGoldenDivaDebris(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 constant PIN("r2");
    register u32 zero8 PIN("r3");
    register u32 zero16 PIN("r4");
    register u32 sprite PIN("r12");

    value = (u32)&gCurrentSprite;
    sprite = value;
    temp = *(u16 *)value;
    constant = 144;
    constant <<= 3;
    ASM_BARRIER(: "+r"(constant));
    value = constant;
    ASM_BARRIER(: "+r"(value));
    zero8 = 0;
    zero16 = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = sprite;
    *(u16 *)temp = value;
    *(u8 *)(temp + 30) = zero8;
    value = sprite;
    value += 32;
    constant = 16;
    *(u8 *)value = constant;
    value++;
    *(u8 *)value = constant;
    value++;
    *(u8 *)value = constant;
    value++;
    temp = 4;
    *(u8 *)value = temp;
    value++;
    *(u8 *)value = temp;
    value++;
    *(u8 *)value = temp;
    value++;
    *(volatile u8 *)value = temp;
    value = (u32)sGoldenDivaDebrisOam;
    temp = sprite;
    *(u32 *)(temp + 4) = value;
    *(u8 *)(temp + 22) = zero8;
    ((struct PrimarySpriteData *)temp)->animationTimer = zero16;
    value = 15;
    ((struct PrimarySpriteData *)temp)->pose = value;
    value = ((struct PrimarySpriteData *)temp)->roomSlot;
    value <<= 4;
    value = ~value;
    temp += 39;
    *(u8 *)temp = value;
    temp++;
    value = 64;
    *(volatile u8 *)temp = value;
    value = sprite;
    value += 41;
    *(u8 *)value = constant;
    value++;
    *(u8 *)value = zero8;
}

void BeginGoldenDivaDebrisShrink(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work1;
    s32 value;
    s32 current;

    sprite = &gCurrentSprite;
    work1 = &sprite->work1;
    current = *work1;
    value = sprite->roomSlot;
    value <<= 4;
    value += 64;
    if (current < value) {
        value = current + 16;
        *work1 = value;
    } else {
        sprite->pose = 16;
    }
}

void UpdateGoldenDivaDebrisShrink(void)
{
    u32 value;
    u32 sprite;
    u32 pointer;
    u32 work2;
    register u32 mask PIN("r4");
    u32 work3;
    register u32 current PIN("ip");

    value = (u8)gBossState;
    if (value == 4) {
        sprite = (u32)&gCurrentSprite;
        pointer = sprite;
        pointer += 39;
        value = *(u8 *)pointer;
        value--;
        *(u8 *)pointer = value;
        mask = 255;
        value = (u8)value;
        current = sprite;
        if (value != 0) {
            work3 = sprite;
            work3 += 42;
            value = *(u8 *)work3;
            if (value != 0) {
                work2 = sprite;
                work2 += 41;
                value = *(u8 *)work2;
                value--;
                *(u8 *)work2 = value;
                pointer = mask;
                pointer &= value;
                if (pointer != 0) {
                    sprite += 40;
                    value = *(u8 *)sprite;
                    value += 4;
                    *(u8 *)sprite = value;
                } else {
                    value = 16;
                    *(u8 *)work2 = value;
                    *(u8 *)work3 = pointer;
                }
            } else {
                pointer = sprite;
                pointer += 41;
                value = *(u8 *)pointer;
                value--;
                *(u8 *)pointer = value;
                value &= mask;
                if (value != 0) {
                    sprite += 40;
                    value = *(u8 *)sprite;
                    value -= 4;
                    *(u8 *)sprite = value;
                } else {
                    value = 16;
                    *(u8 *)pointer = value;
                    value = 1;
                    *(u8 *)work3 = value;
                }
            }

            value = current;
            value += 39;
            value = *(u8 *)value;
            sprite = 255;
            sprite -= value;
            sprite <<= 24;
            value = 240;
            value <<= 20;
            value &= sprite;
            value >>= 24;
            sprite = current;
            sprite = *(u8 *)(sprite + 24);
            if (value == sprite) {
                value = 2;
                pointer = current;
                *(u8 *)(pointer + 27) = value;
            } else {
                value = 0;
                sprite = current;
                *(u8 *)(sprite + 27) = value;
            }
        } else {
            *(u16 *)sprite = value;
        }
    }
}

void SpriteGoldenDivaSpawner(void)
{
    InitGoldenDivaSpawner();
}

void SpriteGoldenDivaBody(void)
{
    register u32 value PIN("r0");
    u32 sprite;
    u32 multiplier;
    register u32 spriteData PIN("r4");
    register u32 destination PIN("r1");
    register u32 work2Pointer PIN("ip");

    value = gCurrentSprite.pose;
    switch (value) {
        case 0:
            InitGoldenDivaBody();
            return;
        case 116:
            UpdateGoldenDivaShopItemCharge();
        case 117:
            UpdateGoldenDivaShopItemHit();
            break;
        case 118:
            UpdateGoldenDivaBodyPose118();
            break;
        case 119:
            UpdateGoldenDivaBodyPose119();
            break;
        case 110:
            BeginGoldenDivaIntro();
            return;
        case 111:
            UpdateGoldenDivaIntro();
            break;
        case 112:
            FinishGoldenDivaIntro();
            break;
        case 113:
            BeginGoldenDivaFirstPhase();
            break;
        case 114:
            UpdateGoldenDivaFirstPhase();
            break;
        case 120:
            BeginGoldenDivaMaskRecall();
            break;
        case 115:
            UpdateGoldenDivaMaskRecall();
            break;
        case 15:
            BeginGoldenDivaAttackSelection();
            break;
        case 16:
            UpdateGoldenDivaMaskDamage();
            break;
        case 17:
            BeginGoldenDivaFaceAnimation();
        case 18:
            UpdateGoldenDivaFaceAnimation();
            break;
        case 19:
            UpdateGoldenDivaPhaseTransition();
            break;
        case 21:
            BeginGoldenDivaAttack();
            break;
        case 22:
            UpdateGoldenDivaAttack();
            break;
        case 28:
            BeginGoldenDivaHitReaction();
            break;
        case 24:
            UpdateGoldenDivaHitReaction();
            break;
        case 26:
            UpdateGoldenDivaRecovery();
            break;
        case 31:
        case 79:
        case 106:
            MoveGoldenDivaBodyRight();
            goto updateMoving;
        case 33:
        case 80:
        case 105:
            MoveGoldenDivaBodyLeft();
        case 32:
        case 34:
updateMoving:
            UpdateGoldenDivaBodyMovement();
            break;
        case 35:
            BeginGoldenDivaTurnRight();
            goto updateProjectile;
        case 37:
            BeginGoldenDivaTurnLeft();
        case 36:
updateProjectile:
            UpdateGoldenDivaTurn();
            break;
        case 50:
            UpdateGoldenDivaBodyAttackPose();
            break;
        case 109:
            UpdateGoldenDivaOrbitPhase();
            break;
        case 122:
            UpdateGoldenDivaDefeatSequence();
            break;
    }

    value = (u8)gBossState;
    if (value <= 11) {
        spriteData = (u32)gSpriteData;
        sprite = (u32)&gCurrentSprite;
        value = 41;
        ASM_BARRIER(: "+r"(value));
        value += sprite;
        work2Pointer = value;
        value = *(u8 *)value;
        ASM_BARRIER(: "+r"(value));
        multiplier = 44;
        destination = value;
        destination *= multiplier;
        destination += spriteData;
        value = ((struct PrimarySpriteData *)sprite)->yPosition;
        value += 56;
        *(u16 *)(destination + 8) = value;
        destination = work2Pointer;
        value = *(u8 *)destination;
        value *= multiplier;
        value += spriteData;
        destination = ((struct PrimarySpriteData *)sprite)->xPosition;
        *(u16 *)(value + 10) = destination;
    } else if (value != 13 && gInitialHealth == 0 && *(u16 *)&gWarioData == 0x1300 && gCurrentSprite.health != 2) {
        gInitialHealth = 80;
        m4aSongNumStart(188);
    }
}

void SpriteGoldenDivaRobe(void)
{
    u32 value;
    register u32 multiplier PIN("r1");
    register u32 spriteData PIN("r2");
    u32 work3;
    u32 sprite;

    sprite = (u32)&gCurrentSprite;
    value = ((struct PrimarySpriteData *)sprite)->pose;
    if (value == 0)
        InitGoldenDivaRobe();

    spriteData = (u32)gSpriteData;
    work3 = sprite;
    work3 += 42;
    value = *(u8 *)work3;
    multiplier = 44;
    value *= multiplier;
    value += spriteData;
    value = *(u16 *)(value + 8);
    value += 192;
    ((struct PrimarySpriteData *)sprite)->yPosition = value;
    value = *(u8 *)work3;
    value *= multiplier;
    value += spriteData;
    value = *(u16 *)(value + 10);
    ((struct PrimarySpriteData *)sprite)->xPosition = value;
}

void SpriteGoldenDivaLowerBody(void)
{
    u32 value;
    register u32 sprite PIN("r1");
    register u32 multiplier PIN("r2");
    register u32 spriteData PIN("r3");
    register u32 work3Copy PIN("r4");
    register u32 work3 PIN("r12");

    value = (u32)&gCurrentSprite;
    value = ((struct PrimarySpriteData *)value)->pose;
    switch (value) {
        case 0:
            InitGoldenDivaLowerBody();
            return;
        case 1:
            UpdateGoldenDivaLowerBody();
            break;
    }

    sprite = (u32)&gCurrentSprite;
    spriteData = (u32)gSpriteData;
    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += sprite;
    work3 = value;
    value = *(u8 *)value;
    multiplier = 44;
    value *= multiplier;
    value += spriteData;
    value = ((struct PrimarySpriteData *)value)->yPosition;
    value += 192;
    ((struct PrimarySpriteData *)sprite)->yPosition = value;
    work3Copy = work3;
    value = *(u8 *)work3Copy;
    value *= multiplier;
    value += spriteData;
    value = *(u16 *)(value + 10);
    ((struct PrimarySpriteData *)sprite)->xPosition = value;
    value = (u8)gBossState;
    value -= 6;
    value = (u8)value;
    if (value <= 5)
        *(u16 *)0x04000052 = 0x090B;
}

void SpriteGoldenDivaFan(void)
{
    s32 value;
    u32 sprite;
    register u32 multiplier PIN("r2");
    register u32 spriteData PIN("r3");
    register u32 work3Copy PIN("r4");
    register u32 work3 PIN("r12");

    sprite = (u32)&gCurrentSprite;
    value = 1;
    ((struct PrimarySpriteData *)sprite)->disableWarioCollisionTimer = value;
    value = ((struct PrimarySpriteData *)sprite)->pose;
    switch (value) {
        case 0:
            InitGoldenDivaFan();
            return;
        case 1:
            FollowGoldenDivaFan();
            break;
        case 17:
            BreakGoldenDivaFan();
            break;
    }

    sprite = (u32)&gCurrentSprite;
    spriteData = (u32)gSpriteData;
    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += sprite;
    work3 = value;
    value = *(u8 *)value;
    multiplier = 44;
    value *= multiplier;
    value += spriteData;
    value = ((struct PrimarySpriteData *)value)->yPosition;
    value += 56;
    ((struct PrimarySpriteData *)sprite)->yPosition = value;
    work3Copy = work3;
    value = *(u8 *)work3Copy;
    value *= multiplier;
    value += spriteData;
    value = *(u16 *)(value + 10);
    ((struct PrimarySpriteData *)sprite)->xPosition = value;
}

void SpriteGoldenDivaFaceHitbox(void)
{
    struct PrimarySpriteData *sprite;
    s32 value;

    sprite = &gCurrentSprite;
    value = sprite->pose;
    switch (value) {
        case 0:
            InitGoldenDivaFaceHitbox();
            break;
        case 16:
            UpdateGoldenDivaFaceHitbox();
            break;
        case 18:
            FollowGoldenDivaFaceHitbox();
            break;
        case 105:
        case 106:
            AdvanceGoldenDivaMaskGraphics();
            SpriteSpawnAsChild(11, 0, 0, sprite->yPosition, sprite->xPosition);
            VoiceSetPlay(1);
        case 51:
        case 53:
            HideGoldenDivaFaceHitbox();
            break;
    }
}

void SpriteGoldenDivaMask(void)
{
    u32 value;
    u32 temp;
    u32 sprite;

    sprite = (u32)&gCurrentSprite;
    temp = *(u16 *)sprite;
    value = 0xFF7F;
    value &= temp;
    *(u16 *)sprite = value;
    value = *(u8 *)(sprite + 28);
    switch (value) {
        case 0:
            InitGoldenDivaMask();
            break;
        case 110:
            BeginGoldenDivaMaskOrbit();
            break;
        case 111:
            UpdateGoldenDivaMaskOrbit();
            break;
        case 116:
            BeginGoldenDivaMaskAttack();
        case 117:
            UpdateGoldenDivaMaskAttack();
            break;
        case 15:
        case 23:
        case 47:
            BeginGoldenDivaMaskFall();
        case 16:
            UpdateGoldenDivaMaskFall();
            break;
        case 17:
            BeginGoldenDivaMaskBounce();
        case 18:
            UpdateGoldenDivaMaskBounce();
            break;
        case 19:
            BeginGoldenDivaMaskGrounded();
        case 20:
            UpdateGoldenDivaMaskGrounded();
            break;
        case 22:
            BeginGoldenDivaMaskThrow();
            break;
        case 27:
        case 29:
            PrepareGoldenDivaMaskMovement();
        case 30:
            func_8023EE0();
            break;
        case 43:
            BeginGoldenDivaMaskGroundRight();
        case 44:
            func_8024478();
            break;
        case 45:
            BeginGoldenDivaMaskGroundLeft();
        case 46:
            func_802449C();
            break;
        case 49:
            UpdateGoldenDivaMaskLanding();
            break;
        case 31:
        case 35:
        case 51:
        case 79:
        case 106:
            MoveGoldenDivaMaskRight();
            goto updateGroundRight;
        case 33:
        case 37:
        case 53:
        case 80:
        case 105:
            MoveGoldenDivaMaskLeft();
            goto updateGroundLeft;
        case 55:
            func_8024688();
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
        case 54:
        case 58:
updateGroundLeft:
            func_802476C();
            break;
        case 59:
            func_80247F0();
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
        case 52:
        case 64:
updateGroundRight:
            func_8024988();
            break;
        case 65:
            func_8024A0C();
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
            goto updateDownRight;
        case 69:
            func_8024BEC();
            goto updateDownLeft;
        case 71:
            BeginGoldenDivaMaskSlopeRight();
        case 68:
        case 72:
updateDownRight:
            func_8024AD4();
            break;
        case 73:
            BeginGoldenDivaMaskSlopeLeft();
        case 70:
        case 74:
updateDownLeft:
            func_8024C00();
            break;
        case 81:
            BeginGoldenDivaMaskLiftRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            BeginGoldenDivaMaskLiftLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            BeginGoldenDivaMaskCarryRight();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            goto finishCarrying;
        case 85:
            BeginGoldenDivaMaskCarryLeft();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
finishCarrying:
            gCurrentSprite.health = 0;
            HandleGoldenDivaMaskFanCollision();
            break;
        case 89:
            temp = 0;
            value = 90;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 90:
            SpriteUtilThrownLeftSoft();
            HandleGoldenDivaMaskFanCollision();
            break;
        case 93:
            value = 94;
            *(u8 *)(sprite + 28) = value;
            value = 1;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaMaskHardThrow();
        case 94:
            SpriteUtilThrownLeftHard();
            goto finishHardThrow;
        case 97:
            temp = 0;
            value = 98;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 98:
            SpriteUtilThrownUpLeftSoft();
            HandleGoldenDivaMaskFanCollision();
            break;
        case 101:
            value = 102;
            *(u8 *)(sprite + 28) = value;
            value = 18;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaMaskHardThrow();
        case 102:
            SpriteUtilThrownUpLeftHard();
            goto finishHardThrow;
        case 91:
            temp = 0;
            value = 92;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 92:
            SpriteUtilThrownRightSoft();
            HandleGoldenDivaMaskFanCollision();
            break;
        case 95:
            value = 96;
            *(u8 *)(sprite + 28) = value;
            value = 2;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaMaskHardThrow();
        case 96:
            SpriteUtilThrownRightHard();
            goto finishHardThrow;
        case 99:
            temp = 0;
            value = 100;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 100:
            SpriteUtilThrownUpRightSoft();
            HandleGoldenDivaMaskFanCollision();
            break;
        case 103:
            value = 104;
            *(u8 *)(sprite + 28) = value;
            value = 17;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaMaskHardThrow();
        case 104:
            SpriteUtilThrownUpRightHard();
finishHardThrow:
            UpdateGoldenDivaMaskHardThrow();
            HandleGoldenDivaMaskFanCollision();
            break;
        default:
            temp = *(u8 *)(sprite + 29);
            value = 0xF0;
            value &= temp;
            if (value != 0) {
                temp = sprite;
                temp += 41;
                value = 4;
            } else {
                temp = sprite;
                temp += 41;
                value = 8;
            }
            *(u8 *)temp = value;
            temp = *(u8 *)(sprite + 29);
            value = 15;
            value &= temp;
            if (value == 2)
                func_8024AC0();
            else
                func_8024BEC();
            break;
    }
}

void SpriteGoldenDivaExplosiveBall(void)
{
    u32 value;
    u32 temp;
    u32 sprite;

    sprite = (u32)&gCurrentSprite;
    value = *(u16 *)sprite;
    temp = 0xFF7F;
    temp &= value;
    *(u16 *)sprite = temp;
    value = 64;
    value &= temp;
    if (value != 0) {
        temp = sprite;
        temp += 37;
        value = 12;
        *(u8 *)temp = value;
        temp++;
        value = 20;
    } else {
        temp = sprite;
        temp += 37;
        value = 24;
        *(u8 *)temp = value;
        temp++;
        value = 8;
    }
    *(u8 *)temp = value;

    value = *(u8 *)(sprite + 28);
    switch (value) {
        case 0:
            InitGoldenDivaExplosiveBall();
            break;
        case 111:
            BeginGoldenDivaExplosiveBallOrbit();
            break;
        case 113:
            UpdateGoldenDivaExplosiveBallOrbit();
            break;
        case 15:
        case 23:
        case 47:
            BeginGoldenDivaExplosiveBallFall();
            break;
        case 16:
            UpdateGoldenDivaExplosiveBallFall();
            break;
        case 17:
            BeginGoldenDivaExplosiveBallBounce();
        case 18:
            UpdateGoldenDivaExplosiveBallBounce();
            break;
        case 27:
        case 29:
            PrepareGoldenDivaExplosiveBallMovement();
        case 30:
            func_8023EE0();
            break;
        case 43:
            BeginGoldenDivaExplosiveBallGroundRight();
        case 44:
            func_8024478();
            break;
        case 45:
            BeginGoldenDivaExplosiveBallGroundLeft();
        case 46:
            func_802449C();
            break;
        case 49:
            UpdateGoldenDivaExplosiveBallLanding();
            break;
        case 31:
        case 35:
        case 51:
        case 79:
        case 106:
            MoveGoldenDivaExplosiveBallRight();
            goto updateGroundRight;
        case 33:
        case 37:
        case 53:
        case 80:
        case 105:
            MoveGoldenDivaExplosiveBallLeft();
            goto updateGroundLeft;
        case 55:
            func_8024688();
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
        case 54:
        case 58:
updateGroundLeft:
            func_802476C();
            break;
        case 59:
            func_80247F0();
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
        case 52:
        case 64:
updateGroundRight:
            func_8024988();
            break;
        case 65:
            func_8024A0C();
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
            goto updateDownRight;
        case 69:
            func_8024BEC();
            goto updateDownLeft;
        case 71:
            BeginGoldenDivaExplosiveBallSlopeRight();
        case 68:
        case 72:
updateDownRight:
            func_8024AD4();
            break;
        case 73:
            BeginGoldenDivaExplosiveBallSlopeLeft();
        case 70:
        case 74:
updateDownLeft:
            func_8024C00();
            break;
        case 81:
            BeginGoldenDivaExplosiveBallLiftRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            BeginGoldenDivaExplosiveBallLiftLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            BeginGoldenDivaExplosiveBallCarryRight();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            BeginGoldenDivaExplosiveBallCarryLeft();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;
        case 89:
            gCurrentSprite.pose = 90;
            gCurrentSprite.health = 0;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 90:
            SpriteUtilThrownLeftSoft();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        case 93:
            gCurrentSprite.pose = 94;
            gCurrentSprite.health = 1;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 94:
            SpriteUtilThrownLeftHard();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        case 97:
            gCurrentSprite.pose = 98;
            gCurrentSprite.health = 0;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 98:
            SpriteUtilThrownUpLeftSoft();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        case 101:
            gCurrentSprite.pose = 102;
            gCurrentSprite.health = 18;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 102:
            SpriteUtilThrownUpLeftHard();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        case 91:
            gCurrentSprite.pose = 92;
            gCurrentSprite.health = 0;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 92:
            SpriteUtilThrownRightSoft();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        case 95:
            gCurrentSprite.pose = 96;
            gCurrentSprite.health = 2;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 96:
            SpriteUtilThrownRightHard();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        case 99:
            gCurrentSprite.pose = 100;
            gCurrentSprite.health = 0;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 100:
            SpriteUtilThrownUpRightSoft();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        case 103:
            gCurrentSprite.pose = 104;
            gCurrentSprite.health = 17;
            PrepareGoldenDivaExplosiveBallHardThrow();
        case 104:
            SpriteUtilThrownUpRightHard();
            UpdateGoldenDivaExplosiveBallHardThrow();
            break;
        default:
            temp = *(u8 *)(sprite + 29);
            value = 0xF0;
            value &= temp;
            if (value != 0) {
                temp = sprite;
                temp += 41;
                value = 4;
            } else {
                temp = sprite;
                temp += 41;
                value = 8;
            }
            *(u8 *)temp = value;
            temp = gCurrentSprite.health;
            value = 15;
            value &= temp;
            if (value == 2)
                func_8024AC0();
            else
                func_8024BEC();
            break;
    }

    sprite = (u32)&gCurrentSprite;
    temp = *(u16 *)sprite;
    value = 128;
    value &= temp;
    if (value == 0 && ((struct PrimarySpriteData *)sprite)->warioCollision == 51) {
        value = 5;
        ((struct PrimarySpriteData *)sprite)->warioCollision = value;
    }
    DespawnGoldenDivaAttack();
}

void SpriteGoldenDivaBlackEgg(void)
{
    register u32 value PIN("r0");
    u32 temp;
    u32 sprite;

    value = (u32)&gCurrentSprite;
    sprite = *(u16 *)value;
    temp = 0xFF5F;
    temp &= sprite;
    *(u16 *)value = temp;
    temp = *(u8 *)(value + 28);
    sprite = value;
    switch (temp) {
        case 0:
            InitGoldenDivaBlackEgg();
            break;
        case 15:
        case 23:
        case 47:
            BeginGoldenDivaBlackEggFall();
        case 16:
            UpdateGoldenDivaBlackEggFall();
            break;
        case 17:
            BeginGoldenDivaBlackEggBounce();
        case 18:
            UpdateGoldenDivaBlackEggBounce();
            break;
        case 27:
        case 29:
            PrepareGoldenDivaBlackEggMovement();
        case 30:
            func_8023EE0();
            break;
        case 43:
            BeginGoldenDivaBlackEggGroundRight();
        case 44:
            func_8024478();
            break;
        case 45:
            BeginGoldenDivaBlackEggGroundLeft();
        case 46:
            func_802449C();
            break;
        case 31:
        case 79:
        case 106:
            MoveGoldenDivaBlackEggOffscreenRight();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
        case 105:
            MoveGoldenDivaBlackEggOffscreenLeft();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
            if (*(u8 *)(sprite + 30) == 46)
                goto changeDirection;
        case 39:
        case 51:
            MoveGoldenDivaBlackEggRight();
            goto updateGroundRight;
        case 37:
            if (*(u8 *)(sprite + 30) != 46)
                goto runGroundLeft;
changeDirection:
            value = 17;
            *(u8 *)(sprite + 28) = value;
            value = *(u16 *)sprite;
            temp = 64;
            value ^= temp;
            *(u16 *)sprite = value;
            break;
        case 41:
        case 53:
runGroundLeft:
            MoveGoldenDivaBlackEggLeft();
            goto updateGroundLeft;
        case 52:
        case 64:
updateGroundRight:
            func_8024988();
            break;
        case 54:
        case 58:
updateGroundLeft:
            func_802476C();
            break;
        case 55:
            func_8024688();
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
            func_802476C();
            break;
        case 59:
            func_80247F0();
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
            func_8024988();
            break;
        case 65:
            func_8024A0C();
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
            goto updateDownRight;
        case 69:
            func_8024BEC();
            goto updateDownLeft;
        case 71:
            BeginGoldenDivaBlackEggSlopeRight();
        case 68:
        case 72:
updateDownRight:
            func_8024AD4();
            break;
        case 73:
            BeginGoldenDivaBlackEggSlopeLeft();
        case 70:
        case 74:
updateDownLeft:
            func_8024C00();
            break;
        case 81:
            BeginGoldenDivaBlackEggLiftRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            BeginGoldenDivaBlackEggLiftLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            BeginGoldenDivaBlackEggCarryRight();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            BeginGoldenDivaBlackEggCarryLeft();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;
        case 89:
            temp = 0;
            value = 90;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 90:
            SpriteUtilThrownLeftSoft();
            break;
        case 93:
            value = 94;
            *(u8 *)(sprite + 28) = value;
            value = 1;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaBlackEggHardThrow();
        case 94:
            SpriteUtilThrownLeftHard();
            UpdateGoldenDivaBlackEggHardThrow();
            break;
        case 97:
            temp = 0;
            value = 98;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;
        case 101:
            value = 102;
            *(u8 *)(sprite + 28) = value;
            value = 18;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaBlackEggHardThrow();
        case 102:
            SpriteUtilThrownUpLeftHard();
            UpdateGoldenDivaBlackEggHardThrow();
            break;
        case 91:
            temp = 0;
            value = 92;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 92:
            SpriteUtilThrownRightSoft();
            break;
        case 95:
            value = 96;
            *(u8 *)(sprite + 28) = value;
            value = 2;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaBlackEggHardThrow();
        case 96:
            SpriteUtilThrownRightHard();
            UpdateGoldenDivaBlackEggHardThrow();
            break;
        case 99:
            temp = 0;
            value = 100;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;
        case 103:
            value = 104;
            *(u8 *)(sprite + 28) = value;
            value = 17;
            *(u8 *)(sprite + 29) = value;
            PrepareGoldenDivaBlackEggHardThrow();
        case 104:
            SpriteUtilThrownUpRightHard();
            UpdateGoldenDivaBlackEggHardThrow();
            break;
        default:
            SpriteUtilDie();
            break;
    }
    DespawnGoldenDivaAttack();
}

void SpriteGoldenDivaGreenEnemy(void)
{
    register u32 value PIN("r0");
    u32 temp;
    register u32 sprite PIN("r2");

    value = (u32)&gCurrentSprite;
    sprite = *(u16 *)value;
    temp = 0xFFDF;
    temp &= sprite;
    *(u16 *)value = temp;
    value = *(u8 *)(value + 28);
    switch (value) {
        case 0:
            InitGoldenDivaGreenEnemy();
            break;
        case 29:
            BeginGoldenDivaGreenEnemyFall();
        case 30:
            func_8023FA8();
            break;
        case 31:
        case 79:
            MoveGoldenDivaGreenEnemyRight();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            MoveGoldenDivaGreenEnemyLeft();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 105:
        case 106:
            UpdateGoldenDivaGreenEnemyDefeat();
            break;
        case 49:
        case 107:
            SpriteUtilDie();
            break;
        case 35:
        case 51:
            BeginGoldenDivaGreenEnemyGroundRight();
        case 52:
            func_8024A3C();
            break;
        case 37:
        case 53:
            BeginGoldenDivaGreenEnemyGroundLeft();
        case 54:
            func_8024820();
            break;
        case 67:
            func_8024AC0();
            goto updateDownRight;
        case 69:
            func_8024BEC();
            goto updateDownLeft;
        case 71:
            BeginGoldenDivaGreenEnemySlopeRight();
        case 68:
        case 72:
updateDownRight:
            func_8024AD4();
            break;
        case 73:
            BeginGoldenDivaGreenEnemySlopeLeft();
        case 70:
        case 74:
updateDownLeft:
            func_8024C00();
            break;
        case 81:
            BeginGoldenDivaGreenEnemyLiftRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            BeginGoldenDivaGreenEnemyLiftLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            BeginGoldenDivaGreenEnemyCarryRight();
            break;
        case 88:
            BeginGoldenDivaGreenEnemyCarryLeft();
            break;
        case 85:
            BeginGoldenDivaGreenEnemyThrownRight();
            break;
        case 86:
            BeginGoldenDivaGreenEnemyThrownLeft();
            break;
        case 89:
            gCurrentSprite.pose = 90;
        case 90:
            SpriteUtilThrownLeftSoft();
            break;
        case 93:
            gCurrentSprite.pose = 94;
        case 94:
            SpriteUtilThrownLeftHard();
            break;
        case 97:
            gCurrentSprite.pose = 98;
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;
        case 101:
            gCurrentSprite.pose = 102;
        case 102:
            SpriteUtilThrownUpLeftHard();
            break;
        case 91:
            gCurrentSprite.pose = 92;
        case 92:
            SpriteUtilThrownRightSoft();
            break;
        case 95:
            gCurrentSprite.pose = 96;
        case 96:
            SpriteUtilThrownRightHard();
            break;
        case 99:
            gCurrentSprite.pose = 100;
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;
        case 103:
            gCurrentSprite.pose = 104;
        case 104:
            SpriteUtilThrownUpRightHard();
            break;
        case 121:
            BeginGoldenDivaGreenEnemyDefeat();
            break;
        case 123:
            BeginGoldenDivaGreenEnemyAttack();
            break;
        default:
            UpdateGoldenDivaGreenEnemy();
            break;
    }

    sprite = (u32)&gCurrentSprite;
    temp = *(u16 *)sprite;
    value = 32;
    value |= temp;
    *(u16 *)sprite = value;
    DespawnGoldenDivaAttack();
}

void SpriteGoldenDivaBlackChick(void)
{
    s32 value;

    value = gCurrentSprite.pose;
    switch (value) {
        case 0:
            InitGoldenDivaBlackChick();
            break;
        case 28:
            func_8023D48();
            break;
        case 23:
            BeginGoldenDivaBlackChickFall();
            break;
        case 16:
            UpdateGoldenDivaBlackChickFall();
            break;
        case 35:
            BeginGoldenDivaBlackChickGroundRight();
        case 36:
            UpdateGoldenDivaBlackChickGroundRight();
            break;
        case 37:
            BeginGoldenDivaBlackChickGroundLeft();
        case 38:
            UpdateGoldenDivaBlackChickGroundLeft();
            break;
        case 51:
            BeginGoldenDivaBlackChickAttackRight();
            break;
        case 31:
        case 106:
            MoveGoldenDivaBlackChickRight();
        case 32: {
            s32 inverse;
            register s32 tableValue PIN("r4");
            struct PrimarySpriteData *sprite;
            s32 scale;
            u8 *work0Pointer;
            const s16 *table;

            SpriteUtilFallOffscreenRight();
            sprite = &gCurrentSprite;
            value = 0;
            *(u8 *)((u32)sprite + 22) = value;
            *(u16 *)((u32)sprite + 20) = value;
            table = sSinCosTable;
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value += 64;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            scale = 256;
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePA = value;

            value = *work0Pointer;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePB = value;

            value = *work0Pointer;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            tableValue = -tableValue;
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePC = value;

            value = *work0Pointer;
            value += 64;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePD = value;
            value = *work0Pointer;
            value += 8;
            *work0Pointer = value;
            break;
        }
        case 53:
            BeginGoldenDivaBlackChickAttackLeft();
            break;
        case 33:
        case 105:
            MoveGoldenDivaBlackChickLeft();
        case 34: {
            s32 inverse;
            register s32 tableValue PIN("r4");
            struct PrimarySpriteData *sprite;
            s32 scale;
            u8 *work0Pointer;
            const s16 *table;

            SpriteUtilFallOffscreenLeft();
            sprite = &gCurrentSprite;
            value = 0;
            *(u8 *)((u32)sprite + 22) = value;
            *(u16 *)((u32)sprite + 20) = value;
            table = sSinCosTable;
            work0Pointer = &sprite->work0;
            value = *work0Pointer;
            value += 64;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            scale = 256;
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePA = value;

            value = *work0Pointer;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePB = value;

            value = *work0Pointer;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            tableValue = -tableValue;
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePC = value;

            value = *work0Pointer;
            value += 64;
            value <<= 1;
            value += (s32)table;
            inverse = 0;
            tableValue = *(s16 *)(value + inverse);
            value = FixedInverse(scale);
            inverse = value;
            value = tableValue;
            value = FixedMul(value, inverse);
            sprite->affinePD = value;
            value = *work0Pointer;
            value -= 8;
            *work0Pointer = value;
            break;
        }
        case 110:
            BeginGoldenDivaBlackChickPose110();
        case 111:
            UpdateGoldenDivaBlackChickPose111();
            break;
        case 112:
            BeginGoldenDivaBlackChickPose112();
        case 113:
            UpdateGoldenDivaBlackChickPose113();
            break;
        default: {
            u32 work1Pointer;
            u32 zero;
            work1Pointer = (u32)&gCurrentSprite;
            work1Pointer += 40;
            zero = 0;
            *(u8 *)work1Pointer = zero;
            SpriteUtilDie();
            break;
        }
    }

    {
        struct PrimarySpriteData *sprite;
        u8 *work1Pointer;
        sprite = &gCurrentSprite;
        work1Pointer = &sprite->work1;
        value = *work1Pointer;
        if (value != 0) {
            value--;
            *work1Pointer = value;
            value <<= 24;
            if (value == 0)
                sprite->pose = 110;
        }
    }
    DespawnGoldenDivaAttack();
}

void SpriteGoldenDivaHammer(void)
{
    u32 value;
    u32 temp;
    u32 sprite;

    sprite = (u32)&gCurrentSprite;
    value = *(u16 *)sprite;
    temp = 0xFF7F;
    temp &= value;
    *(u16 *)sprite = temp;
    value = 1;
    value &= temp;
    if (value != 0) {
        value = *(u8 *)(sprite + 28);
        if (value > 14) {
            value = *(u8 *)(sprite + 29);
            if (value != 0) {
                value--;
                *(u8 *)(sprite + 29) = value;
                value = (u8)value;
                if (value == 0) {
                    value = 114;
                    *(u8 *)(sprite + 28) = value;
                } else if (value == 60) {
                    value = (u32)sGoldenDivaHammerActiveOam;
                    *(u32 *)(sprite + 4) = value;
                    {
                        register u8 zero8 PIN("r0");
                        zero8 = 0;
                        *(u8 *)(sprite + 22) = zero8;
                    }
                    {
                        u16 zero16;
                        zero16 = 0;
                        *(u16 *)(sprite + 20) = zero16;
                    }
                }
            }
        }
    }

    value = *(u8 *)(sprite + 28);
    switch (value) {
        case 0:
            InitGoldenDivaHammer();
        case 111:
            UpdateGoldenDivaHammerOrbit();
            break;
        case 15:
        case 23:
        case 47:
            BeginGoldenDivaHammerFall();
        case 16:
            UpdateGoldenDivaHammerFall();
            break;
        case 114:
            BeginGoldenDivaHammerSwing();
        case 115:
            UpdateGoldenDivaHammerSwing();
            break;
        case 27:
        case 29:
            PrepareGoldenDivaHammerMovement();
        case 30:
            func_8023EE0();
            break;
        case 43:
            BeginGoldenDivaHammerGroundRight();
        case 44:
            func_8024478();
            break;
        case 45:
            BeginGoldenDivaHammerGroundLeft();
        case 46:
            func_802449C();
            break;
        case 49:
            UpdateGoldenDivaHammerLanding();
            break;
        case 31:
        case 79:
            MoveGoldenDivaHammerOffscreenRight();
            goto updateAffine;
        case 33:
        case 80:
            MoveGoldenDivaHammerOffscreenLeft();
        case 32:
        case 34:
updateAffine:
            UpdateGoldenDivaHammerAffine();
            break;
        case 35:
        case 39:
        case 51:
            MoveGoldenDivaHammerRight();
        case 52:
        case 64:
            func_8024988();
            break;
        case 37:
        case 41:
        case 53:
            MoveGoldenDivaHammerLeft();
        case 54:
        case 58:
            func_802476C();
            break;
        case 55:
            func_8024688();
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
            func_802476C();
            break;
        case 59:
            func_80247F0();
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
            func_8024988();
            break;
        case 65:
            func_8024A0C();
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
        case 68:
        case 72:
            func_8024AD4();
            break;
        case 69:
            func_8024BEC();
        case 70:
        case 74:
            func_8024C00();
            break;
        case 71:
            BeginGoldenDivaHammerSlopeRight();
            func_8024AD4();
            break;
        case 73:
            BeginGoldenDivaHammerSlopeLeft();
            func_8024C00();
            break;
        default:
            temp = 0;
            value = 114;
            *(u8 *)(sprite + 28) = value;
            *(u8 *)(sprite + 29) = temp;
            break;
    }
    DespawnGoldenDivaAttack();
}

void SpriteGoldenDivaFace(void)
{
    u32 value;
    u32 temp;
    u32 sprite;

    value = gCurrentSprite.pose;
    switch (value) {
        case 0:
            InitGoldenDivaFace();
            break;
        case 28:
            func_8023D48();
            break;
        case 23:
            BeginGoldenDivaFaceFall();
            break;
        case 16:
            UpdateGoldenDivaFaceFall();
            break;
        case 35:
        case 51:
            BeginGoldenDivaFaceGroundRight();
        case 36:
            UpdateGoldenDivaFaceGroundRight();
            break;
        case 37:
        case 53:
            BeginGoldenDivaFaceGroundLeft();
        case 38:
            UpdateGoldenDivaFaceGroundLeft();
            break;
        case 31:
            MoveGoldenDivaFaceRight();
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
            MoveGoldenDivaFaceLeft();
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        default:
            ResetGoldenDivaFaceMotion();
            SpriteUtilDie();
            break;
    }

    sprite = (u32)&gCurrentSprite;
    value = ((struct PrimarySpriteData *)sprite)->warioCollision;
    if (value == 0) {
        value = gMainTimer;
        temp = value;
        value = 7;
        value &= temp;
        if (value == 0) {
            value = 8;
            value &= temp;
            value = (u8)value;
            if (value != 0)
                value = 4;
            ((struct PrimarySpriteData *)sprite)->palette = value;
        }
    }
}

void SpriteGoldenDivaDebris(void)
{
    s32 value;
    s32 inverse;
    s32 tableValue;
    s32 sine;
    s32 scale;
    struct PrimarySpriteData *sprite;

    {
        struct PrimarySpriteData *initial;

        initial = &gCurrentSprite;
        initial->disableWarioCollisionTimer = 1;
        value = initial->pose;
    }
    switch (value) {
        case 0:
            InitGoldenDivaDebris();
        case 15:
            BeginGoldenDivaDebrisShrink();
            break;
        case 16:
            UpdateGoldenDivaDebrisShrink();
            break;
    }

    sprite = &gCurrentSprite;
    scale = sprite->work1;
    if (scale == 0)
        scale = 256;
    tableValue = (s32)sSinCosTable;
    value = tableValue + 128;
    inverse = 0;
    sine = *(s16 *)value;

    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePA = value;

    value = 0;
    tableValue = *(s16 *)tableValue;
    value = FixedInverse(scale);
    inverse = value;
    value = tableValue;
    value = FixedMul(value, inverse);
    sprite->affinePB = value;

    tableValue = -tableValue;
    value = FixedInverse(scale);
    inverse = value;
    value = tableValue;
    value = FixedMul(value, inverse);
    sprite->affinePC = value;

    value = FixedInverse(scale);
    inverse = value;
    value = sine;
    value = FixedMul(value, inverse);
    sprite->affinePD = value;
}

void SpriteGoldenDivaAttackEffect(void)
{
    u32 value;
    register u32 temp PIN("r1");
    register u32 zero PIN("r2");
    u32 sprite;
    u32 pose;

    sprite = (u32)&gCurrentSprite;
    value = 1;
    ((struct PrimarySpriteData *)sprite)->disableWarioCollisionTimer = value;
    pose = ((struct PrimarySpriteData *)sprite)->pose;
    if (pose == 0) {
        temp = *(u16 *)sprite;
        zero = 128;
        zero <<= 3;
        ASM_BARRIER(: "+r"(zero));
        value = zero;
        zero = 0;
        value |= temp;
        temp = 0xFFFB;
        value &= temp;
        *(u16 *)sprite = value;
        *(u8 *)(sprite + 30) = zero;
        temp = sprite;
        temp += 32;
        value = 32;
        *(u8 *)temp = value;
        temp++;
        *(u8 *)temp = value;
        temp++;
        *(volatile u8 *)temp = value;
        value = sprite;
        value += 35;
        temp = 4;
        *(u8 *)value = temp;
        value++;
        *(u8 *)value = temp;
        value++;
        *(u8 *)value = temp;
        value++;
        *(u8 *)value = temp;
        value = (u32)sGoldenDivaAttackEffectOam;
        *(u32 *)(sprite + 4) = value;
        *(u8 *)(sprite + 22) = zero;
        *(u16 *)(sprite + 20) = pose;
        temp = sprite;
        temp += 39;
        value = 36;
        *(u8 *)temp = value;
        value = 16;
        *(u8 *)(sprite + 28) = value;
    }

    temp = sprite;
    temp += 39;
    value = *(u8 *)temp;
    value--;
    *(u8 *)temp = value;
    value = (u8)value;
    if (value == 0)
        *(u16 *)sprite = value;
}

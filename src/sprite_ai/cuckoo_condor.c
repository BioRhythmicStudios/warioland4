#include "fixed_point.h"
#include "color_effects.h"
#include "background_registers.h"
#include "global_data.h"
#include "gba/m4a.h"
#include "main.h"
#include "oam.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/cuckoo_condor.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

extern void UpdateBossHealthGauge(void);
extern void SpawnPrimarySpriteWithStatus(u8 id, u8 roomSlot, u8 gfxSlot, u32 yPosition, u32 xPosition, u32 status);

/* Compiler-boundary alias: preserve the original untruncated register argument. */
extern void SpriteUtilFindOwnSlotU32(u32 roomSlot) asm("SpriteUtilFindOwnSlot");

/* Scalar view of the first byte of the six-byte fading-state array. */
extern u8 gCuckooCondorColorFadingState asm("gBossDefeatTimer");

extern u8 gSpriteCollisionResult;
extern u8 gSpriteCollisionTileType;
extern s8 gCuckooCondorPendulumLength;
extern u8 gCuckooCondorMoveRight;
extern u8 gCuckooCondorHasCapturedWario;
extern u8 gInitialHealth;
extern u8 gBgAnimationFrame;
extern u16 gSpriteAiRadius;
extern const s16 sSpriteGravityVelocityTable[];

void UpdateCuckooCondorPendulumOrbit(void)
{
    struct PrimarySpriteData *sprite;
    u32 angle;
    int slot;
    u16 radius;
    register u32 temp PIN("r0");
    u32 adjustmentR1;
    register u32 adjustmentR2 PIN("r2");
    register u32 adjustmentR4 PIN("r4");
    struct PrimarySpriteData *parentBase;
    struct PrimarySpriteData *parent;
    register u32 index PIN("r1");
    u16 baseY;
    register u16 savedY;
    u16 baseX;
    const s16 *table;
    const s16 *tableSource;
    s32 value;
    s32 product;
    register u32 work PIN("r0");
    s32 secondTrig;
    register s32 result PIN("r1");
    struct PrimarySpriteData *current;
    s32 xResult;
    u32 angleShifted;
    register s32 shifted PIN("r1");
    register s32 xOffset PIN("r1");

    sprite = &gCurrentSprite;
    work = sprite->work0;
    ASM_BARRIER(: "+r"(work));
    angle = work;
    angle += 0x40;
    angle &= 0xFF;
    slot = SpriteUtilFindSpriteSlot(0xE5);
    radius = gSpriteAiRadius;

    switch (sprite->roomSlot) {
    case 0:
        goto radiusReady;
    case 1:
        temp = radius << 16;
        adjustmentR1 = 0xFFB80000;
        goto addAdjustmentR1;
    case 2:
        temp = radius << 16;
        adjustmentR2 = 0xFF980000;
        ASM_BARRIER(: "+r"(adjustmentR2));
        temp += adjustmentR2;
        goto finishRadius;
    case 3:
        temp = radius << 16;
        adjustmentR4 = 0xFF780000;
        ASM_BARRIER(: "+r"(adjustmentR4));
        temp += adjustmentR4;
        goto finishRadius;
    case 4:
        temp = radius << 16;
        adjustmentR1 = 0xFF580000;
addAdjustmentR1:
        temp += adjustmentR1;
finishRadius:
        radius = temp >> 16;
        break;
    }
radiusReady:

    parentBase = gSpriteData;
    index = slot;
    index *= sizeof(struct PrimarySpriteData);
    index += (u32)parentBase;
    parent = (struct PrimarySpriteData *)index;
    temp = gSpriteAiRadius;
    temp += parent->yPosition;
    temp <<= 16;
    baseY = temp >> 16;
    savedY = baseY;
    baseX = parent->xPosition;
    tableSource = sSinCosTable;
    angleShifted = angle << 16;
    temp = angleShifted >> 15;
    temp += (u32)tableSource;
    angle = 0;
    value = *(const s16 *)((const u8 *)temp + angle);
    table = tableSource;
    angle = angleShifted;

    if (value < 0) {
        register s32 trigAbs PIN("r2");
        trigAbs = -value;
        shifted = radius << 16;
        product = (shifted >> 16) * trigAbs;
        value = shifted;
        if (product < 0)
            product += 0xFF;
        product = (u32)product << 8;
        current = &gCurrentSprite;
        result = baseY;
        result = (s16)result;
        result -= product >> 16;
    } else {
        shifted = radius << 16;
        product = (shifted >> 16) * value;
        value = shifted;
        if (product < 0)
            product += 0xFF;
        product = (u32)product << 8;
        current = &gCurrentSprite;
        result = savedY;
        result = (s16)result;
        result += product >> 16;
    }
    current->yPosition = result;

    temp = angle >> 15;
    temp += 0x80;
    temp += (u32)table;
    secondTrig = *(const s16 *)temp;
    if (secondTrig < 0) {
        secondTrig = -secondTrig;
        product = value >> 16;
        product *= secondTrig;
        if (product < 0)
            product += 0xFF;
        xOffset = (u32)product << 8;
        xResult = baseX;
        xResult = (s16)xResult;
        xResult -= xOffset >> 16;
    } else {
        product = value >> 16;
        product *= secondTrig;
        if (product < 0)
            product += 0xFF;
        xOffset = (u32)product << 8;
        xResult = baseX;
        xResult = (s16)xResult;
        xResult += xOffset >> 16;
    }
    current->xPosition = xResult;
}

void UpdateCuckooCondorPendulumAffine(void)
{
    struct PrimarySpriteData *sprite;
    u32 angle;
    const s16 *table;
    s32 sinValue;
    s32 cosValue;
    s32 scale;
    s32 inverse;
    u32 sinIndex;

    sprite = &gCurrentSprite;
    sprite->status |= 0x80;
    angle = sprite->work0;
    table = sSinCosTable;
    sinIndex = angle;
    sinIndex += 0x40;
    sinIndex <<= 1;
    sinIndex += (u32)table;
    sinValue = *(const s16 *)sinIndex;
    scale = 0x100;

    inverse = FixedInverse(scale);
    sprite->affinePA = FixedMul(sinValue, inverse);

    cosValue = table[angle];
    inverse = FixedInverse(scale);
    sprite->affinePB = FixedMul(cosValue, inverse);

    cosValue = -cosValue;
    inverse = FixedInverse(scale);
    sprite->affinePC = FixedMul(cosValue, inverse);

    inverse = FixedInverse(scale);
    sprite->affinePD = FixedMul(sinValue, inverse);
}

void UpdateCuckooCondorPendulumHeight(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *current;
    u32 roomSlot;
    u32 currentY;
    u32 targetY;
    register u32 speed PIN("r2");
    u32 offset;
    register struct PrimarySpriteData *sprites PIN("r3");
    u32 temp;
    register u32 base PIN("r2");
    register u32 distance PIN("r0");

    current = &gCurrentSprite;
    roomSlot = current->roomSlot;
    sprite = current;
    if (roomSlot == 4) {
        current = (struct PrimarySpriteData *)&gCuckooCondorPendulumLength;
        offset = *(u8 *)current;
    } else {
        offset = 0x20;
        if (roomSlot == 0)
            offset = 0x48;
    }

    temp = sprite->yPosition;
    temp -= offset;
    base = 0x80;
    base <<= 2;
    ASM_BARRIER(: "+r"(base));
    targetY = base;
    temp += targetY;
    currentY = (u16)temp;

    sprites = gSpriteData;
    current = sprite;
    current = (struct PrimarySpriteData *)((u8 *)current + 0x2A);
    speed = *(u8 *)current;
    temp = sizeof(struct PrimarySpriteData);
    temp *= speed;
    temp += (u32)sprites;
    temp = ((struct PrimarySpriteData *)temp)->yPosition;
    targetY += temp;
    targetY <<= 16;
    targetY >>= 16;

    if (currentY > targetY)
        distance = currentY - targetY;
    else
        distance = targetY - currentY;
    distance = (u16)distance;

    if (distance <= 1) {
        speed = 0;
    } else {
        distance -= 2;
        distance = (s32)distance >> 1;
        distance <<= 16;
        speed = distance >> 16;
        if (speed == 0)
            speed = 1;
    }

    ASM_BARRIER(::: "memory");
    if (currentY > targetY) {
        temp = sprite->yPosition;
        temp -= speed;
        sprite->yPosition = temp;
    } else if (currentY < targetY) {
        temp = sprite->yPosition;
        temp = (speed) - (-(temp));
        sprite->yPosition = temp;
    }
}

void UpdateCuckooCondorCapturedWario(void)
{
    struct PrimarySpriteData *sprite;
    const s16 *table;
    u32 index;
    u16 delta;
    u32 zero;
    u32 address;
    s32 signedDelta;
    register u8 *work3Ptr PIN("r12");
    u8 *flag;
    int slot;
    struct WarioData *wario;
    register struct PrimarySpriteData *sprites PIN("r1");
    struct PrimarySpriteData *other;
    u16 yPosition;

    sprite = &gCurrentSprite;
    address = 0x2A;
    address += (u32)sprite;
    work3Ptr = (u8 *)address;
    index = *work3Ptr;
    table = sCuckooCondorClockYVelocity;
    address = index << 1;
    address += (u32)table;
    delta = *(u16 *)address;
    zero = 0;
    signedDelta = *(s16 *)(address + zero);
    address = 0x7FFF;
    if (signedDelta == address) {
        delta = table[0];
        index = 0;
    }
    address = index + 1;
    signedDelta = (u32)work3Ptr;
    *(u8 *)signedDelta = address;
    yPosition = sprite->yPosition;
    yPosition += delta;
    sprite->yPosition = yPosition;

    flag = &gCuckooCondorHasCapturedWario;
    address = *flag;
    if (address != 0) {
        slot = SpriteUtilFindSprite(0xE8, 0);
        wario = &gWarioData;
        address = wario->damageTimer;
        if (address != 0) {
            address = 0;
            *flag = address;
            sprites = gSpriteData;
            address = sizeof(struct PrimarySpriteData);
            address *= slot;
            address += (u32)sprites;
            other = (struct PrimarySpriteData *)address;
            signedDelta = 0x43;
            other->warioCollision = signedDelta;
        } else {
            sprites = gSpriteData;
            address = sizeof(struct PrimarySpriteData);
            address *= slot;
            address += (u32)sprites;
            other = (struct PrimarySpriteData *)address;
            address = other->yPosition;
            signedDelta = *(u16 *)((u8 *)wario + 0x34);
            address -= signedDelta;
            wario->yPosition = address;
        }
    }
}

void InitCuckooCondor(void)
{
    struct PrimarySpriteData *sprite;
    s8 *horizontalOffset;

    gBossState = 0;
    horizontalOffset = &gCuckooCondorPendulumLength;
    *horizontalOffset = 0x10;
    gCuckooCondorMoveRight = 0;
    gCuckooCondorHasCapturedWario = 0;
    gInitialHealth = 0;
    gBossSequenceState = 0;
    gBgAnimationFrame = 0x0E;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawPriority |= 0x80;
    sprite->warioCollision = 0;
    sprite->drawDistanceDown = 0x28;
    sprite->drawDistanceUp = 0x28;
    sprite->drawDistanceLeftRight = 0x28;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 4;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    GP_SET(sprite->pOamData, sCuckooCondorClockIntroOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 1;
    sprite->work3 = 0;

    SpawnHighPriorityPrimarySprite(0xE7, 4, 0, sprite->yPosition + 0x38, sprite->xPosition + 0xF8);
    SpawnHighPriorityPrimarySprite(0xE7, 3, 0, sprite->yPosition + 0x34, sprite->xPosition + 0xE0);
    SpawnHighPriorityPrimarySprite(0xE7, 2, 0, sprite->yPosition + 0x5C, sprite->xPosition + 0x94);
    SpawnHighPriorityPrimarySprite(0xE7, 1, 0, sprite->yPosition + 0x56, sprite->xPosition - 0xB0);
    SpawnHighPriorityPrimarySprite(0xE7, 0, 0, sprite->yPosition + 0x56, sprite->xPosition - 0x90);

    SpawnHighPriorityPrimarySprite(0xE8, 0, 0, sprite->yPosition + (u8)*horizontalOffset + 0xA8, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(0xE8, 1, 0, sprite->yPosition + (u8)*horizontalOffset + 0x60, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(0xE8, 2, 0, sprite->yPosition + (u8)*horizontalOffset + 0x40, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(0xE8, 3, 0, sprite->yPosition + (u8)*horizontalOffset + 0x20, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(0xE8, 4, 0, sprite->yPosition + (u8)*horizontalOffset, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(0xE5, 0, 0, sprite->yPosition - 0x98, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(0xE6, 0, 0, sprite->yPosition, sprite->xPosition);

    sprite->work2 = 0x3C;
    LoadBossSpriteGraphics(0x83, 8, 4);
    sprite->health = 0x0E;
    UpdateBossHealthGauge();
}

void CuckooCondorWaitForIntro(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer;
    if (value != 0) {
        value--;
        *timer = value;
        value <<= 24;
        if (value != 0)
            return;
        {
            register u8 *state PIN("r1");
            state = &gCuckooCondorColorFadingState;
            value = 7;
            *state = value;
        }
    }
    value = gCuckooCondorColorFadingState;
    if (value == 0) {
        value = 2;
        sprite->pose = value;
        value = 0x5A;
        *timer = value;
    }
}

void CuckooCondorStartBattle(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;
    register u32 zero PIN("r4");

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer;
    value--;
    *timer = value;
    value <<= 24;
    if (value == 0) {
        value = gCurrentShopItem;
        zero = value;
        if (zero == 0) {
            SpriteUtilStartBossTimer();
            GP_SET(sprite->pOamData, sCuckooCondorClockIdleOam);
            sprite->currentAnimationFrame = zero;
            sprite->animationTimer = zero;
            m4aSongNumStart(SOUND_A2);
        }
        value = 3;
        sprite->pose = value;
    }
}

void CuckooCondorWaitForTimer(void)
{
    if (gWarioPauseTimer == 0 && gTimerState == 1) {
        SpriteUtilUnsetAllSpritesHighPriority();
        gCurrentSprite.work2 = 0x18;
        gCurrentSprite.pose = 4;
    }
}

void CuckooCondorEnterArena(void)
{
    struct PrimarySpriteData *sprite;
    u32 spriteY;
    register u32 value PIN("r0");
    u32 bgY;
    u32 zero;
    int slot;
    struct PrimarySpriteData *sprites;
    u32 offset;
    u32 size;
    struct PrimarySpriteData *other;

    sprite = &gCurrentSprite;
    spriteY = sprite->yPosition;
    value = spriteY >> 2;
    bgY = gBg1YPosition;
    bgY >>= 2;
    value -= bgY;
    value = (u16)value;
    if (value > 0x24) {
        value = spriteY - 4;
        sprite->yPosition = value;
    } else {
        zero = 0;
        sprite->pose = 0x10;
        {
            u8 *timer;
            timer = (u8 *)sprite;
            timer += 0x29;
            *timer = 0x3C;
        }
        slot = SpriteUtilFindSprite(0xE8, 0);
        sprites = gSpriteData;
        size = sizeof(struct PrimarySpriteData);
        offset = slot;
        offset *= size;
        value = (u32)sprites + 4;
        value = offset + value;
        GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)value, sCuckooCondorClawOpenOam);
        other = (struct PrimarySpriteData *)(offset + (u32)sprites);
        other->currentAnimationFrame = zero;
        other->animationTimer = zero;
        {
            u32 dma;
            dma = 0x040000D4;
            *(vu32 *)dma = (u32)sCuckooCondorBattleGfx;
            *(vu32 *)(dma + 4) = 0x06013000;
            *(vu32 *)(dma + 8) = 0x80000800;
            value = *(vu32 *)(dma + 8);
        }
        m4aSongNumStart(SOUND_A2);
    }
}

int TryStartCuckooCondorGrab(void)
{
    u32 flag;
    struct WarioData *wario;
    struct PrimarySpriteData *sprite;
    u32 warioX;
    u32 spriteX;
    int slot;
    struct PrimarySpriteData *sprites;
    u32 offset;
    u32 value;
    u32 size;
    u32 zero;
    struct PrimarySpriteData *current;
    u32 keep;

    flag = 0;
    wario = &gWarioData;
    warioX = wario->xPosition;
    sprite = &gCurrentSprite;
    spriteX = sprite->xPosition;
    if (spriteX > warioX) {
        spriteX -= warioX;
        if ((s32)spriteX > 0x3F)
            goto after_check;
        goto trigger;
    } else {
        spriteX = warioX - spriteX;
        if ((s32)spriteX <= 0x3F)
            flag = 1;
    }
after_check:
    if (flag == 0)
        RETURN_NO_VALUE(0);
trigger:
    slot = SpriteUtilFindSprite(0xE8, 0);
    sprites = gSpriteData;
    size = sizeof(struct PrimarySpriteData);
    offset = slot;
    offset *= size;
    value = (u32)sprites + 4;
    value = offset + value;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)value, sCuckooCondorClawGrabOam);
    offset += (u32)sprites;
    zero = 0;
    *(u8 *)(offset + 0x16) = zero;
    *(u16 *)(offset + 0x14) = zero;
    current = &gCurrentSprite;
    size = 0x6E;
    current->pose = size;
    current += 0; /* keep current live */
    current->work2 = 0x10;
    m4aSongNumStart(SOUND_A2);
    keep = 0;
    RETURN_NO_VALUE(0);
}

void CuckooCondorMoveClock(void)
{
    register u32 temp PIN("r0");
    u32 value;
    u32 flagPtr;
    struct PrimarySpriteData *sprite;
    u32 directionPtr;

    directionPtr = (u32)&gCuckooCondorMoveRight;
    temp = *(u8 *)directionPtr;
    if (temp != 0)
        goto moveRight;

    sprite = &gCurrentSprite;
    temp = *(volatile u16 *)&sprite->yPosition;
    value = sprite->xPosition;
    value -= 0x3C;
    value = (u16)value;
    func_8023BFC(temp, value);
    temp = gSpriteCollisionTileType;
    if (temp == 0x11) {
        temp = 1;
        *(u8 *)directionPtr = temp;
        goto end;
    }
    temp = sprite->xPosition;
    temp -= 2;
    sprite->xPosition = temp;
    temp = (u32)&gCuckooCondorHasCapturedWario;
    value = *(u8 *)temp;
    flagPtr = temp;
    if (value == 0)
        goto updateTimer;
    value = (u32)&gWarioData;
    temp = *(u16 *)(value + 0x12);
    temp -= 2;
    goto storeWarioX;

moveRight:
    sprite = &gCurrentSprite;
    temp = *(volatile u16 *)&sprite->yPosition;
    value = sprite->xPosition;
    value += 0x3C;
    value = (u16)value;
    func_8023BFC(temp, value);
    temp = gSpriteCollisionTileType;
    if (temp == 0x11) {
        temp = 0;
        *(u8 *)directionPtr = temp;
        goto end;
    }
    temp = sprite->xPosition;
    temp += 2;
    sprite->xPosition = temp;
    temp = (u32)&gCuckooCondorHasCapturedWario;
    value = *(u8 *)temp;
    flagPtr = temp;
    if (value == 0)
        goto updateTimer;
    value = (u32)&gWarioData;
    temp = *(u16 *)(value + 0x12);
    temp += 2;

storeWarioX:
    *(u16 *)(value + 0x12) = temp;

updateTimer:
    temp = (u32)&gCurrentSprite;
    ASM_BARRIER(: "+r"(temp));
    value = temp;
    value += 0x29;
    temp = *(u8 *)value;
    if (temp != 0) {
        temp--;
        goto storeTimer;
    }
    temp = *(u8 *)flagPtr;
    if (temp == 0) {
        TryStartCuckooCondorGrab();
        goto end;
    }
    temp = 0x3C;
storeTimer:
    *(u8 *)value = temp;
end:
    return;
}

void CuckooCondorLowerPendulum(void)
{
    u32 value;
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 temp;
    register u32 base PIN("r1");
    u32 slot;
    register u32 base3 PIN("r3");

    if (gCuckooCondorHasCapturedWario != 0)
        goto setDone;

    value = *(u8 *)&gBossState;
    if (value != 0) {
        gCurrentSprite.pose = 0x11;
        return;
    }

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    temp = *timer;
    if (temp != 0) {
        temp--;
        *timer = temp;
        temp <<= 24;
        if (temp != 0)
            return;

        temp = SpriteUtilFindSprite(0xE8, 0);
        slot = temp;
        base = (u32)gSpriteData;
        temp = sizeof(struct PrimarySpriteData);
        temp *= slot;
        temp += base;
        base = 0x43;
        ((struct PrimarySpriteData *)temp)->warioCollision = base;
        temp = SOUND_A4;
        m4aSongNumStart(temp);
    }

    base = (u32)&gCuckooCondorPendulumLength;
    temp = *(u8 *)base;
    if (temp <= 0xBC) {
        temp += 0x10;
        *(u8 *)base = temp;
        return;
    }

    temp = 0x6F;
    sprite->pose = temp;
    temp = 0x3C;
    *timer = temp;
    temp = 0xBC;
    *(u8 *)base = temp;

    slot = SpriteUtilFindSprite(0xE8, 0);
    base3 = (u32)gSpriteData;
    temp = sizeof(struct PrimarySpriteData);
    slot *= temp;
    temp = base3 + 4;
    temp = (slot) - (-(temp));
    base = (u32)sCuckooCondorPendulumRetractedOam;
    *(u32 *)temp = base;
    slot += base3;
    *(u8 *)(slot + 0x16) = value;
    *(u16 *)(slot + 0x14) = value;
    return;

setDone:
    temp = (u32)&gCurrentSprite;
    slot = 0;
    base = 0x6F;
    *(u8 *)(temp + 0x1C) = base;
    temp += 0x29;
    *(u8 *)temp = slot;
}

void CuckooCondorRaisePendulum(void)
{
    u8 *flag;
    u32 zero;
    u32 timer;
    s32 slot;
    register u32 value PIN("r0");
    u32 temp;
    u32 offset;
    register struct PrimarySpriteData *other PIN("r3");
    u8 *timerPtr;
    u8 *verticalOffset;
    struct PrimarySpriteData *sprite;

    value = (u32)&gCuckooCondorHasCapturedWario;
    temp = *(u8 *)value;
    flag = (u8 *)value;
    if (temp == 0) {
        zero = (u8)gBossState;
        if (zero != 0) {
            gCurrentSprite.pose = 0x11;
            return;
        }

        sprite = &gCurrentSprite;
        timerPtr = &sprite->work2;
        value = *timerPtr;
        if (value != 0) {
            temp = value - 1;
            *timerPtr = temp;
            value = temp;
            timer = (u8)value;
            if (timer != 0) {
                value = timer;
                ASM_BARRIER(: "+r"(value));
                if (value != 0x30)
                    return;
                slot = SpriteUtilFindSprite(0xE8, 0);
                temp = (u32)gSpriteData;
                value = sizeof(struct PrimarySpriteData);
                offset = slot;
                offset *= value;
                other = (struct PrimarySpriteData *)(offset + temp);
                value = 0x44;
                other->warioCollision = value;
                temp += 4;
                offset += temp;
                value = (u32)sCuckooCondorClawCloseOam;
                *(u32 *)offset = value;
                other->currentAnimationFrame = zero;
                value = 1;
                other->animationTimer = value;
                return;
            }

            slot = SpriteUtilFindSprite(0xE8, 0);
            other = (struct PrimarySpriteData *)gSpriteData;
            value = sizeof(struct PrimarySpriteData);
            offset = slot;
            offset *= value;
            value = (u32)other + 4;
            value = offset + value;
            temp = (u32)sCuckooCondorClawIdleOam;
            *(u32 *)value = temp;
            offset += (u32)other;
            *(u8 *)(offset + 0x16) = timer;
            *(u16 *)(offset + 0x14) = timer;
            value = 0x42;
            *(u8 *)(offset + 0x1E) = value;
            return;
        }
    }

    verticalOffset = &gCuckooCondorPendulumLength;
    value = *verticalOffset;
    if (value > 0x10) {
        value -= 8;
        *verticalOffset = value;
        value = *flag;
        if (value != 0) {
            slot = SpriteUtilFindSprite(0xE8, 0);
            gWarioData.yPosition = gSpriteData[slot].yPosition - gWarioData.hitboxOffsetTop;
        }
    } else {
        timer = *flag;
        if (timer == 0) {
            value = (u32)&gCurrentSprite;
            offset = 0x10;
            ((struct PrimarySpriteData *)value)->pose = offset;
            value += 0x29;
            temp = 0x3C;
            *(u8 *)value = temp;
            *verticalOffset = offset;
            slot = SpriteUtilFindSprite(0xE8, 0);
            other = (struct PrimarySpriteData *)gSpriteData;
            value = sizeof(struct PrimarySpriteData);
            offset = slot;
            offset *= value;
            value = (u32)other + 4;
            value = offset + value;
            temp = (u32)sCuckooCondorClawOpenOam;
            *(u32 *)value = temp;
            offset += (u32)other;
            *(u8 *)(offset + 0x16) = timer;
            *(u16 *)(offset + 0x14) = timer;
            m4aSongNumStart(SOUND_A2);
        } else {
            value = (u32)&gCurrentSprite;
            offset = 0x10;
            ((struct PrimarySpriteData *)value)->pose = offset;
            value += 0x29;
            temp = 0x3C;
            *(u8 *)value = temp;
            *verticalOffset = offset;
        }
    }
}

void CuckooCondorSinkAfterHit(void)
{
    u16 difference;

    difference = (gCurrentSprite.yPosition >> 2) - (gBg1YPosition >> 2);
    if (difference <= 0x4F)
        gCurrentSprite.yPosition += 8;
    else {
        gCurrentSprite.pose = 0x12;
        gCurrentSprite.work2 = 0x30;
    }
}

void CuckooCondorHandleClockDamage(void)
{
    register u32 reg6 PIN("r6");
    u32 timer;
    u32 zero;
    u32 pose41;
    u32 savedTimer;
    register u8 *savedTimerPtr PIN("r10");
    u32 raw;
    register u32 temp PIN("r1");
    u32 mask;
    u8 *workPtr;
    u8 *laterPtr;
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u32 stride;
    s32 slot;

    raw = (u32)&gCurrentSprite;
    workPtr = (u8 *)raw;
    workPtr += 0x29;
    mask = *workPtr;
    reg6 = raw;
    if (mask != 0) {
        raw = mask - 1;
        *workPtr = raw;
        timer = (u8)raw;
        if (timer <= 0x20) {
            zero = 7;
            zero &= timer;
            if (zero == 0) {
                raw = *(u8 *)(reg6 + 0x1D);
                if (raw != 0 && timer == 0x20) {
                    raw--;
                    *(u8 *)(reg6 + 0x1D) = raw;
                    UpdateBossHealthGauge();
                    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(reg6 + 4), sCuckooCondorHurtOam);
                    *(u8 *)(reg6 + 0x16) = zero;
                    *(u16 *)(reg6 + 0x14) = zero;
                    VoiceSetPlay(13);
                }

                raw = 8;
                raw &= timer;
                mask = (u8)raw;
                if (mask != 0) {
                    temp = (u32)&gCurrentSprite;
                    raw = 3;
                    ((struct PrimarySpriteData *)temp)->palette = raw;
                    reg6 = temp;
                } else {
                    raw = (u32)&gCurrentSprite;
                    *(u8 *)(raw + 0x1B) = mask;
                    reg6 = raw;
                }
            }
        }
    }

    current = (struct PrimarySpriteData *)reg6;
    temp = current->health;
    raw = (u32)&gBgAnimationFrame;
    raw = *(u8 *)raw;
    raw >>= 1;
    raw++;
    if ((s32)temp < (s32)raw) {
        raw = 0x29;
        raw += (u32)current;
        savedTimerPtr = (u8 *)raw;
        temp = *savedTimerPtr;
        savedTimer = temp;
        if (temp != 0)
            return;
        {
            slot = SpriteUtilFindSpriteSlot(0xE6);
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            raw = (u32)&sprites[slot];
            reg6 = 0x27;
            *(u8 *)(raw + 0x1C) = reg6;

            slot = SpriteUtilFindSprite(0xE7, 4);
            sprites[slot].pose = reg6;
            slot = SpriteUtilFindSprite(0xE7, 3);
            raw = (u32)&sprites[slot];
            pose41 = 0x29;
            *(u8 *)(raw + 0x1C) = pose41;
            slot = SpriteUtilFindSprite(0xE7, 2);
            sprites[slot].pose = reg6;
            slot = SpriteUtilFindSprite(0xE7, 1);
            sprites[slot].pose = reg6;
            slot = SpriteUtilFindSprite(0xE7, 0);
            sprites[slot].pose = pose41;
            slot = SpriteUtilFindSprite(0xE8, 4);
            sprites[slot].pose = pose41;
            slot = SpriteUtilFindSprite(0xE8, 3);
            sprites[slot].pose = reg6;
            slot = SpriteUtilFindSprite(0xE8, 2);
            sprites[slot].pose = pose41;
            slot = SpriteUtilFindSprite(0xE8, 1);
            sprites[slot].pose = reg6;
            slot = SpriteUtilFindSprite(0xE8, 0);
            sprites[slot].pose = pose41;

            current->pose = 0x13;
            temp = savedTimer;
            raw = (u32)savedTimerPtr;
            *(u8 *)raw = temp;
            m4aSongNumStart(SOUND_AA);
            VoiceSetPlay(0);
            return;
        }
    }

    laterPtr = (u8 *)current;
    laterPtr += 0x29;
    raw = *laterPtr;
    if (raw <= 0x10) {
        mask = current->yPosition;
        raw = mask >> 2;
        temp = (u32)&gBg1YPosition;
        temp = *(u16 *)temp;
        temp >>= 2;
        raw -= temp;
        raw = (u16)raw;
        if (raw > 0x24) {
            raw = mask - 4;
            current->yPosition = raw;
        } else {
            raw = 0;
            temp = 0x6F;
            *(u8 *)(reg6 + 0x1C) = temp;
            *laterPtr = raw;
            temp = (u32)sCuckooCondorClockIdleOam;
            *(u32 *)(reg6 + 4) = temp;
            *(u8 *)(reg6 + 0x16) = raw;
            *(u16 *)(reg6 + 0x14) = raw;
        }
    }
}

void CuckooCondorBreakClock(void)
{
    struct PrimarySpriteData *sprite;
    u8 *counter;
    u32 value;

    sprite = &gCurrentSprite;
    value = 0x29;
    counter = (u8 *)sprite + value;
    switch (*counter) {
    case 0:
        gSwitchStates[2] = 2;
        SpriteSpawnAsChild(0x0A, 0, 0, sprite->yPosition,
                           sprite->xPosition - 0x20);
        SpriteSpawnAsChild(0x0A, 0, 0, sprite->yPosition + 0xA0,
                           sprite->xPosition + 0x20);
        break;
    case 2:
        SpriteSpawnAsChild(0x0B, 0, 0, sprite->yPosition,
                           sprite->xPosition + 0x50);
        SpriteSpawnAsChild(0x0B, 0, 0, sprite->yPosition + 0x8C,
                           sprite->xPosition - 0x50);
        break;
    case 6:
        sprite->status &= 0xFBFF;
        sprite->hitboxExtentUp = 0x70;
        sprite->hitboxExtentDown = 0x38;
        sprite->hitboxExtentLeft = 0x28;
        sprite->hitboxExtentRight = 0x28;
        GP_SET(sprite->pOamData, sCuckooCondorBodyOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->warioCollision = 0x47;
        sprite->work3 = 0;
        sprite->pose = 0x14;
        *counter = 0x4D;
        break;
    }
    gCurrentSprite.work2++;
}

void CuckooCondorFlyAndBob(void)
{
    register u32 spriteBase PIN("r2");
    register u32 workPtr PIN("r12");
    u32 index;
    u32 table;
    u32 delta;
    u32 sprite;
    u32 temp;
    u32 value;
    register u32 x PIN("r2");

    spriteBase = (u32)&gCurrentSprite;
    temp = 0x2A;
    ASM_BARRIER(: "+r"(temp));
    temp += spriteBase;
    workPtr = temp;
    index = *(u8 *)temp;
    table = (u32)sCuckooCondorFlightYVelocity;
    temp = index << 1;
    temp += table;
    delta = *(u16 *)temp;
    sprite = 0;
    value = *(s16 *)(temp + sprite);
    temp = 0x7FFF;
    sprite = spriteBase;
    if ((s32)value == (s32)temp) {
        delta = *(u16 *)table;
        index = 0;
    }

    temp = index + 1;
    value = workPtr;
    *(u8 *)value = temp;
    temp = ((struct PrimarySpriteData *)sprite)->yPosition;
    temp += delta;
    ((struct PrimarySpriteData *)sprite)->yPosition = temp;

    value = sprite;
    value += 0x29;
    temp = *(u8 *)value;
    temp--;
    *(u8 *)value = temp;
    temp <<= 24;
    if (temp == 0) {
        temp = 21;
        ((struct PrimarySpriteData *)sprite)->pose = temp;
        temp = ((struct PrimarySpriteData *)sprite)->health;
        if (temp <= 1) {
            temp = 12;
            *(u8 *)value = temp;
            return;
        }
        temp = 25;
        *(u8 *)value = temp;
        return;
    }

    value = *(u16 *)sprite;
    temp = 0x40;
    temp &= value;
    if (temp != 0) {
        temp = ((struct PrimarySpriteData *)sprite)->health;
        if (temp <= 1) {
            temp = ((struct PrimarySpriteData *)sprite)->xPosition;
            temp += 2;
            ((struct PrimarySpriteData *)sprite)->xPosition = temp;
            temp = ((struct PrimarySpriteData *)sprite)->currentAnimationFrame;
            temp++;
            ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = temp;
        } else {
            temp = ((struct PrimarySpriteData *)sprite)->xPosition;
            temp++;
            ((struct PrimarySpriteData *)sprite)->xPosition = temp;
        }
        temp = ((volatile struct PrimarySpriteData *)sprite)->yPosition;
        x = 0x80;
        x <<= 1;
        ASM_BARRIER(: "+r"(x));
        value = x;
        x = ((struct PrimarySpriteData *)sprite)->xPosition;
        value += x;
        value = (u16)value;
        func_8023BFC(temp, value);
        temp = *(u8 *)&gSpriteCollisionTileType;
        if (temp != 0x11)
            return;
        value = *(u16 *)sprite;
        temp = 0xFFBF;
        temp &= value;
    } else {
        temp = ((struct PrimarySpriteData *)sprite)->health;
        if (temp <= 1) {
            temp = ((struct PrimarySpriteData *)sprite)->xPosition;
            temp -= 2;
            ((struct PrimarySpriteData *)sprite)->xPosition = temp;
            temp = ((struct PrimarySpriteData *)sprite)->currentAnimationFrame;
            temp++;
            ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = temp;
        } else {
            temp = ((struct PrimarySpriteData *)sprite)->xPosition;
            temp--;
            ((struct PrimarySpriteData *)sprite)->xPosition = temp;
        }
        temp = ((volatile struct PrimarySpriteData *)sprite)->yPosition;
        x = (u32)-0x100;
        ASM_BARRIER(: "+r"(x));
        value = x;
        x = ((struct PrimarySpriteData *)sprite)->xPosition;
        value += x;
        value = (u16)value;
        func_8023BFC(temp, value);
        temp = *(u8 *)&gSpriteCollisionTileType;
        if (temp != 0x11)
            return;
        value = *(u16 *)sprite;
        temp = 0x40;
        temp |= value;
    }
    *(u16 *)sprite = temp;
}

void CuckooCondorLayEgg(void)
{
    struct PrimarySpriteData *sprite = &gCurrentSprite;
    u32 index = sprite->work3;
    s16 delta = sCuckooCondorFlightYVelocity[index];
    if (delta == 0x7FFF) {
        delta = sCuckooCondorFlightYVelocity[0];
        index = 0;
    }
    sprite->work3 = index + 1;
    sprite->yPosition += delta;
    if (sprite->health <= 4)
        sprite->currentAnimationFrame++;
    if (--sprite->work2 != 0)
        return;

    sprite->pose = 20;
    if (sprite->health <= 4)
        sprite->work2 = 0x2F;
    else
        sprite->work2 = 0x4D;

    if (sprite->status & 0x40)
        SpawnPrimarySpriteWithStatus(0xE9, 0, 0, sprite->yPosition + 0x5C, sprite->xPosition - 0x10, 0);
    else
        SpawnPrimarySpriteWithStatus(0xE9, 0, 0, sprite->yPosition + 0x5C, sprite->xPosition + 0x10, 0x40);
}

void CuckooCondorHandleCondorDamage(void)
{
    struct PrimarySpriteData *sprite;
    register u8 *timer PIN("r12");
    register u32 value PIN("r0");
    u32 mask;
    u32 work;
    s32 other;
    s32 end;
    register u32 one PIN("r5");
    u32 pose;

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer;
    mask = value;
    if (mask != 0) {
        value--;
        *timer = value;
        mask = 0xFF;
        work = (u8)value;
        if (work > 0x0F && (work & 7) == 0) {
            value = sprite->health;
            if (value != 0 && work == 0x30) {
                value--;
                sprite->health = value;
                value &= mask;
                if (value == 0) {
                    gTimerState = 11;
                    SpriteUtilSetWarioBossVictoryPose();
                    other = (s32)gSpriteData;
                    value = 0xFD;
                    value <<= 2;
                    end = other + value;
                    one = 1;
                    pose = 0x6B;
                    do {
                        value = ((struct PrimarySpriteData *)other)->globalID;
                        value += 23;
                        value = (u8)value;
                        if (value <= 1) {
                            mask = *(u16 *)other;
                            value = one;
                            value &= mask;
                            if (value != 0)
                                ((struct PrimarySpriteData *)other)->pose = pose;
                        }
                        other += sizeof(struct PrimarySpriteData);
                    } while (other <= end);
                }
                UpdateBossHealthGauge();
                VoiceSetPlay(13);
            }
            value = work & 8;
            mask = (u8)value;
            if (mask != 0) {
                struct PrimarySpriteData *current;
                current = &gCurrentSprite;
                value = 2;
                current->palette = value;
                if (work == 0x28)
                    VoiceSetPlay(1);
            } else {
                struct PrimarySpriteData *current;
                current = &gCurrentSprite;
                current->palette = mask;
            }
        }
    } else {
        value = sprite->health;
        pose = value;
        if (pose == 0) {
            value = 0x78;
            sprite->pose = value;
            *timer = value;
            m4aSongNumStart(SOUND_A1);
        } else {
            GP_SET(sprite->pOamData, sCuckooCondorBodyOam);
            sprite->currentAnimationFrame = mask;
            sprite->animationTimer = mask;
            sprite->pose = 20;
            if (pose <= 4)
                value = 0x2F;
            else
                value = 0x4D;
            *timer = value;
        }
    }
}

void CuckooCondorExplode(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *base;
    u8 *timer;
    register u32 value PIN("r0");

    base = &gCurrentSprite;
    timer = &base->work2;
    value = *timer;
    value--;
    *timer = value;
    value = *timer;
    sprite = base;
    switch (value) {
    case 110:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0xC0,
                     sprite->xPosition);
        break;
    case 107:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0xC0,
                     sprite->xPosition - 0x40);
        break;
    case 103:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0xC0,
                     sprite->xPosition + 0x48);
        break;
    case 100:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0x80,
                     sprite->xPosition - 0x20);
        break;
    case 97:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0x80,
                     sprite->xPosition - 0x48);
        break;
    case 93:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0x80,
                     sprite->xPosition + 0x40);
        break;
    case 90:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0x40,
                     sprite->xPosition + 0x20);
        break;
    case 87:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0x40,
                     sprite->xPosition - 0x40);
        break;
    case 83:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition - 0x40,
                     sprite->xPosition + 0x48);
        break;
    case 80:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition,
                     sprite->xPosition);
        break;
    case 77:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition,
                     sprite->xPosition - 0x48);
        break;
    case 73:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition,
                     sprite->xPosition + 0x60);
        break;
    case 70:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition + 0x40,
                     sprite->xPosition - 0x20);
        break;
    case 67:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition + 0x40,
                     sprite->xPosition - 0x60);
        break;
    case 63:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition + 0x40,
                     sprite->xPosition + 0x40);
        break;
    case 60:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition + 0x80,
                     sprite->xPosition - 0x20);
        break;
    case 57:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition + 0x80,
                     sprite->xPosition - 0x60);
        break;
    case 53:
        SpawnHighPriorityPrimarySprite(0x0B, 0, 0, sprite->yPosition + 0x80,
                     sprite->xPosition + 0x40);
        break;
    case 50:
        SpawnHighPriorityPrimarySprite(0x0A, 0, 0, sprite->yPosition + 0x60,
                     sprite->xPosition + 0x60);
        break;
    case 40:
        SpawnHighPriorityPrimarySprite(0x0A, 0, 0, sprite->yPosition,
                     sprite->xPosition);
        break;
    case 30:
        SpawnHighPriorityPrimarySprite(0x0A, 0, 0, sprite->yPosition - 0x40,
                     sprite->xPosition - 0x40);
        break;
    case 20:
        SpawnHighPriorityPrimarySprite(0x0A, 0, 0, sprite->yPosition + 0x80,
                     sprite->xPosition - 0x20);
        break;
    case 24:
        SpawnHighPriorityPrimarySprite(0x0A, 0, 0, sprite->yPosition + 0x60,
                     sprite->xPosition + 0x20);
        break;
    case 0:
        SpawnHighPriorityPrimarySprite(0xCF, 0, 0, sprite->yPosition,
                     sprite->xPosition);
        sprite->status = 0;
        break;
    }
}

void CuckooCondorTakeShopItemHit(void)
{
    struct PrimarySpriteData *sprite;
    u8 timer;
    switch (gCurrentShopItem) {
    case 2: timer = 12; break;
    case 3: timer = 11; break;
    case 4: timer = 10; break;
    case 5: timer = 5; break;
    case 6: timer = 6; break;
    case 7: timer = 2; break;
    case 8: timer = 7; break;
    default: timer = 12; break;
    }
    sprite = &gCurrentSprite;
    sprite->work2 = timer;
    GP_SET(sprite->pOamData, sCuckooCondorHurtOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    gBossSequenceState = 0x20;
    gInitialHealth = 0x21;
    sprite->pose = 0x7B;
    if (sprite->health != 0) {
        sprite->health--;
        UpdateBossHealthGauge();
        m4aSongNumStart(SE_DAMAGE);
    }
    VoiceSetPlay(13);
}

void CuckooCondorHandleShopItemHit(void)
{
    u32 timerPtr;
    u32 temp;
    register u32 value PIN("r1");
    u32 health;
    register u32 sprite PIN("r3");
    register u32 healthCopy PIN("r4");

    timerPtr = (u32)&gBossSequenceState;
    temp = *(u8 *)timerPtr;
    value = temp;
    if (value != 0) {
        temp--;
        *(u8 *)timerPtr = temp;
        temp <<= 24;
        value = temp >> 24;
        temp = 7;
        temp &= value;
        if (temp != 0)
            return;

        temp = 8;
        temp &= value;
        ASM_BARRIER(: "+r"(temp));
        temp <<= 24;
        value = temp >> 24;
        if (value != 0) {
            value = (u32)&gCurrentSprite;
            temp = 3;
            ((struct PrimarySpriteData *)value)->palette = temp;
            return;
        }
        temp = (u32)&gCurrentSprite;
        ((struct PrimarySpriteData *)temp)->palette = value;
        return;
    }

    sprite = (u32)&gCurrentSprite;
    temp = sprite;
    temp += 0x29;
    health = ((struct PrimarySpriteData *)sprite)->health;
    healthCopy = health;
    temp = *(u8 *)temp;
    if (healthCopy == temp) {
        temp = (u32)&gBgAnimationFrame;
        *(u8 *)temp = health;
        temp = 3;
        ((struct PrimarySpriteData *)sprite)->pose = temp;
        temp = (u32)sCuckooCondorClockIdleOam;
        GP_SET(((struct PrimarySpriteData *)sprite)->pOamData, (const struct AnimationFrame *)temp);
        ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = value;
        ((struct PrimarySpriteData *)sprite)->animationTimer = value;
        SpriteUtilStartBossTimer();
        temp = SOUND_A2;
        m4aSongNumStart(temp);
        return;
    }

    value = 0x20;
    *(u8 *)timerPtr = value;
    temp = (u32)&gInitialHealth;
    *(u8 *)temp = value;
    if (healthCopy == 0)
        return;
    temp = health - 1;
    ((struct PrimarySpriteData *)sprite)->health = temp;
    UpdateBossHealthGauge();
    temp = SE_DAMAGE;
    m4aSongNumStart(temp);
}

void InitCuckooCondorClockPiece(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    struct PrimarySpriteData *ptr;
    register u32 value PIN("r0");
    u32 status;
    register u32 zeroByte PIN("r2");
    u32 zeroHalf;
    u8 *bytePtr;
    u8 *bytePtr2;

    sprite = &gCurrentSprite;
    status = sprite->status;
    zeroByte = 0x8408;
    ASM_BARRIER(: "+r"(zeroByte));
    value = zeroByte;
    zeroByte = 0;
    zeroHalf = 0;
    value |= status;
    value &= 0xFFFB;
    bytePtr2 = (u8 *)sprite;
    *(u16 *)bytePtr2 = value;
    *(bytePtr2 + 0x1E) = zeroByte;

    bytePtr = (u8 *)sprite + 0x23;
    status = 4;
    *bytePtr = status;
    bytePtr++;
    *bytePtr = status;
    bytePtr++;
    *bytePtr = status;
    bytePtr++;
    *(volatile u8 *)bytePtr = status;

    bytePtr = (u8 *)sprite;
    *(bytePtr + 0x16) = zeroByte;
    *(u16 *)(bytePtr + 0x14) = zeroHalf;
    value = 0x10;
    bytePtr2 = (u8 *)sprite;
    *(bytePtr2 + 0x1C) = value;
    value = *(bytePtr2 + 0x18);
    ptr = sprite;

    switch (value) {
    case 0:
        value = (u32)sCuckooCondorClockPieceLowerLeftOam;
        goto commonSmall;
    case 1:
        value = (u32)sCuckooCondorClockPieceUpperLeftOam;
        GP_SET(ptr->pOamData, (const struct AnimationFrame *)value);
        bytePtr = (u8 *)ptr;
        bytePtr += 0x20;
        status = 0x30;
        *bytePtr = status;
        bytePtr++;
        *(volatile u8 *)bytePtr = status;
        bytePtr2 = (u8 *)ptr;
        bytePtr2 += 0x22;
        value = 0x18;
        *bytePtr2 = value;
        bytePtr2 += 6;
        value = 0x3C;
        *bytePtr2 = value;
        break;
    case 2:
        value = (u32)sCuckooCondorClockPieceLowerRightOam;
commonSmall:
        GP_SET(ptr->pOamData, (const struct AnimationFrame *)value);
        bytePtr = (u8 *)ptr;
        bytePtr += 0x20;
        status = 0x10;
        *bytePtr = status;
        bytePtr++;
        *(volatile u8 *)bytePtr = status;
        bytePtr2 = (u8 *)ptr;
        bytePtr2 += 0x22;
        value = 0x18;
        *bytePtr2 = value;
        break;
    case 3:
        value = (u32)sCuckooCondorClockPieceUpperRightOam;
        GP_SET(ptr->pOamData, (const struct AnimationFrame *)value);
        bytePtr = (u8 *)ptr;
        bytePtr += 0x20;
        status = 0x10;
        *bytePtr = status;
        bytePtr++;
        *bytePtr = status;
        bytePtr++;
        *(volatile u8 *)bytePtr = status;
        break;
    case 4:
        value = (u32)sCuckooCondorClockPieceTopOam;
        GP_SET(ptr->pOamData, (const struct AnimationFrame *)value);
        bytePtr2 = (u8 *)ptr;
        bytePtr2 += 0x20;
        value = 0x10;
        *bytePtr2 = value;
        bytePtr2++;
        *bytePtr2 = value;
        bytePtr2++;
        *bytePtr2 = value;
        status = ptr->status;
        value = 0x80;
        zeroByte = 0;
        value |= status;
        ptr->status = value;
        bytePtr = (u8 *)ptr + 0x27;
        *bytePtr = zeroByte;
        bytePtr2 = (u8 *)ptr + 0x28;
        value = 0x3C;
        *bytePtr2 = value;
        bytePtr = (u8 *)ptr + 0x29;
        *bytePtr = zeroByte;
        break;
    }
    SpriteUtilFindSpriteSlotWork3(0x18);
}

void UpdateCuckooCondorClockPiece(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *parent;
    u32 value;

    sprite = &gCurrentSprite;
    if (sprite->roomSlot == 1)
        goto room1;
    if (sprite->roomSlot == 4)
        goto room4;
    goto end;

room1:
    sprites = gSpriteData;
    parent = &sprites[sprite->work3];
    if (parent->pose <= 15)
        goto end;
    if (gTimerState != 1)
        goto end;
    if (parent->health > 10)
        goto end;
    if (gCuckooCondorHasCapturedWario != 0)
        goto end;
    if (*(u8 *)&gBossState != 0)
        goto end;
    if (sprite->work1 != 0) {
        sprite->work1--;
        goto end;
    }
    func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 0x20));
    if (gSpriteCollisionTileType == 0x11)
        goto end;
    value = SpriteUtilFindSprite(0xE7, 4);
    if (sprites[value].pose != 16)
        goto end;
    sprite->pose = 0x6E;
    goto end;

room4:
    sprite->work0--;
    sprite->work2 = 0;
    sprites = gSpriteData;
    parent = &sprites[sprite->work3];
    if (parent->pose <= 15)
        goto end;
    if (gTimerState != 1)
        goto end;
    if (parent->health > 12)
        goto end;
    if (gCuckooCondorHasCapturedWario != 0)
        goto end;
    if (*(u8 *)&gBossState != 0)
        goto end;
    if (sprite->work1 != 0) {
        sprite->work1--;
        goto end;
    }
    func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 0x80));
    if (gSpriteCollisionTileType == 0x11)
        goto end;
    value = SpriteUtilFindSprite(0xE7, 1);
    if (sprites[value].pose != 16)
        goto end;
    sprite->pose = 0x11;
end:;
}

void CuckooCondorClockPieceStartSpin(void)
{
    gCurrentSprite.pose = 0x12;
    gCurrentSprite.work1 = 0x3C;
    m4aSongNumStart(SOUND_A8);
}

void CuckooCondorClockPieceSpinUp(void)
{
    struct PrimarySpriteData *baseSprite;
    struct PrimarySpriteData *sprite;
    u8 *timer;
    register u32 value PIN("r1");
    register u32 newValue PIN("r2");
    u8 *work0;
    register u32 temp PIN("r0");
    u32 y;

    baseSprite = &gCurrentSprite;
    timer = &baseSprite->work1;
    value = *timer;
    newValue = value - 1;
    *timer = newValue;
    value = newValue << 24;
    sprite = baseSprite;
    if (value != 0) {
        temp = value;
        ASM_BARRIER(: "+r"(temp));
        temp >>= 24;
        if (temp > 0x32) {
            work0 = &sprite->work0;
            temp = *work0;
            temp -= 2;
            goto storeWork0;
        }
        if (temp > 0x23) {
            work0 = &sprite->work0;
            temp = *work0;
            temp -= 4;
            goto storeWork0;
        }
        if (temp > 20) {
            work0 = &sprite->work0;
            temp = *work0;
            temp -= 8;
            goto storeWork0;
        }
        work0 = &sprite->work0;
        temp = *work0;
        temp -= 16;
storeWork0:
        *work0 = temp;
        return;
    }

    if (gCuckooCondorHasCapturedWario == 0 && *(u8 *)&gBossState == 0) {
        temp = *(volatile u8 *)&sprite->work0;
        y = sprite->yPosition;
        SpriteSpawnAsChild(0xC8, temp, 0, y, sprite->xPosition);
        sprite->pose = 19;
        *timer = 0xF0;
    } else {
        sprite->pose = 0x10;
        work0 = &sprite->work1;
        temp = 0x3C;
        *work0 = temp;
    }
}

void CuckooCondorClockPieceFlash(void)
{
    struct PrimarySpriteData *sprite;
    u16 status;
    register u32 four PIN("r4");
    register u32 value PIN("r0");
    u8 *timer;

    sprite = &gCurrentSprite;
    value = sprite->status;
    four = 4;
    status = four;
    status |= value;
    sprite->status = status;
    timer = &sprite->work1;
    value = *timer;
    value--;
    *timer = value;
    value <<= 24;
    if (value == 0) {
        value = 0xFFFB;
        status &= value;
        sprite->status = status;
        value = 0x14;
        sprite->pose = value;
        {
            u8 *timer2;
            timer2 = &sprite->work2;
            *timer2 = four;
        }
        m4aSongNumStart(SOUND_A8);
    }
}

void CuckooCondorClockPieceSpin(void)
{
    gCurrentSprite.work2 += 4;
    if (gCurrentSprite.work2 > 0xFB) {
        gCurrentSprite.pose = 0x10;
        gCurrentSprite.work1 = 0xF0;
    }
}

void CuckooCondorClockPieceStartDebrisDrop(void)
{
    GP_SET(gCurrentSprite.pOamData, sCuckooCondorClockPieceUpperLeftWindupOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 0x6F;
    gCurrentSprite.work0 = 0;
    m4aSongNumStart(SOUND_D0);
}

void CuckooCondorClockPieceDropDebris(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;
    u32 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    value++;
    zero = 0;
    *timer = value;
    value = *timer;

    if (value == 0x4C)
        goto spawnCheck;
    if ((s32)value > 0x4C)
        goto greater;
    if (value == 0x3C)
        goto case3c;
    if (value == 0x40)
        goto case40;
    goto end;

greater:
    if (value == 0x50)
        goto finish;
    goto end;

case3c:
    value = (u32)sCuckooCondorClockPieceUpperLeftDrop1Oam;
    goto setAnim;
case40:
    value = (u32)sCuckooCondorClockPieceUpperLeftDrop2Oam;
    goto setAnim;

spawnCheck:
    func_8023BFC(sprite->yPosition + 0x80, sprite->xPosition);
    if (gSpriteCollisionTileType != 0x11) {
        SpriteSpawnAsChild(0xBD, 0, 0, sprite->yPosition + 0x80, sprite->xPosition);
    }
    value = (u32)sCuckooCondorClockPieceUpperLeftOam;
setAnim:
    GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    goto end;

finish:
    value = (u32)sCuckooCondorClockPieceUpperLeftOam;
    GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    value = 0x10;
    sprite->pose = value;
    timer = (u8 *)sprite;
    timer += 0x28;
    value = 0xB4;
    *timer = value;
end:
    ;
}

void CuckooCondorClockPieceFallRight(void)
{
    gCurrentSprite.status &= 0x7F77;
    gCurrentSprite.pose = 0x28;
    gCurrentSprite.work2 = 2;
    gCurrentSprite.work3 = 0;
}

void CuckooCondorClockPieceFallLeft(void)
{
    gCurrentSprite.status &= 0x7F77;
    gCurrentSprite.pose = 0x2A;
    gCurrentSprite.work2 = 2;
    gCurrentSprite.work3 = 0;
}

void InitCuckooCondorPendulumPart(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    u32 status;
    register u32 zero PIN("r2");
    u8 *ptr;

    sprite = &gCurrentSprite;
    status = sprite->status;
    zero = 0x8408;
    ASM_BARRIER(: "+r"(zero));
    value = zero;
    zero = 0;
    value |= status;
    value &= 0xFFFB;
    sprite->status = value;
    sprite->warioCollision = zero;
    ptr = (u8 *)sprite + 0x23;
    value = 0x20;
    *ptr = value;
    ptr++;
    value = 0x28;
    *ptr = value;
    ptr++;
    value = 0x40;
    *ptr = value;
    ptr++;
    value = 0x3C;
    *ptr = value;
    sprite->currentAnimationFrame = zero;
    value = 0x10;
    sprite->pose = value;
    value = sprite->roomSlot;

    switch (value) {
    case 0:
        GP_SET(sprite->pOamData, sCuckooCondorClawIdleOam);
        ptr = (u8 *)sprite + 0x20;
        zero = 0;
        value = 0x20;
        *ptr = value;
        ptr++;
        *ptr = value;
        ptr++;
        *ptr = value;
        sprite->animationTimer = zero;
        value = 0x42;
        sprite->warioCollision = value;
        break;
    case 1:
        GP_SET(sprite->pOamData, sCuckooCondorPendulumLinkOam);
        ptr = (u8 *)sprite + 0x20;
        zero = 0;
        value = 8;
        *ptr = value;
        ptr++;
        *ptr = value;
        ptr++;
        *ptr = value;
        sprite->animationTimer = zero;
        break;
    case 2:
        GP_SET(sprite->pOamData, sCuckooCondorPendulumLinkOam);
        ptr = (u8 *)sprite + 0x20;
        value = 8;
        *ptr = value;
        ptr++;
        *ptr = value;
        ptr++;
        *ptr = value;
        value = 1;
        sprite->animationTimer = value;
        break;
    case 3:
        GP_SET(sprite->pOamData, sCuckooCondorPendulumLinkOam);
        ptr = (u8 *)sprite + 0x20;
        value = 8;
        *ptr = value;
        ptr++;
        *ptr = value;
        ptr++;
        *ptr = value;
        value = 2;
        sprite->animationTimer = value;
        break;
    case 4:
        GP_SET(sprite->pOamData, sCuckooCondorPendulumLinkOam);
        ptr = (u8 *)sprite + 0x20;
        value = 8;
        *ptr = value;
        ptr++;
        *ptr = value;
        ptr++;
        *ptr = value;
        value = 3;
        sprite->animationTimer = value;
        break;
    }

    value = sprite->roomSlot;
    if (value == 4) {
        value = 0x18;
        SpriteUtilFindSpriteSlotWork3(value);
    } else {
        value = *(volatile u8 *)&sprite->roomSlot;
        value++;
        SpriteUtilFindOwnSlotU32(value);
    }
}

void UpdateCuckooCondorPendulumPart(void)
{
    struct PrimarySpriteData *sprite;
    int slot;

    sprite = &gCurrentSprite;
    UpdateCuckooCondorPendulumHeight();
    slot = SpriteUtilFindSpriteSlot(0x18);
    sprite->xPosition = gSpriteData[slot].xPosition;
}

void CuckooCondorPendulumEndSwing(void)
{
    gCurrentSprite.pose = 0x18;
    gCurrentSprite.work2 = 0x1E;
    gInitialHealth = 0x20;
    m4aSongNumStart(SOUND_A6);
}

void CuckooCondorPendulumStartClockwiseSwing(void)
{
    struct PrimarySpriteData *sprite;
    const struct AnimationFrame *anim;
    struct PrimarySpriteData *base;
    struct PrimarySpriteData *scan;
    u8 *idp;
    struct PrimarySpriteData *end;
    u32 zero;
    u32 one;
    register u32 value PIN("r0");
    u32 status;
    u8 *ptr0;
    register u16 *dst PIN("r4");
    register u32 ydiff PIN("r2");

    sprite = &gCurrentSprite;
    anim = sCuckooCondorClawIdleOam;
    base = gSpriteData;
    zero = 0;
    one = 1;
    idp = &base->globalID;
    scan = base;
    value = 0xFD;
    value <<= 2;
    end = (struct PrimarySpriteData *)((u8 *)scan + value);
loop:
    value = *idp;
    if (value == 0xE8) {
        status = scan->status;
        value = one;
        value &= status;
        if (value != 0) {
            value = 0x22;
            idp[5] = value;
            idp[16] = zero;
        }
    }
    idp += sizeof(struct PrimarySpriteData);
    scan++;
    if ((s32)scan <= (s32)end)
        goto loop;

    status = 0;
    value = 0x22;
    sprite->pose = value;
    ptr0 = (u8 *)sprite;
    ptr0 += 0x27;
    *ptr0 = status;
    value = (u32)anim;
    GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
    sprite->currentAnimationFrame = status;
    sprite->animationTimer = status;
    value = 0xE5;
    value = SpriteUtilFindSpriteSlot(value);
    dst = &gSpriteAiRadius;
    ydiff = sprite->yPosition;
    scan = gSpriteData;
    status = sizeof(struct PrimarySpriteData);
    value *= status;
    value += (u32)scan;
    value = ((struct PrimarySpriteData *)value)->yPosition;
    ydiff -= value;
    ydiff = (s32)ydiff >> 1;
    *dst = ydiff;
    m4aSongNumStart(SE_CRACTUS_ARM_AIM);
}

void CuckooCondorPendulumSwingClockwise(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *parent;
    u8 *timer;
    u8 value;
    int slot;
    u32 size;
    struct PrimarySpriteData *sprites;

    UpdateCuckooCondorPendulumOrbit();
    UpdateCuckooCondorPendulumAffine();
    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    if (value > 0x77) {
        slot = SpriteUtilFindSpriteSlot(0xE5);
        sprites = gSpriteData;
        size = sizeof(struct PrimarySpriteData);
        size *= slot;
        parent = (struct PrimarySpriteData *)(size + (u32)sprites);
        SpawnSecondarySprite(parent->yPosition - 0x20, parent->xPosition - 0x14, 6);
        CuckooCondorPendulumEndSwing();
    } else {
        value += 4;
        *timer = value;
    }
}

void CuckooCondorPendulumStartCounterclockwiseSwing(void)
{
    struct PrimarySpriteData *sprite;
    const struct AnimationFrame *anim;
    struct PrimarySpriteData *base;
    struct PrimarySpriteData *scan;
    u8 *idp;
    struct PrimarySpriteData *end;
    u32 zero;
    u32 one;
    register u32 value PIN("r0");
    u32 status;
    u8 *ptr0;
    register u16 *dst PIN("r4");
    register u32 ydiff PIN("r2");

    sprite = &gCurrentSprite;
    anim = sCuckooCondorClawIdleOam;
    base = gSpriteData;
    zero = 0;
    one = 1;
    idp = &base->globalID;
    scan = base;
    value = 0xFD;
    value <<= 2;
    end = (struct PrimarySpriteData *)((u8 *)scan + value);
loop:
    value = *idp;
    if (value == 0xE8) {
        status = scan->status;
        value = one;
        value &= status;
        if (value != 0) {
            value = 0x20;
            idp[5] = value;
            idp[16] = zero;
        }
    }
    idp += sizeof(struct PrimarySpriteData);
    scan++;
    if ((s32)scan <= (s32)end)
        goto loop;

    status = 0;
    value = 0x20;
    sprite->pose = value;
    ptr0 = (u8 *)sprite;
    ptr0 += 0x27;
    *ptr0 = status;
    value = (u32)anim;
    GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
    sprite->currentAnimationFrame = status;
    sprite->animationTimer = status;
    value = 0xE5;
    value = SpriteUtilFindSpriteSlot(value);
    dst = &gSpriteAiRadius;
    ydiff = sprite->yPosition;
    scan = gSpriteData;
    status = sizeof(struct PrimarySpriteData);
    value *= status;
    value += (u32)scan;
    value = ((struct PrimarySpriteData *)value)->yPosition;
    ydiff -= value;
    ydiff = (s32)ydiff >> 1;
    *dst = ydiff;
    m4aSongNumStart(SE_CRACTUS_ARM_AIM);
}

void CuckooCondorPendulumSwingCounterclockwise(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *parent;
    u8 *timer;
    u8 value;
    int slot;
    u32 size;
    struct PrimarySpriteData *sprites;
    u8 result;

    UpdateCuckooCondorPendulumOrbit();
    UpdateCuckooCondorPendulumAffine();
    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    if ((u8)(value - 1) <= 0x87) {
        slot = SpriteUtilFindSpriteSlot(0xE5);
        sprites = gSpriteData;
        size = sizeof(struct PrimarySpriteData);
        size *= slot;
        parent = (struct PrimarySpriteData *)(size + (u32)sprites);
        SpawnSecondarySprite(parent->yPosition - 0x20, parent->xPosition + 0x14, 6);
        CuckooCondorPendulumEndSwing();
    } else {
        result = value - 4;
        *timer = result;
    }
}

void CuckooCondorPendulumPause(void)
{
    UpdateCuckooCondorPendulumAffine();
    gCurrentSprite.work2--;
    if (gCurrentSprite.work2 == 0) {
        if (gCurrentSprite.work0 > 0x80)
            gCurrentSprite.pose = 0x14;
        else
            gCurrentSprite.pose = 0x12;
        VoiceSetPlay(0);
    }
}

void CuckooCondorPendulumReturnCounterclockwise(void)
{
    UpdateCuckooCondorPendulumOrbit();
    UpdateCuckooCondorPendulumAffine();
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 0x10;
        gCurrentSprite.status &= 0xFF7F;
        gBossState = 0;
    } else
        gCurrentSprite.work0 -= 4;
}

void CuckooCondorPendulumReturnClockwise(void)
{
    UpdateCuckooCondorPendulumOrbit();
    UpdateCuckooCondorPendulumAffine();
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 0x10;
        gCurrentSprite.status &= 0xFF7F;
        gBossState = 0;
    } else
        gCurrentSprite.work0 += 4;
}

void CuckooCondorPendulumFallRight(void)
{
    gCurrentSprite.status &= 0x7F77;
    gCurrentSprite.pose = 0x28;
    if (gCurrentSprite.roomSlot == 0)
        gCurrentSprite.work2 = 1;
    else
        gCurrentSprite.work2 = 3;
    gCurrentSprite.work3 = 0;
}

void CuckooCondorPendulumFallLeft(void)
{
    gCurrentSprite.status &= 0x7F77;
    gCurrentSprite.pose = 0x2A;
    if (gCurrentSprite.roomSlot == 0)
        gCurrentSprite.work2 = 1;
    else
        gCurrentSprite.work2 = 3;
    gCurrentSprite.work3 = 0;
}

void InitCuckooCondorHead(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    register u32 value PIN("r0");
    u32 status;
    register u32 zeroByte PIN("r2");
    u32 zeroHalf;
    u8 *ptr;
    u8 *ptr2;
    register u8 *hitbox PIN("r4");

    sprite = &gCurrentSprite;
    status = sprite->status;
    zeroByte = 0x8408;
    ASM_BARRIER(: "+r"(zeroByte));
    value = zeroByte;
    zeroByte = 0;
    zeroHalf = 0;
    value |= status;
    value &= 0xFFFB;
    ptr = (u8 *)sprite;
    *(u16 *)ptr = value;
    *(ptr + 0x1E) = zeroByte;
    ptr += 0x20;
    value = 0x20;
    *ptr = value;
    ptr2 = (u8 *)sprite + 0x21;
    *ptr2 = zeroByte;
    ptr2++;
    status = 0x28;
    *(volatile u8 *)ptr2 = status;
    hitbox = (u8 *)sprite + 0x23;
    value = 0x40;
    *hitbox = value;
    ptr2 = (u8 *)sprite + 0x24;
    *ptr2 = zeroByte;
    ptr2++;
    *ptr2 = status;
    ptr2++;
    *(volatile u8 *)ptr2 = status;
    value = (u32)sCuckooCondorHeadIdleOam;
    ptr = (u8 *)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(ptr + 4), (const struct AnimationFrame *)value);
    *(ptr + 0x16) = zeroByte;
    *(u16 *)(ptr + 0x14) = zeroHalf;
    value = 0x10;
    *(ptr + 0x1C) = value;
    SpriteUtilFindSpriteSlotWork3(0x18);
}

void UpdateCuckooCondorHead(void)
{
    u32 temp;
    register u32 value PIN("r1");
    u32 timerPtr;
    register u32 oam PIN("r3");
    register struct PrimarySpriteData *sprite PIN("r4");

    temp = (u32)&gInitialHealth;
    value = *(u8 *)temp;
    timerPtr = temp;
    if (value == 0)
        return;

    if (value == 0x20) {
        temp = (u32)&gCurrentSprite;
        oam = (u32)GP(((struct PrimarySpriteData *)temp)->pOamData);
        value = (u32)sCuckooCondorHeadIdleOam;
        sprite = (struct PrimarySpriteData *)temp;

        if (oam == value) {
            temp = sprite->animationTimer;
            if (temp > 3) {
                value = sprite->status;
                temp = 0x40;
                temp |= value;
                sprite->status = temp;
            } else {
                value = sprite->status;
                temp = value & 0xFFBF;
                sprite->status = temp;
            }
        } else if (oam == (u32)sCuckooCondorHeadBlinkOam) {
            temp = sprite->animationTimer;
            if (temp <= 3) {
                value = sprite->status;
                temp = 0x40;
                temp |= value;
                sprite->status = temp;
            } else {
                value = sprite->status;
                temp = value & 0xFFBF;
                sprite->status = temp;
            }
        }

        GP_SET(sprite->pOamData, sCuckooCondorHeadHurtOam);
        temp = 0;
        ((struct PrimarySpriteData *)sprite)->currentAnimationFrame = temp;
        ((struct PrimarySpriteData *)sprite)->animationTimer = temp;
    }

    temp = *(u8 *)timerPtr;
    temp--;
    *(u8 *)timerPtr = temp;
    if ((temp << 24) != 0)
        return;

    timerPtr = (u32)&gCurrentSprite;
    value = *(u16 *)timerPtr;
    temp = 0x40;
    temp &= value;
    sprite = (struct PrimarySpriteData *)timerPtr;

    if (temp != 0)
        GP_SET(sprite->pOamData, sCuckooCondorHeadBlinkOam);
    else
        GP_SET(sprite->pOamData, sCuckooCondorHeadIdleOam);

    temp = 0;
    sprite->currentAnimationFrame = temp;
    temp = 2;
    sprite->animationTimer = temp;

    temp = sprite->status;
    oam = 0xFFBF;
    oam &= temp;
    sprite->status = oam;

    timerPtr = (u32)gSpriteData;
    temp = (u32)sprite;
    temp += 0x2A;
    value = *(u8 *)temp;
    temp = 0x2C;
    temp *= value;
    temp += timerPtr;
    value = ((struct PrimarySpriteData *)temp)->health;
    temp = gBgAnimationFrame;
    temp >>= 1;
    temp++;
    if ((s32)value < (s32)temp) {
        temp = 0x12;
        sprite->pose = temp;
        temp = 0xFBFF;
        temp &= oam;
        sprite->status = temp;
    }
}

void CuckooCondorHeadStartAttack(void)
{
    gCurrentSprite.pose = 0x12;
}

void CuckooCondorHeadAttack(void)
{
    u32 temp;
    u32 value;
    u32 child;
    u32 zero;
    register u32 base PIN("r4");
    register struct PrimarySpriteData *sprite PIN("r5");
    register u32 zeroByte PIN("r6");

    value = (u32)&gCurrentSprite;
    temp = ((struct PrimarySpriteData *)value)->pose;
    if (temp == 0x6A) {
        temp = ((struct PrimarySpriteData *)value)->yPosition;
        temp -= 0x20;
        value = ((struct PrimarySpriteData *)value)->xPosition;
        value -= 0x20;
        child = 0x40;
        SpawnSecondarySprite(temp, value, child);
    } else {
        temp = ((struct PrimarySpriteData *)value)->yPosition;
        temp -= 0x20;
        value = ((struct PrimarySpriteData *)value)->xPosition;
        value += 0x20;
        child = 0x40;
        SpawnSecondarySprite(temp, value, child);
    }

    temp = (u32)&gCurrentSprite;
    child = (u32)GP(((struct PrimarySpriteData *)temp)->pOamData);
    value = (u32)sCuckooCondorHeadIdleOam;
    sprite = (struct PrimarySpriteData *)temp;

    if (child == value) {
        temp = sprite->animationTimer;
        if (temp > 3) {
            value = sprite->status;
            temp = 0x40;
            temp |= value;
            sprite->status = temp;
        } else {
            value = sprite->status;
            temp = value & 0xFFBF;
            sprite->status = temp;
        }
    } else if (child == (u32)sCuckooCondorHeadBlinkOam) {
        temp = sprite->animationTimer;
        if (temp <= 3) {
            value = sprite->status;
            temp = 0x40;
            temp |= value;
            sprite->status = temp;
        } else {
            value = sprite->status;
            temp = value & 0xFFBF;
            sprite->status = temp;
        }
    }

    GP_SET(sprite->pOamData, sCuckooCondorHeadAttackOam);
    zero = 0;
    sprite->currentAnimationFrame = zero;
    zeroByte = 0;
    sprite->animationTimer = zero;

    temp = (u32)sprite;
    temp += 0x2A;
    value = *(u8 *)temp;
    base = (u32)gSpriteData;
    temp = 0x2C;
    child = value;
    child *= temp;
    temp = base + 4;
    temp = (child) - (-(temp));
    value = (u32)sCuckooCondorHeadPhase2Oam;
    *(u32 *)temp = value;
    child += base;
    *(u8 *)(child + 0x16) = zeroByte;
    *(u16 *)(child + 0x14) = zero;
    value = child;
    value += 0x29;
    temp = 0x31;
    *(u8 *)value = temp;
    zero = 0x16;
    *(u8 *)(child + 0x1C) = zero;

    temp = *(u8 *)(child + 0x1D);
    if (temp == 1) {
        temp = *(u16 *)child;
        base = 0x80;
        base <<= 3;
        ASM_BARRIER(: "+r"(base));
        value = base;
        temp |= value;
        *(u16 *)child = temp;
        temp = sprite->status;
        temp |= value;
        sprite->status = temp;
        sprite->pose = zero;
        value = (u32)sprite;
        value += 0x27;
        temp = 0x60;
    } else {
        temp = 0x14;
        sprite->pose = temp;
        value = (u32)sprite;
        value += 0x27;
        temp = 0x30;
    }
    *(u8 *)value = temp;
}

void CuckooCondorHeadRecover(void)
{
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *body PIN("r2");
    register u8 *timer PIN("r2");
    u32 value;
    u32 status;
    register u32 masked PIN("r0");

    sprite = &gCurrentSprite;
    body = sprite;
    timer = &body->work0;
    value = *timer;
    value -= 1;
    *timer = value;
    value <<= 24;
    body = sprite;
    if (value == 0) {
        status = body->status;
        masked = status & 0x40;
        if (masked != 0)
            GP_SET(body->pOamData, sCuckooCondorHeadBlinkOam);
        else
            GP_SET(body->pOamData, sCuckooCondorHeadIdleOam);
        body->currentAnimationFrame = 0;
        body->animationTimer = 2;
        status = *(volatile u16 *)&body->status;
        masked = 0xFFBF;
        masked &= status;
        body->pose = 18;
        masked &= 0xFDFF;
        body->status = masked;
    }
}

void CuckooCondorHeadSpawnDebris(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;
    u32 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    value--;
    *timer = value;
    zero = (u8)value;
    if (zero == 0) {
        SpawnHighPriorityPrimarySprite(0xB, 0, 0, sprite->yPosition, sprite->xPosition);
        sprite->status = zero;
    }
}

void InitCuckooCondorClockBody(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *ptr;
    u16 status;
    register u32 value PIN("r0");
    register u32 constant PIN("r2");
    register u32 zero PIN("r2");
    u32 timer;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    status = sprite->status;
    constant = 0x8408;
    ASM_BARRIER(: "+r"(constant));
    value = constant;
    zero = 0;
    timer = 0;
    value |= status;
    value &= 0xFFFB;
    ptr = sprite;
    ptr->status = value;
    ptr->warioCollision = zero;
    sprite->drawDistanceDown = 0x28;
    sprite->drawDistanceUp = 0x28;
    sprite->drawDistanceLeftRight = 0x60;
    sprite->hitboxExtentUp = 4;
    sprite->hitboxExtentDown = 4;
    sprite->hitboxExtentLeft = 4;
    sprite->hitboxExtentRight = 4;
    oam = sCuckooCondorClockBodyOam;
    ptr = sprite;
    GP_SET(ptr->pOamData, oam);
    ptr->currentAnimationFrame = zero;
    ptr->animationTimer = timer;
    ptr->pose = 0x10;
    SpriteUtilFindSpriteSlotWork3(0x18);
}

void CuckooCondorClockBodyStartFall(void)
{
    gCurrentSprite.status &= 0x7F77;
    gCurrentSprite.pose = 0x28;
    gCurrentSprite.work2 = 1;
    gCurrentSprite.work3 = 0;
    GP_SET(gCurrentSprite.pOamData, sCuckooCondorClockBodyFallingOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
}

void InitCuckooCondorSmallDebris(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    u32 status;
    register u32 zeroByte PIN("r2");
    u32 zeroHalf;
    u8 *ptr;
    u8 *ptr2;

    sprite = &gCurrentSprite;
    status = sprite->status;
    zeroByte = 0x80;
    zeroByte <<= 3;
    ASM_BARRIER(: "+r"(zeroByte));
    value = zeroByte;
    zeroByte = 0;
    zeroHalf = 0;
    value |= status;
    value &= 0xFFFB;
    ptr = (u8 *)sprite;
    *(u16 *)ptr = value;
    value = 0x45;
    *(ptr + 0x1E) = value;
    ptr += 0x20;
    value = 0x10;
    *ptr = value;
    ptr++;
    *ptr = value;
    ptr++;
    *ptr = value;
    ptr2 = (u8 *)sprite + 0x23;
    status = 0x20;
    *ptr2 = status;
    ptr2++;
    *ptr2 = status;
    ptr2++;
    *ptr2 = status;
    ptr2++;
    *ptr2 = status;
    value = (u32)sCuckooCondorSmallDebrisOam;
    ptr = (u8 *)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(ptr + 4), (const struct AnimationFrame *)value);
    *(ptr + 0x16) = zeroByte;
    *(u16 *)(ptr + 0x14) = zeroHalf;
    value = 0x1C;
    *(ptr + 0x1C) = value;
    ptr2 = (u8 *)sprite + 0x2A;
    *ptr2 = zeroByte;
    value = 1;
    ((struct PrimarySpriteData *)ptr)->drawPriority = value;
    SpriteUtilTurnTowardWario();
    m4aSongNumStart(SOUND_D0);
}

void UpdateCuckooCondorSmallDebrisFall(void)
{
    struct PrimarySpriteData *sprite;
    u32 newY;
    register u32 value PIN("r0");
    u8 *work3Ptr;
    u32 index;
    const s16 *table;
    u32 addr;
    u32 delta;
    s32 signedDelta;
    u32 zero;
    u8 *work0Ptr;

    sprite = &gCurrentSprite;
    newY = func_8023A60(sprite->yPosition, sprite->xPosition);
    value = gSpriteCollisionResult;
    if (value != 0) {
        value = 0;
        sprite->yPosition = newY;
        newY = 0x10;
        sprite->pose = newY;
        work0Ptr = &sprite->work0;
        *work0Ptr = value;
        m4aSongNumStart(SOUND_D1);
    } else {
        work3Ptr = &sprite->work3;
        index = *work3Ptr;
        table = sSpriteGravityVelocityTable;
        addr = index << 1;
        addr += (u32)table;
        delta = *(const u16 *)addr;
        zero = 0;
        signedDelta = *(const s16 *)(addr + zero);
        value = 0x7FFF;
        if (signedDelta == value) {
            signedDelta = index - 1;
            signedDelta <<= 1;
            signedDelta += (u32)table;
            value = sprite->yPosition;
            signedDelta = *(const u16 *)signedDelta;
            value += signedDelta;
            sprite->yPosition = value;
        } else {
            value = index + 1;
            *work3Ptr = value;
            value = sprite->yPosition;
            value += delta;
            sprite->yPosition = value;
        }
    }
}

void UpdateCuckooCondorSmallDebris(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;
    register u32 mask PIN("r1");
    register u32 status PIN("r1");
    struct PrimarySpriteData *sprite2;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    value += 1;
    *timer = value;
    mask = 0x1F;
    value &= mask;
    if (value == 0)
        m4aSongNumStart(SOUND_D1);

    status = sprite->status;
    value = 0x40;
    value &= status;
    if (value != 0) {
        value = sprite->xPosition;
        value += 8;
    } else {
        value = sprite->xPosition;
        value -= 8;
    }
    sprite->xPosition = value;
    sprite2 = &gCurrentSprite;
    status = sprite2->status;
    value = (u16)(2 & status);
    if (value == 0)
        sprite2->status = value;
}

void InitCuckooCondorLargeDebris(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    u32 status;
    register u32 zeroByte PIN("r2");
    u32 zeroHalf;
    u8 *ptr;
    u8 *ptr2;

    sprite = &gCurrentSprite;
    status = sprite->status;
    zeroByte = 0x90;
    zeroByte <<= 3;
    ASM_BARRIER(: "+r"(zeroByte));
    value = zeroByte;
    zeroByte = 0;
    zeroHalf = 0;
    value |= status;
    value &= 0xFFFB;
    ptr = (u8 *)sprite;
    *(u16 *)ptr = value;
    value = 0x45;
    *(ptr + 0x1E) = value;
    ptr += 0x20;
    value = 0x10;
    *ptr = value;
    ptr++;
    *ptr = value;
    ptr++;
    *ptr = value;
    ptr2 = (u8 *)sprite + 0x23;
    status = 0x20;
    *ptr2 = status;
    ptr2++;
    *ptr2 = status;
    ptr2++;
    *ptr2 = status;
    ptr2++;
    *ptr2 = status;
    value = (u32)sCuckooCondorClockPieceTopOam;
    ptr = (u8 *)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(ptr + 4), (const struct AnimationFrame *)value);
    *(ptr + 0x16) = zeroByte;
    *(u16 *)(ptr + 0x14) = zeroHalf;
    value = 0x1C;
    *(ptr + 0x1C) = value;
    value = *(ptr + 0x18);
    ptr += 0x27;
    *ptr = value;
    ptr2 = (u8 *)sprite + 0x2A;
    *ptr2 = zeroByte;
    SpriteUtilTurnTowardWario();
    m4aSongNumStart(SOUND_A8);
}

void UpdateCuckooCondorLargeDebrisFall(void)
{
    struct PrimarySpriteData *sprite;
    u32 newY;
    register u32 value PIN("r0");
    u8 *work3Ptr;
    u32 index;
    const s16 *table;
    u32 addr;
    u32 delta;
    s32 signedDelta;
    u32 zero;
    u8 *timer;

    sprite = &gCurrentSprite;
    newY = func_8023A60(sprite->yPosition, sprite->xPosition);
    value = gSpriteCollisionResult;
    if (value != 0) {
        sprite->yPosition = newY;
        value = 0x10;
        sprite->pose = value;
        m4aSongNumStart(SOUND_A9);
    } else {
        work3Ptr = &sprite->work3;
        index = *work3Ptr;
        table = sSpriteGravityVelocityTable;
        addr = index << 1;
        addr += (u32)table;
        delta = *(const u16 *)addr;
        zero = 0;
        signedDelta = *(const s16 *)(addr + zero);
        value = 0x7FFF;
        if (signedDelta == value) {
            signedDelta = index - 1;
            signedDelta <<= 1;
            signedDelta += (u32)table;
            value = sprite->yPosition;
            signedDelta = *(const u16 *)signedDelta;
            value += signedDelta;
            sprite->yPosition = value;
        } else {
            value = index + 1;
            *work3Ptr = value;
            value = sprite->yPosition;
            value += delta;
            sprite->yPosition = value;
        }
    }
    timer = (u8 *)&gCurrentSprite;
    timer += 0x27;
    value = *timer;
    value -= 0x10;
    *timer = value;
}

void UpdateCuckooCondorLargeDebris(void)
{
    if (gCurrentSprite.status & 0x40) {
        gCurrentSprite.work0 += 0x10;
        gCurrentSprite.xPosition += 4;
    } else {
        gCurrentSprite.work0 -= 0x10;
        gCurrentSprite.xPosition -= 4;
    }
    if (!(gCurrentSprite.status & 2))
        gCurrentSprite.status = 0;
}

void InitCuckooCondorEgg(void)
{
    struct PrimarySpriteData *sprite;
    u32 status;
    register u32 zero PIN("r3");
    u32 eight;
    u32 zero16;
    struct PrimarySpriteData *sp2;
    register u32 tmp PIN("r0");
    register u8 *ptr PIN("r1");

    sprite = &gCurrentSprite;
    tmp = sprite->status;
    eight = 8;
    zero = 0;
    zero16 = 0;
    status = eight;
    status |= tmp;
    status &= 0xFFFB;
    ptr = (u8 *)sprite;
    *(u16 *)ptr = status;
    tmp = 5;
    sprite->warioCollision = tmp;
    ptr = &sprite->drawDistanceDown;
    tmp = 16;
    *ptr = tmp;
    sprite->drawDistanceUp = zero;
    sprite->drawDistanceLeftRight = eight;
    ptr += 3;
    tmp = 44;
    *ptr = tmp;
    sprite->hitboxExtentDown = zero;
    ptr += 2;
    tmp = 24;
    *ptr = tmp;
    ptr++;
    *(volatile u8 *)ptr = tmp;
    tmp = (u32)sCuckooCondorEggOam;
    ptr = (u8 *)sprite;
    GP_SET(((struct PrimarySpriteData *)ptr)->pOamData, (const struct AnimationFrame *)tmp);
    ((struct PrimarySpriteData *)ptr)->currentAnimationFrame = zero;
    ((struct PrimarySpriteData *)ptr)->animationTimer = zero16;
    tmp = 64;
    tmp &= status;
    sp2 = sprite;
    if (tmp != 0)
        tmp = 52;
    else
        tmp = 54;
    sp2->pose = tmp;
    zero = (u32)sp2;
    zero += 0x27;
    ptr = 0;
    tmp = 10;
    *(u8 *)zero = tmp;
    zero += 2;
    ASM_BARRIER(: "+r"(zero));
    tmp = 5;
    *(u8 *)zero = tmp;
    tmp = (u32)sp2;
    tmp += 0x2A;
    *(u8 *)tmp = (u32)ptr;
    m4aSongNumStart(SOUND_AB);
}

void CuckooCondorEggStartHatching(void)
{
    struct PrimarySpriteData *sprite;
    u32 zeroHalf;
    register u32 zeroByte PIN("r3");
    u32 value;
    register u32 mask PIN("r1");

    sprite = &gCurrentSprite;
    zeroHalf = 0;
    value = 1;
    sprite->disableWarioCollisionTimer = value;
    mask = sprite->status;
    zeroByte = 0x400;
    ASM_BARRIER(: "+r"(zeroByte));
    value = zeroByte;
    zeroByte = 0;
    value |= mask;
    mask = 0xFEF7;
    value &= mask;
    sprite->status = value;
    GP_SET(sprite->pOamData, sCuckooCondorEggHatchOam);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    value = 0x7B;
    sprite->pose = value;
    sprite->work0 = 0x24;
}

void CuckooCondorEggHatch(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 oldValue;
    u32 value;

    sprite = &gCurrentSprite;
    oldValue = 1;
    sprite->disableWarioCollisionTimer = oldValue;
    timer = &sprite->work0;
    oldValue = *timer;
    value = oldValue - 1;
    *timer = value;
    oldValue = value << 24;
    oldValue >>= 24;
    if (oldValue == 0)
        goto clear;
    oldValue = value << 24;
    oldValue >>= 24;
    if (oldValue != 0x1C)
        goto end;
    SpriteSpawnAsChild(0xEA, 0, 0, sprite->yPosition, sprite->xPosition);
    goto end;
clear:
    sprite->status = oldValue;
end:
    return;
}

void ResetCuckooCondorEggMotion(void)
{
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void CuckooCondorEggStartSlideRight(void)
{
    gCurrentSprite.pose = 0x34;
    ResetCuckooCondorEggMotion();
}

void CuckooCondorEggStartSlideLeft(void)
{
    gCurrentSprite.pose = 0x36;
    ResetCuckooCondorEggMotion();
}

void CuckooCondorEggStartFall(void)
{
    gCurrentSprite.pose = 0x1E;
    gCurrentSprite.work3 = 0;
}

void PrepareCuckooCondorEggFallOffscreen(void)
{
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    gCurrentSprite.status = (gCurrentSprite.status & 0xFEFF) | 0x200;
}

void CuckooCondorEggFallOffscreenRight(void)
{
    gCurrentSprite.pose = 0x20;
    PrepareCuckooCondorEggFallOffscreen();
}

void CuckooCondorEggFallOffscreenLeft(void)
{
    gCurrentSprite.pose = 0x22;
    PrepareCuckooCondorEggFallOffscreen();
}

void ResetCuckooCondorEggBounce(void)
{
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void CuckooCondorEggBounceRight(void)
{
    gCurrentSprite.pose = 0x48;
    ResetCuckooCondorEggBounce();
}

void CuckooCondorEggBounceLeft(void)
{
    gCurrentSprite.pose = 0x4A;
    ResetCuckooCondorEggBounce();
}

void CuckooCondorEggLiftRight(void)
{
    gCurrentSprite.status &= 0xFFF7;
    gCurrentSprite.pose = 0x52;
}

void CuckooCondorEggLiftLeft(void)
{
    gCurrentSprite.status &= 0xFFF7;
    gCurrentSprite.pose = 0x54;
}

void CuckooCondorEggCarryRight(void)
{
    gCurrentSprite.status &= 0xFFF7;
    gCurrentSprite.pose = 0x58;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void CuckooCondorEggCarryLeft(void)
{
    gCurrentSprite.status &= 0xFFF7;
    gCurrentSprite.pose = 0x56;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}

void InitCuckooCondorChick(void)
{
    struct PrimarySpriteData *sprite;
    u32 statusRaw;
    u32 status;
    register u32 zero PIN("r4");
    u16 zero16;
    register u8 *ptr PIN("r1");
    u8 *ptr0;
    struct WarioData *wario;
    u16 spriteX;
    u16 warioX;
    u32 newStatus;
    register u32 flag PIN("r1");

    sprite = &gCurrentSprite;
    statusRaw = sprite->status;
    status = 0xFFFB;
    status &= statusRaw;
    zero = 0;
    zero16 = 0;
    sprite->status = status;
    sprite->warioCollision = 2;
    ptr = &sprite->drawDistanceDown;
    *ptr = 0x10;
    ptr0 = &sprite->drawDistanceUp;
    *ptr0 = zero;
    ptr += 2;
    *ptr = 8;
    ptr++;
    *ptr = 0x28;
    ptr0 = &sprite->hitboxExtentDown;
    *ptr0 = zero;
    ptr += 2;
    *ptr = 0x18;
    ptr++;
    *ptr = 0x14;
    GP_SET(sprite->pOamData, sCuckooCondorChickWalkOam);
    *((u8 *)&sprite->currentAnimationFrame) = zero;
    sprite->animationTimer = zero16;
    sprite->pose = 0x1C;
    ptr++;
    *ptr = 0xA;
    ptr++;
    *(volatile u8 *)ptr = 0xAA;
    ptr0 = &sprite->work3;
    *ptr0 = zero;
    wario = &gWarioData;
    spriteX = sprite->xPosition;
    warioX = wario->xPosition;
    if (spriteX < warioX) {
        newStatus = status & 0xFFBF;
    } else {
        flag = 0x40;
        newStatus = status;
        newStatus |= flag;
    }
    sprite->status = newStatus;
    m4aSongNumStart(SOUND_D2);
}

void CuckooCondorChickWaitToMove(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 0x10;
        gCurrentSprite.work3 = 0;
        gCurrentSprite.work0 = 0x1E;
    }
}

void UpdateCuckooCondorChickWalk(void)
{
    {
        struct PrimarySpriteData *sprite;
        u8 *work3;

        sprite = &gCurrentSprite;
        work3 = &sprite->work3;
        if (*work3 == 1) {
            func_8023BFC((u16)(sprite->yPosition + 8), sprite->xPosition);
            if (gSpriteCollisionTileType != 0x11) {
                sprite->pose = 28;
                *work3 = 0;
                return;
            }
        }
    }

    {
        struct PrimarySpriteData *sprite;
        u8 *work0;
        register u8 *work3 PIN("r6");
        const s16 *table;
        u32 workValue;
        register u32 base PIN("r0");
        register u8 *workPtr PIN("r1");
        u32 index;
        s16 delta;

        base = (u32)&gCurrentSprite;
        workPtr = (u8 *)0x27;
        /* Prevent agbcc from folding the offset into a copy-plus-immediate pair. */
        ASM_BARRIER(: "+r"(workPtr));
        workPtr += base;
        work0 = workPtr;
        workValue = *workPtr;
        sprite = (struct PrimarySpriteData *)base;
        if (workValue != 0) {
            base = workValue - 1;
            *workPtr = base;
        }
        work3 = &sprite->work3;
        index = *work3;
        table = (const s16 *)sCuckooCondorChickYVelocity;
        delta = table[index];
        if (delta == 0x7FFF) {
            delta = table[0];
            index = 0;
        }
        *work3 = index + 1;
        sprite->yPosition += delta;
        if (sprite->status & 0x40) {
            u8 *testWork0;
            testWork0 = work0;
            if (*testWork0 != 0)
                sprite->xPosition += 4;
            else
                sprite->xPosition += 2;
            func_8023BFC((u16)(sprite->yPosition - 0x20), (u16)(sprite->xPosition + 0x20));
            if (gSpriteCollisionTileType == 0x11)
                sprite->status &= 0xFFBF;
        } else {
            u8 *testWork0;
            u32 testValue;
            testWork0 = work0;
            /* agbcc otherwise copies r7 to r0 before this byte load. */
            testValue = *testWork0;
            if (testValue != 0)
                sprite->xPosition -= 4;
            else
                sprite->xPosition -= 2;
            func_8023BFC((u16)(sprite->yPosition - 0x20), (u16)(sprite->xPosition - 0x20));
            if (gSpriteCollisionTileType == 0x11)
                sprite->status |= 0x40;
        }
    }
}
void ResetCuckooCondorChickMotion(void)
{
    GP_SET(gCurrentSprite.pOamData, sCuckooCondorChickWalkOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work0 = 0;
    gCurrentSprite.drawPriority = 0;
    gCurrentSprite.status = (gCurrentSprite.status & 0xFEFF) | 0x280;
}

void CuckooCondorChickKnockbackRight(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 0x20;
    sprite->work2 = 8;
    ResetCuckooCondorChickMotion();
    SpawnSecondarySprite(sprite->yPosition - 0x20, sprite->xPosition, 6);
}

void CuckooCondorChickKnockbackLeft(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 0x22;
    sprite->work2 = 8;
    ResetCuckooCondorChickMotion();
    SpawnSecondarySprite(sprite->yPosition - 0x20, sprite->xPosition, 6);
}

void CuckooCondorChickFallRight(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.pose = 0x20;
    gCurrentSprite.work2 = 4;
    ResetCuckooCondorChickMotion();
}

void CuckooCondorChickFallLeft(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.pose = 0x22;
    gCurrentSprite.work2 = 4;
    ResetCuckooCondorChickMotion();
}

void PrepareCuckooCondorChickDash(void)
{
    GP_SET(gCurrentSprite.pOamData, sCuckooCondorChickDashOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 0x20;
    gCurrentSprite.work1 = 0x3C;
}

void CuckooCondorChickStartDashRight(void)
{
    gCurrentSprite.status |= 0x40;
    gCurrentSprite.pose = 0x24;
    PrepareCuckooCondorChickDash();
}

void UpdateCuckooCondorChickDashRight(void)
{
    register struct PrimarySpriteData *sprite PIN("r1");
    u8 *timer;
    struct PrimarySpriteData *current;
    u16 yPosition;
    u32 value;
    register u32 zero PIN("r1");
    u32 zero2;

    sprite = &gCurrentSprite;
    value = sprite->xPosition;
    value += 4;
    sprite->xPosition = value;
    timer = &sprite->work0;
    value = *timer;
    current = sprite;
    if (value != 0) {
        value -= 1;
        *timer = value;
        value <<= 24;
        zero = value >> 24;
        if (zero == 0) {
            GP_SET(current->pOamData, sCuckooCondorChickWalkOam);
            current->currentAnimationFrame = 0;
            current->animationTimer = zero;
        }
    } else {
        value = current->currentAnimationFrame;
        value += 1;
        current->currentAnimationFrame = value;
    }

    yPosition = current->yPosition - 0x20;
    zero = ((volatile struct PrimarySpriteData *)current)->xPosition;
    zero += 0x20;
    zero = (u16)zero;
    func_8023BFC(yPosition, zero);
    if (gSpriteCollisionTileType == 0x11) {
        zero = current->status;
        value = zero & 0xFFBF;
        zero = 0;
        zero2 = 0;
        current->status = value;
        current->pose = 0x10;
        GP_SET(current->pOamData, sCuckooCondorChickWalkOam);
        current->currentAnimationFrame = zero;
        current->animationTimer = zero2;
    }
}

void CuckooCondorChickStartDashLeft(void)
{
    gCurrentSprite.status &= 0xFFBF;
    gCurrentSprite.pose = 0x26;
    PrepareCuckooCondorChickDash();
}

void UpdateCuckooCondorChickDashLeft(void)
{
    register struct PrimarySpriteData *sprite PIN("r1");
    u8 *timer;
    struct PrimarySpriteData *current;
    u16 yPosition;
    u32 value;
    register u32 arg PIN("r1");
    register u32 zero PIN("r2");
    u32 zero2;

    sprite = &gCurrentSprite;
    value = sprite->xPosition;
    value -= 4;
    sprite->xPosition = value;
    timer = &sprite->work0;
    value = *timer;
    current = sprite;
    if (value != 0) {
        value -= 1;
        *timer = value;
        value <<= 24;
        arg = value >> 24;
        if (arg == 0) {
            GP_SET(current->pOamData, sCuckooCondorChickWalkOam);
            current->currentAnimationFrame = 0;
            current->animationTimer = arg;
        }
    } else {
        value = current->currentAnimationFrame;
        value += 1;
        current->currentAnimationFrame = value;
    }

    yPosition = current->yPosition - 0x20;
    arg = ((volatile struct PrimarySpriteData *)current)->xPosition;
    arg -= 0x20;
    arg = (u16)arg;
    func_8023BFC(yPosition, arg);
    if (gSpriteCollisionTileType == 0x11) {
        arg = current->status;
        value = 0x40;
        zero = 0;
        zero2 = 0;
        value |= arg;
        current->status = value;
        current->pose = 0x10;
        GP_SET(current->pOamData, sCuckooCondorChickWalkOam);
        current->currentAnimationFrame = zero;
        current->animationTimer = zero2;
    }
}

void CuckooCondorChickStartChase(void)
{
    GP_SET(gCurrentSprite.pOamData, sCuckooCondorChickChaseOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 1;
    gCurrentSprite.pose = 0x6F;
    gCurrentSprite.work0 = 0x46;
    SpriteUtilTurnTowardWario();
}

void UpdateCuckooCondorChickChase(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;
    u32 status;
    u16 yPosition;
    struct PrimarySpriteData *sprite2;
    register u8 *timer PIN("r3");
    register u32 zero PIN("r4");

    sprite = &gCurrentSprite;
    status = sprite->status;
    value = 0x40;
    value &= status;
    if (value == 0)
        goto left;

    status = sprite->xPosition;
    value = status + 4;
    sprite->xPosition = value;
    yPosition = sprite->yPosition - 0x20;
    status += 0x24;
    status = (u16)status;
    func_8023BFC(yPosition, status);
    value = gSpriteCollisionTileType;
    if (value != 0x11)
        goto afterCollision;
    status = sprite->status;
    value = 0xFFBF;
    value &= status;
    goto storeStatus;

left:
    status = sprite->xPosition;
    value = status - 4;
    sprite->xPosition = value;
    yPosition = sprite->yPosition - 0x20;
    status -= 0x24;
    status = (u16)status;
    func_8023BFC(yPosition, status);
    value = gSpriteCollisionTileType;
    if (value != 0x11)
        goto afterCollision;
    value = sprite->status;
    status = 0x40;
    value |= status;
storeStatus:
    sprite->status = value;

afterCollision:
    sprite2 = &gCurrentSprite;
    timer = &sprite2->work0;
    value = *timer;
    status = value - 1;
    zero = 0;
    *timer = status;
    value = status << 24;
    if (value == 0)
        goto finishTimer;
    value >>= 24;
    if (value == 0x3C) {
        sprite2->warioCollision = 0x46;
        goto done;
    }
    if (value == 0x28) {
        value = (u32)sCuckooCondorChickAttackOam;
        goto setAnim;
    }
    if (value == 20) {
        value = (u32)sCuckooCondorChickAttackEndOam;
setAnim:
        GP_SET(sprite2->pOamData, (const struct AnimationFrame *)value);
        sprite2->currentAnimationFrame = zero;
        value = 1;
        sprite2->animationTimer = value;
    }
    goto done;
finishTimer:
    value = 0x70;
    sprite2->pose = value;
done:
    ;
}

void CuckooCondorChickStartExplosion(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    struct PrimarySpriteData *ptr;
    struct PrimarySpriteData *ptr2;
    register u32 value PIN("r0");
    register u32 status PIN("r1");
    register u32 zeroByte PIN("r2");
    u32 zeroHalf;

    value = (u32)&gCurrentSprite;
    sprite = (struct PrimarySpriteData *)value;
    ptr = (struct PrimarySpriteData *)value;
    status = ptr->status;
    zeroByte = 0x80;
    zeroByte <<= 3;
    ASM_BARRIER(: "+r"(zeroByte));
    value = zeroByte;
    zeroByte = 0;
    zeroHalf = 0;
    value |= status;
    ptr = sprite;
    ptr->status = value;
    value = (u32)sCuckooCondorChickExplodeOam;
    GP_SET(ptr->pOamData, (const struct AnimationFrame *)value);
    ptr->currentAnimationFrame = zeroByte;
    ptr->animationTimer = zeroHalf;
    ptr = (struct PrimarySpriteData *)((u8 *)ptr + 0x27);
    value = 0x16;
    *(u8 *)ptr = value;
    value = 0x45;
    ptr2 = sprite;
    ptr2->warioCollision = value;
    value = (u32)sprite;
    value += 0x23;
    status = 0x20;
    *(u8 *)value = status;
    value++;
    *(volatile u8 *)value = status;
    status = (u32)sprite;
    status += 0x25;
    value = 0x30;
    *(u8 *)status = value;
    status++;
    *(volatile u8 *)status = value;
    value = 0x71;
    ptr2->pose = value;
    value = 1;
    ptr2->drawPriority = value;
    value = ptr2->yPosition;
    value -= 0x1C;
    ptr2->yPosition = value;
    m4aSongNumStart(SOUND_D3);
}

void UpdateCuckooCondorChickExplosion(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0)
        gCurrentSprite.status = 0;
}

void SpriteCuckooCondor(void)
{
    switch (gCurrentSprite.pose) {
    case 0:
        InitCuckooCondor();
        break;
    case 1:
        CuckooCondorWaitForIntro();
        break;
    case 2:
        CuckooCondorStartBattle();
        break;
    case 3:
        CuckooCondorWaitForTimer();
        break;
    case 4:
        CuckooCondorEnterArena();
        break;
    case 16:
        CuckooCondorMoveClock();
        break;
    case 110:
        CuckooCondorLowerPendulum();
        break;
    case 111:
        CuckooCondorRaisePendulum();
        break;
    case 17:
        CuckooCondorSinkAfterHit();
        break;
    case 18:
        CuckooCondorHandleClockDamage();
        break;
    case 19:
        CuckooCondorBreakClock();
        break;
    case 20:
        CuckooCondorFlyAndBob();
        break;
    case 51:
    case 53:
    case 105:
    case 106:
        gCurrentSprite.pose = 20;
        gCurrentSprite.status &= 0xFDFF;
        CuckooCondorFlyAndBob();
        break;
    case 21:
        CuckooCondorLayEgg();
        break;
    case 22:
        CuckooCondorHandleCondorDamage();
        break;
    case 120:
        CuckooCondorExplode();
        break;
    case 122:
        CuckooCondorTakeShopItemHit();
    case 123:
        CuckooCondorHandleShopItemHit();
        break;
    }

    if (gCurrentSprite.health >= ((gBgAnimationFrame >> 1) + 1)
        && *(u8 *)&gBossState == 0)
        UpdateCuckooCondorCapturedWario();
}

void SpriteCuckooCondorClockPiece(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
    case 0:
        InitCuckooCondorClockPiece();
        break;
    case 16:
        UpdateCuckooCondorClockPiece();
        break;
    case 110:
        if (gCurrentSprite.roomSlot == 1)
            CuckooCondorClockPieceStartDebrisDrop();
        break;
    case 111:
        if (gCurrentSprite.roomSlot == 1)
            CuckooCondorClockPieceDropDebris();
        break;
    case 17:
        if (gCurrentSprite.roomSlot == 4)
            CuckooCondorClockPieceStartSpin();
    case 18:
        if (gCurrentSprite.roomSlot == 4)
            CuckooCondorClockPieceSpinUp();
        break;
    case 19:
        if (gCurrentSprite.roomSlot == 4)
            CuckooCondorClockPieceFlash();
        break;
    case 20:
        if (gCurrentSprite.roomSlot == 4)
            CuckooCondorClockPieceSpin();
        break;
    case 39:
        CuckooCondorClockPieceFallRight();
    case 40:
        SpriteUtilFallOffscreenRight();
        return;
    case 41:
        CuckooCondorClockPieceFallLeft();
    case 42:
        SpriteUtilFallOffscreenLeft();
        return;
    }

    {
        struct PrimarySpriteData *sprite;
        struct PrimarySpriteData *current;
        struct PrimarySpriteData *parent;
        u32 parentSlot;
        u32 roomSlot;

        current = &gCurrentSprite;
        parentSlot = current->work3;
        roomSlot = current->roomSlot;
        sprite = current;
        switch (roomSlot) {
        case 0:
        {
            struct PrimarySpriteData *sprites;
            register u32 index PIN("r1");
            u32 size;

            sprites = gSpriteData;
            size = sizeof(struct PrimarySpriteData);
            index = parentSlot;
            index *= size;
            index += (u32)sprites;
            parent = (struct PrimarySpriteData *)index;
            sprite->yPosition = parent->yPosition + 0x56;
            sprite->xPosition = parent->xPosition + 0xFF70;
            break;
        }
        case 1:
        {
            struct PrimarySpriteData *sprites;
            register u32 index PIN("r1");
            u32 size;

            sprites = gSpriteData;
            size = sizeof(struct PrimarySpriteData);
            index = parentSlot;
            index *= size;
            index += (u32)sprites;
            parent = (struct PrimarySpriteData *)index;
            sprite->yPosition = parent->yPosition + 0x56;
            sprite->xPosition = parent->xPosition + 0xFF50;
            break;
        }
        case 2:
        {
            struct PrimarySpriteData *sprites;
            register u32 index PIN("r1");
            u32 size;

            sprites = gSpriteData;
            size = sizeof(struct PrimarySpriteData);
            index = parentSlot;
            index *= size;
            index += (u32)sprites;
            parent = (struct PrimarySpriteData *)index;
            sprite->yPosition = parent->yPosition + 0x5C;
            sprite->xPosition = parent->xPosition + 0x94;
            break;
        }
        case 3:
        {
            struct PrimarySpriteData *sprites;
            register u32 index PIN("r1");
            u32 size;

            sprites = gSpriteData;
            size = sizeof(struct PrimarySpriteData);
            index = parentSlot;
            index *= size;
            index += (u32)sprites;
            parent = (struct PrimarySpriteData *)index;
            sprite->yPosition = parent->yPosition + 0x34;
            sprite->xPosition = parent->xPosition + 0xE0;
            break;
        }
        case 4:
        {
            register u32 r5Value PIN("r5");
            register u32 r6Value;
            register u8 *workPtr PIN("r8");
            register s32 trig;
            u32 sinIndex;
            s32 inverse;

            {
                struct PrimarySpriteData *sprites;
                register u32 index PIN("r1");
                u32 size;

                sprites = gSpriteData;
                size = sizeof(struct PrimarySpriteData);
                index = parentSlot;
                index *= size;
                index += (u32)sprites;
                parent = (struct PrimarySpriteData *)index;
                sprite->yPosition = parent->yPosition + 0x38;
                sprite->xPosition = parent->xPosition + 0xF8;
            }
            workPtr = &sprite->work2;
            if (*workPtr != 0) {
                r5Value = (u32)sSinCosTable;
                r6Value = (u32)&sprite->work0;

                sinIndex = *(u8 *)r6Value;
                sinIndex += 0x40;
                sinIndex <<= 1;
                sinIndex += r5Value;
                trig = *(const s16 *)sinIndex;
                {
                    register u8 *scaleRead PIN("r2");

                    scaleRead = workPtr;
                    inverse = FixedInverse(*scaleRead);
                }
                sprite->affinePA = FixedMul(trig, inverse);

                sinIndex = *(u8 *)r6Value;
                sinIndex <<= 1;
                sinIndex += r5Value;
                {
                    /* Keep r2 live across each C signed-halfword load so agbcc uses
                     * r1 for the zero offset. Both ASM constraints emit no instructions.
                     * All seven paired lifetimes are required for an exact object match. */
                    register u32 r2Live PIN("r2");
                    ASM_BARRIER(: "=r"(r2Live));
                    trig = *(const s16 *)sinIndex;
                    ASM_BARRIER(: : "r"(r2Live));
                }
                {
                    register u8 *scaleRead PIN("r2");

                    scaleRead = workPtr;
                    inverse = FixedInverse(*scaleRead);
                }
                sprite->affinePB = FixedMul(trig, inverse);

                sinIndex = *(u8 *)r6Value;
                sinIndex <<= 1;
                sinIndex += r5Value;
                {
                    register u32 r2Live PIN("r2");
                    ASM_BARRIER(: "=r"(r2Live));
                    trig = *(const s16 *)sinIndex;
                    ASM_BARRIER(: : "r"(r2Live));
                    trig = -trig;
                }
                {
                    register u8 *scaleRead PIN("r2");

                    scaleRead = workPtr;
                    inverse = FixedInverse(*scaleRead);
                }
                sprite->affinePC = FixedMul(trig, inverse);

                sinIndex = *(u8 *)r6Value;
                sinIndex += 0x40;
                sinIndex <<= 1;
                sinIndex += r5Value;
                {
                    register u32 r2Live PIN("r2");
                    ASM_BARRIER(: "=r"(r2Live));
                    trig = *(const s16 *)sinIndex;
                    ASM_BARRIER(: : "r"(r2Live));
                }
                {
                    register u8 *scaleRead PIN("r2");

                    scaleRead = workPtr;
                    inverse = FixedInverse(*scaleRead);
                }
                sprite->affinePD = FixedMul(trig, inverse);
            } else {
                r6Value = (u32)sSinCosTable;
                {
                    register u32 angleValue;

                    angleValue = 39;
                    ASM_BARRIER(: "+r"(angleValue));
                    angleValue += (u32)sprite;
                    workPtr = (u8 *)angleValue;
                    angleValue = *(u8 *)angleValue;
                    angleValue += 0x40;
                    angleValue <<= 1;
                    angleValue += r6Value;
                    {
                        register u32 r2Live PIN("r2");
                        ASM_BARRIER(: "=r"(r2Live));
                        trig = *(const s16 *)angleValue;
                        ASM_BARRIER(: : "r"(r2Live));
                    }
                }
                r5Value = 0x100;
                inverse = FixedInverse(r5Value);
                sprite->affinePA = FixedMul(trig, inverse);

                {
                    register u8 *angleRead PIN("r2");

                    angleRead = workPtr;
                    sinIndex = *angleRead;
                }
                sinIndex <<= 1;
                sinIndex += r6Value;
                {
                    register u32 r2Live PIN("r2");
                    ASM_BARRIER(: "=r"(r2Live));
                    trig = *(const s16 *)sinIndex;
                    ASM_BARRIER(: : "r"(r2Live));
                }
                inverse = FixedInverse(r5Value);
                sprite->affinePB = FixedMul(trig, inverse);

                {
                    register u8 *angleRead PIN("r2");

                    angleRead = workPtr;
                    sinIndex = *angleRead;
                }
                sinIndex <<= 1;
                sinIndex += r6Value;
                {
                    register u32 r2Live PIN("r2");
                    ASM_BARRIER(: "=r"(r2Live));
                    trig = *(const s16 *)sinIndex;
                    ASM_BARRIER(: : "r"(r2Live));
                    trig = -trig;
                }
                inverse = FixedInverse(r5Value);
                sprite->affinePC = FixedMul(trig, inverse);

                {
                    register u8 *angleRead PIN("r2");

                    angleRead = workPtr;
                    sinIndex = *angleRead;
                }
                sinIndex += 0x40;
                sinIndex <<= 1;
                sinIndex += r6Value;
                {
                    register u32 r2Live PIN("r2");
                    ASM_BARRIER(: "=r"(r2Live));
                    trig = *(const s16 *)sinIndex;
                    ASM_BARRIER(: : "r"(r2Live));
                }
                inverse = FixedInverse(r5Value);
                sprite->affinePD = FixedMul(trig, inverse);
            }
            break;
        }
        default:
            sprite->status = 0;
            break;
        }
    }
}

void SpriteCuckooCondorPendulumPart(void)
{
    u32 base;
    u32 roomSlot;
    struct PrimarySpriteData *sprite;

    base = (u32)&gCurrentSprite;
    roomSlot = ((struct PrimarySpriteData *)base)->roomSlot;
    sprite = (struct PrimarySpriteData *)base;
    if (roomSlot != 0)
        sprite->disableWarioCollisionTimer = 1;

    switch (sprite->pose) {
    case 0:
        InitCuckooCondorPendulumPart();
        break;
    case 16:
        UpdateCuckooCondorPendulumPart();
        break;
    case 33:
        CuckooCondorPendulumStartClockwiseSwing();
    case 34:
        CuckooCondorPendulumSwingClockwise();
        break;
    case 31:
        CuckooCondorPendulumStartCounterclockwiseSwing();
    case 32:
        CuckooCondorPendulumSwingCounterclockwise();
        break;
    case 24:
        CuckooCondorPendulumPause();
        break;
    case 18:
        CuckooCondorPendulumReturnCounterclockwise();
        break;
    case 20:
        CuckooCondorPendulumReturnClockwise();
        break;
    case 39:
        CuckooCondorPendulumFallRight();
    case 40:
        SpriteUtilFallOffscreenRight();
        break;
    case 41:
        CuckooCondorPendulumFallLeft();
    case 42:
        SpriteUtilFallOffscreenLeft();
        break;
    default:
        sprite->pose = 16;
        break;
    }
}

void SpriteCuckooCondorHead(void)
{
    struct PrimarySpriteData *sprite;
    s32 pose;
    struct PrimarySpriteData *current;
    u32 slot;
    register struct PrimarySpriteData *sprites PIN("r3");
    u32 size;
    register u32 base PIN("r3");
    register u32 out PIN("r0");
    register u32 yoff PIN("r3");

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    pose = sprite->pose;
    switch (pose) {
    case 105:
    case 106:
        CuckooCondorHeadAttack();
        goto recover;
    case 0:
        InitCuckooCondorHead();
        break;
    case 16:
        UpdateCuckooCondorHead();
        break;
    case 18:
        CuckooCondorHeadStartAttack();
        break;
    case 20:
    recover:
        CuckooCondorHeadRecover();
        break;
    case 22:
        CuckooCondorHeadSpawnDebris();
        break;
    }

    current = &gCurrentSprite;
    size = (u32)current;
    size += 0x2A;
    slot = *(u8 *)size;
    sprites = gSpriteData;
    size = sizeof(struct PrimarySpriteData);
    slot *= size;
    slot += (u32)sprites;
    base = 0xFF68;
    ASM_BARRIER(: "+r"(base));
    out = base;
    yoff = ((struct PrimarySpriteData *)slot)->yPosition;
    out += yoff;
    current->yPosition = out;
    out = ((struct PrimarySpriteData *)slot)->xPosition;
    current->xPosition = out;
}

void SpriteCuckooCondorClockBody(void)
{
    struct PrimarySpriteData *sprite;
    u32 pose;
    struct PrimarySpriteData *current;
    u32 parentSlot;
    struct PrimarySpriteData *parent;
    u32 size;
    u32 threshold;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    pose = sprite->pose;
    if (pose == 0x27)
        goto pose27;
    if ((s32)pose > 0x27)
        goto greater;
    if (pose == 0)
        goto pose0;
    goto updatePosition;

greater:
    if (pose == 0x28)
        goto fall;
    goto updatePosition;

pose0:
    InitCuckooCondorClockBody();
    goto updatePosition;

pose27:
    CuckooCondorClockBodyStartFall();
fall:
    SpriteUtilFallOffscreenRight();
    return;

updatePosition:
    current = &gCurrentSprite;
    pose = (u32)current;
    pose += 0x2A;
    parentSlot = *(u8 *)pose;
    parent = gSpriteData;
    size = sizeof(struct PrimarySpriteData);
    size *= parentSlot;
    parent = (struct PrimarySpriteData *)(size + (u32)parent);
    parentSlot = parent->health;
    threshold = gBgAnimationFrame;
    threshold >>= 1;
    threshold += 1;
    if ((s32)parentSlot >= (s32)threshold) {
        current->yPosition = parent->yPosition;
        current->xPosition = parent->xPosition;
    }
}

void SpriteCuckooCondorSmallDebris(void)
{
    u8 pose;

    pose = gCurrentSprite.pose;
    if (pose == 0)
        goto pose0;
    if (pose == 0x1C)
        goto pose1C;
    UpdateCuckooCondorSmallDebris();
    goto end;
pose0:
    InitCuckooCondorSmallDebris();
    goto end;
pose1C:
    UpdateCuckooCondorSmallDebrisFall();
end:
    return;
}

void SpriteCuckooCondorLargeDebris(void)
{
    const s16 *table;
    register s32 trig PIN("r4");
    s32 scale;
    struct PrimarySpriteData *sprite;
    u8 *anglePtr;
    u32 sinIndex;
    s32 inverse;

    switch (gCurrentSprite.pose) {
    case 0:
        InitCuckooCondorLargeDebris();
    case 0x1C:
        UpdateCuckooCondorLargeDebrisFall();
        break;
    default:
        UpdateCuckooCondorLargeDebris();
        break;
    }

    table = sSinCosTable;
    sprite = &gCurrentSprite;
    anglePtr = &sprite->work0;

    sinIndex = *anglePtr;
    sinIndex += 0x40;
    sinIndex <<= 1;
    sinIndex += (u32)table;
    trig = *(const s16 *)sinIndex;
    scale = 0x100;
    inverse = FixedInverse(scale);
    sprite->affinePA = FixedMul(trig, inverse);

    trig = table[*anglePtr];
    inverse = FixedInverse(scale);
    sprite->affinePB = FixedMul(trig, inverse);

    trig = -table[*anglePtr];
    inverse = FixedInverse(scale);
    sprite->affinePC = FixedMul(trig, inverse);

    sinIndex = *anglePtr;
    sinIndex += 0x40;
    sinIndex <<= 1;
    sinIndex += (u32)table;
    trig = *(const s16 *)sinIndex;
    inverse = FixedInverse(scale);
    sprite->affinePD = FixedMul(trig, inverse);
}

void SpriteCuckooCondorEgg(void)
{
    u32 base;
    u32 pose;
    struct PrimarySpriteData *sprite;

    base = (u32)&gCurrentSprite;
    pose = ((struct PrimarySpriteData *)base)->pose;
    sprite = (struct PrimarySpriteData *)base;
    switch (pose) {
    case 0:
        InitCuckooCondorEgg();
        break;
    case 29:
        CuckooCondorEggStartFall();
    case 30:
        func_8023FA8();
        break;
    case 31:
    case 79:
        SpawnSecondarySprite(sprite->yPosition - 0x20, sprite->xPosition - 0x20, 6);
    case 106:
        CuckooCondorEggFallOffscreenRight();
    case 32:
        SpriteUtilFallOffscreenRight();
        break;
    case 33:
    case 80:
        SpawnSecondarySprite(sprite->yPosition - 0x20, sprite->xPosition + 0x20, 6);
    case 105:
        CuckooCondorEggFallOffscreenLeft();
    case 34:
        SpriteUtilFallOffscreenLeft();
        break;
    case 49:
    case 107:
        SpriteUtilDie();
        break;
    case 35:
    case 51:
        CuckooCondorEggStartSlideRight();
    case 52:
        func_8024A3C();
        break;
    case 37:
    case 53:
        CuckooCondorEggStartSlideLeft();
    case 54:
        func_8024820();
        break;
    case 67:
        func_8024AC0();
        goto call_8024AD4;
    case 69:
        func_8024BEC();
        goto call_8024C00;
    case 71:
        CuckooCondorEggBounceRight();
call_8024AD4:
    case 68:
    case 72:
        func_8024AD4();
        break;
    case 73:
        CuckooCondorEggBounceLeft();
call_8024C00:
    case 70:
    case 74:
        func_8024C00();
        break;
    case 81:
        CuckooCondorEggLiftRight();
        break;
    case 82:
        SpriteUtilLiftingSpriteRight();
        break;
    case 83:
        CuckooCondorEggLiftLeft();
        break;
    case 84:
        SpriteUtilLiftingSpriteLeft();
        break;
    case 87:
        CuckooCondorEggCarryRight();
        break;
    case 88:
        SpriteUtilCarryingSpriteRight();
        break;
    case 85:
        CuckooCondorEggCarryLeft();
        break;
    case 86:
        SpriteUtilCarryingSpriteLeft();
        break;
    case 89:
        sprite->pose = 0x5A;
    case 90:
        SpriteUtilThrownLeftSoft();
        break;
    case 93:
        sprite->pose = 0x5E;
    case 94:
        SpriteUtilThrownLeftHard();
        break;
    case 97:
        sprite->pose = 0x62;
    case 98:
        SpriteUtilThrownUpLeftSoft();
        break;
    case 101:
        sprite->pose = 0x66;
    case 102:
        SpriteUtilThrownUpLeftHard();
        break;
    case 91:
        sprite->pose = 0x5C;
    case 92:
        SpriteUtilThrownRightSoft();
        break;
    case 95:
        sprite->pose = 0x60;
    case 96:
        SpriteUtilThrownRightHard();
        break;
    case 99:
        sprite->pose = 0x64;
    case 100:
        SpriteUtilThrownUpRightSoft();
        break;
    case 103:
        sprite->pose = 0x68;
    case 104:
        SpriteUtilThrownUpRightHard();
        break;
    case 123:
        CuckooCondorEggHatch();
        break;
    default:
        CuckooCondorEggStartHatching();
        break;
    }
}

void SpriteCuckooCondorChick(void)
{
    struct PrimarySpriteData *sprite;
    const s16 *table;
    u8 *anglePtr;
    register s32 trig PIN("r4");
    s32 scale;
    register u32 sinIndex PIN("r0");
    s32 inverse;
    struct PrimarySpriteData *current;
    u8 *timer;

    switch (gCurrentSprite.pose) {
    case 0:
        InitCuckooCondorChick();
        break;
    case 28:
        func_8023D48();
        break;
    case 23:
        CuckooCondorChickWaitToMove();
        break;
    case 16:
        UpdateCuckooCondorChickWalk();
        break;
    case 35:
        CuckooCondorChickStartDashRight();
    case 36:
        UpdateCuckooCondorChickDashRight();
        break;
    case 37:
        CuckooCondorChickStartDashLeft();
    case 38:
        UpdateCuckooCondorChickDashLeft();
        break;
    case 51:
        CuckooCondorChickFallRight();
        break;
    case 31:
    case 106:
        CuckooCondorChickKnockbackRight();
    case 32:
        SpriteUtilFallOffscreenRight();
        sprite = &gCurrentSprite;
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        table = sSinCosTable;
        anglePtr = &sprite->work0;
        sinIndex = *anglePtr;
        sinIndex += 0x40;
        sinIndex <<= 1;
        sinIndex += (u32)table;
        trig = *(const s16 *)sinIndex;
        scale = 0x100;
        inverse = FixedInverse(scale);
        sprite->affinePA = FixedMul(trig, inverse);
        trig = table[*anglePtr];
        inverse = FixedInverse(scale);
        sprite->affinePB = FixedMul(trig, inverse);
        trig = -table[*anglePtr];
        inverse = FixedInverse(scale);
        sprite->affinePC = FixedMul(trig, inverse);
        sinIndex = *anglePtr;
        sinIndex += 0x40;
        sinIndex <<= 1;
        sinIndex += (u32)table;
        trig = *(const s16 *)sinIndex;
        inverse = FixedInverse(scale);
        sprite->affinePD = FixedMul(trig, inverse);
        *anglePtr += 8;
        break;
    case 53:
        CuckooCondorChickFallLeft();
        break;
    case 33:
    case 105:
        CuckooCondorChickKnockbackLeft();
    case 34:
        SpriteUtilFallOffscreenLeft();
        sprite = &gCurrentSprite;
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        table = sSinCosTable;
        anglePtr = &sprite->work0;
        sinIndex = *anglePtr;
        sinIndex += 0x40;
        sinIndex <<= 1;
        sinIndex += (u32)table;
        trig = *(const s16 *)sinIndex;
        scale = 0x100;
        inverse = FixedInverse(scale);
        sprite->affinePA = FixedMul(trig, inverse);
        trig = table[*anglePtr];
        inverse = FixedInverse(scale);
        sprite->affinePB = FixedMul(trig, inverse);
        trig = -table[*anglePtr];
        inverse = FixedInverse(scale);
        sprite->affinePC = FixedMul(trig, inverse);
        sinIndex = *anglePtr;
        sinIndex += 0x40;
        sinIndex <<= 1;
        sinIndex += (u32)table;
        trig = *(const s16 *)sinIndex;
        inverse = FixedInverse(scale);
        sprite->affinePD = FixedMul(trig, inverse);
        *anglePtr -= 8;
        break;
    case 110:
        CuckooCondorChickStartChase();
    case 111:
        UpdateCuckooCondorChickChase();
        break;
    case 112:
        CuckooCondorChickStartExplosion();
    case 113:
        UpdateCuckooCondorChickExplosion();
        break;
    default:
        gCurrentSprite.work1 = 0;
        SpriteUtilDie();
        break;
    }

    current = &gCurrentSprite;
    timer = &current->work1;
    if (*timer != 0) {
        (*timer)--;
        if (*timer == 0)
            current->pose = 0x6E;
    }
}

#include "sprite_ai/cractus.h"

#include "color_effects.h"
#include "fixed_point.h"
#include "global_data.h"
#include "input.h"
#include "main.h"
#include "score.h"
#include "voice_set.h"
#include "gba/m4a.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

extern u8 gInitialHealth;
extern u8 gUnk_3000A62;
extern u8 gBgAnimationFrame;
extern u8 gBgAnimationTimer;
extern u8 gSpriteAiDynamicGraphicsTimer;

extern GBA_PTR(const void *) const sUnk_878F170[];

void UpdateBossHealthGauge(void);

u32 UpdateCractusGraphicsAnimation(const u8 *animation)
{
    const u8 *data;
    u32 finished;
    struct PrimarySpriteData *sprite;
    u8 *delay;
    u8 *frameIndex;
    u32 value;
    u32 index;
    u32 frame;
    volatile u32 *dma;
    const void *source;
    register GBA_TYPEOF(const void *const *, &(sUnk_878F170)[0]) sourceTable PIN("r2");
    u32 control;

    data = animation;
    if (*(u8*)&gCuckooCondorPendulumLength != 0)
        data = sCractusBossDefaultGraphicsAnimation;

    finished = 0;
    sprite = &gCurrentSprite;
    delay = &sprite->work1;
    value = *delay;
    if (value != 0) {
        value--;
        *delay = value;
        return 0;
    }

    frameIndex = &sprite->work0;
    value = *frameIndex;
    index = value + 1;
    *frameIndex = index;
    value <<= 24;
    index = value >> 24;
    value = index << 2;
    value = (u32)(value + data);
    frame = *(const u8 *)value;
    if (frame == 0xFF) {
        index = 0;
        value = 1;
        *frameIndex = value;
        frame = data[0];
        finished = 1;
    }

    value = index << 2;
    value = (u32)(value + data);
    value = *((const u8 *)value + 1);
    *delay = value;
    dma = (volatile u32 *)0x40000D4;
    sourceTable = sUnk_878F170;
    value = frame << 2;
    value += (u32)sourceTable;
    source = GP(*(GBA_PTR(const void *) const *)value);
    dma[0] = (u32)source;
    dma[1] = 0x6014000;
    control = 0x80000070;
    dma[2] = control;
    value = dma[2];
    source = (const u8 *)source + 0xE0;
    dma[0] = (u32)source;
    dma[1] = 0x6014400;
    dma[2] = control;
    value = dma[2];
    return finished;
}

void UpdateCractusSegmentHorizontalFollow(void)
{
    u32 threshold;
    u32 shift;
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *sprites PIN("r4");
    register u32 current PIN("r5");
    u32 target;
    register u32 value PIN("r0");
    u32 speed;
    register u32 coordinate PIN("r2");
    register u32 index PIN("r2");

    if (*(u8*)&gBossState == 0) {
        threshold = 4;
        shift = 1;
    } else {
        threshold = 8;
        shift = 2;
    }

    sprite = &gCurrentSprite;
    value = 128;
    value <<= 2;
    ASM_BARRIER(: "+r"(value));
    target = value;
    coordinate = sprite->xPosition;
    value = target + coordinate;
    current = (u16)value;
    sprites = gSpriteData;
    index = sprite->work3;
    value = sizeof(struct PrimarySpriteData);
    value *= index;
    value += (u32)sprites;
    value = ((struct PrimarySpriteData *)value)->xPosition;
    target += value;
    target <<= 16;
    target >>= 16;

    value = current - target;
    if ((s32)value > 4) {
        value = target + 4;
        target = (u16)value;
        if (current > target)
            value = current - target;
        else
            value = target - current;
        value = (u16)value;
        if ((s32)value <= (s32)threshold) {
            speed = 0;
        } else {
            value -= threshold;
            value = (s32)value >> shift;
            speed = (u16)value;
        }
        if (current > target) {
            value = sprite->xPosition;
            value -= speed;
            sprite->xPosition = value;
        } else if (current < target) {
            value = sprite->xPosition;
            value = speed + value;
            sprite->xPosition = value;
        }
    } else {
        value = target - current;
        if ((s32)value <= 4)
            return;
        value = target - 4;
        target = (u16)value;
        if (current > target)
            value = current - target;
        else
            value = target - current;
        value = (u16)value;
        if ((s32)value <= (s32)threshold) {
            speed = 0;
        } else {
            value -= threshold;
            value = (s32)value >> shift;
            speed = (u16)value;
        }
        if (current < target) {
            value = sprite->xPosition;
            value = speed + value;
            sprite->xPosition = value;
        } else if (current > target) {
            value = sprite->xPosition;
            value -= speed;
            sprite->xPosition = value;
        }
    }
}

void UpdateCractusSegmentVerticalFollow(void)
{
    u32 shift;
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *initial;
    u32 roomSlot;
    struct PrimarySpriteData *sprites;
    u32 current;
    register u32 target PIN("r1");
    register u32 value PIN("r0");
    register u32 speed PIN("r2");
    u32 index;

    value = *(u8*)&gBossState;
    shift = 1;
    if (value != 0) {
        shift = 2;
        if (value == 2)
            shift = 3;
    }

    initial = &gCurrentSprite;
    roomSlot = initial->roomSlot;
    speed = 48;
    sprite = initial;
    if (roomSlot == 0)
        speed = 64;
    value = sprite->yPosition;
    value -= speed;
    speed = 128;
    speed <<= 2;
    ASM_BARRIER(: "+r"(speed));
    target = speed;
    value += target;
    current = (u16)value;
    sprites = gSpriteData;
    index = sprite->work3;
    value = sizeof(struct PrimarySpriteData);
    value *= index;
    value += (u32)sprites;
    value = ((struct PrimarySpriteData *)value)->yPosition;
    target += value;
    target = (u16)target;

    if (current > target)
        value = current - target;
    else
        value = target - current;
    value = (u16)value;
    if (value <= 1) {
        speed = 0;
    } else {
        value -= 2;
        value = (s32)value >> shift;
        speed = (u16)value;
        if (speed == 0)
            speed = 1;
    }
    ASM_BARRIER(: : : "memory");
    if (current > target) {
        value = sprite->yPosition;
        value -= speed;
        sprite->yPosition = value;
    } else if (current < target) {
        value = sprite->yPosition;
        value = speed + value;
        sprite->yPosition = value;
    }
}

void UpdateCractusSegmentVerticalSpacing(void)
{
    u32 threshold;
    u32 shift;
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *sprites PIN("r4");
    register u32 current PIN("r5");
    u32 target;
    register u32 value PIN("r0");
    u32 speed;
    register u32 coordinate PIN("r2");
    register u32 index PIN("r2");

    if (*(u8*)&gBossState == 0) {
        threshold = 4;
        shift = 1;
    } else {
        threshold = 8;
        shift = 2;
    }

    sprite = &gCurrentSprite;
    value = 128;
    value <<= 2;
    ASM_BARRIER(: "+r"(value));
    target = value;
    coordinate = sprite->yPosition;
    value = target + coordinate;
    current = (u16)value;
    sprites = gSpriteData;
    index = sprite->work3;
    value = sizeof(struct PrimarySpriteData);
    value *= index;
    value += (u32)sprites;
    value = ((struct PrimarySpriteData *)value)->yPosition;
    target += value;
    target <<= 16;
    target >>= 16;

    value = current - target;
    if ((s32)value > 32) {
        value = target;
        value += 32;
        target = (u16)value;
        if (current > target)
            value = current - target;
        else
            value = target - current;
        value = (u16)value;
        if ((s32)value <= (s32)threshold) {
            speed = 0;
        } else {
            value -= threshold;
            value = (s32)value >> shift;
            speed = (u16)value;
        }
        if (current > target) {
            value = sprite->yPosition;
            value -= speed;
            sprite->yPosition = value;
        } else if (current < target) {
            value = sprite->yPosition;
            value = speed + value;
            sprite->yPosition = value;
        }
    } else {
        value = target - current;
        if ((s32)value <= 32)
            return;
        value = target;
        value -= 32;
        target = (u16)value;
        if (current > target)
            value = current - target;
        else
            value = target - current;
        value = (u16)value;
        if ((s32)value <= (s32)threshold) {
            speed = 0;
        } else {
            value -= threshold;
            value = (s32)value >> shift;
            speed = (u16)value;
        }
        if (current < target) {
            value = sprite->yPosition;
            value = speed + value;
            sprite->yPosition = value;
        } else if (current > target) {
            value = sprite->yPosition;
            value -= speed;
            sprite->yPosition = value;
        }
    }
}

void PositionCractusSegmentAtQuarter(void)
{
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *sprite;
    register u32 firstY PIN("r4");
    u32 firstX;
    u32 secondY;
    register u32 secondX PIN("r5");
    register u32 value PIN("r0");
    u32 size;

    sprites = gSpriteData;
    sprite = &gCurrentSprite;
    value = sprite->work2;
    size = sizeof(struct PrimarySpriteData);
    value *= size;
    value += (u32)sprites;
    firstY = ((struct PrimarySpriteData *)value)->yPosition;
    firstX = ((struct PrimarySpriteData *)value)->xPosition;
    value = sprite->work3;
    value *= size;
    value += (u32)sprites;
    secondY = ((struct PrimarySpriteData *)value)->yPosition;
    secondX = ((struct PrimarySpriteData *)value)->xPosition;
    sprites = sprite;

    if (secondY > firstY) {
        value = (u16)(secondY - firstY) >> 2;
        value *= sprites->roomSlot;
        value = secondY - value;
    } else {
        value = (u16)(firstY - secondY) >> 2;
        value *= sprites->roomSlot;
        value = secondY + value;
    }
    sprites->yPosition = value;

    if (secondX > firstX) {
        value = (u16)(secondX - firstX) >> 2;
        value *= sprites->roomSlot;
        value = secondX - value;
    } else {
        value = (u16)(firstX - secondX) >> 2;
        value *= sprites->roomSlot;
        value = secondX + value;
    }
    sprites->xPosition = value;
}

void PositionCractusSegmentAtOneEighth(void)
{
    struct PrimarySpriteData *sprites;
    register struct PrimarySpriteData *sprite PIN("r1");
    u32 firstY;
    u32 firstX;
    u32 secondY;
    u32 secondX;
    u32 value;
    u32 size;

    sprites = gSpriteData;
    sprite = &gCurrentSprite;
    value = sprite->work2;
    size = sizeof(struct PrimarySpriteData);
    value *= size;
    value += (u32)sprites;
    firstY = ((struct PrimarySpriteData *)value)->yPosition;
    firstX = ((struct PrimarySpriteData *)value)->xPosition;
    value = sprite->work3;
    value *= size;
    value += (u32)sprites;
    secondY = ((struct PrimarySpriteData *)value)->yPosition;
    secondX = ((struct PrimarySpriteData *)value)->xPosition;

    if (secondY > firstY)
        value = firstY + ((u16)(secondY - firstY) >> 3);
    else
        value = firstY - ((u16)(firstY - secondY) >> 3);
    sprite->yPosition = value;

    if (secondX > firstX)
        value = firstX + ((u16)(secondX - firstX) >> 3);
    else
        value = firstX - ((u16)(firstX - secondX) >> 3);
    sprite->xPosition = value;
}

void PositionCractusSegmentAtThreeEighths(void)
{
    struct PrimarySpriteData *sprites;
    register struct PrimarySpriteData *sprite PIN("r1");
    struct PrimarySpriteData *current;
    u32 firstY;
    u32 firstX;
    u32 secondY;
    u32 secondX;
    register u32 value PIN("r0");
    register u32 delta PIN("r1");
    u32 size;

    sprites = gSpriteData;
    sprite = &gCurrentSprite;
    value = sprite->work2;
    size = sizeof(struct PrimarySpriteData);
    value *= size;
    value += (u32)sprites;
    firstY = ((struct PrimarySpriteData *)value)->yPosition;
    firstX = ((struct PrimarySpriteData *)value)->xPosition;
    value = sprite->work3;
    value *= size;
    value += (u32)sprites;
    secondY = ((struct PrimarySpriteData *)value)->yPosition;
    secondX = ((struct PrimarySpriteData *)value)->xPosition;
    current = sprite;

    if (secondY > firstY) {
        delta = (u16)(secondY - firstY) >> 3;
        value = (delta << 1) + delta;
        value = firstY + value;
    } else {
        delta = (u16)(firstY - secondY) >> 3;
        value = (delta << 1) + delta;
        value = firstY - value;
    }
    current->yPosition = value;

    if (secondX > firstX) {
        delta = (u16)(secondX - firstX) >> 3;
        value = (delta << 1) + delta;
        value = firstX + value;
    } else {
        delta = (u16)(firstX - secondX) >> 3;
        value = (delta << 1) + delta;
        value = firstX - value;
    }
    current->xPosition = value;
}

void PositionCractusSegmentAtFiveEighths(void)
{
    struct PrimarySpriteData *sprites;
    register struct PrimarySpriteData *sprite PIN("r1");
    struct PrimarySpriteData *current;
    u32 firstY;
    u32 firstX;
    u32 secondY;
    u32 secondX;
    register u32 value PIN("r0");
    register u32 delta PIN("r1");
    u32 size;

    sprites = gSpriteData;
    sprite = &gCurrentSprite;
    value = sprite->work2;
    size = sizeof(struct PrimarySpriteData);
    value *= size;
    value += (u32)sprites;
    firstY = ((struct PrimarySpriteData *)value)->yPosition;
    firstX = ((struct PrimarySpriteData *)value)->xPosition;
    value = sprite->work3;
    value *= size;
    value += (u32)sprites;
    secondY = ((struct PrimarySpriteData *)value)->yPosition;
    secondX = ((struct PrimarySpriteData *)value)->xPosition;
    current = sprite;

    if (secondY > firstY) {
        delta = (u16)(secondY - firstY) >> 3;
        value = (delta << 2) + delta;
        value = firstY + value;
    } else {
        delta = (u16)(firstY - secondY) >> 3;
        value = (delta << 2) + delta;
        value = firstY - value;
    }
    current->yPosition = value;

    if (secondX > firstX) {
        delta = (u16)(secondX - firstX) >> 3;
        value = (delta << 2) + delta;
        value = firstX + value;
    } else {
        delta = (u16)(firstX - secondX) >> 3;
        value = (delta << 2) + delta;
        value = firstX - value;
    }
    current->xPosition = value;
}

void UpdateCractusArmAngleTowardWario(void)
{
    u8 result;
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *initial;
    struct WarioData *wario;
    u16 spriteY;
    u16 warioY;
    register u16 distance PIN("r2");
    register u32 value PIN("r0");
    u32 quarter;
    u32 spriteX;

    if (gInitialHealth != 0) {
        gCurrentSprite.work0 = 0;
        return;
    }

    result = 0;
    initial = &gCurrentSprite;
    spriteY = initial->yPosition;
    wario = &gWarioData;
    warioY = wario->yPosition;
    value = spriteY;
    value += 64;
    sprite = initial;
    if ((s32)value < (s32)warioY) {
        spriteX = sprite->xPosition;
        value = wario->xPosition;
        value += 128;
        if ((s32)spriteX < (s32)value) {
            value = sprite->work0;
            if (value == 0xF0)
                return;
        }
        value = warioY;
        value -= 64;
        value -= spriteY;
        value <<= 16;
        distance = value >> 16;
        if (distance <= 31)
            goto storeResult;
        if (distance <= 64)
            goto firstGradient;
        result = 0xF0;
        goto storeResult;
firstGradient:
        quarter = value >> 18;
        value = 255;
        value -= quarter;
        goto truncateResult;
    } else {
        spriteX = sprite->xPosition;
        value = wario->xPosition;
        value += 128;
        if ((s32)spriteX < (s32)value)
            return;
        value = warioY;
        value -= 64;
        value = spriteY - value;
        value <<= 16;
        distance = value >> 16;
        if (distance <= 31)
            goto storeResult;
        if (distance <= 64)
            goto secondGradient;
        result = 16;
        goto storeResult;
secondGradient:
        value >>= 18;
truncateResult:
        result = (u8)value;
    }
storeResult:
    value = (u32)sprite;
    value += 39;
    *(u8 *)value = result;
}

void DebugMoveCractusSprite(void)
{
    register u16 buttons PIN("r1");
    struct PrimarySpriteData *sprite;
    u32 value;

    if (gSubGameMode == 8) {
        buttons = gButtonsHeld;
        value = buttons & 32;
        if (value != 0) {
            sprite = &gCurrentSprite;
            sprite->xPosition -= 16;
        } else if ((buttons & 16) != 0) {
            sprite = &gCurrentSprite;
            sprite->xPosition += 16;
        } else if ((buttons & 64) != 0) {
            sprite = &gCurrentSprite;
            value = sprite->yPosition - 16;
            sprite->yPosition = value;
        } else if ((buttons & 128) != 0) {
            sprite = &gCurrentSprite;
            value = sprite->yPosition + 16;
            sprite->yPosition = value;
        }
    }
}

void InitCractusSpawner(void)
{
    struct PrimarySpriteData *sprite;

    gUnk_3000A62 = 90;
    *(u8*)&gBossState = 1;
    *(u8*)&gCuckooCondorPendulumLength = 0;
    gCuckooCondorMoveRight = 0;
    gCuckooCondorHasCapturedWario = 0;
    gInitialHealth = 0;
    gBossSequenceState = 0;
    gBgAnimationFrame = 32;
    gBgAnimationTimer = 16;
    gSpriteAiDynamicGraphicsTimer = 12;

    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->warioCollision = 26;
    sprite->drawDistanceDown = 32;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 128;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 56;
    sprite->hitboxExtentRight = 56;
    GP_SET(sprite->pOamData, sCractusSpawnerIdleOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 1;
    sprite->work1 = 0;
    sprite->health = 12;

    UpdateBossHealthGauge();
    SpawnHighPriorityPrimarySprite(220, 0, 0, sprite->yPosition - 240, sprite->xPosition - 32);
    SpawnHighPriorityPrimarySprite(220, 1, 0, sprite->yPosition - 272, sprite->xPosition - 16);
    SpawnHighPriorityPrimarySprite(220, 2, 0, sprite->yPosition - 272, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(220, 3, 0, sprite->yPosition - 272, sprite->xPosition + 16);
    SpawnHighPriorityPrimarySprite(220, 4, 0, sprite->yPosition - 272, sprite->xPosition + 32);
    SpawnHighPriorityPrimarySprite(216, 0, 0, sprite->yPosition - 384, sprite->xPosition);
    {
        u32 yPosition;
        s32 yOffset;

        yPosition = sprite->yPosition;
        yOffset = -320;
        yPosition += yOffset;
        SpawnHighPriorityPrimarySprite(217, 0, 0, yPosition, sprite->xPosition);
    }
    SpawnHighPriorityPrimarySprite(218, 0, 0, sprite->yPosition - 160, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(217, 1, 0, sprite->yPosition - 272, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(217, 2, 0, sprite->yPosition - 224, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(217, 3, 0, sprite->yPosition - 176, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(217, 4, 0, sprite->yPosition - 128, sprite->xPosition);
    SpawnHighPriorityPrimarySprite(219, 4, 0, sprite->yPosition - 272, sprite->xPosition - 32);
    SpawnHighPriorityPrimarySprite(219, 3, 0, sprite->yPosition - 272, sprite->xPosition - 48);
    SpawnHighPriorityPrimarySprite(219, 2, 0, sprite->yPosition - 272, sprite->xPosition - 64);
    SpawnHighPriorityPrimarySprite(219, 1, 0, sprite->yPosition - 272, sprite->xPosition - 80);
    SpawnHighPriorityPrimarySprite(219, 0, 0, sprite->yPosition - 240, sprite->xPosition - 96);
    sprite->work0 = 60;
    LoadBossSpriteGraphics(131, 8, 4);
}

void CractusSpawnerWaitForBattle(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    if (value != 0) {
        value--;
        *timer = value;
        if ((value << 24) != 0)
            return;
        *gBossDefeatTimer = 7;
    }
    if (*gBossDefeatTimer != 0)
        return;
    if (gCurrentShopItem == 0) {
        if (--gUnk_3000A62 != 0)
            return;
        SpriteUtilStartBossTimer();
    }
    sprite->pose = 2;
}

void CractusSpawnerStartBattle(void)
{
    if (gWarioPauseTimer == 0 && gTimerState == 1) {
        SpriteUtilUnsetAllSpritesHighPriority();
        gCurrentSprite.pose = 15;
    }
}

void InitCractusSpawnerDefeat(void)
{
    struct PrimarySpriteData *sprite;
    int zero;
    int zero2;
    struct PrimarySpriteData *sprites;
    u8 *work;
    u32 slot;
    u32 offset;
    struct PrimarySpriteData *target;
    int pose;

    sprite = &gCurrentSprite;
    zero = 0;
    sprite->pose = 32;
    sprite->currentAnimationFrame = zero;
    zero2 = 0;
    sprite->animationTimer = zero;
    GP_SET(sprite->pOamData, sCractusSpawnerDefeatOam);
    sprite->work0 = 62;
    sprite->health = zero2;
    sprite->drawPriority = zero2;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    SpriteUtilFindSpriteSlotWork3(216);
    sprites = gSpriteData;
    work = (u8 *)sprite;
    work += 42;
    slot = *work;
    offset = sizeof(struct PrimarySpriteData);
    offset *= slot;
    offset += (u32)sprites;
    target = (struct PrimarySpriteData *)offset;
    pose = 110;
    target->pose = pose;
    m4aSongNumStart(SE_CRACTUS_DEFEAT_START);
    VoiceSetPlay(0);
}

void UpdateCractusSpawnerDefeat(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    value = (u8)value;
    if (value == 0) {
        sprite->status = value;
        SpriteSpawnAsChild(221, 0, 0, sprite->yPosition - 210, sprite->xPosition + 160);
        SpriteSpawnAsChild(222, 0, 0, sprite->yPosition - 210, sprite->xPosition + 192);
    }
}

void InitCractusBoss(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    struct PrimarySpriteData *spriteR1;
    register struct PrimarySpriteData *spriteR2 PIN("r2");
    u16 status;
    register u16 result PIN("r0");
    register int zero PIN("r3");
    u16 zero16;
    u8 *field;
    u8 *fieldR1;
    int extentR1;
    int extentR2;

    sprite = &gCurrentSprite;
    status = sprite->status;
    result = 0x8408;
    zero = 0;
    zero16 = 0;
    result |= status;
    result &= 0xFFFB;
    spriteR1 = sprite;
    spriteR1->status = result;
    result = 60;
    spriteR1->warioCollision = result;
    result = 12;
    spriteR1->health = result;

    field = &sprite->drawDistanceDown;
    extentR1 = 24;
    *field = extentR1;
    field++;
    *field = extentR1;
    field++;
    extentR1 = 32;
    *field = extentR1;
    field++;
    extentR2 = 80;
    *field = extentR2;
    field++;
    *(volatile u8 *)field = extentR1;

    fieldR1 = &sprite->hitboxExtentLeft;
    result = 96;
    *fieldR1 = result;
    field = &sprite->hitboxExtentRight;
    *field = extentR2;

    {
        const struct AnimationFrame *oam;
        oam = sCractusBossInitialOam;
        spriteR2 = sprite;
        GP_SET(spriteR2->pOamData, oam);
    }
    *(u8 *)((u8 *)spriteR2 + 22) = zero;
    spriteR2->animationTimer = zero16;
    result = 1;
    spriteR2->pose = result;
    field = &sprite->work3;
    *field = zero;
    field -= 3;
    *field = zero;
    field++;
    *field = zero;
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossIntroGraphicsAnimation);
}

void UpdateCractusBossPose1(void)
{
    struct PrimarySpriteData *sprite;
    u8 *indexPtr;
    u32 index;
    const s16 *table;
    const s16 *entry;
    u16 velocity;
    int signedVelocity;
    int zero;
    u32 next;
    u8 *store;

    UpdateCractusGraphicsAnimation(sCractusBossIntroGraphicsAnimation);
    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sCractusBossIntroYVelocity;
    entry = (const s16 *)(((u32)index << 1) + (u32)table);
    velocity = *(const u16 *)entry;
    zero = 0;
    signedVelocity = *entry;
    if (signedVelocity == 0x7FFF) {
        velocity = table[0];
        index = 0;
    }
    next = index + 1;
    store = indexPtr;
    *store = next;
    {
        u16 yPosition;
        yPosition = sprite->yPosition;
        yPosition += velocity;
        sprite->yPosition = yPosition;
    }
    if (*gBossDefeatTimer == 0) {
        GP_SET(sprite->pOamData, sCractusBossIntroOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->pose = 24;
    }
}

void UpdateCractusBossPose24(void)
{
    struct PrimarySpriteData *sprite;
    u8 *indexPtr;
    u32 index;
    const s16 *table;
    const s16 *entry;
    register u16 velocity PIN("r4");
    int signedVelocity;
    int zero;
    u32 next;
    u8 *store;

    UpdateCractusGraphicsAnimation(sCractusBossIntroGraphicsAnimation);
    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sCractusBossIntroYVelocity;
    entry = (const s16 *)(((u32)index << 1) + (u32)table);
    velocity = *(const u16 *)entry;
    zero = 0;
    signedVelocity = *entry;
    if (signedVelocity == 0x7FFF) {
        velocity = table[0];
        index = 0;
    }
    next = index + 1;
    store = indexPtr;
    *store = next;
    sprite->yPosition += velocity;
    if ((*(u32 *)&sprite->animationTimer & 0xFFFFFF) == 0x10001)
        m4aSongNumStart(SE_CRACTUS_INTRO_BOUNCE);
}

void InitCractusBossPose111(void)
{
    struct PrimarySpriteData *sprite;
    const struct AnimationFrame *oam;
    int zero;
    int zero8;

    sprite = &gCurrentSprite;
    oam = sCractusBossReadyOam;
    GP_SET(sprite->pOamData, oam);
    zero = 0;
    sprite->currentAnimationFrame = zero;
    zero8 = 0;
    sprite->animationTimer = zero;
    sprite->work0 = zero8;
    sprite->work1 = zero8;
    sprite->pose = 111;
}

void UpdateCractusBossPose111(void)
{
    struct PrimarySpriteData *sprite;
    int zero;
    u8 *work;
    u8 *work3;
    u8 *work2;
    int value;

    if (UpdateCractusGraphicsAnimation(sCractusBossPose111GraphicsAnimation) != 0) {
        sprite = &gCurrentSprite;
        work = &sprite->work0;
        zero = 0;
        *work = zero;
        work++;
        *work = zero;
        UpdateCractusGraphicsAnimation(sCractusBossPose113GraphicsAnimation);
        SpriteUtilFindSpriteWork3(217, 4);
        work3 = &sprite->work3;
        value = *work3;
        work2 = &sprite->work2;
        *work2 = value;
        *work3 = zero;
        sprite->pose = 113;
    }
}

void UpdateCractusBossPose113(void)
{
  struct PrimarySpriteData *sprite;
  struct PrimarySpriteData *target220;
  struct PrimarySpriteData *target219;
  u8 index;
  s16 velocity;
  u8 slot;
  u8 slot219;
  register u32 value PIN("r0");
  UpdateCractusGraphicsAnimation((const u8 *) sCractusBossPose113GraphicsAnimation);
  sprite = &gCurrentSprite;
  index = sprite->work3;
  velocity = sCractusBossPose113YVelocity[index];
  if (velocity == 0x7FFF)
  {
    sprite->work0 = 0;
    sprite->work1 = 0;
    UpdateCractusGraphicsAnimation((const u8 *) sCractusBossPose112GraphicsAnimation);
    sprite->work3 = 0;
    sprite->work2 = 0;
    sprite->pose = 112;
    *(u8*)&gBossState = 2;
    return;
  }
  index++;
  sprite->work3 = index;
  sprite->yPosition += velocity;
  value = index;
  if (value == 72)
  {
    struct PrimarySpriteData *sprites;
    u32 stride;
    register struct PrimarySpriteData *target PIN("r0");
    register u32 offset PIN("r0");
    u32 basePlus4;
    sprites = gSpriteData;
    offset = sprite->work2;
    stride = sizeof(struct PrimarySpriteData);
    offset *= stride;
    target = (struct PrimarySpriteData *) (offset + ((u32) sprites));
    target->currentAnimationFrame = 0;
    offset = sprite->work2;
    offset *= stride;
    target = (struct PrimarySpriteData *) (offset + ((u32) sprites));
    target->animationTimer = 0;
    offset = sprite->work2;
    offset *= stride;
    basePlus4 = ((u32) sprites) + 4;
    offset += basePlus4;
    GP_SET(*((GBA_PTR(const struct AnimationFrame *) *) offset), sCractusBossArmExtendedOam);
    slot = SpriteUtilFindSprite(220, 0);
    slot219 = SpriteUtilFindSprite(219, 0);
    sprite->xPosition += 256;
    {
      u32 address;
      u32 childX;
      address = slot;
      address *= stride;
      address += (u32)sprites;
      childX = *(u16 *)(address + 10);
      childX += 128;
      *(u16 *)(address + 10) = childX;
    }
    {
      u32 address;
      u32 childX;
      address = slot219;
      address *= stride;
      address += (u32)sprites;
      childX = *(u16 *)(address + 10);
      childX += 128;
      *(u16 *)(address + 10) = childX;
    }
    *(u8*)&gBossState = 0;
  }
  else
    if (value == 74)
  {
    sprite->xPosition -= 128;
  }
}

void UpdateCractusBossPose112(void)
{
    struct PrimarySpriteData *sprite;
    u8 slot220;
    u8 slot219;
    u8 counter;
    u32 state;
    s16 velocity;

    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossPose112GraphicsAnimation);
    sprite = &gCurrentSprite;
    counter = sprite->work3;
    velocity = sCractusBossPose15YVelocity[counter];
    if (velocity == 0x7FFF) {
        velocity = sCractusBossPose15YVelocity[0];
        counter = 0;
    }
    sprite->work3 = counter + 1;
    sprite->yPosition += velocity;

    slot220 = SpriteUtilFindSprite(220, 0);
    slot219 = SpriteUtilFindSprite(219, 0);
    sprite->work2++;
    state = sprite->work2;

    if (state <= 15) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        register u32 address PIN("r1");
        struct PrimarySpriteData *target;

        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 12;
        target->xPosition += 4;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 12;
        target->xPosition += 4;
    } else if ((u8)(state - 41) <= 58) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        u32 address;
        register struct PrimarySpriteData *target PIN("r1");

        if (state > 70) {
            sprite->xPosition -= 2;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition -= 1;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition -= 1;
        } else if (state == 60) {
            GP_SET(sprite->pOamData, sCractusBossImpactOam);
            sprite->currentAnimationFrame = 0;
            sprite->animationTimer = 0;
            m4aSongNumStart(SE_CRACTUS_ARM_RISE);
        }
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->xPosition -= 1;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->xPosition -= 1;
    } else if ((u8)(state - 101) <= 62) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        register u32 address PIN("r1");
        struct PrimarySpriteData *target;

        sprite->yPosition -= 1;
        sprite->xPosition -= 10;
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition -= 3;
        target->xPosition -= 12;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition -= 3;
        target->xPosition -= 12;
    } else if ((u8)(state + 91) <= 14) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        register u32 address PIN("r1");
        struct PrimarySpriteData *target;

        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 4;
        target->xPosition += 2;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 4;
        target->xPosition += 2;
    } else if ((u8)(state + 75) <= 18) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        register u32 address PIN("r1");
        struct PrimarySpriteData *target;

        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 2;
        target->xPosition += 2;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 2;
        target->xPosition += 2;
    } else if (state == 200) {
        sprite->work0 = 0;
        sprite->work1 = 0;
        UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
        sprite->work3 = 0;
        sprite->pose = 15;
        GP_SET(sprite->pOamData, sCractusBossReadyOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
    }
}

u32 AreCractusArmsReady(void)
{
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *sprite220;
    struct PrimarySpriteData *sprite219;
    u32 slot;
    u32 size;
    u8 *work;
    int zero;

    slot = SpriteUtilFindSprite(220, 0);
    slot = (u8)slot;
    sprites = gSpriteData;
    size = sizeof(struct PrimarySpriteData);
    slot *= size;
    sprite220 = (struct PrimarySpriteData *)(slot + (u32)sprites);
    if (sprite220->pose == 15) {
        slot = SpriteUtilFindSprite(219, 0);
        slot = (u8)slot;
        slot *= size;
        sprite219 = (struct PrimarySpriteData *)(slot + (u32)sprites);
        if (sprite219->pose == 15) {
            work = &sprite220->work1;
            zero = 0;
            *work = zero;
            work = &sprite219->work1;
            *work = zero;
            return 1;
        }
    }
    work = (u8 *)&gCurrentSprite;
    work += 42;
    zero = 0;
    *work = zero;
    return 0;
}

void UpdateCractusBossPose15(void)
{
    struct PrimarySpriteData *sprite;
    u8 *indexPtr;
    u32 index;
    const s16 *table;
    u16 velocity;
    u32 zero;
    register u32 savedIndex PIN("r8");
    struct WarioData *wario;
    register u32 value PIN("r0");
    s32 signedVelocity;

    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sCractusBossPose15YVelocity;
    value = index << 1;
    value += (u32)table;
    velocity = *(const u16 *)value;
    zero = 0;
    signedVelocity = *(s16 *)(value + zero);
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)table;
        index = 0;
    }
    value = index + 1;
    index = (u8)value;
    *indexPtr = index;
    zero = 0;
    {
        u16 yPosition;
        yPosition = sprite->yPosition;
        yPosition += velocity;
        sprite->yPosition = yPosition;
    }
    savedIndex = index;
    if (index != 1)
        return;

    wario = &gWarioData;
    value = wario->reaction;
    {
        u8 *hurtTimer;
        hurtTimer = &gCuckooCondorHasCapturedWario;
        if (value != 0)
            *hurtTimer = zero;

        value = *hurtTimer;
        if (value == 0) {
            UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
            value = AreCractusArmsReady();
            if (value != 0) {
                sprite->pose = 19;
                *indexPtr = zero;
                {
                    u8 *global;
                    global = (u8*)&gBossState;
                    *global = zero;
                }
                m4aSongNumStart(SE_CRACTUS_ATTACK_SELECT);
            }
            return;
        }

        UpdateCractusGraphicsAnimation((const u8 *)sCractusBossHurtGraphicsAnimation);
        value = *hurtTimer;
        value--;
        *hurtTimer = value;
        value <<= 24;
        if (value == 0) {
            u8 *work;
            work = &sprite->work0;
            *work = zero;
            work++;
            *work = zero;
            UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
        }
    }

    {
        u32 warioX;
        u32 spriteX;

        {
            struct WarioData *warioRead;
            warioRead = wario;
            warioX = warioRead->xPosition;
        }
        spriteX = sprite->xPosition;
        if (warioX < spriteX) {
            func_8023BFC(sprite->yPosition, spriteX - 128);
            if (gSpriteCollisionTileType != 17) {
                value = spriteX - warioX;
                value <<= 16;
                if (value > 0x13F0000) {
                    sprite->pose = 16;
                    {
                        u8 *global;
                        u32 stored;
                        global = (u8*)&gBossState;
                        stored = savedIndex;
                        *global = stored;
                    }
                }
            }
        } else {
            func_8023BFC(sprite->yPosition, spriteX - 64);
            if (gSpriteCollisionTileType != 17) {
                value = AreCractusArmsReady();
                if (value != 0) {
                    sprite->pose = 18;
                    {
                        u8 *global;
                        global = (u8*)&gBossState;
                        *global = zero;
                    }
                    m4aSongNumStart(SE_CRACTUS_SIDE_ATTACK_SELECT);
                }
            }
        }
    }
}

void UpdateCractusBossPose16(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *target;
    u8 *indexPtr;
    u32 index;
    const s16 *table;
    const s16 *entry;
    u16 velocity;
    int signedVelocity;
    u32 value;
    u32 slot;
    u32 size;
    u32 offset;

    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossHurtGraphicsAnimation);
    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sCractusBossPose15YVelocity;
    value = index << 1;
    value = (u32)(value + (u8 *)table);
    entry = (const s16 *)value;
    velocity = *entry;
    signedVelocity = *(const s16 *)entry;
    if (signedVelocity == 0x7FFF) {
        velocity = table[0];
        index = 0;
    }
    value = index + 1;
    *indexPtr = value;
    {
        u16 yPosition;
        yPosition = sprite->yPosition;
        yPosition += velocity;
        sprite->yPosition = yPosition;
    }
    func_8023BFC(sprite->yPosition, sprite->xPosition - 128);
    if (gSpriteCollisionTileType == 17 || gCuckooCondorHasCapturedWario == 0) {
        sprite->pose = 15;
    } else {
        slot = (u8)SpriteUtilFindSprite(220, 0);
        sprites = gSpriteData;
        size = sizeof(struct PrimarySpriteData);
        offset = slot;
        offset *= size;
        offset += (u32)sprites;
        target = (struct PrimarySpriteData *)offset;
        target->xPosition -= 4;
        slot = (u8)SpriteUtilFindSprite(219, 0);
        offset = slot;
        offset *= size;
        offset += (u32)sprites;
        target = (struct PrimarySpriteData *)offset;
        target->xPosition -= 4;
        sprite->xPosition -= 4;
    }
}

void UpdateCractusBossPose18(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *left;
    struct PrimarySpriteData *right;
    register u32 slot220 PIN("r6");
    u32 slot219;
    u8 *indexPtr;
    register u8 *indexAddress PIN("r1");
    u32 indexOffset;
    u32 index;
    register const s16 *table PIN("r5");
    u32 entryAddress;
    u16 velocity;
    s32 signedVelocity;
    u32 ended;
    u32 zero;
    struct PrimarySpriteData *sprites;
    u32 stride;
    register u32 offset PIN("r0");

    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossHurtGraphicsAnimation);
    ended = 0;
    slot220 = (u8)SpriteUtilFindSprite(220, 0);
    slot219 = (u8)SpriteUtilFindSprite(219, 0);
    sprite = &gCurrentSprite;
    indexOffset = 42;
    ASM_BARRIER(: "+r"(indexOffset));
    indexOffset += (u32)sprite;
    indexAddress = (u8 *)indexOffset;
    indexPtr = indexAddress;
    index = *indexAddress;
    table = sCractusBossPose18YVelocity;
    entryAddress = (index << 1) + (u32)table;
    velocity = *(const u16 *)entryAddress;
    zero = 0;
    signedVelocity = *(s16 *)(entryAddress + zero);
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)table;
        index = 0;
        ended = 1;
    }
    {
        u32 nextIndex;
        u8 *destination;

        nextIndex = index + 1;
        destination = indexPtr;
        *destination = nextIndex;
    }
    {
        u16 yPosition;
        yPosition = sprite->yPosition;
        yPosition += velocity;
        sprite->yPosition = yPosition;
    }
    zero = 0;
    sprites = gSpriteData;
    stride = sizeof(struct PrimarySpriteData);
    offset = slot220;
    offset *= stride;
    left = (struct PrimarySpriteData *)(offset + (u32)sprites);
    {
        u16 yPosition;
        yPosition = left->yPosition;
        yPosition += velocity;
        left->yPosition = yPosition;
    }
    offset = slot219;
    offset *= stride;
    right = (struct PrimarySpriteData *)(offset + (u32)sprites);
    {
        u16 yPosition;
        yPosition = right->yPosition;
        yPosition += velocity;
        right->yPosition = yPosition;
    }
    left->work1 = zero;
    right->work1 = zero;
    func_8023BFC(sprite->yPosition, sprite->xPosition - 64);
    if (gSpriteCollisionTileType != 17) {
        left->xPosition += 4;
        right->xPosition += 4;
        sprite->xPosition += 4;
    }
    {
        u32 endedValue;
        endedValue = ended;
        if (endedValue != 0) {
            u8 *destination;
            destination = indexPtr;
            *destination = zero;
            {
                u8 *work2;
                u32 timer;
                work2 = &sprite->work2;
                timer = 18;
                *work2 = timer;
            }
            {
                u32 warioX;
                u32 spriteX;
                warioX = gWarioData.xPosition;
                spriteX = sprite->xPosition;
                if (warioX < spriteX)
                    sprite->pose = 17;
                else
                    sprite->pose = 15;
            }
        }
    }
}

void UpdateCractusBossPose17(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *target;
    u8 *timer;
    u32 value;
    u32 coordinate;
    u32 slot;
    u32 size;
    int zero;

    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossHurtGraphicsAnimation);
    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer - 1;
    *timer = value;
    value <<= 24;
    if (value != 0) {
        slot = (u8)SpriteUtilFindSprite(220, 0);
        sprites = gSpriteData;
        size = sizeof(struct PrimarySpriteData);
        slot *= size;
        slot = (u32)(slot + (u8 *)sprites);
        target = (struct PrimarySpriteData *)slot;
        coordinate = target->xPosition;
        coordinate -= 4;
        zero = 0;
        target->xPosition = coordinate;
        target->work1 = zero;

        slot = (u8)SpriteUtilFindSprite(219, 0);
        slot *= size;
        slot = (u32)(slot + (u8 *)sprites);
        target = (struct PrimarySpriteData *)slot;
        coordinate = target->xPosition;
        coordinate -= 4;
        target->xPosition = coordinate;
        target->work1 = zero;
        {
            u32 currentCoordinate;
            currentCoordinate = sprite->xPosition;
            currentCoordinate -= 4;
            sprite->xPosition = currentCoordinate;
        }
    } else {
        sprite->pose = 15;
    }
}

void UpdateCractusBossPose19(void)
{
    u32 slot220;
    u32 slot219;

    {
        struct PrimarySpriteData *sprite;
        u8 *indexPtr;
        u32 value;

        sprite = &gCurrentSprite;
        sprite->disableWarioCollisionTimer = 1;
        UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
        {
            u32 found;
            found = SpriteUtilFindSprite(220, 0);
            slot220 = (u8)found;
            found = SpriteUtilFindSprite(219, 0);
            slot219 = (u8)found;
        }
        indexPtr = &sprite->work3;
        value = *indexPtr;
        if (value != 0xFF) {
            {
                u32 y;
                register u32 offset PIN("r1");
                register u32 coordinate PIN("r3");
                offset = 230;
                offset <<= 2;
                ASM_BARRIER(: "+r"(offset));
                y = offset;
                ASM_BARRIER(: "+r"(y));
                coordinate = sprite->yPosition;
                y += coordinate;
                y = (u16)y;
                func_8023BFC(y, sprite->xPosition);
            }
            if (gSpriteCollisionTileType != 0) {
                u32 index;
                register const s16 *table;
                u32 address;
                u32 velocity;
                u32 loadZero;
                register s32 signedLoad PIN("r1");
                register s32 signedVelocity PIN("r12");
                register struct PrimarySpriteData *current PIN("r1");
                register struct PrimarySpriteData *sprites PIN("r3");
                register u32 stride PIN("r2");
                u32 target;

                index = *indexPtr;
                table = sCractusBossPose19YVelocity;
                address = index << 1;
                address = (u32)(address + (u8 *)table);
                velocity = *(const u16 *)address;
                loadZero = 0;
                signedLoad = *(const s16 *)((const u8 *)address + loadZero);
                ASM_BARRIER(: "+r"(signedLoad));
                signedVelocity = signedLoad;
                ASM_BARRIER(: "+r"(signedVelocity));
                if (signedVelocity == 0x7FFF) {
                    address = (index - 1) << 1;
                    address = (u32)(address + (u8 *)table);
                    velocity = *(const u16 *)address;
                } else {
                    value = index + 1;
                    *indexPtr = value;
                }

                current = &gCurrentSprite;
                value = current->yPosition;
                value += velocity;
                current->yPosition = value;
                sprites = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                target = slot220;
                target *= stride;
                target += (u32)sprites;
                value = ((struct PrimarySpriteData *)target)->yPosition;
                value += velocity;
                ((struct PrimarySpriteData *)target)->yPosition = value;
                target = slot219;
                target *= stride;
                target += (u32)sprites;
                value = ((struct PrimarySpriteData *)target)->yPosition;
                value += velocity;
                ((struct PrimarySpriteData *)target)->yPosition = value;
                return;
            }
        }
    }

    {
        register struct PrimarySpriteData *sprite;
        u8 *work3;
        u32 value;

        sprite = &gCurrentSprite;
        work3 = &sprite->work3;
        *work3 = 0xFF;
        {
            u16 y;
            u32 x;
            y = sprite->yPosition;
            x = sprite->xPosition;
            x -= 120;
            x = (u16)x;
            func_8023BFC(y, x);
        }
        if (gSpriteCollisionTileType != 17) {
            struct PrimarySpriteData *sprites;
            u32 stride;
            register u32 target PIN("r1");

            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            target = slot220;
            target *= stride;
            target += (u32)sprites;
            ((struct PrimarySpriteData *)target)->xPosition += 16;
            target = slot219;
            target *= stride;
            target += (u32)sprites;
            ((struct PrimarySpriteData *)target)->xPosition += 16;
            sprite->xPosition += 16;
            return;
        }

        {
            struct WarioData *wario;
            register u32 warioReaction;

            wario = &gWarioData;
            warioReaction = wario->reaction;
            if (warioReaction == 0)
                goto be50_no_reaction;
            goto be50_reaction;

be50_no_reaction:

            if ((u8)(wario->pose - 43) <= 2) {
                struct PrimarySpriteData *sprites;
                u32 stride;
                u32 left;
                u32 clearZero;

                sprite->work2 = 32;
                *work3 = warioReaction;
                sprite->pose = 114;
                GP_SET(sprite->pOamData, sCractusBossDamagedOam);
                sprite->currentAnimationFrame = warioReaction;
                clearZero = 0;
                sprite->animationTimer = warioReaction;
                gCuckooCondorMoveRight = clearZero;
                *(u8*)&gBossState = clearZero;
                sprites = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                left = slot220;
                left *= stride;
                left += (u32)sprites;
                ((struct PrimarySpriteData *)left)->yPosition = sprite->yPosition + 192;
                stride *= slot219;
                stride += (u32)sprites;
                ((struct PrimarySpriteData *)stride)->yPosition = sprite->yPosition + 192;
                ((struct PrimarySpriteData *)left)->xPosition = sprite->xPosition + 64;
                ((struct PrimarySpriteData *)stride)->xPosition = sprite->xPosition - 32;
                ((struct PrimarySpriteData *)left)->work2 = clearZero;
                ((struct PrimarySpriteData *)stride)->work2 = clearZero;
            } else {
                struct PrimarySpriteData *sprites;
                u32 stride;
                u32 leftOffset;
                u32 left;
                u32 clearZero;
                struct PrimarySpriteData *clearBase;
                register u32 clearAddress PIN("r0");

                sprite->work2 = 16;
                *work3 = warioReaction;
                sprite->pose = 20;
                GP_SET(sprite->pOamData, sCractusBossDamagedOam);
                sprite->currentAnimationFrame = warioReaction;
                sprite->animationTimer = warioReaction;
                gCuckooCondorMoveRight = 1;
                *(u8*)&gBossState = 1;
                sprites = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                leftOffset = slot220;
                leftOffset *= stride;
                left = leftOffset + (u32)sprites;
                ((struct PrimarySpriteData *)left)->xPosition = sprite->xPosition + 32;
                clearAddress = slot219;
                clearAddress *= stride;
                stride = clearAddress + (u32)sprites;
                ((struct PrimarySpriteData *)stride)->xPosition = sprite->xPosition - 96;
                value = sprite->health;
                clearBase = sprites;
                if (value <= 6) {
                    ((struct PrimarySpriteData *)left)->yPosition = sprite->yPosition + 160;
                    ((struct PrimarySpriteData *)stride)->yPosition = sprite->yPosition + 160;
                } else {
                    ((struct PrimarySpriteData *)left)->yPosition = sprite->yPosition + 64;
                    ((struct PrimarySpriteData *)stride)->yPosition = sprite->yPosition + 64;
                }
                clearAddress = slot220;
                stride = sizeof(struct PrimarySpriteData);
                clearAddress *= stride;
                clearAddress += (u32)clearBase;
                clearAddress += 41;
                clearZero = 0;
                *(u8 *)clearAddress = clearZero;
                clearAddress = slot219;
                clearAddress *= stride;
                clearAddress += (u32)clearBase;
                clearAddress += 41;
                *(u8 *)clearAddress = clearZero;
                m4aSongNumStart(SE_CRACTUS_ARM_SLAM);
            }

            goto be50_end;

be50_reaction:
            {
            u32 zero;
            u8 *work2;
            u32 clearZero;
            struct PrimarySpriteData *sprites;
            u32 stride;
            u32 left;

            work2 = &sprite->work2;
            zero = 0;
            *work2 = 32;
            *work3 = zero;
            sprite->pose = 116;
            GP_SET(sprite->pOamData, sCractusBossDamagedOam);
            sprite->currentAnimationFrame = zero;
            clearZero = 0;
            sprite->animationTimer = zero;
            gCuckooCondorMoveRight = 1;
            *(u8*)&gBossState = 1;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            left = slot220;
            left *= stride;
            left += (u32)sprites;
            ((struct PrimarySpriteData *)left)->yPosition = sprite->yPosition + 92;
            stride *= slot219;
            stride += (u32)sprites;
            ((struct PrimarySpriteData *)stride)->yPosition = sprite->yPosition + 92;
            ((struct PrimarySpriteData *)left)->xPosition = sprite->xPosition + 32;
            ((struct PrimarySpriteData *)stride)->xPosition = sprite->xPosition - 96;
            left += 41;
            *(u8 *)left = clearZero;
            stride += 41;
            *(u8 *)stride = clearZero;
            {
                register u8 *work;
                work = &sprite->work0;
                *work = clearZero;
                work++;
                *work = clearZero;
            }
            UpdateCractusGraphicsAnimation(sCractusBossDefaultGraphicsAnimation);
            }

be50_end:
            ;
        }
    }
}

void UpdateCractusBossPose114(void)
{
    struct PrimarySpriteData *sprite;
    u8 slot220;
    u32 slot219;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    if ((*((u32 *)&sprite->animationTimer) & 0xFFFFFF) == 0x50003) {
        SpriteSpawnAsChild(223, 0, 0, sprite->yPosition + 128, sprite->xPosition - 84);
    }
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
    slot220 = SpriteUtilFindSprite(220, 0);
    slot219 = (u8)SpriteUtilFindSprite(219, 0);

    if (sprite->work3 != 0xFF) {
        func_8023BFC(sprite->yPosition + 512, sprite->xPosition - 138);
        if (gSpriteCollisionTileType != 17) {
            struct PrimarySpriteData *sprites;
            u32 stride;
            register u32 address PIN("r1");
            struct PrimarySpriteData *target;

            sprite->yPosition += 8;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition += 8;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition += 8;
            return;
        }
    }

    {
        struct PrimarySpriteData *current;
        u8 *work3;
        s32 mask;
        u8 *global58;

        current = &gCurrentSprite;
        work3 = &current->work3;
        {
            register s32 maskSeed PIN("r1");
            maskSeed = -1;
            ASM_BARRIER(: : "r"(maskSeed));
            mask = maskSeed;
        }
        *work3 = 255;
        gCuckooCondorMoveRight = 1;
        global58 = (u8*)&gBossState;
        *global58 = 1;
        func_8023BFC(current->yPosition, current->xPosition - 138);
        if (gSpriteCollisionTileType != 17) {
            struct PrimarySpriteData *sprites;
            u32 stride;
            register u32 address PIN("r1");
            struct PrimarySpriteData *target;

            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->xPosition -= 8;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->xPosition -= 8;
            current->xPosition -= 8;
        } else {
            u32 count;
            u32 countByte;

            count = *((u8 *)current + 41);
            count -= 1;
            *((u8 *)current + 41) = count;
            count &= mask;
            countByte = (u8)count;
            if (countByte != 0) {
                struct PrimarySpriteData *sprites;
                u32 stride;
                register u32 address PIN("r1");
                struct PrimarySpriteData *target;

                current->yPosition -= 12;
                sprites = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                address = slot220;
                address *= stride;
                address += (u32)sprites;
                target = (struct PrimarySpriteData *)address;
                target->yPosition -= 12;
                address = slot219;
                address *= stride;
                address += (u32)sprites;
                target = (struct PrimarySpriteData *)address;
                target->yPosition -= 12;
            } else {
                current->pose = 19;
                *work3 = countByte;
                *global58 = countByte;
            }
        }
    }
}

void UpdateCractusBossPose116(void)
{
    struct PrimarySpriteData *sprite;
    u8 slot220;
    u8 slot219;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateCractusGraphicsAnimation(sCractusBossDefaultGraphicsAnimation);
    slot220 = SpriteUtilFindSprite(220, 0);
    slot219 = SpriteUtilFindSprite(219, 0);

    if (sprite->work3 != 0xFF) {
        func_8023BFC(sprite->yPosition + 400, sprite->xPosition - 138);
        if (gSpriteCollisionTileType != 17) {
            struct PrimarySpriteData *sprites;
            u32 stride;
            register u32 address PIN("r1");
            struct PrimarySpriteData *target;

            sprite->yPosition += 8;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition += 8;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition += 8;
            return;
        }
    }

    gCurrentSprite.work3 = 0xFF;
    if (gWarioData.reaction != 0 && (gMainTimer & 0x3F) == 0)
        m4aSongNumStart(SE_CRACTUS_WARIO_REACTION);
    func_8023BFC(gCurrentSprite.yPosition, gCurrentSprite.xPosition - 640);
    if (gSpriteCollisionTileType != 17) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        register u32 address PIN("r1");
        struct PrimarySpriteData *target;

        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->xPosition -= 4;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->xPosition -= 4;
        gCurrentSprite.xPosition -= 4;
    } else if (gWarioData.reaction == 0) {
        gCurrentSprite.work2--;
        if (gCurrentSprite.work2 != 0) {
            struct PrimarySpriteData *sprites;
            u32 stride;
            register u32 address PIN("r1");
            struct PrimarySpriteData *target;

            gCurrentSprite.yPosition -= 12;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition -= 12;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition -= 12;
        } else {
            gCurrentSprite.pose = 19;
            gCurrentSprite.work3 = 0;
            *(u8*)&gBossState = 0;
        }
    }
}

void UpdateCractusBossPose20(void)
{
    struct PrimarySpriteData *sprite;
    u8 slot220;
    u8 slot219;
    u16 collisionY;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
    slot220 = SpriteUtilFindSprite(220, 0);
    slot219 = SpriteUtilFindSprite(219, 0);

    if (sprite->work2 != 0) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        register u32 address PIN("r1");
        struct PrimarySpriteData *target;

        sprite->work2--;
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->xPosition -= 16;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->xPosition -= 16;
        sprite->xPosition -= 16;
        return;
    }

    if (sprite->health <= 6)
        collisionY = sprite->yPosition + 224;
    else
        collisionY = sprite->yPosition - 140;
    func_8023BFC(collisionY, gCurrentSprite.xPosition);

    if (gSpriteCollisionTileType != 17) {
        struct PrimarySpriteData *sprites;
        u32 stride;
        register u32 address PIN("r1");
        struct PrimarySpriteData *target;

        gCurrentSprite.yPosition += 16;
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        address = slot220;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 16;
        address = slot219;
        address *= stride;
        address += (u32)sprites;
        target = (struct PrimarySpriteData *)address;
        target->yPosition += 16;
    } else {
        gCurrentSprite.pose = 21;
        GP_SET(gCurrentSprite.pOamData, sCractusBossImpactOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        if (gCurrentSprite.health <= 6) {
            gCurrentSprite.work2 = 0;
            gCuckooCondorMoveRight = 0;
            m4aSongNumStart(SE_CRACTUS_GROUND_IMPACT);
        } else {
            gCurrentSprite.work2 = 60;
        }
        if (gCurrentSprite.health <= 9) {
            gCuckooCondorHasCapturedWario = 1;
            if (gCurrentSprite.health <= 1) {
                struct PrimarySpriteData *sprites;
                u32 stride;
                u32 address;
                register u8 *work PIN("r0");
                u32 delay;

                sprites = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                address = slot220;
                address *= stride;
                address += (u32)sprites;
                address += 40;
                work = (u8 *)address;
                delay = 30;
                *work = delay;
                address = slot219;
                address *= stride;
                address += (u32)sprites;
                address += 40;
                work = (u8 *)address;
                *work = delay;
            } else {
                struct PrimarySpriteData *sprites;
                u32 stride;
                u32 address;
                register u8 *work PIN("r0");
                u32 delay;

                sprites = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                address = slot220;
                address *= stride;
                address += (u32)sprites;
                address += 40;
                work = (u8 *)address;
                delay = 10;
                *work = delay;
                address = slot219;
                address *= stride;
                address += (u32)sprites;
                address += 40;
                work = (u8 *)address;
                *work = delay;
            }
        }
    }
}

void UpdateCractusBossPose21(void)
{
    struct PrimarySpriteData *initial;
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *target;
    register u32 slot220 PIN("r6");
    u32 slot219;
    u8 *timer;
    u32 value;
    register u32 speed PIN("r5");
    u32 stride;
    register u32 address PIN("r1");

    initial = &gCurrentSprite;
    if (initial->health <= 6 && (*(u32 *)&initial->animationTimer & 0xFFFFFF) == 0x10003) {
        SpriteSpawnAsChild(223, 0, 0, initial->yPosition + 128, initial->xPosition - 84);
    }
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
    slot220 = SpriteUtilFindSprite(220, 0);
    slot220 = (u8)slot220;
    value = SpriteUtilFindSprite(219, 0);
    slot219 = (u8)value;

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer;
    speed = value;
    if (speed != 0) {
        value--;
        *timer = value;
        value = (u8)value;
        if (value > 20) {
            sprite->yPosition -= 8;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition -= 8;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition -= 8;
        } else {
            sprite->yPosition += 4;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition += 4;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->yPosition += 4;
            {
                u8 *timerRead;
                timerRead = timer;
                if (*timerRead == 0)
                    m4aSongNumStart(SE_CRACTUS_GROUND_IMPACT);
            }
        }
        return;
    }

    func_8023BFC(sprite->yPosition, sprite->xPosition - 128);
    if (gSpriteCollisionTileType == 17) {
        GP_SET(sprite->pOamData, sCractusBossReadyOam);
        sprite->currentAnimationFrame = speed;
        {
            u32 zero;
            u32 pose;
            u8 *timerStore;
            u8 *global;

            zero = 0;
            sprite->animationTimer = speed;
            pose = 22;
            sprite->pose = pose;
            pose = 46;
            timerStore = timer;
            *timerStore = pose;
            global = &gCuckooCondorHasCapturedWario;
            *global = zero;
        }
    } else {
        speed = sprite->health;
        if (speed > 3) {
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->xPosition -= 4;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->xPosition -= 4;
            sprite->xPosition = sprite->xPosition - 4;
        } else {
            if (gCuckooCondorHasCapturedWario == 0 || sprite->xPosition < gWarioData.xPosition)
                speed = 6;
            sprites = gSpriteData;
            stride = sizeof(struct PrimarySpriteData);
            address = slot220;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->xPosition -= speed;
            address = slot219;
            address *= stride;
            address += (u32)sprites;
            target = (struct PrimarySpriteData *)address;
            target->xPosition -= speed;
            {
                struct PrimarySpriteData *current;
                current = &gCurrentSprite;
                current->xPosition -= speed;
            }
        }
    }
}

void UpdateCractusBossPose22(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *target;
    register u32 slot220 PIN("r5");
    register u32 slot219 PIN("r4");
    u32 value;
    u32 size;
    u8 *timer;
    register u32 timerValue PIN("r1");

    sprite = &gCurrentSprite;
    if (sprite->health <= 6 && (*(u32 *)&sprite->animationTimer & 0xFFFFFF) == 0x50003) {
        SpriteSpawnAsChild(223, 0, 0, sprite->yPosition + 128, sprite->xPosition - 84);
    }
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossAttackGraphicsAnimation);
    slot220 = SpriteUtilFindSprite(220, 0);
    slot220 = (u8)slot220;
    slot219 = SpriteUtilFindSprite(219, 0);
    slot219 = (u8)slot219;
    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer;
    timerValue = value;
    if (timerValue != 0) {
        *timer = value - 1;
        sprite->yPosition -= 6;
        sprites = gSpriteData;
        size = sizeof(struct PrimarySpriteData);
        timerValue = slot220;
        timerValue *= size;
        timerValue = (u32)(timerValue + (u8 *)sprites);
        target = (struct PrimarySpriteData *)timerValue;
        target->yPosition -= 5;
        timerValue = slot219;
        timerValue *= size;
        timerValue = (u32)(timerValue + (u8 *)sprites);
        target = (struct PrimarySpriteData *)timerValue;
        target->yPosition -= 5;
    } else {
        gCuckooCondorMoveRight = timerValue;
        *(u8*)&gBossState = timerValue;
        sprite->pose = 15;
    }
}

void UpdateCractusBossPose49(void)
{
    u32 slot220;
    u8 slot219;
    struct PrimarySpriteData *initial;
    struct PrimarySpriteData *sprite;
    u32 zeroR1;
    register u32 zeroR0 PIN("r0");
    u32 zeroR4;
    struct PrimarySpriteData *sprites;
    u32 stride;
    u32 address;
    struct PrimarySpriteData *left;
    struct PrimarySpriteData *right;
    struct PrimarySpriteData *current;
    u32 zeroR2;
    u8 *work;
    register u8 *global PIN("r0");
    u32 health;
    u32 coordinate;

    slot220 = (u8)SpriteUtilFindSprite(220, 0);
    slot219 = (u8)SpriteUtilFindSprite(219, 0);
    initial = &gCurrentSprite;
    health = initial->health;
    if (health != 0) {
        health--;
        initial->health = health;
        health <<= 24;
        if (health == 0) {
            gTimerState = 11;
            SpriteUtilSetWarioBossVictoryPose();
        }
        UpdateBossHealthGauge();
    }
    global = &gCuckooCondorMoveRight;
    zeroR1 = 0;
    *global = zeroR1;
    global = (u8*)&gBossState;
    *global = zeroR1;
    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sCractusBossDamagedOam);
    zeroR0 = 0;
    *(volatile u8 *)&sprite->currentAnimationFrame = zeroR0;
    zeroR4 = 0;
    sprite->animationTimer = zeroR0;
    work = &sprite->work0;
    *work = zeroR4;
    work++;
    *work = zeroR4;
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossDamageGraphicsAnimation);
    work = &sprite->work2;
    *work = zeroR4;
    work++;
    *work = zeroR4;
    gInitialHealth = 1;
    SpawnSecondarySprite(gWarioData.yPosition, gWarioData.xPosition, 5);
    func_8023BFC(sprite->yPosition + 352, sprite->xPosition);
    if (gSpriteCollisionTileType != 0)
        sprite->pose = 50;
    else
        sprite->pose = 109;
    VoiceSetPlay(15);
    gCuckooCondorHasCapturedWario = 0;
    sprites = gSpriteData;
    stride = sizeof(struct PrimarySpriteData);
    address = slot220;
    address *= stride;
    left = (struct PrimarySpriteData *)(address + (u32)sprites);
    current = &gCurrentSprite;
    coordinate = current->yPosition;
    coordinate += 64;
    zeroR2 = 0;
    left->yPosition = coordinate;
    address = slot219;
    address *= stride;
    right = (struct PrimarySpriteData *)(address + (u32)sprites);
    right->yPosition = current->yPosition + 64;
    left->xPosition = current->xPosition + 32;
    right->xPosition = current->xPosition - 96;
    left->work2 = zeroR2;
    right->work2 = zeroR2;
    left->work1 = zeroR2;
    right->work1 = zeroR2;
    left->pose = 15;
    right->pose = 15;
}

void HandleCractusBossDamage(void)
{
    struct PrimarySpriteData *sprite;
    u32 zero;
    u32 value;
    u32 extent;
    register u32 health PIN("r1");
    register u8 *globalR0 PIN("r0");
    register u8 *globalR1 PIN("r1");
    u8 *globalR2;
    u8 *fieldR0;
    register u8 *fieldR1 PIN("r1");
    register u32 zeroR1 PIN("r1");

    sprite = &gCurrentSprite;
    zero = 0;
    sprite->palette = zero;
    value = sprite->health;
    if (value == 0) {
        sprite->pose = 121;
        UpdateCractusGraphicsAnimation((const u8 *)sCractusBossDamageGraphicsAnimation);
        globalR1 = &gInitialHealth;
        value = 2;
        *globalR1 = value;
        m4aSongNumStart(SE_DEFEATED);
    } else {
        if (value <= 5) {
            globalR0 = &gBgAnimationFrame;
            extent = 16;
            *globalR0 = extent;
            globalR1 = &gBgAnimationTimer;
            value = 20;
            *globalR1 = value;
            globalR0 = &gSpriteAiDynamicGraphicsTimer;
            *globalR0 = extent;
        }
        globalR0 = &gInitialHealth;
        *globalR0 = zero;
        globalR2 = &gCuckooCondorHasCapturedWario;
        health = sprite->health;
        value = 15;
        value -= health;
        *globalR2 = value;
        value = (u8)value;
        if (value > 10) {
            value = 10;
            *globalR2 = value;
        }
        value = 15;
        sprite->pose = value;
        value = (u32)sCractusBossReadyOam;
        *(u32 *)((u8 *)sprite + 4) = value;
        sprite->currentAnimationFrame = zero;
        zeroR1 = 0;
        sprite->animationTimer = zero;
        fieldR0 = &sprite->work0;
        *fieldR0 = zeroR1;
        fieldR0++;
        *fieldR0 = zeroR1;
        UpdateCractusGraphicsAnimation((const u8 *)sCractusBossHurtGraphicsAnimation);
        fieldR1 = &sprite->work3;
        value = 2;
        *fieldR1 = value;
    }
}

void UpdateCractusBossPose50(void)
{
    struct PrimarySpriteData *initial;
    struct PrimarySpriteData *sprite;
    u8 slot220;
    u8 slot219;
    u32 leftVelocity;
    u32 rightVelocity;
    u8 index;

    initial = &gCurrentSprite;
    initial->disableWarioCollisionTimer = 1;
    if ((*((u32 *)&initial->animationTimer) & 0xFFFFFF) == 0x50003 && initial->health != 0) {
        SpriteSpawnAsChild(223, 0, 0, initial->yPosition + 128, initial->xPosition - 68);
    }

    sprite = &gCurrentSprite;
    {
        u32 value;
        u32 counter;
        u8 *counterPtr;
        u32 zero;

        counterPtr = &sprite->work2;
        value = *counterPtr;
        value++;
        *counterPtr = value;
        counter = (u8)value;
        if (counter == 2)
            VoiceSetPlay(0);
        if ((counter & 7) == 0) {
            u32 paletteValue;
            paletteValue = counter & 8;
            paletteValue = (u8)paletteValue;
            if (paletteValue != 0)
                sprite->palette = 3;
            else
                sprite->palette = 0;
        }
        zero = (counter - 1) & 0x3F;
        if (zero == 0) {
            struct PrimarySpriteData *effectSprite;

            effectSprite = &gCurrentSprite;
            SpawnPrimarySpriteWithStatus(249, 0, 0, effectSprite->yPosition + 28,
                         effectSprite->xPosition - 72, zero);
            SpawnPrimarySpriteWithStatus(249, 0, 0, effectSprite->yPosition + 32,
                         effectSprite->xPosition + 64, 64);
        }
    }

    slot220 = SpriteUtilFindSprite(220, 0);
    slot219 = SpriteUtilFindSprite(219, 0);

    sprite = &gCurrentSprite;
    {
        u8 *indexPtr;
        const s16 *leftPtr;
        const s16 *rightPtr;
        s32 signedVelocity;
        u32 offset;

        indexPtr = &sprite->work3;
        index = *indexPtr;
        leftPtr = sCractusBossPose50LeftYVelocity;
        offset = index << 1;
        leftPtr = (const s16 *)(offset + (u32)leftPtr);
        rightPtr = sCractusBossPose50RightYVelocity;
        offset += (u32)rightPtr;
        rightVelocity = *(const u16 *)offset;
        leftVelocity = *(const u16 *)leftPtr;
        signedVelocity = *leftPtr;

        if (signedVelocity == 0x7FFF) {
            u32 collision;

            func_8023BFC(sprite->yPosition + 352, sprite->xPosition);
            collision = gSpriteCollisionTileType;
            if (collision != 0) {
                struct PrimarySpriteData *base;
                u32 stride;
                register struct PrimarySpriteData *child PIN("r1");

                sprite->yPosition -= 2;
                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                child = (struct PrimarySpriteData *)(slot220 * stride + (u32)base);
                child->yPosition -= 2;
                child = (struct PrimarySpriteData *)(slot219 * stride + (u32)base);
                child->yPosition -= 2;
                goto animate;
            } else {
                struct PrimarySpriteData *base;
                u32 stride;
                struct PrimarySpriteData *left;
                struct PrimarySpriteData *right;

                HandleCractusBossDamage();
                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                left = (struct PrimarySpriteData *)(slot220 * stride + (u32)base);
                left->work1 = collision;
                right = (struct PrimarySpriteData *)(slot219 * stride + (u32)base);
                right->work1 = collision;
                left->yPosition = sprite->yPosition + 160;
                right->yPosition = sprite->yPosition + 160;
                return;
            }
        } else {
            u32 collision;

            *indexPtr = index + 1;
            func_8023BFC(sprite->yPosition + 352, sprite->xPosition);
            collision = gSpriteCollisionTileType;
            if (collision != 0) {
                struct PrimarySpriteData *base;
                u32 stride;
                register struct PrimarySpriteData *child PIN("r1");

                sprite->yPosition -= 2;
                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                child = (struct PrimarySpriteData *)(slot220 * stride + (u32)base);
                child->yPosition -= 1;
                child = (struct PrimarySpriteData *)(slot219 * stride + (u32)base);
                child->yPosition -= 1;
            }
            {
                struct PrimarySpriteData *base;
                u32 stride;
                register struct PrimarySpriteData *child PIN("r1");

                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                child = (struct PrimarySpriteData *)(slot220 * stride + (u32)base);
                child->yPosition += leftVelocity;
                child = (struct PrimarySpriteData *)(slot219 * stride + (u32)base);
                child->yPosition += rightVelocity;
            }
        }
    }
animate:
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossDamageGraphicsAnimation);
}

void UpdateCractusBossPose109(void)
{
    struct PrimarySpriteData *initial;
    struct PrimarySpriteData *sprite;
    u8 lowSlot220;
    u8 lowSlot219;
    u32 savedSlot220;
    u32 savedSlot219;
    u32 rightVelocity;
    u32 leftVelocity;
    u8 index;

    initial = &gCurrentSprite;
    initial->disableWarioCollisionTimer = 1;
    if ((*((u32 *)&initial->animationTimer) & 0xFFFFFF) == 0x50003 && initial->health != 0) {
        SpriteSpawnAsChild(223, 0, 0, initial->yPosition + 128, initial->xPosition - 68);
    }

    sprite = &gCurrentSprite;
    {
        u32 value;
        u32 counter;
        u8 *counterPtr;

        counterPtr = &sprite->work2;
        value = *counterPtr;
        value++;
        *counterPtr = value;
        value = (u8)value;
        counter = value;
        if (counter == 2)
            VoiceSetPlay(0);
        if ((counter & 7) == 0) {
            u32 paletteValue;
            paletteValue = counter & 8;
            paletteValue = (u8)paletteValue;
            if (paletteValue != 0)
                sprite->palette = 3;
            else
                sprite->palette = 0;
        }
    }

    lowSlot220 = SpriteUtilFindSprite(220, 0);
    savedSlot220 = lowSlot220;
    lowSlot219 = SpriteUtilFindSprite(219, 0);
    savedSlot219 = lowSlot219;

    sprite = &gCurrentSprite;
    {
        u8 *indexPtr;
        u32 tableAddress;
        u32 offset;
        u32 velocity;
        u32 table;

        indexPtr = &sprite->work3;
        index = *indexPtr;
        tableAddress = (u32)sCractusBossPose50LeftYVelocity;
        offset = index << 1;
        tableAddress = offset + tableAddress;
        velocity = *(const u16 *)tableAddress;
        leftVelocity = velocity;
        table = (u32)sCractusBossPose50RightYVelocity;
        offset += table;
        rightVelocity = *(const u16 *)offset;

        if (leftVelocity == 0x7FFF) {
            func_8023BFC(sprite->yPosition + 352, sprite->xPosition);
            if (gSpriteCollisionTileType == 0) {
                struct PrimarySpriteData *base;
                u32 stride;
                register struct PrimarySpriteData *child PIN("r1");

                sprite->yPosition += 2;
                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                child = (struct PrimarySpriteData *)(lowSlot220 * stride + (u32)base);
                child->yPosition += 2;
                child = (struct PrimarySpriteData *)(lowSlot219 * stride + (u32)base);
                child->yPosition += 2;
                goto animate;
            } else {
                struct PrimarySpriteData *base;
                u32 stride;
                struct PrimarySpriteData *left;
                struct PrimarySpriteData *right;
                u32 zero;
                register u8 *work PIN("r0");

                HandleCractusBossDamage();
                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);

                left = (struct PrimarySpriteData *)(lowSlot220 * stride + (u32)base);
                work = &left->work1;
                zero = 0;
                *work = zero;
                right = (struct PrimarySpriteData *)(lowSlot219 * stride + (u32)base);
                work = &right->work1;
                *work = zero;
                left->yPosition = sprite->yPosition + 160;
                right->yPosition = sprite->yPosition + 160;
                return;
            }
        } else {
            *indexPtr = index + 1;
            func_8023BFC(sprite->yPosition + 352, sprite->xPosition);
            if (gSpriteCollisionTileType == 0) {
                struct PrimarySpriteData *base;
                u32 stride;
                register struct PrimarySpriteData *child PIN("r1");

                sprite->yPosition += 2;
                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                child = (struct PrimarySpriteData *)(lowSlot220 * stride + (u32)base);
                child->yPosition += 1;
                child = (struct PrimarySpriteData *)(lowSlot219 * stride + (u32)base);
                child->yPosition += 1;
            }
            {
                struct PrimarySpriteData *base;
                u32 stride;
                register struct PrimarySpriteData *child PIN("r1");

                base = gSpriteData;
                stride = sizeof(struct PrimarySpriteData);
                child = (struct PrimarySpriteData *)(savedSlot220 * stride + (u32)base);
                child->yPosition += leftVelocity;
                child = (struct PrimarySpriteData *)(savedSlot219 * stride + (u32)base);
                child->yPosition += rightVelocity;
            }
        }
    }
animate:
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossDamageGraphicsAnimation);
}

void UpdateCractusBossPose121(void)
{
    struct PrimarySpriteData *sprite;
    u8 *counter;
    u32 value;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossDamageGraphicsAnimation);
    counter = &gInitialHealth;
    value = *counter;
    if (value <= 254) {
        value++;
        *counter = value;
        value = (u8)value;
        if (value == 80) {
            SpawnHighPriorityPrimarySprite(10, sprite->roomSlot, 0, sprite->yPosition + 50, sprite->xPosition - 50);
            SpawnHighPriorityPrimarySprite(10, sprite->roomSlot, 0, sprite->yPosition + 20, sprite->xPosition + 50);
            sprite->status |= 4;
        } else if (value > 239) {
            sprite->status = 0;
        }
    }
}

void UpdateCractusBossPose122(void)
{
    struct PrimarySpriteData *sprite;
    int value;
    int zero;
    u8 *field;

    switch (gCurrentShopItem) {
        case 2: value = 9; break;
        case 3: value = 8; break;
        case 4: value = 7; break;
        case 5: value = 5; break;
        case 6: value = 3; break;
        case 7: value = 4; break;
        case 8: value = 2; break;
        default: value = 10; break;
    }
    sprite = &gCurrentSprite;
    field = &sprite->work2;
    zero = 0;
    *field = value;
    GP_SET(sprite->pOamData, sCractusBossDamagedOam);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->work0 = 0;
    sprite->work1 = 0;
    gBossSequenceState = 32;
    sprite->pose = 123;
    value = sprite->health;
    if (value == 0)
        goto noDamage;
    value--;
    sprite->health = value;
    UpdateBossHealthGauge();
    m4aSongNumStart(SE_DAMAGE);
noDamage:
    VoiceSetPlay(15);
}

void UpdateCractusBossPose123(void)
{
    struct PrimarySpriteData *sprite;
    u8 value;

    UpdateCractusGraphicsAnimation((const u8 *)sCractusBossDamageGraphicsAnimation);
    if (gBossSequenceState != 0) {
        value = --gBossSequenceState;
        if ((value & 7) == 0) {
            if ((value & 8) != 0)
                gCurrentSprite.palette = 3;
            else
                gCurrentSprite.palette = 0;
        }
        return;
    }

    sprite = &gCurrentSprite;
    if (sprite->health == sprite->work2) {
        sprite->pose = 24;
        GP_SET(sprite->pOamData, sCractusBossIntroOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->work0 = 0;
        sprite->work1 = 0;
        UpdateCractusGraphicsAnimation((const u8 *)sCractusBossIntroGraphicsAnimation);
        SpriteUtilStartBossTimer();
        if (sprite->health <= 5) {
            gBgAnimationFrame = 16;
            gBgAnimationTimer = 20;
            gSpriteAiDynamicGraphicsTimer = 16;
        }
    } else {
        gBossSequenceState = 32;
        if (sprite->health != 0) {
            sprite->health--;
            UpdateBossHealthGauge();
            m4aSongNumStart(SE_DAMAGE);
        }
    }
}

void InitCractusStemSegment(void)
{
    struct PrimarySpriteData *sprite;
    volatile u8 *bytePtr;
    volatile u8 *hitboxPtr;
    register u32 status PIN("r0");
    u32 oldStatus;
    register u32 zeroByte PIN("r2");
    u32 zeroHalf;
    register u32 statusBits PIN("r2");
    const struct AnimationFrame *animation;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    statusBits = 0x8408;
    ASM_BARRIER(: "+r"(statusBits));
    status = statusBits;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    status &= 0xFFFB;
    sprite->status = status;

    bytePtr = (volatile u8 *)sprite;
    bytePtr[30] = zeroByte;
    bytePtr += 32;
    status = 40;
    *bytePtr = status;
    bytePtr++;
    *bytePtr = status;
    bytePtr++;
    *bytePtr = status;

    hitboxPtr = (volatile u8 *)sprite + 35;
    oldStatus = 32;
    *hitboxPtr = oldStatus;
    hitboxPtr++;
    *hitboxPtr = oldStatus;
    hitboxPtr++;
    *hitboxPtr = oldStatus;
    hitboxPtr++;
    *hitboxPtr = oldStatus;

    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    status = 16;
    bytePtr = (volatile u8 *)sprite;
    bytePtr[28] = status;

    switch (bytePtr[24]) {
        case 0: animation = sCractusStemSegment0Oam; break;
        case 1: animation = sCractusStemSegment1Oam; break;
        case 2: animation = sCractusStemSegment2Oam; break;
        case 3: animation = sCractusStemSegment3Oam; break;
        default: animation = sCractusStemSegment4Oam; break;
    }
    GP_SET(((struct PrimarySpriteData *)bytePtr)->pOamData, animation);
    {
        u32 roomSlot;

        roomSlot = bytePtr[24];
        if (roomSlot == 0)
            SpriteUtilFindSpriteSlotWork3(216);
        else {
            roomSlot = bytePtr[24];
            roomSlot--;
            ((void (*)(u32))SpriteUtilFindOwnSlot)(roomSlot);(roomSlot); // This weird call is required to have rom match
        }
    }
}

void UpdateCractusStemSegment(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *base;
    u8 *counterStart;
    u8 *counter;
    u32 value;
    s32 roomSlot;

    counterStart = &gInitialHealth;
    value = *counterStart;
    counter = counterStart;
    if (value <= 1)
        goto lowCounter;

    base = &gCurrentSprite;
    roomSlot = base->roomSlot;
    sprite = base;
    switch (roomSlot) {
        case 1:
            if (value > 47) {
                SpawnHighPriorityPrimarySprite(11, 1, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        case 2:
            if (value > 55) {
                SpawnHighPriorityPrimarySprite(11, 2, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        case 3:
            if (value > 63) {
                SpawnHighPriorityPrimarySprite(11, 3, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        case 4:
            if (value > 71) {
                SpawnHighPriorityPrimarySprite(10, 4, 0, sprite->yPosition + 32, sprite->xPosition);
                SpawnHighPriorityPrimarySprite(10, sprite->roomSlot, 0, sprite->yPosition + 64, sprite->xPosition + 100);
                sprite->status = 0;
            }
            break;
        default:
            if (*counter > 55) {
                SpawnHighPriorityPrimarySprite(10, sprite->roomSlot, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
    }
    goto end;

lowCounter:
    UpdateCractusSegmentHorizontalFollow();
    if (gCuckooCondorMoveRight != 0)
        UpdateCractusSegmentVerticalSpacing();
    else
        UpdateCractusSegmentVerticalFollow();
end:
    ;
}

void InitCractusArmSegmentCommon(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    u32 value;
    register u32 temp PIN("r1");
    u32 zeroByte;
    register u32 zeroHalf PIN("r3");
    register u32 statusBits PIN("r2");
    register u8 *fieldR1 PIN("r1");
    u8 *fieldR0;

    sprite = &gCurrentSprite;
    temp = sprite->status;
    statusBits = 0x8408;
    ASM_BARRIER(: "+r"(statusBits));
    value = statusBits;
    ASM_BARRIER(: "+r"(value));
    zeroByte = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    fieldR1 = (u8 *)sprite;
    *(u16 *)fieldR1 = value;
    fieldR1[30] = zeroByte;
    fieldR1 += 32;
    value = 16;
    *fieldR1 = value;
    fieldR1++;
    *fieldR1 = value;
    fieldR1++;
    *(volatile u8 *)fieldR1 = value;
    fieldR0 = (u8 *)sprite + 35;
    temp = 32;
    *fieldR0 = temp;
    fieldR0++;
    *fieldR0 = temp;
    fieldR0++;
    *(volatile u8 *)fieldR0 = temp;
    fieldR1 = (u8 *)sprite + 38;
    value = 48;
    *fieldR1 = value;
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    value = 23;
    fieldR1 = (u8 *)sprite;
    fieldR1[28] = value;
}

void InitCractusArmSegmentA(void)
{
    register struct PrimarySpriteData *sprite PIN("r1");
    const struct AnimationFrame *animation;

    InitCractusArmSegmentCommon();
    switch (gCurrentSprite.roomSlot) {
        case 0:
            sprite = &gCurrentSprite;
            sprite->status |= 0x80;
            GP_SET(sprite->pOamData, sCractusArmSegmentARootOam);
            sprite->work0 = 224;
            goto end;
        case 1:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentA1Oam;
            break;
        case 2:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentA2Oam;
            break;
        case 3:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentA3Oam;
            ASM_BARRIER_VOLATILE(::: "memory");
            break;
        case 4:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentA3Oam;
            break;
        default:
            sprite = &gCurrentSprite;
            animation = sCractusSegmentOam;
            break;
    }
    GP_SET(sprite->pOamData, animation);
end:
    ;
}

void InitCractusArmSegmentB(void)
{
    register struct PrimarySpriteData *sprite PIN("r1");
    const struct AnimationFrame *animation;

    InitCractusArmSegmentCommon();
    switch (gCurrentSprite.roomSlot) {
        case 0:
            sprite = &gCurrentSprite;
            sprite->status |= 0x80;
            GP_SET(sprite->pOamData, sCractusArmSegmentBRootOam);
            sprite->work0 = 224;
            sprite->warioCollision = 13;
            goto end;
        case 1:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentB1Oam;
            break;
        case 2:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentB2Oam;
            break;
        case 3:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentB3Oam;
            ASM_BARRIER_VOLATILE(::: "memory");
            break;
        case 4:
            sprite = &gCurrentSprite;
            animation = sCractusArmSegmentB3Oam;
            break;
        default:
            sprite = &gCurrentSprite;
            animation = sCractusSegmentOam;
            break;
    }
    GP_SET(sprite->pOamData, animation);
end:
    ;
}

void UpdateCractusArmSegmentAPose23(void)
{
    struct PrimarySpriteData *sprite;
    s32 roomSlot;
    u8 *work3;
    u8 *work2;
    u32 value;
    u8 *field;

    sprite = &gCurrentSprite;
    sprite->pose = 24;
    roomSlot = sprite->roomSlot;
    switch (roomSlot) {
        case 0:
            work3 = &sprite->work3;
            value = 40;
            *work3 = value;
            work2 = &sprite->work2;
            *work2 = roomSlot;
            break;
        case 1:
        case 2:
        case 3:
            SpriteUtilFindSpriteWork3(219, 4);
            field = &sprite->work3;
            roomSlot = *field;
            field--;
            *field = roomSlot;
            SpriteUtilFindSpriteWork3(219, 0);
            break;
        case 4:
            SpriteUtilFindSpriteWork3(217, 1);
            break;
    }
}

void UpdateCractusArmSegmentBPose23(void)
{
    struct PrimarySpriteData *sprite;
    s32 roomSlot;
    u8 *work3;
    u8 *work2;
    u32 value;
    u8 *field;

    sprite = &gCurrentSprite;
    sprite->pose = 24;
    roomSlot = sprite->roomSlot;
    switch (roomSlot) {
        case 0:
            work3 = &sprite->work3;
            value = 30;
            *work3 = value;
            work2 = &sprite->work2;
            *work2 = roomSlot;
            break;
        case 1:
        case 2:
        case 3:
            SpriteUtilFindSpriteWork3(220, 4);
            field = &sprite->work3;
            roomSlot = *field;
            field--;
            *field = roomSlot;
            SpriteUtilFindSpriteWork3(220, 0);
            break;
        case 4:
            SpriteUtilFindSpriteWork3(217, 1);
            break;
    }
}

void UpdateCractusArmSegmentPose24(void)
{
    u32 slot;
    register const s16 *entry PIN("r0");
    register const s16 *table PIN("r5");
    u8 *indexPtr;
    u32 index;
    register u32 velocity PIN("r4");
    register u32 base PIN("r0");
    register u32 stride PIN("r1");
    register s32 pose PIN("r2");
    struct PrimarySpriteData *parent;
    struct PrimarySpriteData *sprite;

    slot = SpriteUtilFindSpriteSlot(216);
    sprite = &gCurrentSprite;
    if (sprite->globalID == 219) {
        indexPtr = &sprite->work2;
        index = *indexPtr;
        table = sCractusArmPose24XVelocityA;
        entry = (const s16*)(index << 1);
        entry = (const s16*)((u32)entry + (u32)table);
        velocity = *(const u16*)entry;
        if (*entry == 0x7FFF) {
            velocity = (u16)table[0];
            index = 0;
        }
        *indexPtr = index + 1;
        sprite->xPosition += velocity;
    } else {
        indexPtr = &sprite->work2;
        index = *indexPtr;
        table = sCractusArmPose24XVelocityB;
        entry = (const s16*)(index << 1);
        entry = (const s16*)((u32)entry + (u32)table);
        velocity = *(const u16*)entry;
        if (*entry == 0x7FFF) {
            velocity = (u16)table[0];
            index = 0;
        }
        *indexPtr = index + 1;
        sprite->xPosition += velocity;
    }

    base = (u32)gSpriteData;
    stride = sizeof(struct PrimarySpriteData);
    stride *= slot;
    parent = (struct PrimarySpriteData*)(stride + base);
    pose = parent->pose;
    switch (pose) {
    case 110:
    case 111:
        return;
    case 113:
        gCurrentSprite.yPosition = parent->yPosition + 144;
        return;
    case 15:
        {
            register u32 zero PIN("r1");
            parent = &gCurrentSprite;
            zero = 0;
            parent->pose = pose;
            gCurrentSprite.work2 = zero;
            gCurrentSprite.work1 = zero;
            return;
        }
    case 112:
        UpdateCractusArmAngleTowardWario();
        break;
    }

    {
    register struct PrimarySpriteData *ySprite PIN("r4");
    u32 yVelocity;
    ySprite = &gCurrentSprite;
    indexPtr = &ySprite->work3;
    index = *indexPtr;
    yVelocity = (u16)sCractusBossIntroYVelocity[index];
    if (sCractusBossIntroYVelocity[index] == 0x7FFF) {
        yVelocity = (u16)sCractusBossIntroYVelocity[0];
        index = 0;
    }
    *indexPtr = index + 1;
    ySprite->yPosition += yVelocity;
    }
}


void UpdateCractusArmSegmentPose15(void)
{
    u32 bodySlot;
    s32 threshold;
    u16 spriteY;
    u16 warioY;
    u16 spriteX;
    u16 warioX;
    u32 delta;

    bodySlot = SpriteUtilFindSpriteSlot(216);
    {
        struct PrimarySpriteData *sprites;
        u32 bodyPose;

        sprites = gSpriteData;
        bodyPose = sprites[bodySlot].pose;
        if (bodyPose == 116) {
        struct PrimarySpriteData *sprite;
        register u8 *work0 PIN("r1");

        sprite = &gCurrentSprite;
        (void)*(volatile u8 *)&sprite->globalID;
        work0 = &sprite->work0;
        *work0 = 176;
        } else {
            UpdateCractusArmAngleTowardWario();
        }
    }

    if (gCurrentSprite.globalID == 219) {
        struct PrimarySpriteData *sprites;
        u32 bodyPose;

        sprites = gSpriteData;
        bodyPose = sprites[bodySlot].pose;
        if (bodyPose == 116) {
            u8 *indexPtr;
            u8 index;
            const s16 *table;
            s16 velocity;

            indexPtr = &gCurrentSprite.work2;
            index = *indexPtr;
            table = sCractusArmPose15LeftXVelocity;
            velocity = table[index];
            if (velocity == 0x7FFF) {
                velocity = table[0];
                index = 0;
            }
            *indexPtr = index + 1;
            gCurrentSprite.xPosition += velocity;
            threshold = 255;
        } else {
            u8 *indexPtr;
            u8 index;
            const s16 *table;
            s16 velocity;

            indexPtr = &gCurrentSprite.work2;
            index = *indexPtr;
            table = sCractusArmPose15LeftYVelocity;
            velocity = table[index];
            if (velocity == 0x7FFF) {
                velocity = table[0];
                index = 0;
            }
            *indexPtr = index + 1;
            gCurrentSprite.xPosition += velocity;
            threshold = 120;
        }
    } else {
        struct PrimarySpriteData *sprites;
        u32 bodyPose;

        sprites = gSpriteData;
        bodyPose = sprites[bodySlot].pose;
        if (bodyPose == 116) {
            u8 *indexPtr;
            u8 index;
            const s16 *table;
            s16 velocity;

            indexPtr = &gCurrentSprite.work2;
            index = *indexPtr;
            table = sCractusArmPose15RightXVelocity;
            velocity = table[index];
            if (velocity == 0x7FFF) {
                velocity = table[0];
                index = 0;
            }
            *indexPtr = index + 1;
            gCurrentSprite.xPosition += velocity;
            threshold = 255;
        } else {
            u8 *indexPtr;
            u8 index;
            const s16 *table;
            s16 velocity;

            indexPtr = &gCurrentSprite.work2;
            index = *indexPtr;
            table = sCractusArmPose15RightYVelocity;
            velocity = table[index];
            if (velocity == 0x7FFF) {
                velocity = table[0];
                index = 0;
            }
            *indexPtr = index + 1;
            gCurrentSprite.xPosition += velocity;
            threshold = 52;
        }
    }

    if (gCuckooCondorHasCapturedWario == 0)
        return;

    gCurrentSprite.work1++;
    if ((u8)(gWarioData.pose - 43) <= 2 || gCurrentSprite.health == 1)
        gCurrentSprite.work1++;
    if (gCurrentSprite.work1 < threshold)
        return;

    spriteY = gCurrentSprite.yPosition + 256;
    warioY = gWarioData.yPosition + 256;
    spriteX = gCurrentSprite.xPosition + 256;
    warioX = gWarioData.xPosition + 256;
    if (spriteX < warioX) {
        gCurrentSprite.work1--;
        return;
    }

    delta = (u16)(spriteX - warioX) >> 4;
    if (delta > gSpriteAiDynamicGraphicsTimer)
        gCurrentSprite.work2 = gSpriteAiDynamicGraphicsTimer;
    else
        gCurrentSprite.work2 = delta;

    if (spriteY > warioY) {
        register u32 shifted;
        u32 yDelta;

        shifted = (u16)(spriteY - warioY);
        shifted <<= 16;
        yDelta = shifted >> 20;
        {
            register u32 timerValue PIN("r0");
            timerValue = gSpriteAiDynamicGraphicsTimer;
            if (yDelta > timerValue)
                gCurrentSprite.work3 = timerValue;
            else {
                gCurrentSprite.work3 = yDelta;
                ASM_BARRIER(: : "r"(timerValue));
            }
        }
        gCurrentSprite.work1 = gBgAnimationTimer + gBgAnimationFrame;
        gCurrentSprite.pose = 110;
        m4aSongNumStart(SE_CRACTUS_ARM_AIM);
    } else {
        register u32 shifted;
        u32 yDelta;

        shifted = (u16)(warioY - spriteY);
        shifted <<= 16;
        yDelta = shifted >> 20;
        {
            register u32 timerValue PIN("r0");
            timerValue = gSpriteAiDynamicGraphicsTimer;
            if (yDelta > timerValue)
                gCurrentSprite.work3 = timerValue;
            else {
                gCurrentSprite.work3 = yDelta;
                ASM_BARRIER(: : "r"(timerValue));
            }
        }
        gCurrentSprite.work1 = gBgAnimationTimer + gBgAnimationFrame;
        gCurrentSprite.pose = 112;
        m4aSongNumStart(SE_CRACTUS_ARM_AIM);
    }
}

void UpdateCractusArmSegmentPose110(void)
{
    u8 *sprite;
    u8 *timer;
    register u32 value PIN("r0");
    register u32 timerValue PIN("r2");
    u8 *global;
    s32 threshold;
    u8 *divisorPtr;
    s32 divisor;
    s32 quotient;
    u8 *field;
    u32 component;
    u32 position;

    sprite = (u8 *)&gCurrentSprite;
    timer = sprite + 40;
    value = *timer;
    timerValue = value - 1;
    *timer = timerValue;
    value = timerValue << 24;
    if (value == 0)
        goto finished;
    global = &gBgAnimationTimer;
    value >>= 24;
    threshold = *global;
    if (value < threshold)
        goto moving;
    if (value == threshold)
        m4aSongNumStart(SE_CRACTUS_ARM_SWING);
    sprite += 39;
    divisorPtr = &gBgAnimationFrame;
    divisor = *divisorPtr;
    quotient = 256 / divisor;
    threshold = *sprite;
    threshold -= quotient;
    *sprite = threshold;
    goto end;
moving:
    sprite[30] = 13;
    field = sprite + 42;
    component = *field;
    position = *(u16 *)(sprite + 8);
    position -= component;
    *(u16 *)(sprite + 8) = position;
    field = sprite + 41;
    component = *field;
    position = *(u16 *)(sprite + 10);
    position -= component;
    *(u16 *)(sprite + 10) = position;
    goto update;
finished:
    value = gBgAnimationTimer;
    *timer = value;
    sprite[28] = 111;
update:
    UpdateCractusArmAngleTowardWario();
end:
    ;
}

void UpdateCractusArmSegmentPose111(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;
    u32 slot;
    struct PrimarySpriteData *sprites;
    u32 stride;
    register struct PrimarySpriteData *target PIN("r1");
    u32 zero;
    struct PrimarySpriteData *current;
    u32 pose;

    sprite = &gCurrentSprite;
    timer = &sprite->work1;
    value = *timer;
    value--;
    *timer = value;
    value <<= 24;
    if (value != 0) {
        timer += 2;
        value = sprite->yPosition;
        slot = *timer;
        value += slot;
        sprite->yPosition = value;
        timer = &sprite->work2;
        value = sprite->xPosition;
        slot = *timer;
        value += slot;
        sprite->xPosition = value;
        goto update;
    }

    if (sprite->globalID == 219) {
        value = SpriteUtilFindSprite(219, 4);
        slot = (u8)value;
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        slot *= stride;
        target = (struct PrimarySpriteData *)(slot + (u32)sprites);
        value = target->yPosition + 36;
        zero = 0;
        sprite->yPosition = value;
        value = target->xPosition - 96;
        sprite->xPosition = value;
        sprite->warioCollision = zero;
    } else {
        value = SpriteUtilFindSprite(220, 4);
        slot = (u8)value;
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        slot *= stride;
        target = (struct PrimarySpriteData *)(slot + (u32)sprites);
        value = target->yPosition + 48;
        sprite->yPosition = value;
        value = target->xPosition - 64;
        sprite->xPosition = value;
    }
    current = &gCurrentSprite;
    zero = 0;
    pose = 15;
    current->pose = pose;
    timer = &current->work2;
    *timer = zero;
    current = (struct PrimarySpriteData *)((u8 *)current + 40);
    *(u8 *)current = zero;
update:
    UpdateCractusArmAngleTowardWario();
}

void UpdateCractusArmSegmentPose112(void)
{
    u8 *sprite;
    u8 *timer;
    register u32 value PIN("r0");
    register u32 timerValue PIN("r2");
    u8 *global;
    s32 threshold;
    u8 *divisorPtr;
    s32 divisor;
    s32 quotient;
    u8 *field;
    u8 *fieldR1;
    u32 component;
    u32 position;

    sprite = (u8 *)&gCurrentSprite;
    timer = sprite + 40;
    value = *timer;
    timerValue = value - 1;
    *timer = timerValue;
    value = timerValue << 24;
    if (value == 0)
        goto finished;
    global = &gBgAnimationTimer;
    value >>= 24;
    threshold = *global;
    if (value < threshold)
        goto moving;
    if (value == threshold)
        m4aSongNumStart(SE_CRACTUS_ARM_SWING);
    sprite += 39;
    divisorPtr = &gBgAnimationFrame;
    divisor = *divisorPtr;
    quotient = 256 / divisor;
    threshold = *sprite;
    threshold -= quotient;
    *sprite = threshold;
    goto end;
moving:
    sprite[30] = 13;
    fieldR1 = sprite + 42;
    position = *(u16 *)(sprite + 8);
    component = *fieldR1;
    position += component;
    *(u16 *)(sprite + 8) = position;
    field = sprite + 41;
    component = *field;
    position = *(u16 *)(sprite + 10);
    position -= component;
    *(u16 *)(sprite + 10) = position;
    goto update;
finished:
    value = gBgAnimationTimer;
    *timer = value;
    sprite[28] = 113;
update:
    UpdateCractusArmAngleTowardWario();
end:
    ;
}

void UpdateCractusArmSegmentPose113(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    register u32 value PIN("r0");
    u32 slot;
    struct PrimarySpriteData *sprites;
    u32 stride;
    register struct PrimarySpriteData *target PIN("r1");
    u32 zero;
    struct PrimarySpriteData *current;
    u32 pose;

    sprite = &gCurrentSprite;
    timer = &sprite->work1;
    value = *timer;
    value--;
    *timer = value;
    value <<= 24;
    if (value != 0) {
        u8 *field;
        u32 position;
        field = (u8 *)sprite;
        field += 42;
        value = *field;
        position = sprite->yPosition;
        position -= value;
        sprite->yPosition = position;
        timer = &sprite->work2;
        value = sprite->xPosition;
        slot = *timer;
        value += slot;
        sprite->xPosition = value;
        goto update;
    }

    if (sprite->globalID == 219) {
        value = SpriteUtilFindSprite(219, 4);
        slot = (u8)value;
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        slot *= stride;
        target = (struct PrimarySpriteData *)(slot + (u32)sprites);
        value = target->yPosition + 52;
        zero = 0;
        sprite->yPosition = value;
        value = target->xPosition - 96;
        sprite->xPosition = value;
        sprite->warioCollision = zero;
    } else {
        value = SpriteUtilFindSprite(220, 4);
        slot = (u8)value;
        sprites = gSpriteData;
        stride = sizeof(struct PrimarySpriteData);
        slot *= stride;
        target = (struct PrimarySpriteData *)(slot + (u32)sprites);
        value = target->yPosition + 48;
        sprite->yPosition = value;
        value = target->xPosition - 64;
        sprite->xPosition = value;
    }
    current = &gCurrentSprite;
    zero = 0;
    pose = 15;
    current->pose = pose;
    timer = &current->work2;
    *timer = zero;
    current = (struct PrimarySpriteData *)((u8 *)current + 40);
    *(u8 *)current = zero;
update:
    UpdateCractusArmAngleTowardWario();
}

void InitCractusStemTip(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    u32 value;
    register u32 temp PIN("r1");
    u32 zeroByte;
    register u32 zeroHalf PIN("r3");
    register u32 statusBits PIN("r2");
    register u8 *fieldR1 PIN("r1");
    u8 *fieldR0;

    sprite = &gCurrentSprite;
    temp = sprite->status;
    statusBits = 0x8408;
    ASM_BARRIER(: "+r"(statusBits));
    value = statusBits;
    ASM_BARRIER(: "+r"(value));
    zeroByte = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    fieldR1 = (u8 *)sprite;
    *(u16 *)fieldR1 = value;
    fieldR1[30] = zeroByte;
    fieldR1 += 32;
    value = 8;
    *fieldR1 = value;
    fieldR1++;
    *fieldR1 = value;
    fieldR1++;
    *(volatile u8 *)fieldR1 = value;
    fieldR0 = (u8 *)sprite + 35;
    temp = 32;
    *fieldR0 = temp;
    fieldR0++;
    *fieldR0 = temp;
    fieldR0++;
    *fieldR0 = temp;
    fieldR0++;
    *(volatile u8 *)fieldR0 = temp;
    value = (u32)sBossTreasureOam_83DF9E8;
    fieldR1 = (u8 *)sprite;
    *(u32 *)(fieldR1 + 4) = value;
    fieldR1[22] = zeroByte;
    ((struct PrimarySpriteData *)fieldR1)->animationTimer = zeroHalf;
    value = 16;
    fieldR1[28] = value;
    SpriteUtilFindSpriteWork3(217, 4);
}

void UpdateCractusStemTip(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    register u32 slot PIN("r0");
    u32 size;
    u8 *workIp;
    u8 *workR4;

    if (gInitialHealth > 89) {
        gCurrentSprite.pose = 0;
        gCurrentSprite.globalID = 207;
    } else {
        sprite = &gCurrentSprite;
        sprites = gSpriteData;
        workIp = &sprite->work3;
        slot = *workIp;
        size = sizeof(struct PrimarySpriteData);
        slot *= size;
        sprite->yPosition = ((struct PrimarySpriteData *)(slot + (u32)sprites))->yPosition - 32;
        workR4 = workIp;
        slot = *workR4;
        slot *= size;
        sprite->xPosition = ((struct PrimarySpriteData *)(slot + (u32)sprites))->xPosition;
    }
}

void InitCractusDefeatPiece(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;
    register u32 temp PIN("r1");
    register u32 zeroByte PIN("r2");
    u32 zeroHalf;
    register u32 statusBits PIN("r2");
    u8 *fieldR1;
    register u8 *fieldR0 PIN("r0");

    sprite = &gCurrentSprite;
    zeroHalf = 0;
    value = 1;
    fieldR1 = (u8 *)sprite;
    fieldR1[26] = value;
    temp = *(u16 *)fieldR1;
    statusBits = 0x400;
    ASM_BARRIER(: "+r"(statusBits));
    value = statusBits;
    zeroByte = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    fieldR1 = (u8 *)sprite;
    *(u16 *)fieldR1 = value;
    fieldR1[30] = zeroByte;
    fieldR1 += 32;
    value = 16;
    *fieldR1 = value;
    fieldR1++;
    *fieldR1 = value;
    fieldR1++;
    *fieldR1 = value;
    fieldR0 = (u8 *)sprite + 35;
    temp = 32;
    *fieldR0 = temp;
    fieldR0++;
    *fieldR0 = temp;
    fieldR0++;
    *fieldR0 = temp;
    fieldR0++;
    *(volatile u8 *)fieldR0 = temp;
    value = (u32)sCractusDefeatPieceOam;
    fieldR1 = (u8 *)sprite;
    *(u32 *)(fieldR1 + 4) = value;
    fieldR1[22] = zeroByte;
    ((struct PrimarySpriteData *)fieldR1)->animationTimer = zeroHalf;
    value = 24;
    fieldR1[28] = value;
    value = ((struct PrimarySpriteData *)fieldR1)->xPosition;
    value += 128;
    ((struct PrimarySpriteData *)fieldR1)->xPosition = value;
    fieldR0 = (u8 *)sprite + 42;
    *fieldR0 = zeroByte;
    fieldR0 -= 3;
    *fieldR0 = zeroByte;
}

void UpdateCractusDefeatPiecePose24(void)
{
    u32 value;
    register struct PrimarySpriteData *sprite PIN("r1");

    value = gWarioData.reaction;
    if (value != 0) {
        sprite = &gCurrentSprite;
        value = 16;
        sprite->pose = value;
    }
}

void UpdateCractusDefeatPiecePose16(void)
{
    struct PrimarySpriteData *sprite;
    const s16 *table;
    u8 *timer;
    u8 *indexPtr;
    u32 mask;
    register u32 timerValue PIN("r1");
    u32 index;
    u32 velocity;
    s32 signedVelocity;
    s32 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    timerValue = *timer;
    mask = 63;
    mask &= timerValue;
    if (mask == 0)
        m4aSongNumStart(SE_CRACTUS_DEFEAT_PIECE_MOVE);

    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sCractusDefeatPieceYVelocity;
    mask = index << 1;
    mask += (u32)table;
    velocity = *(const u16 *)mask;
    zero = 0;
    signedVelocity = *(const s16 *)((const u8 *)mask + zero);
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)table;
        index = 0;
    }
    {
        u32 nextIndex;
        u8 *destination;
        nextIndex = index + 1;
        destination = indexPtr;
        *destination = nextIndex;
    }
    sprite->yPosition += velocity;

    mask = *timer + 1;
    *timer = mask;
    mask = (u8)mask;
    if (mask != 200) {
        sprite->xPosition -= 2;
    } else {
        u32 zeroTimer;
        sprite->status ^= SPRITE_STATUS_FACING_RIGHT;
        sprite->pose = 18;
        zeroTimer = 0;
        *timer = zeroTimer;
    }
}

void UpdateCractusDefeatPiecePose18(void)
{
    struct PrimarySpriteData *sprite;
    const s16 *table;
    u8 *timer;
    u8 *indexPtr;
    u32 mask;
    register u32 timerValue PIN("r1");
    u32 index;
    u32 velocity;
    s32 signedVelocity;
    s32 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    timerValue = *timer;
    mask = 63;
    mask &= timerValue;
    if (mask == 0)
        m4aSongNumStart(SE_CRACTUS_DEFEAT_PIECE_MOVE);

    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sCractusDefeatPieceYVelocity;
    mask = index << 1;
    mask += (u32)table;
    velocity = *(const u16 *)mask;
    zero = 0;
    signedVelocity = *(const s16 *)((const u8 *)mask + zero);
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)table;
        index = 0;
    }
    {
        u32 nextIndex;
        u8 *destination;
        nextIndex = index + 1;
        destination = indexPtr;
        *destination = nextIndex;
    }
    sprite->yPosition += velocity;

    mask = *timer + 1;
    *timer = mask;
    mask = (u8)mask;
    if (mask != 200) {
        sprite->xPosition += 2;
    } else {
        u32 zeroTimer;
        sprite->status ^= SPRITE_STATUS_FACING_RIGHT;
        sprite->pose = 24;
        zeroTimer = 0;
        *timer = zeroTimer;
    }
}

void InitCractusDefeatAttachment(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    u32 value;
    register u32 temp PIN("r1");
    register u32 collision PIN("r2");
    register u32 zeroByte PIN("r3");
    register u32 zeroHalf PIN("r4");

    sprite = &gCurrentSprite;
    zeroHalf = 0;
    value = 1;
    temp = (u32)sprite;
    ((struct PrimarySpriteData *)temp)->drawPriority = value;
    temp = *(u16 *)temp;
    collision = 0x400;
    ASM_BARRIER(: "+r"(collision));
    value = collision;
    ASM_BARRIER(: "+r"(value));
    zeroByte = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)sprite;
    *(u16 *)temp = value;
    value = 61;
    *(u8 *)(temp + 30) = value;
    value = (u32)sprite;
    value += 32;
    collision = 16;
    *(u8 *)value = collision;
    value++;
    *(u8 *)value = collision;
    value++;
    *(u8 *)value = collision;
    value++;
    temp = 32;
    *(u8 *)value = temp;
    value++;
    *(volatile u8 *)value = temp;
    temp = (u32)sprite;
    temp += 37;
    value = 24;
    *(u8 *)temp = value;
    temp++;
    value = 20;
    *(volatile u8 *)temp = value;
    value = (u32)sCractusSegmentOam;
    temp = (u32)sprite;
    *(u32 *)(temp + 4) = value;
    ((struct PrimarySpriteData *)temp)->currentAnimationFrame = zeroByte;
    ((struct PrimarySpriteData *)temp)->animationTimer = zeroHalf;
    ((struct PrimarySpriteData *)temp)->pose = collision;
    SpriteUtilFindSpriteSlotWork3(221);
}

void UpdateCractusDefeatAttachment(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    register struct PrimarySpriteData *parent PIN("r4");
    u32 value;
    u32 slot;
    u32 size;

    sprite = &gCurrentSprite;
    sprite->status ^= 4;
    sprites = gSpriteData;
    slot = sprite->work3;
    size = sizeof(struct PrimarySpriteData);
    size *= slot;
    parent = (struct PrimarySpriteData *)(size + (u32)sprites);
    if (parent->status & SPRITE_STATUS_FACING_RIGHT)
        sprite->xPosition = parent->xPosition - 32;
    else
        sprite->xPosition = parent->xPosition + 32;
    slot = sprite->work3;
    size = sizeof(struct PrimarySpriteData);
    size *= slot;
    sprite->yPosition = ((struct PrimarySpriteData *)(size + (u32)sprites))->yPosition;
}

void InitCractusProjectile(void)
{
    struct PrimarySpriteData *sprite;
    register u32 value PIN("r0");
    u32 temp;
    register u32 extent PIN("r2");
    register u32 zeroByte PIN("r3");
    u32 zeroHalf;

    sprite = &gCurrentSprite;
    temp = sprite->status;
    extent = 0x400;
    ASM_BARRIER(: "+r"(extent));
    value = extent;
    ASM_BARRIER(: "+r"(value));
    zeroByte = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    sprite->status = value;
    sprite->warioCollision = 16;
    sprite->drawDistanceDown = 16;
    sprite->drawDistanceUp = 16;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 8;
    sprite->hitboxExtentDown = 8;
    sprite->hitboxExtentLeft = 8;
    sprite->hitboxExtentRight = 8;
    GP_SET(sprite->pOamData, sCractusProjectileFallingOam);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->work0 = 7;
    sprite->work3 = zeroByte;
    sprite->pose = 16;
    sprite->drawPriority = 1;
    func_8023BFC(sprite->yPosition, sprite->xPosition);
    if (gSpriteCollisionTileType != 0)
        sprite->status = zeroHalf;
    else
        m4aSongNumStart(SE_FALLING);
}

void UpdateCractusProjectilePose16(void)
{
    struct PrimarySpriteData *base;
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *current PIN("r4");
    u8 *timer;
    u8 *indexPtr;
    register const s16 *table PIN("r6");
    register u32 value PIN("r0");
    u32 timerValue;
    u32 index;
    u32 velocity;
    s32 signedVelocity;
    s32 zero;

    base = &gCurrentSprite;
    timer = &base->work0;
    timerValue = *timer;
    sprite = base;
    if (timerValue != 0) {
        value = timerValue - 1;
        *timer = value;
        timerValue = (u8)value;
        if (timerValue == 0) {
            value = (u32)sCractusProjectileActiveOam;
            GP_SET(sprite->pOamData, (const struct AnimationFrame *)value);
            sprite->currentAnimationFrame = timerValue;
            sprite->animationTimer = timerValue;
        }
    }

    current = sprite;
    value = func_8023A60(current->yPosition, current->xPosition);
    velocity = value;
    value = gSpriteCollisionResult;
    if (value != 0) {
        register u32 zeroByte PIN("r1");
        u32 zeroHalf;

        zeroByte = 0;
        zeroHalf = 0;
        current->yPosition = velocity;
        value = (u32)sCractusProjectileLandedOam;
        GP_SET(current->pOamData, (const struct AnimationFrame *)value);
        current->currentAnimationFrame = zeroByte;
        current->animationTimer = zeroHalf;
        {
            u8 *work;
            work = &current->work0;
            value = 24;
            *work = value;
        }
        value = 50;
        current->pose = value;
        value = 17;
        current->warioCollision = value;
        value = 2;
        current->drawPriority = value;
        return;
    }

    value = 42;
    ASM_BARRIER(: "+r"(value));
    value += (u32)current;
    indexPtr = (u8 *)value;
    index = *(u8 *)value;
    table = sCractusProjectileYVelocity;
    value = index << 1;
    value += (u32)table;
    velocity = *(const u16 *)value;
    zero = 0;
    signedVelocity = *(const s16 *)((const u8 *)value + zero);
    ASM_BARRIER(: : "r"(sprite));
    if (signedVelocity == 0x7FFF) {
        u32 offset;
        register u32 yPosition PIN("r0");

        offset = index - 1;
        offset <<= 1;
        offset += (u32)table;
        yPosition = current->yPosition;
        offset = *(const u16 *)offset;
        yPosition += offset;
        current->yPosition = yPosition;
    } else {
        u32 nextIndex;
        u8 *destination;

        nextIndex = index + 1;
        destination = indexPtr;
        *destination = nextIndex;
        value = sprite->yPosition;
        value += velocity;
        sprite->yPosition = value;
    }
}

void InitCractusProjectileDespawn(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;
    u8 *timer;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sCractusProjectileDespawnOam);
    value = 0;
    sprite->currentAnimationFrame = value;
    sprite->animationTimer = value;
    timer = &sprite->work0;
    value = 12;
    *timer = value;
    sprite->pose = 50;
    sprite->drawPriority = 1;
}

void UpdateCractusProjectileDespawn(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u32 value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    value = (u8)value;
    if (value == 0)
        sprite->status = value;
}

void SpriteCractusDefeatPiece(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
    case 0:
        InitCractusDefeatPiece();
        break;
    case 24:
        UpdateCractusDefeatPiecePose24();
        break;
    case 16:
        UpdateCractusDefeatPiecePose16();
        break;
    case 18:
        UpdateCractusDefeatPiecePose18();
        break;
    }
}

void SpriteCractusDefeatAttachment(void)
{
    if (gCurrentSprite.pose == 0)
        InitCractusDefeatAttachment();
    UpdateCractusDefeatAttachment();
}

void SpriteCractusSpawner(void)
{
    switch (gCurrentSprite.pose) {
    case 0:
        InitCractusSpawner();
        break;
    case 1:
        CractusSpawnerWaitForBattle();
        break;
    case 2:
        CractusSpawnerStartBattle();
        break;
    case 31:
    case 33:
        InitCractusSpawnerDefeat();
    case 32:
        UpdateCractusSpawnerDefeat();
        break;
    }
    func_8026838();
}

void SpriteCractusBoss(void)
{
    u8 *globalBase;
    u32 timerValue;
    u8 *globalTimer;
    register struct PrimarySpriteData *sprite PIN("r2");

    globalBase = (u8*)&gCuckooCondorPendulumLength;
    timerValue = *globalBase;
    globalTimer = globalBase;
    sprite = &gCurrentSprite;
    if (timerValue != 0) {
        if (timerValue == 60 || timerValue == 1) {
            u8 *base;
            u8 *work;
            register u32 zero PIN("r2");

            base = (u8 *)&gCurrentSprite;
            work = base + 39;
            zero = 0;
            *work = zero;
            work++;
            *work = zero;
            sprite = (struct PrimarySpriteData *)base;
        }
        {
            u32 value;
            value = *globalTimer;
            value--;
            *globalTimer = value;
        }
    }

    switch (sprite->pose) {
    case 0:
        InitCractusBoss();
        break;
    case 1:
        UpdateCractusBossPose1();
        break;
    case 24:
        UpdateCractusBossPose24();
        break;
    case 110:
        InitCractusBossPose111();
    case 111:
        UpdateCractusBossPose111();
        break;
    case 113:
        UpdateCractusBossPose113();
        break;
    case 112:
        UpdateCractusBossPose112();
        break;
    case 15:
        UpdateCractusBossPose15();
        break;
    case 16:
        UpdateCractusBossPose16();
        break;
    case 18:
        UpdateCractusBossPose18();
        break;
    case 17:
        UpdateCractusBossPose17();
        break;
    case 19:
        UpdateCractusBossPose19();
        break;
    case 20:
        UpdateCractusBossPose20();
        break;
    case 21:
        UpdateCractusBossPose21();
        break;
    case 22:
        UpdateCractusBossPose22();
        break;
    case 49:
        UpdateCractusBossPose49();
        break;
    case 50:
        UpdateCractusBossPose50();
        break;
    case 109:
        UpdateCractusBossPose109();
        break;
    case 114:
        UpdateCractusBossPose114();
        break;
    case 116:
        UpdateCractusBossPose116();
        break;
    case 121:
        UpdateCractusBossPose121();
        break;
    case 122:
        UpdateCractusBossPose122();
    case 123:
        UpdateCractusBossPose123();
        break;
    }
}

void SpriteCractusStemSegment(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0)
        InitCractusStemSegment();
    else
        UpdateCractusStemSegment();
}

void SpriteCractusStemTip(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0)
        InitCractusStemTip();
    else
        UpdateCractusStemTip();
}

void UpdateCractusArmSegmentPosition(void)
{
    u8 *counterBase;
    u8 *counterPtr;
    u32 counter;

    counterBase = &gInitialHealth;
    counter = *counterBase;
    counterPtr = counterBase;
    if (counter > 1) {
        u8 *spriteBase;
        struct PrimarySpriteData *sprite;
        s32 roomSlot;

        spriteBase = (u8 *)&gCurrentSprite;
        roomSlot = spriteBase[24];
        sprite = (struct PrimarySpriteData *)spriteBase;
        switch (roomSlot) {
        case 1:
            if (counter > 15) {
                SpawnHighPriorityPrimarySprite(11, 1, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        case 2:
            if (counter > 23) {
                SpawnHighPriorityPrimarySprite(11, 2, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        case 3:
            if (counter > 31) {
                SpawnHighPriorityPrimarySprite(11, 3, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        case 4:
            if (counter > 39) {
                SpawnHighPriorityPrimarySprite(10, 4, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        default:
            if (*counterPtr > 7) {
                SpawnHighPriorityPrimarySprite(10, sprite->roomSlot, 0, sprite->yPosition, sprite->xPosition);
                sprite->status = 0;
            }
            break;
        }
    } else {
        struct PrimarySpriteData *sprite;
        s32 roomSlot;

        sprite = &gCurrentSprite;
        roomSlot = sprite->roomSlot;
        switch (roomSlot) {
        case 1:
            PositionCractusSegmentAtFiveEighths();
            break;
        case 2:
            PositionCractusSegmentAtThreeEighths();
            break;
        case 3:
            PositionCractusSegmentAtOneEighth();
            break;
        case 4:
            {
                struct PrimarySpriteData *sprites;
                u8 *slotPtr;
                u32 slot;
                u32 size;
                struct PrimarySpriteData *target;

                sprites = gSpriteData;
                slotPtr = &sprite->work3;
                slot = *slotPtr;
                size = sizeof(struct PrimarySpriteData);
                slot *= size;
                target = (struct PrimarySpriteData *)(slot + (u32)sprites);
                sprite->yPosition = target->yPosition;
                slot = *slotPtr;
                slot *= size;
                target = (struct PrimarySpriteData *)(slot + (u32)sprites);
                sprite->xPosition = target->xPosition + 32;
            }
            break;
        }
    }
}

void UpdateCractusArmSegmentPose24(void);

void SpriteCractusArmSegment(void)
{
    struct PrimarySpriteData *current;
    s32 pose;

    pose = gInitialHealth;
    if (pose != 0) {
        current = &gCurrentSprite;
        pose = 1;
        current->disableWarioCollisionTimer = pose;
    }

    current = &gCurrentSprite;
    pose = current->pose;
    switch (pose) {
    case 0:
        if (current->globalID == 219)
            InitCractusArmSegmentA();
        else
            InitCractusArmSegmentB();
        break;
    case 23:
        if (current->globalID == 219)
            UpdateCractusArmSegmentAPose23();
        else
            UpdateCractusArmSegmentBPose23();
        UpdateCractusArmSegmentPosition();
        break;
    case 24:
        if (current->roomSlot == 0)
            UpdateCractusArmSegmentPose24();
        UpdateCractusArmSegmentPosition();
        break;
    case 15:
        if (current->roomSlot == 0)
            UpdateCractusArmSegmentPose15();
        UpdateCractusArmSegmentPosition();
        break;
    case 110:
        if (current->roomSlot == 0)
            UpdateCractusArmSegmentPose110();
        UpdateCractusArmSegmentPosition();
        break;
    case 111:
        if (current->roomSlot == 0)
            UpdateCractusArmSegmentPose111();
        UpdateCractusArmSegmentPosition();
        break;
    case 112:
        if (current->roomSlot == 0)
            UpdateCractusArmSegmentPose112();
        UpdateCractusArmSegmentPosition();
        break;
    case 113:
        if (current->roomSlot == 0)
            UpdateCractusArmSegmentPose113();
        UpdateCractusArmSegmentPosition();
        break;
    }

    {
        register struct PrimarySpriteData *sprite;
        const s16 *table;
        u8 *anglePtr;
        register s32 trig PIN("r4");
        s32 scale;
        u32 offset;
        s32 zero;
        u8 *angle;
        s32 inverse;

        sprite = &gCurrentSprite;
        if (sprite->roomSlot == 0) {
            table = sSinCosTable;
            anglePtr = &sprite->work0;

            offset = *anglePtr;
            offset += 64;
            offset <<= 1;
            offset += (u32)table;
            zero = 0;
            trig = *(s16 *)(offset + zero);
            scale = 128;
            scale <<= 1;
            inverse = FixedInverse(scale);
            sprite->affinePA = FixedMul(trig, inverse);

            angle = anglePtr;
            offset = *angle;
            offset <<= 1;
            offset += (u32)table;
            zero = 0;
            trig = *(s16 *)(offset + zero);
            inverse = FixedInverse(scale);
            sprite->affinePB = FixedMul(trig, inverse);

            angle = anglePtr;
            offset = *angle;
            offset <<= 1;
            offset += (u32)table;
            zero = 0;
            trig = *(s16 *)(offset + zero);
            trig = -trig;
            inverse = FixedInverse(scale);
            sprite->affinePC = FixedMul(trig, inverse);

            angle = anglePtr;
            offset = *angle;
            offset += 64;
            offset <<= 1;
            offset += (u32)table;
            zero = 0;
            trig = *(s16 *)(offset + zero);
            inverse = FixedInverse(scale);
            sprite->affinePD = FixedMul(trig, inverse);
        } else {
            offset = 1;
            sprite->disableWarioCollisionTimer = offset;
        }
    }
}

void SpriteCractusProjectile(void)
{
    struct PrimarySpriteData *sprite;
    u32 value;

    value = gWarioData.reaction;
    sprite = &gCurrentSprite;
    if (value == 5)
        sprite->disableWarioCollisionTimer = 1;

    switch (sprite->pose) {
    case 0:
        InitCractusProjectile();
        break;
    case 16:
        UpdateCractusProjectilePose16();
        break;
    case 50:
        UpdateCractusProjectileDespawn();
        break;
    default:
        InitCractusProjectileDespawn();
        break;
    }
}

void SpriteCractusImpactEffect(void)
{
    register u8 *sprite PIN("r12");
    u32 value;
    u32 temp;
    register u32 extent PIN("r2");
    register u32 zero PIN("r3");
    u32 pose;
    register u8 *timer PIN("r1");

    sprite = (u8 *)&gCurrentSprite;
    value = 1;
    temp = (u32)sprite;
    ((struct PrimarySpriteData *)temp)->disableWarioCollisionTimer = value;
    pose = ((struct PrimarySpriteData *)temp)->pose;
    if (pose == 0) {
        temp = *(u16 *)temp;
        extent = 128;
        extent <<= 3;
        ASM_BARRIER(: "+r"(extent));
        value = extent;
        ASM_BARRIER(: "+r"(value));
        zero = 0;
        value |= temp;
        temp = 0xFFFB;
        value &= temp;
        temp = (u32)sprite;
        *(u16 *)temp = value;
        *(u8 *)(temp + 30) = zero;

        value = (u32)sprite + 32;
        temp = 32;
        *(u8 *)value = temp;
        value++;
        extent = 16;
        *(u8 *)value = extent;
        value++;
        *(u8 *)value = temp;
        value++;
        temp = 4;
        *(u8 *)value = temp;
        value++;
        *(u8 *)value = temp;
        value++;
        *(u8 *)value = temp;
        value++;
        *(volatile u8 *)value = temp;

        value = (u32)sCractusImpactEffectOam;
        temp = (u32)sprite;
        *(u32 *)(temp + 4) = value;
        ((struct PrimarySpriteData *)temp)->currentAnimationFrame = zero;
        ((struct PrimarySpriteData *)temp)->animationTimer = pose;
        temp += 39;
        value = 56;
        *(u8 *)temp = value;
        value = (u32)sprite;
        ((struct PrimarySpriteData *)value)->pose = extent;
    }

    timer = sprite + 39;
    value = *timer - 1;
    *timer = value;
    value = (u8)value;
    if (value == 0) {
        temp = (u32)sprite;
        *(u16 *)temp = value;
    }
}

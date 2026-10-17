#include "sprite_ai/catbat.h"
#include "color_effects.h"
#include "sprite.h"
#include "sprite_util.h"
#include "background_registers.h"
#include "fixed_point.h"
#include "global_data.h"
#include "gba/m4a.h"
#include "gba/macro.h"
#include "main.h"
#include "score.h"
#include "sound.h"
#include "voice_set.h"
#include "wario.h"

extern const struct AnimationFrame sUnk_83CBE54[];
extern const struct AnimationFrame sCatbatOam_83CD334[];
extern const struct AnimationFrame sCatbatOam_83CD35C[];
extern const struct AnimationFrame sCatbatOam_83CD374[];
extern const struct AnimationFrame sCatbatOam_83CD39C[];
extern const struct AnimationFrame sCatbatOam_83CD3EC[];
extern const struct AnimationFrame sCatbatOam_83CE468[];
extern const struct AnimationFrame sCatbatOam_83CE490[];
extern const struct AnimationFrame sCatbatOam_83CE4A8[];
extern const struct AnimationFrame sCatbatOam_83CE4D0[];
extern const struct AnimationFrame sCatbatOam_83CE4F8[];
extern const struct AnimationFrame sCatbatOam_83CF164[];
extern const struct AnimationFrame sCatbatOam_83CF370[];
extern const struct AnimationFrame sCatbatOam_83CF3F0[];
extern const struct AnimationFrame sCatbatOam_83CF400[];
extern const struct AnimationFrame sCatbatOam_83CF478[];
extern const struct AnimationFrame sCatbatOam_83CF4B8[];
extern const struct AnimationFrame sCatbatOam_83CF594[];
extern const struct AnimationFrame sCatbatOam_83CF5BC[];
extern const struct AnimationFrame sCatbatOam_83CF5D4[];
extern const struct AnimationFrame sCatbatOam_83CF5EC[];
extern const struct AnimationFrame sCatbatOam_83CF8BC[];
extern const struct AnimationFrame sUnk_83CF8FC[];
extern const u8 sUnk_83CF920[];
extern const u8 sUnk_83CF930[];
extern const u8 sUnk_83CF968[];
extern const u8 sUnk_83CF970[];
extern const s16 sUnk_83CF980[];
extern const s16 sUnk_83CF9EA[];
extern const s16 sUnk_83CFA0A[];
extern const s16 sUnk_83CFA7C[];
extern const s16 sUnk_83CFACE[];
extern GBA_PTR(const void *) const sUnk_878F0E4[];
extern GBA_PTR(const void *) const sUnk_878F0F8[];

void UpdateBossHealthGauge(void);
void DespawnActiveCatbatProjectile(void)
{
    u8 *cursor;
    u8 *end;
    int offset;
    int one;
    register int zero PIN("r3");

    cursor = (u8 *)gSpriteData;
    offset = 0xFD;
    offset <<= 2;
    end = cursor + offset;
    one = 1;
    zero = 0;
    do {
        register u16 status PIN("r1");
        register int active PIN("r0");

        if (((struct PrimarySpriteData *)cursor)->globalID == PSPRITE_CATBAT_PROJECTILE) {
            status = ((struct PrimarySpriteData *)cursor)->status;
            active = one;
            active &= status;
            if (active != 0) {
                ((struct PrimarySpriteData *)cursor)->status = zero;
                break;
            }
        }
        cursor += sizeof(struct PrimarySpriteData);
    } while ((s32)cursor <= (s32)end);
}
int UpdateCatbatGraphicsAnimation(const u8 *table)
{
    const u8 *sequence;
    int wrapped;
    struct PrimarySpriteData *sprite;
    u8 *delay;
    u8 *indexPtr;
    unsigned value;
    unsigned index;
    unsigned frame;
    register unsigned offset PIN("r0");
    volatile u32 *dma;
    register const u8 *tableBase;
    const u8 *src;
    u32 control;

    sequence = table;
    if (gCuckooCondorHasCapturedWario != 0)
        sequence = sUnk_83CF920;

    wrapped = FALSE;
    sprite = &gCurrentSprite;
    delay = &sprite->work1;
    value = *delay;
    if (value != 0) {
        value--;
        *delay = value;
        return FALSE;
    }

    indexPtr = &sprite->work0;
    value = *indexPtr;
    index = value + 1;
    *indexPtr = index;
    value <<= 24;
    index = value >> 24;
    frame = sequence[index * 4];
    if (frame == 0xFF) {
        index = 0;
        value = 1;
        *indexPtr = value;
        frame = sequence[0];
        wrapped = TRUE;
    }
    offset = index << 2;
    *delay = sequence[offset + 1];

    dma = (vu32 *)0x040000D4;
    tableBase = (const u8 *)sUnk_878F0E4;
    offset = frame << 2;
    /* agbcc canonicalizes the commutative address add with the operands reversed. */
    offset -= -(int)tableBase;
    src = GP(*(GBA_PTR(const u8 *) *)offset);
    dma[0] = (u32)src;
    dma[1] = 0x06015820;
    control = 0x80000070;
    dma[2] = control;
    (void)dma[2];
    src += 0xE0;
    dma[0] = (u32)src;
    dma[1] = 0x06015C20;
    dma[2] = control;
    (void)dma[2];
    return wrapped;
}
void UpdateCatbatBackgroundAnimation(const u8 *sequence)
{
    register const u8 *table PIN("r2");
    u8 *delay;
    u8 *indexPtr;
    unsigned value;
    unsigned index;
    unsigned frame;
    register unsigned offset PIN("r0");
    volatile u32 *dma;
    register const u8 *tableBase;
    const u8 *src;

    table = sequence;
    delay = &gBgAnimationTimer;
    value = *delay;
    if (value != 0) {
        value--;
        *delay = value;
        return;
    }

    indexPtr = &gBgAnimationFrame;
    value = *indexPtr;
    index = value + 1;
    *indexPtr = index;
    value <<= 24;
    index = value >> 24;
    offset = index << 2;
    /* agbcc reverses this commutative sequence-address addition. */
    offset -= -(int)table;
    frame = *(const u8 *)offset;
    if (frame == 0xFF) {
        index = 0;
        value = 1;
        *indexPtr = value;
        frame = table[0];
    }
    offset = index << 2;
    *delay = table[offset + 1];

    dma = (vu32 *)0x040000D4;
    tableBase = (const u8 *)sUnk_878F0F8;
    offset = frame << 2;
    /* agbcc reverses this commutative table-address addition. */
    offset -= -(int)tableBase;
    src = GP(*(GBA_PTR(const u8 *) *)offset);
    dma[0] = (u32)src;
    dma[1] = 0x06014000;
    dma[2] = 0x80000200;
    (void)dma[2];
}
void InitCatbatMineSpawner(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    u16 oldStatus;
    register u16 status PIN("r0");
    int mask;
    register int zeroByte PIN("r3");
    int zeroHalf;
    u8 *distance;
    int sixteen;
    u8 *hitbox;
    const struct AnimationFrame *oam;
    register struct PrimarySpriteData *base PIN("r1");

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    mask = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    status = mask;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    base = sprite;
    base->status = status;
    base->warioCollision = 65;

    distance = &sprite->drawDistanceDown;
    sixteen = 16;
    *distance = sixteen;
    distance++;
    *distance = sixteen;
    distance++;
    *(volatile u8 *)distance = sixteen;

    hitbox = &base->hitboxExtentUp;
    *hitbox = 48;
    hitbox++;
    *hitbox = 80;
    hitbox++;
    *hitbox = 32;
    hitbox++;
    *(volatile u8 *)hitbox = 32;

    oam = sCatbatOam_83CF478;
    base = sprite;
    GP_SET(base->pOamData, oam);
    base->currentAnimationFrame = zeroByte;
    base->animationTimer = zeroHalf;
    base->pose = sixteen;
    SpriteUtilFindSpriteSlotWork3(81);
}
void UpdateCatbatMineSpawnerPosition(void)
{
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *sprites PIN("r2");
    int slot;
    int value;
    register u16 status PIN("r1");

    sprite = &gCurrentSprite;
    slot = sprite->work3;
    sprites = gSpriteData;
    if (sprites[slot].status & SPRITE_STATUS_FACING_RIGHT) {
        status = sprite->status;
        value = SPRITE_STATUS_FACING_RIGHT;
        value |= status;
    } else {
        status = sprite->status;
        value = status & ~SPRITE_STATUS_FACING_RIGHT;
    }
    sprite->status = value;

    status = *(volatile u16 *)&sprite->status;
    value = SPRITE_STATUS_FACING_RIGHT;
    value &= status;
    if (value != 0) {
        slot = sprite->work3;
        value = sprites[slot].xPosition + 16;
    } else {
        slot = sprite->work3;
        value = sprites[slot].xPosition - 16;
    }
    *(volatile u16 *)&sprite->xPosition = value;

    {
        u8 *slotPtr;
        /* Materialize the sprite base only after the volatile X-position store. */
        ASM_BARRIER(: "=r"(slotPtr) : "0"(sprite));
        slotPtr += 42;
        slot = *slotPtr;
        value = sprites[slot].yPosition - 216;
        sprite->yPosition = value;
    }
}
void UpdateCatbatHitboxFacing(void)
{
    struct PrimarySpriteData *sprite;
    register u16 status PIN("r1");
    u8 *extent;
    int value;

    sprite = &gCurrentSprite;
    status = sprite->status;
    value = SPRITE_STATUS_FACING_RIGHT;
    if (value & status) {
        extent = &sprite->hitboxExtentLeft;
        value = 48;
        *extent = value;
        extent++;
        value = 80;
    } else {
        extent = &sprite->hitboxExtentLeft;
        value = 80;
        *extent = value;
        extent++;
        value = 48;
    }
    *extent = value;
}
void InitCatbat(void)
{
    struct PrimarySpriteData *sprite;
    register int zeroByte PIN("r2");
    int zeroHalf;
    u8 *ptr;

    gBossState = 0;
    gCuckooCondorPendulumLength = 0;
    gCuckooCondorMoveRight = 0;
    gCuckooCondorHasCapturedWario = 0;
    gInitialHealth = 12;
    gBossSequenceState = 0;
    gBgAnimationFrame = 0;
    gBgAnimationTimer = 0;
    gSpriteAiDynamicGraphicsTimer = 0;

    sprite = &gCurrentSprite;
    {
        int priority;
        register int priorityMask PIN("r1");

        priority = sprite->drawPriority;
        priorityMask = 128;
        priority |= priorityMask;
        zeroHalf = 0;
        sprite->drawPriority = priority;
    }
    {
        register u16 oldStatus PIN("r1");
        u16 status;
        int background;

        oldStatus = sprite->status;
        status = oldStatus & ~SPRITE_STATUS_HIDDEN;
        zeroByte = 0;
        background = SPRITE_STATUS_BACKGROUND;
        status |= background;
        sprite->status = status;
    }
    ptr = &sprite->drawDistanceDown;
    *ptr = 80;
    ptr++;
    *ptr = 80;
    ptr++;
    *ptr = 80;
    ptr++;
    *ptr = 128;
    ptr++;
    *ptr = 124;
    sprite->warioCollision = 64;
    sprite->health = 12;
    sprite->pose = 1;
    GP_SET(sprite->pOamData, sCatbatOam_83CE468);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->work3 = zeroByte;
    sprite->work0 = zeroByte;
    sprite->work1 = zeroByte;
    UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
    SpriteUtilTurnTowardWario();
    UpdateCatbatHitboxFacing();
    UpdateBossHealthGauge();
    sprite->work2 = 60;
    LoadBossSpriteGraphics(131, 8, 4);
    SpawnHighPriorityPrimarySprite(224, 0, 0, sprite->yPosition + 352, sprite->xPosition + 128);
    SpawnHighPriorityPrimarySprite(251, 0, 0, sprite->yPosition - 216, sprite->xPosition - 16);
    sprite->yPosition += 64;
}
void UpdateCatbatIntroWaitForShop(void)
{
    struct PrimarySpriteData *sprite;
    int index;
    const s16 *table;
    u32 velocity;
    int signedVelocity;
    int timerValue;

    UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
    sprite = &gCurrentSprite;
    index = sprite->work3;
    table = sUnk_83CF980;
    velocity = *(const u16 *)&table[index];
    signedVelocity = table[index];
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)&table[0];
        index = 0;
    }
    sprite->work3 = index + 1;
    sprite->yPosition += velocity;

    timerValue = sprite->work2;
    if (timerValue != 0) {
        timerValue--;
        sprite->work2 = timerValue;
        if ((timerValue << 24) != 0)
            return;
        gBossDefeatTimer[0] = 7;
    }

    if (gBossDefeatTimer[0] == 0 && gCurrentShopItem == 0) {
        sprite->pose = 2;
        sprite->work2 = 20;
    }
}
void UpdateCatbatIntroDelay(void)
{
    struct PrimarySpriteData *sprite;
    u8 *indexPtr;
    int index;
    const s16 *table;
    int entryAddress;
    const s16 *entry;
    u32 velocity;
    int signedVelocity;
    int zero;
    int nextIndex;
    u8 *storePtr;
    u8 *timer;
    int timerValue;
    int normalizedTimer;
    int frameZero;

    UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sUnk_83CF980;
    entryAddress = index << 1;
    /* agbcc reverses this commutative table-address addition. */
    entryAddress -= -(int)table;
    entry = (const s16 *)entryAddress;
    velocity = *(const u16 *)entry;
    zero = 0;
    signedVelocity = *entry;
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)&table[0];
        index = 0;
    }
    nextIndex = index + 1;
    storePtr = indexPtr;
    *storePtr = nextIndex;
    sprite->yPosition += velocity;
    timer = &sprite->work2;
    timerValue = *timer - 1;
    *timer = timerValue;
    normalizedTimer = (u8)timerValue;
    if (normalizedTimer == 0) {
        sprite->pose = 3;
        *timer = 96;
        GP_SET(sprite->pOamData, sCatbatOam_83CF164);
        frameZero = 0;
        sprite->currentAnimationFrame = frameZero;
        sprite->animationTimer = normalizedTimer;
        m4aSongNumStart(SOUND_D4);
    }
}
void UpdateCatbatIntroRoar(void)
{
    struct PrimarySpriteData *sprite;
    u8 *indexPtr;
    int index;
    const s16 *table;
    int entryAddress;
    const s16 *entry;
    u32 velocity;
    int signedVelocity;
    int zero;
    int nextIndex;
    u8 *storePtr;
    u8 *timer;
    int timerValue;
    int normalizedTimer;
    int frameZero;

    UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sUnk_83CF980;
    entryAddress = index << 1;
    /* agbcc reverses this commutative table-address addition. */
    entryAddress -= -(int)table;
    entry = (const s16 *)entryAddress;
    velocity = *(const u16 *)entry;
    zero = 0;
    signedVelocity = *entry;
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)&table[0];
        index = 0;
    }
    nextIndex = index + 1;
    storePtr = indexPtr;
    *storePtr = nextIndex;
    sprite->yPosition += velocity;
    timer = &sprite->work2;
    timerValue = *timer - 1;
    *timer = timerValue;
    normalizedTimer = ((u32)timerValue << 24) >> 24;
    if (normalizedTimer == 0) {
        sprite->pose = 4;
        GP_SET(sprite->pOamData, sCatbatOam_83CD334);
        frameZero = 0;
        sprite->currentAnimationFrame = frameZero;
        sprite->animationTimer = normalizedTimer;
        *timer = 60;
    }
}
void UpdateCatbatIntroBossTimer(void)
{
    struct PrimarySpriteData *sprite;
    u8 *indexPtr;
    int index;
    const s16 *table;
    int entryAddress;
    const s16 *entry;
    u32 velocity;
    int signedVelocity;
    int zero;
    int nextIndex;
    u8 *storePtr;
    u8 *timer;
    int timerValue;

    UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sUnk_83CF980;
    entryAddress = index << 1;
    /* agbcc reverses this commutative table-address addition. */
    entryAddress -= -(int)table;
    entry = (const s16 *)entryAddress;
    velocity = *(const u16 *)entry;
    zero = 0;
    signedVelocity = *entry;
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)&table[0];
        index = 0;
    }
    nextIndex = index + 1;
    storePtr = indexPtr;
    *storePtr = nextIndex;
    sprite->yPosition += velocity;
    timer = &sprite->work2;
    timerValue = *timer - 1;
    *timer = timerValue;
    if ((timerValue << 24) == 0) {
        sprite->pose = 5;
        SpriteUtilStartBossTimer();
    }
}
void UpdateCatbatIntroWaitForTimer(void)
{
    struct PrimarySpriteData *sprite;
    u8 *indexPtr;
    int index;
    const s16 *table;
    int entryAddress;
    const s16 *entry;
    u32 velocity;
    int signedVelocity;
    int zero;
    int nextIndex;
    u8 *storePtr;

    UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sUnk_83CF980;
    entryAddress = index << 1;
    /* agbcc reverses this commutative table-address addition. */
    entryAddress -= -(int)table;
    entry = (const s16 *)entryAddress;
    velocity = *(const u16 *)entry;
    zero = 0;
    signedVelocity = *entry;
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)&table[0];
        index = 0;
    }
    nextIndex = index + 1;
    storePtr = indexPtr;
    *storePtr = nextIndex;
    sprite->yPosition += velocity;
    if (gWarioPauseTimer == 0 && gTimerState == 1) {
        SpriteUtilUnsetAllSpritesHighPriority();
        gSwitchStates[2] = 2;
        sprite->work2 = 60;
        sprite->pose = 16;
    }
}
void InitCatbatPatrol(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *current;
    register const struct AnimationFrame *animation PIN("r0");
    u8 health;

    sprite = &gCurrentSprite;
    {
        u8 *work;
        int zero;

        work = &sprite->work3;
        zero = 0;
        *work = zero;
    }
    health = sprite->health;
    current = sprite;
    if (health <= 3)
        sprite->work2 = 60;
    else
        current->work2 = 120;
    current->pose = 16;
    if (current->health <= 1) {
        if (gSpriteAiDynamicGraphicsTimer != 0)
            animation = sCatbatOam_83CE4A8;
        else
            animation = sCatbatOam_83CD374;
    } else {
        if (gSpriteAiDynamicGraphicsTimer != 0)
            animation = sCatbatOam_83CE468;
        else
            animation = sCatbatOam_83CD334;
    }
    GP_SET(current->pOamData, animation);
    {
        int zeroFrame;
        int zeroWork;
        u8 *work;

        zeroFrame = 0;
        current->currentAnimationFrame = zeroFrame;
        zeroWork = 0;
        current->animationTimer = zeroFrame;
        work = &current->work0;
        *work = zeroWork;
        work++;
        *work = zeroWork;
    }
    UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
}
void UpdateCatbatPatrol(void)
{
    struct PrimarySpriteData *sprite;
    struct WarioData *wario;
    int index;
    register struct WarioData *reactionWario PIN("r1");
    const s16 *table;
    u32 velocity;
    int signedVelocity;
    u32 right;
    u32 left;
    register u32 oldX PIN("r6");
    register u32 oldXLeft PIN("r9");

    wario = &gWarioData;
    right = gWarioData.xPosition;
    left = right;
    sprite = &gCurrentSprite;
    oldX = sprite->xPosition;
    oldXLeft = oldX;

    UpdateCatbatGraphicsAnimation((const u8 *)sUnk_83CF8FC);
    index = sprite->work3;
    table = sUnk_83CF980;
    velocity = *(const u16 *)&table[index];
    signedVelocity = table[index];
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)&table[0];
        index = 0;
    }
    sprite->work3 = index + 1;
    {
    u32 zero;
    {
        u32 y;
        y = sprite->yPosition + velocity;
        zero = 0;
        sprite->yPosition = y;
    }

    if (sprite->work2 != 0)
        sprite->work2--;

    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 100));
        if (gSpriteCollisionTileType == 17) {
            {
                register u8 *length PIN("r1");
                register u32 one PIN("r0");
                length = &gCuckooCondorPendulumLength;
                one = 1;
                *length = one;
            }
            sprite->disableWarioCollisionTimer = 1;
            {
                u8 slot;
                slot = SpriteUtilFindSpriteSlotOrU8Max(PSPRITE_CATBAT_MINE_SPAWNER);
                if (slot != 0xFF)
                    gSpriteData[slot].disableWarioCollisionTimer = 1;
            }
            sprite->xPosition += 4;
            {
                register u32 status PIN("r1");
                status = sprite->status;
                if (!(status & SPRITE_STATUS_ONSCREEN)) {
                {
                register u32 mask PIN("r0");
                mask = SPRITE_STATUS_FACING_RIGHT;
                mask ^= status;
                sprite->status = mask;
            }
                UpdateCatbatHitboxFacing();
                sprite->xPosition -= 32;
            }
            }
        } else {
            gCuckooCondorPendulumLength = zero;
            sprite->status &= ~SPRITE_STATUS_HIDDEN;
            if (right > oldX && (reactionWario = wario)->reaction != 8) {
                func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 160));
                if (gSpriteCollisionTileType == 17)
                    sprite->xPosition += 2;
                else
                    sprite->xPosition += 1;
                sprite = &gCurrentSprite;
                if (sprite->health <= 1 && sprite->work2 == 25) {
                    DespawnActiveCatbatProjectile();
                    SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 1, 0, sprite->yPosition,
                        sprite->xPosition, SPRITE_STATUS_FACING_RIGHT);
                }
            } else {
                gCurrentSprite.xPosition += 4;
                gCurrentSprite.currentAnimationFrame++;
                return;
            }
        }
    } else {
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 100));
        if (gSpriteCollisionTileType == 17) {
            {
                register u8 *length PIN("r1");
                register u32 one PIN("r0");
                length = &gCuckooCondorPendulumLength;
                one = 1;
                *length = one;
            }
            sprite->disableWarioCollisionTimer = 1;
            {
                u8 slot;
                slot = SpriteUtilFindSpriteSlotOrU8Max(PSPRITE_CATBAT_MINE_SPAWNER);
                if (slot != 0xFF)
                    gSpriteData[slot].disableWarioCollisionTimer = 1;
            }
            sprite->xPosition -= 4;
            {
                register u32 status PIN("r1");
                status = sprite->status;
                if (!(status & SPRITE_STATUS_ONSCREEN)) {
                {
                register u32 mask PIN("r0");
                mask = SPRITE_STATUS_FACING_RIGHT;
                mask ^= status;
                sprite->status = mask;
            }
                UpdateCatbatHitboxFacing();
                sprite->xPosition += 32;
            }
            }
        } else {
            gCuckooCondorPendulumLength = zero;
            {
                register u16 status PIN("r1");
                register u32 mask PIN("r0");
                status = sprite->status;
                mask = 0xFFFB;
                mask &= status;
                sprite->status = mask;
            }
            if (left < oldXLeft && (reactionWario = wario)->reaction != 8) {
                func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 160));
                if (gSpriteCollisionTileType == 17)
                    sprite->xPosition -= 2;
                else
                    sprite->xPosition -= 1;
                sprite = &gCurrentSprite;
                if (sprite->health <= 1 && sprite->work2 == 25) {
                    DespawnActiveCatbatProjectile();
                    SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 1, 0, sprite->yPosition,
                        sprite->xPosition, 0);
                }
            } else {
                gCurrentSprite.xPosition -= 4;
                gCurrentSprite.currentAnimationFrame++;
                return;
            }
        }
    }

    sprite = &gCurrentSprite;
    if (sprite->work2 == 0 && *(u8 *)&gCuckooCondorPendulumLength == 0) {
        register u16 x PIN("r1");
        u32 y;
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
            y = sprite->yPosition;
            x = sprite->xPosition - 160;
        } else {
            y = sprite->yPosition;
            x = sprite->xPosition + 160;
        }
        func_8023BFC(y, (u16)x);
        if (gSpriteCollisionTileType != 17)
            sprite->pose = 17;
    }
    }
}
void InitCatbatLandingAttack(void)
{
    struct PrimarySpriteData *sprite;
    register int zero PIN("r0");

    sprite = &gCurrentSprite;
    {
        u8 *work;
        work = &sprite->work3;
        zero = 0;
        *work = zero;
        sprite->pose = 18;
        work = &sprite->work0;
        *work = zero;
        work++;
        *work = zero;
    }
    m4aSongNumStart(SOUND_AF);
}
void UpdateCatbatLandingAttack(void)
{
    UpdateCatbatGraphicsAnimation(sUnk_83CF920);
    {
        struct PrimarySpriteData *sprite;
        u8 *indexPtr;
        int index;
        const s16 *table;
        int entryAddress;
        const s16 *entry;
        register u16 velocity PIN("r4");
        int zero;
        int signedVelocity;

        sprite = &gCurrentSprite;
        entryAddress = 42;
        entryAddress += (int)sprite;
        indexPtr = (u8 *)entryAddress;
        index = *indexPtr;
        table = sUnk_83CF9EA;
        entryAddress = index << 1;
        entryAddress += (int)table;
        entry = (const s16 *)entryAddress;
        velocity = *(const u16 *)entry;
        zero = 0;
        signedVelocity = *(s16 *)((u8 *)entry + zero);
        if (signedVelocity == 0x7FFF) {
            int previousOffset;
            const s16 *previous;
            register u16 position PIN("r0");

            previousOffset = index - 1;
            previousOffset <<= 1;
            previousOffset += (int)table;
            previous = (const s16 *)previousOffset;
            position = sprite->yPosition;
            {
                u16 previousValue;
                previousValue = *(const u16 *)previous;
                position -= -(int)previousValue;
            }
            sprite->yPosition = position;
        } else {
            int nextIndex;
            u8 *storePtr;
            u16 position;

            nextIndex = index + 1;
            storePtr = indexPtr;
            *storePtr = nextIndex;
            position = sprite->yPosition;
            position += velocity;
            sprite->yPosition = position;
        }
    }

    {
        struct PrimarySpriteData *sprite;
        int facingMask;
        int facing;
        int maskedFacing;
        register u16 status PIN("r1");

        sprite = &gCurrentSprite;
        func_8023BFC((u16)(sprite->yPosition + 128), sprite->xPosition);
        if (gBackgroundCollisionData.waterType != 1)
            return;

        sprite->pose = 20;
        sprite->work2 = 40;
        status = sprite->status;
        facingMask = SPRITE_STATUS_FACING_RIGHT;
        maskedFacing = facingMask;
        maskedFacing &= status;
        facing = (u16)maskedFacing;
        if (facing != 0) {
            SpawnSecondarySprite(sprite->yPosition + 128, sprite->xPosition - 60, 7);
            SpawnSecondarySprite(sprite->yPosition + 128, sprite->xPosition - 4, 7);
            SpawnSecondarySprite(sprite->yPosition + 128, sprite->xPosition - 116, 7);
            SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_LANDING_DEBRIS, gBossState, 0, sprite->yPosition + 128,
                sprite->xPosition - 128, facingMask);
        } else {
            SpawnSecondarySprite(sprite->yPosition + 128, sprite->xPosition + 4, 7);
            SpawnSecondarySprite(sprite->yPosition + 128, sprite->xPosition + 60, 7);
            SpawnSecondarySprite(sprite->yPosition + 128, sprite->xPosition + 116, 7);
            SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_LANDING_DEBRIS, gBossState, 0, sprite->yPosition + 128,
                sprite->xPosition + 128, facing);
        }
        gBossState++;
    }
}
void UpdateCatbatLandingRecovery(void)
{
    struct PrimarySpriteData *sprite;
    u16 position;
    u16 difference;
    u16 background;

    UpdateCatbatGraphicsAnimation(sUnk_83CF920);
    sprite = &gCurrentSprite;
    position = sprite->yPosition;
    difference = position >> 2;
    background = gBg1YPosition >> 2;
    difference -= background;
    difference = (u16)difference;
    if (difference > 80) {
        int nextPosition;
        nextPosition = position;
        nextPosition -= 8;
        sprite->yPosition = nextPosition;
    } else {
        u8 *timer;
        int value;

        timer = &sprite->work2;
        value = *timer - 1;
        *timer = value;
        value <<= 24;
        if (value == 0)
            sprite->pose = 15;
    }
}
void InitCatbatDefeat(void)
{
    struct PrimarySpriteData *timerSprite;
    int timerValue;
    struct PrimarySpriteData *sprite;
    int zero;
    int pose;
    struct WarioData *wario;

    timerSprite = &gCurrentSprite;
    timerValue = timerSprite->health;
    if (timerValue != 0) {
        timerValue--;
        timerSprite->health = timerValue;
        if ((timerValue << 24) == 0) {
            gSwitchStates[2] = 3;
            gTimerState = 11;
            SpriteUtilSetWarioBossVictoryPose();
        }
        UpdateBossHealthGauge();
    }

    gCuckooCondorMoveRight = 1;
    sprite = &gCurrentSprite;
    zero = 0;
    pose = 50;
    sprite->pose = pose;
    GP_SET(sprite->pOamData, sCatbatOam_83CE490);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite = (struct PrimarySpriteData *)((u8 *)sprite + 41);
    pose = 64;
    *(u8 *)sprite = pose;
    wario = &gWarioData;
    SpawnSecondarySprite(wario->yPosition, wario->xPosition, 5);
    SpriteUtilTurnTowardWario();
    UpdateCatbatHitboxFacing();
    VoiceSetPlay(14);
}
void UpdateCatbatDefeat(void)
{
    struct PrimarySpriteData *sprite;
    u16 position;
    u8 *timerPtr;

    sprite = &gCurrentSprite;
    position = sprite->yPosition;
    if ((u16)((position >> 2) - (gBg1YPosition >> 2)) > 80) {
        int newPosition;
        newPosition = position - 2;
        sprite->yPosition = newPosition;
    }

    timerPtr = &sprite->work2;
    if (*timerPtr != 0) {
        int timerValue;
        u8 timer;

        timerValue = *timerPtr;
        timerValue--;
        *timerPtr = timerValue;
        timer = (u8)timerValue;
        if (timer > 31) {
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
                func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 96));
                if (gSpriteCollisionTileType != 17)
                    sprite->xPosition -= 4;
            } else {
                func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 96));
                if (gSpriteCollisionTileType != 17)
                    sprite->xPosition += 4;
            }

            if ((timer & 7) == 0) {
                if (timer & 8) {
                    gCurrentSprite.palette = 4;
                    if (timer == 56)
                        VoiceSetPlay(0);
                } else {
                    gCurrentSprite.palette = 0;
                }
            }
        }
    } else {
        if (sprite->health == 0)
            sprite->pose = 120;
        else
            sprite->pose = 110;
    }
}
void InitCatbatDamaged(void)
{
    struct PrimarySpriteData *sprite;
    int zero;
    int health;
    u8 slot;

    sprite = &gCurrentSprite;
    health = sprite->health;
    if (health != 0) {
        health--;
        sprite->health = health;
        UpdateBossHealthGauge();
    }

    gCuckooCondorMoveRight = 1;
    zero = 0;
    sprite->pose = 109;
    GP_SET(sprite->pOamData, sCatbatOam_83CD35C);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    if (sprite->health < ((gInitialHealth >> 1) + 1)) {
        gSpriteAiDynamicGraphicsTimer = 1;
        slot = SpriteUtilFindSpriteSlotOrU8Max(PSPRITE_CATBAT_MINE_SPAWNER);
        if (slot != 0xFF)
            gSpriteData[slot].status = zero;
        {
            u8 *timer;
            timer = &sprite->work2;
            *timer = 192;
        }
    } else {
        u8 *timer;
        timer = &sprite->work2;
        *timer = 64;
    }
    SpriteUtilTurnTowardWario();
    UpdateCatbatHitboxFacing();
    VoiceSetPlay(14);
}
void UpdateCatbatDamaged(void)
{
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *savedSprite PIN("r6");
    u8 *timerPtr;
    int timerValue;
    int timer;
    u16 position;

    sprite = &gCurrentSprite;
    position = sprite->yPosition;
    timerValue = (u16)((position >> 2) - (gBg1YPosition >> 2));
    savedSprite = sprite;
    if ((u32)timerValue > 80)
        sprite->yPosition = position - 2;

    timerPtr = &sprite->work2;
    timerValue = *timerPtr;
    if (timerValue == 0)
        goto setPose;

    timerValue--;
    *timerPtr = timerValue;
    timer = (u8)timerValue;

    if (gSpriteAiDynamicGraphicsTimer != 0) {
        if ((s8)timer < 0)
            goto move;

        if (timer == 88) {
            SpawnSecondarySprite(sprite->yPosition - 216, sprite->xPosition, 6);
            SpriteSpawnAsChild(10, 0, 0, sprite->yPosition - 128, sprite->xPosition);
            m4aSongNumStart(SOUND_D5);
        } else if (timer == 80) {
            SpawnSecondarySprite(sprite->yPosition - 144, sprite->xPosition - 32, 6);
        } else if (timer == 72) {
            SpawnSecondarySprite(sprite->yPosition - 144, sprite->xPosition + 32, 6);
        } else if (timer == 64) {
            DmaCopy16(3, sUnk_83CBE54, (void *)0x06015420, 0xE0);
            sprite->warioCollision = 63;
            GP_SET(sprite->pOamData, sCatbatOam_83CE490);
        }
        goto end;
    }

    if ((u8)timer <= 31)
        goto end;

move:
    sprite = savedSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 96));
        if (gSpriteCollisionTileType != 17)
            sprite->xPosition -= 4;
    } else {
        func_8023BFC(savedSprite->yPosition, (u16)(savedSprite->xPosition + 96));
        if (gSpriteCollisionTileType != 17)
            savedSprite->xPosition += 4;
    }

    if ((timer & 7) == 0) {
        u8 normalizedBit;
        u32 normalizedTemp;

        normalizedTemp = 8;
        normalizedTemp &= timer;
        normalizedBit = normalizedTemp;
        if (normalizedBit != 0) {
            gCurrentSprite.palette = 4;
            if (timer == 56)
                VoiceSetPlay(0);
        } else {
            gCurrentSprite.palette = normalizedBit;
        }
    }
    goto end;

setPose:
    sprite->pose = 110;

end:
    ;
}
void InitCatbatProjectileAttack(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *current;
    register const struct AnimationFrame *animation PIN("r0");
    int alternate;

    sprite = &gCurrentSprite;
    sprite->pose = 111;
    alternate = gSpriteAiDynamicGraphicsTimer;
    current = sprite;
    if (alternate != 0)
        animation = sCatbatOam_83CE4D0;
    else
        animation = sCatbatOam_83CD3EC;
    GP_SET(current->pOamData, animation);
    {
        int zeroFrame;
        int zeroWork;
        u8 *work;

        zeroFrame = 0;
        current->currentAnimationFrame = zeroFrame;
        zeroWork = 0;
        current->animationTimer = zeroFrame;
        work = &current->work0;
        *work = zeroWork;
        work++;
        *work = zeroWork;
    }
    {
        u8 *timer;
        int value;
        timer = &current->work2;
        value = 40;
        *timer = value;
    }
    m4aSongNumStart(SOUND_B2);
}
void UpdateCatbatProjectileAttackCharge(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    int value;

    UpdateCatbatGraphicsAnimation(sUnk_83CF968);
    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer - 1;
    *timer = value;
    value <<= 24;
    if (value == 0)
        sprite->pose = 112;
}
void InitCatbatProjectileAttackRelease(void)
{
    struct PrimarySpriteData *sprite;
    register const struct AnimationFrame *animation PIN("r0");

    sprite = &gCurrentSprite;
    sprite->pose = 113;
    if (gSpriteAiDynamicGraphicsTimer != 0)
        animation = sCatbatOam_83CE4A8;
    else
        animation = sCatbatOam_83CD374;
    GP_SET(sprite->pOamData, animation);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->work2 = 64;
    SpriteUtilTurnTowardWario();
    UpdateCatbatHitboxFacing();
}
void UpdateCatbatProjectileAttackRelease(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPtr;
    int timerValue;
    int normalizedTimer;
    int status;
    register int facingMask PIN("r2");
    register int statusBit PIN("r1");
    int facing;

    UpdateCatbatGraphicsAnimation(sUnk_83CF968);
    sprite = &gCurrentSprite;
    timerPtr = &sprite->work2;
    timerValue = *timerPtr;
    timerValue--;
    *timerPtr = timerValue;
    normalizedTimer = (u8)timerValue;
    if (normalizedTimer != 0) {
        status = sprite->status;
        facingMask = SPRITE_STATUS_FACING_RIGHT;
        statusBit = facingMask;
        statusBit &= status;
        statusBit = ((u32)statusBit << 16) >> 16;
        statusBit = -statusBit;
        facing = statusBit >> 31;
        facing &= facingMask;

        switch (*timerPtr) {
            case 60:
                DespawnActiveCatbatProjectile();
                SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 0, 0, sprite->yPosition, sprite->xPosition, facing);
                break;
            case 40:
                DespawnActiveCatbatProjectile();
                SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 3, 0, sprite->yPosition, sprite->xPosition, facing);
                break;
            case 20:
                DespawnActiveCatbatProjectile();
                SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 4, 0, sprite->yPosition, sprite->xPosition, facing);
                break;
        }
    } else {
        sprite->pose = 15;
        gCuckooCondorMoveRight = normalizedTimer;
    }
}
void InitCatbatMoveRight(void)
{
    register struct PrimarySpriteData *sprite PIN("r2");
    register u8 *timer PIN("r3");
    int zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    zero = 0;
    *timer = 45;
    sprite->pose = 32;
    {
        register u8 *work PIN("r0");
        work = &sprite->work0;
        *work = zero;
        work++;
        *work = zero;
    }
    m4aSongNumStart(SOUND_B2);
}
void UpdateCatbatMoveRight(void)
{
    struct PrimarySpriteData *sprite;
    u16 position;

    UpdateCatbatGraphicsAnimation(sUnk_83CF930);
    sprite = &gCurrentSprite;
    if (sprite->work2 != 0) {
        sprite->work2--;
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 64));
        if (gSpriteCollisionTileType != 17)
            sprite->xPosition += 4;
        position = sprite->yPosition;
        if ((u16)((position >> 2) - (gBg1YPosition >> 2)) > 64)
            sprite->yPosition = position - 1;
    } else {
        position = sprite->yPosition;
        if ((u16)((position >> 2) - (gBg1YPosition >> 2)) > 64)
            sprite->yPosition = position - 4;
        else
            sprite->pose = 21;
    }
}
void InitCatbatMoveLeft(void)
{
    register struct PrimarySpriteData *sprite PIN("r2");
    register u8 *timer PIN("r3");
    int zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    zero = 0;
    *timer = 45;
    sprite->pose = 34;
    {
        register u8 *work PIN("r0");
        work = &sprite->work0;
        *work = zero;
        work++;
        *work = zero;
    }
    m4aSongNumStart(SOUND_B2);
}
void UpdateCatbatMoveLeft(void)
{
    struct PrimarySpriteData *sprite;
    u16 position;

    UpdateCatbatGraphicsAnimation(sUnk_83CF930);
    sprite = &gCurrentSprite;
    if (sprite->work2 != 0) {
        sprite->work2--;
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 64));
        if (gSpriteCollisionTileType != 17)
            sprite->xPosition -= 4;
        position = sprite->yPosition;
        if ((u16)((position >> 2) - (gBg1YPosition >> 2)) > 64)
            sprite->yPosition = position - 1;
    } else {
        position = sprite->yPosition;
        if ((u16)((position >> 2) - (gBg1YPosition >> 2)) > 64)
            sprite->yPosition = position - 4;
        else
            sprite->pose = 21;
    }
}
void InitCatbatIdleAttack(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 22;
    if (gSpriteAiDynamicGraphicsTimer != 0)
        GP_SET(sprite->pOamData, sCatbatOam_83CE4A8);
    else
        GP_SET(sprite->pOamData, sCatbatOam_83CD374);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->work2 = 60;
}
void UpdateCatbatIdleAttack(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    register int oldTimer PIN("r0");
    register int timerValue PIN("r1");
    register int shiftedTimer PIN("r0");
    register u16 status PIN("r1");
    int facingMask;
    int facingTemp;
    int facing;

    UpdateCatbatGraphicsAnimation(sUnk_83CF930);
    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    oldTimer = *timer;
    timerValue = oldTimer - 1;
    *timer = timerValue;
    shiftedTimer = timerValue << 24;
    if (shiftedTimer != 0) {
        shiftedTimer = (u32)shiftedTimer >> 24;
        if (shiftedTimer == 40) {
            status = sprite->status;
            facingMask = SPRITE_STATUS_FACING_RIGHT;
            facingTemp = facingMask;
            facingTemp &= status;
            facing = ((u32)facingTemp << 16) >> 16;
            if (facing != 0) {
                DespawnActiveCatbatProjectile();
                SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 0, 0, sprite->yPosition, sprite->xPosition, facingMask);
            } else {
                DespawnActiveCatbatProjectile();
                SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 0, 0, sprite->yPosition, sprite->xPosition, facing);
            }
        }
    } else {
        sprite->pose = 15;
    }
}
void InitCatbatDeathExplosion(void)
{
    struct PrimarySpriteData *sprite;
    int zero;

    sprite = &gCurrentSprite;
    zero = 0;
    sprite->pose = 121;
    GP_SET(sprite->pOamData, sCatbatOam_83CE4F8);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->work2 = 120;
    m4aSongNumStart(SOUND_AD);
}
void UpdateCatbatDeathExplosion(void)
{
    struct PrimarySpriteData *base;
    u8 *timerPtr;
    struct PrimarySpriteData *sprite;
    int timerValue;
    u8 timer;

    base = &gCurrentSprite;
    timerPtr = &base->work2;
    timerValue = *timerPtr;
    timerValue--;
    *timerPtr = timerValue;
    timer = *timerPtr;
    sprite = base;
    switch (timer) {
        case 110:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 192, sprite->xPosition);
            break;
        case 107:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 192, sprite->xPosition - 64);
            break;
        case 103:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 192, sprite->xPosition + 128);
            break;
        case 100:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 128, sprite->xPosition - 32);
            break;
        case 97:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 128, sprite->xPosition - 128);
            break;
        case 93:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 128, sprite->xPosition + 64);
            break;
        case 90:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 64, sprite->xPosition + 32);
            break;
        case 87:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 64, sprite->xPosition - 64);
            break;
        case 83:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition - 64, sprite->xPosition + 128);
            break;
        case 80:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition, sprite->xPosition);
            break;
        case 77:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition, sprite->xPosition - 128);
            break;
        case 73:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition, sprite->xPosition + 96);
            break;
        case 70:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition + 64, sprite->xPosition - 32);
            break;
        case 67:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition + 64, sprite->xPosition - 96);
            break;
        case 63:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition + 64, sprite->xPosition + 64);
            break;
        case 60:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition + 128, sprite->xPosition - 32);
            break;
        case 57:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition + 128, sprite->xPosition - 96);
            break;
        case 53:
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition + 128, sprite->xPosition + 64);
            break;
        case 50:
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition + 96, sprite->xPosition + 96);
            break;
        case 40:
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition, sprite->xPosition);
            break;
        case 30:
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition - 64, sprite->xPosition - 64);
            break;
        case 20:
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition + 128, sprite->xPosition + 64);
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition + 32, sprite->xPosition);
            break;
        case 24:
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition + 32, sprite->xPosition - 128);
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition + 32, sprite->xPosition + 128);
            SpawnHighPriorityPrimarySprite(10, 0, 0, sprite->yPosition + 128, sprite->xPosition);
            break;
        case 0:
            SpawnHighPriorityPrimarySprite(207, 0, 0, sprite->yPosition, sprite->xPosition);
            sprite->status = 0;
            break;
    }
}
void InitCatbatShopItemHit(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work;
    int delay;
    int zero;

    switch (gCurrentShopItem) {
    case 2: delay = 11; break;
    case 3: delay = 9; break;
    case 4: delay = 8; break;
    case 5: delay = 2; break;
    case 6: delay = 5; break;
    case 7: delay = 3; break;
    case 8: delay = 4; break;
    default: delay = 10; break;
    }

    sprite = &gCurrentSprite;
    work = &sprite->work2;
    zero = 0;
    *work = delay;
    GP_SET(sprite->pOamData, sCatbatOam_83CE490);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    gBossSequenceState = 32;
    sprite->pose = 123;
    {
        int health;
        health = sprite->health;
        if (health != 0) {
            health--;
            sprite->health = health;
            UpdateBossHealthGauge();
            m4aSongNumStart(SE_DAMAGE);
        }
    }
    VoiceSetPlay(14);
}
void UpdateCatbatShopItemHit(void)
{
    u8 *globalTimer;
    int timerValue;

    globalTimer = &gBossSequenceState;
    timerValue = *globalTimer;
    if (timerValue != 0) {
        int timer;
        int bit;

        timerValue--;
        *globalTimer = timerValue;
        timer = (u8)timerValue;
        bit = 7;
        bit &= timer;
        if (bit == 0) {
            bit = 8;
            bit &= timer;
            /* Keep the byte normalization as the target's separate shifts. */
            timer = (u8)bit;
            if (timer != 0)
                gCurrentSprite.palette = 4;
            else
                gCurrentSprite.palette = timer;
        }
    } else {
        struct PrimarySpriteData *base;
        u8 *work2Ptr;
        int health;
        register int healthCopy PIN("r2");
        struct PrimarySpriteData *sprite;
        int work2;

        base = &gCurrentSprite;
        work2Ptr = &base->work2;
        health = base->health;
        healthCopy = health;
        sprite = base;
        work2 = *work2Ptr;
        if (healthCopy == work2) {
            register int delay PIN("r0");

            switch (gCurrentShopItem) {
                case 3:
                case 4:
                case 6:
                case 8:
                    delay = 240;
                    break;
                case 5:
                case 7:
                    delay = 120;
                    break;
                default:
                    delay = 180;
                    break;
            }
            {
                u8 *timerPtr;
                int zeroByte;

                timerPtr = &sprite->work2;
                zeroByte = 0;
                *timerPtr = delay;
                gInitialHealth = sprite->health;
                sprite->pose = 2;
                GP_SET(sprite->pOamData, sCatbatOam_83CE468);
                sprite->currentAnimationFrame = zeroByte;
                {
                    int zeroWork;
                    register u8 *work PIN("r0");
                    zeroWork = 0;
                    sprite->animationTimer = zeroByte;
                    work = &sprite->work0;
                    *work = zeroWork;
                    work++;
                    *work = zeroWork;
                }
            }
            UpdateCatbatGraphicsAnimation(sUnk_83CF8FC);
        } else {
            globalTimer[0] = 32;
            if (healthCopy != 0) {
                timerValue = health - 1;
                base->health = timerValue;
                UpdateBossHealthGauge();
                m4aSongNumStart(SE_DAMAGE);
            }
        }
    }
}
void InitCatbatGroundWave(void)
{
    struct PrimarySpriteData *sprite;
    u16 oldStatus;
    register u16 status PIN("r0");
    int mask;
    register int zeroByte PIN("r2");
    int zeroHalf;
    u8 *distance;
    int extentValue;
    u8 *centerDistance;
    u8 *hitbox;
    u8 *sideHitbox;
    const struct AnimationFrame *oam;
    struct PrimarySpriteData *base;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    mask = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    status = mask;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    base = sprite;
    base->status = status;
    base->warioCollision = 83;

    distance = &sprite->drawDistanceDown;
    extentValue = 16;
    *distance = extentValue;
    distance++;
    *distance = extentValue;

    centerDistance = &sprite->drawDistanceLeftRight;
    *centerDistance = 48;

    hitbox = &sprite->hitboxExtentUp;
    extentValue = 20;
    *hitbox = extentValue;
    hitbox++;
    *hitbox = extentValue;

    sideHitbox = &sprite->hitboxExtentLeft;
    *sideHitbox = 56;
    sideHitbox++;
    *sideHitbox = 52;

    oam = sCatbatOam_83CF594;
    base = sprite;
    GP_SET(base->pOamData, oam);
    base->currentAnimationFrame = zeroByte;
    base->animationTimer = zeroHalf;
    base->pose = 1;
}
void UpdateCatbatGroundWave(void)
{
    struct PrimarySpriteData *sprite;
    int one;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    if (gWarioPauseTimer == 0) {
        one = gTimerState;
        if (one == 1) {
            sprite->status &= ~SPRITE_STATUS_HIDDEN;
            SpawnHighPriorityPrimarySprite(11, 0, 0, sprite->yPosition, sprite->xPosition);
            sprite->pose = 16;
            {
                u8 *work;
                work = &sprite->work2;
                *work = one;
            }
        }
    }
}
void UpdateCatbatGroundWaveTurn(void)
{
    struct PrimarySpriteData *sprite;
    int zero;
    u8 *timer;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sCatbatOam_83CF5EC);
    zero = 0;
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->pose = 16;
    timer = &sprite->work2;
    *timer = 8;
    sprite->warioCollision = 83;
}
void InitCatbatGroundWaveReturn(void)
{
    struct PrimarySpriteData *sprite;
    int zero;
    u8 *timer;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sCatbatOam_83CF594);
    zero = 0;
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->pose = 16;
    timer = &sprite->work2;
    *timer = 1;
    sprite->warioCollision = 83;
}
void UpdateCatbatGroundWaveReturn(void)
{
    struct PrimarySpriteData *sprite;
    int position;
    register int velocity PIN("r1");
    u8 *work;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 80));
        if (gSpriteCollisionTileType == 17) {
            sprite->pose = 17;
            return;
        }
        work = &sprite->work2;
        position = sprite->xPosition;
        velocity = *work;
        position += velocity;
    } else {
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 80));
        if (gSpriteCollisionTileType == 17) {
            sprite->pose = 17;
            return;
        }
        {
            u8 *leftWork;
            leftWork = &sprite->work2;
            velocity = *leftWork;
            position = sprite->xPosition;
            position -= velocity;
        }
    }
    sprite->xPosition = position;
}
void InitCatbatGroundWaveDisappear(void)
{
    struct PrimarySpriteData *sprite;
    int zero;
    u8 *timer;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sCatbatOam_83CF5BC);
    zero = 0;
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->pose = 18;
    timer = &sprite->work0;
    *timer = 9;
    sprite->warioCollision = 84;
}
void UpdateCatbatGroundWaveDisappear(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    register int value PIN("r0");
    int zero;
    u8 pose;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    zero = (u8)value;
    if (zero != 0)
        return;

    pose = sprite->pose;
    switch (pose) {
    case 18: {
        int clear;
        value = sprite->status ^ SPRITE_STATUS_FACING_RIGHT;
        clear = 0;
        sprite->status = value;
        GP_SET(sprite->pOamData, sCatbatOam_83CF5D4);
        sprite->currentAnimationFrame = clear;
        sprite->animationTimer = zero;
        sprite->pose = 20;
        value = 7;
        *timer = value;
        break;
    }
    case 20:
        sprite->pose = 15;
        break;
    }
}
void InitCatbatGroundWaveDelay(void)
{
    struct PrimarySpriteData *sprite;
    int zero;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sCatbatOam_83CF594);
    zero = 0;
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->pose = 40;
    sprite->work0 = 30;
}
void UpdateCatbatGroundWaveDelay(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    int value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    value <<= 24;
    if (value == 0)
        sprite->pose = 17;
}
void InitCatbatLandingDebris(void)
{
  struct PrimarySpriteData *sprite;
  u16 oldStatus;
  short persistentZero;
  register unsigned short status PIN("r0");
  u8 *animationFramePtr;
  int mask;
  int zeroByte;
  const struct AnimationFrame *oamData;
  u8 *animationFrameByte;
  int zeroHalf;
  struct PrimarySpriteData *roomSprite;
  struct PrimarySpriteData *parentCopy;
  u8 *rightPtrTemp;
  struct PrimarySpriteData *workSprite;
  int slot;
  int rightAddress;
  sprite = &gCurrentSprite;
  persistentZero = 0;
  oldStatus = sprite->status;
  mask = SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND;
  status = mask;
  zeroByte = persistentZero;
  zeroHalf = persistentZero;
  status |= oldStatus;
  status &= ~SPRITE_STATUS_HIDDEN;
  sprite->status = status;
  sprite->warioCollision = 62;
  {
    u8 *distance;
    int value;
    distance = &sprite->drawDistanceDown;
    value = 64;
    *distance = value;
  }
  {
    u8 *field;
    int sixteen;
    field = &sprite->drawDistanceUp;
    *field = zeroByte;
    field++;
    sixteen = 16;
    *field = sixteen;
    {
      int four;
      u8 *hitbox;
      hitbox = (u8 *)sprite + 35;
      four = 4;
      *hitbox = four;
    }
    field = &sprite->hitboxExtentDown;
    animationFramePtr = &sprite->currentAnimationFrame;
    *field = zeroByte;
    {
      int address;
      int savedAddress;
      int value;
      u8 *left;
      address = 37;
      address += (int) sprite;
      savedAddress = address;
      value = 32;
      left = (u8 *) savedAddress;
      *left = value;
    }
    {
      int address;
      int savedAddress;
      int value;
      u8 *right;
      address = 38;
      address += (int) sprite;
      rightAddress = address;
      savedAddress = rightAddress;
      rightPtrTemp = (u8 *) savedAddress;
      value = 48;
      right = rightPtrTemp;
      *right = value;
    }
    oamData = sCatbatOam_83CF370;
    animationFrameByte = (u8 *) animationFramePtr;
    GP_SET(sprite->pOamData, oamData);
    *animationFrameByte = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->pose = sixteen;
  }
  {
    int roomBit;
    int healthBit;
    s8 healthSeed;
    healthSeed = 1;
    healthBit = healthSeed;
    roomBit = 1;
    sprite->drawPriority = roomBit;
    {
      u8 *work1;
      u8 *work2;
      work1 = &sprite->work1;
      *work1 = zeroByte;
      workSprite = sprite;
      work2 = &workSprite->work2;
      *work2 = 42;
      work1 = &sprite->work3;
      *work1 = zeroByte;
    }
    SpriteSpawnAsChild(PSPRITE_CATBAT_DEBRIS_PARTICLE, sprite->roomSlot, persistentZero, sprite->yPosition, sprite->xPosition);
    slot = SpriteUtilFindSpriteSlot(PSPRITE_CATBAT);
    {
      struct PrimarySpriteData *sprites;
      int offset;
      struct PrimarySpriteData *parent;
      unsigned parentHealth;
      zeroHalf = 0;
      sprites = gSpriteData;
      offset = slot * sizeof(struct PrimarySpriteData);
      sprites = (struct PrimarySpriteData *) (((u8 *) sprites) + offset);
      parent = sprites;
      parentCopy = parent;
      parentHealth = parentCopy->health;
      if (parentHealth <= 5)
      {
        sprite->health |= 16;
      }
      parentHealth = parentCopy->health;
      if (parentHealth <= 9)
      {
        do
        {
          if ((parentHealth <= 7) || (((roomSprite = sprite)->roomSlot & 1) != zeroHalf))
          {
            sprite->health |= healthBit;
          }
        }
        while ((status = 0));
      }
    }
  }
}
void UpdateCatbatLandingDebris(void)
{
    struct PrimarySpriteData *initialSprite;
    u8 *timerPtr;
    u32 timerValue;

    initialSprite = &gCurrentSprite;
    timerPtr = &initialSprite->work2;
    timerValue = *timerPtr;
    if (timerValue != 0) {
        u32 timerByte;
        timerValue--;
        *timerPtr = timerValue;
        timerByte = (u8)timerValue;
        if (timerByte != 0) {
            switch (initialSprite->animationTimer - 1) {
                case 0:
                    gCurrentSprite.hitboxExtentUp = 40;
                    break;
                case 1:
                    gCurrentSprite.hitboxExtentUp = 72;
                    break;
                case 2:
                    gCurrentSprite.hitboxExtentUp = 104;
                    break;
                case 3:
                    gCurrentSprite.hitboxExtentUp = 136;
                    break;
                case 4:
                    gCurrentSprite.hitboxExtentUp = 168;
                    break;
                case 5:
                    gCurrentSprite.hitboxExtentUp = 200;
                    break;
                case 6:
                    gCurrentSprite.hitboxExtentUp = 232;
                    break;
                default:
                    initialSprite->hitboxExtentUp = 16;
                    break;
            }
        } else {
            GP_SET(initialSprite->pOamData, sCatbatOam_83CF3F0);
            initialSprite->currentAnimationFrame = timerByte;
            initialSprite->animationTimer = timerByte;
            m4aSongNumStart(SOUND_B1);
        }
    }

    {
        struct PrimarySpriteData *sprite;
        u16 firstStatus;
        register int onscreen PIN("r0");
        register u16 status PIN("r3");
        struct PrimarySpriteData *collisionSprite;

        sprite = &gCurrentSprite;
        firstStatus = sprite->status;
        onscreen = SPRITE_STATUS_ONSCREEN;
        onscreen &= firstStatus;
        onscreen = ((u32)onscreen << 16) >> 16;
        if (onscreen == 0)
            sprite->status = onscreen;

        {
            u32 collisionY;
            register u32 collisionX PIN("r1");
            status = sprite->status;
            if (status & SPRITE_STATUS_FACING_RIGHT) {
                if (sprite->health & 0xF0) {
                    sprite->xPosition += 6;
                    if (status & SPRITE_STATUS_WARIO_STANDING_ON)
                        gWarioData.xPosition += 6;
                } else {
                    sprite->xPosition += 4;
                    if (status & SPRITE_STATUS_WARIO_STANDING_ON)
                        gWarioData.xPosition += 4;
                }
                collisionSprite = &gCurrentSprite;
                collisionY = collisionSprite->yPosition - 128;
                collisionY <<= 16;
                collisionY >>= 16;
                collisionX = collisionSprite->xPosition + 128;
            } else {
                if (sprite->health & 0xF0) {
                    sprite->xPosition -= 6;
                    if (status & SPRITE_STATUS_WARIO_STANDING_ON)
                        gWarioData.xPosition -= 6;
                } else {
                    sprite->xPosition -= 4;
                    if (status & SPRITE_STATUS_WARIO_STANDING_ON)
                        gWarioData.xPosition -= 4;
                }
                collisionSprite = &gCurrentSprite;
                collisionY = collisionSprite->yPosition - 128;
                collisionY <<= 16;
                collisionY >>= 16;
                collisionX = collisionSprite->xPosition - 128;
            }
            collisionX = (u16)collisionX;
            ((void (*)(int, int))func_8023BFC)(collisionY, collisionX);
        }

        if (gSpriteCollisionTileType == 17) {
            collisionSprite->disableWarioCollisionTimer = 1;
            collisionSprite->status ^= SPRITE_STATUS_HIDDEN;
        }
    }

    {
        struct PrimarySpriteData *sprite;
        register u8 health PIN("r1");
        register u8 *indexPtr PIN("r12");
        int index;
        register const s16 *table PIN("r5");
        int entryOffset;
        const s16 *entry;
        register u16 velocity PIN("r4");
        int zero;
        int signedVelocity;
        int nextIndex;
        register u8 *storePtr PIN("r1");

        sprite = &gCurrentSprite;
        if (sprite->work2 == 0) {
            health = sprite->health;
            if (health & 1) {
                if (health & 0xF0) {
                    int pointerAddress;
                    u8 *pointer;
                    pointerAddress = 42;
                    ASM_BARRIER(: "+r"(pointerAddress));
                    pointerAddress += (int)sprite;
                    pointer = (u8 *)pointerAddress;
                    indexPtr = pointer;
                    index = *pointer;
                    table = sUnk_83CFA7C;
                } else {
                    int pointerAddress;
                    register u8 *pointer PIN("r6");
                    pointerAddress = 42;
                    ASM_BARRIER(: "+r"(pointerAddress));
                    pointerAddress += (int)sprite;
                    pointer = (u8 *)pointerAddress;
                    indexPtr = pointer;
                    index = *pointer;
                    table = sUnk_83CFA0A;
                }
                entryOffset = index << 1;
                entryOffset += (int)table;
                entry = (const s16 *)entryOffset;
                velocity = *(const u16 *)entry;
                zero = 0;
                signedVelocity = *(s16 *)((u8 *)entry + zero);
                if (signedVelocity == 0x7FFF) {
                    velocity = *(const u16 *)table;
                    index = 0;
                }
                nextIndex = index + 1;
                storePtr = indexPtr;
                *storePtr = nextIndex;
                sprite->yPosition += velocity;
            }
        }
    }
}
void InitCatbatDebrisParticle(void)
{
    register struct PrimarySpriteData *sprite PIN("r12");
    u16 oldStatus;
    register u16 status PIN("r0");
    int mask;
    register int zeroByte PIN("r2");
    int zeroHalf;
    u8 *largeExtent;
    u8 *smallExtent;
    int extentValue;
    const struct AnimationFrame *oam;
    register struct PrimarySpriteData *base PIN("r1");

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    mask = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    status = mask;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    base = sprite;
    base->status = status;
    base->warioCollision = zeroByte;

    largeExtent = &base->drawDistanceDown;
    status = 32;
    *largeExtent = status;
    largeExtent++;
    *largeExtent = status;
    largeExtent++;
    *(volatile u8 *)largeExtent = status;

    smallExtent = &sprite->hitboxExtentUp;
    extentValue = 4;
    *smallExtent = extentValue;
    smallExtent++;
    *smallExtent = extentValue;
    smallExtent++;
    *smallExtent = extentValue;
    smallExtent++;
    *(volatile u8 *)smallExtent = extentValue;

    oam = sCatbatOam_83CF400;
    base = sprite;
    GP_SET(base->pOamData, oam);
    base->currentAnimationFrame = zeroByte;
    base->animationTimer = zeroHalf;
    base->pose = 16;
    base->drawPriority = 1;
}
void UpdateCatbatDebrisParticle(void)
{
    int slot;
    register struct PrimarySpriteData *base;
    struct PrimarySpriteData *parent;
    struct PrimarySpriteData *sprite;
    int offset;
    u16 status;
    int exists;
    int value;

    slot = SpriteUtilFindParentSlotOrU8Max(PSPRITE_CATBAT_LANDING_DEBRIS);
    if (slot == 0xFF)
        goto clear;

    base = gSpriteData;
    offset = sizeof(struct PrimarySpriteData);
    offset *= slot;
    /* agbcc reverses this commutative address addition. */
    slot = offset - -(int)base;
    parent = (struct PrimarySpriteData *)slot;
    status = parent->status;
    exists = SPRITE_STATUS_EXISTS;
    exists &= status;
    if (exists != 0)
        goto copy_position;

clear:
    sprite = &gCurrentSprite;
    value = 0;
    sprite->status = value;
    return;

copy_position:
    sprite = &gCurrentSprite;
    value = parent->xPosition;
    sprite->xPosition = value;
}
void InitCatbatProjectile(void)
{
    struct PrimarySpriteData *sprite;
    u16 oldStatus;
    register u16 status PIN("r0");
    int mask;
    register int zeroByte PIN("r2");
    int zeroHalf;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    mask = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    status = mask;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    sprite->status = status;
    sprite->warioCollision = zeroByte;

    {
        u8 *distance;
        int distanceValue;

        distance = &sprite->drawDistanceDown;
        distanceValue = 48;
        *distance = distanceValue;
        distance++;
        *distance = distanceValue;
        distance++;
        *distance = distanceValue;
    }
    {
        u8 *extent;
        register int extentValue PIN("r1");

        extent = &sprite->hitboxExtentUp;
        extentValue = 4;
        *extent = extentValue;
        extent++;
        *extent = extentValue;
        extent++;
        *extent = extentValue;
        extent++;
        *extent = extentValue;
    }

    GP_SET(sprite->pOamData, sCatbatOam_83CD39C);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->pose = 16;
    sprite->work0 = 54;
    sprite->drawPriority = 1;

    if ((s32)SpriteUtilCountSpriteType(PSPRITE_CATBAT_ATTACK_EFFECT) <= 5) {
        register u16 spriteStatus PIN("r1");
        int facingMask;
        int facingTemp;
        int facing;

        m4aSongNumStart(SOUND_B4);
        spriteStatus = sprite->status;
        facingMask = SPRITE_STATUS_FACING_RIGHT;
        facingTemp = facingMask;
        facingTemp &= spriteStatus;
        facing = (u16)facingTemp;
        if (facing != 0) {
            SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_ATTACK_EFFECT, sprite->roomSlot, 0,
                sprite->yPosition, sprite->xPosition, facingMask);
        } else {
            SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_ATTACK_EFFECT, sprite->roomSlot, 0,
                sprite->yPosition, sprite->xPosition, facing);
        }
    }
}
void UpdateCatbatProjectile(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    int value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    value = (u8)value;
    if (value == 0)
        sprite->status = value;
}
void InitCatbatAttackEffect(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *base;
    u16 oldStatus;
    register u16 status PIN("r0");
    int mask;
    register int zeroByte PIN("r2");
    int zeroHalf;
    u8 *extent;
    int sixteen;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    mask = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    status = mask;
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    status &= ~SPRITE_STATUS_HIDDEN;
    base = sprite;
    base->status = status;
    base->warioCollision = 40;

    extent = &sprite->drawDistanceDown;
    sixteen = 16;
    *extent = sixteen;
    extent++;
    *extent = sixteen;
    extent++;
    *extent = sixteen;
    extent++;
    *extent = sixteen;
    extent++;
    *extent = sixteen;
    extent++;
    *extent = sixteen;
    extent++;
    *extent = sixteen;

    GP_SET(base->pOamData, sCatbatOam_83CF478);
    base->currentAnimationFrame = zeroByte;
    base->animationTimer = zeroHalf;
    base->pose = sixteen;
    base->drawPriority = 1;
    sprite->work3 = zeroByte;
    if (gSpriteAiDynamicGraphicsTimer != 0)
        base->palette = 2;
}
void UpdateCatbatAttackEffect(void)
{
    struct PrimarySpriteData *base;
    int roomSlot;
    struct PrimarySpriteData *sprite;
    if (gTimerState == 11) {
        gCurrentSprite.pose = 108;
        gCurrentSprite.disableWarioCollisionTimer = 1;
        return;
    }

    base = &gCurrentSprite;
    roomSlot = base->roomSlot;
    sprite = base;
    switch (roomSlot) {
        case 1:
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                sprite->xPosition += 4;
            else
                sprite->xPosition -= 4;
            sprite->yPosition += 2;
            break;
        case 2:
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                sprite->xPosition += 2;
            else
                sprite->xPosition -= 2;
            sprite->yPosition += 3;
            break;
        case 3:
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                sprite->xPosition += 4;
            else
                sprite->xPosition -= 4;
            sprite->yPosition -= 1;
            break;
        case 4:
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                sprite->xPosition += 3;
            else
                sprite->xPosition -= 3;
            sprite->yPosition += 1;
            break;
        default:
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
                sprite->xPosition += 2;
            else
                sprite->xPosition -= 2;
            sprite->yPosition += 1;
            break;
    }

    {
        register struct PrimarySpriteData *motionSprite PIN("r5");
        u8 *indexPtr;
        int indexValue;
        register const s16 *velocityTable PIN("r6");
        int entryAddress;
        const s16 *entry;
        u32 velocityValue;
        int zero;
        int signedVelocity;
        int nextIndex;
        u8 *storePtr;

        motionSprite = sprite;
        indexPtr = &motionSprite->work3;
        indexValue = *indexPtr;
        velocityTable = sUnk_83CFACE;
        entryAddress = indexValue << 1;
        /* Preserve the target's table-address operand order. */
        entryAddress -= -(int)velocityTable;
        entry = (const s16 *)entryAddress;
        velocityValue = *(const u16 *)entry;
        zero = 0;
        signedVelocity = *(s16 *)((u8 *)entry + zero);
        if (signedVelocity == 0x7FFF) {
            velocityValue = *(const u16 *)&velocityTable[0];
            indexValue = 0;
        }
        nextIndex = indexValue + 1;
        storePtr = indexPtr;
        *storePtr = nextIndex;
        motionSprite->yPosition += velocityValue;
        func_8023BFC(motionSprite->yPosition, motionSprite->xPosition);
        if (gBackgroundCollisionData.waterType == 1) {
            motionSprite->pose = 108;
        } else {
            u16 status;
            register int existsMask PIN("r0");
            status = sprite->status;
            existsMask = 2;
            existsMask &= status;
            existsMask = ((u32)existsMask << 16) >> 16;
            if (existsMask == 0)
                sprite->status = existsMask;
        }
    }
}
void UpdateCatbatAttackEffectDefault(void)
{
    struct PrimarySpriteData *sprite;
    const s16 *table;
    int cosine;
    int sine;
    int scale;
    register int zeroByte PIN("r2");
    int zeroHalf;
    int affine;
    u16 status;

    sprite = &gCurrentSprite;
    status = sprite->status;
    affine = SPRITE_STATUS_AFFINE;
    zeroByte = 0;
    zeroHalf = 0;
    affine |= status;
    sprite->status = affine;
    GP_SET(sprite->pOamData, sCatbatOam_83CF4B8);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->pose = 109;
    sprite->work0 = 32;
    sprite->disableWarioCollisionTimer = 1;

    table = sSinCosTable;
    cosine = table[64];
    scale = 256;
    sprite->affinePA = FixedMul(cosine, FixedInverse(scale));
    sine = table[0];
    sprite->affinePB = FixedMul(sine, FixedInverse(scale));
    sprite->affinePC = FixedMul(-sine, FixedInverse(scale));
    sprite->affinePD = FixedMul(cosine, FixedInverse(scale));
}
void UpdateCatbatAttackEffectHit(void)
{
    struct PrimarySpriteData *sprite;
    int scale;
    const s16 *table;
    int cosine;
    int sine;
    u8 *timer;
    int value;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    value = (u8)value;
    if (value == 0) {
        sprite->status = value;
        return;
    }

    scale = *timer << 3;
    table = sSinCosTable;
    cosine = table[64];
    sprite->affinePA = FixedMul(cosine, FixedInverse(scale));
    sine = table[0];
    sprite->affinePB = FixedMul(sine, FixedInverse(scale));
    sprite->affinePC = FixedMul(-sine, FixedInverse(scale));
    sprite->affinePD = FixedMul(cosine, FixedInverse(scale));
}
void InitCatbatAttackEffectDespawn(void)
{
    int value;
    register struct PrimarySpriteData *sprite PIN("r1");
    int zero;
    register int zeroPalette PIN("r2");

    value = gSpriteAiDynamicGraphicsTimer;
    if (value != 0) {
        sprite = &gCurrentSprite;
        zero = 0;
        sprite->status = zero;
        SpawnSecondarySprite(sprite->yPosition, sprite->xPosition, 64);
    } else {
        sprite = &gCurrentSprite;
        GP_SET(sprite->pOamData, sCatbatOam_83CF8BC);
        sprite->currentAnimationFrame = value;
        zeroPalette = 0;
        sprite->animationTimer = value;
        sprite->pose = 50;
        sprite->work0 = 42;
        sprite->palette = zeroPalette;
    }
}
void UpdateCatbatAttackEffectDespawn(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    int value;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    value = (u8)value;
    if (value == 0)
        sprite->status = value;
}
void SpriteCatbatMineSpawner(void)
{
    u8 pose;

    pose = gCurrentSprite.pose;
    switch (pose) {
    case 0:
        InitCatbatMineSpawner();
        /* Fall through. */
    case 16:
        UpdateCatbatMineSpawnerPosition();
        break;
    }
}
void SpriteCatbat(void)
{
    if (*(vu8 *)&gCuckooCondorPendulumLength != 0) {
        gCurrentSprite.disableWarioCollisionTimer = 1;
        gCurrentSprite.status ^= 4;
    }

    if (gCuckooCondorHasCapturedWario != 0) {
        if (gCuckooCondorHasCapturedWario == 20 || gCuckooCondorHasCapturedWario == 1) {
            gCurrentSprite.work0 = 0;
            gCurrentSprite.work1 = 0;
        }
        if (gWarioData.reaction == 8)
            gCuckooCondorHasCapturedWario = 19;
        else
            gCuckooCondorHasCapturedWario--;
    }

    switch (gCurrentSprite.pose) {
        case 0: InitCatbat(); break;
        case 1: UpdateCatbatIntroWaitForShop(); break;
        case 2: UpdateCatbatIntroDelay(); break;
        case 3: UpdateCatbatIntroRoar(); break;
        case 4: UpdateCatbatIntroBossTimer(); break;
        case 5: UpdateCatbatIntroWaitForTimer(); break;
        case 15: InitCatbatPatrol(); break;
        case 16: UpdateCatbatPatrol(); break;
        case 17: InitCatbatLandingAttack();
        case 18: UpdateCatbatLandingAttack(); break;
        case 20: UpdateCatbatLandingRecovery(); break;
        case 21: InitCatbatIdleAttack();
        case 22: UpdateCatbatIdleAttack(); break;
        case 31: InitCatbatMoveRight();
        case 32: UpdateCatbatMoveRight(); break;
        case 33: InitCatbatMoveLeft();
        case 34: UpdateCatbatMoveLeft(); break;
        case 49: InitCatbatDefeat();
        case 50: UpdateCatbatDefeat(); break;
        case 108: InitCatbatDamaged();
        case 109: UpdateCatbatDamaged(); break;
        case 120: InitCatbatDeathExplosion();
        case 121: UpdateCatbatDeathExplosion(); break;
        case 110: InitCatbatProjectileAttack();
        case 111: UpdateCatbatProjectileAttackCharge(); break;
        case 112: InitCatbatProjectileAttackRelease();
        case 113: UpdateCatbatProjectileAttackRelease(); break;
        case 122: InitCatbatShopItemHit();
        case 123: UpdateCatbatShopItemHit(); break;
    }
    UpdateCatbatBackgroundAnimation(sUnk_83CF970);
}
void SpriteCatbatGroundWave(void)
{
    if (gTimerState == 11) {
        SpriteUtilDie();
        return;
    }

    switch (gCurrentSprite.pose) {
        case 0: InitCatbatGroundWave(); break;
        case 1: UpdateCatbatGroundWave(); break;
        case 15: InitCatbatGroundWaveReturn();
        case 16: UpdateCatbatGroundWaveReturn(); break;
        case 17: InitCatbatGroundWaveDisappear();
        case 18:
        case 20: UpdateCatbatGroundWaveDisappear(); break;
        case 35:
        case 37: UpdateCatbatGroundWaveTurn(); break;
        case 39:
        case 41: InitCatbatGroundWaveDelay();
        case 40: UpdateCatbatGroundWaveDelay(); break;
        default: SpriteUtilDie(); break;
    }
}
void SpriteCatbatLandingDebris(void)
{
    if (gCurrentSprite.pose == 0)
        InitCatbatLandingDebris();
    UpdateCatbatLandingDebris();
    func_8026838();
}
void SpriteCatbatDebrisParticle(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0)
        InitCatbatDebrisParticle();
    UpdateCatbatDebrisParticle();
}
void SpriteCatbatProjectile(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == 0)
        InitCatbatProjectile();
    UpdateCatbatProjectile();
}
void SpriteCatbatAttackEffect(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            InitCatbatAttackEffect();
        case 16:
            UpdateCatbatAttackEffect();
            break;
        case 49:
            InitCatbatAttackEffectDespawn();
        case 50:
            UpdateCatbatAttackEffectDespawn();
            break;
        case 109:
            UpdateCatbatAttackEffectHit();
            break;
        default:
            UpdateCatbatAttackEffectDefault();
            break;
    }
}

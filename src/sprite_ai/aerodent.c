#include "sprite_ai/aerodent.h"

#include "background_registers.h"
#include "color_effects.h"
#include "fixed_point.h"
#include "hud.h"
#include "main.h"
#include "score.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

s32 SpriteCollisionCheckObjectsTouching();
void SpriteUtilCheckCollisionAtPositionOld(u32, u32) asm("func_8023BFC");

void AerodentEmitterStartBurst(void);

int AerodentWeakPointCheckThrowableCollision(void)
{
    u32 thisBottom;
    u32 thisLeft;
    u32 thisRight;
    int slot;
    struct PrimarySpriteData *sprite;
    register u32 requiredStatus;
    struct PrimarySpriteData *current;
    register u32 center PIN("r2");
    register u32 yCenter PIN("r1");
    volatile u32 thisTop;

    current = &gCurrentSprite;
    {
        register u32 offset PIN("r0");

        offset = 0x80;
        offset <<= 5;
        /* Prevent agbcc from folding this materialized shift into the following copy. */
        ASM_BARRIER(: "+r"(offset));
        center = offset;
    }
    {
        u32 temp;

        temp = current->yPosition;
        yCenter = center + temp;
        yCenter <<= 16;
        yCenter >>= 16;
        temp = current->xPosition;
        center += temp;
        center <<= 16;
        temp = current->hitboxExtentUp;
        temp = yCenter - temp;
        temp <<= 16;
        temp >>= 16;
        thisTop = temp;
    }
    current = (struct PrimarySpriteData *)((u8 *)current + 36);
    yCenter += 34;
    current = (struct PrimarySpriteData *)(u32)*(u8 *)current;
    yCenter += (u32)current;
    yCenter = (u16)yCenter;
    thisBottom = yCenter;
    thisLeft = (center + 0xFFBA0000) >> 16;
    thisRight = (center + (240 << 13)) >> 16;

    slot = 0;
    requiredStatus = SPRITE_STATUS_EXISTS | SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
    do {
        {
            u32 temp0;
            register u32 temp1 PIN("r1");

            temp1 = (u32)gSpriteData;
            temp0 = sizeof(struct PrimarySpriteData);
            temp0 *= slot;
            sprite = (struct PrimarySpriteData *)(temp0 + temp1);
            temp1 = sprite->status;
            temp0 = requiredStatus;
            temp0 &= temp1;
            if (temp0 != requiredStatus)
                goto nextSlot;
        }
        if (sprite->globalID == PSPRITE_AERODENT_THROWABLE && sprite->work0 == 0) {
            u32 v1;
            u32 v2;
            u32 v3;
            u32 v4;

            v1 = 0x80;
            v1 <<= 5;
            /* Keep the shifted screen-center constant separate from its later copy. */
            ASM_BARRIER(: "+r"(v1));
            v2 = v1;
            {
                register u32 temp PIN("r0");

                temp = sprite->yPosition;
                v3 = v2 + temp;
                v3 <<= 16;
                v3 >>= 16;
            }
            v1 = sprite->xPosition;
            v2 += v1;
            v2 <<= 16;
            v2 >>= 16;
            {
                register u8 *extent PIN("r0");

                extent = &sprite->hitboxExtentUp;
                v4 = v3 - *extent;
                v4 <<= 16;
                v4 >>= 16;
                extent++;
                /* Preserve the explicit pointer increment instead of folding +1 into the load. */
                ASM_BARRIER(: "+r"(extent));
                v3 += *extent;
                v3 <<= 16;
                v3 >>= 16;
                extent = &sprite->hitboxExtentLeft;
                v1 = v2 - *extent;
                v1 <<= 16;
                v1 >>= 16;
                extent++;
                /* Preserve the explicit pointer increment instead of folding +1 into the load. */
                ASM_BARRIER(: "+r"(extent));
                v2 += *extent;
                v2 <<= 16;
                v2 >>= 16;
            }
            if (SpriteCollisionCheckObjectsTouching(
                    thisTop, thisBottom, thisLeft, thisRight,
                    v4, v3, v1, v2)) {
                sprite->pose = 110;
                sprite->status &= ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
                return TRUE;
            }
        }
nextSlot:
        slot++;
    } while (slot < MAX_SPRITE_COUNT);

    return FALSE;
}
int AerodentMoveUpWithinCamera(void)
{
    u16 *bg;
    u32 offset;
    u32 value;
    struct PrimarySpriteData *sprite;
    u32 adjust;
    u32 bgLimit;
    u32 spritePosition;
    u16 nextY;

    bg = &gBg1YPosition;
    offset = 128;
    offset <<= 1;
    /* agbcc otherwise coalesces the two live 0x100 values and removes the required register copy. */
    ASM_BARRIER(: "+r"(offset));
    value = offset;
    bgLimit = (value + *bg) << 16;
    sprite = &gCurrentSprite;
    offset = sprite->yPosition;
    value = (offset) - (-(value));
    spritePosition = value << 16;
    adjust = 0xFF200000;
    bgLimit += adjust;
    if (spritePosition > bgLimit) {
        nextY = offset - 1;
        sprite->yPosition = nextY;
        return FALSE;
    }
    return TRUE;
}
void AerodentWeakPointUpdateHitbox(void)
{
    struct PrimarySpriteData *sprite;
    unsigned int state;
    u8 *hitbox;

    gCurrentSprite.warioCollision = 0;
    state = gCurrentSprite.work1;
    sprite = &gCurrentSprite;
    switch (state) {
    case 0:
    case 1:
    case 2:
        hitbox = &sprite->hitboxExtentUp;
        *hitbox = 128;
        hitbox++;
        *hitbox = 8;
        hitbox++;
        *hitbox = 96;
        hitbox++;
        *hitbox = 92;
        break;
    case 3:
    case 4:
        hitbox = &sprite->hitboxExtentUp;
        *hitbox = 128;
        hitbox++;
        *hitbox = 16;
        hitbox++;
        *hitbox = 112;
        hitbox++;
        *hitbox = 108;
        break;
    case 5:
    case 6:
        {
            u8 *up;
            u8 *down;
            register int wide PIN("r2");

            up = &sprite->hitboxExtentUp;
            wide = 128;
            *up = wide;
            down = &sprite->hitboxExtentDown;
            *down = 20;
            up = &sprite->hitboxExtentLeft;
            *up = wide;
            down += 2;
            *down = 124;
        }
        break;
    case 8:
        {
            struct PrimarySpriteData *sprites;
            struct PrimarySpriteData *parent;
            int index;
            int stride;

            sprites = gSpriteData;
            index = sprite->work3;
            stride = sizeof(struct PrimarySpriteData);
            index *= stride;
            parent = (struct PrimarySpriteData *)(index + (u32)sprites);
            if (parent->status & 0x20) {
                int zero;
                u8 *upper;
                u8 *lower;

                zero = 0;
                sprite->warioCollision = stride;
                upper = &sprite->hitboxExtentUp;
                *upper = 128;
                lower = &sprite->hitboxExtentDown;
                *lower = zero;
                {
                    u8 *left;

                    left = &sprite->hitboxExtentLeft;
                    *left = 144;
                    left++;
                    *left = 140;
                }
                break;
            }
            {
                u8 *parentWork2;

                parentWork2 = (u8 *)parent;
                parentWork2 += 41;
                if (*parentWork2 != 0)
                    sprite->warioCollision = stride;
                else
                    sprite->warioCollision = 45;
            }
        }
        /* fallthrough */
    case 7:
        hitbox = &sprite->hitboxExtentUp;
        *hitbox = 128;
        hitbox++;
        *hitbox = 24;
        hitbox++;
        *hitbox = 144;
        hitbox++;
        *hitbox = 140;
        break;
    case 9:
    case 10:
        sprite->warioCollision = 44;
        hitbox = &sprite->hitboxExtentUp;
        *hitbox = 128;
        hitbox++;
        *hitbox = 32;
        hitbox++;
        *hitbox = 160;
        hitbox++;
        *hitbox = 156;
        break;
    case 11:
    case 12:
    case 13:
        sprite->warioCollision = 44;
        hitbox = &sprite->hitboxExtentUp;
        *hitbox = 128;
        hitbox++;
        *hitbox = 40;
        hitbox++;
        *hitbox = 176;
        hitbox++;
        *hitbox = 172;
        break;
    default:
        gCurrentSprite.warioCollision = 44;
        hitbox = &gCurrentSprite.hitboxExtentUp;
        *hitbox = 128;
        hitbox++;
        *hitbox = 16;
        hitbox++;
        *hitbox = 192;
        hitbox++;
        *hitbox = 188;
        break;
    }
}
void AerodentLoadNormalGraphics(void)
{
    DmaCopy16(3, sUnk_83DB610, (void *)0x06015800, 0x100);
    DmaCopy16(3, sUnk_83DB610 + 128, (void *)0x06015C00, 0x100);
}
void AerodentLoadFlashGraphics(void)
{
    DmaCopy16(3, sUnk_83DB810, (void *)0x06015800, 0x100);
    DmaCopy16(3, sUnk_83DB810 + 128, (void *)0x06015C00, 0x100);
}
void AerodentUpdateDamageGraphics(void)
{
    if (GP(gCurrentSprite.pOamData) == sAerodentOam_83DABB0 && gCurrentSprite.currentAnimationFrame == 1) {
        if (gCurrentSprite.animationTimer == 0)
            AerodentLoadFlashGraphics();
        else
            AerodentLoadNormalGraphics();
    }
}
void AerodentUpdateInflation(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work2;
    unsigned int value;
    int bit;

    AerodentUpdateDamageGraphics();
    sprite = &gCurrentSprite;
    work2 = &sprite->work2;
    value = *work2;
    if (value == 0)
        return;

    bit = 0x80;
    bit &= value;
    if (bit != 0) {
        int zero;
        int next;
        int masked;

        next = sprite->yPosition + 2;
        zero = 0;
        sprite->yPosition = next;
        next = value - 1;
        *work2 = next;
        masked = 0x7F;
        masked &= next;
        if (masked != 0)
            return;

        {
            u8 *work1;

            work1 = &sprite->work1;
            value = *work1;
            if (value <= 8) {
                *work2 = zero;
                return;
            }
            *work2 = 148;
            next = value - 1;
            *work1 = next;
            next = (u8)next;
            if (next == 8) {
                *work2 = zero;
                GP_SET(sprite->pOamData, sAerodentOam_83DABB0);
                sprite->currentAnimationFrame = zero;
                sprite->animationTimer = masked;
            }
        }
    } else {
        int collision;
        int next;
        int normalized;

        collision = AerodentMoveUpWithinCamera();
        next = *work2 - 1;
        *work2 = next;
        normalized = (u8)next;
        if (normalized == 0) {
            u8 *work1;
            unsigned int frame;

            *work2 = 20;
            work1 = &sprite->work1;
            frame = *work1;
            if (frame > 15)
                goto check_collision;
            frame++;
            *work1 = frame;
            frame = (u8)frame;
            if (frame == 16) {
                GP_SET(sprite->pOamData, sAerodentOam_83DABA0);
                sprite->currentAnimationFrame = normalized;
                sprite->animationTimer = normalized;
            }
        }
        if (gCurrentSprite.work1 <= 15)
            return;
check_collision:
        if (collision == 0)
            return;
        gCurrentSprite.work2 = 0;
    }
}
void AerodentInit(void)
{
    struct PrimarySpriteData *sprite;
    int zeroHalf;

    sprite = &gCurrentSprite;
    {
        int priority;
        int mask;

        priority = sprite->drawPriority;
        mask = 0x80;
        zeroHalf = 0;
        mask |= priority;
        sprite->drawPriority = mask;
    }
    {
        int status;
        register int bits PIN("r2");
        register int value PIN("r0");

        status = sprite->status;
        bits = 0xC488;
        /* Preserve the literal in r2 before copying it to the status accumulator. */
        ASM_BARRIER(: "+r"(bits));
        value = bits;
        bits = 0;
        value |= status;
        sprite->status = value;

        {
            u8 *draw;
            int distance;

            draw = &sprite->drawDistanceDown;
            distance = 64;
            *draw = distance;
            draw++;
            *draw = distance;
            draw++;
            *draw = distance;
        }
        {
            u8 *hitbox;
            register int extent PIN("r1");

            hitbox = &sprite->hitboxExtentUp;
            extent = 4;
            *hitbox = extent;
            hitbox++;
            *hitbox = extent;
            hitbox++;
            *hitbox = extent;
            hitbox++;
            *hitbox = extent;
        }

        GP_SET(sprite->pOamData, sAerodentOam_83DABA0);
        *(u8 *)((u8 *)sprite + 22) = bits;
        sprite->animationTimer = zeroHalf;
        {
            u8 *work;

            work = &sprite->work0;
            *work = bits;
            work++;
            *work = bits;
            work++;
            *work = bits;
            work++;
            *work = bits;
        }
        sprite->warioCollision = bits;
    }
    sprite->pose = 1;
    {
        register int offset PIN("r1");
        register int nextY PIN("r0");
        register int oldY PIN("r2");

        /* Keep -0x100 as the original literal-pool load rather than synthesized shifts. */
        ASM_BARRIER(: "=r"(offset) : "0"(-256));
        nextY = offset;
        /* Prevent the copy from folding into the following three-operand add. */
        ASM_BARRIER(: "+r"(nextY));
        oldY = sprite->yPosition;
        nextY += oldY;
        sprite->yPosition = nextY;
    }
    sprite->xPosition -= 96;
    SpriteUtilTurnTowardWario();
    sprite->health = 16;
    UpdateBossHealthGauge();
    SpawnHighPriorityPrimarySprite(203, sprite->roomSlot, sprite->gfxSlot,
        sprite->yPosition - 64, sprite->xPosition + 224);
    SpawnHighPriorityPrimarySprite(204, sprite->roomSlot, sprite->gfxSlot,
        sprite->yPosition + 240, sprite->xPosition + 128);
    LoadBossSpriteGraphics(131, 8, 4);
}
void AerodentEnterArena(void)
{
    s16 velocity;
    u8 index;

    index = gCurrentSprite.work3;
    velocity = sUnk_83DBA70[index];
    if (velocity == 0x7FFF) {
        velocity = sUnk_83DBA70[0];
        index = 0;
    }
    gCurrentSprite.work3 = index + 1;
    gCurrentSprite.yPosition += velocity;
}
void AerodentIdle(void)
{
    s16 velocity;
    u8 index;

    if (gCurrentSprite.work2 != 0)
        return;

    gCurrentSprite.drawPriority &= 0x7F;
    index = gCurrentSprite.work3;
    velocity = sUnk_83DBA70[index];
    if (velocity == 0x7FFF) {
        velocity = sUnk_83DBA70[0];
        index = 0;
    }
    gCurrentSprite.work3 = index + 1;
    gCurrentSprite.yPosition += velocity;
    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON)
        gCurrentSprite.xPosition++;
    else
        gCurrentSprite.xPosition--;
}
void AerodentStartMoveRight(void)
{
    gCurrentSprite.pose = 36;
    gCurrentSprite.work0 = 60;
    gCurrentSprite.status |= SPRITE_STATUS_WARIO_STANDING_ON;
    m4aSongNumStart(SOUND_92);
}
void AerodentMoveRight(void)
{
    if (!(gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON)) {
        gCurrentSprite.pose = 37;
        return;
    }

    gCurrentSprite.xPosition += 2;
    AerodentMoveUpWithinCamera();
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0)
        gCurrentSprite.pose = 16;
}
void AerodentStartMoveLeft(void)
{
    gCurrentSprite.pose = 38;
    gCurrentSprite.work0 = 60;
    gCurrentSprite.status &= ~SPRITE_STATUS_WARIO_STANDING_ON;
    m4aSongNumStart(SOUND_92);
}
void AerodentMoveLeft(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON) {
        gCurrentSprite.pose = 35;
        return;
    }

    gCurrentSprite.xPosition -= 2;
    AerodentMoveUpWithinCamera();
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0)
        gCurrentSprite.pose = 16;
}
void AerodentStartDeflate(void)
{
    gCurrentSprite.pose = 32;
    gCurrentSprite.work0 = 255;
    gCurrentSprite.status |= SPRITE_STATUS_WARIO_STANDING_ON;
    m4aSongNumStart(SOUND_91);
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DABA0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    AerodentLoadNormalGraphics();
}
void AerodentDeflate(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON)
        gCurrentSprite.xPosition += 2;
    else
        gCurrentSprite.xPosition -= 2;

    if (gCurrentSprite.work0 <= 128) {
        gCurrentSprite.status |= SPRITE_STATUS_HEAVY;
        gCurrentSprite.pose = 16;
    } else {
        gCurrentSprite.work0 -= 2;
    }
}
void AerodentStartInflate(void)
{
    gCurrentSprite.pose = 34;
    gCurrentSprite.work0 = 1;
    gCurrentSprite.status &= ~SPRITE_STATUS_WARIO_STANDING_ON;
    m4aSongNumStart(SOUND_91);
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DABA0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    AerodentLoadNormalGraphics();
}
void AerodentInflate(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_WARIO_STANDING_ON)
        gCurrentSprite.xPosition += 2;
    else
        gCurrentSprite.xPosition -= 2;

    if ((s8)gCurrentSprite.work0 < 0) {
        gCurrentSprite.status |= SPRITE_STATUS_HEAVY;
        gCurrentSprite.pose = 16;
    } else {
        gCurrentSprite.work0 += 2;
    }
}
void AerodentUpdateRotation(void)
{
    struct PrimarySpriteData *sprite;
    register u16 status PIN("r1");

    sprite = &gCurrentSprite;
    status = sprite->status;
    if (status & 0x1000) {
        u8 *timer;
        int value;

        timer = &sprite->work0;
        value = *timer - 2;
        *timer = value;
        if ((u8)value <= 2) {
            sprite->status = status & ~0x20;
            sprite->pose = 16;
            *timer = 0;
            sprite->work2 = 16;
        }
    } else {
        u8 *timer;
        int value;

        timer = &sprite->work0;
        value = *timer + 2;
        *timer = value;
        if ((u8)value > 253) {
            sprite->status = status & ~0x20;
            sprite->pose = 16;
            *timer = 0;
            sprite->work1 = 9;
            sprite->work2 = 16;
        }
    }
}
void AerodentStartDefeat(void)
{
    gCurrentSprite.pose = 48;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DABC8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 100;
    gCurrentSprite.work2 = 0;
}
void AerodentDefeat(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *current;
    u8 *timerPtr;
    int timer;
    u8 timer8;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    timer = *timerPtr - 1;
    *timerPtr = timer;
    timer8 = (u8)timer;
    current = sprite;
    if (timer8 != 0) {
        u8 *counter;
        int counterValue;
        register struct PrimarySpriteData *base PIN("r2");
        int value;

        /* Keep the second byte cast distinct from the zero test above. */
        ASM_BARRIER(: "+r"(timer));
        if ((u8)timer > 49)
            return;
        if (timer & 4) {
            counter = &sprite->work1;
            counterValue = *counter;
            if (counterValue != 0) {
                counterValue--;
                *counter = counterValue;
            }
        }

        base = current;
        value = base->work0;
        if (value == 20)
            SpawnHighPriorityPrimarySprite(10, base->roomSlot, 0, base->yPosition + 160, base->xPosition + 120);
        else if (value == 15)
            SpawnHighPriorityPrimarySprite(10, base->roomSlot, 0, base->yPosition + 200, base->xPosition + 80);
        else if (value == 10)
            SpawnHighPriorityPrimarySprite(10, base->roomSlot, 0, base->yPosition + 130, base->xPosition + 120);
        else if (value == 5)
            SpawnHighPriorityPrimarySprite(10, current->roomSlot, 0, current->yPosition + 170, current->xPosition + 90);
    } else {
        sprite->status = timer8;
        SpawnHighPriorityPrimarySprite(10, sprite->roomSlot, 0, sprite->yPosition + 210, sprite->xPosition + 110);
        SpawnHighPriorityPrimarySprite(207, sprite->roomSlot, 0, sprite->yPosition + 110, sprite->xPosition + 110);
    }
}
void AerodentSpawnShot(void)
{
    u16 direction;

    direction = gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT;
    if (direction != 0) {
        SpawnPrimarySpriteWithStatus(206, 0, gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition, gCurrentSprite.xPosition + 56,
            SPRITE_STATUS_FACING_RIGHT);
    } else {
        SpawnPrimarySpriteWithStatus(206, 0, gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition, gCurrentSprite.xPosition - 56, direction);
    }
    m4aSongNumStart(SOUND_90);
}
void AerodentCompanionInit(void)
{
    gCurrentSprite.drawPriority |= 0x80;
    gCurrentSprite.status = (gCurrentSprite.status & 0xFFFB) | 0x8408;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 32;
    gCurrentSprite.hitboxExtentDown = 24;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    gCurrentSprite.warioCollision = 0;
    SpriteUtilTurnTowardWario();
    SpriteUtilFindSpriteSlotWork3(105);
    gCurrentSprite.health = 16;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB0B0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 15;
    gCurrentSprite.work0 = 60;
}
void AerodentCompanionStartIdle(void)
{
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 100;
    gCurrentSprite.work1 = 0;
    m4aSongNumStart(SOUND_CF);
}
void AerodentCompanionWaitForBattle(void)
{
    if (gCurrentSprite.work0 != 0) {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 != 0)
            return;

        *(volatile u8 *)&gBossDefeatTimer = 7;
    }
    if (gBossDefeatTimer[0] == 0 && gCurrentShopItem == 0)
        AerodentCompanionStartIdle();
}
void AerodentCompanionDescend(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 <= 40) {
        gCurrentSprite.xPosition -= 2;
        gCurrentSprite.yPosition += 2;
        if (gCurrentSprite.work0 == 0)
            gCurrentSprite.pose = 17;
    }
}
void AerodentCompanionStartAttach(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DADA8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.status &= ~SPRITE_STATUS_BACKGROUND;
    gCurrentSprite.work0 = 30;
}
void AerodentCompanionFollowParent(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u8 *slot;

    current = &gCurrentSprite;
    sprites = gSpriteData;
    slot = &current->work3;
    current->xPosition = sprites[*slot].xPosition + 140;
    current->yPosition = sprites[*slot].yPosition;
    current->work0--;
    if (current->work0 == 0)
        current->pose = 23;
}
void AerodentCompanionStartFightCountdown(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DACC0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 60;
}
void AerodentCompanionBeginBossFight(void)
{
    u8 timer;

    gCurrentSprite.xPosition = gSpriteData[gCurrentSprite.work3].xPosition + 140;
    gCurrentSprite.yPosition = gSpriteData[gCurrentSprite.work3].yPosition;
    timer = --gCurrentSprite.work0;
    if (timer == 0) {
        SpriteUtilStartBossTimer();
        gCurrentSprite.pose = 19;
        gSpriteData[gCurrentSprite.work3].pose = 16;
        GP_SET(gSpriteData[gCurrentSprite.work3].pOamData, sAerodentOam_83DAB28);
        gSpriteData[gCurrentSprite.work3].currentAnimationFrame = 0;
        gSpriteData[gCurrentSprite.work3].animationTimer = timer;
        gSpriteData[gCurrentSprite.work3].work2 = 20;
    }
}
void AerodentCompanionStartRise(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DAD28);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 20;
    gCurrentSprite.work0 = 20;
}
void AerodentCompanionRise(void)
{
    {
        struct PrimarySpriteData *sprite;
        u8 *timer;
        u8 *frame;
        int value;

        sprite = &gCurrentSprite;
        if ((*(u32 *)&sprite->animationTimer & 0xFFFFFF) == 0x20000)
            m4aSongNumStart(SOUND_8D);

        timer = &sprite->work0;
        value = *timer - 1;
        *timer = value;
        if ((value << 24) == 0) {
            *timer = 20;
            frame = &sprite->work1;
            value = *frame + 1;
            *frame = value;
            if ((u8)value > 15) {
                sprite->pose = 21;
                *timer = 4;
            }
        }
    }

    {
        struct PrimarySpriteData *current;
        struct PrimarySpriteData *sprites;
        u8 *parentSlot;
        int offset;
        int stride;
        const u16 *heights;

        current = &gCurrentSprite;
        sprites = gSpriteData;
        parentSlot = &current->work3;
        offset = *parentSlot;
        stride = sizeof(struct PrimarySpriteData);
        offset *= stride;
        offset += (u32)sprites;
        current->xPosition = ((struct PrimarySpriteData *)offset)->xPosition + 140;
        offset = *parentSlot;
        stride *= offset;
        stride += (u32)sprites;
        heights = sUnk_83DBA34;
        current->yPosition = ((struct PrimarySpriteData *)stride)->yPosition - heights[current->work1];
    }
}
void AerodentCompanionEnterCombat(void)
{
    gCurrentSprite.drawPriority &= 0x7F;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DACC0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 22;
}
void AerodentCompanionUpdateCombat(void)
{
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *sprites;
    u8 *parentSlot;
    int stride;
    int zero;

    sprite = &gCurrentSprite;
    sprites = gSpriteData;
    parentSlot = &sprite->work3;
    {
        int parentIndex;

        parentIndex = *parentSlot;
        stride = sizeof(struct PrimarySpriteData);
        parentIndex *= stride;
        parentIndex += (u32)sprites;
        sprite->xPosition = ((struct PrimarySpriteData *)parentIndex)->xPosition + 140;
    }
    zero = 0;
    {
        u8 *work1;
        int pose;
        u8 *slotCopy;
        int parentIndex;

        slotCopy = parentSlot;
        parentIndex = *slotCopy;
        parentIndex *= stride;
        parentIndex += (u32)sprites;
        parentIndex = *(u8 *)(parentIndex + 40);
        work1 = &sprite->work1;
        *work1 = parentIndex;

        parentIndex = *(volatile u8 *)slotCopy;
        {
            int parentOffset;

            parentOffset = parentIndex;
            parentOffset *= stride;
            parentOffset += (u32)sprites;
            sprite->yPosition = ((struct PrimarySpriteData *)parentOffset)->yPosition - sUnk_83DBA34[*work1];
        }
        slotCopy = parentSlot;
        parentIndex = *slotCopy;
        parentIndex *= stride;
        parentIndex += (u32)sprites;
        pose = ((struct PrimarySpriteData *)parentIndex)->pose;
        if (pose == 32 || pose == 34) {
            GP_SET(sprite->pOamData, sAerodentOam_83DAD68);
            sprite->currentAnimationFrame = zero;
            sprite->animationTimer = 0;
            sprite->work0 = 10;
            sprite->work2 = zero;
            sprite->pose = 115;
        }
    }

    {
        register struct PrimarySpriteData *current PIN("r4");
        register u8 *slot PIN("r12");
        int parentStride;

        current = sprite;
        slot = &current->work3;
        {
            int parentIndex;

            parentIndex = *slot;
            parentStride = sizeof(struct PrimarySpriteData);
            parentIndex *= parentStride;
            parentIndex += (u32)sprites;
            if (((struct PrimarySpriteData *)parentIndex)->work2 != 0)
                return;
        }

        if (current->work1 != 16) {
            u8 *timer;
            register int oldValue PIN("r0");
            register int value PIN("r1");

            timer = &current->work0;
            oldValue = *timer;
            value = oldValue - 1;
            *timer = value;
            oldValue = value << 24;
            {
                int normalized;

                normalized = (u32)oldValue >> 24;
                if (normalized != 0) {
                    int status;
                    int facing;

                    if (current->health > 12)
                        return;
                    normalized = 63;
                    normalized &= value;
                    if (normalized != 0)
                        return;
                    status = current->status;
                    facing = SPRITE_STATUS_FACING_RIGHT;
                    status ^= facing;
                    value = 0;
                    current->status = status;
                    GP_SET(current->pOamData, sAerodentOam_83DAF00);
                    current->currentAnimationFrame = value;
                    current->animationTimer = normalized;
                    AerodentSpawnShot();
                    return;
                } else {
                    u8 *slotCopy;
                    register int index PIN("r0");

                    slotCopy = slot;
                    index = *slotCopy;
                    index *= parentStride;
                    {
                        int fieldBase;

                        fieldBase = (u32)sprites + 4;
                        index += fieldBase;
                        GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)index, sAerodentOam_83DAB28);
                    }
                    slotCopy = slot;
                    index = *slotCopy;
                    index *= parentStride;
                    index += (u32)sprites;
                    ((struct PrimarySpriteData *)index)->currentAnimationFrame = normalized;
                    index = *slotCopy;
                    index *= parentStride;
                    index += (u32)sprites;
                    ((struct PrimarySpriteData *)index)->animationTimer = normalized;
                    index = *slotCopy;
                    index *= parentStride;
                    index += (u32)sprites;
                    ((struct PrimarySpriteData *)index)->work2 = 20;
                    current->pose = 19;
                    AerodentLoadNormalGraphics();
                    return;
                }
            }
        }

        {
            register int count PIN("r1");
            u8 *timer;
            int value;

            count = SpriteUtilCountSpriteType(205);
            timer = &current->work0;
            value = *timer;
            if (value != 0) {
                value--;
                *timer = value;
                return;
            }
            if (count > 0) {
                if (current->health <= 4 && (u8)gStageFrameCounter == 0)
                    AerodentSpawnShot();
            } else {
                sprite->pose = 112;
            }
        }
    }
}
void AerodentCompanionFireProjectile(void)
{
    u16 direction;

    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DAD48);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 113;
    gCurrentSprite.work0 = 60;
    SpriteUtilTurnTowardWario();
    direction = gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT;
    if (direction != 0) {
        SpawnPrimarySpriteWithStatus(205, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition - 40, gCurrentSprite.xPosition + 32,
            SPRITE_STATUS_FACING_RIGHT);
    } else {
        SpawnPrimarySpriteWithStatus(205, gCurrentSprite.roomSlot, gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition - 40, gCurrentSprite.xPosition - 32, direction);
    }
    m4aSongNumStart(SOUND_8F);
}
void AerodentCompanionRecoverFromProjectile(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u8 *slot;

    current = &gCurrentSprite;
    sprites = gSpriteData;
    slot = &current->work3;
    current->xPosition = sprites[*slot].xPosition + 140;
    current->yPosition = sprites[*slot].yPosition - sUnk_83DBA34[current->work1];
    current->work0--;
    if (current->work0 == 0) {
        current->pose = 21;
        current->work0 = 80;
    }
}
void AerodentCompanionKnockback(void)
{
    struct PrimarySpriteData *sprite;
    s16 (*velocity)[2];
    register u32 index PIN("r2");
    register u16 yVelocity PIN("r4");
    int zero;

    {
        struct PrimarySpriteData *current;
        u8 *work0;
        int value;

        current = &gCurrentSprite;
        work0 = &current->work0;
        value = *work0;
        sprite = current;
        if (value == 0) {
            struct PrimarySpriteData *sprites;
            u8 *parentSlot;
            int parentIndex;
            int parentOffset;
            u16 parentStatus;
            int statusBit;

            sprites = gSpriteData;
            parentSlot = &sprite->work3;
            parentIndex = *parentSlot;
            parentOffset = sizeof(struct PrimarySpriteData);
            parentOffset *= parentIndex;
            parentOffset += (u32)sprites;
            parentStatus = ((struct PrimarySpriteData *)parentOffset)->status;
            statusBit = SPRITE_STATUS_HEAVY;
            statusBit &= parentStatus;
            if (statusBit != 0)
                sprite->pose = 116;
            return;
        }
    }

    {
        struct PrimarySpriteData *sprites;
        u8 *parentSlot;
        int parentIndex;
        int parentOffset;
        int parentPose;

        sprites = gSpriteData;
        parentSlot = &sprite->work3;
        parentIndex = *parentSlot;
        parentOffset = sizeof(struct PrimarySpriteData);
        parentOffset *= parentIndex;
        parentOffset += (u32)sprites;
        parentPose = ((struct PrimarySpriteData *)parentOffset)->pose;
        if (parentPose == 32) {
            sprite->xPosition++;
            index = sprite->work2;
            velocity = sUnk_83DBB62;
        } else {
            sprite->xPosition--;
            index = sprite->work2;
            velocity = sUnk_83DBAE2;
        }
    }

    {
        const s16 *entry;
        int signedVelocity;
        int sentinel;
        u32 xVelocity;

        entry = (const s16 *)((index << 2) + (u32)velocity);
        yVelocity = *(const u16 *)entry;
        zero = 0;
        signedVelocity = *(s16 *)((u8 *)entry + zero);
        sentinel = 0x7FFF;
        if (signedVelocity == sentinel) {
            index = 0;
            yVelocity = *(const u16 *)velocity;
        }
        {
            u32 xValue;

            entry = (const s16 *)(index << 2);
            xValue = (u32)velocity;
            xValue += 2;
            entry = (const s16 *)((u32)entry + xValue);
            xValue = *(const u16 *)entry;
            xVelocity = xValue;
        }
        {
            int nextIndex;

            nextIndex = index + 1;
            index = (u32)sprite;
            index += 41;
            *(u8 *)index = nextIndex;
        }
        sprite->yPosition += yVelocity;
        sprite->xPosition += xVelocity;
    }

    {
        u8 *timer;
        int value;

        timer = &sprite->work0;
        value = *timer;
        value--;
        *timer = value;
        value <<= 24;
        if (value == 0)
            sprite->status |= SPRITE_STATUS_HIDDEN;
    }
}
void AerodentCompanionStartDropAttack(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DADF0);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 117;
    gCurrentSprite.work0 = 30;
    gCurrentSprite.status &= ~SPRITE_STATUS_HIDDEN;
}
void AerodentCompanionDropAttack(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u8 *slot;

    current = &gCurrentSprite;
    sprites = gSpriteData;
    slot = &current->work3;
    current->xPosition = sprites[*slot].xPosition + 112;
    current->yPosition = sprites[*slot].yPosition + 312;
    if (current->work0 == 14)
        current->warioCollision = 45;
    current->work0--;
    if (current->work0 == 0) {
        current->pose = 118;
        current->work0 = current->health * 4 + 60;
    }
}
void AerodentCompanionStartGroundAttack(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DAE78);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 119;
    gCurrentSprite.warioCollision = 45;
}
void AerodentCompanionGroundAttack(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u8 *slot;
    register u16 y PIN("r1");
    u16 distance;

    current = &gCurrentSprite;
    sprites = gSpriteData;
    slot = &current->work3;
    current->xPosition = sprites[*slot].xPosition + 112;
    y = sprites[*slot].yPosition + 312;
    current->yPosition = y;
    distance = (y >> 2) - (gBg1YPosition >> 2);
    current->work0--;
    if (distance <= 64 || current->work0 == 0) {
        current->pose = 120;
        current->warioCollision = 0;
    }
}
void AerodentCompanionStartReturn(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DAE50);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 121;
    gCurrentSprite.work0 = 16;
}
void AerodentCompanionReturnToParent(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    register int value PIN("r6");

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    if (value == 0) {
        register struct PrimarySpriteData *sprites PIN("r5");
        int stride;
        int zero;
        u8 *parentSlot;
        register int index PIN("r0");
        register int offset PIN("r4");
        register u16 parentStatus PIN("r1");
        int statusBit;
        const struct AnimationFrame *animation;
        u8 *slotCopy;

        sprites = gSpriteData;
        parentSlot = (u8 *)42;
        /* Materialize the parent-slot offset before adding the sprite base. */
        parentSlot += (u32)sprite;
        timer = parentSlot;
        index = *(volatile u8 *)parentSlot;
        stride = sizeof(struct PrimarySpriteData);
        offset = index;
        offset *= stride;
        index = offset + (u32)sprites;
        parentStatus = ((struct PrimarySpriteData *)index)->status;
        statusBit = SPRITE_STATUS_HEAVY;
        statusBit &= parentStatus;
        zero = (u16)statusBit;
        if (zero != 0)
            return;

        index = (u32)sprites + 4;
        index = offset + index;
        animation = sAerodentOam_83DAB28;
        GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)index, animation);
        slotCopy = timer;

        index = *slotCopy;
        index *= stride;
        index += (u32)sprites;
        ((struct PrimarySpriteData *)index)->currentAnimationFrame = zero;
        index = *slotCopy;
        index *= stride;
        index += (u32)sprites;
        ((struct PrimarySpriteData *)index)->animationTimer = zero;
        index = *slotCopy;
        index *= stride;
        index += (u32)sprites;
        index += 41;
        parentStatus = 20;
        *(u8 *)index = parentStatus;

        sprite->pose = 19;
        parentStatus = sprite->status;
        statusBit = 0xFFFB;
        statusBit &= parentStatus;
        sprite->status = statusBit;
        return;
    }

    {
        struct PrimarySpriteData *sprites;
        u8 *parentSlot;
        int stride;
        int index;
        int parentOffset;
        register int yOffset;

        sprites = gSpriteData;
        parentSlot = &sprite->work3;
        index = *parentSlot;
        stride = sizeof(struct PrimarySpriteData);
        index *= stride;
        index += (u32)sprites;
        index = ((struct PrimarySpriteData *)index)->xPosition;
        index += 112;
        sprite->xPosition = index;

        index = *(volatile u8 *)parentSlot;
        parentOffset = index;
        parentOffset *= stride;
        parentOffset += (u32)sprites;
        yOffset = 156;
        yOffset <<= 1;
        ASM_BARRIER_VOLATILE(: "+r"(yOffset) : "r"(index), "r"(parentOffset));
        index = yOffset;
        parentOffset = ((struct PrimarySpriteData *)parentOffset)->yPosition;
        index += parentOffset;
        sprite->yPosition = index;

        index = value - 1;
        parentOffset = (u32)timer;
        *(u8 *)parentOffset = index;
        index <<= 24;
        if (index != 0)
            return;

        sprite->status |= SPRITE_STATUS_HIDDEN;
        index = *parentSlot;
        index *= stride;
        index += (u32)sprites;
        parentOffset = 18;
        ((struct PrimarySpriteData *)index)->pose = parentOffset;
        index = *parentSlot;
        index *= stride;
        index += (u32)sprites;
        index += 39;
        parentOffset = 128;
        *(u8 *)index = parentOffset;
        index = *parentSlot;
        index *= stride;
        index += (u32)sprites;
        parentOffset = 2;
        ((struct PrimarySpriteData *)index)->disableWarioCollisionTimer = parentOffset;
    }
}
void AerodentCompanionStartRecoil(void)
{
    gCurrentSprite.pose = 36;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DAE38);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 16;
    gCurrentSprite.warioCollision = 0;
    m4aSongNumStart(SOUND_92);
}
void AerodentCompanionRecoil(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u8 *slot;

    current = &gCurrentSprite;
    sprites = gSpriteData;
    slot = &current->work3;
    current->xPosition = sprites[*slot].xPosition + 112;
    current->yPosition = sprites[*slot].yPosition + 312;
    current->work0--;
    if (current->work0 == 0) {
        current->pose = 118;
        current->work0 = current->health * 4 + 50;
    }
}
void AerodentCompanionTakeDamage(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprite;
    int pose;
    int health;
    u8 remaining;

    current = &gCurrentSprite;
    pose = current->pose;
    sprite = current;
    if (pose == 31)
        sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
    else
        sprite->status |= SPRITE_STATUS_FACING_RIGHT;

    {
        int zero;
        int poseValue;
        int collisionZero;

        zero = 0;
        poseValue = 32;
        sprite->pose = poseValue;
        GP_SET(sprite->pOamData, sAerodentOam_83DB110);
        sprite->currentAnimationFrame = zero;
        collisionZero = 0;
        sprite->animationTimer = zero;
        {
            u8 *work0;

            work0 = &sprite->work0;
            *work0 = poseValue;
            sprite->warioCollision = collisionZero;
        }
    }
    SpawnSecondarySprite(sprite->yPosition, sprite->xPosition, 6);

    health = sprite->health;
    if (health != 0) {
        health--;
        sprite->health = health;
        remaining = health;
        if (remaining == 0) {
            struct PrimarySpriteData *sprites;
            u8 *parentSlot;
            int stride;
            int index;
            int offset;
            struct PrimarySpriteData *parent;

            sprite->drawPriority |= 0x80;
            sprites = gSpriteData;
            parentSlot = &sprite->work3;
            index = *(volatile u8 *)parentSlot;
            stride = sizeof(struct PrimarySpriteData);
            offset = index;
            offset *= stride;
            offset += (u32)sprites;
            parent = (struct PrimarySpriteData *)offset;
            parent->drawPriority |= 0x80;
            gSpriteData[*parentSlot].pose = 47;
            SpriteUtilFindSpriteSlotWork3(204);
            gSpriteData[*parentSlot].status = remaining;
            gTimerState = 11;
            SpriteUtilSetWarioBossVictoryPose();
        }
        UpdateBossHealthGauge();
    }
    VoiceSetPlay(12);
}
void AerodentCompanionDefeat(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timerPtr;
    int timer;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    if (timer != 0) {
        timer--;
        *timerPtr = timer;
        if ((u8)timer == 0) {
            int health;
            u8 *work;

            health = sprite->health;
            if (health == 0) {
                work = &sprite->work2;
                *work = 8;
                sprite->work3 = health;
                sprite->drawPriority = 0x80;
                sprite->status &= 0x7FFF;
                m4aSongNumStart(SOUND_8C);
            }
        }
    } else {
        if (sprite->health != 0)
            sprite->pose = 47;
        else if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
            SpriteUtilFallOffscreenLeft();
        else
            SpriteUtilFallOffscreenRight();
    }

    {
        struct PrimarySpriteData *current;
        register int value PIN("r1");
        u8 flash;
        register int normalized PIN("r0");

        current = &gCurrentSprite;
        value = current->work0;
        if ((value & 7) == 0) {
            flash = value & 8;
            if (flash != 0) {
                current->palette = 4;
                /* Preserve the explicit byte normalization used by the original source. */
                ASM_BARRIER(: "+r"(value));
                normalized = (u8)value;
                if (normalized == 24)
                    VoiceSetPlay(1);
            } else {
                current->palette = flash;
            }
        }
    }
}
void AerodentCompanionStartRecovery(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB0E8);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 40;
    gCurrentSprite.pose = 48;
}
void AerodentCompanionRecover(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *spritesTemp;
    struct PrimarySpriteData *sprites;
    s32 xPosition;
    s32 nextX;
    u8 *timer;
    u32 timerValue;
    s32 yPosition;
    s32 target;

    current = &gCurrentSprite;
    xPosition = current->xPosition;
    spritesTemp = gSpriteData;
    target = spritesTemp[current->work3].xPosition + 112;
    sprites = spritesTemp;
    if (xPosition > target)
        nextX = xPosition - 2;
    else
        nextX = (2) - (-(xPosition));
    current->xPosition = nextX;

    yPosition = current->yPosition;
    target = sprites[current->work3].yPosition + 284;
    if (yPosition > target)
        current->yPosition = yPosition - 3;
    else
        current->yPosition = yPosition + 3;

    timer = &current->work0;
    timerValue = *timer;
    timerValue--;
    *timer = timerValue;
    if ((timerValue << 24) == 0) {
        current->pose = 118;
        *timer = current->health * 4 + 30;
    }
}
void AerodentCompanionApplyShopItemDamage(void)
{
    u8 item;

    switch (gCurrentShopItem) {
    case 2:
        item = 14;
        break;
    case 3:
        item = 13;
        break;
    case 4:
        item = 12;
        break;
    case 5:
        item = 8;
        break;
    case 6:
        item = 2;
        break;
    case 7:
        item = 9;
        break;
    case 8:
        item = 7;
        break;
    default:
        item = 15;
        break;
    }

    gCurrentSprite.work2 = item;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB110);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.pose = 123;
    if (gCurrentSprite.health != 0) {
        gCurrentSprite.health--;
        UpdateBossHealthGauge();
        m4aSongNumStart(SE_DAMAGE);
    }
    VoiceSetPlay(12);
}
void AerodentCompanionUpdateShopItemDamage(void)
{
    struct PrimarySpriteData *sprite;
    u8 *counter;
    int oldValue;
    int newValue;
    int oldFrame;

    sprite = &gCurrentSprite;
    counter = &sprite->work1;
    oldValue = *counter;
    newValue = oldValue + 1;
    *counter = newValue;
    oldValue <<= 24;
    oldFrame = (u32)oldValue >> 24;
    newValue <<= 24;
    newValue = (u32)newValue >> 24;
    if (newValue == 32) {
        u8 *work2;
        int health;
        register int healthCopy PIN("r1");

        work2 = &sprite->work2;
        health = sprite->health;
        healthCopy = health;
        if (healthCopy == *work2) {
            AerodentCompanionStartIdle();
            GP_SET(sprite->pOamData, sAerodentOam_83DB0E8);
            sprite->currentAnimationFrame = 0;
            sprite->animationTimer = 0;
        } else {
            *counter = 0;
            if (healthCopy != 0) {
                int reducedHealth;

                reducedHealth = health - 1;
                sprite->health = reducedHealth;
                UpdateBossHealthGauge();
                m4aSongNumStart(SE_DAMAGE);
            }
        }
    }

    {
        int mask;

        mask = 7;
        mask &= oldFrame;
        if (mask == 0) {
            int bit;

            bit = 8;
            bit &= oldFrame;
            if (bit != 0)
                gCurrentSprite.palette = mask;
            else
                gCurrentSprite.palette = 4;
        }
    }
}
void AerodentThrowableInit(void)
{
    gCurrentSprite.status &= 0xFFFB;
    gCurrentSprite.warioCollision = 46;
    gCurrentSprite.drawDistanceDown = 32;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 32;
    gCurrentSprite.hitboxExtentUp = 48;
    gCurrentSprite.hitboxExtentDown = 24;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB48C);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 106;
    gCurrentSprite.work3 = 0;
}
void AerodentThrowablePatrol(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition + sprite->hitboxExtentRight));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition += 2;
    } else {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition - sprite->hitboxExtentLeft));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status |= SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition -= 2;
    }

    {
        register struct PrimarySpriteData *current PIN("r2");
        register u8 *indexPtr PIN("r12");
        int index;
        const s16 *table;
        const s16 *entry;
        register int velocity PIN("r4");
        int zero;
        int signedVelocity;

        current = &gCurrentSprite;
        indexPtr = &current->work3;
        index = *indexPtr;
        table = sUnk_83DBBE2;
        entry = (const s16 *)index;
        entry = (const s16 *)((u32)entry << 1);
        entry = (const s16 *)((u32)entry + (u32)table);
        velocity = *(volatile u16 *)entry;
        zero = 0;
        signedVelocity = *(s16 *)((u8 *)entry + zero);
        if (signedVelocity == 0x7FFF) {
            int previous;
            register int position PIN("r0");

            previous = index - 1;
            previous <<= 1;
            previous += (u32)table;
            position = current->yPosition;
            previous = *(u16 *)previous;
            position += previous;
            current->yPosition = position;
        } else {
            register int next PIN("r0");
            int position;

            next = index + 1;
            *indexPtr = next;
            position = current->yPosition;
            position += velocity;
            current->yPosition = position;
        }

        {
            struct PrimarySpriteData *alias;
            u8 *timer;
            int value;

            alias = current;
            timer = &alias->work0;
            value = *timer;
            if (value == 40) {
                GP_SET(alias->pOamData, sAerodentOam_83DB3FC);
                alias->currentAnimationFrame = 0;
                alias->animationTimer = 0;
            }
            value = *timer - 1;
            *timer = value;
            value <<= 24;
            {
                int normalized;

                normalized = (u32)value >> 24;
                if (normalized == 0) {
                    int clear;
                    register u8 *work3 PIN("r0");

                    current->pose = 20;
                    GP_SET(current->pOamData, sAerodentOam_83DB43C);
                    current->currentAnimationFrame = normalized;
                    clear = 0;
                    current->animationTimer = normalized;
                    work3 = &current->work3;
                    *work3 = clear;
                }
            }
        }
    }
}
void AerodentThrowableBounce(void)
{
    struct PrimarySpriteData *sprite;
    int nextY;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition + sprite->hitboxExtentRight));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition++;
    } else {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition - sprite->hitboxExtentLeft));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status |= SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition--;
    }

    sprite = &gCurrentSprite;
    nextY = func_8023A60((u16)(sprite->yPosition + 48), sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        sprite->status |= 0x800;
        m4aSongNumStart(SE_ENTER_WATER);
    }
    if (gSpriteCollisionResult != 0) {
        register int frameZero PIN("r1");
        int timerZero;
        u8 *work;

        frameZero = 0;
        timerZero = 0;
        sprite->yPosition = nextY;
        GP_SET(sprite->pOamData, sAerodentOam_83DB4AC);
        sprite->currentAnimationFrame = frameZero;
        sprite->animationTimer = timerZero;
        work = &sprite->work0;
        *work = 30;
        sprite->pose = 24;
        work -= 4;
        *work = 96;
        work++;
        *work = 140;
    } else {
        u8 *savedPtr;
        int index;
        const s16 *table;
        const s16 *entry;
        register int velocity PIN("r3");
        int signedVelocity;
        int position;

        {
            int offset;

            offset = 42;
            /* Keep the constant materialized before adding the sprite base. */
            offset += (u32)sprite;
            savedPtr = (u8 *)offset;
            index = *(u8 *)offset;
        }
        table = sUnk_83DBC08;
        entry = (const s16 *)index;
        entry = (const s16 *)((u32)entry << 1);
        entry = (const s16 *)((u32)entry + (u32)table);
        velocity = *(volatile u16 *)entry;
        {
            int zero;

            zero = 0;
            signedVelocity = *(s16 *)((u8 *)entry + zero);
        }
        if (signedVelocity == 0x7FFF) {
            register int previous PIN("r1");

            previous = index - 1;
            previous <<= 1;
            previous += (u32)table;
            position = sprite->yPosition;
            previous = *(u16 *)previous;
            position += previous;
        } else {
            int next;

            next = index + 1;
            *savedPtr = next;
            position = sprite->yPosition;
            position += velocity;
        }
        sprite->yPosition = position;
    }
}
void AerodentThrowableStartCharge(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB4BC);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 16;
    gCurrentSprite.work0 = 24;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.hitboxExtentUp = 96;
    gCurrentSprite.hitboxExtentDown = 140;
    gCurrentSprite.warioCollision = 1;
    m4aSongNumStart(SE_BOUNCY_WARIO_BOUNCE);
}
void AerodentThrowableCharge(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition + sprite->hitboxExtentRight));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition += 4;
    } else {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition - sprite->hitboxExtentLeft));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status |= SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition -= 4;
    }

    {
        register struct PrimarySpriteData *current PIN("r2");
        register u8 *indexPtr PIN("r12");
        int index;
        const s16 *table;
        const s16 *entry;
        register int velocity PIN("r4");
        int zero;
        int signedVelocity;

        current = &gCurrentSprite;
        indexPtr = (u8 *)42;
        indexPtr = (u8 *)((u32)indexPtr + (u32)current);
        index = *indexPtr;
        table = sUnk_83DBC32;
        entry = (const s16 *)index;
        entry = (const s16 *)((u32)entry << 1);
        entry = (const s16 *)((u32)entry + (u32)table);
        velocity = *(volatile u16 *)entry;
        zero = 0;
        signedVelocity = *(s16 *)((u8 *)entry + zero);
        if (signedVelocity == 0x7FFF) {
            int previous;
            register int position PIN("r0");

            previous = index - 1;
            previous <<= 1;
            previous += (u32)table;
            position = current->yPosition;
            previous = *(u16 *)previous;
            position += previous;
            current->yPosition = position;
        } else {
            register int next PIN("r0");
            int position;

            next = index + 1;
            *indexPtr = next;
            position = current->yPosition;
            position += velocity;
            current->yPosition = position;
        }

        {
            struct PrimarySpriteData *alias;
            u8 *timer;
            int value;

            alias = current;
            timer = &alias->work0;
            value = *timer;
            if (value == 20)
                alias->warioCollision = 46;
            value = *timer - 1;
            *timer = value;
            value <<= 24;
            {
                int normalized;

                normalized = (u32)value >> 24;
                if (normalized == 0) {
                    int clear;
                    register u8 *work3 PIN("r0");

                    current->pose = 22;
                    GP_SET(current->pOamData, sAerodentOam_83DB3A4);
                    current->currentAnimationFrame = normalized;
                    clear = 0;
                    current->animationTimer = normalized;
                    work3 = &current->work3;
                    *work3 = clear;
                }
            }
        }
    }
}
void AerodentThrowableBounceAfterCharge(void)
{
    struct PrimarySpriteData *sprite;
    int nextY;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition + sprite->hitboxExtentRight));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition += 2;
    } else {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition - sprite->hitboxExtentLeft));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status |= SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition -= 2;
    }

    sprite = &gCurrentSprite;
    nextY = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        sprite->status |= 0x800;
        m4aSongNumStart(SE_ENTER_WATER);
    }
    if (gSpriteCollisionResult != 0) {
        register int frameZero PIN("r1");
        int timerZero;
        u8 *work;

        frameZero = 0;
        timerZero = 0;
        sprite->yPosition = nextY;
        GP_SET(sprite->pOamData, sAerodentOam_83DB4AC);
        sprite->currentAnimationFrame = frameZero;
        sprite->animationTimer = timerZero;
        work = &sprite->work0;
        *work = 30;
        sprite->pose = 24;
        m4aSongNumStart(SOUND_5);
    } else {
        u8 *savedPtr;
        int index;
        const s16 *table;
        const s16 *entry;
        register int velocity PIN("r3");
        int signedVelocity;
        int position;

        {
            int offset;

            offset = 42;
            /* Materialize the parent-slot offset before adding the sprite base. */
            offset += (u32)sprite;
            savedPtr = (u8 *)offset;
            index = *(u8 *)offset;
        }
        table = sUnk_83DBC52;
        entry = (const s16 *)index;
        entry = (const s16 *)((u32)entry << 1);
        entry = (const s16 *)((u32)entry + (u32)table);
        velocity = *(volatile u16 *)entry;
        {
            int zero;

            zero = 0;
            signedVelocity = *(s16 *)((u8 *)entry + zero);
        }
        if (signedVelocity == 0x7FFF) {
            register int previous PIN("r1");

            previous = index - 1;
            previous <<= 1;
            previous += (u32)table;
            position = sprite->yPosition;
            previous = *(u16 *)previous;
            position += previous;
        } else {
            int next;

            next = index + 1;
            *savedPtr = next;
            position = sprite->yPosition;
            position += velocity;
        }
        sprite->yPosition = position;
    }
}
void AerodentThrowableStartStun(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB4D4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.hitboxExtentUp = 96;
    gCurrentSprite.hitboxExtentDown = 140;
    gCurrentSprite.pose = 40;
    gCurrentSprite.work0 = 10;
}
void AerodentThrowableStun(void)
{
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0)
        gCurrentSprite.pose = 15;
}
void AerodentThrowableStartGrabbed(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB3A4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
}
void AerodentThrowableUpdateGrabbed(void)
{
    struct PrimarySpriteData *sprite;
    u8 *work;
    int zero;

    sprite = &gCurrentSprite;
    work = &sprite->work0;
    zero = 0;
    *work = 4;
    work += 2;
    *work = 8;
    {
        u8 *work3;

        work3 = &sprite->work3;
        *work3 = zero;
    }
    if (sprite->warioCollision != 59) {
        int frameZero;

        sprite->warioCollision = 59;
        GP_SET(sprite->pOamData, sAerodentOam_83DB3B4);
        frameZero = 0;
        sprite->animationTimer = zero;
        sprite->currentAnimationFrame = frameZero;
        {
            u8 *hitboxUp;
            u8 *hitboxDown;

            hitboxUp = &sprite->hitboxExtentUp;
            *hitboxUp = 64;
            hitboxDown = &sprite->hitboxExtentDown;
            *hitboxDown = frameZero;
        }
    }
}

void AerodentThrowableGrabbedRight(void)
{
    gCurrentSprite.pose = 52;
    AerodentThrowableUpdateGrabbed();
}
void AerodentThrowableGrabbedLeft(void)
{
    gCurrentSprite.pose = 54;
    AerodentThrowableUpdateGrabbed();
}
void AerodentThrowableStartLanding(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sAerodentOam_83DB4AC);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 24;
    sprite->work0 = 15;
}
void AerodentThrowableLand(void)
{
    func_80238A4();
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0)
        gCurrentSprite.pose = 15;
}
void AerodentThrowableStartBreak(void)
{
    gCurrentSprite.status |= SPRITE_STATUS_BACKGROUND;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB4E4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 111;
    gCurrentSprite.work0 = 40;
    SpriteUtilFindSpriteSlotWork3(105);
    SpawnHighPriorityPrimarySprite(11, gCurrentSprite.roomSlot, 0,
        gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition);
    m4aSongNumStart(SOUND_8E);
    VoiceSetPlay(0);
}
void AerodentThrowableBreak(void)
{
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u8 *timer;
    u32 next;
    u32 timerValue;

    current = &gCurrentSprite;
    current->disableWarioCollisionTimer = 1;
    sprites = gSpriteData;
    if (sprites[current->work3].work2 != 0)
        current->yPosition += 2;

    timer = &current->work0;
    timerValue = *timer;
    next = timerValue - 1;
    *timer = next;
    timerValue = (u8)next;
    if (timerValue != 0) {
        next &= 7;
        if (next == 0) {
            SpawnHighPriorityPrimarySprite(12, current->roomSlot, 0,
                current->yPosition - 64, current->xPosition);
        }
    } else {
        current->status = timerValue;
        SpawnHighPriorityPrimarySprite(10, current->roomSlot, 0,
            current->yPosition, current->xPosition);
    }
}
void AerodentThrowableStartBounce(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB3B4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 59;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
}
void AerodentThrowableStartFall(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->pose = 48;
    sprite->status &= ~0x100;
}
void AerodentThrowableFall(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0)
        gCurrentSprite.pose = 29;
}
void AerodentThrowablePrepareFallWithEffect(void)
{
    struct PrimarySpriteData *sprite;
    int zero;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sAerodentOam_83DB3EC);
    sprite->currentAnimationFrame = 0;
    zero = 0;
    sprite->animationTimer = 0;
    sprite->health = zero;
    sprite->work2 = 8;
    sprite->work3 = zero;
    sprite->drawPriority = zero;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    sprite->status = (sprite->status & ~0x100) | 0x200;
}
void AerodentThrowableFallRightWithEffect(void)
{
    gCurrentSprite.pose = 32;
    gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    AerodentThrowablePrepareFallWithEffect();
}
void AerodentThrowableFallLeftWithEffect(void)
{
    gCurrentSprite.pose = 34;
    gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    AerodentThrowablePrepareFallWithEffect();
}

void AerodentThrowablePrepareFall(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB3EC);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
}
void AerodentThrowableFallRight(void)
{
    gCurrentSprite.pose = 32;
    gCurrentSprite.status &= ~SPRITE_STATUS_FACING_RIGHT;
    AerodentThrowablePrepareFall();
}
void AerodentThrowableFallLeft(void)
{
    gCurrentSprite.pose = 34;
    gCurrentSprite.status |= SPRITE_STATUS_FACING_RIGHT;
    AerodentThrowablePrepareFall();
}

void AerodentThrowablePrepareKnockback(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB3B4);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 59;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
}
void AerodentThrowableKnockbackRight(void)
{
    gCurrentSprite.pose = 44;
    AerodentThrowablePrepareKnockback();
}
void AerodentThrowableKnockbackLeft(void)
{
    gCurrentSprite.pose = 46;
    AerodentThrowablePrepareKnockback();
}
void AerodentThrowableDie(void)
{
    SpriteUtilDie();
}

void AerodentThrowablePrepareThrow(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB3B4);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 59;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;
}
void AerodentThrowableThrowRight(void)
{
    gCurrentSprite.pose = 72;
    AerodentThrowablePrepareThrow();
}
void AerodentThrowableThrowLeft(void)
{
    gCurrentSprite.pose = 74;
    AerodentThrowablePrepareThrow();
}
void AerodentThrowableStartLiftRight(void)
{
    gCurrentSprite.pose = 82;
}
void AerodentThrowableStartLiftLeft(void)
{
    gCurrentSprite.pose = 84;
}
void AerodentThrowableStartCarryRight(void)
{
    gCurrentSprite.pose = 88;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}
void AerodentThrowableStartCarryLeft(void)
{
    gCurrentSprite.pose = 86;
    gCurrentSprite.disableWarioCollisionTimer = 1;
}
void AerodentThrowableSetThrownAnimation(void)
{
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB49C);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}
void AerodentEmitterInit(void)
{
    u16 status;
    u8 frame;

    gCurrentSprite.drawPriority |= 0x80;
    status = gCurrentSprite.status & 0xFFFB;
    frame = 0;
    /* agbcc otherwise delays this live zero until after the status OR. */
    ASM_BARRIER(: "+r"(frame));
    status |= 0x400;
    gCurrentSprite.status = status;
    gCurrentSprite.drawDistanceDown = 16;
    gCurrentSprite.drawDistanceUp = 16;
    gCurrentSprite.drawDistanceLeftRight = 16;
    gCurrentSprite.hitboxExtentUp = 32;
    gCurrentSprite.hitboxExtentDown = 28;
    gCurrentSprite.hitboxExtentLeft = 32;
    gCurrentSprite.hitboxExtentRight = 28;
    gCurrentSprite.work0 = 16;
    GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB5C8);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = frame;
    gCurrentSprite.warioCollision = 14;
    gCurrentSprite.pose = 15;
    SpriteUtilFindSpriteSlotWork3(203);
    if (gCurrentSprite.roomSlot != 0)
        AerodentEmitterStartBurst();
}
void AerodentEmitterAttachToParent(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT)
        gCurrentSprite.xPosition = gSpriteData[gCurrentSprite.work3].xPosition + 56;
    else
        gCurrentSprite.xPosition = gSpriteData[gCurrentSprite.work3].xPosition - 56;

    gCurrentSprite.yPosition = gSpriteData[gCurrentSprite.work3].yPosition;
    gCurrentSprite.work0--;
    if (gCurrentSprite.work0 == 0) {
        gCurrentSprite.pose = 16;
        gCurrentSprite.work3 = 0;
        GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB5E8);
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.currentAnimationFrame = 0;
    }
}
void AerodentEmitterPatrol(void)
{
    struct PrimarySpriteData *sprite;
    int nextY;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition + sprite->hitboxExtentRight));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status &= ~SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition++;
    } else {
        func_8023BFC((u16)(sprite->yPosition - 32),
            (u16)(sprite->xPosition - sprite->hitboxExtentLeft));
        if (gSpriteCollisionTileType & 0xF)
            sprite->status |= SPRITE_STATUS_FACING_RIGHT;
        else
            sprite->xPosition--;
    }

    sprite = &gCurrentSprite;
    nextY = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1)
        sprite->pose = 107;
    if (gSpriteCollisionResult != 0) {
        sprite->yPosition = nextY;
        sprite->pose = 23;
    } else {
        u8 *savedPtr;
        int index;
        const s16 *table;
        const s16 *entry;
        register int velocity PIN("r3");
        int signedVelocity;
        int position;

        {
            int offset;

            offset = 42;
            /* Keep the constant materialized before adding the sprite base. */
            offset += (u32)sprite;
            savedPtr = (u8 *)offset;
            index = *(u8 *)offset;
        }
        table = sUnk_83DBC68;
        entry = (const s16 *)index;
        entry = (const s16 *)((u32)entry << 1);
        entry = (const s16 *)((u32)entry + (u32)table);
        velocity = *(volatile u16 *)entry;
        {
            int zero;

            zero = 0;
            signedVelocity = *(s16 *)((u8 *)entry + zero);
        }
        if (signedVelocity == 0x7FFF) {
            register int previous PIN("r1");

            previous = index - 1;
            previous <<= 1;
            previous += (u32)table;
            position = sprite->yPosition;
            previous = *(u16 *)previous;
            position += previous;
        } else {
            int next;

            next = index + 1;
            *savedPtr = next;
            position = sprite->yPosition;
            position += velocity;
        }
        sprite->yPosition = position;
    }
}
void AerodentEmitterStartBurst(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 25;
    gCurrentSprite.hitboxExtentUp = 64;
    gCurrentSprite.hitboxExtentDown = 0;

    if (gCurrentSprite.roomSlot == 2) {
        GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB588);
        gCurrentSprite.pose = 48;
    } else if (gCurrentSprite.roomSlot == 1) {
        GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB5A8);
        gCurrentSprite.pose = 48;
    } else {
        GP_SET(gCurrentSprite.pOamData, sAerodentOam_83DB5A8);
        gCurrentSprite.pose = 24;
    }
}
void AerodentEmitterBurst(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u8 value;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    if (value > 10) {
        if (value == 25) {
            SpawnPrimarySpriteWithStatus(206, 1, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition - 28, 0);
            SpawnPrimarySpriteWithStatus(206, 1, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition + 28, 0);
        }
        if (value == 20) {
            SpawnPrimarySpriteWithStatus(206, 2, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition - 56, 0);
            SpawnPrimarySpriteWithStatus(206, 2, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition + 56, 0);
        }
        if (value == 15) {
            SpawnPrimarySpriteWithStatus(206, 2, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition - 84, 0);
            SpawnPrimarySpriteWithStatus(206, 2, sprite->gfxSlot,
                sprite->yPosition, sprite->xPosition + 84, 0);
        }
        {
            int next;

            next = *timer - 1;
            *timer = next;
        }
    } else {
        int next;

        sprite->disableWarioCollisionTimer = 1;
        if ((value & 1) == 0)
            sprite->status ^= 4;
        next = *timer - 1;
        *timer = next;
        next = (u8)next;
        if (next == 0)
            sprite->status = next;
    }
}
void AerodentEmitterFadeOut(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    u8 value;
    register u32 next PIN("r0");

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer;
    if (value > 10) {
        *timer = value - 1;
        return;
    }

    sprite->disableWarioCollisionTimer = 1;
    if (!(value & 1))
        sprite->status ^= SPRITE_STATUS_HIDDEN;

    next = *timer;
    next--;
    *timer = next;
    next = (u8)next;
    if (next == 0)
        sprite->status = next;
}
void SpriteAerodentWeakPoint(void)
{
    struct PrimarySpriteData *sprite;
    u16 pause;

    pause = gWarioPauseTimer;
    sprite = &gCurrentSprite;
    if (pause != 0)
        sprite->disableWarioCollisionTimer = 1;

    switch (sprite->pose) {
        case 0: {
            struct PrimarySpriteData *current;
            int priority;
            register int value PIN("r0");
            int status;
            register int highStatus PIN("r2");
            int timerZero;
            register int frameZero PIN("r2");
            u8 *distance;
            register int distanceValue PIN("r1");
            int clearMask;

            current = &gCurrentSprite;
            priority = current->drawPriority;
            value = 128;
            timerZero = 0;
            value |= priority;
            current->drawPriority = value;
            status = current->status;
            highStatus = 128;
            highStatus <<= 8;
            /* Keep the high status bit in r2 before copying it to the r0 accumulator. */
            ASM_BARRIER(: "+r"(highStatus));
            value = highStatus;
            frameZero = 0;
            value |= status;
            clearMask = 0xFFFB;
            value &= clearMask;
            current->status = value;
            GP_SET(current->pOamData, sAerodentOam_83DBA60);
            current->currentAnimationFrame = frameZero;
            current->animationTimer = timerZero;
            current->pose = 16;
            distance = &current->drawDistanceDown;
            distanceValue = 64;
            *distance = distanceValue;
            distance++;
            *distance = distanceValue;
            distance++;
            *distance = distanceValue;
            SpriteUtilFindSpriteSlotWork3(105);
            break;
        }
        case 31:
        case 33:
        case 35:
        case 37: {
            struct PrimarySpriteData *sprites;
            int parentSlot;
            register int offset PIN("r0");
            struct PrimarySpriteData *parent;
            int parentPose;

            sprites = gSpriteData;
            offset = (u32)sprite;
            offset += 42;
            parentSlot = *(u8 *)offset;
            offset = sizeof(struct PrimarySpriteData);
            offset *= parentSlot;
            offset += (u32)sprites;
            parent = (struct PrimarySpriteData *)offset;
            parentPose = parent->pose;
            if (parentPose != 18 && parentPose != 32 && parentPose != 34)
                parent->pose = sprite->pose;
        }
        default: {
            struct PrimarySpriteData *current;
            int pose;
            int status;
            int mask;

            current = &gCurrentSprite;
            pose = 16;
            current->pose = pose;
            status = current->status;
            mask = 0xFDFF;
            mask &= status;
            current->status = mask;
            break;
        }
    }

    if (AerodentWeakPointCheckThrowableCollision() != 0) {
        struct PrimarySpriteData *sprites;
        u8 *parentSlot;
        register int stride PIN("r5");
        int slot;
        register int value PIN("r1");

        sprites = gSpriteData;
        parentSlot = (u8 *)&gCurrentSprite;
        parentSlot += 42;
        slot = *parentSlot;
        stride = sizeof(struct PrimarySpriteData);
        slot *= stride;
        slot += (u32)sprites;
        slot += 41;
        value = 129;
        *(u8 *)slot = value;
        SpriteUtilFindSpriteSlotWork3(203);
        {
            struct PrimarySpriteData *parent;
            u32 health;

            slot = *parentSlot;
            slot *= stride;
            parent = (struct PrimarySpriteData *)(slot + (u32)sprites);
            health = parent->health;
            if (health <= 8) {
                parent = (struct PrimarySpriteData *)((u8 *)parent + 39);
                health = 160;
            } else {
                parent = (struct PrimarySpriteData *)((u8 *)parent + 39);
                health = 190;
            }
            *(u8 *)parent = health;
        }
        SpriteUtilFindSpriteSlotWork3(105);
    }

    {
        struct PrimarySpriteData *current;
        register struct PrimarySpriteData *sprites PIN("r8");
        register struct PrimarySpriteData *spritesLoad PIN("r3");
        u8 *parentSlot;
        int stride;
        u8 *work1;
        int offset;
        u32 value;
        register u32 x PIN("r1");
        int yOffset;
        register int collisionMask PIN("r9");
        u8 *collisionFlags;
        int collisionValue;
        register int maskValue PIN("r0");

        current = &gCurrentSprite;
        spritesLoad = gSpriteData;
        /* Preserve the target's r3 load before copying the array base to r8. */
        ASM_BARRIER(: "+r"(spritesLoad));
        sprites = spritesLoad;
        parentSlot = (u8 *)current;
        parentSlot += 42;
        offset = *parentSlot;
        stride = sizeof(struct PrimarySpriteData);
        offset *= stride;
        offset += (u32)sprites;
        offset += 40;
        value = *(u8 *)offset;
        work1 = (u8 *)current;
        work1 += 40;
        *work1 = value;
        AerodentWeakPointUpdateHitbox();

        offset = *parentSlot;
        offset *= stride;
        offset += (u32)sprites;
        x = *(u16 *)(offset + 10);
        x += 128;
        current->xPosition = x;

        offset = *parentSlot;
        yOffset = offset;
        yOffset *= stride;
        yOffset += (u32)sprites;
        value = *work1;
        value <<= 2;
        value += 240;
        yOffset = *(u16 *)(yOffset + 8);
        value += yOffset;
        current->yPosition = value;
        {
            u32 yArg;

            yArg = (value - 32) << 16;
            yArg >>= 16;
            yOffset = current->hitboxExtentRight;
            x += yOffset;
            x = (u16)x;
            SpriteUtilCheckCollisionAtPositionOld(yArg, x);
        }

        collisionFlags = &gSpriteCollisionTileType;
        collisionValue = *collisionFlags;
        maskValue = 15;
        collisionMask = maskValue;
        maskValue &= collisionValue;
        if (maskValue != 0) {
            struct PrimarySpriteData *parent;
            int status;
            register int statusMask PIN("r0");

            offset = *parentSlot;
            /* Keep the parent slot in r0 before copying it to the r1 address accumulator. */
            ASM_BARRIER(: "+r"(offset));
            parent = (struct PrimarySpriteData *)offset;
            parent = (struct PrimarySpriteData *)((u32)parent * stride);
            parent = (struct PrimarySpriteData *)((u32)parent + (u32)sprites);
            status = parent->status;
            statusMask = 0xEFFF;
            statusMask &= status;
            parent->status = statusMask;
        } else {
            {
                u32 yArg;

                yArg = (current->yPosition - 32) << 16;
                yArg >>= 16;
                {
                    u8 *extent;
                int extentValue;

                extent = (u8 *)current;
                extent += 37;
                    extentValue = *extent;
                    x = current->xPosition;
                    x -= extentValue;
                }
                x = (u16)x;
                SpriteUtilCheckCollisionAtPositionOld(yArg, x);
            }
            collisionValue = *collisionFlags;
            maskValue = collisionMask;
            maskValue &= collisionValue;
            if (maskValue != 0) {
                struct PrimarySpriteData *parent;
                int status;
                register int high PIN("r3");
                register int statusBit PIN("r2");

                offset = *parentSlot;
                /* Keep the parent slot in r0 before copying it to the r1 address accumulator. */
                ASM_BARRIER(: "+r"(offset));
                parent = (struct PrimarySpriteData *)offset;
                parent = (struct PrimarySpriteData *)((u32)parent * stride);
                parent = (struct PrimarySpriteData *)((u32)parent + (u32)sprites);
                status = parent->status;
                high = 128;
                high <<= 5;
                /* Preserve the separate r3-to-r2 copy required by the target. */
                ASM_BARRIER(: "+r"(high));
                statusBit = high;
                /* Keep the status-bit copy separate from the subsequent OR. */
                ASM_BARRIER(: "+r"(statusBit));
                status |= statusBit;
                parent->status = status;
            }
        }
    }
}
void SpriteAerodent(void)
{
    register int angle;
    int value;
    register u16 status PIN("r1");
    register struct PrimarySpriteData *current PIN("r2");

    current = &gCurrentSprite;
    status = current->status;
    value = SPRITE_STATUS_HEAVY;
    value &= status;
    value = (u16)value;
    value = -value;
    angle = value >> 31;
    angle &= 128;
    value = 1;
    current->disableWarioCollisionTimer = value;

    switch (current->pose) {
        case 0:
            AerodentInit();
            break;
        case 1:
            AerodentEnterArena();
            break;
        case 16:
            AerodentIdle();
            break;
        case 35:
            AerodentStartMoveRight();
        case 36:
            AerodentMoveRight();
            break;
        case 37:
            AerodentStartMoveLeft();
        case 38:
            AerodentMoveLeft();
            break;
        case 31:
            AerodentStartDeflate();
        case 32:
            AerodentDeflate();
            angle = gCurrentSprite.work0;
            break;
        case 33:
            AerodentStartInflate();
        case 34:
            AerodentInflate();
            angle = gCurrentSprite.work0;
            break;
        case 18:
            AerodentUpdateRotation();
            angle = gCurrentSprite.work0;
            break;
        case 47:
            AerodentStartDefeat();
        case 48:
            AerodentDefeat();
            break;
    }

    AerodentUpdateInflation();
    {
        const s16 *scaleTable;
        const s16 *scaleEntry;
        struct PrimarySpriteData *sprite;
        register int trigOrSine PIN("r4");
        int scale;
        int cosine;
        int cosineLoad;
        int address;
        int zero;

        scaleTable = sUnk_83DBA10;
        sprite = &gCurrentSprite;
        scaleEntry = (const s16 *)(((u32)sprite->work1 << 1) + (u32)scaleTable);
        trigOrSine = (u32)sSinCosTable;
        address = (angle + 64) << 1;
        address += trigOrSine;
        cosineLoad = *(s16 *)address;
        cosine = cosineLoad;
        scale = *scaleEntry;
        sprite->affinePA = FixedMul(cosine, FixedInverse(scale));

        address = angle << 1;
        address += trigOrSine;
        zero = 0;
        trigOrSine = *(s16 *)(address + zero);
        sprite->affinePB = FixedMul(trigOrSine, FixedInverse(scale));
        sprite->affinePC = FixedMul(-trigOrSine, FixedInverse(scale));
        sprite->affinePD = FixedMul(cosine, FixedInverse(scale));
    }
}
void SpriteAerodentCompanion(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            AerodentCompanionInit();
        case 15:
            AerodentCompanionWaitForBattle();
            break;
        case 16:
            AerodentCompanionDescend();
            break;
        case 17:
            AerodentCompanionStartAttach();
        case 18:
            AerodentCompanionFollowParent();
            break;
        case 23:
            AerodentCompanionStartFightCountdown();
        case 24:
            AerodentCompanionBeginBossFight();
            break;
        case 19:
            AerodentCompanionStartRise();
        case 20:
            AerodentCompanionRise();
            break;
        case 21:
            AerodentCompanionEnterCombat();
        case 22:
            AerodentCompanionUpdateCombat();
            break;
        case 112:
            AerodentCompanionFireProjectile();
        case 113:
            AerodentCompanionRecoverFromProjectile();
            break;
        case 115:
            AerodentCompanionKnockback();
            break;
        case 116:
            AerodentCompanionStartDropAttack();
        case 117:
            AerodentCompanionDropAttack();
            break;
        case 118:
            AerodentCompanionStartGroundAttack();
        case 119:
            AerodentCompanionGroundAttack();
            break;
        case 120:
            AerodentCompanionStartReturn();
        case 121:
            AerodentCompanionReturnToParent();
            break;
        case 35:
        case 37:
            AerodentCompanionStartRecoil();
        case 36:
            AerodentCompanionRecoil();
            break;
        case 31:
        case 33:
            AerodentCompanionTakeDamage();
        case 32:
            AerodentCompanionDefeat();
            break;
        case 47:
            AerodentCompanionStartRecovery();
        case 48:
            AerodentCompanionRecover();
            break;
        case 122:
            AerodentCompanionApplyShopItemDamage();
        case 123:
            AerodentCompanionUpdateShopItemDamage();
            break;
    }
}
void SpriteAerodentThrowable(void)
{
    struct PrimarySpriteData *sprite;
    u16 status;
    int mask;

    sprite = &gCurrentSprite;
    status = sprite->status;
    mask = 128;
    mask <<= 4;
    mask &= status;
    if (mask != 0)
        sprite->pose = 107;

    switch (sprite->pose) {
        case 0:
            AerodentThrowableInit();
            goto end;
        case 18:
            AerodentThrowablePatrol();
            goto end;
        case 20:
            AerodentThrowableBounce();
            goto end;
        case 41:
            if (sprite->warioCollision != 59)
                goto state_55E0;
            goto state_56A4;
        case 37:
            if (sprite->warioCollision != 59)
                goto state_5358;
state_56A4:
        case 53:
            AerodentThrowableGrabbedLeft();
state_2476:
        case 54:
        case 58:
            func_802476C();
            goto end;
        case 39:
            if (sprite->warioCollision == 59)
                goto state_5690;
state_55E0:
            AerodentThrowableStartStun();
            goto end;
        case 40:
            AerodentThrowableStun();
            goto end;
        case 35:
            if (sprite->warioCollision == 59)
                goto state_5690;
state_5358:
            AerodentThrowableStartCharge();
state_53A0:
        case 16:
            AerodentThrowableCharge();
            goto end;
state_5690:
        case 51:
            AerodentThrowableGrabbedRight();
state_24988:
        case 52:
        case 64:
            func_8024988();
            goto end;
        case 15:
            AerodentThrowableStartCharge();
            goto state_53A0;
        case 22:
            AerodentThrowableBounceAfterCharge();
            goto end;
        case 23:
            AerodentThrowableStartLanding();
        case 24:
            AerodentThrowableLand();
            goto end;
        case 110:
            AerodentThrowableStartBreak();
        case 111:
            AerodentThrowableBreak();
            goto end;
        case 27:
            AerodentThrowableStartGrabbed();
        case 28:
            func_8023D48();
            goto end;
        case 29:
            AerodentThrowableStartBounce();
        case 30:
            func_8023EE0();
            goto end;
        case 31:
        case 79:
            AerodentThrowableFallRightWithEffect();
        case 32:
            SpriteUtilFallOffscreenRight();
            goto end;
        case 33:
        case 80:
            AerodentThrowableFallLeftWithEffect();
        case 34:
            SpriteUtilFallOffscreenLeft();
            goto end;
        case 43:
            AerodentThrowableKnockbackRight();
        case 44:
            func_8024478();
            goto end;
        case 45:
            AerodentThrowableKnockbackLeft();
        case 46:
            func_802449C();
            goto end;
        case 47:
            AerodentThrowableStartFall();
        case 48:
            AerodentThrowableFall();
            goto end;
        case 55:
            func_8024688();
        case 56:
            func_80246B8();
            goto end;
        case 57:
            func_802473C();
            goto state_2476;
        case 59:
            func_80247F0();
        case 60:
            func_8024820();
            goto end;
        case 61:
            func_80248A4();
        case 62:
            func_80248D4();
            goto end;
        case 63:
            func_8024958();
            goto state_24988;
        case 65:
            func_8024A0C();
        case 66:
            func_8024A3C();
            goto end;
        case 67:
            func_8024AC0();
            goto state_24AD4;
        case 69:
            func_8024BEC();
            goto state_24C00;
        case 71:
            AerodentThrowableThrowRight();
state_24AD4:
        case 68:
        case 72:
            func_8024AD4();
            goto end;
        case 73:
            AerodentThrowableThrowLeft();
state_24C00:
        case 70:
        case 74:
            func_8024C00();
            goto end;
        case 81:
            AerodentThrowableStartLiftRight();
            goto end;
        case 82:
            SpriteUtilLiftingSpriteRight();
            goto end;
        case 83:
            AerodentThrowableStartLiftLeft();
            goto end;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            goto end;
        case 87:
            AerodentThrowableStartCarryRight();
            goto end;
        case 88:
            SpriteUtilCarryingSpriteRight();
            goto end;
        case 85:
            AerodentThrowableStartCarryLeft();
            goto end;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            goto end;
        case 89:
            sprite->pose = 90;
            AerodentThrowableSetThrownAnimation();
        case 90:
            SpriteUtilThrownLeftSoft();
            goto end;
        case 93:
            sprite->pose = 94;
            AerodentThrowableSetThrownAnimation();
        case 94:
            SpriteUtilThrownLeftHard();
            goto end;
        case 97:
            sprite->pose = 98;
            AerodentThrowableSetThrownAnimation();
        case 98:
            SpriteUtilThrownUpLeftSoft();
            goto end;
        case 101:
            sprite->pose = 102;
            AerodentThrowableSetThrownAnimation();
        case 102:
            SpriteUtilThrownUpLeftHard();
            goto end;
        case 91:
            sprite->pose = 92;
            AerodentThrowableSetThrownAnimation();
        case 92:
            SpriteUtilThrownRightSoft();
            goto end;
        case 95:
            sprite->pose = 96;
            AerodentThrowableSetThrownAnimation();
        case 96:
            SpriteUtilThrownRightHard();
            goto end;
        case 99:
            sprite->pose = 100;
            AerodentThrowableSetThrownAnimation();
        case 100:
            SpriteUtilThrownUpRightSoft();
            goto end;
        case 103:
            sprite->pose = 104;
            AerodentThrowableSetThrownAnimation();
        case 104:
            SpriteUtilThrownUpRightHard();
            goto end;
        case 105:
            AerodentThrowableFallLeft();
            goto end;
        case 106:
            AerodentThrowableFallRight();
            goto end;
        default:
            AerodentThrowableDie();
    }
end:
    return;
}
void SpriteAerodentEmitter(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            AerodentEmitterInit();
            break;
        case 15:
            AerodentEmitterAttachToParent();
            break;
        case 16:
            AerodentEmitterPatrol();
            break;
        case 23:
            AerodentEmitterStartBurst();
        case 24:
            AerodentEmitterBurst();
            break;
        case 48:
            AerodentEmitterFadeOut();
            break;
        default:
            SpriteUtilDie();
    }
}

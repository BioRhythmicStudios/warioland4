#include "sprite_ai/shopkeeper.h"

#include "background_registers.h"
#include "fixed_point.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "main.h"
#include "score.h"
#include "screen_shake.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"

struct AGB_ALIGN ShopkeeperDmaRegisters {
    GBA_PTR(const void*) volatile src;
    GBA_PTR(void*) volatile dst;
    volatile u32 control;
};

void ShopkeeperUpdateGraphicsScript(const u8* script)
{
    const u8* scr;
    register struct PrimarySpriteData* s PIN("r6");
    u32 idx;
    register u32 temp PIN("r0");
    u32 frame;
    struct ShopkeeperDmaRegisters* dma;
    const u8* src;
    scr = script;
    temp = (u32)&gCurrentSprite;
    {
        u8* ptr1;
        register u32 c1 PIN("r2");
        ptr1 = (u8*)temp;
        ptr1 += 40;
        c1 = *ptr1;
        s = (struct PrimarySpriteData*)temp;
        if (c1 != 0) {
            temp = c1 - 1;
            *ptr1 = temp;
            return;
        }
    }
    {
        u8* ptr2;
        u32 c2;
        ptr2 = &s->work0;
        temp = *ptr2;
        c2 = temp + 1;
        *ptr2 = c2;
        temp <<= 24;
        idx = temp >> 24;
        temp = idx << 2;
        temp += (u32)scr;
        frame = *(u8*)temp;
        if (frame == 0xFF) {
            idx = 0;
            temp = 1;
            *ptr2 = temp;
            frame = scr[0];
        }
    }
    temp = 0x80;
    temp &= frame;
    if (temp != 0) {
        u32 st;
        st = s->status;
        temp = 0x40;
        temp |= st;
    } else {
        u32 st;
        st = s->status;
        temp = 0xFFBF;
        temp &= st;
    }
    s->status = temp;
    temp = 0x7F;
    frame &= temp;
    temp = idx << 2;
    temp += (u32)scr;
    {
        u32 delay;
        delay = *(u8*)(temp + 1);
        temp = (u32)s;
        temp += 40;
        *(u8*)temp = delay;
    }
    dma = (struct ShopkeeperDmaRegisters*)0x40000D4;
    src = (const u8*)GP(sShopkeeperGraphicsFramePointers[frame]);
    GP_SET(dma->src, src);
    GP_SET(dma->dst, (void*)0x6012400);
    dma->control = 0x80000080;
    (void)dma->control;
    GP_SET(dma->src, src + 0x400);
    GP_SET(dma->dst, (void*)0x6012800);
    dma->control = 0x80000080;
    (void)dma->control;
}

void PlayShopkeeperCarrySoundCue(void)
{
    volatile u8* s;
    u32 p;
    s = (volatile u8*)&gCurrentSprite;
    p = (u32)s;
    p += 39;
    p = *(volatile u8*)p;
    if (p == 2) {
        p = (u32)s;
        p += 40;
        p = *(volatile u8*)p;
        if (p == 6) {
            m4aSongNumStart(500);
        }
    }
}
void PlayShopkeeperReadySoundCue(void)
{
    volatile u8* s;
    u32 p;
    s = (volatile u8*)&gCurrentSprite;
    p = (u32)s;
    p += 39;
    p = *(volatile u8*)p;
    if (p == 6) {
        p = (u32)s;
        p += 40;
        p = *(volatile u8*)p;
        if (p == 60) {
            m4aSongNumStart(501);
        }
    }
}

void ShopkeeperFollowWario(void)
{
    register u32 r0 PIN("r0");
    u32 r1;
    u32 r2;
    struct PrimarySpriteData* r3;
    register u32 r4 PIN("r4");
    u32 r5;

    r3 = &gCurrentSprite;
    r0 = 128;
    r0 <<= 2;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r1 = r0;
    r2 = r3->xPosition;
    r0 = r1 + r2;
    r4 = (u16)r0;
    r2 = (u32)&gWarioData;
    r0 = *(u16*)(r2 + 18);
    r1 += r0;
    r1 <<= 16;
    r1 >>= 16;
    r0 = r4 - r1;
    r5 = r2;
    if ((s32)r0 <= 64) {
        goto x_other_side;
    }

    r0 = r1;
    r0 += 64;
    r1 = (u16)r0;
    if (r4 <= r1) {
        goto x_abs_left1;
    }
    r0 = r4 - r1;
    goto x_abs_done1;
x_abs_left1:
    r0 = r1 - r4;
x_abs_done1:
    r0 = (u16)r0;
    if (r0 > 8) {
        goto x_calc1;
    }
    r2 = 0;
    goto x_speed_done1;
x_calc1:
    r0 -= 8;
    r0 <<= 13;
    r2 = r0 >> 16;
x_speed_done1:
    if (r4 > r1) {
        goto x_sub;
    }
    if (r4 >= r1) {
        goto x_done;
    }
    r0 = r2 + r3->xPosition;
    goto x_store;

x_other_side:
    r0 = r1 - r4;
    if ((s32)r0 <= 64) {
        goto x_done;
    }
    r0 = r1;
    r0 -= 64;
    r1 = (u16)r0;
    if (r4 <= r1) {
        goto x_abs_left2;
    }
    r0 = r4 - r1;
    goto x_abs_done2;
x_abs_left2:
    r0 = r1 - r4;
x_abs_done2:
    r0 = (u16)r0;
    if (r0 > 8) {
        goto x_calc2;
    }
    r2 = 0;
    goto x_speed_done2;
x_calc2:
    r0 -= 8;
    r0 <<= 13;
    r2 = r0 >> 16;
x_speed_done2:
    if (r4 < r1) {
        r0 = *(volatile u16 *)&r3->xPosition;
        /* Pure C changes the operand order and therefore the Thumb encoding. */
        ASM_BARRIER(: "+r"(r0) : "r"(r2));
        r0 = r2 - -(int)r0;

        goto x_store;
    }
    if (r4 <= r1) {
        goto x_done;
    }
x_sub:
    r0 = r3->xPosition;
    r0 -= r2;
x_store:
    r3->xPosition = r0;
x_done:
    r1 = 128;
    r1 <<= 2;

    r0 = r1;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r2 = r3->yPosition;
    r0 += r2;
    r4 = (u16)r0;
    r1 += 4;

    /*
     * A C assignment emits mov r0, r1; the original uses add r0, r1, #0.
     */
    ASM_BARRIER_VOLATILE(: "=r"(r0) : "0"(r1));
    r5 = *(u16*)(r5 + 20);
    r0 += r5;
    r1 = (u16)r0;
    if (r4 <= r1) {
        goto y_abs_left;
    }
    r0 = r4 - r1;
    goto y_abs_done;
y_abs_left:
    r0 = r1 - r4;
y_abs_done:
    r0 = (u16)r0;
    if (r0 > 3) {
        goto y_calc;
    }
    r2 = 0;
    goto y_speed_done;
y_calc:
    r0 -= 4;
    r0 <<= 14;
    r2 = r0 >> 16;
    if (r2 != 0) {
        goto y_speed_done;
    }
    r2 = 2;
y_speed_done:
    if (r4 > r1) {
        r0 = *(volatile u16 *)&r3->yPosition;
        r0 -= r2;
        goto y_store;
    }
    if (r4 >= r1) {
        goto y_done;
    }
    r0 = *(volatile u16 *)&r3->yPosition;
    /*
     * Pure C commutes the operands, producing a different Thumb encoding.
     */
    ASM_BARRIER(: "+r"(r0) : "r"(r2));
    r0 = r2 - -(int)r0;

y_store:
    r3->yPosition = r0;
y_done:;
}

void ShopItemFollowShopkeeper(void)
{
    register u32 r0 PIN("r0");
    register u32 r1 PIN("r1");
    u32 r2;
    register u32 r3 PIN("r3");
    register u32 r4 PIN("r4");
    u32 r5;
    register struct PrimarySpriteData* r6 PIN("r6");

    r2 = 0;
    r5 = 0;
    r0 = (u32)gSpriteData;
    r1 = *(u8*)(r0 + 23);
    r6 = (struct PrimarySpriteData*)r0;
    if (r1 == 9) {
        r1 = *(u16*)r6;
        r0 = 1;
        r0 &= r1;
        if (r0 != 0) {
            goto found;
        }
    }
scan:
    r5++;
    if ((s32)r5 > 23) {
        goto scan_done;
    }
    r0 = 44;
    r0 *= r5;
    r1 = r0 + (u32)r6;
    r0 = *(u8*)(r1 + 23);
    if (r0 != 9) {
        goto scan;
    }
    r1 = *(u16*)r1;
    r0 = 1;
    r0 &= r1;
    if (r0 == 0) {
        goto scan;
    }
    r2++;
scan_done:
    if (r2 == 0) {
        goto end;
    }
found:
    r2 = (u32)&gCurrentSprite;
    r0 = 128;
    r0 <<= 2;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r1 = r0;
    r3 = *(u16 *)&((struct PrimarySpriteData *)r2)->xPosition;
    r0 = r1 + r3;
    r4 = (u16)r0;
    r0 = 44;
    r0 *= r5;
    r0 += (u32)r6;
    r0 = ((struct PrimarySpriteData*)r0)->xPosition;
    r1 += r0;
    r3 = (u16)r1;
    r0 = r4 - r3;
    if ((s32)r0 <= 64) {
        goto x_other_side;
    }

    r0 = *(u16*)r2;
    r1 = 64;
    r1 |= r0;
    *(u16*)r2 = r1;
    r0 = r3;
    r0 += 64;
    r3 = (u16)r0;
    if (r4 <= r3) {
        goto x_abs_left1;
    }
    r0 = r4 - r3;
    goto x_abs_done1;
x_abs_left1:
    r0 = r3 - r4;
x_abs_done1:
    r0 = (u16)r0;
    if (r0 > 8) {
        goto x_calc1;
    }
    r1 = 0;
    goto x_speed_done1;
x_calc1:
    r0 -= 8;
    r0 <<= 13;
    r1 = r0 >> 16;
x_speed_done1:
    if (r4 > r3) {
        goto x_sub;
    }
    if (r4 >= r3) {
        goto x_done;
    }
    r0 = r1 + *(u16 *)&((struct PrimarySpriteData *)r2)->xPosition;
    goto x_store;

x_other_side:
    r0 = r3 - r4;
    if ((s32)r0 <= 64) {
        goto x_done;
    }
    r0 = *(u16*)r2;
    r1 = 0xFFBF;
    r1 &= r0;
    *(u16*)r2 = r1;
    r0 = r3;
    r0 -= 64;
    r3 = (u16)r0;
    if (r4 <= r3) {
        goto x_abs_left2;
    }
    r0 = r4 - r3;
    goto x_abs_done2;
x_abs_left2:
    r0 = r3 - r4;
x_abs_done2:
    r0 = (u16)r0;
    if (r0 > 8) {
        goto x_calc2;
    }
    r1 = 0;
    goto x_speed_done2;
x_calc2:
    r0 -= 8;
    r0 <<= 13;
    r1 = r0 >> 16;
x_speed_done2:
    if (r4 < r3) {
        r0 = *(volatile u16 *)&((struct PrimarySpriteData *)r2)->xPosition;
        /* Pure C changes the operand order and therefore the Thumb encoding. */
        ASM_BARRIER(: "+r"(r0) : "r"(r1));
        r0 = r1 - -(int)r0;

        goto x_store;
    }
    if (r4 <= r3) {
        goto x_done;
    }
x_sub:
    r0 = *(u16 *)&((struct PrimarySpriteData *)r2)->xPosition;
    r0 -= r1;
x_store:
    *(u16 *)&((struct PrimarySpriteData *)r2)->xPosition = r0;
x_done:
    r1 = 128;
    r1 <<= 2;
    ASM_BARRIER_VOLATILE(: "+r"(r1));
    r0 = r1;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r3 = *(u16 *)&((struct PrimarySpriteData *)r2)->yPosition;
    r0 += r3;
    r4 = (u16)r0;
    r0 = 44;
    r1 = r5;
    r1 *= r0;
    r1 += (u32)r6;
    r3 = 129;
    r3 <<= 2;
    ASM_BARRIER_VOLATILE(: "+r"(r3));
    r0 = r3;

    r1 = *(volatile u16*)(r1 + 8);
    /*
     * agbcc changes the operand order for the equivalent C addition; retain
     * the target add r0, r0, r1 instruction.
     */
    ASM_BARRIER(: "+r"(r0) : "r"(r1));
    r0 += r1;
    r3 = (u16)r0;
    if (r4 <= r3) {
        goto y_abs_left;
    }
    r0 = r4 - r3;
    goto y_abs_done;
y_abs_left:
    r0 = r3 - r4;
y_abs_done:
    r0 = (u16)r0;
    if (r0 > 3) {
        goto y_calc;
    }
    r1 = 0;
    goto y_speed_done;
y_calc:
    r0 -= 4;
    r0 <<= 14;
    r1 = r0 >> 16;
    if (r1 != 0) {
        goto y_speed_done;
    }
    r1 = 2;
y_speed_done:
    if (r4 > r3) {
        r0 = *(volatile u16 *)&((struct PrimarySpriteData *)r2)->yPosition;
        r0 -= r1;
        goto y_store;
    }
    if (r4 >= r3) {
        goto end;
    }
    r0 = *(volatile u16 *)&((struct PrimarySpriteData *)r2)->yPosition;
    /*
     * Pure C commutes the operands, producing a different Thumb encoding.
     */
    ASM_BARRIER(: "+r"(r0) : "r"(r1));
    r0 = r1 - -(int)r0;

y_store:
    *(u16 *)&((struct PrimarySpriteData *)r2)->yPosition = r0;
end:;
}

void InitShopkeeper(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    u32 temp;

    s = &gCurrentSprite;
    temp = s->status;
    value = 4;
    value |= temp;
    s->status = value;

    value = gCurrentRoom;
    if (value == 0) {
        value = 2;
        s->pose = value;
    } else {
        func_801E518();
        temp = s->drawPriority;
        value = 128;
        value |= temp;
        s->drawPriority = value;
        value = 1;
        s->pose = value;
        value = s->xPosition;
        value -= 160;
        s->xPosition = value;
        value = gCurrentPassage;
        if (value == 5 || gCurrentShopItem != 0) {
            GF(sWarioPoseRequestFuncTable[gWarioData.reaction])(55);
        } else {
            GF(sWarioPoseRequestFuncTable[gWarioData.reaction])(56);
        }
    }
}

void WaitForShopkeeperRoomReady(void)
{
    if (gBossDefeatTimer[0] == 0) {
        gCurrentSprite.pose = 2;
    }
}

void SetupShopkeeperSelectedItem(void)
{
    struct PrimarySpriteData* s;
    u32 zero;
    u32 sixteen;
    u32 old;
    u32 status;
    register u32 temp PIN("r3");
    register u32 flag PIN("r1");
    register u16 flagCopy PIN("r0");

    s = &gCurrentSprite;
    old = s->status;
    status = 0xFFFB;
    status &= old;
    temp = 0;
    zero = 0;
    flagCopy = 0x400;
    status |= flagCopy;
    status |= temp;
    s->status = status;
    s->warioCollision = temp;
    flag = (u32)&s->drawDistanceDown;
    *(u8*)flag = 32;
    s->drawDistanceUp = temp;
    {
        u8* p = &s->drawDistanceLeftRight;
        sixteen = 16;
        *p = sixteen;
    }
    s->hitboxExtentUp = 4;
    s->hitboxExtentDown = 4;
    s->hitboxExtentLeft = 4;
    s->hitboxExtentRight = 4;
    GP_SET(s->pOamData, sSelectedShopItemOam);
    s->currentAnimationFrame = temp;
    s->animationTimer = zero;
    s->work0 = temp;
    s->work1 = temp;

    if (gCurrentRoom == 0) {
        s->pose = sixteen;
        ShopkeeperUpdateGraphicsScript(sShopkeeperDefaultGraphicsScript);
        switch (gCurrentShopItem) {
            case 1:
                SpriteSpawnAsChild(188, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 2:
                SpriteSpawnAsChild(190, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 3:
                SpriteSpawnAsChild(191, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 4:
                SpriteSpawnAsChild(192, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                break;
            default:
                s->status = zero;
                break;
        }
        return;
    }

    switch (gCurrentShopItem) {
        case 1:
            s = &gCurrentSprite;
            s->pose = 17;
            ShopkeeperUpdateGraphicsScript(sShopkeeperAppleBombGraphicsScript);
            SpawnHighPriorityPrimarySprite(188, s->roomSlot, 0, s->yPosition - 84, s->xPosition + 48);
            SpriteUtilFindSpriteSlotWork3(188);
            break;
        case 2:
            s = &gCurrentSprite;
            s->status |= 8;
            s->pose = 19;
            ShopkeeperUpdateGraphicsScript(sShopkeeperItemApproachGraphicsScript);
            SpawnHighPriorityPrimarySprite(190, s->roomSlot, 0, s->yPosition, s->xPosition - 80);
            SpriteUtilFindSpriteSlotWork3(190);
            break;
        case 3:
            s = &gCurrentSprite;
            s->status |= 8;
            s->pose = 110;
            ShopkeeperUpdateGraphicsScript(sShopkeeperVizormanGraphicsScript);
            SpawnHighPriorityPrimarySprite(191, s->roomSlot, 0, s->yPosition, s->xPosition + 96);
            SpriteUtilFindSpriteSlotWork3(191);
            break;
        case 4:
            s = &gCurrentSprite;
            s->status |= 8;
            s->pose = 114;
            ShopkeeperUpdateGraphicsScript(sShopkeeperItemApproachGraphicsScript);
            SpawnHighPriorityPrimarySprite(192, s->roomSlot, 0, s->yPosition, s->xPosition - 100);
            SpriteUtilFindSpriteSlotWork3(192);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            gCurrentSprite.status |= 8;
            gCurrentSprite.pose = 118;
            ShopkeeperUpdateGraphicsScript(sShopkeeperTransformationGraphicsScript);
            break;
        default:
            if (gCurrentPassage == 5) {
                struct ShopkeeperDmaRegisters* dma = (struct ShopkeeperDmaRegisters*)0x040000D4;
                GP_SET(dma->src, sShopkeeperItemTiles);
                GP_SET(dma->dst, (void*)0x06013000);
                dma->control = 0x80000800;
                (void)dma->control;
                s->status = status | 8;
                GP_SET(s->pOamData, sShopkeeperIdleOam);
                s->pose = 55;
                m4aSongNumStart(191);
            } else {
                s->status = zero;
            }
            break;
    }

    if (gCurrentSprite.status != 0 && gCurrentPassage != 5) {
        if (gCurrentShopItem == 1)
            m4aSongNumStart(171 << 2);
        else
            m4aSongNumStart(0x2AB);
    }
}

void StartShopkeeperSmileJump(void)
{
    struct ShopkeeperDmaRegisters* dma;
    register u32 value PIN("r0");
    struct PrimarySpriteData* s;
    register u32 zero8 PIN("r2");
    u32 zero16;
    u32 status;

    dma = (struct ShopkeeperDmaRegisters*)0x040000D4;
    value = (u32)sShopkeeperItemTiles;
    GP_SET(dma->src, (const void*)value);
    value = 0x06013000;
    GP_SET(dma->dst, (void*)value);
    value = 0x80000800;
    dma->control = value;
    value = dma->control;
    s = &gCurrentSprite;
    status = s->status;
    value = 8;
    zero8 = 0;
    zero16 = 0;
    value |= status;
    s->status = value;
    GP_SET(s->pOamData, sShopkeeperSmileJumpOam);
    *(volatile u8*)((u32)s + 22) = zero8;
    s->animationTimer = zero16;
    value = (u32)s;
    value += 42;
    *(volatile u8*)value = zero8;
    value = 66;
    s->pose = value;
    value = s->yPosition;
    value -= 64;
    ((void (*)())SpawnSecondarySprite)(value, s->xPosition, 64);
    zero16 = s->yPosition;
    value = s->xPosition;
    SpriteSpawnAsChild(10, 0, 0, zero16, value);
}

void UpdateShopkeeperSmileJump(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    u32 result;
    s = &gCurrentSprite;
    s->xPosition -= 4;
    s->yPosition += 8;
    value = s->yPosition;
    result = s->xPosition;
    result = func_8023A60(value, result);
    if (gSpriteCollisionResult != 0) {
        s->yPosition = result;
        GP_SET(s->pOamData, sShopkeeperSmileDropOam);
        s->currentAnimationFrame = 0;
        s->animationTimer = 0;
        s->work0 = 160;
        s->pose = 64;
        m4aSongNumStart(27);
    }
}

void StartShopkeeperSmileDrop(void)
{
    struct ShopkeeperDmaRegisters* dma;
    register u32 value PIN("r0");
    struct PrimarySpriteData* s;
    register u32 zero8 PIN("r2");
    u32 zero16;
    u32 status;
    u8* timer;

    dma = (struct ShopkeeperDmaRegisters*)0x040000D4;
    value = (u32)sShopkeeperItemTiles;
    GP_SET(dma->src, (const void*)value);
    value = 0x06013000;
    GP_SET(dma->dst, (void*)value);
    value = 0x80000800;
    dma->control = value;
    value = dma->control;
    s = &gCurrentSprite;
    status = s->status;
    value = 8;
    zero8 = 0;
    zero16 = 0;
    value |= status;
    s->status = value;
    GP_SET(s->pOamData, sShopkeeperSmileDropOam);
    *(volatile u8*)((u32)s + 22) = zero8;
    s->animationTimer = zero16;
    timer = &s->work0;
    value = 160;
    *timer = value;
    value = 64;
    s->pose = value;
    value = s->yPosition;
    value -= 64;
    ((void (*)())SpawnSecondarySprite)(value, s->xPosition, 64);
    zero16 = s->yPosition;
    value = s->xPosition;
    SpriteSpawnAsChild(10, 0, 0, zero16, value);
}

void UpdateShopkeeperSmileDrop(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    u32 v;
    u32 z;
    s = &gCurrentSprite;
    p = &s->work0;
    v = *p;
    v--;
    *p = v;
    z = (u8)v;
    if (z == 0) {
        GP_SET(s->pOamData, sShopkeeperIdleOam);
        s->currentAnimationFrame = z;
        s->animationTimer = z;
        s->pose = 55;
    }
}

void MoveShopkeeperToSmilePosition(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    u32 temp;
    u32 x;

    value = *(u8*)&gBossState;
    if (value != 0) {
        s = &gCurrentSprite;
        value = *(u32*)((u32)s + 20);
        temp = 0xFFFFFF;
        value &= temp;
        temp = 0x10002;
        if (value == temp) {
            value = 27;
            m4aSongNumStart(value);
        }
        x = s->xPosition;
        value = x >> 2;
        temp = gBg1XPosition;
        temp >>= 2;
        value -= temp;
        value <<= 16;
        temp = 0xFF800000;
        value += temp;
        value >>= 16;
        if (value > 112) {
            value = x + 4;
            s->xPosition = value;
        } else {
            value = (u32)sShopkeeperSmilePositionedOam;
            GP_SET(s->pOamData, (const struct AnimationFrame*)value);
            value = 0;
            *(volatile u8*)((u32)s + 22) = value;
            *(volatile u16*)((u32)s + 20) = value;
            temp = (u32)s;
            temp += 39;
            value = 96;
            *(u8*)temp = value;
            value = 56;
            s->pose = value;
        }
    }
}

void StartShopkeeperSmilePose(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    register u32 value PIN("r0");
    u32 zero;
    u32 temp;
    s = &gCurrentSprite;
    timer = &s->work0;
    value = *timer;
    value--;
    *timer = value;
    temp = (u8)value;
    if (temp == 0) {
        GP_SET(s->pOamData, sShopkeeperSmilePoseOam);
        s->currentAnimationFrame = temp;
        zero = 0;
        s->animationTimer = temp;
        value = 14;
        *timer = value;
        s->work3 = zero;
        s->pose = 57;
        m4aSongNumStart(13);
    }
}

void UpdateShopkeeperSmilePose(void)
{
    register struct PrimarySpriteData* base PIN("r2");
    u32 value;
    u32 index;
    const s16* table;
    register u32 offset PIN("r5");
    register u8* indexPtr PIN("r12");
    register s32 temp PIN("r1");

    base = &gCurrentSprite;
    value = 42;
    value += (u32)base;
    indexPtr = (u8*)value;
    index = *indexPtr;
    table = sShopkeeperSmileVelocityTable;
    value = index;
    value <<= 1;
    value += (u32)table;
    offset = *(volatile u16*)value;
    temp = *(s16*)value;
    value = 0x7FFF;
    {
        struct PrimarySpriteData* s;
        s = base;
        if ((s32)temp == (s32)value) {
            temp = index - 1;
            temp <<= 1;
            temp += (u32)table;
            value = s->yPosition;
            temp = *(volatile u16*)temp;
            ASM_BARRIER(: "+r"(value), "+r"(temp));
            value += temp;
        } else {
            value = index + 1;
            temp = (u32)indexPtr;
            *(volatile u8*)temp = value;
            value = s->yPosition;
            value += offset;
        }
        s->yPosition = value;
        value = s->xPosition;
        value += 12;
        s->xPosition = value;
        temp = (u32)s;
        temp += 39;
        value = *(volatile u8*)temp;
        value -= 1;
        *(volatile u8*)temp = value;
        value <<= 24;
        offset = value >> 24;
        if (offset == 0) {
            value = s->yPosition;
            value -= 42;
            temp = s->xPosition;
            temp += 42;
            SpawnSecondarySprite(value, temp, 70);
            GP_SET(s->pOamData, sShopkeeperSmileJumpOam);
            value = 0;
            *(volatile u8*)((u32)s + 22) = value;
            s->animationTimer = offset;
            value = 58;
            s->pose = value;
            value = 197;
            m4aSongNumStart(value);
            temp = (u32)&gBossState;
            value = 2;
            *(volatile u8*)temp = value;
        }
    }
}

void MoveShopkeeperSmileOffscreen(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    u32 y;
    u32 tmp;

    s = &gCurrentSprite;
    value = s->xPosition;
    value -= 6;
    s->xPosition = value;
    y = s->yPosition;
    tmp = y >> 2;
    value = gBg1YPosition;
    value >>= 2;
    tmp -= value;
    tmp <<= 16;
    tmp >>= 16;
    if (tmp <= 143) {
        value = y;
        value += 8;
        s->yPosition = value;
    } else {
        GP_SET(s->pOamData, (const struct AnimationFrame*)sShopkeeperSmileExitOam);
        value = 0;
        s->currentAnimationFrame = value;
        value = 0;
        s->animationTimer = value;
        tmp = (u32)s;
        tmp += 39;
        value = 180;
        *(volatile u8*)tmp = value;
        value = 59;
        s->pose = value;
        value = 27;
        m4aSongNumStart(value);
    }
}

void WaitForShopkeeperSmileEffect(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    register u32 value PIN("r0");
    u32 byte;
    u8* ptr;
    u32 zero;

    s = &gCurrentSprite;
    timer = &s->work0;
    value = *timer;
    zero = value;
    if (zero != 0) {
        value -= 1;
        *timer = value;
        byte = (u8)value;
        if (byte == 70) {
            ptr = (u8*)&gBossState;
            value = 3;
            *ptr = value;
        } else if (byte == 60) {
            value = 198;
            m4aSongNumStart(value);
        } else if (byte == 0) {
            GP_SET(s->pOamData, (const struct AnimationFrame*)sShopkeeperSmileWaitOam);
            s->currentAnimationFrame = byte;
            s->animationTimer = byte;
        }
    } else {
        value = *(volatile u8*)&gBossState;
        if (value == 4) {
            value = 60;
            s->pose = value;
            GP_SET(s->pOamData, (const struct AnimationFrame*)sShopkeeperSmileFinishOam);
            s->currentAnimationFrame = zero;
            byte = 0;
            s->animationTimer = zero;
            value = 240;
            *timer = value;
            value = (u32)s;
            value += 40;
            *(volatile u8*)value = byte;
            value = s->yPosition;
            value -= 36;
            s->yPosition = value;
            value = 192;
            m4aSongNumStart(value);
        }
    }
}

void RotateShopkeeperSmile(void)
{
    struct PrimarySpriteData* s;
    u32 r0;
    u32 r1;
    s32 trig;
    register s32 scale PIN("r5");
    register u8* angle PIN("r6");
    register const s16* table PIN("r8");
    s = &gCurrentSprite;
    r1 = s->status;
    r0 = 0x80;
    r0 |= r1;
    s->status = r0;
    table = sSinCosTable;
    angle = &s->work1;
    r0 = *angle;
    r0 += 0x40;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    scale = 0x100;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePA = r0;
    r0 = *angle;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePB = r0;
    r0 = *angle;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    trig = -trig;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePC = r0;
    r0 = *angle;
    r0 += 0x40;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePD = r0;
    r0 = *angle;
    r0++;
    *angle = r0;
    r0 = s->yPosition;
    r0--;
    s->yPosition = r0;
    r0 = s->xPosition;
    r0++;
    s->xPosition = r0;
    r1 = (u32)&s->work0;
    r0 = *(u8*)r1;
    r0--;
    *(u8*)r1 = r0;
    r0 <<= 24;
    if (r0 == 0) {
        r0 = 61;
        s->pose = r0;
        r0 = 252;
        *(u8*)r1 = r0;
    }
}

void ShrinkShopkeeperSmile(void)
{
    struct PrimarySpriteData* s;
    u32 r0;
    u32 r1;
    s32 trig;
    register u8* angle PIN("r5");
    register const s16* table PIN("r6");
    register u8* scalePtr PIN("r8");
    s = &gCurrentSprite;
    r1 = s->status;
    r0 = 0x80;
    r0 |= r1;
    s->status = r0;
    table = sSinCosTable;
    angle = &s->work1;
    r0 = *angle;
    r0 += 0x40;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    r0 = 39;
    ASM_BARRIER(: "+r"(r0));
    r0 += (u32)s;
    scalePtr = (u8*)r0;
    r0 = *(u8*)r0;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePA = r0;
    r0 = *angle;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    r1 = (u32)scalePtr;
    r0 = *(u8*)r1;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePB = r0;
    r0 = *angle;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    trig = -trig;
    r1 = (u32)scalePtr;
    r0 = *(u8*)r1;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePC = r0;
    r0 = *angle;
    r0 += 0x40;
    r0 <<= 1;
    r0 += (u32)table;
    r1 = 0;
    trig = *(s16*)r0;
    r1 = (u32)scalePtr;
    r0 = *(u8*)r1;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePD = r0;
    r0 = *angle;
    r0++;
    *angle = r0;
    r1 = (u32)scalePtr;
    r0 = *(u8*)r1;
    r0 -= 4;
    *(u8*)r1 = r0;
    r0 = (u8)r0;
    if (r0 > 8) {
        if (r0 == 152) {
            *(u8*)&gBossState = 5;
        }
    } else {
        r0 = 0;
        s->status = r0;
        ((void (*)())SpawnSecondarySprite)(s->yPosition, s->xPosition, 67);
        r0 = 119;
        m4aSongNumStart(r0);
    }
}

void MoveShopkeeperTransformationIntoPosition(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    u32 temp;

    value = gCurrentPassage;
    if (value == 5) {
        value = *(volatile u8*)&gBossState;
        if (value == 0) {
            return;
        }
    }
    s = &gCurrentSprite;
    value = s->xPosition;
    value >>= 2;
    temp = gBg1XPosition;
    temp >>= 2;
    value -= temp;
    value <<= 16;
    temp = 0xFF950000;
    value += temp;
    value >>= 16;
    if (value > 0x85) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperTransformationGraphicsScript);
        value = s->xPosition;
        value += 2;
        s->xPosition = value;
        PlayShopkeeperCarrySoundCue();
    } else {
        temp = 0;
        value = 119;
        s->pose = value;
        value = (u32)s;
        value += 39;
        *(volatile u8*)value = temp;
        value++;
        *(volatile u8*)value = temp;
        ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
        temp = (u32)s;
        temp += 41;
        value = 60;
        *(volatile u8*)temp = value;
    }
}

void SpawnShopkeeperTransformationItem(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    u8* timer;

    ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
    s = &gCurrentSprite;
    timer = &s->work2;
    value = *timer;
    value -= 1;
    *timer = value;
    value <<= 24;
    if (value != 0) {
        return;
    }
    value = 49;
    s->pose = value;
    value = gCurrentShopItem;
    if (value == 6) {
        goto item6;
    }
    if ((s32)value > 6) {
        goto gt6;
    }
    if (value == 5) {
        goto item5;
    }
    return;
gt6:
    if (value == 7) {
        goto item7;
    }
    if (value == 8) {
        goto item8;
    }
    return;
item5:
    SpawnHighPriorityPrimarySprite(193, s->roomSlot, 0, s->yPosition, s->xPosition);
    m4aSongNumStart(0x1FD);
    return;
item6:
    SpawnHighPriorityPrimarySprite(194, s->roomSlot, 0, s->yPosition - 12, s->xPosition + 20);
    m4aSongNumStart(0x1FF);
    return;
item7:
    SpawnHighPriorityPrimarySprite(195, s->roomSlot, 0, s->yPosition - 64, s->xPosition - 64);
    m4aSongNumStart(0x202);
    return;
item8:
    SpawnHighPriorityPrimarySprite(196, s->roomSlot, 0, s->yPosition - 64, s->xPosition);
    m4aSongNumStart(0x205);
}

void MoveShopkeeperBugleIntoPosition(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    u32 temp;
    u32 slot;
    struct PrimarySpriteData* child;
    struct PrimarySpriteData* base;

    value = gCurrentPassage;
    if (value == 5) {
        value = *(volatile u8*)&gBossState;
        if (value == 0) {
            return;
        }
    }
    s = &gCurrentSprite;
    slot = s->work3;
    value = s->xPosition;
    value >>= 2;
    temp = gBg1XPosition;
    temp >>= 2;
    value -= temp;
    value <<= 16;
    temp = 0xFF950000;
    value += temp;
    value >>= 16;
    if (value > 0x85) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperItemApproachGraphicsScript);
        value = s->xPosition;
        value += 2;
        s->xPosition = value;
        base = gSpriteData;
        value = 44;
        child = (struct PrimarySpriteData*)slot;
        child = (struct PrimarySpriteData*)((u32)child * value);
        child = (struct PrimarySpriteData*)((u32)child + (u32)base);
        value = child->xPosition;
        value += 2;
        child->xPosition = value;
        PlayShopkeeperCarrySoundCue();
    } else {
        temp = 0;
        value = 115;
        s->pose = value;
        value = (u32)s;
        value += 39;
        *(volatile u8*)value = temp;
        value++;
        *(volatile u8*)value = temp;
        ShopkeeperUpdateGraphicsScript(sShopkeeperItemReadyGraphicsScript);
        temp = (u32)s;
        temp += 41;
        value = 120;
        *(volatile u8*)temp = value;
    }
}

void WaitForShopkeeperBugleReady(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    u32 value;
    u32 next;
    register u32 byte PIN("r2");
    u32 slot;
    register u32 ff PIN("r4");
    s = &gCurrentSprite;
    slot = s->work3;
    timer = &s->work2;
    value = *timer;
    next = value - 1;
    *timer = next;
    ff = 255;
    value = next << 24;
    byte = value >> 24;
    if (byte != 0) {
        value = byte;
        ASM_BARRIER(: "+r"(value));
        if (value == 50) {
            register struct PrimarySpriteData* base PIN("r1");
            base = gSpriteData;
            value = 44;
            value *= slot;
            value += (u32)base;
            next = 17;
            *(volatile u8*)(value + 28) = next;
        }
        ShopkeeperUpdateGraphicsScript(sShopkeeperItemReadyGraphicsScript);
        PlayShopkeeperReadySoundCue();
    } else {
        value = 116;
        s->pose = value;
        next |= ff;
        *timer = next;
        value = (u32)s;
        value += 39;
        *(volatile u8*)value = byte;
        value++;
        *(volatile u8*)value = byte;
        ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
    }
}

void FinishShopkeeperBugleSetup(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    u32 value;
    u32 byte;
    s = &gCurrentSprite;
    timer = &s->work2;
    value = *timer - 1;
    *timer = value;
    byte = (u8)value;
    if (byte != 0) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
    } else {
        if (gCurrentPassage == 5) {
            ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
            s->pose = 120;
        } else {
            s->pose = 117;
            value = (u32)s;
            value += 39;
            *(volatile u8*)value = byte;
            value++;
            *(volatile u8*)value = byte;
            ShopkeeperUpdateGraphicsScript(sShopkeeperCarryItemGraphicsScript);
            *timer = 250;
            m4aSongNumStart(685);
        }
    }
}

void MoveBugleWithShopkeeper(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    u8* timer;
    u32 slot;
    u32 byte;
    struct PrimarySpriteData* child;
    struct PrimarySpriteData* base;

    s = &gCurrentSprite;
    slot = s->work3;
    timer = &s->work2;
    value = *timer - 1;
    *timer = value;
    byte = (u8)value;
    if (byte != 0) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperCarryItemGraphicsScript);
        PlayShopkeeperCarrySoundCue();
        value = gCurrentPassage;
        if (value == 2) {
            goto case2;
        }
        if (value == 4) {
            goto case4;
        }
        s->xPosition -= 2;
        base = gSpriteData;
        value = 44;
        child = (struct PrimarySpriteData*)slot;
        child = (struct PrimarySpriteData*)((u32)child * value);
        child = (struct PrimarySpriteData*)((u32)child + (u32)base);
        child->xPosition -= 2;
        goto done_move;
    case2:
        s->xPosition -= 3;
        base = gSpriteData;
        value = 44;
        child = (struct PrimarySpriteData*)slot;
        child = (struct PrimarySpriteData*)((u32)child * value);
        child = (struct PrimarySpriteData*)((u32)child + (u32)base);
        child->xPosition -= 3;
        goto done_move;
    case4:
        s->xPosition -= 4;
        base = gSpriteData;
        value = 44;
        child = (struct PrimarySpriteData*)slot;
        child = (struct PrimarySpriteData*)((u32)child * value);
        child = (struct PrimarySpriteData*)((u32)child + (u32)base);
        child->xPosition -= 4;
    done_move:;
    } else {
        s->status = byte;
        child = gSpriteData;
        value = 44;
        value *= slot;
        value += (u32)child;
        *(volatile u16*)value = byte;
    }
}

void MoveShopkeeperVizormanIntoPosition(void)
{
    register s32 r0 PIN("r0");
    u32 r1;
    u32 r2;
    u32 r4;
    struct PrimarySpriteData* r5;
    register u32 r6 PIN("r6");
    volatile u8* passagePtr;

    passagePtr = &gCurrentPassage;
    r0 = *passagePtr;
    if (r0 == 5) {
        r0 = *(volatile u8*)&gBossState;
        if (r0 == 0) {
            goto end;
        }
    }
    r5 = &gCurrentSprite;
    r0 = (u32)&r5->work3;
    r6 = *(u8*)r0;
    r1 = r5->xPosition;
    r1 >>= 2;
    r0 = gBg1XPosition;
    r0 >>= 2;
    r1 -= r0;
    r1 <<= 16;
    r4 = r1 >> 16;
    r0 = (u32)sShopkeeperVizormanGraphicsScript;
    ShopkeeperUpdateGraphicsScript((const u8*)r0);
    PlayShopkeeperCarrySoundCue();
    r0 = *passagePtr;
    if (r0 == 3) {
        goto passage3;
    }
    if (r0 > 3) {
        goto greater3;
    }
    if (r0 == 2) {
        goto passage2;
    }
    goto passage_default;
greater3:
    if (r0 == 4) {
        goto passage4;
    }
    if (r0 == 5) {
        goto passage5;
    }
    goto passage_default;
passage2:
    r0 = r4;
    r0 -= 48;
    r0 = (u16)r0;
    if ((u32)r0 <= 192) {
        goto finish;
    }

    r0 = r5->xPosition;
    r0 += 2;
    r5->xPosition = r0;

    goto add_secondary2;
passage3:
    r0 = r4;
    r0 -= 127;
    r0 = (u16)r0;
    if ((u32)r0 <= 113) {
        goto finish;
    }
    r0 = r5->xPosition;
    r0 += 3;
    r5->xPosition = r0;
    r2 = (u32)gSpriteData;
    r0 = 44;
    r1 = r6;
    r1 *= r0;
    r1 += r2;
    r0 = ((struct PrimarySpriteData*)r1)->xPosition;
    r0 += 3;
    ((struct PrimarySpriteData*)r1)->xPosition = r0;
    goto end;
passage4:
    r0 = r4;
    r0 -= 96;
    r0 = (u16)r0;
    if ((u32)r0 <= 144) {
        goto finish;
    }

    r0 = r5->xPosition;
    r0 += 2;
    r5->xPosition = r0;
    ASM_BARRIER_VOLATILE(: : "i"(44));
    goto add_secondary2;
passage5:
    r0 = r4;
    r0 -= 103;
    r0 = (u16)r0;
    if ((u32)r0 <= 137) {
        goto finish;
    }

    r0 = r5->xPosition;
    r0 += 2;
    r5->xPosition = r0;
    ASM_BARRIER_VOLATILE(: : "i"(55));
    goto add_secondary2;
passage_default:
    r0 = r4;
    r0 -= 87;
    r0 = (u16)r0;
    if ((u32)r0 <= 153) {
        goto finish;
    }
    r1 = (u32)&gCurrentSprite;
    r0 = ((struct PrimarySpriteData*)r1)->xPosition;
    r0 += 2;
    ((struct PrimarySpriteData*)r1)->xPosition = r0;
add_secondary2:
    r2 = (u32)gSpriteData;
    r0 = 44;
    r1 = r6;
    r1 *= r0;
    r1 += r2;
    r0 = ((struct PrimarySpriteData*)r1)->xPosition;
    r0 += 2;
    ((struct PrimarySpriteData*)r1)->xPosition = r0;
    goto end;
finish:
    r4 = (u32)&gCurrentSprite;
    r1 = 0;
    r0 = 111;
    ((struct PrimarySpriteData*)r4)->pose = r0;
    r0 = r4;
    r0 += 39;
    *(u8*)r0 = r1;
    r0++;
    *(u8*)r0 = r1;
    r0 = (u32)sShopkeeperItemReadyGraphicsScript;
    ShopkeeperUpdateGraphicsScript((const u8*)r0);
    r1 = r4;
    r1 += 41;
    r0 = 120;
    *(u8*)r1 = r0;
end:;
}

void WaitForShopkeeperVizormanReady(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    u32 value;
    u32 next;
    register u32 byte PIN("r2");
    u32 slot;
    s = &gCurrentSprite;
    slot = s->work3;
    timer = &s->work2;
    value = *timer;
    next = value - 1;
    *timer = next;
    value = next << 24;
    byte = value >> 24;
    if (byte != 0) {
        value = byte;
        ASM_BARRIER(: "+r"(value));
        if (value == 50) {
            register struct PrimarySpriteData* base PIN("r1");
            base = gSpriteData;
            value = 44;
            value *= slot;
            value += (u32)base;
            next = 17;
            *(volatile u8*)(value + 28) = next;
        }
        ShopkeeperUpdateGraphicsScript(sShopkeeperItemReadyGraphicsScript);
        PlayShopkeeperReadySoundCue();
    } else {
        value = 112;
        s->pose = value;
        value = 210;
        *timer = value;
        value = (u32)s;
        value += 39;
        *(volatile u8*)value = byte;
        value++;
        *(volatile u8*)value = byte;
        ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
    }
}

void FinishShopkeeperVizormanSetup(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    u32 value;
    u32 byte;
    s = &gCurrentSprite;
    timer = &s->work2;
    value = *timer - 1;
    *timer = value;
    byte = (u8)value;
    if (byte != 0) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
    } else {
        if (gCurrentPassage == 5) {
            ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
            s->pose = 120;
        } else {
            s->pose = 113;
            value = (u32)s;
            value += 39;
            *(volatile u8*)value = byte;
            value++;
            *(volatile u8*)value = byte;
            ShopkeeperUpdateGraphicsScript(sShopkeeperItemApproachGraphicsScript);
            *timer = 250;
            m4aSongNumStart(685);
        }
    }
}

void MoveVizormanWithShopkeeper(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    u8* timer;
    u32 slot;
    u32 byte;
    struct PrimarySpriteData* base;
    struct PrimarySpriteData* child;

    s = &gCurrentSprite;
    slot = s->work3;
    timer = &s->work2;
    value = *timer - 1;
    *timer = value;
    byte = (u8)value;
    if (byte != 0) {
        ShopkeeperUpdateGraphicsScript(sVizormanFollowGraphicsScript);
        PlayShopkeeperCarrySoundCue();
        value = gCurrentPassage;
        if (value == 2 || value == 4) {
            s->xPosition -= 4;
            base = gSpriteData;
            value = 44;
            child = (struct PrimarySpriteData*)slot;
            child = (struct PrimarySpriteData*)((u32)child * value);
            child = (struct PrimarySpriteData*)((u32)child + (u32)base);
            child->xPosition -= 4;
        } else {
            s->xPosition -= 3;
            base = gSpriteData;
            value = 44;
            child = (struct PrimarySpriteData*)slot;
            child = (struct PrimarySpriteData*)((u32)child * value);
            child = (struct PrimarySpriteData*)((u32)child + (u32)base);
            child->xPosition -= 3;
        }
    } else {
        s->status = byte;
        child = gSpriteData;
        value = 44;
        value *= slot;
        value += (u32)child;
        *(volatile u16*)value = byte;
    }
}

void MoveShopkeeperBlastCannonIntoPosition(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    u32 slot;
    u32 temp;

    value = gCurrentPassage;
    if (value == 5 && *(u8*)&gBossState == 0) {
        return;
    }
    s = &gCurrentSprite;
    value = (u32)s + 42;
    slot = *(u8*)value;
    value = s->xPosition;
    value >>= 2;
    temp = gBg1XPosition;
    temp >>= 2;
    value -= temp;
    value <<= 16;
    temp = 0xFFA90000;
    value += temp;
    value >>= 16;
    if (value > 153) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperItemApproachGraphicsScript);
        value = s->xPosition;
        value += 2;
        s->xPosition = value;
        {
            register struct PrimarySpriteData* base PIN("r2");
            base = gSpriteData;
            value = 44;
            temp = slot;
            temp *= value;
            temp += (u32)base;
            value = ((struct PrimarySpriteData*)temp)->xPosition;
            value += 2;
            ((struct PrimarySpriteData*)temp)->xPosition = value;
        }
        PlayShopkeeperCarrySoundCue();
    } else {
        temp = 0;
        value = 20;
        s->pose = value;
        value = (u32)s;
        value += 39;
        *(u8*)value = temp;
        value++;
        *(u8*)value = temp;
        ShopkeeperUpdateGraphicsScript(sShopkeeperItemReadyGraphicsScript);
        temp = (u32)s;
        temp += 41;
        value = 120;
        *(u8*)temp = value;
    }
}

void WaitForShopkeeperBlastCannonReady(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    u32 next;
    u8* timer;
    u32 slot;
    s = &gCurrentSprite;
    slot = s->work3;
    timer = &s->work2;
    value = *timer;
    next = value - 1;
    *timer = next;
    value = next << 24;
    if (value != 0) {
        value >>= 24;
        if (value == 50) {
            struct PrimarySpriteData* base;
            base = gSpriteData;
            value = 44;
            value *= slot;
            value += (u32)base;
            next = 17;
            *(volatile u8*)(value + 28) = next;
        }
    } else {
        if (gCurrentPassage == 5) {
            value = 120;
        } else {
            value = 21;
        }
        s->pose = value;
    }
    ShopkeeperUpdateGraphicsScript(sShopkeeperItemReadyGraphicsScript);
    PlayShopkeeperReadySoundCue();
}

void StartShopkeeperBlastCannonAttack(void)
{
    struct PrimarySpriteData* s;
    register u32 zero PIN("r1");
    u32 value;
    register u8* p PIN("r0");
    register u8* p2 PIN("r1");
    s = &gCurrentSprite;
    zero = 0;
    value = 22;
    s->pose = value;
    p = &s->work0;
    *p = zero;
    p++;
    *p = zero;
    p2 = &s->work2;
    value = 250;
    *p2 = value;
    m4aSongNumStart(685);
}

void MoveBlastCannonWithShopkeeper(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    u8* timer;
    u32 slot;
    u32 byte;
    s = &gCurrentSprite;
    slot = s->work3;
    timer = &s->work2;
    value = *timer - 1;
    *timer = value;
    byte = (u8)value;
    if (byte != 0) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperCarryItemGraphicsScript);
        s->xPosition -= 2;
        {
            struct PrimarySpriteData* child;
            register struct PrimarySpriteData* base PIN("r2");
            base = gSpriteData;
            value = 44;
            child = (struct PrimarySpriteData*)slot;
            child = (struct PrimarySpriteData*)((u32)child * value);
            child = (struct PrimarySpriteData*)((u32)child + (u32)base);
            child->xPosition -= 2;
        }
        PlayShopkeeperCarrySoundCue();
    } else {
        s->status = byte;
        {
            register struct PrimarySpriteData* base PIN("r1");
            base = gSpriteData;
            value = 44;
            value *= slot;
            value += (u32)base;
            *(volatile u16*)value = byte;
        }
    }
}

void MoveShopkeeperAppleBombIntoPosition(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    u32 slot;
    u32 temp;

    value = gCurrentPassage;
    if (value == 5 && *(u8*)&gBossState == 0) {
        return;
    }
    s = &gCurrentSprite;
    value = (u32)s + 42;
    slot = *(u8*)value;
    value = s->xPosition;
    value >>= 2;
    temp = gBg1XPosition;
    temp >>= 2;
    value -= temp;
    value <<= 16;
    temp = 0xFFB90000;
    value += temp;
    value >>= 16;
    if (value > 169) {
        ShopkeeperUpdateGraphicsScript(sShopkeeperAppleBombGraphicsScript);
        value = s->xPosition;
        value += 2;
        s->xPosition = value;
        {
            register struct PrimarySpriteData* base PIN("r2");
            base = gSpriteData;
            value = 44;
            temp = slot;
            temp *= value;
            temp += (u32)base;
            value = ((struct PrimarySpriteData*)temp)->xPosition;
            value += 2;
            ((struct PrimarySpriteData*)temp)->xPosition = value;
        }
        PlayShopkeeperCarrySoundCue();
    } else {
        temp = 0;
        value = 18;
        s->pose = value;
        value = (u32)s;
        value += 39;
        *(u8*)value = temp;
        value++;
        *(u8*)value = temp;
        ShopkeeperUpdateGraphicsScript(sAppleBombPresentationGraphicsScript);
        temp = (u32)s;
        temp += 41;
        value = 179;
        *(u8*)temp = value;
        {
            struct PrimarySpriteData* base;
            base = gSpriteData;
            value = 44;
            value *= slot;
            value += (u32)base;
            temp = 17;
            ((struct PrimarySpriteData*)value)->pose = temp;
        }
    }
}

void UpdateShopkeeperAppleBombPresentation(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    u32 slot;
    register u32 temp PIN("r2");

    s = &gCurrentSprite;
    value = (u32)s + 42;
    slot = *(u8*)value;
    temp = (u32)s + 41;
    value = *(u8*)temp;
    value -= 1;
    *(u8*)temp = value;
    value <<= 24;
    if (value == 0) {
        value = gCurrentPassage;
        if (value == 5) {
            value = 120;
            s->pose = value;
        } else {
            value = 25;
            s->pose = value;
            m4aSongNumStart(685);
        }
    }
    {
        register struct PrimarySpriteData* sp PIN("r12");
        sp = &gCurrentSprite;
        value = sp->work1;
        if (value == 0) {
            value = sp->work0;
            if (value == 1) {
                struct PrimarySpriteData* q;
                q = sp;
                temp = q->xPosition;
                value = temp;
                value -= 28;
                q->xPosition = value;
                {
                    struct PrimarySpriteData* base;
                    struct PrimarySpriteData* child;
                    base = gSpriteData;
                    value = 44;
                    child = (struct PrimarySpriteData*)(slot * value + (u32)base);
                    temp -= 84;
                    child->xPosition = temp;
                    {
                        register struct PrimarySpriteData* q2 PIN("r2");
                        q2 = sp;
                        temp = q2->yPosition;
                    }
                    value = temp;
                    value -= 64;
                    child->yPosition = value;
                }
            } else if (value == 2) {
                temp = sp->xPosition;
                value = temp;
                value += 52;
                sp->xPosition = value;
                {
                    struct PrimarySpriteData* base;
                    struct PrimarySpriteData* child;
                    base = gSpriteData;
                    value = 44;
                    child = (struct PrimarySpriteData*)(slot * value + (u32)base);
                    temp += 100;
                    child->xPosition = temp;
                    {
                        register struct PrimarySpriteData* q2 PIN("r2");
                        q2 = sp;
                        temp = q2->yPosition;
                    }
                    value = temp;
                    value -= 96;
                    child->yPosition = value;
                    value = 19;
                    child->pose = value;
                }
            } else if (value == 3) {
                struct PrimarySpriteData* q;
                q = sp;
                value = q->xPosition;
                value += 12;
                q->xPosition = value;
            } else if (value == 4) {
                register struct PrimarySpriteData* q2 PIN("r2");
                q2 = sp;
                value = q2->xPosition;
                value += 4;
                q2->xPosition = value;
            }
        }
    }
    ShopkeeperUpdateGraphicsScript(sAppleBombPresentationGraphicsScript);
}

void BeginShopkeeperExit(void)
{
    gCurrentSprite.work0 = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.pose = 121;
}

void UpdateShopkeeperExit(void)
{
    ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
}

void BeginShopkeeperItemWait(void)
{
    gCurrentSprite.work0 = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 250;
    gCurrentSprite.pose = 26;
}

void UpdateShopkeeperItemWait(void)
{
    struct PrimarySpriteData* sprite;
    register u8* timer PIN("r1");
    u32 value;

    sprite = &gCurrentSprite;
    timer = &sprite->work2;
    value = *timer - 1;
    *timer = value;
    value = (u8)value;
    if (value != 0) {
        sprite->xPosition -= 2;
        ShopkeeperUpdateGraphicsScript(sShopkeeperDefaultGraphicsScript);
    } else {
        sprite->status = value;
    }
}

void ReplaceShopkeeperWithTransitionSprite(void)
{
    gCurrentSprite.work0 = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.pose = 24;
}

void BeginShopkeeperPostItemIdle(void)
{
    ShopkeeperUpdateGraphicsScript(sShopkeeperAttackGraphicsScript);
}

void UpdateShopkeeperPostItemIdle(void)
{
    struct PrimarySpriteData* sprite;
    u32 status;
    u32 value;
    u32 zero;

    sprite = &gCurrentSprite;
    status = sprite->status;
    value = 4;
    zero = 0;
    value |= status;
    sprite->status = value;
    SpawnHighPriorityPrimarySprite(10, sprite->roomSlot, 0, sprite->yPosition, sprite->xPosition);
    sprite->status = zero;
}

void InitShopkeeperTransition(void)
{
    struct ShopkeeperDmaRegisters* dma;
    register u32 r0 PIN("r0");
    u32 r1;
    register u32 r2 PIN("r2");
    struct PrimarySpriteData* s;
    register u32 status PIN("r4");
    register u32 zero PIN("r5");
    dma = (struct ShopkeeperDmaRegisters*)0x040000D4;
    r0 = (u32)sShopkeeperItemTiles;
    GP_SET(dma->src, (const void*)r0);
    r0 = 0x06013000;
    GP_SET(dma->dst, (void*)r0);
    r0 = 0x80000800;
    dma->control = r0;
    r0 = dma->control;
    s = &gCurrentSprite;
    r1 = s->status;
    r2 = 129;
    r2 <<= 3;
    ASM_BARRIER(: "+r"(r2));
    r0 = r2;

    zero = 0;

    r2 = 0;

    status = r0;

    status |= r1;
    r0 = 0xFFFB;
    status &= r0;
    s->status = status;
    s->warioCollision = zero;
    s->drawDistanceDown = 32;
    s->drawDistanceUp = zero;
    s->drawDistanceLeftRight = 16;
    s->hitboxExtentUp = 4;
    s->hitboxExtentDown = 4;
    s->hitboxExtentLeft = 4;
    s->hitboxExtentRight = 4;
    GP_SET(s->pOamData, sShopkeeperTransitionInitialOam);
    s->currentAnimationFrame = zero;
    s->animationTimer = r2;
    s->work3 = zero;
    s->pose = 18;
    r1 = s->xPosition;
    r1 >>= 2;
    r0 = gBg1XPosition;
    r0 >>= 2;
    r1 -= r0;
    r1 <<= 16;
    r1 >>= 16;
    r2 = (u32)s;

    if (r1 > 120) {
        r1 = 0x40;
        r0 = status;
        r0 |= r1;
    } else {
        r0 = 0xFFBF;
        r0 &= status;
    }
    *(u16*)r2 = r0;
    {
        u32 y = ((struct PrimarySpriteData*)r2)->yPosition - 36;
        u32 x = ((struct PrimarySpriteData*)r2)->xPosition;
        SpriteSpawnAsChild(11, 0, 0, y, x);
    }
}

void UpdateShopkeeperTransitionFall(void)
{
    register struct PrimarySpriteData* s PIN("r2");
    register u8* indexPtr PIN("r12");
    u32 index;
    const s16* table;
    register u32 value PIN("r0");
    s32 signedValue;
    u32 velocity;
    struct PrimarySpriteData* sp;
    s = &gCurrentSprite;
    indexPtr = &s->work3;
    index = *indexPtr;
    table = sShopkeeperTransitionFallVelocityTable;
    value = index << 1;
    value += (u32)table;
    velocity = *(u16*)value;
    signedValue = *(s16*)value;
    value = 0x7FFF;
    sp = s;
    if (signedValue == value) {
        signedValue = index - 1;
        signedValue <<= 1;
        signedValue += (u32)table;
        value = sp->yPosition;
        signedValue = *(u16*)signedValue;
        value += signedValue;
    } else {
        value = index + 1;
        *indexPtr = value;
        value = sp->yPosition;
        value += velocity;
    }
    sp->yPosition = value;
    value = sp->status & 0x40;
    if (value != 0) {
        value = sp->xPosition;
        value -= 8;
    } else {
        value = sp->xPosition;
        value += 8;
    }
    sp->xPosition = value;
    value = func_8023A60(sp->yPosition, sp->xPosition);
    index = value;
    if (gSpriteCollisionResult != 0) {
        register u32 zero PIN("r1");
        register u32 zeroHalf PIN("r2");
        zero = 0;
        zeroHalf = 0;
        sp->yPosition = index;
        sp->pose = 111;
        sp->work0 = 8;
        GP_SET(sp->pOamData, sShopkeeperTransitionImpactOam);
        sp->currentAnimationFrame = zero;
        sp->animationTimer = zeroHalf;
        value = 27;
        m4aSongNumStart(value);
    }
}

void FinishShopkeeperTransitionWait(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    u32 value;
    u32 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    zero = (u8)value;
    if (zero == 0) {
        GP_SET(sprite->pOamData, sShopkeeperSmileDropOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
        *timer = 60;
        sprite->pose = 113;
    }
}

void StartShopkeeperTransitionLanding(void)
{
    struct PrimarySpriteData* sprite;
    u8* timer;
    u32 value;
    u32 zero;

    sprite = &gCurrentSprite;
    timer = &sprite->work0;
    value = *timer - 1;
    *timer = value;
    zero = (u8)value;
    if (zero == 0) {
        GP_SET(sprite->pOamData, sShopkeeperTransitionLandingOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = zero;
        *timer = 60;
        sprite->pose = 115;
        {
            register u32 distance PIN("r1");
            u32 bg;
            distance = sprite->xPosition >> 2;
            bg = gBg1XPosition >> 2;
            distance -= bg;
            distance = (u16)distance;
            if (distance > 120) {
                sprite->status |= 0x40;
            } else {
                sprite->status &= 0xFFBF;
            }
        }
    }
}

void UpdateShopkeeperTransitionRun(void)
{
    register struct PrimarySpriteData* s PIN("r2");
    register u32 value PIN("r0");
    u32 tmp;
    struct PrimarySpriteData* sp;
    s = &gCurrentSprite;
    tmp = s->status;
    value = tmp & 0x40;
    sp = s;
    if (value != 0) {
        value = sp->xPosition;
        value -= 4;
    } else {
        value = sp->xPosition;
        value += 4;
    }
    sp->xPosition = value;
    {
        register struct PrimarySpriteData* p PIN("r4");
        p = sp;
        value = *(volatile u32*)((u32)p + 20);
        tmp = 0xFFFFFF;
        value &= tmp;
        tmp = 0x10002;
        if (value == tmp) {
            value = 27;
            m4aSongNumStart(value);
        }
        value = p->xPosition;
        value >>= 2;
        tmp = gBg1XPosition;
        tmp >>= 2;
        value -= tmp;
        tmp = (u16)value;
        if (tmp == 80 || tmp == 152) {
            GP_SET(p->pOamData, sShopkeeperTransitionRunOam);
            value = 0;
            *(volatile u8*)((u32)p + 22) = value;
            {
                register u32 zero2 PIN("r2");
                zero2 = 0;

                p->animationTimer = value;
                value = 116;
                p->pose = value;
                value = (u32)p + 39;
                *(volatile u8*)value = zero2;
            }
            if (tmp > 120) {
                value = p->status;
                tmp = 64;
                value |= tmp;
                p->status = value;
            } else {
                tmp = sp->status;
                value = 0xFFBF;
                value &= tmp;
                sp->status = value;
            }
        }
    }
}

void WaitForWarioTransitionPose(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    register u32 t PIN("r1");
    register u32 value PIN("r0");
    s = &gCurrentSprite;
    timer = &s->work0;
    t = *timer;
    if (t == 0) {
        value = gWarioData.pose;
        if (value == 65) {
            value = t + 1;
            *timer = value;
            value = 489;
            m4aSongNumStart(value);
        }
    } else {
        value = t + 1;
        t = 0;
        *timer = value;
        value = (u8)value;
        if (value > 89) {
            GP_SET(s->pOamData, sShopkeeperTransitionWaitOam);
            *(volatile u8*)((u32)s + 22) = t;
            *(volatile u16*)((u32)s + 20) = t;
            s->pose = 117;
            t = (u32)&gWarioData;
            value = s->xPosition;
            t = *(volatile u16*)(t + 18);
            if (value > t) {
                value = s->status;
                t = 0x40;
                value |= t;
            } else {
                t = s->status;
                value = 0xFFBF;
                value &= t;
            }
            s->status = value;
        }
    }
}

void BeginShopkeeperTransformationReturn(void)
{
    u32 value;
    register u32 sprite PIN("r1");

    value = gBossSequenceState;
    if (value > 15) {
        sprite = (u32)&gCurrentSprite;
        value = (u32)sShopkeeperSmileDropOam;
        GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(sprite + 4), (const struct AnimationFrame*)value);
        value = 0;
        *(volatile u8*)(sprite + 22) = value;
        *(volatile u16*)(sprite + 20) = value;
        value = 119;
        *(volatile u8*)(sprite + 28) = value;
    }
}

void BeginShopkeeperTransitionReturn(void)
{
    u32 value;
    u32 temp;

    value = gBossSequenceState;
    if (value == 17) {
        value = (u32)&gCurrentSprite;
        temp = (u32)sShopkeeperTransitionRunOam;
        GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(value + 4), (const struct AnimationFrame*)temp);
        temp = 0;
        *(volatile u8*)(value + 22) = temp;
        *(volatile u16*)(value + 20) = temp;
        temp = 121;
        *(volatile u8*)(value + 28) = temp;
        value += 39;
        temp = 60;
        *(volatile u8*)value = temp;
    }
}

void UpdateShopkeeperTransitionDeparture(void)
{
    struct PrimarySpriteData* s;
    u32 saved;
    register u32 value PIN("r0");
    u32 temp;
    s = &gCurrentSprite;
    {
        u8* timer;
        timer = &s->work0;
        value = *timer;
        saved = value;
        if (saved != 0) {
            value--;
            *timer = value;
            temp = (u8)value;
            if (temp == 0) {
                GP_SET(s->pOamData, sShopkeeperIdleOam);
                s->currentAnimationFrame = temp;
                s->animationTimer = temp;
                s->status &= 0xFFBF;
            }
            goto done;
        }
    }
    if ((*(u32*)&s->animationTimer & 0xFFFFFF) == 0x10002) {
        m4aSongNumStart(27);
    }
    {
        register u32 x PIN("r0");
        u32 bg;
        x = s->xPosition;
        x += 6;
        s->xPosition = x;
        x <<= 16;
        x >>= 18;
        bg = gBg1XPosition;
        bg >>= 2;
        x -= bg;
        x = (u16)x;
        if (x > 212) {
            GP_SET(s->pOamData, sShopkeeperTransitionDepartureOam);
            x = 0;
            s->currentAnimationFrame = x;
            s->animationTimer = saved;
            x = 123;
            s->pose = x;
            x = 246;
            x <<= 1;
            m4aSongNumStart(x);
        }
    }
done:;
}

void InitAppleBomb(void)
{
    register volatile u8* s PIN("r12");
    register u32 value PIN("r0");
    u32 temp;
    register u32 flag PIN("r2");
    u32 zeroHalf;
    u32 down;
    u32 side;
    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;
    flag = 0x80;
    flag <<= 3;
    ASM_BARRIER(: "+r"(flag));
    value = flag;
    flag = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)s;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = flag;
    value = (u32)s + 32;
    down = 24;
    *(volatile u8*)value = down;
    value++;
    *(volatile u8*)value = down;
    value++;
    side = 16;
    *(volatile u8*)value = side;
    value++;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sAppleBombIdleOam;
    temp = (u32)s;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = flag;
    *(volatile u16*)(temp + 20) = zeroHalf;
    if (gCurrentRoom == 0) {
        *(volatile u8*)(temp + 28) = side;
    } else {
        flag = (u32)s;
        *(volatile u8*)(flag + 28) = down;
    }
}

void StartAppleBombFuse(void)
{
    struct PrimarySpriteData* s;
    s = &gCurrentSprite;
    GP_SET(s->pOamData, sAppleBombFuseOam);
    s->currentAnimationFrame = 0;
    s->animationTimer = 0;
    s->work0 = 58;
    s->pose = 18;
}

void UpdateAppleBombFuse(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    register u32 v PIN("r0");
    u32 z;
    s = &gCurrentSprite;
    p = &s->work0;
    v = *p;
    v--;
    *p = v;
    z = (u8)v;
    if (z != 0) {
        v = *(u32*)((u8*)s + 20);
        v &= 0xFFFFFF;
        if (v == 0x10001) {
            m4aSongNumStart(502);
        }
    } else {
        s->pose = 24;
        GP_SET(s->pOamData, sAppleBombExplosionOam);
        s->currentAnimationFrame = z;
        s->animationTimer = z;
    }
}

void StartAppleBombExplosion(void)
{
    volatile u8* s;
    u32 p;
    register u32 z PIN("r1");
    register u32 v PIN("r0");
    s = (volatile u8*)&gCurrentSprite;
    p = (u32)s;
    p += 39;
    z = 0;
    v = 31;
    *(volatile u8*)p = v;
    v = (u32)s;
    v += 42;
    *(volatile u8*)v = z;
    v = 20;
    s[28] = v;
    m4aSongNumStart(503);
}

void UpdateAppleBombFlight(void)
{
    u32 r0;
    u32 r1;
    u32 r2;
    struct PrimarySpriteData* r3;
    u32 r4;
    register const s16* r5 PIN("r5");
    register u32 r6 PIN("r6");
    register u8* ip PIN("r12");
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    r1 = (u32)&s->work0;
    r0 = *(u8*)r1;
    if (r0 == 0) {
        goto finish;
    }

    r0--;
    *(u8*)r1 = r0;
    r1 = 7;
    r0 &= r1;
    if (r0 == 0) {
        r1 = s->roomSlot;
        ASM_BARRIER_VOLATILE(: : "r"(r1));
        r3 = (struct PrimarySpriteData*)(u32)s->yPosition;
        r3 = (struct PrimarySpriteData*)((u32)r3 - 32);
        r0 = s->xPosition;
        SpawnHighPriorityPrimarySprite(12, r1, 0, (u32)r3, r0);
    }

    r0 = gCurrentPassage;
    r0--;
    switch (r0) {
        case 0:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r0 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            r0 += (u32)r3;
            ip = (u8*)r0;
            r2 = *(u8*)r0;
            r5 = sAppleBombPassage2FlightPath;
        table_common:
            r0 = r2 << 1;
            r0 += (u32)r5;
            r4 = *(u16*)r0;
            r6 = 0;
            r1 = *(s16*)(r0 + r6);
            r0 = 0x7FFF;
            if ((s32)r1 == (s32)r0) {
                goto sentinel;
            }
            goto normal;
        case 1:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sAppleBombPassage3FlightPath;
            goto table_common;
        case 2:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 16;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sAppleBombPassage4FlightPath;
            goto table_common;
        case 3:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sAppleBombPassage5FlightPath;
            goto table_common;
        case 4:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sAppleBombDefaultFlightPath;
            r0 = r2 << 1;
            r0 += (u32)r5;
            r4 = *(u16*)r0;
            r6 = 0;
            r1 = *(s16*)(r0 + r6);
            r0 = 0x7FFF;
            if ((s32)r1 != (s32)r0) {
                goto normal;
            }
            goto sentinel;
        default:
            goto fallback;
    }

sentinel:
    r1 = r2 - 1;
    r1 <<= 1;
    r1 += (u32)r5;
    r0 = r3->yPosition;
    r1 = *(u16*)r1;
    r0 += r1;
    r3->yPosition = r0;
    goto end;
normal:
    r0 = r2 + 1;
    r1 = (u32)ip;
    *(u8*)r1 = r0;
    r0 = r3->yPosition;
    r0 += r4;
    r3->yPosition = r0;
    goto end;

fallback:
    r0 = s->xPosition;
    r0 += 12;
    s->xPosition = r0;
    r6 = 42;
    ASM_BARRIER_VOLATILE(: "+r"(r6));
    r6 += (u32)s;
    ip = (u8*)r6;
    r2 = *(u8*)r6;
    r5 = sAppleBombPassage1FlightPath;
    r0 = r2 << 1;
    r0 += (u32)r5;
    r3 = (struct PrimarySpriteData*)(u32) * (u16*)r0;
    r6 = 0;
    r1 = *(s16*)(r0 + r6);
    r0 = 0x7FFF;
    if ((s32)r1 == (s32)r0) {
        r1 = r2 - 1;
        r1 <<= 1;
        r1 += (u32)r5;
        r0 = s->yPosition;
        r1 = *(u16*)r1;
        r0 += r1;
        s->yPosition = r0;
    } else {
        r0 = r2 + 1;
        r1 = (u32)ip;
        *(u8*)r1 = r0;
        r0 = s->yPosition;
        r0 += (u32)r3;
        s->yPosition = r0;
    }
    goto end;

finish:
    r0 = 21;
    s->pose = r0;
    r0 = 60;
    *(u8*)r1 = r0;
    r0 = s->yPosition;
    r1 = s->xPosition;
    r2 = 51;
    SpawnSecondarySprite(r0, r1, r2);
    func_8026E78();
    r0 = 252;
    r0 <<= 1;
    m4aSongNumStart(r0);
end:;
}

void UpdateAppleBombBurst(void)
{
    u32 r0;
    register u32 r1 PIN("r1");
    struct PrimarySpriteData* r2;
    struct PrimarySpriteData* r3;
    register u8* ip PIN("r12");

    r3 = &gCurrentSprite;
    r0 = 39;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r0 += (u32)r3;
    ip = (u8*)r0;
    r0 = *(u8*)r0;
    r1 = r0;
    r2 = r3;
    if (r1 == 0) {
        goto timer_zero;
    }
    r0--;
    r1 = (u32)ip;
    *(u8*)r1 = r0;
    r0 = *(u8*)r1;

    switch (r0) {
        case 55:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto y_to_r3_x_minus;
        case 50:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 += 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);

            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_to_r1;
        case 40:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);

            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_to_r1;
        case 29:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            goto y_to_r3_x_minus;
        case 25:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 += 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);

            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_to_r1;
        case 20:
        case 45:
            r3 = (struct PrimarySpriteData*)(u32)r2->yPosition;
            r1 = r2->xPosition;
            goto spawn;
        case 15:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);
            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            goto x_to_r1;
        case 10:
        case 35:
            r0 = r2->yPosition;
            r0 += 32;
            goto y_to_r3_x_minus;
        case 58:
            goto status_case;
        default:
            break;
    }
    goto end;
y_to_r3_x_minus:
    r0 <<= 16;
    r3 = (struct PrimarySpriteData*)(r0 >> 16);
    r0 = r2->xPosition;
    r0 -= 32;
x_to_r1:
    r0 <<= 16;
    r1 = r0 >> 16;
    goto spawn;
status_case:
    r1 = r2->status;
    r0 = 4;
    r0 |= r1;
    r2->status = r0;
    goto end;
spawn:
    r0 = (u32)r3;
    r2 = (struct PrimarySpriteData*)51;
    SpawnSecondarySprite(r0, r1, (u8)(u32)r2);
    goto end;
timer_zero:
    r3->status = r1;
end:;
}

void InitPurchasedItemDisplay(void)
{
    register volatile u8* s PIN("r12");
    register u32 value PIN("r0");
    u32 temp;
    register u32 flag PIN("r2");
    u32 zero;
    register u32 zeroHalf PIN("r4");

    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;

    flag = 0x80;
    flag <<= 3;
    ASM_BARRIER(: "+r"(flag));
    value = flag;
    ASM_BARRIER(: "+r"(value));
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)s;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    value = (u32)s;
    value += 32;
    flag = 16;
    *(volatile u8*)value = flag;
    value++;
    *(volatile u8*)value = flag;
    value++;
    *(volatile u8*)value = flag;
    value++;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)s;
    *(volatile u8*)(value + 22) = zero;
    *(volatile u16*)(value + 20) = zeroHalf;
    *(volatile u8*)(value + 28) = flag;
    temp = (u32)s;
    temp += 39;
    value = 19;
    *(volatile u8*)temp = value;

    value = gCurrentShopItem;
    value -= 1;
    flag = (u32)s;
    switch (value) {
        case 0:
            GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(flag + 4), sPurchasedAppleBombOam);
            break;
        case 1:
            GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(flag + 4), sPurchasedBlastCannonOam);
            break;
        case 4:
            GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(flag + 4), sPurchasedBlackDogOam);
            break;
        case 6:
            GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(flag + 4), sPurchasedBigFistOam);
            break;
        case 7:
            GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(flag + 4), sPurchasedBlackDragonOam);
            break;
        default:
            *(volatile u16*)flag = 0;
            break;
    }
}

void SpriteAppleBomb(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitAppleBomb();
            break;
        case 16:
            ShopItemFollowShopkeeper();
            break;
        case 17:
            StartAppleBombFuse();
        case 18:
            UpdateAppleBombFuse();
            break;
        case 19:
            StartAppleBombExplosion();
        case 20:
            UpdateAppleBombFlight();
            break;
        case 21:
            UpdateAppleBombBurst();
            break;
    }
}

void SpritePurchasedItemDisplay(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    u32 v;
    s = &gCurrentSprite;
    s->disableWarioCollisionTimer = 1;
    if (s->pose == 0) {
        InitPurchasedItemDisplay();
    }
    p = &s->work0;
    v = *p - 1;
    *p = v;
    v = (u8)v;
    if (v == 0) {
        s->status = v;
    }
}

void InitBlastCannon(void)
{
    register volatile u8* s PIN("r12");
    register u32 value PIN("r0");
    u32 temp;
    register u32 flag PIN("r2");
    u32 zeroHalf;
    u32 down;
    u32 side;
    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;
    flag = 0x80;
    flag <<= 3;
    ASM_BARRIER(: "+r"(flag));
    value = flag;
    flag = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)s;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = flag;
    value = (u32)s + 32;
    down = 24;
    *(volatile u8*)value = down;
    value++;
    *(volatile u8*)value = down;
    value++;
    side = 16;
    *(volatile u8*)value = side;
    value++;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sBlastCannonIdleOam;
    temp = (u32)s;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = flag;
    *(volatile u16*)(temp + 20) = zeroHalf;
    if (gCurrentRoom == 0) {
        *(volatile u8*)(temp + 28) = side;
    } else {
        flag = (u32)s;
        *(volatile u8*)(flag + 28) = down;
    }
}

void StartBlastCannonCountdown(void)
{
    struct PrimarySpriteData* s;
    s = &gCurrentSprite;
    GP_SET(s->pOamData, sBlastCannonCountdownOam);
    s->currentAnimationFrame = 0;
    s->animationTimer = 0;
    s->work0 = 64;
    s->pose = 18;
}

void UpdateBlastCannonCountdown(void)
{
    struct PrimarySpriteData* s;
    register u32 old PIN("r0");
    register u32 dec PIN("r1");
    register u32 value PIN("r2");
    u8* p;
    s = &gCurrentSprite;
    p = &s->work0;

    old = *p;
    dec = old - 1;
    *p = dec;
    old = dec << 24;
    value = old >> 24;
    if (value == 0) {
        goto zero;
    }
    ASM_BARRIER(: "=r"(old) : "0"(value));
    if (old != 32) {
        goto done;
    }
    SpawnHighPriorityPrimarySprite(197, s->roomSlot, 0, s->yPosition - 124, s->xPosition + 64);
    m4aSongNumStart(505);
    goto done;
zero:
    old = 24;
    s->pose = old;
    GP_SET(s->pOamData, sBlastCannonIdleOam);
    s->currentAnimationFrame = value;
    s->animationTimer = value;
done:;
}

void InitBlastCannonball(void)
{
    register volatile u8* s PIN("r12");
    register u32 value PIN("r0");
    u32 temp;
    register u32 flag PIN("r2");
    u32 zero;
    register u32 zeroHalf PIN("r4");
    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;

    flag = 0x80;
    flag <<= 3;
    ASM_BARRIER(: "+r"(flag));
    value = flag;
    ASM_BARRIER(: "+r"(value));
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)s;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    value = (u32)s + 32;
    flag = 16;
    *(volatile u8*)value = flag;
    value++;
    *(volatile u8*)value = flag;
    value++;
    *(volatile u8*)value = flag;
    value++;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sBlastCannonballOam;
    temp = (u32)s;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = zero;
    *(volatile u16*)(temp + 20) = zeroHalf;
    temp += 39;
    value = 31;
    *(volatile u8*)temp = value;
    value = (u32)s;
    value += 42;
    *(volatile u8*)value = zero;
    value = (u32)s;
    *(volatile u8*)(value + 28) = flag;
}

void UpdateBlastCannonballFlight(void)
{
    u32 r0;
    u32 r1;
    u32 r2;
    struct PrimarySpriteData* r3;
    u32 r4;
    register const s16* r5 PIN("r5");
    register u32 r6 PIN("r6");
    register u8* ip PIN("r12");
    struct PrimarySpriteData* s;

    s = &gCurrentSprite;
    r1 = (u32)&s->work0;
    r0 = *(u8*)r1;
    if (r0 == 0) {
        goto finish;
    }

    r0--;
    *(u8*)r1 = r0;
    r1 = 7;
    r0 &= r1;
    if (r0 == 0) {
        r1 = s->roomSlot;
        ASM_BARRIER_VOLATILE(: : "r"(r1));
        r3 = (struct PrimarySpriteData*)(u32)s->yPosition;
        r3 = (struct PrimarySpriteData*)((u32)r3 - 32);
        r0 = s->xPosition;
        SpawnHighPriorityPrimarySprite(12, r1, 0, (u32)r3, r0);
    }

    r0 = gCurrentPassage;
    r0--;
    switch (r0) {
        case 0:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r0 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            r0 += (u32)r3;
            ip = (u8*)r0;
            r2 = *(u8*)r0;
            r5 = sBlastCannonballPassage2FlightPath;
        table_common:
            r0 = r2 << 1;
            r0 += (u32)r5;
            r4 = *(u16*)r0;
            r6 = 0;
            r1 = *(s16*)(r0 + r6);
            r0 = 0x7FFF;
            if ((s32)r1 == (s32)r0) {
                goto sentinel;
            }
            goto normal;
        case 1:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sBlastCannonballPassage3FlightPath;
            goto table_common;
        case 2:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 16;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sBlastCannonballPassage4FlightPath;
            goto table_common;
        case 3:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sBlastCannonballPassage5FlightPath;
            goto table_common;
        case 4:
            r3 = &gCurrentSprite;
            r0 = r3->xPosition;
            r0 += 12;
            r3->xPosition = r0;
            r6 = 42;
            ASM_BARRIER_VOLATILE(: "+r"(r6));
            r6 += (u32)r3;
            ip = (u8*)r6;
            r2 = *(u8*)r6;
            r5 = sBlastCannonballDefaultFlightPath;
            r0 = r2 << 1;
            r0 += (u32)r5;
            r4 = *(u16*)r0;
            r6 = 0;
            r1 = *(s16*)(r0 + r6);
            r0 = 0x7FFF;
            if ((s32)r1 != (s32)r0) {
                goto normal;
            }
            goto sentinel;
        default:
            goto fallback;
    }

sentinel:
    r1 = r2 - 1;
    r1 <<= 1;
    r1 += (u32)r5;
    r0 = r3->yPosition;
    r1 = *(u16*)r1;
    r0 += r1;
    r3->yPosition = r0;
    goto end;
normal:
    r0 = r2 + 1;
    r1 = (u32)ip;
    *(u8*)r1 = r0;
    r0 = r3->yPosition;
    r0 += r4;
    r3->yPosition = r0;
    goto end;

fallback:
    r0 = s->xPosition;
    r0 += 12;
    s->xPosition = r0;
    r6 = 42;
    ASM_BARRIER_VOLATILE(: "+r"(r6));
    r6 += (u32)s;
    ip = (u8*)r6;
    r2 = *(u8*)r6;
    r5 = sBlastCannonballPassage1FlightPath;
    r0 = r2 << 1;
    r0 += (u32)r5;
    r3 = (struct PrimarySpriteData*)(u32) * (u16*)r0;
    r6 = 0;
    r1 = *(s16*)(r0 + r6);
    r0 = 0x7FFF;
    if ((s32)r1 == (s32)r0) {
        r1 = r2 - 1;
        r1 <<= 1;
        r1 += (u32)r5;
        r0 = s->yPosition;
        r1 = *(u16*)r1;
        r0 += r1;
        s->yPosition = r0;
    } else {
        r0 = r2 + 1;
        r1 = (u32)ip;
        *(u8*)r1 = r0;
        r0 = s->yPosition;
        r0 += (u32)r3;
        s->yPosition = r0;
    }
    goto end;

finish:
    r0 = 18;
    s->pose = r0;
    r0 = 60;
    *(u8*)r1 = r0;
    r0 = s->yPosition;
    r1 = s->xPosition;
    r2 = 51;
    SpawnSecondarySprite(r0, r1, r2);
    func_8026E78();
    r0 = 252;
    r0 <<= 1;
    m4aSongNumStart(r0);
end:;
}

void UpdateBlastCannonballBurst(void)
{
    u32 r0;
    register u32 r1 PIN("r1");
    struct PrimarySpriteData* r2;
    struct PrimarySpriteData* r3;
    register u8* ip PIN("r12");

    r3 = &gCurrentSprite;
    r0 = 39;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r0 += (u32)r3;
    ip = (u8*)r0;
    r0 = *(u8*)r0;
    r1 = r0;
    r2 = r3;
    if (r1 == 0) {
        goto timer_zero;
    }
    r0--;
    r1 = (u32)ip;
    *(u8*)r1 = r0;
    r0 = *(u8*)r1;

    switch (r0) {
        case 55:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto y_to_r3_x_minus;
        case 50:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 += 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);

            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_to_r1;
        case 40:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);

            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_to_r1;
        case 30:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            goto y_to_r3_x_minus;
        case 25:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 += 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);

            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_to_r1;
        case 20:
        case 45:
            r3 = (struct PrimarySpriteData*)(u32)r2->yPosition;
            r1 = r2->xPosition;
            goto spawn;
        case 15:
            r0 = *(volatile u16*)((u8*)r2 + 8);
            r0 -= 32;
            r0 <<= 16;
            r3 = (struct PrimarySpriteData*)(r0 >> 16);
            r0 = *(volatile u16*)((u8*)r2 + 10);
            r0 += 32;
            goto x_to_r1;
        case 10:
        case 35:
            r0 = r2->yPosition;
            r0 += 32;
            goto y_to_r3_x_minus;
        case 58:
            goto status_case;
        default:
            break;
    }
    goto end;
y_to_r3_x_minus:
    r0 <<= 16;
    r3 = (struct PrimarySpriteData*)(r0 >> 16);
    r0 = r2->xPosition;
    r0 -= 32;
x_to_r1:
    r0 <<= 16;
    r1 = r0 >> 16;
    goto spawn;
status_case:
    r1 = r2->status;
    r0 = 4;
    r0 |= r1;
    r2->status = r0;
    goto end;
spawn:
    r0 = (u32)r3;
    r2 = (struct PrimarySpriteData*)51;
    SpawnSecondarySprite(r0, r1, (u8)(u32)r2);
    goto end;
timer_zero:
    r3->status = r1;
end:;
}

void SpriteBlastCannon(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitBlastCannon();
            break;
        case 16:
            ShopItemFollowShopkeeper();
            break;
        case 17:
            StartBlastCannonCountdown();
        case 18:
            UpdateBlastCannonCountdown();
            break;
    }
}

void SpriteBlastCannonball(void)
{
    struct PrimarySpriteData* s;
    s32 p;
    struct PrimarySpriteData* copy;
    u8* work;
    u32 value;
    s = &gCurrentSprite;
    p = 1;
    s->disableWarioCollisionTimer = p;
    p = s->pose;
    copy = s;
    if (p == 16) {
        goto p16;
    }
    if (p > 16) {
        goto gt16;
    }
    if (p == 0) {
        goto p0;
    }
    goto other;
gt16:
    if (p == 18) {
        goto p18;
    }
    goto other;
p0:
    InitBlastCannonball();
    goto done;
p16:
    UpdateBlastCannonballFlight();
    goto done;
p18:
    UpdateBlastCannonballBurst();
    goto done;
other:
    work = &copy->work0;
    value = *work;
    value--;
    *work = value;
    value = (u8)value;
    if (value == 0) {
        copy->status = value;
    }
done:;
}

void UpdateVizormanBeamPalette(void)
{
    u32 value;
    u32 temp;
    u32 test;
    register struct ShopkeeperDmaRegisters* dma PIN("r2");
    value = gMainTimer;
    test = 1;
    test &= value;
    if (test == 0) {
        value = (u32)&gCurrentSprite;
        temp = value;
        temp += 42;
        value = *(volatile u8*)temp;
        value++;
        *(volatile u8*)temp = value;
        value = (u8)value;
        if (value > 6) {
            *(volatile u8*)temp = test;
        }
        dma = (struct ShopkeeperDmaRegisters*)0x40000D4;
        value = *(volatile u8*)temp;
        value <<= 5;
        temp = (u32)sVizormanBeamPaletteFrames;
        value += temp;
        GP_SET(dma->src, (const void*)value);
        value = 0x50003A0;
        GP_SET(dma->dst, (void*)value);
        value = 0x80000010;
        dma->control = value;
        value = dma->control;
    }
}
void InitVizorman(void)
{
    struct PrimarySpriteData* s;
    u32 input;
    register u32 value PIN("r0");
    register u32 status PIN("r4");
    register u32 zero PIN("r5");
    register u32 temp PIN("r2");
    u32 down;
    u32 side;

    s = &gCurrentSprite;
    input = s->status;
    temp = 0x80;
    temp <<= 3;
    ASM_BARRIER(: "+r"(temp));
    value = temp;
    zero = 0;
    temp = 0;
    status = value;
    status |= input;
    value = 0xFFFB;
    status &= value;
    s->status = status;
    s->warioCollision = zero;
    value = (u32)s;
    value += 32;
    down = 24;
    *(volatile u8*)value = down;
    value++;
    *(volatile u8*)value = down;
    value++;
    side = 16;
    *(volatile u8*)value = side;
    value++;
    input = 4;
    *(volatile u8*)value = input;
    value++;
    *(volatile u8*)value = input;
    value++;
    *(volatile u8*)value = input;
    value++;
    *(volatile u8*)value = input;
    value = (u32)sVizormanIdleOam;
    GP_SET(s->pOamData, (const struct AnimationFrame*)value);
    s->currentAnimationFrame = zero;
    s->animationTimer = temp;
    if (gCurrentRoom == 0) {
        s->pose = side;
    } else {
        s->pose = down;
        input = 8;
        value = status;
        value |= input;
        s->status = value;
    }
}

void SpawnVizormanBeamActors(void)
{
    struct PrimarySpriteData* s;
    s = &gCurrentSprite;
    SpawnHighPriorityPrimarySprite(198, s->roomSlot, 0, s->yPosition - 112, s->xPosition + 16);
    SpawnHighPriorityPrimarySprite(199, s->roomSlot, 0, s->yPosition - 112, s->xPosition + 16);
    s->pose = 24;
    m4aSongNumStart(506);
}

void InitVizormanBeamCore(void)
{
    volatile u8* sprite;
    register u32 value PIN("r0");
    u32 temp;
    register u32 zero PIN("r2");
    u32 zeroHalf;

    sprite = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)sprite;
    zero = 0x80;
    zero <<= 3;
    ASM_BARRIER(: "+r"(zero));
    value = zero;
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)sprite;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    temp += 32;
    value = 16;
    *(volatile u8*)temp = value;
    temp++;
    *(volatile u8*)temp = value;
    temp++;
    *(volatile u8*)temp = value;
    value = (u32)sprite + 35;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sVizormanBeamCoreInitialOam;
    temp = (u32)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = zero;
    *(volatile u16*)(temp + 20) = zeroHalf;
    value = 18;
    *(volatile u8*)(temp + 28) = value;
    temp += 39;
    value = 60;
    *(volatile u8*)temp = value;
    value = (u32)sprite + 42;
    *(volatile u8*)value = zero;
}

void StartVizormanBeamCoreActive(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    u32 v;
    s = &gCurrentSprite;
    p = &s->work0;
    v = *p - 1;
    *p = v;
    if ((u8)v == 0) {
        GP_SET(s->pOamData, sVizormanBeamCoreActiveOam);
        s->currentAnimationFrame = 0;
        s->animationTimer = 0;
        s->pose = 20;
        *p = 135;
    }
}
void StartVizormanBeamCoreEnd(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    u32 v;
    s = &gCurrentSprite;
    p = &s->work0;
    v = *p - 1;
    *p = v;
    if ((u8)v == 0) {
        GP_SET(s->pOamData, sVizormanBeamCoreEndOam);
        s->currentAnimationFrame = 0;
        s->animationTimer = 0;
        s->pose = 22;
        *p = 60;
    }
}
void UpdateVizormanBeamCoreEnd(void)
{
    struct PrimarySpriteData* s = &gCurrentSprite;
    u8* p = &s->work0;
    u32 v = *p - 1;
    *p = v;
    v = (u8)v;
    if (v == 0) {
        s->status = v;
    }
}

void InitVizormanBeamGlow(void)
{
    register volatile u8* sprite PIN("r12");
    register u32 value PIN("r0");
    u32 temp;
    register u32 zero PIN("r2");
    u32 zeroHalf;

    sprite = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)sprite;
    zero = 0x80;
    zero <<= 3;
    ASM_BARRIER(: "+r"(zero));
    value = zero;
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)sprite;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    temp += 32;
    value = 16;
    *(volatile u8*)temp = value;
    temp++;
    *(volatile u8*)temp = value;
    temp++;
    *(volatile u8*)temp = value;
    value = (u32)sprite + 35;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sVizormanBeamGlowInitialOam;
    temp = (u32)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = zero;
    *(volatile u16*)(temp + 20) = zeroHalf;
    value = 18;
    *(volatile u8*)(temp + 28) = value;
    temp += 39;
    value = 60;
    *(volatile u8*)temp = value;
}

void StartVizormanBeamGlow(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    u32 value;
    s = &gCurrentSprite;
    timer = &s->work0;
    value = *timer - 1;
    *timer = value;
    if ((s32)(value << 24) == 0) {
        if (gCurrentPassage != 0) {
            GP_SET(s->pOamData, sVizormanBeamGlowStartOam);
        } else {
            GP_SET(s->pOamData, sVizormanBeamGlowActiveOam);
        }
        s->currentAnimationFrame = 0;
        s->animationTimer = 0;
        s->pose = 20;
        s->work0 = 150;
    }
}

void UpdateVizormanBeamGlowLifetime(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    register u32 old PIN("r0");
    register u32 n PIN("r1");
    s = &gCurrentSprite;
    p = &s->work0;
    old = *p;
    n = old - 1;
    *p = n;
    old = (u8)n;
    if (old != 0) {
        old = (u8)n;
        if (old == 115) {
            func_8026E78();
            m4aSongNumStart(507);
        }
    } else {
        s->status = old;
    }
}

void SpriteVizorman(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitVizorman();
            break;
        case 16:
            ShopItemFollowShopkeeper();
            break;
        case 17:
            SpawnVizormanBeamActors();
            break;
    }
}

void SpriteVizormanBeamCore(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitVizormanBeamCore();
            break;
        case 18:
            StartVizormanBeamCoreActive();
            break;
        case 20:
            StartVizormanBeamCoreEnd();
            break;
        case 22:
            UpdateVizormanBeamCoreEnd();
            break;
    }
    UpdateVizormanBeamPalette();
}

void SpriteVizormanBeamGlow(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitVizormanBeamGlow();
            break;
        case 18:
            StartVizormanBeamGlow();
            break;
        case 20:
            UpdateVizormanBeamGlowLifetime();
            break;
    }
}

void InitBugle(void)
{
    register volatile u8* s PIN("r12");
    register u32 value PIN("r0");
    u32 temp;
    register u32 r2 PIN("r2");
    u32 zeroHalf;
    u32 down;
    u32 side;
    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;
    r2 = 0x80;
    r2 <<= 3;
    ASM_BARRIER(: "+r"(r2));
    value = r2;
    r2 = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)s;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = r2;
    value = (u32)s + 32;
    down = 24;
    *(volatile u8*)value = down;
    value++;
    *(volatile u8*)value = down;
    value++;
    side = 16;
    *(volatile u8*)value = side;
    value++;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sBugleIdleOam;
    temp = (u32)s;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = r2;
    *(volatile u16*)(temp + 20) = zeroHalf;
    if (gCurrentRoom == 0) {
        *(volatile u8*)(temp + 28) = side;
    } else {
        r2 = (u32)s;
        *(volatile u8*)(r2 + 28) = down;
    }
}

void StartBugleAttack(void)
{
    struct PrimarySpriteData* s;
    s = &gCurrentSprite;
    GP_SET(s->pOamData, sBugleAttackOam);
    s->currentAnimationFrame = 0;
    s->animationTimer = 0;
    s->work0 = 0;
    s->work1 = 0;
    s->pose = 18;
    m4aSongNumStart(508);
}

void UpdateBugleAttack(void)
{
    struct PrimarySpriteData* s;
    u32 value;
    register u32 temp PIN("r1");
    u32 zero;

    s = &gCurrentSprite;
    temp = s->work0;
    value = 31;
    value &= temp;
    temp = (u32)s;
    if (value == 0) {
        value = temp;
        value += 40;
        value = *(u8*)value;
        value -= 1;
        switch (value) {
            case 0:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 53);
                break;
            case 1:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 54);
                break;
            case 2:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 55);
                break;
            case 3:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 56);
                break;
            case 4:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 57);
                func_8026E78();
                break;
            case 5:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 58);
                break;
            case 6:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 59);
                break;
            case 7:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 60);
                break;
            case 8:
                SpawnSecondarySprite(gCurrentSprite.yPosition - 88, gCurrentSprite.xPosition - 32, 61);
                break;
            default:
                SpawnSecondarySprite(
                    ((struct PrimarySpriteData*)temp)->yPosition - 88,
                    ((struct PrimarySpriteData*)temp)->xPosition - 32,
                    52
                );
                break;
        }
        s = &gCurrentSprite;
        temp = (u32)s;
        temp += 40;
        value = *(u8*)temp;
        value += 1;
        *(u8*)temp = value;
        temp = (u32)s;
    }
    {
        register u8* ptr PIN("r2");
        ptr = (u8*)temp;
        ptr += 39;
        value = *ptr;
        value += 1;
        zero = 0;
        *ptr = value;
        value = ((struct PrimarySpriteData*)temp)->work1;
        if (value == 10) {
            value = 24;
            ((struct PrimarySpriteData*)temp)->pose = value;
            value = (u32)sBugleIdleOam;
            GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
            ((struct PrimarySpriteData*)temp)->currentAnimationFrame = zero;
            ((struct PrimarySpriteData*)temp)->animationTimer = zero;
        }
    }
}

void SelectBugleNoteAnimation(void)
{
    register volatile u8* sprite PIN("r12");
    register u32 r0 PIN("r0");
    u32 r1;
    register u32 r2 PIN("r2");
    u32 r3;
    register u32 r4 PIN("r4");
    r0 = (u32)&gCurrentSprite;
    sprite = (volatile u8*)r0;
    r1 = *(volatile u16*)r0;

    r2 = 144;
    r2 <<= 3;

    ASM_BARRIER(: "=r"(r0) : "0"(r2));
    r4 = 0;
    r3 = 0;
    r0 |= r1;
    r1 = 0xFFFB;
    r0 &= r1;
    r1 = (u32)sprite;
    *(volatile u16*)r1 = r0;
    *(volatile u8*)(r1 + 30) = r4;
    r0 = (u32)sprite + 32;
    r2 = 16;
    *(volatile u8*)r0 = r2;
    r0++;
    *(volatile u8*)r0 = r2;
    r0++;
    *(volatile u8*)r0 = r2;
    r0++;
    r1 = 4;
    *(volatile u8*)r0 = r1;
    r0++;
    *(volatile u8*)r0 = r1;
    r0++;
    *(volatile u8*)r0 = r1;
    r0++;
    *(volatile u8*)r0 = r1;
    r0 = (u32)sprite;
    *(volatile u8*)(r0 + 22) = r4;
    *(volatile u16*)(r0 + 20) = r3;
    r0 += 39;
    *(volatile u8*)r0 = r4;
    r0 += 2;
    *(volatile u8*)r0 = r1;
    r0++;
    *(volatile u8*)r0 = r4;
    r1 = (u32)sprite;
    *(volatile u8*)(r1 + 28) = r2;
    r0 = *(volatile u8*)(r1 + 24);
    r2 = (u32)sprite;
    switch (r0) {
        case 0:
            r0 = (u32)sBugleNote6Oam;
            goto one;
        case 1:
            r0 = (u32)sBugleNote1Oam;
            goto zero;
        case 2:
            r0 = (u32)sBugleNote10Oam;
            goto one;
        case 3:
            r0 = (u32)sBugleNote2Oam;
            goto zero;
        case 4:
            r0 = (u32)sBugleNote9Oam;
            goto one;
        case 5:
            r0 = (u32)sBugleNote3Oam;
            goto zero;
        case 6:
            r0 = (u32)sBugleNote8Oam;
            goto one;
        case 7:
            r0 = (u32)sBugleNote4Oam;
        zero:
            *(volatile u32*)(r2 + 4) = r0;
            r1 = r2;
            r1 += 40;
            r0 = 0;
            *(volatile u8*)r1 = r0;
            goto done;
        case 8:
            r0 = (u32)sBugleNote7Oam;
        one:
            *(volatile u32*)(r2 + 4) = r0;
            r1 = r2;
            r1 += 40;
            r0 = 1;
            *(volatile u8*)r1 = r0;
            goto done;
        default:
            r0 = (u32)sBugleNote5Oam;
            r2 = (u32)sprite;

            *(volatile u32*)(r2 + 4) = r0;
            r0 = (u32)sprite + 40;
            *(volatile u8*)r0 = r4;
            goto done;
    }
done:;
}

void UpdateBugleNoteAffine(void)
{
    register u32 r0 PIN("r0");
    u32 r1;
    register struct PrimarySpriteData* r2 PIN("r2");
    struct PrimarySpriteData* r3;
    s32 r4;
    s32 r5;
    register struct PrimarySpriteData* r6 PIN("r6");
    s32 r7;
    register u8* r8 PIN("r8");

    r7 = 0;
    r3 = &gCurrentSprite;
    r1 = (u32)&r3->work0;
    r0 = *(u8*)r1;
    r0--;
    *(u8*)r1 = r0;
    r0 <<= 24;
    r2 = (struct PrimarySpriteData*)(r0 >> 24);
    if ((u32)r2 == 0) {
        r3->status = r7;
        goto end;
    }
    if ((u32)r2 > 195) {
        r1 = r3->status;
        r0 = 128;
        r0 |= r1;
        r3->status = r0;
        r1 = (u32)&r3->work2;
        r0 = *(u8*)r1;
        r0 += 4;
        *(u8*)r1 = r0;
        r7 = *(u8*)r1;
    } else if ((u32)r2 <= 59) {
        r1 = r3->status;
        r0 = 128;
        r0 |= r1;
        r1 = 0xBFFF;
        r0 &= r1;
        r3->status = r0;
        r1 = (u32)&r3->work2;
        r0 = *(u8*)r1;
        r0 -= 4;
        *(u8*)r1 = r0;
        r7 = *(u8*)r1;
    } else {
        r1 = r3->roomSlot;
        r0 = 1;
        r0 &= r1;
        if (r0 != 0) {
            r0 = (u32)r2;
            r0 -= 60;
            r0 <<= 24;
            r2 = (struct PrimarySpriteData*)(r0 >> 24);
            if ((u32)r2 > 104) {
                goto clear_affine_bit;
            }
            if ((u32)r2 > 74) {
                goto set_affine_bit;
            }
            if ((u32)r2 > 44) {
                goto clear_affine_bit;
            }
            if ((u32)r2 <= 14) {
                goto clear_affine_bit;
            }
        set_affine_bit:
            r1 = r3->status;
            r2 = (struct PrimarySpriteData*)(129 << 7);
            ASM_BARRIER_VOLATILE(: "+r"(r2));
            r0 = (u32)r2;
            r0 |= r1;
            r3->status = r0;
            r7 = 128 << 2;
            goto scale_ready;
        clear_affine_bit:
            r1 = r3->status;
            r0 = 0xFF7F;
            r0 &= r1;
            r3->status = r0;
        } else {
            r7 = 128 << 1;
        }
    }
scale_ready:

    r6 = &gCurrentSprite;
    r1 = r6->roomSlot;
    r0 = 1;
    r0 &= r1;
    if (r0 != 0) {
        r4 = (s32)sSinCosTable;
        r0 = r4 + 128;
        r1 = 0;
        r5 = *(s16*)r0;
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r5;
        r0 = FixedMul(r0, r1);
        r6->affinePA = r0;
        /* The original signed halfword ASM load is now C. Register-lifetime
         * constraints preserve the target's zero-index register allocation. */
        r2 = 0;
        {
            register u32 r0Live PIN("r0");
            register u32 r1Live PIN("r1");

            /* Both registers must be unavailable for the C signed load to use r2. */
            ASM_BARRIER(: "=r"(r0Live), "=r"(r1Live));
            r4 = *(const s16 *)r4;
            ASM_BARRIER(: : "r"(r0Live), "r"(r1Live));
        }
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r4;
        r0 = FixedMul(r0, r1);
        r6->affinePB = r0;
        r4 = -r4;
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r4;
        r0 = FixedMul(r0, r1);
        r6->affinePC = r0;
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r5;
        goto last_mul;
    } else {
        r5 = (s32)sSinCosTable;
        r0 = 39;
        ASM_BARRIER_VOLATILE(: "+r"(r0));
        r0 += (u32)r6;
        r8 = (u8*)r0;
        r0 = *(u8*)r0;
        r0 += 64;
        r0 <<= 1;
        r0 += r5;
        r1 = 0;
        r4 = *(s16*)(r0 + r1);
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r4;
        r0 = FixedMul(r0, r1);
        r6->affinePA = r0;
        r2 = (struct PrimarySpriteData*)r8;
        r0 = *(u8*)r2;
        r0 <<= 1;
        r0 += r5;
        r1 = 0;
        {
            register u32 r2Live PIN("r2");

            /* Preserve r1 as the index register for this existing C read. */
            ASM_BARRIER(: "=r"(r2Live));
            r4 = *(s16 *)(r0 + r1);
            ASM_BARRIER(: : "r"(r2Live));
        }
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r4;
        r0 = FixedMul(r0, r1);
        r6->affinePB = r0;
        r2 = (struct PrimarySpriteData*)r8;
        r0 = *(u8*)r2;
        r0 <<= 1;
        r0 += r5;
        r1 = 0;
        {
            register u32 r2Live PIN("r2");

            /* Preserve r1 as the index register for this existing C read. */
            ASM_BARRIER(: "=r"(r2Live));
            r4 = *(s16 *)(r0 + r1);
            ASM_BARRIER(: : "r"(r2Live));
        }
        r4 = -r4;
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r4;
        r0 = FixedMul(r0, r1);
        r6->affinePC = r0;
        r2 = (struct PrimarySpriteData*)r8;
        r0 = *(u8*)r2;
        r0 += 64;
        r0 <<= 1;
        r0 += r5;
        r1 = 0;
        {
            register u32 r2Live PIN("r2");

            /* Preserve r1 as the index register for this existing C read. */
            ASM_BARRIER(: "=r"(r2Live));
            r4 = *(s16 *)(r0 + r1);
            ASM_BARRIER(: : "r"(r2Live));
        }
        r0 = r7;
        r0 = FixedInverse(r0);
        r1 = r0;
        r0 = r4;
    last_mul:
        r0 = FixedMul(r0, r1);
        r6->affinePD = r0;
    }

    r2 = (struct PrimarySpriteData*)sBugleNoteAffineAngleTable;
    r1 = (u32)&gCurrentSprite;
    r5 = r1 + 42;
    r0 = *(u8*)r5;
    r0 >>= 3;
    r0 <<= 1;
    r0 += (u32)r2;
    r3 = (struct PrimarySpriteData*)(u32) * (u16*)r0;
    r4 = r1 + 40;
    r0 = *(u8*)r4;
    r2 = (struct PrimarySpriteData*)r1;
    if (r0 != 0) {
        r0 = r2->xPosition;
        r0 += (u32)r3;
    } else {
        r0 = r2->xPosition;
        r0 -= (u32)r3;
    }
    r2->xPosition = r0;
    r0 = *(u8*)r5;
    if (r0 > 94) {
        r0 = 0;
        *(u8*)r5 = r0;
        r0 = *(u8*)r4;
        r1 = 1;
        r0 ^= r1;
        *(u8*)r4 = r0;
    }
    r1 = (u32)r2 + 42;
    r0 = *(u8*)r1;
    r0++;
    *(u8*)r1 = r0;
    r0 = r2->xPosition;
    r0 += 2;
    r2->xPosition = r0;
    r0 = r2->yPosition;
    r0 -= 2;
    r2->yPosition = r0;
end:;
}

void SpriteBugle(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitBugle();
            break;
        case 16:
            ShopItemFollowShopkeeper();
            break;
        case 17:
            StartBugleAttack();
        case 18:
            UpdateBugleAttack();
            break;
    }
}

void SpriteBugleNote(void)
{
    struct PrimarySpriteData* s;
    u32 p;
    s = &gCurrentSprite;
    s->disableWarioCollisionTimer = 1;
    p = s->pose;
    if (p == 0) {
        goto init;
    }
    if (p == 16) {
        goto update;
    }
    s->status = 0;
    goto done;
init:
    SelectBugleNoteAnimation();
update:
    UpdateBugleNoteAffine();
done:;
}

void InitBlackDog(void)
{
    volatile u8* s;
    register u32 value PIN("r0");
    u32 temp;
    register u32 r2 PIN("r2");
    u32 zero;
    u32 side;
    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;
    r2 = 0x81;
    r2 <<= 3;
    ASM_BARRIER(: "+r"(r2));
    value = r2;
    zero = 0;
    r2 = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    *(volatile u16*)s = value;
    *(volatile u8*)(s + 30) = zero;
    temp = (u32)s;
    temp += 32;
    value = 24;
    *(volatile u8*)temp = value;
    value = (u32)s;
    value += 33;
    side = 16;
    *(volatile u8*)value = side;
    temp += 2;
    value = 40;
    *(volatile u8*)temp = value;
    value = (u32)s;
    value += 35;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sBlackDogIdleOam;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(s + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(s + 22) = zero;
    *(volatile u16*)(s + 20) = r2;
    if (gCurrentRoom == 0) {
        *(volatile u8*)(s + 28) = side;
    } else {
        value = 18;
        *(volatile u8*)(s + 28) = value;
        temp = (u32)s + 39;
        value = 144;
        *(volatile u8*)temp = value;
        value = (u32)s;
        value += 42;
        *(volatile u8*)value = zero;
    }
}

void StartBlackDogAttack(void)
{
    u32 r0;
    s32 r1;
    struct PrimarySpriteData* r2;
    register u32 r3 PIN("r3");
    s32 r4;
    const s16* r5;
    u32 r6;
    register u8* ip PIN("r12");

    r2 = &gCurrentSprite;
    r0 = 42;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r0 += (u32)r2;
    ip = (u8*)r0;
    r3 = *(u8*)r0;
    r5 = sBlackDogMovementPath;
    r0 = r3 << 1;
    r0 += (u32)r5;
    r4 = *(u16*)r0;
    r6 = 0;
    r1 = *(s16*)(r0 + r6);
    r0 = 0x7FFF;
    if (r1 == r0) {
        r4 = *(u16*)r5;
        r3 = 0;
    }
    r0 = r3 + 1;
    r1 = (u32)ip;
    *(u8*)r1 = r0;
    r0 = r2->yPosition;
    r0 += r4;
    r2->yPosition = r0;

    r0 = r2->work0;
    if (r0 <= 59) {
        r0 = gCurrentPassage;
        r0--;
        switch (r0) {
            case 0:
                r0 = r2->yPosition;
                r0 -= 5;
                goto shift_common;
            case 1:
                r0 = r2->yPosition;
                r0 -= 7;
                goto shift_common;
            case 2:
                r0 = r2->yPosition;
                r0 -= 3;
                r2->yPosition = r0;
                r0 = r2->xPosition;
                r0 += 6;
                r2->xPosition = r0;
                break;
            case 3:
            case 4:
                r0 = r2->yPosition;
                r0 -= 3;
            shift_common:
                r2->yPosition = r0;
                r0 = r2->xPosition;
                r0 += 3;
                r2->xPosition = r0;
                break;
            default:
                break;
        }
    } else if (r0 == 60) {
        r1 = r2->status;
        r0 = 0xFFF7;
        r0 &= r1;
        r2->status = r0;
    }

    r1 = (u32)r2;
    r1 += 39;
    r0 = *(u8*)r1;
    r0--;
    *(u8*)r1 = r0;
    r0 <<= 24;
    if (r0 != 0) {
        goto end;
    }
    r0 = 20;
    r2->pose = r0;
    r0 = 120;
    *(u8*)r1 = r0;
    r0 = r2->yPosition;
    r0 -= 64;
    r1 = r2->xPosition;
    r1 += 96;
    r2 = (struct PrimarySpriteData*)51;
    SpawnSecondarySprite(r0, r1, (u8)(u32)r2);
    func_8026E78();
    r0 = 255;
    r0 <<= 1;
    m4aSongNumStart(r0);
end:;
}

void UpdateBlackDogAttack(void)
{
    u32 r0;
    u32 r1;
    register u32 r2 PIN("r2");
    struct PrimarySpriteData* r3;
    register u32 r4 PIN("r4");
    register u32 r5 PIN("r5");

    r3 = &gCurrentSprite;
    r0 = r3->yPosition;
    r0 -= 64;
    r0 <<= 16;
    r4 = r0 >> 16;
    r0 = r3->xPosition;
    r0 += 96;
    r0 <<= 16;
    r2 = r0 >> 16;
    r1 = (u32)r3;
    r1 += 39;
    r0 = *(u8*)r1;
    r5 = r0;
    if (r5 == 0) {
        goto timer_zero;
    }
    r0--;
    *(u8*)r1 = r0;
    r0 = *(u8*)r1;

    switch (r0) {
        case 55:
            r0 = r4;
            r0 -= 32;

            goto y_done_x_minus;
        case 50:
        case 100:
            r0 = r4;
            r0 += 32;
            r0 <<= 16;

            r4 = r0 >> 16;

            r0 = r2;

            r0 += 32;

            goto x_done;
        case 40:
        case 90:
            r0 = r4;
            r0 -= 32;
            r0 <<= 16;

            r4 = r0 >> 16;

            r0 = r2;

            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 30:
        case 80:
            r0 = r4;
            r0 -= 32;
            ASM_BARRIER_VOLATILE(: : : "cc");
            goto y_done_x_minus;
        case 25:
        case 75:
            r0 = r4;
            r0 += 32;
            r0 <<= 16;
            r4 = r0 >> 16;

            r0 = r2;
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 15:
        case 65:
            r0 = r4;
            r0 -= 32;
            r0 <<= 16;
            r4 = r0 >> 16;

            r0 = r2;
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 10:
        case 35:
        case 60:
        case 85:
            r0 = r4;
            r0 += 32;
            goto y_done_x_minus;
        case 20:
        case 45:
        case 70:
        case 95:
            goto spawn;
        default:
            goto end;
    }
y_done_x_minus:
    r0 <<= 16;
    r4 = r0 >> 16;
    r0 = r2;
    r0 -= 32;
x_done:
    r0 <<= 16;
    r2 = r0 >> 16;
spawn:
    r0 = r4;
    r1 = r2;
    r2 = 51;
    SpawnSecondarySprite(r0, r1, (u8)r2);
    goto end;
timer_zero:
    r0 = 21;
    r3->pose = r0;
    r0 = (u32)r3;
    r0 += 42;
    *(u8*)r0 = r5;
    r0 = r3->status;
    r1 = 64;
    r0 ^= r1;
    r3->status = r0;
    r0 = gCurrentPassage;
    if (r0 == 5) {
        goto end;
    }
    r0 = 0x2AD;
    m4aSongNumStart(r0);
end:;
}

void UpdateBlackDogCollision(void)
{
    struct PrimarySpriteData* s;
    u8* indexPtr;
    u32 index;
    const s16* table;
    u32 value;
    register s32 signedValue PIN("r1");
    u32 velocity;
    u32 zero;
    s = &gCurrentSprite;
    indexPtr = &s->work3;
    index = *indexPtr;
    table = sBlackDogMovementPath;
    value = index << 1;
    value += (u32)table;
    velocity = *(u16*)value;
    zero = 0;
    signedValue = *(s16*)value;
    if (signedValue == 0x7FFF) {
        velocity = *(u16*)table;
        index = 0;
    }
    value = index + 1;
    *indexPtr = value;
    value = s->yPosition;
    value += velocity;
    value += 6;
    s->yPosition = value;
    s->xPosition -= 6;
    value = func_8023A60(s->yPosition, s->xPosition);
    signedValue = value;
    if (gSpriteCollisionResult != 0) {
        s->yPosition = signedValue;
        if (gCurrentPassage == 5) {
            s->pose = 111;
            s->status ^= 0x40;
        } else {
            register u8* timer PIN("r1");
            s->pose = 22;
            timer = &s->work0;
            value = 60;
            *timer = value;
        }
    }
}

void FinishBlackDogAttack(void)
{
    struct PrimarySpriteData* s;
    u32 v;
    register u8* p PIN("r1");
    s = &gCurrentSprite;
    p = &s->work0;
    v = *p;
    v--;
    *p = v;
    v = (u8)v;
    if (v) {
        s->xPosition -= 8;
    } else {
        s->status = v;
    }
}

void SpriteBlackDog(void)
{
    struct PrimarySpriteData* s;
    s = &gCurrentSprite;
    s->disableWarioCollisionTimer = 1;
    switch (s->pose) {
    case 0:
        InitBlackDog();
        break;
    case 16:
        ShopItemFollowShopkeeper();
        break;
    case 18:
        StartBlackDogAttack();
        break;
    case 20:
        UpdateBlackDogAttack();
        break;
    case 21:
        UpdateBlackDogCollision();
        break;
    case 22:
        FinishBlackDogAttack();
        break;
    /* Empty, but it keeps the switch sparse enough to compile as a compare tree. */
    case 92:
        break;
    }
}

void InitLargeLips(void)
{
    volatile u8* s;
    register u32 value PIN("r0");
    u32 temp;
    register u32 flag PIN("r2");
    u32 zero;
    register u32 side PIN("r5");
    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;

    flag = 129;
    flag <<= 3;
    ASM_BARRIER(: "+r"(flag));
    value = flag;

    zero = 0;
    flag = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    *(volatile u16*)s = value;
    *(volatile u8*)(s + 30) = zero;
    temp = (u32)s + 32;
    value = 32;
    *(volatile u8*)temp = value;
    value = (u32)s + 33;
    side = 16;
    *(volatile u8*)value = side;
    value++;
    *(volatile u8*)value = side;
    value++;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sLargeLipsIdleOam;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(s + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(s + 22) = zero;
    *(volatile u16*)(s + 20) = flag;
    if (gCurrentRoom == 0) {
        *(volatile u8*)(s + 28) = side;
    } else {
        value = 17;
        *(volatile u8*)(s + 28) = value;
        temp = (u32)s + 39;
        value = 128;
        *(volatile u8*)temp = value;
        value = (u32)s;
        value += 42;
        *(volatile u8*)value = zero;
    }
}

void MoveLargeLipsIntoPosition(void)
{
    struct PrimarySpriteData* s;
    register u32 value PIN("r0");
    register u8* indexPtr PIN("r12");
    register u32 index PIN("r3");
    register const s16* table PIN("r5");
    register u32 offset PIN("r4");
    register u32 zero PIN("r6");
    s32 tmp;

    s = &gCurrentSprite;
    indexPtr = &s->work3;
    index = *indexPtr;
    table = sLargeLipsMovementPath;
    value = index;
    value <<= 1;
    value += (u32)table;
    offset = *(u16*)value;
    zero = 0;
    tmp = *(s16*)value;
    value = 0x7FFF;
    if ((s32)tmp == (s32)value) {
        offset = *(u16*)table;
        index = 0;
    }
    value = index + 1;
    *indexPtr = value;
    value = s->yPosition;
    value += offset;
    s->yPosition = value;
    value = s->work0;
    if (value <= 46) {
        value = gCurrentPassage;
        value -= 1;
        switch (value) {
            case 0:
                value = s->yPosition;
                value -= 6;
                s->yPosition = value;
                value = s->xPosition;
                value += 4;
                s->xPosition = value;
                break;
            case 1:
                value = s->yPosition;
                value -= 9;
                goto add5;
            case 2:
                value = s->yPosition;
                value -= 4;
                s->yPosition = value;
                value = s->xPosition;
                value += 7;
                s->xPosition = value;
                break;
            case 3:
                value = s->yPosition;
                value -= 5;
                goto add5;
            case 4:
                value = s->yPosition;
                value -= 4;
            add5:
                s->yPosition = value;
                value = s->xPosition;
                value += 5;
                s->xPosition = value;
                break;
        }
    } else {
        if (value == 50) {
            tmp = s->status;
            value = 0xFFF7;
            value &= tmp;
            s->status = value;
        }
    }
    zero = 39;
    ASM_BARRIER(: "+r"(zero));
    zero += (u32)s;
    value = *(volatile u8*)zero;
    value -= 1;
    *(volatile u8*)zero = value;
    index = (u8)value;
    if (index == 0) {
        tmp = 18;
        s->pose = tmp;
        GP_SET(s->pOamData, sLargeLipsMoveOam);
        s->currentAnimationFrame = index;
        s->animationTimer = index;
        *(volatile u8*)zero = tmp;
    }
}

void StartLargeLipsKiss(void)
{
    struct PrimarySpriteData* s;
    u8* timer;
    u32 value;
    register u32 zero PIN("r1");
    u32 z;
    volatile u32 stackSlot;
    s = &gCurrentSprite;
    timer = &s->work0;
    value = *timer - 1;
    *timer = value;
    z = (u8)value;
    if (z == 0) {
        s->pose = 19;
        GP_SET(s->pOamData, sLargeLipsKissOam);
        s->currentAnimationFrame = z;
        zero = 0;
        s->animationTimer = z;
        value = 204;
        *timer = value;
        s->work2 = zero;
        z = s->yPosition;
        z -= 128;
        value = s->xPosition;
        value -= 64;
        stackSlot = value;
        ASM_BARRIER_ARM(::: "r1");
#ifdef PORTABLE
        SpawnHighPriorityPrimarySprite(201, 0, 0, z, stackSlot);
#else
        ((void (*)(u32, u32, u32, u32))SpawnHighPriorityPrimarySprite)(201, 0, 0, z);
#endif
        func_8026E78();
        m4aSongNumStart(513);
    }
}

void UpdateLargeLipsKiss(void)
{
    struct PrimarySpriteData *s = &gCurrentSprite;
    u8 *work2 = &s->work2;
    u8 timer;

    timer = (*work2)++;

    switch (gCurrentPassage) {
        case 1:
            if (timer == 40) {
                SpawnHighPriorityPrimarySprite(201, 1, 0, s->yPosition - 128, s->xPosition - 64);
            } else if (timer == 80) {
                SpawnHighPriorityPrimarySprite(201, 2, 0, s->yPosition - 128, s->xPosition - 64);
            } else if (timer == 120) {
                SpawnHighPriorityPrimarySprite(201, 0, 0, s->yPosition - 128, s->xPosition - 64);
            } else if (timer == 160) {
                SpawnHighPriorityPrimarySprite(201, 1, 0, s->yPosition - 128, s->xPosition - 64);
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            if (timer == 25) {
                SpawnHighPriorityPrimarySprite(201, 1, 0, s->yPosition - 128, s->xPosition - 64);
            } else if (timer == 50) {
                SpawnHighPriorityPrimarySprite(201, 2, 0, s->yPosition - 128, s->xPosition - 64);
            } else if (timer == 75) {
                SpawnHighPriorityPrimarySprite(201, 0, 0, s->yPosition - 128, s->xPosition - 64);
                *work2 = 0;
            }
            break;
    }

    if (--gCurrentSprite.work0 == 0) {
        gCurrentSprite.status ^= 0x40;
        gCurrentSprite.pose = 20;
        GP_SET(gCurrentSprite.pOamData, sLargeLipsIdleOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.work0 = 112;
        gCurrentSprite.work3 = 0;
        if (gCurrentPassage == 5) {
            m4aSongNumStart(0x2AD);
            gCurrentSprite.pose = 110;
        }
    }
}

void UpdateLargeLipsRetreat(void)
{
    struct PrimarySpriteData* s;
    u8* indexPtr;
    u32 index;
    const s16* table;
    u32 value;
    s32 signedValue;
    u32 velocity;
    u32 zero;
    s = &gCurrentSprite;
    s->xPosition -= 8;
    indexPtr = &s->work3;
    index = *indexPtr;
    table = sLargeLipsMovementPath;
    value = index << 1;
    value = index << 1;
    value += (u32)table;
    velocity = *(u16*)value;
    zero = 0;
    signedValue = *(s16*)value;
    if (signedValue == 0x7FFF) {
        velocity = *(u16*)table;
        index = 0;
    }
    value = index + 1;
    *indexPtr = value;
    s->yPosition += velocity;
    {
        u8* timer;
        timer = &s->work0;
        value = *timer - 1;
        *timer = value;
        value = (u8)value;
        if (value == 0) {
            s->status = value;
        }
    }
}

void UpdateLargeLipsCollision(void)
{
    struct PrimarySpriteData* s;
    u8* indexPtr;
    u32 index;
    const s16* table;
    u32 value;
    s32 signedValue;
    register u32 velocity PIN("r3");
    u32 zero;
    s = &gCurrentSprite;
    indexPtr = &s->work3;
    index = *indexPtr;
    table = sLargeLipsMovementPath;
    value = index << 1;
    value += (u32)table;
    velocity = *(u16*)value;
    zero = 0;
    signedValue = *(s16*)value;
    if (signedValue == 0x7FFF) {
        velocity = *(u16*)table;
        index = 0;
    }
    value = index + 1;
    *indexPtr = value;
    value = s->yPosition;
    value += velocity;
    value += 6;
    s->yPosition = value;
    s->xPosition -= 6;
    value = func_8023A60(s->yPosition, s->xPosition);
    signedValue = value;
    if (gSpriteCollisionResult != 0) {
        s->yPosition = signedValue;
        s->pose = 111;
        s->status ^= 0x40;
    }
}

void InitLargeLipsKissEffect(void)
{
    register volatile u8* s PIN("r12");
    register u32 value PIN("r0");
    u32 temp;
    register u32 flag PIN("r2");
    u32 zero;
    register u32 zeroHalf PIN("r4");
    s = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)s;

    flag = 137;
    flag <<= 7;
    ASM_BARRIER(: "+r"(flag));
    value = flag;
    ASM_BARRIER(: "+r"(value));
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)s;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    value = (u32)s + 32;
    flag = 16;
    *(volatile u8*)value = flag;
    value++;
    *(volatile u8*)value = flag;
    value++;
    *(volatile u8*)value = flag;
    value++;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)s;
    *(volatile u8*)(value + 22) = zero;
    *(volatile u16*)(value + 20) = zeroHalf;
    temp = (u32)s + 39;
    value = 8;
    *(volatile u8*)temp = value;
    value = (u32)s + 41;
    *(volatile u8*)value = zero;
    temp = (u32)s;
    *(volatile u8*)(temp + 28) = flag;
    value = *(volatile u8*)(temp + 24);
    if (value == 1) {
        goto case1;
    }
    if (value == 2) {
        goto case2;
    }
    value = (u32)sLargeLipsKissEffectAOam;
    goto store;
case1:
    value = (u32)sLargeLipsKissEffectBOam;
    flag = (u32)s;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(flag + 4), (const struct AnimationFrame*)value);
    goto done;
case2:
    value = (u32)sLargeLipsKissEffectCOam;
    temp = (u32)s;
store:
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
done:;
}

void ToggleLargeLipsKissEffectFlip(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    register u32 work PIN("r1");
    u32 v;
    s = &gCurrentSprite;
    p = &s->work2;
    work = *p;
    v = 1;
    v &= work;
    if (v) {
        work = s->status;
        v = 4;
        v |= work;
    } else {
        work = s->status;
        v = 0xFFFB;
        v &= work;
    }
    s->status = v;
    {
        register u8* q PIN("r1");
        q = &s->work2;
        v = *q;
        v++;
        *q = v;
    }
}

void ToggleLargeLipsKissEffectFlipReverse(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    register u32 work PIN("r1");
    u32 v;
    s = &gCurrentSprite;
    p = &s->work2;
    work = *p;
    v = 1;
    v &= work;
    if (v) {
        work = s->status;
        v = 0xFFFB;
        v &= work;
    } else {
        work = s->status;
        v = 4;
        v |= work;
    }
    s->status = v;
    {
        register u8* q PIN("r1");
        q = &s->work2;
        v = *q;
        v++;
        *q = v;
    }
}

void GrowLargeLipsKissEffect(void)
{
    struct PrimarySpriteData* s;
    u8* p;
    u32 r0;
    u32 r1;
    u32 r2;
    s32 scale;
    s32 trig;
    s32 cosine;
    s = &gCurrentSprite;
    p = &s->work0;
    r0 = *p;
    scale = r0 << 1;
    r2 = r0 + 4;
    *p = r2;
    r0 = s->xPosition;
    r0++;
    s->xPosition = r0;
    if (scale == 0) {
        scale = 0x80;
        scale <<= 2;
        r0 = 18;
        s->pose = r0;
        r1 = (u32)&s->work1;
        r0 = 1;
        *(u8*)r1 = r0;
        r1 += 2;
        r0 = 64;
        *(u8*)r1 = r0;
        r0 = 255;
        r2 |= r0;
        *p = r2;
    }
    trig = (s32)sSinCosTable;
    r0 = trig + 128;
    r1 = 0;
    cosine = *(s16*)r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = cosine;
    r0 = FixedMul(r0, r1);
    s->affinePA = r0;
    r2 = 0;
    trig = *(s16*)trig;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePB = r0;
    trig = -trig;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s->affinePC = r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = cosine;
    r0 = FixedMul(r0, r1);
    s->affinePD = r0;
    ToggleLargeLipsKissEffectFlip();
}

void MoveLargeLipsKissEffect(void)
{
    struct PrimarySpriteData* s;
    u32 r0;
    register u32 r1 PIN("r1");
    u32 r2;
    u32 r3;
    s32 r4;
    register s32 r5 PIN("r5");
    u8* work3;
    s32 scale;
    s32 ended;
    s = &gCurrentSprite;
    r1 = (u32)s;
    r1 += 39;
    r0 = *(u8*)r1;
    scale = r0 << 1;
    if (r0 > 7) {
        r0--;
        *(u8*)r1 = r0;
    }
    ended = 0;
    r1 = 42;
    ASM_BARRIER(: "+r"(r1));
    r1 += (u32)s;
    work3 = (u8*)r1;
    r2 = *(u8*)r1;
    r3 = (u32)sSpriteAffineScaleTable;
    r0 = r2 << 2;
    r0 += r3;
    r4 = *(u16*)r0;
    r5 = 0;
    r1 = *(s16*)(r0 + r5);
    r0 = 0x7FFF;
    if (r1 == r0) {
        r2 = 0;
        r4 = *(u16*)r3;
        r0 = 1;
        ended = r0;
    }
    r0 = r2 << 2;
    r1 = r3 + 2;
    r0 += r1;
    r1 = *(u16*)r0;
    r0 = r2 + 1;
    r2 = (u32)work3;
    *(u8*)r2 = r0;
    r0 = s->yPosition;
    r0 += r4;
    s->yPosition = r0;
    r0 = s->xPosition;
    r0 += r1;
    s->xPosition = r0;
    r4 = (s32)sSinCosTable;
    r0 = r4 + 128;
    r1 = 0;
    r5 = *(s16*)r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r5;
    r0 = FixedMul(r0, r1);
    s->affinePA = r0;
    r2 = 0;
    r4 = *(s16 *)(r4 + r2);
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r4;
    r0 = FixedMul(r0, r1);
    s->affinePB = r0;
    r4 = -r4;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r4;
    r0 = FixedMul(r0, r1);
    s->affinePC = r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r5;
    r0 = FixedMul(r0, r1);
    s->affinePD = r0;
    ToggleLargeLipsKissEffectFlip();
    r5 = ended;
    if (r5 != 0) {
        r0 = 20;
        s->pose = r0;
        r1 = (u32)s;
        r1 += 40;
        r0 = 1;
        *(u8*)r1 = r0;
    }
}

void FadeLargeLipsKissEffect(void)
{
    struct PrimarySpriteData* s;
    register u32 r0;
    register u32 r1 PIN("r1");
    u32 r2;
    register u32 r3 PIN("r3");
    s32 r4;
    s32 r5;
    register u8* work3 PIN("r12");
    s32 scale;
    s = &gCurrentSprite;
    r1 = (u32)s;
    r1 += 39;
    r0 = *(u8*)r1;
    scale = r0 << 1;
    r0--;
    *(u8*)r1 = r0;
    r0 = (u8)r0;
    if (r0 <= 32) {
        r0 = 0;
        s->status = r0;
        goto done;
    }
    r0 = 42;
    ASM_BARRIER(: "+r"(r0));
    r0 += (u32)s;
    work3 = (u8*)r0;
    r3 = *(u8*)r0;
    r2 = (u32)sSpriteAffineScaleTable;
    r0 = r3 << 2;
    r0 += r2;
    r5 = *(u16*)r0;
    r4 = 0;
    r1 = *(s16*)(r0 + r4);
    r0 = 0x7FFF;
    r4 = (u32)s;
    r4 += 40;
    if (r1 == r0) {
        r3 = 0;
        r5 = *(u16*)r2;
        r0 = *(u8*)r4;
        r0++;
        *(u8*)r4 = r0;
    }
    r0 = r3 << 2;
    r1 = r2 + 2;
    r0 += r1;
    r1 = *(u16*)r0;
    r2 = *(u8*)r4;
    r0 = r3 + 1;
    r3 = (u32)work3;
    *(u8*)r3 = r0;
    r0 = (s16)r5;
    r0 *= r2;
    r4 = s->yPosition;
    ASM_BARRIER(: "+r"(r4));
    r0 += r4;
    s->yPosition = r0;
    r1 = (s16)r1;

    ASM_BARRIER(: "=r"(r0) : "0"(r1));
    r0 *= r2;
    r1 = s->xPosition;

    r0 += r1;
    s->xPosition = r0;
    r4 = (s32)sSinCosTable;
    r0 = r4 + 128;
    r3 = 0;
    /* Keep the existing r1 and r2 values live so the signed load uses r3. */
    r5 = *(const s16 *)r0;
    ASM_BARRIER(: : "r"(r1), "r"(r2));
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r5;
    r0 = FixedMul(r0, r1);
    s->affinePA = r0;
    r0 = 0;
    r4 = *(s16*)r4;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r4;
    r0 = FixedMul(r0, r1);
    s->affinePB = r0;
    r4 = -r4;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r4;
    r0 = FixedMul(r0, r1);
    s->affinePC = r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r5;
    r0 = FixedMul(r0, r1);
    s->affinePD = r0;
    ToggleLargeLipsKissEffectFlipReverse();
done:;
}

void SpriteLargeLips(void)
{
    struct PrimarySpriteData* s;
    register s32 v PIN("r0");
    s = &gCurrentSprite;
    v = 1;
    s->disableWarioCollisionTimer = v;
    v = s->pose;
    switch (v) {
        case 0:
            InitLargeLips();
            break;
        case 16:
            ShopItemFollowShopkeeper();
            break;
        case 17:
            MoveLargeLipsIntoPosition();
            break;
        case 18:
            StartLargeLipsKiss();
            break;
        case 19:
            UpdateLargeLipsKiss();
            break;
        case 20:
            UpdateLargeLipsRetreat();
            break;
        case 110:
            UpdateLargeLipsCollision();
            break;
    }
}

void SpriteLargeLipsKissEffect(void)
{
    struct PrimarySpriteData* s;
    register s32 pose PIN("r1");
    struct PrimarySpriteData* copy;
    s = &gCurrentSprite;
    pose = 1;
    s->disableWarioCollisionTimer = pose;
    pose = s->pose;
    copy = s;
    switch (pose) {
        case 0:
            InitLargeLipsKissEffect();
        case 16:
            GrowLargeLipsKissEffect();
            break;
        case 18:
            MoveLargeLipsKissEffect();
            break;
        case 20:
            FadeLargeLipsKissEffect();
            break;
        default:
            s = 0;
            copy->status = (u32)s;
            break;
    }
}

void InitBigFist(void)
{
    struct PrimarySpriteData* s;
    u32 r0;
    register u32 r1 PIN("r1");
    register u32 r2 PIN("r2");
    register s32 r4 PIN("r4");
    struct PrimarySpriteData* sp;
    s32 scale;
    register s32 cosine PIN("r8");
    s = &gCurrentSprite;
    r1 = s->status;
    r2 = 0x4488;
    ASM_BARRIER(: "+r"(r2));
    r0 = r2;

    r4 = 0;

    r2 = 0;

    r0 |= r1;
    r1 = 0xFFFB;
    r0 &= r1;
    s->status = r0;
    ASM_BARRIER(: "+r"(r4));
    s->warioCollision = r4;
    r1 = (u32)s + 32;
    r0 = 64;
    *(u8*)r1 = r0;
    r1++;
    r0 = 32;
    *(u8*)r1 = r0;
    r1++;
    *(volatile u8 *)r1 = r0;
    r0 = (u32)s + 35;
    r1 = 4;
    *(u8*)r0 = r1;
    r0++;
    *(u8*)r0 = r1;
    r0++;
    *(u8*)r0 = r1;
    r0++;
    *(u8*)r0 = r1;
    GP_SET(s->pOamData, sBigFistIdleOam);
    ASM_BARRIER(: "+r"(r4));
    s->currentAnimationFrame = r4;
    s->animationTimer = r2;
    {
        u8 room = gCurrentRoom;
        if (room == 0) {
            r0 = 16;
            s->pose = r0;
        } else {
            r0 = 18;
            s->pose = r0;
            r1 = (u32)&s->work0;
            r0 = 150;
            *(u8*)r1 = r0;
            r0 = (u32)&s->work3;
            *(u8*)r0 = r4;
            r1 += 1;
            r0 = 128;
            *(volatile u8 *)r1 = r0;
        }
    }
    scale = 0x100;
    r4 = (s32)sSinCosTable;
    r0 = r4 + 128;
    r2 = 0;
    r1 = *(s16*)r0;
    cosine = r1;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = cosine;
    r0 = FixedMul(r0, r1);
    sp = &gCurrentSprite;

    sp->affinePA = r0;
    r0 = 0;
    r4 = *(s16*)r4;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r4;
    r0 = FixedMul(r0, r1);
    sp->affinePB = r0;
    r4 = -r4;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = r4;
    r0 = FixedMul(r0, r1);
    sp->affinePC = r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = cosine;
    r0 = FixedMul(r0, r1);
    sp->affinePD = r0;
}

void StartBigFistAttack(void)
{
    u32 r0;
    u32 r1;
    struct PrimarySpriteData* s2;
    struct PrimarySpriteData* s3;
    s32 trig;
    s32 cosine;
    struct PrimarySpriteData* s6;
    s32 scale;

    scale = 0x100;
    s2 = &gCurrentSprite;
    r0 = (u32)&s2->work0;
    r1 = *(u8*)r0;
    if (r1 > 120) {
        r0 = s2->yPosition;
        r0 -= 2;
        s2->yPosition = r0;
        goto common;
    }

    s2 = (struct PrimarySpriteData*)((u32)s2 + 40);
    r0 = *(u8*)s2;
    if (r0 > 223) {
        scale = 0x200;
    } else if (r1 <= 23) {
        r0 += 32;
        *(u8*)s2 = r0;
        r0 = *(u8*)s2;
        scale = r0 << 1;
    } else if (r1 == 24) {
        r0 = 0x203;
        m4aSongNumStart(r0);
    }

    s3 = &gCurrentSprite;
    r0 = s3->work0;
    s2 = s3;

    if (r0 <= 7) {
        switch (gCurrentPassage) {
            case 1:
                r0 = s2->yPosition;
                r0 -= 28;
                s2->yPosition = r0;
                r0 = s2->xPosition;
                r0 += 40;
                s2->xPosition = r0;
                break;
            case 2:
                r0 = s2->yPosition;
                r0 -= 40;
                s2->yPosition = r0;
                r0 = s2->xPosition;
                r0 += 40;
                s2->xPosition = r0;
                break;
            case 3:
                r0 = s2->yPosition;
                r0 -= 24;
                s2->yPosition = r0;
                r0 = s2->xPosition;
                r0 += 48;
                s2->xPosition = r0;
                break;
            case 4:
            case 5:
                r0 = s2->yPosition;
                r0 -= 20;
                s2->yPosition = r0;
                r0 = s2->xPosition;
                r0 += 40;
                s2->xPosition = r0;
                break;
        }
        r0 = (u32)&gCurrentSprite;
        r1 = ((struct PrimarySpriteData*)r0)->roomSlot;
        s3 = (struct PrimarySpriteData*)r0;
        s3 = (struct PrimarySpriteData*)((u32)s3);
        {
            u32 y;
            y = ((struct PrimarySpriteData*)r0)->yPosition;
            y += 64;
            r0 = ((struct PrimarySpriteData*)r0)->xPosition;
            r0 -= 64;
            SpawnHighPriorityPrimarySprite(11, r1, 0, y, r0);
        }
    } else if (r0 == 8) {
        r1 = s3->status;
        r0 = 0xFFF7;
        r0 &= r1;
        s3->status = r0;
    }

common:
    s6 = &gCurrentSprite;
    r1 = (u32)&s6->work0;
    r0 = *(u8*)r1;
    r0--;
    *(u8*)r1 = r0;
    r0 <<= 24;
    if (r0 == 0) {
        r0 = 20;
        s6->pose = r0;
        r0 = 110;
        *(u8*)r1 = r0;
        r1++;
        r0 = 240;
        *(u8*)r1 = r0;
        r0 = s6->yPosition;
        r0 -= 64;
        r1 = s6->xPosition;
        r1 += 128;
        ((void (*)())SpawnSecondarySprite)(r0, r1, 51);
        func_8026E78();
        r0 = 96;
        r1 = 129;
        ScreenShakeRequestXWithAmplitude(r0, r1);
        r1 = (u32)&gShopItemState;
        r0 = 5;
        *(u8*)r1 = r0;
        r0 = 129;
        r0 <<= 2;
        m4aSongNumStart(r0);
    }

    trig = (s32)sSinCosTable;
    r0 = trig + 128;
    r1 = 0;
    cosine = *(s16*)r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = cosine;
    r0 = FixedMul(r0, r1);
    s6->affinePA = r0;
    r0 = 0;
    trig = *(s16*)trig;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s6->affinePB = r0;
    trig = -trig;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = trig;
    r0 = FixedMul(r0, r1);
    s6->affinePC = r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = cosine;
    r0 = FixedMul(r0, r1);
    s6->affinePD = r0;
}

void UpdateBigFistAttack(void)
{
    u32 r0;
    u32 r1;
    register u32 r2 PIN("r2");
    u32 r3;
    struct PrimarySpriteData* r4;
    u32 r5;
    u8* r6;
    s32 scale;
    u32 spawnFlag;

    r0 = 1;
    spawnFlag = r0;
    scale = 128;
    scale <<= 2;
    r4 = &gCurrentSprite;
    r0 = r4->yPosition;
    r0 -= 64;
    r3 = (u16)r0;
    r0 = r4->xPosition;
    r0 += 128;
    r0 <<= 16;
    r2 = r0 >> 16;
    r6 = (u8*)r4 + 39;
    r5 = *r6;
    if (r5 == 0) {
        goto timer_zero;
    }
    if (r5 <= 15) {
        r1 = (u32)r4 + 40;
        r0 = *(u8*)r1;
        scale = r0 << 1;
        r0 -= 16;
        *(u8*)r1 = r0;
    }
    r0 = *r6;
    r0--;
    *r6 = r0;
    r0 = *r6;
    r0 -= 10;
    switch (r0) {
        case 95:
            r0 = r3;
            r0 -= 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 -= 64;
            goto x_done;
        case 90:
            r0 = r3;
            r0 += 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 64;
            goto x_done;
        case 85:
            r0 = r3;
            r0 -= 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 -= 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 80:
            r0 = r3;
            r0 -= 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 75:
            r0 = r3;
            r0 += 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 -= 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 70:
            r0 = r3;
            r0 -= 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 -= 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 65:
            r0 = r3;
            r0 += 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 60:
            r0 = r3;
            r0 -= 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 -= 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 55:
            r0 = r3;
            r0 -= 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 50:
            r0 = r3;
            r0 += 64;
            r3 = (u16)r0;
            r0 = r2;
            r0 -= 64;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 45:
            r0 = r3;
            r0 -= 32;
            goto y_done_x_minus;
        case 40:
            r0 = r3;
            r0 += 32;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 32;
            goto x_done;
        case 30:
            r0 = r3;
            r0 -= 32;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 20:
            r0 = r3;
            r0 -= 32;
            ASM_BARRIER_VOLATILE(: : "r"(r0) : "memory");
            goto y_done_x_minus;
        case 15:
            r0 = r3;
            r0 += 32;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 5:
            r0 = r3;
            r0 -= 32;
            r3 = (u16)r0;
            r0 = r2;
            r0 += 32;
            ASM_BARRIER_VOLATILE(: "+r"(r0));
            goto x_done;
        case 25:
        case 0:
            goto plus32_minus32;
        case 35:
        case 10:
            goto maybe_spawn;
        default:
            goto no_spawn;
    }
plus32_minus32:
    r0 = r3;
    r0 += 32;

y_done_x_minus:
    r3 = (u16)r0;
    r0 = r2;
    r0 -= 32;
x_done:
    r0 <<= 16;
    r2 = r0 >> 16;
    goto maybe_spawn;
no_spawn:
    r1 = 0;
    spawnFlag = r1;
maybe_spawn:
    r0 = spawnFlag;
    if (r0 != 0) {
        r0 = r3;
        r1 = r2;
        r2 = 51;
        SpawnSecondarySprite(r0, r1, (u8)r2);
    }
    goto affine;

timer_zero:
    r1 = r4->roomSlot;
    ASM_BARRIER_VOLATILE(: : "r"(r1));
    r3 = r4->yPosition;
    r0 = r4->xPosition;
    r0 += 64;
    SpawnHighPriorityPrimarySprite(12, r1, 0, r3, r0);
    r0 = gCurrentPassage;
    if (r0 != 5) {
        goto passage_not_five;
    }
    r0 = r4->status;
    r1 = 64;
    r0 ^= r1;
    r2 = 0;
    r1 = 0xBF7F;
    r0 &= r1;
    r4->status = r0;
    r0 = 110;
    r4->pose = r0;
    r0 = r4->yPosition;
    r0 += 64;
    r4->yPosition = r0;
    r0 = r4->xPosition;
    r0 += 64;
    r4->xPosition = r0;
    r0 = (u32)r4 + 42;
    *(u8*)r0 = r2;
    goto end;
passage_not_five:
    r4->status = r5;
    goto end;

affine:
    r4 = (struct PrimarySpriteData*)sSinCosTable;
    r0 = (u32)r4 + 128;
    r1 = 0;
    r6 = (u8*)(s32) * (s16*)r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = (s32)r6;
    r0 = FixedMul(r0, r1);
    r5 = (u32)&gCurrentSprite;
    ((struct PrimarySpriteData*)r5)->affinePA = r0;
    r0 = 0;
    r4 = (struct PrimarySpriteData*)(s32) * (s16*)r4;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = (s32)r4;
    r0 = FixedMul(r0, r1);
    ((struct PrimarySpriteData*)r5)->affinePB = r0;
    r4 = (struct PrimarySpriteData*)(-(s32)r4);
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = (s32)r4;
    r0 = FixedMul(r0, r1);
    ((struct PrimarySpriteData*)r5)->affinePC = r0;
    r0 = scale;
    r0 = FixedInverse(r0);
    r1 = r0;
    r0 = (s32)r6;
    r0 = FixedMul(r0, r1);
    ((struct PrimarySpriteData*)r5)->affinePD = r0;
end:;
}

void UpdateBigFistCollision(void)
{
    register struct PrimarySpriteData* s PIN("r2");
    register u8* indexPtr PIN("r12");
    u32 index;
    const s16* table;
    register u32 value PIN("r0");
    s32 signedValue;
    u32 velocity;
    struct PrimarySpriteData* sp;
    s = &gCurrentSprite;
    indexPtr = &s->work3;
    index = *indexPtr;
    table = sBigFistMovementPath;
    value = index << 1;
    value += (u32)table;
    velocity = *(u16*)value;
    signedValue = *(s16*)value;
    value = 0x7FFF;
    sp = s;
    if (signedValue == value) {
        signedValue = index - 1;
        signedValue <<= 1;
        signedValue += (u32)table;
        value = sp->yPosition;
        signedValue = *(u16*)signedValue;
        value += signedValue;
    } else {
        value = index + 1;
        *indexPtr = value;
        value = sp->yPosition;
        value += velocity;
    }
    sp->yPosition = value;
    value = sp->xPosition;
    value -= 8;
    sp->xPosition = value;
    value = func_8023A60(sp->yPosition, sp->xPosition);
    signedValue = value;
    if (gSpriteCollisionResult != 0) {
        sp->yPosition = signedValue;
        sp->pose = 111;
        sp->status ^= 0x40;
    }
}

void SpriteBigFist(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
        case 0:
            InitBigFist();
            break;
        case 16:
            ShopItemFollowShopkeeper();
            break;
        case 18:
            StartBigFistAttack();
            break;
        case 20:
            UpdateBigFistAttack();
            break;
        case 110:
            UpdateBigFistCollision();
            break;
    }
}

void PlayBlackDragonAttackSoundCue(void)
{
    volatile u8* sprite;
    u32 value;
    u32 mask;

    sprite = (volatile u8*)&gCurrentSprite;
    value = *(volatile u32*)(sprite + 20);
    mask = 0xFFFFFF;
    value &= mask;
    mask = 0x20002;
    if (value == mask) {
        value = 0x206;
        m4aSongNumStart(value);
    }
}

void InitBlackDragon(void)
{
    volatile u8* sprite;
    register u32 value PIN("r0");
    u32 temp;
    register u32 zero PIN("r2");
    u32 zeroByte;
    u32 pose;

    sprite = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)sprite;
    zero = 129;
    zero <<= 3;
    ASM_BARRIER(: "+r"(zero));
    value = zero;
    zeroByte = 0;
    zero = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    *(volatile u16*)sprite = value;
    *(volatile u8*)(sprite + 30) = zeroByte;
    temp = (u32)sprite;
    temp += 32;
    value = 48;
    *(volatile u8*)temp = value;
    value = (u32)sprite;
    value += 33;
    pose = 16;
    *(volatile u8*)value = pose;
    temp += 2;
    value = 32;
    *(volatile u8*)temp = value;
    value = (u32)sprite;
    value += 35;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sBlackDragonIdleOam;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(sprite + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(sprite + 22) = zeroByte;
    *(volatile u16*)(sprite + 20) = zero;
    if (*(volatile u8*)&gCurrentRoom == 0) {
        *(volatile u8*)(sprite + 28) = pose;
    } else {
        value = 17;
        *(volatile u8*)(sprite + 28) = value;
        temp = (u32)sprite;
        temp += 39;
        value = 171;
        *(volatile u8*)temp = value;
        value = (u32)sprite;
        value += 42;
        *(volatile u8*)value = zeroByte;
    }
}

void MoveBlackDragonIntoAttackPosition(void)
{
    struct PrimarySpriteData* sprite;
    register u8* indexPtr PIN("r12");
    register u32 index PIN("r3");
    register const s16* table PIN("r5");
    const s16* velocityPtr;
    register u32 velocity PIN("r4");
    register u32 zero PIN("r6");
    register u32 temp PIN("r0");
    u32 y;

    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sBlackDragonBobPath;
    velocityPtr = (const s16*)((index << 1) + (u32)table);
    velocity = *(const u16*)velocityPtr;
    zero = 0;
    y = *(const s16*)((const u8*)velocityPtr + zero);
    temp = S16_MAX;
    if (y == temp) {
        velocity = *(const u16*)table;
        index = 0;
    }
    temp = index + 1;
    y = (u32)indexPtr;
    *(volatile u8*)y = temp;
    temp = sprite->yPosition;
    y = temp + velocity;
    sprite->yPosition = y;

    temp = sprite->work0;
    if (temp > 110) {
        if (temp == 120) {
            y = sprite->status;
            temp = 0xFFF7;
            temp &= y;
            sprite->status = temp;
        }
    } else if (temp > 60) {
        switch (gCurrentPassage) {
            case 1:
            case 3:
                temp = sprite->yPosition;
                temp -= 2;
                sprite->yPosition = temp;
                temp = sprite->xPosition;
                temp++;
                sprite->xPosition = temp;
                break;
            case 2:
                temp = y - 3;
                sprite->yPosition = temp;
                temp = sprite->xPosition;
                temp++;
                sprite->xPosition = temp;
                break;
            case 5:
                temp = y - 1;
                sprite->yPosition = temp;
                temp = sprite->xPosition;
                temp++;
                sprite->xPosition = temp;
                break;
        }
    } else if (temp == 60) {
        temp = (u32)sBlackDragonMoveOam;
        GP_SET(sprite->pOamData, (const struct AnimationFrame*)temp);
        temp = 0;
        sprite->currentAnimationFrame = temp;
        /* agbcc reuses r0 here; this single move preserves the required second zero before strh. */
        ASM_BARRIER_VOLATILE(: "=r"(temp));
        temp = 0;
        sprite->animationTimer = 0;
    }

    zero = 39;
    ASM_BARRIER(: "+r"(zero));
    zero += (u32)sprite;
    temp = *(volatile u8*)zero;
    temp--;
    *(volatile u8*)zero = temp;
    index = (u8)temp;
    if (index == 0) {
        temp = 18;
        sprite->pose = temp;
        temp = (u32)sBlackDragonAttackOam;
        GP_SET(sprite->pOamData, (const struct AnimationFrame*)temp);
        sprite->currentAnimationFrame = index;
        y = 0;
        sprite->animationTimer = index;
        *(volatile u8*)zero = y;
        temp = (u32)sprite;
        temp += 40;
        *(volatile u8*)temp = y;
        temp++;
        *(volatile u8*)temp = y;
        temp++;
        *(volatile u8*)temp = y;
        temp = 0x207;
        m4aSongNumStart(temp);
    }
}

void UpdateBlackDragonAttackReturn(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u8* work2Ptr;
    u32 timer;
    u32 temp;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    if (timer <= 49) {
        temp = timer + 1;
        work2Ptr = timerPtr;
        *work2Ptr = temp;
        work2Ptr = &sprite->work2;
        timer = *work2Ptr;
        if (timer <= 9) {
            timer = 7;
            temp &= timer;
            if (temp == 0) {
                temp = *work2Ptr;
                SpawnHighPriorityPrimarySprite(202, temp, 0, sprite->yPosition - 72, sprite->xPosition + 48);
                timer = *work2Ptr;
                timer++;
                *work2Ptr = timer;
                timer = (u8)timer;
                if (timer == 5) {
                    func_8026E78();
                }
            }
        }
    } else {
        u8* indexPtr;
        u32 index;
        const s16* table;
        const s16* velocityPtr;
        u32 velocity;
        register u32 zero PIN("r4");
        register s32 signedVelocity PIN("r1");
        u8* work1Ptr;

        indexPtr = &sprite->work3;
        index = *indexPtr;
        table = sBlackDragonAttackReturnPath;
        velocityPtr = (const s16*)((index << 1) + (u32)table);
        velocity = *(const u16*)velocityPtr;
        zero = 0;
        signedVelocity = *(const s16*)((const u8*)velocityPtr + zero);
        timer = S16_MAX;
        work1Ptr = &sprite->work1;
        if (signedVelocity == timer) {
            velocity = *(const u16*)table;
            index = 0;
            timer = *work1Ptr;
            timer++;
            *work1Ptr = timer;
        }
        timer = index + 1;
        *indexPtr = timer;
        timer = sprite->yPosition;
        timer += velocity;
        temp = 0;
        sprite->yPosition = timer;
        timer = *work1Ptr;
        if (timer > 1) {
            timer = 19;
            sprite->pose = timer;
            GP_SET(sprite->pOamData, sBlackDragonIdleOam);
            *(volatile u8*)((u32)sprite + 22) = temp;
            timer = 0;
            sprite->animationTimer = timer;
            *indexPtr = temp;
            switch (gCurrentPassage) {
                case 2:
                    timer = 88;
                    work2Ptr = timerPtr;
                    *work2Ptr = timer;
                    break;
                case 5:
                    timer = 48;
                    *timerPtr = timer;
                    break;
                default:
                    timer = 174;
                    *timerPtr = timer;
                    break;
            }
        }
    }
}

void UpdateBlackDragonAttack(void)
{
    register u16 offset PIN("r4");

    switch (gCurrentPassage) {
        case 2:
            if (gCurrentSprite.work0 <= 71) {
                u32 index;
                u8* counter;
                gCurrentSprite.xPosition -= 8;
                counter = &gCurrentSprite.work3;
                index = *counter;
                offset = sBlackDragonBobPath[index];
                if (sBlackDragonBobPath[index] == 0x7FFF) {
                    offset = sBlackDragonBobPath[0];
                    index = 0;
                }
                *counter = index + 1;
                gCurrentSprite.yPosition += offset;
            } else if (gCurrentSprite.work0 == 73) {
                gCurrentSprite.status ^= 0x40;
                m4aSongNumStart(0x2AD);
            }
            break;
        case 5:
            if (gCurrentSprite.work0 <= 31) {
                u32 index;
                u8* counter;
                gCurrentSprite.xPosition -= 4;
                counter = &gCurrentSprite.work3;
                index = *counter;
                offset = sBlackDragonBobPath[index];
                if (sBlackDragonBobPath[index] == 0x7FFF) {
                    offset = sBlackDragonBobPath[0];
                    index = 0;
                }
                *counter = index + 1;
                gCurrentSprite.yPosition += offset;
            } else if (gCurrentSprite.work0 == 33) {
                gCurrentSprite.status ^= 0x40;
            }
            break;
        default:
            if (gCurrentSprite.work0 <= 144) {
                u32 index;
                u8* counter;
                gCurrentSprite.xPosition -= 4;
                counter = &gCurrentSprite.work3;
                index = *counter;
                offset = sBlackDragonBobPath[index];
                if (sBlackDragonBobPath[index] == 0x7FFF) {
                    offset = sBlackDragonBobPath[0];
                    index = 0;
                }
                *counter = index + 1;
                gCurrentSprite.yPosition += offset;
            } else if (gCurrentSprite.work0 == 145) {
                gCurrentSprite.status ^= 0x40;
                m4aSongNumStart(0x2AD);
            }
            break;
    }

    if (--gCurrentSprite.work0 == 0) {
        if (gCurrentPassage == 5) {
            gCurrentSprite.status ^= 0x40;
            gCurrentSprite.pose = 20;
        } else {
            gCurrentSprite.status = 0;
        }
    }
}

void UpdateBlackDragonBob(void)
{
    struct PrimarySpriteData* sprite;
    u8* indexPtr;
    u32 index;
    const s16* table;
    const s16* velocityPtr;
    u32 velocity;
    s32 signedVelocity;
    u32 zero;

    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sBlackDragonBobPath;
    velocityPtr = (const s16*)((index << 1) + (u32)table);
    velocity = *(const u16*)velocityPtr;
    zero = 0;
    signedVelocity = *(const s16*)((const u8*)velocityPtr + zero);
    if (signedVelocity == S16_MAX) {
        velocity = *(const u16*)table;
        index = 0;
    }
    {
        u32 nextIndex;
        u8* destination;

        nextIndex = index + 1;
        destination = indexPtr;
        *destination = nextIndex;
    }
    sprite->yPosition += velocity;
}

void InitBlackDragonFire(void)
{
    register volatile u8* sprite PIN("r12");
    u32 value;
    register u32 temp PIN("r1");
    u32 size;
    register u32 zero PIN("r3");
    register u32 zeroHalf PIN("r4");

    sprite = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)sprite;
    size = 0x400;
    ASM_BARRIER(: "+r"(size), "+r"(temp));
    value = size;
    ASM_BARRIER(: "+r"(value));
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)sprite;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    value = (u32)sprite;
    value += 32;
    size = 16;
    *(volatile u8*)value = size;
    value++;
    *(volatile u8*)value = size;
    temp += 34;
    value = 8;
    *(volatile u8*)temp = value;
    value = (u32)sprite;
    value += 35;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sBlackDragonFireOam;
    temp = (u32)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = zero;
    *(volatile u16*)(temp + 20) = zeroHalf;
    *(volatile u8*)(temp + 28) = size;
    temp = *(volatile u8*)(temp + 24);
    temp <<= 1;
    value = 10;
    value -= temp;
    temp = (u32)sprite;
    temp += 40;
    value <<= 2;
    *(volatile u8*)temp = value;
}

void MoveBlackDragonFireIntoPosition(void)
{
    if (gCurrentSprite.work1 != 0) {
        gCurrentSprite.work1--;
        gCurrentSprite.xPosition += 10;
    } else {
        gCurrentSprite.pose = 18;
        gCurrentSprite.work0 = 5;
    }
}

void UpdateBlackDragonFireAttack(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u32 timer;
    u32 slot;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    timer--;
    *timerPtr = timer;
    timer <<= 24;
    if (timer != 0) {
        return;
    }

    slot = sprite->roomSlot;
    if (slot == 4) {
        SpriteUtilFindSpriteSlotWork3(196);
    } else {
        slot++;
        slot = (u8)slot;
        SpriteUtilFindOwnSlot(slot);
    }

    {
        struct PrimarySpriteData* sprite2;
        u32 zero;
        u32 pose;

        sprite2 = &gCurrentSprite;
        zero = 0;
        pose = 20;
        sprite2->pose = pose;
        sprite2->work0 = zero;
    }
}

void UpdateBlackDragonFireCollision(void)
{
    volatile u8* sprite;
    volatile u8* data;
    register u32 value PIN("r0");
    register u32 counter PIN("r1");

    sprite = (volatile u8*)&gCurrentSprite;
    data = (volatile u8*)gSpriteData;
    value = (u32)sprite;
    value += 42;
    counter = *(volatile u8*)value;
    value = 44;
    value *= counter;
    value += (u32)data;
    value = ((volatile struct PrimarySpriteData *)(u32)value)->yPosition;
    *(volatile u16*)(sprite + 8) = value;

    data = sprite;
    data += 39;
    value = *data;
    ASM_BARRIER(: "+r"(value));
    counter = value + 1;
    *data = counter;
    value = 7;
    value &= counter;
    data = (volatile u8*)(u32) * (volatile u8*)(sprite + 24);
    if (value == (u32)data) {
        value = counter;
        value <<= 24;
        value >>= 27;
        value += (u32)data;
        counter = 1;
        value &= counter;
        if (value != 0) {
            value = ((volatile struct PrimarySpriteData *)sprite)->yPosition;
            value += 32;
            counter = ((volatile struct PrimarySpriteData *)sprite)->xPosition;
            counter -= 32;
            data = (volatile u8*)51;
            SpawnSecondarySprite(value, counter, (u8)(u32)data);
        } else {
            value = ((volatile struct PrimarySpriteData *)sprite)->yPosition;
            value -= 32;
            counter = ((volatile struct PrimarySpriteData *)sprite)->xPosition;
            counter += 32;
            data = (volatile u8*)51;
            SpawnSecondarySprite(value, counter, (u8)(u32)data);
        }
    }

    sprite = (volatile u8*)&gCurrentSprite;
    value = ((volatile struct PrimarySpriteData *)sprite)->roomSlot;
    if (value == 4) {
        value = ((volatile struct PrimarySpriteData *)sprite)->yPosition;
        value -= 72;
        *(volatile u16*)(sprite + 8) = value;
        data = (volatile u8*)gSpriteData;
        value = (u32)sprite;
        value += 42;
        counter = *(volatile u8*)value;
        value = 44;
        value *= counter;
        value += (u32)data;
        value = ((volatile struct PrimarySpriteData *)(u32)value)->pose;
        if (value == 19) {
            value = 22;
            *(volatile u8*)(sprite + 28) = value;
            counter = (u32)sprite;
            counter += 39;
            value = 8;
            *(volatile u8*)counter = value;
        }
    } else {
        data = (volatile u8*)gSpriteData;
        value = (u32)sprite;
        value += 42;
        counter = *(volatile u8*)value;
        value = 44;
        value *= counter;
        value += (u32)data;
        value = *(volatile u8*)(value + 28);
        if (value == 22) {
            *(volatile u8*)(sprite + 28) = value;
            counter = (u32)sprite;
            counter += 39;
            value = 8;
            *(volatile u8*)counter = value;
        }
    }
}

void UpdateBlackDragonFireLifetime(void)
{
    struct PrimarySpriteData* sprite;
    register u32 value PIN("r0");
    u8* timerPtr;

    sprite = &gCurrentSprite;
    value = sprite->status;
    value ^= 4;
    sprite->status = value;
    timerPtr = &sprite->work0;
    value = *timerPtr;
    value--;
    *timerPtr = value;
    value = (u8)value;
    if (value == 0) {
        sprite->status = value;
    }
}

void SpriteBlackDragon(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;

    switch (gCurrentSprite.pose) {
        case 0:
            InitBlackDragon();
            break;
        case 16:
            ShopItemFollowShopkeeper();
            break;
        case 17:
            MoveBlackDragonIntoAttackPosition();
            break;
        case 18:
            UpdateBlackDragonAttackReturn();
            break;
        case 19:
            UpdateBlackDragonAttack();
            break;
        case 20:
            UpdateBlackDragonBob();
            break;
    }
    PlayBlackDragonAttackSoundCue();
}

void SpriteBlackDragonFire(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;

    switch (gCurrentSprite.pose) {
        case 0:
            InitBlackDragonFire();
            break;
        case 16:
            MoveBlackDragonFireIntoPosition();
            break;
        case 18:
            UpdateBlackDragonFireAttack();
            break;
        case 20:
            UpdateBlackDragonFireCollision();
            break;
        case 22:
            UpdateBlackDragonFireLifetime();
            break;
    }
}

void InitBossTreasureChest(void)
{
    register volatile u8* sprite PIN("r12");
    u32 value;
    register u32 temp PIN("r1");
    u32 collision;
    register u32 zero PIN("r4");
    register u32 zeroHalf PIN("r5");
    register u32 extent PIN("r3");

    sprite = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)sprite;
    collision = 0x408;
    ASM_BARRIER(: : "r"(collision));
    value = collision;
    ASM_BARRIER(: "+r"(value));
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)sprite;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    value = (u32)sprite + 32;
    collision = 16;
    *(volatile u8*)value = collision;
    value++;
    *(volatile u8*)value = collision;
    value++;
    *(volatile u8*)value = collision;
    value++;
    extent = 64;
    *(volatile u8*)value = extent;
    value++;
    temp = 60;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = extent;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sBossTreasureChestInitialOam;
    temp = (u32)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(temp + 4), (const struct AnimationFrame*)value);
    *(volatile u8*)(temp + 22) = zero;
    *(volatile u16*)(temp + 20) = zeroHalf;
    *(volatile u8*)(temp + 28) = collision;
    temp += 39;
    value = 72;
    *(volatile u8*)temp = value;
    value = (u32)sprite + 40;
    *(volatile u8*)value = zero;
    VoiceSetPlay(21);
}

void StartBossTreasureChestOpening(void)
{
    struct PrimarySpriteData* sprite;
    u32 timer;
    u8* timerPtr;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    timer--;
    *timerPtr = timer;
    {
        u32 timerValue;
        timerValue = (u8)timer;
        if (timerValue == 0) {
            const struct AnimationFrame* animation;
            u32 zero;
            u8* workPtr;

            animation = sBossTreasureChestOpeningOam;
            GP_SET(sprite->pOamData, animation);
            sprite->currentAnimationFrame = timerValue;
            zero = 0;
            sprite->animationTimer = timerValue;
            workPtr = &sprite->work3;
            *workPtr = zero;
            zero = 18;
            sprite->pose = zero;
        }
    }
}

void UpdateBossTreasureChestRise(void)
{
    struct PrimarySpriteData* sprite;
    u8* indexPtr;
    u32 index;
    const s16* table;
    const s16* velocityPtr;
    u32 velocity;
    u32 scratch;
    s32 signedVelocity;

    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sBossTreasureRiseVelocityTable;
    velocityPtr = (const s16*)((index << 1) + (u32)table);
    velocity = *(const u16*)velocityPtr;
    scratch = 0;
    signedVelocity = *velocityPtr;
    if (signedVelocity == S16_MAX) {
        u32 zero;
        u8* workPtr;

        zero = 0;
        sprite->pose = 24;
        *indexPtr = zero;
        workPtr = &sprite->work0;
        *workPtr = 240;
    } else {
        u32 nextIndex;
        u8* destination;

        nextIndex = index + 1;
        nextIndex = (u8)nextIndex;
        index = nextIndex;
        destination = indexPtr;
        *destination = index;
        sprite->yPosition += velocity;
        if (index == 18) {
            VoiceSetPlay(19);
        }
    }
}

void UpdateBossTreasureChestOpening(void)
{
    struct PrimarySpriteData* sprite;
    u8* indexPtr;
    u32 index;
    const s16* table;
    const s16* velocityPtr;
    u32 velocity;
    u32 zero;
    s32 signedVelocity;
    u8* timerPtr;
    u32 timer;
    u32 timerValue;
    u8* treasureFlag;

    sprite = &gCurrentSprite;
    indexPtr = &sprite->work3;
    index = *indexPtr;
    table = sBossTreasureOpeningVelocityTable;
    velocityPtr = (const s16*)((index << 1) + (u32)table);
    velocity = *(const u16*)velocityPtr;
    zero = 0;
    signedVelocity = *(const s16*)((const u8*)velocityPtr + zero);
    if (signedVelocity == S16_MAX) {
        velocity = *(const u16*)table;
        index = 0;
    }
    {
        u32 nextIndex;
        u8* destination;

        nextIndex = index + 1;
        destination = indexPtr;
        *destination = nextIndex;
    }
    sprite->yPosition += velocity;

    timerPtr = &sprite->work0;
    timer = *timerPtr;
    if (timer == 0) {
        return;
    }
    timer--;
    *timerPtr = timer;
    timerValue = (u8)timer;
    if (timerValue != 0 || gTimerState == 11) {
        return;
    }

    GP_SET(sprite->pOamData, sBossTreasureChestDespawnOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = timerValue;
    sprite->pose = 22;
    *timerPtr = 48;
    m4aSongNumStart(SOUND_1D7);

    switch (sprite->roomSlot) {
        case 0:
            treasureFlag = &gCollectedNEJewelPiece;
            break;
        case 1:
            treasureFlag = &gCollectedSEJewelPiece;
            break;
        case 2:
            treasureFlag = &gCollectedSWJewelPiece;
            break;
        case 3:
            treasureFlag = &gCollectedNWJewelPiece;
            break;
        case 4:
            treasureFlag = &gHasBossTreasure5;
            break;
        case 5:
            treasureFlag = &gHasBossTreasure6;
            break;
        case 6:
            treasureFlag = &gHasBossTreasure7;
            break;
        case 7:
            treasureFlag = &gHasBossTreasure8;
            break;
        case 8:
            treasureFlag = &gHasBossTreasure9;
            break;
        case 9:
            treasureFlag = &gHasBossTreasure10;
            break;
        case 10:
            treasureFlag = &gHasBossTreasure11;
            break;
        case 11:
            treasureFlag = &gHasBossTreasure12;
            break;
        default:
            return;
    }
    *treasureFlag = 0;
}

void SpawnBossTreasureRewards(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u32 value;
    register u32 status PIN("r1");

    sprite = &gCurrentSprite;
    status = sprite->status;
    value = status & SPRITE_STATUS_ONSCREEN;
    value = (u16)value;
    if (value != 0) {
        sprite->yPosition -= 8;
        timerPtr = &sprite->work1;
        status = *timerPtr;
        value = status & 7;
        if (value == 0) {
            SpawnSecondarySprite(sprite->yPosition + 32, sprite->xPosition, 71);
        }
        value = *timerPtr;
        value++;
        *timerPtr = value;
    } else {
        sprite->status = value;
    }
}

void BeginBossTreasureChestDespawn(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u32 timer;

    sprite = &gCurrentSprite;
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    timer--;
    *timerPtr = timer;
    timer = (u8)timer;
    if (timer == 0) {
        sprite->status = timer;
    }
}

void WaitBossTreasureChestDespawn(void)
{
    struct PrimarySpriteData* sprite;
    u32 zero;
    u32 pose;

    sprite = &gCurrentSprite;
    zero = 0;
    pose = 111;
    sprite->pose = pose;
    sprite->work3 = zero;
}

void UpdateBossTreasureRewards(void)
{
    register struct PrimarySpriteData* sprite;
    u32 r0;
    register s32 r1 PIN("r1");
    register u32 r2 PIN("r2");
    u32 r3;
    register u32 offset PIN("r4");
    register u32 velocity PIN("r5");
    register const s16* table PIN("r6");
    u8* indexPtr;

    r2 = (u32)&gCurrentSprite;
    r0 = 42;
    ASM_BARRIER_VOLATILE(: "+r"(r0));
    r0 += r2;
    indexPtr = (u8*)r0;
    r3 = *(u8*)r0;
    table = sSpriteGravityVelocityTable;
    r0 = r3 << 1;
    r0 += (u32)table;
    velocity = *(u16*)r0;
    sprite = 0;
    r1 = *(s16*)(r0 + (u32)sprite);
    r0 = S16_MAX;
    sprite = (struct PrimarySpriteData*)r2;
    if (r1 != r0) {
        goto table_regular;
    }
    r1 = r3 - 1;
    r1 <<= 1;
    r1 += (u32)table;
    r0 = sprite->yPosition;
    r1 = *(u16*)r1;
    r0 += r1;
    goto table_done;

table_regular:
    r0 = r3 + 1;
    r1 = (u32)indexPtr;
    *(u8*)r1 = r0;
    r0 = sprite->yPosition;
    r0 += velocity;
table_done:
    sprite->yPosition = r0;

    /*
     * Valid shopkeeper room slots are 0 through 11. The default arm is kept to
     * preserve the original dispatch layout; initializing offset for that
     * unreachable arm changes r4 allocation and breaks the object match.
     */
    switch (sprite->roomSlot) {
        case 0:
            goto pair_se_sw;
        case 1:
            goto single_sw;
        case 2:
            goto offset_64;
        case 3:
            goto pair_5_6;
        case 4:
            goto single_6;
        case 5:
            goto offset_64;
        case 6:
            goto pair_8_9;
        case 7:
            goto single_9;
        case 8:
            goto offset_64;
        case 9:
            goto pair_11_12;
        case 10:
            goto single_12;
        case 11:
            goto offset_64;
        default:
            goto offset_done;
    }

single_sw:
    r0 = (u32)&gCollectedSWJewelPiece;
    goto single_test;
single_6:
    r0 = (u32)&gHasBossTreasure6;
    goto single_test;
single_9:
    r0 = (u32)&gHasBossTreasure9;
    goto single_test;
single_12:
    r0 = (u32)&gHasBossTreasure12;
single_test:
    r0 = *(u8*)r0;
    offset = 64;
    if (r0 == 0) {
        goto offset_done;
    }
    offset = 192;
    goto offset_done;

pair_se_sw:
    r0 = (u32)&gCollectedSEJewelPiece;
    r1 = *(u8*)r0;
    r2 = r0;
    if (r1 == 0) {
        goto pair_se_sw_one;
    }
    r0 = (u32)&gCollectedSWJewelPiece;
    r0 = *(u8*)r0;
    if (r0 != 0) {
        goto offset_320;
    }
pair_se_sw_one:
    r0 = *(u8*)r2;
    offset = 192;
    if (r0 != 0) {
        goto offset_done;
    }
    r0 = (u32)&gCollectedSWJewelPiece;
    goto second_test;

pair_5_6:
    r0 = (u32)&gHasBossTreasure5;
    r1 = *(u8*)r0;
    r2 = r0;
    if (r1 == 0) {
        goto pair_5_6_one;
    }
    r0 = (u32)&gHasBossTreasure6;
    r0 = *(u8*)r0;
    if (r0 != 0) {
        goto offset_320;
    }
pair_5_6_one:
    r0 = *(u8*)r2;
    offset = 192;
    if (r0 != 0) {
        goto offset_done;
    }
    r0 = (u32)&gHasBossTreasure6;
    goto second_test;

pair_8_9:
    r0 = (u32)&gHasBossTreasure8;
    r1 = *(u8*)r0;
    r2 = r0;
    if (r1 == 0) {
        goto pair_8_9_one;
    }
    r0 = (u32)&gHasBossTreasure9;
    r0 = *(u8*)r0;
    if (r0 != 0) {
        goto offset_320;
    }
pair_8_9_one:
    r0 = *(u8*)r2;
    offset = 192;
    if (r0 != 0) {
        goto offset_done;
    }
    r0 = (u32)&gHasBossTreasure9;
    goto second_test;

pair_11_12:
    r0 = (u32)&gHasBossTreasure11;
    r1 = *(u8*)r0;
    r2 = r0;
    if (r1 == 0) {
        goto pair_11_12_one;
    }
    r0 = (u32)&gHasBossTreasure12;
    r0 = *(u8*)r0;
    if (r0 == 0) {
        goto pair_11_12_one;
    }
offset_320:
    offset = 160;
    offset <<= 1;
    goto offset_done;
pair_11_12_one:
    r0 = *(u8*)r2;
    offset = 192;
    if (r0 != 0) {
        goto offset_done;
    }
    r0 = (u32)&gHasBossTreasure12;
second_test:
    r0 = *(u8*)r0;
    if (r0 != 0) {
        goto offset_done;
    }
offset_64:
    offset = 64;
offset_done:
    r0 = sprite->yPosition;
    r0 += offset;
    r0 = (u16)r0;
    r1 = sprite->xPosition;
    r0 = func_8023A60(r0, r1);
    r1 = r0;
    r0 = gSpriteCollisionResult;
    if (r0 == 0) {
        goto end;
    }
    r0 = r1 - offset;
    r1 = 0;
    r2 = 0;
    sprite->yPosition = r0;
    r0 = 112;
    sprite->pose = r0;
    r0 = (u32)sBossTreasureRewardOam;
    GP_SET(sprite->pOamData, (const struct AnimationFrame*)r0);
    sprite->currentAnimationFrame = r1;
    sprite->animationTimer = r2;
    r0 = 22;
    VoiceSetPlay(r0);
end:
    return;
}

void SpriteBossTreasureChest(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;

    if ((u8)(gTimerState - 2) <= 8) {
        gCurrentSprite.currentAnimationFrame--;
        return;
    }

    switch (gCurrentSprite.pose) {
        case 0:
            InitBossTreasureChest();
            break;
        case 16:
            StartBossTreasureChestOpening();
            break;
        case 18:
            UpdateBossTreasureChestRise();
            break;
        case 24:
            UpdateBossTreasureChestOpening();
            break;
        case 20:
            SpawnBossTreasureRewards();
            break;
        case 22:
            BeginBossTreasureChestDespawn();
            break;
        case 110:
            WaitBossTreasureChestDespawn();
        case 111:
            UpdateBossTreasureRewards();
            break;
    }
}

void SpriteShopkeeper(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;

    switch (gCurrentSprite.pose) {
        case 0:
            InitShopkeeper();
            break;
        case 1:
            WaitForShopkeeperRoomReady();
            break;
        case 2:
            SetupShopkeeperSelectedItem();
            break;
        case 16:
            ShopkeeperUpdateGraphicsScript(sShopkeeperDefaultGraphicsScript);
            ShopkeeperFollowWario();
            break;
        case 17:
            MoveShopkeeperAppleBombIntoPosition();
            break;
        case 18:
            UpdateShopkeeperAppleBombPresentation();
            break;
        case 19:
            MoveShopkeeperBlastCannonIntoPosition();
            break;
        case 20:
            WaitForShopkeeperBlastCannonReady();
            break;
        case 21:
            StartShopkeeperBlastCannonAttack();
        case 22:
            MoveBlastCannonWithShopkeeper();
            break;
        case 110:
            MoveShopkeeperVizormanIntoPosition();
            break;
        case 111:
            WaitForShopkeeperVizormanReady();
            break;
        case 112:
            FinishShopkeeperVizormanSetup();
            break;
        case 113:
            MoveVizormanWithShopkeeper();
            break;
        case 114:
            MoveShopkeeperBugleIntoPosition();
            break;
        case 115:
            WaitForShopkeeperBugleReady();
            break;
        case 116:
            FinishShopkeeperBugleSetup();
            break;
        case 117:
            MoveBugleWithShopkeeper();
            break;
        case 118:
            MoveShopkeeperTransformationIntoPosition();
            break;
        case 119:
            SpawnShopkeeperTransformationItem();
            break;
        case 25:
            BeginShopkeeperItemWait();
        case 26:
            UpdateShopkeeperItemWait();
            break;
        case 23:
            ReplaceShopkeeperWithTransitionSprite();
        case 24:
            BeginShopkeeperPostItemIdle();
            break;
        case 49:
            UpdateShopkeeperPostItemIdle();
            break;
        case 120:
            BeginShopkeeperExit();
        case 121:
            UpdateShopkeeperExit();
            break;
        case 55:
            MoveShopkeeperToSmilePosition();
            break;
        case 56:
            StartShopkeeperSmilePose();
            break;
        case 57:
            UpdateShopkeeperSmilePose();
            break;
        case 58:
            MoveShopkeeperSmileOffscreen();
            break;
        case 59:
            WaitForShopkeeperSmileEffect();
            break;
        case 60:
            RotateShopkeeperSmile();
            break;
        case 61:
            ShrinkShopkeeperSmile();
            break;
        case 63:
            StartShopkeeperSmileDrop();
        case 64:
            UpdateShopkeeperSmileDrop();
            break;
        case 65:
            StartShopkeeperSmileJump();
        case 66:
            UpdateShopkeeperSmileJump();
            break;
    }
}

void UpdateShopkeeperCommonAnimation(void)
{
    register volatile u8* sprite PIN("r12");
    u32 value;
    register u32 temp PIN("r1");
    u32 zero;
    register u32 zeroHalf PIN("r3");

    sprite = (volatile u8*)&gCurrentSprite;
    temp = *(volatile u16*)sprite;
    zero = 0x400;
    ASM_BARRIER(: : "r"(zero));
    value = zero;
    zero = 0;
    zeroHalf = 0;
    value |= temp;
    temp = 0xFFFB;
    value &= temp;
    temp = (u32)sprite;
    *(volatile u16*)temp = value;
    *(volatile u8*)(temp + 30) = zero;
    temp += 32;
    value = 64;
    *(volatile u8*)temp = value;
    temp++;
    value = 24;
    *(volatile u8*)temp = value;
    temp++;
    value = 32;
    *(volatile u8*)temp = value;
    value = (u32)sprite;
    value += 35;
    temp = 4;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value++;
    *(volatile u8*)value = temp;
    value = (u32)sprite;
    *(volatile u8*)(value + 22) = zero;
    *(volatile u16*)(value + 20) = zeroHalf;
    value = 16;
    temp = (u32)sprite;
    *(volatile u8*)(temp + 28) = value;
}

void SpriteShopEffectShortB(void)
{
    struct PrimarySpriteData *sprite;
    u32 timer;
    u8 *timerPtr;
    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    if (sprite->pose == 0) {
        UpdateShopkeeperCommonAnimation();
        GP_SET(sprite->pOamData, sShopEffectShortBOam);
        sprite->work0 = 25;
    }
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    timer--;
    *timerPtr = timer;
    if ((u8)timer == 0)
        sprite->status = 0;
}

void SpriteShopEffectShortA(void)
{
    struct PrimarySpriteData *sprite;
    u32 timer;
    u8 *timerPtr;
    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    if (sprite->pose == 0) {
        UpdateShopkeeperCommonAnimation();
        GP_SET(sprite->pOamData, sShopEffectShortAOam);
        sprite->work0 = 25;
    }
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    timer--;
    *timerPtr = timer;
    if ((u8)timer == 0)
        sprite->status = 0;
}

void SpriteShopEffectLong(void)
{
    struct PrimarySpriteData *sprite;
    u32 timer;
    u8 *timerPtr;
    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    if (sprite->pose == 0) {
        UpdateShopkeeperCommonAnimation();
        GP_SET(sprite->pOamData, sShopEffectLongOam);
        sprite->work0 = 43;
    }
    timerPtr = &sprite->work0;
    timer = *timerPtr;
    timer--;
    *timerPtr = timer;
    if ((u8)timer == 0)
        sprite->status = 0;
}

void SpriteShopkeeperTransition(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;

    switch (gCurrentSprite.pose) {
        case 0:
            InitShopkeeperTransition();
            break;
        case 18:
            UpdateShopkeeperTransitionFall();
            break;
        case 111:
            FinishShopkeeperTransitionWait();
            break;
        case 113:
            StartShopkeeperTransitionLanding();
            break;
        case 115:
            UpdateShopkeeperTransitionRun();
            break;
        case 116:
            WaitForWarioTransitionPose();
            break;
        case 117:
            BeginShopkeeperTransformationReturn();
            break;
        case 119:
            BeginShopkeeperTransitionReturn();
            break;
        case 121:
            UpdateShopkeeperTransitionDeparture();
            break;
    }
}

#define SPRITE_IMPLEMENTATION
#include "sprite.h"

#include "background_registers.h"
#include "global_data.h"
#include "main.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "wario.h"

#include "sprite_ai/aerodent.h"
#include "sprite_ai/ball_lightning.h"
#include "sprite_ai/beezley.h"
#include "sprite_ai/boss_corridor_cat.h"
#include "sprite_ai/boss_door.h"
#include "sprite_ai/boss_treasure.h"
#include "sprite_ai/bow_balloon.h"
#include "sprite_ai/bowler.h"
#include "sprite_ai/bubble.h"
#include "sprite_ai/butatabi.h"
#include "sprite_ai/catbat.h"
#include "sprite_ai/caterpillar.h"
#include "sprite_ai/chance_wheel.h"
#include "sprite_ai/chandelier.h"
#include "sprite_ai/coins.h"
#include "sprite_ai/cractus.h"
#include "sprite_ai/crumbling_block.h"
#include "sprite_ai/cuckoo_condor.h"
#include "sprite_ai/deburina.h"
#include "sprite_ai/denden.h"
#include "sprite_ai/dice.h"
#include "sprite_ai/dice_block.h"
#include "sprite_ai/dominoes.h"
#include "sprite_ai/electric_lift.h"
#include "sprite_ai/falling_snow.h"
#include "sprite_ai/flying_carpet.h"
#include "sprite_ai/folding_door.h"
#include "sprite_ai/ghost.h"
#include "sprite_ai/glass_bird.h"
#include "sprite_ai/goggley_blade.h"
#include "sprite_ai/golden_diva.h"
#include "sprite_ai/hoggus.h"
#include "sprite_ai/icicle.h"
#include "sprite_ai/kaentsubo.h"
#include "sprite_ai/keyzer.h"
#include "sprite_ai/magma.h"
#include "sprite_ai/marumen.h"
#include "sprite_ai/mayubird.h"
#include "sprite_ai/menhammer.h"
#include "sprite_ai/menono.h"
#include "sprite_ai/minicula.h"
#include "sprite_ai/moguramen.h"
#include "sprite_ai/money_flower.h"
#include "sprite_ai/money_sunflower.h"
#include "sprite_ai/onomi.h"
#include "sprite_ai/pencil.h"
#include "sprite_ai/petbottom.h"
#include "sprite_ai/pig_head_statue.h"
#include "sprite_ai/pinball.h"
#include "sprite_ai/piston.h"
#include "sprite_ai/professor.h"
#include "sprite_ai/ringosuki.h"
#include "sprite_ai/robobird.h"
#include "sprite_ai/rock.h"
#include "sprite_ai/rotating_platform.h"
#include "sprite_ai/shieragutchi.h"
#include "sprite_ai/shopkeeper.h"
#include "sprite_ai/skeleton_bird.h"
#include "sprite_ai/spear_mask.h"
#include "sprite_ai/spike_cannon.h"
#include "sprite_ai/spiky.h"
#include "sprite_ai/spoiled_rotten.h"
#include "sprite_ai/sprite_a9.h"
#include "sprite_ai/swinging_platform.h"
#include "sprite_ai/switch.h"
#include "sprite_ai/tobawani.h"
#include "sprite_ai/togemasuku.h"
#include "sprite_ai/togenobi.h"
#include "sprite_ai/togerobo.h"
#include "sprite_ai/togetoge.h"
#include "sprite_ai/totsumen.h"
#include "sprite_ai/toy_block_round.h"
#include "sprite_ai/toy_block_square.h"
#include "sprite_ai/toy_block_triangle.h"
#include "sprite_ai/toy_car.h"
#include "sprite_ai/treasure.h"
#include "sprite_ai/utsuboanko.h"
#include "sprite_ai/vortex.h"
#include "sprite_ai/yeti.h"

extern ProcedureFunc sPrimarySpriteAITable[];
extern GBA_PTR(const u8*) const sPrimarySpriteGraphicsPointers[];
extern GBA_PTR(const u8*) const sPrimarySpritePalettePointers[];
extern GBA_PTR(const u8*) const sRoomSpriteGraphicsLists[];
extern const u32 sPrimarySpriteGraphicsSizes[];
extern const u8 sOamWidthByShapeAndSize[];

void UpdatePrimarySprites(void)
{
    register s32 spriteIndex PIN("r6");
    struct PrimarySpriteData* spriteBase;
    struct PrimarySpriteData* currentSprite;
    ProcedureFunc* aiTable;
    struct PrimarySpriteData* sprite;
    register u32 value PIN("r0");

    if (gSubGameMode == 2) {
        SpawnRoomSpritesAtCameraBoundary(3);
        if (gWarioPauseTimer == 0) {
            SpriteCollisionProcess();
            func_80267C0();
            spriteIndex = 0;
            spriteBase = gSpriteData;
            currentSprite = &gCurrentSprite;
            aiTable = sPrimarySpriteAITable;
        firstLoop:
            value = sizeof(struct PrimarySpriteData);
            value *= spriteIndex;
            sprite = (struct PrimarySpriteData*)(value + (u32)spriteBase);
            {
                u32 status;
                status = sprite->status;
                value = 1;
                value &= status;
            }
            if (value != 0) {
                *currentSprite = *sprite;
                aiTable[currentSprite->globalID]();
                AdvanceCurrentSpriteAnimation();
                UpdateCurrentSpriteVisibility();
                *sprite = *currentSprite;
            }
            spriteIndex++;
            if (spriteIndex <= 23) {
                goto firstLoop;
            }
            return;
        }

        spriteIndex = 0;
        currentSprite = &gCurrentSprite;
    secondLoop: {
        u32 spriteBaseAddress = (u32)gSpriteData;
        value = sizeof(struct PrimarySpriteData);
        value *= spriteIndex;
        sprite = (struct PrimarySpriteData*)(value + spriteBaseAddress);
    }
        {
            u32 status;
            status = sprite->status;
            value = 1;
            value &= status;
        }
        if (value != 0 && (sprite->drawPriority & 0x80) != 0) {
            *currentSprite = *sprite;
            sPrimarySpriteAITable[currentSprite->globalID]();
            AdvanceCurrentSpriteAnimation();
            UpdateCurrentSpriteVisibility();
            *sprite = *currentSprite;
        }
        spriteIndex++;
        if (spriteIndex <= 23) {
            goto secondLoop;
        }
        return;
    }

    if (gSubGameMode == 8) {
        SpawnRoomSpritesAtCameraBoundary(3);
        spriteIndex = 0;
        spriteBase = gSpriteData;
        currentSprite = &gCurrentSprite;
        aiTable = sPrimarySpriteAITable;
    thirdLoop:
        value = sizeof(struct PrimarySpriteData);
        value *= spriteIndex;
        sprite = (struct PrimarySpriteData*)(value + (u32)spriteBase);
        if ((sprite->status & 1) != 0) {
            *currentSprite = *sprite;
            aiTable[currentSprite->globalID]();
            AdvanceCurrentSpriteAnimation();
            UpdateCurrentSpriteVisibility();
            *sprite = *currentSprite;
        }
        spriteIndex++;
        if (spriteIndex <= 23) {
            goto thirdLoop;
        }
        return;
    }

    SpawnRoomSpritesAtCameraBoundary(3);
    spriteIndex = 0;
fourthLoop: {
    u32 spriteBaseAddress = (u32)gSpriteData;
    value = sizeof(struct PrimarySpriteData);
    value *= spriteIndex;
    sprite = (struct PrimarySpriteData*)(value + spriteBaseAddress);
}
    if ((sprite->status & 1) != 0) {
        currentSprite = &gCurrentSprite;
        *currentSprite = *sprite;
        if (currentSprite->pose <= 14) {
            sPrimarySpriteAITable[currentSprite->globalID]();
        }
        UpdateCurrentSpriteVisibility();
        *sprite = *currentSprite;
    }
    spriteIndex++;
    if (spriteIndex <= 23) {
        goto fourthLoop;
    }
}

void AdvanceCurrentSpriteAnimation(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->currentAnimationFrame++;
    if (GP(sprite->pOamData)[sprite->animationTimer].time < sprite->currentAnimationFrame) {
        sprite->currentAnimationFrame = 1;
        sprite->animationTimer++;
        if (GP(sprite->pOamData)[sprite->animationTimer].time == 0) {
            sprite->animationTimer = 0;
        }
    }
}

void DrawPrimarySprites(void)
{
    s32 spriteIndex;
    struct PrimarySpriteData* spriteBase;
    struct PrimarySpriteData* currentSprite;
    GBA_TYPEOF(const u16**, &gCurrentSpriteOamData) currentOam;
    u32 temp0;
    u32 temp1;
    struct PrimarySpriteData* sprite;
    temp0 = gCurrentStageNumber;
    if (temp0 > 3) {
        goto laterStages;
    }
    spriteIndex = 0;
    spriteBase = gSpriteData;
    currentSprite = &gCurrentSprite;
    currentOam = &gCurrentSpriteOamData;
firstLoop:
    temp0 = sizeof(struct PrimarySpriteData);

    temp0 *= spriteIndex;
    temp1 = (u32)spriteBase;
    sprite = (struct PrimarySpriteData*)(temp0 + temp1);
    temp1 = sprite->status;
    temp0 = 15;
    temp0 &= temp1;
    if (temp0 == 3) {
        *currentSprite = *sprite;
        temp0 = currentSprite->animationTimer;
        temp1 = (u32)GP(currentSprite->pOamData);
        temp0 <<= 3;
        temp0 += temp1;
        temp0 = *((u32*)temp0);
        GP_SET(*currentOam, (const u16*)temp0);
        temp0 = spriteIndex;
        DrawCurrentPrimarySprite(temp0);
    }
    spriteIndex++;
    if (spriteIndex <= 23) {
        goto firstLoop;
    }
    spriteIndex = 0;
    {
        struct PrimarySpriteData* baseTemporary;
        baseTemporary = (spriteBase = gSpriteData);
    }
    currentSprite = &gCurrentSprite;
    currentOam = &gCurrentSpriteOamData;
secondLoop:
    temp0 = sizeof(struct PrimarySpriteData);

    temp0 *= spriteIndex;
    {
        u32 spriteBaseAddress;
        spriteBaseAddress = (u32)spriteBase;
        sprite = (struct PrimarySpriteData*)(temp0 + spriteBaseAddress);
    }
    temp1 = sprite->status;
    temp0 = 15;
    temp0 &= temp1;
    if (temp0 == 11) {
        *currentSprite = *sprite;
        temp0 = currentSprite->animationTimer;
        temp1 = (u32)GP(currentSprite->pOamData);
        temp0 <<= 3;
        temp0 += temp1;
        temp0 = *((u32*)temp0);
        GP_SET(*currentOam, (const u16*)temp0);
        temp0 = spriteIndex;
        DrawCurrentPrimarySprite(temp0);
    }
    spriteIndex++;
    if (spriteIndex <= 23) {
        goto secondLoop;
    }
    return;
laterStages:
    spriteIndex = 0;

    {
        struct PrimarySpriteData* baseTemporary;
        baseTemporary = (spriteBase = gSpriteData);
    }
    currentSprite = &gCurrentSprite;
    currentOam = &gCurrentSpriteOamData;
thirdLoop:
    temp0 = sizeof(struct PrimarySpriteData);

    temp0 *= spriteIndex;
    {
        u32 spriteBaseAddress;
        spriteBaseAddress = (u32)spriteBase;
        sprite = (struct PrimarySpriteData*)(temp0 + spriteBaseAddress);
    }
    temp1 = sprite->status;
    temp0 = 15;
    temp0 &= temp1;
    if (temp0 == 3) {
        *currentSprite = *sprite;
        temp0 = currentSprite->animationTimer;
        temp1 = (u32)GP(currentSprite->pOamData);
        temp0 <<= 3;
        temp0 += temp1;
        temp0 = *((u32*)temp0);
        GP_SET(*currentOam, (const u16*)temp0);
        temp0 = spriteIndex;
        DrawCurrentPrimarySprite(temp0);
    }
    spriteIndex++;
    if (spriteIndex <= 23) {
        goto thirdLoop;
    }
    spriteIndex = 0;
    {
        struct PrimarySpriteData* baseTemporary;
        baseTemporary = (spriteBase = gSpriteData);
    }
    currentSprite = &gCurrentSprite;
    currentOam = &gCurrentSpriteOamData;
fourthLoop:
    temp0 = sizeof(struct PrimarySpriteData);

    temp0 *= spriteIndex;
    {
        u32 spriteBaseAddress;
        spriteBaseAddress = (u32)spriteBase;
        sprite = (struct PrimarySpriteData*)(temp0 + spriteBaseAddress);
    }
    temp1 = sprite->status;
    temp0 = 15;
    temp0 &= temp1;
    if ((temp0 == 11) && (sprite->globalID != 14)) {
        *currentSprite = *sprite;
        temp0 = currentSprite->animationTimer;
        temp1 = (u32)GP(currentSprite->pOamData);
        temp0 <<= 3;
        temp0 += temp1;
        temp0 = *((u32*)temp0);
        GP_SET(*currentOam, (const u16*)temp0);
        temp0 = spriteIndex;
        DrawCurrentPrimarySprite(temp0);
    }
    spriteIndex++;
    if (spriteIndex <= 23) {
        goto fourthLoop;
    }
    spriteIndex = 0;
    {
        struct PrimarySpriteData* baseTemporary;
        baseTemporary = (spriteBase = gSpriteData);
    }
    currentSprite = &gCurrentSprite;
    currentOam = &gCurrentSpriteOamData;
fifthLoop:
    temp0 = sizeof(struct PrimarySpriteData);

    temp0 *= spriteIndex;
    {
        u32 spriteBaseAddress;
        spriteBaseAddress = (u32)spriteBase;
        sprite = (struct PrimarySpriteData*)(temp0 + spriteBaseAddress);
    }
    temp1 = sprite->status;
    temp0 = 15;
    temp0 &= temp1;
    if ((temp0 == 11) && (sprite->globalID == 14)) {
        *currentSprite = *sprite;
        temp0 = currentSprite->animationTimer;
        temp1 = (u32)GP(currentSprite->pOamData);
        temp0 <<= 3;
        temp0 += temp1;
        temp0 = *((u32*)temp0);
        GP_SET(*currentOam, (const u16*)temp0);
        temp0 = spriteIndex;
        DrawCurrentPrimarySprite(temp0);
    }
    spriteIndex++;
    if (spriteIndex <= 23) {
        goto fifthLoop;
    }
}

void DrawCurrentPrimarySprite(u32 spriteIndex)
{
    register u32 r0 PIN("r0");
    register u32 r1 PIN("r1");
    register u32 r2 PIN("r2");
    register u32 r3 PIN("r3");
    u32 r4;
    u32 r5;
    register u32 r6 PIN("r6");
    u32 r7;
    register u32 r8 PIN("r8");
    register u32 r9 PIN("r9");
    u32 r10;
    register u32 r12 PIN("r12");
    volatile u32 yBase;
    volatile u32 hFlip;
    volatile u32 affine;
    volatile u32 doubleSize;
    volatile u32 blend;
    u32 paletteOffset;
    u32 tileOffset;
    u32 priorityBits;
    u32 totalSlots;

    r12 = spriteIndex;
    r0 = (u32)&gOamSlotsUsed;
    r1 = *(u8*)r0;
    ASM_BARRIER(: "+r"(r1));
    r8 = r1;
    r1 = (u32)&gCurrentSpriteOamData;
    r6 = *(u32*)r1;
    r2 = *(u16*)r6;
    ASM_BARRIER(: "+r"(r2));
    r9 = r2;
    r6 += 2;
    r1 = r9;
    r1 += r8;
    if ((s32)r1 > 127) {
        goto done;
    }

    r3 = r8;
    ASM_BARRIER(: "+r"(r3));
    r0 = r3 << 3;
    r3 = (u32)gOamBuffer;
    r5 = r0 + r3;
    r2 = (u32)&gCurrentSprite;
    r1 = *(u16*)(r2 + 8);
    r1 >>= 2;
    r0 = (u32)&gBg1YPosition;
    r0 = *(u16*)r0;
    r0 >>= 2;
    r1 -= r0;
    r1 = (u16)r1;
    yBase = r1;
    r1 = *(u16*)(r2 + 10);
    r1 >>= 2;
    r0 = (u32)&gBg1XPosition;
    r0 = *(u16*)r0;
    r0 >>= 2;
    r1 -= r0;
    r1 = (u16)r1;
    r10 = r1;
    r1 = *(u16*)r2;
    r0 = 0x40;
    r0 &= r1;
    r0 = (u16)r0;
    hFlip = r0;
    r0 = 0x80;
    r0 &= r1;
    r0 = (u16)r0;
    affine = r0;
    r0 = 0x4000;
    r0 &= r1;
    r0 = (u16)r0;
    doubleSize = r0;
    r0 = 0x2000;
    r0 &= r1;
    r0 = (u16)r0;
    blend = r0;
    r1 = *(u8*)(r2 + 25);
    r0 = *(u8*)(r2 + 27);
    r0 = r1 + r0;
    paletteOffset = r0;
    r1 <<= 6;
    tileOffset = r1;
    r0 = *(u8*)(r2 + 26);
    r4 = 15;
    r4 &= r0;
    r0 = (u32)&gDrawWarioOverBackground;
    r0 = *(u8*)r0;
    if (r0 != 0 && r4 != 0) {
        r4--;
    }
    r0 = r9;
    r0 += r8;
    totalSlots = r0;
    r1 = r9;
    if (r1 == 0) {
        goto after_entries;
    }

    r1 = (u32)gOamBuffer;
    r7 = 1;
    r2 = r8;
    r0 = r2 << 3;
    r3 = r0 + r1;
    r0 = 3;
    r4 &= r0;
    r4 <<= 2;
    priorityBits = r4;
    r8 = r9;

entry_loop:
    r0 = *(u16*)r6;
    r6 += 2;
    *(u16*)r5 = r0;
    r5 += 2;
    r1 = yBase;
    r0 += r1;
    *(u8*)r3 = r0;

    r4 = *(u16*)r6;
    r6 += 2;
    *(u16*)r5 = r4;
    r5 += 2;
    r2 = r10;
    ASM_BARRIER(: "+r"(r2));
    r1 = r4 + r2;
    r0 = 0x1FF;
    r1 &= r0;
    r2 = *(u16*)(r3 + 2);
    r0 = ~0x1FF;
    r0 &= r2;
    r0 |= r1;
    *(u16*)(r3 + 2) = r0;

    r0 = *(u16*)r6;
    *(u16*)r5 = r0;
    r6 += 2;
    r5 += 2;

    r1 = *(u8*)(r3 + 5);
    r0 = 13;
    r0 = -r0;
    r0 &= r1;
    r1 = priorityBits;
    r0 |= r1;
    r2 = r0 >> 4;
    r1 = paletteOffset;
    r2 += r1;
    r2 <<= 4;
    r1 = 15;
    r0 &= r1;
    r0 |= r2;
    *(u8*)(r3 + 5) = r0;

    r2 = *(u16*)(r3 + 4);
    r1 = r2 << 22;
    r1 >>= 22;
    r0 = tileOffset;
    r1 += r0;
    r0 = 0x3FF;
    r9 = r0;
    ASM_BARRIER(: "+r"(r9));
    r0 = r9;
    r1 &= r0;
    r0 = ~0x3FF;
    r0 &= r2;
    r0 |= r1;
    *(u16*)(r3 + 4) = r0;

    r1 = hFlip;
    if (r1 != 0) {
        r2 = *(u8*)(r3 + 3);
        r0 = r2 << 27;
        r0 >>= 31;
        r0 ^= r7;
        r0 &= r7;
        r0 <<= 4;
        r1 = 17;
        r1 = -r1;
        r1 &= r2;
        r1 |= r0;
        *(u8*)(r3 + 3) = r1;
        r0 = *(u8*)(r3 + 1);
        r0 >>= 6;
        r1 >>= 6;
        r0 <<= 2;
        r1 += r0;
        r2 = (u32)sOamWidthByShapeAndSize;
        r1 += r2;
        r1 = *(u8*)r1;
        r1 <<= 3;
        r1 = r4 + r1;
        r0 = r10;
        ASM_BARRIER(: "+r"(r0));
        r1 = r0 - r1;
        r2 = 0x1FF;
        r1 &= r2;
        r2 = *(u16*)(r3 + 2);
        r0 = ~0x1FF;
        r0 &= r2;
        r0 |= r1;
        *(u16*)(r3 + 2) = r0;
    }

    r0 = affine;
    if (r0 != 0) {
        r1 = doubleSize;
        if (r1 != 0) {
            r0 = *(u8*)(r3 + 1);
            r2 = 3;
            r0 |= r2;
        } else {
            r1 = *(u8*)(r3 + 1);
            r0 = 4;
            r0 = -r0;
            r0 &= r1;
            r0 |= r7;
        }
        *(u8*)(r3 + 1) = r0;

        r1 = r12;
        r0 = (s32)r1 >> 4;
        r0 &= r7;
        r0 <<= 5;
        r2 = *(u8*)(r3 + 3);
        r1 = 33;
        r1 = -r1;
        r1 &= r2;
        r1 |= r0;
        r2 = r12;
        r0 = (s32)r2 >> 3;
        r0 &= r7;
        r0 <<= 4;
        r2 = 17;
        r2 = -r2;
        r1 &= r2;
        r1 |= r0;
        r0 = 7;
        r2 = r12;
        r2 &= r0;
        r2 <<= 1;
        r0 = 15;
        ASM_BARRIER(: "+r"(r0));
        r0 = -r0;
        r1 &= r0;
        r1 |= r2;
        *(u8*)(r3 + 3) = r1;
    }

    r0 = blend;
    if (r0 != 0) {
        r0 = *(u8*)(r3 + 1);
        r1 = 13;
        r1 = -r1;
        r1 &= r0;
        r0 = 4;
        r1 |= r0;
        *(u8*)(r3 + 1) = r1;
    }

    r5 += 2;
    r3 += 8;
    r1 = 1;
    r1 = -r1;
    r8 += r1;
    r2 = r8;
    if (r2 != 0) {
        goto entry_loop;
    }

after_entries:
    ASM_BARRIER(: : "r"(r0), "r"(r1), "r"(r2));
    gOamSlotsUsed = (u8)totalSlots;
    r0 = affine;
    if (r0 != 0) {
        r1 = r12;
        r0 = r1 << 5;
        r2 = (u32)gOamBuffer;
        r0 += r2;
        r3 = (u32)&gCurrentSprite;
        r1 = *(u16*)(r3 + 12);
        *(u16*)(r0 + 6) = r1;
        r0 = r12;
        r1 = r0 << 2;
        r0 = r1 + 1;
        r0 <<= 3;
        r0 += r2;
        r2 = *(u16*)(r3 + 14);
        *(u16*)(r0 + 6) = r2;
        r0 = r1 + 2;
        r0 <<= 3;
        r2 = (u32)gOamBuffer;
        r0 += r2;
        r2 = *(u16*)(r3 + 16);
        *(u16*)(r0 + 6) = r2;
        r1 += 3;
        r1 <<= 3;
        r3 = (u32)gOamBuffer;
        r1 += r3;
        r2 = (u32)&gCurrentSprite;
        r0 = *(u16*)(r2 + 18);
        *(u16*)(r1 + 6) = r0;
    }

done:
    return;
}

void UpdateCurrentSpriteVisibility(void)
{
    register u16 status PIN("r10");
    u16 bgY;
    u32 bgX;
    u16 spriteY;
    u32 spriteX;
    u16 bgYShifted;
    u16 spriteYShifted;
    u16 top;
    u32 firstTop;
    u16 bottom;
    u16 bgXShifted;
    u16 spriteXShifted;
    u16 left;
    u16 right;
    register u8* persistent PIN("r1");
    u16 new_var;
    register u16 statusMask PIN("r0");
    u8 persistentValue;
    unsigned long long new_var2;
    u16 secondBgYShifted;
    u16 secondBottom;
    u16 secondBgX;
    u16 secondSpriteX;
    u16 secondSpriteYShifted;
    status = gCurrentSprite.status;
    if ((status & 1) == 0) {
        return;
    }
    bgY = gBg1YPosition;
    bgX = gBg1XPosition;
    spriteY = gCurrentSprite.yPosition;
    right = (spriteX = gCurrentSprite.xPosition);
    bgYShifted = bgY + 0x100;
    spriteYShifted = spriteY + 0x100;
    firstTop = (u16)(bgYShifted - (gCurrentSprite.drawDistanceUp * 4));
    bottom = 0x280;
    bottom = bgYShifted + ((gCurrentSprite.drawDistanceDown * 4) + bottom);
    bgXShifted = bgX + 0x100;
    spriteXShifted = spriteX + 0x100;
    left = bgXShifted - (gCurrentSprite.drawDistanceLeftRight * 4);
    right = 0x3C0;
    right = bgXShifted + ((gCurrentSprite.drawDistanceLeftRight * 4) + right);
    if ((((left < spriteXShifted) && (spriteXShifted < right)) && (firstTop < spriteYShifted)) &&
        (spriteYShifted < bottom)) {
        gCurrentSprite.status = status | 2;
        return;
    }
    gCurrentSprite.status &= 0xFFFD;
    secondSpriteYShifted = bgY + 0x280;
    secondBgYShifted = secondSpriteYShifted;
    secondSpriteYShifted = spriteY + 0x280;
    top = secondBgYShifted - 0x240;
    secondBottom = secondBgYShifted + 0x4C0;
    secondSpriteX = bgX + 0x280;
    secondBgX = secondSpriteX;
    secondSpriteX = spriteX + 0x280;
    left = secondBgX - 0x240;
    right = secondBgX + 0x600;
    if ((((left < secondSpriteX) && (secondSpriteX < right)) && (top < secondSpriteYShifted)) &&
        (secondSpriteYShifted < secondBottom)) {
        return;
    }
    new_var = gCurrentSprite.status;
    if ((new_var & 0x8000) != 0) {
        return;
        if (1) {
        }
    }
    statusMask = 0x10;
    statusMask &= new_var;
    if (statusMask != 0) {
        gCurrentSprite.status = 0;
        return;
    }
    gCurrentSprite.status = 0;
    gCurrentSprite.globalID = 16;
    if (gCurrentSprite.health == 0) {
        return;
    }
    persistent = &gPersistentSpriteData[0][0];
    {
        u32 persistentOffset = (((u32)gCurrentRoom) << 6) + gCurrentSprite.roomSlot;
        persistent = (u8*)(persistentOffset + (u32)persistent);
    }
    persistentValue = *persistent;
    if ((persistentValue & 0xF) == 1) {
        *persistent = persistentValue - 1;
    }
}

void InitializePrimarySprites(void)
{
    LoadRoomSpriteGraphics();
    if (gPauseFlag == 0) {
        LoadRoomSpriteData();
        if (gHasTemporarySave == 0) {
            ResetPersistentSpriteData();
            ClearPrimarySpriteSlots();
            SpawnVisibleRoomSprites();
            SpawnRoomSpritesAtCameraBoundary(1);
            SpawnRoomSpritesAtCameraBoundary(2);
        }
    }
}

void LoadRoomSpriteGraphics(void)
{
    register u32 r0 PIN("r0");
    register u32 r1 PIN("r1");
    u32 r2;
    register u32 r3 PIN("r3");
    u32 r4;
    u32 r5;
    register u32 r6 PIN("r6");
    s32 r7;
    u32 r8;
    register u32 r9 PIN("r9");
    register u32 r10 PIN("r10");
    u32 r12;

    r7 = 0;
    r6 = (u32)&gUnk_3000027;
    r4 = (u32)gRoomSpriteIds;
    r3 = 16;
    r2 = (u32)gRoomSpriteGraphicsSlots;
    r1 = 7;
init_loop:
    r0 = r7 + r4;
    *(u8*)r0 = r3;
    r0 = r7 + r2;
    *(u8*)r0 = r1;
    r7++;
    if (r7 <= 31) {
        goto init_loop;
    }

    r2 = 0x040000D4;
    r1 = (u32)sPrimarySpriteGraphicsPointers;
    r0 = *(u32*)r1;
    *(volatile u32*)(r2 + 0) = r0;
    r0 = 0x06017800;
    *(volatile u32*)(r2 + 4) = r0;
    r3 = (u32)sPrimarySpriteGraphicsSizes;
    r0 = *(u32*)r3;
    r0 >>= 1;
    r5 = 0x80000000;
    r0 |= r5;
    *(volatile u32*)(r2 + 8) = r0;
    r0 = *(volatile u32*)(r2 + 8);

    r1 = (u32)sPrimarySpritePalettePointers;
    r0 = *(u32*)r1;
    *(volatile u32*)(r2 + 0) = r0;
    r0 = 0x050003E0;
    *(volatile u32*)(r2 + 4) = r0;
    r0 = 0x80000010;
    *(volatile u32*)(r2 + 8) = r0;
    r0 = *(volatile u32*)(r2 + 8);

    r3 = 0xFF;
    ASM_BARRIER(: "+r"(r3));
    r9 = r3;
    r0 = *(u8*)r6;
    if ((s32)r0 > 90) {
        r0 = 0;
    }
    r7 = 0;
    r1 = r0 << 2;
    r4 = (u32)sRoomSpriteGraphicsLists;
    r0 = r1 + r4;
    r0 = *(u32*)r0;
    r3 = *(u8*)r0;
    r10 = r1;
    if (r3 == 0) {
        goto done;
    }
    r6 = r2;
    r8 = r5;
    r0 = 0;
    r12 = r0;
main_loop:
    r0 = r1 + r4;
    r0 = *(u32*)r0;
    r0 += r12;
    r5 = *(u8*)(r0 + 1);
    r1 = (u32)gRoomSpriteIds;
    r0 = r7 + r1;
    *(u8*)r0 = r3;
    r0 = (u32)gRoomSpriteGraphicsSlots;
    r2 = r7 + r0;
    r1 = 7;
    r0 = r5;
    r0 &= r1;
    *(u8*)r2 = r0;
    if (r5 == r9) {
        goto advance;
    }
    r9 = r5;
    if (r5 == 8) {
        goto advance;
    }
    r3 -= 16;
    r2 = r3 << 2;
    r1 = (u32)sPrimarySpriteGraphicsPointers;
    r0 = r2 + r1;
    r3 = *(u32*)r0;
    r0 = r5 << 11;
    r1 = 0x06014000;
    r4 = r0 + r1;
    r1 = (u32)sPrimarySpriteGraphicsSizes;
    r0 = r2 + r1;
    r1 = *(u32*)r0;
    *(volatile u32*)(r6 + 0) = r3;
    *(volatile u32*)(r6 + 4) = r4;
    r0 = r1 >> 1;
    r3 = r8;
    r0 |= r3;
    *(volatile u32*)(r6 + 8) = r0;
    r0 = *(volatile u32*)(r6 + 8);
    r0 = (u32)sPrimarySpritePalettePointers;
    r2 += r0;
    r2 = *(u32*)r2;
    r0 = r5 << 5;
    r3 = 0x05000300;
    r4 = r0 + r3;
    r1 >>= 11;
    *(volatile u32*)(r6 + 0) = r2;
    *(volatile u32*)(r6 + 4) = r4;
    r1 <<= 4;
    r0 = r8;
    r1 |= r0;
    *(volatile u32*)(r6 + 8) = r1;
    r0 = *(volatile u32*)(r6 + 8);
advance:
    r1 = 2;
    r12 += r1;
    r7++;
    if (r7 > 31) {
        goto done;
    }
    r1 = r10;
    r4 = (u32)sRoomSpriteGraphicsLists;
    r0 = r1 + r4;
    r0 = *(u32*)r0;
    r0 += r12;
    r3 = *(u8*)r0;
    if (r3 != 0) {
        goto main_loop;
    }
done:
    return;
}

void ResetPersistentSpriteData(void)
{
    if (gStageEntryPauseMenuDisabled != 0) {
        s32 i;
        u8* persistent;
        u8* currentRoom;
        u32 mask;

        i = 0;
        persistent = &gPersistentSpriteData[0][0];
        currentRoom = &gCurrentRoom;
        mask = 0xF;
        do {
            register u8* data PIN("r3");
            register u32 address PIN("r0");
            u32 value;
            address = *currentRoom;
            address <<= 6;
            address = i + address;
            data = (u8*)(address + (u32)persistent);
            value = *data;
            if ((value & mask) == 1) {
                *data = value - 1;
            }
            i++;
        } while (i <= 63);
    } else {
        s32 room;
        u8* persistent;
        u32 zero;

        room = 0;
        persistent = &gPersistentSpriteData[0][0];
        zero = 0;
        do {
            register s32 i PIN("r2");
            u8* data;
            data = (u8*)(room << 6);
            room++;
            i = 63;
            data = (u8*)((u32)data + (u32)persistent);
            data += 63;
            do {
                *data = zero;
                data--;
                i--;
            } while (i >= 0);
        } while (room <= 15);
    }
}

void LoadRoomSpriteData(void)
{
    struct RoomHeader* roomHeader;
    u32 fill;
    register u8* destination PIN("r2");
    register s32 offset PIN("r4");
    u8* middle;
    s32 count;
    u32 value;
    u8* pointer;

    roomHeader = &gCurrentRoomHeader;
    fill = 0xFF;
    destination = &gRoomSpriteData[0][0];
    offset = 0;
    middle = destination + 1;
    count = 63;
    do {
        value = destination[0];
        value |= fill;
        destination[0] = value;
        pointer = (u8*)offset;
        pointer += (u32)middle;
        value = *pointer;
        value |= fill;
        *pointer = value;
        value = destination[2];
        value |= fill;
        destination[2] = value;
        destination += 3;
        offset += 3;
        count--;
    } while (count >= 0);

    value = *(u8*)GP(roomHeader->pHardSpriteData);
    if (value != 0xFF) {
        struct RoomHeader* header;
        s32 sourceOffset;
        u8* end;
        u8* middleBase;

        header = roomHeader;
        sourceOffset = 0;
        destination = &gRoomSpriteData[0][0];
        end = destination + 189;
        middleBase = destination + 1;
        do {
            destination[0] = value;
            pointer = (u8*)sourceOffset;
            pointer += (u32)middleBase;
            value = *((u8*)((u32)sourceOffset + (u32)GP(header->pHardSpriteData)) + 1);
            *pointer = value;
            value = *((u8*)((u32)sourceOffset + (u32)GP(header->pHardSpriteData)) + 2);
            destination[2] = value;
            sourceOffset += 3;
            destination += 3;
            if ((s32)destination > (s32)end) {
                break;
            }
            value = *((u8*)((u32)sourceOffset + (u32)GP(header->pHardSpriteData)));
        } while (value != 0xFF);
    }
}

void ClearPrimarySpriteSlots(void)
{
    s32 base;
    s32 address;
    u32 zero;
    s32 offset;

    base = (s32)gSpriteData;
    zero = 0;
    offset = 253;
    offset <<= 2;
    address = base + offset;
    do {
        *(u16*)address = zero;
        address -= sizeof(struct PrimarySpriteData);
    } while (address >= base);
}

void SpawnRoomSprite(u32 roomSlot, u32 y, u32 x, u32 persistent)
{
    u32 slot;
    u32 persistentValue;
    register u32 index PIN("r4");
    struct PrimarySpriteData* spriteBase;
    register u32 stride PIN("r8");
    u32 one;
    register u8* globalIdPointer PIN("ip");
    register u32 spriteY PIN("sl");
    register u32 temp0 PIN("r0");
    u32 temp1;
    register u32 temp2 PIN("r2");
    struct PrimarySpriteData* sprite;
    u32 xPosition;

    slot = roomSlot;
    xPosition = x;
    ASM_BARRIER(: "+m"(xPosition));
    persistentValue = (u8)persistent;
    index = 0;
    spriteBase = gSpriteData;
    temp2 = sizeof(struct PrimarySpriteData);
    ASM_BARRIER(: "+r"(temp2));
    stride = temp2;
    one = 1;
    temp2 = (u32)&gRoomSpriteData[0][2];
    temp0 = slot << 1;
    temp0 += slot;
    temp0 += temp2;
    globalIdPointer = (u8*)temp0;
    temp1 = y << 6;
    temp1 += 64;
    spriteY = temp1;

loop:
    temp0 = stride;
    temp0 *= index;
    temp1 = (u32)spriteBase;
    sprite = (struct PrimarySpriteData*)(temp0 + temp1);
    temp0 = sprite->status;
    temp2 = one;
    temp2 &= temp0;
    if (temp2 != 0) {
        goto next;
    }

    temp1 = (u32)&gCurrentRoom;
    temp0 = *(u8*)temp1;
    temp0 <<= 6;
    temp0 = slot + temp0;
    temp1 = (u32)&gPersistentSpriteData[0][0];
    temp0 += temp1;
    temp1 = persistentValue + 1;
    *(u8*)temp0 = temp1;

    temp1 = (u32)globalIdPointer;
    temp0 = *(u8*)temp1;
    if (temp0 <= 16) {
        goto normalId;
    }

    temp0 -= 1;
    temp0 = (u8)temp0;
    ASM_BARRIER(: "+r"(temp0));
    temp1 = temp0;
    temp1 -= 16;
    temp2 = (u32)gRoomSpriteGraphicsSlots;
    temp0 = temp1;
    temp0 += temp2;
    temp0 = *(u8*)temp0;
    sprite->gfxSlot = temp0;
    temp0 = (u32)&gRoomSpriteIds[0];
    temp1 += temp0;
    temp0 = *(u8*)temp1;
    goto storeId;

normalId:
    sprite->gfxSlot = temp2;
    temp0 -= 1;
storeId:
    sprite->globalID = temp0;

    temp1 = stride;
    temp1 *= index;
    temp1 += (u32)spriteBase;
    ((struct PrimarySpriteData*)temp1)->status = one;
    temp2 = spriteY;
    ((struct PrimarySpriteData*)temp1)->yPosition = temp2;
    temp2 = xPosition;
    temp0 = temp2 << 6;
    temp0 += 32;
    ((struct PrimarySpriteData*)temp1)->xPosition = temp0;
    ((struct PrimarySpriteData*)temp1)->roomSlot = slot;
    temp0 = 2;
    ((struct PrimarySpriteData*)temp1)->drawPriority = temp0;
    temp0 = persistentValue >> 4;
    ((struct PrimarySpriteData*)temp1)->pose = temp0;
    ((struct PrimarySpriteData*)temp1)->health = one;
    temp0 = 0;
    ((struct PrimarySpriteData*)temp1)->palette = temp0;
    ((struct PrimarySpriteData*)temp1)->disableWarioCollisionTimer = one;
    goto done;

next:
    temp0 = index + 1;
    index = (u8)temp0;
    if (index <= 23) {
        goto loop;
    }
done:
    return;
}

void SpawnVisibleRoomSprites(void)
{
    s32 minY;
    s32 minX;
    s32 maxY;
    s32 maxX;
    s32 slot;
    u8* persistentBase;
    s32 offset;
    u32 data;
    u32 y;
    register u32 temp PIN("r0");

    temp = gBg1YPosition;
    minY = temp >> 6;
    temp = gBg1XPosition;
    minX = temp >> 6;
    temp = 9;
    temp = temp + minY;
    maxY = temp;
    temp = 15;
    temp = temp + minX;
    maxX = temp;
    slot = 0;
    data = (u32)&gRoomSpriteData[0][0];
    y = *(u8*)data;
    if (y == 0xFF) {
        return;
    }
    persistentBase = &gPersistentSpriteData[0][0];
    offset = 0;
    do {
        u32 persistent;
        temp = gCurrentRoom;
        temp <<= 6;
        temp = slot + temp;
        temp += (u32)persistentBase;
        persistent = *(u8*)temp;
        if ((persistent & 0xF) == 0) {
            temp = data + 1;
            temp = offset + temp;
            data = *(u8*)temp;
            if (minY <= (s32)y && (s32)y <= maxY && minX <= (s32)data && (s32)data <= maxX) {
                SpawnRoomSprite(slot, y, data, persistent);
            }
        }
        offset += 3;
        slot++;
        if (slot > 63) {
            return;
        }
        data = (u32)&gRoomSpriteData[0][0];
        temp = offset + data;
        y = *(u8*)temp;
    } while (y != 0xFF);
}

void SpawnRoomSpritesAtCameraBoundary(u32 distance)
{
    s32 minX;
    s32 minY;
    s32 maxX;
    s32 maxY;
    register s32 slot PIN("r5");
    u8* roomData;
    u32 persistent;
    u32 x;
    u32 y;
    u32 temp0;
    register u32 temp1 PIN("r1");

    temp1 = (u32)&gBg1YPosition;
    y = *(u16*)temp1;
    y >>= 6;
    temp1 = (u32)&gBg1XPosition;
    temp1 = *(u16*)temp1;
    temp1 >>= 6;
    minX = temp1 - distance;
    x = distance;
    x += 15;
    temp1 += x;
    maxX = temp1;
    minY = y - distance;
    distance += 9;
    y += distance;
    maxY = y;
    if (minX < 0) {
        minX = 0;
    }
    if (minY < 0) {
        minY = 0;
    }

    slot = 0;
    roomData = &gRoomSpriteData[0][0];
    do {
        temp0 = (u32)roomData;
        y = *(u8*)temp0;
        if (y == 0xFF) {
            return;
        }

        temp1 = (u32)&gPersistentSpriteData[0][0];
        temp0 = gCurrentRoom;
        temp0 <<= 6;
        temp0 = (slot) - (-(temp0));
        temp0 += temp1;
        persistent = *(u8*)temp0;
        if ((persistent & 0xF) == 0) {
            temp0 = (u32)roomData;
            x = *((u8*)temp0 + 1);
            if (maxX == (s32)x) {
                if (minY <= (s32)y && (s32)y <= maxY) {
                    goto spawn;
                }
            }
            if (minX == (s32)x) {
                if (minY <= (s32)y && (s32)y <= maxY) {
                    goto spawn;
                }
            }
            if (maxY == (s32)y) {
                if (minX <= (s32)x && (s32)x <= maxX) {
                    goto spawn;
                }
            }
            goto fourthCheck;
        spawn:
            SpawnRoomSprite(slot, y, x, persistent);
            goto next;
        fourthCheck:
            if (minY == (s32)y) {
                if (minX <= (s32)x && (s32)x <= maxX) {
                    SpawnRoomSprite(slot, y, x, persistent);
                }
            }
        }
    next:
        temp0 = 3;
        roomData += temp0;
        slot++;
    } while (slot <= 63);
}

void SpriteSpawnAsChild(u32 id, u32 roomSlot, u32 gfxSlot, u32 yPosition, u32 xPosition)
{
    u32 normalizedId;
    u32 normalizedRoom;
    u32 normalizedGfx;
    u32 y;
    u32 x;
    register u32 xWork PIN("r4");
    u32 index;
    struct PrimarySpriteData* sprites;
    u32 one;

    xWork = xPosition;
    normalizedId = (u8)id;
    normalizedRoom = (u8)roomSlot;
    normalizedGfx = (u8)gfxSlot;
    y = (u16)yPosition;
    x = (u16)xWork;
    index = 0;
    sprites = gSpriteData;
    one = 1;
    xWork = 0;
    do {
        struct PrimarySpriteData* sprite;
        u32 offset;
        u32 base;
        offset = sizeof(*sprite);
        offset *= index;
        base = (u32)sprites;
        sprite = (struct PrimarySpriteData*)(offset + base);
        if ((sprite->status & 1) == 0) {
            sprite->status = 23;
            sprite->gfxSlot = normalizedGfx;
            sprite->globalID = normalizedId;
            sprite->yPosition = y;
            sprite->xPosition = x;
            sprite->roomSlot = normalizedRoom;
            sprite->drawPriority = 2;
            sprite->pose = xWork;
            sprite->health = xWork;
            sprite->palette = xWork;
            sprite->disableWarioCollisionTimer = one;
            goto end;
        }
        {
            u32 next;
            next = index + 1;
            index = (u8)next;
        }
    } while (index <= 23);
end:
    return;
}

void SpawnPrimarySpriteWithStatus(u32 id, u32 roomSlot, u32 gfxSlot, u32 yPosition, u32 xPosition, u32 status)
{
    u32 normalizedId;
    u32 normalizedRoom;
    u32 normalizedGfx;
    u32 y;
    register u32 xWork PIN("r4");
    u32 x;
    register u32 normalizedStatus PIN("r5");
    u32 index;
    struct PrimarySpriteData* sprites;

    xWork = xPosition;
    normalizedStatus = status;
    normalizedId = (u8)id;
    normalizedRoom = (u8)roomSlot;
    normalizedGfx = (u8)gfxSlot;
    y = (u16)yPosition;
    x = (u16)xWork;
    normalizedStatus = (u16)normalizedStatus;
    index = 0;
    sprites = gSpriteData;
    {
        u32 activeBits;
        activeBits = 23;
        xWork = 0;
        normalizedStatus |= activeBits;
    }
    do {
        struct PrimarySpriteData* sprite;
        u32 offset;
        u32 base;
        offset = sizeof(*sprite);
        offset *= index;
        base = (u32)sprites;
        sprite = (struct PrimarySpriteData*)(offset + base);
        if ((sprite->status & 1) == 0) {
            sprite->status = normalizedStatus;
            sprite->gfxSlot = normalizedGfx;
            sprite->globalID = normalizedId;
            sprite->yPosition = y;
            sprite->xPosition = x;
            sprite->roomSlot = normalizedRoom;
            sprite->drawPriority = 2;
            sprite->pose = xWork;
            sprite->health = xWork;
            sprite->palette = xWork;
            {
                u32 one;
                one = 1;
                sprite->disableWarioCollisionTimer = one;
            }
            goto end;
        }
        {
            u32 next;
            next = index + 1;
            index = (u8)next;
        }
    } while (index <= 23);
end:
    return;
}

void SpawnHighPriorityPrimarySprite(u32 id, u32 roomSlot, u32 gfxSlot, u32 yPosition, u32 xPosition)
{
    u32 normalizedId;
    u32 normalizedRoom;
    u32 normalizedGfx;
    u32 y;
    u32 x;
    register u32 xWork PIN("r4");
    u32 index;
    struct PrimarySpriteData* sprites;
    u32 one;

    xWork = xPosition;
    normalizedId = (u8)id;
    normalizedRoom = (u8)roomSlot;
    normalizedGfx = (u8)gfxSlot;
    y = (u16)yPosition;
    x = (u16)xWork;
    index = 0;
    sprites = gSpriteData;
    one = 1;
    xWork = 0;
    do {
        struct PrimarySpriteData* sprite;
        u32 offset;
        u32 base;
        offset = sizeof(*sprite);
        offset *= index;
        base = (u32)sprites;
        sprite = (struct PrimarySpriteData*)(offset + base);
        if ((sprite->status & 1) == 0) {
            sprite->status = 23;
            sprite->gfxSlot = normalizedGfx;
            sprite->globalID = normalizedId;
            sprite->yPosition = y;
            sprite->xPosition = x;
            sprite->roomSlot = normalizedRoom;
            sprite->drawPriority = 130;
            sprite->pose = xWork;
            sprite->health = xWork;
            sprite->palette = xWork;
            sprite->disableWarioCollisionTimer = one;
            goto end;
        }
        {
            u32 next;
            next = index + 1;
            index = (u8)next;
        }
    } while (index <= 23);
end:
    return;
}

ProcedureFunc sPrimarySpriteAITable[PSPRITE_COUNT] = {
    [PSPRITE_BOX_JEWEL_PIECE_NE] = SpriteBoxJewelPieceNE,
    [PSPRITE_BOX_JEWEL_PIECE_SE] = SpriteBoxJewelPieceSE,
    [PSPRITE_BOX_JEWEL_PIECE_SW] = SpriteBoxJewelPieceSW,
    [PSPRITE_BOX_JEWEL_PIECE_NW] = SpriteBoxJewelPieceNW,
    [PSPRITE_BOX_CD] = SpriteBoxCD,
    [PSPRITE_BOX_HEART_REFILL] = SpriteBoxHeartRefill,
    [PSPRITE_DIAMOND] = SpriteDiamond,
    [PSPRITE_SWITCH] = SpriteSwitch,
    [PSPRITE_KEYZER] = SpriteKeyzer,
    [PSPRITE_SHOPKEEPER] = SpriteShopkeeper,
    [PSPRITE_0A] = SpriteShopEffectLong,
    [PSPRITE_0B] = SpriteShopEffectShortA,
    [PSPRITE_0C] = SpriteShopEffectShortB,
    [PSPRITE_LITTLE_HEART] = SpriteCoin,
    [PSPRITE_BOSS_TREASURE_CHEST] = SpriteBossTreasureChest,
    [PSPRITE_0F] = SpriteShopkeeperTransition,
    [PSPRITE_10] = SpriteUnknown10,
    [PSPRITE_SPEAR_MASK_YELLOW] = SpriteSpearMask,
    [PSPRITE_SPEAR_MASK_BLUE] = SpriteSpearMask,
    [PSPRITE_SPEAR_MASK_RED] = SpriteSpearMask,
    [PSPRITE_ROTATING_PLATFORM] = SpriteRotatingPlatform,
    [PSPRITE_ROCK] = SpriteRock,
    [PSPRITE_SKELETON_BIRD] = SpriteSkeletonBird,
    [PSPRITE_KAENTSUBO] = SpriteKaentsubo,
    [PSPRITE_CUCKOO_CONDOR] = SpriteCuckooCondor,
    [PSPRITE_TOTSUMEN] = SpriteTotsumen,
    [PSPRITE_PIG_HEAD_STATUE] = SpritePigHeadStatue,
    [PSPRITE_MOGURAMEN] = SpriteMoguramen,
    [PSPRITE_SPIKY] = SpriteSpiky,
    [PSPRITE_SPIKY_Z] = SpriteSpiky,
    [PSPRITE_BUBBLE] = SpriteBubble,
    [PSPRITE_TOGEROBO] = SpriteTogerobo,
    [PSPRITE_FALLING_SNOW] = SpriteFallingSnow,
    [PSPRITE_TOGEMASUKU] = SpriteTogemasuku,
    [PSPRITE_BOWLER_BALLLL_LIGHTNING] = SpriteBallLightning,
    [PSPRITE_ELECTRIC_LIFT_STATIONARY] = SpriteElectricLiftStationary,
    [PSPRITE_ELECTRIC_LIFT_VERTICAL] = SpriteElectricLiftVertical,
    [PSPRITE_ELECTRIC_LIFT_HORIZONTAL] = SpriteElectricLiftHorizontal,
    [PSPRITE_MAGIC_CARPET] = SpriteMagicCarpet,
    [PSPRITE_BOW_BALLOON] = SpriteBowBalloon,
    [PSPRITE_CHANCE_WHEEL] = SpriteChanceWheel,
    [PSPRITE_VORTEX] = SpriteVortex,
    [PSPRITE_MARUMEN_PURPLE] = SpriteMarumen,
    [PSPRITE_MARUMEN_RED] = SpriteMarumen,
    [PSPRITE_SPOILED_ROTTEN] = SpriteSpoiledRotten,
    [PSPRITE_DOMINOES_2D] = SpriteDominoActivator,
    [PSPRITE_DOMINOES_2E] = SpriteDominoLongPieceA,
    [PSPRITE_DOMINOES_2F] = SpriteDominoLongPieceB,
    [PSPRITE_DOMINOES_30] = SpriteDominoTile0,
    [PSPRITE_DOMINOES_31] = SpriteDominoTile1,
    [PSPRITE_DOMINOES_32] = SpriteDominoTile2,
    [PSPRITE_DOMINOES_33] = SpriteDominoTile3,
    [PSPRITE_DOMINOES_34] = SpriteDominoTile4,
    [PSPRITE_DOMINOES_35] = SpriteDominoTile5,
    [PSPRITE_DOMINOES_36] = SpriteDominoTile6,
    [PSPRITE_DOMINOES_37] = SpriteDominoTile7,
    [PSPRITE_DOMINOES_38] = SpriteDominoTile8,
    [PSPRITE_DOMINOES_39] = SpriteDominoTile9,
    [PSPRITE_DOMINO_GOAL] = SpriteDominoGoal,
    [PSPRITE_DICE_BLOCK] = SpriteDiceBlock,
    [PSPRITE_MAYUBIRD] = SpriteMayubird,
    [PSPRITE_MINICULA] = SpriteMinicula,
    [PSPRITE_TOGETOGE] = SpriteTogetoge,
    [PSPRITE_GHOST] = SpriteGhost,
    [PSPRITE_BEEZLEY] = SpriteBeezley,
    [PSPRITE_MONEY_FLOWER] = SpriteMoneyFlower,
    [PSPRITE_CATERPILLAR] = SpriteCaterpillar,
    [PSPRITE_TOY_BLOCK_TRIANGLE] = SpriteToyBlockTriangle,
    [PSPRITE_TOY_BLOCK_TRIANGLE_LOCK] = SpriteToyBlockTriangleLock,
    [PSPRITE_TOY_BLOCK_SQUARE] = SpriteToyBlockSquare,
    [PSPRITE_TOY_BLOCK_ROUND] = SpriteToyBlockRound,
    [PSPRITE_TOY_BLOCK_TRIANGLE_DOOR] = SpriteToyBlockTriangleDoor,
    [PSPRITE_MENHANMA] = SpriteMenhammer,
    [PSPRITE_MENONO] = SpriteMenono,
    [PSPRITE_GOGGLEY_BLADE] = SpriteGoggleyBlade,
    [PSPRITE_GOGGLEY_BLADE_2] = SpriteGoggleyBlade,
    [PSPRITE_TOBAWANI] = SpriteTobawani,
    [PSPRITE_SHIERAGUTCHI] = SpriteShieragutchi,
    [PSPRITE_RINGOSUKI] = SpriteRingosuki,
    [PSPRITE_PISTON] = SpritePiston,
    [PSPRITE_PISTON_2] = SpritePiston,
    [PSPRITE_CATBAT] = SpriteCatbat,
    [PSPRITE_VORTEX_2] = SpriteVortex,
    [PSPRITE_SWINGING_PLATFORM] = SpriteSwingingPlatform,
    [PSPRITE_SWINGING_PLATFORM_2] = SpriteSwingingPlatform2,
    [PSPRITE_ROTATING_PLATFORM_2] = SpriteRotatingPlatform2,
    [PSPRITE_ROTATING_PLATFORM_3] = SpriteRotatingPlatform3,
    [PSPRITE_PINBALL] = SpritePinball,
    [PSPRITE_PINBALL_LOCK_UP] = SpritePinballLockUp,
    [PSPRITE_PINBALL_LOCK_DOWN] = SpritePinballLockDown,
    [PSPRITE_PINBALL_LOCK_LEFT] = SpritePinballLockLeft,
    [PSPRITE_PINBALL_LOCK_RIGHT] = SpritePinballLockRight,
    [PSPRITE_PINBALL_COUNTER] = SpritePinballCounter,
    [PSPRITE_GOAL_BLOCK] = SpriteChanceWheel,
    [PSPRITE_SPIKE_CANNON] = SpriteSpikeCannon,
    [PSPRITE_MONEY_SUNFLOWER] = SpriteMoneySunflower,
    [PSPRITE_YETI] = SpriteYeti,
    [PSPRITE_BOWLER] = SpriteBowler,
    [PSPRITE_MAGMA] = SpriteMagma,
    [PSPRITE_BOSS_DOOR_ENTRY_PASSAGE] = SpriteBossDoor,
    [PSPRITE_BOSS_DOOR_EMERALD_PASSAGE] = SpriteBossDoor,
    [PSPRITE_BOSS_DOOR_RUBY_PASSAGE] = SpriteBossDoor,
    [PSPRITE_BOSS_DOOR_TOPAZ_PASSAGE] = SpriteBossDoor,
    [PSPRITE_BOSS_DOOR_SAPPHIRE_PASSAGE] = SpriteBossDoor,
    [PSPRITE_BOSS_DOOR_GOLDEN_PYRAMID] = SpriteBossDoor,
    [PSPRITE_AERODENT] = SpriteAerodent,
    [PSPRITE_HOGGUS] = SpriteHoggus,
    [PSPRITE_DENDEN] = SpriteDenden,
    [PSPRITE_BUTATABI] = SpriteButatabi,
    [PSPRITE_DEBURINA] = SpriteDeburina,
    [PSPRITE_FOLDING_DOOR] = SpriteFoldingDoor,
    [PSPRITE_PENCIL_PURPLE] = SpritePencil,
    [PSPRITE_PENCIL_BLUE] = SpritePencil,
    [PSPRITE_PENCIl_RED] = SpritePencil,
    [PSPRITE_ROBOBIRD] = SpriteRobobird,
    [PSPRITE_UTSUBOANKO] = SpriteUtsuboanko,
    [PSPRITE_TOGENOBI] = SpriteTogenobi,
    [PSPRITE_ICICLE] = SpriteIcicle,
    [PSPRITE_CACTUS] = SpriteCractusSpawner,
    [PSPRITE_DICE] = SpriteDice,
    [PSPRITE_TOY_CAR] = SpriteToyCar,
    [PSPRITE_ONOMI] = SpriteOnomi,
    [PSPRITE_BOSS_CORRIDOR_CAT] = SpriteBossCorridorCat,
    [PSPRITE_PETBOTTOM] = SpritePetbottom,
    [PSPRITE_GLASS_BIRD] = SpriteGlassBird,
    [PSPRITE_GOLDEN_DIVA] = SpriteGoldenDivaSpawner,
    [PSPRITE_CHANDELIER] = SpriteChandelier,
    [PSPRITE_CRUMBLING_BLOCK] = SpriteCrumblingBlock,
    [PSPRITE_PROFESSOR] = SpriteProfessor,
    [PSPRITE_COIN_10POINTS] = SpriteCoin,
    [PSPRITE_COIN_50POINTS] = SpriteCoin,
    [PSPRITE_COIN_100POINTS] = SpriteCoin,
    [PSPRITE_COIN_500POINTS] = SpriteCoin,
    [PSPRITE_COIN_1000POINTS] = SpriteCoin,
    [PSPRITE_JEWEL_PIECE_NE] = SpriteJewelPieceNE,
    [PSPRITE_JEWEL_PIECE_SE] = SpriteJewelPieceSE,
    [PSPRITE_JEWEL_PIECE_SW] = SpriteJewelPieceSW,
    [PSPRITE_JEWEL_PIECE_NW] = SpriteJewelPieceNW,
    [PSPRITE_CD] = SpriteCD,
    [PSPRITE_HEART_REFILL] = SpriteHeartRefill,
    [PSPRITE_GLOW_JEWEL_PIECE_NE] = SpriteTreasureGlowJewelPieceNE,
    [PSPRITE_GLOW_JEWEL_PIECE_SE] = SpriteTreasureGlowJewelPieceSE,
    [PSPRITE_GLOW_JEWEL_PIECE_SW] = SpriteTreasureGlowJewelPieceSW,
    [PSPRITE_GLOW_JEWEL_PIECE_NW] = SpriteTreasureGlowJewelPieceNW,
    [PSPRITE_GLOW_CD] = SpriteTreasureGlowCD,
    [PSPRITE_GLOW_HEART_REFILL] = SpriteTreasureGlowHeartRefill,
    [PSPRITE_92] = SpriteKaentsuboFlame,
    [PSPRITE_93] = SpriteKaentsuboFireball,
    [PSPRITE_94] = SpriteUnknown94,
    [PSPRITE_95] = SpriteUnknown95,
    [PSPRITE_96] = SpriteUnknown96,
    [PSPRITE_97] = SpriteUnknown97,
    [PSPRITE_SKELETON_BIRD_PROJECTILE] = SpriteSkeletonBirdProjectile,
    [PSPRITE_SKELETON_BIRD_DEBRIS] = SpriteSkeletonBirdDebris,
    [PSPRITE_9A] = SpriteMoguramenProjectile,
    [PSPRITE_9B] = SpriteBubbleInstance,
    [PSPRITE_9C] = SpriteUnknown9C,
    [PSPRITE_9D] = SpriteFallingSnowChild,
    [PSPRITE_9E] = SpriteFallingSnowFragment,
    [PSPRITE_BALL_LIGHTNING_BOLT] = SpriteBallLightningBolt,
    [PSPRITE_BALL_LIGHTNING_TRAIL] = SpriteBallLightningTrail,
    [PSPRITE_A1] = SpriteBowBalloonProjectile,
    [PSPRITE_A2] = SpriteChanceWheelRewardEffect,
    [PSPRITE_VORTEX_PART_LARGE] = SpriteVortexPartLarge,
    [PSPRITE_VORTEX_PART_LARGE_2] = SpriteVortexPartLarge,
    [PSPRITE_VORTEX_PART_MEDIUM] = SpriteVortexPartMedium,
    [PSPRITE_VORTEX_PART_MEDIUM_2] = SpriteVortexPartMedium,
    [PSPRITE_A7] = SpriteUnknownA7,
    [PSPRITE_A8] = SpriteUnknownA8,
    [PSPRITE_A9] = SpriteUnknownA9,
    [PSPRITE_AA] = SpriteSwitchEffect,
    [PSPRITE_AB] = SpriteGoldenDivaDebris,
    [PSPRITE_AC] = SpriteUnknownAC,
    [PSPRITE_AD] = SpriteUnknownAD,
    [PSPRITE_AE] = SpriteUnknownAE,
    [PSPRITE_AF] = SpriteUnknownAF,
    [PSPRITE_B0] = SpriteUnknownB0,
    [PSPRITE_B1] = SpriteUnknownB1,
    [PSPRITE_RINGOSUKI_APPLE] = SpriteRingosukiApple,
    [PSPRITE_WARIO_EXITING_VORTEX] = SpriteWarioExitingVortex,
    [PSPRITE_WARIO_ENTERING_VORTEX] = SpriteWarioEnteringVortex,
    [PSPRITE_B5] = SpritePinballCompletionSparkle,
    [PSPRITE_PIG_HEAD_PROJECTILE] = SpritePigHeadProjectile,
    [PSPRITE_CHANCE_WHEEL_DIAMOND] = SpriteChanceWheelDiamond,
    [PSPRITE_B8] = SpriteSpikeCannonProjectile,
    [PSPRITE_B9] = SpritePistonChild,
    [PSPRITE_BOWLER_BALL] = SpriteBowlerBall,
    [PSPRITE_BB] = SpriteYetiSnowball,
    [PSPRITE_ITEM_APPLE] = SpriteAppleBomb,
    [PSPRITE_CUCKOO_CONDOR_SMALL_DEBRIS] = SpriteCuckooCondorSmallDebris,
    [PSPRITE_ITEM_BLAST_CANNON] = SpriteBlastCannon,
    [PSPRITE_ITEM_VIZORMAN] = SpriteVizorman,
    [PSPRITE_ITEM_BUGLE] = SpriteBugle,
    [PSPRITE_C1] = SpriteBlackDog,
    [PSPRITE_C2] = SpriteLargeLips,
    [PSPRITE_C3] = SpriteBigFist,
    [PSPRITE_C4] = SpriteBlackDragon,
    [PSPRITE_C5] = SpriteBlastCannonball,
    [PSPRITE_C6] = SpriteVizormanBeamCore,
    [PSPRITE_C7] = SpriteVizormanBeamGlow,
    [PSPRITE_CUCKOO_CONDOR_LARGE_DEBRIS] = SpriteCuckooCondorLargeDebris,
    [PSPRITE_C9] = SpriteLargeLipsKissEffect,
    [PSPRITE_CA] = SpriteBlackDragonFire,
    [PSPRITE_AERODENT_COMPANION] = SpriteAerodentCompanion,
    [PSPRITE_AERODENT_WEAK_POINT] = SpriteAerodentWeakPoint,
    [PSPRITE_AERODENT_THROWABLE] = SpriteAerodentThrowable,
    [PSPRITE_AERODENT_EMITTER] = SpriteAerodentEmitter,
    [PSPRITE_BOSS_TREASURE] = SpriteBossTreasure,
    [PSPRITE_D0] = SpriteHoggusEgg,
    [PSPRITE_D1] = SpriteDendenShell,
    [PSPRITE_DEBURINA_CHILD] = SpriteDeburinaChild,
    [PSPRITE_KEYZER_ENTERING_VORTEX] = SpriteKeyzerEnteringVortex,
    [PSPRITE_TOGENOBI_OUTER_PART] = SpriteTogenobiOuterPart,
    [PSPRITE_TOGENOBI_INNER_PART] = SpriteTogenobiInnerPart,
    [PSPRITE_TOGENOBI_CENTER_PART] = SpriteTogenobiCenterPart,
    [PSPRITE_ICICLE_WARNING] = SpriteIcicleWarning,
    [PSPRITE_CRACTUS_BOSS] = SpriteCractusBoss,
    [PSPRITE_CRACTUS_STEM_SEGMENT] = SpriteCractusStemSegment,
    [PSPRITE_CRACTUS_STEM_TIP] = SpriteCractusStemTip,
    [PSPRITE_CRACTUS_ARM_SEGMENT_A] = SpriteCractusArmSegment,
    [PSPRITE_CRACTUS_ARM_SEGMENT_B] = SpriteCractusArmSegment,
    [PSPRITE_CRACTUS_DEFEAT_PIECE] = SpriteCractusDefeatPiece,
    [PSPRITE_CRACTUS_DEFEAT_ATTACHMENT] = SpriteCractusDefeatAttachment,
    [PSPRITE_CRACTUS_PROJECTILE] = SpriteCractusProjectile,
    [PSPRITE_CATBAT_GROUND_WAVE] = SpriteCatbatGroundWave,
    [PSPRITE_CATBAT_LANDING_DEBRIS] = SpriteCatbatLandingDebris,
    [PSPRITE_CATBAT_DEBRIS_PARTICLE] = SpriteCatbatDebrisParticle,
    [PSPRITE_CATBAT_PROJECTILE] = SpriteCatbatProjectile,
    [PSPRITE_CATBAT_ATTACK_EFFECT] = SpriteCatbatAttackEffect,
    [PSPRITE_CUCKOO_CONDOR_HEAD] = SpriteCuckooCondorHead,
    [PSPRITE_CUCKOO_CONDOR_CLOCK_BODY] = SpriteCuckooCondorClockBody,
    [PSPRITE_CUCKOO_CONDOR_CLOCK_PIECE] = SpriteCuckooCondorClockPiece,
    [PSPRITE_CUCKOO_CONDOR_PENDULUM_PART] = SpriteCuckooCondorPendulumPart,
    [PSPRITE_CUCKOO_CONDOR_EGG] = SpriteCuckooCondorEgg,
    [PSPRITE_CUCKOO_CONDOR_CHICK] = SpriteCuckooCondorChick,
    [PSPRITE_EB] = SpriteGlassBirdFragment,
    [PSPRITE_EC] = SpriteGoldenDivaBody,
    [PSPRITE_ED] = SpriteGoldenDivaRobe,
    [PSPRITE_EE] = SpriteGoldenDivaLowerBody,
    [PSPRITE_EF] = SpriteGoldenDivaFaceHitbox,
    [PSPRITE_F0] = SpriteGoldenDivaMask,
    [PSPRITE_F1] = SpriteGoldenDivaFan,
    [PSPRITE_F2] = SpriteGoldenDivaExplosiveBall,
    [PSPRITE_F3] = SpriteGoldenDivaBlackEgg,
    [PSPRITE_F4] = SpriteGoldenDivaGreenEnemy,
    [PSPRITE_F5] = SpriteGoldenDivaBlackChick,
    [PSPRITE_F6] = SpriteGoldenDivaHammer,
    [PSPRITE_F7] = SpriteGoldenDivaFace,
    [PSPRITE_F8] = SpriteGoldenDivaAttackEffect,
    [PSPRITE_CRACTUS_IMPACT_EFFECT] = SpriteCractusImpactEffect,
    [PSPRITE_PROFESSOR_PROJECTILE] = SpriteProfessorProjectile,
    [PSPRITE_CATBAT_MINE_SPAWNER] = SpriteCatbatMineSpawner,
};

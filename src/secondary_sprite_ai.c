#include "secondary_sprite_ai.h"

#include "autosave.h"
#include "color_effects.h"
#include "sprite_collision.h"

#include "background_registers.h"
#include "fixed_point.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "hud.h"
#include "main.h"
#include "oam.h"
#include "score.h"
#include "sprite.h"
#include "sprite_ai/coins.h"
#include "sprite_ai/keyzer.h"
#include "sprite_ai/shopkeeper.h"
#include "voice_set.h"
#include "wario.h"

extern const u16 sTimeUpTextPal[];
extern const u16 sTimeUpWarioPal[];
extern const u16 sUnk_840F1C8[];
extern const u16 sUnk_840F200[];
extern const u16 sUnk_840F214[];
extern const u8 sUnk_84168B4[];
extern const u16 sUnk_8411AFC[];
extern const u16 sUnk_8411B48[];
extern const u16 sUnk_8411B94[];
extern const u16 sUnk_8411BE0[];
extern const u16 sUnk_8411C2C[];
extern const u16 sUnk_8411C78[];
extern const u16 sUnk_8411CC4[];
extern const u16 sUnk_8411D10[];
extern const u16 sUnk_8411D5C[];
extern const u16 sUnk_8411DA8[];
extern const u16 sUnk_8411DF4[];
extern const u16 sUnk_8411E40[];
extern const u16 sUnk_8411E8C[];
extern const u16 sUnk_8411ED8[];
extern const u16 sUnk_8411F24[];
extern const u16 sUnk_8411F70[];
extern const u16 sUnk_8411FBC[];
extern const struct AnimationFrame sUnk_840F248[];
extern const struct AnimationFrame sUnk_840F258[];
extern const struct AnimationFrame sUnk_840F268[];
extern const struct AnimationFrame sUnk_840F278[];
extern const struct AnimationFrame sUnk_840F288[];
extern const struct AnimationFrame sUnk_840FAD4[];
extern const struct AnimationFrame sUnk_840FB94[];
extern const struct AnimationFrame sUnk_840FC7C[];
extern const struct AnimationFrame sUnk_840FE74[];
extern const struct AnimationFrame sUnk_840FEA4[];
extern const struct AnimationFrame sUnk_841000C[];
extern const struct AnimationFrame sUnk_8414614[];
extern const struct AnimationFrame sUnk_8414824[];
extern const struct AnimationFrame sUnk_83D86C4[];
extern const struct AnimationFrame sGoldenDivaBlackChickFinalOam[];
extern const struct AnimationFrame sProfessorProjectileOam[];

extern const struct AnimationFrame sUnk_8410544[];
extern const struct AnimationFrame sUnk_84105A4[];
extern const struct AnimationFrame sUnk_8410AB4[];
extern const struct AnimationFrame sUnk_8410B14[];
extern const struct AnimationFrame sUnk_84110B8[];
extern const struct AnimationFrame sUnk_8411128[];
extern const struct AnimationFrame sUnk_8410D90[];
extern const struct AnimationFrame sUnk_8410E00[];
extern const struct AnimationFrame sUnk_8410484[];
extern const struct AnimationFrame sUnk_84104E4[];
extern const struct AnimationFrame sUnk_84109F4[];
extern const struct AnimationFrame sUnk_8410A54[];
extern const struct AnimationFrame sUnk_8412538[];
extern const struct AnimationFrame sUnk_8412598[];
extern const struct AnimationFrame sUnk_8412AA8[];
extern const struct AnimationFrame sUnk_8412B08[];
extern const struct AnimationFrame sUnk_841317C[];
extern const struct AnimationFrame sUnk_84131EC[];
extern const struct AnimationFrame sUnk_8412D84[];
extern const struct AnimationFrame sUnk_8412DF4[];
extern const struct AnimationFrame sUnk_8412478[];
extern const struct AnimationFrame sUnk_84124D8[];
extern const struct AnimationFrame sUnk_84129E8[];
extern const struct AnimationFrame sUnk_8412A48[];
extern const struct AnimationFrame sUnk_84137E4[];
extern const struct AnimationFrame sUnk_8413844[];
extern const struct AnimationFrame sUnk_8413D54[];
extern const struct AnimationFrame sUnk_8413DB4[];
extern const struct AnimationFrame sUnk_8414428[];
extern const struct AnimationFrame sUnk_8414498[];
extern const struct AnimationFrame sUnk_8414030[];
extern const struct AnimationFrame sUnk_84140A0[];
extern const struct AnimationFrame sUnk_8413724[];
extern const struct AnimationFrame sUnk_8413784[];
extern const struct AnimationFrame sUnk_8413C94[];
extern const struct AnimationFrame sUnk_8413CF4[];
extern const struct AnimationFrame sUnk_841162C[];
extern const struct AnimationFrame sUnk_84116DC[];
extern const struct AnimationFrame sUnk_841497C[];
extern const struct AnimationFrame sUnk_8414A0C[];
extern const struct AnimationFrame sUnk_8414A1C[];
extern const struct AnimationFrame sUnk_8414A2C[];
extern const struct AnimationFrame sUnk_8414A3C[];

extern const u16 sUnk_8411A4CFrame0[];
extern const u16 sUnk_8411FE2[];
extern const u16 sUnk_8411FEA[];
extern const u16 sUnk_8411FF8[];
extern const u16 sUnk_8412006[];
extern const u16 sUnk_8412014[];
extern const u16 sUnk_8412022[];
extern const u16 sUnk_8412030[];
extern const u16 sUnk_841203E[];
extern const u16 sUnk_841204C[];
extern const u16 sUnk_841205A[];
extern const s16 sUnk_8416A46[];
extern const u16 sUnk_8411994[];
extern const struct AnimationFrame sUnk_8411A4C[];
extern const u16 sUnk_8414A4C[];
extern const u16 sUnk_8414A54[];
extern const u8 sUnk_8416A06[];
extern const u8 sJewelPieceIconPartialNEGfx[];
extern const u8 sJewelPieceIconPartialSEGfx[];
extern const u8 sJewelPieceIconPartialSWGfx[];
extern const u8 sJewelPieceIconPartialNWGfx[];
extern const u8 sCDIconPartialGfx[];
extern const s16 sUnk_84165F4[];
extern const s16 sUnk_8416674[];
extern const s16 sUnk_84166F4[];
extern const s16 sUnk_8416774[];
extern const s16 sUnk_84167B4[];
extern const s16 sUnk_8416834[];
extern const s16 sUnk_8416A88[];
extern const struct AnimationFrame sUnk_8412080[];
extern const u16 sWarioCenteredScaleEffectOam[];
extern const u16 sLittleHeartOam_Frame1[];
extern const s16 sUnk_8416AA4[];
extern const u16 sBugleNote1OamFrame[];
extern const u16 sBugleNote2OamFrame[];
extern const u16 sBugleNote3OamFrame[];
extern const u16 sBugleNote4OamFrame[];
extern const u16 sBugleNote5OamFrame[];
extern const u16 sBugleNote6OamFrame[];
extern const u16 sBugleNote7OamFrame[];
extern const u16 sBugleNote8OamFrame[];
extern const u16 sBugleNote9OamFrame[];
extern const u16 sBugleNote10OamFrame[];

void ApplySecondarySpriteVerticalMotionTable(void)
{
    u32 valueOrEntry;
    register u32 table PIN("r1");
    struct SecondarySprite* sprite;
    u32 timer;
    u16 movement;
    s16 signedMovement;

    sprite = &gCurrentSecondarySprite;
    timer = sprite->timer;
    table = (u32)sUnk_84165F4;
    valueOrEntry = timer << 1;
    valueOrEntry += table;
    movement = *(const u16*)valueOrEntry;
    signedMovement = *(const s16*)valueOrEntry;

    if (signedMovement == 0x7FFF) {
        sprite->status = 0;
    } else {
        valueOrEntry = timer + 1;
        sprite->timer = valueOrEntry;
        valueOrEntry = sprite->yPosition;
        valueOrEntry += movement;
        sprite->yPosition = valueOrEntry;
    }
}
void ApplySecondarySpriteScaleMotionTableA(void)
{
    s32 tableOrValue;
    register s16* destination PIN("r5");
    register s32 cosine PIN("r6");
    struct SecondarySprite* sprite;
    u32 timer;
    s32 scale;

    sprite = &gCurrentSecondarySprite;
    timer = sprite->timer;
    tableOrValue = (u16)sUnk_84165F4[timer];
    scale = sUnk_84167B4[timer];

    if (scale == 0x7FFF) {
        sprite->status = 0;
    } else {
        sprite->status |= 2;
        sprite->timer = timer + 1;
        sprite->yPosition += tableOrValue;

        destination = &gSecondarySpriteAffinePA;
        tableOrValue = (s32)sSinCosTable;
        cosine = *(const s16*)(tableOrValue + 0x80);
        *destination = FixedMul(cosine, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePB;
        tableOrValue = *(const s16*)tableOrValue;
        *destination = FixedMul(tableOrValue, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePC;
        tableOrValue = -tableOrValue;
        *destination = FixedMul(tableOrValue, FixedInverse(scale));

        tableOrValue = (s32)&gSecondarySpriteAffinePD;
        *(s16*)tableOrValue = FixedMul(cosine, FixedInverse(scale));
    }
}
void ApplySecondarySpriteScaleMotionTableB(void)
{
    s32 tableOrValue;
    register s16* destination PIN("r5");
    register s32 cosine PIN("r6");
    struct SecondarySprite* sprite;
    u32 timer;
    s32 scale;

    sprite = &gCurrentSecondarySprite;
    timer = sprite->timer;
    tableOrValue = (u16)sUnk_84165F4[timer];
    scale = sUnk_8416834[timer];

    if (scale == 0x7FFF) {
        sprite->status = 0;
    } else {
        sprite->status |= 2;
        sprite->timer = timer + 1;
        sprite->yPosition += tableOrValue;

        destination = &gSecondarySpriteAffinePA;
        tableOrValue = (s32)sSinCosTable;
        cosine = *(const s16*)(tableOrValue + 0x80);
        *destination = FixedMul(cosine, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePB;
        tableOrValue = *(const s16*)tableOrValue;
        *destination = FixedMul(tableOrValue, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePC;
        tableOrValue = -tableOrValue;
        *destination = FixedMul(tableOrValue, FixedInverse(scale));

        tableOrValue = (s32)&gSecondarySpriteAffinePD;
        *(s16*)tableOrValue = FixedMul(cosine, FixedInverse(scale));
    }
}
void UpdateSecondarySpriteRisingScaleTrail(void)
{
    register u32 scale PIN("r5");
    s32 xOrDestination;
    register s32 cosine PIN("r8");
    u32 timer;

    {
        register struct SecondarySprite* sprite PIN("r4");
        const s16* xTable;
        u32 offsetOrScaleAddress;
        register s32 scaleTableOrSignedX PIN("r1");

        sprite = &gCurrentSecondarySprite;
        sprite->status |= 2;
        AdvanceSecondarySpriteAnimation(sUnk_8412080);

        timer = sprite->timer;
        xTable = sUnk_8416674;
        offsetOrScaleAddress = timer << 1;
        xTable = (const s16*)(offsetOrScaleAddress + (u32)xTable);
        scaleTableOrSignedX = (s32)sUnk_84166F4;
        offsetOrScaleAddress += scaleTableOrSignedX;
        scale = *(const u16*)offsetOrScaleAddress;
        xOrDestination = *(const u16*)xTable;
        offsetOrScaleAddress = 0;
        scaleTableOrSignedX = *(const s16*)((u32)xTable + offsetOrScaleAddress);

        if (scaleTableOrSignedX == 0x7FFF) {
            sprite->status = 0;
        } else {
            sprite->timer = timer + 1;
            sprite->yPosition -= 6;
            sprite->xPosition += xOrDestination;

            {
                s32 tableOrSine;
                u32 address;
                s32 trigValue;
                u32 zero;

                xOrDestination = (s32)&gSecondarySpriteAffinePA;
                tableOrSine = (s32)sSinCosTable;
                address = tableOrSine;
                address += 0x80;
                zero = 0;
                trigValue = *(const s16*)(address + zero);
                cosine = trigValue;

                scale = (s16)scale;
                *(s16*)xOrDestination = FixedMul(cosine, FixedInverse(scale));

                xOrDestination = (s32)&gSecondarySpriteAffinePB;
                address = 0;
                tableOrSine = *(const s16*)(tableOrSine + address);
                *(s16*)xOrDestination = FixedMul(tableOrSine, FixedInverse(scale));

                xOrDestination = (s32)&gSecondarySpriteAffinePC;
                tableOrSine = -tableOrSine;
                *(s16*)xOrDestination = FixedMul(tableOrSine, FixedInverse(scale));

                tableOrSine = (s32)&gSecondarySpriteAffinePD;
                *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(scale));
            }
        }
    }
}
void UpdateWarioCenteredScaleEffectSecondarySprite(void)
{
    s32 tableOrSine;
    register s16* destination PIN("r5");
    register s32 cosine PIN("r6");
    struct SecondarySprite* sprite;
    u32 timer;
    s32 scale;

    sprite = &gCurrentSecondarySprite;
    sprite->status |= 2;
    GP_SET(gCurrentSecondarySpriteOamData, sWarioCenteredScaleEffectOam);
    timer = sprite->timer;
    scale = sUnk_8416774[timer];

    if (scale == 0x7FFF) {
        sprite->status = 0;
    } else {
        sprite->timer = timer + 1;
        sprite->yPosition = gWarioData.yPosition - 72;
        sprite->xPosition = gWarioData.xPosition - 32;

        destination = &gSecondarySpriteAffinePA;
        tableOrSine = (s32)sSinCosTable;
        cosine = *(const s16*)(tableOrSine + 0x80);
        *destination = FixedMul(cosine, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePB;
        tableOrSine = *(const s16*)tableOrSine;
        *destination = FixedMul(tableOrSine, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePC;
        tableOrSine = -tableOrSine;
        *destination = FixedMul(tableOrSine, FixedInverse(scale));

        tableOrSine = (s32)&gSecondarySpriteAffinePD;
        *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(scale));
    }
}
void UpdateLittleHeartPopSecondarySprite(void)
{
    s32 tableOrSine;
    register s16* destination PIN("r5");
    register s32 cosine PIN("r6");
    struct SecondarySprite* sprite;
    u32 timer;
    s32 scale;

    sprite = &gCurrentSecondarySprite;
    sprite->status |= 2;
    GP_SET(gCurrentSecondarySpriteOamData, sLittleHeartOam_Frame1);
    timer = sprite->timer;
    scale = sUnk_8416774[timer];

    if (scale == 0x7FFF) {
        sprite->status = 0;
    } else {
        sprite->timer = timer + 1;
        sprite->yPosition = gWarioData.yPosition - 40;
        sprite->xPosition = gWarioData.xPosition - 16;

        destination = &gSecondarySpriteAffinePA;
        tableOrSine = (s32)sSinCosTable;
        cosine = *(const s16*)(tableOrSine + 0x80);
        *destination = FixedMul(cosine, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePB;
        tableOrSine = *(const s16*)tableOrSine;
        *destination = FixedMul(tableOrSine, FixedInverse(scale));

        destination = &gSecondarySpriteAffinePC;
        tableOrSine = -tableOrSine;
        *destination = FixedMul(tableOrSine, FixedInverse(scale));

        tableOrSine = (s32)&gSecondarySpriteAffinePD;
        *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(scale));
    }
}
void UpdateVerticalMotionEffectSecondarySprite(void)
{
    AdvanceSecondarySpriteAnimation(sUnk_840F248);
    ApplySecondarySpriteVerticalMotionTable();
}

void UpdateScaleMotionEffectBSecondarySprite(void)
{
    AdvanceSecondarySpriteAnimation(sUnk_840F258);
    ApplySecondarySpriteScaleMotionTableB();
}

void UpdateScaleMotionEffectASecondarySprite(void)
{
    AdvanceSecondarySpriteAnimation(sUnk_840F268);
    ApplySecondarySpriteScaleMotionTableA();
}

void UpdateScaleMotionEffectCSecondarySprite(void)
{
    AdvanceSecondarySpriteAnimation(sUnk_840F278);
    ApplySecondarySpriteScaleMotionTableA();
}

void UpdateScaleMotionEffectDSecondarySprite(void)
{
    AdvanceSecondarySpriteAnimation(sUnk_840F288);
    ApplySecondarySpriteScaleMotionTableA();
}

void UpdateTimedSecondarySpriteAnimationA(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_840FAD4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateSoundedTimedSecondarySpriteAnimation(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_840FB94);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }

    if (gCurrentSecondarySprite.pose == 0) {
        m4aSongNumStart(58);
        gCurrentSecondarySprite.pose++;
    }
}

void UpdateSilentTimedSecondarySpriteAnimation(void)
{
    AdvanceSecondarySpriteAnimation(sUnk_840FB94);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.pose++;
    }
}

void UpdateGoldenDivaBlackChickIntroSecondarySprite(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8414614);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateGoldenDivaBlackChickSecondarySprite(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sGoldenDivaBlackChickFinalOam);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateGoldenDivaFinalEffectSecondarySprite(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8414824);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateShortShopEffectSecondarySprite(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sShopEffectShortAOam);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateLongShopEffectSecondarySprite(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sShopEffectLongOam);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimationB(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_840FC7C);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimationC(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_840FE74);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimationD(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_840FEA4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateProfessorProjectileSecondarySprite(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sProfessorProjectileOam);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimationE(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_841000C);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimationF(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_83D86C4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdatePurchasedShopItemSecondarySprite(void)
{
    gCurrentSecondarySprite.timer++;

    switch (gCurrentShopItem) {
        case 1:
            AdvanceSecondarySpriteAnimation(sPurchasedAppleBombOam);
            break;

        case 2:
            AdvanceSecondarySpriteAnimation(sPurchasedBlastCannonOam);
            break;

        case 5:
            AdvanceSecondarySpriteAnimation(sPurchasedBlackDogOam);
            break;

        case 7:
            AdvanceSecondarySpriteAnimation(sPurchasedBigFistOam);
            break;

        case 8:
            AdvanceSecondarySpriteAnimation(sPurchasedBlackDragonOam);
            break;

        default:
            gCurrentSecondarySprite.status = 0;
            break;
    }

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

u32 UpdateBugleNoteDriftMotion(void)
{
    u32 valueOrDirection;
    struct SecondarySprite* sprite;
    u32 value;
    u32 timer;
    register u16 xVelocity PIN("r4");

    valueOrDirection = (u32)sUnk_8416AA4;
    sprite = &gCurrentSecondarySprite;
    value = sprite->unk5;
    value <<= 24;
    timer = value >> 24;
    value >>= 27;
    value <<= 1;
    value += valueOrDirection;
    xVelocity = *(const u16*)value;
    valueOrDirection = sprite->work0;

    if (valueOrDirection != 0) {
        value = sprite->xPosition;
        value += xVelocity;
    } else {
        value = sprite->xPosition;
        value -= xVelocity;
    }

    sprite->xPosition = value;

    if (timer > 94) {
        value = 0;
        sprite->unk5 = value;
        value = 1;
        value ^= valueOrDirection;
        sprite->work0 = value;
    }

    value = sprite->unk5;
    value++;
    sprite->unk5 = value;
    value = sprite->xPosition;
    value += 2;
    sprite->xPosition = value;
    value = sprite->yPosition;
    value -= 2;
    sprite->yPosition = value;
    RETURN_NO_VALUE(0);
}
u32 ApplyBugleNoteRotatingAffine(u8 angle)
{
    register struct SecondarySprite* sprite PIN("r8");
    u32 scale;
    register s16* destination PIN("r6");
    const s16* table;
    register s32 trigValue PIN("r4");
    struct SecondarySprite* current;
    register u32 status PIN("r2");
    u32 value;

    if (angle <= 59) {
        current = &gCurrentSecondarySprite;
        status = current->status;
        value = 2;
        value |= status;
        current->status = value;
        current->animationTimer += 4;
        scale = current->animationTimer;
        sprite = current;
    } else if (angle > 195) {
        current = &gCurrentSecondarySprite;
        current->animationTimer -= 4;
        scale = current->animationTimer;
        sprite = current;
    } else {
        scale = 0x100;
        sprite = &gCurrentSecondarySprite;
    }

    destination = &gSecondarySpriteAffinePA;
    table = sSinCosTable;
    current = sprite;
    value = current->timer;
    value += 0x40;
    value <<= 1;
    value += (u32)table;
    trigValue = *(const s16*)value;
    *destination = FixedMul(trigValue, FixedInverse(scale));

    destination = &gSecondarySpriteAffinePB;
    current = sprite;
    trigValue = table[current->timer];
    *destination = FixedMul(trigValue, FixedInverse(scale));

    destination = &gSecondarySpriteAffinePC;
    current = sprite;
    trigValue = table[current->timer];
    trigValue = -trigValue;
    *destination = FixedMul(trigValue, FixedInverse(scale));

    destination = &gSecondarySpriteAffinePD;
    current = sprite;
    value = current->timer;
    value += 0x40;
    value <<= 1;
    value += (u32)table;
    trigValue = *(const s16*)value;
    *destination = FixedMul(trigValue, FixedInverse(scale));
    RETURN_NO_VALUE(0);
}
u32 ApplyBugleNotePulseAffine(u8 angle)
{
    u32 scale;
    register s16* destination PIN("r5");
    register s32 tableOrSine PIN("r4");
    s32 cosine;

    if (angle <= 59) {
        gCurrentSecondarySprite.status |= 2;
        gCurrentSecondarySprite.animationTimer += 4;
        scale = gCurrentSecondarySprite.animationTimer;
    } else if (angle > 195) {
        gCurrentSecondarySprite.animationTimer -= 4;
        scale = gCurrentSecondarySprite.animationTimer;
    } else {
        scale = 0x100;

        if (angle > 90) {
            scale = 0x200;

            if (angle > 120) {
                scale = 0x100;

                if (angle > 150) {
                    if (angle <= 180) {
                        scale = 0x200;
                    }
                }
            }
        }
    }

    destination = &gSecondarySpriteAffinePA;
    tableOrSine = (s32)sSinCosTable;
    cosine = *(const s16*)(tableOrSine + 0x80);
    *destination = FixedMul(cosine, FixedInverse(scale));

    destination = &gSecondarySpriteAffinePB;
    tableOrSine = *(const s16*)tableOrSine;
    *destination = FixedMul(tableOrSine, FixedInverse(scale));

    destination = &gSecondarySpriteAffinePC;
    tableOrSine = -tableOrSine;
    *destination = FixedMul(tableOrSine, FixedInverse(scale));

    tableOrSine = (s32)&gSecondarySpriteAffinePD;
    *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(scale));
    RETURN_NO_VALUE(0);
}
void UpdateBugleNote6SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 1;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNoteRotatingAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote6OamFrame);
    }
}

void UpdateBugleNote1SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 0;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNotePulseAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote1OamFrame);
    }
}

void UpdateBugleNote10SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 1;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNoteRotatingAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote10OamFrame);
    }
}

void UpdateBugleNote2SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 0;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNotePulseAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote2OamFrame);
    }
}

void UpdateBugleNote9SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 1;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNoteRotatingAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote9OamFrame);
    }
}

void UpdateBugleNote3SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 0;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNotePulseAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote3OamFrame);
    }
}

void UpdateBugleNote8SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 1;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNoteRotatingAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote8OamFrame);
    }
}

void UpdateBugleNote4SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 0;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNotePulseAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote4OamFrame);
    }
}

void UpdateBugleNote7SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 1;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNoteRotatingAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote7OamFrame);
    }
}

void UpdateBugleNote5SecondarySprite(void)
{
    u32 timer;

    if (gCurrentSecondarySprite.pose == 0) {
        gCurrentSecondarySprite.animationTimer = 4;
        gCurrentSecondarySprite.work0 = 0;
        gCurrentSecondarySprite.pose++;
    }

    gCurrentSecondarySprite.timer++;
    timer = gCurrentSecondarySprite.timer;

    if (timer == U8_MAX) {
        gCurrentSecondarySprite.status = 0;
    } else {
        ApplyBugleNotePulseAffine(timer);
        UpdateBugleNoteDriftMotion();
        GP_SET(gCurrentSecondarySpriteOamData, sBugleNote5OamFrame);
    }
}

void UpdateTimedSecondarySpriteAnimation00(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8410544);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation01(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84105A4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation02(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8410AB4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation03(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8410B14);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation04(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84110B8);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation05(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8411128);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation06(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8410D90);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation07(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8410E00);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation08(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8410484);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation09(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84104E4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation10(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84109F4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation11(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8410A54);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation12(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412538);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation13(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412598);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation14(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412AA8);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation15(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412B08);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation16(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_841317C);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation17(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84131EC);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation18(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412D84);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation19(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412DF4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation20(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412478);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation21(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84124D8);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation22(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84129E8);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation23(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8412A48);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation24(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84137E4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation25(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8413844);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation26(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8413D54);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation27(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8413DB4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation28(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8414428);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation29(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8414498);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation30(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8414030);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation31(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84140A0);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation32(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8413724);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation33(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8413784);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation34(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8413C94);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation35(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_8413CF4);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation36(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_841162C);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void UpdateTimedSecondarySpriteAnimation37(void)
{
    gCurrentSecondarySprite.timer++;
    AdvanceSecondarySpriteAnimation(sUnk_84116DC);

    if (gCurrentSecondarySprite.animationTimer == 0 && gCurrentSecondarySprite.work0 == 0) {
        gCurrentSecondarySprite.status = 0;
    }
}

void ClearCurrentSecondarySprite(void)
{
    gCurrentSecondarySprite.status = 0;
}

void UpdateKeyzerSecondarySprite(void)
{
    u32 specialWarioPose;
    u32 pose;
    const struct AnimationFrame* animation;
    u16 currentPosition;
    u16 targetPosition;
    u16 step;
    u16 secondaryValue;
    u16 difference;

    specialWarioPose = 0;
    if (gWarioData.reaction == 0) {
        pose = gWarioData.pose;
        if ((u8)(pose - 40) <= 6) {
            specialWarioPose = 1;
        } else if ((u8)(pose - 17) <= 1) {
            specialWarioPose = 1;
        }
    }

    if (gTimerState > 4) {
        ConvertKeyzerToTimeUpEffect();
        return;
    }

    switch (gCurrentSecondarySprite.pose) {
        case 0:
            AdvanceSecondarySpriteAnimation(sKeyzerOam_83B49BC);
            gCurrentSecondarySprite.timer++;
            if (gCurrentSecondarySprite.timer > 46) {
                gCurrentSecondarySprite.pose = 1;
            }
            break;

        case 1:
            gCurrentSecondarySprite.pose = 2;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 2:
            AdvanceSecondarySpriteAnimation(sKeyzerOam_83B47C4);
            if (specialWarioPose != 0) {
                gCurrentSecondarySprite.pose = 9;
            } else if (gWarioData.horizontalDirection & 0x10) {
                gCurrentSecondarySprite.pose = 5;
            }
            break;

        case 3:
            gCurrentSecondarySprite.pose = 4;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 4:
            AdvanceSecondarySpriteAnimation(sKeyzerOam_83B47EC);
            if (specialWarioPose != 0) {
                gCurrentSecondarySprite.pose = 11;
            } else if (gWarioData.horizontalDirection & 0x20) {
                gCurrentSecondarySprite.pose = 7;
            }
            break;

        case 5:
            gCurrentSecondarySprite.pose = 6;
            gCurrentSecondarySprite.timer = 24;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 6:
            animation = sKeyzerOam_83B48DC;
            goto animate_to_pose3;

        case 7:
            gCurrentSecondarySprite.pose = 8;
            gCurrentSecondarySprite.timer = 24;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 8:
            animation = sKeyzerOam_83B4904;
            goto animate_to_pose1;

        case 9:
            gCurrentSecondarySprite.pose = 10;
            gCurrentSecondarySprite.timer = 9;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 10:
            animation = sKeyzerOam_83B4814;
            goto animate_to_pose13;

        case 11:
            gCurrentSecondarySprite.pose = 12;
            gCurrentSecondarySprite.timer = 9;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 12:
            animation = sKeyzerOam_83B483C;
        animate_to_pose13:
            AdvanceSecondarySpriteAnimation(animation);
            gCurrentSecondarySprite.timer--;
            if (gCurrentSecondarySprite.timer == 0) {
                gCurrentSecondarySprite.pose = 13;
            }
            break;

        case 13:
            gCurrentSecondarySprite.pose = 14;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 14:
            AdvanceSecondarySpriteAnimation(sKeyzerOam_83B4864);
            if (specialWarioPose == 0) {
                if (gWarioData.horizontalDirection & 0x20) {
                    gCurrentSecondarySprite.pose = 15;
                } else {
                    gCurrentSecondarySprite.pose = 17;
                }
            }
            break;

        case 15:
            gCurrentSecondarySprite.pose = 16;
            gCurrentSecondarySprite.timer = 15;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 16:
            animation = sKeyzerOam_83B488C;
        animate_to_pose1:
            AdvanceSecondarySpriteAnimation(animation);
            gCurrentSecondarySprite.timer--;
            if (gCurrentSecondarySprite.timer == 0) {
                gCurrentSecondarySprite.pose = 1;
            }
            break;

        case 17:
            gCurrentSecondarySprite.pose = 18;
            gCurrentSecondarySprite.timer = 15;
            gCurrentSecondarySprite.animationTimer = 0;
            gCurrentSecondarySprite.work0 = 0;
            /* fall through */
        case 18:
            animation = sKeyzerOam_83B48B4;
        animate_to_pose3:
            AdvanceSecondarySpriteAnimation(animation);
            gCurrentSecondarySprite.timer--;
            if (gCurrentSecondarySprite.timer == 0) {
                gCurrentSecondarySprite.pose = 3;
            }
            break;

        default:
            AdvanceSecondarySpriteAnimation(sKeyzerOam_83B4864);
            break;
    }

    if (gUnk_3000C0E != 0) {
        u16* vortexXPosition;

        currentPosition = (u16)(gCurrentSecondarySprite.xPosition + 0x200);
        targetPosition = (u16)(gVortexXPosition + 0x200);
        vortexXPosition = &gVortexXPosition;
        if (currentPosition > targetPosition) {
            difference = currentPosition - targetPosition;
        } else {
            difference = targetPosition - currentPosition;
        }

        if (difference <= 8) {
            step = 0;
        } else {
            step = (difference - 8) >> 3;
        }

        if (currentPosition > targetPosition) {
            gCurrentSecondarySprite.xPosition -= step;
        } else if (currentPosition < targetPosition) {
            gCurrentSecondarySprite.xPosition += step;
        }

        currentPosition = (u16)(gCurrentSecondarySprite.yPosition + 0x200);
        targetPosition = (u16)(gVortexYPosition + 0x200);
        if (currentPosition > targetPosition) {
            difference = currentPosition - targetPosition;
        } else {
            difference = targetPosition - currentPosition;
        }

        if (difference <= 8) {
            secondaryValue = 0;
        } else {
            secondaryValue = (difference - 8) >> 3;
        }

        if (currentPosition > targetPosition) {
            gCurrentSecondarySprite.yPosition -= secondaryValue;
        } else if (currentPosition < targetPosition) {
            gCurrentSecondarySprite.yPosition += secondaryValue;
        }

        if (step == 0 && secondaryValue == 0) {
            SpawnHighPriorityPrimarySprite(211, 0, gVortexGfxSlot, gVortexYPosition, *vortexXPosition);
            gCurrentSecondarySprite.status = step;
        }
    } else {
        struct WarioData* wario;

        currentPosition = (u16)(gCurrentSecondarySprite.xPosition + 0x200);
        targetPosition = (u16)(gWarioData.xPosition + 0x200);
        secondaryValue = 64;
        wario = &gWarioData;
        if (specialWarioPose != 0) {
            secondaryValue = 4;
        }

        if (wario->horizontalDirection & 0x20) {
            targetPosition = (u16)(targetPosition + secondaryValue);
            if (currentPosition > targetPosition) {
                difference = currentPosition - targetPosition;
            } else {
                difference = targetPosition - currentPosition;
            }

            if (difference <= 8) {
                step = 0;
            } else {
                step = (difference - 8) >> 3;
            }

            if (currentPosition > targetPosition) {
                gCurrentSecondarySprite.xPosition -= step;
            } else if (currentPosition < targetPosition) {
                gCurrentSecondarySprite.xPosition += step;
            }
        } else {
            targetPosition = (u16)(targetPosition - secondaryValue);
            if (currentPosition > targetPosition) {
                difference = currentPosition - targetPosition;
            } else {
                difference = targetPosition - currentPosition;
            }

            if (difference <= 8) {
                step = 0;
            } else {
                step = (difference - 8) >> 3;
            }

            if (currentPosition < targetPosition) {
                gCurrentSecondarySprite.xPosition += step;
            } else if (currentPosition > targetPosition) {
                gCurrentSecondarySprite.xPosition -= step;
            }
        }

        currentPosition = (u16)(gCurrentSecondarySprite.yPosition + 0x200);
        if (specialWarioPose != 0) {
            targetPosition = (u16)(wario->yPosition + 0x200);
        } else {
            targetPosition = (u16)(wario->yPosition + wario->hitboxOffsetTop + 0x200);
        }

        if (currentPosition > targetPosition) {
            difference = currentPosition - targetPosition;
        } else {
            difference = targetPosition - currentPosition;
        }

        if (difference <= 8) {
            step = 0;
        } else {
            step = (difference - 8) >> 3;
        }

        if (currentPosition > targetPosition) {
            gCurrentSecondarySprite.yPosition -= step;
        } else if (currentPosition < targetPosition) {
            gCurrentSecondarySprite.yPosition += step;
        }
    }
}

void UpdateCoin50PointsSecondarySprite(void)
{
    struct SecondarySprite* sprite;
    u32 timer;
    u32 value;
    u32 tableOrPosition;

    AdvanceSecondarySpriteAnimation(sCoin50PointsOamData);
    sprite = &gCurrentSecondarySprite;

    switch (sprite->pose) {
        case 0:
            sprite->yPosition = gWarioData.yPosition - 90;

            if (gWarioData.horizontalDirection & 0x20) {
                sprite->xPosition = gWarioData.xPosition + 32;
                sprite->pose = 1;
            } else {
                sprite->xPosition = gWarioData.xPosition - 32;
                sprite->pose = 2;
            }
            break;

        case 1:
            sprite->timer++;
            timer = sprite->timer;

            if (timer > 30) {
                sprite->pose = 3;
            } else {
                sprite->xPosition += 4;
                tableOrPosition = (u32)sUnk_8416A06;
                value = timer << 1;
                value += tableOrPosition;
                value = *(const u16*)value;
                tableOrPosition = sprite->yPosition;
                value += tableOrPosition;
                sprite->yPosition = value;
            }
            break;

        case 2:
            sprite->timer++;
            timer = sprite->timer;

            if (timer > 30) {
                sprite->pose = 3;
            } else {
                sprite->xPosition -= 4;
                tableOrPosition = (u32)sUnk_8416A06;
                value = timer << 1;
                value += tableOrPosition;
                value = *(const u16*)value;
                tableOrPosition = sprite->yPosition;
                value += tableOrPosition;
                sprite->yPosition = value;
            }
            break;

        default:
            gCurrentSecondarySprite.status = 0;
            break;
    }
}

void UpdateTimeUpSecondarySprite(void)
{
    s32 scale;
    s32 cosine;
    u32 pose;

    scale = 0x200;
    pose = (u32)&gCurrentSecondarySprite;
    pose = *(const u8*)(pose + 3);

    switch (pose) {
        case 0: {
            u32 entry;
            register u32 table PIN("r1");
            struct SecondarySprite* sprite;
            u32 timer;
            u32 signedZero;
            s16 signedMovement;

            sprite = &gCurrentSecondarySprite;
            sprite->status |= 6;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F1C8);
            table = (u32)sUnk_84168B4;
            timer = sprite->timer;
            entry = timer << 1;
            entry += table;
            scale = *(const u16*)entry;
            signedZero = 0;
            signedMovement = *(const s16*)(entry + signedZero);

            if (signedMovement == 0x7FFF) {
                scale = 0x200;
                sprite->pose = 1;
            } else {
                entry = timer + 1;
                sprite->timer = entry;
            }
            break;
        }

        case 1:
            gCurrentSecondarySprite.status |= 6;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F1C8);

            if ((gCurrentSecondarySprite.yPosition >> 2) > 2) {
                gCurrentSecondarySprite.yPosition -= 16;
            } else {
                gCurrentSecondarySprite.yPosition = 8;
                gCurrentSecondarySprite.pose = 2;
            }
            break;

        case 2:
            gCurrentSecondarySprite.status |= 6;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F1C8);

            if (gTimerState == 2) {
                gCurrentSecondarySprite.pose = 3;
                gCurrentSecondarySprite.timer = 7;
            } else if (gTimerState == 4) {
                gCurrentSecondarySprite.pose = 6;
                gCurrentSecondarySprite.timer = 30;
            } else if (gTimerState == 11) {
                gCurrentSecondarySprite.pose = 44;
                gCurrentSecondarySprite.timer = 30;
                gCollectedKeyzer = 1;
                func_801E4D4();

                if (gCurrentPassage == 5) {
                    AutosaveFinalBoss();
                } else {
                    AutosaveBossClear();
                }
            }
            break;

        case 3: {
            struct SecondarySprite* sprite;
            GBA_TYPEOF(const u16**, &gCurrentSecondarySpriteOamData) animationDestination;
            u32 oldStatus;
            register u32 flag PIN("r1");
            u32 status;
            register u32 timerOrPosition PIN("r0");

            sprite = &gCurrentSecondarySprite;
            oldStatus = sprite->status;
            flag = 6;
            ASM_BARRIER(: "+r"(flag) : "r"(oldStatus));
            status = flag;
            status |= oldStatus;
            sprite->status = status;
            animationDestination = &gCurrentSecondarySpriteOamData;
            GP_SET(*animationDestination, sUnk_840F1C8);
            timerOrPosition = sprite->timer;

            if (timerOrPosition != 0) {
                timerOrPosition--;
                sprite->timer = timerOrPosition;
            } else {
                timerOrPosition = sprite->yPosition;
                timerOrPosition -= 16;
                sprite->yPosition = timerOrPosition;
                timerOrPosition <<= 16;

                if (timerOrPosition <= 0xFFB70000) {
                    sprite->yPosition = 24;
                    sprite->xPosition = 828;
                    sprite->pose = 4;
                    GP_SET(*animationDestination, sUnk_840F200);
                    timerOrPosition = 0xFD;
                    status &= timerOrPosition;
                    sprite->status = status;
                    gTimerState = 3;
                }
            }
            break;
        }

        case 4: {
            struct SecondarySprite* sprite;
            GBA_TYPEOF(const u16**, &gCurrentSecondarySpriteOamData) animationDestination;
            u32 oldStatus;
            register u32 flagOrPosition PIN("r0");
            register u32 status PIN("r2");
            register u32 zero PIN("r5");

            sprite = &gCurrentSecondarySprite;
            oldStatus = sprite->status;
            flagOrPosition = 4;
            zero = 0;
            ASM_BARRIER(: "+r"(flagOrPosition) : "r"(zero));
            status = flagOrPosition;
            status |= oldStatus;
            flagOrPosition = 0xFD;
            status &= flagOrPosition;
            sprite->status = status;
            animationDestination = &gCurrentSecondarySpriteOamData;
            GP_SET(*animationDestination, sUnk_840F200);
            flagOrPosition = sprite->xPosition;
            flagOrPosition -= 16;
            sprite->xPosition = flagOrPosition;
            flagOrPosition <<= 16;

            if (flagOrPosition <= 0x025F0000) {
                register u32 bit PIN("r1");
                register u32 newStatus PIN("r0");

                sprite->xPosition = 608;
                sprite->pose = 5;
                GP_SET(*animationDestination, sUnk_840F214);
                bit = 2;
                newStatus = status;
                newStatus |= bit;
                sprite->status = newStatus;
                m4aSongNumStart(452);
            }
            break;
        }

        case 5: {
            struct SecondarySprite* sprite;

            sprite = &gCurrentSecondarySprite;
            sprite->status |= 6;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F214);

            if (gTimerState != 4) {
                if (gStageScore != 0) {
                    break;
                }

                gTimerState = 4;
                gWarioPauseTimer = 1000;
                gUnk_3000046 = 1;
                PlayWarioDefeatJingle();
            }

            sprite->pose = 7;
            sprite->timer = 30;
            break;
        }

        case 6: {
            struct SecondarySprite* sprite;
            register u32 timerOrPosition PIN("r0");

            sprite = &gCurrentSecondarySprite;
            sprite->status |= 6;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F1C8);
            goto state_6_7_movement;

            case 7:
                sprite = &gCurrentSecondarySprite;
                sprite->status |= 6;
                GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F214);

            state_6_7_movement:
                timerOrPosition = sprite->timer;

                if (timerOrPosition != 0) {
                    timerOrPosition--;
                    sprite->timer = timerOrPosition;
                } else {
                    timerOrPosition = sprite->yPosition;
                    timerOrPosition -= 16;
                    sprite->yPosition = timerOrPosition;
                    timerOrPosition <<= 16;

                    if (timerOrPosition <= 0xFFB70000) {
                        sprite->pose = 8;
                    }
                }
                break;
        }

        case 8: {
            volatile u32* dmaReg;
            u32 dmaSourceA;
            u32 dmaSourceB;
            u32 dmaControlSmall;
            u32 dmaControlLarge;
            struct SecondarySprite* sprite;
            u32 offset;

            /* The original object stores through an absolute IWRAM literal here. */
            *(volatile u8*)0x03000047 = 5;
            gDisableWario = 1;
            sprite = &gCurrentSecondarySprite;
            sprite->status &= 0xFB;
            dmaReg = (volatile u32*)0x040000D4;
            dmaReg[0] = (u32)sTimeUpTextPal;
            dmaReg[1] = 0x05000240;
            dmaControlSmall = 0x80000010;
            dmaReg[2] = dmaControlSmall;
            dmaReg[2];

            dmaSourceA = (u32)sTimeUpTextGfx;
            dmaReg[0] = dmaSourceA;
            dmaReg[1] = 0x06010380;
            dmaReg[2] = 0x80000080;
            dmaReg[2];

            dmaSourceB = (u32)sTimeUpWarioGfx;
            dmaReg[0] = dmaSourceB;
            dmaReg[1] = 0x06010780;
            dmaReg[2] = 0x80000080;
            dmaReg[2];

            offset = (u32)-0x300;
            dmaSourceA += offset;
            dmaReg[0] = dmaSourceA;
            dmaReg[1] = 0x06010800;
            dmaControlLarge = 0x80000200;
            dmaReg[2] = dmaControlLarge;
            dmaReg[2];

            dmaSourceB += offset;
            dmaReg[0] = dmaSourceB;
            dmaReg[1] = 0x06010C00;
            dmaReg[2] = dmaControlLarge;
            dmaReg[2];

            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411AFC);
            scale = 16;
            sprite->pose = 11;

            dmaReg[0] = (u32)sTimeUpWarioPal;
            dmaReg[1] = 0x05000200;
            dmaReg[2] = dmaControlSmall;
            dmaReg[2];

            SpawnSecondarySprite(gWarioData.yPosition, gWarioData.xPosition, 76);

            if (gCurrentStageNumber == 4) {
                AutosaveDefeat();
            }
            break;
        }

        case 11:
            if (gTimerState == 6) {
                gCurrentSecondarySprite.yPosition = 320;
                gCurrentSecondarySprite.xPosition = 480;
                GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411B48);
                scale = 32;
                gCurrentSecondarySprite.pose++;
                ConvertObjPaletteBankToGrayscale(8);
            } else {
                GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411AFC);
                scale = 16;
            }
            break;

        case 12:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411B94);
            scale = 64;
            gCurrentSecondarySprite.pose++;
            break;

        case 13:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411BE0);
            scale = 96;
            gCurrentSecondarySprite.pose++;
            break;

        case 14:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411C2C);
            scale = 128;
            gCurrentSecondarySprite.pose++;
            break;

        case 15:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411C78);
            scale = 160;
            gCurrentSecondarySprite.pose++;
            break;

        case 16:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411CC4);
            scale = 192;
            gCurrentSecondarySprite.pose++;
            break;

        case 17:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411D10);
            scale = 224;
            gCurrentSecondarySprite.pose++;
            break;

        case 18:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411D5C);
            scale = 256;
            gCurrentSecondarySprite.pose++;
            break;

        case 19:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411DA8);
            scale = 288;
            gCurrentSecondarySprite.pose++;
            break;

        case 20:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411DF4);
            scale = 320;
            gCurrentSecondarySprite.pose++;
            break;

        case 21:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411E40);
            scale = 352;
            gCurrentSecondarySprite.pose++;
            break;

        case 22:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411E8C);
            scale = 384;
            gCurrentSecondarySprite.pose++;
            break;

        case 23:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411ED8);
            scale = 416;
            gCurrentSecondarySprite.pose++;
            break;

        case 25:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411F70);
            scale = 480;
            gCurrentSecondarySprite.pose++;
            break;

        case 26:
            gTimerState = 7;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FBC);
            scale = 512;
            gCurrentSecondarySprite.pose++;
            break;

        case 27:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411F70);
            scale = 480;
            gCurrentSecondarySprite.pose++;
            break;

        case 29:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411ED8);
            scale = 416;
            gCurrentSecondarySprite.pose++;
            break;

        case 30:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411E8C);
            scale = 384;
            gCurrentSecondarySprite.pose = 39;
            break;

        case 39:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411ED8);
            scale = 416;
            gCurrentSecondarySprite.pose++;
            break;

        case 24:
        case 28:
        case 40:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411F24);
            scale = 448;
            gCurrentSecondarySprite.pose++;
            break;

        case 41:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411F70);
            scale = 480;
            gCurrentSecondarySprite.pose++;
            gCurrentSecondarySprite.timer = 120;
            gTimerState = 8;
            break;

        case 42:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FBC);
            scale = 512;

            if (gCurrentSecondarySprite.timer == 50) {
                gTimerState = 9;
            }

            gCurrentSecondarySprite.timer--;

            if (gCurrentSecondarySprite.timer == 0) {
                gTimerState = 10;
                gCurrentSecondarySprite.pose++;

                if (gCurrentStageNumber == 4) {
                    gSubGameMode = 6;
                    gSpriteAiDropTimer = 0;
                    gStageExitType = 4;
                } else {
                    gSubGameMode = 6;
                    gSpriteAiDropTimer = 0;
                    gStageExitType = 1;
                }
            }
            break;

        case 43:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FBC);
            scale = 512;
            break;

        case 44: {
            struct SecondarySprite* sprite;
            register u32 timerOrPosition PIN("r0");

            sprite = &gCurrentSecondarySprite;
            sprite->status |= 6;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F1C8);
            timerOrPosition = sprite->timer;

            if (timerOrPosition != 0) {
                timerOrPosition--;
                sprite->timer = timerOrPosition;
            } else {
                timerOrPosition = sprite->yPosition;
                timerOrPosition -= 16;
                sprite->yPosition = timerOrPosition;
                timerOrPosition <<= 16;

                if (timerOrPosition <= 0xFFB70000) {
                    sprite->status = 0;
                }
            }
            break;
        }

        default:
            gCurrentSecondarySprite.status &= 0xFD;
            gCurrentSecondarySprite.status &= 0xFB;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_840F1C8);
            break;
    }

    if (gCurrentSecondarySprite.status & 2) {
        register s16* destination PIN("r6");
        register s32 tableOrSine PIN("r4");
        s32 fixedScale;

        destination = &gSecondarySpriteAffinePA;
        tableOrSine = (s32)sSinCosTable;
        cosine = sSinCosTable[0x40];
        fixedScale = (s16)scale;
        *destination = FixedMul(cosine, FixedInverse(fixedScale));

        destination = &gSecondarySpriteAffinePB;
        tableOrSine = sSinCosTable[0];
        *destination = FixedMul(tableOrSine, FixedInverse(fixedScale));

        destination = &gSecondarySpriteAffinePC;
        tableOrSine = -tableOrSine;
        *destination = FixedMul(tableOrSine, FixedInverse(fixedScale));

        tableOrSine = (s32)&gSecondarySpriteAffinePD;
        *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(fixedScale));
    }
}
void UpdateCDIconSecondarySprite(void)
{
    s32 stepOrSine;
    u32 scale;
    register u32 countOrDestination PIN("r6");
    register s32 cosine PIN("r8");
    struct SecondarySprite* sprite;
    u32 angle;
    u32 pose;
    u32 position;

    angle = 0;
    countOrDestination = 0;
    scale = 0x100;
    {
        struct SecondarySprite* current;

        current = &gCurrentSecondarySprite;
        sprite = (struct SecondarySprite*)(u32)current->status;
        pose = 4;
        pose |= (u32)sprite;
        current->status = pose;
        pose = current->pose;
        sprite = current;
    }

    switch (pose) {
        case 0: {
            u32 value;

            value = sprite->timer;
            value += 8;
            sprite->timer = value;
            value <<= 24;

            if (value != 0) {
                {
                    u32 timer;

                    timer = sprite->timer;
                    scale = timer << 1;
                }
            } else {
                scale = 0x200;
                sprite->pose = 1;
            }

            angle = gCurrentSecondarySprite.timer;
            gCurrentSecondarySprite.status |= 2;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411994);
            break;
        }

        case 1: {
            register u32 value PIN("r0");
            u32 temporary;

            stepOrSine = 2;
            sprite->timer += 16;
            position = sprite->yPosition;
            value = position - 9;
            value <<= 16;

            if (value > 0x02770000) {
                sprite->yPosition = 8;
                value = countOrDestination + 1;
                countOrDestination = (u8)value;
                stepOrSine = 4;
            } else {
                value = position;
                value -= 8;
                sprite->yPosition = value;
            }

            value = sprite->xPosition;
            temporary = value >> 2;

            if (temporary > 80) {
                value -= stepOrSine;
                sprite->xPosition = value;
            } else if (temporary < 80) {
                value = stepOrSine + value;
                sprite->xPosition = value;
            } else {
                sprite->xPosition = 320;
                value = countOrDestination + 1;
                countOrDestination = (u8)value;
            }

            angle = sprite->timer;

            if (countOrDestination == 2 && angle == 0) {
                sprite->pose = countOrDestination;
                sprite->timer = 10;
            }

            scale = 0x200;
            gCurrentSecondarySprite.status |= 2;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411994);
            break;
        }

        case 2: {
            u32 value;

            angle = 0;
            scale = 0x200;
            sprite = &gCurrentSecondarySprite;
            sprite->status |= 2;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411994);
            value = sprite->timer;
            value--;
            sprite->timer = value;
            value <<= 24;

            if (value == 0) {
                sprite->pose = 3;
            }
            break;
        }

        case 3: {
            register u32 temporary PIN("r1");
            u32 value;

            angle = 0;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411994);
            sprite = &gCurrentSecondarySprite;
            value = sprite->timer;
            value -= 16;
            sprite->timer = value;
            value <<= 24;

            if (value != 0) {
                sprite->status |= 2;
                sprite->yPosition--;
                sprite->xPosition--;
                {
                    register u32 mask PIN("r0");

                    scale = sprite->timer;
                    temporary = 0x80;
                    temporary <<= 1;
                    ASM_BARRIER(: "+r"(temporary));
                    mask = temporary;
                    ASM_BARRIER(: "+r"(mask));
                    scale |= mask;
                }
            } else {
                sprite->status &= 0xFD;
                sprite->yPosition = 8;
                sprite->xPosition = 320;
                sprite->pose = 4;
            }
            break;
        }

        case 4:
            sprite = &gCurrentSecondarySprite;
            sprite->status &= 0xFD;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411A4CFrame0);

            if (gWarioPauseTimer == 0) {
                sprite->pose = 5;
            }
            break;

        case 6: {
            register struct SecondarySprite* current PIN("r4");
            u8 timer;
            u32 position;

            current = &gCurrentSecondarySprite;
            current->status &= 0xFD;
            current->work0--;
            AdvanceSecondarySpriteAnimation(sUnk_8411A4C);

            timer = current->timer;

            if (timer != 0) {
                timer--;
                current->timer = timer;
            } else {
                position = current->yPosition;
                position -= 16;
                current->yPosition = position;
                position <<= 16;

                if (position <= 0xFFB70000) {
                    current->status = 0;
                }
            }
            break;
        }

        default:
            sprite = &gCurrentSecondarySprite;
            sprite->status &= 0xFD;

            if (gTimerState == 11) {
                sprite->pose = 6;
                sprite->timer = 30;
            }

            if (gWarioPauseTimer != 0 || gSubGameMode != 2) {
                sprite->work0--;
            }

            AdvanceSecondarySpriteAnimation(sUnk_8411A4C);
            break;
    }

    if (gCurrentSecondarySprite.status | 2) {
        u32 value;

        countOrDestination = (u32)&gSecondarySpriteAffinePA;
        stepOrSine = (s32)sSinCosTable;
        value = angle + 0x40;
        value <<= 1;
        value += stepOrSine;
        cosine = *(const s16*)value;
        *(s16*)countOrDestination = FixedMul(cosine, FixedInverse(scale));

        countOrDestination = (u32)&gSecondarySpriteAffinePB;
        value = angle << 1;
        value += stepOrSine;
        stepOrSine = *(const s16*)value;
        *(s16*)countOrDestination = FixedMul(stepOrSine, FixedInverse(scale));

        countOrDestination = (u32)&gSecondarySpriteAffinePC;
        stepOrSine = -stepOrSine;
        *(s16*)countOrDestination = FixedMul(stepOrSine, FixedInverse(scale));

        stepOrSine = (s32)&gSecondarySpriteAffinePD;
        *(s16*)stepOrSine = FixedMul(cosine, FixedInverse(scale));
    }
}
void PlayAllJewelPiecesCollectedJingle(void)
{
    if (gCollectedNEJewelPiece & gCollectedSEJewelPiece & gCollectedSWJewelPiece & gCollectedNWJewelPiece) {
        m4aSongNumStart(342);
    }
}

void ApplyJewelPieceIconAffine(void)
{
    s16* destination;
    register s32 tableOrSine PIN("r4");
    register s32 cosine PIN("r8");
    s32 scale;

    destination = &gSecondarySpriteAffinePA;
    tableOrSine = (s32)sSinCosTable;
    cosine = *(const s16*)(tableOrSine + 0x80);
    scale = 0x200;
    *destination = FixedMul(cosine, FixedInverse(scale));

    destination = &gSecondarySpriteAffinePB;
    tableOrSine = *(const s16*)tableOrSine;
    *destination = FixedMul(tableOrSine, FixedInverse(scale));

    destination = &gSecondarySpriteAffinePC;
    tableOrSine = -tableOrSine;
    *destination = FixedMul(tableOrSine, FixedInverse(scale));

    tableOrSine = (s32)&gSecondarySpriteAffinePD;
    *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(scale));

    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.status |= 2;
}
void UpdateJewelPieceIconSecondarySprite(void)
{
    struct SecondarySprite* current;
    struct SecondarySprite* sprite;
    register u32 value PIN("r0");
    u32 status;
    u32 neState;
    u32 seState;
    u32 swState;
    u32 nwState;
    u8* treasureState;
    volatile u32* dma;
    u32 timer;

    current = &gCurrentSecondarySprite;
    status = current->status;
    value = 4;
    value |= status;
    current->status = value;
    GP_SET(gCurrentSecondarySpriteOamData, sUnk_8414A54);
    value = current->pose;
    sprite = current;

    switch (value) {
        case 0:
            treasureState = &gCollectedNEJewelPiece;
            value = *treasureState;
            neState = value;

            if (neState == 1) {
                value++;
                *treasureState = value;
                value = 0;
                sprite->pose = neState;
                sprite->timer = value;
                break;
            }

            treasureState = &gCollectedSEJewelPiece;
            value = *treasureState;
            seState = value;

            if (seState == 1) {
                value++;
                *treasureState = value;
                neState = 0;
                value = 2;
                sprite->pose = value;
                sprite->timer = neState;
                break;
            }

            treasureState = &gCollectedSWJewelPiece;
            value = *treasureState;
            swState = value;

            if (swState == 1) {
                value++;
                *treasureState = value;
                neState = 0;
                value = 3;
                sprite->pose = value;
                sprite->timer = neState;
                break;
            }

            treasureState = &gCollectedNWJewelPiece;
            value = *treasureState;
            nwState = value;

            if (nwState == 1) {
                value++;
                *treasureState = value;
                neState = 0;
                value = 4;
                sprite->pose = value;
                sprite->timer = neState;
                break;
            }

            if (gTimerState <= 7) {
                break;
            }

            if (neState != 2 && seState != 2 && swState != 2 && nwState != 2) {
                break;
            }

            neState = 0;
            value = 6;
            sprite->pose = value;
            sprite->timer = neState;
            break;

        case 1:
            timer = sprite->timer;

            if (timer > 59) {
                sprite->timer = 20;
                sprite->pose = 5;
                sprite->status &= 0xFD;
                sprite->xPosition += 32;
            } else {
                if (timer == 20) {
                    PlayAllJewelPiecesCollectedJingle();
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialNEGfx;
                    dma[1] = 0x06011C20;
                    dma[2] = 0x80000010;
                    dma[2];
                }

                ApplyJewelPieceIconAffine();
            }
            break;

        case 2:
            timer = sprite->timer;

            if (timer > 59) {
                sprite->timer = 20;
                sprite->pose = 5;
                sprite->status &= 0xFD;
                sprite->xPosition += 32;
            } else {
                if (timer == 20) {
                    PlayAllJewelPiecesCollectedJingle();
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialSEGfx;
                    dma[1] = 0x06012020;
                    dma[2] = 0x80000010;
                    dma[2];
                }

                ApplyJewelPieceIconAffine();
            }
            break;

        case 3:
            timer = sprite->timer;

            if (timer > 59) {
                sprite->timer = 20;
                sprite->pose = 5;
                sprite->status &= 0xFD;
                sprite->xPosition += 32;
            } else {
                if (timer == 20) {
                    PlayAllJewelPiecesCollectedJingle();
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialSWGfx;
                    dma[1] = 0x06012000;
                    dma[2] = 0x80000010;
                    dma[2];
                }

                ApplyJewelPieceIconAffine();
            }
            break;

        case 4:
            timer = sprite->timer;

            if (timer > 59) {
                sprite->timer = 20;
                sprite->pose = 5;
                sprite->status &= 0xFD;
                sprite->xPosition += 32;
            } else {
                if (timer == 20) {
                    PlayAllJewelPiecesCollectedJingle();
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialNWGfx;
                    dma[1] = 0x06011C00;
                    dma[2] = 0x80000010;
                    dma[2];
                }

                ApplyJewelPieceIconAffine();
            }
            break;

        case 5:
            sprite->yPosition -= 4;
            value = sprite->timer;
            value--;
            sprite->timer = value;
            value = (u8)value;

            if (value == U8_MAX) {
                sprite->status = 0;
            }
            break;

        case 6:
            timer = sprite->timer;

            if (timer > 119) {
                sprite->pose = 7;
                sprite->status &= 0xFD;
            } else {
                if (timer == 20) {
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialNEGfx;
                    dma[1] = 0x06011C20;
                    dma[2] = 0x80000010;
                    dma[2];
                } else if (timer == 40) {
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialSEGfx;
                    dma[1] = 0x06012020;
                    dma[2] = 0x80000010;
                    dma[2];
                } else if (timer == 60) {
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialSWGfx;
                    dma[1] = 0x06012000;
                    dma[2] = 0x80000010;
                    dma[2];
                } else if (timer == 80) {
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sJewelPieceIconPartialNWGfx;
                    dma[1] = 0x06011C00;
                    dma[2] = 0x80000010;
                    dma[2];
                }

                ApplyJewelPieceIconAffine();
            }
            break;
    }
}
void UpdateCollectedCDSecondarySprite(void)
{
    struct SecondarySprite* sprite;
    s32 pose;
    register s32 status PIN("r2");
    u8* collectedCD;
    volatile u32* dma;
    u32 timer;
    register u32 value PIN("r0");

    sprite = &gCurrentSecondarySprite;
    pose = sprite->status;
    value = 4;
    ASM_BARRIER(: "+r"(value));
    status = value;
    status |= pose;
    sprite->status = status;
    GP_SET(gCurrentSecondarySpriteOamData, sUnk_8414A4C);
    pose = sprite->pose;

    switch (pose) {
        case 0:
            collectedCD = &gCollectedCD;
            value = *collectedCD;
            status = value;

            if (status == 1) {
                value++;
                *collectedCD = value;
            } else {
                if (status != 2) {
                    break;
                }

                if (gTimerState <= 8) {
                    break;
                }
            }

            sprite->pose = status;
            sprite->timer = pose;
            break;

        case 1:
            timer = sprite->timer;

            if (timer > 59) {
                sprite->timer = 20;
                sprite->pose = 3;
                status &= 0xFD;
                sprite->status = status;
                sprite->xPosition += 16;
            } else {
                if (timer == 20) {
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sCDIconPartialGfx;
                    dma[1] = 0x060114C0;
                    dma[2] = 0x80000010;
                    dma[2];
                }

                ApplyJewelPieceIconAffine();
            }
            break;

        case 2:
            timer = sprite->timer;

            if (timer > 59) {
                sprite->pose = 3;
                status &= 0xFD;
                sprite->status = status;
            } else {
                if (timer == 20) {
                    dma = (volatile u32*)0x040000D4;
                    dma[0] = (u32)sCDIconPartialGfx;
                    dma[1] = 0x060114C0;
                    dma[2] = 0x80000010;
                    dma[2];
                }

                ApplyJewelPieceIconAffine();
            }
            break;

        case 3:
            sprite->yPosition -= 4;
            value = sprite->timer;
            value--;
            sprite->timer = value;
            value = (u8)value;

            if (value == U8_MAX) {
                sprite->status = 0;
            }
            break;
    }
}
void UpdateTreasureExitSecondarySprite(void)
{
    s32 scale;
    u32 angle;
    s32 cosine;
    struct SecondarySprite* current;
    struct SecondarySprite* sprite;
    u32 pose;
    u32 flags;
    u32 position;

    scale = 0x200;
    angle = 0;
    current = &gCurrentSecondarySprite;
    flags = current->status;
    flags |= 6;
    current->status = flags;
    pose = current->pose;
    sprite = current;

    switch (pose) {
        case 0: {
            u32 stateFlags;
            register u32 condition PIN("r0");

            sprite->work0 += 16;
            angle = sprite->work0;
            scale = 0x100;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FE2);
            stateFlags = sprite->timer;
            condition = 1;
            condition &= stateFlags;
            cosine = (s32)&gCurrentSecondarySpriteOamData;

            if (condition == 0) {
                u32 mask;
                u32 background;
                register u32 backgroundPosition PIN("r4");
                volatile u16* backgroundAddress;
                u32 difference;
                u32 scaleCopy;

                backgroundAddress = &gBg1YPosition;
                background = sprite->yPosition;
                backgroundPosition = *backgroundAddress;
                difference = background - backgroundPosition;
                scaleCopy = scale;
                difference += scaleCopy;
                difference <<= 16;
                mask = 252;
                mask <<= 18;
                mask &= difference;
                difference = mask >> 16;

                if (difference > 0x280) {
                    condition = background;
                    condition -= 8;
                } else if (difference <= 0x27F) {
                    condition = background;
                    condition += 8;
                } else {
                    condition = 1;
                    condition |= stateFlags;
                    sprite->timer = condition;
                    difference = 0xC0;
                    difference <<= 1;
                    condition = backgroundPosition + difference;
                }

                sprite->yPosition = condition;
            }

            {
                register struct SecondarySprite* xSprite PIN("r4");
                u32 xFlags;

                xSprite = sprite;
                xFlags = xSprite->timer;
                condition = 2;
                condition &= xFlags;

                if (condition == 0) {
                    u32 mask;
                    u32 background;
                    register u32 backgroundPosition PIN("r6");
                    u32 difference;
                    volatile u16* backgroundAddress;

                    backgroundAddress = &gBg1XPosition;
                    background = xSprite->xPosition;
                    backgroundPosition = *backgroundAddress;
                    difference = background - backgroundPosition;
                    difference += 0x100;
                    difference <<= 16;
                    mask = 254;
                    mask <<= 19;
                    mask &= difference;
                    difference = mask >> 16;

                    if (difference > 0x2E0) {
                        condition = background;
                        condition -= 16;
                        xSprite->xPosition = condition;
                    } else if (difference <= 0x2DF) {
                        condition = background;
                        condition += 16;
                        xSprite->xPosition = condition;
                    } else {
                        condition = 2;
                        condition |= xFlags;
                        sprite->timer = condition;
                        condition = backgroundPosition + 0x1E0;
                        sprite->xPosition = condition;
                    }
                }
            }

            if (gCurrentSecondarySprite.timer == 3 && angle == 0) {
                const u16* animation;
                GBA_PTR(const u16*) * animationDestination;

                gCurrentSecondarySprite.pose++;
                gTimerState = 6;
                animation = sUnk_8411FEA;
                animationDestination = (GBA_TYPEOF(const u16**, animationDestination))cosine;
                GP_SET(*animationDestination, animation);
                VoiceSetPlay(11);
            }
            break;
        }

        case 1:
            scale = 0x100;

            if (gTimerState == 7) {
                volatile u32* dmaReg;
                u32 dmaValue;
                register u32 dmaControl PIN("r2");

                dmaReg = (volatile u32*)0x040000D4;
                dmaValue = (u32)sTimeUpEffect0Gfx;
                dmaReg[0] = dmaValue;
                dmaValue = 0x06010B00;
                dmaReg[1] = dmaValue;
                dmaControl = 0x80000080;
                dmaReg[2] = dmaControl;
                dmaValue = dmaReg[2];
                dmaValue = (u32)sTimeUpEffect1Gfx;
                dmaReg[0] = dmaValue;
                dmaValue = 0x06010F00;
                dmaReg[1] = dmaValue;
                dmaReg[2] = dmaControl;
                dmaReg[2];
                scale += 32;
                GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FF8);
                gCurrentSecondarySprite.pose++;
            } else {
                GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FEA);
            }
            break;

        case 2:
            scale = 0x140;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8412006);
            gCurrentSecondarySprite.pose++;
            break;

        case 3:
            scale = 0x160;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8412014);
            gCurrentSecondarySprite.pose++;
            break;

        case 4:
            scale = 0x180;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8412022);
            gCurrentSecondarySprite.pose++;
            break;

        case 5:
            scale = 0x1A0;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8412030);
            gCurrentSecondarySprite.pose++;
            break;

        case 6:
            scale = 0x1C0;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_841203E);
            gCurrentSecondarySprite.pose++;
            break;

        case 7: {
            volatile u32* dmaReg;
            u32 dmaValue;
            register u32 dmaControl PIN("r2");

            scale = 0x1E0;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_841204C);
            dmaReg = (volatile u32*)0x040000D4;
            dmaValue = (u32)sTimeUpEffect2Gfx;
            dmaReg[0] = dmaValue;
            dmaValue = 0x06010B00;
            dmaReg[1] = dmaValue;
            dmaControl = 0x80000080;
            dmaReg[2] = dmaControl;
            dmaValue = dmaReg[2];
            dmaValue = (u32)sTimeUpEffect3Gfx;
            dmaReg[0] = dmaValue;
            dmaValue = 0x06010F00;
            dmaReg[1] = dmaValue;
            dmaReg[2] = dmaControl;
            dmaReg[2];
            gCurrentSecondarySprite.timer = 30;
            gCurrentSecondarySprite.pose++;
            m4aSongNumStart(454);
            break;
        }

        case 8:
            scale = 0x200;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_841205A);
            gCurrentSecondarySprite.timer--;

            if (gCurrentSecondarySprite.timer == 0) {
                gCurrentSecondarySprite.timer = 0;
                gCurrentSecondarySprite.pose++;
            }
            break;

        case 9: {
            u32 timer;
            u16 movement;
            s16 signedMovement;

            scale = 0x200;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_841205A);
            timer = gCurrentSecondarySprite.timer;
            movement = (u16)sUnk_8416A46[timer];
            signedMovement = sUnk_8416A46[timer];

            if (signedMovement == 0x7FFF) {
                gCurrentSecondarySprite.pose++;
            } else {
                u32 newPosition;

                gCurrentSecondarySprite.timer = timer + 1;
                newPosition = gCurrentSecondarySprite.yPosition;
                newPosition += movement;
                gCurrentSecondarySprite.yPosition = newPosition;
            }
            break;
        }

        case 10:
            scale = 0x200;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_841205A);
            position = gCurrentSecondarySprite.yPosition;
            position += 6;
            gCurrentSecondarySprite.yPosition = position;

            if ((u32)((position - gBg1YPosition) << 16) > 0x037F0000) {
                gCurrentSecondarySprite.pose++;
            }
            break;

        default:
            flags &= 0xFD;
            current->status = flags;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_841205A);
            break;
    }

    if (gCurrentSecondarySprite.status & 2) {
        s16* destination;
        s32 tableOrSine;
        register s32 fixedScale PIN("r5");

        destination = &gSecondarySpriteAffinePA;
        tableOrSine = (s32)sSinCosTable;
        cosine = sSinCosTable[angle + 0x40];
        fixedScale = scale;
        *destination = FixedMul(cosine, FixedInverse(fixedScale));

        destination = &gSecondarySpriteAffinePB;
        tableOrSine = sSinCosTable[angle];
        *destination = FixedMul(tableOrSine, FixedInverse(fixedScale));

        destination = &gSecondarySpriteAffinePC;
        tableOrSine = -tableOrSine;
        *destination = FixedMul(tableOrSine, FixedInverse(fixedScale));

        tableOrSine = (s32)&gSecondarySpriteAffinePD;
        *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(fixedScale));
    }
}
void UpdateStageExitVortexSecondarySprite(void)
{
    register s32 scale PIN("sl");
    register u32 angle PIN("r9");
    register s32 cosine PIN("r8");
    struct SecondarySprite* current;
    struct SecondarySprite* sprite;
    s32 pose;
    u32 position;
    u32 relative;
    u32 unusedZero;

    scale = 0x100;
    angle = 0;
    current = &gCurrentSecondarySprite;
    {
        u32 oldStatus;
        u32 newStatus;

        oldStatus = current->status;
        newStatus = 6;
        unusedZero = 0;
        /* Preserve the original agbcc low-register pressure and scheduling. */
        ASM_BARRIER_ARM_VOLATILE(: "+l"(unusedZero) : "r"(oldStatus), "r"(newStatus), "r"(current) : "r3", "r4", "r5", "r6");
        newStatus |= oldStatus;
        current->status = newStatus;
    }
    pose = current->pose;
    sprite = current;

    switch (pose) {
        case 0: {
            register u32 stateFlags PIN("r6");
            u32 condition;

            current->work0 += 16;
            angle = current->work0;
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FE2);
            stateFlags = current->timer;
            condition = 1;
            condition &= stateFlags;
            cosine = (s32)&gCurrentSecondarySpriteOamData;

            if (condition == 0) {
                u32 mask;
                u32 background;
                u32 backgroundPosition;
                u32 difference;

                mask = (u32)&gBg1YPosition;
                background = current->yPosition;
                backgroundPosition = *(const u16*)mask;
                difference = background - backgroundPosition;
                difference += scale;
                difference <<= 16;
                mask = 252;
                mask <<= 18;
                mask &= difference;
                difference = mask >> 16;

                if (difference > 0x280) {
                    condition = background;
                    condition -= 8;
                } else if (difference <= 0x27F) {
                    condition = background;
                    condition += 8;
                } else {
                    condition = 1;
                    condition |= stateFlags;
                    current->timer = condition;
                    difference = 0xC0;
                    difference <<= 1;
                    condition = backgroundPosition + difference;
                }

                current->yPosition = condition;
            }

            {
                register struct SecondarySprite* xSprite PIN("r3");

                xSprite = sprite;
                stateFlags = xSprite->timer;
                condition = 2;
                condition &= stateFlags;

                if (condition == 0) {
                    u32 mask;
                    u32 background;
                    u32 backgroundPosition;
                    u32 difference;

                    mask = (u32)&gBg1XPosition;
                    background = xSprite->xPosition;
                    backgroundPosition = *(const u16*)mask;
                    difference = background - backgroundPosition;
                    mask = 0x100;
                    difference += mask;
                    difference <<= 16;
                    mask = 254;
                    mask <<= 19;
                    mask &= difference;
                    difference = mask >> 16;

                    if (difference > 0x2E0) {
                        condition = background;
                        condition -= 16;
                        xSprite->xPosition = condition;
                    } else if (difference <= 0x2DF) {
                        condition = background;
                        condition += 16;
                        xSprite->xPosition = condition;
                    } else {
                        condition = 2;
                        condition |= stateFlags;
                        sprite->timer = condition;
                        difference = 0xF0;
                        difference <<= 1;
                        condition = backgroundPosition + difference;
                        sprite->xPosition = condition;
                    }
                }
            }

            relative = (u32)&gCurrentSecondarySprite;
            if (((struct SecondarySprite*)relative)->timer == 3) {
                position = angle;
                if (position == 0) {
                    ((struct SecondarySprite*)relative)->pose++;
                    {
                        const u16* animation;

                        animation = sUnk_8411FEA;
                        sprite = (struct SecondarySprite*)cosine;
                        GP_SET(*(GBA_PTR(const u16*) *)sprite, animation);
                    }
                    ((struct SecondarySprite*)relative)->timer = 30;
                    m4aSongNumStart(23);
                }
            }
            break;
        }

        case 1: {
            u32 value;

            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FEA);
            value = current->timer;
            value--;
            current->timer = value;
            value <<= 24;

            if (value == 0) {
                register u32 zero PIN("r0");

                zero = angle;
                current->timer = zero;
                current->pose++;
                VoiceSetPlay(11);
            }
            break;
        }

        case 2: {
            u32 timer;
            u32 movementWord;
            u32 signedZero;
            u32 tableAddress;
            s16 signedMovement;

            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FEA);
            timer = current->timer;
            relative = (u32)sUnk_8416A88;
            tableAddress = timer << 1;
            tableAddress += relative;
            movementWord = *(const u16*)tableAddress;
            signedZero = 0;
            signedMovement = *(const s16*)(tableAddress + signedZero);

            if (signedMovement == 0x7FFF) {
                current->pose++;
            } else {
                tableAddress = timer + 1;
                current->timer = tableAddress;
                current->yPosition += movementWord;
            }
            break;
        }

        case 3: {
            u32 noValue;
            u32 statePosition;

            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FEA);
            statePosition = current->yPosition;
            statePosition += 16;
            noValue = 0;
            current->yPosition = statePosition;

            if ((u32)((statePosition - gBg1YPosition) << 16) > 0x037F0000) {
                current->pose++;

                if (gCurrentStageNumber == 4) {
                    gSubGameMode = 6;
                    gSpriteAiDropTimer = noValue;
                    gStageExitType = pose;
                } else {
                    gSubGameMode = 6;
                    gSpriteAiDropTimer = noValue;
                    gStageExitType = noValue;
                }
            }
            break;
        }

        default:
            GP_SET(gCurrentSecondarySpriteOamData, sUnk_8411FEA);
            break;
    }

    if (gCurrentSecondarySprite.status & 2) {
        s16* destination;
        register s32 tableOrSine PIN("r4");
        s32 fixedScale;

        destination = &gSecondarySpriteAffinePA;
        tableOrSine = (s32)sSinCosTable;
        cosine = sSinCosTable[angle + 0x40];
        fixedScale = scale;
        *destination = FixedMul(cosine, FixedInverse(fixedScale));

        destination = &gSecondarySpriteAffinePB;
        tableOrSine = sSinCosTable[angle];
        *destination = FixedMul(tableOrSine, FixedInverse(fixedScale));

        destination = &gSecondarySpriteAffinePC;
        tableOrSine = -tableOrSine;
        *destination = FixedMul(tableOrSine, FixedInverse(fixedScale));

        tableOrSine = (s32)&gSecondarySpriteAffinePD;
        *(s16*)tableOrSine = FixedMul(cosine, FixedInverse(fixedScale));
    }

    /* Keep the zero temporary live so agbcc reserves r7 through the function. */
    ASM_BARRIER(: : "r"(unusedZero));
}
void ClampFallingSecondarySpriteAtBottom(u16 xVelocity)
{
    u16 position;

    position = (gCurrentSecondarySprite.yPosition >> 2) & 0xFF;
    position -= 0xB5;

    if (position <= 0x12) {
        gCurrentSecondarySprite.xPosition += xVelocity * 4;
        gCurrentSecondarySprite.yPosition = 0x380;
    }
}

void UpdateFallingSecondarySpriteVariantA(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 10;
    AdvanceSecondarySpriteAnimation(sUnk_841497C);
    ClampFallingSecondarySpriteAtBottom(80);
}

void UpdateFallingSecondarySpriteVariantB(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 12;
    AdvanceSecondarySpriteAnimation(sUnk_841497C);
    ClampFallingSecondarySpriteAtBottom(160);
}

void UpdateFallingSecondarySpriteVariantC(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 14;
    AdvanceSecondarySpriteAnimation(sUnk_841497C);
    ClampFallingSecondarySpriteAtBottom(96);
}

void UpdateFallingSecondarySpriteVariantD(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 11;
    AdvanceSecondarySpriteAnimation(sUnk_841497C);
    ClampFallingSecondarySpriteAtBottom(16);
}

void UpdateFallingSecondarySpriteVariantE(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 12;
    AdvanceSecondarySpriteAnimation(sUnk_8414A0C);
    ClampFallingSecondarySpriteAtBottom(200);
}

void UpdateFallingSecondarySpriteVariantF(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 10;
    AdvanceSecondarySpriteAnimation(sUnk_8414A1C);
    ClampFallingSecondarySpriteAtBottom(120);
}

void UpdateFallingSecondarySpriteVariantG(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 14;
    AdvanceSecondarySpriteAnimation(sUnk_8414A2C);
    ClampFallingSecondarySpriteAtBottom(100);
}

void UpdateFallingSecondarySpriteVariantH(void)
{
    gCurrentSecondarySprite.timer++;
    gCurrentSecondarySprite.yPosition += 16;
    AdvanceSecondarySpriteAnimation(sUnk_8414A3C);
    ClampFallingSecondarySpriteAtBottom(160);
}

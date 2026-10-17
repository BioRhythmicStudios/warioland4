#include "fixed_point.h"
#include "global_data.h"
#include "main.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_ai/toy_block_round.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "wario.h"
#include "gba/m4a.h"


void InitToyBlockRound(void)
{
    struct PrimarySpriteData* sprite;
    u8* ptr;
    u8* temp;
    u16 oldStatus;
    u32 status;
    register u8 zero PIN("r2");
    u16 zero16;
    const struct AnimationFrame* oam;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    status = SPRITE_STATUS_AFFINE | SPRITE_STATUS_HEAVY;
    zero = 0;
    zero16 = 0;
    status |= oldStatus;
    ptr = (u8*)sprite;
    *(u16*)ptr = status;
    ((struct PrimarySpriteData*)ptr)->xPosition += 32;
    ptr += 32;
    *ptr = 32;
    temp = (u8*)sprite + 33;
    *temp = zero;
    ptr += 2;
    *ptr = 16;
    ptr++;
    *ptr = 128;
    temp = (u8*)sprite + 36;
    *temp = zero;
    ptr += 2;
    *ptr = 40;
    ptr += 1;
    *ptr = 36;
    oam = sToyBlockRoundOam;
    ptr = (u8*)sprite;
    GP_SET(((struct PrimarySpriteData*)ptr)->pOamData, oam);
    ptr[22] = zero;
    *(u16*)(ptr + 20) = zero16;
    ((struct PrimarySpriteData*)ptr)->warioCollision = 26;
    temp = (u8*)sprite + 40;
    *temp = zero;
}

void ResetToyBlockRound(void)
{
    gCurrentSprite.pose = 48;
    gCurrentSprite.status &= 0xFCFF;
}

void ToyBlockRoundIdle(void)
{
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            func_8023BFC(gCurrentSprite.yPosition,
                (u16)(gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft));
        } else {
            func_8023BFC(gCurrentSprite.yPosition,
                (u16)(gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight));
        }
        if (gSpriteCollisionTileType == 0) {
            gCurrentSprite.pose = 29;
        }
    }
}

void StartToyBlockRoundFalling(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void ToyBlockRoundFalling(void)
{
    register int previousIndex PIN("r1");
    struct PrimarySpriteData* sprite;
    u32 newY;
    u8 index;
    u16 previousMovement;
    s16 movement;

    sprite = &gCurrentSprite;
    newY = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        sprite->status |= SPRITE_STATUS_UNDERWATER;
        m4aSongNumStart(SE_ENTER_WATER);
    }

    if (gSpriteCollisionResult != 0) {
        register u8 zero PIN("r2");
        u8* workPtr;
        u8* workPtr2;

        zero = 0;
        sprite->yPosition = newY;
        if (sprite->xPosition > gWarioData.xPosition) {
            sprite->pose = 36;
        } else {
            sprite->pose = 38;
        }
        workPtr = (u8*)sprite + 40;
        *workPtr = zero;
        workPtr2 = (u8*)sprite + 41;
        *workPtr2 = 4;
    } else {
        index = sprite->work3;
        if (sprite->status & SPRITE_STATUS_HEAVY) {
            movement = sUnk_8352AFC[index];
            if (movement == 0x7FFF) {
                previousIndex = index - 1;
                previousMovement = ((const u16*)sUnk_8352AFC)[previousIndex];
                sprite->yPosition += previousMovement;
            } else {
                sprite->work3 = index + 1;
                sprite->yPosition += movement;
            }
        } else {
            movement = sSpriteGravityVelocityTable[index];
            if (movement == 0x7FFF) {
                previousIndex = index - 1;
                previousMovement = ((const u16*)sSpriteGravityVelocityTable)[previousIndex];
                sprite->yPosition += previousMovement;
            } else {
                sprite->work3 = index + 1;
                sprite->yPosition += movement;
            }
        }
    }
}

void StartToyBlockRoundCounterclockwiseSpin(void)
{
    gCurrentSprite.pose = 72;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockRoundClockwiseSpin(void)
{
    gCurrentSprite.pose = 74;
    gCurrentSprite.work2 = 6;
    gCurrentSprite.work3 = 0;
}

void StartToyBlockRoundCounterclockwisePush(void)
{
    gCurrentSprite.pose = 44;
    gCurrentSprite.work0 = 0;
}

void ToyBlockRoundCounterclockwisePush(void)
{
    if (gCurrentSprite.work2 > 3) {
        func_8025F48();
    } else {
        gCurrentSprite.pose = 47;
    }
}

void StartToyBlockRoundClockwisePush(void)
{
    gCurrentSprite.pose = 46;
    gCurrentSprite.work0 = 0;
}

void ToyBlockRoundClockwisePush(void)
{
    if (gCurrentSprite.work2 > 3) {
        func_802608C();
    } else {
        gCurrentSprite.pose = 47;
    }
}

void StartToyBlockRoundClockwiseRoll(void)
{
    if (gCurrentSprite.pose == 79) {
        gCurrentSprite.work2 = 20;
    } else {
        gCurrentSprite.work2 = 10;
    }
    gCurrentSprite.work0 = 0;
    gCurrentSprite.pose = 36;
}

void ToyBlockRoundClockwiseRoll(void)
{
    if (gCurrentSprite.work2 != 0) {
        func_802608C();
    } else {
        gCurrentSprite.pose = 47;
    }
}

void StartToyBlockRoundCounterclockwiseRoll(void)
{
    if (gCurrentSprite.pose == 80) {
        gCurrentSprite.work2 = 20;
    } else {
        gCurrentSprite.work2 = 10;
    }
    gCurrentSprite.work0 = 0;
    gCurrentSprite.pose = 38;
}

void ToyBlockRoundCounterclockwiseRoll(void)
{
    if (gCurrentSprite.work2 != 0) {
        func_8025F48();
    } else {
        gCurrentSprite.pose = 47;
    }
}

void SpriteToyBlockRound(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 0;
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = 107;
    }

    switch (gCurrentSprite.pose) {
        case 0:
            InitToyBlockRound();
        case 35:
        case 37:
        case 47:
        case 49:
        case 51:
        case 53:
        case 105:
        case 106:
            ResetToyBlockRound();
        case 48:
            ToyBlockRoundIdle();
            break;

        case 29:
            StartToyBlockRoundFalling();
        case 30:
            ToyBlockRoundFalling();
            break;

        case 43:
            StartToyBlockRoundCounterclockwisePush();
        case 44:
            ToyBlockRoundCounterclockwisePush();
            gCurrentSprite.currentAnimationFrame -= 5;
            break;

        case 45:
            StartToyBlockRoundClockwisePush();
        case 46:
            ToyBlockRoundClockwisePush();
            gCurrentSprite.currentAnimationFrame += 5;
            break;

        case 31:
        case 79:
            StartToyBlockRoundClockwiseRoll();
        case 36:
            gCurrentSprite.currentAnimationFrame += gCurrentSprite.work2;
            ToyBlockRoundClockwiseRoll();
            break;

        case 33:
        case 80:
            StartToyBlockRoundCounterclockwiseRoll();
        case 38:
            gCurrentSprite.currentAnimationFrame -= gCurrentSprite.work2;
            ToyBlockRoundCounterclockwiseRoll();
            break;

        case 55:
            func_8024688();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 56:
            func_80246B8();
            gCurrentSprite.currentAnimationFrame -= gCurrentSprite.work2;
            break;

        case 57:
            func_802473C();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 58:
            func_802476C();
            gCurrentSprite.currentAnimationFrame -= gCurrentSprite.work2;
            break;

        case 59:
            func_80247F0();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 60:
            func_8024820();
            gCurrentSprite.currentAnimationFrame -= gCurrentSprite.work2;
            break;

        case 61:
            func_80248A4();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 62:
            func_80248D4();
            gCurrentSprite.currentAnimationFrame += gCurrentSprite.work2;
            break;

        case 63:
            func_8024958();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 64:
            func_8024988();
            gCurrentSprite.currentAnimationFrame += gCurrentSprite.work2;
            break;

        case 65:
            func_8024A0C();
            m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        case 66:
            func_8024A3C();
            gCurrentSprite.currentAnimationFrame += gCurrentSprite.work2;
            break;

        case 67:
            func_8024AC0();
        case 68:
            func_8024AD4();
            gCurrentSprite.currentAnimationFrame -= gCurrentSprite.work2;
            break;

        case 69:
            func_8024BEC();
        case 70:
            func_8024C00();
            gCurrentSprite.currentAnimationFrame += gCurrentSprite.work2;
            break;

        case 71:
            StartToyBlockRoundCounterclockwiseSpin();
        case 72:
            func_8024AD4();
            gCurrentSprite.currentAnimationFrame -= gCurrentSprite.work2;
            break;

        case 73:
            StartToyBlockRoundClockwiseSpin();
        case 74:
            func_8024C00();
            gCurrentSprite.currentAnimationFrame += gCurrentSprite.work2;
            break;

        default:
            SpriteUtilDie();
            break;
    }

    if (gCurrentSprite.status & SPRITE_STATUS_AFFINE) {
        gCurrentSprite.affinePA = FixedMul(COS(gCurrentSprite.currentAnimationFrame), FixedInverse(256));
        gCurrentSprite.affinePB = FixedMul(SIN(gCurrentSprite.currentAnimationFrame), FixedInverse(256));
        gCurrentSprite.affinePC = FixedMul(-SIN(gCurrentSprite.currentAnimationFrame), FixedInverse(256));
        gCurrentSprite.affinePD = FixedMul(COS(gCurrentSprite.currentAnimationFrame), FixedInverse(256));
    }

    if (gSubGameMode == 2 || gSubGameMode == 8) {
        gCurrentSprite.currentAnimationFrame--;
    }
    func_8026838();
}

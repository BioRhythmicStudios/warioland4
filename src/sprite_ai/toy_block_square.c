#include "block.h"
#include "global_data.h"
#include "sprite.h"
#include "sprite_ai/toy_block_square.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "wario.h"
#include "sound.h"
#include "gba/m4a.h"

extern const struct AnimationFrame sToyBlockSquareOam[];
extern const s16 sToyBlockSquareYMovement[];

void ToyBlockSquareCheckSpriteCollisions(void)
{
    struct PrimarySpriteData* current;
    struct PrimarySpriteData* sprite;
    struct PrimarySpriteData* sprites;
    u16 currentY;
    u16 currentX;
    u16 currentTop;
    u16 currentBottom;
    u16 currentLeft;
    u16 currentRight;
    u16 spriteY;
    u16 spriteX;
    u16 new_var;
    u16 top;
    u16 bottom;
    u16 left;
    u16 right;
    s32 i;

    current = &gCurrentSprite;
    currentY = (u16)(current->yPosition + 0x1000);
    currentX = (u16)(current->xPosition + 0x1000);
    currentTop = (u16)(currentY - current->hitboxExtentUp);
    currentBottom = (u16)(currentY + current->hitboxExtentDown);
    currentLeft = (u16)(currentX - current->hitboxExtentLeft);
    currentRight = (u16)(currentX + current->hitboxExtentRight);

    i = 0;
    sprites = gSpriteData;
    for (; i < MAX_SPRITE_COUNT; i++)
    {
        u16 maskedStatus;
        register u32 statusMask PIN("r3");

        sprite = (struct PrimarySpriteData*)((u32)(i * sizeof(struct PrimarySpriteData)) + (u32)sprites);
        maskedStatus = sprite->status;
        statusMask = 0x217;
        maskedStatus &= statusMask;

        if (maskedStatus == 3 && sprite->globalID != PSPRITE_TOY_BLOCK_SQUARE)
        {
            register u32 basePosition PIN("r0");
            register u32 ySum PIN("r2");

            {
                register u32 baseSeed PIN("r1");
                baseSeed = 0x1000;
                ASM_BARRIER(: : "r"(baseSeed));
                basePosition = baseSeed;
            }

            {
                register u16 positionY PIN("r3");
                positionY = sprite->yPosition;
                ySum = basePosition + positionY;
                spriteY = (u16)ySum;
            }

            {
                s32 positionX;
                positionX = sprite->xPosition;
                spriteX = (u16)(basePosition + positionX);
            }

            top = (u16)(spriteY - sprite->hitboxExtentUp);
            new_var = currentTop;
            bottom = (u16)(spriteY + sprite->hitboxExtentDown);
            left = (u16)(spriteX - sprite->hitboxExtentLeft);
            right = (u16)(spriteX + sprite->hitboxExtentRight);

            if (SpriteCollisionCheckObjectsTouching(
                    new_var,
                    currentBottom,
                    currentLeft,
                    currentRight,
                    top,
                    bottom,
                    left,
                    right))
            {
                if (spriteX > currentX)
                    sprite->pose = 0x6A;
                else
                    sprite->pose = 0x69;

                sprites[i].disableWarioCollisionTimer = 15;
                sprites[i].status |= SPRITE_STATUS_MAYBE_DEAD;
                m4aSongNumStart(SOUND_3B);
            }
        }
    }
}

void ToyBlockSquareSetPassableTiles(void)
{
    u16 y;
    u16 x;
    u16 yTop;
    u16 xLeft;
    s8* blockType;
    s32 value;

    y = gCurrentSprite.yPosition;
    x = gCurrentSprite.xPosition;
    blockType = &gSpriteTileInteractionMode;
    value = 2;
    gSpriteTileInteractionMode = 2;
    yTop = y - 4;
    xLeft = x - 4;
    GetSpriteBlockCollisionAtPosition(yTop, xLeft);
    gSpriteTileInteractionMode = 2;
    x += 4;
    GetSpriteBlockCollisionAtPosition(yTop, x);
    gSpriteTileInteractionMode = 2;
    y -= 68;
    GetSpriteBlockCollisionAtPosition(y, x);
    gSpriteTileInteractionMode = 2;
    GetSpriteBlockCollisionAtPosition(y, xLeft);
}

void ToyBlockSquareSetSolidTiles(void)
{
    u16 y;
    u16 x;
    u16 yTop;
    u16 xLeft;
    s8* blockType;
    s32 value;

    y = gCurrentSprite.yPosition;
    x = gCurrentSprite.xPosition;
    blockType = &gSpriteTileInteractionMode;
    value = 3;
    gSpriteTileInteractionMode = 3;
    yTop = y - 4;
    xLeft = x - 4;
    GetSpriteBlockCollisionAtPosition(yTop, xLeft);
    gSpriteTileInteractionMode = 3;
    x += 4;
    GetSpriteBlockCollisionAtPosition(yTop, x);
    gSpriteTileInteractionMode = 3;
    y -= 68;
    GetSpriteBlockCollisionAtPosition(y, x);
    gSpriteTileInteractionMode = 3;
    GetSpriteBlockCollisionAtPosition(y, xLeft);
}

void InitToyBlockSquare(void)
{
    struct PrimarySpriteData* sprite;
    struct PrimarySpriteData* temp;
    u8* drawPtr;
    u16 zero16;
    register u8 zero PIN("r2");
    u8 value;
    u16 status;
    u16 x;

    sprite = &gCurrentSprite;
    x = sprite->xPosition;
    x += 32;
    zero16 = 0;
    temp = sprite;
    temp->xPosition = x;
    status = temp->status;
    status |= SPRITE_STATUS_DISABLE_DRAW_DISTANCE;
    zero = 0;
    value = 32;
    status |= value;

    {
        u8* hitboxPtr;
        hitboxPtr = (u8*)sprite;
        *(u16*)hitboxPtr = status;
        ((struct PrimarySpriteData*)hitboxPtr)->drawPriority = 1;

        drawPtr = (u8*)sprite + 32;
        *drawPtr++ = value;
        *drawPtr = zero;
        /* Volatile keeps the increment and byte store as separate target instructions. */
        drawPtr++;
        value = 16;
        *(volatile u8 *)drawPtr = value;

        hitboxPtr += 35;
        *hitboxPtr = 120;
        *((u8*)sprite + 36) = zero;
        hitboxPtr += 2;
        *hitboxPtr = 64;
        /* Volatile keeps the increment and byte store as separate target instructions. */
        hitboxPtr++;
        *(volatile u8 *)hitboxPtr = 60;
    }

    {
        const struct AnimationFrame* oam;
        struct PrimarySpriteData* work;
        oam = sToyBlockSquareOam;
        work = sprite;
        GP_SET(work->pOamData, oam);
        *(u8 *)((u8 *)work + 22) = zero;
        work->animationTimer = zero16;
        work->warioCollision = 22;
        work->pose = value;
    }

    *((u8*)sprite + 40) = zero;
    ToyBlockSquareSetSolidTiles();
}

void ToyBlockSquareIdle(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    func_8023B88();

    if (gSpriteCollisionResult == 0)
    {
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
            func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - sprite->hitboxExtentLeft));
        else
            func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + sprite->hitboxExtentRight));

        if (gSpriteCollisionTileType == 0)
            gCurrentSprite.pose = 27;
    }
}

void StartToyBlockSquareFalling(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    ToyBlockSquareSetPassableTiles();
}

void ToyBlockSquareFalling(void)
{
    int new_var;
    struct PrimarySpriteData* sprite;
    u32 newY;
    u8 index;
    u16 new_var2;
    s16 movement;

    ToyBlockSquareCheckSpriteCollisions();
    gWarioData.unk_06 = 0;
    sprite = &gCurrentSprite;

    if (sprite->work3 == 0xFF)
    {
        sprite->work3 = 0;
        gWarioData.unk_06 = 1;
        return;
    }

    newY = func_8023A60(sprite->yPosition, (*sprite).xPosition);

    if ((double)(gBackgroundCollisionData.waterType == 1))
    {
        sprite->status |= SPRITE_STATUS_UNDERWATER;
        m4aSongNumStart(SE_ENTER_WATER);
    }

    if (gSpriteCollisionResult != 0)
    {
        do
        {
            sprite->yPosition = newY;
            sprite->pose = 16;
        } while (0);

        ToyBlockSquareSetSolidTiles();
        m4aSongNumStart(SE_TOY_BLOCK_ROLL);
        return;
    }

    index = sprite->work3;
    movement = sToyBlockSquareYMovement[index];

    if (movement == 0x7FFF)
    {
        new_var = index - 1;
        new_var2 = ((const u16*)sToyBlockSquareYMovement)[new_var];
        sprite->yPosition += new_var2;
    }
    else
    {
        sprite->work3 = index + 1;
        sprite->yPosition += movement;
    }
}

void SpriteToyBlockSquare(void)
{
    switch (gCurrentSprite.pose)
    {
        case 0:
            InitToyBlockSquare();
        case 16:
            ToyBlockSquareIdle();
            break;
        case 27:
            StartToyBlockSquareFalling();
        case 30:
            ToyBlockSquareFalling();
            break;
        default:
            gCurrentSprite.pose = 16;
            gCurrentSprite.status &= 0xFCFF;
            break;
    }

    func_8026838();
}

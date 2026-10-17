#include "sprite_ai/flying_carpet.h"

#include "global_data.h"
#include "input.h"
#include "sprite_util.h"
#include "wario.h"

volatile long UpdateMagicCarpetCollisionTimer(void)
{
    struct PrimarySpriteData* currentLoad;
    struct PrimarySpriteData* current;
    u8 roomSlot;
    int i;
    u16 status;
    currentLoad = &gCurrentSprite;
    roomSlot = currentLoad->roomSlot;
    i = 0;
    current = currentLoad;
    for (; i < 24; i++) {
        if (gSpriteData[i].globalID == PSPRITE_MAGIC_CARPET) {
            if (gSpriteData[i].status & SPRITE_STATUS_EXISTS) {
                if (roomSlot != gSpriteData[i].roomSlot) {
                    if (gSpriteData[i].work1 != 0) {
                        current->disableWarioCollisionTimer = 1;
                        break;
                    }
                }
            }
        }
    }
    RETURN_NO_VALUE(0);
}

void InitMagicCarpet(void)
{
    struct PrimarySpriteData* sprite;
    const struct AnimationFrame* new_var;
    struct PrimarySpriteData** new_var2;
    new_var2 = &sprite;
    sprite = &gCurrentSprite;
    (*new_var2)->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
    sprite->warioCollision = 29;
    (*new_var2)->drawDistanceDown = 16;
    (*new_var2)->drawDistanceUp = 16;
    sprite->drawDistanceLeftRight = 32;
    (*new_var2)->hitboxExtentUp = 4;
    (*new_var2)->hitboxExtentDown = 64;
    (*new_var2)->hitboxExtentLeft = 68;
    (*new_var2)->hitboxExtentRight = 64;
    new_var = sMagicCarpetIdleOam;
    GP_SET((*new_var2)->pOamData, new_var);
    (*new_var2)->currentAnimationFrame = 0;
    (*new_var2)->animationTimer = 0;
    sprite->pose = 24;
    (*new_var2)->yPosition -= 12;
    sprite->work2 = 0;
    (*new_var2)->work1 = 0;
}

void SetMagicCarpetMovementState(void)
{
    struct PrimarySpriteData* sprite;
    u32 status;
    u8* work2;
    u16 newStatus;

    sprite = &gCurrentSprite;
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    status = sprite->status;
    if (status & SPRITE_STATUS_WARIO_STANDING_ON) {
        GP_SET(sprite->pOamData, sMagicCarpetRiddenOam);
        sprite->pose = 22;
        sprite->work0 = 4;
        if (gWarioData.horizontalDirection & 0x20) {
            sprite->work2 = 1;
            newStatus = ~SPRITE_STATUS_FACING_RIGHT;
            newStatus &= status;
            sprite->status = newStatus;
        } else {
            sprite->work2 = 2;
            newStatus = SPRITE_STATUS_FACING_RIGHT;
            newStatus |= status;
            sprite->status = newStatus;
        }
    } else {
        GP_SET(sprite->pOamData, sMagicCarpetFlyingOam);
        sprite->work0 = 30;
        sprite->pose = 20;
        work2 = &sprite->work2;
        if (*work2 == 0) {
            if (gWarioData.horizontalDirection & 0x20) {
                *work2 = 1;
            } else {
                *work2 = 2;
            }
        }
    }
}

void MagicCarpetWaitForWario(void)
{
    struct PrimarySpriteData* sprite;
    u32 status;
    u16 newStatus;
    u8* work2;

    sprite = &gCurrentSprite;
    status = sprite->status;
    if (status & SPRITE_STATUS_WARIO_STANDING_ON) {
        GP_SET(sprite->pOamData, sMagicCarpetFlyingOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->pose = 16;
        if (gWarioData.horizontalDirection & 0x20) {
            newStatus = ~SPRITE_STATUS_FACING_RIGHT;
            newStatus &= status;
            sprite->status = newStatus;
            work2 = &sprite->work2;
            *work2 = 1;
        } else {
            newStatus = SPRITE_STATUS_FACING_RIGHT;
            newStatus |= status;
            sprite->status = newStatus;
            work2 = &sprite->work2;
            *work2 = 2;
        }
    } else {
        func_8023BFC(sprite->yPosition + 12, sprite->xPosition);
        if (gSpriteCollisionTileType == 0) {
            SetMagicCarpetMovementState();
        }
    }
}

void MagicCarpetHandleStandingWario(void)
{
    struct PrimarySpriteData** new_var;
    struct PrimarySpriteData* sprite;
    u16 status;
    u16 standing;
    u16 buttons;
    u8* work2;
    u16 newStatus;
    u8 zero;
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        SetMagicCarpetMovementState();
    } else {
        sprite = &gCurrentSprite;
        status = sprite->status;
        standing = status & SPRITE_STATUS_WARIO_STANDING_ON;
        if (standing != 0) {
            buttons = gButtonsPressed;
            if (buttons & 0x20) {
                work2 = &sprite->work2;
                *work2 = 1;
                newStatus = ~SPRITE_STATUS_FACING_RIGHT;
                newStatus &= status;
                sprite->status = newStatus;
            } else if (buttons & 0x10) {
                work2 = &sprite->work2;
                *work2 = 2;
                newStatus = SPRITE_STATUS_FACING_RIGHT;
                newStatus += (u16)((u32)buttons * 0x10000u);
                newStatus |= status;
                sprite->status = newStatus;
            }
        } else {
            zero = 0;
            sprite->disableWarioCollisionTimer = 6;
            new_var = &sprite;
            GP_SET((*new_var)->pOamData, sMagicCarpetFlyingOam);
            (*new_var)->currentAnimationFrame = standing;
            sprite->animationTimer = standing;
            sprite->work0 = 36;
            (*new_var)->work3 = zero;
            sprite->pose = 18;
        }
    }
}

void MagicCarpetRiseAndMove(void)
{
    struct PrimarySpriteData* sprite;
    struct PrimarySpriteData** new_var;
    u16 newStatus;
    u32 status;
    u8 timer;
    u8 index;
    s16 velocity;

    sprite = &gCurrentSprite;
    status = sprite->status;
    if (status & SPRITE_STATUS_WARIO_STANDING_ON) {
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        GP_SET(sprite->pOamData, sMagicCarpetRiddenOam);
        sprite->pose = 22;
        sprite->work0 = 4;
        if (gWarioData.horizontalDirection & 0x20) {
            sprite->work2 = 1;
            newStatus = ~SPRITE_STATUS_FACING_RIGHT;
            newStatus &= status;
            sprite->status = newStatus;
        } else {
            sprite->work2 = 2;
            newStatus = SPRITE_STATUS_FACING_RIGHT;
            newStatus |= status;
            sprite->status = newStatus;
        }
    } else {
        timer = --sprite->work0;
        if (timer != 0) {
            func_8023BFC((u16)(sprite->yPosition - 16), sprite->xPosition);
            if ((gSpriteCollisionTileType & 0xF) != 0) {
                SetMagicCarpetMovementState();
            } else {
                func_8023BFC((u16)(sprite->yPosition - 16), (u16)(sprite->xPosition - 64));
                if ((gSpriteCollisionTileType & 0xF) != 0) {
                    SetMagicCarpetMovementState();
                } else {
                    func_8023BFC((u16)(sprite->yPosition - 16), (u16)(sprite->xPosition + 64));
                    if ((gSpriteCollisionTileType & 0xF) != 0) {
                        SetMagicCarpetMovementState();
                    } else {
                        index = sprite->work3;
                        velocity = sMagicCarpetVerticalVelocity[index];
                        if (velocity == 0x7FFF) {
                            const s16* previousVelocity;
                            u16 yPosition;

                            previousVelocity = (const s16*)((u8*)sMagicCarpetVerticalVelocity + ((index - 1) << 1));
                            yPosition = *(volatile u16*)&sprite->yPosition;
                            {
                                s16 previousValue;
                                previousValue = *previousVelocity;
                                yPosition += previousValue;
                            }
                            sprite->yPosition = yPosition;
                        } else {
                            sprite->work3 = index + 1;
                            sprite->yPosition += velocity;
                        }

                        new_var = &sprite;
                        sprite = &gCurrentSprite;
                        if (sprite->work2 == 1) {
                            func_8023BFC((u16)(sprite->yPosition - 32), (u16)(sprite->xPosition - 80));
                            if ((gSpriteCollisionTileType & 0xF) == 0) {
                                sprite->xPosition--;
                                if (sprite->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                                    gWarioData.xPosition -= 3;
                                }
                            }
                        } else if (sprite->work2 == 2) {
                            func_8023BFC((u16)(sprite->yPosition - 32), (u16)((*new_var)->xPosition + 76));
                            if ((gSpriteCollisionTileType & 0xF) == 0) {
                                sprite->xPosition++;
                                if (sprite->status & SPRITE_STATUS_WARIO_STANDING_ON) {
                                    gWarioData.xPosition += 3;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            SetMagicCarpetMovementState();
        }
    }
}

void MagicCarpetMoveUnridden(void)
{
    struct PrimarySpriteData* sprite;
    unsigned int groundY;
    u32 status;
    u16 statusCopy;
    u32 standing;
    u16 y;
    u8* work2;
    struct PrimarySpriteData** new_var;
    u8* work0;
    sprite = &gCurrentSprite;
    groundY = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        sprite->status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult != 0) {
        u8 zeroByte;
        u16 zeroHalf;
        s32 yPosition;
        zeroByte = (u8)(groundY << 8);
        zeroHalf = (u16)(groundY * 0x10000);
        GP_SET(sprite->pOamData, sMagicCarpetIdleOam);
        sprite->currentAnimationFrame = zeroByte;
        sprite->animationTimer = zeroHalf;
        sprite->pose = 24;
        yPosition = groundY;
        yPosition -= 12;
        sprite->yPosition = yPosition;
        return;
    }
    status = sprite->status;
    standing = status & SPRITE_STATUS_WARIO_STANDING_ON;
    statusCopy = status;
    if (standing != 0) {
        u16 newStatus;
        sprite->currentAnimationFrame = 0;
        (*(new_var = &sprite))->animationTimer = 0;
        GP_SET(sprite->pOamData, sMagicCarpetRiddenOam);
        sprite->pose = 22;
        sprite->work0 = 4;
        if (gWarioData.horizontalDirection & 0x20) {
            sprite->work2 = 1;
            newStatus = ~SPRITE_STATUS_FACING_RIGHT;
            newStatus &= statusCopy;
            sprite->status = newStatus;
        } else {
            u8* work2Pointer;
            work2Pointer = &sprite->work2;
            newStatus = 2;
            *work2Pointer = newStatus;
            newStatus <<= 5;
            newStatus |= status;
            sprite->status = newStatus;
        }
        return;
    }
    y = sprite->yPosition;
    sprite->yPosition = y + 4;
    work2 = &sprite->work2;
    if ((*work2) == 1) {
        func_8023BFC((u16)(y - 28), (u16)(sprite->xPosition - 80));
        if ((gSpriteCollisionTileType & 0xF) == 0) {
            sprite->xPosition -= 3;
        } else {
            sprite->work0 = 30;
            *work2 = 2;
        }
    } else {
        func_8023BFC((u16)(y - 28), (u16)(sprite->xPosition + 76));
        if ((gSpriteCollisionTileType & 0xF) == 0) {
            sprite->xPosition += 3;
        } else {
            sprite->work0 = 30;
            *work2 = 1;
        }
    }
    sprite = &gCurrentSprite;
    work0 = &sprite->work0;
    (*work0)--;
    if ((*work0) == 0) {
        *work0 = 30;
        sprite->work2 ^= 3;
    }
}

void MagicCarpetMoveWithWario(void)
{
    struct PrimarySpriteData* sprite;
    u32 groundY;
    u32 status;
    u16 statusCopy;
    u32 standing;
    u16 buttons;
    u32 buttonMask;
    u8* work2;
    struct PrimarySpriteData* loadedSprite;

    sprite = &gCurrentSprite;
    groundY = func_8023A60(sprite->yPosition, sprite->xPosition);
    if (gBackgroundCollisionData.waterType == 1) {
        sprite->status |= SPRITE_STATUS_UNDERWATER;
    }
    if (gSpriteCollisionResult != 0) {
        sprite->yPosition = groundY;
        GP_SET(sprite->pOamData, sMagicCarpetFlyingOam);
        sprite->currentAnimationFrame = 0;
        sprite->animationTimer = 0;
        sprite->pose = 16;
        return;
    }

    sprite->yPosition += 2;
    status = sprite->status;
    standing = status & SPRITE_STATUS_WARIO_STANDING_ON;
    statusCopy = status;
    if (standing != 0) {
        gWarioData.yPosition += 2;
        sprite->work0 = 4;
        buttons = gButtonsHeld;
        buttonMask = 0x20;
        buttonMask &= buttons;
        if (buttonMask != 0) {
            work2 = &sprite->work2;
            *work2 = 1;
            sprite->status = statusCopy & ~SPRITE_STATUS_FACING_RIGHT;
        } else {
            buttonMask = 0x10;
            buttonMask &= buttons;
            if (buttonMask != 0) {
                u32 newStatus;
                work2 = &sprite->work2;
                newStatus = 2;
                *work2 = newStatus;
                newStatus <<= 5;
                newStatus |= status;
                sprite->status = newStatus;
            }
        }

        loadedSprite = &gCurrentSprite;
        status = (u32)&loadedSprite->work2;
        status += (u16)(statusCopy * 0x10000u);
        status = *(u8*)status;
        sprite = loadedSprite;
        if (status == 1) {
            func_8023BFC((u16)(sprite->yPosition - 32), (u16)(sprite->xPosition - 80));
            if ((gSpriteCollisionTileType & 0xF) == 0) {
                sprite->xPosition -= 4;
                gWarioData.xPosition -= 4;
            }
            {
                struct WarioData* wario;
                u16 warioX;
                s32 limit;

                wario = &gWarioData;
                warioX = wario->xPosition;
                limit = (s32)sprite->xPosition - 33;
                if (warioX < limit) {
                    wario->xPosition = limit;
                }
            }
        } else if (status == 2) {
            func_8023BFC((u16)(sprite->yPosition - 32), (u16)(sprite->xPosition + 76));
            if ((gSpriteCollisionTileType & 0xF) == 0) {
                sprite->xPosition += 4;
                gWarioData.xPosition += 4;
            }
            {
                struct WarioData* wario;
                u16 warioX;
                s32 limit;

                wario = &gWarioData;
                warioX = wario->xPosition;
                limit = (s32)sprite->xPosition + 33;
                if (warioX > limit) {
                    wario->xPosition = limit;
                }
            }
        }
    } else {
        u8* work0;
        u32 value;

        work0 = &sprite->work0;
        value = *work0;
        value--;
        *work0 = value;
    }

    {
        u8* work0;
        u32 timer;
        u32 zero;
        u8* work3;

        sprite = &gCurrentSprite;
        work0 = &sprite->work0;
        timer = *work0;
        if (timer == 0) {
            GP_SET(sprite->pOamData, sMagicCarpetFlyingOam);
            sprite->currentAnimationFrame = timer;
            zero = 0;
            sprite->animationTimer = timer;
            *work0 = 36;
            work3 = &sprite->work3;
            *work3 = zero;
            sprite->pose = 18;
        }
    }
}

void SpriteMagicCarpet(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_UNDERWATER) {
        sprite->pose = 107;
    }

    switch (sprite->pose) {
        case 0:
            InitMagicCarpet();
            break;
        case 24:
            MagicCarpetWaitForWario();
            break;
        case 16:
            MagicCarpetHandleStandingWario();
            break;
        case 18:
            MagicCarpetRiseAndMove();
            break;
        case 20:
            MagicCarpetMoveUnridden();
            break;
        case 22:
            MagicCarpetMoveWithWario();
            break;
        default:
            SpriteUtilDie();
            gWarioData.unk_1A = 0;
            break;
    }

    func_8026838();
    UpdateMagicCarpetCollisionTimer();
    sprite = &gCurrentSprite;
    if (sprite->work1 != 0) {
        sprite->status &= ~SPRITE_STATUS_BACKGROUND;
    } else {
        sprite->status |= SPRITE_STATUS_BACKGROUND;
    }
}

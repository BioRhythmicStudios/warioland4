#include "global_data.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

extern const struct AnimationFrame sDeburinaChildIdleOam[];
extern const struct AnimationFrame sDeburinaIdleOam[];
extern const struct AnimationFrame sDeburinaTurnOam[];
extern const struct AnimationFrame sDeburinaPushedOam[];
extern const struct AnimationFrame sDeburinaStunnedOam[];
extern const struct AnimationFrame sDeburinaHopOam[];
extern const struct AnimationFrame sDeburinaDefeatedOam[];
extern const struct AnimationFrame sDeburinaTumbleOam[];
extern const struct AnimationFrame sDeburinaSpawnOam[];
extern const struct AnimationFrame sDeburinaAlertOam[];
extern const struct AnimationFrame sDeburinaLeapWindupOam[];
extern const struct AnimationFrame sDeburinaCarriedOam[];
extern const struct AnimationFrame sDeburinaAirborneOam[];

void SpawnEnemyScoreDrop(u32 globalID, u32 yPosition, u32 xPosition, u32 effect);
void SpawnSecondarySprite(u32 yPosition, u32 xPosition, u32 type);

void InitDeburinaAlert(void)
{
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    sprite->status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION | SPRITE_STATUS_BACKGROUND;
    sprite->warioCollision = 0;
    sprite->drawDistanceDown = 40;
    sprite->drawDistanceUp = 8;
    sprite->drawDistanceLeftRight = 32;
    sprite->hitboxExtentUp = 128;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 32;
    sprite->hitboxExtentRight = 28;
    GP_SET(sprite->pOamData, sDeburinaSpawnOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = 111;
    sprite->yPosition += 32;
    sprite->xPosition -= 4;
}

void UpdateDeburinaAlertWait(void)
{
    if (SpriteUtilCheckWarioNearbyLeftRight(200, 200) != 0) {
        GP_SET(gCurrentSprite.pOamData, sDeburinaAlertOam);
        gCurrentSprite.currentAnimationFrame = 0;
        gCurrentSprite.animationTimer = 0;
        gCurrentSprite.pose = 113;
        gCurrentSprite.work0 = 50;
        m4aSongNumStart(SOUND_6E);
    }
}

void StartDeburinaLeapWindup(void)
{
    struct PrimarySpriteData *sprite;
    u8 *timer;
    s16 timerValue;
    register struct PrimarySpriteData *savedSprite PIN("r12");
    register u32 zero PIN("r4");
    u32 one;

    sprite = &gCurrentSprite;
    {
        u32 decrementedTimer;

        timer = &sprite->work0;
        decrementedTimer = *timer;
        decrementedTimer--;
        *timer = decrementedTimer;
        timerValue = (u8)decrementedTimer;
        savedSprite = sprite;
    }
    if (timerValue == 0) {
        register u16 status PIN("r0");
        register u16 currentStatus PIN("r1");

        one = 1;
        sprite->warioCollision = one;
        currentStatus = sprite->status;
        status = currentStatus & ~SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
        zero = 0;
        status &= ~SPRITE_STATUS_BACKGROUND;
        sprite = savedSprite;
        sprite->status = status;
        GP_SET(sprite->pOamData, sDeburinaLeapWindupOam);
        sprite->currentAnimationFrame = zero;
        sprite->animationTimer = timerValue;
        sprite->pose = 115;
        *timer = 48;
        {
            struct WarioData *wario;
            register struct PrimarySpriteData *compareSprite PIN("r2");
            u16 currentX;

            wario = &gWarioData;
            compareSprite = savedSprite;
            currentX = compareSprite->xPosition;
            if (currentX > wario->xPosition) {
                savedSprite->work2 = zero;
            } else {
                savedSprite->work2 = one;
            }
        }
        {
            register struct PrimarySpriteData *spawnSprite PIN("r4");

            spawnSprite = savedSprite;
            SpriteSpawnAsChild(PSPRITE_DEBURINA_CHILD, spawnSprite->roomSlot, spawnSprite->gfxSlot,
                               spawnSprite->yPosition, spawnSprite->xPosition);
        }
    }
}

void UpdateDeburinaLeap(void)
{
    struct PrimarySpriteData *sprite;
    register u32 timerValue PIN("r1");

    sprite = &gCurrentSprite;
    {
        u8 *timer;
        register u32 decrementedTimer PIN("r0");

        timer = &sprite->work0;
        decrementedTimer = *timer;
        decrementedTimer--;
        *timer = decrementedTimer;
        timerValue = (u8)decrementedTimer;
    }
    if (timerValue != 0) {
        if (sprite->work2 != 0) {
            sprite->xPosition++;
        } else {
            sprite->xPosition--;
        }
        func_80238A4();
        func_8023B88();
        if (gSpriteCollisionResult == 0) {
            gCurrentSprite.pose = 27;
        }
    } else {
        GP_SET(sprite->pOamData, sDeburinaIdleOam);
        sprite->currentAnimationFrame = timerValue;
        sprite->animationTimer = timerValue;
        sprite->pose = 16;
        SpriteUtilTurnTowardWario();
    }
}

void InitDeburinaIdle(void)
{
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    GP_SET(gCurrentSprite.pOamData, sDeburinaIdleOam);
    gCurrentSprite.pose = 16;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentUp = 128;
}

void UpdateDeburinaIdle(void)
{
    struct PrimarySpriteData *sprite;
    u32 animationState;

    sprite = &gCurrentSprite;
    animationState = *((u32 *)&sprite->animationTimer) & 0xFFFFFF;
    if (animationState == 0x10002 && (sprite->status & SPRITE_STATUS_ONSCREEN) != 0) {
        m4aSongNumStart(SOUND_6F);
    }

    func_80238A4();
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
            gCurrentSprite.pose = 27;
            return;
        }
    } else {
        if (gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) {
            if ((gSpriteCollisionResult & 0xF0) != 0) {
                u16 xPosition;
                xPosition = gCurrentSprite.xPosition;
                if (((xPosition & 0x3F) + gCurrentSprite.hitboxExtentRight) > 63) {
                    func_8023BFC(gCurrentSprite.yPosition,
                                 (u16)(xPosition + gCurrentSprite.hitboxExtentRight));
                    if ((gSpriteCollisionTileType & 0xF0) != 0) {
                        func_8023BFC((u16)(gCurrentSprite.yPosition - 32),
                                     (u16)(gCurrentSprite.xPosition + gCurrentSprite.hitboxExtentRight));
                        if ((gSpriteCollisionTileType & 0xF) != 0) {
                            gCurrentSprite.pose = 17;
                            return;
                        }
                    } else {
                        gCurrentSprite.pose = 17;
                        return;
                    }
                }
            }
        } else {
            if ((gSpriteCollisionResult & 0xF0) != 0) {
                u16 xPosition;
                xPosition = gCurrentSprite.xPosition;
                if ((xPosition & 0x3F) < gCurrentSprite.hitboxExtentLeft) {
                    func_8023BFC(gCurrentSprite.yPosition,
                                 (u16)(xPosition - gCurrentSprite.hitboxExtentLeft));
                    if ((gSpriteCollisionTileType & 0xF0) != 0) {
                        func_8023BFC((u16)(gCurrentSprite.yPosition - 32),
                                     (u16)(gCurrentSprite.xPosition - gCurrentSprite.hitboxExtentLeft));
                        if ((gSpriteCollisionTileType & 0xF) != 0) {
                            gCurrentSprite.pose = 17;
                            return;
                        }
                    } else {
                        gCurrentSprite.pose = 17;
                        return;
                    }
                }
            }
        }
    }
    func_80263AC();
}

void InitDeburinaTurn(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaTurnOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 18;
    gCurrentSprite.work0 = 40;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentUp = 128;
}

void UpdateDeburinaTurn(void)
{
    func_80238A4();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 27;
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            gCurrentSprite.status ^= SPRITE_STATUS_FACING_RIGHT;
            gCurrentSprite.pose = 15;
        }
    }
}

void InitDeburinaHopBack(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaTurnOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 24;
    gCurrentSprite.work0 = 40;
    gCurrentSprite.warioCollision = 1;
    gCurrentSprite.hitboxExtentUp = 128;
}

void InitDeburinaHopStart(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaHopOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 26;
    gCurrentSprite.work0 = 8;
}

void UpdateDeburinaHopStart(void)
{
    func_80238E8();
    func_8023B88();
    if (gSpriteCollisionResult == 0) {
        gCurrentSprite.pose = 29;
    } else {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            if (gCurrentSprite.xPosition > gWarioData.xPosition) {
                if ((gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) == 0) {
                    gCurrentSprite.pose = 23;
                } else {
                    gCurrentSprite.pose = 17;
                }
            } else {
                if ((gCurrentSprite.status & SPRITE_STATUS_FACING_RIGHT) != 0) {
                    gCurrentSprite.pose = 23;
                } else {
                    gCurrentSprite.pose = 17;
                }
            }
        }
    }
}

void InitDeburinaFalling(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaAirborneOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 28;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
}

void InitDeburinaStunned(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentUp = 96;
}

void InitDeburinaTumbleCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaTumbleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void InitDeburinaTumbleRight(void)
{
    gCurrentSprite.pose = 32;
    InitDeburinaTumbleCommon();
}

void InitDeburinaTumbleLeft(void)
{
    gCurrentSprite.pose = 34;
    InitDeburinaTumbleCommon();
}

void InitDeburinaShortTumbleCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaTumbleOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.health = 0;
    gCurrentSprite.work2 = 7;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.drawPriority = 0;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void InitDeburinaShortTumbleRight(void)
{
    gCurrentSprite.pose = 32;
    InitDeburinaShortTumbleCommon();
}

void InitDeburinaShortTumbleLeft(void)
{
    gCurrentSprite.pose = 34;
    InitDeburinaShortTumbleCommon();
}

void InitDeburinaPushedWindupCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaPushedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 12;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.warioCollision = 1;
}

void InitDeburinaPushedRightWindup(void)
{
    gCurrentSprite.pose = 36;
    InitDeburinaPushedWindupCommon();
}

void InitDeburinaPushedLeftWindup(void)
{
    gCurrentSprite.pose = 38;
    InitDeburinaPushedWindupCommon();
}

void InitDeburinaPushedSlideCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaStunnedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work1 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentUp = 96;
}

void InitDeburinaBumpedRight(void)
{
    gCurrentSprite.pose = 44;
    InitDeburinaPushedSlideCommon();
}

void InitDeburinaBumpedLeft(void)
{
    gCurrentSprite.pose = 46;
    InitDeburinaPushedSlideCommon();
}

void InitDeburinaDefeated(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaDefeatedOam);
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.work0 = 11;
    gCurrentSprite.pose = 50;
    gCurrentSprite.status = (gCurrentSprite.status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES) |
                            SPRITE_STATUS_MAYBE_DEAD;
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void KillDeburina(void)
{
    SpawnEnemyScoreDrop(gCurrentSprite.globalID, gCurrentSprite.yPosition, gCurrentSprite.xPosition, 5);
    SpriteUtilDie();
    gPersistentSpriteData[gCurrentRoom][gCurrentSprite.roomSlot] = PERSISTENT_STATUS_DESPAWNED;
}

void InitDeburinaAirBounceCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work0 = 4;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    if (gCurrentSprite.warioCollision != 5) {
        gCurrentSprite.warioCollision = 5;
        gCurrentSprite.hitboxExtentUp = 96;
    }
}

void InitDeburinaAirBounceRight(void)
{
    gCurrentSprite.pose = 52;
    InitDeburinaAirBounceCommon();
}

void InitDeburinaAirBounceLeft(void)
{
    gCurrentSprite.pose = 54;
    InitDeburinaAirBounceCommon();
}

void InitDeburinaGroundBounceCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaStunnedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 5;
    gCurrentSprite.hitboxExtentUp = 96;
}

void InitDeburinaGroundBounceRight(void)
{
    gCurrentSprite.pose = 72;
    InitDeburinaGroundBounceCommon();
}

void InitDeburinaGroundBounceLeft(void)
{
    gCurrentSprite.pose = 74;
    InitDeburinaGroundBounceCommon();
}

void InitDeburinaThrownCrash(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaAirborneOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
    gCurrentSprite.pose = 76;
    gCurrentSprite.work0 = 40;
    gCurrentSprite.work3 = 0;
    gCurrentSprite.warioCollision = 1;
}

void UpdateDeburinaThrownCrash(void)
{
    struct PrimarySpriteData *base;
    u8 *indexPtr;
    int index;
    const s16 *table;
    const s16 *entry;
    u16 yVelocity;
    int signedVelocity;
    int threshold;
    struct PrimarySpriteData *sprite;
    int zero;
    register struct PrimarySpriteData *currentSprite PIN("r4");
    u8 *timer;

    base = &gCurrentSprite;
    indexPtr = &base->work3;
    index = *indexPtr;
    table = sUnk_8352B18;
    entry = (const s16 *)((u32)(index * sizeof(s16)) + (u32)table);
    yVelocity = *(const u16 *)entry;
    zero = 0;
    signedVelocity = entry[zero];
    threshold = S16_MAX;
    sprite = base;

    {
        register u16 newYPosition PIN("r0");

        if (signedVelocity == threshold) {
            int previousIndexRegister;
            const u16 *previousEntry;
            u32 previousVelocity;

            previousIndexRegister = index - 1;
            previousIndexRegister <<= 1;
            previousEntry = (const u16 *)((u32)previousIndexRegister + (u32)table);
            newYPosition = ((volatile struct PrimarySpriteData *)sprite)->yPosition;
            previousVelocity = *previousEntry;
            newYPosition = newYPosition + previousVelocity;
        } else {
            int nextIndex;
            u8 *storePtr;

            nextIndex = index + 1;
            storePtr = indexPtr;
            *storePtr = nextIndex;
            newYPosition = sprite->yPosition + yVelocity;
        }
        sprite->yPosition = newYPosition;
    }

    currentSprite = sprite;
    func_8023BFC(currentSprite->yPosition - currentSprite->hitboxExtentUp,
                 currentSprite->xPosition);
    if ((gSpriteCollisionTileType & 0xF) != 0) {
        currentSprite->pose = 29;
    }
    timer = &sprite->work0;
    (*timer)--;
    if (*timer == 0) {
        sprite->pose = 27;
    }
}

void InitDeburinaLiftedCommon(void)
{
    GP_SET(gCurrentSprite.pOamData, sDeburinaCarriedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitDeburinaLiftedRight(void)
{
    gCurrentSprite.pose = 82;
    InitDeburinaLiftedCommon();
}

void InitDeburinaLiftedLeft(void)
{
    gCurrentSprite.pose = 84;
    InitDeburinaLiftedCommon();
}

void InitDeburinaCarriedCommon(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    GP_SET(gCurrentSprite.pOamData, sDeburinaCarriedOam);
    gCurrentSprite.animationTimer = 0;
    gCurrentSprite.currentAnimationFrame = 0;
}

void InitDeburinaCarriedRight(void)
{
    gCurrentSprite.pose = 88;
    InitDeburinaCarriedCommon();
}

void InitDeburinaCarriedLeft(void)
{
    gCurrentSprite.pose = 86;
    InitDeburinaCarriedCommon();
}

void InitDeburinaChild(void)
{
    struct PrimarySpriteData *sprite;
    u16 statusValue;
    register u16 finalStatus PIN("r0");
    u8 zeroByte;
    u16 zeroHalf;

    sprite = &gCurrentSprite;
    {
        register u16 status PIN("r1");
        u16 flags;
        register u16 flagsCopy PIN("r0");
        status = sprite->status;
        flags = SPRITE_STATUS_IGNORE_SPRITE_COLLISION;
        flagsCopy = flags;
        /* Keep the zero values scheduled before the status update, as in the original. */
        {
            register u8 zeroTemp PIN("r2");
            zeroTemp = 0;
            zeroByte = zeroTemp;
        }
        zeroHalf = 0;
        statusValue = flagsCopy | status;
    }
    finalStatus = statusValue & ~SPRITE_STATUS_HIDDEN;
    sprite->status = finalStatus;
    sprite->warioCollision = 8;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = zeroByte;
    sprite->drawDistanceLeftRight = 24;
    sprite->hitboxExtentUp = 64;
    sprite->hitboxExtentDown = 168;
    sprite->hitboxExtentLeft = 48;
    sprite->hitboxExtentRight = 44;
    GP_SET(sprite->pOamData, sDeburinaChildIdleOam);
    sprite->currentAnimationFrame = zeroByte;
    sprite->animationTimer = zeroHalf;
    sprite->pose = 16;
}

void UpdateDeburinaChild(void)
{
    s32 parentSlot;
    struct PrimarySpriteData *spriteData;
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *parent;
    register u32 parentExists PIN("r0");
    u16 parentStatus;
    u32 parentOffset;

    parentSlot = SpriteUtilFindParentSlotOrU8Max(PSPRITE_DEBURINA);
    if (parentSlot == U8_MAX) {
        gCurrentSprite.status = 0;
        return;
    }

    spriteData = gSpriteData;
    parentOffset = sizeof(*spriteData) * parentSlot;
    parent = (struct PrimarySpriteData *)(parentOffset + (u32)spriteData);
    parentStatus = parent->status;
    parentExists = SPRITE_STATUS_EXISTS;
    parentExists &= parentStatus;
    sprites = spriteData;
    if (parentExists == 0) {
        gCurrentSprite.status = 0;
        return;
    }

    {
        struct PrimarySpriteData *destination;

        switch (parent->pose) {
            case 15:
            case 16:
            case 17:
            case 18:
            case 23:
            case 24:
            case 27:
            case 28:
            case 75:
            case 76:
            case 115:
                {
                    register struct PrimarySpriteData *currentSprite PIN("r0");
                    register u16 currentStatus PIN("r2");
                    u16 visibleStatus;

                    currentSprite = &gCurrentSprite;
                    currentStatus = currentSprite->status;
                    visibleStatus = currentStatus & ~SPRITE_STATUS_HIDDEN;
                    currentSprite->status = visibleStatus;
                    destination = currentSprite;
                }
                break;
            default:
                {
                    struct PrimarySpriteData *currentSprite;
                    u16 currentStatus;
                    u32 hiddenStatus;

                    currentSprite = &gCurrentSprite;
                    currentSprite->disableWarioCollisionTimer = 2;
                    currentStatus = currentSprite->status;
                    hiddenStatus = SPRITE_STATUS_HIDDEN;
                    hiddenStatus |= currentStatus;
                    currentSprite->status = hiddenStatus;
                    destination = currentSprite;
                }
                break;
        }

        {
            register struct PrimarySpriteData *positionParent PIN("r0");

            positionParent = (struct PrimarySpriteData *)((u32)(sizeof(*sprites) * parentSlot) + (u32)sprites);
            destination->yPosition = positionParent->yPosition;
            destination->xPosition = positionParent->xPosition;
        }
    }
}

void SpriteDeburina(void)
{
    if (gCurrentSprite.status & SPRITE_STATUS_UNDERWATER) {
        gCurrentSprite.pose = 107;
    }

    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitDeburinaAlert();
            break;
        case 111:
            UpdateDeburinaAlertWait();
            break;
        case 113:
            StartDeburinaLeapWindup();
            break;
        case 115:
            UpdateDeburinaLeap();
            break;
        case SPOSE_IDLE_INIT:
            InitDeburinaIdle();
            /* fallthrough */
        case SPOSE_IDLE:
            UpdateDeburinaIdle();
            break;
        case 17:
            InitDeburinaTurn();
            /* fallthrough */
        case 18:
            UpdateDeburinaTurn();
            break;
        case 23:
            InitDeburinaHopBack();
            /* fallthrough */
        case 24:
            func_8023C94();
            break;
        case 25:
            InitDeburinaHopStart();
            /* fallthrough */
        case 26:
            UpdateDeburinaHopStart();
            break;
        case 27:
            InitDeburinaFalling();
            /* fallthrough */
        case 28:
            func_8023D48();
            break;
        case 29:
            InitDeburinaStunned();
            /* fallthrough */
        case 30:
            func_8023EE0();
            break;
        case 31:
        case 79:
            InitDeburinaTumbleRight();
            /* fallthrough */
        case 32:
            SpriteUtilFallOffscreenRight();
            break;
        case 33:
        case 80:
            InitDeburinaTumbleLeft();
            /* fallthrough */
        case 34:
            SpriteUtilFallOffscreenLeft();
            break;
        case 35:
            InitDeburinaPushedRightWindup();
            /* fallthrough */
        case 36:
            SpriteUtilPushedRight();
            break;
        case 37:
            InitDeburinaPushedLeftWindup();
            /* fallthrough */
        case 38:
            SpriteUtilPushedLeft();
            break;
        case 43:
            InitDeburinaBumpedRight();
            /* fallthrough */
        case 44:
            func_8024478();
            break;
        case 45:
            InitDeburinaBumpedLeft();
            /* fallthrough */
        case 46:
            func_802449C();
            break;
        case 47:
            func_80244C0();
            /* fallthrough */
        case 48:
            func_80244E0();
            break;
        case 49:
            InitDeburinaDefeated();
            /* fallthrough */
        case 50:
            SpriteUtilDieAfterDelay();
            break;
        case 51:
            InitDeburinaAirBounceRight();
            /* fallthrough */
        case 52:
        case 64:
            func_8024988();
            break;
        case 53:
            InitDeburinaAirBounceLeft();
            /* fallthrough */
        case 54:
        case 58:
            func_802476C();
            break;
        case 55:
            func_8024688();
            /* fallthrough */
        case 56:
            func_80246B8();
            break;
        case 57:
            func_802473C();
            func_802476C();
            break;
        case 59:
            func_80247F0();
            /* fallthrough */
        case 60:
            func_8024820();
            break;
        case 61:
            func_80248A4();
            /* fallthrough */
        case 62:
            func_80248D4();
            break;
        case 63:
            func_8024958();
            func_8024988();
            break;
        case 65:
            func_8024A0C();
            /* fallthrough */
        case 66:
            func_8024A3C();
            break;
        case 67:
            func_8024AC0();
            /* fallthrough */
        case 68:
        case 72:
            func_8024AD4();
            break;
        case 69:
            func_8024BEC();
            /* fallthrough */
        case 70:
        case 74:
            func_8024C00();
            break;
        case 71:
            InitDeburinaGroundBounceRight();
            func_8024AD4();
            break;
        case 73:
            InitDeburinaGroundBounceLeft();
            func_8024C00();
            break;
        case 75:
            InitDeburinaThrownCrash();
            /* fallthrough */
        case 76:
            UpdateDeburinaThrownCrash();
            break;
        case 81:
            InitDeburinaLiftedRight();
            break;
        case 82:
            SpriteUtilLiftingSpriteRight();
            break;
        case 83:
            InitDeburinaLiftedLeft();
            break;
        case 84:
            SpriteUtilLiftingSpriteLeft();
            break;
        case 87:
            InitDeburinaCarriedRight();
            break;
        case 88:
            SpriteUtilCarryingSpriteRight();
            break;
        case 85:
            InitDeburinaCarriedLeft();
            break;
        case 86:
            SpriteUtilCarryingSpriteLeft();
            break;
        case 89:
            gCurrentSprite.pose = 90;
            /* fallthrough */
        case 90:
            SpriteUtilThrownLeftSoft();
            break;
        case 93:
            gCurrentSprite.pose = 94;
            /* fallthrough */
        case 94:
            SpriteUtilThrownLeftHard();
            break;
        case 97:
            gCurrentSprite.pose = 98;
            /* fallthrough */
        case 98:
            SpriteUtilThrownUpLeftSoft();
            break;
        case 101:
            gCurrentSprite.pose = 102;
            /* fallthrough */
        case 102:
            SpriteUtilThrownUpLeftHard();
            break;
        case 91:
            gCurrentSprite.pose = 92;
            /* fallthrough */
        case 92:
            SpriteUtilThrownRightSoft();
            break;
        case 95:
            gCurrentSprite.pose = 96;
            /* fallthrough */
        case 96:
            SpriteUtilThrownRightHard();
            break;
        case 99:
            gCurrentSprite.pose = 100;
            /* fallthrough */
        case 100:
            SpriteUtilThrownUpRightSoft();
            break;
        case 103:
            gCurrentSprite.pose = 104;
            /* fallthrough */
        case 104:
            SpriteUtilThrownUpRightHard();
            break;
        case 105:
            InitDeburinaShortTumbleLeft();
            break;
        case 106:
            InitDeburinaShortTumbleRight();
            break;
        default:
            KillDeburina();
            break;
    }
}

void SpriteDeburinaChild(void)
{
    if (gCurrentSprite.pose == SPOSE_INIT) {
        InitDeburinaChild();
    }
    UpdateDeburinaChild();
}

#include "sprite_ai/ghost.h"

#include "global_data.h"
#include "gba.h"
#include "sound.h"
#include "score.h"
#include "sprite_ai/keyzer.h"
#include "sprite.h"
#include "sprite_collision.h"
#include "sprite_util.h"
#include "wario.h"

void func_803F214(void)
{
    u32 targetId;
    s32 i;
    u8 *base;
    u8 *entry;
    s32 offset;
    u8 *idBase;
    u8 *persistentData;
    u8 *room;

    targetId = 17;
    i = 0;
    base = gRoomSpriteData;
    if (*base != 0xFF) {
        persistentData = &gPersistentSpriteData[0][0];
        entry = base;
        offset = 0;
        idBase = entry + 2;
        room = &gCurrentRoom;
        do {
            if (*(u8 *)((u32)offset + (u32)idBase) == targetId) {
                u32 destination;
                destination = i + ((*room) << 6);
                destination += (u32)persistentData;
                base = (u8 *)17;
                *(u8 *)destination = (u8)(u32)base;
            }
            entry += 3;
            offset += 3;
            i++;
        } while (i <= 63 && *entry != 0xFF);
    }
}

void func_803F264(void)
{
    u32 targetId;
    s32 i;
    u8 *base;
    u8 *entry;
    s32 offset;
    u8 *idBase;
    u8 *persistentData;
    u8 *room;

    targetId = 17;
    i = 0;
    base = gRoomSpriteData;
    if (*base != 0xFF) {
        persistentData = &gPersistentSpriteData[0][0];
        entry = base;
        offset = 0;
        idBase = entry + 2;
        room = &gCurrentRoom;
        do {
            if (*(u8 *)((u32)offset + (u32)idBase) == targetId) {
                u32 destination;
                destination = i + ((*room) << 6);
                destination += (u32)persistentData;
                base = (u8 *)1;
                *(u8 *)destination = (u8)(u32)base;
            }
            entry += 3;
            offset += 3;
            i++;
        } while (i <= 63 && *entry != 0xFF);
    }
}

void func_803F2B4(void)
{
    u8 *persistentData;
    struct PrimarySpriteData *sprite;
    s32 distanceLimit;
    s32 distance;
    u16 spriteY;
    u16 adjustedSpriteY;
    u16 adjustedWarioY;

    persistentData = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    {
        u32 address;
        u32 persistentStatus;

        address = gCurrentRoom << 6;
        address += sprite->roomSlot;
        address += (u32)persistentData;
        persistentStatus = *(u8 *)address;
        distanceLimit = 276;
        if (persistentStatus == 17) {
            distanceLimit += 100;
        }
    }

    spriteY = sprite->yPosition;
    adjustedSpriteY = spriteY + 0x1000;
    adjustedWarioY = gWarioData.yPosition + 0x1000;

    if (adjustedSpriteY < adjustedWarioY) {
        distance = adjustedWarioY - adjustedSpriteY;
        if (distance > distanceLimit) {
            sprite->yPosition = spriteY + 5;
        } else {
            s32 lowerLimit;
            lowerLimit = distanceLimit;
            lowerLimit -= 10;
            if (distance < lowerLimit) {
                sprite->yPosition = spriteY - 1;
            }
        }
    } else {
        sprite->yPosition = spriteY - 8;
    }
}

u32 func_803F320(void)
{
    u8 *persistentData;
    struct PrimarySpriteData *loadCurrent;
    struct PrimarySpriteData *current;
    u8 *room;
    u32 persistentAddress;

    persistentData = &gPersistentSpriteData[0][0];
    loadCurrent = &gCurrentSprite;
    room = &gCurrentRoom;
    persistentAddress = *room;
    persistentAddress <<= 6;
    persistentAddress += loadCurrent->roomSlot;
    persistentAddress += (u32)persistentData;
    {
        u32 persistentState;

        persistentState = *(u8 *)persistentAddress;
        current = loadCurrent;
        if (persistentState == 17) {
            return 0;
        }
    }

    {
        u32 collectedKeyzer;

        collectedKeyzer = gCollectedKeyzer;
        if (collectedKeyzer == 1) {
        u8 *work1;
        u32 warioX;
        u32 positionOffset;
        u32 spriteX;

        work1 = &current->work1;
        *work1 = collectedKeyzer;
        warioX = gWarioData.xPosition;
        positionOffset = 128;
        positionOffset <<= 5;
        warioX += positionOffset;
        spriteX = current->xPosition;
        spriteX += positionOffset;
        if ((s32)warioX > (s32)spriteX) {
            if ((current->status & 0x40) != 0) {
                goto setPose;
            }
            goto callTransition;
        }
        {
            u32 lowerStatus;
            u32 lowerMask;

            lowerStatus = *(volatile u16 *)&current->status;
            lowerMask = 0x40;
            lowerMask &= lowerStatus;
            if (lowerMask != 0) {
                goto callTransition;
            }
        }
        goto setPose;
        }
    }

    {
        register u32 selectedIndex PIN("r6");
        u32 selectedId;
        s32 index;
        struct PrimarySpriteData *sprites;
        struct PrimarySpriteData *sprite;

        selectedIndex = 0;
        selectedId = 0;
        index = 0;
        sprites = gSpriteData;
        sprite = sprites;
scanLoop:
        if ((sprite->status & 1) != 0) {
            u32 globalId;
            u32 range;

            globalId = sprite->globalID;
            range = (u8)(globalId + 0x7F);
            if (range <= 4 && globalId >= selectedId) {
                selectedIndex = index;
                ASM_BARRIER(::: "memory");
                selectedId = sprite->globalID;
            }
        }
        sprite++;
        index++;
        if (index <= 23) {
            goto scanLoop;
        }
        if (selectedId == 0) {
            return 0;
        }

        {
            register struct PrimarySpriteData *currentAgain PIN("r3");
            u8 *work1;
            u32 one;
            u32 structSize;
            struct PrimarySpriteData *selected;
            u32 selectedX;
            u32 positionOffset;
            u32 currentX;

            currentAgain = current;
            work1 = &currentAgain->work1;
            one = 1;
            *work1 = one;
            structSize = sizeof(struct PrimarySpriteData);
            structSize *= selectedIndex;
            selected = (struct PrimarySpriteData *)((u32)structSize + (u32)sprites);
            selectedX = selected->xPosition;
            positionOffset = 128;
            positionOffset <<= 5;
            selectedX += positionOffset;
            currentX = currentAgain->xPosition;
            currentX += positionOffset;
            if ((s32)selectedX > (s32)currentX) {
                if ((currentAgain->status & 0x40) != 0) {
                    currentAgain->pose = 110;
                    goto done;
                }
                goto callTransition;
            }
            {
                u32 lowerStatus;
                u32 lowerMask;

                lowerStatus = *(volatile u16 *)&currentAgain->status;
                lowerMask = 0x40;
                lowerMask &= lowerStatus;
                if (lowerMask != 0) {
                    goto callTransition;
                }
            }
            goto setPose;
        }
    }

callTransition:
    func_803F944();
    goto done;

setPose:
    current->pose = 110;

done:
    return 1;
}

void func_803F414(void)
{
    u16 currentY;
    u16 currentX;
    u16 currentTop;
    u16 currentBottom;
    u16 currentLeft;
    u16 currentRight;
    s32 i;

    currentY = gCurrentSprite.yPosition + 0x1000;
    currentX = gCurrentSprite.xPosition + 0x1000;
    currentTop = currentY - gCurrentSprite.hitboxExtentUp;
    currentBottom = currentY + gCurrentSprite.hitboxExtentDown;
    currentLeft = currentX - gCurrentSprite.hitboxExtentLeft;
    currentRight = currentX + gCurrentSprite.hitboxExtentRight;

    for (i = 0; i <= 23; i++) {
        struct PrimarySpriteData *sprite;
        u16 spriteY;
        u16 spriteX;
        u16 spriteTop;
        u16 spriteBottom;
        u16 spriteLeft;
        u16 spriteRight;

        {
            struct PrimarySpriteData *sprites;
            u32 offset;

            sprites = gSpriteData;
            offset = sizeof(struct PrimarySpriteData);
            offset *= i;
            sprite = (struct PrimarySpriteData *)(offset + (u32)sprites);
        }
        if (!(sprite->status & 1)) {
            continue;
        }
        if ((u8)(sprite->globalID + 0x7F) > 4) {
            continue;
        }

        spriteY = sprite->yPosition + 0x1000;
        spriteX = sprite->xPosition + 0x1000;
        spriteTop = spriteY - sprite->hitboxExtentUp;
        spriteBottom = spriteY + sprite->hitboxExtentDown;
        spriteLeft = spriteX - sprite->hitboxExtentLeft;
        spriteRight = spriteX + sprite->hitboxExtentRight;

        if (!SpriteCollisionCheckObjectsTouching(
                currentTop, currentBottom, currentLeft, currentRight,
                spriteTop, spriteBottom, spriteLeft, spriteRight)) {
            continue;
        }

        sprite->status = 0;
        m4aSongNumStart(SOUND_5E);
        if (GP(gCurrentSprite.pOamData) != sGhostOam_83C53C4) {
            GP_SET(gCurrentSprite.pOamData, sGhostOam_83C53C4);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
        }
    }
}

void func_803F528(void)
{
    u16 currentY;
    u16 currentX;
    u16 currentTop;
    u16 currentBottom;
    u16 currentLeft;
    u16 currentRight;
    s32 i;

    if (gCurrentSprite.work0 != 0) {
        gCurrentSprite.work0--;
        return;
    }

    currentY = gCurrentSprite.yPosition + 0x1000;
    currentX = gCurrentSprite.xPosition + 0x1000;
    currentTop = currentY - gCurrentSprite.hitboxExtentUp;
    currentBottom = currentY + gCurrentSprite.hitboxExtentDown;
    currentLeft = currentX - gCurrentSprite.hitboxExtentLeft;
    currentRight = currentX + gCurrentSprite.hitboxExtentRight;

    for (i = 0; i <= 7; i++) {
        struct SecondarySprite *sprite;
        u16 spriteY;
        u16 spriteX;
        u16 spriteTop;
        u16 spriteBottom;
        u16 spriteLeft;
        u16 spriteRight;

        sprite = &gSecondarySpriteData[i];
        if (!(sprite->status & 1)) {
            continue;
        }
        if (sprite->id != 75) {
            continue;
        }

        spriteY = sprite->yPosition + 0x1000;
        spriteX = sprite->xPosition + 0x1000;
        spriteTop = spriteY - 40;
        spriteBottom = spriteY + 40;
        spriteLeft = spriteX - 16;
        spriteRight = spriteX + 16;

        if (!SpriteCollisionCheckObjectsTouching(
                currentTop, currentBottom, currentLeft, currentRight,
                spriteTop, spriteBottom, spriteLeft, spriteRight)) {
            continue;
        }

        sprite->status = 0;
        m4aSongNumStart(SOUND_5F);
        SpriteSpawnAsChild(
            PSPRITE_A8,
            gCurrentSprite.roomSlot,
            gCurrentSprite.gfxSlot,
            gCurrentSprite.yPosition,
            gCurrentSprite.xPosition
        );
        gCollectedKeyzer = 0;
        func_803F214();
        gSwitchStates[3] |= 2;
        if (GP(gCurrentSprite.pOamData) != sGhostOam_83C53C4) {
            GP_SET(gCurrentSprite.pOamData, sGhostOam_83C53C4);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
        }
        return;
    }
}

void func_803F66C(void)
{
    struct PrimarySpriteData *sprite;
    register u32 zero PIN("r5");
    u32 despawned;
    register u32 status PIN("r0");
    u32 oldStatus;
    register u32 flags PIN("r2");

    despawned = SpriteUtilDespawnIfSpriteExists(63);
    if (despawned != 0) {
        return;
    }

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    flags = 0xA408;
    ASM_BARRIER(: "+r"(flags));
    status = flags;
    ASM_BARRIER(: "+r"(status));
    zero = 0;
    status |= oldStatus;
    sprite->status = status;

    sprite->warioCollision = 6;
    {
        u8 *field0;
        register u32 value48 PIN("r1");
        u8 *field1;

        field0 = (u8 *)sprite + 32;
        value48 = 48;
        *field0 = value48;
        field1 = (u8 *)sprite + 33;
        field0 = (u8 *)24;
        *field1 = (u32)field0;
        field0 = (u8 *)sprite + 34;
        *field0 = value48;
    }
    {
        u8 *field;
        u32 value;

        field = (u8 *)sprite + 35;
        value = 128;
        *field = value;
        field++;
        value = 32;
        *field = value;
        field++;
        value = 64;
        *field = value;
        field++;
        value = 60;
        *field = value;
    }
    sprite->work0 = zero;
    sprite->work1 = zero;
    sprite->work2 = zero;

    SpriteUtilTurnTowardWario();
    SpriteSpawnAsChild(0xA7, sprite->roomSlot, sprite->gfxSlot,
                       sprite->yPosition, sprite->xPosition);
    if (sprite->pose == 1) {
        SpriteSpawnAsChild(0xA8, sprite->roomSlot, sprite->gfxSlot,
                           sprite->yPosition, sprite->xPosition);
    }

    GP_SET(sprite->pOamData, sGhostOam_83C5364);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = despawned;
    sprite->pose = 16;
}

void func_803F718(void)
{
    u32 targetId;
    struct PrimarySpriteData *loadCurrent;
    struct PrimarySpriteData *current;
    u32 roomSlot;
    u8 *room;
    register struct PrimarySpriteData *sprites PIN("r5");
    u32 activeFlag;
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *mirror;
    register s32 offset PIN("r3");
    register struct PrimarySpriteData *lastSprite PIN("ip");
    const struct AnimationFrame *selectedOam;

    targetId = 0xA7;
    loadCurrent = &gCurrentSprite;
    roomSlot = loadCurrent->roomSlot;
    current = loadCurrent;
    room = &gCurrentRoom;
    sprites = gSpriteData;
    activeFlag = 1;
    sprite = sprites;
    mirror = sprites;
    offset = 0;
    {
        u32 lastOffset;

        lastOffset = 253;
        lastOffset <<= 2;
        lastSprite = (struct PrimarySpriteData *)((u32)sprites + lastOffset);
    }

loop:
    if (sprite->globalID != targetId) {
        goto next;
    }
    if (sprite->roomSlot != roomSlot) {
        goto next;
    }
    {
        register u32 status PIN("r1");
        u32 maskedStatus;

        status = sprite->status;
        maskedStatus = activeFlag;
        maskedStatus &= status;
        if (maskedStatus != 0) {
            goto matched;
        }
    }

next:
    sprite++;
    mirror++;
    offset += sizeof(struct PrimarySpriteData);
    if ((s32)sprite <= (s32)lastSprite) {
        goto loop;
    }

finishSearch:
    {
        u32 persistentIndex;
        u8 *roomPtr;
        register u8 *persistentData PIN("r1");

        roomPtr = room;
        persistentIndex = *roomPtr;
        persistentIndex <<= 6;
        persistentIndex += current->roomSlot;
        persistentData = &gPersistentSpriteData[0][0];
        persistentIndex += (u32)persistentData;
        if (*(u8 *)persistentIndex != 17) {
            goto useDefaultOam;
        }
        selectedOam = sGhostOam_83C53C4;
        goto applyOam;
    }

matched:
    {
        register u32 oamAddress PIN("r0");
        const struct AnimationFrame *oam;
        u32 zero;

        oamAddress = (u32)sprites;
        oamAddress += 4;
        oamAddress = (u32)offset + oamAddress;
        oam = sGhostOam_83C5528;
        GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)oamAddress, oam);
        zero = 0;
        mirror->currentAnimationFrame = zero;
        mirror->animationTimer = zero;
        goto finishSearch;
    }

useDefaultOam:
    selectedOam = sGhostOam_83C5364;

applyOam:
    GP_SET(current->pOamData, selectedOam);
    current->currentAnimationFrame = 0;
    current->animationTimer = 0;
    current->pose = 16;
    m4aSongNumStart(SOUND_5D);
}

void func_803F7CC(void)
{
    struct PrimarySpriteData *sprite;
    u16 spriteX;
    u16 warioX;
    u16 facing;
    sprite = &gCurrentSprite;
    spriteX = sprite->xPosition + 0x1000;
    warioX = gWarioData.xPosition + 0x1000;
    facing = sprite->status & 0x40;

    {
        const u8 *persistent;
        u32 persistentIndex;

        persistent = &gPersistentSpriteData[0][0];
        persistentIndex = gCurrentRoom;
        persistentIndex <<= 6;
        persistentIndex = persistentIndex + sprite->roomSlot;
        persistentIndex += (u32)persistent;
        if (*(const u8 *)persistentIndex == 17) {
        func_8023BFC(sprite->yPosition, sprite->xPosition);
        if (!(gSpriteCollisionTileType & 0xF)) {
            func_8023BFC(sprite->yPosition - 64, sprite->xPosition - 128);
            if (!(gSpriteCollisionTileType & 0xF)) {
                func_8023BFC(sprite->yPosition - 64, sprite->xPosition + 128);
                {
                    u8 *indexPtr;
                    u32 currentIndex;
                    const s16 *table;
                    u32 address;
                    u32 raw;
                    u32 zero;
                    s32 signedValue;
                    u32 direction;
                    u32 newX;

                    indexPtr = &sprite->work2;
                    currentIndex = *indexPtr;
                    table = sUnk_83C55EA;
                    address = currentIndex << 1;
                    address += (u32)table;
                    raw = *(u16 *)address;
                    zero = 0;
                    signedValue = *(s16 *)(address + zero);
                    if (signedValue == 0x7FFF) {
                        func_803F944();
                    } else {
                        {
                            u32 nextIndex;
                            nextIndex = currentIndex + 1;
                            *indexPtr = nextIndex;
                        }
                        direction = facing;
                        if (direction != 0) {
                            newX = sprite->xPosition;
                            newX += raw;
                        } else {
                            newX = sprite->xPosition;
                            newX -= raw;
                        }
                        sprite->xPosition = newX;
                    }
                }
                return;
            }
        }
        }
    }

    {
        register u32 direction PIN("r1");
        direction = facing;
        ASM_BARRIER(: : "r"(direction));
        if (direction != 0) {
            if ((s32)spriteX > (s32)(warioX + 64)) {
            func_803F944();
        } else {
            struct PrimarySpriteData *current;
            register u8 *indexPtr PIN("r2");
            register u8 *indexAlias PIN("ip");
            u32 currentIndex;
            const s16 *table;
            u32 address;
            u32 raw;
            u32 zero;
            s32 signedValue;
            u32 newX;

            current = &gCurrentSprite;
            indexPtr = (u8 *)41;
            ASM_BARRIER(: : "r"(indexPtr));
            indexPtr = (u8 *)((u32)indexPtr + (u32)current);
            indexAlias = indexPtr;
            currentIndex = *indexPtr;
            table = sUnk_83C5598;
            address = currentIndex << 1;
            address += (u32)table;
            raw = *(u16 *)address;
            zero = 0;
            signedValue = *(s16 *)(address + zero);
            if (signedValue == 0x7FFF) {
                register u32 previousAddress PIN("r1");
                previousAddress = currentIndex - 1;
                previousAddress <<= 1;
                previousAddress += (u32)table;
                newX = current->xPosition;
                previousAddress = *(u16 *)previousAddress;
                newX += previousAddress;
            } else {
                u32 nextIndex;
                register u8 *storePtr PIN("r1");
                nextIndex = currentIndex + 1;
                storePtr = indexAlias;
                *storePtr = nextIndex;
                newX = current->xPosition;
                newX += raw;
            }
            current->xPosition = newX;
        }
    } else {
        if ((s32)spriteX < (s32)(warioX - 64)) {
            func_803F944();
        } else {
            struct PrimarySpriteData *current;
            register u8 *indexPtr PIN("r2");
            register u8 *indexAlias PIN("ip");
            u32 currentIndex;
            const s16 *table;
            u32 address;
            u32 raw;
            u32 zero;
            s32 signedValue;
            u32 newX;

            current = &gCurrentSprite;
            indexPtr = (u8 *)41;
            ASM_BARRIER(: : "r"(indexPtr));
            indexPtr = (u8 *)((u32)indexPtr + (u32)current);
            indexAlias = indexPtr;
            currentIndex = *indexPtr;
            table = sUnk_83C5598;
            address = currentIndex << 1;
            address += (u32)table;
            raw = *(u16 *)address;
            zero = 0;
            signedValue = *(s16 *)(address + zero);
            if (signedValue == 0x7FFF) {
                address = currentIndex - 1;
                address <<= 1;
                address += (u32)table;
                raw = *(u16 *)address;
            } else {
                u32 nextIndex;
                register u8 *storePtr PIN("r1");
                nextIndex = currentIndex + 1;
                storePtr = indexAlias;
                *storePtr = nextIndex;
            }
            newX = current->xPosition;
            newX -= raw;
            current->xPosition = newX;
        }
        }
    }
}

void func_803F944(void)
{
    gCurrentSprite.pose = 18;
    gCurrentSprite.work2 = 0;
}

void func_803F958(void)
{
    struct PrimarySpriteData *sprite;
    u32 xPosition;
    u32 spriteX;
    u32 warioX;
    register u32 status;
    u32 facing;
    register u8 *indexPtr PIN("ip");
    u32 index;
    register u32 delta PIN("r4");
    u32 deltaCopy;
    volatile u32 saved[3];

    sprite = &gCurrentSprite;
    xPosition = sprite->xPosition;
    {
        u32 adjusted;
        adjusted = (u16)(xPosition + 0x1000);
        spriteX = adjusted;
        saved[0] = adjusted;
    }
    {
        u32 adjusted;
        adjusted = (u16)(gWarioData.xPosition + 0x1000);
        warioX = adjusted;
        saved[1] = adjusted;
    }
    status = sprite->status;
    {
        u32 mask;
        mask = 0x40;
        mask &= status;
        facing = (u16)mask;
    }
    saved[2] = facing;
    {
        register u8 *address PIN("r0");
        address = (u8 *)41;
        ASM_BARRIER(: : "r"(address));
        address = (u8 *)((u32)address + (u32)sprite);
        indexPtr = address;
        index = *address;
    }
    delta = sUnk_83C5644[index];
    deltaCopy = delta;

    {
        register u32 persistentIndex PIN("r0");
        u32 roomSlot;
        const u8 *persistent;

        persistentIndex = gCurrentRoom;
        persistentIndex <<= 6;
        roomSlot = sprite->roomSlot;
        persistentIndex += roomSlot;
        persistent = &gPersistentSpriteData[0][0];
        persistentIndex += (u32)persistent;
        if (*(const u8 *)persistentIndex == 17) {
        if (facing != 0) {
            if (delta == 0x7FFF) {
                {
                    u32 zero;
                    register u8 *storePtr PIN("r1");
                    zero = 0;
                    storePtr = indexPtr;
                    *storePtr = zero;
                }
                func_803F718();
            } else {
                {
                    u32 nextIndex;
                    nextIndex = index + 1;
                    index = (u8)nextIndex;
                }
                *indexPtr = index;
                {
                    register u32 newX PIN("r0");
                    newX = xPosition + delta;
                    sprite->xPosition = newX;
                }
                if (index == 32) {
                    GP_SET(sprite->pOamData, sGhostOam_83C53FC);
                    sprite->currentAnimationFrame = 0;
                    sprite->animationTimer = 0;
                } else if (index == 42) {
                    {
                        u32 newStatus;
                        newStatus = 0xFFBF;
                        newStatus &= status;
                        sprite->status = newStatus;
                    }
                }
            }
        } else {
            if (delta == 0x7FFF) {
                {
                    register u8 *storePtr PIN("r1");
                    storePtr = indexPtr;
                    *storePtr = facing;
                }
                func_803F718();
            } else {
                {
                    u32 nextIndex;
                    nextIndex = index + 1;
                    index = (u8)nextIndex;
                }
                *indexPtr = index;
                {
                    register u32 newX PIN("r0");
                    newX = xPosition - delta;
                    sprite->xPosition = newX;
                }
                if (index == 32) {
                    u32 zero;
                    GP_SET(sprite->pOamData, sGhostOam_83C53FC);
                    zero = 0;
                    sprite->currentAnimationFrame = zero;
                    sprite->animationTimer = facing;
                } else if (index == 42) {
                    {
                        u32 newStatus;
                        newStatus = 0x40;
                        newStatus |= status;
                        sprite->status = newStatus;
                    }
                }
            }
        }
    } else {
        if (facing != 0) {
            if (delta == 0x7FFF) {
                {
                    u32 zero;
                    register u8 *storePtr PIN("r1");
                    zero = 0;
                    storePtr = indexPtr;
                    *storePtr = zero;
                }
                if (sprite->work1 != 0) {
                    sprite->pose = 110;
                } else {
                    func_803F718();
                }
            } else {
                {
                    u32 nextIndex;
                    nextIndex = index + 1;
                    index = (u8)nextIndex;
                }
                *indexPtr = index;
                {
                    register u32 newX PIN("r0");
                    newX = xPosition + delta;
                    sprite->xPosition = newX;
                }
                if (index == 32) {
                    GP_SET(sprite->pOamData, sGhostOam_83C53B4);
                    sprite->currentAnimationFrame = 0;
                    sprite->animationTimer = 0;
                } else if (index == 42 && spriteX > warioX) {
                    {
                        u32 newStatus;
                        newStatus = 0xFFBF;
                        newStatus &= status;
                        sprite->status = newStatus;
                    }
                }
            }
        } else {
            if (delta == 0x7FFF) {
                {
                    register u8 *storePtr PIN("r1");
                    storePtr = indexPtr;
                    *storePtr = facing;
                }
                if (sprite->work1 != 0) {
                    sprite->pose = 110;
                } else {
                    func_803F718();
                }
            } else {
                {
                    u32 nextIndex;
                    nextIndex = index + 1;
                    index = (u8)nextIndex;
                }
                *indexPtr = index;
                {
                    u32 deltaValue;
                    u32 newX;
                    deltaValue = deltaCopy;
                    newX = xPosition - deltaValue;
                    sprite->xPosition = newX;
                }
                if (index == 32) {
                    u32 zero;
                    GP_SET(sprite->pOamData, sGhostOam_83C53B4);
                    zero = 0;
                    sprite->currentAnimationFrame = zero;
                    sprite->animationTimer = *(volatile u16 *)((u8 *)saved + 8);
                } else if (index == 42 && saved[0] < saved[1]) {
                    {
                        u32 newStatus;
                        newStatus = 0x40;
                        newStatus |= status;
                        sprite->status = newStatus;
                    }
                }
            }
        }
    }
    }
}

void func_803FB14(void)
{
    gCurrentSprite.pose = 20;
    gCurrentSprite.work2 = 0;
}

void func_803FB28(void)
{
    struct PrimarySpriteData *sprite;
    register u32 xPosition PIN("r5");
    u32 spriteX;
    u32 warioX;
    register u32 status PIN("r6");
    register u32 facing PIN("r4");
    register u8 *indexPtr PIN("ip");
    u32 index;
    u32 delta;
    register u32 deltaCopy;
    struct PrimarySpriteData *spriteAlias;
    volatile u32 saved[2];

    sprite = &gCurrentSprite;
    xPosition = sprite->xPosition;
    {
        u32 adjusted;
        adjusted = (u16)(xPosition + 0x1000);
        spriteX = adjusted;
        ASM_BARRIER_ARM(: : : "r0");
        saved[0] = spriteX;
    }
    {
        u32 adjusted;
        adjusted = (u16)(gWarioData.xPosition + 0x1000);
        warioX = adjusted;
        ASM_BARRIER_ARM(: : : "r1");
        saved[1] = warioX;
    }
    status = sprite->status;
    {
        register u32 mask PIN("r0");
        mask = 0x40;
        mask &= status;
        facing = (u16)mask;
    }
    {
        register u8 *address PIN("r1");
        address = (u8 *)41;
        ASM_BARRIER(: : "r"(address));
        address = (u8 *)((u32)address + (u32)sprite);
        indexPtr = address;
        index = *address;
    }
    delta = sUnk_83C56AE[index];
    deltaCopy = delta;
    spriteAlias = sprite;

    if (facing != 0) {
        if (delta == 0x7FFF) {
            u32 zero;
            register u8 *storePtr PIN("r2");
            zero = 0;
            storePtr = indexPtr;
            *storePtr = zero;
            func_803F718();
        } else {
            {
                u32 nextIndex;
                nextIndex = index + 1;
                index = (u8)nextIndex;
            }
            {
                u8 *storePtr;
                storePtr = indexPtr;
                *storePtr = index;
            }
            {
                register u32 newX PIN("r0");
                newX = xPosition + delta;
                sprite->xPosition = newX;
            }
            if (index == 4) {
                if (gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] == 17) {
                    GP_SET(sprite->pOamData, sGhostOam_83C53FC);
                } else {
                    GP_SET(sprite->pOamData, sGhostOam_83C53B4);
                }
                {
                    u32 zero;
                    struct PrimarySpriteData *storeSprite;
                    zero = 0;
                    storeSprite = spriteAlias;
                    storeSprite->currentAnimationFrame = zero;
                    storeSprite->animationTimer = zero;
                }
            } else if (index == 14 && spriteX > warioX) {
                u32 newStatus;
                newStatus = 0xFFBF;
                newStatus &= status;
                sprite->status = newStatus;
            }
        }
    } else {
        if (delta == 0x7FFF) {
            register u8 *storePtr PIN("r2");
            storePtr = indexPtr;
            *storePtr = facing;
            func_803F718();
        } else {
            {
                u32 nextIndex;
                nextIndex = index + 1;
                index = (u8)nextIndex;
            }
            {
                u8 *storePtr;
                storePtr = indexPtr;
                *storePtr = index;
            }
            {
                register u32 newX PIN("r0");
                newX = xPosition - deltaCopy;
                sprite->xPosition = newX;
            }
            if (index == 4) {
                if (gPersistentSpriteData[gCurrentRoom][sprite->roomSlot] == 17) {
                    GP_SET(sprite->pOamData, sGhostOam_83C53FC);
                } else {
                    GP_SET(sprite->pOamData, sGhostOam_83C53B4);
                }
                {
                    u32 zero;
                    register struct PrimarySpriteData *storeSprite PIN("r1");
                    zero = 0;
                    storeSprite = spriteAlias;
                    storeSprite->currentAnimationFrame = zero;
                    storeSprite->animationTimer = zero;
                }
            } else if (index == 14) {
                register u32 savedSpriteX PIN("r2");
                u32 savedWarioX;
                savedSpriteX = saved[0];
                savedWarioX = saved[1];
                if (savedSpriteX < savedWarioX) {
                    u32 newStatus;
                    newStatus = 0x40;
                    newStatus |= status;
                    sprite->status = newStatus;
                }
            }
        }
    }
}

void func_803FC70(void)
{
    u32 targetId;
    struct PrimarySpriteData *loadCurrent;
    u32 roomSlot;
    s32 index;
    struct PrimarySpriteData *current;
    register struct PrimarySpriteData *sprites PIN("r6");
    register u32 oamBase PIN("r9");
    u32 zero;
    const struct AnimationFrame *childOam;

    targetId = 0xA7;
    loadCurrent = &gCurrentSprite;
    roomSlot = loadCurrent->roomSlot;
    index = 0;
    current = loadCurrent;
    sprites = gSpriteData;
    oamBase = (u32)sprites + 4;
    zero = 0;
    childOam = sGhostOam_83C5568;

loop:
    {
        u32 structSize;
        register s32 offset PIN("r3");
        struct PrimarySpriteData *child;

        structSize = sizeof(struct PrimarySpriteData);
        offset = index;
        offset *= structSize;
        child = (struct PrimarySpriteData *)((u32)offset + (u32)sprites);
        if (child->globalID != targetId) {
            goto next;
        }
        if (child->roomSlot != roomSlot) {
            goto next;
        }
        if (!(child->status & 1)) {
            goto next;
        }

        {
            register u32 address PIN("r0");
            u32 base;
            const struct AnimationFrame *oam;

            base = oamBase;
            address = (u32)offset + base;
            oam = childOam;
            GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)address, oam);
        }
        ASM_BARRIER(: "+r"(zero));
        child->currentAnimationFrame = zero;
        child->animationTimer = zero;
        goto found;
    }

next:
    index++;
    if (index <= 23) {
        goto loop;
    }

found:
    {
        u8 zeroTail;
        u32 pose;
        u8 *work3;
        const struct AnimationFrame *oam;

        zeroTail = 0;
        pose = 111;
        current->pose = pose;
        work3 = (u8 *)current + 42;
        *work3 = zeroTail;
        oam = sGhostOam_83C538C;
        GP_SET(current->pOamData, oam);
        current->currentAnimationFrame = zeroTail;
        current->animationTimer = zeroTail;
    }
}

void func_803FCF8(void)
{
    struct PrimarySpriteData *sprite;
    u32 baseX;
    register u32 positionOffset PIN("r1");
    register u32 spriteLimit PIN("ip");
    u32 spriteLimitHigh;
    u16 limitPosition;
    u32 warioLimit;
    u32 warioLimitHigh;
    u32 facing;
    register u8 *verticalIndexPtr PIN("r8");
    register u32 verticalIndex PIN("r2");
    u32 verticalAddress;
    u32 verticalRaw;
    s32 verticalSigned;
    s32 sentinel;

    sprite = &gCurrentSprite;
    baseX = sprite->xPosition;
    {
        register u32 offsetValue PIN("r0");

        offsetValue = 128;
        offsetValue <<= 5;
        ASM_BARRIER(: "+r"(offsetValue));
        positionOffset = offsetValue;
    }
    limitPosition = baseX;
    limitPosition += positionOffset;
    spriteLimit = limitPosition;
    spriteLimitHigh = spriteLimit;
    warioLimit = (u16)(positionOffset + gWarioData.xPosition);
    warioLimitHigh = warioLimit;
    facing = (u16)(sprite->status & 0x40);

    verticalIndex = 42;
    ASM_BARRIER(: : "r"(verticalIndex));
    verticalIndex += (u32)sprite;
    verticalIndexPtr = (u8 *)verticalIndex;
    verticalIndex = *(u8 *)verticalIndex;
    {
        const s16 *table;
        u32 zero;

        table = sUnk_83C56E0;
        verticalAddress = verticalIndex << 1;
        verticalAddress += (u32)table;
        verticalRaw = *(u16 *)verticalAddress;
        zero = 0;
        verticalSigned = *(s16 *)verticalAddress;
    }
    sentinel = 0x7FFF;

    if (verticalSigned == sentinel) {
        u8 *work1;
        u32 zero;

        work1 = &sprite->work1;
        zero = 0;
        *work1 = zero;
        if (facing != 0) {
            if (spriteLimit > warioLimit) {
                goto turnAway;
            }
        } else if (spriteLimitHigh < warioLimitHigh) {
            goto turnAway;
        }

        func_803F718();
        {
            u8 *work2;
            u32 value;

            work2 = &sprite->work2;
            value = 8;
            *work2 = value;
        }
        return;

turnAway:
        func_803FB14();
        return;
    }

    {
        u32 nextIndex;
        register u8 *indexPtr PIN("r2");

        nextIndex = verticalIndex + 1;
        indexPtr = verticalIndexPtr;
        *indexPtr = nextIndex;
    }
    sprite->yPosition += verticalRaw;
    {
        register u8 *horizontalIndexPtr PIN("ip");
        u32 horizontalIndex;
        const s16 *table;
        register u32 address PIN("r0");
        register u32 horizontalRaw PIN("r1");
        s32 horizontalSigned;
        u32 newX;

        address = 41;
        ASM_BARRIER(: : "r"(address));
        address += (u32)sprite;
        horizontalIndexPtr = (u8 *)address;
        horizontalIndex = *(u8 *)address;
        table = sUnk_83C5598;
        address = horizontalIndex << 1;
        address += (u32)table;
        horizontalRaw = *(u16 *)address;

        if (facing != 0) {
            horizontalSigned = (s16)horizontalRaw;
            if (horizontalSigned == sentinel) {
                u32 previousAddress;
                u32 previousRaw;

                previousAddress = horizontalIndex - 1;
                previousAddress <<= 1;
                previousAddress += (u32)table;
                previousRaw = *(u16 *)previousAddress;
                newX = baseX + previousRaw;
                goto applyX;
            }
            {
                u32 nextIndex;
                register u8 *indexPtr PIN("r2");

                nextIndex = horizontalIndex + 1;
                indexPtr = horizontalIndexPtr;
                *indexPtr = nextIndex;
            }
            newX = baseX + horizontalRaw;
            goto applyX;
        }

        horizontalSigned = (s16)horizontalRaw;
        if (horizontalSigned == sentinel) {
            u32 previousAddress;

            previousAddress = horizontalIndex - 1;
            previousAddress <<= 1;
            previousAddress += (u32)table;
            horizontalRaw = *(u16 *)previousAddress;
        } else {
            u32 nextIndex;
            register u8 *indexPtr PIN("r5");

            nextIndex = horizontalIndex + 1;
            indexPtr = horizontalIndexPtr;
            *indexPtr = nextIndex;
        }
        newX = baseX - horizontalRaw;

applyX:
        sprite->xPosition = newX;
    }
}

void func_803FDEC(void)
{
    register struct PrimarySpriteData *sprite PIN("ip");
    register u32 status PIN("r0");
    u32 oldStatus;
    register u32 flags PIN("r2");
    u32 zeroByte;
    register u32 zeroHalf PIN("r4");
    register u8 *fieldPtr PIN("r1");
    u8 *otherPtr;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    flags = 0xA400;
    ASM_BARRIER(: "+r"(flags));
    status = flags;
    ASM_BARRIER(: "+r"(status));
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    oldStatus = 0xFFFB;
    status &= oldStatus;
    fieldPtr = (u8 *)sprite;
    *(u16 *)fieldPtr = status;

    *(fieldPtr + 30) = 6;
    fieldPtr += 32;
    *fieldPtr = 24;
    otherPtr = (u8 *)sprite + 33;
    flags = 16;
    *otherPtr = flags;
    fieldPtr += 2;
    *fieldPtr = 40;
    fieldPtr++;
    *fieldPtr = 96;
    fieldPtr++;
    *fieldPtr = 64;
    fieldPtr++;
    *fieldPtr = 160;
    fieldPtr++;
    *(volatile u8 *)fieldPtr = 160;

    oam = sGhostOam_83C5528;
    fieldPtr = (u8 *)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(fieldPtr + 4), oam);
    *(fieldPtr + 22) = zeroByte;
    *(u16 *)(fieldPtr + 20) = zeroHalf;
    *(fieldPtr + 28) = flags;
}

void func_803FE54(void)
{
    register struct PrimarySpriteData *sprite PIN("ip");
    u32 status;
    register u32 oldStatus PIN("r1");
    register u32 flags PIN("r2");
    u32 zeroByte;
    register u32 zeroHalf PIN("r4");
    u8 *fieldPtr;
    u8 *otherPtr;
    const struct AnimationFrame *oam;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    flags = 0x8440;
    ASM_BARRIER(: "+r"(flags));
    status = flags;
    ASM_BARRIER(: "+r"(status));
    zeroByte = 0;
    zeroHalf = 0;
    status |= oldStatus;
    oldStatus = 0xFFFB;
    status &= oldStatus;
    fieldPtr = (u8 *)sprite;
    *(u16 *)fieldPtr = status;

    *(fieldPtr + 30) = 6;
    otherPtr = (u8 *)sprite + 32;
    flags = 16;
    *otherPtr = flags;
    otherPtr++;
    *(volatile u8 *)otherPtr = flags;
    fieldPtr += 34;
    *fieldPtr = 8;
    fieldPtr++;
    *fieldPtr = 48;
    fieldPtr++;
    *fieldPtr = 40;
    otherPtr = (u8 *)sprite + 37;
    *otherPtr = flags;
    fieldPtr += 2;
    *(volatile u8 *)fieldPtr = 12;

    oam = sKeyzerIdleOam;
    fieldPtr = (u8 *)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame *) *)(fieldPtr + 4), oam);
    *(fieldPtr + 22) = zeroByte;
    *(u16 *)(fieldPtr + 20) = zeroHalf;
    *(fieldPtr + 28) = flags;
    fieldPtr += 39;
    status = 60;
    *fieldPtr = status;
    flags = (u32)sprite;
    *(u8 *)(flags + 31) = status;
}

void func_803FEC8(void)
{
    struct PrimarySpriteData *loadCurrent;
    u32 roomSlot;
    struct PrimarySpriteData *current;
    u32 activeFlag;
    struct PrimarySpriteData *sprite;
    register struct PrimarySpriteData *workSprite PIN("r3");
    struct PrimarySpriteData *lastSprite;
    u32 targetId;

    targetId = 63;
    loadCurrent = &gCurrentSprite;
    roomSlot = loadCurrent->roomSlot;
    current = loadCurrent;
    activeFlag = 1;
    sprite = gSpriteData;
    workSprite = sprite;
    {
        u32 lastOffset;

        lastOffset = 253;
        lastOffset <<= 2;
        lastSprite = (struct PrimarySpriteData *)((u32)sprite + lastOffset);
    }

loop:
    if (sprite->globalID != targetId) {
        goto next;
    }
    if (sprite->roomSlot != roomSlot) {
        goto next;
    }
    {
        register u32 status PIN("r1");
        u32 maskedStatus;

        status = sprite->status;
        maskedStatus = activeFlag;
        maskedStatus &= status;
        if (maskedStatus != 0) {
            goto matched;
        }
    }

next:
    sprite++;
    workSprite++;
    if ((s32)sprite <= (s32)lastSprite) {
        goto loop;
    }

finishSearch:
    {
        u32 zero;
        struct PrimarySpriteData *statusSprite;

        zero = 0;
        statusSprite = current;
        statusSprite->status = zero;
    }
    func_803F264();
    gCollectedKeyzer = 1;
    gSwitchStates[3] |= 2;
    m4aSongNumStart(SOUND_60);

    if (!(gWarioData.horizontalDirection & 0x20)) {
        goto spawnLeft;
    }
    {
        register u32 xPosition PIN("r0");

        xPosition = gWarioData.xPosition;
        xPosition += 64;
        goto spawn;
    }

matched:
    {
        register u8 *timer PIN("r1");
        u32 value;

        timer = (u8 *)workSprite;
        timer += 39;
        value = 100;
        *timer = value;
        goto finishSearch;
    }

spawnLeft:
    {
        register u32 xPosition PIN("r0");

        xPosition = gWarioData.xPosition;
        xPosition -= 64;
    }

spawn:
    {
        register u32 xPosition PIN("r0");
        register u32 spawnX PIN("r1");
        u32 spawnY;

        spawnX = (u16)xPosition;
        spawnY = gWarioData.yPosition;
        spawnY -= 96;
        SpawnSecondarySprite(spawnY, spawnX, 75);
    }
}

void SpriteGhost(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;

    switch (gCurrentSprite.pose) {
        case 0:
        case 1:
            func_803F66C();
            break;

        case 16:
            func_803F2B4();
            if (func_803F320()) {
                return;
            }
            func_803F7CC();
            break;

        case 18:
            func_803F2B4();
            func_803F958();
            break;

        case 20:
            func_803F2B4();
            func_803FB28();
            break;

        case 110:
            func_803FC70();
            /* Fall through. */
        case 111:
            func_803FCF8();
            if (gCollectedKeyzer == 1) {
                func_803F528();
            } else {
                func_803F414();
            }
            break;
    }

    REG_BLDALPHA = 0x90B;
}

void SpriteUnknownA7(void)
{
    struct PrimarySpriteData *loadCurrent;
    u32 roomSlot;
    s32 i;
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    u32 targetId;

    targetId = 63;
    loadCurrent = &gCurrentSprite;
    roomSlot = loadCurrent->roomSlot;
    i = 0;
    current = loadCurrent;
    sprites = gSpriteData;

loop:
    {
        u32 offset;
        struct PrimarySpriteData *sprite;

        offset = sizeof(struct PrimarySpriteData);
        offset *= i;
        sprite = (struct PrimarySpriteData *)(offset + (u32)sprites);
        if (sprite->globalID != targetId) {
            goto next;
        }
        if (sprite->roomSlot != roomSlot) {
            goto next;
        }
        if (!(sprite->status & 1)) {
            goto next;
        }

        current->yPosition = sprite->yPosition;
        if (sprite->status & 0x40) {
            if (sprite->pose == 111) {
                current->xPosition = sprite->xPosition + 52;
                goto found;
            }
            current->xPosition = sprite->xPosition + 24;
            goto found;
        }
        current->xPosition = sprite->xPosition;
        goto found;
    }

next:
    i++;
    if (i <= 23) {
        goto loop;
    }

found:
    current->disableWarioCollisionTimer = 1;
    if (current->pose == 0) {
        func_803FDEC();
    }
}

void SpriteUnknownA8(void)
{
    u32 targetId;
    struct PrimarySpriteData *loadCurrent;
    u32 roomSlot;
    s32 i;
    struct PrimarySpriteData *current;
    struct PrimarySpriteData *sprites;
    struct PrimarySpriteData *sprite;
    struct PrimarySpriteData *positionSprite;

    targetId = 63;
    loadCurrent = &gCurrentSprite;
    roomSlot = loadCurrent->roomSlot;
    i = 0;
    current = loadCurrent;
    sprites = gSpriteData;
    positionSprite = current;

loop:
    {
        u32 offset;

        offset = sizeof(struct PrimarySpriteData);
        offset *= i;
        sprite = (struct PrimarySpriteData *)(offset + (u32)sprites);
        if (sprite->globalID != targetId) {
            goto next;
        }
        if (sprite->roomSlot != roomSlot) {
            goto next;
        }
        if (!(sprite->status & 1)) {
            goto next;
        }

        positionSprite->yPosition = sprite->yPosition;
        if (sprite->status & 0x40) {
            goto useNegativeOffset;
        }
        current->xPosition = sprite->xPosition - 72;
        goto found;
    }

next:
    i++;
    if (i <= 23) {
        goto loop;
    }

found:
    {
        u32 pose;

        pose = gCurrentSprite.pose;
        if (pose == 0) {
            goto initialize;
        }
        if (pose == 49) {
            goto transition;
        }
        goto updateTimer;
    }

useNegativeOffset:
    {
        s32 negativeOffset;
        register s32 adjustedPosition PIN("r0");

        negativeOffset = 0xFFD0;
        adjustedPosition = negativeOffset;
        ASM_BARRIER(: "+r"(negativeOffset), "+r"(adjustedPosition));
        /* agbcc otherwise loads the halfword before copying the literal to r0. */
        sprite = (struct PrimarySpriteData *)sprite->xPosition;
        adjustedPosition += (u32)sprite;
        current->xPosition = adjustedPosition;
        goto found;
    }

initialize:
    func_803FE54();
    goto updateTimer;

transition:
    func_803FEC8();

updateTimer:
    if (gCurrentSprite.status & 1) {
        gCurrentSprite.work0--;
        if (gCurrentSprite.work0 == 0) {
            gCurrentSprite.work0 = 60;
            m4aSongNumStart(SOUND_61);
        }
    }
}

#include "wario.h"
#include "bg_clip.h"
#include "score.h"
#include "sound.h"
#include "voice_set.h"
#include "wario_palette.h"

#ifdef PORTABLE
#include <string.h>
#endif

void PrepareWarioUpdate(void)
{
    struct WarioData* wario;
    u16 direction;
    u8 zero;

    memcpy(&gWarioDataCopy, &gWarioData, sizeof(struct WarioData));
    wario = &gWarioData;
    if (wario->unk_03 != 0) {
        direction = wario->horizontalDirection;
        direction ^= 0x30;
        zero = 0;
        wario->horizontalDirection = direction;
        wario->unk_03 = zero;
    }

    wario->unk_05 = 0;
    wario->unk_08 = 0;
    wario->unk_07 = 0;
    wario->unk_09 = 0;
    wario->unk_0A = 0;
    wario->unk_0C = 0;
    wario->xVelocity = 0;
    wario->yVelocity = 0;
    wario->unk_1E = 0;
    wario->unk_1F = 0;
    wario->unk_3A = 0;
    wario->unk_3B = 0;

    if (wario->reaction != REACTION_NORMAL && gWarioDashAfterimage.unk0 == 1) {
        m4aSongNumStart(SOUND_2B);
    }
}

void CopyWarioPalette(const void* source, int destinationIndex, int size)
{
    const u16* src;
    int start;
    int end;
    register u16* dst PIN("r1");
    int byteOffset;

    src = source;
    start = destinationIndex;
    end = start + size;
    if (start < end) {
        dst = gWarioPalette;
        byteOffset = start << 1;
        dst = (u16*)(byteOffset + (u32)dst);
        start = end - start;
        do {
            *dst++ = *src++;
            start--;
        } while (start != 0);
    }
}

s32 GetAdjustedWarioXVelocity(void)
{
    struct WarioData* wario;
    u16 velocity;
    u16 originalVelocity;

    wario = &gWarioData;
    velocity = wario->xVelocity;
    originalVelocity = velocity;
    if ((gCurrentCarriedSprite.unk1 & 0xF) > 1) {
        s32 value;
        register s32 limit PIN("r1");

        value = (s16)velocity;
        if (value > 40) {
            velocity = 40;
        }
        value = (s16)originalVelocity;
        limit = -40;
        if (value < limit) {
            velocity = (u16)-40;
        }
    }

    {
        u16 direction;
        register u16 flags PIN("r2");

        direction = wario->horizontalDirection;
        flags = (u16)wario->unk_1C;
        if ((direction & flags) != 0) {
            if ((flags & 1) != 0) {
                s32 signedVelocity;
                s32 product;

                signedVelocity = (s16)velocity;
                product = signedVelocity << 1;
                product += signedVelocity;
                product <<= 14;
                velocity = (u32)product >> 16;
            } else {
                s32 product;

                product = (s16)velocity << 2;
                product /= 5;
                velocity = (u16)product;
            }
        } else {
            s32 sum;
            s16* adjustment;
            s32 signedVelocity;

            adjustment = &gWarioHorizontalCollisionOffset;
            sum = (s16)velocity;
            sum += *(u16*)adjustment;
            sum <<= 16;
            velocity = (u32)sum >> 16;
            signedVelocity = sum >> 16;
            if (signedVelocity > 128) {
                velocity = 128;
            } else if (signedVelocity < -128) {
                velocity = (u16)-128;
            }
        }
    }

    return (s16)velocity;
}

void UpdateWarioHorizontalCollisionOffset(void)
{
    struct WarioData* wario;
    u16(*offsets)[3];
    struct WarioCollisionData* collision;
    int mask;
    s32 result;

    wario = &gWarioData;
    offsets = sWarioCollisionPointTable;
    collision = &gWarioCollisionData;

    {
        s32 probeX;
        u16 probeY;

        probeX = offsets[collision->unk_08][0];
        probeY = wario->xPosition;
        probeX += probeY;
        probeY = wario->yPosition + 1;
        result = GetBackgroundCollisionAtPosition(probeY, (u16)probeX) >> 16;
    }
    mask = 0xFF;
    result &= mask;
    if (result == 6) {
        gWarioHorizontalCollisionOffset = 32;
    } else if (result == 5) {
        gWarioHorizontalCollisionOffset = -32;
    } else {
        {
            u16* entry;
            s32 tableIndex;
            u16* secondOffsets;
            s32 probeX;
            u16 probeY;

            tableIndex = collision->unk_08;
            entry = (u16*)(tableIndex * 6);
            secondOffsets = (u16*)((u8*)offsets + 2);
            entry = (u16*)((u8*)entry + (u32)secondOffsets);
            probeX = *entry;
            probeY = wario->xPosition;
            probeX += probeY;
            probeY = wario->yPosition + 1;
            result = GetBackgroundCollisionAtPosition(probeY, (u16)probeX) >> 16;
        }
        result &= mask;
        if (result == 6) {
            gWarioHorizontalCollisionOffset = 32;
        } else if (result == 5) {
            gWarioHorizontalCollisionOffset = -32;
        } else {
            gWarioHorizontalCollisionOffset = 0;
        }
    }
}

void UpdateCarriedSpriteOffsets(void)
{
    struct CarriedSprite* carried;
    register const u16* offsets PIN("r2");
    s32 index;
    s32 xOffset;

    if ((gWarioData.horizontalDirection & 0x10) != 0) {
        carried = &gCurrentCarriedSprite;
        offsets = sCarriedSpriteXOffsets;
        index = carried->state - 1;
        xOffset = offsets[index];
    } else {
        carried = &gCurrentCarriedSprite;
        offsets = sCarriedSpriteXOffsets;
        index = carried->state - 1;
        xOffset = -offsets[index];
    }
    carried->xOffset = xOffset;

    {
        register struct CarriedSprite* current PIN("r2");
        const u16* yOffsets;
        s32 yIndex;

        current = carried;
        yOffsets = sCarriedSpriteYOffsets;
        yIndex = current->state - 1;
        current->yOffset = yOffsets[yIndex];
    }
}

void UpdateWarioPositionHistory(void)
{
    struct WarioAfterimage* afterimage;
    u8 timer;
    u16* indexPtr;
    register u16 index PIN("r2");
    u16* history;
    u16* slot;
    u32 offset;
    u16 position;

    afterimage = &gWarioDashAfterimage;
    timer = afterimage->unk1;
    index = timer;
    if (index != 0) {
        timer--;
        afterimage->unk1 = timer;
        indexPtr = &gWarioAfterimageHistoryIndex;
        index = *indexPtr;
        offset = 0x1F;
        offset &= index;
        history = gWarioAfterimageHistory.previousXPositions;
        offset <<= 1;
        slot = (u16*)(offset + (u32)history);
        position = gPreviousXPosition;
        *slot = position;
        history += 32;
        offset += (u32)history;
        position = gPreviousYPosition;
        *(u16*)offset = position;
        index++;
        *indexPtr = index;
        if ((s16)index > 31) {
            gWarioAfterimageHistoryWrapped = 1;
        }
    } else {
        afterimage->unk0 = 0;
        gWarioAfterimageHistoryIndex = 0;
        gWarioAfterimageHistoryWrapped = 0;
    }
}

void WarioProcessControls(void)
{
    struct WarioData* wario;
    register struct WarioData* current PIN("r4");
    struct WarioCollisionData* collision;
    s32 zero;
    s32 mode;
    u8* previousReaction;
    u8 poseRequest;

    collision = &gWarioCollisionData;
    zero = 0;
    collision->unk_0E = zero;
    collision->unk_0F = zero;
    collision->unk_10 = zero;

    mode = gSubGameMode;
    wario = &gWarioData;
    if (mode > 1) {
        wario->unk_1E++;
        gWarioMotionAfterimage.unk1++;
    }

    previousReaction = &gPreviousReaction;
    current = wario;
    *previousReaction = current->reaction;
    gPreviousXPosition = current->xPosition;
    gPreviousYPosition = current->yPosition;

    poseRequest = GF(sWarioPoseHandlerTable[current->reaction])();
    if (poseRequest != 0xFF) {
        GF(sWarioPoseRequestFuncTable[current->reaction])(poseRequest);
    }

    GF(sUnk_82DED90[wario->reaction])();
    GF(sUnk_82DECD0[wario->reaction])();

    if (gCurrentCarriedSprite.state != 0) {
        UpdateCarriedSpriteOffsets();
    }
}

void WarioProcessCollision(void)
{
    register struct WarioCollisionData* initialCollision PIN("r1");
    struct WarioCollisionData* collision;
    struct WarioData* initialWario;
    struct WarioData* wario;
    u32 currentX;
    register u8 zero PIN("r5");
    u16* previousXPointer;
    u32 previousX;
    u32 currentY;
    u16* previousYPointer;
    u32 previousY;

    initialCollision = &gWarioCollisionData;
    zero = 0;
    initialCollision->unk_00 = 0;
    initialCollision->flags = 0;
    /* Preserve the dedicated byte-clear register used by the original schedule. */
    ASM_BARRIER(: "+r"(zero));
    initialCollision->unk_12 = zero;
    initialCollision->unk_13 = zero;
    initialCollision->unk_14 = zero;
    gWarioCollisionDamageFlag = zero;

    initialWario = &gWarioData;
    previousXPointer = &gPreviousXPosition;
    currentX = initialWario->xPosition;
    previousX = *previousXPointer;
    collision = initialCollision;
    wario = initialWario;
    if (currentX > previousX) {
        collision->unk_00 = 0x10;
        collision->unk_04 = 1;
        collision->unk_05 = zero;
        collision->unk_06 = 0xFFFF;
    } else {
        if (currentX >= previousX) {
            if ((wario->horizontalDirection & 0x10) != 0) {
                collision->unk_04 = 1;
                collision->unk_05 = zero;
                collision->unk_06 = 0xFFFF;
                goto horizontalDone;
            }
        } else {
            collision->unk_00 = 0x20;
        }
        collision->unk_04 = zero;
        collision->unk_05 = 1;
        collision->unk_06 = 0x40;
    }

horizontalDone:
    previousYPointer = &gPreviousYPosition;
    currentY = wario->yPosition;
    previousY = *previousYPointer;
    if (currentY > previousY) {
        collision->flags = 0x80;
    } else if (currentY < previousY) {
        collision->flags = 0x40;
    }

    GF(sUnk_82DED00[wario->reaction])();
    GF(sUnk_82DED90[wario->reaction])();
}

void ProcessWarioInteraction(void)
{
    struct WarioData* wario;
    register struct WarioData* current PIN("r4");
    struct CarriedSprite* carried;
    u8 value;
    u8 direction;

    carried = &gCurrentCarriedSprite;
    value = carried->state;
    wario = &gWarioData;
    if (value == 0) {
        value = wario->pose - WPOSE_NORMAL_WEAK_THROW;
        if (value <= 1) {
            carried->unk1 &= 0x80;
        } else {
            carried->unk1 = 0;
        }
    }

    current = wario;
    if ((current->horizontalDirection & 0x10) != 0) {
        direction = 0;
    } else {
        direction = 1;
    }
    GF(sUnk_82DED30[current->reaction])(direction);

    if (current->reaction == REACTION_WATER) {
        if (gWarioWaterCollisionActive == 0) {
            current->unk_1C = 0;
        }
    } else if (wario->unk_1A == 2) {
        wario->unk_1C = 0;
    }
}

void FinalizeWarioUpdate(void)
{
    if (gSwitchPressed == 0 || gWarioData.reaction == REACTION_NORMAL) {
        GF(sUnk_82DED60[gWarioData.reaction])();
    }

    switch (gPreviousReaction) {
        case REACTION_NORMAL:
        case REACTION_WATER:
        case REACTION_FROZEN:
        case REACTION_ZOMBIE:
        case REACTION_SNOWMAN:
            break;
        default:
            if (gWarioData.damageTimer == 0x5F) {
                m4aSongNumStart(SE_WARIO_BONK);
                VoiceSetPlay(0);
            }
            break;
    }
}

void ResetWarioState(void)
{
    u8 pose;

    PrepareWarioUpdate();
    gWarioMotionAfterimage = sEmptyWarioAfterimage;
    gCurrentWarioEffect = sStartingWarioEffect;
    gCurrentCarriedSprite.state = 0;

    pose = gWarioDataCopy.pose;
    switch (pose) {
        case WPOSE_NORMAL_DASH_ATTACK:
        case WPOSE_NORMAL_DASH_ATTACK_JUMP:
        case WPOSE_NORMAL_GROUND_POUND:
        case WPOSE_NORMAL_SUPER_GROUND_POUND:
        case WPOSE_NORMAL_CHARGING_THROW_2:
        case WPOSE_NORMAL_TURNING_WHILE_CHARING_THROW:
            MPlayStop(GP(gMPlayTable[3].info));
            break;
    }
}

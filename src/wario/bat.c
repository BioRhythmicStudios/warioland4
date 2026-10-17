#include "types.h"

/*
 * Keep the matched ABI declarations from wario/matched_helpers.h.  The
 * narrower convenience prototypes in wario.h alter agbcc call-site output.
 */
#define CopyWarioPalette BatHeader_CopyWarioPalette
#define ResolveWarioCeilingCollision BatHeader_ResolveWarioCeilingCollision
#define ResolveWarioFloorCollision BatHeader_ResolveWarioFloorCollision
#include "wario.h"
#include "wario/swimming.h"
#undef CopyWarioPalette
#undef ResolveWarioCeilingCollision
#undef ResolveWarioFloorCollision

#include "bg_clip.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "input.h"
#include "oam.h"
#include "sound.h"
#include "voice_set.h"
#include "wario/bat.h"
#include "wario/matched_helpers.h"
#include "wario_palette.h"

u8 UpdateBatWario(void)
{
    return GF(sBatWarioPoseTable[gWarioData.pose])();
}

u8 BatWarioTransforming(void)
{
    struct WarioData* wario;
    const struct BatAnimationFrame* animation;
    if (gCurrentWarioEffect.unk_3 <= 3) {
        return 0xFF;
    }
    wario = &gWarioData;
    animation = sBatTransformAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0) {
            return 3;
        }
    }
    return 0xFF;
}

u8 BatWarioFlying(void)
{
    u16* pressedPtr;
    register u16* heldPtr PIN("r1");
    struct WarioData* wario;
    register int one PIN("r2");
    register int pressed PIN("r1");
    int initialHeld;
    int held;
    register int direction PIN("r2");
    register int intersection PIN("r3");
    register int temp PIN("r0");
    register struct WarioData* animationWario PIN("r2");
    const struct BatAnimationFrame* animation;
    int counter;

    pressedPtr = &gButtonsPressed;
    pressed = *pressedPtr;
    one = 1;
    temp = one;
    temp &= pressed;
    if (temp != 0) {
        gWarioData.unk_08 = 1;
        return 0xFE;
    }

    heldPtr = &gButtonsHeld;
    initialHeld = *heldPtr;
    one &= initialHeld;
    wario = &gWarioData;
    if (one == 0) {
        if (wario->yVelocity > 0) {
            wario->yVelocity = one;
        }
        wario->unk_09 = one;
    }

    if ((u16)(wario->yVelocity - 1) <= 31) {
        counter = wario->unk_09;
        temp = counter;
        if (temp != 0) {
            wario->yVelocity = 32;
            wario->unk_09 = counter - 1;
        } else {
            wario->yVelocity = temp;
        }
    }

    held = *heldPtr;
    direction = wario->horizontalDirection;
    intersection = held & direction;
    if (intersection != 0) {
        if (direction & 0x10) {
            wario->xVelocity = 32;
        } else {
            wario->xVelocity = -32;
        }
    } else {
        direction ^= 0x30;
        temp = direction;
        temp &= held;
        ASM_BARRIER(: "+r"(temp));
        temp <<= 16;
        if (temp != 0) {
            wario->horizontalDirection = direction;
        }
        wario->xVelocity = intersection;
    }

    animationWario = wario;
    animation = sBatFlyAnimation;
    if (animationWario->unk_1E >= animation[animationWario->unk_1F].time) {
        animationWario->unk_1E = 0;
        temp = animationWario->unk_1F + 1;
        animationWario->unk_1F = temp;
        temp = (u8)temp;
        if (temp == 1) {
            m4aSongNumStart(SE_BAT_WARIO_FLAP);
        } else if (animation[wario->unk_1F].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 BatWarioLanding(void)
{
    u16 pressed;
    int one;
    struct WarioData* wario;
    const struct BatAnimationFrame* animation;
    pressed = gButtonsPressed;
    one = 1;
    one &= pressed;
    if (one != 0) {
        struct WarioData* current;
        int value;
        current = &gWarioData;
        value = 1;
        current->unk_08 = value;
        return 0xFE;
    }
    wario = &gWarioData;
    animation = sBatLandAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = one;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0) {
            return 3;
        }
    }
    return 0xFF;
}

u8 BatWarioTurning(void)
{
    u16* heldPtr;
    struct WarioData* wario;
    u16 direction;
    register int mask PIN("r0");
    int changedDirection;
    const struct BatAnimationFrame* animation;

    if (gButtonsPressed & 1) {
        gWarioData.unk_08 = 1;
        return 0xFE;
    }

    heldPtr = &gButtonsHeld;
    wario = &gWarioData;
    direction = wario->horizontalDirection;
    /* Keep the original direction live before constructing the mask. */
    ASM_BARRIER(: "+r"(direction));
    mask = 0x30;
    changedDirection = direction;
    changedDirection ^= mask;
    mask = *heldPtr;
    changedDirection &= mask;
    if (changedDirection != 0) {
        return 2;
    }

    animation = sBatTurnAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = changedDirection;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 BatWarioRecovering(void)
{
    struct WarioData* wario;
    const struct BatAnimationFrame* animation;
    wario = &gWarioData;
    animation = sBatRecoverAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0) {
            wario->reaction = 0;
            wario->damageTimer = 96;
            return 2;
        }
    }
    return 0xFF;
}

void SetBatWarioPose(int pose)
{
    int value;
    struct WarioData* wario;
    value = (u8)pose;
    ResetWarioState();
    if (value == 4) {
        goto resetEffects;
    }
    if (value > 4) {
        goto greater;
    }
    if (value == 0) {
        goto transforming;
    }
    if (value == 2) {
        goto onGround;
    }
    wario = &gWarioData;
    goto setPose;
greater:
    if (value == 253) {
        goto touchingLight;
    }
    wario = &gWarioData;
    if (value != 254) {
        goto setPose;
    }
    wario->pose = 1;
    if (gWarioDataCopy.unk_08 == 1) {
        wario->unk_09 = 12;
        wario->yVelocity = 128;
    }
    goto end;
touchingLight:
    gWarioData.pose = 3;
    goto end;
transforming:
    m4aSongNumStart(SE_BAT_WARIO_TRANSFORM);
    VoiceSetPlay(4);
resetEffects:
    gWarioMotionAfterimage = sEmptyWarioAfterimage;
    gCurrentWarioEffect = sStartingWarioEffect;
    gWarioData.pose = value;
    gCurrentWarioEffect.type = 4;
    goto end;
onGround:
    wario = &gWarioData;
    wario->unk_03 = 1;
setPose:
    wario->pose = value;
end:
    return;
}

void UpdateBatWarioMovement(void)
{
    register u8* hitbox PIN("r3");
    register const u8* data PIN("r2");
    struct WarioData* wario;
    s32 movement;
    s32 temp;
    s32 shifted;
    int velocity;
    s16 signedVelocity;
    s32 horizontalMovement;
    const u8* ptr;
    u32 index;

    hitbox = &gWarioCollisionData;
    data = sBatPoseData;
    wario = &gWarioData;
    index = wario->pose * 8;
    ptr = data + 1;
    index += (u32)ptr;
    hitbox[8] = *(u8*)index;
    index = wario->pose * 8;
    ptr = data + 2;
    index += (u32)ptr;
    hitbox[9] = *(u8*)index;
    index = wario->pose * 8;
    data += 3;
    index += (u32)data;
    hitbox[10] = *(u8*)index;
    UpdateWarioHorizontalCollisionOffset();

    movement = 0;
    if (wario->pose == 1) {
        velocity = *(u16*)&wario->yVelocity;
        shifted = velocity << 16;
        temp = shifted >> 19;
        movement = (u16)temp;
        if (shifted >= 0) {
            temp = velocity;
            temp -= 8;
        } else {
            temp = velocity - 4;
        }
        wario->yVelocity = temp;
    }

    gWarioData.yPosition -= movement;
    signedVelocity = gWarioData.yVelocity;
    if (signedVelocity < -128) {
        gWarioData.yVelocity = -128;
    }
    if (gWarioData.unk_1A == 0) {
        horizontalMovement = GetAdjustedWarioXVelocity();
    } else {
        horizontalMovement = (u16)gWarioData.xVelocity;
    }
    horizontalMovement <<= 16;
    horizontalMovement >>= 19;
    movement = (u16)horizontalMovement;
    /* Keep the normalized movement live before reloading gWarioData. */
    ASM_BARRIER(: "+r"(movement));
    gWarioData.xPosition += movement;
}

void ProcessBatWarioCollision(void)
{
    u8* hitbox;
    register const u8* data PIN("r2");
    register struct WarioData* wario PIN("r3");
    struct WarioData* floorWario;
    u8* finalHitbox;
    int result;
    int callResult;
    int collisionType;
    register int flags PIN("r2");
    int floorResult;
    register int mask PIN("r0");
    const u8* ptr;
    u32 index;

    hitbox = &gWarioCollisionData;
    data = sBatPoseData;
    wario = &gWarioData;
    index = wario->pose * 8;
    ptr = data + 4;
    index += (u32)ptr;
    hitbox[11] = *(u8*)index;
    index = wario->pose * 8;
    ptr = data + 5;
    index += (u32)ptr;
    hitbox[12] = *(u8*)index;
    index = wario->pose * 8;
    ptr = data + 6;
    index += (u32)ptr;
    hitbox[13] = *(u8*)index;
    index = wario->pose * 8;
    data += 7;
    index += (u32)data;
    collisionType = *(u8*)index;
    hitbox[17] = collisionType;

    result = 0xFF;
    flags = *(u16*)(hitbox + 2);
    if (flags & 0x40) {
        callResult = ResolveWarioFloorCollision();
        goto normalize;
    }
    mask = 0x80;
    mask &= flags;
    if (mask != 0) {
        callResult = ResolveWarioCeilingCollision();
        /* Prevent agbcc from merging this collision call with the next arm. */
        ASM_BARRIER();
        goto normalize;
    }
    if (*(u16*)hitbox != 0) {
        if (collisionType == 2) {
            callResult = ResolveWarioCeilingCollision();
        } else {
            callResult = ResolveWarioStandardCollision();
        }
        goto normalize;
    }
    if (collisionType == 0) {
        callResult = ResolveWarioLandingCollision();
    normalize:
        result = (u8)callResult;
    }

    floorWario = &gWarioData;
    floorResult = GetBackgroundCollisionAtPosition((u16)(floorWario->yPosition - 48), floorWario->xPosition);
    mask = 0xFF;
    floorResult &= mask;
    if (floorResult == 1) {
        floorWario->reaction = floorResult;
        floorWario->damageTimer = 96;
        SetWarioWaterPose(0);
    } else {
        finalHitbox = &gWarioCollisionData;
        if (finalHitbox[17] != 0xFF && result != 0xFF) {
            SetBatWarioPose(result);
        }
    }
}

void LoadBatWarioGraphics(int index)
{
    int value;
    struct WarioData* wario;
    int pose;
    register GBA_TYPEOF(const struct BatGraphicsFrame**, &(sBatGraphicsTable)[0]) table PIN("r2");
    u32 offset;
    const struct BatGraphicsFrame* frame;
    const u8* gfx;
    value = (u8)index;
    UpdateWarioPositionHistory();
    wario = &gWarioData;
    pose = wario->pose;
    if (pose != 0 || gCurrentWarioEffect.unk_3 > 3) {
        table = sBatGraphicsTable;
        offset = value << 2;
        pose <<= 3;
        offset += pose;
        offset += (u32)table;
        frame = GP(*(GBA_PTR(const struct BatGraphicsFrame*) *)offset);
        frame += wario->unk_1F;
        gfx = GP(frame->data);
        wario->objData1Size = gfx[0] << 5;
        gfx++;
        wario->objData2Size = gfx[0] << 5;
        gfx++;
        GP_SET(wario->pObjData1, (u8*)gfx);
        GP_SET(wario->pObjData2, (u8*)gfx + wario->objData1Size);
        GP_SET(wario->pOamData, GP(frame->oam));
    }
    gWarioPaletteSize = 96;
    CopyWarioPalette(sBatPalette, 0, 48);
}

void ApplyBatWarioMusicEffects(void)
{
    struct MusicPlayerInfo *mplay0, *mplay1;
    if (gWarioMusicState != 13) {
        mplay0 = GP(gMPlayTable[0].info);
        m4aMPlayTempoControl(mplay0, 300);
        mplay1 = GP(gMPlayTable[1].info);
        m4aMPlayTempoControl(mplay1, 300);
        m4aMPlayPitchControl(mplay0, 0xFFFF, 256);
        m4aMPlayPitchControl(mplay1, 0xFFFF, 256);
        m4aMPlayModDepthSet(mplay0, 0xFFFF, 20);
        m4aMPlayModDepthSet(mplay1, 0xFFFF, 20);
        m4aMPlayLFOSpeedSet(mplay0, 0xFFFF, 30);
        m4aMPlayLFOSpeedSet(mplay1, 0xFFFF, 30);
        gWarioMusicState = 13;
    }
}

void UpdateBatWarioHitbox(void)
{
    const u8* data;
    struct WarioData* wario;
    const u8* hitboxes;
    u32 offset;
    u32 address;
    u32 poseOffset;
    u8 value;
    data = sBatPoseData;
    wario = &gWarioData;
    offset = data[wario->pose * 8];
    hitboxes = (const u8*)sWarioHitboxes;
    offset <<= 3;
    address = offset + (u32)hitboxes;
    wario->hitboxOffsetLeft = *(u16*)address;
    address = (u32)(hitboxes + 2);
    address = offset + address;
    wario->hitboxOffsetTop = *(u16*)address;
    address = (u32)(hitboxes + 4);
    address = offset + address;
    wario->hitboxOffsetRight = *(u16*)address;
    hitboxes += 6;
    offset += (u32)hitboxes;
    wario->hitboxOffsetBottom = *(u16*)offset;
    poseOffset = wario->pose * 8;
    data += 7;
    poseOffset += (u32)data;
    value = *(u8*)poseOffset;
    if (value == 2) {
        wario->unk_1A = value;
    } else if (wario->unk_1A != 1) {
        wario->unk_1A = value;
    }
}

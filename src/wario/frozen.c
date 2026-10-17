#include "types.h"

#define CopyWarioPalette FrozenHeader_CopyWarioPalette
#define ResolveWarioCeilingCollision FrozenHeader_ResolveWarioCeilingCollision
#define ResolveWarioFloorCollision FrozenHeader_ResolveWarioFloorCollision
#include "wario.h"
#include "wario/swimming.h"
#undef CopyWarioPalette
#undef ResolveWarioCeilingCollision
#undef ResolveWarioFloorCollision

#include "bg_clip.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "oam.h"
#include "voice_set.h"
#include "wario/frozen.h"
#include "wario/matched_helpers.h"
#include "wario_palette.h"

u8 UpdateFrozenWario(void)
{
    return GF(sFrozenWarioPoseTable[gWarioData.pose])();
}

u8 FrozenWarioTransforming(void)
{
    struct WarioData* wario;
    const struct FrozenAnimationFrame* animation;

    wario = &gWarioData;
    animation = sFrozenTransformAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0) {
            return 1;
        }
    }
    return 0xFF;
}

u8 FrozenWarioSlideLeft(void)
{
    if (gWarioData.horizontalDirection & 0x10) {
        gWarioData.xVelocity = -96;
    } else {
        gWarioData.xVelocity = 96;
    }
    return 0xFF;
}

u8 FrozenWarioSlideRight(void)
{
    if (gWarioData.horizontalDirection & 0x10) {
        gWarioData.xVelocity = -96;
    } else {
        gWarioData.xVelocity = 96;
    }
    return 0xFF;
}

u8 FrozenWarioBonk(void)
{
    struct WarioData* wario;
    const struct FrozenAnimationFrame* animation;

    wario = &gWarioData;
    animation = sFrozenBonkAnimation;
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

void SetFrozenWarioPose(int pose)
{
    int value;
    value = (u8)pose;
    ResetWarioState();
    if (value == 3) {
        goto playVoice;
    }
    if (value > 3) {
        goto greater;
    }
    if (value == 0) {
        goto playFreeze;
    }
    goto setPose;
greater:
    if (value == 253) {
        goto poseOne;
    }
    if (value != 254) {
        goto setPose;
    }
    gWarioData.pose = 2;
    goto end;
poseOne:
    gWarioData.pose = 1;
    goto end;
playFreeze:
    m4aSongNumStart(0x16F);
    goto setPose;
playVoice:
    m4aSongNumStart(0x15F);
    VoiceSetPlay(0);
setPose:
    gWarioData.pose = value;
end:
    return;
}

void UpdateFrozenWarioMovement(void)
{
    register u8* hitbox PIN("r3");
    register const u8* data PIN("r2");
    struct WarioData* wario;
    s32 movement;
    u16 velocity;
    s32 temp;
    const u8* ptr;
    u16 horizontalMovement;
    u32 index;

    hitbox = &gWarioCollisionData;
    data = sFrozenPoseData;
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
    if (wario->unk_1A == 2) {
        velocity = wario->yVelocity;
        temp = velocity << 16;
        temp >>= 19;
        movement = (u16)temp;
        velocity -= 8;
        wario->yVelocity = velocity;
    }
    if ((s16)wario->yVelocity < -128) {
        wario->yVelocity = -128;
    }
    wario->yPosition -= movement;
    if (wario->unk_1A == 0) {
        temp = GetAdjustedWarioXVelocity();
    } else {
        temp = (u16)wario->xVelocity;
    }
    temp <<= 16;
    temp >>= 19;
    movement = (u16)temp;
    /* Keep the normalized movement live in r2 before loading gWarioData. */
    ASM_BARRIER(: "+r"(movement));
    gWarioData.xPosition += movement;
}

void ProcessFrozenWarioCollision(void)
{
    u8* hitbox;
    register const u8* data PIN("r2");
    register struct WarioData* wario PIN("r3");
    int result;
    int callResult;
    struct WarioData* warioAgain;
    int floorResult;
    int mask;
    u8* hitboxAgain;
    const u8* ptr;
    u32 index;
    u16 flags;

    hitbox = &gWarioCollisionData;
    data = sFrozenPoseData;
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
    hitbox[17] = *(u8*)index;
    if (wario->yVelocity <= 47) {
        hitbox[12] = 0;
    }
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
        ASM_BARRIER();
        goto normalize;
    }
    if (*(u16*)hitbox != 0) {
        if (hitbox[17] == 2) {
            callResult = ResolveWarioCeilingCollision();
        } else {
            callResult = ResolveWarioStandardCollision();
        }
        goto normalize;
    }
    if (hitbox[17] == 0) {
        callResult = ResolveWarioLandingCollision();
    normalize:
        result = (u8)callResult;
    }
    warioAgain = &gWarioData;
    floorResult = GetBackgroundCollisionAtPosition((u16)(warioAgain->yPosition - 48), warioAgain->xPosition);
    mask = 0xFF;
    floorResult &= mask;
    if (floorResult == 1) {
        if (warioAgain->pose != 3) {
            SetFrozenWarioPose(3);
        }
        gWarioDustEffect1.unk0 = floorResult;
    } else {
        hitboxAgain = &gWarioCollisionData;
        if (hitboxAgain[17] != 0xFF) {
            if (hitboxAgain[18] != 0) {
                result = 3;
            }
            if (result != 0xFF) {
                SetFrozenWarioPose(result);
            }
        }
    }
}

void LoadFrozenWarioGraphics(int index)
{
    u32 value;
    GBA_TYPEOF(const struct FrozenGraphicsFrame**, &(sFrozenGraphicsTable)[0]) table;
    struct WarioData* wario;
    const struct FrozenGraphicsFrame* frame;
    const u8* gfx;
    value = (u8)index;
    UpdateWarioPositionHistory();
    table = sFrozenGraphicsTable;
    value <<= 2;
    wario = (struct WarioData*)0x03001898;
    value += wario->pose << 3;
    value += (u32)table;
    frame = GP(*(GBA_PTR(const struct FrozenGraphicsFrame*) *)value);
    frame += wario->unk_1F;
    gfx = GP(frame->data);
    wario->objData1Size = gfx[0] << 5;
    gfx++;
    wario->objData2Size = gfx[0] << 5;
    gfx++;
    GP_SET(wario->pObjData1, (u8*)gfx);
    GP_SET(wario->pObjData2, (u8*)gfx + wario->objData1Size);
    GP_SET(wario->pOamData, GP(frame->oam));
    gWarioPaletteSize = 64;
    CopyWarioPalette(sWarioDefaultObjPalette, 0, 16);
    CopyWarioPalette(sFrozenPalette, 16, 16);
}

void ApplyFrozenWarioMusicEffects(void)
{
    struct MusicPlayerInfo* mplay0;
    struct MusicPlayerInfo* mplay1;
    if (gWarioMusicState != 8) {
        mplay0 = GP(gMPlayTable[0].info);
        m4aMPlayTempoControl(mplay0, 350);
        mplay1 = GP(gMPlayTable[1].info);
        m4aMPlayTempoControl(mplay1, 350);
        m4aMPlayPitchControl(mplay0, 0xFFFF, 1000);
        m4aMPlayPitchControl(mplay1, 0xFFFF, 1000);
        m4aMPlayModDepthSet(mplay0, 0xFFFF, 40);
        m4aMPlayModDepthSet(mplay1, 0xFFFF, 40);
        m4aMPlayLFOSpeedSet(mplay0, 0xFFFF, 50);
        m4aMPlayLFOSpeedSet(mplay1, 0xFFFF, 50);
        gWarioMusicState = 8;
    }
}

void UpdateFrozenWarioHitbox(void)
{
    const u8* data;
    struct WarioData* wario;
    const u8* hitboxes;
    u32 offset;
    u32 address;
    u32 poseOffset;
    u8 value;
    data = sFrozenPoseData;
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

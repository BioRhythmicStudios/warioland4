#include "types.h"

#define CopyWarioPalette PuffyHeader_CopyWarioPalette
#define ResolveWarioFloorCollision PuffyHeader_ResolveWarioFloorCollision
#include "wario.h"
#undef CopyWarioPalette
#undef ResolveWarioFloorCollision

#include "gba/m4a.h"
#include "global_data.h"
#include "input.h"
#include "oam.h"
#include "sound.h"
#include "wario/puffy.h"
#include "wario_palette.h"

/* ABI-sensitive declarations required by the original matched source. */
void ResolveWarioFloorCollision(void);
void CopyWarioPalette(const void* source, u32 destination, u32 size);

u8 UpdatePuffyWario(void)
{
    return GF(sPuffyWarioPoseTable[gWarioData.pose])();
}

u8 PuffyWarioTransforming(void)
{
    const struct PuffyAnimationFrame* animation;
    if ((u8)(gWarioData.unk_1F - 2) <= 1) {
        gWarioData.yPosition -= 8;
    }
    animation = sPuffyTransformingAnimation;
    if (gWarioData.unk_1E >= animation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (animation[gWarioData.unk_1F].time == 0) {
            return 1;
        }
    }
    return 0xFF;
}

u8 PuffyWarioFloating(void)
{
    struct WarioData* wario;
    register int held PIN("r2");
    int direction;
    register int temp PIN("r0");
    register struct WarioData* animationWario PIN("r2");
    const struct PuffyAnimationFrame* animation;
    u16* heldPtr;
    if (gButtonsPressed & 2) {
        return 2;
    }
    heldPtr = &gButtonsHeld;
    wario = &gWarioData;
    held = *heldPtr;
    direction = wario->horizontalDirection;
    temp = held;
    temp &= direction;
    if (temp != 0) {
        temp = 0x10;
        temp &= direction;
        if (temp != 0) {
            wario->xVelocity = 0x20;
        } else {
            wario->xVelocity = -0x20;
        }
    } else {
        direction ^= 0x30;
        temp = direction;
        temp &= held;
        temp <<= 16;
        if (temp != 0) {
            wario->horizontalDirection = direction;
            m4aSongNumStart(SE_PUFFY_TURN);
        }
    }
    animationWario = &gWarioData;
    animation = sPuffyFloatingAnimation;
    if (animationWario->unk_1E >= animation[animationWario->unk_1F].time) {
        animationWario->unk_1E = 0;
        animationWario->unk_1F++;
        if (animation[animationWario->unk_1F].time == 0) {
            animationWario->unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 PuffyWarioInhaling(void)
{
    const struct PuffyAnimationFrame* animation;
    if (!(gButtonsHeld & 2) || gWarioData.yVelocity <= 24) {
        return 1;
    }
    animation = sPuffyInhalingAnimation;
    if (gWarioData.unk_1E >= animation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (animation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 PuffyWarioHitCeiling(void)
{
    struct WarioData* wario;
    const struct PuffyAnimationFrame* animation;
    int frame;
    wario = &gWarioData;
    animation = sPuffyHitCeilingAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        frame = wario->unk_1F + 1;
        wario->unk_1F = frame;
        if ((u8)frame == 3) {
            wario->yPosition -= 16;
        } else if (animation[wario->unk_1F].time == 0) {
            wario->reaction = 0;
            wario->damageTimer = 0x60;
            wario->yPosition += 0x40;
            return 2;
        }
    }
    return 0xFF;
}

void SetPuffyWarioPose(u8 pose)
{
    int value;
    u16 copy;
    u8 temp;

    value = pose;
    copy = value;
    temp = value + 3;
    if (temp <= 1) {
        goto end;
    }

    ResetWarioState();
    gWarioData.pose = value;

    if (value == 1) {
        goto pose1;
    }
    if (value > 1) {
        goto greater;
    }
    if (value == 0) {
        goto pose0;
    }
    goto end;

greater:
    if (copy == 2) {
        goto pose2;
    }
    if (copy == 3) {
        goto pose3;
    }
    goto end;

pose0:
    m4aSongNumStart(SE_PUFFY_TRANSFORM);
    goto end;

pose1:
    gWarioData.yVelocity = 24;
    goto end;

pose2:
    gWarioData.yVelocity = 108;
    m4aSongNumStart(SE_PUFFY_INHALE);
    goto end;

pose3:
    m4aSongNumStart(SE_PUFFY_HIT_CEILING);

end:
    return;
}

void UpdatePuffyWarioPosition(void)
{
    register u8* hitbox PIN("r3");
    register const u8* data PIN("r2");
    struct WarioData* wario;
    const u8* ptr;
    u32 index;
    u16 yVelocity, velocity, xMovement;
    hitbox = &gWarioCollisionData;
    data = sPuffyPoseData;
    wario = &gWarioData;
    index = wario->pose * 8;
    ptr = data + 1;
    index += (u32)ptr;
    hitbox[8] = *(const u8*)index;
    index = wario->pose * 8;
    ptr = data + 2;
    index += (u32)ptr;
    hitbox[9] = *(const u8*)index;
    index = wario->pose * 8;
    data += 3;
    index += (u32)data;
    hitbox[10] = *(const u8*)index;
    yVelocity = wario->yVelocity;
    velocity = (u16)(((s16)yVelocity) >> 3);
    if (wario->pose == 2) {
        wario->yVelocity = yVelocity - 4;
    }
    wario->yPosition -= velocity;
    xMovement = (u16)(((s16)wario->xVelocity) >> 3);
    wario->xPosition += xMovement;
}

void UpdatePuffyWarioCollision(void)
{
    u8* hitbox;
    register const u8* data PIN("r2");
    struct WarioData* wario;
    const u8* ptr;
    u32 index;
    hitbox = &gWarioCollisionData;
    data = sPuffyPoseData;
    wario = &gWarioData;
    index = wario->pose * 8;
    ptr = data + 4;
    index += (u32)ptr;
    hitbox[11] = *(const u8*)index;
    index = wario->pose * 8;
    ptr = data + 5;
    index += (u32)ptr;
    hitbox[12] = *(const u8*)index;
    index = wario->pose * 8;
    ptr = data + 6;
    index += (u32)ptr;
    hitbox[13] = *(const u8*)index;
    index = wario->pose * 8;
    data += 7;
    index += (u32)data;
    hitbox[17] = *(const u8*)index;
    ResolveWarioFloorCollision();
    if (wario->pose != 3 && hitbox[19] != 0) {
        SetPuffyWarioPose(3);
    }
}

void LoadPuffyWarioGraphics(int index)
{
    u32 value;
    GBA_TYPEOF(const struct PuffyGraphicsFrame**, &(sPuffyGraphicsTable)[0]) table;
    struct WarioData* wario;
    const struct PuffyGraphicsFrame* frame;
    const u8* data;
    value = (u8)index;
    UpdateWarioPositionHistory();
    table = sPuffyGraphicsTable;
    value <<= 2;
    wario = &gWarioData;
    value += wario->pose << 3;
    value += (u32)table;
    frame = GP(*(GBA_PTR(const struct PuffyGraphicsFrame*) *)value);
    frame += wario->unk_1F;
    data = GP(frame->data);
    wario->objData1Size = data[0] << 5;
    data++;
    wario->objData2Size = data[0] << 5;
    data++;
    GP_SET(wario->pObjData1, (u8*)data);
    GP_SET(wario->pObjData2, (u8*)data + wario->objData1Size);
    GP_SET(wario->pOamData, GP(frame->oam));
    gWarioPaletteSize = 0x20;
    CopyWarioPalette(sPuffyPalette, 0, 16);
}

void ApplyPuffyWarioMusicEffect(void)
{
    s8* state;
    void* player0;
    void* player1;
    u16 tracks;

    state = &gWarioMusicState;
    if (*state != 12) {
        player0 = GP(gMPlayTable[0].info);
        m4aMPlayTempoControl(player0, 200);
        player1 = GP(gMPlayTable[1].info);
        m4aMPlayTempoControl(player1, 200);
        tracks = 0xFFFF;
        m4aMPlayPitchControl(player0, tracks, 200);
        m4aMPlayPitchControl(player1, tracks, 200);
        m4aMPlayModDepthSet(player0, tracks, 10);
        m4aMPlayModDepthSet(player1, tracks, 10);
        m4aMPlayLFOSpeedSet(player0, tracks, 20);
        m4aMPlayLFOSpeedSet(player1, tracks, 20);
        *state = 12;
    }
}

void UpdatePuffyWarioHitbox(void)
{
    const u8* data;
    struct WarioData* wario;
    const u8* hitboxes;
    u32 offset;
    u32 address;
    u32 poseOffset;
    data = sPuffyPoseData;
    wario = &gWarioData;
    offset = data[wario->pose * 8];
    hitboxes = (const u8*)sWarioHitboxes;
    offset <<= 3;
    address = offset + (u32)hitboxes;
    wario->hitboxOffsetLeft = *(const u16*)address;
    address = (u32)(hitboxes + 2);
    address = offset + address;
    wario->hitboxOffsetTop = *(const u16*)address;
    address = (u32)(hitboxes + 4);
    address = offset + address;
    wario->hitboxOffsetRight = *(const u16*)address;
    hitboxes += 6;
    offset += (u32)hitboxes;
    wario->hitboxOffsetBottom = *(const u16*)offset;
    poseOffset = wario->pose * 8;
    data += 7;
    poseOffset += (u32)data;
    wario->unk_1A = *(const u8*)poseOffset;
}

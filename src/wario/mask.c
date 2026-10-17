#include "bg_clip.h"
#include "sound.h"
#include "voice_set.h"
#include "wario/swimming.h"
#include "wario/mask.h"
#include "wario_palette.h"

u8 UpdateMaskWario(void)
{
    return GF(sWarioMaskPoseTable[gWarioData.pose])();
}

u8 MaskWarioTransforming(void)
{
    int newTimer;
    u16 timer;

    {
        u16 oldTimer;
        oldTimer = gWarioData.unk_0C;
        if (oldTimer > 0x120) {
            return 1;
        }

        newTimer = oldTimer + 1;
    }
    gWarioData.unk_0C = newTimer;
    if ((u16)newTimer == 10) {
        m4aSongNumStart(SE_MASK_WARIO_TRANSFORM_START);
    }

    timer = gWarioData.unk_0C;
    if (timer > 10) {
        gWarioData.yPosition -= 8;
        if (gButtonsPressed & (DPAD_LEFT | DPAD_RIGHT)) {
            gWarioData.unk_0C = timer + 6;
        }
    }

    if (gWarioData.unk_0A <= 139) {
        gWarioData.unk_0A++;
    }
    if (gWarioData.unk_0A == 130) {
        m4aSongNumStart(SE_MASK_WARIO_TRANSFORM_END);
    }

    if (gWarioData.unk_1E >= sMaskWarioTransformAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sMaskWarioTransformAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_1F = 0;
        }
    }

    return 0xFF;
}

u8 MaskWarioAirborne(void)
{
    gWarioData.yVelocity -= 8;
    return 0xFF;
}

void SetMaskWarioPose(u8 pose)
{
    if ((u8)(pose + 3) > 1) {
        ResetWarioState();
        gWarioData.pose = pose;
        if (pose == 0) {
            VoiceSetPlay(VS_WARIO_SCREAMS);
        }
    }
}

void UpdateMaskWarioMotion(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u16 yOffset;

    collision = &gWarioCollisionData;
    config = &sMaskWarioPoseProperties[0][0];
    wario = &gWarioData;

    {
        u32 poseOffset;
        const u8 *field;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        field = config + 1;
        poseOffset += (u32)field;
        collision->unk_08 = *(const u8 *)poseOffset;
    }
    {
        u32 poseOffset;
        const u8 *field;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        field = config + 2;
        poseOffset += (u32)field;
        collision->unk_09 = *(const u8 *)poseOffset;
    }
    {
        u32 poseOffset;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        config += 3;
        poseOffset += (u32)config;
        collision->unk_0A = *(const u8 *)poseOffset;
    }

    yOffset = 0;
    if (wario->pose == 1) {
        u16 velocity;
        velocity = wario->yVelocity;
        yOffset = (s16)velocity >> 3;
        wario->yVelocity = velocity - 8;
    }

    if (wario->yVelocity < -128) {
        wario->yVelocity = -128;
    }

    wario->yPosition -= yOffset;
}

void ProcessMaskWarioCollision(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u8 movementResult;
    u16 flags;
    int collisionResult;

    collision = &gWarioCollisionData;
    config = &sMaskWarioPoseProperties[0][0];
    wario = &gWarioData;

    {
        u32 poseOffset;
        const u8 *field;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        field = config + 4;
        poseOffset += (u32)field;
        collision->unk_0B = *(const u8 *)poseOffset;
    }
    {
        u32 poseOffset;
        const u8 *field;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        field = config + 5;
        poseOffset += (u32)field;
        collision->unk_0C = *(const u8 *)poseOffset;
    }
    {
        u32 poseOffset;
        const u8 *field;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        field = config + 6;
        poseOffset += (u32)field;
        collision->unk_0D = *(const u8 *)poseOffset;
    }
    {
        u32 poseOffset;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        config += 7;
        poseOffset += (u32)config;
        collision->unk_11 = *(const u8 *)poseOffset;
    }

    movementResult = 0xFF;
    flags = collision->flags;
    if (flags & 0x40) {
        movementResult = ResolveWarioFloorCollision();
    } else {
        s32 mask;
        mask = 0x80;
        mask &= flags;
        if (mask != 0) {
            movementResult = ResolveWarioCeilingCollision();
        }
    }

    collisionResult = GetBackgroundCollisionAtPosition(gWarioData.yPosition - 0x30, gWarioData.xPosition);
    collisionResult &= 0xFF;
    if (collisionResult == 1) {
        gWarioData.reaction = collisionResult;
        gWarioData.damageTimer = 0x60;
        SetWarioWaterPose(0);
    } else if (movementResult == 0xFD) {
        gWarioData.reaction = 0;
        gWarioData.damageTimer = 0x60;
        ApplyNormalWarioPoseTransition(0x2F);
    }
}

void LoadMaskWarioGraphics(u8 variant)
{
    const struct MaskWarioGraphicsFrame *frame;
    const u8 *gfx;

    UpdateWarioPositionHistory();

    frame = GP(sMaskWarioGraphicsTable[gWarioData.pose][variant]);
    frame += gWarioData.unk_1F;
    gfx = GP(frame->gfx);
    gWarioData.objData1Size = gfx[0] << 5;
    gfx++;
    gWarioData.objData2Size = gfx[0] << 5;
    gfx++;
    GP_SET(gWarioData.pObjData1, (u8 *)gfx);
    GP_SET(gWarioData.pObjData2, (u8 *)gfx + gWarioData.objData1Size);
    GP_SET(gWarioData.pOamData, GP(frame->oam));
    gWarioPaletteSize = 0x20;
    CopyWarioPalette(sMaskWarioPalette, 0, 0x10);
}

void UpdateMaskWarioAnimation(void)
{
}

void UpdateMaskWarioHitbox(void)
{
    const u8 *config;
    struct WarioData *wario;
    const u8 *hitboxes;
    u32 hitboxOffset;
    u8 direction;

    config = &sMaskWarioPoseProperties[0][0];
    wario = &gWarioData;

    {
        u32 poseOffset;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        poseOffset += (u32)config;
        hitboxOffset = *(const u8 *)poseOffset;
    }
    hitboxes = (const u8 *)&sWarioHitboxes[0][0];
    hitboxOffset <<= 3;
    {
        const u8 *address;
        address = (const u8 *)(hitboxOffset + (u32)hitboxes);
        wario->hitboxOffsetLeft = *(const s16 *)address;
        address = hitboxes + 2;
        address = (const u8 *)(hitboxOffset + (u32)address);
        wario->hitboxOffsetTop = *(const s16 *)address;
        address = hitboxes + 4;
        address = (const u8 *)(hitboxOffset + (u32)address);
        wario->hitboxOffsetRight = *(const s16 *)address;
    }
    hitboxes += 6;
    hitboxOffset += (u32)hitboxes;
    wario->hitboxOffsetBottom = *(const s16 *)hitboxOffset;

    {
        u32 poseOffset;
        poseOffset = wario->pose;
        poseOffset <<= 3;
        config += 7;
        poseOffset += (u32)config;
        direction = *(const u8 *)poseOffset;
    }
    if (direction == 2) {
        wario->unk_1A = direction;
    } else if (wario->unk_1A != 1) {
        wario->unk_1A = direction;
    }
}

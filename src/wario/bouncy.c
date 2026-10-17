#include "wario/bouncy.h"
#include "bg_clip.h"
#include "global_data.h"
#include "input.h"
#include "main.h"
#include "sound.h"
#include "voice_set.h"
#include "wario.h"
#include "wario/normal.h"
#include "wario/swimming.h"
#include "wario_palette.h"
#include "gba/m4a.h"

u8 UpdateBouncyWario(void)
{
    return GF(sBouncyWarioPoseTable[gWarioData.pose])();
}

u8 BouncyWarioTransforming(void)
{
    if (gWarioData.unk_1E >= sBouncyWarioTransformingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_08 = 1;
        return 8;
    }

    return 0xFF;
}

u8 BouncyWarioUnknown1(void)
{
    if (gWarioData.unk_1E >= sBouncyWarioTransformingAnimation[gWarioData.unk_1F].time)
        return 8;

    return 0xFF;
}

u8 BouncyWarioJumping(void)
{
    struct WarioData* movementWario;
    register u16 held PIN("r2");
    register u16 direction PIN("r1");
    register u16 common PIN("r4");

    if (gWarioData.pose == WPOSE_BOUNCY_TRANSFORMING_2) {
        if (gWarioData.unk_0C > 299)
            return 0xFE;
        gWarioData.unk_0C++;
    } else if (gWarioData.unk_1E >= sBouncyWarioJumpingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sBouncyWarioJumpingAnimation[gWarioData.unk_1F].time == 0)
            gWarioData.unk_1F = 0;
    }

    {
        const u16* heldPointer;

        heldPointer = &gButtonsHeld;
        movementWario = &gWarioData;
        held = *heldPointer;
    }
    direction = movementWario->horizontalDirection;
    common = held;
    common &= direction;
    if (common != 0) {
        if ((direction & 0x10) != 0)
            movementWario->xVelocity = 32;
        else
            movementWario->xVelocity = -32;
    } else {
        register u32 temp PIN("r0");

        direction ^= 0x30;
        temp = direction & held;
        temp <<= 16;
        held = temp >> 16;
        if (held != 0) {
            movementWario->horizontalDirection = direction;
            movementWario->xVelocity = common;
        } else if (gWarioData.pose == WPOSE_BOUNCY_TRANSFORMING_2) {
            gWarioData.xVelocity = held;
        }
    }

    return 0xFF;
}

u8 BouncyWarioHitCeiling(void)
{
    if (gWarioData.unk_1E >= sBouncyWarioHittingCeilingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sBouncyWarioHittingCeilingAnimation[gWarioData.unk_1F].time == 0)
            return 0xFE;
    }

    return 0xFF;
}

u8 BouncyWarioStartJump(void)
{
    if (gWarioData.unk_1E >= sBouncyWarioStartingJumpAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sBouncyWarioStartingJumpAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_08 = 1;
            return 0xFE;
        }
    }

    return 0xFF;
}

u8 BouncyWarioFallFromCeiling(void)
{
    return 0xFF;
}

u8 BouncyWarioFinalLanding(void)
{
    if (gWarioData.unk_1E >= sBouncyWarioFinalLandingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sBouncyWarioFinalLandingAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_08 = 1;
            return 0xFE;
        }
    }

    return 0xFF;
}

u8 BouncyWarioFinalBounce(void)
{
    if ((gWarioData.horizontalDirection & 0x10) != 0)
        gWarioData.xVelocity = 32;
    else
        gWarioData.xVelocity = -32;

    if (gWarioData.unk_1E >= sBouncyWarioFinalBounceAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sBouncyWarioFinalBounceAnimation[gWarioData.unk_1F].time == 0)
            gWarioData.unk_1F--;
    }

    return 0xFF;
}

u8 BouncyWarioHopping(void)
{
    if (gWarioData.unk_1F <= 1 && (gButtonsHeld & 1) != 0) {
        gWarioData.pose = WPOSE_BOUNCY_STARTING_JUMP;
        gWarioData.unk_1F = 3;
    } else if (gWarioData.unk_1E >= sBouncyWarioHoppingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sBouncyWarioHoppingAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_08 = 1;
            return 0xFE;
        }
    }

    return 0xFF;
}

void SetBouncyWarioPose(u8 result)
{
    if (gWarioData.pose == WPOSE_BOUNCY_FINAL_BOUNCE && result == 0xFD) {
        gWarioData.reaction = REACTION_NORMAL;
        gWarioData.damageTimer = 0x60;
        ApplyNormalWarioPoseTransition(2);
        return;
    }

    ResetWarioState();
    if (result == 0) {
        if (gWarioDataCopy.unk_1A == 2)
            result = 1;
        m4aSongNumStart(SE_WARIO_TRANSFORM_LOOP);
    } else {
        gWarioData.unk_0C = gWarioDataCopy.unk_0C;
    }

    switch (result) {
        case 0xFE:
            if (gWarioDataCopy.pose == WPOSE_BOUNCY_HOPPING) {
                gWarioData.pose = WPOSE_BOUNCY_TRANSFORMING_2;
                gWarioData.yVelocity = 0x80;
                m4aSongNumStart(SE_BOUNCY_WARIO_BOUNCE);
            } else if (gWarioDataCopy.pose == WPOSE_BOUNCY_STARTING_JUMP) {
                gWarioData.pose = WPOSE_BOUNCY_JUMPING;
                gWarioData.yVelocity = 0x80;
                gCurrentWarioEffect.type = 2;
                m4aSongNumStart(SE_BOUNCY_WARIO_JUMP);
            } else if (gWarioDataCopy.pose == WPOSE_BOUNCY_FINAL_LANDING) {
                gWarioData.pose = WPOSE_BOUNCY_FINAL_BOUNCE;
                gWarioData.yVelocity = 0x80;
            } else {
                gWarioData.pose = WPOSE_BOUNCY_FALLING_FROM_CEILING;
            }
            if (gWarioDataCopy.unk_08 == 0)
                gWarioData.yVelocity = 0;
            break;

        case 0xFD:
            if (gWarioData.pose == WPOSE_BOUNCY_FALLING_FROM_CEILING) {
                gWarioData.pose = WPOSE_BOUNCY_FINAL_LANDING;
                m4aSongNumStart(SE_BOUNCY_WARIO_BOUNCE);
            } else {
                gWarioData.pose = WPOSE_BOUNCY_HOPPING;
            }
            if (gWarioData.unk_1A == 1)
                gWarioData.yPosition--;
            break;

        case 3:
            m4aSongNumStart(SE_BOUNCY_WARIO_HIT_CEILING);
            VoiceSetPlay(4);
        default:
            gWarioData.pose = result;
            break;
    }
}

void UpdateBouncyWarioMovement(void)
{
    struct WarioData* wario;
    u16 movement;
    u16 xMovement;

    {
        register struct WarioCollisionData* collision PIN("r4");
        const u8* properties;
        register const u8* property PIN("r1");
        u32 offset;

        collision = &gWarioCollisionData;
        properties = (const u8*)sUnk_82DDB60;
        wario = &gWarioData;

        offset = wario->pose * 8;
        property = properties + 1;
        offset += (u32)property;
        collision->unk_08 = *(const u8*)offset;
        offset = wario->pose * 8;
        property = properties + 2;
        offset += (u32)property;
        collision->unk_09 = *(const u8*)offset;
        offset = wario->pose * 8;
        properties += 3;
        offset += (u32)properties;
        collision->unk_0A = *(const u8*)offset;
    }

    movement = 0;
    if (wario->unk_1A == 2) {
        u16 velocity;
        u32 temp;

        velocity = wario->yVelocity;
        temp = (u32)(u16)velocity << 16;
        temp = (s32)temp >> 19;
        movement = (u16)temp;
        if (wario->pose != WPOSE_BOUNCY_JUMPING) {
            temp = velocity;
            temp -= 8;
            wario->yVelocity = temp;
        }
    }
    {
        register struct WarioData* wario2 PIN("r2");

        wario2 = wario;
        if (wario2->yVelocity < -128)
            wario2->yVelocity = -128;
    }

    wario->yPosition -= movement;
    xMovement = (u16)(wario->xVelocity >> 3);
    wario->xPosition += xMovement;
}

void ProcessBouncyWarioCollision(void)
{
    u8 result;

    {
        struct WarioCollisionData* collision;
        const u8* properties;
        struct WarioData* wario;
        register const u8* property PIN("r1");
        u32 offset;

        collision = &gWarioCollisionData;
        properties = (const u8*)sUnk_82DDB60;
        wario = &gWarioData;

        offset = wario->pose * 8;
        property = properties + 4;
        offset += (u32)property;
        collision->unk_0B = *(const u8*)offset;
        offset = wario->pose * 8;
        property = properties + 5;
        offset += (u32)property;
        collision->unk_0C = *(const u8*)offset;
        offset = wario->pose * 8;
        property = properties + 6;
        offset += (u32)property;
        collision->unk_0D = *(const u8*)offset;
        offset = wario->pose * 8;
        properties += 7;
        offset += (u32)properties;
        collision->unk_11 = *(const u8*)offset;

        if (wario->yVelocity <= 47)
            collision->unk_0C = 0;

        result = 0xFF;
        if ((collision->flags & 0x40) != 0)
            result = ResolveWarioFloorCollision();
        else if ((collision->flags & 0x80) != 0)
            result = ResolveWarioCeilingCollision();
        else if (collision->unk_00 != 0) {
            if (collision->unk_11 == 2)
                result = ResolveWarioCeilingCollision();
            else
                result = ResolveWarioStandardCollision();
        } else if (collision->unk_11 == 0) {
            result = ResolveWarioLandingCollision();
        }
    }

    {
        struct WarioData* wario;
        s32 collisionResult;

        wario = &gWarioData;
        collisionResult = GetBackgroundCollisionAtPosition(wario->yPosition - 48, wario->xPosition);
        if ((collisionResult & 0xFF) == 1) {
            wario->reaction = REACTION_WATER;
            wario->damageTimer = 0x60;
            SetWarioWaterPose(0);
            return;
        }

        if (gWarioCollisionData.unk_11 == 0xFF)
            return;

        if (wario->pose == WPOSE_BOUNCY_FINAL_BOUNCE && result == 0xFD) {
            wario->reaction = REACTION_NORMAL;
            wario->damageTimer = 0x60;
            ApplyNormalWarioPoseTransition(2);
            return;
        }

        if (gWarioCollisionData.unk_13 != 0 && gWarioData.pose == WPOSE_BOUNCY_JUMPING)
            result = 3;
    }

    if (result != 0xFF)
        SetBouncyWarioPose(result);
}

void UpdateBouncyWarioGraphics(u8 variant)
{
    const struct WarioAnimationFrame* frames;
    const struct WarioAnimationFrame* frame;
    const u8* data;
    const u16* palette;

    UpdateWarioPositionHistory();
    frames = GP(sUnk_82DDB10[gWarioData.pose][variant]);
    frame = &frames[gWarioData.unk_1F];
    data = GP(frame->objData);
    gWarioData.objData1Size = *data << 5;
    data++;
    gWarioData.objData2Size = *data << 5;
    data++;
    GP_SET(gWarioData.pObjData1, (u8*)data);
    GP_SET(gWarioData.pObjData2, (u8*)(data + gWarioData.objData1Size));
    GP_SET(gWarioData.pOamData, GP(frame->oamData));
    gWarioPaletteSize = 0x40;

    if (gWarioData.unk_0C > 240) {
        palette = sBouncyWarioPalette;
        if ((gMainTimer & 4) != 0)
            palette += 16;
        else
            palette += 32;
    } else if (gWarioData.unk_0C > 180) {
        if ((gMainTimer & 4) != 0) {
            palette = sBouncyWarioPalette;
            palette += 16;
        } else {
            palette = sWarioDefaultObjPalette;
        }
    } else {
        palette = sWarioDefaultObjPalette;
    }

    CopyWarioPalette(palette, 0, 16);
    {
        register const u16* secondPalette PIN("r2");

        secondPalette = sBouncyWarioPalette;
        CopyWarioPalette(secondPalette, 16, 16);
    }
}

void ApplyBouncyWarioMusicEffect(void)
{
    struct MusicPlayerInfo* player0;
    struct MusicPlayerInfo* player3;

    if (gWarioMusicState == 11)
        return;

    player0 = GP(gMPlayTable[0].info);
    m4aMPlayTempoControl(player0, 300);
    player3 = GP(gMPlayTable[1].info);
    m4aMPlayTempoControl(player3, 300);
    m4aMPlayPitchControl(player0, 0xFFFF, 150);
    m4aMPlayPitchControl(player3, 0xFFFF, 150);
    m4aMPlayModDepthSet(player0, 0xFFFF, 20);
    m4aMPlayModDepthSet(player3, 0xFFFF, 20);
    m4aMPlayLFOSpeedSet(player0, 0xFFFF, 40);
    m4aMPlayLFOSpeedSet(player3, 0xFFFF, 40);
    gWarioMusicState = 11;
}

void UpdateBouncyWarioHitbox(void)
{
    const u8* properties;
    struct WarioData* wario;
    const u8* hitboxes;
    u32 hitboxOffset;
    const u8* address;
    u8 movementType;

    properties = (const u8*)sUnk_82DDB60;
    wario = &gWarioData;
    hitboxOffset = properties[wario->pose * 8];
    hitboxes = (const u8*)sWarioHitboxes;
    hitboxOffset <<= 3;

    address = (const u8*)hitboxOffset + (u32)hitboxes;
    wario->hitboxOffsetLeft = *(const u16*)address;
    address = hitboxes + 2;
    address = (const u8*)hitboxOffset + (u32)address;
    wario->hitboxOffsetTop = *(const u16*)address;
    address = hitboxes + 4;
    address = (const u8*)hitboxOffset + (u32)address;
    wario->hitboxOffsetRight = *(const u16*)address;
    hitboxes += 6;
    hitboxOffset += (u32)hitboxes;
    wario->hitboxOffsetBottom = *(const u16*)hitboxOffset;

    address = (const u8*)(wario->pose * 8);
    properties += 7;
    address = (const u8*)((u32)address + (u32)properties);
    movementType = *address;
    if (movementType == 2)
        wario->unk_1A = movementType;
    else if (wario->unk_1A != 1)
        wario->unk_1A = movementType;
}

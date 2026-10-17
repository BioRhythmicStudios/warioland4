#include "wario/snowman.h"

#include "bg_clip.h"
#include "block.h"
#include "screen_shake.h"
#include "sound.h"
#include "voice_set.h"
#include "wario/swimming.h"
#include "wario_palette.h"


extern s32 CheckWarioPointCollision(u8, u16, void*);
extern u8 ResolveWarioStandardCollision(void);
extern u8 ResolveWarioLandingCollision(void);

extern const struct AnimationFrameU16 sUnk_82E04C4[];

u8 UpdateSnowmanWario(void)
{
    return GF(sSnowmanWarioPoseTable[gWarioData.pose])();
}

u8 SnowmanWarioTransforming(void)
{
    if (gWarioData.unk_1E >= sSnowmanWarioTransformingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioTransformingAnimation[gWarioData.unk_1F].time == 0)
            return WPOSE_SNOWMAN_LANDING_AFTER_MIDAIR_SNOW_CONTACT;
    }
    return 0xFF;
}

u8 SnowmanWarioFallingWithSnow(void)
{
    return 0xFF;
}

u8 SnowmanWarioWalking(void)
{
    struct WarioData* wario;
    register u32 held PIN("r3");
    register u32 direction PIN("r2");
    u32 common;

    wario = &gWarioData;
    if ((wario->unk_1C & 1) != 0) {
        if ((GetWarioBlockCollisionAtPosition((u16)(wario->yPosition + 20), wario->xPosition) & 0x01000000) == 0) {
            wario->horizontalDirection = (wario->unk_1C ^ 0x30) & 0x30;
            return WPOSE_SNOWMAN_ROLLING_SMALL;
        }
    }

    if ((gButtonsPressed & 1) != 0)
        return WPOSE_SNOWMAN_JUMPING;

    {
        const u16* heldPointer;
        struct WarioData* movementWario;

        heldPointer = &gButtonsHeld;
        movementWario = &gWarioData;
        held = *heldPointer;
        direction = movementWario->horizontalDirection;
        common = held;
        common &= direction;
        wario = movementWario;
    }

    if (common != 0) {
        if ((direction & 0x10) != 0)
            wario->xVelocity = 32;
        else
            wario->xVelocity = -32;
    } else {
        u32 temp;

        temp = 0x30;
        temp ^= direction;
        temp &= held;
        temp <<= 16;
        if (temp != 0)
            return WPOSE_SNOWMAN_TURNING;
        return WPOSE_SNOWMAN_UNKNOWN_8;
    }

    {
        register struct WarioData* animationWario PIN("r2");
        register const struct WarioAnimationFrame* animation PIN("r3");

        animationWario = wario;
        animation = sSnowmanWarioWalkingAnimation;
        if (animationWario->unk_1E >= animation[animationWario->unk_1F].time) {
            animationWario->unk_1E = 0;
            if (++animationWario->unk_1F == 1)
                m4aSongNumStart(SE_WARIO_STEP);
            else if (animation[wario->unk_1F].time == 0)
                wario->unk_1F = 0;
        }
    }

    return 0xFF;
}

u8 SnowmanWarioBonk(void)
{
    if (gWarioData.unk_1E >= sSnowmanWarioBonkAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioBonkAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.reaction = 0;
            gWarioData.damageTimer = 0x60;
            return WPOSE_SNOWMAN_WALKING;
        }
    }
    return 0xFF;
}

u8 SnowmanWarioJumping(void)
{
    if (gWarioData.unk_1E >= sSnowmanWarioJumpingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioJumpingAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_08 = 1;
            return 0xFE;
        }
    }
    return 0xFF;
}

u8 SnowmanWarioFalling(void)
{
    const u16* heldPointer;
    struct WarioData* wario;
    register u16 held PIN("r2");
    register u16 direction PIN("r1");
    register u16 common PIN("r3");

    heldPointer = &gButtonsHeld;
    wario = &gWarioData;
    held = *heldPointer;
    direction = wario->horizontalDirection;
    common = held;
    common &= direction;
    if (common != 0) {
        if ((direction & 0x10) != 0)
            wario->xVelocity = 32;
        else
            wario->xVelocity = -32;
    } else {
        direction ^= 0x30;
        if ((u16)(direction & held) != 0)
            wario->horizontalDirection = direction;
        wario->xVelocity = common;
    }
    return 0xFF;
}

u8 SnowmanWarioLanding(void)
{
    if (gWarioData.unk_1E > 29)
        return WPOSE_SNOWMAN_UNKNOWN_8;

    if (gWarioMotionAfterimage.unk1 >= sUnk_82E04C4[gWarioMotionAfterimage.unk2].time) {
        gWarioMotionAfterimage.unk1 = 0;
        gWarioMotionAfterimage.unk2++;
        if (sUnk_82E04C4[gWarioMotionAfterimage.unk2].time == 0)
            gWarioMotionAfterimage.unk2 = 0;
    }
    return 0xFF;
}

u8 SnowmanWarioLandingAfterMidairSnowContact(void)
{
    if (gWarioData.unk_1E >= sSnowmanWarioLandingAfterMidairSnowContactAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioLandingAfterMidairSnowContactAnimation[gWarioData.unk_1F].time == 0)
            return WPOSE_SNOWMAN_UNKNOWN_8;
    }
    return 0xFF;
}

u8 SnowmanWarioUnknown8(void)
{
    if ((gWarioData.unk_1C & 1) != 0) {
        gWarioData.horizontalDirection = (gWarioData.unk_1C ^ 0x30) & 0x30;
        return WPOSE_SNOWMAN_ROLLING_SMALL;
    }
    if ((gButtonsPressed & 1) != 0)
        return WPOSE_SNOWMAN_JUMPING;
    if ((gButtonsHeld & gWarioData.horizontalDirection) != 0)
        return WPOSE_SNOWMAN_WALKING;
    if (((gWarioData.horizontalDirection ^ 0x30) & gButtonsHeld) != 0)
        return WPOSE_SNOWMAN_TURNING;
    return 0xFF;
}

u8 SnowmanWarioTurning(void)
{
    if (gWarioData.unk_1E >= sSnowmanWarioTurningAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioTurningAnimation[gWarioData.unk_1F].time == 0)
            return WPOSE_SNOWMAN_UNKNOWN_8;
    }
    return 0xFF;
}

u8 SnowmanWarioRollingSmall(void)
{
    if ((gWarioData.horizontalDirection & 0x10) != 0)
        gWarioData.xVelocity = 64;
    else
        gWarioData.xVelocity = -64;

    if (gWarioData.unk_1E >= sSnowmanWarioRollingSmallAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioRollingSmallAnimation[gWarioData.unk_1F].time == 0)
            return WPOSE_SNOWMAN_ROLLING_MEDIUM;
    }
    return 0xFF;
}

u8 SnowmanWarioRollingSmallMidair(void)
{
    if ((gWarioData.horizontalDirection & 0x10) != 0)
        gWarioData.xVelocity = 64;
    else
        gWarioData.xVelocity = -64;

    if (gWarioData.unk_1E >= sSnowmanWarioRollingSmallAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioRollingSmallAnimation[gWarioData.unk_1F].time == 0)
            return WPOSE_SNOWMAN_ROLLING_MEDIUM_MIDAIR;
    }
    return 0xFF;
}

u8 SnowmanWarioRollingMedium(void)
{
    if ((gWarioData.horizontalDirection & 0x10) != 0)
        gWarioData.xVelocity = 96;
    else
        gWarioData.xVelocity = -96;

    if (gWarioData.unk_0A > 47)
        return WPOSE_SNOWMAN_ROLLING_LARGE;
    gWarioData.unk_0A++;

    if (gWarioData.unk_1E >= sSnowmanWarioRollingMediumAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioRollingMediumAnimation[gWarioData.unk_1F].time == 0)
            gWarioData.unk_1F = 0;
    }
    return 0xFF;
}

u8 SnowmanWarioRollingMediumMidair(void)
{
    if ((gWarioData.horizontalDirection & 0x10) != 0)
        gWarioData.xVelocity = 96;
    else
        gWarioData.xVelocity = -96;

    if (gWarioData.unk_0A > 47)
        return WPOSE_SNOWMAN_ROLLING_LARGE_MIDAIR;
    gWarioData.unk_0A++;

    if (gWarioData.unk_1E >= sSnowmanWarioRollingMediumAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioRollingMediumAnimation[gWarioData.unk_1F].time == 0)
            gWarioData.unk_1F = 0;
    }
    return 0xFF;
}

u8 SnowmanWarioRollingLarge(void)
{
    if ((gWarioData.horizontalDirection & 0x10) != 0)
        gWarioData.xVelocity = 128;
    else
        gWarioData.xVelocity = -128;

    if (gWarioData.unk_1E >= sSnowmanWarioRollingLargeAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioRollingLargeAnimation[gWarioData.unk_1F].time == 0)
            gWarioData.unk_1F = 0;
    }
    return 0xFF;
}

u8 SnowmanWarioRollingLargeMidair(void)
{
    if ((gWarioData.horizontalDirection & 0x10) != 0)
        gWarioData.xVelocity = 128;
    else
        gWarioData.xVelocity = -128;

    if (gWarioData.unk_1E >= sSnowmanWarioRollingLargeAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sSnowmanWarioRollingLargeAnimation[gWarioData.unk_1F].time == 0)
            gWarioData.unk_1F = 0;
    }
    return 0xFF;
}

void SetSnowmanWarioPose(u8 result)
{
    ResetWarioState();
    if (result == 0) {
        if (gWarioDataCopy.unk_1A == 2)
            result = 1;
        m4aSongNumStart(SE_WARIO_TRANSFORM_LOOP);
        VoiceSetPlay(4);
    } else if (result == 0xFD) {
        const u8* properties;
        u32 offset;

        properties = (const u8*)sSnowmanWarioPoseProperties;
        offset = gWarioDataCopy.pose * 8;
        properties += 7;
        offset += (u32)properties;
        if (*(const u8*)offset != 2)
            return;
    }

    switch (result) {
        case 0xFE:
            if (gWarioDataCopy.pose == WPOSE_SNOWMAN_TRANSFORMING) {
                gWarioData.pose = WPOSE_SNOWMAN_FALLING_WITH_SNOW;
            } else if (gWarioDataCopy.pose > WPOSE_SNOWMAN_TURNING) {
                gWarioData.pose++;
                gWarioData.xVelocity = gWarioDataCopy.xVelocity;
            } else {
                gWarioData.pose = WPOSE_SNOWMAN_FALLING;
                if (gWarioDataCopy.unk_08 != 0)
                    gWarioData.yVelocity = 96;
            }
            break;
        case 0xFD:
            if (gWarioDataCopy.pose == WPOSE_SNOWMAN_FALLING_WITH_SNOW) {
                gWarioData.pose = WPOSE_SNOWMAN_LANDING_AFTER_MIDAIR_SNOW_CONTACT;
            } else if (gWarioDataCopy.pose == WPOSE_SNOWMAN_FALLING) {
                gWarioData.pose = WPOSE_SNOWMAN_LANDING;
                gWarioMotionAfterimage.unk0 = 4;
                ScreenShakeRequestY(64, 0);
                m4aSongNumStart(SE_SNOWMAN_WARIO_LAND);
            } else if (gWarioDataCopy.pose > WPOSE_SNOWMAN_TURNING) {
                gWarioData.pose--;
                gWarioData.xVelocity = gWarioDataCopy.xVelocity;
            }
            break;
        case WPOSE_SNOWMAN_BONK:
            gWarioData.pose = result;
            m4aSongNumStart(SE_WARIO_BONK);
            VoiceSetPlay(0);
            break;
        case WPOSE_SNOWMAN_ROLLING_SMALL:
            gWarioData.pose = result;
            if (gWarioDataCopy.pose <= WPOSE_SNOWMAN_TURNING)
                m4aSongNumStart(SE_SNOWMAN_WARIO_ROLL);
            break;
        case WPOSE_SNOWMAN_TURNING:
            gWarioData.unk_03 = 1;
        default:
            gWarioData.pose = result;
            break;
    }

    if ((u8)(gWarioData.pose - WPOSE_SNOWMAN_ROLLING_MEDIUM) <= 1)
        gWarioData.unk_0A = gWarioDataCopy.unk_0A;
}

void UpdateSnowmanWarioMovement(void)
{
    struct WarioData* wario;
    u32 movement;
    u32 xMovement;

    {
        struct WarioCollisionData* collision;
        const u8* properties;
        register const u8* property PIN("r1");
        u32 offset;

        collision = &gWarioCollisionData;
        properties = (const u8*)sSnowmanWarioPoseProperties;
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

    UpdateWarioHorizontalCollisionOffset();
    movement = 0;
    if (wario->unk_1A == 2) {
        u16 velocity;
        u32 temp;

        velocity = wario->yVelocity;
        temp = (u32)(u16)velocity << 16;
        temp = (s32)temp >> 19;
        movement = (u16)temp;
        velocity -= 8;
        wario->yVelocity = velocity;
    }

    if (wario->yVelocity < -128)
        wario->yVelocity = -128;

    wario->yPosition -= movement;
    if (wario->unk_1A == 0)
        xMovement = GetAdjustedWarioXVelocity();
    else
        xMovement = (u16)wario->xVelocity;

    xMovement <<= 16;
    xMovement = (s32)xMovement >> 19;
    movement = (u16)xMovement;
    /* Keep the completed movement in r2 before loading the position pointer. */
    ASM_BARRIER(: "+r"(movement));

    {
        struct WarioData* positionWario;

        positionWario = &gWarioData;
        positionWario->xPosition += movement;
    }
}

void ProcessSnowmanWarioCollision(void)
{
    u8 result;

    {
        struct WarioCollisionData* collision;
        const u8* properties;
        struct WarioData* wario;
        register const u8* property PIN("r1");
        u32 offset;

        collision = &gWarioCollisionData;
        properties = (const u8*)sSnowmanWarioPoseProperties;
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
        struct WarioCollisionData* collision;
        s32 collisionResult;

        wario = &gWarioData;
        collisionResult = GetBackgroundCollisionAtPosition(wario->yPosition - 48, wario->xPosition);
        if ((collisionResult & 0xFF) == 1) {
            wario->reaction = REACTION_WATER;
            wario->damageTimer = 0x60;
            SetWarioWaterPose(0);
            return;
        }

        {
            struct WarioCollisionData* collisionPointer;
            u8 movementType;

            collisionPointer = &gWarioCollisionData;
            movementType = collisionPointer->unk_11;
            collision = collisionPointer;
            if (movementType == 0xFF)
                return;
        }

        if (wario->pose > WPOSE_SNOWMAN_TURNING && result == 0xFC) {
            result = 3;
        } else {
            struct WarioData* collisionWario;

            collisionWario = &gWarioData;
            if (collisionWario->pose == WPOSE_SNOWMAN_FALLING && result == 0xFD) {
                u32 temp;
                u8 property;

                property = collision->unk_08;
                if (CheckWarioPointCollision(property, collisionWario->yPosition + 1, &temp) <= collision->unk_10)
                    result = 0xFF;
            }
        }
    }

    if (result != 0xFF)
        SetSnowmanWarioPose(result);
}

void UpdateSnowmanWarioGraphics(u8 variant)
{
    const struct WarioAnimationFrame* frames;
    const struct WarioAnimationFrame* frame;
    const u8* data;

    UpdateWarioPositionHistory();
    frames = GP(sSnowmanWarioAnimationTable[gWarioData.pose][variant]);
    frame = &frames[gWarioData.unk_1F];
    data = GP(frame->objData);
    gWarioData.objData1Size = *data << 5;
    data++;
    gWarioData.objData2Size = *data << 5;
    data++;
    GP_SET(gWarioData.pObjData1, (u8*)data);
    GP_SET(gWarioData.pObjData2, (u8*)(data + gWarioData.objData1Size));
    GP_SET(gWarioData.pOamData, GP(frame->oamData));
    gWarioPaletteSize = 0x60;
    CopyWarioPalette(sWarioDefaultObjPalette, 0, 16);
    CopyWarioPalette(sSnowmanWarioPalette, 16, 32);
}

void ApplySnowmanWarioMusicEffect(void)
{
    struct MusicPlayerInfo* player0;
    struct MusicPlayerInfo* player1;

    if (gWarioMusicState == 10)
        return;

    player0 = GP(gMPlayTable[0].info);
    m4aMPlayTempoControl(player0, 180);
    player1 = GP(gMPlayTable[1].info);
    m4aMPlayTempoControl(player1, 180);
    m4aMPlayPitchControl(player0, 0xFFFF, -500);
    m4aMPlayPitchControl(player1, 0xFFFF, -500);
    m4aMPlayModDepthSet(player0, 0xFFFF, 20);
    m4aMPlayModDepthSet(player1, 0xFFFF, 20);
    m4aMPlayLFOSpeedSet(player0, 0xFFFF, 20);
    m4aMPlayLFOSpeedSet(player1, 0xFFFF, 20);
    gWarioMusicState = 10;
}

void UpdateSnowmanWarioHitbox(void)
{
    const u8* properties;
    struct WarioData* wario;
    const u8* hitboxes;
    u32 hitboxOffset;
    const u8* address;
    u8 movementType;

    properties = (const u8*)sSnowmanWarioPoseProperties;
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

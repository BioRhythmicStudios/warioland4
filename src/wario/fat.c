#include "bg_clip.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "input.h"
#include "main.h"
#include "screen_shake.h"
#include "sound.h"
#include "voice_set.h"
#include "wario.h"
#include "wario/normal.h"
#include "wario/swimming.h"
#include "wario/fat.h"
#include "wario_palette.h"

u8 UpdateFatWario(void)
{
    return GF(sFatWarioPoseTable[gWarioData.pose])();
}

u8 FatWarioTransforming(void)
{
    if (gWarioData.unk_1E >= sFatWarioTransformingFrames[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFatWarioTransformingFrames[gWarioData.unk_1F].time == 0) {
            return 9;
        }
    }
    return 0xFF;
}

u8 FatWarioTransformingMidair(void)
{
    if (gWarioData.unk_1E >= sFatWarioTransformingFrames[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFatWarioTransformingFrames[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_1F--;
        }
    }
    return 0xFF;
}

u8 FatWarioTurning(void)
{
    if (gWarioData.unk_1E >= sFatWarioTurningFrames[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFatWarioTurningFrames[gWarioData.unk_1F].time == 0) {
            return 9;
        }
    }
    return 0xFF;
}

u8 FatWarioWalking(void)
{
    struct WarioData *wario;
    const struct FatWarioGraphicsFrame *frames;

    if (gButtonsPressed & A_BUTTON) {
        return 4;
    }

    {
        struct WarioData *address;
        register u16 timer PIN("r2");
        u32 limit;
        register u16 nextTimer PIN("r0");

        address = &gWarioData;
        timer = address->unk_0C;
        limit = 0x167;
        wario = address;
        if (timer > limit) {
            return 8;
        }
        nextTimer = timer + 1;
        wario->unk_0C = nextTimer;
    }

    {
        const u16 *buttons;
        unsigned int held;
        unsigned int direction;
        unsigned int active;

        buttons = &gButtonsHeld;
        held = *buttons;
        direction = wario->horizontalDirection;
        active = held;
        active &= direction;
        if (active != 0) {
            unsigned int right;
            right = DPAD_RIGHT;
            right &= direction;
            if (right != 0) {
                wario->xVelocity = 0x20;
            } else {
                wario->xVelocity = -0x20;
            }
        } else {
            u32 test;
            test = DPAD_LEFT | DPAD_RIGHT;
            test ^= direction;
            test &= held;
            test <<= 16;
            ASM_BARRIER(: "+r"(test));
            if (test != 0) {
                return 2;
            }
            return 9;
        }
    }

    {
        register struct WarioData *animWario PIN("r2");
        unsigned int frame;

        animWario = wario;
        frames = sFatWarioWalkingFrames;
        if (animWario->unk_1E >= frames[animWario->unk_1F].time) {
            animWario->unk_1E = 0;
            frame = animWario->unk_1F;
            frame++;
            animWario->unk_1F = frame;
            frame = (u8)frame;
            if (frame == 1) {
                m4aSongNumStart(SE_WARIO_STEP);
            } else if (frames[wario->unk_1F].time == 0) {
                wario->unk_1F = 0;
            }
        }
    }
    return 0xFF;
}

u8 FatWarioJumping(void)
{
    if (gWarioData.unk_1E >= sFatWarioJumpingFrames[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFatWarioJumpingFrames[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_08 = 1;
            gWarioData.unk_0C += 20;
            return 0xFE;
        }
    }
    return 0xFF;
}

u8 FatWarioFalling(void)
{
    const u16 *buttons;
    struct WarioData *wario;
    register u16 active PIN("r3");
    u16 held;
    u16 direction;

    buttons = &gButtonsHeld;
    wario = &gWarioData;
    held = *buttons;
    direction = wario->horizontalDirection;
    ASM_BARRIER(: "=r"(active) : "0"(held));
    active &= direction;
    if (active != 0) {
        s32 right;
        right = DPAD_RIGHT;
        right &= direction;
        if (right != 0) {
            wario->xVelocity = 0x20;
        } else {
            wario->xVelocity = -0x20;
        }
    } else {
        u32 test;
        direction ^= DPAD_LEFT | DPAD_RIGHT;
        test = direction;
        test &= held;
        test <<= 16;
        ASM_BARRIER(: "+r"(test));
        if (test != 0) {
            wario->horizontalDirection = direction;
        }
        wario->xVelocity = active;
    }
    return 0xFF;
}

u8 FatWarioLanding(void)
{
    struct WarioAfterimage *effect;
    const struct WarioEffectFrame *frames;

    if (gWarioData.unk_1E > 29) {
        return 9;
    }

    effect = &gWarioMotionAfterimage;
    frames = sWarioGroundPoundLandingAfterimageFrames;
    if (effect->unk1 >= frames[effect->unk2].time) {
        effect->unk1 = 0;
        effect->unk2++;
        if (frames[effect->unk2].time == 0) {
            effect->unk2 = 0;
        }
    }
    return 0xFF;
}

u8 FatWarioKillingEnemy(void)
{
    if (gWarioData.unk_1E >= sFatWarioKillingEnemyFrames[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFatWarioKillingEnemyFrames[gWarioData.unk_1F].time == 0) {
            return 9;
        }
    }
    return 0xFF;
}

u8 FatWarioDetransforming(void)
{
    if (gWarioData.unk_1E >= sFatWarioDetransformingFrames[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFatWarioDetransformingFrames[gWarioData.unk_1F].time == 0) {
            gWarioData.reaction = 0;
            gWarioData.damageTimer = 0x60;
            return 2;
        }
    }
    return 0xFF;
}

u8 FatWarioStartingWalkAfterLanding(void)
{
    struct WarioData *wario;
    unsigned int held;
    unsigned int direction;

    wario = &gWarioData;
    if (wario->unk_0A > 7) {
        return 8;
    }
    if (gButtonsPressed & A_BUTTON) {
        return 4;
    }
    held = gButtonsHeld;
    direction = wario->horizontalDirection;
    {
        u16 test;
        test = held;
        test &= direction;
        if (test != 0) {
            return 3;
        }
    }
    {
        u32 test;
        test = DPAD_LEFT | DPAD_RIGHT;
        test ^= direction;
        test &= held;
        if (test != 0) {
            return 2;
        }
    }
    {
        u32 test;
        test = DPAD_UP;
        test &= held;
        if (test != 0) {
            return 10;
        }
    }
    return 0xFF;
}

u8 FatWarioIdleInput(void)
{
    const u16 *buttons;
    struct WarioData *wario;
    unsigned int held;
    unsigned int direction;

    if (gButtonsPressed & A_BUTTON) {
        return 4;
    }
    buttons = &gButtonsHeld;
    wario = &gWarioData;
    held = *buttons;
    direction = wario->horizontalDirection;
    {
        u16 test;
        test = held;
        test &= direction;
        if (test != 0) {
            return 3;
        }
    }
    {
        u32 test;
        test = DPAD_LEFT | DPAD_RIGHT;
        test ^= direction;
        test &= held;
        if (test != 0) {
            return 2;
        }
    }
    {
        u32 test;
        test = DPAD_UP;
        test &= held;
        if (test == 0) {
            return 9;
        }
    }
    return 0xFF;
}

void SetFatWarioPose(u8 pose)
{
    int newPose;
    struct WarioData *poseTarget;

    newPose = pose;
    ResetWarioState();
    if (newPose == 0) {
        if (gWarioDataCopy.unk_1A == 2) {
            newPose = 1;
        }
        m4aSongNumStart(SE_FAT_WARIO_TRANSFORM_START);
        m4aSongNumStart(SE_WARIO_TRANSFORM_LOOP);
        VoiceSetPlay(4);
    } else {
        gWarioData.unk_0A = gWarioDataCopy.unk_0A;
        gWarioData.unk_0C = gWarioDataCopy.unk_0C;
    }

    if (newPose == 0xFD) {
        goto pose_fd;
    }
    if (newPose > 0xFD) {
        goto pose_above_fd;
    }
    if (newPose == 2) {
        goto pose_two;
    }
    poseTarget = &gWarioData;
    goto set_pose;

pose_above_fd:
    poseTarget = &gWarioData;
    if (newPose != 0xFE) {
        goto set_pose;
    }
    {
        struct WarioData *copyAddress;
        struct WarioData *copy;
        u8 copyPose;

        copyAddress = &gWarioDataCopy;
        copyPose = copyAddress->pose;
        copy = copyAddress;
        if (copyPose == 0) {
            poseTarget->pose = 1;
            poseTarget->unk_1E = copy->unk_1E;
            poseTarget->unk_1F = copy->unk_1F;
        } else {
            poseTarget->pose = 5;
        }
        if (copy->unk_08 != 0) {
            poseTarget->yVelocity = 0x6C;
        }
        poseTarget->xVelocity = copy->xVelocity;
    }
    goto end;

pose_fd:
    {
        struct WarioData *copy;
        copy = &gWarioDataCopy;
        if (copy->pose == 1) {
            struct WarioData *wario;
            wario = &gWarioData;
            wario->pose = 0;
            wario->unk_1E = copy->unk_1E;
            wario->unk_1F = copy->unk_1F;
            goto end;
        }
        {
            struct WarioData *wario;
            wario = &gWarioData;
            wario->pose = 6;
            wario->unk_0A++;
        }
        gWarioMotionAfterimage.unk0 = 4;
        ScreenShakeRequestY(0x40, 0);
        m4aSongNumStart(SE_FAT_WARIO_LAND);
    }
    goto end;

pose_two:
    {
        struct WarioData *wario;
        wario = &gWarioData;
        wario->unk_03 = 1;
        poseTarget = wario;
    }
set_pose:
    poseTarget->pose = newPose;
end:
    return;
}

void UpdateFatWarioMotion(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u16 yOffset;

    collision = &gWarioCollisionData;
    config = &sFatWarioPoseProperties[0][0];
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

    UpdateWarioHorizontalCollisionOffset();
    yOffset = 0;
    if (wario->unk_1A == 2) {
        u16 velocity;
        u32 shifted;

        velocity = wario->yVelocity;
        shifted = velocity;
        shifted <<= 16;
        shifted = (s32)shifted >> 19;
        yOffset = (u16)shifted;
        velocity -= 8;
        wario->yVelocity = velocity;
    }

    {
        struct WarioData *clampWario;
        clampWario = wario;
        if (clampWario->yVelocity < -128) {
            clampWario->yVelocity = -128;
        }
    }

    wario->yPosition -= yOffset;
    {
        u32 xVelocity;
        if (wario->unk_1A == 0) {
            xVelocity = GetAdjustedWarioXVelocity();
        } else {
            xVelocity = *(u16 *)&wario->xVelocity;
        }
        xVelocity <<= 16;
        xVelocity = (s32)xVelocity >> 19;
        xVelocity <<= 16;
        xVelocity >>= 16;
        {
            register u32 delta PIN("r2");
            struct WarioData *positionWario;
            u16 xPosition;

            delta = xVelocity;
            ASM_BARRIER(: "+r"(delta));
            positionWario = &gWarioData;
            xPosition = positionWario->xPosition;
            xPosition += delta;
            positionWario->xPosition = xPosition;
        }
    }
}

void ProcessFatWarioCollision(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u8 movementResult;
    u16 flags;
    int collisionResult;
    u32 local;

    collision = &gWarioCollisionData;
    config = &sFatWarioPoseProperties[0][0];
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

    if (wario->yVelocity <= 0x2F) {
        collision->unk_0C = 0;
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
        } else if (collision->unk_00 != 0) {
            if (collision->unk_11 == 2) {
                movementResult = ResolveWarioCeilingCollision();
            } else {
                movementResult = ResolveWarioStandardCollision();
            }
        } else if (collision->unk_11 == 0) {
            movementResult = ResolveWarioLandingCollision();
        }
    }

    {
        struct WarioData *currentWario;
        currentWario = &gWarioData;
        collisionResult = GetBackgroundCollisionAtPosition(currentWario->yPosition - 0x30, currentWario->xPosition);
        collisionResult &= 0xFF;
        if (collisionResult == 1) {
            currentWario->reaction = collisionResult;
            SetWarioWaterPose(12);
            gWarioDustEffect1.unk0 = collisionResult;
        } else {
            struct WarioCollisionData *collision2;
            collision2 = &gWarioCollisionData;
            if (collision2->unk_11 != 0xFF) {
                if (currentWario->pose == 5 && movementResult == 0xFD) {
                    int result;
                    result = CheckWarioPointCollision(collision2->unk_08, (u16)(currentWario->yPosition + 1), &local);
                    if (result != 0 && result <= collision2->unk_10) {
                        movementResult = 0xFF;
                    }
                }
                if (movementResult != 0xFF) {
                    SetFatWarioPose(movementResult);
                }
            }
        }
    }
}

void LoadFatWarioGraphics(int variant)
{
    u32 graphicsVariant;
    const struct FatWarioGraphicsFrame *frame;
    const u8 *gfx;

    graphicsVariant = (u8)variant;
    UpdateWarioPositionHistory();

    frame = GP(sFatWarioGraphicsTable[gWarioData.pose][graphicsVariant]);
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
    CopyWarioPalette(sWarioDefaultObjPalette, 0, 0x10);
}

void UpdateFatWarioMusic(void)
{
    struct MusicPlayerInfo *player0;
    struct MusicPlayerInfo *player1;
    const struct MusicPlayer *table;
    int tracks;
    int pitch;

    if (gWarioMusicState != 7) {
        table = gMPlayTable;
        player0 = GP(table[0].info);
        m4aMPlayTempoControl(player0, 150);
        player1 = GP(table[1].info);
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=150; m4aMPlayTempoControl(a0,a1); }
        tracks = 0xFFFF;
        pitch = -500;
        { struct MusicPlayerInfo *arg0; arg0=player0; m4aMPlayPitchControl(arg0, tracks, pitch); }
        { struct MusicPlayerInfo *arg0; arg0=player1; m4aMPlayPitchControl(arg0, tracks, pitch); }
        { struct MusicPlayerInfo *a0; int a1; a0=player0; a1=tracks; m4aMPlayModDepthSet(a0,a1,10); }
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=tracks; m4aMPlayModDepthSet(a0,a1,10); }
        { struct MusicPlayerInfo *a0; int a1; a0=player0; a1=tracks; m4aMPlayLFOSpeedSet(a0,a1,50); }
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=tracks; m4aMPlayLFOSpeedSet(a0,a1,50); }
        gWarioMusicState = 7;
    }
}

void UpdateFatWarioHitbox(void)
{
    const u8 *config;
    struct WarioData *wario;
    const u8 *hitboxes;
    u32 hitboxOffset;
    u8 direction;

    config = &sFatWarioPoseProperties[0][0];
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

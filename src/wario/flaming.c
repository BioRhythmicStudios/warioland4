#include "bg_clip.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "input.h"
#include "main.h"
#include "sound.h"
#include "voice_set.h"
#include "wario.h"
#include "wario/normal.h"
#include "wario/swimming.h"
#include "wario/flaming.h"
#include "wario_palette.h"

u8 UpdateFlamingWario(void)
{
    return GF(sFlamingWarioPoseTable[gWarioData.pose])();
}

u8 FlamingWarioBlue(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u16 pressed;
    u32 active;
    u8 zero;

    {
        const u16 *buttons;
        buttons = &gButtonsPressed;
        pressed = *buttons;
    }
    active = A_BUTTON;
    active &= pressed;
    wario = &gWarioData;
    if ((active) || wario->unk_07 != 0) {
        wario->unk_08 = 1;
        return 0xFE;
    }

    if (wario->horizontalDirection & DPAD_RIGHT) {
        wario->xVelocity = 0x60;
    } else {
        wario->xVelocity = -0x60;
    }

    frames = sFlamingWarioBlueFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioBlueMidair(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u16 pressed;
    u32 active;
    u8 zero;

    {
        const u16 *buttons;
        buttons = &gButtonsPressed;
        pressed = *buttons;
    }
    active = A_BUTTON;
    active &= pressed;
    wario = &gWarioData;
    if (active) {
        wario->unk_07 = 1;
    }
    if (wario->horizontalDirection & DPAD_RIGHT) {
        wario->xVelocity = 0x60;
    } else {
        wario->xVelocity = -0x60;
    }
    frames = sFlamingWarioBlueFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioGreen(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u16 pressed;
    u32 active;
    u8 zero;

    {
        const u16 *buttons;
        buttons = &gButtonsPressed;
        pressed = *buttons;
    }
    active = A_BUTTON;
    active &= pressed;
    wario = &gWarioData;
    if ((active) || wario->unk_07 != 0) {
        wario->unk_08 = 1;
        return 0xFE;
    }
    if (wario->horizontalDirection & DPAD_RIGHT) {
        wario->xVelocity = 0x60;
    } else {
        wario->xVelocity = -0x60;
    }
    frames = sFlamingWarioGreenFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioGreenMidair(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u16 pressed;
    u32 active;
    u8 zero;

    {
        const u16 *buttons;
        buttons = &gButtonsPressed;
        pressed = *buttons;
    }
    active = A_BUTTON;
    active &= pressed;
    wario = &gWarioData;
    if (active) {
        wario->unk_07 = 1;
    }
    if (wario->horizontalDirection & DPAD_RIGHT) {
        wario->xVelocity = 0x60;
    } else {
        wario->xVelocity = -0x60;
    }
    frames = sFlamingWarioGreenFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioRed(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u16 pressed;
    u32 active;
    u8 zero;

    {
        const u16 *buttons;
        buttons = &gButtonsPressed;
        pressed = *buttons;
    }
    active = A_BUTTON;
    active &= pressed;
    wario = &gWarioData;
    if ((active) || wario->unk_07 != 0) {
        wario->unk_08 = 1;
        return 0xFE;
    }
    if (wario->horizontalDirection & DPAD_RIGHT) {
        wario->xVelocity = 0x60;
    } else {
        wario->xVelocity = -0x60;
    }
    frames = sFlamingWarioRedFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioRedMidair(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u16 pressed;
    u32 active;
    u8 zero;

    {
        const u16 *buttons;
        buttons = &gButtonsPressed;
        pressed = *buttons;
    }
    active = A_BUTTON;
    active &= pressed;
    wario = &gWarioData;
    if (active) {
        wario->unk_07 = 1;
    }
    if (wario->horizontalDirection & DPAD_RIGHT) {
        wario->xVelocity = 0x60;
    } else {
        wario->xVelocity = -0x60;
    }
    frames = sFlamingWarioRedFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioEngulfed(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    register u16 held PIN("r3");
    u32 active;
    register u16 direction PIN("r1");
    u16 timer;
    u8 zero;

    wario = &gWarioData;
    if (wario->horizontalDirection & DPAD_RIGHT) {
        wario->xVelocity = 0x10;
    } else {
        wario->xVelocity = -0x10;
    }

    held = gButtonsHeld;
    direction = wario->horizontalDirection;
    active = held;
    active &= direction;
    if (active != 0) {
        if (direction & DPAD_RIGHT) {
            wario->xVelocity = 0x20;
        } else {
            wario->xVelocity = -0x20;
        }
    } else {
        u32 test;

        direction ^= DPAD_LEFT | DPAD_RIGHT;
        test = direction & held;
        test <<= 16;
        if (test != 0) {
            wario->horizontalDirection = direction;
            wario->xVelocity = active;
        }
    }

    timer = wario->unk_0C;
    if (timer > 0xB3) {
        return 9;
    }
    timer++;
    wario->unk_0C = timer;
    zero = 0;
    frames = sFlamingWarioEngulfedFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioEngulfedMidair(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u16 timer;
    u8 zero;

    wario = &gWarioData;
    timer = wario->unk_0C;
    if (timer <= 0xB3) {
        timer++;
        wario->unk_0C = timer;
    }
    frames = sFlamingWarioEngulfedFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = zero;
        }
    }
    return 0xFF;
}

u8 FlamingWarioRecovering(void)
{
    struct WarioData *wario;
    const struct FlamingWarioGraphicsFrame *frames;
    u8 zero;

    wario = &gWarioData;
    frames = sFlamingWarioRecoveringFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        zero = 0;
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->reaction = zero;
            wario->damageTimer = 0x60;
            return 2;
        }
    }
    return 0xFF;
}

void SetFlamingWarioPose(u8 pose)
{
    int newPose;
    int originalPose;
    struct WarioData *wario;

    newPose = pose;
    originalPose = newPose;
    wario = &gWarioData;
    if (wario->pose == 9) {
        u32 test;

        test = newPose + 3;
        test <<= 24;
        test >>= 24;
        if (test <= 1) {
            goto end;
        }
    }

    ResetWarioState();
    if (newPose == 0) {
        struct WarioData *copy;
        u8 copyDirection;

        copy = &gWarioDataCopy;
        copyDirection = copy->unk_1A;
        newPose = 1;
        if (copyDirection == 2) {
            newPose = 2;
        }
        copy->unk_07 = originalPose;
        copy->xVelocity = originalPose;
        m4aSongNumStart(SE_WARIO_TRANSFORM_LOOP);
        VoiceSetPlay(4);
    } else {
        struct WarioData *copy;

        copy = &gWarioDataCopy;
        if (copy->pose > 6) {
            wario->unk_0C = copy->unk_0C;
        }
    }

    switch (newPose) {
    case 0xFE:
        {
            const u8 *config;
            struct WarioData *target;
            u8 currentPose;
            u32 tableOffset;
            struct WarioData *copy;

            config = &sFlamingWarioPoseProperties[0][0];
            target = (struct WarioData *)0x03001898;
            currentPose = target->pose;
            tableOffset = currentPose;
            tableOffset <<= 3;
            config += 7;
            tableOffset += (u32)config;
            if (*(const u8 *)tableOffset != 2) {
                u32 nextPose;

                nextPose = currentPose + 1;
                target->pose = nextPose;
            }
            copy = &gWarioDataCopy;
            target->unk_1F = copy->unk_1F;
            target->xVelocity = copy->xVelocity;
            if (copy->unk_08 == 1) {
                target->yVelocity = 0xA8;
            }
        }
        break;

    case 0xFD:
        {
            const u8 *config;
            struct WarioData *target;
            u8 currentPose;
            u32 tableOffset;
            struct WarioData *copy;

            config = &sFlamingWarioPoseProperties[0][0];
            target = (struct WarioData *)0x03001898;
            currentPose = target->pose;
            tableOffset = currentPose;
            tableOffset <<= 3;
            config += 7;
            tableOffset += (u32)config;
            if (*(const u8 *)tableOffset == 2) {
                u32 previousPose;

                previousPose = currentPose - 1;
                target->pose = previousPose;
            }
            copy = &gWarioDataCopy;
            target->unk_1F = copy->unk_1F;
            target->xVelocity = copy->xVelocity;
            target->unk_07 = copy->unk_07;
        }
        break;

    case 1:
    case 3:
    case 5:
        {
            struct WarioData *target;
            struct WarioData *copy;

            target = &gWarioData;
            target->pose = newPose;
            copy = &gWarioDataCopy;
            target->xVelocity = copy->xVelocity;
        }
        break;

    case 2:
    case 4:
    case 6:
        {
            struct WarioData *target;
            struct WarioData *copy;

            target = &gWarioData;
            target->pose = newPose;
            copy = &gWarioDataCopy;
            target->xVelocity = copy->xVelocity;
            target->yVelocity = copy->yVelocity;
        }
        break;

    case 7:
    case 8:
        {
            struct WarioData *target;
            struct WarioData *copy;

            target = &gWarioData;
            target->pose = newPose;
            copy = &gWarioDataCopy;
            target->xVelocity = copy->xVelocity;
            m4aSongNumStart(SE_FLAMING_WARIO_ENGULFED);
        }
        break;

    case 9:
        m4aSongNumStart(SE_FLAMING_WARIO_RECOVER);
        /* fall through */
    default:
        {
            struct WarioData *target;

            target = &gWarioData;
            target->pose = newPose;
        }
        break;
    }

end:
    return;
}

void UpdateFlamingWarioMotion(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u16 yOffset;

    collision = &gWarioCollisionData;
    config = &sFlamingWarioPoseProperties[0][0];
    wario = (struct WarioData *)0x03001898;

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

void ProcessFlamingWarioCollision(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u8 movementResult;
    u16 flags;
    int collisionResult;

    collision = &gWarioCollisionData;
    config = &sFlamingWarioPoseProperties[0][0];
    wario = (struct WarioData *)0x03001898;

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
        u32 mask;
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
        u8 pose;

        currentWario = &gWarioData;
        collisionResult = GetBackgroundCollisionAtPosition(currentWario->yPosition - 0x30, currentWario->xPosition);
        collisionResult &= 0xFF;
        if (collisionResult == 1) {
            currentWario->reaction = collisionResult;
            currentWario->damageTimer = 0x60;
            SetWarioWaterPose(0);
        } else {
            switch (currentWario->pose) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    if (gWarioCollisionData.unk_12 != 0) {
                        int nextPose;

                        currentWario->horizontalDirection ^= DPAD_LEFT | DPAD_RIGHT;
                        nextPose = *(volatile u8 *)&currentWario->pose;
                        nextPose += 2;
                        movementResult = nextPose;
                    }
                    break;
            }
            if (movementResult != 0xFF) {
                SetFlamingWarioPose(movementResult);
            }
        }
    }
}

void LoadFlamingWarioGraphics(int variant)
{
    u32 graphicsVariant;
    register const u8 *palette PIN("r5");
    const struct FlamingWarioGraphicsFrame *frame;
    const u8 *gfx;

    graphicsVariant = (u8)variant;
    UpdateWarioPositionHistory();
    frame = GP(sFlamingWarioGraphicsTable[gWarioData.pose][graphicsVariant]);
    frame += gWarioData.unk_1F;
    gfx = GP(frame->gfx);
    gWarioData.objData1Size = gfx[0] << 5;
    gfx++;
    gWarioData.objData2Size = gfx[0] << 5;
    gfx++;
    GP_SET(gWarioData.pObjData1, (u8 *)gfx);
    GP_SET(gWarioData.pObjData2, (u8 *)gfx + gWarioData.objData1Size);
    GP_SET(gWarioData.pOamData, GP(frame->oam));
    gWarioPaletteSize = 0x40;

    switch (gWarioData.pose) {
    case 0:
    case 1:
    case 2:
        palette = sFlamingWarioBluePalette;
        break;
    case 3:
    case 4:
        palette = sFlamingWarioGreenPalette;
        break;
    case 5:
    case 6:
        palette = sFlamingWarioRedPalette;
        break;
    case 7:
    case 8:
        if (((struct WarioData *)0x03001898)->unk_0C <= 99) {
            palette = sFlamingWarioRedPalette;
        } else if (((struct WarioData *)0x03001898)->unk_0C <= 119) {
            palette = sFlamingWarioEngulfedPalette1;
        } else if (((struct WarioData *)0x03001898)->unk_0C <= 139) {
            palette = sFlamingWarioEngulfedPalette2;
        } else {
            palette = sFlamingWarioEngulfedPalette3;
            if (((struct WarioData *)0x03001898)->unk_0C <= 159) {
                palette -= 0x40;
            }
        }
        break;
    case 9:
        {
            u8 frameIndex;

            frameIndex = gWarioData.unk_1F;
            palette = sFlamingWarioRecoveringPalette;
            if (frameIndex <= 1) {
                palette -= 0x40;
            }
        }
        break;
    default:
        break;
    }
    CopyWarioPalette(palette, 0, 0x20);
}

void UpdateFlamingWarioMusic(void)
{
    register struct MusicPlayerInfo *player0 PIN("r8");
    register struct MusicPlayerInfo *player1 PIN("r6");
    const struct MusicPlayer *table;
    register int tracks PIN("r4");
    int value;

    if (gWarioMusicState != 6) {
        table = gMPlayTable;
        player0 = GP(table[0].info);
        value = 350;
        { struct MusicPlayerInfo *a0; int a1; a0=player0; a1=value; m4aMPlayTempoControl(a0,a1); }
        player1 = GP(table[1].info);
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=value; m4aMPlayTempoControl(a0,a1); }
        tracks = 0xFFFF;
        value -= 94;
        { struct MusicPlayerInfo *a0; a0=player0; m4aMPlayPitchControl(a0, tracks, value); }
        { struct MusicPlayerInfo *a0; a0=player1; m4aMPlayPitchControl(a0, tracks, value); }
        { struct MusicPlayerInfo *a0; int a1; a0=player0; a1=tracks; m4aMPlayModDepthSet(a0,a1,20); }
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=tracks; m4aMPlayModDepthSet(a0,a1,20); }
        { struct MusicPlayerInfo *a0; int a1; a0=player0; a1=tracks; m4aMPlayLFOSpeedSet(a0,a1,20); }
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=tracks; m4aMPlayLFOSpeedSet(a0,a1,20); }
        gWarioMusicState = 6;
    }
}

void UpdateFlamingWarioHitbox(void)
{
    const u8 *config;
    struct WarioData *wario;
    const u8 *hitboxes;
    u32 hitboxOffset;
    u8 direction;

    config = &sFlamingWarioPoseProperties[0][0];
    wario = (struct WarioData *)0x03001898;

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

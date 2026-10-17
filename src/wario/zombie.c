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
#include "wario/zombie.h"
#include "wario_palette.h"

u8 UpdateZombieWario(void)
{
    return GF(sZombieWarioPoseTable[gWarioData.pose])();
}

u8 ZombieWarioTransforming(void)
{
    if (gWarioData.unk_1E > 3) {
        return 2;
    }
    return 0xFF;
}

u8 ZombieWarioWalking(void)
{
    struct WarioData *wario;
    u32 held;
    u32 direction;
    const struct ZombieWarioGraphicsFrame *frames;

    if (gButtonsPressed & A_BUTTON) {
        return 3;
    }
    {
        const u16 *buttons;
        struct WarioData *address;
        u32 active;

        buttons = &gButtonsHeld;
        address = &gWarioData;
        held = *buttons;
        direction = address->horizontalDirection;
        active = held;
        active &= direction;
        wario = address;
        if (active != 0) {
            active = DPAD_RIGHT;
            active &= direction;
            if (active != 0) {
                wario->xVelocity = 0x18;
            } else {
                wario->xVelocity = -0x18;
            }
        } else {
            active = DPAD_LEFT | DPAD_RIGHT;
            active ^= direction;
            active &= held;
            active <<= 16;
            ASM_BARRIER(: "+r"(active));
            if (active != 0) {
                return 11;
            }
            return 2;
        }
    }
    frames = sZombieWarioWalkingFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 ZombieWarioEmergingFromHat(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;
    s32 held;
    u16 direction;

    held = gButtonsHeld;
    if (held & A_BUTTON) {
        return 3;
    }
    wario = &gWarioData;
    direction = wario->horizontalDirection;
    if (held & direction) {
        return 1;
    }
    if (held & (direction ^ (DPAD_LEFT | DPAD_RIGHT))) {
        return 11;
    }
    frames = sZombieWarioEmergingFromHatFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 ZombieWarioJumping(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;
    wario = &gWarioData;
    frames = sZombieWarioJumpingFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->unk_08 = 1;
            return 0xFE;
        }
    }
    return 0xFF;
}

u8 ZombieWarioFalling(void)
{
    struct WarioData *wario;
    u32 held;
    u32 direction;
    const struct ZombieWarioGraphicsFrame *frames;

    {
        const u16 *buttons;
        register struct WarioData *address PIN("r1");
        u32 velocity;

        buttons = &gButtonsHeld;
        address = &gWarioData;
        held = *buttons;
        direction = address->horizontalDirection;
        velocity = held;
        velocity &= direction;
        wario = address;
        if (velocity != 0) {
            velocity = DPAD_RIGHT;
            velocity &= direction;
            if (velocity != 0) {
                velocity = 0x18;
            } else {
                velocity = (u16)-0x18;
            }
        } else {
            velocity = DPAD_LEFT | DPAD_RIGHT;
            velocity ^= direction;
            velocity &= held;
            velocity <<= 16;
            ASM_BARRIER(: "+r"(velocity));
            velocity >>= 16;
            if (velocity != 0) {
                return 12;
            }
        }
        wario->xVelocity = velocity;
    }
    frames = sZombieWarioFallingFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        {
            u8 zero;
            zero = 0;
            wario->unk_1E = zero;
        }
        wario->unk_1F++;
        {
            s16 frameTime;
            frameTime = frames[wario->unk_1F].time;
            if (frameTime == 0) {
                wario->unk_1F = frameTime;
            }
        }
    }
    return 0xFF;
}

u8 ZombieWarioUnknown5(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;

    wario = &gWarioData;
    frames = sZombieWarioFallingFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            return 2;
        }
    }
    return 0xFF;
}

u8 ZombieWarioFallingThroughPlatform(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;

    wario = &gWarioData;
    frames = sZombieWarioFallingThroughPlatformFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            return 9;
        }
    }
    return 0xFF;
}

u8 ZombieWarioLanding(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;

    wario = &gWarioData;
    frames = sZombieWarioLandingFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            return 2;
        }
    }
    return 0xFF;
}

u8 ZombieWarioHatLanding(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;

    wario = &gWarioData;
    frames = sZombieWarioHatLandingFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            return 2;
        }
    }
    return 0xFF;
}

u8 ZombieWarioHatFalling(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;
    wario = &gWarioData;
    frames = sZombieWarioHatFallingFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) wario->unk_1F = 0;
    }
    return 0xFF;
}

u8 ZombieWarioTouchingLightSource(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;
    wario = &gWarioData;
    frames = sZombieWarioTouchingLightFrames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            wario->reaction = 0;
            wario->damageTimer = 0x60;
            return 2;
        }
    }
    return 0xFF;
}
u8 ZombieWarioUnknown11(void)
{
    struct WarioData *wario;
    const struct ZombieWarioGraphicsFrame *frames;

    wario = &gWarioData;
    frames = sZombieWarioUnknown11Frames;
    if (wario->unk_1E >= frames[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (frames[wario->unk_1F].time == 0) {
            return 2;
        }
    }
    return 0xFF;
}

u8 ZombieWarioFallingAfterJump(void)
{
    if (gWarioData.unk_1E > 4) {
        return 4;
    }
    return 0xFF;
}

void SetZombieWarioPose(u8 pose)
{
    int newPose;
    struct WarioData *wario;

    newPose = pose;
    ResetWarioState();
    switch (newPose) {
    case 0xFE:
        wario = &gWarioData;
        wario->pose = 4;
        wario->xVelocity = gWarioDataCopy.xVelocity;
        if (gWarioDataCopy.unk_08 == 1) {
            wario->yVelocity = 0x6C;
        } else if (gWarioDataCopy.unk_08 == 2) {
            wario->yVelocity = gWarioDataCopy.yVelocity;
        }
        break;
    case 0xFD:
        if ((gWarioDataCopy.pose == 4) || (gWarioDataCopy.pose == 12)) {
            gWarioData.pose = 7;
            m4aSongNumStart(SE_ZOMBIE_WARIO_LAND);
        } else if (gWarioData.pose == 9) {
            gWarioData.pose = 8;
            m4aSongNumStart(SE_ZOMBIE_WARIO_HAT_LAND);
        }
        break;
    case 0:
        gWarioData.pose = newPose;
        m4aSongNumStart(SE_WARIO_TRANSFORM_LOOP);
        VoiceSetPlay(4);
        break;
    case 11:
        gWarioData.pose = newPose;
        gWarioData.unk_03 = 1;
        break;
    case 12:
        gWarioData.pose = newPose;
        gWarioData.unk_03 = 1;
        gWarioData.yVelocity = gWarioDataCopy.yVelocity;
        break;
    case 10:
        m4aSongNumStart(SE_WARIO_BONK);
        VoiceSetPlay(0);
        /* fallthrough */
    default:
        gWarioData.pose = newPose;
        break;
    }
    gWarioData.unk_05 = sZombieWarioPoseUnk05Values[gWarioData.pose];
}

void UpdateZombieWarioMotion(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u16 yOffset;
    collision = &gWarioCollisionData;
    config = &sZombieWarioPoseProperties[0][0];
    wario = &gWarioData;
    {
        u32 poseOffset;
        const u8 *field;
        poseOffset = wario->pose; poseOffset <<= 3; field = config + 1;
        poseOffset += (u32)field; collision->unk_08 = *(const u8 *)poseOffset;
    }
    {
        u32 poseOffset;
        const u8 *field;
        poseOffset = wario->pose; poseOffset <<= 3; field = config + 2;
        poseOffset += (u32)field; collision->unk_09 = *(const u8 *)poseOffset;
    }
    {
        u32 poseOffset;
        poseOffset = wario->pose; poseOffset <<= 3; config += 3;
        poseOffset += (u32)config; collision->unk_0A = *(const u8 *)poseOffset;
    }
    UpdateWarioHorizontalCollisionOffset();
    yOffset = 0;
    if (wario->unk_1A == 2) {
        u16 velocity;
        u32 shifted;
        velocity = wario->yVelocity; shifted = velocity; shifted <<= 16;
        shifted = (s32)shifted >> 19; shifted <<= 16; yOffset = shifted >> 16;
        velocity -= 8; wario->yVelocity = velocity;
    }
    {
        struct WarioData *clampWario;
        clampWario = wario;
        if (clampWario->yVelocity < -128) clampWario->yVelocity = -128;
    }
    wario->yPosition -= yOffset;
    {
        u32 xVelocity;
        if (wario->unk_1A == 0) xVelocity = GetAdjustedWarioXVelocity();
        else xVelocity = *(u16 *)&wario->xVelocity;
        xVelocity <<= 16; xVelocity = (s32)xVelocity >> 19; xVelocity <<= 16; xVelocity >>= 16;
        {
            register u32 delta PIN("r2");
            struct WarioData *positionWario;
            u16 xPosition;
            delta = xVelocity; ASM_BARRIER(: "+r"(delta)); positionWario = &gWarioData;
            xPosition = positionWario->xPosition; xPosition += delta; positionWario->xPosition = xPosition;
        }
    }
}

void ProcessZombieWarioCollision(void)
{
    struct WarioCollisionData *collision;
    const u8 *config;
    struct WarioData *wario;
    u8 result;
    u16 flags;
    collision = &gWarioCollisionData;
    config = &sZombieWarioPoseProperties[0][0];
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
    if (wario->yVelocity <= 0x2F) collision->unk_0C = 0;
    result = 0xFF;
    if (wario->pose == 9) {
        result = CheckZombieWarioFloor();
        if (result == 0xFE) result = 0xFF;
    } else {
        flags = collision->flags;
        if (flags & 0x40) result = ResolveWarioFloorCollision();
        else {
            s32 mask;
            mask = 0x80;
            mask &= flags;
            if (mask != 0) result = ResolveWarioCeilingCollision();
            else if (collision->unk_00 != 0) {
                if (collision->unk_11 == 2) result = ResolveWarioCeilingCollision();
                else result = ResolveWarioStandardCollision();
            } else if (collision->unk_11 == 0) result = ResolveWarioLandingCollision();
        }
    }
    {
        struct WarioData *currentWario;
        int hit;

        currentWario = &gWarioData;
        hit = GetBackgroundCollisionAtPosition(currentWario->yPosition - 0x30, currentWario->xPosition) & 0xFF;
        if (hit == 1) {
            currentWario->reaction = 1;
            currentWario->damageTimer = 0x60;
            SetWarioWaterPose(0);
            return;
        } else {
            struct WarioCollisionData *collision2;
            collision2 = &gWarioCollisionData;
            if (collision2->unk_11 != 0xFF) {
                if (result == 0xFD) {
                    {
                        u32 currentPose;
                        currentPose = currentWario->pose;
                        if ((currentPose == 4) || (currentPose == 12)) {
                        if (*(u16 *)&currentWario->unk_1C != 0) {
                            result = 7;
                        } else {
                            result = CheckZombieWarioFloor();
                            if (result == 0xFE) {
                                result = 6;
                                m4aSongNumStart(SE_ZOMBIE_WARIO_FALL_THROUGH_PLATFORM);
                            } else if (result == 0xFD) {
                                result = 7;
                                m4aSongNumStart(SE_ZOMBIE_WARIO_LAND);
                            }
                        }
                    } else if (currentPose == 9) {
                        result = 8;
                        m4aSongNumStart(SE_ZOMBIE_WARIO_HAT_LAND);
                    }
                    }
                }
                if (result != 0xFF) {
                    SetZombieWarioPose(result);
                }
            }
        }
    }
}

int CheckZombieWarioFloor(void)
{
    u8 collisionOffset;
    struct WarioData *wario;
    u16 yPosition;
    u32 yValue;
    int result;
    u16 local;

    collisionOffset = gWarioCollisionData.unk_08;
    wario = &gWarioData;
    yValue = wario->yPosition;
    yValue += 1;
    yPosition = (u16)yValue;
    result = CheckWarioPointCollision(collisionOffset, yPosition, &local);
    if (result != 0) {
        if (result > local) {
            wario->yPosition = (yPosition & 0xFFC0) - 1;
            return 0xFD;
        }
        return 0xFE;
    }
    return 0xFF;
}

void LoadZombieWarioGraphics(int variant)
{
    u32 graphicsVariant;
    const struct ZombieWarioGraphicsFrame *frame;
    const u8 *gfx;

    graphicsVariant = (u8)variant;
    UpdateWarioPositionHistory();
    frame = GP(sZombieWarioGraphicsTable[gWarioData.pose][graphicsVariant]);
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
    CopyWarioPalette(sZombieWarioPalette, 0, 0x20);
}

void UpdateZombieWarioMusic(void)
{
    struct MusicPlayerInfo *player0;
    struct MusicPlayerInfo *player1;
    const struct MusicPlayer *table;
    int tracks;
    int pitch;

    if (gWarioMusicState != 9) {
        table = gMPlayTable;
        player0 = GP(table[0].info);
        m4aMPlayTempoControl(player0, 200);
        player1 = GP(table[1].info);
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=200; m4aMPlayTempoControl(a0,a1); }
        tracks = 0xFFFF;
        pitch = -256;
        { struct MusicPlayerInfo *a0; a0=player0; m4aMPlayPitchControl(a0,tracks,pitch); }
        { struct MusicPlayerInfo *a0; a0=player1; m4aMPlayPitchControl(a0,tracks,pitch); }
        { struct MusicPlayerInfo *a0; int a1; a0=player0; a1=tracks; m4aMPlayModDepthSet(a0,a1,20); }
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=tracks; m4aMPlayModDepthSet(a0,a1,20); }
        { struct MusicPlayerInfo *a0; int a1; a0=player0; a1=tracks; m4aMPlayLFOSpeedSet(a0,a1,50); }
        { struct MusicPlayerInfo *a0; int a1; a0=player1; a1=tracks; m4aMPlayLFOSpeedSet(a0,a1,50); }
        gWarioMusicState = 9;
    }
}

void UpdateZombieWarioHitbox(void)
{
    const u8 *config;
    struct WarioData *wario;
    const u8 *hitboxes;
    u32 hitboxOffset;
    u8 direction;

    config = &sZombieWarioPoseProperties[0][0];
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

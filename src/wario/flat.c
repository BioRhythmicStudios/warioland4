#include "gba.h"
#include "input.h"
#include "bg_clip.h"
#include "sound.h"
#include "voice_set.h"
#include "gba/m4a.h"
#include "oam.h"
#include "wario.h"
#include "wario/swimming.h"
#include "wario_palette.h"

#include "wario/flat.h"

extern const u16 sUnk_82DE918[];

extern void ResetWarioState(void);
extern int GetAdjustedWarioXVelocity(void);
extern void UpdateWarioHorizontalCollisionOffset(void);
extern void UpdateWarioPositionHistory(void);
extern u8 ResolveWarioStandardCollision(void);
extern u8 ResolveWarioLandingCollision(void);
extern u8 ResolveWarioCeilingCollision(void);
extern u8 ResolveWarioFloorCollision(void);
extern void func_8016614(u8 value);

u8 UpdateFlatWario(void)
{
    return sFlatWarioPoseTable[gWarioData.pose]();
}

u8 FlatWarioTransforming(void)
{
    if (gWarioData.unk_1E >= sFlatWarioTransformingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFlatWarioTransformingAnimation[gWarioData.unk_1F].time == 0) {
            return WPOSE_FLAT_STANDING;
        }
    }

    return 0xFF;
}

u8 FlatWarioUnknown1(void)
{
    if (gWarioData.unk_1E >= sFlatWarioUnknown1Animation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFlatWarioUnknown1Animation[gWarioData.unk_1F].time == 0) {
            return 0xFE;
        }
    }

    return 0xFF;
}

u8 FlatWarioStanding(void)
{
    if (gButtonsPressed & A_BUTTON) {
        gWarioData.unk_08 = 1;
        return 0xFE;
    }

    if (gButtonsHeld & (DPAD_LEFT | DPAD_RIGHT)) {
        gWarioData.horizontalDirection = gButtonsHeld & (DPAD_LEFT | DPAD_RIGHT);
        return WPOSE_FLAT_WALKING;
    }

    if (gWarioData.unk_1E >= sFlatWarioStandingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFlatWarioStandingAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_1F = 0;
        }
    }

    return 0xFF;
}

u8 FlatWarioWalking(void)
{
    int direction;

    if (gButtonsPressed & A_BUTTON) {
        gWarioData.unk_08 = 1;
        return 0xFE;
    }

    if (gButtonsHeld & gWarioData.horizontalDirection) {
        if (gWarioData.horizontalDirection & DPAD_RIGHT) {
            gWarioData.xVelocity = 32;
        } else {
            gWarioData.xVelocity = -32;
        }
    } else {
        ;
        if ((u16)(gButtonsHeld & (gWarioData.horizontalDirection ^ (DPAD_LEFT | DPAD_RIGHT)))) {
            gWarioData.horizontalDirection ^= DPAD_LEFT | DPAD_RIGHT;
        }
        return WPOSE_FLAT_STANDING;
    }

    if (gWarioData.unk_1E >= sFlatWarioWalkingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFlatWarioWalkingAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.unk_1F = 0;
        }
    }
    return 0xFF;
}

u8 FlatWarioFloating(void)
{
    register struct WarioData* temp asm("r2");
    register struct WarioData* wario asm("r4");
    int counter;
    struct WarioData* current;
    u8 value;
    const u16* yTable;
    u16 direction;
    s32 directionTest;
    u16 heldButtons;

    temp = &gWarioData;
    direction = temp->horizontalDirection;
    directionTest = DPAD_RIGHT;
    directionTest &= direction;
    wario = temp;
    if (directionTest) {
        wario->xPosition += sFlatWarioHorizontalMovement[wario->unk_0A >> 2];
    } else {
        wario->xPosition -= sFlatWarioHorizontalMovement[wario->unk_0A >> 2];
    }

    current = wario;
    yTable = sFlatWarioVerticalMovement;
    counter = current->unk_0A;
    value = (u8)counter;
    current->yPosition += yTable[value >> 2];

    if (value <= 0x2E) {
        const struct WarioAnimationFrame* animation;
        int nextCounter;
        register int frame asm("r1");

        nextCounter = counter + 1;
        current->unk_0A = nextCounter;
        animation = sFlatWarioFloatingAnimation;
        frame = current->unk_1F;
        if (current->unk_1E < animation[frame].time) {
            goto returnFF;
        }
        {
            u8 frameTime;

            temp = current;
            temp->unk_1E = 0;
            temp->unk_1F++;
            frame = temp->unk_1F;
            frameTime = animation[frame].time;
            /* agbcc otherwise swaps the two equivalent branch targets around its literal pool. */
            asm volatile(
                "cmp %0, #0\n"
                "bne .LflatReturnFF"
                :
                : "r"(frameTime), "X"(&&returnFF)
                : "cc");
            goto decrementFrame;
        }
    }

    heldButtons = gButtonsHeld;
    direction = wario->horizontalDirection;
    if (!(heldButtons & direction)) {
        s32 newDirection;

        newDirection = DPAD_LEFT | DPAD_RIGHT;
        newDirection ^= direction;
        wario->horizontalDirection = newDirection;
    }
    return 0xFE;

decrementFrame:
    asm volatile(".LflatDecrementFrame:");
    {
        register int finalFrame asm("r1");
        register int decrementedFrame asm("r0");

        decrementedFrame = finalFrame - 1;
        temp->unk_1F = decrementedFrame;
    }
returnFF:
    asm volatile(".LflatReturnFF:");
    return 0xFF;
}

u8 FlatWarioJumping(void)
{
    register u16 heldButtons asm("r2");
    register u16 direction asm("r1");
    u16 heldDirection;

    if (gWarioData.yVelocity <= 0) {
        return 0xFE;
    }

    heldButtons = gButtonsHeld;
    direction = gWarioData.horizontalDirection;
    heldDirection = heldButtons & direction;
    if (heldDirection) {
        if (direction & DPAD_RIGHT) {
            gWarioData.xVelocity = 32;
        } else {
            gWarioData.xVelocity = -32;
        }
    } else {
        direction ^= DPAD_LEFT | DPAD_RIGHT;
        if ((u16)(direction & heldButtons)) {
            gWarioData.horizontalDirection = direction;
        }
        gWarioData.xVelocity = heldDirection;
    }

    return 0xFF;
}

u8 FlatWarioDetransforming(void)
{
    if (gWarioData.unk_1E >= sFlatWarioDetransformingAnimation[gWarioData.unk_1F].time) {
        gWarioData.unk_1E = 0;
        gWarioData.unk_1F++;
        if (sFlatWarioDetransformingAnimation[gWarioData.unk_1F].time == 0) {
            gWarioData.reaction = REACTION_NORMAL;
            gWarioData.damageTimer = 0x60;
            return WPOSE_FLAT_STANDING;
        }
    }

    return 0xFF;
}


void SetFlatWarioPose(u8 value)
{
    ResetWarioState();
    switch (value) {
    case 0xFE:
        if (gWarioDataCopy.unk_08 == 1) {
            gWarioData.pose = WPOSE_FLAT_JUMPING;
            gWarioData.yVelocity = 0x8A;
            gWarioDustEffect1.unk0 = 13;
        } else {
            gWarioData.pose = WPOSE_FLAT_FLOATING;
        }
        m4aSongNumStart(SE_FLAT_WARIO_JUMP);
        break;
    case 0xFD:
        gWarioData.pose = WPOSE_FLAT_STANDING;
        break;
    case 0:
        m4aSongNumStart(SE_WARIO_TRANSFORM_LOOP);
        VoiceSetPlay(VS_WARIO_HURT);
    default:
        gWarioData.pose = value;
        break;
    }
}

void UpdateFlatWarioMovement(void)
{
    register struct WarioCollisionData* collision asm("r3");
    register const u8* properties asm("r2");
    struct WarioData* wario;
    u16 movement;
    const u8* entry;
    register s16 yVelocity asm("r1");
    int velocity;
    int offset;

    collision = &gWarioCollisionData;
    properties = (const u8*)sFlatWarioPoseProperties;
    wario = &gWarioData;

    offset = wario->pose;
    offset *= 8;
    entry = properties + 1;
    offset += (int)entry;
    collision->unk_08 = *(const u8*)offset;

    offset = wario->pose;
    offset *= 8;
    entry = properties + 2;
    offset += (int)entry;
    collision->unk_09 = *(const u8*)offset;

    offset = wario->pose;
    offset *= 8;
    properties += 3;
    offset += (int)properties;
    collision->unk_0A = *(const u8*)offset;
    UpdateWarioHorizontalCollisionOffset();

    movement = 0;
    if (wario->pose == WPOSE_FLAT_JUMPING) {
        yVelocity = wario->yVelocity;
        movement = (u16)(yVelocity >> 3);
        yVelocity -= 8;
        wario->yVelocity = yVelocity;
    }
    wario->yPosition -= movement;

    if (wario->unk_1A == 0) {
        velocity = GetAdjustedWarioXVelocity();
    } else {
        velocity = (u16)wario->xVelocity;
    }
    movement = (u16)((s16)velocity >> 3);
    {
        s32 movementR2;

        /* This empty constraint emits no instructions; it prevents agbcc from reusing r0 before the final add. */
        asm("" : "=r"(movementR2) : "0"(movement));
        gWarioData.xPosition += movementR2;
    }
}

void ProcessFlatWarioCollision(void)
{
    struct WarioCollisionData* collision;
    register const u8* properties asm("r2");
    register struct WarioData* wario asm("r3");
    u8 result;
    const u8* entry;
    int collisionResult;
    int maskedResult;
    int offset;
    u8 pose;

    collision = &gWarioCollisionData;
    properties = (const u8*)sFlatWarioPoseProperties;
    wario = &gWarioData;

    offset = wario->pose;
    offset *= 8;
    entry = properties + 4;
    offset += (int)entry;
    collision->unk_0B = *(const u8*)offset;

    offset = wario->pose;
    offset *= 8;
    entry = properties + 5;
    offset += (int)entry;
    collision->unk_0C = *(const u8*)offset;

    offset = wario->pose;
    offset *= 8;
    entry = properties + 6;
    offset += (int)entry;
    collision->unk_0D = *(const u8*)offset;

    offset = wario->pose;
    offset *= 8;
    properties += 7;
    offset += (int)properties;
    collision->unk_11 = *(const u8*)offset;

    if (wario->yVelocity <= 0x2F) {
        collision->unk_0C = 0;
    }

    result = 0xFF;
    if (collision->flags & 0x40) {
        result = ResolveWarioFloorCollision();
    } else if (collision->flags & 0x80) {
        result = ResolveWarioCeilingCollision();
    } else if (collision->unk_00 != 0) {
        if (collision->unk_11 == 2) {
            result = ResolveWarioCeilingCollision();
        } else {
            result = ResolveWarioStandardCollision();
        }
    } else if (collision->unk_11 == 0) {
        result = ResolveWarioLandingCollision();
    }

    collisionResult = GetBackgroundCollisionAtPosition(gWarioData.yPosition - 0x30, gWarioData.xPosition);
    maskedResult = collisionResult;
    maskedResult &= 0xFF;
    if (maskedResult == 1) {
        gWarioData.reaction = maskedResult;
        gWarioData.damageTimer = 0x60;
        SetWarioWaterPose(0);
    } else if (gWarioCollisionData.unk_11 != 0xFF) {
        if (gWarioCollisionData.unk_12 != 0) {
            pose = gWarioData.pose;
            if (pose == WPOSE_FLAT_FLOATING) {
                goto flip_direction;
            }
            if (pose != WPOSE_FLAT_JUMPING) {
                goto keep_direction;
            }
flip_direction:
            gWarioData.horizontalDirection ^= DPAD_LEFT | DPAD_RIGHT;
            result = 0xFE;
        }
keep_direction:
        if (result != 0xFF) {
            SetFlatWarioPose(result);
        }
    }
}

void LoadFlatWarioGraphics(u8 value)
{
    const struct WarioAnimationFrame* frame;
    const u8* graphics;

    UpdateWarioPositionHistory();
    frame = sFlatWarioAnimationTable[gWarioData.pose][value];
    frame += gWarioData.unk_1F;
    graphics = (const u8*)frame->objData;
    gWarioData.objData1Size = *graphics++ << 5;
    gWarioData.objData2Size = *graphics++ << 5;
    gWarioData.pObjData1 = (u8*)graphics;
    gWarioData.pObjData2 = (u8*)(graphics + gWarioData.objData1Size);
    gWarioData.pOamData = frame->oamData;
    gWarioPaletteSize = 0x20;
    CopyWarioPalette(sUnk_82DE918, 0, 0x10);
}

void ApplyFlatWarioMusicEffects(void)
{
    struct MusicPlayerInfo* player0;
    struct MusicPlayerInfo* player1;
    u16 tracks;

    if (gWarioMusicState != 14) {
        player0 = gMPlayTable[0].info;
        m4aMPlayTempoControl(player0, 200);
        player1 = gMPlayTable[1].info;
        m4aMPlayTempoControl(player1, 200);
        tracks = 0xFFFF;
        m4aMPlayPitchControl(player0, tracks, 0x100);
        m4aMPlayPitchControl(player1, tracks, 0x100);
        m4aMPlayModDepthSet(player0, tracks, 20);
        m4aMPlayModDepthSet(player1, tracks, 20);
        m4aMPlayLFOSpeedSet(player0, tracks, 50);
        m4aMPlayLFOSpeedSet(player1, tracks, 50);
        gWarioMusicState = 14;
    }
}

void UpdateFlatWarioHitbox(void)
{
    const u8* properties;
    struct WarioData* wario;
    u8 hitboxIndex;
    u8 property;
    const s16* hitboxData;
    int propertyOffset;

    properties = (const u8*)sFlatWarioPoseProperties;
    wario = &gWarioData;
    hitboxIndex = properties[wario->pose * 8];
    hitboxData = (const s16*)sWarioHitboxes;
    wario->hitboxOffsetLeft = hitboxData[hitboxIndex * 4];
    wario->hitboxOffsetTop = ((const s16*)sWarioHitboxes)[hitboxIndex * 4 + 1];
    wario->hitboxOffsetRight = ((const s16*)sWarioHitboxes)[hitboxIndex * 4 + 2];
    wario->hitboxOffsetBottom = ((const s16*)sWarioHitboxes)[hitboxIndex * 4 + 3];

    propertyOffset = wario->pose;
    propertyOffset *= 8;
    properties += 7;
    propertyOffset += (int)properties;
    property = *(const u8*)propertyOffset;
    if (property == 2) {
        wario->unk_1A = property;
    } else if (wario->unk_1A != 1) {
        wario->unk_1A = property;
    }
}

/* Preserve the original zero-filled final text alignment. */
asm(".align 2, 0");

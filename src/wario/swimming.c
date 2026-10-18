#include "wario/swimming.h"

#include "bg_clip.h"
#include "door.h"
#include "main.h"
#include "score.h"
#include "screen_shake.h"
#include "sound.h"
#include "sprite.h"
#include "voice_set.h"
#include "wario/normal.h"
#include "wario_palette.h"


u8 UpdateWarioWaterPose(void)
{
    return GF(sWarioWaterPoseTable[gWarioData.pose])();
}

u8 UpdateWarioWaterLanding(void)
{
    if (gWarioData.yVelocity >= 0)
        return 5;
    return 0xFF;
}

u8 UpdateWarioSwimmingHorizontal(void)
{
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;
    register u8 frame PIN("r2");

    if (!(gButtonsHeld & 2))
        return 5;

    wario = &gWarioData;
    if (wario->horizontalDirection & 0x10) {
        wario->xVelocity -= 4;
        if (wario->xVelocity <= 0)
            return 5;
    } else {
        wario->xVelocity += 4;
        if (wario->xVelocity >= 0)
            return 5;
    }

    if (gWarioDustEffect2.unk0 == 0)
        gWarioDustEffect2.unk0 = 3;

    animation = sWarioSwimmingHorizontalAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F--;
    }

    frame = wario->unk_1F;
    if (frame > 5)
        gCurrentWarioEffect.type = 0;
    else
        gCurrentWarioEffect.unk_3 = frame;

    return 0xFF;
}

u8 UpdateWarioSwimmingVertical(void)
{
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;
    register u8 frame PIN("r2");

    if (!(gButtonsHeld & 3))
        return 5;

    wario = &gWarioData;
    if (wario->verticalDirection & 0x40) {
        wario->yVelocity -= 4;
        if (wario->yVelocity <= 0)
            return 5;
    } else {
        wario->yVelocity += 4;
        if (wario->yVelocity >= 0)
            return 5;
    }

    if (gWarioDustEffect2.unk0 == 0)
        gWarioDustEffect2.unk0 = 3;

    animation = sWarioSwimmingVerticalAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F--;
    }

    frame = wario->unk_1F;
    if (frame > 5)
        gCurrentWarioEffect.type = 0;
    else
        gCurrentWarioEffect.unk_3 = frame;

    return 0xFF;
}

u8 UpdateWarioSwimmingFreely(void)
{
    u32 temp;
    register u32 value PIN("r1");
    struct WarioData *wario;
    u16 *heldPointer;
    register u32 opposite PIN("r4");
    register u32 zero PIN("r5");
    const struct WarioAnimationFrame *animation;

    temp = (u32)&gButtonsPressed;
    value = *(u16 *)temp;
    temp = 1;
    temp &= value;
    if (temp != 0) {
        value = (u32)&gWarioData;
        temp = 0x40;
        ((struct WarioData *)value)->verticalDirection = temp;
        return 2;
    }

    temp = 2;
    temp &= value;
    temp <<= 16;
    zero = temp >> 16;
    if (zero != 0) {
        wario = &gWarioData;
        temp = (u32)&gButtonsHeld;
        value = *(u16 *)temp;
        temp = 0xC0;
        temp &= value;
        wario->verticalDirection = temp;
        temp = 0x30;
        temp &= value;
        if (temp != 0)
            return 1;
        return 2;
    }

    heldPointer = &gButtonsHeld;
    wario = &gWarioData;
    value = wario->horizontalDirection;
    ASM_BARRIER(: "+r"(value));
    temp = 0x30;
    opposite = value;
    opposite ^= temp;
    value = *heldPointer;
    temp = opposite;
    temp &= value;
    if (temp != 0) {
        wario->horizontalDirection = opposite;
        wario->xVelocity = zero;
    } else {
        temp = 0xF0;
        temp &= value;
        if (temp == 0)
            return 5;
    }

    value = *heldPointer;
    temp = 0x10;
    temp &= value;
    if (temp != 0) {
        temp = 0x20;
    } else {
        temp = 0x20;
        temp &= value;
        temp = (u16)temp;
        if (temp != 0)
            temp = 0xFFE0;
    }
    wario->xVelocity = temp;

    value = *heldPointer;
    temp = 0x40;
    temp &= value;
    if (temp != 0) {
        temp = 0x20;
    } else {
        temp = 0x80;
        temp &= value;
        temp = (u16)temp;
        if (temp != 0)
            temp = 0xFFE0;
    }
    wario->yVelocity = temp;

    temp = *heldPointer;
    value = 0xC0;
    value &= temp;
    opposite = 0;
    wario->verticalDirection = value;
    value = (u32)&gWarioDustEffect2;
    temp = *(u8 *)value;
    if (temp == 0) {
        temp = 3;
        *(u8 *)value = temp;
    }

    animation = sWarioSwimmingFreelyAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = opposite;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F = opposite;
    }
    return 0xFF;
}

u8 UpdateWarioFloating(void)
{
    u32 temp;
    u32 value;
    u32 buttons;
    register u32 direction PIN("r3");
    register u32 moving PIN("r4");
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;
    u32 frame;
    u32 zero;

    temp = (u32)&gButtonsPressed;
    value = *(u16 *)temp;
    temp = 1;
    temp &= value;
    if (temp != 0) {
        value = (u32)&gWarioData;
        buttons = 0;
        *(u8 *)value = buttons;
        temp = 2;
        ((struct WarioData *)value)->unk_08 = temp;
        ((struct WarioData *)value)->xVelocity = buttons;
        temp = 0x80;
        ((struct WarioData *)value)->yVelocity = temp;
        value = (u32)&gWarioDustEffect1;
        temp = 3;
        *(u8 *)value = temp;
        m4aSongNumStart(SE_TOBAWANI_KNOCKBACK);
        return 0xFE;
    }

    temp = 2;
    temp &= value;
    if (temp != 0) {
        value = (u32)&gWarioData;
        temp = ((struct WarioData *)value)->yPosition;
        temp += 0x10;
        ((struct WarioData *)value)->yPosition = temp;
        return 1;
    }

    value = (u32)&gButtonsHeld;
    buttons = *(u16 *)value;
    temp = 0x80;
    temp &= buttons;
    if (temp != 0) {
        value = (u32)&gWarioData;
        temp = ((struct WarioData *)value)->yPosition;
        temp += 0x10;
        ((struct WarioData *)value)->yPosition = temp;
        return 3;
    }

    temp = (u32)&gWarioData;
    direction = ((struct WarioData *)temp)->horizontalDirection;
    moving = buttons;
    moving &= direction;
    wario = (struct WarioData *)temp;
    if (moving != 0) {
        temp = 0x10;
        temp &= direction;
        if (temp != 0) {
            temp = 0x20;
            wario->xVelocity = temp;
        } else {
            temp = 0xFFE0;
            wario->xVelocity = temp;
        }
    } else {
        temp = 0x30;
        direction ^= temp;
        temp = direction;
        temp &= buttons;
        temp <<= 16;
        if (temp != 0)
            wario->horizontalDirection = direction;
        wario->xVelocity = moving;
    }

    value = *(u16 *)value;
    temp = 0x30;
    temp &= value;
    if (temp != 0) {
        temp = wario->unk_1F;
        if (temp > 5)
            return 4;
        temp = 0;
        wario->unk_0A = temp;
    }

    /* Prevent agbcc from hoisting the animation-frame load across the state update. */
    ASM_BARRIER_VOLATILE(::: "memory");
    animation = sWarioFloatingAnimation;
    frame = wario->unk_1F;
    value = frame << 1;
    value += frame;
    value <<= 2;
    value += (u32)animation;
    temp = wario->unk_1E;
    value = *(u8 *)(value + 8);
    if (temp >= value) {
        if (frame == 0)
            m4aSongNumStart(SE_WARIO_WATER_PADDLE);
        zero = 0;
        wario->unk_1E = zero;
        temp = wario->unk_1F;
        temp++;
        wario->unk_1F = temp;
        temp = (u8)temp;
        if (temp == 6) {
            temp = wario->unk_0A;
            if (temp <= 3) {
                temp++;
                wario->unk_0A = temp;
                wario->unk_1F = zero;
            }
        }
        direction = (u32)sWarioFloatingAnimation;
        buttons = (u32)&gWarioData;
        value = ((struct WarioData *)buttons)->unk_1F;
        temp = value << 1;
        temp += value;
        temp <<= 2;
        temp += direction;
        temp = *(u8 *)(temp + 8);
        wario = (struct WarioData *)buttons;
        if (temp == 0) {
            temp = 6;
            wario->unk_1F = temp;
        }
    }

    moving = (u32)wario;
    value = *(u8 *)(moving + 31);
    if (value <= 5) {
        temp = (u32)&gCurrentWarioEffect;
        *(u8 *)(temp + 3) = value;
    } else {
        temp = *(u16 *)(moving + 12);
        if (temp == 0)
            m4aSongNumStart(SE_WARIO_WATER_IDLE);
        temp = *(u16 *)(moving + 12);
        if (temp <= 0x4F) {
            temp++;
            *(u16 *)(moving + 12) = temp;
        } else {
            temp = 0;
            wario->unk_0C = temp;
        }
        value = (u32)&gCurrentWarioEffect;
        temp = 0;
        *(u8 *)value = temp;
    }
    return 0xFF;
}

u8 UpdateWarioJumpingOutOfWater(void)
{
    u32 temp;
    register u32 buttons PIN("r1");
    u32 value;
    struct WarioData *callWario;
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;

    temp = (u32)&gButtonsPressed;
    buttons = *(u16 *)temp;
    value = 1;
    value &= buttons;
    if (value != 0) {
        buttons = (u32)&gWarioData;
        temp = 0x40;
        ((struct WarioData *)buttons)->verticalDirection = temp;
        return 2;
    }

    temp = 2;
    temp &= buttons;
    if (temp != 0) {
        temp = (u32)&gWarioData;
        ((struct WarioData *)temp)->yVelocity = value;
        return 1;
    }

    temp = 0x40;
    temp &= buttons;
    if (temp != 0) {
        callWario = &gWarioData;
        temp = GetBackgroundCollisionAtPosition(callWario->yPosition, callWario->xPosition);
        temp = (s32)temp >> 16;
        if (temp == 1) {
            temp = callWario->yPosition;
            temp -= 0x3E;
            temp = (u16)temp;
            temp = GetBackgroundCollisionAtPosition(temp, callWario->xPosition);
            temp = (s32)temp >> 16;
            if (temp == 1)
                return 11;
        }
    }

    temp = (u32)&gButtonsHeld;
    temp = *(u16 *)temp;
    buttons = 0x30;
    buttons &= temp;
    if (buttons != 0) {
        temp = (u32)&gWarioData;
        ((struct WarioData *)temp)->horizontalDirection = buttons;
        return 3;
    }

    buttons = 0xC0;
    buttons &= temp;
    if (buttons != 0) {
        temp = (u32)&gWarioData;
        ((struct WarioData *)temp)->verticalDirection = buttons;
        return 3;
    }

    temp = (u32)&gWarioData;
    buttons = ((struct WarioData *)temp)->unk_1F;
    wario = (struct WarioData *)temp;
    if (buttons != 0) {
        temp = 0x20;
        wario->yVelocity = temp;
        temp = 0x40;
        wario->verticalDirection = temp;
    } else {
        wario->yVelocity = buttons;
    }

    animation = sWarioJumpingOutOfWaterAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F = 1;
    }
    return 0xFF;
}

u8 UpdateWarioWaterDamage(void)
{
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;
    u8 zero;

    wario = &gWarioData;
    if (wario->unk_0A > 0x50) {
        wario->damageTimer = 0x60;
        return 5;
    }
    wario->unk_0A++;
    zero = 0;
    wario->yVelocity = 0x20;
    animation = sWarioTakingDamageAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = zero;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F = zero;
    }
    return 0xFF;
}

u8 UpdateWarioWaterSuperGroundPound(void)
{
    u32 temp;
    register u32 value PIN("r1");
    struct WarioData *wario;
    u32 held;
    register u32 direction PIN("r2");
    register u32 moving PIN("r5");
    const struct WarioAnimationFrame *animation;
    struct WarioAfterimage *afterimage;
    const struct WarioEffectFrame *afterimageAnimation;
    u8 afterimageFrame;

    temp = (u32)&gButtonsHeld;
    value = (u32)&gWarioData;
    held = *(u16 *)temp;
    direction = ((struct WarioData *)value)->horizontalDirection;
    moving = held;
    moving &= direction;
    wario = (struct WarioData *)value;
    if (moving != 0) {
        value = 0x10;
        temp = value;
        temp &= direction;
        if (temp != 0)
            wario->xVelocity = value;
        else
            wario->xVelocity = -0x10;
    } else {
        temp = 0x30;
        value = direction;
        value ^= temp;
        temp = value;
        temp &= held;
        temp <<= 16;
        ASM_BARRIER(: "+r"(temp));
        if (temp != 0)
            wario->horizontalDirection = value;
        wario->xVelocity = moving;
    }

    animation = sWarioGroundPoundAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F = 1;
    }

    afterimage = &gWarioMotionAfterimage;
    if (afterimage->unk0 != 0) {
        afterimageAnimation = sWarioAfterimageFrames;
        if (afterimage->unk1 >= afterimageAnimation[afterimage->unk2].time) {
            afterimage->unk1 = 0;
            afterimage->unk2++;
            afterimageFrame = afterimage->unk2;
            if (afterimageAnimation[afterimageFrame].time == 0)
                afterimage->unk2 = afterimageFrame - 1;
        }
    }
    return 0xFF;
}

u8 UpdateWarioWaterSuperGroundPoundLanding(void)
{
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;
    struct WarioAfterimage *afterimage;
    const struct WarioEffectFrame *afterimageAnimation;

    wario = &gWarioData;
    animation = sWarioGroundPoundLandingAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            return 5;
    }

    afterimage = &gWarioMotionAfterimage;
    afterimageAnimation = sWarioGroundPoundLandingAfterimageFrames;
    if (afterimage->unk1 >= afterimageAnimation[afterimage->unk2].time) {
        afterimage->unk1 = 0;
        afterimage->unk2++;
        if (afterimageAnimation[afterimage->unk2].time == 0)
            afterimage->unk2 = 0;
    }
    return 0xFF;
}

u8 UpdateWarioWaterHorizontalBonk(void)
{
    struct WarioData *wario;

    wario = &gWarioData;
    if (wario->horizontalDirection & 0x10) {
        wario->xVelocity += 2;
        if (wario->xVelocity >= 0)
            return 5;
    } else {
        wario->xVelocity -= 2;
        if (wario->xVelocity <= 0)
            return 5;
    }
    return 0xFF;
}

u8 UpdateWarioWaterVerticalBonk(void)
{
    struct WarioData *wario;

    wario = &gWarioData;
    if (wario->verticalDirection & 0x40) {
        wario->yVelocity += 2;
        if (wario->yVelocity >= 0)
            return 5;
    } else {
        wario->yVelocity -= 2;
        if (wario->yVelocity <= 0)
            return 5;
    }
    return 0xFF;
}

u8 UpdateWarioWaterTransition(void)
{
    if (gWarioData.unk_1E > 3) {
        gSubGameMode = 2;
        return 5;
    }
    return 0xFF;
}

u8 UpdateFatWarioFallingInWater(void)
{
    u32 temp;
    struct WarioData *wario;
    register u32 held PIN("r2");
    register u32 direction PIN("r1");
    register u32 moving PIN("r3");

    if (gWarioDustEffect2.unk0 == 0)
        gWarioDustEffect2.unk0 = 1;

    temp = (u32)&gButtonsHeld;
    wario = &gWarioData;
    held = *(u16 *)temp;
    direction = wario->horizontalDirection;
    moving = held;
    moving &= direction;
    if (moving != 0) {
        held = 0x10;
        temp = held;
        temp &= direction;
        if (temp != 0)
            wario->xVelocity = held;
        else
            wario->xVelocity = -0x10;
    } else {
        temp = 0x30;
        direction ^= temp;
        temp = direction;
        temp &= held;
        temp <<= 16;
        if (temp != 0)
            wario->horizontalDirection = direction;
        wario->xVelocity = moving;
    }
    return 0xFF;
}

u8 UpdateFatWarioLandingInWater(void)
{
    struct WarioAfterimage *afterimage;
    const struct WarioEffectFrame *animation;

    if (gWarioData.unk_1E > 0x1D) {
        gWarioData.damageTimer = 0x60;
        return 5;
    }

    afterimage = &gWarioMotionAfterimage;
    animation = sWarioGroundPoundLandingAfterimageFrames;
    if (afterimage->unk1 >= animation[afterimage->unk2].time) {
        afterimage->unk1 = 0;
        afterimage->unk2++;
        if (animation[afterimage->unk2].time == 0)
            afterimage->unk2 = 0;
    }
    return 0xFF;
}

u8 UpdateBubbleWarioInWater(void)
{
    register u32 temp PIN("r0");
    register u32 value PIN("r1");
    struct WarioData *wario;
    register u32 aux PIN("r3");
    struct WarioData *initialWario;
    const struct WarioAnimationFrame *animation;

    initialWario = &gWarioData;
    value = (u32)initialWario;
    value += 0x3B;
    temp = *(u8 *)value;
    if (temp != 0) {
        temp--;
        *(u8 *)value = temp;
    } else {
        wario = (struct WarioData *)&gButtonsHeld;
        value = initialWario->horizontalDirection;
        ASM_BARRIER(: "+r"(value));
        temp = 0x30;
        aux = value;
        aux ^= temp;
        value = *(u16 *)wario;
        temp = aux;
        temp &= value;
        if (temp != 0) {
            temp = 0;
            initialWario->horizontalDirection = aux;
            value = (u32)initialWario;
            value += 0x3A;
            *(u8 *)value = temp;
            m4aSongNumStart(SE_BUBBLE_WARIO_TURN);
        }
    }

    wario = &gWarioData;
    value = wario->horizontalDirection;
    temp = 0x10;
    temp &= value;
    if (temp != 0) {
        value = (u32)sHorizontalOscillationVelocity;
        temp = (u32)wario;
        temp += 0x3A;
        temp = *(u8 *)temp;
        temp >>= 3;
        temp <<= 1;
        temp += value;
        temp = *(u16 *)temp;
        aux = wario->xPosition;
        value = temp + aux;
        wario->xPosition = value;
        temp = (u32)wario;
        temp += 0x3B;
        temp = *(u8 *)temp;
        if (temp != 0) {
            temp = value + 1;
            wario->xPosition = temp;
        }
    } else {
        value = (u32)sHorizontalOscillationVelocity;
        temp = (u32)wario;
        temp += 0x3A;
        temp = *(u8 *)temp;
        temp >>= 3;
        temp <<= 1;
        temp += value;
        value = wario->xPosition;
        temp = *(u16 *)temp;
        value -= temp;
        wario->xPosition = value;
        temp = (u32)wario;
        temp += 0x3B;
        temp = *(u8 *)temp;
        if (temp != 0) {
            temp = value - 1;
            wario->xPosition = temp;
        }
    }

    aux = (u32)wario;
    aux += 0x3A;
    temp = *(u8 *)aux;
    if (temp <= 0x5E) {
        temp++;
        *(u8 *)aux = temp;
    } else {
        temp = wario->horizontalDirection;
        value = 0x30;
        temp ^= value;
        value = 0;
        wario->horizontalDirection = temp;
        *(u8 *)aux = value;
    }

    temp = wario->unk_0A;
    if (temp != 0) {
        temp = 0x20;
        wario->yVelocity = temp;
    } else {
        temp++;
        wario->unk_0A = temp;
    }

    animation = sBubbleWarioAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F = 0;
    }
    return 0xFF;
}

u8 UpdateWarioWaterRecovery(void)
{
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;

    wario = &gWarioData;
    if (wario->unk_0A > 0x50) {
        wario->damageTimer = 0x60;
        return 5;
    }
    wario->unk_0A++;
    if (gWarioDustEffect2.unk0 == 0)
        gWarioDustEffect2.unk0 = 1;

    animation = sWarioFloatingAnimation;
    if (wario->unk_1E >= animation[wario->unk_1F].time) {
        wario->unk_1E = 0;
        wario->unk_1F++;
        if (animation[wario->unk_1F].time == 0)
            wario->unk_1F = 6;
    }
    return 0xFF;
}

void SetWarioWaterPose(u8 pose)
{
    struct WarioData *wario;
    struct WarioData *copy;
    register struct WarioData *current PIN("r0");
    s16 velocity;

    PrepareWarioUpdate();
    gWarioMotionAfterimage = sEmptyWarioAfterimage;
    gCurrentWarioEffect = sStartingWarioEffect;

    if (gPreviousReaction == gWarioDataCopy.reaction && gWarioDataCopy.pose == 7) {
        if (pose == 8) {
            m4aSongNumStart(SE_WARIO_WATER_GROUND_POUND_LAND);
            VoiceSetPlay(0);
        } else {
            MPlayStop(GP(gMPlayTable[3].info));
        }
    }

    current = &gWarioData;
    current->pose = pose;
    copy = &gWarioDataCopy;
    velocity = copy->yVelocity;
    current->yVelocity = velocity;
    wario = current;

    switch (pose) {
        case 0:
            gCurrentCarriedSprite.state = 0;
            gWarioDustEffect1.unk0 = 1;
            if (copy->yVelocity > -0x6C)
                wario->yVelocity = -0x6C;
            /* fall through */
        case 12:
            m4aSongNumStart(SE_WARIO_WATER_LAND);
            break;
        case 1:
            if (wario->horizontalDirection & 0x10)
                wario->xVelocity = 0x80;
            else
                wario->xVelocity = -0x80;
            gCurrentWarioEffect.type = 1;
            m4aSongNumStart(SE_WARIO_SWIM);
            break;
        case 2:
            if (wario->verticalDirection & 0x40)
                wario->yVelocity = 0x80;
            else
                wario->yVelocity = -0x80;
            gCurrentWarioEffect.type = 1;
            m4aSongNumStart(SE_WARIO_SWIM);
            break;
        case 4:
            wario->yVelocity = 0;
            gCurrentWarioEffect.type = 1;
            break;
        case 15:
            wario->unk_1F = 6;
            gWarioDustEffect2.unk0 = 1;
            /* fall through */
        case 6:
            if (gHeartMeter.unk_2 == 0 && gHeartMeter.current != 0) {
                gHeartMeter.current--;
                SpawnSecondarySprite(gWarioData.yPosition - 0x60, gWarioData.xPosition, 0x30);
            }
            gWarioData.damageTimer = 1;
            m4aSongNumStart(SE_WARIO_WATER_DAMAGE);
            break;
        case 7:
            wario->unk_1F = 1;
            gWarioMotionAfterimage.unk0 = 2;
            gCurrentCarriedSprite.state = 0;
            gWarioDustEffect1.unk0 = 1;
            break;
        case 8:
            wario->yVelocity = 0;
            gWarioMotionAfterimage.unk0 = 3;
            gWarioDustEffect2.unk0 = 5;
            ScreenShakeRequestY(0x40, 0);
            break;
        case 9:
            if (wario->horizontalDirection & 0x10)
                wario->xVelocity = -0x24;
            else
                wario->xVelocity = 0x24;
            gWarioDustEffect2.unk0 = 5;
            break;
        case 10:
            if (wario->verticalDirection & 0x40)
                wario->yVelocity = -0x24;
            else
                wario->yVelocity = 0x24;
            gWarioDustEffect2.unk0 = 5;
            break;
        case 11:
            gSubGameMode = 4;
            gWarioSubgameXPosition = copy->xPosition;
            gWarioSubgameYPosition = copy->yPosition;
            break;
        case 13:
            gWarioMotionAfterimage.unk0 = 4;
            gWarioDustEffect2.unk0 = 5;
            ScreenShakeRequestY(0x40, 0);
            m4aSongNumStart(SE_SNOWMAN_WARIO_LAND);
            break;
        case 14:
            wario->yVelocity = 0;
            m4aSongNumStart(SE_BUBBLE_WARIO_START);
            break;
        default:
            break;
    }
}


void UpdateWarioWaterPhysics(void)
{
    s16 movement;
    u8 pose;
    struct WarioData *wario;

    UpdateWarioTileMovement();

    {
        const u8 *poseData;
        register struct WarioData *hitboxWario;
        u32 hitboxOffset;
        const u8 *hitboxData;
        u32 hitboxAddress;

        poseData = sWarioWaterPoseProperties;
        hitboxWario = &gWarioData;
        hitboxOffset = poseData[hitboxWario->pose * 8];
        hitboxData = (const u8 *)sWarioHitboxes;
        hitboxOffset <<= 3;
        hitboxAddress = hitboxOffset + (u32)hitboxData;
        hitboxWario->hitboxOffsetLeft = *(const u16 *)hitboxAddress;
        hitboxAddress = (u32)hitboxData + 2;
        hitboxAddress = hitboxOffset + hitboxAddress;
        hitboxWario->hitboxOffsetTop = *(const u16 *)hitboxAddress;
        hitboxAddress = (u32)hitboxData + 4;
        hitboxAddress = hitboxOffset + hitboxAddress;
        hitboxWario->hitboxOffsetRight = *(const u16 *)hitboxAddress;
        hitboxData += 6;
        hitboxOffset += (u32)hitboxData;
        hitboxWario->hitboxOffsetBottom = *(const u16 *)hitboxOffset;

        pose = hitboxWario->pose;
        movement = 0;
        switch (pose) {
            case 0:
                movement = gWarioData.yVelocity >> 3;
                gWarioData.yVelocity += 0x10;
                break;
            case 1:
            case 3:
            case 5:
                if (gWarioTileVerticalSpeed == 0x60 || gWarioTileVerticalSpeed == -0x60)
                    movement = gWarioTileVerticalSpeed >> 3;
                else
                    movement = (gWarioData.yVelocity + gWarioTileVerticalSpeed) >> 3;
                break;
            case 2:
                wario = &gWarioData;
                movement = wario->yVelocity >> 3;
                if (wario->verticalDirection & 0x40) {
                    if (gWarioTileVerticalSpeed == 0x60) {
                        movement = gWarioTileVerticalSpeed >> 3;
                    } else if (gWarioTileVerticalSpeed == -0x60) {
                        movement = (wario->yVelocity + gWarioTileVerticalSpeed) >> 3;
                        if (movement > 0)
                            movement = 0;
                    } else if (movement > 0xC) {
                        movement = 0xC;
                    }
                } else {
                    if (gWarioTileVerticalSpeed == -0x60) {
                        movement = gWarioTileVerticalSpeed >> 3;
                    } else if (gWarioTileVerticalSpeed == 0x60) {
                        movement = (wario->yVelocity + gWarioTileVerticalSpeed) >> 3;
                        if (movement < 0)
                            movement = 0;
                    } else if (movement < -0xC) {
                        movement = -0xC;
                    }
                }
                break;
            case 7:
            case 12:
                if (gWarioData.yVelocity < -0x60)
                    gWarioData.yVelocity = -0x60;
                movement = gWarioData.yVelocity >> 3;
                gWarioData.yVelocity -= 8;
                break;
            case 6:
            case 10:
            case 14:
                movement = gWarioData.yVelocity >> 3;
                break;
        }
    }
    {
        register struct WarioData *updatedWario PIN("r1");
        s32 currentYVelocity;
        register s32 minimumVelocity PIN("r0");

        updatedWario = &gWarioData;
        updatedWario->yPosition -= movement;
        currentYVelocity = updatedWario->yVelocity;
        minimumVelocity = 0x80;
        minimumVelocity = -minimumVelocity;
        wario = updatedWario;
        if (currentYVelocity < minimumVelocity)
            wario->yVelocity = 0xFF80;
    }

    movement = 0;
    switch (wario->pose) {
        case 1:
            movement = wario->xVelocity >> 3;
            if (wario->horizontalDirection & 0x10) {
                if (gWarioTileHorizontalSpeed == 0x60) {
                    movement = gWarioTileHorizontalSpeed >> 3;
                } else if (gWarioTileHorizontalSpeed == -0x60) {
                    movement = (wario->xVelocity + gWarioTileHorizontalSpeed) >> 3;
                    if (movement > 0)
                        movement = 0;
                } else if (movement > 0xC) {
                    movement = 0xC;
                }
            } else {
                if (gWarioTileHorizontalSpeed == -0x60) {
                    movement = gWarioTileHorizontalSpeed >> 3;
                } else if (gWarioTileHorizontalSpeed == 0x60) {
                    movement = (wario->xVelocity + gWarioTileHorizontalSpeed) >> 3;
                    if (movement < 0)
                        movement = 0;
                } else if (movement < -0xC) {
                    movement = -0xC;
                }
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            if (gWarioTileHorizontalSpeed == 0x60 || gWarioTileHorizontalSpeed == -0x60)
                movement = gWarioTileHorizontalSpeed >> 3;
            else
                movement = (wario->xVelocity + gWarioTileHorizontalSpeed) >> 3;
            break;
        case 14:
            break;
        default:
            movement = wario->xVelocity >> 3;
            break;
    }
    wario->xPosition += movement;
}

void ProcessWarioWaterCollision(void)
{
    struct WarioCollisionData *collisionData;
    const u8 *poseData;
    register const u8 *fieldData PIN("r1");
    u32 poseOffset;
    struct WarioData *wario;
    u16 collisionResult;
    s32 checkResult;
    u8 result;
    struct WarioData *updatedWario;
    struct WarioData *filterWario;
    s32 poseValue;
    register struct WarioData *collisionWario PIN("r4");
    u32 groundResult;
    u32 activePose;
    struct WarioData *alignWario;
    register u32 alignedY PIN("r1");
    register u32 alignMask PIN("r2");
    register s32 negativeMask PIN("r3");
    register s32 holdR1 PIN("r1");
    register s32 holdR2 PIN("r2");

    collisionData = &gWarioCollisionData;
    poseData = sWarioWaterPoseProperties;
    wario = &gWarioData;
    poseOffset = wario->pose * 8;
    fieldData = poseData + 1;
    poseOffset += (u32)fieldData;
    collisionData->unk_08 = *(const u8 *)poseOffset;
    poseOffset = wario->pose * 8;
    fieldData = poseData + 2;
    poseOffset += (u32)fieldData;
    collisionData->unk_09 = *(const u8 *)poseOffset;
    poseOffset = wario->pose * 8;
    fieldData = poseData + 3;
    poseOffset += (u32)fieldData;
    collisionData->unk_0A = *(const u8 *)poseOffset;
    poseOffset = wario->pose * 8;
    fieldData = poseData + 4;
    poseOffset += (u32)fieldData;
    collisionData->unk_0B = *(const u8 *)poseOffset;
    poseOffset = wario->pose * 8;
    fieldData = poseData + 5;
    poseOffset += (u32)fieldData;
    collisionData->unk_0C = *(const u8 *)poseOffset;
    poseOffset = wario->pose * 8;
    fieldData = poseData + 6;
    poseOffset += (u32)fieldData;
    collisionData->unk_0D = *(const u8 *)poseOffset;
    poseOffset = wario->pose * 8;
    poseData += 7;
    poseOffset += (u32)poseData;
    collisionData->unk_11 = *(const u8 *)poseOffset;

    if (wario->yVelocity <= 0x2F)
        collisionData->unk_0C = 0;

    if (collisionData->unk_00 == 0)
        collisionData->unk_00 = wario->horizontalDirection & 0x30;

    if (collisionData->flags & 0x40) {
        result = ResolveWarioWaterSurfaceCollision();
    } else if (*(s8 *)&gWarioWaterCollisionActive == 0) {
        result = ResolveWarioWaterCollision();
    } else {
        wario->yPosition = gPreviousYPosition;
        result = ResolveWarioWaterCollisionContinuation();
        if (wario->pose == 3 && *(u16 *)&wario->unk_1C == 0 && (gButtonsHeld & 0x80)) {
            if (((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(0, 1, 1) == 0) {
                wario->xPosition = (wario->xPosition & 0xFFC0) + 0x20;
                wario->yPosition++;
                result = 0xFF;
            }
        }
    }

    updatedWario = &gWarioData;
    *(u16 *)&updatedWario->unk_1C &= 0x7FFF;
    wario = updatedWario;
    if (result == 0xFD)
        gWarioWaterCollisionActive = 1;
    else
        gWarioWaterCollisionActive = 0;

    switch (wario->pose) {
        case 12:
            if (result == 0xFD) {
                checkResult = ((s32 (*)(u8, u16, u16 *))CheckWarioPointCollision)(gWarioCollisionData.unk_08, gWarioData.yPosition + 1, &collisionResult);
                if (checkResult != 0 && checkResult > gWarioCollisionData.unk_10)
                    SetWarioWaterPose(13);
            }
            goto stateHandled;
        case 7:
            if (result == 0xFD) {
                checkResult = ((s32 (*)(u8, u16, u16 *))CheckWarioPointCollision)(gWarioCollisionData.unk_08, gWarioData.yPosition + 1, &collisionResult);
                if (checkResult != 0 && checkResult > gWarioCollisionData.unk_10)
                    SetWarioWaterPose(8);
            }
            goto stateHandled;
        case 1:
        case 2:
        case 3:
        case 5:
            if (result == 0xFF) {
                ASM_BARRIER_VOLATILE(: "=r"(holdR1), "=r"(holdR2));
                if (gWarioTileHorizontalSpeed == 0) {
                    ASM_BARRIER_VOLATILE(: : "r"(holdR1), "r"(holdR2));
                    if (gWarioTileVerticalSpeed == 0)
                        result = CheckWarioTileMovement();
                }
            }
            goto handleStateResult;
        default:
handleStateResult:
            if (result != 0xFF && result != 0xFD)
                SetWarioWaterPose(result);
            break;
    }

stateHandled:
    filterWario = &gWarioData;
    poseValue = filterWario->pose;
    wario = filterWario;
    if (poseValue == 4)
        goto collisionDone;
    if (poseValue > 4)
        goto greaterPose;
    if (poseValue == 0)
        goto collisionDone;
    goto processCollision;

greaterPose:
    if (poseValue == 7 || poseValue == 12)
        goto collisionDone;

processCollision:
    collisionWario = wario;
    groundResult = GetBackgroundCollisionAtPosition(collisionWario->yPosition - 0x30, collisionWario->xPosition);
    groundResult &= 0xFF;
    if (groundResult != 0)
        goto collisionDone;

    activePose = collisionWario->pose;
    if (activePose == 6) {
        collisionWario->yVelocity = groundResult;
    } else if (activePose == 14) {
        SetWarioWaterPose(0);
        wario->damageTimer = 0x60;
        m4aSongNumStart(SE_BUBBLE_WARIO_BOUNCE);
        SpawnPrimarySpriteWithStatus(0x9B, 0, 0, wario->yPosition, wario->xPosition, 0x40);
    } else {
        SetWarioWaterPose(4);
    }

    alignWario = &gWarioData;
    alignedY = alignWario->yPosition;
    alignedY -= 0x30;
    negativeMask = 0x40;
    negativeMask = -negativeMask;
    alignMask = negativeMask;
    ASM_BARRIER(: "+r"(alignMask), "+r"(negativeMask));
    alignedY &= alignMask;
    alignedY += 0x60;
    alignWario->yPosition = alignedY;

collisionDone:
}


u8 ResolveWarioWaterCollision(void)
{
    struct WarioCollisionData *collisionData;
    struct WarioData *wario;
    unsigned short secondProbeX;
    u16 newX;
    u16 newY;
    int wallCheckVelocity;
    u16 *newYPointer;
    u16 collision;
    u32 result;
    s16 offset;
    register u32 index PIN("r5");
    struct WarioData **warioPointer;
    u32 point;
    u32 indexOffset;
    int firstProbeX;
    s32 probeOffset;
    u32 tableOffset;
    register u32 rawIndexOffset PIN("r1");
    u8 alternatePoint;
    register unsigned short storedY PIN("r0");
    register struct WarioData *storedWarioR2 PIN("r2");
    register struct WarioData *storedWarioR1 PIN("r1");
    collisionData = &gWarioCollisionData;
    index = collisionData->unk_08;
    point = collisionData->unk_04;
    alternatePoint = collisionData->unk_05;
    offset = collisionData->unk_06;
    if (collisionData->unk_00 != 0)
    {
        wallCheckVelocity = -22;
        result = ((u32 (*)(u8, u16 *, s32))CheckWarioHorizontalCollision)(index, &newX, wallCheckVelocity);
        if (result != 0)
        {
            struct WarioData *initialWario;
            initialWario = &gWarioData;
            initialWario->xPosition = newX;
            initialWario->xVelocity = 0;
            collisionData->unk_12++;
            if (initialWario->pose == 1)
            {
                return 9;
            }
        }
    }
    wario = &gWarioData;
    tableOffset = point << 1;
    rawIndexOffset = index << 1;
    rawIndexOffset += index;
    rawIndexOffset <<= 1;
    indexOffset = rawIndexOffset;
    tableOffset += indexOffset;
    probeOffset = *((const u16 *) (((const u8 *) sWarioCollisionPointTable) + tableOffset));
    tableOffset = wario->xPosition;
    tableOffset += probeOffset;
    tableOffset <<= 16;
    index = tableOffset >> 16;
    point = wario->yPosition;
    ASM_BARRIER_VOLATILE(: : "r"(index));
    firstProbeX = index;
    newYPointer = &newY;
    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(firstProbeX, point, &newX, newYPointer, &collision);
    if (collision != 0)
    {
        if (result != 0)
        {
            wario->unk_1C = result;
            storedY = newY;
            storedWarioR2 = wario;
            goto storeYWithR2;
        }
        if (gPreviousYPosition < newY)
        {
            point = gPreviousYPosition;
            result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(firstProbeX, point, &newX, &newY, &collision);
            if (result != 0)
            {
                storedY = newY;
                storedWarioR1 = wario;
                goto storeYWithR1;
            }
            wario->yPosition = newY + 0x3F;
            ASM_BARRIER_VOLATILE(: : "r"(wario), "r"(result));
            goto returnFD;
        }
        if (collision == 0x10)
        {
            goto collisionType16;
        }
        wario->xPosition = (newX - probeOffset) + offset;
        index = wario->xPosition;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(index, point, &newX, &newY, &collision);
        wario->unk_1C = result;
        if (result != 0)
        {
            storedY = newY;
            storedWarioR2 = wario;
            goto storeYWithR2;
        }
        if (collision == 0)
        {
            register struct WarioData *poseWario PIN("r2");
            poseWario = wario;
            poseWario->xVelocity = 0;
            gWarioCollisionData.unk_12++;
            if (poseWario->pose == 1)
            {
                return 9;
            }
            return 0xFF;
        }
        goto storeNewYMinusOne;
    }
    goto secondProbe;
collisionType16:
    storedY = newY - 1;

    storedWarioR1 = wario;
    goto storeYWithR1;
secondProbe:
    {
        register u32 secondValue PIN("r2");
        const u8 *tableBase;
        struct WarioData *positionWario;
        secondValue = alternatePoint;
        ASM_BARRIER_VOLATILE(: "+r"(secondValue));
        tableOffset = secondValue << 1;
        tableOffset += indexOffset;
        /* Preserve the original pointer lifetime across the second collision probe. */
        warioPointer = &wario;
        tableBase = (const u8 *) sWarioCollisionPointTable;
        tableOffset += (u32) tableBase;
        tableOffset = *((const u16 *) tableOffset);
        {
            register struct WarioData *xWario PIN("r2");
            xWario = wario;
            ASM_BARRIER_VOLATILE(: "+r"(xWario));
            secondValue = xWario->xPosition;
        }
        tableOffset += secondValue;
        tableOffset <<= 16;
        index = tableOffset >> 16;
        positionWario = wario;
        ASM_BARRIER_VOLATILE(: "+r"(positionWario));
        point = positionWario->yPosition;
    }

    result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(index, point, &newX, &newY, &collision);
    if (collision != 0)
    {
        secondProbeX = index;
        if (result != 0)
        {
            register struct WarioData *resultWario PIN("r2");
            resultWario = wario;
            resultWario->unk_1C = result;
            storedY = newY;
            storedWarioR2 = resultWario;
            goto storeYWithR2;
        }
        {
            register u16 *previousYPointer PIN("r1");
            register u32 previousY PIN("r0");
            previousYPointer = &gPreviousYPosition;
            previousY = *previousYPointer;
            if (previousY < newY)
            {
                point = previousY;
                result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(secondProbeX, point, &newX, &newY, &collision);
                wario->unk_1C = result;
                if (result != 0)
                {
                    storedY = newY;
                    storedWarioR1 = *warioPointer;
                    goto storeYWithR1;
                }
                storedY = newY + 0x3F;
                storedWarioR2 = wario;
storeYWithR2:
                storedWarioR2->yPosition = storedY;

                goto returnFD;
            }
        }
    }
    else
        if ((*((const s16 *) ((((const u8 *) sWarioCollisionPointTable) + indexOffset) + 2))) > 0x1F)
        {
            index = wario->xPosition;
            result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(index, point, &newX, &newY, &collision);
            if (collision != 0)
            {
                wario->unk_1C = result;
                goto storeNewYMinusOne;
            }
        }
    goto returnFF;
storeNewYMinusOne:
    storedY = newY - 1;

    storedWarioR1 = wario;
storeYWithR1:
    storedWarioR1->yPosition = storedY;

    goto returnFD;
returnFD:
    return 0xFD;

returnFF:
    return 0xFF;

}

u8 ResolveWarioWaterSurfaceCollision(void)
{
    u16 newX;
    u16 newY;
    u16 collision;
    u32 result;
    register u32 index PIN("r9");
    u32 point;
    {
        struct WarioCollisionData *collisionData;
        const u8 *offsetData;
        u32 tableOffset;
        u32 tableIndex;
        const u8 *entry;
        s32 offset;
        u32 status;
        collisionData = &gWarioCollisionData;
        index = collisionData->unk_08;
        status = collisionData->unk_09;
        ASM_BARRIER_VOLATILE(: : "r"(status));
        point = collisionData->unk_04;
        offsetData = (const u8 *) sWarioCollisionPointTable;
        tableIndex = index << 1;
        tableIndex += index;
        tableOffset = tableIndex << 1;
        entry = offsetData + 4;
#ifdef PORTABLE
        /* Keep the GBA's address addition from becoming a 64-bit pointer subtraction. */
        entry += tableOffset;
#else
        entry = ((const u8 *) tableOffset) - (-((u32) entry));
#endif
        offset = *((const u16 *) entry);
        result = ((int (*)(u8, u16, u8))CheckWarioVerticalCollision)(status, offset, 0);
        if (0 == collisionData->unk_0F)
        {
            if (result == 1)
            {
                struct WarioData *wario;
                const u8 *probe;
                u32 probeOffset;
                u32 sum;
                u32 xPosition;
                wario = &gWarioData;
                probe = (const u8 *) (tableOffset + ((u32) offsetData));
                probeOffset = *((const u16 *) probe);
                xPosition = wario->xPosition;
                sum = probeOffset + xPosition;
                xPosition = (u16) sum;
                xPosition &= 0x0000FFC0;
                wario->xPosition = (xPosition - probeOffset) + 0x40;
            }
            else
                if (result == 8)
                {
                    struct WarioData *wario;
                    const u8 *probe;
                    u32 probeOffset;
                    u32 sum;
                    u32 xPosition;
                    wario = &gWarioData;
                    probe = offsetData + 2;
#ifdef PORTABLE
                    probe += tableOffset;
#else
                    probe = ((const u8 *)tableOffset) - (-((u32)probe));
#endif
                    probeOffset = *((const u16 *) probe);
                    xPosition = wario->xPosition;
                    sum = probeOffset + xPosition;
                    xPosition = (u16) sum;
                    xPosition &= 0x0000FFC0;
                    wario->xPosition = (xPosition - probeOffset) - 1;
                }
                else
                    if ((result & 6) != 0)
                    {
                        struct WarioData *wario;
                        register u32 sum PIN("r0");
                        register u32 aligned PIN("r4");
                        register u32 output PIN("r0");
                        wario = &gWarioData;
                        sum = wario->yPosition + offset;
                        aligned = (u16) sum;
                        aligned &= 0x0000FFC0;
                        output = aligned - offset;
                        output += 0x40;
                        wario->yPosition = output;
                        collisionData->unk_13 = 1;
                    }
        }
        else
        {
            struct WarioData *wario;
            register u32 sum PIN("r0");
            register u32 aligned PIN("r4");
            register u32 output PIN("r0");
            wario = &gWarioData;
            sum = wario->yPosition + offset;
            aligned = (u16) sum;
            aligned &= 0x0000FFC0;
            output = aligned - offset;
            output += 0x40;
            wario->yPosition = output;
            collisionData->unk_13 = 1;
        }
    }
    {
        struct WarioCollisionData *collisionData;
        struct WarioData *wario;
        u32 x;
        register u32 y PIN("r4");
        {
            struct WarioCollisionData *address;
            address = &gWarioCollisionData;
            ASM_BARRIER(: : "r"(address));
            collisionData = address;
        }
        if (collisionData->unk_00 != 0)
        {
            do
            {
                wario = &gWarioData;
                {
                    const s16 *table;
                    register u32 indexTemp PIN("r2");
                    register u32 tableIndex PIN("r0");
                    table = (const s16 *)sWarioCollisionPointTable;
                    indexTemp = index;
                    ASM_BARRIER(: : "r"(indexTemp));
                    tableIndex = indexTemp << 1;
                    tableIndex += index;
                    tableIndex += point;
                    tableIndex <<= 1;
                    tableIndex += (u32)table;
                    point = *(u16 *)tableIndex;
                }
                { register u32 xTemp PIN("r0"); xTemp = wario->xPosition + point; xTemp <<= 16; xTemp >>= 16; x = xTemp; }
                y = wario->yPosition;
                ASM_BARRIER_VOLATILE(: "+r"(collisionData), "+r"(wario) : "r"(y));
                ASM_BARRIER_ARM_VOLATILE(: : : "r0", "r1", "r2", "r5");
                result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &newX, &newY, &collision);
                if (collision != 0)
                {
                    do
                    {
                    }
                    while (0);
                    do
                    {
                        if (result != 0)
                        {
                            wario->unk_1C = result;
                        }
                        else
                        {
                            {
                                u32 value;
                                u32 oldPoint;
                                struct WarioCollisionData *data;
                                value = newX;
                                oldPoint = point;
                                value -= oldPoint;
                                data = collisionData;
                                value += data->unk_06;
                                point = (u16)value;
                            }
                            {
                                register u32 secondX PIN("r3");
                                secondX = wario->xPosition;
                                result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(secondX, y, &newX, &newY, &collision);
                            }
                            if (result != 0)
                            {
                                u32 pointOut;
                                wario->unk_1C = result;
                                pointOut = point;
                                wario->xPosition = pointOut;
                            }
                            else
                            {
                                goto noCollision;
                            }
                        }
                        ASM_BARRIER_VOLATILE(: : "r"(&collision));
                        wario->yPosition = newY;
                        return 0xFD;
                    }
                    while (0);
                }
noCollision:
                result = ((u32 (*)(u8, u16 *, s32))CheckWarioHorizontalCollision)(index, &newX, 0);
            }
            while (0);
            if (result != 0)
            {
                struct WarioData *wario2;
                wario2 = &gWarioData;
                wario2->xPosition = newX;
                wario2->xVelocity = 0;
                if (wario2->pose == 1)
                {
                    return 9;
                }
                if (wario2->pose == 14)
                {
                    wario2->horizontalDirection ^= 0x30;
                    wario2->unk_3A = 0;
                    wario2->unk_3B = 0x10;
                    m4aSongNumStart(SE_BUBBLE_WARIO_TURN);
                }
            }
        }
    }
    {
        struct WarioCollisionData *collisionData;
        collisionData = &gWarioCollisionData;
        if (collisionData->unk_13 != 0)
        {
            struct WarioData *wario;
            wario = &gWarioData;
            if (((u8) (wario->pose - 1)) <= 1)
            {
                register u32 reserve PIN("r2");
                s32 velocity;
                ASM_BARRIER(: "=r"(reserve));
                velocity = wario->yVelocity;
                ASM_BARRIER(: : "r"(reserve));
                if (velocity > 0xF)
                    return 10;
            }
            if (wario->pose == 14)
            {
                SpawnPrimarySpriteWithStatus(0x9B, 0, 0, wario->yPosition, wario->xPosition, 0x40);
                m4aSongNumStart(SE_BUBBLE_WARIO_BOUNCE);
                return 5;
            }
        }
    }
    return 0xFF;
}

u8 ResolveWarioWaterCollisionContinuation(void)
{
    struct WarioCollisionData *collisionData;
    struct WarioData *initialWario;
    struct WarioData *wario;
    u16 newX;
    u16 newY;
    u16 collision;
    u16 x;
    u16 y;
    u32 result;
    u32 pointOffset;
    u32 indexOffset;
    const u8 *offsetData;
    u32 flagValue;
    s32 movement;
    u16 directionMask;
    register u32 collisionFlags PIN("r6");
    u32 index;
    u32 point;
    u8 alternatePoint;

    collisionData = &gWarioCollisionData;
    directionMask = collisionData->unk_00;
    index = collisionData->unk_08;
    point = collisionData->unk_04;
    alternatePoint = collisionData->unk_05;
    initialWario = &gWarioData;
    flagValue = *(u16 *)&initialWario->unk_1C;
    collisionFlags = flagValue;

    if (collisionFlags == 0) {
        movement = -0x3E;
        result = ((u32 (*)(u8, u16 *, s32))CheckWarioHorizontalCollision)(index, &newX, movement);
        if (result != 0) {
            initialWario->xPosition = newX;
            initialWario->xVelocity = collisionFlags;
            if (initialWario->pose == 1)
                return 9;
        }

        wario = &gWarioData;
        offsetData = (const u8 *)sWarioCollisionPointTable;
        pointOffset = point * 2;
        indexOffset = index * 6;
        pointOffset += indexOffset;
        pointOffset += (u32)offsetData;
        x = wario->xPosition + *(const s16 *)pointOffset;
        y = wario->yPosition;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &newX, &newY, &collision);
        if (collision != 0) {
            if (result != 0) {
                wario->unk_1C = result;
                return (wario->yPosition = newY, 0xFD);
            }
            wario->xPosition = newX;
            return 0xFD;
        }

        flagValue = alternatePoint;
        flagValue <<= 1;
        flagValue += indexOffset;
        flagValue += (u32)offsetData;
        x = wario->xPosition + *(const s16 *)flagValue;
        y++;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &newX, &newY, &collision);
        if (collision != 0 && result == 0) {
            if (collision == 0x10)
                wario->unk_1C = 0x8000;
            return 0xFD;
        }
    } else if ((directionMask & flagValue) != 0) {
        movement = -0x3E;
        result = ((u32 (*)(u8, u16 *, s32))CheckWarioHorizontalCollision)(index, &newX, movement);
        if (result != 0) {
            initialWario->xPosition = newX;
            initialWario->xVelocity = 0;
            if (initialWario->pose == 1)
                return 9;
        }

        wario = &gWarioData;
        {
            const s16 *table;
            u32 tableIndex;

            table = (const s16 *)sWarioCollisionPointTable;
            tableIndex = index << 1;
            tableIndex += index;
            tableIndex += point;
            tableIndex <<= 1;
            tableIndex += (u32)table;
            x = wario->xPosition + *(const s16 *)tableIndex;
        }
        y = wario->yPosition;
        result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &newX, &newY, &collision);
        if (result != 0) {
            wario->unk_1C = result;
            wario->yPosition = newY;
            return 0xFD;
        }
        if (collision != 0) {
            y -= 0x40;
            result = GBA_CALL_AS(CheckWarioTileCollision, u32 (*)(u32, u32, u16 *, u16 *, u16 *))(x, y, &newX, &newY, &collision);
            wario->unk_1C = result;
            if (result != 0)
                wario->yPosition = newY;
            else
                wario->yPosition = newY + 0x3F;
        }
        return 0xFD;
    }

    return 0xFF;
}

void UpdateWarioWaterAnimation(u8 direction)
{
    u32 directionValue;
    u8 *damageFlag;
    register struct WarioData *earlyWario PIN("r1");
    register struct WarioData *loadedWario PIN("r0");
    struct WarioData *wario;
    const struct WarioAnimationFrame *animation;
    const u8 *graphics;
    const u16 *palette;
    u16 *historyCounter;
    struct WarioAfterimageData *history;
    u32 counterValue;
    u32 historyIndex;
    u32 historyDestination;
    u32 tempR0;
    u32 tempR1;
    register u32 tempR2 PIN("r2");
    u32 poseValue;
    s32 palettePose;
    u8 damageTimer;

    directionValue = direction;
    damageFlag = &gWarioDamagePaletteActive;
    *damageFlag = 0;
    earlyWario = &gWarioData;
    if (earlyWario->pose != 6 && earlyWario->pose != 15) {
        damageTimer = earlyWario->damageTimer;
        if (damageTimer != 0) {
            damageTimer--;
            earlyWario->damageTimer = damageTimer;
            *damageFlag = 1;
        }
    }

    if (gWarioData.pose == 7) {
        gWarioDashAfterimage.unk0 = 2;
        gWarioDashAfterimage.unk1 = 0x10;
        historyCounter = &gWarioAfterimageHistoryIndex;
        counterValue = *historyCounter;
        historyIndex = 0x1F;
        history = &gWarioAfterimageHistory;
        historyIndex &= counterValue;
        historyIndex <<= 1;
        historyDestination = historyIndex;
        historyDestination += (u32)history;
        *(u16 *)historyDestination = gPreviousXPosition;
        history = (struct WarioAfterimageData *)((u8 *)history + 0x40);
        historyIndex += (u32)history;
        *(u16 *)historyIndex = gPreviousYPosition;
        counterValue++;
        *historyCounter = counterValue;
        if ((s16)counterValue > 0x1F)
            gWarioAfterimageHistoryWrapped = 1;
    } else {
        UpdateWarioPositionHistory();
    }

    loadedWario = &gWarioData;
    poseValue = loadedWario->pose;
    wario = loadedWario;
    if (poseValue == 2)
        goto poseTwo;
    if (poseValue == 10)
        goto poseTen;
    goto defaultAnimation;

poseTwo:
    if (wario->verticalDirection & 0x80) {
        tempR0 = (u32)sWarioWaterAnimationTable;
        tempR1 = directionValue << 2;
        tempR0 += 8;
        tempR1 += tempR0;
        goto loadAnimationR1;
    }
    tempR1 = (u32)sWarioWaterAnimationTable;
    tempR0 = directionValue << 2;
    tempR0 += 16;
    goto loadAnimationR0;

poseTen:
    if (wario->yVelocity >= 0)
        goto defaultAnimation;
    tempR1 = (u32)sWarioWaterVerticalAnimationTable;
    tempR0 = directionValue << 2;

loadAnimationR0:
    tempR0 += tempR1;
    animation = GP(*(GBA_PTR(const struct WarioAnimationFrame *) *)tempR0);
    goto animationSelected;

defaultAnimation:
    tempR2 = (u32)sWarioWaterAnimationTable;
    tempR1 = directionValue << 2;
    tempR0 = wario->pose;
    tempR0 <<= 3;
    tempR1 += tempR0;
    tempR1 += tempR2;

loadAnimationR1:
    animation = GP(*(GBA_PTR(const struct WarioAnimationFrame *) *)tempR1);

animationSelected:
    animation += wario->unk_1F;
    graphics = (const u8 *)GP(animation->objData);
    wario->objData1Size = *graphics << 5;
    graphics++;
    wario->objData2Size = *graphics << 5;
    graphics++;
    GP_SET(wario->pObjData1, (u8 *)graphics);
    GP_SET(wario->pObjData2, (u8 *)graphics + wario->objData1Size);
    GP_SET(wario->pOamData, GP(animation->oamData));

    palettePose = *(volatile u8 *)&wario->pose;
    if (palettePose == 6 && (gMainTimer & 7) <= 3) {
        gWarioPaletteSize = 0x20;
        palette = sWarioDamageFlashPalette;
        CopyWarioPalette(palette, 0, 0x10);
    } else {
        switch (wario->pose) {
            case 7:
            case 8:
                gWarioPaletteSize = 0x60;
                palette = GP(sWarioGroundPoundPaletteFrames[wario->unk_1F]);
                CopyWarioPalette(palette, 0, 0x10);
                palette = sWarioGroundPoundSecondaryPalette;
                CopyWarioPalette(palette, 0x10, 0x10);
                palette = sWarioGroundPoundPalette;
                CopyWarioPalette(palette, 0x20, 0x10);
                break;
            case 15:
                gWarioPaletteSize = 0x40;
                palette = ((gMainTimer & 7) <= 3) ? sWarioDefaultObjPalette : sBubbleWarioPalette;
                CopyWarioPalette(palette, 0, 0x10);
                palette = sWarioSecondaryPalette;
                CopyWarioPalette(palette, 0x10, 0x10);
                break;
            default:
                gWarioPaletteSize = 0x40;
                palette = sWarioDefaultObjPalette;
                CopyWarioPalette(palette, 0, 0x10);
                palette = sWarioSecondaryPalette;
                CopyWarioPalette(palette, 0x10, 0x10);
                break;
        }
    }
}

void UpdateBubbleWarioMusic(void)
{
    register u32 tableOrMask PIN("r4");
    struct MusicPlayerInfo *player0;
    struct MusicPlayerInfo *player1;
    register struct MusicPlayerInfo *normalPlayer0 PIN("r8");
    struct MusicPlayerInfo *callPlayer;
    u32 callMask;
    s8 *state;
    u16 tempo;

    if (gWarioData.pose == 14) {
        state = &gWarioMusicState;
        if (*state == 5)
            return;

        tableOrMask = (u32)gMPlayTable;
        player0 = GP(((const struct MusicPlayer *)tableOrMask)[0].info);
        m4aMPlayTempoControl(player0, 200);
        player1 = GP(((const struct MusicPlayer *)tableOrMask)[1].info);
        m4aMPlayTempoControl(player1, 200);
        tableOrMask = 0xFFFF;
        callPlayer = player0;
        callMask = tableOrMask;
        m4aMPlayPitchControl(callPlayer, callMask, 150);
        callPlayer = player1;
        callMask = tableOrMask;
        m4aMPlayPitchControl(callPlayer, callMask, 150);
        callPlayer = player0;
        callMask = tableOrMask;
        m4aMPlayModDepthSet(callPlayer, callMask, 30);
        callPlayer = player1;
        callMask = tableOrMask;
        m4aMPlayModDepthSet(callPlayer, callMask, 30);
        callPlayer = player0;
        callMask = tableOrMask;
        m4aMPlayLFOSpeedSet(callPlayer, callMask, 20);
        callPlayer = player1;
        callMask = tableOrMask;
        m4aMPlayLFOSpeedSet(callPlayer, callMask, 20);
        *state = 5;
    } else {
        state = &gWarioMusicState;
        if (*state == 4)
            return;

        tableOrMask = (u32)gMPlayTable;
        normalPlayer0 = GP(((const struct MusicPlayer *)tableOrMask)[0].info);
        tempo = 0x100;
        m4aMPlayTempoControl(normalPlayer0, tempo);
        player1 = GP(((const struct MusicPlayer *)tableOrMask)[1].info);
        m4aMPlayTempoControl(player1, tempo);
        tableOrMask = 0xFFFF;
        callPlayer = normalPlayer0;
        callMask = tableOrMask;
        m4aMPlayPitchControl(callPlayer, callMask, 0);
        callPlayer = player1;
        callMask = tableOrMask;
        m4aMPlayPitchControl(callPlayer, callMask, 0);
        callPlayer = normalPlayer0;
        callMask = tableOrMask;
        m4aMPlayModDepthSet(callPlayer, callMask, 0);
        callPlayer = player1;
        callMask = tableOrMask;
        m4aMPlayModDepthSet(callPlayer, callMask, 0);
        callPlayer = normalPlayer0;
        callMask = tableOrMask;
        m4aMPlayLFOSpeedSet(callPlayer, callMask, 20);
        callPlayer = player1;
        callMask = tableOrMask;
        m4aMPlayLFOSpeedSet(callPlayer, callMask, 20);
        *state = 4;
    }
}

void UpdateWarioWaterHitbox(void)
{
    const u8 *poseData;
    struct WarioData *wario;
    u32 offset;
    const u8 *hitboxData;
    u32 address;

    poseData = sWarioWaterPoseProperties;
    wario = &gWarioData;
    offset = poseData[wario->pose * 8];
    hitboxData = (const u8 *)sWarioHitboxes;
    offset <<= 3;
    address = offset + (u32)hitboxData;
    wario->hitboxOffsetLeft = *(const u16 *)address;
    address = (u32)hitboxData + 2;
    address = offset + address;
    wario->hitboxOffsetTop = *(const u16 *)address;
    address = (u32)hitboxData + 4;
    address = offset + address;
    wario->hitboxOffsetRight = *(const u16 *)address;
    hitboxData += 6;
    offset += (u32)hitboxData;
    wario->hitboxOffsetBottom = *(const u16 *)offset;
    address = wario->pose;
    address <<= 3;
    poseData += 7;
    address += (u32)poseData;
    wario->unk_1A = *(const u8 *)address;
}


#include "oam.h"
#include "sound.h"
#include "sprite.h"
#include "sprite_util.h"

extern const struct AnimationFrame sGlassBirdOam[];
extern const struct AnimationFrame sGlassBirdFragmentOam[];
extern const struct AnimationFrame sGlassBirdFragmentBrokenOam[];
extern const struct AnimationFrame sGlassBirdFragmentDyingOam[];
extern const struct AnimationFrame sGlassBirdFragmentCarriedOam[];

void InitGlassBird(void)
{
    struct PrimarySpriteData* sprite;
    sprite = &gCurrentSprite;
    sprite->status |= 0x408;
    sprite->drawDistanceDown = 24;
    sprite->drawDistanceUp = 0;
    sprite->drawDistanceLeftRight = 24;
    sprite->hitboxExtentUp = 1;
    sprite->hitboxExtentDown = 1;
    sprite->hitboxExtentLeft = 1;
    sprite->hitboxExtentRight = 1;
    GP_SET(sprite->pOamData, sGlassBirdOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->warioCollision = 0;
    sprite->pose = 16;
}

void UpdateGlassBird(void)
{
    struct PrimarySpriteData* sprite;
    u32 animation;
    sprite = &gCurrentSprite;
    animation = *(u32*)&sprite->animationTimer & 0xFFFFFF;
    if (animation == 0x40020) {
        SpriteSpawnAsChild(0xEB, sprite->roomSlot, sprite->gfxSlot,
                           sprite->yPosition + 56, sprite->xPosition);
    } else if (animation == 0x40004 && (sprite->status & 2)) {
        m4aSongNumStart(SE_GLASS_BIRD_FLAP);
    }
}

void InitGlassBirdFragment(void)
{
    struct PrimarySpriteData* sprite;
    sprite = &gCurrentSprite;
    sprite->status &= 0xFFFB;
    sprite->drawDistanceDown = 40;
    sprite->drawDistanceUp = 40;
    sprite->drawDistanceLeftRight = 40;
    sprite->hitboxExtentUp = 52;
    sprite->hitboxExtentDown = 0;
    sprite->hitboxExtentLeft = 28;
    sprite->hitboxExtentRight = 24;
    GP_SET(sprite->pOamData, sGlassBirdFragmentOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->warioCollision = 5;
    sprite->pose = 29;
}

void GlassBirdFragmentBreakInit(void)
{
    struct PrimarySpriteData* sprite;
    u16 oldStatus;
    register u16 status PIN("r2");
    u32 one;
    u32 zero16;
    register u32 zero PIN("r3");

    sprite = &gCurrentSprite;
    zero16 = 0;
    one = 1;
    sprite->disableWarioCollisionTimer = one;
    oldStatus = sprite->status;
    status = 0x400;
    zero = 0;
    status |= oldStatus;
    status &= 0xFEF7;
    sprite->status = status;
    GP_SET(sprite->pOamData, sGlassBirdFragmentBrokenOam);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero16;
    sprite->work0 = 28;
    sprite->drawPriority = one;
    sprite->pose = 123;
    if (status & 2)
        m4aSongNumStart(SE_GLASS_BIRD_BREAK);
}

void GlassBirdFragmentBreak(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;
    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    timer = --sprite->work0;
    if (timer == 0)
        sprite->status = 0;
}

void GlassBirdFragmentSetFallingPose(void)
{
    gCurrentSprite.pose = 30;
    gCurrentSprite.work3 = 0;
}

void GlassBirdFragmentDyingInit(void)
{
    struct PrimarySpriteData* sprite;
    sprite = &gCurrentSprite;
    sprite->pose = 123;
    GP_SET(sprite->pOamData, sGlassBirdFragmentDyingOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->work0 = 28;
    sprite->drawPriority = 1;
    sprite->status = (sprite->status & 0xFEFF) | 0x400;
}

void GlassBirdFragmentDyingRightInit(void)
{
    gCurrentSprite.status &= 0xFFBF;
    GlassBirdFragmentDyingInit();
}

void GlassBirdFragmentDyingLeftInit(void)
{
    gCurrentSprite.status |= 0x40;
    GlassBirdFragmentDyingInit();
}

void GlassBirdFragmentLiftedInit(void)
{
    gCurrentSprite.work2 = 8;
    gCurrentSprite.work3 = 0;
}

void GlassBirdFragmentLiftedRightInit(void)
{
    gCurrentSprite.pose = 72;
    GlassBirdFragmentLiftedInit();
}

void GlassBirdFragmentLiftedLeftInit(void)
{
    gCurrentSprite.pose = 74;
    GlassBirdFragmentLiftedInit();
}

void GlassBirdFragmentCarriedInit(void)
{
    struct PrimarySpriteData* sprite;
    sprite = &gCurrentSprite;
    sprite->status &= 0xFFF7;
    GP_SET(sprite->pOamData, sGlassBirdFragmentCarriedOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
}

void GlassBirdFragmentCarriedRightInit(void)
{
    gCurrentSprite.pose = 82;
    GlassBirdFragmentCarriedInit();
}

void GlassBirdFragmentCarriedLeftInit(void)
{
    gCurrentSprite.pose = 84;
    GlassBirdFragmentCarriedInit();
}

void GlassBirdFragmentThrownRightInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.pose = 88;
    GlassBirdFragmentCarriedInit();
}

void GlassBirdFragmentThrownLeftInit(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    gCurrentSprite.pose = 86;
    GlassBirdFragmentCarriedInit();
}

void SpriteGlassBird(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    switch (gCurrentSprite.pose) {
    case 0: InitGlassBird(); break;
    case 16: UpdateGlassBird(); break;
    }
}

void SpriteGlassBirdFragment(void)
{
    struct PrimarySpriteData* sprite;
    struct PrimarySpriteData* current;
    u8 pose;

    current = &gCurrentSprite;
    pose = current->pose;
    sprite = current;
    if (pose != 123 && (sprite->status & 0x800))
        sprite->pose = 107;

    switch (sprite->pose) {
    case 0: goto pose0;
    case 29: goto pose29;
    case 30: goto pose30;
    case 31: case 35: case 51: case 79: case 106: goto clearFlipDying;
    case 33: case 37: case 53: case 80: case 105: goto setFlipDying;
    case 67: goto pose68;
    case 68: case 72: goto liftedRight;
    case 69: goto pose70;
    case 70: case 74: goto liftedLeft;
    case 71: goto pose72;
    case 73: goto pose74;
    case 81: goto pose84;
    case 82: goto pose85;
    case 83: goto pose86;
    case 84: goto pose87;
    case 85: goto pose88;
    case 86: goto pose89;
    case 87: goto pose90;
    case 88: goto pose91;
    case 89: goto pose92;
    case 90: goto pose93;
    case 91: goto pose94;
    case 92: goto pose95;
    case 93: goto pose96;
    case 94: goto pose97;
    case 95: goto pose98;
    case 96: goto pose99;
    case 97: goto pose100;
    case 98: goto pose101;
    case 99: goto pose102;
    case 100: goto pose103;
    case 101: goto pose104;
    case 102: goto pose105;
    case 103: goto pose106;
    case 104: goto pose107;
    case 123: goto dying;
    default: goto defaultPose;
    }

pose0:
    InitGlassBirdFragment();
    goto end;
clearFlipDying:
    GlassBirdFragmentDyingRightInit();
dying:
    GlassBirdFragmentBreak();
    goto end;
setFlipDying:
    GlassBirdFragmentDyingLeftInit();
    goto dying;
pose29:
    GlassBirdFragmentSetFallingPose();
pose30:
    func_8023FA8();
    goto end;
pose68:
    func_8024AC0();
    goto liftedRight;
pose70:
    func_8024BEC();
    goto liftedLeft;
pose72:
    GlassBirdFragmentLiftedRightInit();
liftedRight:
    func_8024AD4();
    goto end;
pose74:
    GlassBirdFragmentLiftedLeftInit();
liftedLeft:
    func_8024C00();
    goto end;
pose84:
    GlassBirdFragmentCarriedRightInit();
    goto end;
pose85:
    SpriteUtilLiftingSpriteRight();
    goto end;
pose86:
    GlassBirdFragmentCarriedLeftInit();
    goto end;
pose87:
    SpriteUtilLiftingSpriteLeft();
    goto end;
pose90:
    GlassBirdFragmentThrownRightInit();
    goto end;
pose91:
    SpriteUtilCarryingSpriteRight();
    goto end;
pose88:
    GlassBirdFragmentThrownLeftInit();
    goto end;
pose89:
    SpriteUtilCarryingSpriteLeft();
    goto end;
pose92:
    sprite->pose = 90;
pose93:
    SpriteUtilThrownLeftSoft();
    goto end;
pose96:
    sprite->pose = 94;
pose97:
    SpriteUtilThrownLeftHard();
    goto end;
pose100:
    sprite->pose = 98;
pose101:
    SpriteUtilThrownUpLeftSoft();
    goto end;
pose104:
    sprite->pose = 102;
pose105:
    SpriteUtilThrownUpLeftHard();
    goto end;
pose94:
    sprite->pose = 92;
pose95:
    SpriteUtilThrownRightSoft();
    goto end;
pose98:
    sprite->pose = 96;
pose99:
    SpriteUtilThrownRightHard();
    goto end;
pose102:
    sprite->pose = 100;
pose103:
    SpriteUtilThrownUpRightSoft();
    goto end;
pose106:
    sprite->pose = 104;
pose107:
    SpriteUtilThrownUpRightHard();
    goto end;
defaultPose:
    GlassBirdFragmentBreakInit();
end:
    return;
}

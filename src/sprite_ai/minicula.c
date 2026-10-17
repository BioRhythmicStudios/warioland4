
#include "sprite.h"
#include "oam.h"

extern const struct AnimationFrame sMiniculaOam_83C4CD8[];
void func_8026450(void);

void InitMinicula(void)
{
    register int zero PIN("r3");
    int zeroHalfword;
    u16 status;
    register int statusMask PIN("r2");
    register int newStatus PIN("r0");
    struct PrimarySpriteData *sprite;

    sprite = &gCurrentSprite;
    status = sprite->status;
    statusMask = 0x80;
    statusMask <<= 3;
    ASM_BARRIER(: : "r"(statusMask));
    newStatus = statusMask;
    ASM_BARRIER(: : "r"(newStatus));
    zero = 0;
    zeroHalfword = 0;
    newStatus |= status;
    sprite->status = newStatus;
    sprite->warioCollision = 19;
    sprite->drawDistanceDown = 16;
    sprite->drawDistanceUp = zero;
    sprite->drawDistanceLeftRight = 8;
    sprite->hitboxExtentUp = 44;
    sprite->hitboxExtentDown = 136;
    sprite->hitboxExtentLeft = 16;
    sprite->hitboxExtentRight = 12;
    GP_SET(sprite->pOamData, sMiniculaOam_83C4CD8);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zeroHalfword;
    sprite->pose = 16;
    sprite->work3 = zero;
    sprite->drawPriority = zero;
}

void SpriteMinicula(void)
{
    if (gCurrentSprite.pose == 0) {
        InitMinicula();
    }
    func_8026450();
}

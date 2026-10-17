
#include "sound.h"
#include "score.h"
#include "wario.h"
#include "sprite.h"
#include "sprite_util.h"

extern const struct AnimationFrame sRobobirdIdleOam[];
extern const struct AnimationFrame sRobobirdAttackStartupOam[];
extern const struct AnimationFrame sRobobirdAttackOam[];
extern const struct AnimationFrame sRobobirdFallingOam[];
extern const struct AnimationFrame sRobobirdFastAttackOam[];

void InitRobobird(void)
{
    register struct PrimarySpriteData* sprite PIN("r12");
    struct PrimarySpriteData* finalSprite;
    register u8* p PIN("r0");
    u8* q;
    u8 zero;
    u8 thirtyTwo;
    u8 sixteen;

    sprite = &gCurrentSprite;
    p = &sprite->drawDistanceDown;
    zero = 0;
    thirtyTwo = 32;
    *p = thirtyTwo;
    p = &sprite->drawDistanceUp;
    sixteen = 16;
    *p = sixteen;
    q = &sprite->drawDistanceLeftRight;
    *q = 24;
    p = &sprite->hitboxExtentUp;
    *p = thirtyTwo;
    q += 2;
    *q = 12;
    p = &sprite->hitboxExtentLeft;
    *p = thirtyTwo;
    q += 2;
    *(volatile u8 *)q = 28;
    {
        register const struct AnimationFrame* oam PIN("r0");
        oam = sRobobirdIdleOam;
        finalSprite = sprite;
        GP_SET(finalSprite->pOamData, oam);
    }
    finalSprite->currentAnimationFrame = zero;
    finalSprite->animationTimer = zero;
    finalSprite->pose = sixteen;
    finalSprite->warioCollision = 1;
    SpriteUtilTurnTowardWario();
}

void RobobirdIdle(void)
{
    struct PrimarySpriteData* sprite;
    struct WarioData* wario;
    u16 warioY;
    u32 nearby;

    wario = &gWarioData;
    sprite = &gCurrentSprite;
    warioY = wario->yPosition;
    if (warioY >= sprite->yPosition)
        SpriteUtilTurnTowardWario();

    nearby = SpriteUtilCheckWarioNearbyLeftRight(120, 320);
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        if (nearby == 8)
            sprite->pose = SPOSE_6E;
    } else if (nearby == 4) {
        sprite->pose = SPOSE_6E;
    }
}

void StartRobobirdAttack(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sRobobirdAttackStartupOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = SPOSE_6F;
    sprite->work0 = 18;
    sprite->warioCollision = 4;
}

void RobobirdAttackDelay(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPointer;
    register u8 oldTimer PIN("r0");
    register u8 nextTimer PIN("r2");
    register u32 shiftedTimer PIN("r0");
    s32 timer;

    sprite = &gCurrentSprite;
    timerPointer = &sprite->work0;
    oldTimer = *timerPointer;
    nextTimer = oldTimer - 1;
    *timerPointer = nextTimer;
    shiftedTimer = nextTimer << 24;
    timer = shiftedTimer >> 24;
    if (timer != 0) {
        u16 compareTimer;
        compareTimer = timer;
        ASM_BARRIER(: "+r"(compareTimer));
        if (compareTimer == 9) {
            u8* extent;
            if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
                extent = &sprite->hitboxExtentRight;
                *extent = 68;
            } else {
                extent = &sprite->hitboxExtentLeft;
                *extent = 72;
            }
        }
    } else {
        GP_SET(sprite->pOamData, sRobobirdAttackOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = SPOSE_71;
        m4aSongNumStart(SE_ROBOBIRD_ATTACK);
    }
}

void RobobirdMoveForward(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
        sprite->xPosition += 8;
    else
        sprite->xPosition -= 8;
}

void StartRobobirdStunned(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sRobobirdFastAttackOam);
    sprite->currentAnimationFrame = 0;
    sprite->animationTimer = 0;
    sprite->pose = SPOSE_28;
}

void RobobirdKnockback(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->disableWarioCollisionTimer = 1;
    if (sprite->status & SPRITE_STATUS_FACING_RIGHT)
        sprite->xPosition += 16;
    else
        sprite->xPosition -= 16;
}

void SetRobobirdDefeated(void)
{
    struct PrimarySpriteData* sprite;
    u8 zero1;
    u8 zero2;

    sprite = &gCurrentSprite;
    GP_SET(sprite->pOamData, sRobobirdFallingOam);
    zero1 = 0;
    sprite->currentAnimationFrame = zero1;
    zero2 = 0;
    sprite->animationTimer = zero1;
    {
        u8* work3;
        work3 = &sprite->work3;
        *work3 = zero2;
    }
    sprite->health = zero2;
    sprite->drawPriority = zero2;
    {
        register u16 status PIN("r1");
        u16 masked;
        status = sprite->status;
        masked = status & ~SPRITE_STATUS_CAN_HIT_OTHER_SPRITES;
        masked |= SPRITE_STATUS_MAYBE_DEAD;
        sprite->status = masked;
    }
}

void InitRobobirdTackledRight(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->pose = SPOSE_TACKLED_RIGHT;
    sprite->work2 = 8;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    SetRobobirdDefeated();
}

void InitRobobirdTackledLeft(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->pose = SPOSE_TACKLED_LEFT;
    sprite->work2 = 8;
    SpawnSecondarySprite(sprite->yPosition - 32, sprite->xPosition, 6);
    SetRobobirdDefeated();
}

void InitRobobirdPushedRight(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_RIGHT;
    gCurrentSprite.work2 = 7;
    SetRobobirdDefeated();
}

void InitRobobirdPushedLeft(void)
{
    gCurrentSprite.pose = SPOSE_TACKLED_LEFT;
    gCurrentSprite.work2 = 7;
    SetRobobirdDefeated();
}

void SpriteRobobird(void)
{
    switch (gCurrentSprite.pose) {
        case SPOSE_INIT:
            InitRobobird();
        case SPOSE_IDLE:
            RobobirdIdle();
            break;
        case SPOSE_6A:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
        case SPOSE_PUSHED_RIGHT_INIT:
        case SPOSE_33:
            InitRobobirdPushedRight();
        case SPOSE_TACKLED_RIGHT:
fall_right:
            SpriteUtilFallOffscreenRight();
            break;
        case SPOSE_TACKLED_RIGHT_INIT:
        case SPOSE_4F:
            InitRobobirdTackledRight();
            goto fall_right;
        case SPOSE_69:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 32, gCurrentSprite.xPosition, 6);
        case SPOSE_PUSHED_LEFT_INIT:
        case SPOSE_35:
            InitRobobirdPushedLeft();
        case SPOSE_TACKLED_LEFT:
fall_left:
            SpriteUtilFallOffscreenLeft();
            break;
        case SPOSE_TACKLED_LEFT_INIT:
        case SPOSE_50:
            InitRobobirdTackledLeft();
            goto fall_left;
        case SPOSE_6E:
            StartRobobirdAttack();
        case SPOSE_6F:
            RobobirdAttackDelay();
            break;
        case SPOSE_71:
            RobobirdMoveForward();
            break;
        case SPOSE_27:
        case SPOSE_29:
            StartRobobirdStunned();
        case SPOSE_28:
            RobobirdKnockback();
            break;
        default:
            SpriteUtilDie();
    }
}

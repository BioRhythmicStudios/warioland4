#include "sprite_ai/switch.h"

#include "background_registers.h"
#include "score.h"
#include "sound.h"
#include "sprite_util.h"
#include "voice_set.h"
#include "wario.h"
#ifdef PORTABLE
#include "port/rumble_snapshot.h"
#endif

void SwitchSetCommonProperties(void)
{
    struct PrimarySpriteData* sprite;
    u16 oldStatus;
    register u16 status PIN("r0");
    u32 statusValue;
    u8 zero;
    u16 zero16;
    u8* ptr;
    u8* temp;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    statusValue = 0x408;
    status = statusValue;
    zero = 0;
    zero16 = 0;
    ASM_BARRIER_VOLATILE(: "+r"(zero16));
    status |= oldStatus;
    ptr = (u8*)sprite;
    *(u16*)ptr = status;
    {
        u8 oldPriority;
        u32 priority;

        oldPriority = ptr[26];
        priority = 0x80;
        priority |= oldPriority;
        ptr = (u8*)sprite;
        ptr[26] = priority;
    }
    ptr += 32;
    *ptr = 48;
    temp = (u8*)sprite + 33;
    *temp = zero;
    ptr += 2;
    *ptr = 16;
    ptr += 2;
    *ptr = 208;
    ptr++;
    *ptr = 24;
    ptr += 1;
    *ptr = 20;
    temp = (u8*)sprite;
    temp[22] = zero;
    *(u16*)(temp + 20) = zero16;
    temp += 40;
    *temp = zero;
}

void SwitchInit(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->warioCollision = 48;
    sprite->hitboxExtentUp = 168;
    GP_SET(sprite->pOamData, sSwitchIdleOam);
    sprite->pose = 16;
    SwitchSetCommonProperties();
}

void SwitchInitPressed(void)
{
    struct PrimarySpriteData* sprite;

    sprite = &gCurrentSprite;
    sprite->warioCollision = 29;
    sprite->hitboxExtentUp = 176;
    GP_SET(sprite->pOamData, sSwitchPressedOam);
    sprite->pose = 48;
    SwitchSetCommonProperties();
    SpawnHighPriorityPrimarySprite(170, sprite->roomSlot, sprite->gfxSlot, sprite->yPosition, sprite->xPosition);
    sprite->work0 = 60;
}

void EscapeSwitchInit(void)
{
    u8* persistent;
    struct PrimarySpriteData* sprite;

    persistent = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    persistent[gCurrentRoom * MAX_SPRITE_SLOTS_PER_ROOM + sprite->roomSlot] = 17;
    sprite->warioCollision = 48;
    sprite->hitboxExtentUp = 168;
    GP_SET(sprite->pOamData, sEscapeSwitchIdleOam);
    sprite->pose = 111;
    sprite->work0 = 1;
    SwitchSetCommonProperties();
}

void EscapeSwitchWaitForWario(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    if (gDisableWario == 0) {
        sprite = &gCurrentSprite;
        timer = --sprite->work0;
        if (timer == 0) {
            GP_SET(sprite->pOamData, sEscapeSwitchWaitingOam);
            sprite->currentAnimationFrame = timer;
            sprite->animationTimer = timer;
            sprite->work0 = 130;
            sprite->pose = 113;
        }
    }
}

void EscapeSwitchReset(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0) {
        GP_SET(sprite->pOamData, sSwitchIdleOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 16;
    }
}

void EscapeSwitchActivate(void)
{
    u8* persistent;
    u8* persistentEntry;
    struct PrimarySpriteData* sprite;
    u32 zero;

    persistent = &gPersistentSpriteData[0][0];
    sprite = &gCurrentSprite;
    persistentEntry = &persistent[gCurrentRoom * MAX_SPRITE_SLOTS_PER_ROOM + sprite->roomSlot];
    zero = 0;
    *persistentEntry = 33;
    sprite->warioCollision = 29;
    GP_SET(sprite->pOamData, sEscapeSwitchPressedOam);
    sprite->currentAnimationFrame = zero;
    sprite->animationTimer = zero;
    sprite->pose = 18;
    sprite->work0 = 13;
#ifdef PORTABLE
    PortRumbleImpact(PORT_RUMBLE_FROG);
#endif
    gSwitchPressed = 1;
    gEscapeHBlankEffectState = 1;
    gSwitchStates[SWITCH_ESCAPE] = SWITCH_STATE_SWITCHING_ON;
    gTimerState = 1;
    gStageFrameCounter = zero;
    gStageTimerDigitsState = 3;
    gStageTimerColonState = 2;
    SpawnSecondarySprite(200, 384, 78);
    {
        struct WarioData* wario;
        u32 y;
        u32 x;

        wario = &gWarioData;
        y = wario->yPosition;
        y -= gBg1YPosition;
        x = wario->xPosition;
        {
            u32 bgX;

            bgX = gBg1XPosition;
            bgX += 64;
            x -= bgX;
            SpawnSecondarySprite(y, x, 79);
        }
    }
    m4aMPlayAllStop();
}

void EscapeSwitchCountdown(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    register u8 oldTimer PIN("r0");
    register u8 timer PIN("r2");
    register u32 shiftedTimer PIN("r0");
    u32 checkedTimer;

    sprite = &gCurrentSprite;
    timerPtr = (u8*)sprite + 39;
    oldTimer = *timerPtr;
    timer = oldTimer - 1;
    {
        u8* storePtr;

        storePtr = timerPtr;
        *storePtr = timer;
    }
    shiftedTimer = timer << 24;
    checkedTimer = shiftedTimer >> 24;
    if (checkedTimer != 0) {
        register u8 soundTimer PIN("r0");

        soundTimer = checkedTimer;
        if (soundTimer == 12) {
            m4aSongNumStart(SE_ESCAPE_SWITCH_COUNTDOWN);
            VoiceSetPlay(5);
        }
    } else {
        u8 resetTimer;
        u8* storePtr;

        GP_SET(sprite->pOamData, sEscapeSwitchActiveOam);
        sprite->currentAnimationFrame = checkedTimer;
        sprite->animationTimer = checkedTimer;
        sprite->pose = 24;
        resetTimer = 40;
        storePtr = timerPtr;
        *storePtr = resetTimer;
        gWarioPauseTimer = 200;
    }
}

void SwitchReleaseDelay(void)
{
    struct PrimarySpriteData* sprite;
    u8 timer;

    sprite = &gCurrentSprite;
    timer = --sprite->work0;
    if (timer == 0) {
        GP_SET(sprite->pOamData, sSwitchReleasedOam);
        sprite->currentAnimationFrame = timer;
        sprite->animationTimer = timer;
        sprite->pose = 20;
        sprite->work0 = 34;
    }
}

void SwitchRespawnDelay(void)
{
    struct PrimarySpriteData* sprite;
    u8* timerPtr;
    u32 rawTimer;
    u32 shiftedTimer;
    u8 timer;

    sprite = &gCurrentSprite;
    timerPtr = (u8*)sprite + 39;
    rawTimer = *timerPtr;
    rawTimer--;
    *timerPtr = rawTimer;
    shiftedTimer = rawTimer << 24;
    timer = shiftedTimer >> 24;
    if (timer == 0) {
        u8* hitboxPtr;
        const struct AnimationFrame* oam;
        struct PrimarySpriteData* work;
        struct PrimarySpriteData* callSprite;

        hitboxPtr = (u8*)sprite + 35;
        *hitboxPtr = 176;
        oam = sSwitchPressedOam;
        work = sprite;
        GP_SET(work->pOamData, oam);
        work->currentAnimationFrame = timer;
        work->animationTimer = timer;
        work->pose = 48;
        {
            u8 roomSlot;
            u8 gfxSlot;

            roomSlot = work->roomSlot;
            gfxSlot = work->gfxSlot;
            callSprite = sprite;
            SpawnHighPriorityPrimarySprite(170, roomSlot, gfxSlot, callSprite->yPosition, callSprite->xPosition);
        }
        *timerPtr = 33;
    }
}

void SwitchUpdateActivationEffects(void)
{
    if (gTimerState <= 4) {
        return;
    }

    switch (gCurrentSprite.work0) {
        case 33:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 30, gCurrentSprite.xPosition, SSPRITE_06);
            GP_SET(gCurrentSprite.pOamData, sSwitchEffectActiveOam);
            gCurrentSprite.currentAnimationFrame = 0;
            gCurrentSprite.animationTimer = 0;
            break;
        case 30:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition - 16, SSPRITE_06);
            break;
        case 26:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition + 40, SSPRITE_06);
            break;
        case 22:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 128, gCurrentSprite.xPosition, SSPRITE_06);
            break;
        case 18:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 64, gCurrentSprite.xPosition, SSPRITE_06);
            break;
        case 14:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 128, gCurrentSprite.xPosition - 24, SSPRITE_06);
            break;
        case 11:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 90, gCurrentSprite.xPosition + 36, SSPRITE_06);
            break;
        case 8:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 160, gCurrentSprite.xPosition - 16, SSPRITE_06);
            break;
        case 4:
            SpawnSecondarySprite(gCurrentSprite.yPosition - 192, gCurrentSprite.xPosition + 24, SSPRITE_06);
            break;
        case 0:
            gCurrentSprite.status = 0;
            break;
    }
    gCurrentSprite.work0--;
}

void SwitchEffectInit(void)
{
    register struct PrimarySpriteData* sprite PIN("ip");
    u16 oldStatus;
    register u16 status PIN("r0");
    register u32 statusBits PIN("r2");
    u8 zero;
    u16 zero16;
    u8 value16;
    u8 value4;
    u8* ptr;
    u8* drawPtr;
    const struct AnimationFrame* oam;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    statusBits = 0x8400;
    status = statusBits;
    zero = 0;
    zero16 = 0;
    ASM_BARRIER_VOLATILE(: "+r"(zero16));
    status |= oldStatus;
    status &= 0xFFFB;
    drawPtr = (u8*)sprite;
    *(u16*)drawPtr = status;
    drawPtr += 32;
    status = 24;
    *drawPtr = status;
    ptr = (u8*)sprite + 33;
    *ptr = zero;
    ptr++;
    value16 = 16;
    *ptr = value16;
    ptr++;
    value4 = 4;
    *ptr = value4;
    ptr++;
    *ptr = value4;
    ptr++;
    *ptr = value4;
    /* Volatile keeps the final byte store separate from the pointer increment. */
    ptr++;
    *(volatile u8 *)ptr = value4;
    oam = sSwitchEffectIdleOam;
    drawPtr = (u8*)sprite;
    GP_SET(*(GBA_PTR(const struct AnimationFrame*) *)(drawPtr + 4), oam);
    drawPtr[22] = zero;
    *(u16*)(drawPtr + 20) = zero16;
    drawPtr[30] = zero;
    drawPtr[28] = value16;
}

void SpriteSwitch(void)
{
    switch (gCurrentSprite.pose) {
        case 0:
            if (gCurrentStageID == 0) {
                EscapeSwitchInit();
            } else {
                SwitchInit();
            }
            break;
        case 1:
            SwitchInit();
            break;
        case 2:
            SwitchInitPressed();
            break;
        case 17:
            EscapeSwitchActivate();
            break;
        case 18:
            EscapeSwitchCountdown();
            break;
        case 24:
            SwitchReleaseDelay();
            break;
        case 20:
            SwitchRespawnDelay();
            break;
        case 111:
            EscapeSwitchWaitForWario();
            break;
        case 113:
            EscapeSwitchReset();
            break;
        case 48:
            SwitchUpdateActivationEffects();
            break;
    }
    func_8026838();
}

void SpriteSwitchEffect(void)
{
    u32 result;

    result = SpriteUtilDespawnChildWithParent(7);
    if (result == 0) {
        gCurrentSprite.disableWarioCollisionTimer = 1;
        if (gCurrentSprite.pose == 0) {
            SwitchEffectInit();
        } else if (gTimerState > 4) {
            gCurrentSprite.status = result;
        }
    }
}
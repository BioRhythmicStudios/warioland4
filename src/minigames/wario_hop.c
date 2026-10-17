/*
    THERE IS SOME WEIRDS ASM ISLANDS THAT CAN'T BE REPLACED BECAUSE OF AGBCC BOUNDARIES
*/

#include "gba.h"
#include "gba/m4a.h"
#include "fixed_point.h"
#include "global_data.h"
#include "input.h"
#include "minigame.h"
#include "minigames/wario_hop.h"
#include "oam.h"
#include "stage_select.h"

extern u8 gWarioHopSpeedLevel;
extern u16 gWarioHopBg0Scroll[2];
extern u16 gWarioHopBg1Scroll[2];
extern u16 gWarioHopBg2Scroll[2];
extern struct WarioHopObjState gWarioHopPlayer;
extern struct WarioHopObjState gWarioHopPlayerShadow;
extern u16 gWarioHopRollingObject[];
extern struct WarioHopSmallState gWarioHopObstacles[];
extern struct WarioHopSmallState gWarioHopMedalEffect;
extern struct WarioHopSmallState gWarioHopBonusEffect;
extern u8 gWarioHopObstacleSlot;
extern u8 gWarioHopObstaclePathIndex;
extern u8 gWarioHopPassedObstacleSlot;
extern u16 gWarioHopScrollStep;
extern u8 gWarioHopJumpState;
extern u8 gWarioHopJumpStartVelocity;
extern u8 gWarioHopJumpAcceleration;
extern u8 gWarioHopJumpDuration;
extern u32 gWarioHopBeatCounter;
extern u16 gWarioHopJumpTimer;
extern u16 gWarioHopLandingOffset;
extern u16 gWarioHopGameOverTimer;
extern u8 gWarioHopMusicActive;
extern u8 gWarioHopMedalRewardTimer;
extern u8 gWarioHopHitLock;
extern u8 gMinigameState;
extern u8 gMinigameNewHighScore;
extern u16 gMinigameScore;
extern u16 gMinigameUiAnimation[2];
extern u8 gMinigameMedalHighlight;
extern struct WarioHopMidState gMinigameCamera;
extern u16 gMedalCount;
extern u16 gMinigameHighScores[];

extern const u8 sUnk_87122F8[];
extern const u8 sUnk_870DAF8[];
extern const u8 sUnk_87124F8[];
extern const u8 sUnk_871A4F8[];
extern const u8 sUnk_871ACF8[];
extern const u8 sUnk_871BCF8[];
extern const u8 sUnk_8711D98[];
extern const u8 sUnk_8711AF8[];
extern const u8 sUnk_8711438[];

extern const u32 sUnk_870CDC8[];
extern const u32 sUnk_870CDB8[];
extern const s32 sUnk_870CDE4[];
extern const u32 sUnk_870CDF4[];
extern const u32 sUnk_870CE04[];
extern const u32 sUnk_870CDA8[];
extern GBA_PTR(const struct AnimationFrame *) const sUnk_870CE14[];
extern GBA_PTR(const struct AnimationFrame *) const sUnk_870CE24[];
extern const s32 sUnk_870CE40[];
extern const s32 sUnk_870CE5C[];
extern const u32 sUnk_870CE78[];
extern const struct AnimationFrame sWarioHopRollingObjectOam[];
extern const struct AnimationFrame sWarioHopMedalHighlightOam[];
extern const struct AnimationFrame sWarioHopPlayerOam[];
extern const struct AnimationFrame sWarioHopBonusEffectOam[];
extern const struct AnimationFrame sWarioHopMedalEffectOam[];
extern GBA_PTR(const u16 *) sUnk_870D880;
extern GBA_PTR(const u16 *) sUnk_870D890;
extern const u8 sUnk_870CC18[];
extern const u8 sUnk_870CCE0[];
extern const u16 sUnk_870CA28[][62];

u32 WarioHopUpdateStateMachine(void)
{
    switch (gMinigameState) {
        case 0:
            WarioHopScrollBackgrounds();
            WarioHopAdvancePlayerAnimation();
            if (MinigameWaitForFrames(40) != 0) {
                gMinigameState++;
            }
            break;
        case 1:
            WarioHopScrollBackgrounds();
            WarioHopAdvancePlayerAnimation();
            gMinigameCamera.angle -= 20;
            if (gMinigameCamera.angle <= 31) {
                gMinigameCamera.angle = 32;
                gMinigameCamera.active = 0;
                gMinigameState++;
            }
            break;
        case 2:
            WarioHopScrollBackgrounds();
            WarioHopAdvancePlayerAnimation();
            if (WarioHopUpdateGameplay() != 0) {
                gMinigameState++;
            }
            break;
        case 3:
            WarioHopUpdateGameOverMusic();
            WarioHopUpdateMedalReward();
            if (MinigameWaitForFrames(120) != 0) {
                gMinigameState++;
                gMinigameCamera.active = 2;
            }
            break;
        case 4:
            WarioHopUpdateGameOverMusic();
            gMinigameCamera.angle += 20;
            if (gMinigameCamera.angle > 512) {
                m4aSongNumStart(0x236);
                gMinigameCamera.angle = 512;
                gMinigameState++;
            }
            break;
        case 5:
            WarioHopUpdateGameOverMusic();
            if (MinigameWaitForFrames(60) != 0) {
                gMinigameState = 0;
                return 1;
            }
            break;
    }

    return 0;
}

void WarioHopApplyBgScroll(void)
{
    *(vu16 *)0x04000010 = (s16)gWarioHopBg0Scroll[0] >> 4;
    *(vu16 *)0x04000014 = (s16)gWarioHopBg1Scroll[0] >> 4;
    *(vu16 *)0x04000018 = (s16)gWarioHopBg2Scroll[0] >> 4;
}

void WarioHopInit(void)
{
    const void *dmaSrc1;
    const void *dmaSrc2;
    register const void *dmaSrc4 PIN("r12");
    register const void *dmaSrc5 PIN("r9");

    register u16 *scrollA PIN("r10");
    register u8 *speedBase PIN("r8");
    register struct WarioHopObjState *playerBase PIN("r6");
    u16 *scrollPtr;
    u32 randomBit;
    register s32 zero PIN("r4");
    register u8 persistentByteZero PIN("r5");

    randomBit = MinigameRandom() & 1;
    if (randomBit == 0) {
        gWarioHopSpeedLevel = randomBit;
    } else {
        gWarioHopSpeedLevel = 1;
    }

    DmaSet(3, (const void *)0x0870D8F8, (void *)0x05000000, 0x80000100);
    {
        vu32 *dmaRegs;
        u32 dmaStatus;
        u32 dmaMask;
        const void *tmpSrc4;
        u8 *tmpSpeedBase;

        dmaRegs = (vu32 *)REG_ADDR_DMA3;
        dmaStatus = dmaRegs[2];
        dmaMask = DMA_ENABLE << 16;

        dmaSrc1 = sUnk_87122F8;
        dmaSrc2 = sUnk_870DAF8;
        tmpSrc4 = sUnk_871A4F8;
        dmaSrc4 = tmpSrc4;
        playerBase = (struct WarioHopObjState *)sUnk_871ACF8;

        dmaSrc5 = playerBase;
        {
            u16 *tmpScrollA = gWarioHopBg0Scroll;
            scrollA = tmpScrollA;
        }
        tmpSpeedBase = &gWarioHopJumpDuration;
        speedBase = tmpSpeedBase;
        playerBase = &gWarioHopPlayer;
        if ((s32)dmaStatus < 0) {
            while (dmaRegs[2] & dmaMask) {
            }
        }
    }

    DmaSet(3, dmaSrc1, (void *)0x05000200, 0x80000100);
    DmaWait(3);
    DmaSet(3, dmaSrc2, (void *)0x06000000, 0x80004000);
    DmaWait(3);
    {
        vu32 *dmaRegs;
        register const void *tmpDmaSrc3 PIN("r3");

        dmaRegs = (vu32 *)REG_ADDR_DMA3;
        tmpDmaSrc3 = sUnk_87124F8;
        dmaRegs[0] = (vu32)tmpDmaSrc3;
        dmaRegs[1] = (vu32)(void *)0x06010000;
        dmaRegs[2] = 0x80004000;
        dmaRegs[2];
    }
    DmaWait(3);
    {
        vu32 *dmaRegs;
        const void *src;

        dmaRegs = (vu32 *)REG_ADDR_DMA3;
        src = dmaSrc4;
        dmaRegs[0] = (vu32)src;
        dmaRegs[1] = (vu32)(void *)0x0600C000;
        dmaRegs[2] = 0x80000400;
        dmaRegs[2];
    }
    DmaWait(3);
    {
        vu32 *dmaRegs;
        register const void *src PIN("r2");

        dmaRegs = (vu32 *)REG_ADDR_DMA3;
        src = dmaSrc5;
        dmaRegs[0] = (vu32)src;
        dmaRegs[1] = (vu32)(void *)0x0600D000;
        dmaRegs[2] = 0x80000800;
        dmaRegs[2];
    }
    DmaWait(3);
    {
        vu32 *dmaRegs;
        register const void *tmpDmaSrc PIN("r3");

        dmaRegs = (vu32 *)REG_ADDR_DMA3;
        tmpDmaSrc = sUnk_871BCF8;
        dmaRegs[0] = (vu32)tmpDmaSrc;
        dmaRegs[1] = (vu32)(void *)0x0600E000;
        dmaRegs[2] = 0x80000800;
        dmaRegs[2];
    }
    DmaWait(3);
    {
        register vu32 *dmaRegs;
        register const void *tmpDmaSrc PIN("r4");
        register s32 bg0Control PIN("r5");
        s32 bg1Control;
        register s32 bg2Control PIN("r2");
        register s32 bg3Control PIN("r3");
        register s32 storeValue PIN("r0");

        dmaRegs = (vu32 *)REG_ADDR_DMA3;
        tmpDmaSrc = (const void *)0x0871CCF8;
        dmaRegs[0] = (vu32)tmpDmaSrc;
        dmaRegs[1] = (vu32)(void *)0x0600F000;
        dmaRegs[2] = 0x80000400;
        dmaRegs[2];

        dmaRegs = (vu32 *)((u8 *)dmaRegs - 0xCC);
        bg0Control = 0x1801;

        storeValue = bg0Control;
        *(vu16 *)dmaRegs = storeValue;
        dmaRegs = (vu32 *)((u8 *)dmaRegs + 2);
        bg1Control = 0x5A02;
        storeValue = bg1Control;
        *(vu16 *)dmaRegs = storeValue;
        dmaRegs = (vu32 *)((u8 *)dmaRegs + 2);
        bg2Control = 0x5C03;

        storeValue = bg2Control;
        *(vu16 *)dmaRegs = storeValue;
        dmaRegs = (vu32 *)((u8 *)dmaRegs + 2);
        bg3Control = 0x1E00;

        storeValue = bg3Control;
        *(vu16 *)dmaRegs = storeValue;
    }

    persistentByteZero = 0;
    zero = 0;

    scrollPtr = scrollA;

    scrollPtr[0] = zero;

    scrollPtr[1] = zero;
    {
        register vu16 *bg1Scroll PIN("r0") = (vu16 *)gWarioHopBg1Scroll;
        register vu16 *bg2Scroll PIN("r1");

        bg1Scroll[0] = zero;
        bg1Scroll[1] = zero;
        bg2Scroll = (vu16 *)gWarioHopBg2Scroll;
        bg2Scroll[0] = zero;
        bg2Scroll[1] = zero;
    }

    *(vu16 *)0x04000012 = zero;
    *(vu16 *)0x04000010 = zero;
    *(vu16 *)0x04000016 = zero;
    *(vu16 *)0x04000014 = zero;
    *(vu16 *)0x0400001A = zero;
    *(vu16 *)0x04000018 = zero;
    *(vu16 *)0x0400001E = zero;
    *(vu16 *)0x0400001C = zero;

    {
        s32 speed;
        s32 selectedSpeed;
        u8 *speedPtr;

        {
            register const u32 *speedTable PIN("r3") = sUnk_870CE04;
            selectedSpeed = speedTable[gWarioHopSpeedLevel];
        }
        speedPtr = speedBase;
        *speedPtr = selectedSpeed;
        speed = *speedPtr;

        {
            s32 accelValue;
            register u8 *accelOut PIN("r1");

            accelValue = 0x2800 / (speed * speed);
            accelOut = &gWarioHopJumpAcceleration;
            *accelOut = accelValue;
        }
        {
            s32 speed2;
            s32 accel;
            register u8 *accelIn PIN("r2");

            speed2 = *speedPtr;
            accelIn = &gWarioHopJumpAcceleration;
            accel = *accelIn;
            {
                register u8 *velocityOut PIN("r3") = &gWarioHopJumpStartVelocity;
                *velocityOut = (accel * speed2) >> 1;
            }
        }
    }

    playerBase->x = 0xA00;
    playerBase->y = 0x8C0;

    *(volatile u16 *)&playerBase->unkC = zero;
    *(volatile u16 *)&playerBase->yOffset = zero;
    playerBase->scale = 0x100;
    *(volatile u16 *)&playerBase->angle = zero;
    *(volatile u16 *)&playerBase->animTimer = zero;
    *(volatile u16 *)&playerBase->animFrame = zero;

    {
        register struct WarioHopObjState *shadow PIN("r1") = &gWarioHopPlayerShadow;
        *shadow = *playerBase;
    }

    {
        register u16 *rolling PIN("r0") = (u16 *)gWarioHopRollingObject;
        rolling[0] = 0xA00;
        rolling[1] = 0x8C0;
        rolling[3] = zero;
        rolling[4] = zero;
    }

    {
        u8 *subStatePtr;
        u16 *scorePtr;
        register u8 *highScorePtr PIN("r6");

        subStatePtr = &gMinigameState;

        *subStatePtr = persistentByteZero;
        scorePtr = &gMinigameScore;
        *scorePtr = zero;
        highScorePtr = &gMinigameNewHighScore;
        *highScorePtr = persistentByteZero;
    }
    WarioHopDrawScoreDigits();
    {
        u8 *medalTimerPtr;
        medalTimerPtr = &gWarioHopMedalRewardTimer;

        *medalTimerPtr = persistentByteZero;
    }
    InitMinigameScoreDisplay();
    *(vu16 *)0x04000054 = zero;
    m4aSongNumStart(0x2BE);

    {
        u32 loopZero;
        register s32 j PIN("r1");
        u32 one;
        register struct WarioHopSmallState *item;
        loopZero = 0;
        one = 1;
        item = gWarioHopObstacles;
        j = 6;
        do {

            item->x = loopZero;
            item->type = zero;
            item->animTimer = loopZero;
            item->animFrame = loopZero;
            item->active = one;
            item++;
            j--;
        } while (j >= 0);
    }

    {
        u32 tailZero;
        s32 tailZero2;
        tailZero = 0;
        gWarioHopObstacleSlot = tailZero;
        gWarioHopObstaclePathIndex = tailZero;
        gWarioHopJumpState = tailZero;
        gWarioHopJumpTimer = tailZero;
        tailZero2 = 0;
        gWarioHopLandingOffset = tailZero2 - 2;
        gWarioHopBeatCounter = tailZero;
        gWarioHopGameOverTimer = tailZero;
        gWarioHopMusicActive = tailZero2;
        gWarioHopPassedObstacleSlot = tailZero2;
        gMinigameUiAnimation[0] = tailZero;
        gMinigameUiAnimation[1] = tailZero;
        gMinigameMedalHighlight = tailZero2;
    }

    gMinigameCamera.xOffset = 0x80;
    gMinigameCamera.yOffset = 0x40;
    gMinigameCamera.angle = 0x200;
    gMinigameCamera.active = 1;

    {
        struct WarioHopSmallState *bonusEffect;
        register struct WarioHopSmallState *medalEffect PIN("r1");

        bonusEffect = &gWarioHopBonusEffect;
        bonusEffect->animTimer = 0;
        bonusEffect->animFrame = 0;
        bonusEffect->type = 0;
        medalEffect = &gWarioHopMedalEffect;
        *medalEffect = *bonusEffect;

    }

    gWarioHopHitLock = 0;
    *(u16 *)0x4000000 = 0x1F00;
}

void WarioHopRestartMusicForNextSpeed(void)
{
    gWarioHopMusicActive = 0;
    gWarioHopBeatCounter = 0;
    m4aSongNumStart(0x2BE);
    if (gWarioHopSpeedLevel != 3) {
        gWarioHopSpeedLevel++;
    }
    if (gWarioHopSpeedLevel == 1) {
        gWarioHopSpeedLevel++;
    }
}

s32 WarioHopUpdateGameplay(void)
{
    switch (gWarioHopJumpState) {
        case 0:
            if (WarioHopIsJumpPressed() != 0) {
                gWarioHopJumpState = 1;
                gWarioHopJumpTimer = 0;
                gWarioHopPlayer.scale = 0x150;
                if (gMPlayMemAccArea[0] == 1 && gWarioHopMusicActive != 0) {
                    m4aSongNumStart(sUnk_870CDC8[gWarioHopObstacles[gWarioHopPassedObstacleSlot].type - 1]);
                    gWarioHopHitLock = 1;
                } else {
                    m4aSongNumStart(0x23D);
                }
            }
            break;
        case 1:
            if (WarioHopUpdateJumpArc() != 0) {
                {
                    s32 resetValue;
                    volatile s16 *resetPtr;

                    gWarioHopHitLock = 0;
                    gWarioHopJumpState = 0;
                    resetPtr = (s16 *)&gWarioHopJumpTimer;
                    resetValue = 0;
                    *resetPtr = resetValue;
                    resetPtr = (s16 *)&gWarioHopLandingOffset;
                    resetValue -= 2;
                    *(u16 *)resetPtr = resetValue;
                }
                gWarioHopPlayer.yOffset = 0;
                gWarioHopPlayerShadow.yOffset = 0;
            }
            break;
    }

    {
        u8 *activePtr;
        u32 active;

        activePtr = &gWarioHopMusicActive;
        active = *activePtr;
        if (active == 1) {
            if (MinigameRandom() % 3000 == 0 && gWarioHopBonusEffect.type == 0) {
                gWarioHopBonusEffect.type++;
            }
            WarioHopSpawnObstacleIfDue();
            gWarioHopBeatCounter++;
            if (gMPlayMemAccArea[0] == 0x7F) {
                gMPlayMemAccArea[0] = 0;
                gWarioHopMedalEffect.type = 1;
                m4aSongNumStart(0x23B);
                WarioHopRestartMusicForNextSpeed();
            }
        } else {
            if (gWarioHopJumpState == 0) {
                u8 *speedPtr;
                u8 *accelPtr;
                s32 speed;

                speedPtr = &gWarioHopJumpDuration;
                *speedPtr = sUnk_870CE04[gWarioHopSpeedLevel];
                accelPtr = &gWarioHopJumpAcceleration;
                speed = *speedPtr;
                *accelPtr = 0x2800 / (speed * speed);
                gWarioHopJumpStartVelocity = ((*speedPtr) * (*accelPtr)) >> 1;
            }
            if (gMPlayMemAccArea[0] == 0x7E) {
                *activePtr = 1;
                gWarioHopObstaclePathIndex = 0;
                gMPlayMemAccArea[0] = 0;
                m4aSongNumStart(sUnk_870CDB8[gWarioHopSpeedLevel]);
            }
        }
    }

    WarioHopUpdateObstacles();
    {
        s32 result;
        result = WarioHopCheckPlayerCollision();
        if (result != 0) {
            return 1;
        }
    }
    WarioHopUpdateMedalReward();
    return 0;
}

s32 WarioHopIsJumpPressed(void)
{
    if (gButtonsPressed & A_BUTTON) {
        return 1;
    }
    return 0;
}

s32 WarioHopUpdateJumpArc(void)
{
    u16 *timer;
    struct WarioHopObjState *first;
    u8 *baseSpeed;
    u8 *accel;
    s32 baseValue;
    register s32 accelValue PIN("r2");
    s32 frame;
    struct WarioHopObjState *second;
    s32 movement;

    timer = &gWarioHopJumpTimer;
    (*timer)++;
    first = &gWarioHopPlayer;
    baseSpeed = &gWarioHopJumpStartVelocity;
    baseValue = *baseSpeed;
    accel = &gWarioHopJumpAcceleration;
    accelValue = *accel;
    frame = *(s16 *)timer;

    movement = baseValue - frame * accelValue;
    first->yOffset = movement;
    first->y -= movement;
    {
        u32 nextScale;

        nextScale = first->scale - 10;
        first->scale = nextScale;
        if ((u16)nextScale <= 0xFF) {
            first->scale = 0x100;
        }
    }

    frame = (s16)*timer;
    second = &gWarioHopPlayerShadow;
    if (frame > 2) {
        baseValue = *baseSpeed;
        accelValue = *accel;
        frame -= 2;
        movement = baseValue - frame * accelValue;
        second->yOffset = movement;
        second->y -= movement;
    }
    frame = *(s16 *)((u8 *)first + 2);
    ASM_BARRIER(: : "r"(accelValue));
    if (frame > 0x8C0) {
        first->y = 0x8C0;
    }
    if ((s16)second->y > 0x8C0) {
        second->y = 0x8C0;
    }
    baseValue = *(s16 *)timer;
    if (baseValue == gWarioHopJumpDuration + 2) {
        return 1;
    }
    return 0;
}

void WarioHopUpdateWaitingForStart(void)
{
    WarioHopScrollBackgrounds();
    WarioHopAdvancePlayerAnimation();
}

void WarioHopSpawnObstacleIfDue(void)
{
    const s32 *speedTable;
    u8 *modePtr;
    s32 mode;
    u32 modeOffset;
    s32 pathIndex;
    const u8 *pathBase;
    u32 pathOffset;
    u32 modePathOffset;
    s32 step;
    s32 kind;

    speedTable = sUnk_870CDE4;
    modePtr = &gWarioHopSpeedLevel;
    mode = *modePtr;
    modeOffset = mode << 2;
    speedTable = (const s32 *)(modeOffset + (u32)speedTable);
    step = 0xA00 / *speedTable;
    pathBase = (const u8 *)sUnk_870CA28;
    pathIndex = gWarioHopObstaclePathIndex;
    pathOffset = pathIndex << 1;
    modePathOffset = (mode << 5) - mode;
    modePathOffset <<= 2;

    if (*(const u16 *)(pathBase + (pathOffset + modePathOffset)) - step + *(const s32 *)((const u8 *)sUnk_870CE78 + modeOffset) == (s32)gWarioHopBeatCounter) {
        s32 pathCount;

        pathCount = *(const s32 *)((const u8 *)sUnk_870CDA8 + modeOffset);
        if (pathIndex == pathCount - 1) {
            kind = 5;
        } else if (pathIndex == 0) {
            kind = 6;
        } else {
            kind = MinigameRandom() % 5;
        }

        gWarioHopObstacles[gWarioHopObstacleSlot].type = kind + 1;
        gWarioHopObstacles[gWarioHopObstacleSlot].y = 0x8C0;

        gWarioHopObstaclePathIndex++;
        if (gWarioHopObstaclePathIndex > 61) {
            gWarioHopObstaclePathIndex = 61;
        }

        gWarioHopObstacleSlot++;
        if (gWarioHopObstacleSlot > 6) {
            gWarioHopObstacleSlot = 0;
        }
    }
}

void WarioHopAdvancePlayerAnimation(void)
{
    struct WarioHopObjState *player;
    register s32 temp PIN("r0");
    register u16 value PIN("r1");

    player = &gWarioHopPlayer;
    temp = 0x100;
    value = temp;
    temp = player->angle;
    value += temp;
    value -= sUnk_870CDF4[gWarioHopSpeedLevel];
    temp = 0xFF;
    value &= temp;
    player->angle = value;
}

void WarioHopUpdateObstacles(void)
{
    s32 offset;
    register struct WarioHopSmallState *base PIN("r3");
    register s32 passLimit PIN("r7");
    register s32 farLimit PIN("r6");
    register s32 i PIN("r5");

    base = gWarioHopObstacles;
    passLimit = 0xA80;
    ASM_BARRIER(: : "r"(base), "r"(passLimit));
    offset = 0;
    farLimit = 0x1180;
    ASM_BARRIER(: : "r"(farLimit));
    i = 6;
    do {
        struct WarioHopSmallState *item;

        item = (struct WarioHopSmallState *)((u8 *)gWarioHopObstacles + offset);
        if (item->type != 0) {
            item->x += gWarioHopScrollStep;
            if ((s16)item->x > 0xA80 && item->active != 0 && gWarioHopJumpState == 0) {
                item->active = 0;
                if (WarioHopAddScoreAndCheckMedal() != 0) {
                    gWarioHopMedalRewardTimer++;
                }
            }
            {
                struct WarioHopSmallState *item2;

                item2 = (struct WarioHopSmallState *)((u8 *)gWarioHopObstacles + offset);
                if ((s16)item2->x > 0x1180) {
                    item2->active = 1;
                    item2->type = 0;
                    item2->x = 0;
                    item2->animTimer = 0;
                    item2->animFrame = 0;
                }
            }
        }
        offset += 12;
        i--;
    } while (i >= 0);
}

void WarioHopUpdateGameOverMusic(void)
{
    register struct MusicPlayerInfo *info PIN("r4");
    u16 song;

    song = gSongTable[sUnk_870CDB8[gWarioHopSpeedLevel]].ms;
    switch ((s16)gWarioHopGameOverTimer) {
        case 0:
            info = GP(gMPlayTable[song].info);
            m4aMPlayTempoControl(info, 280);
            m4aMPlayPitchControl(info, 0xFFFF, -100);
            m4aSongNumStart(0x23C);
            break;
        case 30:
            info = GP(gMPlayTable[song].info);
            m4aMPlayTempoControl(info, 220);
            m4aMPlayPitchControl(info, 0xFFFF, -200);
            break;
        case 60:
            info = GP(gMPlayTable[song].info);
            m4aMPlayTempoControl(info, 200);
            m4aMPlayPitchControl(info, 0xFFFF, -300);
            break;
        case 90:
            info = GP(gMPlayTable[song].info);
            m4aMPlayTempoControl(info, 160);
            m4aMPlayPitchControl(info, 0xFFFF, -400);
            m4aMPlayModDepthSet(info, 0xFFFF, 15);
            break;
        case 120:
            info = GP(gMPlayTable[song].info);
            m4aMPlayTempoControl(info, 130);
            m4aMPlayPitchControl(info, 0xFFFF, -500);
            m4aMPlayFadeOut(info, 3);
            break;
    }
    if ((s16)gWarioHopGameOverTimer == 180) {
        m4aMPlayAllStop();
    }
    if ((s16)gWarioHopGameOverTimer <= 180) {
        gWarioHopGameOverTimer++;
    }
}

void WarioHopScrollBackgrounds(void)
{
    u16 *delta;
    const u32 *table;
    u32 value;

    delta = &gWarioHopScrollStep;
    table = (const u32 *)sUnk_870CDE4;
    value = table[gWarioHopSpeedLevel];
    *delta = value;
    gWarioHopBg0Scroll[0] -= value;
    gWarioHopBg1Scroll[0] -= (s16)*delta / 10;
    gWarioHopBg2Scroll[0] -= (s16)*delta / 18;
    if ((s16)gWarioHopBg0Scroll[0] < 0) {
        gWarioHopBg0Scroll[0] += 0x2000;
    }
    if ((s16)gWarioHopBg1Scroll[0] < 0) {
        gWarioHopBg1Scroll[0] += 0x2000;
    }
    if ((s16)gWarioHopBg2Scroll[0] < 0) {
        gWarioHopBg2Scroll[0] += 0x2000;
    }
}

s32 WarioHopCheckPlayerCollision(void)
{
    s32 leftMin;
    s32 leftMax;
    s32 playerY;
    s32 top;
    s32 bottom;
    s32 i;
    const s32 *widths;
    struct WarioHopSmallState *item;

    leftMin = 160;
    leftMax = 176;
    playerY = (s16)gWarioHopPlayer.y >> 4;
    top = playerY - 48;
    bottom = playerY;
    i = 0;
    widths = sUnk_870CE40;
    item = gWarioHopObstacles;
    do {
        if (item->type != 0) {
            s32 type;
            s32 width;
            s32 height;
            s32 x;

            type = item->type - 1;
            width = widths[type];
            height = sUnk_870CE5C[type];
            x = (s16)item->x >> 4;
            if (x > leftMin && x - width < leftMax) {
                s32 y;

                y = (s16)item->y >> 4;
                if (y > top && y - height < bottom && gWarioHopHitLock == 0) {
                    return 1;
                }
            }
        }
        item++;
        i++;
    } while (i <= 6);
    return 0;
}

void WarioHopDrawScoreDigits(void)
{
    if (gMinigameMedalHighlight != 0) {
        DrawMinigameNumber(gMedalCount, (const u16 *)sUnk_8711D98, 0x3840);
    } else {
        DrawMinigameNumber(gMedalCount, (const u16 *)sUnk_8711AF8, 0x3840);
    }
    DrawMinigameNumber(gMinigameScore, (const u16 *)sUnk_8711438, 0x38C0);
    DrawMinigameHighScore(gMinigameHighScores[1], sUnk_8711438 + 0xC20, 0x4160);
}

s32 WarioHopAddScoreAndCheckMedal(void)
{
    gMinigameScore++;
    gWarioHopPassedObstacleSlot++;
    if (gWarioHopPassedObstacleSlot > 6) {
        gWarioHopPassedObstacleSlot = 0;
    }
    if (gMinigameScore > 999) {
        gMinigameScore = 999;
    } else {
        m4aSongNumStart(0x224);
    }
    if (gMinigameScore > gMinigameHighScores[1]) {
        gMinigameNewHighScore = 1;
        gMinigameHighScores[1] = gMinigameScore;
    }
    WarioHopDrawScoreDigits();
    if (gMinigameScore % 15 == 0) {
        return 1;
    }
    return 0;
}

void WarioHopUpdateMedalReward(void)
{
    if (gWarioHopMedalRewardTimer != 0) {
        gWarioHopMedalRewardTimer++;
    }
    if (gWarioHopMedalRewardTimer > 20) {
        gWarioHopMedalRewardTimer = 0;
        gMedalCount++;
        if (gMedalCount > 999) {
            gMedalCount = 999;
        } else {
            m4aSongNumStart(0x223);
            gMinigameMedalHighlight = 1;
            WarioHopDrawScoreDigits();
        }
    }
}

void WarioHopDrawSprites(void)
{
    /* OamData merges attr1 bits 9..13 into one 5-bit matrixNum. The affine sprites
       here only write the 3-bit affine index (as in DrawSoundRoomSprites). */
    typedef union AGB_ALIGN {
        struct AGB_ALIGN {
            u32 y : 8;
            u32 affineMode : 2;
            u32 objMode : 2;
            u32 mosaic : 1;
            u32 bpp : 1;
            u32 shape : 2;

            u32 x : 9;
            u32 affineIndex : 3;
            u32 preserved : 2;
            u32 size : 2;

            u16 tileNum : 10;
            u16 priority : 2;
            u16 paletteNum : 4;

            u16 fractional : 8;
            u16 integer : 7;
            u16 sign : 1;
        } split;

        struct AGB_ALIGN {
            u16 attr0;
            u16 attr1;
            u16 attr2;
            u16 affineParam;
        } all;
    } WarioHopOamData;

    const struct AnimationFrame* animation;
    const u16* src;
    u16* raw;
    u16 attr;
    s32 i;
    s32 total;
    s32 j;
    u32 active;
    s16 matrix[4];
    s16* matrix1Ptr;
    s16* matrix2Ptr;
    s16* matrix3Ptr;
    u16 matrixD;
    s32 matrixD2;

    i = 0;
    total = gOamSlotsUsed;
    raw = (u16*)&gOamBuffer[total];
    active = gMinigameCamera.active;
    matrix1Ptr = &matrix[1];
    matrix2Ptr = matrix1Ptr + 1;
    matrix3Ptr = matrix2Ptr + 1;

    if (active != 0) {
        if (active == 1) {
            src = GP(sUnk_870D880);
        }
        if (active == 2) {
            src = GP(sUnk_870D890);
        }
        total += *(src++);
        for (; i < total; i++) {
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.y = gMinigameCamera.yOffset + attr - 16;
            gOamBuffer[i].split.affineMode = 3;
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.x = (attr + (s16)gMinigameCamera.xOffset - 32) & 0x1FF;
            ((WarioHopOamData*)gOamBuffer)[i].split.affineIndex = 1;
            *(raw++) = *(src++);
            gOamBuffer[i].split.priority = 0;
            raw++;
        }
    }

    if (gMinigameMedalHighlight != 0) {
        gMinigameUiAnimation[0]++;
        if (sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].time < gMinigameUiAnimation[0]) {
            gMinigameUiAnimation[0] = 1;
            gMinigameUiAnimation[1]++;
            if (sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].time == 0) {
                gMinigameUiAnimation[1] = 0;
                gMinigameUiAnimation[0] = 0;
                gMinigameMedalHighlight = 0;
                WarioHopDrawScoreDigits();
            }
        }
    }
    src = GP(sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].oam);
    total += *(src++);
    for (; i < total; i++) {
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.y = attr + 24;
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.x = (attr + 29) & 0x1FF;
        *(raw++) = *(src++);
        gOamBuffer[i].split.priority = 0;
        raw++;
    }

    animation = GP(sUnk_870CE14[gWarioHopSpeedLevel]);
    if (gMinigameState <= 2) {
        gWarioHopPlayerShadow.animTimer++;
    }
    if (animation[gWarioHopPlayerShadow.animFrame].time < gWarioHopPlayerShadow.animTimer) {
        gWarioHopPlayerShadow.animTimer = 0;
        gWarioHopPlayerShadow.animFrame++;
        if (animation[gWarioHopPlayerShadow.animFrame].time == 0) {
            gWarioHopPlayerShadow.animFrame = 0;
        }
    }
    src = GP(animation[gWarioHopPlayerShadow.animFrame].oam);
    total += *(src++);
    for (; i < total; i++) {
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.y = ((s16)gWarioHopPlayerShadow.y >> 4) + attr;
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.x = (attr + ((s16)gWarioHopPlayerShadow.x >> 4)) & 0x1FF;
        *(raw++) = *(src++);
        gOamBuffer[i].split.priority = 1;
        raw++;
    }

    src = GP(sWarioHopPlayerOam->oam);
    total += *(src++);
    for (; i < total; i++) {
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.y = ((s16)gWarioHopPlayer.y >> 4) + attr;
        gOamBuffer[i].split.affineMode = 1;
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.x = (attr + ((s16)gWarioHopPlayer.x >> 4)) & 0x1FF;
        ((WarioHopOamData*)gOamBuffer)[i].split.affineIndex = 0;
        *(raw++) = *(src++);
        gOamBuffer[i].split.priority = 1;
        raw++;
    }

    if (gWarioHopJumpState == 0) {
        gWarioHopRollingObject[3]++;
        if (sWarioHopRollingObjectOam[gWarioHopRollingObject[4]].time < gWarioHopRollingObject[3]) {
            gWarioHopRollingObject[3] = 0;
            gWarioHopRollingObject[4]++;
            if (sWarioHopRollingObjectOam[gWarioHopRollingObject[4]].time == 0) {
                gWarioHopRollingObject[4] = 0;
            }
        }
        src = GP(sWarioHopRollingObjectOam[gWarioHopRollingObject[4]].oam);
        total += *(src++);
        for (; i < total; i++) {
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.y = ((s16)gWarioHopRollingObject[1] >> 4) + attr;
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.x = (attr + ((s16)gWarioHopRollingObject[0] >> 4)) & 0x1FF;
            *(raw++) = *(src++);
            gOamBuffer[i].split.priority = 1;
            raw++;
        }
    }

    for (j = 0; j < 7; j++) {
        if (((struct WarioHopSmallState*)0x03004950)[j].type != 0) {
            animation = GP(sUnk_870CE24[((struct WarioHopSmallState*)0x03004950)[j].type - 1]);
            ((struct WarioHopSmallState*)0x03004950)[j].animTimer++;
            if (animation[((struct WarioHopSmallState*)0x03004950)[j].animFrame].time <
                ((struct WarioHopSmallState*)0x03004950)[j].animTimer) {
                ((struct WarioHopSmallState*)0x03004950)[j].animTimer = 0;
                ((struct WarioHopSmallState*)0x03004950)[j].animFrame++;
                if (animation[((struct WarioHopSmallState*)0x03004950)[j].animFrame].time == 0) {
                    ((struct WarioHopSmallState*)0x03004950)[j].animFrame = 0;
                }
            }
            /* The statement expression adds one luid between the gWarioHopObstacles load and
               its use, so loop.c hoists that load out of the j loop on the first pass. The
               inner loop then keeps its own copy of the address (adds r0, r2, #0). */
            src = GP(animation[gWarioHopObstacles[({ j; })].animFrame].oam);
            total += *(src++);
            for (; i < total; i++) {
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.y = ((s16)gWarioHopObstacles[j].y >> 4) + attr;
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.x = (attr + ((s16)gWarioHopObstacles[j].x >> 4)) & 0x1FF;
                *(raw++) = *(src++);
                gOamBuffer[i].split.priority = 1;
                raw++;
            }
        }
    }

    if (gWarioHopBonusEffect.type != 0) {
        gWarioHopBonusEffect.type++;
        gWarioHopBonusEffect.animTimer++;
        if (gWarioHopBonusEffect.type > 201) {
            gWarioHopBonusEffect.type = 0;
        }
        if (sWarioHopBonusEffectOam[gWarioHopBonusEffect.animFrame].time < gWarioHopBonusEffect.animTimer) {
            gWarioHopBonusEffect.animTimer = 1;
            gWarioHopBonusEffect.animFrame++;
            if (sWarioHopBonusEffectOam[gWarioHopBonusEffect.animFrame].time == 0) {
                gWarioHopBonusEffect.animFrame = 0;
            }
        }
        if (gWarioHopBonusEffect.type != 0) {
            src = GP(sWarioHopBonusEffectOam[gWarioHopBonusEffect.animFrame].oam);
            total += *(src++);
            for (; i < total; i++) {
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.y = sUnk_870CCE0[gWarioHopBonusEffect.type - 2] + attr;
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.x = (attr + sUnk_870CC18[gWarioHopBonusEffect.type - 2]) & 0x1FF;
                *(raw++) = *(src++);
                gOamBuffer[i].split.priority = 2;
                raw++;
            }
        }
    }

    if (gWarioHopMedalEffect.type != 0) {
        gWarioHopMedalEffect.animTimer++;
        if (sWarioHopMedalEffectOam[gWarioHopMedalEffect.animFrame].time < gWarioHopMedalEffect.animTimer) {
            gWarioHopMedalEffect.animTimer = 1;
            gWarioHopMedalEffect.animFrame++;
            if (sWarioHopMedalEffectOam[gWarioHopMedalEffect.animFrame].time == 0) {
                gWarioHopMedalEffect.animFrame = 0;
                gWarioHopMedalEffect.animTimer = 0;
                gWarioHopMedalEffect.type = 0;
            }
        }
        if (gWarioHopMedalEffect.type != 0) {
            src = GP(sWarioHopMedalEffectOam[gWarioHopMedalEffect.animFrame].oam);
            total += *(src++);
            for (; i < total; i++) {
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.y = attr + 140;
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.x = (attr + 160) & 0x1FF;
                *(raw++) = *(src++);
                gOamBuffer[i].split.priority = 3;
                raw++;
            }
        }
    }

    matrix[0] = FixedMul(sSinCosTable[gWarioHopPlayer.angle + 64], (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    *matrix1Ptr = FixedMul(sSinCosTable[gWarioHopPlayer.angle], (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    *matrix2Ptr =
        FixedMul((s16)(-(u16)sSinCosTable[gWarioHopPlayer.angle]), (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    matrixD = FixedMul(sSinCosTable[gWarioHopPlayer.angle + 64], (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    *matrix3Ptr = matrixD;
    gOamBuffer[0].all.affineParam = matrix[0];
    gOamBuffer[1].all.affineParam = *matrix1Ptr;
    gOamBuffer[2].all.affineParam = *matrix2Ptr;
    gOamBuffer[3].all.affineParam = matrixD;

    matrix[0] = FixedMul(sSinCosTable[64], (s16)FixedInverse((s16)gMinigameCamera.angle));
    *matrix1Ptr = FixedMul(sSinCosTable[0], (s16)FixedInverse((s16)gMinigameCamera.angle));
    *matrix2Ptr = FixedMul((s16)(-(u16)sSinCosTable[0]), (s16)FixedInverse((s16)gMinigameCamera.angle));
    matrixD2 = FixedMul(sSinCosTable[64], (s16)FixedInverse((s16)gMinigameCamera.angle));
    *matrix3Ptr = matrixD2;
    gOamBuffer[4].all.affineParam = matrix[0];
    gOamBuffer[5].all.affineParam = *matrix1Ptr;
    gOamBuffer[6].all.affineParam = *matrix2Ptr;
    gOamBuffer[7].all.affineParam = matrixD2;

    gOamSlotsUsed = total;
}

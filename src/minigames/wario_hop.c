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
extern const struct AnimationFrame *const sUnk_870CE14[];
extern const struct AnimationFrame *const sUnk_870CE24[];
extern const s32 sUnk_870CE40[];
extern const s32 sUnk_870CE5C[];
extern const u32 sUnk_870CE78[];
extern const struct AnimationFrame sWarioHopRollingObjectOam[];
extern const struct AnimationFrame sWarioHopMedalHighlightOam[];
extern const struct AnimationFrame sWarioHopPlayerOam[];
extern const struct AnimationFrame sWarioHopBonusEffectOam[];
extern const struct AnimationFrame sWarioHopMedalEffectOam[];
extern const u16 *sUnk_870D880;
extern const u16 *sUnk_870D890;
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
    register const void *dmaSrc4 asm("r12");
    register const void *dmaSrc5 asm("r9");

    register u16 *scrollA asm("r10");
    register u8 *speedBase asm("r8");
    register struct WarioHopObjState *playerBase asm("r6");
    u16 *scrollPtr;
    u32 randomBit;
    register s32 zero asm("r4");
    register u8 persistentByteZero asm("r5");

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
            register u16 *tmpScrollA asm("r7") = gWarioHopBg0Scroll;
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
        register const void *tmpDmaSrc3 asm("r7");

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
        register const void *src asm("r2");

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
        register const void *tmpDmaSrc asm("r3");

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
        register const void *tmpDmaSrc asm("r4");
        register s32 bg0Control asm("r5");
        register s32 bg1Control asm("r7");
        register s32 bg2Control asm("r2");
        register s32 bg3Control asm("r3");
        register s32 storeValue asm("r0");

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
    *(volatile u16 *)&gWarioHopBg1Scroll[0] = zero;
    *(volatile u16 *)&gWarioHopBg1Scroll[1] = zero;
    *(volatile u16 *)&gWarioHopBg2Scroll[0] = zero;
    *(volatile u16 *)&gWarioHopBg2Scroll[1] = zero;

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
        register u8 *speedPtr asm("r7");

        selectedSpeed = sUnk_870CE04[gWarioHopSpeedLevel];
        speedPtr = speedBase;
        *speedPtr = selectedSpeed;
        speed = *speedPtr;

        gWarioHopJumpAcceleration = 0x2800 / (speed * speed);
        {
            s32 speed2;
            s32 accel;

            speed2 = *speedPtr;
            accel = gWarioHopJumpAcceleration;
            gWarioHopJumpStartVelocity = (accel * speed2) >> 1;
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

    gWarioHopPlayerShadow = *playerBase;

    gWarioHopRollingObject[0] = 0xA00;
    gWarioHopRollingObject[1] = 0x8C0;
    ((u16 *)gWarioHopRollingObject)[3] = zero;
    ((u16 *)gWarioHopRollingObject)[4] = zero;

    {
        u8 *subStatePtr;
        u16 *scorePtr;
        register u8 *highScorePtr asm("r6");

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
        register s32 j asm("r1");
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
        register struct WarioHopSmallState *medalEffect asm("r1");

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
    register s32 accelValue asm("r2");
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
    asm("" : : "r"(accelValue));
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
    register s32 temp asm("r0");
    register u16 value asm("r1");

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
    register struct WarioHopSmallState *base asm("r3");
    register s32 passLimit asm("r7");
    register s32 farLimit asm("r6");
    register s32 i asm("r5");

    base = gWarioHopObstacles;
    passLimit = 0xA80;
    asm("" : : "r"(base), "r"(passLimit));
    offset = 0;
    farLimit = 0x1180;
    asm("" : : "r"(farLimit));
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
    register struct MusicPlayerInfo *info asm("r4");
    u16 song;

    song = gSongTable[sUnk_870CDB8[gWarioHopSpeedLevel]].ms;
    switch ((s16)gWarioHopGameOverTimer) {
        case 0:
            info = gMPlayTable[song].info;
            m4aMPlayTempoControl(info, 280);
            m4aMPlayPitchControl(info, 0xFFFF, -100);
            m4aSongNumStart(0x23C);
            break;
        case 30:
            info = gMPlayTable[song].info;
            m4aMPlayTempoControl(info, 220);
            m4aMPlayPitchControl(info, 0xFFFF, -200);
            break;
        case 60:
            info = gMPlayTable[song].info;
            m4aMPlayTempoControl(info, 200);
            m4aMPlayPitchControl(info, 0xFFFF, -300);
            break;
        case 90:
            info = gMPlayTable[song].info;
            m4aMPlayTempoControl(info, 160);
            m4aMPlayPitchControl(info, 0xFFFF, -400);
            m4aMPlayModDepthSet(info, 0xFFFF, 15);
            break;
        case 120:
            info = gMPlayTable[song].info;
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
    s32 i;
    s32 total;
    u16 *raw;
    register const u16 *frame asm("r4");
    struct WarioHopObjState *player;
    struct WarioHopMidState *stateBase;
    register OamData *oam asm("r3");
    u32 oamOffset;
    u32 count;
    u32 active;
    s16 *affinePtr;
    u32 stackPad;
    u32 effectActive;
    u8 *effectActivePtr;
    s32 xMask;
    s32 matrixMask;
    s32 priorityMask;
    /*
     * These four scalars intentionally form contiguous stack storage. B/C/D
     * are reached through affineBPtr/affineCPtr/affineDPtr rather than by name;
     * replacing them with an array changes agbcc's stack layout and codegen.
     */
    s16 affine[4];
#define affineA affine[0]
    s16 *affineBPtr;
    s16 *affineCPtr;
    s16 *affineDPtr;
    u16 affineDResult;
    s32 affineDResult2;

    i = 0;
    total = gOamSlotsUsed;
    oamOffset = total << 3;
    oam = gOamBuffer;
    raw = (u16 *)(oamOffset + (u8 *)oam);
    stateBase = &gMinigameCamera;
    active = stateBase->active;
    affinePtr = &affineA;
    affinePtr++;
    affineBPtr = affinePtr;
    affinePtr++;
    affineCPtr = affinePtr;
    affinePtr++;
    affineDPtr = affinePtr;

#define DRAW_FRAME_COMMON(_frame, _x, _y, _attr0Set, _attr0Clear, _attr1Set, _attr1Clear, _attr2Set, _attr2Clear) \
    do { \
        frame = (_frame); \
        count = *frame++; \
        total += count; \
        while (i < total) { \
            OamData *oam; \
            u16 attr0; \
            u16 attr1; \
            u16 attr2; \
            oam = &gOamBuffer[i]; \
            attr0 = *frame++; \
            *raw++ = attr0; \
            oam->split.y = attr0 + (_y); \
            if ((_attr0Set) != 0 || (_attr0Clear) != 0) { \
                oam->split.affineMode = (_attr0Set); \
            } \
            attr1 = *frame++; \
            *raw++ = attr1; \
            oam->split.x = attr1 + (_x); \
            if ((_attr1Set) != 0 || (_attr1Clear) != 0) { \
                oam->split.matrixNum = (_attr1Set) >> 1; \
            } \
            attr2 = *frame++; \
            *raw++ = attr2; \
            oam->split.priority = (_attr2Set) >> 2; \
            raw++; \
            i++; \
        } \
    } while (0)

    if (active != 0) {
        if (active == 1) {
            frame = sUnk_870D880;
        }
        if (active == 2) {
            frame = sUnk_870D890;
        }
        count = *frame;
        total += count;
        frame++;
        if (i < total) {
            struct WarioHopMidState *state;
            s32 tempXMask;
            s32 tempMatrixMask;
            s32 tempPriorityMask;

            state = stateBase;
            tempXMask = 0xFFFFFE00;
            xMask = tempXMask;
            tempMatrixMask = -15;
            matrixMask = tempMatrixMask;
            tempPriorityMask = -13;
            priorityMask = tempPriorityMask;
            do {
                s32 value;
                s32 newX;
                register u16 oldAttr asm("r2");
                register s32 mergedAttr asm("r0");
                s32 coord;
                s32 finalValue;
                register s32 attrHigh asm("r1");
                register u8 *oamBytes asm("r3");

                value = *frame++;
                *raw++ = value;
                coord = (u8)state->yOffset;
                coord += value;
                coord -= 16;
                oam->split.y = coord;
                oam->split.affineMode = 3;

                value = *frame++;
                *raw++ = value;
                newX = value + (s16)state->xOffset;
                newX -= 32;
                newX &= 0x1FF;
                oldAttr = oam->all.attr1;
                mergedAttr = xMask;
                mergedAttr &= oldAttr;
                mergedAttr |= newX;
                oam->all.attr1 = mergedAttr;
                oamBytes = (u8 *)oam;
                attrHigh = oamBytes[3];
                mergedAttr = matrixMask;
                mergedAttr &= attrHigh;
                mergedAttr |= 2;
                oamBytes[3] = mergedAttr;

                finalValue = *frame;
                *raw = finalValue;
                frame++;
                attrHigh = oamBytes[5];
                mergedAttr = priorityMask;
                mergedAttr &= attrHigh;
                oamBytes[5] = mergedAttr;

                raw += 2;
                oam++;
                i++;
            } while (i < total);
        }
    }

    effectActivePtr = &gMinigameMedalHighlight;
    effectActive = *effectActivePtr;
    if (effectActive != 0) {
        gMinigameUiAnimation[0]++;
        if (sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].time < gMinigameUiAnimation[0]) {
            gMinigameUiAnimation[0] = 1;
            gMinigameUiAnimation[1]++;
            if (sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].time == 0) {
                u32 resetZero;
                u8 *resetActivePtr;

                gMinigameUiAnimation[1] = 0;
                gMinigameUiAnimation[0] = 0;
                resetZero = 0;
                resetActivePtr = &gMinigameMedalHighlight;
                *resetActivePtr = resetZero;
                WarioHopDrawScoreDigits();
            }
        }
    }
    {
        OamData *oamBase;
        OamData *loopOam;
        u32 offset;
        s32 remainingBase;
        s32 tempCoordMask;
        s32 coordMask;
        s32 tempClearX;
        register s32 clearX asm("r12");
        s32 tempPriority;
        s32 drawPriorityMask;

        frame = sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].oam;
        count = *frame;
        total += count;
        frame++;
        player = &gWarioHopPlayerShadow;
        if (i < total) {
            oamBase = gOamBuffer;
            tempCoordMask = 0x1FF;
            coordMask = tempCoordMask;
            tempClearX = 0xFFFFFE00;
            clearX = tempClearX;
            tempPriority = -13;
            drawPriorityMask = tempPriority;
            offset = i << 3;
            loopOam = (OamData *)(offset + (u8 *)oamBase);
            remainingBase = total;
            i = remainingBase - i;
            do {
                register s32 value asm("r2");
                register s32 coord asm("r0");
                s32 newX;
                register u16 oldAttr asm("r2");
                register s32 mergedAttr asm("r0");
                s32 finalValue;
                register s32 attrHigh asm("r1");
                s32 maskTemp;

                value = *frame++;
                *raw++ = value;
                coord = value;
                coord += 24;
                loopOam->split.y = coord;

                value = *frame++;
                *raw++ = value;
                newX = value;
                newX += 29;
                maskTemp = coordMask;
                newX &= maskTemp;
                oldAttr = loopOam->all.attr1;
                mergedAttr = clearX;
                mergedAttr &= oldAttr;
                mergedAttr |= newX;
                loopOam->all.attr1 = mergedAttr;

                finalValue = *frame;
                *raw = finalValue;
                frame++;
                attrHigh = ((u8 *)&loopOam->all.attr2)[1];
                mergedAttr = drawPriorityMask;
                mergedAttr &= attrHigh;
                ((u8 *)&loopOam->all.attr2)[1] = mergedAttr;

                raw += 2;
                loopOam++;
                i--;
            } while (i != 0);
            i = total;
        }
    }

    {
        register u8 *variantPtr asm("r3");
        u32 variantOffset;
        register const struct AnimationFrame *const *animationTable asm("r1");
        const struct AnimationFrame *playerAnim;
        const struct AnimationFrame **animationEntry;
        register u16 playerTimer asm("r1");
        u32 playerFrameTime;
        register u8 *phasePtr asm("r2");
        OamData *oamBase;
        OamData *loopOam;
        u32 oamByteOffset;
        register s32 tempClearX asm("r2");
        s32 clearX;
        register s32 tempPriority asm("r3");
        s32 drawPriorityMask;
        s32 tempCoordMask;
        s32 coordMask;
        s32 remainingBase;

        variantPtr = &gWarioHopSpeedLevel;
        variantOffset = *variantPtr;
        variantOffset <<= 2;
        animationTable = sUnk_870CE14;
        animationEntry = (const struct AnimationFrame **)(variantOffset + (u32)animationTable);
        playerAnim = *animationEntry;
        phasePtr = &gMinigameState;
        if (*phasePtr <= 2) {
            player->animTimer++;
        }
        playerFrameTime = playerAnim[player->animFrame].time;
        playerTimer = player->animTimer;
        if (playerFrameTime < playerTimer) {
            player->animTimer = 0;
            player->animFrame++;
            if (playerAnim[player->animFrame].time == 0) {
                player->animFrame = 0;
            }
        }

        frame = playerAnim[player->animFrame].oam;
        count = *frame;
        total += count;
        frame++;
        if (i < total) {
            oamBase = gOamBuffer;
            tempClearX = 0xFFFFFE00;
            clearX = tempClearX;
            tempPriority = -13;

            drawPriorityMask = tempPriority;

            oamByteOffset = i << 3;
            loopOam = (OamData *)(oamByteOffset + (u8 *)oamBase);
            tempCoordMask = 0x1FF;
            {
                register s32 coordMaskSeed asm("r2") = tempCoordMask;
                coordMask = coordMaskSeed;
            }
            remainingBase = total;
            i = remainingBase - i;
            do {
                s32 value;
                s32 coord;
                s32 newX;
                s32 maskTemp;
                register u16 oldAttr asm("r2");
                register s32 mergedAttr asm("r0");
                s32 finalValue;
                register s32 attrHigh asm("r1");

                value = *frame++;
                *raw++ = value;
                coord = player->y;
                coord <<= 16;
                coord >>= 20;
                coord += value;
                loopOam->split.y = coord;

                value = *frame++;
                *raw++ = value;
                newX = player->x;
                newX <<= 16;
                newX >>= 20;
                newX = (value) - (-(newX));
                maskTemp = coordMask;
                newX &= maskTemp;
                oldAttr = loopOam->all.attr1;
                mergedAttr = clearX;
                mergedAttr &= oldAttr;
                mergedAttr |= newX;
                loopOam->all.attr1 = mergedAttr;

                finalValue = *frame;
                *raw = finalValue;
                frame++;
                attrHigh = ((u8 *)&loopOam->all.attr2)[1];
                mergedAttr = drawPriorityMask;
                mergedAttr &= attrHigh;
                mergedAttr |= 4;
                ((u8 *)&loopOam->all.attr2)[1] = mergedAttr;

                raw += 2;
                loopOam++;
                i--;
            } while (i != 0);
            i = total;
        }
    }

    {
        const struct AnimationFrame *base;
        const u16 *currentFrame;
        struct WarioHopObjState *currentPlayer;
        OamData *oamBase;
        OamData *oamBaseHigh;
        s32 tempMatrixMask;
        s32 drawMatrixMask;
        s32 tempPriorityMask;
        s32 drawPriorityMask;

        base = sWarioHopPlayerOam;
        currentFrame = base[0].oam;
        count = *currentFrame;
        total += count;
        currentFrame++;
        if (i < total) {
            currentPlayer = &gWarioHopPlayer;
            oamBase = gOamBuffer;
            oamBaseHigh = oamBase;
            tempMatrixMask = -15;
            drawMatrixMask = tempMatrixMask;
            tempPriorityMask = -13;
            drawPriorityMask = tempPriorityMask;
            do {
                s32 value;
                u32 byteOffset;
                OamData *loopOam;
                s32 coord;
                s32 attrHigh;
                s32 newX;
                u16 oldAttr;
                register s32 mergedAttr asm("r0");
                s32 finalValue;

                value = *currentFrame++;
                *raw++ = value;
                byteOffset = i << 3;
                loopOam = oamBaseHigh;
                loopOam = (OamData *)((u8 *)loopOam + byteOffset);
                coord = currentPlayer->y;
                coord <<= 16;
                coord >>= 20;
                coord += value;
                loopOam->split.y = coord;
                attrHigh = ((u8 *)&loopOam->all.attr0)[1];
                coord = -4;
                coord &= attrHigh;
                attrHigh = 1;
                coord |= attrHigh;
                ((u8 *)&loopOam->all.attr0)[1] = coord;

                value = *currentFrame++;
                *raw++ = value;
                newX = currentPlayer->x;
                newX <<= 16;
                newX >>= 20;
                newX = (value) - (-(newX));
                newX &= 0x1FF;
                oldAttr = loopOam->all.attr1;
                mergedAttr = 0xFFFFFE00;
                mergedAttr &= oldAttr;
                mergedAttr |= newX;
                loopOam->all.attr1 = mergedAttr;
                attrHigh = ((u8 *)&loopOam->all.attr1)[1];
                mergedAttr = drawMatrixMask;
                mergedAttr &= attrHigh;
                ((u8 *)&loopOam->all.attr1)[1] = mergedAttr;

                finalValue = *currentFrame;
                *raw = finalValue;
                currentFrame++;
                attrHigh = ((u8 *)&loopOam->all.attr2)[1];
                mergedAttr = drawPriorityMask;
                mergedAttr &= attrHigh;
                attrHigh = 4;
                mergedAttr |= attrHigh;
                ((u8 *)&loopOam->all.attr2)[1] = mergedAttr;

                raw += 2;
                i++;
            } while (i < total);
        }
    }

    {
        u8 *companionActivePtr;
        u32 zero;

        companionActivePtr = &gWarioHopJumpState;
        zero = *companionActivePtr;
        if (zero == 0) {
            u16 *companion;
            register const struct AnimationFrame *companionAnim asm("r3");
            register u16 timer asm("r1");
            u32 frameTime;
            const u16 *currentFrame;

            companion = gWarioHopRollingObject;
            timer = companion[3];
            timer++;
            companion[3] = timer;
            companionAnim = sWarioHopRollingObjectOam;
            frameTime = companionAnim[companion[4]].time;
            if (frameTime < timer) {
                companion[3] = zero;
                companion[4]++;
                if (companionAnim[companion[4]].time == 0) {
                    companion[4] = 0;
                }
            }

            currentFrame = companionAnim[companion[4]].oam;
            count = *currentFrame;
            total += count;
            currentFrame++;
            if (i < total) {
                register OamData *oamBase asm("r1");
                u16 *drawCompanion;
                s32 tempClearX;
                s32 clearX;
                s32 tempPriorityMask;
                s32 drawPriorityMask;
                register u32 byteOffset asm("r0");
                register OamData *loopOam asm("r3");
                s32 tempCoordMask;
                s32 coordMask;
                s32 remainingBase;

                oamBase = gOamBuffer;
                drawCompanion = companion;
                tempClearX = 0xFFFFFE00;
                clearX = tempClearX;
                tempPriorityMask = -13;
                drawPriorityMask = tempPriorityMask;
                byteOffset = i << 3;
                loopOam = (OamData *)(byteOffset + (u32)oamBase);
                tempCoordMask = 0x1FF;
                coordMask = tempCoordMask;
                remainingBase = total;
                i = remainingBase - i;
                do {
                    s32 value;
                    s32 coord;
                    register s32 newX asm("r1");
                    s32 maskTemp;
                    register u16 oldAttr asm("r2");
                    s32 mergedAttr;
                    s32 finalValue;
                    register s32 attrHigh asm("r1");

                    value = *currentFrame++;
                    *raw++ = value;
                    coord = drawCompanion[1];
                    coord <<= 16;
                    coord >>= 20;
                    coord += value;
                    loopOam->split.y = coord;

                    value = *currentFrame++;
                    *raw++ = value;
                    newX = drawCompanion[0];
                    newX <<= 16;
                    newX >>= 20;
                    newX = value + newX;
                    maskTemp = coordMask;
                    newX &= maskTemp;
                    oldAttr = loopOam->all.attr1;
                    mergedAttr = clearX;
                    mergedAttr &= oldAttr;
                    mergedAttr |= newX;
                    loopOam->all.attr1 = mergedAttr;

                    finalValue = *currentFrame;
                    *raw = finalValue;
                    currentFrame++;
                    attrHigh = ((u8 *)&loopOam->all.attr2)[1];
                    mergedAttr = drawPriorityMask;
                    mergedAttr &= attrHigh;
                    attrHigh = 4;
                    mergedAttr |= attrHigh;
                    ((u8 *)&loopOam->all.attr2)[1] = mergedAttr;

                    raw += 2;
                    loopOam++;
                    i--;
                } while (i != 0);
                i = total;
            }
        }
    }

    {
        register s32 itemIndex asm("r6");
        s32 nextIndex;

        itemIndex = 0;
        do {
            u32 indexTwice;
            u32 itemOffset;
            struct WarioHopSmallState *itemsBase;
            register struct WarioHopSmallState *currentItem asm("r2");
            u32 itemType;
            register u32 savedIndexTwice asm("r12");
            s32 nextIndexTemp;

            indexTwice = itemIndex << 1;
            itemOffset = indexTwice + itemIndex;
            itemOffset <<= 2;
            itemsBase = (struct WarioHopSmallState *)0x03004950;
            currentItem = (struct WarioHopSmallState *)((u8 *)itemsBase + itemOffset);
            itemType = currentItem->type;
            savedIndexTwice = indexTwice;
            nextIndexTemp = itemIndex + 1;
            nextIndex = nextIndexTemp;
            if (itemType != 0) {
                const struct AnimationFrame *const *animationTable;
                u32 typeOffset;
                const struct AnimationFrame *currentAnim;
                u32 timer;
                u32 frameTime;
                s32 zero;
                const u16 *currentFrame;
                register struct WarioHopSmallState *drawBase asm("r2");
                register struct WarioHopSmallState *drawItemBase asm("r0");
                register struct WarioHopSmallState *drawItem;
                u32 savedForOffset;
                u32 offsetSum;
                struct WarioHopSmallState *frameState;

                animationTable = sUnk_870CE24;
                typeOffset = currentItem->type - 1;
                typeOffset <<= 2;
                currentAnim = *(const struct AnimationFrame **)(typeOffset + (u32)animationTable);
                timer = currentItem->animTimer;
                timer++;
                zero = 0;
                currentItem->animTimer = timer;
                frameTime = currentAnim[currentItem->animFrame].time;
                if (frameTime < (u16)timer) {
                    currentItem->animTimer = zero;
                    currentItem->animFrame++;
                    if (currentAnim[currentItem->animFrame].time == 0) {
                        currentItem->animFrame = 0;
                    }
                }

                savedForOffset = savedIndexTwice;
                offsetSum = savedForOffset + itemIndex;
                itemOffset = offsetSum << 2;
                drawBase = gWarioHopObstacles;
                frameState = (struct WarioHopSmallState *)(itemOffset + (u32)drawBase);
                currentFrame = currentAnim[frameState->animFrame].oam;
                count = *currentFrame;
                total += count;
                currentFrame++;
                itemIndex++;
                nextIndex = itemIndex;
                if (i < total) {
                    register OamData *oamBase asm("r1");
                    register u32 byteOffset asm("r0");
                    register OamData *loopOam asm("r3");
                    s32 tempCoordMask;
                    s32 coordMask;
                    s32 tempPriorityMask;
                    register s32 drawPriorityMask asm("r12");
                    s32 remainingBase;

                    drawItemBase = drawBase;

                    drawItem = (struct WarioHopSmallState *)(itemOffset - -(int)drawItemBase);
                    byteOffset = i << 3;
                    oamBase = gOamBuffer;
                    loopOam = (OamData *)(byteOffset + (u32)oamBase);
                    tempCoordMask = 0x1FF;
                    coordMask = tempCoordMask;
                    tempPriorityMask = -13;
                    drawPriorityMask = tempPriorityMask;
                    remainingBase = total;
                    i = remainingBase - i;
                    do {
                        s32 value;
                        s32 coord;
                        s32 newX;
                        s32 maskTemp;
                        u16 oldAttr;
                        register s32 mergedAttr asm("r0");
                        s32 finalValue;
                        s32 attrHigh;

                        value = *currentFrame++;
                        *raw++ = value;
                        coord = drawItem->y;
                        coord <<= 16;
                        coord >>= 20;
                        coord += value;
                        loopOam->split.y = coord;

                        value = *currentFrame++;
                        *raw++ = value;
                        newX = drawItem->x;
                        newX <<= 16;
                        newX >>= 20;
                        newX = (value) - (-(newX));
                        maskTemp = coordMask;
                        newX &= maskTemp;
                        oldAttr = loopOam->all.attr1;
                        mergedAttr = 0xFFFFFE00;
                        mergedAttr &= oldAttr;
                        mergedAttr |= newX;
                        loopOam->all.attr1 = mergedAttr;

                        finalValue = *currentFrame;
                        *raw = finalValue;
                        currentFrame++;
                        attrHigh = ((u8 *)&loopOam->all.attr2)[1];
                        mergedAttr = drawPriorityMask;
                        mergedAttr &= attrHigh;
                        attrHigh = 4;
                        mergedAttr |= attrHigh;
                        ((u8 *)&loopOam->all.attr2)[1] = mergedAttr;

                        raw += 2;
                        loopOam++;
                        i--;
                    } while (i != 0);
                    i = total;
                }
            }
            itemIndex = nextIndex;
        } while (itemIndex <= 6);
    }

    {
        register struct WarioHopSmallState *effectState asm("r3");
        register u32 effectType asm("r1");

        effectState = &gWarioHopBonusEffect;
        effectType = effectState->type;
        if (effectType == 0) {
            goto warioHopEffectDone;
        }
        effectType++;
        effectState->type = effectType;
        effectState->animTimer++;
        if ((u8)effectType > 201) {
            effectState->type = 0;
        }
    }
    {
        const struct AnimationFrame *animBase;
        register u32 r2Value asm("r2");
        register u32 r3Value asm("r3");
        u32 frameTime;
        const u16 *currentFrame;
        struct WarioHopSmallState *drawStateSource;

        animBase = sWarioHopBonusEffectOam;
        r2Value = (u32)&gWarioHopBonusEffect;
        frameTime = animBase[((struct WarioHopSmallState *)r2Value)->animFrame].time;
        r2Value = (u32)animBase;
        r3Value = (u32)&gWarioHopBonusEffect;
        r3Value = ((struct WarioHopSmallState *)r3Value)->animTimer;
        if (frameTime < r3Value) {
            u32 one;
            struct WarioHopSmallState *updateState;

            one = 1;
            updateState = &gWarioHopBonusEffect;
            updateState->animTimer = one;
            updateState->animFrame++;
            if (((const struct AnimationFrame *)r2Value)[updateState->animFrame].time == 0) {
                updateState->animFrame = 0;
            }
        }
        r3Value = (u32)&gWarioHopBonusEffect;
        drawStateSource = (struct WarioHopSmallState *)r3Value;
        if (drawStateSource->type != 0) {
            currentFrame = ((const struct AnimationFrame *)r2Value)[drawStateSource->animFrame].oam;
            count = *currentFrame;
            total += count;
            currentFrame++;
            if (i < total) {
                register OamData *oamBase asm("r1");
                struct WarioHopSmallState *drawState;
                register const u8 *xTable asm("r0");
                register const u8 *xTableHigh;
                register u32 byteOffset asm("r0");
                register OamData *loopOam asm("r3");
                register s32 tempCoordMask asm("r1");
                s32 coordMask;
                register s32 tempPriorityMask asm("r2");
                register s32 drawPriorityMask asm("r10");
                s32 remainingBase;

                oamBase = gOamBuffer;
                drawState = drawStateSource;
                xTable = sUnk_870CC18;

                xTableHigh = xTable;
                byteOffset = i << 3;
                loopOam = (OamData *)(byteOffset + (u32)oamBase);
                tempCoordMask = 0x1FF;

                coordMask = tempCoordMask;
                tempPriorityMask = -13;

                drawPriorityMask = tempPriorityMask;
                remainingBase = total;
                i = remainingBase - i;
                do {
                    s32 value;
                    s32 coord;
                    s32 xIndex;
                    s32 newX;
                    s32 maskTemp;
                    u16 oldAttr;
                    register s32 mergedAttr asm("r0");
                    s32 finalValue;
                    s32 attrHigh;

                    value = *currentFrame++;
                    *raw++ = value;
                    coord = drawState->type;
                    coord -= 2;
                    coord = sUnk_870CCE0[coord];
                    coord += value;
                    loopOam->split.y = coord;

                    value = *currentFrame++;
                    *raw++ = value;
                    xIndex = drawState->type;
                    xIndex -= 2;
                    xIndex += (u32)xTableHigh;
                    newX = *(u8 *)xIndex;
                    newX = (value) - (-(newX));
                    maskTemp = coordMask;
                    newX &= maskTemp;
                    oldAttr = loopOam->all.attr1;
                    mergedAttr = 0xFFFFFE00;
                    mergedAttr &= oldAttr;
                    mergedAttr |= newX;
                    loopOam->all.attr1 = mergedAttr;

                    finalValue = *currentFrame;
                    *raw = finalValue;
                    currentFrame++;
                    attrHigh = ((u8 *)&loopOam->all.attr2)[1];
                    mergedAttr = drawPriorityMask;
                    mergedAttr &= attrHigh;
                    attrHigh = 8;
                    mergedAttr |= attrHigh;
                    ((u8 *)&loopOam->all.attr2)[1] = mergedAttr;

                    raw += 2;
                    loopOam++;
                    i--;
                } while (i != 0);
                i = total;
            }
        }
    }
warioHopEffectDone:
;

    {
        register struct WarioHopSmallState *effectState asm("r3");
        const struct AnimationFrame *effectAnim;
        register u16 timer asm("r1");

        effectState = &gWarioHopMedalEffect;
        if (effectState->type != 0) {
            timer = effectState->animTimer;
            timer++;
            effectState->animTimer = timer;
            effectAnim = sWarioHopMedalEffectOam;
            if (effectAnim[effectState->animFrame].time < timer) {
                effectState->animTimer = 1;
                effectState->animFrame++;
                if (effectAnim[effectState->animFrame].time == 0) {
                    effectState->animFrame = 0;
                    effectState->animTimer = 0;
                    effectState->type = 0;
                }
            }
            {
                struct WarioHopSmallState *drawStateSource;
                const u16 *currentFrame;

                drawStateSource = &gWarioHopMedalEffect;
                if (drawStateSource->type != 0) {
                    currentFrame = effectAnim[drawStateSource->animFrame].oam;
                count = *currentFrame;
                total += count;
                currentFrame++;
                if (i < total) {
                    register OamData *oamBase asm("r1");
                    s32 coordMask;
                    register s32 tempClearX asm("r2");
                    register s32 clearX asm("r8");
                    register u32 byteOffset asm("r0");
                    register OamData *loopOam asm("r3");
                    s32 remainingBase;

                    oamBase = gOamBuffer;
                    coordMask = 0x1FF;
                    tempClearX = 0xFFFFFE00;

                    clearX = tempClearX;
                    byteOffset = i << 3;
                    loopOam = (OamData *)(byteOffset + (u32)oamBase);
                    remainingBase = total;
                    i = remainingBase - i;
                    do {
                        register s32 value asm("r2");
                        register s32 coord asm("r0");
                        s32 newX;
                        register u16 oldAttr asm("r2");
                        register s32 mergedAttr asm("r0");
                        s32 finalValue;
                        s32 attrHigh;
                        s32 priorityBits;

                        value = *currentFrame++;
                        *raw++ = value;
                        coord = value;
                        coord += 140;
                        loopOam->split.y = coord;

                        value = *currentFrame++;
                        *raw++ = value;
                        newX = value;
                        newX += 160;
                        newX &= coordMask;
                        oldAttr = loopOam->all.attr1;
                        mergedAttr = clearX;
                        mergedAttr &= oldAttr;
                        mergedAttr |= newX;
                        loopOam->all.attr1 = mergedAttr;

                        finalValue = *currentFrame;
                        *raw = finalValue;
                        currentFrame++;
                        attrHigh = ((u8 *)&loopOam->all.attr2)[1];
                        priorityBits = 12;
                        attrHigh |= priorityBits;
                        ((u8 *)&loopOam->all.attr2)[1] = attrHigh;

                        raw += 2;
                        loopOam++;
                        i--;
                    } while (i != 0);
                    i = total;
                }
                }
            }
        }
    }

#undef DRAW_FRAME_COMMON

    {
        register struct WarioHopObjState *affineState;
        u32 affineAddress;
        register const s16 *affineTable;
        s32 affineZero;
        s32 affineSine;
        s32 affineScaleOffset;
        register s32 affineInverse asm("r0");
        s32 affineResult;
        s16 *affineDst;

        affineState = &gWarioHopPlayer;
        affineAddress = affineState->angle;
        affineAddress += 64;
        affineAddress <<= 1;
        affineTable = sSinCosTable;
        affineAddress += (u32)affineTable;
        affineZero = 0;
        affineSine = *(s16 *)((u8 *)affineAddress + affineZero);
        affineScaleOffset = 6;
        affineInverse = *(s16 *)((u8 *)affineState + affineScaleOffset);
        affineInverse = FixedInverse(affineInverse);
        affineResult = FixedMul(affineSine, (s16)affineInverse);
        affineDst = &affineA;
        *affineDst = affineResult;
    }
    {
        register struct WarioHopObjState *affineState;
        u32 affineAddress;
        register const s16 *affineTable;
        s32 affineZero;
        s32 affineSine;
        s32 affineScaleOffset;
        register s32 affineInverse asm("r0");
        s32 affineResult;
        s16 *affineDst;

        affineState = &gWarioHopPlayer;
        affineAddress = affineState->angle;
        affineAddress <<= 1;
        affineTable = sSinCosTable;
        affineAddress += (u32)affineTable;
        affineZero = 0;
        affineSine = *(s16 *)(affineAddress + affineZero);
        affineScaleOffset = 6;
        affineInverse = *(s16 *)((u8 *)affineState + affineScaleOffset);
        affineInverse = FixedInverse(affineInverse);
        affineResult = FixedMul(affineSine, (s16)affineInverse);
        affineDst = affineBPtr;
        *affineDst = affineResult;
    }
    {
        register struct WarioHopObjState *affineState;
        u32 affineAddress;
        register const u16 *affineTable;
        register s32 affineSine asm("r4");
        s32 affineScaleOffset;
        register s32 affineInverse asm("r0");
        s32 affineResult;
        s16 *affineDst;

        affineState = &gWarioHopPlayer;
        affineAddress = affineState->angle;
        affineAddress <<= 1;
        affineTable = (const u16 *)sSinCosTable;
        affineAddress += (u32)affineTable;
        affineSine = *(u16 *)affineAddress;
        affineSine = -affineSine;
        affineSine = (s16)affineSine;
        affineScaleOffset = 6;
        affineInverse = *(s16 *)((u8 *)affineState + affineScaleOffset);
        affineInverse = FixedInverse(affineInverse);
        affineResult = FixedMul(affineSine, (s16)affineInverse);
        affineDst = affineCPtr;
        *affineDst = affineResult;
    }
    {
        struct WarioHopObjState *affineState;
        register u32 affineAddress;
        register const s16 *affineTable;
        s32 affineZero;
        s32 affineSine;
        register s32 affineScaleOffset asm("r2");
        register s32 affineInverse asm("r0");
        register s16 *affineDst asm("r3");

        affineState = &gWarioHopPlayer;
        affineAddress = affineState->angle;
        affineAddress += 64;
        affineAddress <<= 1;
        affineTable = sSinCosTable;
        affineAddress += (u32)affineTable;
        affineZero = 0;
        affineSine = *(s16 *)((u8 *)affineAddress + affineZero);
        affineScaleOffset = 6;
        affineInverse = *(s16 *)((u8 *)affineState + affineScaleOffset);
        affineInverse = FixedInverse(affineInverse);
        affineDResult = FixedMul(affineSine, (s16)affineInverse);
        affineDst = affineDPtr;
        *(volatile s16 *)affineDst = affineDResult;
    }

    gOamBuffer[0].all.affineParam = affineA;
    gOamBuffer[1].all.affineParam = *affineBPtr;
    gOamBuffer[2].all.affineParam = *affineCPtr;
    gOamBuffer[3].all.affineParam = affineDResult;

    {
        const s16 *affineTable;
        s32 affineZero;
        s32 affineCosTemp;
        s32 affineSecondCos;
        struct WarioHopMidState *affineState;
        s32 affineScaleOffset;
        register s32 affineInverse asm("r0");
        s32 affineResult;
        s16 *affineDst;

        affineTable = sSinCosTable;
        affineTable += 64;
        affineZero = 0;
        affineCosTemp = *(s16 *)((u8 *)affineTable + affineZero);
        affineSecondCos = affineCosTemp;
        affineState = &gMinigameCamera;
        affineScaleOffset = 4;
        affineInverse = *(s16 *)((u8 *)affineState + affineScaleOffset);
        affineInverse = FixedInverse(affineInverse);
        affineResult = FixedMul(affineSecondCos, (s16)affineInverse);
        affineDst = &affineA;
        *affineDst = affineResult;

        {
            register const s16 *affineTableB;
            s32 affineZeroB;
            s32 affineSineB;
            s32 affineScaleOffsetB;
            register s32 affineInverseB asm("r0");
            s32 affineResultB;
            s16 *affineDstB;

            affineTableB = sSinCosTable;
            affineZeroB = 0;
            affineSineB = *(s16 *)((u8 *)affineTableB + affineZeroB);
            affineScaleOffsetB = 4;
            affineInverseB = *(s16 *)((u8 *)affineState + affineScaleOffsetB);
            affineInverseB = FixedInverse(affineInverseB);
            affineResultB = FixedMul(affineSineB, (s16)affineInverseB);
            affineDstB = affineBPtr;
            *affineDstB = affineResultB;
        }
        {
            const u16 *affineTableC;
            register s32 affineSineC asm("r4");
            s32 affineScaleOffsetC;
            register s32 affineInverseC asm("r0");
            s32 affineResultC;
            s16 *affineDstC;

            affineTableC = (const u16 *)sSinCosTable;
            affineSineC = *affineTableC;
            affineSineC = -affineSineC;
            affineSineC = (s16)affineSineC;
            affineScaleOffsetC = 4;
            affineInverseC = *(s16 *)((u8 *)affineState + affineScaleOffsetC);
            affineInverseC = FixedInverse(affineInverseC);
            affineResultC = FixedMul(affineSineC, (s16)affineInverseC);
            affineDstC = affineCPtr;
            *affineDstC = affineResultC;
        }
        {
            s32 affineScaleOffsetD;
            register s32 affineInverseD asm("r0");
            s16 *affineDstD;

            affineScaleOffsetD = 4;
            affineInverseD = *(s16 *)((u8 *)affineState + affineScaleOffsetD);
            affineInverseD = FixedInverse(affineInverseD);
            affineDResult2 = FixedMul(affineSecondCos, (s16)affineInverseD);
            affineDstD = affineDPtr;
            *affineDstD = affineDResult2;
        }
    }

    gOamBuffer[4].all.affineParam = affineA;
    gOamBuffer[5].all.affineParam = *affineBPtr;
    gOamBuffer[6].all.affineParam = *affineCPtr;
    gOamBuffer[7].all.affineParam = affineDResult2;

#undef affineA
    gOamSlotsUsed = total;
}

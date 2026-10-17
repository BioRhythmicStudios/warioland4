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

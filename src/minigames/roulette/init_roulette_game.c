#include "minigames/roulette.h"
#include "minigame.h"
#include "stage_select.h"

void InitRouletteGame(void)
{
  int i;
  u8 *firstTileBlock;
  u8 *secondTileBlock;
  register u8 *thirdTileBlock PIN("r9");
  register u8 *postLoopTiles PIN("r12");
  u8 *nextPalette;
  u8 *mainTiles;
  u8 *objectTiles;
  register u8 *loadTemp PIN("r6");
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    dmaRegs[0] = (u32) sRouletteBgPalette;
    dmaRegs[1] = (u32) ((void *) 0x05000000);
    dmaRegs[2] = (u32) 0x80000100;
    dmaRegs[2];
  }
  ;
  {
    volatile u32 *dmaRegs;
    u32 waitValue;
    u32 busyMask;

    dmaRegs = (volatile u32 *)0x040000D4;
    waitValue = dmaRegs[2];
    busyMask = 0x80000000;
    nextPalette = sRouletteObjPalette;
    mainTiles = sRouletteBgTiles;
    objectTiles = sRouletteObjTiles;
    /* agbcc cannot retain these addresses in high registers from pure assignments. Each boundary emits one target move; the literal load remains compiler-generated. */
    loadTemp = sRouletteTopResultTiles;
    ASM_BARRIER_VOLATILE(: "=r"(firstTileBlock) : "0"(loadTemp));
    loadTemp = sRouletteMiddleResultTiles;
    ASM_BARRIER_VOLATILE(: "=r"(secondTileBlock) : "0"(loadTemp));
    loadTemp = sRouletteBottomResultTiles;
    ASM_BARRIER_VOLATILE(: "=r"(thirdTileBlock) : "0"(loadTemp));
    loadTemp = sRouletteExtraBgTiles;
    ASM_BARRIER_VOLATILE(: "=r"(postLoopTiles) : "0"(loadTemp));
    if ((s32)waitValue < 0)
    {
      do { waitValue = dmaRegs[2] & busyMask; } while (waitValue != 0);
    }
  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    dmaRegs[0] = (u32) nextPalette;
    dmaRegs[1] = (u32) ((void *) 0x05000200);
    dmaRegs[2] = (u32) 0x80000100;
    dmaRegs[2];
  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    while (dmaRegs[2] & 0x80000000)
    {
      ;
    }

  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    dmaRegs[0] = (u32) mainTiles;
    dmaRegs[1] = (u32) ((void *) 0x06000000);
    dmaRegs[2] = (u32) 0x80003400;
    dmaRegs[2];
  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    while (dmaRegs[2] & 0x80000000)
    {
      ;
    }

  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    dmaRegs[0] = (u32) objectTiles;
    dmaRegs[1] = (u32) ((void *) 0x06010000);
    dmaRegs[2] = (u32) 0x80001800;
    dmaRegs[2];
  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    while (dmaRegs[2] & 0x80000000)
    {
      ;
    }

  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    dmaRegs[0] = (u32) firstTileBlock;
    dmaRegs[1] = (u32) ((void *) 0x06013000);
    dmaRegs[2] = (u32) 0x80002000;
    dmaRegs[2];
  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    while (dmaRegs[2] & 0x80000000)
    {
      ;
    }

  }
  ;
  {
    register int dmaIndex PIN("r5");
    int nextIndex;
    register volatile u32 *dmaRegs PIN("r2");
    u32 dmaControl;
    u32 dmaBusyMask;
    u32 tileOffset;
    u32 value;
    u32 address;

    dmaIndex = 0;
    dmaRegs = (volatile u32 *)0x040000D4;
    dmaControl = 0x80000080;
    dmaBusyMask = 0x80000000;
    do
    {
      tileOffset = dmaIndex << 10;
      address = (u32)firstTileBlock;
      value = tileOffset + address;
      dmaRegs[0] = value;
      address = 0x06017000;
      value = tileOffset + address;
      dmaRegs[1] = value;
      dmaRegs[2] = dmaControl;
      value = dmaRegs[2];
      value = dmaRegs[2] & dmaBusyMask;
      nextIndex = dmaIndex + 1;
      if (value != 0)
      {
        do { value = dmaRegs[2] & 0x80000000; } while (value != 0);
      }

      address = (u32)secondTileBlock;
      value = tileOffset + address;
      dmaRegs[0] = value;
      address = 0x06017100;
      value = tileOffset + address;
      dmaRegs[1] = value;
      dmaRegs[2] = dmaControl;
      value = dmaRegs[2];
      /* r5 is dead as an index here; the target reuses it as the DMA pointer. */
      dmaIndex = (u32)dmaRegs;
      value = dmaRegs[2] & dmaBusyMask;
      if (value != 0)
      {
        do { value = ((volatile u32 *)dmaIndex)[2] & 0x80000000; } while (value != 0);
      }

      address = (u32)thirdTileBlock;
      value = tileOffset + address;
      dmaRegs[0] = value;
      address = 0x06017200;
      value = tileOffset + address;
      dmaRegs[1] = value;
      dmaRegs[2] = dmaControl;
      value = dmaRegs[2];
      dmaIndex = (u32)dmaRegs;
      value = ((volatile u32 *)dmaIndex)[2] & dmaBusyMask;
      if (value != 0)
      {
        do { value = ((volatile u32 *)dmaIndex)[2] & 0x80000000; } while (value != 0);
      }
      /* Keep the tile offset live in r1 so the wait mask uses target r3. */
      ASM_BARRIER_VOLATILE(: "+r"(tileOffset));

      dmaIndex = nextIndex;
    } while (dmaIndex <= 3);
  }

  {
    register u8 *postSource PIN("r6");
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;

    /* agbcc otherwise copies the persistent ip source through r0; the target
     * uses r6 for this single DMA source store. */
    postSource = postLoopTiles;
    dmaRegs[0] = (u32) postSource;
    dmaRegs[1] = (u32) ((void *) 0x0600C000);
    dmaRegs[2] = (u32) 0x80000400;
    dmaRegs[2];
  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    while (dmaRegs[2] & 0x80000000)
    {
      ;
    }

  }
  ;
  {
    register u8 *lateTiles PIN("r0");
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;

    /* r0 is consumed immediately by this transfer, so agbcc cannot hoist the
     * address into the otherwise-free callee-saved r7 across the DMA loop. */
    lateTiles = sRouletteExtraObjTiles;
    dmaRegs[0] = (u32) lateTiles;
    dmaRegs[1] = (u32) ((void *) 0x0600D000);
    dmaRegs[2] = (u32) 0x80000400;
    dmaRegs[2];
  }
  ;
  {
    volatile u32 *dmaRegs;

    dmaRegs = (volatile u32 *) 0x040000D4;
    while (dmaRegs[2] & 0x80000000)
    {
      ;
    }

  }
  ;
  {
    register int zero PIN("r4");
    register u16 *roundOffset PIN("r6");
    register u8 *rouletteState PIN("r0");
    volatile u16 *scroll;
    *((volatile u16 *) 0x04000008) = 0x1800;
    *((volatile u16 *) 0x0400000A) = 0x1A01;
    scroll = (volatile u16 *) 0x04000012;
    zero = 0;
    *scroll = zero;
    scroll--;
    *scroll = zero;
    scroll += 3;
    *scroll = zero;
    scroll--;
    *scroll = zero;
    /* The target loads this first global into r6 only after the display
     * scroll registers are cleared; binding it here prevents early hoisting. */
    roundOffset = &gMinigameScore;
    *roundOffset = zero;
    rouletteState = &gMinigameNewHighScore;
    *rouletteState = zero;
    DrawRouletteScoreText();
    gRouletteAngularSpeed = 70;
    *((volatile u16 *) 0x04000000) = 0x1300;
    InitMinigameScoreDisplay();
    *((volatile u16 *) 0x04000054) = zero;
    gRouletteMoveProgress = zero;
    gRouletteAngularAcceleration = zero;
    gRouletteValueSequenceIndex = zero;
    gRouletteSelectedItemIndex = zero;
    gRouletteStopRequested = zero;
    gRouletteValueCount = 3;
    {
      int activeZero;
      register int tileZero PIN("r4");
      u16 angle;
      struct RouletteItem *item;
      int x;
      int y;
      register int remaining PIN("r5");

      activeZero = 0;
      /* agbcc proves the cached r4 is already zero and removes this target write. */
      ASM_BARRIER_VOLATILE(: "=r"(tileZero));
      tileZero = 0;
      angle = 7168;

      item = gRouletteItems;
      x = 1000;

      y = 88;

      remaining = 2;
      do
      {
        item->x = x;
        item->y = y;
        item->angle = angle;
        item->scale = tileZero;
        item->active = activeZero;
        item++;
        remaining--;
      } while (remaining >= 0);
    }

    {
      u16 *rouletteCounter;
      struct RouletteSmallState *topLabel;
      struct RouletteSmallState *middleLabel;
      struct RouletteSmallState *bottomLabel;
      register struct RouletteMainState *mainState;

      rouletteCounter = &gRouletteTickPeriod;
      topLabel = &gRouletteTopResult;
      middleLabel = &gRouletteMiddleResult;
      bottomLabel = &gRouletteBottomResult;

      /* agbcc cannot assign these cached addresses to the required high
       * registers from C alone. Each boundary is one move; all loads and all
       * state initialization remain compiler-generated C. */
      {
        void *sideAnimAddress;
        sideAnimAddress = &gRouletteSideAnim;
        postLoopTiles = (u8 *)sideAnimAddress;
      }
      {
        void *promptAnimAddress;
        promptAnimAddress = &gRoulettePromptAnim;
        firstTileBlock = promptAnimAddress;
      }
      {
        void *centerAnimAddress;
        centerAnimAddress = &gMinigameUiAnimation;
        thirdTileBlock = centerAnimAddress;
      }
      mainState = &gMinigameCameraState;
      {
        void *blendRegisterAddress;
        blendRegisterAddress = &gRouletteBlendEva;
        secondTileBlock = blendRegisterAddress;
      }
      for (i = 0; i < 3; i++)
      {
        GP_SET(gRouletteItemDrawOrder[i], &gRouletteItems[i]);
      }

      *rouletteCounter = 48;
      topLabel->x = 122;
      topLabel->y = 88;
      topLabel->frame = 0;
      middleLabel->x = 122;
      middleLabel->y = 106;
      middleLabel->frame = 0;
      bottomLabel->x = 122;
      bottomLabel->y = 116;
      bottomLabel->frame = 0;
      {
        u16 *resultOffset;
        resultOffset = &gRouletteFrameCounter;
        *resultOffset = 0;
      }
      {
        register u16 *clearPtr PIN("r6");
        clearPtr = (u16 *)postLoopTiles;
        clearPtr[0] = 0;
        clearPtr[1] = 0;
      }
      {
        register u16 *clearPtr PIN("r0");
        clearPtr = (u16 *)firstTileBlock;
        clearPtr[0] = 0;
        clearPtr[1] = 0;
      }
      {
        register u16 *clearPtr PIN("r1");
        clearPtr = (u16 *)thirdTileBlock;
        clearPtr[0] = 0;
        clearPtr[1] = 0;
      }
      {
        register u8 *effectFlag PIN("r6");
        effectFlag = &gMinigameMedalHighlight;
        *effectFlag = 0;
      }
      mainState->x = 120;
      mainState->y = 64;
      mainState->scale = 0x200;
      mainState->active = 1;
      gMinigameState = 0;
      gMinigameSubstate = 0;
      {
        register u8 *resultState PIN("r6");
        resultState = &gMinigameSequenceState;
        *resultState = 0;
      }
      {
        u16 *resultTimer;
        resultTimer = &gMinigameSequenceTimer;
        *resultTimer = 0;
      }
      {
        u8 *fadeLevel;
        fadeLevel = secondTileBlock;
        *fadeLevel = 0;
      }
      gRouletteBlendEvb = 16;
      *((volatile u16 *) 0x04000050) = 0x240;
      {
        volatile u16 *alphaRegister;
        register s8 *fadePtr PIN("r6");
        s32 fadeValue;

        alphaRegister = (volatile u16 *)0x04000052;
        fadePtr = (s8 *)secondTileBlock;
        ASM_BARRIER(: "+r"(fadePtr));
        fadeValue = 0;
        fadeValue = *(s8 *)((u8 *)fadePtr + fadeValue);
        *alphaRegister = (fadeValue << 8) | 16;
      }
    }
    m4aSongNumStartOrChange(708);
    ShuffleRouletteOamTable();
    ShuffleRouletteValuePool();
    {
      int randomIndex;
      randomIndex = 0;
      do
      {
        s32 randomValue;
        u8 *winning;

        randomValue = MinigameRandom();
        winning = gRouletteWinningValues;
        winning += randomIndex;
        *winning = __modsi3(randomValue, gRouletteValueCount);
        randomIndex++;
      } while (randomIndex <= 2);
    }

  }
}

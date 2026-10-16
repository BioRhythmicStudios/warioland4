void func_804A508(void)
{
  register u32 yDelta asm("r8");
  u32 xDelta;
  u32 frame;
  register struct PrimarySpriteData *current asm("r9");
  register struct PrimarySpriteData *spriteBase asm("r2");
  s32 i;
  u32 mode;
  u32 roomSlot;
  u32 angle;
  const u8 *lowAngleTable;
  s32 cosine;
  s32 sine;
  u32 flagValue;
  yDelta = 0;
  xDelta = 0;
  spriteBase = &gCurrentSprite;
  {
    u32 frameValue;
    register u32 nextFrame asm("r1");
    {
      register u8 *frameLoad asm("r1");
      frameLoad = &gUnk_3000A4B;
      frameValue = *frameLoad;
    }
    nextFrame = frameValue + 1;
    {
      register u8 *frameStore asm("r3");
      frameStore = &gUnk_3000A4B;
      *frameStore = nextFrame;
    }
    frameValue <<= 24;
    frame = frameValue >> 24;
  }
  {
    register u8 *modePtr asm("r4");
    modePtr = &spriteBase->work2;
    mode = *modePtr;
  }
  roomSlot = spriteBase->roomSlot;
  current = spriteBase;
  for (i = 0; i < 24; i++)
  {
    if (gSpriteData[i].roomSlot != roomSlot)
      continue;
    if (gSpriteData[i].globalID != PSPRITE_AC)
      continue;
    if (!(gSpriteData[i].status & SPRITE_STATUS_EXISTS))
      continue;
    if (mode != 0)
    {
      if (frame > 59)
      {
        yDelta = (u16) (sUnk_83D00E8[frame - 60] * 4);
        xDelta = (u16) (xDelta + sUnk_83D0070[frame - 60] * 4);
      }
      else
      {
        yDelta = (u16) (yDelta + sUnk_83D00E8[frame] * 4);
        xDelta = (u16) (xDelta + sUnk_83CFFF8[frame] * 4);
      }
      {
        register u8 *workPointer asm("r4");
        u8 *wp;
        wp = &gSpriteData[i].work1;
        workPointer = wp;
        if (*workPointer != 0)
        {
          gWarioData.yPosition += yDelta;
          gWarioData.xPosition += xDelta;
          goto moved;
        }
      }
      if (frame == 59)
      {
        gUnk_3000A4B = 48;
        gUnk_3000A4D = 0;
      }
      else if (frame == 119)
      {
        gUnk_3000A4D = 0;
      }
    }
    else
    {
      if (frame > 47)
      {
        yDelta = (u16) (sUnk_83CFF98[frame - 48] * 4);
        xDelta = (u16) (xDelta + sUnk_83CFF38[frame - 48] * 4);
      }
      else
      {
        yDelta = (u16) (yDelta + sUnk_83CFF98[frame] * 4);
        xDelta = (u16) (xDelta + sUnk_83CFED8[frame] * 4);
      }
      {
        register u8 *workPointer asm("r3");
        u8 *wp;
        wp = &gSpriteData[i].work1;
        workPointer = wp;
        if (*workPointer != 0)
        {
          gWarioData.yPosition += yDelta;
          gWarioData.xPosition += xDelta;
          if (frame == 47)
          {
            gUnk_3000A4B = 60;
            gUnk_3000A4D = 1;
          }
          else if (frame == 95)
          {
            gUnk_3000A4D = 1;
          }
        }
      }
    }
    moved:
    gSpriteData[i].yPosition += yDelta;
    gSpriteData[i].xPosition += xDelta;
    break;
  }
  flagValue = mode;
  if (flagValue != 0)
  {
    if (frame > 59)
    {
      const s16 *table;
      int motionIndex;
      int byteOffset;
      int zero;
      s32 delta;
      register u32 position asm("r4");
      register struct PrimarySpriteData *storeSprite asm("r3");
      table = sUnk_83CFDE8;
      motionIndex = frame - 60;
      byteOffset = motionIndex << 1;
      table = (const s16 *) (byteOffset + ((s32) table));
      zero = 0;
      delta = table[zero] * 4;
      position = (u32) current;
      position = ((struct PrimarySpriteData *)position)->yPosition;
      delta += position;
      storeSprite = current;
      storeSprite->yPosition = delta;
      {
        const s16 *xPointer;
        int xZero;
        u32 xPosition;
        table = sUnk_83CFD70;
        xPointer = (const s16 *) (byteOffset + ((s32) table));
        xZero = 0;
        delta = 4;
        delta = xPointer[xZero] * delta;
        xPosition = storeSprite->xPosition;
        delta += xPosition;
        storeSprite->xPosition = delta;
      }
      angle = sUnk_83CFE9C[motionIndex];
      if (frame == 119)
      {
        struct PrimarySpriteData *workSprite;
        workSprite = current;
        workSprite->work0 = 0;
      }
      goto angleReady;
    }
    else
    {
      const s16 *table;
      register int byteOffset asm("r1");
      register int zero asm("r2");
      register s32 delta asm("r0");
      register u32 position asm("r3");
      register struct PrimarySpriteData *storeSprite asm("r4");
      table = sUnk_83CFDE8;
      byteOffset = frame << 1;
      table = (const s16 *) (byteOffset + ((s32) table));
      zero = 0;
      delta = table[zero] * 4;
      position = (u32) current;
      position = ((struct PrimarySpriteData *)position)->yPosition;
      delta += position;
      storeSprite = current;
      storeSprite->yPosition = delta;
      {
        const s16 *xPointer;
        register u16 xPosition asm("r3");
        table = sUnk_83CFCF8;
        xPointer = (const s16 *) (byteOffset + ((s32) table));
        
        zero = 0;
        /* agbcc otherwise selects r3 instead of the target r2 as the signed-load index. */
        delta = xPointer[zero];
        delta <<= 2;
        xPosition = storeSprite->xPosition;
        delta += xPosition;
        storeSprite->xPosition = delta;
      }
      lowAngleTable = sUnk_83CFE60;
      goto loadLowAngle;
    }
  }
  else
    if (frame > 47)
  {
    const s16 *table;
    int motionIndex;
    int byteOffset;
    register int zero asm("r4");
    register s32 delta asm("r0");
    register u32 position asm("r3");
    register struct PrimarySpriteData *storeSprite asm("r4");
    table = sUnk_83CFC38;
    motionIndex = frame - 48;
    byteOffset = motionIndex << 1;
    table = (const s16 *) (byteOffset + ((s32) table));
    zero = 0;
    /* Keep r3 live so the typed C load uses the original r4 index. */
    {
      register u32 r3Live asm("r3");

      
      delta = *table;
      
    }
    delta <<= 2;
    position = (u32) current;
    position = ((struct PrimarySpriteData *)position)->yPosition;
    delta += position;
    storeSprite = current;
    storeSprite->yPosition = delta;
    {
      const s16 *xPointer;
      int xZero;
      u32 xPosition;
      struct PrimarySpriteData *xStore;
      table = sUnk_83CFBD8;
      xPointer = (const s16 *) (byteOffset + ((s32) table));
      xZero = 0;
      delta = 4;
      delta = xPointer[xZero] * delta;
      xPosition = storeSprite->xPosition;
      delta += xPosition;
      xStore = current;
      xStore->xPosition = delta;
    }
    angle = sUnk_83CFCC8[motionIndex];
    if (frame == 95)
    {
      struct PrimarySpriteData *workSprite;
      workSprite = current;
      workSprite->work0 = 0;
    }
    goto angleReady;
  }
  else
  {
    const s16 *table;
    register int byteOffset;
    register int zero asm("r2");
    register s32 delta ;
    register u32 position asm("r3");
    register struct PrimarySpriteData *storeSprite asm("r4");
    table = sUnk_83CFC38;
    byteOffset = frame << 1;
    table = (const s16 *) (byteOffset + ((s32) table));
    zero = 0;
    /* agbcc otherwise selects r4 instead of the target r2 as the signed-load index. */
    delta = table[zero];
        delta <<= 2;
    position = (u32) current;
    position = ((struct PrimarySpriteData *)position)->yPosition;
    delta += position;
    storeSprite = current;
    storeSprite->yPosition = delta;
    {
      register u32 xPosition;
      register const s16 *xBase;
      xBase = sUnk_83CFB78;
      byteOffset += (s32)xBase;
      delta = *(const s16 *)byteOffset;
      delta <<= 2;
      xPosition = storeSprite->xPosition;
      delta += xPosition;
      storeSprite->xPosition = delta;
    }
    lowAngleTable = sUnk_83CFC98;
  }

  loadLowAngle:
  {
    register u32 loadedAngle;
    lowAngleTable = (const u8 *)((s32)frame - -(s32)lowAngleTable);
    loadedAngle = *lowAngleTable;
    angle = loadedAngle;
  }

  angleReady:
  {
    s32 index;
    const s16 *table;
    s32 zero;
    s32 value;
    index = angle + ((256 / 2) / 2);
    index <<= 1;
    table = sSinCosTable;
    index += (s32) table;
    value = *(s16 *)index;
    cosine = value;
  }
  gCurrentSprite.affinePA = FixedMul(cosine, FixedInverse((s16) (1 * (1 << 8))));
  {
    register s32 index;
    const s16 *table;
    register s32 zero;
    register s32 loadedSine;
    index = angle << 1;
    table = sSinCosTable;
    index += (s32) table;
    zero = 0;
    loadedSine = ((const s16 *)index)[zero];
    sine = loadedSine;
  }
  gCurrentSprite.affinePB = FixedMul(sine, FixedInverse((s16) (1 * (1 << 8))));
  gCurrentSprite.affinePC = FixedMul(-sine, FixedInverse((s16) (1 * (1 << 8))));
  gCurrentSprite.affinePD = FixedMul(cosine, FixedInverse((s16) (1 * (1 << 8))));
}
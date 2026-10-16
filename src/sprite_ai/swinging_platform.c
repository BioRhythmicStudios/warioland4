#include "sprite_ai/swinging_platform.h"

#include "fixed_point.h"
#include "global_data.h"
#include "sprite.h"
#include "sprite_util.h"
#include "wario.h"

void func_804A3FC(void)
{
    s32 scale;
    s32 cosine;
    s32 sine;

    {
        struct PrimarySpriteData *sprite;
        u8 *drawDistance;
        register u8 *hitbox asm("r0");

        sprite = &gCurrentSprite;
        sprite->status |= 0x488;
        sprite->warioCollision = 0;
        sprite->drawDistanceDown = 0;
        drawDistance = &sprite->drawDistanceUp;
        *drawDistance = 64;
        drawDistance++;
        *drawDistance = 64;
        hitbox = &sprite->hitboxExtentUp;
        *hitbox = 4;
        hitbox++;
        *hitbox = 4;
        hitbox++;
        *hitbox = 4;
        hitbox++;
        *(volatile u8 *)hitbox = 4;
        sprite->currentAnimationFrame = 1;
        sprite->animationTimer = 0;
        sprite->pose = SPOSE_IDLE;
        sprite->work0 = 0;
        sprite->work2 = 0;

        if (sprite->globalID == PSPRITE_SWINGING_PLATFORM) {
            sprite->pOamData = sSwingingPlatformOam_83CFB58;
            SpriteSpawnAsChild(PSPRITE_AC, sprite->roomSlot, sprite->gfxSlot,
                sprite->yPosition + 256, sprite->xPosition + 128);
        } else {
            sprite->pOamData = sSwingingPlatformOam_83CFB48;
            SpriteSpawnAsChild(PSPRITE_AD, sprite->roomSlot, sprite->gfxSlot,
                sprite->yPosition + 192, sprite->xPosition + 128);
        }
    }

    cosine = COS(0);
    scale = Q_8_8(1);
    gCurrentSprite.affinePA = FixedMul(cosine, FixedInverse(scale));
    sine = SIN(0);
    gCurrentSprite.affinePB = FixedMul(sine, FixedInverse(scale));
    gCurrentSprite.affinePC = FixedMul(-sine, FixedInverse(scale));
    gCurrentSprite.affinePD = FixedMul(cosine, FixedInverse(scale));
}

void func_804A508(void)
{
  register u32 yDelta asm("r8");
  u32 xDelta;
  u32 frame;
  register struct PrimarySpriteData *current asm("r9");
  register struct PrimarySpriteData *sprite asm("r2");
  struct PrimarySpriteData *sprites;
  struct WarioData *wario;
  int offset60;
  int frameOffset;
  int offset48;
  struct
  {
    u32 mode;
    u32 roomSlot;
    const s16 *mode1HighY;
    const s16 *mode1LowY;
    const s16 *mode0HighY;
    const s16 *mode0LowY;
    u8 *childWork1;
  } locals;
  u32 angle;
  const u8 *lowAngleTable;
  s32 cosine;
  s32 sine;
  u32 flagValue;
  yDelta = 0;
  xDelta = 0;
  sprite = &gCurrentSprite;
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
      u8 *frameStore;
      frameStore = &gUnk_3000A4B;
      *frameStore = nextFrame;
    }
    frameValue <<= 24;
    frame = frameValue >> 24;
  }
  {
    register u8 *modePtr asm("r4");
    modePtr = &sprite->work2;
    locals.mode = *modePtr;
  }
  {
    register u32 roomSlot asm("r1");
    roomSlot = sprite->roomSlot;
    locals.roomSlot = roomSlot;
  }
  current = sprite;
  sprite = gSpriteData;
  sprites = sprite;
  {
    s32 offset;
    const s16 *table;
    table = sUnk_83D00E8;
    offset = frame;
    offset -= 60;
    offset <<= 1;
    offset60 = offset;
    frameOffset = offset60;
    
    locals.mode1HighY = (const s16 *) (frameOffset + ((s32) table));
    frameOffset = frame << 1;
    table = (const s16 *) (frameOffset + ((s32) table));
    locals.mode1LowY = table;
  }
  locals.childWork1 = &sprites->work1;
  wario = &gWarioData;
  {
    s32 offset;
    const s16 *table;
    table = sUnk_83CFF98;
    offset = frame;
    offset -= 48;
    offset48 = offset << 1;
    locals.mode0HighY = (const s16 *) (offset48 + ((s32) table));
    table = (const s16 *) (frameOffset + ((s32) table));
    locals.mode0LowY = table;
  }
  loop:
  {
    u32 childRoomSlot;
    register u32 roomSlot asm("r1");
    childRoomSlot = sprite->roomSlot;
    roomSlot = locals.roomSlot;
    if (childRoomSlot == roomSlot)
    {
    }
    else
    {
      goto next;
    }
  }

  if (sprite->globalID != PSPRITE_AC)
  {
    goto next;
  }
  if (!(sprite->status & SPRITE_STATUS_EXISTS))
  {
    goto next;
  }
  if (locals.mode != 0)
  {
    {
      s32 xVelocity;
      if (frame > 59)
      {
        register const s16 *yPointer asm("r1");
        s32 zero;
        yPointer = *(const s16 *volatile *)&locals.mode1HighY;
        zero = 0;
        
        yDelta = (u16) (yPointer[zero] * 4);
        {
          s32 xZero;
          xZero = 0;
          xVelocity = ((const s16 *) (((const u8 *) sUnk_83D0070) + offset60))[xZero];
        }
      }
      else
      {
        register const s16 *yPointer asm("r1");
        s32 zero;
        yPointer = *(const s16 *volatile *)&locals.mode1LowY;
        zero = 0;
        
        yDelta = (u16) (yDelta + (yPointer[zero] * 4));
        {
          register const s16 *xTable asm("r1");
          register u32 xAddress asm("r0");
          register u32 xZero;
          xTable = sUnk_83CFFF8;
          xAddress = frameOffset;
          xAddress += (u32)xTable;
          xZero = 0;
           xVelocity = ((const s16 *)xAddress)[xZero]; 
        }
      }
      xDelta = (u16) (xDelta + (xVelocity * 4));
    }
    {
      register u8 *workPointer asm("r4");
      register u32 work asm("r1");
      workPointer = locals.childWork1;
      work = *workPointer;
      if (work != 0)
      {
        wario->yPosition += yDelta;
        {
          u32 xPosition;
          xPosition = wario->xPosition;
          xPosition = (xDelta) - (-(xPosition));
          wario->xPosition = xPosition;
        }
        goto applyChildMovement;
      }
      else
        if (frame == 59)
      {
        u8 value;
        register u8 *framePtr asm("r3");
        register u8 *flagPtr asm("r4");
        value = 48;
        framePtr = &gUnk_3000A4B;
        *framePtr = value;
        flagPtr = &gUnk_3000A4D;
        *flagPtr = work;
        goto applyChildMovement;
      }
      else
      {
        if (frame != 119)
        {
          goto applyChildMovement;
        }
        flagValue = work;
        goto storeFlag;
      }
    }
  }
  else
  {
    {
      s32 xVelocity;
      if (frame > 47)
      {
        register const s16 *yPointer asm("r1");
        s32 zero;
        yPointer = *(const s16 *volatile *)&locals.mode0HighY;
        zero = 0;
        
        yDelta = (u16) (yPointer[zero] * 4);
        {
          register const s16 *xTable asm("r1");
          register const s16 *xPointer;

          xTable = sUnk_83CFF38;
          xPointer = (const s16 *)(offset48 + (s32)xTable);
          {
            register u32 xZero;
            xZero = 0;
            xVelocity = xPointer[xZero];
            
          }
        }
      }
      else
      {
        register const s16 *yPointer asm("r4");
        s32 zero;
        yPointer = *(const s16 *volatile *)&locals.mode0LowY;
        zero = 0;
        
        yDelta = (u16) (yDelta + (yPointer[zero] * 4));
        {
          register const s16 *xTable asm("r4");
          register u32 xAddress;
          register u32 xZero;
          xTable = sUnk_83CFED8;
          xAddress = frameOffset + (u32)xTable;
          xZero = 0;
          xVelocity = ((const s16 *)xAddress)[xZero]; 
        }
      }
      xDelta = (u16) (xDelta + (xVelocity * 4));
    }
    {
      register u8 *workPointer asm("r3");
      u32 work;
      workPointer = locals.childWork1;
      work = *workPointer;
      if (work == 0)
      {
        goto applyChildMovement;
      }
      wario->yPosition += yDelta;
      {
        u32 xPosition;
        xPosition = wario->xPosition;
        xPosition = (xDelta) - (-(xPosition));
        wario->xPosition = xPosition;
      }
      if (frame != 47)
      {
        goto checkFrame95;
      }
      {
        u8 value;
        u8 one;
        register u8 *framePtr asm("r4");
        value = 60;
        framePtr = &gUnk_3000A4B;
        *framePtr = value;
        one = 1;
        flagValue = one;
        goto storeFlag;
      }
    }
  }
  storeFlag:
  gUnk_3000A4D = flagValue;

  goto applyChildMovement;
  checkFrame95:
  if (frame == 95)
  {
    register u8 one asm("r4");
    register u8 *flagPtr asm("r3");
    one = 1;
    flagPtr = &gUnk_3000A4D;
    *flagPtr = one;
  }

  applyChildMovement:
  sprite->yPosition += yDelta;

  {
    u32 xPosition;
    xPosition = sprite->xPosition;
    xPosition = (xDelta) - (-(xPosition));
    sprite->xPosition = xPosition;
  }
  goto afterLoop;
  next:
  locals.childWork1 += sizeof(struct PrimarySpriteData);

  sprite++;
  if (((s32) sprite) <= ((s32) (&sprites[24 - 1])))
  {
    goto loop;
  }
  afterLoop:
  flagValue = locals.mode;
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

void func_804A84C(void)
{
  register u32 yDelta asm("r8");
  u32 xDelta;
  u32 frame;
  register struct PrimarySpriteData *current asm("r9");
  register struct PrimarySpriteData *sprite asm("r2");
  struct PrimarySpriteData *sprites;
  struct WarioData *wario;
  int offset60;
  int frameOffset;
  int offset48;
  struct
  {
    u32 mode;
    u32 roomSlot;
    const s16 *mode1HighY;
    const s16 *mode1LowY;
    const s16 *mode0HighY;
    const s16 *mode0LowY;
    u8 *childWork1;
  } locals;
  u32 angle;
  const u8 *lowAngleTable;
  s32 cosine;
  s32 sine;
  u32 flagValue;
  yDelta = 0;
  xDelta = 0;
  sprite = &gCurrentSprite;
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
      u8 *frameStore;
      frameStore = &gUnk_3000A4B;
      *frameStore = nextFrame;
    }
    frameValue <<= 24;
    frame = frameValue >> 24;
  }
  {
    register u8 *modePtr asm("r4");
    modePtr = &sprite->work2;
    locals.mode = *modePtr;
  }
  {
    register u32 roomSlot asm("r1");
    roomSlot = sprite->roomSlot;
    locals.roomSlot = roomSlot;
  }
  current = sprite;
  sprite = gSpriteData;
  sprites = sprite;
  {
    s32 offset;
    const s16 *table;
    table = sUnk_83D0370;
    offset = frame;
    offset -= 60;
    offset <<= 1;
    offset60 = offset;
    frameOffset = offset60;
    
    locals.mode1HighY = (const s16 *) (frameOffset + ((s32) table));
    frameOffset = frame << 1;
    table = (const s16 *) (frameOffset + ((s32) table));
    locals.mode1LowY = table;
  }
  locals.childWork1 = &sprites->work1;
  wario = &gWarioData;
  {
    s32 offset;
    const s16 *table;
    table = sUnk_83D0220;
    offset = frame;
    offset -= 48;
    offset48 = offset << 1;
    locals.mode0HighY = (const s16 *) (offset48 + ((s32) table));
    table = (const s16 *) (frameOffset + ((s32) table));
    locals.mode0LowY = table;
  }
  loop:
  {
    u32 childRoomSlot;
    register u32 roomSlot asm("r1");
    childRoomSlot = sprite->roomSlot;
    roomSlot = locals.roomSlot;
    if (childRoomSlot == roomSlot)
    {
    }
    else
    {
      goto next;
    }
  }

  if (sprite->globalID != PSPRITE_AD)
  {
    goto next;
  }
  if (!(sprite->status & SPRITE_STATUS_EXISTS))
  {
    goto next;
  }
  if (locals.mode != 0)
  {
    {
      s32 xVelocity;
      if (frame > 59)
      {
        register const s16 *yPointer asm("r1");
        s32 zero;
        yPointer = locals.mode1HighY;
        zero = 0;
        
        yDelta = (u16) (yPointer[zero] * 4);
        {
          s32 xZero;
          xZero = 0;
          xVelocity = ((const s16 *) (((const u8 *) sUnk_83D02F8) + offset60))[xZero];
        }
      }
      else
      {
        register const s16 *yPointer asm("r1");
        s32 zero;
        yPointer = locals.mode1LowY;
        zero = 0;
        
        yDelta = (u16) (yDelta + (yPointer[zero] * 4));
        {
          register const s16 *xTable;
          register u32 xAddress asm("r0");
          register u32 xZero;
          xTable = sUnk_83D0280;
          xAddress = frameOffset + (u32)xTable;
          xZero = 0;
           xVelocity = ((const s16 *)xAddress)[xZero]; 
        }
      }
      xDelta = (u16) (xDelta + (xVelocity * 4));
    }
    {
      register u8 *workPointer asm("r4");
      register u32 work asm("r1");
      workPointer = locals.childWork1;
      work = *workPointer;
      if (work != 0)
      {
        wario->yPosition += yDelta;
        {
          u32 xPosition;
          xPosition = wario->xPosition;
          xPosition = (xDelta) - (-(xPosition));
          wario->xPosition = xPosition;
        }
        goto applyChildMovement;
      }
      else
        if (frame == 59)
      {
        u8 value;
        register u8 *framePtr asm("r3");
        register u8 *flagPtr asm("r4");
        value = 48;
        framePtr = &gUnk_3000A4B;
        *framePtr = value;
        flagPtr = &gUnk_3000A4D;
        *flagPtr = work;
        goto applyChildMovement;
      }
      else
      {
        if (frame != 119)
        {
          goto applyChildMovement;
        }
        flagValue = 1;
        goto storeFlag;
      }
    }
  }
  else
  {
    {
      s32 xVelocity;
      if (frame > 47)
      {
        register const s16 *yPointer asm("r1");
        s32 zero;
        yPointer = locals.mode0HighY;
        zero = 0;
        
        yDelta = (u16) (yPointer[zero] * 4);
        {
          const s16 *xTable;
          register const s16 *xPointer;

          xTable = sUnk_83D01C0;
          xPointer = (const s16 *)(offset48 + (s32)xTable);
          {
            register u32 xZero;
            xZero = 0;
            xVelocity = xPointer[xZero];
            
          }
        }
      }
      else
      {
        register const s16 *yPointer asm("r4");
        s32 zero;
        yPointer = locals.mode0LowY;
        zero = 0;
        
        yDelta = (u16) (yDelta + (yPointer[zero] * 4));
        {
          register const s16 *xTable asm("r4");
          register u32 xAddress;
          register u32 xZero;
          xTable = sUnk_83D0160;
          xAddress = frameOffset + (u32)xTable;
          xZero = 0;
          xVelocity = ((const s16 *)xAddress)[xZero]; 
        }
      }
      xDelta = (u16) (xDelta + (xVelocity * 4));
    }
    {
      register u8 *workPointer asm("r3");
      u32 work;
      workPointer = locals.childWork1;
      work = *workPointer;
      if (work == 0)
      {
        goto applyChildMovement;
      }
      wario->yPosition += yDelta;
      {
        u32 xPosition;
        xPosition = wario->xPosition;
        xPosition = (xDelta) - (-(xPosition));
        wario->xPosition = xPosition;
      }
      if (frame != 47)
      {
        goto checkFrame95;
      }
      {
        u8 value;
        u8 one;
        register u8 *framePtr asm("r4");
        value = 60;
        framePtr = &gUnk_3000A4B;
        *framePtr = value;
        one = 1;
        flagValue = one;
        goto storeFlag;
      }
    }
  }
  storeFlag:
  gUnk_3000A4D = flagValue;

  goto applyChildMovement;
  checkFrame95:
  if (frame == 95)
  {
    register u8 one asm("r4");
    register u8 *flagPtr asm("r3");
    one = 1;
    flagPtr = &gUnk_3000A4D;
    *flagPtr = one;
  }

  applyChildMovement:
  sprite->yPosition += yDelta;

  {
    u32 xPosition;
    xPosition = sprite->xPosition;
    xPosition = (xDelta) - (-(xPosition));
    sprite->xPosition = xPosition;
  }
  goto afterLoop;
  next:
  locals.childWork1 += sizeof(struct PrimarySpriteData);

  sprite++;
  if (((s32) sprite) <= ((s32) (&sprites[24 - 1])))
  {
    goto loop;
  }
  afterLoop:
  flagValue = locals.mode;
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
      int byteOffset;
      register int zero asm("r2");
      register short delta asm("r0");
      register u32 position asm("r3");
      struct PrimarySpriteData *storeSprite;
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
        register u32 xPosition asm("r3");
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

void func_804AB90(void)
{
    struct PrimarySpriteData *sprite;
    register u16 status asm("r0");
    register int statusBits asm("r2");
    register int zero asm("r3");
    int zero16;
    u16 oldStatus;

    sprite = &gCurrentSprite;
    oldStatus = sprite->status;
    statusBits = 0x8400;
    status = statusBits;
    zero = 0;
    zero16 = 0;
    status |= oldStatus;
    status &= 0xFFFB;
    sprite->status = status;
    sprite->warioCollision = 29;
    sprite->drawDistanceDown = 8;
    sprite->drawDistanceUp = 8;
    sprite->drawDistanceLeftRight = 16;
    sprite->hitboxExtentUp = 16;
    sprite->hitboxExtentDown = 48;
    sprite->hitboxExtentLeft = 60;
    sprite->hitboxExtentRight = 56;
    {
        const struct AnimationFrame *oam;
        struct PrimarySpriteData *storeSprite;

        oam = sSwingingPlatformOam_83CFB68;
        storeSprite = sprite;
        storeSprite->pOamData = oam;
        storeSprite->currentAnimationFrame = zero;
        storeSprite->animationTimer = zero16;
        storeSprite->pose = SPOSE_IDLE;
    }
    sprite->work1 = zero;
}

void SpriteSwingingPlatform(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == SPOSE_INIT) {
        func_804A3FC();
    } else {
        func_804A508();
    }
}

void SpriteSwingingPlatform2(void)
{
    gCurrentSprite.disableWarioCollisionTimer = 1;
    if (gCurrentSprite.pose == SPOSE_INIT) {
        func_804A3FC();
    } else {
        func_804A84C();
    }
}

void SpriteUnknownAC(void)
{
    if (!SpriteUtilDespawnChildWithParent(PSPRITE_SWINGING_PLATFORM)) {
        if (gCurrentSprite.pose == SPOSE_INIT) {
            func_804AB90();
        }
        func_8026838();
    }
}

void SpriteUnknownAD(void)
{
    if (!SpriteUtilDespawnChildWithParent(PSPRITE_SWINGING_PLATFORM_2)) {
        if (gCurrentSprite.pose == SPOSE_INIT) {
            func_804AB90();
        }
        func_8026838();
    }
}

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
        register u8 *hitbox PIN("r0");

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
            GP_SET(sprite->pOamData, sSwingingPlatformOam_83CFB58);
            SpriteSpawnAsChild(PSPRITE_AC, sprite->roomSlot, sprite->gfxSlot,
                sprite->yPosition + 256, sprite->xPosition + 128);
        } else {
            GP_SET(sprite->pOamData, sSwingingPlatformOam_83CFB48);
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
  register u32 yDelta PIN("r8");
  u32 xDelta;
  u32 frame;
  register struct PrimarySpriteData *current PIN("r9");
  register struct PrimarySpriteData *spriteBase PIN("r2");
  s32 i;
  s32 xLoad;
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
    register u32 nextFrame PIN("r1");
    {
      register u8 *frameLoad PIN("r1");
      frameLoad = &gUnk_3000A4B;
      frameValue = *frameLoad;
    }
    nextFrame = frameValue + 1;
    {
      register u8 *frameStore PIN("r3");
      frameStore = &gUnk_3000A4B;
      *frameStore = nextFrame;
    }
    frameValue <<= 24;
    frame = frameValue >> 24;
  }
  {
    register u8 *modePtr PIN("r4");
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
        do { xLoad = sUnk_83CFFF8[frame]; } while (0);
        xDelta = (u16) (xDelta + xLoad * 4);
      }
      if (gSpriteData[i].work1 != 0)
      {
        gWarioData.yPosition += yDelta;
        { u32 xPosition; xPosition = gWarioData.xPosition; xPosition = (xDelta) - (-(xPosition)); gWarioData.xPosition = xPosition; }
      }
      else if (frame == 59)
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
        do { do { xLoad = sUnk_83CFF38[frame - 48]; } while (0); } while (0);
        xDelta = (u16) (xDelta + xLoad * 4);
      }
      else
      {
        yDelta = (u16) (yDelta + sUnk_83CFF98[frame] * 4);
        do { xLoad = sUnk_83CFED8[frame]; } while (0);
        xDelta = (u16) (xDelta + xLoad * 4);
      }
      if (gSpriteData[i].work1 != 0)
      {
        gWarioData.yPosition += yDelta;
        { u32 xPosition; xPosition = gWarioData.xPosition; xPosition = (xDelta) - (-(xPosition)); gWarioData.xPosition = xPosition; }
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
    gSpriteData[i].yPosition += yDelta;
    { u32 xPosition; xPosition = gSpriteData[i].xPosition; xPosition = (xDelta) - (-(xPosition)); gSpriteData[i].xPosition = xPosition; }
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
      register u32 position PIN("r4");
      register struct PrimarySpriteData *storeSprite PIN("r3");
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
      register int byteOffset PIN("r1");
      register int zero PIN("r2");
      register s32 delta PIN("r0");
      register u32 position PIN("r3");
      register struct PrimarySpriteData *storeSprite PIN("r4");
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
        register u16 xPosition PIN("r3");
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
    register int zero PIN("r4");
    register s32 delta PIN("r0");
    register u32 position PIN("r3");
    register struct PrimarySpriteData *storeSprite PIN("r4");
    table = sUnk_83CFC38;
    motionIndex = frame - 48;
    byteOffset = motionIndex << 1;
    table = (const s16 *) (byteOffset + ((s32) table));
    zero = 0;
    /* Keep r3 live so the typed C load uses the original r4 index. */
    {
      register u32 r3Live PIN("r3");

      
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
    register int zero PIN("r2");
    register s32 delta ;
    register u32 position PIN("r3");
    register struct PrimarySpriteData *storeSprite PIN("r4");
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
  register u32 yDelta PIN("r8");
  u32 xDelta;
  u32 frame;
  register struct PrimarySpriteData *current PIN("r9");
  register struct PrimarySpriteData *spriteBase PIN("r2");
  s32 i;
  s32 xLoad;
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
    register u32 nextFrame PIN("r1");
    {
      register u8 *frameLoad PIN("r1");
      frameLoad = &gUnk_3000A4B;
      frameValue = *frameLoad;
    }
    nextFrame = frameValue + 1;
    {
      register u8 *frameStore PIN("r3");
      frameStore = &gUnk_3000A4B;
      *frameStore = nextFrame;
    }
    frameValue <<= 24;
    frame = frameValue >> 24;
  }
  {
    register u8 *modePtr PIN("r4");
    modePtr = &spriteBase->work2;
    mode = *modePtr;
  }
  roomSlot = spriteBase->roomSlot;
  current = spriteBase;
  for (i = 0; i < 24; i++)
  {
    if (gSpriteData[i].roomSlot != roomSlot)
      continue;
    if (gSpriteData[i].globalID != PSPRITE_AD)
      continue;
    if (!(gSpriteData[i].status & SPRITE_STATUS_EXISTS))
      continue;
    if (mode != 0)
    {
      if (frame > 59)
      {
        yDelta = (u16) (sUnk_83D0370[frame - 60] * 4);
        xDelta = (u16) (xDelta + sUnk_83D02F8[frame - 60] * 4);
      }
      else
      {
        yDelta = (u16) (yDelta + sUnk_83D0370[frame] * 4);
        do { xLoad = sUnk_83D0280[frame]; } while (0);
        xDelta = (u16) (xDelta + xLoad * 4);
      }
      if (gSpriteData[i].work1 != 0)
      {
        gWarioData.yPosition += yDelta;
        { u32 xPosition; xPosition = gWarioData.xPosition; xPosition = (xDelta) - (-(xPosition)); gWarioData.xPosition = xPosition; }
      }
      else if (frame == 59)
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
        yDelta = (u16) (sUnk_83D0220[frame - 48] * 4);
        do { do { xLoad = sUnk_83D01C0[frame - 48]; } while (0); } while (0);
        xDelta = (u16) (xDelta + xLoad * 4);
      }
      else
      {
        yDelta = (u16) (yDelta + sUnk_83D0220[frame] * 4);
        do { xLoad = sUnk_83D0160[frame]; } while (0);
        xDelta = (u16) (xDelta + xLoad * 4);
      }
      if (gSpriteData[i].work1 != 0)
      {
        gWarioData.yPosition += yDelta;
        { u32 xPosition; xPosition = gWarioData.xPosition; xPosition = (xDelta) - (-(xPosition)); gWarioData.xPosition = xPosition; }
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
    gSpriteData[i].yPosition += yDelta;
    { u32 xPosition; xPosition = gSpriteData[i].xPosition; xPosition = (xDelta) - (-(xPosition)); gSpriteData[i].xPosition = xPosition; }
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
      register u32 position PIN("r4");
      register struct PrimarySpriteData *storeSprite PIN("r3");
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
      register int byteOffset PIN("r1");
      register int zero PIN("r2");
      register s32 delta PIN("r0");
      register u32 position PIN("r3");
      register struct PrimarySpriteData *storeSprite PIN("r4");
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
        register u16 xPosition PIN("r3");
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
    register int zero PIN("r4");
    register s32 delta PIN("r0");
    register u32 position PIN("r3");
    register struct PrimarySpriteData *storeSprite PIN("r4");
    table = sUnk_83CFC38;
    motionIndex = frame - 48;
    byteOffset = motionIndex << 1;
    table = (const s16 *) (byteOffset + ((s32) table));
    zero = 0;
    /* Keep r3 live so the typed C load uses the original r4 index. */
    {
      register u32 r3Live PIN("r3");

      
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
    register int zero PIN("r2");
    register s32 delta ;
    register u32 position PIN("r3");
    register struct PrimarySpriteData *storeSprite PIN("r4");
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
    register u16 status PIN("r0");
    register int statusBits PIN("r2");
    register int zero PIN("r3");
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
        GP_SET(storeSprite->pOamData, oam);
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

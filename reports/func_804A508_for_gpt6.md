# func_804A508: report for GPT-6 (session 12)

**Status:** unmatched. The decomp.wtf saved best is still **425 (97.9%)**; nothing was submitted this session (11 of 15 capped local compiles).
Unit: `src/sprite_ai/swinging_platform.c`. The repo source is unchanged; it equals the saved best, with a local diff of 114 lines vs `expected/`.

## Key finding: the original is a natural loop optimized by loop.c
The current best hand-writes a `locals` struct with "hoisted" pointers. The target shows these are compiler products instead:
- **sp+24** starts at `gSpriteData + 0x28` and gets `+0x2c` each iteration: `ldr r0,[sp,#24]; adds r0,#44; str r0,[sp,#24]`. That is a **strength-reduced giv for `&sprite->work1`**. work1 sits at offset 40, which doesn't fit Thumb `ldrb`'s 0–31 immediate, so it is the only field giv that gets reduced. The other fields are accessed as `[r2,#imm]`.
- **sp+8 / sp+12 / sp+16 / sp+20** are loop-invariant addresses (`&sUnk_83D00E8[frame-60]`, `&sUnk_83D00E8[frame]`, `&sUnk_83CFF98[frame-48]`, `&sUnk_83CFF98[frame]`), hoisted and then **spilled**. sp+0 and sp+4 are the spilled `mode` (work2) and `roomSlot`.
- **r10** is the hoisted `(frame-60)*2` used with `sUnk_83D0070`. **r3** is the hoisted `frame*2`, and **r4** is `(frame-48)*2`.
- The end test is `movs r0,#0xfd; lsls r0,#2; add r0,r12; cmp r2,r0; bgt exit`, where r12 = gSpriteData. That looks like biv elimination. There is **no loop entry test**.
- Each matching sprite is processed and then the loop exits (`b afterLoop`). That's why offset48 in r4 can be clobbered on the mode-1 path.

## Experiments (local changed lines; base 114)
| ID | Change | Lines | Note |
|---|---|---|---|
| L1 | `frameOffset = offset60; frameOffset += (s32)table;` | 112 | gets `adds r3,…` but CSE reuses r0 instead of re-reading r10 |
| L2 | compute directly into `offset60` | 218 | offset60 drops to a low reg |
| L3 | `locals` struct -> 7 plain locals (spilled pseudos) | 188 | **loop rotation becomes exactly the target's** (`ldr r1,[sp,#12]; movs r3,#0; ldrsh r0,[r1,r3]`, then r4, ...). Global alloc: childWork1 (prio 1099) gets r7 over xDelta (702) and offset60 (183); offset60 spilled. |
| L5 | L3 + dead `&localsChildWork1` | 188 | optimized away |
| L6 | L3 + `offset60 asm("r10")` | 198 | xDelta spilled instead |
| L7 | L6 with goto-loop -> do/while | 298 | |
| N1 | **natural rewrite** (below), `for (sprite = gSpriteData; sprite <= &gSpriteData[23]; sprite++)` | 340 | **reproduces the hoisting and stack layout:** sp+8/12/16/20, sp+24 = `ip+40`, `mov sl,r0` for offset60. Wrong: loop entry test `cmp ip,r4; bls`, extra giv `ip = sprite+10` (fields via `[ip,#14]`), xDelta in r3 and frame*2 in r7 (swapped) |
| N2 | N1 as do/while | 336 | loop.c does nothing (no hoisting) |
| N3 | `for (i = 0; i < 24; i++) { sprite = &gSpriteData[i]; …` | 275 | no entry test, hoisting OK, but `i` kept (`muls` by 44) |
| N4 | `for (i = 0, sprite = gSpriteData; i < 24; i++, sprite++)` | 330 | i spilled, +10 giv back |

## Open questions
1. Which loop form gives a pointer biv **and** no entry test? Candidates:
   - the `i` form with every access written as `gSpriteData[i].field`, so all givs share base and offsets and i is eliminated;
   - a `while` loop with a bound loop.c can fold;
   - a loop counting down.
2. Where does the extra `sprite+10` giv in N1/N4 come from? It's probably the `xPosition`/`yPosition` read-modify-write fields, so try a different expression shape for `sprite->xPosition += xDelta` (target: `ldrh r0,[r2,#10]; adds r0,r7,r0; strh r0,[r2,#10]`, xDelta first).
3. Once the loop shape is right, check whether global alloc gives xDelta r7 and frame*2 r3 as in the target.

func_804A84C (same structure, sUnk_83D02xx/83D03xx tables) should follow whatever works here.

## N1 source (natural rewrite; after-loop part is the saved best's)
```c
void func_804A508(void)
{
  register u32 yDelta asm("r8");
  u32 xDelta;
  u32 frame;
  register struct PrimarySpriteData *current asm("r9");
  register struct PrimarySpriteData *spriteBase asm("r2");
  struct PrimarySpriteData *sprite;
  struct WarioData *wario;
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
      u8 *frameStore;
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
  wario = &gWarioData;
  for (sprite = gSpriteData; sprite <= &gSpriteData[24 - 1]; sprite++)
  {
    if (sprite->roomSlot != roomSlot)
      continue;
    if (sprite->globalID != PSPRITE_AC)
      continue;
    if (!(sprite->status & SPRITE_STATUS_EXISTS))
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
      if (sprite->work1 != 0)
      {
        wario->yPosition += yDelta;
        wario->xPosition += xDelta;
      }
      else if (frame == 59)
      {
        gUnk_3000A4B = 48;
        gUnk_3000A4D = sprite->work1;
      }
      else if (frame == 119)
      {
        gUnk_3000A4D = sprite->work1;
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
      if (sprite->work1 != 0)
      {
        wario->yPosition += yDelta;
        wario->xPosition += xDelta;
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
    sprite->yPosition += yDelta;
    sprite->xPosition += xDelta;
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
}```

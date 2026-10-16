# func_804A508 handover   2026-10-16 Codex

## Verified status

**Unmatched: best improved from 97.9% / score 425 to 99.2% / score 165.**
Final compile_and_diff repeated the best implementation and confirmed score 165:
17 operand differences, 1 missing instruction, 1 different instruction, 1 extra item.
get_function confirms saved best score 165 and the current source equals it.

15 targeted candidates (one rejected build), plus initial baseline, an intermediate restore, and final revalidation: 18 compile_and_diff calls. No exact-match claim and no commit was made.

The pre-session 97.9% current source was identical to the stored score-425 source. It was not a separate unpublished improvement.

## Repository safety

The file contained pre-existing uncommitted changes in sibling func_804A84C. Those were preserved. A normalized-text comparison against a session-start full-file snapshot confirms the entire file outside func_804A508 is unchanged, including the sibling. No flags, headers, compiler source or other functions were edited, and no inline assembly was introduced. Register-variable annotations already used by this project remain.

Because the MCP workflow automatically commits an exact match, a future matching run must check commit scope carefully: the file has pre-existing sibling changes that must not be committed as part of this function. This session never reached the automatic-commit path. Do not stage this entire file blindly.

Earlier work is preserved at reports/func_804A508_for_gpt6.md (Claude session 12). Read both reports.

## Improvements retained in best O

Changes are cumulative from the session-start best:

1. In the post-loop mode1/low-frame branch, change register short delta asm("r0") to register s32 delta asm("r0"). This restores the target signed X-table load instead of ldrh and removes propagated layout differences. All final position stores are still u16; the widened intermediates preserve the resulting modular positions.
2. In that same branch, use register int byteOffset asm("r1") and register struct PrimarySpriteData *storeSprite asm("r4"). These fix the offset and store-pointer registers.
3. Read each of the four saved Y-table pointer fields through a volatile-qualified pointer lvalue:
   yPointer = *(const s16 *volatile *)&locals.mode1HighY;
   and likewise mode1LowY, mode0HighY, mode0LowY.
   These are reads of private stack locals, not volatile reads of ROM table entries. They prevent folding the saved-pointer loads into an arbitrary address pseudo and retain the hard r1/r4 destinations. All four Y-table address registers now match, along with an affected X-load scratch register.
4. In the post-loop mode1/low-frame X block, change register u32 xPosition asm("r3") to register u16 xPosition asm("r3"). The field already is u16. This fixes its load/add register without extra extension instructions and improves another scratch choice.
5. Pin the short-lived X table pointers in the mode1/low and mode0/high paths to r1. Spell mode1/low X address as xAddress = frameOffset; xAddress += (u32)xTable. This fixes literal-load registers, though address output registers still differ.
6. Make frameOffset an ordinary int instead of register int asm("r3"). This improves the tool score further. Its long-lived frame*2 value still receives r3 naturally; some scratch choices differ.

The source has intentionally retained existing structure, casts and source ordering. Avoid unsolicited cleanup.

## Remaining differences classified

All remaining real mismatches are register allocation/reload inheritance/address-expression differences. The stack size and offsets match (0x1c frame, mode/room at 0/4, Y pointers at 8/12/16/20, childWork1 at 24). There is no observed behavioral control-flow discrepancy or missing algorithm.

| Region | Target vs best | Classification |
| --- | --- | --- |
| Initial mode1HighY setup | mov r3,sl; adds r3,r3,r1; str r3,[sp,#8] vs adds r0,r0,r1; str r0,[sp,#8] | Missing separate copy; temporary allocation and reload inheritance |
| First literal reach and child-filter branches | One LDR PC displacement +4; branch destination addresses -2 | Consequences of missing copy, not different branch logic |
| First literal pool boundary | Best adds a halfword zero alignment slot | Padding compensates for missing 2-byte instruction; later offsets realign |
| Mode1/low X at target 0x1d8 vicinity | add result/address in r1 vs target r0; zero scratch r3 matches | Address temporary allocation |
| Mode0/high X near target 0x232 | add result/address r1 vs r0; zero scratch r4 vs r3 | Address temporary and signed-load scratch allocation |
| Mode0/low X near target 0x250 | add/address r4 vs r0; zero scratch r3 vs r1 | Address temporary and scratch allocation |
| Loop backedge | Target 0x16e vs best 0x16c | Same loop topology; preceding instruction-length difference |
| Post-loop mode1/high first Y load | zero scratch r4 vs target r3 | Reload/scratch allocation |
| Post-loop mode1/low second X load | zero scratch r3 vs target r2 | Reload/scratch allocation |
| Post-loop mode0/low first Y load | zero scratch r4 vs target r2 | Reload/scratch allocation |

Affine tail and table relocations are otherwise unchanged. Full object diff below is exhaustive for disassembled instruction/operand differences; addresses are unit-relative, using ip/sl aliases for r12/r10.

## Compiler findings

### First visible mismatch is created by reload allocation

In best O's combine dump:

    insn 97: pseudo29 = (frame - 60) << 1
    insn 107: pseudo52 = pseudo29 + tableBase50
    insn 109: [framePointer + 8] = pseudo52

Allocation puts pseudo29 in r10, but its shift needs low-register output r0. The greg dump has the shift output in r0, an output copy to r10, then insn107 adds the still-available r0 to tableBase in r1 and stores r0. Thus simply adding an ordinary short-lived C temporary cannot force a fresh high-register reload. In candidate B, even explicitly pinning offset60 to r10 and using compound addition into hard r3 yields a three-register add r3,r0,r1: reload inherits r0, avoiding the target mov r3,r10. This is not just CSE deleting a textual assignment.

Next investigation should examine output-reload inheritance and spill-register selection around this exact insn sequence. The target has a reason to reload r10 into r3 that the hand-hoisted struct implementation currently lacks.

### Saved Y-pointer loads

With ordinary struct field reads, combine deletes the hard yPointer assignment and emits a new pseudo83 = [stack+8], followed by a signed load through pseudo83. Local allocation coalesces its address with the loaded result in r0. This produces ldr r0,[sp,#8]; movs r1,#0; ldrsh r0,[r0,r1].

Qualifying the saved pointer read as volatile preserves the separate assignment, giving the exact target ldr r1,[sp,#8]; movs r3,#0; ldrsh r0,[r1,r3]. Equivalent fixes apply to all four Y pointers. This confirmed the address-lifetime issue directly.

### Natural-loop hypothesis revisited

Claude's prior natural-loop evidence remains useful: the table-address stack slots and childWork1 induction pointer resemble loop.c-generated invariants/givs. One new signed-pointer-bound version of the natural for-loop was tested (D). It still emitted a loop entry check, an extra sprite+10 induction pointer, and broad allocation differences. Changing the bound to signed pointer arithmetic alone does not solve it. No further broad natural-loop variants were attempted.

### FlatWarioFloating lesson

No arbitrary block duplication was added here. Unlike FlatWarioFloating, the verified remaining diff does not show the wrong conditional orientation or basic-block layout. The obstacle is register/address selection and reload reuse. Duplication would need a concrete reload-order justification before further testing.

## Experiments (each parent noted)

| ID | Parent and change | Verified score | Finding |
| --- | --- | --- | --- |
| A | Base: post-loop mode1/low delta short -> s32 | 300, 98.5% | Keep |
| B | A: offset60 hard r10 + compound address add into frameOffset | 327, 98.4% | Three-register add r3,r0,r1; missing r10 copy persists |
| C | A: separate hard-r0 yValue for first high Y load | 300 | No assembly improvement |
| D | Prior N1 natural-loop source: signed pointer bound + s32 delta | 2377, 88.6% | Entry test and +10 giv remain; rejected |
| E | A: post-loop mode1/low byteOffset r1 and storeSprite r4 | 272, 98.6% | Keep |
| F | E: new block-local plain xZero for second X load | Build rejected | Declaration was after statements; compiler requires C89 declaration placement; tool undid edit |
| G | E: corrected F declaration at block start | 272 | No assembly improvement |
| H | E: volatile xPosition field read in that X block | 297, 98.5% | Worse add operand/order; changes downstream scratch |
| I | E: volatile-qualified saved Y-pointer reads (all four) | 239, 98.8% | Keep; all four Y address loads match |
| J | I: volatile ROM X-table read in mode1/low | 406, 98.0% | ldrh + shift/asr extension instead of ldrsh; reject |
| K | I: post-loop mode1/low xPosition u32 -> u16, still hard r3 | 228, 98.9% | Keep |
| L | K: post-loop hard-r2 zero int -> u16 | 228 | Identical assembly; not kept |
| M | K: hard-r3 frameOffset int -> s16 | 440, 97.8% | Gets initial mov r3,sl, but adds six sign-extension instructions; reject |
| N | K: short-lived X table pointers r1, mode1/low compound address expression | 190, 99.0% | Keep |
| O | N: unpin frameOffset | 165, 99.2% | Best; independently recompiled and saved |

One intermediate restore to A was used to examine its complete object diff before E. The final saved source is O, not the last equal-scoring variant or an unverified edit.

## Recommended next experiments

1. Start from best O. Inspect best.i.combine and best.i.greg around insns 97,107,109. Investigate why r0 output reload remains inheritable into the pointer add, whereas target requires a reload from r10 into r3. Do not repeat mere offset60 pinning or a textual copy/compound addition.
2. For the three X-address blocks, inspect where combine/local allocation destroys the hard-r0 address temporary and coalesces it with the table base or frameOffset. Compare the successful volatile private Y-pointer loads. Avoid volatile ROM reads: they demonstrably produce ldrh plus extension.
3. Treat the three post-loop signed-load zero registers as reload/scratch selections, not variables to pin blindly. Narrowing xPosition already changed one downstream choice. Find the earlier reload or hard-register conflict responsible.
4. If revisiting natural loops, solve both entry-check removal and the extra +10 giv before allocation tuning. Signed comparison alone was insufficient.
5. Preserve the other function's working-tree changes. If an exact candidate is found, verify independently, and commit a patch containing only this function's intended changes.

## Full best verified implementation


```c
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
```

## Full remaining object disassembly diff

```diff
--- target

+++ current

@@ -31,5 +31,4 @@

 mov	sl, r0
-mov	r3, sl
-adds	r3, r3, r1
-str	r3, [sp, #8]
+adds	r0, r0, r1
+str	r0, [sp, #8]
 lsls	r3, r5, #1
@@ -40,3 +39,3 @@

 str	r0, [sp, #24]
-ldr	r6, [pc, #92]	@ (1bc <func_804A508+0xb0>)
+ldr	r6, [pc, #96]	@ (1bc <func_804A508+0xb0>)
 ldr	r1, [pc, #96]	@ (1c0 <func_804A508+0xb4>)
@@ -52,3 +51,3 @@

 cmp	r0, r1
-beq.n	178 <func_804A508+0x6c>
+beq.n	176 <func_804A508+0x6a>
 b.n	2b0 <func_804A508+0x1a4>
@@ -56,3 +55,3 @@

 cmp	r0, #172	@ 0xac
-beq.n	180 <func_804A508+0x74>
+beq.n	17e <func_804A508+0x72>
 b.n	2b0 <func_804A508+0x1a4>
@@ -62,3 +61,3 @@

 cmp	r0, #0
-bne.n	18c <func_804A508+0x80>
+bne.n	18a <func_804A508+0x7e>
 b.n	2b0 <func_804A508+0x1a4>
@@ -80,2 +79,3 @@

 b.n	1e0 <func_804A508+0xd4>
+.short	0x0000
 ldr	r1, [sp, #12]
@@ -89,5 +89,5 @@

 ldr	r1, [pc, #36]	@ (200 <func_804A508+0xf4>)
-adds	r0, r3, r1
-movs	r3, #0
-ldrsh	r0, [r0, r3]
+adds	r1, r3, r1
+movs	r3, #0
+ldrsh	r0, [r1, r3]
 lsls	r0, r0, #2
@@ -129,5 +129,5 @@

 ldr	r1, [pc, #8]	@ (23c <func_804A508+0x130>)
-adds	r0, r4, r1
-movs	r3, #0
-ldrsh	r0, [r0, r3]
+adds	r1, r4, r1
+movs	r4, #0
+ldrsh	r0, [r1, r4]
 b.n	258 <func_804A508+0x14c>
@@ -143,5 +143,5 @@

 ldr	r4, [pc, #52]	@ (288 <func_804A508+0x17c>)
-adds	r0, r3, r4
-movs	r1, #0
-ldrsh	r0, [r0, r1]
+adds	r4, r3, r4
+movs	r3, #0
+ldrsh	r0, [r4, r3]
 lsls	r0, r0, #2
@@ -192,3 +192,3 @@

 bgt.n	2c4 <func_804A508+0x1b8>
-b.n	16e <func_804A508+0x62>
+b.n	16c <func_804A508+0x60>
 ldr	r1, [sp, #0]
@@ -203,4 +203,4 @@

 adds	r0, r1, r0
-movs	r3, #0
-ldrsh	r0, [r0, r3]
+movs	r4, #0
+ldrsh	r0, [r0, r4]
 lsls	r0, r0, #2
@@ -240,4 +240,4 @@

 adds	r1, r1, r0
-movs	r2, #0
-ldrsh	r0, [r1, r2]
+movs	r3, #0
+ldrsh	r0, [r1, r3]
 lsls	r0, r0, #2
@@ -286,4 +286,4 @@

 adds	r0, r1, r0
-movs	r2, #0
-ldrsh	r0, [r0, r2]
+movs	r4, #0
+ldrsh	r0, [r0, r4]
 lsls	r0, r0, #2
```

## Session 14 (Claude, 2026-10-16): root cause of the missing `mov r3, sl`

Budget: about 20 compiles; 12 used. func_804A84C: 1 decomp.wtf compile, 370 → **165** (Codex's changes ported). func_804A508: 8 local trials plus RTL dumps; nothing beat 165, so the saved best is unchanged. Every trial below starts from best O and changes only what is listed. "Lines" is the local changed-line count against `expected/` (best O = 46).

### What actually happens at the first store

The greg dump for best O, with registers already assigned:

    insn 97:   r0 = r0 << 1          ; dest is offset60 (pseudo 29, global-allocated r10): OUTPUT reload in r0
    insn 1533: sl = r0               ; the output-reload store
    insn 107:  r0 = r0 + r1          ; input offset60 INHERITED from r0; dest pseudo 52 (REG_EQUIV [sp+8]) local-allocated r0
    insn 109:  [sp+8] = r0

Combine merged `offset <<= 1` with `offset60 = offset` (offset dies there). That makes r0 a reload copy of offset60, and `choose_reload_regs` reuses it. The target instead re-reads r10 into r3, which is also the destination register: a fresh reload with no inheritance.

### Experiments

| ID | Change from best O | Lines | Result |
|---|---|---|---|
| E1 | `register int offset60 asm("r10")`; mode1HighY add uses `offset60` directly | 46 | Same code. Combine still merges the shift into the hard r10 set (`sl = r49 << 1`), so the output-reload inheritance stays |
| E2 | E1 + `register s32 offset asm("r0")` | 46 | Same; combine merges `r0 <<= 1` into `sl = r0` even with both registers hard |
| N1 | session 12's natural loop (unchanged) | 340 | **`mov sl,r0; mov r7,sl; adds r7,r7,r1; str r7,[sp,#8]`**: the target pattern (r7 because of N1's swapped allocation) |
| X1 | function-scope `const s16 *mode1High` = the add; `locals.mode1HighY = mode1High`; the loop reads `yPointer = mode1High` | 122 | mode1High takes r10 and offset60 spills to a new sp+28; frame 0x20 |
| X2 | E1 + X1 | 122 | **`mov r3,sl; adds r3,r3,r1`** exactly as the target, then `str r3,[sp,#28]; str r3,[sp,#8]`: mode1High spilled to its *own* slot, frame 0x20 |
| L3b | `locals` struct → 7 plain locals (no `&`), Y reads plain | 160 | offset60 spilled (sp+0), xDelta r10 |
| L3c | L3b + `xDelta asm("r7")` + `offset60 asm("r10")` | 136 | Prologue matches, including `mov r3,sl; adds r3,r3,r1; str r3,[sp,#8]` and the slots sp+0..20. But childWork1 lands in r7 **together with xDelta** (r7 = FRAME_POINTER_REGNUM, so the pin is not reserved) and the frame is 0x18 |

### Conclusions

1. The target's `mov r3, sl` needs the add's **destination to be a spilled pseudo**. Reload then merges the input and output reloads (`combine_reloads`, RELOAD_OTHER) and picks a fresh register, with no inheritance from r0. X2 and N1 both show this.
2. With the `locals` struct, that spill can't be the struct's own slot. Store equivalence (`REG_EQUIV` to the struct field, `update_equiv_regs` in local-alloc.c) requires `REG_BASIC_BLOCK >= 0` (block-local), and local-alloc always finds a block-local pseudo a register. **The struct form has a ceiling at this instruction.** Pinning, copies and compound adds can't get past it (Codex B, L1, E1, E2).
3. In the target, all 7 "locals" are spilled pseudos. Spill slots are assigned in pseudo order, above any stack variables, which is why mode..childWork1 land at sp+0..24. L3c reproduces this, apart from childWork1.
4. The L3 blocker is global-alloc priority, `floor_log2(refs) * refs / live_length`. In L3b the dump gives childWork1 (pseudo 38) 5 refs over 92 insns and xDelta (pseudo 23) 8 refs over 338, so childWork1 gets r7 first. flow.c adds `loop_depth` per reference, and depth only rises inside real `for`/`while`/`do` loop notes. A goto-loop therefore counts every in-loop reference once, while the original natural loop weighted them up, which is another reason the original was a natural loop.
5. **Never pin r7** (also seen in session 5, EscapeRipple V7c). In Thumb RTL, r7 is the frame pointer before elimination, so an asm("r7") variable is not treated as occupying it.
6. The X-address residual (`adds r1,r3,r1` vs target `adds r0,r3,r1`) has a different cause. Combine folds the pinned-r0 `xAddress` into a new pseudo (`pseudo115 = frameOffset + r1`, then `ldrsh` via a scratch), and local-alloc gives that pseudo a hard-register suggestion of the dying r1 (`combine_regs`). The target ties it to the load result r0 instead.

### Recommended next session

Go back to the natural loop (N1/N3), now that the prologue mechanism is understood. It's the only form that gives both the spilled-invariant reloads and loop-weighted priorities. Open problems from session 12 still apply: the loop entry test, the extra `sprite+10` giv, and xDelta/frame*2 swapped between r3 and r7. Then port to func_804A84C (twin; it now has the same 165 residual).

## Session 15 (Claude, 2026-10-16): natural index loop

Candidates and per-step notes: `reports/swinging_platform_candidates/` (README has the table). Budget about 15 local compiles. The saved best (O, 165) is unchanged unless noted.

- **C1 (index loop, `gSpriteData[i].field`)**: the loop shape is right first time: no pre-loop compare, r2 pointer, no muls, in-loop bound recomputed from ip with `bgt`. This is the form of the matched upstream `SpriteUtilDespawnChildWithParent`. Remaining work is all allocation.
- **C2/C3**: accessing `gWarioData` directly puts the hoisted wario load at the target's position; storing `0` (target reuses r1 = work1, known zero) lowers the work1 giv's priority so offset60 takes r10 and the giv spills to sp+24. Stack layout and frame size now match.
- **C4**: pinning the prologue's `frameStore` to r3 stops CSE linking it to the loop's hoisted `&gUnk_3000A4B` (cse.c doesn't record non-fixed hard regs under SMALL_REGISTER_CLASSES), which had kept r3 busy across the prologue. Prologue now matches through `str r3,[sp,#8]`.
- **C5/C6**: global-alloc priorities (frame .36, wario .33, xDelta .30, frame*2 .25, offset48 .25) can't give the target order on their own. The target order follows if wario conflicts with hard r3/r4 at the work1 reads (`ldr r4,[sp,#24]` / `ldr r3,[sp,#24]`), where frame*2 and offset48 are already dead. Pinning those pointers fixes wario r6 / xDelta r7, but breaks the work1 giv (it becomes `i*44` + a constant) and leaves frame*2/offset48 swapped.
- **C7/C8 (rejected)**: pinning the signed-load zero index to a hard register inside the loop changes which addresses loop.c hoists (Y pointers lost in C7, X hoisted instead of Y in C8). Hard pins must stay outside the loop body or on values the loop doesn't hoist.
- **Allocation note**: in C4's greg dump no hard-register preferences are recorded for frame*2 (pseudo 91), offset48 (156) or wario (122). `regs_someone_prefers` only helps if frame*2/offset48 pick up r3/r4 preferences, which `set_preference` records only from a set involving a hard reg or a locally allocated pseudo.
- **C9 (24 lines, best so far)**: the low-register permutation is a global-alloc priority effect. find_reg's pass 0 only takes registers already in `regs_used_so_far`, so the target order needs frame*2 and offset48 allocated *before* wario. Wrapping their in-loop X loads in `do { xLoad = tbl[idx]; } while (0)` (double-nested for offset48) adds loop depth to those refs (flow.c `REG_N_REFS += loop_depth`), raising frame*2 to .48 and offset48 to .375 above wario .333. Result: r3/r4/r6/r7 exactly as target; loop shape, stack layout and prologue all still match. Remaining 24 lines: reload register for `mode` and the work1-pointer setup (r4 vs r0), `adds r0,r0,r7` vs `adds r0,r7,r0` in three xPosition adds, and two post-loop ldrsh zero indices.
- **C10-C12**: `x = xDelta + x` is canonicalised (no change). Session 12's `(xDelta) - (-(x))` form fixes all three `adds r0,r7,r0` (C11, 18 local lines). Loading `mode` through an r0-pinned temp (C12) is folded by combine (no change).
- **C11 submitted: decomp.wtf score 165 -> 25 (99.8%), saved best, written to `src/` (not committed: no match).** Remaining 9 operand differences are all reload register choices:
  - insn 56 `mode = *(r4)` (mode spilled to sp+0): the output reload gets r4 (the dying address reg); target r0.
  - insn 1450 `work1ptr = ip + 40` (spilled to sp+24): the in/out reload gets r4; target r0.
  - post-loop mode1-low X and mode0-low Y ldrsh zero index: r3 / r4, target r2. The `zero asm("r2")` pins from best O are folded by combine; their old effect depended on the old reload rotation.
  - reload1.c `allocate_reload_reg` round-robins over `spill_regs` from `last_spill_reg`; each insn chain's candidate set comes from `order_regs_for_reload` (call-used unused regs first, then call-saved). Both prologue cases want r0, which suggests r0 is excluded or used-by-pseudo at those insns in ours.
- **Next session:** (1) find what makes r0 unavailable or non-preferred for reload at insns 56 and 1450 (dump `-dg` with reload chains; check which pseudos allocated to r0-r3 are live there); (2) retune the two post-loop zero indices; (3) port the natural loop plus `do { } while (0)` X-load wrappers to func_804A84C (swap PSPRITE_AC->AD and the six tables), which should take it from 165 to about 25 too.

# DrawHomerunDerby: report for GPT-6

**Status:** unmatched. decomp.wtf best is **score 174 (99.7%)**, down from 663 in session 12 (Claude, 2026-10-16); 13 of 15 capped compiles used.
Unit: `src/minigames/homerun_derby.c`. Local check: `try.py minigames/homerun_derby DrawHomerunDerby cand.c` gives 77 changed lines.
About 20 of those 77 lines are only display noise, not real differences:
- Expected-object pool words hold raw addresses (`0x03001444` = gOamBuffer and `0x03004a10` = gHomerunPitcherAnimation, per `build/us/warioland4_us.map`) where ours has relocations. They are the same bytes in the ROM.
- At target line 871, `.word 0x190000c0` is exactly our `lsls r0,r0,#3; adds r0,r0,r4`.

## What fixed things this session
1. **Camera block affine pointers** (fixed the sp+2/sp+4/sp+6 setup at the function start):
   `affinePb = &affineMatrix[1]; affinePc = &affineMatrix[2]; affinePd = affinePb + 2;`
2. **Bat loop counts with `firstOamIndex` itself** (`firstOamIndex = nextOamIndex - firstOamIndex; ... firstOamIndex -= 1; while (firstOamIndex != 0);`), as the camera and static-OAM loops do. That frees r5, so `activeAnimation = sHomerunBallIdleAnimation` stays in r5 across the bat loop as in target (`adds r1,r1,r5`). Local diff went from 216 to 77 lines.
3. **Bonus frame-duration lookup, index-first:** `frameDuration = (u32)((struct AnimationFrame *)((gHomerunBonusRender.animationFrame << 3) + (u32)bonusAnimationFrames))->time;`.
   The old `bonusFrame = &bonusAnimationFrames[(u32)bonusFrame]` produced `adds r0,r4,r0`. The plain `bonusAnimationFrames[...]` form did not change the output either.

## Remaining real differences

### (a) Ball-hit block (gMinigameState 8/9): reload rotation is one step behind
Preheader, identical in target and ours:
```
movs r0,#15 ; negs r0,r0 ; mov ip,r0      reload r0 (-15)
ldr r1,=0x1ff ; mov sl,r1                 reload r1 (511)
mov r2,r8 ; subs r7,r2,r7                 reload r2 (r8, for subs) in OURS
```
In the loop at `positionValue &= coordinateMask` (where `coordinateMask` is `asm("r2")`, copied from `oamXMask` in sl), target emits `mov r2, sl; ands r1, r2` and ours emits `mov r0, sl; ands r1, r0`.
Every reload temporary in the following ball FixedMul/FixedInverse block is then rotated: target uses r3,r1,r2,r3,r1,r2..., ours r1,r2,r3,r1,r2,r3.
- From greg: the `sl` copy is reload insn 4030 in ours, and combine deleted the C-level `coordinateMask = oamXMask` copy (hard to hard).
- Rotation reading: ours last_spill_reg = r2 (the r8 reload), r3 is busy (frameData), so the next pick is r0. Target picks r2, which implies last = r1. So **target's `mov r2,r8` is probably not a reload**: either a real pseudo or hard-register copy, or a different set of reloads.

Diff (target `-`, ours `+`):
```
@@ -289,4 +289,4 @@
 subs r1, #32
-mov r2, sl
-ands r1, r2
+mov r0, sl
+ands r1, r0
 ldrh r2, [r4, #2]
@@ -312,8 +312,8 @@
 lsls r0, r0, #1
-ldr r3, [pc, #196]
-adds r0, r0, r3
-movs r1, #0
-ldrsh r4, [r0, r1]
-movs r2, #18
-ldrsh r0, [r5, r2]
+ldr r1, [pc, #196]
+adds r0, r0, r1
+movs r2, #0
+ldrsh r4, [r0, r2]
+movs r3, #18
+ldrsh r0, [r5, r3]
 bl
@@ -330,8 +330,8 @@
 lsls r0, r0, #1
-ldr r3, [pc, #160]
-adds r0, r0, r3
-movs r1, #0
-ldrsh r4, [r0, r1]
-movs r2, #18
-ldrsh r0, [r5, r2]
+ldr r1, [pc, #160]
+adds r0, r0, r1
+movs r2, #0
+ldrsh r4, [r0, r2]
+movs r3, #18
+ldrsh r0, [r5, r3]
 bl
@@ -344,8 +344,8 @@
 R_ARM_THM_CALL FixedMul
-ldr r3, [sp, #8]
-strh r0, [r3, #0]
+ldr r1, [sp, #8]
+strh r0, [r1, #0]
 ldrh r0, [r5, #16]
 lsls r0, r0, #1
-ldr r1, [pc, #124]
-adds r0, r0, r1
+ldr r2, [pc, #124]
+adds r0, r0, r2
 ldrh r4, [r0, #0]
@@ -354,4 +354,4 @@
 asrs r4, r4, #16
-movs r2, #18
-ldrsh r0, [r5, r2]
+movs r3, #18
+ldrsh r0, [r5, r3]
 bl
@@ -364,4 +364,4 @@
 R_ARM_THM_CALL FixedMul
-ldr r3, [sp, #12]
-strh r0, [r3, #0]
+ldr r1, [sp, #12]
+strh r0, [r1, #0]
 ldrh r0, [r5, #16]
@@ -369,8 +369,8 @@
 lsls r0, r0, #1
-ldr r1, [pc, #80]
-adds r0, r0, r1
-movs r2, #0
-ldrsh r4, [r0, r2]
-movs r3, #18
-ldrsh r0, [r5, r3]
+ldr r2, [pc, #80]
+adds r0, r0, r2
+movs r3, #0
+ldrsh r4, [r0, r3]
+movs r1, #18
+ldrsh r0, [r5, r1]
 bl
@@ -383,4 +383,4 @@
```

### (b) Static-OAM block (sHomerunBallIdleAnimation) preheader
Target `ldr r1,=gOamBuffer; ldr r2,=0xFFFFFE00; mov ip,r2`; ours `... ldr r0,=0xFFFFFE00; mov ip,r0`.
The source has `{ register u32 maskSeed asm("r2"); maskSeed = 0xFFFFFE00; oamAttributeMask = maskSeed; }`. Combine folds it, so the reload gets r0. It is the same r0-vs-r2 pattern as (a).

## Tried and rejected this session (local line counts)
| Try | Result |
|---|---|
| `register u16 oamAttribute6 asm("r2")` | 289 (worse) |
| `coordinateMask = nextOamIndex; secondaryEntryCount = coordinateMask - firstOamIndex;` | no change (combine deletes the copy, insn 745) |
| unpinned `s32 ballMaskCopy = oamXMask; positionValue &= ballMaskCopy;` | no change (r0) |
| ball loop counted with `firstOamIndex` (like the bat loop) | no change (it already got r7) |
| plain `bonusAnimationFrames[frame].time` | no change |

Facts: Thumb agbcc has no REG_ALLOC_ORDER (r0 first). Local alloc never uses r7.

## Open question
How can the ball preheader's `nextOamIndex - firstOamIndex` (r8 in a high register) produce `mov r2, r8` that is not a reload, so that the next loop reload of `sl` rotates to r2? Combine folds hard-to-hard copies such as `asm("r2")` vars, and a pseudo copy gets r0 from local alloc.
Candidates not tried yet:
- Compute the count before `oamXMask = 0x1FF`, or before the `-15`, to reorder the reloads. This would change instruction order, so only if a later pass restores it.
- A pseudo for the r8 copy whose lifetime overlaps r0 and r1 (both busy), e.g. by keeping a pointer in r0 live across it.
- Find a block where the original authors' code naturally makes r2 the count source (the pitcher block uses `positionValue(r1) = nextOamIndex; count = positionValue - firstOamIndex`, which works because positionValue is a real pinned use).

## Current best C (as saved on decomp.wtf, score 174)
```c
void DrawHomerunDerby(void)
{
  u16 affineMatrix[4];
  u16 *affinePb;
  u16 *affinePc;
  u16 *affinePd;
  register struct HomerunCameraState *cameraTransform asm("r9");
  struct HomerunCameraState *cameraForMatrix;
  register struct HomerunCameraState *cameraForRotation asm("r3");
  register struct HomerunPitcherState *pitcherState asm("r3");
  register struct HomerunSimpleAnimation *activeAnimationState asm("r3");
  struct AnimationFrame *activeAnimation;
  u32 animationCounterLimit;
  register u32 oamXMask asm("sl");
  s32 oamAttributeMask;
  register long preservedOamBits;
  register OamByte *dataPointer asm("r2");
  OamByte *oamBuffer;
  OamByte *oamEntry;
  register OamByte *objectState asm("r5");
  struct HomerunCameraState *cameraForSprites;
  s16 matrixCosine;
  s16 animationMatrixPb;
  s16 animationMatrixPc;
  s16 animationMatrixPd;
  struct HomerunCameraState *oamXMaskPointer;
  s16 matrixSine;
  s16 matrixNegativeSine;
  struct HomerunSimpleAnimation *bonusRenderState;
  s16 matrixCosineD;
  s16 ballMatrixCosine;
  s16 ballMatrixSine;
  s16 ballMatrixNegativeSine;
  int ballMatrixCosineD;
  s16 animationMatrixPa;
  register unsigned int oamByteMask asm("sl");
  register OamByte *resultAnimation asm("r4");
  register s32 workingOffset asm("r0");
  struct AnimationFrame *bonusAnimation;
  u32 frameDuration;
  OamByte *pitcherAnimation;
  s32 entryCounter;
  s32 cameraEntryCount;
  s32 secondaryEntryCount;
  s32 batEntryCount;
  register s32 bonusEntryCount asm("r7");
  s32 pitcherEntryCount;
  s32 savedOamIndex;
  struct AnimationFrame *bonusAnimationFrames;
  u16 *affinePointer;
  u16 **affinePointerReference;
  u16 *bonusFrameData;
  u16 *pitcherFrameData;
  u16 *resultFrameData;
  u16 *ballFrameData;
  u16 *secondaryFrameData;
  u16 *batFrameData;
  u16 *activeFrameData;
  register u16 *frameData asm("r3");
  u8 *pitcherOamBytes;
  struct HomerunCameraState *cameraData;
  int clearAffineModeMask;
  register OamByte *oamDataDestination asm("r6");
  u16 frameTimeValue;
  int clearOamXMask;
  u16 affinePdValue;
  s32 ballAffinePd;
  s32 uiAnimationTimer;
  s32 animationTimer;
  u16 affinePaValue;
  register s32 positionValue asm("r1");
  u8 *stackPointer;
  s32 frameCounter;
  s32 bonusAnimationTimer;
  s32 pitcherAnimationTimer;
  s32 resultAnimationTimer;
  register u16 oamAttribute asm("r2");
  register s32 oamCoordinate10 asm("r2");
  u16 oamAttribute11;
  register s32 oamCoordinate12 asm("r2");
  register u16 oamAttribute13 asm("r2");
  u16 oamAttribute14;
  register s32 oamCoordinate15 asm("r2");
  u16 pitchSoundId;
  register u16 oamAttribute16 asm("r2");
  register s32 oamCoordinate17 asm("r2");
  u16 oamAttribute2;
  int oamXMaskValue;
  register struct AnimationFrame *bonusFrame asm("r0");
  register s32 coordinateMask asm("r2");
  register u16 oamAttribute3 asm("r2");
  register s32 oamCoordinate4 asm("r2");
  u16 oamAttribute5;
  u16 oamAttribute6;
  register s32 oamCoordinate7 asm("r2");
  register s32 oamCoordinate8 asm("r2");
  register s32 oamCoordinate9 asm("r2");
  u8 byteValue;
  u8 pitcherFrameTime;
  u8 pitcherAnimationIndex;
  u8 resultFrameTime;
  u32 currentFrameDuration;
  u8 frameTimeA;
  u8 frameTimeB;
  u8 frameTimeC;
  u8 frameTimeD;
  u8 pitcherAnimationType;
  u8 animationType;
  u8 bonusAnimationIndex;
  s32 firstOamIndex;
  register s32 nextOamIndex asm("r8");
  void *oamBufferPointer;
  int clearObjectModeMask;
  void *currentOamBytes;
  void *resultOamBytes;
  OamByte *ballOamBytes;
  void *secondaryOamBytes;
  OamByte *batOamBytes;
  void *staticOamBytes;
  OamByte *bonusOamBytes;
  void *pitcherOamEntry;
  bonusRenderState = &gHomerunBonusRender;
  firstOamIndex = 0;
  nextOamIndex = gOamSlotsUsed;
  workingOffset = nextOamIndex * 8;
  oamBuffer = (OamByte *) gOamBuffer;
  oamDataDestination = (OamByte *) (((s32) workingOffset) + ((s32) oamBuffer));
  byteValue = gMinigameCameraState.oamMode;
  cameraTransform = &gMinigameCameraState;
  if (byteValue != 0)
  {
    if (byteValue == 1)
    {
      frameData = sHomerunCameraOamMode1;
    }
    if (byteValue == 2)
    {
      frameData = sHomerunCameraOamMode2;
    }
    nextOamIndex += *frameData;
    frameData = (u16 *) (((u8 *) frameData) + 2);
    affinePb = &affineMatrix[1];
    affinePc = &affineMatrix[2];
    affinePd = affinePb + 2;
    if (firstOamIndex < nextOamIndex)
    {
      cameraForSprites = cameraTransform;
      oamEntry = oamBuffer;
      oamXMask = 0x1FF;
      oamAttributeMask = -13;
      firstOamIndex = nextOamIndex;
      do
      {
        oamAttribute = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute;
        oamDataDestination += 2;
        *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((*cameraForSprites).yPosition + oamAttribute) - 0x10);
        *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
        oamAttribute2 = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute2;
        oamDataDestination += 2;
        positionValue = *((s16 *) (((u8 *) cameraForSprites) + 8));
        positionValue = oamAttribute2 + positionValue;
        positionValue -= 0x20;
        coordinateMask = oamXMask;
        positionValue &= coordinateMask;
        oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
        preservedOamBits = 0xFFFFFE00;
        preservedOamBits &= oamAttribute2;
        *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
        preservedOamBits = -0xF;
        *((u8 *) (((u8 *) oamEntry) + 3)) = (u8) (preservedOamBits & positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = (u16) (*((u16 *) (((u8 *) frameData) + 0)));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 5));
        *((u8 *) (((u8 *) oamEntry) + 5)) = (u8) (oamAttributeMask & positionValue);
        oamDataDestination += 4;
        oamEntry = oamEntry + 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    cameraForRotation = cameraTransform;
    matrixCosine = sSinCosTable[(((u16) cameraForRotation->rotation) >> 4) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) matrixCosine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForRotation = cameraTransform;
    matrixSine = sSinCosTable[((u16) cameraForRotation->rotation) >> 4];
    *affinePb = (u16) FixedMul((s32) matrixSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixNegativeSine = 0 - sSinCosTable[((u16) cameraForMatrix->rotation) >> 4];
    *affinePc = (u16) FixedMul((s32) matrixNegativeSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixCosineD = sSinCosTable[(((u16) cameraForMatrix->rotation) >> 4) + 0x40];
    affinePdValue = (u16) FixedMul((s32) matrixCosineD, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    *affinePd = affinePdValue;
    oamBuffer = (OamByte *) gOamBuffer;
    *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) affineMatrix[0];
    *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
    *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = affinePdValue;
  }
  if (((((u32) gMinigameState) > 2U) && (gMinigameState != 0xF)) && (gMinigameState != 0xE))
  {
    if (((u32) ((u8) (gMinigameState - 8))) <= 1U)
    {
      dataPointer = (OamByte *) (&gHomerunBallRenderState);
      frameCounter = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
      *((u16 *) (((u8 *) dataPointer) + 0)) = frameCounter;
      frameTimeB = sHomerunBallHitDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
      animationCounterLimit = (u32) ((u16) frameCounter);
      objectState = dataPointer;
      if (((u32) frameTimeB) < animationCounterLimit)
      {
        *((u16 *) (((u8 *) objectState) + 0)) = 0U;
        *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
        frameTimeB = sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
        if (frameTimeB == 0)
        {
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) frameTimeB;
        }
      }
      secondaryFrameData = sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam;
      nextOamIndex += *secondaryFrameData;
      frameData = secondaryFrameData + 1;
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        cameraTransform = (struct HomerunCameraState *) objectState;
        oamAttributeMask = -15;
        workingOffset = firstOamIndex << 3;
        oamEntry = (OamByte *) (workingOffset + positionValue);
        oamXMask = 0x1FF;
        secondaryEntryCount = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute5 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute5;
          oamDataDestination += 2;
          cameraForMatrix = cameraTransform;
          *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((((s32) (cameraForMatrix->rotation << 0x10)) >> 0x14) + oamAttribute5) - 0x20);
          *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
          oamAttribute6 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute6;
          oamDataDestination += 2;
          positionValue = (s32) (cameraTransform->scale << 0x10);
          positionValue >>= 0x14;
          positionValue = oamAttribute6 + positionValue;
          positionValue -= 0x20;
          coordinateMask = oamXMask;
          positionValue &= coordinateMask;
          oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
          preservedOamBits = 0xFFFFFE00;
          preservedOamBits &= oamAttribute2;
          *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
          positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
          currentOamBytes = (u8 *) oamEntry;
          *((u8 *) (((u8 *) currentOamBytes) + 3)) = (u8) (oamAttributeMask & positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          oamDataDestination += 4;
          oamEntry += 8;
          secondaryEntryCount -= 1;
        }
        while (secondaryEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
      ballMatrixCosine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      affineMatrix[0] = (u16) FixedMul((s32) ballMatrixCosine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixSine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePb = (u16) FixedMul((s32) ballMatrixSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixNegativeSine = 0 - sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePc = (u16) FixedMul((s32) ballMatrixNegativeSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixCosineD = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      ballAffinePd = (*affinePd = FixedMul((s32) ballMatrixCosineD, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale))));
      oamBuffer = (OamByte *) gOamBuffer;
      *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) (affinePaValue = affineMatrix[0]);
      *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
      *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
      *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = ballAffinePd;
    }
    else
    {
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (gMinigameState == 7)
      {
        dataPointer = (OamByte *) (&gHomerunBallRenderState);
        positionValue = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
        *((u16 *) (((u8 *) dataPointer) + 0)) = positionValue;
        workingOffset = (u32) sHomerunBallPitchDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
        affinePaValue = (u16) positionValue;
        objectState = dataPointer;
        if (((u32) workingOffset) < ((u32) affinePaValue))
        {
          *((u16 *) (((u8 *) objectState) + 0)) = 0U;
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
          frameTimeA = sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
          animationMatrixPc = (u16) frameTimeA;
          if (frameTimeA == 0)
          {
            *((u16 *) (((u8 *) objectState) + 2)) = animationMatrixPc;
          }
        }
        ballFrameData = sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam;
        nextOamIndex += *ballFrameData;
        frameData = ballFrameData + 1;
        stackPointer = ((u8 *)affineMatrix);
        affinePointerReference = (u16 **) (&stackPointer);
        affinePb = (u16 *) (((u8 *) (*affinePointerReference)) + 2);
        affinePc = (u16 *) (((u8 *) (*affinePointerReference)) + 4);
        affinePd = (u16 *) (((u8 *) (*affinePointerReference)) + 6);
        clearObjectModeMask = -4;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamAttributeMask = 0xFFFFFE00;
          workingOffset = firstOamIndex << 3;
          ballOamBytes = (OamByte *) (((s32) workingOffset) + positionValue);
          cameraTransform = (struct HomerunCameraState *) 0x1FF;
          oamByteMask = -0xD;
          cameraEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamAttribute3 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute3;
            oamDataDestination += 2;
            *((s8 *) (((u8 *) ballOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 0xA))) + oamAttribute3);
            *((u8 *) (((u8 *) ballOamBytes) + 1)) = (u8) (clearObjectModeMask & (*((u8 *) (((u8 *) ballOamBytes) + 1))));
            oamCoordinate4 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate4;
            oamDataDestination += 2;
            positionValue = *((s16 *) (((u8 *) objectState) + 8));
            positionValue = oamCoordinate4 + positionValue;
            positionValue &= (s32) cameraTransform;
            oamAttribute = *((u16 *) (((u8 *) ballOamBytes) + 2));
            preservedOamBits = oamAttributeMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) ballOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) ballOamBytes) + 5));
            *((u8 *) (((u8 *) ballOamBytes) + 5)) = (u8) (oamByteMask & positionValue);
            oamDataDestination += 4;
            ballOamBytes += 8;
            cameraEntryCount -= 1;
          }
          while (cameraEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
      }
    }
    clearOamXMask = 0xFFFFFE00;
    oamAttributeMask = gHomerunMissCount;
    if (((s32) oamAttributeMask) > 2)
    {
      oamAttributeMask = 2;
    }
    objectState = 0;
    if (((s32) objectState) < ((s32) oamAttributeMask))
    {
      do
      {
        nextOamIndex += *sHomerunStaticOam;
        frameData = sHomerunStaticOam + 1;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamByteMask = -0xD;
          workingOffset = firstOamIndex << 3;
          secondaryOamBytes = (OamByte *) (workingOffset + positionValue);
          secondaryEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamCoordinate7 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate7;
            oamDataDestination += 2;
            workingOffset = oamCoordinate7 + 0x20;
            *((s8 *) (((u8 *) secondaryOamBytes) + 0)) = (s8) (workingOffset - (((s32) objectState) * 8));
            oamAttribute5 = *(frameData++);
            oamCoordinate8 = oamAttribute5;
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate8;
            oamDataDestination += 2;
            positionValue = oamCoordinate8 + 0xE0;
            preservedOamBits = 0x1FF;
            positionValue &= preservedOamBits;
            oamAttribute = *((u16 *) (((u8 *) secondaryOamBytes) + 2));
            preservedOamBits = clearOamXMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) secondaryOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) secondaryOamBytes) + 5));
            *((u8 *) (((u8 *) secondaryOamBytes) + 5)) = (u8) ((oamByteMask & positionValue) | 8);
            oamDataDestination += 4;
            secondaryOamBytes += 8;
            secondaryEntryCount -= 1;
          }
          while (secondaryEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
        objectState = (OamByte *) (((s32) objectState) + 1);
      }
      while (((s32) objectState) < ((s32) oamAttributeMask));
    }
    if (gMinigameMedalHighlight != 0)
    {
      uiAnimationTimer = gMinigameUiAnimation.animationTimer + 1;
      gMinigameUiAnimation.animationTimer = uiAnimationTimer;
      if (((u32) sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time) < ((u32) ((u16) uiAnimationTimer)))
      {
        gMinigameUiAnimation.animationTimer = 1U;
        gMinigameUiAnimation.animationFrame = (u16) (gMinigameUiAnimation.animationFrame + 1);
        frameTimeC = sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time;
        if (frameTimeC == 0)
        {
          gMinigameUiAnimation.animationFrame = (u16) frameTimeC;
          gMinigameUiAnimation.animationTimer = (u16) frameTimeC;
          gMinigameMedalHighlight = 0;
          DrawHomerunDerbyScore();
        }
      }
    }
    oamXMaskValue = 0x1FF;
    batFrameData = sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].oam;
    nextOamIndex += *batFrameData;
    frameData = batFrameData + 1;
    cameraTransform = (struct HomerunCameraState *) &gHomerunBatAnimation;
    activeAnimation = sHomerunBallIdleAnimation;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      oamAttributeMask = 0x1FF;
      oamByteMask = -0xD;
      workingOffset = firstOamIndex << 3;
      batOamBytes = (OamByte *) (workingOffset + positionValue);
      firstOamIndex = nextOamIndex - firstOamIndex;
      do
      {
        oamCoordinate9 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate9;
        oamDataDestination += 2;
        workingOffset = oamCoordinate9;
        workingOffset += 0x92;
        *((s8 *) (((u8 *) batOamBytes) + 0)) = (s8) workingOffset;
        oamCoordinate10 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate10;
        oamDataDestination += 2;
        positionValue = oamCoordinate10 + 0x18;
        positionValue &= oamAttributeMask;
        clearOamXMask = 0xFFFFFE00;
        *((u16 *) (((u8 *) batOamBytes) + 2)) = (u16) ((clearOamXMask & (*((u16 *) (((u8 *) batOamBytes) + 2)))) | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        positionValue = *((u8 *) (((u8 *) batOamBytes) + 5));
        *((u8 *) (((u8 *) batOamBytes) + 5)) = (oamByteMask & positionValue) | 8;
        oamDataDestination += 4;
        batOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    activeAnimationState = (struct HomerunSimpleAnimation *) cameraTransform;
    animationTimer = activeAnimationState->animationTimer + 1;
    activeAnimationState->animationTimer = animationTimer;
    currentFrameDuration = (u32) activeAnimation[activeAnimationState->animationFrame].time;
    if (currentFrameDuration < ((u32) ((u16) animationTimer)))
    {
      activeAnimationState->animationTimer = 0U;
      activeAnimationState->animationFrame = (u16) (activeAnimationState->animationFrame + 1);
      frameTimeD = activeAnimation[activeAnimationState->animationFrame].time;
      if (frameTimeD == 0)
      {
        activeAnimationState->animationFrame = (u16) frameTimeD;
      }
    }
    cameraForMatrix = cameraTransform;
    activeFrameData = activeAnimation[((struct HomerunSimpleAnimation *) cameraForMatrix)->animationFrame].oam;
    nextOamIndex += *activeFrameData;
    frameData = activeFrameData + 1;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      {
        register u32 maskSeed asm("r2");
        maskSeed = 0xFFFFFE00;
        oamAttributeMask = maskSeed;
      }
      workingOffset = firstOamIndex << 3;
      staticOamBytes = (OamByte *) (workingOffset + positionValue);
      objectState = (OamByte *) oamXMaskValue;
      {
        register u8 negativeMaskSeed asm("r0") = 15;
        oamByteMask = -(int)negativeMaskSeed;
      }
      {
        register s32 nextCount asm("r1") = nextOamIndex;
        firstOamIndex = nextCount - firstOamIndex;
      }
      do
      {
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        workingOffset = oamCoordinate12;
        workingOffset += 0x60;
        *((u8 *) (((u8 *) staticOamBytes) + 0)) = (u8) workingOffset;
        positionValue = *((u8 *) (((u8 *) staticOamBytes) + 1));
        *((u8 *) (((u8 *) staticOamBytes) + 1)) = (u8) ((clearObjectModeMask & positionValue) | 1);
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        positionValue = oamCoordinate12 + 0xD7;
        positionValue &= (s32) objectState;
        oamAttribute = *((u16 *) (((u8 *) staticOamBytes) + 2));
        preservedOamBits = oamAttributeMask;
        preservedOamBits &= oamAttribute;
        *((u16 *) (((u8 *) staticOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
        pitcherAnimationType = *((u8 *) (((u8 *) staticOamBytes) + 3));
        workingOffset = oamXMask;
        workingOffset &= pitcherAnimationType;
        *((u8 *) (((u8 *) staticOamBytes) + 3)) = (u8) (workingOffset | 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (((u8 *) staticOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) staticOamBytes) + 5))) | 0xC);
        oamDataDestination += 4;
        staticOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    savedOamIndex = nextOamIndex;
    cameraData = cameraTransform;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPa = sSinCosTable[(*((u16 *) (((u8 *) dataPointer) + 4))) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) animationMatrixPa, (s32) ((s16) FixedInverse(0x100)));
    objectState = (OamByte *) 0x100;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPb = sSinCosTable[*((u16 *) (((u8 *) dataPointer) + 4))];
    *affinePb = (u16) FixedMul((s32) animationMatrixPb, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (frameData = (u16 *) cameraData);
    animationMatrixPc = 0 - sSinCosTable[frameData[2]];
    *affinePc = (u16) FixedMul((s32) animationMatrixPc, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (u16 *) cameraData;
    animationMatrixPd = sSinCosTable[frameData[2] + 0x40];
    frameTimeValue = (*affinePd = (u16) FixedMul((s32) animationMatrixPd, (s32) ((s16) FixedInverse((s32) objectState))));
    oamBufferPointer = (u8 *) ((OamByte *) gOamBuffer);
    oamAttribute14 = (affinePaValue = affineMatrix[0]);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x26)) = (u16) oamAttribute14;
    oamAttribute11 = *affinePb;
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x2E)) = (u16) oamAttribute11;
    *((u16 *) (((u8 *) oamBufferPointer) + 0x36)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x3E)) = frameTimeValue;
    if (gHomerunBonusRender.animationIndex == 0)
    {
    }
    else
    {
      bonusAnimationIndex = gHomerunBonusRender.animationIndex;
      bonusAnimationFrames = sHomerunBonusAnimations[bonusAnimationIndex];
      bonusOamBytes = (OamByte *) bonusAnimationFrames;
      bonusAnimationTimer = gHomerunBonusRender.animationTimer + 1;
      gHomerunBonusRender.animationTimer = bonusAnimationTimer;
      frameDuration = (u32) ((struct AnimationFrame *) ((gHomerunBonusRender.animationFrame << 3) + (u32) bonusAnimationFrames))->time;
      if (frameDuration < ((u32) ((u16) bonusAnimationTimer)))
      {
        gHomerunBonusRender.animationTimer = 1U;
        bonusAnimation = bonusAnimationFrames;
        gHomerunBonusRender.animationFrame = (u16) (gHomerunBonusRender.animationFrame + 1);
        if ((byteValue = bonusAnimation[gHomerunBonusRender.animationFrame].time) == 0)
        {
          gHomerunBonusRender.animationFrame = 0;
          animationType = gMinigameSequenceState;
          if (animationType == 1)
          {
            if (!(animationType & ((u32 (*)(void)) MinigameRandom)()))
            {
              m4aSongNumStart(0x237U);
            }
            else
            {
              m4aSongNumStart(0x238U);
            }
            m4aSongNumStart(0x2CCU);
            gHomerunBonusRender.animationIndex = (u8) gHomerunBonusRender.nextAnimationIndex;
            gHomerunBonusRender.animationTimer = 0U;
            gMinigameSequenceState += 1;
          }
        }
      }
      bonusOamBytes = (OamByte *) sHomerunBonusAnimations[(*bonusRenderState).animationIndex];
      bonusFrameData = ((struct AnimationFrame *) bonusOamBytes)[gHomerunBonusRender.animationFrame].oam;
      nextOamIndex += *bonusFrameData;
      frameData = bonusFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        OamByte *render_state;
        positionValue = (s32)gOamBuffer;
        render_state = (OamByte *) (&gHomerunBonusRender);
        cameraTransform = (struct HomerunCameraState *) 0x1FF;
        oamXMask = 0xFFFFFE00;
        bonusOamBytes = (OamByte *)((firstOamIndex << 3) + positionValue);
        bonusEntryCount = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute13 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute13;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) bonusOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) render_state) + 6))) + oamAttribute13);
          oamAttribute2 = *(frameData++);
          *((u16 *)oamDataDestination) = oamAttribute2;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) render_state) + 4));
          positionValue = oamAttribute2 + positionValue;
          positionValue &= (s32) cameraTransform;
          oamAttribute = *((u16 *) (((u8 *) bonusOamBytes) + 2));
          preservedOamBits = oamXMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) bonusOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          *((u8 *) (((u8 *) bonusOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) bonusOamBytes) + 5))) | 0xC);
          oamDataDestination += 4;
          bonusOamBytes += 8;
          bonusEntryCount -= 1;
        }
        while (bonusEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
    }
    pitcherAnimation = (OamByte *) sHomerunPitcherAnimations[gHomerunPitcherAnimation.animationIndex];
    pitcherAnimationTimer = gHomerunPitcherAnimation.animationTimer + 1;
    gHomerunPitcherAnimation.animationTimer = pitcherAnimationTimer;
    if (((u32) ((struct AnimationFrame *) pitcherAnimation)[gHomerunPitcherAnimation.animationFrame].time) >= ((u32) ((u16) pitcherAnimationTimer)))
    {
    }
    else
    {
      gHomerunPitcherAnimation.animationTimer = 0U;
      gHomerunPitcherAnimation.animationFrame = (u16) (gHomerunPitcherAnimation.animationFrame + 1);
      if (gHomerunPitcherAnimation.animationIndex == 4)
      {
        switch (gHomerunPitcherAnimation.animationFrame)
        {
          case 10:
            gMinigameState = 7;
            m4aSongNumStart(sHomerunPitchSounds[gHomerunPitchPath * 2]);
            break;
            pitchSoundId = 0x22BU;

          case 0:
            m4aSongNumStart(0x228U);
            break;

          case 1:
            m4aSongNumStart(0x229U);
            break;

          case 5:
            m4aSongNumStart(0x22AU);
            break;

          case 8:
            m4aSongNumStart(0x22BU);
            break;

        }

      }
      pitcherState = (struct HomerunPitcherState *) &gHomerunPitcherAnimation;
      pitcherFrameTime = ((struct AnimationFrame *) pitcherAnimation)[pitcherState->animationFrame].time;
      if (pitcherFrameTime == 0)
      {
        pitcherState->animationFrame = (u16) pitcherFrameTime;
        pitcherAnimationIndex = pitcherState->animationIndex;
        if (pitcherAnimationIndex == 4)
        {
          pitcherState->animationIndex = 5U;
          pitcherAnimation = (OamByte *) sHomerunPitcherAnimations[5];
        }
        else
          if (((u32) ((u8) (pitcherAnimationIndex - 1))) <= 3U)
        {
          pitcherState->pitchCounter = (u8) (pitcherState->pitchCounter + 1);
          if (gDifficulty == 2)
          {
            dataPointer = &sHomerunPitchAnimationsHard;
          }
          else
          {
            dataPointer = &sHomerunPitchAnimationsNormal;
          }
          entryCounter = (gHomerunPitchPath * 5) - 1;
          pitcherState->animationIndex = (u8) (*((u8 *) ((entryCounter + pitcherState->pitchCounter) + ((s32) dataPointer))));
          dataPointer = (OamByte *) sHomerunPitcherAnimations;
          objectState = (OamByte *) &gHomerunPitcherAnimation;
          pitcherAnimationType = *((u8 *) (((u8 *) objectState) + 4));
          pitcherAnimation = (OamByte *) ((struct AnimationFrame **) dataPointer)[pitcherAnimationType];
          if (pitcherAnimationType == 3)
          {
            m4aSongNumStart(0x226U);
          }
          if ((*((u8 *) (((u8 *) objectState) + 4))) == 2)
          {
            m4aSongNumStart(0x227U);
          }
          oamXMaskPointer = (struct HomerunCameraState *) 0x1FF;
        }
      }
    }
    pitcherFrameData = ((struct AnimationFrame *) pitcherAnimation)[((struct HomerunPitcherState *) &gHomerunPitcherAnimation)->animationFrame].oam;
    nextOamIndex += *pitcherFrameData;
    frameData = pitcherFrameData + 1;
    if (((s32) firstOamIndex) < ((s32) nextOamIndex))
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      objectState = (OamByte *) 0x1FF;
      cameraTransform = (struct HomerunCameraState *) 0xFFFFFE00;
      workingOffset = firstOamIndex << 3;
      pitcherOamEntry = (OamByte *) (workingOffset + positionValue);
      positionValue = nextOamIndex;
      pitcherEntryCount = positionValue - firstOamIndex;
      do
      {
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        workingOffset = oamCoordinate15;
        workingOffset += 0x58;
        *((u8 *) (((u8 *) pitcherOamEntry) + 0)) = (u8) workingOffset;
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        positionValue = oamCoordinate15 + 0x48;
        positionValue &= (s32) objectState;
        pitcherOamBytes = (u8 *) pitcherOamEntry;
        oamAttribute = *((u16 *) (pitcherOamBytes + 2));
        preservedOamBits = (s32) cameraTransform;
        preservedOamBits &= oamAttribute;
        *((u16 *) (pitcherOamBytes + 2)) = (u16) (preservedOamBits | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (pitcherOamBytes + 5)) = (u8) ((*((u8 *) (pitcherOamBytes + 5))) | 0xC);
        oamDataDestination += 4;
        pitcherOamEntry += 8;
        pitcherEntryCount -= 1;
      }
      while (pitcherEntryCount != 0);
      firstOamIndex = nextOamIndex;
    }
    if (((u32) ((u8) (gMinigameState - 0xB))) <= 2U)
    {
      resultAnimation = (OamByte *) sHomerunResultAnimations[gHomerunPitchResult];
      resultAnimationTimer = gHomerunResultRender.animationTimer + 1;
      if (1)
      {
        gHomerunResultRender.animationTimer = resultAnimationTimer;
      }
      if (((u32) ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time) < ((u32) ((u16) resultAnimationTimer)))
      {
        gHomerunResultRender.animationTimer = 0U;
        gHomerunResultRender.animationFrame = (u16) (gHomerunResultRender.animationFrame + 1);
        resultFrameTime = ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time;
        if (resultFrameTime == 0)
        {
          gHomerunResultRender.animationFrame = (u16) resultFrameTime;
        }
      }
      resultFrameData = ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].oam;
      nextOamIndex += *resultFrameData;
      frameData = resultFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        objectState = (OamByte *) (&gHomerunResultRender);
        oamXMask = 0x1FF;
        oamAttributeMask = 0xFFFFFE00;
        clearAffineModeMask = -0xD;
        workingOffset = firstOamIndex << 3;
        resultOamBytes = (OamByte *) (workingOffset + positionValue);
        entryCounter = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute16 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute16;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) resultOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 6))) + oamAttribute16);
          oamCoordinate17 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate17;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) objectState) + 4));
          positionValue = oamCoordinate17 + positionValue;
          positionValue &= oamXMask;
          oamAttribute = *((u16 *) (((u8 *) resultOamBytes) + 2));
          preservedOamBits = oamAttributeMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) resultOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          positionValue = *((u8 *) (((u8 *) resultOamBytes) + 5));
          *((u8 *) (((u8 *) resultOamBytes) + 5)) = (u8) (clearAffineModeMask & positionValue);
          oamDataDestination += 4;
          resultOamBytes += 8;
          entryCounter -= 1;
        }
        while (entryCounter != 0);
      }
    }
  }
  gOamSlotsUsed = nextOamIndex;
}
```


## Codex follow-up   verified 99.8%, score 93

Codex follow-up: improved DrawHomerunDerby from 99.7% score174 to VERIFIED 99.8% score93. Best restored and recompiled. Only actual machine-code differences: static OAM mask preheader target ldr r2,[pc,#0x164]; mov ip,r2 vs current ldr r0; mov ip,r0. objdiff also reports .word0x190000c0 vs lsl0,#3/add0,r4, same encoded bytes; literal relocation display differences are not actionable instructions.
Improvement: ball states8/9 branch declare block-local register s32 countSource asm("r2"); replace secondaryEntryCount=nextOamIndex-firstOamIndex with if(firstOamIndex){countSource=nextOamIndex;}else{countSource=nextOamIndex;} then secondaryEntryCount=countSource-firstOamIndex. Ordinary copy survives combine; jump2 removes duplicate conditional. Removes cyclic reload allocation event and fixes ball-loop sl mask scratch plus four affine calculations.
Follow-ups Q/Y duplicated static maskSeed initialization (or oamAttributeMask assignment) into identical firstOamIndex arms: fixes mask ldr r2 but downstream count r8->r0 insteadr1 and matrix reloads rotate backwards one (score169). R additionally duplicate hard-r1 nextCount copy: fixes count, matrix registers rotate backwards two (score163). Need compensate reload cursor without adding final instructions.
Failures: global mask SI->HI score4674; count via hard2 compoundsubtract174; global count hardr7 5079; local unpinned ballcamera2227; attr6 hardu32r2 unchanged174; volatile scale303; signedHI staticseed402(extra mov2r0); unpin OAM base1021; unpin nextIndex4390; local ballcount174; local staticmask hardip174; volatile hard2seed190(frame24); move firstIndex updatebeforeloop4665; loopliteral mask614. Q169,R163,S duplicated u8negativeMaskSeed adds redundant shifts494; T duplicated hard2 matrix pointer copy163 unchanged; U duplicate entire first static matrix lookup3058; V 3-arm mask seed981; W duplicate workingOffset=coordinate12 before+96 163 unchanged; X duplicate workingOffset=oamXMask static loop163 unchanged; Y169; Z duplicate count subtraction2311 (branches remain).
RTL diagnostics retained at %TEMP%/wl4-derby-codex (local scratch, not in the repo). Use exact function marker ';; Function DrawHomerunDerby\n' (avoid DrawHomerunDerbyScore). base/Q combine+greg dumps. Q static mask ordinary hardr2 constant; baseline hardip constant gets reload scratchr0. Compile dump via existing flags cc -E -DVERSION_US -Iinclude -nostdinc then tools/agbcc/agbcc -O2 -mthumb-interwork -fhex-asm -fprologue-bugfix -Wall -da. No tool or flag changes. Report appended reports/drawhomerunderby_for_gpt6.md. Unrelated working tree untouched. No exact-match claim or commit.

### Best source as restored

```c
void DrawHomerunDerby(void)
{
  u16 affineMatrix[4];
  u16 *affinePb;
  u16 *affinePc;
  u16 *affinePd;
  register struct HomerunCameraState *cameraTransform asm("r9");
  struct HomerunCameraState *cameraForMatrix;
  register struct HomerunCameraState *cameraForRotation asm("r3");
  register struct HomerunPitcherState *pitcherState asm("r3");
  register struct HomerunSimpleAnimation *activeAnimationState asm("r3");
  struct AnimationFrame *activeAnimation;
  u32 animationCounterLimit;
  register u32 oamXMask asm("sl");
  s32 oamAttributeMask;
  register long preservedOamBits;
  register OamByte *dataPointer asm("r2");
  OamByte *oamBuffer;
  OamByte *oamEntry;
  register OamByte *objectState asm("r5");
  struct HomerunCameraState *cameraForSprites;
  s16 matrixCosine;
  s16 animationMatrixPb;
  s16 animationMatrixPc;
  s16 animationMatrixPd;
  struct HomerunCameraState *oamXMaskPointer;
  s16 matrixSine;
  s16 matrixNegativeSine;
  struct HomerunSimpleAnimation *bonusRenderState;
  s16 matrixCosineD;
  s16 ballMatrixCosine;
  s16 ballMatrixSine;
  s16 ballMatrixNegativeSine;
  int ballMatrixCosineD;
  s16 animationMatrixPa;
  register unsigned int oamByteMask asm("sl");
  register OamByte *resultAnimation asm("r4");
  register s32 workingOffset asm("r0");
  struct AnimationFrame *bonusAnimation;
  u32 frameDuration;
  OamByte *pitcherAnimation;
  s32 entryCounter;
  s32 cameraEntryCount;
  s32 secondaryEntryCount;
  s32 batEntryCount;
  register s32 bonusEntryCount asm("r7");
  s32 pitcherEntryCount;
  s32 savedOamIndex;
  struct AnimationFrame *bonusAnimationFrames;
  u16 *affinePointer;
  u16 **affinePointerReference;
  u16 *bonusFrameData;
  u16 *pitcherFrameData;
  u16 *resultFrameData;
  u16 *ballFrameData;
  u16 *secondaryFrameData;
  u16 *batFrameData;
  u16 *activeFrameData;
  register u16 *frameData asm("r3");
  u8 *pitcherOamBytes;
  struct HomerunCameraState *cameraData;
  int clearAffineModeMask;
  register OamByte *oamDataDestination asm("r6");
  u16 frameTimeValue;
  int clearOamXMask;
  u16 affinePdValue;
  s32 ballAffinePd;
  s32 uiAnimationTimer;
  s32 animationTimer;
  u16 affinePaValue;
  register s32 positionValue asm("r1");
  u8 *stackPointer;
  s32 frameCounter;
  s32 bonusAnimationTimer;
  s32 pitcherAnimationTimer;
  s32 resultAnimationTimer;
  register u16 oamAttribute asm("r2");
  register s32 oamCoordinate10 asm("r2");
  u16 oamAttribute11;
  register s32 oamCoordinate12 asm("r2");
  register u16 oamAttribute13 asm("r2");
  u16 oamAttribute14;
  register s32 oamCoordinate15 asm("r2");
  u16 pitchSoundId;
  register u16 oamAttribute16 asm("r2");
  register s32 oamCoordinate17 asm("r2");
  u16 oamAttribute2;
  int oamXMaskValue;
  register struct AnimationFrame *bonusFrame asm("r0");
  register s32 coordinateMask asm("r2");
  register u16 oamAttribute3 asm("r2");
  register s32 oamCoordinate4 asm("r2");
  u16 oamAttribute5;
  u16 oamAttribute6;
  register s32 oamCoordinate7 asm("r2");
  register s32 oamCoordinate8 asm("r2");
  register s32 oamCoordinate9 asm("r2");
  u8 byteValue;
  u8 pitcherFrameTime;
  u8 pitcherAnimationIndex;
  u8 resultFrameTime;
  u32 currentFrameDuration;
  u8 frameTimeA;
  u8 frameTimeB;
  u8 frameTimeC;
  u8 frameTimeD;
  u8 pitcherAnimationType;
  u8 animationType;
  u8 bonusAnimationIndex;
  s32 firstOamIndex;
  register s32 nextOamIndex asm("r8");
  void *oamBufferPointer;
  int clearObjectModeMask;
  void *currentOamBytes;
  void *resultOamBytes;
  OamByte *ballOamBytes;
  void *secondaryOamBytes;
  OamByte *batOamBytes;
  void *staticOamBytes;
  OamByte *bonusOamBytes;
  void *pitcherOamEntry;
  bonusRenderState = &gHomerunBonusRender;
  firstOamIndex = 0;
  nextOamIndex = gOamSlotsUsed;
  workingOffset = nextOamIndex * 8;
  oamBuffer = (OamByte *) gOamBuffer;
  oamDataDestination = (OamByte *) (((s32) workingOffset) + ((s32) oamBuffer));
  byteValue = gMinigameCameraState.oamMode;
  cameraTransform = &gMinigameCameraState;
  if (byteValue != 0)
  {
    if (byteValue == 1)
    {
      frameData = sHomerunCameraOamMode1;
    }
    if (byteValue == 2)
    {
      frameData = sHomerunCameraOamMode2;
    }
    nextOamIndex += *frameData;
    frameData = (u16 *) (((u8 *) frameData) + 2);
    affinePb = &affineMatrix[1];
    affinePc = &affineMatrix[2];
    affinePd = affinePb + 2;
    if (firstOamIndex < nextOamIndex)
    {
      cameraForSprites = cameraTransform;
      oamEntry = oamBuffer;
      oamXMask = 0x1FF;
      oamAttributeMask = -13;
      firstOamIndex = nextOamIndex;
      do
      {
        oamAttribute = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute;
        oamDataDestination += 2;
        *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((*cameraForSprites).yPosition + oamAttribute) - 0x10);
        *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
        oamAttribute2 = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute2;
        oamDataDestination += 2;
        positionValue = *((s16 *) (((u8 *) cameraForSprites) + 8));
        positionValue = oamAttribute2 + positionValue;
        positionValue -= 0x20;
        coordinateMask = oamXMask;
        positionValue &= coordinateMask;
        oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
        preservedOamBits = 0xFFFFFE00;
        preservedOamBits &= oamAttribute2;
        *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
        preservedOamBits = -0xF;
        *((u8 *) (((u8 *) oamEntry) + 3)) = (u8) (preservedOamBits & positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = (u16) (*((u16 *) (((u8 *) frameData) + 0)));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 5));
        *((u8 *) (((u8 *) oamEntry) + 5)) = (u8) (oamAttributeMask & positionValue);
        oamDataDestination += 4;
        oamEntry = oamEntry + 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    cameraForRotation = cameraTransform;
    matrixCosine = sSinCosTable[(((u16) cameraForRotation->rotation) >> 4) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) matrixCosine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForRotation = cameraTransform;
    matrixSine = sSinCosTable[((u16) cameraForRotation->rotation) >> 4];
    *affinePb = (u16) FixedMul((s32) matrixSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixNegativeSine = 0 - sSinCosTable[((u16) cameraForMatrix->rotation) >> 4];
    *affinePc = (u16) FixedMul((s32) matrixNegativeSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixCosineD = sSinCosTable[(((u16) cameraForMatrix->rotation) >> 4) + 0x40];
    affinePdValue = (u16) FixedMul((s32) matrixCosineD, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    *affinePd = affinePdValue;
    oamBuffer = (OamByte *) gOamBuffer;
    *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) affineMatrix[0];
    *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
    *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = affinePdValue;
  }
  if (((((u32) gMinigameState) > 2U) && (gMinigameState != 0xF)) && (gMinigameState != 0xE))
  {
    if (((u32) ((u8) (gMinigameState - 8))) <= 1U)
    {
      register s32 countSource asm("r2");
      dataPointer = (OamByte *) (&gHomerunBallRenderState);
      frameCounter = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
      *((u16 *) (((u8 *) dataPointer) + 0)) = frameCounter;
      frameTimeB = sHomerunBallHitDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
      animationCounterLimit = (u32) ((u16) frameCounter);
      objectState = dataPointer;
      if (((u32) frameTimeB) < animationCounterLimit)
      {
        *((u16 *) (((u8 *) objectState) + 0)) = 0U;
        *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
        frameTimeB = sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
        if (frameTimeB == 0)
        {
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) frameTimeB;
        }
      }
      secondaryFrameData = sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam;
      nextOamIndex += *secondaryFrameData;
      frameData = secondaryFrameData + 1;
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        cameraTransform = (struct HomerunCameraState *) objectState;
        oamAttributeMask = -15;
        workingOffset = firstOamIndex << 3;
        oamEntry = (OamByte *) (workingOffset + positionValue);
        oamXMask = 0x1FF;
        if (firstOamIndex) {
          countSource = nextOamIndex;
        } else {
          countSource = nextOamIndex;
        }
        secondaryEntryCount = countSource - firstOamIndex;
        do
        {
          oamAttribute5 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute5;
          oamDataDestination += 2;
          cameraForMatrix = cameraTransform;
          *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((((s32) (cameraForMatrix->rotation << 0x10)) >> 0x14) + oamAttribute5) - 0x20);
          *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
          oamAttribute6 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute6;
          oamDataDestination += 2;
          positionValue = (s32) (cameraTransform->scale << 0x10);
          positionValue >>= 0x14;
          positionValue = oamAttribute6 + positionValue;
          positionValue -= 0x20;
          coordinateMask = oamXMask;
          positionValue &= coordinateMask;
          oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
          preservedOamBits = 0xFFFFFE00;
          preservedOamBits &= oamAttribute2;
          *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
          positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
          currentOamBytes = (u8 *) oamEntry;
          *((u8 *) (((u8 *) currentOamBytes) + 3)) = (u8) (oamAttributeMask & positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          oamDataDestination += 4;
          oamEntry += 8;
          secondaryEntryCount -= 1;
        }
        while (secondaryEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
      ballMatrixCosine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      affineMatrix[0] = (u16) FixedMul((s32) ballMatrixCosine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixSine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePb = (u16) FixedMul((s32) ballMatrixSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixNegativeSine = 0 - sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePc = (u16) FixedMul((s32) ballMatrixNegativeSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixCosineD = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      ballAffinePd = (*affinePd = FixedMul((s32) ballMatrixCosineD, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale))));
      oamBuffer = (OamByte *) gOamBuffer;
      *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) (affinePaValue = affineMatrix[0]);
      *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
      *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
      *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = ballAffinePd;
    }
    else
    {
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (gMinigameState == 7)
      {
        dataPointer = (OamByte *) (&gHomerunBallRenderState);
        positionValue = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
        *((u16 *) (((u8 *) dataPointer) + 0)) = positionValue;
        workingOffset = (u32) sHomerunBallPitchDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
        affinePaValue = (u16) positionValue;
        objectState = dataPointer;
        if (((u32) workingOffset) < ((u32) affinePaValue))
        {
          *((u16 *) (((u8 *) objectState) + 0)) = 0U;
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
          frameTimeA = sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
          animationMatrixPc = (u16) frameTimeA;
          if (frameTimeA == 0)
          {
            *((u16 *) (((u8 *) objectState) + 2)) = animationMatrixPc;
          }
        }
        ballFrameData = sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam;
        nextOamIndex += *ballFrameData;
        frameData = ballFrameData + 1;
        stackPointer = ((u8 *)affineMatrix);
        affinePointerReference = (u16 **) (&stackPointer);
        affinePb = (u16 *) (((u8 *) (*affinePointerReference)) + 2);
        affinePc = (u16 *) (((u8 *) (*affinePointerReference)) + 4);
        affinePd = (u16 *) (((u8 *) (*affinePointerReference)) + 6);
        clearObjectModeMask = -4;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamAttributeMask = 0xFFFFFE00;
          workingOffset = firstOamIndex << 3;
          ballOamBytes = (OamByte *) (((s32) workingOffset) + positionValue);
          cameraTransform = (struct HomerunCameraState *) 0x1FF;
          oamByteMask = -0xD;
          cameraEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamAttribute3 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute3;
            oamDataDestination += 2;
            *((s8 *) (((u8 *) ballOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 0xA))) + oamAttribute3);
            *((u8 *) (((u8 *) ballOamBytes) + 1)) = (u8) (clearObjectModeMask & (*((u8 *) (((u8 *) ballOamBytes) + 1))));
            oamCoordinate4 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate4;
            oamDataDestination += 2;
            positionValue = *((s16 *) (((u8 *) objectState) + 8));
            positionValue = oamCoordinate4 + positionValue;
            positionValue &= (s32) cameraTransform;
            oamAttribute = *((u16 *) (((u8 *) ballOamBytes) + 2));
            preservedOamBits = oamAttributeMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) ballOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) ballOamBytes) + 5));
            *((u8 *) (((u8 *) ballOamBytes) + 5)) = (u8) (oamByteMask & positionValue);
            oamDataDestination += 4;
            ballOamBytes += 8;
            cameraEntryCount -= 1;
          }
          while (cameraEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
      }
    }
    clearOamXMask = 0xFFFFFE00;
    oamAttributeMask = gHomerunMissCount;
    if (((s32) oamAttributeMask) > 2)
    {
      oamAttributeMask = 2;
    }
    objectState = 0;
    if (((s32) objectState) < ((s32) oamAttributeMask))
    {
      do
      {
        nextOamIndex += *sHomerunStaticOam;
        frameData = sHomerunStaticOam + 1;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamByteMask = -0xD;
          workingOffset = firstOamIndex << 3;
          secondaryOamBytes = (OamByte *) (workingOffset + positionValue);
          secondaryEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamCoordinate7 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate7;
            oamDataDestination += 2;
            workingOffset = oamCoordinate7 + 0x20;
            *((s8 *) (((u8 *) secondaryOamBytes) + 0)) = (s8) (workingOffset - (((s32) objectState) * 8));
            oamAttribute5 = *(frameData++);
            oamCoordinate8 = oamAttribute5;
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate8;
            oamDataDestination += 2;
            positionValue = oamCoordinate8 + 0xE0;
            preservedOamBits = 0x1FF;
            positionValue &= preservedOamBits;
            oamAttribute = *((u16 *) (((u8 *) secondaryOamBytes) + 2));
            preservedOamBits = clearOamXMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) secondaryOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) secondaryOamBytes) + 5));
            *((u8 *) (((u8 *) secondaryOamBytes) + 5)) = (u8) ((oamByteMask & positionValue) | 8);
            oamDataDestination += 4;
            secondaryOamBytes += 8;
            secondaryEntryCount -= 1;
          }
          while (secondaryEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
        objectState = (OamByte *) (((s32) objectState) + 1);
      }
      while (((s32) objectState) < ((s32) oamAttributeMask));
    }
    if (gMinigameMedalHighlight != 0)
    {
      uiAnimationTimer = gMinigameUiAnimation.animationTimer + 1;
      gMinigameUiAnimation.animationTimer = uiAnimationTimer;
      if (((u32) sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time) < ((u32) ((u16) uiAnimationTimer)))
      {
        gMinigameUiAnimation.animationTimer = 1U;
        gMinigameUiAnimation.animationFrame = (u16) (gMinigameUiAnimation.animationFrame + 1);
        frameTimeC = sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time;
        if (frameTimeC == 0)
        {
          gMinigameUiAnimation.animationFrame = (u16) frameTimeC;
          gMinigameUiAnimation.animationTimer = (u16) frameTimeC;
          gMinigameMedalHighlight = 0;
          DrawHomerunDerbyScore();
        }
      }
    }
    oamXMaskValue = 0x1FF;
    batFrameData = sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].oam;
    nextOamIndex += *batFrameData;
    frameData = batFrameData + 1;
    cameraTransform = (struct HomerunCameraState *) &gHomerunBatAnimation;
    activeAnimation = sHomerunBallIdleAnimation;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      oamAttributeMask = 0x1FF;
      oamByteMask = -0xD;
      workingOffset = firstOamIndex << 3;
      batOamBytes = (OamByte *) (workingOffset + positionValue);
      firstOamIndex = nextOamIndex - firstOamIndex;
      do
      {
        oamCoordinate9 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate9;
        oamDataDestination += 2;
        workingOffset = oamCoordinate9;
        workingOffset += 0x92;
        *((s8 *) (((u8 *) batOamBytes) + 0)) = (s8) workingOffset;
        oamCoordinate10 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate10;
        oamDataDestination += 2;
        positionValue = oamCoordinate10 + 0x18;
        positionValue &= oamAttributeMask;
        clearOamXMask = 0xFFFFFE00;
        *((u16 *) (((u8 *) batOamBytes) + 2)) = (u16) ((clearOamXMask & (*((u16 *) (((u8 *) batOamBytes) + 2)))) | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        positionValue = *((u8 *) (((u8 *) batOamBytes) + 5));
        *((u8 *) (((u8 *) batOamBytes) + 5)) = (oamByteMask & positionValue) | 8;
        oamDataDestination += 4;
        batOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    activeAnimationState = (struct HomerunSimpleAnimation *) cameraTransform;
    animationTimer = activeAnimationState->animationTimer + 1;
    activeAnimationState->animationTimer = animationTimer;
    currentFrameDuration = (u32) activeAnimation[activeAnimationState->animationFrame].time;
    if (currentFrameDuration < ((u32) ((u16) animationTimer)))
    {
      activeAnimationState->animationTimer = 0U;
      activeAnimationState->animationFrame = (u16) (activeAnimationState->animationFrame + 1);
      frameTimeD = activeAnimation[activeAnimationState->animationFrame].time;
      if (frameTimeD == 0)
      {
        activeAnimationState->animationFrame = (u16) frameTimeD;
      }
    }
    cameraForMatrix = cameraTransform;
    activeFrameData = activeAnimation[((struct HomerunSimpleAnimation *) cameraForMatrix)->animationFrame].oam;
    nextOamIndex += *activeFrameData;
    frameData = activeFrameData + 1;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      {
        register u32 maskSeed asm("r2");
        maskSeed = 0xFFFFFE00;
        oamAttributeMask = maskSeed;
      }
      workingOffset = firstOamIndex << 3;
      staticOamBytes = (OamByte *) (workingOffset + positionValue);
      objectState = (OamByte *) oamXMaskValue;
      {
        register u8 negativeMaskSeed asm("r0") = 15;
        oamByteMask = -(int)negativeMaskSeed;
      }
      {
        register s32 nextCount asm("r1") = nextOamIndex;
        firstOamIndex = nextCount - firstOamIndex;
      }
      do
      {
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        workingOffset = oamCoordinate12;
        workingOffset += 0x60;
        *((u8 *) (((u8 *) staticOamBytes) + 0)) = (u8) workingOffset;
        positionValue = *((u8 *) (((u8 *) staticOamBytes) + 1));
        *((u8 *) (((u8 *) staticOamBytes) + 1)) = (u8) ((clearObjectModeMask & positionValue) | 1);
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        positionValue = oamCoordinate12 + 0xD7;
        positionValue &= (s32) objectState;
        oamAttribute = *((u16 *) (((u8 *) staticOamBytes) + 2));
        preservedOamBits = oamAttributeMask;
        preservedOamBits &= oamAttribute;
        *((u16 *) (((u8 *) staticOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
        pitcherAnimationType = *((u8 *) (((u8 *) staticOamBytes) + 3));
        workingOffset = oamXMask;
        workingOffset &= pitcherAnimationType;
        *((u8 *) (((u8 *) staticOamBytes) + 3)) = (u8) (workingOffset | 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (((u8 *) staticOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) staticOamBytes) + 5))) | 0xC);
        oamDataDestination += 4;
        staticOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    savedOamIndex = nextOamIndex;
    cameraData = cameraTransform;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPa = sSinCosTable[(*((u16 *) (((u8 *) dataPointer) + 4))) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) animationMatrixPa, (s32) ((s16) FixedInverse(0x100)));
    objectState = (OamByte *) 0x100;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPb = sSinCosTable[*((u16 *) (((u8 *) dataPointer) + 4))];
    *affinePb = (u16) FixedMul((s32) animationMatrixPb, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (frameData = (u16 *) cameraData);
    animationMatrixPc = 0 - sSinCosTable[frameData[2]];
    *affinePc = (u16) FixedMul((s32) animationMatrixPc, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (u16 *) cameraData;
    animationMatrixPd = sSinCosTable[frameData[2] + 0x40];
    frameTimeValue = (*affinePd = (u16) FixedMul((s32) animationMatrixPd, (s32) ((s16) FixedInverse((s32) objectState))));
    oamBufferPointer = (u8 *) ((OamByte *) gOamBuffer);
    oamAttribute14 = (affinePaValue = affineMatrix[0]);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x26)) = (u16) oamAttribute14;
    oamAttribute11 = *affinePb;
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x2E)) = (u16) oamAttribute11;
    *((u16 *) (((u8 *) oamBufferPointer) + 0x36)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x3E)) = frameTimeValue;
    if (gHomerunBonusRender.animationIndex == 0)
    {
    }
    else
    {
      bonusAnimationIndex = gHomerunBonusRender.animationIndex;
      bonusAnimationFrames = sHomerunBonusAnimations[bonusAnimationIndex];
      bonusOamBytes = (OamByte *) bonusAnimationFrames;
      bonusAnimationTimer = gHomerunBonusRender.animationTimer + 1;
      gHomerunBonusRender.animationTimer = bonusAnimationTimer;
      frameDuration = (u32) ((struct AnimationFrame *) ((gHomerunBonusRender.animationFrame << 3) + (u32) bonusAnimationFrames))->time;
      if (frameDuration < ((u32) ((u16) bonusAnimationTimer)))
      {
        gHomerunBonusRender.animationTimer = 1U;
        bonusAnimation = bonusAnimationFrames;
        gHomerunBonusRender.animationFrame = (u16) (gHomerunBonusRender.animationFrame + 1);
        if ((byteValue = bonusAnimation[gHomerunBonusRender.animationFrame].time) == 0)
        {
          gHomerunBonusRender.animationFrame = 0;
          animationType = gMinigameSequenceState;
          if (animationType == 1)
          {
            if (!(animationType & ((u32 (*)(void)) MinigameRandom)()))
            {
              m4aSongNumStart(0x237U);
            }
            else
            {
              m4aSongNumStart(0x238U);
            }
            m4aSongNumStart(0x2CCU);
            gHomerunBonusRender.animationIndex = (u8) gHomerunBonusRender.nextAnimationIndex;
            gHomerunBonusRender.animationTimer = 0U;
            gMinigameSequenceState += 1;
          }
        }
      }
      bonusOamBytes = (OamByte *) sHomerunBonusAnimations[(*bonusRenderState).animationIndex];
      bonusFrameData = ((struct AnimationFrame *) bonusOamBytes)[gHomerunBonusRender.animationFrame].oam;
      nextOamIndex += *bonusFrameData;
      frameData = bonusFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        OamByte *render_state;
        positionValue = (s32)gOamBuffer;
        render_state = (OamByte *) (&gHomerunBonusRender);
        cameraTransform = (struct HomerunCameraState *) 0x1FF;
        oamXMask = 0xFFFFFE00;
        bonusOamBytes = (OamByte *)((firstOamIndex << 3) + positionValue);
        bonusEntryCount = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute13 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute13;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) bonusOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) render_state) + 6))) + oamAttribute13);
          oamAttribute2 = *(frameData++);
          *((u16 *)oamDataDestination) = oamAttribute2;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) render_state) + 4));
          positionValue = oamAttribute2 + positionValue;
          positionValue &= (s32) cameraTransform;
          oamAttribute = *((u16 *) (((u8 *) bonusOamBytes) + 2));
          preservedOamBits = oamXMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) bonusOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          *((u8 *) (((u8 *) bonusOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) bonusOamBytes) + 5))) | 0xC);
          oamDataDestination += 4;
          bonusOamBytes += 8;
          bonusEntryCount -= 1;
        }
        while (bonusEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
    }
    pitcherAnimation = (OamByte *) sHomerunPitcherAnimations[gHomerunPitcherAnimation.animationIndex];
    pitcherAnimationTimer = gHomerunPitcherAnimation.animationTimer + 1;
    gHomerunPitcherAnimation.animationTimer = pitcherAnimationTimer;
    if (((u32) ((struct AnimationFrame *) pitcherAnimation)[gHomerunPitcherAnimation.animationFrame].time) >= ((u32) ((u16) pitcherAnimationTimer)))
    {
    }
    else
    {
      gHomerunPitcherAnimation.animationTimer = 0U;
      gHomerunPitcherAnimation.animationFrame = (u16) (gHomerunPitcherAnimation.animationFrame + 1);
      if (gHomerunPitcherAnimation.animationIndex == 4)
      {
        switch (gHomerunPitcherAnimation.animationFrame)
        {
          case 10:
            gMinigameState = 7;
            m4aSongNumStart(sHomerunPitchSounds[gHomerunPitchPath * 2]);
            break;
            pitchSoundId = 0x22BU;

          case 0:
            m4aSongNumStart(0x228U);
            break;

          case 1:
            m4aSongNumStart(0x229U);
            break;

          case 5:
            m4aSongNumStart(0x22AU);
            break;

          case 8:
            m4aSongNumStart(0x22BU);
            break;

        }

      }
      pitcherState = (struct HomerunPitcherState *) &gHomerunPitcherAnimation;
      pitcherFrameTime = ((struct AnimationFrame *) pitcherAnimation)[pitcherState->animationFrame].time;
      if (pitcherFrameTime == 0)
      {
        pitcherState->animationFrame = (u16) pitcherFrameTime;
        pitcherAnimationIndex = pitcherState->animationIndex;
        if (pitcherAnimationIndex == 4)
        {
          pitcherState->animationIndex = 5U;
          pitcherAnimation = (OamByte *) sHomerunPitcherAnimations[5];
        }
        else
          if (((u32) ((u8) (pitcherAnimationIndex - 1))) <= 3U)
        {
          pitcherState->pitchCounter = (u8) (pitcherState->pitchCounter + 1);
          if (gDifficulty == 2)
          {
            dataPointer = &sHomerunPitchAnimationsHard;
          }
          else
          {
            dataPointer = &sHomerunPitchAnimationsNormal;
          }
          entryCounter = (gHomerunPitchPath * 5) - 1;
          pitcherState->animationIndex = (u8) (*((u8 *) ((entryCounter + pitcherState->pitchCounter) + ((s32) dataPointer))));
          dataPointer = (OamByte *) sHomerunPitcherAnimations;
          objectState = (OamByte *) &gHomerunPitcherAnimation;
          pitcherAnimationType = *((u8 *) (((u8 *) objectState) + 4));
          pitcherAnimation = (OamByte *) ((struct AnimationFrame **) dataPointer)[pitcherAnimationType];
          if (pitcherAnimationType == 3)
          {
            m4aSongNumStart(0x226U);
          }
          if ((*((u8 *) (((u8 *) objectState) + 4))) == 2)
          {
            m4aSongNumStart(0x227U);
          }
          oamXMaskPointer = (struct HomerunCameraState *) 0x1FF;
        }
      }
    }
    pitcherFrameData = ((struct AnimationFrame *) pitcherAnimation)[((struct HomerunPitcherState *) &gHomerunPitcherAnimation)->animationFrame].oam;
    nextOamIndex += *pitcherFrameData;
    frameData = pitcherFrameData + 1;
    if (((s32) firstOamIndex) < ((s32) nextOamIndex))
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      objectState = (OamByte *) 0x1FF;
      cameraTransform = (struct HomerunCameraState *) 0xFFFFFE00;
      workingOffset = firstOamIndex << 3;
      pitcherOamEntry = (OamByte *) (workingOffset + positionValue);
      positionValue = nextOamIndex;
      pitcherEntryCount = positionValue - firstOamIndex;
      do
      {
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        workingOffset = oamCoordinate15;
        workingOffset += 0x58;
        *((u8 *) (((u8 *) pitcherOamEntry) + 0)) = (u8) workingOffset;
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        positionValue = oamCoordinate15 + 0x48;
        positionValue &= (s32) objectState;
        pitcherOamBytes = (u8 *) pitcherOamEntry;
        oamAttribute = *((u16 *) (pitcherOamBytes + 2));
        preservedOamBits = (s32) cameraTransform;
        preservedOamBits &= oamAttribute;
        *((u16 *) (pitcherOamBytes + 2)) = (u16) (preservedOamBits | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (pitcherOamBytes + 5)) = (u8) ((*((u8 *) (pitcherOamBytes + 5))) | 0xC);
        oamDataDestination += 4;
        pitcherOamEntry += 8;
        pitcherEntryCount -= 1;
      }
      while (pitcherEntryCount != 0);
      firstOamIndex = nextOamIndex;
    }
    if (((u32) ((u8) (gMinigameState - 0xB))) <= 2U)
    {
      resultAnimation = (OamByte *) sHomerunResultAnimations[gHomerunPitchResult];
      resultAnimationTimer = gHomerunResultRender.animationTimer + 1;
      if (1)
      {
        gHomerunResultRender.animationTimer = resultAnimationTimer;
      }
      if (((u32) ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time) < ((u32) ((u16) resultAnimationTimer)))
      {
        gHomerunResultRender.animationTimer = 0U;
        gHomerunResultRender.animationFrame = (u16) (gHomerunResultRender.animationFrame + 1);
        resultFrameTime = ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time;
        if (resultFrameTime == 0)
        {
          gHomerunResultRender.animationFrame = (u16) resultFrameTime;
        }
      }
      resultFrameData = ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].oam;
      nextOamIndex += *resultFrameData;
      frameData = resultFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        objectState = (OamByte *) (&gHomerunResultRender);
        oamXMask = 0x1FF;
        oamAttributeMask = 0xFFFFFE00;
        clearAffineModeMask = -0xD;
        workingOffset = firstOamIndex << 3;
        resultOamBytes = (OamByte *) (workingOffset + positionValue);
        entryCounter = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute16 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute16;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) resultOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 6))) + oamAttribute16);
          oamCoordinate17 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate17;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) objectState) + 4));
          positionValue = oamCoordinate17 + positionValue;
          positionValue &= oamXMask;
          oamAttribute = *((u16 *) (((u8 *) resultOamBytes) + 2));
          preservedOamBits = oamAttributeMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) resultOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          positionValue = *((u8 *) (((u8 *) resultOamBytes) + 5));
          *((u8 *) (((u8 *) resultOamBytes) + 5)) = (u8) (clearAffineModeMask & positionValue);
          oamDataDestination += 4;
          resultOamBytes += 8;
          entryCounter -= 1;
        }
        while (entryCounter != 0);
      }
    }
  }
  gOamSlotsUsed = nextOamIndex;
}
asm(".set $t.hd_085c, DrawHomerunDerby + 0x85c\n"
    ".set $d.hd_0860, DrawHomerunDerby + 0x860\n"
    ".set $t.hd_0914, DrawHomerunDerby + 0x914\n"
    ".set $d.hd_0918, DrawHomerunDerby + 0x918\n"
    ".set $d.hd_0958, DrawHomerunDerby + 0x958\n"
    ".set $t.hd_095c, DrawHomerunDerby + 0x95c\n"
    ".set $d.hd_0aa8, DrawHomerunDerby + 0xaa8\n"
    ".set $t.hd_0aac, DrawHomerunDerby + 0xaac\n"
    // .set copies DrawHomerunDerby's type and size; these are mapping symbols, not functions
    ".type $t.hd_085c, %notype\n.size $t.hd_085c, 0\n"
    ".type $d.hd_0860, %notype\n.size $d.hd_0860, 0\n"
    ".type $t.hd_0914, %notype\n.size $t.hd_0914, 0\n"
    ".type $d.hd_0918, %notype\n.size $d.hd_0918, 0\n"
    ".type $d.hd_0958, %notype\n.size $d.hd_0958, 0\n"
    ".type $t.hd_095c, %notype\n.size $t.hd_095c, 0\n"
    ".type $d.hd_0aa8, %notype\n.size $d.hd_0aa8, 0\n"
    ".type $t.hd_0aac, %notype\n.size $t.hd_0aac, 0");

s32 UpdateHomerunScoreSequence(void)
{
    u8 *sequenceStatePointer;
    s32 sequenceState;

    sequenceStatePointer = &gMinigameSequenceState;
    sequenceState = *sequenceStatePointer;

    if (sequenceState == 1) {
        goto state1;
    }
    if (sequenceState > 1) {
        goto stateGt;
    }
    if (sequenceState == 0) {
        goto state0;
    }
    goto ret0;

stateGt:
    if (sequenceState == 2) {
        goto state2;
    }
    goto ret0;

state0:
    if (MinigameWaitForFrames(60) != 0) {
        gMinigameScore++;
        if ((u16)gMinigameScore > 999) {
            gMinigameScore = 999;
        } else {
            m4aSongNumStart(0x224);
        }

        if (gMinigameScore > gMinigameHighScores) {
            gMinigameNewHighScore = 1;
            gMinigameHighScores = gMinigameScore;
        }
        DrawHomerunDerbyScore();
        goto incState;
    }
    goto ret0;

state1:
    if (MinigameWaitForFrames(20) != 0) {
        if ((gMinigameScore % 3U) == 0) {
            gMedalCount++;
            if ((u16)gMedalCount > 999) {
                gMedalCount = 999;
            } else {
                m4aSongNumStart(0x223);
                gMinigameMedalHighlight = sequenceState;
            }
            DrawHomerunDerbyScore();
        }
incState:
        gMinigameSequenceState++;
    }
    goto ret0;

state2:
    if (MinigameWaitForFrames(30) != 0) {
        gMinigameSequenceTimer = 0;
        *sequenceStatePointer = 0;
        return 1;
    }

ret0:
    return 0;
}

s32 UpdateHomerunBonusAnimation(void)
{
    u8 *sequenceStatePointer;
    u8 *sequenceStatePointerReload;
    struct HomerunBonusState *bonusState;
    s32 yPosition;
    u16 nextYPosition;
    s32 animationIndex;
    s32 sequenceState;
    s32 minimumAnimation;

    sequenceStatePointer = &gMinigameSequenceState;
    sequenceState = *sequenceStatePointer;
    sequenceStatePointerReload = sequenceStatePointer;

    switch (sequenceState) {
    case 0:
    {
        struct HomerunBonusState *movingBonusState;
        s32 movementDirection;
        register struct HomerunBonusState *centeredBonusState asm("r5");

        movingBonusState = (struct HomerunBonusState *)gHomerunBonusAnimation;
        movementDirection = movingBonusState->direction;
        bonusState = movingBonusState;
        if (movementDirection != 0) {
            bonusState->yPosition++;
            if (bonusState->yPosition > 0x50) {
                bonusState->yPosition = 0x50;
            }
        } else {
            bonusState->yPosition--;
            if (bonusState->yPosition <= 0x4F) {
                bonusState->yPosition = 0x50;
            }
        }

        centeredBonusState = bonusState;
        if (centeredBonusState->yPosition == 0x50) {
            (*sequenceStatePointerReload)++;
            centeredBonusState->animationTimer = 0;
            centeredBonusState->animationFrame = 0;

            animationIndex = centeredBonusState->animationIndex;
            if (animationIndex != 4) {
                if (animationIndex <= 4) {
                    if (animationIndex < (minimumAnimation = 1)) {
                        break;
                    }
                    centeredBonusState->animationIndex = MinigameRandom() % 2 + 7;
                }
            } else {
                bonusState->animationIndex = MinigameRandom() % 2 + 5;
            }
        }
        break;
    }

    case 1:
        break;

    case 2:
    {
        struct HomerunBonusState *returningBonusState;
        register s32 direction asm("r4");

        returningBonusState = (struct HomerunBonusState *)gHomerunBonusAnimation;
        direction = returningBonusState->direction;
        if (direction != 0) {
            register u16 *timerOutput asm("r1");

            nextYPosition = returningBonusState->yPosition + 1;
            direction = 0;
            returningBonusState->yPosition = nextYPosition;
            yPosition = returningBonusState->yPosition;
            if (yPosition > sHomerunBonusYLimits[returningBonusState->direction - 1]) {
                timerOutput = &gMinigameSequenceTimer;
                *timerOutput = 0;
                *sequenceStatePointer = direction;
                returningBonusState->animationIndex = direction;
                return 1;
            }
        } else {
            register u8 zeroValue asm("r6");

            nextYPosition = returningBonusState->yPosition - 1;
            zeroValue = 0;
            returningBonusState->yPosition = nextYPosition;
            yPosition = returningBonusState->yPosition;
            if (yPosition < sHomerunBonusYLimits[returningBonusState->direction + 1]) {
                gMinigameSequenceTimer = direction;
                *sequenceStatePointer = zeroValue;
                returningBonusState->animationIndex = zeroValue;
                return 1;
            }
        }
        break;
    }
    }

    return 0;
}

```

### Remaining objdump display diff

```diff
--- 

+++ 

@@ -685,4 +685,4 @@

 ldr	r1, [pc, #352]	@ (153c <DrawHomerunDerby+0x710>)
-ldr	r2, [pc, #356]	@ (1540 <DrawHomerunDerby+0x714>)
-mov	ip, r2
+ldr	r0, [pc, #356]	@ (1540 <DrawHomerunDerby+0x714>)
+mov	ip, r0
 lsls	r0, r7, #3
@@ -834,3 +834,4 @@

 ldrh	r0, [r2, #2]
-.word	0x190000c0
+lsls	r0, r0, #3
+adds	r0, r0, r4
 ldrb	r0, [r0, #4]
@@ -977,4 +978,2 @@

 mov	pc, r0
-asrs	r4, r0, #17
-lsls	r0, r0, #12
 .word	0x000001ff
@@ -1037,5 +1036,2 @@

 .word	0x0000022b
-ldr	r2, [pc, #64]	@ (1784 <DrawHomerunDerby+0x958>)
-lsls	r0, r0, #12
-.word	0x00000000
 subs	r0, #1
@@ -1234,3 +1230,2 @@

 .short	0x0000
-.word	0x03004a10
 .word	0x00000226
```


## GPT-6 hypothesis: reuse positionValue/r1 in Q (2026-10-16)

Confirmed not previously recorded/tested in the report or saved candidate notes. This is a single new C candidate, not R: start from Q (duplicated static maskSeed initialization), replace only the block-local nextCount assignment/subtraction with:

```c
      positionValue = nextOamIndex;
      firstOamIndex = positionValue - firstOamIndex;
```

Result: **99.7%, score169**, exactly Q's result. Complete emitted function assembly is identical to Q after normalizing only generated .LCB<number> long-branch label names. All other instruction operands, literals and labels match Q's compiler output.

| Check | Outcome |
|---|---|
| Static mask load | Correct: ldr r2; mov ip,r2 |
| r8 iteration-count transfer | Incorrect: mov r0,r8; sub r7,r0,r7, target uses r1 |
| Following affine reload sequence | Still Q's shifted sequence, first camera copy r1 vs targetr2 |
| All other matching instructions | Same as Q; best's downstream exact registers are not preserved |

### Exact first divergence and compiler evidence

The reuse does not survive combine. Q and experiment enter CSE with exactly the same count operations (only instruction IDs differ):

```
Q:       insn2171 r1 = r8; insn2173 pseudo128 = r1 - pseudo128
Reuse:   insn2170 r1 = r8; insn2172 pseudo128 = r1 - pseudo128
```

Combine deletes the first copy in both:
```
Q:       note2171 NOTE_INSN_DELETED
         insn2173 (set (reg/v:SI 128)
                      (minus:SI (reg/v:SI 8 r8) (reg/v:SI 128)))
Reuse:   note2170 NOTE_INSN_DELETED
         insn2172 (set (reg/v:SI 128)
                      (minus:SI (reg/v:SI 8 r8) (reg/v:SI 128)))
```

Thus reusing an existing hard-r1 C variable has no surviving allocation constraint at the subtraction. No C-source lifetime distinction remains here to distinguish it from Q.

Global-allocation dump mappings for the relevant pseudos are identical across score93, Q and reuse: pseudo33 (mask) ->12/ip; pseudo128 (firstOamIndex) ->7; pseudo137 (static OAM pointer) ->4. This is not a change to those pseudo assignments.

Score93's mask is a single pseudo33=-512 definition before reload. The .greg dump shows an inserted reload:
```
insn4185 r0 = -512
insn2130 ip = r0
```

Q/reuse instead retain ordinary hard-r2 constant definitions in both mask arms (insn2131 and2139), followed by insn2144 ip=r2. No mask scratch-register allocation is required there. Late branch merging removes duplicate mask paths, but this happens after reload.

After the mask, the negative-mask calculation still emits the same neg r0,r0; mov sl,r0. The **first newly emitted reload with a different register after the mask** is the loop-count input reload:

```
score93 .greg:
  insn4191 (set (reg:SI 1 r1) (reg/v:SI 8 r8))
  insn2159 (set (reg/v:SI 7 r7)
                (minus:SI (reg:SI 1 r1) (reg/v:SI 7 r7)))

Q .greg:
  insn4202 (set (reg:SI 0 r0) (reg/v:SI 8 r8))
  insn2173 (set (reg/v:SI 7 r7)
                (minus:SI (reg:SI 0 r0) (reg/v:SI 7 r7)))

reuse .greg:
  insn4199 (set (reg:SI 0 r0) (reg/v:SI 8 r8))
  insn2172 (set (reg/v:SI 7 r7)
                (minus:SI (reg:SI 0 r0) (reg/v:SI 7 r7)))
```

Compiler source tools/agbcc/gcc/reload1.c explicitly implements round-robin spill-register selection: allocate_reload_reg begins from last_spill_reg, advances through spill_regs, and updates last_spill_reg=i on selection (line5220 in this checkout). The single extra mask reload in score93 consumes a selection; Q/reuse eliminate that event. The count reload and later unconstrained matrix reloads therefore differ despite unchanged relevant global assignments. This agrees with R: preserving the hard-r1 count copy fixes that instruction but removes another reload event, rotating subsequent matrix temporaries further.

Compiler files were read only. Existing flags were unchanged. Diagnostic -da dumps were generated separately without editing Makefile/configuration/compiler.

### Artifacts and preservation

%TEMP%/wl4-derby-codex (local scratch, not in the repo):
- before-r1-experiment.c: exact whole-file snapshot of score93 before this experiment.
- score93-full.c, Qcompare-full.c, reuseR1-full.c: comparison inputs.
- score93/Qcompare/reuseR1.i and .s, with .rtl/.cse/.combine/.lreg/.greg/.jump2 and other -da dumps.
- score93/Qcompare/reuseR1-function-<pass>: exact function dump extracts.
- reuseR1-vs-Q-asm.diff: only generated long-branch-label numbering differences.

One new candidate was tested with compile_and_diff, then score93 restored and reverified. Three separate diagnostic compiler invocations produced the comparison dumps: five compiler invocations total including candidate and restoration. No further C changes were tested. Source restored exactly against the pre-experiment snapshot; unrelated source preserved. No commit or match claim.

Next investigation should target preservation/compensation of the missing mask reload event, rather than another spelling of a same-block hard-r1 copy: combine demonstrably folds both spellings into the same subtraction.


## Reload cursor trace and byte-exact solution (2026-10-16)

**Byte-for-byte match achieved:** the candidate's entire 2792-byte function equals the original ROM at **0x0808EDFC**, including relocated instructions and literal pools. SHA256: `f2c26b48b0d7dc96c5cc06dc99b11c61c43b507df45b3ba4656185048d08633e`.

MCP/objdiff still reports **99.8%, score87**, solely because the expected object classifies the bytes `c0 00 00 19` as `.word 0x190000c0`, while the compiler classifies them as `lsl r0,#3; add r0,r4`. Those are identical bytes. No expected-object metadata was changed or forced match status requested. Report raw-byte success separately from this outstanding UI classification discrepancy.

### Actual allocator observation

The existing compiler executable has debug symbols but gdb/lldb are unavailable. A small temporary Python ptrace debugger observed allocate_reload_reg entry/return, reading current_function_name, chain->insn UID, spill_regs, last_spill_reg and reload_reg_rtx. It patched breakpoints only in the diagnostic child process memory; compiler files, flags, headers and source tools were not edited. Two diagnostic compiler executions used the already-saved preprocessed score93 and Q inputs. One subsequent C candidate was compiled through MCP: **three compiler invocations total**, no brute-force variations.

In all observed events, spill_regs = [r0,r1,r2,r3], so cursor index equals register number. These are measured before/after cursor values, not inferred from final assembly.

| Input/block | Event / input RTL UID | Selected register | Cursor before -> after |
|---|---|---|---|
| score93 static | -512 mask materialization, 2130 | r0 | 2 -> 0 |
| score93 static | copy mask to ip | r0 -> ip, same reload | unchanged at0 |
| score93 static | neg15 / copy result to sl | existing r0, no allocate_reload_reg call | unchanged at0 |
| score93 static | r8 count input, 2159 | r1 | 0 -> 1 |
| score93 first affine | camera base, 2324 | r2 | 1 -> 2 |
| score93 first affine | table address, 2329 | r3 | 2 -> 3 |
| score93 first affine | signed-halfword scratch, 2338 | r1 | 3 -> 1 |
| Q static | -512 definitions, 2131/2139 | ordinary hard r2 | no allocation; remains2 |
| Q static | copy mask to ip,2144 | ordinary r2 -> ip | no allocation; remains2 |
| Q static | neg15 / copy result to sl | existing r0, no allocate_reload_reg call | unchanged at2 |
| Q static | r8 count input,2173 | r0 | 2 -> 0 |
| Q first affine | camera base,2338 | r1 | 0 -> 1 |
| Q first affine | table address,2343 | r2 | 1 -> 2 |
| Q first affine | signed-halfword scratch,2352 | r3 | 2 -> 3 |
| score93 pitcher | -512 mask,3437 | r0, then copied to r9 | 2 -> 0 |
| score93 pitcher | r8 count,3448 | r1 | 0 -> 1 |
| Q pitcher | -512 mask,3451 | r0, then copied to r9 | 2 -> 0 |
| Q pitcher | r8 count,3462 | r1 | 0 -> 1 |

This corrects any implication that the pitcher source-level r1 assignment survived: both copies are deleted by combine. Pitcher's matching r1 is also a newly inserted reload.

An ordinary mask reload into r2 would update the cursor to2. With otherwise identical following state, the count would then choose r0 rather thanr1. Therefore simply forcing the mask's allocation choice cannot solve both requirements. Another existing useful operation must consume the missing cursor transition, or the r2 materialization must avoid the ordinary allocation path.

### Evidence-backed candidate

Started from Q, retaining its duplicated hard-r2 mask initialization. Replaced only:
```c
      {
        register u8 negativeMaskSeed asm("r0") = 15;
        oamByteMask = -(int)negativeMaskSeed;
      }
```
with:
```c
      oamByteMask = -15;
```

Specific compiler evidence: tools/agbcc/gcc/thumb.md's movsi negative-constant define_split turns a constant in (-256,0) into a positive MOV followed by NEG in the same destination. Thus a direct high-register constant -15 can require an allocator-chosen low-register temporary yet emit the already-required `mov r0,#15; neg r0,r0; mov sl,r0`. Q's original byte-seed expression instead reuses its pre-existing r0 input without an allocate_reload_reg event.

The new source supplies the missing allocation naturally through useful existing work: materialize -15, not an artificial discarded operation. It restores the required sequence `ldr r2,mask; mov ip,r2; ... mov r0,#15; neg r0,r0; mov sl,r0; mov r1,r8; sub r7,r1,r7` and every affine register. The new candidate's cursor transition is supported by the previous runtime traces, machine-description split and exact emitted result; the candidate itself was not run through a fourth diagnostic compilation.

Rejected prior approaches without retesting: Q/Y duplicated mask alone removes an event; R removes another count event; S duplicates a QI variable and retains unwanted extension shifts; T/U manipulate affine copies/whole reads rather than the missing event; V leaves multiarm control flow; W/X do not change the relevant allocation; Z leaves branches/count restructuring. Reusing positionValue was already proved identical to Q after combine. This direct constant -15 operation was not among those failed variants.

### Two independent byte verifications

1. Parsed both ELF32 objects and compared all function bytes and relocation records. The only raw differences are three candidate-extra ABS32 relocations: function offsets0x85c gOamBuffer,0x914/0xABC gHomerunPitcherAnimation. Resolving them yields the expected words 0x03001444 and0x03004A10; all 2792 bytes then agree. Common relocation types, symbols and addends agree.
2. Located the expected function's unique 32-byte prologue in baserom_us.gba at0x8EDFC. Derived undefined symbol addresses consistently from the original ROM's target relocation sites. Linked expected and candidate objects independently with GNU ld at original unit text base0x0808DFD0, using temporary absolute symbol definitions with proper Thumb function types. Extracted both 2792-byte functions: **zero differences against the ROM in either**. Calls, data and function size all checked, not only executable instruction rows.

The existing build ELF places this function at a different address (0x0808E658), so it was not used as authority for original code addresses. An initial comparison at that stale build address correctly failed; resolving against original ROM addresses and proper Thumb symbol types produced the final exact comparison.

Artifacts under %TEMP%/wl4-derby-codex (local scratch, not in the repo): trace_reload.py; trace-score93.json; trace-Qcompare.json; byte-exact-best.c; before-r1-experiment.c (score93 whole-file backup); cursor-candidate-raw-verification.txt; cursor-independent-verification.txt; rom-verify-{expected,candidate}.elf/.text; original-rom-symbols.s/.o/.json. Symbol helper assembly contains only symbol definitions and no instructions; no inline assembly was added to C.

Score93 remains preserved in the earlier report and whole-file snapshot. The working source now contains the verified byte-exact improvement. Outside DrawHomerunDerby, source text is unchanged against that snapshot. Commit staging must exclude the pre-existing whole-file line-ending differences and all unrelated working-tree modifications.

### Byte-exact implementation

```c
void DrawHomerunDerby(void)
{
  u16 affineMatrix[4];
  u16 *affinePb;
  u16 *affinePc;
  u16 *affinePd;
  register struct HomerunCameraState *cameraTransform asm("r9");
  struct HomerunCameraState *cameraForMatrix;
  register struct HomerunCameraState *cameraForRotation asm("r3");
  register struct HomerunPitcherState *pitcherState asm("r3");
  register struct HomerunSimpleAnimation *activeAnimationState asm("r3");
  struct AnimationFrame *activeAnimation;
  u32 animationCounterLimit;
  register u32 oamXMask asm("sl");
  s32 oamAttributeMask;
  register long preservedOamBits;
  register OamByte *dataPointer asm("r2");
  OamByte *oamBuffer;
  OamByte *oamEntry;
  register OamByte *objectState asm("r5");
  struct HomerunCameraState *cameraForSprites;
  s16 matrixCosine;
  s16 animationMatrixPb;
  s16 animationMatrixPc;
  s16 animationMatrixPd;
  struct HomerunCameraState *oamXMaskPointer;
  s16 matrixSine;
  s16 matrixNegativeSine;
  struct HomerunSimpleAnimation *bonusRenderState;
  s16 matrixCosineD;
  s16 ballMatrixCosine;
  s16 ballMatrixSine;
  s16 ballMatrixNegativeSine;
  int ballMatrixCosineD;
  s16 animationMatrixPa;
  register unsigned int oamByteMask asm("sl");
  register OamByte *resultAnimation asm("r4");
  register s32 workingOffset asm("r0");
  struct AnimationFrame *bonusAnimation;
  u32 frameDuration;
  OamByte *pitcherAnimation;
  s32 entryCounter;
  s32 cameraEntryCount;
  s32 secondaryEntryCount;
  s32 batEntryCount;
  register s32 bonusEntryCount asm("r7");
  s32 pitcherEntryCount;
  s32 savedOamIndex;
  struct AnimationFrame *bonusAnimationFrames;
  u16 *affinePointer;
  u16 **affinePointerReference;
  u16 *bonusFrameData;
  u16 *pitcherFrameData;
  u16 *resultFrameData;
  u16 *ballFrameData;
  u16 *secondaryFrameData;
  u16 *batFrameData;
  u16 *activeFrameData;
  register u16 *frameData asm("r3");
  u8 *pitcherOamBytes;
  struct HomerunCameraState *cameraData;
  int clearAffineModeMask;
  register OamByte *oamDataDestination asm("r6");
  u16 frameTimeValue;
  int clearOamXMask;
  u16 affinePdValue;
  s32 ballAffinePd;
  s32 uiAnimationTimer;
  s32 animationTimer;
  u16 affinePaValue;
  register s32 positionValue asm("r1");
  u8 *stackPointer;
  s32 frameCounter;
  s32 bonusAnimationTimer;
  s32 pitcherAnimationTimer;
  s32 resultAnimationTimer;
  register u16 oamAttribute asm("r2");
  register s32 oamCoordinate10 asm("r2");
  u16 oamAttribute11;
  register s32 oamCoordinate12 asm("r2");
  register u16 oamAttribute13 asm("r2");
  u16 oamAttribute14;
  register s32 oamCoordinate15 asm("r2");
  u16 pitchSoundId;
  register u16 oamAttribute16 asm("r2");
  register s32 oamCoordinate17 asm("r2");
  u16 oamAttribute2;
  int oamXMaskValue;
  register struct AnimationFrame *bonusFrame asm("r0");
  register s32 coordinateMask asm("r2");
  register u16 oamAttribute3 asm("r2");
  register s32 oamCoordinate4 asm("r2");
  u16 oamAttribute5;
  u16 oamAttribute6;
  register s32 oamCoordinate7 asm("r2");
  register s32 oamCoordinate8 asm("r2");
  register s32 oamCoordinate9 asm("r2");
  u8 byteValue;
  u8 pitcherFrameTime;
  u8 pitcherAnimationIndex;
  u8 resultFrameTime;
  u32 currentFrameDuration;
  u8 frameTimeA;
  u8 frameTimeB;
  u8 frameTimeC;
  u8 frameTimeD;
  u8 pitcherAnimationType;
  u8 animationType;
  u8 bonusAnimationIndex;
  s32 firstOamIndex;
  register s32 nextOamIndex asm("r8");
  void *oamBufferPointer;
  int clearObjectModeMask;
  void *currentOamBytes;
  void *resultOamBytes;
  OamByte *ballOamBytes;
  void *secondaryOamBytes;
  OamByte *batOamBytes;
  void *staticOamBytes;
  OamByte *bonusOamBytes;
  void *pitcherOamEntry;
  bonusRenderState = &gHomerunBonusRender;
  firstOamIndex = 0;
  nextOamIndex = gOamSlotsUsed;
  workingOffset = nextOamIndex * 8;
  oamBuffer = (OamByte *) gOamBuffer;
  oamDataDestination = (OamByte *) (((s32) workingOffset) + ((s32) oamBuffer));
  byteValue = gMinigameCameraState.oamMode;
  cameraTransform = &gMinigameCameraState;
  if (byteValue != 0)
  {
    if (byteValue == 1)
    {
      frameData = sHomerunCameraOamMode1;
    }
    if (byteValue == 2)
    {
      frameData = sHomerunCameraOamMode2;
    }
    nextOamIndex += *frameData;
    frameData = (u16 *) (((u8 *) frameData) + 2);
    affinePb = &affineMatrix[1];
    affinePc = &affineMatrix[2];
    affinePd = affinePb + 2;
    if (firstOamIndex < nextOamIndex)
    {
      cameraForSprites = cameraTransform;
      oamEntry = oamBuffer;
      oamXMask = 0x1FF;
      oamAttributeMask = -13;
      firstOamIndex = nextOamIndex;
      do
      {
        oamAttribute = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute;
        oamDataDestination += 2;
        *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((*cameraForSprites).yPosition + oamAttribute) - 0x10);
        *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
        oamAttribute2 = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute2;
        oamDataDestination += 2;
        positionValue = *((s16 *) (((u8 *) cameraForSprites) + 8));
        positionValue = oamAttribute2 + positionValue;
        positionValue -= 0x20;
        coordinateMask = oamXMask;
        positionValue &= coordinateMask;
        oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
        preservedOamBits = 0xFFFFFE00;
        preservedOamBits &= oamAttribute2;
        *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
        preservedOamBits = -0xF;
        *((u8 *) (((u8 *) oamEntry) + 3)) = (u8) (preservedOamBits & positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = (u16) (*((u16 *) (((u8 *) frameData) + 0)));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 5));
        *((u8 *) (((u8 *) oamEntry) + 5)) = (u8) (oamAttributeMask & positionValue);
        oamDataDestination += 4;
        oamEntry = oamEntry + 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    cameraForRotation = cameraTransform;
    matrixCosine = sSinCosTable[(((u16) cameraForRotation->rotation) >> 4) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) matrixCosine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForRotation = cameraTransform;
    matrixSine = sSinCosTable[((u16) cameraForRotation->rotation) >> 4];
    *affinePb = (u16) FixedMul((s32) matrixSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixNegativeSine = 0 - sSinCosTable[((u16) cameraForMatrix->rotation) >> 4];
    *affinePc = (u16) FixedMul((s32) matrixNegativeSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixCosineD = sSinCosTable[(((u16) cameraForMatrix->rotation) >> 4) + 0x40];
    affinePdValue = (u16) FixedMul((s32) matrixCosineD, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    *affinePd = affinePdValue;
    oamBuffer = (OamByte *) gOamBuffer;
    *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) affineMatrix[0];
    *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
    *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = affinePdValue;
  }
  if (((((u32) gMinigameState) > 2U) && (gMinigameState != 0xF)) && (gMinigameState != 0xE))
  {
    if (((u32) ((u8) (gMinigameState - 8))) <= 1U)
    {
      register s32 countSource asm("r2");
      dataPointer = (OamByte *) (&gHomerunBallRenderState);
      frameCounter = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
      *((u16 *) (((u8 *) dataPointer) + 0)) = frameCounter;
      frameTimeB = sHomerunBallHitDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
      animationCounterLimit = (u32) ((u16) frameCounter);
      objectState = dataPointer;
      if (((u32) frameTimeB) < animationCounterLimit)
      {
        *((u16 *) (((u8 *) objectState) + 0)) = 0U;
        *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
        frameTimeB = sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
        if (frameTimeB == 0)
        {
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) frameTimeB;
        }
      }
      secondaryFrameData = sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam;
      nextOamIndex += *secondaryFrameData;
      frameData = secondaryFrameData + 1;
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        cameraTransform = (struct HomerunCameraState *) objectState;
        oamAttributeMask = -15;
        workingOffset = firstOamIndex << 3;
        oamEntry = (OamByte *) (workingOffset + positionValue);
        oamXMask = 0x1FF;
        if (firstOamIndex) {
          countSource = nextOamIndex;
        } else {
          countSource = nextOamIndex;
        }
        secondaryEntryCount = countSource - firstOamIndex;
        do
        {
          oamAttribute5 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute5;
          oamDataDestination += 2;
          cameraForMatrix = cameraTransform;
          *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((((s32) (cameraForMatrix->rotation << 0x10)) >> 0x14) + oamAttribute5) - 0x20);
          *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
          oamAttribute6 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute6;
          oamDataDestination += 2;
          positionValue = (s32) (cameraTransform->scale << 0x10);
          positionValue >>= 0x14;
          positionValue = oamAttribute6 + positionValue;
          positionValue -= 0x20;
          coordinateMask = oamXMask;
          positionValue &= coordinateMask;
          oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
          preservedOamBits = 0xFFFFFE00;
          preservedOamBits &= oamAttribute2;
          *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
          positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
          currentOamBytes = (u8 *) oamEntry;
          *((u8 *) (((u8 *) currentOamBytes) + 3)) = (u8) (oamAttributeMask & positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          oamDataDestination += 4;
          oamEntry += 8;
          secondaryEntryCount -= 1;
        }
        while (secondaryEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
      ballMatrixCosine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      affineMatrix[0] = (u16) FixedMul((s32) ballMatrixCosine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixSine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePb = (u16) FixedMul((s32) ballMatrixSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixNegativeSine = 0 - sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePc = (u16) FixedMul((s32) ballMatrixNegativeSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixCosineD = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      ballAffinePd = (*affinePd = FixedMul((s32) ballMatrixCosineD, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale))));
      oamBuffer = (OamByte *) gOamBuffer;
      *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) (affinePaValue = affineMatrix[0]);
      *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
      *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
      *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = ballAffinePd;
    }
    else
    {
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (gMinigameState == 7)
      {
        dataPointer = (OamByte *) (&gHomerunBallRenderState);
        positionValue = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
        *((u16 *) (((u8 *) dataPointer) + 0)) = positionValue;
        workingOffset = (u32) sHomerunBallPitchDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
        affinePaValue = (u16) positionValue;
        objectState = dataPointer;
        if (((u32) workingOffset) < ((u32) affinePaValue))
        {
          *((u16 *) (((u8 *) objectState) + 0)) = 0U;
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
          frameTimeA = sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
          animationMatrixPc = (u16) frameTimeA;
          if (frameTimeA == 0)
          {
            *((u16 *) (((u8 *) objectState) + 2)) = animationMatrixPc;
          }
        }
        ballFrameData = sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam;
        nextOamIndex += *ballFrameData;
        frameData = ballFrameData + 1;
        stackPointer = ((u8 *)affineMatrix);
        affinePointerReference = (u16 **) (&stackPointer);
        affinePb = (u16 *) (((u8 *) (*affinePointerReference)) + 2);
        affinePc = (u16 *) (((u8 *) (*affinePointerReference)) + 4);
        affinePd = (u16 *) (((u8 *) (*affinePointerReference)) + 6);
        clearObjectModeMask = -4;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamAttributeMask = 0xFFFFFE00;
          workingOffset = firstOamIndex << 3;
          ballOamBytes = (OamByte *) (((s32) workingOffset) + positionValue);
          cameraTransform = (struct HomerunCameraState *) 0x1FF;
          oamByteMask = -0xD;
          cameraEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamAttribute3 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute3;
            oamDataDestination += 2;
            *((s8 *) (((u8 *) ballOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 0xA))) + oamAttribute3);
            *((u8 *) (((u8 *) ballOamBytes) + 1)) = (u8) (clearObjectModeMask & (*((u8 *) (((u8 *) ballOamBytes) + 1))));
            oamCoordinate4 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate4;
            oamDataDestination += 2;
            positionValue = *((s16 *) (((u8 *) objectState) + 8));
            positionValue = oamCoordinate4 + positionValue;
            positionValue &= (s32) cameraTransform;
            oamAttribute = *((u16 *) (((u8 *) ballOamBytes) + 2));
            preservedOamBits = oamAttributeMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) ballOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) ballOamBytes) + 5));
            *((u8 *) (((u8 *) ballOamBytes) + 5)) = (u8) (oamByteMask & positionValue);
            oamDataDestination += 4;
            ballOamBytes += 8;
            cameraEntryCount -= 1;
          }
          while (cameraEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
      }
    }
    clearOamXMask = 0xFFFFFE00;
    oamAttributeMask = gHomerunMissCount;
    if (((s32) oamAttributeMask) > 2)
    {
      oamAttributeMask = 2;
    }
    objectState = 0;
    if (((s32) objectState) < ((s32) oamAttributeMask))
    {
      do
      {
        nextOamIndex += *sHomerunStaticOam;
        frameData = sHomerunStaticOam + 1;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamByteMask = -0xD;
          workingOffset = firstOamIndex << 3;
          secondaryOamBytes = (OamByte *) (workingOffset + positionValue);
          secondaryEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamCoordinate7 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate7;
            oamDataDestination += 2;
            workingOffset = oamCoordinate7 + 0x20;
            *((s8 *) (((u8 *) secondaryOamBytes) + 0)) = (s8) (workingOffset - (((s32) objectState) * 8));
            oamAttribute5 = *(frameData++);
            oamCoordinate8 = oamAttribute5;
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate8;
            oamDataDestination += 2;
            positionValue = oamCoordinate8 + 0xE0;
            preservedOamBits = 0x1FF;
            positionValue &= preservedOamBits;
            oamAttribute = *((u16 *) (((u8 *) secondaryOamBytes) + 2));
            preservedOamBits = clearOamXMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) secondaryOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) secondaryOamBytes) + 5));
            *((u8 *) (((u8 *) secondaryOamBytes) + 5)) = (u8) ((oamByteMask & positionValue) | 8);
            oamDataDestination += 4;
            secondaryOamBytes += 8;
            secondaryEntryCount -= 1;
          }
          while (secondaryEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
        objectState = (OamByte *) (((s32) objectState) + 1);
      }
      while (((s32) objectState) < ((s32) oamAttributeMask));
    }
    if (gMinigameMedalHighlight != 0)
    {
      uiAnimationTimer = gMinigameUiAnimation.animationTimer + 1;
      gMinigameUiAnimation.animationTimer = uiAnimationTimer;
      if (((u32) sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time) < ((u32) ((u16) uiAnimationTimer)))
      {
        gMinigameUiAnimation.animationTimer = 1U;
        gMinigameUiAnimation.animationFrame = (u16) (gMinigameUiAnimation.animationFrame + 1);
        frameTimeC = sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time;
        if (frameTimeC == 0)
        {
          gMinigameUiAnimation.animationFrame = (u16) frameTimeC;
          gMinigameUiAnimation.animationTimer = (u16) frameTimeC;
          gMinigameMedalHighlight = 0;
          DrawHomerunDerbyScore();
        }
      }
    }
    oamXMaskValue = 0x1FF;
    batFrameData = sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].oam;
    nextOamIndex += *batFrameData;
    frameData = batFrameData + 1;
    cameraTransform = (struct HomerunCameraState *) &gHomerunBatAnimation;
    activeAnimation = sHomerunBallIdleAnimation;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      oamAttributeMask = 0x1FF;
      oamByteMask = -0xD;
      workingOffset = firstOamIndex << 3;
      batOamBytes = (OamByte *) (workingOffset + positionValue);
      firstOamIndex = nextOamIndex - firstOamIndex;
      do
      {
        oamCoordinate9 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate9;
        oamDataDestination += 2;
        workingOffset = oamCoordinate9;
        workingOffset += 0x92;
        *((s8 *) (((u8 *) batOamBytes) + 0)) = (s8) workingOffset;
        oamCoordinate10 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate10;
        oamDataDestination += 2;
        positionValue = oamCoordinate10 + 0x18;
        positionValue &= oamAttributeMask;
        clearOamXMask = 0xFFFFFE00;
        *((u16 *) (((u8 *) batOamBytes) + 2)) = (u16) ((clearOamXMask & (*((u16 *) (((u8 *) batOamBytes) + 2)))) | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        positionValue = *((u8 *) (((u8 *) batOamBytes) + 5));
        *((u8 *) (((u8 *) batOamBytes) + 5)) = (oamByteMask & positionValue) | 8;
        oamDataDestination += 4;
        batOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    activeAnimationState = (struct HomerunSimpleAnimation *) cameraTransform;
    animationTimer = activeAnimationState->animationTimer + 1;
    activeAnimationState->animationTimer = animationTimer;
    currentFrameDuration = (u32) activeAnimation[activeAnimationState->animationFrame].time;
    if (currentFrameDuration < ((u32) ((u16) animationTimer)))
    {
      activeAnimationState->animationTimer = 0U;
      activeAnimationState->animationFrame = (u16) (activeAnimationState->animationFrame + 1);
      frameTimeD = activeAnimation[activeAnimationState->animationFrame].time;
      if (frameTimeD == 0)
      {
        activeAnimationState->animationFrame = (u16) frameTimeD;
      }
    }
    cameraForMatrix = cameraTransform;
    activeFrameData = activeAnimation[((struct HomerunSimpleAnimation *) cameraForMatrix)->animationFrame].oam;
    nextOamIndex += *activeFrameData;
    frameData = activeFrameData + 1;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      {
        register u32 maskSeed asm("r2");
        if (firstOamIndex) {
          maskSeed = 0xFFFFFE00;
        } else {
          maskSeed = 0xFFFFFE00;
        }
        oamAttributeMask = maskSeed;
      }
      workingOffset = firstOamIndex << 3;
      staticOamBytes = (OamByte *) (workingOffset + positionValue);
      objectState = (OamByte *) oamXMaskValue;
      oamByteMask = -15;
      {
        register s32 nextCount asm("r1") = nextOamIndex;
        firstOamIndex = nextCount - firstOamIndex;
      }
      do
      {
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        workingOffset = oamCoordinate12;
        workingOffset += 0x60;
        *((u8 *) (((u8 *) staticOamBytes) + 0)) = (u8) workingOffset;
        positionValue = *((u8 *) (((u8 *) staticOamBytes) + 1));
        *((u8 *) (((u8 *) staticOamBytes) + 1)) = (u8) ((clearObjectModeMask & positionValue) | 1);
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        positionValue = oamCoordinate12 + 0xD7;
        positionValue &= (s32) objectState;
        oamAttribute = *((u16 *) (((u8 *) staticOamBytes) + 2));
        preservedOamBits = oamAttributeMask;
        preservedOamBits &= oamAttribute;
        *((u16 *) (((u8 *) staticOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
        pitcherAnimationType = *((u8 *) (((u8 *) staticOamBytes) + 3));
        workingOffset = oamXMask;
        workingOffset &= pitcherAnimationType;
        *((u8 *) (((u8 *) staticOamBytes) + 3)) = (u8) (workingOffset | 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (((u8 *) staticOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) staticOamBytes) + 5))) | 0xC);
        oamDataDestination += 4;
        staticOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    savedOamIndex = nextOamIndex;
    cameraData = cameraTransform;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPa = sSinCosTable[(*((u16 *) (((u8 *) dataPointer) + 4))) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) animationMatrixPa, (s32) ((s16) FixedInverse(0x100)));
    objectState = (OamByte *) 0x100;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPb = sSinCosTable[*((u16 *) (((u8 *) dataPointer) + 4))];
    *affinePb = (u16) FixedMul((s32) animationMatrixPb, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (frameData = (u16 *) cameraData);
    animationMatrixPc = 0 - sSinCosTable[frameData[2]];
    *affinePc = (u16) FixedMul((s32) animationMatrixPc, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (u16 *) cameraData;
    animationMatrixPd = sSinCosTable[frameData[2] + 0x40];
    frameTimeValue = (*affinePd = (u16) FixedMul((s32) animationMatrixPd, (s32) ((s16) FixedInverse((s32) objectState))));
    oamBufferPointer = (u8 *) ((OamByte *) gOamBuffer);
    oamAttribute14 = (affinePaValue = affineMatrix[0]);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x26)) = (u16) oamAttribute14;
    oamAttribute11 = *affinePb;
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x2E)) = (u16) oamAttribute11;
    *((u16 *) (((u8 *) oamBufferPointer) + 0x36)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x3E)) = frameTimeValue;
    if (gHomerunBonusRender.animationIndex == 0)
    {
    }
    else
    {
      bonusAnimationIndex = gHomerunBonusRender.animationIndex;
      bonusAnimationFrames = sHomerunBonusAnimations[bonusAnimationIndex];
      bonusOamBytes = (OamByte *) bonusAnimationFrames;
      bonusAnimationTimer = gHomerunBonusRender.animationTimer + 1;
      gHomerunBonusRender.animationTimer = bonusAnimationTimer;
      frameDuration = (u32) ((struct AnimationFrame *) ((gHomerunBonusRender.animationFrame << 3) + (u32) bonusAnimationFrames))->time;
      if (frameDuration < ((u32) ((u16) bonusAnimationTimer)))
      {
        gHomerunBonusRender.animationTimer = 1U;
        bonusAnimation = bonusAnimationFrames;
        gHomerunBonusRender.animationFrame = (u16) (gHomerunBonusRender.animationFrame + 1);
        if ((byteValue = bonusAnimation[gHomerunBonusRender.animationFrame].time) == 0)
        {
          gHomerunBonusRender.animationFrame = 0;
          animationType = gMinigameSequenceState;
          if (animationType == 1)
          {
            if (!(animationType & ((u32 (*)(void)) MinigameRandom)()))
            {
              m4aSongNumStart(0x237U);
            }
            else
            {
              m4aSongNumStart(0x238U);
            }
            m4aSongNumStart(0x2CCU);
            gHomerunBonusRender.animationIndex = (u8) gHomerunBonusRender.nextAnimationIndex;
            gHomerunBonusRender.animationTimer = 0U;
            gMinigameSequenceState += 1;
          }
        }
      }
      bonusOamBytes = (OamByte *) sHomerunBonusAnimations[(*bonusRenderState).animationIndex];
      bonusFrameData = ((struct AnimationFrame *) bonusOamBytes)[gHomerunBonusRender.animationFrame].oam;
      nextOamIndex += *bonusFrameData;
      frameData = bonusFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        OamByte *render_state;
        positionValue = (s32)gOamBuffer;
        render_state = (OamByte *) (&gHomerunBonusRender);
        cameraTransform = (struct HomerunCameraState *) 0x1FF;
        oamXMask = 0xFFFFFE00;
        bonusOamBytes = (OamByte *)((firstOamIndex << 3) + positionValue);
        bonusEntryCount = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute13 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute13;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) bonusOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) render_state) + 6))) + oamAttribute13);
          oamAttribute2 = *(frameData++);
          *((u16 *)oamDataDestination) = oamAttribute2;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) render_state) + 4));
          positionValue = oamAttribute2 + positionValue;
          positionValue &= (s32) cameraTransform;
          oamAttribute = *((u16 *) (((u8 *) bonusOamBytes) + 2));
          preservedOamBits = oamXMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) bonusOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          *((u8 *) (((u8 *) bonusOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) bonusOamBytes) + 5))) | 0xC);
          oamDataDestination += 4;
          bonusOamBytes += 8;
          bonusEntryCount -= 1;
        }
        while (bonusEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
    }
    pitcherAnimation = (OamByte *) sHomerunPitcherAnimations[gHomerunPitcherAnimation.animationIndex];
    pitcherAnimationTimer = gHomerunPitcherAnimation.animationTimer + 1;
    gHomerunPitcherAnimation.animationTimer = pitcherAnimationTimer;
    if (((u32) ((struct AnimationFrame *) pitcherAnimation)[gHomerunPitcherAnimation.animationFrame].time) >= ((u32) ((u16) pitcherAnimationTimer)))
    {
    }
    else
    {
      gHomerunPitcherAnimation.animationTimer = 0U;
      gHomerunPitcherAnimation.animationFrame = (u16) (gHomerunPitcherAnimation.animationFrame + 1);
      if (gHomerunPitcherAnimation.animationIndex == 4)
      {
        switch (gHomerunPitcherAnimation.animationFrame)
        {
          case 10:
            gMinigameState = 7;
            m4aSongNumStart(sHomerunPitchSounds[gHomerunPitchPath * 2]);
            break;
            pitchSoundId = 0x22BU;

          case 0:
            m4aSongNumStart(0x228U);
            break;

          case 1:
            m4aSongNumStart(0x229U);
            break;

          case 5:
            m4aSongNumStart(0x22AU);
            break;

          case 8:
            m4aSongNumStart(0x22BU);
            break;

        }

      }
      pitcherState = (struct HomerunPitcherState *) &gHomerunPitcherAnimation;
      pitcherFrameTime = ((struct AnimationFrame *) pitcherAnimation)[pitcherState->animationFrame].time;
      if (pitcherFrameTime == 0)
      {
        pitcherState->animationFrame = (u16) pitcherFrameTime;
        pitcherAnimationIndex = pitcherState->animationIndex;
        if (pitcherAnimationIndex == 4)
        {
          pitcherState->animationIndex = 5U;
          pitcherAnimation = (OamByte *) sHomerunPitcherAnimations[5];
        }
        else
          if (((u32) ((u8) (pitcherAnimationIndex - 1))) <= 3U)
        {
          pitcherState->pitchCounter = (u8) (pitcherState->pitchCounter + 1);
          if (gDifficulty == 2)
          {
            dataPointer = &sHomerunPitchAnimationsHard;
          }
          else
          {
            dataPointer = &sHomerunPitchAnimationsNormal;
          }
          entryCounter = (gHomerunPitchPath * 5) - 1;
          pitcherState->animationIndex = (u8) (*((u8 *) ((entryCounter + pitcherState->pitchCounter) + ((s32) dataPointer))));
          dataPointer = (OamByte *) sHomerunPitcherAnimations;
          objectState = (OamByte *) &gHomerunPitcherAnimation;
          pitcherAnimationType = *((u8 *) (((u8 *) objectState) + 4));
          pitcherAnimation = (OamByte *) ((struct AnimationFrame **) dataPointer)[pitcherAnimationType];
          if (pitcherAnimationType == 3)
          {
            m4aSongNumStart(0x226U);
          }
          if ((*((u8 *) (((u8 *) objectState) + 4))) == 2)
          {
            m4aSongNumStart(0x227U);
          }
          oamXMaskPointer = (struct HomerunCameraState *) 0x1FF;
        }
      }
    }
    pitcherFrameData = ((struct AnimationFrame *) pitcherAnimation)[((struct HomerunPitcherState *) &gHomerunPitcherAnimation)->animationFrame].oam;
    nextOamIndex += *pitcherFrameData;
    frameData = pitcherFrameData + 1;
    if (((s32) firstOamIndex) < ((s32) nextOamIndex))
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      objectState = (OamByte *) 0x1FF;
      cameraTransform = (struct HomerunCameraState *) 0xFFFFFE00;
      workingOffset = firstOamIndex << 3;
      pitcherOamEntry = (OamByte *) (workingOffset + positionValue);
      positionValue = nextOamIndex;
      pitcherEntryCount = positionValue - firstOamIndex;
      do
      {
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        workingOffset = oamCoordinate15;
        workingOffset += 0x58;
        *((u8 *) (((u8 *) pitcherOamEntry) + 0)) = (u8) workingOffset;
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        positionValue = oamCoordinate15 + 0x48;
        positionValue &= (s32) objectState;
        pitcherOamBytes = (u8 *) pitcherOamEntry;
        oamAttribute = *((u16 *) (pitcherOamBytes + 2));
        preservedOamBits = (s32) cameraTransform;
        preservedOamBits &= oamAttribute;
        *((u16 *) (pitcherOamBytes + 2)) = (u16) (preservedOamBits | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (pitcherOamBytes + 5)) = (u8) ((*((u8 *) (pitcherOamBytes + 5))) | 0xC);
        oamDataDestination += 4;
        pitcherOamEntry += 8;
        pitcherEntryCount -= 1;
      }
      while (pitcherEntryCount != 0);
      firstOamIndex = nextOamIndex;
    }
    if (((u32) ((u8) (gMinigameState - 0xB))) <= 2U)
    {
      resultAnimation = (OamByte *) sHomerunResultAnimations[gHomerunPitchResult];
      resultAnimationTimer = gHomerunResultRender.animationTimer + 1;
      if (1)
      {
        gHomerunResultRender.animationTimer = resultAnimationTimer;
      }
      if (((u32) ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time) < ((u32) ((u16) resultAnimationTimer)))
      {
        gHomerunResultRender.animationTimer = 0U;
        gHomerunResultRender.animationFrame = (u16) (gHomerunResultRender.animationFrame + 1);
        resultFrameTime = ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time;
        if (resultFrameTime == 0)
        {
          gHomerunResultRender.animationFrame = (u16) resultFrameTime;
        }
      }
      resultFrameData = ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].oam;
      nextOamIndex += *resultFrameData;
      frameData = resultFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        objectState = (OamByte *) (&gHomerunResultRender);
        oamXMask = 0x1FF;
        oamAttributeMask = 0xFFFFFE00;
        clearAffineModeMask = -0xD;
        workingOffset = firstOamIndex << 3;
        resultOamBytes = (OamByte *) (workingOffset + positionValue);
        entryCounter = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute16 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute16;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) resultOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 6))) + oamAttribute16);
          oamCoordinate17 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate17;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) objectState) + 4));
          positionValue = oamCoordinate17 + positionValue;
          positionValue &= oamXMask;
          oamAttribute = *((u16 *) (((u8 *) resultOamBytes) + 2));
          preservedOamBits = oamAttributeMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) resultOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          positionValue = *((u8 *) (((u8 *) resultOamBytes) + 5));
          *((u8 *) (((u8 *) resultOamBytes) + 5)) = (u8) (clearAffineModeMask & positionValue);
          oamDataDestination += 4;
          resultOamBytes += 8;
          entryCounter -= 1;
        }
        while (entryCounter != 0);
      }
    }
  }
  gOamSlotsUsed = nextOamIndex;
}
```


### Commit completion

Committed verified source only as `89a6f56` (13 additions, 6 deletions). Rechecked staged source against byte-exact snapshot and linked bytes against all2792 original ROM bytes before committing. Used the existing repository author identity for this command only; no Git configuration changed. Unrelated changes and report edits remain outside the commit. MCP display classification discrepancy remains score87.

# ResolveWarioCeilingCollision   verified 88.7%

ResolveWarioCeilingCollision follow-up: best VERIFIED 88.7%, score2236 (baseline79.5%,4044). Saved/restored current source; outside function unchanged against pre-session snapshot. No inline asm, headers, flags or compiler tools changed, no commit. Improvements: (A) block-local initialX/initialY instead of reusing main x/y, 86.4%2678; (H) explicit shared compareFC/adjustFC/returnFC labels plus preserve original probe X in savedX for ProcessHorizontalTileInteraction, 88.0%2363; (T) right and centre table reads via explicit byte offsets (right<<1)+((side*3)<<1), centre base table+1 plus offset, 88.7%2236. Target explicitly retains pre-correction probe X at sp+0x10; old draft incorrectly passed corrected x. Shared labels reproduce long forward jumps from initial check into common FC tail. Do not regress original-X preservation.
Remaining: target frame0x1c matches best frame, but right/offset/savedX spills are permuted (target right14,offsetC,savedX10; current C,10,14). Target pointValue atsp18 usesstrh/ldrh; current spills extendedword withstr/ldr. Initial probe uses targetx7 andprevYpointer8; current x passesr0 andprevYpointer7. Common FC maskload must be before branch into shared alignment check (target r0mask, r2currentY,r3previousY); current mask hoisted only into compare block with otherregs. Result targetr2, currentr1. Main retry block layout and outputpointer lifetimes differ. Byte-offset table change reduces right/centre instructions, retained inbest.
22 distinct candidates A-W (M skipped), plus baseline/restoration compiles. A2678; B left5+initialX7 3047; C result2/adjX3/previous8 3157; D padded outputs/frame struct3914; E buildfail C89 mixeddecl; F struct actualoffset/savedX/right/point4122; G broad main+initial pins5826; H2363; I scopedinitialX7 previous8 result2 oldY1 3029; J plusleft5 adjX3 current2prev3 2785; K buildfail generateddecl; L fixedexplicitstruct andseed1/2 initialstores4481; N L with explicit initialWario6,address0,outputptr5/4,mask0 4312 (initial prefix veryclose but mainallocation badlyregressed); O volatilepoint4557(frame0x20 and shiftsoutputs); P Fsharedtail3756; Q Hresult2only2485; R Q initialX7 left5 prev8 seed0 oldY1 2538; S Q sharedFD/newY0labels2539; T2236; U rightadditionoperandorder2236same; V TmainpreviousBasehard5 andcachedvalue2542; W TplainretryYcache2706.
Do not pin all variables at once: can clobber live hard vars, increase spills and change global assignments. Explicit aggregate outputs fixes frame offsets/halfword point but causes reloads and loses held output pointers. Need block-local output pointers and separately staged table/camera arithmetic if continuing that alternative; candidate N is a useful close initial prefix, not best.
Detailed report with best complete C and full objdump diff: reports/ceiling_collision_for_gpt6.md. Diagnostics in local scratch, not in the repo: %TEMP%/wl4-ceiling-codex (original.c,best-score2236.c,target.asm,best.asm,best.diff,best.i,best.s,best-function-combine/greg/jump2). RTL generated with same project flags plus -da diagnostic dumps, no config edits. Next: investigate original scalar HI stack allocation vs explicit frame struct before more hardreg pins; stage main retry/store-FD paths around existing target labels; inspect reload events after each small change.

## Best implementation

```c
u8 ResolveWarioCeilingCollision(void)
{
    u16 adjX;
    u16 adjY;
    u16 sec;
    u32 side = gWarioCollisionData.unk_08;
    u32 left = gWarioCollisionData.unk_04;
    u32 right = gWarioCollisionData.unk_05;
    s16 offset = gWarioCollisionData.unk_06;
    u32 result;
    u32 x;
    u32 y;
    u32 savedX;
    u32 currentY;
    u32 previousY;
    u16 pointValue;

    if (gWarioCollisionData.unk_00 != 0) {
        result = CheckWarioHorizontalCollision(side, &adjX, -22);
        if (result != 0) {
            gWarioData.xPosition = adjX;
            if (gWarioCollisionData.unk_0B == 0) {
                gWarioData.xVelocity = 0;
                gWarioCollisionData.unk_12++;
                goto main;
            }
            if (gWarioCollisionData.unk_0B == 1 || (s32)result > gWarioCollisionData.unk_0E) {
                u32 initialX;
                u32 initialY;
                initialX = (u16)(adjX + sWarioCollisionPointTable[side * 3 + left]);
                gWarioData.yPosition = (gWarioData.yPosition + gPreviousYPosition) >> 1;
                initialY = (u16)(gWarioData.yPosition + 1);
                CheckWarioTileCollision(initialX, initialY, &adjX, &adjY, &sec);
                if (sec == 0) goto returnFC;
                if (!(sec & 0x10)) goto adjustFC;
                currentY = gWarioData.yPosition;
                previousY = gPreviousYPosition;
                goto compareFC;
            }
        }
    }

main:
    if (gWarioData.unk_1A == 1) {
        return 0xFF;
    }

    pointValue = sWarioCollisionPointTable[side * 3 + left];
    x = (u16)(pointValue + gWarioData.xPosition);
    y = gWarioData.yPosition;
    savedX = x;
    result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
    if (sec != 0) {
        if (result != 0) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY;
            return 0xFD;
        }
        if (gPreviousYPosition < adjY) {
            y = gPreviousYPosition;
            result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
            gWarioData.unk_1C = result;
            if (result != 0) {
                gWarioData.yPosition = adjY;
            } else {
                gWarioData.yPosition = adjY + 63;
            }
            return 0xFD;
        }
        if (sec != 0x10) {
            gWarioData.xPosition = offset + (adjX - pointValue);
            x = gWarioData.xPosition;
            result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
            gWarioData.unk_1C = result;
            if (result != 0) {
                gWarioData.yPosition = adjY;
                return 0xFD;
            }
            if (sec != 0) {
                gWarioData.yPosition = adjY - 1;
                return 0xFD;
            }
            if (gWarioCollisionData.unk_0B == 0) {
                gWarioData.xVelocity = result;
                return 0xFF;
            }
            result = ProcessHorizontalTileInteraction(y, savedX);
            if (result != 0 && gWarioCollisionData.unk_0B != 1) {
                return 0xFF;
            }
            CheckWarioTileCollision(gWarioData.xPosition, (u16)(gWarioData.yPosition + 1), &adjX, &adjY, &sec);
            if (sec == 0) goto returnFC;
            if (!(sec & 0x10)) goto adjustFC;
            currentY = gWarioData.yPosition;
            previousY = gPreviousYPosition;
            compareFC:
            if ((currentY & 0xFFC0) == (previousY & 0xFFC0)) goto returnFC;
adjustFC:
            gWarioData.yPosition = adjY - 2;
returnFC:
            return 0xFC;
        }
    }

    x = (u16)(*(const u16 *)((u32)sWarioCollisionPointTable + (right << 1) + ((side * 3) << 1)) + gWarioData.xPosition);
    y = gWarioData.yPosition;
    result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
    if (sec != 0) {
        if (result != 0) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY;
            return 0xFD;
        }
        if (gPreviousYPosition >= adjY) {
            return 0xFF;
        }
        y = gPreviousYPosition;
        result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
        gWarioData.unk_1C = result;
        if (result != 0) {
            gWarioData.yPosition = adjY;
        } else {
            gWarioData.yPosition = adjY + 63;
        }
        return 0xFD;
    }
    if (*(const s16 *)((u32)(sWarioCollisionPointTable + 1) + ((side * 3) << 1)) > 31) {
        x = gWarioData.xPosition;
        result = ((u32 (*)(u32, u32, u16 *, u16 *, u16 *))CheckWarioTileCollision)(x, y, &adjX, &adjY, &sec);
        if (sec != 0 && gPreviousYPosition < adjY) {
            gWarioData.unk_1C = result;
            gWarioData.yPosition = adjY - 1;
            return 0xFD;
        }
    }
    return 0xFF;
}
```

## Remaining assembly diff

This is the full objdump instruction display diff. Literal symbols in relocatable objects can appear as missing zero words; inspect relocation records in target.asm and best.asm.

```diff
--- 

+++ 

@@ -10,10 +10,10 @@

 ldrb	r5, [r4, #4]
 ldrb	r1, [r4, #5]
-str	r1, [sp, #20]
+str	r1, [sp, #12]
 ldrh	r2, [r4, #6]
-str	r2, [sp, #12]
+str	r2, [sp, #16]
 ldrh	r0, [r4, #0]
 cmp	r0, #0
-beq.n	4550 <ResolveWarioCeilingCollision+0xc4>
+beq.n	4544 <ResolveWarioCeilingCollision+0xb8>
 movs	r2, #22
 negs	r2, r2
@@ -21,11 +21,11 @@

 add	r1, sp, #4
 bl	3a58 <CheckWarioHorizontalCollision>
-adds	r2, r0, #0
-cmp	r2, #0
-beq.n	4550 <ResolveWarioCeilingCollision+0xc4>
+adds	r1, r0, #0
+cmp	r1, #0
+beq.n	4544 <ResolveWarioCeilingCollision+0xb8>
 ldr	r6, [pc, #24]	@ (44dc <ResolveWarioCeilingCollision+0x50>)
 add	r0, sp, #4
-ldrh	r3, [r0, #0]
-strh	r3, [r6, #18]
+ldrh	r2, [r0, #0]
+strh	r2, [r6, #18]
 ldrb	r0, [r4, #11]
 cmp	r0, #0
@@ -35,13 +35,13 @@

 adds	r0, #1
 strb	r0, [r4, #18]
-b.n	4550 <ResolveWarioCeilingCollision+0xc4>
+b.n	4544 <ResolveWarioCeilingCollision+0xb8>
 cmp	r0, #1
 beq.n	44ea <ResolveWarioCeilingCollision+0x5e>
 ldrb	r4, [r4, #14]
-cmp	r2, r4
-ble.n	4550 <ResolveWarioCeilingCollision+0xc4>
-ldr	r1, [pc, #88]	@ (4544 <ResolveWarioCeilingCollision+0xb8>)
-mov	r4, sl
-lsls	r0, r4, #1
+cmp	r1, r4
+ble.n	4544 <ResolveWarioCeilingCollision+0xb8>
+ldr	r1, [pc, #80]	@ (453c <ResolveWarioCeilingCollision+0xb0>)
+mov	r3, sl
+lsls	r0, r3, #1
 add	r0, sl
 adds	r0, r0, r5
@@ -49,12 +49,11 @@

 adds	r0, r0, r1
 ldrh	r0, [r0, #0]
-adds	r0, r3, r0
+adds	r0, r2, r0
 lsls	r0, r0, #16
-lsrs	r7, r0, #16
+lsrs	r0, r0, #16
 ldrh	r1, [r6, #20]
-ldr	r0, [pc, #68]	@ (4548 <ResolveWarioCeilingCollision+0xbc>)
-mov	r8, r0
-ldrh	r0, [r0, #0]
-adds	r1, r1, r0
+ldr	r7, [pc, #60]	@ (4540 <ResolveWarioCeilingCollision+0xb4>)
+ldrh	r2, [r7, #0]
+adds	r1, r1, r2
 asrs	r1, r1, #1
 strh	r1, [r6, #20]
@@ -66,5 +65,4 @@

 add	r4, sp, #8
 str	r4, [sp, #0]
-adds	r0, r7, #0
 add	r2, sp, #4
 adds	r3, r5, #0
@@ -72,44 +70,39 @@

 ldrh	r4, [r4, #0]
 cmp	r4, #0
-bne.n	452e <ResolveWarioCeilingCollision+0xa2>
-b.n	46c0 <ResolveWarioCeilingCollision+0x234>
+bne.n	452a <ResolveWarioCeilingCollision+0x9e>
+b.n	46a2 <ResolveWarioCeilingCollision+0x216>
 movs	r0, #16
 ands	r0, r4
 cmp	r0, #0
-bne.n	4538 <ResolveWarioCeilingCollision+0xac>
-b.n	46b6 <ResolveWarioCeilingCollision+0x22a>
+bne.n	4534 <ResolveWarioCeilingCollision+0xa8>
+b.n	4698 <ResolveWarioCeilingCollision+0x20c>
 ldrh	r2, [r6, #20]
-ldr	r0, [pc, #16]	@ (454c <ResolveWarioCeilingCollision+0xc0>)
-mov	r1, r8
-ldrh	r3, [r1, #0]
-b.n	46ac <ResolveWarioCeilingCollision+0x220>
-.short	0x0000
-.word	0x0000ffc0
-ldr	r3, [pc, #96]	@ (45b4 <ResolveWarioCeilingCollision+0x128>)
-mov	r9, r3
-ldrb	r0, [r3, #26]
+ldrh	r1, [r7, #0]
+b.n	468e <ResolveWarioCeilingCollision+0x202>
+.short	0x0000
+ldr	r4, [pc, #88]	@ (45a0 <ResolveWarioCeilingCollision+0x114>)
+mov	r9, r4
+ldrb	r0, [r4, #26]
 cmp	r0, #1
-bne.n	455c <ResolveWarioCeilingCollision+0xd0>
-b.n	4794 <ResolveWarioCeilingCollision+0x308>
-ldr	r1, [pc, #88]	@ (45b8 <ResolveWarioCeilingCollision+0x12c>)
-mov	r4, sl
-lsls	r6, r4, #1
-adds	r0, r6, r4
+bne.n	4550 <ResolveWarioCeilingCollision+0xc4>
+b.n	477c <ResolveWarioCeilingCollision+0x2f0>
+ldr	r1, [pc, #80]	@ (45a4 <ResolveWarioCeilingCollision+0x118>)
+mov	r0, sl
+lsls	r6, r0, #1
+adds	r0, r6, r0
 adds	r0, r0, r5
 lsls	r0, r0, #1
 adds	r0, r0, r1
 ldrh	r0, [r0, #0]
-mov	r1, sp
-strh	r0, [r1, #24]
-adds	r3, r0, #0
-mov	r2, r9
-ldrh	r2, [r2, #18]
-adds	r0, r3, r2
+str	r0, [sp, #24]
+mov	r1, r9
+ldrh	r0, [r1, #18]
+ldr	r2, [sp, #24]
+adds	r0, r2, r0
 lsls	r0, r0, #16
 lsrs	r7, r0, #16
-mov	r3, r9
-ldrh	r3, [r3, #20]
+ldrh	r3, [r1, #20]
 mov	r8, r3
-str	r7, [sp, #16]
+str	r7, [sp, #20]
 mov	r5, sp
 adds	r5, #6
@@ -121,56 +114,49 @@

 adds	r3, r5, #0
 bl	38c8 <CheckWarioTileCollision>
-adds	r2, r0, #0
-ldrh	r3, [r4, #0]
-adds	r1, r6, #0
-cmp	r3, #0
-bne.n	45a4 <ResolveWarioCeilingCollision+0x118>
-b.n	46cc <ResolveWarioCeilingCollision+0x240>
+adds	r1, r0, #0
+ldrh	r2, [r4, #0]
 cmp	r2, #0
-beq.n	45bc <ResolveWarioCeilingCollision+0x130>
+bne.n	4592 <ResolveWarioCeilingCollision+0x106>
+b.n	46b4 <ResolveWarioCeilingCollision+0x228>
+cmp	r1, #0
+beq.n	45a8 <ResolveWarioCeilingCollision+0x11c>
 mov	r4, r9
-strh	r2, [r4, #28]
+strh	r1, [r4, #28]
 mov	r1, sp
 ldrh	r0, [r1, #6]
-b.n	464e <ResolveWarioCeilingCollision+0x1c2>
-.short	0x0000
-ldr	r5, [pc, #44]	@ (45ec <ResolveWarioCeilingCollision+0x160>)
-ldrh	r0, [r5, #0]
-mov	r2, sp
-ldrh	r2, [r2, #6]
-cmp	r0, r2
-bcs.n	45fc <ResolveWarioCeilingCollision+0x170>
-adds	r5, r0, #0
-mov	r8, r5
-add	r3, sp, #8
-str	r3, [sp, #0]
-adds	r0, r7, #0
-mov	r1, r8
-add	r2, sp, #4
-subs	r3, #2
-bl	38c8 <CheckWarioTileCollision>
-adds	r2, r0, #0
-mov	r4, r9
-strh	r2, [r4, #28]
-cmp	r2, #0
-beq.n	45f0 <ResolveWarioCeilingCollision+0x164>
-mov	r1, sp
-ldrh	r0, [r1, #6]
-b.n	464e <ResolveWarioCeilingCollision+0x1c2>
+b.n	4630 <ResolveWarioCeilingCollision+0x1a4>
+ldr	r1, [pc, #48]	@ (45dc <ResolveWarioCeilingCollision+0x150>)
+ldrh	r0, [r1, #0]
+mov	r3, sp
+ldrh	r3, [r3, #6]
+cmp	r0, r3
+bcs.n	45e0 <ResolveWarioCeilingCollision+0x154>
+adds	r1, r0, #0
+mov	r8, r1
+add	r4, sp, #8
+str	r4, [sp, #0]
+adds	r0, r7, #0
+add	r2, sp, #4
+mov	r3, sp
+adds	r3, #6
+bl	38c8 <CheckWarioTileCollision>
+adds	r1, r0, #0
+mov	r0, r9
+strh	r1, [r0, #28]
+cmp	r1, #0
+bne.n	4616 <ResolveWarioCeilingCollision+0x18a>
+mov	r3, sp
+ldrh	r0, [r3, #6]
+adds	r0, #63	@ 0x3f
+b.n	462e <ResolveWarioCeilingCollision+0x1a2>
+.short	0x0000
 .word	0x00000000
-mov	r2, sp
-ldrh	r0, [r2, #6]
-adds	r0, #63	@ 0x3f
-mov	r3, r9
-strh	r0, [r3, #20]
-b.n	463c <ResolveWarioCeilingCollision+0x1b0>
-cmp	r3, #16
-beq.n	46cc <ResolveWarioCeilingCollision+0x240>
+cmp	r2, #16
+beq.n	46b4 <ResolveWarioCeilingCollision+0x228>
 add	r0, sp, #4
 ldrh	r1, [r0, #0]
-mov	r4, sp
-ldrh	r4, [r4, #24]
-subs	r1, r1, r4
-ldr	r2, [sp, #12]
+ldr	r0, [sp, #24]
+subs	r1, r1, r0
+ldr	r2, [sp, #16]
 lsls	r0, r2, #16
 asrs	r0, r0, #16
@@ -187,9 +173,9 @@

 adds	r3, #6
 bl	38c8 <CheckWarioTileCollision>
-adds	r2, r0, #0
+adds	r1, r0, #0
 mov	r0, r9
-strh	r2, [r0, #28]
-cmp	r2, #0
-beq.n	4640 <ResolveWarioCeilingCollision+0x1b4>
+strh	r1, [r0, #28]
+cmp	r1, #0
+beq.n	4622 <ResolveWarioCeilingCollision+0x196>
 mov	r1, sp
 ldrh	r0, [r1, #6]
@@ -197,35 +183,36 @@

 strh	r0, [r2, #20]
 movs	r0, #253	@ 0xfd
-b.n	4796 <ResolveWarioCeilingCollision+0x30a>
+b.n	477e <ResolveWarioCeilingCollision+0x2f2>
 mov	r3, sp
 ldrh	r0, [r3, #8]
 cmp	r0, #0
-beq.n	4652 <ResolveWarioCeilingCollision+0x1c6>
+beq.n	4634 <ResolveWarioCeilingCollision+0x1a8>
 ldrh	r0, [r3, #6]
 subs	r0, #1
 mov	r4, r9
 strh	r0, [r4, #20]
-b.n	463c <ResolveWarioCeilingCollision+0x1b0>
-ldr	r4, [pc, #12]	@ (4660 <ResolveWarioCeilingCollision+0x1d4>)
+b.n	461e <ResolveWarioCeilingCollision+0x192>
+ldr	r4, [pc, #12]	@ (4644 <ResolveWarioCeilingCollision+0x1b8>)
 ldrb	r0, [r4, #11]
 cmp	r0, #0
-bne.n	4664 <ResolveWarioCeilingCollision+0x1d8>
+bne.n	4648 <ResolveWarioCeilingCollision+0x1bc>
 mov	r0, r9
-strh	r2, [r0, #22]
-b.n	4794 <ResolveWarioCeilingCollision+0x308>
+strh	r1, [r0, #22]
+b.n	477c <ResolveWarioCeilingCollision+0x2f0>
+.short	0x0000
 .word	0x00000000
-ldr	r1, [sp, #16]
 mov	r0, r8
+ldr	r1, [sp, #20]
 bl	0 <ProcessHorizontalTileInteraction>
-adds	r2, r0, #0
-cmp	r2, #0
-beq.n	467a <ResolveWarioCeilingCollision+0x1ee>
+adds	r1, r0, #0
+cmp	r1, #0
+beq.n	465e <ResolveWarioCeilingCollision+0x1d2>
 ldrb	r0, [r4, #11]
 cmp	r0, #1
-beq.n	467a <ResolveWarioCeilingCollision+0x1ee>
-b.n	4794 <ResolveWarioCeilingCollision+0x308>
-mov	r1, r9
-ldrh	r0, [r1, #18]
-ldrh	r1, [r1, #20]
+beq.n	465e <ResolveWarioCeilingCollision+0x1d2>
+b.n	477c <ResolveWarioCeilingCollision+0x2f0>
+ldr	r6, [pc, #72]	@ (46a8 <ResolveWarioCeilingCollision+0x21c>)
+ldrh	r0, [r6, #18]
+ldrh	r1, [r6, #20]
 adds	r1, #1
 lsls	r1, r1, #16
@@ -240,43 +227,44 @@

 ldrh	r4, [r3, #8]
 cmp	r4, #0
-beq.n	46c0 <ResolveWarioCeilingCollision+0x234>
+beq.n	46a2 <ResolveWarioCeilingCollision+0x216>
 movs	r0, #16
 ands	r0, r4
 cmp	r0, #0
-beq.n	46b6 <ResolveWarioCeilingCollision+0x22a>
-mov	r4, r9
-ldrh	r2, [r4, #20]
-ldr	r0, [pc, #24]	@ (46c4 <ResolveWarioCeilingCollision+0x238>)
-ldrh	r3, [r5, #0]
-adds	r1, r0, #0
-ands	r1, r2
-ands	r0, r3
-cmp	r1, r0
-beq.n	46c0 <ResolveWarioCeilingCollision+0x234>
-ldr	r1, [pc, #16]	@ (46c8 <ResolveWarioCeilingCollision+0x23c>)
-mov	r2, sp
-ldrh	r0, [r2, #6]
+beq.n	4698 <ResolveWarioCeilingCollision+0x20c>
+ldrh	r2, [r6, #20]
+ldr	r0, [pc, #32]	@ (46ac <ResolveWarioCeilingCollision+0x220>)
+ldrh	r1, [r0, #0]
+ldr	r0, [pc, #32]	@ (46b0 <ResolveWarioCeilingCollision+0x224>)
+ands	r2, r0
+ands	r1, r0
+cmp	r2, r1
+beq.n	46a2 <ResolveWarioCeilingCollision+0x216>
+ldr	r1, [pc, #12]	@ (46a8 <ResolveWarioCeilingCollision+0x21c>)
+mov	r4, sp
+ldrh	r0, [r4, #6]
 subs	r0, #2
 strh	r0, [r1, #20]
 movs	r0, #252	@ 0xfc
-b.n	4796 <ResolveWarioCeilingCollision+0x30a>
+b.n	477e <ResolveWarioCeilingCollision+0x2f2>
+.short	0x0000
 .word	0x0000ffc0
-.word	0x00000000
-ldr	r6, [pc, #68]	@ (4714 <ResolveWarioCeilingCollision+0x288>)
-ldr	r3, [sp, #20]
-lsls	r0, r3, #1
+ldr	r0, [sp, #12]
+lsls	r1, r0, #1
+mov	r2, sl
+adds	r0, r6, r2
+lsls	r0, r0, #1
+mov	r9, r0
+add	r1, r9
+ldr	r3, [pc, #60]	@ (4700 <ResolveWarioCeilingCollision+0x274>)
+mov	sl, r3
 add	r1, sl
-lsls	r1, r1, #1
-mov	r9, r1
-add	r0, r9
-ldr	r4, [pc, #60]	@ (4718 <ResolveWarioCeilingCollision+0x28c>)
-adds	r0, r0, r4
-ldrh	r0, [r0, #0]
-ldrh	r1, [r6, #18]
+ldr	r6, [pc, #56]	@ (4704 <ResolveWarioCeilingCollision+0x278>)
+ldrh	r0, [r6, #18]
+ldrh	r1, [r1, #0]
 adds	r0, r0, r1
 lsls	r0, r0, #16
 lsrs	r7, r0, #16
-ldrh	r2, [r6, #20]
-mov	r8, r2
+ldrh	r4, [r6, #20]
+mov	r8, r4
 mov	r4, sp
 adds	r4, #6
@@ -288,20 +276,20 @@

 adds	r3, r4, #0
 bl	38c8 <CheckWarioTileCollision>
-adds	r2, r0, #0
+adds	r1, r0, #0
 ldrh	r0, [r5, #0]
 cmp	r0, #0
-beq.n	474e <ResolveWarioCeilingCollision+0x2c2>
-cmp	r2, #0
-beq.n	471c <ResolveWarioCeilingCollision+0x290>
-strh	r2, [r6, #28]
+beq.n	473a <ResolveWarioCeilingCollision+0x2ae>
+cmp	r1, #0
+beq.n	4708 <ResolveWarioCeilingCollision+0x27c>
+strh	r1, [r6, #28]
 ldrh	r0, [r4, #0]
-b.n	4788 <ResolveWarioCeilingCollision+0x2fc>
-.short	0x0000
-ldr	r1, [pc, #36]	@ (4744 <ResolveWarioCeilingCollision+0x2b8>)
+b.n	4774 <ResolveWarioCeilingCollision+0x2e8>
+.short	0x0000
+ldr	r1, [pc, #36]	@ (4730 <ResolveWarioCeilingCollision+0x2a4>)
 ldrh	r0, [r1, #0]
-ldrh	r3, [r4, #0]
-cmp	r0, r3
-bcs.n	4794 <ResolveWarioCeilingCollision+0x308>
-adds	r1, r0, #0
+ldrh	r2, [r4, #0]
+cmp	r0, r2
+bcs.n	477c <ResolveWarioCeilingCollision+0x2f0>
+ldrh	r1, [r1, #0]
 mov	r8, r1
 str	r5, [sp, #0]
@@ -310,25 +298,25 @@

 adds	r3, r4, #0
 bl	38c8 <CheckWarioTileCollision>
-adds	r2, r0, #0
-strh	r2, [r6, #28]
-cmp	r2, #0
-beq.n	4748 <ResolveWarioCeilingCollision+0x2bc>
+adds	r1, r0, #0
+strh	r1, [r6, #28]
+cmp	r1, #0
+beq.n	4734 <ResolveWarioCeilingCollision+0x2a8>
 ldrh	r0, [r4, #0]
-b.n	4788 <ResolveWarioCeilingCollision+0x2fc>
+b.n	4774 <ResolveWarioCeilingCollision+0x2e8>
 .short	0x0000
 .word	0x00000000
 ldrh	r0, [r4, #0]
 adds	r0, #63	@ 0x3f
-b.n	4788 <ResolveWarioCeilingCollision+0x2fc>
-ldr	r0, [pc, #60]	@ (478c <ResolveWarioCeilingCollision+0x300>)
+b.n	4774 <ResolveWarioCeilingCollision+0x2e8>
+mov	r0, sl
 adds	r0, #2
 add	r0, r9
-movs	r4, #0
-ldrsh	r0, [r0, r4]
+movs	r3, #0
+ldrsh	r0, [r0, r3]
 cmp	r0, #31
-ble.n	4794 <ResolveWarioCeilingCollision+0x308>
+ble.n	477c <ResolveWarioCeilingCollision+0x2f0>
 ldrh	r7, [r6, #18]
-add	r0, sp, #8
-str	r0, [sp, #0]
+add	r4, sp, #8
+str	r4, [sp, #0]
 adds	r0, r7, #0
 mov	r1, r8
@@ -337,18 +325,19 @@

 adds	r3, #6
 bl	38c8 <CheckWarioTileCollision>
-adds	r2, r0, #0
-mov	r1, sp
-ldrh	r0, [r1, #8]
-cmp	r0, #0
-beq.n	4794 <ResolveWarioCeilingCollision+0x308>
-ldr	r0, [pc, #20]	@ (4790 <ResolveWarioCeilingCollision+0x304>)
-ldrh	r3, [r1, #6]
+adds	r1, r0, #0
+mov	r2, sp
+ldrh	r0, [r2, #8]
+cmp	r0, #0
+beq.n	477c <ResolveWarioCeilingCollision+0x2f0>
+ldr	r0, [pc, #16]	@ (4778 <ResolveWarioCeilingCollision+0x2ec>)
+ldrh	r3, [r2, #6]
 ldrh	r0, [r0, #0]
 cmp	r0, r3
-bcs.n	4794 <ResolveWarioCeilingCollision+0x308>
-strh	r2, [r6, #28]
+bcs.n	477c <ResolveWarioCeilingCollision+0x2f0>
+strh	r1, [r6, #28]
 subs	r0, r3, #1
 strh	r0, [r6, #20]
-b.n	463c <ResolveWarioCeilingCollision+0x1b0>
+b.n	461e <ResolveWarioCeilingCollision+0x192>
+.word	0x00000000
 movs	r0, #255	@ 0xff
 add	sp, #28
```

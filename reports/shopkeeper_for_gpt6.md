# Verified match   SetupShopkeeperSelectedItem (Codex, 2026-10-16)

This function is now **100% matched**, verified twice with decomp.wtf `compile_and_diff`.
Source commit: `b501a4f` (Match SetupShopkeeperSelectedItem).
The commit changes only `src/sprite_ai/shopkeeper.c`: one insertion, three deletions.
No remaining assembly differences. The earlier report below is preserved as historical investigation, not current status.

## Successful change

Replace:

```c
    flag = 0x80;
    flag <<= 3;
    flagCopy = flag;
```

with:

```c
    flagCopy = 0x400;
```

Keep `register u16 flagCopy asm("r0");`, the u32 status variable, the separate zero variables, and the later use of `flag` as the drawDistanceDown pointer. No inline assembly was added. The existing register-variable declarations remain.

## Exact compiler mechanism established by RTL

The obstacle was narrow-value extension and constant provenance, not reload rotation, branch layout, or pointer lifetimes. The saved 99.3% implementation constructs a constant in a hard SImode register, then copies its low half into a hard HImode register:

```
(set (reg/v:SI 1 r1) (const_int 1024))
(set (reg/v:HI 0 r0) (reg:HI 1 r1))
(set (reg:SI 32) (ashift:SI (reg:SI 0 r0) (const_int 16)))
(set (reg:SI 31) (lshiftrt:SI (reg:SI 32) (const_int 16)))
```

Those shifts survive combine in the baseline. Direct assignment creates a recognized movhi constant with REG_EQUAL:

```
(set (reg/v:HI 0 r0) (const_int 1024))
    (expr_list:REG_EQUAL (const_int 1024) ...)
```

The successful CSE dump still contains the extension pair. The successful **combine** dump deletes the left shift and changes the right shift into a plain SImode register copy. The HImode constant survives until later lowering, which emits the required constant construction in r1 and copy into r0. Both target ORs remain. This refines the older report: u32 status is compatible with the desired HImode constant copy; direct constant assignment preserves enough information for combine to remove extension.

Matched sequence:

```asm
mov r3, #0
mov r5, #0
mov r1, #0x80
lsl r1, #3
mov r0, r1
orr r2, r0
orr r2, r3
strh r2, [r4, #0]
```

## Verification and repository handling

- Initial live progress: 4006/4024, no active worker. Claimed only this function.
- Retrieved full target, best/current source, diff, function notes, note search, previous report, source context and PrimarySpriteData definition.
- Initial compile confirmed score 124 / 99.3%.
- Successful candidate J: tool reported exact match and committed it.
- Repeated the identical source through compile_and_diff: tool confirmed it still matches.
- Inspected git show: source commit contains exactly the three-line replacement above, with no unrelated changes.
- Initial git index was empty. Existing working-tree changes were preserved; shopkeeper.c's pre-existing difference from HEAD was line endings only.
- Compiler flags, headers, tools/agbcc and unrelated functions were not edited.
- Diagnostic compiles used the existing compiler options plus -da, writing dumps under /tmp, without changing configured flags or compiler sources.

## Targeted experiments (all start from original score-124 source)

Ten candidates, plus initial baseline verification and final independent match verification = twelve compile_and_diff calls. Diagnostic RTL compiles of baseline, C and J were additional local compiles, not submitted candidate variants.

| ID | Minimal change | Verified result | Finding |
| --- | --- | --- | --- |
| A | Separate u16 finalStatus = status \| flagCopy \| temp; use finalStatus for store and later default | 98.4%, score 280 | Extensions disappear but copy and zero OR fold; initial operand/order changes |
| B | Store status; field \|= 0x400; status = field \| temp; store again | 97.9%, score 380 | Desired HImode constant copy appears, but extra field load/store and zero OR removed |
| C | Local union {u32 word; u16 half;} initialized from status; half \|= flagCopy; status = word | 99.1%, score 158 | No stack union; combine folds copy/zero OR and swaps zero/sixteen allocation |
| D | status = (u16)(status \| flagCopy); then existing status \|= temp | 99.2%, score 140 | Extensions gone, but copy and zero OR fold; temp zero moves after constant |
| E | D plus volatile hard-register temp | 99.2%, score 141 | Does not prevent folding; stack frame grows from 4 to 8 bytes |
| F | Hard-r0 union flagCopy; word = old; half = flag; OR word into status | 99.1%, score 161 | Constant built directly in r0; copy and zero OR disappear; zero/sixteen registers swap |
| G | field = status \| flagCopy; status = field \| temp; field = status | 98.3%, score 295 | Extra field store/reload remains; copy and zero OR disappear |
| H | status \|= flagCopy & 0x400 | 99.1%, score 145 | Extra AND removed, but extension pair remains; zero/sixteen registers swap |
| I | s16 hard-r0 flagCopy instead of u16 | 99.3%, score 124 | Same shape; LSR becomes ASR, still two unwanted instructions |
| J | flagCopy = 0x400 directly; remove intermediate flag setup | **100%, score 0** | Exact; independently reverified |

No unsuccessful variant was retained. Candidate J is the preserved source.

## Former remaining diff (resolved)

```
2 operand, 2 extra
markers: |=different instruction i=operand differs <=missing (target only) >=extra (yours only)
   TARGET                                      | YOURS
   push {r4, r5, r6, lr}                       | push {r4, r5, r6, lr}
   sub sp, #0x4                                | sub sp, #0x4
 i ldr r4, [pc, #0x74] (->0x464)               | ldr r4, [pc, #0x78] (->0x468)
   ldrh r0, [r4, #0x0]                         | ldrh r0, [r4, #0x0]
 i ldr r2, [pc, #0x74] (->0x468)               | ldr r2, [pc, #0x78] (->0x46c)
   and r2, r0                                  | and r2, r0
   mov r3, #0x0                                | mov r3, #0x0
   ...
   lsl r1, #0x3                                | lsl r1, #0x3
   mov r0, r1                                  | mov r0, r1
 >                                             | lsl r0, #0x10
 >                                             | lsr r0, #0x10
   orr r2, r0                                  | orr r2, r0
   orr r2, r3                                  | orr r2, r3
   ...
```

## Best verified C source (complete)

```c
void SetupShopkeeperSelectedItem(void)
{
    struct PrimarySpriteData* s;
    u32 zero;
    u32 sixteen;
    u32 old;
    u32 status;
    register u32 temp asm("r3");
    register u32 flag asm("r1");
    register u16 flagCopy asm("r0");

    s = &gCurrentSprite;
    old = s->status;
    status = 0xFFFB;
    status &= old;
    temp = 0;
    zero = 0;
    flagCopy = 0x400;
    status |= flagCopy;
    status |= temp;
    s->status = status;
    s->warioCollision = temp;
    flag = (u32)&s->drawDistanceDown;
    *(u8*)flag = 32;
    s->drawDistanceUp = temp;
    {
        u8* p = &s->drawDistanceLeftRight;
        sixteen = 16;
        *p = sixteen;
    }
    s->hitboxExtentUp = 4;
    s->hitboxExtentDown = 4;
    s->hitboxExtentLeft = 4;
    s->hitboxExtentRight = 4;
    s->pOamData = sSelectedShopItemOam;
    s->currentAnimationFrame = temp;
    s->animationTimer = zero;
    s->work0 = temp;
    s->work1 = temp;

    if (gCurrentRoom == 0) {
        s->pose = sixteen;
        ShopkeeperUpdateGraphicsScript(sShopkeeperDefaultGraphicsScript);
        switch (gCurrentShopItem) {
            case 1:
                SpriteSpawnAsChild(188, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 2:
                SpriteSpawnAsChild(190, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 3:
                SpriteSpawnAsChild(191, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 4:
                SpriteSpawnAsChild(192, gCurrentSprite.roomSlot, 0, gCurrentSprite.yPosition, gCurrentSprite.xPosition);
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                break;
            default:
                s->status = zero;
                break;
        }
        return;
    }

    switch (gCurrentShopItem) {
        case 1:
            s = &gCurrentSprite;
            s->pose = 17;
            ShopkeeperUpdateGraphicsScript(sShopkeeperAppleBombGraphicsScript);
            SpawnHighPriorityPrimarySprite(188, s->roomSlot, 0, s->yPosition - 84, s->xPosition + 48);
            SpriteUtilFindSpriteSlotWork3(188);
            break;
        case 2:
            s = &gCurrentSprite;
            s->status |= 8;
            s->pose = 19;
            ShopkeeperUpdateGraphicsScript(sShopkeeperItemApproachGraphicsScript);
            SpawnHighPriorityPrimarySprite(190, s->roomSlot, 0, s->yPosition, s->xPosition - 80);
            SpriteUtilFindSpriteSlotWork3(190);
            break;
        case 3:
            s = &gCurrentSprite;
            s->status |= 8;
            s->pose = 110;
            ShopkeeperUpdateGraphicsScript(sShopkeeperVizormanGraphicsScript);
            SpawnHighPriorityPrimarySprite(191, s->roomSlot, 0, s->yPosition, s->xPosition + 96);
            SpriteUtilFindSpriteSlotWork3(191);
            break;
        case 4:
            s = &gCurrentSprite;
            s->status |= 8;
            s->pose = 114;
            ShopkeeperUpdateGraphicsScript(sShopkeeperItemApproachGraphicsScript);
            SpawnHighPriorityPrimarySprite(192, s->roomSlot, 0, s->yPosition, s->xPosition - 100);
            SpriteUtilFindSpriteSlotWork3(192);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            gCurrentSprite.status |= 8;
            gCurrentSprite.pose = 118;
            ShopkeeperUpdateGraphicsScript(sShopkeeperTransformationGraphicsScript);
            break;
        default:
            if (gCurrentPassage == 5) {
                struct ShopkeeperDmaRegisters* dma = (struct ShopkeeperDmaRegisters*)0x040000D4;
                dma->src = sShopkeeperItemTiles;
                dma->dst = (void*)0x06013000;
                dma->control = 0x80000800;
                (void)dma->control;
                s->status = status | 8;
                s->pOamData = sShopkeeperIdleOam;
                s->pose = 55;
                m4aSongNumStart(191);
            } else {
                s->status = zero;
            }
            break;
    }

    if (gCurrentSprite.status != 0 && gCurrentPassage != 5) {
        if (gCurrentShopItem == 1)
            m4aSongNumStart(171 << 2);
        else
            m4aSongNumStart(0x2AB);
    }
}
```

## Recommendations

No further experiments are needed for this function. Preserve the exact matched implementation.
For similar matching problems, distinguish an explicit narrow constant assignment from a narrow copy of a hard wider register: identical runtime values can carry different RTL constant information. Inspect CSE and combine before pursuing register permutations.

---

# Previous handover (preserved verbatim)

# SetupShopkeeperSelectedItem: constant-materialisation puzzle (for outside review)

This document is self-contained. It concerns one unmatched function in a matching decompilation of *Wario Land 4* (Game Boy Advance), where the goal is C that compiles **byte-for-byte** to the original.

The function is 724 bytes. Control flow, both jump tables and every case body already match. The only real difference is how the first statement builds and ORs in the constant `0x400`.

## Setup

- **Compiler:** agbcc, a GCC 2.95-era ARM/Thumb fork (`version_string = "2.9-arm-000512"`).
- **Flags:** `-O2 -mthumb-interwork -fhex-asm -fprologue-bugfix`. Output is Thumb code for the ARM7TDMI.
- **Rules:**
  - No inline asm statements, including empty `asm("")` barriers.
  - `register T x asm("rN");` pinning **is** allowed.
  - Headers, data and other functions must not change.
- **Status:** 99.3% match (decomp.wtf score 124).
- **Reading the diff:** most of the score is pc-relative literal offsets, which shift by 4 because ours has 2 extra instructions. The real difference is the 2 instructions shown below.
- **Struct:** `gCurrentSprite.status` is a `u16` at offset 0.

## The difference

The function prologue:

```
TARGET                          OURS (saved best)
push {r4, r5, r6, lr}           (same)
sub  sp, #4
ldr  r4, =gCurrentSprite
ldrh r0, [r4, #0]               ; old status
ldr  r2, =0xfffb
ands r2, r0                     ; status = old & 0xFFFB
movs r3, #0                     ; temp (byte zero, used for many strb)
movs r5, #0                     ; zero (halfword zero, used for strh)
movs r1, #128
lsls r1, r1, #3                 ; 0x400 built in r1
adds r0, r1, #0                 ; copied to r0  ("mov r0, r1")
                                lsls r0, r0, #16    <-- extra
                                lsrs r0, r0, #16    <-- extra
orrs r2, r0                     ; status |= 0x400
orrs r2, r3                     ; status |= temp (temp == 0, but not folded)
strh r2, [r4, #0]
...
```

`status` (r2) stays live into the switch's default case. There, `s->status = status | 8` compiles to `movs r0,#8; orrs r2,r0; strh r2,[r4]`, so `status` behaves like a 32-bit register value.

## Current best C (the relevant part; the rest of the function already matches)

```c
void SetupShopkeeperSelectedItem(void)
{
    struct PrimarySpriteData* s;
    u32 zero;
    u32 sixteen;
    u32 old;
    u32 status;
    register u32 temp asm("r3");
    register u32 flag asm("r1");
    register u16 flagCopy asm("r0");

    s = &gCurrentSprite;
    old = s->status;
    status = 0xFFFB;
    status &= old;
    temp = 0;
    zero = 0;
    flag = 0x80;
    flag <<= 3;
    flagCopy = flag;          /* gives the r1 -> r0 copy ... */
    status |= flagCopy;       /* ... but u16 -> u32 adds lsl/lsr #16 */
    status |= temp;
    s->status = status;
    s->warioCollision = temp;
    flag = (u32)&s->drawDistanceDown;
    *(u8*)flag = 32;
    s->drawDistanceUp = temp;
    {
        u8* p = &s->drawDistanceLeftRight;
        sixteen = 16;
        *p = sixteen;
    }
    s->hitboxExtentUp = 4;
    s->hitboxExtentDown = 4;
    s->hitboxExtentLeft = 4;
    s->hitboxExtentRight = 4;
    s->pOamData = sSelectedShopItemOam;
    s->currentAnimationFrame = temp;
    s->animationTimer = zero;
    s->work0 = temp;
    s->work1 = temp;

    if (gCurrentRoom == 0) {
        s->pose = sixteen;
        ShopkeeperUpdateGraphicsScript(sShopkeeperDefaultGraphicsScript);
        switch (gCurrentShopItem) {
            /* cases 1-4: SpriteSpawnAsChild(188/190/191/192, gCurrentSprite.roomSlot, 0,
               gCurrentSprite.yPosition, gCurrentSprite.xPosition); break;
               cases 5-8: break;  default: s->status = zero; */
        }
        return;
    }
    switch (gCurrentShopItem) {
        /* cases 1-4: set pose, ShopkeeperUpdateGraphicsScript, SpawnHighPriorityPrimarySprite,
           SpriteUtilFindSpriteSlotWork3; cases 5-8: status |= 8, pose 118, script;
           default: if (gCurrentPassage == 5) { DMA3 sShopkeeperItemTiles -> 0x06013000,
           0x80000800; s->status = status | 8; s->pOamData = sShopkeeperIdleOam; s->pose = 55;
           m4aSongNumStart(191); } else s->status = zero; */
    }
    if (gCurrentSprite.status != 0 && gCurrentPassage != 5) {
        if (gCurrentShopItem == 1) m4aSongNumStart(171 << 2);
        else m4aSongNumStart(0x2AB);
    }
}
```

## What the compiler is doing

### The copy is a 16-bit move of a constant

The sequence `movs rA,#128; lsls rA,#3; adds rB,rA,#0` is how agbcc materialises **`0x400` in HImode (`u16`)**. A Thumb `movhi` of 1024 is not a valid single instruction. So the value is built in an SImode register and then moved as a halfword. CSE cannot fold it back to a constant, because `(set (reg:HI) (const_int 1024))` would not be recognised.

Two already-matched functions in this project produce the identical sequence:

- **`BossTreasureInit`** (plain C): `gCurrentSprite.status |= SPRITE_STATUS_IGNORE_SPRITE_COLLISION;` (`0x400`) compiles to `mov r2,#0x80; lsl r2,#3; add r0,r2,#0; ... orr r0,r1`.
- **`InitCatbatMineSpawner`**: `register u16 status asm("r0"); int mask = SPRITE_STATUS_IGNORE_SPRITE_COLLISION; status = mask; status |= oldStatus;` produces the same copy, with no extension, because the OR is entirely 16-bit.

In our function the OR target `status` must stay 32-bit (see the `orrs r2, r3` and the later `status | 8`). OR-ing a `u16` register into it forces a zero-extension, the extra `lsl/lsr`. When the OR is done in `u16`, the copy appears correctly, but other things break.

### Two different zero registers

`r3` (`temp`) and `r5` (`zero`) both hold 0. `r3` is used for byte stores and for `orrs r2, r3`; `r5` is used for halfword stores. In the target, `status | temp` is not folded, which suggests `temp`'s value wasn't a known constant to CSE at that point. Our version achieves that with a pinned register.

## Everything tried

### Earlier agents (decomp.wtf scores)

- **Plain-C rewrite, 98.5% (255)** (Claude):
  - `old = s->status; status = 0xFFFB; status &= old;`
  - `register u32 temp asm("r3")`
  - `register u32 flag asm("r1") = 0x80; flag <<= 3; flagCopy = flag;`
  - The remaining diff was the `mov r0, r1` copy, plus `mov r6, #0x10` scheduled early.
- **99.3% (124)** (Codex): the `drawDistanceDown` store written through `flag` reused as a pointer; a block-local `u8* p` for `drawDistanceLeftRight`; and `register u16 flagCopy asm("r0")` (gives the copy, plus the extension).
- **Other Codex results:**
  - Plain `u32 flagCopy`: 132 (constant built directly in `r0`).
  - Pinning `flagCopy` as `u32`, `s32` or a pointer in hard `r0` removes the copy and reorders the zeros.
  - `volatile flag` adds stack space.
  - `u16 flag` changes the zero ordering and operands.

### This session (Claude; local harness, changed disassembly lines; the best scores 114, mostly literal-offset noise)

| # | Change from best | Result |
|---|---|---|
| a0 | `u32 flagCopy` (unpinned) | Constant built directly in `r0`; no copy |
| a1 | `status \|= 0x400` (no `flag`/`flagCopy`) | Same as a0 |
| a2/a4 | `u16 status` (and `u16 old`) | `ands` operand order flips; zeros move to `r6` |
| a3 | a2 + unpinned `u16 flagCopy` | Worse |
| b1/b2 | `u16 status` + `status \|= SPRITE_STATUS_IGNORE_SPRITE_COLLISION` | **Copy correct** (`movs r1; lsls r1; adds r0,r1; orrs r2,r0`), but `orrs r2,r3` folds away and a u16 re-truncation (`lsls/lsrs #16; adds r2,r0`) is added |
| b3 | b1 + `register u8 temp asm("r3")` | Byte stores widen through `r0` |
| b4 | b1 + `register u16 temp asm("r3")` | `bics/orrs` sequence |
| b5 | `u16 bit = 0x400; status \|= bit;` | CSE folds; constant built directly in `r0` |
| c4 | `u16 flagCopy` (unpinned) | Folds |
| c5 | `register u16 status asm("r2")` | `0xFFFB` loaded via `r1` plus copy; the `orr r3` disappears |

## The open question

What C produces both of the following?

1. The OR of `0x400` performed on a **16-bit** value, so the halfword-constant copy `movs r1,#128; lsls r1,#3; adds r0,r1,#0` survives,
2. while the destination `status` is a value that can then be OR'd with an unfolded zero register `r3` (`orrs r2, r3`), stored with `strh`, and later used again as `status | 8`. All of this must happen with **no zero-extension** instructions.

Specific sub-questions:

- Could the original be two separate statements on the struct field? For example `s->status &= ~4; s->status |= 0x400;`, with the `| temp` coming from a third statement or a macro, and with CSE merging the loads and stores? `BossTreasureInit` shows that `field |= 0x400` alone gives the exact copy.
- Is `orrs r2, r3` (`| 0`) perhaps a macro artefact, such as `SPRITE_SET_FLAGS(status, x, 0)`? Or is it a `u8` field's value that happens to be 0 (for example `| s->someByte` after it was stored as 0)?
- How would a GBA-era programmer have written `status` so that it can be reused for the later `s->status = status | 8` without re-reading the field?

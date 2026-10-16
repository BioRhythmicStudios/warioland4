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

# UpdateCatbatPatrol: register-allocation puzzle (for outside review)

This brief describes one unmatched function in a matching decompilation of *Wario Land 4* (Game Boy Advance). The goal is C source that compiles **byte-for-byte** to the original machine code. Every instruction already matches except five register operands. We know the compiler mechanism that causes the mismatch, but not the C source shape that triggers it.

## Setup

- **Compiler:** agbcc, a GCC 2.95-era ARM/Thumb fork (`version_string = "2.9-arm-000512"`).
- **Flags:** `-O2 -mthumb-interwork -fhex-asm -fprologue-bugfix`. Output is Thumb code for the ARM7TDMI.
- **Rules:**
  - No inline asm statements, not even empty `asm("")` barriers.
  - Register-pinned variables (`register T x asm("rN");`) **are** allowed.
  - Headers, data and other functions must not change.
- **Status:** 99.9% match (decomp.wtf score 17). Control flow, instruction count, literal pool and stack frame (`sub sp, #16`, pushes r4–r7 and r8–r10) are all identical.

## The difference

All five differing operands involve the address of `sprite->work3`, at offset `0x2a` in `gCurrentSprite`. They appear right after the call to `UpdateCatbatGraphicsAnimation`.

```
TARGET                           OURS
movs r7, #42                     movs r1, #42
adds r7, r7, r4                  adds r1, r1, r4
mov  ip, r7          ; r12       mov  sl, r1          ; r10
ldrb r2, [r7, #0]                ldrb r2, [r1, #0]
ldr  r5, =sUnk_83CF980           (identical from here...)
lsls r0, r2, #1
adds r0, r0, r5
ldrh r3, [r0, #0]
movs r7, #0
ldrsh r1, [r0, r7]
ldr  r0, =0x7fff
cmp  r1, r0
bne  1f
ldrh r3, [r5, #0]
movs r2, #0
1:
adds r0, r2, #1                  adds r0, r2, #1
mov  r1, ip                      mov  r1, sl
strb r0, [r1, #0]                strb r0, [r1, #0]
```

Two separate effects produce this:

1. **The pointer's home register:** target `r12` (`ip`), ours `r10` (`sl`).
2. **The low temporary that builds and dereferences the pointer:** target `r7`, ours `r1`.

### Other relevant target code (already matched)

`r10` holds the constant 0 used by two byte stores to `gCuckooCondorPendulumLength`, one in each facing branch:

```
ldrh r0, [r4, #8]      ; sprite->yPosition
adds r0, r0, r3        ; + velocity
movs r7, #0
mov  sl, r7            ; r10 = 0 (via r7)
strh r0, [r4, #8]
...
right branch:  ldr r0, =gCuckooCondorPendulumLength ; mov r1, sl ; strb r1, [r0]
left  branch:  ldr r0, =gCuckooCondorPendulumLength ; mov r7, sl ; strb r7, [r0]
```

The prologue stores Wario's x position to two stack slots and reloads one of them later:

```
ldr  r0, =gWarioData ; mov r8, r0
ldrh r1, [r0, #18]
str  r1, [sp, #12]
str  r1, [sp, #8]
ldr  r4, =gCurrentSprite
ldrh r6, [r4, #10] ; mov r9, r6
...
right branch:  ldr r7, [sp, #12] ; cmp r7, r6
left branch:   ldr r0, [sp, #8]  ; cmp r0, r9
```

## Current best C (score 17; only the five operands above differ)

```c
void UpdateCatbatPatrol(void)
{
    struct PrimarySpriteData *sprite;
    register struct WarioData *wario asm("r8");
    int index;
    register u32 rightX asm("r7");
    register struct WarioData *reactionWario asm("r1");
    const s16 *table;
    u32 velocity;
    int signedVelocity;
    volatile struct { u32 left; u32 right; } locals;
    register u32 warioX asm("r1");
    register u32 oldX asm("r6");
    register u32 oldXLeft asm("r9");

    wario = &gWarioData;
    warioX = wario->xPosition;
    locals.right = warioX;
    locals.left = warioX;
    sprite = &gCurrentSprite;
    oldX = sprite->xPosition;
    oldXLeft = oldX;

    UpdateCatbatGraphicsAnimation((const u8 *)sUnk_83CF8FC);
    index = sprite->work3;
    table = sUnk_83CF980;
    velocity = *(const u16 *)&table[index];
    signedVelocity = table[index];
    if (signedVelocity == 0x7FFF) {
        velocity = *(const u16 *)&table[0];
        index = 0;
    }
    sprite->work3 = index + 1;
    {
    register u32 zero asm("r10");
    {
        u32 y;
        y = sprite->yPosition + velocity;
        zero = 0;
        sprite->yPosition = y;
    }

    if (sprite->work2 != 0)
        sprite->work2--;

    if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 100));
        if (gSpriteCollisionTileType == 17) {
            {
                register u8 *length asm("r1");
                register u32 one asm("r0");
                length = &gCuckooCondorPendulumLength;
                one = 1;
                *length = one;
            }
            sprite->disableWarioCollisionTimer = 1;
            {
                u8 slot;
                slot = SpriteUtilFindSpriteSlotOrU8Max(PSPRITE_CATBAT_MINE_SPAWNER);
                if (slot != 0xFF)
                    gSpriteData[slot].disableWarioCollisionTimer = 1;
            }
            sprite->xPosition += 4;
            {
                register u32 status asm("r1");
                status = sprite->status;
                if (!(status & SPRITE_STATUS_ONSCREEN)) {
                {
                register u32 mask asm("r0");
                mask = SPRITE_STATUS_FACING_RIGHT;
                mask ^= status;
                sprite->status = mask;
            }
                UpdateCatbatHitboxFacing();
                sprite->xPosition -= 32;
            }
            }
        } else {
            gCuckooCondorPendulumLength = zero;
            sprite->status &= ~SPRITE_STATUS_HIDDEN;
            if ((rightX = locals.right) > oldX && (reactionWario = wario)->reaction != 8) {
                func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 160));
                if (gSpriteCollisionTileType == 17)
                    sprite->xPosition += 2;
                else
                    sprite->xPosition += 1;
                sprite = &gCurrentSprite;
                if (sprite->health <= 1 && sprite->work2 == 25) {
                    DespawnActiveCatbatProjectile();
                    SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 1, 0, sprite->yPosition,
                        sprite->xPosition, SPRITE_STATUS_FACING_RIGHT);
                }
            } else {
                gCurrentSprite.xPosition += 4;
                gCurrentSprite.currentAnimationFrame++;
                return;
            }
        }
    } else {
        func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition - 100));
        if (gSpriteCollisionTileType == 17) {
            {
                register u8 *length asm("r1");
                register u32 one asm("r0");
                length = &gCuckooCondorPendulumLength;
                one = 1;
                *length = one;
            }
            sprite->disableWarioCollisionTimer = 1;
            {
                u8 slot;
                slot = SpriteUtilFindSpriteSlotOrU8Max(PSPRITE_CATBAT_MINE_SPAWNER);
                if (slot != 0xFF)
                    gSpriteData[slot].disableWarioCollisionTimer = 1;
            }
            sprite->xPosition -= 4;
            {
                register u32 status asm("r1");
                status = sprite->status;
                if (!(status & SPRITE_STATUS_ONSCREEN)) {
                {
                register u32 mask asm("r0");
                mask = SPRITE_STATUS_FACING_RIGHT;
                mask ^= status;
                sprite->status = mask;
            }
                UpdateCatbatHitboxFacing();
                sprite->xPosition += 32;
            }
            }
        } else {
            gCuckooCondorPendulumLength = zero;
            sprite->status &= ~SPRITE_STATUS_HIDDEN;
            if (locals.left < oldXLeft && (reactionWario = wario)->reaction != 8) {
                func_8023BFC(sprite->yPosition, (u16)(sprite->xPosition + 160));
                if (gSpriteCollisionTileType == 17)
                    sprite->xPosition -= 2;
                else
                    sprite->xPosition -= 1;
                sprite = &gCurrentSprite;
                if (sprite->health <= 1 && sprite->work2 == 25) {
                    DespawnActiveCatbatProjectile();
                    SpawnPrimarySpriteWithStatus(PSPRITE_CATBAT_PROJECTILE, 1, 0, sprite->yPosition,
                        sprite->xPosition, 0);
                }
            } else {
                gCurrentSprite.xPosition -= 4;
                gCurrentSprite.currentAnimationFrame++;
                return;
            }
        }
    }

    sprite = &gCurrentSprite;
    if (sprite->work2 == 0 && *(u8 *)&gCuckooCondorPendulumLength == 0) {
        register u16 x asm("r1");
        u32 y;
        if (sprite->status & SPRITE_STATUS_FACING_RIGHT) {
            y = sprite->yPosition;
            x = sprite->xPosition - 160;
        } else {
            y = sprite->yPosition;
            x = sprite->xPosition + 160;
        }
        func_8023BFC(y, (u16)x);
        if (gSpriteCollisionTileType != 17)
            sprite->pose = 17;
    }
    }
}
```

## What the compiler is doing

This was established from agbcc `-dg`/`-dl` dumps and from reading agbcc's source.

### A. The low temporary is a reload register, chosen round-robin

Thumb `ldrb`/`strb` cannot use `r12` as a base, and `adds rd, #imm` cannot target `r12`. So reload builds and dereferences the pointer through a low "reload register" and copies it to `r12`.

agbcc picks reload registers in `allocate_reload_reg` (`reload1.c`, around line 5080). It walks round-robin through `spill_regs`, which is sorted by register number. It starts just after `last_spill_reg`, which carries over from one instruction to the next. It skips registers that hold live pseudos at that instruction. **So the temporary depends on which reloads came earlier in instruction order**, not on any variable declaration.

Spill registers in our build:

| Register | Spilled for |
|---|---|
| `r0` | insns 9, 44, 46 |
| `r7` | insn 63, the `ldrsh` scratch clobber |
| `r1` | insns 100, 111, 380 |

### B. The pointer's home is a global-allocation choice

`global.c find_reg` scans hard registers in plain numeric order (`r0`..`r15`) in two passes. Pass 0 only accepts registers already in `regs_used_so_far` (ever-live or call-clobbered). The pointer pseudo has a high-register preferred class (`pref STACK_REG`).

- **Ours:** `r8` and `r9` conflict (pinned variables). `r10` is already ever-live because of the **pinned** `zero asm("r10")`, so pass 0 takes `r10`.
- **Target:** `r10` cannot have been in use yet when the pointer was allocated, so the scan reached `r12`, which is call-clobbered and therefore accepted in pass 0.

So in the original, the zero was an ordinary pseudo that global allocation placed in `r10` *after* the pointer.

**Verified:** unpinning `zero` (plain `u32 zero;`) does move the pointer to `ip`/`r12`. The low temporary is still `r1`, and `zero` then lands in `r7`.

### C. Why an unpinned zero doesn't land in r10 (allocation priority)

A plain local set once to `0` gets a `REG_EQUIV (const_int 0)` note from `local-alloc.c update_equiv_regs`. That routine also doubles the pseudo's `REG_LIVE_LENGTH` (local-alloc.c line 852), lowering its priority. If the pseudo then gets no hard register, reload just rematerialises the constant.

`global.c allocno_compare` sorts allocation candidates by:

```
priority = (floor_log2(n_refs) * n_refs / live_length) * 10000 * size     (truncated to int)
```

Callee-saved registers in use:

| Register | Holds |
|---|---|
| `r4` | `sprite` |
| `r5` | table, later `&gSpriteCollisionTileType` |
| `r6` | `oldX` |
| `r8` | `wario` |
| `r9` | `oldXLeft` |

That leaves `r7` and `r10` for long-lived values that cross calls.

We replaced the volatile struct with **plain locals** (`u32 right = warioX; u32 left = warioX;`, no `rightX` pin) and unpinned `zero`. The `-dg` dump then shows:

| Pseudo | Role | refs | live length | priority | Allocation |
|---|---|---|---|---|---|
| 30 | `left` copy of Wario x | 2 | 63 | 317 | `r7` in global alloc, then evicted by reload (`r7` needed for the `ldrsh` clobber at insn 63), giving stack slot `sp+8` |
| 29 | `right` copy (our identification) | 2 | 64 | 312 | `r10` |
| 63 | `zero` (REG_EQUIV; live length already doubled) | 3 | 98 | 306 | no hard register; constant rematerialised |
| 39 | work3 pointer | 3 | 28 | high | `r12` (correct) |

The target behaviour we infer fits every observed target instruction:

1. `left` gets `r7`, is evicted by reload, and goes to the stack.
2. `zero` gets `r10`.
3. `right` gets no register and goes to the stack.

The target's `str r1,[sp,#12]` / `str r1,[sp,#8]` and `ldr r7,[sp,#12]` are **spilled pseudos**, reloaded through the round-robin `r7`, not a volatile struct.

For this order, `zero`'s priority has to sit **between `right` (312) and `left` (317)**. With `REG_EQUIV` that means a live length of 95 or less (currently 98). Without `REG_EQUIV` it would be about 3/49, roughly 612. That would allocate `zero` *first*, and it would take `r5` or `r7` instead (confirmed by test T1 below). So the window is narrow.

## Everything tried

### Earlier agents (Codex and earlier Claude sessions)

Scores are decomp.wtf scores; lower is better and 0 is a match. The baseline best is 17.

- **Astra's two-pointer split:** score **444 (97.4%)**, with 79 operand diffs, 2 extra and 2 missing instructions. The idea was `register u8 *loadWork3 asm("r7"); register u8 *savedWork3 asm("r12"); loadWork3 = &sprite->work3; savedWork3 = loadWork3; index = *loadWork3; ... *savedWork3 = index + 1;`. Retention in `r12` worked, but the change added `mov r0, r7` before the load, moved `sprite` to `r5` and `table` to `r7`, and reshuffled allocation throughout the function.
- **Only the pointer pinned to `r12`**, address computed before the field load: 142. The low address became `mov r0, r4; add r0, #42`, and the zero stores swapped `r7`/`r1`.
- **`rightX asm("r7")` pin removed:** 22.
- **Integer address pinned to `r7` plus an `r12` copy:**
  - with `index` pinned to `r2`: 633
  - with `index` plain: 474 (an extra `mov r0, r7`, and `sprite`/`table` moved)
  - reading `sprite->work3` instead of dereferencing the integer: also 474, because CSE unifies the two forms
- **Pointer pinned to `r12` and `table` pinned to `r5`:** 294.
- **Explicit `register u32 next asm("r0") = index + 1; register u8 *dst asm("r1") = &sprite->work3;`:** 31. This fixes the store-copy order, but the address and zero mismatches remain.
- **`rightX` declaration moved into the branch:** 17, no change.
- **`index` read before assigning the pointer:** 140.
- **Signed table load before the unsigned load:** 228.
- **Various `volatile` forms** (on the pointer and on `zero`): worse, because they add stack traffic.

### This session (Claude; local harness)

The harness splices the candidate into the source file, compiles it with the real agbcc, and diffs the function's disassembly against the original object. The metric is the number of changed lines; the current best scores 10 on this metric.

| # | Change from the current best | Result |
|---|---|---|
| v1 | `zero` unpinned (plain `u32`) | Pointer moves to `ip` ✔, `zero` moves to `r7`, the `mov sl` disappears, temporary still `r1`. About 122 lines. |
| v2–v4 | v1, with `zero = 0` moved earlier (before the call / before the index load / before the work3 store) | Identical to v1, because CSE propagates the constant. |
| v5 | No `zero` variable; literal `gCuckooCondorPendulumLength = 0` in both branches | Same as v1. |
| b1 | Volatile struct replaced by plain `u32 right, left`; `rightX` pin removed; `zero` plain | Two pseudos: one on the stack (`sp+8`), the other in `r10`. `zero` gets no register (priority table above). About 124 lines. |
| b2, b3 | b1 with `warioX` unpinned, or with the `right`/`left` declarations swapped | About 124–128 lines. |
| c1–c4 | b1 with `zero` declared as `register u32`, `u8`, `s32` or `u16` | No change. |
| T1 | b1 with `zero = sprite->work0 >> 7`, deliberately non-constant (hypothesis test only; adds code) | `zero` is now allocated early and takes `r5`. Confirms that priority, not conflicts, decides the outcome. |
| Sweep | All 128 combinations of 7 toggles on b1 (listed below) | Best **114** changed lines; nothing came near the current best (10). |

The seven sweep toggles:

- `warioX` pinned or plain
- `right`/`left` assignment order
- `reactionWario` pinned, or plain `wario->reaction`
- `oldXLeft` pinned or plain
- `wario` pinned or plain
- `oldX` pinned or plain
- `zero = 0` before or after the `yPosition` store

## A related success (for calibration)

`UpdateCractusArmSegmentPose24` had the same `mov rX,#off; add rX,base; mov ip,rX; ldrb ..,[rX]` pattern, with `r1` where the target had `r7`.

It matched when the whole index / sentinel / store / position step was **duplicated in both if/else branches**. The compiler cross-jumps the identical tails *after* reload, and the first copy's extra reloads advance the round-robin, so the second copy gets `r7`.

The source *shape*, not register pins, drove the reload choice there, and the same is probably true here.

## The open question

What natural C makes agbcc do all four of the following? No inline asm is allowed; `register ... asm("rN")` pins are.

1. Keep the work3 pointer in a pseudo that global allocation puts in **r12**. That requires `r10` not to be ever-live or already used when the pointer is allocated.
2. Give the zero used by the two `gCuckooCondorPendulumLength` stores its own pseudo, which lands in **r10**. It must be allocated after the pointer and before one of the Wario-x copies: either avoid `REG_EQUIV`, or shift live lengths so its priority falls between about 312 and 317.
3. Leave both Wario-x copies spilled to `sp+12`/`sp+8`.
4. Make the reload rotation reach `r7` for the work3 address (`movs r7,#42; adds r7,r7,r4`). This may follow automatically once the spill-register set and order match the target; in the target, `r7` is the spill register used for the `ldrsh` clobber and for the zero copies.

Specific sub-questions:

- Is there an idiomatic way a GBA-era programmer would write this that produces a zero *without* `REG_EQUIV`? For example: a variable assigned on two paths, a value reused from an earlier computation, or a struct field.
- Could the two Wario-x copies be two separate reads of `gWarioData.xPosition`, or a `u16`/`u32` pair? Either would change their reference counts and live lengths.
- Was the facing-right/left split perhaps written differently in the original, such as a shared helper, duplicated code or a `goto`? That would change instruction order, and with it the round-robin, as it did for Cractus.

---

## Session 2 update (2026-10-16): allocation solved, three reload choices remain

This session applied the lesson from the next sibling function, `UpdateBlackDragonAttack`. That function matched once each branch got its *own* block-local variables, so "the same" value could live in different registers in different branches. In Catbat the two facing halves already use separate variables:

- `right` (stack slot `sp+12`) is compared against `oldX` (`r6`).
- `left` (stack slot `sp+8`) is compared against `oldXLeft` (`r9`).

The per-branch lever therefore became making the two halves' code **slightly different at the RTL level**, to steer the global-allocation priority race described above.

### Region model

The live lengths of the three competing pseudos can be split into regions of RTL instructions:

- **A** is the shared code before the facing split.
- **B_R / B_L** is each half's head, before `gCuckooCondorPendulumLength = zero`.
- **C_R / C_L** runs from that store to the half's x-compare.

That gives:

```
zero_len  = A_z + B_R + B_L          (REG_EQUIV doubles it for the priority)
right_len = A   + B_R + C_R
left_len  = A   + B_L + C_L
```

Equal-priority ties go to the lower pseudo number (`right` < `left` < `zero`).

The target's stack slots fix the required order. `left` sits at `sp+8`, so it was spilled first: it never got a register. `right` sits at `sp+12`, so it was evicted from `r7` by reload later (hence `ldr r7, [sp, #12]`). The priority order must therefore be **right > zero > left**:

| Allocation step | Result |
|---|---|
| `right` | `r7`, later evicted to `sp+12` |
| `zero` | `r10` |
| `left` | no register, `sp+8` |

With `zero` at 3 refs / 98 doubled (priority 306) and `right` at 2/64 (312), the only change needed is to make `left_len` ≥ 66 (priority ≤ 303). In other words, add at least 3 RTL instructions to region **C_L** (left half only), without changing the final machine code.

### Trials (local harness; "lines" = changed disassembly lines; the saved best scores 10 on this metric)

The base for all trials is variant b1: the volatile struct replaced by plain `u32 right; u32 left;` (`right = warioX; left = warioX;`), the `rightX` pin removed, and `zero` unpinned (plain `u32`).

| # | Change | right / left / zero(doubled) live length | Outcome |
|---|---|---|---|
| P1 | Left-half first `func_8023BFC` call with arguments passed through `r0`/`r1`-pinned variables | 64 / 69 / 110 | Pinning *adds* RTL moves (left +6, zero +12). 130 lines |
| P2 | Right-half `status &= ~HIDDEN` rewritten with `r1`/`r0`-pinned temps (adds to C_R) | 66 / 63 / 98 | **`zero` → r10, both x copies on the stack.** But the slots are swapped (`right` spilled first), the mask became `movs r0,#5; negs`, and the address temp is still `r1`. 97 lines |
| P3 | Same rewrite moved to the **left** half, with `mask = 0xFFFB` | 64 / 65 / 98 | One instruction short (`left` 307 > `zero` 306). 124 lines |
| **P4** | P3 with `register u16 status asm("r1")` (the u16 adds one RTL instruction) | **64 / 66 / 98** | **Allocation now identical to the target:** pointer → `ip`/r12, `zero` → r10, `right` → `sp+12` (evicted from r7), `left` → `sp+8`. **16 lines**, all reload-register choices (below) |
| P6a | P4 with the `warioX` pin removed | — | The x load becomes `ldrh r0,[r0,#18]` (the output reload shares the incoming `r0`). 22 lines |
| P6b | P4 with `right = wario->xPosition; left = right;` | — | Same as P6a. 22 lines |
| P7 | P4 with the `reactionWario` pin removed (plain `wario->reaction`) | — | The `wario` reload becomes `mov r0,r8; ldrb r0,[r0]` (shares the load's output); the target uses `r1`. 24 lines |
| P8 | P4 with `register u32 react asm("r0"); (react = wario->reaction) != 8` | — | Same as P7. 24 lines |

**P4's exact change from the "current best C" shown above:**

1. In the declarations, replace `register u32 rightX asm("r7");` and `volatile struct { u32 left; u32 right; } locals;` with `u32 right; u32 left;`. Keep `reactionWario` (r1), `warioX` (r1), `oldX` (r6), `oldXLeft` (r9) and `wario` (r8) pinned.
2. Change `locals.right = warioX; locals.left = warioX;` to `right = warioX; left = warioX;`.
3. Change `register u32 zero asm("r10");` to `u32 zero;` (same position, still set between the `yPosition` add and its store).
4. Right half: `if (right > oldX && (reactionWario = wario)->reaction != 8)`. The status line stays `sprite->status &= ~SPRITE_STATUS_HIDDEN;`.
5. Left half: replace `sprite->status &= ~SPRITE_STATUS_HIDDEN;` with the block below, and use `if (left < oldXLeft && (reactionWario = wario)->reaction != 8)`.

```c
            {
                register u16 status asm("r1");
                register u32 mask asm("r0");
                status = sprite->status;
                mask = 0xFFFB;
                mask &= status;
                sprite->status = mask;
            }
```

**P4's remaining diff (target vs ours):**

```
TARGET                          OURS (P4)
movs r7, #42                    movs r1, #42        ; work3 address temp
adds r7, r7, r4                 adds r1, r1, r4
mov  ip, r7                     mov  ip, r1
ldrb r2, [r7, #0]               ldrb r2, [r1, #0]
...
ldr  r0, =gCuckooCondorPendulumLength   (same)       ; left half
mov  r7, sl                     mov  r1, sl          ; zero copy for strb
strb r7, [r0, #0]               strb r1, [r0, #0]
ldrh r1, [r4, #0] ... strh      (same)
ldr  r0, [sp, #8]               ldr  r7, [sp, #8]    ; left reload
cmp  r0, r9                     cmp  r7, r9
```

### Rotation analysis of the three remaining choices

The spill registers are `{r0, r1, r7}`. Round-robin starts after the last allocated register.

- **Work3 address (want r7).** The only reload before it is the `wario` `ldr r0,=gWarioData; mov r8,r0` (r0), so the next pick is r1. The target must have had one more `r1` reload in between. The obvious candidate is the prologue's `ldrh r1,[r0,#18]` being an *output reload* of the x value into a spilled pseudo, but P6a/P6b make that load share `r0` instead.
- **Left-half zero copy (want r7) and left compare (want r0).** The target's follow path does `mov r1, r8; ldrb r0, [r1]; cmp r0, #8` (`wario->reaction`). If that `mov r1, r8` is a reload, the rotation sits at r1 when the left half begins. The zero copy then gets r7 and the compare reload gets r0, exactly as in the target. In P4 the `r1`-pinned `reactionWario` makes it a plain copy, not a reload. Unpinning it (P7/P8) produces a reload that shares the load's own output `r0`. So the loaded `reaction` byte must have been an allocated pseudo (in r0), not a reload output, and we haven't found C that produces that.

### Updated open question

Starting from P4 (allocation identical to the target), what C makes these two things happen? No inline asm; register pins allowed.

1. Load `wario->xPosition` into the stack-spilled x pseudos through an `r1` reload (output reload not sharing the `r0` address), so the work3 address reload rotates to `r7`.
2. Make `wario->reaction` load into a real pseudo (r0) with the `wario` (r8) address reloaded through `r1`, in both halves.

Both are "reload must not share with the other operand" situations. That suggests the original's intermediate values were ordinary pseudos where ours collapse into reload outputs.

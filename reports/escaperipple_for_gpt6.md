# UpdateEscapeRippleHBlank: register-allocation puzzle (for outside review)

This document is self-contained. It describes one unmatched function in a matching decompilation of *Wario Land 4* (Game Boy Advance). The goal is C that compiles **byte-for-byte** to the original. Everything matches except **two register operands**. The responsible compiler mechanism is now well understood, but the C that produces it is still unknown.

## Setup

- **Compiler:** agbcc, a GCC 2.95-era ARM/Thumb fork (`version_string = "2.9-arm-000512"`), run with `-O2 -mthumb-interwork -fhex-asm -fprologue-bugfix`. Output is Thumb code for the ARM7TDMI.
- **Rules:**
  - No inline asm statements, including empty `asm("")` barriers.
  - `register T x asm("rN");` pinning **is** allowed.
  - Headers, data and other functions must not change.
- **Status:** 99.8% match (decomp.wtf score 5). 100 instructions, of which one pair differs.

## The difference

The difference is in the second loop (the `arg0 != 0` path):

```
TARGET                         OURS
mov  r6, r8                    mov  r6, r8          ; scroll pointer (r8) into a low reg
ldrh r0, [r6, #14]             ldrh r0, [r6, #14]   ; gBackgroundScroll.bg3Vertical
adds r0, r0, r1                adds r0, r0, r1      ; + i
mov  r7, ip                    mov  r6, ip          ; <-- phase pointer (r12) into a low reg
ldrb r7, [r7, #0]              ldrb r7, [r6, #0]    ; <-- gHBlankWavePhase
adds r0, r0, r7                adds r0, r0, r7
ands r0, r5                    (same)
adds r0, r4, r0
ldrb r0, [r0]; lsls; asrs      (same)
ldrh r6, [r2, #0]              (same)               ; *src (gHBlankLineValues)
adds r0, r0, r6
strh r0, [r3, #0]  ...         (same)
```

Before loop 2 the target has `mov ip, r3` (phase pointer into `r12`), `ldr r0,=gBackgroundScroll; mov r8, r0`, `ldr r3,=gHBlankLineOffsets`, `movs r5,#31` and `ldr r2,=gHBlankLineValues`. Loop 1 (the `arg0 == 0` path) already matches, including `mov r6, r3; ... ldrb r7, [r6, #0]`.

## Current best C (score 5)

```c
void UpdateEscapeRippleHBlank(u8 arg0)
{
    register u32 arg_r5 asm("r5");
    u8* state_r2;
    register s32 index_r1 asm("r1");
    register u8* table_r4 asm("r4");
    register u32 temp_r0 asm("r0");

    arg_r5 = arg0;
    state_r2 = gHBlankPrimaryState;

    {
        s32 zero_r3;

        zero_r3 = 0;
        state_r2[0xB] = zero_r3;
        state_r2[0x7] = zero_r3;

        index_r1 = 0;
        if (gEscapeHBlankEffectState > 0xE) {
            index_r1 = 1;
        }

        index_r1 <<= 5;
        temp_r0 = (u32)sHBlankEscapeRipple32;
        table_r4 = (u8*)(index_r1 + temp_r0);

        state_r2[0x3] = zero_r3;

        temp_r0 = ((u16*)state_r2)[0] + 1;
        ((u16*)state_r2)[0] = temp_r0;

        if (((s16)temp_r0) > 3) {
            state_r2[3] = 1;
            ((u16*)state_r2)[0] = zero_r3;
        }
    }

    {
        register u8* phase_r3 asm("r3");

        phase_r3 = &gHBlankWavePhase;
        phase_r3[0] += state_r2[3];

        if (arg_r5 == 0) {
            s32 i_r1;
            register u8* phase_r6 asm("r6");
            register struct BackgroundScroll* scroll_r5 asm("r5");
            register s32 mask_r3 asm("r3");
            s16* dst_r2;

            i_r1 = 0;
            phase_r6 = phase_r3;
            scroll_r5 = &gBackgroundScroll;
            mask_r3 = 0x1F;
            dst_r2 = gHBlankLineOffsets;

            do {
                *dst_r2 = (s8)table_r4[(scroll_r5->bg3Vertical + i_r1 + *phase_r6) & mask_r3];
                dst_r2++;
                i_r1++;
            } while (i_r1 <= 0x9F);
        } else {
            register s32 i_r1 asm("r1");
            register u8* phase_ip asm("r12");
            register struct BackgroundScroll* scroll_r8 asm("r8");
            register s16* dst_r3 asm("r3");
            register s32 mask_r5 asm("r5");
            register u16* src_r2 asm("r2");

            i_r1 = 0;
            phase_ip = phase_r3;
            scroll_r8 = &gBackgroundScroll;
            dst_r3 = gHBlankLineOffsets;
            mask_r5 = 0x1F;
            src_r2 = gHBlankLineValues;

            do {
                register struct BackgroundScroll* scroll_r6 asm("r6");
                register u32 phase_r7 asm("r7");
                register u32 idx_r0 asm("r0");

                scroll_r6 = scroll_r8;
                idx_r0 = scroll_r6->bg3Vertical;
                idx_r0 += i_r1;
                phase_r7 = *phase_ip;
                idx_r0 += phase_r7;
                idx_r0 &= mask_r5;
                *dst_r3 = (s8)table_r4[idx_r0] + *src_r2;

                dst_r3++;
                src_r2++;
                i_r1++;
            } while (i_r1 <= 0x9F);
        }
    }
}
```

## What the compiler is doing

These findings come from agbcc `-dg` dumps and from reading agbcc's source.

1. **Local allocation never hands out `r7`.**
   - `local-alloc.c find_free_reg` excludes every `ELIMINABLE_REGS` "from" register, and `r7` is the Thumb frame pointer.
   - So the target's `mov r7, ip; ldrb r7, [r7]` cannot come from a block-local temporary.
   - Both the low copy of the pointer and the loaded byte must be **reload registers**, or global-allocation pseudos.

2. **Loop 1 shows the target idiom.**
   - Its `ldrb r7, [r6, #0]` is reload insn 279 in the dump: the phase byte is an operand of the add (`... + *phase_r6`), and reload loads it into spill register `r7`.
   - Spill registers in this function are `{r0, r1, r6, r7}`.
   - agbcc chooses reload registers **round-robin** from that list (`reload1.c allocate_reload_reg`), skipping registers that hold live values.

3. **Why our best uses `r6`.**
   - With `phase_ip` pinned to the hard register `r12`, `(mem (reg ip))` is not a legitimate Thumb address.
   - So the **expander** inserts a copy into a new pseudo (block-local, "pointer"), and local allocation gives that pseudo `r6`.
   - That copy is our `mov r6, ip`. It is decided long before reload, so the rotation never gets a say.

4. **What the target probably had.**
   - The phase pointer was an ordinary pseudo that *global* allocation placed in `r12`, so the address copy became a **reload**, rotated to `r7`.
   - The byte was a reload output sharing `r7`, as in loop 1.
   - For global allocation to put a low-preferring pointer in `r12`, it must conflict with every register `r0`–`r7` during loop 2.

5. **Warning: `r7` pins are unsafe.** agbcc apparently does not record conflicts against a user variable pinned to `r7` (variant V7c below produced a miscompile).

## Everything tried

### Earlier agents (decomp.wtf scores; baseline is 5)

**Codex, about 22 compiles, no progress:**

- Removing individual or all inner fixed register bindings.
- A combined `scroll + index + phase` expression.
- A signed phase value.
- A pointer or integer `r7` self-dereference (adds `mov r6, r7`).
- Volatile phase access (an extra move).
- A volatile scroll register (stack overhead).
- Moving the scroll binding outside the loop.
- Binding the source halfword to `r6` with the source read before the phase (the scheduler delays it; same diff).

**Earlier Claude session:**

- `r7` variables typed as pointer, `u8` or `u32`, and casts: all give an extra `mov r6, r7` (97.8%) or worse.
- A dead store to `scroll_r6`: optimised out.

### This session (local harness; "lines" = changed disassembly lines; the best scores 4 on this metric)

| # | Change from best | Lines | Result |
|---|---|---|---|
| V1 | `phase_r7` unpinned (plain `u32`) | 6 | `mov r6, ip; ldrb r6, [r6]`: the target's shape (copy and value tied), but in `r6`, and the copy moves before `adds r0, r1` |
| V2 | `scroll_r6` unpinned | 8 | Scroll-address reload shares the output `r0` (`mov r0, r8; ldrh r0, [r0]`) |
| V3 | V1 + V2 | 10 | Both effects |
| V4 | One `u32 phase` shared by both loops | 6 | CSE splits it back into per-loop temps; same as V1 |
| V6a | `scroll_r6` removed, `idx_r0` unpinned, `phase_r7` kept | 8 | Scroll reload still shares `r0` |
| V6b/V6c | V6a + `idx_r0 += *phase_ip` inline (`idx_r0` plain or pinned) | 10 | Expand-time copy, `r6` shape again |
| V7a | `phase_ip` unpinned + inline phase load | 15 | The address is now a reload, but global allocation puts the pointer in **`r7`** (`adds r7, r3, #0`), not `r12` |
| V7b | V7a + `scroll_r6` removed | 21 | Pointer in `r6`; scroll reload shares `r0` |
| V7c | Only `phase_ip` unpinned | 11 | Pointer in `r7` **and** the pinned `phase_r7` writes `r7` in the loop: `ldrb r7, [r7]` with no copy. **Miscompile** (the pointer is clobbered on the next iteration) |

## The open question

What natural C (no inline asm) makes agbcc do the following?

1. Keep the loop-2 phase pointer as a pseudo that **global** allocation places in **`r12`**. Being an address base, it prefers low registers, so it must conflict with `r0`–`r7` throughout loop 2. Equivalently, `r6` and `r7` must be occupied by allocated values across the loop.
2. Load the phase byte as an **operand of the add**, as loop 1 does, so that both the address copy and the value are reloads. Then the round-robin (spill set `{r0, r1, r6, r7}`, where the scroll-pointer copy `mov r6, r8` is the previous reload) lands on `r7` and the value shares it.
3. Make the scroll copy `mov r6, r8` itself a reload that does **not** share the `ldrh` output `r0`. Every attempt so far makes it share `r0`.

Specific sub-questions:

- Could the two loops be one loop, or a shared helper, in the original? If loop 2 were written as "loop 1 plus `*src`", the pointer variables would be shared and their allocation would change.
- Is `&gHBlankWavePhase` perhaps re-read inside loop 2, as a loop-invariant hoisted by GCC's loop optimiser, rather than being a C variable? That would make the `r12` pseudo compiler-created.
- Is there a known agbcc idiom that keeps an address-only pointer in a high register (`r8`–`r12`) without pinning?

---

## Session 2 (2026-10-16): applying the Catbat lesson (not matched)

UpdateCatbatPatrol matched once its high-register pointer was **unpinned** and the data was read **through the global symbol**. Explanation: pinning a high register forces an expand-time copy and removes a reload event. This session tried the same on loop 2. That took 3 local trials plus RTL dumps; nothing was submitted, and the saved best (score 5) is unchanged.

| # | Change from best | Lines | Result |
|---|---|---|---|
| E1 | Phase read as `idx_r0 += gHBlankWavePhase;` (no `phase_ip`, no `phase_r7`); scroll still pinned `r8`/`r6` | 10 | The value is now an add-operand reload (target shape). But the compiler re-loads the symbol into a **low** register: `ldr r7, =gHBlankWavePhase; ... ldrb r6, [r7]`. The target instead copies the earlier `r3` into `ip` (`mov ip, r3`) and reloads with `mov r7, ip; ldrb r7, [r7]` |
| E2 | E1 + scroll through the symbol (`gBackgroundScroll.bg3Vertical`), `idx_r0` still pinned | 40 | The `r8` scroll pointer and the push of `r8` disappear |
| E3 | E2 with `idx_r0` unpinned | 40 | Same as E2 |

### Why the lesson doesn't transfer directly

Global allocation decides placement partly from a pseudo's *preferred class*, which comes from the instructions that set and use it.

- **Catbat's work3 pointer** was set by `sprite + 42`. A Thumb `add` can write a high register, so in `-dg` it had **`pref STACK_REG`** (high registers) and global allocation put it straight into `r12`.
- **The loop-2 phase pointer here** can only be set by a copy of `r3` or by `ldr` from the literal pool. In every variant (E1 pseudo 75; v7a/v7c pseudo 67) it shows **`pref LO_REGS`**, so it lands in `r6`/`r7`.

For the target's `mov ip, r3` … `mov r7, ip; ldrb r7, [r7]`, the loop-2 pointer must end up in `ip`. Either its setter must give it a high-register preference, or reload must evict it from `r7`, which needs `r7` as a spill register at an instruction where the pointer is live.

### Updated open question

What natural C makes a copy of `&gHBlankWavePhase` taken before `dst_r3 = gHBlankLineOffsets` live in **`ip`** across loop 2, without pinning it? In other words, what makes the pointer high-preferring or evictable, while the phase byte stays an add-operand reload?

---

## Session 3 (2026-10-16): GPT-6 plan (pointer arithmetic for register-class preference)

This session followed `reports/escaperipple_gpt6_plan.md`. It used **3 compiles**, within the 3–5 limit. All were local, with `-dl -dg` RTL dumps; nothing was submitted, and the saved best (score 5, 99.8%) is unchanged. No `r7` pins were used.

### Base for the candidates

The candidates are only meaningful with the loop-2 phase pointer **unpinned**: with `asm("r12")`, its preference class is irrelevant. So all three start from Session 1's **v7a**:

- `u8* phase_ip;` is a plain variable, assigned from `phase_r3` before `dst_r3 = gHBlankLineOffsets`.
- The phase byte is read inline (`idx_r0 += *phase_ip;`), so it's an add-operand reload.
- There is **no `r7` pin**. Session 1's v7c, which pinned `phase_r7` to `r7`, was the miscompile.

Each candidate changes only the `phase_ip = ...` line (C also changes the read). In `-dg`, the pointer is pseudo 67 in all three.

### Results

| # | Source | RTL after optimisation (`.lreg`) | Preferred class | Global allocation | Asm vs target | Lines |
|---|---|---|---|---|---|---|
| A | `phase_ip = phase_r3 + 0;` | **Collapsed** to a plain copy `(set (reg 67) (reg r3))`; the `+ 0` folds away at expansion | `LO_REGS` | `r7` | `adds r7, r3, #0` (target `mov ip, r3`); `ldrb r6, [r7]` (target `mov r7, ip; ldrb r7, [r7]`). Identical to v7a | 15 |
| B | `i_r1 = 0; phase_ip = &phase_r3[i_r1];` | **Survives** as `(set (reg 67) (plus (reg r3) (reg r1)))`: `i_r1` is a pinned hard register, so CSE doesn't fold the zero | `LO_REGS` | `r7` | Emits `adds r7, r3, r1` (a real 3-operand add) instead of `mov ip, r3`; loop body same as A | 15 |
| C | `phase_ip = phase_r3 + 1;` … `idx_r0 += phase_ip[-1];` | **Survives** as `(set (reg 67) (plus (reg r3) (const_int 1)))`; the loop optimiser hoists the `- 1` into a new pseudo 77 (`(plus (reg 67) (const_int -1))`) before the loop | `LO_REGS` (pseudo 67, put in `r0`) | 67 → `r0`, 77 → `r7` | Two extra instructions: `adds r0, r3, #1` … `subs r7, r0, #1`; loop body `ldrb r6, [r7]` | 14 |

**No candidate changed the pointer's preference from `LO_REGS` to `STACK_REG`.** In each case global allocation chose a low register itself; no reload moved the pointer to `r12`. None reproduced the target's `mov ip, r3` without extra instructions.

### Why this family can't work

Thumb's `addsi3` (`tools/agbcc/gcc/thumb.md`, line 595) has these alternatives:

```
op0: "=l,l,l,*r,*h,l,!k"   op1: "%0,0,l,*0,*0,!k,!k"   op2: "I,J,lL,*h,*r,!M,!O"
```

The only alternatives allowing a **high** destination (`*r`/`*h`) are the two-operand forms, where the destination is tied to `op1` and `op2` is a register. So a pointer can only pick up a high preference from an `add` if that add **survives** into the final code. But the target's set of the pointer is a bare `mov ip, r3`, with no arithmetic.

That leaves two outcomes, both seen here:

- The arithmetic collapses to a copy (A), which gives `LO_REGS`.
- The arithmetic survives (B, C), which costs at least one instruction the target doesn't have.

Catbat's pointer got `STACK_REG` because its set really was an addition that stays in the target (`movs r7, #42; adds r7, r7, r4; mov ip, r7`). EscapeRipple has nothing equivalent.

### Remaining route

The pointer must get into `ip` with only a register copy. That means either:

1. it is **evicted from `r7` by reload**, needing `r7` as a spill register at a loop-2 instruction where the pointer is live (and `r6` costlier at that point); or
2. its preference becomes high through its **uses** rather than its set. For example, if the pointer's other references in the function (outside loop 2) are high-friendly operations such as moves or comparisons, rather than memory bases.

Neither has a known natural source form yet. The saved best's exact remaining diff is unchanged:

```
TARGET                         OURS (saved best, score 5)
mov  r7, ip                    mov  r6, ip
ldrb r7, [r7, #0]              ldrb r7, [r6, #0]
```

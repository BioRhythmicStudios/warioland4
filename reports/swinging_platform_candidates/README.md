# func_804A508 natural-loop candidates (session 15)

Each file is one complete `func_804A508` definition. "Lines" = changed lines from the local `try.py` diff against `expected/` (the saved best O, score 165, is 46 lines). None of these beats the saved best yet; the saved best in `src/` is unchanged.

| File | Change from previous | Lines | Result |
|---|---|---|---|
| C1 | Index loop `for (s32 i = 0; i < 24; i++)`, all accesses `gSpriteData[i].field`, break after processing; session-12 prologue, best-O post-loop | 256 | No pre-loop compare, r2 pointer `adds r2,#44`, no muls, in-loop bound `movs #253; lsls #2; add ip; cmp; bgt` exactly as target. work1 pointer in r10 (not spilled), offset60 spilled |
| C2 | `wario` variable removed, `gWarioData.` used in the loop | 258 | `ldr =gWarioData` now at the target's position (after the work1 pointer setup) |
| C3 | `gUnk_3000A4D = 0` instead of re-reading work1 (frame 59/119) | 204 | work1 refs drop, so offset60 wins r10 and work1 spills to sp+24; slots sp+0..24 and frame 0x1c match |
| C4 | `frameStore` pinned `asm("r3")` | 184 | CSE no longer reuses the prologue's `&gUnk_3000A4B` pseudo for the loop invariant (hard regs aren't recorded under SMALL_REGISTER_CLASSES); prologue `mov r3,sl; adds r3,r3,r1; str r3,[sp,#8]` matches |
| C5 | work1 read through `register u8 *workPointer` r4 (mode 1) / r3 (mode 0) | 159 | wario r6 and xDelta r7 now match (hard r3/r4 conflicts). Regression: work1 giv becomes `i*44` from 0 with `gSpriteData+40` added in the loop. frame*2/offset48 are r4/r3 (target r3/r4) |
| C6 | C5 with a pseudo `wp` between the address and the pinned pointer | 159 | Identical code to C5; the intermediary doesn't restore the work1 giv |
| C7 | From C4: all 8 in-loop table loads as `p = &tbl[idx]; register s32 zero asm("rN") = 0; *(p + zero)` | 306 | Rejected. Hard-register pins inside the loop change loop.c's invariant hoisting: the Y table pointers are no longer hoisted, frame drops to 8 |
| C8 | From C4: pinned-zero form on the 4 X loads only | 278 | Rejected. The X addresses are hoisted in place of the Y pointers |
| C9 | From C4: the three X loads that use frame*2/offset48 read into `xLoad` inside `do { } while (0)` (double-nested for the `frame - 48` load) | **24** | Best so far. Loop notes raise REG_N_REFS (flow adds loop_depth per ref), so frame*2 (.48) and offset48 (.375) sort above frame/wario/xDelta; global alloc then gives frame*2 r3, offset48 r4, wario r6, xDelta r7 exactly as target. Left: mode/work1-setup reload reg r4 vs r0, xPosition add operand order (3), two post-loop zero indices |
| C10 | C9 with `x = xDelta + x` for the three xPosition adds | 24 | No change; the operand order is canonicalised |
| C11 | C9 with session-12's `xPosition = (xDelta) - (-(xPosition))` form for the three xPosition adds | **18** | **decomp.wtf score 25 (99.8%), new saved best, now in `src/`.** Left: output reload r4 (target r0) for `mode` and the work1-pointer setup, plus two post-loop ldrsh zero indices (r3/r4, target r2) |
| C12 | C11 with `mode` loaded through `register u32 modeValue asm("r0")` | 18 | Identical code; combine folds the hard-r0 copy away |
| C13 | C11 with `volatile u32 mode` (a real stack variable) | 89 | Fixes the `mode` store (`ldrb r0,[r4]; str r0,[sp]`), but mode's in-loop reload event disappears, every later reload rotates one step and two X loads cross-jump. Shows the target's `mode` is a spilled pseudo like ours |
| C14 | C11 with `modePtr` unpinned | 20 | Address and output both r0 (local-alloc gives modePtr r0; output reuses it). Target: address r4, output r0 |
| C15 | C11 with `mode = spriteBase->work2` (no modePtr) | 20 | Same code as C14 |
| C16 | C11 with the post-loop mode1-low X load inside `do { } while (0)` | 18 | No change: combine still folds the hard-r2 zero (LOG_LINKS exist within the basic block) |
| C17 | C11 with plain C for the four post-loop position updates (`current->yPosition += tbl[idx] * 4` etc.) | 221 | Rejected: changes global allocation and the whole rotation |

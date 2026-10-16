# Matching progress: Wario Land 4 (decomp.wtf project `warioland4-2`)

Shared log for Claude Code, GPT-6 and Codex. Update it after every session, and record **where** each improvement lives:

- **Git**: committed on branch `decomp-wtf`.
- **Working tree**: present in the local `src/` files but not committed.
- **decomp.wtf saved best**: the "best attempt" stored in the decomp.wtf database. It can differ from the local file.

## Status (updated 2026-10-16, session 15, by Claude via decomp.wtf MCP; count unchanged since session 13)

- **4,009 / 4,024 functions matched, 97.1% of code** (decomp.wtf `progress()`). It was 4,000 at the start of 2026-10-16. Matched that day: UpdateCractusArmSegmentPose24, CheckWarioHorizontalCollision, UpdateBlackDragonAttack, UpdateCatbatPatrol and func_800A43C (Claude); UpdateTogenobiOuterPartPosition, SetupShopkeeperSelectedItem and FlatWarioFloating (Codex); DrawHomerunDerby (Codex's code, recorded in session 13 after a reference-object fix; see the session log).
- **15 unmatched:** 12 have been scored and 3 are blocked by SWI (never scored).
- `project_info()` still reports a stale 3,994 / 95.28%. Use `progress()` and `list_functions()`, which agree with `tools/objcmp.py`.

## Build check (how to confirm nothing regressed)

- `make check` (via WSL) **builds and links**, but the ROM MD5 **does not match**, and won't until all unmatched functions match. The unmatched C is compiled straight in (there's no NONMATCHING/asm fallback), and `AgbMain` near the start of ROM has a different size, so everything after it shifts. The last 100% report (`report.json`, 2026-10-15 13:40) predates the asm-free rewrites.
- The real regression check is per function. Build every objdiff unit (`make -k build/us/objdiff/...`), then compare each function's disassembly with `expected/us/obj/*.o`, normalising branch targets, jump tables and `.has_asm` symbol suffixes. The script is `tools/objcmp.py` (committed; usage in its docstring).
  - 2026-10-16 before work: exactly the 24 unmatched differed and all 4,000 matched were identical.
  - 2026-10-16 after two matches: exactly 22 differ.
  - 2026-10-16 session 13: exactly 15 differ, the same 15 that decomp.wtf lists as open.
  - objcmp compares disassembly text, so a stale mapping symbol (`$d`) or a raw address in place of a relocation in `expected/` shows up as a difference even when the bytes are equal. If it disagrees with decomp.wtf, check raw bytes against the ROM (see session 13).
- Nobody currently has a function claimed.
- Three functions need an inline SWI and can't match under the no-inline-asm rule: `AgbMain`, `WaitForVBlankInterrupt` (`swi 2`) and `MusicPlayerJumpTableCopy` (`swi 0x2A`). The realistic ceiling is therefore **4,021**, unless the rule is relaxed for SWI.

## Git and working-tree state

- Branch `decomp-wtf`. The session-13 snapshot commit ("Snapshot working-tree bests, reports and progress log") holds every working-tree best attempt (all open functions), the GPT-6/Codex reports and both handover files. After it, the working tree is clean apart from `tools/agbcc`. Backup branch `backup-before-codex` = `9862bed`.
- `myfork/decomp-wtf` = `d92fce8` (session 13 commits plus the path clean-up, pushed without force and confirmed with `git ls-remote`).
- Session 14: `src/sprite_ai/swinging_platform.c` has an **uncommitted** working-tree change: func_804A84C's new best (score 165, written by `compile_and_diff`). It equals the decomp.wtf saved best. func_804A508 is unchanged (HEAD = saved best 165). Session 14's func_804A84C change was committed in `e4a35e0`.
- Session 15: func_804A508 (score 25) and the session 15 reports were committed in `6f94b8c` and pushed to `myfork/decomp-wtf` without force (confirmed with `git ls-remote`). func_804A84C (score 25) was committed separately after it; both equal their decomp.wtf saved bests.
- `tools/agbcc` is a dirty submodule (`gcc_arm/obstack.c`, `gcc_arm/splay-tree.c`) and was deliberately left untouched and uncommitted.
- decomp.wtf's "current source" for each function **is the local working-tree file**. It builds from `src/`. `compile_and_diff` writes the function into `src/` and **auto-commits on a match**, so it is not a read-only check.
- `expected/` is gitignored and local only. Back it up before changing it. The original `expected/us/obj/minigames/homerun_derby.o` is kept as `homerun_derby.o.bak-asm-era-2026-10-15` next to it.

## Local file vs decomp.wtf saved best

Checked in session 13. For 13 of the 15 open functions the local file and the saved best are the same text. The other two differ in text but **compile to byte-identical objects** (verified with the project's exact compile commands), so their scores are equal. The cleaner local versions are kept.

| Function | Difference |
|---|---|
| DrawRoulette (6984) | The saved best uses byte-offset loads (`*(s16 *)((u8 *)p + zero)`) and `(s16 *)&stack + 2` in 13 places; the local file uses plain field access and `&stack.affineC`. |
| WarioHopInit (1004) | The saved best has an unused `u32 i; i = 0;`; the local file removed it. |

## Unmatched functions (best score: lower is better, 0 = match)

| Function | Best | Match % | Remaining diff (summary) |
|---|---|---|---|
| ~~CheckWarioHorizontalCollision~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit 4e06c44** |
| ~~UpdateCractusArmSegmentPose24~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit 91ba73b** |
| UpdateEscapeRippleHBlank (parked) | 5 | 99.8 | 2 operands: target `mov r7, r12; ldrb r7, [r7]`, ours `mov r6, r12; ldrb r7, [r6]` |
| ~~FlatWarioFloating~~ | 0 | 100 | **MATCHED 2026-10-16 (Codex), commit 0e4db9a**. Final time check duplicated in both arms of `if (counter)` |
| ~~UpdateCatbatPatrol~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude, from GPT-6's plan), commit 8654129** |
| UpdateLayeredSubGameCutscene | 85 | 99.7 | Case 3 BG0 stores. The 85 version uses a `(u16)-2` literal hack; Codex prefers its cleaner 133 version |
| ~~SetupShopkeeperSelectedItem~~ | 0 | 100 | **MATCHED 2026-10-16 (Codex), commit b501a4f** |
| ~~UpdateTogenobiOuterPartPosition~~ | 0 | 100 | **MATCHED 2026-10-16 (Codex), commit 19ffaff**. `sSinCosTable` used directly on the second lookup |
| ~~UpdateBlackDragonAttack~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit 8be1759** |
| ~~func_800A43C~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit b0321e0** |
| ~~DrawHomerunDerby~~ | 0 | 100 | **MATCHED**: code by Codex (commit 89a6f56); recorded by decomp.wtf in session 13 after the `expected/` fix (commit 7fc2668) |
| func_804A508 | 25 | 99.8 | Session 15: natural index loop plus `do { } while (0)` X-load wrappers (165 → 25). Left: 9 operand diffs, all reload register choices (two prologue output reloads r4 vs r0, two post-loop ldrsh zero indices). Before: Codex improved it from 425. Missing `mov r3, r10` at the first `mode1HighY` stack store; X-address and ldrsh scratch registers. Session 14: the `locals` struct form can't produce the `mov r3, r10` (needs a spilled pseudo); next step is the natural loop. See `reports/swinging_platform_for_gpt6.md` |
| func_804A84C | 25 | 99.8 | Session 15: ported func_804A508's natural-loop source (AC→AD, six child tables), 165 → 25. Same 9-operand reload residual as the twin. Before: session 14 ported Codex's changes (370 → 165) |
| WarioHopDrawSprites | 709 | 98.5 | Register scheduling throughout |
| WarioHopInit | 1004 | 95.5 | Register allocation of hoisted DMA sources and struct copies |
| UpdateBossDoorOpeningSprites | 1235 | 97.9 | Limit init registers; stack slot offsets |
| RefreshRouletteResultTiles | 1690 | 80.3 | Loop form and register allocation (extra `r9` saved) |
| ResolveWarioCeilingCollision | 2236 | 88.7 | Codex improved it from 4044. Stack-slot order of right/offset/savedX; pointValue spill width. See `reports/ceiling_collision_for_gpt6.md` |
| DrawRoulette | 6984 | 87.7 | Large; register allocation |
| RenderPauseScreenOam | 28807 | 45.4 | **Incomplete**: the last ~505 target instructions are not written yet |
| DrawSoundRoomSprites | 29282 | 42.9 | **Incomplete**: menu-state branches are missing |
| AgbMain | – | – | Blocked: inline `svc 2` |
| WaitForVBlankInterrupt | – | – | Blocked: inline `svc 2` |
| MusicPlayerJumpTableCopy | – | – | Blocked: whole body is `swi 0x2A` |

## Reload register rotation (CONFIRMED 2026-10-16; produced both matches)

agbcc's `allocate_reload_reg` (tools/agbcc/gcc/reload1.c ~line 5080) picks reload registers **round-robin** from `spill_regs`. That set is sorted by register number, e.g. `{r0, r1, r6, r7}`, and the search starts after `last_spill_reg`, skipping registers that hold live values at that instruction. So a reload's register depends on **which reloads came earlier in instruction order**. Fixes therefore come from adding or removing earlier reloads, not from pinning the temp.

- **Diagnose:** preprocess the file (`cc -E -Iinclude -nostdinc -DVERSION_US`), then run `tools/agbcc/agbcc -O2 -mthumb-interwork -fhex-asm -fprologue-bugfix x.i -o x.s -dg`. In `x.i.greg`, `Spilling for insn N. Spilling reg R.` lines give the spill set, and the RTL shows which insns are reload insns.
- **Cractus:** the original duplicated the whole x-step in both if/else branches. The compiler cross-jumps the identical tails after reload, but the IF copy's reloads move the rotation so the ELSE copy gets `r7`.
- **CheckWarioHorizontalCollision:**
  - The target's `ldr r0,[sp,#4]` is a reload of a stack-spilled pseudo. It's emulated by loading into a variable pinned to a high register (`asm("r12")`), which forces a reload with no extra code.
  - `mov r4,r8` is a reload of `index`. Writing it plainly, without a pinned `sideCopy`, makes `r4` a spill register.

## Shared mechanism behind most near-misses (original hypothesis)

Thumb `ldrb` and `ldrsh` need a low base register, and `ldrsh` has no immediate offset. So when a pointer lives in `r12`, agbcc's **reload pass** copies it into a low *reload register* (`mov rX, r12`), and it materialises the zero offset into one too (`mov rX, #0`).

Most of the remaining operand diffs are this reload-register choice, not ordinary allocation:

- **Catbat, Cractus, BlackDragon** (same work-index → `0x7FFF`-sentinel table idiom): the target's address temp is the **same register** later used for the `ldrsh` zero (`mov r7, #0x2a; add r7, r4; mov r12, r7; ldrb r2, [r7] … mov r7, #0; ldrsh r1, [r0, r7]`). That suggests one reload register serving both.
- **UpdateEscapeRippleHBlank**: `mov r7, r12; ldrb r7, [r7]`. The reload register equals the load's destination, so the output register is being reused.
- **func_800A43C, func_804A508/84C, DrawHomerunDerby**: the `ldrsh` zero-index register differs.

Reload picks registers by spill cost: which hard registers hold live values at that instruction. So the fix is more likely to come from changing **what is live** at that point than from pinning the temp. Pinning has been tried many times and makes it worse.

The already-matched siblings `MoveBlackDragonIntoAttackPosition` and `MoveLargeLipsIntoPosition` (src/sprite_ai/shopkeeper.c) use this idiom with `indexPtr asm("r12")` + `zero asm("r6")`. Note that those matched functions also contain `asm("")` barriers, which the current rules forbid in new work.

## Recommended next (updated after session 2)

1. ~~UpdateCatbatPatrol~~: matched in session 7 (commit 8654129), following GPT-6's Experiment A.
2. ~~UpdateBlackDragonAttack~~: matched in session 4. Its lesson (separate variables per branch) may also apply to Catbat's duplicated facing-right/left branches: try branch-local `zero`/x variables there.
3. **UpdateEscapeRippleHBlank** (5): **PARKED, 2 bytes off** (ROM offsets 0x9c and 0x9e). Codex session 4 (6 candidates plus a read-only allocator audit) found no justified new mechanism; see `reports/escaperipple_for_gpt6.md` Session 4 and `reports/escaperipple_codex_results.md`. Resume only with a new mechanism or a matching analogue.
4. ~~func_800A43C~~ matched in session 11. ~~DrawHomerunDerby~~: 663 -> 174 (session 12), byte-exact by Codex (89a6f56), recorded as matched in session 13.
5. **func_804A508 / func_804A84C**: session 12 found that the original is a natural loop optimized by loop.c: hoisted, spilled table addresses and a strength-reduced `&sprite->work1` giv at sp+24. A natural rewrite (N1) reproduces the prologue layout; the loop form is still open. See `reports/func_804A508_for_gpt6.md`.

### Original recommendations (session 1, kept for history)

1. **CheckWarioHorizontalCollision** (score 3, 1 operand). The closest of all. Codex's 14 experiments are listed in its decomp.wtf note; don't repeat them. Untested direction: in the 2nd probe block, keep `index` (r8) itself unreferenced after `sideCopy = row`, so copy propagation has no `r0 == index` equivalence to use, for example by computing `doubled` from `sideCopy` *after* `mov r4, r8` without touching `row`. Codex's experiment 13 came close (ADD exact, but `mov r4, r0`).
2. **UpdateCractusArmSegmentPose24** (score 11, 4 operands in one 4-instruction block), then apply the same fix to **UpdateCatbatPatrol** (17) and **UpdateBlackDragonAttack** (286), which share the idiom. Untested idea: one variable carries the work-index address *and* is later reused as the `ldrsh` zero offset, so both reloads share a register as in the target. Don't repeat Astra's two-pointer `r7`/`r12` split (Catbat: 97.4%, score 444).
3. **UpdateEscapeRippleHBlank** (score 5, 2 operands). Treat it as a reload-register problem: the target reuses the load's own destination `r7` as the base. Both Codex (about 22 compiles) and Claude tried pinning variants; untested is leaving `phase_r7` as an ordinary variable while keeping `r6` live (for example `scroll_r6` used after the load), so `r6` is no longer the cheapest reload register.

Deprioritised:
- FlatWarioFloating: exhaustively tried; wait for a genuinely new idea from GPT-6.
- SetupShopkeeperSelectedItem (124): a reasonable fourth.
- The two incomplete functions (RenderPauseScreenOam, DrawSoundRoomSprites): need structural work, not register tuning.

## Session log

- **2026-10-16, Claude Code (session 16): func_804A508 reload rotation; no improvement (best 25 unchanged).**
  - Used 5 local trials (C13-C17) plus 4 diagnostic compiler runs (3 reload traces with Codex's ptrace tracer, 1 `-da` dump). Nothing submitted; src/ unchanged. Candidates in `reports/swinging_platform_candidates/`, write-up in the session 16 section of `reports/swinging_platform_for_gpt6.md`.
  - The tracer (`trace_reload.py`, copied from the Derby scratch folder and generalised) shows every rotation event in C11 except three already matches the target. `spill_regs` = [r0..r4].
  - **Prologue:** `mode`'s output reuses the dying pinned `modePtr` r4 (reload.c single-output rule), not a rotation event. The work1 setup (insn 1450) is an event at cursor 3 → r4; the target's r0 needs r4 busy there or no event. If 1450 gets r0, all later events stay as they are.
  - **Post-loop:** at 911 and 1121 the target's `movs r2,#0; ldrsh r0,[rA,r2]` must be a `(plus reg reg)` address with a real zero register (the pattern then hides the scratch event, so the events still match ours). Combine folds our pinned `zero asm("r2")`, because flow links hard-reg sets to their uses inside a basic block.
  - Rejected: volatile stack `mode` (C13, shifts the in-loop rotation), unpinned/direct `mode` reads (C14/C15), `do { } while (0)` around the post-loop load (C16), plain-C post-loop updates (C17, 221 lines).
  - **Next:** find a zero register that survives combine, and a work1 giv-init shape with no reload event; check each candidate with the tracer.
- **2026-10-16, Claude Code (session 15): func_804A508 165 → 25; not matched.**
  - Budget: about 15 compiles. Used 14: 12 local trials (`try.py`), 1 RTL dump and 1 decomp.wtf compile. Candidates and a per-step table are in `reports/swinging_platform_candidates/`; the write-up is the session 15 section of `reports/swinging_platform_for_gpt6.md`.
  - **Loop form (C1).** `for (i = 0; i < 24; i++)` with every access written `gSpriteData[i].field` and a `break` after the match gives the target loop exactly: no entry test, an r2 pointer with `adds #44`, no muls, and the in-loop bound `movs #253; lsls #2; add ip; cmp; bgt`.
  - **Stack and prologue (C2-C4).** `gWarioData` is used directly; frame 59/119 store `0`, not the re-read work1. `frameStore` is pinned to r3 (cse.c doesn't record non-fixed hard regs under SMALL_REGISTER_CLASSES). Together these give frame 0x1c, slots sp+0..24 and the `mov r3,sl; adds r3,r3,r1; str r3,[sp,#8]` prologue.
  - **Register permutation (C9, the key finding).** global.c `find_reg` pass 0 only takes registers already in `regs_used_so_far`, so the target's frame*2 r3 / offset48 r4 / wario r6 / xDelta r7 needs frame*2 and offset48 allocated before wario. The X loads that use them are written `do { xLoad = tbl[idx]; } while (0);` (double-nested for `frame - 48`). The loop notes add depth to those refs (flow.c `REG_N_REFS += loop_depth`), so their priority rises above wario's (.48 and .375 against .333).
  - **xPosition adds (C11).** Session 12's `(xDelta) - (-(x))` form fixes the operand order. Submitted: **score 25**, saved best.
  - Rejected: C5/C6 (pinned work1 pointers break the giv), C7/C8 (hard pins inside the loop change hoisting), C10 (`xDelta + x` is canonicalised), C12 (an r0-pinned mode temp is folded by combine).
  - **Left (9 operands):** the output reloads for `mode` (insn 56) and the work1-pointer setup (insn 1450) take r4 (target r0), and the two post-loop ldrsh zero indices take r3/r4 (target r2). All are reload rotation (`last_spill_reg`).
  - **func_804A84C (1 decomp.wtf compile, plus 1 local trial).** Ported C11 unchanged apart from AC→AD and the six child tables: **165 → 25**, saved best, with the same 9 operand differences as the twin. `tools/objcmp.py` after a full objdiff build: 4,024 compared, the same 15 differ, every matched function identical.
  - **Next:** the remaining reloads in both functions. Try the DrawHomerunDerby technique: coordinate when values are materialised so reload's `last_spill_reg` rotation lands on r0 for the `mode` and work1-pointer output reloads and on r2 for the two post-loop zero indices. A fix should port 1:1 between the twins.

- **2026-10-16, Claude Code (session 14): func_804A84C 370 → 165; func_804A508 root cause found, not matched.**
  - Budget: about 20 compiles. Used 12: 1 decomp.wtf compile plus 11 local trials (`try.py`), with RTL dumps (`-dc -dl -dg`) of already-compiled variants.
  - **func_804A84C (1 submitted compile).** Ported Codex's func_804A508 changes: volatile private-pointer Y loads, pinned `xTable` r1 / `xAddress` r0, `flagValue = work` on frame 119, and the post-loop mode1-low pins. Score went from 370 to **165**, saved as best; it is now in the working tree but not committed (no match). Its remaining diff is the same as its twin's.
  - **func_804A508 (8 local trials, nothing submitted, best 165 unchanged).** What causes the first diff:
    - Combine folds `offset <<= 1` into `offset60 = offset`, so `r0` is an *output reload* of offset60 (r10). Reload then *inherits* `r0` for the `mode1HighY` add (`adds r0,r0,r1`), where the target re-reads r10 (`mov r3,sl; adds r3,r3,r1`).
    - E1 (offset60 `asm("r10")`) and E2 (plus `offset asm("r0")`): no change. Combine still merges the shift into the hard r10 set.
    - N1 (session 12's natural loop) does produce the copy (`mov r7,sl; adds r7,r7,r1`).
    - X2 (offset60 r10 plus mode1High as a long-lived pseudo the loop reads) gives `mov r3,sl; adds r3,r3,r1` **exactly**, but the pseudo spills to its own slot (sp+28, frame 0x20). The target pattern needs the add's destination to be a spilled pseudo (a combined in/out reload gets a fresh register).
    - A struct field can't be that spill. Store equivalence (`REG_EQUIV`, local-alloc.c) only applies to block-local pseudos, and local-alloc always finds them a register. So the `locals` struct form has a ceiling here.
    - L3b/L3c (the struct split into 7 plain locals): slots sp+0..20 and the r10 copy come out right, but **childWork1 beats xDelta for r7**. Global priority is log2(refs)·refs/live_length: about 5/92 insns for childWork1 against 8/338 for xDelta. In a goto-loop every reference counts once, because `REG_N_REFS += loop_depth` needs real loop notes, so the original natural loop weighted these differently.
    - Reconfirmed the session-5 warning: never pin `r7`. It is `FRAME_POINTER_REGNUM` in Thumb RTL, and L3c put both childWork1 and xDelta in r7.
    - X-address residual: combine folds the pinned r0 `xAddress` into a new pseudo (frameOffset + r1), and local-alloc suggests the dying hard r1 for it.
  - **Next:** the natural-loop rewrite (N1/N3 family): remove the loop entry test and the +10 giv. Real loop notes then also fix the allocation priorities. Port the result to func_804A84C.
  - Nothing committed. Notes are on decomp.wtf (both functions released) and in `reports/swinging_platform_for_gpt6.md` (session 14 section).

- **2026-10-16, Claude Code (session 13): DrawHomerunDerby recorded as matched; snapshot commit.**
  - **Read-only audit.**
    - decomp.wtf `progress()` was 4008/4024 and `objcmp.py` showed 16 differing, the same 16 as decomp.wtf's open list.
    - `myfork/decomp-wtf` = `89a6f56` (pushed).
  - **Why decomp.wtf scored the byte-exact DrawHomerunDerby as 87.**
    - `expected/us/obj/minigames/homerun_derby.o` was captured by `tools/gen_objdiff.py` (commit 330b9b1, 2026-10-15) from the asm-era source.
    - That source emitted two Thumb instructions as `asm(".word 0x190000c0")` at DrawHomerunDerby+0x6e8, so the reference object marked those 4 bytes `$d` (data).
    - objdiff showed the target as one `.word` and ours as `lsls r0,r0,#3; adds r0,r0,r4`: "1 different, 1 extra" = 87. Same bytes.
    - The reference also held raw addresses instead of relocations at +0x85c (gOamBuffer = 0x03001444) and +0x914/+0xabc (gHomerunPitcherAnimation = 0x03004A10). These are equal after linking and did not affect the score.
    - The scan found no other `asm(".word")` in the snapshot source, so no other unit has this problem.
  - **Fix.**
    1. Checked **all 16 functions** of `homerun_derby.o` against the base ROM (`.text` at 0x0808DFD0). All non-relocation bytes are equal. 339 relocations are identical to the reference; the 3 extra ones resolve through the ROM to the reference's raw words.
    2. Backed up the old reference (`homerun_derby.o.bak-asm-era-2026-10-15`).
    3. Removed the leftover file-scope `asm(".set $t.hd_* ...")` mapping-symbol block from `homerun_derby.c`, rebuilt and rechecked: all 16 functions still match the ROM.
    4. Installed the ROM-verified build object as the new reference, as `gen_objdiff.py` does.
  - **Result.**
    - `compile_and_diff` with the unchanged function returned a match; decomp.wtf auto-committed `7fc2668` (the `.set` removal only).
    - `progress()`: **4009/4024, 97.1%**. `objcmp.py`: exactly 15 differ.
  - **DrawRoulette / WarioHopInit.** The working-tree text differs from the saved best, but both compile to byte-identical objects with the project's exact commands, so the scores are equal (6984 / 1004). The cleaner working-tree versions are kept.
  - **Housekeeping.**
    - Deleted the junk file `tatus --short` (a captured `git diff --stat`) and `reports/WARIOLAND4_CLAUDE_HANDOVER - Copy.md` (identical duplicate).
    - Committed all working-tree bests, reports and handover files (excluding `tools/agbcc`). Not pushed.
- **2026-10-16, Claude Code (session 12):**
  - **DrawHomerunDerby (13 compiles: 12 local, 1 decomp.wtf; not matched; decomp.wtf best 663 -> 174, 99.7%).** Saved on decomp.wtf only; the repo source is unchanged.
    - Fixes: camera affine pointers as `&affineMatrix[1]`/`&affineMatrix[2]`/`affinePb + 2`; the bat loop counts with `firstOamIndex` itself (frees r5 for `activeAnimation`, 216 -> 77 local diff lines); index-first bonus frame lookup.
    - Left: in the ball-hit block, the `sl` reload goes to r0 where target has r2, shifting every later temp. Most likely target's `mov r2, r8` is not a reload. The static-OAM preheader has the same r0-vs-r2 pattern.
    - Raw-address pool words in `expected/` (gOamBuffer, gHomerunPitcherAnimation) are equivalent to our relocations.
    - Wrote `reports/drawhomerunderby_for_gpt6.md` (it includes the best C); released with notes.
  - **func_804A508 (11 local compiles; not matched; saved best 425 untouched; nothing submitted).**
    - The target's stack slots are loop.c products: invariant table addresses hoisted then spilled, and sp+24 is the strength-reduced `&sprite->work1` giv.
    - Plain spilled locals (L3) give the exact loop-body reload rotation, but global alloc then keeps childWork1 in r7.
    - The natural-loop rewrite (N1) reproduces the prologue hoisting and layout, but adds a loop entry test and an extra `sprite+10` giv.
    - Wrote `reports/func_804A508_for_gpt6.md`; released with notes. func_804A84C not attempted (it was conditional on 804A508 matching).
  - **UpdateEscapeRippleHBlank parked (2 bytes off at 0x9c/0x9e).** Recorded Codex's session 4 (`reports/escaperipple_codex_results.md`; 6 candidates, none better, plus a read-only allocator audit). Verified its correction in `tools/agbcc/gcc/thumb.h`: `STACK_REG` = sp only (`0x02000`), `HI_REGS` = `0x0ff00`, `LO_REGS` = `0x000ff`. Fixed the `STACK_REG` wording in the Catbat and EscapeRipple reports and in this log.
- **2026-10-16, Claude Code (session 11):**
  - **Matched func_800A43C** (15 compiles: 13 local, 2 decomp.wtf).
    - Path: rewrote block 1 only (K1); unpinned `xPositions`/`yPositions` (K2); put an `r0` pin on the re-read destination to stop CSE forwarding (K4); moved x/y into the call arguments (K5, 88 lines); gave all 8 blocks identical C (K9, decomp.wtf score 6); removed the `pathOamAddressLow` pin and the `timerPointerLow` temp (K12, **MATCH**).
    - Dead ends: blocks 2–8 with a `constant` variable (K6), with a shared-step tail (K7) or with a pinned `constantValue` (K8); assignment inside the call argument (K11, normalised away).
    - **Lesson: post-reload `move2add` and reload inheritance make identical C look different per block** (`ldr =1084` vs `adds r4,#63`; one vs two `movs #2`). Before writing per-block variants, try making repeated blocks identical.
  - `objcmp.py`: exactly 19 differ. decomp.wtf commit b0321e0 also swept in the previously uncommitted working-tree attempt for `UpdateLayeredSubGameCutscene` (same file; still unmatched).
- **2026-10-16, Claude Code (session 10):**
  - **Pin-scan follow-up.** Classified each baseline's diff hunks (local harness; literal-pool and branch-offset lines excluded) by whether they involve the pinned high-register pointers:
    - **func_800A43C:** 103 real changed lines in 26 hunks; only 2 hunks touch `r8`/`r9`. Even those differ only in the low temporary (`movs r5,#2; add r9,r5` vs `r1`; `mov r2,r9; ldr [r2]` vs `r4`). The other 24 are ldrsh zero-index registers and one `ldr r3,=1084` vs `adds r4,#63`.
    - **DrawHomerunDerby:** 97 real lines in 23 hunks; the only `r9` hunk is epilogue literal-alignment noise. The rest are ldrsh zero-index registers, the affine stack-pointer setup (`sp+4`/`sp+6`) and table temporaries.
    - **Verdict:** neither is a Catbat-style pin problem. Both are the reload-rotation family, like func_804A508, so Cractus/BlackDragon-style source-shape attempts are the right tool, not unpinning.
- **2026-10-16, Claude Code (session 9):**
  - Committed e216e88 (EscapeRipple sessions 2–3, its GPT-6 plan and the pin scan). EscapeRipple Session 3 (GPT-6 pointer-arithmetic plan, 3 compiles): no candidate changed `LO_REGS` to `STACK_REG`. (Correction, session 12: in agbcc's `thumb.h`, `STACK_REG` is sp only (`0x02000`); the high-register class is `HI_REGS` (`0x0ff00`).) A Thumb `add` can only target a high register in the tied two-operand form, and the target's set is a bare `mov ip, r3`.
  - **func_804A508, Catbat lesson (2 local compiles; not matched; best 425 untouched).** Unpinning `current` (F1, 343 lines) or reading through `gCurrentSprite` (F2, 213 lines) both regress against the baseline's 114. The `r9` pin already matches the target. The real diffs are reload rotation in the loop body (`mov r3, r10`, ldrsh zero-index registers, table temporaries). func_804A84C was not tried (that was conditional on a match).
- **2026-10-16, Claude Code (session 8):**
  - **UpdateEscapeRippleHBlank, Catbat lesson (3 local trials; not matched; best 5 untouched).**
    - Reading the phase byte through the symbol makes it an add-operand reload, but the pointer pseudo is `pref LO_REGS`. It's set by a copy or ldr; Catbat's was set by an `add`, which can target a high register. So the pointer lands in a low register instead of `ip`.
    - Added a Session 2 section to `reports/escaperipple_for_gpt6.md`.
  - **High-register pin scan** of the remaining near-misses: our `asm("r8".."r12")` pins compared with the target's `mov rLow, rHigh; ld/st [rLow]` reload signatures.
    - Candidates: func_804A508 / func_804A84C (`current`@r9, 9 target reloads each), func_800A43C (`xPositions`@r8 and `yPositions`@r9, 16 and 20 reloads), DrawHomerunDerby (`cameraTransform`@r9, 12 reloads), WarioHopInit (`speedBase`@r8 and `scrollA`@r10, 1 reload each).
    - Not candidates: the high pins in BossDoor, WarioHopDrawSprites, DrawSoundRoomSprites and DrawRoulette are integers or masks, not dereferenced pointers.
- **2026-10-16, Claude Code (session 7):**
  - **Matched UpdateCatbatPatrol**, following GPT-6's plan (`reports/catbat_gpt6_plan.md`, Experiment A): 4 local trials and 1 decomp.wtf compile.
    - Fix on top of P4: `wario` unpinned, and x read as `gWarioData.xPosition` into `right`, with `left = right`.
    - Mechanism: the pinned `r8` pointer forced an expand-time copy (no reload event). Unpinned, `wario` has lower priority and lands in `r8`, and the x load becomes an output reload into `r1` that advances the rotation.
  - `objcmp.py`: exactly 20 differ. Session 3 written in `reports/catbat_for_gpt6.md`.
  - Note: `reports/catbat_for_gpt6.md` had been overwritten with the plan's text. It was restored from git (the plan is kept in its own file) before appending.
- **2026-10-16, Claude Code (session 6):**
  - Committed the reports and the progress log (3197079). Catbat left alone (GPT-6 is reviewing it).
  - **SetupShopkeeperSelectedItem (13 local trials; not matched; saved best 124 untouched).**
    - Key finding: the target's `movs r1,#128; lsls r1,#3; adds r0,r1,#0` is a 16-bit (HImode) move of the constant `0x400`. That's how matched `BossTreasureInit` (`status |= 0x400`, plain C) and `InitCatbatMineSpawner` get it.
    - A 16-bit OR reproduces the copy (b1), but then loses the `orrs r2, r3` and adds a re-truncation.
    - No form found that gives the 16-bit constant copy and keeps `status` as a 32-bit value.
    - Wrote `reports/shopkeeper_for_gpt6.md`; released with notes.
- **2026-10-16, Claude Code (session 5):**
  - **UpdateCatbatPatrol (9 local trials; not matched; saved best 17 untouched).**
    - Per-branch asymmetry solved the global allocation. Variant P4 (plain `right`/`left` locals, plain `zero`, and only the left half's status update rewritten with `r1`/`r0` pins and `u16 status`) gives the target's exact allocation: pointer `r12`, `zero` `r10`, both x copies spilled to `sp+12`/`sp+8`.
    - What's left are 3 reload-rotation choices (`r1`/`r7`/`r0`).
    - Added a Session 2 section to `reports/catbat_for_gpt6.md`.
  - **UpdateEscapeRippleHBlank (12 local trials; not matched; saved best 5 untouched).**
    - Found the mechanism: local allocation never uses `r7` (the Thumb frame pointer), so the target's `mov r7, ip; ldrb r7, [r7]` must be reloads.
    - Our pinned `r12` pointer makes the expander copy it into a block-local pseudo (`r6`).
    - Unpinning the pointer makes the address a reload, but the pointer then goes to a low register.
    - **Warning:** pinning a variable to `r7` can miscompile (V7c).
    - Wrote `reports/escaperipple_for_gpt6.md`.
  - Nothing was submitted to decomp.wtf for either function; both are released with notes.
- **2026-10-16, Claude Code (session 4):**
  - **Matched UpdateBlackDragonAttack** in 1 decomp.wtf compile, after 6 local trial compiles.
    - Baseline `-dg`: spill regs `{r0, r1, r6}`, matching the target's temporaries.
    - D1 (duplicated bob code, shared locals): case-2 and case-5 address temps became correct (`r0`/`r6`), but default broke. 86 changed lines vs 78 for the saved best.
    - D2/D3 (default with its own sprite variable): no change.
    - **D4 (block-local `index`/`counter` in each case, no goto, no sprite variable): MATCH.**
  - Lesson: when the target gives the "same" variable different registers in different branches, the original had separate (block-local, macro or inline) variables. One shared C variable is one pseudo with one hard register.
  - `objcmp.py` after the match: exactly 21 functions differ. decomp.wtf auto-committed as 8be1759.
    - **Note:** that commit also swept in the previously uncommitted working-tree best for `SetupShopkeeperSelectedItem` (Codex's score-124 attempt, same file). It's still unmatched, but its saved best is now in git.
- **2026-10-16, Claude Code (session 3):**
  - Committed `tools/objcmp.py` (ff00c65, force-added because `tools/` is gitignored). Usage is in its docstring.
  - **UpdateCatbatPatrol: not matched; saved best (17) unchanged.** Root cause found with `-dg`: the pinned `zero asm("r10")` makes `r10` count as "already used", so global allocation puts the work3 pointer there instead of `r12`. Unpinning it fixes the pointer, but a single-constant `zero` gets `REG_EQUIV` and loses the priority race to the wario-x pseudos. In the target, the `sp+8`/`sp+0xc` stores are spilled pseudos, not a volatile struct.
  - A 128-variant local sweep found nothing better than 114 changed lines. Full analysis is in the decomp.wtf release note. Testing was local only (scratchpad harness `try.py`, which compiles a candidate and diffs one function against `expected/`); nothing was submitted.
- **2026-10-16, Claude Code (session 2):**
  - Build check: links OK; MD5 mismatch expected (see above); per-function comparison showed exactly the 24 unmatched differing.
  - **Matched UpdateCractusArmSegmentPose24** (4 compiles). Pinned shared r7: score 557. Unpinned shared var: 11. Duplicated x-step per branch: **0**.
  - **Matched CheckWarioHorizontalCollision** (4 compiles). Fully unpinned block 2: 205. `sideCopy = index`: 11. High-register load for `locals.offset`: 5. Plain `doubled`/`row`: **0**.
  - decomp.wtf auto-committed both (91ba73b, 4e06c44); each commit touches only its own function. Notes added on decomp.wtf.
- **2026-10-16, Claude Code:** read-only review per HANDOVER.md §8. Verified the counts and read the notes on all 24 functions. Compared the local files with the saved bests and found three mismatches (func_800A43C, DrawRoulette, WarioHopInit), all score-neutral. No function source edited, no compiles run, nothing committed.

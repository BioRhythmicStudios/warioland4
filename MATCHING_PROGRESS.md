# Matching progress: Wario Land 4 (decomp.wtf project `warioland4-2`)

Shared log for Claude Code, GPT-6 and Codex. Update it after every session, and record **where** each improvement lives:

- **Git**: committed on branch `decomp-wtf`.
- **Working tree**: present in the local `src/` files but not committed.
- **decomp.wtf saved best**: the "best attempt" stored in the decomp.wtf database. It can differ from the local file.

## Status (updated 2026-10-17, session 27, by Claude)

- **2026-10-17 Claude session 27, WarioHopDrawSprites: MATCHED.**
  - Went from 709 / 98.5% to exact in 10 local compiles (9 trials plus 1 re-check of the checkpointed copy) and 1 decomp.wtf submission. It was auto-committed as `04a2c15` (single file, 0 CR bytes in the blob).
  - Independent check: full objdiff build + `tools/objcmp.py` = **11 non-identical** (was 12). decomp.wtf **4,013 / 4,024, 98.4% of code**. Not pushed.
  - Approach: clean sibling-style rewrite (no pins, no asm), in the `DrawSoundRoomSprites` / `RenderPauseScreenOam` loop form with a 3-bit `affineIndex` union, rather than editing Codex's pinned N1 (190 changed lines). H1 was at 192.
  - What mattered:
    - A `u32 active` camera-mode local and chained `matrixNPtr` locals (106).
    - Using `gWarioHopPlayerShadow` directly instead of a `shadow` pointer local, which fixed the shadow loop's hoist order (88).
    - `gWarioHopObstacles[({ j; })].animFrame` in the obstacle `src =` line (exact).
  - Why the statement expression works: it adds one luid (block notes count), so loop pass 1 hoists the j loop's symbol load (life 4 → 5 meets `threshold*savings*lifetime >= insn_count`). In pass 2, `combine_movables` then can't merge it with the inner loop's hoisted load, and cse2 stops folding the inner draw pointer into a copy. That restores the target's `adds r0, r2, #0; adds r6, r1, r0`.
  - Candidates H1–H8, the runner and the analysis are in `reports/hop_draw_sibling_candidates/README.md`.
  - Sibling check: `UpdateBossDoorOpeningSprites` has the same OAM-loop siblings plus an `s16 affine[4]` block like `RenderStageEntrySprites`. Not claimed this session.

- **2026-10-17 Claude session 26, RenderPauseScreenOam: MATCHED.** 28807 / 45.4% to exact in 7 local trials + 1 decomp.wtf submission (plus 1 diagnostic `-dg` compile). Commits `9337476` (declare `sUnk_86D36DC` as `[][4]`; a `[state - 1][piece]` table, only used here) and `239c3c4` (match), both auto-committed by decomp.wtf, single file each, verified 0 CR bytes in the committed blobs. Independent check: full objdiff build + `tools/objcmp.py` = **12 non-identical** (was 13); decomp.wtf **4,012 / 4,024, 98.0% of code**. Not pushed. Full target from `objdump -d -r --disassemble=RenderPauseScreenOam expected/us/obj/pause_screen.o` (1,016 normalized lines). The tail that was missing has: a second per-jewel block (`!gPauseAllJewelPiecesCollected && state` draws `sUnk_86D36DC[state-1][i]`), an all-jewels sprite (no >128 check), keyzer (`sUnk_86D36D4` x table, y+96), CD (gated on `gUnk_3003C4A == 0`, x+200) and the high-score medal (>599/>999/>799). Jewel animation table is `sUnk_86D36FC[i]`, not `sUnk_86D36DC`. What mattered, in order: (1) the DrawSoundRoomSprites `for (; currentSlot < nextSlot; currentSlot++)` loop copied literally into all 10 sections (91.8% opcodes); (2) not reusing one `animation` local across section 1 and the item sections (99.2%); (3) high-score arms assign `src = X->oam` directly so they cross-jump instead of if-converting, keyzer/CD state locals as `u32` (no copy into r4); (4) **section 1 uses `gPauseScreenAnimation` directly instead of a local**, which loads the frame index before the pointer and gives the target's spill set with r5 (96 changed lines); (5) one shared `animation` local for language/jewel/jewel-piece/keyzer/CD (target keeps all of them in r3) = exact. No register pins, no asm. Candidates A–F in the session scratchpad.

- **2026-10-17 Codex session 25, WarioHopInit:** 15 isolated local compiler invocations including one rejected agbcc ICE; no exact match. Saved1004 / 95.5% and original source unchanged. P1 unpins playerBase, adds r6 to spill set and fixes first DMA r9 input reload (r6 vs baseline r5). Closest full local diff P2 combines that with rolling coordinates read from player fields: 334 -> 132 changed lines, not a server score; many setup/copy/register differences remain. P1 also preserved as simpler independent improvement. Full candidates, RTL, traces, compiler-error evidence and preservation checks: `reports/hop_init_candidates/README.md`. All earlier candidates and unrelated source/compiler files preserved. No installation/submission/staging/commit/push. Claim released; baseline4,011/4,024 unchanged.

- **2026-10-17 Codex session 24, final UpdateEscapeRippleHBlank session:** no exact match; saved score5 / 99.8% unchanged, project4,011/4,024. Nine compiler invocations total: baseline, four byte-load expansion variants, four allocation diagnostic controls (including archived C4). No new r7 pins, no lifetime repeats. Actual local trace proves address pseudo77 has LO_REGS preferred, ALL_REGS alternate, only r6 available (mask0xffffffbf); suggested-ip search fails, preferred search succeeds in r6, so global allocation never reassigns it. Existing byte result is hardr7. Archived ordinary C4 pointer48 has only r7 available globally, chooses r7 without trying ALL_REGS/ip or retrying. Scalar/struct/bitfield forms still allocate address locally; builtin byte copy adds stack storage. Original baseline remains best (two operands at0x9c/0x9e). Full sources, traces/masks, RTL and preservation check: `reports/escaperipple_final24/README.md`. Both worktrees, source/compiler, prior M1/N1/S3 artifacts preserved. No install/submission/staging/commit/push. Claim released; stop for today.

- **2026-10-17 Codex session 23, UpdateBossDoorOpeningSprites:** claimed while unclaimed; 15 isolated local compiler runs, no exact match. Best local S3 moves three offset declarations after affine pointers, fixing all six spilled-pointer/offset slots while preserving all 104 reload allocation choices/cursors; full changed lines 506 -> 480 (not a server score). Earliest meaningful mismatch remains limit stores r3/r0 versus target r1/r2, followed by reused icon zero r3 and loop-setup choices. Active-state pin tests broaden spill set and regress; rejected. Full sources, disassemblies, traces and RTL: `reports/boss_door_candidates/README.md`. Original source and saved score1235 / 97.9% unchanged; no install/submission/commit/push. SHA256 verified 2,429 protected source/prior-artifact/compiler files unchanged, including Roulette M1 and WarioHop N1. Baseline still 4,011/4,024. Claim released; recommend WarioHopInit next, focusing on DMA definition/hoisting structure.

- **2026-10-17 Codex session 22, WarioHopDrawSprites:** claimed while unclaimed; 15 isolated local compiler runs. Best local N1 reduces full normalized disassembly changed lines 380 -> 190, restores the obstacle type reload and eliminates three affine-base reloads; not exact, no new server score. All sources, full diffs, emitted assembly, reload traces and report: `reports/hop_draw_candidates/README.md`. Saved best and original source remain score 709 / 98.5%; no installation/submission/commit/push. Main remaining issues: obstacle draw-pointer copy folds away, loop-setup registers and later affine registers. Baseline 79 reload events with spill set [0,1,2], N1 76 with [0,1,2,3]: allocation changes globally. Roulette M1/score1690, Claude and parked twins preserved. Claim released. Recommended next independent target: UpdateBossDoorOpeningSprites (limit initialization and stack layout).

- **2026-10-17 Codex session 21, RefreshRouletteResultTiles:** claimed before testing; 15 isolated local compiler runs. Closest local candidate M1 reduces full normalized disassembly changed lines 203 -> 62, but is not exact and has no new server score. Complete source, diffs and traces in `reports/roulette_result_candidates/`; analysis in its README. Committed source and saved best remain score 1690 / 80.3%. Main remaining issues: quotient-copy source operand, mask initialization order, loop.c moving second-loop increments, and third-loop temporaries. No source installation/submission/commit/push; project remains 4,011 matched. Claim released with notes. Claude's function and parked twins untouched.

- **4,011 / 4,024 functions matched, 97.7% of code**, confirmed by decomp.wtf after UpdateLayeredSubGameCutscene matched. Independent objcmp: 4,024 compared, exactly 13 non-identical, no missing units. The matched function has an empty full disassembly diff.
- **UpdateLayeredSubGameCutscene matched in `c23c5df`.** Case 3 now uses a signed s16 value pinned to r1 and a scoped vertical-offset pointer pinned to r0, removing the `(u16)-2` literal and duplicate copy without changing any of the 14 reload events. Seven isolated compiler trials (baseline plus A-F); E matched. An independent isolated objcmp passed before installation, followed by a full project build/objcmp before commit and server submission.
- **Approved cleanup: `637aed9` reverts only `9a6ff05`.** The server submission auto-committed unwanted line endings throughout cutscenes.c, including extra CR characters within the function. The user approved reverting that formatting-only commit after read-only verification. No conflict occurred. Afterward HEAD and the working file both equal `c23c5df:src/cutscenes.c` exactly (blob `5bbdc9c3320fce83a6f3814de7e4238acc75a00d`). Rebuilt cutscenes and reran `tools/objcmp.py cutscenes UpdateLayeredSubGameCutscene`: function still identical, 13 remaining project differences. decomp.wtf still reports 100% and 4,011 matched; nobody holds a claim. Unrelated uncommitted work and the parked twins were preserved. No reset, clean, amend or push was performed.

- **2026-10-17 Codex session 19:** claimed func_804A508, inspected preference/inheritance/death-note paths, and captured full P3 reload relationships. Two diagnostic compiler invocations (one tracer decode failure, one successful); no speculative source trials. Work1 is a single in/out reload ip->spilled pseudo, with no notes or valid inherited r0 value; the dying-output reuse rule is skipped. Both signed-load scratch reloads have null input and earlyclobber outputs, so inheritance and dying-address reuse do not apply. REG_UNUSED narrows their type but does not introduce an r2 preference with empty sharing masks. No supported candidate met the requested prediction gate. P3 remains seven operand differences / 50 unchanged events; C11, saved bests and twin untouched. Full evidence: `reports/swinging_platform_candidates/session19/README.md`. No install/submission/commit; claim released at session end.

- **2026-10-17 Codex session 18:** claimed func_804A508 again and ran 12 local compiler invocations starting from isolated P3. Confirmed all three remaining sites are real reload allocations. Availability candidates: event 5 {r0,r1,r2,r4}, event 40 {r2,r3}, event 43 {r2,r3,r4}. Existing-value lifetime changes force the desired r0/r2/r2 independently, but move target instructions, disrupt hoisting, or perturb subsequent allocations; none improves P3. Exact candidates, full operand diffs and all event traces are preserved in `reports/swinging_platform_candidates/session18/`. C11 source, twin and saved score-25 bests remain unchanged; no submission or commit. P3 still has seven operand differences. Claim released with findings at session end.

- **2026-10-17 Codex update:** func_804A508 was claimed before compilation and tested locally in 20 compiler runs. A new local candidate fixes the mode load/store (7 remaining operand differences, versus C11's 9); all 50 reload allocation events are unchanged. Candidate: `reports/swinging_platform_candidates/P3_u8_mode_r0.c`. Both twins' committed C11 source and decomp.wtf score-25 saved bests remain unchanged; no match, submission, or commit. See session 17 in `reports/swinging_platform_for_gpt6.md`. func_804A84C and Claude's RenderPauseScreenOam were not edited.

- **2026-10-16 baseline: 4,010 / 4,024 functions matched, 97.5% of code** (decomp.wtf `progress()`). It was 4,000 at the start of that day. Matched that day: UpdateCractusArmSegmentPose24, CheckWarioHorizontalCollision, UpdateBlackDragonAttack, UpdateCatbatPatrol, func_800A43C and DrawSoundRoomSprites (Claude); UpdateTogenobiOuterPartPosition, SetupShopkeeperSelectedItem and FlatWarioFloating (Codex); DrawHomerunDerby (Codex's code, recorded in session 13 after a reference-object fix; see the session log).
- **13 unmatched:** 10 have been scored and 3 are blocked by SWI (never scored).
- `project_info()` still reports a stale 3,994 / 95.28%. Use `progress()` and `list_functions()`, which agree with `tools/objcmp.py`.

## Build check (how to confirm nothing regressed)

- `make check` (via WSL) **builds and links**, but the ROM MD5 **does not match**, and won't until all unmatched functions match. The unmatched C is compiled straight in (there's no NONMATCHING/asm fallback), and `AgbMain` near the start of ROM has a different size, so everything after it shifts. The last 100% report (`report.json`, 2026-10-15 13:40) predates the asm-free rewrites.
- The real regression check is per function. Build every objdiff unit (`make -k build/us/objdiff/...`), then compare each function's disassembly with `expected/us/obj/*.o`, normalising branch targets, jump tables and `.has_asm` symbol suffixes. The script is `tools/objcmp.py` (committed; usage in its docstring).
  - 2026-10-16 before work: exactly the 24 unmatched differed and all 4,000 matched were identical.
  - 2026-10-16 after two matches: exactly 22 differ.
  - 2026-10-16 session 13: exactly 15 differ, the same 15 that decomp.wtf lists as open.
  - 2026-10-16 session 17: exactly 14 differ (DrawSoundRoomSprites matched; every other function unchanged).
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
| ~~UpdateLayeredSubGameCutscene~~ | 0 | 100 | **MATCHED 2026-10-17, Codex, `c23c5df`**. Signed s16 r1 temporary plus scoped vertical pointer r0; full project objcmp verified. Formatting-only server commit `9a6ff05` reverted by approved `637aed9`; match preserved. |
| ~~SetupShopkeeperSelectedItem~~ | 0 | 100 | **MATCHED 2026-10-16 (Codex), commit b501a4f** |
| ~~UpdateTogenobiOuterPartPosition~~ | 0 | 100 | **MATCHED 2026-10-16 (Codex), commit 19ffaff**. `sSinCosTable` used directly on the second lookup |
| ~~UpdateBlackDragonAttack~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit 8be1759** |
| ~~func_800A43C~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit b0321e0** |
| ~~DrawHomerunDerby~~ | 0 | 100 | **MATCHED**: code by Codex (commit 89a6f56); recorded by decomp.wtf in session 13 after the `expected/` fix (commit 7fc2668) |
| func_804A508 | 25 | 99.8 | Session 15: natural index loop plus `do { } while (0)` X-load wrappers (165 → 25). Left: 9 operand diffs, all reload register choices (two prologue output reloads r4 vs r0, two post-loop ldrsh zero indices). Before: Codex improved it from 425. Missing `mov r3, r10` at the first `mode1HighY` stack store; X-address and ldrsh scratch registers. Session 14: the `locals` struct form can't produce the `mov r3, r10` (needs a spilled pseudo); next step is the natural loop. See `reports/swinging_platform_for_gpt6.md` |
| func_804A84C | 25 | 99.8 | Session 15: ported func_804A508's natural-loop source (AC→AD, six child tables), 165 → 25. Same 9-operand reload residual as the twin. Before: session 14 ported Codex's changes (370 → 165) |
| ~~WarioHopDrawSprites~~ | 0 | 100 | **MATCHED 2026-10-17 (Claude, session 27), commit 04a2c15**. Sibling-style rewrite plus a `({ j; })` luid nudge for the obstacle pointer; see session 27 |
| WarioHopInit | 1004 | 95.5 | Register allocation of hoisted DMA sources and struct copies |
| UpdateBossDoorOpeningSprites | 1235 | 97.9 | Limit init registers; stack slot offsets |
| RefreshRouletteResultTiles | 1690 | 80.3 | Saved source unchanged; session-21 local M1 has 62 vs baseline 203 full changed lines, unsubmitted. Quotient copy, loop motion and third-loop allocation remain. Live baseline saves only r8; old extra-r9 note was stale. See `reports/roulette_result_candidates/README.md`. |
| ResolveWarioCeilingCollision | 2236 | 88.7 | Codex improved it from 4044. Stack-slot order of right/offset/savedX; pointValue spill width. See `reports/ceiling_collision_for_gpt6.md` |
| DrawRoulette | 6984 | 87.7 | Large; register allocation |
| ~~RenderPauseScreenOam~~ | 0 | 100 | **MATCHED 2026-10-17 (Claude, session 26), commits 9337476 + 239c3c4**. Clean rewrite in DrawSoundRoomSprites style; see session 26 |
| ~~DrawSoundRoomSprites~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude, session 17), commit 8c3a47a**. Rewritten from scratch in RenderStageEntrySprites/RenderVortexCutsceneOam style |
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
- The incomplete function RenderPauseScreenOam needs structural work, not register tuning. (DrawSoundRoomSprites, the other one, matched in session 17 by copying the matched OAM-drawing siblings; try the same for RenderPauseScreenOam.)

## Session log

- **2026-10-16, Claude Code (session 17): DrawSoundRoomSprites MATCHED (42.9% / score 29282 → 0), commit 8c3a47a.**
  - Used 6 local trials (`try.py`, candidates A-F) plus 1 decomp.wtf compile (the match). decomp.wtf truncates the target at ~580 lines, so the full 1,026-line target came from `arm-none-eabi-objdump` of `expected/us/obj/sound_room.o`.
  - **Approach:** threw away the macro-based attempt and wrote every OAM draw loop exactly like the matched siblings `RenderStageEntrySprites` (stage_entry.c) and `RenderVortexCutsceneOam` (vortex_cutscene.c): `for (; currentSlot < nextSlot; currentSlot++) { attr = *(src++); *(dest++) = attr; gOamBuffer[currentSlot].split.y = ...; ... .split.x = (attr + X) & 0x1FF; *(dest++) = *(src++); .split.priority = 0; dest++; }`. loop.c then produces the target's reversed counters, `mov r7, r8` final values and hoisted masks on its own. No register pins, no asm.
  - **What each fix did:**
    - First draft (A): 449 changed lines.
    - Explicit `& 0x1FF` on the x stores (C): the 0x1ff and 0xfffffe00 masks are hoisted into sl/ip, as in the target. A separate `u32 objectX` (B) shifted the loop registers instead.
    - The grid and music tables are 2-D `[passage][stage]`: the target computes `stage*4 + passage*16`.
    - The selected icon clears only the 3-bit affine index (`and #~0xE`), so it uses a local union with `affineIndex : 3` (D, copied from vortex). That also restored the strength-reduced oam pointer in that loop. D: 122 changed lines.
    - The music frame is `if (x % 2 == 1 && x <= 99) A; else B;`, giving the target's `cmp #1` and load order. State 4 uses `s32 cycle = gStageEntrySequenceTimer % 600` (shortened u16 modulo then a signed compare), and the default branch uses an `animation = sUnk_8642820` local (E: 104).
    - The last affine value is a `u16 matrixD` used for both `*matrix3Ptr` and `gOamBuffer[3]`, giving the target's zero-extension, as in vortex's `warioPd` (F: match).
  - **File-level changes:** in sound_room.c, retyped the externs only this function uses: `sUnk_8642898`/`8642820` as `struct AnimationFrame[]`, `sUnk_863C854` as an `AnimationFrame` pointer array, `sUnk_863C7D4`/`C814` as `const u16 *const [][4]`, the five raw OAM frames as `const u16[]`. `sUnk_863C694`/`6D4` became `const u32 [][4]`; their other users cast them to `u8 *`, so they are unaffected. The data file defines all of them as raw bytes.
  - **Verification:** full objdiff build + `tools/objcmp.py`: 4,024 compared, exactly 14 differ (the 11 scored open functions + 3 SWI), all matched functions identical. `progress()`: **4010/4024, 97.5%**.
  - **Lesson for RenderPauseScreenOam** (the other "incomplete" one): look for a matched sibling with the same OAM idiom and copy its loop shape literally before tuning anything.

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

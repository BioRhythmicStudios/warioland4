# Matching progress: Wario Land 4 (decomp.wtf project `warioland4-2`)

Shared log for Claude Code, GPT-6 and Codex. Update it after every session, and record **where** each improvement lives:

- **Git**: committed on branch `decomp-wtf`.
- **Working tree**: present in the local `src/` files but not committed.
- **decomp.wtf saved best**: the "best attempt" stored in the decomp.wtf database. It can differ from the local file.

## Status (updated 2026-10-16 by Claude via decomp.wtf MCP)

- **4,003 / 4,024 functions matched, 95.9% of code by size.** It was 4,000 at the start of 2026-10-16; UpdateCractusArmSegmentPose24, CheckWarioHorizontalCollision and UpdateBlackDragonAttack were matched that day.
- **21 unmatched:** 18 have been tried and 3 are new (never scored).

## Build check (how to confirm nothing regressed)

- `make check` (via WSL) **builds and links**, but the ROM MD5 **does not match**, and won't until all unmatched functions match. The unmatched C is compiled straight in (there's no NONMATCHING/asm fallback), and `AgbMain` near the start of ROM has a different size, so everything after it shifts. The last 100% report (`report.json`, 2026-10-15 13:40) predates the asm-free rewrites.
- The real regression check is per function. Build every objdiff unit (`make -k build/us/objdiff/...`), then compare each function's disassembly with `expected/us/obj/*.o`, normalising branch targets, jump tables and `.has_asm` symbol suffixes. The script is in Claude's scratchpad (`objcmp.py`).
  - 2026-10-16 before work: exactly the 24 unmatched differed and all 4,000 matched were identical.
  - 2026-10-16 after two matches: exactly 22 differ.
- Nobody currently has a function claimed. The latest matches are Codex's three: PinballCounterUpdateDisplay, InitHomerunDerby and ResolveWarioStandardCollision.
- Three functions need an inline SWI and can't match under the no-inline-asm rule: `AgbMain`, `WaitForVBlankInterrupt` (`swi 2`) and `MusicPlayerJumpTableCopy` (`swi 0x2A`). The realistic ceiling is therefore **4,021**, unless the rule is relaxed for SWI.

## Git and working-tree state

- Branch `decomp-wtf`, HEAD `51c24e5` (Match ResolveWarioStandardCollision). Backup branch `backup-before-codex` = `9862bed`.
- 13 source files have **uncommitted** changes that hold the current best attempts: boss_door_opening, cutscenes, draw_roulette, refresh_roulette_result_tiles, wario_hop, pause_screen, sound_room, catbat, cractus, shopkeeper, swinging_platform, togenobi and wario/normal.
  - Example: CheckWarioHorizontalCollision went from score 5 to score 3 (the `sideCopy` change) and exists **only in the working tree**.
  - Committing a snapshot is recommended. It has not been done; waiting for the user's OK.
- `HANDOVER.md` is untracked. `tools/agbcc` is a dirty submodule and was deliberately left untouched.
- decomp.wtf's "current source" for each function **is the local working-tree file**. It builds from `src/`.

## Local file vs decomp.wtf saved best

21 of the 24 functions have identical text in both places. Three differ. In all three the current build scores the same % as the saved best, so the difference looks codegen-neutral. That is **not** verified by compiling.

| Function | Difference |
|---|---|
| func_800A43C | The saved best (362) wraps each `value = *randomPointer;` in dead `register s32 zeroIndex asm("rN")` blocks; the local file has plain loads. Codex notes say the explicit zero "folds away". |
| DrawRoulette | The saved best uses `(s16 *)&stack` + 2 and `*(s16 *)((u8 *)sinPtr + zero)`; the local file uses `&stack.affineC` and `*sinPtr`. |
| WarioHopInit | The saved best has an unused `u32 i; i = 0;`; the local file removed it. |

## Unmatched functions (best score: lower is better, 0 = match)

| Function | Best | Match % | Remaining diff (summary) |
|---|---|---|---|
| ~~CheckWarioHorizontalCollision~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit 4e06c44** |
| ~~UpdateCractusArmSegmentPose24~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit 91ba73b** |
| UpdateEscapeRippleHBlank | 5 | 99.8 | 2 operands: target `mov r7, r12; ldrb r7, [r7]`, ours `mov r6, r12; ldrb r7, [r6]` |
| FlatWarioFloating | 8 | 99.8 | One branch pair inverted (`bne/b` vs `beq/b`). About 45 compiles across 3 agents, all at the same score |
| UpdateCatbatPatrol | 17 | 99.9 | 5 operands: work3 address temp `r1` / saved in `r10`; target uses `r7` / `r12` |
| UpdateLayeredSubGameCutscene | 85 | 99.7 | Case 3 BG0 stores. The 85 version uses a `(u16)-2` literal hack; Codex prefers its cleaner 133 version |
| SetupShopkeeperSelectedItem | 124 | 99.3 | `u16 flagCopy asm("r0")` adds `lsl/lsr #16`; with `u32` the score is 132 and only `mov r0, r1` is missing |
| UpdateTogenobiOuterPartPosition | 254 | 95.5 | `table2` lands in `r8` (extra push/pop); target keeps it in `r2` |
| ~~UpdateBlackDragonAttack~~ | 0 | 100 | **MATCHED 2026-10-16 (Claude), commit 8be1759** |
| func_800A43C | 362 | 99.5 | Zero-index `ldrsh` registers in 8 unrolled particle blocks, plus `1084` literal vs `add #63` |
| func_804A84C | 370 | 98.2 | Missing `mov r3, r10`; ldrsh zero-index and pointer registers |
| func_804A508 | 425 | 97.9 | Same shape as func_804A84C |
| DrawHomerunDerby | 663 | 99.0 | Affine pointer setup registers; ball-hit matrix table register |
| WarioHopDrawSprites | 709 | 98.5 | Register scheduling throughout |
| WarioHopInit | 1004 | 95.5 | Register allocation of hoisted DMA sources and struct copies |
| UpdateBossDoorOpeningSprites | 1235 | 97.9 | Limit init registers; stack slot offsets |
| RefreshRouletteResultTiles | 1690 | 80.3 | Loop form and register allocation (extra `r9` saved) |
| ResolveWarioCeilingCollision | 4044 | 79.5 | Register and local layout; shared tails |
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

1. **UpdateCatbatPatrol** (17): root cause known (see session 3 log and the decomp.wtf note), but the C form isn't found yet. Good candidate for a GPT-6 consult: we need a natural way for `zero` to avoid `REG_EQUIV` or to win the priority race.
2. ~~UpdateBlackDragonAttack~~: matched in session 4. Its lesson (separate variables per branch) may also apply to Catbat's duplicated facing-right/left branches: try branch-local `zero`/x variables there.
3. **UpdateEscapeRippleHBlank** (5): mechanism understood but not solved; see `reports/escaperipple_for_gpt6.md`. Ready for a GPT-6 consult.
4. **Catbat** is now three reload choices from a match, starting from P4; see the session 2 section of `reports/catbat_for_gpt6.md`. Also ready for a GPT-6 consult.

### Original recommendations (session 1, kept for history)

1. **CheckWarioHorizontalCollision** (score 3, 1 operand). The closest of all. Codex's 14 experiments are listed in its decomp.wtf note; don't repeat them. Untested direction: in the 2nd probe block, keep `index` (r8) itself unreferenced after `sideCopy = row`, so copy propagation has no `r0 == index` equivalence to use, for example by computing `doubled` from `sideCopy` *after* `mov r4, r8` without touching `row`. Codex's experiment 13 came close (ADD exact, but `mov r4, r0`).
2. **UpdateCractusArmSegmentPose24** (score 11, 4 operands in one 4-instruction block), then apply the same fix to **UpdateCatbatPatrol** (17) and **UpdateBlackDragonAttack** (286), which share the idiom. Untested idea: one variable carries the work-index address *and* is later reused as the `ldrsh` zero offset, so both reloads share a register as in the target. Don't repeat Astra's two-pointer `r7`/`r12` split (Catbat: 97.4%, score 444).
3. **UpdateEscapeRippleHBlank** (score 5, 2 operands). Treat it as a reload-register problem: the target reuses the load's own destination `r7` as the base. Both Codex (about 22 compiles) and Claude tried pinning variants; untested is leaving `phase_r7` as an ordinary variable while keeping `r6` live (for example `scroll_r6` used after the load), so `r6` is no longer the cheapest reload register.

Deprioritised:
- FlatWarioFloating: exhaustively tried; wait for a genuinely new idea from GPT-6.
- SetupShopkeeperSelectedItem (124): a reasonable fourth.
- The two incomplete functions (RenderPauseScreenOam, DrawSoundRoomSprites): need structural work, not register tuning.

## Session log

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

# Wario Land 4 Decompilation — Handover from GPT-6 and Codex

Welcome back, Claude!

While you were away, GPT-6 helped me install and configure Codex CLI, connect it to decomp.wtf, and continue work on the remaining unmatched Wario Land 4 functions.

We've made meaningful progress, and I'd like you and GPT-6/Codex to collaborate on finishing the decompilation rather than independently repeating each other's experiments.

## 1. Current project status

**Latest confirmed decomp.wtf statistics:**

- 4,000 / 4,024 functions exactly matched.
- 95.7% of code matched by size.
- 24 functions remain unmatched.
- 21 functions have been attempted and marked "tried".
- 3 functions remain in "to do".
- Some outstanding functions are BIOS/SWI-related exceptions that may not be matchable under the project's C-only restrictions.

Please verify the current figures through decomp.wtf before starting.

## 2. What happened while you were away

Codex CLI v0.162.1 was installed and connected to decomp.wtf.

We ran one automatic Codex agent, which worked through the available unmatched functions using the project's compile_and_diff workflow.

**Codex achieved three verified exact matches:**

1. PinballCounterUpdateDisplay
2. InitHomerunDerby
3. ResolveWarioStandardCollision

This increased the match count from 3,997 to 4,000.

Codex also substantially improved numerous functions, although it could not finish all of them.

The automatic agent eventually stopped because decomp.wtf had no eligible functions left to claim. Most remaining functions were marked "tried", so simply restarting the automatic agent was not useful.

We subsequently connected Codex CLI directly to decomp.wtf through its MCP integration, allowing targeted investigation of specific functions.

## 3. Promising near-matches

The following figures are from previous verified compilations and saved attempts:

| Function | Best reported match |
|---|---|
| CheckWarioHorizontalCollision | 99.9% |
| UpdateCatbatPatrol | 99.9% |
| UpdateEscapeRippleHBlank | 99.9% |
| FlatWarioFloating | 99.8% |
| UpdateCractusArmSegmentPose24 | 99.8% |
| UpdateLayeredSubGameCutscene | 99.7% |
| func_800A43C | 99.5% |
| SetupShopkeeperSelectedItem | 99.3% |
| DrawHomerunDerby | 99.0% |
| UpdateBlackDragonAttack | 96.4% |

Many of these have extensive saved notes from previous agents.

Please read those notes before attempting modifications.

**Important:** A 99.9% similarity score does not necessarily mean the remaining difference will be easy to solve. Register allocation and compiler scheduling have been particularly difficult.

## 4. Specific work already completed

### CheckWarioHorizontalCollision

This was investigated again using interactive Codex.

- Previous match: 99.9%, diff score 5.
- New verified result: 99.9%, diff score 3.
- Fourteen targeted candidates were tested.
- Two original differences were fixed.
- One remaining register operand mismatch:

Target:
`add r0, r2, r4`

Compiled:
`add r0, r2, r0`

The improved attempt was saved and verified through decomp.wtf. Detailed notes were recorded.

This is a promising candidate for further investigation.

### UpdateCatbatPatrol

Current verified best:

- Match: 99.9%.
- Diff score: 17.
- Five remaining register operand differences.

The main issue involves computing and preserving the address of `sprite->work3`.

Target assembly uses `r7` to construct and initially read the address, and `r12` to preserve it for the later store.

The compiled output instead uses `r1` and `r10`.

Codex tried approximately 11 initial candidates without improving the existing best.

We then consulted Astra Pro for an alternative hypothesis.

Astra suggested separating the initial load pointer and retained store pointer:

```c
register u8 *loadWork3 asm("r7");
register u8 *savedWork3 asm("r12");
```

The proposed code calculated the address through `loadWork3`, copied it to `savedWork3`, read the index, and then performed the final store through `savedWork3`.

**This experiment FAILED when actually compiled.**

Results:

- Match regressed to 97.4%.
- Diff score increased from 17 to 444.
- 79 operand differences.
- 2 additional instructions.
- 2 missing instructions.

Although the `r12` retention worked, the compiler introduced `mov r0, r7` before the initial load, shifted the sprite/table pointer into `r5`, and changed register allocation throughout the function.

The store-pointer move also happened too early.

Codex correctly restored and reverified the original 99.9%, score-17 implementation.

**Do not repeat Astra's exact two-pointer experiment.**

The alternative store-ordering experiment was not justified by these results and was not tested.

### FlatWarioFloating

Current best: 99.8%.

Only one conditional branch pair differs:

Target:
```asm
bne 0x288
b 0x284
```

Compiled:
```asm
beq 0x284
b 0x288
```

The surrounding assembly and literal pool otherwise match.

Claude and Codex have already tried numerous C control-flow arrangements, including different if/else structures, goto labels, loops, and return placement.

Codex made approximately 22 additional compilation attempts without improvement.

Read the complete saved notes before trying another approach.

### UpdateCractusArmSegmentPose24

Improved from 93.1% to 99.8%.

Four register operands reportedly remain different.

### UpdateEscapeRippleHBlank

Best reported match: 99.9%.

Potentially worthwhile for a focused second pass.

## 5. Tooling and project preservation

Local repository:

`C:\Users\Benny\Decomp\warioland4-2`

Branch:

`decomp-wtf`

Before Codex began, we committed Claude's source changes:

`9862bed` — Save Claude decompilation progress before Codex

We also created and updated a backup branch:

`backup-before-codex`

The `tools/agbcc` submodule had modified content and was deliberately left untouched.

**Do not reset, clean, or modify the compiler toolchain without permission.**

The decomp.wtf project database and saved function attempts may contain changes that are not represented in the Git repository.

Check the actual local source and decomp.wtf state before assuming they're synchronized.

The direct MCP connection is configured using:

`decomp mcp`

Codex CLI has successfully connected to decomp.wtf with 30 available MCP tools.

We had a temporary HTTP 401 authentication issue, which was resolved by accessing the application through its authenticated local URL and restoring the MCP connection.

If you encounter connection problems, please diagnose them rather than resetting project data.

## 6. How I'd like Claude and GPT-6/Codex to collaborate

I want us to use the strengths of both systems.

**Claude Code — Primary implementation and testing agent**

Your responsibilities:

- Inspect the repository and existing best C implementations.
- Read saved decomp.wtf attempt notes.
- Investigate compiler behaviour.
- Propose and implement targeted modifications.
- Compile and compare the actual assembly.
- Preserve improvements and exact matches.
- Identify and document remaining blockers.

**GPT-6 — Independent analysis and second opinion**

I'll bring particularly stubborn functions back to GPT-6 for another perspective.

GPT-6 can help analyse:

- ARM/Thumb instruction differences.
- Register allocation and variable lifetimes.
- Compiler control-flow and scheduling.
- Possible C source restructurings.
- Which hypotheses are worth spending compilation attempts on.

I'll share your assembly diffs and conclusions with GPT-6, then bring any promising ideas back for you to test.

**Codex CLI — Additional compilation-driven agent**

Once its usage allowance resets, Codex can take on individual functions that remain difficult.

It should not work simultaneously on the same function as Claude.

All speculative solutions must be tested by compilation before being counted as improvements.

## 7. Rules for our next round

1. Read the best existing attempt and previous notes before editing a function.
2. Preserve verified 100% matches.
3. Avoid repeating experiments already documented as unsuccessful.
4. Prioritise targeted, testable changes over broad rewrites.
5. Compile and compare assembly after every meaningful candidate.
6. Never call a function matched without verification.
7. Restore the best implementation if an experiment regresses.
8. Keep compiler settings and unrelated files unchanged.
9. Record exact mismatched instructions, register operands, and compilation results.
10. Save progress so another agent can continue without repeating work.

Please maintain a short progress document such as `MATCHING_PROGRESS.md`, distinguishing local Git changes from saved decomp.wtf attempts.

## 8. What I'd like you to do first

**Do not immediately start rewriting functions.**

First:

1. Inspect the current repository and decomp.wtf project state.
2. Confirm the 4,000 matched functions and 24 remaining.
3. Review the previous agent notes on the closest unmatched functions.
4. Check whether any functions have improvements saved in decomp.wtf that are not yet reflected in the local repository.
5. Recommend the three most promising functions to tackle next, based on actual assembly differences and previous attempts.

Then begin working through the functions systematically.

When you encounter a stubborn mismatch, prepare a concise report containing:

- Function name.
- Current best C implementation or relevant portion.
- Exact target-versus-compiled assembly differences.
- Compiler configuration where relevant.
- Changes already attempted.
- Your current hypothesis.

I'll bring that report to GPT-6 for independent analysis, and we can test new ideas together.

**Our goal is to finish as many of the final 24 functions as possible, without sacrificing the 4,000 already matched.**

Welcome back — Codex made a strong showing while you were away. Let's see what we can accomplish together.
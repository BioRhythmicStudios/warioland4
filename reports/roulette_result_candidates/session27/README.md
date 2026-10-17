# RefreshRouletteResultTiles, Claude session 27 (2026-10-17): parked

14 local compiles (one was a syntax error). No match. Codex's M1 (62 changed lines) is still the best local candidate.
Neither the saved best (score 1690) nor the source was changed.

Runner: `python3 reports/roulette_result_candidates/session27/run_trial.py minigames/roulette/refresh_roulette_result_tiles RefreshRouletteResultTiles <file.c> [N]` (under WSL, from the repo root).

| File | Change | Changed |
|---|---|---|
| R1 | Plain C with the `DmaSet`/`DmaWait` macros, `for (i = 3; i >= 0; i--)` | 212 |
| R2 | R1 with `for (i = 0; i < 4; i++)`. GCC reverses it into the target's countdown (`movs r5,#3 ... subs; bge`), and only r8 is saved | 204 |
| R3 | Plain C in the target's statement order: tiles, `next`, DMA base, control, dst, then `src += quotient`. The hoisted mask, stride and count come out in the target's order (mask, 0x400, 3), which M1 lacks | 200 (95.7% opcodes) |
| R4 | R3 plus pins next r8, idx r3, quotient r2, src r1, dst r6, control ip, DMA r4 | 186 |
| R5 | R4 without the idx/quotient pins | 196 |
| R6 | R5 plus pinned idx, `src = remainder << 8; src += tiles; src = quotient + src` (the src lines then match) | 184 |
| R7 | R6 with idx unpinned and `remainder = idx % 4` (division matches except idx r1 vs r3 and the missing `asrs r0; mov r2,r0` copy) | 184 |
| R8 | Per-block pin scopes. Block 3 as `offset = rem << 8; offset += tiles; src(r4) = quotient + offset; dst ip; DMA r2`, which matches the target except the counter register (r3 vs r5). Block 1 then derives its mask as `control - 128` | 168 |
| R9 | R7's blocks 1-2 plus R8's block 3: same mask side effect | 168 |
| R10 | M1 with count-up loops and R8's block 3: no reversal any more | 138 |
| R11 | M1 with R8's block 3 and counter pinned to r5: an entry test appears (as Codex saw) | 96 |
| R12 | M1 with R8's block 3, counter unpinned | 86 |

What's left:
- In blocks 1-2, the hoisted stride, counter and inner mask come out as r5/r3/r2 instead of r2/r5/r3 (R3-R9). This is allocation priority.
- The division needs `idx` in r3 and a separate `asrs r0,r0,#2; adds r2,r0,#0` copy.
- `next->frame` should be read through r1.

Behaviour: the current source is functionally equivalent to the target, checked by reading the code and by
`tools/diff_test_functions.py` (300 randomized runs, 0 differences).

# ResolveWarioCeilingCollision, session 28 (parked)

Functionally equivalent to the target: manual read plus `tools/diff_test_functions.py`
(300 randomized runs, 0 differences, 318/397 original halfwords covered).

Trials (`python3 run_trial.py wario/normal ResolveWarioCeilingCollision CAND.c`, WSL, repo root;
"changed" = changed lines in the objcmp-style unified diff, target is 391 lines):

| Trial | Change | Changed |
|---|---|---|
| C0 | current src | 345 |
| C1 | declaration order: `adjX adjY sec side left offset savedX right pointValue result x y currentY previousY`, collision fields read in order 08/04/05/06 | 337 |
| C2 | reuse `x` for the initial probe | 401 |
| C3 | C2 + shared `setYFD`/`returnFD` labels | 359 |
| C4 | C1 + `register u32 result asm("r2")` | 307 |
| C5 | C3 + result pin | 307 |
| **C6** | C4 + `mask = 0xFFC0` assigned before each entry to compareFC (target loads the mask before the branch) | **305 (best)** |
| C7 | C6 + shared FD tails | 321 |
| C8 | C6 + `u16 *prevY = &gPreviousYPosition` | 379 |

Remaining differences in C6:
- Early path: target keeps `initialX` in r7 and `&gPreviousYPosition` in r8 across the call and reuses r8 at compareFC.
- Main path: target spills `pointValue` as HImode (`mov r1,sp; strh r0,[r1,#24]`, later `ldrh`), which needs
  a compiler temporary rather than a promoted u16 local; ours spills SImode.
- Target holds `&gPreviousYPosition` in r5 through the main path and reuses it in the compareFC tail.
- Target shares `ldrh r0,[sp,#6]; b <store yPosition; return 0xFD>` tails; C3/C7 shared tails didn't reproduce them.

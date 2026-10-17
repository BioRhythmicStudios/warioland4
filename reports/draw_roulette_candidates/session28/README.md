# DrawRoulette, session 28 (Claude): equivalent, parked

Functionally equivalent to the target: `tools/diff_test_functions.py`, 300 randomized runs, 0 differences
(1000/1138 original halfwords covered; 149 runs time out or fault identically in both builds).

Baseline (current src, isolated compile with reports/ceiling_collision_candidates/session28/run_trial.py):
target 1111 lines, ours 1081, 1356 changed lines, opcode similarity 93.4%. Parked after this one compile,
because a ~15-compile session can't close a gap that size.

The one global difference, worth fixing first next time:
- Target frame is `sub sp,#0x20`, exactly `struct RouletteRenderStack` (32 bytes), so the target has no spills.
- Ours is `sub sp,#0x24`: `negSin0` in the item affine block (scale clamp 512/16) spills to sp+32.
- Target holds sin64/sin0/negSin0 in r5/r8/sl across the FixedMul(…, FixedInverse(scale)) calls, with
  r4 = &gRouletteItemDrawOrder[itemOffset] built from a fresh literal load.
- Ours keeps the gRouletteItemDrawOrder base in ip from the preceding loop and puts sin64/sin0 in r8/sl.
  r5 is taken, so negSin0 gets no callee-saved register.
`spslots.py NAME` (WSL, repo root) compares sp-slot usage between the target and build/gen_trials/DrawRoulette/NAME.dis.

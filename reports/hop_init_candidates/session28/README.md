# WarioHopInit, session 28 (Claude): behaviour bug found and fixed (V8 installed, score 954)

## Behaviour bug in the current source
The current src (Hbase.c) has four locals pinned to r7: `tmpScrollA`, `tmpDmaSrc3`,
`bg1Control` and `speedPtr`. A `register ... asm("r7")` local does not reserve r7, so the
allocator also keeps `&gWarioHopRollingObject` in r7 across those blocks. The rolling-object
stores then go through `&gWarioHopJumpDuration`:
`strh r0,[r7,#0]; strh r1,[r7,#2]; strh r4,[r7,#6]; strh r4,[r7,#8]`.
In game terms: the jump-duration byte and the bytes next to it get overwritten with 0xA00/0x8C0/0,
and gWarioHopRollingObject is never initialised. `tools/diff_test_functions.py` shows a difference
on every run (e.g. 0x3004941..43). Codex's P2 has the same four pins and the same bug.

## Fix (first version; superseded by V8, see the follow-up at the end)
H0.c = Hbase.c with the four r7 pins removed (the locals become plain locals). Differential test:
100 runs, 0 differences, 306/454 original halfwords covered. Changed lines: 298 vs 334 for Hbase.
P2u.c (P2 with its r7 pins removed) is also correct, but scores 370.

## Why it doesn't converge without r7 pins
In the target, everything loaded before the first DMA wait is assigned before the wait in the
source (src1/src2/src4/src5, scroll, duration, player pointers). Constants used after the wait
(sUnk_87124F8, gWarioHopRollingObject, LC20..LC30) are loaded where they are used.
Without pins, agbcc emits new loads of those constant-pool entries (insns 1220-1229 in the -dL dump)
at the first `if ((s32)dmaStatus < 0)` branch, on both paths. They are present by the loop dump and
absent from the jump dump, and loop.c reports no movables for them. Final code keeps
sUnk_87124F8 in r3 and the rolling pointer in r7 from there. In H0 src2 is also
constant-propagated rather than held in r4. The old r7 pins blocked this (a hard-register variable
isn't CSE'd), but that is exactly what caused the miscompile, and r7 pins are now off-limits.

Pipeline: `sh test.sh NAME [RUNS]` (WSL, repo root; edit G) compiles NAME.c in isolation, builds
`build/playtest/trial_NAME.gba` with `tools/build_playtest_rom.py --asm`, then diff-tests it.

## Follow-up: V8 installed (score 954, behaviour-correct)

At the user's request H0 was sent to decomp.wtf (1306). That couldn't replace the 1004 saved best, which keeps
the lowest score, and releasing the claim wrote the buggy 1004 version back into src. So the pre-loading
was blocked another way: pinning each pre-loaded post-wait pointer to the low register the target uses,
never r7. A hard-register variable isn't CSE'd, so the load stays where it is used.
Each step was checked with `hoist.py` (which constants are loaded before the first wait's branch).
Steps that passed got a diff test (`test.sh`):

| Trial | Added pin | Changed | Opcode sim | Behaviour |
|---|---|---|---|---|
| H0 | none (r7 pins removed) | 298 | 97.3% | OK (server 1306) |
| V1 | DMA3 source r3, rolling pointer r0 | 364 | 97.8% | OK (server 1048) |
| V3 | + Bg1/Bg2 scroll pointers r0/r1 | 364 | 98.0% | OK |
| V4 | + acceleration out r1 / in r2 | 360 | 98.0% | (bug, see V7) |
| V5 | + speed table r3 | 342 | 97.8% | |
| V6 | + start-velocity pointer r3 | 334 | 97.8% | |
| V7 | + shadow destination r1 | 188 | 98.0% | **wrong**: 2 bytes at gWarioHopJumpStartVelocity |
| **V8** | V7 with the acceleration store after the division | **162** | **98.3%** | **OK, 300 runs (server 954, saved best)** |

V7's bug is the same class as the original. `accelOut asm("r1")` was assigned before
`0x2800 / (speed * speed)`, so its value had to survive the `__divsi3` call, and r1 is caller-saved.
The compiler stored through a clobbered r1. **Never keep a pinned call-clobbered register live across a call.**
Diff-test every pinned candidate.

Remaining V8 diff: the register rotation of the pointer loads before the wait (target r6->r9, r7->r10, r5->r8;
ours r7->r9, r5->r10, r7->r8), literal-pool offsets, and the BG-control and x/y constant moves.

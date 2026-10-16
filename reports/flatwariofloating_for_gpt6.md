# FlatWarioFloating   exact match verified (2026-10-16)

Status: **100% matched**, independently verified twice with decomp.wtf compile_and_diff.
Commit: `0e4db9a` (Match FlatWarioFloating).
Commit scope verified with git show: only src/wario/flat.c, 10 insertions and 4 deletions, all inside this function.
No remaining assembly differences. No compiler flags, compiler source, headers or unrelated functions were edited. No inline assembly added.

## What changed

The first animation-test early return now uses goto returnFF. After incrementing and re-reading frame, the final time-check suffix is duplicated under `if (counter)`; each branch jumps to returnFF for nonzero time and decrementFrame for zero time. returnFF follows the decrement store. Both arms are identical, so observable behavior and the number of runtime loads/stores are preserved.

The redundant C duplication is intentional matching structure. Do not simplify it to a single conditional: early and late compiler passes treat those forms differently.

Matched branch pair:

```asm
cmp r0, #0
bne 0x288
b   0x284
```

The register assignments, literal pools and every other instruction match too.

## Why the old source converged on the wrong pair

The baseline's expand (.rtl) dump already has an EQ branch around the nonzero-time return value materialization. Early jump optimization forwards that branch to decrementFrame. At that stage the fallthrough still sets return register r0 to 255 and jumps to the common epilogue. Only late jump optimization (.jump2, after register allocation) cross-jumps that duplicated 255 materialization to the final returnFF block. It retains the EQ condition, leaving:

```
if time == 0: goto decrementFrame
goto returnFF
```

This is **not** the literal-pool pass or Thumb's final branch emitter deciding to invert the condition. The .jump2 and .mach dumps retain the same EQ branch. Nor is it register allocation or reload rotation.

Relevant compiler source (read-only investigation, line numbers in this checkout):

- tools/agbcc/gcc/jump.c:1731 1780 detects a conditional jumping over an unconditional jump, calls invert_jump and removes the unconditional jump.
- jump.c:1794 1907 handles the `if (foo) bar; else break` layout and can exchange ranges. Its checks include LABEL_NUSES(label1) == 1.
- jump.c:1910 1994 handles conditional/unconditional cross-jumping.
- jump.c:2691 find_cross_jump and :2840 do_cross_jump find matching tails, retarget jumps and delete duplicate instructions.

Simple source inversion or two-goto rewrites are therefore normalized early. Plain shared returns can also let the early optimizer move the decrement block into the wrong place.

## What the successful RTL demonstrates

The duplicated suffix retains two incoming paths to decrementFrame through early optimization. The early jump pass inverts each local branch around the goto so BOTH suffix copies have the desired NE-to-returnFF, unconditional-to-decrement shape. It cannot immediately apply the same single-use-label range reshuffle while decrementFrame has those two users.

In this diagnostic compile (UIDs are specific to this source):

- Expand: selector jump 253; time jumps 267 and 292 use EQ around local gotos; decrementFrame label 357, returnFF label 380.
- Early .jump: time jumps 267 and 292 now use **NE -> 380**, with unconditional jumps 273 and 298 -> 357.
- .cse, .combine and .greg retain both suffix copies and the selector. Register allocation produces the same registers as the target.
- Late .jump2 removes selector jump 253 and the first suffix (267/273). It retains **292: NE -> 380; 298: jump -> 357**. The decrement and return labels remain in the desired order.
- Final lowering inserts the target literal pools without changing this pair.

This is supported directly by dumps, not inferred from score alone. The role of the LABEL_NUSES check in preventing the earlier reshuffle follows from the source check plus the two retained edges; no compiler instrumentation or modified compiler was used.

The choice of selector matters: `counter & 1` leaves an otherwise-dead AND calculation after the late dispatch removal. Testing `counter` directly only needs a compare, which is removed with its branch.

## Experiments this session

All candidates used compile_and_diff. Initial baseline: 99.8%, score 8. Five candidates, followed by an independent recompile of the exact successful source.

| ID | Source change | Verified outcome |
| --- | --- | --- |
| A | Block-local u8 result=255 for only the second early return | Score 8 unchanged |
| B | Final condition time != 0 OR value > 0x2E (second term is false in this path) | Score 8 unchanged |
| C | First early return -> goto returnFF; second early return unchanged | Score 8 unchanged |
| D | Both early returns use shared returnFF; duplicate final suffix under counter & 1 | 97.7%, score 108: branch pair CORRECT; extra mov r0,#1 and and r5,r0, plus three shifted literal operands |
| E | D with selector counter directly | **Exact match**, then **exact match reverified** |

Implementation audit: the initial edit intended to create C's duplication only changed the first early return because a string replacement matched one occurrence. Source inspection caught it; C is accurately described above. Actual duplication was applied and tested in D, then refined to E. No results from an untested source are claimed.

B's dump is informative: expand and early jump initially retain a NE time branch plus the redundant range test. CSE removes the redundant test and re-canonicalizes to EQ-to-decrement, so this technique does not defer the simplification far enough.

Previous failed inversions, switches, ordinary loops/gotos, shared-result-variable variants and __builtin_expect attempts are documented in decomp.wtf function notes and were reviewed before this session. No new brute-force permutation sweep was used.

## Verification and preservation

- Started from the claimed score-8 saved best, inspected complete target/diff/notes, and verified that baseline with compile_and_diff.
- Existing file differences from HEAD ignoring line endings were empty; initial Git index was empty.
- Successful candidate committed by decomp.wtf as 0e4db9a.
- Identical source submitted again: tool confirmed the function still matches.
- git show verifies that only this function was committed.
- Diagnostic -da runs wrote temporary files under /tmp and did not change project compiler settings.
- Reports and agent notes preserve the findings. No follow-up experiment is required for this function.

## Complete best verified source

```c
u8 FlatWarioFloating(void)
{
    register struct WarioData* temp asm("r2");
    register struct WarioData* wario asm("r4");
    int counter;
    struct WarioData* current;
    u8 value;
    const u16* yTable;
    u16 direction;
    s32 directionTest;
    u16 heldButtons;
    const struct WarioAnimationFrame* animation;
    register int frame asm("r1");

    temp = &gWarioData;
    direction = temp->horizontalDirection;
    directionTest = DPAD_RIGHT;
    directionTest &= direction;
    wario = temp;
    if (directionTest) {
        wario->xPosition += sFlatWarioHorizontalMovement[wario->unk_0A >> 2];
    } else {
        wario->xPosition -= sFlatWarioHorizontalMovement[wario->unk_0A >> 2];
    }

    current = wario;
    yTable = sFlatWarioVerticalMovement;
    counter = current->unk_0A;
    value = (u8)counter;
    current->yPosition += yTable[value >> 2];

    if (value <= 0x2E) {
        current->unk_0A = counter + 1;
        animation = sFlatWarioFloatingAnimation;
        frame = current->unk_1F;
        if (current->unk_1E < animation[frame].time) {
            goto returnFF;
        }
        temp = current;
        temp->unk_1E = 0;
        temp->unk_1F++;
        frame = temp->unk_1F;
        if (counter) {
            if (animation[frame].time != 0)
                goto returnFF;
            goto decrementFrame;
        } else {
            if (animation[frame].time != 0)
                goto returnFF;
            goto decrementFrame;
        }
    }

    heldButtons = gButtonsHeld;
    direction = wario->horizontalDirection;
    if (!(heldButtons & direction)) {
        s32 newDirection;

        newDirection = DPAD_LEFT | DPAD_RIGHT;
        newDirection ^= direction;
        wario->horizontalDirection = newDirection;
    }
    return 0xFE;

decrementFrame:
    {
        register int dec asm("r0");
        dec = frame - 1;
        temp->unk_1F = dec;
    }
returnFF:
    return 0xFF;
}
```

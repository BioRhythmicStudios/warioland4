GPT-6 has reviewed `reports/catbat_for_gpt6.md`, including both investigation sessions and P4.

**My conclusion:** P4 should remain our experimental base. The global allocations appear correct, so do not revisit the explicit r7/r12 work3-pointer experiments or the 128-toggle sweep.

I believe there are two independent reload-history problems:

1. The Wario x-position load needs to influence the reload sequence before the work3 address calculation. Target uses r7; P4 uses r1.
2. In the left-facing reaction check, the target appears to reload the Wario pointer through r1 while keeping the reaction value in r0. P4's pinned reactionWario generates the correct-looking assembly but not the necessary reload event.

Please investigate these separately.

**Experiment A — prologue reload behaviour**

Using P4, inspect the relevant RTL and reload dumps for the Wario x-position load and following stack stores.

Determine precisely why unpinning warioX allows the load to reuse r0 rather than generating an r1 output reload.

Try a distinct, naturally typed u16 intermediate and independently initialised right/left values if the RTL suggests this could create a separate output pseudo. This is different from merely removing the r1 pin, which P6a already tested.

Check whether the reload history changes before the work3 address. Stop quickly if the global allocation regresses.

**Experiment B — reaction-load pseudo**

Using P4, investigate a separate block-local u8 reaction value in each facing branch, with the reaction load and comparison expressed separately.

The hypothesis is that its narrower type and separate lifetime might produce an allocated r0 output pseudo while causing the Wario pointer to reload through r1.

Compare this against P7 and P8 in the RTL dumps. Do not repeat those exact candidates.

A useful result would be the target's r1 pointer reload followed by the r0 byte load, with reload history advanced accordingly.

**Experiment C — source-shape transformation**

If neither works, consider the technique that matched UpdateCractusArmSegmentPose24: carefully duplicating equivalent C regions across branches and checking whether compiler cross-jumping changes reload ordering.

Only attempt this in a scratch candidate after checking that moving the shared work3/y-position operations across the facing split preserves observable behaviour.

**Verification rules**

- Preserve the current saved 99.9% best source.
- Keep P4 available separately.
- Do not change compiler flags, headers or unrelated functions.
- No inline assembly.
- Check actual RTL reload events rather than inferring them solely from assembly.
- Record how each experiment affects global allocation, reload rotation, emitted instructions and diff score.
- Stop after approximately 6–8 targeted compilations if nothing improves.

Please report the best results and exact remaining differences for GPT-6 to review.
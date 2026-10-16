GPT-6 has reviewed reports/escaperipple_for_gpt6.md and proposes a targeted experiment concerning the phase pointer's register-class preference.

Catbat's work3 pointer received STACK_REG preference because its address originated from an addition. EscapeRipple's phase pointer originates from a copy or literal load and receives LO_REGS preference.

Hypothesis: Constructing EscapeRipple's phase pointer through pointer arithmetic may influence global allocation, potentially allowing r12 without explicitly pinning it.

Using the current best implementation as the baseline, test these source expressions independently:

A. phase_ip = phase_r3 + 0;

B. phase_ip = &phase_r3[i_r1]; with i_r1 known to be zero at that point.

C. phase_ip = phase_r3 + 1; with a corresponding phase_ip[-1] read that preserves the original address.

For each candidate:

1. Inspect RTL after expansion and optimisation.
2. Identify the pointer pseudo and its preferred register class.
3. Determine whether global allocation selects r12, or whether reload moves the pointer there.
4. Check whether the resulting assembly preserves the target's `mov ip, r3` with no additional instructions.
5. Verify actual compiled behaviour and the complete assembly diff.

Do not assume arithmetic syntax survives optimisation. If the pointer expression collapses to the original copy before register preferences are established, record that and move on.

Avoid unsafe r7 register pinning. Keep the verified 99.8% best untouched.

Limit this exploratory investigation to about 3–5 compilations, using RTL inspection to reject unpromising candidates early.

Report whether any candidate changes LO_REGS to STACK_REG, even if it doesn't improve the final assembly match.
# WarioHopDrawSprites candidates (Claude, session 27, 2026-10-17)

H8 matched. decomp.wtf auto-committed it as `04a2c15`. The starting point was decomp.wtf score 709 / 98.5%.
Codex's session 22 N1 (`reports/hop_draw_candidates/`), at 190 changed lines, was the reference.

These candidates start over without register pins, in the style of the matched OAM siblings
(`DrawSoundRoomSprites`, `RenderPauseScreenOam`, `RenderStageEntrySprites`). They use the
`for (; i < total; i++) { attr = *(src++); *(raw++) = attr; ... raw++; }` loop and a local OAM union with a
3-bit `affineIndex`, so that the affine sprites get the target's `& -15` mask.

"Changed" counts lines in the objcmp-normalized disassembly diff against
`expected/us/obj/minigames/wario_hop.o` (952 lines).

| File | Change from the previous kept candidate | Changed |
|---|---|---|
| H1.c | Full sibling-style rewrite of all 9 draw loops and the two affine blocks | 192 |
| H2.c | H1, plus a `u32 active` local for the camera mode and chained `matrixNPtr = matrixN-1Ptr + 1` | 106 |
| H3.c | H2, but the shadow loop reads `gWarioHopPlayerShadow` directly. That fixes its hoist order, but the `shadow` local still adds a load (rejected) | 130 |
| H4.c | H2, plus a base-pointer local for the obstacles (rejected) | 110 |
| H5.c | H2, with the `shadow` local removed and `gWarioHopPlayerShadow.` used everywhere. Only the obstacle copy remains | 88 |
| H6.c | H5, plus an `obstacle` pointer assigned before the inner loop (rejected) | 267 |
| H7.c | H5, plus an `obstacle` pointer also used for `animFrame` (rejected) | 269 |
| H8.c | H5, with `gWarioHopObstacles[({ j; })].animFrame` in the obstacle `src =` line | **0** |

## Why H8 works

The target sets up the obstacle draw pointer as `adds r0, r2, #0; adds r6, r1, r0`. H5 instead emits
`adds r6, r1, #0`, because cse2 reuses the `&gWarioHopObstacles[j]` sum from the `animFrame` load.

- Loop pass 1 hoists the inner draw loop's `gWarioHopObstacles` load (pseudo 772) into the j-loop body.
- In H5, the j loop's own symbol load (pseudo 754, life 4) is "not desirable" to hoist:
  `threshold * savings * lifetime >= insn_count` fails (`move_movables` in loop.c).
- In pass 2, `combine_movables` matches 772 with 754. Their savings and lifetimes add, the pair is hoisted
  as one register, and cse2 then folds the inner sum into a copy.
- Luids count every non-line-number note, including block notes. The statement expression adds one
  between the symbol load and the index arithmetic, so 754 reaches life 5 and is hoisted in pass 1.
- In pass 2, 772 counts as already moved, so it needs twice the score and stays in the j body as its own
  pseudo. cse2 no longer sees the two sums as equal. Reload rematerializes the symbol and inheritance
  copies it from r2, which gives the target's `adds r0, r2, #0`.

The H5 side comes from RTL dumps (cse, gcse, loop and cse2). The H8 side is inferred from `loop.c`, where
`moved_once` doubles the required score, and from the exact result. No dump was taken for H8.

There is a precedent for statement expressions in matched code: `src/credits.c`. H8 uses no asm and no register pins.

## Running a trial

Run under WSL from the repo root. `src/` is never written. Output goes to `build/hopdraw_trials/`.

```sh
python3 reports/hop_draw_sibling_candidates/run_trial.py reports/hop_draw_sibling_candidates/H8.c 60
python3 reports/hop_draw_sibling_candidates/opcode_diff.py H8
```

`run_trial.py` splices the candidate in place of the function's current definition, which ends at the
first column-0 `}`, in a copy of `src/minigames/wario_hop.c`.

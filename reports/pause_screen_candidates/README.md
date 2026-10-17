# RenderPauseScreenOam candidates (Claude, session 26, 2026-10-17)

F matched: commits `9337476` (declares `sUnk_86D36DC` as `[][4]`) and `239c3c4` (the match).
The starting point was decomp.wtf score 28807 / 45.4%, where the last ~505 target instructions had not been written.

"Changed" counts lines in the objcmp-normalized disassembly diff against
`expected/us/obj/pause_screen.o` (1,016 lines). "Opcodes" is the similarity of
the instruction sequences with operands stripped.

| File | Change from the previous kept candidate | Changed | Opcodes |
|---|---|---|---|
| A.c | Full rewrite: all 10 sections use the DrawSoundRoomSprites `for (; currentSlot < nextSlot; currentSlot++)` loop | 874 | 91.8% |
| B.c | Separate animation locals per section instead of one reused `animation` | 401 | 99.2% |
| C.c | B with one local shared by jewel/keyzer/CD only (rejected) | 419 | 99.2% |
| D.c | B, plus high-score arms assign `src = X->oam` (they cross-jump instead of if-converting), and keyzer/CD state locals as `u32` | 306 | 99.7% |
| E.c | D, plus section 1 reads `gPauseScreenAnimation` directly. The frame index then loads before the pointer, and reload gets the target's spill set (r5) | 96 | 99.9% |
| F.c | E, plus one shared `animation` local for language/jewel/jewel-piece/keyzer/CD (the target keeps all of them in r3) | **0** | 100% |

## Running a trial

Run under WSL from the repo root. `src/` is never written. Output goes to `build/pause_trials/`.

```sh
python3 reports/pause_screen_candidates/run_trial.py reports/pause_screen_candidates/F.c 60
python3 reports/pause_screen_candidates/opcode_diff.py F
```

`run_trial.py` splices the candidate into a copy of `src/pause_screen.c` and compiles it
like the Makefile does (`cc -E` then `agbcc -O2 -mthumb-interwork -fhex-asm -fprologue-bugfix`).
It then diffs against the expected object using `tools/objcmp.py`'s normalizer.
The end marker it looks for is this function's last statement, so adapt it before reusing the script for another function.

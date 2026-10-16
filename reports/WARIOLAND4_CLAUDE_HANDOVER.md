# Wario Land 4 — Claude Code handover after Codex's shift

**Prepared:** 16 October 2026  
**Project:** Wario Land 4 GBA matching decompilation  
**Longer-term goal:** native Windows/Linux/Android ports, then an optional playable Waluigi feature  
**Request:** Codex is being retired for now. Claude Code will resume as the primary investigator.

## 1. Progress snapshot — verify against decomp.wtf on return

- **Last confirmed exact matches:** **4,008 / 4,024 functions** — **16 remain**.
- **Last displayed code-size match:** **96.7%**.
- At the start of our recent concerted push, the project was approximately **3,997 / 4,024** and **95.4%** matched by code size.
- **Do not increase the exact-match total based solely on an independent ROM comparison.** In particular, `DrawHomerunDerby` has a verification discrepancy described below.
- decomp.wtf is a local application: its saved attempts, agent notes and matching database are **not fully backed up by Git commits alone**.

## 2. Codex's confirmed exact-match contributions

These successes were reported as verified through the project's matching workflow. Check the current decomp.wtf state before relying on an individual count.

| Function | Key finding / status |
| --- | --- |
| `PinballCounterUpdateDisplay` | Exact match; Codex's early automated pass. |
| `InitHomerunDerby` | Exact match; Codex's early automated pass. |
| `ResolveWarioStandardCollision` | Exact match; Codex's early automated pass. |
| `UpdateTogenobiOuterPartPosition` | Exact match, reverified; using `sSinCosTable` directly on the second lookup produced the required copy order. Commit reported as `19ffaff`. |
| `SetupShopkeeperSelectedItem` | Exact match after earlier ~99.3% best. Commit shown as `b501a4f`. |
| `FlatWarioFloating` | **100%, verified twice**; commit `0e4db9a`. Final conditional time check was duplicated in both arms of `if (counter)`, using common decrement/return labels. Early optimisation retained the required `bne` orientation, and later passes eliminated the duplicated code. This is a useful agbcc source-structure lesson. |

Claude also achieved exact matches during this period, notably `UpdateCatbatPatrol`, `UpdateCractusArmSegmentPose24`, `CheckWarioHorizontalCollision`, and `UpdateBlackDragonAttack` as shown in project activity. **Please verify the live function list**, especially functions whose status was corrected during the evening, rather than reconstructing counts from our chat.

## 3. `DrawHomerunDerby`: very important, not yet officially reconciled

- Earlier saved decomp.wtf best: **99.8%, score 93**. Codex subsequently changed the static-OAM mask/preheader source to restore an apparently missing reload-allocation event.
- Codex reported **two independent byte comparisons of a linked 2,792-byte candidate matching the original ROM exactly**, including resolved calls/literals at the intended ROM address.
- **However, the decomp.wtf MCP matcher still reported a nonzero score (87).** Its explanation was that the tool classified some byte-identical items as different. **Do not simply mark this function 100% until the discrepancy is independently understood and verified using the normal matching workflow.**
- The candidate was committed as **`89a6f56`**, message `Match DrawHomerunDerby reload allocation`. Git confirmed the commit changed **only** `src/minigames/homerun_derby.c` (13 insertions, 6 deletions). Source and extensive notes remain at `reports/drawhomerunderby_for_gpt6.md`, including later uncommitted report updates.
- A Git author-identity error briefly blocked the commit. It was subsequently fixed locally, and `git log` confirmed the commit exists. No need to recreate it.
- The breakthrough involved coordinating materialisation of `0xFFFFFE00` and `-15` so the compiler's reload cursor delivered the intended register choices. The previous Q/R and `positionValue` experiments failed because downstream reloads shifted; inspect the saved report before testing variants.

## 4. Other stubborn functions and saved evidence

### `func_804A508` — swinging platform

- Codex improved **97.9%, score 425 → 99.2%, score 165**, reverified and saved as the best decomp.wtf attempt. **Still unmatched; no exact-match commit.**
- Saved report: `reports/swinging_platform_for_gpt6.md`. Read Claude's preceding report `reports/func_804A508_for_gpt6.md` too.
- Main remaining mismatch occurs at the first `mode1HighY` stack-slot store: target has `mov r3, sl; adds r3, r3, r1; str r3,[sp,#8]`, while current code reuses `r0` for the add/store. This missing copy shifts some branch/literal-pool alignment. Other later X-address and signed-load scratch register differences remain.
- Useful successful changes: widened signed delta, private-stack volatile-qualified **pointer reads** (not volatile ROM reads), narrowed `xPosition`, block-local pointer types/ordering, and unpinned `frameOffset`. See report for precise source and 15 failed/tested candidates.
- **Caution:** `src/sprite_ai/swinging_platform.c` also contains pre-existing uncommitted edits to sibling `func_804A84C`. Never stage the entire file blindly when committing just this function.

### `UpdateEscapeRippleHBlank`

- Best remains **99.8%, decomp.wtf score 5**. Codex independently verified only **two differing bytes** in the 212-byte function.
- Target `mov r7, ip; ldrb r7,[r7,#0]`; current `mov r6, ip; ldrb r7,[r6,#0]`.
- Reports: `reports/escaperipple_for_gpt6.md` in main repository; `ESCAPERIPPLE_CODEX_RESULTS.md` in **separate worktree** `../warioland4-escaperipple`.
- Six pointer-lifetime/source-shape candidates all regressed. Subsequent read-only allocator audit found ordinary phase-pointer pseudos prefer `LO_REGS` and remain in `r7`; forced-conflict ideas weren't substantiated. No safe matching change found; don't repeat these variants or unsafe `r7` pins.
- Codex's control experiment changing the two operands in a **separate assembly copy** matched all 212 bytes, but **that is not a C-source match**.

### Other investigated functions

- Near-matches and analysis are documented in `reports/`, including Shopkeeper, Catbat and collision analyses. Read prior notes before repeating experiments.
- Catbat's matching work was aided by investigation of Wario x-position and agbcc reload behaviour; explicit `r7`/`r12` pointer pinning had previously caused a major regression and should not be recycled indiscriminately.

## 5. Git and worktree state

**Primary working tree**  
`.` (repository root)  
Branch: `decomp-wtf`  
Latest confirmed HEAD in terminal: **`89a6f56`** (`Match DrawHomerunDerby reload allocation`).

**Separate Codex scratch worktree**  
`../warioland4-escaperipple`  
Branch: `codex-escaperipple` (shown at `10b8238` in worktree listing). It contains isolated scratch evidence. **Do not delete before reviewing/copying any useful report.**

**Remotes**

- `origin` → `https://github.com/TsilaAllaoui/warioland4` (someone else's upstream repo)
- `myfork` → `https://github.com/BioRhythmicStudios/warioland4` (our fork)
- Intended publication branch: `decomp-wtf` on `myfork`. **The configuration and diff review are confirmed; successful push is not confirmed in this handover.** Verify with `git ls-remote --heads myfork decomp-wtf` and compare to local HEAD before assuming it's backed up remotely.

A recent `git status --short` showed **numerous uncommitted changes**: several source files (`boss_door_opening.c`, roulette files, `wario_hop.c`, `pause_screen.c`, `sound_room.c`, `swinging_platform.c`, `wario/normal.c`), modified tracked reports, untracked `HANDOVER.md` and some `reports/*.md`, and a **modified `tools/agbcc` submodule**. The `agbcc` modification predates some Codex work and was deliberately left alone. Recheck the current state; it may have changed since that screenshot.

**Safety:** Do not run `git reset --hard`, `git clean`, broad `git add -A`, force-push, or stage toolchain modifications without reviewing them. Preserve Claude's unfinished work. Some reports contain absolute user paths and should be reviewed before public publishing. Do not commit/upload ROMs, extracted Nintendo assets, auth tokens or local secrets.

Suggested first **read-only** commands:

```bash
cd ~/Decomp/warioland4-2
git status --short
git --no-pager log -12 --oneline
git worktree list
git remote -v
git ls-remote --heads myfork decomp-wtf
```

## 6. Tooling and coordination

- Codex CLI was connected successfully to the local decomp.wtf MCP server, exposing ~30 tools, including `compile_and_diff`, `get_function`, agent notes, and function claiming/release.
- The **decomp.wtf automatic Codex agent launcher** once failed with `url is not supported for stdio in mcp_servers.decomp`; the **manually launched Codex CLI** remained connected. This appears to be launcher/config interaction, not a decompilation-source issue.
- Local decomp.wtf progress and Git history are separate things. **Back up its local database/notes** before migrations or cleanup. Do not assume pushing the Git branch captures every saved attempt.
- Codex is being paused/retired for now. Claude should take over without two agents modifying the same file or backend function concurrently.

## 7. Requested next actions for Claude

1. **Audit only, first:** Verify decomp.wtf's current exact-match count and status for `DrawHomerunDerby`, `func_804A508`, `UpdateEscapeRippleHBlank`, and recent exact matches. Do not infer a new green tick from a high similarity score.
2. Review commit `89a6f56`, the updated Homerun Derby report and the **score-87 versus 2,792-byte equality discrepancy**. Check source and actual tool comparison context (ROM address, relocation handling, raw bytes), and whether a normal verified exact match can be recorded without changing compiler settings.
3. Confirm previously verified matches are preserved and inspect *all* uncommitted/staged work before making another commit or push. Keep the `tools/agbcc` submodule untouched unless explicitly approved.
4. Before continuing any unmatched function, read its saved best C, complete target-versus-current assembly, prior attempts, and notes; use small targeted experiments and restore regressions.
5. Review and preserve useful evidence from the `codex-escaperipple` worktree before removing it. Do not merge scratch code blindly.
6. After all committed/uncommitted work is reconciled, help prepare a clean push to `BioRhythmicStudios/warioland4` on `decomp-wtf`. Respect upstream contribution policies regarding AI-assisted decompilation if considering a PR.
7. Provide a short prioritized list of remaining mismatches and recommend the next function to pursue. The owner is comfortable spending a day on one hard function if the experiments are well documented.

**Bottom line:** Codex materially advanced the project, delivered several exact matches, solved the long-stubborn `FlatWarioFloating` branch layout, improved `func_804A508` to 99.2%, thoroughly investigated EscapeRipple, and left a potentially byte-exact `DrawHomerunDerby` candidate that still needs decomp.wtf score reconciliation. The project's last reliably reported official total is **4,008 / 4,024**. Preserve all of this before pursuing further matches.

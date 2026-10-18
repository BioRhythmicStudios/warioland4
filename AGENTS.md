# Working rules for this worktree

This is the `port` worktree of the Wario Land 4 decompilation (branch `port`,
`C:\Users\Benny\Decomp\warioland4-port`). The main tree is `C:\Users\Benny\Decomp\warioland4-2`.
Read `CODEX_HANDOVER.md` first, then `docs/PC_PORT_PLAN.md` and `port/README.md`.

## Rules

1. **Never touch `tools/agbcc`.** Don't edit, stage, restore or replace it. It is a WSL
   symlink into the main tree, and Windows git shows it as a type change it cannot hash.
   Leave that as it is.
2. **Never edit the main `warioland4-2` tree**, except for cherry-picks the user has approved.
   Ask before doing anything there.
3. **The GBA build must stay byte-identical after every change.** The ROM MD5 must stay
   `940b597bef3be3f4041021b47a2f4096`, and every object must equal `build/us/obj`. Check this
   after each change, in a fresh `build/v7-NN` directory (see `CODEX_HANDOVER.md`). The last
   one used is `build/v7-18`.
4. **Delete only under `build/`, as plain commands with literal paths.** For example
   `rm build/v7-03.log`. No `rm` inside `sh -c`, no wildcards built by scripts, nothing
   outside `build/` unless the user asks for that exact path.
5. **Commit by name only.** Use `git add path/one path/two`. Never `git add -A`, `git add .`
   or `git commit -a`. Never force-push. Push to `myfork` (`git push myfork port`), never
   to `origin`.
6. **Update "Where we are" in `docs/PC_PORT_PLAN.md` before stopping.** Cover what changed,
   the latest commits, the last verified `build/v7-NN`, and the next step.

## Practical notes

- **Don't run `make clean` in this worktree.** It runs `rm -rf include/data`, and
  `include/data` is a symlink into the main tree.
- **Plain `git status` and `git diff --stat` fail** on `tools/agbcc`. Pass paths instead:
  `git status --short -- port src include docs CMakeLists.txt`.
- **Line endings.** Some files are CRLF, for example `port/README.md` and
  `src/wario/normal.c`. Git Bash `sed -i` turns them into LF, so edit them in a way that
  keeps CRLF.
- **Game source changes** need `#ifdef PORTABLE` (or the `GP()`/`GBA_PTR()` accessors) so the
  GBA build is unchanged. Then regenerate `port/generated/` as `port/README.md` describes.
- **Wait for the user's approval** before starting each milestone step.
- **Write commit messages in plain prose**, saying what changed, why, and how it was tested.

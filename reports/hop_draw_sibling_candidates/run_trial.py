"""Isolated WarioHopDrawSprites trial. Run under WSL from the repo root:
    python3 reports/hop_draw_sibling_candidates/run_trial.py reports/hop_draw_sibling_candidates/NAME.c [DIFF_LINES]
Splices the candidate definition into a copy of src/minigames/wario_hop.c (src/ is never written),
compiles it like the Makefile does and diffs objcmp-normalized disassembly with expected/."""
import difflib
import re
import subprocess as sp
import sys
from pathlib import Path

FUNC = 'WarioHopDrawSprites'
cand = Path(sys.argv[1])
out = Path('build/hopdraw_trials')
out.mkdir(parents=True, exist_ok=True)
name = cand.stem

base = Path('src/minigames/wario_hop.c').read_text()
start = base.index('void %s(void)\n{' % FUNC)
end = base.index('\n}\n', start) + 3  # the original body has no column-0 brace
body = cand.read_text().replace('\r\n', '\n')
text = base[:start] + body.rstrip('\n') + '\n' + base[end:]
src = out / (name + '.c')
src.write_text(text)
pre = sp.run(['cc', '-E', '-DVERSION_US', '-Iinclude', '-nostdinc', str(src)], capture_output=True, text=True, check=True).stdout
asm = out / (name + '.s')
r = sp.run(['tools/agbcc/agbcc', '-O2', '-mthumb-interwork', '-fhex-asm', '-fprologue-bugfix', '-Wall', '-o', str(asm)],
           input=pre, capture_output=True, text=True)
if r.returncode:
    print(r.stderr[-3000:])
    sys.exit(1)
warn = [l for l in r.stderr.splitlines() if FUNC in l or 'warning' in l]
obj = out / (name + '.o')
sp.run(['arm-none-eabi-as', '-Iasm', '-mcpu=arm7tdmi', '--defsym', 'VERSION_US=1', str(asm), '-o', str(obj)], check=True)

scope = {'__file__': str(Path('tools/objcmp.py').resolve())}
exec(Path('tools/objcmp.py').read_text().split('\nbad = []')[0], scope)
a = scope['funcs']('expected/us/obj/minigames/wario_hop.o')[FUNC]
b = scope['funcs'](str(obj))[FUNC]
diff = list(difflib.unified_diff(a, b, n=1, lineterm=''))
(out / (name + '.diff')).write_text('\n'.join(diff) + '\n')
(out / (name + '.dis')).write_text('\n'.join(b) + '\n')
sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
changed = sum(l[:1] in '+-' for l in diff[2:])
print('%s: target %d lines, ours %d, changed %d, similarity %.1f%%' % (name, len(a), len(b), changed, 100 * sm.ratio()))
for w in warn[:10]:
    print('  ' + w)
print('\n'.join(diff[:int(sys.argv[2]) if len(sys.argv) > 2 else 60]))

"""Generic isolated trial. Run under WSL from the repo root:
    python3 run_trial.py UNIT FUNC CANDIDATE.c [DIFF_LINES]
UNIT is the path under src/ without .c (e.g. wario/normal). The candidate holds one complete
definition of FUNC; it replaces FUNC's definition (up to the first column-0 '}') in a copy of
the unit. src/ is never written. Output goes to build/gen_trials/<FUNC>/.
"""
import difflib
import re
import subprocess as sp
import sys
from pathlib import Path

unit, func, cand = sys.argv[1], sys.argv[2], Path(sys.argv[3])
nshow = int(sys.argv[4]) if len(sys.argv) > 4 else 40
out = Path('build/gen_trials') / func
out.mkdir(parents=True, exist_ok=True)
name = cand.stem
base = Path('src/%s.c' % unit).read_text()
m = re.search(r'^[^\n;{}]*\b%s\([^;{]*\)\s*\{' % func, base, re.M)
start = m.start()
end = base.index('\n}\n', start) + 3
body = cand.read_text().replace('\r\n', '\n')
src = out / (name + '.c')
src.write_text(base[:start] + body.rstrip('\n') + '\n' + base[end:])
pre = sp.run(['cc', '-E', '-DVERSION_US', '-Iinclude', '-nostdinc', str(src)], capture_output=True, text=True, check=True).stdout
flags = ['-O2', '-mthumb-interwork', '-fhex-asm', '-fprologue-bugfix', '-Wall']
asm = out / (name + '.s')
r = sp.run(['tools/agbcc/agbcc'] + flags + ['-o', str(asm)], input=pre, capture_output=True, text=True)
if r.returncode:
    print(r.stderr[-3000:])
    sys.exit(1)
obj = out / (name + '.o')
sp.run(['arm-none-eabi-as', '-Iasm', '-mcpu=arm7tdmi', '--defsym', 'VERSION_US=1', str(asm), '-o', str(obj)], check=True)
scope = {'__file__': str(Path('tools/objcmp.py').resolve())}
exec(Path('tools/objcmp.py').read_text().split('\nbad = []')[0], scope)
a = scope['funcs']('expected/us/obj/%s.o' % unit)[func]
b = scope['funcs'](str(obj))[func]
diff = list(difflib.unified_diff(a, b, n=1, lineterm=''))
(out / (name + '.diff')).write_text('\n'.join(diff) + '\n')
(out / (name + '.dis')).write_text('\n'.join(b) + '\n')
changed = sum(l[:1] in '+-' for l in diff[2:])
op = lambda l: l.split()[0] if l else l
osim = difflib.SequenceMatcher(None, [op(x) for x in a], [op(x) for x in b], autojunk=False).ratio()
print('%s: target %d lines, ours %d, changed %d, opcode similarity %.1f%%' % (name, len(a), len(b), changed, 100 * osim))
for w in r.stderr.splitlines():
    if 'warning' in w and func in w:
        print('  ' + w)
print('\n'.join(diff[2:2 + nshow]))

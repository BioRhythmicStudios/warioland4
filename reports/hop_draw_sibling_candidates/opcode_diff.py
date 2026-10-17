"""Opcode-only diff of target vs a run_trial.py build (run under WSL from repo root): opcode_diff.py NAME"""
import difflib, sys
from pathlib import Path
scope = {'__file__': str(Path('tools/objcmp.py').resolve())}
exec(Path('tools/objcmp.py').read_text().split('\nbad = []')[0], scope)
a = scope['funcs']('expected/us/obj/minigames/wario_hop.o')['WarioHopDrawSprites']
b = (Path('build/hopdraw_trials') / (sys.argv[1] + '.dis')).read_text().splitlines()
op = lambda l: l.split()[0] if l else l
ao, bo = [op(x) for x in a], [op(x) for x in b]
sm = difflib.SequenceMatcher(None, ao, bo, autojunk=False)
print('opcode similarity %.1f%%' % (100 * sm.ratio()))
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag != 'equal':
        print('%s T[%d:%d] Y[%d:%d]' % (tag, i1, i2, j1, j2))
        for k in range(i1, i2): print('   -%4d %s' % (k, a[k]))
        for k in range(j1, j2): print('   +%4d %s' % (k, b[k]))

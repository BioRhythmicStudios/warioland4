import re, sys
from pathlib import Path
from collections import Counter
scope = {'__file__': str(Path('tools/objcmp.py').resolve())}
exec(Path('tools/objcmp.py').read_text().split('\nbad = []')[0], scope)
a = scope['funcs']('expected/us/obj/minigames/roulette/draw_roulette.o')['DrawRoulette']
b = Path('build/gen_trials/DrawRoulette/%s.dis' % sys.argv[1]).read_text().split('\n')
def sp(lines):
    c = Counter()
    for l in lines:
        m = re.search(r'\[sp, #(\d+)\]', l) or re.search(r'add r\d+, sp, #(\d+)', l) or re.search(r'mov r\d+, sp', l)
        if m: c[(l.split()[0], int(m.group(1)) if m.groups() else -1)] += 1
    return c
print('target', sorted(sp(a).items(), key=lambda x: x[0][1]))
print('ours  ', sorted(sp(b).items(), key=lambda x: x[0][1]))

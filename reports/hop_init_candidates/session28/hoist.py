# print the loads before the first wait's branch and the literal each one names
import re, sys
from pathlib import Path
for n in sys.argv[1:]:
    s = Path('build/gen_trials/WarioHopInit/%s.s' % n).read_text().split('\n')
    i = s.index('WarioHopInit:')
    body = s[i:]
    lits = []
    j = body.index('.L73:')
    for l in body[j + 1:]:
        if '.word' not in l:
            break
        lits.append(l.split()[-1])
    k = next(k for k, l in enumerate(body) if 'bge\t.L37' in l)
    pre = [l.strip() for l in body[k - 13:k] if 'ldr' in l and '.L73' in l]
    def name(l):
        m = re.search(r'\.L73\+0x([0-9a-f]+)', l)
        return lits[int(m.group(1), 16) // 4] if m else '?'
    print(n, ' | '.join('%s=%s' % (l.split()[1].rstrip(','), name(l)) for l in pre))

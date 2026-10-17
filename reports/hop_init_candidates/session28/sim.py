import difflib, sys
from pathlib import Path
scope = {'__file__': str(Path('tools/objcmp.py').resolve())}
exec(Path('tools/objcmp.py').read_text().split('\nbad = []')[0], scope)
a = scope['funcs']('expected/us/obj/minigames/wario_hop.o')['WarioHopInit']
op = lambda l: l.split()[0] if l else l
for n in sys.argv[1:]:
    b = Path('build/gen_trials/WarioHopInit/%s.dis' % n).read_text().split('\n')[:-1]
    s = difflib.SequenceMatcher(None, [op(x) for x in a], [op(x) for x in b], autojunk=False)
    full = difflib.SequenceMatcher(None, a, b, autojunk=False)
    print(n, 'opcode %.3f exact-line %.3f len %d' % (s.ratio(), full.ratio(), len(b)))

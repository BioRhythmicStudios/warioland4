import re, sys
src, func, out = sys.argv[1:4]
t = open(src).read().replace('\r\n', '\n')
m = re.search(r'^[^\n;{}]*\b%s\([^;{]*\)\s*\{' % func, t, re.M)
open(out, 'w').write(t[m.start():t.index('\n}\n', m.start()) + 3])

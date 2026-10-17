#!/usr/bin/env python3
"""Rewrite empty-template asm statements as ASM_BARRIER() / ASM_BARRIER_VOLATILE().

`asm("" : "+r"(x))` becomes `ASM_BARRIER(: "+r"(x))` and `asm volatile("" ...)`
becomes `ASM_BARRIER_VOLATILE(...)`. The macros (include/gba/defines.h) expand to
the same asm statement for agbcc and to nothing for PORTABLE builds, so the GBA
build is unchanged. Only asm statements whose template is the empty string are
touched. The rewrite is idempotent, so run it again after merging new matches.

usage: tools/port_asm_barrier_codemod.py [--check] [paths...]   (default: src include)
  --check  rewrite nothing; list remaining barriers and exit 1 if any are found
"""
import pathlib
import re
import sys

BARRIER = re.compile(r'\b(?:asm|__asm__|__asm)\b(\s+(?:volatile|__volatile__)\b)?\s*\(\s*""')
SKIP = {pathlib.Path('include/gba/defines.h')}


def closing_paren(text, open_index):
    depth = 0
    i = open_index
    while i < len(text):
        c = text[i]
        if c == '"':
            i += 1
            while text[i] != '"':
                i += 2 if text[i] == '\\' else 1
        elif c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise ValueError('unbalanced asm statement')


def rewrite(text):
    out, last, count = [], 0, 0
    for m in BARRIER.finditer(text):
        if m.start() < last:
            continue
        open_index = text.index('(', m.end(1) if m.group(1) else m.start())
        close = closing_paren(text, open_index)
        operands = text[m.end():close].strip(' ')
        name = 'ASM_BARRIER_VOLATILE' if m.group(1) else 'ASM_BARRIER'
        out += [text[last:m.start()], f'{name}({operands})']
        last, count = close + 1, count + 1
    out.append(text[last:])
    return ''.join(out), count


def main(argv):
    check = '--check' in argv
    roots = [a for a in argv if a != '--check'] or ['src', 'include']
    files = []
    for root in roots:
        p = pathlib.Path(root)
        files += [p] if p.is_file() else sorted(p.rglob('*.[ch]'))
    total = 0
    for path in files:
        if path in SKIP:
            continue
        text = path.read_bytes().decode('utf-8')
        new, n = rewrite(text)
        if not n:
            continue
        total += n
        if check:
            for m in BARRIER.finditer(text):
                print(f'{path}:{text.count(chr(10), 0, m.start()) + 1}: {m.group(0)}')
        else:
            path.write_bytes(new.encode('utf-8'))
            print(f'{path}: {n}')
    print(f'{"remaining" if check else "rewrote"} {total} barriers')
    return 1 if check and total else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

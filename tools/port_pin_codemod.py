#!/usr/bin/env python3
"""Rewrite register pins `register T x asm("r4")` as `register T x PIN("r4")`.

PIN (include/gba/defines.h) expands to asm() for agbcc and to nothing for PORTABLE
builds, so the GBA build is unchanged. Only asm() labels inside `register`
declarations are touched; symbol-renaming asm labels elsewhere are left alone.
The rewrite is idempotent, so run it again after merging new matches.

usage: tools/port_pin_codemod.py [--check] [paths...]   (default: src include)
  --check  rewrite nothing; list remaining pins and exit 1 if any are found
"""
import pathlib
import re
import sys

REGS = r'r(?:[0-9]|1[0-2])|sl|fp|ip|sb|sp|lr'
PIN_ASM = re.compile(r'\b(?:asm|__asm__|__asm)\s*\(\s*("(?:%s)")\s*\)' % REGS)
REGISTER = re.compile(r'\bregister\b')


def register_decls(text):
    """Yield (start, end) of each `register` declaration, including `register union { ... } x`."""
    for m in REGISTER.finditer(text):
        depth = 0
        for i in range(m.end(), len(text)):
            c = text[i]
            if c == '{':
                depth += 1
            elif c == '}':
                depth -= 1
                if depth < 0:
                    break
            elif c == ';' and depth == 0:
                yield m.start(), i + 1
                break


def rewrite(text):
    out, last, count = [], 0, 0
    for start, end in register_decls(text):
        if start < last:
            continue
        new, n = PIN_ASM.subn(r'PIN(\1)', text[start:end])
        out += [text[last:start], new]
        last, count = end, count + n
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
        raw = path.read_bytes()
        text = raw.decode('utf-8')
        new, n = rewrite(text)
        if not n:
            continue
        total += n
        if check:
            for start, end in register_decls(text):
                for m in PIN_ASM.finditer(text, start, end):
                    print(f'{path}:{text.count(chr(10), 0, m.start()) + 1}: {m.group(0)}')
        else:
            path.write_bytes(new.encode('utf-8'))
            print(f'{path}: {n}')
    print(f'{"remaining" if check else "rewrote"} {total} pins')
    return 1 if check and total else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Mark every struct and union definition with AGB_ALIGN.

agbcc aligns every struct and union to at least 4 bytes, so their sizes are multiples of 4;
`struct { u8 a; u8 b; }` takes 4 bytes. AGB_ALIGN (include/gba/types.h) expands to nothing
for agbcc and to __attribute__((aligned(4))) for PORTABLE builds, which gives the PC build
the same layouts. `struct Foo {` becomes `struct AGB_ALIGN Foo {`, and the GBA build is
unchanged. The rewrite is idempotent, so run it again after merging new matches.

usage: tools/port_struct_align_codemod.py [--check] [paths...]   (default: src include, except src/data)
  --check  rewrite nothing; list unmarked definitions and exit 1 if any are found
"""
import pathlib
import re
import sys

# A definition: the keyword, an optional tag, then the opening brace.
RECORD = re.compile(r'\b(struct|union)(\s+)(?!AGB_ALIGN\b)((?:\w+\s*)?\{)')
SKIP = {pathlib.Path('include/gba/defines.h')}


def code_spans(text):
    """Yield (start, end) for the stretches of text outside comments, strings and directives."""
    i, start, n = 0, 0, len(text)
    while i < n:
        if text.startswith('//', i) or (text[i] == '#' and not text[text.rfind('\n', 0, i) + 1:i].strip()):
            yield start, i
            while True:
                i = text.find('\n', i)
                if i < 0 or text[i - 1] != '\\':
                    break
                i += 1
            i = n if i < 0 else i
            start = i
        elif text.startswith('/*', i):
            yield start, i
            i = text.index('*/', i) + 2
            start = i
        elif text[i] in '"\'':
            q = text[i]
            i += 1
            while text[i] != q:
                i += 2 if text[i] == '\\' else 1
            i += 1
        else:
            i += 1
    yield start, n


def rewrite(text):
    out, last, found = [], 0, []
    for start, end in code_spans(text):
        for m in RECORD.finditer(text, start, end):
            out += [text[last:m.start()], m.group(1), ' AGB_ALIGN', m.group(2), m.group(3)]
            last = m.end()
            found.append(m.start())
    out.append(text[last:])
    return ''.join(out), found


def main(argv):
    check = '--check' in argv
    roots = [a for a in argv if a != '--check'] or ['src', 'include']
    files = []
    for root in roots:
        p = pathlib.Path(root)
        files += [p] if p.is_file() else sorted(p.rglob('*.[ch]'))
    files = [f for f in files if f.parts[:2] not in (('src', 'data'), ('include', 'data')) and f not in SKIP]
    total = 0
    for path in files:
        text = path.read_bytes().decode('utf-8')
        new, found = rewrite(text)
        if not found:
            continue
        total += len(found)
        if check:
            for pos in found:
                print(f'{path}:{text.count(chr(10), 0, pos) + 1}: {text[pos:text.find(chr(10), pos)].strip()}')
        else:
            path.write_bytes(new.encode('utf-8'))
            print(f'{path}: {len(found)}')
    print(f'{"unmarked" if check else "marked"} {total} definitions')
    return 1 if check and total else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

#!/usr/bin/env python3
"""Wrap the data that game sources define in #ifndef PORTABLE, with extern declarations for PC builds.

The PC port keeps every game variable and table at its GBA address: ROM data is read
from the user's ROM, and RAM variables live in the mapped EWRAM/IWRAM. So a definition
such as

    const u16 sVortexPal[] = {
    #include "data/sprites/vortex.pal.inc"
    };

becomes

    #ifndef PORTABLE
    const u16 sVortexPal[] = {
    #include "data/sprites/vortex.pal.inc"
    };
    #else
    extern const u16 sVortexPal[];
    #endif

and the port links sVortexPal to its address. Consecutive definitions share one block.
Tables of function pointers stay defined in PC builds, since they hold native addresses.
The GBA build is unchanged. The rewrite is idempotent, so run it again after merging new
matches.

usage: tools/port_data_codemod.py [--check] [paths...]   (default: src, except src/data)
  --check  rewrite nothing; list remaining definitions and exit 1 if any are found
"""
import pathlib
import re
import sys

ATTRIBUTE = re.compile(r'__attribute__\s*\(\(')
DROP_WORDS = re.compile(r'\b(?:IWRAM_DATA|static)\b')
FUNC_TYPEDEF = re.compile(r'typedef\b[^;]*?\(\s*\*\s*(\w+)\s*\)\s*\(')
IDENT = re.compile(r'[A-Za-z_]\w*')


def function_pointer_typedefs():
    names = set()
    for path in sorted(pathlib.Path('include').rglob('*.h')) + sorted(pathlib.Path('src').rglob('*.c')):
        names.update(FUNC_TYPEDEF.findall(path.read_bytes().decode('utf-8')))
    return names


def skip_literal(text, i):
    quote = text[i]
    i += 1
    while text[i] != quote:
        i += 2 if text[i] == '\\' else 1
    return i + 1


def top_level_items(text):
    """Yield (kind, start, end) for each top-level directive, declaration and function.

    kind is '#' for a preprocessor line, ';' for a declaration and '}' for a function body.
    start and end are offsets of the first and one past the last character of the item.
    """
    i, n = 0, len(text)
    start = None
    depth = 0
    has_init = False
    head = ''
    while i < n:
        c = text[i]
        if text.startswith('//', i):
            i = text.find('\n', i)
            i = n if i < 0 else i
            continue
        if text.startswith('/*', i):
            i = text.index('*/', i) + 2
            continue
        if c in '"\'':
            if start is None:
                start = i
            i = skip_literal(text, i)
            continue
        if c == '#' and not text[line_start(text, i):i].strip():
            end = i
            while True:
                end = text.find('\n', end)
                end = n if end < 0 else end
                if text[end - 1] != '\\':
                    break
                end += 1
            if start is None:
                yield '#', i, end
            i = end
            continue
        if c.isspace():
            i += 1
            continue
        if start is None:
            start, has_init, head = i, False, ''
        if c in '([{':
            if c == '{' and depth == 0 and not has_init and not re.match(r'\s*(typedef|struct|union|enum)\b', text[start:i]):
                head = 'function'
            depth += 1
        elif c in ')]}':
            depth -= 1
            if depth == 0 and c == '}' and head == 'function':
                yield '}', start, i + 1
                start = None
        elif c == '=' and depth == 0:
            has_init = True
        elif c == ';' and depth == 0:
            yield ';', start, i + 1
            start = None
        i += 1


def strip_attributes(head):
    out = head
    while True:
        m = ATTRIBUTE.search(out)
        if not m:
            return out
        depth, j = 0, m.end() - 2
        while True:
            if out[j] == '(':
                depth += 1
            elif out[j] == ')':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        out = out[:m.start()] + out[j + 1:]


def data_definition(item, func_types):
    """Return the extern declaration for a data definition, or None if it is not one to wrap."""
    if re.match(r'\s*(typedef|extern)\b', item):
        return None
    depth = 0
    for k, c in enumerate(item):
        if c in '([{':
            depth += 1
        elif c in ')]}':
            depth -= 1
        elif c == '=' and depth == 0:
            break
    else:
        return None
    if '".text"' in item[:k]:
        return None  # padding between functions, not data
    head = ' '.join(strip_attributes(item[:k]).split())
    if '(*' in head.replace(' ', '') or set(IDENT.findall(head)) & func_types:
        return None
    head = ' '.join(DROP_WORDS.sub('', head).split())
    if ',' in re.sub(r'\[[^]]*\]', '', head):
        raise ValueError('several declarators in one definition: ' + head)
    return 'extern ' + head + ';'


def line_start(text, i):
    return text.rfind('\n', 0, i) + 1


def line_end(text, i):
    j = text.find('\n', i)
    return len(text) if j < 0 else j + 1


def rewrite(text, func_types):
    """Return the rewritten text and the declarations that were wrapped."""
    blocks = []      # [start offset, end offset, [declarations]]
    guard = []       # stack of top-level conditionals: True inside #ifndef PORTABLE
    previous_end = None
    for kind, start, end in top_level_items(text):
        if kind == '#':
            directive = text[start:end].split()
            word = directive[0].lstrip('#') or (directive[1] if len(directive) > 1 else '')
            if word in ('if', 'ifdef', 'ifndef'):
                guard.append(text[start:end].replace(' ', '').startswith(('#ifndefPORTABLE', '#if!defined(PORTABLE)')))
            elif word == 'endif':
                guard.pop()
            previous_end = None
            continue
        if any(guard):
            previous_end = None
            continue
        decl = data_definition(text[start:end], func_types) if kind == ';' else None
        if decl is None:
            previous_end = None
            continue
        first, last = line_start(text, start), line_end(text, end)
        if text[first:start].strip() or text[end:last].strip() not in ('', '// clang-format off', '// clang-format on'):
            raise ValueError('definition shares a line with other code at offset %d' % start)
        if previous_end is not None and not re.sub(r'//[^\n]*|/\*.*?\*/', '', text[previous_end:first], flags=re.S).strip():
            blocks[-1][1] = last
            blocks[-1][2].append(decl)
        else:
            blocks.append([first, last, [decl]])
        previous_end = last
    out, pos = [], 0
    for first, last, decls in blocks:
        body = text[first:last]
        if not body.endswith('\n'):
            body += '\n'
        out += [text[pos:first], '#ifndef PORTABLE\n', body, '#else\n', ''.join(d + '\n' for d in decls), '#endif\n']
        pos = last
    out.append(text[pos:])
    return ''.join(out), [d for b in blocks for d in b[2]]


def main(argv):
    check = '--check' in argv
    roots = [a for a in argv if a != '--check'] or ['src']
    files = []
    for root in roots:
        p = pathlib.Path(root)
        files += [p] if p.is_file() else sorted(p.rglob('*.c'))
    files = [f for f in files if f.parts[:2] != ('src', 'data')]
    func_types = function_pointer_typedefs()
    total = 0
    for path in files:
        text = path.read_bytes().decode('utf-8')
        new, decls = rewrite(text, func_types)
        if not decls:
            continue
        total += len(decls)
        if check:
            for d in decls:
                print(f'{path}: {d}')
        else:
            path.write_bytes(new.encode('utf-8'))
            print(f'{path}: {len(decls)}')
    print(f'{"remaining" if check else "wrapped"} {total} definitions')
    return 1 if check and total else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

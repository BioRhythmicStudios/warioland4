#!/usr/bin/env python3
"""Generate _Static_assert checks that PC builds lay out the game's structs as agbcc does.

Run under WSL from the repo root (it needs tools/agbcc and libclang):
    ~/.cache/wl4-libclang-venv/bin/python port/tools/gen_layout_asserts.py

Every struct and union defined in include/ is measured twice: by agbcc, through a probe
file of sizeof/offsetof values that it compiles to assembly, and by libclang with the PC
target. The output, port/generated/layout_asserts_<n>.c, asserts agbcc's size and field
offsets. Some headers declare the same function or variable with different types (in the
PC view or agbcc's), so the headers are split into groups that compile together, one file per
group.

Structs defined in a game source are checked the same way by port/generated/layout_checks/
<source>.c, which includes the source itself. CMakeLists.txt compiles those but never links
them.

Pointers held in GBA memory are GBA_PTR() unions of 4 bytes (include/gba/gba_ptr.h). A
struct that still holds native pointers cannot match, so its checks are compiled only when
PORT_CHECK_POINTER_LAYOUTS is defined; only the unused FlashSetupInfo is left. Bitfields
have no offset, so only the size of a struct with bitfields is checked.
"""
import pathlib
import re
import subprocess as sp
import sys

import clang.cindex as ci

OUT = pathlib.Path('port/generated')
HOST_ARGS = ['-target', 'x86_64-w64-windows-gnu', '-mno-ms-bitfields', '-std=gnu89', '-funsigned-char',
             '-DPORTABLE', '-DVERSION_US', '-Iinclude', '-nostdinc', '-ffreestanding',
             '-isystem', 'port/tools/layout_stubs',
             # As in CMakeLists.txt: the GbaPtr conversion left these as warnings in the game sources.
             '-Wno-error=incompatible-pointer-types', '-Wno-error=incompatible-function-pointer-types',
             '-Wno-error=int-conversion']
CPP = ['cc', '-E', '-Iinclude', '-nostdinc', '-DVERSION_US', '-']
AGBCC = ['tools/agbcc/agbcc', '-O2', '-mthumb-interwork', '-fhex-asm']
RECORDS = (ci.CursorKind.STRUCT_DECL, ci.CursorKind.UNION_DECL)


def includes(headers):
    return ''.join('#include "%s"\n' % h for h in headers)


def parse(index, source):
    tu = index.parse('probe.c', HOST_ARGS, unsaved_files=[('probe.c', source)])
    errors = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error]
    return tu, errors


GBA_VIEW_ARGS = [a for a in HOST_ARGS if a != '-DPORTABLE']
CONFLICT = re.compile(r'conflicting types|redeclaration|redefinition')


def agree(index, headers):
    """True if the headers compile together on the PC, and agbcc's view (without PORTABLE,
    where some declarations differ) has no conflicting declarations either."""
    if parse(index, includes(headers))[1]:
        return False
    tu = index.parse('probe.c', GBA_VIEW_ARGS, unsaved_files=[('probe.c', includes(headers))])
    return not any(CONFLICT.search(d.spelling) for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error)


def group_headers(index, headers):
    """Split the headers into groups whose declarations agree with each other."""
    groups = []
    for h in headers:
        for g in groups:
            if agree(index, g + [h]):
                g.append(h)
                break
        else:
            tu, errors = parse(index, includes([h]))
            if errors:
                raise SystemExit('%s does not compile on its own: %s' % (h, errors[0]))
            groups.append([h])
    return groups


def holds_pointer(t, seen=()):
    t = t.get_canonical()
    if t.kind in (ci.TypeKind.POINTER, ci.TypeKind.BLOCKPOINTER, ci.TypeKind.FUNCTIONPROTO,
                  ci.TypeKind.FUNCTIONNOPROTO):
        return True
    if t.kind == ci.TypeKind.CONSTANTARRAY and t.element_count == 0:
        return False  # GBA_PTR()'s zero-length type carrier takes no space
    if t.kind in (ci.TypeKind.CONSTANTARRAY, ci.TypeKind.INCOMPLETEARRAY):
        return holds_pointer(t.element_type, seen)
    if t.kind == ci.TypeKind.RECORD:
        decl = t.get_declaration()
        if decl.hash in seen:
            return False
        return any(holds_pointer(f.type, seen + (decl.hash,)) for f in t.get_fields())
    return False


def fields(t, top=None):
    """Yield (name, host offset in bytes, is_bitfield), descending into unnamed members.

    libclang looks a field of an unnamed member up through the outermost record.
    """
    top = top or t
    for f in t.get_fields():
        if not f.spelling:
            yield from fields(f.type.get_canonical(), top)
            continue
        off = top.get_offset(f.spelling)
        yield f.spelling, off // 8 if off >= 0 else None, f.is_bitfield()


def type_name(cursor):
    """The C spelling of a record type, or None for a record without a tag."""
    tokens = [tok.spelling for _, tok in zip(range(3), cursor.get_tokens())]
    tokens = [s for s in tokens if s != 'AGB_ALIGN']
    if len(tokens) < 2 or tokens[1] == '{':
        return None
    return tokens[0] + ' ' + tokens[1]


def collect(index, group, records, source=None, prefix='include/'):
    tu, errors = parse(index, source or includes(group))
    assert not errors, errors
    typedef_of = {}
    for c in tu.cursor.walk_preorder():
        if c.kind == ci.CursorKind.TYPEDEF_DECL:
            u = c.underlying_typedef_type.get_canonical()
            if u.kind == ci.TypeKind.RECORD:
                typedef_of.setdefault(u.get_declaration().hash, c.spelling)
    for c in tu.cursor.get_children():
        nodes = [c] + ([d for d in c.get_children() if d.kind in RECORDS] if c.kind == ci.CursorKind.TYPEDEF_DECL else [])
        for d in nodes:
            if d.kind not in RECORDS or not d.is_definition() or not d.location.file:
                continue
            path = str(d.location.file.name).removeprefix('./')
            if not path.startswith(prefix):
                continue
            key = (path, d.location.line, d.location.column)
            if key in records:
                continue
            name = type_name(d) or typedef_of.get(d.hash)
            if name is None:
                continue
            t = d.type.get_canonical()
            fs = list(fields(t))
            records[key] = {
                'name': name, 'group': group, 'host_size': t.get_size(), 'host_align': t.get_align(), 'pointer': holds_pointer(t),
                'fields': [(n, off) for n, off, bit in fs if not bit and off is not None and n],
                'bitfields': any(bit for _, _, bit in fs),
            }


def agbcc_layouts(group, recs, source=None):
    """Return {(record name, item): value} for size, align and each field offset, as agbcc sees them."""
    items = []
    for r in recs:
        items.append((r['name'], '#size', 'sizeof(%s)' % r['name']))
        items.append((r['name'], '#align', '__alignof__(%s)' % r['name']))
        for f, _ in r['fields']:
            items.append((r['name'], f, '(unsigned int)&((%s *)0)->%s' % (r['name'], f)))
    src = (source or includes(group)) + 'const unsigned int layout_probe[] = {\n'
    src += ''.join('    %s,\n' % e for _, _, e in items) + '};\n'
    pre = sp.run(CPP, input=src, capture_output=True, text=True, check=True).stdout
    res = sp.run(AGBCC + ['-o', '/dev/stdout'], input=pre, capture_output=True, text=True)
    if res.returncode:
        (OUT / 'failed_probe.c').write_text(pre)
        raise SystemExit('agbcc failed on the group starting with %s (preprocessed probe: %s):\n%s'
                         % (group[0], OUT / 'failed_probe.c', res.stderr))
    asm = res.stdout
    body = asm[asm.index('layout_probe:'):]
    values = [int(v, 0) for v in re.findall(r'\.word\s+(\S+)', body)[:len(items)]]
    assert len(values) == len(items)
    return {(n, i): v for (n, i, _), v in zip(items, values)}


GENERATED_HEADER = [
    '// Generated by port/tools/gen_layout_asserts.py from the layouts agbcc gives these types.',
    '// Do not edit; rerun the script after changing a struct.',
    '#include "port/layout_assert.h"', '',
]


def checks_for(recs, gba, summary):
    lines = []
    for r in recs:
        n = r['name']
        checks = ['LAYOUT_SIZE(%s, %d);' % (n, gba[(n, '#size')])]
        checks += ['LAYOUT_OFFSET(%s, %s, %d);' % (n, f, gba[(n, f)]) for f, _ in r['fields']]
        host_ok = r['host_size'] == gba[(n, '#size')] and all(off == gba[(n, f)] for f, off in r['fields'])
        if r['pointer']:
            summary['pointer'] += 1
            lines += ['#ifdef PORT_CHECK_POINTER_LAYOUTS'] + checks + ['#endif']
        else:
            summary['checked'] += 1
            lines += checks
            if not host_ok:
                summary['mismatch'].append((n, r['host_size'], gba[(n, '#size')], r['host_align'], gba[(n, '#align')]))
    return lines


def main():
    headers = sorted(str(p) for p in pathlib.Path('include').rglob('*.h') if p.parts[1] != 'data')
    index = ci.Index.create()
    groups = group_headers(index, headers)
    records = {}
    for g in groups:
        collect(index, g, records)
    OUT.mkdir(parents=True, exist_ok=True)
    for old in OUT.glob('layout_asserts_*.c'):
        old.unlink()
    summary = {'checked': 0, 'pointer': 0, 'mismatch': []}
    for gi, g in enumerate(groups):
        recs = sorted((r for r in records.values() if r['group'] is g), key=lambda r: r['name'])
        if not recs:
            continue
        gba = agbcc_layouts(g, recs)
        lines = GENERATED_HEADER + ['#include "%s"' % h[len('include/'):] for h in g] + ['']
        lines += checks_for(recs, gba, summary)
        (OUT / ('layout_asserts_%d.c' % gi)).write_text('\n'.join(lines) + '\n')

    # Records defined in a game source file. Their checks include that file, so CMakeLists.txt
    # compiles them in an object library that is never linked.
    src_out = OUT / 'layout_checks'
    src_out.mkdir(exist_ok=True)
    for old in src_out.glob('*.c'):
        old.unlink()
    sources = sorted(p.as_posix() for p in pathlib.Path('src').rglob('*.c') if p.parts[1] != 'data')
    src_records = 0
    for path in sources:
        source = '#include "%s"\n' % path
        found = {}
        collect(index, [path], found, source, prefix=path)
        recs = sorted(found.values(), key=lambda r: r['name'])
        if not recs:
            continue
        src_records += len(recs)
        gba = agbcc_layouts([path], recs, source)
        lines = GENERATED_HEADER + ['#include "../../../%s"' % path, '']
        lines += checks_for(recs, gba, summary)
        (src_out / (path[len('src/'):-len('.c')].replace('/', '__') + '.c')).write_text('\n'.join(lines) + '\n')

    print('%d header groups, %d records in headers and %d in game sources: %d checked now, '
          '%d with native pointers, not checked'
          % (len(groups), len(records), src_records, summary['checked'], summary['pointer']))
    for n, hs, gs, ha, ga in summary['mismatch']:
        print('MISMATCH %s: PC size %d align %d, agbcc size %d align %d' % (n, hs, ha, gs, ga))
    for gi, g in enumerate(groups[1:], 1):
        print('group %d: %s' % (gi, ' '.join(g)))
    return 1 if summary['mismatch'] else 0


if __name__ == '__main__':
    sys.exit(main())

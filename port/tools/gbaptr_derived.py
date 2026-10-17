"""Second GbaPtr pass: locals and casts that point into converted tables.

usage: python port/tools/gbaptr_derived.py [--build-dir build_pc] [--dry-run]

Run after port/tools/gbaptr_rewrite.py. A local of type T ** (or T *const *) that is set from a
converted GBA_PTR() table or global, or from an address computed as an integer, points at
4-byte GBA pointers, while its declared type reads 8-byte native ones. For each such local it:
- declares it GBA_TYPEOF(T **, &table[0]) when it is set from one table or global, so that it
  has the type of the table's elements on the PC, and GBA_PTR(T *) * otherwise,
- turns casts assigned to it into GBA_TYPEOF(T **, local),
- converts its dereferences with GP()/GP_SET() as gbaptr_rewrite.py does.
A cast to T ** that is dereferenced at once, like *(const u16 *const *)(base + offset), becomes
GP(*(GBA_PTR(const u16 *) const *)(base + offset)).

Parameters, locals with several declarators and casts of local addresses are left alone and
listed, as are locals of type T ** that are dereferenced but never set from GBA memory (output
parameters filled in by the caller's native locals, for example), for checking by hand.
On the GBA all these macros expand to the original code.
"""
import argparse
import os
import re
import sys
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, os.path.dirname(__file__))
import clang_ast as ca  # noqa: E402
import gbaptr_rewrite as gr  # noqa: E402

# Clang names a GBA_PTR() union after the place the macro was expanded.
GBA_UNION = re.compile(r'union \(unnamed at [^)]*\)')
PTR_PTR = re.compile(r'\*\s*((const|volatile)\s*)*\*\s*((const|volatile)\s*)*$')


def qual(node):
    return (node.get('type') or {}).get('qualType', '')


def strip(node, kinds=('ParenExpr', 'ImplicitCastExpr')):
    while node.get('kind') in kinds and node.get('inner'):
        node = node['inner'][0]
    return node


def gba_union_kind(q):
    """True if q is a GBA_PTR() union, a pointer to one, or an array of them."""
    m = GBA_UNION.search(q)
    if not m:
        return False
    rest = q[m.end():].strip()
    return not rest or rest.startswith('*') or rest.startswith('[')


def ptr_ptr_kind(types, q):
    """'data'/'func' for T ** types (the pointed-to pointer's kind), else None."""
    d = types.resolve({'qualType': q})
    if re.search(r'\(\*\s*((const|volatile)\s*)*\*', d):
        return 'func'
    if PTR_PTR.search(d.strip()):
        return 'data'
    return None


def split_ptr_ptr(text):
    """'const u16* const*' -> ('const u16*', 'const'), or None."""
    t = ' '.join(text.split())
    if not t.endswith('*'):
        return None
    inner = t[:-1].rstrip()
    m = re.match(r'^(.*\*)\s*((?:(?:const|volatile)\s*)*)$', inner)
    if not m:
        return None
    return m.group(1).strip(), m.group(2).strip()


def cast_type_span(cast):
    """(file, start, end) of the type name inside a C cast's parentheses."""
    sp = gr.span_of(cast)
    op = gr.span_of(cast['inner'][0]) if cast.get('inner') else None
    if not sp or not op or sp[0] != op[0]:
        return None
    f, s, _ = sp
    between = gr.text_of(f, s, op[1])
    close = between.rfind(')')
    if not between.startswith('(') or close <= 0:
        return None
    return f, s + 1, s + close


def deref_base(node):
    """For *P or P[i] with a pointer-typed result, the stripped pointer operand P."""
    k = node.get('kind')
    if k == 'UnaryOperator' and node.get('opcode') == '*':
        return node['inner'][0]
    if k == 'ArraySubscriptExpr':
        a, b = node['inner'][0], node['inner'][1]
        return a if '*' in qual(a) or '[' in qual(a) else b
    return None


def pointer_root(p):
    """The local DeclRefExpr or cast at the root of P, through parens, ++/-- and P +/- n."""
    while True:
        p = strip(p)
        k = p.get('kind')
        if k == 'UnaryOperator' and p.get('opcode') in ('++', '--'):
            p = p['inner'][0]
        elif k == 'BinaryOperator' and p.get('opcode') in ('+', '-'):
            a, b = p['inner'][0], p['inner'][1]
            p = a if '*' in qual(a) else b
        else:
            return p


def unit(item):
    r, entry = item
    types = gr.Types()
    edits = gr.Edits()
    manual, info = [], []
    nodes = list(ca.walk(ca.dump_ast(entry)))
    parents_of = {}
    locals_ = {}  # id -> (node, parents)
    for node, parents in nodes:
        parents_of[id(node)] = parents
        if node.get('kind') == 'TypedefDecl':
            types.typedefs[node.get('name')] = node.get('type')
        elif node.get('kind') in ('VarDecl', 'ParmVarDecl') and parents and \
                parents[-1].get('kind') != 'TranslationUnitDecl':
            if ptr_ptr_kind(types, qual(node)) and node.get('storageClass') not in ('extern', 'static'):
                locals_[node['id']] = (node, parents)

    def where(n):
        l = n.get('_loc') or n.get('_begin')
        return '%s:%s' % (ca.rel(l.file) if l else '?', l.line if l else '?')

    def ref_id(n):
        n = strip(n)
        if n.get('kind') == 'DeclRefExpr':
            return (n.get('referencedDecl') or {}).get('id')
        return None

    # Sources assigned to each local.
    sources = {}  # local id -> list of RHS nodes
    for node, parents in nodes:
        k = node.get('kind')
        if k == 'BinaryOperator' and node.get('opcode') == '=':
            lid = ref_id(node['inner'][0])
            if lid in locals_:
                sources.setdefault(lid, []).append(node['inner'][1])
        elif k == 'VarDecl' and node['id'] in locals_ and node.get('init') and node.get('inner'):
            sources.setdefault(node['id'], []).append(node['inner'][-1])

    derived = {}  # local id -> (kind, typeof source text or None)
    for lid, rhss in sources.items():
        node, _ = locals_[lid]
        gba_src, from_gba = None, False
        for rhs in rhss:
            s = strip(rhs)
            if gba_union_kind(qual(s)):
                from_gba = True
                sp = gr.span_of(s)
                ok = s.get('kind') == 'DeclRefExpr' or (
                    s.get('kind') == 'UnaryOperator' and s.get('opcode') == '&' and strip(s['inner'][0]).get('kind') == 'DeclRefExpr'
                    and strip(s['inner'][0]).get('referencedDecl', {}).get('id') not in locals_)
                if sp and ok:
                    text = gr.text_of(*sp)
                    src = '&(%s)[0]' % text if qual(s).rstrip().endswith(']') else text
                    if gba_src not in (None, src):
                        manual.append((where(node), node.get('name'), 'set from two different tables'))
                    gba_src = src
            elif s.get('kind') == 'CStyleCastExpr':
                op = strip(s['inner'][0])
                if op.get('kind') == 'UnaryOperator' and op.get('opcode') == '&':
                    continue  # address of something native
                if not ptr_ptr_kind(types, qual(op)) or ref_id(op) not in locals_:
                    from_gba = True  # an address computed as an integer or byte pointer
        if from_gba:
            derived[lid] = (ptr_ptr_kind(types, qual(node)), gba_src)

    # Declarations of derived locals.
    for lid, (kind, src) in derived.items():
        node, parents = locals_[lid]
        name = node.get('name')
        if node.get('kind') == 'ParmVarDecl':
            manual.append((where(node), name, 'parameter points into GBA memory'))
            continue
        sp = gr.span_of(node)
        nl = node['_loc'].editable() if node['_loc'] else None
        if not sp or not nl:
            manual.append((where(node), name, 'declaration inside a macro'))
            continue
        f, s, e = sp
        before = gr.text_of(f, s, nl[1])
        if ',' in before or '(' in before:
            manual.append((where(node), name, 'declaration needs hand editing'))
            continue
        words = before.split()
        storage = []
        while words and words[0] in ('register', 'auto'):
            storage.append(words.pop(0))
        ttext = ' '.join(words)
        if src:
            new = 'GBA_TYPEOF(%s, %s)' % (ttext, src)
        else:
            parts = split_ptr_ptr(ttext)
            if not parts or kind == 'func':
                manual.append((where(node), name, 'cannot split the type %s' % ttext))
                continue
            macro = 'GBA_PTR'
            new = '%s(%s)%s *' % (macro, parts[0], ' ' + parts[1] if parts[1] else '')
        edits.replace(f, s, nl[1], ' '.join(storage + [new]) + ' ')

    # Casts assigned to derived locals, and dereferences.
    for node, parents in nodes:
        k = node.get('kind')
        if k == 'BinaryOperator' and node.get('opcode') == '=':
            lid = ref_id(node['inner'][0])
            if lid in derived:
                c = strip(node['inner'][1])
                if c.get('kind') == 'CStyleCastExpr' and ptr_ptr_kind(types, qual(c)):
                    ts = cast_type_span(c)
                    if not ts:
                        manual.append((where(node), locals_[lid][0].get('name'), 'cast inside a macro'))
                    else:
                        tt = gr.text_of(*ts)
                        edits.replace(ts[0], ts[1], ts[2], 'GBA_TYPEOF(%s, %s)' % (tt.strip(), locals_[lid][0].get('name')))
            continue
        base = deref_base(node)
        if base is None or not gr.Types.kind(types, node.get('type')) or qual(node).rstrip().endswith(']'):
            continue
        root = pointer_root(base)
        rid = ref_id(root) if root.get('kind') == 'DeclRefExpr' else None
        if rid in derived:
            kind = derived[rid][0]
            gr.convert_value(edits, manual, where(node), locals_[rid][0].get('name'), kind, node, parents)
        elif rid in locals_:
            info.append((where(node), locals_[rid][0].get('name'), 'dereferenced, never set from GBA memory'))
        elif root.get('kind') == 'CStyleCastExpr' and ptr_ptr_kind(types, qual(root)):
            op = strip(root['inner'][0])
            if op.get('kind') == 'UnaryOperator' and op.get('opcode') == '&':
                info.append((where(node), qual(root), 'cast of an address, left alone'))
                continue
            kind = ptr_ptr_kind(types, qual(root))
            ts = cast_type_span(root)
            parts = split_ptr_ptr(gr.text_of(*ts)) if ts else None
            if not parts or kind == 'func':
                manual.append((where(node), qual(root), 'cast needs hand editing'))
                continue
            edits.replace(ts[0], ts[1], ts[2], 'GBA_PTR(%s)%s *' % (parts[0], ' ' + parts[1] if parts[1] else ''))
            gr.convert_value(edits, manual, where(node), qual(root), kind, node, parents)
    return edits.items, manual, info


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build-dir', default='build_pc')
    ap.add_argument('--dry-run', action='store_true')
    ap.add_argument('--info', action='store_true', help='also list derefs left alone')
    a = ap.parse_args()
    all_edits, manual, info = set(), set(), set()
    with ProcessPoolExecutor() as ex:
        for e, m, i in ex.map(unit, ca.game_commands(a.build_dir)):
            all_edits |= e
            manual |= set(m)
            info |= set(i)
    files = gr.apply_edits(all_edits, a.dry_run)
    print('%d edits in %d files%s' % (len(all_edits), len(files), ' (dry run)' if a.dry_run else ''))
    for m in sorted(manual):
        print('MANUAL %s  %s  %s' % m)
    if a.info:
        for m in sorted(info):
            print('INFO %s  %s  %s' % m)


if __name__ == '__main__':
    main()

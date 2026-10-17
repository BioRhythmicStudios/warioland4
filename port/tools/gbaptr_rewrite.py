"""Converts pointers held in GBA memory to the GBA_PTR macros of include/gba/gba_ptr.h.

usage: python port/tools/gbaptr_rewrite.py [--build-dir build_pc] [--only REGEX] [--dry-run]

It reads the JSON AST of every game source in the PC build (compile_commands.json) and finds:
- pointer fields of structs and unions declared in src/ and include/,
- file-scope pointer variables and pointer tables, except tables the PC build defines itself
  (native function tables like sPrimarySpriteAITable),
and rewrites their declarations to GBA_PTR()/GBA_FUNC_PTR() and their uses to GP()/GP_SET()
and friends. On the GBA those macros expand to the original code, so the ROM is unchanged.

--only limits the conversion to targets whose key matches REGEX. Keys look like
'F include/sprite.h Struct.field' for fields and 'V gName' for variables.

Sites it cannot convert safely (address-of a converted pointer, a table that decays to a
pointer, uses inside macro bodies, declarators it cannot parse) are listed for hand editing.
Run it on code that has not been converted yet: converted declarations are no longer pointer
types, so a second run leaves them alone.
"""
import argparse
import os
import re
import sys
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, os.path.dirname(__file__))
import clang_ast as ca  # noqa: E402

FUNC_PTR_RE = re.compile(r'\(\*[^()]*\)\s*\(')
NATIVE_TABLES = 'native'


def strip_dims(q):
    q = q.strip()
    while q.endswith(']'):
        q = q[:q.rindex('[')].rstrip()
    return q


class Types:
    def __init__(self):
        self.typedefs = {}

    def resolve(self, t):
        """Desugared spelling of a type dict, with typedef names of the base expanded."""
        q = t.get('desugaredQualType') or t.get('qualType')
        for _ in range(8):
            base = re.sub(r'\b(const|volatile)\b', '', strip_dims(q)).strip()
            if base in self.typedefs:
                sub = self.typedefs[base]
                q = sub.get('desugaredQualType') or sub.get('qualType')
            else:
                break
        return q

    def kind(self, t):
        """'data', 'func' or None for a field or variable type (arrays look at the element)."""
        if not t:
            return None
        q = strip_dims(self.resolve(t))
        if FUNC_PTR_RE.search(q):
            return 'func'
        if re.search(r'\*\s*((const|volatile)\s*)*$', q):
            return 'data'
        return None


def in_project(path):
    r = ca.rel(path) if path else None
    return r if r and (r.startswith('src/') or r.startswith('include/')) else None


def field_key(f, record, name):
    return 'F %s %s.%s' % (f, record, name)


def scan_targets(item):
    """Pass 1: pointer fields and file-scope pointer variables seen in one translation unit."""
    _, entry = item
    types = Types()
    found = {}
    defined = set()
    for node, parents in ca.walk(ca.dump_ast(entry)):
        k = node.get('kind')
        if k == 'TypedefDecl':
            types.typedefs[node.get('name')] = node.get('type')
        elif k == 'FieldDecl':
            # The t_/f_ members of a GBA_PTR/GBA_FUNC_PTR union are already converted.
            if node.get('name') in ('t_', 'f_') and not parents[-1].get('name'):
                continue
            kind = types.kind(node.get('type'))
            f = in_project(node['_loc'].file) if node['_loc'] else None
            if kind and f:
                rec = parents[-1].get('name') or '<anon@%d>' % node['_loc'].line
                found[field_key(f, rec, node.get('name'))] = kind
        elif k == 'VarDecl' and parents and parents[-1].get('kind') == 'TranslationUnitDecl':
            kind = types.kind(node.get('type'))
            f = in_project(node['_loc'].file) if node['_loc'] else None
            if kind and f:
                found['V ' + node.get('name')] = kind
                if node.get('init'):
                    defined.add('V ' + node.get('name'))
    return found, defined


class Edits:
    """Insertions and replacements per file, applied together."""

    def __init__(self):
        self.items = set()  # (file, start, end, text, order)

    def insert_open(self, file, pos, text, span):
        # Openers at one position: the outermost (longest span) first.
        self.items.add((file, pos, pos, text, (1, -span)))

    def insert_close(self, file, pos, text, span):
        # Closers at one position come before openers, innermost first.
        self.items.add((file, pos, pos, text, (0, span)))

    def replace(self, file, start, end, text):
        self.items.add((file, start, end, text, (2, 0)))


def apply_edits(items, dry_run):
    by_file = {}
    for it in items:
        by_file.setdefault(it[0], []).append(it[1:])
    for f, edits in sorted(by_file.items()):
        path = os.path.join(ca.ROOT, f)
        with open(path, 'rb') as fh:
            data = fh.read()
        # Sort by position; at one position, zero-width inserts go before a replacement.
        edits.sort(key=lambda e: (e[0], e[3], e[1]))
        out = []
        pos = 0
        for start, end, text, _ in edits:
            if start < pos:
                raise RuntimeError('overlapping edits in %s at %d' % (f, start))
            out.append(data[pos:start])
            out.append(text.encode())
            pos = end
        out.append(data[pos:])
        if not dry_run:
            with open(path, 'wb') as fh:
                fh.write(b''.join(out))
    return sorted(by_file)


def text_of(f, start, end, cache={}):
    if f not in cache:
        with open(os.path.join(ca.ROOT, f), 'rb') as fh:
            cache[f] = fh.read()
    return cache[f][start:end].decode()


def skip_parens(node, parents):
    """The node and parent chain above any ParenExpr wrapping it."""
    i = len(parents)
    cur = node
    while i > 0 and parents[i - 1].get('kind') == 'ParenExpr':
        i -= 1
        cur = parents[i]
    return cur, parents[:i]


def span_of(node):
    """(file, start, end) of a node's text, or None when it is not editable as a whole."""
    b, e = node.get('_begin'), node.get('_end')
    if not b or not e:
        return None
    be, ee = b.editable(), e.editable()
    if not be or not ee or be[0] != ee[0]:
        return None
    f = ca.rel(be[0])
    if not f:
        return None
    return f, be[1], ee[1] + ee[2]


def rewrite_unit(args):
    item, targets = args
    r, entry = item
    types = Types()
    edits = Edits()
    manual = []
    decls = []
    ids = {}  # decl id -> (key, kind)

    def where(node):
        l = node.get('_loc') or node.get('_begin')
        return '%s:%s' % (ca.rel(l.file) if l else '?', l.line if l else '?')

    def target_value(node, parents):
        """The expression holding the pointer value itself, climbing array subscripts."""
        cur, ps = node, parents
        while True:
            q = strip_dims((cur.get('type') or {}).get('qualType', ''))
            full = (cur.get('type') or {}).get('qualType', '')
            if not full.rstrip().endswith(']'):
                return cur, ps
            # An array: it must decay and be subscripted.
            if not ps or ps[-1].get('kind') != 'ImplicitCastExpr' or ps[-1].get('castKind') != 'ArrayToPointerDecay':
                return None, 'array %s used without a subscript' % q
            dec = ps[-1]
            if len(ps) < 2 or ps[-2].get('kind') != 'ArraySubscriptExpr' or ps[-2]['inner'][0] is not dec:
                return None, 'array decays to a pointer'
            cur, ps = ps[-2], ps[:-2]

    # Locations are filled in during the walk, so finish it before looking at siblings.
    for node, parents in list(ca.walk(ca.dump_ast(entry))):
        k = node.get('kind')
        if k == 'TypedefDecl':
            types.typedefs[node.get('name')] = node.get('type')
            continue
        if k == 'FieldDecl' or (k == 'VarDecl' and parents and parents[-1].get('kind') == 'TranslationUnitDecl'):
            f = in_project(node['_loc'].file) if node['_loc'] else None
            if not f or not types.kind(node.get('type')):
                continue
            if k == 'FieldDecl':
                rec = parents[-1].get('name') or '<anon@%d>' % node['_loc'].line
                key = field_key(f, rec, node.get('name'))
            else:
                key = 'V ' + node.get('name')
            if key not in targets:
                continue
            ids[node['id']] = (key, targets[key])
            decls.append((key, targets[key], node))
            continue

        if k == 'MemberExpr':
            ref = ids.get(node.get('referencedMemberDecl'))
        elif k == 'DeclRefExpr':
            ref = ids.get((node.get('referencedDecl') or {}).get('id'))
        else:
            continue
        if not ref:
            continue
        key, kind = ref
        val, ps = target_value(node, parents)
        if val is None:
            manual.append((where(node), key, ps))
            continue
        convert_value(edits, manual, where(node), key, kind, val, ps)

    decl_edits = []
    for key, kind, node in decls:
        res = rewrite_decl(node, kind, types)
        if isinstance(res, str):
            manual.append((where(node), key, res))
        else:
            decl_edits.append(res)
    return r, edits.items, decl_edits, manual


def convert_value(edits, manual, where, key, kind, val, ps):
    """Edits for one use of a converted pointer: val is the expression holding the
    pointer (ps its parents). Reads become GP(), writes GP_SET() and so on."""
    outer, ops = skip_parens(val, ps)
    parent = ops[-1] if ops else {}
    pk = parent.get('kind')
    get, set_ = ('GF', 'GF_SET') if kind == 'func' else ('GP', 'GP_SET')
    sp = span_of(val)
    if sp is None:
        manual.append((where, key, 'inside a macro body'))
        return
    f, s, e = sp
    if pk == 'ImplicitCastExpr' and parent.get('castKind') == 'LValueToRValue':
        edits.insert_open(f, s, get + '(', e - s)
        edits.insert_close(f, e, ')', e - s)
    elif pk in ('BinaryOperator', 'CompoundAssignOperator') and parent['inner'][0] is outer \
            and parent.get('opcode') in ('=', '+=', '-='):
        if kind == 'func' and parent['opcode'] != '=':
            manual.append((where, key, 'arithmetic on a function pointer'))
            return
        macro = {'=': set_, '+=': 'GP_ADD', '-=': 'GP_SUB'}[parent['opcode']]
        osp, vsp, psp = span_of(outer), span_of(parent['inner'][1]), span_of(parent)
        if not osp or not vsp or not psp or osp[0] != vsp[0]:
            manual.append((where, key, 'assignment inside a macro body'))
            return
        gp = ops[-2] if len(ops) >= 2 else {}
        if gp.get('kind') not in ('CompoundStmt', 'ForStmt', 'IfStmt', 'WhileStmt', 'DoStmt', 'LabelStmt',
                                  'CaseStmt', 'DefaultStmt') and not (gp.get('kind') == 'BinaryOperator' and gp.get('opcode') == ','):
            manual.append((where, key, 'value of an assignment is used (%s)' % gp.get('kind')))
        edits.insert_open(f, osp[1], macro + '(', psp[2] - psp[1])
        edits.replace(f, osp[2], vsp[1], ', ')
        edits.insert_close(f, vsp[2], ')', psp[2] - psp[1])
    elif pk == 'UnaryOperator' and parent.get('opcode') in ('++', '--'):
        if kind == 'func':
            manual.append((where, key, 'arithmetic on a function pointer'))
            return
        psp, osp = span_of(parent), span_of(outer)
        if not psp or not osp:
            manual.append((where, key, 'increment inside a macro body'))
            return
        name = ('GP_POST' if parent.get('isPostfix') else 'GP_PRE') + ('INC' if parent['opcode'] == '++' else 'DEC')
        if parent.get('isPostfix'):
            edits.insert_open(f, osp[1], name + '(', psp[2] - psp[1])
            edits.replace(f, osp[2], psp[2], ')')
        else:
            edits.replace(f, psp[1], osp[1], name + '(')
            edits.insert_close(f, osp[2], ')', psp[2] - psp[1])
    elif pk == 'UnaryExprOrTypeTraitExpr':
        pass  # sizeof: the size is the same 4 bytes
    else:
        manual.append((where, key, 'used as %s %s' % (pk, parent.get('opcode') or parent.get('castKind') or '')))


STORAGE = ('extern', 'static')


def rewrite_decl(node, kind, types):
    """A replacement (file, start, end, text) for a declaration, or a reason string."""
    sp = span_of(node)
    nl = node['_loc'].editable() if node['_loc'] else None
    if not sp or not nl:
        return 'declaration inside a macro'
    f, s, e = sp
    text = text_of(f, s, e)
    name = node.get('name')
    noff = nl[1] - s
    if text[noff:noff + len(name)] != name:
        return 'cannot find the declarator name'
    before, after = text[:noff], text[noff + len(name):]
    if '(' in before or ',' in before or '(' in after:
        return 'declarator needs hand editing: %s' % ' '.join(text.split())
    rest = text_of(f, e, e + 200).lstrip()
    if not rest.startswith(';') and not rest.startswith('='):
        return 'shares its declaration with other declarators'
    storage = []
    toks = before.split()
    while toks and toks[0] in STORAGE:
        storage.append(toks.pop(0))
    body = ' '.join(toks)
    quals = []
    m = re.match(r'^(.*\*)\s*((?:(?:const|volatile)\s*)*)$', body, re.S)
    if m:
        ptype = m.group(1).strip()
        quals = m.group(2).split()
    else:
        # A pointer typedef: qualifiers anywhere in the specifiers apply to the pointer.
        words = body.split()
        quals = [w for w in words if w in ('const', 'volatile')]
        rest_words = [w for w in words if w not in ('const', 'volatile')]
        if len(rest_words) != 1:
            return 'cannot parse the type: %s' % body
        ptype = rest_words[0]
    macro = 'GBA_FUNC_PTR' if kind == 'func' else 'GBA_PTR'
    if kind == 'func' and '*' in ptype:
        return 'function pointer needs a typedef: %s' % ' '.join(text.split())
    out = ' '.join(storage + ['%s(%s)' % (macro, ptype)] + quals + [name]) + after
    return f, s, e, out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build-dir', default='build_pc')
    ap.add_argument('--only', default='')
    ap.add_argument('--exclude', default='')
    ap.add_argument('--dry-run', action='store_true')
    ap.add_argument('--list', action='store_true', help='list the targets and stop')
    a = ap.parse_args()

    items = ca.game_commands(a.build_dir)
    targets, defined = {}, set()
    with ProcessPoolExecutor() as ex:
        for found, d in ex.map(scan_targets, items):
            targets.update(found)
            defined |= d
    for k in defined:
        targets.pop(k, None)
    if a.only:
        targets = {k: v for k, v in targets.items() if re.search(a.only, k)}
    if a.exclude:
        targets = {k: v for k, v in targets.items() if not re.search(a.exclude, k)}
    if a.list:
        for k in sorted(targets):
            print(targets[k], k)
        print('%d targets; natively defined, left alone: %s' % (len(targets), ' '.join(sorted(defined))))
        return

    all_edits = set()
    decl_edits = set()
    manual = set()
    with ProcessPoolExecutor() as ex:
        for r, items_, dedits, man in ex.map(rewrite_unit, [(it, targets) for it in items]):
            all_edits |= items_
            decl_edits |= {(f, s, e, text, (2, 0)) for f, s, e, text in dedits}
            manual |= set(man)
    files = apply_edits(all_edits | decl_edits, a.dry_run)
    print('%d targets, %d use edits, %d declarations, %d files%s' % (
        len(targets), len(all_edits), len(decl_edits), len(files), ' (dry run)' if a.dry_run else ''))
    for m in sorted(manual):
        print('MANUAL %s  %s  %s' % m)


if __name__ == '__main__':
    main()

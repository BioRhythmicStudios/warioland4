"""Finds functions that are declared or called with a different signature from their definition.

agbcc passes and returns every integer in a full 32-bit register, so on the GBA a caller that
thinks `u8 f(void)` returns `s32`, or that passes a `u16` where the definition takes a `u32`,
still works. On x86-64 and arm64 the upper bits of narrow values are not guaranteed, so those
mismatches read garbage.

The check compiles every game source of the PC build to LLVM IR at -O0 and compares, for each
function, the definition's return and parameter types (with their zeroext/signext attributes)
against every `declare` and every direct call, including calls through a cast function pointer.

Usage: python port/tools/check_signatures.py [BUILD_DIR] [-j N] [--keep DIR]
Exits 1 if any mismatch is found.
"""
import argparse
import os
import re
import shlex
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(__file__))
from clang_ast import ROOT, game_commands  # noqa: E402

ATTRS_WITH_ARG = re.compile(r'^(align|dereferenceable|dereferenceable_or_null|byval|sret|elementtype|'
                            r'nofpclass|range|initializes|captures|memory)\b')
DEFINE_RE = re.compile(r'^(define|declare)\s+(.*?)@("?[\w.$]+"?)\((.*)\)(.*)$')
CALL_RE = re.compile(r'\b(?:tail |musttail |notail )?call\s+(.*?)@("?[\w.$]+"?)\((.*)\)')


def ir_command(entry, out, opt):
    args = shlex.split(entry['command'], posix=False) if 'command' in entry else list(entry['arguments'])
    args = [a.strip('"') for a in args]
    keep = []
    skip = False
    for a in args:
        if skip:
            skip = False
            continue
        if a in ('-o', '-MT', '-MQ', '-MF'):
            skip = True
            continue
        if a in ('-c', '-MD', '-MMD') or a.startswith('-MF') or a.startswith('-O'):
            continue
        keep.append(a)
    if opt == '-O0':
        keep += ['-O0', '-Xclang', '-disable-O0-optnone']
    else:
        keep += [opt, '-fno-inline']
    return keep + ['-S', '-emit-llvm', '-g0', '-w', '-o', out]


def split_top(s):
    parts, depth, cur = [], 0, ''
    for ch in s:
        if ch in '([{<':
            depth += 1
        elif ch in ')]}>':
            depth -= 1
        if ch == ',' and depth == 0:
            parts.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        parts.append(cur.strip())
    return parts


def strip_attr_args(tokens):
    out = []
    for t in tokens:
        if t.startswith('#') or t.startswith('!'):
            continue
        out.append(t)
    return out


def norm_type(text, has_value):
    """'i8 noundef zeroext %3' -> 'i8 zeroext'. Pointers, structs and floats keep their type."""
    text = re.sub(r'\([^()]*\)', '', text)  # attribute arguments like align(4)
    tokens = text.split()
    if has_value and len(tokens) > 1:
        tokens = tokens[:-1]
    if not tokens:
        return '?'
    ty = tokens[0]
    if ty == '...':
        return '...'
    ext = [t for t in tokens[1:] if t in ('zeroext', 'signext')]
    return ' '.join([ty] + ext)


def norm_ret(prefix):
    prefix = re.sub(r'\([^()]*\)', '', prefix)
    tokens = [t for t in prefix.split() if t not in (
        'dso_local', 'dllimport', 'dllexport', 'internal', 'private', 'linkonce_odr', 'weak',
        'extern_weak', 'available_externally', 'hidden', 'protected', 'noundef', 'nonnull',
        'noalias', 'fastcc', 'ccc', 'local_unnamed_addr', 'unnamed_addr', 'inreg', 'nnan')]
    ext = [t for t in tokens if t in ('zeroext', 'signext')]
    tys = [t for t in tokens if t not in ('zeroext', 'signext')]
    return ' '.join(tys[-1:] + ext) if tys else '?'


def signature(ret, params, has_values):
    return (norm_ret(ret), tuple(norm_type(p, has_values) for p in split_top(params)))


def width(ty):
    m = re.match(r'i(\d+)', ty)
    return int(m.group(1)) if m else None


def compatible(defn, use, param_bits, ret_bits):
    """True when calling `defn` as `use` cannot read undefined bits on a 64-bit ABI.

    param_bits[i] is how many low bits of parameter i the definition actually uses, and
    ret_bits how many low bits of the result the caller uses (both from the optimised IR).
    """
    dret, dparams = defn
    uret, uparams = use
    if not ret_ok(dret, uret, ret_bits):
        return False
    if '...' in uparams or '...' in dparams:
        return True
    for i, d in enumerate(dparams):
        used = param_bits[i] if param_bits and i < len(param_bits) else width(d.split()[0])
        if i >= len(uparams):
            # The caller passes nothing: fine only if the definition ignores the parameter.
            if used != 0:
                return False
            continue
        if not param_ok(d, uparams[i], used):
            return False
    return True


def ret_ok(d, u, used):
    if u.startswith('void') or d == u or used == 0:
        return True
    uw = width(u.split()[0])
    if d.startswith('void'):
        return False
    dw = width(d.split()[0])
    if dw is None or uw is None:
        # A pointer read as a 32-bit integer is a GBA address: the port keeps the block at base 0.
        return d.split()[0] == u.split()[0] or (d.startswith('ptr') and uw == 32)
    # The caller reads `used` (or all uw) bits. They are defined if the callee produced them.
    return (used if used is not None else uw) <= dw


def param_ok(d, u, used):
    if d == u or used == 0:
        return True
    dw, uw = width(d.split()[0]), width(u.split()[0])
    if dw is None or uw is None:
        return d.split()[0] == u.split()[0]
    # The callee reads `used` bits. They are defined if the caller passed at least that many.
    return (used if used is not None else dw) <= uw


USE_TRUNC = re.compile(r'= trunc (?:nuw |nsw )*i(\d+) %s to i(\d+)')
USE_AND = re.compile(r'= and i(\d+) (?:%s, (\d+)|(\d+), %s)')
USE_SHL = re.compile(r'= shl (?:nuw |nsw )*i(\d+) %s, (\d+)')


def used_bits(body, var, full):
    """How many low bits of SSA value `var` the function body depends on (None if unknown)."""
    ref = re.compile(re.escape(var) + r'(?![\w.])')
    bits = 0
    for line in body:
        if not ref.search(line):
            continue
        if re.match(r'\s*' + re.escape(var) + r'\s*=', line):
            continue
        v = re.escape(var)
        m = re.search(USE_TRUNC.pattern % v, line)
        if m:
            bits = max(bits, int(m.group(2)))
            continue
        m = re.search(USE_AND.pattern % (v, v), line)
        if m:
            mask = int(m.group(2) or m.group(3))
            bits = max(bits, mask.bit_length() if mask >= 0 else full)
            continue
        m = re.search(USE_SHL.pattern % v, line)
        if m:
            bits = max(bits, int(m.group(1)) - int(m.group(2)))
            continue
        return full
    return bits


def parse_opt_ir(path, rel):
    """From optimised IR: per defined function, the used bits of each parameter, and per
    (caller unit, callee) the most result bits any call there uses."""
    param_bits, ret_bits = {}, {}
    with open(path, encoding='utf-8', errors='replace') as f:
        lines = f.read().split('\n')
    i = 0
    while i < len(lines):
        m = DEFINE_RE.match(lines[i])
        if not m or m.group(1) != 'define':
            i += 1
            continue
        _, ret, name, params, _ = m.groups()
        j = i + 1
        while j < len(lines) and lines[j] != '}':
            j += 1
        body = lines[i + 1:j]
        bits = []
        for p in split_top(params):
            toks = re.sub(r'\([^()]*\)', '', p).split()
            var = toks[-1] if toks and toks[-1].startswith('%') else None
            w = width(toks[0]) if toks else None
            bits.append(used_bits(body, var, w) if var and w else w)
        if 'internal' not in ret.split() and 'private' not in ret.split():
            param_bits[name] = bits
        for line in body:
            cm = re.match(r'\s*(%[\w.]+) = (?:tail |notail |musttail )?call\s+(.*?)@("?[\w.$]+"?)\(', line)
            if cm:
                var, cret, callee = cm.groups()
                w = width(norm_ret(cret).split()[0])
                b = used_bits(body, var, w) if w else None
                key = (rel, callee)
                prev = ret_bits.get(key, 0)
                ret_bits[key] = None if b is None or prev is None else max(prev, b)
                continue
            cm = re.match(r'\s*(?:tail |notail |musttail )?call\s+(.*?)@("?[\w.$]+"?)\(', line)
            if cm:
                ret_bits.setdefault((rel, cm.group(2)), 0)
        i = j + 1
    return param_bits, ret_bits


def parse_ir(path, rel):
    defs, uses = {}, []
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            m = DEFINE_RE.match(line)
            if m:
                kind, ret, name, params, _ = m.groups()
                if 'internal' in ret.split() or 'private' in ret.split():
                    continue
                sig = signature(ret, params, has_values=(kind == 'define'))
                if kind == 'define':
                    defs[name] = (sig, rel)
                else:
                    uses.append((name, sig, rel, 'declare'))
                continue
            m = CALL_RE.search(line)
            if m:
                ret, name, params = m.groups()
                if name.startswith('llvm.'):
                    continue
                uses.append((name, signature(ret, params, has_values=True), rel, 'call'))
    return defs, uses


def fmt(sig):
    ret, params = sig
    return '%s (%s)' % (ret, ', '.join(params))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('build_dir', nargs='?', default='build_pc')
    ap.add_argument('-j', type=int, default=os.cpu_count() or 4)
    ap.add_argument('--keep', help='write the .ll files here instead of a temporary directory')
    args = ap.parse_args()

    units = game_commands(args.build_dir)
    outdir = args.keep or tempfile.mkdtemp(prefix='wl4sig')
    os.makedirs(outdir, exist_ok=True)

    def emit(item):
        rel, entry = item
        outs = []
        for opt in ('-O0', '-O2'):
            out = os.path.join(outdir, rel.replace('/', '__') + opt + '.ll')
            r = subprocess.run(ir_command(entry, out, opt), cwd=entry.get('directory', ROOT),
                               capture_output=True, text=True)
            if r.returncode:
                sys.stderr.write('%s: clang failed\n%s' % (rel, r.stderr))
                return None
            outs.append(out)
        return rel, outs

    with ThreadPoolExecutor(args.j) as ex:
        results = [r for r in ex.map(emit, units) if r]

    defs, uses, param_bits, ret_bits = {}, [], {}, {}
    for rel, (o0, o2) in results:
        d, u = parse_ir(o0, rel)
        defs.update(d)
        uses.extend(u)
        p, r = parse_opt_ir(o2, rel)
        param_bits.update(p)
        ret_bits.update(r)

    problems = {}
    for name, sig, rel, kind in uses:
        if name not in defs:
            continue
        dsig, drel = defs[name]
        rb = ret_bits.get((rel, name), 0 if kind == 'declare' else None)
        if not compatible(dsig, sig, param_bits.get(name), rb):
            problems.setdefault((name, drel, dsig), set()).add((rel, kind, sig))

    for (name, drel, dsig), sites in sorted(problems.items()):
        print('%s  defined in %s as %s' % (name.strip('"'), drel, fmt(dsig)))
        for rel, kind, sig in sorted(sites):
            print('    %-7s %s: %s' % (kind, rel, fmt(sig)))
    print('%d functions with mismatched uses' % len(problems))
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())

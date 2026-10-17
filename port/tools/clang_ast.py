"""Helpers for reading Clang's JSON AST dump (-Xclang -ast-dump=json) of the PC build.

Clang's JSON omits "file" (and "line") from a location when it equals the previously printed
one, so walk() replays the dump in output order and fills every location in completely.
"""
import json
import os
import shlex
import subprocess

ROOT = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..'))


def norm(path):
    return os.path.normpath(path).replace('\\', '/')


def rel(path):
    """Repo-relative path with forward slashes, or None outside the repo."""
    if not path:
        return None
    p = norm(os.path.abspath(path))
    r = norm(ROOT)
    if p.lower().startswith(r.lower() + '/'):
        return p[len(r) + 1:]
    return None


def compile_commands(build_dir):
    with open(os.path.join(ROOT, build_dir, 'compile_commands.json'), encoding='utf-8') as f:
        return json.load(f)


def game_commands(build_dir):
    out = []
    for e in compile_commands(build_dir):
        r = rel(e['file'])
        if r and r.startswith('src/'):
            out.append((r, e))
    return sorted(out)


def dump_ast(entry):
    """Runs the compile command with the JSON AST dump instead of code generation."""
    args = shlex.split(entry['command'], posix=False) if 'command' in entry else list(entry['arguments'])
    args = [a.strip('"') for a in args]
    keep = []
    skip = False
    for a in args:
        if skip:
            skip = False
            continue
        if a == '-o':
            skip = True
            continue
        if a in ('-c',) or a.startswith('-MF') or a in ('-MD', '-MT', '-MQ'):
            if a in ('-MT', '-MQ'):
                skip = True
            continue
        keep.append(a)
    keep += ['-fsyntax-only', '-Xclang', '-ast-dump=json', '-w']
    res = subprocess.run(keep, cwd=entry['directory'], capture_output=True)
    if not res.stdout:
        raise RuntimeError(res.stderr.decode(errors='replace'))
    return json.loads(res.stdout)


class Loc:
    """A source location. For a macro expansion, file/offset are the expansion location and
    spell_* the spelling location; arg is set when the token came from a macro argument."""
    __slots__ = ('file', 'offset', 'line', 'col', 'toklen', 'macro', 'arg',
                 'spell_file', 'spell_offset', 'spell_toklen')

    def __repr__(self):
        return '%s:%s:%s' % (self.file, self.line, self.col)

    def editable(self):
        """(file, offset, toklen) where text for this token can be edited, or None."""
        if not self.macro:
            return self.file, self.offset, self.toklen
        if self.arg:
            return self.spell_file, self.spell_offset, self.spell_toklen
        return None


def walk(root):
    """Yields (node, parents) in output order; every node gets '_loc', '_begin', '_end' (Loc)."""
    state = {'file': None, 'line': None}

    def one(d):
        if d is None or not d:
            return None
        if 'file' in d:
            state['file'] = d['file']
        if 'line' in d:
            state['line'] = d['line']
        if 'offset' not in d:
            return None
        l = Loc()
        l.file = state['file']
        l.offset = d['offset']
        l.line = state['line']
        l.col = d.get('col')
        l.toklen = d.get('tokLen', 0)
        l.macro = False
        l.arg = bool(d.get('isMacroArgExpansion'))
        l.spell_file = l.file
        l.spell_offset = l.offset
        l.spell_toklen = l.toklen
        return l

    def loc(d):
        if not d:
            return None
        if 'spellingLoc' in d or 'expansionLoc' in d:
            s = one(d.get('spellingLoc'))
            e = one(d.get('expansionLoc'))
            if e is None:
                return s
            e.macro = True
            if s is not None:
                e.spell_file = s.file
                e.spell_offset = s.offset
                e.spell_toklen = s.toklen
            return e
        return one(d)

    stack = [(root, ())]
    while stack:
        node, parents = stack.pop()
        node['_loc'] = loc(node.get('loc'))
        r = node.get('range') or {}
        node['_begin'] = loc(r.get('begin'))
        node['_end'] = loc(r.get('end'))
        yield node, parents
        inner = node.get('inner') or []
        p2 = parents + (node,)
        for child in reversed(inner):
            stack.append((child, p2))

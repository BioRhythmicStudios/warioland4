#!/usr/bin/env python3
"""Per-function regression check against the original objects.

Compares every function in build/us/objdiff/*.o with expected/us/obj/*.o
(the units listed in objdiff.json), ignoring differences that only come from
a function's position inside its object: branch targets, jump-table entries
and decomp.wtf's `.has_asm` symbol suffix. Use this instead of the ROM MD5
while unmatched C is still compiled into the ROM.

Build the objects first (from WSL, in the repo root):
    make -k $(python3 -c "import json;print(' '.join(u['base_path'] for u in json.load(open('objdiff.json'))['units']))")

Usage (needs arm-none-eabi-objdump on PATH, e.g. from WSL):
    python3 tools/objcmp.py                    # list every function that differs
    python3 tools/objcmp.py UNIT FUNCTION      # also show that function's diff
"""
import difflib
import json
import os
import re
import subprocess
import sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
units = json.load(open(os.path.join(root, 'objdiff.json')))['units']


def funcs(path):
    out = subprocess.run(['arm-none-eabi-objdump', '-d', '-r', '--no-show-raw-insn', path],
                         capture_output=True, text=True).stdout
    res = {}
    cur = None
    for line in out.splitlines():
        m = re.match(r'^([0-9a-f]+) <(.+)>:$', line)
        if m:
            cur = m.group(2)
            base = int(m.group(1), 16)
            res[cur] = []
            continue
        if cur and line.strip():
            l = re.sub(r'^\s*[0-9a-f]+:\s*', '', line)
            l = l.split('@')[0]
            mb = re.match(r'^(b\S*)\s+([0-9a-f]+)\s*<', l)
            if mb and mb.group(1) != 'bl':
                l = '%s +%x' % (mb.group(1), int(mb.group(2), 16) - base)
            elif mb:
                l = 'bl'
            l = re.sub(r'<[^>]*>', '', l)
            l = re.sub(r'\s+', ' ', l).strip()
            l = l.replace('.has_asm', '')
            if l == 'R_ARM_ABS32 .text' and res[cur] and res[cur][-1].startswith('.word 0x'):
                res[cur][-1] = '.word text+%x' % (int(res[cur][-1][6:], 16) - base)
            res[cur].append(l)
    return res


bad = []
missing = []
total = 0
for u in units:
    t = os.path.join(root, u['target_path'])
    b = os.path.join(root, u['base_path'])
    if not os.path.exists(t) or not os.path.exists(b):
        missing.append(u['name'])
        continue
    ft = funcs(t)
    fb = funcs(b)
    for name, ins in ft.items():
        if name.startswith('$') or name.startswith('.'):
            continue
        total += 1
        if fb.get(name) != ins:
            bad.append((u['name'], name, 'absent' if name not in fb else 'differs'))

print('units missing objects:', missing)
print('functions compared:', total, 'non-identical:', len(bad))
for x in bad:
    print(' ', *x)

if len(sys.argv) > 2:
    u = next(x for x in units if x['name'] == sys.argv[1])
    a = funcs(os.path.join(root, u['target_path']))[sys.argv[2]]
    b = funcs(os.path.join(root, u['base_path']))[sys.argv[2]]
    print('\n'.join(difflib.unified_diff(a, b, lineterm='', n=1)))

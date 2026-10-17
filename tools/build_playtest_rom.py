"""Build a playtest ROM from the current C without shifting anything else.

Run under WSL from the repo root, after `make -k` has produced build/us/asm and build/us/obj:
    python3 tools/build_playtest_rom.py [--asm UNIT=FILE.s ...] [--out ROM]

--asm replaces a unit's generated assembly with a trial compile (e.g. from a candidate
source), so a candidate can be differential-tested without writing src/.

The normal build is not shiftable: data blobs and some tables hold raw ROM addresses, so
a non-matching function that changes size moves everything after it and breaks unrelated
code. This script keeps every function at its original address:
- a function that compiles smaller than the original is padded to the original size;
- a function that compiles larger is moved to free space after .data, and a stub at the
  original address jumps to it (r0-r3 and lr are preserved).
Only the generated assembly in build/playtest/ is patched; src/ and build/us/ are not written.
The ROM is compared with the base ROM, and every difference must lie inside a listed function.

This script needs only the ARM binutils and tools/gbafix. Its companion,
tools/diff_test_functions.py, which checks the playtest ROM's functions against the base ROM,
needs the unicorn emulator module: pip install unicorn
"""
import argparse
import re
import subprocess as sp
from pathlib import Path

UNMATCHED = {
    'boss_door_opening': ['UpdateBossDoorOpeningSprites'],
    'hblank': ['UpdateEscapeRippleHBlank'],
    'minigames/roulette/draw_roulette': ['DrawRoulette'],
    'minigames/roulette/refresh_roulette_result_tiles': ['RefreshRouletteResultTiles'],
    'minigames/wario_hop': ['WarioHopInit'],
    'sprite_ai/swinging_platform': ['func_804A508', 'func_804A84C'],
    'wario/normal': ['ResolveWarioCeilingCollision'],
}
OUT = Path('build/playtest')
ROM = OUT / 'warioland4_us_playtest.gba'
ASM = {}  # unit -> replacement assembly (--asm)


def run(*cmd, **kw):
    return sp.run(cmd, check=True, capture_output=True, text=True, **kw).stdout


def func_sizes(obj):
    sizes = {}
    for line in run('arm-none-eabi-nm', '-S', str(obj)).splitlines():
        p = line.split()
        if len(p) == 4 and p[2] in 'tT':
            sizes[p[3]] = int(p[1], 16)
    return sizes


def patch(asm, func, delta, orig_size):
    lines = asm.split('\n')
    head = next(i for i, l in enumerate(lines) if re.match(r'\s*\.globl\s+%s$' % func, l))
    start = head - 1 if '.align' in lines[head - 1] else head
    end = next(i for i, l in enumerate(lines) if re.match(r'\s*\.size\s+%s,' % func, l))
    if delta > 0:
        # Smaller than the original: pad after the function.
        lines.insert(end + 1, '\t.space\t%d, 0' % delta)
        return '\n'.join(lines)
    # Larger: move the body to .playtest under a new name, leave a stub of the original size.
    body = [re.sub(r'\b%s\b' % func, func + '_C', l) for l in lines[start:end + 1]]
    stub = [
        '\t.align\t2, 0', '\t.globl\t%s' % func, '\t.type\t %s,function' % func, '\t.thumb_func', '%s:' % func,
        '\tpush\t{r0}', '\tldr\tr0, .Lplaytest_%s' % func, '\tmov\tip, r0', '\tpop\t{r0}', '\tbx\tip',
        '\t.align\t2, 0', '.Lplaytest_%s:' % func, '\t.word\t%s_C' % func,
        '\t.space\t%d - (. - %s), 0' % (orig_size, func), '\t.size\t %s,%d' % (func, orig_size),
    ]
    tail = ['\t.section\t.playtest,"ax",%progbits'] + body + ['\t.text']
    return '\n'.join(lines[:start] + stub + lines[end + 1:] + tail)


def main():
    global ROM
    ap = argparse.ArgumentParser()
    ap.add_argument('--asm', action='append', default=[], metavar='UNIT=FILE.s')
    ap.add_argument('--out', type=Path)
    args = ap.parse_args()
    for a in args.asm:
        unit, path = a.split('=', 1)
        assert unit in UNMATCHED, unit
        ASM[unit] = Path(path)
    if args.out:
        ROM = args.out
    moved = []
    for unit, funcs in UNMATCHED.items():
        exp = func_sizes('expected/us/obj/%s.o' % unit)
        if unit in ASM:
            trial = OUT / 'trial' / (unit + '.o')
            trial.parent.mkdir(parents=True, exist_ok=True)
            run('arm-none-eabi-as', '-Iasm', '-mcpu=arm7tdmi', '--defsym', 'VERSION_US=1', str(ASM[unit]), '-o', str(trial))
            built = func_sizes(trial)
            asm = ASM[unit].read_text() + '    .align 2, 0\n'  # as the Makefile appends
        else:
            built = func_sizes('build/us/obj/%s.o' % unit)
            asm = Path('build/us/asm/%s.s' % unit).read_text()
        changed = unit in ASM
        for f in funcs:
            delta = exp[f] - built[f]
            if delta:
                asm = patch(asm, f, delta, exp[f])
                changed = True
                print('%-30s %+d bytes -> %s' % (f, -delta, 'padded' if delta > 0 else 'moved to .playtest'))
                if delta < 0:
                    moved.append(unit)
            elif unit in ASM:
                print('%-30s same size as the original' % f)
        if changed:
            s = OUT / 'asm' / (unit + '.s')
            o = OUT / 'obj' / (unit + '.o')
            s.parent.mkdir(parents=True, exist_ok=True)
            o.parent.mkdir(parents=True, exist_ok=True)
            s.write_text(asm)
            run('arm-none-eabi-as', '-Iasm', '-mcpu=arm7tdmi', '--defsym', 'VERSION_US=1', str(s), '-o', str(o))

    ld = Path('linker.ld').read_text()
    extra = ''.join('        obj/%s.o(.playtest);\n' % u for u in sorted(set(moved)))
    marker = '    /* Discard everything not specifically mentioned above. */'
    ld = ld.replace(marker, '    .playtest : ALIGN(4) {\n%s    } > ROM\n\n%s' % (extra, marker))
    (OUT / 'linker.ld').write_text(ld)
    elf = ROM.with_suffix('.elf')
    run('arm-none-eabi-ld', '-n', '-T', str(OUT / 'linker.ld'), '-Map=%s' % ROM.with_suffix('.map'),
        '-L' + str(OUT), '-Lbuild/us', 'tools/agbcc/libgcc.a', 'tools/agbcc/libc.a', '-o', str(elf))
    run('arm-none-eabi-objcopy', '-O', 'binary', '--gap-fill', '0xff', '--pad-to', '0x08800000', str(elf), str(ROM))
    run('tools/gbafix/gbafix', str(ROM), '-tWARIOLANDE', '-cAWAE', '-m01', '-r00')

    # Every difference from the base ROM must be inside an unmatched function or .playtest.
    syms = []
    for line in run('arm-none-eabi-nm', '-n', '-S', str(elf)).splitlines():
        p = line.split()
        if len(p) == 4 and p[2] in 'tT':
            syms.append((int(p[0], 16), int(p[1], 16), p[3]))
    allowed = {f for fs in UNMATCHED.values() for f in fs}
    allowed |= {f + '_C' for f in allowed}
    # A padded function's slot runs to its original size, past its .size.
    exp_size = {}
    for unit, funcs in UNMATCHED.items():
        e = func_sizes('expected/us/obj/%s.o' % unit)
        exp_size.update((f, e[f]) for f in funcs)
    syms = [(st, max(sz, exp_size.get(n, 0)), n) for st, sz, n in syms]
    # .playtest also holds the long-branch veneers ld adds for calls out of it.
    hdr = run('arm-none-eabi-objdump', '-h', str(elf)).split()
    pt = hdr.index('.playtest') if '.playtest' in hdr else None
    pt_range = (int(hdr[pt + 2], 16), int(hdr[pt + 2], 16) + int(hdr[pt + 1], 16)) if pt else (0, 0)
    a = Path('baserom_us.gba').read_bytes()
    b = ROM.read_bytes()
    owners, stray = {}, []
    for i in (i for i in range(len(a)) if a[i] != b[i]):
        addr = 0x08000000 + i
        own = next((n for st, sz, n in syms if st <= addr < st + sz), None)
        if own not in allowed and pt_range[0] <= addr < pt_range[1]:
            own = '.playtest veneers'
        if own in allowed or own == '.playtest veneers':
            owners[own] = owners.get(own, 0) + 1
        else:
            stray.append(addr)
    for n, c in sorted(owners.items()):
        print('  %-32s %4d bytes differ' % (n, c))
    if stray:
        print('STRAY DIFFERENCES: %d bytes, first at %s' % (len(stray), ', '.join(hex(x) for x in stray[:8])))
        raise SystemExit(1)
    print('OK: %s differs from the base ROM only inside the unmatched functions' % ROM)


main()

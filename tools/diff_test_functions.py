"""Differential test: does a non-matching function behave like the original?

Runs a function from the base ROM and from the playtest ROM (tools/build_playtest_rom.py)
in an ARM7TDMI emulator, from identical randomized memory, and compares every byte of
memory afterwards (and r0 for functions that return a value). Register choices, spill
slots and code layout don't matter; any difference in loads, stores, calls or results does.
Callees run their real code, which is identical in both ROMs.

Requires the unicorn emulator module: pip install unicorn
(a virtualenv works: python3 -m venv venv && venv/bin/pip install unicorn).
Build the playtest ROM first (tools/build_playtest_rom.py), then run under WSL from the repo root:
    python3 tools/diff_test_functions.py [FUNC ...] [--runs N] [--rom-b ROM]
--rom-b tests another playtest build, e.g. one made with build_playtest_rom.py --asm ... --out ROM;
its .elf must sit next to it.

Hardware model: DMA channels perform their copy when the control register is read back
(the code always reads it straight after enabling) and then read as idle. Other IO reads
return the randomized fill. Memory outside the mapped GBA regions is mapped on demand with
deterministic filler, so random pointers don't crash either run differently.
"""
import argparse
import random
import struct
import subprocess as sp
import sys
from pathlib import Path

from unicorn import (UC_ARCH_ARM, UC_HOOK_CODE, UC_HOOK_INTR, UC_HOOK_MEM_READ, UC_HOOK_MEM_UNMAPPED,
                     UC_HOOK_MEM_WRITE, UC_MODE_THUMB, UC_PROT_ALL, Uc, UcError)
from unicorn.arm_const import UC_ARM_REG_CPSR, UC_ARM_REG_LR, UC_ARM_REG_PC, UC_ARM_REG_R0, UC_ARM_REG_R1, UC_ARM_REG_SP

FUNCS = {
    # name: (returns a value, argument choices)
    'RefreshRouletteResultTiles': (False, None),
    'ResolveWarioCeilingCollision': (True, None),
    'WarioHopInit': (False, None),
    'DrawRoulette': (False, None),
    'UpdateBossDoorOpeningSprites': (False, None),
    'UpdateEscapeRippleHBlank': (False, (0, 1)),
    'func_804A508': (False, None),
    'func_804A84C': (False, None),
}
REGIONS = [  # base, size, compared
    (0x02000000, 0x40000, True),   # EWRAM
    (0x03000000, 0x8000, True),    # IWRAM
    (0x04000000, 0x1000, True),    # IO
    (0x05000000, 0x400, True),     # palette
    (0x06000000, 0x18000, True),   # VRAM
    (0x07000000, 0x400, True),     # OAM
]
SP0 = 0x03007E00
STACK_SCRATCH = (SP0 - 0x1000, SP0)  # locals and spills: not compared
RET = 0x0E000000                     # SRAM area, used as the return trap
MAX_INSNS = 3_000_000


def elf_symbols(elf):
    syms = {}
    for line in sp.run(['arm-none-eabi-nm', '-S', elf], capture_output=True, text=True, check=True).stdout.splitlines():
        p = line.split()
        if len(p) == 4 and p[2] in 'tT':
            syms[p[3]] = (int(p[0], 16), int(p[1], 16))
    return syms


def fill(rng, size, mode):
    if mode == 0:
        return bytes(size)
    if mode == 1:
        return rng.randbytes(size)
    if mode == 2:  # small values, the usual shape of state bytes and counters
        return bytes(rng.choices((0, 0, 0, 1, 1, 2, 3, 4, 0xFF), k=size))
    # sparse: mostly zero with some random and small bytes
    b = bytearray(size)
    for i in rng.sample(range(size), size // 8):
        b[i] = rng.choice((1, 2, 3, rng.randrange(256)))
    return bytes(b)


class Run:
    def __init__(self, rom, seed, mode, func_range, arg):
        self.uc = uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
        self.seed = seed
        uc.mem_map(0x08000000, 0x800000, UC_PROT_ALL)
        uc.mem_write(0x08000000, rom)
        rng = random.Random(seed)
        for base, size, _ in REGIONS:
            uc.mem_map(base, max(size, 0x1000), UC_PROT_ALL)
            uc.mem_write(base, fill(rng, size, mode))
        uc.mem_map(RET, 0x1000, UC_PROT_ALL)
        uc.mem_map(0, 0x4000, UC_PROT_ALL)  # BIOS: filler, read by nothing we test
        self.extra = {}
        self.pending = {}
        self.covered = set()
        self.error = None
        lo, hi = func_range
        uc.hook_add(UC_HOOK_MEM_UNMAPPED, self.on_unmapped)
        uc.hook_add(UC_HOOK_MEM_WRITE, self.on_io_write, begin=0x040000B0, end=0x040000E0)
        uc.hook_add(UC_HOOK_MEM_READ, self.on_io_read, begin=0x040000B0, end=0x040000E0)
        uc.hook_add(UC_HOOK_CODE, self.on_code, begin=lo, end=hi - 1)
        uc.hook_add(UC_HOOK_INTR, self.on_intr)
        uc.reg_write(UC_ARM_REG_SP, SP0)
        uc.reg_write(UC_ARM_REG_LR, RET | 1)
        rr = random.Random(seed ^ 0x5A5A)
        for reg in range(UC_ARM_REG_R0, UC_ARM_REG_R0 + 4):
            uc.reg_write(reg, rr.getrandbits(32))
        if arg is not None:
            uc.reg_write(UC_ARM_REG_R0, arg)

    def on_unmapped(self, uc, access, addr, size, value, data):
        page = addr & ~0xFFF
        if page in self.extra or len(self.extra) > 4096:
            return False
        uc.mem_map(page, 0x1000, UC_PROT_ALL)
        uc.mem_write(page, random.Random((self.seed << 32) ^ page).randbytes(0x1000))
        self.extra[page] = True
        return True

    def dma(self, ch):
        uc = self.uc
        base = 0x040000B0 + 12 * ch
        src, dst, cnt = struct.unpack('<III', uc.mem_read(base, 12))
        count = (cnt & 0xFFFF) or (0x4000 if ch == 3 else 0x4000)
        width = 4 if cnt & (1 << 26) else 2
        dctl, sctl = (cnt >> 21) & 3, (cnt >> 23) & 3
        step = {0: width, 1: -width, 2: 0, 3: width}
        src &= ~(width - 1)
        dst &= ~(width - 1)
        for _ in range(count):
            uc.mem_write(dst, bytes(uc.mem_read(src, width)))
            src += step[sctl]
            dst += step[dctl]
        uc.mem_write(base + 8, struct.pack('<I', cnt & 0x7FFFFFFF))

    def on_io_write(self, uc, access, addr, size, value, data):
        ch, off = divmod(addr - 0x040000B0, 12)
        top = off + size - 1
        if top >= 11 and off <= 11:  # write reaching the enable bit
            enable = (value >> ((11 - off) * 8)) & 0x80
            if enable:
                self.pending[ch] = True

    def on_io_read(self, uc, access, addr, size, value, data):
        ch = (addr - 0x040000B0) // 12
        if self.pending.pop(ch, False):
            self.dma(ch)

    def on_code(self, uc, addr, size, data):
        self.covered.add(addr)

    def on_intr(self, uc, intno, data):
        pc = uc.reg_read(UC_ARM_REG_PC)
        self.error = 'interrupt %d near %#x' % (intno, pc)
        uc.emu_stop()

    def go(self, entry):
        try:
            self.uc.emu_start(entry | 1, RET, count=MAX_INSNS)
        except UcError as e:
            self.error = str(e)
        if self.error is None and self.uc.reg_read(UC_ARM_REG_PC) & ~1 != RET:
            self.error = 'did not return within %d instructions' % MAX_INSNS
        for ch in list(self.pending):
            self.dma(ch)
        return self

    def snapshot(self):
        mem = {}
        for base, size, cmp in REGIONS:
            if cmp:
                mem[base] = bytearray(self.uc.mem_read(base, size))
        iw = mem[0x03000000]
        lo, hi = STACK_SCRATCH
        iw[lo - 0x03000000:hi - 0x03000000] = bytes(hi - lo)
        for page in self.extra:
            mem[page] = bytes(self.uc.mem_read(page, 0x1000))
        return mem


def first_diff(ma, mb):
    for base in sorted(set(ma) | set(mb)):
        a, b = ma.get(base), mb.get(base)
        if a is None or b is None:
            continue  # a page only one side touched: the other side's read pattern differed, caught below
        if a != b:
            i = next(i for i in range(len(a)) if a[i] != b[i])
            n = sum(x != y for x, y in zip(a, b))
            return '%d bytes differ, first at %#x (orig %02x, ours %02x)' % (n, base + i, a[i], b[i])
    if set(ma) != set(mb):
        return 'different on-demand pages touched: orig-only %s, ours-only %s' % (
            [hex(p) for p in sorted(set(ma) - set(mb))][:4], [hex(p) for p in sorted(set(mb) - set(ma))][:4])
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('funcs', nargs='*')
    ap.add_argument('--runs', type=int, default=200)
    ap.add_argument('--rom-b', default='build/playtest/warioland4_us_playtest.gba')
    args = ap.parse_args()
    rom_a = Path('baserom_us.gba').read_bytes()
    rom_b = Path(args.rom_b).read_bytes()
    syms = elf_symbols(str(Path(args.rom_b).with_suffix('.elf')))
    exp_sizes = {}
    for line in sp.run('arm-none-eabi-nm -S expected/us/obj/*.o expected/us/obj/*/*.o expected/us/obj/*/*/*.o',
                       shell=True, capture_output=True, text=True).stdout.splitlines():
        p = line.split()
        if len(p) == 4 and p[3] in FUNCS:
            exp_sizes[p[3]] = int(p[1], 16)
    bad = 0
    for name in args.funcs or FUNCS:
        returns, choices = FUNCS[name]
        addr, _ = syms[name]
        body_b = syms.get(name + '_C', syms[name])
        range_a = (addr, addr + exp_sizes[name])
        range_b = (body_b[0], body_b[0] + body_b[1])
        diffs, errors, cov_a, cov_b = [], 0, set(), set()
        for seed in range(args.runs):
            arg = choices[seed % len(choices)] if choices else None
            a = Run(rom_a, seed, seed % 4, range_a, arg).go(addr)
            b = Run(rom_b, seed, seed % 4, range_b, arg).go(addr)
            cov_a |= a.covered
            cov_b |= b.covered
            if a.error or b.error:
                if a.error and b.error:
                    errors += 1
                else:
                    diffs.append('seed %d: orig %s / ours %s' % (seed, a.error or 'ok', b.error or 'ok'))
                continue
            d = first_diff(a.snapshot(), b.snapshot())
            if returns:
                ra, rb = a.uc.reg_read(UC_ARM_REG_R0) & 0xFF, b.uc.reg_read(UC_ARM_REG_R0) & 0xFF
                if ra != rb:
                    d = (d + '; ' if d else '') + 'return %#x vs %#x' % (ra, rb)
            if d:
                diffs.append('seed %d (mode %d): %s' % (seed, seed % 4, d))
        n_a = (range_a[1] - range_a[0]) // 2
        print('%-30s runs %d, both faulted/timed out %d, differences %d, coverage orig %d/%d halfwords, ours %d'
              % (name, args.runs, errors, len(diffs), len(cov_a), n_a, len(cov_b)))
        for d in diffs[:6]:
            print('    ' + d)
        bad += bool(diffs)
    sys.exit(1 if bad else 0)


main()

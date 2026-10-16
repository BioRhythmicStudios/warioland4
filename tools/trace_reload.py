#!/usr/bin/env python3
"""Trace agbcc's reload-register rotation for one function.

Runs tools/agbcc/agbcc on a preprocessed file under ptrace and logs every
allocate_reload_reg call made while compiling FUNC: the insn UID, the reload
index, the register chosen, and last_spill_reg (the round-robin cursor)
before and after. Compare the event list with the target's registers to see
which reload rotates differently (see MATCHING_PROGRESS.md, "Reload register
rotation").

Usage (Linux/WSL x86-64, from anywhere):
    cc -E -DVERSION_US -Iinclude -Isrc/<dir> -nostdinc src/<dir>/<file>.c > /tmp/x.i
    python3 tools/trace_reload.py /tmp/x.i FUNC [OUT_DIR]

Writes OUT_DIR/trace-<name>.json (all events), .s and .stderr; OUT_DIR
defaults to the input file's directory. Prints one line per event.

Notes:
- Read-only on disk: breakpoints are patched only in the traced child's
  memory. The compiler binary, flags and sources are not changed.
- The breakpoint and variable offsets are for the current tools/agbcc/agbcc
  build. If the binary is rebuilt they must be found again (nm agbcc).
- Originally written by Codex for DrawHomerunDerby (reports/
  drawhomerunderby_for_gpt6.md); generalised to any function in session 16.
"""
import ctypes, json, os, signal, struct, sys
from pathlib import Path

lib = ctypes.CDLL(None, use_errno=True)
lib.ptrace.restype = ctypes.c_long
lib.ptrace.argtypes = [ctypes.c_uint, ctypes.c_uint, ctypes.c_void_p, ctypes.c_void_p]


def pt(op, pid, a=0, b=0):
    z = lib.ptrace(op, pid, ctypes.c_void_p(a), ctypes.c_void_p(b))
    if z == -1 and ctypes.get_errno():
        raise OSError(ctypes.get_errno(), os.strerror(ctypes.get_errno()))
    return z


class Regs(ctypes.Structure):
    _fields_ = [(n, ctypes.c_ulonglong) for n in
                'r15 r14 r13 r12 rbp rbx r11 r10 r9 r8 rax rcx rdx rsi rdi orig_rax rip cs '
                'eflags rsp ss fs_base gs_base ds es fs gs'.split()]


def regs(pid):
    x = Regs()
    pt(12, pid, 0, ctypes.addressof(x))
    return x


if len(sys.argv) < 3:
    sys.exit(__doc__)
src = Path(sys.argv[1]).resolve()
func = sys.argv[2]
out = Path(sys.argv[3]).resolve() if len(sys.argv) > 3 else src.parent
root = Path(__file__).resolve().parent.parent
agbcc = root / 'tools/agbcc/agbcc'
tag = src.stem

pid = os.fork()
if pid == 0:
    os.chdir(root)
    fd = os.open(str(out / ('trace-' + tag + '.stderr')), os.O_WRONLY | os.O_CREAT | os.O_TRUNC, 0o600)
    os.dup2(fd, 2)
    pt(0, 0)
    os.execv(str(agbcc), ['agbcc', '-O2', '-mthumb-interwork', '-fhex-asm', '-fprologue-bugfix', '-Wall',
                          str(src), '-o', str(out / ('trace-' + tag + '.s'))])
os.waitpid(pid, 0)

base = None
for l in Path('/proc/%d/maps' % pid).read_text().splitlines():
    if '/tools/agbcc/agbcc' in l and l.split()[2] == '00000000':
        base = int(l.split('-')[0], 16)
        break
assert base, 'agbcc mapping not found'
mem = os.open('/proc/%d/mem' % pid, os.O_RDONLY)


def raw(a, n): return os.pread(mem, n, a)
def u64(a): return struct.unpack('<Q', raw(a, 8))[0]
def i32(a): return struct.unpack('<i', raw(a, 4))[0]


def string(a):
    return raw(a, 80).split(b'\0')[0].decode(errors='replace') if a else ''


# allocate_reload_reg entry and return breakpoints.
addresses = [base + 0x14e75f, base + 0x14ef1f]
original = {a: pt(1, pid, a) & 0xffffffffffffffff for a in addresses}


def arm(a): pt(4, pid, a, (original[a] & ~255) | 204)


for a in addresses:
    arm(a)
events, pending, status = [], None, 0
try:
    while True:
        pt(7, pid)
        _, status = os.waitpid(pid, 0)
        if os.WIFEXITED(status):
            break
        if not os.WIFSTOPPED(status):
            raise RuntimeError(status)
        x = regs(pid)
        a = x.rip - 1
        if a not in original:
            raise RuntimeError(('unexpected stop', status, hex(x.rip)))
        if a == addresses[0]:
            pending = None
            if string(u64(base + 0x1f3ed0)) == func:  # current_function_name
                n = i32(base + 0x2012b0)  # n_spills
                pending = {'uid': i32(u64(x.rdi + 32) + 8), 'r': x.rsi & 0xffffffff,
                           'noerror': x.rdx & 0xffffffff, 'force_group': x.rcx & 0xffffffff,
                           'before': i32(base + 0x20151c),  # last_spill_reg
                           'spill_regs': list(struct.unpack('<' + 'h' * n, raw(base + 0x2014e0, 2 * n)))}
        elif pending:
            pending['after'] = i32(base + 0x20151c)
            pending['success'] = x.rax & 0xffffffff
            j = pending['r']
            rp = u64(base + 0x1ff6c0 + 8 * j)  # reload_reg_rtx[r]
            pending['selected_reg'] = i32(rp + 8) if rp else None
            events.append(pending)
            pending = None
        pt(4, pid, a, original[a])
        x.rip = a
        pt(13, pid, 0, ctypes.addressof(x))
        pt(9, pid)
        os.waitpid(pid, 0)
        arm(a)
finally:
    if not os.WIFEXITED(status):
        try:
            os.kill(pid, signal.SIGKILL)
            os.waitpid(pid, 0)
        except ProcessLookupError:
            pass

(out / ('trace-' + tag + '.json')).write_text(json.dumps(events, indent=2))
print(tag, 'exit', os.waitstatus_to_exitcode(status), 'events', len(events))
for e in events:
    print('insn %-5d reload %d -> r%s  cursor %d -> %d  spill_regs %s' %
          (e['uid'], e['r'], e['selected_reg'], e['before'], e['after'], e['spill_regs']))

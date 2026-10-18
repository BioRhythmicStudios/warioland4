#!/usr/bin/env python3
"""Compare the port's GBA memory with mGBA's, frame by frame.

usage: python port/tools/diff_frames.py PORT.bin MGBA.bin [--align idle|step|frame] [--show N]
                                        [--from STEP] [--list] [--no-mask] [--mask-sound]

PORT.bin comes from wl4 --frames N --dump PORT.bin, MGBA.bin from port/tools/mgba_dump.lua.
Both hold memory at the start of each VBlank.

Alignment: the two need not reach the game's main loop on the same frame (the port runs
the boot code in no time), and on the GBA one step of the main loop can take more than a
frame, where the port's take none. With --align idle (the default) the frames compared,
n-th with n-th, are those on which a step has finished: gMainTimer, which counts the steps,
has changed, and the loop waits for VBlank (it has cleared bit 0 of gInterruptCheck).
wl4's PortFrameSteps() counts the same steps. --align step compares the first frame of each
step instead, finished or not, and --align frame every frame.

Masked by default, because they are not the game's state or the port does not model them:
- IWRAM above the game's variables: the GBA's stacks. The port's game stack is elsewhere.
- VCOUNT, DISPSTAT's status bits and the timer counters, which depend on CPU timing.
- SOUNDCNT_X's channel status bits, which the PSG sets as it plays.
- Write-only DMA registers, which the game cannot read back.

--mask-sound also masks the sound engine's working state and the sound registers it writes.

Prints the first frame that differs, with each difference by symbol, and exits with 1;
or reports that every compared frame matched.
"""
import argparse
import bisect
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import frame_dump  # noqa: E402

MAIN_TIMER = 0x03000C41  # gMainTimer, a u8 the main loop counts up
INTERRUPT_CHECK = 0x03000C42  # gInterruptCheck: bit 0 clear while the main loop waits for VBlank

# (start, end, bit mask of the bits to ignore in each byte, reason)
MASKS = [
    (0x03006280, 0x03007FF0, 0xFF, 'the GBA stacks (above gMPlayInfo_7, the last IWRAM variable)'),
    (0x04000004, 0x04000005, 0x07, 'DISPSTAT status bits'),
    (0x04000006, 0x04000008, 0xFF, 'VCOUNT'),
    (0x04000100, 0x04000102, 0xFF, 'timer 0 counter'),
    (0x04000104, 0x04000106, 0xFF, 'timer 1 counter'),
    (0x04000108, 0x0400010A, 0xFF, 'timer 2 counter'),
    (0x0400010C, 0x0400010E, 0xFF, 'timer 3 counter'),
    # Write-only: the game cannot read them back. DMA source addresses also differ because
    # the port's game stack is not in IWRAM.
    (0x040000A0, 0x040000A8, 0xFF, 'sound FIFOs'),
    (0x040000B0, 0x040000B8, 0xFF, 'DMA 0 addresses'),
    (0x040000BC, 0x040000C4, 0xFF, 'DMA 1 addresses'),
    (0x040000C8, 0x040000D0, 0xFF, 'DMA 2 addresses'),
    (0x040000D4, 0x040000DC, 0xFF, 'DMA 3 addresses'),
    # The background scroll registers are write-only and 9 bits wide; mGBA keeps only those bits.
] + [(0x04000011 + 2 * i, 0x04000012 + 2 * i, 0xFE, 'BG scroll, unused bits') for i in range(8)] + [
    # mGBA stores the sound FIFO channels' destination as fixed, whatever the game wrote.
    (0x040000C6, 0x040000C7, 0x60, 'DMA 1 destination mode'),
    (0x040000D2, 0x040000D3, 0x60, 'DMA 2 destination mode'),
    (0x04000084, 0x04000085, 0x0F, 'SOUNDCNT_X channel status'),
]
# Sound engine state, by symbol and size: what SoundMain, MPlayMain and the ply_ routines
# change.
SOUND_MASKS = [
    ('gSoundInfo', 0xFB0),
    ('gCgbChans', 0x100),
    ('gMPlayMemAccArea', 0x10),  # what the songs' memacc commands write
] + [('gMPlayInfo_%d' % i, 0x40) for i in range(8)]
# The music players' tracks, in IWRAM the linker script leaves unnamed (gUnk_3002B98 is
# inside it), and the sound channel status bits.
SOUND_RANGES = [
    (0x03001A08, 0x03002C28, 0xFF, 'sound engine state (music player tracks)'),
    (0x04000060, 0x04000082, 0xFF, 'PSG sound channel registers, which the sound engine writes'),
    (0x04000090, 0x040000A0, 0xFF, 'wave RAM, which the sound engine writes'),
]


def build_masks(symbols, sound):
    masks = list(MASKS)
    if sound:
        masks += SOUND_RANGES
        for name, size in SOUND_MASKS:
            if name in symbols:
                masks.append((symbols[name], symbols[name] + size, 0xFF, 'sound engine state (%s)' % name))
    return masks


def state_masks(symbols, a, b):
    """Masks that apply only in some game states, judged on both frames."""
    masks = []
    # In a room without a raster effect (gHBlankMode 0), SetHBlankMode copies its HBlank
    # callback from address 0, which DMA cannot read: it writes the last value any DMA moved,
    # on mGBA mostly sound FIFO data, which the port does not transfer. The copy never runs.
    mode, trampoline = symbols.get('gHBlankMode'), symbols.get('gHBlankCallbackTrampoline')
    if mode is not None and trampoline is not None and (a.read(mode, 1) == 0 or b.read(mode, 1) == 0):
        masks.append((trampoline, trampoline + 0x200, 0xFF, 'open bus copied as the HBlank callback'))
    return masks


def apply_masks(frame, masks):
    data = bytearray(frame.data)
    for start, end, bits, _ in masks:
        pos = frame.offset(start)
        if bits == 0xFF:
            data[pos:pos + end - start] = bytes(end - start)
        else:
            for i in range(pos, pos + end - start):
                data[i] &= ~bits & 0xFF
    return data


def steps(path, align):
    """Yields the frames to compare: all of them, or one for each step of the main loop."""
    previous = 0  # gMainTimer before the first loop
    for frame in frame_dump.frames(path):
        timer = frame.read(MAIN_TIMER, 1)
        if align == 'idle':
            if timer != previous and not frame.read(INTERRUPT_CHECK, 1) & 1:
                yield frame
                previous = timer
            continue
        if align == 'frame' or timer != previous:
            yield frame
        previous = timer


BLOCK = 256


def differences(a, b):
    """Yields (offset, length) of each run of differing bytes."""
    start = None
    for block in range(0, len(a), BLOCK):
        if a[block:block + BLOCK] == b[block:block + BLOCK]:
            if start is not None:
                yield start, block - start
                start = None
            continue
        for i in range(block, min(block + BLOCK, len(a))):
            if a[i] != b[i]:
                if start is None:
                    start = i
            elif start is not None:
                yield start, i - start
                start = None
    if start is not None:
        yield start, len(a) - start


class Symbolizer:
    def __init__(self, symbols):
        self.items = sorted((address, name) for name, address in symbols.items())
        self.addresses = [address for address, _ in self.items]

    def __call__(self, address):
        i = bisect.bisect_right(self.addresses, address) - 1
        if i < 0 or address - self.addresses[i] > 0x10000:
            return ''
        base, name = self.items[i]
        return name if address == base else '%s+0x%X' % (name, address - base)


def region_name(address):
    for name, base, size in frame_dump.REGIONS:
        if base <= address < base + size:
            return name
    return '?'


def hexbytes(data, limit=16):
    text = ' '.join('%02X' % x for x in data[:limit])
    return text + (' ...' if len(data) > limit else '')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('port')
    ap.add_argument('mgba')
    ap.add_argument('--align', choices=['idle', 'step', 'frame'], default='idle')
    ap.add_argument('--show', type=int, default=40, help='differences to print for the first differing frame')
    ap.add_argument('--list', action='store_true', help='summarise every differing frame, not just the first')
    ap.add_argument('--from', dest='start', type=int, default=1, help='first step to compare')
    ap.add_argument('--no-mask', action='store_true', help='compare everything, sound and stacks included')
    ap.add_argument('--mask-sound', action='store_true', help="leave out the sound engine's state")
    ap.add_argument('--symbols', default='port/generated/gba_symbols.s')
    args = ap.parse_args()

    symbols = frame_dump.gba_symbols(args.symbols)
    symbolize = Symbolizer(symbols)
    masks = [] if args.no_mask else build_masks(symbols, sound=args.mask_sound)

    compared = differing = 0
    first = None
    for n, (a, b) in enumerate(zip(steps(args.port, args.align), steps(args.mgba, args.align)), 1):
        if n < args.start:
            continue
        compared += 1
        pair_masks = masks if args.no_mask else masks + state_masks(symbols, a, b)
        da, db = apply_masks(a, pair_masks), apply_masks(b, pair_masks)
        if da == db:
            continue
        differing += 1
        runs = list(differences(da, db))
        if first is None:
            first = n
            print('First difference: step %d (port frame %d, mGBA frame %d), %d bytes in %d runs'
                  % (n, a.number, b.number, sum(length for _, length in runs), len(runs)))
            per_region = {}
            for offset, length in runs:
                name = region_name(a.address(offset))
                per_region[name] = per_region.get(name, 0) + length
            print('  by region: ' + ', '.join('%s %d' % item for item in per_region.items()))
            for offset, length in runs[:args.show]:
                address = a.address(offset)
                print('  0x%08X %-32s port %s | mGBA %s' % (address, symbolize(address),
                      hexbytes(da[offset:offset + length]), hexbytes(db[offset:offset + length])))
            if len(runs) > args.show:
                print('  ... %d more runs' % (len(runs) - args.show))
            if not args.list:
                break
        else:
            summary = {}
            for offset, length in runs:
                name = region_name(a.address(offset))
                summary[name] = summary.get(name, 0) + length
            print('step %d (port %d, mGBA %d): %s' % (n, a.number, b.number,
                  ', '.join('%s %d' % item for item in summary.items())))

    if first is None:
        print('%d steps compared (align by %s): all identical' % (compared, args.align))
        return 0
    if args.list:
        print('%d of %d steps differ' % (differing, compared))
    return 1


if __name__ == '__main__':
    sys.exit(main())

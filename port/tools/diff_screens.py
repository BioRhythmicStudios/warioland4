#!/usr/bin/env python3
"""Compare the port's pictures with mGBA's screenshots, frame by frame.

usage: python port/tools/diff_screens.py PORT.bin PORT.shots MGBA.bin MGBA_RUN_DIR
                                         [--from STEP] [--to STEP] [--list] [--show N]
                                         [--tolerance] [--save DIR]

PORT.bin and PORT.shots come from wl4 --frames N --dump PORT.bin --shots PORT.shots, MGBA.bin
and MGBA_RUN_DIR from port/tools/run_mgba_dump.ps1 -Shots (the run directory it prints).
The frames are paired as diff_frames.py pairs them: on the steps of the game's main loop, when
each has finished, n-th with n-th, so boot time and lag frames do not matter.

Run wl4 with --mgba-colors to compare exactly. Without it the port blends colours as the
GBA does, on 5-bit channels, and mGBA's blended colours can be a step off either way (its
results can still look like plain 5-bit colours); --tolerance then counts a pixel as
matching when no channel is more than one 5-bit step away. That is only a sanity check of the
GBA colour maths: the exact comparison is the one with --mgba-colors.

Prints the first differing step with its differing pixels, or that every step matched.
--save writes a PNG for each differing step reported: port, mGBA and the differences side
by side. The decoded screenshots are cached in MGBA_RUN_DIR/screens.rgb.
"""
import argparse
import os
import struct
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import diff_frames  # noqa: E402

WIDTH, HEIGHT = 240, 160
FRAME_BYTES = WIDTH * HEIGHT * 3
SHOT_MAGIC = 0x53344C57  # "WL4S"
SHOT_HEADER = struct.Struct('<II')


# --- PNG ------------------------------------------------------------------------------------

def read_png(path):
    """The RGB bytes of an 8-bit RGB or RGBA PNG without interlacing, as mGBA writes them."""
    data = open(path, 'rb').read()
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError('%s: not a PNG' % path)
    pos, idat = 8, []
    width = height = channels = None
    while pos < len(data):
        length, kind = struct.unpack('>I4s', data[pos:pos + 8])
        body = data[pos + 8:pos + 8 + length]
        if kind == b'IHDR':
            width, height, depth, color, _, _, interlace = struct.unpack('>IIBBBBB', body)
            if depth != 8 or color not in (2, 6) or interlace:
                raise ValueError('%s: unsupported PNG format' % path)
            channels = 3 if color == 2 else 4
        elif kind == b'IDAT':
            idat.append(body)
        pos += 12 + length
    raw = zlib.decompress(b''.join(idat))
    stride = width * channels
    rows, previous = [], bytearray(stride)
    for y in range(height):
        kind = raw[y * (stride + 1)]
        row = bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        if kind == 1:
            for i in range(channels, stride):
                row[i] = (row[i] + row[i - channels]) & 0xFF
        elif kind == 2:
            for i in range(stride):
                row[i] = (row[i] + previous[i]) & 0xFF
        elif kind == 3:
            for i in range(stride):
                left = row[i - channels] if i >= channels else 0
                row[i] = (row[i] + ((left + previous[i]) >> 1)) & 0xFF
        elif kind == 4:
            for i in range(stride):
                a = row[i - channels] if i >= channels else 0
                b = previous[i]
                c = previous[i - channels] if i >= channels else 0
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                row[i] = (row[i] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 0xFF
        elif kind != 0:
            raise ValueError('%s: bad PNG filter %d' % (path, kind))
        rows.append(bytes(row) if channels == 3 else bytes(b for i, b in enumerate(row) if i % 4 != 3))
        previous = row
    return b''.join(rows), width, height


def write_png(path, rgb, width, height):
    raw = b''.join(b'\0' + rgb[y * width * 3:(y + 1) * width * 3] for y in range(height))

    def chunk(kind, body):
        return struct.pack('>I', len(body)) + kind + body + struct.pack('>I', zlib.crc32(kind + body))

    with open(path, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n')
        f.write(chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0)))
        f.write(chunk(b'IDAT', zlib.compress(raw, 6)))
        f.write(chunk(b'IEND', b''))


# --- Inputs ---------------------------------------------------------------------------------

class MgbaScreens:
    """mGBA's screenshots: wl4-N.png in the run directory goes with the N-th frame of the dump,
    from 0 (frame N+1 when the dump starts at the first frame)."""

    def __init__(self, run_dir):
        self.run_dir = run_dir
        self.cache = os.path.join(run_dir, 'screens.rgb')
        count = 0
        while os.path.exists(os.path.join(run_dir, 'wl4-%d.png' % count)):
            count += 1
        if count == 0:
            raise SystemExit('%s has no screenshots (wl4-0.png, ...)' % run_dir)
        if not os.path.exists(self.cache) or os.path.getsize(self.cache) != count * FRAME_BYTES:
            sys.stderr.write('Decoding %d screenshots into %s ...\n' % (count, self.cache))
            with open(self.cache + '.tmp', 'wb') as out:
                for n in range(count):
                    rgb, width, height = read_png(os.path.join(run_dir, 'wl4-%d.png' % n))
                    if (width, height) != (WIDTH, HEIGHT):
                        raise SystemExit('wl4-%d.png is %dx%d' % (n, width, height))
                    out.write(rgb)
            os.replace(self.cache + '.tmp', self.cache)
        self.count = count
        self.file = open(self.cache, 'rb')

    def frame(self, dump_frame):
        if not 0 <= dump_frame.index < self.count:
            return None
        self.file.seek(dump_frame.index * FRAME_BYTES)
        return self.file.read(FRAME_BYTES)


class PortScreens:
    def __init__(self, path):
        self.file = open(path, 'rb')
        self.count = os.path.getsize(path) // (SHOT_HEADER.size + FRAME_BYTES)

    def frame(self, dump_frame):
        """The picture that goes with a frame of the dump: wl4 writes them side by side."""
        if not 0 <= dump_frame.index < self.count:
            return None
        self.file.seek(dump_frame.index * (SHOT_HEADER.size + FRAME_BYTES))
        magic, frame = SHOT_HEADER.unpack(self.file.read(SHOT_HEADER.size))
        if magic != SHOT_MAGIC or frame != dump_frame.number:
            raise SystemExit('bad screenshot header for frame %d' % dump_frame.number)
        return self.file.read(FRAME_BYTES)


# --- Comparing ------------------------------------------------------------------------------

def differing_pixels(port, mgba, tolerance):
    """Yields (x, y, port RGB, mGBA RGB) for each pixel that differs."""
    for y in range(HEIGHT):
        a = port[y * WIDTH * 3:(y + 1) * WIDTH * 3]
        b = mgba[y * WIDTH * 3:(y + 1) * WIDTH * 3]
        if a == b:
            continue
        for x in range(WIDTH):
            pa, pb = a[x * 3:x * 3 + 3], b[x * 3:x * 3 + 3]
            if pa == pb:
                continue
            if tolerance and all(abs((va >> 3) - (vb >> 3)) <= 1 for va, vb in zip(pa, pb)):
                continue
            yield x, y, tuple(pa), tuple(pb)


def save_comparison(path, port, mgba, pixels):
    """Port, mGBA and a map of the differing pixels (in red), side by side."""
    marked = bytearray(FRAME_BYTES)
    for i in range(0, FRAME_BYTES, 3):
        grey = (port[i] + port[i + 1] + port[i + 2]) // 12
        marked[i:i + 3] = bytes((grey, grey, grey))
    for x, y, _, _ in pixels:
        marked[(y * WIDTH + x) * 3:(y * WIDTH + x) * 3 + 3] = b'\xff\x00\x00'
    rows = []
    for y in range(HEIGHT):
        line = slice(y * WIDTH * 3, (y + 1) * WIDTH * 3)
        rows.append(port[line] + b'\0\0\0' * 4 + mgba[line] + b'\0\0\0' * 4 + bytes(marked[line]))
    write_png(path, b''.join(rows), WIDTH * 3 + 8, HEIGHT)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('port_dump')
    ap.add_argument('port_shots')
    ap.add_argument('mgba_dump')
    ap.add_argument('mgba_run_dir')
    ap.add_argument('--from', dest='start', type=int, default=1, help='first step to compare')
    ap.add_argument('--to', dest='end', type=int, default=None, help='last step to compare')
    ap.add_argument('--list', action='store_true', help='summarise every differing step, not just the first')
    ap.add_argument('--show', type=int, default=20, help='differing pixels to print for the first differing step')
    ap.add_argument('--tolerance', action='store_true', help='allow one 5-bit step per channel (see above)')
    ap.add_argument('--save', metavar='DIR', help='write a comparison PNG for each differing step reported')
    args = ap.parse_args()

    port_screens = PortScreens(args.port_shots)
    mgba_screens = MgbaScreens(args.mgba_run_dir)
    if args.save:
        os.makedirs(args.save, exist_ok=True)

    compared = differing = 0
    first = None
    pairs = zip(diff_frames.steps(args.port_dump, 'idle'), diff_frames.steps(args.mgba_dump, 'idle'))
    for n, (a, b) in enumerate(pairs, 1):
        if n < args.start:
            continue
        if args.end is not None and n > args.end:
            break
        port, mgba = port_screens.frame(a), mgba_screens.frame(b)
        if port is None or mgba is None:
            break
        compared += 1
        if port == mgba:
            continue
        pixels = list(differing_pixels(port, mgba, args.tolerance))
        if not pixels:
            continue
        differing += 1
        lines = sorted({y for _, y, _, _ in pixels})
        if args.save:
            save_comparison(os.path.join(args.save, 'step%05d.png' % n), port, mgba, pixels)
        if first is None:
            first = n
            print('First difference: step %d (port frame %d, mGBA frame %d): %d pixels on %d lines (%d-%d)'
                  % (n, a.number, b.number, len(pixels), len(lines), lines[0], lines[-1]))
            for x, y, pa, pb in pixels[:args.show]:
                print('  (%3d, %3d) port %02X %02X %02X | mGBA %02X %02X %02X' % ((x, y) + pa + pb))
            if len(pixels) > args.show:
                print('  ... %d more' % (len(pixels) - args.show))
            if not args.list:
                break
        else:
            print('step %d (port %d, mGBA %d): %d pixels on lines %d-%d'
                  % (n, a.number, b.number, len(pixels), lines[0], lines[-1]))

    if first is None:
        print('%d steps compared: all identical%s' % (compared, ' (within tolerance)' if args.tolerance else ''))
        return 0
    if args.list:
        print('%d of %d steps differ' % (differing, compared))
    return 1


if __name__ == '__main__':
    sys.exit(main())

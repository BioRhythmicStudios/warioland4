"""Reading frame dumps: the format that wl4 --dump and port/tools/mgba_dump.lua write.

Each frame is a header (magic "WL4F", frame number; two little-endian u32) followed by GBA
memory, region after region, in the order of REGIONS.
"""
import pathlib
import re
import struct
import sys

MAGIC = 0x46344C57
HEADER = struct.Struct('<II')
# (name, GBA base address, size), in dump order.
REGIONS = [
    ('EWRAM', 0x02000000, 0x40000),
    ('IWRAM', 0x03000000, 0x8000),
    ('I/O', 0x04000000, 0x400),
    ('palette', 0x05000000, 0x400),
    ('VRAM', 0x06000000, 0x18000),
    ('OAM', 0x07000000, 0x400),
    ('SRAM', 0x0E000000, 0x8000),
]
FRAME_SIZE = HEADER.size + sum(size for _, _, size in REGIONS)


class Frame:
    def __init__(self, number, data, index=0):
        self.number = number
        self.data = data  # the memory, without the header
        self.index = index  # the position in the dump file, from 0

    def offset(self, address):
        """The offset of a GBA address in self.data, or None."""
        pos = 0
        for _, base, size in REGIONS:
            if base <= address < base + size:
                return pos + address - base
            pos += size
        return None

    def read(self, address, size):
        pos = self.offset(address)
        return int.from_bytes(self.data[pos:pos + size], 'little')

    def address(self, offset):
        """The GBA address of an offset in self.data."""
        for _, base, size in REGIONS:
            if offset < size:
                return base + offset
            offset -= size
        raise ValueError(offset)


def frames(path):
    """Yields the frames of a dump file."""
    with open(path, 'rb') as f:
        index = 0
        while True:
            chunk = f.read(FRAME_SIZE)
            if not chunk:
                return
            if len(chunk) < FRAME_SIZE:
                sys.stderr.write('%s: the last frame is incomplete; stopping before it\n' % path)
                return
            magic, number = HEADER.unpack_from(chunk)
            if magic != MAGIC:
                raise ValueError('%s: bad frame header' % path)
            yield Frame(number, memoryview(chunk)[HEADER.size:], index)
            index += 1


def gba_symbols(path='port/generated/gba_symbols.s'):
    """{name: address} for the game's data symbols in RAM, from gen_symbols.py's output."""
    symbols = {}
    for name, value in re.findall(r'\.set (\w+), (0x[0-9A-Fa-f]+)', pathlib.Path(path).read_text()):
        address = int(value, 16)
        if 0x02000000 <= address < 0x08000000:
            symbols[name] = address
    return symbols

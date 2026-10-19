"""Draw an original square-within-square placeholder ICO using only geometry.

Project-authored asset generator; MIT, same terms as the project's LICENSE.
Uses no artwork, game data, font, image library or downloaded inputs.
Normal builds use the checked-in output and do not require Python.
"""
import struct
from pathlib import Path


def bitmap(size):
    # Original abstract design: gold square inset in a purple square.
    pixels = bytearray()
    for y in range(size):
        for x in range(size):
            inset = size // 4
            r, g, b = ((255, 204, 64) if inset <= x < size - inset
                       and inset <= y < size - inset else (40, 17, 66))
            pixels.extend((b, g, r, 255))
    mask = bytes(((size + 31) // 32) * 4 * size)
    return (struct.pack('<IiiHHIIiiII', 40, size, size * 2, 1, 32,
                        0, len(pixels), 0, 0, 0, 0) + pixels + mask)


def generate():
    sizes = (16, 32, 48, 256)
    images = [bitmap(size) for size in sizes]
    header = struct.pack('<HHH', 0, 1, len(images))
    offset = 6 + len(images) * 16
    entries = bytearray()
    for size, data in zip(sizes, images):
        entries.extend(struct.pack('<BBBBHHII', size % 256, size % 256,
                                   0, 0, 1, 32, len(data), offset))
        offset += len(data)
    return header + entries + b''.join(images)


if __name__ == '__main__':
    target = Path(__file__).resolve().parents[1] / 'assets' / 'neutral-placeholder.ico'
    target.write_bytes(generate())

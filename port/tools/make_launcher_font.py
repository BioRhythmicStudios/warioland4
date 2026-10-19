"""Convert an atlas rendered by launcher_font_atlas.c into a private Windows TTF.
Requires fontTools only when regenerating the committed font asset.
The source glyph shapes are SDL debug text's public-domain 8x8 VGA font.
"""
import struct
import sys
from pathlib import Path
from fontTools.fontBuilder import FontBuilder
from fontTools.pens.ttGlyphPen import TTGlyphPen


def generate(atlas, destination):
    data = Path(atlas).read_bytes()
    offset = struct.unpack_from("<I", data, 10)[0]
    width, height = struct.unpack_from("<ii", data, 18)
    bits = struct.unpack_from("<H", data, 28)[0]
    assert width == 128 and abs(height) == 112 and bits in (24, 32)
    stride = ((width * bits + 31) // 32) * 4

    def white(x, y):
        row = abs(height) - 1 - y if height > 0 else y
        return data[offset + row * stride + x * (bits // 8)] > 128

    codes = list(range(32, 127)) + list(range(160, 256))
    names = {code: f"uni{code:04X}" for code in codes}
    glyphs = {}
    for name, code in [(".notdef", 63)] + [(names[code], code) for code in codes]:
        pen = TTGlyphPen(None)
        cell_x, cell_y = ((code - 32) % 16) * 8, ((code - 32) // 16) * 8
        for row in range(8):
            col = 0
            while col < 8:
                if not white(cell_x + col, cell_y + row):
                    col += 1
                    continue
                start = col
                while col < 8 and white(cell_x + col, cell_y + row):
                    col += 1
                x, y = start * 100, 600 - row * 100
                pen.moveTo((x, y))
                pen.lineTo((x, y + 100))
                pen.lineTo((col * 100, y + 100))
                pen.lineTo((col * 100, y))
                pen.closePath()
        glyphs[name] = pen.glyph()
    builder = FontBuilder(800, isTTF=True)
    builder.setupGlyphOrder([".notdef"] + [names[code] for code in codes])
    builder.setupCharacterMap(names)
    builder.setupGlyf(glyphs)
    for glyph in glyphs.values():
        glyph.recalcBounds(builder.font["glyf"])
    builder.setupHorizontalMetrics({name: (800, getattr(glyph, "xMin", 0)) for name, glyph in glyphs.items()})
    builder.setupHorizontalHeader(ascent=700, descent=-100, lineGap=0)
    builder.setupNameTable({"familyName": "WL4 Pixel", "styleName": "Regular",
        "uniqueFontIdentifier": "WL4Pixel-1", "fullName": "WL4 Pixel",
        "psName": "WL4Pixel", "version": "Version 1.000",
        "copyright": "Public-domain VGA glyphs from SDL debug text; private TTF conversion for WL4."})
    builder.setupOS2(sTypoAscender=700, sTypoDescender=-100, sTypoLineGap=0,
                    usWinAscent=700, usWinDescent=100)
    builder.setupPost(isFixedPitch=1)
    builder.setupMaxp()
    builder.font.recalcTimestamp = False
    builder.font["head"].created = builder.font["head"].modified = 3875126400
    builder.save(destination)
    print(f"Created {destination}: {len(codes)} glyphs at an exact 8x8 grid")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit("usage: make_launcher_font.py FONT-ATLAS.bmp OUTPUT.ttf")
    generate(sys.argv[1], sys.argv[2])

#include "port/ppu.h"

#include <stdint.h>
#include <string.h>

#include "port/gba_io.h"
#include "port/gba_memory.h"

// The display, one line at a time. Each line is drawn layer by layer into line buffers (the
// four backgrounds and the sprites), then composed pixel by pixel: the window that covers the
// pixel picks the layers and whether effects apply, the two topmost layers are found by
// priority, and they are blended or brightened as BLDCNT asks.
//
// Where the hardware's behaviour is unclear, this follows mGBA 0.10.5's software renderer,
// which the port is checked against:
// - A background turned on during the frame appears three lines later (two in the bitmap
//   modes); one turned on during VBlank appears at once.
// - Sprites share 1210 cycles a line (954 when DISPCNT frees HBlank): a regular sprite costs
//   its width, an affine one 10 plus twice its box width. Sprites past the budget are not drawn.
// - Where a sprite's transparent pixel lies over another sprite's pixel and the sprite has the
//   higher priority, that pixel takes the sprite's priority (the GBA's OBJ priority quirk).
// - Semi-transparent sprites blend with a second target below them in any blend mode, even
//   where the window turns effects off. If none of the layers is a second target, they are
//   plain sprites; if they blend with nothing, BLDY's brightness applies to them.
// - A layer that is a first target in a window with effects on is brightened or darkened
//   even when it ends up below another layer, and blends that way.
// - The window registers are clamped and wrap as mGBA does.

#define WIDTH PORT_SCREEN_WIDTH
#define HEIGHT PORT_SCREEN_HEIGHT

// Registers, as offsets into the I/O page.
#define REG_DISPCNT 0x000
#define REG_BG0CNT 0x008
#define REG_BG0HOFS 0x010
#define REG_BG2PA 0x020
#define REG_BG2X 0x028
#define REG_WIN0H 0x040
#define REG_WIN1H 0x042
#define REG_WIN0V 0x044
#define REG_WIN1V 0x046
#define REG_WININ 0x048
#define REG_WINOUT 0x04A
#define REG_MOSAIC 0x04C
#define REG_BLDCNT 0x050
#define REG_BLDALPHA 0x052
#define REG_BLDY 0x054
#define AFFINE_STRIDE 0x10  // from BG2's affine registers to BG3's

#define DISPCNT_MODE(d) ((d) & 7)
#define DISPCNT_FRAME 0x0010
#define DISPCNT_HBLANK_FREE 0x0020
#define DISPCNT_OBJ_1D 0x0040
#define DISPCNT_FORCED_BLANK 0x0080
#define DISPCNT_BG(n) (0x0100u << (n))
#define DISPCNT_OBJ 0x1000
#define DISPCNT_WIN0 0x2000
#define DISPCNT_WIN1 0x4000
#define DISPCNT_OBJWIN 0x8000

#define BGCNT_MOSAIC 0x0040
#define BGCNT_256_COLORS 0x0080
#define BGCNT_WRAP 0x2000

#define OBJ_AFFINE 0x0100
#define OBJ_DOUBLE_SIZE 0x0200  // affine sprites; regular ones are hidden by this bit
#define OBJ_MODE(a) (((a) >> 10) & 3)
#define OBJ_MODE_SEMI 1
#define OBJ_MODE_WINDOW 2
#define OBJ_MOSAIC 0x1000
#define OBJ_256_COLORS 0x2000
#define OBJ_HFLIP 0x1000
#define OBJ_VFLIP 0x2000

#define BLEND_ALPHA 1
#define BLEND_BRIGHTEN 2
#define BLEND_DARKEN 3

// One bit per layer in the window and blend registers.
#define LAYER_OBJ 4
#define LAYER_BACKDROP 5
#define WINDOW_EFFECTS 0x20
#define IN_OBJ_WINDOW 0x80  // flag on a pixel's window control: it comes from the OBJ window

#define OBJ_VRAM 0x10000u
#define OBJ_PALETTE 0x100u
#define OBJ_CYCLES 1210
#define OBJ_CYCLES_HBLANK_FREE 954

// A background is drawn while enabled is ENABLED. Turned on during the frame, it counts up
// from 1 (2 in the bitmap modes), one per line. Turned off during the frame, it goes to -2 and
// counts up to 0, and turning it on again before then takes effect at once.
#define ENABLED 4

struct Background {
    int enabled;
    int32_t x, y;  // affine backgrounds: the current reference point, 20.8 fixed point
};

static enum PortPpuColors sColors;
static struct Background sBg[4];
static uint16_t sDispcnt;  // as last seen, to act on changes
static uint32_t sFrame[HEIGHT * WIDTH];

// One line's layers. Background and sprite pixels are BGR555 colours with OPAQUE set, or 0.
#define OPAQUE 0x8000
#define NO_OBJ 4  // in sObjPriority: no sprite pixel
static uint16_t sBgLine[4][WIDTH];
static uint16_t sObjColor[WIDTH];
static uint8_t sObjPriority[WIDTH];
static uint8_t sObjSemi[WIDTH];
static uint8_t sObjWindow[WIDTH];
static uint8_t sWindow[WIDTH];  // the window control that applies to each pixel
static uint8_t sSpan[WIDTH];    // the same, ignoring the OBJ window

// --- Memory --------------------------------------------------------------------------------

static inline uint8_t Vram8(uint32_t offset)
{
    return ((const uint8_t *)(uintptr_t)GBA_VRAM_BASE)[offset];
}

static inline uint16_t Vram16(uint32_t offset)
{
    return ((const uint16_t *)(uintptr_t)GBA_VRAM_BASE)[offset >> 1];
}

static inline uint16_t Palette(unsigned int index)
{
    return ((const uint16_t *)(uintptr_t)GBA_PLTT_BASE)[index] & 0x7FFF;
}

static inline uint16_t Oam16(unsigned int offset)
{
    return ((const uint16_t *)(uintptr_t)GBA_OAM_BASE)[offset >> 1];
}

static inline uint16_t Reg(unsigned int offset)
{
    return PortIoRead16(offset);
}

// An affine reference point register: 28-bit signed, 20.8 fixed point.
static int32_t AffineReference(unsigned int offset)
{
    uint32_t value = Reg(offset) | (uint32_t)Reg(offset + 2) << 16;
    return (int32_t)(value << 4) >> 4;
}

// --- Colours -------------------------------------------------------------------------------
// Colours are 8-bit channels, red in the low byte. On the GBA path every colour is the
// expansion of a 5-bit one, so the maths below can take the 5 bits back out.

static uint32_t Expand(uint16_t color)
{
    uint32_t r = color & 0x1F, g = (color >> 5) & 0x1F, b = (color >> 10) & 0x1F;
    return (r << 3 | r >> 2) | (g << 3 | g >> 2) << 8 | (b << 3 | b >> 2) << 16;
}

static uint32_t Channel5(uint32_t value)
{
    return value << 3 | value >> 2;
}

// mGBA works on each channel in place, at its bit position, and masks the result. For green
// and blue that keeps the fraction of a division below the channel, so darkening rounds
// those two up where it rounds red down.
static uint32_t Brighten(uint32_t color, unsigned int evy)
{
    uint32_t out = 0;
    int shift;

    for (shift = 0; shift < 24; shift += 8) {
        uint32_t mask = 0xFFu << shift, c;
        if (sColors == PORT_PPU_COLORS_MGBA) {
            c = color & mask;
            out |= (c + ((mask - c) * evy) / 16) & mask;
        } else {
            c = (color >> (shift + 3)) & 0x1F;
            out |= Channel5(c + ((31 - c) * evy) / 16) << shift;
        }
    }
    return out;
}

static uint32_t Darken(uint32_t color, unsigned int evy)
{
    uint32_t out = 0;
    int shift;

    for (shift = 0; shift < 24; shift += 8) {
        uint32_t mask = 0xFFu << shift, c;
        if (sColors == PORT_PPU_COLORS_MGBA) {
            c = color & mask;
            out |= (c - (c * evy) / 16) & mask;
        } else {
            c = (color >> (shift + 3)) & 0x1F;
            out |= Channel5(c - (c * evy) / 16) << shift;
        }
    }
    return out;
}

static uint32_t Mix(uint32_t a, unsigned int eva, uint32_t b, unsigned int evb)
{
    uint32_t out = 0;
    int shift;

    for (shift = 0; shift < 24; shift += 8) {
        uint32_t ca = (a >> shift) & 0xFF, cb = (b >> shift) & 0xFF, c;
        if (sColors == PORT_PPU_COLORS_MGBA) {
            c = (((ca << shift) * eva + (cb << shift) * evb) / 16) >> shift;
            if (c > 0xFF)
                c = 0xFF;
        } else {
            c = ((ca >> 3) * eva + (cb >> 3) * evb) / 16;
            c = Channel5(c > 31 ? 31 : c);
        }
        out |= c << shift;
    }
    return out;
}

// --- Backgrounds ---------------------------------------------------------------------------

static void DrawTextBackground(int n, unsigned int line, uint16_t mosaic, uint16_t *out)
{
    uint16_t cnt = Reg(REG_BG0CNT + 2 * n);
    unsigned int hofs = Reg(REG_BG0HOFS + 4 * n) & 0x1FF;
    unsigned int vofs = Reg(REG_BG0HOFS + 4 * n + 2) & 0x1FF;
    uint32_t charBase = (uint32_t)((cnt >> 2) & 3) << 14;
    uint32_t screenBase = (uint32_t)((cnt >> 8) & 0x1F) << 11;
    unsigned int size = cnt >> 14;
    unsigned int mosaicH = 1, inY, x;
    uint32_t row;

    if (cnt & BGCNT_MOSAIC) {
        mosaicH = (mosaic & 0xF) + 1;
        line -= line % (((mosaic >> 4) & 0xF) + 1);
    }
    inY = line + vofs;
    // Maps are 32x32 tiles; the larger sizes put further maps to the right and below.
    row = inY & 0xF8;
    if (size == 2)
        row += inY & 0x100;
    else if (size == 3)
        row += (inY & 0x100) << 1;
    row = screenBase + row * 8;

    for (x = 0; x < WIDTH; x++) {
        unsigned int inX = (x - x % mosaicH + hofs) & 0x1FF;
        uint32_t entryOffset = (inX & 0xF8) >> 2;
        uint16_t entry, color = 0;
        unsigned int tx, ty, index;
        uint32_t tile;

        if (size & 1)
            entryOffset += (inX & 0x100) << 3;
        entry = Vram16(row + entryOffset);
        tx = inX & 7;
        ty = inY & 7;
        if (entry & 0x400)
            tx ^= 7;
        if (entry & 0x800)
            ty ^= 7;
        if (cnt & BGCNT_256_COLORS) {
            tile = charBase + (entry & 0x3FF) * 64 + ty * 8;
            // Backgrounds cannot read the sprites' tiles.
            if (tile < OBJ_VRAM && (index = Vram8(tile + tx)) != 0)
                color = Palette(index) | OPAQUE;
        } else {
            tile = charBase + (entry & 0x3FF) * 32 + ty * 4;
            if (tile < OBJ_VRAM && (index = (Vram8(tile + tx / 2) >> (tx & 1) * 4) & 0xF) != 0)
                color = Palette((entry >> 12) * 16 + index) | OPAQUE;
        }
        out[x] = color;
    }
}

// The start of an affine line: the reference point, moved back to the first line of a
// mosaic block.
static void AffineStart(int n, unsigned int line, uint16_t cnt, uint16_t mosaic, int32_t *x, int32_t *y,
                        unsigned int *mosaicH)
{
    unsigned int affine = REG_BG2PA + (n - 2) * AFFINE_STRIDE;

    *x = sBg[n].x;
    *y = sBg[n].y;
    *mosaicH = 1;
    if (cnt & BGCNT_MOSAIC) {
        int32_t back = (int32_t)(line % (((mosaic >> 4) & 0xF) + 1));
        *x -= back * (int16_t)Reg(affine + 2);
        *y -= back * (int16_t)Reg(affine + 6);
        *mosaicH = (mosaic & 0xF) + 1;
    }
}

static void DrawAffineBackground(int n, unsigned int line, uint16_t mosaic, uint16_t *out)
{
    uint16_t cnt = Reg(REG_BG0CNT + 2 * n);
    unsigned int affine = REG_BG2PA + (n - 2) * AFFINE_STRIDE;
    int32_t pa = (int16_t)Reg(affine), pc = (int16_t)Reg(affine + 4);
    uint32_t charBase = (uint32_t)((cnt >> 2) & 3) << 14;
    uint32_t screenBase = (uint32_t)((cnt >> 8) & 0x1F) << 11;
    unsigned int size = cnt >> 14, mosaicH, x;
    int32_t mask = (0x8000 << size) - 1;  // the map's size in 20.8 fixed point, less one
    int32_t startX, startY;

    AffineStart(n, line, cnt, mosaic, &startX, &startY, &mosaicH);
    for (x = 0; x < WIDTH; x++) {
        int32_t sx = (int32_t)(x - x % mosaicH);
        int32_t px = startX + sx * pa, py = startY + sx * pc;
        unsigned int tile, index;

        if (cnt & BGCNT_WRAP) {
            px &= mask;
            py &= mask;
        } else if ((px | py) & ~mask) {
            out[x] = 0;
            continue;
        }
        tile = Vram8(screenBase + (px >> 11) + ((uint32_t)((py >> 7) & 0x7F0) << size));
        index = Vram8(charBase + (tile << 6) + ((py & 0x700) >> 5) + ((px & 0x700) >> 8));
        out[x] = index ? Palette(index) | OPAQUE : 0;
    }
}

// Modes 3-5: BG2 is a bitmap, with BG2's affine transform.
static void DrawBitmapBackground(uint16_t dispcnt, unsigned int line, uint16_t mosaic, uint16_t *out)
{
    unsigned int mode = DISPCNT_MODE(dispcnt);
    uint16_t cnt = Reg(REG_BG0CNT + 4);
    int32_t pa = (int16_t)Reg(REG_BG2PA), pc = (int16_t)Reg(REG_BG2PA + 4);
    int32_t width = mode == 5 ? 160 : WIDTH, height = mode == 5 ? 128 : HEIGHT;
    uint32_t page = mode != 3 && (dispcnt & DISPCNT_FRAME) ? 0xA000 : 0;
    unsigned int mosaicH, x;
    int32_t startX, startY;

    AffineStart(2, line, cnt, mosaic, &startX, &startY, &mosaicH);
    for (x = 0; x < WIDTH; x++) {
        int32_t sx = (int32_t)(x - x % mosaicH);
        int32_t px = (startX + sx * pa) >> 8, py = (startY + sx * pc) >> 8;
        uint32_t pixel;

        if (px < 0 || py < 0 || px >= width || py >= height) {
            out[x] = 0;
            continue;
        }
        pixel = (uint32_t)(py * width + px);
        if (mode == 4) {
            unsigned int index = Vram8(page + pixel);
            out[x] = index ? Palette(index) | OPAQUE : 0;
        } else {
            out[x] = (Vram16(page + pixel * 2) & 0x7FFF) | OPAQUE;
        }
    }
}

// --- Sprites -------------------------------------------------------------------------------

static const uint8_t kObjSizes[16][2] = {
    { 8, 8 },  { 16, 16 }, { 32, 32 }, { 64, 64 },  // square
    { 16, 8 }, { 32, 8 },  { 32, 16 }, { 64, 32 },  // wide
    { 8, 16 }, { 8, 32 },  { 16, 32 }, { 32, 64 },  // tall
    { 0, 0 },  { 0, 0 },   { 0, 0 },   { 0, 0 },    // shape 3 is not used
};

// Where a sprite's tiles are, in the 32 KiB of sprite VRAM.
struct ObjTiles {
    uint32_t base;    // the first tile
    uint32_t stride;  // bytes from one row of tiles to the next, per pixel row of a tile
    uint32_t wrap;    // the 2D map wraps within a row of 32 tiles
    uint32_t rowBase;
    int color256;
};

static unsigned int ObjTexel(const struct ObjTiles *t, int lx, int ly)
{
    uint32_t offset;
    uint16_t data;

    if (t->color256) {
        offset = (uint32_t)(ly & ~7) * t->stride + (ly & 7) * 8 + t->rowBase;
        offset = (offset + (((uint32_t)(lx & ~7) * 8 + (lx & 6) + t->base) & t->wrap)) & 0x7FFE;
        data = Vram16(OBJ_VRAM + offset);
        return (data >> ((lx & 1) << 3)) & 0xFF;
    }
    offset = (uint32_t)(ly & ~7) * t->stride + (ly & 7) * 4 + t->rowBase;
    offset = (offset + (((uint32_t)(lx & ~7) * 4 + ((lx >> 1) & 2) + t->base) & t->wrap)) & 0x7FFE;
    data = Vram16(OBJ_VRAM + offset);
    return (data >> ((lx & 3) << 2)) & 0xF;
}

static void PutObjPixel(int x, unsigned int texel, uint16_t color, unsigned int priority, int semi)
{
    if (sObjPriority[x] <= priority)
        return;
    if (texel) {
        sObjColor[x] = color | OPAQUE;
        sObjPriority[x] = (uint8_t)priority;
        sObjSemi[x] = (uint8_t)semi;
    } else if (sObjPriority[x] != NO_OBJ) {
        // The OBJ priority quirk: the pixel below takes this sprite's priority.
        sObjPriority[x] = (uint8_t)priority;
        sObjSemi[x] = (uint8_t)semi;
    }
}

// palette: the palette RAM index of the sprite's colour 0.
static void PutObj(int x, unsigned int texel, unsigned int palette, unsigned int priority, int mode)
{
    if (mode == OBJ_MODE_WINDOW) {
        if (texel)
            sObjWindow[x] = 1;
    } else {
        PutObjPixel(x, texel, texel ? Palette(palette + texel) : 0, priority, mode == OBJ_MODE_SEMI);
    }
}

// Draws one sprite on a line. y is the line, or the first line of the mosaic block.
static void DrawSprite(uint16_t a, uint16_t b, uint16_t c, int y, uint16_t dispcnt, uint16_t mosaic)
{
    int width = kObjSizes[(a >> 14) * 4 + (b >> 14)][0];
    int height = kObjSizes[(a >> 14) * 4 + (b >> 14)][1];
    int spriteY = a & 0xFF;
    int x0 = (int)(b & 0x1FF) - ((b & 0x100) ? 0x200 : 0);
    int mode = OBJ_MODE(a);
    unsigned int priority = (c >> 10) & 3;
    unsigned int tile = c & 0x3FF;
    int mosaicH = (a & OBJ_MOSAIC) ? ((mosaic >> 8) & 0xF) + 1 : 1;
    unsigned int palette;
    struct ObjTiles t;
    int inY, outX, end;

    // In the bitmap modes the first half of sprite VRAM holds the bitmap.
    if (DISPCNT_MODE(dispcnt) >= 3 && tile < 512)
        return;

    t.color256 = (a & OBJ_256_COLORS) != 0;
    if (dispcnt & DISPCNT_OBJ_1D) {
        t.base = tile * 0x20;
        t.stride = (uint32_t)(t.color256 ? width : width / 2);
        t.wrap = 0x7FFE;
        t.rowBase = 0;
    } else {
        t.base = (tile & ~(unsigned int)t.color256) * 0x20;
        t.stride = 0x80;
        t.wrap = 0x3FE;
        t.rowBase = t.base & 0x7C00;
    }
    palette = t.color256 ? OBJ_PALETTE : OBJ_PALETTE + (c >> 12) * 16;

    inY = y - spriteY;
    if (a & OBJ_AFFINE) {
        unsigned int matrix = ((b >> 9) & 0x1F) * 32;
        int32_t pa = (int16_t)Oam16(matrix + 6), pb = (int16_t)Oam16(matrix + 14);
        int32_t pc = (int16_t)Oam16(matrix + 22), pd = (int16_t)Oam16(matrix + 30);
        int boxW = (a & OBJ_DOUBLE_SIZE) ? width * 2 : width;
        int boxH = (a & OBJ_DOUBLE_SIZE) ? height * 2 : height;
        int lx, ly;

        if (inY < 0)
            inY += 256;
        outX = x0 > 0 ? x0 : 0;
        end = x0 + boxW < WIDTH ? x0 + boxW : WIDTH;
        if (mosaicH > 1 && end != WIDTH && end % mosaicH)
            end += mosaicH - end % mosaicH;
        // With mosaic, the texel is held from the start of each block.
        lx = (pa * (outX - x0 - 1 - boxW / 2) + pb * (inY - boxH / 2) + (width << 7)) >> 8;
        ly = (pc * (outX - x0 - 1 - boxW / 2) + pd * (inY - boxH / 2) + (height << 7)) >> 8;
        for (; outX < end; outX++) {
            int inX = outX - x0;
            if (mosaicH == 1 || outX % mosaicH == 0) {
                lx = (pa * (inX - boxW / 2) + pb * (inY - boxH / 2) + (width << 7)) >> 8;
                ly = (pc * (inX - boxW / 2) + pd * (inY - boxH / 2) + (height << 7)) >> 8;
            }
            if (lx < 0 || ly < 0 || lx >= width || ly >= height)
                continue;
            PutObj(outX, ObjTexel(&t, lx, ly), palette, priority, mode);
        }
    } else {
        int inX, step = 1;

        if (spriteY + height - 256 >= 0)
            inY += 256;
        if (b & OBJ_VFLIP)
            inY = height - inY - 1;
        outX = x0 > 0 ? x0 : 0;
        end = x0 + width;
        if (end % mosaicH)
            end += mosaicH - end % mosaicH;
        if (end > WIDTH)
            end = WIDTH;
        inX = outX - x0;
        if (b & OBJ_HFLIP) {
            inX = width - inX - 1;
            step = -1;
        }
        for (; outX < end; outX++, inX += step) {
            int lx = inX - step * (outX % mosaicH);
            if (lx < 0)
                lx = 0;
            else if (lx > width - 1)
                lx = width - 1;
            PutObj(outX, ObjTexel(&t, lx, inY), palette, priority, mode);
        }
    }
}

static void DrawSprites(unsigned int line, uint16_t dispcnt, uint16_t mosaic)
{
    int cycles = (dispcnt & DISPCNT_HBLANK_FREE) ? OBJ_CYCLES_HBLANK_FREE : OBJ_CYCLES;
    int mosaicV = ((mosaic >> 12) & 0xF) + 1;
    int y = (int)line, i;

    memset(sObjPriority, NO_OBJ, sizeof(sObjPriority));
    memset(sObjWindow, 0, sizeof(sObjWindow));
    if (!(dispcnt & DISPCNT_OBJ))
        return;

    for (i = 0; i < 128; i++) {
        uint16_t a = Oam16(i * 8), b = Oam16(i * 8 + 2), c = Oam16(i * 8 + 4);
        int width = kObjSizes[(a >> 14) * 4 + (b >> 14)][0];
        int height = kObjSizes[(a >> 14) * 4 + (b >> 14)][1];
        int spriteY = a & 0xFF, spriteX = b & 0x1FF, cost = width, endY, localY = y;

        if (!(a & OBJ_AFFINE) && (a & OBJ_DOUBLE_SIZE))
            continue;  // hidden
        if (a & OBJ_AFFINE) {
            if (a & OBJ_DOUBLE_SIZE) {
                width *= 2;
                height *= 2;
            }
            cost = 10 + width * 2;
        }
        // Off screen: these take no time.
        if (spriteY >= HEIGHT && spriteY + height < 228)
            continue;
        if (spriteX >= WIDTH && spriteX + width < 512)
            continue;
        endY = spriteY + height;
        if ((y < spriteY && (endY - 256 < 0 || y >= endY - 256)) || y >= endY)
            continue;
        if ((a & OBJ_MOSAIC) && mosaicV > 1) {
            localY = y - y % mosaicV;
            if (localY < spriteY && spriteY < HEIGHT)
                localY = spriteY;
            if (localY >= (endY & 0xFF))
                localY = endY - 1;
        }
        DrawSprite(a, b, c, localY, dispcnt, mosaic);
        cycles -= cost;
        if (cycles <= 0)
            break;
    }
}

// --- Windows -------------------------------------------------------------------------------

// Whether v is inside a window's range, from its register (start in the high byte, end in the
// low byte). mGBA clamps the values to the screen; an end before the start wraps around.
static int InWindowRange(unsigned int v, uint16_t reg, unsigned int limit)
{
    unsigned int start = reg >> 8, end = reg & 0xFF;

    if (start > limit && start > end)
        start = 0;
    if (end > limit) {
        end = limit;
        if (start > limit)
            start = limit;
    }
    if (end < start)
        return v < end || v >= start;
    return v >= start && v < end;
}

static void ComputeWindows(unsigned int line, uint16_t dispcnt)
{
    uint16_t winin = Reg(REG_WININ), winout = Reg(REG_WINOUT);
    int in0 = (dispcnt & DISPCNT_WIN0) && InWindowRange(line, Reg(REG_WIN0V), HEIGHT);
    int in1 = (dispcnt & DISPCNT_WIN1) && InWindowRange(line, Reg(REG_WIN1V), HEIGHT);
    uint16_t win0H = Reg(REG_WIN0H), win1H = Reg(REG_WIN1H);
    unsigned int x;

    if (!(dispcnt & (DISPCNT_WIN0 | DISPCNT_WIN1 | DISPCNT_OBJWIN))) {
        memset(sWindow, 0x3F, sizeof(sWindow));
        memset(sSpan, 0x3F, sizeof(sSpan));
        return;
    }
    // WIN0 is above WIN1, which is above the OBJ window.
    for (x = 0; x < WIDTH; x++) {
        if (in0 && InWindowRange(x, win0H, WIDTH)) {
            sSpan[x] = sWindow[x] = winin & 0x3F;
        } else if (in1 && InWindowRange(x, win1H, WIDTH)) {
            sSpan[x] = sWindow[x] = (winin >> 8) & 0x3F;
        } else {
            sSpan[x] = sWindow[x] = winout & 0x3F;
            if ((dispcnt & DISPCNT_OBJWIN) && sObjWindow[x])
                sWindow[x] = ((winout >> 8) & 0x3F) | IN_OBJ_WINDOW;
        }
    }
}

// --- Composition ---------------------------------------------------------------------------

static void Compose(uint32_t *row, uint16_t dispcnt, const int *drawn)
{
    uint16_t bldcnt = Reg(REG_BLDCNT), bldalpha = Reg(REG_BLDALPHA);
    unsigned int target1 = bldcnt & 0x3F, target2 = (bldcnt >> 8) & 0x3F, effect = (bldcnt >> 6) & 3;
    unsigned int eva = bldalpha & 0x1F, evb = (bldalpha >> 8) & 0x1F, evy = Reg(REG_BLDY) & 0x1F;
    int brightness = effect == BLEND_BRIGHTEN || effect == BLEND_DARKEN;
    int identity, anyTarget2, objWindowEffects, n;
    unsigned int priority[4];
    uint16_t backdrop = Palette(0);
    unsigned int x;

    if (eva > 16)
        eva = 16;
    if (evb > 16)
        evb = 16;
    if (evy > 16)
        evy = 16;
    // Alpha blending that leaves the first target as it is: mGBA marks no first targets then.
    identity = effect == BLEND_ALPHA && eva == 16 && evb == 0;
    // Whether semi-transparent sprites have anything to blend with: a second target that is
    // the backdrop or a background DISPCNT has turned on.
    anyTarget2 = (target2 >> LAYER_BACKDROP) & 1;
    for (n = 0; n < 4; n++) {
        priority[n] = Reg(REG_BG0CNT + 2 * n) & 3;
        if (((target2 >> n) & 1) && sBg[n].enabled)
            anyTarget2 = 1;
    }
    objWindowEffects = (dispcnt & DISPCNT_OBJWIN) && (Reg(REG_WINOUT) & (WINDOW_EFFECTS << 8));

    for (x = 0; x < WIDTH; x++) {
        unsigned int control = sWindow[x];
        int effects = (control & WINDOW_EFFECTS) != 0, spanEffects = (sSpan[x] & WINDOW_EFFECTS) != 0;
        int layers[2], count = 0, p, i;
        int objFirst = 0, objReblend = 0, objVariant = 0;
        uint32_t colors[2];
        int first1 = 0, second2 = 0;

        // The two topmost layers. A sprite goes in front of the backgrounds of its priority.
        for (p = 0; p < 4 && count < 2; p++) {
            if (sObjPriority[x] == p && (sObjColor[x] & OPAQUE) && (control & (1u << LAYER_OBJ)))
                layers[count++] = LAYER_OBJ;
            for (n = 0; n < 4 && count < 2; n++)
                if (drawn[n] && priority[n] == (unsigned int)p && (control & (1u << n)) && (sBgLine[n][x] & OPAQUE))
                    layers[count++] = n;
        }
        while (count < 2)
            layers[count++] = LAYER_BACKDROP;

        if (layers[0] == LAYER_OBJ || layers[1] == LAYER_OBJ) {
            int semi = sObjSemi[x];
            int objTarget1 = (target1 >> LAYER_OBJ) & 1;
            // The OBJ window and the window below it disagree about effects.
            int split = (dispcnt & DISPCNT_OBJWIN) && objWindowEffects != spanEffects;

            objFirst = (spanEffects && objTarget1 && effect == BLEND_ALPHA) || semi;
            objVariant = objTarget1 && spanEffects && brightness;
            if (semi || (objTarget1 && effect == BLEND_ALPHA) || split) {
                if (anyTarget2) {
                    objReblend = 1;
                    objVariant = 0;
                } else {
                    objFirst = 0;
                }
            }
            if (control & IN_OBJ_WINDOW)
                objVariant = objVariant && objWindowEffects;
        }

        for (i = 0; i < 2; i++) {
            int layer = layers[i], variant;
            if (layer == LAYER_OBJ) {
                colors[i] = Expand(sObjColor[x] & 0x7FFF);
                variant = objVariant;
            } else if (layer == LAYER_BACKDROP) {
                colors[i] = Expand(backdrop);
                variant = ((target1 >> LAYER_BACKDROP) & 1) && brightness && spanEffects;
            } else {
                colors[i] = Expand(sBgLine[layer][x] & 0x7FFF);
                variant = ((target1 >> layer) & 1) && brightness && effects;
            }
            if (variant)
                colors[i] = effect == BLEND_BRIGHTEN ? Brighten(colors[i], evy) : Darken(colors[i], evy);
        }

        if (layers[0] == LAYER_OBJ)
            first1 = objFirst;
        else if (layers[0] != LAYER_BACKDROP)
            first1 = effect == BLEND_ALPHA && !identity && ((target1 >> layers[0]) & 1) && effects;
        second2 = (target2 >> layers[1]) & 1;

        if (first1 && second2)
            row[x] = Mix(colors[0], eva, colors[1], evb);
        else if (layers[0] == LAYER_OBJ && objReblend && brightness && effects)
            row[x] = effect == BLEND_BRIGHTEN ? Brighten(colors[0], evy) : Darken(colors[0], evy);
        else
            row[x] = colors[0];
    }
}

// --- Lines and frames ----------------------------------------------------------------------

// DISPCNT turning backgrounds on or off. frameStart: before line 0 is drawn.
static void EnableBackgrounds(uint16_t dispcnt, int frameStart)
{
    int n;

    for (n = 0; n < 4; n++) {
        int active = (dispcnt & DISPCNT_BG(n)) != 0, was = sBg[n].enabled;
        if (!active) {
            if (frameStart || (was > 0 && was < ENABLED))
                sBg[n].enabled = 0;
            else if (was == ENABLED)
                sBg[n].enabled = -2;
        } else if (!was) {
            sBg[n].enabled = frameStart ? ENABLED : DISPCNT_MODE(dispcnt) > 2 ? 2 : 1;
        } else if (was < 0) {
            sBg[n].enabled = ENABLED;
        }
    }
}

static void DrawLine(unsigned int line)
{
    uint16_t dispcnt = Reg(REG_DISPCNT) & 0xFFF7;
    uint16_t mosaic = Reg(REG_MOSAIC);
    unsigned int mode = DISPCNT_MODE(dispcnt);
    unsigned int writes = PortIoTakeAffineWrites();
    uint32_t *row = &sFrame[line * WIDTH];
    int drawn[4] = { 0, 0, 0, 0 };
    int n;

    // Register writes since the last line. mGBA acts on each one as it happens; here they are
    // taken in at the next line, so two changes to DISPCNT within one line count as one.
    if (dispcnt != sDispcnt) {
        sDispcnt = dispcnt;
        EnableBackgrounds(dispcnt, line == 0);
    }
    for (n = 0; n < 4; n++) {
        if (writes & (1u << n)) {
            int bg = 2 + n / 2;
            unsigned int reg = REG_BG2X + (bg - 2) * AFFINE_STRIDE + (n & 1) * 4;
            if (n & 1)
                sBg[bg].y = AffineReference(reg);
            else
                sBg[bg].x = AffineReference(reg);
        }
    }
    EnableBackgrounds(dispcnt, line == HEIGHT - 1);

    if (dispcnt & DISPCNT_FORCED_BLANK) {
        for (n = 0; n < WIDTH; n++)
            row[n] = 0xFFFFFF;
        return;
    }

    for (n = 0; n < 4; n++) {
        if (sBg[n].enabled != ENABLED)
            continue;
        if (mode == 0 || (mode == 1 && n < 2)) {
            DrawTextBackground(n, line, mosaic, sBgLine[n]);
            drawn[n] = 1;
        } else if ((mode == 1 && n == 2) || (mode == 2 && n >= 2)) {
            DrawAffineBackground(n, line, mosaic, sBgLine[n]);
            drawn[n] = 1;
        } else if (mode >= 3 && mode <= 5 && n == 2) {
            DrawBitmapBackground(dispcnt, line, mosaic, sBgLine[n]);
            drawn[n] = 1;
        }
    }
    DrawSprites(line, dispcnt, mosaic);
    ComputeWindows(line, dispcnt);
    Compose(row, dispcnt, drawn);

    // The affine reference points move on by (PB, PD) for each line a background is drawn.
    if (mode != 0) {
        for (n = 2; n < 4; n++) {
            if (sBg[n].enabled == ENABLED) {
                unsigned int affine = REG_BG2PA + (n - 2) * AFFINE_STRIDE;
                sBg[n].x += (int16_t)Reg(affine + 2);
                sBg[n].y += (int16_t)Reg(affine + 6);
            }
        }
    }
    for (n = 0; n < 4; n++)
        if (sBg[n].enabled != 0 && sBg[n].enabled < ENABLED)
            sBg[n].enabled++;
}

// The start of VBlank: the affine reference points are reloaded, and backgrounds turned on
// during the frame are shown from the next one.
static void FinishFrame(void)
{
    int n;

    for (n = 2; n < 4; n++) {
        sBg[n].x = AffineReference(REG_BG2X + (n - 2) * AFFINE_STRIDE);
        sBg[n].y = AffineReference(REG_BG2X + (n - 2) * AFFINE_STRIDE + 4);
    }
    for (n = 0; n < 4; n++)
        if (sBg[n].enabled > 0)
            sBg[n].enabled = ENABLED;
}

void PortPpuReset(enum PortPpuColors colors)
{
    unsigned int i;

    sColors = colors;
    memset(sBg, 0, sizeof(sBg));
    sDispcnt = 0x0080;
    for (i = 0; i < WIDTH * HEIGHT; i++)
        sFrame[i] = 0xFFFFFF;
    (void)PortIoTakeAffineWrites();
}

void PortPpuHBlank(unsigned int line)
{
    if (line < HEIGHT)
        DrawLine(line);
    else if (line == HEIGHT)
        FinishFrame();
}

const uint32_t *PortPpuFrame(void)
{
    return sFrame;
}

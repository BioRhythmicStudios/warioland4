#ifndef GUARD_GBA_DEFINES
#define GUARD_GBA_DEFINES

#define TRUE  1
#define FALSE 0
#define NULL ((void*)0)

#define IWRAM_DATA __attribute__((section("iwram_data")))
#define EWRAM_DATA __attribute__((section("ewram_data")))

#define NAKED __attribute__((naked))
#define UNUSED __attribute__((unused))

#define ALIGNED(n) __attribute__((aligned(n)))

// Pins a local to a register so agbcc allocates it as the original did.
// Portable builds drop the pin and let the compiler choose.
#ifdef PORTABLE
#define PIN(reg)
#else
#define PIN(reg) asm(reg)
#endif

// A few matched functions are declared non-void but return without a value, and every
// caller ignores the result. Portable builds return the given value so the compiler accepts them.
#ifdef PORTABLE
#define RETURN_NO_VALUE(value) return value
#else
#define RETURN_NO_VALUE(value) return
#endif

// Empty asm statements that steer agbcc's register allocation and scheduling. The operands
// follow the template, e.g. ASM_BARRIER(: "+r"(x)). Portable builds keep them, because some
// carry a value: ASM_BARRIER(: "=r"(x) : "0"(y)) sets x to y. ASM_BARRIER_ARM is for the
// ones that name ARM registers and carry no value; portable builds drop those.
#define ASM_BARRIER(...) asm("" __VA_ARGS__)
#define ASM_BARRIER_VOLATILE(...) asm volatile("" __VA_ARGS__)
#ifdef PORTABLE
#define ASM_BARRIER_ARM(...)
#define ASM_BARRIER_ARM_VOLATILE(...)
#else
#define ASM_BARRIER_ARM(...) asm("" __VA_ARGS__)
#define ASM_BARRIER_ARM_VOLATILE(...) asm volatile("" __VA_ARGS__)
#endif

// GBA_CALL_AS(func, type) is matched code calling func through a cast function type, e.g. to
// read a u8 result as u32. agbcc passes and returns every integer in a full register, so on the
// GBA the cast only steers code generation. On 64-bit ABIs the upper bits of narrow values are
// undefined, so portable builds call func through its real prototype.
#ifdef PORTABLE
#define GBA_CALL_AS(func, ...) (func)
#else
#define GBA_CALL_AS(func, ...) ((__VA_ARGS__)(func))
#endif

#define SOUND_INFO_PTR (*(GBA_PTR(struct SoundInfo *) *)0x3007FF0)
#define INTR_CHECK     (*(u16 *)0x3007FF8)
#define INTR_VECTOR    (*(GBA_PTR(void *) *)0x3007FFC)

#define EWRAM_START 0x02000000
#define EWRAM_SIZE  0x40000
#define EWRAM_END   (EWRAM_START + EWRAM_SIZE)
#define IWRAM_START 0x03000000
#define IWRAM_SIZE  0x8000
#define IWRAM_END   (IWRAM_START + IWRAM_SIZE)

#define PLTT      0x5000000
#define PLTT_SIZE 0x400

#define BG_PLTT      PLTT
#define BG_PLTT_SIZE 0x200

#define OBJ_PLTT      (PLTT + 0x200)
#define OBJ_PLTT_SIZE 0x200

#define VRAM      0x6000000
#define VRAM_SIZE 0x18000

#define BG_VRAM           VRAM
#define BG_VRAM_SIZE      0x10000
#define BG_CHAR_SIZE      0x4000
#define BG_SCREEN_SIZE    0x800
#define BG_CHAR_ADDR(n)   (void *)(BG_VRAM + (BG_CHAR_SIZE * (n)))
#define BG_SCREEN_ADDR(n) (void *)(BG_VRAM + (BG_SCREEN_SIZE * (n)))
#define BG_TILE_ADDR(n)   (void *)(BG_VRAM + (0x80 * (n)))

#define BG_TILE_H_FLIP(n) (0x400 + (n))
#define BG_TILE_V_FLIP(n) (0x800 + (n))

// text-mode BG
#define OBJ_VRAM0      (void *)(VRAM + 0x10000)
#define OBJ_VRAM0_SIZE 0x8000

// bitmap-mode BG
#define OBJ_VRAM1      (void *)(VRAM + 0x14000)
#define OBJ_VRAM1_SIZE 0x4000

#define OAM      0x7000000
#define OAM_SIZE 0x400

#define ROM_HEADER_SIZE   0xC0

#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 160

#define TILE_SIZE_4BPP 32
#define TILE_SIZE_8BPP 64

#define TILE_OFFSET_4BPP(n) ((n) * TILE_SIZE_4BPP)
#define TILE_OFFSET_8BPP(n) ((n) * TILE_SIZE_8BPP)

#define TOTAL_OBJ_TILE_COUNT 1024

#define PLTT_ROW_SIZE 32

#define RGB(r, g, b) ((r) | ((g) << 5) | ((b) << 10))
#define RGB2(r, g, b) (((b) << 10) | ((g) << 5) | (r))
#define _RGB(r, g, b) ((((b) & 0x1F) << 10) + (((g) & 0x1F) << 5) + ((r) & 0x1F))

#define RGB_BLACK RGB(0, 0, 0)
#define RGB_WHITE RGB(31, 31, 31)
#define RGB_RED RGB(31, 0, 0)
#define RGB_GREEN RGB(0, 31, 0)
#define RGB_BLUE RGB(0, 0, 31)
#define RGB_YELLOW RGB(31, 31, 0)
#define RGB_MAGENTA RGB(31, 0, 31)
#define RGB_CYAN RGB(0, 31, 31)
#define RGB_WHITEALPHA (RGB_WHITE | 0x8000)

#define SYSTEM_CLOCK           (16 * 1024 * 1024)   // System Clock

#endif // GUARD_GBA_DEFINES

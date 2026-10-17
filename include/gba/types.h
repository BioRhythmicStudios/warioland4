#ifndef GUARD_GBA_TYPES_H
#define GUARD_GBA_TYPES_H

// agbcc aligns every struct and union to at least 4 bytes, so their sizes are multiples of 4.
// Every definition carries AGB_ALIGN (tools/port_struct_align_codemod.py) so that portable
// builds lay them out the same way.
#ifdef PORTABLE
#define AGB_ALIGN __attribute__((aligned(4)))
#else
#define AGB_ALIGN
#endif

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

typedef volatile u8   vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile s8   vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;

typedef float  f32;
typedef double f64;

typedef u8  bool8;
typedef u16 bool16;
typedef u32 bool32;

#define S8_MIN (-128)
#define S8_MAX (127)
#define U8_MAX (255)

#define S16_MIN (-32768)
#define S16_MAX (32767)
#define U16_MAX (65535)

#define S32_MIN (-2147483648)
#define S32_MAX (2147483647)
#define U32_MAX (4294967295ul)

#define S64_MIN (-9223372036854775808ll)
#define S64_MAX (9223372036854775807ll)
#define U64_MAX (18446744073709551615ull)

struct AGB_ALIGN BgCnt {
    u16 priority:2;
    u16 charBaseBlock:2;
    u16 dummy:2;
    u16 mosaic:1;
    u16 palettes:1;
    u16 screenBaseBlock:5;
    u16 areaOverflowMode:1;
    u16 screenSize:2;
};
typedef volatile struct BgCnt vBgCnt;

struct AGB_ALIGN PlttData {
    u16 r:5; // red
    u16 g:5; // green
    u16 b:5; // blue
    u16 unused_15:1;
};

typedef union AGB_ALIGN {
    struct AGB_ALIGN {
    /*0x00*/ u32 y:8;
    /*0x01*/ u32 affineMode:2;  // 0x1, 0x2 -> 0x4
             u32 objMode:2;     // 0x4, 0x8 -> 0xC
             u32 mosaic:1;      // 0x10
             u32 bpp:1;         // 0x20
             u32 shape:2;       // 0x40, 0x80 -> 0xC0

    /*0x02*/ u32 x:9;
             u32 matrixNum:5;   // bits 3/4 are h-flip/v-flip if not in affine mode
             u32 size:2;        // 0x4000, 0x8000 -> 0xC000

    /*0x04*/ u16 tileNum:10;    // 0x3FF
             u16 priority:2;    // 0x400, 0x800 -> 0xC00
             u16 paletteNum:4;

    /*0x06*/ u16 fractional:8;
             u16 integer:7;
             u16 sign:1;
    } split;

    struct AGB_ALIGN {
        u16 attr0;
        u16 attr1;
        u16 attr2;
        u16 affineParam;
    } all;
} OamData;

#define ST_OAM_HFLIP     0x08
#define ST_OAM_VFLIP     0x10
#define ST_OAM_MNUM_FLIP_MASK 0x18

#define ST_OAM_OBJ_NORMAL 0
#define ST_OAM_OBJ_BLEND  1
#define ST_OAM_OBJ_WINDOW 2

#define ST_OAM_AFFINE_OFF    0
#define ST_OAM_AFFINE_NORMAL 1
#define ST_OAM_AFFINE_ERASE  2
#define ST_OAM_AFFINE_DOUBLE 3

#define ST_OAM_AFFINE_ON_MASK     1
#define ST_OAM_AFFINE_DOUBLE_MASK 2

#define ST_OAM_4BPP 0
#define ST_OAM_8BPP 1

#define ST_OAM_SQUARE      0
#define ST_OAM_H_RECTANGLE 1
#define ST_OAM_V_RECTANGLE 2

#define ST_OAM_SIZE_0   0
#define ST_OAM_SIZE_1   1
#define ST_OAM_SIZE_2   2
#define ST_OAM_SIZE_3   3

#define SPRITE_SIZE_8x8     ((ST_OAM_SIZE_0 << 2) | (ST_OAM_SQUARE))
#define SPRITE_SIZE_16x16   ((ST_OAM_SIZE_1 << 2) | (ST_OAM_SQUARE))
#define SPRITE_SIZE_32x32   ((ST_OAM_SIZE_2 << 2) | (ST_OAM_SQUARE))
#define SPRITE_SIZE_64x64   ((ST_OAM_SIZE_3 << 2) | (ST_OAM_SQUARE))

#define SPRITE_SIZE_16x8    ((ST_OAM_SIZE_0 << 2) | (ST_OAM_H_RECTANGLE))
#define SPRITE_SIZE_32x8    ((ST_OAM_SIZE_1 << 2) | (ST_OAM_H_RECTANGLE))
#define SPRITE_SIZE_32x16   ((ST_OAM_SIZE_2 << 2) | (ST_OAM_H_RECTANGLE))
#define SPRITE_SIZE_64x32   ((ST_OAM_SIZE_3 << 2) | (ST_OAM_H_RECTANGLE))

#define SPRITE_SIZE_8x16    ((ST_OAM_SIZE_0 << 2) | (ST_OAM_V_RECTANGLE))
#define SPRITE_SIZE_8x32    ((ST_OAM_SIZE_1 << 2) | (ST_OAM_V_RECTANGLE))
#define SPRITE_SIZE_16x32   ((ST_OAM_SIZE_2 << 2) | (ST_OAM_V_RECTANGLE))
#define SPRITE_SIZE_32x64   ((ST_OAM_SIZE_3 << 2) | (ST_OAM_V_RECTANGLE))

#define SPRITE_SIZE(dim)  ((dim >> 2) & 0x03)
#define SPRITE_SHAPE(dim) (dim & 0x03)

struct AGB_ALIGN BgAffineSrcData {
    s32 texX;
    s32 texY;
    s16 scrX;
    s16 scrY;
    s16 sx;
    s16 sy;
    u16 alpha;
};

struct AGB_ALIGN BgAffineDstData {
    s16 pa;
    s16 pb;
    s16 pc;
    s16 pd;
    s32 dx;
    s32 dy;
};

struct AGB_ALIGN ObjAffineSrcData {
    s16 xScale;
    s16 yScale;
    u16 rotation;
};

// Normal SIO Control Structure
struct AGB_ALIGN SioNormalCnt {
    u16 sck_I_O:1;          // Clock I/O Select
    u16 sck:1;              // Internal Clock Select
    u16 ackRecv:1;          // Transfer Enable Flag Receive
    u16 ackSend:1;          // Transfer Enable Flag Send
    u16 unused_6_4:3;
    u16 enable:1;           // SIO Enable
    u16 unused_11_8:4;
    u16 mode:2;             // Communication Mode Select
    u16 ifEnable:1;         // Interrupt Request Enable
    u16 unused_15:1;
    u8  data;               // Data
    u8  unused_31_24;
};

#define ST_SIO_8BIT_MODE            0       // Normal 8-bit communication mode
#define ST_SIO_32BIT_MODE           1       // Normal 32-bit communication mode

#define ST_SIO_SCK_OUT              0       // Select external clock
#define ST_SIO_SCK_IN               1       // Select internal clock
#define ST_SIO_IN_SCK_256K          0       // Select internal clock 256KHz
#define ST_SIO_IN_SCK_2M            1       //                  Select 2MHz

// Multi-player SIO Control Structure
struct AGB_ALIGN SioMultiCnt {
    u16 baudRate:2;    // baud rate
    u16 si:1;          // SI terminal
    u16 sd:1;          // SD terminal
    u16 id:2;          // ID
    u16 error:1;       // error flag
    u16 enable:1;      // SIO enable
    u16 unused_11_8:4;
    u16 mode:2;        // communication mode (should equal 2)
    u16 ifEnable:1;  // IRQ enable
    u16 unused_15:1;
    u16 data;          // data
};

#define ST_SIO_MULTI_MODE           2       // Multi-play communication mode

#define ST_SIO_9600_BPS             0       // Baud rate 9600 bps
#define ST_SIO_38400_BPS            1       //          38400 bps
#define ST_SIO_57600_BPS            2       //          57600 bps
#define ST_SIO_115200_BPS           3       //         115200 bps
#define ST_SIO_MULTI_PARENT         1       // Multi-play communication  Connect master
#define ST_SIO_MULTI_CHILD          0       //                  Connect slave

// UART - SIO Control Structure
struct AGB_ALIGN SioUartCnt {
    u16 baudRate:2;         // Baud Rate
    u16 ctsEnable:1;        // Send Signal Enable
    u16 paritySelect:1;     // Parity Even/Odd
    u16 transDataFull:1;    // Transmit Data Full
    u16 recvDataEmpty:1;    // Receive Data Empty
    u16 error:1;            // Error Detect
    u16 length:1;           // Data Length
    u16 fifoEnable:1;       // FIFO Enable
    u16 parityEnable:1;     // Parity Enable
    u16 transEnable:1;      // Transmitter Enable
    u16 recvEnable:1;       // Receiver Enable
    u16 mode:2;             // Communication Mode Select
    u16 ifEnable:1;         // Interrupt Request Enable
    u16 unused_15:1;
    u8  data;               // Data
    u8  unused_31_24;
};

#define ST_SIO_UART_MODE            3       // UART communication mode

#define ST_SIO_UART_7BIT            0       // UART communication data length 7 bits
#define ST_SIO_UART_8BIT            1       //                       8 bits
#define ST_SIO_PARITY_EVEN          0       // Even parity
#define ST_SIO_PARITY_ODD           1       // Odd parity

// JOY Bus Communication Control Structure
struct AGB_ALIGN JoyCnt {
    u8  ifReset:1;         // JOY Bus Reset Interrupt Request
    u8  ifRecv:1;          // JOY Bus Received Interrupt Request
    u8  ifSend:1;          // JOY Bus Sent Interrupt Request
    u8  unused_5_3:3;
    u8  ifEnable:1;        // Interrupt Request Enable
    u8  unused_7:1;
};

// JOY Bus Communication Status Structure
struct AGB_ALIGN JoyStat {
    u8  unused_0:1;
    u8  recv:1;             // Receive Status
    u8  unused_2:1;
    u8  send:1;             // Send Status
    u8  flags:2;            // General Flag
    u8  unused_7_6:2;
};

// General Input/Output Control Structure
struct AGB_ALIGN RCnt {
    u8  sc:1;               // Data
    u8  sd:1;
    u8  si:1;
    u8  so:1;
    u8  sc_i_o:1;           // I/O Select
    u8  sd_i_o:1;
    u8  si_i_o:1;
    u8  so_i_o:1;
    u8  ifEnable:1;         // Interrupt Request Enable
    u8  unused_13_9:5;
    u8  sioModeMaster:2;    // SIO Mode Master
};

#define ST_R_SIO_MASTER_MODE        0       // SIO master mode
#define ST_R_DIRECT_MODE            2       // General input/output communication mode
#define ST_R_JOY_MODE               3       // JOY communication mode

#define ST_R_IN                     0       // Select input
#define ST_R_OUT                    1       // Select output

#include "gba/gba_ptr.h"

#endif // GUARD_GBA_TYPES_H

#include "background_registers.h"
#include "color_effects.h"
#include "compiler.h"
#include "game_screen_helpers.h"
#include "global_data.h"
#include "hblank.h"
#include "main.h"
#include "score.h"
#include "scrolling.h"
#include "sound.h"
#include "wario.h"

struct AGB_ALIGN GrayscaleWeights {
    s32 red;
    s32 green;
    s32 blue;
};

struct AGB_ALIGN PaletteFadeLoopState {
    s32 nextBank;
    s32 count;
};

struct AGB_ALIGN PaletteBufferPair {
    GBA_PTR(u16*) dst;
    GBA_PTR(u16*) srcB;
};

typedef u16 (*ColorBlendFunction)(u32, u32, u32, u32);

#ifdef PORTABLE
#include <string.h>
#else
void* memcpy(void* destination, const void* source, unsigned long size);
#endif

extern const struct GrayscaleWeights sGrayscaleWeights;
extern ColorBlendFunction sColorBlendFunctionTable[];
#ifdef PORTABLE
// On the GBA this follows sColorBlendFunctionTable directly (src/data/color_fading_pointers.c).
extern ColorBlendFunction sColorBlendFunctionTableTail[];
#endif
extern const s8 sRoomPaletteColorOffsets[];
extern const u8 sPaletteFadeAmounts[];
extern const u8 sPaletteFadeStepLimits[];
extern const s8 sVerticalWindowFadeSteps[18];

void BackupPalettesForColorFade(void)
{
    BackupPalettesToSecondaryBuffers();
    WaitForDma3Transfer();
    BackupPalettesToPrimaryBuffers();
}

void BackupPalettesToSecondaryBuffers(void)
{
    vu32* dma;
    u32 control;

    dma = (vu32*)0x040000D4;
    dma[0] = 0x05000000;
    dma[1] = 0x02020400;
    control = 0x80000100;
    dma[2] = control;
    WaitForDma3Transfer(dma[2]);
    dma[0] = 0x05000200;
    dma[1] = 0x02020600;
    dma[2] = control;
    dma[2];
}

void BackupPalettesToPrimaryBuffers(void)
{
    vu32* dma;
    u32 control;

    dma = (vu32*)0x040000D4;
    dma[0] = 0x05000000;
    dma[1] = 0x02020000;
    control = 0x80000100;
    dma[2] = control;
    WaitForDma3Transfer(dma[2]);
    dma[0] = 0x05000200;
    dma[1] = 0x02020200;
    dma[2] = control;
    dma[2];
}

void CopySecondaryPalettesToPrimaryBuffers(void)
{
    vu32* dma;
    u32 control;

    dma = (vu32*)0x040000D4;
    dma[0] = 0x02020400;
    dma[1] = 0x02020000;
    control = 0x80000100;
    dma[2] = control;
    WaitForDma3Transfer(dma[2]);
    dma[0] = 0x02020600;
    dma[1] = 0x02020200;
    dma[2] = control;
    dma[2];
}

void CopyAlternatePalettesToPrimaryBuffers(void)
{
    vu32* dma;
    u32 control;

    dma = (vu32*)0x040000D4;
    dma[0] = 0x02020800;
    dma[1] = 0x02020000;
    control = 0x80000100;
    dma[2] = control;
    WaitForDma3Transfer(dma[2]);
    dma[0] = 0x02020A00;
    dma[1] = 0x02020200;
    dma[2] = control;
    dma[2];
}

void CopyAlternateBgAndSecondaryObjPalettesToPrimaryBuffers(void)
{
    vu32* dma;
    u32 control;

    dma = (vu32*)0x040000D4;
    dma[0] = 0x02020800;
    dma[1] = 0x02020000;
    control = 0x80000100;
    dma[2] = control;
    WaitForDma3Transfer(dma[2]);
    dma[0] = 0x02020600;
    dma[1] = 0x02020200;
    dma[2] = control;
    dma[2];
}

void BuildGrayscaleBgPaletteBuffer(void)
{
    struct GrayscaleWeights weights;
    s32 redWeight;
    s32 greenWeight;
    s32 blueWeight;
    u16* destination;
    const u16* source;
    s32 remaining;

    weights = sGrayscaleWeights;
    destination = (u16*)0x02020800;
    source = (const u16*)0x02020400;
    redWeight = weights.red;
    greenWeight = weights.green;
    blueWeight = weights.blue;
    remaining = 255;
    do {
        u32 color;
        s32 red;
        register s32 green PIN("r2");
        register s32 blue PIN("r4");
        register s32 value PIN("r0");

        color = *source;
        red = 31;
        red &= color;
        blue = 248;
        blue <<= 2;
        COMPILER_BARRIER(blue);
        value = blue;
        ASM_BARRIER_VOLATILE(: "+r"(value));
        green = color;
        green &= value;
        green >>= 5;
        blue = 248;
        blue <<= 7;
        COMPILER_BARRIER(blue);
        value = blue;
        ASM_BARRIER_VOLATILE(: "+r"(value));
        color &= value;
        blue = color >> 10;

        value = redWeight;
        value *= red;
        {
            s32 addend;

            addend = greenWeight;
            addend *= green;
            value += addend;
            addend = blueWeight;
            addend *= blue;
            value += addend;
        }
        value /= 3;
        if (value > 31) {
            value = 31;
        }
        blue = 31;
        blue &= value;
        value = blue << 5;
        value |= blue;
        {
            register s32 shifted PIN("r1");

            shifted = blue << 10;
            value |= shifted;
        }
        *destination = value;
        remaining--;
        destination++;
        source++;
    } while (remaining >= 0);
}

void UploadColorFadePalettes(void)
{
    struct ColorFading* fade;
    u32 flags;
    vu32* dma;
    register u32 value PIN("r0");

    fade = &gColorFading;
    flags = fade->uploadFlags;
    if (flags != 0) {
        value = flags & 1;
        if (value != 0) {
            dma = (vu32*)0x040000D4;
            dma[0] = 0x02020000;
            dma[1] = 0x05000000;
            dma[2] = 0x80000100;
            dma[2];
            value = flags & 0xFE;
            fade->uploadFlags = value;
        }
        flags = fade->uploadFlags;
        value = flags & 2;
        if (value != 0) {
            dma = (vu32*)0x040000D4;
            dma[0] = 0x02020200;
            dma[1] = 0x05000200;
            dma[2] = 0x80000100;
            dma[2];
            value = flags & 0xFD;
            fade->uploadFlags = value;
        }
    }
}

void ColorFadingProcess(void)
{
    u8* fade;
    u32 flags;
    vu32* dmaSmall;
    register u32 value PIN("r0");

    fade = (u8*)&gColorFading;
    flags = fade[4];
    if (flags != 0) {
        value = flags & 1;
        if (value != 0) {
            dmaSmall = (vu32*)0x040000D4;
            dmaSmall[0] = 0x02020000;
            value = 160;
            value <<= 19;
            dmaSmall[1] = value;
            dmaSmall[2] = 0x80000100;
            dmaSmall[2];
            value = flags & 0xFE;
            fade[4] = value;
        }

        flags = fade[4];
        value = flags & 2;
        if (value != 0) {
            dmaSmall = (vu32*)0x040000D4;
            dmaSmall[0] = 0x02020200;
            dmaSmall[1] = 0x05000200;
            dmaSmall[2] = 0x80000100;
            dmaSmall[2];
            value = flags & 0xFD;
            fade[4] = value;
        }
    } else {
        u8* timer;
        register u32 timerFlags PIN("r3");
        register vu32* dma PIN("r2");
        register u32 timerValue PIN("r0");
        u32 base;

        timer = gBossDefeatTimer;
        timerFlags = timer[4];
        if (timerFlags != 0) {
            timerValue = timerFlags & 2;
            if (timerValue != 0) {
                dma = (vu32*)0x040000D4;
                timerValue = timer[1] << 5;
                base = 0x02020E00;
                dma[0] = timerValue + base;
                timerValue = timer[1] << 5;
                base = 0x05000200;
                dma[1] = timerValue + base;
                timerValue = timer[2] << 4;
                base = 0x80000000;
                timerValue |= base;
                dma[2] = timerValue;
                dma[2];
                timerValue = 0xFD;
                goto clear_timer_flag;
            }
        } else {
            timer = gGoldenDivaRoomTimer;
            timerFlags = timer[4];
            if (timerFlags != 0) {
                timerValue = timerFlags & 1;
                if (timerValue != 0) {
                    dma = (vu32*)0x040000D4;
                    timerValue = timer[1] << 5;
                    base = 0x02020C00;
                    dma[0] = timerValue + base;
                    timerValue = timer[1] << 5;
                    base = 160;
                    base <<= 19;
                    dma[1] = timerValue + base;
                    timerValue = timer[2] << 4;
                    base = 0x80000000;
                    timerValue |= base;
                    dma[2] = timerValue;
                    dma[2];
                    timerValue = 0xFE;
                clear_timer_flag:
                    timerValue &= timerFlags;
                    timer[4] = timerValue;
                }
            }
        }
    }
}

void UploadBossColorFadePalettes(void)
{
    u8* timer;
    u32 flags;
    register vu32* dma PIN("r2");
    register u32 value PIN("r0");
    u32 base;

    timer = gBossDefeatTimer;
    flags = timer[4];
    if (flags != 0) {
        value = flags & 2;
        if (value != 0) {
            dma = (vu32*)0x040000D4;
            value = timer[1] << 5;
            base = 0x02020E00;
            dma[0] = value + base;
            value = timer[1] << 5;
            base = 0x05000200;
            dma[1] = value + base;
            value = timer[2] << 4;
            base = 0x80000000;
            value |= base;
            dma[2] = value;
            dma[2];
            value = flags & 0xFD;
            timer[4] = value;
        }
    } else {
        timer = gGoldenDivaRoomTimer;
        flags = timer[4];
        if (flags != 0) {
            value = flags & 1;
            if (value != 0) {
                dma = (vu32*)0x040000D4;
                value = timer[1] << 5;
                base = 0x02020C00;
                dma[0] = value + base;
                value = timer[1] << 5;
                base = 0x05000000;
                dma[1] = value + base;
                value = timer[2] << 4;
                base = 0x80000000;
                value |= base;
                dma[2] = value;
                dma[2];
                value = flags & 0xFE;
                timer[4] = value;
            }
        }
    }
}

void ApplyPaletteFade(u32 arg0, u32 arg1)
{
    register u32 mode PIN("r9");
    register u32 amount PIN("r4");
    u32 bank;
    register struct ColorFading* fade PIN("sl");
    register u16* destination PIN("r8");
    register u16* source PIN("r6");
    volatile struct PaletteFadeLoopState state;

    mode = (u8)arg0;
    amount = (u8)arg1;
    bank = 0;
    fade = &gColorFading;

    do {
        if (bank == 0) {
            register struct ColorFading* fadeLow PIN("r3");
            u32 flags;
            u32 test;
            u16* sourceInit;

            {
                register u16* destinationInit PIN("r1");

                destinationInit = (u16*)0x02020000;
                COMPILER_BARRIER(destinationInit);
                destination = destinationInit;
            }
            fadeLow = fade;
            flags = fadeLow->sourceFlags;
            test = 1;
            test &= flags;
            sourceInit = (u16*)0x02020400;
            source = sourceInit;
            if (test != 0) {
                sourceInit = (u16*)0x02020800;
                source = sourceInit;
            }
        } else {
            register u16* destinationInit PIN("r7");
            struct ColorFading* fadeLow;
            u32 flags;
            u32 test;
            u16* sourceInit;

            destinationInit = (u16*)0x02020200;
            destination = destinationInit;
            fadeLow = fade;
            flags = fadeLow->sourceFlags;
            test = 2;
            test &= flags;
            sourceInit = (u16*)0x02020600;
            source = sourceInit;
            if (test != 0) {
                sourceInit = (u16*)0x02020A00;
                source = sourceInit;
            }
        }

        bank++;
        state.nextBank = bank;

        {
            register s32 whiteInit PIN("r1");
            register s32 white PIN("ip");
            register s32 maximum PIN("r5");
            register s32 countInit PIN("r2");
            register struct ColorFading* fadeInner;

            whiteInit = 31;
            COMPILER_BARRIER(whiteInit);
            white = whiteInit;
            maximum = 31;
            countInit = 255;
            state.count = countInit;

            do {
                register u32 color PIN("r1");
                register u32 red PIN("r2");
                register u32 green PIN("r3");
                register s32 temporary PIN("r0");
                u32 bits;
                register u32 type PIN("r0");

                color = *source;
                red = maximum;
                red &= color;
                bits = 248;
                bits <<= 2;
                bits &= color;
                green = bits >> 5;
                bits = 248;
                bits <<= 7;
                bits &= color;
                color = bits >> 10;

                fadeInner = fade;
                type = fadeInner->type;
                COMPILER_BARRIER_INPUT(fadeInner);
                if (type == 1) {
                    u32 modeLow;

                    modeLow = mode;
                    if (modeLow == 0) {
                        s32 whiteRed;
                        s32 whiteGreen;
                        register s32 whiteBlue PIN("r7");

                        temporary = maximum - red;
                        temporary *= amount;
                        temporary >>= 5;
                        whiteRed = white;
                        temporary = whiteRed - temporary;
                        red = (u8)temporary;

                        temporary = maximum - green;
                        temporary *= amount;
                        temporary >>= 5;
                        whiteGreen = white;
                        temporary = whiteGreen - temporary;
                        green = (u8)temporary;

                        temporary = maximum - color;
                        temporary *= amount;
                        temporary >>= 5;
                        whiteBlue = white;
                        temporary = whiteBlue - temporary;
                    } else {
                        temporary = maximum - red;
                        temporary *= amount;
                        temporary >>= 5;
                        temporary = red + temporary;
                        red = (u8)temporary;

                        temporary = maximum - green;
                        temporary *= amount;
                        temporary >>= 5;
                        temporary = green + temporary;
                        green = (u8)temporary;

                        temporary = maximum - color;
                        temporary *= amount;
                        temporary >>= 5;
                        temporary = color + temporary;
                    }
                } else {
                    u32 modeLow;

                    modeLow = mode;
                    if (modeLow == 0) {
                        temporary = red;
                        temporary *= amount;
                        temporary >>= 5;
                        red = (u8)temporary;

                        temporary = green;
                        temporary *= amount;
                        temporary >>= 5;
                        green = (u8)temporary;

                        temporary = color;
                        temporary *= amount;
                        temporary >>= 5;
                    } else {
                        temporary = red;
                        temporary *= amount;
                        temporary >>= 5;
                        temporary = red - temporary;
                        red = (u8)temporary;

                        temporary = green;
                        temporary *= amount;
                        temporary >>= 5;
                        temporary = green - temporary;
                        green = (u8)temporary;

                        temporary = color;
                        temporary *= amount;
                        temporary >>= 5;
                        temporary = color - temporary;
                    }
                }

                color = (u8)temporary;
                COMPILER_BARRIER(color);
                temporary = green << 5;
                red |= temporary;
                temporary = color << 10;
                red |= temporary;

                {
                    register u16* destinationLow PIN("r1");

                    destinationLow = destination;
                    *destinationLow = red;
                }

                {
                    register s32 count PIN("r2");
                    s32 increment;

                    count = state.count;
                    count--;
                    state.count = count;
                    increment = 2;
                    destination = (u16*)((u8*)destination + increment);
                    source++;
                    if (count < 0) {
                        break;
                    }
                }
            } while (1);
        }

        {
            register s32 nextBankLow PIN("r7");
            register u32 narrowed PIN("r0");

            nextBankLow = state.nextBank;
            narrowed = nextBankLow;
            bank = (u8)narrowed;
        }
    } while (bank <= 1);

    {
        u32 value;
        register struct ColorFading* fadeLow PIN("r1");

        value = 3;
        fadeLow = &gColorFading;
        fadeLow->uploadFlags = value;
    }
}

u32 UpdatePaletteInterpolation(u32 arg0, u32 arg1)
{
    u32 paletteSet;
    register struct ColorFading* fade PIN("r9");
    register s32 step PIN("ip");
    register s32 interval PIN("r8");
    volatile struct PaletteBufferPair pointers;

    paletteSet = (u8)arg0;

    {
        u32 rawSpeed;
        struct ColorFading* fadeAddress;
        u32 pending;

        rawSpeed = (u8)arg1;
        fadeAddress = &gColorFading;
        pending = fadeAddress->interpolationPending;
        fade = fadeAddress;
        if (pending != 0) {
            u32 zero;
            register struct ColorFading* fadeLow PIN("r1");

            zero = 0;
            fadeLow = fade;
            fadeLow->interpolationPending = zero;
            zero = 1;
            fadeLow->uploadFlags = zero;
            return 0;
        }

        {
            register struct ColorFading* fadeLow PIN("r1");

            fadeLow = fade;
            if (fadeLow->progress == 32) {
                return 1;
            }
        }

        {
            s32 signedSpeed;

            signedSpeed = (s8)rawSpeed;
            step = signedSpeed;
            if (signedSpeed >= 0) {
                goto nonnegative_speed;
            }
        }
    }

    {
        s32 inverted;
        register s32 stepCopy PIN("r2");

        inverted = 255;
        stepCopy = step;
        COMPILER_BARRIER(stepCopy);
        inverted ^= stepCopy;
        inverted++;
        step = inverted;
    }
    {
        s32 one;

        one = 1;
        interval = one;
    }
    goto speed_ready;

nonnegative_speed:
    interval = step;
    {
        register s32 one PIN("r1");

        one = 1;
        COMPILER_BARRIER(one);
        step = one;
    }

speed_ready: {
    register struct ColorFading* fadeLow PIN("r2");
    u32 timer;

    fadeLow = fade;
    timer = fadeLow->timer;
    timer++;
    fadeLow->timer = timer;
    timer = fadeLow->timer;
    if ((s32)timer < interval) {
        return 0;
    }
}

    {
        register u32 zero PIN("r0");
        struct ColorFading* fadeLow;
        u32 level;

        zero = 0;
        fadeLow = fade;
        fadeLow->timer = zero;
        level = fadeLow->progress;
        level += step;
        fadeLow->progress = level;
        level = (u8)level;
        if (level > 32) {
            fadeLow->progress = 32;
        }
    }

    {
        register struct ColorFading* fadeLow PIN("r2");
        u32 factor;
        register u16* srcA PIN("sl");
        s32 count;

        fadeLow = fade;
        factor = fadeLow->progress;
        GP_SET(pointers.dst, (u16*)0x02020000);

        if (paletteSet == 0) {
            register u16* srcAInit PIN("r1");
            register u16* srcBInit PIN("r2");

            srcAInit = (u16*)0x02020400;
            COMPILER_BARRIER(srcAInit);
            srcA = srcAInit;
            srcBInit = (u16*)0x02020800;
            GP_SET(pointers.srcB, srcBInit);
        } else {
            u16* srcBInit;
            u16* srcAInit;

            srcBInit = (u16*)0x02020400;
            GP_SET(pointers.srcB, srcBInit);
            srcAInit = (u16*)0x02020800;
            srcA = srcAInit;
        }

        {
            s32 countInit;

            countInit = 255;
            count = countInit;
        }

        do {
            {
                u16 color;
                u32 redA;
                u32 greenA;
                u32 blueA;
                u32 redB;
                register u32 greenB PIN("r4");
                u32 blueB;
                register s32 temp PIN("r0");
                register u32 bits PIN("r0");
                u32 redOut;
                u32 greenOut;
                register u32 blueOut PIN("r2");
                register u16* srcB PIN("r2");

                color = *srcA;
                redA = 31;
                redA &= color;
                bits = 248;
                bits <<= 2;
                bits &= color;
                greenA = bits >> 5;
                bits = 248;
                bits <<= 7;
                bits &= color;
                blueA = bits >> 10;

                srcB = GP(pointers.srcB);
                color = *srcB;
                redB = 31;
                redB &= color;
                bits = 248;
                bits <<= 2;
                bits &= color;
                greenB = bits >> 5;
                bits = 248;
                bits <<= 7;
                bits &= color;
                blueB = bits >> 10;

                if (redB > redA) {
                    u32 multiplier;

                    temp = redB - redA;
                    multiplier = factor;
                    multiplier *= temp;
                    temp = multiplier;
                    temp >>= 5;
                    temp = redA + temp;
                } else {
                    u32 multiplier;

                    temp = redA - redB;
                    multiplier = factor;
                    multiplier *= temp;
                    temp = multiplier;
                    temp >>= 5;
                    temp = redA - temp;
                }
                redOut = (u8)temp;

                if (greenB > greenA) {
                    u32 multiplier;

                    temp = greenB - greenA;
                    multiplier = factor;
                    multiplier *= temp;
                    temp = multiplier;
                    temp >>= 5;
                    temp = greenA + temp;
                } else {
                    u32 multiplier;

                    temp = greenA - greenB;
                    multiplier = factor;
                    multiplier *= temp;
                    temp = multiplier;
                    temp >>= 5;
                    temp = greenA - temp;
                }
                greenOut = (u8)temp;

                if (blueB > blueA) {
                    u32 multiplier;

                    temp = blueB - blueA;
                    multiplier = factor;
                    multiplier *= temp;
                    temp = multiplier;
                    temp >>= 5;
                    temp = blueA + temp;
                } else {
                    u32 multiplier;

                    temp = blueA - blueB;
                    multiplier = factor;
                    multiplier *= temp;
                    temp = multiplier;
                    temp >>= 5;
                    temp = blueA - temp;
                }
                blueOut = (u8)temp;
                COMPILER_BARRIER(blueOut);

                temp = greenOut << 5;
                redOut |= temp;
                temp = blueOut << 10;
                redOut |= temp;
                temp = (s32)GP(pointers.dst);
                *(u16*)temp = redOut;

                redOut = 1;
                redOut = -redOut;
                count += redOut;
                temp += 2;
                GP_SET(pointers.dst, (u16*)temp);
                blueOut = 2;
                srcA = (u16*)((u8*)srcA + blueOut);
                GP_POSTINC(pointers.srcB);
                redOut = count;
                COMPILER_BARRIER(redOut);
                if ((s32)redOut < 0) {
                    break;
                }
            }
        } while (1);
    }

    {
        u32 one;
        register struct ColorFading* fadeLow PIN("r2");

        one = 1;
        fadeLow = fade;
        fadeLow->interpolationPending = one;
    }
    return 0;
}

static const u16 sPaletteInterpolationAlignment __attribute__((section(".text"))) = 0;

void ConvertObjPaletteBankToGrayscale(u8 arg0)
{
    register u32 paletteIndex PIN("r6");
    register u16* palette PIN("r4");
    register s32 colorIndex PIN("r5");
    register u32 r7Value;

    paletteIndex = arg0;
    {
        register u32 offset PIN("r0");
        u32 base;

        offset = paletteIndex << 5;
        base = 0x05000200;
        palette = (u16*)(offset + base);
    }
    colorIndex = paletteIndex << 4;
    if (colorIndex <= 0xFF) {
        do {
            u32 color;
            register s32 red PIN("r0");
            register s32 green PIN("r3");
            register u32 bits PIN("r1");
            s32 gray;

            color = *palette;
            red = 0x1F;
            red &= color;
            green = 0xF8;
            green <<= 2;
            bits = green;
            ASM_BARRIER(: : "r"(bits), "r"(green));
            green = color;
            green &= bits;
            green >>= 5;
            ASM_BARRIER_VOLATILE(: "+r"(r7Value));
            r7Value = 0xF8;
            r7Value <<= 7;
            bits = r7Value;
            ASM_BARRIER(: : "r"(bits), "r"(r7Value));
            color &= bits;
            color >>= 10;
            red += green;
            red += color;
            gray = red / 3;
            if (gray > 0x1F) {
                gray = 0x1F;
            }
            red = gray << 10;
            bits = gray << 5;
            red |= bits;
            red |= gray;
            *palette = red;
            colorIndex++;
            palette++;
        } while (colorIndex <= 0xFF);
    }

    {
        vu32* dma;
        register u32 offset PIN("r1");
        u32 base;
        register u32 value PIN("r0");

        dma = (vu32*)0x040000D4;
        offset = paletteIndex << 5;
        base = 0x05000200;
        value = offset + base;
        dma[0] = value;
        r7Value = 0x02020600;
        ASM_BARRIER_VOLATILE(: "+r"(r7Value));
        value = offset + r7Value;
        dma[1] = value;
        value = 0x200;
        value -= offset;
        value = (s32)value >> 1;
        offset = 0x80000000;
        value |= offset;
        dma[2] = value;
        dma[2];
    }
}

void LoadBossSpriteGraphics(u32 arg0, u32 arg1, u32 arg2)
{
    register u32 value PIN("r0");
    u8* state;
    register u32 paletteOffset PIN("r2");
    u32 type;
    s32 mode;
    u32 palette;
    u32 count;
    u32 copyBack;
    register vu32* dma PIN("r1");
    u16* dst;

    type = (u8)arg0;
    palette = (u8)arg1;
    count = (u8)arg2;
    if (type == 0) {
        return;
    }

    state = gBossDefeatTimer;
    value = 0;
    state[0] = type;
    mode = 0x7F;
    mode &= type;
    state[1] = palette;
    state[2] = count;
    state[3] = value;
    state[5] = value;
    if (mode > 4) {
        mode -= 4;
        value = 1;
        state[5] = value;
        value = type - 4;
        state[0] = value;
    }

    copyBack = 0;
    if (gSubGameMode == 2 || gSubGameMode == 6) {
        dma = (vu32*)0x040000D4;
        paletteOffset = palette << 5;
        type = 0x05000200;
        value = paletteOffset + type;
        dma[0] = value;
        type = 0x02021200;
        value = paletteOffset + type;
        dma[1] = value;
        type = count << 4;
        value = 0x80000000;
        value |= type;
        dma[2] = value;
        value = dma[2];
    } else {
        dma = (vu32*)0x040000D4;
        paletteOffset = palette << 5;
        type = 0x02020600;
        value = paletteOffset + type;
        dma[0] = value;
        type = 0x02021200;
        value = paletteOffset + type;
        dma[1] = value;
        type = count << 4;
        value = 0x80000000;
        value |= type;
        dma[2] = value;
        value = dma[2];
        copyBack = 1;
    }

    if (mode == 1) {
        mode = 0x05000200;
        dst = (u16*)(paletteOffset + mode);
        if (type != 0) {
            value = 0x7FFF;
            COMPILER_BARRIER(value);
            mode = value;
            value = type;
            do {
                *dst = mode;
                value--;
                dst++;
            } while (value != 0);
        }
        goto copy_palette;
    }
    if (mode == 2) {
        return;
    }
    if (mode == 3) {
        mode = 0x05000200;
        COMPILER_BARRIER(mode);
        dst = (u16*)(paletteOffset + mode);
        if (type != 0) {
            mode = 0;
            value = type;
            do {
                *dst = mode;
                value--;
                dst++;
            } while (value != 0);
        }
        goto copy_palette;
    }
    if (mode == 4) {
        copyBack = 0;
    }

copy_palette:

    if (copyBack != 0) {
        dma = (vu32*)0x040000D4;
        mode = 0x05000200;
        value = paletteOffset + mode;
        dma[0] = value;
        mode = 0x02020600;
        value = paletteOffset + mode;
        dma[1] = value;
        value = 0x80000000;
        value |= type;
        dma[2] = value;
        value = dma[2];
    }

    return;
}

void StartGoldenDivaPaletteFade(u32 arg0, u32 arg1, u32 arg2)
{
    u32 type;
    u32 paletteIndex;
    u32 tileCount;
    register s32 mode PIN("r4");
    u32 uploadAgain;
    vu32* reg;
    u32 offset;
    register u32 value PIN("r0");
    u8* timer;

    value = arg0;
    type = (u8)value;

    offset = arg1;
    paletteIndex = (u8)offset;

    reg = (vu32*)arg2;
    reg = (vu32*)((u32)reg << 24);
    tileCount = (u32)reg >> 24;

    if (type == 0) {
        return;
    }

    timer = gGoldenDivaRoomTimer;
    value = 0;
    timer[0] = type;
    mode = 0x7F;
    mode &= type;
    timer[1] = paletteIndex;
    timer[2] = tileCount;
    timer[3] = value;
    timer[5] = value;

    if (mode > 4) {
        mode -= 4;
        value = 1;
        timer[5] = value;
        value = type - 4;
        timer[0] = value;
    }

    uploadAgain = 0;
    {
        s32 zero;
        zero = 0;
        value = gSubGameMode;
    }

    if (gSubGameMode == 2 || gSubGameMode == 6) {
        value = timer[0];
        if (value == 2 || value == 4 || value == 6 || value == 8) {
            reg = (vu32*)0x040000D4;
            offset = paletteIndex << 5;
            value = 0x05000000;
            COMPILER_BARRIER(value);
            value = offset + value;
            reg[0] = value;
            value = 0x02021000;
            offset += value;
            reg[1] = offset;
            value = tileCount << 4;
            offset = 0x80000000;
            value |= offset;
            reg[2] = value;
            reg[2];
        }
    } else {
        reg = (vu32*)0x040000D4;
        value = paletteIndex << 5;
        offset = 0x02020400;
        offset += value;
        reg[0] = offset;
        offset = 0x02021000;
        value += offset;
        reg[1] = value;
        value = tileCount << 4;
        offset = 0x80000000;
        value |= offset;
        reg[2] = value;
        reg[2];
        uploadAgain = 1;
    }

    if (mode == 1) {
        u32 base;
        u16* ptr;
        register u32 fill PIN("r2");

        base = 0x05000000;
        COMPILER_BARRIER(base);
        value = paletteIndex << 5;
        base = (value) - (-(base));
        ptr = (u16*)base;
        value = tileCount << 4;
        if (value != 0) {
            mode = 0x7FFF;
            COMPILER_BARRIER(mode);
            fill = mode;
            do {
                *ptr = fill;
                value--;
                ptr++;
            } while (value != 0);
        }
    } else if (mode == 2) {
        return;
    } else if (mode == 3) {
        u32 base;
        u16* ptr;
        u32 fill;

        base = 0x05000000;
        COMPILER_BARRIER(base);
        value = paletteIndex << 5;
        base = (value) - (-(base));
        ptr = (u16*)base;
        value = tileCount << 4;
        if (value != 0) {
            fill = 0;
            do {
                *ptr = fill;
                value--;
                ptr++;
            } while (value != 0);
        }
    } else if (mode == 4) {
        uploadAgain = 0;
    }

    if (uploadAgain != 0) {
        reg = (vu32*)0x040000D4;
        offset = paletteIndex << 5;
        value = 0x05000000;
        COMPILER_BARRIER(value);
        value = offset + value;
        reg[0] = value;
        value = 0x02020400;
        COMPILER_BARRIER(value);
        offset += value;
        reg[1] = offset;
        value = tileCount << 4;
        offset = 0x80000000;
        value |= offset;
        reg[2] = value;
        reg[2];
    }
}

void UpdateBossDefeatPaletteFade(void)
{
    s32 temp_r1;
    s32 temp_r0;
    u8 temp_r2;

    if ((gBossDefeatTimer[4] == 0) && (gBossDefeatTimer[0] != 0) && (temp_r2 = 0x80 & gBossDefeatTimer[0]) == 0) {
        if ((u32)gBossDefeatTimer[0] > 4U) {
            gBossDefeatTimer[5] = 1U;
            gBossDefeatTimer[0] -= 4;
        }
        if ((gBossDefeatTimer[5] == 0) || !(1 & gMainTimer)) {
            temp_r0 = gBossDefeatTimer[3] + 1;
            gBossDefeatTimer[3] = temp_r0;
            temp_r0 = (u8)temp_r0;
            if ((u32)temp_r0 <= 0x20U) {
                temp_r1 = gBossDefeatTimer[1] << 5;
                ApplyTimedPaletteFade((s16*)(temp_r1 + 0x02020E00), (u16*)(temp_r1 + 0x02021200), gBossDefeatTimer);
                gBossDefeatTimer[4] = 2U;
                return;
            }
            gBossDefeatTimer[0] = temp_r2;
            gBossDefeatTimer[3] = temp_r2;
            gBossDefeatTimer[5] = temp_r2;
        }
    }
}

void UpdateGoldenDivaPaletteFade(void)
{
    s32 temp_r1;
    s32 temp_r0;
    u8 temp_r2;

    if ((gGoldenDivaRoomTimer[4] == 0) && (gGoldenDivaRoomTimer[0] != 0) &&
        (temp_r2 = 0x80 & gGoldenDivaRoomTimer[0]) == 0) {
        if ((u32)gGoldenDivaRoomTimer[0] > 4U) {
            gGoldenDivaRoomTimer[5] = 1U;
            gGoldenDivaRoomTimer[0] -= 4;
        }
        if ((gGoldenDivaRoomTimer[5] == 0) || !(1 & gMainTimer)) {
            temp_r0 = gGoldenDivaRoomTimer[3] + 1;
            gGoldenDivaRoomTimer[3] = temp_r0;
            temp_r0 = (u8)temp_r0;
            if ((u32)temp_r0 <= 0x20U) {
                temp_r1 = gGoldenDivaRoomTimer[1] << 5;
                ApplyTimedPaletteFade((s16*)(temp_r1 + 0x02020C00), (u16*)(temp_r1 + 0x02021000), gGoldenDivaRoomTimer);
                gGoldenDivaRoomTimer[4] = 1U;
                return;
            }
            gGoldenDivaRoomTimer[0] = temp_r2;
            gGoldenDivaRoomTimer[3] = temp_r2;
            gGoldenDivaRoomTimer[5] = temp_r2;
        }
    }
}

void ApplyTimedPaletteFade(s16* arg0, u16* arg1, u8* arg2)
{
    s16* dst;
    u16* src;
    u8* timer;
    register s32 i PIN("r6");
    ColorBlendFunction* table;
    register u16 color PIN("r2");
    u32 red;
    u32 green;
    u32 blue;
    u32 amount;
    u32 index;
    ColorBlendFunction func;
    u32 count;

    dst = arg0;
    src = arg1;
    timer = arg2;
    count = timer[2];
    i = 0;
    if (count != 0) {
        table = sColorBlendFunctionTable;
        do {
            color = *src;
            red = color & 0x1F;
            green = (color & 0x3E0) >> 5;
            blue = (color & 0x7C00) >> 10;
            index = timer[0];
            index--;
#ifdef PORTABLE
            // The GBA indexes both arrays as one through a byte address; the native arrays
            // are separate and their addresses do not fit a u32.
            amount = timer[3];
            func = index == 0 ? table[0] : sColorBlendFunctionTableTail[index - 1];
#else
            index <<= 2;
            index += (u32)table;
            amount = timer[3];
            func = *(ColorBlendFunction*)index;
#endif
            *dst = func(red, green, blue, amount);
            i++;
            amount = 2;
            dst = (s16*)((u32)dst + amount);
            src++;
        } while (i < (s32)(timer[2] << 4));
    }
}

s32 BlendColorFromWhite(u8 arg0, u8 arg1, u8 arg2, u8 arg3)
{
    s32 max1;
    register s32 max2 PIN("r5");
    s32 greenValue;
    register s32 temp PIN("r1");
    s32 red;

    max1 = 0x1F;
    red = max1 - arg0;
    red *= arg3;
    red >>= 5;
    max2 = 0x1F;
    red = max2 - red;
    red &= max2;

    temp = max1 - arg1;
    COMPILER_BARRIER(temp);
    greenValue = temp;
    greenValue *= arg3;
    greenValue >>= 5;
    greenValue = max2 - greenValue;
    greenValue &= max2;

    max1 = max1 - arg2;
    COMPILER_BARRIER(max1);
    temp = max1;
    temp *= arg3;
    temp >>= 5;
    temp = max2 - temp;
    temp &= max2;

    greenValue <<= 5;
    red |= greenValue;
    temp <<= 10;
    red |= temp;
    return red;
}

s32 BlendColorToWhite(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    s32 red;
    s32 green;
    s32 blue;
    s32 amount;
    s32 maximum;
    register s32 delta PIN("r4");
    register s32 mask PIN("r6");

    red = (u8)arg0;
    green = (u8)arg1;
    blue = (u8)arg2;
    amount = (u8)arg3;
    maximum = 31;
    delta = maximum - red;
    delta *= amount;
    delta >>= 5;
    red += delta;
    mask = 31;
    red &= mask;
    delta = maximum - green;
    delta *= amount;
    delta >>= 5;
    green += delta;
    green &= mask;
    maximum -= blue;
    amount *= maximum;
    amount >>= 5;
    blue += amount;
    blue &= mask;
    green <<= 5;
    red |= green;
    blue <<= 10;
    red |= blue;
    return red;
}

s32 BlendColorFromBlack(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    s32 red;
    s32 green;
    s32 blue;
    s32 amount;
    register s32 temporary PIN("r4");
    register s32 mask PIN("r5");

    red = (u8)arg0;
    green = (u8)arg1;
    COMPILER_BARRIER(green);
    blue = (u8)arg2;
    amount = (u8)arg3;
    red *= amount;
    red >>= 5;
    mask = 31;
    red &= mask;
    temporary = green;
    temporary *= amount;
    temporary >>= 5;
    temporary &= mask;
    green = blue;
    green *= amount;
    green >>= 5;
    green &= mask;
    temporary <<= 5;
    red |= temporary;
    green <<= 10;
    red |= green;
    return red;
}

s32 BlendColorToBlack(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    s32 red;
    s32 green;
    s32 blue;
    s32 amount;
    s32 temporary;
    register s32 mask PIN("r5");

    red = (u8)arg0;
    green = (u8)arg1;
    blue = (u8)arg2;
    amount = (u8)arg3;
    temporary = red;
    temporary *= amount;
    temporary >>= 5;
    red -= temporary;
    mask = 31;
    red &= mask;
    temporary = green;
    temporary *= amount;
    temporary >>= 5;
    green -= temporary;
    green &= mask;
    amount *= blue;
    amount >>= 5;
    blue -= amount;
    blue &= mask;
    green <<= 5;
    red |= green;
    blue <<= 10;
    red |= blue;
    return red;
}

void UpdateEscapePaletteFadeSequence(void)
{
    u8* state;
    s32 step;
    u8* stateBase;
    u32 callArg0;
    u32 callArg1;
    u8 initialState;
    if (gSubGameMode != 2) {
        return;
    }
    stateBase = &gEscapeHBlankEffectState;
    initialState = *stateBase;
    state = stateBase;
    if (initialState <= 14) {
        if (gTimerState == 11) {
            *state = 17;
        } else if (gTimerState > 1) {
            *state = 15;
        }
    }
    step = 2;
    if ((gStageTimerDigits[1] <= 3) && (gStageTimerDigits[2] == 0)) {
        step = 1;
    }
    switch (*state) {
        case 1:
            SetHurryUpMusic(1);
            gColorFading.timer = 0;
            gEscapeHBlankEffectState++;
            break;

        case 2:
            if (gWarioPauseTimer != 0) {
                (*state)++;
            }
            break;

        case 3:
            if (gWarioPauseTimer == 0) {
                gColorFading.timer = 0;
                (*state)++;
            }
            break;

        case 4:
            SetHBlankMode(3);

        case 5:
            BackupPalettesToSecondaryBuffers();
            BuildRoomTintPalette(0);
            gColorFading.timer = 0;
            gColorFading.progress = 0;
            gColorFading.interpolationPending = 0;
            gEscapeHBlankEffectState++;
            break;

        case 6:

        case 11:
            callArg1 = step;
            callArg0 = 0;
            COMPILER_BARRIER2(callArg0, callArg1);
            if (UpdatePaletteInterpolation(callArg0, callArg1) != 0) {
                gColorFading.timer = 0;
                gColorFading.progress = 0;
                gEscapeHBlankEffectState++;
            }
            break;

        case 7:

        case 9:

        case 12: {
            struct ColorFading* fade;
            fade = &gColorFading;
            fade->timer = 0;
            fade->progress = 0;
            fade->interpolationPending = 0;
            (*state)++;
            break;
        }

        case 8:

        case 13:
            callArg1 = step;
            callArg0 = 1;
            if (UpdatePaletteInterpolation(callArg0, callArg1) != 0) {
                gColorFading.timer = 0;
                gColorFading.progress = 0;
                gEscapeHBlankEffectState++;
            }
            break;

        case 10:
            BuildRoomTintPalette(1);
            gColorFading.timer = 0;
            gColorFading.progress = 0;
            gEscapeHBlankEffectState++;
            break;

        case 14:
            gColorFading.timer = 0;
            gColorFading.progress = 0;
            *state = 5;
            break;

        case 15:
            BackupPalettesToSecondaryBuffers();
            BuildGrayscaleBgPaletteBuffer();
            gColorFading.timer = 0;
            gColorFading.progress = 0;
            gColorFading.interpolationPending = 0;
            gEscapeHBlankEffectState++;
            break;

        case 16:
            if (UpdatePaletteInterpolation(0, 2) != 0) {
                gColorFading.timer = 0;
                gEscapeHBlankEffectState++;
            }
            break;

        case 17:
            break;
    }
}

void BuildRoomTintPalette(u32 arg0)
{
    s8 channels[3];
    u16* destination;
    u16* source;
    s32 index;
    s8* channelBase;
    register const s8* offsetTable PIN("sl");
    u8* roomIndex;
    u32 direction;
    u16* nextDestination;
    s32 nextIndex;
    s8* channel;
    s32 channelIndex;

    direction = (u8)arg0;
    destination = (u16*)0x02020800;
    source = (u16*)0x02020400;
    *destination = 0;
    *source = 0;
    destination++;
    source++;
    index = 1;
    channelBase = channels;
    offsetTable = sRoomPaletteColorOffsets;
    roomIndex = &gStageRoomTableIndex;

    do {
        channelBase[0] = *source & 31;
        channelBase[1] = (*source & 0x3E0) >> 5;
        channelBase[2] = (*source & 0x7C00) >> 10;

        channelBase[0] += offsetTable[(((*roomIndex << 1) + direction) * 3)];
        {
            u32 offsetIndex;
            const s8* greenBase;
            offsetIndex = (((*roomIndex << 1) + direction) * 3);
            greenBase = offsetTable + 1;
            offsetIndex += (u32)greenBase;
            channelBase[1] += *(const s8*)offsetIndex;
        }
        {
            u32 offsetIndex;
            const s8* blueBase;
            offsetIndex = (((*roomIndex << 1) + direction) * 3);
            blueBase = sRoomPaletteColorOffsets + 2;
            channelBase[2] += blueBase[offsetIndex];
        }

        nextDestination = destination + 1;
        source++;
        nextIndex = index + 1;
        channel = channels;
        channelIndex = 2;
        do {
            if (*channel > 31) {
                *channel = 31;
            } else if (*channel < 0) {
                *channel = 0;
            }
            channel++;
            channelIndex--;
        } while (channelIndex >= 0);

        *destination = channelBase[0] | (channelBase[1] << 5) | (channelBase[2] << 10);
        index = nextIndex;
        destination = nextDestination;
    } while (index <= 255);
}

s32 UpdatePaletteFadeStep(u32 arg0, u32 arg1)
{
    u32 mode;
    u32 tableIndex;
    s32 result;
    u32 amount;

    mode = (u8)arg0;
    tableIndex = (u8)arg1;
    result = 0;

    if (gColorFading.progress == 0) {
        if (mode == 0) {
            gBldy = 0;
        }
        gColorFading.sourceFlags = 0;
        if (tableIndex == 0 && gSwitchPressed != 0 && gEscapeHBlankEffectState > 16) {
            gColorFading.sourceFlags = 1;
        }
    } else if (gColorFading.progress == sPaletteFadeStepLimits[tableIndex]) {
        if (mode != 0) {
            if (gColorFading.type == 1) {
                *(vu16*)0x04000050 = 0x00BF;
            } else {
                *(vu16*)0x04000050 = 0x00FF;
            }
            gBldy = 16;
            *(vu16*)0x04000054 = 16;
        } else {
            if ((gColorFading.sourceFlags & 1) != 0) {
                CopyAlternateBgAndSecondaryObjPalettesToPrimaryBuffers();
            } else {
                CopySecondaryPalettesToPrimaryBuffers();
            }
            gColorFading.uploadFlags = 3;
        }
        gColorFading.progress++;
    } else if (gColorFading.progress > sPaletteFadeStepLimits[tableIndex]) {
        result = 1;
    }
    if (gColorFading.progress < sPaletteFadeStepLimits[tableIndex]) {
        if (tableIndex == 2) {
            amount = gColorFading.progress;
        } else {
            amount = sPaletteFadeAmounts[gColorFading.progress];
        }
        ApplyPaletteFade(mode, amount);
        gColorFading.progress++;
    }
    if (result != 0) {
        gColorFading.progress = 0;
    }
    return result;
}

void InitializeColorFadeEffect(void)
{
    switch (((u8*)&gColorFading)[0]) {
        case 1:
        case 2:
            FillColorFadePalettes();
            HandleSwitchColorFade();
            break;
        case 5:
            BackupPalettesToSecondaryBuffers();
            if (HandleSwitchColorFade() != 0) {
                vu32* dma;
                register u32 value PIN("r0");
                dma = (vu32*)0x040000D4;
                value = 0x02020800;
                dma[0] = value;
                value = 0x05000000;
                dma[1] = value;
                value = 0x80000100;
                dma[2] = value;
                dma[2];
            }
            break;
    }
}

void PrepareColorFadeBuffers(void)
{
    switch (((u8*)&gColorFading)[0]) {
        case 1:
        case 2:
            FillColorFadePalettes();
            break;
        case 5:
            BackupPalettesToSecondaryBuffers();
            break;
    }
}

void FillColorFadePalettes(void)
{
    u8* fade;
    u16* dst0;
    u16* dst1;
    u16* dst2;
    u16* dst3;
    s32 count;
    u32 color;

    fade = (u8*)&gColorFading;
    fade[2] = 0;
    BackupPalettesToSecondaryBuffers();
    color = 0;
    if (fade[0] == 1) {
        color = 0x7FFF;
    }
    dst0 = (u16*)0x02020000;
    dst1 = (u16*)0x02020200;
    dst2 = (u16*)0x05000000;
    dst3 = (u16*)0x05000200;
    count = 255;
    do {
        *dst0 = color;
        *dst1 = color;
        *dst2 = color;
        *dst3 = color;
        dst0++;
        dst1++;
        dst2++;
        dst3++;
        count--;
    } while (count >= 0);
}

s32 HandleSwitchColorFade(void)
{
    u32 result;
    u8* timer;
    u8* fade;

    if (gSwitchPressed == 0) {
        return 0;
    }
    result = 0;
    timer = &gEscapeHBlankEffectState;
    if (*timer <= 14) {
        *timer = 4;
        fade = (u8*)&gColorFading;
        fade[5] = result;
        fade[4] = result;
    } else {
        *timer = 17;
        BuildGrayscaleBgPaletteBuffer();
        result = 1;
    }
    return result;
}

s32 ExpandColorFadeWindow(void)
{
    struct Window* window;
    struct Window* rest;
    s32 step;
    register s32 current PIN("r2");
    s32 completed;
    register s32 value PIN("r0");

    window = &gWindow;
    step = window->content;
    if (step == 0) {
        gBldy = step;
        m4aSongNumStart(221);
    }

    value = window->content;
    step = value + 1;
    window->content = step;
    value = step << 24;
    if (value == 0) {
        value = 0xFF;
        step |= value;
        window->content = step;
    }

    value = window->content;
    value >>= 1;
    step = value + 2;
    completed = 0;
    value = window->left;
    if (value > step) {
        value -= step;
        window->left = value;
    } else {
        window->left = completed;
        completed = 1;
    }

    value = (s32)&gWindow;
    current = ((struct Window*)value)->top;
    rest = (struct Window*)value;
    if (current > step) {
        value = current - step;
        rest->top = value;
    } else {
        value = 0;
        rest->top = value;
        completed++;
    }

    current = rest->right;
    window = (struct Window*)240;
    value = (s32)window - step;
    if (current < value) {
        value = current + step;
        rest->right = value;
    } else {
        rest->right = (s32)window;
        completed++;
    }

    current = rest->bottom;
    window = (struct Window*)160;
    value = (s32)window - step;
    if (current < value) {
        value = current + step;
        rest->bottom = value;
    } else {
        rest->bottom = (s32)window;
        completed++;
    }

    completed >>= 2;
    if (completed != 0) {
        value = 0;
        rest->content = value;
    }
    value = completed;
    return value;
}

s32 CollapseColorFadeWindowAroundWario(void)
{
    s32 distances[4];
    struct Window* window;
    s32 targetX;
    s32 targetY;
    s32 result;

    {
        struct Window* first;

        register s32 content PIN("r0");

        first = &gWindow;
        content = first->content;
        if (content == 0) {
            content++;
            first->content = content;
            m4aSongNumStart(222);
        }
        window = first;
    }

    {
        u8* wario;
        u16* bgX;
        u16* bgY;
        s32 zero;
        s32* clear;

        wario = (u8*)&gWarioData;
        bgX = &gBg1XPosition;
        bgY = &gBg1YPosition;
        zero = 0;
        clear = &distances[3];
        do {
            *clear = zero;
            clear--;
        } while ((s32)clear >= (s32)distances);

        {
            s32 warioPosition;
            s32 backgroundPosition;

            warioPosition = *(u16*)(wario + 18);
            backgroundPosition = *bgX;
            targetX = warioPosition - backgroundPosition;
        }
        if (targetX < 0) {
            targetX = 0;
        } else {
            targetX >>= 2;
            if (targetX > 240) {
                targetX = 240;
            }
        }

        {
            s32 warioPosition;
            s32 backgroundPosition;

            warioPosition = *(u16*)(wario + 20);
            backgroundPosition = *bgY;
            backgroundPosition += 63;
            targetY = warioPosition - backgroundPosition;
        }
        if (targetY < 0) {
            targetY = 0;
        } else {
            targetY >>= 2;
            if (targetY > 160) {
                targetY = 160;
            }
        }
    }

    distances[0] = targetX - window->left;
    distances[1] = window->right - targetX;
    distances[2] = targetY - window->top;
    distances[3] = window->bottom - targetY;

    {
        s32 selected;
        register s32 i PIN("r4");
        s32* scan;

        selected = 0;
        i = 0;
        scan = distances;
        do {
            if (distances[selected] <= *scan) {
                selected = i;
            }
            scan++;
            i++;
        } while (i <= 3);

        {
            s32 amount;
            register s32 zero PIN("r2");
            register s32* current PIN("r1");
            register s32 count PIN("r3");
            s32 value;
            register s32 reduced PIN("r1");

            value = selected;
            value <<= 2;
            value += (s32)distances;
            value = *(s32*)value;
            reduced = value - 2;
            value >>= 3;
            amount = reduced - value;
            zero = 0;
            current = distances;
            count = 3;
            do {
                value = *current;
                if (amount < value) {
                    *current = value - amount;
                } else {
                    *current = zero;
                }
                current++;
                count--;
            } while (count >= 0);
        }
    }

    {
        register s32 step PIN("r3");
        s32 current;
        s32 limit;

        step = distances[0];
        if (step != 0) {
            current = window->left;
            limit = targetX - step;
            if (current < limit) {
                limit = current + step;
                window->left = limit;
            } else {
                window->left = targetX;
            }
        }

        step = distances[1];
        if (step != 0) {
            current = window->right;
            limit = targetX + step;
            if (current > limit) {
                limit = current - step;
                window->right = limit;
            } else {
                window->right = targetX;
            }
        }

        step = distances[2];
        if (step != 0) {
            current = window->top;
            limit = targetY - step;
            if (current < limit) {
                limit = current + step;
                window->top = limit;
            } else {
                window->top = targetY;
            }
        }

        step = distances[3];
        if (step != 0) {
            current = window->bottom;
            limit = targetY + step;
            if (current > limit) {
                limit = current - step;
                window->bottom = limit;
            } else {
                window->bottom = targetY;
            }
        }
    }

    {
        register s32 completed PIN("r4");

        completed = 0;
        if (window->left == targetX && window->right == targetX && window->top == targetY &&
            window->bottom == targetY) {
            gBldCnt = 255;
            gBldy = 16;
            completed = 1;
            window->content = 0;
        }
        result = completed;
    }

    return result;
}

s32 CollapseColorFadeWindowVertically(void)
{
    s8 steps[17];
    s32 padding[4];
    struct Window* first;
    struct Window* window;
    s32 value;
    register s32 content PIN("r1");
    s32 step;
    s32 center;
    s32 result;
    u8 contentValue;

    memcpy(steps, sVerticalWindowFadeSteps, 17);
    first = &gWindow;
    value = first->content;
    if (value == 1) {
        value = 449;
        m4aSongNumStart(value);
    }

    window = first;
    {
        s32* start;
        s32* current;

        start = &padding[0];
        step = 0;
        current = &padding[3];
        do {
            *current = step;
            current--;
        } while ((s32)current >= (s32)start);
    }

    center = 80;
    content = window->content;
    value = (s32)steps;
    value += content;
    step = 0;
    step = *(s8*)value;
    if (content != 0) {
        value = content + 1;
        window->content = value;
    }

    if (step != 0) {
        content = window->top;
        value = center - step;
        if (content < value) {
            value = content + step;
            window->top = value;
        } else {
            window->top = center;
        }

        if (step != 0) {
            content = window->bottom;
            value = center + step;
            if (content > value) {
                value = content - step;
                window->bottom = value;
            } else {
                window->bottom = center;
            }
        }
    }

    result = 0;
    content = (s32)window;
    value = ((struct Window*)content)->top;
    if (value != center) {
        goto done;
    }
    value = ((struct Window*)content)->bottom;
    if (value != center) {
        goto done;
    }
    contentValue = ((struct Window*)content)->content;
    if (contentValue == 0) {
        value = 1;
        ((struct Window*)content)->content = value;
        goto done;
    }
    if (contentValue != 17) {
        goto done;
    }
    result = 1;
    value = 0;
    window->content = value;

done:
    value = result;
    return value;
}

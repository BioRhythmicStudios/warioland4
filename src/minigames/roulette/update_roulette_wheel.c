#include "minigames/roulette.h"

void UpdateRouletteWheel(void)
{
    struct RouletteItem *base;
    struct RouletteItem *current;
    const s16 *sinTable;
    register s16 *speedPtr;
    s32 maxAngle;
    int count;

    {
        struct RouletteItem *first;
        first = gRouletteItems;
        if (first->active == 0) {
            LoadRouletteResultTiles();
            first->active = 1;
        }
        base = first;
    }

    maxAngle = 255 << 5;
    sinTable = sSinCosTable;
    current = base;
    speedPtr = &gRouletteAngularSpeed;
    count = 2;
    do {
        if (current->active != 0) {
            {
                u16 speed;
                register u16 oldAngle PIN("r2");
                s32 sum;
                s32 signedSum;

                speed = *speedPtr;
                oldAngle = current->angle;
                sum = speed + oldAngle;
                current->angle = sum;
                signedSum = (s16)sum;
                if (signedSum > maxAngle) {
                    s32 correction;
                    u16 corrected;

                    correction = -8160;
                    corrected = sum + correction;
                    current->angle = corrected;
                }
            }

            {
                register s32 index PIN("r2");
                s32 sample;
                s32 product;

                index = (s16)current->angle >> 5;
                {
                    s32 offset;
                    const s16 *samplePtr;

                    offset = index;
                    offset += 64;
                    offset <<= 1;
                    samplePtr = (const s16 *)offset;
                    samplePtr = (const s16 *)((u32)samplePtr + (u32)sinTable);
                    sample = *samplePtr;
                }
                product = (sample << 3);
                product += sample;
                product >>= 5;
                sample = 124;
                sample -= product;
                current->x = sample;

                sample = sinTable[index];
                index = 256;
                sample += index;
                product = (u32)sample >> 31;
                sample += product;
                sample >>= 1;
                sample <<= 1;
                current->scale = sample;
            }
        }
        current++;
        count--;
    } while (count >= 0);

    {
        s32 accumulatorValue;
        s32 zeroOffset;

        accumulatorValue = (s32)&gRouletteAngularAcceleration;
        zeroOffset = 0;
        accumulatorValue = *(s16 *)(accumulatorValue + zeroOffset);
        if (accumulatorValue == 0) {
        volatile u8 *selectedIndex;
        register const s16 *selectedSin PIN("r4");

        selectedIndex = &gRouletteSelectedItemIndex;
        {
            struct RouletteItem *selected;
            u8 index;

            index = *selectedIndex;
            selected = (struct RouletteItem *)((index * 3) << 2);
            selected = (struct RouletteItem *)((u32)selected + (u32)base);
            selected->angle = 224 << 5;
        }

        {
            struct RouletteItem *selected;
            s32 sample;
            register s32 product PIN("r1");
            s32 index;

            {
                u8 selectedValue;

                selectedValue = *selectedIndex;
                selected = (struct RouletteItem *)((selectedValue * 3) << 2);
                selected = (struct RouletteItem *)((u32)selected + (u32)base);
            }
            selectedSin = sSinCosTable;
            index = (s16)selected->angle >> 5;
            {
                s32 offset;
                const s16 *samplePtr;

                offset = index;
                offset += 64;
                offset <<= 1;
                samplePtr = (const s16 *)offset;
                samplePtr = (const s16 *)((u32)samplePtr + (u32)selectedSin);
                sample = *samplePtr;
            }
            product = (sample << 3) + sample;
            product >>= 5;
            sample = 124;
            sample -= product;
            selected->x = sample;
        }

        {
            struct RouletteItem *selected;
            s32 one;

            {
                u8 selectedValue;

                selectedValue = *selectedIndex;
                selected = (struct RouletteItem *)((selectedValue * 3) << 2);
                selected = (struct RouletteItem *)((u32)selected + (u32)base);
            }
            one = 1;
            selected->active = one;
        }

        {
            struct RouletteItem *selected;
            s32 sample;
            s32 roundingAdjustment;
            s32 index;

            {
                u8 selectedValue;

                selectedValue = *selectedIndex;
                selected = (struct RouletteItem *)((selectedValue * 3) << 2);
                selected = (struct RouletteItem *)((u32)selected + (u32)base);
            }
            index = (s16)selected->angle >> 5;
            sample = selectedSin[index];
            roundingAdjustment = 256;
            sample += roundingAdjustment;
            roundingAdjustment = (u32)sample >> 31;
            sample += roundingAdjustment;
            sample >>= 1;
            sample <<= 1;
            selected->scale = sample;
        }

        {
            struct RouletteItem *selected;

            {
                u8 selectedValue;

                selectedValue = *selectedIndex;
                selected = (struct RouletteItem *)((selectedValue * 3) << 2);
                selected = (struct RouletteItem *)((u32)selected + (u32)base);
            }
            selected->value = gRouletteValuePool[gRouletteValueSequenceIndex];
        }

        if (gMinigameState == 7)
            m4aSongNumStart(584);
        if (gMinigameState == 9)
            m4aSongNumStart(585);
        if (gMinigameState == 11)
            m4aSongNumStart(586);
        }
    }

    {
        s16 *accumulatorPtr;
        s16 *speedPtr2;
        s32 sum;

        accumulatorPtr = &gRouletteAngularAcceleration;
        speedPtr2 = &gRouletteAngularSpeed;
        sum = *(u16 *)speedPtr2;
        sum += *(u16 *)accumulatorPtr;
        *accumulatorPtr = sum;
        if ((s16)sum > 2047) {
        register u16 zero PIN("r4");
        volatile u8 *selectedPtr;
        u16 selectedValue;
        register volatile u8 *sequencePtr PIN("r3");
        u32 sequenceValue;

        zero = 0;
        *accumulatorPtr = zero;
        selectedPtr = &gRouletteSelectedItemIndex;
        selectedValue = *selectedPtr;
        selectedValue++;
        *selectedPtr = selectedValue;
        sequencePtr = &gRouletteValueSequenceIndex;
        sequenceValue = *sequencePtr;
        sequenceValue++;
        *sequencePtr = sequenceValue;
        selectedValue = (u8)selectedValue;
        if (selectedValue > 2)
            *selectedPtr = zero;
        {
            u16 *limitPtr;
            u32 currentSequence;
            u32 limit;

            limitPtr = &gRouletteValueCount;
            currentSequence = *sequencePtr;
            limit = *limitPtr;
            if (currentSequence >= limit)
                *sequencePtr = zero;
        }
        }
    }
}

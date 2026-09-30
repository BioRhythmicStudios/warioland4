#include "minigames/roulette.h"
#include "fixed_point.h"

extern struct RouletteItem *gRouletteItemDrawOrder[3];
extern const u16 *gRouletteItemOamTable[15];
extern const struct AnimationFrame sRouletteMainState1Oam[];
extern const struct AnimationFrame sRouletteMainState2Oam[];
extern const struct AnimationFrame sRoulettePromptOam[];
extern const struct AnimationFrame sRouletteCenterResultOam[];
extern const struct AnimationFrame sRouletteSideResultOam[];
extern const u16 sRouletteAffineTableB[];
extern const u16 sRouletteAffineTableA[];
extern const u16 sRouletteAffineTableC[];
extern s32 FixedInverse(s32);
extern s32 FixedMul(s32, s32);
extern void SortRouletteItemsByScale(void);
extern void DrawRouletteScoreText(void);

void DrawRoulette(void)
{
    register int slot;
    int total;
    u16 *rawDst;
    register struct RouletteMainState *mainState asm("r8");
    const u16 *src;
    OamData *oamBase;
    struct RouletteRenderStack stack;
    s32 inverse;
    u8 state;

    slot = 0;
    total = gOamSlotsUsed;
    {
        int byteOffset;
        byteOffset = total << 3;
        oamBase = gOamBuffer;
        rawDst = (u16 *)(byteOffset + (u32)oamBase);
    }

    {
        int active;
        struct RouletteMainState *statePtr;
        statePtr = &gMinigameCameraState;
        active = statePtr->active;
        mainState = statePtr;
        if (active != 0) {
            if (active == 1)
                src = sRouletteMainState1Oam[0].oam;
            if (active == 2)
                src = sRouletteMainState2Oam[0].oam;
            total += *src++;
            {
                s16 *affineBAddress;
                register s16 *affineCAddress asm("r1");
                register s16 *affineDAddress asm("r4");
                /* agbcc otherwise delays these stack addresses until after the OAM loop. */
                affineBAddress = &stack.affineB;
                stack.affineBPtr = affineBAddress;
                asm volatile("mov %0, sp\nadd %0, %0, #4" : "=r"(affineCAddress));
                stack.affineCPtr = affineCAddress;
                affineDAddress = &stack.affineD;
                stack.affineDPtr = affineDAddress;
            }
            if (slot < total) {
                struct RouletteMainState *position;
                OamData *oam;
                register int xMask asm("r10");
                register int attr2Mask asm("r12");
                position = mainState;
                oam = oamBase;
                xMask = 0x1FF;
                attr2Mask = -13;
                slot = total;
                do {
                    {
                        register int value asm("r2");
                        value = *src++;
                        *rawDst++ = value;
                        ((u8 *)oam)[0] = position->y + value - 16;
                        ((u8 *)oam)[1] |= 3;
                    }
                    {
                        register int value asm("r2");
                        register int coord asm("r1");
                        u16 xOffset;
                        int maskTemp;
                        value = *src++;
                        *rawDst++ = value;
                        xOffset = 8;
                        coord = *(s16 *)((u8 *)position + xOffset);
                        coord = value + coord;
                        coord -= 32;
                        /* agbcc otherwise copies sl through r0; the target uses r2. */
                        asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                        {
                            int oldAttr;
                            int maskedAttr;
                            oldAttr = oam->all.attr1;
                            maskedAttr = 0xFFFFFE00;
                            maskedAttr &= oldAttr;
                            maskedAttr |= coord;
                            oam->all.attr1 = maskedAttr;
                        }
                    }
                    {
                        u16 oldByte;
                        int mask;
                        oldByte = ((u8 *)oam)[3];
                        mask = 15;
                        mask = -mask;
                        mask &= oldByte;
                        ((u8 *)oam)[3] = mask;
                    }
                    {
                        int value;
                        register int oldByte asm("r1");
                        value = *src;
                        *rawDst = value;
                        src++;
                        oldByte = ((u8 *)oam)[5];
                        value = attr2Mask;
                        value &= oldByte;
                        ((u8 *)oam)[5] = value;
                    }
                    rawDst += 2;
                    oam++;
                    slot--;
                } while (slot != 0);
                slot = total;
            }
            {
                s32 sin64;
                s32 sin0;
                register s32 negSin0 asm("r4");
                u32 affineDResult;

                {
                    register const s16 *sinPtr asm("r0");
                    s32 scaleValue;
                    u16 result;
                    sinPtr = sSinCosTable;
                    sinPtr += 64;
                    /* The earlier source pointer still occupies r3 in agbcc's allocator. */
                    asm volatile("mov r3, #0\nldrsh r5, [r0, r3]"
                                 : "=r"(sin64) : "r"(sinPtr) : "r3");
                    asm volatile("mov r4, r8\nmov r1, #4\nldrsh r0, [r4, r1]"
                                 : "=r"(scaleValue) : : "r1", "r4");
                    result = FixedMul(sin64, (s16)FixedInverse(scaleValue));
                    stack.affineA = result;
                }

                {
                    register const s16 *sinPtr asm("r2");
                    s32 scaleValue;
                    s32 result;
                    register s16 *outPtr asm("r3");
                    sinPtr = sSinCosTable;
                    asm volatile("mov r3, #0\nldrsh r4, [r2, r3]"
                                 : "=r"(sin0) : "r"(sinPtr) : "r3");
                    asm volatile("mov r1, r8\nmov r2, #4\nldrsh r0, [r1, r2]"
                                 : "=r"(scaleValue) : : "r1", "r2");
                    result = FixedMul(sin0, (s16)FixedInverse(scaleValue));
                    outPtr = stack.affineBPtr;
                    *outPtr = result;
                }

                {
                    const s16 *sinPtr;
                    s32 scaleValue;
                    s32 result;
                    register s16 *outPtr asm("r3");
                    sinPtr = sSinCosTable;
                    negSin0 = (u16)*sinPtr;
                    negSin0 = -negSin0;
                    negSin0 = (s16)negSin0;
                    asm volatile("mov r1, r8\nmov r2, #4\nldrsh r0, [r1, r2]"
                                 : "=r"(scaleValue) : : "r1", "r2");
                    result = FixedMul(negSin0, (s16)FixedInverse(scaleValue));
                    outPtr = stack.affineCPtr;
                    *outPtr = result;
                }

                {
                    s32 scaleValue;
                    s32 result;
                    register s16 *outPtr asm("r2");
                    asm volatile("mov r4, r8\nmov r1, #4\nldrsh r0, [r4, r1]"
                                 : "=r"(scaleValue) : : "r1", "r4");
                    result = FixedMul(sin64, (s16)FixedInverse(scaleValue));
                    affineDResult = (u16)result;
                    outPtr = stack.affineDPtr;
                    *outPtr = affineDResult;
                }

                gOamBuffer[0].all.affineParam = stack.affineA;
                {
                    register s16 *affineBAddress asm("r3");
                    int value;
                    affineBAddress = stack.affineBPtr;
                    value = *(u16 *)affineBAddress;
                    gOamBuffer[1].all.affineParam = value;
                }
                {
                    register s16 *affineCAddress asm("r4");
                    int value;
                    affineCAddress = stack.affineCPtr;
                    value = *(u16 *)affineCAddress;
                    gOamBuffer[2].all.affineParam = value;
                }
                gOamBuffer[3].all.affineParam = affineDResult;
            }
        }
    }

    {
        register u8 *statePtr asm("r1");
        statePtr = &gMinigameState;
        state = *statePtr;
    }
    if ((u8)(state - 4) <= 1) {
        gRoulettePromptAnim.timer++;
        if (sRoulettePromptOam[gRoulettePromptAnim.frame].time < gRoulettePromptAnim.timer) {
            gRoulettePromptAnim.timer = 1;
            gRoulettePromptAnim.frame++;
            if (sRoulettePromptOam[gRoulettePromptAnim.frame].time == 0)
                gRoulettePromptAnim.frame = 0;
        }
        src = sRoulettePromptOam[gRoulettePromptAnim.frame].oam;
        total += *src++;
        if (slot < total) {
            OamData *oam;
            OamData *oamBase2;
            int xMask;
            register int xMaskLoad asm("r5");
            int attr1Mask;
            int attr1Load;
            register int attr2Mask asm("r5");
            int byteOffset;
            register int totalTemp asm("r1");
            oamBase2 = gOamBuffer;
            xMaskLoad = 0x1FF;
            asm volatile("" : "+r"(xMaskLoad));
            xMask = xMaskLoad;
            attr1Load = 0xFFFFFE00;
            attr1Mask = attr1Load;
            attr2Mask = 13;
            attr2Mask = -attr2Mask;
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u32)oamBase2);
            /* Preserve the target r0 copy of total before subtraction. */
            asm volatile("mov %0, %2\nsub %1, %0, %1"
                         : "=r"(totalTemp), "+r"(slot) : "r"(total));
            do {
                register int value asm("r2");
                int coord;
                value = *src++;
                *rawDst++ = value;
                {
                    register int out asm("r0");
                    out = value;
                    out += 64;
                    ((u8 *)oam)[0] = out;
                }

                value = *src++;
                *rawDst++ = value;
                coord = value;
                coord += 140;
                {
                    /* Keep the target's r8 -> r2 mask copy. */
                    asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                }
                {
                    register int oldAttr asm("r2");
                    register int maskedAttr asm("r0");
                    oldAttr = oam->all.attr1;
                    maskedAttr = attr1Mask;
                    maskedAttr &= oldAttr;
                    maskedAttr |= coord;
                    oam->all.attr1 = maskedAttr;
                }

                {
                    int out;
                    register int oldByte asm("r1");
                    out = *src;
                    *rawDst = out;
                    src++;
                    oldByte = ((u8 *)oam)[5];
                    out = attr2Mask;
                    out &= oldByte;
                    oldByte = 4;
                    out |= oldByte;
                    ((u8 *)oam)[5] = out;
                }
                rawDst += 2;
                oam++;
                slot--;
            } while (slot != 0);
            slot = total;
        }
    }

    {
        register u8 *rouletteStatePtr asm("r3");
        rouletteStatePtr = &gMinigameState;
        if (*rouletteStatePtr > 6) {
            register int itemIndex asm("r4");
            itemIndex = 0;
            do {
                register struct RouletteItem **currentItemRef asm("r5");
                struct RouletteItem **itemsBase;
                int itemOffset;
                int nextItem;
                register struct RouletteItem *itemPtr asm("r0");
                register int itemZero asm("r5");
                register int itemX asm("r2");
                register int itemLimit asm("r0");
                itemsBase = gRouletteItemDrawOrder;
                itemOffset = itemIndex << 2;
                itemsBase = (struct RouletteItem **)(itemOffset + (u32)itemsBase);
                itemPtr = *itemsBase;
                itemZero = 0;
                asm volatile("ldrsh %0, [%1, %2]" : "=r"(itemX) : "r"(itemPtr), "r"(itemZero));
                itemLimit = 250;
                itemLimit <<= 2;
                stack.itemOffset = itemOffset;
                nextItem = itemIndex + 1;
                stack.nextItem = nextItem;
                if (itemX != itemLimit) {
                    switch (gMinigameState) {
                    case 7:
                    case 9:
                    case 11:
                        goto rouletteItemUpdate;
                    case 8:
                    case 10:
                    case 12:
                    default:
                        goto rouletteItemDraw;
                    }
rouletteItemUpdate:
                    SortRouletteItemsByScale();
rouletteItemDraw:
                    {
                        const u16 **animBase;
                        register struct RouletteItem **itemsBase2 asm("r2");
                        register int savedOffset asm("r3");
                        animBase = gRouletteItemOamTable;
                        itemsBase2 = gRouletteItemDrawOrder;
                        savedOffset = stack.itemOffset;
                        currentItemRef = (struct RouletteItem **)(savedOffset + (u32)itemsBase2);
                        src = animBase[(*currentItemRef)->value];
                    }
                    total += *src++;
                    {
                        s16 *affineBAddress;
                        register s16 *affineCAddress asm("r1");
                        affineBAddress = &stack.affineB;
                        stack.affineBPtr = affineBAddress;
                        asm volatile("mov %0, sp\nadd %0, %0, #4" : "=r"(affineCAddress));
                        stack.affineCPtr = affineCAddress;
                        affineBAddress += 2;
                        stack.affineDPtr = affineBAddress;
                    }
                    itemIndex++;
                    stack.nextItem = itemIndex;
                    if (slot < total) {
                        int matrixIndex;
                        int nibbleMask;
                        int nibbleLoad;
                        register int attr1Mask asm("r12");
                        register int attr1Load asm("r2");
                        stack.itemRef = currentItemRef;
                        matrixIndex = itemIndex;
                        nibbleLoad = 15;
                        /* Keep the persistent nibble mask in r8 for both priority paths. */
                        asm volatile("mov %0, %1" : "=r"(nibbleMask) : "r"(nibbleLoad));
                        attr1Load = 0xFFFFFE00;
                        asm volatile("" : "+r"(attr1Load) : : "memory");
                        attr1Mask = attr1Load;
                        do {
                            int value;
                            int byteOffset;
                            OamData *oam;
                            register OamData *base asm("r4");
                            register struct RouletteItem **savedRef asm("r1");
                            struct RouletteItem *savedItem;
                            register int coord asm("r1");

                            value = *src++;
                            *rawDst++ = value;
                            byteOffset = slot << 3;
                            base = gOamBuffer;
                            oam = (OamData *)(byteOffset + (u32)base);
                            savedRef = stack.itemRef;
                            savedItem = *savedRef;
                            ((u8 *)oam)[0] = savedItem->y + value - 16;
                            ((u8 *)oam)[1] |= 3;

                            value = *src++;
                            *rawDst++ = value;
                            {
                                register struct RouletteItem **ref asm("r4");
                                register struct RouletteItem *cur asm("r0");
                                register int zero asm("r4");
                                ref = stack.itemRef;
                                cur = *ref;
                                zero = 0;
                                asm volatile("ldrsh %0, [%1, %2]" : "=r"(coord) : "r"(cur), "r"(zero));
                            }
                            coord = value + coord;
                            coord -= 32;
                            coord &= 0x1FF;
                            {
                                register int oldAttr asm("r2");
                                int maskedAttr;
                                oldAttr = oam->all.attr1;
                                maskedAttr = attr1Mask;
                                maskedAttr &= oldAttr;
                                maskedAttr |= coord;
                                oam->all.attr1 = maskedAttr;
                            }
                            {
                                int bits;
                                register int seven asm("r0");
                                int oldByte;
                                int mask;
                                seven = 7;
                                bits = matrixIndex;
                                bits &= seven;
                                bits <<= 1;
                                oldByte = ((u8 *)oam)[3];
                                /* agbcc reuses seven; force the target's rematerialized 15. */
                                asm volatile("mov %0, #15" : "=r"(mask));
                                mask = -mask;
                                mask &= oldByte;
                                mask |= bits;
                                ((u8 *)oam)[3] = mask;
                            }

                            {
                                int third;
                                third = *src;
                                *rawDst = third;
                            }
                            src++;
                            rawDst++;
                            {
                                u32 priority;
                                int oldByte;
                                oldByte = ((u8 *)oam)[5];
                                priority = 13;
                                priority = -priority;
                                priority &= oldByte;
                                ((u8 *)oam)[5] = priority;
                                {
                                    register u8 *itemState asm("r1");
                                    u32 itemStateValue;
                                    itemState = &gMinigameState;
                                    asm volatile("" : "+r"(itemState) : : "memory");
                                    itemStateValue = *itemState;
                                    if (itemStateValue > 10) {
                                        u32 low;
                                        register u32 high asm("r0");
                                        register u32 maskTemp asm("r2");
                                        high = priority >> 4;
                                        high++;
                                        high <<= 4;
                                        /* Preserve priority in r1 and keep the nibble mask in r2. */
                                        asm volatile("mov %0, %2\nmov %1, r8"
                                                     : "=r"(low), "=r"(maskTemp) : "r"(priority));
                                        low &= maskTemp;
                                        low |= high;
                                        high = (u8)low >> 4;
                                        high++;
                                        high <<= 4;
                                        low &= maskTemp;
                                        low |= high;
                                        ((u8 *)oam)[5] = low;
                                    } else if (itemStateValue > 8) {
                                        u32 low;
                                        register u32 high asm("r1");
                                        high = priority >> 4;
                                        high++;
                                        high <<= 4;
                                        {
                                            register u32 maskTemp asm("r4");
                                            /* Preserve priority in r0 and the nibble mask in r4. */
                                            asm volatile("mov %0, %2\nmov %1, r8"
                                                         : "=r"(low), "=r"(maskTemp) : "r"(priority));
                                            low &= maskTemp;
                                        }
                                        low |= high;
                                        ((u8 *)oam)[5] = low;
                                    }
                                }
                            }
                            rawDst++;
                            slot++;
                        } while (slot < total);
                    }

                    {
                        register const s16 *sinPtr asm("r0");
                        s32 sin64;
                        struct RouletteItem **affineItemRef;
                        register int savedOffset asm("r2");
                        register struct RouletteItem **itemsBase3 asm("r3");
                        s32 sin0;
                        s32 negSin0;

                        /* Prevent agbcc from hoisting this literal load above the OAM loop. */
                        sinPtr = sSinCosTable;
                        sinPtr += 64;
                        /* agbcc otherwise chooses r7 for the zero index. */
                        sin64 = *sinPtr;
                        savedOffset = stack.itemOffset;
                        itemsBase3 = gRouletteItemDrawOrder;
                        affineItemRef = (struct RouletteItem **)(savedOffset + (u32)itemsBase3);

                        {
                            struct RouletteItem *scaleItem;
                            scaleItem = *affineItemRef;
                            inverse = scaleItem->scale;
                        }
                        stack.affineA = FixedMul(sin64, (s16)FixedInverse(inverse));

                        {
                            register const s16 *zeroSinPtr asm("r2");
                            zeroSinPtr = sSinCosTable;
                            /* Load sin(0) in r2, then preserve it in r8 across calls. */
                            asm volatile("mov r3, #0\nldrsh r2, [r2, r3]\nmov r8, r2"
                                         : "=r"(sin0) : "r"(zeroSinPtr) : "r2", "r3");
                        }
                        {
                            struct RouletteItem *scaleItem;
                            scaleItem = *affineItemRef;
                            inverse = scaleItem->scale;
                        }
                        {
                            s32 result;
                            result = FixedMul(sin0, (s16)FixedInverse(inverse));
                            /* agbcc selects r1 for the spill pointer; the target reloads it in r2. */
                            asm volatile("ldr r2, [sp, #8]\nstrh r0, [r2]"
                                         : "+r"(result) : : "r2", "memory");
                        }

                        {
                            register const s16 *negativeSinPtr asm("r3");
                            register int value asm("r0");
                            negativeSinPtr = sSinCosTable;
                            value = *(u16 *)negativeSinPtr;
                            value = -value;
                            value = (s16)value;
                            negSin0 = value;
                        }
                        {
                            struct RouletteItem *scaleItem;
                            scaleItem = *affineItemRef;
                            inverse = scaleItem->scale;
                        }
                        {
                            s32 result;
                            result = FixedMul(negSin0, (s16)FixedInverse(inverse));
                            /* Match the target's r2 reload for the affine-C spill pointer. */
                            asm volatile("ldr r2, [sp, #12]\nstrh r0, [r2]"
                                         : "+r"(result) : : "r2", "memory");
                        }

                        {
                            struct RouletteItem *scaleItem;
                            scaleItem = *affineItemRef;
                            asm volatile("mov r3, #4\nldrsh r0, [r0, r3]"
                                         : "+r"(scaleItem) : : "r3");
                            inverse = (s32)scaleItem;
                        }
                        *stack.affineDPtr = FixedMul(sin64, (s16)FixedInverse(inverse));

                        {
                            register struct RouletteItem *scaleItem asm("r0");
                            register s32 scaleCheck asm("r1");
                            scaleItem = *affineItemRef;
                            asm volatile("mov r2, #4\nldrsh %0, [%1, r2]"
                                         : "=r"(scaleCheck) : "r"(scaleItem) : "r2");
                            if (scaleCheck > 512) {
                                s32 result;
                                stack.affineA = FixedMul(sin64, (s16)FixedInverse(512));
                                {
                                    register s16 *out asm("r3");
                                    result = FixedMul(sin0, (s16)FixedInverse(512));
                                    out = stack.affineBPtr;
                                    *out = result;
                                }
                                {
                                    register s16 *out asm("r1");
                                    result = FixedMul(negSin0, (s16)FixedInverse(512));
                                    out = stack.affineCPtr;
                                    *out = result;
                                }
                                {
                                    register s16 *out asm("r2");
                                    result = FixedMul(sin64, (s16)FixedInverse(512));
                                    out = stack.affineDPtr;
                                    *out = result;
                                }
                            } else if (scaleCheck <= 15) {
                                s32 result;
                                stack.affineA = FixedMul(sin64, (s16)FixedInverse(16));
                                {
                                    register s16 *out asm("r3");
                                    result = FixedMul(sin0, (s16)FixedInverse(16));
                                    out = stack.affineBPtr;
                                    *out = result;
                                }
                                {
                                    register s16 *out asm("r4");
                                    result = FixedMul(negSin0, (s16)FixedInverse(16));
                                    out = stack.affineCPtr;
                                    *out = result;
                                }
                                {
                                    register s16 *out asm("r5");
                                    result = FixedMul(sin64, (s16)FixedInverse(16));
                                    out = stack.affineDPtr;
                                    *out = result;
                                }
                            }
                        }
                    }
                    {
                        OamData *base;
                        register int next asm("r0");
                        int byteOffset;
                        register int firstValue asm("r0");
                        int matrixBase;
                        int dstOffset;
                        register s16 *valuePtrB asm("r4");
                        register s16 *valuePtrC asm("r5");
                        register s16 *valuePtrD asm("r1");
                        int value;

                        base = gOamBuffer;
                        next = stack.nextItem;
                        byteOffset = next << 5;
                        byteOffset = (int)(byteOffset + (u8 *)base);
                        firstValue = (u16)stack.affineA;
                        ((OamData *)byteOffset)->all.affineParam = firstValue;

                        value = stack.nextItem;
                        matrixBase = value << 2;
                        dstOffset = matrixBase + 1;
                        dstOffset <<= 3;
                        dstOffset += (int)base;
                        valuePtrB = stack.affineBPtr;
                        value = *(u16 *)valuePtrB;
                        ((OamData *)dstOffset)->all.affineParam = value;

                        dstOffset = matrixBase + 2;
                        dstOffset <<= 3;
                        dstOffset += (int)base;
                        valuePtrC = stack.affineCPtr;
                        value = *(u16 *)valuePtrC;
                        ((OamData *)dstOffset)->all.affineParam = value;

                        matrixBase += 3;
                        matrixBase <<= 3;
                        matrixBase += (int)base;
                        valuePtrD = stack.affineDPtr;
                        firstValue = *(u16 *)valuePtrD;
                        ((OamData *)matrixBase)->all.affineParam = firstValue;
                    }
                }
                itemIndex = stack.nextItem;
            } while (itemIndex <= 2);
        }
    }

    {
        register u8 *statePtr asm("r2");
        u32 stateValue;
        statePtr = &gMinigameState;
        stateValue = *statePtr;
        stateValue -= 7;
        stateValue = (u8)stateValue;
        if (stateValue > 3) {
            src = sRouletteAffineTableB;
            total += *src++;
            if (slot < total) {
                OamData *baseLoad;
                register struct RouletteSmallState *positionLoad asm("r4");
                struct RouletteSmallState *position;
                int attrMaskLoad;
                int attrByteMask;
                int byteOffset;
                OamData *oam;
                int xMaskLoad;
                register int xMask asm("r12");
                int totalTemp;

                baseLoad = gOamBuffer;
                positionLoad = &gRouletteMiddleResult;
                /* Force the target low-register load before moving the pointer to r8. */
                asm volatile("mov %0, %1" : "=r"(position) : "r"(positionLoad));
                /* agbcc otherwise reuses an earlier mask instead of the target setup. */
                asm volatile("mov r5, #13\nneg r5, r5\nmov sl, r5" : : : "r5");
                byteOffset = slot << 3;
                oam = (OamData *)(byteOffset + (u8 *)baseLoad);
                xMaskLoad = 0x1FF;
                xMask = xMaskLoad;
                totalTemp = total;
                slot = totalTemp - slot;
                do {
                    int value;
                    register int coord asm("r1");
                    register struct RouletteSmallState *positionLow asm("r5");
                    int out;

                    value = *src++;
                    *rawDst++ = value;
                    positionLow = position;
                    out = *(u8 *)((u8 *)positionLow + 2);
                    out += value;
                    ((u8 *)oam)[0] = out;
                    {
                        register int oldByte asm("r1");
                        oldByte = ((u8 *)oam)[1];
                        asm volatile("mov %0, sl" : "=r"(out));
                        out &= oldByte;
                        oldByte = 4;
                        out |= oldByte;
                        ((u8 *)oam)[1] = out;
                    }

                    value = *src++;
                    *rawDst++ = value;
                    out = 0;
                    coord = *(s16 *)((u8 *)positionLow + out);
                    coord = value + coord;
                    {
                        /* Keep the target's ip -> r2 mask copy. */
                        asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                    }
                    {
                        int oldAttr;
                        int maskedAttr;
                        oldAttr = oam->all.attr1;
                        maskedAttr = -512;
                        maskedAttr &= oldAttr;
                        maskedAttr |= coord;
                        oam->all.attr1 = maskedAttr;
                    }

                    out = *src;
                    *rawDst = out;
                    src++;
                    {
                        register int oldByte asm("r1");
                        register int four asm("r5");
                        oldByte = ((u8 *)oam)[5];
                        asm volatile("mov %0, sl" : "=r"(out));
                        out &= oldByte;
                        four = 4;
                        out |= four;
                        ((u8 *)oam)[5] = out;
                    }
                    rawDst += 2;
                    oam++;
                    slot--;
                } while (slot != 0);
                slot = total;
            }
        }
    }

    {
        register u8 *statePtr asm("r1");
        u32 stateValue;
        statePtr = &gMinigameState;
        stateValue = *statePtr;
        stateValue -= 7;
        stateValue = (u8)stateValue;
        if (stateValue > 1) {
            src = sRouletteAffineTableA;
            total += *src++;
            if (slot < total) {
                OamData *baseLoad;
                register struct RouletteSmallState *positionLoad asm("r2");
                struct RouletteSmallState *position;
                int byteOffset;
                OamData *oam;
                register int xMaskLoad asm("r5");
                register int xMask asm("r12");
                register int totalTemp asm("r0");

                baseLoad = gOamBuffer;
                positionLoad = &gRouletteTopResult;
                /* Force the target low-register load before moving the pointer to r8. */
                asm volatile("mov %0, %1" : "=r"(position) : "r"(positionLoad));
                /* Materialize -13 in sl at this exact loop boundary. */
                asm volatile("mov r4, #13\nneg r4, r4\nmov sl, r4" : : : "r4");
                byteOffset = slot << 3;
                oam = (OamData *)(byteOffset + (u8 *)baseLoad);
                xMaskLoad = 0x1FF;
                /* Keep the literal load in the target low register before moving it to ip. */
                asm volatile("mov %0, %1" : "=r"(xMask) : "r"(xMaskLoad));
                /* Preserve the target low-register copy of total before subtraction. */
                asm volatile("mov %0, %2\nsub %1, %0, %1"
                             : "=r"(totalTemp), "+r"(slot) : "r"(total));
                do {
                    int value;
                    register int coord asm("r1");
                    int out;

                    value = *src++;
                    *rawDst++ = value;
                    {
                        struct RouletteSmallState *positionLow;
                        positionLow = position;
                        out = *(u8 *)((u8 *)positionLow + 2);
                    }
                    out += value;
                    ((u8 *)oam)[0] = out;
                    {
                        register int oldByte asm("r1");
                        oldByte = ((u8 *)oam)[1];
                        asm volatile("mov %0, sl" : "=r"(out));
                        out &= oldByte;
                        /* The target rematerializes 4 in r2 for this OR. */
                        asm volatile("mov r2, #4\norr %0, r2" : "+r"(out) : : "r2");
                        ((u8 *)oam)[1] = out;
                    }

                    value = *src++;
                    *rawDst++ = value;
                    {
                        register struct RouletteSmallState *positionLow asm("r5");
                        positionLow = position;
                        out = 0;
                        asm volatile("ldrsh %0, [%1, %2]" : "=r"(coord) : "r"(positionLow), "r"(out));
                    }
                    coord = value + coord;
                    {
                        /* Keep the target's ip -> r2 mask copy. */
                        asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                    }
                    {
                        int oldAttr;
                        int maskedAttr;
                        oldAttr = oam->all.attr1;
                        maskedAttr = -512;
                        maskedAttr &= oldAttr;
                        maskedAttr |= coord;
                        oam->all.attr1 = maskedAttr;
                    }

                    out = *src;
                    *rawDst = out;
                    src++;
                    {
                        register int oldByte asm("r1");
                        register int four asm("r5");
                        oldByte = ((u8 *)oam)[5];
                        asm volatile("mov %0, sl" : "=r"(out));
                        out &= oldByte;
                        four = 4;
                        out |= four;
                        ((u8 *)oam)[5] = out;
                    }
                    rawDst += 2;
                    oam++;
                    slot--;
                } while (slot != 0);
                slot = total;
            }
        }
    }

    {
        register u8 *statePtr asm("r1");
        u32 stateValue;
        statePtr = &gMinigameState;
        stateValue = *statePtr;
        stateValue -= 7;
        stateValue = (u8)stateValue;
        if (stateValue > 5) {
            src = sRouletteAffineTableC;
            total += *src++;
            if (slot < total) {
                OamData *baseLoad;
                register struct RouletteSmallState *positionLoad asm("r2");
                struct RouletteSmallState *position;
                int byteOffset;
                OamData *oam;
                register int xMaskLoad asm("r5");
                register int xMask asm("r12");
                register int totalTemp asm("r0");

                baseLoad = gOamBuffer;
                positionLoad = &gRouletteBottomResult;
                /* Force the target low-register load before moving the pointer to r8. */
                asm volatile("mov %0, %1" : "=r"(position) : "r"(positionLoad));
                /* Materialize -13 in sl at this exact loop boundary. */
                asm volatile("mov r4, #13\nneg r4, r4\nmov sl, r4" : : : "r4");
                byteOffset = slot << 3;
                oam = (OamData *)(byteOffset + (u8 *)baseLoad);
                xMaskLoad = 0x1FF;
                /* Keep the literal load in the target low register before moving it to ip. */
                asm volatile("mov %0, %1" : "=r"(xMask) : "r"(xMaskLoad));
                /* Preserve the target low-register copy of total before subtraction. */
                asm volatile("mov %0, %2\nsub %1, %0, %1"
                             : "=r"(totalTemp), "+r"(slot) : "r"(total));
                do {
                    int value;
                    register int coord asm("r1");
                    int out;

                    value = *src++;
                    *rawDst++ = value;
                    {
                        struct RouletteSmallState *positionLow;
                        positionLow = position;
                        out = *(u8 *)((u8 *)positionLow + 2);
                    }
                    out += value;
                    ((u8 *)oam)[0] = out;
                    {
                        register int oldByte asm("r1");
                        oldByte = ((u8 *)oam)[1];
                        asm volatile("mov %0, sl" : "=r"(out));
                        out &= oldByte;
                        /* The target rematerializes 4 in r2 for this OR. */
                        asm volatile("mov r2, #4\norr %0, r2" : "+r"(out) : : "r2");
                        ((u8 *)oam)[1] = out;
                    }

                    value = *src++;
                    *rawDst++ = value;
                    {
                        register struct RouletteSmallState *positionLow asm("r5");
                        positionLow = position;
                        out = 0;
                        asm volatile("ldrsh %0, [%1, %2]" : "=r"(coord) : "r"(positionLow), "r"(out));
                    }
                    coord = value + coord;
                    {
                        /* Keep the target's ip -> r2 mask copy. */
                        asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                    }
                    {
                        int oldAttr;
                        int maskedAttr;
                        oldAttr = oam->all.attr1;
                        maskedAttr = -512;
                        maskedAttr &= oldAttr;
                        maskedAttr |= coord;
                        oam->all.attr1 = maskedAttr;
                    }

                    out = *src;
                    *rawDst = out;
                    src++;
                    {
                        register int oldByte asm("r1");
                        register int four asm("r5");
                        oldByte = ((u8 *)oam)[5];
                        asm volatile("mov %0, sl" : "=r"(out));
                        out &= oldByte;
                        four = 4;
                        out |= four;
                        ((u8 *)oam)[5] = out;
                    }
                    rawDst += 2;
                    oam++;
                    slot--;
                } while (slot != 0);
                slot = total;
            }
        }
    }

    {
        register u8 *animationFlag asm("r1");
        int flagValue;
        animationFlag = &gMinigameMedalHighlight;
        flagValue = *animationFlag;
        if (flagValue != 0) {
            struct RouletteAnimationState *anim;
            const struct AnimationFrame *frames;
            register int timerValue asm("r0");
            u32 frameTime;
            anim = &gMinigameUiAnimation;
            timerValue = anim->timer;
            timerValue++;
            anim->timer = timerValue;
            frames = sRouletteCenterResultOam;
            frameTime = frames[anim->frame].time;
            timerValue = (u16)timerValue;
            if (frameTime < timerValue) {
                timerValue = 1;
                anim->timer = timerValue;
                timerValue = anim->frame;
                timerValue++;
                anim->frame = timerValue;
                timerValue = anim->frame;
                if (frames[timerValue].time == 0) {
                    anim->frame = 0;
                    anim->timer = 0;
                    {
                        int zero;
                        register u8 *flagReload asm("r2");
                        /* The target materializes zero before loading the flag address. */
                        asm volatile("mov %0, #0" : "=r"(zero));
                        flagReload = &gMinigameMedalHighlight;
                        *flagReload = zero;
                    }
                    DrawRouletteScoreText();
                }
            }
        }
    }

    {
        const struct AnimationFrame *frames;
        struct RouletteAnimationState *anim;
        struct RouletteAnimationState *nextAnim;
        int frameIndex;
        frames = sRouletteCenterResultOam;
        anim = &gMinigameUiAnimation;
        frameIndex = anim->frame;
        src = frames[frameIndex].oam;
        total += *src++;
        nextAnim = &gRouletteSideAnim;
        if (slot < total) {
            OamData *base;
            register int xMaskLoad asm("r4");
            int xMask;
            int attr1MaskLoad;
            int attr1Mask;
            int attrByteLoad;
            int attrByteMask;
            int byteOffset;
            OamData *oam;
            register int totalTemp asm("r0");

            base = gOamBuffer;
            xMaskLoad = 0x1FF;
            /* Keep the target r4 literal load before moving the mask to sl. */
            asm volatile("mov %0, %1" : "=r"(xMask) : "r"(xMaskLoad));
            attr1MaskLoad = -512;
            attr1Mask = attr1MaskLoad;
            /* agbcc chooses r0 here; the target materializes -13 through r2 into r8. */
            asm volatile("mov r2, #13\nneg r2, r2\nmov %0, r2"
                         : "=r"(attrByteMask) : : "r2");
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u8 *)base);
            /* Preserve the target r0 copy of total before subtraction. */
            asm volatile("mov %0, %2\nsub %1, %0, %1"
                         : "=r"(totalTemp), "+r"(slot) : "r"(total));
            do {
                register int value asm("r2");
                register int out asm("r0");
                int coord;

                value = *src++;
                *rawDst++ = value;
                out = value;
                out += 24;
                ((u8 *)oam)[0] = out;

                value = *src++;
                *rawDst++ = value;
                coord = value;
                coord += 21;
                {
                    /* The target copies sl through r2 before masking. */
                    asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                }
                {
                    register int oldAttr asm("r2");
                    oldAttr = oam->all.attr1;
                    out = attr1Mask;
                    out &= oldAttr;
                    out |= coord;
                    oam->all.attr1 = out;
                }

                out = *src;
                *rawDst = out;
                src++;
                {
                    register int oldByte asm("r1");
                    oldByte = ((u8 *)oam)[5];
                    out = attrByteMask;
                    out &= oldByte;
                    ((u8 *)oam)[5] = out;
                }
                rawDst += 2;
                oam++;
                slot--;
            } while (slot != 0);
            slot = total;
        }

        {
            int timerValue;
            register int frameIndex2 asm("r1");
            register const struct AnimationFrame *nextFrames asm("r3");
            u32 frameTime;
            timerValue = nextAnim->timer;
            timerValue++;
            nextAnim->timer = timerValue;
            frameIndex2 = nextAnim->frame;
            frameIndex2 <<= 3;
            /*
             * agbcc hoists this literal into r7 across the preceding OAM loop,
             * clobbering the live slot counter. Keep the single target load here.
             */
            nextFrames = sRouletteSideResultOam;
            frameIndex2 = frameIndex2 + (u32)nextFrames;
            frameTime = *(u8 *)((u8 *)frameIndex2 + 4);
            timerValue = (u16)timerValue;
            if (frameTime < timerValue) {
                timerValue = 1;
                nextAnim->timer = timerValue;
                timerValue = nextAnim->frame;
                timerValue++;
                nextAnim->frame = timerValue;
                timerValue = nextAnim->frame;
                if (nextFrames[timerValue].time == 0)
                    nextAnim->frame = 0;
            }
        }

        frameIndex = nextAnim->frame;
        frameIndex <<= 3;
        {
            register const struct AnimationFrame *sideFrames asm("r4");
            const struct AnimationFrame *framePtr;
            /* Prevent reuse of the earlier literal; the target reloads it in r4 here. */
            sideFrames = sRouletteSideResultOam; asm volatile("" : "+r"(sideFrames) : : "memory");
            frameIndex = (int)(frameIndex + (u8 *)sideFrames);
            framePtr = (const struct AnimationFrame *)frameIndex;
            src = framePtr->oam;
        }
        total += *src++;
        if (slot < total) {
            OamData *base;
            int xMaskLoad;
            int xMask;
            register int attr1MaskLoad asm("r2");
            register int attr1Mask asm("r12");
            int attrByteLoad;
            int attrByteMask;
            int byteOffset;
            OamData *oam;
            register int totalTemp asm("r0");

            base = gOamBuffer;
            xMaskLoad = 0x1FF;
            xMask = xMaskLoad;
            attr1MaskLoad = -512;
            /* Keep the target r2 literal load before moving the mask to ip. */
            asm volatile("mov %0, %1" : "=r"(attr1Mask) : "r"(attr1MaskLoad));
            /* agbcc chooses r0; the target materializes -13 through r4 into r8. */
            asm volatile("mov r4, #13\nneg r4, r4\nmov %0, r4"
                         : "=r"(attrByteMask) : : "r4");
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u8 *)base);
            /* Preserve the target r0 copy of total before subtraction. */
            asm volatile("mov %0, %2\nsub %1, %0, %1"
                         : "=r"(totalTemp), "+r"(slot) : "r"(total));
            do {
                register int value asm("r2");
                register int out asm("r0");
                int coord;

                value = *src++;
                *rawDst++ = value;
                out = value;
                out += 112;
                ((u8 *)oam)[0] = out;

                value = *src++;
                *rawDst++ = value;
                coord = value;
                coord += 16;
                {
                    /* The target copies sl through r2 before masking. */
                    asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                }
                {
                    register int oldAttr asm("r2");
                    oldAttr = oam->all.attr1;
                    out = attr1Mask;
                    out &= oldAttr;
                    out |= coord;
                    oam->all.attr1 = out;
                }

                out = *src;
                *rawDst = out;
                src++;
                {
                    int oldByte;
                    oldByte = ((u8 *)oam)[5];
                    out = attrByteMask;
                    out &= oldByte;
                    oldByte = 4;
                    out |= oldByte;
                    ((u8 *)oam)[5] = out;
                }
                rawDst += 2;
                oam++;
                slot--;
            } while (slot != 0);
            slot = total;
        }

        frameIndex = nextAnim->frame;
        frameIndex <<= 3;
        {
            register const struct AnimationFrame *sideFrames asm("r3");
            const struct AnimationFrame *framePtr;
            /* The target performs a separate r3 literal load for the right-side list. */
            sideFrames = sRouletteSideResultOam;
            frameIndex = (int)(frameIndex + (u8 *)sideFrames);
            framePtr = (const struct AnimationFrame *)frameIndex;
            src = framePtr->oam;
        }
        total += *src++;
        if (slot < total) {
            OamData *base;
            register int xMaskLoad asm("r4");
            int xMask;
            register int attr1MaskLoad asm("r5");
            register int attr1Mask asm("r10");
            int attrByteMask;
            int byteOffset;
            OamData *oam;
            register int totalTemp asm("r0");

            base = gOamBuffer;
            xMaskLoad = 0x1FF;
            /* Keep the target r4 literal load before moving the mask to r8. */
            asm volatile("mov %0, %1" : "=r"(xMask) : "r"(xMaskLoad));
            attr1MaskLoad = -512;
            /* Keep the target r5 literal load before moving the mask to sl. */
            asm volatile("mov %0, %1" : "=r"(attr1Mask) : "r"(attr1MaskLoad));
            attrByteMask = -13;
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u8 *)base);
            /* Preserve the target r0 copy of total before subtraction. */
            asm volatile("mov %0, %2\nsub %1, %0, %1"
                         : "=r"(totalTemp), "+r"(slot) : "r"(total));
            do {
                register int value asm("r2");
                register int out asm("r0");
                int coord;

                value = *src++;
                *rawDst++ = value;
                out = value;
                out += 112;
                ((u8 *)oam)[0] = out;

                value = *src++;
                *rawDst++ = value;
                coord = value;
                coord += 224;
                {
                    /* The target copies r8 through r2 before masking. */
                    asm volatile("mov r2, %1\nand %0, r2" : "+r"(coord) : "r"(xMask) : "r2");
                }
                {
                    register int oldAttr asm("r2");
                    oldAttr = oam->all.attr1;
                    out = attr1Mask;
                    out &= oldAttr;
                    out |= coord;
                    oam->all.attr1 = out;
                }

                out = *src;
                *rawDst = out;
                src++;
                {
                    int oldByte;
                    oldByte = ((u8 *)oam)[5];
                    out = attrByteMask;
                    out &= oldByte;
                    oldByte = 4;
                    out |= oldByte;
                    /*
                     * agbcc otherwise inserts the literal pool between this store and
                     * the following raw OAM pointer increment. Keep only these two
                     * target instructions in one localized compiler boundary.
                     */
                    asm volatile("strb %1, [%2, #5]\nadd %0, #4"
                                 : "+r"(rawDst) : "r"(out), "r"(oam) : "memory");
                }
                oam++;
                slot--;
            } while (slot != 0);
        }
    }

    {
        register int finalTotal asm("r4");
        register u8 *slotsUsed asm("r3");
        finalTotal = total;
        slotsUsed = &gOamSlotsUsed;
        *slotsUsed = finalTotal;
    }
}

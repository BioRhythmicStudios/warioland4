#include "minigames/roulette.h"
#include "fixed_point.h"

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
    int slot;
    register int total PIN("r9");
    u16 *rawDst;
    struct RouletteMainState *mainState;
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
                src = GP(sRouletteMainState1Oam[0].oam);
            if (active == 2)
                src = GP(sRouletteMainState2Oam[0].oam);
            total += *src++;
            {
                s16 *affineBAddress;
                s16 *affineCAddress;
                s16 *affineDAddress;
                
                affineBAddress = &stack.affineB;
                GP_SET(stack.affineBPtr, affineBAddress);
                affineCAddress = &stack.affineC;
                GP_SET(stack.affineCPtr, affineCAddress);
                affineDAddress = &stack.affineD;
                GP_SET(stack.affineDPtr, affineDAddress);
            }
            if (slot < total) {
                struct RouletteMainState *position;
                OamData *oam;
                int xMask;
                int attr2Mask;
                position = mainState;
                oam = oamBase;
                xMask = 0x1FF;
                attr2Mask = -13;
                slot = total;
                do {
                    {
                        int value;
                        value = *src++;
                        *rawDst++ = value;
                        ((u8 *)oam)[0] = position->y + value - 16;
                        ((u8 *)oam)[1] |= 3;
                    }
                    {
                        int value;
                        int coord;

                        int maskTemp;
                        value = *src++;
                        *rawDst++ = value;

                        coord = (s16)position->x;
                        coord = (value) - (-(coord));
                        coord -= 32;
                        
                        maskTemp = xMask;
                        
                        coord &= maskTemp;
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
                        int oldByte;
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
                s32 negSin0;
                u32 affineDResult;

                {
                    const s16 *sinPtr;
                    struct RouletteMainState *scalePtr;
                    s32 scaleValue;
                    u16 result;
                    sinPtr = sSinCosTable;
                    sinPtr += 64;
                    
                    {
                        int zero;
                        zero = 0;
                        sin64 = *sinPtr;
                    }

                    scalePtr = mainState;
                    scaleValue = *(s16 *)&scalePtr->scale;
                    result = FixedMul(sin64, (s16)FixedInverse(scaleValue));
                    stack.affineA = result;
                }

                {
                    const s16 *sinPtr;
                    s32 scaleValue;
                    s32 result;
                    s16 *outPtr;
                    sinPtr = sSinCosTable;
                    {
                        int zero;
                        zero = 0;
                        sin0 = *sinPtr;
                    }
                    {
                        struct RouletteMainState *scalePointer;
                        int scaleOffset;
                        scalePointer = mainState;
                        scaleOffset = 4;
                        scaleValue = (s16)scalePointer->scale;
                    }
                    result = FixedMul(sin0, (s16)FixedInverse(scaleValue));
                    outPtr = GP(stack.affineBPtr);
                    *outPtr = result;
                }

                {
                    const s16 *sinPtr;
                    s32 scaleValue;
                    s32 result;
                    s16 *outPtr;
                    sinPtr = sSinCosTable;
                    negSin0 = (u16)*sinPtr;
                    negSin0 = -negSin0;
                    negSin0 = (s16)negSin0;
                    {
                        struct RouletteMainState *scalePointer;
                        int scaleOffset;
                        scalePointer = mainState;
                        scaleOffset = 4;
                        scaleValue = (s16)scalePointer->scale;
                    }
                    result = FixedMul(negSin0, (s16)FixedInverse(scaleValue));
                    outPtr = GP(stack.affineCPtr);
                    *outPtr = result;
                }

                {
                    struct RouletteMainState *scalePtr;
                    s32 scaleValue;
                    s32 result;
                    s16 *outPtr;

                    scalePtr = mainState;
                    scaleValue = *(s16 *)&scalePtr->scale;
                    result = FixedMul(sin64, (s16)FixedInverse(scaleValue));
                    affineDResult = (u16)result;
                    outPtr = GP(stack.affineDPtr);
                    *outPtr = affineDResult;
                }

                gOamBuffer[0].all.affineParam = stack.affineA;
                {
                    s16 *affineBAddress;
                    int value;
                    affineBAddress = GP(stack.affineBPtr);
                    value = *(u16 *)affineBAddress;
                    gOamBuffer[1].all.affineParam = value;
                }
                {
                    s16 *affineCAddress;
                    int value;
                    affineCAddress = GP(stack.affineCPtr);
                    value = *(u16 *)affineCAddress;
                    gOamBuffer[2].all.affineParam = value;
                }
                gOamBuffer[3].all.affineParam = affineDResult;
            }
        }
    }

    {
        u8 *statePtr;
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
        src = GP(sRoulettePromptOam[gRoulettePromptAnim.frame].oam);
        total += *src++;
        if (slot < total) {
            OamData *oam;
            OamData *oamBase2;
            int xMask;
            int xMaskLoad;
            int attr1Mask;
            int attr1Load;
            int attr2Mask;
            int byteOffset;
            int totalTemp;
            oamBase2 = gOamBuffer;
            xMaskLoad = 0x1FF;
            
            xMask = xMaskLoad;
            attr1Load = 0xFFFFFE00;
            attr1Mask = attr1Load;
            attr2Mask = 13;
            attr2Mask = -attr2Mask;
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u32)oamBase2);
            
            totalTemp = total;
            slot = totalTemp - slot;
            do {
                int value;
                int coord;
                value = *src++;
                *rawDst++ = value;
                {
                    int out;
                    out = value;
                    out += 64;
                    ((u8 *)oam)[0] = out;
                }

                value = *src++;
                *rawDst++ = value;
                coord = value;
                coord += 140;
                {
                    int maskTemp;
                    
                    maskTemp = xMask;
                    
                    coord &= maskTemp;
                }
                {
                    int oldAttr;
                    int maskedAttr;
                    oldAttr = oam->all.attr1;
                    maskedAttr = attr1Mask;
                    maskedAttr &= oldAttr;
                    maskedAttr |= coord;
                    oam->all.attr1 = maskedAttr;
                }

                {
                    int out;
                    int oldByte;
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
        u8 *rouletteStatePtr;
        rouletteStatePtr = &gMinigameState;
        if (*rouletteStatePtr > 6) {
            int itemIndex;
            itemIndex = 0;
            do {
                GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) currentItemRef;
                GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) itemsBase;
                int itemOffset;
                int nextItem;
                struct RouletteItem *itemPtr;
                int itemZero;
                int itemX;
                int itemLimit;
                itemsBase = gRouletteItemDrawOrder;
                itemOffset = itemIndex << 2;
                itemsBase = (GBA_TYPEOF(struct RouletteItem **, itemsBase))(itemOffset + (u32)itemsBase);
                itemPtr = GP(*itemsBase);
                itemZero = 0;
                itemX = itemPtr->x;
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
                        GBA_TYPEOF(const u16 **, &(gRouletteItemOamTable)[0]) animBase;
                        GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) itemsBase2;
                        int savedOffset;
                        animBase = gRouletteItemOamTable;
                        itemsBase2 = gRouletteItemDrawOrder;
                        savedOffset = stack.itemOffset;
                        currentItemRef = (GBA_TYPEOF(struct RouletteItem **, currentItemRef))(savedOffset + (u32)itemsBase2);
                        src = GP(animBase[(GP(*currentItemRef))->value]);
                    }
                    total += *src++;
                    {
                        s16 *affineBAddress;
                        s16 *affineCAddress;
                        affineBAddress = &stack.affineB;
                        GP_SET(stack.affineBPtr, affineBAddress);
                        affineCAddress = &stack.affineC;
                        GP_SET(stack.affineCPtr, affineCAddress);
                        affineBAddress += 2;
                        GP_SET(stack.affineDPtr, affineBAddress);
                    }
                    itemIndex++;
                    stack.nextItem = itemIndex;
                    if (slot < total) {
                        int matrixIndex;
                        int nibbleMask;
                        int nibbleLoad;
                        int attr1Mask;
                        int attr1Load;
                        GP_SET(stack.itemRef, currentItemRef);
                        matrixIndex = itemIndex;
                        nibbleLoad = 15;

                        nibbleMask = nibbleLoad;
                        attr1Load = 0xFFFFFE00;
                        
                        attr1Mask = attr1Load;
                        do {
                            int value;
                            int byteOffset;
                            OamData *oam;
                            OamData *base;
                            GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) savedRef;
                            struct RouletteItem *savedItem;
                            int coord;

                            value = *src++;
                            *rawDst++ = value;
                            byteOffset = slot << 3;
                            base = gOamBuffer;
                            oam = (OamData *)(byteOffset + (u32)base);
                            savedRef = GP(stack.itemRef);
                            savedItem = GP(*savedRef);
                            ((u8 *)oam)[0] = savedItem->y + value - 16;
                            ((u8 *)oam)[1] |= 3;

                            value = *src++;
                            *rawDst++ = value;
                            {
                                GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) ref;
                                struct RouletteItem *cur;
                                int zero;
                                ref = GP(stack.itemRef);
                                cur = GP(*ref);
                                zero = 0;
                                coord = cur->x;
                            }
                            coord = value + coord;
                            coord -= 32;
                            coord &= 0x1FF;
                            {
                                int oldAttr;
                                int maskedAttr;
                                oldAttr = oam->all.attr1;
                                maskedAttr = attr1Mask;
                                maskedAttr &= oldAttr;
                                maskedAttr |= coord;
                                oam->all.attr1 = maskedAttr;
                            }
                            {
                                int bits;
                                int seven;
                                int oldByte;
                                int mask;
                                seven = 7;
                                bits = matrixIndex;
                                bits &= seven;
                                bits <<= 1;
                                oldByte = ((u8 *)oam)[3];
                                
                                seven = 15;
                                
                                mask = seven;
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
                                    u8 *itemState;
                                    u32 itemStateValue;
                                    itemState = &gMinigameState;
                                    
                                    itemStateValue = *itemState;
                                    if (itemStateValue > 10) {
                                        u32 low;
                                        u32 high;
                                        u32 maskTemp;
                                        high = priority >> 4;
                                        high++;
                                        high <<= 4;
                                        
                                        low = priority;
                                        maskTemp = nibbleMask;
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
                                        u32 high;
                                        high = priority >> 4;
                                        high++;
                                        high <<= 4;
                                        {
                                            u32 maskTemp;
                                            
                                            low = priority;
                                            
                                            maskTemp = nibbleMask;
                                            
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
                        const s16 *sinPtr;
                        s32 sin64;
                        GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) affineItemRef;
                        int savedOffset;
                        GBA_TYPEOF(struct RouletteItem **, &(gRouletteItemDrawOrder)[0]) itemsBase3;
                        s32 sin0;
                        s32 negSin0;

                        sinPtr = sSinCosTable;
                        sinPtr += 64;
                        
                        sin64 = *sinPtr;
                        savedOffset = stack.itemOffset;
                        itemsBase3 = gRouletteItemDrawOrder;
                        affineItemRef = (GBA_TYPEOF(struct RouletteItem **, affineItemRef))(savedOffset + (u32)itemsBase3);

                        {
                            struct RouletteItem *scaleItem;
                            scaleItem = GP(*affineItemRef);
                            inverse = scaleItem->scale;
                        }
                        stack.affineA = FixedMul(sin64, (s16)FixedInverse(inverse));

                        {
                            const s16 *zeroSinPtr;
                            u32 sinZero;
                            zeroSinPtr = sSinCosTable;
                            sinZero = 0;
                            
                            {
                                s32 loadedSin;
                                loadedSin = *zeroSinPtr;
                                sin0 = loadedSin;
                            }
                        }
                        {
                            struct RouletteItem *scaleItem;
                            scaleItem = GP(*affineItemRef);
                            inverse = scaleItem->scale;
                        }
                        {
                            s32 result;
                            result = FixedMul(sin0, (s16)FixedInverse(inverse));
                            {
                                s16 *out;
                                out = GP(stack.affineBPtr);
                                *out = result;
                            }
                        }

                        {
                            const s16 *negativeSinPtr;
                            int value;
                            negativeSinPtr = sSinCosTable;
                            value = *(u16 *)negativeSinPtr;
                            value = -value;
                            value = (s16)value;
                            negSin0 = value;
                        }
                        {
                            struct RouletteItem *scaleItem;
                            scaleItem = GP(*affineItemRef);
                            inverse = scaleItem->scale;
                        }
                        {
                            s32 result;
                            result = FixedMul(negSin0, (s16)FixedInverse(inverse));
                            {
                                s16 *out;
                                out = GP(stack.affineCPtr);
                                *out = result;
                            }
                        }

                        {
                            struct RouletteItem *scaleItem;
                            u32 scaleOffset;
                            scaleItem = GP(*affineItemRef);
                            scaleOffset = 4;
                            inverse = scaleItem->scale;
                        }
                        *GP(stack.affineDPtr) = FixedMul(sin64, (s16)FixedInverse(inverse));

                        {
                            struct RouletteItem *scaleItem;
                            s32 scaleCheck;
                            u32 scaleOffset;
                            scaleItem = GP(*affineItemRef);
                            scaleOffset = 4;
                            scaleCheck = scaleItem->scale;
                            if (scaleCheck > 512) {
                                s32 result;
                                stack.affineA = FixedMul(sin64, (s16)FixedInverse(512));
                                {
                                    s16 *out;
                                    result = FixedMul(sin0, (s16)FixedInverse(512));
                                    out = GP(stack.affineBPtr);
                                    *out = result;
                                }
                                {
                                    s16 *out;
                                    result = FixedMul(negSin0, (s16)FixedInverse(512));
                                    out = GP(stack.affineCPtr);
                                    *out = result;
                                }
                                {
                                    s16 *out;
                                    result = FixedMul(sin64, (s16)FixedInverse(512));
                                    out = GP(stack.affineDPtr);
                                    *out = result;
                                }
                            } else if (scaleCheck <= 15) {
                                s32 result;
                                stack.affineA = FixedMul(sin64, (s16)FixedInverse(16));
                                {
                                    s16 *out;
                                    result = FixedMul(sin0, (s16)FixedInverse(16));
                                    out = GP(stack.affineBPtr);
                                    *out = result;
                                }
                                {
                                    s16 *out;
                                    result = FixedMul(negSin0, (s16)FixedInverse(16));
                                    out = GP(stack.affineCPtr);
                                    *out = result;
                                }
                                {
                                    s16 *out;
                                    result = FixedMul(sin64, (s16)FixedInverse(16));
                                    out = GP(stack.affineDPtr);
                                    *out = result;
                                }
                            }
                        }
                    }
                    {
                        OamData *base;
                        int next;
                        int byteOffset;
                        int firstValue;
                        int matrixBase;
                        int dstOffset;
                        s16 *valuePtrB;
                        s16 *valuePtrC;
                        s16 *valuePtrD;
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
                        valuePtrB = GP(stack.affineBPtr);
                        value = *(u16 *)valuePtrB;
                        ((OamData *)dstOffset)->all.affineParam = value;

                        dstOffset = matrixBase + 2;
                        dstOffset <<= 3;
                        dstOffset += (int)base;
                        valuePtrC = GP(stack.affineCPtr);
                        value = *(u16 *)valuePtrC;
                        ((OamData *)dstOffset)->all.affineParam = value;

                        matrixBase += 3;
                        matrixBase <<= 3;
                        matrixBase += (int)base;
                        valuePtrD = GP(stack.affineDPtr);
                        firstValue = *(u16 *)valuePtrD;
                        ((OamData *)matrixBase)->all.affineParam = firstValue;
                    }
                }
                itemIndex = stack.nextItem;
            } while (itemIndex <= 2);
        }
    }

    {
        u8 *statePtr;
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
                struct RouletteSmallState *positionLoad;
                struct RouletteSmallState *position;
                int attrMaskLoad;
                int attrByteMask;
                int byteOffset;
                OamData *oam;
                int xMaskLoad;
                int xMask;
                int totalTemp;

                baseLoad = gOamBuffer;
                positionLoad = &gRouletteMiddleResult;
                
                position = positionLoad;
                
                {
                    u8 seed;
                    seed = 13;
                    {
                        int negSeed;
                        negSeed = -(int)seed;
                        attrByteMask = negSeed;
                    }
                }
                byteOffset = slot << 3;
                oam = (OamData *)(byteOffset + (u8 *)baseLoad);
                xMaskLoad = 0x1FF;
                xMask = xMaskLoad;
                totalTemp = total;
                slot = totalTemp - slot;
                do {
                    int value;
                    int coord;
                    struct RouletteSmallState *positionLow;
                    int out;

                    value = *src++;
                    *rawDst++ = value;
                    positionLow = position;
                    out = (u8)positionLow->y;
                    out += value;
                    ((u8 *)oam)[0] = out;
                    {
                        int oldByte;
                        oldByte = ((u8 *)oam)[1];
                        out = attrByteMask;
                        out &= oldByte;
                        oldByte = 4;
                        out |= oldByte;
                        ((u8 *)oam)[1] = out;
                    }

                    value = *src++;
                    *rawDst++ = value;
                    coord = (s16)positionLow->x;
                    coord = value + coord;
                    {
                        
                        {
                            int mask;
                            mask = xMask;
                            coord &= mask;
                        }
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
                        int oldByte;
                        int four;
                        oldByte = ((u8 *)oam)[5];
                        out = attrByteMask;
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
        u8 *statePtr;
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
                struct RouletteSmallState *positionLoad;
                struct RouletteSmallState *position;
                int byteOffset;
                OamData *oam;
                int xMaskLoad;
                int xMask;
                int totalTemp;
                int attrByteMask;

                baseLoad = gOamBuffer;
                positionLoad = &gRouletteTopResult;
                
                position = positionLoad;
                
                {
                    u8 seed;
                    seed = 13;
                    {
                        int negSeed;
                        negSeed = -(int)seed;
                        attrByteMask = negSeed;
                    }
                }
                byteOffset = slot << 3;
                oam = (OamData *)(byteOffset + (u8 *)baseLoad);
                xMaskLoad = 0x1FF;
                
                xMask = xMaskLoad;
                
                totalTemp = total;
                slot = totalTemp - slot;
                do {
                    int value;
                    int coord;
                    int out;

                    value = *src++;
                    *rawDst++ = value;
                    {
                        struct RouletteSmallState *positionLow;
                        positionLow = position;
                        out = (u8)positionLow->y;
                    }
                    out += value;
                    ((u8 *)oam)[0] = out;
                    {
                        int oldByte;
                        oldByte = ((u8 *)oam)[1];
                        out = attrByteMask;
                        out &= oldByte;
                        
                        {
                            int four;
                            four = 4;
                            out |= four;
                        }
                        ((u8 *)oam)[1] = out;
                    }

                    value = *src++;
                    *rawDst++ = value;
                    {
                        struct RouletteSmallState *positionLow;
                        positionLow = position;
                        out = 0;
                        coord = (s16)positionLow->x;
                    }
                    coord = value + coord;
                    {
                        
                        {
                            int mask;
                            mask = xMask;
                            coord &= mask;
                        }
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
                        int oldByte;
                        int four;
                        oldByte = ((u8 *)oam)[5];
                        out = attrByteMask;
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
        u8 *statePtr;
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
                struct RouletteSmallState *positionLoad;
                struct RouletteSmallState *position;
                int byteOffset;
                OamData *oam;
                int xMaskLoad;
                int xMask;
                int totalTemp;
                int attrByteMask;

                baseLoad = gOamBuffer;
                positionLoad = &gRouletteBottomResult;
                
                position = positionLoad;
                
                {
                    u8 seed;
                    seed = 13;
                    {
                        int negSeed;
                        negSeed = -(int)seed;
                        attrByteMask = negSeed;
                    }
                }
                byteOffset = slot << 3;
                oam = (OamData *)(byteOffset + (u8 *)baseLoad);
                xMaskLoad = 0x1FF;
                
                xMask = xMaskLoad;
                
                totalTemp = total;
                slot = totalTemp - slot;
                do {
                    int value;
                    int coord;
                    int out;

                    value = *src++;
                    *rawDst++ = value;
                    {
                        struct RouletteSmallState *positionLow;
                        positionLow = position;
                        out = (u8)positionLow->y;
                    }
                    out += value;
                    ((u8 *)oam)[0] = out;
                    {
                        int oldByte;
                        oldByte = ((u8 *)oam)[1];
                        out = attrByteMask;
                        out &= oldByte;
                        
                        {
                            int four;
                            four = 4;
                            out |= four;
                        }
                        ((u8 *)oam)[1] = out;
                    }

                    value = *src++;
                    *rawDst++ = value;
                    {
                        struct RouletteSmallState *positionLow;
                        positionLow = position;
                        out = 0;
                        coord = (s16)positionLow->x;
                    }
                    coord = value + coord;
                    {
                        
                        {
                            int mask;
                            mask = xMask;
                            coord &= mask;
                        }
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
                        int oldByte;
                        int four;
                        oldByte = ((u8 *)oam)[5];
                        out = attrByteMask;
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
        u8 *animationFlag ;
        int flagValue;
        animationFlag = &gMinigameMedalHighlight;
        flagValue = *animationFlag;
        if (flagValue != 0) {
            struct RouletteAnimationState *anim;
            const struct AnimationFrame *frames;
            int timerValue;
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
                        u8 *flagReload;
                        
                        zero = 0;
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
        src = GP(frames[frameIndex].oam);
        total += *src++;
        nextAnim = &gRouletteSideAnim;
        if (slot < total) {
            OamData *base;
            int xMaskLoad;
            int xMask;
            int attr1MaskLoad;
            int attr1Mask;
            int attrByteLoad;
            int attrByteMask;
            int byteOffset;
            OamData *oam;
            int totalTemp;

            base = gOamBuffer;
            xMaskLoad = 0x1FF;
            
            xMask = xMaskLoad;
            attr1MaskLoad = -512;
            attr1Mask = attr1MaskLoad;
            
            {
                u8 seed;
                seed = 13;
                {
                    int negSeed;
                    negSeed = -(int)seed;
                    attrByteMask = negSeed;
                }
            }
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u8 *)base);
            
            totalTemp = total;
            slot = totalTemp - slot;
            do {
                int value;
                int out;
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
                    
                    {
                            int mask;
                            mask = xMask;
                            coord &= mask;
                        }
                }
                {
                    int oldAttr;
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
            int frameIndex2;
            const struct AnimationFrame *nextFrames;
            u32 frameTime;
            timerValue = nextAnim->timer;
            timerValue++;
            nextAnim->timer = timerValue;
            frameIndex2 = nextAnim->frame;
            frameIndex2 <<= 3;
            
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
            const struct AnimationFrame *sideFrames;
            const struct AnimationFrame *framePtr;
            
            sideFrames = sRouletteSideResultOam; 
            frameIndex = (int)(frameIndex + (u8 *)sideFrames);
            framePtr = (const struct AnimationFrame *)frameIndex;
            src = GP(framePtr->oam);
        }
        total += *src++;
        if (slot < total) {
            OamData *base;
            int xMaskLoad;
            int xMask;
            int attr1MaskLoad;
            int attr1Mask;
            int attrByteLoad;
            int attrByteMask;
            int byteOffset;
            OamData *oam;
            int totalTemp;

            base = gOamBuffer;
            xMaskLoad = 0x1FF;
            xMask = xMaskLoad;
            attr1MaskLoad = -512;
            
            attr1Mask = attr1MaskLoad;
            
            { 
                u8 seed; 
                seed = 13;
                attrByteMask = -(int)seed; 
            }
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u8 *)base);
            
            totalTemp = total; slot = totalTemp - slot;
            do {
                int value;
                int out;
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
                    
                    {
                            int mask;
                            mask = xMask;
                            coord &= mask;
                        }
                }
                {
                    int oldAttr;
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
            const struct AnimationFrame *sideFrames;
            const struct AnimationFrame *framePtr;
            
            sideFrames = sRouletteSideResultOam;
            frameIndex = (int)(frameIndex + (u8 *)sideFrames);
            framePtr = (const struct AnimationFrame *)frameIndex;
            src = GP(framePtr->oam);
        }
        total += *src++;
        if (slot < total) {
            OamData *base;
            int xMaskLoad;
            int xMask;
            int attr1MaskLoad;
            int attr1Mask;
            int attrByteMask;
            int byteOffset;
            OamData *oam;
            int totalTemp;

            base = gOamBuffer;
            xMaskLoad = 0x1FF;
            
            xMask = xMaskLoad;
            attr1MaskLoad = -512;
            
            attr1Mask = attr1MaskLoad;
            attrByteMask = -13;
            byteOffset = slot << 3;
            oam = (OamData *)(byteOffset + (u8 *)base);
            
            totalTemp = total; slot = totalTemp - slot;
            do {
                int value;
                int out;
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
                    
                    {
                            int mask;
                            mask = xMask;
                            coord &= mask;
                        }
                }
                {
                    int oldAttr;
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
                    
                    ((u8 *)oam)[5] = out; rawDst += 2;
                }
                oam++;
                slot--;
            } while (slot != 0);
        }
    }

    {
        int finalTotal;
        u8 *slotsUsed;
        finalTotal = total;
        slotsUsed = &gOamSlotsUsed;
        *slotsUsed = finalTotal;
    }
}

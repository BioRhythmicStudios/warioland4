#include "stage_ejection.h"
#include "fixed_point.h"
#include "gba/m4a.h"
#include "init_helpers.h"
#include "minigame.h"
#include "score.h"
#include "stage_select.h"
#include "voice_set.h"

extern u8 gUnk_3003C4A;
extern u16 gStageEntryMainSpriteState[];
extern s16 gStageEntryHorizontalScroll[];
extern u16 gStageEntrySequenceTimer;

extern const s32 sStageEjectionParticleSpawnDelays[];
extern const s32 sStageEjectionParticleYVelocities[];
extern const s32 sStageEjectionParticleXVelocities[];
extern const s32 sStageEjectionParticleSpawnOffsets[];
extern const s32 sStageEjectionTreasureSpawnYThresholds[];
extern GBA_PTR(const u16*) const sStageEjectionTreasureOamFrames[];
extern const s32 sStageEjectionTreasureYVelocities[];
extern const s32 sStageEjectionTreasureXVelocities[];
extern const u16 sStageEjectionWarioOam[];
extern const struct AnimationFrame sStageEjectionParticleAnimation[];
extern const u8 sStageEjectionObjPalette[];
extern const u8 sStageEjectionObjTiles[];
extern const u8 sStageEjectionPassagePalettes[];
extern const u8 sStageEjectionPassageExtraPalettes[];
extern const u8 sStageEjectionBgPalette[];
extern const u8 sStageEjectionBgTiles[];
extern const u8 sStageEjectionBgTilemap[];

struct AGB_ALIGN Dma3RegsStageEjection {
    GBA_PTR(const void*) src;
    GBA_PTR(void*) dst;
    u32 control;
};
#define DMA3_STAGE_EJECTION ((volatile struct Dma3RegsStageEjection *)0x040000D4)

s32 UpdateStageEjectionExitState(void)
{
    if ((s16)gStageEntryMainSpriteState[5] > 0xB40) {
        ClearStageEjectionCollectionFlags();
        return 1;
    }
    UpdateStageEjectionArcMovement();
    return 0;
}

void InitializeStageEjection(void)
{
    u8* passageFlag;
    u8* base;
    u8* sprite;
    s32 scoreStep;
    s16* stageEntryY;
    u8 curveTimer;

    if ((gCurrentPassage == 0) || (gCurrentPassage == 5)) {
        passageFlag = &gUnk_3003C4A;
        curveTimer = 1;
    } else {
        passageFlag = &gUnk_3003C4A;
        curveTimer = 0;
    }
    *passageFlag = curveTimer;
    InitializeVideoMemory();

    GP_SET(DMA3_STAGE_EJECTION->src, sStageEjectionObjPalette);
    GP_SET(DMA3_STAGE_EJECTION->dst, (void*)0x05000200);
    DMA3_STAGE_EJECTION->control = 0x80000100;
    DMA3_STAGE_EJECTION->control;

    GP_SET(DMA3_STAGE_EJECTION->src, sStageEjectionBgPalette);
    GP_SET(DMA3_STAGE_EJECTION->dst, (void*)0x05000000);
    DMA3_STAGE_EJECTION->control = 0x80000100;
    DMA3_STAGE_EJECTION->control;
    {
        register u32 mask PIN("r1");
        volatile struct Dma3RegsStageEjection* dma;
        s32 control;
        dma = DMA3_STAGE_EJECTION;
        control = dma->control;
        mask = 0x80000000;
        if (control < 0) {
            do {
                control = dma->control & mask;
            } while (control != 0);
        }
    }

    GP_SET(DMA3_STAGE_EJECTION->src, sStageEjectionBgTiles);
    GP_SET(DMA3_STAGE_EJECTION->dst, (void*)0x06000000);
    DMA3_STAGE_EJECTION->control = 0x80000500;
    DMA3_STAGE_EJECTION->control;
    {
        u32 control;
        u32 mask;
        volatile struct Dma3RegsStageEjection* dma;
        dma = DMA3_STAGE_EJECTION;
        control = dma->control;
        mask = 0x80000000;
        if ((s32)control < 0) {
            do {
                control = dma->control;
                control &= mask;
            } while (control != 0);
        }
    }

    GP_SET(DMA3_STAGE_EJECTION->src, sStageEjectionObjTiles);
    GP_SET(DMA3_STAGE_EJECTION->dst, (void*)0x06010000);
    DMA3_STAGE_EJECTION->control = 0x80002000;
    DMA3_STAGE_EJECTION->control;
    {
        u32 control;
        u32 mask;
        volatile struct Dma3RegsStageEjection* dma;
        dma = DMA3_STAGE_EJECTION;
        control = dma->control;
        mask = 0x80000000;
        if ((s32)control < 0) {
            do {
                control = dma->control;
                control &= mask;
            } while (control != 0);
        }
    }

    if (gUnk_3003C4A == 0) {
        GP_SET(DMA3_STAGE_EJECTION->src, &sStageEjectionPassageExtraPalettes[(gCurrentPassage - 1) << 5]);
        GP_SET(DMA3_STAGE_EJECTION->dst, (void*)0x050003C0);
        DMA3_STAGE_EJECTION->control = 0x80000010;
        DMA3_STAGE_EJECTION->control;
        {
            u32 control;
            u32 mask;
            volatile struct Dma3RegsStageEjection* dma;
            dma = DMA3_STAGE_EJECTION;
            control = dma->control;
            mask = 0x80000000;
            if ((s32)control < 0) {
                do {
                    control = dma->control;
                    control &= mask;
                } while (control != 0);
            }
        }
    }

    GP_SET(DMA3_STAGE_EJECTION->src, &sStageEjectionPassagePalettes[gCurrentPassage << 5]);
    GP_SET(DMA3_STAGE_EJECTION->dst, (void*)0x050003A0);
    DMA3_STAGE_EJECTION->control = 0x80000010;
    DMA3_STAGE_EJECTION->control;
    {
        u32 control;
        u32 mask;
        volatile struct Dma3RegsStageEjection* dma;
        dma = DMA3_STAGE_EJECTION;
        control = dma->control;
        mask = 0x80000000;
        if ((s32)control < 0) {
            do {
                control = dma->control;
                control &= mask;
            } while (control != 0);
        }
    }

    GP_SET(DMA3_STAGE_EJECTION->src, sStageEjectionBgTilemap);
    GP_SET(DMA3_STAGE_EJECTION->dst, (void*)0x0600C000);
    DMA3_STAGE_EJECTION->control = 0x80000400;
    DMA3_STAGE_EJECTION->control;

    *(vu16*)0x04000008 = 0x1880;
    m4aSongNumStart(0x1CEU);
    VoiceSetPlay(6);
    InitMinigameScoreDisplay();
    *(vu16*)0x04000000 = 0x1100;

    gStageEjectionParticleIndex = 0;
    gStageEjectionParticleSpawnTimer = 0;

    base = (u8*)gStageEjectionParticles;
    {
        s32 zero;
        zero = 0;
        sprite = base + 0x270;
        do {
            sprite[0xC] = zero;
            sprite -= 0x10;
        } while ((s32)sprite >= (s32)base);
    }

    gStageEjectionTreasureIndex = 0;
    AdvanceStageEjectionTreasureIndex();

    base = (u8*)gStageEjectionTreasureParticles;
    {
        s32 zero;
        zero = 0;
        sprite = base;
        sprite += 0x40;
        do {
            sprite[0xC] = zero;
            sprite -= 0x10;
        } while ((s32)sprite >= (s32)base);
    }

    if (gStageScore == 0) {
        gStageEjectionParticleSpawnCount = (u8)gStageScore;
    } else {
        u8* scoreCounter;

        scoreCounter = &gStageEjectionParticleSpawnCount;
        scoreStep = ((s32)gStageScore / 200) + 1;
        *scoreCounter = scoreStep;
        if ((u32)(u8)scoreStep > 5U) {
            *scoreCounter = 5;
        }
    }

    {
        s32 zero;

        stageEntryY = &gStageEntryHorizontalScroll;
        zero = 0;
        stageEntryY[1] = zero;
        gStageEntryMainSpriteState[4] = 0x780;
        gStageEntryMainSpriteState[5] = 0xFF00;
        gStageEntryMainSpriteState[6] = zero;
        gStageEntryMainSpriteState[7] = 0x1000;
        gStageEntryMainSpriteState[8] = 0x1000;
        gStageEntrySequenceTimer = zero;
        *(vu16*)0x04000012 = zero;
        *(vu16*)0x04000010 = zero;
    }
}

void UpdateStageEjectionParticles(void)
{
    struct StageEjectionParticle* particle;
    u16 y;
    s32 signedY;
    u16 x;
    s32 signedX;

    if (gStageEjectionParticleSpawnCount == 0) {
        return;
    }
    gStageEjectionParticleSpawnTimer++;
    if (gStageEjectionParticleSpawnTimer == sStageEjectionParticleSpawnDelays[gStageEjectionParticleSpawnCount - 1]) {
        gStageEjectionParticles[gStageEjectionParticleIndex].active = 1;
        gStageEjectionParticles[gStageEjectionParticleIndex].x =
            gStageEntryMainSpriteState[4] + sStageEjectionParticleSpawnOffsets[MinigameRandom() % 6];
        gStageEjectionParticles[gStageEjectionParticleIndex].y =
            gStageEntryMainSpriteState[5] + sStageEjectionParticleSpawnOffsets[MinigameRandom() % 6];
        gStageEjectionParticles[gStageEjectionParticleIndex].animationTimer = 0;
        gStageEjectionParticles[gStageEjectionParticleIndex].frame = 0;
        if (gStageEjectionParticleIndex & 1) {
            gStageEjectionParticles[gStageEjectionParticleIndex].xVelocity =
                sStageEjectionParticleXVelocities[MinigameRandom() % 6];
        } else {
            gStageEjectionParticles[gStageEjectionParticleIndex].xVelocity =
                -sStageEjectionParticleXVelocities[MinigameRandom() % 6];
        }
        gStageEjectionParticles[gStageEjectionParticleIndex].yVelocity =
            sStageEjectionParticleYVelocities[MinigameRandom() % 6];
        gStageEjectionParticleSpawnTimer = 0;
        gStageEjectionParticleIndex++;
        if (gStageEjectionParticleIndex == 0x27) {
            gStageEjectionParticleIndex = 0;
        }
    }
    {
        s32 xLimit;
        s32 yMin;
        s32 yMax;
        struct StageEjectionParticle* end;
        s32 offset;

        xLimit = 0xF00;
        yMin = -0x100;
        particle = gStageEjectionParticles;
        yMax = 0xB40;
        offset = 0x9C;
        offset <<= 2;
        end = (struct StageEjectionParticle*)((u8*)particle + offset);
        do {
            x = particle->x;
            signedX = *(s16*)&particle->x;
            if ((signedX > xLimit) || ((signedY = (s16)(y = particle->y)) < yMin) || (signedY > yMax)) {
                particle->active = 0;
            } else if (particle->active != 0) {
                particle->y = y + particle->yVelocity;
                particle->x = x + particle->xVelocity;
            }
            particle++;
        } while ((s32)particle <= (s32)end);
    }
}

void UpdateStageEjectionTreasureParticles(void)
{
    if (((u32)gStageEjectionTreasureIndex <= 4U) &&
        ((s32)(s16)gStageEntryMainSpriteState[5] >
         sStageEjectionTreasureSpawnYThresholds[gStageEjectionTreasureIndex])) {
        gStageEjectionTreasureParticles[gStageEjectionTreasureIndex].active = 1;
        gStageEjectionTreasureParticles[gStageEjectionTreasureIndex].x = gStageEntryMainSpriteState[4];
        gStageEjectionTreasureParticles[gStageEjectionTreasureIndex].y = gStageEntryMainSpriteState[5];

        if (1 & gStageEjectionTreasureIndex) {
            gStageEjectionTreasureParticles[gStageEjectionTreasureIndex].xVelocity =
                sStageEjectionTreasureXVelocities[MinigameRandom() % 6];
        } else {
            gStageEjectionTreasureParticles[gStageEjectionTreasureIndex].xVelocity =
                -sStageEjectionTreasureXVelocities[MinigameRandom() % 6];
        }

        gStageEjectionTreasureParticles[gStageEjectionTreasureIndex].yVelocity =
            sStageEjectionTreasureYVelocities[MinigameRandom() % 6];
        gStageEjectionTreasureIndex += 1;
        AdvanceStageEjectionTreasureIndex();
    }

    {
        struct StageEjectionTreasureParticle* base_r4;
        u32 setup_r2;
        s32 maxX_ip;
        s32 minY_r6;
        struct StageEjectionTreasureParticle* it_r1;
        s32 maxY_r5;

        base_r4 = gStageEjectionTreasureParticles;
        setup_r2 = 0xF00;
        maxX_ip = setup_r2;
        minY_r6 = -0x100;
        it_r1 = base_r4;
        maxY_r5 = 0xB40;

        do {
            u32 xraw_r3;
            u32 yraw_r2;
            register s32 work_r0 PIN("r0");

            xraw_r3 = it_r1->x;

            if (((s32) * (s16*)&it_r1->x > maxX_ip) || (yraw_r2 = it_r1->y, ((s32) * (s16*)&it_r1->y < minY_r6)) ||
                ((s32) * (s16*)&it_r1->y > maxY_r5)) {
                it_r1->active = 0U;
            } else if (it_r1->active != 0) {
                work_r0 = it_r1->yVelocity + 1;
                it_r1->yVelocity = work_r0;

                work_r0 = yraw_r2 + work_r0;
                it_r1->y = work_r0;

                work_r0 = it_r1->xVelocity;
                work_r0 = xraw_r3 + work_r0;
                it_r1->x = work_r0;
            }

            it_r1 += 1;
        } while ((s32)it_r1 <= (s32)((u8*)base_r4 + 0x40));
    }
}

void AdvanceStageEjectionTreasureIndex(void)
{
    switch (gStageEjectionTreasureIndex) {
        case 0:
            if ((u32)(u8)(gCollectedNEJewelPiece - 1) > 1U) {
                gStageEjectionTreasureIndex += 1;
                case 1:
                    if ((u32)(u8)(gCollectedSEJewelPiece - 1) > 1U) {
                        gStageEjectionTreasureIndex += 1;
                        case 2:
                            if ((u32)(u8)(gCollectedSWJewelPiece - 1) > 1U) {
                                gStageEjectionTreasureIndex += 1;
                                case 3:
                                    if ((u32)(u8)(gCollectedNWJewelPiece - 1) > 1U) {
                                        gStageEjectionTreasureIndex += 1;
                                        case 4:
                                            if ((u32)(u8)(gCollectedCD - 1) > 1U) {
                                                gStageEjectionTreasureIndex += 1;
                                            }
                                    }
                                case 5:
                                    break;
                            }
                    }
            }
            break;
    }
}

void ClearStageEjectionCollectionFlags(void)
{
    gCollectedNEJewelPiece = 0;
    gCollectedSEJewelPiece = 0;
    gCollectedSWJewelPiece = 0;
    gCollectedNWJewelPiece = 0;
    gCollectedCD = 0;
    gCollectedKeyzer = 0;
}

void UpdateStageEjectionArcMovement(void)
{
    s32 horizontalStep;
    u16 curveTimer;
    gStageEntryMainSpriteState[5] = (u16)(gStageEntryMainSpriteState[5] + 0x1E);
    if (((u32)gStageEntrySequenceTimer) > 0x1EU) {
        horizontalStep = gStageEntrySequenceTimer - 0x2D;
    } else {
        horizontalStep = 0xF - gStageEntrySequenceTimer;
    }
    do {
        gStageEntryMainSpriteState[4] = (u16)((horizontalStep * 2) + gStageEntryMainSpriteState[4]);
    } while (0);
    curveTimer = (gStageEntrySequenceTimer = gStageEntrySequenceTimer + 1);
    if (((u32)curveTimer) > 0x3CU) {
        gStageEntrySequenceTimer = 0;
    }
    gStageEntryMainSpriteState[6] = (gStageEntryMainSpriteState[6] + 0xFD) & 0xFF;
}

void DrawStageEjection(void)
{
    unsigned int new_var4;
    s32 drawn;
    s32 next;
    u16* rawOut;
    int new_var6;
    const u16* frame;
    u32 new_var9;
    u16* state;
    u8* new_var2;
    const s16* sineTable;
    register u16* stateCopy PIN("r7");
    long long pad;
    struct StageEjectionParticle* new_var7;
    u16 affine[4];
    int new_var;
    u16 affine3Value;
    u16* aff1Ptr;
    u16* aff2Ptr;
    OamData* new_var8;
    u16* aff3Ptr;
    u16* new_var3;
    drawn = 0;
    {
        OamData* oamBase;
        u32 byteOffset;
        next = gOamSlotsUsed;
        byteOffset = next << 3;
        oamBase = gOamBuffer;
        rawOut = (u16*)(byteOffset + ((u32)oamBase));
        frame = sStageEjectionWarioOam;
        new_var4 = -13;
        next += *frame;
        frame++;
        if (next > 0x80) {
            return;
        }
        state = gStageEntryMainSpriteState;
        sineTable = sSinCosTable;
        aff1Ptr = &affine[1];
        aff2Ptr = &affine[2];
        aff3Ptr = &affine[3];
        if (drawn < next) {
            s32 attr2Mask;
            u8* oamBytes;
            stateCopy = state;
            attr2Mask = new_var4;
            oamBytes = (u8*)oamBase;
            new_var3 = stateCopy;
            do {
                u32 attr;
                u32 work0;
                register u32 work1 PIN("r1");
                attr = *(frame++);
                *(rawOut++) = attr;
                work0 = ((s16)new_var3[5]) >> 4;
                work0 += attr;
                oamBytes[0] = work0;
                work1 = oamBytes[1];
                work0 = -4;
                work0 &= work1;
                work1 = 1;
                work0 |= work1;
                oamBytes[1] = work0;
                attr = *(frame++);
                *(rawOut++) = attr;
                work1 = ((s16)new_var3[4]) >> 4;
                work1 = attr + work1;
                work0 = 0x1FF;
                work1 &= work0;
                attr = ((u16*)oamBytes)[1];
                work0 = 0xFFFFFE00;
                work0 &= attr;
                work0 |= work1;
                ((u16*)oamBytes)[1] = work0;
                work1 = oamBytes[3];
                work0 = -15;
                work0 &= work1;
                oamBytes[3] = work0;
                work0 = *frame;
                *rawOut = work0;
                frame++;
                work1 = oamBytes[5];
                work0 = attr2Mask;
                work0 &= work1;
                oamBytes[5] = work0;
                rawOut += 2;
                oamBytes += 8;
                work1 = 1;
                drawn += work1;
            } while (drawn < next);
        }
    }
    new_var = state[6] + 0x40;
    affine[0] = FixedMul(sineTable[new_var], (s16)FixedInverse(state[7] >> 4));
    *aff1Ptr = FixedMul(sineTable[state[6]], (s16)FixedInverse(state[7] >> 4));
    new_var9 = (u32)sStageEjectionParticleAnimation;
    new_var = state[6] + 0x40;
    *aff2Ptr = FixedMul((s16)(-sineTable[state[6]]), (s16)FixedInverse(state[8] >> 4));
    new_var = state[6] + 0x40;
    affine3Value = FixedMul(sineTable[new_var], (s16)FixedInverse(state[8] >> 4));
    *aff3Ptr = affine3Value;
    gOamBuffer[0].all.affineParam = affine[0];
    (*(gOamBuffer + 1)).all.affineParam = *aff1Ptr;
    gOamBuffer[2].all.affineParam = *aff2Ptr;
    gOamBuffer[3].all.affineParam = affine3Value;
    {
        s32 slot;
        struct StageEjectionTreasureParticle* obj;
        slot = 0;
        obj = gStageEjectionTreasureParticles;
        do {
            new_var2 = (u8*)obj;
            if (new_var2[12] != 0) {
                {
                    GBA_TYPEOF(const u16* const*, &(sStageEjectionTreasureOamFrames)[0]) frameTable;
                    frameTable = sStageEjectionTreasureOamFrames;
                    frame = GP(frameTable[slot]);
                }
                next += *frame;
                frame++;
                if (next > 0x80) {
                    return;
                }
                new_var8 = gOamBuffer;
                if (drawn < next) {
                    u8* oamBytes;
                    struct StageEjectionTreasureParticle* curObj;
                    s32 mask;
                    register s32 remaining PIN("r9");
                    curObj = obj;
                    {
                        u32 byteOffset;
                        OamData* oamBase2;
                        byteOffset = drawn << 3;
                        oamBase2 = new_var8;
                        oamBytes = (u8*)(byteOffset + ((u32)oamBase2));
                    }
                    {
                        s32 maskValue;
                        maskValue = 0x1FF;
                        mask = maskValue;
                    }
                    remaining = next - drawn;
                    do {
                        u32 attr;
                        long work;
                        attr = *(frame++);
                        *(rawOut++) = attr;
                        {
                            u32 yWork;
                            yWork = ((s16)curObj->y) >> 4;
                            yWork += attr;
                            oamBytes[0] = yWork;
                        }
                        attr = *(frame++);
                        *(rawOut++) = attr;
                        {
                            u32 xWork;
                            u32 maskWork;
                            u32 oldAttr;
                            xWork = ((s16)curObj->x) >> 4;
                            xWork = (attr) - (-(xWork));
                            maskWork = mask;
                            xWork &= maskWork;
                            oldAttr = ((u16*)oamBytes)[1];
                            maskWork = 0xFFFFFE00;
                            maskWork &= oldAttr;
                            maskWork |= xWork;
                            ((u16*)oamBytes)[1] = maskWork;
                        };
                        *rawOut = *(frame++);
                        oamBytes[5] &= new_var4;
                        rawOut += 2;
                        oamBytes += 8;
                        remaining -= 1;
                    } while (remaining != 0);
                    drawn = next;
                }
            }
            obj++;
            slot += 1;
        } while (slot <= 4);
    }
    {
        register s32 slot PIN("r12");
        struct StageEjectionParticle* obj;
        s32 offset;
        slot = 0;
        obj = gStageEjectionParticles;
        offset = 0;
        do {
            if (obj->active != 0) {
                u32 timer;
                register u32 frameValue PIN("r1");
                register u32 animationTable PIN("r2");
                u32 zero;
                timer = obj->animationTimer + 1;
                zero = 0;
                obj->animationTimer = timer;
                animationTable = new_var9;
                frameValue = obj->frame;
                frameValue <<= 3;
                frameValue += animationTable;
                frameValue = ((const struct AnimationFrame*)frameValue)->time;
                timer = (u16)timer;
                if (frameValue < timer) {
                    obj->animationTimer = zero;
                    timer = obj->frame;
                    timer += 1;
                    obj->frame = timer;
                    timer = obj->frame;
                    timer <<= 3;
                    timer += animationTable;
                    timer = ((const struct AnimationFrame*)timer)->time;
                    if (timer == 0) {
                        obj->frame = timer;
                    }
                }
                timer = obj->frame;
                timer <<= 3;
                zero = new_var9;
                timer += zero;
                frame = GP(((const struct AnimationFrame*)timer)->oam);
                next += *frame;
                frame++;
                if (next > 0x80) {
                    return;
                }
                if (drawn < next) {
                    u8* oamBytes;
                    struct StageEjectionParticle* curObj;
                    s32 mask;
                    register s32 remaining PIN("r9");
                    {
                        register u32 objectBase PIN("r0");
                        u32 objectOffset;
                        objectBase = (u32)gStageEjectionParticles;
                        objectOffset = offset;
                        curObj = (struct StageEjectionParticle*)(objectOffset + objectBase);
                    }
                    {
                        s32 drawnValue;
                        u32 byteOffset;
                        OamData* oamBase;
                        drawnValue = drawn;
                        byteOffset = drawnValue << 3;
                        oamBase = new_var8;
                        oamBytes = (u8*)(byteOffset + ((u32)oamBase));
                    }
                    mask = 0x1FF;
                    new_var6 = drawn;
                    remaining = next - new_var6;
                    do {
                        u32 attr;
                        s32 work;
                        attr = *(frame++);
                        *(rawOut++) = attr;
                        oamBytes[0] = attr + (((s16)curObj->y) >> 4);
                        attr = *(frame++);
                        *(rawOut++) = attr;
                        {
                            u32 xWork;
                            u32 maskWork;
                            u32 oldAttr;
                            xWork = ((s16)curObj->x) >> 4;
                            xWork = (attr) - (-(xWork));
                            maskWork = mask;
                            xWork &= maskWork;
                            oldAttr = ((u16*)oamBytes)[1];
                            maskWork = 0xFFFFFE00;
                            maskWork &= oldAttr;
                            maskWork |= xWork;
                            ((u16*)oamBytes)[1] = maskWork;
                        }
                        timer = *frame;
                        *rawOut = timer;
                        frame++;
                        oamBytes[5] &= new_var4;
                        rawOut += 2;
                        oamBytes += 8;
                        remaining -= 1;
                    } while (remaining != 0);
                    drawn = next;
                }
            }
            obj++;
            offset += 0x10;
            slot += 1;
        } while (slot <= 0x27);
    }
    gOamSlotsUsed = next;
}

void UpdateStageEjectionEffects(void)
{
    u16 angle;

    UpdateStageEjectionParticles();
    UpdateStageEjectionTreasureParticles();

    angle = (gStageEntryHorizontalScroll[1] + 0x96) & 0xFFF;
    gStageEntryHorizontalScroll[1] = angle;
    *(volatile u16*)0x04000012 = angle >> 4;
}

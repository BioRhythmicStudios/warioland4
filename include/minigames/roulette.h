#ifndef GUARD_MINIGAMES_ROULETTE_H
#define GUARD_MINIGAMES_ROULETTE_H

#include "fixed_point.h"
#include "oam.h"
#include "types.h"

struct AGB_ALIGN RouletteAnimationState {
    u16 timer;
    u16 frame;
};

struct AGB_ALIGN RouletteItem {
    s16 x;
    s16 y;
    s16 scale;
    u8 value;
    u8 active;
    s16 angle;
    s16 reserved;
};

struct AGB_ALIGN RouletteSmallState {
    u16 x;
    u16 y;
    u8 frame;
    u8 pad5[3];
};

struct AGB_ALIGN RouletteMainState {
    u8 pad0[4];
    u16 scale;
    u8 pad6[2];
    u16 x;
    u16 y;
    u8 active;
};

extern GBA_PTR(struct RouletteItem*) gRouletteItemDrawOrder[3];

struct AGB_ALIGN RouletteRenderStack {
    s16 affineA;
    s16 affineB;
    s16 affineC;
    s16 affineD;
    GBA_PTR(s16*) affineBPtr;
    GBA_PTR(s16*) affineCPtr;
    GBA_PTR(s16*) affineDPtr;
    int nextItem;
    int itemOffset;
    GBA_PTR(GBA_TYPEOF(struct RouletteItem**, &(gRouletteItemDrawOrder)[0])) itemRef;
};

extern u16 gUnk_300188E;
extern u8 gMinigameState;
extern u8 gMinigameSubstate;
extern u8 gMinigameSequenceState;
extern u16 gMinigameSequenceTimer;
extern u8 gMinigameNewHighScore;
extern u16 gMinigameScore;
extern struct RouletteAnimationState gMinigameUiAnimation;
extern u8 gMinigameMedalHighlight;
extern struct RouletteMainState gMinigameCameraState;
extern u16 gMinigameHighScores[];
extern u16 gMedalCount;

extern s8 gRouletteBlendEvb;
extern u8 gRouletteBlendEva;
extern u8 gRouletteStopRequested;
extern struct RouletteSmallState gRouletteTopResult;
extern struct RouletteSmallState gRouletteMiddleResult;
extern struct RouletteSmallState gRouletteBottomResult;
extern u8 gRouletteWinningValues[3];
extern s16 gRouletteTickPeriod;
extern u16 gRouletteMoveStartX;
extern u16 gRouletteMoveStartY;
extern u16 gRouletteMoveStartTile;
extern u16 gRouletteAngularAcceleration;
extern struct RouletteItem gRouletteItems[3];
extern u16 gRouletteAngularSpeed;
extern u8 gRouletteMoveProgress;
extern u8 gRouletteValueSequenceIndex;
extern u8 gRouletteSelectedItemIndex;
extern u16 gRouletteValueCount;
extern struct RouletteAnimationState gRouletteSideAnim;
extern struct RouletteAnimationState gRoulettePromptAnim;
extern volatile u16 gRouletteFrameCounter;
extern u8 gRouletteValuePool[];
extern s16 gRouletteMoveStepTile;
extern s16 gRouletteMoveStepX;
extern s16 gRouletteMoveStepY;
extern u8 gRouletteOamOrder[];
extern GBA_PTR(const u16*) gRouletteItemOamTable[15];

extern u8 sRouletteBgPalette[];
extern u8 sRouletteObjPalette[];
extern u8 sRouletteBgTiles[];
extern u8 sRouletteObjTiles[];
extern u8 sRouletteTopResultTiles[];
extern u8 sRouletteMiddleResultTiles[];
extern u8 sRouletteBottomResultTiles[];
extern u8 sRouletteExtraBgTiles[];
extern u8 sRouletteExtraObjTiles[];
extern const s16 sRouletteInitialTileSin;
extern GBA_PTR(const u16*) sRouletteItemOamFrames[];
extern const u8 sRouletteAwardMedalTiles[];
extern const u8 sRouletteMedalTiles[];
extern const u8 sRouletteScoreTiles[];
extern const u16 sRouletteAffineTableB[];
extern const u16 sRouletteAffineTableA[];
extern const u16 sRouletteAffineTableC[];
extern u8 sRouletteResultFlashTiles[];

extern int __modsi3(int, int);
extern unsigned int __umodsi3(unsigned int, unsigned int);
extern int __divsi3(int, int);

void RefreshRouletteResultTiles(void);
void UpdateRouletteWheel(void);
int IsRouletteConfirmPressed(void);
void SortRouletteItemsByScale(void);
void UpdateRouletteResultTiles(void);
int MoveRouletteResultIntoSlot(void);
void ShuffleRouletteValuePool(void);
int UpdateRouletteResults(void);
int UpdateRouletteIntro(void);
void DrawRouletteScoreText(void);
void ShuffleRouletteOamTable(void);
void LoadRouletteResultTiles(void);
int GetRouletteFrameIndex(u8 value);
void BobRouletteResultLabels(void);
void ResetRouletteRound(void);
void AnimateRouletteResultTiles(void);

#define CALC_SOURCE(stateObject, graphicsBase, destination) do { \
    register s32 frame PIN("r1"); \
    register s32 quotient PIN("r2"); \
    register s32 frameOffset PIN("r0"); \
    frameOffset = GetRouletteFrameIndex((stateObject).frame); \
    frame = frameOffset; \
    if (frame < 0) \
        frameOffset = frame + 3; \
    frameOffset >>= 2; \
    ASM_BARRIER_VOLATILE(: "+r"(frameOffset)); \
    quotient = frameOffset; \
    frameOffset = quotient << 2; \
    frameOffset = frame - frameOffset; \
    quotient <<= 12; \
    frameOffset <<= 8; \
    { \
        register u8 *base PIN("r1"); \
        base = (graphicsBase); \
        frameOffset += (s32)base; \
    } \
    src = (u8 *)(quotient + frameOffset); \
    dst = (u8 *)(destination); \
} while (0)

#define CALC_PARTS_FROM_VALUE(callExpression, graphicsBase) do { \
    register s32 frame PIN("r3"); \
    register s32 frameOffset PIN("r0"); \
    frameOffset = (callExpression); \
    frame = frameOffset; \
    if (frame < 0) \
        frameOffset = frame + 3; \
    frameOffset >>= 2; \
    ASM_BARRIER_VOLATILE(: "+r"(frameOffset)); \
    quotient = frameOffset; \
    frameOffset = quotient << 2; \
    frameOffset = frame - frameOffset; \
    quotient <<= 12; \
    src = (u8 *)(frameOffset << 8); \
    { \
        register u8 *base PIN("r0"); \
        base = (graphicsBase); \
        src += (s32)base; \
    } \
} while (0)

// Both adds stay in one asm statement so agbcc keeps the original order.
#ifdef PORTABLE
#define ROULETTE_ADVANCE_DMA_POINTERS(dst, src, step) ((dst) += (step), (src) += (step))
#else
#define ROULETTE_ADVANCE_DMA_POINTERS(dst, src, step) \
    asm volatile("add %0, %0, %2\nadd %1, %1, %2" : "+r"(dst), "+r"(src) : "r"(step))
#endif

#define RUN_FOUR_DMAS(destination) do { \
    register volatile u32 *dma PIN("r4"); \
    register u32 control PIN("r12"); \
    register u32 controlLoad PIN("r0"); \
    register u8 *dst PIN("r6"); \
    register u32 busy; \
    register u32 step PIN("r2"); \
    register int count PIN("r5"); \
    register u32 value PIN("r0"); \
    dma = (volatile u32 *)0x040000D4; \
    controlLoad = 0x80000080; \
    control = controlLoad; \
    dst = (u8 *)(destination); \
    src = (u8 *)((u32)quotient + (u32)src); \
    busy = 0x80000000; \
    step = 0x400; \
    count = 3; \
    do { \
        dma[0] = (u32)src; \
        dma[1] = (u32)dst; \
        dma[2] = control; \
        value = dma[2]; \
        value = dma[2]; \
        value &= busy; \
        if (value != 0) { \
            register u32 pollMask PIN("r3"); \
            pollMask = 0x80000000; \
            do { \
                value = dma[2]; \
                value &= pollMask; \
            } while (value != 0); \
        } \
        ROULETTE_ADVANCE_DMA_POINTERS(dst, src, step); \
        count--; \
    } while (count >= 0); \
} while (0)

extern const struct AnimationFrame sRouletteMainState1Oam[];
extern const struct AnimationFrame sRouletteMainState2Oam[];
extern const struct AnimationFrame sRoulettePromptOam[];
extern const struct AnimationFrame sRouletteCenterResultOam[];
extern const struct AnimationFrame sRouletteSideResultOam[];

void InitRouletteGame(void);
int UpdateRouletteGame(void);
void ApplyRouletteBlend(void);
void DrawRoulette(void);

#endif  // GUARD_MINIGAMES_ROULETTE_H

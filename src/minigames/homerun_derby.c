#include "minigames/homerun_derby.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "input.h"
#include "fixed_point.h"
#include "minigame.h"
#include "oam.h"
#include "stage_select.h"

struct AGB_ALIGN Dma3Regs {
    vu32 src;
    vu32 dst;
    vu32 cnt;
};

#define DMA3 ((volatile struct Dma3Regs *)0x040000D4)

#define HR_DMA(s, d, c) \
    DMA3->src = (u32)(s); \
    DMA3->dst = (d); \
    DMA3->cnt = (c); \
    dmaStatus = DMA3->cnt; \
    dmaStatus = DMA3->cnt; \
    if ((s32)dmaStatus < 0) { \
        do { \
            dmaStatus = DMA3->cnt & 0x80000000; \
        } while (dmaStatus != 0); \
    }


typedef u8 OamByte;

struct AGB_ALIGN HomerunBallState {
    u16 animationTimer;
    u16 animationFrame;
    u16 xPosition;
    u16 yPosition;
    u16 screenX;
    s16 screenY;
    s8 xVelocity;
    s8 yVelocity;
    s8 gravity;
    s8 reservedF;
    u16 rotation;
    u16 scale;
};


struct AGB_ALIGN HomerunBallRenderState {
    u16 animationTimer;
    u16 animationFrame;
    s16 xPosition;
    s16 yPosition;
    s16 screenX;
    u8 screenY;
    u8 reservedB;
    u8 xVelocity;
    u8 yVelocity;
    u8 gravity;
    u8 reservedF;
    u16 rotation;
    s16 scale;
};

struct AGB_ALIGN HomerunBatState {
    u16 animationTimer;
    u16 animationFrame;
    u16 angle;
    u8 swingFrame;
    u8 contactFrame;
};

struct AGB_ALIGN HomerunCameraState {
    u16 reserved0;
    u16 reserved2;
    u16 scale;
    u16 rotation;
    s16 xPosition;
    u8 yPosition;
    u8 reservedB;
    u8 oamMode;
};

struct AGB_ALIGN HomerunSimpleAnimation {
    u16 animationTimer;
    u16 animationFrame;
    s16 xPosition;
    u8 yPosition;
    u8 reserved7;
    u8 animationIndex;
    u8 nextAnimationIndex;
};

struct AGB_ALIGN HomerunPitcherState {
    u16 animationTimer;
    u16 animationFrame;
    u8 animationIndex;
    u8 pitchCounter;
};

struct AGB_ALIGN HomerunAnimationCursor {
    u16 animationTimer;
    u16 animationFrame;
};

struct AGB_ALIGN HomerunBonusState {
    u16 animationTimer;
    u16 animationFrame;
    u16 xPosition;
    s16 yPosition;
    u8 animationIndex;
    u8 nextAnimationIndex;
    u8 direction;
};

extern u8 gMinigameState;
extern u8 gMinigameSubstate;
extern u8 gMinigameSequenceState;
extern u16 gMinigameSequenceTimer;
extern u8 gMinigameNewHighScore;
extern u16 gMinigameScore;
extern struct HomerunAnimationCursor gMinigameUiAnimation;
extern u8 gMinigameMedalHighlight;
extern u8 gMinigameCamera[];
extern struct HomerunCameraState gMinigameCameraState;
extern u16 gHomerunBgX;
extern u16 gHomerunBgY;
extern u16 gHomerunBall[];
extern struct HomerunBallRenderState gHomerunBallRenderState;
extern u8 gHomerunPitchPath;
extern u8 gHomerunPitchFrame;
extern u16 gHomerunBat[];
extern struct HomerunAnimationCursor gHomerunBatAnimation;
extern u16 gHomerunBatX;
extern u16 gHomerunBatY;
extern u8 gHomerunBatSwingActive;
extern u8 gHomerunPitcher[];
extern struct HomerunPitcherState gHomerunPitcherAnimation;
extern u16 gHomerunResultAnimation[];
extern struct HomerunSimpleAnimation gHomerunResultRender;
extern u16 gHomerunBonusAnimation[];
extern struct HomerunSimpleAnimation gHomerunBonusRender;
extern u8 gHomerunBonusCycle;
extern u8 gHomerunPitchResult;
extern u8 gHomerunMissCount;
extern u8 gHomerunPitchLevel;
extern u16 gMedalCount;
extern u16 gMinigameHighScores;

extern GBA_PTR(struct AnimationFrame *) sHomerunPitcherAnimations[];
extern GBA_PTR(struct AnimationFrame *) sHomerunResultAnimations[];
extern u16 sHomerunPitchSounds[];
extern u8 sHomerunPitchPathChoices[];
extern u8 sHomerunPitchAnimationsNormal[];
extern u8 sHomerunPitchAnimationsHard[];
extern const u8 sHomerunPitchPathLengths[];
extern const u16 sHomerunPitchPathX[];
extern const u16 sHomerunPitchPathY[];
extern u8 sHomerunHitSpeed[];
extern s8 sHomerunHitLift[];
extern const u8 sHomerunBatAngles[];
extern GBA_PTR(struct AnimationFrame *) sHomerunBonusAnimations[];
extern s32 sHomerunBonusYLimits[];
extern struct AnimationFrame sHomerunBallIdleAnimation[];
extern struct AnimationFrame sHomerunBallHitAnimation[];
extern struct AnimationFrame sHomerunBallHitDurations[];
extern struct AnimationFrame sHomerunBatAnimation[];
extern struct AnimationFrame sHomerunBallPitchAnimation[];
extern GBA_PTR(u16 *) sHomerunCameraOamMode2;
extern GBA_PTR(u16 *) sHomerunCameraOamMode1;
extern GBA_PTR(u16 *) sHomerunStaticOam;
extern struct AnimationFrame sHomerunBallPitchDurations[];
extern u8 sHomerunBgTilemapA[];
extern u8 sHomerunBgTilemapB[];
extern u8 sHomerunBgPalette[];
extern u8 sHomerunBgTiles[];
extern const u16 sHomerunMedalTiles[];
extern const u16 sHomerunHighlightedMedalTiles[];
extern const u16 sHomerunDigitTiles[];
extern u8 sHomerunObjPalette[];
extern u8 sHomerunObjTiles[];
extern u8 sHomerunBgTilemapD[];
extern u8 sHomerunBgTilemapC[];
extern u8 sHomerunHardModeObjTiles[];

/*

ALL asm inlined code islands are for agbcc frontier, i couldn't find a replacement in pure C

*/

s32 UpdateHomerunDerby(void)
{
    u8 *stateAddress;
    u8 *savedStateAddress;
    register u8 *cameraData PIN("r4");
    u8 *cameraDataForExit;
    u8 *swingActiveFlag;
    u8 *pitchAnimationTable;
    u8 *pitcherData;
    u8 *pitcherState;
    register u8 *pitcherDataForSound PIN("r5");
    u8 *ballState;
    u8 currentState;
    s32 difficulty;
    s32 pitchAnimationIndex;
    s32 updatedMissCount;
    u8 animationIndex;
    register s32 nextValue PIN("r0");

    stateAddress = &gMinigameState;
    currentState = *stateAddress;
    savedStateAddress = stateAddress;

    switch (currentState) {
    case 0:
        if (MinigameWaitForFrames(10) == 0) {
            break;
        }
        *(vu16 *)0x04000000 = 0x1800;
        gMinigameState++;
        break;

    case 1:
        cameraData = gMinigameCamera;
        nextValue = *(u16 *)(cameraData + 4) + 0xC0;
        *(u16 *)(cameraData + 4) = nextValue;
        *(u16 *)(cameraData + 6) = (*(u16 *)(cameraData + 6) + 0xFE7) & 0xFFF;
        if ((u16)nextValue > 0x2000) {
            m4aSongNumStart(0x236);
            *(u16 *)(cameraData + 4) = 0x2000;
            nextValue = 0xC00;
            *(u16 *)(cameraData + 6) = nextValue;
            gMinigameState++;
        }
        break;

    case 2:
        if (MinigameWaitForFrames(0x28) != 0) {
            gMinigameState++;
            gMinigameCamera[12] = 0;
            *(vu16 *)0x04000000 = 0x1C00;
        }
        break;

    case 3:
        if (MinigameWaitForFrames(20) != 0) {
            if (gHomerunPitchResult == 1 || gHomerunPitchResult == 4) {
                m4aSongNumStart(0x2C5);
                gMinigameState++;
            } else {
                gMinigameState = 5;
                gHomerunPitchResult = 4;
            }
        }
        break;

    case 4:
        UpdateHomerunBatInput();
        if (MinigameWaitForFrames(100) != 0) {
            gMinigameState++;
        }
        break;

    case 5:
        UpdateHomerunBatInput();
        if (MinigameWaitForFrames(30) != 0) {
            gMinigameState++;
            pitcherData = gHomerunPitcher;
            pitcherData[5]++;
            difficulty = gDifficulty;
            pitcherState = pitcherData;
            if (difficulty == 2) {
                pitchAnimationTable = sHomerunPitchAnimationsHard;
                pitchAnimationIndex = (gHomerunPitchPath * 5) - 1;
                pitchAnimationIndex += pitcherState[5];
            } else {
                pitchAnimationTable = sHomerunPitchAnimationsNormal;
                pitchAnimationIndex = (gHomerunPitchPath * 5) - 1;
                pitchAnimationIndex += pitcherState[5];
            }
            pitchAnimationIndex += (s32)pitchAnimationTable;
            animationIndex = *(u8 *)pitchAnimationIndex;
            pitcherState[4] = animationIndex;
            pitcherDataForSound = pitcherState;
            if (pitcherDataForSound[4] == 3) {
                m4aSongNumStart(0x226);
            }
            if (pitcherDataForSound[4] == 2) {
                m4aSongNumStart(0x227);
            }
            *(u16 *)(pitcherState + 2) = 0;
            *(u16 *)(pitcherState + 0) = 0;
        }
        break;

    case 6:
        UpdateHomerunBatInput();
        break;

    case 7:
        UpdateHomerunBatInput();
        if (UpdateHomerunPitch() != 0) {
            if (gHomerunPitchResult == 2) {
                m4aSongNumStart(0x2C9);
                gHomerunMissCount++;
                gMinigameState = 12;
            } else {
                gMinigameState++;
                ballState = gHomerunBall;
                *(u16 *)(ballState + 4) = *(u16 *)(ballState + 8) << 4;
                *(u16 *)(ballState + 6) = *(u16 *)(ballState + 10) << 4;
                gHomerunBatSwingActive = 1;
            }
        }
        break;

    case 8:
        swingActiveFlag = &gHomerunBatSwingActive;
        if (*swingActiveFlag != 0 && UpdateHomerunBatSwing() != 0) {
            *swingActiveFlag = 0;
        }
        if (UpdateHomerunHitBall() != 0) {
            switch (gHomerunPitchResult) {
            case 1:
                m4aSongNumStart(0x2C6);
                gMinigameState = 11;
                break;
            case 0:
                updatedMissCount = gHomerunMissCount + 1;
                gHomerunMissCount = updatedMissCount;
                if ((u8)updatedMissCount > 2) {
                    gHomerunMissCount = 2;
                }
                gMinigameState = 12;
                m4aSongNumStart(0x2CA);
                break;
            case 5:
                m4aSongNumStart(0x235);
                gMinigameState = 9;
                break;
            }
        }
        break;

    case 9:
        swingActiveFlag = &gHomerunBatSwingActive;
        if (*swingActiveFlag != 0 && UpdateHomerunBatSwing() != 0) {
            *swingActiveFlag = 0;
        }
        if (UpdateHomerunMissedBall() != 0) {
            if (gHomerunPitchResult == 1) {
                m4aSongNumStart(0x2C6);
            }
            gMinigameState++;
        }
        break;

    case 10:
        swingActiveFlag = &gHomerunBatSwingActive;
        if (*swingActiveFlag != 0 && UpdateHomerunBatSwing() != 0) {
            *swingActiveFlag = 0;
        }
        if (MinigameWaitForFrames(80) != 0) {
            *(vu16 *)0x04000000 = 0x1C00;
            if (gHomerunPitchResult == 1) {
                gMinigameState = 11;
            } else {
                updatedMissCount = gHomerunMissCount + 1;
                gHomerunMissCount = updatedMissCount;
                if ((u8)updatedMissCount > 2) {
                    gHomerunMissCount = 2;
                }
                m4aSongNumStart(0x2CA);
                gMinigameState = 12;
            }
        }
        break;

    case 11:
        if (UpdateHomerunScoreSequence() != 0) {
            gHomerunMissCount = 0;
            ResetHomerunDerbyPitch();
            if ((gMinigameScore % 9) != 0) {
                gMinigameState = 3;
            } else {
                StartHomerunBonusAnimation();
                gMinigameState = 17;
            }
        }
        break;

    case 12:
        swingActiveFlag = &gHomerunBatSwingActive;
        if (*swingActiveFlag != 0 && UpdateHomerunBatSwing() != 0) {
            *swingActiveFlag = 0;
        }
        if (MinigameWaitForFrames(100) != 0) {
            if (gHomerunMissCount == 3) {
                if ((MinigameRandom() & 1) == 0) {
                    m4aSongNumStart(0x2C7);
                } else {
                    m4aSongNumStart(0x2C8);
                }
                gHomerunPitchResult = 3;
                gMinigameState++;
            } else {
                ResetHomerunDerbyPitch();
                gMinigameState = 3;
            }
        }
        break;

    case 13:
        nextValue = MinigameWaitForFrames(120);
        if (nextValue == 0) {
            break;
        }
        gMinigameState++;
        *(vu16 *)0x04000000 = 0x1800;
        gMinigameCamera[12] = 2;
        nextValue = 0x200;
        *(u16 *)(gMinigameCamera + 4) = nextValue;
        break;

    case 14:
        cameraDataForExit = gMinigameCamera;
        nextValue = *(u16 *)(cameraDataForExit + 4) + 0xC0;
        *(u16 *)(cameraDataForExit + 4) = nextValue;
        *(u16 *)(cameraDataForExit + 6) = (*(u16 *)(cameraDataForExit + 6) + 25) & 0xFFF;
        if ((u16)nextValue > 0x2000) {
            *(u16 *)(cameraDataForExit + 4) = 0x2000;
            (*savedStateAddress)++;
            m4aSongNumStart(0x236);
        }
        break;

    case 15:
        if (MinigameWaitForFrames(80) != 0) {
            gMinigameState = 0;
            return 1;
        }
        break;

    case 16:
        if (MinigameWaitForFrames(30) != 0) {
            gMinigameState = 0;
            return 1;
        }
        break;

    case 17:
        nextValue = UpdateHomerunBonusAnimation();
        if (nextValue != 0) {
            gMinigameState = 3;
        }
        break;
    }

    return 0;
}

void ResetHomerunDerbyRound(void)
{
    u16 zeroHalfword;
    register u8 zeroByte PIN("r3");
    u16 *ballState;
    u16 *batState;
    u16 *pitcherState;
    u16 *resultState;

    ballState = gHomerunBall;
    zeroByte = 0;
    zeroHalfword = 0;
    ballState[4] = 60;
    ballState[5] = 88;
    ballState[0] = zeroHalfword;
    ballState[1] = zeroHalfword;
    ballState[8] = zeroHalfword;
    ballState[9] = 64;
    gHomerunPitchFrame = zeroByte;

    batState = gHomerunBat;
    batState[0] = zeroHalfword;
    batState[1] = zeroHalfword;
    ((u8 *)batState)[6] = zeroByte;
    batState[2] = sHomerunBatAngles[0];

    gHomerunBatX = 215;
    gHomerunBatY = 121;
    gHomerunBatSwingActive = zeroByte;

    pitcherState = (u16 *)gHomerunPitcher;
    pitcherState[0] = zeroHalfword;
    pitcherState[1] = zeroHalfword;
    ((u8 *)pitcherState)[4] = zeroByte;
    ((u8 *)pitcherState)[5] = zeroByte;

    resultState = gHomerunResultAnimation;
    resultState[0] = zeroHalfword;
    resultState[1] = zeroHalfword;
    resultState[2] = 120;
    resultState[3] = 80;
    SelectHomerunPitchPath();
}

void InitHomerunDerby(void)
{
    u32 dmaStatus;
    struct HomerunCameraState* camera;
    register u32 dmaMask PIN("r2");

    DMA3->src = (u32)sHomerunBgPalette;
    DMA3->dst = 0x05000000;
    DMA3->cnt = 0x80000100;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    DMA3->src = (u32)sHomerunObjPalette;
    DMA3->dst = 0x05000200;
    DMA3->cnt = 0x80000100;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    DMA3->src = (u32)sHomerunBgTiles;
    DMA3->dst = 0x06000000;
    DMA3->cnt = 0x80002600;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    DMA3->src = (u32)sHomerunObjTiles;
    DMA3->dst = 0x06010000;
    DMA3->cnt = 0x80004000;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    if (gDifficulty == 2) {
        DMA3->src = (u32)sHomerunHardModeObjTiles;
    DMA3->dst = 0x06010000;
    DMA3->cnt = 0x80000800;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    }
    DMA3->src = (u32)sHomerunBgTilemapA;
    DMA3->dst = 0x0600C000;
    DMA3->cnt = 0x80000400;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    DMA3->src = (u32)sHomerunBgTilemapC;
    DMA3->dst = 0x0600C800;
    DMA3->cnt = 0x80000400;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    DMA3->src = (u32)sHomerunBgTilemapB;
    DMA3->dst = 0x0600D000;
    DMA3->cnt = 0x80001000;
    dmaStatus = DMA3->cnt;
    dmaStatus = DMA3->cnt;
    dmaMask = 0x80000000;
    if ((s32)dmaStatus < 0) {
        do { dmaStatus = DMA3->cnt & dmaMask; } while (dmaStatus != 0);
    }
    DMA3->src = (u32)sHomerunBgTilemapD;
    DMA3->dst = 0x0600F000;
    DMA3->cnt = 0x80000400;
    dmaStatus = DMA3->cnt;

    *(vu16 *)0x0400000E = 0x1803;
    *(vu16 *)0x0400000A = 0x1901;
    *(vu16 *)0x04000008 = 0xDA00;
    *(vu16 *)0x0400000C = 0x1E02;
    *(vu16 *)0x0400001A = 0;
    *(vu16 *)0x04000018 = 0;
    *(vu16 *)0x0400001E = 0;
    *(vu16 *)0x0400001C = 0;
    gHomerunBgX = 0;
    gHomerunBgY = 0;
    *(vu16 *)0x04000012 = 0;
    *(vu16 *)0x04000010 = 0;

    if (gDifficulty == 2) {
        gHomerunPitchLevel = 1;
    } else {
        gHomerunPitchLevel = 0;
    }
    gHomerunPitchResult = 4;
    ResetHomerunDerbyRound();

    gHomerunBonusAnimation[0] = 0;
    gHomerunBonusAnimation[1] = 0;
    ((u8 *)gHomerunBonusAnimation)[8] = 0;
    ((u8 *)gHomerunBonusAnimation)[9] = 0;
    ((u8 *)gHomerunBonusAnimation)[10] = 0;
    gHomerunBonusCycle = 0;

    camera = (struct HomerunCameraState*)gMinigameCamera;
    camera->scale = 0x200;
    camera->xPosition = 0x68;
    *(u16*)&camera->yPosition = 0x40;
    camera->rotation = 0;
    camera->oamMode = 1;

    gMinigameState = 0;
    gMinigameSubstate = 0;
    gMinigameSequenceState = 0;
    gMinigameSequenceTimer = 0;
    gMinigameScore = 0;
    gMinigameNewHighScore = 0;
    DrawHomerunDerbyScore();
    gHomerunMissCount = 0;
    InitMinigameScoreDisplay();
    *(vu16 *)0x04000054 = 0;
    *(vu16 *)0x04000050 = 0x1C42;
    *(vu16 *)0x04000052 = 0x060A;
    *(vu16 *)0x04000000 = 0x0800;
}

void ResetHomerunDerbyPitch(void)
{
    u32 scoreRemainder;

    *(vu16 *)0x04000200 ^= 1;
    *(vu16 *)0x04000000 = 0x1C00;

    scoreRemainder = gMinigameScore % 5;
    if ((scoreRemainder << 16) == 0 && gHomerunPitchResult == 1) {
        gHomerunPitchLevel++;
    }
    if (gHomerunPitchLevel > 9) {
        gHomerunPitchLevel = 9;
    }

    ResetHomerunDerbyRound();
    *(vu16 *)0x04000200 |= 1;
}

void SelectHomerunPitchPath(void)
{
    unsigned char *dst;
    unsigned char *table;
    s32 randomSelection;
    unsigned char row;
    s32 difficultyOffset;

    dst = &gHomerunPitchPath;
    table = sHomerunPitchPathChoices;
    randomSelection = MinigameRandom();
    difficultyOffset = randomSelection % 20;
    row = gHomerunPitchLevel;
    difficultyOffset += row * 20;
    *dst = table[difficultyOffset];
}

s32 UpdateHomerunPitch(void)
{
    u16 *ballState;
    u16 *ballStateReload;
    const u16 *pathXCoordinates;
    const u16 *pathYCoordinates;
    u8 *pitchFramePointer;
    u8 *frameIndexPointer;
    s32 coordinateOffset;
    s32 xPosition;
    s32 yPosition;

    ballState = gHomerunBall;
    pathXCoordinates = sHomerunPitchPathX;
    pitchFramePointer = &gHomerunPitchFrame;
    frameIndexPointer = &gHomerunPitchPath;
    coordinateOffset = (*frameIndexPointer * 63) + *pitchFramePointer;
    xPosition = pathXCoordinates[coordinateOffset] + 60;
    ballState[4] = xPosition;

    pathYCoordinates = sHomerunPitchPathY;
    coordinateOffset = (*frameIndexPointer * 63) + *pitchFramePointer;
    yPosition = pathYCoordinates[coordinateOffset] + 88;
    ballState[5] = yPosition;

    if ((s32)(xPosition << 16) >> 16 > 140) {
        if (CheckHomerunBatHit() != 0) {
            return 1;
        }
    }

    gHomerunPitchFrame++;
    if ((u8)gHomerunPitchFrame == sHomerunPitchPathLengths[gHomerunPitchPath]) {
        ballStateReload = gHomerunBall;
        ballStateReload[2] = ballStateReload[4] << 4;
        ballStateReload[3] = ballStateReload[5] << 4;
        gHomerunPitchResult = 2;
        return 1;
    }

    return 0;
}

void UpdateHomerunBatInput(void)
{
    if (gButtonsPressed & 1) {
        if (gHomerunBatSwingActive == 0) {
            gHomerunBatSwingActive = 1;
            m4aSongNumStart(0x232);
        }
    }

    if (gHomerunBatSwingActive != 0) {
        if (UpdateHomerunBatSwing() != 0) {
            gHomerunBatSwingActive = 0;
        }
    }
}

s32 CheckHomerunBatHit(void)
{
    const s16 *pathXCoordinates;
    u8 *difficultyPointer;
    s32 nextPathX;
    s32 nextPathY;
    s32 previousPathX;
    s32 previousPathY;
    s32 currentPathOffset;
    s32 nextPathOffset;
    s32 previousPathOffset;
    s32 batOffsetX;
    u8 **liftTablePointer;
    s32 batOffsetY;
    s32 batCrossProduct;
    s32 secondaryValue;
    s32 workingValue;
    s32 trigIndex;
    s16 ballScreenY;
    u8 previousPitchResult;
    u16 batAngle;
    struct HomerunBallState *ballState;
    pathXCoordinates = (const s16 *)sHomerunPitchPathX;
    previousPitchResult = gHomerunPitchFrame;
    nextPathOffset = (previousPitchResult + 1) * 2;
    difficultyPointer = &gHomerunPitchPath;
    currentPathOffset = (*difficultyPointer) * 0x3F;
    currentPathOffset *= 2;
    nextPathOffset += currentPathOffset;
    nextPathX = (*((s16 *)(((u8 *)pathXCoordinates) + nextPathOffset))) + 0x3C;
    nextPathY = (*((s16 *)(((u8 *)sHomerunPitchPathY) + nextPathOffset))) + 0x58;
    previousPathOffset = (previousPitchResult - 1) * 2;
    previousPathOffset += currentPathOffset;
    previousPathX = (*((s16 *)(((u8 *)pathXCoordinates) + previousPathOffset))) + 0x3C;
    previousPathY = (*((s16 *)(previousPathOffset + ((u8 *)sHomerunPitchPathY)))) + 0x58;
    batOffsetX = ((s16)gHomerunBatX) - 0xD7;
    batOffsetY = ((s16)gHomerunBatY) - 0x60;
    batCrossProduct = (((s16)gHomerunBatY) * 0xD7) - (((s16)gHomerunBatX) * 0x60);
    ballState = (struct HomerunBallState *)gHomerunBall;
    ballScreenY = ballState->screenY;
    if ((((((s16)gHomerunBatY) <= ballScreenY) && (ballScreenY <= 0x60)) &&
         ((batOffsetY * previousPathX) >= ((batOffsetX * previousPathY) + batCrossProduct))) &&
        ((nextPathX * batOffsetY) <= ((nextPathY * batOffsetX) + batCrossProduct))) {
        struct HomerunBatState *batState;
        u8 *trigTable;
        register u8 contactFrame PIN("r3");
        register s32 velocityMask PIN("r5");
        register s32 sineValue PIN("r9");
        u8 *velocityTable;
        register unsigned int scaledVelocity;
        register s32 velocityComponent PIN("r0");
        register s32 trigIndexRegister PIN("r1");
        batState = (struct HomerunBatState *)gHomerunBat;
        contactFrame = batState->swingFrame;
        batState->contactFrame = contactFrame;
        trigTable = (u8 *)sSinCosTable;
        batAngle = batState->angle;
        trigIndex = 2;
        trigIndex = batAngle * trigIndex;
        velocityMask = 0xFF;
        trigIndex &= velocityMask;
        trigIndex += 0x40;
        trigIndex <<= 1;
        sineValue = *((s16 *)(trigTable + trigIndex));
        velocityTable = sHomerunHitSpeed;
        scaledVelocity = sineValue * ((s8)velocityTable[*difficultyPointer]);
        velocityComponent = scaledVelocity;
        trigIndexRegister = batAngle - 0x40;
        trigIndexRegister <<= 1;
        secondaryValue = *((s16 *)(trigTable + trigIndexRegister));
        velocityComponent *= secondaryValue;
        velocityComponent *= secondaryValue;
        ballState->xVelocity = -(((s32)velocityComponent) >> 0x18);
        batAngle = batState->angle;
        trigIndex = batAngle * 2;
        trigIndex &= velocityMask;
        trigIndex <<= 1;
        workingValue = *((s16 *)(trigIndex + trigTable));
        scaledVelocity = workingValue * ((s8)(*((volatile u8 *)(velocityTable + (*difficultyPointer)))));
        trigIndexRegister = batAngle - 0x40;
        trigIndexRegister <<= 1;
        secondaryValue = *((s16 *)(trigTable + trigIndexRegister));
        velocityComponent = secondaryValue;
        velocityComponent *= scaledVelocity;
        ballState->yVelocity = -(((s32)velocityComponent) >> 0x10);
        velocityTable = (u8 *)sHomerunHitLift;
        liftTablePointer = &velocityTable;
        secondaryValue = ((s8 *)(*liftTablePointer))[*difficultyPointer];
        trigIndex = batState->angle - 0x40;
        trigIndex <<= 1;
        velocityComponent = *((s16 *)(trigTable + trigIndex));
        velocityComponent *= secondaryValue;
        scaledVelocity = ((s32)velocityComponent) >> 8;
        ballState->gravity = scaledVelocity;
        if (((u8)contactFrame) == 0xC) {
            trigIndex = *difficultyPointer;
            secondaryValue = ((s8 *)velocityTable)[trigIndex];
            trigIndex = batState->angle;
            trigIndex = trigIndex - 0x40;
            trigIndex <<= 1;
            velocityComponent = *((s16 *)(trigTable + trigIndex));
            velocityComponent *= secondaryValue;
            ballState->gravity = scaledVelocity + (((s32)velocityComponent) >> 8);
        }
        if (((u8)(batState->contactFrame - 0xA)) <= 2) {
            m4aSongNumStart(0x234U);
        } else {
            m4aSongNumStart(0x233U);
        }
        return 1;
    }
    return 0;
}

s32 UpdateHomerunBatSwing(void)
{
    u8 *batState;
    u16 *batXPosition;
    u16 *batYPosition;
    const u8 *swingAngles;
    register s32 batCenterX PIN("r6");
    register s32 swingFrame PIN("r2");
    register const s16 *trigTable PIN("r3");
    s32 batAngle;
    s32 trigValue;
    s32 scaledOffset;
    s32 position;

    batState = (u8 *)gHomerunBat;
    swingFrame = batState[6] + 1;
    batState[6] = swingFrame;

    swingAngles = sHomerunBatAngles;
    batAngle = swingAngles[batState[6]];
    ((u16 *)batState)[2] = batAngle;

    batXPosition = &gHomerunBatX;
    trigTable = sSinCosTable;
    trigValue = trigTable[((u16 *)batState)[2]];
    scaledOffset = (trigValue * 25) >> 8;
    batCenterX = 215;
    *batXPosition = batCenterX - scaledOffset;

    batYPosition = &gHomerunBatY;
    batAngle = ((u16 *)batState)[2];
    batAngle += 64;
    trigValue = trigTable[batAngle];
    scaledOffset = (trigValue * 25) >> 8;
    *batYPosition = scaledOffset + 96;

    if ((u8)swingFrame > 21) {
        goto reset;
    }
    position = 0;
    goto done;

reset:
    {
        const u8 *initialSwingAngles;

        batState[6] = 0;
        initialSwingAngles = swingAngles;
        ((u16 *)batState)[2] = initialSwingAngles[0];
        *batXPosition = batCenterX;
        *batYPosition = 121;
        position = 1;
    }

done:
    return position;
}


s32 UpdateHomerunHitBall(void)
{
    struct HomerunBallState *ballState;
    register s32 xPosition PIN("r4");
    s32 yPosition;
    register s32 nextXPosition PIN("r3");
    s32 nextYPosition;
    s32 coordinateMask;
    register s32 workingValue PIN("r0");
    u8 *pitchResultPointer;
    u8 pitchResult;

    ballState = (struct HomerunBallState *)gHomerunBall;
    xPosition = ballState->xPosition;
    nextXPosition = xPosition << 16;
    if (nextXPosition > 0
        && (yPosition = ballState->yPosition, nextYPosition = yPosition << 16, nextYPosition > 0)
        && (workingValue = nextYPosition >> 20, workingValue <= 0x9F)
        && (workingValue = nextXPosition >> 20, workingValue <= 0xEF)) {
        workingValue = 0xC;
        workingValue = *(s8 *)((u8 *)ballState + workingValue);
        nextXPosition = xPosition + workingValue;
        ballState->xPosition = nextXPosition;
        workingValue = 0xD;
        workingValue = *(s8 *)((u8 *)ballState + workingValue);
        xPosition = yPosition + workingValue;
        ballState->yPosition = xPosition;
        workingValue = ballState->rotation;
        workingValue += 5;
        coordinateMask = 0xFF;
        workingValue &= coordinateMask;
        ballState->rotation = workingValue;
        workingValue = 0xE;
        workingValue = *(s8 *)((u8 *)ballState + workingValue);
        yPosition = ballState->scale;
        workingValue += yPosition;
        ballState->scale = workingValue;
        {
            s32 backgroundY;

            workingValue <<= 16;
            backgroundY = 0x80;
            backgroundY <<= 18;
            if ((u32)workingValue <= backgroundY) {
                goto ret0;
            } else {
                u16 *ballXPosition;
                register s32 backgroundX PIN("r2");
                vu16 *backgroundYRegister;
                s32 backgroundPositionAddress;

                ballXPosition = &gHomerunBgX;
                backgroundX = (s32)(nextXPosition << 16) >> 20;
                backgroundPositionAddress = 0x80;
                backgroundPositionAddress <<= 1;
                backgroundY = backgroundPositionAddress;
                backgroundY += 0;
                backgroundX = backgroundY - backgroundX;
                *ballXPosition = backgroundX;
                backgroundPositionAddress = (s32)&gHomerunBgY;
                workingValue = (s32)(xPosition << 16) >> 20;
                backgroundY -= workingValue;
                *(u16 *)backgroundPositionAddress = backgroundY;
                backgroundYRegister = (vu16 *)0x04000012;
                *backgroundYRegister = backgroundY;
                backgroundYRegister--;
                *backgroundYRegister = backgroundX;
                backgroundYRegister += 3;
                {
                    register s32 screenOffset PIN("r3");

                    xPosition = 0xC0;
                    xPosition <<= 1;
                    ASM_BARRIER(: "+r"(xPosition));
                    screenOffset = xPosition;
                    backgroundY += screenOffset;
                    backgroundY &= coordinateMask;
                    *backgroundYRegister = backgroundY;
                    backgroundYRegister--;
                    backgroundX += screenOffset;
                }
                backgroundX &= coordinateMask;
                *backgroundYRegister = backgroundX;
                *(vu16 *)0x04000000 = 0x1F00;
                pitchResultPointer = &gHomerunPitchResult;
                pitchResult = 5;
            }
        }
    } else if ((u8)(((struct HomerunBatState *)gHomerunBat)->contactFrame - 0xA) <= 2) {
        pitchResultPointer = &gHomerunPitchResult;
        pitchResult = 1;
    } else {
        pitchResultPointer = &gHomerunPitchResult;
        pitchResult = 0;
    }

    *pitchResultPointer = pitchResult;
    return 1;

ret0:
    return 0;
}


s32 UpdateHomerunMissedBall(void)
{
    struct HomerunBallState *ballState;
    s32 xPosition;
    s32 yPosition;
    register s32 workingValue PIN("r0");
    s32 groundHeight;
    register s32 minimumY PIN("r3");
    u8 *pitchResultPointer;
    u8 pitchResult;

    ballState = (struct HomerunBallState *)gHomerunBall;
    xPosition = ballState->xPosition;
    workingValue = xPosition << 16;
    groundHeight = workingValue >> 20;
    minimumY = 20;
    minimumY = -minimumY;
    if (groundHeight > minimumY
        && (yPosition = ballState->yPosition, workingValue = yPosition << 16, workingValue = workingValue >> 20, workingValue > minimumY)
        && workingValue <= 0xB3
        && (workingValue = 0x103, groundHeight <= workingValue)) {
        workingValue = 0xC;
        workingValue = *(s8 *)((u8 *)ballState + workingValue);
        workingValue = xPosition + workingValue;
        ballState->xPosition = workingValue;
        workingValue = 0xD;
        workingValue = *(s8 *)((u8 *)ballState + workingValue);
        workingValue = yPosition + workingValue;
        ballState->yPosition = workingValue;
        workingValue = ballState->rotation;
        workingValue += 5;
        groundHeight = 0xFF;
        workingValue &= groundHeight;
        ballState->rotation = workingValue;
        workingValue = 0xE;
        workingValue = *(s8 *)((u8 *)ballState + workingValue);
        groundHeight = ballState->scale;
        workingValue += groundHeight;
        ballState->scale = workingValue;
        workingValue = (u16)workingValue;
        groundHeight = 0x80;
        groundHeight <<= 3;
        if ((u32)workingValue > (u32)groundHeight) {
            ballState->scale = groundHeight;
        }
        goto ret0;
    } else if ((u8)(((struct HomerunBatState *)gHomerunBat)->contactFrame - 0xA) <= 2) {
        pitchResultPointer = &gHomerunPitchResult;
        pitchResult = 1;
    } else {
        pitchResultPointer = &gHomerunPitchResult;
        pitchResult = 0;
    }

    *pitchResultPointer = pitchResult;
    return 1;

ret0:
    return 0;
}

void DrawHomerunDerbyScore(void)
{
    const u16 *scoreTiles;
    u32 score;

    if (gMinigameMedalHighlight != 0) {
        DrawMinigameNumber(gMedalCount, sHomerunHighlightedMedalTiles, 0x3800);
    } else {
        DrawMinigameNumber(gMedalCount, sHomerunMedalTiles, 0x3800);
    }

    score = gMinigameScore;
    scoreTiles = sHomerunDigitTiles;
    DrawMinigameNumber(score, scoreTiles, 0x4000);
    DrawMinigameHighScore(gMinigameHighScores, scoreTiles + 0x400, 0x4800);
}


void DrawHomerunDerby(void)
{
  u16 affineMatrix[4];
  u16 *affinePb;
  u16 *affinePc;
  u16 *affinePd;
  register struct HomerunCameraState *cameraTransform PIN("r9");
  struct HomerunCameraState *cameraForMatrix;
  register struct HomerunCameraState *cameraForRotation PIN("r3");
  register struct HomerunPitcherState *pitcherState PIN("r3");
  register struct HomerunSimpleAnimation *activeAnimationState PIN("r3");
  struct AnimationFrame *activeAnimation;
  u32 animationCounterLimit;
  register u32 oamXMask PIN("sl");
  s32 oamAttributeMask;
  register long preservedOamBits;
  register OamByte *dataPointer PIN("r2");
  OamByte *oamBuffer;
  OamByte *oamEntry;
  register OamByte *objectState PIN("r5");
  struct HomerunCameraState *cameraForSprites;
  s16 matrixCosine;
  s16 animationMatrixPb;
  s16 animationMatrixPc;
  s16 animationMatrixPd;
  struct HomerunCameraState *oamXMaskPointer;
  s16 matrixSine;
  s16 matrixNegativeSine;
  struct HomerunSimpleAnimation *bonusRenderState;
  s16 matrixCosineD;
  s16 ballMatrixCosine;
  s16 ballMatrixSine;
  s16 ballMatrixNegativeSine;
  int ballMatrixCosineD;
  s16 animationMatrixPa;
  register unsigned int oamByteMask PIN("sl");
  register OamByte *resultAnimation PIN("r4");
  register s32 workingOffset PIN("r0");
  struct AnimationFrame *bonusAnimation;
  u32 frameDuration;
  OamByte *pitcherAnimation;
  s32 entryCounter;
  s32 cameraEntryCount;
  s32 secondaryEntryCount;
  s32 batEntryCount;
  register s32 bonusEntryCount PIN("r7");
  s32 pitcherEntryCount;
  s32 savedOamIndex;
  struct AnimationFrame *bonusAnimationFrames;
  u16 *affinePointer;
  u16 **affinePointerReference;
  u16 *bonusFrameData;
  u16 *pitcherFrameData;
  u16 *resultFrameData;
  u16 *ballFrameData;
  u16 *secondaryFrameData;
  u16 *batFrameData;
  u16 *activeFrameData;
  register u16 *frameData PIN("r3");
  u8 *pitcherOamBytes;
  struct HomerunCameraState *cameraData;
  int clearAffineModeMask;
  register OamByte *oamDataDestination PIN("r6");
  u16 frameTimeValue;
  int clearOamXMask;
  u16 affinePdValue;
  s32 ballAffinePd;
  s32 uiAnimationTimer;
  s32 animationTimer;
  u16 affinePaValue;
  register s32 positionValue PIN("r1");
  u8 *stackPointer;
  s32 frameCounter;
  s32 bonusAnimationTimer;
  s32 pitcherAnimationTimer;
  s32 resultAnimationTimer;
  register u16 oamAttribute PIN("r2");
  register s32 oamCoordinate10 PIN("r2");
  u16 oamAttribute11;
  register s32 oamCoordinate12 PIN("r2");
  register u16 oamAttribute13 PIN("r2");
  u16 oamAttribute14;
  register s32 oamCoordinate15 PIN("r2");
  u16 pitchSoundId;
  register u16 oamAttribute16 PIN("r2");
  register s32 oamCoordinate17 PIN("r2");
  u16 oamAttribute2;
  int oamXMaskValue;
  register struct AnimationFrame *bonusFrame PIN("r0");
  register s32 coordinateMask PIN("r2");
  register u16 oamAttribute3 PIN("r2");
  register s32 oamCoordinate4 PIN("r2");
  u16 oamAttribute5;
  u16 oamAttribute6;
  register s32 oamCoordinate7 PIN("r2");
  register s32 oamCoordinate8 PIN("r2");
  register s32 oamCoordinate9 PIN("r2");
  u8 byteValue;
  u8 pitcherFrameTime;
  u8 pitcherAnimationIndex;
  u8 resultFrameTime;
  u32 currentFrameDuration;
  u8 frameTimeA;
  u8 frameTimeB;
  u8 frameTimeC;
  u8 frameTimeD;
  u8 pitcherAnimationType;
  u8 animationType;
  u8 bonusAnimationIndex;
  s32 firstOamIndex;
  register s32 nextOamIndex PIN("r8");
  void *oamBufferPointer;
  int clearObjectModeMask;
  void *currentOamBytes;
  void *resultOamBytes;
  OamByte *ballOamBytes;
  void *secondaryOamBytes;
  OamByte *batOamBytes;
  void *staticOamBytes;
  OamByte *bonusOamBytes;
  void *pitcherOamEntry;
  bonusRenderState = &gHomerunBonusRender;
  firstOamIndex = 0;
  nextOamIndex = gOamSlotsUsed;
  workingOffset = nextOamIndex * 8;
  oamBuffer = (OamByte *) gOamBuffer;
  oamDataDestination = (OamByte *) (((s32) workingOffset) + ((s32) oamBuffer));
  byteValue = gMinigameCameraState.oamMode;
  cameraTransform = &gMinigameCameraState;
  if (byteValue != 0)
  {
    if (byteValue == 1)
    {
      frameData = GP(sHomerunCameraOamMode1);
    }
    if (byteValue == 2)
    {
      frameData = GP(sHomerunCameraOamMode2);
    }
    nextOamIndex += *frameData;
    frameData = (u16 *) (((u8 *) frameData) + 2);
    affinePb = &affineMatrix[1];
    affinePc = &affineMatrix[2];
    affinePd = affinePb + 2;
    if (firstOamIndex < nextOamIndex)
    {
      cameraForSprites = cameraTransform;
      oamEntry = oamBuffer;
      oamXMask = 0x1FF;
      oamAttributeMask = -13;
      firstOamIndex = nextOamIndex;
      do
      {
        oamAttribute = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute;
        oamDataDestination += 2;
        *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((*cameraForSprites).yPosition + oamAttribute) - 0x10);
        *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
        oamAttribute2 = *((u16 *) (((u8 *) frameData) + 0));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute2;
        oamDataDestination += 2;
        positionValue = *((s16 *) (((u8 *) cameraForSprites) + 8));
        positionValue = oamAttribute2 + positionValue;
        positionValue -= 0x20;
        coordinateMask = oamXMask;
        positionValue &= coordinateMask;
        oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
        preservedOamBits = 0xFFFFFE00;
        preservedOamBits &= oamAttribute2;
        *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
        preservedOamBits = -0xF;
        *((u8 *) (((u8 *) oamEntry) + 3)) = (u8) (preservedOamBits & positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = (u16) (*((u16 *) (((u8 *) frameData) + 0)));
        frameData = (u16 *) (((u8 *) frameData) + 2);
        positionValue = *((u8 *) (((u8 *) oamEntry) + 5));
        *((u8 *) (((u8 *) oamEntry) + 5)) = (u8) (oamAttributeMask & positionValue);
        oamDataDestination += 4;
        oamEntry = oamEntry + 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    cameraForRotation = cameraTransform;
    matrixCosine = sSinCosTable[(((u16) cameraForRotation->rotation) >> 4) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) matrixCosine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForRotation = cameraTransform;
    matrixSine = sSinCosTable[((u16) cameraForRotation->rotation) >> 4];
    *affinePb = (u16) FixedMul((s32) matrixSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForRotation->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixNegativeSine = 0 - sSinCosTable[((u16) cameraForMatrix->rotation) >> 4];
    *affinePc = (u16) FixedMul((s32) matrixNegativeSine, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    cameraForMatrix = cameraTransform;
    matrixCosineD = sSinCosTable[(((u16) cameraForMatrix->rotation) >> 4) + 0x40];
    affinePdValue = (u16) FixedMul((s32) matrixCosineD, (s32) ((s16) FixedInverse((s32) (((u16) cameraForMatrix->scale) >> 4))));
    *affinePd = affinePdValue;
    oamBuffer = (OamByte *) gOamBuffer;
    *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) affineMatrix[0];
    *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
    *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = affinePdValue;
  }
  if (((((u32) gMinigameState) > 2U) && (gMinigameState != 0xF)) && (gMinigameState != 0xE))
  {
    if (((u32) ((u8) (gMinigameState - 8))) <= 1U)
    {
      register s32 countSource PIN("r2");
      dataPointer = (OamByte *) (&gHomerunBallRenderState);
      frameCounter = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
      *((u16 *) (((u8 *) dataPointer) + 0)) = frameCounter;
      frameTimeB = sHomerunBallHitDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
      animationCounterLimit = (u32) ((u16) frameCounter);
      objectState = dataPointer;
      if (((u32) frameTimeB) < animationCounterLimit)
      {
        *((u16 *) (((u8 *) objectState) + 0)) = 0U;
        *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
        frameTimeB = sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
        if (frameTimeB == 0)
        {
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) frameTimeB;
        }
      }
      secondaryFrameData = GP(sHomerunBallHitAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam);
      nextOamIndex += *secondaryFrameData;
      frameData = secondaryFrameData + 1;
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        cameraTransform = (struct HomerunCameraState *) objectState;
        oamAttributeMask = -15;
        workingOffset = firstOamIndex << 3;
        oamEntry = (OamByte *) (workingOffset + positionValue);
        oamXMask = 0x1FF;
        if (firstOamIndex) {
          countSource = nextOamIndex;
        } else {
          countSource = nextOamIndex;
        }
        secondaryEntryCount = countSource - firstOamIndex;
        do
        {
          oamAttribute5 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute5;
          oamDataDestination += 2;
          cameraForMatrix = cameraTransform;
          *((s8 *) (((u8 *) oamEntry) + 0)) = (s8) (((((s32) (cameraForMatrix->rotation << 0x10)) >> 0x14) + oamAttribute5) - 0x20);
          *((u8 *) (((u8 *) oamEntry) + 1)) = (u8) ((*((u8 *) (((u8 *) oamEntry) + 1))) | 3);
          oamAttribute6 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute6;
          oamDataDestination += 2;
          positionValue = (s32) (cameraTransform->scale << 0x10);
          positionValue >>= 0x14;
          positionValue = oamAttribute6 + positionValue;
          positionValue -= 0x20;
          coordinateMask = oamXMask;
          positionValue &= coordinateMask;
          oamAttribute2 = *((u16 *) (((u8 *) oamEntry) + 2));
          preservedOamBits = 0xFFFFFE00;
          preservedOamBits &= oamAttribute2;
          *((u16 *) (((u8 *) oamEntry) + 2)) = (u16) (preservedOamBits | positionValue);
          positionValue = *((u8 *) (((u8 *) oamEntry) + 3));
          currentOamBytes = (u8 *) oamEntry;
          *((u8 *) (((u8 *) currentOamBytes) + 3)) = (u8) (oamAttributeMask & positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          oamDataDestination += 4;
          oamEntry += 8;
          secondaryEntryCount -= 1;
        }
        while (secondaryEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
      ballMatrixCosine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      affineMatrix[0] = (u16) FixedMul((s32) ballMatrixCosine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixSine = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePb = (u16) FixedMul((s32) ballMatrixSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixNegativeSine = 0 - sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation];
      *affinePc = (u16) FixedMul((s32) ballMatrixNegativeSine, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale)));
      ballMatrixCosineD = sSinCosTable[((struct HomerunBallRenderState *) objectState)->rotation + 0x40];
      ballAffinePd = (*affinePd = FixedMul((s32) ballMatrixCosineD, (s32) ((s16) FixedInverse((s32) ((struct HomerunBallRenderState *) objectState)->scale))));
      oamBuffer = (OamByte *) gOamBuffer;
      *((u16 *) (((u8 *) oamBuffer) + 6)) = (u16) (affinePaValue = affineMatrix[0]);
      *((u16 *) (((u8 *) oamBuffer) + 0xE)) = (u16) (*affinePb);
      *((u16 *) (((u8 *) oamBuffer) + 0x16)) = (u16) (*affinePc);
      *((u16 *) (((u8 *) oamBuffer) + 0x1E)) = ballAffinePd;
    }
    else
    {
      affinePb = (u16 *) (((u8 *)affineMatrix) + 2);
      affinePc = (u16 *) (((u8 *)affineMatrix) + 4);
      affinePd = (u16 *) (((u8 *)affineMatrix) + 6);
      if (gMinigameState == 7)
      {
        dataPointer = (OamByte *) (&gHomerunBallRenderState);
        positionValue = (*((u16 *) (((u8 *) dataPointer) + 0))) + 1;
        *((u16 *) (((u8 *) dataPointer) + 0)) = positionValue;
        workingOffset = (u32) sHomerunBallPitchDurations[*((u16 *) (((u8 *) dataPointer) + 2))].time;
        affinePaValue = (u16) positionValue;
        objectState = dataPointer;
        if (((u32) workingOffset) < ((u32) affinePaValue))
        {
          *((u16 *) (((u8 *) objectState) + 0)) = 0U;
          *((u16 *) (((u8 *) objectState) + 2)) = (u16) ((*((u16 *) (((u8 *) objectState) + 2))) + 1);
          frameTimeA = sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].time;
          animationMatrixPc = (u16) frameTimeA;
          if (frameTimeA == 0)
          {
            *((u16 *) (((u8 *) objectState) + 2)) = animationMatrixPc;
          }
        }
        ballFrameData = GP(sHomerunBallPitchAnimation[*((u16 *) (((u8 *) objectState) + 2))].oam);
        nextOamIndex += *ballFrameData;
        frameData = ballFrameData + 1;
        stackPointer = ((u8 *)affineMatrix);
        affinePointerReference = (u16 **) (&stackPointer);
        affinePb = (u16 *) (((u8 *) (*affinePointerReference)) + 2);
        affinePc = (u16 *) (((u8 *) (*affinePointerReference)) + 4);
        affinePd = (u16 *) (((u8 *) (*affinePointerReference)) + 6);
        clearObjectModeMask = -4;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamAttributeMask = 0xFFFFFE00;
          workingOffset = firstOamIndex << 3;
          ballOamBytes = (OamByte *) (((s32) workingOffset) + positionValue);
          cameraTransform = (struct HomerunCameraState *) 0x1FF;
          oamByteMask = -0xD;
          cameraEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamAttribute3 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute3;
            oamDataDestination += 2;
            *((s8 *) (((u8 *) ballOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 0xA))) + oamAttribute3);
            *((u8 *) (((u8 *) ballOamBytes) + 1)) = (u8) (clearObjectModeMask & (*((u8 *) (((u8 *) ballOamBytes) + 1))));
            oamCoordinate4 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate4;
            oamDataDestination += 2;
            positionValue = *((s16 *) (((u8 *) objectState) + 8));
            positionValue = oamCoordinate4 + positionValue;
            positionValue &= (s32) cameraTransform;
            oamAttribute = *((u16 *) (((u8 *) ballOamBytes) + 2));
            preservedOamBits = oamAttributeMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) ballOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) ballOamBytes) + 5));
            *((u8 *) (((u8 *) ballOamBytes) + 5)) = (u8) (oamByteMask & positionValue);
            oamDataDestination += 4;
            ballOamBytes += 8;
            cameraEntryCount -= 1;
          }
          while (cameraEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
      }
    }
    clearOamXMask = 0xFFFFFE00;
    oamAttributeMask = gHomerunMissCount;
    if (((s32) oamAttributeMask) > 2)
    {
      oamAttributeMask = 2;
    }
    objectState = 0;
    if (((s32) objectState) < ((s32) oamAttributeMask))
    {
      do
      {
        nextOamIndex += *GP(sHomerunStaticOam);
        frameData = GP(sHomerunStaticOam) + 1;
        if (((s32) firstOamIndex) < ((s32) nextOamIndex))
        {
          positionValue = (s32) ((OamByte *) gOamBuffer);
          oamByteMask = -0xD;
          workingOffset = firstOamIndex << 3;
          secondaryOamBytes = (OamByte *) (workingOffset + positionValue);
          secondaryEntryCount = nextOamIndex - firstOamIndex;
          do
          {
            oamCoordinate7 = *(frameData++);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate7;
            oamDataDestination += 2;
            workingOffset = oamCoordinate7 + 0x20;
            *((s8 *) (((u8 *) secondaryOamBytes) + 0)) = (s8) (workingOffset - (((s32) objectState) * 8));
            oamAttribute5 = *(frameData++);
            oamCoordinate8 = oamAttribute5;
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate8;
            oamDataDestination += 2;
            positionValue = oamCoordinate8 + 0xE0;
            preservedOamBits = 0x1FF;
            positionValue &= preservedOamBits;
            oamAttribute = *((u16 *) (((u8 *) secondaryOamBytes) + 2));
            preservedOamBits = clearOamXMask;
            preservedOamBits &= oamAttribute;
            *((u16 *) (((u8 *) secondaryOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
            *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
            positionValue = *((u8 *) (((u8 *) secondaryOamBytes) + 5));
            *((u8 *) (((u8 *) secondaryOamBytes) + 5)) = (u8) ((oamByteMask & positionValue) | 8);
            oamDataDestination += 4;
            secondaryOamBytes += 8;
            secondaryEntryCount -= 1;
          }
          while (secondaryEntryCount != 0);
          firstOamIndex = nextOamIndex;
        }
        objectState = (OamByte *) (((s32) objectState) + 1);
      }
      while (((s32) objectState) < ((s32) oamAttributeMask));
    }
    if (gMinigameMedalHighlight != 0)
    {
      uiAnimationTimer = gMinigameUiAnimation.animationTimer + 1;
      gMinigameUiAnimation.animationTimer = uiAnimationTimer;
      if (((u32) sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time) < ((u32) ((u16) uiAnimationTimer)))
      {
        gMinigameUiAnimation.animationTimer = 1U;
        gMinigameUiAnimation.animationFrame = (u16) (gMinigameUiAnimation.animationFrame + 1);
        frameTimeC = sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].time;
        if (frameTimeC == 0)
        {
          gMinigameUiAnimation.animationFrame = (u16) frameTimeC;
          gMinigameUiAnimation.animationTimer = (u16) frameTimeC;
          gMinigameMedalHighlight = 0;
          DrawHomerunDerbyScore();
        }
      }
    }
    oamXMaskValue = 0x1FF;
    batFrameData = GP(sHomerunBatAnimation[gMinigameUiAnimation.animationFrame].oam);
    nextOamIndex += *batFrameData;
    frameData = batFrameData + 1;
    cameraTransform = (struct HomerunCameraState *) &gHomerunBatAnimation;
    activeAnimation = sHomerunBallIdleAnimation;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      oamAttributeMask = 0x1FF;
      oamByteMask = -0xD;
      workingOffset = firstOamIndex << 3;
      batOamBytes = (OamByte *) (workingOffset + positionValue);
      firstOamIndex = nextOamIndex - firstOamIndex;
      do
      {
        oamCoordinate9 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate9;
        oamDataDestination += 2;
        workingOffset = oamCoordinate9;
        workingOffset += 0x92;
        *((s8 *) (((u8 *) batOamBytes) + 0)) = (s8) workingOffset;
        oamCoordinate10 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate10;
        oamDataDestination += 2;
        positionValue = oamCoordinate10 + 0x18;
        positionValue &= oamAttributeMask;
        clearOamXMask = 0xFFFFFE00;
        *((u16 *) (((u8 *) batOamBytes) + 2)) = (u16) ((clearOamXMask & (*((u16 *) (((u8 *) batOamBytes) + 2)))) | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        positionValue = *((u8 *) (((u8 *) batOamBytes) + 5));
        *((u8 *) (((u8 *) batOamBytes) + 5)) = (oamByteMask & positionValue) | 8;
        oamDataDestination += 4;
        batOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    activeAnimationState = (struct HomerunSimpleAnimation *) cameraTransform;
    animationTimer = activeAnimationState->animationTimer + 1;
    activeAnimationState->animationTimer = animationTimer;
    currentFrameDuration = (u32) activeAnimation[activeAnimationState->animationFrame].time;
    if (currentFrameDuration < ((u32) ((u16) animationTimer)))
    {
      activeAnimationState->animationTimer = 0U;
      activeAnimationState->animationFrame = (u16) (activeAnimationState->animationFrame + 1);
      frameTimeD = activeAnimation[activeAnimationState->animationFrame].time;
      if (frameTimeD == 0)
      {
        activeAnimationState->animationFrame = (u16) frameTimeD;
      }
    }
    cameraForMatrix = cameraTransform;
    activeFrameData = GP(activeAnimation[((struct HomerunSimpleAnimation *) cameraForMatrix)->animationFrame].oam);
    nextOamIndex += *activeFrameData;
    frameData = activeFrameData + 1;
    if (((s32) firstOamIndex) < nextOamIndex)
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      {
        register u32 maskSeed PIN("r2");
        if (firstOamIndex) {
          maskSeed = 0xFFFFFE00;
        } else {
          maskSeed = 0xFFFFFE00;
        }
        oamAttributeMask = maskSeed;
      }
      workingOffset = firstOamIndex << 3;
      staticOamBytes = (OamByte *) (workingOffset + positionValue);
      objectState = (OamByte *) oamXMaskValue;
      oamByteMask = -15;
      {
        register s32 nextCount PIN("r1") = nextOamIndex;
        firstOamIndex = nextCount - firstOamIndex;
      }
      do
      {
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        workingOffset = oamCoordinate12;
        workingOffset += 0x60;
        *((u8 *) (((u8 *) staticOamBytes) + 0)) = (u8) workingOffset;
        positionValue = *((u8 *) (((u8 *) staticOamBytes) + 1));
        *((u8 *) (((u8 *) staticOamBytes) + 1)) = (u8) ((clearObjectModeMask & positionValue) | 1);
        oamCoordinate12 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate12;
        oamDataDestination += 2;
        positionValue = oamCoordinate12 + 0xD7;
        positionValue &= (s32) objectState;
        oamAttribute = *((u16 *) (((u8 *) staticOamBytes) + 2));
        preservedOamBits = oamAttributeMask;
        preservedOamBits &= oamAttribute;
        *((u16 *) (((u8 *) staticOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
        pitcherAnimationType = *((u8 *) (((u8 *) staticOamBytes) + 3));
        workingOffset = oamXMask;
        workingOffset &= pitcherAnimationType;
        *((u8 *) (((u8 *) staticOamBytes) + 3)) = (u8) (workingOffset | 2);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (((u8 *) staticOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) staticOamBytes) + 5))) | 0xC);
        oamDataDestination += 4;
        staticOamBytes += 8;
        firstOamIndex -= 1;
      }
      while (firstOamIndex != 0);
      firstOamIndex = nextOamIndex;
    }
    savedOamIndex = nextOamIndex;
    cameraData = cameraTransform;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPa = sSinCosTable[(*((u16 *) (((u8 *) dataPointer) + 4))) + 0x40];
    affineMatrix[0] = (u16) FixedMul((s32) animationMatrixPa, (s32) ((s16) FixedInverse(0x100)));
    objectState = (OamByte *) 0x100;
    dataPointer = (OamByte *) cameraData;
    animationMatrixPb = sSinCosTable[*((u16 *) (((u8 *) dataPointer) + 4))];
    *affinePb = (u16) FixedMul((s32) animationMatrixPb, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (frameData = (u16 *) cameraData);
    animationMatrixPc = 0 - sSinCosTable[frameData[2]];
    *affinePc = (u16) FixedMul((s32) animationMatrixPc, (s32) ((s16) FixedInverse((s32) objectState)));
    frameData = (u16 *) cameraData;
    animationMatrixPd = sSinCosTable[frameData[2] + 0x40];
    frameTimeValue = (*affinePd = (u16) FixedMul((s32) animationMatrixPd, (s32) ((s16) FixedInverse((s32) objectState))));
    oamBufferPointer = (u8 *) ((OamByte *) gOamBuffer);
    oamAttribute14 = (affinePaValue = affineMatrix[0]);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x26)) = (u16) oamAttribute14;
    oamAttribute11 = *affinePb;
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x2E)) = (u16) oamAttribute11;
    *((u16 *) (((u8 *) oamBufferPointer) + 0x36)) = (u16) (*affinePc);
    *((u16 *) (((u8 *) ((OamByte *) gOamBuffer)) + 0x3E)) = frameTimeValue;
    if (gHomerunBonusRender.animationIndex == 0)
    {
    }
    else
    {
      bonusAnimationIndex = gHomerunBonusRender.animationIndex;
      bonusAnimationFrames = GP(sHomerunBonusAnimations[bonusAnimationIndex]);
      bonusOamBytes = (OamByte *) bonusAnimationFrames;
      bonusAnimationTimer = gHomerunBonusRender.animationTimer + 1;
      gHomerunBonusRender.animationTimer = bonusAnimationTimer;
      frameDuration = (u32) ((struct AnimationFrame *) ((gHomerunBonusRender.animationFrame << 3) + (u32) bonusAnimationFrames))->time;
      if (frameDuration < ((u32) ((u16) bonusAnimationTimer)))
      {
        gHomerunBonusRender.animationTimer = 1U;
        bonusAnimation = bonusAnimationFrames;
        gHomerunBonusRender.animationFrame = (u16) (gHomerunBonusRender.animationFrame + 1);
        if ((byteValue = bonusAnimation[gHomerunBonusRender.animationFrame].time) == 0)
        {
          gHomerunBonusRender.animationFrame = 0;
          animationType = gMinigameSequenceState;
          if (animationType == 1)
          {
            if (!(animationType & ((u32 (*)(void)) MinigameRandom)()))
            {
              m4aSongNumStart(0x237U);
            }
            else
            {
              m4aSongNumStart(0x238U);
            }
            m4aSongNumStart(0x2CCU);
            gHomerunBonusRender.animationIndex = (u8) gHomerunBonusRender.nextAnimationIndex;
            gHomerunBonusRender.animationTimer = 0U;
            gMinigameSequenceState += 1;
          }
        }
      }
      bonusOamBytes = (OamByte *) GP(sHomerunBonusAnimations[(*bonusRenderState).animationIndex]);
      bonusFrameData = GP(((struct AnimationFrame *) bonusOamBytes)[gHomerunBonusRender.animationFrame].oam);
      nextOamIndex += *bonusFrameData;
      frameData = bonusFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        OamByte *render_state;
        positionValue = (s32)gOamBuffer;
        render_state = (OamByte *) (&gHomerunBonusRender);
        cameraTransform = (struct HomerunCameraState *) 0x1FF;
        oamXMask = 0xFFFFFE00;
        bonusOamBytes = (OamByte *)((firstOamIndex << 3) + positionValue);
        bonusEntryCount = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute13 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute13;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) bonusOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) render_state) + 6))) + oamAttribute13);
          oamAttribute2 = *(frameData++);
          *((u16 *)oamDataDestination) = oamAttribute2;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) render_state) + 4));
          positionValue = oamAttribute2 + positionValue;
          positionValue &= (s32) cameraTransform;
          oamAttribute = *((u16 *) (((u8 *) bonusOamBytes) + 2));
          preservedOamBits = oamXMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) bonusOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          *((u8 *) (((u8 *) bonusOamBytes) + 5)) = (u8) ((*((u8 *) (((u8 *) bonusOamBytes) + 5))) | 0xC);
          oamDataDestination += 4;
          bonusOamBytes += 8;
          bonusEntryCount -= 1;
        }
        while (bonusEntryCount != 0);
        firstOamIndex = nextOamIndex;
      }
    }
    pitcherAnimation = (OamByte *) GP(sHomerunPitcherAnimations[gHomerunPitcherAnimation.animationIndex]);
    pitcherAnimationTimer = gHomerunPitcherAnimation.animationTimer + 1;
    gHomerunPitcherAnimation.animationTimer = pitcherAnimationTimer;
    if (((u32) ((struct AnimationFrame *) pitcherAnimation)[gHomerunPitcherAnimation.animationFrame].time) >= ((u32) ((u16) pitcherAnimationTimer)))
    {
    }
    else
    {
      gHomerunPitcherAnimation.animationTimer = 0U;
      gHomerunPitcherAnimation.animationFrame = (u16) (gHomerunPitcherAnimation.animationFrame + 1);
      if (gHomerunPitcherAnimation.animationIndex == 4)
      {
        switch (gHomerunPitcherAnimation.animationFrame)
        {
          case 10:
            gMinigameState = 7;
            m4aSongNumStart(sHomerunPitchSounds[gHomerunPitchPath * 2]);
            break;
            pitchSoundId = 0x22BU;

          case 0:
            m4aSongNumStart(0x228U);
            break;

          case 1:
            m4aSongNumStart(0x229U);
            break;

          case 5:
            m4aSongNumStart(0x22AU);
            break;

          case 8:
            m4aSongNumStart(0x22BU);
            break;

        }

      }
      pitcherState = (struct HomerunPitcherState *) &gHomerunPitcherAnimation;
      pitcherFrameTime = ((struct AnimationFrame *) pitcherAnimation)[pitcherState->animationFrame].time;
      if (pitcherFrameTime == 0)
      {
        pitcherState->animationFrame = (u16) pitcherFrameTime;
        pitcherAnimationIndex = pitcherState->animationIndex;
        if (pitcherAnimationIndex == 4)
        {
          pitcherState->animationIndex = 5U;
          pitcherAnimation = (OamByte *) GP(sHomerunPitcherAnimations[5]);
        }
        else
          if (((u32) ((u8) (pitcherAnimationIndex - 1))) <= 3U)
        {
          pitcherState->pitchCounter = (u8) (pitcherState->pitchCounter + 1);
          if (gDifficulty == 2)
          {
            dataPointer = &sHomerunPitchAnimationsHard;
          }
          else
          {
            dataPointer = &sHomerunPitchAnimationsNormal;
          }
          entryCounter = (gHomerunPitchPath * 5) - 1;
          pitcherState->animationIndex = (u8) (*((u8 *) ((entryCounter + pitcherState->pitchCounter) + ((s32) dataPointer))));
          dataPointer = (OamByte *) sHomerunPitcherAnimations;
          objectState = (OamByte *) &gHomerunPitcherAnimation;
          pitcherAnimationType = *((u8 *) (((u8 *) objectState) + 4));
          pitcherAnimation = (OamByte *) GP(((GBA_PTR(struct AnimationFrame *) *) dataPointer)[pitcherAnimationType]);
          if (pitcherAnimationType == 3)
          {
            m4aSongNumStart(0x226U);
          }
          if ((*((u8 *) (((u8 *) objectState) + 4))) == 2)
          {
            m4aSongNumStart(0x227U);
          }
          oamXMaskPointer = (struct HomerunCameraState *) 0x1FF;
        }
      }
    }
    pitcherFrameData = GP(((struct AnimationFrame *) pitcherAnimation)[((struct HomerunPitcherState *) &gHomerunPitcherAnimation)->animationFrame].oam);
    nextOamIndex += *pitcherFrameData;
    frameData = pitcherFrameData + 1;
    if (((s32) firstOamIndex) < ((s32) nextOamIndex))
    {
      positionValue = (s32) ((OamByte *) gOamBuffer);
      objectState = (OamByte *) 0x1FF;
      cameraTransform = (struct HomerunCameraState *) 0xFFFFFE00;
      workingOffset = firstOamIndex << 3;
      pitcherOamEntry = (OamByte *) (workingOffset + positionValue);
      positionValue = nextOamIndex;
      pitcherEntryCount = positionValue - firstOamIndex;
      do
      {
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        workingOffset = oamCoordinate15;
        workingOffset += 0x58;
        *((u8 *) (((u8 *) pitcherOamEntry) + 0)) = (u8) workingOffset;
        oamCoordinate15 = *(frameData++);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate15;
        oamDataDestination += 2;
        positionValue = oamCoordinate15 + 0x48;
        positionValue &= (s32) objectState;
        pitcherOamBytes = (u8 *) pitcherOamEntry;
        oamAttribute = *((u16 *) (pitcherOamBytes + 2));
        preservedOamBits = (s32) cameraTransform;
        preservedOamBits &= oamAttribute;
        *((u16 *) (pitcherOamBytes + 2)) = (u16) (preservedOamBits | positionValue);
        *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
        *((u8 *) (pitcherOamBytes + 5)) = (u8) ((*((u8 *) (pitcherOamBytes + 5))) | 0xC);
        oamDataDestination += 4;
        pitcherOamEntry += 8;
        pitcherEntryCount -= 1;
      }
      while (pitcherEntryCount != 0);
      firstOamIndex = nextOamIndex;
    }
    if (((u32) ((u8) (gMinigameState - 0xB))) <= 2U)
    {
      resultAnimation = (OamByte *) GP(sHomerunResultAnimations[gHomerunPitchResult]);
      resultAnimationTimer = gHomerunResultRender.animationTimer + 1;
      if (1)
      {
        gHomerunResultRender.animationTimer = resultAnimationTimer;
      }
      if (((u32) ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time) < ((u32) ((u16) resultAnimationTimer)))
      {
        gHomerunResultRender.animationTimer = 0U;
        gHomerunResultRender.animationFrame = (u16) (gHomerunResultRender.animationFrame + 1);
        resultFrameTime = ((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].time;
        if (resultFrameTime == 0)
        {
          gHomerunResultRender.animationFrame = (u16) resultFrameTime;
        }
      }
      resultFrameData = GP(((struct AnimationFrame *) resultAnimation)[gHomerunResultRender.animationFrame].oam);
      nextOamIndex += *resultFrameData;
      frameData = resultFrameData + 1;
      if (((s32) firstOamIndex) < ((s32) nextOamIndex))
      {
        positionValue = (s32) ((OamByte *) gOamBuffer);
        objectState = (OamByte *) (&gHomerunResultRender);
        oamXMask = 0x1FF;
        oamAttributeMask = 0xFFFFFE00;
        clearAffineModeMask = -0xD;
        workingOffset = firstOamIndex << 3;
        resultOamBytes = (OamByte *) (workingOffset + positionValue);
        entryCounter = nextOamIndex - firstOamIndex;
        do
        {
          oamAttribute16 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamAttribute16;
          oamDataDestination += 2;
          *((s8 *) (((u8 *) resultOamBytes) + 0)) = (s8) ((*((u8 *) (((u8 *) objectState) + 6))) + oamAttribute16);
          oamCoordinate17 = *(frameData++);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = oamCoordinate17;
          oamDataDestination += 2;
          positionValue = *((s16 *) (((u8 *) objectState) + 4));
          positionValue = oamCoordinate17 + positionValue;
          positionValue &= oamXMask;
          oamAttribute = *((u16 *) (((u8 *) resultOamBytes) + 2));
          preservedOamBits = oamAttributeMask;
          preservedOamBits &= oamAttribute;
          *((u16 *) (((u8 *) resultOamBytes) + 2)) = (u16) (preservedOamBits | positionValue);
          *((u16 *) (((u8 *) oamDataDestination) + 0)) = *(frameData++);
          positionValue = *((u8 *) (((u8 *) resultOamBytes) + 5));
          *((u8 *) (((u8 *) resultOamBytes) + 5)) = (u8) (clearAffineModeMask & positionValue);
          oamDataDestination += 4;
          resultOamBytes += 8;
          entryCounter -= 1;
        }
        while (entryCounter != 0);
      }
    }
  }
  gOamSlotsUsed = nextOamIndex;
}

s32 UpdateHomerunScoreSequence(void)
{
    u8 *sequenceStatePointer;
    s32 sequenceState;

    sequenceStatePointer = &gMinigameSequenceState;
    sequenceState = *sequenceStatePointer;

    if (sequenceState == 1) {
        goto state1;
    }
    if (sequenceState > 1) {
        goto stateGt;
    }
    if (sequenceState == 0) {
        goto state0;
    }
    goto ret0;

stateGt:
    if (sequenceState == 2) {
        goto state2;
    }
    goto ret0;

state0:
    if (MinigameWaitForFrames(60) != 0) {
        gMinigameScore++;
        if ((u16)gMinigameScore > 999) {
            gMinigameScore = 999;
        } else {
            m4aSongNumStart(0x224);
        }

        if (gMinigameScore > gMinigameHighScores) {
            gMinigameNewHighScore = 1;
            gMinigameHighScores = gMinigameScore;
        }
        DrawHomerunDerbyScore();
        goto incState;
    }
    goto ret0;

state1:
    if (MinigameWaitForFrames(20) != 0) {
        if ((gMinigameScore % 3U) == 0) {
            gMedalCount++;
            if ((u16)gMedalCount > 999) {
                gMedalCount = 999;
            } else {
                m4aSongNumStart(0x223);
                gMinigameMedalHighlight = sequenceState;
            }
            DrawHomerunDerbyScore();
        }
incState:
        gMinigameSequenceState++;
    }
    goto ret0;

state2:
    if (MinigameWaitForFrames(30) != 0) {
        gMinigameSequenceTimer = 0;
        *sequenceStatePointer = 0;
        return 1;
    }

ret0:
    return 0;
}

s32 UpdateHomerunBonusAnimation(void)
{
    u8 *sequenceStatePointer;
    u8 *sequenceStatePointerReload;
    struct HomerunBonusState *bonusState;
    s32 yPosition;
    u16 nextYPosition;
    s32 animationIndex;
    s32 sequenceState;
    s32 minimumAnimation;

    sequenceStatePointer = &gMinigameSequenceState;
    sequenceState = *sequenceStatePointer;
    sequenceStatePointerReload = sequenceStatePointer;

    switch (sequenceState) {
    case 0:
    {
        struct HomerunBonusState *movingBonusState;
        s32 movementDirection;
        register struct HomerunBonusState *centeredBonusState PIN("r5");

        movingBonusState = (struct HomerunBonusState *)gHomerunBonusAnimation;
        movementDirection = movingBonusState->direction;
        bonusState = movingBonusState;
        if (movementDirection != 0) {
            bonusState->yPosition++;
            if (bonusState->yPosition > 0x50) {
                bonusState->yPosition = 0x50;
            }
        } else {
            bonusState->yPosition--;
            if (bonusState->yPosition <= 0x4F) {
                bonusState->yPosition = 0x50;
            }
        }

        centeredBonusState = bonusState;
        if (centeredBonusState->yPosition == 0x50) {
            (*sequenceStatePointerReload)++;
            centeredBonusState->animationTimer = 0;
            centeredBonusState->animationFrame = 0;

            animationIndex = centeredBonusState->animationIndex;
            if (animationIndex != 4) {
                if (animationIndex <= 4) {
                    if (animationIndex < (minimumAnimation = 1)) {
                        break;
                    }
                    centeredBonusState->animationIndex = MinigameRandom() % 2 + 7;
                }
            } else {
                bonusState->animationIndex = MinigameRandom() % 2 + 5;
            }
        }
        break;
    }

    case 1:
        break;

    case 2:
    {
        struct HomerunBonusState *returningBonusState;
        register s32 direction PIN("r4");

        returningBonusState = (struct HomerunBonusState *)gHomerunBonusAnimation;
        direction = returningBonusState->direction;
        if (direction != 0) {
            register u16 *timerOutput PIN("r1");

            nextYPosition = returningBonusState->yPosition + 1;
            direction = 0;
            returningBonusState->yPosition = nextYPosition;
            yPosition = returningBonusState->yPosition;
            if (yPosition > sHomerunBonusYLimits[returningBonusState->direction - 1]) {
                timerOutput = &gMinigameSequenceTimer;
                *timerOutput = 0;
                *sequenceStatePointer = direction;
                returningBonusState->animationIndex = direction;
                return 1;
            }
        } else {
            register u8 zeroValue PIN("r6");

            nextYPosition = returningBonusState->yPosition - 1;
            zeroValue = 0;
            returningBonusState->yPosition = nextYPosition;
            yPosition = returningBonusState->yPosition;
            if (yPosition < sHomerunBonusYLimits[returningBonusState->direction + 1]) {
                gMinigameSequenceTimer = direction;
                *sequenceStatePointer = zeroValue;
                returningBonusState->animationIndex = zeroValue;
                return 1;
            }
        }
        break;
    }
    }

    return 0;
}

void StartHomerunBonusAnimation(void)
{
    struct HomerunBonusState *bonusState;
    s32 evenRandomBase;
    s32 direction;
    u32 randomValue;
    u8 cycleIndex;
    s32 animationIndex;

    randomValue = MinigameRandom();
    bonusState = (struct HomerunBonusState *)gHomerunBonusAnimation;
    evenRandomBase = randomValue >> 0x1F;
    evenRandomBase = (randomValue) - (-(evenRandomBase));
    evenRandomBase = evenRandomBase >> 1;
    evenRandomBase <<= 1;
    direction = randomValue - evenRandomBase;
    bonusState->direction = direction;
    bonusState->xPosition = 0x78;
    bonusState->yPosition = sHomerunBonusYLimits[bonusState->direction];
    cycleIndex = (u8)(gHomerunBonusCycle % 3U);
    switch (cycleIndex) {
    case 1:
        animationIndex = 4;
        bonusState->animationIndex = animationIndex;
        break;
    case 2:
        animationIndex = 9;
        bonusState->animationIndex = animationIndex;
        break;
    default:
        animationIndex = direction + 1;
        bonusState->animationIndex = animationIndex;
        if ((MinigameRandom() % 3) == 0) {
            animationIndex = 3;
            bonusState->animationIndex = animationIndex;
        }
        break;
    }
    {
        struct HomerunBonusState *resetBonusState;
        s32 zeroValue;

        resetBonusState = (struct HomerunBonusState *)gHomerunBonusAnimation;
        zeroValue = 0;
        resetBonusState->animationTimer = zeroValue;
        resetBonusState->animationFrame = zeroValue;
        zeroValue = resetBonusState->animationIndex;
        resetBonusState->nextAnimationIndex = zeroValue;
    }
    gHomerunBonusCycle++;
    m4aSongNumStart(0x2CB);
}

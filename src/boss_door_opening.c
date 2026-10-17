#include "boss_door_opening.h"
#include "autosave.h"
#include "fixed_point.h"
#include "init_helpers.h"
#include "main.h"
#include "minigame.h"
#include "score.h"
#include "stage_select.h"
#include "voice_set.h"

extern const u8 sUnk_86A9E48[];
extern const u8 sUnk_86AA048[];
extern const u8 sUnk_86AE048[];
extern const u8 sUnk_86AE248[];
extern const u8 sUnk_86B0E48[];
extern const u8 sUnk_86B1648[];
extern const u8 sUnk_86B1E48[];
extern const u8 sUnk_86B2648[];
extern const u8 sUnk_86B2708[];
extern const u16 sUnk_863C5DC[];
extern const s32 sUnk_863C674[2][4];
extern const u8 sUnk_86AE648[];
s32 UpdateStageEntryHorizontalShake(void);
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C654[];
extern const struct AnimationFrame sUnk_8642464[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C5EC[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C5F4[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C634[];

s32 UpdateBossDoorOpeningState(void)
{
    u32 work;
    u32 value;
    s32 timer;
    s32* timerAlias;
    u32 state;
    u8 passage;
    u8* statePtr;
    u16* timerPtr;

    work = (u32)&gUnk_3004731;
    value = *(u8*)work;
    state = value;
    statePtr = (u8*)work;

    switch (state) {
        case 0:
            if (ExpandMinigameWindow() == 0) {
                goto done;
            }
            if (gUnk_3003C56 != 0) {
                gUnk_3004731 = 5;
                goto done;
            }

            passage = gUnk_3004730;
            if (passage == 1) {
                goto setPassageState;
            }
            if (passage == 2) {
                goto setState3;
            }
            gUnk_3004731 = 2;
            goto done;

        setPassageState:
            gUnk_3004731 = passage;
            goto done;

        case 1:
            if (UpdateBossDoorOpeningSequence() != 0) {
            setState3:
                gUnk_3004731 = 3;
            }
            goto done;

        case 2:
            timerPtr = &gStageEntrySequenceTimer;
            work = *timerPtr;
            timer = work + 1;
            *timerPtr = timer;
            work = gButtonsPressed;
            value = 1;
            value &= work;
            if (value != 0) {
                goto skipOpening;
            }
            work = (u16)timer;
            if (work > 100) {
                work = 8;
                goto setStateAndTimer;
            }
            goto done;

        case 3:
            timerPtr = &gStageEntrySequenceTimer;
            work = *timerPtr;
            timer = work + 1;
            *timerPtr = timer;
            work = gButtonsPressed;
            value = 1;
            value &= work;
            if (value != 0) {
            skipOpening:
                m4aSongNumStart(0x126);
                gUnk_3004731 = 8;
                *timerPtr = 0;
                goto done;
            }
            work = (u16)timer;
            if (work > 30) {
                work = 4;
                goto setStateAndTimer;
            }
            goto done;

        case 4:
            gStageEntryMainSpriteState.second -= 20;
            gStageEntryMainSpriteState.first -= 4;
            timerPtr = &gStageEntrySequenceTimer;
            work = *timerPtr;
            timerAlias = &timer;
            timer = work + 1;
            *timerPtr = *timerAlias;
            work = gButtonsPressed;
            value = 1;
            value &= work;
            if (value != 0) {
                m4aSongNumStart(0x126);
                gUnk_3004731 = 8;
                *timerPtr = 0;
                goto done;
            }
            work = (u16)timer;
            if (90 < work) {
                work = 8;
            setStateAndTimer:
                *statePtr = work;
                *timerPtr = value;
            }
            goto done;

        case 8:
            if ((u8)(gUnk_3004730 - 1) <= 1) {
                gStageEntryMainSpriteState.second -= 20;
                gStageEntryMainSpriteState.first -= 4;
            }
            if (ContractMinigameWindow() != 0) {
                return 1;
            }
            goto done;

        case 5:
            gUnk_300476C++;
            if (gUnk_300476C > 80) {
                gUnk_300476C = 80;
                (*statePtr)++;
            }
            goto done;

        case 6:
            gUnk_300476A -= 5;
            if ((u16)gUnk_300476A <= 255) {
                u32 collectionBase;

                m4aSongNumStart(0x1A8);
                gUnk_3004758.state = 1;
                gUnk_300476A = 0x100;
                gUnk_3004731++;
                state = (u32)gCurrentCollection;
                timer = state;
                collectionBase = timer;
                work = (u32)&gCurrentPassage;
                value = *(u8*)work;
                work = value << 1;
                work += value;
                work <<= 3;
                work += collectionBase;
                work = *(u8*)(work + 16);
                work <<= 28;
                if ((s32)work < 0) {
                    gUnk_3004760.state = 1;
                }
            }
            goto done;

        case 7: {
            s32 tailTimer;
            u16* tailTimerPtr;

            tailTimerPtr = &gStageEntrySequenceTimer;
            tailTimer = *tailTimerPtr + 1;
            *tailTimerPtr = tailTimer;
            if ((u16)tailTimer > 120) {
                *statePtr = 8;
                *tailTimerPtr = 0;
            }
        }
            goto done;
    }

done:
    return 0;
}

void InitializeBossDoorOpeningScene(void)
{
    const volatile u8* passage;
    const u8* backgroundPalette;
    const u8* new_var3;
    register const u8* passagePalette PIN("r3");
    const u8* new_var;
    const u8* backgroundGfx;
    int new_var4;
    int tempPtr;
    const u8* tilemapGfx;
    const u8* screenC000Gfx;
    const u8* screenD000Gfx;
    const u8* screenE000Gfx;
    volatile u16* display;
    volatile u32* new_var2;

    InitializeVideoMemory();
    gStageEntryMainSpriteState.second = 0x2000;
    gStageEntryMainSpriteState.width = 120;
    gStageEntryMainSpriteState.first = 2400;
    gStageEntryMainSpriteState.x = 0;
    gStageEntryMainSpriteState.y = 0;

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)sUnk_86AE048;
        dma[1] = 0x05000000;
        dma[2] = 0x80000100;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        new_var = &gCurrentPassage;
        dma[0] = (u32)(sUnk_86B2648 + (gCurrentPassage * 32));
        new_var2 = dma;
        dma[1] = 0x05000160;
        dma[2] = 0x80000010;
        dmaStatus = new_var2[2];
        dmaStatus = new_var2[2];
        dmaMask = 0x80000000;
        new_var3 = new_var;
        passage = new_var3;
        backgroundPalette = sUnk_86A9E48;
        passagePalette = sUnk_86B2708;
        backgroundGfx = sUnk_86AE248;
        tilemapGfx = sUnk_86AA048;
        screenC000Gfx = sUnk_86B1E48;
        screenD000Gfx = sUnk_86B0E48;
        screenE000Gfx = sUnk_86B1648;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = new_var2[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    tempPtr = (u32)passagePalette;
    *(volatile u16*)0x05000000 = 0;

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)backgroundPalette;
        dma[1] = 0x05000200;
        dma[2] = 0x80000100;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = ((*passage) * 64) + tempPtr;
        dma[1] = 0x05000360;
        dma[2] = 0x80000020;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)backgroundGfx;
        dma[1] = 0x06000000;
        dma[2] = 0x80001600;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)tilemapGfx;
        dma[1] = 0x06010000;
        dma[2] = 0x80002000;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)screenC000Gfx;
        dma[1] = 0x0600C000;
        dma[2] = 0x80000400;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)screenD000Gfx;
        dma[1] = 0x0600D000;
        dma[2] = 0x80000400;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    {
        volatile u32* dma;
        u32 dmaStatus;
        u32 dmaMask;

        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)screenE000Gfx;
        dma[1] = 0x0600E000;
        dma[2] = 0x80000400;
        dmaStatus = dma[2];
        dmaStatus = dma[2];
        dmaMask = 0x80000000;
        if (((s32)dmaStatus) < 0) {
            do {
                dmaStatus = dma[2] & dmaMask;
            } while (dmaStatus != 0);
        }
    }

    display = (volatile u16*)0x04000008;
    *display = 0x5800;
    display++;
    *display = 0x5A01;
    display++;
    *display = 0x5C02;
    InitMinigameScoreDisplay();
    *(volatile u16*)0x04000054 = 0;
    *(volatile u16*)0x04000000 = 0x1700;
    gUnk_3004760.state = 0;
    gUnk_3004760.animationTimer = 0;
    gUnk_3004760.frame = 0;
    gStageEntryWindowLeft = 120;
    gStageEntryWindowRight = 120;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 160;
    *(volatile u16*)0x04000000 |= 0x2000;
    display = (volatile u16*)0x04000048;
    *display = 63;
    *(volatile u16*)0x0400004A = 0;
    display += 4;
    *display = 0x1641;
    display++;
    *display = 0x0808;
    gStageEntryShakeMode = 0;
    new_var4 = 2;
    gStageEntrySequenceTimer = 0;
    gStageEntrySequenceStep = 0;
    gUnk_3004731 = 0;
    gUnk_3004768 = 0x900;
    gStageEntryHorizontalScroll.vertical = 0;
    ((volatile u16*)0x04000012)[0] = 0;
    ((volatile u16*)0x04000012)[-1] = 0;
    ((volatile u16*)0x04000012)[2] = 0;
    ((volatile u16*)0x04000012)[1] = 0;
    ((volatile u16*)0x04000012)[4] = 0;
    ((volatile u16*)0x04000012)[3] = 0;
}

void PrepareBossDoorOpeningResult(void)
{
    s32 i;

    if (gCurrentPassage == PASSAGE_ENTRY || gCurrentPassage == PASSAGE_GOLDEN) {
        gUnk_3003C4A = 1;
    } else {
        gUnk_3003C4A = 0;
    }

    for (i = 0; i <= 3; i++) {
        if (gCurrentCollection[gCurrentPassage][i].jewelPieceNE &&
            gCurrentCollection[gCurrentPassage][i].jewelPieceSE &&
            gCurrentCollection[gCurrentPassage][i].jewelPieceSW &&
            gCurrentCollection[gCurrentPassage][i].jewelPieceNW) {
            if (gCurrentCollection[gCurrentPassage][STAGE_BOSS].BOSS_DEFEATED) {
                gUnk_3004738[i].state = 2;
            } else {
                gUnk_3004738[i].state = 1;
            }
        } else {
            gUnk_3004738[i].state = 0;
        }
        gUnk_3004738[i].animationTimer = 0;
        gUnk_3004738[i].frame = 0;
    }

    if (gCurrentCollection[gCurrentPassage][STAGE_BOSS].BOSS_DEFEATED) {
        gUnk_3004730 = 3;
        gStageEntryExitRequested = 1;
    } else if (gCurrentCollection[gCurrentPassage][STAGE_BOSS].SHOW_BOSS_DOOR_OPENING) {
        gUnk_3004730 = 1;
        gCurrentCollection[gCurrentPassage][STAGE_BOSS].SHOW_BOSS_DOOR_OPENING = 0;
        gStageEntryExitRequested = 0;
        AutosaveDefeat();
    } else if (
        (gUnk_3004738[0].state != 0 && gUnk_3004738[1].state != 0 && gUnk_3004738[2].state != 0 &&
         gUnk_3004738[3].state != 0) ||
        (gUnk_3003C4A != 0 && gUnk_3004738[0].state != 0)
    ) {
        gUnk_3004730 = 2;
        gStageEntryExitRequested = 0;
    } else {
        gUnk_3004730 = 0;
        gStageEntryExitRequested = 1;
    }

    InitializeBossDoorOpeningScene();
    m4aSongNumStart(sUnk_863C5DC[gUnk_3004730 * 2]);
}

void PrepareBossDoorOpeningJewelDisplay(void)
{
    s32 i;

    if (gCurrentPassage == PASSAGE_ENTRY || gCurrentPassage == PASSAGE_GOLDEN) {
        gUnk_3003C4A = 1;
    } else {
        gUnk_3003C4A = 0;
    }

    for (i = 0; i <= 3; i++) {
        if (gCurrentCollection[gCurrentPassage][i].jewelPieceNE &&
            gCurrentCollection[gCurrentPassage][i].jewelPieceSE &&
            gCurrentCollection[gCurrentPassage][i].jewelPieceSW &&
            gCurrentCollection[gCurrentPassage][i].jewelPieceNW) {
            gUnk_3004738[i].state = 1;
        } else {
            gUnk_3004738[i].state = 0;
        }
        gUnk_3004738[i].animationTimer = 0;
        gUnk_3004738[i].frame = 0;
    }

    gUnk_300476A = 0x200;
    gUnk_300476C = sUnk_863C674[gUnk_3003C4A][gCurrentStageNumber];
    gUnk_3004758.animationTimer = 0;
    gUnk_3004758.frame = 0;
    gUnk_3004758.state = 0;
    gUnk_3004730 = 0;
    InitializeBossDoorOpeningScene();
    m4aSongNumStart(0x1A9);
}

void UpdateBossDoorOpeningDisplay(void)
{
    s32 offset;
    volatile u32* dma;
    s32 timer;
    u32 mask;

    offset = UpdateStageEntryHorizontalShake();
    *(volatile u16*)0x04000040 = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
    *(volatile u16*)0x04000044 = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;
    *(volatile u16*)0x04000016 = gStageEntryHorizontalScroll.vertical + offset;
    *(volatile u16*)0x0400001A = gStageEntryHorizontalScroll.vertical + offset;

    timer = gMainTimer;
    mask = 31;
    timer &= mask;
    if (timer == 0) {
        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)sUnk_86AE248;
        dma[1] = 0x06000000;
        dma[2] = 0x80000200;
        dma[2];
    }

    timer = gMainTimer;
    timer &= mask;
    if (timer == 16) {
        dma = (volatile u32*)0x040000D4;
        dma[0] = (u32)sUnk_86AE648;
        dma[1] = 0x06000000;
        dma[2] = 0x80000200;
        dma[2];
    }
}

s32 UpdateBossDoorOpeningSequence(void)
{
    s32 timer;

    switch (gStageEntrySequenceStep) {
        case 0:
            timer = gStageEntrySequenceTimer + 1;
            gStageEntrySequenceTimer = timer;
            if ((u16)timer > 30) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep++;
                gStageEntryShakeMode = 2;
            }
            break;

        case 1:
            timer = gStageEntrySequenceTimer + 1;
            gStageEntrySequenceTimer = timer;
            if ((u16)timer > 70) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep++;
                gStageEntryShakeMode = 4;
                m4aSongNumStart(0x1DC);
                VoiceSetPlay(10);
            }
            break;

        case 2:
            gUnk_3004768 -= 6;
            if (gUnk_3004768 <= 255) {
                gUnk_3004768 = 0x100;
                gStageEntrySequenceStep++;
            }
            break;

        case 3:
            timer = gStageEntrySequenceTimer + 1;
            gStageEntrySequenceTimer = timer;
            if ((u16)timer > 30) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                gStageEntryShakeMode = 1;
                return 1;
            }
            break;
    }

    return 0;
}

void UpdateBossDoorOpeningSprites(void)
{
    register s32 oldSlot PIN("r10");
    s32 slot;
    u16* rawDestination;
    s32 i;
    s32 limit;
    s32 inactiveTableOffset;
    s32 activeTableOffset;
    s32 iconOffset;
    const u16* source;
    OamData* oam;
    struct BossDoorOpeningIcon* largeIcon;
    struct BossDoorOpeningBackground* background;
    register u32 r8Work PIN("r8");
    const struct AnimationFrame* inactiveAnimation;
    u16 attr;
    s16 affine[4];
    s16* affineBPtr;
    s16* affineCPtr;
    s16* affineDPtr;

    

    oldSlot = 0;
    slot = gOamSlotsUsed;
    {
        u32 initialOffset;
        register OamData* initialBuffer PIN("r0");
        initialOffset = (u32)slot << 3;
        initialBuffer = gOamBuffer;
        rawDestination = (u16*)(initialOffset + (u32)initialBuffer);
    }

    if (gUnk_3003C56 != 0) {
        {
            struct BossDoorOpeningIcon* iconAddress;
            u8 iconState;
            iconAddress = &gUnk_3004758;
            iconState = iconAddress->state;
            largeIcon = iconAddress;
            if (iconState != 0) {
                const u8* tableBase;
                const u8* stageAddress;
                u32 tableIndex;
                register const u8* modeAddress PIN("r3");
                register u32 modeIndex PIN("r1");
                register const struct AnimationFrame* largeAnimation PIN("r3");
                u32 timer;

                tableBase = (const u8*)sUnk_863C654;
                stageAddress = &gCurrentStageNumber;
                tableIndex = *stageAddress;
                tableIndex <<= 2;
                modeAddress = &gUnk_3003C4A;
                modeIndex = *modeAddress;
                modeIndex <<= 4;
                tableIndex += modeIndex;
                tableIndex += (u32)tableBase;
                largeAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)tableIndex);
                timer = largeIcon->animationTimer;
                timer++;
                largeIcon->animationTimer = timer;
                if (largeAnimation[largeIcon->frame].time < (u16)timer) {
                    largeIcon->animationTimer = 1;
                    largeIcon->frame++;
                    if (largeAnimation[largeIcon->frame].time == 0) {
                        largeIcon->animationTimer = oldSlot;
                        largeIcon->frame = oldSlot;
                        largeIcon->state = 0;
                    }
                }
                if (largeIcon->state != 0) {
                    source = (GP(largeAnimation[largeIcon->frame].oam));
                    slot += *source++;
                    if (slot > MAX_OAM_SLOTS) {
                        return;
                    }
                    if (oldSlot < slot) {
                        OamData* largeBuffer;
                        u32 xMask;
                        s32 priorityMask;
                        largeBuffer = gOamBuffer;
                        xMask = 0x1FF;
                        r8Work = 0xFFFFFE00;
                        priorityMask = -13;
                        oam = (OamData*)(((u32)oldSlot << 3) + (u32)largeBuffer);
                        oldSlot = slot - oldSlot;
                        do {
                            attr = *source++;
                            *rawDestination++ = attr;
                            oam->split.y = attr + 0x50;
                            attr = *source++;
                            *rawDestination++ = attr;
                            {
                                u32 newX;
                                register u32 oldX PIN("r2");
                                register u32 mergedX PIN("r0");
                                newX = (attr + 0x78) & xMask;
                                oldX = oam->all.attr1;
                                mergedX = r8Work;
                                mergedX &= oldX;
                                mergedX |= newX;
                                oam->all.attr1 = mergedX;
                            }
                            {
                                u16 tileAttr;
                                tileAttr = *source;
                                *rawDestination = tileAttr;
                                source++;
                            }
                            ((u8*)oam)[5] = (((u8*)oam)[5] & priorityMask) | 8;
                            rawDestination += 2;
                            oam++;
                            oldSlot--;
                        } while (oldSlot != 0);
                        oldSlot = slot;
                    }
                }
            }
        }

        {
            register const u8* doorModeAddress PIN("r3");
            u32 doorMode;
            register s32 limitValue PIN("r1");
            register s32 one PIN("r2");
            doorModeAddress = &gUnk_3003C4A;
            doorMode = *doorModeAddress;
            limitValue = 4;
            limit = limitValue;
            if (doorMode != 0) {
                one = 1;
                limit = one;
            }
        }
        {
            register s32 zero PIN("r3");
            zero = 0;
            i = zero;
            if (zero < limit) {
                {
                    register s32 activeOffsetZero PIN("r1");
                    register s32 iconOffsetZero PIN("r2");
                    activeOffsetZero = 0;
                    
                    activeTableOffset = activeOffsetZero;
                    iconOffsetZero = 0;
                    
                    iconOffset = iconOffsetZero;
                }
                do {
                    const u8* firstStageAddress;
                    u32 firstStage;
                    register s32 firstLoopIndex PIN("r3");
                    const u8* firstPassageAddress;
                    u32 firstPassage;
                    const u8* savedPassageAddress;

                    firstStageAddress = &gCurrentStageNumber;
                    firstStage = *firstStageAddress;
                    firstLoopIndex = i;
                    if (firstStage != firstLoopIndex) {
                        goto active_inactive_render;
                    }
                    firstPassageAddress = &gUnk_3004731;
                    firstPassage = *firstPassageAddress;
                    savedPassageAddress = firstPassageAddress;
                    if (firstPassage <= 6) {
                        if (firstPassage <= 4) {
                            goto active_recheck;
                        }
                        {
                            u32 activeState;
                            {
                                register struct BossDoorOpeningIcon* activeIcon PIN("r1");
                                register u8* iconBase PIN("r0");
                                register s32 activeIconOffset PIN("r2");
                                iconBase = (u8*)gUnk_3004738;
                                activeIconOffset = iconOffset;
                                
                                activeIcon = (struct BossDoorOpeningIcon*)((u32)activeIconOffset + (u32)iconBase);
                                activeState = activeIcon->state;
                            }
                            if (activeState != 0) {
                                u32 tableIndex;
                                const u8* modeAddress;
                                const u8* tableBase;
                                register const struct AnimationFrame* activeAnimation PIN("r3");
                                tableIndex = activeState;
                                
                                tableIndex--;
                                tableIndex <<= 4;
                                {
                                    s32 activeOffsetValue;
                                    activeOffsetValue = activeTableOffset;
                                    tableIndex = activeOffsetValue - (-tableIndex);
                                }
                                modeAddress = &gUnk_3003C4A;
                                tableIndex += (u32)(*modeAddress) << 5;
                                tableBase = (const u8*)sUnk_863C5F4;
                                tableIndex += (u32)tableBase;
                                activeAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)tableIndex);
                                source = GP(activeAnimation[0].oam);
                                slot += *source++;
                                {
                                    register s32 activeSlotCheck PIN("r1");
                                    activeSlotCheck = slot;
                                    
                                    if (activeSlotCheck > MAX_OAM_SLOTS) {
                                        return;
                                    }
                                }
                                if (oldSlot < slot) {
                                    register s32 matrixInit PIN("r2");
                                    s32 matrixMask;
                                    const u8* verticalAddress;
                                    s32 previousSlot;
                                    register OamData* jewelBuffer PIN("r2");
                                    register u32 xMask PIN("r6");
                                    register s32 priorityInit PIN("r0");
                                    s32 priorityMask;
                                    register s32 remaining PIN("r2");
                                    s32 loopRemaining;
                                    matrixInit = 15;
                                    
                                    matrixInit = -matrixInit;
                                    matrixMask = matrixInit;
                                    verticalAddress = (const u8*)&gUnk_300476C;
                                    r8Work = (u32)verticalAddress;
                                    previousSlot = oldSlot;
                                    {
                                        u32 oamOffset;
                                        oamOffset = (u32)previousSlot << 3;
                                        jewelBuffer = gOamBuffer;
                                        oam = (OamData*)(oamOffset + (u32)jewelBuffer);
                                    }
                                    xMask = 0x1FF;
                                    priorityInit = -13;
                                    
                                    priorityMask = priorityInit;
                                    remaining = slot;
                                    
                                    remaining -= previousSlot;
                                    
                                    oldSlot = remaining;
                                    do {
                                        attr = *source++;
                                        *rawDestination++ = attr;
                                        {
                                            register const u8* vertical PIN("r1");
                                            u32 y;
                                            vertical = (const u8*)r8Work;
                                            y = *vertical;
                                            y += attr;
                                            y -= 0x10;
                                            oam->split.y = y;
                                        }
                                        ((u8*)oam)[1] |= 3;
                                        attr = *source++;
                                        *rawDestination++ = attr;
                                        {
                                            u32 newX;
                                            register u32 oldX PIN("r2");
                                            u32 mergedX;
                                            newX = (attr + 0x68) & xMask;
                                            oldX = oam->all.attr1;
                                            mergedX = oldX & 0xFFFFFE00;
                                            mergedX |= newX;
                                            oam->all.attr1 = mergedX;
                                        }
                                        {
                                            register u32 oldMatrix PIN("r1");
                                            u32 maskedMatrix;
                                            oldMatrix = ((u8*)oam)[3];
                                            maskedMatrix = matrixMask;
                                            maskedMatrix &= oldMatrix;
                                            ((u8*)oam)[3] = maskedMatrix;
                                        }
                                        {
                                            u16 tileAttr;
                                            tileAttr = *source;
                                            *rawDestination = tileAttr;
                                            source++;
                                        }
                                        {
                                            u32 priority;
                                            priority = ((u8*)oam)[5];
                                            priority &= priorityMask;
                                            ((u8*)oam)[5] = priority;
                                        }
                                        rawDestination += 2;
                                        oam++;
                                        {
                                            register s32 decrement PIN("r2");
                                            decrement = 1;
                                            decrement = -decrement;
                                            
                                            oldSlot += decrement;
                                            loopRemaining = oldSlot;
                                            
                                        }
                                    } while (loopRemaining != 0);
                                    oldSlot = slot;
                                }
                            }
                        }

                        {
                            register const s16* activeSinTable PIN("r5");
                            s32 cosineValue;
                            const s16* cosineAddress;
                            s32 cosineIndex;
                            s32 loadedCosine;
                            register s32 result PIN("r0");
                            register s16* activeBPtr PIN("r6");
                            register s16* activeCPtr PIN("r4");

                            activeSinTable = sSinCosTable;
                            cosineAddress = activeSinTable + 0x40;
                            cosineIndex = 0;
                            loadedCosine = *(const s16*)((const u8*)cosineAddress + cosineIndex);
                            cosineValue = loadedCosine;
                            {
                                const s16* scaleAddress;
                                s32 scale;
                                s32 right;
                                scaleAddress = &gUnk_300476A;
                                scale = scaleAddress[0];
                                right = FixedInverse(scale);
                                right <<= 16;
                                right >>= 16;
                                result = FixedMul(cosineValue, right);
                            }
                            affine[0] = result;
                            {
                                s32 sineValue;
                                s32 sineIndex;
                                const s16* scaleAddress;
                                s32 scale;
                                s32 right;
                                sineIndex = 0;
                                sineValue = activeSinTable[sineIndex];
                                scaleAddress = &gUnk_300476A;
                                scale = scaleAddress[0];
                                right = FixedInverse(scale);
                                right <<= 16;
                                right >>= 16;
                                result = FixedMul(sineValue, right);
                                activeBPtr = affine;
                                activeBPtr++;
                                *activeBPtr = result;
                            }
                            {
                                register s32 sineValue PIN("r4");
                                const s16* scaleAddress;
                                s32 scale;
                                s32 right;
                                sineValue = ((const u16*)activeSinTable)[0];
                                sineValue = -sineValue;
                                sineValue = (s16)sineValue;
                                scaleAddress = &gUnk_300476A;
                                scale = scaleAddress[0];
                                right = FixedInverse(scale);
                                right <<= 16;
                                right >>= 16;
                                result = FixedMul(sineValue, right);
                                activeCPtr = affine;
                                activeCPtr += 2;
                                *activeCPtr = result;
                            }
                            {
                                const s16* scaleAddress;
                                s32 scale;
                                s32 right;
                                u32 finalValue;
                                register s16* activeDPtr PIN("r1");
                                u32 firstValue;
                                register OamData* affineBuffer PIN("r3");
                                scaleAddress = &gUnk_300476A;
                                scale = scaleAddress[0];
                                right = FixedInverse(scale);
                                right <<= 16;
                                right >>= 16;
                                result = FixedMul(cosineValue, right);
                                finalValue = (u16)result;
                                activeDPtr = affine;
                                activeDPtr += 3;
                                *activeDPtr = finalValue;
                                firstValue = (u16)affine[0];
                                affineBuffer = gOamBuffer;
                                affineBuffer[0].all.affineParam = firstValue;
                                firstValue = (u16)*activeBPtr;
                                affineBuffer[1].all.affineParam = firstValue;
                                firstValue = (u16)*activeCPtr;
                                affineBuffer[2].all.affineParam = firstValue;
                                affineBuffer[3].all.affineParam = finalValue;
                            }
                        }
                    } else {
                    active_recheck: {
                        register const u8* stageAddress PIN("r1");
                        u32 stage;
                        register s32 loopIndex PIN("r3");
                        u32 passageState;
                        stageAddress = &gCurrentStageNumber;
                        stage = *stageAddress;
                        loopIndex = i;
                        if (stage == loopIndex) {
                            passageState = *savedPassageAddress;
                            if (passageState <= 6) {
                                goto active_next;
                            }
                        }
                    }
                    active_inactive_render: {
                        u8* iconsBase;
                        register s32 currentIconOffset PIN("r1");
                        struct BossDoorOpeningIcon* loopIcon;
                        u32 activeState;
                        u8* savedIconsBase;
                        iconsBase = (u8*)gUnk_3004738;
                        currentIconOffset = iconOffset;
                        
                        loopIcon = (struct BossDoorOpeningIcon*)((u32)currentIconOffset + (u32)iconsBase);
                        activeState = loopIcon->state;
                        savedIconsBase = iconsBase;
                        if (activeState == 1) {
                            const u8* firstTableBase;
                            const u8* modeAddress;
                            u32 modeOffset;
                            s32 tableOffset;
                            const struct AnimationFrame* firstAnimation;
                            u32 timer;
                            firstTableBase = (const u8*)sUnk_863C634;
                            modeAddress = &gUnk_3003C4A;
                            modeOffset = *modeAddress;
                            modeOffset <<= 4;
                            tableOffset = activeTableOffset;
                            modeOffset = (tableOffset) - (-(modeOffset));
                            modeOffset += (u32)firstTableBase;
                            firstAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)modeOffset);
                            timer = loopIcon->animationTimer;
                            timer++;
                            loopIcon->animationTimer = timer;
                            if (firstAnimation[loopIcon->frame].time < (u16)timer) {
                                loopIcon->animationTimer = activeState;
                                loopIcon->frame++;
                                if (firstAnimation[loopIcon->frame].time == 0) {
                                    loopIcon->animationTimer = 0;
                                    loopIcon->frame = 0;
                                }
                            }
                            {
                                u32 frameIconAddress;
                                frameIconAddress = iconOffset;
                                frameIconAddress += (u32)savedIconsBase;
                                source = (GP(firstAnimation[((struct BossDoorOpeningIcon*)frameIconAddress)->frame].oam));
                                slot += *source++;
                                if (slot > MAX_OAM_SLOTS) {
                                    return;
                                }
                                if (oldSlot < slot) {
                                    register u32 xMask;
                                    s32 clearMask;
                                    register s32 priorityMask;
                                    s32 previousSlot;
                                    u32 oamOffset;
                                    register OamData* centerBuffer PIN("r1");
                                    OamData* centerOam;
                                    s32 remaining;
                                    register s32 loopRemaining PIN("r2");
                                    xMask = 0x1FF;
                                    clearMask = 0xFFFFFE00;
                                    r8Work = clearMask;
                                    priorityMask = -13;
                                    previousSlot = oldSlot;
                                    oamOffset = (u32)previousSlot << 3;
                                    centerBuffer = gOamBuffer;
                                    centerOam = (OamData*)((u32)oamOffset + (u32)centerBuffer);
                                    oam = centerOam;
                                    remaining = slot;
                                    remaining -= previousSlot;
                                    oldSlot = remaining;
                                center_first_loop: {
                                    attr = *source++;
                                    *rawDestination++ = attr;
                                    oam->split.y = attr + 0x50;
                                    attr = *source++;
                                    *rawDestination++ = attr;
                                    {
                                        u32 newX;
                                        register u32 oldX PIN("r2");
                                        register u32 mergedX PIN("r0");
                                        newX = (attr + 0x78) & xMask;
                                        oldX = oam->all.attr1;
                                        mergedX = r8Work;
                                        mergedX &= oldX;
                                        mergedX |= newX;
                                        oam->all.attr1 = mergedX;
                                    }
                                    {
                                        u16 tileAttr;
                                        tileAttr = *source;
                                        *rawDestination = tileAttr;
                                        source++;
                                    }
                                    {
                                        u32 oldPriority;
                                        register u32 newPriority PIN("r0");
                                        oldPriority = ((u8*)oam)[5];
                                        newPriority = priorityMask;
                                        newPriority &= oldPriority;
                                        oldPriority = 8;
                                        newPriority |= oldPriority;
                                        ((u8*)oam)[5] = newPriority;
                                    }
                                    rawDestination += 2;
                                    oam++;
                                    {
                                        register s32 decrement PIN("r1");
                                        decrement = 1;
                                        
                                        decrement = -decrement;
                                        
                                        oldSlot += decrement;
                                        loopRemaining = oldSlot;
                                        
                                    }
                                    if (loopRemaining != 0) {
                                        goto center_first_loop;
                                    }
                                }
                                    oldSlot = slot;
                                }
                            }
                        }
                        {
                            register u32 secondIconAddress PIN("r1");
                            u32 secondState;
                            secondIconAddress = iconOffset;
                            secondIconAddress += (u32)savedIconsBase;
                            secondState = ((struct BossDoorOpeningIcon*)secondIconAddress)->state;
                            if (secondState != 0) {
                                u32 tableIndex;
                                register s32 tableOffset PIN("r3");
                                const u8* modeAddress;
                                u32 modeOffset;
                                const u8* tableBase;
                                register const struct AnimationFrame* secondAnimation PIN("r3");
                                tableIndex = secondState;
                                
                                tableIndex--;
                                tableIndex <<= 4;
                                tableOffset = activeTableOffset;
                                
                                tableIndex = (tableOffset) - (-(tableIndex));
                                modeAddress = &gUnk_3003C4A;
                                modeOffset = *modeAddress;
                                modeOffset <<= 5;
                                tableIndex += modeOffset;
                                tableBase = (const u8*)sUnk_863C5F4;
                                tableIndex += (u32)tableBase;
                                secondAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)tableIndex);
                                source = (GP(secondAnimation[0].oam));
                                slot += *source++;
                                {
                                    s32 slotCheck;
                                    slotCheck = slot;
                                    if (slotCheck > MAX_OAM_SLOTS) {
                                        return;
                                    }
                                }
                                if (oldSlot < slot) {
                                    u32 xMask;
                                    s32 clearMask;
                                    s32 priorityMask;
                                    s32 previousSlot;
                                    u32 oamOffset;
                                    register OamData* centerBuffer PIN("r2");
                                    OamData* centerOam;
                                    s32 remaining;
                                    register s32 loopRemaining PIN("r2");
                                    xMask = 0x1FF;
                                    clearMask = 0xFFFFFE00;
                                    r8Work = clearMask;
                                    priorityMask = -13;
                                    previousSlot = oldSlot;
                                    oamOffset = (u32)previousSlot << 3;
                                    centerBuffer = gOamBuffer;
                                    centerOam = (OamData*)((u32)oamOffset + (u32)centerBuffer);
                                    oam = centerOam;
                                    remaining = slot;
                                    remaining -= previousSlot;
                                    oldSlot = remaining;
                                    do {
                                        attr = *source++;
                                        *rawDestination++ = attr;
                                        oam->split.y = attr + 0x50;
                                        attr = *source++;
                                        *rawDestination++ = attr;
                                        {
                                            u32 newX;
                                            register u32 oldX PIN("r2");
                                            register u32 mergedX PIN("r0");
                                            newX = (attr + 0x78) & xMask;
                                            oldX = oam->all.attr1;
                                            mergedX = r8Work;
                                            mergedX &= oldX;
                                            mergedX |= newX;
                                            oam->all.attr1 = mergedX;
                                        }
                                        {
                                            u16 tileAttr;
                                            tileAttr = *source;
                                            *rawDestination = tileAttr;
                                            source++;
                                        }
                                        {
                                            u32 oldPriority;
                                            register u32 newPriority PIN("r0");
                                            oldPriority = ((u8*)oam)[5];
                                            newPriority = priorityMask;
                                            newPriority &= oldPriority;
                                            oldPriority = 8;
                                            newPriority |= oldPriority;
                                            ((u8*)oam)[5] = newPriority;
                                        }
                                        rawDestination += 2;
                                        oam++;
                                        {
                                            register s32 decrement PIN("r1");
                                            decrement = 1;
                                            
                                            decrement = -decrement;
                                            
                                            oldSlot += decrement;
                                            loopRemaining = oldSlot;
                                            
                                        }
                                    } while (loopRemaining != 0);
                                    oldSlot = slot;
                                }
                            }
                        }
                    }
                    }
                active_next: {
                    register s32 nextTableOffset PIN("r3");
                    nextTableOffset = activeTableOffset;
                    
                    nextTableOffset += 4;
                    activeTableOffset = nextTableOffset;
                }
                    {
                        s32 nextIconOffset;
                        nextIconOffset = iconOffset;
                        
                        nextIconOffset += 8;
                        iconOffset = nextIconOffset;
                    }
                    {
                        register s32 nextIndex PIN("r1");
                        register s32 loopLimitCheck PIN("r2");
                        nextIndex = i;
                        nextIndex++;
                        i = nextIndex;
                        loopLimitCheck = limit;
                        if (nextIndex >= loopLimitCheck) {
                            break;
                        }
                    }
                } while (1);
            }
        }

        {
            const u8* tableBase;
            const u8* modeAddress;
            u32 tableIndex;
            const struct AnimationFrame* finalAnimation;
            struct BossDoorOpeningIcon* iconAddress;
            struct BossDoorOpeningIcon* finalIcon;
            u32 finalState;
            tableBase = (const u8*)sUnk_863C5EC;
            modeAddress = &gUnk_3003C4A;
            tableIndex = *modeAddress;
            tableIndex <<= 2;
            tableIndex += (u32)tableBase;
            finalAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)tableIndex);
            iconAddress = &gUnk_3004760;
            finalState = iconAddress->state;
            finalIcon = iconAddress;
            if (finalState != 0) {
                register u32 timer PIN("r0");
                timer = finalIcon->animationTimer;
                timer++;
                finalIcon->animationTimer = timer;
                if (finalAnimation[finalIcon->frame].time < (u16)timer) {
                    timer = 1;
                    finalIcon->animationTimer = timer;
                    finalIcon->frame++;
                    if (finalAnimation[finalIcon->frame].time == 0) {
                        finalIcon->animationTimer = 0;
                        finalIcon->frame = 0;
                        finalIcon->state = 0;
                    }
                }
            }
            source = GP(finalAnimation[finalIcon->frame].oam);
            slot += *source++;
            {
                s32 slotCheck;
                slotCheck = slot;
                
                if (slotCheck > MAX_OAM_SLOTS) {
                    return;
                }
            }
            if (oldSlot < slot) {
                OamData* finalBuffer;
                u32 xMask;
                s32 clearMask;
                s32 priorityMask;
                register s32 previousSlot PIN("r2");
                finalBuffer = gOamBuffer;
                xMask = 0x1FF;
                clearMask = 0xFFFFFE00;
                r8Work = clearMask;
                priorityMask = -13;
                previousSlot = oldSlot;
                oam = (OamData*)(((u32)previousSlot << 3) + (u32)finalBuffer);
                oldSlot = slot - previousSlot;
                do {
                    attr = *source++;
                    *rawDestination++ = attr;
                    {
                        register const s16* verticalAddress PIN("r0");
                        s32 newY;
                        verticalAddress = &gUnk_3004768;
                        newY = ((s16)*verticalAddress >> 4) + attr;
                        oam->split.y = newY;
                    }
                    attr = *source++;
                    *rawDestination++ = attr;
                    {
                        u32 newX;
                        register u32 oldX PIN("r2");
                        register u32 mergedX PIN("r0");
                        newX = (attr + 0x78) & xMask;
                        oldX = oam->all.attr1;
                        mergedX = r8Work;
                        mergedX &= oldX;
                        mergedX |= newX;
                        oam->all.attr1 = mergedX;
                    }
                    {
                        u16 tileAttr;
                        tileAttr = *source;
                        *rawDestination = tileAttr;
                        source++;
                    }
                    {
                        u32 oldPriority;
                        register u32 newPriority PIN("r0");
                        oldPriority = ((u8*)oam)[5];
                        newPriority = priorityMask;
                        newPriority &= oldPriority;
                        oldPriority = 8;
                        newPriority |= oldPriority;
                        ((u8*)oam)[5] = newPriority;
                    }
                    
                    rawDestination += 2;
                    oam++;
                    {
                        register s32 decrement PIN("r1");
                        s32 remaining;
                        decrement = 1;
                        decrement = -decrement;
                        oldSlot += decrement;
                        remaining = oldSlot;
                    }
                } while (oldSlot != 0);
                oldSlot = slot;
            }
        }
        goto finish;
    }

    {
        u32 inactiveState;
        inactiveState = gUnk_3004731;
        background = &gStageEntryMainSpriteState;
        inactiveAnimation = sUnk_8642464;
        if (inactiveState > 3 && (u8)(gUnk_3004730 - 1) <= 1) {
            register u32 timer PIN("r0");
            u32 frameTime;
            timer = background->x;
            timer++;
            background->x = timer;
            frameTime = inactiveAnimation[background->y].time;
            if (frameTime < (u16)timer) {
                timer = 1;
                background->x = timer;
                background->y++;
                if (inactiveAnimation[background->y].time == 0) {
                    register u32 zero PIN("r3");
                    zero = oldSlot;
                    background->x = zero;
                    background->y = zero;
                }
            }
        }
    }
    source = (GP(inactiveAnimation[background->y].oam));
    slot += *source++;
    if (slot > MAX_OAM_SLOTS) {
        return;
    }
    r8Work = (u32)sSinCosTable;
    {
        register s16* bPointer PIN("r2");
        s16* cPointer;
        register s16* dPointer PIN("r1");
        bPointer = affine;
        bPointer++;
        affineBPtr = bPointer;
        cPointer = affine;
        
        cPointer += 2;
        affineCPtr = cPointer;
        dPointer = &affine[3];
        affineDPtr = dPointer;
    }
    
    if (oldSlot < slot) {
        OamData* inactiveBuffer;
        struct BossDoorOpeningBackground* inactiveBackground;
        register s32 previousSlot PIN("r2");
        u32 oamOffset;
        u32 xMaskValue;
        register u32 xMask PIN("r12");
        s32 remaining;
        inactiveBuffer = gOamBuffer;
        inactiveBackground = background;
        previousSlot = oldSlot;
        oamOffset = (u32)previousSlot << 3;
        oam = (OamData*)(oamOffset + (u32)inactiveBuffer);
        xMaskValue = 0x1FF;
        xMask = xMaskValue;
        remaining = slot;
        remaining -= previousSlot;
        oldSlot = remaining;
        do {
            attr = *source++;
            *rawDestination++ = attr;
            {
                s32 newY;
                newY = inactiveBackground->first;
                newY <<= 16;
                newY >>= 20;
                newY += attr;
                newY -= 0x20;
                oam->split.y = newY;
            }
            oam->split.affineMode = 3;
            attr = *source++;
            *rawDestination++ = attr;
            {
                s32 widthOffset;
                s32 newX;
                register u32 oldX PIN("r2");
                u32 mergedX;
                widthOffset = 8;
                newX = attr + *(s16*)((u8*)inactiveBackground + widthOffset);
                newX -= 0x20;
                newX &= xMask;
                oldX = oam->all.attr1;
                mergedX = oldX & 0xFFFFFE00;
                mergedX |= newX;
                oam->all.attr1 = mergedX;
            }
            {
                u32 oldMatrix;
                s32 matrixMask;
                oldMatrix = ((u8*)oam)[3];
                matrixMask = 15;
                matrixMask = -matrixMask;
                matrixMask &= oldMatrix;
                ((u8*)oam)[3] = matrixMask;
            }
            {
                u16 tileAttr;
                tileAttr = *source;
                *rawDestination = tileAttr;
                source++;
            }
            {
                u32 oldPriority;
                s32 priorityMask;
                oldPriority = ((u8*)oam)[5];
                priorityMask = 13;
                priorityMask = -priorityMask;
                priorityMask &= oldPriority;
                ((u8*)oam)[5] = priorityMask;
            }
            rawDestination += 2;
            oam++;
            {
                s32 decrement;
                s32 remainingCount;
                decrement = 1;
                decrement = -decrement;
                oldSlot += decrement;
                remainingCount = oldSlot;
            }
        } while (oldSlot != 0);
        oldSlot = slot;
    }
    {
        s32 cosine;
        register s32 sine PIN("r4");
        register const s16* sineTable PIN("r3");
        register const u16* unsignedSineTable PIN("r2");
        cosine = ((const s16*)r8Work)[0x40];
        affine[0] = FixedMul(cosine, (s16)FixedInverse(background->second >> 4));
        sineTable = (const s16*)r8Work;
        sine = sineTable[0];
        *affineBPtr = FixedMul(sine, (s16)FixedInverse(background->second >> 4));
        unsignedSineTable = (const u16*)r8Work;
        sine = -unsignedSineTable[0];
        sine = (s16)sine;
        {
            s32 cValue;
            register s16* cStorePointer PIN("r3");
            cValue = FixedMul(sine, (s16)FixedInverse(background->second >> 4));
            cStorePointer = affineCPtr;
            *cStorePointer = cValue;
        }
        {
            s32 dValue;
            OamData* affineBuffer;
            register s16* otherPointer PIN("r3");
            u16 value;
            dValue = FixedMul(cosine, (s16)FixedInverse(background->second >> 4));
            *affineDPtr = dValue;
            affineBuffer = gOamBuffer;
            value = affine[0];
            affineBuffer[0].all.affineParam = value;
            otherPointer = affineBPtr;
            value = *(u16*)otherPointer;
            affineBuffer[1].all.affineParam = value;
            otherPointer = affineCPtr;
            value = *(u16*)otherPointer;
            affineBuffer[2].all.affineParam = value;
            affineBuffer[3].all.affineParam = dValue;
        }
    }
    if (gUnk_3004730 == 2) {
        goto finish;
    }

    {
        const u8* modeAddress;
        u32 modeValue;
        register s32 initialLimit PIN("r3");
        s32 loopIndex;
        register s32 loopLimit PIN("r3");
        modeAddress = &gUnk_3003C4A;
        modeValue = *modeAddress;
        initialLimit = 4;
        limit = initialLimit;
        if (modeValue != 0) {
            limit = 1;
        }
        loopIndex = 0;
        i = loopIndex;
        loopLimit = limit;
        if (loopIndex < loopLimit) {
            register s32 initialOffset PIN("r0");
            struct BossDoorOpeningIcon* currentIcon;
            initialOffset = 0;
            
            inactiveTableOffset = initialOffset;
            currentIcon = gUnk_3004738;
            do {
                u32 firstState;
                firstState = currentIcon->state;
                if (firstState == 1) {
                    const u8* tableBase;
                    const u8* currentModeAddress;
                    u32 tableIndex;
                    register const struct AnimationFrame* firstAnimation PIN("r3");
                    u32 timer;
                    tableBase = (const u8*)sUnk_863C634;
                    currentModeAddress = &gUnk_3003C4A;
                    tableIndex = *currentModeAddress;
                    tableIndex <<= 4;
                    tableIndex = (inactiveTableOffset) - (-(tableIndex));
                    tableIndex += (u32)tableBase;
                    firstAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)tableIndex);
                    timer = currentIcon->animationTimer;
                    timer++;
                    currentIcon->animationTimer = timer;
                    if (firstAnimation[currentIcon->frame].time < (u16)timer) {
                        currentIcon->animationTimer = firstState;
                        currentIcon->frame++;
                        if (firstAnimation[currentIcon->frame].time == 0) {
                            currentIcon->animationTimer = 0;
                            currentIcon->frame = 0;
                        }
                    }
                    source = GP(firstAnimation[currentIcon->frame].oam);
                    slot += *source++;
                    {
                        u32 xMaskInit;
                        register u32 xMask PIN("r12");
                        s32 priorityInit;
                        register s32 priorityMask PIN("r8");
                        s32 slotCheck;
                        register const s16* verticalPosition PIN("r6");
                        s32 previousSlot;
                        u32 oamOffset;
                        register OamData* gateBuffer PIN("r2");
                        s32 remaining;
                        slotCheck = slot;
                        if (slotCheck > MAX_OAM_SLOTS) {
                            return;
                        }
                        if (oldSlot < slot) {
                            xMaskInit = 0x1FF;
                            xMask = xMaskInit;
                            priorityInit = 13;
                            priorityInit = -priorityInit;
                            priorityMask = priorityInit;
                            verticalPosition = &gUnk_3004768;
                            previousSlot = oldSlot;
                            oamOffset = (u32)previousSlot << 3;
                            gateBuffer = gOamBuffer;
                            oam = (OamData*)(oamOffset + (u32)gateBuffer);
                            remaining = slot;
                            remaining -= previousSlot;
                            oldSlot = remaining;
                            do {
                                attr = *source++;
                                *rawDestination++ = attr;
                                {
                                    s32 y;
                                    y = *(const u16*)verticalPosition;
                                    y <<= 16;
                                    y >>= 20;
                                    y += attr;
                                    y -= 0x40;
                                    oam->split.y = y;
                                }
                                attr = *source++;
                                *rawDestination++ = attr;
                                {
                                    u32 newX;
                                    register u32 oldX PIN("r2");
                                    u32 mergedX;
                                    newX = (attr + 0x78) & xMask;
                                    oldX = oam->all.attr1;
                                    mergedX = oldX & 0xFFFFFE00;
                                    mergedX |= newX;
                                    oam->all.attr1 = mergedX;
                                }
                                {
                                    u16 tileAttr;
                                    tileAttr = *source;
                                    *rawDestination = tileAttr;
                                    source++;
                                }
                                {
                                    register u32 oldPriority PIN("r1");
                                    register u32 newPriority PIN("r0");
                                    oldPriority = ((u8*)oam)[5];
                                    newPriority = priorityMask;
                                    newPriority &= oldPriority;
                                    newPriority |= 8;
                                    ((u8*)oam)[5] = newPriority;
                                }
                                rawDestination += 2;
                                oam++;
                                {
                                    s32 decrement;
                                    s32 remainingCount;
                                    decrement = 1;
                                    decrement = -decrement;
                                    oldSlot += decrement;
                                    remainingCount = oldSlot;
                                }
                            } while (oldSlot != 0);
                            oldSlot = slot;
                        }
                    }
                }
                
                {
                    register u32 tableIndex PIN("r0");
                    tableIndex = currentIcon->state;
                    if (tableIndex != 0) {
                        u32 tableOffset;
                        const u8* currentModeAddress;
                        u32 modeOffset;
                        const u8* tableBase;
                        register const struct AnimationFrame* secondAnimation PIN("r3");
                        tableIndex--;
                        tableIndex <<= 4;
                        tableOffset = inactiveTableOffset;
                        tableIndex = tableOffset + tableIndex;
                        currentModeAddress = &gUnk_3003C4A;
                        modeOffset = *currentModeAddress;
                        modeOffset <<= 5;
                        tableIndex += modeOffset;
                        tableBase = (const u8*)sUnk_863C5F4;
                        tableIndex += (u32)tableBase;
                        secondAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)tableIndex);
                        source = GP(secondAnimation[0].oam);
                        slot += *source++;
                        {
                            u32 xMaskInit;
                            register u32 xMask PIN("r12");
                            s32 priorityInit;
                            register s32 priorityMask PIN("r8");
                            register s32 slotCheck PIN("r2");
                            register const s16* verticalPosition PIN("r6");
                            s32 previousSlot;
                            u32 oamOffset;
                            register OamData* gateBuffer PIN("r1");
                            s32 remaining;
                            slotCheck = slot;
                            
                            if (slotCheck > MAX_OAM_SLOTS) {
                                return;
                            }
                            if (oldSlot < slot) {
                                xMaskInit = 0x1FF;
                                xMask = xMaskInit;
                                priorityInit = 13;
                                priorityInit = -priorityInit;
                                priorityMask = priorityInit;
                                verticalPosition = &gUnk_3004768;
                                previousSlot = oldSlot;
                                oamOffset = (u32)previousSlot << 3;
                                gateBuffer = gOamBuffer;
                                oam = (OamData*)(oamOffset + (u32)gateBuffer);
                                remaining = slot;
                                remaining -= previousSlot;
                                oldSlot = remaining;
                                do {
                                    attr = *source++;
                                    *rawDestination++ = attr;
                                    {
                                        s32 y;
                                        y = *(const u16*)verticalPosition;
                                        y <<= 16;
                                        y >>= 20;
                                        y += attr;
                                        y -= 0x40;
                                        oam->split.y = y;
                                    }
                                    attr = *source++;
                                    *rawDestination++ = attr;
                                    {
                                        u32 newX;
                                        register u32 oldX PIN("r2");
                                        u32 mergedX;
                                        newX = (attr + 0x78) & xMask;
                                        oldX = oam->all.attr1;
                                        mergedX = oldX & 0xFFFFFE00;
                                        mergedX |= newX;
                                        oam->all.attr1 = mergedX;
                                    }
                                    {
                                        u16 tileAttr;
                                        tileAttr = *source;
                                        *rawDestination = tileAttr;
                                        source++;
                                    }
                                    {
                                        register u32 oldPriority PIN("r1");
                                        register u32 newPriority PIN("r0");
                                        oldPriority = ((u8*)oam)[5];
                                        newPriority = priorityMask;
                                        newPriority &= oldPriority;
                                        newPriority |= 8;
                                        ((u8*)oam)[5] = newPriority;
                                    }
                                    rawDestination += 2;
                                    oam++;
                                    {
                                        s32 decrement;
                                        s32 remainingCount;
                                        decrement = 1;
                                        decrement = -decrement;
                                        oldSlot += decrement;
                                        remainingCount = oldSlot;
                                    }
                                } while (oldSlot != 0);
                                oldSlot = slot;
                            }
                        }
                    }
                }
                inactiveTableOffset += 4;
                currentIcon++;
                {
                    register s32 nextIndex PIN("r3");
                    s32 loopLimitCheck;
                    nextIndex = i;
                    nextIndex++;
                    i = nextIndex;
                    loopLimitCheck = limit;
                    if (nextIndex >= loopLimitCheck) {
                        break;
                    }
                }
            } while (1);
        }
    }

    {
        const u8* tableBase;
        register const u8* modeAddress PIN("r2");
        u32 tableIndex;
        register const struct AnimationFrame* finalAnimation PIN("r3");
        struct BossDoorOpeningIcon* finalIcon;
        tableBase = (const u8*)sUnk_863C5EC;
        modeAddress = &gUnk_3003C4A;
        tableIndex = *modeAddress;
        tableIndex <<= 2;
        tableIndex += (u32)tableBase;
        finalAnimation = GP(*(GBA_PTR(const struct AnimationFrame*) const *)tableIndex);
        finalIcon = &gUnk_3004760;
        source = (GP(finalAnimation[finalIcon->frame].oam));
        slot += *source++;
        if (slot > MAX_OAM_SLOTS) {
            return;
        }
        if (oldSlot < slot) {
            register u32 finalWork PIN("r1");
            u32 xMaskValue;
            u32 clearMaskValue;
            u32 clearMask;
            register s32 priorityMask PIN("r6");
            register const u16* verticalPosition PIN("r5");
            register s32 previousSlot;
            u32 oamOffset;
            s32 remaining;
            finalWork = (u32)gOamBuffer;
            xMaskValue = 0x1FF;
            r8Work = xMaskValue;
            clearMaskValue = 0xFFFFFE00;
            clearMask = clearMaskValue;
            priorityMask = 13;
            priorityMask = -priorityMask;
            verticalPosition = (const u16*)&gUnk_3004768;
            previousSlot = oldSlot;
            oamOffset = (u32)previousSlot << 3;
            oam = (OamData*)(oamOffset + finalWork);
            remaining = slot;
            remaining -= previousSlot;
            oldSlot = remaining;
            do {
                attr = *source++;
                *rawDestination++ = attr;
                {
                    s32 y;
                    y = *verticalPosition;
                    y <<= 16;
                    y >>= 20;
                    y += attr;
                    oam->split.y = y;
                }
                attr = *source++;
                *rawDestination++ = attr;
                {
                    register u32 oldX PIN("r2");
                    register u32 mergedX;
                    finalWork = attr + 0x78 + (u16)(finalWork * 0x10000u);
                    mergedX = (oam->all.attr1 & clearMask) | (finalWork & r8Work);
                    oam->all.attr1 = mergedX;
                }
                {
                    u16 tileAttr;
                    tileAttr = *source;
                    *rawDestination = tileAttr;
                    source++;
                }
                {
                    register u32 oldPriority PIN("r1");
                    register u32 newPriority;
                    register u32 priorityBit PIN("r1");
                    oldPriority = ((u8*)oam)[5];
                    newPriority = priorityMask;
                    newPriority &= oldPriority;
                    priorityBit = 8;
                    newPriority |= priorityBit;
                    ((u8*)oam)[5] = newPriority;
                }
                rawDestination += 2;
                oam++;
                {
                    register s32 decrement PIN("r0");
                    register s32 remainingCount PIN("r1");
                    decrement = 1;
                    decrement = -decrement;
                    
                    oldSlot += decrement;
                    remainingCount = oldSlot;
                }
            } while (oldSlot != 0);
            oldSlot = slot;
        }
    }
finish: {
    register u8* slotsUsed;
    register u32 finalSlot PIN("r2");
    slotsUsed = &gOamSlotsUsed;
    finalSlot = slot;
    *slotsUsed = finalSlot;
}
    
}

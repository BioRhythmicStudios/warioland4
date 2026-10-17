#include "passage_screen.h"

#include "color_effects.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "init_helpers.h"
#include "minigame.h"
#include "save_file.h"
#include "shop.h"
#include "stage_select.h"

extern u8 gStageEntryStageVariant;
extern u8 gUnk_3003C45;
extern u8 gUnk_3003C4A;
extern u8 gUnk_3003C54;
extern u8 gUnk_3003C55;
extern u8 gUnk_3004700;
extern u8 gUnk_3004728;
extern u8 gUnk_3004729;
extern u8 gStageEntryExitRequested;
extern u8 gStageEntrySelectedStage;
extern u8 gStageEntryMainSpriteFalling;
extern u8 gStageEntrySequenceStep;
extern u16 gSoundRoomMessageTimer;
extern u16 gStageEntryMainSpriteState[];
extern s16 gStageEntryHorizontalScroll;
extern u16 gStageEntryWindowLeft;
extern u16 gStageEntryWindowRight;
extern u16 gStageEntryWindowTop;
extern u16 gStageEntryWindowBottom;
extern u16 gStageEntrySequenceTimer;
extern u16 gMedalCount;
extern s32 gTotalScore;
extern s32 gHighScoreTable[6][4];
extern u8 gCurrentCollection[];
extern struct PassageIconState gUnk_3004708[];
extern GBA_PTR(const struct AnimationFrame*) gUnk_3003C40;

extern const u32 sUnk_863C41C[];
extern const u32 sUnk_863C424[];
extern const u32 sUnk_863C42C[];
extern const u32 sUnk_863C434[];
extern const u32 sUnk_863C494[];
extern const u32 sUnk_863C52C[];
extern const u32 sUnk_863C55C[];
extern const u32 sUnk_863C584[];
extern const u32 sUnk_863C5A4[];
extern const u32 sUnk_863C5AC[];
extern const u32 sUnk_863C5B4[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C4AC[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C4BC[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C4CC[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_863C5D4[];
extern const struct AnimationFrame sUnk_864130C[];
extern const struct AnimationFrame sUnk_864131C[];
extern const struct AnimationFrame sUnk_864132C[];
extern const struct AnimationFrame sUnk_864133C[];
extern const struct AnimationFrame sUnk_86413BC[];
extern const struct AnimationFrame sUnk_86413CC[];
extern GBA_PTR(const u8*) const sFileSelectPassageNameTableLanguage1[];
extern GBA_PTR(const u8*) const sFileSelectPassageNameTableLanguage0[];
extern const u8 sStageEntryPassageOverlayGraphics[];
extern const u8 sUnk_869CE48[];
extern const u8 sUnk_869F448[];
extern const u8 sUnk_869F648[];
extern const u8 sUnk_86A0A48[];
extern const u8 sUnk_86A0E08[];
extern const u8 sUnk_86A11C8[];
extern const u8 sUnk_86A13C8[];
extern const u8 sUnk_86A23C8[];
extern const u8 sUnk_86A25C8[];

void Shop_DrawText(const u8* text, u32 destination, u32 length);
s32 __divsi3(s32 numerator, s32 denominator);

s32 PassageScreenSubroutine(void)
{
    gSoundRoomMessageTimer = 0;
    switch (gUnk_3004700) {
        case 0:
            if (gUnk_3003C45 == 0) {
                if (HandlePassageScreenInput() != 0) {
                    GP_SET(gUnk_3003C40, GP(sUnk_863C5D4[gStageEntryMainSpriteFalling]));
                    return 1;
                }
            } else {
                UpdatePassageStageCursorMovement();
                gUnk_3003C54 = 0;
            }
            GP_SET(gUnk_3003C40, GP(sUnk_863C5D4[gStageEntryMainSpriteFalling]));
            break;
        case 1:
            UpdatePassageStageCursorSound();
            if (UpdatePassageStageCursorArrival() != 0) {
                gUnk_3003C55 = 0;
                gUnk_3004700 = 0;
            }
            break;
        case 2:
            if (UpdatePassageStageSwitch() != 0) {
                gUnk_3004700 = 0;
            }
            break;
        case 3:
            UpdatePassageStageCursorSound();
            if (UpdatePassageScreenExitMovement() != 0) {
                gUnk_3003C55 = 0;
                return 1;
            }
            break;
        case 4:
            if (OpenPassageScreenWindow() != 0) {
                return 1;
            }
            break;
        case 5:
            if (ClosePassageScreenWindow() != 0) {
                gUnk_3004700 = 0;
            }
            break;
    }
    return 0;
}

void InitializePassageScreen(void)
{
    InitializeVideoMemory();
    {
        vu16* ime;
        register u32 zero PIN("r4");
        ime = (vu16*)0x04000208;
        zero = 0;
        *ime = zero;
        *((vu16*)0x04000004) &= 0xFFEF;
        *((vu16*)0x04000200) &= 0xFFFD;
        *ime = 1;
        if ((gCurrentPassage == 0) || (gCurrentPassage == 5)) {
            gUnk_3003C4A = 1;
        } else {
            gUnk_3003C4A = zero;
        }
    }
    {
        u16* mainSpriteState;
        {
            register u8* currentPassagePointer PIN("r4");
            const u32* passagePaletteTable;
            register const u8* blankScoreTiles PIN("r9");
            register const u32* passageGraphicsTable PIN("r6");
            u8* selectedStagePointer;
            register u8* selectedStageIndexPointer PIN("r5");
            register u8* collectionData PIN("r10");
            {
                vu32* dma;
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                dma[0] = (u32)sUnk_86A11C8;
                dma[1] = 0x05000200;
                dma[2] = 0x80000100;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                currentPassagePointer = &gCurrentPassage;
                passagePaletteTable = sUnk_863C494;
                {
                    const u8* blankTilesTemp;
                    blankTilesTemp = sUnk_869CE48;
                    blankScoreTiles = blankTilesTemp;
                }
                passageGraphicsTable = sUnk_863C434;
                selectedStagePointer = &gStageEntrySelectedStage;
                selectedStageIndexPointer = &gUnk_3004728;
                collectionData = gCurrentCollection;
                mainSpriteState = gStageEntryMainSpriteState;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                vu32* dma;
                u32 source;
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                source = *currentPassagePointer;
                source <<= 5;
                source += (u32)sUnk_86A23C8;
                dma[0] = source;
                dma[1] = 0x05000240;
                dma[2] = 0x80000010;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                vu32* dma;
                u32 source;
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                source = *currentPassagePointer;
                source <<= 2;
                source += (u32)passagePaletteTable;
                source = *((u32*)source);
                dma[0] = source;
                dma[1] = 0x05000000;
                dma[2] = 0x80000100;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                register u8* passageFlag PIN("r3");
                passageFlag = &gUnk_3003C4A;
                if (*passageFlag == 0) {
                    vu32* dma;
                    u32 source;
                    dma = (vu32*)0x040000D4;
                    source = *currentPassagePointer;
                    source--;
                    source <<= 5;
                    source += (u32)sUnk_86A25C8;
                    dma[0] = source;
                    dma[1] = 0x05000260;
                    dma[2] = 0x80000010;
                    dma[2];
                }
            }
            *((vu16*)0x05000000) = 0;
            {
                vu32* dma;
                u32 source;
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                source = (u32)blankScoreTiles;
                dma[0] = source;
                dma[1] = 0x06000000;
                dma[2] = 0x80003C00;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                vu32* dma;
                register const u8* source PIN("r2");
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                source = sUnk_86A13C8;
                dma[0] = (u32)source;
                dma[1] = 0x06010000;
                dma[2] = 0x80000800;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                vu32* dma;
                u32 passageOffset;
                u32 source;
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                passageOffset = *currentPassagePointer;
                passageOffset <<= 4;
                passageOffset += (u32)passageGraphicsTable;
                source = *((u32*)(passageOffset + 0));
                dma[0] = source;
                dma[1] = 0x0600C000;
                dma[2] = 0x80000800;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                vu32* dma;
                u32 passageOffset;
                u32 tableOffset;
                u32 source;
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                passageOffset = *currentPassagePointer;
                passageOffset <<= 4;
                tableOffset = (u32)passageGraphicsTable;
                tableOffset = tableOffset + 4;
                passageOffset += tableOffset;
                source = *((u32*)passageOffset);
                dma[0] = source;
                dma[1] = 0x0600D000;
                dma[2] = 0x80000800;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                vu32* dma;
                u32 passageOffset;
                u32 tableOffset;
                u32 source;
                u32 control;
                u32 busyMask;
                dma = (vu32*)0x040000D4;
                passageOffset = *currentPassagePointer;
                passageOffset <<= 4;
                tableOffset = (u32)passageGraphicsTable;
                tableOffset = tableOffset + 8;
                passageOffset += tableOffset;
                source = *((u32*)passageOffset);
                dma[0] = source;
                dma[1] = 0x0600E000;
                dma[2] = 0x80000800;
                control = dma[2];
                control = dma[2];
                busyMask = 0x80000000;
                if (((s32)control) < 0) {
                    do {
                        control = dma[2];
                        control &= busyMask;
                    } while (control != 0);
                }
            }
            {
                vu32* dma;
                u32 passageOffset;
                register u32 tableOffset PIN("r1");
                u32 source;
                dma = (vu32*)0x040000D4;
                passageOffset = *currentPassagePointer;
                passageOffset <<= 4;
                tableOffset = (u32)passageGraphicsTable;
                tableOffset = tableOffset + 12;
                passageOffset += tableOffset;
                source = *((u32*)passageOffset);
                dma[0] = source;
                dma[1] = 0x0600F000;
                dma[2] = 0x80000800;
                dma[2];
            }
            {
                vu16* bgControl;
                register u32 value PIN("r0");
                register u32 oddValue PIN("r2");
                register u32 evenValue PIN("r3");
                bgControl = (vu16*)0x04000008;
                evenValue = 0x1800;
                ASM_BARRIER(: "+r"(evenValue));
                value = evenValue;
                *bgControl++ = value;
                oddValue = 0x1A01;
                ASM_BARRIER(: "+r"(oddValue));
                value = oddValue;
                *bgControl++ = value;
                evenValue = 0x5C02;
                ASM_BARRIER(: "+r"(evenValue));
                value = evenValue;
                *bgControl++ = value;
                oddValue = 0x1E03;
                ASM_BARRIER(: "+r"(oddValue));
                value = oddValue;
                *bgControl = value;
            }
            *selectedStagePointer = 0;
            {
                u32 one;
                register u8* passageFlag PIN("r1");
                one = 1;
                *selectedStageIndexPointer = one;
                passageFlag = &gUnk_3003C4A;
                if (*passageFlag != 0) {
                    const u8* collection;
                    u32 passage;
                    u32 address;
                    s32 collected;
                    collection = gCurrentCollection;
                    passage = *currentPassagePointer;
                    address = passage << 1;
                    address += passage;
                    address <<= 3;
                    address += (u32)collection;
                    collected = *(u8*)address;
                    collected <<= 26;
                    if (collected >= 0) {
                        *selectedStagePointer = one;
                    }
                    goto passage_selection_done;
                } else {
                    s32 stageIndex;
                    u32 passage;
                    register u32 address PIN("r1");
                    s32 collected;
                    register u32 selected PIN("r0");
                    register u32 stageOffset PIN("r2");
                    stageIndex = 0;
                    passage = *currentPassagePointer;
                    address = passage << 1;
                    address += passage;
                    address <<= 3;
                    address += (u32)collectionData;
                    collected = *(u8*)address;
                    collected <<= 26;
                    if (collected >= 0) {
                        selected = 1;
                        goto passage_selection_store;
                    }
                passage_selection_loop:
                    selected = 4;
                    *selectedStageIndexPointer = selected;
                    stageIndex++;
                    if (stageIndex > 3) {
                        goto passage_selection_done;
                    }
                    stageOffset = stageIndex << 2;
                    {
                        u32 loopPassage;
                        u32 loopAddress;
                        loopPassage = *currentPassagePointer;
                        loopAddress = loopPassage << 1;
                        loopAddress += loopPassage;
                        loopAddress <<= 3;
                        stageOffset += loopAddress;
                    }
                    stageOffset += (u32)collectionData;
                    collected = *(u8*)stageOffset;
                    collected <<= 26;
                    if (collected < 0) {
                        goto passage_selection_loop;
                    }
                    selected = stageIndex + 1;
                passage_selection_store:
                    ASM_BARRIER(: "+r"(selectedStagePointer));
                    *selectedStagePointer = selected;
                    *selectedStageIndexPointer = stageIndex;
                }
            passage_selection_done:;
            }
            {
                register u32 scrollValue PIN("r0");
                u32 zeroByte;
                register u32 zeroHalf PIN("r4");
                u32 offset;
                u32 x;
                {
                    register u8* passageFlag PIN("r2");
                    u32 flag;
                    u32 tableIndex;
                    u32 selectedStage;
                    const u32* scrollTable;
                    register u32 base PIN("r2");
                    passageFlag = &gUnk_3003C4A;
                    flag = *passageFlag;
                    tableIndex = flag << 2;
                    tableIndex += flag;
                    selectedStage = *selectedStagePointer;
                    tableIndex += selectedStage;
                    tableIndex <<= 2;
                    scrollTable = sUnk_863C55C;
                    tableIndex += (u32)scrollTable;
                    flag = *(u32*)tableIndex;
                    base = 0x200;
                    ASM_BARRIER(: "+r"(base));
                    scrollValue = base;
                    ASM_BARRIER(: "+r"(scrollValue));
                    scrollValue -= flag;
                }
                zeroByte = 0;
                zeroHalf = 0;
                {
                    s16* horizontalScroll;
                    horizontalScroll = &gStageEntryHorizontalScroll;
                    *horizontalScroll = scrollValue;
                }
                {
                    u16* state;
                    state = mainSpriteState;
                    state[0] = zeroHalf;
                    state[1] = zeroHalf;
                }
                {
                    u8* currentStagePointer;
                    u32 stageOffset;
                    u8* passageFlag;
                    u32 flag;
                    u32 flagOffset;
                    const u32* xTable;
                    u32* entry;
                    u32 xValue;
                    register u16* state PIN("r1");
                    currentStagePointer = &gCurrentStageNumber;
                    stageOffset = *currentStagePointer;
                    stageOffset <<= 2;
                    passageFlag = &gUnk_3003C4A;
                    flag = *passageFlag;
                    flagOffset = flag << 1;
                    flagOffset += flag;
                    flagOffset <<= 3;
                    stageOffset += flagOffset;
                    xTable = sUnk_863C52C;
                    stageOffset += (u32)xTable;
                    entry = (u32*)stageOffset;
                    xValue = *entry;
                    xValue <<= 5;
                    state = mainSpriteState;
                    state[4] = xValue;
                    xValue = 160;
                    xValue <<= 4;
                    state[5] = xValue;
                }
                {
                    register u8* falling PIN("r2");
                    falling = &gStageEntryMainSpriteFalling;
                    *falling = zeroByte;
                }
                GP_SET(gUnk_3003C40, GP(sUnk_863C5D4[0]));
                gStageEntrySequenceTimer = zeroHalf;
                gStageEntrySequenceStep = zeroByte;
                gUnk_3003C54 = zeroByte;
                gUnk_3003C55 = zeroByte;
                gStageEntryExitRequested = zeroByte;
                gSoundRoomMessageTimer = zeroHalf;
            }
        }
    }
    {
        u32* scorePointer;
        u32 score;
        const u8* numberTiles;
        u32 position;
        scorePointer = &gTotalScore;
        score = *scorePointer;
        position = 0x2980;
        numberTiles = sUnk_869F648;
        UploadTallFiveDigitNumberTiles(score, numberTiles, position);
    }
    {
        u16* medalPointer;
        u32 medalCount;
        const u8* numberTiles;
        u32 position;
        medalPointer = &gMedalCount;
        medalCount = *medalPointer;
        position = 0x20E0;
        numberTiles = sUnk_869F648;
        DrawMinigameNumber(medalCount, numberTiles, position);
    }
    {
        s32 i;
        const u8* collection;
        u16 zero;
        register struct PassageIconState* icon PIN("r4");
        i = 0;
        collection = gCurrentCollection;
        zero = 0;
        icon = gUnk_3004708;
        do {
            u32 bits;
            u32 passage;
            u32 passageOffset;
            u32 test;
            bits = i << 2;
            passage = gCurrentPassage;
            passageOffset = passage << 1;
            passageOffset += passage;
            passageOffset <<= 3;
            bits += passageOffset;
            bits += (u32)collection;
            bits = *(u8*)bits;
            test = bits << 31;
            if (test == 0) {
                goto icon_inactive;
            }
            test = bits << 30;
            if ((s32)test >= 0) {
                goto icon_inactive;
            }
            test = bits << 29;
            if ((s32)test >= 0) {
                goto icon_inactive;
            }
            test = bits << 28;
            if ((s32)test >= 0) {
                goto icon_inactive;
            }
            icon->active = 1;
            goto icon_frame_reset;
        icon_inactive:
            icon->active = zero;
        icon_frame_reset:
            icon->frame = zero;
            icon->timer = zero;
            icon++;
            i++;
        } while (i <= 3);
    }
    {
        u32 zero;
        zero = 0;
        LoadPassageStageDisplayGraphics();
        DrawPassageStageScore();
        InitMinigameScoreDisplay();
        *((vu16*)0x04000012) = zero;
        *((vu16*)0x04000010) = zero;
        *((vu16*)0x04000016) = zero;
        *((vu16*)0x04000014) = zero;
        *((vu16*)0x0400001A) = zero;
        *((vu16*)0x04000018) = gStageEntryHorizontalScroll;
        *((vu16*)0x0400001E) = zero;
        *((vu16*)0x0400001C) = zero;
        *((vu16*)0x04000054) = zero;
        {
            vu16* displayControl;
            displayControl = (vu16*)0x04000000;
            {
                register u32 displayBase PIN("r1");
                register u32 value PIN("r0");
                displayBase = 0x1F00;
                ASM_BARRIER(: "+r"(displayBase));
                value = displayBase;
                *displayControl = value;
            }
            *displayControl |= 0x2000;
        }
        *((vu16*)0x04000048) = zero;
        *((vu16*)0x0400004A) = 63;
    }
    gStageEntryWindowLeft = 120;
    gStageEntryWindowRight = 120;
    gStageEntryWindowTop = 80;
    gStageEntryWindowBottom = 80;
    *((vu16*)0x04000050) = 0x1844;
    *((vu16*)0x04000052) = 0x0A06;
}

void BeginPassageStageCursorArrival(void)
{
    gCurrentStageNumber = 0;
    gStageEntryStageVariant = 0;
    BuildTemporaryStageSelectionSave();
    gUnk_3003C45 = 1;
    InitializePassageScreen();
    gUnk_3003C45 = 0;
    gStageEntryMainSpriteState[4] = sUnk_863C41C[gUnk_3003C4A] << 5;
    gStageEntryMainSpriteState[5] = 0x880;
    gUnk_3004700 = 1;
}

void BeginPassageStageSwitch(void)
{
    s16* scroll;
    const u32* table;
    u32 passage;
    u32 stage;
    u32 value;
    register u32 work PIN("r2");
    u32 result;

    gStageEntryStageVariant = gCurrentStageNumber;
    gUnk_3003C45 = 0;
    InitializePassageScreen();
    scroll = &gStageEntryHorizontalScroll;
    table = sUnk_863C55C;
    stage = gUnk_3004728;
    passage = gUnk_3003C4A;
    value = table[passage * 5 + stage];
    work = 0x200;
    ASM_BARRIER(: "=r"(result) : "0"(work));
    result -= value;
    work = 0;
    *scroll = result;
    gUnk_3004729 = 80;
    gStageEntryWindowLeft = work;
    gStageEntryWindowRight = 240;
    gStageEntryWindowTop = work;
    gStageEntryWindowBottom = 160;
    gUnk_3004700 = 2;
}

void InitializePassageStageEntry(void)
{
    gStageEntryStageVariant = gCurrentStageNumber;
    m4aSongNumStart(0x27F);
    gUnk_3003C45 = 0;
    InitializePassageScreen();
    gUnk_3004700 = 0;
}

void BeginPassageScreenWindowClose(void)
{
    gStageEntryStageVariant = gCurrentStageNumber;
    gUnk_3003C45 = 0;
    InitializePassageScreen();
    if (gCurrentStageNumber == 5) {
        m4aSongNumStart(0x27F);
    }
    gStageEntryWindowLeft = 0;
    gStageEntryWindowRight = 240;
    gStageEntryWindowTop = 0;
    gStageEntryWindowBottom = 160;
    gUnk_3004700 = 5;
}

void UpdatePassageStageCursorMovement(void)
{
    const u32* table;
    s32 target;
    s32 previous;
    register u32 stage PIN("r0");
    u32 passage;
    u32 offset;

    table = sUnk_863C52C;
    stage = gCurrentStageNumber;
    stage <<= 2;
    passage = gUnk_3003C4A;
    offset = passage * 3;
    offset <<= 3;
    stage += offset;
    stage += (u32)table;
    target = *(const u32*)stage;
    stage = gStageEntryStageVariant;
    stage <<= 2;
    stage += offset;
    stage += (u32)table;
    previous = *(const u32*)stage;
    UpdatePassageStageCursorSound();

    if (target > previous) {
        u16* state;
        s32 position;
        register s32 current PIN("r0");
        state = gStageEntryMainSpriteState;
        current = state[4];
        current += 60;
        previous = 0;
        state[4] = current;
        position = target << 5;
        current = (s16)state[4];
        if (position > current) {
            return;
        }
        state[4] = position;
    } else {
        u16* state;
        s32 position;
        register s32 current PIN("r0");
        state = gStageEntryMainSpriteState;
        current = state[4];
        current -= 60;
        previous = 0;
        state[4] = current;
        position = target << 5;
        current = (s16)state[4];
        if (position < current) {
            return;
        }
        state[4] = position;
    }

    gUnk_3003C45 = previous;
    LoadPassageStageDisplayGraphics();
    DrawPassageStageScore();
    gUnk_3003C55 = previous;
}

void UpdatePassageStageCursorSound(void)
{
    u8* counter;
    u32 value;

    counter = &gUnk_3003C55;
    (*counter)++;
    value = *counter;
    value &= 7;
    if (value == 0) {
        m4aSongNumStart(1);
    }
}

s32 HandlePassageScreenInput(void)
{
    u32 r4Value;
    u32 r5Value;
    u32 direction;
    u32 oldFalling;
    u32 pressedPtrSaved;
    u8* fallingInitial;
    u16* pressedPtr;
    u32 pressed;
    register u32 mask PIN("r0");
    u16 held;
    u32 nextStage;

    fallingInitial = &gStageEntryMainSpriteFalling;
    oldFalling = *fallingInitial;
    direction = 4;
    pressedPtr = &gButtonsPressed;
    pressed = *pressedPtr;
    r5Value = 1;
    mask = 1;
    mask &= pressed;
    r4Value = (u32)fallingInitial;
    pressedPtrSaved = (u32)pressedPtr;

    if (mask != 0) {
        mask = gCurrentStageNumber;
        if (mask == 4) {
            BeginPassageBossStageEntry();
            goto done;
        }
        if (mask == 5) {
            m4aSongNumStart(0x127);
        } else {
            m4aSongNumStart(294);
            m4aSongNumStart(458);
        }
        gUnk_3004700 = 4;
        m4aMPlayFadeOut(GP(gMPlayTable[2].info), 6);
        gStageEntryExitRequested = 0;
        goto done;
    }

    mask = 2;
    mask &= pressed;
    if (mask != 0) {
        *(u8*)r4Value = r5Value;
        BeginPassageScreenExit();
        if (*(u8*)r4Value == oldFalling) {
            goto done;
        }
        m4aSongNumStart(8);
        goto done;
    }

    {
        register u32 heldValue PIN("r1");
        u16 rightMask;
        heldValue = gButtonsHeld;
        mask = 32;
        mask &= heldValue;
        rightMask = (u16)mask;
        if (rightMask != 0) {
            gUnk_3003C54++;
            if (gUnk_3003C54 > 10) {
                direction = 0;
                *(u8*)r4Value = r5Value;
            }
        } else {
            mask = 16;
            mask &= heldValue;
            heldValue = (u16)mask;
            if (heldValue != 0) {
                gUnk_3003C54++;
                if (gUnk_3003C54 > 10) {
                    direction = 1;
                    *(u8*)r4Value = rightMask;
                }
            } else {
                gUnk_3003C54 = heldValue;
            }
        }

        {
            u32 pressedAgain;
            pressedAgain = *(u16*)pressedPtrSaved;
            mask = 32;
            mask &= pressedAgain;
            rightMask = (u16)mask;
            if (rightMask != 0) {
                direction = 0;
                mask = 1;
                *(u8*)r4Value = mask;
            } else {
                mask = 16;
                mask &= pressedAgain;
                if (mask != 0) {
                    direction = 1;
                    *(u8*)r4Value = rightMask;
                }
            }
        }
    }

    if (*(u8*)r4Value != oldFalling) {
        m4aSongNumStart(8);
    }

    r5Value = (u32)&gCurrentStageNumber;
    {
        u32 current;
        u32 compareValue;
        current = *(u8*)r5Value;
        compareValue = current + 1;
        mask = gStageEntrySelectedStage;
        if (compareValue == mask && direction == 1) {
            goto invalid;
        }
        if (direction == 4) {
            goto invalid;
        }

        {
            const u8* table;
            u32 offset;
            u32 passageValue;
            table = (const u8*)sUnk_863C4CC;
            offset = direction << 2;
            mask = current << 3;
            offset += mask;
            passageValue = gUnk_3003C4A;
            mask = passageValue * 3;
            mask <<= 4;
            offset += mask;
            offset += (u32)table;
            r4Value = *(const u32*)offset;
        }
    }

    if (r4Value == 7) {
        *(u8*)r5Value = 4;
        BeginPassageBossStageEntry();
        goto done;
    }
    if (r4Value == 8) {
        *(u8*)r5Value = 0;
        BeginPassageScreenExit();
        goto done;
    }

    gUnk_3003C45 = 1;
    LoadPassageStageDisplayGraphics();
    DrawPassageStageScore();
    gStageEntryStageVariant = *(u8*)r5Value;
    *(u8*)r5Value = r4Value;
    BuildTemporaryStageSelectionSave();
    goto done;

invalid:
    gSoundRoomMessageTimer = 1000;

done:
    nextStage = 0;
    return nextStage;
}

void BeginPassageScreenExit(void)
{
    m4aSongNumStart(0x129);
    gStageEntryExitRequested = 1;
    gUnk_3004700 = 3;
    gUnk_3003C45 = 1;
    LoadPassageStageDisplayGraphics();
    DrawPassageStageScore();
    gUnk_3003C45 = 0;
}

void BeginPassageBossStageEntry(void)
{
    m4aSongNumStart(0x126);
    gStageEntryExitRequested = 0;
    gUnk_3004700 = 4;
}

void LoadPassageStageDisplayGraphics(void)
{
    register u32 passage PIN("r12");
    u32 stage;
    u32 paletteVariant;
    register u32 graphicsIndex PIN("r5");
    u32 passageTimesFour;
    register vu32* dma PIN("r4");

    {
        u32 value;
        value = gUnk_3003C45;
        if (value != 0) {
            value = 5;
            passage = value;
            stage = 2;
            goto clearPaletteVariant;
        }
    }

    {
        register u8* stagePointer PIN("r1");
        u32 currentStage;
        stagePointer = &gCurrentStageNumber;
        currentStage = *stagePointer;
        if (currentStage == 5) {
            register u32 passageFive PIN("r3");
            passageFive = 5;
            ASM_BARRIER(: "+r"(passageFive));
            passage = passageFive;
            stage = 1;
            goto zeroPaletteVariant;
        }
        if (currentStage == 4) {
            u8* collection;
            u32 stageValue;
            u32 collectionOffset;
            u8* passagePointer;
            u32 passageValue;
            s32 flag;

            collection = gCurrentCollection;
            stageValue = *stagePointer;
            collectionOffset = stageValue << 2;
            passagePointer = &gCurrentPassage;
            passageValue = *passagePointer;
            flag = passageValue << 1;
            flag += passageValue;
            flag <<= 3;
            collectionOffset += flag;
            collectionOffset += (u32)collection;
            flag = *(u8*)collectionOffset;
            flag <<= 26;
            if (flag < 0) {
                passage = passageValue;
                stage = 5;
                goto clearPaletteVariant;
            }
            passage = passageValue;
            stage = stageValue;
        clearPaletteVariant: {
            u32 zero;
            zero = 0;
            paletteVariant = zero;
        }
            goto copyGraphics;
        }

        {
            u8* passagePointer;
            u32 passageValue;
            u8* collection;
            u32 collectionOffset;
            register s32 flag PIN("r0");
            register u32 one PIN("r3");

            passagePointer = &gCurrentPassage;
            passageValue = *passagePointer;
            passage = passageValue;
            stage = *stagePointer;
            collection = gCurrentCollection;
            collectionOffset = stage << 2;
            flag = passageValue << 1;
            flag += passage;
            flag <<= 3;
            collectionOffset += flag;
            collectionOffset += (u32)collection;
            flag = *(u8*)collectionOffset;
            flag <<= 26;
            one = 1;
            ASM_BARRIER(: "+r"(one));
            paletteVariant = one;
            if (flag >= 0) {
                goto copyGraphics;
            }
        }
    }
zeroPaletteVariant: {
    u32 zero;
    zero = 0;
    paletteVariant = zero;
}

copyGraphics:
    graphicsIndex = 0;
    {
        u32 stageTimesFour;
        u32 passageValue;
        u32 source;
        u32 temp;

        stageTimesFour = stage << 2;
        passageValue = passage;
        passageTimesFour = passageValue << 2;
        dma = (vu32*)0x040000D4;
        source = passageTimesFour + passageValue;
        source <<= 10;
        stageTimesFour += stage;
        stageTimesFour <<= 5;
        passageValue = (u32)sStageEntryPassageOverlayGraphics;
        stageTimesFour += passageValue;
        source += stageTimesFour;

        do {
            u32 graphicsOffset;
            u32 sourceAddress;
            register u32 destinationBase PIN("r3");
            u32 control;
            register u32 busyMask PIN("r3");
            register u32 nextIndex PIN("r1");

            graphicsOffset = graphicsIndex << 10;
            sourceAddress = source + graphicsOffset;
            dma[0] = sourceAddress;
            destinationBase = 0x06005C00;
            ASM_BARRIER(: "+r"(destinationBase));
            graphicsOffset += destinationBase;
            dma[1] = graphicsOffset;
            control = 0x80000050;
            dma[2] = control;
            control = dma[2];
            control = dma[2];
            busyMask = 0x80000000;
            nextIndex = graphicsIndex + 1;
            if ((s32)control < 0) {
                do {
                    control = dma[2];
                    control &= busyMask;
                } while (control != 0);
            }
            graphicsIndex = nextIndex;
        } while ((s32)graphicsIndex <= 4);
    }

    {
        register u32 variant PIN("r0");
        variant = paletteVariant;
        ASM_BARRIER(: "+r"(variant));
        if (variant != 0) {
            vu32* paletteDma;
            u32 stageOffset;
            u32 passageValue;
            u32 passageOffset;
            u32 paletteBase;

            paletteDma = (vu32*)0x040000D4;
            stageOffset = stage << 5;
            passageValue = passage;
            passageOffset = passageTimesFour + passageValue;
            passageOffset <<= 5;
            paletteBase = (u32)sUnk_86A0E08;
            passageOffset += paletteBase;
            stageOffset += passageOffset;
            paletteDma[0] = stageOffset;
            paletteDma[1] = 0x05000140;
            paletteDma[2] = 0x80000010;
            paletteDma[2];
        } else {
            u32 currentStage;
            vu32* paletteDma;
            u32 stageOffset;
            u32 passageValue;
            u32 passageOffset;
            u32 paletteBase;

            currentStage = gCurrentStageNumber;
            if (currentStage == 4) {
                stage = 4;
            }
            paletteDma = (vu32*)0x040000D4;
            stageOffset = stage << 5;
            passageValue = passage;
            passageOffset = passageTimesFour + passageValue;
            passageOffset <<= 5;
            paletteBase = (u32)sUnk_86A0A48;
            passageOffset += paletteBase;
            stageOffset += passageOffset;
            paletteDma[0] = stageOffset;
            paletteDma[1] = 0x05000140;
            paletteDma[2] = 0x80000010;
            paletteDma[2];
        }
    }

    BackupPalettesForColorFade();
}

void DrawPassageStageScore(void)
{
    u8* stagePointer;

    if (gUnk_3003C45 != 0) {
        goto drawBlankScores;
    }

    {
        u8* stageLoad;
        u32 stageValue;
        stageLoad = &gCurrentStageNumber;
        stageValue = *stageLoad;
        stagePointer = stageLoad;
        if (stageValue <= 3) {
            goto drawScore;
        }
    }

drawBlankScores: {
    s32 row;
    register vu32* dma PIN("r3");
    const u8* source;
    u32 destination;

    row = 0;
    dma = (vu32*)0x040000D4;
    source = sUnk_869CE48;
    destination = 0x06002580;
    do {
        u32 offset;
        u32 control;
        register u32 busyMask PIN("r1");

        dma[0] = (u32)source;
        offset = row << 5;
        offset += destination;
        dma[1] = offset;
        control = 0x80000010;
        dma[2] = control;
        control = dma[2];
        control = dma[2];
        busyMask = 0x80000000;
        row++;
        if ((s32)control < 0) {
            do {
                control = dma[2];
                control &= busyMask;
            } while (control != 0);
        }
    } while (row <= 14);
}
    goto end;

drawScore: {
    register vu32* dma PIN("r1");
    const u8* source;
    register u32 control PIN("r0");
    register u32 busyMask PIN("r3");
    s32(*scores)[4];
    u8* passagePointer;
    const u8* digitTiles;
    register const u8* savedDigitTiles PIN("r8");
    s32 score;

    dma = (vu32*)0x040000D4;
    source = sUnk_869F448;
    dma[0] = (u32)source;
    dma[1] = 0x06002600;
    dma[2] = 0x800000B0;
    control = dma[2];
    control = dma[2];
    busyMask = 0x80000000;
    scores = gHighScoreTable;
    passagePointer = &gCurrentPassage;
    digitTiles = (const u8*)(132 << 4);
    digitTiles += (u32)source;
    savedDigitTiles = digitTiles;
    if ((s32)control < 0) {
        register u32 waitMask PIN("r2");
        waitMask = busyMask;
        do {
            control = dma[2];
            control &= waitMask;
        } while (control != 0);
    }

    {
        u32 scoreAddress;
        u32 passageOffset;
        s32 scoreArgument;

        scoreAddress = *stagePointer;
        scoreAddress <<= 2;
        passageOffset = *passagePointer;
        passageOffset <<= 4;
        scoreAddress += passageOffset;
        scoreAddress += (u32)scores;
        scoreArgument = *(s32*)scoreAddress;
        digitTiles = savedDigitTiles;
        UploadFiveDigitNumberTiles(scoreArgument, digitTiles, 0x26A0);
    }

    {
        u32 scoreAddress;
        u32 passageOffset;
        s32 scoreValue;
        vu32* copyDma;
        register u32 copySource PIN("r0");

        scoreAddress = *stagePointer;
        scoreAddress <<= 2;
        passageOffset = *passagePointer;
        passageOffset <<= 4;
        scoreAddress += passageOffset;
        scoreAddress += (u32)scores;
        scoreValue = *(s32*)scoreAddress;
        if (scoreValue > 999) {
            s32 sourceOffset;
            copyDma = (vu32*)0x040000D4;
            sourceOffset = -0xC60;
            copySource = (u32)digitTiles + sourceOffset;
        } else if (scoreValue > 799) {
            s32 sourceOffset;
            copyDma = (vu32*)0x040000D4;
            sourceOffset = -0xBE0;
            copySource = (u32)digitTiles + sourceOffset;
        } else if (scoreValue > 599) {
            copyDma = (vu32*)0x040000D4;
            copySource = -0xB60;
            copySource += (u32)savedDigitTiles;
        } else {
            goto end;
        }

        copyDma[0] = copySource;
        copyDma[1] = 0x06002580;
        copyDma[2] = 0x80000040;
        copyDma[2];
    }
}

end:
    return;
}

s32 UpdatePassageStageCursorArrival(void)
{
    s32 target;
    s32 delta;

    switch (gStageEntrySequenceStep) {
        case 0: {
            u16* state;
            s32 current;
            state = gStageEntryMainSpriteState;
            state[4] += 60;
            current = (s16)state[4];
            target = sUnk_863C424[gUnk_3003C4A] << 5;
            if (current > target) {
                state[4] = target;
                gStageEntrySequenceStep++;
            }
            break;
        }
        case 1: {
            u16* state;
            s32 current;
            state = gStageEntryMainSpriteState;
            state[4] += 60;
            delta = sUnk_863C42C[gUnk_3003C4A] - sUnk_863C424[gUnk_3003C4A];
            state[5] += __divsi3(720, delta);
            current = (s16)state[4];
            target = sUnk_863C42C[gUnk_3003C4A] << 5;
            if (current > target) {
                u32 finalSpeed;
                state[4] = target;
                finalSpeed = 2560;
                state[5] = finalSpeed;
                gStageEntrySequenceStep++;
            }
            break;
        }
        case 2: {
            u16* state;
            s32 current;
            const u32* table;
            u32 stage;
            u32 passage;
            u32 offset;
            state = gStageEntryMainSpriteState;
            state[4] += 60;
            current = (s16)state[4];
            table = sUnk_863C52C;
            stage = gCurrentStageNumber;
            stage <<= 2;
            passage = gUnk_3003C4A;
            offset = passage * 3;
            offset <<= 3;
            stage += offset;
            stage += (u32)table;
            target = *(const u32*)stage;
            target <<= 5;
            if (current > target) {
                state[4] = target;
                gStageEntrySequenceStep = 0;
                LoadPassageStageDisplayGraphics();
                DrawPassageStageScore();
                return 1;
            }
            break;
        }
    }
    return 0;
}

s32 UpdatePassageStageSwitch(void)
{
    s32 target;
    s32 current;
    const u32* new_var2;
    u32 new_var;
    switch (gStageEntrySequenceStep) {
        case 0:
            if (ClosePassageScreenWindow() != 0) {
                gStageEntrySequenceStep++;
            }
            break;

        case 1:
            gUnk_3004729--;
            if (gUnk_3004729 <= 63) {
                gStageEntrySequenceStep++;
            }
            break;

        case 2:
            gStageEntryHorizontalScroll -= 2;
            current = gStageEntryHorizontalScroll;
            new_var2 = sUnk_863C55C;
            new_var = gStageEntrySelectedStage;
            new_var = (gUnk_3003C4A * 5) + new_var;
            target = 0x200 - new_var2[new_var];
            if (current < target) {
                gStageEntryHorizontalScroll = target;
                gUnk_3003C45 = 1;
                LoadPassageStageDisplayGraphics();
                DrawPassageStageScore();
                gStageEntryStageVariant = gCurrentStageNumber;
                {
                    u8* currentStage;
                    const u8* table2;
                    u32 stageOffset;
                    u32 passageValue;
                    u32 passageOffset;
                    currentStage = &gCurrentStageNumber;
                    table2 = (const u8*)sUnk_863C4CC;
                    stageOffset = *currentStage;
                    stageOffset <<= 3;
                    passageValue = gUnk_3003C4A;
                    passageOffset = passageValue * 3;
                    passageOffset <<= 4;
                    stageOffset += passageOffset;
                    table2 += 4;
                    stageOffset += (u32)table2;
                    *currentStage = *(const u32*)stageOffset;
                }
                BuildTemporaryStageSelectionSave();
                gStageEntrySequenceStep++;
            }
            break;

        case 3:
            UpdatePassageStageCursorMovement();
            if (gUnk_3003C45 == 0) {
                gStageEntrySequenceStep = 0;
                return (double)1;
            }
            break;
    }

    target = 0;
    return target;
}

s32 UpdatePassageScreenExitMovement(void)
{
    s32 target;
    int new_var;
    s32 current;
    s32 delta;
    u16* state;
    switch (gStageEntrySequenceStep) {
        case 0:;
            gStageEntryMainSpriteState[4] -= 80;
            current = (s16)gStageEntryMainSpriteState[4];
            target = sUnk_863C42C[gUnk_3003C4A] << 5;
            if (current < target) {
                gStageEntryMainSpriteState[4] = target;
                gStageEntrySequenceStep++;
            }
            break;

        case 1:
            state = gStageEntryMainSpriteState;
            state[4] -= 80;
            delta = sUnk_863C42C[gUnk_3003C4A] - sUnk_863C424[gUnk_3003C4A];
            state[5] -= __divsi3(960, delta);
            {
                s32 branchCurrent;
                branchCurrent = (s16)state[4];
                target = sUnk_863C424[gUnk_3003C4A] << 5;
                if (branchCurrent < target) {
                    state[4] = target;
                    new_var = 2176;
                    state[5] = new_var;
                    gStageEntrySequenceStep++;
                }
            }
            break;

        case 2:
            gStageEntryMainSpriteState[4] -= 80;
            delta = gStageEntryMainSpriteState[4];
            current = (s16)delta;
            target = sUnk_863C41C[gUnk_3003C4A] << 5;
            if (current < target) {
                gStageEntryMainSpriteState[4] = target;
                gStageEntrySequenceStep = 0;
                return 1;
            }
            break;
    }

    return 0;
}

void DrawPassageScreenSprites(void)
{
    u16* copy;
    s32 used;
    s32 drawn;
    u32 tableOffset;
    struct AGB_ALIGN {
        s32 index;
        s32 count;
        s32 stateOffset;
        s32 scratch;
    } locals;
    drawn = 0;
    {
        register u8* slots PIN("r2");
        u32 offset;
        register u8* oam PIN("r3");
        slots = &gOamSlotsUsed;
        used = *slots;
        offset = used << 3;
        oam = gOamBuffer;
        offset += (u32)oam;
        copy = (u16*)offset;
    }
    {
        u8* modePtr;
        u32 mode;
        register s32 four PIN("r4");
        modePtr = &gUnk_3003C4A;
        mode = *modePtr;
        four = 4;
        locals.count = four;
        if (mode != 0) {
            locals.count = 1;
        }
    }
    {
        register s32 zero PIN("r1");
        register s32 loopCount PIN("r2");
        zero = 0;
        locals.index = zero;
        loopCount = locals.count;
        if (drawn < loopCount) {
            register s32 stateZero PIN("r4");
            tableOffset = zero;
            stateZero = 0;
            locals.stateOffset = stateZero;
            do {
                const struct AnimationFrame* animation;
                register u8* stateBase PIN("r0");
                s32 stateIndex;
                struct PassageIconState* iconState;
                stateBase = (u8*)gUnk_3004708;
                stateIndex = locals.stateOffset;
                iconState = (struct PassageIconState*)stateIndex;
                iconState = (struct PassageIconState*)(((u32)iconState) + ((u32)stateBase));
                if (iconState->active != 0) {
                    {
                        struct PassageIconState* state;
                        state = iconState;
                        animation = GP(*((GBA_PTR(const struct AnimationFrame*) const *)(((const u8*)sUnk_863C4AC) + tableOffset)));
                        do {
                            struct PassageIconState* advanceState;
                            const struct AnimationFrame* advanceAnimation;
                            u32 advanceTimer;
                            u32 advanceFrame;
                            advanceState = state;
                            advanceAnimation = animation;
                            advanceTimer = advanceState->timer + 1;
                            advanceState->timer = advanceTimer;
                            advanceFrame = advanceState->frame;
                            if (advanceAnimation[advanceFrame].time < ((u16)advanceTimer)) {
                                advanceState->timer = 1;
                                advanceState->frame++;
                                advanceFrame = advanceState->frame;
                                if (advanceAnimation[advanceFrame].time == 0) {
                                    advanceState->frame = 0;
                                }
                            }
                        } while (0);
                    }
                    {
                        register s32 stateOffset2 PIN("r2");
                        register u8* stateBase2 PIN("r4");
                        register u32 frameAddress PIN("r0");
                        const u16* frameOam;
                        stateOffset2 = locals.stateOffset;
                        stateBase2 = (u8*)gUnk_3004708;
                        frameAddress = stateOffset2 + ((u32)stateBase2);
                        frameAddress = *((u16*)(frameAddress + 2));
                        frameAddress <<= 3;
                        frameAddress += (u32)animation;
                        frameOam = GP(*((GBA_PTR(const u16*) *)frameAddress));
                        do {
                            const u16* drawFrame;
                            drawFrame = frameOam;
                            used += *(drawFrame++);
                            if (used > 128) {
                                goto overflow;
                            }
                            if (drawn < used) {
                                u32 coordinateOffset;
                                register u32 clearMask PIN("r10");
                                register u32 byteMask PIN("r9");
                                register u32 screenOffset PIN("r0");
                                register u8* screenBase PIN("r2");
                                register u8* screen PIN("r4");
                                coordinateOffset = tableOffset;
                                clearMask = 0xFFFFFE00;
                                byteMask = (u32)(-13);
                                screenOffset = drawn << 3;
                                screenBase = gOamBuffer;
                                ASM_BARRIER(: "+r"(screenOffset));
                                screen = (u8*)screenOffset;
                                screen += (u32)screenBase;
                                drawn = used - drawn;
                                do {
                                    u32 attr;
                                    register u32 value PIN("r0");
                                    u32 x;
                                    attr = *(drawFrame++);
                                    *(copy++) = attr;
                                    value = attr;
                                    value += 36;
                                    screen[0] = value;
                                    attr = *(drawFrame++);
                                    *(copy++) = attr;
                                    value = gUnk_3003C4A;
                                    value <<= 4;
                                    value += coordinateOffset;
                                    x = *((const s32*)(((const u8*)sUnk_863C584) + value));
                                    x = (attr) - (-(x));
                                    attr = 0x1FF;
                                    x &= attr;
                                    attr = *((u16*)(screen + 2));
                                    value = clearMask;
                                    value &= attr;
                                    value |= x;
                                    *((u16*)(screen + 2)) = value;
                                    value = *drawFrame;
                                    *copy = value;
                                    drawFrame++;
                                    {
                                        register u32 oldByte PIN("r1");
                                        u32 mergedByte;
                                        oldByte = screen[5];
                                        mergedByte = byteMask;
                                        mergedByte &= oldByte;
                                        screen[5] = mergedByte;
                                    }
                                    copy += 2;
                                    screen += 8;
                                    drawn--;
                                } while (drawn != 0);
                                drawn = used;
                            }
                        } while (0);
                    }
                    animation = GP(*((GBA_PTR(const struct AnimationFrame*) const *)(((const u8*)sUnk_863C4BC) + tableOffset)));
                    do {
                        register const u16* drawFrame PIN("r3");
                        drawFrame = GP(animation[0].oam);
                        used += *(drawFrame++);
                        if (used > 128) {
                            goto overflow;
                        }
                        if (drawn < used) {
                            register u8* passagePtr PIN("r12");
                            const s32* xTable;
                            u32 byteMask;
                            register u32 screenOffset PIN("r0");
                            register u8* screenBase PIN("r1");
                            register u8* screen PIN("r4");
                            screen = &gUnk_3003C4A;
                            ASM_BARRIER(: "+r"(screen));
                            passagePtr = screen;
                            screenOffset = (u32)sUnk_863C5A4;
                            xTable = (const s32*)screenOffset;
                            screenOffset = drawn << 3;
                            screenBase = gOamBuffer;
                            ASM_BARRIER(: "+r"(screenOffset));
                            screen = (u8*)screenOffset;
                            screen += (u32)screenBase;
                            {
                                register u32 lowMask PIN("r2");
                                lowMask = (u32)(-13);
                                ASM_BARRIER(: "+r"(lowMask));
                                byteMask = lowMask;
                            }
                            drawn = used - drawn;
                            do {
                                u32 attr;
                                u32 value;
                                register u32 x PIN("r1");
                                attr = *(drawFrame++);
                                *(copy++) = attr;
                                {
                                    register u8* lowPtr PIN("r1");
                                    lowPtr = passagePtr;
                                    value = *lowPtr;
                                }
                                value = sUnk_863C5AC[value];
                                value += attr;
                                screen[0] = value;
                                attr = *(drawFrame++);
                                *(copy++) = attr;
                                {
                                    register u8* lowPtr PIN("r1");
                                    lowPtr = passagePtr;
                                    value = *lowPtr;
                                }
                                x = xTable[value];
                                x = attr + x;
                                attr = 0x1FF;
                                x &= attr;
                                attr = *((u16*)(screen + 2));
                                value = 0xFFFFFE00;
                                value &= attr;
                                value |= x;
                                *((u16*)(screen + 2)) = value;
                                value = *drawFrame;
                                *copy = value;
                                drawFrame++;
                                {
                                    register u32 oldByte PIN("r1");
                                    u32 mergedByte;
                                    oldByte = screen[5];
                                    mergedByte = byteMask;
                                    mergedByte &= oldByte;
                                    screen[5] = mergedByte;
                                }
                                copy += 2;
                                screen += 8;
                                drawn--;
                            } while (drawn != 0);
                            drawn = used;
                            do {
                            } while (0);
                        }
                    } while (0);
                } else {
                    {
                        register u8* passagePtr PIN("r2");
                        u32 passage;
                        u32 offset;
                        register u8* collection PIN("r3");
                        passagePtr = &gCurrentPassage;
                        passage = *passagePtr;
                        offset = passage << 1;
                        offset += passage;
                        offset <<= 3;
                        offset += tableOffset;
                        collection = gCurrentCollection;
                        offset += (u32)collection;
                        offset = *((u8*)offset);
                        offset <<= 31;
                        if (offset != 0) {
                            do {
                                register const u16* drawFrame PIN("r3");
                                drawFrame = GP(sUnk_864131C[0].oam);
                                used += *(drawFrame++);
                                if (used > 128) {
                                    goto overflow;
                                }
                                if (drawn < used) {
                                    u32 coordinateOffset;
                                    u32 clearMask;
                                    register u32 byteMask PIN("r9");
                                    u32 screenOffset;
                                    register u8* screenBase PIN("r1");
                                    u8* screen;
                                    coordinateOffset = tableOffset;
                                    {
                                        register union AGB_ALIGN {
                                            unsigned long long q;
                                            struct AGB_ALIGN {
                                                u32 lo;
                                                u32 hi;
                                            } w;
                                        } p34 PIN("r3");
                                        p34.w.hi = 0xFFFFFE00;
                                        clearMask = p34.w.hi;
                                    }
                                    byteMask = (u32)(-13);
                                    screenOffset = drawn << 3;
                                    screenBase = gOamBuffer;
                                    screenOffset += (u32)screenBase;
                                    screen = (u8*)screenOffset;
                                    drawn = used - drawn;
                                    do {
                                        u32 attr;
                                        register u32 value PIN("r0");
                                        u32 x;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = attr;
                                        value += 36;
                                        screen[0] = value;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = gUnk_3003C4A;
                                        value <<= 4;
                                        value += coordinateOffset;
                                        x = *((const s32*)(((const u8*)sUnk_863C584) + value));
                                        x = (attr) - (-(x));
                                        attr = 0x1FF;
                                        x &= attr;
                                        attr = *((u16*)(screen + 2));
                                        value = clearMask;
                                        value &= attr;
                                        value |= x;
                                        *((u16*)(screen + 2)) = value;
                                        value = *drawFrame;
                                        *copy = value;
                                        drawFrame++;
                                        {
                                            register u32 oldByte PIN("r1");
                                            u32 mergedByte;
                                            oldByte = screen[5];
                                            mergedByte = byteMask;
                                            mergedByte &= oldByte;
                                            screen[5] = mergedByte;
                                        }
                                        copy += 2;
                                        screen += 8;
                                        drawn--;
                                    } while (drawn != 0);
                                    drawn = used;
                                }
                            } while (0);
                        }
                    }
                    {
                        register u8* passagePtr PIN("r3");
                        u32 passage;
                        register u32 offset PIN("r1");
                        register u8* collection PIN("r4");
                        passagePtr = &gCurrentPassage;
                        passage = *passagePtr;
                        offset = passage << 1;
                        offset += passage;
                        offset <<= 3;
                        offset += tableOffset;
                        collection = gCurrentCollection;
                        offset += (u32)collection;
                        passage = *((u8*)offset);
                        passage <<= 30;
                        if (((s32)passage) < 0) {
                            do {
                                const u16* drawFrame;
                                drawFrame = GP(sUnk_864133C[0].oam);
                                used += *(drawFrame++);
                                if (used > 128) {
                                    goto overflow;
                                }
                                if (drawn < used) {
                                    u32 coordinateOffset;
                                    register u32 clearMask PIN("r10");
                                    register u32 byteMask PIN("r9");
                                    u32 screenOffset;
                                    register u8* screenBase PIN("r2");
                                    u8* screen;
                                    coordinateOffset = tableOffset;
                                    clearMask = 0xFFFFFE00;
                                    byteMask = (u32)(-13);
                                    screenOffset = drawn << 3;
                                    screenBase = gOamBuffer;
                                    screenOffset += (u32)screenBase;
                                    screen = (u8*)screenOffset;
                                    drawn = used - drawn;
                                    do {
                                        u32 attr;
                                        register u32 value PIN("r0");
                                        u32 x;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = attr;
                                        value += 36;
                                        screen[0] = value;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = gUnk_3003C4A;
                                        value <<= 4;
                                        value += coordinateOffset;
                                        x = *((const s32*)(((const u8*)sUnk_863C584) + value));
                                        x = (attr) - (-(x));
                                        attr = 0x1FF;
                                        x &= attr;
                                        attr = *((u16*)(screen + 2));
                                        value = clearMask;
                                        value &= attr;
                                        value |= x;
                                        *((u16*)(screen + 2)) = value;
                                        value = *drawFrame;
                                        *copy = value;
                                        drawFrame++;
                                        {
                                            register u32 oldByte PIN("r1");
                                            u32 mergedByte;
                                            oldByte = screen[5];
                                            mergedByte = byteMask;
                                            mergedByte &= oldByte;
                                            screen[5] = mergedByte;
                                        }
                                        copy += 2;
                                        screen += 8;
                                        drawn--;
                                    } while (drawn != 0);
                                    drawn = used;
                                }
                            } while (0);
                        }
                    }
                    {
                        register u8* passagePtr PIN("r3");
                        u32 passage;
                        register u32 offset PIN("r1");
                        register u8* collection PIN("r4");
                        passagePtr = &gCurrentPassage;
                        passage = *passagePtr;
                        offset = passage << 1;
                        offset += passage;
                        offset <<= 3;
                        offset += tableOffset;
                        collection = gCurrentCollection;
                        offset += (u32)collection;
                        passage = *((u8*)offset);
                        passage <<= 29;
                        if (((s32)passage) < 0) {
                            do {
                                const u16* drawFrame;
                                drawFrame = GP(sUnk_864132C[0].oam);
                                used += *(drawFrame++);
                                if (used > 128) {
                                    goto overflow;
                                }
                                if (drawn < used) {
                                    u32 coordinateOffset;
                                    register u32 clearMask PIN("r10");
                                    register u32 byteMask PIN("r9");
                                    u32 screenOffset;
                                    register u8* screenBase PIN("r2");
                                    u8* screen;
                                    coordinateOffset = tableOffset;
                                    clearMask = 0xFFFFFE00;
                                    byteMask = (u32)(-13);
                                    screenOffset = drawn << 3;
                                    screenBase = gOamBuffer;
                                    screenOffset += (u32)screenBase;
                                    screen = (u8*)screenOffset;
                                    drawn = used - drawn;
                                    do {
                                        u32 attr;
                                        register u32 value PIN("r0");
                                        u32 x;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = attr;
                                        value += 36;
                                        screen[0] = value;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = gUnk_3003C4A;
                                        value <<= 4;
                                        value += coordinateOffset;
                                        x = *((const s32*)(((const u8*)sUnk_863C584) + value));
                                        x = (attr) - (-(x));
                                        attr = 0x1FF;
                                        x &= attr;
                                        attr = *((u16*)(screen + 2));
                                        value = clearMask;
                                        value &= attr;
                                        value |= x;
                                        *((u16*)(screen + 2)) = value;
                                        value = *drawFrame;
                                        *copy = value;
                                        drawFrame++;
                                        {
                                            register u32 oldByte PIN("r1");
                                            u32 mergedByte;
                                            oldByte = screen[5];
                                            mergedByte = byteMask;
                                            mergedByte &= oldByte;
                                            screen[5] = mergedByte;
                                        }
                                        copy += 2;
                                        screen += 8;
                                        drawn--;
                                    } while (drawn != 0);
                                    drawn = used;
                                }
                            } while (0);
                        }
                    }
                    {
                        register u8* passagePtr PIN("r3");
                        u32 passage;
                        register u32 offset PIN("r1");
                        register u8* collection PIN("r4");
                        passagePtr = &gCurrentPassage;
                        passage = *passagePtr;
                        offset = passage << 1;
                        offset += passage;
                        offset <<= 3;
                        offset += tableOffset;
                        collection = gCurrentCollection;
                        offset += (u32)collection;
                        passage = *((u8*)offset);
                        passage <<= 28;
                        if (((s32)passage) < 0) {
                            do {
                                register const u16* drawFrame PIN("r3");
                                drawFrame = GP(sUnk_864130C[0].oam);
                                used += *(drawFrame++);
                                if (used > 128) {
                                    goto overflow;
                                }
                                if (drawn < used) {
                                    u32 coordinateOffset;
                                    register u32 clearMask PIN("r10");
                                    register u32 byteMask PIN("r9");
                                    register u32 screenOffset PIN("r0");
                                    register u8* screenBase PIN("r2");
                                    register u8* screen PIN("r4");
                                    coordinateOffset = tableOffset;
                                    clearMask = 0xFFFFFE00;
                                    byteMask = (u32)(-13);
                                    screenOffset = drawn << 3;
                                    screenBase = gOamBuffer;
                                    screenOffset += (u32)screenBase;
                                    screen = (u8*)screenOffset;
                                    drawn = used - drawn;
                                    do {
                                        u32 attr;
                                        register u32 value PIN("r0");
                                        u32 x;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = attr;
                                        value += 36;
                                        screen[0] = value;
                                        attr = *(drawFrame++);
                                        *(copy++) = attr;
                                        value = gUnk_3003C4A;
                                        value <<= 4;
                                        value += coordinateOffset;
                                        x = *((const s32*)(((const u8*)sUnk_863C584) + value));
                                        x = (attr) - (-(x));
                                        attr = 0x1FF;
                                        x &= attr;
                                        attr = *((u16*)(screen + 2));
                                        value = clearMask;
                                        value &= attr;
                                        value |= x;
                                        *((u16*)(screen + 2)) = value;
                                        value = *drawFrame;
                                        *copy = value;
                                        drawFrame++;
                                        {
                                            register u32 oldByte PIN("r1");
                                            u32 mergedByte;
                                            oldByte = screen[5];
                                            mergedByte = byteMask;
                                            mergedByte &= oldByte;
                                            screen[5] = mergedByte;
                                        }
                                        copy += 2;
                                        screen += 8;
                                        drawn--;
                                    } while (drawn != 0);
                                    drawn = used;
                                }
                            } while (0);
                        }
                    }
                }
                {
                    register union AGB_ALIGN {
                        unsigned long long q;
                        struct AGB_ALIGN {
                            u32 lo;
                            u32 hi;
                        } w;
                    } pair34 PIN("r3");
                    s32 index;
                    register s32 count PIN("r1");
                    pair34.w.lo = 4;
                    tableOffset += pair34.w.lo;
                    pair34.w.hi = locals.stateOffset;
                    pair34.w.hi += 8;
                    locals.stateOffset = pair34.w.hi;
                    index = locals.index;
                    index++;
                    locals.index = index;
                    count = locals.count;
                    if (index >= count) {
                        break;
                    }
                }
            } while (1);
        }
    }
    {
        register u8* modePtr PIN("r2");
        u32 mode;
        modePtr = &gUnk_3003C4A;
        mode = *modePtr;
        if (mode == 0) {
            register s32 zero PIN("r3");
            zero = 0;
            locals.index = zero;
            tableOffset = zero;
            do {
                register u8* passagePtr PIN("r4");
                u32 passage;
                register u32 collectionOffset PIN("r1");
                register u8* collection PIN("r0");
                u32 flags;
                passagePtr = &gCurrentPassage;
                passage = *passagePtr;
                collectionOffset = passage << 1;
                collectionOffset += passage;
                collectionOffset <<= 3;
                collectionOffset += tableOffset;
                collection = gCurrentCollection;
                collectionOffset += (u32)collection;
                flags = *((u8*)collectionOffset);
                flags <<= 27;
                if (((s32)flags) < 0) {
                    do {
                        const u16* drawFrame;
                        register u32 frameValue PIN("r0");
                        frameValue = (u32)sUnk_86413BC;
                        drawFrame = GP(*(GBA_PTR(const u16*) *)frameValue);
                        frameValue = *drawFrame;
                        used += frameValue;
                        drawFrame++;
                        if (used > 128) {
                            goto overflow;
                        }
                        if (drawn < used) {
                            u32 coordinateOffset;
                            u32 byteMask;
                            register u32 coordinateMask PIN("r9");
                            u32 screenOffset;
                            register u8* screenBase PIN("r2");
                            u8* screen;
                            coordinateOffset = tableOffset;
                            {
                                register union AGB_ALIGN {
                                    unsigned long long q;
                                    struct AGB_ALIGN {
                                        u32 lo;
                                        u32 hi;
                                    } w;
                                } p01 PIN("r0");
                                p01.w.hi = (u32)(-13);
                                byteMask = p01.w.hi;
                            }
                            screenOffset = drawn << 3;
                            screenBase = gOamBuffer;
                            screenOffset += (u32)screenBase;
                            screen = (u8*)screenOffset;
                            coordinateMask = 0x1FF;
                            drawn = used - drawn;
                            do {
                                register u32 attr PIN("r2");
                                u32 value;
                                register u32 x PIN("r1");
                                attr = *(drawFrame++);
                                *(copy++) = attr;
                                value = attr;
                                value += 20;
                                screen[0] = value;
                                attr = *(drawFrame++);
                                *(copy++) = attr;
                                value = gUnk_3003C4A;
                                value <<= 4;
                                value += coordinateOffset;
                                x = *((const s32*)(((const u8*)sUnk_863C584) + value));
                                x = attr + x;
                                {
                                    register union AGB_ALIGN {
                                        unsigned long long q;
                                        struct AGB_ALIGN {
                                            u32 lo;
                                            u32 hi;
                                        } w;
                                    } pair12 PIN("r1");
                                    pair12.w.lo = x;
                                    pair12.w.hi = coordinateMask;
                                    pair12.w.lo &= pair12.w.hi;
                                    x = pair12.w.lo;
                                }
                                attr = *((u16*)(screen + 2));
                                value = 0xFFFFFE00;
                                value &= attr;
                                value |= x;
                                *((u16*)(screen + 2)) = value;
                                value = *drawFrame;
                                *copy = value;
                                drawFrame++;
                                {
                                    register u32 oldByte PIN("r1");
                                    u32 mergedByte;
                                    oldByte = screen[5];
                                    mergedByte = byteMask;
                                    mergedByte &= oldByte;
                                    screen[5] = mergedByte;
                                }
                                copy += 2;
                                screen += 8;
                                drawn--;
                            } while (drawn != 0);
                            drawn = used;
                        }
                    } while (0);
                }
                {
                    union AGB_ALIGN {
                        unsigned long long q;
                        struct AGB_ALIGN {
                            u32 lo;
                            u32 hi;
                        } w;
                    } pair23;
                    register s32 nextIndex PIN("r4");
                    pair23.w.hi = 4;
                    tableOffset += pair23.w.hi;
                    nextIndex = locals.index;
                    nextIndex++;
                    locals.index = nextIndex;
                    if (nextIndex > 3) {
                        break;
                    }
                }
            } while (1);
        }
    }
    {
        register u8* selectedPtr PIN("r0");
        register u32 selected PIN("r1");
        selectedPtr = &gStageEntrySelectedStage;
        selected = *selectedPtr;
        if (selected != 0) {
            register s32 zero PIN("r0");
            zero = 0;
            locals.index = zero;
            {
                register u8* selectedPtr2 PIN("r2");
                register u32 selected2 PIN("r1");
                s32 limit;
                register s32 index PIN("r3");
                selectedPtr2 = &gStageEntrySelectedStage;
                selected2 = *selectedPtr2;
                limit = 5;
                limit -= selected2;
                index = locals.index;
                if (index < limit) {
                    do {
                        s32 nextIndex;
                        do {
                            const u16* drawFrame;
                            register s32 indexTemp PIN("r4");
                            drawFrame = GP(sUnk_86413CC[0].oam);
                            used += *(drawFrame++);
                            if (used > 128) {
                                goto overflow;
                            }
                            indexTemp = locals.index;
                            indexTemp++;
                            nextIndex = indexTemp;
                            if (drawn < used) {
                                register u32 coordinateOffset PIN("r12");
                                register u32 clearMask PIN("r10");
                                u32 coordinateMask;
                                register u32 screenOffset PIN("r0");
                                register u8* screenBase PIN("r2");
                                u8* screen;
                                screenOffset = locals.index;
                                screenOffset <<= 2;
                                coordinateOffset = screenOffset;
                                {
                                    register union AGB_ALIGN {
                                        unsigned long long q;
                                        struct AGB_ALIGN {
                                            u32 lo;
                                            u32 hi;
                                        } w;
                                    } p01 PIN("r0");
                                    p01.w.hi = 0xFFFFFE00;
                                    clearMask = p01.w.hi;
                                }
                                screenOffset = drawn << 3;
                                screenBase = gOamBuffer;
                                screenOffset += (u32)screenBase;
                                screen = (u8*)screenOffset;
                                coordinateMask = 0x1FF;
                                drawn = used - drawn;
                                do {
                                    register u32 attr PIN("r2");
                                    u32 value;
                                    register u32 x PIN("r1");
                                    attr = *(drawFrame++);
                                    *(copy++) = attr;
                                    value = attr;
                                    value += 80;
                                    screen[0] = value;
                                    attr = *(drawFrame++);
                                    *(copy++) = attr;
                                    value = gUnk_3003C4A;
                                    value <<= 4;
                                    value += coordinateOffset;
                                    x = *((const s32*)(((const u8*)sUnk_863C5B4) + value));
                                    x = attr + x;
                                    {
                                        register union AGB_ALIGN {
                                            unsigned long long q;
                                            struct AGB_ALIGN {
                                                u32 lo;
                                                u32 hi;
                                            } w;
                                        } pair12 PIN("r1");
                                        pair12.w.lo = x;
                                        pair12.w.hi = coordinateMask;
                                        pair12.w.lo &= pair12.w.hi;
                                        x = pair12.w.lo;
                                    }
                                    attr = *((u16*)(screen + 2));
                                    value = clearMask;
                                    value &= attr;
                                    value |= x;
                                    *((u16*)(screen + 2)) = value;
                                    value = *drawFrame;
                                    *copy = value;
                                    drawFrame++;
                                    {
                                        u32 oldByte;
                                        u32 mergedByte;
                                        oldByte = screen[5];
                                        mergedByte = (u32)(-13);
                                        mergedByte &= oldByte;
                                        oldByte = 8;
                                        mergedByte |= oldByte;
                                        screen[5] = mergedByte;
                                    }
                                    copy += 2;
                                    screen += 8;
                                    drawn--;
                                } while (drawn != 0);
                                drawn = used;
                            }
                        } while (0);
                        {
                            register s32 storeIndex PIN("r3");
                            register u8* selectedPtr3 PIN("r4");
                            register u32 selected3 PIN("r1");
                            s32 limit3;
                            storeIndex = nextIndex;
                            locals.index = storeIndex;
                            selectedPtr3 = &gStageEntrySelectedStage;
                            selected3 = *selectedPtr3;
                            limit3 = 5;
                            limit3 -= selected3;
                            if (storeIndex >= limit3) {
                                break;
                            }
                        }
                    } while (1);
                }
            }
        }
    }
    if (gUnk_3004700 == 2) {
        do {
            const u16* drawFrame;
            drawFrame = GP(sUnk_86413CC[0].oam);
            used += *(drawFrame++);
            if (used > 128) {
                goto overflow;
            }
            if (drawn < used) {
                u8* screenBase;
                u32 byteMask;
                u8* yPosition;
                u8* reverseIndex;
                register u32 screenOffset PIN("r0");
                u8* screen;
                u32 coordinateMask;
                register u32 lowValue PIN("r0");
                register u8* lowPointer PIN("r2");
                screenBase = gOamBuffer;
                lowValue = (u32)(-13);
                byteMask = lowValue;
                lowPointer = &gUnk_3004729;
                ASM_BARRIER(: "+r"(lowPointer));
                yPosition = lowPointer;
                {
                    register u8* reverseLow PIN("r4");
                    reverseLow = &gUnk_3004728;
                    ASM_BARRIER(: "+r"(reverseLow));
                    reverseIndex = reverseLow;
                }
                screenOffset = drawn << 3;
                ASM_BARRIER(: "+r"(screenOffset));
                screen = (u8*)screenOffset;
                screen += (u32)screenBase;
                lowValue = 0x1FF;
                ASM_BARRIER(: "+r"(lowValue));
                coordinateMask = lowValue;
                drawn = used - drawn;
                do {
                    register u32 attr PIN("r2");
                    u32 value;
                    register u32 x PIN("r1");
                    attr = *(drawFrame++);
                    *(copy++) = attr;
                    {
                        u8* positionLow;
                        positionLow = yPosition;
                        value = positionLow[0];
                    }
                    value += attr;
                    screen[0] = value;
                    attr = *(drawFrame++);
                    *(copy++) = attr;
                    {
                        u8* reverseLow;
                        reverseLow = reverseIndex;
                        value = reverseLow[0];
                    }
                    x = 4;
                    x -= value;
                    x <<= 2;
                    locals.scratch = x;
                    {
                        u8* modePtr;
                        modePtr = &gUnk_3003C4A;
                        value = modePtr[0];
                    }
                    value <<= 4;
                    x = locals.scratch;
                    value = (x) - (-(value));
                    {
                        const s32* coordinateTable;
                        coordinateTable = sUnk_863C5B4;
                        value += (u32)coordinateTable;
                    }
                    locals.scratch = value;
                    x = *((const s32*)value);
                    x = attr + x;
                    attr = coordinateMask;
                    ASM_BARRIER(: "+r"(attr));
                    x &= attr;
                    attr = *((u16*)(screen + 2));
                    value = 0xFFFFFE00;
                    value &= attr;
                    value |= x;
                    *((u16*)(screen + 2)) = value;
                    value = *drawFrame;
                    *copy = value;
                    drawFrame++;
                    {
                        u32 oldByte;
                        register u32 mergedByte PIN("r0");
                        oldByte = screen[5];
                        mergedByte = byteMask;
                        mergedByte &= oldByte;
                        oldByte = 8;
                        mergedByte |= oldByte;
                        screen[5] = mergedByte;
                    }
                    copy += 2;
                    screen += 8;
                    drawn--;
                } while (drawn != 0);
                drawn = used;
            }
        } while (0);
    }
    {
        u16* state;
        register u32 timer PIN("r1");
        u32 frame;
        register GBA_TYPEOF(const struct AnimationFrame**, &gUnk_3003C40) animationPointer PIN("r3");
        const struct AnimationFrame* animation;
        GBA_TYPEOF(const struct AnimationFrame**, &gUnk_3003C40) savedAnimationPointer;
        state = gStageEntryMainSpriteState;
        timer = state[0];
        timer++;
        state[0] = timer;
        frame = state[1];
        animationPointer = &gUnk_3003C40;
        animation = GP(*animationPointer);
        {
            u32 frameTime;
            frameTime = animation[frame].time;
            timer = (u16)timer;
            savedAnimationPointer = animationPointer;
            if (frameTime < timer) {
                state[0] = 1;
                frame = state[1];
                frame++;
                state[1] = frame;
                frame = state[1];
                {
                    register u32 nextTime PIN("r3");
                    nextTime = animation[frame].time;
                    if (nextTime == 0) {
                        state[1] = nextTime;
                    }
                }
            }
        }
        frame = state[1];
        {
            GBA_TYPEOF(const struct AnimationFrame**, &gUnk_3003C40) pointerLow;
            register const struct AnimationFrame* animationLow PIN("r1");
            pointerLow = savedAnimationPointer;
            animationLow = GP(*pointerLow);
            do {
                const u16* drawFrame;
                drawFrame = GP(animationLow[frame].oam);
                used += *(drawFrame++);
                if (used > 128) {
                    goto overflow;
                }
                if (drawn < used) {
                    u8* screenBase;
                    const u16* mainState;
                    register u32 clearMask PIN("r10");
                    register u32 screenOffset PIN("r0");
                    u8* screen;
                    u32 coordinateMask;
                    u32 lowValue;
                    screenBase = gOamBuffer;
                    mainState = state;
                    {
                        register u32 clearLow PIN("r4");
                        clearLow = 0xFFFFFE00;
                        ASM_BARRIER(: "+r"(clearLow));
                        clearMask = clearLow;
                    }
                    screenOffset = drawn << 3;
                    ASM_BARRIER(: "+r"(screenOffset));
                    screen = (u8*)screenOffset;
                    screen += (u32)screenBase;
                    lowValue = 0x1FF;
                    coordinateMask = lowValue;
                    drawn = used - drawn;
                    do {
                        u32 attr;
                        register s32 value PIN("r0");
                        s32 x;
                        register const u16* stateLow PIN("r1");
                        attr = *(drawFrame++);
                        *(copy++) = attr;
                        stateLow = mainState;
                        value = stateLow[5];
                        value <<= 16;
                        value >>= 21;
                        value += attr;
                        screen[0] = value;
                        attr = *(drawFrame++);
                        *(copy++) = attr;
                        x = stateLow[4];
                        x <<= 16;
                        x >>= 21;
                        x = (attr) - (-(x));
                        attr = coordinateMask;
                        ASM_BARRIER(: "+r"(attr));
                        x &= attr;
                        attr = *((u16*)(screen + 2));
                        value = clearMask;
                        value &= attr;
                        value |= x;
                        *((u16*)(screen + 2)) = value;
                        value = *drawFrame;
                        *copy = value;
                        drawFrame++;
                        value = screen[5];
                        x = 12;
                        value |= x;
                        screen[5] = value;
                        copy += 2;
                        screen += 8;
                        drawn--;
                    } while (drawn != 0);
                }
            } while (0);
        }
    }
    {
        register u8* slots PIN("r3");
        slots = &gOamSlotsUsed;
        *slots = used;
    }
overflow:
    return;
}

s32 ClosePassageScreenWindow(void)
{
    u16* leftPtr;
    u16* rightPtr;
    u16* bottomPtr;
    register s32 left PIN("r1");
    s32 value;

    leftPtr = &gStageEntryWindowLeft;
    left = *leftPtr;
    left += 10;
    *leftPtr = left;

    rightPtr = &gStageEntryWindowRight;
    value = *rightPtr;
    value -= 10;
    *rightPtr = value;

    left = (s16)left;
    bottomPtr = &gStageEntryWindowBottom;
    if (left > 40) {
        u16* topPtr;
        topPtr = &gStageEntryWindowTop;
        left = *topPtr;
        left += 10;
        *topPtr = left;
        value = *bottomPtr;
        value -= 10;
        *bottomPtr = value;
    }

    value = (s16)*bottomPtr;
    if (value <= 79) {
        register u16* topPtr PIN("r1");
        topPtr = &gStageEntryWindowTop;
        value = 80;
        *topPtr = value;
        *bottomPtr = value;
    }

    value = (s16)*rightPtr;
    if (value <= 119) {
        goto finished;
    }
    value = 0;
    goto done;
finished:
    value = 120;
    *leftPtr = value;
    *rightPtr = value;
    value = 1;
done:
    return value;
}

s32 OpenPassageScreenWindow(void)
{
    s32 right;
    gStageEntryWindowLeft -= 10;
    right = gStageEntryWindowRight + 10;
    gStageEntryWindowRight = right;
    gStageEntryWindowTop -= 10;
    gStageEntryWindowBottom += 10;
    if ((s16)gStageEntryWindowBottom > 160) {
        gStageEntryWindowTop = 0;
        gStageEntryWindowBottom = 160;
    }
    if ((s16)right > 240) {
        gStageEntryWindowLeft = 0;
        gStageEntryWindowRight = 240;
        return 1;
    }
    return 0;
}

#define DRAW_PASSAGE_NAME_TEXT(tableValue) \
    do { \
        GBA_TYPEOF(const u8 *const *, &(tableValue)[0]) nameTable; \
        u8 *stagePointer; \
        u32 stageOffset; \
        u8 *passagePointer; \
        u32 passageValue; \
        u32 passageOffset; \
        const u8 *text; \
        nameTable = (tableValue); \
        stagePointer = &gCurrentStageNumber; \
        stageOffset = *stagePointer; \
        stageOffset <<= 2; \
        passagePointer = &gCurrentPassage; \
        passageValue = *passagePointer; \
        passageOffset = passageValue; \
        passageOffset <<= 1; \
        passageOffset += passageValue; \
        passageOffset <<= 3; \
        stageOffset += passageOffset; \
        stageOffset += (u32)nameTable; \
        text = GP(*(GBA_PTR(const u8 *) *)stageOffset); \
        Shop_DrawText(text, 0x7800, 26); \
    } while (0)

void PassageScreenVBlank(void)
{
    volatile u16* window;
    window = (volatile u16*)0x04000040;
    *window = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
    window += 2;
    *window = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;
    switch (gLanguage) {
        case 0:
            DRAW_PASSAGE_NAME_TEXT(sFileSelectPassageNameTableLanguage0);
            break;
        case 1:
            DRAW_PASSAGE_NAME_TEXT(sFileSelectPassageNameTableLanguage1);
            break;
    }
    *(volatile u16*)0x04000018 = gStageEntryHorizontalScroll;
}

#undef DRAW_PASSAGE_NAME_TEXT

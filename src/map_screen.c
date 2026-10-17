#include "map_screen.h"
#include "boss_treasure_cutscene.h"
#include "gba/m4a.h"
#include "global_data.h"
#include "init_helpers.h"
#include "score.h"
#include "stage_select.h"

extern const u8 sUnk_8642958[];
extern const u8 sUnk_8646358[];
extern const u8 sUnk_8642B58[];
extern const u8 sUnk_8646558[];
extern const u8 sUnk_864A558[];
extern const u8 sUnk_864AD58[];
extern const u8 sUnk_864B558[];
extern const u8 sUnk_864BF58[];
extern u16 gUnk_3003C74[];
extern u8 gUnk_3003C60;
extern u16 gUnk_3003C7C[];
extern u16 gUnk_3003C80[];
extern u16 gUnk_3003C84[];
extern u16 gUnk_3003C90;

extern u8 gUnk_3003C6E;
extern u16 gUnk_3003C6C;
extern u16 gUnk_3003C70;
extern u16 gUnk_3003C78[];
extern GBA_PTR(const struct AnimationFrame*) gUnk_3003C64;
extern u8 gUnk_3003C96;
extern const u16 sUnk_864BD58[];
extern u8 gCurrentPassage;
extern u8 gCurrentStageNumber;
extern u8 gUnk_3003C94;
extern u8 gStageEntryMainSpriteFalling;
extern u16 gStageEntryMainSpriteState[];
extern GBA_PTR(const struct AnimationFrame*) gUnk_3003C40;
extern u16 gUnk_3003C88[];
extern u16 gStageEntryWindowLeft;
extern u16 gStageEntryWindowRight;
extern const s32 sUnk_86392A4[];
extern const struct AnimationFrame sUnk_8640404[];
extern const struct AnimationFrame sUnk_8640900[];
extern const s32 sUnk_8639510[];
extern const s32 sUnk_863951C[];
extern const s32 sUnk_863926C[];
extern const s32 sUnk_8639288[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_86392C0[];
extern const s32 sUnk_8639528[];
extern GBA_PTR(const struct AnimationFrame*) gUnk_3003C68;
extern u32 gStageEntryShakeMode;
extern s16 gSoundRoomMessageTimer;
extern const struct AnimationFrame sUnk_86404A4[];
extern const struct AnimationFrame sUnk_8640394[];
extern u8 gUnk_3003C45;
extern u8 gStageEntrySequenceStep;
extern u16 gStageEntrySequenceTimer;
extern u16 gUnk_3003C46;
extern u16 gUnk_3003C48;
extern u16 gUnk_3003C92;
extern u8 gStageEntryShakeTimer;
extern const s32 sUnk_86395FC[];
extern const s32 sUnk_86395C4[];
extern const s32 sUnk_86395E0[];
extern const s32 sUnk_86394C8[];
extern const s32 sUnk_8639500[];
extern const s32 sUnk_86392D0[];
extern const s32 sUnk_8639420[];
extern const s32 sUnk_8639490[];
extern u8 gUnk_3003C95;
extern u8 gStageEntryExitRequested;
extern u16 gButtonsPressed;
extern u8 gUnk_3003C6F;
extern const struct AnimationFrame sUnk_863FD1C[];
extern u8 gUnk_3003C97;
extern const struct AnimationFrame sUnk_8640454[];
extern const struct AnimationFrame sUnk_86409E0[];
extern const struct AnimationFrame sUnk_8640960[];
extern const struct AnimationFrame sUnk_8640858[];
extern const struct AnimationFrame sUnk_863FFAC[];
extern const struct AnimationFrame sUnk_8640C68[];
extern const struct AnimationFrame sUnk_8641070[];
extern const s32 sUnk_86395B4[];
extern const s32 sUnk_86395A4[];
extern const struct AnimationFrame sUnk_8640B28[];
extern const s32 sUnk_8639534[];
extern const s32 sUnk_8639550[];
extern GBA_PTR(const struct AnimationFrame*) const sUnk_8639618[];
extern const u16 sUnk_8640A58[];
extern const struct AnimationFrame sUnk_863FF84[];
void VoiceSetPlay(s32 voice);

void m4aSongNumStart(u16 songNum);
void BuildTemporaryStageSelectionSave(void);
extern u16 gUnk_3003C62;
extern const u16 sUnk_864BD78[];
extern const u16 sUnk_864BD98[];
extern const u16 sUnk_864BDB8[];
extern const u16 sUnk_864BDD8[];

extern u8 gLanguage;
extern GBA_PTR(const u16*) const sFileSelectStageNameTableLanguage0[];
extern GBA_PTR(const u16*) const sFileSelectStageNameTableLanguage1[];
s32 UpdateStageEntryHorizontalShake(void);
void Shop_DrawText(const u16* text, u32 dst, u32 unused);

int UpdateMapScreen(void)
{
    gSoundRoomMessageTimer = 0;

    switch (gUnk_3003C94) {
        case 0:
            if (gUnk_3003C45 != 0) {
                ApplyMapScreenScrollVelocity();
                UpdateMapScreenPassageTransition();
            } else if (UpdateMapScreenPassageInput() != 0) {
                gUnk_3003C94 = 3;
            }

            GP_SET(gUnk_3003C40, GP(sUnk_86392C0[gStageEntryMainSpriteFalling]));
            AdvanceMapScreenWarioAnimation();
            UpdateMapScreenTreasurePalettes();

            if (gStageEntryMainSpriteFalling == 1) {
                GP_SET(gUnk_3003C64, sUnk_86404A4);
                gUnk_3003C78[0] = 0;
                gUnk_3003C78[1] = 0;
            } else {
                GP_SET(gUnk_3003C64, sUnk_8640394);
            }

            AdvanceMapScreenDoorAnimation();
            break;

        case 1:
            UpdateMapScreenIntroSequence();
            break;

        case 2:
            UpdateMapScreenBossUnlockSequence();
            break;

        case 4:
            UpdateMapScreenFinalPassageSequence();
            UpdateMapScreenTreasurePalettes();
            break;

        case 3:
            AdvanceMapScreenWarioAnimation();
            if (UpdateMapScreenStageStartTransition() != 0) {
                return 1;
            }
            break;
    }

    UpdateMapScreenPaletteCycle();
    return 0;
}

void InitMapScreen(void)
{
    register const void* bgTiles0;
    register const void* objTiles;
    register const void* bgMap0;
    register const void* bgMap1;
    const void* bgMap2;
    const void* bgMap3;
    const struct MusicPlayer* musicPlayers;
    const struct Song* songs;
    register volatile u32* dma PIN("r1");
    register u32 mask PIN("r2");
    u32 control;
    u16 zero16;
    u8 zero8;
    u16* mainState;
    u8* mapMode;
    s32 bgVertical;

    gCurrentStageNumber = 6;
    InitializeVideoMemory();

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)sUnk_8642958;
    dma[1] = 0x05000000;
    dma[2] = 0x80000100;
    control = dma[2];
    control = dma[2];
    mask = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & mask;
        } while (control != 0);
    }

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)sUnk_8646358;
    dma[1] = 0x05000200;
    dma[2] = 0x80000100;
    control = dma[2];
    control = dma[2];
    mask = 0x80000000;
    bgTiles0 = sUnk_8642B58;
    objTiles = sUnk_8646558;
    bgMap0 = sUnk_864A558;
    bgMap1 = sUnk_864AD58;
    bgMap2 = sUnk_864B558;
    bgMap3 = sUnk_864BF58;
    musicPlayers = gMPlayTable;
    songs = gSongTable;
    if ((s32)control < 0) {
        do {
            control = dma[2] & mask;
        } while (control != 0);
    }

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)bgTiles0;
    dma[1] = 0x06000000;
    dma[2] = 0x80001C00;
    control = dma[2];
    control = dma[2];
    mask = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & mask;
        } while (control != 0);
    }

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)objTiles;
    dma[1] = 0x06010000;
    dma[2] = 0x80002000;
    control = dma[2];
    control = dma[2];
    mask = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & mask;
        } while (control != 0);
    }

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)bgMap0;
    dma[1] = 0x0600C000;
    dma[2] = 0x80000400;
    control = dma[2];
    control = dma[2];
    mask = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & mask;
        } while (control != 0);
    }

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)bgMap1;
    dma[1] = 0x0600D000;
    dma[2] = 0x80000400;
    control = dma[2];
    control = dma[2];
    mask = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & mask;
        } while (control != 0);
    }

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)bgMap2;
    dma[1] = 0x0600E000;
    dma[2] = 0x80000400;
    control = dma[2];
    control = dma[2];
    mask = 0x80000000;
    if ((s32)control < 0) {
        do {
            control = dma[2] & mask;
        } while (control != 0);
    }

    dma = (volatile u32*)0x040000D4;
    dma[0] = (u32)bgMap3;
    dma[1] = 0x0600F000;
    dma[2] = 0x80000400;
    control = dma[2];

    {
        volatile u16* bgControl;
        bgControl = (volatile u16*)((u8*)dma - 0xCC);
        *bgControl++ = 0x1801;
        *bgControl++ = 0x1A02;
        *bgControl++ = 0x1C03;
        *bgControl = 0x1E00;
    }

    m4aMPlayVolumeControl(GP(musicPlayers[*(const u16*)((const u8*)songs + 0x13FC)].info), 0xFFFF, 0x100);
    InitMinigameScoreDisplay();

    gSoundRoomMessageTimer = 0;
    mainState = gStageEntryMainSpriteState;
    zero16 = 0;
    zero8 = 0;
    mainState[0] = zero16;
    mainState[1] = zero16;
    mainState[4] = sUnk_863926C[gCurrentPassage] << 5;
    mainState[5] = sUnk_8639288[gCurrentPassage] << 5;
    gStageEntryMainSpriteFalling = zero8;
    GP_SET(gUnk_3003C40, GP(sUnk_86392C0[0]));
    gUnk_3003C74[0] = zero16;
    gUnk_3003C74[1] = zero16;
    gUnk_3003C96 = zero8;
    gUnk_3003C78[0] = zero16;
    gUnk_3003C78[1] = zero16;
    GP_SET(gUnk_3003C64, sUnk_86404A4);
    gUnk_3003C60 = 5;
    gUnk_3003C7C[0] = zero16;
    gUnk_3003C7C[1] = zero16;
    gUnk_3003C60 = (gTotalScore / 10) % 10;
    gUnk_3003C45 = zero8;
    gCurrentPassageTemp = gCurrentPassage;
    gUnk_3003C90 = 120;
    gUnk_3003C92 = 1920;
    gUnk_3003C94 = zero8;
    gStageEntrySequenceStep = zero8;
    gStageEntrySequenceTimer = zero16;
    gStageEntryShakeTimer = zero8;
    gStageEntryShakeMode = 0;
    gUnk_3003C97 = zero8;
    gUnk_3003C80[0] = zero16;
    gUnk_3003C80[1] = zero16;
    gUnk_3003C84[0] = zero16;
    gUnk_3003C84[1] = zero16;
    gUnk_3003C6F = zero8;

    {
        const u8* collection;
        collection = (const u8*)gCurrentCollection;
        if ((collection[16] & 0x20) == 0) {
            u8* modeZero;
            modeZero = &gUnk_3003C95;
            *modeZero = zero8;
            mapMode = modeZero;
        } else {
            register u8* modeStore PIN("r1");
            u32 modeValue;

            if (((collection[40] & 0x20) != 0) && ((collection[64] & 0x20) != 0) && ((collection[88] & 0x20) != 0) &&
                ((collection[112] & 0x20) != 0)) {
                modeStore = &gUnk_3003C95;
                modeValue = 2;
            } else {
                modeStore = &gUnk_3003C95;
                modeValue = 1;
            }
            *modeStore = modeValue;
            mapMode = modeStore;
        }
    }

    gUnk_3003C88[2] = 0;
    gUnk_3003C88[3] = 0;
    gUnk_3003C88[0] = sUnk_8639510[*mapMode] << 4;
    gUnk_3003C88[1] = sUnk_863951C[*mapMode] << 4;
    GP_SET(gUnk_3003C68, (const struct AnimationFrame*)sUnk_8639528[*mapMode]);

    {
        vu16* scroll;
        register u16 zero;
        vu16* bg23;

        scroll = (vu16*)0x04000012;
        bgVertical = (s16)gUnk_3003C6C >> 4;
        zero = 0;
        *scroll = bgVertical;
        scroll--;
        *scroll = zero;
        scroll += 3;
        *scroll = bgVertical;
        scroll--;
        *scroll = zero;
        scroll += 3;
        *scroll = bgVertical;

        bg23 = (vu16*)0x04000018;
        *bg23 = zero;
        bg23 += 3;
        *bg23 = zero;
        bg23--;
        *bg23 = zero;

        scroll -= 13;
        *scroll = 0x3F00;
        scroll += 40;
        *scroll = 0x1641;
    }

    if (*mapMode == 0) {
        gStageEntryWindowLeft = 80;
        gStageEntryWindowRight = 160;
    } else {
        gStageEntryWindowLeft = 0;
        gStageEntryWindowRight = 240;
    }

    *(vu16*)0x04000040 = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
    *(vu16*)0x04000044 = 160;
    *(vu16*)0x04000048 = 63;
    *(vu16*)0x0400004A = 8;
    {
        vu16* blendReg;
        register u16 blendValue PIN("r0");
        blendReg = (vu16*)0x04000052;
        ASM_BARRIER(: "=r"(blendValue) : "0"(0x0A0A), "r"(blendReg));
        *blendReg = blendValue;
    }
}
void StartMapScreenFromStageSelect(void)
{
    u16* dst;
    const s32* table;
    u8* passage;
    s32 value;

    m4aSongNumStart(0x27F);
    dst = &gUnk_3003C6C;
    table = sUnk_86392A4;
    passage = &gCurrentPassage;
    value = table[*passage];
    *dst = value << 4;
    InitMapScreen();
}

void StartMapScreenAfterStageExit(void)
{
    u16* dst;
    const s32* table;
    u8* passage;
    s32 value;

    dst = &gUnk_3003C6C;
    table = sUnk_86392A4;
    passage = &gCurrentPassage;
    value = table[*passage];
    *dst = value << 4;
    gCurrentStageNumber = 6;
    BuildTemporaryStageSelectionSave();
    InitMapScreen();
}

void StartMapScreenIntroSequence(void)
{
    gCurrentPassage = 0;
    m4aSongNumStart(0x27F);
    gUnk_3003C6C = 0;
    InitMapScreen();
    gUnk_3003C94 = 1;
    GP_SET(gUnk_3003C64, sUnk_8640404);
    gStageEntryMainSpriteState[4] = 0xF00;
    gStageEntryMainSpriteState[5] = 0x1900;
    GP_SET(gUnk_3003C40, sUnk_8640900);
}

void StartMapScreenFinalPassageIntro(void)
{
    u16* scroll;
    u16* position;
    const s32* table;
    s32 value;

    m4aSongNumStart(0x27F);
    scroll = &gUnk_3003C6C;
    *scroll = 0x1E0;
    InitMapScreen();
    gUnk_3003C94 = 4;
    gStageEntryMainSpriteFalling = 1;
    gCurrentPassage = 5;

    position = gStageEntryMainSpriteState;
    table = sUnk_863926C;
    value = table[5];
    position[4] = value << 5;
    table = sUnk_8639288;
    value = table[5] + 42;
    position[5] = value << 5;

    GP_SET(gUnk_3003C40, GP(sUnk_86392C0[1]));
    gUnk_3003C88[0] = sUnk_8639510[1] << 4;
    gUnk_3003C88[1] = sUnk_863951C[1] << 4;
    GP_SET(gUnk_3003C68, sUnk_8639528[1]);
    gStageEntryShakeMode = 2;
    m4aSongNumStart(0x1AB);
}

void StartMapScreenBossUnlockSequence(void)
{
    u16* dst;
    const s32* table;
    u8* passage;
    s32 value;

    dst = &gUnk_3003C6C;
    table = sUnk_86392A4;
    passage = &gCurrentPassage;
    value = table[*passage];
    *dst = value << 4;
    m4aSongNumStart(0x27F);
    InitMapScreen();
    gUnk_3003C94 = 2;
    gUnk_3003C88[0] = sUnk_8639510[0] << 4;
    gUnk_3003C88[1] = sUnk_863951C[0] << 4;
    gStageEntryWindowLeft = 80;
    gStageEntryWindowRight = 160;
}

int UpdateMapScreenPassageInput(void)
{
    register u32 directionOrPrevious PIN("r5");
    u32 fallingOrDestination;
    register u32 mapModeOrDurationTable PIN("r8");
    register u32 transitionPointer PIN("r9");
    u32 passageTableOrXSpeed;
    register s32 passageOrTransition PIN("r4");
    u32 directionOffset;
    u8* fallingSource;
    register u16 buttons PIN("r2");
    u8* currentPassagePointer;
    u32 previousFalling;
    u32 pressed;

    fallingSource = &gStageEntryMainSpriteFalling;
    previousFalling = *fallingSource;
    directionOrPrevious = 4;
    buttons = gButtonsPressed;
    pressed = buttons & 1;
    fallingOrDestination = (u32)fallingSource;

    if (pressed != 0) {
        m4aSongNumStart(0x125);
        gStageEntryExitRequested = 0;
        return 1;
    }

    if (buttons & 0x20) {
        directionOrPrevious = 0;
        *(u8*)fallingOrDestination = 2;
    } else if (buttons & 0x40) {
        directionOrPrevious = 2;
        *(u8*)fallingOrDestination = 1;
    } else if ((u16)(buttons & 0x10) != 0) {
        directionOrPrevious = 1;
        *(u8*)fallingOrDestination = 3;
    } else if (buttons & 0x80) {
        directionOrPrevious = 3;
        *(u8*)fallingOrDestination = 0;
    }

    if (*(u8*)fallingOrDestination != previousFalling) {
        m4aSongNumStart(8);
    }

    passageTableOrXSpeed = (u32)sUnk_86392D0;
    directionOffset = directionOrPrevious << 2;
    currentPassagePointer = &gCurrentPassage;
    passageOrTransition = *currentPassagePointer;

    {
        register u32 byteOffset PIN("r1");
        u32 mode;
        s32 tableValue;

        byteOffset = directionOffset + (passageOrTransition << 4);
        {
            register u8* mapModePointer PIN("r2");
            mapModePointer = &gUnk_3003C95;
            mapModeOrDurationTable = (u32)mapModePointer;
            mode = *mapModePointer;
        }
        tableValue = (mode << 3) - mode;
        tableValue <<= 4;
        byteOffset += tableValue;
        byteOffset += passageTableOrXSpeed;
        tableValue = *(const s32*)byteOffset;

        if ((tableValue != 7) && (directionOrPrevious != 4)) {
            directionOrPrevious = (u32)&gCurrentPassageTemp;
            *(u8*)directionOrPrevious = passageOrTransition;
            {
                u8* transitionInit;
                transitionInit = &gUnk_3003C6E;
                ASM_BARRIER(: : "r"(transitionInit));
                transitionPointer = (u32)transitionInit;
            }
            {
                const s32* transitionTable;
                u32 transitionAddress;
                transitionTable = sUnk_8639420;
                ASM_BARRIER(: : "r"(transitionTable));
                transitionAddress = directionOffset + ((*currentPassagePointer) << 4);
                transitionAddress += (u32)transitionTable;
                passageOrTransition = *(const s32*)transitionAddress;
            }
            {
                register u8* transitionStore PIN("r1");
                transitionStore = (u8*)transitionPointer;
                *transitionStore = passageOrTransition;
            }
            {
                register u8* transitionRead PIN("r2");
                const s32* fallingTable;
                fallingTable = sUnk_8639490;
                transitionRead = (u8*)transitionPointer;
                *(u8*)fallingOrDestination = fallingTable[*transitionRead];
            }

            byteOffset = directionOffset + ((*currentPassagePointer) << 4);
            mode = *(u8*)mapModeOrDurationTable;
            tableValue = (mode << 3) - mode;
            tableValue <<= 4;
            byteOffset += tableValue;
            byteOffset += passageTableOrXSpeed;
            tableValue = *(const s32*)byteOffset;
            *currentPassagePointer = tableValue;

            fallingOrDestination = (u32)&gUnk_3003C70;
            {
                s32 numerator;

                numerator = (sUnk_86392A4[*currentPassagePointer] - sUnk_86392A4[*(u8*)directionOrPrevious]) << 4;
                mapModeOrDurationTable = (u32)sUnk_86394C8;
                {
                    register u8* transitionRead PIN("r2");
                    transitionRead = (u8*)transitionPointer;
                    *(u16*)fallingOrDestination = numerator / ((const s32*)mapModeOrDurationTable)[*transitionRead];
                }
            }

            {
                register u8* transitionRead PIN("r2");
                s32 denominator;
                s32 numerator;

                numerator = (sUnk_863926C[*currentPassagePointer] - sUnk_863926C[*(u8*)directionOrPrevious]) << 5;
                transitionRead = (u8*)transitionPointer;
                denominator = ((const s32*)mapModeOrDurationTable)[*transitionRead];
                passageTableOrXSpeed = numerator / denominator;
            }
            {
                register u32 xValue PIN("r1");
                register u16* xDestination PIN("r0");
                xValue = passageTableOrXSpeed;
                xDestination = &gUnk_3003C46;
                *xDestination = xValue;
            }

            fallingOrDestination = (u32)&gUnk_3003C48;
            {
                register u8* transitionRead PIN("r2");
                s32 denominator;
                s32 numerator;

                numerator = (sUnk_8639288[*currentPassagePointer] - sUnk_8639288[*(u8*)directionOrPrevious]) << 5;
                transitionRead = (u8*)transitionPointer;
                denominator = ((const s32*)mapModeOrDurationTable)[*transitionRead];
                *(u16*)fallingOrDestination = numerator / denominator;
            }

            passageOrTransition = (u8)passageOrTransition;
            if ((u32)passageOrTransition <= 3) {
                u32 transitionOffset;
                u16* xDestination;

                const s32* curveBase;
                register u8* transitionRead PIN("r2");
                curveBase = sUnk_8639500;
                transitionRead = (u8*)transitionPointer;
                transitionOffset = (*transitionRead) << 2;
                passageOrTransition = *(const s32*)(transitionOffset + (u32)curveBase);
                passageOrTransition -= ((gUnk_3003C6F << 1) * passageOrTransition) /
                                       *(const s32*)(mapModeOrDurationTable + transitionOffset);
                passageOrTransition += passageTableOrXSpeed;
                xDestination = &gUnk_3003C46;
                *xDestination = passageOrTransition;
            }

            gUnk_3003C45 = 1;
            BuildTemporaryStageSelectionSave();
        } else {
            gSoundRoomMessageTimer = 1000;
        }
    }

    return 0;
}
void UpdateMapScreenBackgroundsAndText(void)
{
    register s32 scroll PIN("r1");
    vu16* reg;
    vu16* hOffset;
    const u16* text;
    u32 language;

    scroll = UpdateStageEntryHorizontalShake();
    hOffset = (vu16*)0x04000012;
    scroll = ((s16)gUnk_3003C6C >> 4) + scroll;
    *hOffset = scroll;

    reg = (vu16*)0x04000010;
    *reg = 0;
    reg += 3;
    *reg = scroll;
    reg -= 1;
    *reg = 0;
    reg += 3;
    *reg = scroll;
    reg -= 1;
    *reg = 0;

    language = gLanguage;
    switch (language) {
        case 0:
            text = GP(sFileSelectStageNameTableLanguage0[gCurrentPassage]);
            Shop_DrawText(text, 0x7800, 26);
            break;

        case 1:
            text = GP(sFileSelectStageNameTableLanguage1[gCurrentPassage]);
            Shop_DrawText(text, 0x7800, 26);
            break;
    }

    *(vu16*)0x04000040 = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
    *(vu16*)0x04000044 = 160;
}

void ApplyMapScreenScrollVelocity(void)
{
    if (gUnk_3003C6E <= 3) {
        gUnk_3003C6C += gUnk_3003C70;
    }
}

void UpdateMapScreenPassageTransition(void)
{
    s32 duration;
    s32 speed;
    s32 curve;
    u8* transitionIndex;
    const s32* durationTable;
    u16* xVelocity;
    u32 transition;

    transitionIndex = &gUnk_3003C6E;
    transition = *transitionIndex;
    ASM_BARRIER(: "+r"(transition) : : "memory");
    durationTable = sUnk_86394C8;
    xVelocity = &gUnk_3003C46;

    if (transition <= 3) {
        s32 passage;
        u32 position;
        u32 temp;
        u32 positions;

        positions = (u32)sUnk_863926C;
        position = (gCurrentPassage << 2) + positions;
        temp = (gCurrentPassageTemp << 2) + positions;
        passage = *(s32*)position;
        temp = *(s32*)temp;
        passage -= temp;
        passage <<= 5;
        duration = durationTable[*transitionIndex];
        speed = passage / duration;
        curve = sUnk_8639500[*transitionIndex];
        curve -= ((gUnk_3003C6F << 1) * curve) / duration;
        curve += speed;
        *xVelocity = curve;
    }

    gStageEntryMainSpriteState[4] += *xVelocity;
    gStageEntryMainSpriteState[5] += gUnk_3003C48;
    gUnk_3003C6F++;

    if (gUnk_3003C6F == durationTable[*transitionIndex]) {
        gStageEntryMainSpriteState[4] = sUnk_863926C[gCurrentPassage] << 5;
        gStageEntryMainSpriteState[5] = sUnk_8639288[gCurrentPassage] << 5;
        gCurrentPassageTemp = gCurrentPassage;
        gUnk_3003C6C = sUnk_86392A4[gCurrentPassage] << 4;
        gUnk_3003C6F = 0;
        gUnk_3003C45 = 0;
    }
}

void UpdateMapScreenIntroSequence(void)
{
    switch (gStageEntrySequenceStep) {
        case 0:
            if (gUnk_3003C78[0] == 0) {
                m4aSongNumStart(0x1B2);
            }
            gUnk_3003C78[0]++;
            if (GP(gUnk_3003C64)[gUnk_3003C78[1]].time < gUnk_3003C78[0]) {
                gUnk_3003C78[0] = 1;
                gUnk_3003C78[1]++;
                if (GP(gUnk_3003C64)[gUnk_3003C78[1]].time == 0) {
                    gStageEntrySequenceStep++;
                    GP_SET(gUnk_3003C64, sUnk_8640454);
                    gUnk_3003C78[0] = 0;
                    gUnk_3003C78[1] = 0;
                    gStageEntryMainSpriteState[5] = 0x400;
                    gUnk_3003C48 = ((sUnk_8639288[0] - 32) << 5) / 40;
                    gUnk_3003C70 = 28;
                    m4aSongNumStart(0x1B3);
                }
            }
            break;

        case 1:
            gStageEntryMainSpriteState[5] += gUnk_3003C48;
            gUnk_3003C6C += gUnk_3003C70;
            AdvanceMapScreenWarioAnimation();
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer == 40) {
                gStageEntryMainSpriteState[4] = sUnk_863926C[0] << 5;
                gStageEntryMainSpriteState[5] = sUnk_8639288[0] << 5;
                gUnk_3003C6C = 0x480;
                gStageEntrySequenceTimer = 0;
                gStageEntryShakeMode = 4;
                gStageEntryMainSpriteState[1] = 0;
                gStageEntryMainSpriteState[0] = 0;
                GP_SET(gUnk_3003C40, sUnk_86409E0);
                VoiceSetPlay(4);
                m4aSongNumStart(0x1B4);
                gStageEntrySequenceStep++;
            }
            break;

        case 2:
            gStageEntryMainSpriteState[0]++;
            if (GP(gUnk_3003C40)[gStageEntryMainSpriteState[1]].time < gStageEntryMainSpriteState[0]) {
                gStageEntryMainSpriteState[0] = 1;
                gStageEntryMainSpriteState[1]++;
                if (GP(gUnk_3003C40)[gStageEntryMainSpriteState[1]].time == 0) {
                    gStageEntryShakeMode = 0;
                    gStageEntryShakeTimer = 0;
                    GP_SET(gUnk_3003C40, sUnk_8640960);
                    gStageEntrySequenceStep++;
                    gStageEntryMainSpriteState[0] = 0;
                    gStageEntryMainSpriteState[1] = 0;
                }
            }
            break;

        case 3:
            gUnk_3003C78[0]++;
            if (GP(gUnk_3003C64)[gUnk_3003C78[1]].time < gUnk_3003C78[0]) {
                gUnk_3003C78[0] = 1;
                gUnk_3003C78[1]++;
                if ((gUnk_3003C78[1] == 1) && (GP(gUnk_3003C64) != sUnk_86404A4)) {
                    m4aSongNumStart(0x1B2);
                }
                if (GP(gUnk_3003C64)[gUnk_3003C78[1]].time == 0) {
                    GP_SET(gUnk_3003C64, sUnk_86404A4);
                    gUnk_3003C78[0] = 0;
                    gUnk_3003C78[1] = 0;
                }
            }

            gStageEntryMainSpriteState[0]++;
            if (GP(gUnk_3003C40)[gStageEntryMainSpriteState[1]].time < gStageEntryMainSpriteState[0]) {
                gStageEntryMainSpriteState[0] = 1;
                gStageEntryMainSpriteState[1]++;
                if ((gStageEntryMainSpriteState[1] == 1) || (gStageEntryMainSpriteState[1] == 3)) {
                    m4aSongNumStart(8);
                }
                if (GP(gUnk_3003C40)[gStageEntryMainSpriteState[1]].time == 0) {
                    GP_SET(gUnk_3003C40, sUnk_8640858);
                    gUnk_3003C94 = 0;
                    gStageEntrySequenceStep = 0;
                    gUnk_3003C78[0] = 0;
                    gUnk_3003C78[1] = 0;
                    gStageEntryMainSpriteState[0] = 0;
                    gStageEntryMainSpriteState[1] = 0;
                }
            }
            break;
    }
}
void UpdateMapScreenBossUnlockSequence(void)
{
    register u32 phase PIN("r3");

    switch (gStageEntrySequenceStep) {
        case 0:
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer > 60) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep++;
                gStageEntryShakeMode = 3;
                m4aSongNumStart(0x1AA);
            }
            break;

        case 1:
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer > 50) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep++;
                gStageEntryShakeMode = 2;
                gStageEntryMainSpriteFalling = 1;
                m4aSongNumStart(8);
                GP_SET(gUnk_3003C40, GP(sUnk_86392C0[gStageEntryMainSpriteFalling]));
            }
            break;

        case 2:
            gUnk_3003C92 += 5;
            if (gUnk_3003C92 > 0xDC0) {
                gStageEntryShakeMode = 4;
                gUnk_3003C88[0] -= 16;
                gStageEntrySequenceStep++;
            }
            break;

        case 3:
            gUnk_3003C88[1] -= 3;
            phase = gStageEntrySequenceTimer & 3;
            if (phase == 0) {
                gUnk_3003C88[0] += 32;
            } else if (phase == 2) {
                gUnk_3003C88[0] -= 32;
            }
            gStageEntrySequenceTimer++;

            if (gUnk_3003C88[1] < (sUnk_863951C[1] << 4)) {
                gStageEntryShakeTimer = 0;
                gStageEntryShakeMode = 1;
                gUnk_3003C88[1] = sUnk_863951C[1] << 4;
                gUnk_3003C88[0] = sUnk_8639510[1] << 4;
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep++;
            }
            break;

        case 4:
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer > 40) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep++;
            }
            break;

        case 5:
            gStageEntryWindowLeft--;
            gStageEntryWindowRight++;
            if ((s16)gStageEntryWindowRight > 240) {
                gStageEntryWindowLeft = 0;
                gStageEntryWindowRight = 240;
                gUnk_3003C94 = 0;
                gStageEntrySequenceStep = 0;
            }
            break;
    }
}

void UpdateMapScreenFinalPassageSequence(void)
{
    register u32 phase PIN("r3");
    register s32 velocity PIN("r2");

    switch (gStageEntrySequenceStep) {
        case 0:
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer > 80) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep++;
                gUnk_3003C88[0] -= 16;
                gStageEntryShakeMode = 4;
            }
            break;

        case 1:
            gUnk_3003C88[1] -= 2;
            phase = gStageEntrySequenceTimer & 3;
            if (phase == 0) {
                gUnk_3003C88[0] += 32;
            } else if (phase == 2) {
                gUnk_3003C88[0] -= 32;
            }
            gStageEntrySequenceTimer++;

            if (gUnk_3003C88[1] < (sUnk_863951C[2] << 4)) {
                u32* shakeMode;
                u32 zero2;
                u32 zero1;

                shakeMode = &gStageEntryShakeMode;
                zero2 = 0;
                zero1 = 0;
                gUnk_3003C88[1] = sUnk_863951C[2] << 4;
                gUnk_3003C88[0] = sUnk_8639510[2] << 4;
                gStageEntrySequenceTimer = zero2;
                gStageEntrySequenceStep++;
                gStageEntryShakeTimer = zero1;
                *shakeMode = 1;
                GP_SET(gUnk_3003C68, sUnk_863FD1C);
                gUnk_3003C88[2] = zero2;
                gUnk_3003C88[3] = zero2;
                m4aSongNumStart(0x1C9);
                gUnk_3003C97 = 1;
            }
            break;

        case 2:
            gUnk_3003C88[2]++;
            if (GP(gUnk_3003C68)[gUnk_3003C88[3]].time < gUnk_3003C88[2]) {
                gUnk_3003C88[2] = 1;
                gUnk_3003C88[3]++;
                if (gUnk_3003C88[3] == 3) {
                    gStageEntryShakeMode = 1;
                }
                if (GP(gUnk_3003C68)[gUnk_3003C88[3]].time == 0) {
                    gUnk_3003C88[3] = 0;
                    GP_SET(gUnk_3003C68, sUnk_8639528[2]);
                    gStageEntrySequenceStep++;
                    {
                        u16* velocityPtr;
                        register s32 storedVelocity PIN("r0");

                        velocityPtr = &gUnk_3003C48;
                        velocity = 33;
                        velocity = -velocity;
                        ASM_BARRIER(: "+r"(velocity));
                        storedVelocity = velocity;
                        *velocityPtr = storedVelocity;
                    }
                    gUnk_3003C70 = 16;
                }
            }
            break;

        case 3: {
            register u32 yPosition PIN("r1");

            gStageEntryMainSpriteState[5] += gUnk_3003C48;
            ASM_BARRIER(: "=r"(yPosition));
            gUnk_3003C6C += gUnk_3003C70;
            ASM_BARRIER(: : "r"(yPosition));
        }
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer == 40) {
                gStageEntryMainSpriteState[4] = sUnk_863926C[gCurrentPassage] << 5;
                gStageEntryMainSpriteState[5] = sUnk_8639288[gCurrentPassage] << 5;
                gUnk_3003C6C = 0x480;
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                gUnk_3003C94 = 0;
            }
            break;
    }
}
int UpdateMapScreenStageStartTransition(void)
{
    u16* velocity;

    switch (gStageEntrySequenceStep) {
        case 0:
            gStageEntryMainSpriteFalling = sUnk_86395FC[gCurrentPassage];
            GP_SET(gUnk_3003C40, GP(sUnk_86392C0[gStageEntryMainSpriteFalling]));

            velocity = &gUnk_3003C46;
            *velocity = ((sUnk_86395C4[gCurrentPassage] - sUnk_863926C[gCurrentPassage]) << 5) / 12;

            velocity = &gUnk_3003C48;
            *velocity = ((sUnk_86395E0[gCurrentPassage] - sUnk_8639288[gCurrentPassage]) << 5) / 12;

            gStageEntrySequenceStep++;

        case 1:
            gStageEntryMainSpriteState[4] += gUnk_3003C46;
            gStageEntryMainSpriteState[5] += gUnk_3003C48;
            gStageEntrySequenceTimer++;
            if (gStageEntrySequenceTimer == 12) {
                gStageEntrySequenceTimer = 0;
                gStageEntrySequenceStep = 0;
                return 1;
            }
            break;
    }

    return 0;
}

void AdvanceMapScreenWarioAnimation(void)
{
    u16* state;
    const struct AnimationFrame* animation;
    register u32 timer PIN("r0");
    u32 frame;

    state = gStageEntryMainSpriteState;
    timer = state[0];
    timer += 1;
    state[0] = timer;
    frame = state[1];
    animation = GP(gUnk_3003C40);
    frame <<= 3;
    frame += (u32)animation;
    frame = *(u8*)(frame + 4);
    timer = (u16)timer;

    if (frame < timer) {
        timer = 1;
        state[0] = timer;
        timer = state[1];
        timer += 1;
        state[1] = timer;
        timer = (u16)timer;
        if (timer == 1 || timer == 3 || timer == 5) {
            timer = gUnk_3003C45;
            if (timer != 0) {
                timer = 1;
                m4aSongNumStart(timer);
            }
        }

        {
            u16* state2;
            const struct AnimationFrame* animation2;

            state2 = gStageEntryMainSpriteState;
            timer = state2[1];
            animation2 = GP(gUnk_3003C40);
            timer <<= 3;
            timer += (u32)animation2;
            timer = *(u8*)(timer + 4);
            if (timer == 0) {
                state2[1] = timer;
            }
        }
    }
}

void AdvanceMapScreenDoorAnimation(void)
{
    u16* state;
    const struct AnimationFrame* animation;
    register u32 timer PIN("r0");
    u32 duration;
    u32 frame;

    state = gUnk_3003C78;
    timer = state[0];
    timer += 1;
    state[0] = timer;
    frame = state[1];
    animation = GP(gUnk_3003C64);
    duration = animation[frame].time;

    if (duration < (u16)timer) {
        state[0] = 1;
        timer = state[1];
        timer += 1;
        state[1] = timer;
        timer = state[1];
        animation = (const struct AnimationFrame*)(u32)animation[timer].time;
        if ((u32)animation == 0) {
            state[1] = (u32)animation;
        }
    }
}

void UpdateMapScreenPaletteCycle(void)
{
    volatile u8* ptr;
    register u32 index PIN("r0");
    vu32* dma;
    u32 temp;

    ptr = &gUnk_3003C96;
    index = *ptr;
    if (index > 5) {
        index = 0;
        *ptr = index;
    }

    index = *ptr;
    if (index > 3) {
        temp = index;
        index = 6;
        index -= temp;
    } else {
        index = *ptr;
    }

    dma = (vu32*)0x040000D4;
    index <<= 3;
    dma[0] = (u32)((u8*)sUnk_864BD58 + index);
    dma[1] = 0x05000012;
    dma[2] = 0x80000004;
    dma[2];
}

void UpdateMapScreenTreasurePalettes(void)
{
    u16* timerPtr;
    register s32 phase PIN("r3");
    register u8* collection PIN("r4");
    vu32* dma;
    register u32 mask PIN("r5");
    u32 test;
    register u32 byte PIN("r1");

    timerPtr = &gStageTransitionPaletteCycleTimer;
    test = *timerPtr;
    test += 1;
    *timerPtr = test;
    test = *timerPtr;
    byte = 10;
    phase = test / byte;
    phase = (u16)phase;
    if (phase > 11) {
        test = 0;
        *timerPtr = test;
        phase = 0;
    }

    if (phase > 6) {
        test = 12;
        phase = test - phase;
    }

    collection = gCurrentCollection;
    test = (u32)(collection + 0x70);
    byte = *(u8*)test;
    mask = 0x20;
    test = mask;
    test &= byte;
    if (test != 0) {
        dma = (vu32*)0x040000D4;
        test = phase << 1;
        byte = (u32)sBossTreasureGoldenPaletteCycle;
        test += byte;
        dma[0] = test;
        test = 0x050000DC;
        dma[1] = test;
        test = 0x80000001;
        dma[2] = test;
        test = dma[2];
    }

    test = (u32)(collection + 0x40);
    byte = *(u8*)test;
    test = mask;
    test &= byte;
    if (test != 0) {
        dma = (vu32*)0x040000D4;
        test = phase << 1;
        byte = (u32)sBossTreasureSapphirePaletteCycle;
        test += byte;
        dma[0] = test;
        test = 0x050000FC;
        dma[1] = test;
        test = 0x80000001;
        dma[2] = test;
        test = dma[2];
    }

    test = (u32)(collection + 0x28);
    byte = *(u8*)test;
    test = mask;
    test &= byte;
    if (test != 0) {
        dma = (vu32*)0x040000D4;
        test = phase << 1;
        byte = (u32)sBossTreasureEmeraldPaletteCycle;
        test += byte;
        dma[0] = test;
        test = 0x0500011C;
        dma[1] = test;
        test = 0x80000001;
        dma[2] = test;
        test = dma[2];
    }

    test = (u32)(collection + 0x58);
    byte = *(u8*)test;
    test = mask;
    test &= byte;
    if (test != 0) {
        dma = (vu32*)0x040000D4;
        test = phase << 1;
        byte = (u32)sBossTreasureRubyPaletteCycle;
        test += byte;
        dma[0] = test;
        test = 0x0500013C;
        dma[1] = test;
        test = 0x80000001;
        dma[2] = test;
        test = dma[2];
    }
}

void DrawMapScreenSprites(void)
{
    const struct AnimationFrame* animation;
    const u16* frame;
    u16* copyRaw;
    OamData* screenDst;
    s32 drawn;
    register s32 used PIN("r6");
    s32 count;
    u16* attr2Ptr;
    s32 remaining;
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 low;
    u16* state;
    int attrByte;
    s32 yBase = 90;
    u8* slotsPtr = &gOamSlotsUsed;

    drawn = 0;
    {
        register u8* slotsRead PIN("r4") = &gOamSlotsUsed;
        u32 oamByteOffset;
        used = *(volatile u8*)slotsRead;
        oamByteOffset = (u32)used << 3;
        {
            register OamData* oamStart PIN("r0") = gOamBuffer;
            copyRaw = (u16*)(oamByteOffset + (u32)oamStart);
        }
    }
    {
        u16* animState = gUnk_3003C74;
        animState[0]++;
        if (sUnk_863FFAC[animState[1]].time < animState[0]) {
            animState[0] = 1;
            animState[1]++;
            gUnk_3003C96++;
            if (sUnk_863FFAC[animState[1]].time == 0) {
                animState[1] = 0;
            }
        }
    }
    {
        u16* frameState;
        frameState = gUnk_3003C74;
        frame = GP(sUnk_863FFAC[frameState[1]].oam);
    }
    do {
        count = *frame;
        used += count;
        frame++;
        if (used > 128) {
            goto overflow;
        }
        if (drawn < used) {
            u32 xMask;
            u32 highMask;
            s32 modeMask;
            OamData* oamBase;
            oamBase = gOamBuffer;
            {
                u32 xMaskLoad = 0x1FF;
                xMask = xMaskLoad;
            }
            highMask = 0xFFFFFE00;
            {
                s32 modeMaskLoad = -13;
                // Couldn't reproduce this part in pure C, so I had to use inline assembly to get the same result.
                modeMask = modeMaskLoad;
            }
            screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBase);
            remaining = used - drawn;
            do {
                attr0 = *(frame++);
                *(copyRaw++) = attr0;
                screenDst->split.y = (((s32)attr0) + yBase) - (((s16)gUnk_3003C6C) >> 4);
                attr1 = *(frame++);
                *(copyRaw++) = attr1;
                low = (attr1 + 120) & xMask;
                screenDst->all.attr1 = (screenDst->all.attr1 & highMask) | ((attr1 + 120) & xMask);
                attr2 = *frame;
                *(copyRaw++) = attr2;
                frame++;
                {
                    register u32 oldAttrByte PIN("r1");
                    register u32 maskedByte PIN("r0");
                    oldAttrByte = ((u8*)(&screenDst->all.attr2))[1];
                    maskedByte = modeMask;
                    maskedByte &= oldAttrByte;
                    maskedByte |= 8;
                    ((u8*)(&screenDst->all.attr2))[1] = maskedByte;
                }
                copyRaw++;
                screenDst++;
                remaining--;
            } while (remaining != 0);
            drawn = used;
        }
    } while (0);
    if (*((s16*)&gUnk_3003C6C) != 0) {
        u16* mainState;
        frame = GP(GP(gUnk_3003C40)[gStageEntryMainSpriteState[1]].oam);
        do {
            count = *frame;
            used += count;
            frame++;
            if (used > 128) {
                goto overflow;
            }
            if (drawn < used) {
                u32 xMask;
                u32 highMask;
                s32 modeMask;
                OamData* oamBaseMain;
                oamBaseMain = gOamBuffer;
                mainState = gStageEntryMainSpriteState;
                highMask = 0xFFFFFE00;
                modeMask = -13;
                screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBaseMain);
                xMask = 0x1FF;
                remaining = used - drawn;
                do {
                    attr0 = *(frame++);
                    *(copyRaw++) = attr0;
                    screenDst->split.y = ((s32)attr0) + (((s16)mainState[5]) >> 5);
                    attr1 = *(frame++);
                    *(copyRaw++) = attr1;
                    low = (attr1 + (((s16)mainState[4]) >> 5)) & xMask;
                    screenDst->all.attr1 =
                        (screenDst->all.attr1 & highMask) | ((attr1 + (((s16)mainState[4]) >> 5)) & xMask);
                    attr2 = *frame;
                    *(copyRaw++) = attr2;
                    frame++;
                    {
                        register u32 attrByteLocal PIN("r1");
                        register u32 modeValue PIN("r0");
                        attrByteLocal = ((u8*)(&screenDst->all.attr2))[1];
                        modeValue = (u32)modeMask;
                        modeValue &= attrByteLocal;
                        modeValue |= 8;
                        ((u8*)(&screenDst->all.attr2))[1] = modeValue;
                    }
                    copyRaw++;
                    screenDst++;
                    remaining--;
                } while (remaining != 0);
                drawn = used;
            }
        } while (0);
    }
    if (gUnk_3003C95 == 2) {
        gUnk_3003C84[0]++;
        if (sUnk_8640C68[gUnk_3003C84[1]].time < gUnk_3003C84[0]) {
            gUnk_3003C84[0] = 1;
            gUnk_3003C84[1]++;
            if (sUnk_8640C68[gUnk_3003C84[1]].time == 0) {
                gUnk_3003C84[1] = 0;
            }
        }
        frame = GP(sUnk_8640C68[gUnk_3003C84[1]].oam);
        do {
            count = *frame;
            used += count;
            frame++;
            if (used > 128) {
                goto overflow;
            }
            if (drawn < used) {
                u32 xMask;
                u32 highMask;
                s32 modeMask;
                OamData* oamBase;
                oamBase = gOamBuffer;
                xMask = 0x1FF;
                highMask = 0xFFFFFE00;
                modeMask = -13;
                screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBase);
                remaining = used - drawn;
                do {
                    attr0 = *(frame++);
                    *(copyRaw++) = attr0;
                    {
                        s32 y160 = 160;
                        screenDst->split.y = (((s32)attr0) + y160) - (((s16)gUnk_3003C6C) >> 4);
                    }
                    attr1 = *(frame++);
                    *(copyRaw++) = attr1;
                    {
                        u32 low84;
                        low84 = (attr1 + 120) & xMask;
                        screenDst->all.attr1 = (screenDst->all.attr1 & highMask) | low84;
                    }
                    attr2 = *frame;
                    *(copyRaw++) = attr2;
                    frame++;
                    {
                        register u32 attrByteLocal PIN("r1");
                        register u32 modeValue PIN("r0");
                        attrByteLocal = ((u8*)(&screenDst->all.attr2))[1];
                        modeValue = (u32)modeMask;
                        modeValue &= attrByteLocal;
                        modeValue |= 8;
                        ((u8*)(&screenDst->all.attr2))[1] = modeValue;
                    }
                    copyRaw++;
                    screenDst++;
                    remaining--;
                } while (remaining != 0);
                drawn = used;
            }
        } while (0);
    }
    if (gUnk_3003C97 != 0) {
        gUnk_3003C80[0]++;
        if (sUnk_8641070[gUnk_3003C80[1]].time < gUnk_3003C80[0]) {
            gUnk_3003C80[0] = 1;
            gUnk_3003C80[1]++;
            if (sUnk_8641070[gUnk_3003C80[1]].time == 0) {
                gUnk_3003C80[1] = 0;
                gUnk_3003C97 = 0;
            }
        }
        if (gUnk_3003C97 != 0) {
            s32 i;
            {
                s32 indexInit;
                indexInit = 0;
                i = indexInit;
            }
            do {
                frame = GP(sUnk_8641070[gUnk_3003C80[1]].oam);
                do {
                    count = *frame;
                    used += count;
                    frame++;
                    if (used > 128) {
                        goto overflow;
                    }
                    if (drawn < used) {
                        register s32 mask PIN("r0");
                        register u32 yValue PIN("r8");
                        register u32 xValue PIN("r12");
                        register u32 lowLocal PIN("r1");
                        {
                            const u32* yTable;
                            u32 tableOffset;
                            yTable = (const u32*)sUnk_86395B4;
                            tableOffset = i << 2;
                            yValue = *(const u32*)((const u8*)yTable + tableOffset);
                            {
                                u32 oamOffset;
                                register OamData* oamBaseLocal PIN("r2");
                                oamOffset = drawn << 3;
                                oamBaseLocal = gOamBuffer;
                                screenDst = (OamData*)(oamOffset - -(int)oamBaseLocal);
                            }
                            xValue = *(const u32*)((const u8*)sUnk_86395A4 + tableOffset);
                        }
                        remaining = used - drawn;
                        do {
                            attr0 = *(frame++);
                            *(copyRaw++) = attr0;
                            screenDst->split.y = (((s32)attr0) + yValue) - (((s16)gUnk_3003C6C) >> 4);
                            attr1 = *(frame++);
                            *(copyRaw++) = attr1;
                            lowLocal = attr1 + xValue;
                            mask = 0x1FF;
                            lowLocal &= mask;
                            {
                                u16 oldAttr1;
                                oldAttr1 = screenDst->all.attr1;
                                mask = 0xFFFFFE00;
                                mask &= oldAttr1;
                                mask |= lowLocal;
                                screenDst->all.attr1 = mask;
                            }
                            attr2 = *frame;
                            *(copyRaw++) = attr2;
                            frame++;
                            attrByte = ((u8*)(&screenDst->all.attr2))[1];
                            mask = -13;
                            mask &= attrByte;
                            mask |= 8;
                            ((u8*)(&screenDst->all.attr2))[1] = mask;
                            copyRaw++;
                            screenDst++;
                            remaining--;
                        } while (remaining != 0);
                        drawn = used;
                    }
                } while (0);
                i++;
            } while (i <= 3);
        }
    }
    if ((gUnk_3003C94 != 4) || (gStageEntrySequenceStep != 2)) {
        gUnk_3003C88[2]++;
        if (GP(gUnk_3003C68)[gUnk_3003C88[3]].time < gUnk_3003C88[2]) {
            gUnk_3003C88[2] = 1;
            gUnk_3003C88[3]++;
            if (GP(gUnk_3003C68)[gUnk_3003C88[3]].time == 0) {
                gUnk_3003C88[3] = 0;
            }
        }
    }
    if (gUnk_3003C95 != 0) {
        u16* spriteState;
        spriteState = gUnk_3003C88;
        frame = GP(GP(gUnk_3003C68)[spriteState[3]].oam);
        do {
            count = *frame;
            used += count;
            frame++;
            if (used > 128) {
                goto overflow;
            }
            if (drawn < used) {
                u16* spritePos;
                u32 xMask;
                u32 highMask;
                s32 modeMask;
                OamData* oamBase;
                oamBase = gOamBuffer;
                spritePos = spriteState;
                highMask = 0xFFFFFE00;
                screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBase);
                xMask = 0x1FF;
                modeMask = -13;
                remaining = used - drawn;
                do {
                    attr0 = *(frame++);
                    *(copyRaw++) = attr0;
                    screenDst->split.y = (((s32)attr0) + (spritePos[1] >> 4)) - (((s16)gUnk_3003C6C) >> 4);
                    attr1 = *(frame++);
                    *(copyRaw++) = attr1;
                    low = (attr1 + (spritePos[0] >> 4)) & xMask;
                    {
                        register u16 oldAttr1Local PIN("r2");
                        register u32 mergedAttr1Local PIN("r0");
                        oldAttr1Local = screenDst->all.attr1;
                        mergedAttr1Local = highMask;
                        mergedAttr1Local &= oldAttr1Local;
                        mergedAttr1Local |= low;
                        screenDst->all.attr1 = mergedAttr1Local;
                    }
                    attr2 = *frame;
                    *(copyRaw++) = attr2;
                    frame++;
                    ((u8*)(&screenDst->all.attr2))[1] |= 12;
                    copyRaw++;
                    screenDst++;
                    remaining--;
                } while (remaining != 0);
                drawn = used;
            }
        } while (0);
    }
    if (*((s16*)&gUnk_3003C6C) == 0) {
        u16* mainStateZero;
        frame = GP(GP(gUnk_3003C40)[gStageEntryMainSpriteState[1]].oam);
        do {
            count = *frame;
            used += count;
            frame++;
            if (used > 128) {
                goto overflow;
            }
            if (drawn < used) {
                u32 xMask;
                u32 highMask;
                OamData* oamBaseZero;
                oamBaseZero = gOamBuffer;
                mainStateZero = gStageEntryMainSpriteState;
                highMask = 0xFFFFFE00;
                screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBaseZero);
                xMask = 0x1FF;
                remaining = used - drawn;
                do {
                    attr0 = *(frame++);
                    *(copyRaw++) = attr0;
                    screenDst->split.y = ((s32)attr0) + (((s16)mainStateZero[5]) >> 5);
                    attr1 = *(frame++);
                    *(copyRaw++) = attr1;
                    low = (attr1 + (((s16)mainStateZero[4]) >> 5)) & xMask;
                    {
                        register u16 oldAttr1Zero PIN("r2");
                        s32 mergedAttr1;
                        oldAttr1Zero = screenDst->all.attr1;
                        mergedAttr1 = (oldAttr1Zero & highMask) | low;
                        screenDst->all.attr1 = mergedAttr1;
                    }
                    attr2 = *frame;
                    *(copyRaw++) = attr2;
                    frame++;
                    ((u8*)(&screenDst->all.attr2))[1] |= 12;
                    copyRaw++;
                    screenDst++;
                    remaining--;
                } while (remaining != 0);
                drawn = used;
            }
        } while (0);
    }
    {
        register u8* stageSelectPtr PIN("r2");
        stageSelectPtr = &gUnk_3003C60;
        if (*stageSelectPtr <= 2) {
            state = gUnk_3003C7C;
            {
                register u16 nextTime PIN("r1");
                nextTime = state[0];
                nextTime++;
                state[0] = nextTime;
                animation = sUnk_8640B28;
                if (animation[state[1]].time < nextTime) {
                    state[0] = 1;
                    state[1]++;
                    if (animation[state[1]].time == 0) {
                        state[1] = 0;
                    }
                }
            }
            frame = GP(animation[state[1]].oam);
            do {
                count = *frame;
                used += count;
                frame++;
                if (used > 128) {
                    goto overflow;
                }
                if (drawn < used) {
                    u8* stagePtrLocal;
                    register const u32* xTableLocal PIN("sl");
                    register u32 xMask PIN("r9");
                    register u32 highMask PIN("r12");
                    OamData* oamBaseLe;
                    s32 modeMask;
                    oamBaseLe = gOamBuffer;
                    stagePtrLocal = &gUnk_3003C60;
                    xTableLocal = (const u32*)sUnk_8639534;
                    highMask = 0xFFFFFE00;
                    screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBaseLe);
                    xMask = 0x1FF;
                    modeMask = -13;
                    remaining = used - drawn;
                    do {
                        attr0 = *(frame++);
                        *(copyRaw++) = attr0;
                        {
                            u8* stageReadY;
                            stageReadY = stagePtrLocal;
                            screenDst->split.y =
                                (((s32)attr0) + ((const u32*)sUnk_8639550)[*stageReadY]) - (((s16)gUnk_3003C6C) >> 4);
                        }
                        attr1 = *(frame++);
                        *(copyRaw++) = attr1;
                        {
                            u8* stageReadX;
                            register const u32* xEntry PIN("r0");
                            u16 oldAttr1Local;
                            register u32 merged PIN("r0");
                            stageReadX = stagePtrLocal;
                            xEntry = xTableLocal + *stageReadX;
                            low = attr1 + *xEntry;
                            low &= xMask;
                            oldAttr1Local = screenDst->all.attr1;
                            ASM_BARRIER(: : "r"(xEntry));
                            merged = highMask;
                            merged &= oldAttr1Local;
                            merged |= low;
                            screenDst->all.attr1 = merged;
                        }
                        attr2 = *frame;
                        *(copyRaw++) = attr2;
                        frame++;
                        ((u8*)(&screenDst->all.attr2))[1] |= 12;
                        copyRaw++;
                        screenDst++;
                        remaining--;
                    } while (remaining != 0);
                    drawn = used;
                }
            } while (0);
        } else {
            animation = GP(sUnk_8639618[*stageSelectPtr - 3]);
            state = gUnk_3003C7C;
            state[0]++;
            if (animation[state[1]].time < state[0]) {
                state[0] = 1;
                state[1]++;
                if (animation[state[1]].time == 0) {
                    state[1] = 0;
                }
            }
            frame = GP(animation[state[1]].oam);
            do {
                count = *frame;
                used += count;
                frame++;
                if (used > 128) {
                    goto overflow;
                }
                if (drawn < used) {
                    OamData* oamBaseLate;
                    register u32 highMask PIN("sl");
                    s32 modeMask;
                    u8* stagePtrLate;
                    const u32* yTableLate;
                    const u32* xTableLate;
                    yTableLate = (const u32*)sUnk_8639550;
                    stagePtrLate = &gUnk_3003C60;
                    xTableLate = (const u32*)sUnk_8639534;
                    highMask = 0xFFFFFE00;
                    oamBaseLate = gOamBuffer;
                    modeMask = -13;
                    screenDst = &oamBaseLate[drawn];
                    do {
                        attr0 = *(frame++);
                        *(copyRaw++) = attr0;
                        {
                            u32 stageIndexY;
                            stageIndexY = *stagePtrLate;
                            stageIndexY -= 3;
                            screenDst->split.y = (((s32)attr0) + yTableLate[stageIndexY]) - (((s16)gUnk_3003C6C) >> 4);
                        }
                        attr1 = *(frame++);
                        *(copyRaw++) = attr1;
                        {
                            u32 stageIndexX;
                            u32 lowLocal;
                            u32 xMaskLocal;
                            stageIndexX = *stagePtrLate;
                            stageIndexX -= 3;
                            lowLocal = attr1 + xTableLate[stageIndexX];
                            xMaskLocal = 0x1FF;
                            lowLocal &= xMaskLocal;
                            low = lowLocal;
                        }
                        {
                            register u16 oldAttr1Local PIN("r2");
                            register u32 mergedAttr1Local PIN("r0");
                            oldAttr1Local = screenDst->all.attr1;
                            mergedAttr1Local = highMask;
                            mergedAttr1Local &= oldAttr1Local;
                            mergedAttr1Local |= low;
                            screenDst->all.attr1 = mergedAttr1Local;
                        }
                        attr2 = *frame;
                        *(copyRaw++) = attr2;
                        frame++;
                        ((u8*)(attr2Ptr = &screenDst->all.attr2))[1] |= 12;
                        copyRaw++;
                        screenDst++;
                        drawn++;
                    } while (drawn < used);
                }
            } while (0);
        }
    }
    {
        u8* modePtrCheck;
        modePtrCheck = &gUnk_3003C94;
        if (*modePtrCheck <= 3) {
            frame = GP(GP(gUnk_3003C64)[gUnk_3003C78[1]].oam);
            do {
                count = *frame;
                used += count;
                frame++;
                if (used > 128) {
                    goto overflow;
                }
                if (drawn < used) {
                    u32 xMask;
                    u32 highMask;
                    s32 modeMask;
                    OamData* oamBaseMode2;
                    oamBaseMode2 = gOamBuffer;
                    {
                        highMask = 0xFFFFFE00;
                    }
                    modeMask = -13;
                    screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBaseMode2);
                    {
                        u32 xMaskLow;
                        xMaskLow = 0x1FF;
                        xMask = xMaskLow;
                    }
                    remaining = used - drawn;
                    do {
                        attr0 = *(frame++);
                        *(copyRaw++) = attr0;
                        {
                            s32 attr0Late = attr0;
                            screenDst->split.y = (attr0Late + 48) - (((s16)gUnk_3003C6C) >> 4);
                        }
                        attr1 = *(frame++);
                        *(copyRaw++) = attr1;
                        low = (attr1 + 120) & xMask;
                        {
                            register u16 oldAttr1Local PIN("r2");
                            register u32 merged PIN("r0");
                            oldAttr1Local = screenDst->all.attr1;
                            merged = highMask;
                            merged &= oldAttr1Local;
                            merged |= low;
                            screenDst->all.attr1 = merged;
                        }
                        attr2 = *frame;
                        *copyRaw = attr2;
                        frame++;
                        copyRaw++;
                        {
                            u32 attrByteLocal;
                            attrByteLocal = ((u8*)(&screenDst->all.attr2))[1] | 12;
                            ((u8*)(&screenDst->all.attr2))[1] = attrByteLocal;
                            if (gUnk_3003C94 == 1) {
                                attrByteLocal &= modeMask;
                                attrByteLocal |= 8;
                                ((u8*)(&screenDst->all.attr2))[1] = attrByteLocal;
                            }
                        }
                        copyRaw++;
                        screenDst++;
                        remaining--;
                    } while (remaining != 0);
                    drawn = used;
                }
            } while (0);
        }
    }
    if ((gUnk_3003C95 == 0) || (gUnk_3003C94 == 2)) {
        frame = sUnk_8640A58;
        do {
            count = *frame;
            used += count;
            frame++;
            if (used > 128) {
                goto overflow;
            }
            if (drawn < used) {
                u32 xMask;
                u32 highMask;
                OamData* oamBase;
                u16* yPosPtr;
                u16* xPosPtr;
                oamBase = gOamBuffer;
                xMask = 0x1FF;
                highMask = 0xFFFFFE00;
                yPosPtr = &gUnk_3003C92;
                xPosPtr = &gUnk_3003C90;
                screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBase);
                remaining = used - drawn;
                do {
                    attr0 = *(frame++);
                    *(copyRaw++) = attr0;
                    screenDst->split.y = ((s32)attr0) + ((((s32)*yPosPtr) - ((s16)gUnk_3003C6C)) >> 4);
                    attr1 = *(frame++);
                    *(copyRaw++) = attr1;
                    low = (attr1 + ((s16)*xPosPtr)) & xMask;
                    screenDst->all.attr1 = (screenDst->all.attr1 & highMask) | ((attr1 + ((s16)*xPosPtr)) & xMask);
                    attr2 = *frame;
                    *(copyRaw++) = attr2;
                    frame++;
                    ((u8*)(&screenDst->all.attr2))[1] |= 12;
                    copyRaw++;
                    screenDst++;
                    remaining--;
                } while (remaining != 0);
                drawn = used;
            }
        } while (0);
    }
    {
        register const struct AnimationFrame* finalAnimation PIN("r0");
        finalAnimation = sUnk_863FF84;
        frame = GP(finalAnimation[gUnk_3003C74[1]].oam);
    }
    do {
        count = *frame;
        used += count;
        frame++;
        if (used > 128) {
            goto overflow;
        }
        if (drawn < used) {
            u32 xMask;
            u32 highMask;
            s32 modeMask;
            OamData* oamBaseFinal;
            oamBaseFinal = gOamBuffer;
            xMask = 0x1FF;
            highMask = 0xFFFFFE00;
            screenDst = (OamData*)(((u32)drawn << 3) + (u32)oamBaseFinal);
            modeMask = -13;
            remaining = used - drawn;
            do {
                attr0 = *(frame++);
                *(copyRaw++) = attr0;
                screenDst->split.y = (((s32)attr0) + yBase) - (((s16)gUnk_3003C6C) >> 4);
                attr1 = *(frame++);
                *(copyRaw++) = attr1;
                low = (attr1 + 120) & xMask;
                screenDst->all.attr1 = (screenDst->all.attr1 & highMask) | ((attr1 + 120) & xMask);
                attr2 = *frame;
                *(copyRaw++) = attr2;
                frame++;
                ((u8*)(&screenDst->all.attr2))[1] |= 12;
                copyRaw++;
                screenDst++;
                remaining--;
            } while (remaining != 0);
            drawn = used;
        }
    } while (0);
    *slotsPtr = used;
overflow:
    return;
}

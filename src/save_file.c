#include "global_data.h"

extern u8 gHasTemporarySave;
extern s8 gCurrentStageID;
extern u8 gCurrentStageNumber;
extern u8 gCurrentPassage;
extern u8 gSelectedSaveFile;
extern u8 gSaveFlag;
extern u8 gUnk_3000014;
extern u8 gTitleScreenStyle;
extern u8 gUnk_300001D;

extern u32 sUnk_86391C4[][6];
extern const u8 sBackupSramTestSignature[];
extern const u8 sSaveFooterSignature[];
extern const u8 sMainSaveHeaderSignature1[];
extern const u8 sMainSaveHeaderSignature2[];
extern const u8 sUsTitleSaveVersionString[];
extern const u8 sSaveSlotValidationSignatures[];
extern const u8 sStageSelectionSaveSignature[];
extern GBA_PTR(u8*) const sPrimarySaveHeaderBuffers[];
extern GBA_PTR(u8*) const sSecondarySaveHeaderBuffers[];
extern GBA_PTR(u8*) const sMainSaveDataBuffers[];

extern u8 gLanguage;
extern u8 gUnk_300001C;
extern u8 gUnk_3003BF8[];
extern u32 gUnk_3000040;
extern const u8 sTemporarySaveHeaderSignature1[];
extern const u8 sTemporarySaveHeaderSignature2[];

typedef union AGB_ALIGN RegPair8 {
    unsigned long long q;
    struct AGB_ALIGN {
        u32 lo;
        u32 hi;
    } w;
} RegPair8;

void func_8000E94(void);
void func_8000EAC(void);
void func_8000CE0(void);
void func_8000FBC(void);
void func_800101C(void);
void func_8001038(void);
u32 ClassifyAndRepairSaveSlotCopy(u8 selectedFile);
u32 ValidateStageSelectionSaveSlot(u8 selectedFile);
u32 ValidateMainSaveSlot(u8 selectedFile, u32 statusValue);
u32 ValidateSelectedSaveHeader(void);
extern const u8 sPassageNameValidationStrings[];
extern const u8 sStageNameValidationStrings[];

void RestoreGameStateFromTemporarySave(void);
void SerializeGlobalSaveDataToWorkingBuffer(void);
void RestoreGlobalSaveDataFromWorkingBuffer(void);
void RestoreTempSaveStageSelection(void);
u32 ValidateAndClearSaveSlotCopy(u8 copy, u8 selectedFile);

void func_8000E54(void);
void func_8000EEC(void);
void func_8000FE8(void);
void func_8000F3C(void);
void func_8000F64(void);

void func_8000F90(u32 dmaControl);
void SramWriteUnchecked(u8* src, u8* dest, u32 size);
u32 SramWriteChecked(u8* src, u8* dest, u32 size);

extern u8 gStageEntryPauseMenuDisabled;
extern u8 gStageRoomTableIndex;
extern u32 gUnk_3000040;
extern const u8 sTemporarySaveHeaderSignature1[];
extern const u8 sTemporarySaveHeaderSignature2[];
extern GBA_PTR(u8*) const sStageSelectionSaveBuffers[];
extern u8 gDifficulty;
extern u16 gMedalCount;
extern u32 gTotalScore;
extern u8 gUnk_300001C;
extern u8 gCurrentCollection[];
extern u8 gHighScoreTable[];
extern u16 gMinigameHighScores[];
extern u8 gUnk_3003BF8[];

void SerializeGameStateToTemporarySave(void);
void func_8000EC4(void);

typedef struct AGB_ALIGN {
    u32 words[2];
} SaveBlock8;
typedef struct AGB_ALIGN {
    u32 words[3];
} SaveBlock12;
typedef struct AGB_ALIGN {
    u32 words[11];
} SaveBlock44;

extern u8 gUnk_3000025;
extern u8 gRoomTransitionTileDataCount;
extern u8 gTimerState;
extern u16 gClearedRoomTileCount;
extern u16 gUnk_300003A;
extern u16 gGoldenBossTransitionBg0Y;
extern SaveBlock8 gWarioDashAfterimage;
extern SaveBlock8 gCurrentCarriedSprite;
extern SaveBlock8 gWarioMotionAfterimage;
extern SaveBlock8 gCurrentWarioEffect;
extern u32 gHeartMeter;
extern u32 gHeartGauge;
extern SaveBlock12 gWarioDustEffect1;
extern SaveBlock12 gWarioDustEffect2;
extern SaveBlock44 gSpriteData[];
extern u8 gPersistentSpriteData[][64];
extern u8 gSwitchPressed;
extern u8 gCollectedNEJewelPiece;
extern u8 gCollectedSEJewelPiece;
extern u8 gCollectedSWJewelPiece;
extern u8 gUnk_3000C04;
extern u8 gUnk_3000C05;
extern u8 gEnemyScoreDropCounter;
extern u8 gCollectedNWJewelPiece;
extern u8 gCollectedCD;
extern u8 gCollectedKeyzer;
extern u8 gUnk_3000C0E;
extern u32 gStageScore;
extern u32 gStageFrameCounter;
extern u8 gSwitchStates[];
extern u8 gStageTimerDigits[];
extern s8 gScoreDigits[];
extern u16 gBg0XPosition;
extern u16 gBg0YPosition;
extern u16 gBg1XPosition;
extern u16 gBg1YPosition;
extern u16 gBg2XPosition;
extern u16 gBg2YPosition;
extern u16 gBg3XPosition;
extern u16 gBg3YPosition;
#ifdef PORTABLE
#include <string.h>
#else
void* memcpy(void* destination, const void* source, u32 size);
#endif

void BuildTitleScreenSaveHeader(void);
void ValidateAndRepairSaveHeaderCopies(void);
u32 ValidateSaveHeaderBlock(u8 saveType);
void InitializeTemporarySaveBuffer(void);
void SerializeGameStateToTemporarySave(void);
void RefreshTemporarySaveStageId(void);
void RestoreGameStateFromTemporarySave(void);
void BuildMainSaveWorkingBuffer(void);
void SerializeGlobalSaveDataToWorkingBuffer(void);
void RestoreGlobalSaveDataFromWorkingBuffer(void);
void CopySaveFileBlock16(u8* destination, const u8* source);
void CopySaveFileBlock8(u8* destination, const u8* source);
void RunBackupSramSelfTest(void);
void DmaLoadSelectedSaveFileBlocks(void);
void InitializeSaveFileStatuses(void);
u32 ClassifyAndRepairSaveSlotCopy(u8 selectedFile);
u32 ValidateAndClearSaveSlotCopy(u8 copy, u8 selectedFile);
u32 ValidateMainSaveSlot(u8 selectedFile, u32 statusValue);
void FlushSaveFileAndMaybeRestoreTempSave(void);
void FlushSaveFileAndRefreshSaveData(void);
void BuildSelectedSaveValidationHeader(void);
u32 ValidateSelectedSaveHeader(void);
void BuildTemporaryStageSelectionSave(void);
u32 ValidateStageSelectionSaveSlot(u8 selectedFile);
void RestoreTempSaveStageSelection(void);

void BuildTitleScreenSaveHeader(void)
{
    u8* data;
    u8* source;
    u32* checksumPtr;
    u32 checksum;
    u32 value;
    s32 index;

    data = (u8*)0x02038008;
    *data = 48;
    data++;
    *data = 49;
    data++;
    *data = gTitleScreenStyle;
    data++;
    *data = 0;
    data++;

    index = 0;
    source = sUsTitleSaveVersionString;
    while (index <= 19) {
        *data = *(u8*)(index + (u32)source);
        index++;
        data++;
    }

    checksumPtr = (u32*)0x02038000;
    value = 0;
    *checksumPtr++ = value;
    value--;
    *checksumPtr = value;
    checksumPtr--;

    checksum = 0;
    index = 3;
    while (index >= 0) {
        checksum += *checksumPtr++;
        checksum += *checksumPtr++;
        checksum += *checksumPtr++;
        checksum += *checksumPtr++;
        index--;
    }

    ((u32*)0x02038000)[0] = checksum;
    checksum = ~checksum;
    ((u32*)0x02038000)[1] = checksum;
    func_8000E54();
    gUnk_300001D = 1;
}

void ValidateAndRepairSaveHeaderCopies(void)
{
    u16 zero;
    u32 firstValid;
    u32 secondValid;
    u32 thirdValid;
    vu32* dma;
    u32* save0;
    u32 control;
    u32 clear;
    u16* zeroPtr;

    RunBackupSramSelfTest();
    if (gUnk_3000014 == 0) {
        func_8001038();
    }

    firstValid = ValidateSaveHeaderBlock(0);
    secondValid = ValidateSaveHeaderBlock(1);
    thirdValid = ValidateSaveHeaderBlock(2);

    if (firstValid != 0) {
        zeroPtr = &zero;
        clear = 0;
        *zeroPtr = clear;
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)zeroPtr;
        save0 = (u32*)0x02038000;
        dma[1] = (u32)save0;
        control = 0x81000020;
        dma[2] = control;
        (void)dma[2];

        if (thirdValid != 0) {
            zeroPtr = &zero;
            *zeroPtr = clear;
            dma[0] = (u32)zeroPtr;
            dma[1] = 0x02038900;
            dma[2] = control;
            (void)dma[2];

            if (secondValid != 0) {
                zeroPtr = &zero;
                *zeroPtr = clear;
                dma[0] = (u32)zeroPtr;
                dma[1] = 0x02038040;
                dma[2] = control;
                (void)dma[2];
            } else {
                dma[0] = 0x02038040;
                dma[1] = (u32)save0;
                dma[2] = 0x80000020;
                (void)dma[2];
                firstValid = 0;
                func_8000E54();
            }
        } else {
            u32 copyControl;

            dma[0] = 0x02038900;
            dma[1] = (u32)save0;
            copyControl = 0x80000020;
            dma[2] = copyControl;
            (void)dma[2];
            firstValid = 0;
            func_8000E54();

            if (secondValid != 0) {
                dma[0] = (u32)save0;
                dma[1] = 0x02038040;
                dma[2] = copyControl;
                (void)dma[2];
                func_8000E94();
            }
        }
    } else {
        u32* source;
        u32 copyControl;

        dma = (vu32*)0x040000D4;
        source = (u32*)0x02038000;
        dma[0] = (u32)source;
        dma[1] = 0x02038040;
        copyControl = 0x80000020;
        dma[2] = copyControl;
        (void)dma[2];
        func_8000E94();

        if (thirdValid != 0) {
            dma[0] = (u32)source;
            dma[1] = 0x02038900;
            dma[2] = copyControl;
            (void)dma[2];
            func_8000EAC();
        }
    }

    if (firstValid == 0) {
        u8* source;
        u8* destination;
        u32 value;

        source = (u8*)0x0203800A;
        destination = &gTitleScreenStyle;
        value = *source;
        value = (0 - value) >> 31;
        *destination = value;
    } else {
        func_8000CE0();
        gTitleScreenStyle = 0;
        BuildTitleScreenSaveHeader();
    }

    gLanguage = 0;
    gUnk_300001D = 1;
}

u32 ValidateSaveHeaderBlock(u8 saveType)
{
    u32 result;
    u32* save;
    register u32* ptr PIN("r1");
    u32 sum;
    register u32 storedChecksum PIN("r5");
    u8* data;
    s32 i;

    result = 0;
    save = (u32*)0x02038900;
    if (saveType != 2) {
        save = (u32*)0x02038000;
        if (saveType == 1) {
            save = (u32*)((u8*)save + 64);
        }
    }

    sum = 0;
    ptr = save;
    storedChecksum = ptr[0];
    data = (u8*)ptr + 12;
    i = 3;
    do {
        sum += *ptr++;
        sum += *ptr++;
        sum += *ptr++;
        sum += *ptr++;
        i--;
    } while (i >= 0);

    if (storedChecksum != sum) {
        result |= 1;
    }

    {
        u8* scan;

        register const u8* table PIN("r7");

        scan = data;
        i = 0;
        table = sUsTitleSaveVersionString;
        storedChecksum = 2;
        do {
            if (*scan != *(u8*)(i + (u32)table)) {
                result |= storedChecksum;
            }
            i++;
            scan++;
        } while (i <= 19);
    }

    ptr = save;
    {
        u32 first;

        first = *ptr++;
        if (first != ~*ptr) {
            result |= 4;
        }
    }

    return result;
}

void InitializeTemporarySaveBuffer(void)
{
    u32* data;
    register u32* checksum PIN("r5");
    register u32 sum PIN("r1");
    int i;

    gStageEntryPauseMenuDisabled = 0;
    gHasTemporarySave = gStageRoomTableIndex | 0x80;
    gSaveFlag = 2;

    data = (u32*)0x02039000;
    CopySaveFileBlock16((u8*)data, sTemporarySaveHeaderSignature1);
    CopySaveFileBlock16((u8*)0x02039A80, sTemporarySaveHeaderSignature2);

    checksum = data + 4;
    *checksum++ = 0;
    *checksum = -1;
    checksum--;

    SerializeGameStateToTemporarySave();

    sum = 0;
    i = 0x1C0;
    do {
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        i--;
    } while (i != 0);

    gUnk_3000040 = sum;
    *checksum++ = sum;
    *checksum = ~sum;
    func_8000EC4();
}

void SerializeGameStateToTemporarySave(void)
{
    register u8* destination PIN("r4");
    register u8* padding PIN("r2");
    u32 zero;
    register u32 mask PIN("r3");
    s32 i;
    u16* clearedTileCount;
    u16* musicState;
    u16* unkA;
    u8* sharedIp;
    int new_var;
    u16* new_var2;
    register u8* warioData PIN("r8");
    u8* source7;
    destination = (u8*)0x02039018;
    *(destination++) = gStageRoomTableIndex;
    *(destination++) = gCurrentPassage;
    *(destination++) = gCurrentStageNumber;
    *(destination++) = gUnk_3000025;
    *(destination++) = gRoomTransitionTileDataCount;
    *(destination++) = gTimerState;
    padding = destination;
    clearedTileCount = &gClearedRoomTileCount;
    musicState = (u16*)(&gGameMusicState);
    unkA = &gUnk_300003A;
    sharedIp = (u8*)(&gGoldenBossTransitionBg0Y);
    warioData = (u8*)(&gWarioData);
    zero = 0;
    mask = 3;
    do {
        *(destination++) = zero;
        padding++;
    } while ((((u32)padding) & mask) != 0);
    *((u16*)destination) = *clearedTileCount;
    destination += 2;
    *((u16*)destination) = musicState[1];
    destination += 2;
    padding = destination;
    if ((((u32)destination) & 3) != 0) {
        zero = 0;
        mask = 3;
        do {
            *(destination++) = zero;
            padding++;
        } while ((((u32)padding) & mask) != 0);
    }
    *((u16*)destination) = *unkA;
    destination += 2;
    {
        u16* source;
        source = (u16*)sharedIp;
        *((u16*)destination) = *source;
    }
    destination += 2;
    padding = destination;
    if ((((u32)destination) & 3) != 0) {
        zero = 0;
        mask = 3;
        do {
            *(destination++) = zero;
            padding++;
        } while ((((u32)padding) & mask) != 0);
    }
    memcpy(destination, warioData, 60);
    destination += 60;
    {
        register u32* source8 PIN("r0");
        register RegPair8 value8 PIN("r0");
        source8 = (u32*)&gWarioDashAfterimage;
        value8.w.hi = source8[1];
        value8.w.lo = source8[0];
        *((unsigned long long*)destination) = value8.q;
    }
    destination += 8;
    *((SaveBlock8*)destination) = gCurrentCarriedSprite;
    destination += 8;
    *((SaveBlock8*)destination) = gWarioMotionAfterimage;
    destination += 8;
    *((SaveBlock8*)destination) = gCurrentWarioEffect;
    destination += 8;
    *((u32*)destination) = gHeartMeter;
    destination += 4;
    *((u32*)destination) = gHeartGauge;
    destination += 4;
    *((SaveBlock12*)destination) = gWarioDustEffect1;
    destination += 12;
    *((SaveBlock12*)destination) = gWarioDustEffect2;
    destination += 12;
    i = 0;
    do {
        *((SaveBlock44*)destination) = gSpriteData[i];
        destination += 44;
        i++;
    } while (i <= 23);
    {
        u8* switchPressed;
        register u8* northEastJewel PIN("r8");
        register u8* southWestJewel PIN("r10");
        register SaveBlock12* secondarySprite PIN("r3");
        register s32 secondaryIndex PIN("r2");
        register u8* source1 PIN("r1");
        register u8* source2 PIN("r2");
        register u8* source3 PIN("r3");
        register u8* source5 PIN("r5");
        register u8* source6 PIN("r6");
        u8 value;
        switchPressed = &gSwitchPressed;
        northEastJewel = &gCollectedNEJewelPiece;
        new_var = 3;
        sharedIp = &gCollectedSEJewelPiece;
        southWestJewel = &gCollectedSWJewelPiece;
        secondarySprite = (SaveBlock12*)gSecondarySpriteData;
        secondaryIndex = 7;
        do {
            *((SaveBlock12*)destination) = *secondarySprite;
            destination += 12;
            secondarySprite++;
            secondaryIndex--;
        } while (secondaryIndex >= 0);
        source1 = switchPressed;
        value = *source1;
        *(destination++) = value;
        source2 = &gUnk_3000C04;
        value = *source2;
        *(destination++) = value;
        source3 = &gUnk_3000C05;
        value = *source3;
        *(destination++) = value;
        source5 = &gEnemyScoreDropCounter;
        value = *source5;
        *(destination++) = value;
        padding = destination;
        if ((((u32)destination) & new_var) != 0) {
            register u32 padValue PIN("r3");
            register u32 padMask PIN("r1");
            padValue = 0;
            padMask = new_var;
            do {
                *(destination++) = padValue;
                padding++;
            } while ((((u32)padding) & padMask) != 0);
        }
        source6 = northEastJewel;
        value = *source6;
        *(destination++) = value;
        source7 = sharedIp;
        value = *source7;
        *(destination++) = value;
        ASM_BARRIER(: : "r"(source1), "r"(source2), "r"(source3), "r"(source5), "r"(source6));
        source1 = southWestJewel;
        value = *source1;
        *(destination++) = value;
        source2 = &gCollectedNWJewelPiece;
        value = *source2;
        *(destination++) = value;
        padding = destination;
        if ((((u32)destination) & new_var) != 0) {
            u32 padValue;
            register u32 padMask PIN("r1");
            padValue = 0;
            padMask = new_var;
            do {
                *(destination++) = padValue;
                padding++;
            } while ((((u32)padding) & padMask) != 0);
        }
        source3 = &gCollectedCD;
        value = *source3;
        *(destination++) = value;
        source5 = &gCollectedKeyzer;
        value = *source5;
        *(destination++) = value;
        source6 = switchPressed;
        value = *source6;
        *(destination++) = value;
        source7 = &gUnk_3000C0E;
        value = *source7;
        *(destination++) = value;
        ASM_BARRIER(: : "r"(source1), "r"(source2), "r"(source3), "r"(source5), "r"(source6));
        padding = destination;
        if ((((u32)destination) & new_var) != 0) {
            u32 padValue;
            register u32 padMask PIN("r1");
            padValue = 0;
            padMask = new_var;
            do {
                *(destination++) = padValue;
                padding++;
            } while ((((u32)padding) & padMask) != 0);
        }
    }
    {
        u32* source;
        source = &gTotalScore;
        *((u32*)destination) = *source;
    }
    destination += 4;
    {
        u32* source;
        source = &gStageScore;
        *((u32*)destination) = *source;
    }
    destination += 4;
    {
        u32* source;
        source = &gStageFrameCounter;
        *((u32*)destination) = *source;
    }
    destination += 4;
    {
        register s32 index PIN("r2");
        u8* source;
        index = 0;
        source = gRoomSpriteIds;
        do {
            *(destination++) = *((u8*)(index + ((u32)source)));
            index++;
        } while (index <= 31);
        if ((index & new_var) != 0) {
            u32 padValue;
            register u32 padMask PIN("r1");
            padValue = 0;
            padMask = new_var;
            do {
                *(destination++) = padValue;
                index++;
            } while ((index & padMask) != 0);
        }
    }
    {
        register s32 index PIN("r2");
        u8* source;
        index = 0;
        source = gRoomSpriteGraphicsSlots;
        do {
            *(destination++) = *((u8*)(index + ((u32)source)));
            index++;
        } while (index <= 31);
        if ((index & new_var) != 0) {
            u32 padValue;
            register u32 padMask PIN("r1");
            padValue = 0;
            padMask = new_var;
            do {
                *(destination++) = padValue;
                index++;
            } while ((index & padMask) != 0);
        }
    }
    {
        u32 cursor;
        u8* base;
        s32 next;
        s32 remaining;
        cursor = 0;
        base = (u8*)gPersistentSpriteData;
        do {
            u32 offset;
            offset = cursor << 6;
            next = cursor + 1;
            cursor = offset + ((u32)base);
            remaining = 63;
            do {
                u8 value0;
                value0 = *((u8*)cursor);
                *(destination++) = value0;
                cursor++;
                remaining--;
            } while (remaining >= 0);
            cursor = next;
        } while (((s32)cursor) <= 15);
    }
    {
        s32 index;
        u8* source;
        index = 0;
        source = gSwitchStates;
        do {
            u8 value0;
            value0 = *((u8*)(index + ((u32)source)));
            *(destination++) = value0;
            index++;
        } while (index <= 4);
        {
            register u32 test0 PIN("r0");
            test0 = index & new_var;
            if (test0 != 0) {
                u32 padValue;
                register u32 padMask PIN("r1");
                padValue = 0;
                padMask = new_var;
                do {
                    *(destination++) = padValue;
                    index++;
                } while ((index & padMask) != 0);
            }
        }
    }
    {
        s32 index;
        u8* source;
        index = 0;
        source = gStageTimerDigits;
        do {
            u8 value0;
            value0 = *((u8*)(index + ((u32)source)));
            *(destination++) = value0;
            index++;
        } while (index <= 2);
        {
            register u32 test0 PIN("r0");
            test0 = index & new_var;
            if (test0 != 0) {
                u32 padValue;
                register u32 padMask PIN("r1");
                padValue = 0;
                padMask = new_var;
                do {
                    *(destination++) = padValue;
                    index++;
                } while ((index & padMask) != 0);
            }
        }
    }
    {
        s32 index;
        s8* source;
        index = 0;
        source = gScoreDigits;
        do {
            u8 value0;
            value0 = *((u8*)(index + ((u32)source)));
            *(destination++) = value0;
            index++;
        } while (index <= 4);
        {
            register u32 test0 PIN("r0");
            test0 = index & new_var;
            if (test0 != 0) {
                u32 padValue;
                register u32 padMask PIN("r1");
                padValue = 0;
                padMask = new_var;
                do {
                    *(destination++) = padValue;
                    index++;
                } while ((index & padMask) != 0);
            }
        }
    }
    {
        register u32 cursor PIN("r1");
        u8* base;
        s32 next;
        s32 remaining;
        cursor = 0;
        base = gCurrentCollection;
        do {
            u32 offset0;
            next = cursor + 1;
            offset0 = ((cursor << 1) + cursor) << new_var;
            cursor = offset0 + ((u32)base);
            remaining = new_var;
            do {
                u32 value0;
                value0 = *((u32*)cursor);
                *((u32*)destination) = value0;
                destination += 4;
                cursor += 4;
                remaining--;
            } while (remaining >= 0);
            cursor = next;
        } while (((s32)cursor) <= 4);
    }
    {
        u32* source;
        register u32 value0 PIN("r0");
        source = (u32*)(&gUnk_30000D8);
        value0 = *source;
        *((u32*)destination) = value0;
    }
    destination += 4;
    {
        u32* source;
        register RegPair8 v PIN("r0");
        source = (u32*)&gCameraPositionState;
        v.w.lo = source[0];
        v.w.hi = source[1];
        *((unsigned long long*)destination) = v.q;
    }
    destination += 8;
    source7 = (u8*)&gBg0XPosition;
    *((u16*)destination) = *((volatile u16*)source7);
    destination += 2;
    {
        register u16* source PIN("r1");
        source = &gBg0YPosition;
        *((u16*)destination) = *source;
    }
    destination += 2;
    {
        register u16* source PIN("r2");
        source = &gBg1XPosition;
        *((u16*)destination) = *source;
    }
    destination += 2;
    {
        register u16* source PIN("r3");
        source = &gBg1YPosition;
        *((u16*)destination) = *source;
    }
    destination += 2;
    {
        register u16* source PIN("r5");
        source = &gBg2XPosition;
        *((u16*)destination) = *source;
    }
    destination += 2;
    {
        register u16* source PIN("r6");
        source = &gBg2YPosition;
        *((u16*)destination) = *source;
    }
    destination += 2;
    source7 = (u8*)&gBg3XPosition;
    ASM_BARRIER_ARM(: "+r"(source7) : : "r0", "r1", "r2", "r3", "r5", "r6");
    *((u16*)destination) = *((u16*)source7);
    destination += 2;
    {
        register u16* source PIN("r1");
        source = &gBg3YPosition;
        new_var2 = source;
        *((u16*)destination) = (u16)*source;
        destination += 2;
    }
    CopySaveFileBlock8(destination, sSaveFooterSignature);
}

void RefreshTemporarySaveStageId(void)
{
    if (gHasTemporarySave != 0) {
        RestoreGameStateFromTemporarySave();
        gCurrentStageID = sUnk_86391C4[gCurrentPassage][gCurrentStageNumber];
    }
}

void RestoreGameStateFromTemporarySave(void)
{
    u8* source;
    register u8* padding PIN("r2");
    s32 i;

    source = (u8*)0x02039018;
    gStageRoomTableIndex = *source++;
    gCurrentPassage = *source++;
    gCurrentStageNumber = *source++;
    gUnk_3000025 = *source++;
    gRoomTransitionTileDataCount = *source++;
    gTimerState = *source++;

    {
        u16* cleared;
        u16* music;
        register u16* unkA PIN("r6");
        register u16* bossY PIN("r7");
        register u8* wario PIN("r12");
        register u32 mask PIN("r1");

        padding = source;
        cleared = &gClearedRoomTileCount;
        music = (u16*)&gGameMusicState;
        unkA = &gUnk_300003A;
        bossY = &gGoldenBossTransitionBg0Y;
        wario = (u8*)&gWarioData;
        mask = 3;
        do {
            source++;
            padding++;
        } while (((u32)padding & mask) != 0);

        *cleared = *(u16*)source;
        source += 2;
        music[1] = *(u16*)source;
        source += 2;

        padding = source;
        if (((u32)source & 3) != 0) {
            mask = 3;
            do {
                source++;
                padding++;
            } while (((u32)padding & mask) != 0);
        }

        *unkA = *(u16*)source;
        source += 2;
        gGoldenBossTransitionBg0Y = *(u16*)source;
        source += 2;

        padding = source;
        if (((u32)source & 3) != 0) {
            mask = 3;
            do {
                source++;
                padding++;
            } while (((u32)padding & mask) != 0);
        }

        memcpy(wario, source, 60);
        source += 60;
    }

    gWarioDashAfterimage = *(SaveBlock8*)source;
    source += 8;
    gCurrentCarriedSprite = *(SaveBlock8*)source;
    source += 8;
    gWarioMotionAfterimage = *(SaveBlock8*)source;
    source += 8;
    gCurrentWarioEffect = *(SaveBlock8*)source;
    source += 8;
    gHeartMeter = *(u32*)source;
    source += 4;
    gHeartGauge = *(u32*)source;
    source += 4;
    gWarioDustEffect1 = *(SaveBlock12*)source;
    source += 12;
    gWarioDustEffect2 = *(SaveBlock12*)source;
    source += 12;

    i = 0;
    do {
        gSpriteData[i] = *(SaveBlock44*)source;
        source += 44;
        i++;
    } while (i <= 23);

    {
        u8* switchPressed;
        register u8* northEastJewel PIN("r12");
        register u8* southEastJewel PIN("r8");
        u8* southWestJewel;
        SaveBlock12* secondarySprite;
        s32 secondaryIndex;

        switchPressed = &gSwitchPressed;
        northEastJewel = &gCollectedNEJewelPiece;
        southEastJewel = &gCollectedSEJewelPiece;
        southWestJewel = &gCollectedSWJewelPiece;
        secondarySprite = (SaveBlock12*)gSecondarySpriteData;
        secondaryIndex = 7;
        do {
            *secondarySprite = *(SaveBlock12*)source;
            source += 12;
            secondarySprite++;
            secondaryIndex--;
        } while (secondaryIndex >= 0);

        {
            u8 value;
            register u8* destination1 PIN("r1");
            register u8* destination2 PIN("r2");
            register u8* destination3 PIN("r3");
            register u8* destination5 PIN("r5");
            u8* destination6;
            u8* destination7;

            value = *source;
            destination1 = switchPressed;
            *destination1 = value;
            source++;
            value = *source;
            destination2 = &gUnk_3000C04;
            *destination2 = value;
            source++;
            value = *source;
            destination3 = &gUnk_3000C05;
            *destination3 = value;
            source++;
            value = *source;
            destination5 = &gEnemyScoreDropCounter;
            *destination5 = value;
            source++;
            padding = source;
            if (((u32)source & 3) != 0) {
                register u32 mask PIN("r1");
                mask = 3;
                do {
                    source++;
                    padding++;
                } while (((u32)padding & mask) != 0);
            }

            value = *source;
            destination6 = northEastJewel;
            *destination6 = value;
            source++;
            value = *source;
            destination7 = southEastJewel;
            *destination7 = value;
            ASM_BARRIER(
                    :
                    : "r"(destination1), "r"(destination2), "r"(destination3), "r"(destination5), "r"(destination6));
            source++;
            value = *source;
            destination1 = southWestJewel;
            *destination1 = value;
            source++;
            value = *source;
            destination2 = &gCollectedNWJewelPiece;
            *destination2 = value;
            source++;
            padding = source;
            if (((u32)source & 3) != 0) {
                register u32 mask PIN("r1");
                mask = 3;
                do {
                    source++;
                    padding++;
                } while (((u32)padding & mask) != 0);
            }

            value = *source;
            destination3 = &gCollectedCD;
            *destination3 = value;
            source++;
            value = *source;
            destination5 = &gCollectedKeyzer;
            *destination5 = value;
            source++;
            value = *source;
            destination6 = switchPressed;
            *destination6 = value;
            source++;
            value = *source;
            destination7 = &gUnk_3000C0E;
            *destination7 = value;
            ASM_BARRIER(
                    :
                    : "r"(destination1), "r"(destination2), "r"(destination3), "r"(destination5), "r"(destination6));
            source++;
        }
        padding = source;
        if (((u32)source & 3) != 0) {
            register u32 mask PIN("r1");
            mask = 3;
            do {
                source++;
                padding++;
            } while (((u32)padding & mask) != 0);
        }
    }

    gTotalScore = *(u32*)source;
    source += 4;
    gStageScore = *(u32*)source;
    source += 4;
    gStageFrameCounter = *(u32*)source;
    source += 4;

    {
        s32 index;
        u8* destination;
        index = 0;
        destination = gRoomSpriteIds;
        do {
            *(u8*)(index + (u32)destination) = *source++;
            index++;
        } while (index <= 31);
        if ((index & 3) != 0) {
            u32 mask;
            mask = 3;
            do {
                source++;
                index++;
            } while ((index & mask) != 0);
        }
    }
    {
        s32 index;
        u8* destination;
        index = 0;
        destination = gRoomSpriteGraphicsSlots;
        do {
            *(u8*)(index + (u32)destination) = *source++;
            index++;
        } while (index <= 31);
        if ((index & 3) != 0) {
            u32 mask;
            mask = 3;
            do {
                source++;
                index++;
            } while ((index & mask) != 0);
        }
    }
    {
        u32 cursor;
        u8* base;
        s32 next;
        s32 remaining;
        cursor = 0;
        base = (u8*)gPersistentSpriteData;
        do {
            u32 offset;
            offset = cursor << 6;
            next = cursor + 1;
            cursor = offset + (u32)base;
            remaining = 63;
            do {
                *(u8*)cursor = *source++;
                cursor++;
                remaining--;
            } while (remaining >= 0);
            cursor = next;
        } while ((s32)cursor <= 15);
    }
    {
        s32 index;
        u8* destination;
        index = 0;
        destination = gSwitchStates;
        do {
            *(u8*)(index + (u32)destination) = *source++;
            index++;
        } while (index <= 4);
        if ((index & 3) != 0) {
            u32 mask;
            mask = 3;
            do {
                source++;
                index++;
            } while ((index & mask) != 0);
        }
    }
    {
        s32 index;
        u8* destination;
        index = 0;
        destination = gStageTimerDigits;
        do {
            *(u8*)(index + (u32)destination) = *source++;
            index++;
        } while (index <= 2);
        if ((index & 3) != 0) {
            u32 mask;
            mask = 3;
            do {
                source++;
                index++;
            } while ((index & mask) != 0);
        }
    }
    {
        s32 index;
        s8* destination;
        index = 0;
        destination = gScoreDigits;
        do {
            *(u8*)(index + (u32)destination) = *source++;
            index++;
        } while (index <= 4);
        if ((index & 3) != 0) {
            u32 mask;
            mask = 3;
            do {
                source++;
                index++;
            } while ((index & mask) != 0);
        }
    }
    {
        register s32 index PIN("r1");
        u8* base;
        s32 next;
        s32 remaining;
        index = 0;
        base = gCurrentCollection;
        do {
            register u8* destination PIN("r1");
            next = index + 1;
            destination = (u8*)(((((index << 1) + index) << 3)) + (u32)base);
            remaining = 3;
            do {
                *(u32*)destination = *(u32*)source;
                source += 4;
                destination += 4;
                remaining--;
            } while (remaining >= 0);
            index = next;
        } while (index <= 4);
    }

    *(u32*)&gUnk_30000D8 = *(u32*)source;
    source += 4;
    *(SaveBlock8*)&gCameraPositionState = *(SaveBlock8*)source;
    source += 8;
    gBg0XPosition = *(u16*)source;
    source += 2;
    gBg0YPosition = *(u16*)source;
    source += 2;
    gBg1XPosition = *(u16*)source;
    source += 2;
    gBg1YPosition = *(u16*)source;
    source += 2;
    gBg2XPosition = *(u16*)source;
    source += 2;
    gBg2YPosition = *(u16*)source;
    source += 2;
    gBg3XPosition = *(u16*)source;
    source += 2;
    gBg3YPosition = *(u16*)source;
}

void BuildMainSaveWorkingBuffer(void)
{
    u32* data;
    u32* checksum;
    u32 sum;
    s32 i;

    data = (u32*)0x02038100;
    CopySaveFileBlock16((u8*)data, sMainSaveHeaderSignature1);
    CopySaveFileBlock16((u8*)0x02038260, sMainSaveHeaderSignature2);

    checksum = data + 4;
    ASM_BARRIER_VOLATILE(: "+r"(checksum));
    *checksum++ = 0;
    *checksum = -1;
    checksum--;

    SerializeGlobalSaveDataToWorkingBuffer();

    sum = 0;
    i = 14;
    do {
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        sum += *data++;
        i--;
    } while (i >= 0);

    *checksum++ = sum;
    sum = ~sum;
    *checksum = sum;
    *(u8*)0x020382E0 = 0;
    func_8000EEC();
}

void SerializeGlobalSaveDataToWorkingBuffer(void)
{
    u8* destination;
    u8* padding;
    u16* medal;
    u32* totalScore;
    u8* unknown;
    const u8* footer;
    register u32 zero PIN("r4");
    u32 work;

    destination = (u8*)0x02038118;
    *destination++ = gDifficulty;
    *destination++ = gCurrentPassage;
    *destination++ = gCurrentStageNumber;

    padding = destination;
    medal = &gMedalCount;
    totalScore = &gTotalScore;
    unknown = &gUnk_300001C;
    ASM_BARRIER_VOLATILE(: "+r"(unknown));
    footer = sSaveFooterSignature;
    zero = 0;
    work = 3;
    do {
        *destination++ = zero;
        padding++;
    } while (((u32)padding & work) != 0);

    *(u16*)destination = *medal;
    destination += 2;
    padding = destination;
    if (((u32)destination & 3) != 0) {
        zero = 0;
        work = 3;
        do {
            *destination++ = zero;
            padding++;
        } while (((u32)padding & work) != 0);
    }

    *(u32*)destination = *totalScore;
    destination += 4;

    {
        register int outer PIN("r3");
        register u8* base PIN("r6");
        u32 offset;
        u32* source;
        int inner;

        outer = 0;
        base = gCurrentCollection;
        offset = 0;
        do {
            source = (u32*)(offset + (u32)base);
            inner = 5;
            do {
                *(u32*)destination = *source++;
                destination += 4;
                inner--;
            } while (inner >= 0);
            ASM_BARRIER_VOLATILE(: "+r"(offset));
            offset += 24;
            outer++;
        } while (outer <= 6);
    }

    {
        int cursor;
        register u8* base PIN("r5");
        int next;
        int inner;
        u32 shifted;

        cursor = 0;
        base = gHighScoreTable;
        do {
            next = cursor + 1;
            shifted = cursor << 4;
            cursor = shifted + (u32)base;
            inner = 3;
            do {
                *(u32*)destination = *(u32*)cursor;
                destination += 4;
                cursor += 4;
                inner--;
            } while (inner >= 0);
            cursor = next;
        } while (cursor <= 6);
    }

    {
        register u16* source PIN("r3");
        int inner;

        source = gMinigameHighScores;
        inner = 2;
        do {
            *(u16*)destination = *source;
            destination += 2;
            source++;
            inner--;
        } while (inner >= 0);
    }

    padding = (u8*)3;
    zero = 0;
    work = 3;
    do {
        *destination++ = zero;
        padding++;
    } while (((u32)padding & work) != 0);

    *destination++ = *unknown;
    padding = destination;
    if (((u32)destination & 3) != 0) {
        zero = 0;
        work = 3;
        do {
            *destination++ = zero;
            padding++;
        } while (((u32)padding & work) != 0);
    }

    CopySaveFileBlock8(destination, footer);
}

void RestoreGlobalSaveDataFromWorkingBuffer(void)
{
    u8* source;
    register u8* padding PIN("r1");
    u16* medal;
    u32* totalScore;
    u8* unknown;
    register u32 mask PIN("r2");

    source = (u8*)0x02038118;
    gDifficulty = *source++;
    gCurrentPassage = *source++;
    gCurrentStageNumber = *source++;

    padding = source;
    medal = &gMedalCount;
    totalScore = &gTotalScore;
    unknown = &gUnk_300001C;
    mask = 3;
    do {
        source++;
        padding++;
    } while (((u32)padding & mask) != 0);

    *medal = *(u16*)source;
    source += 2;
    padding = source;
    if (((u32)source & 3) != 0) {
        mask = 3;
        do {
            source++;
            padding++;
        } while (((u32)padding & mask) != 0);
    }

    *totalScore = *(u32*)source;
    source += 4;

    {
        register int outer PIN("r2");
        register u8* base PIN("r6");
        u32 offset;
        u32* destination;
        int inner;

        outer = 0;
        base = gCurrentCollection;
        offset = 0;
        do {
            destination = (u32*)(offset + (u32)base);
            inner = 5;
            do {
                *destination++ = *(u32*)source;
                source += 4;
                inner--;
            } while (inner >= 0);
            ASM_BARRIER_VOLATILE(: "+r"(offset));
            offset += 24;
            outer++;
        } while (outer <= 6);
    }

    {
        int cursor;
        register u8* base PIN("r5");
        register int next PIN("r4");
        int inner;
        u32 shifted;

        cursor = 0;
        base = gHighScoreTable;
        do {
            next = cursor + 1;
            shifted = cursor << 4;
            cursor = shifted + (u32)base;
            inner = 3;
            do {
                *(u32*)cursor = *(u32*)source;
                source += 4;
                cursor += 4;
                inner--;
            } while (inner >= 0);
            cursor = next;
        } while (cursor <= 6);
    }

    {
        register u16* destination PIN("r2");
        int inner;

        destination = gMinigameHighScores;
        inner = 2;
        do {
            *destination = *(u16*)source;
            source += 2;
            destination++;
            inner--;
        } while (inner >= 0);
    }

    padding = (u8*)3;
    mask = 3;
    do {
        source++;
        padding++;
    } while (((u32)padding & mask) != 0);

    *unknown = *source;
    source++;
    ASM_BARRIER_VOLATILE(: "+r"(source));
    padding = source;
    if (((u32)padding & 3) != 0) {
        mask = 3;
        do {
            padding++;
        } while (((u32)padding & mask) != 0);
    }
}

void CopySaveFileBlock16(u8* destination, const u8* source)
{
    s32 index;

    index = 0;
    do {
        *destination = source[index];
        index++;
        destination++;
    } while (index <= 15);
}

void CopySaveFileBlock8(u8* destination, const u8* source)
{
    s32 index;

    index = 0;
    do {
        *destination = source[index];
        index++;
        destination++;
    } while (index <= 7);
}

void RunBackupSramSelfTest(void)
{
    u8 buffer[16];
    u8* sram;
    u32 failed;
    u32 temp;
    u32 result;
    s32 index;

    failed = 0;
    gUnk_3000014 = failed;
    result = SramWriteChecked(sBackupSramTestSignature, (u8*)0x0E000980, 16);
    temp = -result;
    temp |= result;
    failed = temp >> 31;
    sram = (u8*)0x0E000980;
    SramWriteUnchecked(sram, buffer, 16);
    index = 0;
    while (index <= 15) {
        buffer[index]++;
        index++;
    }
    if (SramWriteChecked(buffer, (u8*)0x0E000980, 16) != 0) {
        failed |= 2;
    }
    gUnk_3000014 = (((-failed) | failed) >> 31);
}

void DmaLoadSelectedSaveFileBlocks(void)
{
    vu32* dma;

    dma = (vu32*)0x040000D4;
    dma[0] = (u32)GP(sPrimarySaveHeaderBuffers[gSelectedSaveFile]);
    dma[1] = (u32)GP(sSecondarySaveHeaderBuffers[gSelectedSaveFile]);
    dma[2] = 0x80000100;
    func_8000F90(dma[2]);
}

void InitializeSaveFileStatuses(void)
{
    u32 r4Value;
    u8* record;
    u8* records;
    u32 encodedIndex;
    register u32 status PIN("r5");
    u32 zero0;
    register u32 zero1 PIN("r1");

    zero0 = 0;
    zero1 = 0;
    r4Value = (u32)gUnk_3003BF8;
    ((u32*)r4Value)[0] = zero0;
    ((u32*)r4Value)[1] = zero1;
    ((u32*)r4Value)[2] = zero0;
    ((u32*)r4Value)[3] = zero1;
    {
        u8* selectedPtr;
        selectedPtr = &gSelectedSaveFile;
        ASM_BARRIER_VOLATILE(: "+r"(zero1));
        zero1 = 0;
        *selectedPtr = zero1;
    }
    gSaveFlag = zero1;

    if (gUnk_3000014 == 0) {
        func_800101C();
    }

    record = (u8*)r4Value;
    records = record;
    encodedIndex = 0;
    do {
        r4Value = encodedIndex >> 24;
        ASM_BARRIER_VOLATILE(: "+r"(encodedIndex));
        status = ClassifyAndRepairSaveSlotCopy(r4Value);
        status = (status << 24) >> 24;
        if (status == 2) {
            if (ValidateStageSelectionSaveSlot(r4Value) != 0) {
                record[0] = 3;
            } else {
                record[0] = 1;
            }
        }

        if (ValidateMainSaveSlot(encodedIndex >> 24, status) == 0) {
            record[0] = 2;
        }

        record += 8;
        encodedIndex += 0x01000000;
    } while ((s32)(u32)record <= (s32)((u32)records + 8));

    {
        u8* selectedPtr;
        u8* saveFlagPtr;
        u8* recordsPtr;
        u32 selectedValue;

        selectedPtr = &gSelectedSaveFile;
        *selectedPtr = 0;
        selectedValue = ValidateSelectedSaveHeader();
        *selectedPtr = selectedValue;
        saveFlagPtr = &gSaveFlag;
        recordsPtr = gUnk_3003BF8;
        {
            u32 saveStatus;
            u32 selectedIndex;

            selectedIndex = *selectedPtr;
            selectedIndex <<= 3;
            selectedIndex += (u32)recordsPtr;
            saveStatus = *(u8*)selectedIndex;
            *saveFlagPtr = saveStatus;
            saveStatus <<= 24;
            if (saveStatus != 0) {
                register u8* tempSavePtr PIN("r0");
                tempSavePtr = &gHasTemporarySave;
                selectedValue = *selectedPtr;
                selectedValue <<= 3;
                selectedValue += (u32)recordsPtr;
                *tempSavePtr = *(u8*)(selectedValue + 1);
            } else {
                selectedIndex = selectedValue << 24;
                selectedValue = 0;
                if (selectedIndex == 0) {
                    selectedValue = 1;
                }
                selectedIndex = selectedValue << 3;
                recordsPtr = (u8*)(selectedIndex + (u32)recordsPtr);
                saveStatus = recordsPtr[0];
                *saveFlagPtr = saveStatus;
                saveStatus <<= 24;
                if (saveStatus != 0) {
                    *selectedPtr = selectedValue;
                    selectedValue = (u32)&gHasTemporarySave;
                    saveStatus = recordsPtr[1];
                    *(u8*)selectedValue = saveStatus;
                }
            }
        }
    }

    gUnk_300001C = 0;
}

u32 ClassifyAndRepairSaveSlotCopy(u8 selectedFile)
{
    u32 selected;
    u32 firstResult;
    u32 secondResult;
    u32 offset;
    u32 statusValue;

    selected = selectedFile;
    gSelectedSaveFile = selected;

    {
        register u32 arg0 PIN("r0");
        register u32 arg1 PIN("r1");

        arg0 = 0;
        arg1 = selected;
        firstResult = ValidateAndClearSaveSlotCopy(arg0, arg1);
        arg0 = 1;
        arg1 = selected;
        secondResult = ValidateAndClearSaveSlotCopy(arg0, arg1);
    }

    if (firstResult == 0) {
        register vu32* dma PIN("r2");
        u32* table;

        dma = (vu32*)0x040000D4;
        table = (u32*)sPrimarySaveHeaderBuffers;
        offset = selected << 2;
        dma[0] = *(u32*)(offset + (u32)table);
        table = (u32*)sSecondarySaveHeaderBuffers;
        offset = offset + (u32)table;
        dma[1] = *(u32*)offset;
        dma[2] = 0x80000100;
        func_8000F90(dma[2]);

        {
            u8* record;

            record = gUnk_3003BF8;
            offset = selected << 3;
            record = (u8*)(offset + (u32)record);
            record[5] = firstResult;
        }
    } else if (firstResult == 1) {
        u8* record;
        u8* base;

        base = gUnk_3003BF8;
        offset = selected << 3;
        record = (u8*)(offset + (u32)base);
        record[5] = 2;
        if (secondResult != 0) {
            record[5] = 3;
        }
    } else {
        u8* record;
        u8* base;

        base = gUnk_3003BF8;
        offset = selected << 3;
        record = (u8*)(offset + (u32)base);
        record[5] = 4;
        if (secondResult == 0) {
            record[5] = 2;
        } else if (secondResult == 1) {
            record[5] = 3;
        }
    }

    {
        register u8* base PIN("r0");

        base = gUnk_3003BF8;
        selected = offset + (u32)base;
    }

    statusValue = ((u8*)selected)[5];
    if (statusValue == 4) {
        ASM_BARRIER_VOLATILE(: "+r"(statusValue));
        func_8000F90(statusValue);
        func_8000F64();
        statusValue = 0;
        ((u8*)selected)[5] = statusValue;
    } else if (firstResult != 0) {
        statusValue = 1;
        ASM_BARRIER_VOLATILE(: "+r"(statusValue));
    } else {
        statusValue = 2;
    }

    return statusValue;
}

u32 ValidateAndClearSaveSlotCopy(u8 copy, u8 selectedFile)
{
    u32 zero;
    s32 result;
    u32* save;
    u32* checksumStorage;
    u32* scan;
    register u32 sum PIN("r2");
    s32 i;
    u32 headerMismatch;
    u32 footerMismatch;

    result = 0;
    headerMismatch = result;
    footerMismatch = 0;

    if (copy == 0) {
        save = GP(sPrimarySaveHeaderBuffers[selectedFile]);
    } else {
        save = GP(sSecondarySaveHeaderBuffers[selectedFile]);
    }

    checksumStorage = &gUnk_3000040;
    *checksumStorage = save[4];
    scan = save;
    sum = 0;
    i = 14;
    do {
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        i--;
    } while (i >= 0);

    {
        register u32 checksum PIN("r1");
        checksum = *checksumStorage;
        if (checksum != sum || checksum != ~save[5]) {
            result |= 1;
        }
    }

    {
        u8* data;
        register const u8* expected PIN("r8");
        u32 one;
        data = (u8*)save;
        i = 0;
        expected = sMainSaveHeaderSignature1;
        one = 1;
        do {
            if (*(u8*)(i + (u32)expected) != *data) {
                headerMismatch |= one;
            }
            i++;
            data++;
        } while (i <= 15);
    }

    {
        u8* data;
        register const u8* expected PIN("r8");
        u32 one;
        data = (u8*)save + 0x160;
        i = 0;
        expected = sMainSaveHeaderSignature2;
        one = 1;
        do {
            if (*(u8*)(i + (u32)expected) != *data) {
                footerMismatch |= one;
            }
            i++;
            data++;
        } while (i <= 15);
    }

    result |= headerMismatch + footerMismatch;
    if (result > 1) {
        vu32* dma;
        u32* zeroPtr;
        u32 control;

        zero = 0;
        dma = (vu32*)0x040000D4;
        zeroPtr = &zero;
        dma[0] = (u32)zeroPtr;
        dma[1] = (u32)save;
        control = 0x85000078;
        dma[2] = control;
        (void)dma[2];
    }

    return result;
}

u32 ValidateMainSaveSlot(u8 selectedFile, u32 statusValue)
{
    u32 zero;
    u32 selected;
    u32 status;
    s32 result;
    u32 headerMismatch;
    u32 footerMismatch;
    u32 offset;
    u32* save;
    u32* checksumStorage;
    u32* scan;
    u32 sum;
    s32 i;

    selected = selectedFile;
    status = (statusValue << 24) >> 24;
    result = 0;
    headerMismatch = result;
    footerMismatch = result;
    gSelectedSaveFile = selected;
    save = GP(sMainSaveDataBuffers[selected]);

    checksumStorage = &gUnk_3000040;
    *checksumStorage = save[4];
    scan = save;
    sum = 0;
    i = 448;
    do {
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        sum += *scan++;
        i--;
    } while (i != 0);

    {
        register u32 checksum PIN("r1");
        checksum = *checksumStorage;
        if (checksum != sum || checksum != ~save[5]) {
            result |= 1;
        }
    }

    {
        u8* data;
        register const u8* expected PIN("r3");
        u32 one;
        data = (u8*)save;
        i = 0;
        selected <<= 3;
        offset = selected;
        expected = sTemporarySaveHeaderSignature1;
        one = 1;
        do {
            if (*(u8*)(i + (u32)expected) != *data) {
                headerMismatch |= one;
            }
            i++;
            data++;
        } while (i <= 15);
    }

    {
        u8* data;
        register const u8* expected PIN("r3");
        u32 one;

        data = (u8*)save + 0xA80;
        i = 0;
        expected = sTemporarySaveHeaderSignature2;
        one = 1;
        do {
            if (*(u8*)(i + (u32)expected) != *data) {
                footerMismatch |= one;
            }
            i++;
            data++;
        } while (i <= 15);
    }

    result |= headerMismatch + footerMismatch;

    {
        u32 statusCheck;
        statusCheck = status;
        if (statusCheck != 2 || result > 1) {
            vu32* dma;
            u32* zeroPtr;
            u32 control;

            zero = 0;
            dma = (vu32*)0x040000D4;
            zeroPtr = &zero;
            dma[0] = (u32)zeroPtr;
            dma[1] = (u32)save;
            control = 0x85000E00;
            dma[2] = control;
            (void)dma[2];
            func_8000FBC();
            {
                u8* record;
                record = gUnk_3003BF8;
                record += offset;
                record[1] = 0;
            }
            result = 3;
        } else if (result == 1) {
            u8* record;
            record = gUnk_3003BF8;
            record += offset;
            record[5] = result;
        } else {
            u8* record;
            u32 savedValue;
            u32 value;
            record = gUnk_3003BF8;
            record += offset;
            savedValue = ((u8*)save)[24];
            value = 0x80;
            value |= savedValue;
            record[1] = value;
            result = 0;
        }
    }

    return result;
}

void FlushSaveFileAndMaybeRestoreTempSave(void)
{
    vu32* dma;

    if (gSelectedSaveFile == 1) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(sPrimarySaveHeaderBuffers[1]);
        dma[1] = (u32)GP(sPrimarySaveHeaderBuffers[0]);
        dma[2] = 0x80000200;
        (void)dma[2];
        dma[0] = (u32)GP(sMainSaveDataBuffers[1]);
        dma[1] = (u32)GP(sMainSaveDataBuffers[0]);
        dma[2] = 0x80001C00;
        (void)dma[2];
    }

    if (gSaveFlag != 0) {
        RestoreGlobalSaveDataFromWorkingBuffer();
    }

    if (gSaveFlag == 3) {
        RestoreTempSaveStageSelection();
    }
}

void FlushSaveFileAndRefreshSaveData(void)
{
    vu32* dma;

    if (gSelectedSaveFile == 1) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(sPrimarySaveHeaderBuffers[1]);
        dma[1] = (u32)GP(sPrimarySaveHeaderBuffers[0]);
        dma[2] = 0x80000200;
        (void)dma[2];
        dma[0] = (u32)GP(sMainSaveDataBuffers[1]);
        dma[1] = (u32)GP(sMainSaveDataBuffers[0]);
        dma[2] = 0x80001C00;
        (void)dma[2];
    }
    RestoreGlobalSaveDataFromWorkingBuffer();
}

void BuildSelectedSaveValidationHeader(void)
{
    s32 i;

    {
        u8* data;
        u8* selectedPtr;
        u8* selectedGlobal;
        u8* table;
        u32 selected;

        data = (u8*)0x02038080;
        *data = 1;
        data++;
        (*data)++;
        data++;
        *data = 0;
        data++;
        *data = 0;
        data++;
        *data = 0xFF;
        data++;
        *data = 0xFF;
        data++;

        selectedPtr = &gSelectedSaveFile;
        *data = *selectedPtr;
        data++;

        i = 0;
        table = sSaveSlotValidationSignatures;
        selectedGlobal = selectedPtr;
        do {
            u32 offset;

            selected = *selectedGlobal;
            offset = selected << 3;
            offset += selected;
            offset = (i) - (-(offset));
            offset += (u32)table;
            *data = *(u8*)offset;
            i++;
            data++;
        } while (i <= 8);

        i = 0;
        {
            register u8* source PIN("r1");

            source = sStageSelectionSaveSignature;
            do {
                *data = *(u8*)(i + (u32)source);
                i++;
                data++;
            } while (i <= 15);
        }
    }

    {
        u16* sumPtr;
        u16 sum;

        sum = 0;
        sumPtr = (u16*)0x02038080;
        i = 3;
        do {
            sum += *sumPtr++;
            sum += *sumPtr++;
            sum += *sumPtr++;
            sum += *sumPtr++;
            i--;
        } while (i >= 0);

        sumPtr = (u16*)0x02038082;
        *sumPtr = sum;
        sumPtr[1] = ~sum;
    }

    func_8000FE8();
}

u32 ValidateSelectedSaveHeader(void)
{
    u8* data;
    s32 i;
    u16 value;

    data = (u8*)0x02038080;
    if (*data != 0) {
        u16* sumPtr;

        ASM_BARRIER_VOLATILE(: "+r"(data));

        value = 0;
        sumPtr = (u16*)0x02038080;
        i = 3;
        do {
            value += *sumPtr++;
            value += *sumPtr++;
            value += *sumPtr++;
            value += *sumPtr++;
            i--;
        } while (i >= 0);

        {
            register u16* storedPtr PIN("r1");

            storedPtr = (u16*)0x02038082;
            if (*storedPtr == value) {
                u8 selected;
                register u8* expected PIN("r1");
                u32 offset;

                data = (u8*)0x02038086;
                selected = *data;
                data++;
                value = 0;
                expected = sSaveSlotValidationSignatures;
                offset = selected << 3;
                offset += selected;
                expected = (u8*)(offset + (u32)expected);
                i = 8;
                do {
                    if (*data != *expected) {
                        value++;
                    }
                    expected++;
                    i--;
                    data++;
                } while (i >= 0);

                i = 0;
                {
                    register u8* expectedBase PIN("r6");

                    expectedBase = sStageSelectionSaveSignature;
                    do {
                        expected = (u8*)(i + (u32)expectedBase);
                        if (*data != *expected) {
                            value++;
                        }
                        i++;
                        data++;
                    } while (i <= 15);
                }

                if (value == 0) {
                    return selected;
                }
            }
        }
    }

    return 0;
}

void BuildTemporaryStageSelectionSave(void)
{
    u8* data;
    u8* out;
    u8* expected;
    u32* checksumPtr;
    u32 sum;
    u32 value;
    s32 i;

    if (gCurrentPassage <= 6) {
        if (gCurrentStageNumber <= 5) {
            if (sUnk_86391C4[gCurrentPassage][gCurrentStageNumber] == 50) {
                return;
            }
        } else {
            gCurrentStageNumber = 6;
        }

        data = (u8*)0x020382E0;
        *data++ = 1;
        *data++ = gCurrentPassage;
        *data++ = gCurrentStageNumber;
        (*data)++;

        out = (u8*)0x020382F0;
        i = 0;
        while (i <= 7) {
            *out = sPassageNameValidationStrings[i + (gCurrentPassage * 8)];
            i++;
            out++;
        }

        i = 0;
        while (i <= 7) {
            *out = sStageNameValidationStrings[i + (gCurrentStageNumber * 8)];
            i++;
            out++;
        }

        checksumPtr = (u32*)0x020382E4;
        value = 0;
        *checksumPtr++ = value;
        value--;
        *checksumPtr++ = value;
        *checksumPtr = 0x3164724E;

        sum = 0;
        data = (u8*)0x020382E0;
        checksumPtr = (u32*)data;
        i = 1;
        while (i >= 0) {
            sum += *checksumPtr++;
            sum += *checksumPtr++;
            sum += *checksumPtr++;
            sum += *checksumPtr++;
            i--;
        }

        checksumPtr = (u32*)0x020382E4;
        *checksumPtr++ = sum;
        *checksumPtr = ~sum;
        func_8000F3C();
    }
}

u32 ValidateStageSelectionSaveSlot(u8 selectedFile)
{
    u8* save;
    register u8* data PIN("r2");
    u32 passage;
    register u32 stage PIN("r5");

    data = GP(sStageSelectionSaveBuffers[selectedFile]);
    save = data;
    if (data[0] != 0) {
        data++;
        passage = data[0];
        stage = data[1];
        if (passage <= 6) {
            if (stage <= 5) {
                if (sUnk_86391C4[passage][stage] == 50) {
                    return 0;
                }
            } else if (stage > 6) {
                return 0;
            }

            {
                u32 sum;
                register u32* ptr PIN("r1");
                register u32 checksum PIN("r2");
                int count;

                sum = 0;
                ptr = (u32*)save;
                checksum = *(u32*)(save + 4);
                count = 1;
                do {
                    sum += *ptr++;
                    sum += *ptr++;
                    sum += *ptr++;
                    sum += *ptr++;
                    count--;
                } while (count >= 0);

                if (checksum != sum) {
                    return 0;
                }
            }

            {
                u32 mismatch;
                u8* scan;
                register u8* expected PIN("r1");
                register int count PIN("r4");
                u32 expectedValue;

                mismatch = 0;
                scan = save + 16;
                expected = (u8*)(passage << 3);
                stage <<= 3;
                expected += (u32)sPassageNameValidationStrings;
                count = 7;
                do {
                    expectedValue = *expected;
                    if (*scan != expectedValue) {
                        mismatch++;
                    }
                    expected++;
                    count--;
                    scan++;
                } while (count >= 0);

                {
                    register u8* secondBase PIN("r0");

                    secondBase = sStageNameValidationStrings;
                    expected = (u8*)(stage + (u32)secondBase);
                }
                count = 7;
                do {
                    stage = *expected;
                    if (*scan != stage) {
                        mismatch++;
                    }
                    expected++;
                    count--;
                    scan++;
                } while (count >= 0);

                if (mismatch == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void RestoreTempSaveStageSelection(void)
{
    u8* data;
    u8 passage;
    u8 stage;

    if (*(u8*)0x020382E0 != 0) {
        data = (u8*)0x020382E1;
        passage = data[0];
        stage = data[1];
        gCurrentPassage = passage;
        gCurrentStageNumber = stage;
    }
}

#ifndef PORTABLE
const u8 sBackupSramTestSignature[16] = "Wari4_AGB_BackUp";
const u8 sSaveFooterSignature[8] = "SAVE_END";
const u8 sMainSaveHeaderSignature1[16] = "AutoSAVEWar4key1";
const u8 sMainSaveHeaderSignature2[16] = "key2AutoSAVEWar4";
const u8 sTemporarySaveHeaderSignature1[16] = "DisContinueKey1b";
const u8 sTemporarySaveHeaderSignature2[16] = "DisContinue2bKey";
const u8 sUsTitleSaveVersionString[20] = "AGBWarioLand-USver00";
const u8 sJpTitleSaveVersionString[20] = "WARIOLANDNIPPONVer00";

u8* const sPrimarySaveHeaderBuffers[2] = {
    (u8*)0x02038100,
    (u8*)0x02038500,
};

u8* const sSecondarySaveHeaderBuffers[2] = {
    (u8*)0x02038300,
    (u8*)0x02038700,
};

u8* const sMainSaveDataBuffers[2] = {
    (u8*)0x02039000,
    (u8*)0x0203C800,
};

u8* const sStageSelectionSaveBuffers[2] = {
    (u8*)0x020382E0,
    (u8*)0x020386E0,
};

const u8 sSaveSlotValidationSignatures[18] =
    "WAR4SAVEA"
    "BSAVEWAR4";
const u8 sStageSelectionSaveSignature[16] __attribute__((aligned(4))) = "WARABGSELECTSAVE";
const u8 sPassageNameValidationStrings[48] =
    " FIRST  "
    "  GREEN "
    "   RED  "
    " YELLOW "
    "    BLUE"
    " GOLDEN ";
const u8 sStageNameValidationStrings[56] =
    "StAGe000"
    "01sTAgEn"
    "WarioSt2"
    "AgbWar03"
    "STe4boSS"
    "MinIGaMe"
    "sElecTWn";
#else
extern const u8 sBackupSramTestSignature[16];
extern const u8 sSaveFooterSignature[8];
extern const u8 sMainSaveHeaderSignature1[16];
extern const u8 sMainSaveHeaderSignature2[16];
extern const u8 sTemporarySaveHeaderSignature1[16];
extern const u8 sTemporarySaveHeaderSignature2[16];
extern const u8 sUsTitleSaveVersionString[20];
extern const u8 sJpTitleSaveVersionString[20];
extern const u8 sSaveSlotValidationSignatures[18];
extern const u8 sStageSelectionSaveSignature[16];
extern const u8 sPassageNameValidationStrings[48];
extern const u8 sStageNameValidationStrings[56];
#endif

#include "demo.h"

#include "demo_input.h"
#include "interrupt_callbacks.h"
#include "main.h"
#include "save_file.h"
#include "score.h"
#include "sound.h"
#include "sram_misc.h"
#include "wario.h"

struct AGB_ALIGN DemoDmaRegisters {
    GBA_PTR(const void*) source;
    GBA_PTR(void*) destination;
    vu32 control;
};

struct AGB_ALIGN DemoMusicState {
    u8 state;
    u8 memoryAccessState;
    u16 songId;
    u16 previousSongId;
    u8 playerId;
    u8 padding07;
    u16 specialSongId;
    u8 specialState;
    u8 specialPlayerId;
};

extern const u8 sStageRoomCounts[];
extern const u32 sUnk_86391C4[][6];

#define BGM_DEMO_BOSS_STAGE 0x1ED

void SetupDemo(void)
{
    u32 demoIndex;
    u8* passage;
    register u8* stage PIN("r9");
    register u8* addressLoad PIN("r2");
    const u8* configurations;
    u32 doubledIndex;
    const struct DemoConfiguration* configuration;
    int stageValue;
    s8* stageId;
    register const u8* data PIN("r4");
    u32 zero;
    u8* roomCount;
    u32 stageOffset;
    u32 passageOffset;
    const u8* field;
    register u8* bossData PIN("r2");
    s32 bossOffset;
    u32* collectionBase;
    u32 collectionOffset;

    demoIndex = sDemoOrder[gUnk_3000020];
    gHasTemporarySave = 0;
    gDifficulty = 0;

    addressLoad = &gCurrentPassage;
    passage = addressLoad;
    configurations = (const u8*)sDemoConfigurations;
    doubledIndex = demoIndex << 1;
    configuration = (const struct DemoConfiguration*)((doubledIndex + demoIndex) << 2);
    configuration = (const struct DemoConfiguration*)((u32)configuration + (u32)configurations);
    *addressLoad = configuration->passage;

    addressLoad = &gCurrentStageNumber;
    stage = addressLoad;
    stageValue = configuration->stage;
    *addressLoad = stageValue;
    gUnk_3000025 = configuration->room;

    stageId = &gCurrentStageID;
    data = (const u8*)sUnk_86391C4;
    stageOffset = *addressLoad << 2;
    passageOffset = *passage * 24;
    stageOffset += passageOffset;
    stageOffset += (u32)data;
    *stageId = *(const u32*)stageOffset;

    roomCount = &gStageRoomTableIndex;
    data = sStageRoomCounts;
    *roomCount = data[*stageId * 12];

    zero = 0;
    gGameMusicState.songId = configuration->songId;

    if (stageValue == 4) {
        bossData = (u8*)gStageTimerDigits;
        bossOffset = *stageId;
        bossOffset *= 12;
        field = data;
        field += 8;
        {
            u8* bossEntry;
            bossEntry = (u8*)bossOffset;
            bossEntry += (u32)field;
            bossData[0] = *bossEntry;
        }

        bossOffset = *stageId;
        bossOffset *= 12;
        field = data + 7;
        {
            u8* bossEntry;
            bossEntry = (u8*)bossOffset;
            bossEntry += (u32)field;
            bossData[1] = *bossEntry;
        }

        bossOffset = *stageId;
        bossOffset *= 12;
        field = data + 6;
        {
            u8* bossEntry;
            bossEntry = (u8*)bossOffset;
            bossEntry += (u32)field;
            bossData[2] = *bossEntry;
        }
        m4aSongNumStart(BGM_DEMO_BOSS_STAGE);
    }

    gPauseFlag = zero;
    gTimerState = zero;
    gSwitchPressed = zero;
    gStageEntryPauseMenuDisabled = zero;
    collectionBase = (u32*)gCurrentCollection;
    addressLoad = stage;
    collectionOffset = *addressLoad << 2;
    passageOffset = *passage * 24;
    collectionOffset += passageOffset;
    collectionOffset += (u32)collectionBase;
    addressLoad = 0;
    *(u32*)collectionOffset = (u32)addressLoad;

    if (gDemoState == DEMO_STATE_PLAYBACK) {
        volatile struct DemoDmaRegisters* dma;
        u32 pointerOffset;
        u32 control;
        const u16* counts;
        GBA_TYPEOF(const u16* const*, &(sDemoInputStreams)[0]) inputTable;

        dma = (volatile struct DemoDmaRegisters*)0x040000D4;
        inputTable = sDemoInputStreams;
        pointerOffset = demoIndex << 2;
        GP_SET(dma->source, GP(*(GBA_PTR(const u16*) const *)(pointerOffset + (u32)inputTable)));
        GP_SET(dma->destination, gDemoInputs);
        counts = sDemoInputDmaWordCounts;
        counts = (const u16*)((u32)doubledIndex + (u32)counts);
        control = *counts >> 1;
        control |= 0x80000000;
        dma->control = control;
        dma->control;

        GP_SET(dma->source, GP(*(GBA_PTR(const u16*) const *)((const u8*)sDemoInputLengthStreams + pointerOffset)));
        GP_SET(dma->destination, gDemoInputLengths);
        dma->control = control;
        dma->control;
    } else if (gDemoState == 3) {
        gDemoState = DEMO_STATE_PLAYBACK;
    }

    gUnk_3000020 |= 0x80;
}

void StopDemo(void)
{
    u32 demoIndex;
    struct WarioData* wario;
    const struct DemoConfiguration* configurations;
    const struct DemoConfiguration* configuration;

    gUnk_3000020 &= 0x7F;
    demoIndex = sDemoOrder[gUnk_3000020];

    if (gUnk_3000025 != 0) {
        wario = &gWarioData;
        configurations = sDemoConfigurations;
        configuration = (const struct DemoConfiguration*)(demoIndex * sizeof(*configuration));
        configuration = (const struct DemoConfiguration*)((u32)configuration + (u32)configurations);
        wario->xPosition = configuration->warioX;
        wario->yPosition = configuration->warioY;
    }

    m4aSongNumStop(BGM_WORK_IT);
}

void StartDemoPlayback(void)
{
    gDemoState = DEMO_STATE_PLAYBACK;
    SetupDemo();
}

void DemoVBlankCallback(void)
{
    m4aSoundVSync();
    m4aSoundMain();
}

u32 AdvanceDemo(void)
{
    u32 wrapped;
    u32 zero;

    vu16* ime;

    wrapped = 0;
    ime = (vu16*)0x04000208;
    zero = 0;
    *ime = wrapped;
    REG_DISPSTAT &= 0xFFEF;
    REG_IE &= 0xFFFD;
    REG_IME = 1;
    InterruptCallbackSetVBlank(DemoVBlankCallback);
    m4aMPlayAllStop();

    if (gDemoState == DEMO_STATE_RECORDING) {
        volatile struct DemoDmaRegisters* dma;
        u32 control;

        dma = (volatile struct DemoDmaRegisters*)0x040000D4;
        GP_SET(dma->source, gDemoInputs);
        GP_SET(dma->destination, (void*)0x02038C00);
        control = 0x80000100;
        dma->control = control;
        dma->control;
        GP_SET(dma->source, gDemoInputLengths);
        GP_SET(dma->destination, (void*)0x02038E00);
        dma->control = control;
        dma->control;
        func_8001000();
    }

    gDemoState = zero;
    gStageEntryPauseMenuDisabled = zero;
    gPauseFlag = zero;
    gUnk_3000022 = zero;
    gUnk_3000020++;

    if (gUnk_3000020 > 15) {
        gUnk_3000020 = zero;
        wrapped = 1;
    }

    InitializeSaveFileStatuses();
    return wrapped;
}

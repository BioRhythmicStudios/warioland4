#ifndef DEMO_H
#define DEMO_H

#include "gba.h"

#define DEMO_INPUT_SIZE 256

struct AGB_ALIGN DemoConfiguration {
    u8 passage;
    u8 stage;
    u8 room;
    u8 padding3;
    u16 warioX;
    u16 warioY;
    u16 songId;
    u16 paddingA;
};

extern const u16 sDemoInputDmaWordCounts[];
extern const struct DemoConfiguration sDemoConfigurations[];
extern const u8 sDemoOrder[];
extern GBA_PTR(const u16*) const sDemoInputStreams[];
extern GBA_PTR(const u16*) const sDemoInputLengthStreams[];

extern u16 gDemoInputs[DEMO_INPUT_SIZE];
extern u16 gDemoInputLengths[DEMO_INPUT_SIZE];
extern u16 gDemoSequenceIndex;
extern u16 gDemoButtonPressTimer;
extern u16 gDemoButtonsPressed;

void SetupDemo(void);
void StopDemo(void);
void StartDemoPlayback(void);
void DemoVBlankCallback(void);
u32 AdvanceDemo(void);

#endif  // DEMO_H

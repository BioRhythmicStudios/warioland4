#include "animated_graphics.h"
#include "global_data.h"

void UpdateAnimatedGraphics(void)
{
    struct AnimatedGraphicsState* graphicsState;
    int slotIndex;
    UpdateAnimatedGraphicsSwitches();
    graphicsState = gAnimatedGraphicsStates;
    slotIndex = 0;
    do {
        int frameChanged;
        frameChanged = 0;
        switch (graphicsState->mode) {
            case 0:
                break;
            case 1: {
                u8 updatedTimer;
                u8 zeroValue;
                updatedTimer = (graphicsState->frameTimer = graphicsState->frameTimer + 1);
                zeroValue = 0;
                if (updatedTimer == graphicsState->frameDelay) {
                    frameChanged = 1;
                    graphicsState->frameTimer = zeroValue;
                    graphicsState->frameIndex++;
                    if (graphicsState->frameIndex == graphicsState->frameCount) {
                        graphicsState->frameIndex = zeroValue;
                    }
                }
                break;
            }
            case 2:
                break;
            case 3:
                if (graphicsState->frameIndex != (graphicsState->frameCount - 1)) {
                    u8 updatedTimer;
                    u8 zeroValue;
                    updatedTimer = (graphicsState->frameTimer = graphicsState->frameTimer + 1);
                    zeroValue = 0;
                    if (updatedTimer == graphicsState->frameDelay) {
                        frameChanged = 1;
                        graphicsState->frameTimer = zeroValue;
                        graphicsState->frameIndex++;
                    }
                }
                break;
            case 4: {
                u8 updatedTimer;
                u8 zeroValue;
                updatedTimer = (graphicsState->frameTimer = graphicsState->frameTimer + 1);
                zeroValue = 0;
                if (updatedTimer == graphicsState->frameDelay) {
                    s8 updatedFrameIndex;
                    frameChanged = 1;
                    graphicsState->frameTimer = zeroValue;
                    graphicsState->frameIndex++;
                    updatedFrameIndex = graphicsState->frameIndex;
                    if (updatedFrameIndex == graphicsState->frameCount) {
                        graphicsState->frameIndex = 2 - updatedFrameIndex;
                    }
                }
                break;
            }
            case 5: {
                u8 currentFrameIndex;
                currentFrameIndex = graphicsState->frameIndex;
                if (graphicsState->frameIndex != 0) {
                    u8 updatedTimer;
                    u8 zeroValue;
                    updatedTimer = (graphicsState->frameTimer = graphicsState->frameTimer + 1);
                    zeroValue = 0;
                    if (updatedTimer == graphicsState->frameDelay) {
                        frameChanged = 1;
                        graphicsState->frameTimer = zeroValue;
                        graphicsState->frameIndex = currentFrameIndex - 1;
                    }
                }
                break;
            }
            case 6: {
                u8 updatedTimer;
                u8 zeroValue;
                updatedTimer = (graphicsState->frameTimer = graphicsState->frameTimer + 1);
                zeroValue = 0;
                if (updatedTimer == graphicsState->frameDelay) {
                    frameChanged = 1;
                    graphicsState->frameTimer = zeroValue;
                    {
                        int previousFrameIndex;
                        previousFrameIndex = (u8)graphicsState->frameIndex - 1;
                        graphicsState->frameIndex = previousFrameIndex;
                        if ((s32)(previousFrameIndex << 24) < 0) {
                            graphicsState->frameIndex = graphicsState->frameCount - 1;
                        }
                    }
                }
                break;
            }
        }
        if (frameChanged) {
            u8 sourceFrameIndex;
            u32 sourceAddress;
            u32 destinationAddress;
            vu32* dmaRegisters;
            sourceFrameIndex = graphicsState->frameIndex;
            if (graphicsState->frameIndex < 0) {
                int complementedFrameIndex;
                complementedFrameIndex = ~graphicsState->frameIndex;
                sourceFrameIndex = complementedFrameIndex + 1;
            }
            sourceAddress = (u32)GP(graphicsState->tiles) + (((s8)sourceFrameIndex) << 7);
            destinationAddress = 0x06004000 + (slotIndex << 7);
            dmaRegisters = (vu32*)0x040000D4;
            dmaRegisters[0] = sourceAddress;
            dmaRegisters[1] = destinationAddress;
            dmaRegisters[2] = 0x80000040;
            dmaRegisters[2];
        }
        slotIndex++;
        graphicsState++;
    } while (15 >= slotIndex);
}

void InitAnimatedGraphics(void)
{
    register struct AnimatedGraphicsState* graphicsState PIN("r3");
    int slotIndex;
    register const u8* switchStateTable PIN("sl");
    register const u8* slotSwitchIds PIN("r9");
    register const u16* activeAnimationIds PIN("r8");
    register const u16* inactiveAnimationIds PIN("ip");
    int zeroValue;
    int animationListOffset;
    vu32* dmaRegisters;
    graphicsState = gAnimatedGraphicsStates;
    slotIndex = 0;
    switchStateTable = gSwitchStates;
    slotSwitchIds = sAnimatedGraphicsSwitchIds;
    activeAnimationIds = sAnimatedGraphicsActiveIds;
    inactiveAnimationIds = sAnimatedGraphicsInactiveIds;
    zeroValue = 0;
    animationListOffset = 0;
    dmaRegisters = (vu32*)0x040000D4;
    do {
        const struct AnimatedGraphicsDefinition* definition;
        u8 roomId;
        u8 slotSwitchId;
        u32 animationIdAddress;
        s32 frameCount;
        roomId = *((u8*)(&gCurrentRoomHeader));
        slotSwitchId = slotSwitchIds[slotIndex + (roomId << 4)];
        if (switchStateTable[slotSwitchId] & 1) {
            animationIdAddress = animationListOffset;
            animationIdAddress += roomId << 5;
            animationIdAddress += (u32)activeAnimationIds;
        } else {
            animationIdAddress = animationListOffset;
            animationIdAddress += roomId << 5;
            animationIdAddress += (u32)inactiveAnimationIds;
        }
        {
            register u16 animationId PIN("r0");
            register u32 definitionOffset PIN("r1");
            const struct AnimatedGraphicsDefinition* definitionTable;
            animationId = *((u16*)animationIdAddress);
            definitionOffset = animationId << 3;
            definitionTable = sAnimatedGraphicsDefinitions;
            definition = (const struct AnimatedGraphicsDefinition*)(definitionOffset + ((u32)definitionTable));
        }
        graphicsState->mode = definition->mode;
        graphicsState->frameDelay = definition->frameDelay;
        frameCount = definition->frameCount;
        graphicsState->frameCount = frameCount;
        graphicsState->frameTimer = zeroValue;
        graphicsState->frameIndex = zeroValue;
        GP_SET(graphicsState->tiles, GP(definition->tiles));
        if ((graphicsState->mode == 3) || (graphicsState->mode == 6)) {
            u8 lastFrameIndex;
            lastFrameIndex = frameCount - 1;
            graphicsState->frameIndex = lastFrameIndex;
        }
        {
            int tileFrameOffset;
            u32 sourceAddress;
            u32 destinationAddress;
            u32 destinationBase;

            tileFrameOffset = graphicsState->frameIndex << 7;
            sourceAddress = (u32)GP(graphicsState->tiles);
            sourceAddress += tileFrameOffset;
            destinationAddress = slotIndex << 7;
            destinationBase = 0x06004000;
            destinationAddress += destinationBase;
            dmaRegisters[0] = sourceAddress;
            dmaRegisters[1] = destinationAddress;
        }
        {
            u32 dmaControl;

            dmaControl = 0x80000040;
            dmaRegisters[2] = dmaControl;
            dmaRegisters[2];
        }
        animationListOffset += 2;
        slotIndex++;
        graphicsState++;
    } while (slotIndex <= 15);
}

void UpdateAnimatedGraphicsSwitches(void)
{
    u8 switchId;

    switchId = 1;

    do {
        if (gSwitchStates[switchId] > 1) {
            gSwitchStates[switchId] ^= 3;
            ReloadAnimatedGraphicsForSwitch(switchId);
        }
        switchId++;
    } while (switchId <= 4);
}

void ReloadAnimatedGraphicsForSwitch(u8 switchId)
{
    struct AnimatedGraphicsState* graphicsState;
    int slotIndex;
    u8* selectedSwitchState;
    int zeroValue;
    vu32* dmaRegisters;
    int animationListOffset;
    register const u16* activeAnimationIds PIN("sl");
    register const u16* inactiveAnimationIds PIN("r9");
    int reversePlaybackModeStart;
    int slotLimitBias;
    int definitionOffsetCopy;
    int animationMode;
    graphicsState = gAnimatedGraphicsStates;
    slotIndex = 0;
    selectedSwitchState = gSwitchStates + switchId;
    zeroValue = 0;
    dmaRegisters = (vu32*)0x040000D4;
    animationListOffset = 0;
    activeAnimationIds = sAnimatedGraphicsActiveIds;
    inactiveAnimationIds = sAnimatedGraphicsInactiveIds;
    switchId = (u8)switchId;
    slotLimitBias = 1;
    do {
        int roomId;
        roomId = *((u8*)(&gCurrentRoomHeader));
        if (switchId == sAnimatedGraphicsSwitchIds[slotIndex + (roomId << 4)]) {
            u32 animationIdAddress;
            const struct AnimatedGraphicsDefinition* definition;
            u8 frameCount;
            {
                u8* selectedSwitchStatePtr;
                selectedSwitchStatePtr = selectedSwitchState;
                if ((*selectedSwitchStatePtr) != 0) {
                    animationIdAddress = animationListOffset;
                    animationIdAddress += roomId << 5;
                    animationIdAddress += (u32)activeAnimationIds;
                } else {
                    animationIdAddress = animationListOffset;
                    animationIdAddress += roomId << 5;
                    animationIdAddress += (u32)inactiveAnimationIds;
                }
            }
            {
                register u32 animationId PIN("r0");
                u32 definitionOffset;
                register const struct AnimatedGraphicsDefinition* definitionTable PIN("r0");
                animationId = *((u16*)animationIdAddress);
                definitionOffset = animationId << 3;
                definitionOffsetCopy = definitionOffset;
                definitionTable = sAnimatedGraphicsDefinitions;
                definition = (const struct AnimatedGraphicsDefinition*)(definitionOffsetCopy + ((u32)definitionTable));
            }
            graphicsState->mode = definition->mode;
            graphicsState->frameDelay = definition->frameDelay;
            frameCount = definition->frameCount;
            graphicsState->frameCount = frameCount;
            graphicsState->frameTimer = zeroValue;
            graphicsState->frameIndex = zeroValue;
            GP_SET(graphicsState->tiles, GP(definition->tiles));
            animationMode = definition->mode;
            reversePlaybackModeStart = 5;
            if ((animationMode <= 6) && (animationMode >= reversePlaybackModeStart)) {
                u8 lastFrameIndex;
                lastFrameIndex = frameCount - 1;
                graphicsState->frameIndex = lastFrameIndex;
            }
            {
                int tileFrameOffset;
                u32 sourceAddress;
                u32 destinationAddress;
                u32 destinationBase;
                tileFrameOffset = graphicsState->frameIndex << 7;
                sourceAddress = (u32)GP(graphicsState->tiles);
                sourceAddress += tileFrameOffset;
                destinationAddress = slotIndex << 7;
                destinationBase = 0x06004000;
                destinationAddress += destinationBase;
                dmaRegisters[0] = sourceAddress;
                dmaRegisters[1] = destinationAddress;
            }
            dmaRegisters[2] = 0x80000040;
            if (zeroValue) {
            }
            dmaRegisters[2];
        }
        animationListOffset += 2;
        slotIndex++;
        graphicsState++;
    } while (((slotIndex - 1) + 1) <= ((15 - slotLimitBias) + 1));
}

#include "cutscenes.h"
#include "fixed_point.h"
#include "gba.h"
#include "gba/m4a.h"
#include "demo.h"
#include "global_data.h"
#include "interrupt_callbacks.h"
#include "interrupts.h"
#include "main.h"
#include "oam.h"
#include "score.h"
#include "sound.h"
#include "voice_set.h"
#include "wario.h"

typedef struct MusicPlayerInfo MusicPlayerInfo;

#define CUTSCENE_GRAY(value) (((value) << 10) | ((value) << 5) | (value))
extern u8 sUnk_8283F14[];
extern u16 sUnk_82844B8[];
extern u16 sUnk_82844EE[];
extern u32 sUnk_8283F54[];
extern u8 sUnk_82844FC[];
extern u32 sUnk_828455C[];
extern u32 sUnk_8285A40[];
extern u16 sUnk_828566C[];
extern u16 sUnk_82856DE[];
extern u16 sUnk_828588C[];
extern u8 sUnk_8285FCC[];
extern u8 sUnk_828602C[];
extern u8 sUnk_82873E4[];
extern u8 sUnk_8287472[];
extern u8 sUnk_828759C[];
extern u8 sUnk_8287758[];
extern u16 sUnk_8288D2E[];
extern u16 sUnk_8288C48[];
extern u8 sUnk_82877B8[];
extern u8 sUnk_878E6D4[];
extern u8 sUnk_878E6D8[];
extern u8 sUnk_8289124[];
extern u8 sUnk_828AEC6[];
extern u8 sUnk_8289324[];
extern const u16 sUnk_828AE70[];
extern const u16 sUnk_828AE1A[];
extern const u16 sUnk_828ACD8[];
extern u8 sUnk_828AEE8[];
extern const s8 sUnk_878E6DC[];
extern u8 sUnk_828B210[];
extern u8 sUnk_828C570[];
extern u8 sUnk_828C710[];
extern u8 sUnk_828B234[];
extern u8 sUnk_828C730[];
extern const u16 sUnk_828E278[];
extern u16 sUnk_828E268[];
extern u16 sUnk_828E270[];
extern u8 sUnk_828E780[];
extern u16 sUnk_82940C8[];
extern u16 sUnk_8293FBE[];
extern u16 sUnk_8293FAC[];
extern u8 sUnk_828E880[];
extern const u16 sUnk_82941D2[];
extern const u16 sUnk_829420E[];
extern const u16 sUnk_829424A[];
extern const u16 sUnk_82942A2[];
extern u8 sUnk_82D40F4[];
extern u8 sUnk_82D830E[];
extern u16 sUnk_82D820C[];
extern u8 sUnk_82D41D4[];
extern u8 sUnk_82D8390[];
extern const u16 sUnk_82D825E[];
extern const u16 sUnk_82D8278[];
extern const u16 sUnk_82D8292[];
extern const u16 sUnk_82D82D0[];
extern struct MusicPlayerInfo gMPlayInfo_4;
extern u8 sUnk_8298990[];
extern u8 sUnk_82942FC[];
extern u16 sUnk_829892E[];
extern u16 sUnk_8298908[];
extern u16 sUnk_8295234[];
extern u16 sUnk_8295534[];
extern u16 sUnk_82953CA[];
extern u16 sUnk_82955A6[];
extern u16 sUnk_8295604[];
extern u16 sUnk_829571E[];
extern u16 sUnk_8295864[];
extern u16 sUnk_82958F2[];
extern u8 sUnk_829443C[];
extern u8 sUnk_8295954[];
extern u8 sUnk_8298A50[];
extern u8 sUnk_829B240[];
extern u8 sUnk_829D46E[];
extern u8 sUnk_829B2C0[];
extern u8 sUnk_829D4D0[];
extern u8 sUnk_829D194[];
extern u16 sUnk_829D1D4[];
extern u16 sUnk_829D212[];
extern u16 sUnk_829D270[];
extern const u16 sUnk_829F95E[];
extern const u16 sUnk_829F9B4[];
extern const u16 sUnk_829FA0A[];
extern const u16 sUnk_829FA48[];
extern u8 sUnk_829FD48[];
extern u8 sUnk_82A2F12[];
extern u8 sUnk_82A36AC[];
extern u8 sUnk_829FF48[];
extern u8 sUnk_82A2F74[];
extern u8 sUnk_82A36CC[];
extern u8 sUnk_82A3710[];
extern u16 sUnk_82A1CF0[];
extern u16 sUnk_82A1E7A[];
extern u16 sUnk_82A1FBC[];
extern u8 sUnk_82A229C[];
extern u8 sUnk_82A2840[];
extern u8 sUnk_82A2A24[];
extern const u16 sUnk_878E6E0[];
extern const u16 sUnk_878E6F0[];
extern const u16 sUnk_878E700[];
extern const u16 sUnk_878E718[];
extern const u16 sUnk_878E730[];
extern const u16 sUnk_878E73C[];
extern u8 sUnk_82A37A0[];
extern u8 sUnk_82A4E1A[];
extern u32 sUnk_82A37C0[];
extern u32 sUnk_82A4E3C[];
extern u16 sUnk_82A4934[];
extern u8 sUnk_82A5318[];
extern u8 sUnk_82A53F8[];
extern u16 sUnk_82A2CD0[];
extern u8 sUnk_82A8820[];
extern u8 sUnk_82AA5B2[];
extern u8 sUnk_82A88E0[];
extern u8 sUnk_82A9508[];
extern u8 sUnk_82AA7B4[];
extern u16 sUnk_82AA202[];
extern u16 sUnk_82AA07C[];
extern u16 sUnk_82AA2B0[];
extern u8 sUnk_82CFB22[];
extern u8 sUnk_82CAEE6[];
extern u8 sUnk_82CAF08[];
extern u16 sUnk_82CBD78[];
extern u8 sUnk_82D1060[];
extern u16 sUnk_82D239C[];
extern u8 sUnk_82CBE44[];
extern u16 sUnk_82CD390[];
extern u8 sUnk_82CD4B4[];
extern u16 sUnk_82CE598[];
extern u8 sUnk_82CFB44[];
extern u16 sUnk_82D0F64[];
extern u8 sUnk_82CE704[];
extern u16 sUnk_82CFA14[];
extern u8 sUnk_82C9FCC[];
extern u8 sUnk_82CA00C[];
extern u16 sUnk_82CAD88[];
extern u8 sUnk_82CAC74[];
extern u8 sUnk_82D248E[];
extern u8 sUnk_82D24B0[];
extern u8 sEndingCutsceneBackgroundPalette[];
extern u8 sEndingCutsceneSpritePalette[];
extern u8 sEndingCutsceneBackground0Graphics[];
extern u8 sEndingCutsceneBackground1Graphics[];
extern u8 sEndingCutsceneBackground2Graphics[];
extern u8 sEndingCutsceneSpriteGraphics[];
extern u16 sEndingCutsceneBackground0TileRuns[];
extern u16 sEndingCutsceneBackground1TileRuns[];
extern u16 sEndingCutsceneBackground2TileRuns[];
extern u8 sUnk_82D9C78[];
extern u8 sUnk_82D9D18[];
extern u16 sUnk_82DC454[];
extern u16 sUnk_82DC66E[];
extern u16 sUnk_82DC902[];
extern u16 sUnk_82DCBA4[];
extern u16 sUnk_82DC878[];
extern u16 sUnk_82DCE4A[];
extern u16 sUnk_82DCFCC[];
extern u16 sUnk_82DC738[];
extern u16 sUnk_82DC7EA[];
extern u16 sUnk_82DCD16[];
extern u16 sUnk_82DCDC8[];
extern const u16 sUnk_8285C08[];
extern const u16 sUnk_8285C10[];
extern const u16 sUnk_8285C1E[];
extern const u16 sUnk_8285C32[];
extern const u16 sUnk_8285C4C[];
extern const u16 sUnk_8285C6C[];
extern const u16 sUnk_8285C92[];
extern const u16 sUnk_8285CBE[];
extern const u16 sUnk_8285CF0[];
extern const u16 sUnk_8285D28[];
extern const u16 sUnk_8285D66[];
extern const u16 sUnk_8285DAA[];
extern const u16 sUnk_8285DF4[];
extern const u16 sUnk_8285E44[];
extern const u16 sUnk_8285E9A[];
extern const u16 sUnk_8285EF6[];
extern const u16 sUnk_8285F58[];
extern const u16 sUnk_8285F60[];
extern const u16 sUnk_8285F68[];
extern const u16 sUnk_8285F76[];
extern const u16 sUnk_8285F7E[];
extern const u16 sUnk_8285F8C[];
extern const u16 sUnk_8285F9A[];
extern const u16 sUnk_8285FAE[];
extern const u16 sUnk_828B1C0[];
extern const u16 sUnk_828E304[];
extern const u16 sUnk_828E330[];
extern const u16 sUnk_828E362[];
extern const u16 sUnk_828E394[];
extern const u16 sUnk_828E3F2[];
extern const u16 sUnk_828E41E[];
extern const u16 sUnk_828E476[];
extern const u16 sUnk_828E4A2[];
extern const u16 sUnk_828E4E0[];
extern const u16 sUnk_828E506[];
extern const u16 sUnk_828E52C[];
extern const u16 sUnk_828E540[];
extern const u16 sUnk_828E566[];
extern const u16 sUnk_828E58C[];
extern const u16 sUnk_828E5AC[];
extern const u16 sUnk_828E5CC[];
extern const u16 sUnk_828E5EC[];
extern const u16 sUnk_828E61E[];
extern const u16 sUnk_828E632[];
extern const u16 sUnk_828E646[];
extern const u16 sUnk_828E660[];
extern const u16 sUnk_828E66E[];
extern const u16 sUnk_828E68E[];
extern const u16 sUnk_828E6BA[];
extern const u16 sUnk_828E6F8[];
extern const u16 sUnk_828E70C[];
extern const u16 sUnk_828E714[];
extern const u16 sUnk_828E71C[];
extern const u16 sUnk_828E74E[];
extern const u16 sUnk_878E760[];
extern const s16 sUnk_878E770[];
extern const u32 sUnk_878E778[];
void UpdateAffineCutsceneBackground(s32 frame);
void UpdateCutsceneSequence150(s32 frame);
void UpdateCutsceneBackgroundVariantTransition(s32 frame);
void UpdateLayeredBackgroundTransition(s32 inputFrame);
void UpdateLayeredSpriteCutsceneSequence(s32 inputFrame);
u16* AppendCutsceneScaledSpritePair(u16* destination);
void func_8005500(s32 frame);
void func_80058AC(s32 frame);
void InitializeSubGameTransitionGraphics(void);
void UpdateLayeredSubGameCutscene(s32 inputFrame);
u16* AppendLayeredWarioCutsceneOam(u16* destination, s32 mode, s32 timer);
void func_8006680(void);
void UpdateLayeredEndingCutsceneSequence(s32 inputFrame);
void UpdateLayeredWarioCutsceneSequence(s32 inputFrame);
void InitializeEndingTreasureCutscene(void);
void InitializeEndingBackgroundSetup(void);
void UpdateEndingScrollCutscene(s32 frame);
void InitializeEndingCutsceneParticlePositions(s32 seed);
void UpdateEndingCutsceneParticlePositions(void);
void InitializeEndingCutscene(void);
u16* BuildCutsceneBackgroundAffineMatrix(u16* destination, s32 angle, s32 scaleX, s32 scaleY, s32 centerX, s32 centerY);
void WaitForVBlankInterrupt(void);
// minigame_transition.h declares WaitForVBlankInterrupt differently, so it is not included.
void SubGameClearGraphicsMemory(void);
u16* AppendCutsceneOamTemplate(const u16* source, u16* destination, s32 xOffset, s32 yOffset);
void FinalizeCutsceneOamBuffer(u16* destination);
void ResetCutsceneBackgroundRegisters(void);

void InitializeAffineCutsceneBackground(void)
{
    vu32* dma;
    s32 fill;
    vu16* registers;
    u16* affine;

    fill = 0;
    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)&fill;
    dma[1] = 0x06003FC0;
    dma[2] = 0x85000010;
    dma[2];

    fill = -1;
    dma[0] = (u32)&fill;
    dma[1] = 0x06008000;
    dma[2] = 0x85000400;
    dma[2];

    dma[0] = (u32)sUnk_8283F14;
    dma[1] = PLTT;
    dma[2] = 0x80000020;
    dma[2];

    CopyCutsceneTilemapRect(sUnk_82844B8, (u16*)0x060088E8, 9, 3, 16);
    DecodeCutsceneTileRuns(sUnk_82844EE, (u16*)0x06008000);

    REG_BG0CNT = 0x1001;
    REG_BG2CNT = 0x5180;
    gUnk_3002C2A = 8;
    gUnk_3002C2E = 0x300;
    gUnk_3002C30 = 2;

    BuildCutsceneBackgroundAffineMatrix(gUnk_3002C32, (u16)gUnk_3002C2C, (u16)gUnk_3002C2E, 2, 120 - gUnk_3002C28, 72);

    affine = gUnk_3002C32;
    registers = (vu16*)REG_ADDR_BG2PA;
    *registers++ = *affine++;
    *registers++ = *affine++;
    *registers++ = *affine++;
    *registers++ = *affine++;
    *registers++ = *affine++;
    *registers++ = *affine++;
    *registers++ = *affine++;
    *registers = *affine;

    REG_DISPCNT = 1;
}

void UpdateAffineCutsceneBackground(s32 frame)
{
    switch ((u32)gUnk_3002C60) {
        case 0:
            LZ77UnCompVram(sUnk_8283F54, (void*)BG_CHAR_ADDR(0));
            gUnk_3002C64 = 0;
            gUnk_3002C60++;
            m4aSongNumStartOrChange(0x27B);
            break;
        case 1:
            gUnk_3002C64++;
            if (gUnk_3002C64 == 1) {
                REG_DISPCNT = 0x501;
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;
        case 2: {
            u32 timer;

            if ((u16)gUnk_3002C2E > 0x100) {
                gUnk_3002C2E -= 0x10;
            }
            timer = gUnk_3002C64;
            if (timer <= 27) {
                if ((frame & 3) == 3) {
                    gUnk_3002C30 = gUnk_3002C30 * 2;
                }
            } else if (timer <= 31) {
                gUnk_3002C30 += 0x10;
            } else {
                if ((u16)gUnk_3002C30 <= 0x100) {
                    gUnk_3002C30 = 0x100;
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                    break;
                }
                gUnk_3002C30 -= 0x10;
            }
            gUnk_3002C64++;
            break;
        }
        case 3: {
            s32 timer;
            s32 value;
            s32 color;

            timer = (u32)gUnk_3002C64 >> 1;
            gUnk_3002C64++;
            if (timer <= 31) {
                value = (timer * 15) * 2;
                if (value < 0) {
                    value += 31;
                }
                color = value >> 5;
                *(vu16*)0x0500002A = CUTSCENE_GRAY(color);

                value = (timer * 5) << 2;
                if (value < 0) {
                    value += 31;
                }
                color = value >> 5;
                *(vu16*)0x0500002C = CUTSCENE_GRAY(color);

                timer = timer * 3;
                value = timer << 2;
                if (value < 0) {
                    value += 31;
                }
                color = value >> 5;
                *(vu16*)0x0500002E = CUTSCENE_GRAY(color);

                value = timer << 1;
                if (value < 0) {
                    value += 31;
                }
                color = value >> 5;
                *(vu16*)0x05000032 = CUTSCENE_GRAY(color);
            } else {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
                InitCutsceneDarkenBlend(0x25);
            }
            break;
        }
        case 4:
            IncreaseCutsceneBlendCoefficient(3);
            gUnk_3002C64++;
            if (gUnk_3002C64 == 51) {
                gSubGameMode++;
            }
            break;
    }
}

void InitializeVariantCutsceneBackground(void)
{
    vu32* dma;
    u16 fillValue;
    const u16* tileRuns;
    u16* fillPtr;
    u32* variant;
    u32 variantValue;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_82844FC;
    dma[1] = PLTT;
    dma[2] = 0x80000030;
    dma[2];

    dma[0] = (u32)sUnk_82844FC;
    dma[1] = OBJ_PLTT;
    dma[2] = 0x80000030;
    dma[2];

    LZ77UnCompVram(sUnk_828455C, (void*)VRAM);
    LZ77UnCompVram(sUnk_8285A40, OBJ_VRAM0);

    fillPtr = &fillValue;
    fillValue = 0;
    dma[0] = (u32)fillPtr;
    dma[1] = 0x06008000;
    dma[2] = 0x81000800;
    dma[2];

    DecodeCutsceneTileRuns(sUnk_828566C, (u16*)0x06008000);

    variant = &gUnk_3002C7C;
    variantValue = *variant;
    tileRuns = sUnk_82856DE;
    if (variantValue != 0) {
        tileRuns = sUnk_828588C;
    }
    DecodeCutsceneTileRuns(tileRuns, (u16*)0x06008800);

    REG_BG0CNT = 0x1000;
    REG_BG1CNT = 0x1100;
    gCutsceneBackgroundVerticalOffset = 0;
    InitCutsceneDarkenBlendFromBlack(19);
    WaitForVBlankInterrupt();

    if (*variant != 0) {
        REG_DISPCNT = 0x1000;
    } else {
        REG_DISPCNT = 0x300;
    }
}

void UpdateCutsceneSequence150(s32 frame)
{
    const u16* oamFrame;
    s32 animationFinished;
    u32 timer;
    s16 verticalOffset;

    oamFrame = 0;
    animationFinished = 0;

    if (gUnk_3002C7C == 0) {
        switch (gUnk_3002C60) {
            case 0:
                DecreaseCutsceneBlendCoefficient(3);
                {
                    u32 loadedTimer;
                    u32 oldTimer;
                    loadedTimer = gUnk_3002C64;
                    oldTimer = loadedTimer;
                    loadedTimer++;
                    gUnk_3002C64 = loadedTimer;
                    timer = oldTimer;
                }
                if (timer > 120) {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
                break;

            case 1:
                verticalOffset = gCutsceneBackgroundVerticalOffset;
                if (verticalOffset <= 95) {
                    if (gCutsceneBackgroundVerticalOffset <= 79 && (frame & 1) != 0) {
                        gCutsceneBackgroundVerticalOffset++;
                    } else if (gCutsceneBackgroundVerticalOffset <= 89 && (frame & 3) == 3) {
                        gCutsceneBackgroundVerticalOffset++;
                    } else if ((frame & 7) == 7) {
                        gCutsceneBackgroundVerticalOffset++;
                    }
                    *(vu16*)0x04000016 = gCutsceneBackgroundVerticalOffset;
                } else {
                    {
                        u32 loadedTimer;
                        u32 oldTimer;
                        loadedTimer = gUnk_3002C64;
                        oldTimer = loadedTimer;
                        loadedTimer++;
                        gUnk_3002C64 = loadedTimer;
                        timer = oldTimer;
                    }
                    if (timer > 80) {
                        gUnk_3002C64 = 0;
                        gSubGameMode++;
                    }
                }
                break;
        }
        return;
    }

    switch (gUnk_3002C60) {
        case 0:
            DecreaseCutsceneBlendCoefficient(3);
            {
                u32 loadedTimer;
                u32 oldTimer;
                loadedTimer = gUnk_3002C64;
                oldTimer = loadedTimer;
                loadedTimer++;
                gUnk_3002C64 = loadedTimer;
                timer = oldTimer;
            }
            if (timer > 120) {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 1:
            if (gUnk_3002C78 != 0) {
                animationFinished = SelectCutsceneAnimationSequence150B(gUnk_3002C64++, &oamFrame);
            } else {
                animationFinished = SelectCutsceneAnimationSequence150A(gUnk_3002C64++, &oamFrame);
            }
            if (animationFinished != 0) {
                gUnk_3002C68 = 0;
                gUnk_3002C60++;
            }
            break;

        case 2:
            if (gUnk_3002C78 != 0) {
                SelectCutsceneAnimationSequence150B(gUnk_3002C64, &oamFrame);
            } else {
                SelectCutsceneAnimationSequence150A(gUnk_3002C64, &oamFrame);
            }
            if (gUnk_3002C68 == 65) {
                m4aSongNumStartOrChange(0x27C);
            }
            {
                u32 loadedTimer;
                u32 oldTimer;
                loadedTimer = gUnk_3002C68;
                oldTimer = loadedTimer;
                loadedTimer++;
                gUnk_3002C68 = loadedTimer;
                timer = oldTimer;
            }
            if (timer > 120 && IncreaseCutsceneBlendCoefficient(3) != 0) {
                gSubGameMode++;
            }
            break;
    }

    {
        u16* buffer;
        register u16* destination PIN("r1");
        buffer = gOamBuffer;
        destination = AppendCutsceneOamTemplate(oamFrame, buffer, 120, 80);
        FinalizeCutsceneOamBuffer(destination);
    }
}

void InitializeCutsceneBackgroundGraphics(void)
{
    vu32* dma;
    vu16 zero;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_8285FCC;
    dma[1] = PLTT;
    dma[2] = 0x80000030;
    dma[2];

    LZ77UnCompVram((const u32*)sUnk_828602C, (void*)VRAM);

    zero = 0;
    dma[0] = (u32)&zero;
    dma[1] = 0x06008000;
    dma[2] = 0x81000C00;
    dma[2];

    DecodeCutsceneTileRuns((u16*)sUnk_82873E4, (u16*)0x06008000);
    DecodeCutsceneTileRuns((u16*)sUnk_8287472, (u16*)0x06008800);
    DecodeCutsceneTileRuns((u16*)sUnk_828759C, (u16*)0x06009000);

    REG_BG0CNT = 0x1000;
    REG_BG1CNT = 0x1100;
    REG_BG2CNT = 0x1200;
    REG_BLDCNT = 0;
    REG_DISPCNT = 0;
}

void UpdateCutsceneDisplayTimeline(u16 frame)
{
    s32* timerAddress;
    s32* timerCopy;
    s32 timer;

    timerAddress = &gUnk_3002C64;
    timer = *timerAddress;
    timerCopy = timerAddress;
    if (timer == 30) {
        REG_DISPCNT = 0x100;
    } else if (timer == 32) {
        REG_DISPCNT = 0x200;
    } else if (timer == 34) {
        REG_DISPCNT = 0x400;
    } else if (timer == 234) {
        REG_DISPCNT = 0;
    } else if (timer == 236) {
        gSubGameMode++;
    }
    (*timerCopy)++;
}

void InitializeLayeredScrollCutscene(void)
{
    vu32* dma;
    u16 fillValue;
    u16* fillPointer;
    u32 fillValue32;
    s32 horizontalOffset;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_8287758;
    dma[1] = PLTT;
    dma[2] = 0x80000030;
    dma[2];

    fillPointer = &fillValue;
    fillValue = 0;
    dma[0] = (u32)fillPointer;
    dma[1] = 0x06007FE0;
    dma[2] = 0x81000010;
    dma[2];

    fillValue32 = 0x03FF03FF;
    dma[0] = (u32)&fillValue32;
    dma[1] = 0x06008000;
    dma[2] = 0x85000800;
    dma[2];

    DecodeCutsceneTileRuns(sUnk_8288D2E, (u16*)0x06008000);
    DecodeCutsceneTileRuns(sUnk_8288C48, (u16*)0x06009000);

    fillValue = 21;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06009440;
    dma[2] = 0x81000060;
    dma[2];

    fillValue = 21;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06009C40;
    dma[2] = 0x81000060;
    dma[2];

    LoadCutsceneBackgroundVariant(0);

    fillValue = 21;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x0600A000;
    dma[2] = 0x81000220;
    dma[2];

    fillValue = 0x03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x0600A440;
    dma[2] = 0x810001E0;
    dma[2];

    REG_BG0CNT = 0x5000;
    REG_BG1CNT = 0x5200;
    REG_BG2CNT = 0x1400;
    gCutsceneBackgroundHorizontalOffset = (horizontalOffset = -192);
    REG_BG0HOFS = gCutsceneBackgroundHorizontalOffset;
    REG_BG1HOFS = gCutsceneBackgroundHorizontalOffset;
    REG_DISPCNT = 0;
}

void LoadCutsceneBackgroundVariant(u32 variant)
{
    u32 index;
    s32 i;
    u16 value;
    u16* destination;

    index = variant & 3;
    DecodeCutsceneTileRuns(GP(sUnk_878E790[index]), (u16*)0x06008000);
    CopyCutsceneTilemapRect(sUnk_8288E6C + index * 8, (u16*)0x0600934E, 2, 4, 32);
    CopyCutsceneTilemapRect(sUnk_8288EAC + index * 36, (u16*)0x06009220, 4, 9, 32);
    DecodeCutsceneTileRuns(GP(sUnk_878E780[index]), (u16*)0x06009000);
    value = sUnk_878E6CC[index];
    destination = (u16*)0x06009C7E;
    for (i = 23; i >= 0; i--) {
        *destination = value;
        destination--;
    }
}

void UpdateCutsceneBackgroundVariantTransition(s32 frame)
{
    short horizontal;
    s32 signedHorizontal;
    u32 timer;
    switch (gUnk_3002C60) {
        case 0:
            LZ77UnCompVram(sUnk_82877B8, (void*)0x06000000);
            InitCutsceneDarkenBlendFromBlack(39);
            *((vu16*)0x04000000) = 0x0700;
            WaitForVBlankInterrupt();
            *((vu16*)0x05000000) = 0x7FFF;
            gUnk_3002C60++;
            break;

        case 1: {
            u16 horizontal1;
            u32 nextHorizontal;
            s32 signedHorizontal1;
            DecreaseCutsceneBlendCoefficient(15);
            horizontal1 = gCutsceneBackgroundHorizontalOffset;
            signedHorizontal1 = (s16)horizontal1;
            if (signedHorizontal1 < (-24)) {
                if ((frame & 3) == 3) {
                    nextHorizontal = horizontal1 + 1;
                    gCutsceneBackgroundHorizontalOffset = nextHorizontal;
                    *((vu16*)0x04000010) = nextHorizontal;
                    *((vu16*)0x04000014) = nextHorizontal;
                }
            }
            if (frame == 510) {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;
        }

        case 2: {
            u32 eventTimer;
            u32* timerPointer;
            u32* timerPointerWork;
            vu16* bg2Vertical;
            register u32 work PIN("r0");

            eventTimer = gUnk_3002C64;
            switch (eventTimer) {
                case 32:
                    LoadCutsceneBackgroundVariant(1);
                    break;
                case 64:
                    LoadCutsceneBackgroundVariant(2);
                    break;
                case 146:
                    LoadCutsceneBackgroundVariant(3);
                    break;
                case 396:
                    gSubGameMode++;
                    break;
            }

            bg2Vertical = (vu16*)0x0400001A;
            timerPointerWork = &gUnk_3002C64;
            work = *timerPointerWork;
            work >>= 4;
            *bg2Vertical = work;

            {
                u32 horizontalAddress;
                u16 horizontalValue;
                u32 zero;
                s32 signedValue;
                s16* horizontalPointer;

                horizontalAddress = (u32)&gCutsceneBackgroundHorizontalOffset;
                horizontalValue = *(u16*)horizontalAddress;
                zero = 0;
                signedValue = *(s16*)(horizontalAddress + zero);
                horizontalPointer = (s16*)horizontalAddress;
                timerPointer = timerPointerWork;

                if (signedValue <= 15) {
                    if (signedValue < -8) {
                        if ((frame & 7) == 7) {
                            work = horizontalValue + 1;
                            goto storeHorizontal;
                        }
                    }

                    {
                        u16 middleHorizontal;
                        s32 middleSigned;

                        middleHorizontal = *(u16*)horizontalPointer;
                        work = 0;
                        middleSigned = *(s16*)((u8*)horizontalPointer + work);
                        if (middleSigned < -4) {
                            if ((frame & 15) == 15) {
                                work = middleHorizontal + 1;
                                goto storeHorizontal;
                            }
                        }
                        if ((frame & 31) == 31) {
                            work = *(u16*)horizontalPointer;
                            work++;
                        storeHorizontal:
                            *(u16*)horizontalPointer = work;
                        }
                    }

                    {
                        vu16* bg0Horizontal;
                        u32 currentHorizontal;
                        u32 maskedHorizontal;
                        vu16* bg2Horizontal;

                        bg0Horizontal = (vu16*)0x04000010;
                        currentHorizontal = *(u16*)horizontalPointer;
                        maskedHorizontal = 0x1FF;
                        maskedHorizontal &= currentHorizontal;
                        *bg0Horizontal = maskedHorizontal;
                        bg2Horizontal = (vu16*)0x04000014;
                        *bg2Horizontal = maskedHorizontal;
                    }
                }
            }

            {
                u32 timerValue;
                timerValue = *timerPointer;
                if (timerValue > 146) {
                    vu16* blendY;
                    const u8* waveTable;
                    u32 waveMask;

                    blendY = (vu16*)0x04000012;
                    waveTable = sUnk_878E6D8;
                    waveMask = 3;
                    timerValue &= waveMask;
                    work = timerValue + (u32)waveTable;
                    *blendY = *(u8*)work;
                } else if (timerValue > 64) {
                    vu16* blendY;
                    const u8* waveTable;
                    u32 waveIndex;
                    u32 waveMask;

                    blendY = (vu16*)0x04000012;
                    waveTable = sUnk_878E6D4;
                    waveIndex = timerValue >> 1;
                    waveMask = 3;
                    waveIndex &= waveMask;
                    waveIndex = waveIndex + (u32)waveTable;
                    *blendY = *(u8*)waveIndex;
                }
            }

            if (*timerPointer == 184) {
                m4aSongNumStartOrChange(636);
            }
            (*timerPointer)++;
            break;
        }
    }
}

void InitializeWindowedCutsceneBackgrounds(void)
{
    vu32* dma;
    u32 fillValue;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_8289124;
    dma[1] = PLTT;
    dma[2] = 0x80000100;
    dma[2];

    dma[0] = (u32)sUnk_828AEC6;
    dma[1] = OBJ_PLTT;
    dma[2] = 0x80000010;
    dma[2];

    fillValue = 0;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06007FE0;
    dma[2] = 0x85000008;
    dma[2];

    fillValue = 0x03FF03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06008000;
    dma[2] = 0x85000C00;
    dma[2];

    REG_BG0CNT = 0xD000;
    REG_BG1CNT = 0xD401;
    REG_BG2CNT = 0xD802;
    gCutsceneBackgroundHorizontalOffset = 0x100;
    REG_BG0HOFS = 0x100;
    REG_BG1HOFS = 0x100;
    REG_DISPCNT = 0;
    REG_WIN0H = 0xF0;
    REG_WIN0V = 0x1090;
    REG_WININ = 0x37;
    REG_WINOUT = 0;
}

void UpdateLayeredBackgroundTransition(s32 inputFrame)
{
    s32 frame;
    u8* oam;
    u16* destination;
    s32 count;
    const s8* new_var2;
    u32 new_var;
    vu32* dma;
    s32 maxCount;
    s32 x;
    s16 offset;
    s16 wave;
    s32 waveIndex;
    s32 range;
    frame = inputFrame;
    switch (gUnk_3002C60) {
        case 0:
            LZ77UnCompVram(sUnk_8289324, (void*)0x06000000);
            DecodeCutsceneTileRuns(sUnk_828AE70, (u16*)0x06008000);
            DecodeCutsceneTileRuns(sUnk_828AE1A, (u16*)0x0600A000);
            DecodeCutsceneTileRuns(sUnk_828ACD8, (u16*)0x0600C000);
            gUnk_3002C60++;
            break;

        case 1:
            LZ77UnCompVram(sUnk_828AEE8, (void*)0x06010000);
            gUnk_3002C60++;
            *((vu16*)0x04000000) = 0x3700;
            InitCutsceneBrightenBlend(55);
            break;

        case 2: {
            u16 rawOffset;
            s32 signedOffset;
            u16 updatedOffset;

            rawOffset = gCutsceneBackgroundHorizontalOffset;
            signedOffset = gCutsceneBackgroundHorizontalOffset;
            if (signedOffset > 0) {
                if (signedOffset > 80) {
                    updatedOffset = rawOffset - 2;
                    gCutsceneBackgroundHorizontalOffset = updatedOffset;
                }
                offset = gCutsceneBackgroundHorizontalOffset;
                if (offset > 40) {
                    gCutsceneBackgroundHorizontalOffset--;
                }
                offset = gCutsceneBackgroundHorizontalOffset;
                if (offset > 16) {
                    gCutsceneBackgroundHorizontalOffset--;
                } else if ((offset > 8) && ((frame & 1) != 0)) {
                    gCutsceneBackgroundHorizontalOffset--;
                } else {
                    offset = gCutsceneBackgroundHorizontalOffset;
                    if ((offset > 4) && ((frame & 3) == 3)) {
                        gCutsceneBackgroundHorizontalOffset--;
                    } else if ((frame & 7) == 7) {
                        gCutsceneBackgroundHorizontalOffset--;
                    }
                }
                *((vu16*)0x04000010) = gCutsceneBackgroundHorizontalOffset & 0x1FF;
                *((vu16*)0x04000014) = gCutsceneBackgroundHorizontalOffset & 0x1FF;
            }
        }
            waveIndex = frame;
            waveIndex &= 3;
            new_var2 = &sUnk_878E6DC[waveIndex];
            wave = *new_var2;
            *((vu16*)0x04000012) = wave;
            ;
            *((vu16*)0x04000014) = (gCutsceneBackgroundHorizontalOffset - wave) & 0x1FF;
            new_var = (u32)(((u8*)0x05000020) + ((frame % 13) * 32));
            {
                u32 dmaValue;
                dma = (vu32*)0x040000D4;
                dma[0] = new_var;
                ASM_BARRIER(: "=r"(dmaValue) : "0"((u32)0x05000020));
                dma[1] = dmaValue;
                dmaValue = 0x84000008;
                dma[2] = dmaValue;
                dmaValue = dma[2];
            }
            if (gCutsceneBackgroundHorizontalOffset == 240) {
                *((vu16*)0x04000054) = 1;
            } else if (gCutsceneBackgroundHorizontalOffset == 238) {
                *((vu16*)0x04000054) = 2;
            } else if (gCutsceneBackgroundHorizontalOffset == 236) {
                *((vu16*)0x04000054) = 3;
            } else if (gCutsceneBackgroundHorizontalOffset == 120) {
                *((vu16*)0x04000054) = 16;
            } else if (gCutsceneBackgroundHorizontalOffset == 116) {
                *((vu16*)0x04000054) = 15;
            } else if (gCutsceneBackgroundHorizontalOffset == 112) {
                *((vu16*)0x04000054) = 14;
            } else if (gCutsceneBackgroundHorizontalOffset == 108) {
                *((vu16*)0x04000054) = 13;
            } else if (gCutsceneBackgroundHorizontalOffset == 104) {
                *((vu16*)0x04000054) = 12;
            } else if (gCutsceneBackgroundHorizontalOffset == 100) {
                *((vu16*)0x04000054) = 11;
            } else if (gCutsceneBackgroundHorizontalOffset == 96) {
                *((vu16*)0x04000054) = 10;
            } else if (gCutsceneBackgroundHorizontalOffset == 92) {
                *((vu16*)0x04000054) = 9;
            } else if (gCutsceneBackgroundHorizontalOffset == 88) {
                *((vu16*)0x04000054) = 8;
            } else if (gCutsceneBackgroundHorizontalOffset == 84) {
                *((vu16*)0x04000054) = 7;
            } else if (gCutsceneBackgroundHorizontalOffset == 80) {
                *((vu16*)0x04000054) = 6;
            } else if (gCutsceneBackgroundHorizontalOffset == 78) {
                *((vu16*)0x04000054) = 5;
            } else if (gCutsceneBackgroundHorizontalOffset == 76) {
                *((vu16*)0x04000054) = 4;
            } else if (gCutsceneBackgroundHorizontalOffset == 74) {
                *((vu16*)0x04000054) = 3;
            }
            if (frame == 350) {
                gSubGameMode++;
            }
            SelectAlternatingCutsceneFrame(frame, &oam);
            range = frame - 140;
            if ((((u32)range) <= 11) || (((u32)(range - 24)) <= 11)) {
                oam = 0;
            }
            if (oam != 0) {
                count = 128 / (*((u16*)oam));
            } else {
                count = 0;
            }
            {
                register s32 signedMax PIN("r2");
                s32 roundedMax;
                signedMax = gCutsceneBackgroundHorizontalOffset;
                roundedMax = signedMax;
                if (signedMax < 0) {
                    roundedMax += 15;
                }
                roundedMax >>= 4;
                maxCount = roundedMax;
            }
            if (count > maxCount) {
                count = maxCount;
            }
            destination = gOamBuffer;
            if (count != 0) {
                count--;
                if (count != (-1)) {
                    {
                        s32 signedWave;
                        signedWave = wave;
                        x = (count * 8) - 128;
                        do {
                            destination = AppendCutsceneOamTemplate(
                                (u16*)oam, destination, (-gCutsceneBackgroundHorizontalOffset) - x, signedWave + 64
                            );
                            x -= 8;
                            count--;
                        } while (count != (-1));
                    }
                }
            } else {
                s32 elseOffset;
                ASM_BARRIER(: "=r"(elseOffset));
#ifdef PORTABLE
                elseOffset = gCutsceneBackgroundHorizontalOffset;  // still in r2 on the GBA
#endif
                destination = AppendCutsceneOamTemplate((u16*)oam, destination, (-elseOffset) + 128, wave + 64);
            }
            FinalizeCutsceneOamBuffer(destination);
            break;
    }
}

void InitializeScaledSpriteCutscene(void)
{
    vu32* dma;
    u16 fillValue;
    u16* fillPtr;
    s16* scaleY;
    s16* scaleX;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_828B210;
    dma[1] = PLTT;
    dma[2] = 0x80000011;
    dma[2];

    dma[0] = (u32)sUnk_828C570;
    dma[1] = OBJ_PLTT;
    dma[2] = 0x800000D0;
    dma[2];

    if (gUnk_3002C7C == 1) {
        dma[0] = (u32)sUnk_828C710;
        dma[1] = 0x05000240;
        dma[2] = 0x80000010;
        dma[2];
    }

    fillPtr = &fillValue;
    fillValue = 0;
    dma[0] = (u32)fillPtr;
    dma[1] = VRAM;
    dma[2] = 0x81000780;
    dma[2];

    fillPtr = &fillValue;
    fillValue = 0;
    dma[0] = (u32)fillPtr;
    dma[1] = 0x06008700;
    dma[2] = 0x81000780;
    dma[2];

    gCutscenePrimaryObjectX = 88;
    gCutscenePrimaryObjectY = 120;
    scaleY = &gUnk_3002C52;
    scaleX = &gUnk_3002C50;
    *scaleX = 0x200;
    *scaleY = 0x200;

    if (gUnk_3002C7C == 0) {
        m4aMPlayVolumeControl(&gMPlayInfo_2, 0xFFFF, 210);
    }
    m4aSongNumStartOrChange(440);
    REG_DISPCNT = 4;
}

void UpdateLayeredSpriteCutsceneSequence(s32 inputFrame)
{
    s32 frame;
    const u16* oamA;
    const u16* oamB;
    u16* dst;
    s32 timer;
    s32 done;

    frame = inputFrame;
    oamA = 0;
    oamB = 0;
    dst = gOamBuffer;

    switch (gUnk_3002C60) {
        case 0:
            LZ77UnCompVram(sUnk_828B234, (void*)0x06000F00);
            LZ77UnCompVram(sUnk_828C730, (void*)0x06014000);
            gUnk_3002C64 = 0;
            gUnk_3002C60++;
            break;
        case 1:
            WaitForVBlankInterrupt();
            gCutsceneAnimationFinished = 280;
            gUnk_3002C9A = 144;
            {
                vu16* offsetPointer;
                register s32 negativeOffset PIN("r3");
                register s32 offsetValue PIN("r0");
                offsetPointer = &gUnk_3002C44;
                negativeOffset = 8;
                negativeOffset = -negativeOffset;
                ASM_BARRIER(: "+r"(negativeOffset));
                offsetValue = negativeOffset;
                *offsetPointer = offsetValue;
            }
            InitCutsceneDarkenBlendFromBlack(52);
            {
                vu16* displayPointer;
                register s32 displayValue PIN("r4");
                register s32 storedDisplayValue PIN("r0");
                displayPointer = (volatile u16*)0x04000000;
                displayValue = 0x1404;
                ASM_BARRIER(: "+r"(displayPointer), "+r"(displayValue));
                storedDisplayValue = displayValue;
                *displayPointer = storedDisplayValue;
            }
            gUnk_3002C60++;
            gUnk_3002C64 = 0;
            break;
        case 2:
            DecreaseCutsceneBlendCoefficient(15);
            gUnk_3002C64++;
            SelectCutsceneAnimationLoop72(frame, &oamA);
            timer = gUnk_3002C64;
            switch (timer - 150) {
                case 0:
                case 10:
                case 20:
                case 30:
                case 40:
                case 50:
                case 60:
                case 70:
                    gUnk_3002C44++;
                    break;
            }
            if (gUnk_3002C64 & 1) {
                gCutsceneAnimationFinished--;
                if ((s16)gCutsceneAnimationFinished == 119) {
                    gUnk_3002C60++;
                    gUnk_3002C64 = 0;
                    m4aSongNumStartOrChange(0x1B9);
                    gUnk_3002C68 = 0;
                    *(volatile u16*)0x04000054 = 3;
                    *(volatile u16*)0x04000050 = 180;
                }
            }
            break;
        case 3: {
            s32* counterPointer;
            s32 counterValue;
            s32 callTimer;
            counterPointer = &gUnk_3002C64;
            counterValue = *counterPointer;
            callTimer = counterValue;
            counterValue++;
            *counterPointer = counterValue;
            done = SelectCutsceneAnimationLoop40(callTimer, &oamA);
            if (done) {
                gUnk_3002C60++;
                *counterPointer = 0;
            }
        } break;
        case 4: {
            s32* counterPointer;
            s32 counterValue;
            s32 callTimer;
            counterPointer = &gUnk_3002C64;
            counterValue = *counterPointer;
            callTimer = counterValue;
            counterValue++;
            *counterPointer = counterValue;
            done = SelectCutsceneAnimationLoop24(callTimer, &oamA);
            if (done) {
                gUnk_3002C60++;
                *counterPointer = 0;
            }
        } break;
        case 5:
            SelectCutsceneAnimationLoop8(frame, &oamA);
            if ((gUnk_3002C64 & 3) == 3 && (u16)gUnk_3002C56 > 4) {
                gUnk_3002C56--;
            }
            if ((u16)gUnk_3002C50 > 16) {
                gUnk_3002C50 -= 16;
                gUnk_3002C52 -= 16;
            } else {
                gUnk_3002C58 = 3;
                if (gCutscenePrimaryObjectY <= 127) {
                    if ((gUnk_3002C64 & 7) == 7) {
                        gCutscenePrimaryObjectY++;
                    }
                } else {
                    gUnk_3002C60++;
                    gUnk_3002C64 = -1;
                    gUnk_3002C9C = gCutsceneAnimationFinished - 60;
                    gUnk_3002C9E = gUnk_3002C9A + 60;
                }
            }
            gUnk_3002C64++;
            break;
        case 6:
            SelectCutsceneAnimationLoop8(frame, &oamA);
            {
                s32* counterPointer;
                s32 counterValue;
                s32 callTimer;
                counterPointer = &gUnk_3002C64;
                counterValue = *counterPointer;
                callTimer = counterValue;
                counterValue++;
                *counterPointer = counterValue;
                SelectCutsceneAnimationLoop60(callTimer, &oamB);
                if (gUnk_3002C9C < gCutsceneAnimationFinished) {
                    gUnk_3002C9C++;
                }
                if (gUnk_3002C9E > gUnk_3002C9A) {
                    gUnk_3002C9E--;
                }
                if (gUnk_3002C9C == gCutsceneAnimationFinished && gUnk_3002C9E == gUnk_3002C9A) {
                    gUnk_3002C60++;
                    *counterPointer = 0;
                    m4aSongNumStartOrChange(442);
                }
            }
            break;
        case 7: {
            s32* counterPointer;
            s32 counterValue;
            s32 callTimer;
            counterPointer = &gUnk_3002C64;
            counterValue = *counterPointer;
            callTimer = counterValue;
            counterValue++;
            *counterPointer = counterValue;
            if (SelectCutsceneAnimationLoop63(callTimer, &oamA)) {
                gUnk_3002C60++;
                *counterPointer = 0;
                gUnk_3002C9E -= 2;
            }
        } break;
        case 8: {
            s32* counterPointer;
            s32 counterValue;
            counterPointer = &gUnk_3002C64;
            counterValue = *counterPointer;
            if ((u32)counterValue > 54) {
                counterValue = 54;
            }
            SelectCutsceneAnimationLoop55(counterValue, &oamB);
            if ((*counterPointer & 3) == 3) {
                gUnk_3002C9E++;
            }
            {
                s32 oldCounter;
                s32 callTimer;
                oldCounter = *counterPointer;
                callTimer = oldCounter;
                oldCounter++;
                *counterPointer = oldCounter;
                done = SelectCutsceneAnimationLoop59(callTimer, &oamA);
            }
            if (done) {
                gUnk_3002C60++;
                *counterPointer = 0;
            }
        } break;
        case 9:
            SelectCutsceneAnimationLoop55(54, &oamB);
            {
                s32* counterPointer;
                s32 counterValue;
                s32 callTimer;
                counterPointer = &gUnk_3002C64;
                counterValue = *counterPointer;
                callTimer = counterValue;
                counterValue++;
                *counterPointer = counterValue;
                done = SelectCutsceneAnimationLoop52(callTimer, &oamA);
                if (done) {
                    gUnk_3002C60++;
                    *counterPointer = 0;
                }
            }
            break;
        case 10:
            SelectCutsceneAnimationLoop55(54, &oamB);
            {
                s32* counterPointer;
                s32 counterValue;
                s32 callTimer;
                counterPointer = &gUnk_3002C64;
                counterValue = *counterPointer;
                callTimer = counterValue;
                counterValue++;
                *counterPointer = counterValue;
                if (SelectCutsceneAnimationLoop90(callTimer, &oamA)) {
                    gSubGameMode++;
                }
            }
            break;
    }

    if (gUnk_3002C60 > 4 && (frame & 15) == 15 && (u16)gUnk_3002C2E <= 0x11F) {
        gUnk_3002C2E++;
        gUnk_3002C30++;
    }
    if (gUnk_3002C60 > 2) {
        s32* loadedTimerPointer;
        s32* blendTimerPointer;
        loadedTimerPointer = &gUnk_3002C68;
        timer = *loadedTimerPointer;
        blendTimerPointer = loadedTimerPointer;
        if (timer == 15) {
            *(volatile u16*)0x04000054 = 4;
        } else if (timer == 30) {
            *(volatile u16*)0x04000054 = 5;
        } else if (timer == 45) {
            *(volatile u16*)0x04000054 = 6;
        } else if (timer == 61) {
            *(volatile u16*)0x04000054 = 7;
        } else if (timer == 77) {
            *(volatile u16*)0x04000054 = 8;
        } else if (timer == 78) {
            *(volatile u16*)0x04000054 = 16;
        } else if (timer == 81) {
            *(volatile u16*)0x04000054 = 15;
        } else if (timer == 84) {
            *(volatile u16*)0x04000054 = 14;
        } else if (timer == 87) {
            *(volatile u16*)0x04000054 = 13;
        } else if (timer == 90) {
            *(volatile u16*)0x04000054 = 12;
        } else {
            timer = *loadedTimerPointer;
            if (timer == 93) {
                *(volatile u16*)0x04000054 = 11;
            } else if (timer == 96) {
                *(volatile u16*)0x04000054 = 10;
            } else if (timer == 99) {
                *(volatile u16*)0x04000054 = 9;
            } else if (timer == 102) {
                *(volatile u16*)0x04000054 = 8;
            } else if (timer == 105) {
                *(volatile u16*)0x04000054 = 7;
            } else if (timer == 107) {
                *(volatile u16*)0x04000054 = 6;
            } else if (timer == 109) {
                *(volatile u16*)0x04000054 = 5;
            } else if (timer == 111) {
                *(volatile u16*)0x04000054 = 4;
            } else if (timer == 113) {
                *(volatile u16*)0x04000054 = 3;
            } else if (timer == 115) {
                *(volatile u16*)0x04000054 = 2;
            } else if (timer == 117) {
                *(volatile u16*)0x04000054 = 1;
            } else if (timer == 119) {
                *(volatile u16*)0x04000054 = 0;
            }
        }
        (*blendTimerPointer)++;
    }

    if (gUnk_3002C60 > 1) {
        WriteCutsceneOamAffineMatrix(0, (u16)gUnk_3002C4E, (u16)gUnk_3002C50, (u16)gUnk_3002C52);
        if (gUnk_3002C60 == 5 && (u16)gUnk_3002C50 > 128) {
            dst = AppendCutsceneScaledSpritePair(dst);
        }
        if (gUnk_3002C60 > 5 && oamB) {
            dst = AppendCutsceneOamTemplate(oamB, dst, gUnk_3002C9C, gUnk_3002C9E);
        }
        if (oamA) {
            dst = AppendCutsceneOamTemplate(oamA, dst, gCutsceneAnimationFinished, gUnk_3002C9A - gUnk_3002C44);
        }
        if (gUnk_3002C60 == 5 && (u16)gUnk_3002C50 <= 127) {
            dst = AppendCutsceneScaledSpritePair(dst);
        }
        dst = AppendCutsceneOamTemplate(sUnk_828E278, dst, 120, gUnk_3002C44 + 96);
        FinalizeCutsceneOamBuffer(dst);
    }
}

u16* AppendCutsceneScaledSpritePair(u16* destination)
{
    u16 attributes;
    s32 x;
    s32 y;
    s32 width;
    s32 leftOffset;
    u16* next;
    u16* attribute;

    attributes = ((u16)gUnk_3002C56 << 12) | ((u16)gUnk_3002C58 << 10);
    x = gCutscenePrimaryObjectX;
    y = gCutscenePrimaryObjectY;
    width = (s16)FixedMul(64, gUnk_3002C50);
    leftOffset = 64;
    leftOffset -= width;
    leftOffset += (u32)leftOffset >> 31;
    leftOffset >>= 1;
    next = AppendCutsceneOamTemplate(sUnk_828E268, destination, x += leftOffset, y);
    destination[2] = (destination[2] & 0x3FF) | attributes;

    attribute = next + 2;
    next = AppendCutsceneOamTemplate(sUnk_828E270, next, x += width, y);
    *attribute = (*attribute & 0x3FF) | attributes;

    return next;
}

void InitializeThreeLayerCutsceneBackground(void)
{
    vu32* dma;
    u32 fillValue;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_828E780;
    dma[1] = PLTT;
    dma[2] = 0x80000080;
    dma[2];

    fillValue = 0;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06007FE0;
    dma[2] = 0x85000008;
    dma[2];

    fillValue = 0x03FF03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06008000;
    dma[2] = 0x85000800;
    dma[2];

    DecodeCutsceneTileRuns(sUnk_82940C8, (u16*)0x06008800);
    DecodeCutsceneTileRuns(sUnk_8293FBE, (u16*)0x06009800);
    DecodeCutsceneTileRuns(sUnk_8293FAC, (u16*)0x06008000);

    REG_BG1CNT = 0x1000;
    REG_BG1CNT = 0x9101;
    REG_BG2CNT = 0x1302;
    InitCutsceneDarkenBlendFromBlack(39);
    REG_BG1VOFS = 0xFF70;
    REG_BG2VOFS = 16;
    REG_DISPCNT = 0;
}

void func_8005500(s32 frame)
{
    u32 timer;
    s16 verticalOffset;

    switch (gUnk_3002C60) {
        case 0:
            LZ77UnCompVram(sUnk_828E880, (void*)0x06000000);
            WaitForVBlankInterrupt();
            *(vu16*)0x04000000 = 0x0700;
            gUnk_3002C60++;
            break;

        case 1:
            if (DecreaseCutsceneBlendCoefficient(3) != 0) {
                *(vu16*)0x04000050 = 0x0442;
                *(vu16*)0x04000052 = 0x0808;
                gUnk_3002C60++;
                gUnk_3002C64 = 0;
                gCutsceneBackgroundVerticalOffset = 16;
            }
            break;

        case 2:
            if ((frame & 7) == 7) {
                s16* verticalPointer;
                u32 unsignedVertical;
                register s32 workValue PIN("r0");
                u32 zero;

                verticalPointer = &gCutsceneBackgroundVerticalOffset;
                unsignedVertical = *(u16*)verticalPointer;
                zero = 0;
                workValue = *(s16*)((u8*)verticalPointer + zero);
                if (workValue > 0) {
                    vu16* bg3Horizontal;

                    bg3Horizontal = (vu16*)0x0400001A;
                    workValue = unsignedVertical - 1;
                    *verticalPointer = workValue;
                    *bg3Horizontal = workValue;
                } else {
                    register s32 negativeValue PIN("r1");

                    negativeValue = -144;
                    ASM_BARRIER(: : "r"(negativeValue));
                    workValue = negativeValue;
                    *verticalPointer = workValue;
                    gUnk_3002C60++;
                }
            }
            break;

        case 3: {
            u32 work;
            s32 signedVertical;
            register u32 offsetOrPointer PIN("r2");
            register u32 unsignedVertical PIN("r4");

            work = (u32)&gCutsceneBackgroundVerticalOffset;
            unsignedVertical = *(u16*)work;
            offsetOrPointer = 0;
            signedVertical = *(s16*)(work + offsetOrPointer);
            offsetOrPointer = work;
            if (signedVertical < 0) {
                if (signedVertical < -40) {
                    work = unsignedVertical + 1;
                    ASM_BARRIER(: : "r"(work));
                    goto storeVertical3;
                }
                if (signedVertical < -20) {
                    if ((frame & 1) != 0) {
                        work = unsignedVertical + 1;
                        ASM_BARRIER(: : "r"(work));
                        goto storeVertical3;
                    }
                }

                unsignedVertical = *(u16*)offsetOrPointer;
                work = 0;
                signedVertical = *(s16*)(offsetOrPointer + work);
                if (signedVertical < -10) {
                    if ((frame & 3) == 3) {
                        work = unsignedVertical + 1;
                        goto storeVertical3;
                    }
                }
                if ((frame & 7) == 7) {
                    ASM_BARRIER(: : : "memory");
                    work = *(u16*)offsetOrPointer;
                    work++;
                storeVertical3:
                    *(u16*)offsetOrPointer = work;
                }
                *(vu16*)0x04000016 = *(u16*)offsetOrPointer;
            } else {
                if (gUnk_3002C78 != 0) {
                    GP_SET(gUnk_3002C88, (u16*)0x06008386);
                    GP_SET(gUnk_3002C90, sUnk_82941D2);
                    GP_SET(gUnk_3002C94, sUnk_829420E);
                    gCutsceneAnimationFinished = 30;
                    gUnk_3002C9A = 24;
                } else {
                    GP_SET(gUnk_3002C88, (u16*)0x06008382);
                    GP_SET(gUnk_3002C90, sUnk_829424A);
                    GP_SET(gUnk_3002C94, sUnk_82942A2);
                    gCutsceneAnimationFinished = 44;
                    gUnk_3002C9A = 28;
                }
                GP_SET(gUnk_3002C8C, (u16*)((u8*)GP(gUnk_3002C88) + 64));
                gUnk_3002C60++;
                gUnk_3002C64 = 0;
            }
            break;
        }

        case 4:
            if ((frame & 3) == 3) {
                register u32* timerPointer PIN("r4");

                timerPointer = &gUnk_3002C64;
                if (*timerPointer < (u32)gCutsceneAnimationFinished) {
                    GBA_TYPEOF(u16**, &gUnk_3002C88) destination1Pointer;
                    register GBA_TYPEOF(u16**, &gUnk_3002C8C) destination2Pointer PIN("r5");

                    destination1Pointer = &gUnk_3002C88;
                    {
                        u16* destination;
                        GBA_TYPEOF(const u16**, &gUnk_3002C90) sourcePointer;
                        const u16* source;
                        u16 value;

                        destination = GP(*destination1Pointer);
                        sourcePointer = &gUnk_3002C90;
                        source = GP(*sourcePointer);
                        value = *source;
                        *destination = value;
                        source++;
                        GP_SET(*sourcePointer, source);
                        destination++;
                        GP_SET(*destination1Pointer, destination);
                    }
                    destination2Pointer = &gUnk_3002C8C;
                    {
                        u16* destination;
                        GBA_TYPEOF(const u16**, &gUnk_3002C94) sourcePointer;
                        const u16* source;
                        u16 value;

                        destination = GP(*destination2Pointer);
                        sourcePointer = &gUnk_3002C94;
                        source = GP(*sourcePointer);
                        value = *source;
                        *destination = value;
                        source++;
                        GP_SET(*sourcePointer, source);
                        destination++;
                        GP_SET(*destination2Pointer, destination);
                    }
                    if ((++*timerPointer % (u32)gUnk_3002C9A) == 0) {
                        s32 divisor;
                        u32 adjustment;
                        u16* destination;

                        divisor = gUnk_3002C9A;
                        adjustment = 64 - divisor;
                        adjustment <<= 1;
                        destination = GP(*destination1Pointer);
                        destination = (u16*)((u8*)destination + adjustment);
                        GP_SET(*destination1Pointer, destination);
                        destination = (u16*)((u8*)destination + 64);
                        GP_SET(*destination2Pointer, destination);
                    }
                } else {
                    gUnk_3002C60++;
                    *timerPointer = 0;
                }
            }
            break;

        case 5:
            gUnk_3002C64++;
            if (gUnk_3002C64 == 81) {
                m4aMPlayVolumeControl(&gMPlayInfo_2, 0xFFFF, 0x100);
                gSubGameMode++;
            }
            break;
    }
}

void InitializeThreeLayerCutsceneBackgroundWithSprites(void)
{
    vu32* dma;
    u32 fillValue;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_82D40F4;
    dma[1] = PLTT;
    dma[2] = 0x80000070;
    dma[2];

    dma[0] = (u32)sUnk_82D830E;
    dma[1] = OBJ_PLTT;
    dma[2] = 0x80000040;
    dma[2];

    fillValue = 0;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06007FE0;
    dma[2] = 0x85000008;
    dma[2];

    fillValue = 0x03FF03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06008000;
    dma[2] = 0x85000800;
    dma[2];

    DecodeCutsceneTileRuns(sUnk_82940C8, (u16*)0x06008800);
    DecodeCutsceneTileRuns(sUnk_82D820C, (u16*)0x06009800);
    DecodeCutsceneTileRuns(sUnk_8293FAC, (u16*)0x06008000);

    REG_BG1CNT = 0x1000;
    REG_BG1CNT = 0x9101;
    REG_BG2CNT = 0x1302;
    InitCutsceneDarkenBlendFromBlack(39);
    REG_BG1VOFS = 0xFF70;
    REG_BG2VOFS = 16;
    REG_DISPCNT = 0;
}

void func_80058AC(s32 frame)
{
    s32 frameReg;
    u8* mainOam;
    u8* overlayOam;
    u16* destination;
    s32 done;

    frameReg = frame;
    mainOam = 0;
    overlayOam = 0;

    switch (gUnk_3002C60) {
        case 0:
            LZ77UnCompVram(sUnk_82D41D4, (void*)0x06000000);
            LZ77UnCompVram(sUnk_82D8390, (void*)0x06010000);
            WaitForVBlankInterrupt();
            *(vu16*)0x04000000 = 0x1700;
            gUnk_3002C60++;
            break;

        case 1:
            if (DecreaseCutsceneBlendCoefficient(3) != 0) {
                *(vu16*)0x04000050 = 0x0442;
                *(vu16*)0x04000052 = 0x0808;
                gUnk_3002C60++;
                gUnk_3002C64 = 0;
                gCutsceneBackgroundVerticalOffset = 16;
            }
            break;

        case 2:
            if ((frameReg & 7) == 7) {
                s16* verticalPointer;
                u32 unsignedVertical;
                register s32 workValue PIN("r0");
                u32 zero;

                verticalPointer = &gCutsceneBackgroundVerticalOffset;
                unsignedVertical = *(u16*)verticalPointer;
                zero = 0;
                workValue = *(s16*)((u8*)verticalPointer + zero);
                if (workValue > 0) {
                    vu16* bg3Vertical;

                    bg3Vertical = (vu16*)0x0400001A;
                    workValue = unsignedVertical - 1;
                    *verticalPointer = workValue;
                    *bg3Vertical = workValue;
                } else {
                    register s32 negativeValue PIN("r1");

                    negativeValue = -144;
                    ASM_BARRIER(: : "r"(negativeValue));
                    workValue = negativeValue;
                    *verticalPointer = workValue;
                    gUnk_3002C60++;
                }
            }
            break;

        case 3: {
            u32 work;
            s32 signedVertical;
            register u32 offsetOrPointer PIN("r2");
            register u32 unsignedVertical PIN("r4");

            work = (u32)&gCutsceneBackgroundVerticalOffset;
            unsignedVertical = *(u16*)work;
            offsetOrPointer = 0;
            signedVertical = *(s16*)(work + offsetOrPointer);
            offsetOrPointer = work;
            if (signedVertical < 0) {
                if (signedVertical < -40) {
                    work = unsignedVertical + 1;
                    ASM_BARRIER(: : "r"(work));
                    goto storeVertical3;
                }
                if (signedVertical < -20) {
                    if ((frameReg & 1) != 0) {
                        work = unsignedVertical + 1;
                        ASM_BARRIER(: : "r"(work));
                        goto storeVertical3;
                    }
                }

                unsignedVertical = *(u16*)offsetOrPointer;
                work = 0;
                signedVertical = *(s16*)(offsetOrPointer + work);
                if (signedVertical < -10) {
                    if ((frameReg & 3) == 3) {
                        work = unsignedVertical + 1;
                        goto storeVertical3;
                    }
                }
                if ((frameReg & 7) == 7) {
                    ASM_BARRIER(: : : "memory");
                    work = *(u16*)offsetOrPointer;
                    work++;
                storeVertical3:
                    *(u16*)offsetOrPointer = work;
                }
                *(vu16*)0x04000016 = *(u16*)offsetOrPointer;
            } else {
                if (gUnk_3002C78 != 0) {
                    GP_SET(gUnk_3002C88, (u16*)0x06008390);
                    GP_SET(gUnk_3002C90, sUnk_82D825E);
                    GP_SET(gUnk_3002C94, sUnk_82D8278);
                    gCutsceneAnimationFinished = 13;
                    gUnk_3002C9A = 13;
                } else {
                    GP_SET(gUnk_3002C88, (u16*)0x06008388);
                    GP_SET(gUnk_3002C90, sUnk_82D8292);
                    GP_SET(gUnk_3002C94, sUnk_82D82D0);
                    gCutsceneAnimationFinished = 31;
                    gUnk_3002C9A = 22;
                }
                GP_SET(gUnk_3002C8C, (u16*)((u8*)GP(gUnk_3002C88) + 64));
                gUnk_3002C60++;
                gUnk_3002C64 = 0;
            }
            break;
        }

        case 4:
            if ((frameReg & 3) == 3) {
                register u32* timerPointer PIN("r4");

                timerPointer = &gUnk_3002C64;
                if (*timerPointer < (u32)gCutsceneAnimationFinished) {
                    GBA_TYPEOF(u16**, &gUnk_3002C88) destination1Pointer;
                    register GBA_TYPEOF(u16**, &gUnk_3002C8C) destination2Pointer PIN("r5");

                    destination1Pointer = &gUnk_3002C88;
                    {
                        u16* dst;
                        GBA_TYPEOF(const u16**, &gUnk_3002C90) sourcePointer;
                        const u16* source;
                        u16 value;

                        dst = GP(*destination1Pointer);
                        sourcePointer = &gUnk_3002C90;
                        source = GP(*sourcePointer);
                        value = *source;
                        *dst = value;
                        source++;
                        GP_SET(*sourcePointer, source);
                        dst++;
                        GP_SET(*destination1Pointer, dst);
                    }
                    destination2Pointer = &gUnk_3002C8C;
                    {
                        u16* dst;
                        GBA_TYPEOF(const u16**, &gUnk_3002C94) sourcePointer;
                        const u16* source;
                        u16 value;

                        dst = GP(*destination2Pointer);
                        sourcePointer = &gUnk_3002C94;
                        source = GP(*sourcePointer);
                        value = *source;
                        *dst = value;
                        source++;
                        GP_SET(*sourcePointer, source);
                        dst++;
                        GP_SET(*destination2Pointer, dst);
                    }
                    if ((++*timerPointer % (u32)gUnk_3002C9A) == 0) {
                        s32 divisor;
                        u32 adjustment;
                        u16* dst;

                        divisor = gUnk_3002C9A;
                        adjustment = 64 - divisor;
                        adjustment <<= 1;
                        dst = GP(*destination1Pointer);
                        dst = (u16*)((u8*)dst + adjustment);
                        GP_SET(*destination1Pointer, dst);
                        dst = (u16*)((u8*)dst + 64);
                        GP_SET(*destination2Pointer, dst);
                    }
                } else {
                    gUnk_3002C60++;
                    *timerPointer = 0;
                }
            }
            break;

        case 5:
            gUnk_3002C64++;
            if (gUnk_3002C64 == 181) {
                m4aMPlayVolumeControl(&gMPlayInfo_2, 0xFFFF, 0x100);
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 6:
            gUnk_3002C64++;
            if (gUnk_3002C64 == 181) {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 7:
            if (gTitleScreenStyle == 1) {
                SelectTitleScreenSHardLogoIntroOamFrame(gUnk_3002C64, &mainOam);
            } else {
                SelectTitleScreenNormalLogoIntroOamFrame(gUnk_3002C64, &mainOam);
            }
            gUnk_3002C64++;
            if ((gButtonsPressed & 9) != 0) {
                m4aSongNumStartOrChange(0x125);
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 8:
            if (gTitleScreenStyle == 1) {
                done = SelectTitleScreenSHardLogoIdleOamFrame(gUnk_3002C64, &mainOam);
            } else {
                done = SelectTitleScreenNormalLogoIdleOamFrame(gUnk_3002C64, &mainOam);
            }
            gUnk_3002C64++;
            if (done != 0) {
                m4aMPlayVolumeControl(&gMPlayInfo_2, 0xFFFF, 180);
                gSubGameMode++;
            }
            break;
    }

    destination = gOamBuffer;
    if (gUnk_3002C60 > 5) {
        SelectTitleScreenOverlayOamFrame(gUnk_3002C64, &overlayOam);
        destination = AppendCutsceneOamTemplate((u16*)overlayOam, destination, 120, 88);
    }
    destination = AppendCutsceneOamTemplate((u16*)mainOam, destination, 108, 160);
    FinalizeCutsceneOamBuffer(destination);
}

void ApplyCutsceneBg0Scroll(void)
{
    vu16* verticalOffset;

    verticalOffset = (vu16*)0x04000012;
    *verticalOffset = 95;
    verticalOffset--;
    *verticalOffset = -(gUnk_300188E << 3);
}

void InitializeSubGameTransitionGraphics(void)
{
    vu32* dma;
    u32 fillValue;

    if (gUnk_3002C74 != 0) {
        SubGameClearGraphicsMemory();
        m4aMPlayFadeOut(&gMPlayInfo_4, 12);
        m4aSongNumStartOrChange(0x12B);
        m4aSongNumStartOrChange(0x27C);
    } else {
        u32 fillConstant;
        vu32* fillDma;
        u32 fillSource;
        u32 fillControl;

        fillConstant = 0x03FF03FF;
        fillValue = fillConstant;
        fillDma = (vu32*)REG_ADDR_DMA3;
        fillSource = (u32)&fillValue;
        fillDma[0] = fillSource;
        fillDma[1] = 0x0600A000;
        fillControl = 0x85000100;
        fillDma[2] = fillControl;
        fillDma[2];

        fillValue = fillConstant;
        {
            register u32 secondSource PIN("r4");

            secondSource = (u32)&fillValue;
            fillDma[0] = secondSource;
        }
        fillDma[1] = 0x0600A800;
        fillDma[2] = fillControl;
        fillDma[2];
    }

    {
        vu32* paletteDma;

        paletteDma = (vu32*)REG_ADDR_DMA3;
        paletteDma[0] = (u32)sUnk_8298990;
        paletteDma[1] = OBJ_PLTT;
        paletteDma[2] = 0x80000060;
        paletteDma[2];

        paletteDma[0] = (u32)sUnk_82942FC;
        paletteDma[1] = PLTT;
        paletteDma[2] = 0x800000A0;
        paletteDma[2];
    }

    if (gUnk_3002C74 != 0) {
        const u16* tileRuns;

        if (gUnk_3002C78 != 0) {
            tileRuns = sUnk_8298908;
        } else {
            tileRuns = sUnk_829892E;
        }
        DecodeCutsceneTileRuns(tileRuns, (u16*)0x06008000);
        CutsceneLoadObjPalette(15);
    } else {
        DecodeCutsceneTileRuns(sUnk_8295234, (u16*)0x06008000);
        DecodeCutsceneTileRuns(sUnk_8295534, (u16*)0x06008000);
        DecodeCutsceneTileRuns(sUnk_82953CA, (u16*)0x06008800);
        DecodeCutsceneTileRuns(sUnk_8295534, (u16*)0x06008800);
    }

    DecodeCutsceneTileRuns(sUnk_82955A6, (u16*)0x06009000);
    DecodeCutsceneTileRuns(sUnk_8295604, (u16*)0x0600A000);
    DecodeCutsceneTileRuns(sUnk_829571E, (u16*)0x0600A800);
    DecodeCutsceneTileRuns(sUnk_8295864, (u16*)0x0600B000);
    DecodeCutsceneTileRuns(sUnk_82958F2, (u16*)0x0600B800);

    {
        vu16* bgRegister;
        u32 evenValue;
        register u32 oddValue PIN("r4");
        register u16 storeValue PIN("r0");

        bgRegister = (vu16*)0x04000008;
        evenValue = 0x5000;
        *bgRegister = (storeValue = evenValue);
        bgRegister++;
        oddValue = 0x1201;
        *bgRegister = (storeValue = oddValue);
        bgRegister++;
        evenValue = 0x5402;
        *bgRegister = (storeValue = evenValue);
        bgRegister++;
        oddValue = 0x5603;
        *bgRegister = (storeValue = oddValue);
    }

    {
        s16 zero;
        s32 activeVariant;
        register s16 verticalValue PIN("r1");
        s32 negativeValue;
        s16* verticalPointer;
        vu16* scrollRegister;

        gCutsceneBackgroundHorizontalOffset = (zero = 0);
        verticalPointer = &gCutsceneBackgroundVerticalOffset;
        activeVariant = gUnk_3002C74;
        verticalValue = 80;
        if (activeVariant != 0) {
            verticalValue = (negativeValue = -2);
        }
        *verticalPointer = verticalValue;

        scrollRegister = (vu16*)0x04000010;
        *scrollRegister = zero;
        scrollRegister++;
        *scrollRegister = verticalValue;

        {
            vu16* blendRegister;
            register u32 blendControl PIN("r4");
            register u16 storeValue PIN("r0");

            blendRegister = (vu16*)0x04000050;
            blendControl = 0x0E41;
            *blendRegister = (storeValue = blendControl);
        }
        {
            s16* alphaA;
            register s16 alphaValue PIN("r0");

            alphaA = &gUnk_3002C5A;
            alphaValue = 16;
            *alphaA = alphaValue;
        }
        gUnk_3002C5C = zero;
        {
            vu16* alphaRegister;
            u16 alphaValue;

            alphaRegister = (vu16*)0x04000052;
            alphaValue = 16;
            *alphaRegister = alphaValue;
        }

        {
            s16* animationPointer;
            register u16 storeValue PIN("r0");

            if (activeVariant != 0) {
                gCutsceneAnimationFinished = 109;
                animationPointer = &gUnk_3002C9A;
                storeValue = 148;
                goto storeAnimationPosition;
            } else {
                u32 animationValue;
                register u32 positionValue PIN("r4");

                animationPointer = &gCutsceneAnimationFinished;
                animationValue = 160;
                animationValue <<= 1;
                storeValue = animationValue;
                *animationPointer = storeValue;
                animationPointer = &gUnk_3002C9A;
                positionValue = 174;
                positionValue <<= 1;
                storeValue = positionValue;
            }

        storeAnimationPosition:
            *animationPointer = storeValue;
        }
    }

    gUnk_3002C9C = 150;
    {
        s16* offsetPointer;
        s16 offsetValue;

        offsetPointer = &gLayeredCutsceneOamOffsetY;
        offsetValue = 0;
        if (gTitleScreenStyle == 1) {
            offsetValue = 1;
        }
        *offsetPointer = offsetValue;
    }
    REG_DISPCNT = 0;
}

void UpdateLayeredSubGameCutscene(s32 frame)
{
    u16* destination;
    const u16* oamData;
    u32 fillValue;
    vu16* bg0Vofs;
    vu32* dma;

    switch (gUnk_3002C60) {
        case 0:
            LZ77UnCompVram(sUnk_829443C, (void*)0x06000000);
            LZ77UnCompVram(sUnk_8295954, (void*)0x06004000);
            gUnk_3002C60++;
            break;

        case 1:
            LZ77UnCompVram(sUnk_8298A50, (void*)0x06010000);
            if (gUnk_3002C74 != 0) {
                gUnk_3002C60 = 6;
            } else {
                InterruptCallbackSetVCount(ApplyCutsceneBg0Scroll);
                *((vu16*)0x04000004) &= 0x00FF;
                *((vu16*)0x04000004) |= 0x9720;
                *((vu16*)0x04000200) |= 4;
                gUnk_3002C60++;
            }
            WaitForVBlankInterrupt();
            *((vu16*)0x04000000) = 0x1F00;
            break;

        case 2:
            if (gUnk_3002C9A > 148) {
                gUnk_3002C9A--;
            }
            *((vu16*)0x04000010) = -(frame << 1);
            if (gCutsceneBackgroundVerticalOffset > 0) {
                if (gCutsceneBackgroundVerticalOffset > 10 && (frame & 3) == 3) {
                    gCutsceneBackgroundVerticalOffset--;
                } else if (gCutsceneBackgroundVerticalOffset > 4 && (frame & 7) == 7) {
                    gCutsceneBackgroundVerticalOffset--;
                } else if ((frame & 15) == 15) {
                    gCutsceneBackgroundVerticalOffset--;
                }
            }
            bg0Vofs = (vu16*)0x04000012;
            *bg0Vofs = gCutsceneBackgroundVerticalOffset;
            if (frame > 400 && (frame & 15) == 15) {
                u16* alphaA = (u16*)&gUnk_3002C5A;
                u16* alphaB;
                if (*alphaA != 0) {
                    (*alphaA)--;
                }
                alphaB = (u16*)&gUnk_3002C5C;
                if (*alphaB <= 15) {
                    (*alphaB)++;
                }
                *((vu16*)0x04000052) = (*alphaB << 8) | *alphaA;
                CutsceneLoadObjPalette(*alphaB);
                if (*alphaB == 16) {
                    *((vu16*)0x04000200) &= 0xFFFB;
                    *bg0Vofs = 0;
                    gUnk_3002C60++;
                }
            }
            break;

        case 3:
            if ((u32)gUnk_3002C64 <= 7) {
                u32 dst = 0x06008000 + (gUnk_3002C64++ << 9);
                fillValue = 0x03FF03FF;
                dma = (vu32*)0x040000D4;
                dma[0] = (u32)&fillValue;
                dma[1] = dst;
                dma[2] = 0x85000080;
                dma[2];
            } else {
                if (gUnk_3002C78 != 0) {
                    oamData = sUnk_8298908;
                } else {
                    oamData = sUnk_829892E;
                }
                DecodeCutsceneTileRuns(oamData, (u16*)0x06008000);
                {
                    vu16* scroll;
                    register s16 negTwo PIN("r1");

                    gCutsceneBackgroundHorizontalOffset = 0;
                    {
                        register s16* vertical PIN("r0");
                        vertical = &gCutsceneBackgroundVerticalOffset;
                        *vertical = -2;
                    }
                    negTwo = -2;
                    scroll = (vu16*)0x04000010;
                    *scroll = 0;
                    scroll++;
                    *scroll = negTwo;
                }
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 4:
            if ((u32)(gUnk_3002C64++) > 300) {
                s32 low = frame & 15;
                u16* alphaA = (u16*)&gUnk_3002C5A;
                if (low == 15) {
                    u16* alphaB;
                    if (*alphaA <= 15) {
                        (*alphaA)++;
                    }
                    alphaB = (u16*)&gUnk_3002C5C;
                    if (*alphaB != 0) {
                        (*alphaB)--;
                    }
                    *((vu16*)0x04000052) = (*alphaB << 8) | *alphaA;
                }
                if (*alphaA == 16) {
                    gUnk_3002C60++;
                }
            }
            break;

        case 5:
            gCutsceneAnimationFinished -= 8;
            if (gCutsceneAnimationFinished <= 108) {
                gCutsceneAnimationFinished = 109;
            }
            // fallthrough
        case 6:
            if (gCutsceneAnimationFinished == 109) {
                InitCutsceneDarkenBlend(63);
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 7:
            if (gButtonsPressed != 0) {
                gUnk_3002C64 = 0;
                gUnk_3002C5E = 0;
                *((vu16*)0x04000054) = 0;
            }
            gUnk_3002C64++;
            if ((u32)gUnk_3002C64 > 3600) {
                if (IncreaseCutsceneBlendCoefficient(3) != 0) {
                    StartDemoPlayback();
                }
            } else if (gButtonsPressed == 1 || gButtonsPressed == 8) {
                m4aSongNumStartOrChange(298);
                VoiceSetPlay(3);
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 8:
            if ((u32)(gUnk_3002C64++) <= 15) {
                gUnk_3002C9A++;
            } else {
                gUnk_3002C9E = gUnk_3002C9A + 40;
                gLayeredCutsceneOamOffsetX = gUnk_3002C9C - 14;
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 9:
        case 10:
            gUnk_3002C9A -= 4;
            if (gUnk_3002C9A < -100) {
                InitCutsceneDarkenBlend(63);
                gUnk_3002C60++;
            }
            break;

        case 11:
            if (IncreaseCutsceneBlendCoefficient(0) != 0) {
                gSubGameMode++;
            }
            break;
    }

    destination = gOamBuffer;
    if (gUnk_3002C60 == 9) {
        if (SelectCutsceneOamAnimation08(gUnk_3002C64++, &oamData) != 0) {
            gUnk_3002C60++;
        }
        destination = AppendCutsceneOamTemplate(oamData, destination, gUnk_3002C9E, gLayeredCutsceneOamOffsetX);
        gUnk_3002C9E++;
    }
    destination = AppendLayeredWarioCutsceneOam(destination, gUnk_3002C60, frame);
    if (gUnk_3002C60 > 4) {
        SetCutsceneOamFrame07(&oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, gCutsceneAnimationFinished, 76);
    }
    if (gUnk_3002C60 == 7) {
        if (gLayeredCutsceneOamOffsetY != 0) {
            SelectCutsceneOamAnimation09(frame, &oamData);
        } else {
            SelectCutsceneOamAnimation04(frame, &oamData);
        }
        destination = AppendCutsceneOamTemplate(oamData, destination, 74, 76);
    }
    if (gUnk_3002C60 > 7) {
        if (gLayeredCutsceneOamOffsetY != 0) {
            SelectCutsceneOamAnimation10(frame, &oamData);
        } else {
            SelectCutsceneOamAnimation05(frame, &oamData);
        }
        destination = AppendCutsceneOamTemplate(oamData, destination, 74, 76);
    }
    FinalizeCutsceneOamBuffer(destination);
    *((vu16*)0x04000014) = -(frame << 2);
    *((vu16*)0x04000018) = -(frame >> 3);
    *((vu16*)0x0400001C) = -(frame >> 4);
    if (gUnk_3002C60 <= 5 && (gButtonsPressed & 9) != 0) {
        *((vu16*)0x04000200) &= 0xFFFB;
        gSubGameMode = 46;
    }
}

u16* AppendLayeredWarioCutsceneOam(u16* destination, s32 mode, s32 timer)
{
    u16* oamData;
    s32 x;
    s32 y;
    s32 shifted;
    s32 xLimit;
    s32 sineOffset;

    oamData = 0;
    x = gUnk_3002C9A;
    y = gUnk_3002C9C;

    if (mode == 2) {
        SelectCutsceneBlinkOamAnimation01(timer, &oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, x - 84, y);
        SelectCutsceneBlinkOamAnimation02(timer, &oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, x - 38, y - 26);
        gUnk_3002C2C = 0;
    }

    if (mode > 6) {
        SetCutsceneOamFrame06(&oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, 96, 155);
    }

    if (mode > 2) {
        shifted = timer << 3;
        x = (shifted + 500) & 0x7FF;
        xLimit = 400;
        if (x > xLimit) {
            x = xLimit;
        }
        SetCutsceneOamFrame09(&oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, x - 64, 160);

        x = (shifted + 800) & 0x7FF;
        if (x > xLimit) {
            x = xLimit;
        }
        SetCutsceneOamFrame10(&oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, x - 64, 160);

        x = (shifted + 1300) & 0x7FF;
        if (x > xLimit) {
            x = xLimit;
        }
        SetCutsceneOamFrame11(&oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, x - 64, 160);

        x = (shifted + 2000) & 0x7FF;
        if (x > xLimit) {
            x = xLimit;
        }
        SetCutsceneOamFrame12(&oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, x - 64, 160);

        sineOffset = FixedMul(sSinCosTable[gUnk_3002C2C++ & 0xFF], 4);
        x = gUnk_3002C9A - (s16)sineOffset;
        SelectCutsceneOamAnimation03(timer, &oamData);
        destination = AppendCutsceneOamTemplate(oamData, destination, x + 64, y);
    }

    SelectCutsceneOamAnimation07(timer, &oamData);
    destination = AppendCutsceneOamTemplate(oamData, destination, x, y);
    SelectCutsceneOamAnimation02(timer, &oamData);
    destination = AppendCutsceneOamTemplate(oamData, destination, x, y);
    SetCutsceneOamFrame08(&oamData);
    destination = AppendCutsceneOamTemplate(oamData, destination, x - 19, y - 11);
    destination = AppendCutsceneOamTemplate(oamData, destination, x + 36, y - 11);
    SelectCutsceneOamAnimation01(timer, &oamData);
    destination = AppendCutsceneOamTemplate(oamData, destination, x, y);

    WriteCutsceneOamAffineMatrix(0, (-(timer << 4)) & 0xFF, 0x100, 0x100);
    return destination;
}

void func_8006680(void)
{
    s32 fillValue;
    s32 row;

    {
        vu32* dma;
        u8* rowDestination;
        const u8* rowSource;
        register u32 rowControl PIN("r2");

        dma = (vu32*)REG_ADDR_DMA3;
        dma[0] = (u32)sUnk_829B240;
        dma[1] = PLTT;
        dma[2] = 0x80000040;
        dma[2];

        dma[0] = (u32)sUnk_829D46E;
        dma[1] = OBJ_PLTT;
        dma[2] = 0x80000030;
        dma[2];

        fillValue = -1;
        dma[0] = (u32)&fillValue;
        dma[1] = 0x06007F80;
        dma[2] = 0x85000008;
        dma[2];

        LZ77UnCompVram((const u32*)sUnk_829B2C0, (void*)VRAM);
        LZ77UnCompVram((const u32*)sUnk_829D4D0, OBJ_VRAM0);

        rowDestination = (u8*)0x06008000;
        rowSource = sUnk_829D194;
        rowControl = 0x80000020;
        row = 17;
        do {
            dma[0] = (u32)rowSource;
            dma[1] = (u32)rowDestination;
            dma[2] = rowControl;
            dma[2];
            row--;
            rowDestination += 0x40;
        } while (row >= 0);
    }

    DecodeCutsceneTileRuns(sUnk_829D1D4, (u16*)0x06008800);
    DecodeCutsceneTileRuns(sUnk_829D212, (u16*)0x06009000);

    {
        u8* tilemap;
        register u8* copySource PIN("r3");
        vu32* copyDma;
        u32 copyControl;

        {
            const u16* runs;

            runs = sUnk_829D270;
            tilemap = (u8*)0x06009800;
            DecodeCutsceneTileRuns(runs, (u16*)tilemap);
        }

        copySource = tilemap;
        copyDma = (vu32*)REG_ADDR_DMA3;
        {
            register u32 offset PIN("r0");

            copyDma[0] = (u32)copySource;
            offset = 0xE8;
            offset <<= 3;
            copyDma[1] = (u32)copySource + offset;
            copyControl = 0x8000001E;
            copyDma[2] = copyControl;
            copyDma[2];

            copyDma[0] = (u32)copySource;
            offset = 0xF0;
            offset <<= 3;
            copyDma[1] = (u32)copySource + offset;
            copyDma[2] = copyControl;
            copyDma[2];

            copyDma[0] = (u32)copySource;
            offset = 0xF8;
            offset <<= 3;
            copyDma[1] = (u32)copySource + offset;
            copyDma[2] = copyControl;
            copyDma[2];
        }
    }

    {
        vu16* bgRegister;
        register u32 value PIN("r2");
        register u16 storeValue PIN("r0");

        bgRegister = (vu16*)0x04000008;
        value = 0x1001;
        *bgRegister = (storeValue = value);
        bgRegister++;
        value += 0xFF;
        *bgRegister = (storeValue = value);
        bgRegister++;
        value = 0x1202;
        *bgRegister = (storeValue = value);
        bgRegister++;
        value = 0x1303;
        *bgRegister = (storeValue = value);
        bgRegister = (vu16*)((u8*)bgRegister - 0xE);
        *bgRegister = 0;
    }
}

void UpdateLayeredEndingCutsceneSequence(s32 inputFrame)
{
    s32 frame;
    u16* destination;
    const u16* oam;
    frame = inputFrame;
    destination = gOamBuffer;
    switch (gUnk_3002C60) {
        case 0: {
            s16* position;
            s16* particleSeed;
            s32 i;
            s32 spacing;
            s16* positionBase;
            s32 loopValue;
            s32 loopIndex;
            s32 loopPrimaryY;
            gUnk_3002C4E = 0;
            gUnk_3002C50 = 320;
            gUnk_3002C52 = 320;
            gCutscenePrimaryObjectX = 120;
            position = &gCutscenePrimaryObjectY;
            *position = 180;
            gUnk_3002CA4 = 120;
            particleSeed = &gUnk_3002CA6;
            {
                s32 product;
                register s32 primaryYValue PIN("r5");
                product = FixedMul(16, gUnk_3002C52);
                primaryYValue = (u16)*position;
                product += primaryYValue;
                *particleSeed = product;
            }
            i = 3;
            positionBase = gCutsceneObjectYPositions;
            spacing = 18;
            position = positionBase + 3;
            do {
                loopValue = FixedMul(16, gUnk_3002C52);
                loopIndex = i << 1;
                loopPrimaryY = (u16)gCutscenePrimaryObjectY;
                loopValue += loopPrimaryY;
                loopValue += spacing;
                *position = loopValue;
                *(s16*)((u8*)gCutsceneObjectXPositions + loopIndex) = gCutscenePrimaryObjectX;
                spacing -= 6;
                position--;
                i--;
            } while (i >= 0);
            gCutsceneAnimationFinished = 0;
            {
                s32 negativeValue;
                register s16 scroll PIN("r1");
                gUnk_3002C9A = (scroll = (negativeValue = -24));
                *((vu16*)0x0400001A) = scroll;
                *((vu16*)0x0400001E) = scroll;
            }
            gUnk_3002C9C = 120;
            gUnk_3002C9E = 64;
            gLayeredCutsceneOamOffsetX = 120;
            gLayeredCutsceneOamOffsetY = 64;
            m4aMPlayVolumeControl((void*)&gMPlayInfo_2, 0xFFFF, 200);
            gUnk_3002C64 = 0;
            gUnk_3002C60++;
            break;
        }

        case 1: {
            u32 timer;
            timer = gUnk_3002C64;
            if (timer == 0) {
                gUnk_3002C5A = 17;
                gUnk_3002C5C = 0;
                *((vu16*)0x04000052) = 16;
                *((vu16*)0x04000050) = 0x0C41;
                *((vu16*)0x04000000) |= 0x1F00;
            } else if (timer == 125) {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
                break;
            }
            gUnk_3002C64++;
            break;
        }

        case 2: {
            s32 i;
            register s16* yPosition PIN("r6");
            s32 animationFrame;
            s32 offset;
            s32 result;
            if ((gUnk_3002C9C > 64) && ((frame & 3) == 3)) {
                gUnk_3002C9C--;
                gLayeredCutsceneOamOffsetX++;
                CutsceneLoadBgObjPalette(((u32)gUnk_3002C64++) >> 3);
            }
            if ((gUnk_3002C9E <= 71) && ((frame & 31) == 31)) {
                vu16* bg2Horizontal;
                u16* finished;
                u32 finishedValue;
                gUnk_3002C9E++;
                gLayeredCutsceneOamOffsetY++;
                bg2Horizontal = (vu16*)0x04000016;
                finished = &gCutsceneAnimationFinished;
                finishedValue = (*finished) - 1;
                *finished = finishedValue;
                *bg2Horizontal = finishedValue;
            }
            if ((gCutscenePrimaryObjectY > 116) && ((frame & 3) == 3)) {
                gCutscenePrimaryObjectY--;
                gUnk_3002C50--;
                gUnk_3002C52--;
            }
            i = 3;
            {
                register s16* yBase PIN("r0");
                yBase = gCutsceneObjectYPositions;
                yPosition = yBase + 3;
            }
            animationFrame = frame + 18;
            offset = 18;
            do {
                unsigned short randomOffset;
                s32 parity;
                result = SelectCutsceneBgAnimation24Frame(animationFrame, &oam);
                if (result != 0) {
                    result = FixedMul(16, gUnk_3002C52) + (u16)gCutscenePrimaryObjectY;
                    result += offset;
                    *yPosition = result;
                }
                parity = i & 1;
                randomOffset = -4;
                if (parity != 0) {
                    randomOffset = 4;
                }
                destination =
                    AppendCutsceneOamTemplate(oam, destination, gUnk_3002CA4 + (i * randomOffset), *yPosition);
                yPosition--;
                animationFrame -= 6;
                offset -= 6;
                i--;
            } while (i >= 0);
            SelectCutsceneBgAnimation36Frame(frame, &oam);
            destination = AppendCutsceneOamTemplate(oam, destination, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);
            if ((u16)gUnk_3002C50 == 256) {
                gCutscenePrimaryObjectY += 18;
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;
        }

        case 3: {
            u32 timer;
            SelectCutsceneBgAnimation28Frame(frame, &oam);
            destination = AppendCutsceneOamTemplate(oam, destination, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);
            timer = gUnk_3002C64;
            if ((((u32)(timer - 32)) <= 63) && ((timer & 7) == 7)) {
                vu16* scrollRegister;
                register u16* valuePointer PIN("r1");
                register s32 value PIN("r0");
                scrollRegister = (vu16*)0x04000016;
                valuePointer = (u16*)&gCutsceneAnimationFinished;
                value = (*valuePointer) - 1;
                *valuePointer = value;
                *scrollRegister = value;
                scrollRegister += 2;
                valuePointer = (u16*)&gUnk_3002C9A;
                value = (*valuePointer) + 1;
                *valuePointer = value;
                *scrollRegister = value;
                *((vu16*)0x0400001E) = value;
                valuePointer = (u16*)&gUnk_3002C9E;
                value = (*valuePointer) + 1;
                *valuePointer = value;
                valuePointer = (u16*)&gLayeredCutsceneOamOffsetY;
                value = (*valuePointer) + 1;
                *valuePointer = value;
                gCutscenePrimaryObjectY++;
            }
            gUnk_3002C64++;
            {
                u16* alphaAPointer;
                register u16 alphaA PIN("r3");
                alphaAPointer = &gUnk_3002C5A;
                alphaA = *alphaAPointer;
                if (alphaA != 0) {
                    if ((frame & 7) == 7) {
                        vu16* blendRegister;
                        u32 alphaB;
                        register u16 alphaResult PIN("r0");
                        blendRegister = (vu16*)0x04000052;
                        {
                            u16* alphaBPointer;
                            alphaBPointer = &gUnk_3002C5C;
                            alphaB = (*alphaBPointer) + 1;
                            *alphaBPointer = alphaB;
                        }
                        alphaB <<= 8;
                        alphaResult = alphaA - 1;
                        *alphaAPointer = alphaResult;
                        alphaResult |= alphaB;
                        *blendRegister = alphaResult;
                    }
                } else {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
            }
            break;
        }

        case 4: {
            u32* timerPointer;
            u32 timer;
            u32 oldTimer;
            register s32 result PIN("r4");
            timerPointer = &gUnk_3002C64;
            timer = *timerPointer;
            oldTimer = timer;
            timer++;
            *timerPointer = timer;
            result = SelectCutsceneBgAnimation72Frame(oldTimer, &oam);
            destination = AppendCutsceneOamTemplate(oam, destination, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);
            if ((*timerPointer) == 32) {
                VoiceSetPlay(1);
            }
            if (result != 0) {
                *timerPointer = 20;
                gUnk_3002C60++;
            }
            break;
        }

        case 5: {
            SelectCutsceneBgAnimation28Frame(gUnk_3002C64, &oam);
            destination = AppendCutsceneOamTemplate(oam, destination, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);
            gUnk_3002C64++;
            if (gUnk_3002C64 == 111) {
                gCutscenePrimaryObjectY -= 18;
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;
        }

        case 6: {
            if ((frame & 3) == 3) {
                if (gCutscenePrimaryObjectY <= 159) {
                    gCutscenePrimaryObjectY++;
                    gUnk_3002C50 -= 4;
                    gUnk_3002C52 -= 4;
                } else {
                    InitCutsceneBrightenBlend(62);
                    *((vu16*)0x04000000) = 0x1E00;
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
            }
            if (SelectCutsceneBgAnimation24Frame(frame, &oam) != 0) {
                s16* particlePointer;
                s32 particleY;
                particlePointer = &gUnk_3002CA6;
                particleY = FixedMul(16, gUnk_3002C52);
                particleY += (u16)gCutscenePrimaryObjectY;
                *particlePointer = particleY;
            }
            {
                u16* attribute;
                register u16 mask PIN("r4");
                u16 attributeValue;
                attribute = destination + 2;
                destination = AppendCutsceneOamTemplate(oam, destination, gUnk_3002CA4, gUnk_3002CA6);
                attributeValue = *attribute;
                mask = 0x0400;
                attributeValue |= mask;
                *attribute = attributeValue;
                SelectCutsceneBgAnimation36Frame(frame, &oam);
                attribute = destination + 2;
                destination =
                    AppendCutsceneOamTemplate(oam, destination, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);
                *attribute |= mask;
            }
            break;
        }

        case 7: {
            if (IncreaseCutsceneBlendCoefficient(7) != 0) {
                gSubGameMode++;
            }
            break;
        }
    }

    if (gUnk_3002C60 != 0) {
        destination = AppendCutsceneOamTemplate(sUnk_829F95E, destination, gUnk_3002C9C, gUnk_3002C9E);
        destination = AppendCutsceneOamTemplate(
            sUnk_829F9B4, destination, gLayeredCutsceneOamOffsetX, gLayeredCutsceneOamOffsetY
        );
        destination = AppendCutsceneOamTemplate(sUnk_829FA0A, destination, gUnk_3002C9C + 16, gUnk_3002C9E);
        destination = AppendCutsceneOamTemplate(
            sUnk_829FA48, destination, gLayeredCutsceneOamOffsetX - 16, gLayeredCutsceneOamOffsetY
        );
    }
    WriteCutsceneOamAffineMatrix(0, (u16)gUnk_3002C4E, (u16)gUnk_3002C50, (u16)gUnk_3002C52);
    FinalizeCutsceneOamBuffer(destination);
}

void InitializeLayeredWarioCutscene(void)
{
    vu32* dma;
    u16 fillValue;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_829FD48;
    dma[1] = PLTT;
    dma[2] = 0x80000100;
    dma[2];

    dma[0] = (u32)sUnk_82A2F12;
    dma[1] = 0x05000380;
    dma[2] = 0x80000030;
    dma[2];

    dma[0] = (u32)sUnk_82A36AC;
    dma[1] = 0x05000280;
    dma[2] = 0x80000010;
    dma[2];

    LZ77UnCompVram((const u32*)sUnk_829FF48, (void*)0x06000000);
    LZ77UnCompVram((const u32*)sUnk_82A2F74, (void*)0x06014000);
    LZ77UnCompVram((const u32*)sUnk_82A36CC, (void*)0x06011280);
    LZ77UnCompVram((const u32*)sUnk_82A3710, (void*)0x06011680);

    {
        u16* fillPointer;

        fillPointer = &fillValue;
        *fillPointer = 0x03FF;
        dma[0] = (u32)fillPointer;
    }
    dma[1] = 0x06008000;
    dma[2] = 0x81000260;
    dma[2];
    DecodeCutsceneTileRuns(sUnk_82A1CF0, (u16*)0x06008000);

    fillValue = 0x03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06009000;
    dma[2] = 0x81000260;
    dma[2];
    DecodeCutsceneTileRuns(sUnk_82A1E7A, (u16*)0x06009000);

    fillValue = 0x93A0;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x0600A380;
    dma[2] = 0x810000C0;
    dma[2];
    DecodeCutsceneTileRuns(sUnk_82A1FBC, (u16*)0x0600A000);

    CutsceneWarioLoadObjPalette();
    CutsceneWarioSetPose(0, 0, 16);

    gCutscenePrimaryObjectX = -32;
    gCutscenePrimaryObjectY = 164;
    gUnk_3002C9C = -32;
    gUnk_3002C9E = 164;
    gLayeredCutsceneOamOffsetX = 0;

    REG_BG0CNT = 0x5000;
    REG_BG1CNT = 0x5201;
    REG_BG2CNT = 0x5402;
    gCutsceneBackgroundHorizontalOffset = 0;
    gCutsceneBackgroundVerticalOffset = 16;
    InitCutsceneBrightenBlendFromWhite(55);
    WaitForVBlankInterrupt();
    REG_DISPCNT = 0x1700;
}

void UpdateLayeredWarioCutsceneSequence(s32 inputFrame)
{
    register s32 frame PIN("r6");
    s32 effectMode;
    const u16* stateOam;
    const u16* effectOam;
    u16* destination;
    s32 timer;
    s32 i;
    s32 x;
    s32 y;
    s32 delta;

    frame = inputFrame;
    stateOam = 0;
    effectMode = 0;

    switch (gUnk_3002C60) {
        case 0:
            DecreaseCutsceneBlendCoefficient(3);
            {
                s16* horizontalPointer;
                register u16 horizontalValue PIN("r2");
                register vu16* backgroundRegister PIN("r0");
                vu16* verticalRegister;
                s16* verticalPointer;
                s32 verticalValue;

                horizontalPointer = &gCutsceneBackgroundHorizontalOffset;
                horizontalValue = *horizontalPointer;
                if (*horizontalPointer <= 15 && (frame & 3) == 3) {
                    backgroundRegister = (vu16*)0x04000010;
                    horizontalValue++;
                    *horizontalPointer = horizontalValue;
                    *backgroundRegister = horizontalValue;
                    verticalRegister = (vu16*)0x04000012;
                    verticalPointer = &gCutsceneBackgroundVerticalOffset;
                    verticalValue = *(u16*)verticalPointer;
                    verticalValue--;
                    *verticalPointer = verticalValue;
                    *verticalRegister = verticalValue;
                    backgroundRegister = (vu16*)0x04000014;
                    *backgroundRegister = horizontalValue;
                    backgroundRegister++;
                    *backgroundRegister = verticalValue;
                }
            }
            if ((frame & 7) == 7) {
                *(vu16*)0x04000018 = gCutsceneBackgroundHorizontalOffset >> 1;
            }
            if (gCutscenePrimaryObjectY > 112) {
                gCutscenePrimaryObjectY--;
            }
            if (gCutscenePrimaryObjectX <= 119) {
                s32 modeValue;

                modeValue = 16;
                effectMode = modeValue;
                if (gCutscenePrimaryObjectX <= 39) {
                    gCutscenePrimaryObjectX++;
                } else {
                    gCutscenePrimaryObjectX += 2;
                }
            } else {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 1:
            if (gUnk_3002C64 == 0) {
                CutsceneWarioSetPose(0, 51, 16);
                VoiceSetPlay(3);
            }
            gUnk_3002C64++;
            if (gUnk_3002C64 == 50) {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 2:
            effectMode = 16;
            gUnk_3002C64++;
            if (gUnk_3002C64 == 1) {
                CutsceneWarioSetPose(0, 0, 16);
            } else if (gCutscenePrimaryObjectX <= 271) {
                gCutscenePrimaryObjectX += 2;
            } else {
                InitCutsceneDarkenBlend(55);
                gUnk_3002C60++;
            }
            break;

        case 3:
            if (IncreaseCutsceneBlendCoefficient(1)) {
                LZ77UnCompVram(sUnk_82A229C, (void*)0x06008000);
                gCutsceneBackgroundHorizontalOffset = 0;
                gCutsceneBackgroundVerticalOffset = 0;
                *(vu16*)0x04000010 = 0;
                *(vu16*)0x04000012 = 0;
                *(vu16*)0x04000014 = 0;
                *(vu16*)0x04000016 = 0;
                gCutscenePrimaryObjectX = -32;
                gCutscenePrimaryObjectY = 88;
                gUnk_3002C9C = -32;
                gUnk_3002C9E = 88;
                gCutsceneAnimationFinished = 424;
                gUnk_3002C68 = 0;
                gUnk_3002C60++;
                m4aMPlayVolumeControl(&gMPlayInfo_2, 0xFFFF, 200);
                m4aSongNumStartOrChange(0x27E);
                *(vu16*)0x04000000 = 0x1300;
            }
            break;

        case 4:
            DecreaseCutsceneBlendCoefficient(1);
            effectMode = 16;
            if (gCutscenePrimaryObjectX <= 55) {
                gCutscenePrimaryObjectX += 2;
            } else {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 5:
            effectMode = 16;
            {
                s32* counterPointer;
                s32 counterValue;
                register s32 halfTimer PIN("r5");

                counterPointer = &gUnk_3002C64;
                counterValue = *counterPointer;
                halfTimer = (u32)counterValue >> 1;
                gCutscenePrimaryObjectY = halfTimer + 88;
                gCutsceneBackgroundHorizontalOffset += 2;
                gCutsceneBackgroundVerticalOffset = halfTimer;
                gUnk_3002C9C -= 2;
                gUnk_3002C9E = halfTimer + 80;
                *(vu16*)0x04000010 = gCutsceneBackgroundHorizontalOffset;
                *(vu16*)0x04000012 = halfTimer;
                *(vu16*)0x04000014 = gCutsceneBackgroundHorizontalOffset;
                *(vu16*)0x04000016 = halfTimer;
                counterValue += 2;
                *counterPointer = counterValue;
                if ((u32)counterValue > 112) {
                    gUnk_3002C60++;
                }
            }
            break;

        case 6:
            effectMode = 16;
            gCutsceneBackgroundHorizontalOffset += 2;
            gUnk_3002C9C -= 2;
            *(vu16*)0x04000010 = gCutsceneBackgroundHorizontalOffset;
            *(vu16*)0x04000014 = gCutsceneBackgroundHorizontalOffset;
            if ((u16)gCutsceneBackgroundHorizontalOffset == 272) {
                CutsceneWarioSetPose(0, 2, 16);
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            CutsceneWarioSelectEffectCAnimOam(0, &stateOam);
            break;

        case 7: {
            s32* counterPointer;
            s32 counterValue;
            s32 oldCounter;

            counterPointer = &gUnk_3002C64;
            counterValue = *counterPointer;
            oldCounter = counterValue;
            counterValue++;
            *counterPointer = counterValue;
            if (CutsceneWarioSelectEffectCAnimOam(oldCounter, &stateOam)) {
                *counterPointer = 0;
                gUnk_3002C60++;
                m4aSongNumStartOrChange(0x1B7);
                VoiceSetPlay(8);
            }
        } break;

        case 8: {
            s32* counterPointer;
            s32 counterValue;
            s32 oldCounter;

            counterPointer = &gUnk_3002C64;
            counterValue = *counterPointer;
            oldCounter = counterValue;
            counterValue++;
            *counterPointer = counterValue;
            CutsceneWarioSelectEffectBAnimOam(oldCounter, &stateOam);
        }
            {
                s16* animationPointer;
                register u16 animationValue PIN("r0");

                animationPointer = &gCutsceneAnimationFinished;
                animationValue = *animationPointer;
                animationValue += 2;
                *animationPointer = animationValue;
                if (animationValue == 600) {
                    CutsceneWarioSetPose(0, 0, 16);
                    gUnk_3002C60++;
                }
            }
            break;

        case 9:
            effectMode = 16;
            if (gCutscenePrimaryObjectX <= 255) {
                gCutscenePrimaryObjectX += 2;
            } else {
                gUnk_3002C60++;
            }
            break;

        case 10:
            if (IncreaseCutsceneBlendCoefficient(1)) {
                LZ77UnCompVram(sUnk_82A2840, (void*)0x06008000);
                LZ77UnCompVram(sUnk_82A2A24, (void*)0x06009000);
                *(vu16*)0x04000010 = 0;
                *(vu16*)0x04000012 = 0;
                *(vu16*)0x04000014 = 0;
                *(vu16*)0x04000016 = 0;
                gCutscenePrimaryObjectX = -32;
                gCutscenePrimaryObjectY = 144;
                gUnk_3002C9C = -32;
                gUnk_3002C9E = 144;
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 11:
            DecreaseCutsceneBlendCoefficient(1);
            if (gCutscenePrimaryObjectX <= 47) {
                gCutscenePrimaryObjectX += 2;
                effectMode = 16;
            }
            if (gCutscenePrimaryObjectX == 46) {
                CutsceneWarioSetPose(0, 2, 16);
            }
            {
                s32* counterPointer;
                s32 counterValue;
                s32 oldCounter;

                counterPointer = &gUnk_3002C64;
                counterValue = *counterPointer;
                oldCounter = counterValue;
                counterValue++;
                *counterPointer = counterValue;
                if (CutsceneWarioSelectEffectAAnimOam(oldCounter, &stateOam)) {
                    CutsceneWarioSetPose(0, 0, 16);
                    gUnk_3002C60++;
                }
                if (*counterPointer == 217) {
                    m4aSongNumStartOrChange(438);
                }
            }
            break;

        case 12:
            effectMode = 16;
            if (gCutscenePrimaryObjectX <= 95) {
                gCutscenePrimaryObjectX += 2;
            } else {
                CutsceneWarioSetPose(0, 27, 16);
                m4aSongNumStartOrChange(0x1B5);
                gUnk_3002C60++;
            }
            break;

        case 13: {
            s16* xPointer;
            u16 xValue;
            register s32 signedX PIN("r0");
            register s32 parabolaOffset PIN("r5");

            xPointer = &gCutscenePrimaryObjectX;
            xValue = *xPointer;
            xValue++;
            *xPointer = xValue;
            signedX = *xPointer;
            ASM_BARRIER(: "+r"(signedX));
            parabolaOffset = signedX;
            parabolaOffset -= 104;
            gCutscenePrimaryObjectY = (parabolaOffset * parabolaOffset) / 8 + 136;
            if (gCutscenePrimaryObjectY > 200) {
                gUnk_3002C60++;
            }
            {
                s32 finalLoadOffset;

                finalLoadOffset = 0;
                if (*(s16*)((u8*)xPointer + finalLoadOffset) <= 103) {
                    s32 modeValue;

                    modeValue = 16;
                    effectMode = modeValue;
                }
            }
        } break;

        case 14:
            if (IncreaseCutsceneBlendCoefficient(1)) {
                gSubGameMode++;
            }
            break;
    }

    x = gCutscenePrimaryObjectX;
    y = gCutscenePrimaryObjectY;
    destination = ((u16 * (*)(s32, s32, s32, s32, s32)) CutsceneWarioDrawPoseOam)(x, y, 1, effectMode, 0);
    if (destination > GP(gUnk_3002C70)) {
        GP_SET(gUnk_3002C70, destination);
    }

    if ((u16)(gUnk_3002C60 - 6) <= 3) {
        destination = AppendCutsceneOamTemplate(
            stateOam, destination, gCutsceneAnimationFinished - gCutsceneBackgroundHorizontalOffset, 144
        );
    }
    if (gUnk_3002C60 == 11) {
        destination = AppendCutsceneOamTemplate(stateOam, destination, 152, 144);
    }

    if (gUnk_3002C60 > 10) {
        s32 loopIndex;
        s32 tableOffset;

        loopIndex = 0;
        do {
            tableOffset = loopIndex << 1;
            CutsceneWarioSelectEffectDAnimOam(frame, &effectOam);
            destination = AppendCutsceneOamTemplate(
                effectOam,
                destination,
                *(const u16*)((const u8*)sUnk_878E6E0 + tableOffset),
                *(const u16*)((const u8*)sUnk_878E6F0 + tableOffset)
            );
            frame += 6;
            loopIndex++;
        } while (loopIndex <= 7);
    } else if (gUnk_3002C60 > 3) {
        s32 loopIndex;
        s32 tableOffset;

        loopIndex = 0;
        do {
            tableOffset = loopIndex << 1;
            CutsceneWarioSelectEffectDAnimOam(frame, &effectOam);
            destination = AppendCutsceneOamTemplate(
                effectOam,
                destination,
                *(const u16*)((const u8*)sUnk_878E700 + tableOffset) - gCutsceneBackgroundHorizontalOffset,
                *(const u16*)((const u8*)sUnk_878E718 + tableOffset) - gCutsceneBackgroundVerticalOffset
            );
            frame += 6;
            loopIndex++;
        } while (loopIndex <= 11);
    } else {
        register s32 loopIndex PIN("r5");
        s32 tableOffset;

        loopIndex = 0;
        do {
            tableOffset = loopIndex << 1;
            CutsceneWarioSelectEffectDAnimOam(frame, &effectOam);
            destination = AppendCutsceneOamTemplate(
                effectOam,
                destination,
                *(const u16*)((const u8*)sUnk_878E730 + tableOffset) - gCutsceneBackgroundHorizontalOffset,
                *(const u16*)((const u8*)sUnk_878E73C + tableOffset) - gCutsceneBackgroundVerticalOffset
            );
            frame += 6;
            loopIndex++;
        } while (loopIndex <= 5);
    }

    {
        s32 modeValue;

        modeValue = effectMode;
        if (modeValue == 16) {
            register s32 oldOffset PIN("r0");
            s32 updatedOffset;
            s16* offsetPointer;

            offsetPointer = &gLayeredCutsceneOamOffsetX;
            oldOffset = (u16)*offsetPointer;
            updatedOffset = oldOffset + 1;
            *offsetPointer = updatedOffset;
            oldOffset = (s16)oldOffset;
            if (CutsceneWarioSelectEffectEAnimOam(oldOffset, &effectOam)) {
                gUnk_3002C9C = gCutscenePrimaryObjectX;
                gUnk_3002C9E = gCutscenePrimaryObjectY;
            }
            destination = AppendCutsceneOamTemplate(effectOam, destination, gUnk_3002C9C, gUnk_3002C9E);
        } else {
            gLayeredCutsceneOamOffsetX = 0;
        }
    }

    FinalizeCutsceneOamBuffer(destination);

    if ((u16)(gUnk_3002C60 - 4) <= 5) {
        s32 timelineValue;
        s32 pitchBits;
        s32 depth;
        s32 lfoSpeed;
        register s32 tempo PIN("r6");
        MusicPlayerInfo* player;
        u32 mask;

        {
            s32* timelinePointer;
            s32 oldTimeline;

            timelinePointer = &gUnk_3002C68;
            oldTimeline = *timelinePointer;
            timelineValue = oldTimeline + 1;
            *timelinePointer = timelineValue;
        }
        if (timelineValue == 61) {
            timelineValue = 160;
            pitchBits = 0xFF6A;
            depth = 5;
            lfoSpeed = 3;
            tempo = 240;
        } else if (timelineValue == 120) {
            timelineValue = 140;
            pitchBits = 0xFF38;
            depth = 10;
            lfoSpeed = 5;
            tempo = 230;
        } else if (timelineValue == 180) {
            timelineValue = 100;
            pitchBits = 0xFED4;
            depth = 20;
            lfoSpeed = 10;
            tempo = 220;
        } else if (timelineValue == 240) {
            timelineValue = 60;
            pitchBits = 0xFE0C;
            depth = 30;
            lfoSpeed = 20;
            tempo = 210;
        } else if (timelineValue == 300) {
            timelineValue = 30;
            pitchBits = 0xFD44;
            depth = 40;
            lfoSpeed = 30;
            tempo = 200;
        } else {
            if (timelineValue == 360) {
                MPlayStop(&gMPlayInfo_2);
            }
            return;
        }

        player = &gMPlayInfo_2;
        mask = 0xFFFF;
        ((void (*)(MusicPlayerInfo*, u32, s32))m4aMPlayVolumeControl)(player, mask, timelineValue);
        ((void (*)(MusicPlayerInfo*, u32, s32))m4aMPlayPitchControl)(player, mask, (s16)pitchBits);
        ((void (*)(MusicPlayerInfo*, u32, s32))m4aMPlayModDepthSet)(player, mask, depth);
        ((void (*)(MusicPlayerInfo*, u32, s32))m4aMPlayLFOSpeedSet)(player, mask, lfoSpeed);
        ((void (*)(MusicPlayerInfo*, s32))m4aMPlayTempoControl)(player, tempo);
    }
}

void InitializeAffineWarioCutscene(void)
{
    vu32* dma;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_82A37A0;
    dma[1] = PLTT;
    dma[2] = 0x80000010;
    dma[2];

    dma[0] = (u32)sUnk_82A4E1A;
    dma[1] = OBJ_PLTT;
    dma[2] = 0x80000010;
    dma[2];

    LZ77UnCompVram(sUnk_82A37C0, (void*)VRAM);
    LZ77UnCompVram(sUnk_82A4E3C, (void*)OBJ_VRAM0);
    DecodeCutsceneTileRuns(sUnk_82A4934, (u16*)0x06008000);

    REG_BG0CNT = 0x5000;
    gCutscenePrimaryObjectX = 120;
    gCutscenePrimaryObjectY = 80;
    gUnk_3002C50 = gUnk_3002C52 = 0x300;
    gUnk_3002C4E = 0;
    WriteCutsceneOamAffineMatrix(0, 0, (u16)gUnk_3002C50, (u16)gUnk_3002C50);
    InitCutsceneDarkenBlendFromBlack(49);
    WaitForVBlankInterrupt();
    REG_DISPCNT = 0x100;
}

void UpdateAffineWarioCutscene(u16 frame)
{
    u16* destination;
    u16* oamTemplate;

    (void)frame;

    switch (gUnk_3002C60) {
        case 0:
            if (DecreaseCutsceneBlendCoefficient(1) != 0) {
                REG_DISPCNT = 0x1100;
                VoiceSetPlay(6);
                gUnk_3002C60++;
            }
            break;

        case 1:
            gUnk_3002C50 -= 16;
            gUnk_3002C52 -= 16;
            if ((u16)gUnk_3002C50 <= 15) {
                gUnk_3002C60++;
            }
            break;

        case 2:
            if (IncreaseCutsceneBlendCoefficient(1) != 0) {
                gSubGameMode++;
            }
            break;
    }

    destination = (u16*)gOamBuffer;
    if (gUnk_3002C60 <= 1) {
        CutsceneWarioGetStaticOam(&oamTemplate);
        destination =
            AppendCutsceneOamTemplate(oamTemplate, destination, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);
        WriteCutsceneOamAffineMatrix(0, (u16)gUnk_3002C4E, (u16)gUnk_3002C50, (u16)gUnk_3002C52);
    }
    FinalizeCutsceneOamBuffer(destination);
}

void ConfigureLayeredCutsceneOamOffsets(CutsceneOamSelector selector)
{
    u16 treasureCount = gUnk_3002C84;

    if (selector == SelectLayeredCutsceneOamSequence01 || selector == SelectLayeredCutsceneOamSequence07 ||
        selector == SelectLayeredCutsceneOamSequence09) {
        gLayeredCutsceneOamOffsetX = sUnk_878E748[treasureCount];
        gLayeredCutsceneOamOffsetY = sUnk_878E750[treasureCount];
    } else if (selector == SelectLayeredCutsceneOamSequence03 || selector == SelectLayeredCutsceneOamSequence05) {
        gLayeredCutsceneOamOffsetX = -16;
        gLayeredCutsceneOamOffsetY = sUnk_878E758[treasureCount];
    }
}

void InitializeCutsceneObjectRandomPositions(s32 seed)
{
    u16* xPosition;
    u16* yPosition;
    s16* randomSeed;
    s32 multiplier;
    s32 addend;
    s32 product;
    s32 value;
    s32 adjusted;
    s32 count;

    xPosition = gCutsceneObjectXPositions;
    yPosition = gCutsceneObjectYPositions;
    randomSeed = &gUnk_3002CA4;
    multiplier = 109;
    addend = 1021;
    count = 7;
    do {
        seed = (multiplier * seed + addend) % 240;
        *xPosition = seed;
        xPosition++;

        product = seed * multiplier;
        value = product + addend;
        adjusted = value;
        if (value < 0) {
            adjusted = product + 1276;
        }
        seed = value - ((adjusted >> 8) << 8);
        *yPosition = seed;
        yPosition++;
        count--;
    } while (count >= 0);

    *randomSeed = seed;
}

void UpdateLayeredCutsceneObjectPositions(s32 horizontalOffset)
{
    s32 i;
    u16* x;
    u16* y;
    u16* yStart;
    s16* randomSeed;

    y = gCutsceneObjectYPositions;
    yStart = y;
    *y += 3;
    y++;
    *y += 3;
    y++;
    *y += 3;
    y++;
    *y += 4;
    y++;
    *y += 4;
    y++;
    *y += 3;
    y++;
    *y += 4;
    y++;
    *y += 4;

    x = gCutsceneObjectXPositions;
    y = yStart;
    randomSeed = &gUnk_3002CA4;
    i = 7;
    while (i >= 0) {
        if (horizontalOffset != 0) {
            *x += horizontalOffset;
        }
        *y = *(u8*)y;
        if ((*y & ~3) == 200) {
            *randomSeed = (*randomSeed * 109 + 1021) % 240;
            *x = *randomSeed;
        }
        i--;
        x++;
        y++;
    }
}

void InitializeLayeredObjectCutscene(void)
{
    vu32* dma;
    u32 fillValue;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_829FD48;
    dma[1] = PLTT;
    dma[2] = 0x80000100;
    dma[2];

    dma[0] = (u32)sUnk_82A5318;
    dma[1] = 0x05000200;
    dma[2] = 0x80000070;
    dma[2];

    LZ77UnCompVram((const u32*)sUnk_829FF48, (void*)0x06000000);
    LZ77UnCompVram((const u32*)sUnk_82A53F8, (void*)0x06010000);

    fillValue = 0x03FF03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06008000;
    dma[2] = 0x85000200;
    dma[2];

    fillValue = 0xA0C5A0C5;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06009000;
    dma[2] = 0x85000200;
    dma[2];

    DecodeCutsceneTileRuns(sUnk_82A2CD0, (u16*)0x06009000);

    REG_BG0CNT = 0x5000;
    REG_BG1CNT = 0x5201;
    REG_BG2CNT = 0x5402;
    gCutsceneBackgroundHorizontalOffset = 0;
    gCutsceneBackgroundVerticalOffset = 0;
    gCutscenePrimaryObjectX = 120;
    gCutscenePrimaryObjectY = 220;
    gCutsceneAnimationFinished = 120;
    gUnk_3002C9A = 204;
    InitializeCutsceneObjectRandomPositions(0);
    InitCutsceneDarkenBlendFromBlack(55);
    WaitForVBlankInterrupt();
    REG_DISPCNT = 0x1700;
}

void UpdateLayeredEndingTreasureCutsceneSequence(s32 inputFrame)
{
    u8* primaryOam;
    u8* offsetOam;
    u8* centerOam;
    u8* secondaryOam;
    u8* gfxOam;
    u8* objectOam;
    u16* destination;
    s32 frame;
    u16 verticalWave;
    s32 treasureCount;
    u16 fillValue;
    vu32* dma;

    frame = inputFrame;
    primaryOam = 0;
    offsetOam = 0;
    centerOam = 0;
    secondaryOam = 0;
    gfxOam = 0;
    verticalWave = sUnk_878E760[frame & 7];
    treasureCount = gEndingCutsceneCollectedTreasureCount;
    gLayeredCutsceneOamOffsetX = 0;
    gLayeredCutsceneOamOffsetY = 0;

    switch (gUnk_3002C60) {
        case 0:
            if (!DecreaseCutsceneBlendCoefficient(1)) {
                break;
            }
            SelectLayeredCutsceneOamSequence02(frame, &primaryOam);
            if (treasureCount != 0) {
                SelectLayeredCutsceneOamSequence03(frame, &offsetOam);
                ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence03);
            }
            {
                s16* animationPointer;
                s32 animationValue;
                s32 zero;
                s32 threshold;
                s32* counterPointer;
                s32 counterValue;
                s32 bitValue;
                register s32 animationTimer PIN("r3");
                s16* secondaryPointer;

                if ((frame & 1) != 0) {
                    s16* decrementPointer;
                    u16 decrementValue;

                    decrementPointer = &gCutscenePrimaryObjectY;
                    decrementValue = *decrementPointer;
                    decrementValue--;
                    *decrementPointer = decrementValue;
                }
                animationPointer = &gCutscenePrimaryObjectY;
                zero = 0;
                animationValue = *animationPointer;
                threshold = 64;
                threshold = -threshold;
                counterPointer = &gUnk_3002C64;
                if (animationValue < threshold) {
                    *counterPointer = 0;
                    gUnk_3002C60++;
                }
                counterValue = *counterPointer;
                bitValue = counterValue & 0x80;
                counterValue++;
                *counterPointer = counterValue;
                animationTimer = 8;
                if (bitValue != 0) {
                    animationTimer = 4;
                }
                SelectLayeredCutsceneOamSequence15(animationTimer, &centerOam);
                secondaryPointer = &gUnk_3002C9A;
                if (*secondaryPointer > threshold) {
                    *secondaryPointer = (u16)*animationPointer - 16;
                }
            }
            break;

        case 1:
            if (!IncreaseCutsceneBlendCoefficient(1)) {
                break;
            }
            LZ77UnCompVram(sUnk_82A2840, (void*)0x06008000);
            LZ77UnCompVram(sUnk_82A2A24, (void*)0x06009000);
            gCutsceneBackgroundHorizontalOffset = 0;
            gCutsceneBackgroundVerticalOffset = 0;
            gCutscenePrimaryObjectX = 120;
            gCutscenePrimaryObjectY = 180;
            gCutsceneAnimationFinished = 128;
            gUnk_3002C9C = 104;
            gUnk_3002C60++;
            break;

        case 2:
            if (!DecreaseCutsceneBlendCoefficient(1)) {
                break;
            }
            {
                s16* animationPointer;
                u16 animationValue;
                s32 signedValue;
                register s32 deltaValue PIN("r3");
                register s32 animationTimer PIN("r3");

                animationPointer = &gCutsceneAnimationFinished;
                animationValue = *animationPointer;
                signedValue = *animationPointer;
                if (signedValue > 88) {
                    if ((frame & 1) != 0) {
                        s16* secondaryYPointer;

                        signedValue = animationValue - 1;
                        *animationPointer = signedValue;
                        signedValue = *animationPointer;
                        ASM_BARRIER(: "+r"(signedValue));
                        deltaValue = signedValue;
                        deltaValue -= 104;
                        secondaryYPointer = &gUnk_3002C9A;
                        signedValue = deltaValue;
                        signedValue *= deltaValue;
                        if (signedValue < 0) {
                            signedValue += 7;
                        }
                        *secondaryYPointer = (signedValue >> 3) + 112;
                    }
                    {
                        s16* reloadPointer;

                        reloadPointer = &gCutsceneAnimationFinished;
                        signedValue = *reloadPointer;
                    }
                    animationTimer = 6;
                    if (signedValue > 96) {
                        animationTimer = 10;
                    }
                    SelectLayeredCutsceneOamSequence13(animationTimer, &centerOam);
                } else {
                    s32* counterPointer;
                    s32 counterValue;
                    s32 oldCounter;

                    counterPointer = &gUnk_3002C64;
                    counterValue = *counterPointer;
                    oldCounter = counterValue;
                    counterValue++;
                    *counterPointer = counterValue;
                    if (SelectLayeredCutsceneOamSequence14(oldCounter, &centerOam)) {
                        *counterPointer = 0;
                        gUnk_3002C60++;
                    }
                }
            }
            if (gCutsceneAnimationFinished == 120) {
                m4aSongNumStartOrChange(13);
            }
            break;

        case 3: {
            u32* counterPointer;
            u32 counterValue;
            u32 oldCounter;
            s16* secondaryXPointer;

            SelectLayeredCutsceneOamSequence14(100, &centerOam);
            counterPointer = (u32*)&gUnk_3002C64;
            counterValue = *counterPointer;
            oldCounter = counterValue;
            counterValue++;
            *counterPointer = counterValue;
            if (oldCounter > 160) {
                s32 zero;
                s32 secondaryX;

                secondaryXPointer = &gUnk_3002C9C;
                zero = 0;
                secondaryX = *(s16*)secondaryXPointer;
                if (secondaryX <= 119) {
                    register s32 parabolaDelta PIN("r3");
                    s16* secondaryYPointer;
                    register s32 parabolaValue PIN("r0");

                    SelectLayeredCutsceneOamSequence19(0, &secondaryOam);
                    if ((frame & 1) != 0) {
                        (*secondaryXPointer)++;
                    }
                    zero = 0;
                    secondaryX = *(s16*)secondaryXPointer;
                    ASM_BARRIER(: "+r"(secondaryX));
                    parabolaDelta = secondaryX;
                    parabolaDelta -= 116;
                    secondaryYPointer = &gUnk_3002C9E;
                    parabolaValue = parabolaDelta;
                    parabolaValue *= parabolaDelta;
                    *secondaryYPointer = parabolaValue / 2 + 136;
                } else if (secondaryX <= 151) {
                    register s32 parabolaDelta PIN("r3");
                    s16* secondaryYPointer;
                    s32 parabolaValue;

                    SelectLayeredCutsceneOamSequence19(8, &secondaryOam);
                    (*secondaryXPointer)++;
                    zero = 0;
                    secondaryX = *(s16*)secondaryXPointer;
                    ASM_BARRIER(: "+r"(secondaryX));
                    parabolaDelta = secondaryX;
                    parabolaDelta -= 136;
                    secondaryYPointer = &gUnk_3002C9E;
                    parabolaValue = parabolaDelta;
                    parabolaValue *= parabolaDelta;
                    *secondaryYPointer = parabolaValue / 16 + 128;
                } else if (secondaryX == 152) {
                    *counterPointer = 0;
                    gUnk_3002C60++;
                }
                secondaryXPointer = &gUnk_3002C9C;
                zero = 0;
                secondaryX = *(s16*)secondaryXPointer;
                if (secondaryX == 106) {
                    m4aSongNumStartOrChange(13);
                }
                zero = 0;
                secondaryX = *(s16*)secondaryXPointer;
                if (secondaryX == 120) {
                    m4aSongNumStartOrChange(24);
                }
            }
        }
            {
                s16* secondaryXPointer;
                s32 zero;
                s32 secondaryX;

                secondaryXPointer = &gUnk_3002C9C;
                zero = 0;
                secondaryX = *(s16*)secondaryXPointer;
                if (secondaryX <= 119) {
                    SelectLayeredCutsceneOamSequence02(frame, &primaryOam);
                    if (treasureCount != 0) {
                        SelectLayeredCutsceneOamSequence03(frame, &offsetOam);
                        ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence03);
                    }
                    if ((frame & 3) == 3) {
                        gCutscenePrimaryObjectY--;
                    }
                } else {
                    SelectLayeredCutsceneOamSequence04(frame, &primaryOam);
                    if (treasureCount != 0) {
                        SelectLayeredCutsceneOamSequence05(frame, &offsetOam);
                        ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence05);
                    }
                    {
                        s16* primaryYPointer;
                        register u16 primaryYValue PIN("r1");

                        primaryYPointer = &gCutscenePrimaryObjectY;
                        primaryYValue = *primaryYPointer;
                        primaryYValue++;
                        *primaryYPointer = primaryYValue;
                    }
                    zero = 0;
                    secondaryX = *(s16*)secondaryXPointer;
                    if (secondaryX == 122) {
                        m4aSongNumStartOrChange(6);
                    }
                }
            }
            break;

        case 4: {
            u32* counterPointer;
            u32 counterValue;

            SelectLayeredCutsceneOamSequence14(28, &centerOam);
            counterPointer = (u32*)&gUnk_3002C64;
            counterValue = *counterPointer;
            if (counterValue > 160) {
                *counterPointer = 0;
                gUnk_3002C60++;
            } else if (counterValue > 88) {
                SelectLayeredCutsceneOamSequence16(counterValue, &secondaryOam);
            } else if ((counterValue & 8) != 0) {
                SelectLayeredCutsceneOamSequence16(counterValue, &secondaryOam);
                gUnk_3002C9C--;
            } else {
                SelectLayeredCutsceneOamSequence18(counterValue, &secondaryOam);
                if (*counterPointer > 4) {
                    gUnk_3002C9C++;
                }
            }
        }
            gUnk_3002C9E = 144;
            gUnk_3002C64++;
            if ((u32)gUnk_3002C64 <= 39) {
                gCutscenePrimaryObjectY++;
            } else {
                SelectLayeredCutsceneOamSequence02(frame, &primaryOam);
                if (treasureCount != 0) {
                    SelectLayeredCutsceneOamSequence03(frame, &offsetOam);
                    ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence03);
                }
                if ((frame & 3) == 3) {
                    gCutscenePrimaryObjectY--;
                }
            }
            break;

        case 5:
            SelectLayeredCutsceneOamSequence14(100, &centerOam);
            if (gUnk_3002C9C > 88) {
                s16* secondaryXPointer;
                s32 zero;
                register s32 secondaryX PIN("r0");
                register s32 parabolaDelta PIN("r3");
                s16* secondaryYPointer;
                s32 squareValue;
                register s32 parabolaValue PIN("r0");

                secondaryXPointer = &gUnk_3002C9C;
                SelectLayeredCutsceneOamSequence17(frame, &secondaryOam);
                (*secondaryXPointer)--;
                zero = 0;
                secondaryX = *secondaryXPointer;
                ASM_BARRIER(: "+r"(secondaryX));
                parabolaDelta = secondaryX;
                parabolaDelta -= 120;
                secondaryYPointer = &gUnk_3002C9E;
                squareValue = parabolaDelta;
                squareValue *= parabolaDelta;
                parabolaValue = squareValue << 1;
                parabolaValue += squareValue;
                if (parabolaValue < 0) {
                    parabolaValue += 127;
                }
                *secondaryYPointer = (parabolaValue >> 7) + 120;
            } else if (gUnk_3002C9C > -32) {
                SelectLayeredCutsceneOamSequence16(frame, &secondaryOam);
                gUnk_3002C9C -= 2;
            } else {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            SelectLayeredCutsceneOamSequence02(frame, &primaryOam);
            if (treasureCount != 0) {
                SelectLayeredCutsceneOamSequence03(frame, &offsetOam);
                ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence03);
            }
            if ((frame & 3) == 3) {
                gCutscenePrimaryObjectY--;
            }
            break;

        case 6:
            if (gCutscenePrimaryObjectX > 96) {
                if (treasureCount != 0) {
                    SelectLayeredCutsceneOamSequence08(frame, &primaryOam);
                    SelectLayeredCutsceneOamSequence09(frame, &offsetOam);
                    ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence09);
                } else {
                    SelectLayeredCutsceneOamSequence12(frame, &primaryOam);
                }
                {
                    s16* primaryXPointer;
                    register u16 primaryXValue PIN("r1");
                    s32 zero;
                    register s32 primaryXSigned PIN("r0");
                    register s32 parabolaDelta PIN("r3");
                    s16* primaryYPointer;
                    s32 parabolaValue;

                    primaryXPointer = &gCutscenePrimaryObjectX;
                    primaryXValue = *primaryXPointer;
                    primaryXValue--;
                    *primaryXPointer = primaryXValue;
                    zero = 0;
                    primaryXSigned = *primaryXPointer;
                    ASM_BARRIER(: "+r"(primaryXSigned));
                    parabolaDelta = primaryXSigned;
                    parabolaDelta -= 104;
                    primaryYPointer = &gCutscenePrimaryObjectY;
                    parabolaValue = parabolaDelta;
                    parabolaValue *= parabolaDelta;
                    if (parabolaValue < 0) {
                        parabolaValue += 7;
                    }
                    *primaryYPointer = (parabolaValue >> 3) + 136;
                    primaryXSigned = (s16)primaryXValue;
                    if (primaryXSigned == 118) {
                        m4aSongNumStartOrChange(13);
                    }
                }
                SelectLayeredCutsceneOamSequence14(100, &centerOam);
            } else {
                s32 hasTreasure;
                s32* counterPointer;
                s32 counterValue;
                s32 oldCounter;

                hasTreasure = treasureCount;
                if (hasTreasure != 0) {
                    SelectLayeredCutsceneOamSequence06(frame, &primaryOam);
                    SelectLayeredCutsceneOamSequence07(frame, &offsetOam);
                    ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence07);
                } else {
                    SelectLayeredCutsceneOamSequence11(frame, &primaryOam);
                }
                counterPointer = &gUnk_3002C64;
                counterValue = *counterPointer;
                oldCounter = counterValue;
                counterValue++;
                *counterPointer = counterValue;
                if (SelectLayeredCutsceneOamSequence15(oldCounter, &centerOam)) {
                    *counterPointer = 0;
                    gUnk_3002C60++;
                }
            }
            break;

        case 7:
            if (gCutsceneAnimationFinished > -32) {
                SelectLayeredCutsceneOamSequence13(frame, &centerOam);
                gCutsceneAnimationFinished -= 2;
                if (treasureCount != 0) {
                    SelectLayeredCutsceneOamSequence06(frame, &primaryOam);
                    SelectLayeredCutsceneOamSequence07(frame, &offsetOam);
                    ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence07);
                } else {
                    SelectLayeredCutsceneOamSequence11(frame, &primaryOam);
                }
            } else {
                if (treasureCount != 0) {
                    SelectLayeredCutsceneOamSequence00(frame, &primaryOam);
                    SelectLayeredCutsceneOamSequence01(frame, &offsetOam);
                    ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence01);
                } else {
                    SelectLayeredCutsceneOamSequence10(frame, &primaryOam);
                }
                gCutscenePrimaryObjectX -= 2;
                if (gCutscenePrimaryObjectX < -32) {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
            }
            break;

        case 8:
            if (!IncreaseCutsceneBlendCoefficient(1)) {
                break;
            }
            LZ77UnCompVram(sUnk_82A229C, (void*)0x06008000);
            {
                s16* objectPointer;
                vu16* backgroundRegisterPointer;
                s16* animationPointer;
                register s32 horizontalBase PIN("r5");
                register s32 horizontalValue PIN("r1");
                s32 verticalValue;
                register s32 animationOffset PIN("r6");
                register s32 animationValue PIN("r0");
                register s32 primaryOffset PIN("r3");
                register s32 objectY PIN("r2");

                objectPointer = &gCutsceneBackgroundHorizontalOffset;
                horizontalBase = 136;
                horizontalBase <<= 1;
                ASM_BARRIER(: "+r"(horizontalBase));
                horizontalValue = horizontalBase;
                *objectPointer = horizontalValue;
                objectPointer = &gCutsceneBackgroundVerticalOffset;
                verticalValue = 56;
                *objectPointer = verticalValue;
                backgroundRegisterPointer = (vu16*)0x04000010;
                *backgroundRegisterPointer = horizontalValue;
                backgroundRegisterPointer++;
                *backgroundRegisterPointer = verticalValue;
                backgroundRegisterPointer++;
                *backgroundRegisterPointer = horizontalValue;
                backgroundRegisterPointer++;
                *backgroundRegisterPointer = verticalValue;
                animationPointer = &gCutsceneAnimationFinished;
                animationOffset = 128;
                animationOffset <<= 1;
                ASM_BARRIER(: "+r"(animationOffset));
                animationValue = horizontalValue + animationOffset;
                *animationPointer = animationValue;
                objectPointer = &gUnk_3002C9A;
                objectY = 200;
                *objectPointer = objectY;
                objectPointer = &gCutscenePrimaryObjectX;
                primaryOffset = 148;
                primaryOffset <<= 1;
                ASM_BARRIER(: "+r"(primaryOffset));
                horizontalValue += primaryOffset;
                *objectPointer = horizontalValue;
                objectPointer = &gCutscenePrimaryObjectY;
                *objectPointer = objectY;
            }
            WaitForVBlankInterrupt();
            {
                vu16* displayControl;
                register s32 displayValue PIN("r4");
                register s32 displayCopy PIN("r0");

                displayControl = (vu16*)0x04000000;
                displayValue = 152;
                displayValue <<= 5;
                ASM_BARRIER(: "+r"(displayValue));
                displayCopy = displayValue;
                *displayControl = displayCopy;
            }
            gUnk_3002C60++;
            break;

        case 9:
            DecreaseCutsceneBlendCoefficient(1);
            {
                s16* horizontalPointer;
                u16 horizontalValue;
                s32 decrementedValue;
                u32 unsignedDifference;

                horizontalPointer = &gCutsceneBackgroundHorizontalOffset;
                horizontalValue = *horizontalPointer;
                decrementedValue = horizontalValue - 1;
                unsignedDifference = (u16)decrementedValue;
                if (unsignedDifference <= 54) {
                    if ((frame & 1) != 0) {
                        s16* verticalPointer;
                        u16 verticalValue;

                        *horizontalPointer = decrementedValue;
                        verticalPointer = &gCutsceneBackgroundVerticalOffset;
                        verticalValue = *verticalPointer;
                        verticalValue--;
                        *verticalPointer = verticalValue;
                    }
                } else {
                    s32 signedHorizontal;

                    signedHorizontal = (s16)horizontalValue;
                    if (signedHorizontal > 0) {
                        *horizontalPointer = decrementedValue;
                    }
                }
            }
            if (treasureCount != 0) {
                SelectLayeredCutsceneOamSequence00(frame, &primaryOam);
                SelectLayeredCutsceneOamSequence01(frame, &offsetOam);
                ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence01);
            } else {
                SelectLayeredCutsceneOamSequence10(frame, &primaryOam);
            }
            {
                s16* primaryXPointer;
                u16 primaryXValue;
                s32 zero;
                s32 signedPrimaryX;
                register s32 negativeLimit PIN("r0");

                primaryXPointer = &gCutscenePrimaryObjectX;
                primaryXValue = *primaryXPointer;
                zero = 0;
                signedPrimaryX = *primaryXPointer;
                negativeLimit = -32;
                if (signedPrimaryX > negativeLimit) {
                    negativeLimit = primaryXValue - 2;
                    *primaryXPointer = negativeLimit;
                } else {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
            }
            {
                s16* primaryXPointer;
                u16 primaryXValue;
                u32 rangeValue;

                primaryXPointer = &gCutscenePrimaryObjectX;
                primaryXValue = *primaryXPointer;
                rangeValue = primaryXValue;
                rangeValue -= 64;
                rangeValue = (u16)rangeValue;
                if (rangeValue <= 103) {
                    s16* primaryYPointer;
                    s32 zero;
                    s32 signedValue;
                    register s32 result PIN("r0");

                    rangeValue = primaryXValue + 1;
                    *primaryXPointer = rangeValue;
                    primaryYPointer = &gCutscenePrimaryObjectY;
                    zero = 0;
                    signedValue = *primaryXPointer;
                    signedValue -= 64;
                    result = signedValue << 3;
                    result -= signedValue;
                    result <<= 1;
                    result /= 13;
                    result += 88;
                    *primaryYPointer = result;
                }
            }
            SelectLayeredCutsceneOamSequence13(frame, &centerOam);
            {
                s16* animationPointer;
                u16 animationValue;
                s32 zero;
                s32 signedAnimation;
                register s32 negativeLimit PIN("r0");

                animationPointer = &gCutsceneAnimationFinished;
                animationValue = *animationPointer;
                zero = 0;
                signedAnimation = *animationPointer;
                negativeLimit = -32;
                if (signedAnimation > negativeLimit) {
                    negativeLimit = animationValue - 2;
                    *animationPointer = negativeLimit;
                }
                {
                    u16 animationValue2;
                    u32 rangeValue;

                    animationValue2 = *animationPointer;
                    rangeValue = animationValue2;
                    rangeValue -= 64;
                    rangeValue = (u16)rangeValue;
                    if (rangeValue <= 103) {
                        s16* secondaryYPointer;
                        s32 zero2;
                        s32 signedValue;
                        register s32 result PIN("r0");

                        rangeValue = animationValue2 + 1;
                        *animationPointer = rangeValue;
                        secondaryYPointer = &gUnk_3002C9A;
                        zero2 = 0;
                        signedValue = *animationPointer;
                        signedValue -= 64;
                        result = signedValue << 3;
                        result -= signedValue;
                        result <<= 1;
                        result /= 13;
                        result += 88;
                        *secondaryYPointer = result;
                    }
                }
            }
            break;

        case 10:
            if (!IncreaseCutsceneBlendCoefficient(1)) {
                break;
            }
            {
                vu32* fillDma;
                u32 fillConstant;
                u32 fillControl;

                {
                    register u16* fillPointer PIN("r0");
                    register u32 fillLoad PIN("r1");
                    fillPointer = &fillValue;
                    fillLoad = 0x03FF;
                    ASM_BARRIER(: "+r"(fillPointer), "+r"(fillLoad));
                    fillConstant = fillLoad;
                    *fillPointer = fillConstant;
                    fillDma = (vu32*)0x040000D4;
                    fillDma[0] = (u32)fillPointer;
                }
                fillDma[1] = 0x06008000;
                fillControl = 0x81000260;
                fillDma[2] = fillControl;
                fillDma[2];
                DecodeCutsceneTileRuns(sUnk_82A1CF0, (u16*)0x06008000);

                fillValue = fillConstant;
                fillDma[0] = (u32)&fillValue;
                fillDma[1] = 0x06009000;
                fillDma[2] = fillControl;
                fillDma[2];
                DecodeCutsceneTileRuns(sUnk_82A1E7A, (u16*)0x06009000);

                {
                    u16* fillPointer;
                    register u32 thirdFill PIN("r0");
                    register u32 thirdConstant PIN("r2");

                    fillPointer = &fillValue;
                    thirdConstant = 0x93A0;
                    ASM_BARRIER(: "+r"(thirdConstant));
                    thirdFill = thirdConstant;
                    *fillPointer = thirdFill;
                    fillDma[0] = (u32)fillPointer;
                }
                fillDma[1] = 0x0600A380;
                fillDma[2] = 0x810000C0;
                fillDma[2];
                DecodeCutsceneTileRuns(sUnk_82A1FBC, (u16*)0x0600A000);
            }
            {
                s16* objectPointer;
                vu16* backgroundRegisterPointer;
                s16* primaryXPointer;
                s32 horizontalValue;
                s32 zero;
                s32 primaryXValue;

                objectPointer = &gCutsceneBackgroundHorizontalOffset;
                horizontalValue = 16;
                *objectPointer = horizontalValue;
                objectPointer = &gCutsceneBackgroundVerticalOffset;
                zero = 0;
                *objectPointer = zero;
                backgroundRegisterPointer = (vu16*)0x04000010;
                *backgroundRegisterPointer = horizontalValue;
                backgroundRegisterPointer++;
                *backgroundRegisterPointer = zero;
                backgroundRegisterPointer++;
                *backgroundRegisterPointer = horizontalValue;
                backgroundRegisterPointer++;
                *backgroundRegisterPointer = zero;
                objectPointer = &gCutsceneAnimationFinished;
                horizontalValue += 240;
                *objectPointer = horizontalValue;
                objectPointer = &gUnk_3002C9A;
                zero = 112;
                *objectPointer = zero;
                primaryXPointer = &gCutscenePrimaryObjectX;
                primaryXValue = 148;
                primaryXValue <<= 1;
                *primaryXPointer = primaryXValue;
                objectPointer = &gCutscenePrimaryObjectY;
                *objectPointer = zero;
            }
            WaitForVBlankInterrupt();
            {
                vu16* displayControl;
                register s32 displayValue PIN("r3");
                register s32 displayCopy PIN("r0");

                displayControl = (vu16*)0x04000000;
                displayValue = 184;
                displayValue <<= 5;
                ASM_BARRIER(: "+r"(displayValue));
                displayCopy = displayValue;
                *displayControl = displayCopy;
            }
            gUnk_3002C60++;
            break;

        case 11:
            if (DecreaseCutsceneBlendCoefficient(1)) {
                SelectCutsceneGfx5(frame, &gfxOam);
            }
            {
                register s32 guard6 PIN("r6");
                register s32 guard0 PIN("r0");
                register s32 guard1 PIN("r1");
                register s32 guard2 PIN("r2");
                register s32 guard3 PIN("r3");
                ASM_BARRIER(: "=r"(guard6), "=r"(guard0), "=r"(guard1), "=r"(guard2), "=r"(guard3));
                if (treasureCount != 0) {
                    SelectLayeredCutsceneOamSequence00(frame, &primaryOam);
                    SelectLayeredCutsceneOamSequence01(frame, &offsetOam);
                    ConfigureLayeredCutsceneOamOffsets(SelectLayeredCutsceneOamSequence01);
                } else {
                    ASM_BARRIER(: : "r"(guard6), "r"(guard0), "r"(guard1), "r"(guard2), "r"(guard3));
                    SelectLayeredCutsceneOamSequence10(frame, &primaryOam);
                }
            }
            if (gCutscenePrimaryObjectX > -32) {
                gCutscenePrimaryObjectX -= 2;
            } else {
                InitCutsceneBrightenBlend(55);
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            if (gCutscenePrimaryObjectX <= 31) {
                gCutscenePrimaryObjectY = ((32 - gCutscenePrimaryObjectX) * 6) / 5 + 112;
            }
            SelectLayeredCutsceneOamSequence13(frame, &centerOam);
            if (gCutsceneAnimationFinished > -32) {
                gCutsceneAnimationFinished -= 2;
            }
            if (gCutsceneAnimationFinished <= 31) {
                gUnk_3002C9A = ((32 - gCutsceneAnimationFinished) * 6) / 5 + 112;
            }
            break;

        case 12:
            if (IncreaseCutsceneBlendCoefficient(7)) {
                gSubGameMode++;
            }
            break;
    }

    *(vu16*)0x04000010 = gCutsceneBackgroundHorizontalOffset;
    *(vu16*)0x04000012 = verticalWave + gCutsceneBackgroundVerticalOffset;
    *(vu16*)0x04000014 = gCutsceneBackgroundHorizontalOffset;
    *(vu16*)0x04000016 = verticalWave + gCutsceneBackgroundVerticalOffset;

    destination = (u16*)gOamBuffer;
    if (gUnk_3002C60 <= 10) {
        u16* xPositions;
        u16* yPositions;
        u8** objectPointer;

        xPositions = (u16*)gCutsceneObjectXPositions;
        yPositions = (u16*)gCutsceneObjectYPositions;
        UpdateLayeredCutsceneObjectPositions(gCutsceneBackgroundHorizontalOffset > 0);
        objectPointer = &objectOam;
        SelectLayeredCutsceneOamSequence20(frame, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions++, *yPositions++);
        SelectLayeredCutsceneOamSequence21(frame, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions++, *yPositions++);
        SelectLayeredCutsceneOamSequence22(frame, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions++, *yPositions++);
        SelectLayeredCutsceneOamSequence23(frame, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions++, *yPositions++);
        SelectLayeredCutsceneOamSequence24(frame, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions++, *yPositions++);
        SelectLayeredCutsceneOamSequence20(frame + 24, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions++, *yPositions++);
        SelectLayeredCutsceneOamSequence20(frame + 56, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions++, *yPositions++);
        SelectLayeredCutsceneOamSequence20(frame + 72, objectPointer);
        destination = AppendCutsceneOamTemplate((const u16*)objectOam, destination, *xPositions, *yPositions);
    }
    if (gUnk_3002C60 == 11) {
        destination = AppendCutsceneOamTemplate((const u16*)gfxOam, destination, 216, 88);
    }

    {
        const u16* selectedOam;
        s32 centerX;
        s32 centerY;
        s32 backgroundY;

        selectedOam = (const u16*)centerOam;
        centerX = gCutsceneAnimationFinished - gCutsceneBackgroundHorizontalOffset;
        centerY = gUnk_3002C9A;
        backgroundY = gCutsceneBackgroundVerticalOffset;
        backgroundY--;
        centerY -= backgroundY;
        centerY -= verticalWave;
        destination = AppendCutsceneOamTemplate(selectedOam, destination, centerX, centerY);
    }
    destination = AppendCutsceneOamTemplate(
        (const u16*)secondaryOam,
        destination,
        gUnk_3002C9C - gCutsceneBackgroundHorizontalOffset,
        gUnk_3002C9E - gCutsceneBackgroundVerticalOffset - verticalWave
    );
    if (gUnk_3002C60 <= 5) {
        destination = AppendCutsceneOamTemplate(
            (const u16*)offsetOam,
            destination,
            gCutscenePrimaryObjectX - gCutsceneBackgroundHorizontalOffset + gLayeredCutsceneOamOffsetX,
            gCutscenePrimaryObjectY - gCutsceneBackgroundVerticalOffset - verticalWave + gLayeredCutsceneOamOffsetY
        );
    }
    {
        register const u16* selectedOam PIN("r0");
        s16* primaryXPointer;
        register s16* primaryYPointer PIN("r8");
        s16* primaryYLoadPointer;
        s32 primaryYLoaded;
        s32 primaryXPosition;
        s32 primaryYPosition;
        register s32 primaryYCopy PIN("ip");

        selectedOam = (const u16*)primaryOam;
        primaryXPointer = &gCutscenePrimaryObjectX;
        primaryXPosition = *primaryXPointer;
        primaryYLoaded = gCutsceneBackgroundHorizontalOffset;
        primaryXPosition -= primaryYLoaded;
        primaryYLoadPointer = &gCutscenePrimaryObjectY;
        primaryYPointer = primaryYLoadPointer;
        primaryYLoaded = *primaryYLoadPointer;
        ASM_BARRIER(: "+r"(primaryYLoaded));
        primaryYCopy = primaryYLoaded;
        primaryYLoaded = gCutsceneBackgroundVerticalOffset;
        primaryYPosition = primaryYCopy;
        ASM_BARRIER(: "+r"(primaryYLoaded), "+r"(primaryYPosition));
        primaryYPosition -= primaryYLoaded;
        primaryYPosition -= verticalWave;
        primaryYCopy = primaryYPosition;
        ASM_BARRIER_VOLATILE(: "+r"(primaryYCopy));
        destination = AppendCutsceneOamTemplate(selectedOam, destination, primaryXPosition, primaryYPosition);
        if (gUnk_3002C60 > 5) {
            register s16* primaryYLowPointer PIN("r4");

            selectedOam = (const u16*)offsetOam;
            primaryXPosition = *primaryXPointer - gCutsceneBackgroundHorizontalOffset;
            primaryXPosition += gLayeredCutsceneOamOffsetX;
            primaryYLowPointer = primaryYPointer;
            {
                s32 backgroundYLoaded;

                primaryYPosition = *primaryYLowPointer;
                backgroundYLoaded = gCutsceneBackgroundVerticalOffset;
                primaryYPosition -= backgroundYLoaded;
            }
            primaryYPosition -= verticalWave;
            primaryYPosition += gLayeredCutsceneOamOffsetY;
            destination = AppendCutsceneOamTemplate(selectedOam, destination, primaryXPosition, primaryYPosition);
        }
    }
    if (treasureCount != 0) {
        WriteCutsceneOamAffineMatrix(
            0, 0, (*(s16*)&gEndingCutsceneTreasureScale), (*(s16*)&gEndingCutsceneTreasureScale)
        );
    }
    FinalizeCutsceneOamBuffer(destination);
}

s32 GetEndingCutsceneTreasureX(void)
{
    u32 chestCount;

    chestCount = (u16)gUnk_3002C84;
    if (chestCount == 0) {
        return 141;
    }
    if (chestCount == 1) {
        return 137;
    }
    if (chestCount == 2) {
        return 132;
    }
    return 126;
}

void InitializeEndingTreasureCutscene(void)
{
    vu32* dma;
    u32 fillValue;
    vu16* verticalScroll;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_82A8820;
    dma[1] = PLTT;
    dma[2] = 0x80000060;
    dma[2];

    dma[0] = (u32)sUnk_82AA5B2;
    dma[1] = OBJ_PLTT;
    dma[2] = 0x80000100;
    dma[2];

    LZ77UnCompVram((const u32*)sUnk_829B2C0, (void*)VRAM);
    LZ77UnCompVram((const u32*)sUnk_82A88E0, (void*)0x06004400);
    LZ77UnCompVram((const u32*)sUnk_82A9508, (void*)0x06006800);
    LZ77UnCompVram((const u32*)sUnk_82AA7B4, OBJ_VRAM0);

    fillValue = 0x03FF03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06008000;
    dma[2] = 0x85000800;
    dma[2];

    DecodeCutsceneTileRuns(sUnk_82AA07C, (u16*)0x06008000);
    DecodeCutsceneTileRuns(sUnk_829D212, (u16*)0x06008800);
    DecodeCutsceneTileRuns(sUnk_829D270, (u16*)0x06009000);
    DecodeCutsceneTileRuns(sUnk_82AA2B0, (u16*)0x06009800);

    REG_BG0CNT = 0x1000;
    REG_BG1CNT = 0x1101;
    REG_BG2CNT = 0x1202;
    REG_BG3CNT = 0x1303;

    InitCutsceneBrightenBlendFromWhite(63);

    {
        s32 negativeValue;
        register s16 initialScroll PIN("r1");

        gCutsceneAnimationFinished = (initialScroll = (negativeValue = -16));

        verticalScroll = (vu16*)REG_ADDR_BG0VOFS;
        *verticalScroll = 0;
        verticalScroll += 2;
        *verticalScroll = initialScroll;
        verticalScroll += 2;
        *verticalScroll = 0;
        verticalScroll += 2;
        *verticalScroll = 0;
    }

    gUnk_3002C9E = GetEndingCutsceneTreasureX();
    gCutscenePrimaryObjectX = 136;
    gCutscenePrimaryObjectY = 144;
    gUnk_3002C9A = 136;
    gUnk_3002C9C = 144;
    gLayeredCutsceneOamOffsetX = 0;
    gLayeredCutsceneOamOffsetY = 0;
    gUnk_3002CA4 = 0;

    WaitForVBlankInterrupt();
    REG_WINOUT = 0x083F;
    REG_DISPCNT = 0x9F00;

    m4aMPlayFadeOut(GP(gMPlayTable[gSongTable[800].ms].info), 30);
    m4aSongNumStartOrChange(420);
}

void ApplyCutsceneOamAffineMatrix(u16* oamData, u16* destination, u16 matrixIndex)
{
    u16 affineBits = matrixIndex;

    if (oamData != 0) {
        u16 count = *oamData;
        u16* entry = destination - count * 4;

        affineBits = (affineBits & 0x7F) << 9;
        while (count-- != 0) {
            *entry = (*entry | 0x300) - 4;
            entry++;
            *entry = (*entry | affineBits) - 4;
            entry += 3;
        }
    }
}

#define RENDER_PRIMARY_PATH(START, SELECTOR, PATHFN, MATRIX) \
    do { \
        s32 pathTimer = pathFrame - (START); \
        SELECTOR(pathTimer, pathOut); \
        if (PATHFN(pathTimer, &outputs[0], yOut, scaleXOut, scaleYOut)) { \
            gLayeredCutsceneOamOffsetX |= 1; \
            gLayeredCutsceneOamOffsetY++; \
        } \
        oamDest = AppendCutsceneOamTemplate(pathOam, oamDest, (s16)outputs[0], (s16)*yOut); \
        WriteCutsceneOamAffineMatrix((MATRIX), 0, (s16)*scaleXOut, (s16)*scaleYOut); \
    } while (0)

#define RENDER_PRIMARY_PATH_TIMER(TIMER, SELECTOR, PATHFN, MATRIX) \
    do { \
        s32 pathTimer = (TIMER); \
        SELECTOR(pathTimer, pathOut); \
        if (PATHFN(pathTimer, &outputs[0], yOut, scaleXOut, scaleYOut)) { \
            gLayeredCutsceneOamOffsetX |= 1; \
            gLayeredCutsceneOamOffsetY++; \
        } \
        oamDest = AppendCutsceneOamTemplate(pathOam, oamDest, (s16)outputs[0], (s16)*yOut); \
        WriteCutsceneOamAffineMatrix((MATRIX), 0, (s16)*scaleXOut, (s16)*scaleYOut); \
    } while (0)

#define RENDER_AFFINE_PATH(START, PATHFN, MATRIX) \
    do { \
        if (pathFrame >= (START)) { \
            s32 pathTimer = pathFrame - (START); \
            SelectEndingCutsceneOamSequence26(pathTimer, pathOut); \
            if (!PATHFN(pathTimer, &outputs[0], yOut, scaleXOut, scaleYOut)) { \
                oamDest = AppendCutsceneOamTemplate(pathOam, oamDest, (s16)outputs[0], (s16)*yOut); \
                ApplyCutsceneOamAffineMatrix(pathOam, oamDest, (MATRIX)); \
                WriteCutsceneOamAffineMatrix((MATRIX), 0, (s16)*scaleXOut, (s16)*scaleYOut); \
            } \
        } \
    } while (0)

void func_8008B20(u32 frame)
{
    u16* mainOam;
    u16* secondaryOam;
    u16* thirdOam;
    u16* pathOam;
    u16* tempOam;
    u16 outputs[4];
    s32 renderPaths = 0;
    s32 sequenceResult;
    u32 timer;
    register s32 pathFrame PIN("r8");
    s32 pathBase;

    secondaryOam = 0;
    mainOam = 0;
    pathOam = 0;
    thirdOam = 0;

    switch (gUnk_3002C60) {
        case 0:
            if (DecreaseCutsceneBlendCoefficient(15)) {
                u16 oldX = gCutsceneAnimationFinished;
                if (gCutsceneAnimationFinished > -120) {
                    if ((frame & 7) == 7) {
                        volatile u16* scrollRegister = (volatile u16*)0x04000016;
                        gCutsceneAnimationFinished = oldX - 1;
                        *scrollRegister = oldX;
                    }
                } else {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
            }
            *(volatile u16*)0x04000014 = (frame & 3) - 1;
            SelectEndingCutsceneOamSequence00(frame, &mainOam);
            break;

        case 1:
            timer = gUnk_3002C64;
            if (timer <= 119) {
                SelectEndingCutsceneOamSequence01(timer, &mainOam);
            } else if (timer <= 239) {
                SelectEndingCutsceneOamSequence04(timer - 120, &mainOam);
                if (gUnk_3002C64 == 120) {
                    m4aSongNumStartOrChange(0x322);
                }
            } else {
                if (SelectEndingCutsceneOamSequence05(timer - 240, &mainOam)) {
                    gUnk_3002C64 = 0;
                    gUnk_3002C68 = 0;
                    gUnk_3002C60++;
                    break;
                }
            }
            gUnk_3002C64++;
            break;

        case 2:
            renderPaths = 1;
            if (gLayeredCutsceneOamOffsetX != 0) {
                m4aSongNumStartOrChange(428);
                if (gLayeredCutsceneOamOffsetY == 1) {
                    gUnk_3002CA4 = 1;
                    gUnk_3002CA6 = 0;
                }
                gUnk_3002C68 = 8;
            }

            if (gUnk_3002C68 != 0) {
                SelectEndingCutsceneOamSequence07(0, &secondaryOam);
                gUnk_3002C68--;
            } else {
                SelectEndingCutsceneOamSequence06(0, &secondaryOam);
            }

            if (gUnk_3002CA4 != 0) {
                u16 old = gUnk_3002CA6;
                gUnk_3002CA6 = old + 1;
                SelectEndingCutsceneOamSequence08((s16)old, &mainOam);
            } else {
                SelectEndingCutsceneOamSequence02(gUnk_3002C64, &mainOam);
            }
            gUnk_3002C64++;

            if (gLayeredCutsceneOamOffsetY == 4) {
                gUnk_3002C68 = 0;
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 3:
            gUnk_3002C9A = 96;
            gUnk_3002C9C = 144;
            {
                u16 variant = gUnk_3002C84;
                if (variant == 0) {
                    sequenceResult = SelectEndingCutsceneOamSequence32(gUnk_3002C64, &secondaryOam);
                } else if (variant == 1) {
                    sequenceResult = SelectEndingCutsceneOamSequence31(gUnk_3002C64, &secondaryOam);
                } else if (variant == 2) {
                    sequenceResult = SelectEndingCutsceneOamSequence29(gUnk_3002C64, &secondaryOam);
                } else if (variant == 3) {
                    sequenceResult = SelectEndingCutsceneOamSequence30(gUnk_3002C64, &secondaryOam);
                }
            }

            if (sequenceResult == 2) {
                gUnk_3002CA4 = 0;
            }

            if (gUnk_3002CA4 != 0) {
                u16 old = gUnk_3002CA6;
                gUnk_3002CA6 = old + 1;
                SelectEndingCutsceneOamSequence08((s16)old, &mainOam);
            } else {
                SelectEndingCutsceneOamSequence09(gUnk_3002C68, &mainOam);
                SelectEndingCutsceneOamSequence28(gUnk_3002C68, &thirdOam);
                gUnk_3002C68++;
            }

            *(volatile u16*)0x0400001E = frame;
            gUnk_3002C64++;
            if (sequenceResult == 1 && IncreaseCutsceneBlendCoefficient(7)) {
                gSubGameMode++;
            }
            break;
    }

    {
        u16* oamDest = (u16*)gOamBuffer;
        register u16* yOut PIN("r5") = &outputs[1];

        {
            s32 renderCheck = renderPaths;
            if (renderCheck) {
                pathBase = gUnk_3002C64;
                pathFrame = pathBase - 1;
                if (pathFrame == 0) {
                    m4aSongNumStartOrChange(0x1AD);
                }
                if (pathFrame == 90) {
                    m4aSongNumStartOrChange(430);
                }

                gLayeredCutsceneOamOffsetX = 0;

                {
                    u16** pathOut = &pathOam;
                    u16* scaleXOut = &outputs[2];
                    u16* scaleYOut = &outputs[3];
                    if (pathFrame > 44) {
                        RENDER_PRIMARY_PATH_TIMER(
                            pathBase - 46, SelectEndingCutsceneOamSequence24, ReadEndingCutscenePath03, 4
                        );
                    }
                    if (pathFrame > 29) {
                        RENDER_PRIMARY_PATH(30, SelectEndingCutsceneOamSequence23, ReadEndingCutscenePath02, 3);
                    }
                    if (pathFrame > 14) {
                        RENDER_PRIMARY_PATH(15, SelectEndingCutsceneOamSequence22, ReadEndingCutscenePath01, 2);
                    }
                    SelectEndingCutsceneOamSequence21(pathFrame, pathOut);
                    if (ReadEndingCutscenePath00(pathFrame, &outputs[0], yOut, scaleXOut, scaleYOut)) {
                        gLayeredCutsceneOamOffsetX |= 1;
                        gLayeredCutsceneOamOffsetY++;
                    }
                    oamDest = AppendCutsceneOamTemplate(pathOam, oamDest, (s16)outputs[0], (s16)*yOut);
                    WriteCutsceneOamAffineMatrix(1, 0, (s16)*scaleXOut, (s16)*scaleYOut);

                    RENDER_AFFINE_PATH(45, ReadEndingCutscenePath28, 29);
                    RENDER_AFFINE_PATH(49, ReadEndingCutscenePath29, 30);
                    RENDER_AFFINE_PATH(53, ReadEndingCutscenePath30, 31);
                    RENDER_AFFINE_PATH(57, ReadEndingCutscenePath31, 31);
                    RENDER_AFFINE_PATH(61, ReadEndingCutscenePath32, 31);
                    RENDER_AFFINE_PATH(65, ReadEndingCutscenePath33, 31);
                    RENDER_AFFINE_PATH(69, ReadEndingCutscenePath34, 31);
                    RENDER_AFFINE_PATH(73, ReadEndingCutscenePath35, 31);

                    RENDER_AFFINE_PATH(30, ReadEndingCutscenePath20, 21);
                    RENDER_AFFINE_PATH(34, ReadEndingCutscenePath21, 22);
                    RENDER_AFFINE_PATH(38, ReadEndingCutscenePath22, 23);
                    RENDER_AFFINE_PATH(42, ReadEndingCutscenePath23, 24);
                    RENDER_AFFINE_PATH(46, ReadEndingCutscenePath24, 25);
                    RENDER_AFFINE_PATH(50, ReadEndingCutscenePath25, 26);
                    RENDER_AFFINE_PATH(54, ReadEndingCutscenePath26, 27);
                    RENDER_AFFINE_PATH(58, ReadEndingCutscenePath27, 28);

                    RENDER_AFFINE_PATH(15, ReadEndingCutscenePath12, 13);
                    RENDER_AFFINE_PATH(19, ReadEndingCutscenePath13, 14);
                    RENDER_AFFINE_PATH(23, ReadEndingCutscenePath14, 15);
                    RENDER_AFFINE_PATH(27, ReadEndingCutscenePath15, 16);
                    RENDER_AFFINE_PATH(31, ReadEndingCutscenePath16, 17);
                    RENDER_AFFINE_PATH(35, ReadEndingCutscenePath17, 18);
                    RENDER_AFFINE_PATH(39, ReadEndingCutscenePath18, 19);
                    RENDER_AFFINE_PATH(43, ReadEndingCutscenePath19, 20);

                    RENDER_AFFINE_PATH(0, ReadEndingCutscenePath04, 5);
                    RENDER_AFFINE_PATH(4, ReadEndingCutscenePath05, 6);
                    RENDER_AFFINE_PATH(8, ReadEndingCutscenePath06, 7);
                    RENDER_AFFINE_PATH(12, ReadEndingCutscenePath07, 8);
                    RENDER_AFFINE_PATH(16, ReadEndingCutscenePath08, 9);
                    RENDER_AFFINE_PATH(20, ReadEndingCutscenePath09, 10);
                    RENDER_AFFINE_PATH(24, ReadEndingCutscenePath10, 11);
                    RENDER_AFFINE_PATH(28, ReadEndingCutscenePath11, 12);
                }
            }
        }

        if (gEndingCutsceneCollectedTreasureCount != 0) {
            SelectEndingCutsceneOamSequence25(gUnk_3002C64, &tempOam);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 184, gUnk_3002C9E);
            {
                s16 affineScale = gEndingCutsceneTreasureScale;
                WriteCutsceneOamAffineMatrix(0, 0, affineScale, affineScale);
            }
        }

        if (gUnk_3002C84 == 0) {
            *yOut = gUnk_3002C9C - 12;
        } else if (gUnk_3002C84 == 1) {
            *yOut = gUnk_3002C9C - 20;
        } else {
            *yOut = gUnk_3002C9C - 24;
        }

        oamDest = AppendCutsceneOamTemplate(thirdOam, oamDest, gUnk_3002C9A, (s16)*yOut);
        oamDest = AppendCutsceneOamTemplate(secondaryOam, oamDest, gUnk_3002C9A, gUnk_3002C9C);
        oamDest = AppendCutsceneOamTemplate(mainOam, oamDest, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);

        if (gUnk_3002C60 == 0) {
            SelectEndingCutsceneOamSequence20(frame, &tempOam);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 60, 128);
            SelectEndingCutsceneOamSequence18(frame, &tempOam);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 72, 130);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 88, 134);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 104, 138);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 120, 140);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 136, 136);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 152, 132);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 168, 128);
            SelectEndingCutsceneOamSequence19(frame, &tempOam);
            oamDest = AppendCutsceneOamTemplate(tempOam, oamDest, 180, 128);
        }

        FinalizeCutsceneOamBuffer(oamDest);
    }
}

void InitializeEndingBackgroundSetup(void)
{
    u32 fillValue;
    u32 firstBgControlValue;

    {
        vu32* dma;
        u32 tier;
        u16* primaryTilemap;
        u32 control;

        tier = *(u16*)&gUnk_3002C84;
        fillValue = 0x03FF03FF;
        dma = (vu32*)REG_ADDR_DMA3;
        dma[0] = (u32)&fillValue;
        primaryTilemap = (u16*)0x06008000;
        dma[1] = (u32)primaryTilemap;
        dma[2] = 0x85000A00;
        dma[2];

        dma[0] = (u32)sUnk_82CFB22;
        dma[1] = PLTT;
        control = 0x80000010;
        dma[2] = control;
        dma[2];

        dma[0] = (u32)sUnk_82CAEE6;
        dma[1] = 0x05000020;
        dma[2] = control;
        dma[2];

        if (tier == 0) {
            LZ77UnCompVram(sUnk_82CAF08, (void*)0x06004000);
            DecodeCutsceneTileRuns(sUnk_82CBD78, primaryTilemap);
            LZ77UnCompVram(sUnk_82D1060, (void*)VRAM);
            DecodeCutsceneTileRuns(sUnk_82D239C, (u16*)0x06009000);
        } else if (tier == 1) {
            LZ77UnCompVram(sUnk_82CBE44, (void*)0x06004000);
            DecodeCutsceneTileRuns(sUnk_82CD390, primaryTilemap);
            LZ77UnCompVram(sUnk_82D1060, (void*)VRAM);
            DecodeCutsceneTileRuns(sUnk_82D239C, (u16*)0x06009000);
        } else if (tier == 2) {
            LZ77UnCompVram(sUnk_82CD4B4, (void*)0x06004000);
            DecodeCutsceneTileRuns(sUnk_82CE598, primaryTilemap);
            LZ77UnCompVram(sUnk_82CFB44, (void*)VRAM);
            DecodeCutsceneTileRuns(sUnk_82D0F64, (u16*)0x06009000);
        } else {
            LZ77UnCompVram(sUnk_82CE704, (void*)0x06004000);
            DecodeCutsceneTileRuns(sUnk_82CFA14, primaryTilemap);
            LZ77UnCompVram(sUnk_82CFB44, (void*)VRAM);
            DecodeCutsceneTileRuns(sUnk_82D0F64, (u16*)0x06009000);
        }
    }

    {
        vu32* dma;

        dma = (vu32*)REG_ADDR_DMA3;
        dma[0] = (u32)sUnk_82C9FCC;
        dma[1] = 0x050001C0;
        dma[2] = 0x80000020;
        dma[2];

        LZ77UnCompVram(sUnk_82CA00C, (void*)0x06007000);
        DecodeCutsceneTileRuns(sUnk_82CAD88, (u16*)0x06009800);
        LZ77UnCompVram(sUnk_82CAC74, (void*)0x0600A000);

        dma[0] = (u32)sUnk_82D248E;
        dma[1] = 0x05000200;
        dma[2] = 0x80000010;
        dma[2];
    }

    LZ77UnCompVram(sUnk_82D24B0, (void*)OBJ_VRAM0);

    {
        vu16* bgRegister;
        u32 registerValue;
        register u16 storeValue PIN("r0");

        bgRegister = (vu16*)REG_ADDR_BG0CNT;
        registerValue = 0x5000;
        firstBgControlValue = registerValue;
        *bgRegister = (storeValue = firstBgControlValue);
        bgRegister++;

        registerValue = 0x1201;
        *bgRegister = (storeValue = registerValue);
        bgRegister++;

        registerValue = 0x1302;
        *bgRegister = (storeValue = registerValue);
        bgRegister++;

        registerValue = 0x1403;
        *bgRegister = (storeValue = registerValue);
    }

    CalculateCutsceneScrollPath02((s16)gUnk_3002C84, 0, &gCutsceneAnimationFinished, &gUnk_3002C9A);
    CalculateCutsceneScrollPath01((s16)gUnk_3002C84, 0, &gUnk_3002C9C, &gUnk_3002C9E);

    {
        vu16* scroll;
        register u16 value PIN("r0");

        scroll = (vu16*)REG_ADDR_BG0HOFS;
        value = gCutsceneAnimationFinished;
        *scroll = value;
        scroll++;

        value = gUnk_3002C9A;
        *scroll = value;
        scroll++;

        value = gUnk_3002C9C;
        *scroll = value;
        scroll += 3;

        value = gUnk_3002C9E;
        *scroll = value;
    }

    gLayeredCutsceneOamOffsetX = 0;
    gLayeredCutsceneOamOffsetY = 0;
    gUnk_3002C68 = 0;
    gUnk_3002C5A = 0;
    gUnk_3002C5C = 16;
    WaitForVBlankInterrupt();
    InitCutsceneBrightenBlendFromWhite(63);
    REG_DISPCNT = 0x1B00;
}

void UpdateEndingScrollCutscene(s32 frame)
{
    u8* oamFrame;
    vu16* scroll;

    oamFrame = 0;
    switch ((u32)gUnk_3002C60) {
        case 0:
            if (DecreaseCutsceneBlendCoefficient(3) != 0) {
                REG_BLDALPHA = (gUnk_3002C5C << 8) | gUnk_3002C5A;
                REG_BLDCNT = 0x0844;
                REG_DISPCNT = 0x1F00;
                gUnk_3002C60++;
            }
            break;
        case 1:
            if ((frame & 3) == 3) {
                if ((u16)gUnk_3002C5A > 6) {
                    if ((u16)gUnk_3002C5C > 8) {
                        gUnk_3002C5C--;
                    }
                }
                if ((u16)gUnk_3002C5A <= 13) {
                    gUnk_3002C5A++;
                    REG_BLDALPHA = (gUnk_3002C5C << 8) | gUnk_3002C5A;
                } else {
                    gUnk_3002C60++;
                }
            }
            break;
        case 2:
            gLayeredCutsceneOamOffsetX = 1;
            if (gLayeredCutsceneOamOffsetY == 1) {
                gUnk_3002C60++;
            }
            break;
        case 3:
            if ((frame & 3) == 3) {
                if ((u16)gUnk_3002C5C <= 15) {
                    gUnk_3002C5C++;
                }
                if (*(u16*)&gUnk_3002C5A != 0) {
                    *(u16*)&gUnk_3002C5A = *(u16*)&gUnk_3002C5A - 1;
                    REG_BLDALPHA = (gUnk_3002C5C << 8) | *(u16*)&gUnk_3002C5A;
                } else {
                    InitCutsceneBrightenBlend(0x3F);
                    REG_DISPCNT = 0x1B00;
                    gUnk_3002C60++;
                }
            }
            break;
        case 4:
            if (IncreaseCutsceneBlendCoefficient(7) != 0) {
                gSubGameMode++;
            }
            break;
    }

    CalculateCutsceneScrollPath02((s16)gUnk_3002C84, frame, &gCutsceneAnimationFinished, &gUnk_3002C9A);
    CalculateCutsceneScrollPath01((s16)gUnk_3002C84, frame, &gUnk_3002C9C, &gUnk_3002C9E);

    scroll = (vu16*)0x04000010;
    *scroll = gCutsceneAnimationFinished;
    scroll++;
    *scroll = gUnk_3002C9A;
    scroll++;
    *scroll = gUnk_3002C9C;
    scroll += 3;
    *scroll = gUnk_3002C9E;
    scroll--;
    *scroll = -(frame >> 1);
    scroll += 2;
    *scroll = -(frame >> 2);

    if (gLayeredCutsceneOamOffsetX != 0) {
        if (gUnk_3002C78 != 0) {
            gLayeredCutsceneOamOffsetY = SelectCutsceneScrollFrameSequence02(gUnk_3002C68++, &oamFrame);
        } else {
            gLayeredCutsceneOamOffsetY = SelectCutsceneScrollFrameSequence01(gUnk_3002C68++, &oamFrame);
        }
        FinalizeCutsceneOamBuffer(AppendCutsceneOamTemplate((u16*)oamFrame, (u16*)gOamBuffer, 120, 140));
    }
}

void InitializeEndingCutsceneParticlePositions(s32 seed)
{
    u16* xPosition;
    u16* yPosition;
    s16* randomSeed;
    s32 multiplier;
    s32 addend;
    s32 value;
    s32 adjusted;
    s32 count;

    xPosition = gCutsceneObjectXPositions;
    yPosition = gCutsceneObjectYPositions;
    randomSeed = &gUnk_3002CA6;
    multiplier = 109;
    addend = 1021;
    count = 7;
    do {
        seed = multiplier * seed;
        value = seed + addend;
        adjusted = value;
        if (value < 0) {
            adjusted = seed + 1084;
        }
        seed = adjusted >> 6;
        seed = value - (seed << 6);
        value = multiplier;
        *xPosition = (seed + 24) << 8;
        xPosition++;

        seed = value * seed;
        value = seed + addend;
        adjusted = value;
        if (value < 0) {
            adjusted = seed + 1052;
        }
        seed = adjusted >> 5;
        seed = value - (seed << 5);
        *yPosition = seed;
        yPosition++;
        count--;
    } while (count >= 0);

    *randomSeed = seed;
}

void UpdateEndingCutsceneParticlePositions(void)
{
    u16* xPosition;
    u16* yPosition;
    u16* xBase;
    u16* yBase;
    s32 count;

    xPosition = gCutsceneObjectXPositions;
    yPosition = gCutsceneObjectYPositions;
    yBase = yPosition;

    *yPosition = *yPosition + 1;
    yPosition++;
    *yPosition = *yPosition + 1;
    yPosition++;
    *yPosition = *yPosition + 1;
    yPosition++;
    *yPosition = *yPosition + 1;
    yPosition++;
    *yPosition = *yPosition + 1;
    yPosition++;
    *yPosition = *yPosition + 1;
    yPosition++;
    *yPosition = *yPosition + 1;
    yPosition++;
    *yPosition = *yPosition + 1;

    xBase = xPosition;
    *xPosition = *xPosition + 35;
    xPosition++;
    *xPosition = *xPosition + 35;
    xPosition++;
    *xPosition = *xPosition + 35;
    xPosition++;
    *xPosition = *xPosition + 35;
    xPosition++;
    *xPosition = *xPosition + 35;
    xPosition++;
    *xPosition = *xPosition + 35;
    xPosition++;
    *xPosition = *xPosition + 35;
    xPosition++;
    *xPosition = *xPosition + 35;

    xPosition = xBase;
    yPosition = yBase;
    count = 7;
    do {
        if ((s16)*yPosition > 32) {
            *yPosition = *yPosition - 40;
            {
                s16* randomSeed;
                randomSeed = &gUnk_3002CA6;
                *randomSeed = (109 * *randomSeed + 1021) % 64;
                {
                    s16 signedSeed;

                    signedSeed = *randomSeed;
                    *xPosition = (signedSeed + 24) << 8;
                }
            }
        }
        count--;
        xPosition++;
        yPosition++;
    } while (count >= 0);
}

void InitializeEndingCutscene(void)
{
    vu32* dma;
    u32 fillValue;
    vu16* scrollRegister;
    s16* treasurePosition;
    s32 treasureX;

    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)sUnk_82A8820;
    dma[1] = PLTT;
    dma[2] = 0x80000060;
    dma[2];

    dma[0] = (u32)sUnk_82AA5B2;
    dma[1] = 0x05000200;
    dma[2] = 0x80000100;
    dma[2];

    LZ77UnCompVram((const u32*)sUnk_829B2C0, (void*)0x06000000);
    LZ77UnCompVram((const u32*)sUnk_82A88E0, (void*)0x06004400);
    LZ77UnCompVram((const u32*)sUnk_82A9508, (void*)0x06006800);
    LZ77UnCompVram((const u32*)sUnk_82AA7B4, (void*)0x06010000);

    fillValue = 0x43404340;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06008000;
    dma[2] = 0x85000400;
    dma[2];
    DecodeCutsceneTileRuns(sUnk_82AA202, (u16*)0x06008000);

    fillValue = 0x03FF03FF;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06009000;
    dma[2] = 0x85000200;
    dma[2];
    DecodeCutsceneTileRuns(sUnk_82AA07C, (u16*)0x06009000);
    DecodeCutsceneTileRuns(sUnk_829D270, (u16*)0x06009800);

    REG_BG0CNT = 0x9000;
    REG_BG1CNT = 0x1201;
    REG_BG2CNT = 0x1302;

    scrollRegister = (vu16*)0x04000010;
    *scrollRegister = 0;
    scrollRegister++;
    *scrollRegister = 0;
    scrollRegister += 2;
    *scrollRegister = 0;
    scrollRegister += 2;
    *scrollRegister = 0;
    scrollRegister += 2;
    *scrollRegister = 0;

    gCutscenePrimaryObjectX = 136;
    gCutscenePrimaryObjectY = 144;
    gCutsceneAnimationFinished = 96;
    gUnk_3002C9A = 144;
    gUnk_3002C9C = 184;

    treasurePosition = &gUnk_3002C9E;
    *treasurePosition = treasureX = GetEndingCutsceneTreasureX();
    gLayeredCutsceneOamOffsetX = treasureX - 4;

    {
        s32 tier;

        tier = *(u16*)&gUnk_3002C84;
        if (tier == 0) {
            gLayeredCutsceneOamOffsetY = tier;
        } else if (tier == 1) {
            gLayeredCutsceneOamOffsetY = tier;
        } else if (tier == 2) {
            gLayeredCutsceneOamOffsetY = 3;
        } else {
            gLayeredCutsceneOamOffsetY = 7;
        }
    }

    gUnk_3002C68 = 0;
    gUnk_3002CA4 = 0;
    InitializeEndingCutsceneParticlePositions(0);
    gUnk_3002C5A = 0;
    gUnk_3002C5C = 16;

    WaitForVBlankInterrupt();
    InitCutsceneBrightenBlendFromWhite(22);
    REG_DISPCNT = 0x1600;
}

void SelectEndingCutsceneTreasureOam(s32 timer, u16** oamFrame)
{
    s32 sequenceTimer;
    u32 treasureTier;

    sequenceTimer = timer & 0x1FF;
    treasureTier = *(u16*)&gUnk_3002C84;
    if (treasureTier == 0) {
        SelectEndingCutsceneOamSequence32(sequenceTimer + 270, oamFrame);
    } else if (treasureTier == 1) {
        SelectEndingCutsceneOamSequence31(sequenceTimer + 318, oamFrame);
    } else if (treasureTier == 2) {
        SelectEndingCutsceneOamSequence29(sequenceTimer + 298, oamFrame);
    } else if (treasureTier == 3) {
        SelectEndingCutsceneOamSequence30(sequenceTimer + 296, oamFrame);
    }
}

void SelectEndingCutsceneTreasureFlashOam(s32 timer, u16** oamFrame)
{
    u32 treasureTier;

    treasureTier = *(u16*)&gUnk_3002C84;
    if (treasureTier == 0) {
        SelectEndingCutsceneOamSequence35(timer, oamFrame);
    } else if (treasureTier == 1) {
        SelectEndingCutsceneOamSequence36(timer, oamFrame);
    } else if (treasureTier == 2) {
        SelectEndingCutsceneOamSequence33(timer, oamFrame);
    } else if (treasureTier == 3) {
        SelectEndingCutsceneOamSequence34(timer, oamFrame);
    }
}

void SetCutsceneOamSemiTransparentPriority(const u16* oamData, u16* destination)
{
    if (oamData != 0) {
        u16 count = *oamData;
        u16* entry = destination - count * 4;

        while (count-- != 0) {
            *entry |= 0x400;
            entry += 2;
            *entry |= 0x400;
            entry += 2;
        }
    }
}

void func_800A43C(s32 inputFrame)
{
    s32 frame;
    u16* treasureOam;
    u16* mainOam;
    u16* specialOam;
    u16* particleOam;
    u16* pathOam;
    register u16* destination PIN("r6");
    s32 renderTreasurePaths;
    s16 pathX;
    s16 pathY;
    u32 timer;
    s32 pathFrame;

    frame = inputFrame;
    renderTreasurePaths = 0;
    treasureOam = 0;
    mainOam = 0;
    pathOam = 0;
    specialOam = 0;

    switch (gUnk_3002C60) {
        case 0:
            if (DecreaseCutsceneBlendCoefficient(7) != 0) {
                gUnk_3002C64++;
                if (gUnk_3002C64 == 121) {
                    *(vu16*)0x04000052 = (gUnk_3002C5C << 8) | gUnk_3002C5A;
                    *(vu16*)0x04000050 = 0x1641;
                    *(vu16*)0x04000000 = 0x1700;
                    gUnk_3002C60++;
                }
            }
            SelectEndingCutsceneTreasureOam(frame, (u16**)&treasureOam);
            gUnk_3002CA4 = 1;
            SelectEndingCutsceneOamSequence10(0, (u16**)&mainOam);
            break;

        case 1:
            if ((frame & 15) == 15) {
                if ((u16)gUnk_3002C5A <= 9) {
                    gUnk_3002C5A++;
                }
                if ((u16)gUnk_3002C5C > 10) {
                    gUnk_3002C5C--;
                }
                *(vu16*)0x04000052 = (gUnk_3002C5C << 8) | gUnk_3002C5A;
                if ((u16)gUnk_3002C5A == 2) {
                    m4aSongNumStartOrChange(151);
                }
                if ((u16)gUnk_3002C5A == 10) {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
            }
            SelectEndingCutsceneTreasureOam(frame, (u16**)&treasureOam);
            SelectEndingCutsceneOamSequence10(0, (u16**)&mainOam);
            break;

        case 2:
            SelectEndingCutsceneOamSequence10(0, (u16**)&mainOam);
            SelectEndingCutsceneTreasureOam(frame, (u16**)&treasureOam);
            ReadEndingCutsceneIndexedPathPair(
                gUnk_3002C84, gUnk_3002C64, (u16*)&gCutsceneAnimationFinished, (u16*)&gUnk_3002C9A
            );
            gUnk_3002C64++;
            if (gUnk_3002C64 == 232) {
                gUnk_3002C68 = 0;
                gUnk_3002C60++;
            }
            break;

        case 3: {
            register s32 renderOne PIN("r5");
            renderOne = 1;
            renderTreasurePaths = renderOne;
        }
            if ((u32)gUnk_3002C64 <= 358) {
                SelectEndingCutsceneTreasureOam(frame, (u16**)&treasureOam);
            } else {
                SelectEndingCutsceneTreasureFlashOam(gUnk_3002C64, (u16**)&treasureOam);
            }

            if (ReadEndingCutsceneIndexedPathPair(
                    gUnk_3002C84, gUnk_3002C64, (u16*)&gCutsceneAnimationFinished, (u16*)&gUnk_3002C9A
                ) != 0) {
                renderTreasurePaths = 0;
                gUnk_3002C60++;
            }

            if ((u32)gUnk_3002C64 <= 479) {
                SelectEndingCutsceneOamSequence10(gUnk_3002C64, (u16**)&mainOam);
            } else {
                register s32 frameOffset PIN("r1") = -480;
                SelectEndingCutsceneOamSequence16(gUnk_3002C64 + frameOffset, (u16**)&mainOam);
            }
            gUnk_3002C64++;
            gUnk_3002C68++;
            break;

        case 4: {
            register s32 frameCheck PIN("r2");
            s32 mask;
            mask = 15;
            frameCheck = frame;
            mask &= frameCheck;
            if (mask == 15) {
                u16* coeffAPtr = (u16*)&gUnk_3002C5A;
                u16 coeffA = *coeffAPtr;
                if (coeffA != 0) {
                    *coeffAPtr = coeffA - 1;
                }
                if ((u16)gUnk_3002C5C <= 15) {
                    gUnk_3002C5C++;
                }
                *(vu16*)0x04000052 = (gUnk_3002C5C << 8) | *coeffAPtr;
                if (*coeffAPtr == 0) {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                }
            }
            SelectEndingCutsceneOamSequence15(0, (u16**)&mainOam);
        } break;

        case 5:
            SelectEndingCutsceneOamSequence15(0, (u16**)&mainOam);
            gUnk_3002C64++;
            if (gUnk_3002C64 == 121) {
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;

        case 6:
            if ((u32)gUnk_3002C64 <= 64) {
                SelectEndingCutsceneOamSequence14(0, (u16**)&mainOam);
            } else if ((u32)gUnk_3002C64 <= 119) {
                SelectEndingCutsceneOamSequence11(gUnk_3002C64 - 65, (u16**)&mainOam);
                if (gUnk_3002C64 == 65) {
                    m4aSongNumStartOrChange(421);
                }
            } else if ((u32)gUnk_3002C64 <= 311) {
                SelectEndingCutsceneOamSequence12(gUnk_3002C64 - 120, (u16**)&mainOam);
            } else {
                if (gUnk_3002C64 == 312) {
                    m4aSongNumStartOrChange(422);
                }
                {
                    s32 selectorFrame;
                    register s32 selectorOffset PIN("r3");
                    selectorFrame = gUnk_3002C64;
                    selectorOffset = -312;
                    selectorFrame += selectorOffset;
                    if (SelectEndingCutsceneOamSequence13(selectorFrame, (u16**)&mainOam) != 0) {
                        gUnk_3002C64 = 0;
                        gUnk_3002C60++;
                        break;
                    }
                }
            }
            gUnk_3002C64++;
            break;

        case 7:
            if (gCutscenePrimaryObjectX <= 183) {
                SelectEndingCutsceneOamSequence42(gUnk_3002C64, (u16**)&mainOam);
                gCutscenePrimaryObjectX += 2;
            } else {
                SelectEndingCutsceneOamSequence43(gUnk_3002C64, (u16**)&mainOam);
                if ((frame & gLayeredCutsceneOamOffsetY) == gLayeredCutsceneOamOffsetY) {
                    if (gUnk_3002C9E <= gLayeredCutsceneOamOffsetX) {
                        gUnk_3002C64 = 0;
                        gUnk_3002C60++;
                        break;
                    }
                    gUnk_3002C9E--;
                }
            }
            gUnk_3002C64++;
            break;

        case 8:
            if (gEndingCutsceneCollectedTreasureCount == 0) {
                if (SelectEndingCutsceneOamSequence45(gUnk_3002C64, (u16**)&mainOam) != 0) {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                    break;
                }
            } else {
                if (gUnk_3002C64 == 0) {
                    gUnk_3002C9C = sUnk_878E770[gUnk_3002C84];
                    {
                        u16* destinationY = (u16*)&gUnk_3002C9E;
                        u16* tableY = (u16*)sUnk_878E778;
                        *destinationY = tableY[gUnk_3002C84];
                    }
                }
                SelectEndingCutsceneOamSequence49(gUnk_3002C64, (u16**)&specialOam);
                if (SelectEndingCutsceneOamSequence48(gUnk_3002C64, (u16**)&mainOam) != 0) {
                    gUnk_3002C64 = 0;
                    gUnk_3002C60++;
                    break;
                }
            }
            gUnk_3002C64++;
            break;

        case 9:
            if (gEndingCutsceneCollectedTreasureCount != 0) {
                SelectEndingCutsceneOamSequence46(gUnk_3002C64, (u16**)&mainOam);
                SelectEndingCutsceneOamSequence47(gUnk_3002C64, (u16**)&specialOam);
            } else {
                SelectEndingCutsceneOamSequence44(gUnk_3002C64, (u16**)&mainOam);
            }
            gCutscenePrimaryObjectX -= 2;
            gUnk_3002C9C -= 2;
            if (gCutscenePrimaryObjectX < -64) {
                InitCutsceneDarkenBlend(22);
                *(vu16*)0x04000000 = 0x1600;
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            gUnk_3002C64++;
            break;

        case 10:
            if (IncreaseCutsceneBlendCoefficient(7) != 0) {
                gSubGameMode++;
            }
            break;
    }

    destination = (u16*)gOamBuffer;
    if (gUnk_3002C60 <= 7 && gEndingCutsceneCollectedTreasureCount != 0) {
        SelectEndingCutsceneOamSequence25(gUnk_3002C64, (u16**)&specialOam);
        destination = AppendCutsceneOamTemplate(specialOam, destination, gUnk_3002C9C, gUnk_3002C9E);
        WriteCutsceneOamAffineMatrix(0, 0, *(s16*)&gEndingCutsceneTreasureScale, *(s16*)&gEndingCutsceneTreasureScale);
    }

    if ((u16)gUnk_3002C5A > 7) {
        s16* xPositions;
        s16* yPositions;
        xPositions = gCutsceneObjectXPositions;
        yPositions = gCutsceneObjectYPositions;
        UpdateEndingCutsceneParticlePositions();

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (0), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (9), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (18), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (27), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (36), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (45), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (54), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);

        do {
            if (SelectEndingCutsceneOamSequence27(frame + (63), &particleOam) != 0) {
                s16* randomPointer;
                s32 product;
                s32 sum;
                s32 remainderBase;
                randomPointer = &gUnk_3002CA6;
                *yPositions = -(*randomPointer * 109 + 1021) % 24;
                product = *randomPointer * 109;
                sum = product + 1021;
                remainderBase = sum;
                if (sum < 0)
                    remainderBase = product + 1084;
                *randomPointer = sum - ((remainderBase >> 6) << 6);
                {
                    register s32 value PIN("r0");
                    value = *randomPointer;
                    value += 24;
                    value <<= 8;
                    *xPositions = value;
                }
            }
            destination = AppendCutsceneOamTemplate(particleOam, destination,
                (u16)*xPositions++ >> 8, (u16)*yPositions++);
        } while (0);
    }

    {
        register s32 renderPathsCheck PIN("r5");
        u32* timerPointer;
        u16** pathOamAddress;
        renderPathsCheck = renderTreasurePaths;
        if (renderPathsCheck != 0) {
            timerPointer = &gUnk_3002C68;
            timer = *timerPointer;
            pathFrame = 0;
            if (timer > 115) {
                pathFrame = timer;
            }

            pathOamAddress = (u16**)&pathOam;
            SelectEndingCutsceneOamSequence38(pathFrame, pathOamAddress);
            {
                s32* readPointer;
                readPointer = (s32*)timerPointer;
                timer = *readPointer;
            }
            ReadEndingCutscenePath36(timer, (u16*)&pathX, (u16*)&pathY);
            destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);

            SelectEndingCutsceneOamSequence37(pathFrame + 7, pathOamAddress);
            {
                s32* readPointer;
                readPointer = (s32*)timerPointer;
                timer = *readPointer;
            }
            ReadEndingCutscenePath37(timer, (u16*)&pathX, (u16*)&pathY);
            destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);

            SelectEndingCutsceneOamSequence39(pathFrame, pathOamAddress);
            {
                s32* readPointer;
                readPointer = (s32*)timerPointer;
                timer = *readPointer;
            }
            ReadEndingCutscenePath38(timer, (u16*)&pathX, (u16*)&pathY);
            destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);

            SelectEndingCutsceneOamSequence40(pathFrame + 7, pathOamAddress);
            {
                s32* readPointer;
                readPointer = (s32*)timerPointer;
                timer = *readPointer;
            }
            ReadEndingCutscenePath39(timer, (u16*)&pathX, (u16*)&pathY);
            destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);

            if (pathFrame > 216) {
                pathFrame -= 216;

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath40(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath41(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath42(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath43(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath44(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath45(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath46(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);

                do {
                    SelectEndingCutsceneOamSequence26(pathFrame, pathOamAddress);
                    ReadEndingCutscenePath47(pathFrame, (u16*)&pathX, (u16*)&pathY);
                    destination = AppendCutsceneOamTemplate(pathOam, destination, pathX, pathY);
                    SetCutsceneOamSemiTransparentPriority(pathOam, destination);
                } while (0);
            }
        }
    }

    destination = AppendCutsceneOamTemplate(treasureOam, destination, gCutsceneAnimationFinished, gUnk_3002C9A);
    if (gUnk_3002CA4 != 0) {
        SetCutsceneOamSemiTransparentPriority(treasureOam, destination);
    }

    destination = AppendCutsceneOamTemplate(mainOam, destination, gCutscenePrimaryObjectX, gCutscenePrimaryObjectY);

    if (gUnk_3002C60 > 7 && gEndingCutsceneCollectedTreasureCount != 0) {
        destination = AppendCutsceneOamTemplate(specialOam, destination, gUnk_3002C9C, gUnk_3002C9E);
        WriteCutsceneOamAffineMatrix(0, 0, *(s16*)&gEndingCutsceneTreasureScale, *(s16*)&gEndingCutsceneTreasureScale);
    }
    FinalizeCutsceneOamBuffer(destination);
}

void InitializeTitleScreenLogoCutscene(void)
{
    vu32* dma;
    u32 fillValue;
    const u16* tileRuns;
    s32 variantValue;
    s32* variant;

    fillValue = 0x03FF03FF;
    dma = (vu32*)REG_ADDR_DMA3;
    dma[0] = (u32)&fillValue;
    dma[1] = 0x06008000;
    dma[2] = 0x85000600;
    dma[2];

    dma[0] = (u32)sUnk_82D9C78;
    dma[1] = PLTT;
    dma[2] = 0x80000050;
    dma[2];

    dma[0] = (u32)sUnk_82D830E;
    dma[1] = OBJ_PLTT;
    dma[2] = 0x80000040;
    dma[2];

    LZ77UnCompVram((const u32*)sUnk_82D9D18, (void*)VRAM);
    LZ77UnCompVram((const u32*)sUnk_82D8390, OBJ_VRAM0);
    DecodeCutsceneTileRuns(sUnk_82DC454, (u16*)0x06009000);

    if (gClearMessage == 0) {
        DecodeCutsceneTileRuns(sUnk_82DC66E, (u16*)0x06008800);
        variantValue = gUnk_3002C78;
        tileRuns = sUnk_82DC902;
        if (variantValue != 0) {
            tileRuns = sUnk_82DCBA4;
        }
    } else if (gClearMessage == 1) {
        DecodeCutsceneTileRuns(sUnk_82DC878, (u16*)0x06008800);
        variantValue = gUnk_3002C78;
        tileRuns = sUnk_82DCE4A;
        if (variantValue != 0) {
            tileRuns = sUnk_82DCFCC;
        }
    } else {
        variant = &gUnk_3002C78;
        variantValue = *variant;
        tileRuns = sUnk_82DC738;
        if (variantValue != 0) {
            tileRuns = sUnk_82DC7EA;
        }
        DecodeCutsceneTileRuns(tileRuns, (u16*)0x06008800);
        variantValue = *variant;
        tileRuns = sUnk_82DCD16;
        if (variantValue != 0) {
            tileRuns = sUnk_82DCDC8;
        }
    }

    DecodeCutsceneTileRuns(tileRuns, (u16*)0x06008000);
    REG_BG0CNT = 0x1000;
    REG_BG1CNT = 0x1101;
    REG_BG2CNT = 0x1202;
    gCutsceneAnimationFinished = 0;
    WaitForVBlankInterrupt();
    InitCutsceneDarkenBlendFromBlack(55);
    REG_DISPCNT = 0x1700;
}

void UpdateTitleScreenLogoCutscene(u16 frame)
{
    u8* oamFrame;

    switch (gUnk_3002C60) {
        case 0:
            if (DecreaseCutsceneBlendCoefficient(3)) {
                gUnk_3002C60++;
            }
            break;
        case 1:
            if (gButtonsPressed & 9) {
                m4aSongNumStartOrChange(294);
                gUnk_3002C64 = 0;
                gUnk_3002C60++;
            }
            break;
        case 2:
            gUnk_3002C60 = 3;
            break;
        case 3:
            if (IncreaseCutsceneBlendCoefficient(3)) {
                m4aMPlayVolumeControl(&gMPlayInfo_2, 0xFFFF, 0x100);
                gSubGameMode++;
            }
            break;
    }

    if (gUnk_3002C60 <= 1) {
        if (gTitleScreenStyle == 1) {
            SelectTitleScreenSHardLogoIntroOamFrame(gUnk_3002C64, &oamFrame);
        } else {
            SelectTitleScreenNormalLogoIntroOamFrame(gUnk_3002C64, &oamFrame);
        }
    } else {
        if (gTitleScreenStyle == 1) {
            gCutsceneAnimationFinished = SelectTitleScreenSHardLogoIdleOamFrame(gUnk_3002C64, &oamFrame);
        } else {
            gCutsceneAnimationFinished = SelectTitleScreenNormalLogoIdleOamFrame(gUnk_3002C64, &oamFrame);
        }
    }

    FinalizeCutsceneOamBuffer(AppendCutsceneOamTemplate((u16*)oamFrame, (u16*)gOamBuffer, 108, 160));
    gUnk_3002C64++;
}

void CountEndingTreasuresAndSelectTier(void)
{
    s32 count;

    count = gCollectedNEJewelPiece != 0;
    if (gCollectedSEJewelPiece != 0) {
        count++;
    }
    if (gCollectedSWJewelPiece != 0) {
        count++;
    }
    if (gCollectedNWJewelPiece != 0) {
        count++;
    }
    if (gHasBossTreasure5 != 0) {
        count++;
    }
    if (gHasBossTreasure6 != 0) {
        count++;
    }
    if (gHasBossTreasure7 != 0) {
        count++;
    }
    if (gHasBossTreasure8 != 0) {
        count++;
    }
    if (gHasBossTreasure9 != 0) {
        count++;
    }
    if (gHasBossTreasure10 != 0) {
        count++;
    }
    if (gHasBossTreasure11 != 0) {
        count++;
    }
    if (gHasBossTreasure12 != 0) {
        count++;
    }

    gEndingCutsceneCollectedTreasureCount = count;
    if (count <= 1) {
        gUnk_3002C84 = 0;
        gEndingCutsceneTreasureScale = 160;
    } else if (count <= 5) {
        gUnk_3002C84 = 1;
        gEndingCutsceneTreasureScale = 224;
    } else if (count <= 11) {
        gUnk_3002C84 = 2;
        gEndingCutsceneTreasureScale = 288;
    } else {
        gUnk_3002C84 = 3;
        gEndingCutsceneTreasureScale = 384;
    }
}

u16* BuildCutsceneBackgroundAffineMatrix(u16* destination, s32 angle, s32 scaleX, s32 scaleY, s32 centerX, s32 centerY)
{
    const s16* angleEntry;
    s16 cosine;
    s16 sine;
    u16 pa;
    u16 pb;
    u16 pc;
    u16 pd;
    s32 dx;
    s32 dy;

    angle &= 0xFF;
    cosine = sSinCosTable[angle + 64];
    pa = FixedMul(cosine, (s16)FixedInverse((s16)scaleX));

    angleEntry = &sSinCosTable[angle];
    sine = *angleEntry;
    pb = FixedMul(sine, (s16)FixedInverse((s16)scaleX));

    sine = -(u16)*angleEntry;
    pc = FixedMul(sine, (s16)FixedInverse((s16)scaleY));
    pd = FixedMul(cosine, (s16)FixedInverse((s16)scaleY));

    dx = (centerX << 8) - (s16)pa * centerX - (s16)pb * centerY;
    dy = (centerY << 8) - (s16)pc * centerX - (s16)pd * centerY;
    dx += gUnk_3002C42 << 8;
    dy += gUnk_3002C44 << 8;

    *destination++ = pa;
    *destination++ = pb;
    *destination++ = pc;
    *destination++ = pd;
    *destination++ = dx;
    *destination++ = dx >> 16;
    *destination++ = dy;
    *destination++ = dy >> 16;

    return destination;
}

void WriteCutsceneOamAffineMatrix(u32 matrixIndex, s32 angle, s32 scaleX, s32 scaleY)
{
    u16* destination;
    const s16* angleEntry;
    s16 cosine;
    s16 sine;

    destination = (u16*)((u8*)gOamBuffer + 6 + (matrixIndex << 5));
    angle &= 0xFF;
    cosine = sSinCosTable[angle + 64];
    *destination = FixedMul(cosine, (s16)FixedInverse((s16)scaleX));
    destination += 4;

    angleEntry = &sSinCosTable[angle];
    sine = *angleEntry;
    *destination = FixedMul(sine, (s16)FixedInverse((s16)scaleX));
    destination += 4;

    sine = -(u16)*angleEntry;
    *destination = FixedMul(sine, (s16)FixedInverse((s16)scaleY));
    destination += 4;

    *destination = FixedMul(cosine, (s16)FixedInverse((s16)scaleY));
    destination++;

    if ((OamData*)destination > GP(gUnk_3002C70)) {
        GP_SET(gUnk_3002C70, (OamData*)destination);
    }
}

void WaitForVBlankInterrupt(void)
{
    gInterruptCheck &= 0xFFFE;

    do {
        SYSCALL(SYSCALL_Halt);
    } while ((gInterruptCheck & 1) == 0);
}

void CopyCutsceneTilemapRect(const u16* source, u16* destination, u16 width, u16 height, u16 destinationStride)
{
    u32 row;
    u32 column;

    row = height;
    if (row != 0) {
        do {
            u16* rowDestination;

            rowDestination = destination;
            column = width;
            row--;
            if (column != 0) {
                do {
                    *rowDestination++ = *source++;
                    column--;
                } while (column != 0);
            }
            destination += destinationStride;
        } while (row != 0);
    }
}

u16* AppendCutsceneOamTemplate(const u16* source, u16* destination, s32 xOffset, s32 yOffset)
{
    u16 count;

    if (source != NULL) {
        count = *source;
        source++;
        count--;
        while (count != 0xFFFF) {
            *destination = (*source & 0xFF00) | ((*source + yOffset) & 0xFF);
            destination++;
            source++;

            *destination = (*source & 0xFE00) | ((*source + xOffset) & 0x1FF);
            destination++;
            source++;

            *destination = *source;
            source++;
            destination += 2;
            count--;
        }
        if (destination > (u16*)GP(gUnk_3002C70)) {
            GP_SET(gUnk_3002C70, (OamData*)destination);
        }
    }
    return destination;
}

void FinalizeCutsceneOamBuffer(u16* destination)
{
    while (destination < (u16*)GP(gUnk_3002C70)) {
        *destination = 0xD0;
        destination++;
        *destination = 0;
        destination += 3;
    }

    gUnk_3002C6C = (u32)GP(gUnk_3002C70) - (u32)gOamBuffer;
}

void ResetCutsceneBackgroundRegisters(void)
{
    vu16* registers;

    registers = (vu16*)REG_ADDR_BG0HOFS;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0x100;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0x100;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0x100;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0x100;
    *registers++ = 0;
    *registers++ = 0;
    *registers++ = 0;
    *registers = 0;
    registers += 7;
    *registers = 0;
}

void DecodeCutsceneTileRuns(const u16* source, u16* destination)
{
    u16 command;
    u16 count;
    u16 value;
    u16 repeat;
    u16 offset;
    u16* output;

    command = *source;
    while (command != 0) {
        offset = (((u32)command) << 17) >> 22;
        count = (command & 0x1F) + 1;
        output = destination + offset;
        repeat = command & 0x8000;
        source++;

        do {
            if (repeat != 0) {
                value = *source++;
                while (count-- != 0) {
                    *output++ = value;
                }
            } else {
                value = *source++;
                while (count-- != 0) {
                    *output++ = value++;
                }
            }
        } while (0);

        command = *source;
    }
}

void InitCutsceneDarkenBlend(u16 blendTargets)
{
    gUnk_3002C5E = 0;
    REG_BLDY = 0;
    REG_BLDCNT = blendTargets | 0xC0;
}

void InitCutsceneDarkenBlendFromBlack(u16 blendTargets)
{
    gUnk_3002C5E = 16;
    REG_BLDY = 16;
    REG_BLDCNT = blendTargets | 0xC0;
}

void InitCutsceneBrightenBlend(u16 blendTargets)
{
    gUnk_3002C5E = 0;
    REG_BLDY = 0;
    REG_BLDCNT = blendTargets | 0x80;
}

void InitCutsceneBrightenBlendFromWhite(u16 blendTargets)
{
    gUnk_3002C5E = 16;
    REG_BLDY = 16;
    REG_BLDCNT = blendTargets | 0x80;
}

s32 IncreaseCutsceneBlendCoefficient(u16 mask)
{
    u16* coefficient;
    u16 value;

    if ((gUnk_300188E & mask) == mask) {
        coefficient = (u16*)&gUnk_3002C5E;
        value = *coefficient;
        if (value <= 15) {
            value++;
            *coefficient = value;
        }
        REG_BLDY = *coefficient;
    }
    return *(u16*)&gUnk_3002C5E == 16;
}

s32 DecreaseCutsceneBlendCoefficient(u16 mask)
{
    u16* coefficient;
    u16 value;

    if ((gUnk_300188E & mask) == mask) {
        coefficient = (u16*)&gUnk_3002C5E;
        value = *coefficient;
        if (value != 0) {
            value--;
            *coefficient = value;
        }
        REG_BLDY = *coefficient;
    }
    return *(u16*)&gUnk_3002C5E == 0;
}

s32 SelectCutsceneAnimationSequence150A(s32 timer, const u16** frame)
{
    if (timer <= 15) {
        *frame = sUnk_8285C08;
    } else if (timer <= 23) {
        *frame = sUnk_8285C10;
    } else if (timer <= 31) {
        *frame = sUnk_8285C1E;
    } else if (timer <= 47) {
        *frame = sUnk_8285C32;
    } else if (timer <= 55) {
        *frame = sUnk_8285C4C;
    } else if (timer <= 63) {
        *frame = sUnk_8285C6C;
    } else if (timer <= 71) {
        *frame = sUnk_8285C92;
    } else if (timer <= 87) {
        *frame = sUnk_8285CBE;
    } else if (timer <= 95) {
        *frame = sUnk_8285CF0;
    } else if (timer <= 103) {
        *frame = sUnk_8285D28;
    } else if (timer <= 111) {
        *frame = sUnk_8285D66;
    } else if (timer <= 119) {
        *frame = sUnk_8285DAA;
    } else if (timer <= 127) {
        *frame = sUnk_8285DF4;
    } else if (timer <= 135) {
        *frame = sUnk_8285E44;
    } else if (timer <= 143) {
        *frame = sUnk_8285E9A;
    } else {
        *frame = sUnk_8285EF6;
    }

    if (timer == 0 || timer == 16 || timer == 24 || timer == 32 || timer == 48 || timer == 56 || timer == 64 ||
        timer == 72 || timer == 88 || timer == 96 || timer == 104 || timer == 112 || timer == 120) {
        m4aSongNumStartOrChange(0x1EA);
    }
    return timer > 150;
}

s32 SelectCutsceneAnimationSequence150B(s32 timer, const u16** frame)
{
    if (timer <= 18) {
        *frame = sUnk_8285F58;
    } else if (timer <= 37) {
        *frame = sUnk_8285F60;
    } else if (timer <= 56) {
        *frame = sUnk_8285F68;
    } else if (timer <= 75) {
        *frame = sUnk_8285F76;
    } else if (timer <= 94) {
        *frame = sUnk_8285F7E;
    } else if (timer <= 113) {
        *frame = sUnk_8285F8C;
    } else if (timer <= 132) {
        *frame = sUnk_8285F9A;
    } else {
        *frame = sUnk_8285FAE;
    }

    if (timer == 0 || timer == 19 || timer == 38 || timer == 57 || timer == 76) {
        m4aSongNumStartOrChange(0x1EA);
    }
    return timer > 150;
}

s32 SelectAlternatingCutsceneFrame(s32 timer, const u16** frame)
{
    s32 parity;

    parity = timer & 1;
    if (parity == 0) {
        *frame = sUnk_828B1C0;
    } else {
        *frame = 0;
    }
    return parity;
}

s32 SelectCutsceneAnimationLoop72(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 72;
    if (index <= 5) {
        *frame = sUnk_828E304;
    } else if (index <= 11) {
        *frame = sUnk_828E330;
    } else if (index <= 17) {
        *frame = sUnk_828E362;
    } else if (index <= 23) {
        *frame = sUnk_828E394;
    } else if (index <= 29) {
        *frame = sUnk_828E304;
    } else if (index <= 35) {
        *frame = sUnk_828E3F2;
    } else if (index <= 41) {
        *frame = sUnk_828E41E;
    } else if (index <= 47) {
        *frame = sUnk_828E394;
    } else if (index <= 53) {
        *frame = sUnk_828E304;
    } else if (index <= 59) {
        *frame = sUnk_828E330;
    } else if (index <= 65) {
        *frame = sUnk_828E362;
    } else {
        *frame = sUnk_828E394;
    }
    return index == 71;
}

s32 SelectCutsceneAnimationLoop40(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 40;
    if (index <= 19) {
        *frame = sUnk_828E476;
    } else {
        *frame = sUnk_828E4A2;
    }
    return index == 39;
}

s32 SelectCutsceneAnimationLoop24(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 24;
    if (index <= 3) {
        *frame = sUnk_828E4E0;
    } else if (index <= 7) {
        *frame = sUnk_828E506;
    } else if (index <= 11) {
        *frame = sUnk_828E4E0;
    } else if (index <= 15) {
        *frame = sUnk_828E506;
    } else if (index <= 19) {
        *frame = sUnk_828E4E0;
    } else {
        *frame = sUnk_828E506;
    }
    return index == 23;
}

s32 SelectCutsceneAnimationLoop52(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 52;
    if (index <= 3) {
        *frame = sUnk_828E52C;
    } else if (index <= 7) {
        *frame = sUnk_828E540;
    } else if (index <= 11) {
        *frame = sUnk_828E566;
    } else if (index <= 15) {
        *frame = sUnk_828E540;
    } else if (index <= 19) {
        *frame = sUnk_828E52C;
    } else if (index <= 23) {
        *frame = sUnk_828E540;
    } else if (index <= 27) {
        *frame = sUnk_828E566;
    } else if (index <= 31) {
        *frame = sUnk_828E540;
    } else {
        *frame = sUnk_828E52C;
    }
    return index == 51;
}

s32 SelectCutsceneAnimationLoop59(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 59;
    if (index <= 5) {
        *frame = sUnk_828E58C;
    } else if (index <= 11) {
        *frame = sUnk_828E5AC;
    } else if (index <= 17) {
        *frame = sUnk_828E5CC;
    } else if (index <= 20) {
        *frame = sUnk_828E58C;
    } else if (index <= 23) {
        *frame = sUnk_828E5AC;
    } else if (index <= 26) {
        *frame = sUnk_828E5CC;
    } else if (index <= 30) {
        *frame = sUnk_828E58C;
    } else if (index <= 34) {
        *frame = sUnk_828E5AC;
    } else if (index <= 38) {
        *frame = sUnk_828E5CC;
    } else {
        *frame = sUnk_828E58C;
    }
    return index == 58;
}

s32 SelectCutsceneAnimationLoop90(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 90;
    if (index <= 19) {
        *frame = sUnk_828E52C;
    } else if (index <= 29) {
        *frame = sUnk_828E5EC;
    } else if (index <= 39) {
        *frame = sUnk_828E71C;
    } else {
        *frame = sUnk_828E74E;
    }
    return index == 89;
}

s32 SelectCutsceneAnimationLoop63(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 63;
    if (index <= 0) {
        *frame = sUnk_828E61E;
    } else if (index <= 1) {
        *frame = sUnk_828E632;
    } else if (index <= 2) {
        *frame = sUnk_828E646;
    } else if (index <= 3) {
        *frame = sUnk_828E61E;
    } else if (index <= 4) {
        *frame = sUnk_828E632;
    } else if (index <= 5) {
        *frame = sUnk_828E646;
    } else if (index <= 8) {
        *frame = sUnk_828E61E;
    } else if (index <= 11) {
        *frame = sUnk_828E632;
    } else if (index <= 14) {
        *frame = sUnk_828E646;
    } else if (index <= 18) {
        *frame = sUnk_828E61E;
    } else if (index <= 22) {
        *frame = sUnk_828E632;
    } else if (index <= 26) {
        *frame = sUnk_828E646;
    } else if (index <= 30) {
        *frame = sUnk_828E61E;
    } else if (index <= 34) {
        *frame = sUnk_828E632;
    } else if (index <= 38) {
        *frame = sUnk_828E646;
    } else if (index <= 46) {
        *frame = sUnk_828E61E;
    } else if (index <= 54) {
        *frame = sUnk_828E632;
    } else {
        *frame = sUnk_828E646;
    }
    return index == 62;
}

s32 SelectCutsceneAnimationLoop60(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 60;
    if (index <= 29) {
        *frame = sUnk_828E660;
    } else if (index <= 37) {
        *frame = sUnk_828E66E;
    } else if (index <= 45) {
        *frame = sUnk_828E68E;
    } else if (index <= 53) {
        *frame = sUnk_828E6BA;
    } else {
        *frame = sUnk_828E6F8;
    }
    return index == 59;
}

s32 SelectCutsceneAnimationLoop55(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 55;
    if (index <= 9) {
        *frame = sUnk_828E6F8;
    } else if (index <= 18) {
        *frame = sUnk_828E6BA;
    } else if (index <= 26) {
        *frame = sUnk_828E68E;
    } else if (index <= 34) {
        *frame = sUnk_828E66E;
    } else {
        *frame = sUnk_828E660;
    }
    return index == 54;
}

s32 SelectCutsceneAnimationLoop8(s32 timer, const u16** frame)
{
    s32 index;

    index = timer % 8;
    if (index <= 3) {
        *frame = sUnk_828E70C;
    } else {
        *frame = sUnk_828E714;
    }
    return index == 7;
}

s32 SelectCutsceneOamAnimation01(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 9;

    if (frame <= 5) {
        *ppOamData = sUnk_829A7C0;
    } else {
        *ppOamData = sUnk_829A7E6;
    }

    return frame == 8;
}

s32 SelectCutsceneOamAnimation02(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 9;

    if (frame <= 5) {
        *ppOamData = sUnk_829A80C;
    } else {
        *ppOamData = sUnk_829A850;
    }

    return frame == 8;
}

s32 SelectCutsceneOamAnimation03(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 16;

    if (frame <= 3) {
        *ppOamData = sUnk_829A894;
    } else if (frame <= 7) {
        *ppOamData = sUnk_829A8A8;
    } else if (frame <= 11) {
        *ppOamData = sUnk_829A8BC;
    } else {
        *ppOamData = sUnk_829A8D0;
    }

    return frame == 15;
}

void SetCutsceneOamFrame01(u16** ppOamData)
{
    *ppOamData = sUnk_829A8E4;
}

void SetCutsceneOamFrame02(u16** ppOamData)
{
    *ppOamData = sUnk_829A8F8;
}

void SetCutsceneOamFrame03(u16** ppOamData)
{
    *ppOamData = sUnk_829A90C;
}

s32 SelectCutsceneOamAnimation04(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 7) {
        *ppOamData = sUnk_829A926;
    } else if (frame <= 15) {
        *ppOamData = sUnk_829A92E;
    } else {
        *ppOamData = sUnk_829A936;
    }

    return frame == 23;
}

s32 SelectCutsceneOamAnimation05(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 116;

    if (frame <= 7) {
        *ppOamData = sUnk_829A9A6;
    } else if (frame <= 15) {
        *ppOamData = sUnk_829A9AE;
    } else {
        *ppOamData = sUnk_829A9B6;
    }

    return frame == 115;
}

s32 SelectCutsceneOamAnimation06(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 5) {
        *ppOamData = sUnk_829A9BE;
    } else if (frame <= 11) {
        *ppOamData = sUnk_829A9D8;
    } else if (frame <= 17) {
        *ppOamData = sUnk_829A9F2;
    } else {
        *ppOamData = sUnk_829AA0C;
    }

    return frame == 23;
}

s32 SelectCutsceneOamAnimation07(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 9;

    if (frame <= 5) {
        *ppOamData = sUnk_829AA5E;
    } else {
        *ppOamData = sUnk_829AA6C;
    }

    return frame == 8;
}

s32 SelectCutsceneBlinkOamAnimation01(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 8;

    if (frame <= 1) {
        *ppOamData = sUnk_829AA7A;
    } else if (frame <= 3) {
        *ppOamData = 0;
    } else if (frame <= 5) {
        *ppOamData = sUnk_829AA94;
    } else {
        *ppOamData = 0;
    }

    return frame == 7;
}

s32 SelectCutsceneBlinkOamAnimation02(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 4;

    if (frame <= 0) {
        *ppOamData = sUnk_829AAAE;
    } else if (frame <= 1) {
        *ppOamData = 0;
    } else if (frame <= 2) {
        *ppOamData = sUnk_829AAE0;
    } else {
        *ppOamData = 0;
    }

    return frame == 3;
}

s32 SelectCutsceneOamAnimation08(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 42;

    if (frame <= 2) {
        *ppOamData = sUnk_829AB84;
    } else if (frame <= 5) {
        *ppOamData = sUnk_829ABB6;
    } else if (frame <= 9) {
        *ppOamData = sUnk_829AC00;
    } else if (frame <= 13) {
        *ppOamData = sUnk_829AC62;
    } else if (frame <= 18) {
        *ppOamData = sUnk_829ACC4;
    } else if (frame <= 23) {
        *ppOamData = sUnk_829AD0E;
    } else if (frame <= 29) {
        *ppOamData = sUnk_829AD58;
    } else if (frame <= 35) {
        *ppOamData = sUnk_829AD8A;
    } else {
        *ppOamData = sUnk_829ADA4;
    }

    return frame == 41;
}

void SetCutsceneOamFrame04(u16** ppOamData)
{
    *ppOamData = sUnk_829AB1E;
}

void SetCutsceneOamFrame05(u16** ppOamData)
{
    *ppOamData = sUnk_829AB2C;
}

void SetCutsceneOamFrame06(u16** ppOamData)
{
    *ppOamData = sUnk_829AB46;
}

void SetCutsceneOamFrame07(u16** ppOamData)
{
    *ppOamData = sUnk_829ADBE;
}

void SetCutsceneOamFrame08(u16** ppOamData)
{
    *ppOamData = sUnk_829AA56;
}

void SetCutsceneOamFrame09(u16** ppOamData)
{
    *ppOamData = sUnk_829A93E;
}

void SetCutsceneOamFrame10(u16** ppOamData)
{
    *ppOamData = sUnk_829A952;
}

void SetCutsceneOamFrame11(u16** ppOamData)
{
    *ppOamData = sUnk_829A966;
}

void SetCutsceneOamFrame12(u16** ppOamData)
{
    *ppOamData = sUnk_829A986;
}

void SetCutsceneOamFrame13(u16** ppOamData)
{
    *ppOamData = sUnk_829AA26;
}

void SetCutsceneOamFrame14(u16** ppOamData)
{
    *ppOamData = sUnk_829AA3A;
}

void SetCutsceneOamFrame15(u16** ppOamData)
{
    *ppOamData = sUnk_829AA48;
}

s32 SelectCutsceneOamAnimation09(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 7) {
        *ppOamData = sUnk_829ADEA;
    } else if (frame <= 15) {
        *ppOamData = sUnk_829ADF8;
    } else {
        *ppOamData = sUnk_829AE06;
    }

    return frame == 23;
}

s32 SelectCutsceneOamAnimation10(s32 timer, u16** ppOamData)
{
    s32 frame;

    frame = timer % 116;

    if (frame <= 7) {
        *ppOamData = sUnk_829AE14;
    } else if (frame <= 15) {
        *ppOamData = sUnk_829AE22;
    } else {
        *ppOamData = sUnk_829AE30;
    }

    return frame == 115;
}

void CutsceneLoadObjPalette(u32 paletteFrame)
{
    if (paletteFrame <= 15) {
        vu32* dma;
        u32 offset;
        u32 control;

        dma = (vu32*)0x040000D4;
        offset = paletteFrame << 5;

        dma[0] = (u32)(sUnk_829AE3E + offset);
        dma[1] = 0x05000200;
        control = 0x80000010;
        dma[2] = control;
        dma[2];

        dma[0] = (u32)(sUnk_829B03E + offset);
        dma[1] = 0x05000220;
        dma[2] = control;
        dma[2];
    }
}

s32 SelectCutsceneBgAnimation72Frame(s32 timer, u8** ppData)
{
    s32 frame;

    frame = timer % 72;

    if (frame <= 19) {
        *ppData = &sUnk_829F590;
    } else if (frame <= 23) {
        *ppData = &sUnk_829F5D4;
    } else if (frame <= 27) {
        *ppData = &sUnk_829F63C;
    } else if (frame <= 31) {
        *ppData = &sUnk_829F686;
    } else if (frame <= 37) {
        *ppData = &sUnk_829F6B8;
    } else if (frame <= 43) {
        *ppData = &sUnk_829F6E4;
    } else if (frame <= 47) {
        *ppData = &sUnk_829F738;
    } else if (frame <= 51) {
        *ppData = &sUnk_829F764;
    } else if (frame <= 55) {
        *ppData = &sUnk_829F790;
    } else if (frame <= 59) {
        *ppData = &sUnk_829F7E6;
    } else if (frame <= 63) {
        *ppData = &sUnk_829F85A;
    } else if (frame <= 67) {
        *ppData = &sUnk_829F8AA;
    } else {
        *ppData = &sUnk_829F8FA;
    }

    return frame == 71;
}

s32 SelectCutsceneBgAnimation28Frame(s32 timer, u8** ppData)
{
    s32 frame;

    frame = timer % 28;

    if (frame <= 7) {
        *ppData = &sUnk_829F4DC;
    } else {
        if (frame > 13) {
            if (frame <= 21) {
                *ppData = &sUnk_829F558;
                return frame == 27;
            }
        }
        *ppData = &sUnk_829F520;
    }

    return frame == 27;
}

s32 SelectCutsceneBgAnimation36Frame(s32 timer, u8** ppData)
{
    s32 frame;

    frame = timer % 36;

    if (frame <= 5) {
        *ppData = &sUnk_829F4D4;
    } else if (frame <= 11) {
        *ppData = &sUnk_829F710;
    } else if (frame <= 17) {
        *ppData = &sUnk_829F718;
    } else if (frame <= 23) {
        *ppData = &sUnk_829F720;
    } else if (frame <= 29) {
        *ppData = &sUnk_829F728;
    } else {
        *ppData = &sUnk_829F730;
    }

    return frame == 35;
}

s32 SelectCutsceneBgAnimation24Frame(s32 timer, u8** ppData)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 5) {
        *ppData = &sUnk_829F93E;
    } else if (frame <= 11) {
        *ppData = &sUnk_829F946;
    } else if (frame <= 17) {
        *ppData = &sUnk_829F94E;
    } else {
        *ppData = &sUnk_829F956;
    }

    return frame == 0;
}

void CutsceneLoadBgObjPalette(u32 paletteFrame)
{
    vu32* dma;
    s32 offset;

    if (paletteFrame <= 7) {
        dma = (vu32*)0x040000D4;
        offset = (paletteFrame * 15) * 2;

        dma[0] = (u32)(&sUnk_829FA86 + offset);
        dma[1] = 0x05000002;
        dma[2] = 0x8000000F;
        dma[2];

        dma[0] = (u32)(&sUnk_829FB76 + offset);
        dma[1] = 0x05000202;
        dma[2] = 0x8000000F;
        dma[2];

        offset = (paletteFrame * 7) * 4;
        dma[0] = (u32)(&sUnk_829FC66 + offset);
        dma[1] = 0x05000222;
        dma[2] = 0x8000000E;
        dma[2];
    }
}

s32 CutsceneWarioSelectEffectAAnimOam(s32 timer, u16** outOam)
{
    s32 frame;

    frame = timer % 329;

    if (frame <= 49) {
        *outOam = sCutsceneWarioEffectAFrame0Oam;
    } else if (frame <= 53) {
        *outOam = sCutsceneWarioEffectAFrame1Oam;
    } else if (frame <= 57) {
        *outOam = sCutsceneWarioEffectAFrame2Oam;
    } else if (frame <= 61) {
        *outOam = sCutsceneWarioEffectAFrame1Oam;
    } else if (frame <= 111) {
        *outOam = sCutsceneWarioEffectAFrame0Oam;
    } else if (frame <= 115) {
        *outOam = sCutsceneWarioEffectAFrame1Oam;
    } else if (frame <= 123) {
        *outOam = sCutsceneWarioEffectAFrame2Oam;
    } else if (frame <= 127) {
        *outOam = sCutsceneWarioEffectAFrame1Oam;
    } else if (frame <= 177) {
        *outOam = sCutsceneWarioEffectAFrame0Oam;
    } else if (frame <= 192) {
        *outOam = sCutsceneWarioEffectAFrame3Oam;
    } else if (frame <= 200) {
        *outOam = sCutsceneWarioEffectAFrame4Oam;
    } else if (frame <= 208) {
        *outOam = sCutsceneWarioEffectAFrame5Oam;
    } else if (frame <= 212) {
        *outOam = sCutsceneWarioEffectAFrame6Oam;
    } else if (frame <= 216) {
        *outOam = sCutsceneWarioEffectAFrame7Oam;
    } else if (frame <= 220) {
        *outOam = sCutsceneWarioEffectAFrame8Oam;
    } else if (frame <= 224) {
        *outOam = sCutsceneWarioEffectAFrame9Oam;
    } else if (frame <= 228) {
        *outOam = sCutsceneWarioEffectAFrame10Oam;
    } else {
        *outOam = sCutsceneWarioEffectAFrame11Oam;
    }

    return frame == 328;
}

s32 CutsceneWarioSelectEffectBAnimOam(s32 timer, u16** outOam)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 5) {
        *outOam = sCutsceneWarioEffectBFrame0Oam;
    } else if (frame <= 11) {
        *outOam = sCutsceneWarioEffectBFrame1Oam;
    } else if (frame <= 17) {
        *outOam = sCutsceneWarioEffectBFrame2Oam;
    } else {
        *outOam = sCutsceneWarioEffectBFrame3Oam;
    }

    return frame == 23;
}

s32 CutsceneWarioSelectEffectCAnimOam(s32 timer, u16** outOam)
{
    s32 frame;

    frame = timer % 74;

    if (frame <= 49) {
        *outOam = sCutsceneWarioEffectCFrame0Oam;
    } else if (frame <= 55) {
        *outOam = sCutsceneWarioEffectCFrame1Oam;
    } else if (frame <= 61) {
        *outOam = sCutsceneWarioEffectCFrame2Oam;
    } else if (frame <= 67) {
        *outOam = sCutsceneWarioEffectCFrame3Oam;
    } else {
        *outOam = sCutsceneWarioEffectCFrame4Oam;
    }

    return frame == 73;
}

s32 CutsceneWarioSelectEffectDAnimOam(s32 timer, u16** outOam)
{
    s32 frame;

    frame = timer % 30;

    if (frame <= 5) {
        *outOam = sCutsceneWarioEffectDFrame0Oam;
    } else if (frame <= 11) {
        *outOam = sCutsceneWarioEffectDFrame1Oam;
    } else if (frame <= 17) {
        *outOam = sCutsceneWarioEffectDFrame2Oam;
    } else if (frame <= 23) {
        *outOam = sCutsceneWarioEffectDFrame3Oam;
    } else {
        *outOam = sCutsceneWarioEffectDFrame4Oam;
    }

    return frame == 29;
}

s32 CutsceneWarioSelectEffectEAnimOam(s32 timer, u16** outOam)
{
    s32 frame;

    frame = timer % 22;

    if (frame <= 3) {
        *outOam = sCutsceneWarioEffectEFrame0Oam;
    } else if (frame <= 7) {
        *outOam = sCutsceneWarioEffectEFrame1Oam;
    } else if (frame <= 11) {
        *outOam = sCutsceneWarioEffectEFrame2Oam;
    } else if (frame <= 15) {
        *outOam = sCutsceneWarioEffectEFrame3Oam;
    } else {
        *outOam = 0;
    }

    return frame == 0;
}

s32 CutsceneWarioSelectEffectFAnimOam(s32 timer, u16** outOam)
{
    s32 frame;

    frame = timer % 22;

    if (frame <= 3) {
        *outOam = sCutsceneWarioEffectFFrame0Oam;
    } else if (frame <= 7) {
        *outOam = sCutsceneWarioEffectFFrame1Oam;
    } else if (frame <= 11) {
        *outOam = sCutsceneWarioEffectFFrame2Oam;
    } else if (frame <= 15) {
        *outOam = sCutsceneWarioEffectFFrame3Oam;
    } else {
        *outOam = 0;
    }

    return frame == 0;
}

void CutsceneWarioGetStaticOam(u16** outOam)
{
    *outOam = sCutsceneWarioStaticOam;
}

void CutsceneWarioSetPose(u8 reaction, u8 pose, u16 direction)
{
    gWarioData.reaction = reaction;
    gWarioData.pose = pose;
    gWarioData.horizontalDirection = direction;
    gWarioData.unk_1F = 0;
    gWarioData.unk_1E = 0;
}

u16* CutsceneWarioDrawPoseOam(s32 xOffset, s32 yOffset, u16 priority, u16 heldKeys, u16 pressedKeys)
{
    u16 oldPressed;
    u16 oldHeld;
    u16 oamCount;
    u16* oamSrc;
    u16* oamDst;
    u16 attr1;
    OamData* oamSlot;
    s32 remainingSprites;
    u16 attr0;
    s32 isFacingRight;
    s32 direction;
    s32 xMask;
    s32 xKeepMask;
    unsigned char priorityBits;
    s32 priorityMask;
    u16 priorityCopy;

    priorityCopy = priority;
    oldPressed = gButtonsPressed;
    oldHeld = gButtonsHeld;

    gButtonsPressed = pressedKeys;
    gButtonsHeld = heldKeys;
    GF(sWarioPoseHandlerTable[gWarioData.reaction])();
    gButtonsPressed = oldPressed;
    gButtonsHeld = oldHeld;

    isFacingRight = gWarioData.horizontalDirection & 0x10;
    direction = 1;
    if (isFacingRight) {
        direction = 0;
    }
    GF(sUnk_82DED30[gWarioData.reaction])(direction);

    gOamSlotsUsed = 0;
    oamDst = (u16*)gOamBuffer;
    oamSrc = (u16*)GP(gWarioData.pOamData);
    oamCount = oldHeld = *(oamSrc++);

    isFacingRight = 0;
    if (oldHeld != isFacingRight) {
        xMask = 0x1FF;
        xKeepMask = 0xFFFFFE00;
        priorityBits = ((priorityCopy & 3) << 1) << 1;
        oamSlot = (OamData*)oamDst;
        priorityMask = -13;
        remainingSprites = oamCount;

        do {
            attr0 = *(oamSrc++);
            *(oamDst++) = attr0;
            ((u8*)oamSlot)[0] = attr0 + yOffset;

            attr1 = *(oamSrc++);
            *(oamDst++) = attr1;
            {
                s32 newAttr1;
                newAttr1 = (attr1 + xOffset) & xMask;
                oamSlot->all.attr1 = (xKeepMask & oamSlot->all.attr1) | newAttr1;
            }

            *oamDst = *oamSrc;
            oamSrc++;
            ((u8*)oamSlot)[5] = (((u8*)oamSlot)[5] & priorityMask) | priorityBits;
            oamDst += 2;
            oamSlot++;
            remainingSprites--;
        } while (remainingSprites != 0);
    }

    gOamSlotsUsed = oldHeld;
    gWarioData.unk_1E++;
    return oamDst;
}

void CutsceneWarioLoadObjTiles(void)
{
    vu32* dma;
    u32 control;

    if (gWarioData.objData1Size != 0) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(gWarioData.pObjData1);
        dma[1] = 0x06010000;
        control = (gWarioData.objData1Size >> 1) | 0x80000000;
        dma[2] = control;
        (void)dma[2];
    }

    if (gWarioData.objData2Size != 0) {
        dma = (vu32*)0x040000D4;
        dma[0] = (u32)GP(gWarioData.pObjData2);
        dma[1] = 0x06010400;
        control = (gWarioData.objData2Size >> 1) | 0x80000000;
        dma[2] = control;
        (void)dma[2];
    }
}

void CutsceneWarioLoadObjPalette(void)
{
    vu32* dma;

    dma = (vu32*)0x040000D4;

    dma[0] = (u32)sWarioDefaultObjPalette;
    dma[1] = 0x05000200;
    dma[2] = 0x80000010;
    (void)dma[2];
}

s32 SelectLayeredCutsceneOamSequence00(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 32;
    if (frame == 0 || frame == 15) {
        m4aSongNumStartOrChange(1);
    }
    if (frame <= 2) {
        *oamFrame = &sLayeredCutsceneOamFrame000;
    } else if (frame <= 6) {
        *oamFrame = &sLayeredCutsceneOamFrame001;
    } else if (frame <= 10) {
        *oamFrame = &sLayeredCutsceneOamFrame002;
    } else if (frame <= 13) {
        *oamFrame = &sLayeredCutsceneOamFrame003;
    } else if (frame <= 16) {
        *oamFrame = &sLayeredCutsceneOamFrame007;
    } else if (frame <= 20) {
        *oamFrame = &sLayeredCutsceneOamFrame004;
    } else if (frame <= 24) {
        *oamFrame = &sLayeredCutsceneOamFrame005;
    } else if (frame <= 28) {
        *oamFrame = &sLayeredCutsceneOamFrame006;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame007;
    }

    return frame == 31;
}

s32 SelectLayeredCutsceneOamSequence01(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 32;
    if (frame <= 2) {
        *oamFrame = &sLayeredCutsceneOamFrame021;
    } else if (frame <= 6) {
        *oamFrame = &sLayeredCutsceneOamFrame022;
    } else if (frame <= 10) {
        *oamFrame = &sLayeredCutsceneOamFrame023;
    } else if (frame <= 13) {
        *oamFrame = &sLayeredCutsceneOamFrame024;
    } else if (frame <= 16) {
        *oamFrame = &sLayeredCutsceneOamFrame028;
    } else if (frame <= 20) {
        *oamFrame = &sLayeredCutsceneOamFrame025;
    } else if (frame <= 24) {
        *oamFrame = &sLayeredCutsceneOamFrame026;
    } else if (frame <= 28) {
        *oamFrame = &sLayeredCutsceneOamFrame027;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame028;
    }

    return frame == 31;
}

s32 SelectLayeredCutsceneOamSequence02(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 32;
    if (frame == 0 || frame == 15) {
        m4aSongNumStartOrChange(5);
    }
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame008;
    } else if (frame <= 11) {
        *oamFrame = &sLayeredCutsceneOamFrame009;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame010;
    } else if (frame <= 23) {
        *oamFrame = &sLayeredCutsceneOamFrame011;
    } else if (frame <= 27) {
        *oamFrame = &sLayeredCutsceneOamFrame012;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame013;
    }

    return frame == 31;
}

s32 SelectLayeredCutsceneOamSequence03(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 32;
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame032;
    } else if (frame <= 11) {
        *oamFrame = &sLayeredCutsceneOamFrame033;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame034;
    } else if (frame <= 23) {
        *oamFrame = &sLayeredCutsceneOamFrame032;
    } else if (frame <= 27) {
        *oamFrame = &sLayeredCutsceneOamFrame033;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame034;
    }

    return frame == 31;
}

s32 SelectLayeredCutsceneOamSequence04(s32 timer, u8** oamFrame)
{
    *oamFrame = &sLayeredCutsceneOamFrame017;
    return timer == 3;
}

s32 SelectLayeredCutsceneOamSequence05(s32 timer, u8** oamFrame)
{
    *oamFrame = &sLayeredCutsceneOamFrame035;
    return timer == 3;
}

s32 SelectLayeredCutsceneOamSequence06(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 32;
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame018;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame020;
    } else if (frame <= 23) {
        *oamFrame = &sLayeredCutsceneOamFrame019;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame020;
    }

    return frame == 31;
}

s32 SelectLayeredCutsceneOamSequence07(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 32;
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame036;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame037;
    } else if (frame <= 23) {
        *oamFrame = &sLayeredCutsceneOamFrame038;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame037;
    }

    return frame == 31;
}

s32 SelectLayeredCutsceneOamSequence08(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 18;
    if (frame <= 5) {
        *oamFrame = &sLayeredCutsceneOamFrame014;
    } else if (frame <= 11) {
        *oamFrame = &sLayeredCutsceneOamFrame015;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame016;
    }

    return frame == 17;
}

s32 SelectLayeredCutsceneOamSequence09(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 18;
    if (frame <= 5) {
        *oamFrame = &sLayeredCutsceneOamFrame029;
    } else if (frame <= 11) {
        *oamFrame = &sLayeredCutsceneOamFrame030;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame031;
    }

    return frame == 17;
}

s32 SelectLayeredCutsceneOamSequence10(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 32;
    if (frame == 0 || frame == 15) {
        m4aSongNumStartOrChange(1);
    }
    if (frame <= 2) {
        *oamFrame = &sLayeredCutsceneOamFrame039;
    } else if (frame <= 6) {
        *oamFrame = &sLayeredCutsceneOamFrame040;
    } else if (frame <= 10) {
        *oamFrame = &sLayeredCutsceneOamFrame041;
    } else if (frame <= 13) {
        *oamFrame = &sLayeredCutsceneOamFrame042;
    } else if (frame <= 16) {
        *oamFrame = &sLayeredCutsceneOamFrame046;
    } else if (frame <= 20) {
        *oamFrame = &sLayeredCutsceneOamFrame043;
    } else if (frame <= 24) {
        *oamFrame = &sLayeredCutsceneOamFrame044;
    } else if (frame <= 28) {
        *oamFrame = &sLayeredCutsceneOamFrame045;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame046;
    }

    return frame == 31;
}

s32 SelectLayeredCutsceneOamSequence11(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 24;
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame050;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame051;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame052;
    }

    return frame == 23;
}

s32 SelectLayeredCutsceneOamSequence12(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 18;
    if (frame <= 5) {
        *oamFrame = &sLayeredCutsceneOamFrame047;
    } else if (frame <= 11) {
        *oamFrame = &sLayeredCutsceneOamFrame048;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame049;
    }

    return frame == 17;
}

s32 SelectLayeredCutsceneOamSequence13(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 14;
    if (frame <= 5) {
        *oamFrame = &sLayeredCutsceneOamFrame053;
    } else if (frame <= 9) {
        *oamFrame = &sLayeredCutsceneOamFrame054;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame055;
    }

    return frame == 13;
}

s32 SelectLayeredCutsceneOamSequence14(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 148;
    if (frame <= 19) {
        *oamFrame = &sLayeredCutsceneOamFrame054;
    } else if (frame <= 23) {
        *oamFrame = &sLayeredCutsceneOamFrame059;
    } else if (frame <= 27) {
        *oamFrame = &sLayeredCutsceneOamFrame058;
    } else if (frame <= 47) {
        *oamFrame = &sLayeredCutsceneOamFrame056;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame057;
    }

    return frame == 147;
}

s32 SelectLayeredCutsceneOamSequence15(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 12;
    if (frame <= 3) {
        *oamFrame = &sLayeredCutsceneOamFrame057;
    } else if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame058;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame059;
    }

    return frame == 11;
}

s32 SelectLayeredCutsceneOamSequence16(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 24;
    if (frame == 0 || frame == 12) {
        m4aSongNumStartOrChange(1);
    }
    if (frame <= 3) {
        *oamFrame = &sLayeredCutsceneOamFrame060;
    } else if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame061;
    } else if (frame <= 11) {
        *oamFrame = &sLayeredCutsceneOamFrame062;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame063;
    } else if (frame <= 19) {
        *oamFrame = &sLayeredCutsceneOamFrame064;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame065;
    }

    return frame == 23;
}

s32 SelectLayeredCutsceneOamSequence17(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 16;
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame066;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame067;
    }

    return frame == 15;
}

s32 SelectLayeredCutsceneOamSequence18(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 24;
    if (frame == 0 || frame == 12) {
        m4aSongNumStartOrChange(1);
    }
    if (frame <= 3) {
        *oamFrame = &sLayeredCutsceneOamFrame068;
    } else if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame069;
    } else if (frame <= 11) {
        *oamFrame = &sLayeredCutsceneOamFrame070;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame071;
    } else if (frame <= 19) {
        *oamFrame = &sLayeredCutsceneOamFrame072;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame073;
    }

    return frame == 23;
}

s32 SelectLayeredCutsceneOamSequence19(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 16;
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame074;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame075;
    }

    return frame == 15;
}

s32 SelectLayeredCutsceneOamSequence20(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 136;
    if (frame <= 7) {
        *oamFrame = &sLayeredCutsceneOamFrame076;
    } else if (frame <= 15) {
        *oamFrame = &sLayeredCutsceneOamFrame077;
    } else if (frame <= 23) {
        *oamFrame = &sLayeredCutsceneOamFrame090;
    } else if (frame <= 31) {
        *oamFrame = &sLayeredCutsceneOamFrame078;
    } else if (frame <= 39) {
        *oamFrame = &sLayeredCutsceneOamFrame079;
    } else if (frame <= 47) {
        *oamFrame = &sLayeredCutsceneOamFrame090;
    } else if (frame <= 55) {
        *oamFrame = &sLayeredCutsceneOamFrame080;
    } else if (frame <= 63) {
        *oamFrame = &sLayeredCutsceneOamFrame081;
    } else if (frame <= 71) {
        *oamFrame = &sLayeredCutsceneOamFrame082;
    } else if (frame <= 79) {
        *oamFrame = &sLayeredCutsceneOamFrame083;
    } else if (frame <= 87) {
        *oamFrame = &sLayeredCutsceneOamFrame084;
    } else if (frame <= 95) {
        *oamFrame = &sLayeredCutsceneOamFrame085;
    } else if (frame <= 103) {
        *oamFrame = &sLayeredCutsceneOamFrame086;
    } else if (frame <= 111) {
        *oamFrame = &sLayeredCutsceneOamFrame087;
    } else if (frame <= 119) {
        *oamFrame = &sLayeredCutsceneOamFrame088;
    } else if (frame <= 127) {
        *oamFrame = &sLayeredCutsceneOamFrame090;
    } else {
        *oamFrame = &sLayeredCutsceneOamFrame089;
    }

    return frame == 135;
}

void SelectLayeredCutsceneOamSequence21(s32 timer, u8** oamFrame)
{
    register u8* frameOam PIN("r2");

    frameOam = &sLayeredCutsceneOamFrame091;
    *oamFrame = frameOam;
}

void SelectLayeredCutsceneOamSequence22(s32 timer, u8** oamFrame)
{
    register u8* frameOam PIN("r2");

    frameOam = &sLayeredCutsceneOamFrame092;
    *oamFrame = frameOam;
}

void SelectLayeredCutsceneOamSequence23(s32 timer, u8** oamFrame)
{
    register u8* frameOam PIN("r2");

    frameOam = &sLayeredCutsceneOamFrame093;
    *oamFrame = frameOam;
}

void SelectLayeredCutsceneOamSequence24(s32 timer, u8** oamFrame)
{
    register u8* frameOam PIN("r2");

    frameOam = &sLayeredCutsceneOamFrame094;
    *oamFrame = frameOam;
}

s32 SelectCutsceneGfx1(s32 timer, u8** outGfx)
{
    s32 frameTimer;
    frameTimer = timer % 24;

    if (frameTimer <= 5) {
        *outGfx = GBA_AS(*outGfx, &sCutsceneGfx1_Frame0);
    } else if (frameTimer <= 11) {
        *outGfx = &sCutsceneGfx1_Frame1;
    } else if (frameTimer <= 17) {
        *outGfx = &sCutsceneGfx1_Frame2;
    } else {
        *outGfx = &sCutsceneGfx1_Frame3;
    }

    return frameTimer == 23;
}

s32 SelectCutsceneGfx2(s32 timer, u8** outGfx)
{
    s32 frameTimer;
    frameTimer = timer % 42;

    if (frameTimer <= 5) {
        *outGfx = &sCutsceneGfx2_Frame0;
    } else if (frameTimer <= 11) {
        *outGfx = &sCutsceneGfx2_Frame1;
    } else if (frameTimer <= 17) {
        *outGfx = &sCutsceneGfx2_Frame2;
    } else if (frameTimer <= 23) {
        *outGfx = &sCutsceneGfx2_Frame3;
    } else if (frameTimer <= 29) {
        *outGfx = &sCutsceneGfx2_Frame4;
    } else if (frameTimer <= 35) {
        *outGfx = &sCutsceneGfx2_Frame5;
    } else {
        *outGfx = &sCutsceneGfx2_Frame6;
    }

    return frameTimer == 41;
}

s32 SelectCutsceneGfx3(s32 timer, u8** outGfx)
{
    s32 frameTimer;
    frameTimer = timer % 24;

    if (frameTimer <= 5) {
        *outGfx = &sCutsceneGfx3_Frame0;
    } else if (frameTimer <= 11) {
        *outGfx = &sCutsceneGfx3_Frame1;
    } else if (frameTimer <= 17) {
        *outGfx = &sCutsceneGfx3_Frame2;
    } else {
        *outGfx = &sCutsceneGfx3_Frame3;
    }

    return frameTimer == 23;
}

s32 SelectCutsceneGfx4(s32 timer, u8** outGfx)
{
    s32 frameTimer;
    frameTimer = timer % 48;

    if (frameTimer <= 5) {
        *outGfx = &sCutsceneGfx4_Frame0;
    } else if (frameTimer <= 11) {
        *outGfx = &sCutsceneGfx4_Frame1;
    } else if (frameTimer <= 17) {
        *outGfx = &sCutsceneGfx4_Frame2;
    } else if (frameTimer <= 23) {
        *outGfx = &sCutsceneGfx4_Frame3;
    } else if (frameTimer <= 29) {
        *outGfx = &sCutsceneGfx4_Frame4;
    } else if (frameTimer <= 35) {
        *outGfx = &sCutsceneGfx4_Frame5;
    } else if (frameTimer <= 41) {
        *outGfx = &sCutsceneGfx4_Frame6;
    } else {
        *outGfx = &sCutsceneGfx4_Frame7;
    }

    return frameTimer == 47;
}

s32 SelectCutsceneGfx5(s32 timer, u8** outGfx)
{
    s32 frameTimer;
    frameTimer = timer % 48;

    if (frameTimer <= 5) {
        *outGfx = &sCutsceneGfx5_Frame0;
    } else if (frameTimer <= 11) {
        *outGfx = &sCutsceneGfx5_Frame1;
    } else if (frameTimer <= 17) {
        *outGfx = &sCutsceneGfx5_Frame2;
    } else if (frameTimer <= 23) {
        *outGfx = &sCutsceneGfx5_Frame3;
    } else if (frameTimer <= 29) {
        *outGfx = &sCutsceneGfx5_Frame4;
    } else if (frameTimer <= 35) {
        *outGfx = &sCutsceneGfx5_Frame5;
    } else if (frameTimer <= 41) {
        *outGfx = &sCutsceneGfx5_Frame6;
    } else {
        *outGfx = &sCutsceneGfx5_Frame7;
    }

    return frameTimer == 47;
}

s32 SelectEndingCutsceneOamSequence00(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame000;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame001;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame002;
    } else {
        *oamFrame = sEndingCutsceneOamFrame001;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence01(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame003;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame004;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame005;
    } else {
        *oamFrame = sEndingCutsceneOamFrame004;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence02(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame022;

    return timer > 98;
}

s32 SelectEndingCutsceneOamSequence03(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame021;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame022;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame023;
    } else {
        *oamFrame = sEndingCutsceneOamFrame022;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence04(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 20;

    if (frame <= 9) {
        *oamFrame = sEndingCutsceneOamFrame006;
    } else {
        *oamFrame = sEndingCutsceneOamFrame007;
    }

    return frame == 19;
}

s32 SelectEndingCutsceneOamSequence05(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 184;

    if (frame <= 99) {
        *oamFrame = sEndingCutsceneOamFrame010;
    } else if (frame <= 103) {
        *oamFrame = sEndingCutsceneOamFrame008;
    } else if (frame <= 107) {
        *oamFrame = sEndingCutsceneOamFrame009;
    } else if (frame <= 111) {
        *oamFrame = sEndingCutsceneOamFrame008;
    } else if (frame <= 121) {
        *oamFrame = sEndingCutsceneOamFrame010;
    } else if (frame <= 125) {
        *oamFrame = sEndingCutsceneOamFrame008;
    } else if (frame <= 129) {
        *oamFrame = sEndingCutsceneOamFrame009;
    } else if (frame <= 133) {
        *oamFrame = sEndingCutsceneOamFrame008;
    } else {
        *oamFrame = sEndingCutsceneOamFrame010;
    }

    return frame == 183;
}

s32 SelectEndingCutsceneOamSequence06(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame011;

    return timer > 98;
}

s32 SelectEndingCutsceneOamSequence07(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame012;

    return timer > 98;
}

s32 SelectEndingCutsceneOamSequence08(s32 timer, u16** oamFrame)
{
    if (timer <= 7) {
        *oamFrame = sEndingCutsceneOamFrame013;
    } else if (timer <= 15) {
        *oamFrame = sEndingCutsceneOamFrame014;
    } else if (timer <= 23) {
        *oamFrame = sEndingCutsceneOamFrame015;
    } else if (timer <= 43) {
        *oamFrame = sEndingCutsceneOamFrame016;
    } else if (timer <= 47) {
        *oamFrame = sEndingCutsceneOamFrame017;
    } else if (timer <= 51) {
        *oamFrame = sEndingCutsceneOamFrame018;
    } else if (timer <= 59) {
        *oamFrame = sEndingCutsceneOamFrame016;
    } else if (timer <= 65) {
        *oamFrame = sEndingCutsceneOamFrame017;
    } else if (timer <= 71) {
        *oamFrame = sEndingCutsceneOamFrame018;
    } else {
        *oamFrame = sEndingCutsceneOamFrame016;
    }

    return timer > 170;
}

s32 SelectEndingCutsceneOamSequence09(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 16;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame019;
    } else {
        *oamFrame = sEndingCutsceneOamFrame020;
    }

    return frame == 15;
}

s32 SelectEndingCutsceneOamSequence10(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame016;

    return timer > 2;
}

s32 SelectEndingCutsceneOamSequence11(s32 timer, u16** oamFrame)
{
    if (timer <= 7) {
        *oamFrame = sEndingCutsceneOamFrame024;
    } else if (timer <= 15) {
        *oamFrame = sEndingCutsceneOamFrame025;
    } else if (timer <= 23) {
        *oamFrame = sEndingCutsceneOamFrame026;
    } else if (timer <= 31) {
        *oamFrame = sEndingCutsceneOamFrame027;
    } else {
        *oamFrame = sEndingCutsceneOamFrame028;
    }

    return timer > 130;
}

s32 SelectEndingCutsceneOamSequence12(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame028;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame029;
    } else {
        *oamFrame = sEndingCutsceneOamFrame030;
    }

    return frame == 23;
}

s32 SelectEndingCutsceneOamSequence13(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 48;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame028;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame029;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame030;
    } else if (frame <= 31) {
        *oamFrame = sEndingCutsceneOamFrame031;
    } else if (frame <= 39) {
        *oamFrame = sEndingCutsceneOamFrame032;
    } else {
        *oamFrame = sEndingCutsceneOamFrame033;
    }

    return frame == 47;
}

s32 SelectEndingCutsceneOamSequence14(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame024;

    return timer > 2;
}

s32 SelectEndingCutsceneOamSequence15(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame035;

    return timer > 2;
}

s32 SelectEndingCutsceneOamSequence16(s32 timer, u16** oamFrame)
{
    if (timer <= 44) {
        *oamFrame = sEndingCutsceneOamFrame024;
    } else if (timer <= 106) {
        *oamFrame = sEndingCutsceneOamFrame034;
    } else {
        *oamFrame = sEndingCutsceneOamFrame035;
    }

    return timer == 126;
}

s32 SelectEndingCutsceneOamSequence17(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 3) {
        *oamFrame = sEndingCutsceneOamFrame033;
    } else if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame032;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame031;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame030;
    } else if (frame <= 19) {
        *oamFrame = sEndingCutsceneOamFrame029;
    } else {
        *oamFrame = sEndingCutsceneOamFrame028;
    }

    return frame == 23;
}

s32 SelectEndingCutsceneOamSequence18(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 30;

    if (frame <= 5) {
        *oamFrame = sEndingCutsceneOamFrame036;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame037;
    } else if (frame <= 17) {
        *oamFrame = sEndingCutsceneOamFrame038;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame039;
    } else {
        *oamFrame = 0;
    }

    return frame == 29;
}

s32 SelectEndingCutsceneOamSequence19(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 30;

    if (frame <= 5) {
        *oamFrame = sEndingCutsceneOamFrame040;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame041;
    } else if (frame <= 17) {
        *oamFrame = sEndingCutsceneOamFrame042;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame043;
    } else {
        *oamFrame = 0;
    }

    return frame == 29;
}

s32 SelectEndingCutsceneOamSequence20(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 30;

    if (frame <= 5) {
        *oamFrame = sEndingCutsceneOamFrame044;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame045;
    } else if (frame <= 17) {
        *oamFrame = sEndingCutsceneOamFrame046;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame047;
    } else {
        *oamFrame = 0;
    }

    return frame == 29;
}

s32 SelectEndingCutsceneOamSequence21(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame048;

    return timer == 3;
}

s32 SelectEndingCutsceneOamSequence22(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame049;

    return timer == 3;
}

s32 SelectEndingCutsceneOamSequence23(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame050;

    return timer == 3;
}

s32 SelectEndingCutsceneOamSequence24(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame051;

    return timer == 3;
}

s32 SelectEndingCutsceneOamSequence25(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame052;

    return timer == 3;
}

s32 SelectEndingCutsceneOamSequence25Frame053(s32 timer, u16** oamFrame)
{
    *oamFrame = sEndingCutsceneOamFrame053;

    return timer == 3;
}

s32 SelectEndingCutsceneOamSequence26(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 3) {
        *oamFrame = sEndingCutsceneOamFrame054;
    } else if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame055;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame056;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame057;
    } else if (frame <= 19) {
        *oamFrame = sEndingCutsceneOamFrame058;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame059;
    } else if (frame <= 27) {
        *oamFrame = sEndingCutsceneOamFrame060;
    } else {
        *oamFrame = sEndingCutsceneOamFrame061;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence27(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 72;

    if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame062;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame063;
    } else if (frame <= 35) {
        *oamFrame = sEndingCutsceneOamFrame064;
    } else if (frame <= 47) {
        *oamFrame = sEndingCutsceneOamFrame065;
    } else if (frame <= 59) {
        *oamFrame = sEndingCutsceneOamFrame066;
    } else {
        *oamFrame = sEndingCutsceneOamFrame067;
    }

    return frame == 71;
}

s32 SelectEndingCutsceneOamSequence28(s32 timer, u16** oamFrame)
{
    if (timer <= 3) {
        *oamFrame = sEndingCutsceneOamFrame068;
    } else if (timer <= 7) {
        *oamFrame = sEndingCutsceneOamFrame069;
    } else if (timer <= 11) {
        *oamFrame = sEndingCutsceneOamFrame070;
    } else if (timer <= 15) {
        *oamFrame = sEndingCutsceneOamFrame071;
    } else if (timer <= 19) {
        *oamFrame = sEndingCutsceneOamFrame072;
    } else if (timer <= 23) {
        *oamFrame = sEndingCutsceneOamFrame073;
    } else if (timer <= 27) {
        *oamFrame = sEndingCutsceneOamFrame074;
    } else if (timer <= 31) {
        *oamFrame = sEndingCutsceneOamFrame075;
    } else {
        *oamFrame = 0;
    }

    return timer > 30;
}

s32 SelectEndingCutsceneOamSequence29(s32 timer, u16** oamFrame)
{
    if (timer <= 1) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 3) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 5) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 7) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 9) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 11) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 13) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 17) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 21) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 25) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 29) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 33) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 37) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 41) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 45) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 49) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 53) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 57) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 61) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 65) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 69) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 73) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 77) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 81) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 85) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 105) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 107) {
        *oamFrame = sEndingCutsceneOamFrame084;
    } else if (timer <= 109) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 111) {
        *oamFrame = sEndingCutsceneOamFrame084;
    } else if (timer <= 113) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 163) {
        *oamFrame = sEndingCutsceneOamFrame084;
    } else if (timer <= 213) {
        *oamFrame = sEndingCutsceneOamFrame085;
    } else if (timer <= 221) {
        *oamFrame = sEndingCutsceneOamFrame084;
    } else if (timer <= 241) {
        *oamFrame = sEndingCutsceneOamFrame085;
    } else if (timer <= 247) {
        *oamFrame = sEndingCutsceneOamFrame084;
    } else if (timer <= 297) {
        *oamFrame = sEndingCutsceneOamFrame085;
    } else if (timer <= 347) {
        *oamFrame = sEndingCutsceneOamFrame086;
    } else {
        *oamFrame = sEndingCutsceneOamFrame085;
    }

    if (timer == 78) {
        m4aSongNumStartOrChange(425);
    }

    if (timer == 114) {
        return 2;
    }
    return timer > 347;
}

s32 SelectEndingCutsceneOamSequence30(s32 timer, u16** oamFrame)
{
    if (timer <= 1) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 3) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 5) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 7) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 9) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 11) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 13) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 15) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 19) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 23) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 27) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 31) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 35) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 39) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 43) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 47) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 51) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 55) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 59) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 63) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 67) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 71) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 75) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 79) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 83) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 103) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 105) {
        *oamFrame = sEndingCutsceneOamFrame089;
    } else if (timer <= 107) {
        *oamFrame = sEndingCutsceneOamFrame087;
    } else if (timer <= 109) {
        *oamFrame = sEndingCutsceneOamFrame089;
    } else if (timer <= 111) {
        *oamFrame = sEndingCutsceneOamFrame087;
    } else if (timer <= 161) {
        *oamFrame = sEndingCutsceneOamFrame089;
    } else if (timer <= 211) {
        *oamFrame = sEndingCutsceneOamFrame088;
    } else if (timer <= 219) {
        *oamFrame = sEndingCutsceneOamFrame089;
    } else if (timer <= 239) {
        *oamFrame = sEndingCutsceneOamFrame088;
    } else if (timer <= 245) {
        *oamFrame = sEndingCutsceneOamFrame089;
    } else if (timer <= 295) {
        *oamFrame = sEndingCutsceneOamFrame088;
    } else if (timer <= 345) {
        *oamFrame = sEndingCutsceneOamFrame090;
    } else {
        *oamFrame = sEndingCutsceneOamFrame088;
    }

    if (timer == 76) {
        m4aSongNumStartOrChange(425);
    }

    if (timer == 112) {
        return 2;
    }
    return timer > 345;
}

s32 SelectEndingCutsceneOamSequence31(s32 timer, u16** oamFrame)
{
    if (timer <= 1) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 3) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 5) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 7) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 9) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 11) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 13) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 17) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 21) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 25) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 29) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 33) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 37) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 41) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 45) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 49) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 53) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 57) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 61) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 65) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 69) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 73) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 77) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 81) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 85) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 105) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 109) {
        *oamFrame = sEndingCutsceneOamFrame100;
    } else if (timer <= 113) {
        *oamFrame = sEndingCutsceneOamFrame099;
    } else if (timer <= 117) {
        *oamFrame = sEndingCutsceneOamFrame101;
    } else if (timer <= 121) {
        *oamFrame = sEndingCutsceneOamFrame099;
    } else if (timer <= 125) {
        *oamFrame = sEndingCutsceneOamFrame100;
    } else if (timer <= 127) {
        *oamFrame = sEndingCutsceneOamFrame093;
    } else if (timer <= 129) {
        *oamFrame = sEndingCutsceneOamFrame091;
    } else if (timer <= 131) {
        *oamFrame = sEndingCutsceneOamFrame093;
    } else if (timer <= 133) {
        *oamFrame = sEndingCutsceneOamFrame091;
    } else if (timer <= 183) {
        *oamFrame = sEndingCutsceneOamFrame093;
    } else if (timer <= 233) {
        *oamFrame = sEndingCutsceneOamFrame092;
    } else if (timer <= 241) {
        *oamFrame = sEndingCutsceneOamFrame093;
    } else if (timer <= 261) {
        *oamFrame = sEndingCutsceneOamFrame092;
    } else if (timer <= 267) {
        *oamFrame = sEndingCutsceneOamFrame093;
    } else if (timer <= 317) {
        *oamFrame = sEndingCutsceneOamFrame092;
    } else if (timer <= 367) {
        *oamFrame = sEndingCutsceneOamFrame094;
    } else {
        *oamFrame = sEndingCutsceneOamFrame092;
    }

    if (timer == 78) {
        m4aSongNumStartOrChange(425);
    }

    if (timer == 134) {
        return 2;
    }
    return timer > 367;
}

s32 SelectEndingCutsceneOamSequence32(s32 timer, u16** oamFrame)
{
    if (timer <= 1) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 3) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 5) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 7) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 9) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 11) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 13) {
        *oamFrame = sEndingCutsceneOamFrame076;
    } else if (timer <= 17) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 21) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 25) {
        *oamFrame = sEndingCutsceneOamFrame077;
    } else if (timer <= 29) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 33) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 37) {
        *oamFrame = sEndingCutsceneOamFrame078;
    } else if (timer <= 41) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 45) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 49) {
        *oamFrame = sEndingCutsceneOamFrame079;
    } else if (timer <= 53) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 57) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 61) {
        *oamFrame = sEndingCutsceneOamFrame080;
    } else if (timer <= 65) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 69) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 73) {
        *oamFrame = sEndingCutsceneOamFrame081;
    } else if (timer <= 77) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 81) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 85) {
        *oamFrame = sEndingCutsceneOamFrame082;
    } else if (timer <= 105) {
        *oamFrame = sEndingCutsceneOamFrame083;
    } else if (timer <= 109) {
        *oamFrame = sEndingCutsceneOamFrame100;
    } else if (timer <= 113) {
        *oamFrame = sEndingCutsceneOamFrame099;
    } else if (timer <= 117) {
        *oamFrame = sEndingCutsceneOamFrame101;
    } else if (timer <= 121) {
        *oamFrame = sEndingCutsceneOamFrame099;
    } else if (timer <= 125) {
        *oamFrame = sEndingCutsceneOamFrame100;
    } else if (timer <= 127) {
        *oamFrame = sEndingCutsceneOamFrame097;
    } else if (timer <= 129) {
        *oamFrame = sEndingCutsceneOamFrame095;
    } else if (timer <= 131) {
        *oamFrame = sEndingCutsceneOamFrame097;
    } else if (timer <= 133) {
        *oamFrame = sEndingCutsceneOamFrame095;
    } else if (timer <= 135) {
        *oamFrame = sEndingCutsceneOamFrame097;
    } else if (timer <= 185) {
        *oamFrame = sEndingCutsceneOamFrame096;
    } else if (timer <= 193) {
        *oamFrame = sEndingCutsceneOamFrame102;
    } else if (timer <= 201) {
        *oamFrame = sEndingCutsceneOamFrame103;
    } else if (timer <= 209) {
        *oamFrame = sEndingCutsceneOamFrame104;
    } else if (timer <= 217) {
        *oamFrame = sEndingCutsceneOamFrame105;
    } else if (timer <= 225) {
        *oamFrame = sEndingCutsceneOamFrame102;
    } else if (timer <= 233) {
        *oamFrame = sEndingCutsceneOamFrame103;
    } else if (timer <= 241) {
        *oamFrame = sEndingCutsceneOamFrame104;
    } else if (timer <= 249) {
        *oamFrame = sEndingCutsceneOamFrame105;
    } else if (timer <= 269) {
        *oamFrame = sEndingCutsceneOamFrame096;
    } else if (timer <= 289) {
        *oamFrame = sEndingCutsceneOamFrame098;
    } else {
        *oamFrame = sEndingCutsceneOamFrame096;
    }

    if (timer == 78) {
        m4aSongNumStartOrChange(425);
    }

    if (timer == 134) {
        return 2;
    }
    return timer > 289;
}

s32 SelectEndingCutsceneOamSequence33(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame106;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame115;
    } else {
        *oamFrame = sEndingCutsceneOamFrame116;
    }

    return frame == 23;
}

s32 SelectEndingCutsceneOamSequence34(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame107;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame117;
    } else {
        *oamFrame = sEndingCutsceneOamFrame118;
    }

    return frame == 23;
}

s32 SelectEndingCutsceneOamSequence35(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 16;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame108;
    } else {
        *oamFrame = sEndingCutsceneOamFrame109;
    }

    return frame == 15;
}

s32 SelectEndingCutsceneOamSequence36(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 47;

    if (frame <= 14) {
        *oamFrame = sEndingCutsceneOamFrame110;
    } else if (frame <= 22) {
        *oamFrame = sEndingCutsceneOamFrame111;
    } else if (frame <= 30) {
        *oamFrame = sEndingCutsceneOamFrame112;
    } else if (frame <= 38) {
        *oamFrame = sEndingCutsceneOamFrame113;
    } else {
        *oamFrame = sEndingCutsceneOamFrame114;
    }

    return frame == 46;
}

s32 SelectEndingCutsceneOamSequence37(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame119;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame120;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame121;
    } else {
        *oamFrame = sEndingCutsceneOamFrame120;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence38(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame122;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame123;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame124;
    } else {
        *oamFrame = sEndingCutsceneOamFrame123;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence39(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame125;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame126;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame127;
    } else {
        *oamFrame = sEndingCutsceneOamFrame126;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence40(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame128;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame129;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame130;
    } else {
        *oamFrame = sEndingCutsceneOamFrame129;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence41(s32 timer, u16** oamFrame)
{
    s32 frame;
    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame131;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame132;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame133;
    } else {
        *oamFrame = sEndingCutsceneOamFrame132;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence41Variant01(s32 timer, u16** oamFrame)
{
    s32 frame;
    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame134;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame135;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame136;
    } else {
        *oamFrame = sEndingCutsceneOamFrame135;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence41Variant02(s32 timer, u16** oamFrame)
{
    s32 frame;
    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame137;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame138;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame139;
    } else {
        *oamFrame = sEndingCutsceneOamFrame138;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence41Variant03(s32 timer, u16** oamFrame)
{
    s32 frame;
    frame = timer % 32;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame140;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame141;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame142;
    } else {
        *oamFrame = sEndingCutsceneOamFrame141;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence41Variant04(s32 timer, u16** oamFrame)
{
    s32 frame;
    frame = timer % 64;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame119;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame137;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame122;
    } else if (frame <= 31) {
        *oamFrame = sEndingCutsceneOamFrame140;
    } else if (frame <= 39) {
        *oamFrame = sEndingCutsceneOamFrame128;
    } else if (frame <= 47) {
        *oamFrame = sEndingCutsceneOamFrame134;
    } else if (frame <= 55) {
        *oamFrame = sEndingCutsceneOamFrame125;
    } else {
        *oamFrame = sEndingCutsceneOamFrame131;
    }

    return frame == 63;
}

s32 SelectEndingCutsceneOamSequence42(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame == 0 || frame == 15) {
        m4aSongNumStartOrChange(1);
    }

    if (frame <= 3) {
        *oamFrame = sEndingCutsceneOamFrame143;
    } else if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame144;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame145;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame146;
    } else if (frame <= 19) {
        *oamFrame = sEndingCutsceneOamFrame147;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame148;
    } else if (frame <= 27) {
        *oamFrame = sEndingCutsceneOamFrame149;
    } else {
        *oamFrame = sEndingCutsceneOamFrame150;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence43(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 24;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame151;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame152;
    } else {
        *oamFrame = sEndingCutsceneOamFrame153;
    }

    return frame == 23;
}

s32 SelectEndingCutsceneOamSequence44(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame == 0 || frame == 15) {
        m4aSongNumStartOrChange(1);
    }

    if (frame <= 3) {
        *oamFrame = sEndingCutsceneOamFrame154;
    } else if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame155;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame156;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame157;
    } else if (frame <= 19) {
        *oamFrame = sEndingCutsceneOamFrame158;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame159;
    } else if (frame <= 27) {
        *oamFrame = sEndingCutsceneOamFrame160;
    } else {
        *oamFrame = sEndingCutsceneOamFrame161;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence45(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 48;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame151;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame152;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame163;
    } else if (frame <= 31) {
        *oamFrame = sEndingCutsceneOamFrame162;
    } else if (frame <= 39) {
        *oamFrame = sEndingCutsceneOamFrame153;
    } else {
        *oamFrame = sEndingCutsceneOamFrame164;
    }

    return frame == 47;
}

s32 SelectEndingCutsceneOamSequence46(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame == 0 || frame == 15) {
        m4aSongNumStartOrChange(1);
    }

    if (frame <= 3) {
        *oamFrame = sEndingCutsceneOamFrame165;
    } else if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame166;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame167;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame168;
    } else if (frame <= 19) {
        *oamFrame = sEndingCutsceneOamFrame169;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame170;
    } else if (frame <= 27) {
        *oamFrame = sEndingCutsceneOamFrame171;
    } else {
        *oamFrame = sEndingCutsceneOamFrame172;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence47(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 32;

    if (frame <= 3) {
        *oamFrame = sEndingCutsceneOamFrame173;
    } else if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame174;
    } else if (frame <= 11) {
        *oamFrame = sEndingCutsceneOamFrame175;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame176;
    } else if (frame <= 19) {
        *oamFrame = sEndingCutsceneOamFrame177;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame178;
    } else if (frame <= 27) {
        *oamFrame = sEndingCutsceneOamFrame179;
    } else {
        *oamFrame = sEndingCutsceneOamFrame180;
    }

    return frame == 31;
}

s32 SelectEndingCutsceneOamSequence48(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 48;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame181;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame182;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame183;
    } else if (frame <= 31) {
        *oamFrame = sEndingCutsceneOamFrame181;
    } else if (frame <= 39) {
        *oamFrame = sEndingCutsceneOamFrame182;
    } else {
        *oamFrame = sEndingCutsceneOamFrame183;
    }

    return frame == 47;
}

s32 SelectEndingCutsceneOamSequence49(s32 timer, u16** oamFrame)
{
    s32 frame;

    frame = timer % 48;

    if (frame <= 7) {
        *oamFrame = sEndingCutsceneOamFrame184;
    } else if (frame <= 15) {
        *oamFrame = sEndingCutsceneOamFrame185;
    } else if (frame <= 23) {
        *oamFrame = sEndingCutsceneOamFrame186;
    } else if (frame <= 31) {
        *oamFrame = sEndingCutsceneOamFrame184;
    } else if (frame <= 39) {
        *oamFrame = sEndingCutsceneOamFrame185;
    } else {
        *oamFrame = sEndingCutsceneOamFrame186;
    }

    return frame == 47;
}

s32 ReadEndingCutscenePath00(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = 0;

    if (timer >= 0) {
        frame = 269;
        if ((u32)timer <= (u32)frame) {
            frame = timer;
        }
    }

    *xOut = sEndingCutscenePathData000[frame];
    *yOut = sEndingCutscenePathData001[frame];
    *scaleOut = sEndingCutscenePathData002[frame];
    *oamFrameOut = sEndingCutscenePathData003[frame];

    return timer == 269;
}

s32 ReadEndingCutscenePath01(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = 0;

    if (timer >= 0) {
        frame = 269;
        if ((u32)timer <= (u32)frame) {
            frame = timer;
        }
    }

    *xOut = sEndingCutscenePathData004[frame];
    *yOut = sEndingCutscenePathData005[frame];
    *scaleOut = sEndingCutscenePathData006[frame];
    *oamFrameOut = sEndingCutscenePathData007[frame];

    return timer == 269;
}

s32 ReadEndingCutscenePath02(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = 0;

    if (timer >= 0) {
        frame = 269;
        if ((u32)timer <= (u32)frame) {
            frame = timer;
        }
    }

    *xOut = sEndingCutscenePathData008[frame];
    *yOut = sEndingCutscenePathData009[frame];
    *scaleOut = sEndingCutscenePathData010[frame];
    *oamFrameOut = sEndingCutscenePathData011[frame];

    return timer == 269;
}

s32 ReadEndingCutscenePath03(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = 0;

    if (timer >= 0) {
        frame = 269;
        if ((u32)timer <= (u32)frame) {
            frame = timer;
        }
    }

    *xOut = sEndingCutscenePathData012[frame];
    *yOut = sEndingCutscenePathData013[frame];
    *scaleOut = sEndingCutscenePathData014[frame];
    *oamFrameOut = sEndingCutscenePathData015[frame];

    return timer == 269;
}

s32 ReadEndingCutscenePath04(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData016[frame];
    *yOut = sEndingCutscenePathData017[frame];
    *scaleOut = sEndingCutscenePathData018[frame];
    *oamFrameOut = sEndingCutscenePathData019[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath05(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData020[frame];
    *yOut = sEndingCutscenePathData021[frame];
    *scaleOut = sEndingCutscenePathData022[frame];
    *oamFrameOut = sEndingCutscenePathData023[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath06(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData024[frame];
    *yOut = sEndingCutscenePathData025[frame];
    *scaleOut = sEndingCutscenePathData026[frame];
    *oamFrameOut = sEndingCutscenePathData027[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath07(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData028[frame];
    *yOut = sEndingCutscenePathData029[frame];
    *scaleOut = sEndingCutscenePathData030[frame];
    *oamFrameOut = sEndingCutscenePathData031[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath08(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData032[frame];
    *yOut = sEndingCutscenePathData033[frame];
    *scaleOut = sEndingCutscenePathData034[frame];
    *oamFrameOut = sEndingCutscenePathData035[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath09(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData036[frame];
    *yOut = sEndingCutscenePathData037[frame];
    *scaleOut = sEndingCutscenePathData038[frame];
    *oamFrameOut = sEndingCutscenePathData039[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath10(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData040[frame];
    *yOut = sEndingCutscenePathData041[frame];
    *scaleOut = sEndingCutscenePathData042[frame];
    *oamFrameOut = sEndingCutscenePathData043[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath11(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 279) {
        frame = 279;
    }

    *xOut = sEndingCutscenePathData044[frame];
    *yOut = sEndingCutscenePathData045[frame];
    *scaleOut = sEndingCutscenePathData046[frame];
    *oamFrameOut = sEndingCutscenePathData047[frame];

    return (u32)timer > 278;
}

s32 ReadEndingCutscenePath12(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData048[frame];
    *yOut = sEndingCutscenePathData049[frame];
    *scaleOut = sEndingCutscenePathData050[frame];
    *oamFrameOut = sEndingCutscenePathData051[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath13(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData052[frame];
    *yOut = sEndingCutscenePathData053[frame];
    *scaleOut = sEndingCutscenePathData054[frame];
    *oamFrameOut = sEndingCutscenePathData055[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath14(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData056[frame];
    *yOut = sEndingCutscenePathData057[frame];
    *scaleOut = sEndingCutscenePathData058[frame];
    *oamFrameOut = sEndingCutscenePathData059[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath15(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData060[frame];
    *yOut = sEndingCutscenePathData061[frame];
    *scaleOut = sEndingCutscenePathData062[frame];
    *oamFrameOut = sEndingCutscenePathData063[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath16(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData064[frame];
    *yOut = sEndingCutscenePathData065[frame];
    *scaleOut = sEndingCutscenePathData066[frame];
    *oamFrameOut = sEndingCutscenePathData067[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath17(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData068[frame];
    *yOut = sEndingCutscenePathData069[frame];
    *scaleOut = sEndingCutscenePathData070[frame];
    *oamFrameOut = sEndingCutscenePathData071[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath18(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData072[frame];
    *yOut = sEndingCutscenePathData073[frame];
    *scaleOut = sEndingCutscenePathData074[frame];
    *oamFrameOut = sEndingCutscenePathData075[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath19(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData076[frame];
    *yOut = sEndingCutscenePathData077[frame];
    *scaleOut = sEndingCutscenePathData078[frame];
    *oamFrameOut = sEndingCutscenePathData079[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath20(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData080[frame];
    *yOut = sEndingCutscenePathData081[frame];
    *scaleOut = sEndingCutscenePathData082[frame];
    *oamFrameOut = sEndingCutscenePathData083[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath21(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData084[frame];
    *yOut = sEndingCutscenePathData085[frame];
    *scaleOut = sEndingCutscenePathData086[frame];
    *oamFrameOut = sEndingCutscenePathData087[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath22(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData088[frame];
    *yOut = sEndingCutscenePathData089[frame];
    *scaleOut = sEndingCutscenePathData090[frame];
    *oamFrameOut = sEndingCutscenePathData091[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath23(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData092[frame];
    *yOut = sEndingCutscenePathData093[frame];
    *scaleOut = sEndingCutscenePathData094[frame];
    *oamFrameOut = sEndingCutscenePathData095[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath24(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData096[frame];
    *yOut = sEndingCutscenePathData097[frame];
    *scaleOut = sEndingCutscenePathData098[frame];
    *oamFrameOut = sEndingCutscenePathData099[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath25(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData100[frame];
    *yOut = sEndingCutscenePathData101[frame];
    *scaleOut = sEndingCutscenePathData102[frame];
    *oamFrameOut = sEndingCutscenePathData103[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath26(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData104[frame];
    *yOut = sEndingCutscenePathData105[frame];
    *scaleOut = sEndingCutscenePathData106[frame];
    *oamFrameOut = sEndingCutscenePathData107[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath27(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData108[frame];
    *yOut = sEndingCutscenePathData109[frame];
    *scaleOut = sEndingCutscenePathData110[frame];
    *oamFrameOut = sEndingCutscenePathData111[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath28(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData112[frame];
    *yOut = sEndingCutscenePathData113[frame];
    *scaleOut = sEndingCutscenePathData114[frame];
    *oamFrameOut = sEndingCutscenePathData115[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath29(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData116[frame];
    *yOut = sEndingCutscenePathData117[frame];
    *scaleOut = sEndingCutscenePathData118[frame];
    *oamFrameOut = sEndingCutscenePathData119[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath30(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData120[frame];
    *yOut = sEndingCutscenePathData121[frame];
    *scaleOut = sEndingCutscenePathData122[frame];
    *oamFrameOut = sEndingCutscenePathData123[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath31(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData124[frame];
    *yOut = sEndingCutscenePathData125[frame];
    *scaleOut = sEndingCutscenePathData126[frame];
    *oamFrameOut = sEndingCutscenePathData127[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath32(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData128[frame];
    *yOut = sEndingCutscenePathData129[frame];
    *scaleOut = sEndingCutscenePathData130[frame];
    *oamFrameOut = sEndingCutscenePathData131[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath33(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData132[frame];
    *yOut = sEndingCutscenePathData133[frame];
    *scaleOut = sEndingCutscenePathData134[frame];
    *oamFrameOut = sEndingCutscenePathData135[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath34(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData136[frame];
    *yOut = sEndingCutscenePathData137[frame];
    *scaleOut = sEndingCutscenePathData138[frame];
    *oamFrameOut = sEndingCutscenePathData139[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutscenePath35(s32 timer, u16* xOut, u16* yOut, u16* scaleOut, u16* oamFrameOut)
{
    s32 frame;

    frame = timer;

    if ((u32)timer > 280) {
        frame = 280;
    }

    *xOut = sEndingCutscenePathData140[frame];
    *yOut = sEndingCutscenePathData141[frame];
    *scaleOut = sEndingCutscenePathData142[frame];
    *oamFrameOut = sEndingCutscenePathData143[frame];

    return (u32)timer > 279;
}

s32 ReadEndingCutsceneIndexedPathPair(s32 pathIndex, u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;
    u16* xPath;
    u16* yPath;

    maxTimer = 0x2BA;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    xPath = GP(sEndingCutscenePathXTable[pathIndex]);
    yPath = GP(sEndingCutscenePathYTable[pathIndex]);

    *xOut = xPath[timer];
    *yOut = yPath[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath36(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 0x1D3;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData144[timer];
    *yOut = sEndingCutscenePathData145[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath37(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 0x1D3;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData146[timer];
    *yOut = sEndingCutscenePathData147[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath38(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 0x1D3;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData148[timer];
    *yOut = sEndingCutscenePathData149[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath39(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 0x1D3;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData150[timer];
    *yOut = sEndingCutscenePathData151[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath40(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 252;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData152[timer];
    *yOut = sEndingCutscenePathData153[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath41(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 268;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData154[timer];
    *yOut = sEndingCutscenePathData155[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath42(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 260;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData156[timer];
    *yOut = sEndingCutscenePathData157[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath43(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 0x115;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData158[timer];
    *yOut = sEndingCutscenePathData159[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath44(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 256;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData160[timer];
    *yOut = sEndingCutscenePathData161[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath45(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 272;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData162[timer];
    *yOut = sEndingCutscenePathData163[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath46(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 264;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData164[timer];
    *yOut = sEndingCutscenePathData165[timer];

    return timer == maxTimer;
}

s32 ReadEndingCutscenePath47(u32 timer, u16* xOut, u16* yOut)
{
    u32 maxTimer;

    maxTimer = 280;

    if (timer > maxTimer) {
        timer = maxTimer;
    }

    *xOut = sEndingCutscenePathData166[timer];
    *yOut = sEndingCutscenePathData167[timer];

    return timer == maxTimer;
}

s32 SelectCutsceneScrollFrameSequence01(s32 timer, u8** ppData)
{
    if (timer <= 7) {
        *ppData = &sUnk_82D270C;
        return timer > 0x12A;
    }
    if (timer <= 15) {
        *ppData = &sUnk_82D2714;
        return timer > 0x12A;
    }
    if (timer <= 23) {
        *ppData = &sUnk_82D2722;
        return timer > 0x12A;
    }
    if (timer <= 31) {
        *ppData = &sUnk_82D272A;
        return timer > 0x12A;
    }
    if (timer <= 47) {
        *ppData = &sUnk_82D2738;
        return timer > 0x12A;
    }
    if (timer <= 55) {
        *ppData = &sUnk_82D2746;
        return timer > 0x12A;
    }
    if (timer <= 63) {
        *ppData = &sUnk_82D275A;
        return timer > 0x12A;
    }
    if (timer <= 71) {
        *ppData = &sUnk_82D276E;
        return timer > 0x12A;
    }
    if (timer <= 87) {
        *ppData = &sUnk_82D2956;
        return timer > 0x12A;
    }
    if (timer <= 95) {
        *ppData = &sUnk_82D2788;
        return timer > 0x12A;
    }
    if (timer <= 103) {
        *ppData = &sUnk_82D27B4;
        return timer > 0x12A;
    }
    if (timer <= 111) {
        *ppData = &sUnk_82D27E0;
        return timer > 0x12A;
    }
    if (timer <= 119) {
        *ppData = &sUnk_82D2812;
        return timer > 0x12A;
    }
    if (timer <= 139) {
        *ppData = &sUnk_82D283E;
        return timer > 0x12A;
    }
    if (timer <= 159) {
        *ppData = &sUnk_82D286A;
        return timer > 0x12A;
    }
    if (timer <= 179) {
        *ppData = &sUnk_82D289C;
        return timer > 0x12A;
    }
    if (timer <= 199) {
        *ppData = &sUnk_82D28D4;
        return timer > 0x12A;
    }
    *ppData = &sUnk_82D2912;
    return timer > 0x12A;
}

s32 SelectCutsceneScrollFrameSequence02(s32 timer, u8** ppData)
{
    if (timer <= 7) {
        *ppData = &sUnk_82D2976;
        return timer > 0xFC;
    }
    if (timer <= 15) {
        *ppData = &sUnk_82D297E;
        return timer > 0xFC;
    }
    if (timer <= 23) {
        *ppData = &sUnk_82D298C;
        return timer > 0xFC;
    }
    if (timer <= 31) {
        *ppData = &sUnk_82D2994;
        return timer > 0xFC;
    }
    if (timer <= 47) {
        *ppData = &sUnk_82D29A2;
        return timer > 0xFC;
    }
    if (timer <= 55) {
        *ppData = &sUnk_82D29B0;
        return timer > 0xFC;
    }
    if (timer <= 63) {
        *ppData = &sUnk_82D29BE;
        return timer > 0xFC;
    }
    if (timer <= 93) {
        *ppData = &sUnk_82D29D2;
        return timer > 0xFC;
    }
    if (timer <= 113) {
        *ppData = &sUnk_82D29E6;
        return timer > 0xFC;
    }
    if (timer <= 133) {
        *ppData = &sUnk_82D2A00;
        return timer > 0xFC;
    }
    if (timer <= 153) {
        *ppData = &sUnk_82D2A20;
        return timer > 0xFC;
    }
    *ppData = &sUnk_82D2A46;
    return timer > 0xFC;
}

s32 CalculateCutsceneScrollPath01(s32 tableIndex, s32 step, s16* pX, s16* pY)
{
    u16* table1;
    u16* table2;
    s32 index;

    table1 = GP(sUnk_878E7C0[(s16)tableIndex]);
    table2 = GP(sUnk_878E7D0[(s16)tableIndex]);
    index = 0;
    if (step >= 0) {
        index = step;
        if (step > 179) {
            index = 179;
        }
    }
    *pX = -table1[index];
    *pY = -table2[index];
    return step > 178;
}

s32 CalculateCutsceneScrollPath02(s32 tableIndex, s32 step, s16* pX, s16* pY)
{
    u16* table1;
    u16* table2;
    s32 index;

    table1 = GP(sUnk_878E7E0[(s16)tableIndex]);
    table2 = GP(sUnk_878E7F0[(s16)tableIndex]);
    index = 0;
    if (step >= 0) {
        index = step;
        if (step > 179) {
            index = 179;
        }
    }
    *pX = -table1[index];
    *pY = -table2[index];
    return step > 178;
}

s32 SelectTitleScreenNormalLogoIntroOamFrame(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 28;

    if (frame <= 5) {
        *oamFrame = &sTitleScreenNormalLogoFrame0Oam;
    } else if (frame <= 11) {
        *oamFrame = &sTitleScreenNormalLogoFrame1Oam;
    } else if (frame <= 17) {
        *oamFrame = &sTitleScreenNormalLogoFrame0Oam;
    } else {
        *oamFrame = &sTitleScreenNormalLogoFrame2Oam;
    }

    return frame == 27;
}

s32 SelectTitleScreenNormalLogoIdleOamFrame(s32 timer, u8** oamFrame)
{
    *oamFrame = &sTitleScreenNormalLogoIdleOam;

    return timer == 30;
}

s32 SelectTitleScreenSHardLogoIntroOamFrame(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 80;

    if (frame <= 29) {
        *oamFrame = &sTitleScreenSHardLogoFrame0Oam;
    } else if (frame <= 39) {
        *oamFrame = &sTitleScreenSHardLogoFrame1Oam;
    } else if (frame <= 49) {
        *oamFrame = &sTitleScreenSHardLogoFrame0Oam;
    } else if (frame <= 59) {
        *oamFrame = &sTitleScreenSHardLogoFrame1Oam;
    } else if (frame <= 69) {
        *oamFrame = &sTitleScreenSHardLogoFrame0Oam;
    } else {
        *oamFrame = &sTitleScreenSHardLogoFrame1Oam;
    }

    return frame == 79;
}

s32 SelectTitleScreenSHardLogoIdleOamFrame(s32 timer, u8** oamFrame)
{
    *oamFrame = &sTitleScreenSHardLogoFrame0Oam;

    return timer == 30;
}

s32 SelectTitleScreenOverlayOamFrame(s32 timer, u8** oamFrame)
{
    s32 frame;

    frame = timer % 208;

    if (frame <= 3) {
        *oamFrame = &sTitleScreenOverlayFrame00Oam;
    } else if (frame <= 7) {
        *oamFrame = &sTitleScreenOverlayFrame01Oam;
    } else if (frame <= 11) {
        *oamFrame = &sTitleScreenOverlayFrame02Oam;
    } else if (frame <= 15) {
        *oamFrame = &sTitleScreenOverlayFrame03Oam;
    } else if (frame <= 19) {
        *oamFrame = &sTitleScreenOverlayFrame04Oam;
    } else if (frame <= 23) {
        *oamFrame = &sTitleScreenOverlayFrame05Oam;
    } else if (frame <= 27) {
        *oamFrame = &sTitleScreenOverlayFrame06Oam;
    } else if (frame <= 31) {
        *oamFrame = &sTitleScreenOverlayFrame07Oam;
    } else if (frame <= 35) {
        *oamFrame = &sTitleScreenOverlayFrame08Oam;
    } else if (frame <= 39) {
        *oamFrame = &sTitleScreenOverlayFrame09Oam;
    } else if (frame <= 43) {
        *oamFrame = &sTitleScreenOverlayFrame10Oam;
    } else if (frame <= 47) {
        *oamFrame = &sTitleScreenOverlayFrame11Oam;
    } else if (frame <= 51) {
        *oamFrame = &sTitleScreenOverlayFrame12Oam;
    } else if (frame <= 55) {
        *oamFrame = &sTitleScreenOverlayFrame13Oam;
    } else if (frame <= 59) {
        *oamFrame = &sTitleScreenOverlayFrame14Oam;
    } else if (frame <= 63) {
        *oamFrame = &sTitleScreenOverlayFrame15Oam;
    } else if (frame <= 67) {
        *oamFrame = &sTitleScreenOverlayFrame16Oam;
    } else if (frame <= 71) {
        *oamFrame = &sTitleScreenOverlayFrame17Oam;
    } else if (frame <= 75) {
        *oamFrame = &sTitleScreenOverlayFrame18Oam;
    } else if (frame <= 79) {
        *oamFrame = &sTitleScreenOverlayFrame19Oam;
    } else if (frame <= 83) {
        *oamFrame = &sTitleScreenOverlayFrame20Oam;
    } else if (frame <= 87) {
        *oamFrame = &sTitleScreenOverlayFrame21Oam;
    } else if (frame <= 91) {
        *oamFrame = &sTitleScreenOverlayFrame22Oam;
    } else if (frame <= 95) {
        *oamFrame = &sTitleScreenOverlayFrame23Oam;
    } else if (frame <= 99) {
        *oamFrame = &sTitleScreenOverlayFrame24Oam;
    } else if (frame <= 103) {
        *oamFrame = &sTitleScreenOverlayFrame25Oam;
    } else if (frame <= 107) {
        *oamFrame = &sTitleScreenOverlayFrame26Oam;
    } else if (frame <= 111) {
        *oamFrame = &sTitleScreenOverlayFrame27Oam;
    } else if (frame <= 115) {
        *oamFrame = &sTitleScreenOverlayFrame28Oam;
    } else if (frame <= 119) {
        *oamFrame = &sTitleScreenOverlayFrame29Oam;
    } else if (frame <= 123) {
        *oamFrame = &sTitleScreenOverlayFrame30Oam;
    } else if (frame <= 127) {
        *oamFrame = &sTitleScreenOverlayFrame31Oam;
    } else if (frame <= 131) {
        *oamFrame = &sTitleScreenOverlayFrame32Oam;
    } else if (frame <= 135) {
        *oamFrame = &sTitleScreenOverlayFrame33Oam;
    } else if (frame <= 139) {
        *oamFrame = &sTitleScreenOverlayFrame34Oam;
    } else if (frame <= 143) {
        *oamFrame = &sTitleScreenOverlayFrame35Oam;
    } else if (frame <= 147) {
        *oamFrame = &sTitleScreenOverlayFrame36Oam;
    } else if (frame <= 151) {
        *oamFrame = &sTitleScreenOverlayFrame37Oam;
    } else if (frame <= 155) {
        *oamFrame = &sTitleScreenOverlayFrame38Oam;
    } else if (frame <= 159) {
        *oamFrame = &sTitleScreenOverlayFrame39Oam;
    } else if (frame <= 163) {
        *oamFrame = &sTitleScreenOverlayFrame40Oam;
    } else if (frame <= 167) {
        *oamFrame = &sTitleScreenOverlayFrame41Oam;
    } else if (frame <= 171) {
        *oamFrame = &sTitleScreenOverlayFrame42Oam;
    } else if (frame <= 175) {
        *oamFrame = &sTitleScreenOverlayFrame43Oam;
    } else if (frame <= 179) {
        *oamFrame = &sTitleScreenOverlayFrame44Oam;
    } else if (frame <= 183) {
        *oamFrame = &sTitleScreenOverlayFrame45Oam;
    } else if (frame <= 187) {
        *oamFrame = &sTitleScreenOverlayFrame46Oam;
    } else if (frame <= 191) {
        *oamFrame = &sTitleScreenOverlayFrame47Oam;
    } else if (frame <= 195) {
        *oamFrame = &sTitleScreenOverlayFrame48Oam;
    } else if (frame <= 199) {
        *oamFrame = &sTitleScreenOverlayFrame49Oam;
    } else if (frame <= 203) {
        *oamFrame = &sTitleScreenOverlayFrame50Oam;
    } else {
        *oamFrame = &sTitleScreenOverlayFrame51Oam;
    }

    return frame == 207;
}

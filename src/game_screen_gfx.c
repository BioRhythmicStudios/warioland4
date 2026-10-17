#include "game_screen_gfx.h"

#include "background_registers.h"
#include "bg_clip.h"
#include "color_effects.h"
#include "global_data.h"
#include "main.h"
#include "wario.h"

void GameScreenDrawWario(void)
{
    struct AnimationFrame* pAnimation;
    const u16* src;
    u16* dest;
    s32 currentSlot;
    s32 nextSlot;
    u16 attr;
    s32 xOffset;
    s32 yOffset;
    s32 priority;
    s32 palette;
    s32 afterimage;
    s32 i;

    GameScreenDrawWarioEffects();

    if (gWarioData.reaction > 1) {
        gWarioData.damageTimer = 0;
        gWarioDamagePaletteActive = 0;
    }

    if ((gWarioDamagePaletteActive != 0) && (gMainTimer & 2)) {
        return;
    }

    dest = (u16*)gOamBuffer;
    dest += gOamSlotsUsed * sizeof(OamData) / sizeof(u16);
    nextSlot = gOamSlotsUsed;
    currentSlot = gOamSlotsUsed;

    xOffset = PIXELS_FROM_SUBPIXELS(gWarioData.xPosition) - PIXELS_FROM_SUBPIXELS(gBg1XPosition);
    yOffset = PIXELS_FROM_SUBPIXELS(gWarioData.yPosition) - PIXELS_FROM_SUBPIXELS(gBg1YPosition);

    priority = 2;
    if (gDrawWarioOverBackground) {
        priority = 1;
    }
    palette = 2;

    if (gWarioMotionAfterimage.unk0) {
        if (gWarioData.horizontalDirection & DPAD_RIGHT) {
            pAnimation = GP(sUnk_82DDCD0[gWarioMotionAfterimage.unk0 - 1].left);
        } else {
            pAnimation = GP(sUnk_82DDCD0[gWarioMotionAfterimage.unk0 - 1].right);
        }
        pAnimation += gWarioMotionAfterimage.unk2;
        GP_SET(gWarioMotionAfterimage.unk4, GP(pAnimation->oam));

        src = GP(gWarioMotionAfterimage.unk4);
        nextSlot += *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = (attr + yOffset) & 0xFF;

            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + xOffset) & 0x1FF;

            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = priority;

            dest++;
        }
    }

    src = GP(gWarioData.pOamData);
    nextSlot += *(src++);
    for (; currentSlot < nextSlot; currentSlot++) {
        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.y = (attr + yOffset + 2) & 0xFF;

        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.x = (attr + xOffset) & 0x1FF;

        *(dest++) = *(src++);
        gOamBuffer[currentSlot].split.priority = priority;

        dest++;
    }

    if (!(gWarioDashAfterimage.unk0) || (gMainTimer & 2)) {
        gOamSlotsUsed = nextSlot;
        return;
    }

    afterimage = gWarioAfterimageHistoryIndex - 4;
    i = 0;
    while ((gWarioAfterimageHistoryWrapped != 0) || (afterimage >= 0)) {
        src = GP(gWarioData.pOamData);
        nextSlot += *(src++);

        afterimage &= 0x1F;
        xOffset = PIXELS_FROM_SUBPIXELS(gWarioAfterimageHistory.previousXPositions[afterimage]) -
                  PIXELS_FROM_SUBPIXELS(gBg1XPosition);
        yOffset = PIXELS_FROM_SUBPIXELS(gWarioAfterimageHistory.previousYPositions[afterimage]) -
                  PIXELS_FROM_SUBPIXELS(gBg1YPosition);

        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = (attr + yOffset) & 0xFF;

            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + xOffset) & 0x1FF;

            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = priority;
            gOamBuffer[currentSlot].split.paletteNum = palette;

            dest++;
        }

        i++;
        if ((i > 2)) {
            break;
        }

        afterimage -= (i * 3);
    }

    gOamSlotsUsed = nextSlot;
}

void GameScreenDrawWarioEffects(void)
{
    struct AnimationFrameU16* pAnimation;
    const u16* src;
    u16* dest;
    u16 attr;
    s16 effectX;
    s16 effectY;
    s16 xOffset;
    s16 yOffset;
    s32 priority;
    u32 temp;
    s32 currentSlot;
    s32 nextSlot;

    dest = (u16*)gOamBuffer;
    dest += gOamSlotsUsed * sizeof(OamData) / sizeof(u16);
    nextSlot = gOamSlotsUsed;
    currentSlot = gOamSlotsUsed;
    priority = 2;
    if (gDrawWarioOverBackground != 0) {
        priority = 1;
    }

    effectX = 0;
    switch (gWarioDustEffect1.unk0 - 1) {
        case 0:
            gWarioDustEffect1.unk0 += 1;
            gWarioDustEffect1.unk1 = 0;
            gWarioDustEffect1.unk2 = 0;
            gWarioDustEffect1.unk4 = gWarioData.xPosition;
            gWarioDustEffect1.unk6 = gWarioData.yPosition;
        case 1:
            if (gWarioDustEffect1.unk1 >= sUnk_82E11E4[gWarioDustEffect1.unk2].time) {
                gWarioDustEffect1.unk1 = 0;
                gWarioDustEffect1.unk2 += 1;
                if (sUnk_82E11E4[gWarioDustEffect1.unk2].time == 0) {
                    gWarioDustEffect1.unk0 = sUnk_82E11E4[gWarioDustEffect1.unk2].time;
                }
            }
            pAnimation = sUnk_82E11E4;
            break;

        case 2:
            gWarioDustEffect1.unk0 += 1;
            gWarioDustEffect1.unk1 = 0;
            gWarioDustEffect1.unk2 = 0;
            gWarioDustEffect1.unk4 = gWarioData.xPosition;
            gWarioDustEffect1.unk6 = gWarioData.yPosition - 0x18;
        case 3:
            if (gWarioDustEffect1.unk1 >= sUnk_82E12BC[gWarioDustEffect1.unk2].time) {
                gWarioDustEffect1.unk1 = 0;
                gWarioDustEffect1.unk2 += 1;
                if (sUnk_82E12BC[gWarioDustEffect1.unk2].time == 0) {
                    gWarioDustEffect1.unk0 = sUnk_82E12BC[gWarioDustEffect1.unk2].time;
                }
            }
            pAnimation = sUnk_82E12BC;
            break;

        case 4:
        case 6:
        case 8:
            gWarioDustEffect1.unk0 += 1;
            gWarioDustEffect1.unk1 = 0;
            gWarioDustEffect1.unk2 = 0;
            effectY = 8;
            if (0x10 & gWarioData.horizontalDirection) {
                GP_SET(gWarioDustEffect1.unk8, sUnk_82E1304);
                if (gWarioData.pose == 0x2C) {
                    effectX = -12;
                    effectY = -0x28;
                } else {
                    if (gWarioDustEffect1.unk0 == 6) {
                        effectX = -0x28;
                    } else if (gWarioDustEffect1.unk0 == 0xA) {
                        effectX = -0x18;
                    }
                }
            } else {
                GP_SET(gWarioDustEffect1.unk8, sUnk_82E1334);
                if (gWarioData.pose == 0x2C) {
                    effectX = 12;
                    effectY = -0x28;
                }
                if (gWarioDustEffect1.unk0 == 6) {
                    effectX = 0x28;
                } else if (gWarioDustEffect1.unk0 == 0xA) {
                    effectX = 0x18;
                }
            }
            gWarioDustEffect1.unk4 = gWarioData.xPosition + effectX;
            gWarioDustEffect1.unk6 = gWarioData.yPosition + effectY;
        case 5:
        case 7:
        case 9:
            if (gWarioDustEffect1.unk1 >= sUnk_82E1304[gWarioDustEffect1.unk2].time) {
                gWarioDustEffect1.unk1 = 0;
                gWarioDustEffect1.unk2 += 1;
                if (sUnk_82E1304[gWarioDustEffect1.unk2].time == 0) {
                    gWarioDustEffect1.unk0 = sUnk_82E1304[gWarioDustEffect1.unk2].time;
                }
            }
            pAnimation = GP(gWarioDustEffect1.unk8);
            break;

        case 10:
            gWarioDustEffect1.unk0 += 1;
            gWarioDustEffect1.unk1 = 0;
            gWarioDustEffect1.unk2 = 0;
            gWarioDustEffect1.unk4 = gWarioData.xPosition;
            gWarioDustEffect1.unk6 = gWarioData.yPosition - 0x28;
            GP_SET(gWarioDustEffect1.unk8, sUnk_82E14FC);
        case 11:
            if (gWarioDustEffect1.unk1 >= sUnk_82E14FC[gWarioDustEffect1.unk2].time) {
                gWarioDustEffect1.unk1 = 0;
                gWarioDustEffect1.unk2 += 1;
                if (sUnk_82E14FC[gWarioDustEffect1.unk2].time == 0) {
                    gWarioDustEffect1.unk0 = sUnk_82E14FC[gWarioDustEffect1.unk2].time;
                }
            }
            pAnimation = GP(gWarioDustEffect1.unk8);
            break;

        case 12:
            gWarioDustEffect1.unk0 += 1;
            gWarioDustEffect1.unk1 = 0;
            gWarioDustEffect1.unk2 = 0;
            gWarioDustEffect1.unk4 = gWarioData.xPosition;
            gWarioDustEffect1.unk6 = gWarioData.yPosition + 0x20;
            GP_SET(gWarioDustEffect1.unk8, sUnk_82E1304);
        case 13:
            if (gWarioDustEffect1.unk1 >= sUnk_82E1304[gWarioDustEffect1.unk2].time) {
                gWarioDustEffect1.unk1 = 0;
                gWarioDustEffect1.unk2 += 1;
                if (sUnk_82E1304[gWarioDustEffect1.unk2].time == 0) {
                    gWarioDustEffect1.unk0 = sUnk_82E1304[gWarioDustEffect1.unk2].time;
                }
            }
            pAnimation = GP(gWarioDustEffect1.unk8);
            break;
    }

    if (gWarioDustEffect1.unk0 != 0) {
        gWarioDustEffect1.unk1 += 1;
        xOffset = PIXELS_FROM_SUBPIXELS(gWarioDustEffect1.unk4) - PIXELS_FROM_SUBPIXELS(gBg1XPosition);
        yOffset = PIXELS_FROM_SUBPIXELS(gWarioDustEffect1.unk6) - PIXELS_FROM_SUBPIXELS(gBg1YPosition);
        pAnimation += gWarioDustEffect1.unk2;
        src = GP(pAnimation->oam);
        nextSlot = nextSlot + *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = (attr + yOffset) & 0xFF;

            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + xOffset) & 0x1FF;

            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = priority;

            dest++;
        }
    }

    switch (gWarioDustEffect2.unk0 - 1) {
        case 0:
            gWarioDustEffect2.unk0 += 1;
            gWarioDustEffect2.unk1 = 0;
            gWarioDustEffect2.unk2 = 0;
            if (0x10 & gWarioData.horizontalDirection) {
                GP_SET(gWarioDustEffect2.unk8, sUnk_82E1294);
                if (gWarioData.pose == 0xF) {
                    effectX = 0x20;
                } else {
                    effectX = 0;
                }
            } else {
                GP_SET(gWarioDustEffect2.unk8, sUnk_82E126C);
                if (gWarioData.pose == 0xF) {
                    effectX = -0x20;
                } else {
                    effectX = 0;
                }
            }
            gWarioDustEffect2.unk4 = gWarioData.xPosition + effectX;
            gWarioDustEffect2.unk6 = gWarioData.yPosition - 0x50;
        case 1:
            if (gWarioDustEffect2.unk1 >= sUnk_82E126C[gWarioDustEffect2.unk2].time) {
                gWarioDustEffect2.unk1 = 0;
                gWarioDustEffect2.unk2 += 1;
                if (sUnk_82E126C[gWarioDustEffect2.unk2].time == 0) {
                    gWarioDustEffect2.unk0 = sUnk_82E126C[gWarioDustEffect2.unk2].time;
                }
            }
            pAnimation = GP(gWarioDustEffect2.unk8);
            break;

        case 2:
            gWarioDustEffect2.unk0 += 1;
            gWarioDustEffect2.unk1 = 0;
            gWarioDustEffect2.unk2 = 0;
            if (0x10 & gWarioData.horizontalDirection) {
                GP_SET(gWarioDustEffect2.unk8, sUnk_82E13DC);
                effectX = 0x20;
            } else {
                GP_SET(gWarioDustEffect2.unk8, sUnk_82E140C);
                effectX = -0x20;
            }
            gWarioDustEffect2.unk4 = gWarioData.xPosition + effectX;
            gWarioDustEffect2.unk6 = gWarioData.yPosition - 0x20;
            temp = GetBackgroundCollisionAtPosition(gWarioData.yPosition - 0x68, gWarioDustEffect2.unk4);
            if (!(temp & 0xFF)) {
                gWarioDustEffect2.unk0 = 0;
                break;
            }
        case 3:
            if (gWarioDustEffect2.unk1 >= sUnk_82E13DC[gWarioDustEffect2.unk2].time) {
                gWarioDustEffect2.unk1 = 0;
                gWarioDustEffect2.unk2++;
                if (gWarioDustEffect2.unk2 == 2) {
                    temp = GetBackgroundCollisionAtPosition(gWarioDustEffect2.unk6 - 0x78, gWarioDustEffect2.unk4);
                    if (!(temp & 0xFF)) {
                        gWarioDustEffect2.unk2 = 4;
                    }
                } else {
                    if (sUnk_82E13DC[gWarioDustEffect2.unk2].time == 0) {
                        gWarioDustEffect2.unk0 = sUnk_82E13DC[gWarioDustEffect2.unk2].time;
                    }
                }
            }
            pAnimation = GP(gWarioDustEffect2.unk8);
            break;

        case 4:
            effectX = 0;
            effectY = 0;
            gWarioDustEffect2.unk0 += 1;
            gWarioDustEffect2.unk1 = 0;
            gWarioDustEffect2.unk2 = 0;
            if (0x10 & gWarioData.horizontalDirection) {
                if (gWarioData.pose == 9) {
                    GP_SET(gWarioDustEffect2.unk8, sUnk_82E143C);
                    effectX += 0x2C;
                    effectY += -0x20;
                } else {
                    GP_SET(gWarioDustEffect2.unk8, sUnk_82E149C);
                }
                if ((gWarioData.pose == 0xD) || (gWarioData.pose == 8)) {
                    effectX -= 0x10;
                    effectY -= 0x68;
                }
            } else {
                if (gWarioData.pose == 9) {
                    GP_SET(gWarioDustEffect2.unk8, sUnk_82E149C);
                    effectX -= 0x2C;
                    effectY -= 0x20;
                } else {
                    GP_SET(gWarioDustEffect2.unk8, sUnk_82E143C);
                }
                if ((gWarioData.pose == 0xD) || (gWarioData.pose == 8)) {
                    effectX += 0x10;
                    effectY -= 0x68;
                }
            }
            gWarioDustEffect2.unk4 = gWarioData.xPosition + effectX;
            gWarioDustEffect2.unk6 = gWarioData.yPosition + effectY;
        case 5:
            if (gWarioDustEffect2.unk1 >= sUnk_82E149C[gWarioDustEffect2.unk2].time) {
                gWarioDustEffect2.unk1 = 0;
                gWarioDustEffect2.unk2 += 1;
                if (sUnk_82E149C[gWarioDustEffect2.unk2].time == 0) {
                    gWarioDustEffect2.unk0 = sUnk_82E149C[gWarioDustEffect2.unk2].time;
                }
            }
            pAnimation = GP(gWarioDustEffect2.unk8);
            break;
    }
    if (gWarioDustEffect2.unk0 != 0) {
        gWarioDustEffect2.unk1 += 1;
        xOffset = PIXELS_FROM_SUBPIXELS(gWarioDustEffect2.unk4) - PIXELS_FROM_SUBPIXELS(gBg1XPosition);
        yOffset = PIXELS_FROM_SUBPIXELS(gWarioDustEffect2.unk6) - PIXELS_FROM_SUBPIXELS(gBg1YPosition);
        pAnimation += gWarioDustEffect2.unk2;
        src = GP(pAnimation->oam);
        nextSlot = nextSlot + *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = (attr + yOffset) & 0xFF;

            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + xOffset) & 0x1FF;

            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = priority;

            dest++;
        }
    }

    effectX = 0;
    effectY = 0;
    switch (gCurrentWarioEffect.type - 1) {
        case 0:
            if (gWarioData.horizontalDirection & DPAD_RIGHT) {
                pAnimation = GP(sUnk_82DDCF0.unk0);
            } else {
                pAnimation = GP(sUnk_82DDCF0.unk4);
            }
            if (gWarioData.pose == 4) {
                effectY -= 0x10;
            } else {
                effectY -= 8;
            }
            break;

        case 1:
            pAnimation = GP(sUnk_82DDCF0.unk8);
            if (gCurrentWarioEffect.animationTimer >= sUnk_82E06BC[gCurrentWarioEffect.unk_3].time) {
                gCurrentWarioEffect.animationTimer = 0;
                gCurrentWarioEffect.unk_3 += 1;
                if (sUnk_82E06BC[gCurrentWarioEffect.unk_3].time == 0) {
                    gCurrentWarioEffect.unk_3 = sUnk_82E06BC[gCurrentWarioEffect.unk_3].time;
                }
            }
            break;

        case 2:
            if (0x10 & gWarioData.horizontalDirection) {
                pAnimation = GP(sUnk_82DDCF0.unkC);
            } else {
                pAnimation = GP(sUnk_82DDCF0.unk10);
            }
            if (gCurrentWarioEffect.animationTimer >= sUnk_82E09A4[gCurrentWarioEffect.unk_3].time) {
                gCurrentWarioEffect.animationTimer = 0;
                gCurrentWarioEffect.unk_3 += 1;
                if (sUnk_82E09A4[gCurrentWarioEffect.unk_3].time == 0) {
                    gCurrentWarioEffect.unk_3 = sUnk_82E09A4[gCurrentWarioEffect.unk_3].time;
                }
            }
            break;

        case 3:
            pAnimation = GP(sUnk_82DDCF0.unk14);
            if (gCurrentWarioEffect.animationTimer >= sUnk_82E0BA8[gCurrentWarioEffect.unk_3].time) {
                gCurrentWarioEffect.animationTimer = 0;
                gCurrentWarioEffect.unk_3 += 1;
                if (sUnk_82E0BA8[gCurrentWarioEffect.unk_3].time == 0) {
                    gCurrentWarioEffect.type = sUnk_82E0BA8[gCurrentWarioEffect.unk_3].time;
                }
            }
            break;

        case 4:
            if (0x10 & gWarioData.horizontalDirection) {
                pAnimation = GP(sUnk_82DDCF0.unk18);
            } else {
                pAnimation = GP(sUnk_82DDCF0.unk1C);
            }
            if (gCurrentWarioEffect.animationTimer >= sUnk_82E0CD8[gCurrentWarioEffect.unk_3].time) {
                gCurrentWarioEffect.animationTimer = 0;
                gCurrentWarioEffect.unk_3 += 1;
                if (sUnk_82E0CD8[gCurrentWarioEffect.unk_3].time == 0) {
                    gCurrentWarioEffect.unk_3 = sUnk_82E0CD8[gCurrentWarioEffect.unk_3].time;
                }
            }
            break;

        case 5:
            if (0x10 & gWarioData.horizontalDirection) {
                pAnimation = GP(sUnk_82DDCF0.unk20);
            } else {
                pAnimation = GP(sUnk_82DDCF0.unk24);
            }
            if (gCurrentWarioEffect.animationTimer >= sUnk_82E1598[gCurrentWarioEffect.unk_3].time) {
                gCurrentWarioEffect.animationTimer = 0;
                gCurrentWarioEffect.unk_3 += 1;
                if (sUnk_82E1598[gCurrentWarioEffect.unk_3].time == 0) {
                    gCurrentWarioEffect.unk_3 = sUnk_82E1598[gCurrentWarioEffect.unk_3].time;
                }
            }
            if (gWarioData.pose == 0x3B) {
                if ((gWarioData.horizontalDirection & DPAD_RIGHT)) {
                    effectX -= 2;
                } else {
                    effectX += 2;
                }
                effectY += 0x14;
            }
            break;

        case 6:
            pAnimation = GP(sUnk_82DDCF0.unk28);
            if (gCurrentWarioEffect.animationTimer >= sUnk_82E1658[gCurrentWarioEffect.unk_3].time) {
                gCurrentWarioEffect.animationTimer = 0;
                gCurrentWarioEffect.unk_3 += 1;
                if (sUnk_82E1658[gCurrentWarioEffect.unk_3].time == 0) {
                    gCurrentWarioEffect.unk_3 = sUnk_82E1658[gCurrentWarioEffect.unk_3].time;
                }
            }
            break;

        case 7:
            pAnimation = GP(sUnk_82DDCF0.unk2C);
            if (gCurrentWarioEffect.animationTimer >= sUnk_82E1690[gCurrentWarioEffect.unk_3].time) {
                gCurrentWarioEffect.animationTimer = 0;
                gCurrentWarioEffect.unk_3 += 1;
                if (sUnk_82E1690[gCurrentWarioEffect.unk_3].time == 0) {
                    gCurrentWarioEffect.unk_3 = sUnk_82E1690[gCurrentWarioEffect.unk_3].time;
                }
            }
            break;
    }

    if (gCurrentWarioEffect.type != 0) {
        if (gWarioPauseTimer == 0) {
            gCurrentWarioEffect.animationTimer += 1;
        }
        xOffset = PIXELS_FROM_SUBPIXELS(gWarioData.xPosition) - PIXELS_FROM_SUBPIXELS(gBg1XPosition) + effectX;
        yOffset = PIXELS_FROM_SUBPIXELS(gWarioData.yPosition) - PIXELS_FROM_SUBPIXELS(gBg1YPosition) + effectY;
        pAnimation += gCurrentWarioEffect.unk_3;
        src = GP(pAnimation->oam);
        nextSlot = nextSlot + *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = (attr + yOffset) & 0xFF;

            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + xOffset) & 0x1FF;

            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = priority;

            dest++;
        }
    }
    gOamSlotsUsed = nextSlot;
}

s32 GameScreenFadeIn(void)
{
    s32 result;

    result = 0;
    switch (gColorFading.type - 1) {
        case 0:
        case 1:
            result = UpdatePaletteFadeStep(0, 0);
            break;

        case 2:
        case 3:
            if (gBldy != 0) {
                gBldy -= 1;
            } else {
                result = 1;
            }
            break;

        case 4:
            result = ExpandColorFadeWindow();
    }
    return result;
}

s32 GameScreenFadeOut(void)
{
    s32 result;

    result = 0;
    switch (gColorFading.type - 1) {
        case 0:
        case 1:
            result = UpdatePaletteFadeStep(1, 0);
            break;
        case 2:
        case 3:
            if (gBldy <= 15) {
                gBldy += 1;
            } else {
                result = 1;
            }
            break;
        case 4:
            result = CollapseColorFadeWindowAroundWario();
    }
    return result;
}

void RenderPauseScreenOam(void)
{
    const struct AnimationFrame* animation;
    const u16* src;
    u16* dest;
    u16 attr;
    s32 currentSlot;
    s32 nextSlot;
    s32 i;
    u32 keyzerState;
    u32 cdState;
    s32 score;

    currentSlot = 0;
    nextSlot = gOamSlotsUsed;
    dest = (u16*)&gOamBuffer[nextSlot];

    gPauseScreenState.animationTimer++;
    if (gPauseScreenAnimation[gPauseScreenState.animationFrame].time < gPauseScreenState.animationTimer) {
        gPauseScreenState.animationTimer = 1;
        gPauseScreenState.animationFrame++;
        if (gPauseScreenAnimation[gPauseScreenState.animationFrame].time == 0) {
            gPauseScreenState.animationFrame = 0;
            if (gPauseScreenAnimation == sUnk_86D3F58) {
                gPauseScreenState.animationFrame = 1;
            }
        }
    }
    src = gPauseScreenAnimation[gPauseScreenState.animationFrame].oam;
    nextSlot += *(src++);
    if (nextSlot > 128) {
        return;
    }
    for (; currentSlot < nextSlot; currentSlot++) {
        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.y = attr + sUnk_86D36B8[gPauseScreenState.selection];
        attr = *(src++);
        *(dest++) = attr;
        gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36A4[gPauseScreenState.selection]) & 0x1FF;
        *(dest++) = *(src++);
        gOamBuffer[currentSlot].split.priority = 0;
        dest++;
    }

    if ((u8)(gPauseScreenState.selection - 3) <= 1) {
        if (gLanguage == 0) {
            animation = sUnk_86D370C[0][gPauseScreenState.selection - 3];
        } else {
            animation = sUnk_86D370C[1][gPauseScreenState.selection - 3];
        }
        src = animation->oam;
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 80;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 120) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 176;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 203;
    gOamBuffer[currentSlot].split.paletteNum = 6;
    currentSlot++;
    dest++;

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 184;
    gOamBuffer[currentSlot].split.size = 0;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 132;
    gOamBuffer[currentSlot].split.paletteNum = 4;
    currentSlot++;
    dest++;

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 200;
    gOamBuffer[currentSlot].split.size = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 134;
    gOamBuffer[currentSlot].split.paletteNum = 4;
    currentSlot++;
    dest++;

    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 8;
    gOamBuffer[currentSlot].split.size = 2;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 144;
    gOamBuffer[currentSlot].split.paletteNum = 7;
    currentSlot++;
    dest++;

    nextSlot += 5;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.y = 32;
    gOamBuffer[currentSlot].split.shape = 1;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.x = 40;
    gOamBuffer[currentSlot].split.size = 2;
    *(dest++) = 0;
    gOamBuffer[currentSlot].split.tileNum = 148;
    gOamBuffer[currentSlot].split.paletteNum = 7;
    currentSlot++;
    dest++;

    for (i = 0; i < 4; i++) {
        if (gPauseJewelPieceStates[i] == 2) {
            animation = sUnk_86D36FC[i];
            gPauseJewelAnimationStates[i].animationTimer++;
            if (animation[gPauseJewelAnimationStates[i].animationFrame].time <
                gPauseJewelAnimationStates[i].animationTimer) {
                gPauseJewelAnimationStates[i].animationTimer = 1;
                gPauseJewelAnimationStates[i].animationFrame++;
                if (animation[gPauseJewelAnimationStates[i].animationFrame].time == 0) {
                    gPauseJewelAnimationStates[i].animationFrame = 0;
                }
            }
            src = animation[gPauseJewelAnimationStates[i].animationFrame].oam;
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 88;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36CC[gUnk_3003C4A]) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
        if (gPauseAllJewelPiecesCollected == 0 && gPauseJewelPieceStates[i] != 0) {
            animation = sUnk_86D36DC[gPauseJewelPieceStates[i] - 1][i];
            src = animation->oam;
            nextSlot += *(src++);
            if (nextSlot > 128) {
                return;
            }
            for (; currentSlot < nextSlot; currentSlot++) {
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.y = attr + 88;
                attr = *(src++);
                *(dest++) = attr;
                gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36CC[gUnk_3003C4A]) & 0x1FF;
                *(dest++) = *(src++);
                gOamBuffer[currentSlot].split.priority = 0;
                dest++;
            }
        }
    }

    if (gPauseAllJewelPiecesCollected != 0) {
        src = sUnk_86D3DB0->oam;
        nextSlot += *(src++);
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 88;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36CC[gUnk_3003C4A]) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    keyzerState = gCollectedKeyzer;
    if (keyzerState == 1 || gCurrentCollection[gCurrentPassage][gCurrentStageNumber].keyzer) {
        animation = sUnk_86D3D08;
        if (keyzerState == 1) {
            animation = sUnk_86D3CF8;
        }
        gPauseKeyzerAnimationState.animationTimer++;
        if (animation[gPauseKeyzerAnimationState.animationFrame].time < gPauseKeyzerAnimationState.animationTimer) {
            gPauseKeyzerAnimationState.animationTimer = 1;
            gPauseKeyzerAnimationState.animationFrame++;
            if (animation[gPauseKeyzerAnimationState.animationFrame].time == 0) {
                gPauseKeyzerAnimationState.animationFrame = 0;
            }
        }
        src = animation[gPauseKeyzerAnimationState.animationFrame].oam;
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 96;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + sUnk_86D36D4[gUnk_3003C4A]) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    cdState = (u8)(gCollectedCD - 1);
    if ((cdState <= 1 || gCurrentCollection[gCurrentPassage][gCurrentStageNumber].cd) && gUnk_3003C4A == 0) {
        animation = sUnk_86D3FD0;
        if (cdState <= 1) {
            animation = sUnk_86D3F80;
        }
        gPauseCdAnimationState.animationTimer++;
        if (animation[gPauseCdAnimationState.animationFrame].time < gPauseCdAnimationState.animationTimer) {
            gPauseCdAnimationState.animationTimer = 1;
            gPauseCdAnimationState.animationFrame++;
            if (animation[gPauseCdAnimationState.animationFrame].time == 0) {
                gPauseCdAnimationState.animationFrame = 0;
            }
        }
        src = animation[gPauseCdAnimationState.animationFrame].oam;
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 88;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 200) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }

    score = gHighScoreTable[gCurrentPassage][gCurrentStageNumber];
    if (score > 599) {
        if (score > 999) {
            src = sUnk_86D4008->oam;
        } else if (score > 799) {
            src = sUnk_86D4018->oam;
        } else {
            src = sUnk_86D4028->oam;
        }
        nextSlot += *(src++);
        if (nextSlot > 128) {
            return;
        }
        for (; currentSlot < nextSlot; currentSlot++) {
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.y = attr + 48;
            attr = *(src++);
            *(dest++) = attr;
            gOamBuffer[currentSlot].split.x = (attr + 168) & 0x1FF;
            *(dest++) = *(src++);
            gOamBuffer[currentSlot].split.priority = 0;
            dest++;
        }
    }
    gOamSlotsUsed = nextSlot;
}

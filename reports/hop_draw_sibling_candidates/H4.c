void WarioHopDrawSprites(void)
{
    /* OamData merges attr1 bits 9..13 into one 5-bit matrixNum. The affine sprites
       here only write the 3-bit affine index (as in DrawSoundRoomSprites). */
    typedef union {
        struct {
            u32 y : 8;
            u32 affineMode : 2;
            u32 objMode : 2;
            u32 mosaic : 1;
            u32 bpp : 1;
            u32 shape : 2;

            u32 x : 9;
            u32 affineIndex : 3;
            u32 preserved : 2;
            u32 size : 2;

            u16 tileNum : 10;
            u16 priority : 2;
            u16 paletteNum : 4;

            u16 fractional : 8;
            u16 integer : 7;
            u16 sign : 1;
        } split;

        struct {
            u16 attr0;
            u16 attr1;
            u16 attr2;
            u16 affineParam;
        } all;
    } WarioHopOamData;

    const struct AnimationFrame* animation;
    struct WarioHopObjState* shadow;
    struct WarioHopSmallState* obstacles;
    const u16* src;
    u16* raw;
    u16 attr;
    s32 i;
    s32 total;
    s32 j;
    u32 active;
    s16 matrix[4];
    s16* matrix1Ptr;
    s16* matrix2Ptr;
    s16* matrix3Ptr;
    u16 matrixD;
    s32 matrixD2;

    i = 0;
    total = gOamSlotsUsed;
    raw = (u16*)&gOamBuffer[total];
    active = gMinigameCamera.active;
    matrix1Ptr = &matrix[1];
    matrix2Ptr = matrix1Ptr + 1;
    matrix3Ptr = matrix2Ptr + 1;

    if (active != 0) {
        if (active == 1) {
            src = sUnk_870D880;
        }
        if (active == 2) {
            src = sUnk_870D890;
        }
        total += *(src++);
        for (; i < total; i++) {
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.y = gMinigameCamera.yOffset + attr - 16;
            gOamBuffer[i].split.affineMode = 3;
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.x = (attr + (s16)gMinigameCamera.xOffset - 32) & 0x1FF;
            ((WarioHopOamData*)gOamBuffer)[i].split.affineIndex = 1;
            *(raw++) = *(src++);
            gOamBuffer[i].split.priority = 0;
            raw++;
        }
    }

    if (gMinigameMedalHighlight != 0) {
        gMinigameUiAnimation[0]++;
        if (sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].time < gMinigameUiAnimation[0]) {
            gMinigameUiAnimation[0] = 1;
            gMinigameUiAnimation[1]++;
            if (sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].time == 0) {
                gMinigameUiAnimation[1] = 0;
                gMinigameUiAnimation[0] = 0;
                gMinigameMedalHighlight = 0;
                WarioHopDrawScoreDigits();
            }
        }
    }
    src = sWarioHopMedalHighlightOam[gMinigameUiAnimation[1]].oam;
    total += *(src++);
    shadow = &gWarioHopPlayerShadow;
    for (; i < total; i++) {
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.y = attr + 24;
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.x = (attr + 29) & 0x1FF;
        *(raw++) = *(src++);
        gOamBuffer[i].split.priority = 0;
        raw++;
    }

    animation = sUnk_870CE14[gWarioHopSpeedLevel];
    if (gMinigameState <= 2) {
        shadow->animTimer++;
    }
    if (animation[shadow->animFrame].time < shadow->animTimer) {
        shadow->animTimer = 0;
        shadow->animFrame++;
        if (animation[shadow->animFrame].time == 0) {
            shadow->animFrame = 0;
        }
    }
    src = animation[shadow->animFrame].oam;
    total += *(src++);
    for (; i < total; i++) {
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.y = ((s16)shadow->y >> 4) + attr;
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.x = (attr + ((s16)shadow->x >> 4)) & 0x1FF;
        *(raw++) = *(src++);
        gOamBuffer[i].split.priority = 1;
        raw++;
    }

    src = sWarioHopPlayerOam->oam;
    total += *(src++);
    for (; i < total; i++) {
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.y = ((s16)gWarioHopPlayer.y >> 4) + attr;
        gOamBuffer[i].split.affineMode = 1;
        attr = *(src++);
        *(raw++) = attr;
        gOamBuffer[i].split.x = (attr + ((s16)gWarioHopPlayer.x >> 4)) & 0x1FF;
        ((WarioHopOamData*)gOamBuffer)[i].split.affineIndex = 0;
        *(raw++) = *(src++);
        gOamBuffer[i].split.priority = 1;
        raw++;
    }

    if (gWarioHopJumpState == 0) {
        gWarioHopRollingObject[3]++;
        if (sWarioHopRollingObjectOam[gWarioHopRollingObject[4]].time < gWarioHopRollingObject[3]) {
            gWarioHopRollingObject[3] = 0;
            gWarioHopRollingObject[4]++;
            if (sWarioHopRollingObjectOam[gWarioHopRollingObject[4]].time == 0) {
                gWarioHopRollingObject[4] = 0;
            }
        }
        src = sWarioHopRollingObjectOam[gWarioHopRollingObject[4]].oam;
        total += *(src++);
        for (; i < total; i++) {
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.y = ((s16)gWarioHopRollingObject[1] >> 4) + attr;
            attr = *(src++);
            *(raw++) = attr;
            gOamBuffer[i].split.x = (attr + ((s16)gWarioHopRollingObject[0] >> 4)) & 0x1FF;
            *(raw++) = *(src++);
            gOamBuffer[i].split.priority = 1;
            raw++;
        }
    }

    for (j = 0; j < 7; j++) {
        if (((struct WarioHopSmallState*)0x03004950)[j].type != 0) {
            animation = sUnk_870CE24[((struct WarioHopSmallState*)0x03004950)[j].type - 1];
            ((struct WarioHopSmallState*)0x03004950)[j].animTimer++;
            if (animation[((struct WarioHopSmallState*)0x03004950)[j].animFrame].time <
                ((struct WarioHopSmallState*)0x03004950)[j].animTimer) {
                ((struct WarioHopSmallState*)0x03004950)[j].animTimer = 0;
                ((struct WarioHopSmallState*)0x03004950)[j].animFrame++;
                if (animation[((struct WarioHopSmallState*)0x03004950)[j].animFrame].time == 0) {
                    ((struct WarioHopSmallState*)0x03004950)[j].animFrame = 0;
                }
            }
            obstacles = gWarioHopObstacles;
            src = animation[obstacles[j].animFrame].oam;
            total += *(src++);
            for (; i < total; i++) {
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.y = ((s16)obstacles[j].y >> 4) + attr;
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.x = (attr + ((s16)obstacles[j].x >> 4)) & 0x1FF;
                *(raw++) = *(src++);
                gOamBuffer[i].split.priority = 1;
                raw++;
            }
        }
    }

    if (gWarioHopBonusEffect.type != 0) {
        gWarioHopBonusEffect.type++;
        gWarioHopBonusEffect.animTimer++;
        if (gWarioHopBonusEffect.type > 201) {
            gWarioHopBonusEffect.type = 0;
        }
        if (sWarioHopBonusEffectOam[gWarioHopBonusEffect.animFrame].time < gWarioHopBonusEffect.animTimer) {
            gWarioHopBonusEffect.animTimer = 1;
            gWarioHopBonusEffect.animFrame++;
            if (sWarioHopBonusEffectOam[gWarioHopBonusEffect.animFrame].time == 0) {
                gWarioHopBonusEffect.animFrame = 0;
            }
        }
        if (gWarioHopBonusEffect.type != 0) {
            src = sWarioHopBonusEffectOam[gWarioHopBonusEffect.animFrame].oam;
            total += *(src++);
            for (; i < total; i++) {
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.y = sUnk_870CCE0[gWarioHopBonusEffect.type - 2] + attr;
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.x = (attr + sUnk_870CC18[gWarioHopBonusEffect.type - 2]) & 0x1FF;
                *(raw++) = *(src++);
                gOamBuffer[i].split.priority = 2;
                raw++;
            }
        }
    }

    if (gWarioHopMedalEffect.type != 0) {
        gWarioHopMedalEffect.animTimer++;
        if (sWarioHopMedalEffectOam[gWarioHopMedalEffect.animFrame].time < gWarioHopMedalEffect.animTimer) {
            gWarioHopMedalEffect.animTimer = 1;
            gWarioHopMedalEffect.animFrame++;
            if (sWarioHopMedalEffectOam[gWarioHopMedalEffect.animFrame].time == 0) {
                gWarioHopMedalEffect.animFrame = 0;
                gWarioHopMedalEffect.animTimer = 0;
                gWarioHopMedalEffect.type = 0;
            }
        }
        if (gWarioHopMedalEffect.type != 0) {
            src = sWarioHopMedalEffectOam[gWarioHopMedalEffect.animFrame].oam;
            total += *(src++);
            for (; i < total; i++) {
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.y = attr + 140;
                attr = *(src++);
                *(raw++) = attr;
                gOamBuffer[i].split.x = (attr + 160) & 0x1FF;
                *(raw++) = *(src++);
                gOamBuffer[i].split.priority = 3;
                raw++;
            }
        }
    }

    matrix[0] = FixedMul(sSinCosTable[gWarioHopPlayer.angle + 64], (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    *matrix1Ptr = FixedMul(sSinCosTable[gWarioHopPlayer.angle], (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    *matrix2Ptr =
        FixedMul((s16)(-(u16)sSinCosTable[gWarioHopPlayer.angle]), (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    matrixD = FixedMul(sSinCosTable[gWarioHopPlayer.angle + 64], (s16)FixedInverse((s16)gWarioHopPlayer.scale));
    *matrix3Ptr = matrixD;
    gOamBuffer[0].all.affineParam = matrix[0];
    gOamBuffer[1].all.affineParam = *matrix1Ptr;
    gOamBuffer[2].all.affineParam = *matrix2Ptr;
    gOamBuffer[3].all.affineParam = matrixD;

    matrix[0] = FixedMul(sSinCosTable[64], (s16)FixedInverse((s16)gMinigameCamera.angle));
    *matrix1Ptr = FixedMul(sSinCosTable[0], (s16)FixedInverse((s16)gMinigameCamera.angle));
    *matrix2Ptr = FixedMul((s16)(-(u16)sSinCosTable[0]), (s16)FixedInverse((s16)gMinigameCamera.angle));
    matrixD2 = FixedMul(sSinCosTable[64], (s16)FixedInverse((s16)gMinigameCamera.angle));
    *matrix3Ptr = matrixD2;
    gOamBuffer[4].all.affineParam = matrix[0];
    gOamBuffer[5].all.affineParam = *matrix1Ptr;
    gOamBuffer[6].all.affineParam = *matrix2Ptr;
    gOamBuffer[7].all.affineParam = matrixD2;

    gOamSlotsUsed = total;
}

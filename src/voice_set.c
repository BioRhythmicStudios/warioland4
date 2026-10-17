#include "voice_set.h"
#include "minigame.h"
#include "sound.h"

const s32 sVoiceSet_Wario_ReactionEnd[8];
const s32 sVoiceSet_Wario_Treasure[18];
const s32 sVoiceSet_Wario_KeyzerGet[7];
const s32 sVoiceSet_Wario_VortexOpen[15];
const s32 sVoiceSet_Wario_Hurt[9];
const s32 sVoiceSet_Wario_HurryUp[5];
const s32 sVoiceSet_Wario_Screams[5];
const s32 sVoiceSet_Wario_Goodbye_Unused[5];
const s32 sVoiceSet_Wario_EnteringPyramid[13];
const s32 sVoiceSet_Wario_WorkoutEnd[7];
const s32 sVoiceSet_Wario_EnteringBossRoom[6];
const s32 sVoiceSet_Wario_Dying[8];
const s32 sVoiceSet_12[3];
const s32 sVoiceSet_13[2];
const s32 sVoiceSet_14[2];
const s32 sVoiceSet_15[2];
const s32 sVoiceSet_16[2];
const s32 sVoiceSet_17[2];
const s32 sVoiceSet_18[19];
const s32 sVoiceSet_19[3];
const s32 sVoiceSet_20[3];
const s32 sVoiceSet_21[3];
const s32 sVoiceSet_22[3];

#ifndef PORTABLE
const s32 sVoiceSetSizes[VS_COUNT] = {
    [VS_WARIO_REACTION_END] = ARRAY_SIZE(sVoiceSet_Wario_ReactionEnd),
    [VS_WARIO_TREASURE] = ARRAY_SIZE(sVoiceSet_Wario_Treasure),
    [VS_WARIO_KEYZER_GET] = ARRAY_SIZE(sVoiceSet_Wario_KeyzerGet),
    [VS_WARIO_VORTEX_OPEN] = ARRAY_SIZE(sVoiceSet_Wario_VortexOpen),
    [VS_WARIO_HURT] = ARRAY_SIZE(sVoiceSet_Wario_Hurt),
    [VS_WARIO_HURRY_UP] = ARRAY_SIZE(sVoiceSet_Wario_HurryUp),
    [VS_WARIO_SCREAMS] = ARRAY_SIZE(sVoiceSet_Wario_Screams),
    [VS_WARIO_GOODBYE_UNUSED] = ARRAY_SIZE(sVoiceSet_Wario_Goodbye_Unused),
    [VS_WARIO_ENTERING_PYRAMID] = ARRAY_SIZE(sVoiceSet_Wario_EnteringPyramid),
    [VS_WARIO_WORKOUT_END] = ARRAY_SIZE(sVoiceSet_Wario_WorkoutEnd),
    [VS_WARIO_ENTERING_BOSS_ROOM] = ARRAY_SIZE(sVoiceSet_Wario_EnteringBossRoom),
    [VS_WARIO_DYING] = ARRAY_SIZE(sVoiceSet_Wario_Dying),
    [VS_12] = ARRAY_SIZE(sVoiceSet_12),
    [VS_13] = ARRAY_SIZE(sVoiceSet_13),
    [VS_14] = ARRAY_SIZE(sVoiceSet_14),
    [VS_15] = ARRAY_SIZE(sVoiceSet_15),
    [VS_16] = ARRAY_SIZE(sVoiceSet_16),
    [VS_17] = ARRAY_SIZE(sVoiceSet_17),
    [VS_18] = ARRAY_SIZE(sVoiceSet_18),
    [VS_19] = ARRAY_SIZE(sVoiceSet_19),
    [VS_20] = ARRAY_SIZE(sVoiceSet_20),
    [VS_21] = ARRAY_SIZE(sVoiceSet_21),
    [VS_22] = ARRAY_SIZE(sVoiceSet_22),
};

const s32 sVoiceSet_Wario_ReactionEnd[] = {
    SE_WARIO_HEY,
    SE_WARIO_HEY_HEY,
    SE_WARIO_HA,
    SE_WARIO_HE_HEHEHEHE,
    SE_WARIO_OK_2,
    SE_WARIO_HEY_GO,
    SE_WARIO_WOW_2,
    SE_WARIO_WOWWW,
};

const s32 sVoiceSet_Wario_Treasure[] = {
    SE_WARIO_YAHOO_3,
    SE_WARIO_YAHOO_4,
    SE_WARIO_EXCELLENT_2,
    SE_WARIO_ALRIGHT_1,
    SE_WARIO_ALRIGHT_2,
    SE_WARIO_HAHAHA,
    SE_WARIO_HA_HAHAHAHA_1,
    SE_WARIO_HA_HAHAHAHA_2,
    SE_WARIO_OK_1,
    SE_WARIO_OK_3,
    SE_WARIO_YEAH,
    SE_WARIO_HEHEHE_HEY_GO,
    SE_WARIO_HEY_GO,
    SE_WARIO_WOW_GO,
    SE_WARIO_WAH,
    SE_WARIO_WOW_1,
    SE_WARIO_WOW_2,
    SE_WARIO_WOWWW,
};

const s32 sVoiceSet_Wario_KeyzerGet[] = {
    SE_WARIO_YAHOO_1,
    SE_WARIO_YAHOO_2,
    SE_WARIO_YAHOO_5,
    SE_WARIO_EXCELLENT_1,
    SE_WARIO_EEE_EXCELLENT,
    SE_WARIO_MAMA_MIA_1,
    SE_WARIO_MAMA_MIA_2,
};

const s32 sVoiceSet_Wario_VortexOpen[] = {
    SE_WARIO_HERE_WE_GO,
    SE_WARIO_HERE_I_GO_1,
    SE_WARIO_HERE_I_GO_2,
    SE_WARIO_ALRIGHT_1,
    SE_WARIO_ALRIGHT_2,
    SE_WARIO_GOOD_LUCK,
    SE_WARIO_GOGOGO_GOOD_LUCK,
    SE_WARIO_OK_1,
    SE_WARIO_OK_2,
    SE_WARIO_OK_3,
    SE_WARIO_YEAH,
    SE_WARIO_HEHEHE_HEY_GO,
    SE_WARIO_HEY_GO,
    SE_WARIO_WOW_GO,
    SE_WARIO_WOW_1,
};

const s32 sVoiceSet_Wario_Hurt[] = {
    SE_WARIO_OH_BOY,
    SE_WARIO_AW_BOY,
    SE_WARIO_HEY,
    SE_WARIO_HEY_HEY,
    SE_WARIO_HE_HEHEHEHE,
    SE_WARIO_YEAH,
    SE_WARIO_NONO,
    SE_WARIO_ARR_NONO,
    SE_WARIO_NO_NO,
};

const s32 sVoiceSet_Wario_HurryUp[] = {
    SE_WARIO_HURRY_UP_1,
    SE_WARIO_HURRY_UP_2,
    SE_WARIO_HURRY_UP_3,
    SE_WARIO_HUHUHU_HURRY_UP,
    SE_WARIO_NONO_HURRY_UP,
};

const s32 sVoiceSet_Wario_Screams[] = {
    SE_WARIO_WAAAAA_1,
    SE_WARIO_WAAAAA_2,
    SE_WARIO_WAAAAA_3,
    SE_WARIO_EEEEEH_1,
    SE_WARIO_EEEEEH_2,
};

const s32 sVoiceSet_Wario_Goodbye_Unused[] = {
    SE_WARIO_YAHOO_2,
    SE_WARIO_YAHOO_4,
    SE_WARIO_GOODBYE_GOODBYE,
    SE_WARIO_BYBYBY_BYE_BYE,
    SE_WARIO_BYE_BYE,
};

const s32 sVoiceSet_Wario_EnteringPyramid[] = {
    SE_WARIO_MAMA_MIA_1,
    SE_WARIO_MAMA_MIA_2,
    SE_WARIO_HERE_WE_GO,
    SE_WARIO_HERE_I_GO_1,
    SE_WARIO_HERE_I_GO_2,
    SE_WARIO_ALRIGHT_1,
    SE_WARIO_ALRIGHT_2,
    SE_WARIO_HEY,
    SE_WARIO_HEY_HEY,
    SE_WARIO_OK_1,
    SE_WARIO_OK_2,
    SE_WARIO_OK_3,
    SE_WARIO_YEAH,
};

const s32 sVoiceSet_Wario_WorkoutEnd[] = {
    SE_WARIO_ALRIGHT_1,
    SE_WARIO_ALRIGHT_2,
    SE_WARIO_SORRY,
    SE_WARIO_SOSOSO_SORRY,
    SE_WARIO_SORRYYY,
    SE_WARIO_WOW_2,
    SE_WARIO_WOWWW,
};

const s32 sVoiceSet_Wario_EnteringBossRoom[] = {
    SE_WARIO_OH_BOY,
    SE_WARIO_AW_BOY,
    SE_WARIO_HEY,
    SE_WARIO_HEY_HEY,
    SE_WARIO_HE_HEHEHEHE,
    SE_WARIO_YEAH,
};

const s32 sVoiceSet_Wario_Dying[] = {
    SE_WARIO_OH_BOY,
    SE_WARIO_AW_BOY,
    SE_WARIO_HEY,
    SE_WARIO_HE_HEHEHEHE,
    SE_WARIO_YEAH,
    SE_WARIO_NONO,
    SE_WARIO_ARR_NONO,
    SE_WARIO_NO_NO,
};

const s32 sVoiceSet_12[] = {
    SOUND_93,
    SOUND_85,
    SOUND_86,
};

const s32 sVoiceSet_13[] = {
    SOUND_AC,
    SOUND_87,
};

const s32 sVoiceSet_14[] = {
    SOUND_B3,
    SOUND_88,
};

const s32 sVoiceSet_15[] = {
    SOUND_99,
    SOUND_89,
};

const s32 sVoiceSet_16[] = {
    SOUND_9F,
    SOUND_8A,
};

const s32 sVoiceSet_17[] = {
    SOUND_C8,
    SOUND_8B,
};

const s32 sVoiceSet_18[] = {
    SOUND_208, SOUND_209, SOUND_20A, SOUND_20B, SOUND_20C, SOUND_20D, SOUND_20E, SOUND_20F, SOUND_210, SOUND_211,
    SOUND_212, SOUND_213, SOUND_214, SOUND_215, SOUND_216, SOUND_217, SOUND_218, SOUND_219, SOUND_21A,
};

const s32 sVoiceSet_19[] = {
    SOUND_1DD,
    SOUND_1DE,
    SOUND_1DF,
};

const s32 sVoiceSet_20[] = {
    SOUND_1E0,
    SOUND_1E1,
    SOUND_1E2,
};

const s32 sVoiceSet_21[] = {
    SOUND_1D6,
    SOUND_1E4,
    SOUND_1E5,
};

const s32 sVoiceSet_22[] = {
    SOUND_1E6,
    SOUND_1E7,
    SOUND_1E8,
};

const s32* const sVoiceSetPointers[VS_COUNT] = {
    [VS_WARIO_REACTION_END] = sVoiceSet_Wario_ReactionEnd,
    [VS_WARIO_TREASURE] = sVoiceSet_Wario_Treasure,
    [VS_WARIO_KEYZER_GET] = sVoiceSet_Wario_KeyzerGet,
    [VS_WARIO_VORTEX_OPEN] = sVoiceSet_Wario_VortexOpen,
    [VS_WARIO_HURT] = sVoiceSet_Wario_Hurt,
    [VS_WARIO_HURRY_UP] = sVoiceSet_Wario_HurryUp,
    [VS_WARIO_SCREAMS] = sVoiceSet_Wario_Screams,
    [VS_WARIO_GOODBYE_UNUSED] = sVoiceSet_Wario_Goodbye_Unused,
    [VS_WARIO_ENTERING_PYRAMID] = sVoiceSet_Wario_EnteringPyramid,
    [VS_WARIO_WORKOUT_END] = sVoiceSet_Wario_WorkoutEnd,
    [VS_WARIO_ENTERING_BOSS_ROOM] = sVoiceSet_Wario_EnteringBossRoom,
    [VS_WARIO_DYING] = sVoiceSet_Wario_Dying,
    [VS_12] = sVoiceSet_12,
    [VS_13] = sVoiceSet_13,
    [VS_14] = sVoiceSet_14,
    [VS_15] = sVoiceSet_15,
    [VS_16] = sVoiceSet_16,
    [VS_17] = sVoiceSet_17,
    [VS_18] = sVoiceSet_18,
    [VS_19] = sVoiceSet_19,
    [VS_20] = sVoiceSet_20,
    [VS_21] = sVoiceSet_21,
    [VS_22] = sVoiceSet_22,
};
#else
extern const s32 sVoiceSetSizes[VS_COUNT];
extern const s32 sVoiceSet_Wario_ReactionEnd[];
extern const s32 sVoiceSet_Wario_Treasure[];
extern const s32 sVoiceSet_Wario_KeyzerGet[];
extern const s32 sVoiceSet_Wario_VortexOpen[];
extern const s32 sVoiceSet_Wario_Hurt[];
extern const s32 sVoiceSet_Wario_HurryUp[];
extern const s32 sVoiceSet_Wario_Screams[];
extern const s32 sVoiceSet_Wario_Goodbye_Unused[];
extern const s32 sVoiceSet_Wario_EnteringPyramid[];
extern const s32 sVoiceSet_Wario_WorkoutEnd[];
extern const s32 sVoiceSet_Wario_EnteringBossRoom[];
extern const s32 sVoiceSet_Wario_Dying[];
extern const s32 sVoiceSet_12[];
extern const s32 sVoiceSet_13[];
extern const s32 sVoiceSet_14[];
extern const s32 sVoiceSet_15[];
extern const s32 sVoiceSet_16[];
extern const s32 sVoiceSet_17[];
extern const s32 sVoiceSet_18[];
extern const s32 sVoiceSet_19[];
extern const s32 sVoiceSet_20[];
extern const s32 sVoiceSet_21[];
extern const s32 sVoiceSet_22[];
extern GBA_PTR(const s32*) const sVoiceSetPointers[VS_COUNT];
#endif

void VoiceSetPlay(s32 voiceSet)
{
    s32 chosenSound;

    if (voiceSet >= VS_COUNT) {
        return;
    }
    switch (voiceSet) {
        case VS_WARIO_REACTION_END:
        case VS_WARIO_TREASURE:
        case VS_WARIO_KEYZER_GET:
        case VS_WARIO_VORTEX_OPEN:
        case VS_WARIO_HURT:
        case VS_WARIO_SCREAMS:
        case VS_WARIO_GOODBYE_UNUSED:
        case VS_WARIO_ENTERING_PYRAMID:
        case VS_WARIO_WORKOUT_END:
        case VS_WARIO_ENTERING_BOSS_ROOM:
        case VS_WARIO_DYING:
            // Don't let Wario interrupt himself if already speaking (excl. "Hurry up!" lines)
            if (gMPlayInfo_3.status & 0xF) {
                return;
            }
    }
    chosenSound = MinigameRandom() % sVoiceSetSizes[voiceSet];
    m4aSongNumStart(GP(sVoiceSetPointers[voiceSet])[chosenSound]);
}

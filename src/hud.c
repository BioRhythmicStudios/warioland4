#include "main.h"
#include "score.h"
#include "sprite_ai/chance_wheel.h"
#include "tile_effect.h"
#include "wario.h"
#ifdef PORTABLE
#include "port/rumble_snapshot.h"
#endif

extern const u8 sBossLife20Gfx[];
extern const u8 sBossLife19Gfx[];
extern const u8 sBossLife18Gfx[];
extern const u8 sBossLife17Gfx[];
extern const u8 sBossLife16Gfx[];
extern const u8 sBossLife15Gfx[];
extern const u8 sBossLife14Gfx[];
extern const u8 sBossLife13Gfx[];
extern const u8 sBossLife12Gfx[];
extern const u8 sBossLife11Gfx[];
extern const u8 sBossLife10Gfx[];
extern const u8 sBossLife9Gfx[];
extern const u8 sBossLife8Gfx[];
extern const u8 sBossLife7Gfx[];
extern const u8 sBossLife6Gfx[];
extern const u8 sBossLife5Gfx[];
extern const u8 sBossLife4Gfx[];
extern const u8 sBossLife3Gfx[];
extern const u8 sBossLife2Gfx[];
extern const u8 sBossLife1Gfx[];
extern const u8 sBossLife0Gfx[];

extern const u8 sUnk_83BF986[];
extern const u8 sBigBoardSpaceSpriteTable[];

#ifndef PORTABLE
const u16 sCommonSpritesPal[] = {
#include "data/common_sprites.pal.inc"
};
const u8 sCommonSpritesGfx[] = {
#include "data/common_sprites.4bpp.inc"
};

const u8 sScoreDigitsGfx[] = {
#include "data/hud/score_digits.4bpp.inc"
};

const u8 sTimeDigitsUpperGfx[] = {
#include "data/hud/time_digits_upper.4bpp.inc"
};
const u8 sTimeColonUpperGfx[] = {
#include "data/hud/time_colon_upper.4bpp.inc"
};
const u8 sTimeColonEmptyUpperGfx[] = {
#include "data/hud/time_colon_empty_upper.4bpp.inc"
};
const u8 sTimeDigitsLowerGfx[] = {
#include "data/hud/time_digits_lower.4bpp.inc"
};
const u8 sTimeColonLowerGfx[] = {
#include "data/hud/time_colon_lower.4bpp.inc"
};
const u8 sTimeColonEmptyLowerGfx[] = {
#include "data/hud/time_colon_empty_lower.4bpp.inc"
};

const u8 sTimeDigitsYellowUpperGfx[] = {
#include "data/hud/time_digits_yellow_upper.4bpp.inc"
};
const u8 sTimeColonYellowUpperGfx[] = {
#include "data/hud/time_colon_yellow_upper.4bpp.inc"
};
const u8 sTimeColonEmptyYellowUpperGfx[] = {
#include "data/hud/time_colon_empty_yellow_upper.4bpp.inc"
};
const u8 sTimeDigitsYellowLowerGfx[] = {
#include "data/hud/time_digits_yellow_lower.4bpp.inc"
};
const u8 sTimeColonYellowLowerGfx[] = {
#include "data/hud/time_colon_yellow_lower.4bpp.inc"
};
const u8 sTimeColonEmptyYellowLowerGfx[] = {
#include "data/hud/time_colon_empty_yellow_lower.4bpp.inc"
};

const u8 sTimeDigitsRedUpperGfx[] = {
#include "data/hud/time_digits_red_upper.4bpp.inc"
};
const u8 sTimeColonRedUpperGfx[] = {
#include "data/hud/time_colon_red_upper.4bpp.inc"
};
const u8 sTimeColonEmptyRedUpperGfx[] = {
#include "data/hud/time_colon_empty_red_upper.4bpp.inc"
};
const u8 sTimeDigitsRedLowerGfx[] = {
#include "data/hud/time_digits_red_lower.4bpp.inc"
};
const u8 sTimeColonRedLowerGfx[] = {
#include "data/hud/time_colon_red_lower.4bpp.inc"
};
const u8 sTimeColonEmptyRedLowerGfx[] = {
#include "data/hud/time_colon_empty_red_lower.4bpp.inc"
};

const u8 sJewelPieceIconPartialNEGfx[] = {
#include "data/hud/jewel_piece_icon_partial_ne.4bpp.inc"
};
const u8 sJewelPieceIconPartialSEGfx[] = {
#include "data/hud/jewel_piece_icon_partial_se.4bpp.inc"
};
const u8 sJewelPieceIconPartialSWGfx[] = {
#include "data/hud/jewel_piece_icon_partial_sw.4bpp.inc"
};
const u8 sJewelPieceIconPartialNWGfx[] = {
#include "data/hud/jewel_piece_icon_partial_nw.4bpp.inc"
};
const u8 sJewelPieceIconNEGfx[] = {
#include "data/hud/jewel_piece_icon_ne.4bpp.inc"
};
const u8 sJewelPieceIconSEGfx[] = {
#include "data/hud/jewel_piece_icon_se.4bpp.inc"
};
const u8 sJewelPieceIconSWGfx[] = {
#include "data/hud/jewel_piece_icon_sw.4bpp.inc"
};
const u8 sJewelPieceIconNWGfx[] = {
#include "data/hud/jewel_piece_icon_nw.4bpp.inc"
};
const u8 sCDIconPartialGfx[] = {
#include "data/hud/cd_icon.4bpp.inc"
};
const u8 sCDIconGfx[] = {
#include "data/hud/cd_icon.4bpp.inc"
};

const u8 sTimeUpBaseGfx[] = {
#include "data/hud/time_up_base.4bpp.inc"
};
const u8 sTimeUpTextGfx[] = {
#include "data/hud/time_up_text.4bpp.inc"
};
const u8 sTimeUpWarioGfx[] = {
#include "data/hud/time_up_wario.4bpp.inc"
};
const u8 sTimeUpEffect0Gfx[] = {
#include "data/hud/time_up_effect_0.4bpp.inc"
};
const u8 sTimeUpEffect1Gfx[] = {
#include "data/hud/time_up_effect_1.4bpp.inc"
};
const u8 sTimeUpEffect2Gfx[] = {
#include "data/hud/time_up_effect_2.4bpp.inc"
};
const u8 sTimeUpEffect3Gfx[] = {
#include "data/hud/time_up_effect_3.4bpp.inc"
};

// clang-format off
const u8 sHeartMeterEmptyGfx[] = {
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter1HeartGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter2HeartsGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter3HeartsGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter4HeartsGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter5HeartsGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter6HeartsGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter7HeartsGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
const u8 sHeartMeter8HeartsGfx[] = {
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
#include "data/hud/heart_meter_heart_filled.4bpp.inc"
};
const u8 sHeartMeter1HeartFlashingGfx[] = {
#include "data/hud/heart_meter_heart_flashing.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
#include "data/hud/heart_meter_heart_empty.4bpp.inc"
};
// clang-format on

const u8 sHeartGaugeEmptyGfx[] = {
#include "data/hud/heart_gauge_empty.4bpp.inc"
};
const u8 sHeartGaugeFill1Gfx[] = {
#include "data/hud/heart_gauge_fill_1.4bpp.inc"
};
const u8 sHeartGaugeFill2Gfx[] = {
#include "data/hud/heart_gauge_fill_2.4bpp.inc"
};
const u8 sHeartGaugeFill3Gfx[] = {
#include "data/hud/heart_gauge_fill_3.4bpp.inc"
};
const u8 sHeartGaugeFill4Gfx[] = {
#include "data/hud/heart_gauge_fill_4.4bpp.inc"
};
const u8 sHeartGaugeFill5Gfx[] = {
#include "data/hud/heart_gauge_fill_5.4bpp.inc"
};
const u8 sHeartGaugeFill6Gfx[] = {
#include "data/hud/heart_gauge_fill_6.4bpp.inc"
};
const u8 sHeartGaugeFill7Gfx[] = {
#include "data/hud/heart_gauge_fill_7.4bpp.inc"
};
const u8 sHeartGaugeFullGfx[] = {
#include "data/hud/heart_gauge_full.4bpp.inc"
};

const u8 sBossStageSpritesGfx[] = {
#include "data/boss_stage_sprites.4bpp.inc"
};

const u8 sAppleBombGfx[] = {
#include "data/sprites/apple_bomb.4bpp.inc"
};
const u8 sBlastCannonGfx[] = {
#include "data/sprites/blast_cannon.4bpp.inc"
};
const u8 sVizormanGfx[] = {
#include "data/sprites/vizorman.4bpp.inc"
};
const u8 sBugleGfx[] = {
#include "data/sprites/bugle.4bpp.inc"
};
const u8 sBlackDogGfx[] = {
#include "data/sprites/black_dog.4bpp.inc"
};
const u8 sLargeLipsGfx[] = {
#include "data/sprites/large_lips.4bpp.inc"
};
const u8 sBigFistGfx[] = {
#include "data/sprites/big_fist.4bpp.inc"
};
const u8 sBlackDragonGfx[] = {
#include "data/sprites/black_dragon.4bpp.inc"
};

const u16 sTimeUpTextPal[] = {
#include "data/hud/time_up_text.pal.inc"
};
const u16 sTimeUpWarioPal[] = {
#include "data/hud/time_up_wario.pal.inc"
};

const u8* sBossLifeMeterGfx[] = {
    sBossLife0Gfx,  sBossLife1Gfx,  sBossLife2Gfx,  sBossLife3Gfx,  sBossLife4Gfx,  sBossLife5Gfx,  sBossLife6Gfx,
    sBossLife7Gfx,  sBossLife8Gfx,  sBossLife9Gfx,  sBossLife10Gfx, sBossLife11Gfx, sBossLife12Gfx, sBossLife13Gfx,
    sBossLife14Gfx, sBossLife15Gfx, sBossLife16Gfx, sBossLife17Gfx, sBossLife18Gfx, sBossLife19Gfx, sBossLife20Gfx,
};
const u8* sHeartGaugeGfx[] = {
    sHeartGaugeEmptyGfx,
    sHeartGaugeFill1Gfx,
    sHeartGaugeFill2Gfx,
    sHeartGaugeFill3Gfx,
    sHeartGaugeFill4Gfx,
    sHeartGaugeFill5Gfx,
    sHeartGaugeFill6Gfx,
    sHeartGaugeFill7Gfx,
    sHeartGaugeFullGfx,
};
const u8* sHeartMeterGfx[] = {
    sHeartMeterEmptyGfx,
    sHeartMeter1HeartGfx,
    sHeartMeter2HeartsGfx,
    sHeartMeter3HeartsGfx,
    sHeartMeter4HeartsGfx,
    sHeartMeter5HeartsGfx,
    sHeartMeter6HeartsGfx,
    sHeartMeter7HeartsGfx,
    sHeartMeter8HeartsGfx,
    sHeartMeter1HeartFlashingGfx,
};
#else
extern const u16 sCommonSpritesPal[];
extern const u8 sCommonSpritesGfx[0x3000];
extern const u8 sScoreDigitsGfx[];
extern const u8 sTimeDigitsUpperGfx[];
extern const u8 sTimeColonUpperGfx[];
extern const u8 sTimeColonEmptyUpperGfx[];
extern const u8 sTimeDigitsLowerGfx[];
extern const u8 sTimeColonLowerGfx[];
extern const u8 sTimeColonEmptyLowerGfx[];
extern const u8 sTimeDigitsYellowUpperGfx[];
extern const u8 sTimeColonYellowUpperGfx[];
extern const u8 sTimeColonEmptyYellowUpperGfx[];
extern const u8 sTimeDigitsYellowLowerGfx[];
extern const u8 sTimeColonYellowLowerGfx[];
extern const u8 sTimeColonEmptyYellowLowerGfx[];
extern const u8 sTimeDigitsRedUpperGfx[];
extern const u8 sTimeColonRedUpperGfx[];
extern const u8 sTimeColonEmptyRedUpperGfx[];
extern const u8 sTimeDigitsRedLowerGfx[];
extern const u8 sTimeColonRedLowerGfx[];
extern const u8 sTimeColonEmptyRedLowerGfx[];
extern const u8 sJewelPieceIconPartialNEGfx[];
extern const u8 sJewelPieceIconPartialSEGfx[];
extern const u8 sJewelPieceIconPartialSWGfx[];
extern const u8 sJewelPieceIconPartialNWGfx[];
extern const u8 sJewelPieceIconNEGfx[];
extern const u8 sJewelPieceIconSEGfx[];
extern const u8 sJewelPieceIconSWGfx[];
extern const u8 sJewelPieceIconNWGfx[];
extern const u8 sCDIconPartialGfx[];
extern const u8 sCDIconGfx[];
extern const u8 sTimeUpBaseGfx[];
extern const u8 sTimeUpTextGfx[];
extern const u8 sTimeUpWarioGfx[];
extern const u8 sTimeUpEffect0Gfx[];
extern const u8 sTimeUpEffect1Gfx[];
extern const u8 sTimeUpEffect2Gfx[];
extern const u8 sTimeUpEffect3Gfx[];
extern const u8 sHeartMeterEmptyGfx[];
extern const u8 sHeartMeter1HeartGfx[];
extern const u8 sHeartMeter2HeartsGfx[];
extern const u8 sHeartMeter3HeartsGfx[];
extern const u8 sHeartMeter4HeartsGfx[];
extern const u8 sHeartMeter5HeartsGfx[];
extern const u8 sHeartMeter6HeartsGfx[];
extern const u8 sHeartMeter7HeartsGfx[];
extern const u8 sHeartMeter8HeartsGfx[];
extern const u8 sHeartMeter1HeartFlashingGfx[];
extern const u8 sHeartGaugeEmptyGfx[];
extern const u8 sHeartGaugeFill1Gfx[];
extern const u8 sHeartGaugeFill2Gfx[];
extern const u8 sHeartGaugeFill3Gfx[];
extern const u8 sHeartGaugeFill4Gfx[];
extern const u8 sHeartGaugeFill5Gfx[];
extern const u8 sHeartGaugeFill6Gfx[];
extern const u8 sHeartGaugeFill7Gfx[];
extern const u8 sHeartGaugeFullGfx[];
extern const u8 sBossStageSpritesGfx[0xC00];
extern const u8 sAppleBombGfx[0x1000];
extern const u8 sBlastCannonGfx[0x1000];
extern const u8 sVizormanGfx[0x1000];
extern const u8 sBugleGfx[0x1000];
extern const u8 sBlackDogGfx[0x1000];
extern const u8 sLargeLipsGfx[0x1000];
extern const u8 sBigFistGfx[0x1000];
extern const u8 sBlackDragonGfx[0x1000];
extern const u16 sTimeUpTextPal[];
extern const u16 sTimeUpWarioPal[];
extern GBA_PTR(const u8*) sBossLifeMeterGfx[];
extern GBA_PTR(const u8*) sHeartGaugeGfx[];
extern GBA_PTR(const u8*) sHeartMeterGfx[];
#endif

void LoadHudSpriteGraphics(void)
{
    DmaCopy16(3, sCommonSpritesGfx, 0x6011000, sizeof(sCommonSpritesGfx));

    if (gCurrentStageNumber != STAGE_BOSS) {
        return;
    }

    DmaCopy16(3, sBossStageSpritesGfx, 0x6012400, sizeof(sBossStageSpritesGfx));

    switch (gCurrentShopItem) {
        case SHOP_ITEM_APPLE_BOMB:
            DmaCopy16(3, sAppleBombGfx, 0x6013000, sizeof(sAppleBombGfx));
            break;
        case SHOP_ITEM_BLAST_CANNON:
            DmaCopy16(3, sBlastCannonGfx, 0x6013000, sizeof(sBlastCannonGfx));
            break;
        case SHOP_ITEM_VIZORMAN:
            DmaCopy16(3, sVizormanGfx, 0x6013000, sizeof(sVizormanGfx));
            break;
        case SHOP_ITEM_BUGLE:
            DmaCopy16(3, sBugleGfx, 0x6013000, sizeof(sBugleGfx));
            break;
        case SHOP_ITEM_BLACK_DOG:
            DmaCopy16(3, sBlackDogGfx, 0x6013000, sizeof(sBlackDogGfx));
            break;
        case SHOP_ITEM_LARGE_LIPS:
            DmaCopy16(3, sLargeLipsGfx, 0x6013000, sizeof(sLargeLipsGfx));
            break;
        case SHOP_ITEM_BIG_FIST:
            DmaCopy16(3, sBigFistGfx, 0x6013000, sizeof(sBigFistGfx));
            break;
        case SHOP_ITEM_BLACK_DRAGON:
            DmaCopy16(3, sBlackDragonGfx, 0x6013000, sizeof(sBlackDragonGfx));
            break;
    }
}

void UpdateBossHealthGauge(void)
{
    u8 health;

    health = gCurrentSprite.health;
#ifdef PORTABLE
    PortRumbleBossHealth();
#endif
    DmaCopy16(3, GP(sBossLifeMeterGfx[health]), 0x6012C00, 10 * TILE_SIZE_4BPP);
}

void UpdateWarioLifeHud(void)
{
    u32 heartGaugeIncrease;
    struct WarioLife* heartMeter;

    heartGaugeIncrease = 0;
    gHeartGauge.unk_2 = 0;

    if (gHeartGauge.filling != 0) {
        if (gHeartGauge.unk_3 == 0) {
            if (gHeartGauge.filling == 0x80) {
                gHeartGauge.current = 0;
                gHeartGauge.filling = 0;
            } else if (gHeartGauge.filling == 0xFF) {
                heartGaugeIncrease = 8;
            } else {
                gHeartGauge.unk_3 = 1;
                gHeartGauge.current++;
                heartGaugeIncrease = gHeartGauge.current;
                gHeartGauge.filling--;
                SpawnSecondarySprite((u16)(gWarioData.yPosition - 40), (u16)(gWarioData.xPosition - 16), 63);
            }
        } else {
            gHeartGauge.unk_3--;
        }
    }

    GP_SET(gCurrentHeartGaugeGfx, (u8*)GP(sHeartGaugeGfx[gHeartGauge.current]));

    if (heartGaugeIncrease == 8) {
        if (gHeartMeter.current + gHeartMeter.filling > 7) {
            gHeartGauge.filling = 0xFF;
            gHeartGauge.unk_3 = 0;
        } else {
            gHeartGauge.filling = 0x80;
            gHeartGauge.unk_3 = 8;
            gHeartMeter.filling++;
            if (gHeartMeter.current + gHeartMeter.filling > 8) {
                gHeartMeter.filling = 8 - gHeartMeter.current;
            }
            gHeartMeter.unk_2 = gHeartMeter.filling << 3;
            gHeartMeter.unk_3 = heartGaugeIncrease;
        }
    }

    if (gHeartMeter.filling != 0) {
        if (gHeartMeter.unk_3 == 0) {
            gHeartMeter.unk_3 = 8;
            gHeartMeter.current++;
            gHeartMeter.filling--;
            SpawnSecondarySprite((u16)(gWarioData.yPosition - 72), (u16)(gWarioData.xPosition - 32), 62);
        } else {
            gHeartMeter.unk_3--;
        }
    }

    heartMeter = &gHeartMeter;
    if (heartMeter->unk_2 != 0) {
        heartMeter->unk_2--;
    }

    if (heartMeter->current == 1 && gTimerState <= 3 && (gMainTimer & 0x20) != 0) {
        if ((gMainTimer & 0x3F) == 0x20 && gWarioPauseTimer == 0) {
            m4aSongNumStart(0x1C8);
        }
        GP_SET(gCurrentHeartMeterGfx, (u8*)GP(sHeartMeterGfx[9]));
    } else {
        GP_SET(gCurrentHeartMeterGfx, (u8*)GP(sHeartMeterGfx[heartMeter->current]));
    }
}

void DrawGameplayHudOam(void)
{
    u16* dest;
    int new_var;
    register u32 count PIN("ip");
    u8 stage;
    int new_var2;
    u8* base;
    dest = (u16*)gOamBuffer;
    new_var2 = 0;
    ASM_BARRIER_VOLATILE(: "=r"(count) : "0"(new_var2));
    new_var = 6;
    ;
    stage = gCurrentStageNumber;
    base = (u8*)dest;
    if (stage > 3) {
        if ((gCurrentRoom != 0) && (gTimerState != 11)) {
            *(dest++) = count;
            ((OamData*)base)->split.y = 6;
            ((OamData*)base)->split.shape = 1;
            do {
                *(dest++) = count;
            } while (0);
            ((OamData*)base)->split.x = 161;
            ((OamData*)base)->split.size = 1;
            *dest = count;
            ((OamData*)base)->split.tileNum = 352;
            ((OamData*)base)->split.paletteNum = 14;
            dest += 2;
            *(dest++) = 0;
            {
                u8* oam;
                oam = base + 8;
                ((OamData*)oam)->split.y = 6;
                ((OamData*)oam)->split.shape = 1;
                *(dest++) = 0;
                ((OamData*)oam)->split.x = 193;
                ((OamData*)oam)->split.size = 1;
                *dest = 0;
                ((OamData*)oam)->split.tileNum = 356;
                ASM_BARRIER_VOLATILE(: "+r"(count));
                ((OamData*)oam)->split.paletteNum = 14;
                dest += 2;
            }
            *(dest++) = 0;
            {
                u8* oam;
                oam = base + 16;
                ((OamData*)oam)->split.y = 6;
                ((OamData*)oam)->split.shape = 1;
                *(dest++) = new_var2;
                ((OamData*)oam)->split.x = 225;
                ((OamData*)oam)->split.size = 0;
                *dest = 0;
                ((OamData*)oam)->split.tileNum = 360;
                ((OamData*)oam)->split.paletteNum = 14;
                dest += 2;
            }
            goto setInitialOamCount;
        }
    } else if (gTimerState <= 2) {
        u32 xMask;

        {
            u8* oam;
            {
                register u32 tempCount PIN("r2");
                tempCount = count;
                *(dest++) = tempCount;
                stage = 6;
                new_var = stage;
                ASM_BARRIER_VOLATILE(: : "r"(new_var), "r"(base));
                ((OamData*)base)->split.y = new_var;
                *(dest++) = tempCount;
            }
            {
                u32 result;
                register u16 oldAttr PIN("r1");
                register u8* storeOam PIN("r2");
                u32 hardMask;

                oldAttr = *(u16*)(base + 2);
                hardMask = 0xFFFFFE00;
                result = hardMask;
                result &= oldAttr;
                oldAttr = 179;
                result |= oldAttr;
                storeOam = base;
                *(u16*)(storeOam + 2) = result;
                ASM_BARRIER_VOLATILE(: "=r"(xMask) : "0"(hardMask));
                oam = storeOam;
            }
            {
                u32 storeCount;
                ASM_BARRIER_VOLATILE(: "=r"(storeCount) : "0"(count));
                *dest = storeCount;
            }
            ((OamData*)oam)->split.tileNum = 203;
            ((OamData*)oam)->split.paletteNum = new_var;
            dest += 2;
        }
        count = 2;
        {
            u32 zero;
            zero = 0;
            *(dest++) = zero;
        }
        {
            u8* oam;
            oam = base + 8;
            {
                int yValue;
                yValue = new_var;
                *(base + 8) = yValue;
            }
            ((OamData*)oam)->split.shape = 1;
            *(dest++) = 0;
            ((OamData*)oam)->all.attr1 = (((OamData*)oam)->all.attr1 & xMask) | 188;
            ((OamData*)oam)->split.size = 0;
            *dest = 0;
            ((OamData*)oam)->split.tileNum = 132;
            ((OamData*)oam)->split.paletteNum = 4;
            dest += 2;
        }
        *(dest++) = 0;
        {
            u8* oam;
            oam = base + 16;
            ((OamData*)oam)->split.y = new_var;
            ((OamData*)oam)->split.shape = 1;
            *(dest++) = 0;
            ((OamData*)oam)->all.attr1 = (((OamData*)oam)->all.attr1 & xMask) | 204;
            ((OamData*)oam)->split.size = 1;
            *dest = 0;
            ((OamData*)oam)->split.tileNum = 134;
            ((OamData*)oam)->split.paletteNum = 4;
            dest += count;
        }
        goto setInitialOamCount;
    }
    goto processStatusOam;
setInitialOamCount:
    count = 3;

processStatusOam:
    if (gCurrentStageNumber > 3) {
        if (gCurrentRoom != 0) {
            *(dest++) = 0;
            {
                u8* oam;
                oam = base + (count * 8);
                ((OamData*)oam)->split.y = new_var;
                ((OamData*)oam)->split.shape = 1;
                *(dest++) = 0;
                ((OamData*)oam)->split.x = 9;
                ((OamData*)oam)->split.size = 1;
                *dest = 0;
                ((OamData*)oam)->split.tileNum = 144;
                ((OamData*)oam)->split.paletteNum = 7;
                dest += 2;
            }
            count++;
            *(dest++) = 0;
            {
                u8* oam;
                oam = base + (count * 8);
                ((OamData*)oam)->split.y = new_var;
                ((OamData*)oam)->split.shape = 1;
                *(dest++) = 0;
                ((OamData*)oam)->split.x = 41;
                ((OamData*)oam)->split.size = 1;
                *dest = 0;
                ((OamData*)oam)->split.tileNum = 148;
                ((OamData*)oam)->split.paletteNum = 7;
                count++;
            }
        }
    } else {
        *(dest++) = 0;
        {
            u8* oam;
            oam = base + (count * 8);
            ((OamData*)oam)->split.y = new_var;
            ((OamData*)oam)->split.shape = 1;
            *(dest++) = 0;
            ((OamData*)oam)->split.x = 9;
            goto dummy_label_71025;
        dummy_label_71025:;

            ;
            ;
            ;
            ((OamData*)oam)->split.size = 2;
            *dest = 0;
            ((OamData*)oam)->split.tileNum = 144;
            ((OamData*)oam)->split.paletteNum = 7;
            dest += 2;
        }
        count++;
        *(dest++) = 0;
        {
            u8* oam;
            oam = base + (count * 8);
            ((OamData*)oam)->split.y = new_var;
            ((OamData*)oam)->split.shape = 1;
            *(dest++) = 0;
            ((OamData*)oam)->split.x = 41;
            ((OamData*)oam)->split.size = 2;
            *dest = 0;
            ((OamData*)oam)->split.tileNum = 148;
            ((OamData*)oam)->split.paletteNum = 7;
            count++;
        }
    }

    gOamSlotsUsed = count;
}

void UpdateBigBoardSecondarySprite(void)
{
    struct SecondarySprite* source;
    struct SecondarySprite* sprite;
    u32 pose;
    u32 switchPose;

    source = &gCurrentSecondarySprite;
    pose = source->pose;
    sprite = source;
    switchPose = pose;
    switch (switchPose) {
        case 0:
            sprite->status |= 4;
            sprite->timer = 8;
            sprite->pose = 1;
            break;
        case 1:
            if (sprite->work0 == 0) {
                break;
            }
            {
                u32 timer;
                timer = sprite->timer;
                timer--;
                sprite->timer = timer;
                timer <<= 24;
                if (timer != 0) {
                    break;
                }
            }
            {
                register u32 frame PIN("r0");
                u32 nextFrame;
                u32 value;
                u32 four;
                frame = sprite->animationTimer;
                nextFrame = frame + 1;
                sprite->animationTimer = nextFrame;
                value = 2;
                sprite->pose = value;
                four = 4;
                sprite->timer = four;
                frame -= 6;
                frame = (u16)frame;
                if (frame <= 67) {
                    break;
                }
                value = sprite->xPosition;
                frame = value;
                frame += 64;
                sprite->xPosition = frame;
                frame = (u16)nextFrame;
                if (frame == 79) {
                    frame = value;
                    frame += 80;
                    sprite->xPosition = frame;
                    sprite->pose = four;
                } else if (frame == 1) {
                    frame = value;
                    frame += 80;
                    sprite->xPosition = frame;
                }
            }
            break;
        case 2: {
            u32 value;
            register u32 mask PIN("r1");
            value = sprite->animationTimer;
            value -= 7;
            value = (u16)value;
            if (value <= 67) {
                gUnk_300003A += 16;
            }
            value = sprite->timer;
            value--;
            sprite->timer = value;
            mask = 255;
            value <<= 24;
            if (value != 0) {
                break;
            }
            value = 1;
            sprite->pose = value;
            value = 8;
            sprite->timer = value;
            value = sprite->work0;
            value--;
            sprite->work0 = value;
            value &= mask;
            if (value != 0) {
                goto playMoveSound;
            }
            goto playStopSound;
        }
        case 3:
            if (sprite->work0 == 0) {
                break;
            }
            {
                u32 timer;
                timer = sprite->timer;
                timer--;
                sprite->timer = timer;
                timer <<= 24;
                if (timer != 0) {
                    break;
                }
            }
            {
                u32 frame;
                register u32 four PIN("r4");
                u32 oldX;
                register u32 value PIN("r1");
                frame = sprite->animationTimer;
                frame--;
                sprite->animationTimer = frame;
                four = 4;
                sprite->pose = four;
                sprite->timer = four;
                oldX = sprite->xPosition;
                value = oldX;
                value -= 64;
                sprite->xPosition = value;
                frame = (u16)frame;
                if (frame == 78) {
                    frame = oldX;
                    frame -= 80;
                    sprite->xPosition = frame;
                }
            }
            break;
        case 4: {
            register struct SecondarySprite* current PIN("r1");
            u32 value;
            register u32 mask PIN("r3");
            current = sprite;
            value = current->timer;
            value--;
            current->timer = value;
            mask = 255;
            value <<= 24;
            if (value != 0) {
                break;
            }
            value = 3;
            current->pose = value;
            value = 8;
            current->timer = value;
            value = current->work0;
            value--;
            current->work0 = value;
            value &= mask;
            if (value != 0) {
            playMoveSound:
                m4aSongNumStart(224);
                break;
            }
            value = current->animationTimer;
            if (value == 79) {
                value = 5;
                current->pose = value;
            playStopSound:
                m4aSongNumStart(225);
                break;
            }
            value = 1;
            sprite->pose = value;
            m4aSongNumStart(225);
        } break;
        case 5:
            break;
    }

    if (gUnk_3000038 == 0 || gCurrentSecondarySprite.pose == 2) {
        GP_SET(gCurrentSecondarySpriteOamData, sUnk_83BF986);
    } else {
        struct SecondarySprite* current;
        GBA_TYPEOF(const u8**, &gCurrentSecondarySpriteOamData) destination;
        const u8* table;
        const u8* spaces;
        u32 index;

        current = &gCurrentSecondarySprite;
        destination = &gCurrentSecondarySpriteOamData;
        table = sBigBoardSpaceSpriteTable;
        spaces = sBigBoardSpaces;
        index = current->animationTimer;
        index += (u32)spaces;
        index = *(const u8*)index;
        index <<= 3;
        index += (u32)table;
        index = *(const u32*)index;
        GP_SET(*destination, (const u8*)index);
    }
}

void StartCollectedJewelPieceHudEffect(void)
{
    s8* state;
    u32 position;
    u32 direction;
    s32 signedDirection;

    state = gStageTimerDigits;
    if (state[0] == 0) {
        position = (u8)state[1];
        direction = (u8)state[2];
        signedDirection = state[2];
        if (signedDirection == 1 && position == 0) {
            StartTileEffect(0);
            gCollectedNEJewelPiece = 1;
        } else if (direction == 0) {
            if ((s8)position == 4) {
                StartTileEffect(1);
                gCollectedSEJewelPiece = 1;
            } else if ((s8)position == 2) {
                StartTileEffect(2);
                gCollectedSWJewelPiece = 1;
            }
        }
    }
}

void StartNextCollectedTreasureHudEffect(void)
{
    if (gCollectedNEJewelPiece == 2) {
        StartTileEffect(0);
        gCollectedNEJewelPiece = 1;
    } else if (gCollectedNWJewelPiece == 2) {
        StartTileEffect(3);
        gCollectedNWJewelPiece = 1;
    } else if (gHasBossTreasure7 == 2) {
        StartTileEffect(6);
        gHasBossTreasure7 = 1;
    } else if (gHasBossTreasure10 == 2) {
        StartTileEffect(9);
        gHasBossTreasure10 = 1;
    } else if (gCollectedSEJewelPiece == 2) {
        StartTileEffect(1);
        gCollectedSEJewelPiece = 1;
    } else if (gHasBossTreasure5 == 2) {
        StartTileEffect(4);
        gHasBossTreasure5 = 1;
    } else if (gHasBossTreasure8 == 2) {
        StartTileEffect(7);
        gHasBossTreasure8 = 1;
    } else if (gHasBossTreasure11 == 2) {
        StartTileEffect(10);
        gHasBossTreasure11 = 1;
    } else if (gCollectedSWJewelPiece == 2) {
        StartTileEffect(2);
        gCollectedSWJewelPiece = 1;
    } else if (gHasBossTreasure6 == 2) {
        StartTileEffect(5);
        gHasBossTreasure6 = 1;
    } else if (gHasBossTreasure9 == 2) {
        StartTileEffect(8);
        gHasBossTreasure9 = 1;
    } else if (gHasBossTreasure12 == 2) {
        StartTileEffect(11);
        gHasBossTreasure12 = 1;
    }
}

void UpdateTreasureCollectionHudEffects(void)
{
    s8* state;
    u32 position;
    u32 savedPosition;
    s32 direction;
    s32 positionIndex;

    state = gStageTimerDigits;
    if (state[0] != 0) {
        return;
    }

    position = (u8)state[1];
    savedPosition = position;
    direction = state[2];
    if (direction == 2) {
        if (position == 0 && gGoldenTreasureCount == 12) {
            StartNextCollectedTreasureHudEffect();
        }
    } else if (direction == 1) {
        switch ((s8)position) {
            case 5:
                if (gGoldenTreasureCount > 10) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 4:
                if (gGoldenTreasureCount > 9) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 3:
                if (gGoldenTreasureCount > 8) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 2:
                if (gGoldenTreasureCount > 7) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 1:
                if (gGoldenTreasureCount > 6) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 0:
                if (gGoldenTreasureCount > 5) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
        }
    } else if (direction == 0) {
        positionIndex = (savedPosition << 24) + (0xFF << 24);
        positionIndex >>= 24;
        switch (positionIndex) {
            case 4:
                if (gGoldenTreasureCount > 4) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 3:
                if (gGoldenTreasureCount > 3) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 2:
                if (gGoldenTreasureCount > 2) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 1:
                if (gGoldenTreasureCount > 1) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
            case 0:
                if (gGoldenTreasureCount != 0) {
                    StartNextCollectedTreasureHudEffect();
                }
                break;
        }
    }
}

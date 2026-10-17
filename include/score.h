#ifndef SCORE_H
#define SCORE_H

#include "global_data.h"
#include "oam.h"
#include "types.h"

// Number of points per internal unit of score
#define SCORE_RATIO 10
// Convert an amount of points for storage
#define CONVERT_SCORE(points) ((points) / SCORE_RATIO)

#define MAX_SCORE CONVERT_SCORE(999990)

enum ScoreSpriteID {
    SSPRITE_SCORE_10,
    SSPRITE_SCORE_50,
    SSPRITE_SCORE_100,
    SSPRITE_SCORE_500,
    SSPRITE_SCORE_1000,
    SSPRITE_ENEMY_KILLED_EFFECT,
    SSPRITE_06,
    SSPRITE_07,
    SSPRITE_08,
    SSPRITE_09,
    SSPRITE_0A,
    SSPRITE_0B,
    SSPRITE_0C,
    SSPRITE_0D,
    SSPRITE_0E,
    SSPRITE_0F,
    SSPRITE_10,
    SSPRITE_11,
    SSPRITE_12,
    SSPRITE_13,
    SSPRITE_14,
    SSPRITE_15,
    SSPRITE_16,
    SSPRITE_17,
    SSPRITE_18,
    SSPRITE_19,
    SSPRITE_1A,
    SSPRITE_1B,
    SSPRITE_1C,
    SSPRITE_1D,
    SSPRITE_1E,
    SSPRITE_1F,
    SSPRITE_20,
    SSPRITE_21,
    SSPRITE_22,
    SSPRITE_23,
    SSPRITE_24,
    SSPRITE_25,
    SSPRITE_26,
    SSPRITE_27,
    SSPRITE_28,
    SSPRITE_29,
    SSPRITE_2A,
    SSPRITE_2B,
    SSPRITE_2C,
    SSPRITE_2D,
    SSPRITE_2E,
    SSPRITE_2F,
    SSPRITE_30,
    SSPRITE_31,
    SSPRITE_32,
    SSPRITE_33,
    SSPRITE_34,
    SSPRITE_35,
    SSPRITE_36,
    SSPRITE_37,
    SSPRITE_38,
    SSPRITE_39,
    SSPRITE_3A,
    SSPRITE_3B,
    SSPRITE_3C,
    SSPRITE_3D,
    SSPRITE_3E,
    SSPRITE_3F,
    SSPRITE_40,
    SSPRITE_JEWEL_PIECE_ICONS,
    SSPRITE_CD_ICON,
    SSPRITE_43,
    SSPRITE_44,
    SSPRITE_45,
    SSPRITE_46,
    SSPRITE_47,
    SSPRITE_48,
    SSPRITE_49,
    SSPRITE_4A,
    SSPRITE_KEYZER,
    SSPRITE_WARIO_TIME_UP,
    SSPRITE_WARIO_DYING,
    SSPRITE_4E,
    SSPRITE_4F,
    SSPRITE_50,
    SSPRITE_51,
    SSPRITE_52,
    SSPRITE_53,
    SSPRITE_54,
    SSPRITE_55,
    SSPRITE_56,
    SSPRITE_57,
    SSPRITE_58,
    SSPRITE_59,
    SSPRITE_MAX = 90
};

struct AGB_ALIGN ItemCollection {
    u8 jewelPieceNE : 1;
    u8 jewelPieceSE : 1;
    u8 jewelPieceSW : 1;
    u8 jewelPieceNW : 1;
    u8 cd : 1;
    u8 keyzer : 1;
};

#define BOSS_REWARD_55_SEC jewelPieceNE
#define BOSS_REWARD_35_SEC jewelPieceSE
#define BOSS_REWARD_15_SEC jewelPieceSW
#define SHOW_BOSS_DOOR_OPENING jewelPieceNW
#define BOSS_DEFEATED keyzer
#define DIVA_DEFEATED cd

extern const struct AnimationFrame sUnk_840FB94[];
extern GBA_FUNC_PTR(ProcedureFunc) const sSecondarySpriteAITable[];

extern struct ItemCollection gCurrentCollection[PASSAGE_MAX][STAGE_MAX];
extern s32 gHighScoreTable[PASSAGE_MAX][NORMAL_STAGE_COUNT];
// gSecondarySpriteData
// gCurrentSecondarySprite
extern s32 gStageFrameCounter;
extern s8 gStageTimerDigits[];
extern s32 gTotalScore;
extern s32 gStageScore;
extern s32 gTotalScoreTemp;
extern u8 gNewHighScore;
extern s8 gScoreDigits[];
extern u8 gStageTimerDigitsState;
extern u8 gStageScoreDigitsDirty;
extern u8 gStageTimerColonState;
extern u8 gUnk_3000C04;
// gUnk_3000C05
extern u8 gEnemyScoreDropCounter;
extern u8 gCollectedNEJewelPiece;
extern u8 gCollectedSEJewelPiece;
extern u8 gCollectedSWJewelPiece;
extern u8 gCollectedNWJewelPiece;
extern u8 gCollectedCD;
extern u8 gCollectedKeyzer;
extern u8 gSwitchPressed;
extern u8 gUnk_3000C0E;
extern GBA_PTR(u8*) gCurrentHeartMeterGfx;
extern GBA_PTR(u8*) gCurrentHeartGaugeGfx;
extern u8 gVortexGfxSlot;
extern u16 gVortexYPosition;
extern u16 gVortexXPosition;
extern u8 gBossTreasureType;
extern u8 gBossTreasureStage;
#define gHasBossChest55sec gCollectedNEJewelPiece
#define gHasBossChest35sec gCollectedSEJewelPiece
#define gHasBossChest15sec gCollectedSWJewelPiece
extern u8 gHasBossTreasure5;
extern u8 gHasBossTreasure6;
extern u8 gHasBossTreasure7;
extern u8 gHasBossTreasure8;
extern u8 gHasBossTreasure9;
extern u8 gHasBossTreasure10;
extern u8 gHasBossTreasure11;
extern u8 gHasBossTreasure12;

void UpdateSecondarySprites(void);
void UploadStageScoreDigitTiles(void);
void UploadStageTimerDigitTiles(void);
void UploadStageTimerColonTiles(void);
void StartTimeUpSequence(void);
void PlayWarioDefeatJingle(void);
void UpdateStageTimerAndScoreDrain(void);
u8 LoadStageCollectiblesFromSave(void);
void DrawCurrentSecondarySprite(u32 index);
void InitStageHudAndScore(void);
void AddStageScore(s32);
void SpawnSecondarySprite(u32 y, u32 x, u32 id);
void SpawnOnePointTileEffect(u32 y, u32 x);
void SpawnTenPointTileEffect(u32 y, u32 x);
void SpawnTileDebrisParticle(u32 y, u32 x, u32 debrisOffset, u32 direction, u32 parity);
void SpawnScoreCoinDropSprites(u32 y, u32 x, s32 amount);
// void SpawnScoreCoinDropSprites(u16 yPosition, u16 xPosition, s32 amount);
void SpawnEnemyScoreDrop(u32 id, u32 y, u32 x, u32 amount);
void ChangeStageScoreAndDropCoins(s32);
void ConvertKeyzerToTimeUpEffect(void);
void ResetTimeUpWarioSecondarySprite(void);
void AdvanceSecondarySpriteAnimation(const struct AnimationFrame* animation);

#endif  // SCORE_H

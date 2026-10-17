#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

#ifndef ASM_INCLUDE
#define ASM_INCLUDE(path) __asm__(".include \"" path "\"\n")
#endif

#include "gba.h"
#include "oam.h"

enum Passage {
    PASSAGE_ENTRY,
    PASSAGE_EMERALD,
    PASSAGE_RUBY,
    PASSAGE_TOPAZ,
    PASSAGE_SAPPHIRE,
    PASSAGE_GOLDEN,
    PASSAGE_SOUND_ROOM,
    PASSAGE_MAX
};

#define ENTRY_GOLDEN_STAGE_COUNT 1
#define NORMAL_STAGE_COUNT 4
#define STAGE_BOSS 4
#define STAGE_MINIGAMES 5
#define STAGE_MAX 6

enum Difficulty {
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_SUPER_HARD,
};

enum ClearMessage {
    CLEAR_MESSAGE_TRY_FOR_ALL_TREASURES,
    CLEAR_MESSAGE_ALL_TREASURES,
    CLEAR_MESSAGE_SUPER_HARD_UNLOCKED,
};

enum TitleScreenStyle {
    TITLE_STYLE_NORMAL,
    TITLE_STYLE_S_HARD_UNLOCKED,
};

enum Language {
    LANGUAGE_ENGLISH,
    LANGUAGE_JAPANESE,
};

enum SaveFile {
    SAVE_A,
    SAVE_B,
    SAVE_COUNT
};

enum SwitchID {
    SWITCH_UNUSED,
    SWITCH_RED,
    SWITCH_PURPLE,
    SWITCH_GREEN,
    SWITCH_ESCAPE,
    SWITCH_COUNT
};

enum SwitchState {
    SWITCH_STATE_OFF,
    SWITCH_STATE_ON,
    SWITCH_STATE_SWITCHING_ON,
    SWITCH_STATE_SWITCHING_OFF,
};

struct AGB_ALIGN GameScreenDebugInfo {
    u8 freeCameraEnabled;
    u8 roomEntryTensDigit;
    u8 roomEntryOnesDigitAndPalette;
    u8 reserved03;
    u16 tileAttribute;
    u8 reserved06[2];
};

struct AGB_ALIGN BackgroundInfo {
    GBA_PTR(u16*) pBg0Data;
    u16 bg0Width;
    u16 bg0Height;
    GBA_PTR(u16*) pBg1Data;
    u16 bg1Width;
    u16 bg1Height;
    GBA_PTR(u16*) pBg2Data;
    u16 bg2Width;
    u16 bg2Height;
    GBA_PTR(u16*) pBg3Data;
    u16 bg3Width;
    u16 bg3Height;
};

struct AGB_ALIGN RoomHeader {
    u8 tileset;
    u8 bg0Param;
    u8 bg1Param;
    u8 bg2Param;
    u8 bg3Param;
    GBA_PTR(u8*) pBg0Data;
    GBA_PTR(u8*) pBg1Data;
    GBA_PTR(u8*) pBg2Data;
    GBA_PTR(u8*) pBg3Data;
    u8 cameraControl;
    u8 layer3Scrolling;
    u8 bgPriorityAlpha;
    GBA_PTR(void*) pHardSpriteData;
    GBA_PTR(void*) pNormalSpriteData;
    GBA_PTR(void*) pSHardSpriteData;
    u8 raster;
    u8 water;
    u16 musicVolume;
};

struct AGB_ALIGN BackgroundCollisionData {
    u16 primaryType;
    u16 waterType;
    u16 bg2Type;
};

struct AGB_ALIGN BackgroundScroll {
    u16 bg0Horizontal;
    u16 bg0Vertical;
    u16 bg1Horizontal;
    u16 bg1Vertical;
    u16 bg2Horizontal;
    u16 bg2Vertical;
    u16 bg3Horizontal;
    u16 bg3Vertical;
};

struct AGB_ALIGN Window {
    u8 left;
    u8 right;
    u8 top;
    u8 bottom;
    u8 content;
};

struct AGB_ALIGN WarioCollisionData {
    u16 unk_00;
    u16 flags;
    u8 unk_04;
    u8 unk_05;
    u16 unk_06;
    u8 unk_08;
    u8 unk_09;
    u8 unk_0A;
    u8 unk_0B;
    u8 unk_0C;
    u8 unk_0D;
    u8 unk_0E;
    u8 unk_0F;
    u8 unk_10;
    u8 unk_11;
    u8 unk_12;
    u8 unk_13;
    u8 unk_14;
};

struct AGB_ALIGN WarioPoseProperty {
    u8 unk_00;
    u8 unk_01;
    u8 unk_02;
    u8 unk_03;
    u8 unk_04;
    u8 unk_05;
    u8 unk_06;
    u8 unk_07;
};

struct AGB_ALIGN WarioAnimationFrame {
    GBA_PTR(const u8*) objData;
    GBA_PTR(const u16*) oamData;
    u8 time;
    u8 padding[3];
};

struct AGB_ALIGN WarioHitbox {
    s16 left;
    s16 top;
    s16 right;
    s16 bottom;
};

struct AGB_ALIGN TransparencyState {
    u8 targetAlpha;
    u8 currentAlpha;
    u8 defaultAlpha;
    u8 blendTimer;
};

struct AGB_ALIGN BackgroundTileTables {
    GBA_PTR(const u16*) top;
    GBA_PTR(const u8*) bottom;
    GBA_PTR(const u16*) attributes;
};

struct AGB_ALIGN GameMusicState {
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

struct AGB_ALIGN BackgroundPositionState {
    u16 x;
    u16 y;
    s8 xOffset;
    s8 yOffset;
};

struct AGB_ALIGN RoomEffectState {
    u8 type;
    u8 timer;
    u16 value;
};

struct AGB_ALIGN ScreenShakeParameters {
    u8 duration;
    u8 frameTimer;
    u8 amplitude;
    u8 direction;
};

struct AGB_ALIGN WarioEffectFrame {
    GBA_PTR(const void*) data;
    u16 time;
    u16 pad;
};

struct AGB_ALIGN SecondarySprite {
    u8 status;
    u8 work0;
    u8 id;
    u8 pose;
    u8 timer;
    u8 unk5;
    u16 animationTimer;
    u16 yPosition;
    u16 xPosition;
};

struct AGB_ALIGN SaveDataClearData {
    u8 pad_00[0x08];
    u8 unk_08;
    u8 unk_09;
    u8 unk_0A;
    u8 pad_0B;
    u16 unk_0C;
    u16 unk_0E;
    u8 pad_10[0x04];
    u8 unk_14;
    u8 unk_15;
    u8 unk_16;
    u8 pad_17;
    u16 unk_18;
    u16 unk_1A;
    u8 pad_1C[0x04];
    u8 unk_20;
    u8 unk_21;
    u8 unk_22;
};

struct AGB_ALIGN SaveClearAnimationState {
    u16 y;
    u16 x;
    u8 pad_04[4];
    u8 timer;
    u8 frame;
    u8 type;
    u8 pad_0B;
};

/* 0x03000000 */
extern u8 gCurrentPassage;
extern u8 gCurrentStageNumber;
extern s8 gCurrentStageID;
extern const u16 sHorizontalOscillationVelocity[];
extern u16 gRandomSeed;
extern u8 gCurrentPassageTemp;
extern u8 gClearMessage;
extern u8 gHasTemporarySave;
extern u8 gSaveFlag;
extern u8 gUnk_3000014;
extern u8 gTitleScreenStyle;
extern u8 gLanguage;
extern u8 gDifficulty;
extern u8 gResetSaveFile;
extern u8 gSelectedSaveFile;
extern s8 gUnk_300001A;
extern u8 gUnk_300001B;
extern u8 gUnk_300001C;
extern u8 gDisableSoftReset;

/* 0x03000020 */
extern u8 gUnk_3000020;
extern u8 gSpriteAiDropTimer;
extern u8 gUnk_3000022;
extern u8 gStageRoomTableIndex;
extern u8 gCurrentRoom;
extern u8 gUnk_3000025;
extern u8 gUnk_3000026;
extern u8 gUnk_3000027;
extern s8 gSpriteTileInteractionMode;
extern u16 gBldAlpha;
extern u16 gBldCnt;
extern u8 gSwitchStates[SWITCH_COUNT];
extern u8 gDrawWarioOverBackground;
extern u8 gUnk_3000034;
extern u8 gEscapeHBlankEffectState;
extern u8 gHBlankMode;
extern u8 gUnk_3000038;
extern u16 gUnk_300003A;
extern u16 gGoldenBossTransitionBg0Y;
extern u16 gClearedRoomTileCount;
extern u8 gShopItemState;
extern u8 gShopItemTimer;
extern u8 gUnk_3000046;
extern u8 gTimerState;
extern u8 gUnk_3000A62;
extern u8 gGoldenDivaShopItemHealthTarget;
extern u16 gSpriteAiRadius;
extern u8 gStageExitType;
extern struct GameScreenDebugInfo gGameScreenDebugInfo;
extern struct BackgroundInfo gBackgroundInfo;
extern struct RoomHeader gCurrentRoomHeader;
extern struct BackgroundCollisionData gBackgroundCollisionData;
extern struct BackgroundScroll gBackgroundScroll;
extern struct Window gWindow;
extern u8 gUnk_30000C8;
extern struct TransparencyState gUnk_30000D8;
extern u8 gColorFadingState[6];
extern u8 gGoldenDivaRoomTimer[6];
extern struct ScreenShakeParameters gScreenShakeY;
extern struct ScreenShakeParameters gScreenShakeX;

extern u8 gBossDefeatTimer[];
extern u8 gUnk_3000023;
extern struct BackgroundTileTables gBackgroundTileTables;
extern u8 gRoomTransitionTileDataCount;
// extern u8 gUnk_300342C;

/* 0x03000524 */
extern u8 gRoomSpriteIds[];

/* 0x03000544 */
extern u8 gRoomSpriteGraphicsSlots[];

/* 0x03000964 */
extern u8 gRoomSpriteData[][3];

/* 0x03000B80 */
extern struct SecondarySprite gSecondarySpriteData[8];
extern struct SecondarySprite gCurrentSecondarySprite;
extern GBA_PTR(const u8*) gCurrentSecondarySpriteOamData;
extern s16 gSecondarySpriteAffinePA;
extern s16 gSecondarySpriteAffinePB;
extern s16 gSecondarySpriteAffinePC;
extern s16 gSecondarySpriteAffinePD;

/* 0x0300188E */
extern u16 gUnk_300188E;

/* 0x03002C28 */
extern s16 gUnk_3002C28;
extern s16 gUnk_3002C2A;
extern u16 gUnk_3002C2C;
extern s16 gUnk_3002C2E;
extern u16 gUnk_3002C30;
extern u16 gUnk_3002C32[8];
extern s16 gUnk_3002C42;
extern s16 gUnk_3002C44;
extern s16 gCutsceneBackgroundHorizontalOffset;
extern s16 gCutsceneBackgroundVerticalOffset;
extern s16 gCutscenePrimaryObjectX;
extern s16 gCutscenePrimaryObjectY;
extern s16 gUnk_3002C4E;
extern s16 gUnk_3002C50;
extern s16 gUnk_3002C52;
extern s16 gUnk_3002C54;
extern s16 gUnk_3002C56;
extern s16 gUnk_3002C58;
extern s16 gUnk_3002C5A;
extern s16 gUnk_3002C5C;
extern s16 gUnk_3002C5E;

/* 0x03002C60 */
extern u16 gUnk_3002C60;
extern s32 gUnk_3002C64;
extern s32 gUnk_3002C68;
extern u32 gUnk_3002C6C;
extern GBA_PTR(OamData*) gUnk_3002C70;
extern u32 gUnk_3002C74;
extern s32 gUnk_3002C78;
extern s32 gUnk_3002C7C;
extern u16 gEndingCutsceneCollectedTreasureCount;
extern u16 gEndingCutsceneTreasureScale;
extern u16 gUnk_3002C84;
extern GBA_PTR(u16*) gUnk_3002C88;
extern GBA_PTR(u16*) gUnk_3002C8C;
extern GBA_PTR(const u16*) gUnk_3002C90;
extern GBA_PTR(const u16*) gUnk_3002C94;
extern s16 gCutsceneAnimationFinished;
extern s16 gUnk_3002C9A;
extern s16 gUnk_3002C9C;
extern s16 gUnk_3002C9E;
extern s16 gLayeredCutsceneOamOffsetX;
extern s16 gLayeredCutsceneOamOffsetY;
extern s16 gUnk_3002CA4;
extern s16 gUnk_3002CA6;
extern s16 gCutsceneObjectXPositions[8];
extern s16 gCutsceneObjectYPositions[8];

/* 0x030031F4 */
extern struct BackgroundTileTables gBackgroundTileTables;
extern struct GameMusicState gGameMusicState;
extern u16 gRoomCameraBounds[4];
extern struct RoomEffectState gLayer3ScrollEffectState;
extern struct RoomEffectState gBg0ScrollEffect;
extern u16 gHBlankPaletteFadeColors[3];
extern struct BackgroundPositionState gCameraPositionState;

/* 0x0300342C */
extern u8 gRoomTransitionTileDataCount;

/* 0x030037BE */
extern u8 gScrollingUpdateFrameCounter;
extern u8 gUnk_30037BF;

/* 0x03003BF5 */
extern u8 gUnk_3003BF5;
extern u8 gUnk_3003BF6;

/* 0x03003C39 */
extern u8 gStageEntrySequenceStep;
extern u8 gUnk_3003C4A;
extern u8 gUnk_3003C56;

/* Other IWRAM globals */
extern u8 gDemoState;
extern u8 gVideoMode;
extern u8 gVideoPage;
extern struct WarioData gWarioData;
extern u16 gButtonsPressed;
extern u16 gButtonsHeld;
extern u8 gOamSlotsUsed;
extern s8 gWarioMusicState;
extern struct WarioCollisionData gWarioCollisionData;

/* ROM data */
extern const s16 sWarioHitboxes[][4];
extern const struct WarioEffectFrame sWarioAfterimageFrames[];
extern const struct WarioEffectFrame sWarioGroundPoundLandingAfterimageFrames[];
extern GBA_PTR(const struct WarioAnimationFrame*) const sUnk_82DDB10[][2];
extern const struct WarioPoseProperty sUnk_82DDB60[];
extern const u16 sBouncyWarioPalette[];
extern const struct AnimationFrame sAerodentOam_83DABA0[];

extern u8 gInitialHealth;
extern u8 gPaletteFlashTimer;
extern u8 gBgAnimationFrame;
extern u8 gBgAnimationTimer;

extern s16 gSubGameMode;
extern s8 gStageEntrySoftResetDisabled;

extern struct SaveDataClearData gUnk_3004A70;

#endif /* GLOBAL_DATA_H */

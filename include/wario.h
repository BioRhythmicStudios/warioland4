#ifndef WARIO_H
#define WARIO_H

#include "gba.h"

#include "oam.h"

#define MAX_HEARTS (8)
#define HEART_GAUGE_MAX (8)

#define WarioRequestPose(pose) (GF(sWarioPoseRequestFuncTable[gWarioData.reaction])(pose))

typedef u8 (*WarioPoseFunc)(void);
typedef void (*WarioInteractionFunc)(u8);

enum WarioReaction {
    REACTION_NORMAL,
    REACTION_WATER,
    REACTION_FLAMING,
    REACTION_FAT,
    REACTION_FROZEN,
    REACTION_ZOMBIE,
    REACTION_SNOWMAN,
    REACTION_BOUNCY,
    REACTION_PUFFY,
    REACTION_BAT,
    REACTION_FLAT,
    REACTION_MASK,
    REACTION_COUNT
};

enum WarioPoseNormal {
    WPOSE_NORMAL_WALKING,
    WPOSE_NORMAL_TURNING,
    WPOSE_NORMAL_STANDING,
    WPOSE_NORMAL_JUMPING,
    WPOSE_NORMAL_FALLING,
    WPOSE_NORMAL_CROUCHING,
    WPOSE_NORMAL_CROUCH_SLIDE,
    WPOSE_NORMAL_CRAWLING,
    WPOSE_NORMAL_TURNING_CROUCHED,
    WPOSE_NORMAL_CROUCH_JUMP,
    WPOSE_NORMAL_SLIDING_ON_ICE,
    WPOSE_NORMAL_STOPPING_FROM_DASH,
    WPOSE_NORMAL_STARTING_ROLL,
    WPOSE_NORMAL_JUMPING_OUT_OF_ROLL,
    WPOSE_NORMAL_ROLLING,
    WPOSE_NORMAL_ROLLING_JUMP,
    WPOSE_NORMAL_ROLLING_BONK,
    WPOSE_NORMAL_ENTERING_PIPE_UP,
    WPOSE_NORMAL_ENTERING_PIPE_DOWN,
    WPOSE_NORMAL_TAKING_DAMAGE,
    WPOSE_NORMAL_BUMPING_INTO_ENEMY,
    WPOSE_NORMAL_ROLLING_IN_MIDAIR,
    WPOSE_NORMAL_SHOULDER_BASH,
    WPOSE_NORMAL_DASH_ATTACK,
    WPOSE_NORMAL_SHOULDER_BASH_JUMP,
    WPOSE_NORMAL_DASH_ATTACK_JUMP,
    WPOSE_NORMAL_SHOULDER_BASH_BONK,
    WPOSE_NORMAL_GROUND_POUND,
    WPOSE_NORMAL_SUPER_GROUND_POUND,
    WPOSE_NORMAL_GROUND_POUND_LANDING,
    WPOSE_NORMAL_SUPER_GROUND_POUND_LANDING,
    WPOSE_NORMAL_LANDING_ON_ENEMY,
    WPOSE_NORMAL_LIFTING_SPRITE,
    WPOSE_NORMAL_LIFTING_HEAVY_SPRITE,
    WPOSE_NORMAL_CHARGING_THROW_1,
    WPOSE_NORMAL_CHARGING_THROW_2,
    WPOSE_NORMAL_WEAK_THROW,
    WPOSE_NORMAL_CHARGED_THROW,
    WPOSE_NORMAL_WEAK_THROW_UPWARDS,
    WPOSE_NORMAL_TURNING_WHILE_CHARING_THROW,
    WPOSE_NORMAL_CLIMBING_LADDER,
    WPOSE_NORMAL_IDLE_ON_LADDER,
    WPOSE_NORMAL_SLIDING_DOWN_LADDER,
    WPOSE_NORMAL_CLIMBING_FENCE_VERTICAL,
    WPOSE_NORMAL_CLIMBING_FENCE_HORIZONTAL,
    WPOSE_NORMAL_IDLE_ON_FENCE,
    WPOSE_NORMAL_UNKNOWN_46,
    WPOSE_NORMAL_STUNNED,
    WPOSE_NORMAL_UNKNOWN_48,
    WPOSE_NORMAL_ENTERING_DOOR,
    WPOSE_NORMAL_EXITING_DOOR,
    WPOSE_NORMAL_LOOKING_UP,
    WPOSE_NORMAL_UNKNOWN_52,
    WPOSE_NORMAL_UNKNOWN_53,
    WPOSE_NORMAL_DUMBBELLS_1,
    WPOSE_NORMAL_DUMBBELLS_2,
    WPOSE_NORMAL_BARBELL,
    WPOSE_NORMAL_BARBELL_ENDING,
    WPOSE_NORMAL_UNKNOWN_58,
    WPOSE_NORMAL_JUMP_ROPE,
    WPOSE_NORMAL_UNKNOWN_60,
    WPOSE_NORMAL_BOSS_DEFEATED_MIDAIR,
    WPOSE_NORMAL_BOSS_DEFEATED,
    WPOSE_NORMAL_UNKNOWN_63,
    WPOSE_NORMAL_UNKNOWN_64,
    WPOSE_NORMAL_FLEXING,
    WPOSE_NORMAL_UNKNOWN_66,
    WPOSE_NORMAL_UNKNOWN_67,
    WPOSE_NORMAL_UNKNOWN_68,
    WPOSE_NORMAL_UNKNOWN_69,
    WPOSE_NORMAL_UNKNOWN_70,
    WPOSE_NORMAL_UNKNOWN_71,
    WPOSE_NORMAL_UNKNOWN_72,
    WPOSE_NORMAL_UNKNOWN_73,
    WPOSE_NORMAL_UNKNOWN_74,
    WPOSE_NORMAL_UNKNOWN_75,
    WPOSE_NORMAL_COUNT
};

enum WarioPoseWater {
    WPOSE_WATER_LANDING,
    WPOSE_WATER_SWIMMING_HORIZONTAL,
    WPOSE_WATER_SWIMMING_VERTICAL,
    WPOSE_WATER_SWIMMING_FREELY,
    WPOSE_WATER_FLOATING,
    WPOSE_WATER_JUMPING_OUT,
    WPOSE_WATER_TAKING_DAMAGE,
    WPOSE_WATER_SUPER_GROUND_POUND,
    WPOSE_WATER_SUPER_GROUND_POUND_LANDING,
    WPOSE_WATER_BONK_HORIZONTAL,
    WPOSE_WATER_BONK_VERTICAL,
    WPOSE_WATER_UNKNOWN_11,
    WPOSE_WATER_FAT_WARIO_FALLING,
    WPOSE_WATER_FAT_WARIO_LANDING,
    WPOSE_WATER_BUBBLE_WARIO,
    WPOSE_WATER_UNKNOWN_15,
    WPOSE_WATER_COUNT
};

enum WarioPoseFlaming {
    WPOSE_FLAMING_TRANSFORMING,
    WPOSE_FLAMING_BLUE,
    WPOSE_FLAMING_BLUE_MIDAIR,
    WPOSE_FLAMING_GREEN,
    WPOSE_FLAMING_GREEN_MIDAIR,
    WPOSE_FLAMING_RED,
    WPOSE_FLAMING_RED_MIDAIR,
    WPOSE_FLAMING_ENGULFED,
    WPOSE_FLAMING_ENGULFED_MIDAIR,
    WPOSE_FLAMING_RECOVERING,
    WPOSE_FLAMING_COUNT
};

enum WarioPoseFat {
    WPOSE_FAT_TRANSFORMING,
    WPOSE_FAT_TRANSFORMING_MIDAIR,
    WPOSE_FAT_TURNING,
    WPOSE_FAT_WALKING,
    WPOSE_FAT_JUMPING,
    WPOSE_FAT_FALLING,
    WPOSE_FAT_LANDING,
    WPOSE_FAT_KILLING_ENEMY,
    WPOSE_FAT_DETRANSFORMING,
    WPOSE_FAT_STARTING_WALK_AFTER_LANDING,
    WPOSE_FAT_UNKNOWN_10,
    WPOSE_FAT_COUNT
};

enum WarioPoseFrozen {
    WPOSE_FROZEN_TRANSFORMING,
    WPOSE_FROZEN_SLIDING,
    WPOSE_FROZEN_FALLING,
    WPOSE_FROZEN_BONK,
    WPOSE_FROZEN_COUNT
};

enum WarioPoseZombie {
    WPOSE_ZOMBIE_TRANSFORMING,
    WPOSE_ZOMBIE_WALKING,
    WPOSE_ZOMBIE_EMERGING_FROM_HAT,
    WPOSE_ZOMBIE_JUMPING,
    WPOSE_ZOMBIE_FALLING,
    WPOSE_ZOMBIE_UNKNOWN_5,
    WPOSE_ZOMBIE_FALLING_THROUGH_PLATFORM,
    WPOSE_ZOMBIE_LANDING,
    WPOSE_ZOMBIE_HAT_LANDING,
    WPOSE_ZOMBIE_HAT_FALLING,
    WPOSE_ZOMBIE_TOUCHING_LIGHT_SOURCE,
    WPOSE_ZOMBIE_UNKNOWN_11,
    WPOSE_ZOMBIE_FALLING_AFTER_JUMP,
    WPOSE_ZOMBIE_COUNT
};

enum WarioPoseSnowman {
    WPOSE_SNOWMAN_TRANSFORMING,
    WPOSE_SNOWMAN_FALLING_WITH_SNOW,
    WPOSE_SNOWMAN_WALKING,
    WPOSE_SNOWMAN_BONK,
    WPOSE_SNOWMAN_JUMPING,
    WPOSE_SNOWMAN_FALLING,
    WPOSE_SNOWMAN_LANDING,
    WPOSE_SNOWMAN_LANDING_AFTER_MIDAIR_SNOW_CONTACT,
    WPOSE_SNOWMAN_UNKNOWN_8,  // Either transformation finishing or touching slope
    WPOSE_SNOWMAN_TURNING,
    WPOSE_SNOWMAN_ROLLING_SMALL,
    WPOSE_SNOWMAN_ROLLING_SMALL_MIDAIR,
    WPOSE_SNOWMAN_ROLLING_MEDIUM,
    WPOSE_SNOWMAN_ROLLING_MEDIUM_MIDAIR,
    WPOSE_SNOWMAN_ROLLING_LARGE,
    WPOSE_SNOWMAN_ROLLING_LARGE_MIDAIR,
    WPOSE_SNOWMAN_COUNT
};

enum WarioPoseBouncy {
    WPOSE_BOUNCY_TRANSFORMING,
    WPOSE_BOUNCY_UNKNOWN_1,
    WPOSE_BOUNCY_JUMPING,
    WPOSE_BOUNCY_HITTING_CEILING,
    WPOSE_BOUNCY_STARTING_JUMP,
    WPOSE_BOUNCY_FALLING_FROM_CEILING,
    WPOSE_BOUNCY_FINAL_LANDING,
    WPOSE_BOUNCY_FINAL_BOUNCE,
    WPOSE_BOUNCY_TRANSFORMING_2,
    WPOSE_BOUNCY_HOPPING,
    WPOSE_BOUNCY_COUNT
};

enum WarioPosePuffy {
    WPOSE_PUFFY_TRANSFORMING,
    WPOSE_PUFFY_FLOATING,
    WPOSE_PUFFY_INHALING,
    WPOSE_PUFFY_HITTING_CEILING,
    WPOSE_PUFFY_COUNT
};

enum WarioPoseBat {
    WPOSE_BAT_TRANSFORMING,
    WPOSE_BAT_FLYING,
    WPOSE_BAT_UNKNOWN_2,
    WPOSE_BAT_ON_GROUND,
    WPOSE_BAT_TOUCHING_LIGHT_SOURCE,
    WPOSE_BAT_COUNT
};

enum WarioPoseFlat {
    WPOSE_FLAT_TRANSFORMING,
    WPOSE_FLAT_UNKNOWN_1,
    WPOSE_FLAT_STANDING,
    WPOSE_FLAT_WALKING,
    WPOSE_FLAT_FLOATING,
    WPOSE_FLAT_JUMPING,
    WPOSE_FLAT_DETRANSFORMING,
    WPOSE_FLAT_COUNT
};

enum WarioPoseMask {
    WPOSE_MASK_TRANSFORMING,
    WPOSE_MASK_UNKNOWN_1,
    WPOSE_MASK_COUNT
};

struct AGB_ALIGN WarioData {
    /* 0x00 */ u8 reaction;
    /* 0x01 */ u8 pose;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 damageTimer;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
    /* 0x0A */ u8 unk_0A;
    /* 0x0C */ u16 unk_0C;
    /* 0x0E */ u16 horizontalDirection;
    /* 0x10 */ u16 verticalDirection;
    /* 0x12 */ u16 xPosition;
    /* 0x14 */ u16 yPosition;
    /* 0x16 */ s16 xVelocity;
    /* 0x18 */ s16 yVelocity;
    /* 0x1A */ u8 unk_1A;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ u8 unk_1E;
    /* 0x1F */ u8 unk_1F;
    /* 0x20 */ GBA_PTR(const u16*) pOamData;
    /* 0x24 */ GBA_PTR(u8*) pObjData1;
    /* 0x28 */ u16 objData1Size;
    /* 0x2C */ GBA_PTR(u8*) pObjData2;
    /* 0x30 */ u16 objData2Size;
    /* 0x32 */ s16 hitboxOffsetLeft;
    /* 0x34 */ s16 hitboxOffsetTop;
    /* 0x36 */ s16 hitboxOffsetRight;
    /* 0x38 */ s16 hitboxOffsetBottom;
    /* 0x3A */ u8 unk_3A;
    /* 0x3B */ u8 unk_3B;
}; /* size: 0x3C */

struct AGB_ALIGN WarioLife {
    u8 current;
    u8 filling;
    u8 unk_2;
    u8 unk_3;
};

struct AGB_ALIGN WarioAfterimage {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    GBA_PTR(const u16*) unk4;
};

struct AGB_ALIGN WarioEffect {
    u8 type;
    u8 status;
    u8 animationTimer;
    u8 unk_3;
    GBA_PTR(void*) pObjData;
};

struct AGB_ALIGN CarriedSprite {
    u8 state;
    u8 unk1;
    u16 xOffset;
    u16 yOffset;
};

struct AGB_ALIGN WarioDustEffect {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u16 unk4;
    u16 unk6;
    GBA_PTR(struct AnimationFrameU16*) unk8;
};

struct AGB_ALIGN WarioAfterimageData {
    u16 previousXPositions[32];
    u16 previousYPositions[32];
};

struct AGB_ALIGN WarioAnimation {
    GBA_PTR(struct AnimationFrame*) left;
    GBA_PTR(struct AnimationFrame*) right;
};

extern struct WarioData gWarioData;
extern struct WarioData gWarioDataCopy;
extern struct WarioLife gHeartMeter;
extern struct WarioLife gHeartGauge;
extern struct WarioAfterimage gWarioMotionAfterimage;
extern struct WarioAfterimage gWarioDashAfterimage;
extern struct WarioEffect gCurrentWarioEffect;
extern struct CarriedSprite gCurrentCarriedSprite;
extern struct WarioDustEffect gWarioDustEffect1;
extern struct WarioDustEffect gWarioDustEffect2;
extern u8 gWarioDamagePaletteActive;
extern u8 gPreviousReaction;
extern u16 gPreviousXPosition;
extern u16 gPreviousYPosition;
extern u16 gWarioPauseTimer;
extern u8 gDisableWario;

extern struct WarioAfterimageData gWarioAfterimageHistory;
extern s16 gWarioAfterimageHistoryIndex;
extern s8 gWarioAfterimageHistoryWrapped;
extern s8 gWarioWaterCollisionActive;
extern s8 gWarioCanEnterDoor;
extern s8 gWarioCollisionDamageFlag;
extern s16 gWarioHorizontalCollisionOffset;

extern struct WarioData sStartingWarioData;
extern struct WarioLife sStartingHeartMeter;
extern struct WarioLife sStartingHeartGauge;
extern struct WarioAfterimage sEmptyWarioAfterimage;
extern struct WarioEffect sStartingWarioEffect;
extern struct CarriedSprite sEmptyCarriedSprite;
extern struct WarioDustEffect sEmptyDustEffect;
extern struct WarioAnimation sUnk_82DDCD0[];
extern u16 sWarioDefaultObjPalette[];
extern u16 sWarioGroundPoundPalette[];
extern GBA_FUNC_PTR(WarioPoseFunc) sWarioPoseHandlerTable[REACTION_COUNT];
extern GBA_FUNC_PTR(WarioInteractionFunc) sWarioPoseRequestFuncTable[REACTION_COUNT];
extern GBA_FUNC_PTR(WarioInteractionFunc) sUnk_82DED30[];
extern GBA_FUNC_PTR(WarioPoseFunc) sWarioNormalPoseTable[WPOSE_NORMAL_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sWarioWaterPoseTable[WPOSE_WATER_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sFlamingWarioPoseTable[WPOSE_FLAMING_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sFatWarioPoseTable[WPOSE_FAT_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sFrozenWarioPoseTable[WPOSE_FROZEN_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sZombieWarioPoseTable[WPOSE_ZOMBIE_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sSnowmanWarioPoseTable[WPOSE_SNOWMAN_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sBouncyWarioPoseTable[WPOSE_BOUNCY_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sPuffyWarioPoseTable[WPOSE_PUFFY_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sBatWarioPoseTable[WPOSE_BAT_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sFlatWarioPoseTable[WPOSE_FLAT_COUNT];
extern GBA_FUNC_PTR(WarioPoseFunc) sWarioMaskPoseTable[WPOSE_MASK_COUNT];
extern u8 sUnk_82DF094[];
extern u16 sWarioCollisionPointTable[][3];
extern u16 sCarriedSpriteXOffsets[];
extern u16 sCarriedSpriteYOffsets[];
extern GBA_FUNC_PTR(WarioPoseFunc) sUnk_82DECD0[];
extern GBA_FUNC_PTR(WarioPoseFunc) sUnk_82DED00[];
extern GBA_FUNC_PTR(WarioPoseFunc) sUnk_82DED60[];
extern GBA_FUNC_PTR(WarioPoseFunc) sUnk_82DED90[];

void PrepareWarioUpdate(void);
void CopyWarioPalette(const void* source, int destinationIndex, int size);
s32 GetAdjustedWarioXVelocity(void);
void UpdateWarioHorizontalCollisionOffset(void);
void UpdateCarriedSpriteOffsets(void);
void UpdateWarioPositionHistory(void);
void WarioProcessControls(void);
void WarioProcessCollision(void);
void ProcessWarioInteraction(void);
void FinalizeWarioUpdate(void);
void ResetWarioState(void);
void ProcessWarioInteraction(void);
void FinalizeWarioUpdate(void);

// void CopyWarioPalette(const void *src, u8 arg1, u8 arg2);
void UpdateWarioPositionHistory(void);
void ResetWarioState(void);
void ApplyNormalWarioPoseTransition(u8 pose);
u8 ResolveWarioCeilingCollision(void);
u8 ResolveWarioFloorCollision(void);
void func_8016614(u8 arg0);

#endif  // WARIO_H

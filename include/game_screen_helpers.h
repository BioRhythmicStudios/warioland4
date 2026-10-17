#ifndef GAME_SCREEN_HELPERS_H
#define GAME_SCREEN_HELPERS_H

#include "global_data.h"
#include "scrolling.h"
#include "types.h"

struct AGB_ALIGN TilesetLoadData {
    GBA_PTR(const void*) backgroundGraphics;
    u16 backgroundGraphicsSize;
    u16 padding06;
    GBA_PTR(const void*) palette;
    GBA_PTR(const void*) animatedGraphics;
    u16 animatedGraphicsSize;
    u16 padding12;
    GBA_PTR(const u16*) metatileTop;
    GBA_PTR(const u16*) metatileBottom;
    GBA_PTR(const u16*) metatileAttributes;
    GBA_PTR(const void*) extraPalette;
};

struct AGB_ALIGN RoomStartData {
    u8 type;
    u8 room;
    u8 x;
    u8 unk03;
    u8 unk04;
    u8 y;
    u8 unk06;
    s8 xOffset;
    s8 yOffset;
    u8 flags;
    u16 music;
};

extern const struct TilesetLoadData sTilesetLoadData[];
extern GBA_PTR(const struct RoomHeader*) const sRoomHeaderTables[];
extern GBA_PTR(const struct RoomStartData*) const sRoomStartDataTables[];
extern const u8 sBlankBackgroundTiles[];
extern const u8 sBossRoomBackgroundData[];
extern const u8 sBossPauseBackgroundTiles[];
extern const u8 sBossPauseAlternateBackgroundTiles[];
extern const u8 sEmptyRoomBackgroundTilemap[];
extern const u8 sBossRoomOverlayTiles[];
extern const u8 sGoldenBossRoomOverlayTiles[];
extern const u8 sBossRoomOverlayTilemap[];
extern const u8 sGoldenBossRoomOverlayTilemap[];
extern const u8 sStageRoomCounts[];
extern const u32 sWarioTransformationMusic[];

void ConfigureRoomDisplay(void);
u32 GetBackgroundScreenSize(u8 parameter);
void LoadRoom(void);
void LoadRoomBackgrounds(void);
void RestoreTemporarySaveRoomTiles(void);
void InitializeRoomState(void);
void InitializeRoomEffects(void);
void DrawRoomBackgroundLayer(u8 layer);
void PreloadStageRoomBackgrounds(void);
u32 DecompressRoomBackground(u8 type, const u8* source, u8* destination);
u32 LoadBossPauseBackgroundTiles(u8 useAlternateTiles);
void UpdateBossRoomState(void);
void InitializeRoomMusic(void);
void SetHurryUpMusic(u8 startMusic);
void ProcessRoomMusic(void);
void ProcessWarioTransformationMusic(void);
void SelectWarioTransformationMusic(void);
void FadeOutMusicForPause(void);
void ProcessGoldenPassageBossTransition(void);
void UpdateGoldenPassageBossTransitionTimer(void);
void ProcessRoomBackgrounds(void);
void ProcessRoomWindowEffect(void);
void UpdateRoomAnimatedGraphics(void);
void DrawGameScreen(void);
void func_806B410(void);
void func_806BF88(void);
void GameScreenDraw(void);
void FadeOutMusicForPause(void);

#endif  // GAME_SCREEN_HELPERS_H

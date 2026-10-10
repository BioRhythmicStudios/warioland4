#ifndef MAP_SCREEN_H
#define MAP_SCREEN_H

int UpdateMapScreen(void);
void InitMapScreen(void);
void StartMapScreenFromStageSelect(void);
void StartMapScreenAfterStageExit(void);
void StartMapScreenIntroSequence(void);
void StartMapScreenFinalPassageIntro(void);
void StartMapScreenBossUnlockSequence(void);
int UpdateMapScreenPassageInput(void);
void UpdateMapScreenBackgroundsAndText(void);
void ApplyMapScreenScrollVelocity(void);
void UpdateMapScreenPassageTransition(void);
void UpdateMapScreenIntroSequence(void);
void UpdateMapScreenBossUnlockSequence(void);
void UpdateMapScreenFinalPassageSequence(void);
int UpdateMapScreenStageStartTransition(void);
void AdvanceMapScreenWarioAnimation(void);
void AdvanceMapScreenDoorAnimation(void);
void UpdateMapScreenPaletteCycle(void);
void UpdateMapScreenTreasurePalettes(void);
void DrawMapScreenSprites(void);

#endif  // MAP_SCREEN_H

#include "sound_room.h"

#include "global_data.h"
#include "fixed_point.h"
#include "init_helpers.h"
#include "main.h"
#include "minigame.h"
#include "oam.h"
#include "score.h"
#include "stage_select.h"
#include "gba/m4a.h"

extern s16 gSoundRoomTileWaveOffset;
extern s8 gSoundRoomMenuState;
extern s8 gSoundRoomSelectedStage;
extern s8 gSoundRoomSelectedPassage;
extern u16 gSoundRoomTrackAnimState[];
extern u16 gSoundRoomPreviewAnimState[];
extern s8 gSoundRoomPreviewRequest;
extern s8 gSoundRoomBonusTrackUnlocked;
extern s8 gSoundRoomMusicEnabled;
extern s8 gSoundRoomPitchOffset;
extern s8 gSoundRoomTempoOffset;
extern s16 gSoundRoomMessageTimer;
extern u16 gStageEntryMainSpriteState[];
extern u16 gStageEntrySequenceTimer;
extern u16 gStageEntryWindowLeft;
extern u16 gStageEntryWindowRight;
extern u16 gStageEntryWindowTop;
extern u16 gStageEntryWindowBottom;

extern const u8 sUnk_86B2888[];
extern const u8 sUnk_86B3088[];
extern const u8 sUnk_86B3888[];
extern const u8 sUnk_86B4088[];
extern const u8 sUnk_86B4288[];
extern const u8 sUnk_86B8A88[];
extern const u8 sUnk_86BF288[];
extern const u8 sUnk_86BF488[];
extern const u8 sUnk_86CB7C8[];
extern const u8 sUnk_86CB908[];
extern const u8 sUnk_86D3108[];
extern const u8 sUnk_86D3226[];
extern const u32 sUnk_863D920[];
extern const u32 sUnk_863D948[];
extern const u32 sUnk_863D8A0[];
extern const u32 sUnk_863D8E0[];
extern const u32 sUnk_863C694[];
extern const u32 sUnk_863C6D4[];
extern const u32 sUnk_863C714[];
extern const u32 sUnk_863C754[];
extern const u32 sUnk_863C794[];
extern const u32 sUnk_863C7D4[];
extern const u32 sUnk_863C814[];
extern const u32 sUnk_863C854[];
extern const u32 sUnk_863C85C[];
extern const u16 sUnk_863D85C[];
extern const u16 sUnk_863D97C[];
extern const s16 sUnk_863D984[];
extern const u16 sUnk_863D9A0[];
extern const struct AnimationFrame sUnk_8642860[];
extern const u8 *const sUnk_864261C[];
extern const u8 *const sUnk_864262A[];
extern const u8 *const sUnk_8642638[];
extern const u8 *const sUnk_86427A0[];
extern const u16 sUnk_86427BA[];
extern const u16 sUnk_8642820[];
extern const u16 sUnk_8642898[];

void Shop_DrawText(const u8 *text, u32 destination, s32 length);

s32 UpdateSoundRoomTransition(void)
{
    s32 result;
    u8 *step;

    step = &gStageEntrySequenceStep;
    switch (*step)
    {
        case 0:
            result = ExpandMinigameWindow();
            if (result != 0)
                (*step)++;
            break;
        case 1:
            result = UpdateSoundRoomMenu();
            if (result != 0)
                (*step)++;
            break;
        case 2:
            result = ContractMinigameWindow();
            if (result != 0)
                return 1;
            break;
    }
    return 0;
}

void InitSoundRoomScreen(void)
{
  vu32 *dma;
  long long new_var;
  register u32 busy asm("r2");
  s32 passedScoreCount;
  register s32 passage asm("r1");
  s32 nextPassage;
  s32 minigame;
  s32 *score;
  s16 *bgReg;
  s8 *clearCounter0;
  s8 *clearCounter1;
  s8 *clearCounter2;
  u16 *clearPair;
  s32 (*highScores)[4];
  s32 scoreThreshold;
  u32 scoreOffset;
  u32 dmaStatus;
  InitializeVideoMemory();
  dma = (vu32 *) 0x040000D4;
  dma[0] = (u32) sUnk_86B4088;
  dma[1] = 0x05000000;
  dma[2] = 0x80000100;
  dma[2];
  busy = 0x80000000;
  while ((dma[2] & busy) != 0)
  {
  }

  dma = (vu32 *) 0x040000D4;
  dma[0] = (u32) sUnk_86B4088;
  dma[1] = 0x05000200;
  dma[2] = 0x80000100;
  dma[2];
  dmaStatus = dma[2];
  busy = 0x80000000;
  {
    u8 *tileset0;
    u8 *tileset1;
    u8 *tilemap0;
    u8 *tilemap1;
    u8 *tilemap2;
    tileset0 = sUnk_86B8A88;
    tileset1 = sUnk_86B4288;
    tilemap0 = sUnk_86B2888;
    tilemap1 = sUnk_86B3088;
    tilemap2 = sUnk_86B3888;
    if (((s32) dmaStatus) < 0)
    {
      while ((dma[2] & busy) != 0)
      {
      }

    }
    *((vu16 *) 0x05000000) = 0;
    dma = (vu32 *) 0x040000D4;
    dma[0] = (u32) tileset0;
    dma[1] = 0x06000000;
    dma[2] = 0x80003400;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0)
    {
    }

    dma = (vu32 *) 0x040000D4;
    dma[0] = (u32) tileset1;
    dma[1] = 0x06011800;
    dma[2] = 0x80002400;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0)
    {
    }

    dma = (vu32 *) 0x040000D4;
    dma[0] = (u32) tilemap0;
    dma[1] = 0x0600C000;
    dma[2] = 0x80000400;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0)
    {
    }

    dma = (vu32 *) 0x040000D4;
    dma[0] = (u32) tilemap1;
    dma[1] = 0x0600D000;
    dma[2] = 0x80000400;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0)
    {
    }

    dma = (vu32 *) 0x040000D4;
    dma[0] = (u32) tilemap2;
    dma[1] = 0x0600E000;
    dma[2] = 0x80000400;
    dma[2];
  }
  bgReg = (s16 *) 0x04000008;
  bgReg[0] = 0x1800;
  bgReg++;
  bgReg[0] = 0x1A01;
  asm volatile("add r1, #2");
  bgReg[0] = 0x1C02;
  InitMinigameScoreDisplay();
  *((vu16 *) 0x04000054) = 0;
  *((vu16 *) 0x04000000) = 0x1700;
  gStageEntryWindowLeft = 0x78;
  gStageEntryWindowRight = 0x78;
  gStageEntryWindowTop = 0;
  gStageEntryWindowBottom = 0xA0;
  *((vu16 *) 0x04000000) |= 0x2000;
  *((vu16 *) 0x04000048) = 0x3F;
  *((vu16 *) 0x0400004A) = 0;
  gStageEntrySequenceTimer = 0;
  gStageEntrySequenceStep = 0;
  gSoundRoomMessageTimer = 0;
  *((vu16 *) 0x04000012) = 0;
  *((vu16 *) 0x04000010) = 0;
  *((vu16 *) 0x04000016) = 0;
  *((vu16 *) 0x04000014) = 0;
  *((vu16 *) 0x0400001A) = 0;
  *((vu16 *) 0x04000018) = 0;
  gSoundRoomTileWaveOffset = 0;
  gSoundRoomMenuState = 0;
  gSoundRoomSelectedStage = 0;
  gSoundRoomSelectedPassage = 0;
  gStageEntryMainSpriteState[4] = 0x14;
  gStageEntryMainSpriteState[5] = 0x10;
  gStageEntryMainSpriteState[0] = 0;
  gStageEntryMainSpriteState[1] = 0;
  ((u16 *) gSoundRoomTrackAnimState)[0] = 0;
  ((u16 *) gSoundRoomTrackAnimState)[1] = 0;
  gSoundRoomPreviewRequest = 0;
  gSoundRoomBonusTrackUnlocked = 0;
  passedScoreCount = 0;
  passage = 0;
  clearCounter0 = &gSoundRoomMusicEnabled;
  clearCounter1 = &gSoundRoomPitchOffset;
  clearCounter2 = &gSoundRoomTempoOffset;
  clearPair = (u16 *) gSoundRoomPreviewAnimState;
  highScores = gHighScoreTable;
  scoreThreshold = 0x3E7;
  do
  {
    scoreOffset = passage << 4;
    nextPassage = passage + 1;
    score = (s32 *) (scoreOffset + (u32) highScores);
    minigame = 3;
    do
    {
      if ((*score) > scoreThreshold)
      {
        passedScoreCount++;
      }
      score++;
      minigame--;
    }
    while (minigame >= 0);
    passage = (new_var = nextPassage);
  }
  while (passage <= 5);
  if (passedScoreCount > 0x11)
  {
    gSoundRoomBonusTrackUnlocked = 1;
  }
  *clearCounter0 = 0;
  *clearCounter1 = 0;
  *clearCounter2 = 0;
  clearPair[0] = 0;
  clearPair[1] = 0;
  m4aSongNumStart(0x26A);
}

asm(".set $t, InitSoundRoomScreen + 0x240\n"
    ".set $d, InitSoundRoomScreen + 0x246\n");

void SoundRoomVBlank(void)
{
  vu16 *window;
  register u32 soundValue asm("r2");
  u16 *row0;
  int new_var;
  u16 *row1;
  s32 i;
  s32 random;
  s32 adjusted;
  u32 tileOffset;
  const u8 *text;
  const u32 *trackTable;
  new_var = 4;
  window = (vu16 *) 0x04000040;
  window[0] = (gStageEntryWindowLeft << 8) | gStageEntryWindowRight;
  window += 2;
  window[0] = (gStageEntryWindowTop << 8) | gStageEntryWindowBottom;
  switch ((u8) gSoundRoomMenuState)
  {
    case 1:
      switch (gLanguage)
    {
      case 0:
        text = sUnk_86D3226;
        break;

      case 1:
        asm volatile("" ::: "memory");
        text = sUnk_86D3108;
        break;

      default:
        return;

    }

      goto drawText;

    case 2:
    {
      register s32 li asm("r5");
      u32 tileMask;
      u32 paletteBase;
      register u32 paletteLoad asm("r0");
      soundValue = gMPlayMemAccArea[0];
      li = 1;
      tileMask = 0x0FFF;
      paletteLoad = 0x2000;
      asm volatile("" : "=r"(paletteBase) : "0"(paletteLoad));
      row0 = (u16 *) 0x0600C444;
      row1 = (u16 *) 0x0600C484;
      do
      {
        *row0 &= tileMask;
        *row0 += paletteBase;
        *row1 &= tileMask;
        *row1 += paletteBase;
        row0++;
        row1++;
        li++;
      }
      while (li <= 26);
      if (soundValue == 0)
      {
        gSoundRoomMessageTimer = soundValue;
        text = sUnk_86D3108;
        goto drawTrackText;
      }
      adjusted = soundValue;
      if (adjusted > 100)
      {
        gSoundRoomMessageTimer = 1000;
        switch (gLanguage)
        {
          case 0:
            trackTable = sUnk_863D948;
            goto drawTrackName;
          case 1:
            trackTable = sUnk_863D920;
            break;
          default:
            goto updateRandomTrack;
        }
      drawTrackName:
        {
          const u32 *trackTableReg;
          u32 trackIndex;
          trackTableReg = trackTable;
          trackIndex = adjusted;
          trackIndex -= 101;
          text = (const u8 *) trackTableReg[trackIndex];
        }
      drawTrackText:
        Shop_DrawText(text, 0x7800, 26);
        goto updateRandomTrack;
      }
      if (adjusted <= 0)
      {
        goto updateRandomTrack;
      }
      {
        u32 tileOffsetReg;
        u16 *row0Reg;
        u16 *row1Reg;

        tileOffsetReg = adjusted << 1;
        row0Reg = (u16 *) (0x0600C442 + tileOffsetReg);
        *row0Reg = ((*row0Reg) & 0x0FFF) + 0xF000;
        row1Reg = (u16 *) (0x0600C482 + tileOffsetReg);
        *row1Reg = ((*row1Reg) & 0x0FFF) + 0xF000;
      }
      updateRandomTrack:
      if ((gStageEntrySequenceTimer % 300) == 0)
      {
        {
          s32 randomOriginal;
          random = MinigameRandom();
          randomOriginal = random;
          if (randomOriginal < 0)
          {
            random += 15;
          }
          LoadSoundRoomTrackGraphics(randomOriginal - ((random >> 4) << (new_var = 4)));
        }
      }

      return;
    }

    case 4:
      i = gStageEntrySequenceTimer % 600;
      if ((i == 0) || (i == 300))
    {
      LoadRandomSoundRoomPreview();
    }
      break;

    default:
      break;

  }

  switch (gLanguage)
  {
    case 0:
    {
      const u32 *languageTable;
      u32 languageIndex;
      u32 languageTemp;

      languageTable = sUnk_863D8E0;
      languageIndex = (u8) gSoundRoomSelectedStage;
      languageIndex <<= 2;
      languageTemp = (u8) gSoundRoomSelectedPassage;
      languageTemp <<= 4;
      languageIndex += languageTemp;
      languageIndex += (u32) languageTable;
      languageTemp = *(const u32 *) languageIndex;
      text = (const u8 *) languageTemp;
    }

    drawText:
      Shop_DrawText(text, 0x7800, 26);
      break;

    case 1:
    {
      const u32 *languageTable;
      u32 languageIndex;
      register u32 languageTemp asm("r0");

      languageTable = sUnk_863D8A0;
      languageIndex = (u8) gSoundRoomSelectedStage;
      languageIndex <<= 2;
      languageTemp = (u8) gSoundRoomSelectedPassage;
      languageTemp <<= 4;
      languageIndex += languageTemp;
      languageIndex += (u32) languageTable;
      languageTemp = *(const u32 *) languageIndex;
      Shop_DrawText((const u8 *) languageTemp, 0x7800, 26);
      break;
    }

    default:
      return;
  }

}

asm(".set $t, UpdateSoundRoomMenu + 0x258\n");

s32 UpdateSoundRoomMenu(void)
{
  u8 *new_var5;
  u16 buttons;
  int new_var7;
  s32 selected;
  u32 passageOffset;
  int new_var2;
  s32 value;
  int i;
  s32 adjusted;
  int new_var12;
  unsigned long new_var10;
  short new_var8;
  u16 *row0;
  int new_var9;
  u32 new_var4;
  u16 *row1;
  switch ((u8) gSoundRoomMenuState)
  {
    case 0:
    {
      register u16 *clearTarget asm("r3");
      register u32 clearValue asm("r2");
      clearTarget = &gSoundRoomMessageTimer;
      clearValue = 0;
      *clearTarget = clearValue;
    }
      if ((*((u8 *) (&gSoundRoomBonusTrackUnlocked))) != 0)
    {
      buttons = gButtonsPressed;
      if ((buttons & 0x30) != 0)
      {
        m4aSongNumStart(0x124);
        gSoundRoomMenuState = 1;
        gStageEntryMainSpriteState[4] = 60;
        gStageEntryMainSpriteState[5] = 16;
        goto checkSecret;
      }
      value = 0x80;
    }
    else
    {
      buttons = gButtonsPressed;
      value = 0x90;
    }
      if (((value & buttons) != 0) && (SelectFirstUnlockedSoundRoomTrack() != 0))
    {
      m4aSongNumStart(0x124);
      gSoundRoomMenuState = 3;
      {
        u16 *spriteState;
        register const u8 *graphicsTable asm("r2");
        u8 *stagePtr;
        u8 *passagePtr;
        register u32 offset asm("r1");
        register u32 tableValue asm("r0");
        spriteState = gStageEntryMainSpriteState;
        graphicsTable = (const u8 *) sUnk_863C694;
        stagePtr = (u8 *) (&gSoundRoomSelectedStage);
        offset = *stagePtr;
        offset <<= 2;
        passagePtr = (u8 *) (&gSoundRoomSelectedPassage);
        tableValue = *passagePtr;
        tableValue <<= 4;
        offset += tableValue;
        offset += (u32) graphicsTable;
        tableValue = *((const u32 *) offset);
        spriteState[4] = tableValue;
        graphicsTable = (const u8 *) sUnk_863C6D4;
        tableValue = *stagePtr;
        tableValue <<= 2;
        offset = *passagePtr;
        offset <<= 4;
        tableValue += offset;
        tableValue += (u32) graphicsTable;
        tableValue = *((const u32 *) tableValue);
        spriteState[5] = tableValue;
      }
    }
      checkSecret:
    if ((gButtonsHeld & 0x34C) == 0x34C)
    {
      gSoundRoomBonusTrackUnlocked = 1;
    }

      buttons = gButtonsPressed;
      value = 3;
      goto checkExitButtons;

    case 3:
      gSoundRoomMessageTimer = 1000;
      MoveSoundRoomSelection();
      buttons = gButtonsPressed;
      if ((buttons & 1) != 0)
    {
      const u8 *trackTable;
      u8 *stagePtr;
      u8 *passagePtr;
      s32 selectedTrack;
      register u32 offset asm("r1");
      u32 temp;
      gStageEntrySequenceTimer = 0;
      trackTable = (const u8 *) sUnk_863C714;
      stagePtr = (u8 *) (&gSoundRoomSelectedStage);
      offset = *stagePtr;
      offset <<= 2;
      passagePtr = (u8 *) (&gSoundRoomSelectedPassage);
      temp = *passagePtr;
      temp <<= 4;
      offset += temp;
      offset += (u32) trackTable;
      selectedTrack = *((const s32 *) offset);
      m4aSongNumStart(0x120);
      temp = (u32) sUnk_863D85C;
      selectedTrack++;
      selectedTrack <<= 2;
      selectedTrack += temp;
      temp = *((const u16 *) selectedTrack);
      m4aSongNumStart(temp);
      gSoundRoomMenuState = 4;
      gSoundRoomTrackAnimState[0] = 0;
      gSoundRoomTrackAnimState[1] = 0;
      temp = *stagePtr;
      temp <<= 2;
      offset = *passagePtr;
      offset <<= 4;
      temp += offset;
      temp += (u32) trackTable;
      temp = *((const s32 *) temp);
      LoadSoundRoomTrackGraphics(temp);
      break;
    }
      value = 2;
      goto checkExitButtons;

    case 4:
    {
      u16 *cycleOffset;
      gStageEntrySequenceTimer++;
      gSoundRoomMessageTimer = 1000;
      cycleOffset = &gSoundRoomTileWaveOffset;
      *cycleOffset = (*cycleOffset + 4) & 0xFF;
      if (((gButtonsPressed & 3) != 0) || (gMPlayMemAccArea[0] == 0x7F))
      {
        u32 zero;
        zero = 0;
        gMPlayMemAccArea[0] = zero;
        m4aSongNumStart(0x26A);
        m4aSongNumStart(0x121);
        gSoundRoomMenuState = 3;
        *cycleOffset = zero;
      }
      break;
    }

    case 1:
      gSoundRoomMessageTimer = 1000;
      ;
      if ((gButtonsPressed & 0x30) != 0)
    {
      gSoundRoomMessageTimer = 0;
      m4aSongNumStart(0x124);
      gSoundRoomMenuState = 0;
      {
        u16 *spriteStateBase;
        spriteStateBase = (u16 *) 0x03003C0C;
        spriteStateBase[4] = 20;
        spriteStateBase[5] = 16;
      }
    }
    else
    {
      new_var5 = (u8 *) sUnk_863C6D4;
      if (((gButtonsPressed & 0x80) != 0) && (SelectFirstUnlockedSoundRoomTrack() != 0))
      {
        m4aSongNumStart(0x124);
        gSoundRoomMenuState = 3;
        {
          u16 *spriteState;
          register const u8 *graphicsTable asm("r2");
          u8 *stagePtr;
          u8 *passagePtr;
          register u32 offset asm("r1");
          register u32 tableValue asm("r0");
          spriteState = gStageEntryMainSpriteState;
          graphicsTable = (const u8 *) sUnk_863C694;
          stagePtr = (u8 *) (&gSoundRoomSelectedStage);
          offset = *stagePtr;
          offset <<= 2;
          passagePtr = (u8 *) (&gSoundRoomSelectedPassage);
          tableValue = *passagePtr;
          tableValue <<= 4;
          offset += tableValue;
          offset += (u32) graphicsTable;
          tableValue = *((const u32 *) offset);
          spriteState[4] = tableValue;
          graphicsTable = (const u8 *) sUnk_863C6D4;
          tableValue = *stagePtr;
          tableValue <<= 2;
          offset = *passagePtr;
          offset <<= 4;
          tableValue += offset;
          tableValue += (u32) graphicsTable;
          tableValue = *((const u32 *) tableValue);
          spriteState[5] = tableValue;
        }
      }
    }
      if (((new_var12 = gButtonsPressed) & 1) != 0)
    {
      new_var7 = MinigameRandom();
      value = new_var7;
      adjusted = value;
      if (value < 0)
      {
        adjusted += 15;
      }
      LoadSoundRoomTrackGraphics(value - ((adjusted >> 4) << 4));
      gStageEntrySequenceTimer = 0;
      gSoundRoomMessageTimer = 0;
      m4aSongNumStart(0x120);
      m4aSongNumStart(0x2D9);
      {
        const u8 *languageSongTable;
        u32 languageSong;
        languageSongTable = (const u8 *) sUnk_863D97C;
        languageSong = gLanguage;
        languageSong <<= 2;
        languageSong += (u32) languageSongTable;
        languageSong = *((const u16 *) languageSong);
        m4aSongNumStart(languageSong);
      }
      gSoundRoomMenuState = 2;
      break;
    }
      buttons = gButtonsPressed;
      value = 2;

checkExitButtons:
      value &= buttons;
      if (value != 0)
      {
        m4aSongNumStart(0x129);
        return 1;
      }
      break;

    case 2:
      gStageEntrySequenceTimer++;
      if ((gButtonsPressed & 0x10) != 0)
    {
      gSoundRoomTempoOffset++;
    }
      if ((gButtonsPressed & 0x20) != 0)
    {
      gSoundRoomTempoOffset--;
    }
      if ((gButtonsPressed & 0x40) != 0)
    {
      gSoundRoomPitchOffset++;
    }
      if ((gButtonsPressed & 0x80) != 0)
    {
      gSoundRoomPitchOffset--;
    }
      if ((gButtonsPressed & 0x300) != 0)
    {
      gSoundRoomMusicEnabled = gSoundRoomMusicEnabled ^ 1;
      m4aSongNumStart(0x11D);
    }
      if (gSoundRoomTempoOffset > 3)
    {
      gSoundRoomTempoOffset = 3;
    }
      if (gSoundRoomTempoOffset < (-3))
    {
      gSoundRoomTempoOffset = -3;
    }
      if (gSoundRoomPitchOffset > 3)
    {
      gSoundRoomPitchOffset = 3;
    }
      if (gSoundRoomPitchOffset < (-3))
    {
      gSoundRoomPitchOffset = -(new_var7 = 3);
    }
      gSoundRoomPreviewAnimState[0]++;
      if (sUnk_8642860[gSoundRoomPreviewAnimState[1]].time < gSoundRoomPreviewAnimState[0])
    {
      gSoundRoomPreviewAnimState[0] = 0;
      gSoundRoomPreviewAnimState[1]++;
      if (sUnk_8642860[gSoundRoomPreviewAnimState[1]].time == 0)
      {
        gSoundRoomPreviewAnimState[1] = 0;
      }
    }
      if (((gButtonsPressed & 3) != 0) || (gMPlayMemAccArea[0] == 0x7F))
    {
      m4aSongNumStop(0x2D9);
      gSoundRoomMessageTimer = 0;
      gMPlayMemAccArea[0] = 0;
      m4aSongNumStart(0x26A);
      m4aSongNumStart(0x121);
      gSoundRoomMenuState = 1;
      gSoundRoomTempoOffset = 0;
      gSoundRoomPitchOffset = 0;
      gSoundRoomMusicEnabled = 0;
      {
        int tileIndex;
        register unsigned long paletteBits asm("r5");
        register int paletteTemp asm("r2");
        tileIndex = 1;
        new_var4 = 0x0FFF;
        paletteTemp = 0x2000;
        asm volatile("" : : "r"(paletteTemp));
        paletteBits = paletteTemp;
        asm volatile("" : "+r"(tileIndex), "+r"(new_var4), "+r"(paletteBits));
        row0 = (u16 *) 0x0600C444;
        row1 = (u16 *) 0x0600C484;
        do
        {
          {
            u32 maskedTile;
            register u16 tileValue asm("r1");
            tileValue = *row0;
            maskedTile = new_var4;
            maskedTile &= tileValue;
            maskedTile += paletteBits;
            *row0 = maskedTile;
            tileValue = *row1;
            maskedTile = new_var4;
            maskedTile &= tileValue;
            maskedTile += paletteBits;
            *row1 = maskedTile;
          }
          row0++;
          row1++;
          tileIndex++;
        } while (tileIndex <= 26);
      }

      gSoundRoomPreviewAnimState[0] = 0;
      gSoundRoomPreviewAnimState[1] = 0;
    }
      UpdateSoundRoomAudioEffects();
      break;

  }

  return 0;
}

s32 SelectFirstUnlockedSoundRoomTrack(void)
{
  u8 *selectedPassage;
  u8 *selectedStage;
  s32 passage;
  s32 nextPassage;
  s32 stage;
  u8 *collection;
  selectedPassage = &gSoundRoomSelectedPassage;
  if ((*selectedPassage) != 0)
  {
    goto returnTrue;
  }
  {
    u8 *stagePtr;
    stagePtr = &gSoundRoomSelectedStage;
    if ((*stagePtr) == 0)
    {
      selectedStage = stagePtr;
      goto search;
    }
    selectedStage = stagePtr;
  }
  goto returnTrue;
  found:
  *(&gSoundRoomSelectedPassage) = passage;

  *selectedStage = stage;
  returnTrue:
  return 1;

  search:
  passage = 0;

  collection = (u8 *) gCurrentCollection;
  do
  {
    stage = 0;
    nextPassage = passage + 1;
    collection = ((u8 *) gCurrentCollection) + ((nextPassage * 3) << 3);
    do
    {
      if (((s32) ((*collection) << 27)) < 0)
      {
        goto found;
      }
      collection += 4;
      stage++;
    }
    while (stage <= 3);
    passage = nextPassage;
  }
  while (passage <= 3);
  return 0;
}

s32 MoveSoundRoomSelection(void)
{
  s32 currentItem;
  register u16 buttons asm("r2");
  const u32 *new_var;
  u8 *new_var3;
  const u32 *selectionTable;
  s32 direction;
  register const u32 *neighbors asm("r1");
  const u32 *list;
  register const u32 *listCursor asm("r5");
  const u32 *passageTable;
  s32 candidate;
  s32 passage;
  short new_var2;
  s32 stage;
  const u8 *collection;
  s8 *new_var4;
  selectionTable = sUnk_863C714;
  currentItem = *((s32 *) ((((*((u8 *) (&gSoundRoomSelectedStage))) << 2) + ((*((u8 *) (&gSoundRoomSelectedPassage))) << 4)) + ((s32) selectionTable)));
  buttons = gButtonsPressed;
  direction = ((buttons & 0x40) != 0) ? (4) : (0);
  if ((buttons & 0x10) != 0)
  {
    direction = 1;
  }
  if ((buttons & 0x80) != 0)
  {
    direction = 3;
  }
  if ((buttons & 0x20) != 0)
  {
    direction = 2;
  }
  if (direction != 0)
  {
    {
      register const u8 *neighborsBase asm("r2");
      s32 directionOffset;
      neighborsBase = sUnk_863C85C;
      currentItem <<= 6;
      directionOffset = (direction - 1) << 10;
      currentItem += directionOffset;
      currentItem += (s32) neighborsBase;
      neighbors = (const u32 *) currentItem;
    }
    if ((*neighbors) != 99)
    {
      passageTable = sUnk_863C754;
      collection = (const u8 *) gCurrentCollection;
      list = neighbors;
      listCursor = list;
      do
      {
        candidate = *listCursor;
        stage = passageTable[candidate];
        {
          register const u32 *stageTable asm("r0");
          stageTable = sUnk_863C794;
          passage = stageTable[candidate];
        }
        {
          s32 entryOffset;
          entryOffset = stage << 2;
          entryOffset += ((passage + 1) * 3) << 3;
          entryOffset += (s32) collection;
          new_var2 = ((s32) (((u32) (*((u8 *) entryOffset))) << 27)) < 0;
        }
        if (new_var2)
        {
          goto found;
        }
        list++;
        listCursor++;
      }
      while ((*list) != 99);
    }
    if (direction == 4)
    {
      gSoundRoomMenuState = 0;
      m4aSongNumStart(0x124);
      gStageEntryMainSpriteState[4] = 20;
      gStageEntryMainSpriteState[5] = 16;
    }
  }
  return 0;
  found:
  m4aSongNumStart(0x124);

  {
    u8 *passagePtr;
    u8 *stagePtr;
    u16 *spriteState;
    register const u8 *table asm("r4");
    register u32 stageOffset asm("r0");
    register u32 passageOffset asm("r1");
    u32 zero;

    passagePtr = &gSoundRoomSelectedPassage;
    *passagePtr = passage;
    stagePtr = &gSoundRoomSelectedStage;
    *stagePtr = stage;
    spriteState = gStageEntryMainSpriteState;
    table = (const u8 *)sUnk_863C694;
    stageOffset = *stagePtr;
    stageOffset <<= 2;
    passageOffset = *passagePtr;
    passageOffset <<= 4;
    stageOffset += passageOffset;
    stageOffset += (u32)table;
    stageOffset = *((const u32 *)stageOffset);
    zero = 0;
    spriteState[4] = stageOffset;
    table = (const u8 *)sUnk_863C6D4;
    stageOffset = *stagePtr;
    stageOffset <<= 2;
    passageOffset = *passagePtr;
    passageOffset <<= 4;
    stageOffset += passageOffset;
    stageOffset += (u32)table;
    stageOffset = *((const u32 *)stageOffset);
    spriteState[5] = stageOffset;
    gSoundRoomMessageTimer = zero;
  }
  return 0;
}

void UpdateSoundRoomAudioEffects(void)
{
  u16 songId;
  u16 pairedSongId;
  struct MusicPlayerInfo *firstPlayer;
  struct MusicPlayerInfo *secondPlayer;
  u8 *new_var;
  u16 *new_var2;
  u16 tempo;
  s16 pitch;
  const u16 *new_var3;
  const u32 *songIndexTable;
  u32 pitchMask;
  const s16 *pitchTable;
  s8 *pitchAdjustPtr;
  new_var = (u8 *) gSongTable;
  songId = *((u16 *) (new_var + 0x16CC));
  songIndexTable = (const u32 *)sUnk_863D97C;
  pairedSongId = ((const struct Song *) gSongTable)[songIndexTable[gLanguage]].ms;
  if (*(u8 *)&gSoundRoomMusicEnabled != 0)
  {
    m4aMPlayVolumeControl(gMPlayTable[songId].info, 0xFFFF, 0x100);
  }
  else
  {
    m4aMPlayVolumeControl(gMPlayTable[songId].info, 0xFFFF, 0);
  }
  firstPlayer = gMPlayTable[songId].info;
  new_var3 = sUnk_863D9A0;
  new_var2 = &(*((u16 *) (((u8 *) new_var3) + ((((s8) gSoundRoomTempoOffset) + 3) << 2))));
  tempo = *new_var2;
  m4aMPlayTempoControl(firstPlayer, tempo);
  songId = 2;
  secondPlayer = gMPlayTable[pairedSongId].info;
  tempo = *((u16 *) (((u8 *) sUnk_863D9A0) + ((((s8) gSoundRoomTempoOffset) + 3) << songId)));
  m4aMPlayTempoControl(secondPlayer, tempo);
  pitchMask = 0xFFFF;
  pitchTable = sUnk_863D984;
  pitchAdjustPtr = &gSoundRoomPitchOffset;
  pitch = *((s16 *) (((u8 *) pitchTable) + (((*pitchAdjustPtr) + 3) << 2)));
  m4aMPlayPitchControl(firstPlayer, pitchMask, pitch);
  pitch = *((s16 *) (((u8 *) pitchTable) + (((*pitchAdjustPtr) + 3) << 2)));
  m4aMPlayPitchControl(secondPlayer, pitchMask, pitch);
}

void LoadSoundRoomTrackGraphics(s32 index)
{
    int new_var;
    vu32 *dma;
    s32 row;
    s32 column;
    s32 baseOffset;
    s32 tileOffset;
    s32 i;
    u32 busy;

    row = (index + ((s32)(((u32)index) >> 31))) >> 1;
    baseOffset = (row * 3) << 11;
    column = index - (row << 1);
    tileOffset = (column * 3) << 7;

    dma = (vu32 *)0x040000D4;
    dma[0] = (u32)(sUnk_86BF288 + (index << 5));
    dma[1] = 0x050003E0;
    dma[2] = 0x80000010;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0)
    {
    }

    {
        vu32 *dma3;
        i = 0;
        dma3 = (vu32 *)0x040000D4;
        do
        {
            dma3[0] = (u32)(((sUnk_86BF488 + baseOffset) + tileOffset) + (i << 10));
            dma3[1] = 0x06016000 + (i << 10);
            dma3[2] = 0x800000C0;
            dma3[2];
            ;
            while ((dma3[2] & 0x80000000) != (new_var = 0))
            {
            }
            i++;
        }
        while (i <= 5);
    }
}


void DrawSoundRoomSprites(void)
{
  const struct AnimationFrame *animation;
  const u16 *frameData;
  u16 *rawDst;
  OamData *dst;
  long slot;
  u32 frameCount;
  u32 nextUsed;
  u32 attr0;
  unsigned int attr1;
  u32 attr2;
  unsigned int index;
  int stage;
  u32 value;
  unsigned short remainder;
  register s32 yOffset asm("ip");
  s32 xOffset;
  short inverse;
  s32 sine;
  struct SoundRoomStack
  {
    s16 pa;
    s16 pb;
    s16 pc;
    u16 pd;
    s32 passage;
  } stack;
  u32 highMaskCopy = 0xFE00;
  s16 *pbOut;
  s16 *pcOut;
  long used;
  u16 *pdOut;
  slot = 0;
  used = gOamSlotsUsed;
  {
    int rawOffset;
    u16 *oamBase;
    rawOffset = used;
    rawOffset = rawOffset << 3;
    oamBase = (u16 *) gOamBuffer;
    rawDst = (u16 *) (rawOffset + ((u32) oamBase));
  }
  if ((((u8) gSoundRoomMenuState) != 4) && (((u8) gSoundRoomMenuState) != 2))
  {
    gStageEntryMainSpriteState[0]++;
    animation = (const struct AnimationFrame *) sUnk_8642898;
    if (animation[gStageEntryMainSpriteState[1]].time < gStageEntryMainSpriteState[0])
    {
      gStageEntryMainSpriteState[0] = 0;
      gStageEntryMainSpriteState[1]++;
      if (animation[gStageEntryMainSpriteState[1]].time == 0)
      {
        do
        {
        }
        while (0);
        gStageEntryMainSpriteState[1] = slot;
      }
    }
    frameData = animation[gStageEntryMainSpriteState[1]].oam;
    used += *(frameData++);
    if (used > 128)
    {
      goto finish;
    }
    if (slot < used)
    {
      register OamData *oamBase asm("r1");
      u32 oamOffset;
      u16 *state;
      u32 xMask;
      u32 highMask;
      register s32 attr2Mask asm("r9");
      oamBase = gOamBuffer;
      state = gStageEntryMainSpriteState;
      { register u32 xMaskTemp asm("r2"); xMaskTemp = 0x1FF; asm("" : "+r"(xMaskTemp)); xMask = xMaskTemp; }
      {
        u32 highMaskTemp;
        highMaskTemp = 0xFFFFFE00;
        highMask = highMaskTemp;
      }
      {
        s32 attr2MaskTemp;
        attr2MaskTemp = -13;
        attr2Mask = attr2MaskTemp;
      }
      oamOffset = (u32) slot;
      oamOffset = oamOffset << 3;
      dst = (OamData *) (oamOffset + (u32) oamBase);
      {
        long usedTemp;
        usedTemp = used;
        slot = usedTemp - slot;
      }
      do
      {
        if (1)
        {
          {
            register u32 loadTemp asm("r2");
            loadTemp = *(frameData++);
            attr0 = loadTemp;
          }
          *(rawDst++) = attr0;
        }
        ((u8 *) dst)[0] = (u8) (attr0 + ((u8 *) state)[10]);
        {
          register u32 loadTemp asm("r2");
          loadTemp = *(frameData++);
          attr1 = loadTemp;
        }
        *(rawDst++) = attr1;
        {
          s16 stateVal4;
          stateVal4 = ((s16 *) state)[4];
          {
            s32 newX;
            newX = attr1 + stateVal4;
            { register u32 xMaskCopy asm("r2"); xMaskCopy = xMask; asm("" : "+r"(xMaskCopy)); newX &= xMaskCopy; }
            { register u16 oldAttr1 asm("r2"); u32 merged;
            oldAttr1 = dst->all.attr1;
            merged = highMask;
            merged &= oldAttr1;
            merged |= newX;
            dst->all.attr1 = merged; }
          }
        }
        *rawDst = (attr2 = *(frameData++));
        {
          u8 old5 = ((u8 *) dst)[5];
          s32 merged5;
          asm volatile(
              "mov %0, %2\n"
              "and %0, %1"
              : "=&r"(merged5)
              : "r"(old5), "r"(attr2Mask));
          ((u8 *) dst)[5] = merged5;
        }
        rawDst += 2;
        dst++;
        slot--;
      }
      while (slot != 0);
      slot = used;
    }
  }
  {
    register s8 bonusUnlocked asm("r0");
    bonusUnlocked = gSoundRoomBonusTrackUnlocked;
    if (bonusUnlocked != 0)
    {
    { const struct AnimationFrame *bonusBase; bonusBase = sUnk_8642860; frameData = bonusBase[gSoundRoomPreviewAnimState[1]].oam; }
    used += *(frameData++);
    if (used > 128)
    {
      goto finish;
    }
    if (slot < used)
    {
      register OamData *oamBase asm("r1");
      u32 oamOffset;
      u32 xMask;
      register u32 highMask asm("r10");
      register int attr2Mask asm("r6");
      oamBase = gOamBuffer;
      {
        register u32 xMaskTemp asm("r0");
        xMaskTemp = 0x1FF;
        asm("" : "+r"(xMaskTemp));
        xMask = xMaskTemp;
      }
      {
        register u32 highMaskTemp asm("r2");
        highMaskTemp = 0xFFFFFE00;
        asm("" : "+r"(highMaskTemp));
        highMask = highMaskTemp;
      }
      attr2Mask = -13;
      oamOffset = (u32) slot;
      oamOffset = oamOffset << 3;
      dst = (OamData *) (oamOffset + (u32) oamBase);
      {
        long usedTemp;
        usedTemp = used;
        asm("" : "+r"(usedTemp));
        slot = usedTemp - slot;
      }
      do
      {
        attr0 = *(frameData++);
        *(rawDst++) = attr0;
        ((u8 *) dst)[0] = (u8) (attr0 + 8);
        attr1 = *(frameData++);
        *(rawDst++) = attr1;
        {
          register unsigned short newX asm("r1");
          register u32 xMaskCopy asm("r2");
          newX = attr1;
          newX += 56;
          xMaskCopy = xMask;
          asm("" : "+r"(newX), "+r"(xMaskCopy));
          newX &= xMaskCopy;
          { register u16 oldAttr1 asm("r2"); u32 highMaskCopy;
          oldAttr1 = dst->all.attr1;
          highMaskCopy = highMask;
          if (1)
          {
            highMaskCopy &= oldAttr1;
            asm volatile("orr %0, %1" : "+r"(highMaskCopy) : "r"(newX));
          }
          dst->all.attr1 = highMaskCopy; }
        }
        *rawDst = (attr2 = *(frameData++));
        {
          register u8 old5 asm("r1");
          s32 maskCopy5;
          old5 = ((u8 *) dst)[5];
          maskCopy5 = attr2Mask;
          asm("" : "+r"(maskCopy5) : "r"(old5));
          ((u8 *) dst)[5] = maskCopy5 & old5;
        }
        rawDst += 2;
        dst++;
        slot--;
      }
      while (slot != 0);
      slot = used;
    }
    }
  }
  stack.passage = (stage = 0);
  pbOut = &stack.pb;
  pcOut = &stack.pc;
  pdOut = &stack.pd;
  do
  {
    while (stack.passage <= 3)
    {
      stage = 0;
      { register s32 tNext asm("r2"); tNext = stack.passage + 1; asm("" : "+r"(tNext)); nextUsed = tNext; }
      while (stage <= 3)
      {
        register s32 passageCmp asm("r0");
        register u32 selectedPassage asm("r4");
        u32 selectedStageValue;
        register u32 menuPtr asm("r1");
        u32 menuValue;
        passageCmp = stack.passage;
        selectedPassage = (u32) &gSoundRoomSelectedPassage;
        selectedPassage = *(volatile u8 *) selectedPassage;
        asm("" : "+r"(selectedPassage));
        if (passageCmp != selectedPassage)
          goto grid_unselected;
        selectedStageValue = (u32) &gSoundRoomSelectedStage;
        selectedStageValue = *(volatile u8 *) selectedStageValue;
        if (stage != selectedStageValue)
          goto grid_unselected;
        menuPtr = (u32) &gSoundRoomMenuState;
        menuValue = *(volatile u8 *) menuPtr;
        if (menuValue <= 2)
          goto grid_unselected;
          {
            u32 stageOffset;
            s32 passageValue;
            register u32 passageOffset asm("r0");
            stageOffset = stage << 2;
            passageValue = stack.passage;
            passageOffset = passageValue << 4;
            asm("" : "+r"(stageOffset), "+r"(passageValue), "+r"(passageOffset));
            index = stageOffset + passageOffset;
          }
          { register const u8 *frameTable asm("r4"); register u32 frameAddr asm("r0"); frameTable=(const u8 *)sUnk_863C7D4; frameAddr=index+(u32)frameTable; frameData=*((const u16 * const *)frameAddr); }
          used += *(frameData++);
          frameCount = stage + 1;
          if (slot < used)
          {
            { register const u8 *yTable asm("r1"); register u32 yAddr asm("r0"); yTable=(const u8 *)sUnk_863C6D4; yAddr=index+(u32)yTable; yOffset=*((const s32 *)yAddr); }
            {
              register OamData *oamBase asm("r1");
              u32 oamOffset;
              oamBase = gOamBuffer;
              xOffset = *((const s32 *) (((const u8 *) sUnk_863C694) + index));
              oamOffset = ((u32) slot) << 3;
              dst = (OamData *) (oamOffset + (u32) oamBase);
            }
            do
            {
              attr0 = *(frameData++);
              *(rawDst++) = attr0;
              ((u8 *) dst)[0] = (u8) (attr0 + yOffset - 8);
              ((u8 *) dst)[1] |= 3;
              attr1 = *(frameData++);
              *(rawDst++) = attr1;
              {
                s32 newX;
                newX = attr1 + xOffset - 8;
                { register u32 xMask asm("r2"); xMask=0x01FF; newX &= xMask; }
                { u16 oldAttr1;
                u32 highMaskCopy;
                oldAttr1 = dst->all.attr1;
                highMaskCopy = 0xFFFFFE00;
                highMaskCopy &= oldAttr1;
                highMaskCopy |= newX;
                dst->all.attr1 = highMaskCopy; }
              }
              {
                u8 old3;
                s32 mask3;
                old3 = ((u8 *) dst)[3];
                mask3 = -15;
                mask3 &= old3;
                ((u8 *) dst)[3] = (u8) mask3;
              }
              *rawDst = (attr2 = *(frameData++));
              {
                u8 old5;
                s32 mask5;
                old5 = ((u8 *) dst)[5];
                mask5 = -13;
                mask5 &= old5;
                ((u8 *) dst)[5] = (u8) mask5;
              }
              rawDst += 2;
              dst++;
              slot++;
            }
            while (slot < used);
          }
        goto grid_done;

      grid_unselected:
        {
          u32 collectionBits;
          {
            register const u8 *collection asm("r2");
            s32 stageOffset;
            s32 nextValue;
            register s32 collectionOffset asm("r0");
            collection = (const u8 *) gCurrentCollection;
            stageOffset = stage << 2;
            nextValue = nextUsed;
            collectionOffset = nextValue << 1;
            collectionOffset += nextUsed;
            collectionOffset <<= 3;
            collectionOffset = stageOffset + collectionOffset;
            collectionBits = *(const u8 *)((u32)collectionOffset + (u32)collection);
            collectionBits <<= 27;
            frameCount = stage + 1;
            if ((s32) collectionBits < 0)
            {
              register s32 passageValue asm("r2");
              register s32 passageOffset asm("r0");
              register const u8 *frameTable asm("r3");
              passageValue = stack.passage;
              passageOffset = passageValue << 4;
              stageOffset += passageOffset;
              frameTable = (const u8 *) sUnk_863C7D4;
              { register u32 addr asm("r0"); addr = (u32) stageOffset + (u32) frameTable; frameData = *((const u16 * const *) addr); }
              used += *(frameData++);
              if (slot < used)
              {
                register const u8 *yTable asm("r2");
                register OamData *oamBase asm("r2");
                const u8 *xTable;
                u32 oamOffset;
                yTable = (const u8 *) sUnk_863C6D4;
                { register u32 addr asm("r0"); addr = (u32) stageOffset + (u32) yTable; yOffset = *((const s32 *) addr); }
                oamOffset = (u32) slot;
                oamOffset <<= 3;
                oamBase = gOamBuffer;
                dst = (OamData *) (oamOffset + (u32) oamBase);
                xTable = (const u8 *) sUnk_863C694;
                xOffset = *((const s32 *) ((u32)stageOffset + (u32)xTable));
              slot = used - slot;
              do
              {
                attr0 = *(frameData++);
                *(rawDst++) = attr0;
                ((u8 *) dst)[0] = (u8) (yOffset + attr0);
                attr1 = *(frameData++);
                *(rawDst++) = attr1;
                {
                  s32 newX;
                  u32 temp;
                  u32 merged;
                  newX = attr1 + xOffset;
                  temp = 0x01FF;
                  newX &= temp;
                  temp = dst->all.attr1;
                  merged = 0xFFFFFE00;
                  merged &= temp;
                  merged |= newX;
                  dst->all.attr1 = merged;
                }
                attr2 = (*rawDst = *(frameData++));
                {
                  u8 old5;
                  s32 mask5;
                  old5 = ((u8 *) dst)[5];
                  mask5 = -13;
                  mask5 &= old5;
                  ((u8 *) dst)[5] = (u8) mask5;
                }
                rawDst += 2;
                dst++;
                slot--;
              }
              while (slot != 0);
              slot = used;
            }
          }
          }

        }
      grid_done:
        stage = frameCount;
      }

      stage = nextUsed;
      stack.passage = stage;
    }

    {
      u8 menuStateValue;
      menuStateValue = (u8)gSoundRoomMenuState;
      asm("" : : "r"(stage));
      if (menuStateValue > 2)
      {
      {
        const u8 *tableBase;
        u32 stageIndex;
        register const s8 *passagePtr asm("r3");
        register u32 passageIndex asm("r1");
        tableBase = (const u8 *) sUnk_863C814;
        stageIndex = (u8) gSoundRoomSelectedStage;
        stageIndex <<= 2;
        passagePtr = &gSoundRoomSelectedPassage;
        passageIndex = (u8) *passagePtr;
        passageIndex <<= 4;
        stageIndex += passageIndex;
        stageIndex += (u32) tableBase;
        frameData = *((const u16 * const *) stageIndex);
      }
    }
    else
    {
      register s8 musicEnabled asm("r0");
      musicEnabled = gSoundRoomMusicEnabled;
      if (musicEnabled != 0)
      {
        {
          u8 musicValue;
          u8 bit;
          musicValue = (u8)gMPlayMemAccArea[0];
          bit = musicValue & 1;
          frameData = (const u16 *) sUnk_8642638;
          if ((bit == 1) && (musicValue <= 99))
          {
            frameData = (const u16 *) sUnk_864262A;
          }
        }
      }
      else
      {
        frameData = (const u16 *) sUnk_864261C;
        if (1)
        {
          do
          {
          }
          while (0);
        }
      }
    }
    }
    {
      register s8 menuValue asm("r0");
      menuValue = gSoundRoomMenuState;
      if (menuValue != 0)
      {
      do
      {
        used += *(frameData++);
        if (used > 128)
        {
        }
        if (slot < used)
        {
          register OamData *oamBase asm("r1");
          u32 oamOffset;
          s32 loopAttr2Mask;
          register u32 loopXMask asm("r9");
          register u32 loopHighMask asm("r10");
          oamBase = gOamBuffer;
          { register u32 t asm("r2"); t=0x1FF; asm("" : "+r"(t)); loopXMask=t; }
          { register u32 t asm("r3"); t=0xFFFFFE00; asm("" : "+r"(t)); loopHighMask=t; }
          loopAttr2Mask = -13;
          oamOffset = (u32) slot;
          oamOffset = oamOffset << 3;
          dst = (OamData *) (oamOffset + (u32) oamBase);
          { long t; t=used; asm("" : "+r"(t)); slot = t - slot; }
          do
          {
            attr0 = *(frameData++);
            if (1)
            {
              *(rawDst++) = attr0;
            }
            ((u8 *) dst)[0] = (u8) (attr0 + 88);
            if (0)
            {
              slot--;
              ((u8 *) dst)[1] |= 3;
            }
            attr1 = *(frameData++);
            *(rawDst++) = attr1;
            {
              int newX;
              newX = attr1 + 160;
              { register u32 xm asm("r2"); xm=loopXMask; asm("" : "+r"(xm)); newX &= xm; }
              { register u16 oldAttr1 asm("r2"); u32 merged;
                oldAttr1=dst->all.attr1; merged=loopHighMask;
                merged &= oldAttr1;
                merged |= newX;
                dst->all.attr1=merged; }
            }
            if (0)
            {
              goto finish;
            }
            attr2 = (*rawDst = *(frameData++));
            {
              register u8 old5 asm("r1");
              s32 maskCopy5;
              old5 = ((u8 *) dst)[5];
              maskCopy5 = loopAttr2Mask;
              ((u8 *) dst)[5] = maskCopy5 & old5;
            }
            rawDst += 2;
            dst++;
            slot--;
          }
          while (0 != slot);
          slot = used;
        }
      }
      while (0);
      }
    }
    if (((u8) gSoundRoomMenuState) == 2)
    {
      remainder = gStageEntrySequenceTimer % 7;
      if (remainder == 0)
      {
        {
          s32 randomValue;
          randomValue = MinigameRandom();
          randomValue = randomValue % 10;
          if (randomValue <= 4)
          {
            gSoundRoomPreviewRequest = 1;
          }
          else
          {
            gSoundRoomPreviewRequest = 0;
          }
        }
      }
      {
        u8 previewRequest;
        previewRequest = gSoundRoomPreviewRequest;
        if (previewRequest == 0)
        {
          frameData = (const u16 *) sUnk_86427A0;
        }
        if (previewRequest == 1)
        {
          frameData = (const u16 *) sUnk_86427BA;
        }
      }
      used += *(frameData++);
      if (slot < used)
      {
        OamData *oamBase;
        u32 oamOffset;
        s32 loopAttr2Mask;
        u32 loopXMask;
        register u32 loopHighMask asm("r10");
        oamBase = gOamBuffer;
        loopXMask = 0x1FF;
        { register u32 highMaskLoad asm("r2"); highMaskLoad = 0xFFFFFE00; asm("" : "+r"(highMaskLoad)); loopHighMask = highMaskLoad; }
        loopAttr2Mask = -13;
        oamOffset = (u32) slot;
        oamOffset <<= 3;
        dst = (OamData *) (oamOffset + (u32) oamBase);
        { long usedTemp; usedTemp=used; asm("" : "+r"(usedTemp)); slot = usedTemp - slot; }
        do
        {
          attr0 = *(frameData++);
          *(rawDst++) = attr0;
          ((u8 *) dst)[0] = (u8) (attr0 + 16);
          attr1 = *(frameData++);
          *(rawDst++) = attr1;
                    {
            s32 newX;
            register u32 temp asm("r2");
            u32 merged;
            newX = attr1 + 160;
            temp = loopXMask;
            asm("" : "+r"(newX), "+r"(temp));
            newX &= temp;
            temp = dst->all.attr1;
            merged = loopHighMask;
            merged &= temp;
            merged |= newX;
            dst->all.attr1 = merged;
          }
          attr2 = (*rawDst = *(frameData++));
          {
            register u8 old5 asm("r1");
            s32 maskCopy5;
            old5 = ((u8 *) dst)[5];
            maskCopy5 = loopAttr2Mask;
            ((u8 *) dst)[5] = maskCopy5 & old5;
          }
          rawDst += 2;
          dst++;
          slot--;
        }
        while (slot != 0);
      }
      goto updateAffine;
    }
    if (((u8) gSoundRoomMenuState) == 4)
    {
      value = gStageEntrySequenceTimer;
      remainder = value % 1200;
      if (remainder <= 299)
      {
        remainder = value % 7;
        if (remainder == 0)
        {
          {
            s32 randomValue;
            randomValue = MinigameRandom() % 10;
            if (randomValue <= 4)
            {
              gSoundRoomPreviewRequest = 1;
            }
            else
            {
              gSoundRoomPreviewRequest = 0;
              if (1)
              {
              }
            }
          }
        }
        {
          u8 previewRequest;
          previewRequest = gSoundRoomPreviewRequest;
          if (previewRequest == 0)
          {
            frameData = (const u16 *) sUnk_86427A0;
          }
          if (previewRequest == 1)
          {
            frameData = (const u16 *) sUnk_86427BA;
          }
        }
        used += *(frameData++);
        if (slot < used)
        {
          OamData *oamBase;
          u32 oamOffset;
          s32 loopAttr2Mask;
          u32 loopXMask;
          u32 loopHighMask;
          oamBase = gOamBuffer;
          loopXMask = 0x1FF;
          loopHighMask = 0xFFFFFE00;
          loopAttr2Mask = -13;
          oamOffset = (u32) slot;
          oamOffset <<= 3;
          dst = (OamData *) (oamOffset + (u32) oamBase);
          slot = used - slot;
          do
          {
            attr0 = *(frameData++);
            *(rawDst++) = attr0;
            ((u8 *) dst)[0] = (u8) (attr0 + 16);
            attr1 = *(frameData++);
            *(rawDst++) = attr1;
                        {
              s32 newX;
              register u32 temp asm("r2");
              u32 merged;
              newX = attr1 + 160;
              temp = loopXMask;
              asm("" : "+r"(newX), "+r"(temp));
              newX &= temp;
              temp = dst->all.attr1;
              merged = loopHighMask;
              merged &= temp;
              merged |= newX;
              dst->all.attr1 = merged;
            }
            attr2 = (*rawDst = *(frameData++));
            {
              register u8 old5 asm("r1");
              s32 maskCopy5;
              old5 = ((u8 *) dst)[5];
              maskCopy5 = loopAttr2Mask;
              ((u8 *) dst)[5] = maskCopy5 & old5;
            }
            rawDst += 2;
            dst++;
            slot--;
          }
          while (slot != 0);
        }
        goto updateAffine;
      }
      {
        s32 remainder600;
        remainder600 = gStageEntrySequenceTimer % 600;
        if ((remainder600 == 0) || (remainder600 == 300))
        {
          gSoundRoomTrackAnimState[0] = 1;
          gSoundRoomTrackAnimState[1] = 0;
        }
        if (remainder600 <= 299)
        {
          animation = (const struct AnimationFrame *) sUnk_863C854[0];
        }
        else
        {
          animation = (const struct AnimationFrame *) sUnk_863C854[1];
        }
      }
      gSoundRoomTrackAnimState[0]++;
      if (animation[gSoundRoomTrackAnimState[1]].time < gSoundRoomTrackAnimState[0])
      {
        {
          register u16 zero asm("r0");
          zero = 0;
          gSoundRoomTrackAnimState[0] = zero;
        }
        gSoundRoomTrackAnimState[1]++;
        if ((&animation[gSoundRoomTrackAnimState[1]])->time == 0)
        {
          gSoundRoomTrackAnimState[1] = 0;
        }
      }
      frameData = animation[gSoundRoomTrackAnimState[1]].oam;
      used += *(frameData++);
      if (slot < used)
      {
        register OamData *oamBase asm("r1");
        u32 oamOffset;
        s32 loopAttr2Mask;
        u32 loopXMask;
        u32 loopHighMask;
        oamBase = gOamBuffer;
        loopXMask = 0x1FF;
        loopHighMask = 0xFFFFFE00;
        loopAttr2Mask = -13;
        oamOffset = (u32) slot;
        oamOffset <<= 3;
        dst = (OamData *) (oamOffset + (u32) oamBase);
        slot = used - slot;
        do
        {
          attr0 = *(frameData++);
          *(rawDst++) = attr0;
          ((u8 *) dst)[0] = (u8) (attr0 + 16);
          attr1 = *(frameData++);
          *(rawDst++) = attr1;
                    {
            s32 newX;
            register u32 temp asm("r2");
            u32 merged;
            newX = attr1 + 160;
            temp = loopXMask;
            asm("" : "+r"(newX), "+r"(temp));
            newX &= temp;
            temp = dst->all.attr1;
            merged = loopHighMask;
            merged &= temp;
            merged |= newX;
            dst->all.attr1 = merged;
          }
          *rawDst = (attr2 = *(frameData++));
          {
            register u8 old5 asm("r1");
            s32 maskCopy5;
            old5 = ((u8 *) dst)[5];
            maskCopy5 = loopAttr2Mask;
            ((u8 *) dst)[5] = maskCopy5 & old5;
          }
          rawDst += 2;
          dst++;
          slot--;
        }
        while (slot != 0);
      }
      goto updateAffine;
    }
    else
    {
      animation = (const struct AnimationFrame *) sUnk_8642820;
      gSoundRoomTrackAnimState[0]++;
      if (animation[gSoundRoomTrackAnimState[1]].time < gSoundRoomTrackAnimState[0])
      {
        gSoundRoomTrackAnimState[0] = 1;
        gSoundRoomTrackAnimState[1]++;
        if (animation[gSoundRoomTrackAnimState[1]].time == 0)
        {
          gSoundRoomTrackAnimState[1] = 0;
        }
      }
      frameData = animation[gSoundRoomTrackAnimState[1]].oam;
      used += *(frameData++);
      if (slot < used)
      {
        register OamData *oamBase asm("r1");
        u32 oamOffset;
        s32 loopAttr2Mask;
        u32 loopXMask;
        u32 loopHighMask;
        oamBase = gOamBuffer;
        loopXMask = 0x1FF;
        {
          u32 highMaskLoad;
          highMaskLoad = 0xFFFFFE00;
          loopHighMask = highMaskLoad;
        }
        loopAttr2Mask = -13;
        oamOffset = (u32) slot;
        oamOffset <<= 3;
        if (1)
        {
        }
        dst = (OamData *) (oamOffset + (u32) oamBase);
        { register long t asm("r1"); t=used; asm("" : "+r"(t)); slot = t - slot; }
        do
        {
          attr0 = *(frameData++);
          *(rawDst++) = attr0;
          ((u8 *) dst)[0] = (u8) (attr0 + 16);
          attr1 = *(frameData++);
          *(rawDst++) = attr1;
                    {
            s32 newX;
            register u32 temp asm("r2");
            u32 merged;
            newX = attr1 + 160;
            temp = loopXMask;
            asm("" : "+r"(newX), "+r"(temp));
            newX &= temp;
            temp = dst->all.attr1;
            merged = loopHighMask;
            merged &= temp;
            merged |= newX;
            dst->all.attr1 = merged;
          }
          *rawDst = (attr2 = *(frameData++));
          {
            register u8 old5 asm("r1");
            s32 maskCopy5;
            old5 = ((u8 *) dst)[5];
            maskCopy5 = loopAttr2Mask;
            ((u8 *) dst)[5] = maskCopy5 & old5;
          }
          rawDst += 2;
          dst++;
          slot--;
        }
        while (0 != slot);
      }
    }
    highMaskCopy = 0xFE00;
  }
  while (0);
  updateAffine:
  { register s16 *wavePtr asm("r3"); u32 phase; register const u8 *sinBase asm("r4"); wavePtr=&gSoundRoomTileWaveOffset; phase=(u16)*wavePtr; phase += 64; phase <<= 1; sinBase=(const u8 *)sSinCosTable; sine=*(const s16 *)(phase + (u32) sinBase); }
  inverse = FixedInverse(384);
  stack.pa = FixedMul(sine, (s16) inverse);
  { register s16 *wavePtr asm("r2"); u32 phase; register const u8 *sinBase asm("r3"); wavePtr=&gSoundRoomTileWaveOffset; phase=(u16)*wavePtr; phase <<= 1; sinBase=(const u8 *)sSinCosTable; phase += (u32)sinBase; { register u32 hold asm("r3"); sine=*(const s16 *)phase; asm volatile("" : : "r"(hold)); } }
  inverse = FixedInverse(384);
  { s32 v; register s16 *p asm("r2"); v = FixedMul(sine, (s16) inverse); p = pbOut; asm volatile("" : "+r"(p)); *p = v; }
  { register s16 *wavePtr asm("r3"); u32 phase; register const u8 *sinBase asm("r4"); wavePtr=&gSoundRoomTileWaveOffset; phase=(u16)*wavePtr; phase <<= 1; sinBase=(const u8 *)sSinCosTable; sine=*(const u16 *)(phase + (u32) sinBase); }
  { register s32 neg asm("r4"); neg=sine; neg=-neg; neg=(s16)neg; sine=neg; }
  inverse = FixedInverse(384);
  { s32 v; s16 *p; v = FixedMul(sine, (s16) inverse); p = pcOut; asm volatile("" : "+r"(p)); *p = v; }
  { register s16 *wavePtr asm("r2"); u32 phase; register const u8 *sinBase asm("r3"); wavePtr=&gSoundRoomTileWaveOffset; phase=(u16)*wavePtr; phase += 64; phase <<= 1; sinBase=(const u8 *)sSinCosTable; phase += (u32)sinBase; { register u32 hold asm("r3"); sine=*(const s16 *)phase; asm volatile("" : : "r"(hold)); } }
  inverse = FixedInverse(384);
  {
    u16 pdValue;
    register u32 h3 asm("r3");
    register u16 *p asm("r2");
    pdValue = FixedMul(sine, (s16) inverse);
    p = pdOut;
    asm volatile("" : "+r"(p));
    *p = pdValue;
    asm volatile("" : : "r"(h3));
    gOamBuffer[0].all.affineParam = stack.pa;
    gOamBuffer[1].all.affineParam = *pbOut;
    gOamBuffer[2].all.affineParam = *pcOut;
    gOamBuffer[3].all.affineParam = pdValue;
  }
  gOamSlotsUsed = used;
  finish:
  return;

}


void LoadRandomSoundRoomPreview(void)
{
    vu32 *dma;
    register s32 index asm("r3");
    u32 busy;

    index = MinigameRandom() % 5;
    dma = (vu32 *)0x040000D4;
    dma[0] = (u32)(sUnk_86CB7C8 + (index << 6));
    dma[1] = 0x050003A0;
    dma[2] = 0x80000020;
    dma[2];
    busy = 0x80000000;
    while ((dma[2] & busy) != 0)
    {
    }

    dma = (vu32 *)0x040000D4;
    dma[0] = (u32)(sUnk_86CB908 + ((index * 3) << 11));
    dma[1] = 0x06010000;
    dma[2] = 0x80000C00;
    dma[2];
}

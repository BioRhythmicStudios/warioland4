#include "game_screen_helpers.h"

s32 BossPause(void)
{
    u32 ret;

    ret = 0;
    switch ((u8)gUnk_300001A) {
        case 1:
            m4aMPlayVolumeControl(GP(gMPlayTable[0].info), 0xFFFF, gCurrentRoomHeader.musicVolume >> 2);
            m4aMPlayVolumeControl(GP(gMPlayTable[1].info), 0xFFFF, gCurrentRoomHeader.musicVolume >> 2);
            m4aMPlayVolumeControl(GP(gMPlayTable[2].info), 0xFFFF, 0);
            m4aMPlayVolumeControl(GP(gMPlayTable[3].info), 0xFFFF, 0);
            m4aMPlayVolumeControl(GP(gMPlayTable[4].info), 0xFFFF, 0);
            m4aMPlayVolumeControl(GP(gMPlayTable[5].info), 0xFFFF, 0);
            m4aMPlayVolumeControl(GP(gMPlayTable[6].info), 0xFFFF, 0);
            REG_WIN1H = 0x7878;
            REG_WIN1V = 0x5050;
            gUnk_300001A++;
            break;
        case 2:
            REG_IME = 0;
            REG_DISPSTAT &= 0xFFEF;
            REG_IE &= 0xFFFD;
            REG_IME = 1;
            gUnk_300001A++;
            break;
        case 3:
            REG_BG3CNT = (REG_BG3CNT & 0xE0FF) | 0x700;
            LoadBossPauseBackgroundTiles(0);
            DmaSet(3, (void*)0x0201F040, (void*)0x06003800, 0x80000400);
            *(vu8*)REG_ADDR_WINOUT |= 8;
            gUnk_300001A++;
            gUnk_300188E = 0;
            break;
        case 4:
            gUnk_300188E++;
            if (gUnk_300188E > 0x1D) {
                gUnk_300001A++;
                gUnk_300188E = 0;
            }
            break;
        case 5:
            gUnk_300188E++;
            if (gButtonsPressed & 8) {
                gUnk_300001A++;
                gUnk_300188E = 0;
            } else if ((gUnk_300188E & 0x1F) == 0) {
                if (gUnk_300188E & 0x20) {
                    *(vu8*)REG_ADDR_WINOUT &= 0x37;
                } else {
                    *(vu8*)REG_ADDR_WINOUT |= 8;
                }
            }
            break;
        case 6:
            m4aSongNumStart(0x123);
            *(vu8*)REG_ADDR_WINOUT &= 0x37;
            m4aMPlayVolumeControl(GP(gMPlayTable[0].info), 0xFFFF, gCurrentRoomHeader.musicVolume);
            m4aMPlayVolumeControl(GP(gMPlayTable[1].info), 0xFFFF, gCurrentRoomHeader.musicVolume);
            m4aMPlayVolumeControl(GP(gMPlayTable[2].info), 0xFFFF, 0x100);
            m4aMPlayVolumeControl(GP(gMPlayTable[3].info), 0xFFFF, 0x100);
            m4aMPlayVolumeControl(GP(gMPlayTable[4].info), 0xFFFF, 0x100);
            m4aMPlayVolumeControl(GP(gMPlayTable[5].info), 0xFFFF, 0x100);
            m4aMPlayVolumeControl(GP(gMPlayTable[6].info), 0xFFFF, 0x100);
            gUnk_300001A++;
            break;
        case 7:
            REG_BG3CNT = (REG_BG3CNT & 0xE0FF) | 0x600;
            LoadBossPauseBackgroundTiles(1);
            DmaSet(3, (void*)0x0201F840, (void*)0x06003800, 0x80000400);
            gUnk_300001A++;
            break;
        case 8:
            if (gHBlankMode != 0) {
                REG_IE |= 2;
                REG_DISPSTAT |= 0x10;
            }
            gUnk_300001A++;
            break;
        case 9:
            REG_WIN1H = 0xF0;
            REG_WIN1V = 0xA0;
            gUnk_300001A++;
            break;
        case 10:
            gUnk_300188E = 0;
            gUnk_300001A = 0;
            ret = 1;
            break;
    }

    return ret;
}

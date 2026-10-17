#ifndef GUARD_GBA_SYSCALL_H
#define GUARD_GBA_SYSCALL_H

#include "gba/types.h"


#ifdef PORTABLE
// Portable builds hand BIOS calls to the platform layer.
void PlatformSyscall(int num);
#define _SYSCALL(num) PlatformSyscall(num)
#else
#define _SYSCALL(num) asm("svc " #num)
#endif
#define SYSCALL(num) _SYSCALL(num)

#define SYSCALL_SoftReset 0
#define SYSCALL_RegisterRamReset 1
#define SYSCALL_Halt 2
#define SYSCALL_Stop 3
#define SYSCALL_IntrWait 4
#define SYSCALL_VBlankIntrWait 5
#define SYSCALL_Div 6
#define SYSCALL_DivArm 7
#define SYSCALL_Sqrt 8
#define SYSCALL_ArcTan 9
#define SYSCALL_ArcTan2 10
#define SYSCALL_CPUSet 11
#define SYSCALL_CPUSetFast 12
#define SYSCALL_BiosChecksum 13
#define SYSCALL_BgAffineSet 14
#define SYSCALL_ObjAffineSet 15
#define SYSCALL_BitUnPack 16
#define SYSCALL_LZ77UnCompWRAM 17
#define SYSCALL_LZ77UnCompVRAM 18
#define SYSCALL_HuffUnComp 19
#define SYSCALL_LRUnCompWRAM 20
#define SYSCALL_LRUnCompVRAM 21
#define SYSCALL_Diff8bitUnFilterWRAM 22
#define SYSCALL_Diff8bitUnFilterVRAM 23
#define SYSCALL_Diff16bitUnFilter 24
#define SYSCALL_SoundBiasChange 25
#define SYSCALL_SoundDriverInit 26
#define SYSCALL_SoundDriverMode 27
#define SYSCALL_SoundDriverMain 28
#define SYSCALL_SoundDriverVSync 29
#define SYSCALL_SoundChannelClear 30
#define SYSCALL_MIDIKey2Freq 31
#define SYSCALL_MusicPlayerOpen 32
#define SYSCALL_MusicPlayerStart 33
#define SYSCALL_MusicPlayerStop 34
#define SYSCALL_MusicPlayerContinue 35
#define SYSCALL_MusicPlayerFadeOut 36
#define SYSCALL_MultiBoot 37
#define SYSCALL_HardReset 38
#define SYSCALL_CustomHalt 39
#define SYSCALL_SoundDriverVSyncOff 40
#define SYSCALL_SoundDriverVSyncOn 41
#define SYSCALL_GetJumpList 42

#define RESET_EWRAM      0x01
#define RESET_IWRAM      0x02
#define RESET_PALETTE    0x04
#define RESET_VRAM       0x08
#define RESET_OAM        0x10
#define RESET_SIO_REGS   0x20
#define RESET_SOUND_REGS 0x40
#define RESET_REGS       0x80
#define RESET_ALL        0xFF

void SoftReset(u32 resetFlags);

void RegisterRamReset(u32 resetFlags);

void VBlankIntrWait(void);

u16 Sqrt(u32 num);

u16 ArcTan2(s16 x, s16 y);

#define CPU_SET_SRC_FIXED 0x01000000
#define CPU_SET_16BIT     0x00000000
#define CPU_SET_32BIT     0x04000000

void CPUSet(const void *src, void *dest, u32 control);

#define CPU_FAST_SET_SRC_FIXED 0x01000000

void CpuFastSet(const void *src, void *dest, u32 control);

void BgAffineSet(struct BgAffineSrcData *src, struct BgAffineDstData *dest, s32 count);

void ObjAffineSet(struct ObjAffineSrcData *src, void *dest, s32 count, s32 offset);

void LZ77UnCompWram(const u32 *src, void *dest);

void LZ77UnCompVram(const u32 *src, void *dest);

void RLUnCompWram(const void *src, void *dest);

void RLUnCompVram(const void *src, void *dest);

int MultiBoot(struct MultiBootParam *mp);

s32 Div(s32 num, s32 denom);

s32 Mod(s32 num, s32 denom);

void SoundBiasReset(void);

void SoundBiasSet(void);

#endif // GUARD_GBA_SYSCALL_H

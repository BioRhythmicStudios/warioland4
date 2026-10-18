// Native versions of the m4a asm routines (asm/m4a_asm.s) that game code calls directly,
// built with the game's flags. They replace the weak stubs in port/generated/gba_stubs.c.
// The sound engine itself (SoundMain, MPlayMain and the ply_ routines it calls) is not
// ported yet: SoundMain is a silent placeholder, so music players never advance.
#include "gba/defines.h"
#include "gba/m4a.h"

// The template in the ROM holds GBA addresses, which is what the jump table in GBA RAM holds.
extern const u32 sMPlayJumpTableTemplate[36];

void MPlayJumpTableCopy(GBA_TYPEOF(MPlayFunc *, &gMPlayJumpTable[0]) mplayJumpTable)
{
    u32 *dest = (u32 *)mplayJumpTable;
    int i;

    for (i = 0; i < 36; i++)
        dest[i] = sMPlayJumpTableTemplate[i];
}

// Stops a track's channels: switches off the CGB ones and unlinks them all.
void TrackStop(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    struct SoundChannel *chan;

    (void)mplayInfo;
    if (!(track->flags & 0x80))
        return;
    for (chan = GP(track->chan); chan != NULL; chan = (struct SoundChannel *)chan->np) {
        if (chan->status != 0) {
            if (chan->type & 7)
                GF(GP(SOUND_INFO_PTR)->CgbOscOff)(chan->type & 7);
            chan->status = 0;
        }
        GP_SET(chan->track, NULL);
    }
    GP_SET(track->chan, NULL);
}

// Restarts the sound FIFO DMA transfers every pcmDmaPeriod VBlanks.
void m4aSoundVSync(void)
{
    struct SoundInfo *soundInfo = GP(SOUND_INFO_PTR);

    if (soundInfo->ident - ID_NUMBER > 1)
        return;
    if ((s32)(soundInfo->pcmDmaCounter - 1) > 0) {
        soundInfo->pcmDmaCounter--;
        return;
    }
    soundInfo->pcmDmaCounter = soundInfo->pcmDmaPeriod;
    if (REG_DMA1CNT & (DMA_REPEAT << 16))
        REG_DMA1CNT = ((DMA_ENABLE | DMA_START_NOW | DMA_32BIT | DMA_SRC_INC | DMA_DEST_FIXED) << 16) | 4;
    if (REG_DMA2CNT & (DMA_REPEAT << 16))
        REG_DMA2CNT = ((DMA_ENABLE | DMA_START_NOW | DMA_32BIT | DMA_SRC_INC | DMA_DEST_FIXED) << 16) | 4;
    REG_DMA1CNT_H = DMA_32BIT;
    REG_DMA2CNT_H = DMA_32BIT;
    REG_DMA1CNT_H = DMA_ENABLE | DMA_START_SPECIAL | DMA_32BIT | DMA_REPEAT;
    REG_DMA2CNT_H = DMA_ENABLE | DMA_START_SPECIAL | DMA_32BIT | DMA_REPEAT;
}

// Jump table entry 35 (Clear64byte): zeroes 64 bytes.
void SoundMainBTM(void *x)
{
    u32 *d = x;
    int i;

    for (i = 0; i < 16; i++)
        d[i] = 0;
}

// Jump table entry 34 (ClearChain): unlinks a channel from its track's channel list.
void RealClearChain(void *x)
{
    struct SoundChannel *chan = x;
    struct MusicPlayerTrack *track = GP(chan->track);
    struct SoundChannel *next, *prev;

    if (track == NULL)
        return;
    next = (struct SoundChannel *)chan->np;
    prev = (struct SoundChannel *)chan->pp;
    if (prev != NULL)
        prev->np = chan->np;
    else
        GP_SET(track->chan, next);
    if (next != NULL)
        next->pp = chan->pp;
    GP_SET(chan->track, NULL);
}

// Placeholder: mixes nothing and runs no music players.
void SoundMain(void)
{
}

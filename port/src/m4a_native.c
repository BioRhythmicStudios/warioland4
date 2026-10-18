// Native versions of the m4a asm routines (asm/m4a_asm.s), built with the game's flags. They
// replace the weak stubs in port/generated/gba_stubs.c. Each follows its asm step by step, so
// the channels, tracks and PCM buffer in GBA RAM come out as they do on the GBA.
#include "gba/defines.h"
#include "gba/m4a.h"

#include <string.h>

#include "port/gba_io.h"

// The template in the ROM holds GBA addresses, which is what the jump table in GBA RAM holds.
extern const u32 sMPlayJumpTableTemplate[36];
extern const u8 sClockTable[];

u32 MidiKeyToFreq(struct WaveData *wav, u8 key, u8 fineAdjust);

typedef void (*MPlayCommand)(struct MusicPlayerInfo *, struct MusicPlayerTrack *);

// A channel is in use while any of these status bits is set.
#define CHANNEL_ON 0xC7

#define NEXT_CHANNEL(chan) ((struct SoundChannel *)(uintptr_t)(chan)->np)

// The engine reads song data through a guard (sub_0814F890): an address below EWRAM, in the
// BIOS, reads as 0. The PC cannot read there at all, so the guard comes first.
static int IsGuarded(const void *p)
{
    return (uintptr_t)p < 0x02000000;
}

static u32 GuardedByte(const u8 *p)
{
    return IsGuarded(p) ? 0 : *p;
}

// The next command byte (sub_0814F8AC).
static u32 ReadCommand(struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);

    GP_SET(track->cmdPtr, p + 1);
    return GuardedByte(p);
}

void MPlayJumpTableCopy(GBA_TYPEOF(MPlayFunc *, &gMPlayJumpTable[0]) mplayJumpTable)
{
    u32 *dest = (u32 *)mplayJumpTable;
    int i;

    for (i = 0; i < 36; i++)
        dest[i] = sMPlayJumpTableTemplate[i];
}

// --- SoundMain: runs the music players and mixes the Direct Sound channels ------------------

// VCOUNT counted from the start of VBlank, as SoundMain measures time. The host's view: the
// game's own reads would move the display on.
static u32 LinesSinceVBlank(void)
{
    u32 line = PortIoHost()[6];

    return line < 160 ? line + 228 : line;
}

// Adds a sample to an output byte. The asm adds bits 8-15 of the product into the top byte of
// a rotating word, so the sum wraps at 8 bits.
static void MixSample(s8 *out, s32 sample, u32 volume)
{
    *out = (s8)(*out + (((s32)volume * sample) >> 8));
}

void SoundMain(void)
{
    struct SoundInfo *soundInfo = GP(SOUND_INFO_PTR);
    struct SoundChannel *chan;
    u32 maxLines, divFreq, counter;
    s32 samples, chansLeft;
    s8 *buffer;

    if (soundInfo->ident != ID_NUMBER)
        return;
    soundInfo->ident++;
    maxLines = soundInfo->maxLines;
    if (maxLines != 0)
        maxLines += LinesSinceVBlank();
    if (soundInfo->func != 0)
        ((void (*)(struct MusicPlayerInfo *))PortGbaFunctionToNative(soundInfo->func))(
            (struct MusicPlayerInfo *)(uintptr_t)soundInfo->intp);
    GF(soundInfo->CgbSound)();

    // The part of the buffer that the FIFO DMA plays in pcmDmaCounter frames.
    samples = soundInfo->pcmSamplesPerVBlank;
    buffer = soundInfo->pcmBuffer;
    counter = soundInfo->pcmDmaCounter;
    if (counter > 1)
        buffer += (s32)((soundInfo->pcmDmaPeriod - (counter - 1)) * (u32)samples);

    if (soundInfo->reverb != 0) {
        // Echo of the oldest part of the buffer, both channels mixed together.
        s8 *p = buffer, *old = counter == 2 ? soundInfo->pcmBuffer : buffer + samples;
        s32 n = samples;

        do {
            s32 sum = p[PCM_DMA_BUF_SIZE] + p[0] + old[PCM_DMA_BUF_SIZE] + old[0];
            s32 value = (s32)(sum * (u32)soundInfo->reverb) >> 9;

            if (value & 0x80)
                value++;
            p[PCM_DMA_BUF_SIZE] = (s8)value;
            p[0] = (s8)value;
            p++;
            old++;
        } while (--n > 0);
    } else {
        // Cleared in words: 1 and 2 for bits 2 and 3 of the length, then at least one run of 4.
        u32 *right = (u32 *)buffer, *left = (u32 *)(buffer + PCM_DMA_BUF_SIZE);
        u32 n = (u32)samples;
        s32 runs;

        if (n & 4) {
            *right++ = 0;
            *left++ = 0;
        }
        if (n & 8) {
            right[0] = right[1] = 0;
            left[0] = left[1] = 0;
            right += 2;
            left += 2;
        }
        runs = (s32)(n >> 4);
        do {
            right[0] = right[1] = right[2] = right[3] = 0;
            left[0] = left[1] = left[2] = left[3] = 0;
            right += 4;
            left += 4;
        } while (--runs > 0);
    }

    divFreq = soundInfo->divFreq;
    chan = soundInfo->chans;
    chansLeft = soundInfo->maxChans;
    do {
        struct WaveData *wav = GP(chan->wav);
        const s8 *loopStart = NULL;
        s32 loopLength, words, count;
        const s8 *pos;
        s8 *out;
        u32 status, envelope;

        if (maxLines != 0 && LinesSinceVBlank() >= maxLines)
            break;
        status = chan->status;
        if (!(status & CHANNEL_ON))
            goto next;

        // The envelope: attack (3), decay (2), sustain (1), release (0x40), echo (4).
        if (status & 0x80) {
            if (status & 0x40) {
                chan->status = 0;
                goto next;
            }
            status = 3;
            chan->status = status;
            chan->cp = (u32)(uintptr_t)wav->data;
            chan->ct = wav->size;
            envelope = 0;
            chan->ev = 0;
            chan->fw = 0;
            if (wav->status & 0xC000) {
                status |= 0x10;  // looped
                chan->status = status;
            }
            goto attack;
        }
        envelope = chan->ev;
        if (status & 4) {
            if (chan->echoLength-- > 1)
                goto envelopeSet;
            chan->status = 0;
            goto next;
        }
        if (status & 0x40) {
            envelope = envelope * chan->release >> 8;
            if (envelope > chan->echoVolume)
                goto envelopeSet;
        echo:
            envelope = chan->echoVolume;
            if (envelope == 0) {
                chan->status = 0;
                goto next;
            }
            status |= 4;
            chan->status = status;
            goto envelopeSet;
        }
        if ((status & 3) == 2) {
            envelope = envelope * chan->decay >> 8;
            if (envelope > chan->sustain)
                goto envelopeSet;
            envelope = chan->sustain;
            if (envelope == 0)
                goto echo;
            status--;
            chan->status = status;
        } else if ((status & 3) == 3) {
        attack:
            envelope += chan->attack;
            if (envelope >= 0xFF) {
                envelope = 0xFF;
                status--;
                chan->status = status;
            }
        }
    envelopeSet:
        chan->ev = envelope;
        envelope = (soundInfo->masterVolume + 1) * envelope >> 4;
        chan->er = chan->rightVolume * envelope >> 8;
        chan->el = chan->leftVolume * envelope >> 8;

        loopLength = status & 0x10;
        if (loopLength != 0) {
            loopStart = wav->data + wav->loopStart;
            loopLength = wav->size - wav->loopStart;
        }

        // The mix, a word (4 samples) at a time: at least one word, then while samples remain.
        out = buffer;
        count = chan->ct;
        pos = (const s8 *)(uintptr_t)chan->cp;
        words = samples;
        if (chan->type & 8) {
            // Fixed frequency: a sample per output sample.
            do {
                int i;
                for (i = 0; i < 4; i++) {
                    s32 sample = *pos++;

                    MixSample(out, sample, chan->er);
                    MixSample(out + PCM_DMA_BUF_SIZE, sample, chan->el);
                    out++;
                    if (--count == 0) {
                        if (loopLength == 0) {
                            chan->status = 0;
                            goto next;
                        }
                        count = loopLength;
                        pos = loopStart;
                    }
                }
            } while ((words -= 4) > 0);
        } else {
            // Resampled, interpolating between neighbouring samples. pos is one past the first.
            u32 fraction = chan->fw, step = divFreq * chan->freq;
            s32 sample = pos[0], delta = pos[1] - sample;

            pos++;
            do {
                int i;
                for (i = 0; i < 4; i++) {
                    s32 value = sample + ((s32)(fraction * (u32)delta) >> 23);
                    u32 advance;

                    MixSample(out, value, chan->er);
                    MixSample(out + PCM_DMA_BUF_SIZE, value, chan->el);
                    out++;
                    fraction += step;
                    advance = fraction >> 23;
                    if (advance == 0)
                        continue;
                    fraction &= ~0x3F800000u;
                    count -= (s32)advance;
                    if (count <= 0) {
                        s32 skip = -count;

                        if (loopLength == 0) {
                            chan->status = 0;
                            goto next;
                        }
                        for (;;) {
                            count += loopLength;
                            if (count > 0)
                                break;
                            skip -= loopLength;
                        }
                        pos = loopStart + skip;
                    } else {
                        pos += advance - 1;
                    }
                    sample = *pos;
                    delta = *++pos - sample;
                }
            } while ((words -= 4) > 0);
            pos--;
            chan->fw = fraction;
        }
        chan->ct = count;
        chan->cp = (u32)(uintptr_t)pos;

    next:
        chan++;
    } while (--chansLeft > 0);

    soundInfo->ident = ID_NUMBER;
}

// --- MPlayMain: one frame of a music player ------------------------------------------------

// Sets a channel's volumes from its velocity, the track's volumes and the rhythm pan.
static void ChnVolSetAsm(struct SoundChannel *chan, struct MusicPlayerTrack *track)
{
    s32 pan = (s8)chan->rp;
    u32 volume;

    volume = (u32)((s32)(track->volMR * ((0x80 + pan) * chan->ve)) >> 14);
    chan->rightVolume = volume > 0xFF ? 0xFF : volume;
    volume = (u32)((s32)(track->volML * ((0x7F - pan) * chan->ve)) >> 14);
    chan->leftVolume = volume > 0xFF ? 0xFF : volume;
}

static void ClearModMAsm(struct MusicPlayerTrack *track)
{
    track->modM = 0;
    track->lfoSpeedC = 0;
    track->flags |= track->modT == 0 ? 0xC : 3;
}

// The track's key, shifted, for a channel's pitch: never below 0.
static u32 ShiftedKey(u32 key, struct MusicPlayerTrack *track)
{
    s32 shifted = (s32)key + (s8)track->keyM;

    return shifted < 0 ? 0 : (u32)shifted;
}

static MPlayCommand JumpTableEntry(struct SoundInfo *soundInfo, u32 index)
{
    return (MPlayCommand)PortGbaFunctionToNative(((u32 *)(uintptr_t)soundInfo->MPlayJumpTable)[index]);
}

// Runs a tick of a playing track: its channels' gate times, its commands up to the next wait,
// and its LFO.
static void TrackTick(struct MusicPlayerInfo *mplayInfo, struct SoundInfo *soundInfo,
                      struct MusicPlayerTrack *track)
{
    struct SoundChannel *chan;
    u32 speed;
    s32 depth;

    for (chan = GP(track->chan); chan != NULL; chan = NEXT_CHANNEL(chan)) {
        if (chan->status & CHANNEL_ON) {
            if (chan->gt != 0 && --chan->gt == 0)
                chan->status |= 0x40;
        } else {
            ClearChain(chan);
        }
    }
    if (track->flags & 0x40) {
        Clear64byte(track);
        track->flags = 0x80;
        track->bendRange = 2;
        track->volX = 64;
        track->lfoSpeed = 22;
        track->tone.type = 1;
    }

    while (track->wait == 0) {
        u8 *p = GP(track->cmdPtr);
        u32 cmd = *p;

        if (cmd < 0x80) {
            cmd = track->runningStatus;
        } else {
            GP_SET(track->cmdPtr, p + 1);
            if (cmd >= 0xBD)
                track->runningStatus = cmd;
        }
        if (cmd >= 0xCF) {
            GF(soundInfo->plynote)(cmd - 0xCF, mplayInfo, track);
        } else if (cmd > 0xB0) {
            mplayInfo->cmd = cmd - 0xB1;
            JumpTableEntry(soundInfo, cmd - 0xB1)(mplayInfo, track);
            if (track->flags == 0)
                return;
        } else {
            track->wait = sClockTable[cmd - 0x80];
        }
    }

    track->wait--;
    if (track->lfoSpeed == 0 || track->mod == 0)
        return;
    if (track->lfoDelayC != 0) {
        track->lfoDelayC--;
        return;
    }
    // A triangle wave. The asm keeps the sum before it wraps to 8 bits.
    speed = track->lfoSpeedC + track->lfoSpeed;
    track->lfoSpeedC = speed;
    depth = (speed - 0x40) & 0x80 ? (s8)speed : (s32)(0x80 - speed);
    depth = (s32)(track->mod * depth) >> 6;
    if ((u8)(track->modM ^ depth) != 0) {
        track->modM = depth;
        track->flags |= track->modT == 0 ? 0xC : 3;
    }
}

void MPlayMain(struct MusicPlayerInfo *mplayInfo)
{
    struct SoundInfo *soundInfo;
    struct MusicPlayerTrack *track;
    u32 tempo;
    s32 n;

    if (mplayInfo->ident != ID_NUMBER)
        return;
    mplayInfo->ident++;
    if (mplayInfo->func != 0)
        ((void (*)(struct MusicPlayerInfo *))PortGbaFunctionToNative(mplayInfo->func))(
            (struct MusicPlayerInfo *)(uintptr_t)mplayInfo->intp);
    if ((s32)mplayInfo->status < 0)
        goto end;
    soundInfo = GP(SOUND_INFO_PTR);
    FadeOutBody(mplayInfo);
    if ((s32)mplayInfo->status < 0)
        goto end;

    // A tick for every 150 of tempo.
    tempo = mplayInfo->tempoC + mplayInfo->tempoI;
    for (;;) {
        u32 bit = 1, active = 0;

        mplayInfo->tempoC = tempo;
        if (tempo < 150)
            break;
        track = GP(mplayInfo->tracks);
        n = mplayInfo->trackCount;
        do {
            if (track->flags & 0x80) {
                active |= bit;
                TrackTick(mplayInfo, soundInfo, track);
            }
            track++;
            bit <<= 1;
        } while (--n > 0);
        mplayInfo->clock++;
        if (active == 0) {
            mplayInfo->status = MUSICPLAYER_STATUS_PAUSE;
            goto end;
        }
        mplayInfo->status = active;
        tempo = mplayInfo->tempoC - 150u;
    }

    // Volume and pitch changes reach the channels.
    track = GP(mplayInfo->tracks);
    n = mplayInfo->trackCount;
    do {
        if ((track->flags & 0x80) && (track->flags & 0xF)) {
            struct SoundChannel *chan;

            TrkVolPitSet(mplayInfo, track);
            for (chan = GP(track->chan); chan != NULL; chan = NEXT_CHANNEL(chan)) {
                u32 cgbType;

                if (!(chan->status & CHANNEL_ON)) {
                    ClearChain(chan);
                    continue;
                }
                cgbType = chan->type & 7;
                if (track->flags & 3) {
                    ChnVolSetAsm(chan, track);
                    if (cgbType != 0)
                        ((struct CgbChannel *)chan)->mo |= 1;
                }
                if (track->flags & 0xC) {
                    u32 key = ShiftedKey(chan->ky, track);

                    if (cgbType != 0) {
                        chan->freq = GF(soundInfo->MidiKeyToCgbFreq)(cgbType, key, track->pitM);
                        ((struct CgbChannel *)chan)->mo |= 2;
                    } else {
                        chan->freq = MidiKeyToFreq(GP(chan->wav), key, track->pitM);
                    }
                }
            }
            track->flags &= 0xF0;
        }
        track++;
    } while (--n > 0);

end:
    mplayInfo->ident = ID_NUMBER;
}

// Stops a track's channels: switches off the CGB ones and unlinks them all.
void TrackStop(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    struct SoundChannel *chan;

    (void)mplayInfo;
    if (!(track->flags & 0x80))
        return;
    for (chan = GP(track->chan); chan != NULL; chan = NEXT_CHANNEL(chan)) {
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
    next = NEXT_CHANNEL(chan);
    prev = (struct SoundChannel *)(uintptr_t)chan->pp;
    if (prev != NULL)
        prev->np = chan->np;
    else
        GP_SET(track->chan, next);
    if (next != NULL)
        next->pp = chan->pp;
    GP_SET(chan->track, NULL);
}

// --- Notes ------------------------------------------------------------------------------------

// Starts a note (commands 0xCF-0xFF; noteIndex counts from 0xCF) on a free channel, or on the
// one with the lowest priority.
void ply_note(u32 noteIndex, struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    struct SoundInfo *soundInfo = GP(SOUND_INFO_PTR);
    const struct ToneData *tone;
    struct SoundChannel *chan;
    u8 *p;
    u32 key, priority, cgbType, freq;
    s32 rhythmPan = 0;

    track->gateTime = sClockTable[noteIndex];
    p = GP(track->cmdPtr);
    if (*p < 0x80) {
        track->key = *p++;
        if (*p < 0x80) {
            track->velocity = *p++;
            if (*p < 0x80)
                track->gateTime += *p++;
        }
        GP_SET(track->cmdPtr, p);
    }

    // A drum kit or key split picks the tone by key.
    if (track->tone.type & 0xC0) {
        u32 index = track->key;

        key = track->key;
        if (track->tone.type & 0x40) {
            u32 keySplitTable;  // in the place of the envelope

            memcpy(&keySplitTable, &track->tone.attack, 4);
            index = ((const u8 *)(uintptr_t)keySplitTable)[key];
        }
        tone = (const struct ToneData *)(uintptr_t)(track->tone.wav.a + index * 12);
        if (tone->type & 0xC0)
            return;
        if (track->tone.type & 0x80) {
            if (tone->pan_sweep & 0x80)
                rhythmPan = (s32)(tone->pan_sweep - 0xC0) * 2;
            key = tone->key;
        }
    } else {
        tone = &track->tone;
        key = track->key;
    }

    priority = mplayInfo->priority + track->priority;
    if (priority > 0xFF)
        priority = 0xFF;
    cgbType = tone->type & 7;
    if (cgbType != 0) {
        // A CGB tone has its own channel; take it unless it plays something more important.
        struct CgbChannel *cgbChans = GP(soundInfo->cgbChans);

        if (cgbChans == NULL)
            return;
        chan = (struct SoundChannel *)&cgbChans[cgbType - 1];
        if ((chan->status & CHANNEL_ON) && !(chan->status & 0x40)) {
            if (chan->pr > priority)
                return;
            if (chan->pr == priority && chan->track.a < (u32)(uintptr_t)track)
                return;
        }
    } else {
        // A free channel, or else the lowest priority one, releasing ones first, and of those
        // the one of the highest track.
        struct SoundChannel *c = soundInfo->chans;
        u32 bestPriority = priority, bestTrack = (u32)(uintptr_t)track;
        int releasing = 0;
        s32 left = soundInfo->maxChans;

        chan = NULL;
        do {
            if (!(c->status & CHANNEL_ON)) {
                chan = c;
                break;
            }
            if (c->status & 0x40) {
                if (!releasing) {
                    releasing = 1;
                    bestPriority = c->pr;
                    bestTrack = c->track.a;
                    chan = c;
                    goto nextChannel;
                }
            } else if (releasing) {
                goto nextChannel;
            }
            if (c->pr < bestPriority) {
                bestPriority = c->pr;
                bestTrack = c->track.a;
                chan = c;
            } else if (c->pr == bestPriority) {
                if (c->track.a > bestTrack) {
                    bestTrack = c->track.a;
                    chan = c;
                } else if (c->track.a == bestTrack) {
                    chan = c;
                }
            }
        nextChannel:
            c++;
        } while (--left > 0);
        if (chan == NULL)
            return;
    }

    ClearChain(chan);
    chan->pp = 0;
    chan->np = track->chan.a;
    if (track->chan.a != 0)
        GP(track->chan)->pp = (u32)(uintptr_t)chan;
    GP_SET(track->chan, chan);
    GP_SET(chan->track, track);
    track->lfoDelayC = track->lfoDelay;
    if (track->lfoDelay != 0)
        ClearModMAsm(track);
    TrkVolPitSet(mplayInfo, track);

    memcpy(&chan->gt, &track->gateTime, 4);  // gate time, key, velocity and a byte more
    chan->pr = priority;
    chan->ky = key;
    chan->rp = rhythmPan;
    chan->type = tone->type;
    chan->wav.a = tone->wav.a;
    memcpy(&chan->attack, &tone->attack, 4);
    memcpy(&chan->echoVolume, &track->echoVolume, 2);
    ChnVolSetAsm(chan, track);
    key = ShiftedKey(chan->ky, track);
    if (cgbType != 0) {
        struct CgbChannel *cgb = (struct CgbChannel *)chan;
        u32 sweep = tone->pan_sweep;

        cgb->le = tone->length;
        if ((sweep & 0x80) || !(sweep & 0x70))
            sweep = 8;
        cgb->sw = sweep;
        freq = GF(soundInfo->MidiKeyToCgbFreq)(cgbType, key, track->pitM);
    } else {
        freq = MidiKeyToFreq(GP(chan->wav), key, track->pitM);
    }
    chan->freq = freq;
    chan->status = 0x80;
    track->flags &= 0xF0;
}

// Releases the track's note of a key, given or the last one.
void ply_endtie(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);
    struct SoundChannel *chan;
    u32 key;

    (void)mplayInfo;
    if (*p < 0x80) {
        key = *p;
        track->key = key;
        GP_SET(track->cmdPtr, p + 1);
    } else {
        key = track->key;
    }
    for (chan = GP(track->chan); chan != NULL; chan = NEXT_CHANNEL(chan)) {
        if ((chan->status & 0x83) && !(chan->status & 0x40) && chan->mk == key) {
            chan->status |= 0x40;
            return;
        }
    }
}

// --- The other commands, through the jump table ----------------------------------------------

// Ends the track: releases its channels and unlinks them.
void ply_fine(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    struct SoundChannel *chan;

    (void)mplayInfo;
    for (chan = GP(track->chan); chan != NULL; chan = NEXT_CHANNEL(chan)) {
        if (chan->status & CHANNEL_ON)
            chan->status |= 0x40;
        RealClearChain(chan);
    }
    track->flags = 0;
}

void ply_goto(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);

    (void)mplayInfo;
    track->cmdPtr.a = (u32)p[3] << 24 | (u32)p[2] << 16 | (u32)p[1] << 8 | GuardedByte(p);
}

void ply_patt(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    if (track->patternLevel >= 3) {
        ply_fine(mplayInfo, track);
        return;
    }
    GP_SET(track->patternStack[track->patternLevel], GP(track->cmdPtr) + 4);
    track->patternLevel++;
    ply_goto(mplayInfo, track);
}

void ply_pend(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    if (track->patternLevel != 0) {
        track->patternLevel--;
        track->cmdPtr.a = track->patternStack[track->patternLevel].a;
    }
}

// Repeats a part: a count (0 for always) and an address.
void ply_rept(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);
    u32 repeats;

    if (*p == 0) {
        GP_SET(track->cmdPtr, p + 1);
        ply_goto(mplayInfo, track);
        return;
    }
    repeats = track->repN + 1u;
    track->repN = repeats;
    if (repeats < ReadCommand(track)) {
        ply_goto(mplayInfo, track);
        return;
    }
    track->repN = 0;
    GP_SET(track->cmdPtr, p + 5);
}

void ply_prio(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->priority = ReadCommand(track);
}

void ply_tempo(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u32 tempo = ReadCommand(track) * 2;

    mplayInfo->tempoD = tempo;
    mplayInfo->tempoI = tempo * mplayInfo->tempoU >> 8;
}

void ply_keysh(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->keyShift = ReadCommand(track);
    track->flags |= 0xC;
}

// Sets the track's tone from the song's tone table.
void ply_voice(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);
    const u32 *src;
    u32 words[3];
    int i;

    GP_SET(track->cmdPtr, p + 1);
    src = (const u32 *)(GP(mplayInfo->tone) + *p);
    for (i = 0; i < 3; i++)
        words[i] = IsGuarded(src) ? 0 : src[i];
    memcpy(&track->tone, words, sizeof(words));
}

void ply_vol(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->vol = ReadCommand(track);
    track->flags |= 3;
}

void ply_pan(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->pan = ReadCommand(track) - 0x40;
    track->flags |= 3;
}

void ply_bend(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->bend = ReadCommand(track) - 0x40;
    track->flags |= 0xC;
}

void ply_bendr(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->bendRange = ReadCommand(track);
    track->flags |= 0xC;
}

void ply_lfodl(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->lfoDelay = ReadCommand(track);
}

void ply_modt(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u32 type = ReadCommand(track);

    (void)mplayInfo;
    if (track->modT != type) {
        track->modT = type;
        track->flags |= 0xF;
    }
}

void ply_tune(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    (void)mplayInfo;
    track->tune = ReadCommand(track) - 0x40;
    track->flags |= 0xC;
}

// Writes a sound register: an offset from 0x04000060 and a value.
void ply_port(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);

    (void)mplayInfo;
    GP_SET(track->cmdPtr, p + 2);
    *(vu8 *)(uintptr_t)(REG_BASE + 0x60 + *p) = GuardedByte(p + 1);
}

// The LFO commands read without the guard.
void ply_lfos(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);

    (void)mplayInfo;
    GP_SET(track->cmdPtr, p + 1);
    track->lfoSpeed = *p;
    if (*p == 0)
        ClearModMAsm(track);
}

void ply_mod(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u8 *p = GP(track->cmdPtr);

    (void)mplayInfo;
    GP_SET(track->cmdPtr, p + 1);
    track->mod = *p;
    if (*p == 0)
        ClearModMAsm(track);
}

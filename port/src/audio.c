#include "port/audio.h"

#include <string.h>

#include "port/gba_io.h"
#include "port/gba_memory.h"

// Written from GBATEK's description of the hardware. Time moves in steps of one output sample
// (PORT_AUDIO_CYCLES_PER_SAMPLE CPU cycles); a register write takes effect at the next step.

#define REG_TM0CNT_L 0x100
#define REG_SOUNDCNT_X 0x084
#define REG_SOUNDBIAS 0x088
#define REG_WAVE_RAM 0x090
#define REG_FIFO_A 0x0A0
#define REG_FIFO_B 0x0A4

#define CPU_HZ 16777216
#define FRAME_SEQUENCER_CYCLES (CPU_HZ / 512)
#define RING_FRAMES 65536  // one second

struct Envelope {
    uint8_t initial, increase, period;  // NRx2
    uint8_t volume, timer;
};

struct Square {
    int on;
    uint8_t duty, position;
    uint16_t frequency;
    int32_t timer;
    uint16_t length;
    int lengthOn;
    struct Envelope envelope;
};

struct Wave {
    int on, dac;
    uint8_t bank, twoBanks, volumeCode, force75;
    uint16_t rate;
    int32_t timer;
    uint16_t length;
    int lengthOn;
    uint8_t position;
    uint8_t ram[2][16];
};

struct Noise {
    int on;
    uint8_t ratio, shift, narrow;
    uint16_t lfsr;
    int32_t timer;
    uint16_t length;
    int lengthOn;
    struct Envelope envelope;
};

struct Fifo {
    uint8_t data[32];
    unsigned int read, count;
    uint32_t dma;  // where DMA fills it from, or 0
    int8_t sample;
};

static struct {
    int enabled;  // SOUNDCNT_X bit 7
    struct Square square[2];
    struct {
        uint8_t shift, decrease, period, timer;
        int on;
        uint16_t shadow;
    } sweep;
    struct Wave wave;
    struct Noise noise;
    uint8_t nr50, nr51, soundcntH[2];
    uint16_t bias;
    struct Fifo fifo[2];
    int32_t timerCount[2];
    int timerRunning[2];
    unsigned int clock;  // cycles not yet turned into a sample
    int32_t sequencerTimer;
    unsigned int sequencerStep;
    int32_t lastIn[2], lastOut[2];  // the output's DC filter
    int16_t ring[RING_FRAMES * 2];
    size_t ringRead, ringCount;
} s;

static const uint8_t kDuty[4][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 1, 1 },
    { 0, 1, 1, 1, 1, 1, 1, 0 },
};

static void UpdateStatus(void)
{
    uint8_t *io = PortIoHost();

    io[REG_SOUNDCNT_X] = (uint8_t)((io[REG_SOUNDCNT_X] & 0x80) | (s.square[0].on ? 1 : 0) | (s.square[1].on ? 2 : 0)
                                   | (s.wave.on ? 4 : 0) | (s.noise.on ? 8 : 0));
}

// The CPU reaches the wave RAM bank that is not playing (bank 1 with the sound off).
static unsigned int CpuWaveBank(void)
{
    return s.enabled ? !s.wave.bank : 1;
}

static void ShowWaveRam(void)
{
    memcpy(PortIoHost() + REG_WAVE_RAM, s.wave.ram[CpuWaveBank()], 16);
}

// --- Envelopes, length and sweep: the frame sequencer --------------------------------------

static void WriteEnvelope(struct Envelope *e, uint8_t value)
{
    e->initial = value >> 4;
    e->increase = (value >> 3) & 1;
    e->period = value & 7;
}

static int DacOn(const struct Envelope *e)
{
    return e->initial != 0 || e->increase;
}

static void StartEnvelope(struct Envelope *e)
{
    e->volume = e->initial;
    e->timer = e->period;
}

static void ClockEnvelope(struct Envelope *e)
{
    if (e->period == 0 || --e->timer != 0)
        return;
    e->timer = e->period;
    if (e->increase && e->volume < 15)
        e->volume++;
    else if (!e->increase && e->volume > 0)
        e->volume--;
}

static void ClockLength(int *on, uint16_t *length, int lengthOn)
{
    if (lengthOn && *length != 0 && --*length == 0)
        *on = 0;
}

static int SweepTarget(void)
{
    int delta = s.sweep.shadow >> s.sweep.shift;

    return s.sweep.decrease ? s.sweep.shadow - delta : s.sweep.shadow + delta;
}

static void ClockSweep(void)
{
    int target;

    if (--s.sweep.timer != 0)
        return;
    s.sweep.timer = s.sweep.period ? s.sweep.period : 8;
    if (!s.sweep.on || s.sweep.period == 0)
        return;
    target = SweepTarget();
    if (target > 2047) {
        s.square[0].on = 0;
    } else if (s.sweep.shift != 0) {
        s.square[0].frequency = s.sweep.shadow = (uint16_t)target;
        if (SweepTarget() > 2047)
            s.square[0].on = 0;
    }
}

// 512 times a second: lengths at 256 Hz, the sweep at 128 Hz, envelopes at 64 Hz.
static void ClockSequencer(void)
{
    s.sequencerStep = (s.sequencerStep + 1) & 7;
    if ((s.sequencerStep & 1) == 0) {
        ClockLength(&s.square[0].on, &s.square[0].length, s.square[0].lengthOn);
        ClockLength(&s.square[1].on, &s.square[1].length, s.square[1].lengthOn);
        ClockLength(&s.wave.on, &s.wave.length, s.wave.lengthOn);
        ClockLength(&s.noise.on, &s.noise.length, s.noise.lengthOn);
    }
    if (s.sequencerStep == 2 || s.sequencerStep == 6)
        ClockSweep();
    if (s.sequencerStep == 7) {
        ClockEnvelope(&s.square[0].envelope);
        ClockEnvelope(&s.square[1].envelope);
        ClockEnvelope(&s.noise.envelope);
    }
}

// --- Triggers -------------------------------------------------------------------------------

static void TriggerSquare(int n)
{
    struct Square *c = &s.square[n];

    c->on = DacOn(&c->envelope);
    StartEnvelope(&c->envelope);
    if (c->length == 0)
        c->length = 64;
    c->timer = 16 * (2048 - c->frequency);
    if (n == 0) {
        s.sweep.shadow = c->frequency;
        s.sweep.timer = s.sweep.period ? s.sweep.period : 8;
        s.sweep.on = s.sweep.period != 0 || s.sweep.shift != 0;
        if (s.sweep.shift != 0 && SweepTarget() > 2047)
            c->on = 0;
    }
}

static void TriggerWave(void)
{
    s.wave.on = s.wave.dac;
    if (s.wave.length == 0)
        s.wave.length = 256;
    s.wave.position = 0;
    s.wave.timer = 8 * (2048 - s.wave.rate);
}

static void TriggerNoise(void)
{
    s.noise.on = DacOn(&s.noise.envelope);
    StartEnvelope(&s.noise.envelope);
    if (s.noise.length == 0)
        s.noise.length = 64;
    s.noise.lfsr = s.noise.narrow ? 0x7F : 0x7FFF;
    s.noise.timer = (s.noise.ratio ? 64 * s.noise.ratio : 32) << s.noise.shift;
}

// --- FIFOs ----------------------------------------------------------------------------------

static void FifoPush(struct Fifo *f, uint8_t value)
{
    if (f->count == sizeof(f->data))
        return;
    f->data[(f->read + f->count) % sizeof(f->data)] = value;
    f->count++;
}

static uint8_t ReadGba8(uint32_t address)
{
    if ((address - GBA_EWRAM_BASE < GBA_EWRAM_SIZE) || (address - GBA_IWRAM_BASE < GBA_IWRAM_SIZE)
        || (address - GBA_ROM_BASE < GBA_ROM_SIZE))
        return *(const volatile uint8_t *)(uintptr_t)address;
    return 0;
}

// A timer overflow: the FIFO plays its next byte, and asks DMA for 4 words when 4 or fewer
// are left.
static void FifoTick(struct Fifo *f)
{
    if (f->count != 0) {
        f->sample = (int8_t)f->data[f->read];
        f->read = (f->read + 1) % sizeof(f->data);
        f->count--;
    }
    if (f->count <= 16 && f->dma != 0) {
        int i;
        for (i = 0; i < 16; i++)
            FifoPush(f, ReadGba8(f->dma++));
    }
}

void PortAudioSetFifoDma(int fifo, uint32_t source)
{
    s.fifo[fifo].dma = source;
}

static void RunTimers(int32_t cycles)
{
    static const unsigned int kPrescale[4] = { 0, 6, 8, 10 };
    const uint8_t *io = PortIoHost();
    int n;

    for (n = 0; n < 2; n++) {
        unsigned int base = REG_TM0CNT_L + 4 * n;
        uint16_t reload = (uint16_t)(io[base] | io[base + 1] << 8), control = io[base + 2];
        int32_t period = (int32_t)((0x10000u - reload) << kPrescale[control & 3]);

        if (!(control & 0x80) || (control & 4)) {  // stopped, or counting the timer below
            s.timerRunning[n] = 0;
            continue;
        }
        if (!s.timerRunning[n]) {
            s.timerRunning[n] = 1;
            s.timerCount[n] = period;
        }
        for (s.timerCount[n] -= cycles; s.timerCount[n] <= 0; s.timerCount[n] += period) {
            if (((s.soundcntH[1] >> 2) & 1) == n)
                FifoTick(&s.fifo[0]);
            if (((s.soundcntH[1] >> 6) & 1) == n)
                FifoTick(&s.fifo[1]);
        }
    }
}

// --- Channels -------------------------------------------------------------------------------

// Steps a channel's timer by the cycles of a sample; returns how many periods ended.
static unsigned int Steps(int32_t *timer, int32_t period, int32_t cycles)
{
    unsigned int n = 0;

    *timer -= cycles;
    if (*timer <= 0) {
        n = (unsigned int)(-*timer / period) + 1;
        *timer += (int32_t)n * period;
    }
    return n;
}

static int SquareOutput(struct Square *c, int32_t cycles)
{
    c->position = (uint8_t)((c->position + Steps(&c->timer, 16 * (2048 - c->frequency), cycles)) & 7);
    return c->on && kDuty[c->duty][c->position] ? c->envelope.volume : 0;
}

static int WaveOutput(int32_t cycles)
{
    static const uint8_t kShift[4] = { 4, 0, 1, 2 };
    struct Wave *w = &s.wave;
    unsigned int samples = w->twoBanks ? 64 : 32, bank, value;

    w->position = (uint8_t)((w->position + Steps(&w->timer, 8 * (2048 - w->rate), cycles)) % samples);
    if (!w->on)
        return 0;
    bank = (w->bank + w->position / 32) & 1;
    value = w->ram[bank][(w->position % 32) / 2];
    value = w->position & 1 ? value & 0xF : value >> 4;
    return w->force75 ? (int)(value * 3 / 4) : (int)(value >> kShift[w->volumeCode]);
}

static int NoiseOutput(int32_t cycles)
{
    struct Noise *c = &s.noise;
    unsigned int n = Steps(&c->timer, (c->ratio ? 64 * c->ratio : 32) << c->shift, cycles);

    while (n-- != 0) {
        unsigned int bit = (c->lfsr ^ (c->lfsr >> 1)) & 1;
        c->lfsr = (uint16_t)((c->lfsr >> 1) | bit << 14);
        if (c->narrow)
            c->lfsr = (uint16_t)((c->lfsr & ~0x40u) | bit << 6);
    }
    return c->on && !(c->lfsr & 1) ? c->envelope.volume : 0;
}

// --- Mixing ---------------------------------------------------------------------------------

// The hardware's output stage: a capacitor blocks DC, which the PSG's unsigned waves carry.
static int16_t BlockDc(int side, int32_t in)
{
    int64_t out = in - s.lastIn[side] + (((int64_t)s.lastOut[side] * 65400) >> 16);

    s.lastIn[side] = in;
    s.lastOut[side] = (int32_t)out;
    return (int16_t)(out > 32767 ? 32767 : out < -32768 ? -32768 : out);
}

static void Sample(void)
{
    int32_t cycles = PORT_AUDIO_CYCLES_PER_SAMPLE;
    int out[4], side;
    int32_t mixed[2];

    s.sequencerTimer -= cycles;
    while (s.sequencerTimer <= 0) {
        s.sequencerTimer += FRAME_SEQUENCER_CYCLES;
        ClockSequencer();
    }
    RunTimers(cycles);
    out[0] = SquareOutput(&s.square[0], cycles);
    out[1] = SquareOutput(&s.square[1], cycles);
    out[2] = WaveOutput(cycles);
    out[3] = NoiseOutput(cycles);
    UpdateStatus();

    for (side = 0; side < 2; side++) {  // left, right
        unsigned int pan = side == 0 ? s.nr51 >> 4 : s.nr51, volume = side == 0 ? s.nr50 >> 4 : s.nr50;
        int32_t psg = 0, value;
        int n;

        for (n = 0; n < 4; n++)
            if (pan & (1u << n))
                psg += out[n];
        // SOUNDCNT_H's PSG volume: 25%, 50%, 100% (3 is prohibited). The balance against
        // Direct Sound is mGBA's.
        psg = (psg * 8 * (int32_t)(1 + (volume & 7))) >> (4 - (s.soundcntH[0] & 3));
        value = s.enabled ? psg : 0;
        // Direct Sound: SOUNDCNT_H's enable bits are right (bit 0) then left (bit 1).
        if (s.soundcntH[1] & (side == 0 ? 2 : 1))
            value += (s.fifo[0].sample * 4) >> !(s.soundcntH[0] & 4);
        if (s.soundcntH[1] & (side == 0 ? 0x20 : 0x10))
            value += (s.fifo[1].sample * 4) >> !(s.soundcntH[0] & 8);
        // The 10-bit DAC around the bias level.
        value += s.bias;
        value = value < 0 ? 0 : value > 0x3FF ? 0x3FF : value;
        mixed[side] = (value - s.bias) * 64;
    }

    {
        size_t at = (s.ringRead + s.ringCount) % RING_FRAMES;
        s.ring[at * 2] = BlockDc(0, mixed[0]);
        s.ring[at * 2 + 1] = BlockDc(1, mixed[1]);
        if (s.ringCount < RING_FRAMES)
            s.ringCount++;
        else
            s.ringRead = (s.ringRead + 1) % RING_FRAMES;
    }
}

void PortAudioRun(unsigned int cycles)
{
    s.clock += cycles;
    while (s.clock >= PORT_AUDIO_CYCLES_PER_SAMPLE) {
        s.clock -= PORT_AUDIO_CYCLES_PER_SAMPLE;
        Sample();
    }
}

size_t PortAudioTake(int16_t *frames, size_t maxFrames)
{
    size_t n = 0;

    while (n < maxFrames && s.ringCount != 0) {
        frames[n * 2] = s.ring[s.ringRead * 2];
        frames[n * 2 + 1] = s.ring[s.ringRead * 2 + 1];
        s.ringRead = (s.ringRead + 1) % RING_FRAMES;
        s.ringCount--;
        n++;
    }
    return n;
}

// --- Registers ------------------------------------------------------------------------------

static void PowerOff(void)
{
    uint8_t *io = PortIoHost();

    memset(&s.square, 0, sizeof(s.square));
    memset(&s.sweep, 0, sizeof(s.sweep));
    memset(&s.noise, 0, sizeof(s.noise));
    s.wave.on = s.wave.dac = 0;
    s.wave.bank = s.wave.twoBanks = s.wave.volumeCode = s.wave.force75 = 0;
    s.wave.rate = s.wave.length = 0;
    s.wave.lengthOn = 0;
    s.nr50 = s.nr51 = 0;
    s.soundcntH[0] = 0;
    memset(io + 0x060, 0, 0x082 - 0x060);
    io[0x082] = 0;
}

void PortAudioWrite(unsigned int offset, uint8_t value)
{
    uint8_t *io = PortIoHost();
    uint8_t keep = 0xFF;  // the bits that read back

    if (offset >= REG_WAVE_RAM && offset < REG_WAVE_RAM + 16) {
        s.wave.ram[CpuWaveBank()][offset - REG_WAVE_RAM] = value;
        return;
    }
    if (offset >= REG_FIFO_A && offset < REG_FIFO_B + 4) {
        FifoPush(&s.fifo[offset >= REG_FIFO_B], value);
        return;
    }
    switch (offset) {
    case 0x060:  // NR10: sweep
        s.sweep.shift = value & 7;
        s.sweep.decrease = (value >> 3) & 1;
        s.sweep.period = (value >> 4) & 7;
        keep = 0x7F;
        break;
    case 0x062:  // NR11, NR21: duty and length
    case 0x068: {
        struct Square *c = &s.square[offset == 0x068];
        c->duty = value >> 6;
        c->length = (uint16_t)(64 - (value & 63));
        keep = 0xC0;
        break;
    }
    case 0x063:  // NR12, NR22: envelope
    case 0x069: {
        struct Square *c = &s.square[offset == 0x069];
        WriteEnvelope(&c->envelope, value);
        if (!DacOn(&c->envelope))
            c->on = 0;
        break;
    }
    case 0x064:  // NR13, NR23: frequency, low bits
    case 0x06C:
        s.square[offset == 0x06C].frequency = (uint16_t)((s.square[offset == 0x06C].frequency & 0x700) | value);
        keep = 0;
        break;
    case 0x065:  // NR14, NR24: frequency, high bits, length on, restart
    case 0x06D: {
        int n = offset == 0x06D;
        struct Square *c = &s.square[n];
        c->frequency = (uint16_t)((c->frequency & 0xFF) | (value & 7) << 8);
        c->lengthOn = (value >> 6) & 1;
        if (value & 0x80)
            TriggerSquare(n);
        keep = 0x40;
        break;
    }
    case 0x070:  // NR30: banks and the wave DAC
        s.wave.twoBanks = (value >> 5) & 1;
        s.wave.bank = (value >> 6) & 1;
        s.wave.dac = value >> 7;
        if (!s.wave.dac)
            s.wave.on = 0;
        keep = 0xE0;
        io[offset] = value & keep;
        ShowWaveRam();
        return;
    case 0x072:  // NR31: length
        s.wave.length = (uint16_t)(256 - value);
        keep = 0;
        break;
    case 0x073:  // NR32: volume
        s.wave.volumeCode = (value >> 5) & 3;
        s.wave.force75 = value >> 7;
        keep = 0xE0;
        break;
    case 0x074:  // NR33: rate, low bits
        s.wave.rate = (uint16_t)((s.wave.rate & 0x700) | value);
        keep = 0;
        break;
    case 0x075:  // NR34
        s.wave.rate = (uint16_t)((s.wave.rate & 0xFF) | (value & 7) << 8);
        s.wave.lengthOn = (value >> 6) & 1;
        if (value & 0x80)
            TriggerWave();
        keep = 0x40;
        break;
    case 0x078:  // NR41: length
        s.noise.length = (uint16_t)(64 - (value & 63));
        keep = 0;
        break;
    case 0x079:  // NR42: envelope
        WriteEnvelope(&s.noise.envelope, value);
        if (!DacOn(&s.noise.envelope))
            s.noise.on = 0;
        break;
    case 0x07C:  // NR43: noise clock
        s.noise.ratio = value & 7;
        s.noise.narrow = (value >> 3) & 1;
        s.noise.shift = value >> 4;
        break;
    case 0x07D:  // NR44
        s.noise.lengthOn = (value >> 6) & 1;
        if (value & 0x80)
            TriggerNoise();
        keep = 0x40;
        break;
    case 0x080:  // NR50: PSG master volume
        s.nr50 = value;
        keep = 0x77;
        break;
    case 0x081:  // NR51: PSG panning
        s.nr51 = value;
        break;
    case 0x082:  // SOUNDCNT_H: PSG and Direct Sound volumes
        s.soundcntH[0] = value;
        keep = 0x0F;
        break;
    case 0x083:  // SOUNDCNT_H: Direct Sound outputs, timers and FIFO resets
        s.soundcntH[1] = value;
        if (value & 0x08)
            s.fifo[0].read = s.fifo[0].count = 0;
        if (value & 0x80)
            s.fifo[1].read = s.fifo[1].count = 0;
        keep = 0x77;
        break;
    case 0x084:  // SOUNDCNT_X: master enable; the status bits are read-only
        if (!(value & 0x80) && s.enabled)
            PowerOff();
        s.enabled = value >> 7;
        io[offset] = value & 0x80;
        UpdateStatus();
        ShowWaveRam();
        return;
    case 0x088:  // SOUNDBIAS
    case 0x089:
        keep = offset == 0x088 ? 0xFE : 0xC3;
        io[offset] = value & keep;
        s.bias = (uint16_t)((io[0x088] | io[0x089] << 8) & 0x3FF);
        return;
    case 0x061:
    case 0x071:
    case 0x085:
        keep = 0;
        break;
    default:
        return;  // not a register
    }
    io[offset] = value & keep;
    UpdateStatus();
}

void PortAudioReset(void)
{
    const uint8_t *io = PortIoHost();

    memset(&s, 0, sizeof(s));
    s.sequencerTimer = FRAME_SEQUENCER_CYCLES;
    s.bias = (uint16_t)((io[REG_SOUNDBIAS] | io[REG_SOUNDBIAS + 1] << 8) & 0x3FF);
}

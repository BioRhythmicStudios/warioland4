#ifndef GUARD_PORT_AUDIO_H
#define GUARD_PORT_AUDIO_H

// The GBA's sound hardware: the four PSG channels and the two Direct Sound FIFOs, which DMA 1
// and 2 fill from the sound engine's buffer, mixed as SOUNDCNT and SOUNDBIAS set them.
//
// gba_io.c passes on the bytes the game writes to the sound registers (0x060-0x0A7) and moves
// the hardware on with the display. The registers in the host view then read as on the GBA,
// and as mGBA keeps them: write-only bits read as 0, SOUNDCNT_X shows which PSG channels are
// playing, and wave RAM shows the bank that is not playing.

#include <stddef.h>
#include <stdint.h>

#define PORT_AUDIO_RATE 65536  // samples per second, SOUNDBIAS's 8-bit resolution
#define PORT_AUDIO_CYCLES_PER_SAMPLE 256

// Puts the sound hardware in the state the BIOS leaves.
void PortAudioReset(void);

// A byte the game wrote to a sound register, at an offset in the I/O page.
void PortAudioWrite(unsigned int offset, uint8_t value);

// DMA 1 or 2 started (source != 0) or stopped (0) feeding FIFO A (0) or B (1).
void PortAudioSetFifoDma(int fifo, uint32_t source);

// Moves the hardware on by a number of CPU cycles, producing samples.
void PortAudioRun(unsigned int cycles);

// Copies out up to maxFrames stereo frames (left, right) produced since the last call, and
// returns how many. Frames not taken before the buffer fills are dropped, oldest first.
size_t PortAudioTake(int16_t *frames, size_t maxFrames);

#endif  // GUARD_PORT_AUDIO_H

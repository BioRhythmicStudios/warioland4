#ifndef GUARD_PORT_SAVE_H
#define GUARD_PORT_SAVE_H

// Cartridge SRAM: 32 KiB in mGBA-compatible .sav format. No game-state layout changes.
// Missing files start erased. Unreadable or incorrectly sized saves are rejected visibly
// without opening a writable session. Do not open another file over pending progress.
int PortSaveOpen(const char *path);

// Once per game frame: wait for 30 stable frames, then persist changes. Failed writes keep
// the latest SRAM and retry after at least 300 frames, once SRAM has settled again.
void PortSaveFrame(void);

// NULL when no write failure is pending; otherwise a player-readable message including the
// destination. The window shows a nonmodal warning; callers may show details on request.
const char *PortSaveError(void);
const char *PortSavePath(void);

// Write current SRAM immediately. Returns -1 on failure, leaving the session open and SRAM
// pending; the caller must offer retry/continue or explicitly confirm discarding on exit.
int PortSaveClose(void);

// Developer test: creates its own disposable child directory; never uses the default save.
int PortSaveTest(const char *directory);

#endif  // GUARD_PORT_SAVE_H

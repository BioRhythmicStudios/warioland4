# Hall of Hieroglyphs Frog Switch freeze (2026-10-18)

## Later manual gameplay confirmation (reported 2026-10-18)

The user has now manually confirmed that the Windows port's Frog Switch no longer
freezes, the escape timer works, and Hall of Hieroglyphs was completed. This closes
the prior request for manual gameplay follow-up. It is user-reported gameplay,
not a new ASan run, an input recording or a continuous automated mGBA proof.
The earlier RAM-positioned fixture limitations below still apply to that automated
evidence only. Broader manual results and private-alpha gaps are recorded in
`docs/M3_ALPHA_READINESS.md`; M3 remains incomplete. No fix code changed here.


Implementation: `c25cc4d` (parent `ff28d49`). The change is entirely in the native
runtime and its tests; no game source, generated symbols, route recordings or ROM
assets changed. The user explicitly prioritized this bug over extending M3.

## Root cause and debugger evidence

The existing normal and ASan executables both stall after Wario lands on the Frog
Switch. ASan displayed no memory error. These reproductions replayed the existing
Hall boot inputs, then positioned Wario above the real room-0 switch in process RAM;
the ordinary collision and sprite code performed activation. No switch/timer state
was forced. The full collectible route was not extended.

LLDB attached to the running frozen normal process (PID 13272). The game thread,
not the debugger's injected break-in thread, had this stack:

```
HBlankCopyBg3HOffsetFromLineOffsets  src/hblank.c:424
InterruptCallbackCallHBlank        src/interrupt_callbacks.c:26
HandleInterrupts                   port/src/frame.c:87
PortFrameHalt                      port/src/frame.c:94
PlatformSyscall                    port/src/bios.c:110
AgbMain                           src/main.c:262
GameEntry
_mco_main
```

The instruction pointer was `0x00007FF685B1AE60` at
`movzwl 0x4000006, %eax` (VCOUNT). Another sample stopped at
`0x00007FF685B58251`, `InterruptCallbackCallHBlank+33`, acknowledging IF.
Across samples, `sFrames=6380` and `sSteps=6369` stayed fixed while the display
line changed (68, 55, 56). `gEscapeHBlankEffectState=5`, `gHBlankMode=3`,
`gTimerState=1`, `gSwitchPressed=1`, IE=`0x2003`, IME=`1`.

This is VBlank starvation: an active hardware-event wait, with the emulated display
and interrupt callbacks still running. It is not an arithmetic loop in Frog Switch
sprite code, an OS lock, or an ASan-detected fault. The switch enables HBlank IRQs.
The HBlank callback reads VCOUNT, and BeforeAccess used to advance the display by
half a scanline for every such read. HBlank IRQ processing therefore consumes the
following scanline boundary, including VBlank at line 160, inside the callback.
That event never reaches the host's frame counter. The callback's `REG_IF |= 2`
acknowledgement also clears pending VBlank (IF is write-one-to-clear), so the main
loop never receives its VBlank wakeup. Host event processing repeats indefinitely.

LLDB transcripts are preserved in the ignored artifact directory below. LLDB's
Windows single-step handling interferes with this port's deliberate I/O traps;
attaching to an already frozen process was sufficient to capture the evidence.
The ASan attach stalled while loading symbols and was closed. There is no claimed
ASan debugger backtrace. LLDB held source files open until it exited; this explained
the transient source-write failures. No backup was restored over any source file.

## Fix

`PortFrameInInterrupt()` reports the scoped interrupt callback dispatch in frame.c.
The I/O layer keeps VCOUNT/DISPSTAT reads on the current event during callbacks.
Ordinary game-code polling still advances the display, so scanline waits still end.
This follows the runtime's existing model that game code takes no emulated time.

The I/O regression uses the real frame dispatcher and interrupt wrappers with a
RAM-registered test HBlank callback. It reads both VCOUNT and DISPSTAT, checks that
they describe the same HBlank, observes three host VBlanks and VBlank delivery, and
checks 616 callbacks (160 + 228 + 228). A callback limit makes failures finite.

## Verification and limits

- Normal and ASan builds succeed. Coroutine, BIOS (all 50 LZ77 streams), and I/O
  tests pass in both, including the new regression.
- Both replay the unchanged `hall_room6_entry.keys` for 25,200 frames without faults.
- Both complete 7,600-frame isolated Frog Switch/exit fixtures without faults.
  Their final captures contain 1,190 frames each, native frames 6411-7600.
  The countdown progresses from 1:00 through 0:59 and onward. Captures include six
  distinct timer values and 55/56 distinct horizontal positions before exit setup.
  Both enter GameScreen submode 6, run the normal vortex/stage-exit sequence, and
  finish in `GM_STAGE_SELECT` (main mode 1, submode 9).
- **Fixture limitation:** RAM positioning places Wario at `(424,170)` above the
  switch after normal boot, and later at `(504,264)` in the vortex after movement
  and countdown have been observed. Only Wario's two position fields are changed.
  The switch, timer, collision, movement and exit code run normally. This verifies
  activation and exit handling separately; it is **not** an uninterrupted input-only
  traversal from the switch through the intervening barrier, nor a full M3 playthrough.
  Several preserved attempts at that short route stop at ordinary terrain. No second
  collision bug is claimed. Manual traversal remains useful follow-up.
- Fresh `build/v7-24` and final `build/v7-25` print `ROM-SAME` and `OBJ-SAME`, MD5
  `940b597bef3be3f4041021b47a2f4096`. As previously documented, make check's base-ROM
  checksum failure is expected because unmatched functions remain. Next fresh build: v7-26.
- No new mGBA comparison is claimed for the positioned fixtures. Prior comparison
  windows, masks and limitations remain exactly as recorded in CODEX_HANDOVER.md.

## Artifacts and reproduction

Everything is preserved under ignored `build/frog-freeze-20261018/`.
Important files: `lldb-evidence.txt`, `lldb-sample2.txt`, `normal-before.exe`,
`exit_fixture2.py`, `escape-roll.keys`, `exit2-normal/asan.bin`, their `.shots`,
`.log` and `-report.txt` files, selected PNGs, `verify_fixture.py`, and normal/ASan
route logs. Earlier unsuccessful probes are retained too. `ARTIFACTS.txt` lists the
new directory; all earlier M3 artifact directories and input scripts remain intact.

From PowerShell, with the ASan runtime DLL directory on PATH for the second run,
choose NEW output tags (the probe creates files under those names):

```
python build/frog-freeze-20261018/exit_fixture2.py NEW-NORMAL build_pc/wl4.exe
python build/frog-freeze-20261018/exit_fixture2.py NEW-ASAN build_pc_asan/wl4.exe
```

These are Windows diagnostic helpers, not a new game feature or saved checkpoint.
They use Read/WriteProcessMemory and suspend/resume the test process for positioning.
No existing save is loaded or overwritten. All diagnostic game/debugger processes
were closed after verification. Normal and ASan rebuilt executables are ready for
manual playtesting. General route extension remains paused outside this bug task.

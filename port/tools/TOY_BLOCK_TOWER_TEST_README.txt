Toy Block Tower crash-fix test build (private, Windows x64)

Double-click Play-Toy-Block-Tower-Test.cmd. It looks for your supported US ROM
at C:\Users\Benny\Decomp\warioland4-2\baserom_us.gba. If yours is elsewhere,
drag the ROM file onto the .cmd instead. No ROM is included in this folder.

On its first run the launcher copies your current wl4.sav and settings.cfg, if
present, into test-data. The game writes only to those test copies. Your regular
save, settings, Windows Beta 3 installation and ROM remain untouched. The test
folder is independent; any progress here will stay in its test-data save.

Please repeat: Toy Block Tower frog switch, collect Keyzer, enter the large red
pipe during escape, exit the room, and repeat the transition if practical.
After closing the game, the launcher displays the exit code. If it faults, send
the newest test-data\run-*.log and a screenshot of any memory-fault dialog.

This build fixes 32-bit GBA address wraparound in an environmental tile lookup.
The focused pre-fix test reproduced the exact faulting address; the full
Toy Block Tower route still needs this manual retest.

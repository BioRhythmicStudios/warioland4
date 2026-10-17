# usage (WSL, repo root): sh test.sh NAME  -> compile G/hop/NAME.c, build a playtest ROM with it, diff-test WarioHopInit
G=/mnt/c/Users/Benny/AppData/Local/Temp/claude/C--Users-Benny-Decomp-warioland4-2/e7f228ff-4d33-4d78-a856-ac7245da4e24/scratchpad
python3 $G/gen/run_trial.py minigames/wario_hop WarioHopInit $G/hop/$1.c 0 || exit 1
python3 tools/build_playtest_rom.py --asm minigames/wario_hop=build/gen_trials/WarioHopInit/$1.s --out build/playtest/trial_$1.gba | grep -v "^  " || exit 1
$G/emu_venv/bin/python tools/diff_test_functions.py WarioHopInit --runs ${2:-100} --rom-b build/playtest/trial_$1.gba

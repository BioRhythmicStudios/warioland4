G=/mnt/c/Users/Benny/AppData/Local/Temp/claude/C--Users-Benny-Decomp-warioland4-2/e7f228ff-4d33-4d78-a856-ac7245da4e24/scratchpad
python3 $G/hop/$1 $G && python3 $G/gen/run_trial.py minigames/wario_hop WarioHopInit $G/hop/$2.c 0 | grep -v warning
python3 $G/hop/sim.py $2
python3 $G/hop/hoist.py $2

#!/bin/bash
read -p "Enter run number: " run
read -p "Enter flight altitude: " alt
read -e -p "Enter Raw Data directory: " dir
if [[ -w JOB ]]; then
	rm JOB
fi
num_col=$(cat COL | wc -l)
echo $dir"/" >> JOB
echo "curr_run: "$run >> JOB
echo "freq_em: 1000" >> JOB
echo "pulse_ms: 20" >> JOB
echo "lin_scale: 1" >> JOB
echo "map_d: 128" >> JOB
echo "alpha_c_thres: 500" >> JOB
echo "num_col: "$num_col >> JOB
echo "f_drift: -3000" >> JOB
./spectrumAnalysis > /dev/null
./altFilter.py $alt RUN_+([0-9]).csv
./finalAnalysis > /dev/null
./spectraCollarID $num_col RUN_+([0-9]).csv META_+([0-9]).csv

#! /usr/bin/env bash
read -p "Enter run number: " run
read -p "Enter flight altitude: " alt
read -e -p "Enter Raw Data directory: " dir
dir=$(echo $dir | sed "s%~%$HOME%")
if [[ -w JOB ]]; then
	rm JOB
fi
if [ -e "COL" ]
then
	echo "Collar definitions found!"
else
	echo "Collar definitions not found!"
	exit 1
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
if [ -e "spectrumAnalysis" ]
then
	./spectrumAnalysis > /dev/null
else
	if [ -e "build-spectrumAnalysis" ]
	then
		./build-spectrumAnalysis
		./spectrumAnalysis > /dev/null
	else
		echo "Could not find ./spectrumAnalysis!"
	fi
fi
runFile=$(ls | grep -E RUN_[[:digit:]]\+$run.csv$)
./altFilter.py $alt $runFile
./finalAnalysis > /dev/null
metaFile=$(ls | grep -E META_[[:digit:]]\+$run.csv)
./spectraCollarID $num_col $runFile $metaFile




#! /usr/bin/env bash
read -p "Raw data path: "
echo $REPLY > JOB
read -p "Run number: "
echo "curr_run: $REPLY" >> JOB
echo "freq_em: 1000" >> JOB
echo "pulse_ms: 20" >> JOB
echo "lin_scale: 1" >> JOB
echo "map_d: 128" >> JOB
echo "alpha_c_thres: 500" >> JOB
read -p "Number of collars: "
echo "num_col: $REPLY" >> JOB
echo "f_drift: -3000" >> JOB
if [ -e "COL" ]
then
	echo "Collar definitions found!"
else
	echo "Collar definitions not found!"
	exit 1
fi
if [ -e "spectrumAnalysis" ]
then
	./spectrumAnalysis
else
	if [ -e "build-spectrumAnalysis" ]
	then
		./build-spectrumAnalysis
		./spectrumAnalysis
	else
		echo "Could not find ./spectrumAnalysis!"
	fi
fi


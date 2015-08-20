#!/bin/bash
OPTIND=1
run=-1
freq=172464000
gain="19.7"
output="/media/RAW_DATA/rct/"
sampling_freq=2048000
port="/dev/ttyACM0"

while getopts "r:f:g:o:s:p:" opt; do
	case $opt in
		r)
			run=$OPTARG
			;;
		f)
			freq=$OPTARG
			;;
		g)
			gain=$OPTARG
			;;
		o)
			output=$OPTARG
			;;
		s)
			sampling_freq=$OPTARG
			;;
		p)
			port=$OPTARG
			;;
	esac
done

if [[ "$run" -ne $run ]]; then
	echo "ERROR: Bad run number"
	exit 1
fi

if [[ $run -eq -1 ]]; then
	run=`./getRunNum.py $output`
fi

if [[ "$freq" -ne $freq ]]; then
	echo "ERROR: Bad frequency"
	exit 1
fi

if [[ "sampling_freq" -ne $sampling_freq ]]; then
	echo "ERROR: Bad sampling frequency"
	exit 1
fi

echo $output > gps_logger_args
echo "GPS_" >> gps_logger_args
echo "" >> gps_logger_args
echo $run >> gps_logger_args
echo $port >> gps_logger_args
gps_logger/gps_logger.py &
mavproxypid=$!

sdr_record/sdr_record -g $gain -s $sampling_freq -f $freq -r $run -o $output &
sdr_record_pid=$!
trap "kill -s SIGINT $mavproxypid; kill -s SIGINT $sdr_record_pid; sleep 1; rm gps_logger_args; exit 0" SIGINT SIGTERM
while :
do
	sleep 1
done

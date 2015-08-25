#!/bin/bash
OPTIND=1
run=-1
freq=172464000
gain="19.7"
output="/home/pi/rct/"
sampling_freq=2048000
port="/dev/ttyAMA0"

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
	run=`/home/pi/radio_collar_tracker/getRunNum.py $output`
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
/home/pi/radio_collar_tracker/gps_logger/gps_logger.py &
mavproxypid=$!

/home/pi/radio_collar_tracker/sdr_record/sdr_record -g $gain -s $sampling_freq -f $freq -r $run -o $output &
sdr_record_pid=$!

trap "echo 'got sigint'; /bin/kill -s SIGINT $mavproxypid; /bin/kill -s SIGINT $sdr_record_pid; sleep 1; rm gps_logger_args; exit 0" SIGINT SIGTERM
/home/pi/radio_collar_tracker/autostart/parser.sh $mavproxypid $sdr_record_pid
while :
do
	sleep 1
done

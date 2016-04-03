#!/bin/bash
OPTIND=1
run=-1
freq=172464000
gain="19.7"
output="/home/pi/rct/"
sampling_freq=2048000
port="/dev/ttyAMA0"
led_num=17
sdr_log="/home/pi/sdr_log.log"
gps_log="/home/pi/gps_log.log"


led_dir="/sys/class/gpio/gpio$led_num"
if [ ! -e $led_dir ]
then
	echo $led_num > /sys/class/gpio/export
fi

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
	run=`/home/pi/radio_collar_tracker_drone/getRunNum.py $output`
fi

if [[ "$freq" -ne $freq ]]; then
	echo "ERROR: Bad frequency"
	exit 1
fi

if [[ "sampling_freq" -ne $sampling_freq ]]; then
	echo "ERROR: Bad sampling frequency"
	exit 1
fi

/home/pi/radio_collar_tracker_drone/gps_logger/gps_logger.py -o $output -r $run -i $port &>> ${gps_log} &
mavproxypid=$!

/home/pi/radio_collar_tracker_drone/sdr_record/sdr_record -g $gain -s $sampling_freq -f $freq -r $run -o $output &>> ${sdr_log} &
sdr_record_pid=$!

trap "echo 'got sigint'; /bin/kill -s SIGINT $mavproxypid; /bin/kill -s SIGINT $sdr_record_pid; echo low > $led_dir/direction; sleep 1; rm gps_logger_args; exit 0" SIGINT SIGTERM
run=true
while $run
do
	sleep 1
	echo high > $led_dir/direction
	if ! ps -p $mavproxypid > /dev/null
	then
		run=false
	fi
	if ! ps -p $sdr_record_pid > /dev/null
	then
		run=false
	fi
done
echo low > $led_dir/direction
/bin/kill -s SIGINT $mavproxypid
/bin/kill -s SIGINT $sdr_record_pid

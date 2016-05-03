# -* bash *-
INSTALL_DIR=&INSTALL_PREFIX

source $INSTALL_DIR/etc/rct_config

led_dir="/sys/class/gpio/gpio$led_num"
if [ ! -e $led_dir ]
then
	echo $led_num > /sys/class/gpio/export
fi

sdr_log="$log_dir/rct_sdr_log.log"
gps_log="$log_dir/rct_gps_log.log"
run=-1

OPTIND=1
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

echo "SDR_STARTER starting"

if [[ "$run" -ne $run ]]; then
	echo "ERROR: Bad run number"
	exit 1
fi

if [[ $run -eq -1 ]]; then
	run=`rct_getRunNum.py $output_dir`
fi

if [[ "$freq" -ne $freq ]]; then
	echo "ERROR: Bad frequency"
	exit 1
fi

if [[ "sampling_freq" -ne $sampling_freq ]]; then
	echo "ERROR: Bad sampling frequency"
	exit 1
fi

rct_gps_logger.py -o $output_dir -r $run -i $mav_port &>> ${gps_log} &
mavproxypid=$!

sdr_record -g $gain -s $sampling_freq -f $freq -r $run -o $output_dir &>> ${sdr_log} &
sdr_record_pid=$!

trap "echo 'got sigint'; kill -s SIGINT $mavproxypid; kill -s SIGINT $sdr_record_pid; echo low > $led_dir/direction; sleep 1; exit 0" SIGINT SIGTERM
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
kill -s SIGINT $mavproxypid
kill -s SIGINT $sdr_record_pid

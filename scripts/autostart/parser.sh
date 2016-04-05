#!
# -* bash *-
### BEGIN INIT INFO
# Provides: rctstart
# Required-Start: $portmap $time $remote_fs
# Required-Stop:
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Start radio collar tracker payload on switch
### END INIT INFO
# setup here
timestamp() {
	date
}

log="/home/pi/rct.log"
switch_num="4"
led_num=17
led_dir="/sys/class/gpio/gpio$led_num"
switch_dir="/sys/class/gpio/gpio$switch_num"
keep_alive='/home/pi/radio_collar_tracker_drone/gps_logger/gps_keep_alive.py'
sdr_starter='/home/pi/radio_collar_tracker_drone/sdr_starter.sh'
port="/dev/ttyAMA0"

case "$1" in
	stop)
		echo "$(timestamp): Payload power off!" >> $log
		killall rctstart
		exit
		;;
esac

# start
echo "$(timestamp): Starting..." >> $log
# check for autostart file!
if [ ! -e "/home/pi/autostart" ]
	then
		echo "$(timestamp): Autostart not found!" >> $log
	exit
fi

if [ ! -e $led_dir ]
then
	echo $led_num > /sys/class/gpio/export
fi

if [ ! -e $switch_dir ]
then
	echo $switch_num > /sys/class/gpio/export
fi

stateVal="startWait"


while true
do
	case $stateVal in
		"startWait" )
		# State 1 - wait for start
			$keep_alive -i $port &>> /home/pi/gps_keep.log &
			keep_alive_pid=$!
			switchVal=`cat $switch_dir/value`
			until [ "$switchVal" = "0" ]; do
				sleep 3
				switchVal=`cat $switch_dir/value`
			done
			echo "$(timestamp): Received start signal!" >> $log
			/bin/kill -s INT ${keep_alive_pid}
			stateVal="startgo"
			;;
		"startgo" )
		# State 2 - go for start, initialize
			${sdr_starter} &
			sdr_starter_pid=$!
			echo "$(timestamp): Started program!" >> $log
			runNum=`/home/pi/radio_collar_tracker_drone/getRunNum.py -e /home/pi/rct/`
			echo "$(timestamp): $runNum" >> $log
			stateVal="endWait"
			;;
		"endWait" )
			switchVal=`cat $switch_dir/value`
			until [ "$switchVal" = "1" ]; do
				sleep 3
				switchVal=`cat $switch_dir/value`
			done
			echo "$(timestamp): Received stop signal!" >> $log
			stateVal="endgo"
			;;
		"endgo" )
			/bin/kill -s TERM ${sdr_starter_pid}
			# killall ct -s INT
			echo "$(timestamp): Ended program!" >> $log
			echo "$(timestamp): Begin dmesg dump" >> $log
			dmesg >> $log
			echo "$(timestamp): end dmesg dump" >> $log
			stateVal="startWait"
			;;
	esac
done

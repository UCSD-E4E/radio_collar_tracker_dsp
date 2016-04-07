#!/bin/sh
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

INSTALL_DIR=&INSTALL_PREFIX

source $INSTALL_DIR/etc/rct_config

log="$log_dir/rctstart_rct.log"
led_dir="/sys/class/gpio/gpio$led_num"
switch_dir="/sys/class/gpio/gpio$switch_num"
keep_alive='rct_gps_keep_alive.py'
sdr_starter='rct_sdr_starter.sh'

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
			$keep_alive -i $mav_port &>> $log_dir/gps_keep.log &
			keep_alive_pid=$!
			switchVal=`cat $switch_dir/value`
			until [ "$switchVal" = "0" ]; do
				sleep 3
				switchVal=`cat $switch_dir/value`
			done
			echo "$(timestamp): Received start signal!" >> $log
			kill -s INT ${keep_alive_pid}
			stateVal="startgo"
			;;
		"startgo" )
		# State 2 - go for start, initialize
			${sdr_starter} &
			sdr_starter_pid=$!
			echo "$(timestamp): Started program!" >> $log
			runNum=`rct_getRunNum.py -e $output_dir`
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
			kill -s TERM ${sdr_starter_pid}
			# killall ct -s INT
			echo "$(timestamp): Ended program!" >> $log
			echo "$(timestamp): Begin dmesg dump" >> $log
			dmesg >> $log
			echo "$(timestamp): end dmesg dump" >> $log
			stateVal="startWait"
			;;
	esac
done

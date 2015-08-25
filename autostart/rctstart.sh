#!/bin/sh
### BEGIN INIT INFO
# Required-Start: $portmap $time $remote_fs
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Start radio collar tracker payload on switch
### END INIT INFO
# setup here
switch_num="4"

switch_dir="/sys/class/gpio/gpio$switch_num"
timestamp(){
	date
}

log="/home/pi/rct.log"
# check for autostart file!
if [ ! -e "/home/pi/autostart" ]
	then
		echo "Autostart not found!"
	exit
fi

echo $switch_num > /sys/class/gpio/export
stateVal="startWait"
echo "$timestamp: Starting..." >> $log
while true
do
	case $stateVal in
		"startWait" )
		# State 1 - wait for start
		switchVal=`cat $switch_dir/value`
			until [ "$switchVal" = "0" ]; do
				sleep 3
				switchVal=`cat $switch_dir/value`
			done
			echo "$timestamp: Received start signal!" >> $log
			stateVal="startgo"
			;;
		"startgo" )
		# State 2 - go for start, initialize
			/home/pi/radio_collar_tracker/sdr_starter.sh &
			sdr_starter_pid=$!
			echo "$timestamp: Started program!" >> $log
			stateVal="endWait"
			;;
		"endWait" )
			switchVal=`cat $switch_dir/value`
			until [ "$switchVal" = "1" ]; do
				sleep 3
				switchVal=`cat $switch_dir/value`
			done
			echo "$timestamp: Received stop signal!" >> $log
			stateVal="endgo"
			;;
		"endgo" )
			/bin/kill -s TERM $sdr_starter_pid
			# killall ct -s INT
			echo "$timestamp: Ended program!" >> $log
			stateVal="startWait"
			;;
	esac
done

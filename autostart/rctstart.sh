#!/bin/sh
# setup here
switch_num="60"

switch_dir="/sys/class/gpio/gpio$switch_num"
timestamp(){
	date
}

log="/home/debian/rct.log"
# check for autostart file!
if [ ! -e ~/autostart ]
	then
	exit
fi

echo $switch_num > /sys/class/gpio/export
stateVal="startWait"
echo "$(timestamp): Starting..." >> $log
while true
do
	case $stateVal in
		"startWait" )
		# State 1 - wait for start
		switchVal=`cat $(switch_dir)/value`
			until [ "$switchVal" = "1" ]; do
				sleep 3
				switchVal=`cat $(switch_dir)/value`
			done
			echo "$(timestamp): Received start signal!" >> $log
			stateVal="startgo"
			;;
		"startgo" )
		# State 2 - go for start, initialize
			cpufreq-set -g performance >> $log 2>&1
			cpufreq-info >> $log 2>&1
			~/radio_collar/sdr_starter.sh
			sdr_starter_pid=$!
			echo "$(timestamp): Started program!" >> $log
			stateVal="endWait"
			;;
		"endWait" )
			switchVal=`cat $(switch_dir)/value`
			until [ "$switchVal" = "0" ]; do
				sleep 3
				switchVal=`cat $(switch_dir)/value`
			done
			echo "$(timestamp): Received stop signal!" >> $log
			stateVal="endgo"
			;;
		"endgo" )
			kill $(sdr_starter_pid) -s SIGINT
			# killall ct -s INT
			echo "$(timestamp): Ended program!" >> $log
			stateVal="startWait"
			;;
	esac
done

#!/bin/sh
# setup here
timestamp(){
	date
}

log="/home/debian/rct.log"
# check for autostart file!
if [ ! -e /home/debian/autostart ]
	then
	exit
fi

echo 30 > /sys/class/gpio/export
stateVal="startWait"
echo "$(timestamp): Starting..." >> $log
while true
do
	case $stateVal in
		"startWait" )
		# State 1 - wait for start
			switchVal=$(cat /sys/class/gpio/gpio30/value)
			until [ "$switchVal" = "1" ]; do
				sleep 3
				switchVal=$(cat /sys/class/gpio/gpio30/value)
			done
			echo "$(timestamp): Received start signal!" >> $log
			stateVal="startgo"
			;;
		"startgo" )
		# State 2 - go for start, initialize
			cpufreq-set -g performance >> $log 2>&1
			cpufreq-info >> $log 2>&1
			/home/debian/xcode/collarTracker > /home/debian/out.tmp &
			# /home/debian/ct > /home/debian/out.tmp &
			/home/debian/parser.sh < /home/debian/out.tmp >> $log 2>&1 &
			echo "$(timestamp): Started program!" >> $log
			stateVal="endWait"
			;;
		"endWait" )
			switchVal=$(cat /sys/class/gpio/gpio30/value)
			until [ "$switchVal" = "0" ]; do
				sleep 3
				switchVal=$(cat /sys/class/gpio/gpio30/value)
			done
			echo "$(timestamp): Received stop signal!" >> $log
			stateVal="endgo"
			;;
		"endgo" )
			killall collarTracker -s INT
			# killall ct -s INT
			echo "$(timestamp): Ended program!" >> $log
			stateVal="startWait"
			;;
	esac
done

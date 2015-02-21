#!/bin/sh
# setup here
# check for autostart file!
if [ ! -e /home/debian/autostart ]
	then
	exit
fi

echo 30 > /sys/class/gpio/export

stateVal="startWait"
echo "Starting..."
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
			echo "Received start signal!"
			stateVal="startgo"
			;;
		"startgo" )
		# State 2 - go for start, initialize
			#/home/debian/gpio/main > /dev/null &
			/home/debian/xcode/run | /home/debian/parser.sh >> /home/debian/rct.log 2>&1 &
			pid=$!
			echo "Started program!"
			stateVal="endWait"
			;;
		"endWait" )
			switchVal=$(cat /sys/class/gpio/gpio30/value)
			until [ "$switchVal" = "0" ]; do
				sleep 3
				switchVal=$(cat /sys/class/gpio/gpio30/value)
			done
			echo "Received stop signal!"
			stateVal="endgo"
			;;
		"endgo" )
			kill %1
			echo "Ended program!"
			stateVal="startWait"
	esac
done
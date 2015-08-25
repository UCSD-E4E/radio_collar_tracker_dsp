#!/bin/bash
mavproxypid=$1
sdr_starterpid=$2

led_num="60"

led_dir="/sys/class/gpio/gpio$led_num"
if [ ! -e $led_dir ]
    then
    echo $led_num > /sys/class/gpio/export
fi
run=true
while $run; do
	echo high > $(led_dir)/direction
	sleep 0.5
	if ! ps -p $mavproxypid > /dev/null
	then
		run=false
	fi
	if ! ps -p $sdr_starterpid > /dev/null
	then
		run=false
	fi
done
echo low > $(led_dir)/direction

#!/bin/bash
if [ ! -e /sys/class/gpio/gpio60 ]
    then
    echo 60 > /sys/class/gpio/export
fi
while true; do
	read -t 5 line
    echo $line
	retval=$?
	if [ "$retval" = "142" ]
        then	# TIMEOUT!
        echo "Error! TIMEOUT!"
		echo high > /sys/class/gpio/gpio60/direction
		exit 0
		#reboot
	fi
	if [ "$retval" = "1" ]
        then	# EOF reached.  Normal end
		echo high > /sys/class/gpio/gpio60/direction
		exit 0
	fi
	if [ "$retval" = "0" ]
        then
		if [ "$(echo $line | grep FILE | wc -l)" != "0" ]
            then
			echo low > /sys/class/gpio/gpio60/direction
		fi
		if [ "$(echo $line | grep SIGINT | wc -l)" = "1" ]
            then
			echo high > /sys/class/gpio/gpio60/direction
			exit 0
		fi
		if [ "$(echo $line | grep GPS | wc -l)" = "1" ]
            then
			echo low > /sys/class/gpio/gpio60/direction
		fi
	fi
done
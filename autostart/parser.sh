#!/bin/bash
if [ ! -e /sys/class/gpio/gpio60 ]
    then
    echo 60 > /sys/class/gpio/export
fi
run=true
while $run; do
	sleep 0.5
	read line
	retval=$?
	if [ "$retval" = "1" ]
        then	# EOF reached.  Normal end
        # echo "Found EOF, waiting..."
        sleep 2
        read line
        retval=$?
        if [ "$retval" = "1" ]
        	then	# EOF reached, no change for 2 seconds!
	        echo "Error! TIMEOUT! REBOOT!"
			echo low > /sys/class/gpio/gpio60/direction
			run=false;
			killall collarTracker -s INT
			# reboot
		fi
	fi
	if [ "$retval" = "0" ]
		echo $line
        then
		if [ "$(echo $line | grep Frame | wc -l)" != "0" ]
            then
            # echo "Got Frame!"
			echo high > /sys/class/gpio/gpio60/direction
		fi
		if [ "$(echo $line | grep SIGINT | wc -l)" = "1" ]
            then
            # echo "Got SIGINT!, exiting..."
			echo low > /sys/class/gpio/gpio60/direction
			run=false;
		fi
		if [ "$(echo $line | grep GPS | wc -l)" = "1" ]
            then
            # echo "Got GPS!"
			echo high > /sys/class/gpio/gpio60/direction
		fi
	fi
done
echo low > /sys/class/gpio/gpio60/direction

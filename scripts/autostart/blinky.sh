#!/bin/bash
OPTIND=1

while getopts "l:i:" opt
do
	case $opt in
		h|\?)
			exit
			;;
		l)
			LED_NUM=$OPTARG
			if [[ $LED_NUM =~ '^[0-9]+$' ]]
			then
				echo "Bad Number!"
				exit 1
			fi
			;;
		i)
			LED_ITVAL_S=$OPTARG
			if [[ $LED_NUM =~ '^(0).[0-9]+$' ]]
			then
				echo "Bad sleep interval!  Must be a floating point number less than 1"
				exit 1
			fi
			;;
	esac
done

# Generated vars
LED_DIR="/sys/class/gpio/gpio${LED_NUM}"
if [ ! -e $LED_DIR ]
then
	echo $LED_NUM > /sys/class/gpio/export
fi

while [ 0 ]
do
	sleep ${LED_ITVAL_S}
	echo high > $LED_DIR/direction
	sleep ${LED_ITVAL_S}
	echo low > $LED_DIR/direction
done

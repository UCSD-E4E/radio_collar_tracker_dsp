echo GPS and SDR operating!
sleep 1
echo currentRun: 1
for a in 1 2 3 4 5 6
do
	echo 'Got message HIGHRES_IMU (spec: https://pixhawk.ethz.ch/mavlink/#HIGHRES_IMU)'
	echo '    time: 123456789'
	echo '    acc: 123456789'
	echo '    mag: 123456789'
	echo '    temp: 123456789'
	echo ''
	echo 'Current Frame: %03d Gain: %03d'
	echo 'FILE: %06d'
	sleep 1
done
sleep 15

echo got SIGINT!
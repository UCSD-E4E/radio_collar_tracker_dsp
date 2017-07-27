#!/usr/bin/env python
import subprocess
import time
from enum import Enum
import threading
import os
import signal

class SDR_INIT_STATES(Enum):
	find_devices = 0
	wait_recycle = 1
	usrp_probe = 2

devnull = open(os.devnull, 'w')
thread_op = True

def init_SDR():
	global thread_op
	init_sdr_state = SDR_INIT_STATES.find_devices
	while thread_op:
		if init_sdr_state == SDR_INIT_STATES.find_devices:
			uhd_find_dev_retval = subprocess.call(['/usr/local/bin/uhd_find_devices', '--args=\"type=b200\"'], stdout=devnull, stderr=devnull)
			if uhd_find_dev_retval == 0:
				init_sdr_state = SDR_INIT_STATES.usrp_probe
			else:
				init_sdr_state = SDR_INIT_STATES.wait_recycle
		elif init_sdr_state == SDR_INIT_STATES.wait_recycle:
			time.sleep(1)
			init_sdr_state = SDR_INIT_STATES.find_devices
		elif init_sdr_state == SDR_INIT_STATES.usrp_probe:
			uhd_usrp_probe_retval = subprocess.call(['/usr/local/bin/uhd_usrp_probe', '--args=\"type=b200\"', '--init-only'], stdout=devnull, stderr=devnull)
			if uhd_usrp_probe_retval == 0:
				return 0
			else:
				return 1
	return 1

def sigint_handler(signal, frame):
	print("REceived sig")
	global thread_op
	thread_op = False

def main():
	signal.signal(signal.SIGINT, sigint_handler)
	signal.signal(signal.SIGTERM, sigint_handler)
	init_SDR_thread = threading.Thread(target=init_SDR)
	init_SDR_thread.start()
	# init_SDR_thread.join()
	signal.pause()

if __name__ == '__main__':
	main()
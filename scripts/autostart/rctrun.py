#!/usr/bin/env python
import subprocess
import time
from enum import Enum
import threading
import os
import signal
import serial
import pynmea2
import mmap
# import mraa

WAIT_COUNT = 60

devnull = open(os.devnull, 'w')
thread_op = True
mmap_file = None
shared_states = None

def get_var(var):
	var_file = open('/usr/local/etc/rct_config')
	for line in var_file:
		if line.split('=')[0] == var:
			value = line.split('=')[1]
			return value.strip().strip('"').strip("'")
	return None

class SDR_INIT_STATES(Enum):
	find_devices = 0
	wait_recycle = 1
	usrp_probe = 2
	rdy = 3
	fail = 4

def init_SDR():
	global thread_op
	init_sdr_state = SDR_INIT_STATES.find_devices
	while thread_op:
		shared_states[0] = str(unichr(init_sdr_state.value))
		if init_sdr_state == SDR_INIT_STATES.find_devices:
			uhd_find_dev_retval = subprocess.call(['/usr/local/bin/uhd_find_devices', '--args=\"type=b200\"'], stdout=devnull, stderr=devnull)
			if uhd_find_dev_retval == 0:
				init_sdr_state = SDR_INIT_STATES.usrp_probe
			else:
				init_sdr_state = SDR_INIT_STATES.fail
		elif init_sdr_state == SDR_INIT_STATES.wait_recycle:
			time.sleep(1)
			init_sdr_state = SDR_INIT_STATES.find_devices
		elif init_sdr_state == SDR_INIT_STATES.usrp_probe:
			uhd_usrp_probe_retval = subprocess.call(['/usr/local/bin/uhd_usrp_probe', '--args=\"type=b200\"', '--init-only'], stdout=devnull, stderr=devnull)
			if uhd_usrp_probe_retval == 0:
				init_sdr_state = SDR_INIT_STATES.rdy
			else:
				init_sdr_state = SDR_INIT_STATES.fail
	return 0

class OUTPUT_DIR_STATES(Enum):
	get_output_dir = 0
	check_output_dir = 1
	check_space = 2
	wait_recycle = 3
	rdy = 4
	fail = 5

def init_output_dir():
	global thread_op
	init_output_dir_state = OUTPUT_DIR_STATES.get_output_dir
	counter = 0
	while thread_op:
		shared_states[1] = str(unichr(init_output_dir_state.value))
		if init_output_dir_state == OUTPUT_DIR_STATES.get_output_dir:
			output_dir = get_var('output_dir')
			init_output_dir_state = OUTPUT_DIR_STATES.check_output_dir
		elif init_output_dir_state == OUTPUT_DIR_STATES.check_output_dir:
			if os.path.isdir(output_dir) and os.path.isfile(os.path.join(output_dir, 'fileCount')):
				init_output_dir_state = OUTPUT_DIR_STATES.check_space
			else:
				init_output_dir_state = OUTPUT_DIR_STATES.fail
		elif init_output_dir_state == OUTPUT_DIR_STATES.check_space:
			df = subprocess.Popen(['df', output_dir], stdout=subprocess.PIPE)
			output = df.communicate()[0]
			device, size, used, available, percent, mountpoint = output.split('\n')[1].split()
			if available > 20 * 60 * 2000000 * 4:
				# enough space
				init_output_dir_state = OUTPUT_DIR_STATES.rdy
			else:
				init_output_dir_state = OUTPUT_DIR_STATES.wait_recycle
		elif init_output_dir_state == OUTPUT_DIR_STATES.wait_recycle:
			time.sleep(1)
			counter = counter + 1
			if counter > WAIT_COUNT:
				init_output_dir_state = OUTPUT_DIR_STATES.fail
			else:
				init_output_dir_state = OUTPUT_DIR_STATES.check_output_dir
	return 0

class GPS_STATES(Enum):
	get_tty = 0
	get_msg = 1
	wait_recycle = 2
	rdy = 3
	fail = 4

def accept_gps(msg):
	if msg.gps_qual == 0:
		return False
	if msg.gps_qual == 7:
		return False
	if msg.gps_qual == 8:
		return False
	if msg.num_sats < 6:
		return False
	return True

def init_gps():
	global thread_op
	init_gps_state = GPS_STATES.get_tty
	counter = 0
	msg_counter = 0
	while thread_op:
		shared_states[2] = str(unichr(init_gps_state.value))
		if init_gps_state == GPS_STATES.get_tty:
			tty_device = get_var('gps_port')
			tty_baud = get_var('gps_baud')
			try:
				tty_stream = serial.Serial(tty_device, tty_baud, timeout = 1)
			except serial.SerialException, e:
				init_gps_state = GPS_STATES.fail
			init_gps_state = GPS_STATES.get_msg

		elif init_gps_state == GPS_STATES.get_msg:
			try:
				line = tty_stream.readline()
			except serial.serialutil.SerialException, e:
				init_gps_state = GPS_STATES.fail
				continue
			if line is not None:
				msg = None
				try:
					msg = pynmea2.parse(line)
				except pynmea2.ParseError, e:
					init_gps_state = GPS_STATES.fail
					continue
				if msg.sentence_type == 'GGA':
					if accept_gps(msg):
						# good GPS
						init_gps_state = GPS_STATES.rdy
					else:
						init_gps_state = GPS_STATES.get_msg
				else:
					msg_counter = msg_counter + 1
					if msg_counter > 20:
						init_gps_state = GPS_STATES.fail
					else:
						init_gps_state = GPS_STATES.get_msg
			else:
				init_gps_state = GPS_STATES.get_msg

		elif init_gps_state == GPS_STATES.wait_recycle:
			time.sleep(1)
			if counter > WAIT_COUNT / 2:
				init_gps_state = GPS_STATES.fail
			else:
				init_gps_state = GPS_STATES.get_msg
	return 0

def sigint_handler(signal, frame):
	print("Received sig")
	global thread_op
	thread_op = False

def main():
	# Set up mmap files
	if not os.path.isdir('/var/local/rct'):
		os.makedirs('/var/local/rct')
	mmap_file = open('/var/local/rct/status.dat', 'w+b')
	mmap_file.write('\0\0\0')
	mmap_file.flush()
	global shared_states
	shared_states = mmap.mmap(mmap_file.fileno(), 3)
	signal.signal(signal.SIGINT, sigint_handler)
	signal.signal(signal.SIGTERM, sigint_handler)
	init_SDR_thread = threading.Thread(target=init_SDR)
	init_output_thread = threading.Thread(target=init_output_dir)
	init_gps_thread = threading.Thread(target=init_gps)
	init_SDR_thread.start()
	init_output_thread.start()
	init_gps_thread.start()
	signal.pause()
	init_SDR_thread.join()
	init_output_thread.join()
	init_gps_thread.join()
	shared_states.close()
	mmap_file.close()

if __name__ == '__main__':
	main()
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
import sys
import mraa

WAIT_COUNT = 60
SW1_PIN = 61

devnull = None
init_thread_op = True
master_thread_op = True
mmap_file = None
shared_states = None

class SDR_INIT_STATES(Enum):
	find_devices = 0
	wait_recycle = 1
	usrp_probe = 2
	rdy = 3
	fail = 4

class GPS_STATES(Enum):
	get_tty = 0
	get_msg = 1
	wait_recycle = 2
	rdy = 3
	fail = 4

class OUTPUT_DIR_STATES(Enum):
	get_output_dir = 0
	check_output_dir = 1
	check_space = 2
	wait_recycle = 3
	rdy = 4
	fail = 5

class RCT_STATES(Enum):
	init		=	0
	wait_init	=	1
	wait_start	=	2
	start		=	3
	wait_end	=	4
	finish		=	5
	fail		=	6

def get_var(var):
	var_file = open('&INSTALL_PREFIX/etc/rct_config')
	for line in var_file:
		if line.split('=')[0] == var:
			value = line.split('=')[1]
			return value.strip().strip('"').strip("'")
	return None

def init_SDR():
	global init_thread_op
	global shared_states
	init_sdr_state = SDR_INIT_STATES.find_devices
	while init_thread_op:
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
		elif init_sdr_state == SDR_INIT_STATES.fail:
			time.sleep(10)
			init_sdr_state = SDR_INIT_STATES.init
		else:
			time.sleep(1)
	return 0

def init_output_dir():
	global init_thread_op
	global shared_states
	init_output_dir_state = OUTPUT_DIR_STATES.get_output_dir
	counter = 0
	while init_thread_op:
		shared_states[1] = str(unichr(init_output_dir_state.value))
		if init_output_dir_state == OUTPUT_DIR_STATES.get_output_dir:
			output_dir = get_var('output_dir')
			init_output_dir_state = OUTPUT_DIR_STATES.check_output_dir
		elif init_output_dir_state == OUTPUT_DIR_STATES.check_output_dir:
			if os.path.isdir(output_dir) and os.path.isfile(os.path.join(output_dir, 'fileCount')):
				init_output_dir_state = OUTPUT_DIR_STATES.check_space
			else:
				init_output_dir_state = OUTPUT_DIR_STATES.wait_recycle
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
		elif init_output_dir_state == OUTPUT_DIR_STATES.fail:
			time.sleep(10)
			init_output_dir_state = OUTPUT_DIR_STATES.get_output_dir
		else:
			time.sleep(1)
	return 0

def accept_gps(msg):
	return True
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
	global init_thread_op
	global shared_states
	init_gps_state = GPS_STATES.get_tty
	counter = 0
	msg_counter = 0
	tty_stream = None
	while init_thread_op:
		shared_states[2] = str(unichr(init_gps_state.value))
		if init_gps_state == GPS_STATES.get_tty:
			tty_device = get_var('gps_port')
			tty_baud = get_var('gps_baud')
			try:
				tty_stream = serial.Serial(tty_device, tty_baud, timeout = 1)
			except serial.SerialException, e:
				init_gps_state = GPS_STATES.fail
				print("GPS fail: bad serial!")
				continue
			if tty_stream is None:
				init_gps_state = GPS_STATES.fail
				print("GPS fail: no serial!")
				continue
			else:
				init_gps_state = GPS_STATES.get_msg

		elif init_gps_state == GPS_STATES.get_msg:
			try:
				line = tty_stream.readline()
			except serial.serialutil.SerialException, e:
				init_gps_state = GPS_STATES.fail
				print("GPS fail: no serial!")
				continue
			if line is not None:
				msg = None
				try:
					msg = pynmea2.parse(line)
				except pynmea2.ParseError, e:
					init_gps_state = GPS_STATES.fail
					print("GPS fail: bad NMEA!")
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
						print("GPS fail: no GGA message!")
						continue
					else:
						init_gps_state = GPS_STATES.get_msg
			else:
				init_gps_state = GPS_STATES.get_msg

		elif init_gps_state == GPS_STATES.wait_recycle:
			time.sleep(1)
			if counter > WAIT_COUNT / 2:
				init_gps_state = GPS_STATES.fail
				print("GPS fail: bad state!")
				continue
			else:
				init_gps_state = GPS_STATES.get_msg
		elif init_gps_state == GPS_STATES.fail:
			time.sleep(10)
			init_gps_state = GPS_STATES.get_output_dir
		else:
			time.sleep(1)
	return 0

def init_state_complete():
	global shared_states
	return SDR_INIT_STATES(ord(shared_states[0])) == SDR_INIT_STATES.rdy and OUTPUT_DIR_STATES(ord(shared_states[1])) == OUTPUT_DIR_STATES.rdy and GPS_STATES(ord(shared_states[2])) == GPS_STATES.rdy

def init_RCT():
	global master_thread_op
	global init_thread_op
	init_RCT_state = RCT_STATES.init
	if 'mraa' in sys.modules:
		switch_handle = mraa.Gpio(SW1_PIN)
		switch_handle.dir(mraa.DIR_IN)
	else:
		switch_file = open('switch', 'a+')
		switch_file.write('0')
		switch_file.flush()
		switch_handle = mmap.mmap(switch_file.fileno(), 1)
	while master_thread_op:
		shared_states[3] = str(unichr(init_RCT_state.value))
		if init_RCT_state == RCT_STATES.init:
			init_SDR_thread = threading.Thread(target=init_SDR)
			init_output_thread = threading.Thread(target=init_output_dir)
			init_gps_thread = threading.Thread(target=init_gps)
			init_SDR_thread.start()
			init_output_thread.start()
			init_gps_thread.start()
			init_RCT_state = RCT_STATES.wait_init
		elif init_RCT_state == RCT_STATES.wait_init:
			# wait for init state machines to complete
			time.sleep(1)
			init_state = init_state_complete()
			if init_state:
				init_RCT_state = RCT_STATES.wait_start
			else:
				init_RCT_state = RCT_STATES.wait_init
		elif init_RCT_state == RCT_STATES.wait_start:
			time.sleep(1)
			if 'mraa' in sys.modules:
				switch_state = switch_handle.read()
			else:
				switch_state = switch_handle[0]
			if switch_state:
				init_RCT_state = RCT_STATES.start
				init_thread_op = False
				init_SDR_thread.join()
				init_output_thread.join()
				init_gps_thread.join()
			else:
				init_RCT_state = RCT_STATES.wait_start
		elif init_RCT_state == RCT_STATES.start:
			# sdr_starter = subprocess.Popen(['sdr_starter'])
			sdr_starter = subprocess.Popen(['&INSTALL_PREFIX/bin/rct_sdr_starter'])
			init_RCT_state = RCT_STATES.wait_end
		elif init_RCT_state == RCT_STATES.wait_end:
			time.sleep(3)
			if 'mraa' in sys.modules:
				switch_state = switch_handle.read()
			else:
				switch_state = switch_handle[0]
			if switch_state:
				init_RCT_state = RCT_STATES.wait_end
			else:
				init_RCT_state = RCT_STATES.finish
		elif init_RCT_state == RCT_STATES.finish:
			sdr_starter.terminate()
			rct_retval = sdr_starter.wait()
			if not rct_retval:
				init_RCT_state = RCT_STATES.fail
			subprocess.call(['sync'])
			init_RCT_state = RCT_STATES.init
		pass
	if 'mraa' not in sys.modules:
		switch_file.close()
		switch_handle.close()
	init_SDR_thread.join()
	init_output_thread.join()
	init_gps_thread.join()

def sigint_handler(signal, frame):
	print("Received sig")
	global init_thread_op
	global master_thread_op
	init_thread_op = False
	master_thread_op = False

def main():
	# Check for autostart
	autostart_flag = get_var('autostart')
	if autostart_flag != 'true':
		return
	# Set up mmap files
	global shared_states
	if not os.path.isdir('/var/local/rct'):
		os.makedirs('/var/local/rct')
	mmap_file = open('/var/local/rct/status.dat', 'w+b')
	mmap_file.write('\0' * 4)
	mmap_file.flush()
	shared_states = mmap.mmap(mmap_file.fileno(), 4)
	rct_blink = subprocess.Popen(['&INSTALL_PREFIX/bin/rct_blink'])

	devnull = open(os.devnull, 'w')

	signal.signal(signal.SIGINT, sigint_handler)
	signal.signal(signal.SIGTERM, sigint_handler)

	init_RCT_thread = threading.Thread(target=init_RCT)
	init_RCT_thread.start()

	signal.pause()

	init_RCT_thread.join()
	rct_blink.terminate()
	rct_blink.wait()

	shared_states.close()
	mmap_file.close()
	devnull.close()

if __name__ == '__main__':
	main()
#!/usr/bin/python3
import subprocess
import time
from enum import Enum
import threading
import os
import signal
import serial
import json
import mmap
import sys
from rct_udp_command import CommandListener
import sys
import shlex
import argparse
import datetime
import sys

WAIT_COUNT = 60

init_thread_op = True
run = True
mmap_file = None
shared_states = None
devnull = None
cmdListener = None
output_dir = None

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
		if line.split('=')[0].strip() == var:
			value = line.split('=')[1]
			return value.strip().strip('"').strip("'")
	return None

def init_SDR(test = False):
	global init_thread_op
	global shared_states
	init_sdr_state = SDR_INIT_STATES.find_devices
	while init_thread_op:
		shared_states[0] = init_sdr_state.value
		if init_sdr_state == SDR_INIT_STATES.find_devices:
			if not test:
				uhd_find_dev_retval = subprocess.call(['/usr/local/bin/uhd_find_devices', '--args=\"type=b200\"'], stdout=devnull, stderr=devnull)
				if uhd_find_dev_retval == 0:
					init_sdr_state = SDR_INIT_STATES.usrp_probe
				else:
					init_sdr_state = SDR_INIT_STATES.fail
			else:
				time.sleep(1)
				init_sdr_state = SDR_INIT_STATES.usrp_probe
		elif init_sdr_state == SDR_INIT_STATES.wait_recycle:
			time.sleep(1)
			init_sdr_state = SDR_INIT_STATES.find_devices
		elif init_sdr_state == SDR_INIT_STATES.usrp_probe:
			if not test:
				uhd_usrp_probe_retval = subprocess.call(['/usr/local/bin/uhd_usrp_probe', '--args=\"type=b200\"', '--init-only'], stdout=devnull, stderr=devnull)
				if uhd_usrp_probe_retval == 0:
					init_sdr_state = SDR_INIT_STATES.rdy
				else:
					init_sdr_state = SDR_INIT_STATES.fail
			else:
				time.sleep(1)
				init_sdr_state = SDR_INIT_STATES.rdy
		elif init_sdr_state == SDR_INIT_STATES.fail:
			time.sleep(10)
			init_sdr_state = SDR_INIT_STATES.find_devices
		else:
			time.sleep(1)
	return 0

def init_output_dir(test = False):
	global init_thread_op
	global shared_states
	global output_dir
	init_output_dir_state = OUTPUT_DIR_STATES.get_output_dir
	counter = 0
	while init_thread_op:
		shared_states[1] = init_output_dir_state.value
		if init_output_dir_state == OUTPUT_DIR_STATES.get_output_dir:
			output_dir = get_var('output_dir')
			if output_dir is None:
				init_output_dir_state = OUTPUT_DIR_STATES.fail
			else:
				init_output_dir_state = OUTPUT_DIR_STATES.check_output_dir
			if test:
				output_dir = '/tmp/'
				if not os.path.isfile('/tmp/LAST_RUN.TXT'):
					with open('/tmp/LAST_RUN.TXT', 'w') as lastrun:
						lastrun.write('1')
		elif init_output_dir_state == OUTPUT_DIR_STATES.check_output_dir:
			if os.path.isdir(output_dir) and os.path.isfile(os.path.join(output_dir, 'LAST_RUN.TXT')):
				init_output_dir_state = OUTPUT_DIR_STATES.check_space
			else:
				init_output_dir_state = OUTPUT_DIR_STATES.wait_recycle
		elif init_output_dir_state == OUTPUT_DIR_STATES.check_space:
			if not test:
				df = subprocess.Popen(['df', '-B1', output_dir], stdout=subprocess.PIPE)
				output = df.communicate()[0].decode('utf-8')
				device, size, used, available, percent, mountpoint = output.split('\n')[1].split()
				if int(available) > 20 * 60 * 1500000 * 4:
					# enough space
					init_output_dir_state = OUTPUT_DIR_STATES.rdy
				else:
					init_output_dir_state = OUTPUT_DIR_STATES.wait_recycle
			else:
				init_output_dir_state = OUTPUT_DIR_STATES.rdy
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
	if msg.gps_qual == 0:
		return False
	if msg.gps_qual == 7:
		return False
	if msg.gps_qual == 8:
		return False
	if msg.num_sats < 6:
		return False
	return True

def init_gps(test=False):
	global init_thread_op
	global run
	global shared_states
	init_gps_state = GPS_STATES.get_tty
	counter = 0
	msg_counter = 0
	tty_stream = None

	prev_gps = 0

	while init_thread_op:
		shared_states[2] = init_gps_state.value
		if init_gps_state == GPS_STATES.get_tty:
			tty_device = get_var('gps_target')
			tty_baud = get_var('gps_baud')
			if not test:
				try:
					tty_stream = serial.Serial(tty_device, tty_baud, timeout = 1)
				except serial.SerialException as e:
					init_gps_state = GPS_STATES.fail
					print("GPS fail: bad serial!")
					print(e)
					continue
				if tty_stream is None:
					init_gps_state = GPS_STATES.fail
					print("GPS fail: no serial!")
					continue
				else:
					init_gps_state = GPS_STATES.get_msg
			else:
				init_gps_state = GPS_STATES.get_msg

		elif init_gps_state == GPS_STATES.get_msg:
			if not test:
				try:
					line = tty_stream.readline().decode("utf-8")
				except serial.serialutil.SerialException as e:
					init_gps_state = GPS_STATES.fail
					print("GPS fail: no serial!")
					continue
				if line is not None and line != "":
					msg = None
					try:
						msg = json.loads(line)
						init_gps_state = GPS_STATES.rdy
					except json.JSONDecodeError as e:
						init_gps_state = GPS_STATES.fail
						print("GPS fail: bad message!")
						init_gps_state = GPS_STATES.get_msg
						continue
				else:
					init_gps_state = GPS_STATES.get_msg
			else:
				time.sleep(1)
				init_gps_state = GPS_STATES.rdy
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
			init_gps_state = GPS_STATES.get_tty
		else: # init_gps_state = GPS_STATES.rdy
			if not test:
				try:
					line = tty_stream.readline().decode("utf-8")
				except serial.serialutil.SerialException as e:
					init_gps_state = GPS_STATES.fail
					print("GPS fail: no serial!")
					continue
				if line is not None and line != "":
					msg = None
					try:
						msg = json.loads(line)
					except json.JSONDecodeError as e:
						init_gps_state = GPS_STATES.fail
						print("GPS fail: bad message!")
						print(e)
						init_gps_state = GPS_STATES.get_msg
						continue
			else:
				time.sleep(1)
			current_gps = datetime.datetime.now()
			if prev_gps == 0:
				continue
			else:
				if (current_gps - prev_gps).total_seconds() > 5:
					init_gps_state = GPS_STATES.wait_recycle
			prev_gps = current_gps
	return 0

def init_state_complete():
	global shared_states
	return SDR_INIT_STATES(shared_states[0]) == SDR_INIT_STATES.rdy and \
		OUTPUT_DIR_STATES(shared_states[1]) == OUTPUT_DIR_STATES.rdy and \
		GPS_STATES(shared_states[2]) == GPS_STATES.rdy

def init_RCT(test = False):
	global run
	global init_thread_op
	global cmdListener
	init_RCT_state = RCT_STATES.init
	
	while run:
		shared_states[3] = init_RCT_state.value
		if init_RCT_state == RCT_STATES.init:
			init_SDR_thread = threading.Thread(target=init_SDR, kwargs={'test':test})
			init_output_thread = threading.Thread(target=init_output_dir, kwargs={'test':test})
			init_gps_thread = threading.Thread(target=init_gps, kwargs={'test':test})
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
			switch_state = shared_states[4]
			if switch_state:
				print("Got start flag!, running")
				init_RCT_state = RCT_STATES.start
				init_thread_op = False
				init_SDR_thread.join()
				init_output_thread.join()
				init_gps_thread.join()
			else:
				init_RCT_state = RCT_STATES.wait_start
		elif init_RCT_state == RCT_STATES.start:
			print("Starting drone_run")
			if not test:
				with open(os.path.join(output_dir, 'LAST_RUN.TXT')) as lastrun:
					last_run = int(lastrun.readline().strip())
				with open(os.path.join(output_dir, 'LAST_RUN.TXT'), 'w') as lastrun:
					lastrun.write(str(last_run + 1))
				run_num = last_run + 1
			else:
				run_num = 8
			run_dir = os.path.join(output_dir, 'RUN_%06d' % (run_num))
			if not test:
				os.makedirs(run_dir)
			else:
				try:
					os.makedirs(run_dir)
				except:
					pass

			localize_file = os.path.join(run_dir, 'LOCALIZE_%06d' % (run_num))
			with open(localize_file, 'w') as file:
				file.write("")
				
			cmdListener.setRun(run_dir, run_num)
			time.sleep(1)

			sampling_freq = int(get_var('sampling_freq'))
			center_freq = int(get_var('center_freq'))

			if not test:
				sdr_record_cmd = ('sdr_record -g 22.0 -s %d -c %d'
					' -r %d -o %s | tee -a /var/log/rtt.log' % (sampling_freq, center_freq, run_num, run_dir))
			else:
				sdr_record_cmd = ('sdr_record -g 22.0 '
									'-s %d ' % (sampling_freq) +
									'-c %d ' % (center_freq) +
									'-r %d ' % (run_num) +
									'-o %s ' % (run_dir) +
									'--test_config ' +
									'--test_data /media/ntlhui/FA56-CFCD/2019.05.05/RUN_000008'
									'| tee -a /var/log/rtt.log')

			sdr_record = subprocess.Popen(sdr_record_cmd, shell=True)
			
			init_RCT_state = RCT_STATES.wait_end
		elif init_RCT_state == RCT_STATES.wait_end:
			time.sleep(1)
			switch_state = shared_states[4]
			if switch_state:
				init_RCT_state = RCT_STATES.wait_end
			else:
				init_RCT_state = RCT_STATES.finish
				print('Got stop flag!, stopping')
			if sdr_record.poll() != None:
				init_RCT_state = RCT_STATES.fail
				print('SDR Record failed!')
		elif init_RCT_state == RCT_STATES.finish:
			sdr_record.send_signal(2)
			rct_retval = sdr_record.wait()
			if not rct_retval:
				init_RCT_state = RCT_STATES.fail
			subprocess.call(['sync'])
			init_RCT_state = RCT_STATES.init
		elif init_RCT_state == RCT_STATES.fail:
			switch_state = shared_states[4]
			if switch_state:
				init_RCT_state = RCT_STATES.fail
			else:
				init_RCT_state = RCT_STATES.init

	init_SDR_thread.join()
	init_output_thread.join()
	init_gps_thread.join()

def sigint_handler(signal, frame):
	print("Received sig")
	global init_thread_op
	global run
	init_thread_op = False
	run = False

def main():

	parser = argparse.ArgumentParser(description='RCT Boostrapper')
	parser.add_argument('--autostart', action='store_true')
	parser.add_argument('--test', action='store_true')

	args = parser.parse_args()

	global cmdListener
	# Check for autostart
	autostart_flag = get_var('autostart')
	if autostart_flag != 'true' and args.autostart:
		print("Autostart not set!")
		return
	# Set up mmap files
	global shared_states
	if not os.path.isdir('/var/local/rct'):
		os.makedirs('/var/local/rct')

	devnull = open('/dev/null', 'w')

	mmap_file = open('/var/local/rct/status.dat', 'w+b')
	mmap_file.write(('\0' * 5).encode('utf-8'))
	mmap_file.flush()
	shared_states = mmap.mmap(mmap_file.fileno(), 5)

	cmdListener = CommandListener(shared_states, 4)

	signal.signal(signal.SIGINT, sigint_handler)
	signal.signal(signal.SIGTERM, sigint_handler)

	init_RCT_thread = threading.Thread(target=init_RCT, kwargs={'test':args.test})
	init_RCT_thread.start()

	signal.pause()

	cmdListener.stop()

	init_RCT_thread.join()

	shared_states.close()
	mmap_file.close()

if __name__ == '__main__':
	print(sys.argv)
	main()

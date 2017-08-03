#!/usr/bin/env python
import mraa
import os
import mmap
import threading
import signal
from enum import Enum
import time

thread_op = True
shared_states = None

STA_PIN = 53
SDR_PIN = 55
DIR_PIN = 57
GPS_PIN = 59
RDY_PIN = 61

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

def blink_RCT():
	global thread_op
	rdy_pin_state = False
	sta_pin_state = False
	rdy_pin_handle = mraa.Gpio(RDY_PIN)
	sta_pin_handle = mraa.Gpio(STA_PIN)
	rdy_pin_handle.dir(mraa.DIR_OUT)
	sta_pin_handle.dir(mraa.DIR_OUT)
	rdy_pin_handle.write(rdy_pin_state)
	sta_pin_handle.write(sta_pin_state)
	while thread_op:
		rct_state = RCT_STATES(ord(shared_states[3]))
		if rct_state == RCT_STATES(init):
			rdy_pin_state = False
			sta_pin_state = False
		elif rct_state == RCT_STATES(wait_init):
			rdy_pin_state = False
			sta_pin_state = False
		elif rct_state == RCT_STATES(wait_start):
			rdy_pin_state = False
			sta_pin_state = True
		elif rct_state == RCT_STATES(start):
			rdy_pin_state = False
			sta_pin_state = False
		elif rct_state == RCT_STATES(wait_end):
			rdy_pin_state = False
			sta_pin_state = not sta_pin_state
		elif rct_state == RCT_STATES(finish):
			rdy_pin_state = False
			sta_pin_state = True
		else:
			rdy_pin_state = False
			sta_pin_state = True
		rdy_pin_handle.write(rdy_pin_state)
		sta_pin_handle.write(sta_pin_state)
		time.sleep(1)
		pass
	rdy_pin_handle.write(False)
	sta_pin_handle.write(False)


def blink_SDR():
	global thread_op
	pin_state = False
	pin_handle = mraa.Gpio(SDR_PIN)
	pin_handle.dir(mraa.DIR_OUT)
	pin_handle.write(pin_state)
	while thread_op:
		sdr_state = SDR_INIT_STATES(ord(shared_states[0]))
		if sdr_state == SDR_INIT_STATES.find_devices:
			pin_state = not pin_state
		elif sdr_state == SDR_INIT_STATES.wait_recycle:
			pin_state = not pin_state
		elif sdr_state == SDR_INIT_STATES.usrp_probe:
			pin_state = not pin_state
		elif sdr_state == SDR_INIT_STATES.rdy:
			pin_state = True
		else:
			pin_state = False
		pin_handle.write(pin_state)
		time.sleep(1)
		pass
	pin_handle.write(False)

def blink_GPS():
	global thread_op
	pin_state = False
	pin_handle = mraa.Gpio(GPS_PIN)
	pin_handle.dir(mraa.DIR_OUT)
	pin_handle.write(pin_state)
	while thread_op:
		gps_state = GPS_STATES(ord(shared_states[2]))
		if gps_state == GPS_STATES.get_tty:
			pin_state = not pin_state
		elif gps_state == GPS_STATES.get_msg:
			pin_state = not pin_state
		elif gps_state == GPS_STATES.wait_recycle:
			pin_state = not pin_state
		elif gps_state == GPS_STATES.rdy:
			pin_state = True
		else:
			pin_state = False
		pin_handle.write(pin_state)
		time.sleep(1)
		pass
	pin_handle.write(False)

def blink_DIR():
	global thread_op
	pin_state = False
	pin_handle = mraa.Gpio(DIR_PIN)
	pin_handle.dir(mraa.DIR_OUT)
	pin_handle.write(pin_state)
	while thread_op:
		dir_state = OUTPUT_DIR_STATES(ord(shared_states[1]))
		if dir_state == OUTPUT_DIR_STATES.get_output_dir:
			pin_state = not pin_state
		elif dir_state == OUTPUT_DIR_STATES.check_output_dir:
			pin_state = not pin_state
		elif dir_state == OUTPUT_DIR_STATES.check_space:
			pin_state = not pin_state
		elif dir_state == OUTPUT_DIR_STATES.wait_recycle:
			pin_state = not pin_state
		elif dir_state == OUTPUT_DIR_STATES.rdy:
			pin_state = True
		else:
			pin_state = False
		pin_handle.write(pin_state)
		time.sleep(1)
		pass
	pin_handle.write(False)

def sigint_handler(signal, frame):
	print("Received sig")
	global thread_op
	thread_op = False

def main():
	# Set up mmap files
	if not os.path.isdir('/var/local/rct'):
		return 1
	mmap_file = open('/var/local/rct/status.dat', 'rb')
	global shared_states
	shared_states = mmap.mmap(mmap_file.fileno(), 4, mmap.MAP_SHARED, mmap.PROT_READ)
	signal.signal(signal.SIGINT, sigint_handler)
	signal.signal(signal.SIGTERM, sigint_handler)
	blink_SDR_thread = threading.Thread(target=blink_SDR)
	blink_GPS_thread = threading.Thread(target=blink_GPS)
	blink_DIR_thread = threading.Thread(target=blink_DIR)
	blink_RCT_thread = threading.Thread(target=blink_RCT)
	blink_SDR_thread.start()
	blink_GPS_thread.start()
	blink_DIR_thread.start()
	blink_RCT_thread.start()
	signal.pause()
	blink_SDR_thread.join()
	blink_GPS_thread.join()
	blink_DIR_thread.join()
	blink_RCT_thread.join()
	shared_states.close()
	mmap_file.close()
	
if __name__ == '__main__':
	main()

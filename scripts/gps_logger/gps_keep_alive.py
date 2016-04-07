# -* python *-

from pymavlink import mavutil
import time
import signal
import argparse
import sys
import os
from time import sleep

def handler(signum, frame):
	global runstate
	global led_handle
	runstate = False
	print("Handled...")
	led_handle.write('low')
	led_handle.close()
	sys.exit(0)

signal.signal(signal.SIGINT, handler)

global runstate
global led_handle
print("GPS_KEEPALIVE: Started")
runstate = True

led_handle = open("/sys/class/gpio/gpio17/direction", 'w')
led_handle.write('low')
led_handle.flush()

if not os.path.exists("/sys/class/gpio/gpio17"):
	print("GPS_KEEPALIVE: Could not access LED!")



parser = argparse.ArgumentParser(description = "MAVLink Keep Alive")
parser.add_argument('-i', '--port', help = 'MAVLink port', metavar = 'port', dest = 'port', required = True)

args = parser.parse_args()
port = args.port

# connect to MAV
mavmaster = mavutil.mavlink_connection(port, 57600)
fail_counter = 0
while True:
	if mavmaster.wait_heartbeat(blocking=False) is not None:
		break
	fail_counter += 1
	if fail_counter > 1000:
		led_handle.write('low')
		led_handle.flush()
		print("GPS_KEEPALIVE: ERROR: Timeout connecting!")
		sys.exit(1)
	if (fail_counter / 30) % 2 == 1:
		led_handle.write('high')
		led_handle.flush()
	if (fail_counter / 30) % 2 == 0:
		led_handle.write('low')
		led_handle.flush()
	sleep(0.005)

print("GPS_KEEPALIVE: Connected")

mavmaster.mav.request_data_stream_send(mavmaster.target_system,
		mavmaster.target_component, mavutil.mavlink.MAV_DATA_STREAM_POSITION,
		10, 1)

print("GPS_KEEPALIVE: Running")
while runstate:
	msg = mavmaster.recv_match(blocking=False, timeout = 10)
	if (fail_counter / 150) % 2 == 1:
		led_handle.write('high')
		led_handle.flush()
	if (fail_counter / 150) % 2 == 0:
		led_handle.write('low')
		led_handle.flush()
	fail_counter += 1
	sleep(0.005)
print("GPS_KEEPALIVE: Ending thread")

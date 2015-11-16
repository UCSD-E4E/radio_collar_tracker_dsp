#!/usr/bin/env python

from pymavlink import mavutil
import signal

def handler(signum, frame):
	global runstate
	runstate = False
	sys.exit(0)

signal.signal(signal.SIGINT, handler)

global runstate
print("GPS_KEEPALIVE: Started")
runstate = True

parser = argparse.ArgumentParser(description = "MAVLink Keep Alive")
parser.add_argument('-i', '--port', help = 'MAVLink port', metavar = 'port', dest = 'port', required = True)

args = parser.parse_args()
port = args.port

# connect to MAV
mavmaster = mavutil.mavlink_connect(port, 57600)
fail_counter = 0
while True:
	if mavmaster.wait_heartbeat(blocking=False) is not None:
		break
	fail_counter += 1
    if fail_counter > 1000:
        print("GPS_KEEPALIVE: ERROR: Timeout connecting!")
        sys.exit(1)
    sleep(0.005)

print("GPS_KEEPALIVE: Connected")

mavmaster.mav.request_data_stream_send(mavmaster.target_system,
        mavmaster.target_component, mavutil.mavlink.MAV_DATA_STREAM_POSITION,
        10, 1)

print("GPS_KEEPALIVE: Running")
while runstate:
    msg = mavmaster.recv_match(blocking=True, timeout = 10)
print("GPS_KEEPALIVE: Ending thread")

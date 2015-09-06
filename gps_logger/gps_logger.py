#!/usr/bin/env python
# Usage: api start ./gps_logger.py [-d <destination>] [-p <prefix>]
#        [-r <runNum>] [-s <suffix>]
#
# Defaults to   destination:    /media/RAW_DATA/rct/
#               prefix:         GPS_
#               suffix:         
#               runNum:         1
from pymavlink import mavutil
import time
import argparse
import signal
import sys
from time import sleep

def handler(signum, frame):
    global runstate
    runstate = False


signal.signal(signal.SIGINT, handler)

global runstate
print("GPS_LOGGER: Started")
runstate = True

argfile = open("gps_logger_args", "r")
dataDir = argfile.readline().strip()
gpsPrefix = argfile.readline().strip()
gpsSuffix = argfile.readline().strip()
runNum = int(argfile.readline().strip())
port = argfile.readline().strip()

logfile = open("%s/%s%06d" % (dataDir, gpsPrefix, runNum), "w")

# connect to MAV
mavmaster = mavutil.mavlink_connection(port, 57600)
fail_counter = 0
while True:
    if mavmaster.wait_heartbeat(blocking=False) is not None:
        break
    fail_counter += 1
    if fail_counter > 1000:
        print("GPS_LOGGER: ERROR: Timeout connecting!")
        sys.exit(1)
    sleep(0.005)

print("GPS_LOGGER: Connected")
mavmaster.mav.request_data_stream_send(mavmaster.target_system, 
        mavmaster.target_component, mavutil.mavlink.MAV_DATA_STREAM_POSITION,
        10, 1)

print("GPS_LOGGER: Running")



while runstate:
    msg = mavmaster.recv_match(blocking=False)
    if msg is not None:
        if msg.get_type() == 'GLOBAL_POSITION_INT':
    	    logfile.write("%.3f, %d, %d\n" % (time.time(), 
                msg.lat, msg.lon))
print("GPS_LOGGER: Ending thread")
logfile.close()


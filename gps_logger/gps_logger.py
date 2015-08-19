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

global runstate
print("GPS_LOGGER: Started")
runstate = True

argfile = open("gps_logger_args", "r")
dataDir = argfile.readline().strip()
gpsPrefix = argfile.readline().strip()
gpsSuffix = argfile.readline().strip()
runNum = int(argfile.readline().strip())

logfile = open("%s/%s%06d" % (dataDir, gpsPrefix, runNum), "w")

# connect to MAV
mavmaster = mavutil.mavlink_connection("/dev/ttyACM0", 57600)
mavmaster.wait_heartbeat()
print("GPS_LOGGER: Connected")
mavmaster.mav.request_data_stream_send(mavmaster.target_system, 
        mavmaster.target_component, mavutil.mavlink.MAV_DATA_STREAM_POSITION,
        10, 1)

def handler(signum, frame):
    global runstate
    runstate = False


signal.signal(signal.SIGINT, handler)

print("GPS_LOGGER: Running")



while runstate:
    msg = mavmaster.recv_match(blocking=False)
    if msg is not None:
        if msg.get_type() == 'GLOBAL_POSITION_INT':
    	    logfile.write("%.3f, %d, %d\n" % (time.time(), 
                msg.lat, msg.lon))
    time.sleep(0.5)
print("GPS_LOGGER: Ending thread")
logfile.close()


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

parser = argparse.ArgumentParser(description = "GPS Logging using MAVLink")
parser.add_argument('-o', '--output_dir', help = 'Output directory', metavar = 'data_dir', dest = 'dataDir', required = True)
parser.add_argument('-p', '--prefix', help = 'Output File Prefix, default "GPS_"', metavar = 'prefix', dest = 'prefix', default = 'GPS_')
parser.add_argument('-s', '--suffix', help = 'Output File Suffix, default ""', metavar = 'suffix', dest = 'suffix', default = '')
parser.add_argument('-r', '--run', help = 'Run Number', metavar = 'run_num', dest = 'runNum', required = True, type = int)
parser.add_argument('-i', '--port', help = 'MAVLink port', metavar = 'port', dest = 'port', required = True)

args = parser.parse_args()
dataDir = args.dataDir
gpsPrefix = args.prefix
gpsSuffix = args.suffix
runNum = args.runNum
port = args.port

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
    msg = mavmaster.recv_match(blocking=True, timeout = 10)
    if msg is not None:
        if msg.get_type() == 'GLOBAL_POSITION_INT':
    	    logfile.write("%.3f, %d, %d, %d, %d, %d, %d, %d, %d, %d\n" % (time.time(), 
                msg.lat, msg.lon, msg.time_boot_ms, msg.alt, msg.relative_alt, msg.vx, msg.vy, msg.vz, msg.hdg))
print("GPS_LOGGER: Ending thread")
logfile.close()


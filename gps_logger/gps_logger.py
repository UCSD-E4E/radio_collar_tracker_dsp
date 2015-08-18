#!/usr/bin/env python
# Usage: api start ./gps_logger.py [-d <destination>] [-p <prefix>]
#        [-r <runNum>] [-s <suffix>]
#
# Defaults to   destination:    /media/RAW_DATA/rct/
#               prefix:         GPS_
#               suffix:         
#               runNum:         1
from droneapi.lib import VehicleMode
from pymavlink import mavutil
import time
import argparse
import signal

parser = argparse.ArgumentParser();
parser.add_argument("-d", dest="destination", default="/media/RAW_DATA/rct/")
parser.add_argument("-p", dest="prefix", default="GPS_")
parser.add_argument("-s", dest="suffix", default="")
parser.add_argument("-r", dest="runNum", default=1)

argfile = open("gps_logger_args", "r")

args = parser.parse_args(argfile.readline().strip().split())
dataDir = args.destination
gpsPrefix = args.prefix
gpsSuffix = args.suffix
runNum = int(args.runNum)

api = local_connect()
v = api.get_vehicles()[0]

logfile = open("%s/%s%d%s" % (dataDir, gpsPrefix, runNum, gpsSuffix), "w")

current_milli_time = lambda: int(round(time.time() * 1000))

while True:
    if v.location.lat is not None and v.location.lon is not None:
    	logfile.write("%.3f, %f, %f\n" % (current_milli_time() / 1000.0, float(v.location.lat), float(v.location.lon)))
    time.sleep(0.5)


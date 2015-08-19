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

argfile = open("gps_logger_args", "r")
dataDir = argfile.readline().strip()
gpsPrefix = argfile.readline().strip()
gpsSuffix = argfile.readline().strip()
runNum = int(argfile.readline().strip())


api = local_connect()
v = api.get_vehicles()[0]

logfile = open("%s/%s%d" % (dataDir, gpsPrefix, runNum), "w")

print "Running"

while True:
    if v.location.lat is not None and v.location.lon is not None:
    	logfile.write("%.3f, %f, %f\n" % (round(time.time()) / 1000.0, float(v.location.lat), float(v.location.lon)))
    time.sleep(0.5)


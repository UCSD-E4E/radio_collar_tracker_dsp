#!/usr/bin/env python
# -* python *-
# Usage: api start ./gps_logger.py [-d <destination>] [-p <prefix>]
#        [-r <runNum>] [-s <suffix>]
#
# Defaults to   destination:    /media/RAW_DATA/rct/
#               prefix:         GPS_
#               suffix:
#               runNum:         1
import time
import argparse
import signal
import sys
from time import sleep
import serial
import pynmea2
import calendar
import math

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
parser.add_argument('-i', '--port', help = 'NMEA port', metavar = 'port', dest = 'port', required = True)
parser.add_argument('-b', '--baud_rate', help = 'baud rate', metavar = 'baud', dest = 'baud', required = False, default = 57600, type = int)

args = parser.parse_args()
dataDir = args.dataDir
gpsPrefix = args.prefix
gpsSuffix = args.suffix
runNum = args.runNum
port = args.port
baud = args.baud

logfile = open("%s/%s%06d" % (dataDir, gpsPrefix, runNum), "w")
stream = serial.Serial(port, baud, timeout = 5)
print("GPS_LOGGER: Running")


ref_time = time.time()
gps_time = 0
offset = gps_time - ref_time

while runstate:
    line = None
    try:
        line = stream.readline()
    except serial.serialutil.SerialException, e:
        continue

    if line is not None:
        msg = None
        try:
            msg = pynmea2.parse(line);
        except pynmea2.ParseError, e:
            continue
        # logfile.write("%.3f, %d, %d, %.3f, %d, %d, %d, %d, %d, %d\n" % (time.time(),
        #     msg.latitude, msg.longitude, msg.timestamp))
        print(msg.sentence_type)
        if msg.sentence_type == 'GGA':
            alt = 0;
            if(msg.altitude is not None):
                alt = msg.altitude
        if msg.sentence_type == 'RMC':
            local_timestamp = time.time()
            lat = 0
            if msg.latitude is not None:
                lat = msg.latitude
            lon = 0
            if msg.longitude is not None:
                lon = msg.longitude
            if msg.data[0] == '':
                global_timestamp = 0
            else:
                global_timestamp = calendar.timegm(msg.datetime.timetuple())
            alt = 0
            rel_alt = -1
            spd_idx = msg.name_to_idx['spd_over_grnd']
            if msg.data[spd_idx] != '':
                spd = float(msg.data[spd_idx]) * 0.514444
            else:
                spd = 0
            bearing_idx = msg.name_to_idx['true_course']
            if msg.data[bearing_idx] != '':
                bearing = float(msg.data[bearing_idx])
            else:
                bearing = 0
            vx = spd * math.cos(math.radians(bearing))
            vy = spd * math.sin(math.radians(bearing))
            vz = -1
            hdg = 999
            logfile.write("%.3f, %d, %d, %.3f, %d, %d, %d, %d, %d, %d\n" % (local_timestamp,
                lat*1e7, lon*1e7, global_timestamp, alt, rel_alt, vx, vy, vz, hdg))
print("GPS_LOGGER: Ending thread")
logfile.close()


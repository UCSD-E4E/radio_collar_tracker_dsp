#!/usr/bin/env python

import serial
import sys
import time
port = serial.Serial(sys.argv[1], baudrate=1200, dsrdtr=True)
time.sleep(0.5)
port.close()

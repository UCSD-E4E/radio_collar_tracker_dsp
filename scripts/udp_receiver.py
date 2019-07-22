#!/usr/bin/env python3

import socket
import argparse
import os
import json
import numpy as np
import utm

class Ping(object):
	"""Ping object"""
	def __init__(self, packet):
		super(Ping, self).__init__()
		assert(isinstance(packet, dict))
		assert('ping' in packet)
		assert('time' in packet['ping'])
		assert('lat' in packet['ping'])
		assert('lon' in packet['ping'])
		assert('alt' in packet['ping'])
		assert('amp' in packet['ping'])
		assert('txf' in packet['ping'])

		self._time = int(packet['ping']['time'])
		self._lat = float(packet['ping']['lat'])
		self._lon = float(packet['ping']['lon'])
		self._alt = float(packet['ping']['alt'])
		self._amp = float(packet['ping']['amp'])
		self._txf = int(packet['ping']['txf'])

		x = utm.from_latlon(self._lat, self._lon)
		self._easting = x[0]
		self._northing = x[1]
		self._zonenum = x[2]
		self._zone = x[3]


	def toNumpy(self):
		return np.array([self._easting, self._northing, self._alt, self._amp])

	def getFrequency(self):
		return self._txf

	def getUTMZone(self):
		return (self._zonenum, self._zone)


def main():
	parser = argparse.ArgumentParser("Radio Telemetry Tracker Payload Receiver")

	args = parser.parse_args()

	UDP_PORT = 9000
	BUFFER_LEN = 1024

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.bind(('', UDP_PORT))

	pings = []

	try:
		while True:
			data, addr = sock.recvfrom(BUFFER_LEN)
			print(data)
			packet = json.loads(data)
			if 'ping' in packet:
				pings.append(Ping(packet))

				if len(pings) > 4:
					D = np.array([ping.toNumpy() for ping in pings])
	except:
		pass

if __name__ == '__main__':
	main()
#!/usr/bin/env python

import socket
import argparse
import os
import json
import numpy as np
import utm
import heatMap
import generateKML
import pos_estimate
import platform
import select
import threading
import datetime

import tkinter as tk

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
		self._lat = float(packet['ping']['lat']) / 1e7
		self._lon = float(packet['ping']['lon']) / 1e7

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

	def getLonLat( self ):
		return ( self._lon, self._lat )

	def getFrequency(self):
		return self._txf

	def getUTMZone(self):
		return (self._zonenum, self._zone)

def findFile( filename, path ):
	for root, dirs, files in os.walk( path ):
		if filename in files:
			return os.path.join( root, filename )
	return None

def waitForHeartbeat(socket, timeout = 30):
	counter = 0
	while counter < 30:
		ready = select.select([socket], [], [], 1)
		if ready[0]:
			data, addr = socket.recvfrom(1024)
			packet = json.loads(data.decode('utf-8'))
			if 'heartbeat' in packet:
				return addr
		else:
			print("waiting")
			counter += 1
	return None

class CommandGateway():
	"""docstring for CommandGateway"""
	def __init__(self, mav_IP, socket):
		self.mav_IP = mav_IP
		self._socket = socket
		self.thread = threading.Thread(target=self.mainloop)
		self.thread.start()
		self.run = True
		self._freqElements = []

		self.getFreqs()

	def getFreqs(self):
		cmdPacket = {}
		cmdPacket['cmd'] = {}
		cmdPacket['cmd']['id'] = 'gcs'
		cmdPacket['cmd']['action'] = 'getF'
		msg = json.dumps(cmdPacket)
		print("Send: %s" % (msg))
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)

	def isAlive(self):
		return self.run

	def startCommand(self):
		cmdPacket = {}
		cmdPacket['cmd'] = {}
		cmdPacket['cmd']['id'] = 'gcs'
		cmdPacket['cmd']['action'] = 'start'
		msg = json.dumps(cmdPacket)
		print("Send: %s" % msg)
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)

	def stopCommand(self):
		cmdPacket = {}
		cmdPacket['cmd'] = {}
		cmdPacket['cmd']['id'] = 'gcs'
		cmdPacket['cmd']['action'] = 'stop'
		msg = json.dumps(cmdPacket)
		print("Send: %s" % msg)
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)

	def windowClose(self):
		self.m.destroy()
		self.run = False

	def setFreqs(self, freqs):
		assert(isinstance(freqs, list))
		
		if len(self._freqElements) < len(self.freqs):
			for i in range(len(self.freqs) - len(self._freqElements)):
				self._freqElements.append(tk.Entry(self.freqFrame))
				self._freqElements[-1].pack()
		if len(self.freqs) < len(self._freqElements):
			for i in range(len(self._freqElements) - len(self.freqs)):
				element = self._freqElements.pop(-1)
				element.destroy()

		for freq in freqs:
			freqButton.delete(0, END)
			freqButton.insert(0, freq)

	def addFreq(self):
		self._freqElements.append(tk.Entry(self.freqFrame))
		self._freqElements[-1].pack()

	def sendFreq(self):
		freqs = []
		for entryObj in self._freqElements:
			freq = int(entryObj.get())
			freqs.append(freq)
		cmdPacket = {}
		cmdPacket['cmd'] = {}
		cmdPacket['cmd']['id'] = 'gcs'
		cmdPacket['cmd']['action'] = 'setF'
		cmdPacket['cmd']['setF'] = freqs
		msg = json.dumps(cmdPacket)
		print("Send: %s" % msg)
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)


	def removeFreq(self):
		element = self._freqElements.pop(-1)
		element.destroy()
		

	def mainloop(self):
		self.m = tk.Tk()
		self.startButton = tk.Button(self.m, text='Start', command=self.startCommand)
		self.stopButton = tk.Button(self.m, text='Stop', command=self.stopCommand)
		self.startButton.pack()
		self.stopButton.pack()
		self.freqFrame = tk.LabelFrame(self.m, text='Frequencies', padx=5, pady=5)
		self.freqFrame.pack()
		self.addFreqButton = tk.Button(self.m, text='Add Frequency', command=self.addFreq)
		self.addFreqButton.pack()
		self.removeFreqButton = tk.Button(self.m, text = 'Remove Frequency', command = self.removeFreq)
		self.removeFreqButton.pack();
		self.commitFreqButton = tk.Button(self.m, text="Upload Frequencies", command = self.sendFreq)
		self.commitFreqButton.pack()
		self.m.protocol("WM_DELETE_WINDOW", self.windowClose)
		self.m.mainloop()
		

def main():
	# create a point.kml file if one doesn't exist
	if findFile( "point.kml", "." ) is None:
		open('point.kml', 'a+').close()

	isWindows = ( platform.system() == 'Windows' )
	parser = argparse.ArgumentParser("Radio Telemetry Tracker Payload Receiver")
	args = parser.parse_args()
	UDP_PORT = 9000
	BUFFER_LEN = 1024
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


	sock.bind(("", UDP_PORT))
	mav_IP = waitForHeartbeat(sock)
	if mav_IP is None:
		print("No hearbeat packets received!")
		return
	commandGateway = CommandGateway(mav_IP, sock)

	pings = []
	guess = [0,0,0]

	last_heartbeat = datetime.datetime.now()

	while commandGateway.isAlive():
		ready = select.select([sock], [], [], 1)
		if ready[0]:
			data, addr = sock.recvfrom(BUFFER_LEN)
			print(data.decode('utf-8').strip())
			packet = json.loads(data.decode('utf-8'))
			if 'ping' in packet:
				pings.append(Ping(packet))
				if len(pings) > 4:
					initZoneNum = pings[0].getUTMZone()[0]
					initZone = pings[0].getUTMZone()[1]
					D = np.array([ping.toNumpy() for ping in pings])
					# Save previous estimates
					guess = pos_estimate.calculateEstimate( D, initZoneNum, initZone, guess )
					# Convert to lat lon
					ll = utm.to_latlon( guess[0], guess[1], initZoneNum, zone_letter=initZone )
					ll = [ ll[1],ll[0] ]
					newpackage = generateKML.kmlPackage( "RECEIVED PINGS", [ll[0],ll[1]], None )
					generateKML.generateKML( [ newpackage ] )
			if 'heartbeat' in packet:
				last_heartbeat = datetime.datetime.now()
			if 'frequencies' in packet:
				freqs = packet['frequencies']
				commandGateway.setFreqs([int(freq) for freq in freqs])

		if (datetime.datetime.now() - last_heartbeat).total_seconds() > 30:
			print("No heartbeats!")
			# msgbox?

if __name__ == '__main__':
	main()

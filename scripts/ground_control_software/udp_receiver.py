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
import time

import matplotlib
matplotlib.use('TkAgg')

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
		self.freqs = []
		self.freqFrame = None

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
		self.freqs = freqs

		while self.freqFrame is None:
			time.sleep(0.01)

		if len(self._freqElements) < len(freqs):
			for i in range(len(freqs) - len(self._freqElements)):
				self._freqElements.append(tk.Entry(self.freqFrame))
				self._freqElements[-1].pack()
		if len(freqs) < len(self._freqElements):
			for i in range(len(self._freqElements) - len(freqs)):
				element = self._freqElements.pop(-1)
				element.destroy()

		for i in range(len(freqs)):
			self._freqElements[i].delete(0, "end")
			self._freqElements[i].insert(0, freqs[i])

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
		cmdPacket['cmd']['frequencies'] = freqs
		msg = json.dumps(cmdPacket)
		print("Send: %s" % msg)
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)


	def removeFreq(self):
		element = self._freqElements.pop(-1)
		element.destroy()

	def setOptions(self, options):
		if self.configureWindow is not None:
			self.centerFreqEntry.insert(0, int(options['center_freq'][0]))
			self.samplingFreqEntry.insert(0, int(options['sampling_freq'][0]))
			self.pingWidthEntry.insert(0, int(options['ping_width_ms'][0]))
			self.pingMaxEntry.insert(0, float(options['ping_max_len_mult'][0]))
			self.pingMinEntry.insert(0, float(options['ping_min_len_mult'][0]))

	def sendOptions(self):
		# {"options": {"center_freq": ["173500000"], "autostart": ["true"], "ping_width_ms": ["27"], "gps_baud": ["9600"], "frequencies": ["173965000"], "output_dir": ["/mnt/RAW_DATA"], "gps_mode": ["false"], "ping_min_snr": ["5"], "sampling_freq": ["1500000"], "ping_max_len_mult": ["1.5"], "gps_target": ["/dev/ttyACM0"], "ping_min_len_mult": ["0.5"]}}
		packet = {}
		packet['cmd'] = {}
		packet['cmd']['id'] = 'gcs'
		packet['cmd']['action'] = 'setOpts'
		packet['cmd']['options'] = {}
		packet['cmd']['options']['center_freq'] = self.centerFreqEntry.get()
		packet['cmd']['options']['sampling_freq'] = self.samplingFreqEntry.get()
		packet['cmd']['options']['ping_width_ms'] = self.pingWidthEntry.get()
		packet['cmd']['options']['ping_min_len_mult'] = self.pingMinEntry.get()
		packet['cmd']['options']['ping_max_len_mult'] = self.pingMaxEntry.get()
		msg = json.dumps(packet)
		print("Send: %s" % msg)
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)
		self.configureWindow.destroy()
		self.configureWindow = None

	def upgradeSoftware(self):
		ok = tk.tkMessageBox.askokcancel(title='Software Upgrade', 
			message='You are trying to upgrade the remote software.  This may'
			' cause software issues!  Are you sure about this?', parent=self.m)

		if not ok:
			return

		fname = tk.tkFileDialog.askopenfilename(initialdir=".", 
			title='Select Upgrade Package', filetypes=(('Repo Archive', '*.zip')))
		if fname is None or fname == '':
			return
		cmdPacket = {}
		cmdPacket['cmd'] = {}
		cmdPacket['cmd']['id'] = 'gcs'
		cmdPacket['cmd']['action'] = 'upgrade'
		msg = json.dumps(packet)
		print("Send: %s" % (msg))
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)

		sock = socket.socket()
		host = socket.gethostname()
		port = 9500
		mav_IP = (self.mav_IP[0], port)
		sock.connect(mav_IP)
		byteCounter = 0
		with open(fname) as archiveFile:
			frame = archiveFile.read(1024)
			byteCounter += len(frame)
			while frame:
				sock.send(frame)
				frame = archiveFile.read(1024)
				byteCounter += len(frame)
		sock.shutdown()
		print("Sent %d bytes" % (byteCounter))


	def configureOpts(self):
		cmdPacket = {}
		cmdPacket['cmd'] = {}
		cmdPacket['cmd']['id'] = 'gcs'
		cmdPacket['cmd']['action'] = 'getOpts'
		msg = json.dumps(cmdPacket)
		print("Send: %s" % msg)
		self._socket.sendto(msg.encode('utf-8'), self.mav_IP)

		self.configureWindow = tk.Toplevel(self.m)

		self.centerFreqLabel = tk.Label(self.configureWindow, text="Center Frequency")
		self.centerFreqLabel.grid(row=1, column=1)

		self.centerFreqEntry = tk.Entry(self.configureWindow)
		self.centerFreqEntry.grid(row=1, column=2)

		self.samplingFreqLabel = tk.Label(self.configureWindow, text="Sampling Frequency")
		self.samplingFreqLabel.grid(row=2, column=1)

		self.samplingFreqEntry = tk.Entry(self.configureWindow)
		self.samplingFreqEntry.grid(row=2, column=2)

		self.pingWidthLabel = tk.Label(self.configureWindow, text="Ping Width")
		self.pingWidthLabel.grid(row=3, column=1)

		self.pingWidthEntry = tk.Entry(self.configureWindow)
		self.pingWidthEntry.grid(row=3, column=2)

		self.pingMaxLabel = tk.Label(self.configureWindow, text="Ping Width Max")
		self.pingMaxLabel.grid(row=4, column=1)

		self.pingMaxEntry = tk.Entry(self.configureWindow)
		self.pingMaxEntry.grid(row=4, column=2)

		self.pingMinLabel = tk.Label(self.configureWindow, text="Ping Width Min")
		self.pingMinLabel.grid(row=5, column=1)

		self.pingMinEntry = tk.Entry(self.configureWindow)
		self.pingMinEntry.grid(row=5, column=2)

		self.sendConfigButton = tk.Button(self.configureWindow, text='Send Parameters', command = self.sendOptions)
		self.sendConfigButton.grid(row=6, column=1, columnspan=2)


		

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
		self.configureButton = tk.Button(self.m, text="Configure", command = self.configureOpts)
		self.configureButton.pack()

		self.upgradeButton = tk.Button(self.m, text = "Upgrade Software", command = self.upgradeSoftware)
		self.upgradeButton.pack()

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

	pingFrequencies = []
	pingEstimates = []
	pingPackages = []

	while commandGateway.isAlive():
		ready = select.select([sock], [], [], 1)
		if ready[0]:
			data, addr = sock.recvfrom(BUFFER_LEN)
			# print(data.decode('utf-8').strip())
			packet = json.loads(data.decode('utf-8'))
			if 'ping' in packet:
				ping = Ping(packet)
				if ping.getFrequency() not in pingFrequencies:
					pingFrequencies.append(ping.getFrequency())
					pings.append([])
					pingEstimates.append([])
					pingPackages.append(generateKML.kmlPackage("%.3f MHz" % (ping.getFrequency() / 1e6), list(ping.getLonLat()), 0, False))
				freqIdx = pingFrequencies.index(ping.getFrequency())
				pings[freqIdx].append(Ping(packet))
				print("Got ping at %d" % (pingFrequencies[freqIdx]))
				if len(pings[freqIdx]) > 4:
					initZoneNum = pings[freqIdx][0].getUTMZone()[0]
					initZone = pings[freqIdx][0].getUTMZone()[1]
					D = np.array([ping.toNumpy() for ping in pings[freqIdx]])
					# Save previous estimates
					guess = pos_estimate.calculateEstimate( D, initZoneNum, initZone, pingEstimates[freqIdx] )
					# Convert to lat lon
					if guess is None:
						print("Failed to calculate estimate for collar %d" % (pingFrequencies[freqIdx]))
						pingPackages[freqIdx].setStatus(False)
						pingEstimates[freqIdx] = None
					else:
						print("Found good esimtate for collar %d" % (pingFrequencies[freqIdx]))
						ll = utm.to_latlon( guess[0], guess[1], initZoneNum, zone_letter=initZone )
						ll = [ ll[1],ll[0] ]
						pingEstimates[freqIdx] = guess
						pingPackages[freqIdx].setEstimate(ll)
						pingPackages[freqIdx].setScore(guess[2])
						pingPackages[freqIdx].setStatus(True)
				generateKML.generateKML( pingPackages )
			if 'heartbeat' in packet:
				last_heartbeat = datetime.datetime.now()
			if 'frequencies' in packet:
				freqs = packet['frequencies']
				commandGateway.setFreqs([int(freq) for freq in freqs])
			if 'options' in packet:
				commandGateway.setOptions(packet['options'])

		if (datetime.datetime.now() - last_heartbeat).total_seconds() > 30:
			print("No heartbeats!")
			# msgbox?

if __name__ == '__main__':
	main()

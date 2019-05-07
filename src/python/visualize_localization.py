#!/usr/bin/env python3

import argparse
import glob
import os
import json
import numpy as np
from matplotlib import pyplot as plt; plt.ion()
from osgeo import gdal
import shapefile

class Ping(object):
	"""RCT Ping"""
	def __init__(self, lat, lon, amplitude, freq, alt, sequence):
		super(Ping, self).__init__()
		self.lat = lat
		self.lon = lon
		self.amplitude = amplitude
		self.freq = freq
		self.alt = alt
		self.seq = sequence


class Estimate(object):
	"""RCT Estimate"""
	def __init__(self, lat, lon, alt, freq, sequence):
		super(Estimate, self).__init__()
		self.lat = lat
		self.lon = lon
		self.alt = alt
		self.freq = freq
		self.seq = sequence
		

if __name__ == '__main__':
	parser = argparse.ArgumentParser('Visualizer for localization data')
	parser.add_argument('data_dir', help="Data directory")

	args = parser.parse_args()
	# data_dir = "/media/ntlhui/FA56-CFCD/2019.05.05/RUN_000006/"
	data_dir = args.data_dir

	if os.path.isdir(data_dir):
		data_dir = data_dir + os.path.sep
	run_num = int(os.path.basename(os.path.dirname(data_dir)).split('_')[1])

	localization_file = os.path.join(data_dir, "LOCALIZE_%06d" % (run_num))

	assert(os.path.isfile(localization_file))

	localization_data = open(localization_file)

	pings = []
	estimates = []
	freqs = set()
	est_seq = 1
	ping_seq = 1

	for line in localization_data:
		data = json.loads(line)

		if 'ping' in data.keys():
			# got ping
			ping = data['ping']
			ping_obj = Ping(ping['lat'] / 1e7, ping['lon'] / 1e7, ping['amp'], ping['txf'], 0, ping_seq)
			freqs.add(ping['txf'])
			pings.append(ping_obj)
			ping_seq += 1

		if 'estimate' in data.keys():
			# got estimate
			estimate = data['estimate']
			estimate_obj = Estimate(estimate['lat'], estimate['lon'], 0, 0, est_seq)
			estimates.append(estimate_obj)
			est_seq += 1

	for freq in freqs:
		h_freq = freq / 1e3
		f_pings = [ping for ping in pings if ping.freq == freq]
		f_est  = [estimate for estimate in estimates]
		if len(f_pings) < 5:
			continue

		writer = shapefile.Writer(os.path.join(data_dir, "tx_%d_ping" % (h_freq)), shapeType = shapefile.POINT)
		writer.field('amplitude', 'N', decimal = 10)
		writer.field('sequence', 'N')
		for ping in f_pings:
			writer.point(ping.lon, ping.lat)
			writer.record(ping.amplitude, ping.seq)
		writer.close()
		proj = open(os.path.join(data_dir, "tx_%06d_ping.prj" % (h_freq)), 'w')
		epsg1 = 'GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433]]'
		proj.write(epsg1)
		proj.close()

		writer = shapefile.Writer(os.path.join(data_dir, "tx_%d_est" % (h_freq)), shapeType = shapefile.POINT)
		writer.field('sequence', 'N')
		for est in f_est:
			writer.point(est.lon, est.lat)
			writer.record(est.seq)
		writer.close()
		proj = open(os.path.join(data_dir, "tx_%06d_est.prj" % (h_freq)), 'w')
		epsg1 = 'GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433]]'
		proj.write(epsg1)
		proj.close()
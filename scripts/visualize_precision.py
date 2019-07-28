#!/usr/bin/env python3

import numpy as np
from scipy.optimize import least_squares
import scipy.stats
import argparse
import os
from visualize_localization import Ping
import json
import utm
from osgeo import gdal
import math
import osr

def prob_d_pos(dx, tx, n, k, R, mu, sigma):
	retval = prob_d(np.linalg.norm(dx - tx), n, k, R, mu, sigma)
	if retval < 1e-9:
		return -9
	else:
		return np.log10(retval)

def prob_d(d, n, k, R, mu, sigma):
	P = scipy.stats.norm(mu, sigma)
	return P.pdf(10 * n * np.log10(d) + k - R) * 10 * n / np.log(10) / d

class SignalModel:
	def __init__(self, mu, sigma, n, k, R):
		self.mu = mu
		self.sigma = sigma
		self.P = scipy.stats.norm(mu, sigma)
		self.n = n
		self.k = k
		self.R = R

	def p_d(self, d):
		return self.P.pdf(10 * self.n * np.log10(d) + self.k - self.R) * 10 * self.n / np.log(10) / d

	def p_x(self, dx, tx):
		return self.p_d(np.linalg.norm(dx - tx))

def residuals(x, data):
	P = x[0]
	n = x[1]
	tx = x[2]
	ty = x[3]
	k = x[4]

	R = data[:,0]
	dx = data[:,1]
	dy = data[:,2]

	d = np.linalg.norm(np.array([dx - tx, dy - ty]).transpose())
	return P - 10 * n * np.log10(d) + k - R



def mse(R, x, P, n, t, k):
	d = np.linalg.norm(x - t)
	return (R - P + 10 * n * np.log10(d) + k) ** 2

if __name__ == '__main__':
	parser = argparse.ArgumentParser("Precision visualizer for localization data")
	parser.add_argument('data_dir', help="Data directory")

	args = parser.parse_args()
	data_dir = args.data_dir
	# data_dir = '/media/ntlhui/FA56-CFCD/2017.08.17/RUN_000008/'

	if os.path.isdir(data_dir):
		data_dir = "%s%s" % (data_dir, os.path.sep)
	run_num = int(os.path.basename(os.path.dirname(data_dir)).split('_')[1])

	localization_file = os.path.join(data_dir, "LOCALIZE_%06d" % (run_num))

	assert(os.path.isfile(localization_file))

	with open(localization_file) as localization_data:
		pings = []

		freqs = set()
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

	for freq in freqs:
		f_pings = [ping for ping in pings if ping.freq == freq]
		if len(f_pings) <= 6:
			continue

		print("%03.3f has %d pings" % (freq / 1e6, len(f_pings)))

		amplitudes = [ping.amplitude for ping in f_pings]
		# lons = [ping.lon for ping in f_pings]
		# lats = [ping.lat for ping in f_pings]

		eastings = [utm.from_latlon(ping.lat, ping.lon)[0] for ping in f_pings]
		northings = [utm.from_latlon(ping.lat, ping.lon)[1] for ping in f_pings]
		zonenum = utm.from_latlon(f_pings[0].lat, f_pings[0].lon)[2]
		zone = utm.from_latlon(f_pings[0].lat, f_pings[0].lon)[3]

		x0 = np.array([40, 2, np.mean(eastings), np.mean(northings), 0])

		data = np.array([amplitudes, eastings, northings]).transpose()
		res_x = least_squares(residuals, x0, bounds=([0, 1.5, 0, 0, 0], [np.inf, 6, 1e9, 1e9, 20]), kwargs={'data':data})
		if not res_x.status:
			print("Failed to converge!")
			break

		P = res_x.x[0]
		n = res_x.x[1]
		tx = res_x.x[2]
		ty = res_x.x[3]
		k = res_x.x[4]

		print("Params: %.3f, %.3f, %.0f, %.0f, %.3f" % (P, n, tx, ty, k))

		dx = data[:,1]
		dy = data[:,2]
		R = amplitudes

		d = np.linalg.norm(np.array([dx, dy]).transpose() - np.array([tx, ty]), axis=1)


		# data
		outputFileName = "%s/DATA_%03.3f.csv" % (data_dir, freq / 1e6)
		with open(outputFileName, 'w') as ofile:
			for i in range(len(R)):
				ofile.write("%f,%f\n" % (R[i], d[i]))

		P_samp = R - k + 10 * n * np.log10(d)
		mu_P = np.mean(P_samp)
		sigma_P = np.var(P_samp)
		print("P variation: %.3f" % (sigma_P))

		margin = 10
		tiffXSize = int(2 * np.max(d) + margin)
		tiffYSize = int(2 * np.max(d) + margin)
		pixelSize = 1
		heatMapArea = np.ones((tiffYSize, tiffXSize)) # [y, x]
		minY = ty - np.max(d) - margin / 2
		refY = ty + np.max(d) + margin / 2
		refX = tx - np.max(d) - margin / 2
		maxX = tx + np.max(d) + margin / 2

		models = [SignalModel(mu_P, sigma_P, n, k, powers) for powers in R]

		for x in range(tiffXSize):
			for y in range(tiffYSize):
				# for i in [0]:
				for i in range(len(R)):
					heatMapArea[y, x] += models[i].p_x(np.array([refX + x, refY - y]), np.array([tx, ty]))

		if np.isnan(np.min(heatMapArea)):
			break
		heatMapArea = np.power(10, heatMapArea)
		heatMapArea -= np.min(heatMapArea)
		heatMapArea /= np.sum(heatMapArea)

		outputFileName = '%s/PRECISION_%03.3f_heatmap.tiff' % (data_dir, freq / 1e6)
		driver = gdal.GetDriverByName('GTiff')
		dataset = driver.Create(
			outputFileName,
			tiffXSize,
			tiffYSize,
			1,
			gdal.GDT_Float32, ['COMPRESS=LZW'])
		spatialReference = osr.SpatialReference()
		
		spatialReference.SetUTM(zonenum, zone >= 'N')
		spatialReference.SetWellKnownGeogCS('WGS84')
		wkt = spatialReference.ExportToWkt()
		retval = dataset.SetProjection(wkt)
		dataset.SetGeoTransform((
			refX,    # 3
			1,                      # 4
			0,
			refY,    # 0
			0,  # 1
			-1))                     # 2
		band = dataset.GetRasterBand(1)

		band.WriteArray(heatMapArea)
		band.SetStatistics(np.amin(heatMapArea), np.amax(heatMapArea), np.mean(heatMapArea), np.std(heatMapArea))
		dataset.FlushCache()
		dataset = None
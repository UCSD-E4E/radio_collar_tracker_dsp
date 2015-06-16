#!/usr/bin/env python
import sys
import numpy as np
import matplotlib.pyplot as plot
# USING utm 0.4.0 from https://pypi.python.org/pypi/utm
import utm
import os
import argparse

parser = argparse.ArgumentParser(description='Processes RUN_XXXXXX.csv files '
	'from the Radio Collar Tracker software to generate maps of radio collar '
	'signal strength')
parser.add_argument('filename', help='CSV file from the spectrumAnalysis code')
parser.add_argument('-d', '--directory', default=os.getcwd(),
	help='output directory for map files', metavar='<directory>')
parser.add_argument('--ignore-run-number', action='store_true',
	dest='run_num_mismatch',
	help='Ignore run number mistmatch between filename and configuration file')
args = parser.parse_args()

filename = args.filename
# ignore_number_mismatch = args.run_num_mismatch
output_path = args.directory

# Load config file
try:
	job_cfg = open('JOB', 'r')
except Exception, e:
	print("Unable to open JOB config file! Aborting...")
	sys.exit(1)
job_cfg.readline()	# toss data path
run_num = int(job_cfg.readline().rpartition(" ")[2])
freq_em = int(job_cfg.readline().rpartition(" ")[2])
pulse_ms = int(job_cfg.readline().rpartition(" ")[2])
lin_scale = int(job_cfg.readline().rpartition(" ")[2])
map_d = int(job_cfg.readline().rpartition(" ")[2])
alpha_c_thres = int(job_cfg.readline().rpartition(" ")[2])
num_col = int(job_cfg.readline().rpartition(" ")[2])
freq_drift = int(job_cfg.readline().rpartition(" ")[2])

# Load collar file
try:
	col_cfg = open('COL', 'r')
except Exception, e:
	print("Unable to open collar definitions file! Aborting...")
	sys.exit(1)
collars = []
for i in range(num_col):
	try:
		collars.append(int(col_cfg.readline().strip()))
	except Exception, e:
		print("Error: Could not load collar definitions! Aborting...")
		sys.exit(1)

# Load data file
try:
	filenum = int(filename.partition("_")[2].rpartition(".")[0])
except Exception,e:
	run_num = -1
if run_num != filenum:
	print("Error: Filename does not match expected run number from config! Exiting...")
	sys.exit(1)


if filename.rpartition(".")[2] != "csv":
	print("Invalid file! Usage: display_data.py data_file")
	print("data_file   CSV file from the spectrumAnalysis code")
# make list of columns
names = ['lat', 'lon', 'alt', 'r1', 'r2']
for i in xrange(1, num_col + 1):
	names.append('col' + str(i))

# Read CSV
data = np.genfromtxt(filename, delimiter=',', names=names)
coldata = [[0 for x in range(len(data['lat']))] for x in range(num_col)]
# Modify values
lat = [x / 10000000 for x in data['lat']]
lon = [x / 10000000 for x in data['lon']]
alt = [x / 1000 for x in data['alt']]
for i in xrange(1, num_col + 1):
	colname = 'col' + str(i)
	coldata[i-1] = [x / 1000 for x in data[colname]]
# convert deg to utm
zone = "X"
zonenum = 60;
for i in range(len(data['lat'])):
	utm_coord = utm.from_latlon(lat[i], lon[i])
	zonenum = utm_coord[2]
	zone = utm_coord[3]
	lon[i] = utm_coord[0]
	lat[i] = utm_coord[1]

maxCol = np.amax(coldata)
minCol = np.amin(coldata)
for i in xrange(1, num_col + 1):
	fig = plot.figure(i)
	fig.set_size_inches(8, 6)
	fig.set_dpi(72)
	curColMap = plot.cm.get_cmap('jet')
	sc = plot.scatter(lon, lat, c=coldata[i - 1], cmap=curColMap, vmin = minCol, vmax = maxCol)
	colorbar = plot.colorbar(sc)
	colorbar.set_label('SNR')
	plot.grid()
	ax = plot.gca()
	ax.get_xaxis().get_major_formatter().set_useOffset(False)
	ax.get_yaxis().get_major_formatter().set_useOffset(False)
	ax.set_xlabel('Easting')
	ax.set_ylabel('Northing')
	ax.set_title('Run %d, Collar at %0.3f MHz\nUTM Zone: %d %s' % (run_num, collars[i-1]/1000000, zonenum, zone))
	plot.savefig('%s/RUN_%06d_COL_%0.3f.png'%(output_path, run_num, collars[i - 1]/1000000), bbox_inches='tight')
	print('Collar at %0.3f MHz: %s/RUN_%06d_COL_%0.3f.png'%(collars[i - 1]/1000000, output_path, run_num, collars[i - 1]/1000000))
	# plot.show(block=False)

#!/usr/bin/env python
# Altitude Filter for Radio Collar Tracker

# TODO: add run code
import sys
import shutil
if len(sys.argv) != 3:
	print "Bad args!\n"
	sys.exit(1)

tarAlt = int(sys.argv[1])

shutil.move(sys.argv[2], sys.argv[2] + ".bak")

csvFile = open(sys.argv[2] + ".bak")
csv = csvFile.readlines()
csvFile.close()
strArray = [line.rstrip().split(",") for line in csv]
array = [[int(x) for x in line] for line in strArray]

maxAlt = 0

for row in array:
	row[0] = row[0] / 10000000.0
	row[1] = row[1] / 10000000.0
	row[2] = row[2] / 1000.0
	if row[2] > maxAlt:
		maxAlt = row[2]
	for collar in range(3, len(array[0])):
		row[collar] = row[collar] / 1000.0

filteredAlt = []
minThresholdAlt = (tarAlt + array[0][2]) * 0.9
maxThresholdAlt = (tarAlt + array[0][2]) * 1.1
for row in array:
	if row[2] < maxThresholdAlt and row[2] > minThresholdAlt:
		filteredAlt.append(row)

csvFile = open(sys.argv[2], 'w')
string = ""
for row in filteredAlt:
	string += str(int(row[0] * 10000000)) + ","
	string += str(int(row[1] * 10000000)) + ","
	string += str(int(row[2] * 1000)) + ","
	for collar in range(3, len(array[0])):
		string += str(int(row[collar] * 1000)) + ","
	string = string.rstrip(",")
	string += "\n"
csvFile.write(string)
csvFile.close()




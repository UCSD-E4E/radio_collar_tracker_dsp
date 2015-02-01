#!/usr/bin/env python
# Altitude Filter for Radio Collar Tracker

# TODO: add run code
import sys
import shutil
if len(sys.argv) != 3:
	print "Bad args!\n"
	sys.exit(1)

csvFile = open(sys.argv[1])
csv = csvFile.readlines()
csvFile.close()
strArray = [line.rstrip().split(",") for line in csv]
array = [[int(x) for x in line] for line in strArray]

for row in array:
	row[0] = row[0] / 10000000.0
	row[1] = row[1] / 10000000.0
	row[2] = row[2] / 1000.0
	for collar in range(3, len(array[0])):
		row[collar] = row[collar] / 1000.0

csvFile = open(sys.argv[2], 'w')
string = ""
for row in array:
	for collar in row:
		string += str(collar) + ","
	string = string.rstrip(",")
	string += "\n"
csvFile.write(string)
csvFile.close()




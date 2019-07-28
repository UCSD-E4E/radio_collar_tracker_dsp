#!/usr/bin/env python3
import generateKML
import pos_estimate
import numpy as np
import utm

def findMax( someList ):
	tempMax = someList[0]
	for i in someList:
		if tempMax < i:
			tempMax = i
	return tempMax

def findMin( someList ):
	tempMin = someList[0]
	for i in someList:
		if tempMin > i:
			tempMin = i
	return tempMin


# data is in form [[x,y,z,rd],[x,y,z,rd],...] in utm
def generateHeatMap( data ):
	minHeatDim = [ int( min( data[:,1] ) ), int( min( data[:,0] ) ) ]
	maxHeatDim = [ int( max( data[:,1] ) ), int( max( data[:,0] ) ) ]
	heatMap = np.zeros(( maxHeatDim[0] - minHeatDim[0] + 1, \
												maxHeatDim[1] - minHeatDim[1] + 1 ))
	for x, y, z, rd in data:
		heatMap[int(y-minHeatDim[1]),int(x-minHeatDim[0])] = 1

	zonenum = data.getUTMZone[0]
	zone = data.getUTMZone[1]
	coords = [[minHeatDim[0],maxHeatDim[1]],
						[maxHeatDim[0],maxHeatDim[1]],
						[maxHeatDim[0],minHeatDim[1]],
						[minHeatDim[0],minHeatDim[1]]]

	ll = [utm.to_latlon( x[0], x[1], zonenum, zone_letter=zone ) for x in coords]
	ll = [ [x[1],x[0]] for x in ll ]
	testKML = generateKML.kmlPackage( "NOTICE", estimate, [heatMap, ll] )
	generateKML.generateKML( [ testKML ] )


#!/usr/bin/env python3
import numpy as np
import math
from scipy.optimize import least_squares
import utm

def distance( p1, p2 ):
  """ Returns the distance between two points """
  return math.sqrt( ( ( p1[0] - p2[0] ) ** 2 ) + \
                    ( ( p1[1] - p2[1] ) ** 2 ) + \
                    ( ( p1[2] - p2[2] ) ** 2 ) )

def average( listData ):
  """ Returns the average of some list """
  sumElement = 0
  for element in listData:
    sumElement += element
  return sumElement / len( listData )

def residuals( p, data ):
  result = np.zeros( len( data  ) ) 
  for i in range( len( data ) ):
    ping = data[i]
    xd = ping[0]
    yd = ping[1]
    zd = ping[2]
    rd = ping[3]
    xT = p[0]
    yT = p[1]
    r1 = p[2]
    n = p[3]

    result[i] = rd - r1 - 10  *  n * math.log10( distance( [xd, yd, zd], [xT, yT, 0] ) )

  return result


# Edit so that data is not a Data, but rather list of pings
def calculateEstimate( data, zonenum, zone, guess ):
  if len( data ) <= 5:
    xT = average( data[:,0] )
    yT = average( data[:,1] )
    k = np.max( data[:,3] )
  else:
    xT = guess[0]
    yT = guess[1]
    k = guess[2]
  n = -4
  p = [ xT, yT, k, n ]
  print( least_squares( residuals, p, kwargs={ "data":data } ) )
  return [ p[0], p[1], k ]



# IGNORE:
#test.addPing( [1,0,0,30] )
#test.addPing( [2,0,0,23.98] )
#test.addPing( [3,0,0,20.46] )
#test.addPing( [4,0,0,17.96] )

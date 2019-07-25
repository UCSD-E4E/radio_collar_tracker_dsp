#!/usr/bin/env python3

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

def main():
  # create a point.kml file if one doesn't exist
  if findFile( "point.kml", "." ) is None:
    open('point.kml', 'a').close()

  isWindows = ( platform.system() == 'Windows' )
  parser = argparse.ArgumentParser("Radio Telemetry Tracker Payload Receiver")
  args = parser.parse_args()
  UDP_PORT = 9000
  BUFFER_LEN = 1024
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

  # Checks if we're on a windows machine
  if isWindows:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


  sock.bind(("", UDP_PORT))

  pings = []
  guess = [0,0,0]

  while True:
    data, addr = sock.recvfrom(BUFFER_LEN)
    print(data)
    packet = json.loads(data)
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

if __name__ == '__main__':
  main()

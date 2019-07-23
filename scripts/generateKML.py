#!/usr/bin/env python3
import simplekml
import numpy as np
import utm
import math
import os
import platform
from PIL import Image


def find( startingPath, name ):
  """
  This function finds a file called "name" starting from startingPath

  name - The file to find
  startingPath - The starting path of where your search stars

  Returns the relative path to the file, else None
  """
  assert( isinstance( startingPath, str ) )
  assert( isinstance( name, str ) )
  # Go through each file from your starting path
  for root, directories, files in os.walk( startingPath ):
    # If the name matches the file, return the relative path
    if name in files:
      return os.path.join( root, name )
  return None


class kmlPackage:
  """
  label - a label for estimate and overlay
  estimate - a coord in the form [lon, lat] 
  overlay - Optional photo overlay. Each overlay should be a list
            of 2 lists:
              1. list of pixels for image with values of 0-1 with 1 being
                 most heat
              2. list of coordinates to represent corners of overlay
  """
  def __init__( self, label, estimate, overlay ):
    # Validate arguments
    assert( isinstance( label, str ) )
    assert( len( estimate ) == 2 )
    for coord in estimate:
      assert( isinstance( coord, float ) )
    assert( isinstance( overlay, list ) or overlay is None )
    if overlay is not None:
      assert( len( overlay ) == 2 )
      assert( overlay[0] is not None )
      assert( overlay[1] is not None )
      for corner in overlay[1]:
        assert( isinstance( corner, list ) )
        assert( len( corner ) == 2 )

    self.label = label
    self.estimate = estimate
    self.overlay = overlay

  def getLabel( self ):
    return self.label

  def getEstimate( self ):
    return self.estimate

  def getOverlay( self ):
    return self.overlay

  def setLabel( self, label ):
    self.label = label

  def setEstimate( self, estimate ):
    self.estimate = estimate

  def setOverlay( self, overlay ):
    self.overlay = overlay





def generateKML( packages ):
  """
  Generates a KML file with a point and optional polygon.
  Note: all coordinates should have the form [lon,lat]

  Parameters:
    packages - A list of kmlPackage
  """
  # Generate a point
  kml = simplekml.Kml()

  for package in packages:
    # Note: newpoint coords take in [long,lat]
    temp = package.getLabel() + " Estimate"
    kml.newpoint( name=temp, \
                  coords=[(package.estimate[0],package.estimate[1])] )
    print( "package.estimate is " )
    print( package.estimate )

    # Generate an overlay if provided
    if package.overlay is not None:
      pixels = package.overlay[0]
      corners = package.overlay[1]
      # Generate a png image of the pixels given
      imgPath = convertPixelArrToPNG( pixels )
      # Create a ground overlay
      temp = package.getLabel() + " Precision"
      ground = kml.newgroundoverlay(name=temp)
      ground.icon.href = imgPath
      ground.gxlatlonquad.coords = corners

    # Save the final KML file
  kml.save( "point.kml" )

def convertPixelArrToPNG( initPixelArr ):
  """
  Creates a PNG file named "heatmap.png" from an array of pixels with values
  ranging from 0-1. This function also converts all white pixels to clear.

  Parameters:
    initPixelArr = An array of float pixels with values ranging from 0-1
  """
  # creates and zeros out an array
  # Note: each element holds 4 values in RGBA format
  pixels = np.zeros((len(initPixelArr),len(initPixelArr[0]),4), dtype=np.uint8)
  # Iterate through each pixel in the image
  for x in range( 0, len( initPixelArr ) ):
    for y in range( 0, len( initPixelArr[x] ) ):
      # Find the sum of the RGB values
      sumRGB = int( math.floor( initPixelArr[x,y] * 510 ) )
      # Ensure each pixel value was 0 <= x <= 1
      assert( sumRGB >= 0 and sumRGB <= 510 )
      # default rgb to 255
      red = 255
      green = 255
      blue = 255

      # generate pixel for heat map
      if sumRGB <= 255:
        red -= sumRGB
      else:
        red = 0
        sumRGB -= 255
        green -= sumRGB
      pixels[x,y] = (red,green,blue,255)
  img = Image.fromarray( pixels, 'RGBA' )

  # Convert all white space to clear
  newData = []
  for coord in img.getdata():
      if coord[0] == 255 and coord[1] == 255 and coord[2] == 255:
          newData.append((255, 255, 255, 0))
      else:
          newData.append(coord)

  img.putdata(newData)
  pngNum = 0
  imgPath = "heat_map_" + str( pngNum ) + ".png"

  isWindows = ( platform.system() == 'Windows' )

  # If we're working on a windows machine, save the png in Temp folder
  if isWindows == True:
    user = os.getenv( 'username' )
    tempPath = 'C:\\Users\\' + user + '\\AppData\\Local\\Temp'
    while True:
      if find( tempPath, imgPath ) is not None:
        pngNum += 1
        imgPath = "heat_map_" + str( pngNum ) + ".png"
      else:
        break
    imgPath = tempPath + "\\" + imgPath

  # Assume a linux based system
  else:
    while True:
      if find( "/tmp", imgPath ) is not None:
        pngNum += 1
        imgPath = "heat_map_" + str( pngNum ) + ".png"
      else:
        break
    imgPath = "/tmp/" + imgPath

  img.save( imgPath )
  return imgPath







"""
#This is an example execution
estimate = [-33.98985,18.43348] # Estimate point
contour = [[-117.2335,32.8818], [-117.2240,32.8810], # Polygon
          [-117.2300,32.8830], [-117.2335, 32.8818]]

# Points for heatmap
arr = np.zeros((30,30))
arr[15,15] = 1
arr[15,14] = 0.5
#arr[15,13] = 0.3
#arr[14,15] = 0.8
#arr[13,14] = 0.5
#arr[18,18] = 0.1
#arr[10,11] = 0.6
#arr[5,3] = 0.3
#arr[1,1] = 0.5

arr1 = np.zeros((30,30))
#arr1[15,15] = 1
#arr1[15,14] = 0.5
#arr1[15,13] = 0.3
#arr1[14,15] = 0.8
#arr1[13,14] = 0.5
#arr1[18,18] = 0.1
#arr1[10,11] = 0.6
#arr1[5,3] = 0.3
arr[1,1] = 0.5

for i in range( 0, 29 ):
  for j in range( 0, 29 ):
    arr1[i,j] = 1

#11S
zonenum = 11
zone = 'S'
# bottom left, CCW
coords = [[477854, 3638598],
          [477954, 3638598],
          [477954, 3638698],
          [477854, 3638698]]

ll = [utm.to_latlon( x[0], x[1], zonenum, zone_letter=zone ) for x in coords]
ll = [ [x[1],x[0]] for x in ll ]

testKML = kmlPackage( "Test Label", estimate, [arr, ll] )
test1 = kmlPackage( "Label 1", estimate, [arr1, contour] )
"""

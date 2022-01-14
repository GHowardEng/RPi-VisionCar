# Generate ArUco markers from library of choice
# Grant Howard
# 2022

import cv2 
import numpy as np 
  
# Set pixel dimensions of markers
dim = (400,400)

# Number of markers to generate
numMarkers = 70

# Set flag to display each marker as it is generated
dispFlag = False

# Select set of markers (dictionary)
arucoSet = "DICT_ARUCO_ORIGINAL"

# From these options
ARUCO_DICT = {
  "DICT_4X4_50": cv2.aruco.DICT_4X4_50,
  "DICT_4X4_100": cv2.aruco.DICT_4X4_100,
  "DICT_4X4_250": cv2.aruco.DICT_4X4_250,
  "DICT_4X4_1000": cv2.aruco.DICT_4X4_1000,
  "DICT_5X5_50": cv2.aruco.DICT_5X5_50,
  "DICT_5X5_100": cv2.aruco.DICT_5X5_100,
  "DICT_5X5_250": cv2.aruco.DICT_5X5_250,
  "DICT_5X5_1000": cv2.aruco.DICT_5X5_1000,
  "DICT_6X6_50": cv2.aruco.DICT_6X6_50,
  "DICT_6X6_100": cv2.aruco.DICT_6X6_100,
  "DICT_6X6_250": cv2.aruco.DICT_6X6_250,
  "DICT_6X6_1000": cv2.aruco.DICT_6X6_1000,
  "DICT_7X7_50": cv2.aruco.DICT_7X7_50,
  "DICT_7X7_100": cv2.aruco.DICT_7X7_100,
  "DICT_7X7_250": cv2.aruco.DICT_7X7_250,
  "DICT_7X7_1000": cv2.aruco.DICT_7X7_1000,
  "DICT_ARUCO_ORIGINAL": cv2.aruco.DICT_ARUCO_ORIGINAL
}

# main
def main():

  # Lookup marker set
  _arucoDict = cv2.aruco.Dictionary_get(ARUCO_DICT[arucoSet])
   
  print("Using marker '{}' from set '{}'".format(
    markerId, arucoSet))
     
  # Create the ArUco marker
  _marker = np.zeros((dim[0], dim[1], 1), dtype="uint8")
  cv2.aruco.drawMarker(_arucoDict, markerId, dim[0], _marker, 1)
   
  # Save the ArUco tag to the current directory
  cv2.imwrite(fdir+arucoSet+"/"+fname, _marker)
  
  # Show if desired
  if(dispFlag):
      cv2.imshow("ArUco Marker", _marker)
      cv2.waitKey(0)
  
  return


# Set range of markers to generate
# And iterate through all    
for markerId in range(1,numMarkers+1):

    fileId = str(markerId)

    fname = arucoSet + fileId +".jpg"
    fdir  = "out/"
       
    if __name__ == '__main__':
      main()

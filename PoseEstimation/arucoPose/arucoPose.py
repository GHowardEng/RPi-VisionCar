# Aruco marker pose estimations
# Grant Howard
# 2022

import cv2
import numpy as np
import glob

# Define Markers

# Side length of the ArUco marker in meters
# Adjust to size of printed markers
sideL = 0.1

# Select Dictionary
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


def main():
    print("Using '{}' marker set".format(arucoSet))

    # Lookup and generate marker set
    _arucoDict   = cv2.aruco.Dictionary_get(ARUCO_DICT[arucoSet])
    _arucoParams = cv2.aruco.DetectorParameters_create()       

    # Read calibration parameters
    calFileName = 'calibration.yaml'

    calFile = cv2.FileStorage(calFileName, cv2.FILE_STORAGE_READ)

    mtx = calFile.getNode('K').mat()
    dist = calFile.getNode('D').mat()

    calFile.release()
    
    # Init stream from camera
    #vid = cv2.VideoCapture(0)
    #img = vid.read()
       
#     img = cv2.imread('')
#     imshow(img)
#     key = cv2.waitkey(1) & 0xFF
    
    # Loop through each image with wildcard
    for fname in glob.glob('ar*.jpg'):
        print(fname)
        frameIn = cv2.imread(fname)
        
        # Detect ArUco markers in the video frame
        (corners, ids, rejected) = cv2.aruco.detectMarkers(frameIn, _arucoDict, parameters=_arucoParams, cameraMatrix=mtx, distCoeff=dist)
        
        if ids is None:
            print("Nothing in frame")
        
        else:
            
            frameOut = frameIn
            
            # Overlay on image
            cv2.aruco.drawDetectedMarkers(frameOut, corners, ids)
            
            rvecs, tvecs, obj_points = cv2.aruco.estimatePoseSingleMarkers(corners, sideL, mtx, dist)
            
            for i, marker in enumerate(ids):
                cv2.aruco.drawAxis(frameOut, mtx, dist, rvecs[i], tvecs[i], 0.05)
            
            cv2.imshow('Out', frameOut)
            key = cv2.waitKey(0)
    
    # fin
    cv2.destroyAllWindows()        
    return

if __name__ == '__main__':
  main()
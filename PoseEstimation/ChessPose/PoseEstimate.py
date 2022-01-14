# Some code sourced from OpenCV Docs
# https://docs.opencv.org/4.x/d7/d53/tutorial_py_pose.html
# Grant Howard
# 2022

import numpy as np
import cv2 as cv
import glob

boardDim = (9,6)
calFileName = 'calibration.yaml'

# Add execution timing!
# For both init and tracking loop

# Read calibration parameters, yaml method
calFile = cv.FileStorage(calFileName, cv.FILE_STORAGE_READ)

mtx = calFile.getNode('K').mat()
dist = calFile.getNode('D').mat()

calFile.release()
  
# (Camera calbiration data required here, CV docs method)
# Load previously saved data
#with np.load('B.npz') as X:
#    mtx, dist, _, _ = [X[i] for i in ('mtx','dist','rvecs','tvecs')]
    
# Draw axis
def draw(img, corners, imgpts):
    # Initial point (origin?)
    # Required rounding
    corner = tuple(corners[0].ravel())   
    pt1 = (round(corner[0]), round(corner[1]))
    
    # axis point 0
    impt0 = tuple(imgpts[0].ravel())
    impt0 = (round(impt0[0]), round(impt0[1]))
    
    # axis point 1
    impt1 = tuple(imgpts[1].ravel())
    impt1 = (round(impt1[0]), round(impt1[1]))
    
    # axis point 2
    impt2 = tuple(imgpts[2].ravel())
    impt2 = (round(impt2[0]), round(impt2[1]))
    
    img = cv.line(img, pt1, impt0, (255,0,0), 5)
    img = cv.line(img, pt1, impt1, (0,255,0), 5)
    img = cv.line(img, pt1, impt2, (0,0,255), 5)
    
    return img

# Stop criteria (for estimation solution?)
criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# Init object points array
objp = np.zeros((boardDim[0]*boardDim[1],3), np.float32)
objp[:,:2] = np.mgrid[0:boardDim[0],0:boardDim[1]].T.reshape(-1,2)

axis = np.float32([[3,0,0], [0,3,0], [0,0,-3]]).reshape(-1,3)


fdir = 'srcImg/'
#fname = 'left22.jpg'
#img = cv.imread(fdir+fname)

# Loop through each image with wildcard
for fname in glob.glob('left*.jpg'):
    print(fname)
    
    # Run detection and project on image

    img = cv.imread(fname)
    gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)

    ret, corners = cv.findChessboardCorners(gray, (boardDim[0],boardDim[1]),None)                                                      

    if ret == True:   
        
        # More precise corner detection
        corners2 = cv.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
        
        # Find the rotation and translation vectors.
        ret,rvecs, tvecs = cv.solvePnP(objp, corners2, mtx, dist)
        
        # project 3D points to image plane
        imgpts, jac = cv.projectPoints(axis, rvecs, tvecs, mtx, dist)
        
        # overlay projected axes
        img = draw(img,corners2,imgpts)
        
        # display for viewing pleasure
        cv.imshow('img',img)
        k = cv.waitKey(0) & 0xFF
        
        # save if requested
        if k == ord('s'):
            cv.imwrite('outputImg/' + fname[:6]+'_overlay'+'.png', img)
            print('Saved Result')
            
    else:
        print('Nothing to see here!')
        
#fin
cv.destroyAllWindows()
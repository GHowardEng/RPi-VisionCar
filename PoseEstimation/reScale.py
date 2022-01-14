# Rescale images as needed
# Grant Howard
# 2022

import cv2 as cv

fname = 'ar6.jpg'
img = cv.imread(fname)

factor = 0.3

width = int(img.shape[1] * factor)
height = int(img.shape[0] * factor)
scaleDim = (width, height)

img = cv.resize(img, scaleDim, interpolation = cv.INTER_AREA)

cv.imshow('scaled',img)
cv.waitKey(0) & 0xFF

cv.imwrite(fname, img)
        
cv.destroyAllWindows()
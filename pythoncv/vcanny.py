# !/usr/bin/env python
import cv2
#  import matplotlib.pyplot as plt
# import numpy as np
# import glob
# import os
# from optparse import OptionParser



def CannyThreshold(lowThreshold):
        detected_edges = cv2.GaussianBlur(gray, (3, 3), 0)
        detected_edges = cv2.Canny(detected_edges, lowThreshold, lowThreshold *
                                   ratio, apertureSize=kernel_size)
        # just add some colours to edges from original image.
        dst = cv2.bitwise_and(frame, frame, mask=detected_edges)
        cv2.imshow('canny demo', dst)

lowThreshold = 0
max_lowThreshold = 100
ratio = 3
kernel_size = 3

cv2.namedWindow('canny demo')
cv2.createTrackbar('Min threshold', 'canny demo', lowThreshold,
                   max_lowThreshold, CannyThreshold)

cap = cv2.VideoCapture(0)
# if True:
while True:
    ret, frame = cap.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    CannyThreshold(0)  # initialization
    cv2.waitKey(30)

cap.release()
cv2.destroyAllWindows()

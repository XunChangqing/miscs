# !/usr/bin/env python
import cv2
#  import matplotlib.pyplot as plt
import numpy as np
import glob
import os
from optparse import OptionParser
usage = 'usage: %prog sampledir'
parser = OptionParser(usage=usage)
(options, args) = parser.parse_args()
if len(args) < 1:
    print 'no sampledir'
    exit()
sampledir = args[0]
# sampledir = 'sample7'
sampledstdir = sampledir+'dst'
if not os.path.exists(sampledstdir):
    os.makedirs(sampledstdir)

bg = None
# get the depth background
for eachbgfile in glob.glob(sampledir+'/*dbg.png'):
    # nbg = plt.imread(eachbgfile)
    nbg = np.float32(cv2.imread(eachbgfile, cv2.CV_LOAD_IMAGE_UNCHANGED))
    if bg is None:
        bg = nbg
    else:
        bgmasked = np.ma.masked_values(bg, 0.0)
        nbgmasked = np.ma.masked_values(nbg, 0.0)
        ret1masked = np.ma.minimum(bgmasked, nbgmasked)
        bgzeromasked = np.ma.masked_greater(bg, 0.0)
        ret2masked = np.ma.maximum(bgzeromasked, nbgmasked)
        bg1ret = np.ma.MaskedArray.filled(ret1masked, 0.0)
        bg2ret = np.ma.MaskedArray.filled(ret2masked, 0.0)
        bg = np.maximum(bg1ret, bg2ret)

# plt.imshow(bg)

bgthd = 0.05
frameCount = 0

# if True:
while True:
    fileName = '%05d' % frameCount

    curDepthFileName = sampledir + '/' + fileName + 'd.png'
    curColorFileName = sampledir + '/' + fileName + 'c.png'
    try:
        # curDepthMat = plt.imread(curDepthFileName)
        # curColorMat = plt.imread(curColorFileName)
        curDepthMat = np.float32(cv2.imread(curDepthFileName,
                                            cv2.CV_LOAD_IMAGE_UNCHANGED))
        curColorMat = np.float32(cv2.imread(curColorFileName,
                                            cv2.CV_LOAD_IMAGE_UNCHANGED))
    except:
        print 'end'
        break

    sub = bg-curDepthMat

    bginvalidmask = bg == 0
    curvalidmask = curDepthMat != 0
    validfgmask = curvalidmask & bginvalidmask

    fgmask = sub > (bg * bgthd)
    fgmask = fgmask & curvalidmask
    fgmask = fgmask | validfgmask

    fgmaskuint8 = np.uint8(fgmask)
    element = cv2.getStructuringElement(cv2.MORPH_CROSS, (3, 3))
    fgmaskpost = cv2.erode(fgmaskuint8, element)
    fgmaskpost = cv2.dilate(fgmaskpost, element)

    contours, hierarchy = cv2.findContours(fgmaskpost.copy(), cv2.RETR_EXTERNAL,
                                           cv2.CHAIN_APPROX_NONE)
    if len(contours) > 0:
        cntareas = [cv2.contourArea(cnt) for cnt in contours]
        maxcntidx = cntareas.index(np.max(cntareas))
        fgmaskfinal = np.zeros(fgmaskpost.shape, np.uint8)
        cv2.drawContours(fgmaskfinal, contours, maxcntidx, (255), -1)

        # plt.figure(1)
        # plt.imshow(fgmaskfinal)

        curColorMatMasked = np.ma.array(curColorMat, mask=~fgmaskfinal)
        curColorMat = np.ma.filled(curColorMatMasked, 0)

        curDepthMatMasked = np.ma.array(curDepthMat, mask=~fgmaskfinal)
        curDepthMat = np.ma.filled(curDepthMatMasked, 0)
        # plt.figure(2)
        # plt.imshow(curColorMatMasked)
        cv2.imshow('fgmaskfinal', fgmaskfinal)
        cv2.imshow('color', np.uint8(curColorMat))
        cv2.imshow('depth', np.uint16(curDepthMat))

        oMaskFileName = sampledstdir + '/' + fileName + 'm.png'
        oDepthFileName = sampledstdir + '/' + fileName + 'd.png'
        oColorFileName = sampledstdir + '/' + fileName + 'c.png'
        # plt.imsave(oMaskFileName, fgmaskfinal)
        # plt.imsave(oDepthFileName, curDepthMat)
        # plt.imsave(oColorFileName, curColorMat)
        cv2.imwrite(oMaskFileName, fgmaskfinal)
        cv2.imwrite(oDepthFileName, np.uint16(curDepthMat))
        cv2.imwrite(oColorFileName, np.uint8(curColorMat))

    frameCount += 1
    cv2.waitKey(30)

cv2.destroyAllWindows()

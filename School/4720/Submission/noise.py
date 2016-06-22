#!/usr/bin/python3.4

#Image filtering algorithms for CIS*4720 A1
#By: Michael Lang 0733368


import numpy
import imageIO as io
import math
import sys
import os
import time


def medianFilter(imgname, filtersize = 3):
    img_old = io.imread_gray(imgname)
    img = numpy.zeros(img_old.shape,dtype = numpy.int16)
    edge_h = math.floor(filtersize/2)
    edge_w = math.floor(filtersize/2)
    window = numpy.zeros(filtersize**2,dtype = numpy.int16) #Creating the window
    for x in range(edge_w,img.shape[0]-edge_w):
        for y in range(edge_h,img.shape[1]-edge_h):
            i = 0
            for fx in range(0, filtersize):
                for fy in range(0, filtersize):
                    window[i] = img_old[x+fx-edge_w][y+fy-edge_h] #Adding the neighbours to the window
                    i = i + 1
            window.sort() #Sorting the window
            img[x][y] = window[filtersize**2/2] #Finding the median value
    return img


def alphaMeanFilter(imgname, filtersize = 3, a = 2):
    img_old = io.imread_gray(imgname)
    img = numpy.zeros(img_old.shape,dtype = numpy.int16)
    edge_h = math.floor(filtersize/2)
    edge_w = math.floor(filtersize/2)
    window = numpy.zeros(filtersize**2,dtype = numpy.int16) #Creating the window
    for x in range(edge_w,img.shape[0]-edge_w):
        for y in range(edge_h,img.shape[1]-edge_h):
            i = 0
            for fx in range(0, filtersize):
                for fy in range(0, filtersize):
                    window[i] = img_old[x+fx-edge_w][y+fy-edge_h] #Adding the neighbours to the window
                    i = i + 1
            window.sort() #sorting the window
            trimmed = window[a:-a] #trimming the outliers from the window
            img[x][y] = numpy.mean(trimmed) #Calculating the mean of the window
    return img

def kuwaharaFilter(imgname, filtersize = 3):
    img_old = io.imread_gray(imgname)
    img = numpy.zeros(img_old.shape,dtype = numpy.int16)
    edge_h = math.floor(filtersize/2)
    edge_w = math.floor(filtersize/2)
    width = math.floor(filtersize/2)
    for x in range(edge_w,img.shape[0]-edge_w):
        for y in range(edge_h,img.shape[1]-edge_h):
            a = img_old[numpy.ix_(range(x-width,x+1),range(y-width,y+1))]  #Dividing the window into 4 sub windows
            b = img_old[numpy.ix_(range(x-width,x+1),range(y,y+width+1))]  
            c = img_old[numpy.ix_(range(x,x+width+1),range(y-width,y+1))]  
            d = img_old[numpy.ix_(range(x,x+width+1),range(y,y+width+1))] 

            minMatrix = min(a,b,c,d, key = lambda m: numpy.std(m)) #finding the sub window with the lowest Standard Deviation
            img[x][y] = numpy.mean(minMatrix) #Getting the mean of the subwindow

    return img



filename = sys.argv[1]

timeStart = time.time()
med = medianFilter(filename)
print("Median time = %f" % (time.time() - timeStart))

timeStart = time.time()
atrim = alphaMeanFilter(filename)
print("Alpha trimmed mean time = %f" % (time.time() - timeStart))

timeStart = time.time()
kuwa = kuwaharaFilter(filename)
print("Kuwahara time = %f" % (time.time() - timeStart))

io.imwrite_gray("smoothedkuwa.png",kuwa)
io.imwrite_gray("smoothedmedian.png",med)
io.imwrite_gray("smoothedalpha.png",atrim)

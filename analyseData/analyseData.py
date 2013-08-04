# -*- coding: utf-8 -*-
"""
Created on Sat Aug 03 19:26:47 2013

@author: Nikhil
"""
from mpl_toolkits.mplot3d import Axes3D
import json
import numpy as np
import numpy
import pylab as pl


def readFile(filename):
    data = []
    time = []
    
    reader = open(filename, 'r')
    for line in reader:
        if line.find("{") < 0:
            continue
        jsonv = line[line.find("{"):line.find("\n")]
        vals =  json.loads(jsonv)   
        data = np.append(data, vals.values()[0])
        time = np.append(time, vals.values()[1])
    return [data, time]
    
def smooth(x,window_len=11,window='hanning'):
    """smooth the data using a window with requested size.
    
    This method is based on the convolution of a scaled window with the signal.
    The signal is prepared by introducing reflected copies of the signal 
    (with the window size) in both ends so that transient parts are minimized
    in the begining and end part of the output signal.
    
    input:
        x: the input signal 
        window_len: the dimension of the smoothing window; should be an odd integer
        window: the type of window from 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'
            flat window will produce a moving average smoothing.

    output:
        the smoothed signal
        
    example:

    t=linspace(-2,2,0.1)
    x=sin(t)+randn(len(t))*0.1
    y=smooth(x)
    
    see also: 
    
    numpy.hanning, numpy.hamming, numpy.bartlett, numpy.blackman, numpy.convolve
    scipy.signal.lfilter
 
    TODO: the window parameter could be the window itself if an array instead of a string
    NOTE: length(output) != length(input), to correct this: return y[(window_len/2-1):-(window_len/2)] instead of just y.
    """

    if x.ndim != 1:
        raise ValueError, "smooth only accepts 1 dimension arrays."

    if x.size < window_len:
        raise ValueError, "Input vector needs to be bigger than window size."


    if window_len<3:
        return x


    if not window in ['flat', 'hanning', 'hamming', 'bartlett', 'blackman']:
        raise ValueError, "Window is on of 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'"


    s=numpy.r_[x[window_len-1:0:-1],x,x[-1:-window_len:-1]]
    #print(len(s))
    if window == 'flat': #moving average
        w=numpy.ones(window_len,'d')
    else:
        w=eval('numpy.'+window+'(window_len)')

    y=numpy.convolve(w/w.sum(),s,mode='valid')
    return y
    

pressureFile = "pressure.txt"#"..\\madiData\\data-r1\\pressure.txt"
temperatureFile = "temperature.txt"
imuFile = ""

[pressure, time] = readFile(pressureFile)
[temperature, time2] = readFile(temperatureFile)
maxPressure = max(pressure)
minPressure = min(pressure)

depth = [maxPressure - x for x in pressure]

#pl.scatter(time, pressure)
##pl.scatter(time, depth)
#pl.xlabel("Time")
#pl.ylabel("Pressure")
##pl.savefig("Depth vs Time")
#pl.show()
#pl.scatter(time, (depth - np.mean(depth))/np.std(depth))
#pl.plot(time2, (temperature - np.mean(temperature))/np.std(temperature))
#pl.show()


print time

timeInter = range(int(time[0]), int(time[-1]), 100)
depthInter = np.interp(timeInter, time, depth)
tempInter = np.interp(timeInter, time2, temperature)
#pl.scatter(depthInter, tempInter)
#pl.show()
fig = pl.figure()
ax = Axes3D(fig)
length = len(timeInter)
tempInter = smooth(tempInter, window_len = 200)[0:length]
print len(tempInter)
print len(depthInter)
ax.plot(xs = timeInter, zs =depthInter, ys = tempInter)
pl.show()

print "done"



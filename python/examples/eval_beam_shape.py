import sys, os
import numpy as np

from  bkradsim.mathematics import MathVectorApproxFunction

from bkradsim.radar import RadarConfig, RadarConfigParser, ADC, Radar, Target, TargetCollection
from bkradsim.mathematics import *

"""
This sample script shows the received intensity vs offset angle for a target moving in a out of boresight in the horizontal plane. 
The radar records a range bin registry per position of target
"""

config_str = """
Frequency        10.0 #Ghz
PeakPower        10000 #W
PulseWidth       0.25 #microsec
SamplingTime     0.25 #microsec
PRT              0.13333333333 #ms
BandWidth        4.0 #Mhz
NoiseFigure      4.0 #db
DuplexSwitchTime 3.08333333 #microsec
AntennaeGain     30.0 #db
AzBeamWidth      2.0 #deg
ElBeamWidth      40.0 #deg

Azimuth          90.0 #deg

ADCResolution    16 #bit
ADCMode          Power
ADCMin2Noise     0.0002 #unit
"""

config = RadarConfigParser().parse_string(config_str)

base = 10000.0 #target distance to radar

#Radar setup
radar = Radar( config )
radar.add_noise = False
avg_noise = radar.avg_noise

angles = np.arange(-2.5, 2.5, 0.01)  #deg, x-axis of figure
a_rad = angles * np.pi / 180.0 #rad, a 'angles' but in rads
num = len(angles)                    # number of elements along x-axis
result = np.zeros(num)               #unit, resultant measured intensity

#target setup
time = np.arange(0.0, num)    #s, time parameter used creatinh a target pth
pos  = np.zeros([num, 3])     #m, matrix shwoing the 3d cartesian position of the target
pos[:, 0] = base * np.cos(a_rad).T
pos[:, 1] = base * np.sin(a_rad).T
path = MathVectorApproxFunction(time, pos)  #Function describing the movement of the target
collection = TargetCollection();
collection.append_copy( Target(path, 1.0) )


signal_strength = 40 * avg_noise #W

for i, t in enumerate(time):
    radar.reset(t)
    reg = radar.generate_pulse_data(collection, True, signal_strength).get_copy_registry()
    result[i] = reg[253]   #distance 10km corresponds to rb 253 using standard radar config. 

import matplotlib.pyplot as plt
plt.plot(angles, result, linewidth=2.0, color = (0.0, 0.0, 0.0))
plt.xlabel('Deviation (degrees)', fontsize=16)
plt.ylabel('Intensity (unit int)', fontsize=16)
plt.tick_params(labelsize=16)

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
    plt.show()

print("Finished")

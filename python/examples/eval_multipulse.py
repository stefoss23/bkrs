import sys, os
import numpy as np

from bkradsim.radar import RadarConfigParser, Radar, ADC, Target, TargetCollection
from bkradsim.mathematics import *

#Input:
target_distance = 50000.0 #m
pulse_period    = 2

"""
This test is created to evaluate rads responce to unambuguous range targets.

Initializes a radar(Standard) with fixed Pulse Repetion Period and direction.

The unambiguous range is 20 km. 
A target is also initialized at a certain distance. 

Input: 
- target distance
- pulse period. 

The output is the range bin registry for the specific pulse period.
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
ADCMin2Noise     0.002 #unit
"""

config = RadarConfigParser().parse_string(config_str)

radar = Radar(config)

radar.add_noise = False
avg_noise = radar.avg_noise #W

collection = TargetCollection()
collection.append_copy( Target(np.array([target_distance, 0, 0]), 1.0) )

target_signal = 40 * avg_noise #W

for p in range(pulse_period):
  radar.generate_pulse_data(collection)

data = radar.generate_pulse_data(collection, True, target_signal).get_copy_registry()

NumBars = len(data)

print("Number of bins: %d" % NumBars)

x = np.zeros(NumBars)

min = radar.minimum_range #m
max = radar.instrumented_range #m
dr  = radar.range_bin #m

SamplingTime = radar.sampling_time #s
MinTime      = radar.minimum_receive_time #s

print("SamplingTime (microsec):", SamplingTime * 1e6)
print("MinTime      (microsec):", MinTime * 1e6)

y = MinTime + np.arange(0, NumBars) * SamplingTime

y /= 1e-6

import matplotlib.pyplot as plt
plt.bar(y, data, 4.0, color=(0.0, 0.0, 0.0))
plt.xlabel('Time (microsec)', fontsize=16)
plt.ylabel('Output', fontsize=16)
plt.xlim(0, 133)
plt.tick_params(labelsize=16)

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
   plt.show()


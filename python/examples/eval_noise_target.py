import sys, os
import numpy as np

from bkradsim.radar import RadarConfigParser, Radar, ADC, Target, TargetCollection
from bkradsim.mathematics import *

"""
Output is a histogram showing signal as function of time after emission. A signal from a target should appear, with higher strength than the noise.
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
ADCMin2Noise     0.02 #unit
"""

config = RadarConfigParser().parse_string(config_str)

radar = Radar(config)

avg_noise = radar.avg_noise #W

signal_strength = 100 * avg_noise #W

collection = TargetCollection()
collection.append_copy( Target( np.array([10021.0, 0, 0]), 1.0) )

data = radar.generate_pulse_data(collection, True, signal_strength).get_copy_registry()

NumBars = len(data)

print("Number og bins: %d" % NumBars)
print("Avg noise level (1e-14W): %f" % (avg_noise * 1e14))

x = np.zeros(NumBars)

min = radar.minimum_range
max = radar.instrumented_range
dr  = radar.range_bin

sampling_time = radar.sampling_time
min_time      = radar.minimum_receive_time

print("SamplingTime:", sampling_time)
print("MinTime     :", min_time)

y = min_time + sampling_time * np.arange(0, NumBars)

y /= 1e-6

import matplotlib.pyplot as plt
plt.bar(y, data, 0.2, color=(0.0, 0.0, 0.0))
plt.xlabel('Time (microsec)', fontsize=16)
plt.ylabel('Output', fontsize=16)
plt.tick_params(labelsize=16)

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
  plt.show()


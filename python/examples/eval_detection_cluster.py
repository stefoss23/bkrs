import sys
import numpy as np

from bkradsim.radar import RadarConfig, RadarConfigParser, Radar, Target, TargetCollection

config_str = """
#This is a radar config for a general navigation radar
#Receives detections up to 60 km. 

Frequency        9.4 #Ghz
PeakPower        10000 #W
PulseWidth       0.25 #microsec
PRT              2.0 #ms
MaxReceiveTime   0.4 #ms
BandWidth        4.0 #Mhz
NoiseFigure      4.0 #db
DuplexSwitchTime 1.0 #microsec
AntennaeGain     30.0 #db
AzBeamWidth      2.0 #deg
ElBeamWidth      40.0 #deg
AntRotationSpeed 180.0 #deg/s, clockwise rotation
Azimuth          87.4 #deg

ADCResolution    14 #bit
ADCMode          Power
ADCMin2Noise     0.1 #unit
"""

config = RadarConfigParser().parse_string(config_str)
radar = Radar(config)

targets = TargetCollection()
targets.append_copy( Target(np.array([5000, 0, 0]), 2.0 ) )

rotation = 5.2 #deg
tmax = rotation / 180.0; #s

pulses = []
while (radar.current_time < tmax):
  pulse = radar.generate_pulse_data(targets)
  pulses.append(pulse)

num_pulses = len(pulses)
num_bins = len(pulses[0])
signal_mtrix = np.zeros([num_pulses, num_bins])
for i, pulse in enumerate(pulses):
  signal_mtrix[i, :] = pulse.get_copy_registry()

import matplotlib.pyplot as plt
bin_start = 254
bin_end = 263
x_axis = np.arange(bin_start, bin_end, 1)
y_axis = np.arange(0, num_pulses, 1)
m_test = len(x_axis) * len(y_axis)
    
X, Y = np.meshgrid(x_axis, y_axis)

plt.pcolormesh(X, Y, signal_mtrix[:, bin_start : bin_end], cmap='Greys')
plt.colorbar()
plt.grid()

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
   plt.show()
  



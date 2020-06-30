"""
This produces of signal measured received signal strength of a target moving into an out of boresight.
The ADC measures power levels. 
Radar pointing in x direction
Target moves in a demicircle
"""

import sys, os
import numpy as np

from bkradsim.mathematics import MathVectorApproxFunction
from bkradsim.radar import RadarConfig, Radar, RadarConfigParser, Target, TargetCollection

#deg_to_rad
dtr = np.pi / 180.0

setup_file = "../python_setup.conf"
with open(setup_file, "r") as f:
   config_file = os.path.join(f.readline().rstrip('\r\n'), "radar_configs/short_range_radar.txt")
config = RadarConfigParser().parse_file(config_file)

#initiating radar from standard config
radar = Radar( config )
radar.add_noise = False
avg_noise = radar.avg_noise #W

#The distance of the target will correspond to range bin #253
base_distance = 9987.5

#creating path in demi-circle
#time
dt = 0.001 #s
tmax = 0.5 #s
t = np.arange(0, tmax, dt) #s
num_entries = len(t)

#angle
theta_min = -3.0 * dtr #rad
theta_max =  3.0 * dtr #rad
theta = theta_min + (theta_max - theta_min) / tmax * t #s

#position matrix
position = np.zeros([num_entries, 3])
position[:, 0] = base_distance * np.cos(theta)
position[:, 1] = base_distance * np.sin(theta)
position[:, 2] = 0

path = MathVectorApproxFunction(t, position) #[m m, m] = func(s)

collection = TargetCollection()
collection.append_copy( Target(path, 1.0) )
signal_strength = 40 * avg_noise #W


#simulating radar signals
container = []
while (radar.current_time < tmax):
  container.append( radar.generate_pulse_data(collection, True, signal_strength) )

#extracting plot data
time_vector = np.zeros( len(container) )
data = np.zeros( len(container) )
for i, pulse_data in enumerate(container):
  time_vector[i] = pulse_data.time
  data[i] = pulse_data.get_copy_registry()[253]


import matplotlib.pyplot as plt
theta_axis = ( theta_min + (theta_max - theta_min) / tmax * time_vector ) / dtr
plt.plot(theta_axis, data, linewidth=3.0, color=(0.8, 0.0, 0.0))
plt.xlabel('Deviation (degrees)', fontsize=16)
plt.ylabel('Received Signal (unit)', fontsize=16)
plt.tick_params(labelsize=15)
plt.grid()

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
  plt.show()








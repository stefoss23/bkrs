import sys, os
import numpy as np

from bkradsim.radar import RadarConfigParser, Radar
from bkradsim.mathematics import *

"""
The output is a histogram showing signal noise as function of distance. 
"""

setup_file = "../python_setup.conf"
with open(setup_file, "r") as f:
   config_file = os.path.join(f.readline().rstrip('\r\n'), "radar_configs/short_range_radar.txt")
config = RadarConfigParser().parse_file(config_file)

radar = Radar(config)

data = radar.generate_pulse_data().get_copy_registry()

AvgNoise = radar.avg_noise

NumBars = len(data)

print("Number of bins: %d" % NumBars)
print("Avg noise level (1e-14W): %f" % (AvgNoise * 1e14))

x = np.arange(NumBars)

min = radar.minimum_range  #m
max = radar.instrumented_range  #m
dr  = radar.range_bin  #m

x = min + dr * np.arange(0, NumBars)

import matplotlib.pyplot as plt
plt.bar(x, data, 25)

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
  plt.show()


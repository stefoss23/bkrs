"""
This returns the result of a incoming square pulse filtered by the radar. 
"""
import sys, os
import numpy as np

from bkradsim.mathematics import ComplexApproxFunction
from bkradsim.radar import RadarConfigParser, Radar

setup_file = "../python_setup.conf"
with open(setup_file, "r") as f:
   config_file = os.path.join(f.readline().rstrip('\r\n'), "radar_configs/short_range_radar.txt")
config = RadarConfigParser().parse_file(config_file)

R = Radar(config)

P = R.get_filtered_pulse()

import matplotlib.pyplot as plt
plt.plot(P.entry / 1e-6, np.abs(P.value))

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
   plt.show()


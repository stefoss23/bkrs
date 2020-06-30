import sys
import numpy as np

import matplotlib.pyplot as plt

from bkradsim.radar import *

print(np.version.version)

Filter = create_bandpass_filter(BandpassFilter.Standard, 1e6)

plt.plot(Filter.entry / 1e6, Filter.value)

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
   plt.show()

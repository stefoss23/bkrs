
"""
The output is a graph showing:
- An incoming squaure pulse
- Filtration due to a standard filter

The code shows the mathematical treatment of the square pulse:
1) Fourier inversion to a function F
2) F is modulated due to the bandpass filter F -> F * Filter -> HF
3) The modulated signal HF is then inverse fourier transformed into G
"""

import sys
import numpy as np

import matplotlib.pyplot as plt

from bkradsim.mathematics import ComplexApproxFunction, Fourier
from bkradsim.radar import *

pulsewidth =   1.5e-6 #s

f_base     = 1 / pulsewidth #hz
f_min      = - 15 * f_base #hz
f_max      =   15 * f_base #hz

bandwidth  = f_base #hz

#Incoming square pulse
Pulse = ComplexApproxFunction( np.array([- 0.5 * pulsewidth, 0.5 * pulsewidth]), 
                                      np.array([1.0, 1.0]), 0, 0  ) #func(s)=unit

#Fourier transform of incoming square pulse
dt      = pulsewidth / 1000 #s
df      = (f_max - f_min) / 3000 #hz
f       = np.arange(f_min, f_max, df)  #hz, frequency scale
F_pulse = Fourier.Transform(Pulse, f, dt, - 0.5 * pulsewidth, 0.5 * pulsewidth) #func(hz)=unit

#Modulation of Pulse due to bandpass filter
Filter = create_bandpass_filter(BandpassFilter.Standard, bandwidth) #func(hz)=unit
num_entries = len(f)
entry_mod = f
value_mod = np.zeros(num_entries, dtype=complex)
for i, f_current in enumerate(entry_mod):
   value_mod[i] = Filter(f_current) * F_pulse(f_current)
HF = ComplexApproxFunction(entry_mod, value_mod, 0, 0)

#Inverse Fourier transform of the modulated pulse
t_min      =  -3 * pulsewidth #s
t_max      =   3 * pulsewidth #s
t = np.arange(t_min, t_max, dt) #s, timescale
G = Fourier.Transform(HF, t, df, f_min, f_max, True)

plt.plot(t / 1e-6, Pulse(t), linewidth=2.0, color=(0.8, 0.0, 0.0))
plt.plot(t / 1e-6, abs(G(t)), '--', linewidth=2.0, color=(0.35, 0.35, 0.35))
plt.xlabel('Time (microsec)', fontsize=16)
plt.ylabel('Amplitude', fontsize=16)
plt.tick_params(length=10.0, width=2.0, labelsize=12)
plt.legend(['Before filtering', 'After filtering'], fontsize=16)

if ( len(sys.argv) < 2 or sys.argv[1] != "hide" ):
    plt.show()




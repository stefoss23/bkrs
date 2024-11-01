# BKRadSim 
Signal level radar simulator tool for tracking algorithm development

BKRadSim is a radar signal level simulation software library specifically designed
for naval radars. Its end product is the raw signal sampling after reception
and analog to digital conversion, which can then be used as input to a digital
processing algorithm designed by the user. The main motivation behind the
library is to create a tool for learning or developing tracking algorithms. It is
mainly written for C++ by with bindings to Python as well. The simulator
may be called sequentially, but also parallell to any digital processing in order
to test real time track performance.

     
<img src="https://github.com/stefoss23/librads/blob/master/latex/noise_target_3_c.png" alt="output"
 	title="measurements" width="500" height="250" />
     
*10-bit signal measurements as function of time during reception of a single pulse.*

System Requirements:
- Linux Only

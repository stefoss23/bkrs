import sys, os

import unittest
import numpy as np

import bkradsim.utils as Utils
from bkradsim.radar import RadarConfig, RadarConfigParser, ADCMode

deg_to_rad = np.pi / 180.0

class RadarConfigTester(unittest.TestCase):

    config_str = """
Frequency        10.0 #Ghz
PeakPower        10000 #W
PulseWidth       0.25 #microsec
SamplingTime     0.25 #microsec
PRT              0.13333333333 #ms
BandWidth        4.0 #Mhz
NoiseFigure      4.0 #db
DuplexSwitchTime 1.0 #microsec
AntennaeGain     30.0 #db
AzBeamWidth      2.0 #deg

Azimuth          90.0 #deg

ADCResolution    1024 #int
ADCMode          Power
ADCMin2Noise     0.01953125 #unit
"""

    @classmethod
    def setUpClass(cls):
       setup_file = "../python_setup.conf"
       with open(setup_file, "r") as f:
         cls.config_file = os.path.join(f.readline().rstrip('\r\n'), "radar_configs/short_range_radar.txt")

    def test_assert_parameters(self):
        config = RadarConfig()
        with self.assertRaises(Exception):
            config.AssertParametersSet()

    def test_parse_file(self):
        parser = RadarConfigParser()
        config = parser.parse_file(self.config_file)
        assert( Utils.complex_equal(config.max_receive_time, 0.13025e-3, 1e-3) )
        assert( Utils.complex_equal(config.pulse_width, 0.25e-6, 1e-3) )
        assert( Utils.complex_equal(config.duplex_switch_time, 3.08333333e-6, 3.08333333e-3) )
        config.assert_parameters_set()

    def test_parse_string(self):
        parser = RadarConfigParser()
        config = parser.parse_string(self.config_str)
        assert( Utils.complex_equal(config.duplex_switch_time, 1.0e-6, 1.0e-4) )

    def test_set_get_params(self):
        config = RadarConfig()
        config.frequency   = 1.0 #GHz
        config.peak_power  = 10000.0 #W
        config.pulse_width = 0.25 #microsec
        config.sampling_time = 0.25 #microsec
        config.prt           = 0.133333333 #ms
        config.bandwidth     = 4.0 #Mhz
        config.noise_figure  = 4.0 #db
        config.duplex_switch_time = 1.0 #microsec
        config.ant_gain = 30.0 #db
        config.az_beamwidth = 2.0 #deg
        config.el_beamwidth = 40.0 #deg
        config.ant_rot_speed = 360 #deg/s
        config.azimuth     = 90.0 #deg
        config.adc_resolution = 10 #bit
        config.adc_mode       = ADCMode.Power
        config.adc_min_2_noise = 0.25
        config.adc_max_2_noise = 0.50

        self.assertAlmostEqual(config.frequency, 1.0e9, places=4)
        self.assertAlmostEqual(config.peak_power, 10000.0, places=4)
        self.assertAlmostEqual(config.pulse_width, 0.25e-6, places=4)
        self.assertAlmostEqual(config.sampling_time, 0.25e-6, places=4)
        self.assertAlmostEqual(config.prt, 0.1333333e-3, places=4)
        self.assertAlmostEqual(config.bandwidth, 4.0e6, places=4)
        self.assertAlmostEqual(config.noise_figure, 10**0.4, places=4)
        self.assertAlmostEqual(config.duplex_switch_time, 1.9e-6, places=4)
        self.assertAlmostEqual(config.ant_gain, 1000.0, places=4)
        self.assertAlmostEqual(config.az_beamwidth, 2.0 * deg_to_rad, places=4)
        self.assertAlmostEqual(config.el_beamwidth, 40.0 * deg_to_rad, places=4)
        self.assertAlmostEqual(config.ant_rot_speed, - 2 * np.pi, places=4)
        self.assertAlmostEqual(config.azimuth, 0, places=4)
        assert( config.adc_resolution == 10 )
        assert( config.adc_mode == ADCMode.Power )
        assert( config.adc_min_2_noise == 0.25 )
        assert( config.adc_max_2_noise == 0.50 )


if __name__ == '__main__':
    unittest.main()


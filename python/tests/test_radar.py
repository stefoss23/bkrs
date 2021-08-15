#!/usr/bin/python3

import os
import numpy as np

from bkradsim.radar import RadarConfig, RadarConfigParser
from bkradsim.radar import Radar
from bkradsim.radar import ADC

import unittest

deg_to_rad = np.pi / 180.0

class TestStringMethods(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
       setup_file = "../python_setup.conf"
       with open(setup_file, "r") as f:
          config_file = os.path.join(f.readline().rstrip('\r\n'), "radar_configs/short_range_radar.txt")

       with open(setup_file, "r") as f:
          config_file_naval = os.path.join(f.readline().rstrip('\r\n'), "radar_configs/naval_radar.txt")

       cls.config = RadarConfigParser().parse_file(config_file)
       cls.config_naval = RadarConfigParser().parse_file(config_file_naval)

    def test_set(self):
        radar = Radar( self.config )
        radar.add_noise = False
        assert( not radar.add_noise )
        radar.use_filtered_pulse = False
        assert( not radar.use_filtered_pulse )

    def test1(self):
        R = Radar( self.config )
        adc = R.get_adc()
        self.assertEqual(1024, adc.num_levels)

    def test4(self):
        radar = Radar( self.config )
        self.assertAlmostEqual(radar.get_current_theta, 0,5 * np.pi, places = 4)
        self.assertAlmostEqual(radar.peak_power, 10000, places=4)
        self.assertAlmostEqual(radar.pulse_width, 0.25e-6, places=4)
        self.assertAlmostEqual(radar.prt, 0.133333e-3, places=4)
        radar.ant_rot_speed = 0.2
        self.assertAlmostEqual(radar.ant_rot_speed, 0.2, places=4)
        radar.initial_hor_theta = 0.4
        self.assertAlmostEqual(radar.initial_hor_theta, 0.4, places=4)
        assert(radar.minimum_range > 0)
        assert(radar.instrumented_range > 0)
        assert(radar.range_bin > 0)
        assert(radar.unambiguous_range > 0)
        self.assertAlmostEqual(radar.get_range(0), radar.minimum_range, places=4)
        self.assertAlmostEqual( radar.max_receive_time, 1.3025e-4 )
        self.assertAlmostEqual( radar.duplexer_switch_time, 3.083333e-6 )
        radar.get_filtered_pulse()
        assert( isinstance(radar.avg_noise, float) )
        assert( isinstance(radar.sampling_time, float) )
        assert( isinstance(radar.minimum_receive_time, float) )

    def test_beam_pattern(self):
        radar = Radar( self.config )
        hor_pattern = radar.get_horizontal_beam_shape()
        self.assertAlmostEqual(hor_pattern(0), 1, places=4)
        self.assertAlmostEqual(hor_pattern(1.0 * deg_to_rad), 0.5, places=4)
        el_pattern = radar.get_elevation_beam_shape()
        self.assertAlmostEqual( el_pattern(0), 1, places=4)
        self.assertAlmostEqual( el_pattern(20 * deg_to_rad), 0.5, places=4)

if __name__ == '__main__':
    unittest.main()

import os
import numpy as np
import unittest

from bkradsim.radar import Radar, RadarConfig, RadarConfigParser, Target, TargetCollection


class TestPulseData(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
     setup_file = "../python_setup.conf"
     with open(setup_file, "r") as f:
        config_file = os.path.join(f.readline().rstrip('\r\n'), "radar_configs/short_range_radar.txt")
     cls.config = RadarConfigParser().parse_file(config_file)
     cls.radar = Radar(cls.config)

     bin_indx = 190
     range_bin = cls.radar.range_bin
     distance = cls.radar.get_range(bin_indx) - 0.5 * range_bin;

     pos = np.array([distance, 0, 0]) 
     T = Target(pos, 2.0);
     cls.collection = TargetCollection();
     cls.collection.append_copy(T)


  def test_generate_null(self):

     self.radar.add_noise = False
     self.radar.add_target = True
     self.radar.use_pdf = True

     self.assertAlmostEqual( self.radar.current_time, 0, places=3 )
     assert( np.allclose(self.radar.get_current_boresight(), np.array([1, 0, 0]), rtol=1e-3) )

     data = self.radar.generate_pulse_data()
     assert( len(data) > 0 )
     assert( data.has_original() )
     self.assertAlmostEqual( data.time, 0, places=3 )
     assert( len(data.boresight) == 3 )
     reg = data.get_copy_registry()
     assert( type(reg) == np.ndarray )
     assert( type(reg[0]) == np.uint16 )
     self.assertAlmostEqual( self.radar.current_time, 0.13333333e-3, places=3 )

     data = self.radar.generate_pulse_data()
     self.assertAlmostEqual( data.time, 0.13333333e-3, places=3 )
     self.assertAlmostEqual( self.radar.current_time, 0.26666666e-3, places=3 )     
   

  def test_generate_target(self):

     self.radar.add_noise = False
     self.radar.add_target = True
     self.radar.use_pdf = True

     self.radar.reset()
     data = self.radar.generate_pulse_data(self.collection, True, 1e-5)
     self.assertAlmostEqual( data.time, 0.13333333e-3, places=3 )     
     self.assertAlmostEqual( self.radar.current_time, 0.26666666e-3, places=3 )
     reg = data.get_copy_registry();
     assert( reg[100] == 0 )
     assert( reg[190] == 1023 )
     assert( reg[300] == 0 )


  def test_generate_block_target(self):

     self.radar.add_noise = False
     self.radar.add_target = False
     self.radar.use_pdf = True

     self.radar.reset()
     data = self.radar.generate_pulse_data(self.collection, True, 1e-5)
     reg = data.get_copy_registry();
     assert( reg[100] == 0 )
     assert( reg[190] == 0 )
     assert( reg[300] == 0 )
     

  def test_generate_block_pdf(self):
     
     self.radar.add_noise = True
     self.radar.add_target = True
     self.radar.use_pdf = False 

     self.radar.reset()
     data = self.radar.generate_pulse_data(self.collection, True, 1e-5)
     reg = data.get_copy_registry();

     assert( reg[0] == reg[111] )
     assert( reg[2] == reg[222] )


if __name__ == '__main__':
    unittest.main()

     

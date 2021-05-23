#include <stdlib.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <complex>

#include <radsim/utils/utils.hpp>

#ifndef UTILS_ASSERT_HPP
#define UTILS_ASSERT_HPP


namespace radsim {

namespace Assert {

  void doubleEqual(double value, double expected, double max_rel_error, const std::string& filename, const std::string& function, int line);
  void complexEqual(std::complex<double> value, std::complex<double> expected, double max_rel_error, const std::string& filename, const std::string& function, int line);
  void intEqual(int value, int expected, const std::string& filename, const std::string& function, int line);
  void stringEqual(const std::string& value, const std::string& expected, const std::string& filename, const std::string& function, int line);
  void true_(bool value, const std::string& filename, const std::string& function, int line);
  void false_(bool value, const std::string& filename, const std::string& function, int line);

}

#ifndef assertDoubleEqual
#define assertDoubleEqual(value, expected, max_rel_error) \
{ \
  Assert::doubleEqual(value, expected, max_rel_error, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
}
#endif

#ifndef assertComplexEqual
#define assertComplexEqual(value, expected, max_rel_error) \
{ \
  Assert::complexEqual(value, expected, max_rel_error, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
}
#endif

#ifndef assertIntEqual
#define assertIntEqual(value, expected) \
{ \
  Assert::intEqual(value, expected, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
}
#endif

#ifndef assertStringEqual
#define assertStringEqual(value, expected) \
{ \
  Assert::stringEqual(value, expected, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
}
#endif

#ifndef assertTrue
#define assertTrue(value) \
{ \
  Assert::true_(value, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
}
#endif

#ifndef assertFalse
#define assertFalse(value) \
{ \
  Assert::false_(value, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
}
#endif

}




#endif

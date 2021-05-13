#include <stdlib.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <complex>

#include <radsim/utils/utils.hpp>

#ifndef UTILS_ASSERT_HPP
#define UTILS_ASSERT_HPP

namespace radsim {

class Assert {
  
  private:

  enum class ErrorType {
    DOUBLE_EQUAL,
    COMPLEX_EQUAL,
    INT_EQUAL,
    STRING_EQUAL,
    TRUE_,
    FALSE_
  };

  static std::string errorTypeString(ErrorType type)
  {
    switch(type) {
      case ErrorType::DOUBLE_EQUAL: return "ASSERT EQUAL ERROR(DOUBLE)"; break;
      case ErrorType::COMPLEX_EQUAL: return "ASSERT EQUAL ERROR(COMPLEX)"; break;
      case ErrorType::INT_EQUAL: return "ASSERT EQUAL ERROR(INT)"; break;
      case ErrorType::STRING_EQUAL: return "ASSERT EQUAL ERROR(STRING)"; break;
      case ErrorType::TRUE_: return "ASSERT TRUE ERROR"; break;
      case ErrorType::FALSE_: return "ASSERT FALSE ERROR"; break;
      default: return "DEFAULT ERROR";
    }
  }

  static void printErrorMessage(ErrorType type, const std::string& filename, const std::string& function, int line) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << errorTypeString(type) << std::endl;
    std::cout << "FILE: " << filename << std::endl;
    std::cout << "FUNCTION: " << function << std::endl;
    std::cout << "LINE: " << line << std::endl;
    std::cout << std::endl;
  }

  template <class T> 
  static void handleEqualError(T value, T expected, ErrorType type, const std::string& filename, const std::string& function, int line) {
    printErrorMessage(type, filename, function, line);
    std::cout << "VALUE: " << value << ", EXPECTED: " << expected << std::endl;
    std::cout << std::endl;
    abort();  
  }

  static void handleError(ErrorType type, const std::string& filename, const std::string& function, int line) {
    printErrorMessage(type, filename, function, line);
    abort();
  }    

  public:
  Assert() = delete;

  static void doubleEqual(double value, double expected, double max_rel_error, const std::string& filename, const std::string& function, int line) {
    if (!double_equal(value, expected, max_rel_error))
      handleEqualError<double>(value, expected, ErrorType::DOUBLE_EQUAL, filename, function, line);
  }

  static void complexEqual(std::complex<double> value, std::complex<double> expected, double max_rel_error, const std::string& filename, const std::string& function, int line) {
    if (!complex_equal(value, expected, max_rel_error))
      handleEqualError<std::complex<double>>(value, expected, ErrorType::COMPLEX_EQUAL, filename, function, line);
  }

  static void intEqual(int value, int expected, const std::string& filename, const std::string& function, int line) {
    if (value != expected)
      handleEqualError<int>(value, expected, ErrorType::INT_EQUAL, filename, function, line);
  }

  static void stringEqual(const std::string& value, const std::string& expected, const std::string& filename, const std::string& function, int line) {
    if (value != expected) {
      handleEqualError<std::string>(value, expected, ErrorType::STRING_EQUAL, filename, function, line);
    }
  }

  static void true_(bool value, const std::string& filename, const std::string& function, int line) {
    if (!value)
      handleError(ErrorType::TRUE_, filename, function, line);
  }

  static void false_(bool value, const std::string& filename, const std::string& function, int line) {
    if (value)
      handleError(ErrorType::FALSE_, filename, function, line);
  }

};


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

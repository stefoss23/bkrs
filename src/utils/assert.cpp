#include <string>

#include <radsim/utils/utils.hpp>
#include <radsim/utils/assert.hpp>


using namespace std;



namespace {

  enum class ErrorType {
    DOUBLE_EQUAL,
    COMPLEX_EQUAL,
    INT_EQUAL,
    STRING_EQUAL,
    TRUE_,
    FALSE_
  };

  std::string errorTypeString(ErrorType type)
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

  void printErrorMessage(ErrorType type, const string& filename, const string& function, int line) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << errorTypeString(type) << std::endl;
    std::cout << "FILE: " << filename << std::endl;
    std::cout << "FUNCTION: " << function << std::endl;
    std::cout << "LINE: " << line << std::endl;
    std::cout << std::endl;
  }

  template <class T> 
  void handleEqualError(T value, T expected, ErrorType type, const string& filename, const string& function, int line) {
    printErrorMessage(type, filename, function, line);
    std::cout << "VALUE: " << value << ", EXPECTED: " << expected << std::endl;
    std::cout << std::endl;
    abort();  
  }

  void handleError(ErrorType type, const string& filename, const string& function, int line) {
    printErrorMessage(type, filename, function, line);
    abort();
  }
}


namespace radsim {
namespace Assert {
  
  void doubleEqual(double value, double expected, double max_rel_error, const string& filename, const string& function, int line) {
    if (!double_equal(value, expected, max_rel_error))
      handleEqualError<double>(value, expected, ErrorType::DOUBLE_EQUAL, filename, function, line);
  }

  void complexEqual(std::complex<double> value, std::complex<double> expected, double max_rel_error, const std::string& filename, const std::string& function, int line) {
    if (!complex_equal(value, expected, max_rel_error))
      handleEqualError<std::complex<double>>(value, expected, ErrorType::COMPLEX_EQUAL, filename, function, line);
  }

  void intEqual(int value, int expected, const string& filename, const string& function, int line) {
    if (value != expected)
      handleEqualError<int>(value, expected, ErrorType::INT_EQUAL, filename, function, line);
  }

  void stringEqual(const std::string& value, const string& expected, const string& filename, const string& function, int line) {
    if (value != expected) {
      handleEqualError<std::string>(value, expected, ErrorType::STRING_EQUAL, filename, function, line);
    }
  }

  void true_(bool value, const string& filename, const string& function, int line) {
    if (!value)
      handleError(ErrorType::TRUE_, filename, function, line);
  }

  void false_(bool value, const string& filename, const string& function, int line) {
    if (value)
      handleError(ErrorType::FALSE_, filename, function, line);
  }

}
}

#include <exception>
#include <iostream>

namespace radsim {

#ifndef UTILS_TEST_UTILS_HPP
#define UTILS_TEST_UTILS_HPP

template <class Ex>
void assert_throw(void (*test_function)(void)) {
  try {
    test_function();
    std::cout << "Error: Function does not call exception." << std::endl;
    throw;
  }
  catch (Ex e) {
  }
}

}

#endif

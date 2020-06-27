#pybind11
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
set( PYBIND11_CPP_STANDARD "-std=c++14" )
add_subdirectory( pybind11 )
set(PYBIND11_PYTHON_VERSION 3.6)
#end pybind11

set(PYTHON_DIR ${CMAKE_SOURCE_DIR}/python)
set(PYTHON_BINARY_DIR ${CMAKE_BINARY_DIR}/python/cxx)
set(RDRPY_DIR ${CMAKE_SOURCE_DIR}/python/lib/site-packages/bkradsim)
set(PYTHON_TEST_DIR ${CMAKE_SOURCE_DIR}/python/tests)
set(PYTHON_EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/python/examples)

add_subdirectory(lib)
add_subdirectory(cxx)
add_subdirectory(tests)

if (ENABLE_PYTHON_EXAMPLES)
  add_subdirectory(examples)
endif()

#configure_file(python_setup.conf.in python_setup.conf)
#configure_file(python_setup.conf.in ${PYTHON_DIR}/python_setup.conf)

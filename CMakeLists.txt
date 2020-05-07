cmake_minimum_required(VERSION 3.5.1)
project(RadarSamplerSimulator)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wno-unused-parameter")

option( ENABLE_CXX_EXAMPLES     "Enable CXX examples."                           OFF)
option( ENABLE_PYTHON           "Enable python bindings."                        OFF)
option( ENABLE_PYTHON_EXAMPLES  "Enable python examples."                        OFF)
option( MEM_CHECK               "Use valgrind to check for memory errors"        OFF)

if ( ENABLE_PYTHON_EXAMPLES )
  if (NOT ENABLE_PYTHON )
    message( FATAL_ERROR " For python examples to work, ENABLE_PYTHON must be ON." )
  endif()
endif()

include_directories(include)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(TEST_DIR ${CMAKE_SOURCE_DIR}/test-data)

add_library(rads SHARED  utils/src/utils.cpp
                         utils/src/rng.cpp
                         #utils/src/timer.cpp
           
                         #mathematics/src/mathutils.cpp
                         #mathematics/src/approx_function.cpp
                         #mathematics/src/fourier.cpp
                         #mathematics/src/math_vector.cpp

                         #radar/src/target.cpp
                         #radar/src/adc.cpp
                         #radar/src/radar_config_parser.cpp
                         #radar/src/pulse_data.cpp
                         #radar/src/radar_state.cpp
                         #radar/src/radar.cpp
                         #radar/src/radar_config.cpp
                         #radar/src/bandpass_filter.cpp
                         #radar/src/beam_pattern.cpp
                         #radar/src/radar_data_queue.cpp
                         #radar/src/radar_interface.cpp
           )
target_link_libraries(rads pthread)
install(TARGETS rads DESTINATION "lib")
install(DIRECTORY include/ DESTINATION include)

#Start test setup
enable_testing()
add_subdirectory(utils)
#add_subdirectory(mathematics)
#add_subdirectory(radar)
#End test setup

#add applications
if (ENABLE_CXX_EXAMPLES)
  add_subdirectory(examples)
endif()
#End add applications

#add python packages
if (ENABLE_PYTHON)
  add_subdirectory(python)
endif()

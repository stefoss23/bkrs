cmake_minimum_required(VERSION 3.5.1)
project(RadarSamplerSimulator)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wno-unused-parameter")

option( ENABLE_CXX_EXAMPLES     "Enable CXX examples."                           ON)
option( ENABLE_PYTHON           "Enable python bindings."                        ON)
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

add_library(rads SHARED  src/utils/utils.cpp
                         src/utils/rng.cpp
                         src/utils/timer.cpp
                         src/utils/assert.cpp
           
                         src/mathematics/mathutils.cpp
                         src/mathematics/approx_function.cpp
                         src/mathematics/fourier.cpp
                         src/mathematics/math_vector.cpp

                         src/radar/target.cpp
                         src/radar/adc.cpp
                         src/radar/radar_config_parser.cpp
                         src/radar/pulse_data.cpp
                         src/radar/pulse_data_writer.cpp
                         src/radar/radar_state.cpp
                         src/radar/radar.cpp
                         src/radar/radar_config.cpp
                         src/radar/bandpass_filter.cpp
                         src/radar/beam_pattern.cpp
                         src/radar/radar_data_queue.cpp
                         src/radar/radar_interface.cpp
           )
target_link_libraries(rads pthread)
install(TARGETS rads DESTINATION "lib")
install(DIRECTORY include/ DESTINATION include)

#Start test setup
enable_testing()
add_subdirectory(tests)
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

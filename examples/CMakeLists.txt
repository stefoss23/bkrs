file(COPY ${CMAKE_SOURCE_DIR}/test-data/radar_configs/stationary_radar.txt
     DESTINATION ${CMAKE_BINARY_DIR}/examples)

file(COPY ${CMAKE_SOURCE_DIR}/test-data/radar_configs/naval_radar.txt
     DESTINATION ${CMAKE_BINARY_DIR}/examples)

add_executable(generate_pulses generate_pulses.cpp)
target_link_libraries(generate_pulses rads)
add_test(NAME example_generate_pulses COMMAND generate_pulses)

add_executable(detect_target detect_target.cpp)
target_link_libraries(detect_target rads)
add_test(NAME example_detect_target COMMAND detect_target)

add_executable(interface interface.cpp)
target_link_libraries(interface rads pthread)
add_test(NAME example_interface COMMAND interface)

add_executable(processor processor.cpp)
target_link_libraries(processor rads pthread)
add_test(NAME example_processor COMMAND processor)

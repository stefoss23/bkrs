

foreach (test   test_utils
                test_rng
                test_timer
                test_assert
    )
    add_executable(${test} utils/${test}.cpp)
    target_link_libraries(${test} rads )
    add_test(NAME ${test} COMMAND ${test})
endforeach ()




foreach (test   test_mathutils
                test_approx_function
                test_fourier
                test_math_vector
    )
    add_executable(${test} mathematics/${test}.cpp)
    target_link_libraries(${test} rads)
    if (MEM_CHECK)
      SET(test_name valgrind_${test})
      add_test(NAME ${test_name} COMMAND valgrind --leak-check=yes --error-exitcode=1 ./${test})
    else()
      add_test(NAME ${test} COMMAND ${test})
    endif()
endforeach ()





foreach (test   test_target
                test_radar
                test_radar_signal
                test_adc
                test_bandpass_filter
                test_radar_config
                test_beam_pattern
                test_radar_data_queue
                test_interface_plain
                test_pulse_data
                test_config_parser
                test_pulse_data_writer
    )
    add_executable(${test} radar/${test}.cpp)
    target_link_libraries(${test} rads)
    if (MEM_CHECK)
      SET(test_name valgrind_${test})
      add_test(NAME ${test_name} COMMAND valgrind --leak-check=yes --error-exitcode=1 ./${test} ${TEST_DIR})
    else()
      add_test(NAME ${test} COMMAND ${test} ${TEST_DIR})
    endif()
endforeach ()

#these are tests than cannot run successfully w/ valgrind
foreach (test   test_radar_exceptions
                test_interface
                test_interface_performance
    )
    add_executable(${test} radar/${test}.cpp)
    target_link_libraries(${test} rads)
    add_test(NAME ${test} COMMAND ${test} ${TEST_DIR})
endforeach ()



foreach (test   test_utils
                test_rng
                test_timer
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

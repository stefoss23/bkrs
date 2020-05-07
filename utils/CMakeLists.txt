

foreach (test   test_utils
                #test_rng
                #test_timer
    )
    add_executable(${test} test/${test}.cpp)
    target_link_libraries(${test} rads )
    add_test(NAME ${test} COMMAND ${test})
endforeach ()

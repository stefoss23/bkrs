

foreach (test   test_utils
                test_rng
                test_timer
    )
    add_executable(${test} utils/${test}.cpp)
    target_link_libraries(${test} rads )
    add_test(NAME ${test} COMMAND ${test})
endforeach ()

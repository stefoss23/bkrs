


pybind11_add_module(test_devel_python t_vectors.cpp)
set_target_properties(test_devel_python
        PROPERTIES PREFIX ""
                   SUFFIX ".so"
        )
target_link_libraries(test_devel_python PRIVATE rads)


foreach (test  #test_approx_function
               #test_radar
               #test_adc
               #test_bandpass
               #test_beampattern
               #test_fourier
               #test_target
               test_vectors
               #test_math_vector
               test_utils
               test_math_utils
               #test_radar_config
               #test_radar_generate
        )
        add_test(NAME python_${test} COMMAND python ${PYTHON_TEST_DIR}/${test}.py)
endforeach()

EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_BINARY_DIR}/python/tests/test_devel_python.so" "${PYTHON_TEST_DIR}/test_devel_python.so")

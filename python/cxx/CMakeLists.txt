
foreach (tgt    utils
                mathematics
                #radar
    )

    pybind11_add_module(${tgt}_python ${tgt}_pybind.cpp)
    set_target_properties(${tgt}_python
            PROPERTIES PREFIX ""
                   SUFFIX ".so"
            )
    target_link_libraries(${tgt}_python PRIVATE rads)


endforeach ()

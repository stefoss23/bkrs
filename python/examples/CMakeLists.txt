foreach (example  eval_beam_shape
                  eval_bandpass
                  eval_circling_target
                  eval_filtered_pulse
                  eval_multipulse
                  eval_noise_signal
                  eval_noise_target
                  eval_samplings2
                  eval_detection_cluster

        )
        add_test(NAME example_${example} COMMAND python ${PYTHON_EXAMPLE_DIR}/${example}.py hide)
endforeach()

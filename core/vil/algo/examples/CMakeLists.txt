# core/vil/algo/examples/CMakeLists.txt

ADD_EXECUTABLE( vil_algo_example1 algo_example1.cxx )
TARGET_LINK_LIBRARIES( vil_algo_example1 vil_algo vcl )

ADD_EXECUTABLE( vil_algo_example2 algo_example2.cxx )
TARGET_LINK_LIBRARIES( vil_algo_example2 vil_algo vcl )

ADD_EXECUTABLE( vil_convolve_tool vil_convolve_tool.cxx )
TARGET_LINK_LIBRARIES( vil_convolve_tool vil_algo vcl )

ADD_EXECUTABLE( vil_correlate_grad_tool vil_correlate_grad_tool.cxx )
TARGET_LINK_LIBRARIES( vil_correlate_grad_tool vil_algo vul vcl )

ADD_EXECUTABLE( vil_exp_grad_filter_tool vil_exp_grad_filter_tool.cxx )
TARGET_LINK_LIBRARIES( vil_exp_grad_filter_tool vil_algo vcl )

ADD_EXECUTABLE( vil_example_complex example_complex.cxx )
TARGET_LINK_LIBRARIES( vil_example_complex vil_algo vcl )

# core/vil/algo/examples/CMakeLists.txt

LINK_LIBRARIES( vil_algo vul vcl )

ADD_EXECUTABLE( vil_algo_example1 algo_example1.cxx )
ADD_EXECUTABLE( vil_algo_example2 algo_example2.cxx )
ADD_EXECUTABLE( vil_convolve_tool vil_convolve_tool.cxx )
ADD_EXECUTABLE( vil_correlate_grad_tool vil_correlate_grad_tool.cxx )
ADD_EXECUTABLE( vil_exp_grad_filter_tool vil_exp_grad_filter_tool.cxx )
ADD_EXECUTABLE( vil_example_complex example_complex.cxx )

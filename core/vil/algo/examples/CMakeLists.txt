# core/vil/algo/examples/CMakeLists.txt

add_executable( vil_algo_example1 algo_example1.cxx )
target_link_libraries( vil_algo_example1 vil_algo vcl )

add_executable( vil_algo_example2 algo_example2.cxx )
target_link_libraries( vil_algo_example2 vil_algo vcl )

add_executable( vil_convolve_tool vil_convolve_tool.cxx )
target_link_libraries( vil_convolve_tool vil_algo vcl )

add_executable( vil_correlate_grad_tool vil_correlate_grad_tool.cxx )
target_link_libraries( vil_correlate_grad_tool vil_algo vul vcl )

add_executable( vil_exp_grad_filter_tool vil_exp_grad_filter_tool.cxx )
target_link_libraries( vil_exp_grad_filter_tool vil_algo vcl )

add_executable( vil_example_complex example_complex.cxx )
target_link_libraries( vil_example_complex vil_algo vcl )

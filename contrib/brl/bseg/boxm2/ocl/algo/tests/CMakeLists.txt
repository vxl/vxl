

add_executable( boxm2_ocl_algo_test_all
  test_driver.cxx
  test_ocl_refine_multi_data_performance.cxx)
target_link_libraries( boxm2_ocl_algo_test_all ${VXL_LIB_PREFIX}testlib boxm2_ocl boxm2_ocl_algo boxm2_ocl_pro boxm2_ocl_utils boxm2_cpp boxm2_cpp_algo boxm2_cpp_pro boxm2_pro brdb )

# add_test( NAME boxm2_ocl_algo_test_refine_multi_data_performance COMMAND $<TARGET_FILE:boxm2_ocl_algo_test_all> test_ocl_refine_multi_data_performance)

add_executable( boxm2_ocl_algo_test_include test_include.cxx)
target_link_libraries( boxm2_ocl_algo_test_include boxm2_ocl_algo )

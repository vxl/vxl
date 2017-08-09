add_executable( bstm_multi_cpp_algo_test_all
  test_driver.cxx

 )
target_link_libraries( bstm_multi_cpp_algo_test_all ${VXL_LIB_PREFIX}testlib bstm_multi_cpp_algo bstm_multi bstm bstm_multi_io ${VXL_LIB_PREFIX}vcl)

add_executable( bstm_multi_cpp_algo_test_include test_include.cxx )
target_link_libraries( bstm_multi_cpp_algo_test_include bstm_multi_cpp_algo )

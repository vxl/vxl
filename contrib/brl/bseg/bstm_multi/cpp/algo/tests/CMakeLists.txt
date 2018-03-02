add_executable( bstm_multi_cpp_algo_test_all
  test_driver.cxx
  test_bstm_to_multi_bstm_block_function.cxx
 )
target_link_libraries( bstm_multi_cpp_algo_test_all ${VXL_LIB_PREFIX}testlib bstm_multi_cpp_algo bstm_multi bstm bstm_multi_io ${VXL_LIB_PREFIX}vcl)

if(VXL_RUN_FAILING_TESTS)
add_test( NAME bstm_multi_test_bstm_to_multi_bstm_block_function COMMAND $<TARGET_FILE:bstm_multi_cpp_algo_test_all> test_bstm_to_multi_bstm_block_function )
endif()

add_executable( bstm_multi_cpp_algo_test_include test_include.cxx )
target_link_libraries( bstm_multi_cpp_algo_test_include bstm_multi_cpp_algo )

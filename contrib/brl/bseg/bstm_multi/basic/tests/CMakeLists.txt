add_executable( bstm_multi_basic_test_all
    test_driver.cxx
    test_array_4d.cxx
 )

target_link_libraries( bstm_multi_basic_test_all ${VXL_LIB_PREFIX}testlib boxm2 boxm2_basic bstm boxm2_io bstm_io bstm_basic bstm_cpp_algo bstm_multi bstm_multi_basic)

add_test( NAME bstm_multi_basic_test_array_4d COMMAND $<TARGET_FILE:bstm_multi_basic_test_all>  test_array_4d )

add_executable( bstm_multi_basic_test_include test_include.cxx )
target_link_libraries( bstm_multi_basic_test_include bstm_multi_basic )

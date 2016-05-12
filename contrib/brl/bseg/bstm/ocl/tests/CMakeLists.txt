add_executable( bstm_ocl_test_all
  test_driver.cxx
 )

target_link_libraries( bstm_ocl_test_all ${VXL_LIB_PREFIX}testlib bstm_ocl_algo bstm_ocl ${VXL_LIB_PREFIX}vcl )

add_executable( bstm_ocl_test_include test_include.cxx )
target_link_libraries( bstm_ocl_test_include bstm_ocl )


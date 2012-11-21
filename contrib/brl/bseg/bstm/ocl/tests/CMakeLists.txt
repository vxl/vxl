ADD_EXECUTABLE( bstm_ocl_test_all
  test_driver.cxx
 )

TARGET_LINK_LIBRARIES( bstm_ocl_test_all testlib bstm_ocl_algo bstm_ocl vcl )

ADD_EXECUTABLE( bstm_ocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bstm_ocl_test_include bstm_ocl )


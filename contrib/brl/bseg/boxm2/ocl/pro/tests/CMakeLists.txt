ADD_EXECUTABLE( boxm2_ocl_pro_test_all
  test_driver.cxx
  test_batch_update.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_ocl_pro_test_all testlib boxm2_ocl_pro brdb vpgl_algo vpgl_pro vil_pro sdet vul vpl )

ADD_TEST( boxm2_ocl_pro_test_batch_update               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_batch_update)



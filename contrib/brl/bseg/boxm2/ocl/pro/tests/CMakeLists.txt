ADD_EXECUTABLE( boxm2_ocl_pro_test_all
  test_driver.cxx
  test_batch_update.cxx
  test_filter_kernel.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_ocl_pro_test_all testlib boxm2_ocl_pro brdb boct vpgl_algo vpgl_pro vil_pro sdet vnl vul vpl )

ADD_TEST( boxm2_ocl_pro_test_batch_update ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_pro_test_all  test_batch_update)
ADD_TEST( boxm2_ocl_pro_test_filter_kernel ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_pro_test_all  test_filter_kernel)

ADD_EXECUTABLE( boxm2_ocl_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_ocl_pro_test_include boxm2_ocl )

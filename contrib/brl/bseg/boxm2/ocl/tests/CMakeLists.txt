ADD_EXECUTABLE( boxm2_ocl_test_all
  test_driver.cxx
  #test_refine.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_ocl_test_all testlib boxm2_ocl brdb vpgl_algo vpgl_pro vil_pro sdet vul vpl )

ADD_TEST( boxm2_ocl_test_refine        ${EXECUTABLE_OUTPUT_PATH}/boxm2_ocl_test_all  test_refine  )

ADD_EXECUTABLE( boxm2_ocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_ocl_test_include boxm2_ocl )

#ADD_EXECUTABLE( boxm2_ocl_test_template_include test_template_include.cxx )
#TARGET_LINK_LIBRARIES( boxm2_ocl_test_template_include boxm2_ocl )

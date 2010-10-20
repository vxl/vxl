ADD_EXECUTABLE( icam_ocl_test_all
  test_driver.cxx
  test_depth_trans.cxx
  )

TARGET_LINK_LIBRARIES( icam_ocl_test_all icam_ocl testlib vnl vpgl vpgl_algo vgl_algo vil vgl vpl vul)

ADD_TEST( icam_ocl_test_depth_trans    ${EXECUTABLE_OUTPUT_PATH}/icam_ocl_test_all test_depth_trans  )

#ADD_EXECUTABLE( icam_ocl_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( icam_ocl_test_include icam_ocl)

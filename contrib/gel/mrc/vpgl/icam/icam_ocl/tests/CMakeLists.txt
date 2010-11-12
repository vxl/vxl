ADD_EXECUTABLE( icam_ocl_test_all
  test_driver.cxx
  test_depth_trans.cxx
)

TARGET_LINK_LIBRARIES( icam_ocl_test_all icam_ocl testlib vnl vpgl vpgl_algo vgl_algo vil vgl vpl vul)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../trans_parallel_transf_search.cl    ${CMAKE_CURRENT_BINARY_DIR}/trans_parallel_transf_search.cl COPYONLY )

ADD_TEST( icam_ocl_test_depth_trans    ${EXECUTABLE_OUTPUT_PATH}/icam_ocl_test_all test_depth_trans  )

ADD_EXECUTABLE( icam_ocl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( icam_ocl_test_include icam_ocl)

ADD_EXECUTABLE( boxm_test_all
  test_driver.cxx
  test_binary_io.cxx
 )

TARGET_LINK_LIBRARIES( boxm_test_all boxm boct testlib vnl vpl)

ADD_TEST( boxm_test_binary_io   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_binary_io  )

ADD_EXECUTABLE( boxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_test_include boxm)

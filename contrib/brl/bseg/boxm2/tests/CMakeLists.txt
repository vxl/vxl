ADD_EXECUTABLE( boxm2_test_all
  test_driver.cxx
  test_binary_io.cxx
  test_block_iter.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_test_all boxm2 boct testlib vnl vpgl vgl_xio vgl_algo vil vgl vpl vul)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/camera0.txt ${CMAKE_CURRENT_BINARY_DIR}/camera0.txt COPYONLY )

ADD_TEST( boxm2_test_binary_io    ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all test_binary_io  )
ADD_TEST( boxm2_test_block_iter   ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all test_block_iter  )

ADD_EXECUTABLE( boxm2_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_test_include boxm2)

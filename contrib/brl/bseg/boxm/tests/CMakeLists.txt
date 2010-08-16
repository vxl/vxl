ADD_EXECUTABLE( boxm_test_all
  test_driver.cxx
  test_binary_io.cxx
  test_block_iter.cxx
  test_region_finder.cxx
  test_cell_iterator.cxx
 )
 
SET(boxm_test_all_sources
    test_utils.h    test_utils.cxx
    )

ADD_LIBRARY(boxm_tests  ${boxm_test_all_sources})


TARGET_LINK_LIBRARIES( boxm_test_all boxm_tests boxm boct testlib vnl vpgl vgl_xio vgl_algo vil vgl vpl vul)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/camera0.txt ${CMAKE_CURRENT_BINARY_DIR}/camera0.txt COPYONLY )

ADD_TEST( boxm_test_binary_io    ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_binary_io  )
ADD_TEST( boxm_test_block_iter   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_iter  )
ADD_TEST( boxm_test_region_finder ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_region_finder )
ADD_TEST( boxm_test_cell_iterator ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_cell_iterator )

ADD_EXECUTABLE( boxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_test_include boxm)

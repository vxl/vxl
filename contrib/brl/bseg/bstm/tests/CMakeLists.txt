ADD_EXECUTABLE( bstm_test_all
  test_driver.cxx
  test_block.cxx
  test_time_tree.cxx
 )

TARGET_LINK_LIBRARIES( bstm_test_all testlib boxm2 boxm2_basic bstm boxm2_io bstm_io bstm_basic bstm_cpp_algo)

ADD_TEST( bstm_test_block      ${EXECUTABLE_OUTPUT_PATH}/bstm_test_all  test_block     )
ADD_TEST( bstm_test_time_tree  ${EXECUTABLE_OUTPUT_PATH}/bstm_test_all  test_time_tree )

ADD_EXECUTABLE( bstm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bstm_test_include bstm)

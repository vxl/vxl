ADD_EXECUTABLE( bstm_test_all
  test_driver.cxx
  test_block.cxx
  test_time_tree.cxx
 )

TARGET_LINK_LIBRARIES( bstm_test_all testlib boxm2 boxm2_basic bstm boxm2_io bstm_io bstm_basic bstm_cpp_algo)

ADD_TEST( NAME bstm_test_block COMMAND $<TARGET_FILE:bstm_test_all>  test_block     )
ADD_TEST( NAME bstm_test_time_tree COMMAND $<TARGET_FILE:bstm_test_all>  test_time_tree )

ADD_EXECUTABLE( bstm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bstm_test_include bstm)

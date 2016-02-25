add_executable( bstm_test_all
  test_driver.cxx
  test_block.cxx
  test_time_tree.cxx
 )

target_link_libraries( bstm_test_all ${VXL_LIB_PREFIX}testlib boxm2 boxm2_basic bstm boxm2_io bstm_io bstm_basic bstm_cpp_algo)

add_test( NAME bstm_test_block COMMAND $<TARGET_FILE:bstm_test_all>  test_block     )
add_test( NAME bstm_test_time_tree COMMAND $<TARGET_FILE:bstm_test_all>  test_time_tree )

add_executable( bstm_test_include test_include.cxx )
target_link_libraries( bstm_test_include bstm)

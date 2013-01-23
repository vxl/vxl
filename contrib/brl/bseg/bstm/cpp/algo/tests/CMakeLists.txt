ADD_EXECUTABLE( bstm_cpp_algo_test_all
  test_driver.cxx
  test_time_tree_ingestion.cxx
 )
TARGET_LINK_LIBRARIES( bstm_cpp_algo_test_all testlib bstm_cpp_algo bstm bstm_basic bstm_io vcl)

ADD_TEST( bstm_test_time_tree_ingestion  ${EXECUTABLE_OUTPUT_PATH}/bstm_cpp_algo_test_all  test_time_tree_ingestion  )
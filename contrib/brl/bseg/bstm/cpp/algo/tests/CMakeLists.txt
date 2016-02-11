add_executable( bstm_cpp_algo_test_all
  test_driver.cxx
  test_time_tree_ingestion.cxx
 )
target_link_libraries( bstm_cpp_algo_test_all ${VXL_LIB_PREFIX}testlib bstm_cpp_algo bstm bstm_basic bstm_io ${VXL_LIB_PREFIX}vcl)

add_test( NAME bstm_test_time_tree_ingestion COMMAND $<TARGET_FILE:bstm_cpp_algo_test_all>  test_time_tree_ingestion  )

add_executable( bstm_cpp_algo_test_include test_include.cxx )
target_link_libraries( bstm_cpp_algo_test_include bstm_cpp_algo )

add_executable( bstm_cpp_algo_test_template_include test_template_include.cxx )
target_link_libraries( bstm_cpp_algo_test_template_include bstm_cpp_algo )

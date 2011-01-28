ADD_EXECUTABLE( boxm2_cpp_algo_test_all
  test_driver.cxx
  test_merge_mixtures.cxx
 )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_all testlib boxm2_cpp_algo )

ADD_TEST( boxm2_test_merge_mixtures      ${EXECUTABLE_OUTPUT_PATH}/boxm2_cpp_algo_test_all  test_merge_mixtures  )

ADD_EXECUTABLE( boxm2_cpp_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_include boxm2_cpp_algo )

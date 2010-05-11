ADD_EXECUTABLE( boxm2_algo_rt_test_all
  test_driver.cxx
  test_bayesian_optimizer.cxx
  test_edges.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_algo_rt_test_all boxm2_algo_rt boxm2 boxm2_util boxm2_sample boct testlib vpl vul)

ADD_TEST( boxm2_test_bayesian_optimizer ${EXECUTABLE_OUTPUT_PATH}/boxm2_algo_rt_test_all test_bayesian_optimizer  )
ADD_TEST( boxm2_test_edges              ${EXECUTABLE_OUTPUT_PATH}/boxm2_algo_rt_test_all test_edges  )

ADD_EXECUTABLE( boxm2_algo_rt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_algo_rt_test_include boxm2_algo_rt)

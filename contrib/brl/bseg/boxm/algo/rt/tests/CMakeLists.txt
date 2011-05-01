ADD_EXECUTABLE( boxm_algo_rt_test_all
  test_driver.cxx
  test_bayesian_optimizer.cxx
  test_edges.cxx
  test_shadow_appearance.cxx
 )

TARGET_LINK_LIBRARIES( boxm_algo_rt_test_all boxm_algo_rt boxm boxm_util boxm_sample boct testlib vpl vul)

ADD_TEST( boxm_algo_rt_test_bayesian_optimizer ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_rt_test_all test_bayesian_optimizer  )
ADD_TEST( boxm_algo_rt_test_edges              ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_rt_test_all test_edges  )
ADD_TEST( boxm_algo_rt_test_shadow_appearance              ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_rt_test_all test_shadow_appearance  )
ADD_EXECUTABLE( boxm_algo_rt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_algo_rt_test_include boxm_algo_rt)
ADD_EXECUTABLE( boxm_algo_rt_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( boxm_algo_rt_test_template_include boxm_algo_rt)

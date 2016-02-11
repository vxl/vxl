add_executable( boxm_algo_rt_test_all
  test_driver.cxx
  test_bayesian_optimizer.cxx
  test_edges.cxx
  test_shadow_appearance.cxx
 )

target_link_libraries( boxm_algo_rt_test_all boxm_algo_rt boxm boxm_util boxm_sample boct ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul)

add_test( NAME boxm_algo_rt_test_bayesian_optimizer COMMAND $<TARGET_FILE:boxm_algo_rt_test_all> test_bayesian_optimizer  )
add_test( NAME boxm_algo_rt_test_edges COMMAND $<TARGET_FILE:boxm_algo_rt_test_all> test_edges  )
add_test( NAME boxm_algo_rt_test_shadow_appearance COMMAND $<TARGET_FILE:boxm_algo_rt_test_all> test_shadow_appearance  )
add_executable( boxm_algo_rt_test_include test_include.cxx )
target_link_libraries( boxm_algo_rt_test_include boxm_algo_rt)
add_executable( boxm_algo_rt_test_template_include test_template_include.cxx )
target_link_libraries( boxm_algo_rt_test_template_include boxm_algo_rt)

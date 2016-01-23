add_executable( boxm_sample_algo_test_all
  test_driver.cxx
  test_sigma_normalizer.cxx
 )

target_link_libraries( boxm_sample_algo_test_all boxm_sample_algo boxm_sample boxm testlib vcl vnl vnl_algo vbl bsta bsta_algo)

add_test( NAME boxm_sample_test_sigma_normalizer COMMAND $<TARGET_FILE:boxm_sample_algo_test_all> test_sigma_normalizer  )

add_executable( boxm_sample_algo_test_include test_include.cxx )
target_link_libraries( boxm_sample_algo_test_include boxm_sample_algo)

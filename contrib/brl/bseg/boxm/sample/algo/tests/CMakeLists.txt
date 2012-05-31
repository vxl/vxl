ADD_EXECUTABLE( boxm_sample_algo_test_all
  test_driver.cxx
  test_sigma_normalizer.cxx
 )

TARGET_LINK_LIBRARIES( boxm_sample_algo_test_all boxm_sample_algo boxm_sample boxm testlib vcl vnl vnl_algo vbl bsta bsta_algo)

ADD_TEST( boxm_sample_test_sigma_normalizer    ${EXECUTABLE_OUTPUT_PATH}/boxm_sample_algo_test_all test_sigma_normalizer  )

ADD_EXECUTABLE( boxm_sample_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_sample_algo_test_include boxm_sample_algo)

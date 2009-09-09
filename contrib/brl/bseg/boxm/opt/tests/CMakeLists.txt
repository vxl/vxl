ADD_EXECUTABLE( boxm_opt_test_all
  test_driver.cxx
  test_bayesian_optimizer.cxx
 )

TARGET_LINK_LIBRARIES( boxm_opt_test_all boxm_opt boxm boct testlib vnl vgl_xio vpl vul vpgl)

ADD_TEST( boxm_test_bayesian_optimizer     ${EXECUTABLE_OUTPUT_PATH}/boxm_opt_test_all test_bayesian_optimizer  )

ADD_EXECUTABLE( boxm_opt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_opt_test_include boxm_opt boxm boct)

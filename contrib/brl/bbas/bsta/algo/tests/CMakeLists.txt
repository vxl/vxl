# brl/bbas/bsta/algo/tests/CMakeLists.txt
# Tests for bsta/algo library

ADD_EXECUTABLE( bsta_algo_test_all
  test_driver.cxx
  test_fit_weibull.cxx
  test_gaussian_model.cxx
  test_mean_shift.cxx
  test_beta_updater.cxx
)

TARGET_LINK_LIBRARIES( bsta_algo_test_all bsta_algo testlib)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_2_2.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_2_2.txt COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_2_7.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_2_7.txt COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_7_2.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_7_2.txt COPYONLY )

ADD_TEST( bsta_algo_test_fit_weibull  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_fit_weibull )
ADD_TEST( bsta_algo_test_gaussian_model ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_gaussian_model )
ADD_TEST( bsta_algo_test_mean_shift  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_mean_shift )
ADD_TEST( bsta_algo_test_beta_updater  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_beta_updater )

ADD_EXECUTABLE( bsta_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bsta_algo_test_include bsta_algo )

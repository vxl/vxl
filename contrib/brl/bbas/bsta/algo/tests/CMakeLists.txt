# brl/bbas/bsta/algo/tests/CMakeLists.txt
# Tests for bsta/algo library

ADD_EXECUTABLE( bsta_algo_test_all
  test_driver.cxx
  test_fit_weibull.cxx
)

TARGET_LINK_LIBRARIES( bsta_algo_test_all bsta_algo testlib)

ADD_TEST( bsta_algo_test_fit_weibull  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_fit_weibull )

ADD_EXECUTABLE( bsta_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bsta_algo_test_include bsta_algo )

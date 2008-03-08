# brl/bbas/bsta/tests/CMakeLists.txt
# Tests for bsta library

ADD_EXECUTABLE( bsta_test_all
  test_driver.cxx
  test_gaussian_full.cxx
  test_gaussian_indep.cxx
  test_gaussian_sphere.cxx
  test_mixture.cxx
  test_bsta_histogram.cxx
  test_k_medoid.cxx
  test_otsu_threshold.cxx
)

TARGET_LINK_LIBRARIES( bsta_test_all bsta vpl vsl vbl vbl_io testlib)

ADD_TEST( bsta_test_gaussian_full           ${EXECUTABLE_OUTPUT_PATH}/bsta_test_all test_gaussian_full )
ADD_TEST( bsta_test_gaussian_indep          ${EXECUTABLE_OUTPUT_PATH}/bsta_test_all test_gaussian_indep )
ADD_TEST( bsta_test_gaussian_sphere         ${EXECUTABLE_OUTPUT_PATH}/bsta_test_all test_gaussian_sphere )
ADD_TEST( bsta_test_mixture                 ${EXECUTABLE_OUTPUT_PATH}/bsta_test_all test_mixture )
ADD_TEST( bsta_test_histogram               ${EXECUTABLE_OUTPUT_PATH}/bsta_test_all test_bsta_histogram )
ADD_TEST( bsta_test_k_medoid                ${EXECUTABLE_OUTPUT_PATH}/bsta_test_all test_k_medoid )
ADD_TEST( bsta_test_otsu_threshold          ${EXECUTABLE_OUTPUT_PATH}/bsta_test_all test_otsu_threshold )

ADD_EXECUTABLE( bsta_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bsta_test_include bsta )

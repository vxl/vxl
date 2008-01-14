# brl/bbas/bsta/tests/CMakeLists.txt
# Tests for bsta library

ADD_EXECUTABLE( bsta_test_all
  test_driver.cxx
  test_gaussian_full.cxx
  test_gaussian_indep.cxx
  test_gaussian_sphere.cxx
  test_mixture.cxx
  test_bsta_histogram.cxx
)

TARGET_LINK_LIBRARIES( bsta_test_all bsta testlib)

ADD_TEST( bsta_test_gaussian_full           bsta_test_all test_gaussian_full )
ADD_TEST( bsta_test_gaussian_indep          bsta_test_all test_gaussian_indep )
ADD_TEST( bsta_test_gaussian_sphere         bsta_test_all test_gaussian_sphere )
ADD_TEST( bsta_test_mixture                 bsta_test_all test_mixture )
ADD_TEST( bsta_test_histogram   bsta_test_all test_bsta_histogram )
ADD_EXECUTABLE( bsta_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( bsta_test_include bsta )

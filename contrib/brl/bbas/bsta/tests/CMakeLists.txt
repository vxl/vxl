# This brl/bbas/bsta/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE ( bsta_test_all
  test_driver.cxx
  test_bsta_histogram.cxx
  test_k_medoid.cxx
  test_otsu_threshold.cxx
)
TARGET_LINK_LIBRARIES( bsta_test_all bsta testlib )

ADD_TEST( bsta_test_histogram   bsta_test_all test_bsta_histogram )
ADD_TEST( bsta_k_medoid        bsta_test_all test_k_medoid )
ADD_TEST( bsta_otsu_thresold   bsta_test_all test_otsu_threshold )

#this is for test_includes
ADD_EXECUTABLE( bsta_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bsta_test_include bsta)

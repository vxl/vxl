# This brl/bbas/bsta/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE ( bsta_test_driver
  test_driver.cxx
  test_bsta_histogram.cxx
)
TARGET_LINK_LIBRARIES( bsta_test_driver bsta vnl testlib )

ADD_TEST( bsta_test_histogram   bsta_test_driver test_bsta_histogram)

#this is for test_includes
ADD_EXECUTABLE( bsta_test_include test_bsta_include.cxx)
TARGET_LINK_LIBRARIES( bsta_test_include bsta)

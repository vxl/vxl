# ./tbl/vipl/tests/CMakeLists.txt

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_EXECUTABLE( vipl_test_all
  test_driver.cxx        test_driver.h

  vipl_test_threshold.cxx
  vipl_test_histogram.cxx
  vipl_test_erode_disk.cxx
  vipl_test_gaussian_convolution.cxx
)
TARGET_LINK_LIBRARIES( vipl_test_all vipl testlib )

ADD_TEST(vipl_test_histogram            vipl_test_all vipl_test_histogram)
ADD_TEST(vipl_test_threshold            vipl_test_all vipl_test_threshold)
ADD_TEST(vipl_test_erode_disk           vipl_test_all vipl_test_erode_disk)
ADD_TEST(vipl_test_gaussian_convolution vipl_test_all vipl_test_gaussian_convolution)

ADD_EXECUTABLE( vipl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vipl_test_include vipl )

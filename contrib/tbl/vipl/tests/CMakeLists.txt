# ./tbl/vipl/tests/CMakeLists.txt

ADD_EXECUTABLE( vipl_test_driver
  test_driver.cxx        test_driver.h

  vipl_test_threshold.cxx
  vipl_test_erode_disk.cxx
  vipl_test_gaussian_convolution.cxx
)

ADD_TEST(vipl_test_threshold            vipl_test_driver vipl_test_threshold)
ADD_TEST(vipl_test_erode_disk           vipl_test_driver vipl_test_erode_disk)
#ADD_TEST(vipl_test_gaussian_convolution vipl_test_driver vipl_test_gaussian_convolution)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/tbl/vipl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

# ./tbl/vepl/tests/CMakeLists.txt

ADD_EXECUTABLE( vepl_test_driver
  test_driver.cxx

  test_threshold.cxx
  test_erode_disk.cxx
)

ADD_TEST(vepl_test_threshold  vepl_test_driver vepl_test_threshold)
ADD_TEST(vepl_test_erode_disk vepl_test_driver vepl_test_erode_disk)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/tbl/vepl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

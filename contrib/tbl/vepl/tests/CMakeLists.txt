# ./tbl/vepl/tests/CMakeLists.txt

ADD_EXECUTABLE( vepl_test_driver
  test_driver.cxx        test_driver.h

  vepl_test_threshold.cxx
  vepl_test_erode_disk.cxx
)
TARGET_LINK_LIBRARIES( vepl_test_driver vepl testlib )

ADD_TEST(vepl_test_threshold  vepl_test_driver vepl_test_threshold)
ADD_TEST(vepl_test_erode_disk vepl_test_driver vepl_test_erode_disk)

# ./tbl/vepl/tests/CMakeLists.txt

ADD_EXECUTABLE( vepl_test_threshold test_threshold.cxx )
ADD_TEST(       vepl_test_threshold vepl_test_threshold)
ADD_EXECUTABLE( vepl_test_erode_disk test_erode_disk.cxx )
ADD_TEST(       vepl_test_erode_disk vepl_test_erode_disk)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/tbl/vepl/CMakeListsLink.txt )

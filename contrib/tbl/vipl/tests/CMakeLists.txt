# ./tbl/vipl/tests/CMakeLists.txt

ADD_EXECUTABLE(vipl_test_threshold test_threshold.cxx)
ADD_TEST(      vipl_test_threshold vipl_test_threshold)
ADD_EXECUTABLE(vipl_test_erode_disk test_erode_disk.cxx)
ADD_TEST(      vipl_test_erode_disk vipl_test_erode_disk)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/tbl/vipl/CMakeListsLink.txt )

# vxl/vbl/io/tests/CMakeLists.txt

ADD_EXECUTABLE(vbl_io_test_all vbl_io_test_all.cxx)
ADD_EXECUTABLE(test_vbl_io test_vbl_io_sources)
ADD_TEST(vbl_io_test_all vbl_io_test_all)
ADD_TEST(vbl_test_vbl_io test_vbl_io)

SOURCE_FILES(test_vbl_io_sources
  test_vbl_io.cxx
  vbl_io_test_classes.h
)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

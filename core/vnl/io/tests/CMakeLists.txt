# ./vxl/vnl/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vnl_io_test_all vnl_io_test_all.cxx)
ADD_EXECUTABLE( test_vnl_io test_vnl_io.cxx)
ADD_TEST( vnl_io_test_all vnl_io_test_all)
ADD_TEST( vnl_test_vnl_io test_vnl_io)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

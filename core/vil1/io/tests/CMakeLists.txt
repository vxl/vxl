# ./vxl/vil/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_io_test_all vil_io_test_all.cxx)
ADD_EXECUTABLE( test_vil_io test_vil_io.cxx)
ADD_TEST( vil_io_test_all vil_io_test_all)
ADD_TEST( vil_test_vil_io test_vil_io)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )

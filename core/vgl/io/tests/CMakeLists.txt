#
# ./vxl/vgl/io/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )


ADD_EXECUTABLE( test_vgl_io test_vgl_io.cxx)
ADD_TEST( vgl_test_vgl_io test_vgl_io)

#
# ./vxl/vbl/io/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )

ADD_EXECUTABLE( test_vbl_io test_vbl_io.cxx)
ADD_TEST( vbl_test_vbl_io test_vbl_io)

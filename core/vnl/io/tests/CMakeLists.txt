#
# ./vxl/vnl/io/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )


ADD_EXECUTABLE( test_vnl_io test_vnl_io.cxx)
ADD_TEST( vnl_test_vnl_io test_vnl_io)

ADD_EXECUTABLE( test_sym_matrix_io test_sym_matrix_io.cxx)
ADD_TEST( vnl_test_sym_matrix_io test_sym_matrix_io)


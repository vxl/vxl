# ./vxl/vul/io/tests/CMakeLists.txt

ADD_EXECUTABLE( test_user_info_io test_user_info_io.cxx )
ADD_TEST(   vul_test_user_info_io test_user_info_io     )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/io/CMakeListsLink.txt )

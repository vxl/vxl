# ./vxl/vul/io/tests/CMakeLists.txt

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/io/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

ADD_EXECUTABLE( vul_io_test_driver
  test_driver.cxx

  test_include.cxx
  test_user_info_io.cxx
)

ADD_TEST( vul_io_test_include       vul_io_test_driver test_include )
ADD_TEST( vul_io_test_user_info_io  vul_io_test_driver test_user_info_io )

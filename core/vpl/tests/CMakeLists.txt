# ./vxl/vpl/tests/CMakeLists.txt

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

ADD_EXECUTABLE( vpl_test_driver
  test_driver.cxx

  test_include.cxx
  test_unistd.cxx
)

ADD_TEST( vpl_test_include  vpl_test_driver test_include )
ADD_TEST( vpl_test_unistd   vpl_test_driver test_unistd )

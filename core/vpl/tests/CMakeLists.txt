# ./vxl/vpl/tests/CMakeLists.txt

ADD_EXECUTABLE( vpl_test_driver
  test_driver.cxx

  test_include.cxx
  test_unistd.cxx
)
TARGET_LINK_LIBRARIES( vpl_test_driver vpl testlib vcl )

ADD_TEST( vpl_test_include  vpl_test_driver test_include )
ADD_TEST( vpl_test_unistd   vpl_test_driver test_unistd )

# ./vxl/vul/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vul_io_test_driver
  test_driver.cxx

  test_include.cxx
  test_user_info_io.cxx
)
TARGET_LINK_LIBRARIES( vul_io_test_driver  vul_io testlib )

ADD_TEST( vul_io_test_include       vul_io_test_driver test_include )
ADD_TEST( vul_io_test_user_info_io  vul_io_test_driver test_user_info_io )

# ./vxl/vil/tests/CMakeLists.txt

ADD_EXECUTABLE( vil2_test_driver
  test_driver.cxx


  test_file_format_read.cxx

)
TARGET_LINK_LIBRARIES( vil2_test_driver vil2 vpl vul testlib vcl )


ADD_TEST( vil2_test_file_format_read  vil2_test_driver test_file_format_read ${allvxl_SOURCE_DIR}/vxl/vil/tests/file_read_data )


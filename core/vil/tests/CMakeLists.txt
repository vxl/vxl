# ./vxl/vil/tests/CMakeLists.txt

ADD_EXECUTABLE( vil2_test_driver
  test_driver.cxx

  test_save_load_image.cxx
  test_file_format_read.cxx
	test_image_view.cxx
  test_bilin_interp.cxx
)
TARGET_LINK_LIBRARIES( vil2_test_driver vil2 vpl vul testlib vcl )


ADD_TEST( vil2_test_file_format_read  vil2_test_driver test_file_format_read ${allvxl_SOURCE_DIR}/vxl/vil/tests/file_read_data )
ADD_TEST( vil2_test_save_load_image   vil2_test_driver test_save_load_image )
ADD_TEST( test_image_view   vil2_test_driver test_image_view )
ADD_TEST( test_bilin_interp   vil2_test_driver test_bilin_interp )

# ./vxl/vil/tests/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

ADD_EXECUTABLE( vil_test_driver
  test_driver.cxx

  test_include.cxx
  test_flipud.cxx
  test_assign.cxx
  test_interpolate.cxx
  test_load.cxx
  test_memory_image.cxx
  test_memory_image_of.cxx
  test_save_load_image.cxx
  test_write_endian.cxx
  test_colour_space.cxx
  test_copy.cxx
  test_file_format_read.cxx
)

ADD_TEST( vil_test_include           vil_test_driver test_include )
ADD_TEST( vil_test_flipud            vil_test_driver test_flipud )
ADD_TEST( vil_test_assign            vil_test_driver test_assign )
ADD_TEST( vil_test_interpolate       vil_test_driver test_interpolate )
ADD_TEST( vil_test_load              vil_test_driver test_load )
ADD_TEST( vil_test_memory_image      vil_test_driver test_memory_image )
ADD_TEST( vil_test_memory_image_of   vil_test_driver test_memory_image_of )
ADD_TEST( vil_test_save_load_image   vil_test_driver test_save_load_image )
ADD_TEST( vil_test_write_endian      vil_test_driver test_write_endian )
ADD_TEST( vil_test_colour_space      vil_test_driver test_colour_space )
ADD_TEST( vil_test_copy              vil_test_driver test_copy ${allvxl_SOURCE_DIR}/vxl/vil/tests/square.pgm )
ADD_TEST( vil_test_file_format_read  vil_test_driver test_file_format_read ${allvxl_SOURCE_DIR}/vxl/vil/tests/file_read_data )
